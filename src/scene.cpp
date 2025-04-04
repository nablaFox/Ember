#include <algorithm>
#include "scene.hpp"

using namespace etna;
using namespace ignis;

Scene::Scene(const CreateInfo&) {}

Scene::~Scene() {}

void Scene::addNodeHelper(SceneNode node, const Transform& transform) {
	if (node == nullptr)
		return;

	if (node->getType() == _SceneNode::Type::MESH) {
		auto meshNode = std::static_pointer_cast<_MeshNode>(node);
		m_meshes[meshNode->getName()] = meshNode;
	}

	else if (node->getType() == _SceneNode::Type::CAMERA) {
		auto cameraNode = std::static_pointer_cast<_CameraNode>(node);
		m_camera[cameraNode->getName()] = cameraNode;
	}

	for (auto& child : node->getChildren()) {
		addNodeHelper(child, transform);
	}
}

SceneNode Scene::addNode(SceneNode node, const Transform& transform) {
	if (node == nullptr)
		return nullptr;

	node->translate(transform.position);
	node->rotate(transform.yaw, transform.pitch, transform.roll);

	m_roots.push_back(node);

	addNodeHelper(node, transform);

	return node;
}

MeshNode Scene::addMesh(MeshNode node, const Transform& transform) {
	return std::static_pointer_cast<_MeshNode>(addNode(node, transform));
}

CameraNode Scene::addCamera(CameraNode node, const Transform& transform) {
	return std::static_pointer_cast<_CameraNode>(addNode(node, transform));
}

MeshNode Scene::createMeshNode(const CreateMeshNodeInfo& info) {
	return addMesh(scene::createMeshNode(info));
}

CameraNode Scene::createCameraNode(const CreateCameraNodeInfo& info) {
	return addCamera(scene::createCameraNode(info));
}

MeshNode Scene::getMesh(const std::string& name) const {
	auto it = m_meshes.find(name);

	if (it != m_meshes.end()) {
		return it->second;
	}

	return nullptr;
}

CameraNode Scene::getCamera(const std::string& name) const {
	auto it = m_camera.find(name);

	if (it != m_camera.end()) {
		return it->second;
	}

	return nullptr;
}

void Scene::removeMesh(const std::string& name) {
	auto it = m_meshes.find(name);

	if (it != m_meshes.end()) {
		m_meshes.erase(it);
	}

	auto newEnd = std::remove_if(m_roots.begin(), m_roots.end(),
								 [&name](const SceneNode& node) {
									 return node && (node->getName() == name);
								 });

	m_roots.erase(newEnd, m_roots.end());
}

void Scene::removeCamera(const std::string& name) {
	auto it = m_camera.find(name);

	if (it != m_camera.end()) {
		m_camera.erase(it);
	}
}

void Scene::render(Renderer& renderer,
				   const CameraNode& cameraNode,
				   const Viewport& viewport) {
	Viewport vp{viewport};

	if (viewport.width == 0) {
		vp.x = 0;
		vp.width = (float)renderer.getRenderTarget().getExtent().width;
	}

	if (viewport.height == 0) {
		vp.y = 0;
		vp.height = (float)renderer.getRenderTarget().getExtent().height;
	}

	cameraNode->camera->updateAspect(vp.width / vp.height);

	for (const auto& [_, meshNode] : m_meshes) {
		if (meshNode->mesh == nullptr)
			continue;

		const MaterialHandle material = meshNode->material;
		const MeshHandle mesh = meshNode->mesh;
		const Mat4 worldMatrix = meshNode->getWorldMatrix();

		renderer.draw({
			.mesh = mesh,
			.material = material,
			.transform = worldMatrix,
			.viewport = vp,
			.ubo = cameraNode->camera->getDataBuffer(),
			.instanceBuffer = meshNode->instanceBuffer,
			.instanceCount = meshNode->instanceCount,
		});
	}
}

#ifndef NDEBUG

void Scene::print() const {
	for (const auto& node : m_roots) {
		node->print();
	}

	std::cout << std::endl;
}

#endif
