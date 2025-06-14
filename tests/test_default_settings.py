import os
from pathlib import Path
from PySide6.QtCore import QSettings


def read_value(settings_dir: Path, key: str, default: int = 0) -> int:
    """Mimic Settings_Read_value from the application."""
    old_path = settings_dir / "settings_old.ini"
    new_path = settings_dir / "settings.ini"
    new_settings = QSettings(str(new_path), QSettings.IniFormat)
    if old_path.exists():
        old_settings = QSettings(str(old_path), QSettings.IniFormat)
        if old_settings.contains(key):
            return int(old_settings.value(key))
        if new_settings.contains(key):
            return int(new_settings.value(key))
        return default
    if new_settings.contains(key):
        return int(new_settings.value(key))
    return default


def test_default_engine_indices(tmp_path: Path):
    # No settings files exist yet -> should fall back to default index 0
    assert read_value(tmp_path, "/settings/ImageEngine", 0) == 0
    assert read_value(tmp_path, "/settings/GIFEngine", 0) == 0
    assert read_value(tmp_path, "/settings/VideoEngine", 0) == 0

