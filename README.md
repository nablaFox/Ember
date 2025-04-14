## Etna Engine

Simple C++ 20 Vulkan graphics engine powered by [Ignis](https://github.com/nablaFox/Ignis).

### Example

```cpp
#include "etna/etna_core.hpp"
#include "etna/scene.hpp"

constexpr uint32_t WINDOW_WIDTH{800};
constexpr uint32_t WINDOW_HEIGHT{600};

using namespace etna;

int main(void) {
	engine::init();

	Window window({
		.width = WINDOW_WIDTH,
		.height = WINDOW_HEIGHT,
		.title = "Basic Etna",
		.captureMouse = true,
	});

	Scene scene;

	MeshNode sphere = scene.createMeshNode({
		.name = "Sphere",
		.mesh = engine::getSphere(),
		.transform =
			{
				.position = {0, 0, -3},
				.scale = Vec3(0.5),
			},
		.material = engine::createGridMaterial({
			.color = BLUE * 0.08,
			.gridColor = BLUE,
			.gridSpacing = 0.1,
			.thickness = 0.005,
		}),
	});

	CameraNode playerCamera = scene.createCameraNode({
		.name = "PlayerCamera",
		.transform = {.position = {0, 0, 1}},
	});

	Renderer renderer({});

	while (!window.shouldClose()) {
		window.pollEvents();

		sphere->rotate(0, 0.01, 0.01);

		renderer.beginFrame(window);

		scene.render(renderer, playerCamera);

		renderer.endFrame();

		window.swapBuffers();
	}
}
```

**Output**:

![Etna Demo Gif](docs/example.gif)

For more check-out [`examples`](./examples).

### Standalone Usage

Etna is a static library intended to be linked against your applications. You
can find precompiled binary releases compatible with all supported compilers on
Windows/Linux/macOS [here]().

Such releases are structured as follows:

```
etna-<version>
├─ include
├─ lib
│    ├─ libetna.a
│    └─ libglfw.a
└─ LICENSE
```

Building with Etna requires linking against the Vulkan loader, `libetna.a` and
`libglfw.a`. Linking with [GLFW](https://github.com/glfw/glfw) additionally
requires platform-specific libraries. Refer to [docs](docs/some/file) for the
appropriate linker flags for your target platform.

### CMake Usage

```cmake
add_subdirectory(path/to/etna-source)
target_link_libraries(your_target PRIVATE etna::etna)
```

### Bootstrap 

To quickly start a new Etna project, you can run the following:

```bash
SCRIPT=https://raw.githubusercontent.com/nablaFox/etna/main/scripts/bootstrap.sh
curl -sSL $SCRIPT | sh -s -- --name <project_name> --build <cmake|standalone>
```

This will create a new `<project_name>` directory and set up your project based
on the selected build type:

- `--build cmake` will create a CMake project by downloading the latest source release.

- `--build standalone` will create a standalone project using the latest
    precompiled release and setup a simple `build.sh` script targetting the
    same OS/WS as the one it is run on.

### Building

Requirements:

- CMake 3.14+
- GCC 11+ or Clang 12+
- Vulkan SDK

```bash
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build --prefix <install_path>
```

### Dependencies

- [Ignis](https://github.com/nablaFox/Ignis)
- [GLFW](https://github.com/glfw/glfw)

### Documentation

Work in progress.
