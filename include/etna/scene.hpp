#pragma once

#include <unordered_map>
#include "transform.hpp"
#include "mesh.hpp"
#include "material.hpp"
#include "camera.hpp"

namespace etna {

class Scene;
struct MeshNode;

struct SceneNode {
	MeshNode& addMesh(std::string,
					  const MeshHandle,
					  Transform,
					  const MaterialHandle = nullptr);

	Camera& addCamera(std::string, Camera);

	Mat4 getWorldMatrix() const;

	void updateTransform(Transform);

private:
	Transform m_localTransform;
	Transform m_worldTransform;

	SceneNode* m_parent{nullptr};
	std::vector<SceneNode*> m_children;
	Scene* m_scene;
};

struct MeshNode : SceneNode {
	MaterialHandle material;
	MeshHandle mesh;
};

class Scene {
public:
	Scene();
	~Scene();

	const std::unordered_map<std::string, MeshNode>& getMeshes() const {
		return m_meshNodes;
	}

	const std::unordered_map<std::string, Camera>& getCameras() const {
		return m_cameraNodes;
	}

	Camera& getCamera(std::string);

	MeshNode& getMesh(std::string);

	SceneNode createRoot(std::string, Transform);

private:
	// TODO: in the future a graph
	std::unordered_map<std::string, MeshNode> m_meshNodes;
	std::unordered_map<std::string, Camera> m_cameraNodes;
};

}  // namespace etna
