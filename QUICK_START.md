# 🚀 Quick Start Guide - IWR6843 ESPHome Component

## 📦 Installation (2 Minuten)

### Option 1: Direkt von GitHub (Empfohlen)

Füge einfach diese Zeilen zu deiner ESPHome YAML-Datei hinzu:

```yaml
# Add to the TOP of your YAML file
external_components:
  - source:
      type: git
      url: https://github.com/bytelink-ai/IWR6843
      ref: main
    components: [ iwr6843 ]
```

### Option 2: Mit spezifischer Version

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/bytelink-ai/IWR6843
      ref: v1.1.0  # Specific version tag
    components: [ iwr6843 ]
```

## 📝 Komplette Minimal-Konfiguration

Kopiere diese komplette Konfiguration und passe nur WiFi/Namen an:

```yaml
esphome:
  name: iwr6843-radar
  platform: ESP32
  board: esp32dev

# WiFi anpassen
wifi:
  ssid: "DEIN_WIFI_NAME"
  password: "DEIN_WIFI_PASSWORT"

# Component von GitHub laden
external_components:
  - source:
      type: git
      url: https://github.com/bytelink-ai/IWR6843
      ref: main
    components: [ iwr6843 ]

# Standard ESPHome features
logger:
  level: INFO

api:
ota:

# Hardware Konfiguration
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

# IWR6843 Radar
iwr6843:
  id: my_radar
  spi_id: spi_bus
  uart_id: uart_radar
  cs_pin: GPIO25
  sop2_pin: GPIO33
  nrst_pin: GPIO32
  ceiling_height: 290  # cm (Deckenhöhe anpassen)
  max_tracks: 5
  update_interval: 50ms

# Anwesenheitssensor
binary_sensor:
  - platform: iwr6843
    iwr6843_id: my_radar
    presence:
      name: "Anwesenheit"
      device_class: presence

# Basis-Sensoren
sensor:
  - platform: iwr6843
    iwr6843_id: my_radar
    num_targets:
      name: "Anzahl Personen"
    frame_number:
      name: "Frame Nummer"
```

## 🎯 Erweiterte Konfiguration mit Person-Tracking

Für detailliertes Tracking einzelner Personen:

```yaml
sensor:
  - platform: iwr6843
    iwr6843_id: my_radar
    num_targets:
      name: "Anzahl Personen"
    
    # Tracking für bis zu 5 Personen
    tracks:
      - track_id: 0  # Person 1
        position_x:
          name: "Person 1 X-Position"
        position_y:
          name: "Person 1 Y-Position"
        position_z:
          name: "Person 1 Z-Position"
        velocity_x:
          name: "Person 1 Geschwindigkeit X"
        velocity_y:
          name: "Person 1 Geschwindigkeit Y"
        distance:
          name: "Person 1 Distanz"
      
      - track_id: 1  # Person 2
        position_x:
          name: "Person 2 X-Position"
        position_y:
          name: "Person 2 Y-Position"
        distance:
          name: "Person 2 Distanz"
      
      # ... bis zu track_id: 19 möglich
```

## 🔌 Hardware-Verbindungen

| ESP32 Pin | IWR6843 Pin | Funktion |
|-----------|-------------|----------|
| GPIO13 | PINE14 | SPI CLK |
| GPIO12 | PINF14 | SPI MISO |
| GPIO14 | PING14 | SPI MOSI |
| GPIO25 | PINH14 | SPI CS (mit 10kΩ Pull-up!) |
| GPIO27 | PINN5 | UART TX |
| GPIO26 | PINN4 | UART RX |
| GPIO33 | SOP2 | Boot Mode |
| GPIO32 | NRST | Reset |
| GND | GND | Ground |
| 3.3V | VCC | Power |

**⚠️ WICHTIG:** 10kΩ Pull-up Widerstand zwischen CS Pin (GPIO25) und 3.3V!

## 🔧 Kompilieren und Hochladen

```bash
# Im ESPHome Dashboard
esphome run iwr6843-radar.yaml

# Oder per CLI
esphome compile iwr6843-radar.yaml
esphome upload iwr6843-radar.yaml
```

## 📱 In Home Assistant nutzen

Nach dem Upload erscheinen automatisch:

**Binary Sensoren:**
- `binary_sensor.anwesenheit` - Präsenzerkennung

**Sensoren:**
- `sensor.anzahl_personen` - Anzahl erkannter Personen
- `sensor.person_1_x_position` - Position jeder Person
- `sensor.person_1_distanz` - Entfernung jeder Person
- etc.

## 🔍 Troubleshooting

### Fehler: "Component iwr6843 not found"
→ Prüfe die `external_components` Konfiguration am Anfang der YAML

### Keine Daten vom Radar
→ Prüfe ob die IWR6843 Firmware korrekt geflasht ist (SPI-Version!)

### SPI Fehler
→ Prüfe CS Pull-up Widerstand (10kΩ zu 3.3V)

### "Failed to configure radar"
→ Prüfe UART-Verbindungen (TX/RX nicht vertauscht?)

## 📚 Weitere Dokumentation

- **Vollständige Anleitung:** [README.md](README.md)
- **Setup Guide (Deutsch):** [SETUP_GUIDE.md](SETUP_GUIDE.md)
- **Hardware Setup:** [docs/hardware-setup.md](docs/hardware-setup.md)
- **Beispiele:** `example.yaml` und `example_detailed.yaml`

## 🆘 Support

- **GitHub Issues:** https://github.com/bytelink-ai/IWR6843/issues
- **GitHub Discussions:** https://github.com/bytelink-ai/IWR6843/discussions
- **Repository:** https://github.com/bytelink-ai/IWR6843

## ⚡ Schnelltest

Nach dem Upload, öffne ESPHome Logs:

```
[INFO] IWR6843: Setting up IWR6843...
[INFO] IWR6843: Resetting radar...
[INFO] IWR6843: Configuring radar via UART...
[INFO] IWR6843: IWR6843 setup complete
[INFO] IWR6843: Frame 123: 2 targets, presence: 1
```

Wenn du diese Logs siehst → **Alles funktioniert!** 🎉

---

**Viel Erfolg! Bei Fragen → GitHub Issues oder Discussions nutzen!**

