"""
MainActivity Template Generator
Reads the MainActivity Java template and injects package name
"""

from pathlib import Path


class MainActivityTemplate:
    # Path to the extracted template
    TEMPLATE_FILE = Path(__file__).parent.parent / "mainactivity_extracted.txt"
    
    @staticmethod
    def generate(package_name, parameters=None):
        """
        Generate MainActivity.java with the given package name

        Args:
            package_name: Java package name (e.g., "com.example.rnbo_test")
            parameters: List of parameter dicts (optional, for future use)

        Returns:
            String containing the complete MainActivity.java content
        """
        # Read template
        try:
            with open(MainActivityTemplate.TEMPLATE_FILE, 'r', encoding='utf-8') as f:
                template = f.read()
        except FileNotFoundError:
            # Fallback: use minimal template
            return MainActivityTemplate._get_minimal_template(package_name)

        # Replace package name placeholders
        content = template.replace('{package_name}', package_name)

        return content

    @staticmethod
    def _get_minimal_template(package_name):
        """Fallback minimal template if extraction fails"""
        return f'''package {package_name};

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import {package_name}.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {{

    static {{
        System.loadLibrary("rnbo_test");
    }}

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {{
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        init(getAssets());
    }}

    public void declareSliders(@NonNull String[] paramNames) {{
        // Minimal implementation
    }}

    public native void init(android.content.res.AssetManager assetManager);
    public native void updateParam(String name, float val);
}}
'''
