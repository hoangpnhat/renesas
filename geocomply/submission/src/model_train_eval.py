"""
ML model for location spoofing detection.
Train, evaluate, and compare with rules-based baseline.
"""

import numpy as np
import pandas as pd
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import (
    precision_score, recall_score, f1_score,
    confusion_matrix, classification_report,
    precision_recall_curve, roc_auc_score
)
import matplotlib.pyplot as plt
import seaborn as sns
import json
import os
from typing import Dict, Tuple


class SpoofingMLModel:
    """ML model for location spoofing detection."""

    def __init__(self):
        # Use Random Forest for interpretability
        self.model = RandomForestClassifier(
            n_estimators=100,
            max_depth=10,
            min_samples_split=20,
            min_samples_leaf=10,
            random_state=42,
            n_jobs=-1
        )
        self.feature_columns = None
        self.threshold = 0.5  # Default threshold, will be tuned

    def engineer_features(self, df: pd.DataFrame) -> pd.DataFrame:
        """Create features for ML model."""
        features = df.copy()

        # Basic location features
        features['lat_rounded'] = features['latitude'].round(1)
        features['lon_rounded'] = features['longitude'].round(1)

        # Speed-related features
        features['speed_kmh'] = features['speed'] * 3.6
        features['is_stationary'] = (features['speed'] < 0.5).astype(int)
        features['is_fast_moving'] = (features['speed'] > 20).astype(int)  # > 72 km/h

        # Accuracy features
        features['accuracy_perfect'] = (features['accuracy'] < 2).astype(int)
        features['accuracy_poor'] = (features['accuracy'] > 50).astype(int)

        # Network features
        features['has_wifi'] = (features['wifi_count'] > 0).astype(int)
        features['network_is_wifi'] = (features['network_type'] == 'WiFi').astype(int)
        features['network_is_5g'] = (features['network_type'] == '5G').astype(int)

        # Sensor features
        features['low_accel_variance'] = (features['accelerometer_variance'] < 0.01).astype(int)
        features['low_gyro_variance'] = (features['gyroscope_variance'] < 0.01).astype(int)
        features['sensor_mismatch'] = (
            (features['speed'] > 1.0) &
            ((features['accelerometer_variance'] < 0.01) | (features['gyroscope_variance'] < 0.01))
        ).astype(int)

        # Device features
        features['low_battery'] = (features['battery_level'] < 20).astype(int)
        features['dev_options_enabled'] = features['developer_options_enabled'].astype(int)

        # IP/Location features (simplified)
        features['ip_prefix'] = features['ip_address'].str.split('.').str[:3].str.join('.')
        features['ip_is_west'] = features['ip_prefix'].isin(['192.168.1', '10.0.1', '172.16.1']).astype(int)
        features['ip_is_east'] = features['ip_prefix'].isin(['192.168.2', '10.0.2', '172.16.2']).astype(int)
        features['ip_is_central'] = features['ip_prefix'].isin(['192.168.3', '10.0.3', '172.16.3']).astype(int)

        # Timezone features
        features['tz_expected'] = (features['longitude'] / 15).round().astype(int)
        features['tz_mismatch'] = (features['device_timezone_offset'] - features['tz_expected']).abs()
        features['tz_mismatch_significant'] = (features['tz_mismatch'] > 2).astype(int)

        # Direct security indicators
        features['mock_location'] = features['mock_location_enabled'].astype(int)

        return features

    def select_features(self, df: pd.DataFrame) -> pd.DataFrame:
        """Select features for model training."""
        # Define features to use
        feature_cols = [
            # Location
            'latitude', 'longitude', 'accuracy', 'altitude',

            # Speed and movement
            'speed', 'speed_kmh', 'bearing',
            'is_stationary', 'is_fast_moving',

            # Accuracy flags
            'accuracy_perfect', 'accuracy_poor',

            # Network
            'wifi_count', 'cell_tower_count',
            'has_wifi', 'network_is_wifi', 'network_is_5g',

            # Sensors
            'accelerometer_variance', 'gyroscope_variance',
            'low_accel_variance', 'low_gyro_variance',
            'sensor_mismatch',

            # Device
            'battery_level', 'is_charging',
            'low_battery', 'dev_options_enabled',

            # IP/Geo
            'ip_is_west', 'ip_is_east', 'ip_is_central',

            # Timezone
            'device_timezone_offset', 'tz_expected',
            'tz_mismatch', 'tz_mismatch_significant',

            # Security
            'mock_location',
        ]

        self.feature_columns = feature_cols
        return df[feature_cols]

    def train(self, X_train: pd.DataFrame, y_train: pd.Series):
        """Train the ML model."""
        print("Training ML model...")
        self.model.fit(X_train, y_train)
        print(f"Model trained with {len(self.feature_columns)} features")

        # Show feature importance
        feature_importance = pd.DataFrame({
            'feature': self.feature_columns,
            'importance': self.model.feature_importances_
        }).sort_values('importance', ascending=False)

        print("\nTop 10 most important features:")
        for idx, row in feature_importance.head(10).iterrows():
            print(f"  {row['feature']:30s}: {row['importance']:.4f}")

        return feature_importance

    def predict(self, X: pd.DataFrame) -> Tuple[np.ndarray, np.ndarray]:
        """Generate predictions and probability scores."""
        # Predict probabilities
        probas = self.model.predict_proba(X)[:, 1]  # Probability of spoof class

        # Predict labels using threshold
        predictions = (probas >= self.threshold).astype(int)

        return predictions, probas

    def tune_threshold(self, X_val: pd.DataFrame, y_val: pd.Series,
                      target_precision: float = 0.90) -> float:
        """
        Tune decision threshold to achieve target precision.
        """
        print(f"\nTuning threshold to achieve precision >= {target_precision:.2f}...")

        # Get prediction probabilities
        probas = self.model.predict_proba(X_val)[:, 1]

        # Calculate precision-recall curve
        precisions, recalls, thresholds = precision_recall_curve(y_val, probas)

        # Find threshold that achieves target precision
        valid_thresholds = thresholds[precisions[:-1] >= target_precision]

        if len(valid_thresholds) > 0:
            # Choose threshold with highest recall among valid ones
            best_idx = np.where(precisions[:-1] >= target_precision)[0][-1]
            best_threshold = thresholds[best_idx]
            best_precision = precisions[best_idx]
            best_recall = recalls[best_idx]

            print(f"  Optimal threshold: {best_threshold:.3f}")
            print(f"  Precision: {best_precision:.3f}")
            print(f"  Recall: {best_recall:.3f}")

            self.threshold = best_threshold
        else:
            print(f"  Could not achieve target precision {target_precision:.2f}")
            print(f"  Using default threshold: {self.threshold:.3f}")

        return self.threshold

    def evaluate(self, X: pd.DataFrame, y_true: pd.Series, dataset_name: str = "Dataset") -> Dict:
        """Evaluate model performance."""
        y_pred, y_scores = self.predict(X)

        metrics = {
            'precision': precision_score(y_true, y_pred),
            'recall': recall_score(y_true, y_pred),
            'f1': f1_score(y_true, y_pred),
            'roc_auc': roc_auc_score(y_true, y_scores),
            'confusion_matrix': confusion_matrix(y_true, y_pred).tolist(),
            'threshold': self.threshold
        }

        print(f"\n{dataset_name} Results:")
        print(f"  Precision: {metrics['precision']:.3f}")
        print(f"  Recall:    {metrics['recall']:.3f}")
        print(f"  F1 Score:  {metrics['f1']:.3f}")
        print(f"  ROC AUC:   {metrics['roc_auc']:.3f}")
        print(f"  Threshold: {metrics['threshold']:.3f}")
        print(f"\nConfusion Matrix:")
        cm = metrics['confusion_matrix']
        print(f"  TN: {cm[0][0]:5d}  FP: {cm[0][1]:5d}")
        print(f"  FN: {cm[1][0]:5d}  TP: {cm[1][1]:5d}")

        return metrics

    def plot_pr_curve(self, X: pd.DataFrame, y_true: pd.Series,
                     save_path: str = None):
        """Plot precision-recall curve."""
        y_scores = self.model.predict_proba(X)[:, 1]

        precisions, recalls, thresholds = precision_recall_curve(y_true, y_scores)

        plt.figure(figsize=(10, 6))
        plt.plot(recalls, precisions, linewidth=2)
        plt.axvline(x=recall_score(y_true, (y_scores >= self.threshold)),
                   color='r', linestyle='--', label=f'Operating Point (threshold={self.threshold:.3f})')
        plt.xlabel('Recall', fontsize=12)
        plt.ylabel('Precision', fontsize=12)
        plt.title('Precision-Recall Curve', fontsize=14)
        plt.grid(True, alpha=0.3)
        plt.legend()
        plt.tight_layout()

        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            print(f"[OK] PR curve saved to {save_path}")
        else:
            plt.show()

        plt.close()


