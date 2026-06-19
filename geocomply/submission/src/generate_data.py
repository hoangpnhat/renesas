"""
Generate synthetic location data for spoofing detection.
This script creates realistic mobile device location events with controlled spoofing injection.
"""

import numpy as np
import pandas as pd
from datetime import datetime, timedelta
import random
from typing import List, Dict, Tuple
import json
import os

# Set random seed for reproducibility
np.random.seed(42)
random.seed(42)

# Configuration
TRAIN_SIZE = 15000
TEST_SIZE = 5000
SPOOF_RATE = 0.20  # 20% spoofing rate in training data

# Real-world location bounds (focus on US for simplicity)
US_BOUNDS = {
    'lat_min': 25.0,
    'lat_max': 49.0,
    'lon_min': -125.0,
    'lon_max': -66.0
}

# IP geolocation data (simplified - mapping regions to IP prefixes)
IP_REGIONS = {
    'west': ['192.168.1', '10.0.1', '172.16.1'],  # CA, WA, OR
    'east': ['192.168.2', '10.0.2', '172.16.2'],  # NY, MA, FL
    'central': ['192.168.3', '10.0.3', '172.16.3'],  # TX, IL, CO
}

REGION_COORDS = {
    'west': {'lat': (32.0, 48.0), 'lon': (-125.0, -114.0)},
    'east': {'lat': (25.0, 45.0), 'lon': (-85.0, -66.0)},
    'central': {'lat': (28.0, 45.0), 'lon': (-104.0, -86.0)},
}


