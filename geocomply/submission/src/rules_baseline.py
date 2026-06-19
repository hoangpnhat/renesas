"""
Rules-based baseline for location spoofing detection.
Implements heuristic rules to detect various spoofing patterns.
"""

import numpy as np
import pandas as pd
from typing import Dict, List, Tuple
import os


class RulesBasedDetector:
    """Heuristic rules for detecting location spoofing."""

    def __init__(self):
        # Define thresholds for rules
        self.MAX_SPEED_KMH = 150  # Maximum realistic speed (even for planes during taxi)
        self.PERFECT_ACCURACY_THRESHOLD = 2.0  # Too-perfect accuracy in meters
        self.MIN_SENSOR_VARIANCE_MOVING = 0.01  # Minimum expected sensor activity when moving
        self.TIMEZONE_TOLERANCE = 1  # Allowed timezone offset difference

        # IP region to coordinate mapping (simplified)
        self.IP_REGION_COORDS = {
            '192.168.1': {'region': 'west', 'lat_range': (32.0, 48.0), 'lon_range': (-125.0, -114.0)},
            '10.0.1': {'region': 'west', 'lat_range': (32.0, 48.0), 'lon_range': (-125.0, -114.0)},
            '172.16.1': {'region': 'west', 'lat_range': (32.0, 48.0), 'lon_range': (-125.0, -114.0)},
            '192.168.2': {'region': 'east', 'lat_range': (25.0, 45.0), 'lon_range': (-85.0, -66.0)},
            '10.0.2': {'region': 'east', 'lat_range': (25.0, 45.0), 'lon_range': (-85.0, -66.0)},
            '172.16.2': {'region': 'east', 'lat_range': (25.0, 45.0), 'lon_range': (-85.0, -66.0)},
            '192.168.3': {'region': 'central', 'lat_range': (28.0, 45.0), 'lon_range': (-104.0, -86.0)},
            '10.0.3': {'region': 'central', 'lat_range': (28.0, 45.0), 'lon_range': (-104.0, -86.0)},
            '172.16.3': {'region': 'central', 'lat_range': (28.0, 45.0), 'lon_range': (-104.0, -86.0)},
        }

    def calculate_distance(self, lat1: float, lon1: float, lat2: float, lon2: float) -> float:
        """Calculate distance in km using Haversine formula."""
        R = 6371  # Earth radius in km

        lat1, lon1, lat2, lon2 = map(np.radians, [lat1, lon1, lat2, lon2])
        dlat = lat2 - lat1
        dlon = lon2 - lon1

        a = np.sin(dlat/2)**2 + np.cos(lat1) * np.cos(lat2) * np.sin(dlon/2)**2
        c = 2 * np.arcsin(np.sqrt(a))

        return R * c

    def get_ip_region_info(self, ip_address: str) -> Dict:
        """Extract region info from IP address prefix."""
        prefix = '.'.join(ip_address.split('.')[:3])
        return self.IP_REGION_COORDS.get(prefix, None)

    def check_teleportation(self, df: pd.DataFrame) -> pd.Series:
        """
        Rule 1: Detect teleportation (impossible speed between consecutive points).
        Checks if movement speed exceeds maximum realistic speed.
        """
        flags = pd.Series(False, index=df.index)

        # Sort by device and timestamp
        df_sorted = df.sort_values(['device_id', 'timestamp']).reset_index(drop=True)

        for device_id in df_sorted['device_id'].unique():
            device_data = df_sorted[df_sorted['device_id'] == device_id].copy()

            if len(device_data) < 2:
                continue

            # Calculate distance and time between consecutive points
            device_data['prev_lat'] = device_data['latitude'].shift(1)
            device_data['prev_lon'] = device_data['longitude'].shift(1)
            device_data['prev_time'] = pd.to_datetime(device_data['timestamp']).shift(1)
            device_data['current_time'] = pd.to_datetime(device_data['timestamp'])

            for idx in device_data.index[1:]:
                if pd.isna(device_data.loc[idx, 'prev_lat']):
                    continue

                # Calculate distance
                distance_km = self.calculate_distance(
                    device_data.loc[idx, 'prev_lat'],
                    device_data.loc[idx, 'prev_lon'],
                    device_data.loc[idx, 'latitude'],
                    device_data.loc[idx, 'longitude']
                )

                # Calculate time difference in hours
                time_diff = (device_data.loc[idx, 'current_time'] -
                           device_data.loc[idx, 'prev_time']).total_seconds() / 3600

                if time_diff > 0:
                    speed_kmh = distance_km / time_diff

                    # Flag if speed exceeds maximum realistic speed
                    if speed_kmh > self.MAX_SPEED_KMH:
                        original_idx = df_sorted.loc[idx, 'event_id']
                        flags[df['event_id'] == original_idx] = True

        return flags

    def check_ip_geo_mismatch(self, df: pd.DataFrame) -> pd.Series:
        """
        Rule 2: Detect IP/Geo mismatch.
        IP region should roughly match GPS coordinates.
        """
        flags = pd.Series(False, index=df.index)

        for idx in df.index:
            ip_info = self.get_ip_region_info(df.loc[idx, 'ip_address'])

            if ip_info is None:
                continue

            lat = df.loc[idx, 'latitude']
            lon = df.loc[idx, 'longitude']

            # Check if coordinates are within expected region for IP
            lat_in_range = ip_info['lat_range'][0] <= lat <= ip_info['lat_range'][1]
            lon_in_range = ip_info['lon_range'][0] <= lon <= ip_info['lon_range'][1]

            # Allow some tolerance for border cases
            if not (lat_in_range and lon_in_range):
                # Check if it's a significant mismatch (not just border case)
                lat_diff = min(abs(lat - ip_info['lat_range'][0]), abs(lat - ip_info['lat_range'][1]))
                lon_diff = min(abs(lon - ip_info['lon_range'][0]), abs(lon - ip_info['lon_range'][1]))

                # If outside by more than 5 degrees, flag it
                if lat_diff > 5 or lon_diff > 5:
                    flags[idx] = True

        return flags

    def check_mock_location(self, df: pd.DataFrame) -> pd.Series:
        """
        Rule 3: Detect mock location enabled.
        Direct indicator of potential spoofing.
        """
        return df['mock_location_enabled'] == True

    def check_sensor_mismatch(self, df: pd.DataFrame) -> pd.Series:
        """
        Rule 4: Detect sensor mismatch (movement without sensor activity).
        If device is moving but sensors show no activity, likely spoofed.
        """
        flags = pd.Series(False, index=df.index)

        for idx in df.index:
            speed = df.loc[idx, 'speed']
            accel_var = df.loc[idx, 'accelerometer_variance']
            gyro_var = df.loc[idx, 'gyroscope_variance']

            # If moving (speed > 1 m/s ~ 3.6 km/h) but sensors inactive
            if speed > 1.0:
                if accel_var < self.MIN_SENSOR_VARIANCE_MOVING or gyro_var < self.MIN_SENSOR_VARIANCE_MOVING:
                    flags[idx] = True

        return flags

    def check_timezone_mismatch(self, df: pd.DataFrame) -> pd.Series:
        """
        Rule 5: Detect timezone mismatch.
        Device timezone should roughly match GPS coordinates.
        """
        flags = pd.Series(False, index=df.index)

        for idx in df.index:
            lon = df.loc[idx, 'longitude']
            device_tz = df.loc[idx, 'device_timezone_offset']

            # Calculate expected timezone from longitude (rough estimate)
            expected_tz = int(lon / 15)

            # Allow tolerance for timezone boundaries and DST
            if abs(device_tz - expected_tz) > self.TIMEZONE_TOLERANCE + 1:
                flags[idx] = True

        return flags

    def check_perfect_accuracy(self, df: pd.DataFrame) -> pd.Series:
        """
        Rule 6: Detect suspiciously perfect accuracy.
        Real GPS has natural variance; perfect accuracy is suspicious.
        """
        return df['accuracy'] < self.PERFECT_ACCURACY_THRESHOLD

    def apply_all_rules(self, df: pd.DataFrame) -> pd.DataFrame:
        """
        Apply all detection rules and return DataFrame with results.
        """
        print("Applying rules-based detection...")

        # Make a copy to avoid modifying original
        result_df = df.copy()

        # Apply each rule
        print("  - Rule 1: Checking for teleportation...")
        result_df['rule_teleportation'] = self.check_teleportation(df)

        print("  - Rule 2: Checking for IP/Geo mismatch...")
        result_df['rule_ip_geo_mismatch'] = self.check_ip_geo_mismatch(df)

        print("  - Rule 3: Checking for mock location...")
        result_df['rule_mock_location'] = self.check_mock_location(df)

        print("  - Rule 4: Checking for sensor mismatch...")
        result_df['rule_sensor_mismatch'] = self.check_sensor_mismatch(df)

        print("  - Rule 5: Checking for timezone mismatch...")
        result_df['rule_timezone_mismatch'] = self.check_timezone_mismatch(df)

        print("  - Rule 6: Checking for perfect accuracy...")
        result_df['rule_perfect_accuracy'] = self.check_perfect_accuracy(df)

        # Aggregate rules: flag as spoof if ANY rule triggers
        rule_columns = [col for col in result_df.columns if col.startswith('rule_')]
        result_df['spoof_flag_rules'] = result_df[rule_columns].any(axis=1)

        # Count how many rules triggered
        result_df['num_rules_triggered'] = result_df[rule_columns].sum(axis=1)

        # Create a confidence score (0-1) based on number of rules triggered
        max_rules = len(rule_columns)
        result_df['spoof_score_rules'] = result_df['num_rules_triggered'] / max_rules

        print(f"Rules-based detection complete.")
        print(f"  Flagged: {result_df['spoof_flag_rules'].sum()} / {len(result_df)} events")

        return result_df

    def evaluate(self, df_with_labels: pd.DataFrame, df_predictions: pd.DataFrame) -> Dict:
        """
        Evaluate rules-based detector performance.
        """
        from sklearn.metrics import precision_score, recall_score, f1_score, confusion_matrix

        y_true = df_with_labels['is_spoofed']
        y_pred = df_predictions['spoof_flag_rules']

        metrics = {
            'precision': precision_score(y_true, y_pred),
            'recall': recall_score(y_true, y_pred),
            'f1': f1_score(y_true, y_pred),
            'confusion_matrix': confusion_matrix(y_true, y_pred).tolist()
        }

        return metrics


