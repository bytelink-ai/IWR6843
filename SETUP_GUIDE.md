# Setup Guide für IWR6843 mit ESPHome

## Schritt-für-Schritt Anleitung

### 1. Hardware vorbereiten

#### Benötigte Komponenten:
- ✅ ESP32 (z.B. ESP32-DevKit)
- ✅ IWR6843 mit Custom Firmware (SPI Version)
- ✅ 10kΩ Widerstand (Pull-up für CS)
- ✅ Jumper Kabel
- ✅ Breadboard (optional)
- ✅ USB-Kabel für ESP32

#### Verkabelung:

```
ESP32          IWR6843          Beschreibung
--------------------------------------------------
GPIO13    →    PINE14          SPI Clock
GPIO12    ←    PINF14          SPI MISO (Daten)
GPIO14    →    PING14          SPI MOSI
GPIO25    →    PINH14          SPI CS (+ 10kΩ → 3.3V)
GPIO27    →    PINN5           UART TX (Config)
GPIO26    ←    PINN4           UART RX (Config)
GPIO33    →    SOP2            Boot Mode
GPIO32    →    NRST            Reset
GND       →    GND             Common Ground
3.3V      →    VCC             Power
```

**Wichtig:** CS Pin braucht 10kΩ Pull-up zu 3.3V!

### 2. IWR6843 Firmware flashen

#### A. Firmware downloaden:
```bash
# Von GitHub:
wget https://github.com/bytelink-ai/iwr6843-spi-firmware/releases/download/v1.0/overhead_3d_people_track_demo_default.bin
```

#### B. UniFlash installieren:
- Download: https://www.ti.com/tool/UNIFLASH
- Installieren und öffnen

#### C. Flashen:
1. IWR6843 via JTAG verbinden
2. UniFlash > New Configuration > IWR6843
3. Browse zu: `overhead_3d_people_track_demo_default.bin`
4. Load Image
5. Warte auf "Success"
6. Power cycle

### 3. ESPHome Setup

#### A. ESPHome installieren (falls noch nicht):

**Home Assistant Add-on:**
1. Home Assistant > Settings > Add-ons
2. Search: "ESPHome"
3. Install
4. Start

**Standalone:**
```bash
pip install esphome
```

#### B. Component installieren:

**Methode 1: Git (Empfohlen)**

Erstelle `iwr6843-radar.yaml`:

```yaml
esphome:
  name: iwr6843-radar
  platform: ESP32
  board: esp32dev

# External Component von GitHub
external_components:
  - source:
      type: git
      url: https://github.com/bytelink-ai/IWR6843
      ref: main
    components: [ iwr6843 ]

# WiFi
wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password

# Home Assistant API
api:
  encryption:
    key: !secret api_encryption_key

ota:
  password: !secret ota_password

logger:
  level: DEBUG

# SPI Bus
spi:
  id: spi_bus
  clk_pin: GPIO13
  miso_pin: GPIO12
  mosi_pin: GPIO14

# UART für Config
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
  ceiling_height: 290  # Deine Deckenhöhe in cm
  max_tracks: 5

# Presence Detection
binary_sensor:
  - platform: iwr6843
    iwr6843_id: my_radar
    presence:
      name: "Radar Präsenz"
      device_class: presence

# Number of Targets
sensor:
  - platform: iwr6843
    iwr6843_id: my_radar
    num_targets:
      name: "Anzahl Personen"
    frame_number:
      name: "Frame Nummer"
```

**secrets.yaml:**
```yaml
wifi_ssid: "Dein_WiFi"
wifi_password: "Dein_Passwort"
api_encryption_key: "32_zeichen_base64_key"
ota_password: "dein_ota_passwort"
```

#### C. ESP32 flashen:

```bash
# Compile
esphome compile iwr6843-radar.yaml

# Upload via USB
esphome upload iwr6843-radar.yaml

# Oder Over-The-Air (nach erstem Upload):
esphome upload iwr6843-radar.yaml --device iwr6843-radar.local
```

### 4. Home Assistant Integration

#### A. Device automatisch hinzufügen:
1. Home Assistant > Settings > Devices & Services
2. ESPHome Integration sollte "iwr6843-radar" finden
3. Configure und Add

#### B. Entities prüfen:
- `binary_sensor.radar_prasenz` - Präsenzerkennung
- `sensor.anzahl_personen` - Anzahl getrackte Personen
- `sensor.frame_nummer` - Aktuelle Framenummer

### 5. Testing

#### A. Logs überwachen:

```bash
# Via ESPHome CLI:
esphome logs iwr6843-radar.yaml

# Oder in Home Assistant:
Settings > Add-ons > ESPHome > Log
```

