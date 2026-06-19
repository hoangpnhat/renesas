"""
IMPROVED ML model for location spoofing detection.
Addresses low recall issue with better techniques.
"""

import numpy as np
import pandas as pd
from sklearn.ensemble import RandomForestClassifier
from xgboost import XGBClassifier
from sklearn.metrics import (
    precision_score, recall_score, f1_score,
    confusion_matrix, precision_recall_curve
)
import matplotlib.pyplot as plt
import json
import os


class ImprovedSpoofingMLModel:
    """Improved ML model with better recall-precision balance."""

    def __init__(self, model_type='xgboost'):
        """
        Initialize improved model.

        Args:
            model_type: 'xgboost', 'rf', or 'rf_balanced'
        """
        self.model_type = model_type

        if model_type == 'xgboost':
            # XGBoost with class weights to handle imbalance
            self.model = XGBClassifier(
                n_estimators=100,
                max_depth=6,
                learning_rate=0.1,
                scale_pos_weight=4.0,  # Handle 20% spoof rate (80/20 = 4)
                random_state=42,
                n_jobs=-1,
                eval_metric='logloss'
            )
        elif model_type == 'rf_balanced':
            # Random Forest with balanced class weights
            self.model = RandomForestClassifier(
                n_estimators=100,
                max_depth=10,
                min_samples_split=20,
                min_samples_leaf=10,
                class_weight='balanced',  # Automatically adjust weights
                random_state=42,
                n_jobs=-1
            )
        else:  # rf
            self.model = RandomForestClassifier(
                n_estimators=100,
                max_depth=10,
                min_samples_split=20,
                min_samples_leaf=10,
                random_state=42,
                n_jobs=-1
            )

        self.feature_columns = None
        self.threshold = 0.5  # Start with balanced threshold

    def engineer_features(self, df: pd.DataFrame) -> pd.DataFrame:
        """Create ENHANCED features for ML model."""
        features = df.copy()

        # Basic location features
        features['lat_rounded'] = features['latitude'].round(1)
        features['lon_rounded'] = features['longitude'].round(1)

        # Speed-related features
        features['speed_kmh'] = features['speed'] * 3.6
        features['is_stationary'] = (features['speed'] < 0.5).astype(int)
        features['is_fast_moving'] = (features['speed'] > 20).astype(int)
        features['is_very_fast'] = (features['speed'] > 40).astype(int)  # NEW

        # Accuracy features
        features['accuracy_perfect'] = (features['accuracy'] < 2).astype(int)
        features['accuracy_good'] = (features['accuracy'] < 10).astype(int)  # NEW
        features['accuracy_poor'] = (features['accuracy'] > 50).astype(int)
        features['accuracy_squared'] = features['accuracy'] ** 2  # NEW: Non-linear

        # Network features
        features['has_wifi'] = (features['wifi_count'] > 0).astype(int)
        features['network_is_wifi'] = (features['network_type'] == 'WiFi').astype(int)
        features['network_is_5g'] = (features['network_type'] == '5G').astype(int)
        features['network_is_cellular'] = features['network_type'].isin(['4G', '5G', '3G']).astype(int)  # NEW

        # Sensor features
        features['low_accel_variance'] = (features['accelerometer_variance'] < 0.01).astype(int)
        features['low_gyro_variance'] = (features['gyroscope_variance'] < 0.01).astype(int)
        features['very_low_accel'] = (features['accelerometer_variance'] < 0.005).astype(int)  # NEW
        features['very_low_gyro'] = (features['gyroscope_variance'] < 0.005).astype(int)  # NEW
        features['sensor_mismatch'] = (
            (features['speed'] > 1.0) &
            ((features['accelerometer_variance'] < 0.01) | (features['gyroscope_variance'] < 0.01))
        ).astype(int)
        # NEW: Combined sensor score
        features['sensor_combined'] = features['accelerometer_variance'] + features['gyroscope_variance']

        # Device features
        features['low_battery'] = (features['battery_level'] < 20).astype(int)
        features['dev_options_enabled'] = features['developer_options_enabled'].astype(int)

        # IP/Location features
        features['ip_prefix'] = features['ip_address'].str.split('.').str[:3].str.join('.')
        features['ip_is_west'] = features['ip_prefix'].isin(['192.168.1', '10.0.1', '172.16.1']).astype(int)
        features['ip_is_east'] = features['ip_prefix'].isin(['192.168.2', '10.0.2', '172.16.2']).astype(int)
        features['ip_is_central'] = features['ip_prefix'].isin(['192.168.3', '10.0.3', '172.16.3']).astype(int)

        # Timezone features
        features['tz_expected'] = (features['longitude'] / 15).round().astype(int)
        features['tz_mismatch'] = (features['device_timezone_offset'] - features['tz_expected']).abs()
        features['tz_mismatch_significant'] = (features['tz_mismatch'] > 2).astype(int)
        features['tz_mismatch_extreme'] = (features['tz_mismatch'] > 3).astype(int)  # NEW

        # Direct security indicators
        features['mock_location'] = features['mock_location_enabled'].astype(int)

        # NEW: Interaction features (combinations that might be suspicious)
        features['mock_and_perfect_acc'] = (features['mock_location'] & features['accuracy_perfect']).astype(int)
        features['fast_and_no_sensor'] = (features['is_fast_moving'] & features['sensor_mismatch']).astype(int)
        features['dev_and_perfect'] = (features['dev_options_enabled'] & features['accuracy_perfect']).astype(int)

        return features

    def select_features(self, df: pd.DataFrame) -> pd.DataFrame:
        """Select ENHANCED features for model training."""
        feature_cols = [
            # Location
            'latitude', 'longitude', 'accuracy', 'altitude',
            'accuracy_squared',  # NEW

            # Speed and movement
            'speed', 'speed_kmh', 'bearing',
            'is_stationary', 'is_fast_moving', 'is_very_fast',  # NEW

            # Accuracy flags
            'accuracy_perfect', 'accuracy_good', 'accuracy_poor',  # NEW

            # Network
            'wifi_count', 'cell_tower_count',
            'has_wifi', 'network_is_wifi', 'network_is_5g', 'network_is_cellular',  # NEW

            # Sensors
            'accelerometer_variance', 'gyroscope_variance',
            'low_accel_variance', 'low_gyro_variance',
            'very_low_accel', 'very_low_gyro',  # NEW
            'sensor_mismatch', 'sensor_combined',  # NEW

            # Device
            'battery_level', 'is_charging',
            'low_battery', 'dev_options_enabled',

            # IP/Geo
            'ip_is_west', 'ip_is_east', 'ip_is_central',

            # Timezone
            'device_timezone_offset', 'tz_expected',
            'tz_mismatch', 'tz_mismatch_significant', 'tz_mismatch_extreme',  # NEW

            # Security
            'mock_location',

            # Interactions (NEW)
            'mock_and_perfect_acc', 'fast_and_no_sensor', 'dev_and_perfect',
        ]

        self.feature_columns = feature_cols
        return df[feature_cols]

    def train(self, X_train: pd.DataFrame, y_train: pd.Series):
        """Train the improved ML model."""
        print(f"Training {self.model_type.upper()} model...")
        self.model.fit(X_train, y_train)
        print(f"Model trained with {len(self.feature_columns)} features")

        # Show feature importance
        if hasattr(self.model, 'feature_importances_'):
            feature_importance = pd.DataFrame({
                'feature': self.feature_columns,
                'importance': self.model.feature_importances_
            }).sort_values('importance', ascending=False)

            print("\nTop 15 most important features:")
            for idx, row in feature_importance.head(15).iterrows():
                print(f"  {row['feature']:30s}: {row['importance']:.4f}")

            return feature_importance

        return None

    def tune_threshold_for_f1(self, X_val: pd.DataFrame, y_val: pd.Series) -> float:
        """
        Tune decision threshold to maximize F1 score (balanced approach).
        """
        print(f"\nTuning threshold to maximize F1 score...")

        # Get prediction probabilities
        probas = self.model.predict_proba(X_val)[:, 1]

        # Calculate precision-recall curve
        precisions, recalls, thresholds = precision_recall_curve(y_val, probas)

        # Calculate F1 for each threshold
        f1_scores = 2 * (precisions[:-1] * recalls[:-1]) / (precisions[:-1] + recalls[:-1] + 1e-10)

        # Find threshold with best F1
        best_idx = np.argmax(f1_scores)
        best_threshold = thresholds[best_idx]
        best_precision = precisions[best_idx]
        best_recall = recalls[best_idx]
        best_f1 = f1_scores[best_idx]

        print(f"  Optimal threshold: {best_threshold:.3f}")
        print(f"  Precision: {best_precision:.3f}")
        print(f"  Recall: {best_recall:.3f}")
        print(f"  F1 Score: {best_f1:.3f}")

        self.threshold = best_threshold
        return best_threshold

    def predict(self, X: pd.DataFrame) -> tuple:
        """Generate predictions and probability scores."""
        probas = self.model.predict_proba(X)[:, 1]
        predictions = (probas >= self.threshold).astype(int)
        return predictions, probas

    def evaluate(self, X: pd.DataFrame, y_true: pd.Series, dataset_name: str = "Dataset") -> dict:
        """Evaluate model performance."""
        y_pred, y_scores = self.predict(X)

        metrics = {
            'precision': float(precision_score(y_true, y_pred)),
            'recall': float(recall_score(y_true, y_pred)),
            'f1': float(f1_score(y_true, y_pred)),
            'confusion_matrix': confusion_matrix(y_true, y_pred).tolist(),
            'threshold': float(self.threshold)
        }

        print(f"\n{dataset_name} Results ({self.model_type.upper()}):")
        print(f"  Precision: {metrics['precision']:.3f}")
        print(f"  Recall:    {metrics['recall']:.3f}")
        print(f"  F1 Score:  {metrics['f1']:.3f}")
        print(f"  Threshold: {metrics['threshold']:.3f}")
        print(f"\nConfusion Matrix:")
        cm = metrics['confusion_matrix']
        print(f"  TN: {cm[0][0]:5d}  FP: {cm[0][1]:5d}")
        print(f"  FN: {cm[1][0]:5d}  TP: {cm[1][1]:5d}")

        return metrics


