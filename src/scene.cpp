#include "scene.hpp"

using namespace etna;
using namespace ignis;

Scene::Scene() {}

Scene::~Scene() {}

SceneNode& Scene::addMesh(const MeshHandle mesh,
						  Transform transform,
						  const MaterialHandle material) {
	m_nodes.push_back({.transform = transform, .material = material, .mesh = mesh});

	return m_nodes.back();
}

SceneNode& Scene::addNode(SceneNode node) {
	m_nodes.push_back(node);

	return m_nodes.back();
}
