"""
Advanced ML Models for Location Spoofing Detection
1. Sequential Features (temporal context)
2. Ensemble (XGBoost + LightGBM + CatBoost)
3. Deep Learning (LSTM for time series)
"""

import numpy as np
import pandas as pd
from sklearn.metrics import precision_score, recall_score, f1_score, confusion_matrix
from xgboost import XGBClassifier
from lightgbm import LGBMClassifier
from catboost import CatBoostClassifier
import os
import json
import warnings
warnings.filterwarnings('ignore')

# Deep Learning imports
try:
    import torch
    import torch.nn as nn
    from torch.utils.data import Dataset, DataLoader
    TORCH_AVAILABLE = True
except ImportError:
    TORCH_AVAILABLE = False
    print("PyTorch not available, LSTM will be skipped")


# ============================================================================
# 1. SEQUENTIAL FEATURES ENGINEERING
# ============================================================================

def add_sequential_features(df):
    """
    Add features based on previous/next events for each device.
    Creates temporal context for better detection.
    """
    print("\n[1] Adding Sequential Features...")

    features = df.copy()

    # Sort by device and timestamp
    features = features.sort_values(['device_id', 'timestamp']).reset_index(drop=True)

    # Group by device
    for device_id in features['device_id'].unique():
        device_mask = features['device_id'] == device_id
        device_indices = features[device_mask].index

        if len(device_indices) < 2:
            continue

        # Previous event features (lag features)
        features.loc[device_indices, 'prev_latitude'] = features.loc[device_indices, 'latitude'].shift(1)
        features.loc[device_indices, 'prev_longitude'] = features.loc[device_indices, 'longitude'].shift(1)
        features.loc[device_indices, 'prev_accuracy'] = features.loc[device_indices, 'accuracy'].shift(1)
        features.loc[device_indices, 'prev_speed'] = features.loc[device_indices, 'speed'].shift(1)

        # Next event features (lead features)
        features.loc[device_indices, 'next_latitude'] = features.loc[device_indices, 'latitude'].shift(-1)
        features.loc[device_indices, 'next_longitude'] = features.loc[device_indices, 'longitude'].shift(-1)

        # Calculate time delta
        features.loc[device_indices, 'time_delta'] = (
            pd.to_datetime(features.loc[device_indices, 'timestamp']).diff().dt.total_seconds()
        )

    # Calculate distances and velocities
    features['distance_from_prev'] = calculate_haversine_vectorized(
        features['prev_latitude'], features['prev_longitude'],
        features['latitude'], features['longitude']
    )

    features['distance_to_next'] = calculate_haversine_vectorized(
        features['latitude'], features['longitude'],
        features['next_latitude'], features['next_longitude']
    )

    # Velocity (km/h)
    features['velocity_from_prev'] = (features['distance_from_prev'] / (features['time_delta'] / 3600))
    features['velocity_to_next'] = (features['distance_to_next'] / (features['time_delta'].shift(-1) / 3600))

    # Acceleration (change in velocity)
    features['acceleration'] = features.groupby('device_id')['velocity_from_prev'].diff()

    # Direction change (bearing difference)
    features['bearing_change'] = features.groupby('device_id')['bearing'].diff().abs()

    # Accuracy change
    features['accuracy_change'] = features.groupby('device_id')['accuracy'].diff().abs()

    # Fill NaN with 0 for first/last events
    sequential_cols = [
        'prev_latitude', 'prev_longitude', 'prev_accuracy', 'prev_speed',
        'next_latitude', 'next_longitude', 'time_delta',
        'distance_from_prev', 'distance_to_next',
        'velocity_from_prev', 'velocity_to_next', 'acceleration',
        'bearing_change', 'accuracy_change'
    ]
    features[sequential_cols] = features[sequential_cols].fillna(0)

    # Flag extreme values
    features['extreme_velocity'] = (features['velocity_from_prev'] > 150).astype(int)
    features['sudden_stop'] = ((features['prev_speed'] > 10) & (features['speed'] < 1)).astype(int)
    features['sudden_acceleration'] = (features['acceleration'].abs() > 50).astype(int)

    print(f"  Added {len(sequential_cols) + 3} sequential features")

    return features


