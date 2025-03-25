#pragma once

#include "ignis/types.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "transform.hpp"
#include "engine.hpp"

namespace etna {

struct SceneNode {
	Transform* transform;
	Material material;
	Mesh mesh;
};

struct DirectionalLight {
	Vec3 direction;
	Color color;
};

struct SceneData {
	Mat4 viewproj;
	Color ambientColor;
	DirectionalLight sun;
};

class Scene {
public:
	Scene();
	~Scene();

	SceneNode addMesh(const Mesh, Transform, const Material = nullptr);

	const std::vector<SceneNode>& getNodes() const { return m_nodes; }

	auto getSceneDataBuff(uint32_t currFrame) const {
		return m_sceneDataBuffers[currFrame];
	}

private:
	// TODO: in the future a graph
	std::vector<SceneNode> m_nodes;

	ignis::BufferId m_sceneDataBuffers[Engine::ETNA_FRAMES_IN_FLIGHT];
};

}  // namespace etna
