"""
MediaPipe Setup Instructions
"""


class MediaPipeSetupTemplate:
    @staticmethod
    def get_instructions():
        return '''# MediaPipe Hand Landmarker Model Setup

## Download Model File

1. Download hand_landmarker.task from:
   https://storage.googleapis.com/mediapipe-models/hand_landmarker/hand_landmarker/float16/latest/hand_landmarker.task

2. Place in Android project:
   app/src/main/assets/hand_landmarker.task

## File Structure After Setup:

app/
├── src/
│   └── main/
│       ├── assets/
│       │   ├── description.json (already exists)
│       │   └── hand_landmarker.task (NEW - download this)
│       ├── cpp/
│       ├── java/
│       └── res/
└── build.gradle (updated with dependencies)

## Model Info:
- Size: ~9 MB
- Format: .task (TFLite bundle)
- Type: float16 (optimized for mobile)
'''