def calculate_haversine_vectorized(lat1, lon1, lat2, lon2):
    """Vectorized Haversine distance calculation."""
    R = 6371  # Earth radius in km

    lat1, lon1, lat2, lon2 = map(np.radians, [lat1, lon1, lat2, lon2])
    dlat = lat2 - lat1
    dlon = lon2 - lon1

    a = np.sin(dlat/2)**2 + np.cos(lat1) * np.cos(lat2) * np.sin(dlon/2)**2
    c = 2 * np.arcsin(np.sqrt(a.clip(0, 1)))

    return R * c


# ============================================================================
# 2. ENSEMBLE MODEL
# ============================================================================

class EnsembleModel:
    """Ensemble of XGBoost + LightGBM + CatBoost."""

    def __init__(self):
        print("\n[2] Initializing Ensemble Model (XGBoost + LightGBM + CatBoost)...")

        # XGBoost
        self.xgb = XGBClassifier(
            n_estimators=100,
            max_depth=6,
            learning_rate=0.1,
            scale_pos_weight=4.0,
            random_state=42,
            n_jobs=-1,
            eval_metric='logloss'
        )

        # LightGBM
        self.lgbm = LGBMClassifier(
            n_estimators=100,
            max_depth=6,
            learning_rate=0.1,
            class_weight='balanced',
            random_state=42,
            n_jobs=-1,
            verbose=-1
        )

        # CatBoost
        self.catboost = CatBoostClassifier(
            iterations=100,
            depth=6,
            learning_rate=0.1,
            scale_pos_weight=4.0,
            random_state=42,
            verbose=0
        )

        self.models = {
            'xgboost': self.xgb,
            'lightgbm': self.lgbm,
            'catboost': self.catboost
        }

        # Ensemble weights (can be optimized)
        self.weights = {'xgboost': 0.4, 'lightgbm': 0.3, 'catboost': 0.3}
        self.threshold = 0.5

    def fit(self, X_train, y_train):
        """Train all models in ensemble."""
        print("  Training XGBoost...")
        self.xgb.fit(X_train, y_train)

        print("  Training LightGBM...")
        self.lgbm.fit(X_train, y_train)

        print("  Training CatBoost...")
        self.catboost.fit(X_train, y_train)

        print("  [OK] All models trained")

    def predict_proba(self, X):
        """Weighted average of predictions."""
        predictions = {}

        for name, model in self.models.items():
            pred_proba = model.predict_proba(X)[:, 1]
            predictions[name] = pred_proba * self.weights[name]

        # Weighted ensemble
        ensemble_proba = sum(predictions.values())
        return ensemble_proba

    def predict(self, X):
        """Binary predictions."""
        proba = self.predict_proba(X)
        return (proba >= self.threshold).astype(int)

    def tune_threshold(self, X_val, y_val):
        """Tune threshold for best F1."""
        from sklearn.metrics import precision_recall_curve

        probas = self.predict_proba(X_val)
        precisions, recalls, thresholds = precision_recall_curve(y_val, probas)

        f1_scores = 2 * (precisions[:-1] * recalls[:-1]) / (precisions[:-1] + recalls[:-1] + 1e-10)
        best_idx = np.argmax(f1_scores)

        self.threshold = thresholds[best_idx]
        print(f"\n  Optimal ensemble threshold: {self.threshold:.3f}")
        print(f"  Precision: {precisions[best_idx]:.3f}")
        print(f"  Recall: {recalls[best_idx]:.3f}")
        print(f"  F1: {f1_scores[best_idx]:.3f}")


# ============================================================================
# 3. LSTM MODEL (Deep Learning)
# ============================================================================

