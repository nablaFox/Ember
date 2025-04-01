#include "model.hpp"

using namespace etna;

Model::Model(MeshHandle mesh, MaterialHandle mat, Transform trans)
	: m_root(std::make_shared<Node>()) {
	m_root->mesh = mesh;
	m_root->material = mat;
	m_root->transform = trans;
}

Model::~Model() {}

Model::Node& Model::Node::addSubMesh(std::string name,
									 MeshHandle mesh,
									 MaterialHandle material,
									 Transform transform) {
	Node node{
		.mesh = mesh,
		.material = material,
		.transform = transform,
		.name = name,
	};

	return *children.emplace_back(std::make_shared<Node>(node));
}