def main():
    """Run rules-based detection on training data."""
    print("=" * 60)
    print("Rules-Based Location Spoofing Detection")
    print("=" * 60)

    # Load data
    script_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir = os.path.join(os.path.dirname(script_dir), 'data')

    train_path = os.path.join(data_dir, 'train.csv')
    print(f"\nLoading training data from {train_path}...")
    train_df = pd.read_csv(train_path)
    print(f"Loaded {len(train_df)} events")

    # Initialize detector
    detector = RulesBasedDetector()

    # Apply rules
    print("\n" + "-" * 60)
    train_results = detector.apply_all_rules(train_df)

    # Evaluate
    print("\n" + "-" * 60)
    print("Evaluating on training data...")
    metrics = detector.evaluate(train_df, train_results)

    print(f"\nResults:")
    print(f"  Precision: {metrics['precision']:.3f}")
    print(f"  Recall:    {metrics['recall']:.3f}")
    print(f"  F1 Score:  {metrics['f1']:.3f}")
    print(f"\nConfusion Matrix:")
    print(f"  TN: {metrics['confusion_matrix'][0][0]:5d}  FP: {metrics['confusion_matrix'][0][1]:5d}")
    print(f"  FN: {metrics['confusion_matrix'][1][0]:5d}  TP: {metrics['confusion_matrix'][1][1]:5d}")

    # Save results
    output_path = os.path.join(data_dir, 'train_rules_results.csv')
    train_results.to_csv(output_path, index=False)
    print(f"\n[OK] Results saved to {output_path}")

    # Process test data
    test_path = os.path.join(data_dir, 'test.csv')
    print(f"\n" + "=" * 60)
    print(f"Processing test data from {test_path}...")
    test_df = pd.read_csv(test_path)
    print(f"Loaded {len(test_df)} events")

    print("\n" + "-" * 60)
    test_results = detector.apply_all_rules(test_df)

    output_path = os.path.join(data_dir, 'test_rules_results.csv')
    test_results.to_csv(output_path, index=False)
    print(f"\n[OK] Test results saved to {output_path}")

    print("\n" + "=" * 60)
    print("Rules-based detection complete!")
    print("=" * 60)


if __name__ == "__main__":
    main()
