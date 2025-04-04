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

namespace scene {

SceneNode createRoot(const std::string&, const Transform& = {});

struct CreateMeshNodeInfo {
	std::string name;
	MeshHandle mesh;
	Transform transform;
	MaterialHandle material{nullptr};
};

MeshNode createMeshNode(const CreateMeshNodeInfo&);

struct CreateCameraNodeInfo {
	std::string name;
	Transform transform;
	float fov{60.f};
	float near{0.1f};
	float far{100.f};
	float aspect{1.f};
};

CameraNode createCameraNode(const CreateCameraNodeInfo&);

SceneNode loadFromPath(const std::string&);

}  // namespace scene

struct _SceneNode {
	enum class Type {
		MESH,
		CAMERA,
		ROOT,
	};

	_SceneNode(Type, const std::string&, const Transform&);

	SceneNode add(SceneNode);

	MeshNode createMeshNode(const scene::CreateMeshNodeInfo&);
	CameraNode createCameraNode(const scene::CreateCameraNodeInfo&);

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

}  // namespace etna
