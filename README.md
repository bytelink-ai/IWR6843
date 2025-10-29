# ESPHome IWR6843 mmWave Radar Component

ESPHome custom component for Texas Instruments IWR6843 mmWave radar sensor with SPI data interface.

## Features

- ✅ **SPI Data Interface** - High-speed data transfer from IWR6843 (slave) to ESP32 (master)
- ✅ **UART Configuration** - Send config commands via UART
- ✅ **3D People Tracking** - Track multiple targets in 3D space
- ✅ **Presence Detection** - Binary sensor for occupancy detection
- ✅ **Height Detection** - Ceiling-mounted overhead tracking
- ✅ **Home Assistant Integration** - Full ESPHome/HA support

## Hardware Requirements

### Components
- ESP32 development board
- IWR6843 mmWave radar (with custom firmware - see below)
- Connecting wires
- 10kΩ pull-up resistor for CS pin (recommended)

### Pin Connections

| ESP32 Pin | IWR6843 Pin | Function |
|-----------|-------------|----------|
| GPIO13 | PINE14 (PADAC) | SPI CLK |
| GPIO12 | PINF14 (PADAJ) | SPI MISO (Data from radar) |
| GPIO14 | PING14 (PADAK) | SPI MOSI (Data to radar) |
| GPIO25 | PINH14 (PADAL) | SPI CS (with 10kΩ pull-up) |
| GPIO27 | PINN5 (PADBE) | UART TX (ESP → Radar) |
| GPIO26 | PINN4 (PADBD) | UART RX (Radar → ESP) |
| GPIO33 | SOP2 | Boot mode select |
| GPIO32 | NRST | Reset |
| GND | GND | Common ground |
| 3.3V | VCC | Power (check current requirements) |

**Important:** Add a 10kΩ pull-up resistor between CS pin and 3.3V!

## IWR6843 Firmware

This component requires custom firmware on the IWR6843 that outputs data via SPI instead of UART. 

