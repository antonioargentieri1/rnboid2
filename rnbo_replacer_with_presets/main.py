#!/usr/bin/env python3
"""
RNBO Android Replacer - v2.2 MULTI-MODE
Main Entry Point
"""

import tkinter as tk
from gui.main_window import RNBOReplacerGUI


def main():
    root = tk.Tk()
    app = RNBOReplacerGUI(root)
    
    # Centra finestra
    root.update_idletasks()
    width = root.winfo_width()
    height = root.winfo_height()
    x = (root.winfo_screenwidth() // 2) - (width // 2)
    y = (root.winfo_screenheight() // 2) - (height // 2)
    root.geometry(f'{width}x{height}+{x}+{y}')
    
    root.mainloop()


if __name__ == "__main__":
    main()
