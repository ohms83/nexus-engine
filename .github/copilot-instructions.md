# Nexus Engine - AI Coding Assistant Instructions

## Project Overview
**Nexus** is a modern C++20 graphics engine with an ECS (Entity-Component-System) architecture. It combines SDL3 for windowing, OpenGL for rendering, and EnTT for entity management. The project includes a scene graph system, asset managers, and a material/physics rendering pipeline.

**Key Technologies**: C++20, CMake 3.31+, SDL3, OpenGL 3+, EnTT, ImGui, GLM

---

## Architecture & Key Components

### Engine Core (engine/)
- **Engine** (Engine.h/cpp): Singleton managing all subsystems - RenderSystem, TextureManager, MaterialManager, ModelManager, TaskScheduler, SceneManager
- **Application** (Application.h): Base class for applications; manages window, main loop, frame timing
- **Initialization Flow**: `Engine::Initialize()` → creates managers → registers ECS components → initializes modules

### Scene System (engine/include/nexus/scene/)
- **Scene Graph**: Hierarchy-based with `SceneNode` as base (supports parent-child relationships)
- **Entity-Component Pattern**: Uses EnTT for efficient component storage; all nodes wrap an `entt::registry`
  - Components use `IMPLEMENT_COMPONENT(Type)` macro to register with the reflection system
  - Key components: Position, Orientation, Scale (transform), Model, Light, Camera
- **SceneManager**: Manages multiple Scene instances; handles load/unload
- **Important Files**:
  - [SceneNode.h](../engine/include/nexus/scene/SceneNode.h) - Base for all scene objects
  - [Scene.h](../engine/include/nexus/scene/Scene.h) - Root of scene hierarchy
  - [Component.h](../engine/include/nexus/ecs/Component.h) - Component registration mechanism

### Graphics Subsystem (engine/include/nexus/graphics/)
- **RenderSystem**: Manages rendering context, frame timing, draw calls
- **RenderingInterface**: Abstract interface for graphics API (implementations in opengl/)
- **Manager Pattern**:
  - `TextureManager`: Resource caching for textures (loads via TextureLoader)
  - `MaterialManager`: Material definitions and shaders
  - `ModelManager`: Model loading and mesh caching (uses ASSIMP for format support)
- **Render Pipeline**:
  - `SceneRenderer` (base class) → `ForwardSceneRenderer`, `DummySceneRenderer`
  - Uses `RenderPass` system for composable rendering stages
  - Frustum culling via Frustum/Sphere geometry (geom/)

### Type System & Conventions
- **Namespace**: All engine code in `nxs` namespace; use `NXS_NAMESPACE { }` macro
- **Smart Pointers**: 
  - `Ref<T>` = `std::shared_ptr<T>` (reference-counted)
  - `Ptr<T>` = `std::unique_ptr<T>` (auto-release)
- **Casting**: Use convenience macros: `PTR_CAST` (dynamic_ptr), `D_CAST`, `CAST`, `C_CAST`, `R_CAST`
- **Identifiers**: `Identifier` = `uint32_t`; `InvalidID = 0`, `MaxIdentifier = UINT32_MAX`

---

## Developer Workflows

### Building
```bash
# Build (auto-detects Visual Studio on Windows)
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Debug

# Or use existing build directories
cd cmake-build-debug
cmake --build . --config Debug
```

### Testing
- **Framework**: Google Test (GTest 1.17.0)
- **Run all tests**: 
  ```bash
  ctest --build-config Debug  # or Release
  ```
- **Test data location**: Tests reference `TEST_DATA_FOLDER` macro (set to `tests/test_data`)
- **Structure**: Tests mirror engine structure (tests/scene/, tests/graphics/, etc.)

### Debugging Graphics
- **Remotery profiler** is integrated (Remotery.h/c in 3rdparty)
- **ImGui integration**: Already configured for SDL3 + OpenGL3 backends
- **Profiling**: Frame time available via `RenderSystem::GetrenderTime()`, draw count via `GetDrawCount()`

---

## Code Patterns & Conventions

### Component Definition Pattern
```cpp
// Header: ecs/component/MyComponent.h
struct MyComponent : public IComponent
{
    IMPLEMENT_COMPONENT(MyComponent);
    // ... data members
};

// Registration in Engine::InitModules()
IComponent::RegisterComponent<MyComponent>();
```

### Scene Node Creation
```cpp
// Create typed nodes with template method
auto modelNode = scene->EmplaceChild<ModelNode>("MyModel");
modelNode->SetName("Updated Name");
auto children = modelNode->GetAllDescendants(/*output*/, /*parentFirst=*/true);
```

### Rendering Implementation
```cpp
// SceneRenderer subclasses override Render()
class CustomRenderer final : public SceneRenderer {
    void Render(RenderSystem& renderSystem, const entt::registry& registry) override {
        // Query components via registry.view<ComponentType>()
        // Use registered render passes: GetRenderPasses()
    }
};
```

### Resource Manager Pattern
```cpp
// Managers use path-based loading and caching
auto texture = Engine::Instance().GetTextureManager()->Load("assets/textures/diffuse.png");
// Resources auto-purge unused via PurgeUnused() call
```

### Logging
```cpp
DECLARE_LOG_EXTERN(MyCategory);  // Header
DEFINE_LOG(MyCategory);           // Implementation
// Then: LOG_INFO(LogMyCategory, "message");
```

### Reflection (Property Inspection)
```cpp
class MyClass : public IReflection {
    IMPLEMENT_REFLECTION(MyClass);
    void AcceptReflector(IReflector& reflector) override {
        reflector.VisitProperty("myProperty", typeid(int), &myValue);
    }
};
```

---

## Project-Specific Gotchas

1. **EnTT Integration**: Scene nodes wrap an `entt::registry`; components accessed via `registry.view<T>()` pattern
2. **Singleton Pattern**: Engine uses `Engine::Instance()` throughout; verify `IsShuttingDown()` in destructors
3. **Component Registration**: Must register in `InitModules()` or components won't be found by reflection
4. **Smart Pointer Casting**: `PTR_CAST<DerivedType>(basePtr)` (dynamic_pointer_cast), not `dynamic_cast`
5. **Asset Paths**: Relative to `assets/` folder structure (meshes/, shaders/, textures/, config/)
6. **Editor Integration**: `editMode` flag in ApplicationConfig enables ImGui editor UI

---

## File Structure Quick Reference
- **engine/include/nexus/** - Public API headers (mirrored in src/)
- **engine/src/** - Implementation (core/, ecs/, graphics/, scene/, time/, io/, memory/, math/)
- **engine/3rdparty/** - Vendored dependencies (SDL3, imgui, glm, entt, rapidjson, etc.)
- **examples/** - Standalone example applications
- **tests/** - Google Test suite
- **assets/** - Shaders, meshes, textures, config files
- **editor/** - Editor application (separate executable)

---

## Cross-Component Communication
- **Manager access**: Via `Engine::Instance().Get[Manager]()`
- **Scene graph traversal**: `SceneNode::FindNode()`, `GetAllChildren()`, `GetAllDescendants()`
- **ECS queries**: `registry.view<ComponentA, ComponentB>()` for multi-component systems
- **Render pass registration**: `SceneRenderer::RegisterRenderPass()` for extensible pipelines
