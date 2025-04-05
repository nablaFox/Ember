#pragma once

#include <unordered_map>
#include "scene_graph.hpp"
#include "renderer.hpp"

namespace etna {

struct SceneRenderInfo {
	Viewport viewport;
	Color ambient{WHITE};
};

struct SceneData {
	Color ambient;
	ignis::BufferId camera;
	ignis::BufferId* lights;
};

class Scene {
public:
	Scene();
	~Scene();

	SceneNode addNode(SceneNode, const Transform& = {});
	MeshNode addMesh(MeshNode, const Transform& = {});
	CameraNode addCamera(CameraNode, const Transform& = {});
	LightNode addLight(LightNode, const Transform& = {});

	MeshNode createMeshNode(const CreateMeshNodeInfo&);
	CameraNode createCameraNode(const CreateCameraNodeInfo&);
	LightNode createLightNode(const CreateLightNodeInfo&);

	MeshNode getMesh(const std::string& name) const;
	CameraNode getCamera(const std::string& name) const;
	LightNode getLight(const std::string& name) const;

	void removeMesh(const std::string&);
	void removeCamera(const std::string&);
	void removeLight(const std::string&);

	void render(Renderer&, const CameraNode&, const SceneRenderInfo& = {});

public:
	const std::vector<SceneNode>& getRoots() const { return m_roots; }

#ifndef NDEBUG
	void print() const;
#endif

private:
	std::vector<SceneNode> m_roots;

	std::unordered_map<std::string, MeshNode> m_meshes;
	std::unordered_map<std::string, CameraNode> m_camera;
	std::unordered_map<std::string, LightNode> m_lights;

	void addNodeHelper(SceneNode node, const Transform& transform);

	ignis::BufferId m_sceneBuffer{IGNIS_INVALID_BUFFER_ID};

public:
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;
};

}  // namespace etna
