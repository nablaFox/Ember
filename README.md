## Etna Engine

Simple C++ 20 graphics engine written with [Ignis](https://github.com/nablaFox/Ignis).

### Example

```cpp
int main(int argc, char* argv[]) {
	Engine engine;

	Window window({
		.width = WINDOW_WIDTH,
		.height = WINDOW_HEIGHT,
		.title = "Etna Demo",
		.captureMouse = true,
	});

	Scene scene;

	SceneNode root = scene.createRoot("root", {});

	MaterialHandle sphereMaterial = Engine::createGridMaterial({
		.color = BLUE,
		.gridSpacing = 0.1,
		.thickness = 0.005,
	});

	Transform sphereTransform = {
		.scale = 0.5,
		.pitch = M_PI / 2,
		.position = {1.5, 0.5, -5},
	};

	MeshNode& sphere = root.addMesh("Sphere", Engine::createSphere(BLUE * 0.08),
									sphereTransform, sphereMaterial);

	CameraNode& playerCamera =
		root.addCamera("PlayerCamera", {.fov = 70, .aspect = window.getAspect()},
					   {.position = {0, 1, 0}});

	Renderer renderer({});

	while (!window.shouldClose()) {
		window.pollEvents();

		updateFirstPersonCamera(playerCamera, window);

		renderer.beginFrame();

		renderer.renderScene(scene, window);

		renderer.endFrame();

		window.swapBuffers();
	}
}
```

You can find more examples in the `examples` directory.
