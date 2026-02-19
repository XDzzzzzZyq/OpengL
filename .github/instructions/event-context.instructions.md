# Event and Context Systems

## Overview

The Event and Context systems are the **communication backbone** of the renderer architecture. They enable decoupled, type-safe interaction between the Renderer, Editor, and UI layers.

---

## Context System

### Purpose
The Context system provides **explicit data flow** across layers. It aggregates read-only views of application state that can be safely queried without coupling.

### Design Principles
1. **Immutability** - Layers below cannot mutate context they receive
2. **Explicit ownership** - Context data is owned by Editor, read by others
3. **No hidden state** - All queryable state is in Context
4. **Type safety** - Strongly typed accessors prevent misuse

### Context Structure

```cpp
class Context {
    SceneContext scene;      // Scene graph read-only view
    EditorContext editor;    // Editor state (selections, etc.)
    RenderContext render;    // Render settings
};
```

**Ownership Model:**
- Editor owns and mutates Context
- Renderer receives `const Context&`
- UI reads Context for display
- No shared ownership - single source of truth

---

## SceneContext

### Purpose
Provides read-only access to the active scene and its objects.

### Interface
```cpp
class SceneContext {
    UID* active_scene;  // Root of scene hierarchy
    
    const ObjectID* GetActiveCamera() const;
    const ObjectID* GetActiveEnvironment() const;
    const ObjectID* GetPPS(int target) const;
    const std::vector<const ObjectID*> GetObjectIDs() const;
};
```

### Key Methods

**GetActiveCamera()**
- Returns currently active Camera object
- Used by Renderer to obtain view/projection matrices
- Const pointer enforces read-only semantics

**GetActiveEnvironment()**
- Returns active Environment for IBL
- Provides HDR environment map
- Used in lighting calculations

**GetPPS(int target)**
- Returns post-processing stack for target
- Enables multi-pass effects

**GetObjectIDs()**
- Returns all renderable objects in scene
- Renderer iterates for draw calls
- Const ensures objects not mutated during render

### Usage Pattern (Renderer)
```cpp
void Renderer::Render(const Context& ctx) {
    auto* camera = ctx.scene.GetActiveCamera();
    auto* env = ctx.scene.GetActiveEnvironment();
    
    // Use camera and environment for rendering
    // Cannot modify them (const pointers)
}
```

---

## EditorContext

### Purpose
Stores editor-specific state that doesn't belong in Scene or Renderer.

### Interface
```cpp
class EditorContext {
    SelectionManager<ObjectID> selections;
    
    // Future extensions:
    // UndoStack undo_history;
    // Clipboard clipboard;
    // Tool tool_state;
};
```

### Selection Management

**SelectionManager** tracks user-selected objects:
- Multi-selection support
- Query: `IsSelected(obj)`, `GetSelected()`
- Mutate: `Select(obj)`, `Deselect(obj)`, `Clear()`

**UI Integration:**
- Outliner queries selections to highlight
- Viewport renders selection outline
- Transform panel shows selected object properties

---

## RenderContext

### Purpose
Links to rendering configuration without coupling to RenderConfigs internals.

### Interface
```cpp
class RenderContext {
    RenderConfigs* config;  // Non-owning pointer
    
    RenderConfigs* GetConfig() const;
};
```

### RenderConfigs (Brief)
Contains user-facing render settings:
- Resolution
- Anti-aliasing mode (FXAA, MSAA)
- Ambient occlusion (SSAO, GTAO)
- Shadow quality
- Tone mapping curve
- Exposure

**Ownership:**
- Created and owned by Editor or Application
- Linked via RenderContext
- Renderer reads settings without owning

---

## Event System

### Purpose
The Event system enables **decoupled communication** via type-safe publish-subscribe. Layers emit events without knowing who (if anyone) is listening.

### Design Principles
1. **Type safety** - Templates prevent type errors
2. **Decoupling** - Emitters don't know subscribers
3. **No polling** - Push-based notification
4. **Flexibility** - Any component can subscribe

### EventPool Implementation

```cpp
class EventPool {
    template<typename Event>
    void subscribe(Handler<Event> handler);
    
    template<typename Event>
    void emit(const Event& event) const;
    
private:
    std::unordered_map<
        std::type_index,
        std::vector<std::function<void(const void*)>>
    > handlers;
};
```

**How It Works:**
- `subscribe<Event>(handler)` registers a typed handler
- `emit<Event>(event)` calls all handlers for that Event type
- `std::type_index` provides runtime type discrimination
- Type erasure via `void*` with safe casting

### Event Categories

**Input Events** (`src/editor/events/KeyMouseEvents.h`)
- Mouse click, drag, scroll
- Keyboard press, release
- Window focus, resize

**Camera Events** (`src/editor/events/CameraEvents.h`)
- Camera move, rotate
- Projection change (ortho/perspective)
- Active camera switch

**Viewport Events** (`src/editor/events/ViewportEvents.h`)
- Viewport resize
- Viewport focus/blur
- Render mode change

**Shader Events** (`src/editor/events/ShaderEvents.h`)
- Shader compile success/failure
- Shader reload
- Uniform update

**Material Events** (`src/editor/events/MaterialEvents.h`)
- Material property change
- Texture assignment
- Material create/delete

**Editor Events** (`src/editor/events/EditorEvents.h`)
- Object create/delete
- Selection change
- Scene load/save

