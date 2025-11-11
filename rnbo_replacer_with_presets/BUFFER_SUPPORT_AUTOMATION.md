# Buffer Support - Modifiche Automatizzate

## Panoramica
Questo documento descrive tutte le modifiche automatizzate dal replacer per supportare il caricamento di buffer audio (file AIFF) in RNBO Android tramite AssetManager.

## Modifiche Automatiche Applicate

### 1. MainActivity.java
**File template**: `mainactivity_extracted.txt`

**Modifiche**:
- Linea ~213: `init()` → `init(getAssets())`
  - Passa l'AssetManager al codice nativo
- Linea ~2293: `public native void init();` → `public native void init(android.content.res.AssetManager assetManager);`
  - Aggiorna la firma JNI per accettare AssetManager

### 2. native-lib.cpp
**File template**: `templates/nativelib.py`

**Modifiche**:
- **Encoding JNI Underscore**: Implementato encoding corretto per package names con underscore
  - Esempio: `com.example.rnbo_test` → `com_example_rnbo_1test`
  - Regola JNI: `_` deve diventare `_1` prima di convertire `.` in `_`
- **AssetManager Support**:
  - Include headers: `<android/asset_manager.h>` e `<android/asset_manager_jni.h>`
  - Parametro `jobject assetManager` nella funzione `init()`
  - Conversione: `AAssetManager* nativeAssetManager = AAssetManager_fromJava(env, assetManager);`
  - Passa AssetManager a MSPLoader: `mspLoader->init(..., nativeAssetManager);`

### 3. MSPLoader.h
**File template**: `templates/msploader_h.py`

**Modifiche**:
- Include headers AssetManager: `<android/asset_manager.h>` e `<android/asset_manager_jni.h>`
- Firma metodo init: `int init(..., AAssetManager* assetManager);`
  - Aggiunto parametro AssetManager per permettere il caricamento dei buffer

### 4. MSPLoader.cpp
**File template**: `templates/msploader.py`

**Modifiche**:
- Include: `"BufferLoader.h"`
- Metodo `init()` aggiornato per caricare buffer:
  ```cpp
  if (assetManager) {
      __android_log_print(ANDROID_LOG_INFO, "MSPLoader", "Loading audio buffers...");
      int buffersLoaded = BufferLoader::loadBuffers(rnboObject, assetManager);
      __android_log_print(ANDROID_LOG_INFO, "MSPLoader", "Buffers loaded: %d", buffersLoaded);
  }
  ```

### 5. BufferLoader.h e BufferLoader.cpp
**File template**: `templates/buffer_loader_h.py` e `templates/buffer_loader_cpp.py`

**Nuovi file generati**:
- `BufferLoader.h`: Header con API per caricamento buffer
- `BufferLoader.cpp`: Implementazione completa (~250 righe)
  - Parser AIFF con supporto big-endian
  - Lettura file da Android assets
  - Integrazione con RNBO buffer~ objects

### 6. dependencies.json
**Gestione automatica**: `core/replacer.py` - metodo `_copy_description_to_assets()`

**Modifiche**:
- Copia `export/dependencies.json` → `app/src/main/assets/dependencies.json`
- Path corretti: usa forward slash `/` invece di backslash `\`
  - Esempio: `media/drumLoop.aif` (non `media\\drumLoop.aif`)
- Formato JSON:
  ```json
  [
    {
      "id": "drums",
      "file": "media/drumLoop.aif"
    }
  ]
  ```

## File Modificati nel Replacer

1. **`templates/nativelib.py`**
   - Aggiunto encoding JNI underscore corretto
   - Codice: `jni_package = package_name.replace('_', '_1').replace('.', '_')`

2. **`mainactivity_extracted.txt`**
   - Aggiornato template base MainActivity con AssetManager support

3. **`core/replacer.py`**
   - Metodo `_copy_description_to_assets()` esteso per copiare anche `dependencies.json`

## Template Già Corretti (Nessuna Modifica Necessaria)

- ✅ `templates/msploader_h.py` - Già include AssetManager
- ✅ `templates/msploader.py` - Già include BufferLoader
- ✅ `templates/buffer_loader_h.py` - Già completo
- ✅ `templates/buffer_loader_cpp.py` - Già completo

## Workflow Automatizzato

Quando esegui il replacer Python:

1. **Step 1-2**: Copia rnbo e cpp files dall'export
2. **Step 3**: Genera i file C++ dai template:
   - `native-lib.cpp` con encoding JNI corretto
   - `MSPLoader.h` e `MSPLoader.cpp` con AssetManager
   - `BufferLoader.h` e `BufferLoader.cpp`
3. **Step 4**: Copia JSON in assets:
   - `description.json` → assets
   - **`dependencies.json` → assets** (NUOVO!)
4. **Step 5-7**: Sostituisce rnbo, media, genera MainActivity.java
5. **Step 8**: Aggiorna build.gradle e AndroidManifest

## Verifica Funzionamento

Dopo l'esecuzione del replacer, verifica:

```bash
# 1. Controlla che dependencies.json sia in assets
ls -lh app/src/main/assets/dependencies.json

# 2. Verifica il contenuto (deve avere forward slash)
cat app/src/main/assets/dependencies.json

# 3. Controlla che i file audio esistano
ls -lh app/src/main/assets/media/

# 4. Controlla i nomi JNI in native-lib.cpp
grep "Java_" app/src/main/cpp/native-lib.cpp
# Deve mostrare: Java_com_example_rnbo_1test_MainActivity_init
# NON: Java_com_example_rnbo_test_MainActivity_init
```

## Log di Debug

Quando l'app si avvia, cerca questi log:

```
MSPLoader: Loading audio buffers...
BufferLoader: === Buffer Loading Started ===
BufferLoader: Found buffer: drums -> media/drumLoop.aif
BufferLoader: Successfully loaded: media/drumLoop.aif
BufferLoader: === Buffer Loading Complete: 1/1 loaded ===
MSPLoader: Buffers loaded: 1
```

## Risoluzione Problemi

### Problema: "UnsatisfiedLinkError" per init()
**Causa**: Encoding JNI underscore errato
**Soluzione**: Il template `nativelib.py` ora gestisce automaticamente `_` → `_1`

### Problema: "dependencies.json not found"
**Causa**: File non copiato in assets
**Soluzione**: Il replacer ora copia automaticamente il file

### Problema: "Failed to open: media//drumLoop.aif"
**Causa**: Double slash nel path (backslash Windows)
**Soluzione**: Usa sempre forward slash `/` in dependencies.json

## Credits

Implementazione: STEP 4 - Buffer Support
Data: 2025-11-09
Modifiche automatizzate per semplificare il workflow RNBO Android
