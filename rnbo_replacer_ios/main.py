#!/usr/bin/env python3
"""
RNBO Replacer iOS/macOS
Main entry point for the iOS/macOS replacer

Usage:
    python main.py --template path/to/SwiftRNBO-main --export path/to/rnbo_export --output path/to/output

Example:
    python main.py --template ../SwiftRNBO-main --export ./my_patch_export --output ./MyRNBOApp
"""

import argparse
import sys
from pathlib import Path

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent))

from core.replacer import IOSReplacer
from utils.logger import Logger


def main():
    """Main entry point"""

    parser = argparse.ArgumentParser(
        description='RNBO iOS/macOS Project Generator',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  Generate a new iOS/macOS project:
    python main.py --template ../SwiftRNBO-main --export ./my_export --output ./MyApp

  Specify custom app name:
    python main.py --template ../SwiftRNBO-main --export ./my_export --output ./MyApp --name "My RNBO App"
        """
    )

    parser.add_argument(
        '--template',
        required=True,
        help='Path to SwiftRNBO-main template directory'
    )

    parser.add_argument(
        '--export',
        required=True,
        help='Path to RNBO export directory (contains description.json)'
    )

    parser.add_argument(
        '--output',
        required=True,
        help='Output directory for the new Xcode project'
    )

    parser.add_argument(
        '--name',
        help='Custom app name (optional, defaults to patch name from description.json)'
    )

    parser.add_argument(
        '--log',
        help='Path to log file (optional)'
    )

    args = parser.parse_args()

    # Create logger
    logger = Logger(log_file=args.log)

    # Print header
    logger.log("=" * 80)
    logger.log("  RNBO iOS/macOS Project Generator")
    logger.log("  Automatic Xcode project creation with AutoUI")
    logger.log("=" * 80)
    logger.log("")
    logger.log(f"Template: {args.template}")
    logger.log(f"Export:   {args.export}")
    logger.log(f"Output:   {args.output}")
    if args.name:
        logger.log(f"App Name: {args.name}")
    logger.log("")

    # Create replacer
    replacer = IOSReplacer(
        template_path=args.template,
        export_path=args.export,
        output_path=args.output,
        logger=logger
    )

    # Run replacement
    try:
        success = replacer.run(app_name=args.name)

        if success:
            logger.log("")
            logger.success("🎉 Project generation complete!")
            sys.exit(0)
        else:
            logger.log("")
            logger.error("❌ Project generation failed!")
            sys.exit(1)

    except KeyboardInterrupt:
        logger.log("")
        logger.warning("⚠️  Operation cancelled by user")
        sys.exit(130)

    except Exception as e:
        logger.log("")
        logger.error(f"❌ Unexpected error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main()
