# Documentation System

## Overview

The project documentation is built with [MkDocs](https://www.mkdocs.org/) using the Material theme.
API reference pages are generated automatically from C++ source comments via the [mkdoxy](https://github.com/JakubAndrysek/MkDoxy) plugin.
Markdown macros (e.g., class link shortcuts) are provided by the [mkdocs-macros-plugin](https://mkdocs-macros-plugin.readthedocs.io/).

The deployed site lives at: **<https://xdzzzzzzyq.github.io/OpengL/>**

---

## Site Structure

```
docs/
├── index.md              # Home page (user-facing introduction)
├── features.md           # Feature overview
├── release.md            # Release notes / changelog
├── api.md                # API reference landing page
├── macros.py             # Macro definitions for mkdocs-macros-plugin
└── handbook/             # Developer Handbook (technical docs for contributors)
    ├── index.md          # Handbook overview + build/test quick reference
    ├── architecture.md   # Three-layer architecture overview
    ├── renderer.md       # Renderer layer documentation
    └── editor.md         # Editor layer documentation
```

### Navigation (`mkdocs.yml`)

```yaml
nav:
  - Home: index.md
  - Features: features.md
  - Release: release.md
  - Developer Handbook:
    - Overview: handbook/index.md
    - Architecture: handbook/architecture.md
    - Renderer Layer: handbook/renderer.md
    - Editor Layer: handbook/editor.md
  - API Reference: Renderer\classes
```

- **Home** — A welcoming, user-friendly introduction. Keep it light; avoid implementation details.
- **Features** — High-level capability list. Mirror the checked items in the codebase's progress trackers.
- **Release** — Changelog. Add a new section for each tagged release.
- **Developer Handbook** — Technical documentation aimed at contributors. Mirrors the `.github/instructions/` files in spirit.
- **API Reference** — Auto-generated; do not edit manually. Populated by mkdoxy from Doxygen XML.

---

## mkdoxy — Auto-generated API Reference

mkdoxy runs Doxygen internally and converts the XML output into MkDocs pages.

### Configuration (in `mkdocs.yml`)

```yaml
plugins:
  - mkdoxy:
      projects:
        Renderer:
          src-dirs: src/
          full-doc: True
          doxy-cfg:
            FILE_PATTERNS: "*.cpp *.h*"
            EXCLUDE_PATTERNS: "TextEditor.*"
            RECURSIVE: True
            GENERATE_XML: True
            EXTRACT_ALL: True
            EXTRACT_PRIVATE: True
            EXTRACT_STATIC: True
```

- The project name `Renderer` becomes the URL prefix for all generated pages (e.g., `Renderer/classContext/`).
- Pages are generated under `Renderer/` in the site output; the nav entry `Renderer\classes` links to the class index.
- `EXTRACT_ALL: True` documents every symbol, including undocumented ones.
- `EXTRACT_PRIVATE: True` and `EXTRACT_STATIC: True` expose private and static members.

### Doxygen Comment Style

Follow Doxygen-style Javadoc annotations on all public APIs:

```cpp
/**
 * @brief Short one-line description.
 *
 * Longer explanation if needed.
 *
 * @param ctx   Immutable scene context passed to the renderer.
 * @param rend  When false, the rasterization pass is skipped.
 * @return      Pointer to the completed framebuffer, or nullptr on error.
 * @note        Caller must not hold the framebuffer across frames.
 */
void Render(const Context& ctx, bool rend = true, bool buff = true);
```

Use `@brief`, `@param`, `@return`, and `@note`. Inline comments use `// comment` (space after `//`).

### Adding a New Class to the Docs

1. Add Doxygen comments to the class and its public methods in the header file.
2. Rebuild the documentation — mkdoxy will pick up the new class automatically.
3. Reference the generated page from handbook pages using the `cls()` macro (see below).

---

## Markdown Macros

Macros are defined in `docs/macros.py` and loaded by mkdocs-macros-plugin.

### `cls(name)` — Link to a Generated Class Page

```python
@env.macro
def cls(name):
    return f"[`{name}`](/OpengL/Renderer/class{name}/)"
```

**Usage in Markdown:**

```markdown
They communicate only through the {{cls("Context")}} and event system.
```

**Rendered output:**

> They communicate only through the [`Context`](/OpengL/Renderer/classContext/) and event system.

The macro constructs the absolute path `/OpengL/Renderer/class<Name>/`, which is the URL mkdoxy uses for class pages on the deployed site.

### Adding New Macros

Add new `@env.macro` decorated functions to `docs/macros.py`:

```python
@env.macro
def file(path):
    """Link to a source file on GitHub."""
    base = "https://github.com/XDzzzzzZyq/OpengL/blob/main/"
    return f"[`{path}`]({base}{path})"
```

Then use `{{file("src/render/Renderer.h")}}` in any Markdown page.

---

## Local Development

```bash
pip install mkdocs mkdocs-material mkdoxy mkdocs-macros-plugin
mkdocs serve          # live-reload server at http://127.0.0.1:8000
mkdocs build          # static output in site/
```

The CI workflow (`.github/workflows/docs_build.yml`) builds and deploys to GitHub Pages on every push to `master`.

---

## Content Guidelines

| Section | Audience | Tone |
|---|---|---|
| Home (`index.md`) | New visitors, end users | Welcoming, non-technical |
| Features (`features.md`) | Users evaluating the project | Descriptive, capability-focused |
| Release (`release.md`) | All users | Factual, version-tagged |
| Developer Handbook | Contributors, maintainers | Technical, precise |
| API Reference | Developers using the API | Auto-generated; improve via Doxygen comments |

- Do not hard-code `http://127.0.0.1:8000/` links — they only work during local development.
- Use relative links between handbook pages (e.g., `[Renderer](renderer.md)`).
- Use the `cls()` macro for links to API reference pages instead of writing raw URLs.
