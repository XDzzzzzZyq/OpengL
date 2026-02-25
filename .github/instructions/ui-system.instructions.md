# UI System

## Overview

The UI layer is a **pure presentation layer** built on ImGui. It displays data from Context and sends user actions as Events or Controller calls. It must not directly mutate Renderer or Scene state.

## Location
- `src/UI/ImguiManager.h` - UI system coordinator
- `src/UI/ImguiLayer.h` - Base class for UI panels
- `src/UI/ImguiMenu.h` - Menu system base
- `src/UI/ImguiItem.h` - Reusable UI widgets
- `src/UI/layer/` - Specific panels (Viewport, Outliner, etc.)
- `src/UI/item/` - Widget implementations
- `src/UI/menu/` - Menu components

## Core Responsibilities

1. **Data Presentation**
   - Read Context to display current state
   - Show scene hierarchy (Outliner)
   - Display object properties (Transform Panel)
   - Visualize render settings (RenderConfig Viewer)

2. **User Input Handling**
   - Capture mouse/keyboard events
   - Provide interactive widgets (sliders, buttons, text inputs)
   - Emit Events for state changes
   - Call Controllers for complex actions

3. **Layout Management**
   - Docking system via ImGui
   - Multi-viewport support
   - Panel visibility and arrangement
   - Theme and styling

4. **Visual Feedback**
   - Highlight selected objects
   - Show active camera frustum
   - Display debug visualizations
   - Render gizmos (transform handles)

## Architecture Pattern

### ImguiManager
- Coordinates all UI panels and menus
- Manages ImGui context lifecycle
- Handles docking and window management
- Owns Layer and Menu instances

### ImguiLayer (Base Class)
```cpp
class ImguiLayer {
public:
    virtual ~ImguiLayer() = default;

    // Register UI-relevant events with the event system
    virtual void RegisterEvents(EventPool& pool) {};

    // Called every frame to render this UI layer
    virtual void RenderLayer(const Context& ctx, const EventPool& evt) {};

    // Control whether this layer is currently active / rendered
    bool uly_activate = true;      // Whether layer is active (receives events)
    bool uly_is_rendered = true;   // Whether layer is rendered (visible)

protected:
    std::string uly_name;          // Layer name for display and lookup
    GLuint uly_ID = -1;            // Unique layer ID assigned by ImguiManager
    ImLayerType uly_type;          // Layer type identifier
};
```

**Derived Layers:**
- **Viewport** - 3D scene view with picking and gizmos
- **Outliner** - Scene hierarchy tree
- **TransformPanel** - Object transform editor
- **MaterialViewer** - Material properties
- **RenderConfigViewer** - Render settings
- **ShaderEditor** - GLSL code editor
- **ParamControl** - Generic parameter panel

### ImguiItem (Reusable Widgets)
```cpp
class ImguiItem {
    virtual void Draw() = 0;
};
```

**Widget Types:**
- **Button** - Clickable buttons
- **Text** - Labeled text display
- **TextEditor** - Multi-line code editor
- **TextureViewer** - Texture preview
- **NodeEditor** - Node graph (WIP)
- **ParaInput** - Parameter input fields

### ImguiMenu (Menus)
- **ImguiMButton** - Menu buttons
- **ImguiMOption** - Dropdown options
- **ImguiMSwitch** - Toggle switches

## Data Flow

```
Context (read) → UI Display
User Action → Event Emit → Editor/Controller
```

**UI is Read-Only:**
- UI queries Context for display data
- Never mutates Context directly
- Changes propagate via Events

**Example: Changing Camera Position**
```
User drags gizmo in Viewport
  ↓
Viewport emits CameraEvent
  ↓
CameraController receives event
  ↓
CameraController updates Camera transform
  ↓
SceneContext reflects new transform
  ↓
Viewport reads Context on next frame
  ↓
Gizmo drawn at new position
```

## Key UI Panels

### Viewport
- Displays rendered scene from active camera
- Handles mouse picking for object selection
- Renders transform gizmos
- Supports camera controls (orbit, pan, zoom)
- Emits ViewportEvents for resize

