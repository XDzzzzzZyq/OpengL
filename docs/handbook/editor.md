# Editor

The Editor layer handles application logic and scene state management, including controllers and the event system. It does not operate GPU resources directly, and instead passes data to the Renderer through the {{cls("Context")}}.

## Responsibilities

- Maintain scene and editing state.
- Drive controllers to execute mutations.
- Publish change signals through the event system.
- Collaborate with the UI without being driven directly by UI state.

## Constraints

- Must not depend on Renderer implementation details.
- Must not own GPU resources.
- All cross-layer communication must go through {{cls("Context")}} or events.

## Related Directories

- {{file("src/editor/")}}: Core implementation.
- {{file("src/editor/controllers/")}}: Controller logic.
- {{file("src/editor/events/")}}: Event definitions.
