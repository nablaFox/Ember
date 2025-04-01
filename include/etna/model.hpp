#pragma once

#include "material.hpp"
#include "mesh.hpp"
#include "transform.hpp"

namespace etna {

class Model {
public:
	struct Node {
		MeshHandle mesh{nullptr};
		MaterialHandle material{nullptr};
		std::vector<std::shared_ptr<Node>> children;
		Transform transform;
		std::string name;

		Node& addSubMesh(std::string, MeshHandle, MaterialHandle, Transform = {});
	};

	Model(MeshHandle, MaterialHandle, Transform = {});

	~Model();

	auto& getRoot() const { return *m_root; };

	// TODO: implement
	static Model loadFromPath(const std::string&);

private:
	std::shared_ptr<Node> m_root;
};

}  // namespace etna
