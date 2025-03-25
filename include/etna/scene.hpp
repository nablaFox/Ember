#pragma once

#include "ignis/types.hpp"
#include "material.hpp"
#include "mesh.hpp"

namespace etna {

struct Transform;

struct RenderSceneInfo {};

struct SceneNode {
	Transform* transform;
	Material material;
	Mesh mesh;
};

class Scene {
public:
	Scene();
	~Scene();

	SceneNode addMesh(const Mesh, Transform, const Material = nullptr);

	const std::vector<SceneNode>& getNodes() const { return m_nodes; }

	auto getSceneDataUBO() const { return sceneDataUBO; }

private:
	// TODO: in the future a graph
	std::vector<SceneNode> m_nodes;
	BufferId sceneDataUBO;
};

}  // namespace etna
