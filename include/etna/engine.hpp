#pragma once

#include "ignis/types.hpp"
#include "ignis/device.hpp"
#include "mesh.hpp"
#include "material.hpp"

namespace etna {

struct Camera;
struct Transform;
class RenderTarget;

class Engine {
public:
	struct CreateInfo {
		const char* appName{"Etna Application"};
	};

	Engine(const CreateInfo&);

	~Engine();

	void beginFrame();

	void endFrame();

	void setCamera(const Camera& camera);

	void setRenderTarget(RenderTarget*);

	void draw(const Mesh, Transform, const Material = nullptr);

	void drawInstanced(BufferId,
					   const Mesh,
					   const Material);	 // TODO: move into ignis ns

	Mesh createMesh(const _Mesh::CreateInfo&);

	Material createMaterial(const _Material::CreateInfo&);

	Mesh createSphere();

	Mesh createCube();

private:
	ignis::Device* m_device;
	_Material* m_defaultMaterial;
	RenderTarget* m_currentRenderTarget;
	BufferId primitivesVertices;
	BufferId primitivesIndices;

public:
	Engine(const Engine&) = delete;
	Engine(Engine&&) = delete;
	Engine& operator=(const Engine&) = delete;
	Engine& operator=(Engine&&) = delete;
};

}  // namespace etna
