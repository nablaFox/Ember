#include "scene.hpp"
#include "mesh.hpp"
#include "material.hpp"
#include "model.hpp"

using namespace etna;
using namespace ignis;

SceneNode::SceneNode(Scene* scene,
					 std::string name,
					 Transform transform,
					 SceneNode* parent)
	: m_scene(scene),
	  m_transform(transform),
	  m_parent(parent),
	  m_worldMatrix(transform.getWorldMatrix()) {}

MeshNode& SceneNode::addMesh(std::string name,
							 const MeshHandle mesh,
							 Transform transform,
							 const MaterialHandle material) {
	MeshNode node{m_scene, name, transform, this};

	node.mesh = mesh;
	node.material = material;

	auto result = m_scene->m_meshNodes.emplace(name, std::move(node));

	MeshNode& res = result.first->second;

	m_children.push_back(&res);

	updateChildrenTransform(m_worldMatrix);

	return res;
}

CameraNode& SceneNode::addCamera(std::string name,
								 Camera camera,
								 Transform transform,
								 Viewport viewport) {
	CameraNode node{m_scene, name, transform, this};

	node.camera = camera;
	node.viewport = viewport;

	auto result = m_scene->m_cameraNodes.emplace(name, std::move(node));

	CameraNode& res = result.first->second;

	m_children.push_back(&res);

	updateChildrenTransform(m_worldMatrix);

	return res;
}

static MeshNode& addModelChildren(const ModelRoot root,
								  SceneNode& sceneNode,
								  std::string name) {
	MeshNode& rootNode = sceneNode.addMesh(
		name, root->node.mesh, root->node.transform, root->node.material);

	for (auto child : root->children) {
		addModelChildren(child, rootNode, name + "/" + child->name);
	}

	return rootNode;
}

MeshNode& SceneNode::addModel(std::string name,
							  const ModelRoot model,
							  Transform transform) {
	assert(model != nullptr && "Model is null");

	MeshNode& toReturn = addModelChildren(model, *this, name + "-" + model->name);

	toReturn.translate(transform.position);
	toReturn.rotate(transform.yaw, transform.pitch, transform.roll);

	return toReturn;
}

void SceneNode::updateChildrenTransform(Mat4 transform) {
	for (auto child : m_children) {
		child->m_worldMatrix = transform * child->m_transform.getWorldMatrix();
		child->updateChildrenTransform(child->m_worldMatrix);
	}
}

void SceneNode::updateTransform(Transform transform) {
	m_transform = transform;
	m_worldMatrix = transform.getWorldMatrix();
	updateChildrenTransform(m_worldMatrix);
}

void SceneNode::updatePosition(Vec3 position) {
	m_transform.position = position;
	updateTransform(m_transform);
}

void SceneNode::translate(Vec3 translation) {
	m_transform.position += translation;
	updateTransform(m_transform);
}

void SceneNode::rotate(float yaw, float pitch, float roll) {
	m_transform.yaw += yaw;
	m_transform.pitch += pitch;
	m_transform.roll += roll;
	updateTransform(m_transform);
}

Scene::Scene() {}

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

SceneNode Scene::createRoot(std::string name, Transform transform) {
	return SceneNode(this, name, transform);
}
