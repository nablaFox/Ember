#pragma once

#include <unordered_map>
#include "scene_graph.hpp"
#include "renderer.hpp"

namespace etna {

class Scene {
public:
	struct CreateInfo {
		uint32_t maxLights{0};
	};

	Scene(const CreateInfo&);
	~Scene();

	SceneNode addNode(SceneNode, const Transform& = {});
	MeshNode addMesh(MeshNode, const Transform& = {});
	CameraNode addCamera(CameraNode, const Transform& = {});

	MeshNode createMeshNode(const CreateMeshNodeInfo&);
	CameraNode createCameraNode(const CreateCameraNodeInfo&);

	MeshNode getMesh(const std::string& name) const;
	CameraNode getCamera(const std::string& name) const;

	void removeMesh(const std::string&);
	void removeCamera(const std::string&);

	void render(Renderer&, const CameraNode&, const Viewport& = {});

public:
	const std::vector<SceneNode>& getRoots() const { return m_roots; }

#ifndef NDEBUG
	void print() const;
#endif

private:
	std::vector<SceneNode> m_roots;

	std::unordered_map<std::string, MeshNode> m_meshes;
	std::unordered_map<std::string, CameraNode> m_camera;

	void addNodeHelper(SceneNode node, const Transform& transform);

public:
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;
};

}  // namespace etna
