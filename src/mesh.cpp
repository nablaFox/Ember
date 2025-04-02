#include "mesh.hpp"
#include "command.hpp"
#include "engine.hpp"

using namespace etna;
using namespace ignis;

Mesh::Mesh(const CreateInfo& info) {
	Command uploadCmd({.device = _device, .queue = engine::getUploadQueue()});

	m_vertexBuffer = _device.createSSBO(info.vertices.size() * sizeof(Vertex));

	m_indexBuffer = new Buffer(_device.createIndexBuffer32(info.indices.size()));

	uploadCmd.begin();

	uploadCmd.updateBuffer(m_vertexBuffer, info.vertices.data());

	uploadCmd.updateBuffer(*m_indexBuffer, info.indices.data());

	uploadCmd.end();

	SubmitCmdInfo uploadCmdInfo{.command = uploadCmd};

	_device.submitCommands({uploadCmdInfo}, nullptr);

	_device.waitIdle();
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
