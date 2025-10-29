# Hardware Setup Guide

## Required Components

### Main Components
- **ESP32 Development Board**
  - ESP32-DevKit (recommended)
  - ESP32-WROOM-32
  - ESP32-S2/S3/C3 (experimental)
  - Minimum 4MB flash

- **IWR6843 mmWave Radar**
  - With custom SPI firmware
  - Must be flashed with overhead tracking firmware
  - See [Firmware Guide](firmware-guide.md)

### Additional Components
- 10kΩ resistor (for CS pull-up)
- Jumper wires (male-to-male or male-to-female)
- Breadboard (optional, for prototyping)
- USB cable for ESP32
- Power supply (5V, min. 2A recommended)

## Pin Connections

### Standard Configuration

| ESP32 GPIO | Function | IWR6843 Pin | IWR6843 Function | Notes |
|------------|----------|-------------|------------------|-------|
| GPIO13 | SPI CLK | PINE14 (PADAC) | MSS_SPIA_CLK | Clock signal |
| GPIO12 | SPI MISO | PINF14 (PADAJ) | MSS_SPIA_MISO | Data from radar |
| GPIO14 | SPI MOSI | PING14 (PADAK) | MSS_SPIA_MOSI | Data to radar |
| GPIO25 | SPI CS | PINH14 (PADAL) | MSS_SPIA_CS0 | **Needs 10kΩ pull-up!** |
| GPIO27 | UART TX | PINN5 (PADBE) | MSS_UARTA_RX | Config commands |
| GPIO26 | UART RX | PINN4 (PADBD) | MSS_UARTA_TX | Config responses |
| GPIO33 | Control | SOP2 | Boot Mode | High = functional |
| GPIO32 | Control | NRST | Reset | Active low |
| GND | Ground | GND | Ground | Common ground |
| 3.3V | Power | VCC | Power | Check current rating |

### Wiring Diagram

```
                   ┌─────────────┐
                   │   ESP32     │
                   │             │
    GPIO13 ────────┤ CLK    MISO ├──────── GPIO12
                   │             │
    GPIO14 ────────┤ MOSI     CS ├──────── GPIO25 ──┐
                   │             │                   │
    GPIO27 ────────┤ TX       RX ├──────── GPIO26   │
                   │             │                  ┌┴┐
    GPIO33 ────────┤ SOP2   NRST ├──────── GPIO32  │ │ 10kΩ
                   │             │                  └┬┘
    GND    ────────┤ GND     3.3V├──────── 3.3V     │
                   └─────────────┘                   │
                         │                           │
                         │                          3.3V
                         │
                   ┌─────────────┐
                   │  IWR6843    │
                   │             │
    PINE14 ────────┤ CLK    MISO ├──────── PINF14
    (PADAC)        │             │         (PADAJ)
                   │             │
    PING14 ────────┤ MOSI     CS ├──────── PINH14
    (PADAK)        │             │         (PADAL)
                   │             │
    PINN5  ────────┤ RX       TX ├──────── PINN4
    (PADBE)        │             │         (PADBD)
                   │             │
    SOP2   ────────┤ SOP2   NRST ├──────── NRST
                   │             │
    GND    ────────┤ GND     VCC ├──────── VCC
                   └─────────────┘
```

## Important Notes

### CS Pin Pull-up
**Critical:** The CS (Chip Select) pin MUST have a 10kΩ pull-up resistor to 3.3V!

Without this resistor:
- SPI communication may fail
- First byte might repeat
- Unstable data transfer

Connection:
```
GPIO25 ───┬──── PINH14 (CS)
          │
         ┌┴┐
         │ │ 10kΩ
         └┬┘
          │
         3.3V
```

### Power Requirements

**ESP32:**
- Voltage: 3.3V logic, 5V USB power
- Current: ~80-260mA (depending on WiFi usage)

**IWR6843:**
- Voltage: 3.3V
- Current: ~600mA peak, ~300mA average
- **Total: Min. 1A power supply recommended (2A safer)**

### Voltage Levels
- Both ESP32 and IWR6843 use 3.3V logic
- No level shifters needed
- Can share common 3.3V power rail (if supply is adequate)
- **Do NOT connect 5V directly to IWR6843!**

## Alternative Pin Configurations

You can change the pins in your YAML:

```yaml
spi:
  clk_pin: GPIO18  # Custom clock pin
  miso_pin: GPIO19 # Custom MISO pin
  mosi_pin: GPIO23 # Custom MOSI pin

iwr6843:
  cs_pin: GPIO5    # Custom CS pin (still needs pull-up!)
  sop2_pin: GPIO22
  nrst_pin: GPIO21
```

**Available ESP32 GPIOs:**
- Input/Output: 0, 2, 4, 5, 12-19, 21-23, 25-27, 32-39
- Avoid: GPIO 6-11 (connected to flash)
- Boot pins: 0, 2, 12, 15 (be careful with these)

## Physical Mounting

### Radar Placement
- Mount overhead, facing down
- Typical height: 2.5-3.0m
- Coverage: ~6m radius at 3m height
- Tilt: 0° (straight down) recommended

### ESP32 Placement
- Can be separate from radar
- Use quality cables for SPI (max ~30cm)
- Keep SPI wires away from power cables
- Shorter wires = more reliable

## Testing Connections

### Multimeter Checks
Before powering on:

1. **Power Rails:**
   - 3.3V to GND: Should read 3.3V
   - No shorts between VCC and GND

2. **CS Pull-up:**
   - CS to 3.3V: Should read ~3.3V (pulled high)
   - Resistance: Should measure ~10kΩ

3. **Connections:**
   - Continuity test all signal wires
   - Check for shorts between adjacent pins

### Initial Power-on
1. Connect only ESP32 first
2. Check 3.3V rail is stable
3. Connect IWR6843
4. Check no excessive heat
5. Upload firmware

## Common Wiring Mistakes

### ❌ Wrong
- Swapped TX/RX on UART
- Missing CS pull-up resistor
- Using 5V for IWR6843
- Long/unshielded SPI wires

### ✅ Correct
- TX → RX, RX → TX
- 10kΩ pull-up on CS
- 3.3V for both devices
- Short, shielded SPI wires

## Troubleshooting

### No SPI Communication
- Check MISO connection (most common)
- Verify CS pull-up present
- Test with logic analyzer if available

### UART Not Working
- Swap TX/RX (easy mistake)
- Check baud rate (115200)
- Verify pin numbers in config

### Power Issues
- Check current rating of supply
- Measure actual voltage at IWR6843
- Add bulk capacitor (100µF) if needed

### Intermittent Failures
- Check all ground connections
- Shorten SPI wires
- Add decoupling capacitors

## Next Steps

After hardware setup:
1. [Flash IWR6843 Firmware](firmware-guide.md)
2. [Configure ESPHome](../SETUP_GUIDE.md)
3. [Test and Verify](testing-guide.md)

