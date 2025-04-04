#pragma once

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

	void removeMesh(std::string);

	void removeCamera(std::string);

	SceneNode addNode(SceneNode, const Transform& = {});
	MeshNode addMesh(MeshNode, const Transform& = {});
	CameraNode addCamera(CameraNode, const Transform& = {});

	MeshNode createMeshNode(const CreateMeshNodeInfo&);
	CameraNode createCameraNode(const CreateCameraNodeInfo&);

	void render(Renderer&, const CameraNode&, const Viewport& = {});

public:
	CameraNode searchCamera(const std::string&);

	MeshNode searchMesh(const std::string&);

	const std::vector<SceneNode>& getNodes() const { return m_nodes; }

	const std::vector<MeshNode>& getMeshes() const { return m_meshNodes; }

#ifndef NDEBUG
	void print() const;
#endif

public:
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;

private:
	std::vector<SceneNode> m_nodes;
	std::vector<MeshNode> m_meshNodes;
};

}  // namespace etna
