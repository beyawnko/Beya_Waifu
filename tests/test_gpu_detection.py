import re

sample_output1 = """RealESRGAN-ncnn-vulkan 0.2.0
GPU device 0: NVIDIA GTX 1050
GPU device 1: Intel(R) UHD Graphics 630
"""

sample_output2 = """RealCUGAN-ncnn-vulkan
[0] GeForce RTX 2080
[1] Intel(R) HD Graphics
"""

pattern = re.compile(r"^\s*(?:GPU device\s*)?\[?(\d+)\]?[:\-]?\s*(.+)$", re.I | re.M)


def parse_devices(text):
    return [f"{m.group(1)}: {m.group(2).strip()}" for m in pattern.finditer(text)]


def test_parse_sample_output1():
    assert parse_devices(sample_output1) == [
        "0: NVIDIA GTX 1050",
        "1: Intel(R) UHD Graphics 630",
    ]


def test_parse_sample_output2():
    assert parse_devices(sample_output2) == [
        "0: GeForce RTX 2080",
        "1: Intel(R) HD Graphics",
    ]