def perform_ablation_study(train_df: pd.DataFrame, test_labels_df: pd.DataFrame,
                          rules_results_df: pd.DataFrame, ml_results_df: pd.DataFrame,
                          y_test: pd.Series) -> Dict:
    """
    Compare Rules-only, ML-only, and Hybrid approaches.
    """
    print("\n" + "=" * 60)
    print("ABLATION STUDY: Rules vs ML vs Hybrid")
    print("=" * 60)

    # Load rules-based predictions for training data
    rules_train_path = os.path.join(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
        'data', 'train_rules_results.csv'
    )
    rules_train = pd.read_csv(rules_train_path)

    results = {}

    # 1. Rules-only (on test)
    print("\n[1] Rules-only approach:")
    y_pred_rules = rules_results_df['spoof_flag_rules']
    results['rules_only'] = {
        'precision': precision_score(y_test, y_pred_rules),
        'recall': recall_score(y_test, y_pred_rules),
        'f1': f1_score(y_test, y_pred_rules)
    }
    print(f"  Precision: {results['rules_only']['precision']:.3f}")
    print(f"  Recall:    {results['rules_only']['recall']:.3f}")
    print(f"  F1:        {results['rules_only']['f1']:.3f}")

    # 2. ML-only (on test)
    print("\n[2] ML-only approach:")
    y_pred_ml = ml_results_df['spoof_flag_ml']
    results['ml_only'] = {
        'precision': precision_score(y_test, y_pred_ml),
        'recall': recall_score(y_test, y_pred_ml),
        'f1': f1_score(y_test, y_pred_ml)
    }
    print(f"  Precision: {results['ml_only']['precision']:.3f}")
    print(f"  Recall:    {results['ml_only']['recall']:.3f}")
    print(f"  F1:        {results['ml_only']['f1']:.3f}")

    # 3. Hybrid (rules OR ml)
    print("\n[3] Hybrid approach (Rules OR ML):")
    y_pred_hybrid = (rules_results_df['spoof_flag_rules'] | ml_results_df['spoof_flag_ml']).astype(int)
    results['hybrid'] = {
        'precision': precision_score(y_test, y_pred_hybrid),
        'recall': recall_score(y_test, y_pred_hybrid),
        'f1': f1_score(y_test, y_pred_hybrid)
    }
    print(f"  Precision: {results['hybrid']['precision']:.3f}")
    print(f"  Recall:    {results['hybrid']['recall']:.3f}")
    print(f"  F1:        {results['hybrid']['f1']:.3f}")

    # Summary comparison
    print("\n" + "-" * 60)
    print("SUMMARY:")
    print(f"{'Approach':<15} {'Precision':>10} {'Recall':>10} {'F1':>10}")
    print("-" * 60)
    for name, metrics in results.items():
        print(f"{name:<15} {metrics['precision']:>10.3f} {metrics['recall']:>10.3f} {metrics['f1']:>10.3f}")

    return results


