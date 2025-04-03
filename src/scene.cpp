#include "scene.hpp"
#include "mesh.hpp"
#include "material.hpp"
#include "engine.hpp"

using namespace etna;
using namespace ignis;

Scene::Scene(const CreateInfo&) {}

Scene::~Scene() {}

CameraNode* Scene::getCamera(std::string name) {
	auto it = m_cameraNodes.find(name);

	if (it == m_cameraNodes.end())
		return nullptr;

	return &it->second;
}

MeshNode* Scene::getMesh(std::string name) {
	auto it = m_meshNodes.find(name);

	if (it == m_meshNodes.end())
		return nullptr;

	return &it->second;
}

SceneNode Scene::addNodeHelper(SceneNode node,
							   const Transform& transform,
							   const std::string& newName) {
	const std::string name = newName.empty() ? node->getName() : newName;

	std::string nameToUse = name;

	if (node->getType() == _SceneNode::Type::MESH) {
		if (m_meshNodes.find(name) != m_meshNodes.end()) {
			nameToUse = name + "_" + std::to_string(m_meshNodes.size());
		}

		m_meshNodes[nameToUse] = std::static_pointer_cast<_MeshNode>(node);
	}

	else if (node->getType() == _SceneNode::Type::CAMERA) {
		if (m_cameraNodes.find(name) != m_cameraNodes.end()) {
			nameToUse = name + "_" + std::to_string(m_cameraNodes.size());
		}

		m_cameraNodes[nameToUse] = std::static_pointer_cast<_CameraNode>(node);
	}

	for (auto& child : node->getChildren()) {
		addNodeHelper(child, transform, newName);
	}

	return node;
}

SceneNode Scene::addNode(SceneNode node,
						 const Transform& transform,
						 const std::string& newName) {
	if (node == nullptr)
		return nullptr;

	node->translate(transform.position);
	node->rotate(transform.yaw, transform.pitch, transform.roll);

#ifndef NDEBUG
	m_nodes.push_back(node);
#endif

	return addNodeHelper(node, transform, newName);
}

MeshNode Scene::createMeshNode(const scene::CreateMeshNodeInfo& info) {
	return addMesh(scene::createMeshNode(info));
}

CameraNode Scene::createCameraNode(const scene::CreateCameraNodeInfo& info) {
	return addCamera(scene::createCameraNode(info));
}

MeshNode Scene::addMesh(MeshNode node,
						const Transform& transform,
						const std::string& newName) {
	return std::static_pointer_cast<_MeshNode>(addNode(node, transform, newName));
}

CameraNode Scene::addCamera(CameraNode node,
							const Transform& transform,
							const std::string& newName) {
	return std::static_pointer_cast<_CameraNode>(addNode(node, transform, newName));
}

struct CameraData {
	Mat4 viewproj;
	Mat4 view;
	Mat4 proj;
};

void Scene::render(Renderer& renderer,
				   const CameraNode& camera,
				   const Viewport& viewport) {
	Viewport vp = viewport;

	if (viewport.width == 0) {
		vp.x = 0;
		vp.width = (float)renderer.getRenderTarget().getExtent().width;
	}

	if (viewport.height == 0) {
		vp.y = 0;
		vp.height = (float)renderer.getRenderTarget().getExtent().height;
	}

	const Mat4 view = camera->getViewMatrix();

	const Mat4 proj = camera->getProjMatrix(vp.width / vp.height);

	const CameraData cameraData{
		.viewproj = proj * view,
		.view = view,
		.proj = proj,
	};

	ignis::BufferId cameraDataBuff =
		_device.createUBO(sizeof(CameraData), &cameraData);

	renderer.queueForDeletion(cameraDataBuff);

	for (const auto& [_, meshNode] : m_meshNodes) {
		if (meshNode->mesh == nullptr)
			continue;

		const MaterialHandle material = meshNode->material;
		const MeshHandle mesh = meshNode->mesh;
		const Mat4 worldMatrix = meshNode->getWorldMatrix();

		renderer.draw(mesh, material, worldMatrix,
					  {.viewport = vp, .ubo = cameraDataBuff});
	}
}

#ifndef NDEBUG

void Scene::print() const {
	for (const auto& node : m_nodes) {
		node->print();
	}
}

#endif
