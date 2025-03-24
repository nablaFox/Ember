## Etna Engine

Simple C++ 20 graphics engine written with [Ignis](https://github.com/nablaFox/Ignis).

### Example


```cpp
Engine engine;

Mesh mesh = engine.createMesh();
Material material = engine.createMaterial();

// mesh will point to a single buffer allocated for primitives
Mesh spherePrimitive = engine.createSphere();
Mesh cubePrimitive = engine.createCube();

Window window(configs);

while (true) {
    engine.beginFrame();
    engine.setCamera(camera);
    engine.setRenderTarget(window);

    engine.draw(mesh, material);

    engine.drawInstanced(instanceBuffer, mesh, material, transform);

    window.swapBuffers();

    engine.endFrame();
}

return 0;

// The idea of a scene where organizing drawable things is a further abstraction that will be added later
```


