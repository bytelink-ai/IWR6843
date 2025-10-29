# GitHub Repository Setup Guide

## Komplette Anleitung zum Aufsetzen des GitHub Repositories

### 📋 Voraussetzungen

- Git installiert
- GitHub Account
- Projekt lokal im Ordner `esphome-iwr6843`

### 🚀 Schritt 1: GitHub Repository erstellen

#### A. Via GitHub Website:

1. Gehe zu https://github.com/new
2. Repository Name: `esphome-iwr6843`
3. Description: `ESPHome custom component for TI IWR6843 mmWave radar with SPI data interface`
4. Visibility: **Public** (für Community) oder **Private**
5. **NICHT** ankreuzen:
   - ❌ Add a README file
   - ❌ Add .gitignore
   - ❌ Choose a license
   
   (Wir haben bereits alle Dateien lokal!)

6. Click **"Create repository"**

### 🔧 Schritt 2: Lokales Repository initialisieren

```bash
# Navigate zum Projekt
cd esphome-iwr6843

# Git Repository initialisieren
git init

# Branch zu 'main' umbenennen (moderner Standard)
git branch -M main

# Alle Dateien zum Staging hinzufügen
git add .

# Ersten Commit erstellen
git commit -m "Initial commit: ESPHome IWR6843 mmWave Radar Component

Features:
- SPI data interface for IWR6843
- UART configuration interface
- 3D people tracking and presence detection
- Home Assistant integration
- Complete documentation
- GitHub Actions CI/CD
- Issue/PR templates"

# Remote Repository hinzufügen (ersetze 'bytelink-ai' mit deinem GitHub Username!)
git remote add origin https://github.com/bytelink-ai/esphome-iwr6843.git

# Pushen zum GitHub
git push -u origin main
```

### 🔐 Schritt 3: GitHub Actions Secrets (Optional)

Falls du GitHub Actions für automatische Releases nutzen willst:

1. Gehe zu deinem Repository auf GitHub
2. Settings → Secrets and variables → Actions
3. New repository secret:
   - Name: `GITHUB_TOKEN` (wird automatisch bereitgestellt)

### 📝 Schritt 4: Repository Settings konfigurieren

#### A. About Section:
1. Gehe zu Repository Hauptseite
2. Click ⚙️ (Settings Icon) neben "About"
3. Füge hinzu:
   - **Description:** `ESPHome custom component for TI IWR6843 mmWave radar`
   - **Website:** (optional: deine Docs)
   - **Topics:** `esphome`, `home-assistant`, `iwr6843`, `mmwave-radar`, `presence-detection`, `people-tracking`, `spi`, `esp32`
4. ✅ Include in the home page: Releases

#### B. Features aktivieren:
- Settings → General → Features:
  - ✅ Wikis (optional)
  - ✅ Issues
  - ✅ Discussions (empfohlen für Community Support)
  - ✅ Projects (optional)

#### C. Branch Protection (empfohlen):
Settings → Branches → Add rule:
- Branch name pattern: `main`
- ✅ Require a pull request before merging
- ✅ Require status checks to pass before merging
- Select: `validate`, `lint`, `cpp-lint`

### 🏷️ Schritt 5: Ersten Release erstellen

```bash
# Tag erstellen
git tag -a v1.0.0 -m "Release v1.0.0

Initial release of ESPHome IWR6843 component

Features:
- SPI data interface
- 3D people tracking
- Presence detection
- Home Assistant integration"

# Tag pushen
git push origin v1.0.0
```

Das triggert automatisch:
- GitHub Actions Release Workflow
- Erstellung von Release Notes
- Artifact Upload

### 📚 Schritt 6: README personalisieren

Ersetze in allen Dateien `bytelink-ai` mit deinem GitHub Username:

```bash
# Auf Linux/Mac:
find . -type f -name "*.md" -o -name "*.py" -o -name "*.yaml" | \
  xargs sed -i 's/bytelink-ai/DEIN_USERNAME/g'

# Auf Windows (PowerShell):
Get-ChildItem -Recurse -Include *.md,*.py,*.yaml | 
  ForEach-Object {
    (Get-Content $_.FullName) -replace 'bytelink-ai','DEIN_USERNAME' | 
    Set-Content $_.FullName
  }
```

Dann commit und push:
```bash
git add .
git commit -m "Update username references"
git push
```

### 🎨 Schritt 7: Optional - GitHub Pages aktivieren

Für eine schöne Dokumentations-Website:

1. Settings → Pages
2. Source: Deploy from a branch
3. Branch: `main` / `docs` folder
4. Save
5. Deine Docs sind dann verfügbar unter: `https://bytelink-ai.github.io/esphome-iwr6843/`

### 🤝 Schritt 8: Community Features

#### A. Discussions aktivieren:
1. Settings → General → Features
2. ✅ Discussions
3. Setup categories:
   - 💬 General
   - 💡 Ideas
   - 🙏 Q&A
   - 🎉 Show and tell
   - 📣 Announcements

#### B. Labels erstellen:
Settings → Issues → Labels → New label:

