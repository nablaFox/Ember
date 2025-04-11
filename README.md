## Etna Engine

Simple C++ 20 graphics engine written with [Ignis](https://github.com/nablaFox/Ignis).

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

![Etna Demo Gif](./docs/example.gif)

For more check-out [`examples`](./examples).

### Building

Etna is built as a static library using CMake:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build  # Omit 'sudo' for local user install
```

Prebuilt releases are available [here](https://github.com/nablaFox/Etna/releases).

### Standalone Usage (Linux/X11)

After setting up your project:

```bash
mkdir my_project
cd my_project
curl -L -O "https://github.com/nablaFox/Etna/releases/download/v0.2.0/etna-0.2.0.zip"
unzip etna-0.2.0.zip
```

compile and run with:

```bash
g++ main.cpp -Ietna-0.2.0/include -Letna-0.2.0/lib \
  -letna \
  -lglfw3 \
  -lIgnis \
  -lm -ldl /usr/lib/libX11.so \
  /lib/libvulkan.so \
  -o my_project

./my_project
```

### CMake

Add Etna to your project with:

```cmake
add_subdirectory(etna_path)
target_link_libraries(your_target PRIVATE etna::etna)
```

Include paths and library linking will be handled automatically for your
platform.

### Dependencies

- [Ignis](https://github.com/nablaFox/Ignis)
- [GLFW](https://github.com/glfw/glfw)

### Documentation

Work in progress.
