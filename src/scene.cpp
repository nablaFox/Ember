#include "scene.hpp"
#include "mesh.hpp"
#include "material.hpp"

using namespace etna;
using namespace ignis;

Scene::Scene(const CreateInfo&) {}

Scene::~Scene() {}

static SceneNode addNodeHelper(SceneNode node,
							   const Transform& transform,
							   std::vector<MeshNode>& meshes) {
	if (node->getType() == _SceneNode::Type::MESH) {
		meshes.push_back(std::static_pointer_cast<_MeshNode>(node));
	}

	for (auto& child : node->getChildren()) {
		addNodeHelper(child, transform, meshes);
	}

	return node;
}

SceneNode Scene::addNode(SceneNode node, const Transform& transform) {
	if (node == nullptr)
		return nullptr;

	node->translate(transform.position);
	node->rotate(transform.yaw, transform.pitch, transform.roll);

#ifndef NDEBUG
	m_nodes.push_back(node);
#endif

	return addNodeHelper(node, transform, m_meshNodes);
}

MeshNode Scene::createMeshNode(const CreateMeshNodeInfo& info) {
	return addMesh(scene::createMeshNode(info));
}

CameraNode Scene::createCameraNode(const CreateCameraNodeInfo& info) {
	return addCamera(scene::createCameraNode(info));
}

MeshNode Scene::addMesh(MeshNode node, const Transform& transform) {
	return std::static_pointer_cast<_MeshNode>(addNode(node, transform));
}

CameraNode Scene::addCamera(CameraNode node, const Transform& transform) {
	return std::static_pointer_cast<_CameraNode>(addNode(node, transform));
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

	for (const auto& meshNode : m_meshNodes) {
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
		});
	}
}

CameraNode Scene::searchCamera(const std::string& name) {
	CameraNode camera;

	for (const auto& node : m_nodes) {
		camera = scene::searchCamera(node, name);

		if (camera != nullptr)
			break;
	}

	return camera;
}

MeshNode Scene::searchMesh(const std::string& name) {
	MeshNode mesh;

	for (const auto& node : m_nodes) {
		mesh = scene::searchMesh(node, name);

		if (mesh != nullptr)
			break;
	}

	return mesh;
}

#ifndef NDEBUG

void Scene::print() const {
	for (const auto& node : m_nodes) {
		node->print();
	}
}

#endif