if TORCH_AVAILABLE:
    class LocationSequenceDataset(Dataset):
        """Dataset for sequential location data."""

        def __init__(self, sequences, labels):
            self.sequences = torch.FloatTensor(sequences)
            self.labels = torch.FloatTensor(labels)

        def __len__(self):
            return len(self.labels)

        def __getitem__(self, idx):
            return self.sequences[idx], self.labels[idx]


    class LSTMSpoofDetector(nn.Module):
        """LSTM model for sequence-based spoof detection."""

        def __init__(self, input_size, hidden_size=64, num_layers=2, dropout=0.3):
            super(LSTMSpoofDetector, self).__init__()

            self.lstm = nn.LSTM(
                input_size=input_size,
                hidden_size=hidden_size,
                num_layers=num_layers,
                dropout=dropout,
                batch_first=True
            )

            self.fc1 = nn.Linear(hidden_size, 32)
            self.relu = nn.ReLU()
            self.dropout = nn.Dropout(dropout)
            self.fc2 = nn.Linear(32, 1)
            self.sigmoid = nn.Sigmoid()

        def forward(self, x):
            # LSTM
            lstm_out, (h_n, c_n) = self.lstm(x)

            # Use last hidden state
            last_hidden = h_n[-1]

            # Fully connected layers
            out = self.fc1(last_hidden)
            out = self.relu(out)
            out = self.dropout(out)
            out = self.fc2(out)
            out = self.sigmoid(out)

            return out


    class LSTMModel:
        """Wrapper for LSTM training and prediction."""

        def __init__(self, input_size, sequence_length=5):
            print(f"\n[3] Initializing LSTM Model (sequence_length={sequence_length})...")

            self.sequence_length = sequence_length
            self.input_size = input_size
            self.device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

            self.model = LSTMSpoofDetector(input_size).to(self.device)
            self.threshold = 0.5

            print(f"  Device: {self.device}")
            print(f"  Model parameters: {sum(p.numel() for p in self.model.parameters()):,}")

        def create_sequences(self, df, feature_cols):
            """Create sequences from dataframe."""
            sequences = []
            labels = []

            for device_id in df['device_id'].unique():
                device_data = df[df['device_id'] == device_id].sort_values('timestamp')

                if len(device_data) < self.sequence_length:
                    continue

                # Create sliding windows
                for i in range(len(device_data) - self.sequence_length + 1):
                    seq = device_data.iloc[i:i+self.sequence_length][feature_cols].values
                    label = device_data.iloc[i+self.sequence_length-1]['is_spoofed']

                    sequences.append(seq)
                    labels.append(label)

            return np.array(sequences), np.array(labels)

        def fit(self, X_train, y_train, epochs=20, batch_size=64):
            """Train LSTM model."""
            print(f"  Training for {epochs} epochs...")

            # Create dataset and dataloader
            dataset = LocationSequenceDataset(X_train, y_train)
            dataloader = DataLoader(dataset, batch_size=batch_size, shuffle=True)

            # Loss and optimizer
            criterion = nn.BCELoss()
            optimizer = torch.optim.Adam(self.model.parameters(), lr=0.001)

            # Training loop
            self.model.train()
            for epoch in range(epochs):
                total_loss = 0
                for sequences, labels in dataloader:
                    sequences = sequences.to(self.device)
                    labels = labels.to(self.device).unsqueeze(1)

                    # Forward pass
                    outputs = self.model(sequences)
                    loss = criterion(outputs, labels)

                    # Backward pass
                    optimizer.zero_grad()
                    loss.backward()
                    optimizer.step()

                    total_loss += loss.item()

                if (epoch + 1) % 5 == 0:
                    print(f"    Epoch {epoch+1}/{epochs}, Loss: {total_loss/len(dataloader):.4f}")

            print("  [OK] Training complete")

        def predict_proba(self, X):
            """Predict probabilities."""
            self.model.eval()

            dataset = LocationSequenceDataset(X, np.zeros(len(X)))
            dataloader = DataLoader(dataset, batch_size=64, shuffle=False)

            predictions = []
            with torch.no_grad():
                for sequences, _ in dataloader:
                    sequences = sequences.to(self.device)
                    outputs = self.model(sequences)
                    predictions.extend(outputs.cpu().numpy().flatten())

            return np.array(predictions)

        def predict(self, X):
            """Binary predictions."""
            proba = self.predict_proba(X)
            return (proba >= self.threshold).astype(int)


# ============================================================================
# FEATURE ENGINEERING
# ============================================================================

