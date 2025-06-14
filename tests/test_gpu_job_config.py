import pytest

# Helper replicating the logic from Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF

def build_gpu_job_config(multi_gpu_enabled, gpu_list, fallback_gpu_id):
    """Build GPU job string.

    gpu_list: list of dicts with keys ID, Threads
    fallback_gpu_id: string (e.g. "0" or "-1")
    """
    if multi_gpu_enabled:
        if gpu_list:
            ids = []
            jobs = []
            for gpu in gpu_list:
                ids.append(gpu['ID'])
                threads = gpu.get('Threads', '1')
                jobs.append(f"1:{threads}:1")
            return f"-g {','.join(ids)} -j {','.join(jobs)}"
        else:
            # fallback to single selection
            if fallback_gpu_id == "-1":
                return "-g -1"
        return f"-g {fallback_gpu_id}"
    else:
        if fallback_gpu_id == "-1":
            return "-g -1"
        return f"-g {fallback_gpu_id}"


def prepare_arguments(is_multi_gpu, job_args, gpu_id):
    """Replicate RealCuganProcessor::prepareArguments GPU logic."""
    base = ["-i", "in.png", "-o", "out.png", "-s", "2", "-n", "1", "-t", "128", "-m", "models"]
    if is_multi_gpu:
        base += job_args.split(" ")
    else:
        base += ["-g", gpu_id.split(":")[0]]
    base += ["-f", "png"]
    return base

@pytest.mark.parametrize("gpu_list,expected", [
    ([{'ID': '0', 'Threads': '2'}, {'ID': '1', 'Threads': '2'}], "-g 0,1 -j 1:2:1,1:2:1"),
    ([{'ID': '-1', 'Threads': '4'}, {'ID': '1', 'Threads': '2'}], "-g -1,1 -j 1:4:1,1:2:1"),
])
def test_multi_gpu_configs(gpu_list, expected):
    assert build_gpu_job_config(True, gpu_list, '0') == expected

def test_multi_gpu_empty_list_fallback_cpu():
    assert build_gpu_job_config(True, [], '-1') == "-g -1"

def test_single_gpu():
    assert build_gpu_job_config(False, [], '2') == "-g 2"


def test_single_cpu():
    assert build_gpu_job_config(False, [], '-1') == "-g -1"


def test_prepare_multi_gpu_arguments():
    job = "-g 0,1 -j 1:2:1,1:2:1"
    args = prepare_arguments(True, job, '0')
    assert args[-6:] == ['-g', '0,1', '-j', '1:2:1,1:2:1', '-f', 'png']


def test_prepare_single_gpu_arguments():
    args = prepare_arguments(False, '', '3: GPU')
    assert args[-4:] == ['-g', '3', '-f', 'png']