def analyze_errors(test_df: pd.DataFrame, y_test: pd.Series,
                  y_pred: pd.Series, y_scores: pd.Series) -> Dict:
    """
    Analyze false positives and false negatives.
    """
    print("\n" + "=" * 60)
    print("ERROR ANALYSIS")
    print("=" * 60)

    # Add predictions to dataframe
    analysis_df = test_df.copy()
    analysis_df['y_true'] = y_test.values
    analysis_df['y_pred'] = y_pred
    analysis_df['y_score'] = y_scores

    # False Positives (predicted spoof, actually legitimate)
    fp_df = analysis_df[(analysis_df['y_true'] == 0) & (analysis_df['y_pred'] == 1)]
    print(f"\nFalse Positives: {len(fp_df)}")

    # False Negatives (predicted legitimate, actually spoof)
    fn_df = analysis_df[(analysis_df['y_true'] == 1) & (analysis_df['y_pred'] == 0)]
    print(f"False Negatives: {len(fn_df)}")

    errors = {
        'false_positives': [],
        'false_negatives': []
    }

    # Analyze top 5 FPs (highest confidence mistakes)
    if len(fp_df) > 0:
        print("\n" + "-" * 60)
        print("Top 5 False Positives (high confidence mistakes):")
        top_fp = fp_df.nlargest(5, 'y_score')
        for idx, row in top_fp.iterrows():
            error_info = {
                'event_id': row['event_id'],
                'score': float(row['y_score']),
                'reason': []
            }

            print(f"\n  Event {row['event_id']}:")
            print(f"    Score: {row['y_score']:.3f}")

            # Identify likely reasons
            if row['mock_location_enabled']:
                error_info['reason'].append("mock_location_enabled")
                print(f"    - Mock location enabled (dev testing?)")

            if row['accuracy'] < 2:
                error_info['reason'].append("perfect_accuracy")
                print(f"    - Very high accuracy ({row['accuracy']:.1f}m)")

            if row['device_timezone_offset'] != int(row['longitude'] / 15):
                error_info['reason'].append("timezone_mismatch")
                print(f"    - Timezone mismatch (could be near border or using VPN)")

            if row['speed'] > 1 and row['accelerometer_variance'] < 0.01:
                error_info['reason'].append("sensor_mismatch")
                print(f"    - Low sensor activity while moving")

            errors['false_positives'].append(error_info)

    # Analyze top 5 FNs (missed spoofs with low scores)
    if len(fn_df) > 0:
        print("\n" + "-" * 60)
        print("Top 5 False Negatives (missed spoofs):")
        top_fn = fn_df.nsmallest(5, 'y_score')
        for idx, row in top_fn.iterrows():
            error_info = {
                'event_id': row['event_id'],
                'score': float(row['y_score']),
                'reason': []
            }

            print(f"\n  Event {row['event_id']}:")
            print(f"    Score: {row['y_score']:.3f}")

            # Identify why it was missed
            if not row['mock_location_enabled'] and row['accuracy'] > 5:
                error_info['reason'].append("subtle_spoof")
                print(f"    - Subtle spoofing (no obvious indicators)")

            if 'spoof_note' in row and pd.notna(row['spoof_note']):
                error_info['reason'].append(row['spoof_note'])
                print(f"    - Type: {row['spoof_note']}")

            errors['false_negatives'].append(error_info)

    return errors


