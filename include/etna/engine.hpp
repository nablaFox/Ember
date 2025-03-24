#pragma once

#include "ignis/types.hpp"
#include "ignis/device.hpp"
#include "image.hpp"
#include "mesh.hpp"
#include "material.hpp"

namespace etna {

struct Camera;
struct Transform;
class RenderTarget;

class Engine {
public:
	Engine();

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

	ignis::Device& getDevice() const { return *m_device; }

	static constexpr uint32_t FRAMES_IN_FLIGHT{2};

	static constexpr ignis::ColorFormat ETNA_COLOR_FORMAT{
		ignis::ColorFormat::RGBA16};

	static constexpr ignis::DepthFormat ETNA_DEPTH_FORMAT{
		ignis::DepthFormat::D32_SFLOAT};

private:
	ignis::Device* m_device{nullptr};
	_Material* m_defaultMaterial{nullptr};
	BufferId m_primitiveVertices{IGNIS_INVALID_BUFFER_ID};
	BufferId m_primitiveIndices{IGNIS_INVALID_BUFFER_ID};

	uint32_t m_graphicsQueue{0};
	uint32_t m_uploadQueue{0};

	RenderTarget* m_currentRenderTarget{nullptr};

	struct FrameData {
		ignis::Semaphore* m_imageAvailable;
		ignis::Semaphore* m_renderFinished;
		ignis::Fence* m_inFlight;
		ignis::Command* m_cmd;
	} frames[FRAMES_IN_FLIGHT];

public:
	Engine(const Engine&) = delete;
	Engine(Engine&&) = delete;
	Engine& operator=(const Engine&) = delete;
	Engine& operator=(Engine&&) = delete;
};

}  // namespace etna
