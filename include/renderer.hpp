#pragma once

#include "ignis/ignis.hpp"
#include "mesh.hpp"

using namespace ignis;

class Window;

struct Camera {
	vec3 position;
	Rotation rot;
};

struct DirectionalLight {
	vec3 direction;
	Color color;
};

struct SceneData {
	Camera camera;
	Color ambientColor;
	DirectionalLight sun;
};

struct WorldTransform {
	float scale;
	Rotation rot;
	vec3 position;
};

struct PushConstants {
	mat4 worldTransform;
	VkDeviceAddress verticesAddress;
};

class Renderer {
public:
	Renderer(const Window&);
	~Renderer();

	void beginScene(SceneData);
	void endScene();

	auto getFrameTime() -> float;

	void draw(const Mesh&, WorldTransform);

private:
	Device* m_device;
	ColorImage* m_drawImage;
	DepthImage* m_depthImage;
	Command* m_cmd;
	Buffer* m_sceneDataUBO;
	Pipeline* m_pipeline;

	DrawAttachment m_drawAttachment;
	DepthAttachment m_depthAttachment;
};
