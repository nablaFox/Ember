#include <algorithm>
#include "etna/scene_graph.hpp"
#include "etna/engine.hpp"

using namespace etna;

_SceneNode::_SceneNode(Type type,
					   const std::string& name,
					   const Transform& transform)
	: m_transform(transform),
	  m_worldMatrix(transform.getWorldMatrix()),
	  m_name(name),
	  m_type(type) {}

SceneNode _SceneNode::add(SceneNode node) {
	if (node == nullptr)
		return nullptr;

	SceneNode newNode = m_children.emplace_back(node);
	newNode->m_parent = this;
	return newNode;
}

MeshNode _SceneNode::createMeshNode(const CreateMeshNodeInfo& info) {
	MeshNode node = scene::createMeshNode(info);
	add(node);
	return node;
}

CameraNode _SceneNode::createCameraNode(const CreateCameraNodeInfo& info) {
	CameraNode node = scene::createCameraNode(info);
	add(node);
	return node;
}

LightNode _SceneNode::createLightNode(const DirectionalLight::CreateInfo& info) {
	LightNode node = scene::createLightNode(info);
	add(node);
	return node;
}

void _SceneNode::remove() {
	if (m_parent == nullptr)
		return;

	for (const auto& child : m_parent->m_children) {
		if (child->m_name == m_name) {
			m_parent->m_children.erase(
				std::remove(m_parent->m_children.begin(), m_parent->m_children.end(),
							child),
				m_parent->m_children.end());

			break;
		}
	}
}

void _SceneNode::updateChildrenTransform(const Mat4& transform) {
	if (m_type == Type::CAMERA) {
		_CameraNode* cameraNode = static_cast<_CameraNode*>(this);
		cameraNode->camera->updateTransform(transform);
	}

	else if (m_type == Type::LIGHT) {
		_LightNode* lightNode = static_cast<_LightNode*>(this);
		auto light = lightNode->light;

		light->updateDirection(Transform::getRotMatrix3(transform) *
							   light->getDirection());
	}

	for (auto child : m_children) {
		child->m_worldMatrix = transform * child->m_transform.getWorldMatrix();
		child->updateChildrenTransform(child->m_worldMatrix);
	}
}

void _SceneNode::updateTransform(const Transform& transform) {
	m_transform = transform;
	m_worldMatrix = m_parent != nullptr
						? m_parent->m_worldMatrix * transform.getWorldMatrix()
						: transform.getWorldMatrix();

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

SceneNode scene::createRoot(const std::string& name, const Transform& transform) {
	return std::make_shared<_SceneNode>(_SceneNode::Type::ROOT, name, transform);
}

SceneNode scene::loadFromFile(const std::string& path) {
	throw std::runtime_error("Not implemented");
}

MeshNode scene::createMeshNode(const CreateMeshNodeInfo& info) {
	MeshNode node = std::make_shared<_MeshNode>(_SceneNode::Type::MESH, info.name,
												info.transform);

	node->mesh = info.mesh;
	node->material = info.material;
	node->instanceBuffer = info.instanceBuffer;
	node->instanceCount = info.instanceCount;

	return node;
}

_MeshNode::~_MeshNode() {
	_device.destroyBuffer(instanceBuffer);
}

CameraNode scene::createCameraNode(const CreateCameraNodeInfo& info) {
	CameraNode node = std::make_shared<_CameraNode>(_SceneNode::Type::CAMERA,
													info.name, info.transform);

	node->camera = std::shared_ptr<Camera>(new Camera(info.cameraInfo));
	node->camera->updateTransform(info.transform);

	return node;
}

LightNode scene::createLightNode(const DirectionalLight::CreateInfo& info) {
	LightNode node = std::make_shared<_LightNode>(_SceneNode::Type::LIGHT, info.name,
												  Transform{});

	node->light = std::make_shared<DirectionalLight>(info);

	return node;
}

SceneNode scene::find(const std::string& name, const SceneNode& root) {
	if (name.empty()) {
		return nullptr;
	}

	if (root->getName() == name) {
		return root;
	}

	for (const auto& child : root->getChildren()) {
		SceneNode node = find(name, child);

		if (node != nullptr)
			return node;
	}

	return nullptr;
}

std::vector<MeshNode> scene::getMeshes(const SceneNode& root) {
	std::vector<MeshNode> meshes;

	if (root->getType() == _SceneNode::Type::MESH) {
		meshes.push_back(std::static_pointer_cast<_MeshNode>(root));
	}

	for (const auto& child : root->getChildren()) {
		auto childMeshes = getMeshes(child);
		meshes.insert(meshes.end(), childMeshes.begin(), childMeshes.end());
	}

	return meshes;
}

std::vector<CameraNode> scene::getCameras(const SceneNode& root) {
	std::vector<CameraNode> cameras;

	if (root->getType() == _SceneNode::Type::CAMERA) {
		cameras.push_back(std::static_pointer_cast<_CameraNode>(root));
	}

	for (const auto& child : root->getChildren()) {
		auto childCameras = getCameras(child);
		cameras.insert(cameras.end(), childCameras.begin(), childCameras.end());
	}

	return cameras;
}

std::vector<LightNode> scene::getLights(const SceneNode& root) {
	std::vector<LightNode> lights;

	if (root->getType() == _SceneNode::Type::LIGHT) {
		lights.push_back(std::static_pointer_cast<_LightNode>(root));
	}

	for (const auto& child : root->getChildren()) {
		auto childLights = getLights(child);
		lights.insert(lights.end(), childLights.begin(), childLights.end());
	}

	return lights;
}

#ifndef NDEBUG

#define GREEN(x) "\033[32m" x "\033[0m"
#define BLUE(x) "\033[34m" x "\033[0m"
#define YELLOW(x) "\033[33m" x "\033[0m"

const char* _SceneNode::getTypeLabel() const {
	switch (m_type) {
		case _SceneNode::Type::MESH:
			return GREEN("Mesh");
		case _SceneNode::Type::CAMERA:
			return BLUE("Camera");
		case _SceneNode::Type::LIGHT:
			return YELLOW("Light");
		case _SceneNode::Type::ROOT:
			return "Root";
		default:
			return "Unknown";
	}
}

void _SceneNode::print() const {
	std::cout << getTypeLabel() << ": " << m_name << std::endl;

	for (const auto& child : m_children) {
		std::cout << "  ";
		child->print();
	}
}

#endif
