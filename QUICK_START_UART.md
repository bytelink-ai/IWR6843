# 🚀 Quick Start - IWR6843 ESPHome mit Dual-UART

## 📦 Hardware-Verbindungen

### IWR6843 → ESP32

| ESP32 Pin | IWR6843 Pin | Funktion | Beschreibung |
|-----------|-------------|----------|--------------|
| **GPIO27** | PINN5 (PADBE) | **Config TX** | CLI Port - Config senden (115200) |
| **GPIO26** | PINN4 (PADBD) | **Config RX** | CLI Port - Config Antwort (115200) |
| **GPIO17** | PINN13 (PADBL) | **Data RX** | Data Port - Radar-Daten (921600) |
| **GPIO33** | SOP2 | Boot Mode | Optional: Boot mode select |
| **GPIO32** | NRST | Reset | Optional: Reset pin |
| **GND** | GND | Ground | Common ground |
| **3.3V** | VCC | Power | 3.3V Power |

### ⚡ Wichtig:
- **Config UART (GPIO27/26)**: CLI Port @ 115200 Baud - für Konfiguration
- **Data UART (GPIO17)**: Data Port @ 921600 Baud - für Radar-Daten
- GPIO17 ist **nur RX** (Empfang) - TX wird nicht verwendet

## 📝 Komplette ESPHome Konfiguration

```yaml
esphome:
  name: iwr6843-radar
  platform: ESP32
  board: esp32dev

wifi:
  ssid: "DEIN_WIFI"
  password: "DEIN_PASSWORT"

logger:
  level: DEBUG

api:
ota:

# Dual-UART Konfiguration
uart:
  # CLI Port: Für Config-Befehle @ 115200
  - id: uart_config
    tx_pin: GPIO27
    rx_pin: GPIO26
    baud_rate: 115200
  
  # Data Port: Für Radar-Daten @ 921600
  - id: uart_data
    tx_pin: GPIO16  # Nicht verwendet, aber erforderlich
    rx_pin: GPIO17  # Empfang der Radar-Daten
    baud_rate: 921600

# IWR6843 Component von GitHub laden
external_components:
  - source:
      type: git
      url: https://github.com/bytelink-ai/IWR6843
      ref: main
    components: [ iwr6843 ]

# IWR6843 Radar konfigurieren
iwr6843:
  id: my_radar
  config_uart_id: uart_config  # CLI Port
  data_uart_id: uart_data      # Data Port
  sop2_pin: GPIO33             # Optional
  nrst_pin: GPIO32             # Optional
  ceiling_height: 290          # Deckenhöhe in cm
  max_tracks: 5                # Max. Personen
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
    
    # Optional: Detailliertes Tracking
    tracks:
      - track_id: 0
        position_x:
          name: "Person 1 X"
        position_y:
          name: "Person 1 Y"
        position_z:
          name: "Person 1 Z"
        distance:
          name: "Person 1 Distanz"
```

## 🔧 Installation

1. **ESPHome YAML erstellen** (siehe oben)
2. **Kompilieren und flashen:**

```bash
esphome run iwr6843.yaml
```

3. **In Home Assistant:**
   - ESPHome Integration öffnen
   - Device sollte automatisch erscheinen
   - Sensoren sind sofort verfügbar!

## 📊 Was passiert beim Boot

```
[INFO] Setting up IWR6843...
[INFO] Resetting radar...
[INFO] Waiting for radar boot...
[INFO] Starting radar configuration...
[DEBUG] TX: sensorStop
[DEBUG] TX: flushCfg
...
[DEBUG] TX: sensorStart
[INFO] Radar configuration complete
[INFO] IWR6843 setup complete
[INFO] Starting UART data polling (interval: 50 ms, baud: 921600)...
[DEBUG] Frame 1: 2 targets, presence: 1
```

## 🎯 Erwartete Sensoren in Home Assistant

### Binary Sensors
- `binary_sensor.anwesenheit` - Präsenzerkennung

### Sensors
- `sensor.anzahl_personen` - Wie viele Personen erkannt wurden
- `sensor.frame_nummer` - Frame-Zähler
- `sensor.person_1_x` - X-Position Person 1 (cm)
- `sensor.person_1_y` - Y-Position Person 1 (cm)
- `sensor.person_1_z` - Z-Position Person 1 (cm)
- `sensor.person_1_distanz` - Distanz Person 1 (cm)
- ... (bis zu 5 Personen)

## ⚠️ Troubleshooting

### Keine Daten empfangen

**Symptome:**
```
[INFO] IWR6843 setup complete
[INFO] Starting UART data polling...
# Keine weiteren Logs
```

**Lösungen:**
1. Prüfe GPIO17 Verbindung zum IWR6843 Data Port (PINN13/PADBL)
2. Prüfe Baudrate: Data Port **muss** 921600 sein!
3. Prüfe Ground-Verbindung
4. Aktiviere UART-Debug:

```yaml
uart:
  - id: uart_data
    rx_pin: GPIO17
    baud_rate: 921600
    debug:  # Debug hinzufügen
      direction: RX
      dummy_receiver: true
```

### Config-Befehle funktionieren nicht

**Symptome:**
```
[ERROR] Failed to send config: sensorStop
```

**Lösungen:**
1. Prüfe GPIO27/26 Verbindung zum CLI Port (PINN5/PINN4)
2. Baudrate 115200 für Config-UART prüfen
3. TX/RX nicht vertauscht?

### Radar bootet nicht

**Symptome:**
- Keine Antwort auf Config-Befehle
- Timeout beim Konfigurieren

**Lösungen:**
1. Prüfe 3.3V Power-Versorgung
2. Prüfe NRST und SOP2 Pins (falls verbunden)
3. Manuelle Reset-Sequenz:
   - NRST low → 100ms warten → NRST high
   - 1 Sekunde warten
   - Config senden

## 📚 Weiterführende Infos

- **Full README:** [README.md](README.md)
- **Hardware Setup:** [docs/hardware-setup.md](docs/hardware-setup.md)
- **GitHub:** https://github.com/bytelink-ai/IWR6843
- **Issues:** https://github.com/bytelink-ai/IWR6843/issues

## 🎉 Erfolg!

Wenn du diese Logs siehst, funktioniert alles:
```
[DEBUG] Frame 1: 2 targets, presence: 1
[DEBUG] Target 0: pos=(1.23, 2.34, 0.56), vel=(0.01, 0.02, 0.00)
```

**Viel Erfolg mit deinem IWR6843 Radar!** 🚀

