try:
    import torch  # noqa: F401
    import torchaudio  # noqa: F401
    import torchvision  # noqa: F401
    TORCH_AVAILABLE = True
except Exception:
    TORCH_AVAILABLE = False


def pytest_ignore_collect(collection_path, config):
    return not TORCH_AVAILABLE
