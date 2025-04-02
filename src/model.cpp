#include "model.hpp"

using namespace etna;

ModelRoot Model::createRoot(const std::string& name, const ModelNode& node) {
	return std::make_shared<Model>(Model(name, node));
}

ModelRoot Model::loadFromPath(const std::string& filePath) {
	throw std::runtime_error("Not implemented");
}

ModelRoot Model::add(const std::string& name, const ModelNode& node) {
	return children.emplace_back(createRoot(name, node));
}

ModelRoot Model::add(const std::string& name, ModelRoot model) {
	Model newModel = *model;

	newModel.name = name;

	return children.emplace_back(std::make_shared<Model>(newModel));
}
