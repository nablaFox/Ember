#pragma once

#include <unordered_map>
#include "scene_graph.hpp"
#include "renderer.hpp"

namespace etna {

struct SceneRenderInfo {
	Viewport viewport;
	Color ambient{WHITE};
};

class Scene {
public:
	// Note: if we use a dynamic amount of them we need an SSBO
	static constexpr uint32_t MAX_LIGHTS = 16;

	Scene();
	~Scene();

	SceneNode addNode(SceneNode, const Transform& = {});
	MeshNode addMesh(MeshNode, const Transform& = {});
	CameraNode addCamera(CameraNode, const Transform& = {});

	MeshNode createMeshNode(const CreateMeshNodeInfo&);
	CameraNode createCameraNode(const CreateCameraNodeInfo&);
	LightNode createLightNode(const DirectionalLight::CreateInfo&);

	MeshNode getMesh(const std::string& name) const;
	CameraNode getCamera(const std::string& name) const;
	LightNode getLight(const std::string& name) const;

	void removeMesh(const std::string&);
	void removeCamera(const std::string&);
	void removeLight(const std::string&);

	void render(Renderer&, const CameraNode&, const SceneRenderInfo& = {});

public:
	const std::vector<SceneNode>& getRoots() const { return m_roots; }

	void print() const;

private:
	std::vector<SceneNode> m_roots;

	std::unordered_map<std::string, MeshNode> m_meshes;
	std::unordered_map<std::string, CameraNode> m_camera;
	std::unordered_map<std::string, LightNode> m_lights;

	void addNodeHelper(SceneNode node, const Transform& transform);
	void updateLights();

	ignis::BufferId m_sceneBuffer{IGNIS_INVALID_BUFFER_ID};
	ignis::BufferId m_lightsBuffer{IGNIS_INVALID_BUFFER_ID};

	struct SceneData {
		Color ambient;
		ignis::BufferId lights;
		uint32_t lightCount;
	};

public:
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;
};

}  // namespace etna
