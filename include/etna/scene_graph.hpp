#include <memory>
#include "transform.hpp"
#include "mesh.hpp"
#include "material.hpp"
#include "camera.hpp"

namespace etna {

struct _MeshNode;
struct _CameraNode;
struct _SceneNode;

using SceneNode = std::shared_ptr<_SceneNode>;
using MeshNode = std::shared_ptr<_MeshNode>;
using CameraNode = std::shared_ptr<_CameraNode>;

struct CreateMeshNodeInfo {
	std::string name;
	MeshHandle mesh;
	Transform transform;
	MaterialHandle material{nullptr};
};

struct CreateCameraNodeInfo {
	std::string name;
	Transform transform;
	Camera::CreateInfo cameraInfo;
};

struct _SceneNode {
	enum class Type {
		MESH,
		CAMERA,
		ROOT,
	};

	_SceneNode(Type, const std::string&, const Transform&);

	SceneNode add(SceneNode);

	MeshNode createMeshNode(const CreateMeshNodeInfo&);
	CameraNode createCameraNode(const CreateCameraNodeInfo&);

	const Transform& getTransform() const { return m_transform; }

	Mat4 getWorldMatrix() const { return m_worldMatrix; }

	void updateTransform(const Transform&);

	void updatePosition(const Vec3&);

	void translate(const Vec3&);

	void rotate(float yaw, float pitch, float roll);

	const std::string& getName() const { return m_name; }

	Type getType() const { return m_type; }

	const std::vector<SceneNode>& getChildren() const { return m_children; }

#ifndef NDEBUG
	void print() const;
#endif

protected:
	Transform m_transform;
	Mat4 m_worldMatrix;
	std::string m_name;
	Type m_type;

	std::vector<SceneNode> m_children;

	void updateChildrenTransform(const Mat4&);
};

struct _MeshNode : public _SceneNode {
	using _SceneNode::_SceneNode;

	MaterialHandle material;
	MeshHandle mesh;
};

struct _CameraNode : public _SceneNode {
	using _SceneNode::_SceneNode;

	CameraHandle camera;

	Mat4 getViewMatrix() const;
	Mat4 getProjMatrix(float aspect) const;
};

namespace scene {

SceneNode createRoot(const std::string&, const Transform& = {});

MeshNode createMeshNode(const CreateMeshNodeInfo&);

CameraNode createCameraNode(const CreateCameraNodeInfo&);

SceneNode loadFromFile(const std::string& path);

CameraNode searchCamera(const SceneNode root, const std::string& name);

MeshNode searchMesh(const SceneNode root, const std::string& name);

}  // namespace scene

}  // namespace etna
