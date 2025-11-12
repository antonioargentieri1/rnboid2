"""
Logger utility for iOS replacer
"""

import sys
from datetime import datetime


class Logger:
    """Simple logger for console and file output"""

    def __init__(self, log_file=None):
        self.log_file = log_file
        self.logs = []

    def log(self, message):
        """Log a message to console and optionally to file"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        formatted = f"[{timestamp}] {message}"

        print(formatted)
        self.logs.append(formatted)

        if self.log_file:
            with open(self.log_file, 'a') as f:
                f.write(formatted + '\n')

    def success(self, message):
        """Log a success message"""
        self.log(f"✅ {message}")

    def error(self, message):
        """Log an error message"""
        self.log(f"❌ {message}")

    def info(self, message):
        """Log an info message"""
        self.log(f"ℹ️  {message}")

    def warning(self, message):
        """Log a warning message"""
        self.log(f"⚠️  {message}")

    def section(self, title):
        """Log a section header"""
        self.log("\n" + "=" * 80)
        self.log(f"  {title}")
        self.log("=" * 80)

    def save_logs(self):
        """Save all logs to file"""
        if self.log_file:
            with open(self.log_file, 'w') as f:
                f.write('\n'.join(self.logs))
