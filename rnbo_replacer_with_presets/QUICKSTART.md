# QUICK START GUIDE

## 📁 Struttura Progetto

```
rnbo_replacer/
├── main.py                 ← ESEGUI QUESTO!
├── gui/                    ← Interfaccia grafica
├── core/                   ← Logica sostituzione
├── templates/              ← Template Java/C++/XML
└── utils/                  ← Utility logging
```

## 🚀 Avvio Rapido

```bash
# Dalla directory rnbo_replacer
python3 main.py
```

## 📝 Uso

1. **Seleziona Template Android**
   - Clicca "Sfoglia..." accanto a "Template Android"
   - Scegli la directory del tuo progetto Android Studio

2. **Seleziona Export RNBO**
   - Clicca "Sfoglia..." accanto a "Export RNBO"
   - Scegli la directory dell'export C++ da Max/MSP

3. **Verifica Percorsi**
   - Clicca "1️⃣ Verifica Percorsi"
   - Controlla il log per verificare che tutto sia OK

4. **Sostituisci File**
   - Clicca "2️⃣ Sostituisci File"
   - Conferma l'operazione
   - Attendi il completamento

## ✅ Risultato

Il tool:
- ✅ Crea backup automatici (cpp_backup, java_backup)
- ✅ Sostituisce i file RNBO
- ✅ Aggiorna MainActivity con Multi-Mode System
- ✅ Applica MSPLoader ottimizzato
- ✅ Copia description.json in assets/

## 🔧 Dopo la Sostituzione

```bash
cd /path/to/android/project
./gradlew clean
./gradlew build
```

## ❓ Problemi?

Controlla il log nella finestra per dettagli su eventuali errori.
Tutti i backup sono in cpp_backup/ e java_backup/

## 📚 Documentazione Completa

Vedi `README.md` per architettura dettagliata e troubleshooting.
