import pytest
from pathlib import Path
from tests import PYSIDE6_AVAILABLE
if not PYSIDE6_AVAILABLE:
    pytest.skip("PySide6 not available", allow_module_level=True)
from PySide6.QtCore import QSettings

# Helper from test_gpu_job_config.py

def build_gpu_job_config(multi_gpu_enabled, gpu_list, fallback_gpu_id):
    if multi_gpu_enabled:
        if gpu_list:
            ids = []
            jobs = []
            for gpu in gpu_list:
                ids.append(gpu['ID'])
                threads = gpu.get('Threads', '1')
                jobs.append(f"1:{threads}:1")
            return f"-g {','.join(ids)} -j {','.join(jobs)}"
        if fallback_gpu_id == "-1":
            return "-g -1"
        return f"-g {fallback_gpu_id}"
    if fallback_gpu_id == "-1":
        return "-g -1"
    return f"-g {fallback_gpu_id}"


def parse_video_settings(settings_dir: Path):
    ini = QSettings(str(settings_dir / "settings.ini"), QSettings.IniFormat)
    multi = ini.value("/RealCUGAN_NCNN_Vulkan/RealCUGAN_MultiGPU_Enabled", False, type=bool)
    gpu_list = ini.value("/RealCUGAN_NCNN_Vulkan/RealCUGAN_GPUJobConfig_MultiGPU", [])
    fallback = ini.value("/RealCUGAN_NCNN_Vulkan/RealCUGAN_GPUID", "0")
    jobs_video = ini.value("/RealCUGAN_NCNN_Vulkan/RealCUGANJobsVideo", "1:2:2")
    gpu_config = build_gpu_job_config(multi, gpu_list, fallback)
    return gpu_config, jobs_video


def test_parse_video_settings(tmp_path: Path):
    ini = QSettings(str(tmp_path / "settings.ini"), QSettings.IniFormat)
    ini.setValue("/RealCUGAN_NCNN_Vulkan/RealCUGAN_MultiGPU_Enabled", True)
    ini.setValue("/RealCUGAN_NCNN_Vulkan/RealCUGAN_GPUID", "0")
    ini.setValue(
        "/RealCUGAN_NCNN_Vulkan/RealCUGAN_GPUJobConfig_MultiGPU",
        [{"ID": "0", "Threads": "2"}, {"ID": "1", "Threads": "3"}],
    )
    ini.setValue("/RealCUGAN_NCNN_Vulkan/RealCUGANJobsVideo", "2:4:4")

    gpu_config, jobs_video = parse_video_settings(tmp_path)
    assert gpu_config == "-g 0,1 -j 1:2:1,1:3:1"
    assert jobs_video == "2:4:4"


def test_parse_video_settings_defaults(tmp_path: Path):
    gpu_config, jobs_video = parse_video_settings(tmp_path)
    assert gpu_config == "-g 0"
    assert jobs_video == "1:2:2"
