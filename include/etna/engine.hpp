#pragma once

#include "mesh.hpp"
#include "material.hpp"
#include "ignis/device.hpp"
#include "configs.hpp"

namespace etna {

struct Camera;
struct RenderTarget;
class Scene;
struct SceneData;

class Engine {
public:
	Engine();

	~Engine();

	void beginFrame();

	void endFrame();

	void renderScene(const Scene&,
					 const RenderTarget&,
					 const Camera&,
					 VkViewport = {});

	Mesh createMesh(const _Mesh::CreateInfo&);

	Material createMaterial(const _Material::CreateInfo&);

	ignis::Command& getCommand() const {
		return *m_framesData[m_currentFrame].m_cmd;
	}

	static ignis::Device& getDevice();

private:
	_Material* m_defaultMaterial{nullptr};

	uint32_t m_graphicsQueue{0};
	uint32_t m_uploadQueue{0};

	struct FrameData {
		ignis::Fence* m_inFlight;
		ignis::Command* m_cmd;
	} m_framesData[ETNA_FRAMES_IN_FLIGHT];

	struct PushConstants {
		ignis::BufferId vertices;
		ignis::BufferId material;
		ignis::BufferId sceneData;
	} m_pushConstants;

	uint32_t m_currentFrame{0};

public:
	Engine(const Engine&) = delete;
	Engine(Engine&&) = delete;
	Engine& operator=(const Engine&) = delete;
	Engine& operator=(Engine&&) = delete;
};

}  // namespace etna
