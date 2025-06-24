# Updated pipeFrameToEncoder signature

Adjusted RealCuganProcessor.h so `pipeFrameToEncoder` accepts the upscaled frame data as a const QByteArray reference. This aligns the header with the implementation and call site in RealCuganProcessor.cpp.

Related memories:
- [2025-06-25T221500Z-consolidated-merge-summary-note.md](2025-06-25T221500Z-consolidated-merge-summary-note.md)
- [2025-06-25T230000Z-clazy-checks.md](2025-06-25T230000Z-clazy-checks.md)