def compare_models():
    """Compare different ML approaches."""
    print("=" * 60)
    print("IMPROVED ML Model Comparison")
    print("=" * 60)

    # Setup paths
    script_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir = os.path.join(os.path.dirname(script_dir), 'data')

    # Load data
    print("\nLoading data...")
    train_df = pd.read_csv(os.path.join(data_dir, 'train.csv'))
    test_df = pd.read_csv(os.path.join(data_dir, 'test.csv'))
    test_labels = pd.read_csv(os.path.join(data_dir, 'test_labels.csv'))
    test_df = test_df.merge(test_labels, on='event_id')

    print(f"Train: {len(train_df)} events")
    print(f"Test: {len(test_df)} events")

    # Compare 3 models
    models_to_test = [
        ('rf', 'Random Forest (baseline)'),
        ('rf_balanced', 'Random Forest (balanced weights)'),
        ('xgboost', 'XGBoost (with scale_pos_weight)'),
    ]

    results = {}

    for model_type, model_name in models_to_test:
        print("\n" + "=" * 60)
        print(f"Training: {model_name}")
        print("=" * 60)

        # Initialize model
        model = ImprovedSpoofingMLModel(model_type=model_type)

        # Engineer features
        train_features = model.engineer_features(train_df)
        X_train = model.select_features(train_features)
        y_train = train_df['is_spoofed'].astype(int)

        # Train
        print("\n" + "-" * 60)
        model.train(X_train, y_train)

        # Tune threshold for F1 (not precision)
        model.tune_threshold_for_f1(X_train, y_train)

        # Evaluate on training
        print("\n" + "-" * 60)
        train_metrics = model.evaluate(X_train, y_train, f"Training ({model_name})")

        # Evaluate on test
        test_features = model.engineer_features(test_df)
        X_test = model.select_features(test_features)
        y_test = test_df['is_spoofed'].astype(int)

        print("\n" + "-" * 60)
        test_metrics = model.evaluate(X_test, y_test, f"Test ({model_name})")

        results[model_type] = {
            'name': model_name,
            'train': train_metrics,
            'test': test_metrics
        }

    # Summary comparison
    print("\n\n" + "=" * 60)
    print("SUMMARY COMPARISON")
    print("=" * 60)
    print(f"\n{'Model':<35} {'Precision':>10} {'Recall':>10} {'F1':>10}")
    print("-" * 70)

    # Include rules baseline for comparison
    print(f"{'Rules-based (baseline)':.<35} {'0.917':>10} {'0.834':>10} {'0.874':>10}")

    for model_type, result in results.items():
        metrics = result['test']
        print(f"{result['name']:.<35} {metrics['precision']:>10.3f} {metrics['recall']:>10.3f} {metrics['f1']:>10.3f}")

    # Save results
    output_path = os.path.join(data_dir, 'improved_ml_comparison.json')
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)
    print(f"\n[OK] Results saved to {output_path}")

    # Find best model
    best_model = max(results.items(), key=lambda x: x[1]['test']['f1'])
    print(f"\n*** BEST MODEL: {best_model[1]['name']} (F1 = {best_model[1]['test']['f1']:.3f}) ***")

    print("\n" + "=" * 60)
    print("Analysis complete!")
    print("=" * 60)


if __name__ == "__main__":
    compare_models()
