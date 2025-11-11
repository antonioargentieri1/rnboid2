#!/usr/bin/env python3
"""
Script di verifica template MainActivity
Controlla che non ci siano doppie graffe nel template
"""

from pathlib import Path

def check_template():
    template_file = Path(__file__).parent / "mainactivity_extracted.txt"
    
    if not template_file.exists():
        print("❌ Template file not found!")
        return False
    
    with open(template_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Conta doppie graffe (escludendo quelle nei commenti)
    double_braces_open = content.count('{{')
    double_braces_close = content.count('}}')
    
    print("=" * 60)
    print("Template MainActivity Verification")
    print("=" * 60)
    
    if double_braces_open == 0 and double_braces_close == 0:
        print("✅ Template OK - No double braces found")
        print(f"📄 File: {template_file.name}")
        print(f"📏 Lines: {len(content.splitlines())}")
        print(f"📊 Size: {len(content)} characters")
        return True
    else:
        print("❌ Template ERROR - Double braces found!")
        print(f"   {{ {{ found: {double_braces_open} times")
        print(f"   }} }} found: {double_braces_close} times")
        print("")
        print("🔧 Fix needed:")
        print("   Run: sed 's/{{/{/g; s/}}/}/g' mainactivity_extracted.txt")
        return False

if __name__ == "__main__":
    import sys
    result = check_template()
    sys.exit(0 if result else 1)