**Erwartete Log-Meldungen:**
```
[iwr6843] Setting up IWR6843...
[iwr6843] Resetting radar...
[iwr6843] Configuring radar via UART...
[iwr6843] TX: sensorStop
[iwr6843] TX: flushCfg
...
[iwr6843] Radar configuration complete
[iwr6843] IWR6843 setup complete
[iwr6843] Frame 1: 0 targets, presence: 0
```

#### B. Funktionstest:

1. **Power On:**
   - LED am ESP32 sollte leuchten
   - IWR6843 sollte booten (ca. 1-2 Sekunden)

2. **Configuration:**
   - Logs zeigen "Radar configuration complete"
   - Keine Errors

3. **Detection:**
   - Gehe in Sensorbereich (unter Radar)
   - `binary_sensor.radar_prasenz` sollte `on` werden
   - `sensor.anzahl_personen` sollte `1` zeigen
   - Logs zeigen: `Frame X: 1 targets, presence: 1`

4. **Tracking:**
   - Bewege dich im Raum
   - Frame Number sollte kontinuierlich steigen
   - Target Position sollte sich ändern

### 6. Konfiguration anpassen

#### Ceiling Height:
```yaml
iwr6843:
  ceiling_height: 290  # Deine tatsächliche Deckenhöhe in cm
```

#### Detection Zone:
In `iwr6843.cpp` anpassen:
```cpp
"boundaryBox -2.5 2.5 0.5 6 0 2.8",  // X, Y, Z Grenzen
```

#### Max Targets:
```yaml
iwr6843:
  max_tracks: 5  # Reduzieren spart CPU
```

#### Update Rate:
```yaml
iwr6843:
  update_interval: 50ms  # Erhöhen = weniger CPU Load
```

### 7. Troubleshooting

#### Kein WiFi:
```
[W][wifi] WiFi Connection failed!
```
**Lösung:** WiFi SSID/Passwort in secrets.yaml prüfen

#### SPI Error:
```
[E][iwr6843] Invalid magic word
```
**Lösung:** 
- SPI Kabel prüfen (besonders MISO)
- CS Pull-up vorhanden?
- IWR6843 Firmware korrekt?

#### UART Config Failed:
```
[E][iwr6843] Failed to configure radar
```
**Lösung:**
- UART TX/RX vertauscht?
- Baudrate 115200?
- IWR6843 bootet?

#### Keine Targets:
```
Frame 100: 0 targets, presence: 0
```
**Lösung:**
- ceiling_height anpassen
- boundaryBox vergrößern
- Radar zeigt nach unten?
- Person im Sensorbereich?

### 8. Home Assistant Automationen

#### Licht bei Präsenz:
```yaml
automation:
  - alias: "Büro - Licht bei Präsenz"
    trigger:
      - platform: state
        entity_id: binary_sensor.radar_prasenz
        to: 'on'
    action:
      - service: light.turn_on
        target:
          entity_id: light.buro_decke
        data:
          brightness: 255

  - alias: "Büro - Licht aus ohne Präsenz"
    trigger:
      - platform: state
        entity_id: binary_sensor.radar_prasenz
        to: 'off'
        for:
          minutes: 5
    action:
      - service: light.turn_off
        target:
          entity_id: light.buro_decke
```

#### Notification bei mehreren Personen:
```yaml
automation:
  - alias: "Meeting erkannt"
    trigger:
      - platform: numeric_state
        entity_id: sensor.anzahl_personen
        above: 2
    action:
      - service: notify.mobile_app
        data:
          message: "Meeting läuft - {{ states('sensor.anzahl_personen') }} Personen erkannt"
```

### 9. Performance Optimierung

#### Für mehr Stabilität:
```yaml
iwr6843:
  update_interval: 100ms  # Weniger häufige Updates

logger:
  level: INFO  # Weniger Log-Output
```

#### Für mehr Details:
```yaml
iwr6843:
  update_interval: 50ms

logger:
  level: VERBOSE
  logs:
    iwr6843: VERBOSE
```

### 10. Weiterführende Infos

- **README.md** - Vollständige Dokumentation
- **example.yaml** - Beispiel-Konfiguration
- **ESP32_INTEGRATION.md** - Technische Details
- **GitHub Issues** - Support & Bug Reports

## Quick Start Checklist

- [ ] Hardware verkabelt (mit CS Pull-up!)
- [ ] IWR6843 Firmware geflasht
- [ ] ESPHome YAML erstellt
- [ ] ESP32 geflasht
- [ ] Home Assistant findet Device
- [ ] Logs zeigen "setup complete"
- [ ] Presence Detection funktioniert
- [ ] Targets werden getrackt

**Bei Problemen:** Siehe README.md Troubleshooting Section!