def engineer_base_features(df):
    """Base features (similar to improved model)."""
    features = df.copy()

    features['speed_kmh'] = features['speed'] * 3.6
    features['is_fast_moving'] = (features['speed'] > 20).astype(int)
    features['accuracy_perfect'] = (features['accuracy'] < 2).astype(int)
    features['low_accel_variance'] = (features['accelerometer_variance'] < 0.01).astype(int)
    features['low_gyro_variance'] = (features['gyroscope_variance'] < 0.01).astype(int)
    features['sensor_mismatch'] = (
        (features['speed'] > 1.0) &
        ((features['accelerometer_variance'] < 0.01) | (features['gyroscope_variance'] < 0.01))
    ).astype(int)
    features['sensor_combined'] = features['accelerometer_variance'] + features['gyroscope_variance']
    features['mock_location'] = features['mock_location_enabled'].astype(int)
    features['tz_expected'] = (features['longitude'] / 15).round().astype(int)
    features['tz_mismatch'] = abs(features['device_timezone_offset'] - features['tz_expected'])

    return features


def select_features(df, include_sequential=False):
    """Select features for modeling."""
    base_cols = [
        'latitude', 'longitude', 'accuracy', 'speed', 'speed_kmh',
        'accelerometer_variance', 'gyroscope_variance', 'sensor_combined',
        'is_fast_moving', 'accuracy_perfect',
        'low_accel_variance', 'low_gyro_variance', 'sensor_mismatch',
        'mock_location', 'tz_mismatch'
    ]

    if include_sequential:
        sequential_cols = [
            'distance_from_prev', 'distance_to_next',
            'velocity_from_prev', 'velocity_to_next',
            'acceleration', 'bearing_change', 'accuracy_change',
            'extreme_velocity', 'sudden_stop', 'sudden_acceleration'
        ]
        return base_cols + sequential_cols

    return base_cols


# ============================================================================
# EVALUATION
# ============================================================================

def evaluate_model(y_true, y_pred, model_name):
    """Evaluate and print metrics."""
    precision = precision_score(y_true, y_pred)
    recall = recall_score(y_true, y_pred)
    f1 = f1_score(y_true, y_pred)
    cm = confusion_matrix(y_true, y_pred)

    print(f"\n{model_name}:")
    print(f"  Precision: {precision:.3f}")
    print(f"  Recall:    {recall:.3f}")
    print(f"  F1 Score:  {f1:.3f}")
    print(f"  Confusion Matrix:")
    print(f"    TN: {cm[0][0]:5d}  FP: {cm[0][1]:5d}")
    print(f"    FN: {cm[1][0]:5d}  TP: {cm[1][1]:5d}")

    return {'precision': float(precision), 'recall': float(recall), 'f1': float(f1)}


# ============================================================================
# MAIN COMPARISON
# ============================================================================