**Interaction Flow:**
1. User clicks in viewport
2. Viewport performs ray-cast picking
3. Emits SelectionEvent with picked ObjectID
4. SelectionManager updates
5. Outliner highlights selected object

### Outliner
- Tree view of scene hierarchy
- Shows parent-child relationships
- Icons for object types (Camera, Light, Mesh, etc.)
- Selection synchronization with viewport
- Drag-and-drop for reparenting

### TransformPanel
- Position, Rotation, Scale inputs
- Reads selected object's Transform
- Emits TransformEvent on user edit
- Supports numeric input and sliders

### MaterialViewer
- Material properties (albedo, metallic, roughness)
- Texture preview and assignment
- Emits MaterialEvent on changes
- Live updates visible in viewport

### RenderConfigViewer
- Render settings UI (resolution, AA, AO, shadows)
- Binds to RenderConfigs in RenderContext
- Changes immediately affect rendering
- Presets for quality levels

### ShaderEditor
- Multi-tab code editor for GLSL
- Syntax highlighting
- Compile button
- Error display
- Structure analysis view (uniforms, attributes)

## Architectural Boundaries

### ✅ UI MAY:
- Read Context (const access)
- Emit Events
- Call Controller methods (for complex actions)
- Display ImGui widgets
- Handle ImGui input events

### ❌ UI MUST NOT:
- Directly mutate Scene objects
- Call OpenGL functions
- Access Renderer internals
- Modify Context state
- Depend on specific Renderer implementation

## Event Emission Patterns

**Direct Property Edit:**
```cpp
if (ImGui::DragFloat("Roughness", &value)) {
    events.emit(MaterialPropertyChanged{obj, "roughness", value});
}
```

**Button Actions:**
```cpp
if (ImGui::Button("Add Light")) {
    events.emit(CreateObjectEvent{ObjectType::PointLight});
}
```

**Selection Changes:**
```cpp
if (ImGui::Selectable(obj->name, is_selected)) {
    events.emit(SelectionEvent{obj});
}
```

## ImGui Integration

### Initialization
- `ImguiManager(EventPool&, Window&)` constructor creates the ImGui context, initializes GLFW + OpenGL3 backends, and applies theme/style (RAII — no separate `Init()` call)
- ImGui backends: GLFW + OpenGL3
- Custom font and theme applied
- Destructor (`~ImguiManager`) shuts down backends and destroys the ImGui context

### Rendering Loop
```cpp
void ImguiManager::NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImguiManager::Render() {
    for (auto& layer : layers) {
        if (layer->is_visible) {
            layer->OnRender();
        }
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
```

### Docking
- Main dockspace created by ImguiManager
- Panels dock into dockspace
- Layout saved/loaded from `imgui.ini`

## Theming

**ImguiTheme** (`src/UI/ImguiTheme.h`)
- Color scheme configuration
- Style settings (rounding, padding, etc.)
- Supports light/dark themes
- Applied globally to all UI

## Performance Considerations

- ImGui is immediate-mode (rebuilt every frame)
- Keep `OnRender()` lightweight
- Avoid heavy computations in UI code
- Defer expensive operations to background threads
- Throttle updates for real-time displays (e.g., FPS counter)

## Gizmo System

**Transform Gizmos:**
- Translate (arrows)
- Rotate (circles)
- Scale (cubes)
- Rendered in viewport after scene
- Interactive manipulation
- Local vs. World space modes

**Implementation:**
- Custom OpenGL rendering
- Ray-intersection for picking
- Event emission for transform changes

## Extension Points

**Adding a New Panel:**
1. Create class inheriting from `ImguiLayer`
2. Implement `OnRender()` with ImGui calls
3. Add instance to ImguiManager
4. Register event subscriptions in constructor

**Adding a New Widget:**
1. Create class inheriting from `ImguiItem`
2. Implement `Draw()` method
3. Reuse across multiple panels
4. Encapsulate common UI patterns

## Future Enhancements

- Node editor for shader graphs
- Advanced timeline/animation UI
- Performance profiler overlay
- Customizable shortcuts
- Plugin system for user extensions
