#include "scene_graph.hpp"

using namespace etna;

_SceneNode::_SceneNode(Type type,
					   const std::string& name,
					   const Transform& transform)
	: m_name(name),
	  m_transform(transform),
	  m_type(type),
	  m_worldMatrix(transform.getWorldMatrix()) {}

SceneNode _SceneNode::add(SceneNode node) {
	SceneNode newNode = m_children.emplace_back(node);
	updateChildrenTransform(m_worldMatrix);
	return newNode;
}

MeshNode _SceneNode::createMeshNode(const scene::CreateMeshNodeInfo& info) {
	MeshNode node = scene::createMeshNode(info);
	add(node);
	return node;
}

CameraNode _SceneNode::createCameraNode(const scene::CreateCameraNodeInfo& info) {
	CameraNode node = scene::createCameraNode(info);
	add(node);
	return node;
}

CameraNode createCameraNode(const scene::CreateCameraNodeInfo&);

void _SceneNode::updateChildrenTransform(const Mat4& transform) {
	for (auto child : m_children) {
		child->m_worldMatrix = transform * child->m_transform.getWorldMatrix();
		child->updateChildrenTransform(child->m_worldMatrix);
	}
}

void _SceneNode::updateTransform(const Transform& transform) {
	m_transform = transform;
	m_worldMatrix = transform.getWorldMatrix();
	updateChildrenTransform(m_worldMatrix);
}

void _SceneNode::updatePosition(const Vec3& position) {
	m_transform.position = position;
	updateTransform(m_transform);
}

void _SceneNode::translate(const Vec3& translation) {
	m_transform.position += translation;
	updateTransform(m_transform);
}

void _SceneNode::rotate(float yaw, float pitch, float roll) {
	m_transform.yaw += yaw;
	m_transform.pitch += pitch;
	m_transform.roll += roll;
	updateTransform(m_transform);
}

Mat4 _CameraNode::getViewMatrix() const {
	return camera.getViewMatrix(getWorldMatrix());
}

Mat4 _CameraNode::getProjMatrix(float aspect) const {
	return camera.getProjMatrix(aspect);
}

SceneNode scene::createRoot(const std::string& name, const Transform& transform) {
	return std::make_shared<_SceneNode>(_SceneNode::Type::ROOT, name, transform);
}

MeshNode scene::createMeshNode(const CreateMeshNodeInfo& info) {
	MeshNode node = std::make_shared<_MeshNode>(_SceneNode::Type::MESH, info.name,
												info.transform);

	node->mesh = info.mesh;
	node->material = info.material;

	return node;
}

CameraNode scene::createCameraNode(const CreateCameraNodeInfo& info) {
	CameraNode node = std::make_shared<_CameraNode>(_SceneNode::Type::CAMERA,
													info.name, info.transform);

	node->camera = info.camera;

	return node;
}

SceneNode scene::loadFromPath(const std::string& path) {
	throw std::runtime_error("Not implemented");
}

#ifndef NDEBUG

#define GREEN(x) "\033[32m" x "\033[0m"
#define BLUE(x) "\033[34m" x "\033[0m"

void _SceneNode::print() const {
	if (m_type == Type::MESH) {
		std::cout << GREEN("Mesh") << ": " << m_name << std::endl;
	} else if (m_type == Type::CAMERA) {
		std::cout << BLUE("Camera") << ": " << m_name << std::endl;
	} else {
		std::cout << "Root: " << m_name << std::endl;
	}

	for (const auto& child : m_children) {
		std::cout << "  ";
		child->print();
	}
}

#endif