def main():
    print("=" * 70)
    print("ADVANCED ML MODELS COMPARISON")
    print("=" * 70)

    # Load data
    script_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir = os.path.join(os.path.dirname(script_dir), 'data')

    print("\nLoading data...")
    train_df = pd.read_csv(os.path.join(data_dir, 'train.csv'))
    test_df = pd.read_csv(os.path.join(data_dir, 'test.csv'))
    test_labels = pd.read_csv(os.path.join(data_dir, 'test_labels.csv'))
    test_df = test_df.merge(test_labels, on='event_id')

    print(f"  Train: {len(train_df)} events")
    print(f"  Test: {len(test_df)} events")

    results = {}

    # ========================================================================
    # APPROACH 1: SEQUENTIAL FEATURES + XGBOOST
    # ========================================================================
    print("\n" + "=" * 70)
    print("APPROACH 1: Sequential Features + XGBoost")
    print("=" * 70)

    train_seq = add_sequential_features(train_df)
    test_seq = add_sequential_features(test_df)

    train_seq = engineer_base_features(train_seq)
    test_seq = engineer_base_features(test_seq)

    feature_cols_seq = select_features(train_seq, include_sequential=True)
    X_train_seq = train_seq[feature_cols_seq]
    y_train_seq = train_seq['is_spoofed'].astype(int)
    X_test_seq = test_seq[feature_cols_seq]
    y_test_seq = test_seq['is_spoofed'].astype(int)

    print(f"\nTraining XGBoost with {len(feature_cols_seq)} features...")
    model_seq = XGBClassifier(
        n_estimators=100,
        max_depth=6,
        learning_rate=0.1,
        scale_pos_weight=4.0,
        random_state=42,
        n_jobs=-1,
        eval_metric='logloss'
    )
    model_seq.fit(X_train_seq, y_train_seq)

    y_pred_seq = model_seq.predict(X_test_seq)
    results['sequential'] = evaluate_model(y_test_seq, y_pred_seq, "XGBoost + Sequential Features")

    # ========================================================================
    # APPROACH 2: ENSEMBLE
    # ========================================================================
    print("\n" + "=" * 70)
    print("APPROACH 2: Ensemble (XGBoost + LightGBM + CatBoost)")
    print("=" * 70)

    ensemble = EnsembleModel()
    ensemble.fit(X_train_seq, y_train_seq)
    ensemble.tune_threshold(X_train_seq, y_train_seq)

    y_pred_ensemble = ensemble.predict(X_test_seq)
    results['ensemble'] = evaluate_model(y_test_seq, y_pred_ensemble, "Ensemble Model")

    # ========================================================================
    # APPROACH 3: LSTM (if PyTorch available)
    # ========================================================================
    if TORCH_AVAILABLE:
        print("\n" + "=" * 70)
        print("APPROACH 3: LSTM (Deep Learning)")
        print("=" * 70)

        # Prepare sequences
        lstm_model = LSTMModel(input_size=len(feature_cols_seq), sequence_length=5)

        print("\nCreating sequences...")
        X_train_lstm, y_train_lstm = lstm_model.create_sequences(train_seq, feature_cols_seq)
        X_test_lstm, y_test_lstm = lstm_model.create_sequences(test_seq, feature_cols_seq)

        print(f"  Train sequences: {len(X_train_lstm)}")
        print(f"  Test sequences: {len(X_test_lstm)}")

        # Train
        lstm_model.fit(X_train_lstm, y_train_lstm, epochs=20, batch_size=64)

        # Predict
        y_pred_lstm = lstm_model.predict(X_test_lstm)
        results['lstm'] = evaluate_model(y_test_lstm, y_pred_lstm, "LSTM Model")
    else:
        print("\n[SKIPPED] LSTM - PyTorch not available")
        results['lstm'] = {'precision': 0, 'recall': 0, 'f1': 0}

    # ========================================================================
    # COMPARISON SUMMARY
    # ========================================================================
    print("\n\n" + "=" * 70)
    print("FINAL COMPARISON")
    print("=" * 70)

    print(f"\n{'Model':<35} {'Precision':>10} {'Recall':>10} {'F1':>10}")
    print("-" * 70)

    # Add baseline for comparison
    print(f"{'XGBoost (Baseline - Improved)':<35} {'0.873':>10} {'0.898':>10} {'0.886':>10}")
    print(f"{'Rules-based':<35} {'0.917':>10} {'0.834':>10} {'0.874':>10}")
    print("-" * 70)

    for name, metrics in results.items():
        display_name = {
            'sequential': 'XGBoost + Sequential Features',
            'ensemble': 'Ensemble (XGB+LGBM+Cat)',
            'lstm': 'LSTM (Deep Learning)'
        }[name]
        print(f"{display_name:<35} {metrics['precision']:>10.3f} {metrics['recall']:>10.3f} {metrics['f1']:>10.3f}")

    # Find best
    best = max(results.items(), key=lambda x: x[1]['f1'])
    print(f"\n*** BEST ADVANCED MODEL: {best[0].upper()} (F1 = {best[1]['f1']:.3f}) ***")

    # Save results
    output_path = os.path.join(data_dir, 'advanced_models_comparison.json')
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)
    print(f"\n[OK] Results saved to {output_path}")

    print("\n" + "=" * 70)
    print("Comparison complete!")
    print("=" * 70)


if __name__ == "__main__":
    main()
