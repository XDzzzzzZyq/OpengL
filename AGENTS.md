# AGENTS.md
# Guidance for agentic coding in this repo

This repository is a C++20 OpenGL 4.6 renderer with strict layer isolation
(Renderer ↔ Editor ↔ UI). Use this file as the single source of truth for
commands, architectural rules, and code style expectations.

------------------------------------------------------------------------------
BUILD / LINT / TEST
------------------------------------------------------------------------------

Primary build system: Visual Studio solution + msbuild.

Build (Windows, VS toolchain):
- Debug x64
  msbuild /m /p:Configuration=Debug,Platform=x64 OpenGL.sln
- Release x64
  msbuild /m /p:Configuration=Release,Platform=x64 OpenGL.sln
- Debug Win32
  msbuild /m /p:Configuration=Debug,Platform=Win32 OpenGL.sln

CI reference:
- See `.github/workflows/msbuild.yml` for the exact matrix and steps.

Tests:
- Test binary output path (from VCXProj):
  test\renderer_test\build\<Platform>\<Config>\renderer_test.exe
- Run all tests:
  .\test\renderer_test.exe

Run a single test (GoogleTest filter):
- List tests:
  .\test\renderer_test.exe --gtest_list_tests
- Run a single test case:
  .\test\renderer_test.exe --gtest_filter=RendererEnvir.RenderShader
- Run a suite:
  .\test\renderer_test.exe --gtest_filter=RendererEnvir.*

OpenGL environment:
- Full test coverage requires OpenGL 4.6.
- If GL 4.6 is unavailable, tests will skip (see gtest skips in tests).

Lint / format:
- No repo-wide lint or formatter configured.
- There is a `.clang-format` under `test/gtest/` (third-party). Do not apply
  clang-format to project code unless explicitly requested.

------------------------------------------------------------------------------
COPILOT / CURSOR RULES
------------------------------------------------------------------------------

Copilot instructions (must follow):
- File: `.github/copilot-instructions.md`
- Highlights:
  - Strict layer isolation: Renderer ↔ Editor ↔ UI (no direct coupling).
  - Minimal global state.
  - Explicit data flow via Context and Event systems.
  - Renderer is stateless with respect to application logic.
  - GPU resources are owned and managed explicitly.
  - Before commit: build Debug+Release (x64), no warnings, tests pass, no
    cross-layer dependency violations, no OpenGL state leaks.

Cursor rules:
- None found in `.cursor/rules/` or `.cursorrules`.

------------------------------------------------------------------------------
ARCHITECTURE RULES (HARD REQUIREMENTS)
------------------------------------------------------------------------------

Layer boundaries:
- Renderer: pure rendering service; owns GPU resources; consumes Context;
  must not mutate application-level state; must not depend on Editor or UI.
- Editor: application logic and scene mutation; owns Controllers; communicates
  via Context and Events; must not directly manipulate GPU resources.
- UI: presentation only; must not mutate Renderer directly; changes flow
  through Controllers or Events.

Communication:
- Cross-layer communication MUST go through:
  - Context system (data)
  - Event system (signals)
- Direct coupling across layers is forbidden.

Threading:
- Systems are generally not thread-safe. Assume main-thread only.

------------------------------------------------------------------------------
CODE STYLE GUIDELINES
------------------------------------------------------------------------------

Language / standard:
- C++20 (see project settings and msbuild configuration).

Formatting:
- Indentation: tabs are common in existing files.
- Braces: Allman-style braces are common in headers and cpp files.
- Keep line lengths reasonable; avoid excessive wrapping.

Includes:
- Prefer local project headers before system headers in the same file.
- Group includes loosely by: local headers, third-party (GL/GLFW/GLM), STL.
- Use `#pragma once` in headers.

Naming:
- Types / classes: PascalCase (e.g., `RenderConfigs`, `Transform3D`).
- Methods: PascalCase (e.g., `ApplyTransform`, `GetConfig`).
- Members: existing code uses prefixes and mixed styles:
  - `r_` for renderer/config state, `o_` for object data, `is_` for flags,
    `tex_` for texture fields, etc.
  - Keep the surrounding style when modifying a file.
- Enums: PascalCase for enum types; enum values often PascalCase or ALL_CAPS
  depending on file. Match local conventions.

Types / ownership:
- Prefer explicit ownership and RAII for GPU resources.
- Use `std::unique_ptr`/`std::shared_ptr` when sharing ownership explicitly.
- Avoid raw owning pointers; raw pointers are used for non-owning references.

Const correctness:
- Use `const` for non-mutating methods and references.
- Context is passed as `const Context&` to Renderer.

Error handling / validation:
- OpenGL error checks are wrapped by macros (see `src/extra/macros.h`).
- Use `GLERRTEST` / `GLDEBUG` in debug-only checks if available in the file.
- For invalid state, prefer early returns or `GTEST_SKIP()` in tests.

Tests (GoogleTest):
- Tests live in `test/renderer_test/`.
- Use `TEST_F(RendererEnvir, Name)` when the fixture is required.
- Use `GTEST_SKIP()` when GL capabilities are insufficient.

Comments / documentation:
- Doxygen-style block comments are used extensively in headers.
- If you add public APIs, include `@brief`, `@param`, `@return`, `@note`.
- Inline comments follow Blender C/C++ style: `// comment` with a space.

Shader / resource conventions:
- Shader resources are in `res/shaders/`.
- Texture slots and offsets are documented in `src/render/Texture.h`.
- When adding new textures or shader stages, update slot usage comments.

File layout references:
- Code: `src/`
- Tests: `test/`
- Resources: `res/`
- Dependencies: `dep/`

------------------------------------------------------------------------------
PRACTICAL GUIDANCE FOR AGENTS
------------------------------------------------------------------------------

- Respect layer isolation; do not introduce new cross-layer headers.
- Prefer Context/Event-driven flows rather than direct calls across layers.
- Do not add global state unless a file already uses it and there is no
  alternative.
- When extending renderer behavior, keep GPU ownership inside Renderer or
  dedicated resource managers; avoid leaking OpenGL handles outward.
- Avoid formatting churn; keep edits minimal and localized.

------------------------------------------------------------------------------
KNOWN COMMAND PATHS (WINDOWS)
------------------------------------------------------------------------------

Open solution:
- `OpenGL.sln`

Build artifacts for tests:
- `test\renderer_test\build\<Platform>\<Config>\renderer_test.exe`

------------------------------------------------------------------------------
END
------------------------------------------------------------------------------