```
bug - 🐛 Something isn't working - #d73a4a
enhancement - ✨ New feature or request - #a2eeef
documentation - 📚 Improvements or additions to documentation - #0075ca
good first issue - 👋 Good for newcomers - #7057ff
help wanted - 🙏 Extra attention is needed - #008672
question - ❓ Further information is requested - #d876e3
wontfix - 🚫 This will not be worked on - #ffffff
duplicate - 🔄 This issue or pull request already exists - #cfd3d7
```

### 📊 Schritt 9: Repository Stats & Badges

Füge in README.md ganz oben hinzu:

```markdown
# ESPHome IWR6843

![GitHub release (latest by date)](https://img.shields.io/github/v/release/bytelink-ai/esphome-iwr6843)
![GitHub](https://img.shields.io/github/license/bytelink-ai/esphome-iwr6843)
![GitHub stars](https://img.shields.io/github/stars/bytelink-ai/esphome-iwr6843?style=social)
![GitHub issues](https://img.shields.io/github/issues/bytelink-ai/esphome-iwr6843)
![Build Status](https://img.shields.io/github/actions/workflow/status/bytelink-ai/esphome-iwr6843/validate.yml)
```

### 🔄 Schritt 10: Zukünftige Updates

Für weitere Änderungen:

```bash
# Änderungen machen
git add .
git commit -m "Deine Commit Message"
git push

# Für neuen Release:
git tag -a v1.1.0 -m "Release v1.1.0"
git push origin v1.1.0
```

### ✅ Checklist: Repository fertig?

- [ ] Repository auf GitHub erstellt
- [ ] Alle Dateien gepusht
- [ ] Username überall ersetzt
- [ ] About Section konfiguriert
- [ ] Topics hinzugefügt
- [ ] Branch Protection aktiviert (optional)
- [ ] Ersten Release (v1.0.0) erstellt
- [ ] README Badges hinzugefügt
- [ ] Discussions aktiviert (optional)
- [ ] Labels erstellt

### 📢 Schritt 11: Repository bewerben

#### Community Announcement:
1. **ESPHome Discord:** #custom-components channel
2. **Home Assistant Community:** Forum Post
3. **Reddit:** r/homeassistant, r/esp32
4. **ESPHome GitHub:** Überlege PR für "External Components" Liste

#### Beispiel Forum Post:
```markdown
**Titel:** [Custom Component] IWR6843 mmWave Radar for ESPHome

Hi everyone!

I've created an ESPHome custom component for the TI IWR6843 mmWave radar 
with SPI data interface.

Features:
- 3D people tracking and counting
- Presence detection
- Height estimation
- Home Assistant integration

GitHub: https://github.com/bytelink-ai/esphome-iwr6843
Documentation: Full setup guide included

Feedback welcome!
```

### 🆘 Troubleshooting

#### Problem: Push rejected (non-fast-forward)
```bash
# Pull zuerst
git pull origin main --rebase
git push origin main
```

#### Problem: Git credentials
```bash
# SSH Setup (empfohlen):
ssh-keygen -t ed25519 -C "your_email@example.com"
# Public Key zu GitHub → Settings → SSH Keys hinzufügen

# Dann:
git remote set-url origin git@github.com:bytelink-ai/esphome-iwr6843.git
```

#### Problem: GitHub Actions schlagen fehl
- Check Logs in: Actions Tab auf GitHub
- Meist: ESPHome Version compatibility
- Fix: Update `.github/workflows/validate.yml`

### 📊 Repository Struktur (Final)

```
esphome-iwr6843/
├── .github/
│   ├── workflows/
│   │   ├── validate.yml          # CI for validating component
│   │   └── release.yml           # Auto-release workflow
│   ├── ISSUE_TEMPLATE/
│   │   ├── bug_report.yml        # Bug report template
│   │   └── feature_request.yml   # Feature request template
│   ├── PULL_REQUEST_TEMPLATE.md  # PR template
│   ├── FUNDING.yml               # Sponsorship info
│   └── dependabot.yml            # Dependency updates
├── components/
│   └── iwr6843/
│       ├── __init__.py           # Python component init
│       ├── iwr6843.h             # C++ header
│       ├── iwr6843.cpp           # C++ implementation
│       ├── binary_sensor.py      # Presence sensor
│       ├── sensor.py             # Number sensors
│       └── manifest.json         # Component metadata
├── docs/
│   └── hardware-setup.md         # Hardware guide
├── example.yaml                  # Example configuration
├── README.md                     # Main documentation
├── SETUP_GUIDE.md               # Step-by-step setup
├── CONTRIBUTING.md              # Contribution guidelines
├── CODE_OF_CONDUCT.md           # Code of conduct
├── CHANGELOG.md                 # Version history
├── SECURITY.md                  # Security policy
├── LICENSE                      # MIT License
├── VERSION                      # Current version
├── .gitignore                   # Git ignore rules
├── .gitattributes              # Git attributes
├── .editorconfig               # Editor config
├── .clang-format               # C++ formatting
├── .pylintrc                   # Python linting
└── pyproject.toml              # Python project config
```

### 🎉 Fertig!

Dein professionelles GitHub Repository ist jetzt komplett aufgesetzt mit:
✅ CI/CD Pipeline
✅ Issue/PR Templates
✅ Community Guidelines
✅ Professional Documentation
✅ Version Management
✅ Code Quality Tools

**Next:** Teile den Link mit der Community! 🚀

