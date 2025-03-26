#include "engine.hpp"
#include "scene.hpp"

using namespace etna;
using namespace ignis;

Scene::Scene() {
	for (uint32_t i{0}; i < Engine::ETNA_FRAMES_IN_FLIGHT; i++) {
		m_sceneDataBuffers[i] = Engine::getDevice().createSSBO(sizeof(SceneData));
	}
}

Scene::~Scene() {
	for (uint32_t i{0}; i < Engine::ETNA_FRAMES_IN_FLIGHT; i++) {
		Engine::getDevice().destroyBuffer(m_sceneDataBuffers[i]);
	}
}

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
