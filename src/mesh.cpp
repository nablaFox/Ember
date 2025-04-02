#include "mesh.hpp"
#include "command.hpp"
#include "engine.hpp"

using namespace etna;
using namespace ignis;

Mesh::Mesh(const CreateInfo& info) {
	auto& device = engine::getDevice();

	// PONDER: maybe we can query here the appropriate queue for the upload instead
	// of having it global
	Command uploadCmd({.device = device, .queue = engine::getUploadQueue()});

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

Mesh::~Mesh() {
	auto& device = engine::getDevice();

	device.destroyBuffer(m_vertexBuffer);
	delete m_indexBuffer;
}

MeshHandle Mesh::create(const CreateInfo& info) {
	return std::shared_ptr<Mesh>(new Mesh(info));
}

uint32_t Mesh::indexCount() const {
	return m_indexBuffer->getSize() / sizeof(Index);
}

uint32_t Mesh::vertexCount() const {
	return engine::getDevice().getBuffer(m_vertexBuffer).getSize() / sizeof(Vertex);
}
