#include "engine.hpp"
#include "scene.hpp"

using namespace etna;
using namespace ignis;

Scene::Scene() {
	for (uint32_t i{0}; i < ETNA_FRAMES_IN_FLIGHT; i++) {
		m_sceneDataBuffers[i] = Engine::getDevice().createSSBO(sizeof(SceneData));
	}
}

Scene::~Scene() {
	for (uint32_t i{0}; i < ETNA_FRAMES_IN_FLIGHT; i++) {
		Engine::getDevice().destroyBuffer(m_sceneDataBuffers[i]);
	}
}

SceneNode Scene::addMesh(const Mesh mesh,
						 Transform transform,
						 const Material material) {
	m_nodes.push_back({.transform = &transform, .material = material, .mesh = mesh});

	return m_nodes.back();
}
