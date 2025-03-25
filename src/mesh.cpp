#include "mesh.hpp"
#include "command.hpp"
#include "engine.hpp"

using namespace etna;
using namespace ignis;

_Mesh::_Mesh(const CreateInfo& info) {
	auto& device = Engine::getDevice();

	// PONDER: maybe we can query here the appropriate queue for the upload instead
	// of having it global
	Command uploadCmd({.device = device, .queue = Engine::getUploadQueue()});

	m_vertexBuffer = device.createSSBO(info.vertices.size() * sizeof(Vertex));

	m_indexBuffer = new Buffer(device.createIndexBuffer32(info.indices.size()));

	uploadCmd.begin();

	uploadCmd.updateBuffer(m_vertexBuffer, info.vertices.data());

	uploadCmd.updateBuffer(*m_indexBuffer, info.indices.data());

	uploadCmd.end();

	SubmitCmdInfo uploadCmdInfo{.command = uploadCmd};

	device.submitCommands({uploadCmdInfo}, nullptr);

	device.waitIdle();
}

_Mesh::~_Mesh() {
	auto& device = Engine::getDevice();

	device.destroyBuffer(m_vertexBuffer);
	delete m_indexBuffer;
}

uint32_t _Mesh::indexCount() const {
	return m_indexBuffer->getSize() / sizeof(Index);
}

uint32_t _Mesh::vertexCount() const {
	return Engine::getDevice().getBuffer(m_vertexBuffer).getSize() / sizeof(Vertex);
}
