"""
Main GUI Window - RNBO Android Replacer
"""

import tkinter as tk
from tkinter import filedialog, messagebox, scrolledtext
import threading
from core.replacer import RNBOReplacer
from utils.logger import Logger


class RNBOReplacerGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("RNBO Android Replacer v2.2 - MULTI-MODE")
        self.root.geometry("900x750")
        
        # Variabili
        self.template_path = tk.StringVar()
        self.export_path = tk.StringVar()
        self.rename_cpp = tk.BooleanVar(value=True)
        
        # Logger
        self.logger = None
        
        # Core replacer
        self.replacer = None
        
        self.create_widgets()
        
    def create_widgets(self):
        # Frame principale
        main_frame = tk.Frame(self.root, padx=20, pady=20, bg='#000000')
        main_frame.pack(fill=tk.BOTH, expand=True)
        
        # Titolo
        self._create_title(main_frame)
        
        # Template
        self._create_template_section(main_frame)
        
        # Export
        self._create_export_section(main_frame)
        
        # Opzioni
        self._create_options_section(main_frame)
        
        # Pulsanti
        self._create_buttons(main_frame)
        
        # Status
        self._create_status(main_frame)
        
        # Log
        self._create_log(main_frame)
        
    def _create_title(self, parent):
        title_frame = tk.Frame(parent, bg='#000000', pady=15)
        title_frame.pack(fill=tk.X, pady=(0, 20))

        tk.Label(title_frame,
                text="RNBO Android Replacer v2.2 - MULTI-MODE",
                font=('Arial', 18, 'bold'),
                bg='#000000',
                fg='#FFFFFF').pack()

        tk.Label(title_frame,
                text="Multi-Mode System: Live | All | Setup",
                font=('Arial', 10),
                bg='#000000',
                fg='#FFFFFF').pack()
    
    def _create_template_section(self, parent):
        template_frame = tk.LabelFrame(parent,
                                      text=" Template Android ",
                                      font=('Arial', 11, 'bold'),
                                      padx=15, pady=15,
                                      bg='#000000',
                                      fg='#FFFFFF')
        template_frame.pack(fill=tk.X, pady=(0, 15))

        tk.Entry(template_frame,
                textvariable=self.template_path,
                font=('Arial', 10),
                width=70,
                bg='#333333',
                fg='#FFFFFF',
                insertbackground='#FFFFFF').pack(side=tk.LEFT, padx=(0, 10))

        tk.Button(template_frame,
                 text="Sfoglia...",
                 command=self.browse_template,
                 bg='#333333',
                 fg='#FFFFFF',
                 font=('Arial', 10, 'bold'),
                 cursor='hand2',
                 padx=20).pack(side=tk.LEFT)
    
    def _create_export_section(self, parent):
        export_frame = tk.LabelFrame(parent,
                                    text=" Export RNBO ",
                                    font=('Arial', 11, 'bold'),
                                    padx=15, pady=15,
                                    bg='#000000',
                                    fg='#FFFFFF')
        export_frame.pack(fill=tk.X, pady=(0, 15))

        tk.Entry(export_frame,
                textvariable=self.export_path,
                font=('Arial', 10),
                width=70,
                bg='#333333',
                fg='#FFFFFF',
                insertbackground='#FFFFFF').pack(side=tk.LEFT, padx=(0, 10))

        tk.Button(export_frame,
                 text="Sfoglia...",
                 command=self.browse_export,
                 bg='#333333',
                 fg='#FFFFFF',
                 font=('Arial', 10, 'bold'),
                 cursor='hand2',
                 padx=20).pack(side=tk.LEFT)
    
    def _create_options_section(self, parent):
        options_frame = tk.LabelFrame(parent,
                                     text=" Opzioni ",
                                     font=('Arial', 11, 'bold'),
                                     padx=15, pady=15,
                                     bg='#000000',
                                     fg='#FFFFFF')
        options_frame.pack(fill=tk.X, pady=(0, 15))

        tk.Checkbutton(options_frame,
                      text="Rinomina automaticamente il file .cpp (Consigliato)",
                      variable=self.rename_cpp,
                      font=('Arial', 10),
                      bg='#000000',
                      fg='#FFFFFF',
                      selectcolor='#333333',
                      cursor='hand2').pack(anchor=tk.W)
    
    def _create_buttons(self, parent):
        button_frame = tk.Frame(parent, bg='#000000', pady=10)
        button_frame.pack(fill=tk.X, pady=(10, 0))

        center_frame = tk.Frame(button_frame, bg='#000000')
        center_frame.pack()

        tk.Button(center_frame,
                 text="[1] Verifica Percorsi",
                 command=self.verify_paths,
                 bg='#333333',
                 fg='#FFFFFF',
                 font=('Arial', 11, 'bold'),
                 cursor='hand2',
                 padx=25,
                 pady=12,
                 width=20).pack(side=tk.LEFT, padx=5)

        self.replace_button = tk.Button(center_frame,
                                       text="[2] Sostituisci File",
                                       command=self.start_replacement,
                                       bg='#333333',
                                       fg='#FFFFFF',
                                       font=('Arial', 11, 'bold'),
                                       cursor='hand2',
                                       padx=25,
                                       pady=12,
                                       width=20)
        self.replace_button.pack(side=tk.LEFT, padx=5)

        tk.Button(center_frame,
                 text="[X] Esci",
                 command=self.root.quit,
                 bg='#666666',
                 fg='#FFFFFF',
                 font=('Arial', 11, 'bold'),
                 cursor='hand2',
                 padx=25,
                 pady=12,
                 width=20).pack(side=tk.LEFT, padx=5)
    
    def _create_status(self, parent):
        status_frame = tk.Frame(parent, bg='#000000', pady=10)
        status_frame.pack(fill=tk.X, pady=(15, 10))

        self.status_label = tk.Label(status_frame,
                                     text="Pronto - Seleziona le directory",
                                     font=('Arial', 10, 'bold'),
                                     bg='#000000',
                                     fg='#FFFFFF')
        self.status_label.pack()
    
    def _create_log(self, parent):
        log_frame = tk.LabelFrame(parent,
                                 text=" Log Operazioni ",
                                 font=('Arial', 11, 'bold'),
                                 padx=10, pady=10,
                                 bg='#000000',
                                 fg='#FFFFFF')
        log_frame.pack(fill=tk.BOTH, expand=True, pady=(10, 0))

        self.log_text = scrolledtext.ScrolledText(log_frame,
                                                  height=12,
                                                  font=('Courier', 9),
                                                  wrap=tk.WORD,
                                                  bg='#000000',
                                                  fg='#FFFFFF',
                                                  insertbackground='#FFFFFF')
        self.log_text.pack(fill=tk.BOTH, expand=True)

        # Inizializza logger
        self.logger = Logger(self.log_text)

        # Messaggio iniziale
        self.logger.log("=" * 80)
        self.logger.log("RNBO Android Replacer v2.2 - MULTI-MODE Edition")
        self.logger.log("=" * 80)
        self.logger.log("\nNEW v2.2 - MULTI-MODE SYSTEM:")
        self.logger.log("   - 3 Modalita: Live | All | Setup")
        self.logger.log("   - Setup Mode: Seleziona parametri per Live")
        self.logger.log("   - Live Mode: Solo parametri selezionati")
        self.logger.log("   - All Mode: Tutti i parametri")
        self.logger.log("   - Salvataggio configurazione persistente")
        self.logger.log("\nISTRUZIONI:")
        self.logger.log("1. Seleziona Template Android")
        self.logger.log("2. Seleziona Export RNBO")
        self.logger.log("3. Verifica Percorsi")
        self.logger.log("4. Sostituisci File")
        self.logger.log("\n")
    
    def browse_template(self):
        directory = filedialog.askdirectory(title="Seleziona Template Android")
        if directory:
            self.template_path.set(directory)
            self.logger.log(f"\n[OK] Template: {directory}")

    def browse_export(self):
        directory = filedialog.askdirectory(title="Seleziona Export RNBO")
        if directory:
            self.export_path.set(directory)
            self.logger.log(f"\n[OK] Export: {directory}")
    
    def verify_paths(self):
        template = self.template_path.get()
        export = self.export_path.get()

        if not template or not export:
            messagebox.showerror("Errore", "Seleziona entrambe le directory!")
            return

        # Crea replacer instance
        self.replacer = RNBOReplacer(template, export, self.logger)

        # Verifica
        if self.replacer.verify_paths():
            self.status_label.config(text="[OK] Percorsi verificati - Pronto per sostituzione")
            messagebox.showinfo("Verifica Completata",
                              "Tutti i percorsi sono corretti!\nPuoi procedere con la sostituzione.")
        else:
            self.status_label.config(text="[ERROR] Errore nella verifica percorsi")
            messagebox.showerror("Errore",
                               "Verifica fallita! Controlla il log per i dettagli.")
    
    def start_replacement(self):
        if not self.replacer:
            messagebox.showerror("Errore", "Verifica prima i percorsi!")
            return

        # Conferma
        if not messagebox.askyesno("Conferma",
                                   "Procedere con la sostituzione?\n\n"
                                   "ATTENZIONE: I file del template verranno modificati."):
            return

        # Disabilita button
        self.replace_button.config(state=tk.DISABLED)
        self.status_label.config(text="[WORKING] Sostituzione in corso...")

        # Thread per sostituzione
        def replace_thread():
            try:
                success = self.replacer.replace_files(self.rename_cpp.get())

                self.root.after(0, lambda: self._on_replacement_complete(success))
            except Exception as e:
                self.root.after(0, lambda: self._on_replacement_error(str(e)))

        threading.Thread(target=replace_thread, daemon=True).start()
    
    def _on_replacement_complete(self, success):
        self.replace_button.config(state=tk.NORMAL)

        if success:
            self.status_label.config(text="[OK] Sostituzione completata con successo!")
            messagebox.showinfo("Completato",
                              "Sostituzione completata con successo!\n\n"
                              "Puoi ora compilare il progetto Android.")
        else:
            self.status_label.config(text="[ERROR] Errore durante la sostituzione")
            messagebox.showerror("Errore",
                               "Si e verificato un errore. Controlla il log.")

    def _on_replacement_error(self, error):
        self.replace_button.config(state=tk.NORMAL)
        self.status_label.config(text="[ERROR] Errore critico")
        self.logger.log(f"\n[ERROR] ERRORE CRITICO: {error}")
        messagebox.showerror("Errore Critico", f"Errore: {error}")
