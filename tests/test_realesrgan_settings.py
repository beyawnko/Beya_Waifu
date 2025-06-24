import re
from pathlib import Path

HEADER = Path(__file__).resolve().parents[1] / "Waifu2x-Extension-QT" / "realesrgan_settings.h"

def test_realesrgan_settings_fields_exist():
    text = HEADER.read_text()
    assert "bool verboseLog" in text
    assert "QString sourceFile" in text
    assert "QString ffmpegPath" in text
    assert "QString ffprobePath" in text
    assert "QString modelPath" in text
    assert re.search(r"double\s+videoFps\s*=\s*0\.0", text)
