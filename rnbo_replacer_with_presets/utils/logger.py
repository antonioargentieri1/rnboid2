"""
Logger Utility - Handles logging to GUI
"""

import tkinter as tk


class Logger:
    def __init__(self, text_widget):
        """
        Initialize logger with a tkinter Text or ScrolledText widget
        
        Args:
            text_widget: tkinter.scrolledtext.ScrolledText widget for output
        """
        self.text_widget = text_widget
    
    def log(self, message):
        """Log a message to the text widget"""
        self.text_widget.insert(tk.END, message + "\n")
        self.text_widget.see(tk.END)
        self.text_widget.update()
    
    def clear(self):
        """Clear all log messages"""
        self.text_widget.delete('1.0', tk.END)