class LocationDataGenerator:
    """Generate synthetic location data with realistic patterns and spoofing."""

    def __init__(self, num_samples: int, spoof_rate: float):
        self.num_samples = num_samples
        self.spoof_rate = spoof_rate
        self.num_spoofed = int(num_samples * spoof_rate)
        self.num_legitimate = num_samples - self.num_spoofed

    def generate_ip_address(self, region: str) -> str:
        """Generate IP address for a region."""
        prefix = random.choice(IP_REGIONS[region])
        return f"{prefix}.{random.randint(1, 254)}"

    def calculate_distance(self, lat1: float, lon1: float, lat2: float, lon2: float) -> float:
        """Calculate distance in km using Haversine formula."""
        R = 6371  # Earth radius in km

        lat1, lon1, lat2, lon2 = map(np.radians, [lat1, lon1, lat2, lon2])
        dlat = lat2 - lat1
        dlon = lon2 - lon1

        a = np.sin(dlat/2)**2 + np.cos(lat1) * np.cos(lat2) * np.sin(dlon/2)**2
        c = 2 * np.arcsin(np.sqrt(a))

        return R * c

    def generate_legitimate_track(self, track_length: int, start_time: datetime, region: str) -> List[Dict]:
        """Generate a legitimate location track for a device."""
        events = []

        # Start position within region
        lat = np.random.uniform(*REGION_COORDS[region]['lat'])
        lon = np.random.uniform(*REGION_COORDS[region]['lon'])

        # Movement mode: walking (1-5 km/h) or driving (20-80 km/h)
        is_driving = random.random() > 0.5
        if is_driving:
            speed_kmh = np.random.uniform(20, 80)
            accuracy = np.random.uniform(5, 15)  # meters
        else:
            speed_kmh = np.random.uniform(1, 5)
            accuracy = np.random.uniform(3, 10)  # meters

        current_time = start_time

        for i in range(track_length):
            # Generate event
            event = self._create_event(
                lat=lat,
                lon=lon,
                timestamp=current_time,
                region=region,
                speed_kmh=speed_kmh,
                accuracy=accuracy,
                is_spoofed=False,
                is_driving=is_driving
            )
            events.append(event)

            # Move to next position
            time_delta = random.randint(30, 300)  # 30s to 5min between events
            current_time += timedelta(seconds=time_delta)

            # Calculate realistic movement
            distance_km = (speed_kmh / 3600) * time_delta

            # Add some randomness to speed
            speed_kmh *= np.random.uniform(0.8, 1.2)
            speed_kmh = np.clip(speed_kmh, 1, 100)

            # Random direction with slight bias (smooth movement)
            bearing = np.random.uniform(0, 360)
            lat += (distance_km / 111) * np.cos(np.radians(bearing)) + np.random.normal(0, 0.0001)
            lon += (distance_km / (111 * np.cos(np.radians(lat)))) * np.sin(np.radians(bearing)) + np.random.normal(0, 0.0001)

            # Keep within region bounds (with some leeway)
            lat = np.clip(lat, REGION_COORDS[region]['lat'][0], REGION_COORDS[region]['lat'][1])
            lon = np.clip(lon, REGION_COORDS[region]['lon'][0], REGION_COORDS[region]['lon'][1])

        return events

    def generate_spoofed_track(self, track_length: int, start_time: datetime, spoof_type: str) -> List[Dict]:
        """Generate a spoofed location track with specific spoofing pattern."""
        events = []

        # Start with a legitimate-looking position
        region = random.choice(list(REGION_COORDS.keys()))
        lat = np.random.uniform(*REGION_COORDS[region]['lat'])
        lon = np.random.uniform(*REGION_COORDS[region]['lon'])

        current_time = start_time

        for i in range(track_length):
            is_spoofed = False
            spoof_note = ""

            # Inject spoofing based on type
            if spoof_type == "teleportation":
                # Random jumps in location
                if i > 0 and random.random() < 0.3:  # 30% chance of teleport
                    # Jump to random location
                    lat = np.random.uniform(US_BOUNDS['lat_min'], US_BOUNDS['lat_max'])
                    lon = np.random.uniform(US_BOUNDS['lon_min'], US_BOUNDS['lon_max'])
                    is_spoofed = True
                    spoof_note = "teleported to random location"

            elif spoof_type == "ip_geo_mismatch":
                # Location in one region, IP in another
                wrong_region = random.choice([r for r in REGION_COORDS.keys() if r != region])
                event = self._create_event(
                    lat=lat,
                    lon=lon,
                    timestamp=current_time,
                    region=wrong_region,  # Wrong IP region
                    speed_kmh=np.random.uniform(1, 50),
                    accuracy=np.random.uniform(5, 20),
                    is_spoofed=True,
                    is_driving=random.random() > 0.5
                )
                event['spoof_note'] = "IP region doesn't match GPS location"
                events.append(event)
                current_time += timedelta(seconds=random.randint(30, 300))

                # Small movement
                lat += np.random.normal(0, 0.01)
                lon += np.random.normal(0, 0.01)
                continue

            elif spoof_type == "mock_location":
                # Mock location enabled, too-perfect accuracy
                event = self._create_event(
                    lat=lat,
                    lon=lon,
                    timestamp=current_time,
                    region=region,
                    speed_kmh=np.random.uniform(1, 50),
                    accuracy=1.0,  # Suspiciously perfect accuracy
                    is_spoofed=True,
                    is_driving=random.random() > 0.5
                )
                event['mock_location_enabled'] = True
                event['developer_options_enabled'] = True
                event['spoof_note'] = "mock location app detected"
                events.append(event)
                current_time += timedelta(seconds=random.randint(30, 300))

                lat += np.random.normal(0, 0.001)
                lon += np.random.normal(0, 0.001)
                continue

            elif spoof_type == "sensor_mismatch":
                # Movement reported but no sensor activity
                event = self._create_event(
                    lat=lat,
                    lon=lon,
                    timestamp=current_time,
                    region=region,
                    speed_kmh=np.random.uniform(20, 80),
                    accuracy=np.random.uniform(5, 15),
                    is_spoofed=True,
                    is_driving=True
                )
                # Zero sensor activity despite movement
                event['accelerometer_variance'] = 0.001
                event['gyroscope_variance'] = 0.001
                event['spoof_note'] = "movement without sensor activity"
                events.append(event)
                current_time += timedelta(seconds=random.randint(30, 300))

                # Teleport occasionally
                if random.random() < 0.2:
                    lat = np.random.uniform(*REGION_COORDS[region]['lat'])
                    lon = np.random.uniform(*REGION_COORDS[region]['lon'])
                else:
                    lat += np.random.normal(0, 0.01)
                    lon += np.random.normal(0, 0.01)
                continue

            elif spoof_type == "timezone_mismatch":
                # Timezone doesn't match location
                event = self._create_event(
                    lat=lat,
                    lon=lon,
                    timestamp=current_time,
                    region=region,
                    speed_kmh=np.random.uniform(1, 50),
                    accuracy=np.random.uniform(5, 15),
                    is_spoofed=True,
                    is_driving=random.random() > 0.5
                )
                # Wrong timezone
                event['device_timezone_offset'] = random.choice([-5, -6, -7, -8])  # Wrong for location
                event['spoof_note'] = "timezone doesn't match GPS coordinates"
                events.append(event)
                current_time += timedelta(seconds=random.randint(30, 300))

                lat += np.random.normal(0, 0.01)
                lon += np.random.normal(0, 0.01)
                continue

            # Create event (for teleportation type)
            if spoof_type == "teleportation":
                event = self._create_event(
                    lat=lat,
                    lon=lon,
                    timestamp=current_time,
                    region=region,
                    speed_kmh=np.random.uniform(1, 50),
                    accuracy=np.random.uniform(5, 20),
                    is_spoofed=is_spoofed,
                    is_driving=random.random() > 0.5
                )
                if is_spoofed:
                    event['spoof_note'] = spoof_note
                events.append(event)

            current_time += timedelta(seconds=random.randint(30, 300))

            # Normal movement for teleportation
            if spoof_type == "teleportation":
                lat += np.random.normal(0, 0.01)
                lon += np.random.normal(0, 0.01)

        return events

    def _create_event(self, lat: float, lon: float, timestamp: datetime, region: str,
                     speed_kmh: float, accuracy: float, is_spoofed: bool, is_driving: bool) -> Dict:
        """Create a single location event with all features."""

        # Calculate expected timezone offset based on longitude
        expected_tz_offset = int(lon / 15)  # Rough estimate

        # Sensor data variance (higher when moving)
        if is_driving:
            accel_var = np.random.uniform(0.5, 2.0)
            gyro_var = np.random.uniform(0.3, 1.5)
        else:
            accel_var = np.random.uniform(0.1, 0.5)
            gyro_var = np.random.uniform(0.05, 0.3)

        event = {
            # Basic location
            'latitude': round(lat, 6),
            'longitude': round(lon, 6),
            'timestamp': timestamp.isoformat(),
            'accuracy': round(accuracy, 2),
            'altitude': round(np.random.uniform(0, 500), 1),
            'speed': round(speed_kmh / 3.6, 2),  # m/s
            'bearing': round(np.random.uniform(0, 360), 1),

            # Network info
            'ip_address': self.generate_ip_address(region),
            'network_type': random.choice(['WiFi', '4G', '5G', '3G']),
            'wifi_count': random.randint(0, 15) if random.random() > 0.3 else 0,
            'cell_tower_count': random.randint(1, 5),

            # Device info
            'battery_level': round(np.random.uniform(10, 100), 1),
            'is_charging': random.random() > 0.7,
            'device_timezone_offset': expected_tz_offset + random.choice([0, 0, 0, 1, -1]),  # Mostly correct

            # Sensors
            'accelerometer_variance': round(accel_var, 3),
            'gyroscope_variance': round(gyro_var, 3),

            # Security indicators
            'mock_location_enabled': False,
            'developer_options_enabled': random.random() < 0.05,  # 5% have dev options
            'location_permission': random.choice(['ALWAYS', 'WHILE_IN_USE']),

            # Ground truth
            'is_spoofed': is_spoofed,
        }

        return event

    def generate_dataset(self) -> pd.DataFrame:
        """Generate complete dataset with legitimate and spoofed events."""
        all_events = []
        event_id = 0

        # Generate legitimate tracks
        print(f"Generating {self.num_legitimate} legitimate events...")
        num_legit_tracks = self.num_legitimate // 10  # Average 10 events per track

        for i in range(num_legit_tracks):
            region = random.choice(list(REGION_COORDS.keys()))
            track_length = random.randint(5, 15)
            start_time = datetime(2024, 1, 1) + timedelta(days=random.randint(0, 365),
                                                          hours=random.randint(0, 23))

            track_events = self.generate_legitimate_track(track_length, start_time, region)

            for event in track_events:
                event['event_id'] = f"evt_{event_id:06d}"
                event['device_id'] = f"dev_legit_{i:04d}"
                all_events.append(event)
                event_id += 1

                if len(all_events) >= self.num_legitimate:
                    break

            if len(all_events) >= self.num_legitimate:
                break

        # Generate spoofed tracks
        print(f"Generating {self.num_spoofed} spoofed events...")
        spoof_types = ["teleportation", "ip_geo_mismatch", "mock_location",
                      "sensor_mismatch", "timezone_mismatch"]
        num_spoof_tracks = self.num_spoofed // 10

        for i in range(num_spoof_tracks):
            spoof_type = random.choice(spoof_types)
            track_length = random.randint(5, 15)
            start_time = datetime(2024, 1, 1) + timedelta(days=random.randint(0, 365),
                                                          hours=random.randint(0, 23))

            track_events = self.generate_spoofed_track(track_length, start_time, spoof_type)

            for event in track_events:
                event['event_id'] = f"evt_{event_id:06d}"
                event['device_id'] = f"dev_spoof_{i:04d}"
                event['spoof_type'] = spoof_type
                all_events.append(event)
                event_id += 1

                if len(all_events) >= self.num_samples:
                    break

            if len(all_events) >= self.num_samples:
                break

        # Convert to DataFrame and shuffle
        df = pd.DataFrame(all_events[:self.num_samples])
        df = df.sample(frac=1, random_state=42).reset_index(drop=True)

        print(f"\nDataset generated: {len(df)} total events")
        print(f"Spoofed: {df['is_spoofed'].sum()} ({df['is_spoofed'].mean()*100:.1f}%)")
        print(f"Legitimate: {(~df['is_spoofed']).sum()} ({(~df['is_spoofed']).mean()*100:.1f}%)")

        return df


