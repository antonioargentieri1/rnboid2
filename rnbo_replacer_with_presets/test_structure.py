#!/usr/bin/env python3
"""
Test Script - Verifica struttura modulare
"""

import sys
from pathlib import Path

def test_imports():
    """Test che tutti i moduli si importino correttamente"""
    print("🔍 Testing imports...")
    
    try:
        from gui.main_window import RNBOReplacerGUI
        print("  ✅ gui.main_window")
    except ImportError as e:
        print(f"  ❌ gui.main_window: {e}")
        return False
    
    try:
        from core.replacer import RNBOReplacer
        print("  ✅ core.replacer")
    except ImportError as e:
        print(f"  ❌ core.replacer: {e}")
        return False
    
    try:
        from core.analyzer import RNBOAnalyzer
        print("  ✅ core.analyzer")
    except ImportError as e:
        print(f"  ❌ core.analyzer: {e}")
        return False
    
    try:
        from templates.mainactivity import MainActivityTemplate
        print("  ✅ templates.mainactivity")
    except ImportError as e:
        print(f"  ❌ templates.mainactivity: {e}")
        return False
    
    try:
        from templates.msploader import MSPLoaderTemplate
        print("  ✅ templates.msploader")
    except ImportError as e:
        print(f"  ❌ templates.msploader: {e}")
        return False
    
    try:
        from templates.layout import LayoutTemplate
        print("  ✅ templates.layout")
    except ImportError as e:
        print(f"  ❌ templates.layout: {e}")
        return False
    
    try:
        from utils.logger import Logger
        print("  ✅ utils.logger")
    except ImportError as e:
        print(f"  ❌ utils.logger: {e}")
        return False
    
    return True

def test_templates():
    """Test che i template generino contenuto"""
    print("\n🔍 Testing templates...")
    
    try:
        from templates.msploader import MSPLoaderTemplate
        content = MSPLoaderTemplate.get_content()
        if len(content) > 1000:
            print(f"  ✅ MSPLoader template ({len(content)} chars)")
        else:
            print(f"  ⚠️  MSPLoader template too short")
    except Exception as e:
        print(f"  ❌ MSPLoader: {e}")
        return False
    
    try:
        from templates.layout import LayoutTemplate
        content = LayoutTemplate.get_content()
        if "ConstraintLayout" in content:
            print(f"  ✅ Layout template ({len(content)} chars)")
        else:
            print(f"  ⚠️  Layout template missing expected content")
    except Exception as e:
        print(f"  ❌ Layout: {e}")
        return False
    
    try:
        from templates.mainactivity import MainActivityTemplate
        content = MainActivityTemplate.generate("com.test.example")
        if "package com.test.example" in content:
            print(f"  ✅ MainActivity template ({len(content)} chars)")
        else:
            print(f"  ⚠️  MainActivity package replacement failed")
    except Exception as e:
        print(f"  ❌ MainActivity: {e}")
        return False
    
    return True

def test_file_structure():
    """Test che tutti i file esistano"""
    print("\n🔍 Testing file structure...")
    
    required_files = [
        "main.py",
        "README.md",
        "mainactivity_extracted.txt",
        "gui/__init__.py",
        "gui/main_window.py",
        "core/__init__.py",
        "core/replacer.py",
        "core/analyzer.py",
        "templates/__init__.py",
        "templates/mainactivity.py",
        "templates/msploader.py",
        "templates/layout.py",
        "utils/__init__.py",
        "utils/logger.py",
    ]
    
    all_exist = True
    for file_path in required_files:
        p = Path(file_path)
        if p.exists():
            print(f"  ✅ {file_path}")
        else:
            print(f"  ❌ {file_path} - MISSING")
            all_exist = False
    
    return all_exist

def main():
    print("=" * 60)
    print("RNBO Replacer - Modular Structure Test")
    print("=" * 60)
    
    # Test 1: File Structure
    if not test_file_structure():
        print("\n❌ File structure test FAILED")
        return 1
    
    # Test 2: Imports
    if not test_imports():
        print("\n❌ Import test FAILED")
        return 1
    
    # Test 3: Templates
    if not test_templates():
        print("\n❌ Template test FAILED")
        return 1
    
    print("\n" + "=" * 60)
    print("✅ ALL TESTS PASSED!")
    print("=" * 60)
    print("\n🚀 Ready to run: python3 main.py")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
