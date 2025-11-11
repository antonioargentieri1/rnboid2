"""
Templates Module - Code Templates
"""

from .mainactivity import MainActivityTemplate
from .layout import LayoutTemplate
from .msploader import MSPLoaderTemplate
from .build_gradle import BuildGradleTemplate
from .androidmanifest import AndroidManifestTemplate
from .mediapipe_setup import MediaPipeSetupTemplate
from .cmakelists import CMakeListsTemplate
from .themes import ThemesTemplate
from .colors import ColorsTemplate
from .gesturemanager import GestureManagerTemplate

__all__ = [
    'MainActivityTemplate', 
    'LayoutTemplate', 
    'MSPLoaderTemplate',
    'BuildGradleTemplate',
    'AndroidManifestTemplate',
    'MediaPipeSetupTemplate',
    'CMakeListsTemplate',
    'ThemesTemplate',
    'ColorsTemplate',
    'GestureManagerTemplate'
]
