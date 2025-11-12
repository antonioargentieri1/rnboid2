#!/usr/bin/env python3
"""
Test script to verify all imports work correctly
"""

import sys
from pathlib import Path

# Add current directory to path
sys.path.insert(0, str(Path(__file__).parent))

print("Testing imports...")
print("-" * 50)

try:
    from utils.logger import Logger
    print("✅ utils.logger.Logger")
except ImportError as e:
    print(f"❌ utils.logger.Logger: {e}")
    sys.exit(1)

try:
    from core.analyzer import RNBOAnalyzer
    print("✅ core.analyzer.RNBOAnalyzer")
except ImportError as e:
    print(f"❌ core.analyzer.RNBOAnalyzer: {e}")
    sys.exit(1)

try:
    from core.replacer import IOSReplacer
    print("✅ core.replacer.IOSReplacer")
except ImportError as e:
    print(f"❌ core.replacer.IOSReplacer: {e}")
    sys.exit(1)

print("-" * 50)
print("✅ All imports successful!")
print()

# Test basic functionality
print("Testing basic functionality...")
print("-" * 50)

logger = Logger()
logger.log("Test log message")
logger.success("Test success message")
logger.info("Test info message")
logger.warning("Test warning message")
logger.error("Test error message")

print("-" * 50)
print("✅ All tests passed!")
