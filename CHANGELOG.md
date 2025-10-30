# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [3.0.0] - 2025-10-30

### ⚠️ BREAKING CHANGES
- **Single-UART Mode Support**: Component now supports both Single-UART @ 115200 and Dual-UART modes
- Firmware changed to output both CLI and data on same UART0 @ 115200 baud
- No firmware changes needed if using standard Single-UART mode

### Added
- **Single-UART Mode**: Automatic detection when `config_uart_id` == `data_uart_id`
- **Flexible Configuration**: Works with both UART0 @ 115200 (Single) and Dual-UART setups
- Improved logging to show which mode is active (Single vs Dual-UART)
- Better compatibility with stock IWR6843 firmware configurations

### Changed
- Component now auto-detects Single-UART vs Dual-UART configuration
- Updated firmware guide to show UART0 @ 115200 as primary option
- `dump_config()` now displays "Single-UART @ 115200" or separate UARTs
- Setup logs now indicate active UART mode

### Migration from v2.0.0 (Dual-UART)

**v2.0.0 (Dual-UART @ 115200 + 921600):**
```yaml
uart:
  - id: uart_config
    baud_rate: 115200
  - id: uart_data
    baud_rate: 921600

iwr6843:
  config_uart_id: uart_config
  data_uart_id: uart_data
```

**v3.0.0 (Single-UART @ 115200) - RECOMMENDED:**
```yaml
uart:
  - id: uart_radar
    tx_pin: GPIO27
    rx_pin: GPIO26
    baud_rate: 115200

iwr6843:
  config_uart_id: uart_radar
  data_uart_id: uart_radar  # Same UART!
```

**Benefits of Single-UART:**
- ✅ Only ONE UART port needed
- ✅ Standard 115200 baudrate (reliable)
- ✅ Simpler wiring (no GPIO17 needed)
- ✅ Works with stock firmware
- ✅ CLI and data on same port

**Firmware:**
- Use `xwr68xx_overhead_uart0_115200.bin` for Single-UART mode
- Use custom Dual-UART firmware for 921600 data mode (v2.0.0)

## [2.0.0] - 2025-10-30

### ⚠️ BREAKING CHANGES
- **Complete rewrite from SPI to Dual-UART interface**
- Now uses **standard IWR6843 firmware** (no custom firmware needed!)
- Configuration changed from `spi_id` + `uart_id` to `config_uart_id` + `data_uart_id`
- Removed dependency on SPI - only UART required now

### Added
- **Dual-UART Support**: 
  - Config UART (CLI Port @ 115200) for sending configuration commands
  - Data UART (Data Port @ 921600) for receiving radar data
- **Automatic Configuration**: Config is sent automatically on boot
- **Standard Firmware Compatible**: Works with stock IWR6843 firmware
- New `QUICK_START_UART.md` guide for easy setup
- Improved frame parsing with magic word detection
- Better buffer management for UART data streams

### Changed
- **Hardware connections** (see QUICK_START_UART.md):
  - GPIO27/26: Config UART @ 115200 (CLI Port)
  - GPIO17: Data UART @ 921600 (Data Port - RX only)
  - SOP2/NRST pins now optional
- Component base class changed from `SPIDevice` to plain `Component`
- Frame reading now uses UART streaming instead of SPI polling
- Data parsing optimized for UART byte-by-byte reception

### Removed
- SPI dependency and all SPI-related code
- Custom SPI firmware requirement
- CS pin configuration
- `spi_setup()` and SPI transaction handling

### Migration Guide from v1.x

**Old (SPI-based):**
```yaml
spi:
  id: spi_bus
  clk_pin: GPIO13
  miso_pin: GPIO12
  mosi_pin: GPIO14

uart:
  id: uart_radar
  tx_pin: GPIO27
  rx_pin: GPIO26
  baud_rate: 115200

iwr6843:
  spi_id: spi_bus
  uart_id: uart_radar
  cs_pin: GPIO25
  # ...
```

**New (Dual-UART):**
```yaml
uart:
  - id: uart_config
    tx_pin: GPIO27
    rx_pin: GPIO26
    baud_rate: 115200
  
  - id: uart_data
    tx_pin: GPIO16  # Not used
    rx_pin: GPIO17
    baud_rate: 921600

iwr6843:
  config_uart_id: uart_config
  data_uart_id: uart_data
  # sop2_pin/nrst_pin now optional
  # ...
```

**Benefits:**
- ✅ No custom firmware needed
- ✅ Easier hardware setup (no SPI wiring)
- ✅ Uses standard IWR6843 configuration
- ✅ Config automatically loaded on boot
- ✅ More reliable data reception

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

[Unreleased]: https://github.com/bytelink-ai/IWR6843/compare/v2.0.0...HEAD
[2.0.0]: https://github.com/bytelink-ai/IWR6843/compare/v1.1.1...v2.0.0
[1.1.1]: https://github.com/bytelink-ai/IWR6843/compare/v1.1.0...v1.1.1
[1.1.0]: https://github.com/bytelink-ai/IWR6843/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/bytelink-ai/IWR6843/releases/tag/v1.0.0

