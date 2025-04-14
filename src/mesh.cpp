#include "etna/mesh.hpp"
#include "ignis/command.hpp"
#include "etna/engine.hpp"

using namespace etna;
using namespace ignis;

Mesh::Mesh(const CreateInfo& info)
	: m_vertexBuffer(_device.createSSBO(info.vertices.size() * sizeof(Vertex))) {
	m_indexBuffer = new Buffer(_device.createIndexBuffer32(info.indices.size()));

	etna::engine::immediateSubmit([&](ignis::Command& cmd) {
		cmd.updateBuffer(m_vertexBuffer, info.vertices.data());
		cmd.updateBuffer(*m_indexBuffer, info.indices.data());
	});
}

Mesh::~Mesh() {
	_device.destroyBuffer(m_vertexBuffer);
	delete m_indexBuffer;
}

MeshHandle Mesh::create(const CreateInfo& info) {
	return std::shared_ptr<Mesh>(new Mesh(info));
}

uint32_t Mesh::indexCount() const {
	return m_indexBuffer->getSize() / sizeof(Index);
}

uint32_t Mesh::vertexCount() const {
	return _device.getBuffer(m_vertexBuffer).getSize() / sizeof(Vertex);
}
