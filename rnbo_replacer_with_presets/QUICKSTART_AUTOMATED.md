# Quick Start - Replacer Automatizzato con Buffer Support

## 🚀 Uso Rapido

Il replacer è ora completamente automatizzato per il buffer support! Basta eseguire:

```bash
cd rnbo_replacer_with_presets
python replacer.py
```

## ✅ Cosa Viene Automatizzato

### 1. Encoding JNI Corretto
- Package names con underscore (es. `rnbo_test`) vengono codificati correttamente
- `com.example.rnbo_test` → `com_example_rnbo_1test`
- **Nessuna modifica manuale richiesta!**

### 2. AssetManager Integration
- MainActivity chiama automaticamente `init(getAssets())`
- native-lib.cpp genera con il parametro AssetManager
- MSPLoader riceve l'AssetManager per caricare i buffer
- **Nessuna modifica manuale richiesta!**

### 3. BufferLoader Generation
- `BufferLoader.h` e `BufferLoader.cpp` generati automaticamente
- Parser AIFF completo con supporto big-endian
- Integrazione con RNBO buffer~ objects
- **Nessuna modifica manuale richiesta!**

### 4. File Assets
- `dependencies.json` copiato automaticamente in `assets/`
- Path corretti con forward slash `/`
- File audio copiati dalla cartella `media/`
- **Nessuna modifica manuale richiesta!**

## 📋 Prerequisiti

Assicurati di avere nella cartella `export/`:

```
export/
├── description.json          # Generato da RNBO export
├── dependencies.json         # Contiene mapping buffer → file
├── rnbo/                    # Cartella RNBO C++ code
└── media/                   # File audio (es. drumLoop.aif)
    └── drumLoop.aif
```

### Formato dependencies.json

```json
[
  {
    "id": "drums",
    "file": "media/drumLoop.aif"
  }
]
```

**IMPORTANTE**: Usa sempre forward slash `/` nei path, mai backslash `\`!

## 🔧 Esecuzione

```bash
# 1. Naviga nella cartella del replacer
cd rnbo_replacer_with_presets

# 2. Verifica che export/ contenga tutti i file
ls -la ../export/

# 3. Esegui il replacer
python replacer.py

# 4. Il replacer mostrerà:
#    ✓ Step 1: Copia rnbo folder
#    ✓ Step 2: Copia export cpp files
#    ✓ Step 3: Generate C++ files (BufferLoader, MSPLoader, native-lib)
#    ✓ Step 4: Copia JSON files in assets (description.json + dependencies.json)
#    ✓ Step 5: Sostituisce rnbo folder
#    ✓ Step 6: Copia media files
#    ✓ Step 7: Generate MainActivity.java
#    ✓ Step 8: Update build files
```

## ✅ Verifica Post-Esecuzione

```bash
# 1. Verifica dependencies.json in assets
cat Max-MSP-RNBO-CPP-Native-Android-master/app/src/main/assets/dependencies.json

# 2. Verifica file audio
ls -lh Max-MSP-RNBO-CPP-Native-Android-master/app/src/main/assets/media/

# 3. Verifica encoding JNI in native-lib.cpp
grep "Java_com_example" Max-MSP-RNBO-CPP-Native-Android-master/app/src/main/cpp/native-lib.cpp
# Deve mostrare: Java_com_example_rnbo_1test_...
```

## 🏗️ Build e Test

```bash
cd Max-MSP-RNBO-CPP-Native-Android-master

# 1. Clean build
./gradlew clean

# 2. Build APK
./gradlew assembleDebug

# 3. Install su device/emulator
./gradlew installDebug

# 4. Monitor i log
adb logcat | grep -E "(BufferLoader|MSPLoader)"
```

## 📊 Log Attesi

Se tutto funziona correttamente, vedrai:

```
MSPLoader: Loading audio buffers...
BufferLoader: === Buffer Loading Started ===
BufferLoader: Found buffer: drums -> media/drumLoop.aif
BufferLoader: File size: 614400 bytes
BufferLoader: AIFF loaded: 44100 Hz, 2 channels, 69300 frames
BufferLoader: Successfully loaded: media/drumLoop.aif
BufferLoader: === Buffer Loading Complete: 1/1 loaded ===
MSPLoader: Buffers loaded: 1
```

## 🐛 Troubleshooting

### "dependencies.json not found"
```bash
# Verifica che il file esista in export/
ls -la export/dependencies.json

# Se manca, crealo:
echo '[{"id":"drums","file":"media/drumLoop.aif"}]' > export/dependencies.json

# Riesegui replacer
python replacer.py
```

### "Failed to open: media//drumLoop.aif" (double slash)
```bash
# Controlla il contenuto di dependencies.json
cat export/dependencies.json

# Se vedi backslash (\), correggilo:
sed -i 's/\\\\/\//g' export/dependencies.json

# Riesegui replacer
python replacer.py
```

### "UnsatisfiedLinkError" per init()
- ✅ Il template `nativelib.py` ora gestisce automaticamente l'encoding underscore
- ✅ Non serve più modificare manualmente i nomi JNI!
- Se l'errore persiste, verifica che il package name in `build.gradle` corrisponda

## 📚 Documentazione Dettagliata

Per maggiori dettagli sulle modifiche automatizzate, vedi:
- `BUFFER_SUPPORT_AUTOMATION.md` - Documentazione tecnica completa
- `STEP4_BUFFER_SUPPORT_README.md` - Architettura buffer support
- `STEP4_TESTING_CHECKLIST.md` - Checklist di testing

## 🎯 Risultato

Dopo l'esecuzione del replacer, il progetto Android è completamente configurato con:
- ✅ JNI correttamente configurato (anche con underscore nel package name)
- ✅ AssetManager integration funzionante
- ✅ BufferLoader completo e funzionante
- ✅ dependencies.json e file audio negli assets
- ✅ Pronto per compilare e testare!

**Non serve più modificare manualmente nessun file!** 🎉
