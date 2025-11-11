"""
colors.xml Template
"""


class ColorsTemplate:
    @staticmethod
    def get_content():
        return '''<?xml version="1.0" encoding="utf-8"?>
<resources>
    <!-- Dark Theme Colors -->
    <color name="black">#FF000000</color>
    <color name="white">#FFFFFFFF</color>
    <color name="gray_dark">#FF333333</color>
    <color name="gray_medium">#FF666666</color>
    <color name="gray_medium_light">#FF888888</color>
    <color name="gray_light">#FFCCCCCC</color>

    <!-- Legacy colors for compatibility -->
    <color name="purple_200">#FF333333</color>
    <color name="purple_500">#FF666666</color>
    <color name="purple_700">#FF888888</color>
    <color name="teal_200">#FF666666</color>
    <color name="teal_700">#FF333333</color>
</resources>
'''
