# Dataset Card: Location Spoofing Detection

## Overview

Synthetic dataset for training and evaluating location spoofing detection models in mobile environments (iOS/Android focus).

## Dataset Statistics

| Split | Size | Spoof Rate | Time Period |
|-------|------|------------|-------------|
| Train | 14,889 events | 17.2% | 2024 (365 days) |
| Test | 4,922 events | 17.8% | 2024 (365 days) |

**Total**: 19,811 location events from ~3,000 simulated devices

## Schema

### Core Location Features

| Field | Type | Range/Values | How SDK Collects | Detection Value |
|-------|------|-------------|------------------|-----------------|
| `event_id` | string | evt_XXXXXX | Auto-generated | Tracking |
| `device_id` | string | dev_[type]_XXXX | Device UDID/hash | Session tracking |
| `timestamp` | ISO datetime | 2024-01-01 to 2024-12-31 | System clock | Temporal analysis |
| `latitude` | float | 25.0 to 49.0 (US bounds) | GPS/Location Services | Geolocation |
| `longitude` | float | -125.0 to -66.0 (US bounds) | GPS/Location Services | Geolocation |
| `accuracy` | float | 1.0 to 50.0 meters | GPS accuracy estimate | Spoofing indicator (perfect accuracy suspicious) |
| `altitude` | float | 0 to 500 meters | GPS/barometer | Movement patterns |
| `speed` | float | 0 to 30 m/s | GPS/derived | Speed validation |
| `bearing` | float | 0 to 360 degrees | GPS/magnetometer | Direction consistency |

**SDK Collection Method**:
- iOS: `CLLocationManager` (requires user permission)
- Android: `FusedLocationProviderClient` or `LocationManager`

**Privacy**: No PII; location data synthetic and anonymized

### Network Features

| Field | Type | Range/Values | How SDK Collects | Detection Value |
|-------|------|-------------|------------------|-----------------|
| `ip_address` | string | 192.168.x.x, 10.0.x.x, 172.16.x.x | Network interface API | IP/Geo mismatch detection |
| `network_type` | string | WiFi, 4G, 5G, 3G | Network connectivity API | Connection context |
| `wifi_count` | int | 0 to 15 | WiFi scan results | Indoor/outdoor inference |
| `cell_tower_count` | int | 1 to 5 | Cellular API | Network availability |

**SDK Collection Method**:
- iOS: `CTTelephonyNetworkInfo`, WiFi via `NEHotspotHelper` (limited)
- Android: `ConnectivityManager`, `WifiManager`, `TelephonyManager`

**Detection Value**: IP geolocation should roughly match GPS coordinates; significant mismatch indicates VPN/proxy use or spoofing

### Sensor Features

| Field | Type | Range/Values | How SDK Collects | Detection Value |
|-------|------|-------------|------------------|-----------------|
| `accelerometer_variance` | float | 0.001 to 2.0 | Motion sensors (variance over window) | Movement/sensor consistency |
| `gyroscope_variance` | float | 0.001 to 1.5 | Gyroscope (variance over window) | Rotation patterns |

**SDK Collection Method**:
- iOS: `CMMotionManager` (Core Motion framework)
- Android: `SensorManager` with `TYPE_ACCELEROMETER`, `TYPE_GYROSCOPE`

**Detection Value**:
- Moving devices should show sensor activity
- Stationary devices have low variance
- Spoofed locations often lack corresponding sensor changes

### Device Features

| Field | Type | Range/Values | How SDK Collects | Detection Value |
|-------|------|-------------|------------------|-----------------|
| `battery_level` | float | 10.0 to 100.0% | Battery status API | Device state context |
| `is_charging` | boolean | true/false | Battery status API | Power state |
| `device_timezone_offset` | int | -8 to -5 (US timezones) | System timezone | Timezone/location consistency |

**SDK Collection Method**:
- iOS: `UIDevice.current.batteryLevel`, `NSTimeZone`
- Android: `BatteryManager`, `TimeZone.getDefault()`

**Detection Value**: Timezone offset should align with longitude (~15° per hour offset)

### Security Indicators

| Field | Type | Range/Values | How SDK Collects | Detection Value |
|-------|------|-------------|------------------|-----------------|
| `mock_location_enabled` | boolean | true/false | Developer settings check | Direct spoofing indicator |
| `developer_options_enabled` | boolean | true/false | System settings check | Risk factor |
| `location_permission` | string | ALWAYS, WHILE_IN_USE | Permission status API | Permission context |

**SDK Collection Method**:
- iOS: `CLLocationManager.authorizationStatus` (mock detection via `isMock` property)
- Android: `Settings.Secure.ALLOW_MOCK_LOCATION` (API <18), behavioral detection (API 18+)

**Detection Value**:
- Mock location = high-confidence spoofing indicator
- Developer options = elevated risk profile

### Ground Truth

| Field | Type | Range/Values | Purpose |
|-------|------|-------------|---------|
| `is_spoofed` | boolean | true/false | Training label |
| `spoof_type` | string | teleportation, ip_geo_mismatch, mock_location, sensor_mismatch, timezone_mismatch | Spoof taxonomy |
| `spoof_note` | string | Descriptive text | Error analysis |

## Spoofing Taxonomy

### 1. Teleportation (20% of spoofs)
**Pattern**: Device appears to jump large distances in impossibly short time

**Indicators**:
- Speed > 150 km/h between consecutive points
- Distance inconsistent with time delta
- Example: NYC to LA in 2 minutes

**Real-world scenario**: User manually sets fake GPS coordinates

### 2. IP/Geo Mismatch (20% of spoofs)
**Pattern**: IP geolocation doesn't match GPS coordinates

