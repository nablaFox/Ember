#pragma once

#include "ignis/types.hpp"
#include "transform.hpp"
#include "engine.hpp"

namespace etna {

struct SceneNode {
	Transform transform;
	MaterialHandle material;
	MeshHandle mesh;
};

struct DirectionalLight {
	Vec3 direction;
	float padding;
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

	// PONDER: maybe we should add names and fetching by name

	SceneNode& addNode(SceneNode);

	SceneNode& addMesh(const MeshHandle, Transform, const MaterialHandle = nullptr);

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
