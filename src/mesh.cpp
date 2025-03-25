#include "mesh.hpp"
#include "engine.hpp"

using namespace etna;

_Mesh::_Mesh(const CreateInfo& info) {
	auto& device = Engine::getDevice();

	m_vertexBuffer = device.createSSBO(info.vertices.size() * sizeof(Vertex),
									   info.vertices.data());

	m_indexBuffer =
		device.createIndexBuffer32(info.indices.size(), info.indices.data());

	m_indexCount = info.indices.size();
	m_vertexCount = info.vertices.size();
}

_Mesh::~_Mesh() {
	auto& device = Engine::getDevice();

	device.destroyBuffer(m_vertexBuffer);
	device.destroyBuffer(m_indexBuffer);
}
