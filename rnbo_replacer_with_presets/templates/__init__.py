"""
Templates Module - Code Templates
"""

from .mainactivity import MainActivityTemplate
from .layout import LayoutTemplate
from .msploader import MSPLoaderTemplate
from .build_gradle import BuildGradleTemplate
from .androidmanifest import AndroidManifestTemplate
from .cmakelists import CMakeListsTemplate
from .themes import ThemesTemplate
from .colors import ColorsTemplate

__all__ = [
    'MainActivityTemplate',
    'LayoutTemplate',
    'MSPLoaderTemplate',
    'BuildGradleTemplate',
    'AndroidManifestTemplate',
    'CMakeListsTemplate',
    'ThemesTemplate',
    'ColorsTemplate'
]