def main():
    """Generate train and test datasets."""
    print("=" * 60)
    print("Location Spoofing Detection - Data Generation")
    print("=" * 60)

    # Get the script directory and create absolute paths
    script_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir = os.path.join(os.path.dirname(script_dir), 'data')
    os.makedirs(data_dir, exist_ok=True)

    # Generate training data
    print("\n[1/2] Generating TRAINING data...")
    train_generator = LocationDataGenerator(TRAIN_SIZE, SPOOF_RATE)
    train_df = train_generator.generate_dataset()

    # Save training data
    train_path = os.path.join(data_dir, "train.csv")
    train_df.to_csv(train_path, index=False)
    print(f"[OK] Training data saved to {train_path}")

    # Generate test data (similar distribution)
    print("\n[2/2] Generating TEST data...")
    test_generator = LocationDataGenerator(TEST_SIZE, SPOOF_RATE)
    test_df = test_generator.generate_dataset()

    # Remove labels from test set (but keep for evaluation)
    test_with_labels = test_df.copy()
    test_for_scoring = test_df.drop(columns=['is_spoofed', 'spoof_type'], errors='ignore')

    # Save test data
    test_path = os.path.join(data_dir, "test.csv")
    test_for_scoring.to_csv(test_path, index=False)
    print(f"[OK] Test data saved to {test_path}")

    # Save test labels separately for evaluation
    test_labels_path = os.path.join(data_dir, "test_labels.csv")
    test_with_labels[['event_id', 'is_spoofed']].to_csv(test_labels_path, index=False)
    print(f"[OK] Test labels saved to {test_labels_path}")

    print("\n" + "=" * 60)
    print("Data generation complete!")
    print("=" * 60)


if __name__ == "__main__":
    main()
