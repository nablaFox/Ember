#pragma once

#include "material.hpp"
#include "mesh.hpp"
#include "transform.hpp"

namespace etna {

// PONDER: models are not only made of meshes with materials
// and scene.addModel should in principle return a SceneNode not just a MeshNode

struct ModelNode {
	MeshHandle mesh{nullptr};
	MaterialHandle material{nullptr};
	Transform transform{};
};

struct Model {
	ModelNode node;
	std::vector<std::shared_ptr<Model>> children;
	std::string name;

	std::shared_ptr<Model> add(const std::string&, const ModelNode& = {});

	std::shared_ptr<Model> add(const std::string&, std::shared_ptr<Model>);

	static std::shared_ptr<Model> loadFromPath(const std::string&);

	static std::shared_ptr<Model> createRoot(const std::string&,
											 const ModelNode& = {});

private:
	Model(const std::string& name, const ModelNode& node)
		: name(name), node(node) {};
};

using ModelRoot = std::shared_ptr<Model>;

}  // namespace etna
