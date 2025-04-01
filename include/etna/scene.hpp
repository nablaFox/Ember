#pragma once

#include "transform.hpp"
#include "mesh.hpp"
#include "material.hpp"

namespace etna {

struct SceneNode {
	Transform transform;
	MaterialHandle material;
	MeshHandle mesh;
};

class Scene {
public:
	Scene();
	~Scene();

	// PONDER: maybe we should add names and fetching by name

	SceneNode& addNode(SceneNode);

	SceneNode& addMesh(const MeshHandle, Transform, const MaterialHandle = nullptr);

	const std::vector<SceneNode>& getNodes() const { return m_nodes; }

private:
	// TODO: in the future a graph
	std::vector<SceneNode> m_nodes;
};

}  // namespace etna
