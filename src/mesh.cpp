#include "mesh.hpp"
#include "engine.hpp"

using namespace etna;

_Mesh::~_Mesh() {
	auto& device = Engine::getDevice();

	device.destroyBuffer(vertexBuffer);
	device.destroyBuffer(indexBuffer);
}