**Indicators**:
- IP prefix region (west/east/central) inconsistent with lat/lon
- >5° geographic separation
- Example: GPS in California, IP in New York

**Real-world scenario**: VPN/proxy combined with GPS spoofing

### 3. Mock Location Apps (20% of spoofs)
**Pattern**: Mock location/developer settings enabled

**Indicators**:
- `mock_location_enabled = true`
- Often paired with perfect accuracy (1.0m)
- Developer options enabled

**Real-world scenario**: User installed "Fake GPS" app

### 4. Sensor Mismatch (20% of spoofs)
**Pattern**: Movement without corresponding sensor activity

**Indicators**:
- Speed > 1 m/s but accelerometer/gyro variance < 0.01
- Suggests synthetic location data without real movement
- Example: Driving at 50 km/h with zero acceleration variance

**Real-world scenario**: Software-based spoofing that doesn't simulate sensors

### 5. Timezone Mismatch (20% of spoofs)
**Pattern**: Device timezone doesn't match GPS coordinates

**Indicators**:
- Timezone offset differs from expected (lon/15) by >2 hours
- Example: GPS in PST region but device timezone is EST

**Real-world scenario**: User changed location but not device settings, or timezone spoofing

## Data Generation Process

### Legitimate Events (80-83%)

**Movement Models**:
1. **Walking**: 1-5 km/h speed, high GPS variance (3-10m accuracy)
2. **Driving**: 20-80 km/h speed, moderate variance (5-15m accuracy)

**Realistic Features**:
- Smooth trajectories with natural jitter
- Sensor variance proportional to movement
- IP region matches GPS location
- Timezone consistent with longitude

**Per-Device Tracks**: 5-15 events per device over hours to days

### Spoofed Events (17-20%)

**Injection Strategy**:
- Each spoof type represented equally
- Multiple spoofing artifacts per event common
- Some subtle spoofs (single indicator) to challenge models

**Controlled Realism**:
- Spoofed events mimic common attack patterns
- Not all spoofed events trigger all detectors
- Some legitimate events have edge-case characteristics (near borders, VPN users, developers)

## Dataset Distributions

### Geographic Distribution
- **West Coast (CA, WA, OR)**: 33%
- **East Coast (NY, MA, FL)**: 33%
- **Central (TX, IL, CO)**: 33%

### Network Distribution
- **WiFi**: 35%
- **5G**: 25%
- **4G**: 30%
- **3G**: 10%

### Movement Distribution
- **Stationary** (speed < 0.5 m/s): 15%
- **Walking** (0.5-5 km/h): 35%
- **Driving** (20-80 km/h): 50%

### Time Distribution
- Events spread across full year (2024)
- Random hours (0-23) to avoid temporal bias
- 30s to 5min intervals between events

## Limitations & Biases

### Known Limitations

1. **Synthetic Only**: Not trained on real-world spoofing patterns
   - May miss sophisticated attacks
   - Real GPS noise differs from simulated noise
   - Human behavior patterns simplified

2. **US-Centric**: All locations within US bounds
   - IP/Geo rules specific to US regions
   - Timezone logic assumes Western hemisphere
   - Need retraining for other regions

3. **Simplified Network**:
   - IP addresses private ranges (not real ISP data)
   - No actual IP geolocation database
   - Real VPN patterns more complex

4. **Perfect Ground Truth**: All labels known accurately
   - Real data has labeling uncertainty
   - Edge cases clearer than reality

5. **Missing Patterns**:
   - No WiFi/Bluetooth beacon simulation
   - No cellular tower triangulation
   - No historical user behavior profiles
   - No device fingerprinting

### Potential Biases

1. **Developer Bias**: Dev options enabled always suspicious in data, but 5% of real users have this
2. **Accuracy Bias**: Perfect accuracy (<2m) flagged as suspicious, but modern phones can achieve this
3. **Timezone Bias**: Assumes users don't travel across timezones, but business travelers do
4. **VPN Bias**: IP/Geo mismatch always flagged, but legitimate VPN users exist

### Adversarial Considerations

This dataset does NOT include:
- Sophisticated attacks that mimic sensor data
- Time-delayed spoofing
- Gradual location drift
- Coordinated multi-device spoofing
- Attacks designed to evade these specific rules

**Real-world deployment would require**:
- Adversarial training
- Continuous model updates
- Human review pipeline
- Device fingerprinting
- Historical behavior modeling

## Validation

### Data Quality Checks
- ✓ No missing values
- ✓ Lat/lon within bounds
- ✓ Timestamps chronological per device
- ✓ Speed calculations consistent
- ✓ Label distribution within target range (10-30%)

### Sanity Checks
- ✓ Spoofed events trigger at least one rule
- ✓ Legitimate events mostly clean
- ✓ Geographic distribution balanced
- ✓ Device counts reasonable

## Usage Recommendations

### Training
- Use for prototyping and algorithm development
- Test detection logic before real-world deployment
- Benchmark different approaches (rules vs ML)

### Limitations in Production
- **Must** supplement with real-world data
- **Must** tune for specific use case (gaming, geo-compliance, fraud)
- **Must** consider legitimate edge cases
- **Must** implement human review for edge cases

### Ethical Considerations
- This data is for defensive security research only
- Do not use patterns to build spoofing tools
- Consider user privacy in real implementations
- Avoid discriminatory targeting based on demographics

## Version & Updates

- **Version**: 1.0
- **Generated**: January 2025
- **Generator**: OpenAI-assisted synthetic data pipeline
- **Reproducible**: Random seed 42 fixed

## Contact & Citation

This dataset was created for GeoComply's AI Systems Internship assignment.

For questions or issues, refer to the submission repository.
