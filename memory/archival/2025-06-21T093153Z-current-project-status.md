# Current Project Status and Known Issues Report

This report summarizes the known issues and upcoming tasks for the Beya_Waifu project, based on previous analysis.

## Source of Analysis
This report is a synthesis of the findings documented in [[2025-06-21T091949Z-known-issues.md]].

## Key Known Issues

1.  **Qt `uic` Compiler Bug (`setClass`/`setCssClass`)**:
    *   **Description**: The Qt User Interface Compiler (`uic`) incorrectly generates `setClass` or `setCssClass` attributes in the `ui_mainwindow.h` file for some UI elements, particularly buttons. This can lead to runtime errors or unexpected behavior.
    *   **Status**: Partial patches have been applied to some buttons, but a comprehensive and robust solution is required to address this for all affected elements.
    *   **Impact**: Potential UI inconsistencies or failures.

2.  **Obsolete Widget References in C++ Code**:
    *   **Description**: Following UI changes, some C++ code still contains references to widgets that no longer exist. These references have been temporarily commented out to allow the project to compile.
    *   **Status**: Awaiting refactoring to cleanly remove these obsolete references and ensure code integrity.
    *   **Impact**: Code clutter, potential for confusion, and risk of errors if commented-out code is inadvertently reactivated.

3.  **Windows Build Script and Vulkan SDK**:
    *   **Description**: The Windows build script (`build_windows.ps1`) has been updated to download the Vulkan SDK directly.
    *   **Status**: The functionality of this direct download and the overall environment setup needs to be verified on fresh Windows systems to ensure reliability.
    *   **Impact**: Potential build failures or runtime issues on Windows environments if the SDK is not correctly set up.

4.  **`TODO.md` Items**:
    *   **Description**: The `TODO.md` file lists several pending items:
        *   Placeholders for checks in `.git/hooks/sendemail-validate.sample`.
        *   Refactoring and enhancement comments within `realcugan-ncnn-vulkan/src/stb_image.h` (a third-party library).
    *   **Status**: These items are pending action.
    *   **Impact**: Minor; `sendemail-validate.sample` is a non-critical sample file. `stb_image.h` changes are within a submodule.

## Recommended Next Steps

1.  **Address `uic` Bug**: Investigate and implement a stable solution for the `setClass`/`setCssClass` issue in `ui_mainwindow.h`. This might involve a post-processing script for `uic`-generated files or finding an alternative way to apply styling.
2.  **Refactor Obsolete Code**: Systematically remove all commented-out code and obsolete widget references from the C++ sources. Ensure cross-platform builds remain successful after these changes.
3.  **Verify Windows Build**: Conduct tests of the Windows build process on a clean machine to confirm the Vulkan SDK download and environment setup are working correctly.
4.  **Process `TODO.md`**: Convert the items in `TODO.md` into actionable GitHub issues or integrate them into the project's task tracking system.

## Related Memories
- [[2025-06-21T091949Z-known-issues.md]]
- [[2025-06-19T022947Z-liquidglass-qsb-docs.md]] (Linked from the parent report)
- [[2025-06-19T022900Z-mainwindow-ui-test.md]] (Linked from the parent report)
