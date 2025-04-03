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

	const std::unordered_map<std::string, MeshNode>& getMeshes() const {
		return m_meshNodes;
	}

	const std::unordered_map<std::string, CameraNode>& getCameras() const {
		return m_cameraNodes;
	}

	void removeMesh(std::string);

	void removeCamera(std::string);

	CameraNode* getCamera(std::string);

	MeshNode* getMesh(std::string);

	SceneNode addNode(SceneNode, const Transform& = {}, const std::string& = "");
	MeshNode addMesh(MeshNode, const Transform& = {}, const std::string& = "");
	CameraNode addCamera(CameraNode, const Transform& = {}, const std::string& = "");

	MeshNode createMeshNode(const scene::CreateMeshNodeInfo&);
	CameraNode createCameraNode(const scene::CreateCameraNodeInfo&);

	void render(Renderer&, const CameraNode&, const Viewport& = {});

#ifndef NDEBUG
	void print() const;
#endif

private:
	SceneNode addNodeHelper(SceneNode, const Transform&, const std::string&);

#ifndef NDEBUG
	std::vector<SceneNode> m_nodes;
#endif

	std::unordered_map<std::string, MeshNode> m_meshNodes;
	std::unordered_map<std::string, CameraNode> m_cameraNodes;
};

}  // namespace etna
