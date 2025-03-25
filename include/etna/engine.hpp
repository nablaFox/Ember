#pragma once

#include "ignis/device.hpp"
#include "image.hpp"
#include "mesh.hpp"
#include "material.hpp"

namespace etna {

struct Camera;
struct Transform;
class RenderTarget;
class Scene;

class Engine {
public:
	Engine();

	~Engine();

	void setCamera(const Camera& camera);

	void setRenderTarget(RenderTarget*);

	Mesh createMesh(const _Mesh::CreateInfo&);

	Material createMaterial(const _Material::CreateInfo&);

	ignis::Command& getCommand() const {
		return *m_framesData[m_currentFrame].m_cmd;
	}

	static ignis::Device& getDevice();

public:
	static constexpr uint32_t FRAMES_IN_FLIGHT{2};

	static constexpr ignis::ColorFormat ETNA_COLOR_FORMAT{
		ignis::ColorFormat::RGBA16};

	static constexpr ignis::DepthFormat ETNA_DEPTH_FORMAT{
		ignis::DepthFormat::D32_SFLOAT};

private:
	_Material* m_defaultMaterial{nullptr};

	uint32_t m_graphicsQueue{0};
	uint32_t m_uploadQueue{0};

	struct FrameData {
		ignis::Fence* m_inFlight;
		ignis::Command* m_cmd;
	} m_framesData[FRAMES_IN_FLIGHT];

	uint32_t m_currentFrame{0};

public:
	Engine(const Engine&) = delete;
	Engine(Engine&&) = delete;
	Engine& operator=(const Engine&) = delete;
	Engine& operator=(Engine&&) = delete;
};

}  // namespace etna