### Usage Patterns

**Subscribing:**
```cpp
void Init(EventPool& pool) {
    pool.subscribe<ViewportResizeEvent>([this](const auto& e) {
        this->OnViewportResize(e.width, e.height);
    });
}
```

**Emitting:**
```cpp
void OnUserResizeViewport(int w, int h) {
    events.emit(ViewportResizeEvent{w, h});
}
```

**Multi-Subscriber:**
- Multiple handlers can subscribe to same event
- All handlers called in subscription order
- Handlers should not assume execution order

### Event Design Guidelines

**Event Structs:**
```cpp
struct ViewportResizeEvent {
    int width;
    int height;
};
```
- Plain data structs (no behavior)
- Const members preferred
- Minimal data (only what subscribers need)
- Descriptive names

**Handler Signature:**
```cpp
using Handler<Event> = std::function<void(const Event&)>;
```
- Handlers receive `const Event&`
- Cannot modify event (prevents side effects)
- Return type is `void` (fire-and-forget)

---

## Communication Patterns

### Pattern 1: UI → Editor → Renderer

**Scenario:** User changes material roughness

1. UI slider changed
2. UI emits `MaterialPropertyChanged`
3. Editor subscribes to event, updates Material object
4. Context now reflects new material state
5. Renderer reads Context on next frame
6. Rendered result shows updated material

**Code Flow:**
```
UI: events.emit(MaterialPropertyChanged{obj, "roughness", 0.5});
  ↓
Editor: [subscribed handler] obj->SetRoughness(0.5);
  ↓
Renderer: auto roughness = material->GetRoughness(); // reads 0.5
```

### Pattern 2: Viewport Resize Propagation

**Scenario:** User resizes viewport panel

1. ImGui detects viewport size change
2. Viewport panel emits `ViewportResizeEvent`
3. Renderer subscribes, resizes framebuffers
4. Camera subscribes, updates aspect ratio
5. Next frame renders at new resolution

**Code Flow:**
```
Viewport: events.emit(ViewportResizeEvent{1920, 1080});
  ↓
Renderer: [subscribed] ResizeFramebuffers(1920, 1080);
Camera:   [subscribed] UpdateAspectRatio(1920.0 / 1080.0);
```

### Pattern 3: Shader Recompilation

**Scenario:** User edits shader in ShaderEditor

1. User presses "Compile" button
2. ShaderEditor emits `ShaderCompileRequest`
3. ShaderController subscribes, attempts compilation
4. On success: emits `ShaderCompileSuccess`
5. On failure: emits `ShaderCompileError{errorLog}`
6. ShaderEditor subscribes to both, shows result

**Code Flow:**
```
ShaderEditor: events.emit(ShaderCompileRequest{shader_id});
  ↓
ShaderController: [subscribed] CompileShader(shader_id);
  if (success) events.emit(ShaderCompileSuccess{shader_id});
  else         events.emit(ShaderCompileError{shader_id, log});
  ↓
ShaderEditor: [subscribed to both]
  OnSuccess: ShowMessage("Compile OK");
  OnError:   DisplayErrorLog(log);
```

---

## Architectural Invariants

### Context System
1. **Renderer receives const Context** - Cannot mutate
2. **Editor owns Context** - Single source of truth
3. **UI reads Context** - Display only, no mutation
4. **No Context copies** - Pass by reference

### Event System
1. **Events are immutable** - Const reference in handlers
2. **Events are data-only** - No methods except constructors
3. **Handlers are stateless** - Or manage their own state
4. **No handler ordering guarantees** - Avoid dependencies
5. **Events are synchronous** - All handlers run before `emit()` returns

---

## Anti-Patterns to Avoid

### ❌ Mutating Context in Renderer
```cpp
void Renderer::Render(const Context& ctx) {
    auto* obj = const_cast<ObjectID*>(ctx.scene.GetActiveCamera());
    obj->SetPosition(...);  // FORBIDDEN: Renderer mutating scene
}
```

### ❌ Direct Layer Coupling
```cpp
class UI {
    Renderer* renderer;  // FORBIDDEN: Direct dependency
    
    void OnButtonClick() {
        renderer->SetResolution(...);  // Should use Event instead
    }
};
```

### ❌ Modifying Events
```cpp
pool.subscribe<MyEvent>([](const MyEvent& e) {
    const_cast<MyEvent&>(e).value = 42;  // FORBIDDEN: Event mutation
});
```

### ❌ Implicit State in Event Handlers
```cpp
// BAD: Handler depends on execution order
int counter = 0;
pool.subscribe<Event>([&](const auto& e) { counter++; });
pool.subscribe<Event>([&](const auto& e) { assert(counter == 1); }); // Fragile
```

---

## Extension Points

### Adding New Context Data
1. Add member to appropriate Context (Scene/Editor/Render)
2. Provide const getter
3. Document ownership and lifetime
4. Update layers to use new data

### Adding New Event Type
1. Define struct in appropriate `events/` file
2. Subscribers use `pool.subscribe<NewEvent>(...)`
3. Emitters use `pool.emit(NewEvent{...})`
4. No registration required (template-based)

---

## Future Enhancements

### Context System
- Versioned Context for undo/redo
- Context snapshots for background rendering
- Performance Context (metrics, profiling)

### Event System
- Event replay for debugging
- Event filtering/routing
- Async event handling for heavy tasks
- Event priorities
