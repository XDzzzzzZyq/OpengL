# Editor System

## Overview

The Editor layer contains **application logic and scene mutation**. It owns the Controllers, manages user input, maintains selections, and orchestrates state changes through the Event system.

## Location
- `src/editor/Context.h` - Context definitions
- `src/editor/Events.h` - Event pool and base types
- `src/editor/Input.h` - Input handling
- `src/editor/SelectionManager.h` - Object selection state
- `src/editor/Controllers.h` - Controller base class
- `src/editor/controllers/` - Specific controller implementations
- `src/editor/events/` - Event type definitions

## Core Responsibilities

1. **Scene State Management**
   - Create, modify, delete scene objects
   - Update transforms, materials, parameters
   - Manage scene hierarchy (parent-child relationships)
   - Handle object selection

2. **Controller Orchestration**
   - CameraController - Camera movement and view manipulation
   - ShaderController - Shader compilation and editing
   - ViewportController - Viewport interaction and rendering

3. **Event Handling**
   - Subscribe to UI events (mouse, keyboard)
   - Emit state change notifications
   - Coordinate cross-system communication

4. **Context Provisioning**
   - Populate SceneContext with active scene data
   - Update EditorContext with selections
   - Provide RenderContext with settings

## Key Components

### Context System

**SceneContext** - Read-only view of the scene
```cpp
class SceneContext {
    UID* active_scene;  // Currently active scene root
    
    const ObjectID* GetActiveCamera() const;
    const ObjectID* GetActiveEnvironment() const;
    const std::vector<const ObjectID*> GetObjectIDs() const;
};
```
- Provides immutable scene data to Renderer
- Query methods return `const` pointers
- Must not expose mutating operations

**EditorContext** - Editor-specific state
```cpp
class EditorContext {
    SelectionManager<ObjectID> selections;  // Current selection set
};
```
- Tracks user selections
- May expand with undo/redo, clipboard, etc.

**RenderContext** - Rendering configuration
```cpp
class RenderContext {
    RenderConfigs* config;  // Pointer to render settings
};
```
- Links to RenderConfigs owned elsewhere
- Provides Renderer access to user settings

**Context** - Unified context container
```cpp
class Context {
    SceneContext scene;
    EditorContext editor;
    RenderContext render;
    
    void Init(EventPool& pool);
};
```
- Aggregates all context types
- Passed to Renderer and UI layers
- Single source of truth for state queries

### Event System

**EventPool** - Type-safe event dispatcher
```cpp
class EventPool {
    template<typename Event>
    void subscribe(Handler<Event> handler);
    
    template<typename Event>
    void emit(const Event& event) const;
};
```

**Design:**
- Template-based type safety
- No runtime type checks needed
- Handlers receive `const Event&`
- Decouples event sources from listeners

**Event Categories:**
- **EditorEvents** - General editor actions
- **KeyMouseEvents** - Input events (click, drag, key press)
- **CameraEvents** - Camera state changes
- **ViewportEvents** - Viewport resize, focus
- **ShaderEvents** - Shader recompilation, errors
- **MaterialEvents** - Material property updates

**Event Flow:**
```
UI action → Event emitted → Handlers notified → State updated → UI refreshes
```

### Controllers

**Base Pattern:**
```cpp
class Controller {
    virtual void Update(Context& ctx, EventPool& events);
};
```

**CameraController** (`src/editor/controllers/CameraController.h`)
- Handles camera movement (orbit, pan, zoom)
- Responds to mouse drag and scroll events
- Updates Camera transform in scene
- Emits CameraEvents on changes

**ShaderController** (`src/editor/controllers/ShaderController.h`)
- Manages shader compilation lifecycle
- Watches shader files for changes
- Triggers recompilation on edits
- Emits ShaderEvents (success/failure)

**ViewportController** (`src/editor/controllers/ViewportController.h`)
- Manages viewport interaction
- Handles picking (object selection)
- Triggers render updates
- Emits ViewportEvents

### SelectionManager

```cpp
template<typename T>
class SelectionManager {
    void Select(T* object);
    void Deselect(T* object);
    void Clear();
    bool IsSelected(const T* object) const;
    std::vector<T*> GetSelected() const;
};
```

- Generic selection container
- Used for ObjectID selection in editor
- Supports multi-select
- Thread-safe (if needed)

## Data Flow

```
User Input → Editor → Scene Mutation → Event Emission → UI Update
                  ↓
           Context Update → Renderer Read
```

**Example: Moving an Object**
1. User drags transform gizmo (UI)
2. UI emits `TransformEvent`
3. Editor receives event, updates object's Transform
4. SceneContext reflects new transform
5. Renderer reads updated transform on next frame
6. UI displays new position

## Architectural Boundaries

### ✅ Editor MAY:
- Mutate scene objects
- Own Controllers and managers
- Emit and subscribe to Events
- Update Context state
- Call into SceneManager for object creation

### ❌ Editor MUST NOT:
- Directly manipulate GPU resources
- Call OpenGL functions
- Depend on UI implementation details
- Store rendering-specific state (belongs in RenderContext)

## Integration with Other Layers

**With Renderer:**
- Provides immutable Scene via SceneContext
- Renderer NEVER calls back into Editor
- One-way dependency: Editor → Renderer (via Context)

**With UI:**
- UI reads Context for display
- UI emits Events for user actions
- Editor subscribes to UI events
- Two-way via Events, not direct calls

## Typical Workflows

### Scene Loading
1. User selects "Open Scene"
2. UI emits `LoadSceneEvent`
3. Editor calls `SceneManager::LoadScene(path)`
4. New scene objects created
5. SceneContext updated with new scene
6. UI refreshes outliner

### Object Selection
1. User clicks in viewport
2. ViewportController performs picking
3. SelectionManager updated
4. EditorContext reflects selection
5. UI highlights selected object
6. Transform panel shows properties

### Parameter Edit
1. User changes material roughness slider
2. UI emits `MaterialEvent`
3. Editor updates Material object
4. SceneContext provides updated material
5. Renderer sees new value next frame

## Future Enhancements

- Undo/Redo system (Command pattern)
- Clipboard for copy/paste
- Multi-scene editing
- Live link to external tools
- Scripting API exposure