### Firmware Source
Get the modified firmware from the companion repository:
- **Firmware Repository:** [iwr6843-spi-firmware](https://github.com/bytelink-ai/iwr6843-spi-firmware)
- **Binary File:** `overhead_3d_people_track_demo_default.bin`

### Flashing Instructions

1. **Install UniFlash:**
   - Download from [TI Uniflash](https://www.ti.com/tool/UNIFLASH)

2. **Flash the firmware:**
   ```bash
   # Connect IWR6843 via JTAG
   # Open UniFlash
   # Select IWR6843 device
   # Load: overhead_3d_people_track_demo_default.bin
   # Flash Image
   ```

3. **Verify:**
   - Power cycle the radar
   - Check that it boots correctly

## Installation

### Method 1: Git Clone (Recommended)

```yaml
# In your ESPHome YAML file:
external_components:
  - source:
      type: git
      url: https://github.com/bytelink-ai/esphome-iwr6843
      ref: main
    components: [ iwr6843 ]
```

### Method 2: Local Development

```bash
# Clone to your ESPHome config directory
cd /config/esphome
git clone https://github.com/bytelink-ai/esphome-iwr6843.git

# In your YAML:
external_components:
  - source:
      type: local
      path: esphome-iwr6843/components
    components: [ iwr6843 ]
```

## Configuration

### Basic Example

```yaml
# SPI Configuration
spi:
  id: spi_bus
  clk_pin: GPIO13
  miso_pin: GPIO12
  mosi_pin: GPIO14

# UART Configuration
uart:
  id: uart_radar
  tx_pin: GPIO27
  rx_pin: GPIO26
  baud_rate: 115200

# IWR6843 Component
iwr6843:
  id: my_radar
  spi_id: spi_bus
  uart_id: uart_radar
  cs_pin: GPIO25
  sop2_pin: GPIO33
  nrst_pin: GPIO32
  ceiling_height: 290  # cm
  max_tracks: 5
  update_interval: 50ms

# Presence Detection
binary_sensor:
  - platform: iwr6843
    iwr6843_id: my_radar
    presence:
      name: "Room Presence"

# Number of Targets
sensor:
  - platform: iwr6843
    iwr6843_id: my_radar
    num_targets:
      name: "Number of People"
```

### Configuration Options

#### Main Component

| Option | Type | Required | Default | Description |
|--------|------|----------|---------|-------------|
| `spi_id` | ID | Yes | - | SPI bus ID |
| `uart_id` | ID | Yes | - | UART component ID |
| `cs_pin` | Pin | Yes | - | SPI chip select pin |
| `sop2_pin` | Pin | Yes | - | Boot mode select pin |
| `nrst_pin` | Pin | Yes | - | Reset pin |
| `ceiling_height` | int | No | 250 | Ceiling height in cm (100-500) |
| `max_tracks` | int | No | 5 | Maximum targets to track (1-20) |
| `update_interval` | time | No | 50ms | SPI read interval |

#### Binary Sensor

```yaml
binary_sensor:
  - platform: iwr6843
    iwr6843_id: my_radar
    presence:
      name: "Presence"
      device_class: presence
      # All standard binary_sensor options...
```

#### Sensor

```yaml
sensor:
  - platform: iwr6843
    iwr6843_id: my_radar
    num_targets:
      name: "Number of Targets"
    frame_number:
      name: "Frame Number"
```

## Data Format

The component receives data in TLV (Type-Length-Value) format via SPI:

### Frame Structure
```
Header (32 bytes):
- Magic Word: 0x0102 0x0304 0x0506 0x0708
- Platform: 0xA6843
- Frame Number
- Subframe Number
- Number of TLVs
- Total Packet Length

TLV Blocks:
- Point Cloud (optional)
- Target List (3D positions, velocities)
- Target Heights
- Presence Indication
```

### Target Data
Each tracked target includes:
- **Position:** X, Y, Z (meters)
- **Velocity:** VX, VY, VZ (m/s)
- **Acceleration:** AX, AY, AZ (m/s²)
- **Height:** Estimated height (cm)
- **TID:** Unique tracking ID

## Home Assistant Integration

The component automatically integrates with Home Assistant via ESPHome:

### Entities Created

1. **Binary Sensor:** `binary_sensor.room_presence`
   - State: `on` (presence detected) / `off` (no presence)
   - Device class: presence

2. **Sensor:** `sensor.number_of_people`
   - Value: Number of tracked targets (0-5)

3. **Sensor:** `sensor.frame_number`
   - Value: Current frame number from radar

### Automations

```yaml
# Example: Turn on lights when presence detected
automation:
  - alias: "Room Presence - Lights On"
    trigger:
      - platform: state
        entity_id: binary_sensor.room_presence
        to: 'on'
    action:
      - service: light.turn_on
        target:
          entity_id: light.room_lights
```

## Troubleshooting

### No Data Received

**Symptom:** No presence detection, num_targets always 0

**Solutions:**
1. Check SPI connections (especially MISO from radar to ESP32)
2. Verify CS pin has pull-up resistor
3. Check radar firmware is correct SPI version
4. Monitor logs for SPI errors:
   ```
   logger:
     level: VERBOSE
     logs:
       iwr6843: VERBOSE
   ```

### Invalid Magic Word

**Symptom:** Log shows "Invalid magic word"

**Solutions:**
1. Radar not configured - check UART connection
2. SPI timing issues - try lower SPI speed
3. Check that radar is in functional mode (SOP2 high)

### Configuration Failed

**Symptom:** "Failed to configure radar"

**Solutions:**
1. Check UART TX/RX not swapped
2. Verify baud rate is 115200
3. Check UART pins have proper pull-ups
4. Monitor UART traffic to see responses

### Targets Not Detected

**Symptom:** Radar works but doesn't detect people

**Solutions:**
1. Adjust `ceiling_height` to match installation
2. Check `boundaryBox` in config matches room size
3. Verify radar is mounted overhead facing down
4. Check person is within detection range (6m max)

### ESP32 Resets/Crashes

**Symptom:** Watchdog resets, crashes

**Solutions:**
1. Increase `update_interval` (reduce SPI polling)
2. Check power supply can handle ESP32 + radar
3. Reduce log level from DEBUG/VERBOSE
4. Check for memory issues (buffer overflow)

## Performance

- **Frame Rate:** 20 fps (50ms per frame)
- **Update Rate:** Configurable, default 50ms
- **Latency:** <100ms from detection to HA
- **CPU Usage:** ~5% on ESP32 @ 240MHz
- **Memory:** ~8KB RAM for buffers

## Advanced Usage

### Custom Config Commands

You can modify the radar configuration in `iwr6843.cpp`:

```cpp
static const char *CONFIG_COMMANDS[] = {
    // Modify these commands for your use case
    "boundaryBox -3 3 0.5 6 0 3",  // Detection zone
    "sensorPosition 2.8 0 0",      // Sensor position
    // ...
};
```

### Access Target Data

```cpp
// In your custom code:
auto radar = id(my_radar);
if (radar->get_num_targets() > 0) {
  const auto& targets = radar->get_targets();
  for (const auto& target : targets) {
    ESP_LOGI("app", "Target at (%.2f, %.2f, %.2f)", 
             target.pos_x, target.pos_y, target.pos_z);
  }
}
```

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Submit a pull request

## License

MIT License - see LICENSE file

## Credits

- **IWR6843 Firmware:** Based on TI mmWave SDK and Radar Toolbox
- **ESPHome:** https://esphome.io
- **TI E2E Forums:** For SPI configuration guidance

## Support

- **Issues:** https://github.com/bytelink-ai/esphome-iwr6843/issues
- **Discussions:** https://github.com/bytelink-ai/esphome-iwr6843/discussions
- **TI E2E Forum:** https://e2e.ti.com

## References

- [IWR6843 Datasheet](https://www.ti.com/product/IWR6843)
- [mmWave SDK Documentation](https://www.ti.com/tool/MMWAVE-SDK)
- [ESPHome Custom Components](https://esphome.io/custom/custom_component.html)

