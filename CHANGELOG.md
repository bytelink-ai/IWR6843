# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [1.1.1] - 2025-10-30

### Fixed
- **Watchdog Timeout**: Fixed ESP32 task watchdog timeout during radar configuration
  - Added `App.feed_wdt()` calls during long configuration process
  - Reduced delay between config commands from 50ms to 20ms
  - Added watchdog feeding in UART response waiting loop
  - Split 1000ms boot wait into 10x100ms chunks with WDT feeding
- **ESPHome 2025.x Compatibility**: Fixed missing `UNIT_METER_PER_SECOND` constant
- **CS Pin Registration**: Fixed duplicate GPIO pin registration error
- **Binary Sensor Callback**: Fixed Python lambda to C++ lambda conversion

### Changed
- Configuration timeout reduced from 500ms to 300ms per command
- Boot wait now includes progress logging
- Improved error handling during setup phase

## [1.1.0] - 2025-10-30

### Added
- Detailed person tracking sensors for individual targets
- Position sensors (X, Y, Z) for each tracked person (in cm)
- Velocity sensors (VX, VY, VZ) for each tracked person (in m/s)
- Distance sensor for each tracked person calculated from 3D position
- Support for up to 20 simultaneous tracks with individual sensors
- New `example_detailed.yaml` with comprehensive sensor configuration
- Automatic calculation of 3D distance from position data
- Track-specific sensor publishing with NaN for absent tracks

### Changed
- Enhanced `sensor.py` to support track-specific sensor configuration
- Updated C++ implementation (`iwr6843.cpp`) to publish track data to individual sensors
- Improved sensor organization with track_id based grouping
- Added `<map>` include for C++ STL map support

### Technical Details
- Track sensor mapping: 0=X, 1=Y, 2=Z, 3=VX, 4=VY, 5=VZ, 6=Distance
- Each track can have any combination of the 7 sensor types
- Sensors publish NaN when track is not detected
- Frame-by-frame updates for all configured track sensors

## [1.0.0] - 2025-10-30

### Added
- SPI data interface support for IWR6843
- UART configuration interface
- 3D people tracking and counting
- Presence detection binary sensor
- Height detection support
- Home Assistant integration
- Configurable ceiling height
- Configurable max tracks
- Automatic radar configuration via UART
- Full documentation and examples

### Features
- Real-time presence detection
- Multiple target tracking (up to 20 targets)
- 3D position and velocity tracking
- Height estimation for tracked targets
- Adjustable update intervals
- Comprehensive error handling
- Detailed logging support

### Hardware Support
- ESP32 (all variants)
- IWR6843 mmWave radar with custom SPI firmware
- Tested with ESP32-DevKit, ESP32-WROOM-32

### Documentation
- Complete README with hardware setup
- Step-by-step SETUP_GUIDE
- Example configurations
- Troubleshooting guide
- Contributing guidelines
- Code of conduct

[Unreleased]: https://github.com/bytelink-ai/IWR6843/compare/v1.1.1...HEAD
[1.1.1]: https://github.com/bytelink-ai/IWR6843/compare/v1.1.0...v1.1.1
[1.1.0]: https://github.com/bytelink-ai/IWR6843/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/bytelink-ai/IWR6843/releases/tag/v1.0.0

