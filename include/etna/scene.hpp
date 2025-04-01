#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include "transform.hpp"
#include "camera.hpp"

namespace etna {

class Scene;
struct MeshNode;
class Material;
class Mesh;
struct CameraNode;

struct SceneNode {
	MeshNode& addMesh(std::string,
					  const std::shared_ptr<Mesh>,
					  Transform,
					  const std::shared_ptr<Material> = nullptr);

	CameraNode& addCamera(std::string, Camera, Transform, Viewport = {});

	const Transform& getTransform() const { return m_transform; }

	Mat4 getWorldMatrix() const { return m_worldTransform; }

	void updateTransform(Transform);

	void translate(Vec3);

	void rotate(float yaw, float pitch, float roll);

protected:
	friend class Scene;
	SceneNode(Scene* scene, std::string, Transform, SceneNode* parent = nullptr);

	Transform m_transform;
	Mat4 m_worldTransform;

	SceneNode* m_parent{nullptr};
	std::vector<SceneNode*> m_children;
	Scene* m_scene;

	void updateChildrenTransform(Mat4);
};

struct MeshNode : public SceneNode {
	using SceneNode::SceneNode;

	std::shared_ptr<Material> material;
	std::shared_ptr<Mesh> mesh;
};

struct CameraNode : public SceneNode {
	using SceneNode::SceneNode;

	Viewport viewport;
	Camera camera;	// PONDER: should this be a ptr?
};

class Scene {
public:
	Scene();
	~Scene();

	const std::unordered_map<std::string, MeshNode>& getMeshes() const {
		return m_meshNodes;
	}

	const std::unordered_map<std::string, CameraNode>& getCameras() const {
		return m_cameraNodes;
	}

	CameraNode* getCamera(std::string);

	MeshNode* getMesh(std::string);

	SceneNode createRoot(std::string, Transform);

private:
	friend struct SceneNode;

	std::unordered_map<std::string, MeshNode> m_meshNodes;
	std::unordered_map<std::string, CameraNode> m_cameraNodes;
};

}  // namespace etna
