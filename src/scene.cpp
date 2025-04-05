#include <algorithm>
#include "scene.hpp"
#include "engine.hpp"

using namespace etna;
using namespace ignis;

Scene::Scene() {
	m_sceneBuffer = _device.createUBO(sizeof(SceneData));
	m_lightsBuffer = _device.createUBO(sizeof(ignis::BufferId) * Scene::MAX_LIGHTS);
}

Scene::~Scene() {
	_device.destroyBuffer(m_sceneBuffer);
	_device.destroyBuffer(m_lightsBuffer);
}

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

LightHandle Scene::addLight(const DirectionalLight::CreateInfo& info) {
	LightHandle light = std::make_shared<DirectionalLight>(info);

	m_lights[info.name] = light;

	std::vector<ignis::BufferId> lights;

	for (const auto& [_, light] : m_lights) {
		if (light->getIntensity() > 0) {
			lights.push_back(light->getDataBuffer());
		}
	}

	if (lights.size() > Scene::MAX_LIGHTS) {
		throw std::runtime_error("Too many lights in the scene");
	}

	if (lights.size() > 0) {
		_device.updateBuffer(m_lightsBuffer, lights.data());
	}

	return light;
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

LightHandle Scene::getLight(const std::string& name) const {
	auto it = m_lights.find(name);

	if (it != m_lights.end()) {
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

void Scene::removeLight(const std::string& name) {
	auto it = m_lights.find(name);

	if (it != m_lights.end()) {
		m_lights.erase(it);
	}
}

void Scene::render(Renderer& renderer,
				   const CameraNode& cameraNode,
				   const SceneRenderInfo& info) {
	Viewport vp{info.viewport};

	if (vp.width == 0) {
		vp.x = 0;
		vp.width = (float)renderer.getRenderTarget().getExtent().width;
	}

	if (vp.height == 0) {
		vp.y = 0;
		vp.height = (float)renderer.getRenderTarget().getExtent().height;
	}

	const SceneData sceneData{
		.ambient = info.ambient,
		.lights = m_lightsBuffer,
		.lightCount = static_cast<uint32_t>(m_lights.size()),
	};

	_device.updateBuffer(m_sceneBuffer, &sceneData);

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
			.buff1 = m_sceneBuffer,
			.buff2 = cameraNode->camera->getDataBuffer(),
			.instanceBuffer = meshNode->instanceBuffer,
			.instanceCount = meshNode->instanceCount,
		});
	}
}

void Scene::print() const {
#ifndef NDEBUG

	for (const auto& node : m_roots) {
		node->print();
	}

	for (const auto& [name, light] : m_lights) {
		std::cout << "\033[33m" << name << "\033[0m" << ": ";
		std::cout << light->getIntensity() << " ";

		const Vec3& dir = light->getDirection();

		std::cout << "[" << dir[0] << " " << dir[1] << " " << dir[2] << "]"
				  << std::endl;
	}

	std::cout << std::endl;
#endif
}