def main():
    """Main training and evaluation pipeline."""
    print("=" * 60)
    print("ML Model Training & Evaluation")
    print("=" * 60)

    # Setup paths
    script_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir = os.path.join(os.path.dirname(script_dir), 'data')

    # Load training data
    print("\nLoading training data...")
    train_df = pd.read_csv(os.path.join(data_dir, 'train.csv'))
    print(f"Loaded {len(train_df)} training events")

    # Initialize model
    model = SpoofingMLModel()

    # Engineer features
    print("\nEngineering features...")
    train_features = model.engineer_features(train_df)
    X_train = model.select_features(train_features)
    y_train = train_df['is_spoofed'].astype(int)

    # Train model
    print("\n" + "-" * 60)
    feature_importance = model.train(X_train, y_train)

    # Tune threshold on training data (targeting precision ~85% for better balance)
    model.tune_threshold(X_train, y_train, target_precision=0.85)

    # Evaluate on training data
    print("\n" + "-" * 60)
    train_metrics = model.evaluate(X_train, y_train, "Training Set")

    # Load and process test data
    print("\n" + "=" * 60)
    print("Processing test data...")
    test_df = pd.read_csv(os.path.join(data_dir, 'test.csv'))
    test_labels = pd.read_csv(os.path.join(data_dir, 'test_labels.csv'))
    test_df = test_df.merge(test_labels, on='event_id')

    test_features = model.engineer_features(test_df)
    X_test = model.select_features(test_features)
    y_test = test_df['is_spoofed'].astype(int)

    # Evaluate on test data
    print("\n" + "-" * 60)
    test_metrics = model.evaluate(X_test, y_test, "Test Set")

    # Generate predictions
    y_pred_test, y_scores_test = model.predict(X_test)

    # Create results dataframe
    test_results = test_df.copy()
    test_results['spoof_score_ml'] = y_scores_test
    test_results['spoof_flag_ml'] = y_pred_test

    # Save test results
    output_path = os.path.join(data_dir, 'test_ml_results.csv')
    test_results.to_csv(output_path, index=False)
    print(f"\n[OK] Test ML results saved to {output_path}")

    # Plot PR curve
    pr_curve_path = os.path.join(data_dir, 'pr_curve.png')
    model.plot_pr_curve(X_test, y_test, save_path=pr_curve_path)

    # Load rules-based results for comparison
    rules_test_results = pd.read_csv(os.path.join(data_dir, 'test_rules_results.csv'))

    # Perform ablation study
    ablation_results = perform_ablation_study(
        train_df, test_labels, rules_test_results, test_results, y_test
    )

    # Error analysis
    errors = analyze_errors(test_df, y_test, y_pred_test, y_scores_test)

    # Save all metrics
    all_metrics = {
        'train_metrics': train_metrics,
        'test_metrics': test_metrics,
        'ablation_study': ablation_results,
        'error_analysis': errors
    }

    metrics_path = os.path.join(data_dir, 'model_metrics.json')
    with open(metrics_path, 'w') as f:
        json.dump(all_metrics, f, indent=2)
    print(f"\n[OK] All metrics saved to {metrics_path}")

    print("\n" + "=" * 60)
    print("ML model training and evaluation complete!")
    print("=" * 60)


if __name__ == "__main__":
    main()
