#include "mesh.hpp"
#include "renderer.hpp"

Mesh::Mesh(uint32_t verticesSize, uint32_t indicesSize) {
	m_vertices.resize(verticesSize);
	m_indices.resize(indicesSize);

	m_device = &Renderer::getInstance().getDevice();

	m_indexBuffer = Buffer::createIndexBuffer32(m_device, indicesSize);
	m_vertexBuffer = Buffer::createVertexBuffer<Vertex>(m_device, verticesSize);

	m_waitForUpload = new Fence(*m_device);
}

void Mesh::update(std::span<Vertex> newVertices, std::span<Index> newIndices) {
	m_vertices.assign(newVertices.begin(), newVertices.end());
	m_dirty = true;
}

void Mesh::upload() {
	if (!m_dirty) {
		return;
	}

	m_waitForUpload->reset();

	Command cmd(*m_device);

	cmd.begin();

	cmd.updateBuffer(*m_vertexBuffer, m_vertices.data());
	cmd.updateBuffer(*m_indexBuffer, m_indices.data());

	cmd.end();

	m_device->submitCommands({{&cmd}}, *m_waitForUpload);

	m_waitForUpload->wait();

	m_dirty = false;
}

auto Mesh::getIndexBuffer() -> ignis::Buffer& {
	if (m_dirty) {
		upload();
	}

	return *m_indexBuffer;
}

auto Mesh::getVertexBufferAddress() -> VkDeviceAddress {
	if (m_dirty) {
		upload();
	}

	return m_vertexBuffer->getDeviceAddress();
}
