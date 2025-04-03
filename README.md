## Etna Engine

Simple C++ 20 graphics engine written with [Ignis](https://github.com/nablaFox/Ignis).

### Example

```cpp
#include "shared.hpp"

constexpr uint32_t WINDOW_WIDTH{800};
constexpr uint32_t WINDOW_HEIGHT{600};

int main(void) {
	engine::init();

	Window window({
		.width = WINDOW_WIDTH,
		.height = WINDOW_HEIGHT,
		.title = "Basic Etna",
		.captureMouse = true,
	});

	Scene scene;
	SceneNode root = scene.createRoot("root", {});

	MeshNode& sphere = root.addMesh("Sphere", engine::getSphere(),
					{
						.position = {1.5, 0.5, -5},
						.scale = Vec3(0.5),
					},
					engine::createGridMaterial({
						.color = BLUE * 0.08,
						.gridColor = BLUE,
						.gridSpacing = 0.1,
						.thickness = 0.005,
					}));

	CameraNode& playerCamera = root.addCamera("PlayerCamera", {.position = {0, 1, 0}});

	Renderer renderer({});

	while (!window.shouldClose()) {
		window.pollEvents();

		updateFirstPersonCamera(playerCamera, window);

		sphere.rotate(0, 0.01, 0.01);

		renderer.beginFrame();

		renderer.renderScene(scene, window, playerCamera);

		renderer.endFrame();

		window.swapBuffers();
	}
}
```

**Output**:

![Etna Demo Gif](./examples/example.gif)

You can find more examples in the `examples` directory.

### Building

```bash
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
