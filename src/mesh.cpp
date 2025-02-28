#include "mesh.hpp"
#include "device.hpp"

using namespace ignis;
using namespace ember;

Mesh::Mesh(uint32_t verticesSize, uint32_t indicesSize) {
	auto device = EmberDevice::getDevice();

	m_vertices.resize(verticesSize);
	m_indices.resize(indicesSize);

	m_indexBuffer = Buffer::createIndexBuffer32(device, indicesSize);
	m_vertexBuffer =
		Buffer::createVertexBuffer(device, verticesSize * sizeof(Vertex));

	m_waitForUpload = new Fence(*device);
}

void Mesh::updateVertices(std::span<Vertex> newVertices) {
	assert(newVertices.size() == m_vertices.size());

	m_vertices.assign(newVertices.begin(), newVertices.end());
	m_dirty = true;
}

void Mesh::updateIndices(std::span<Index> newIndices) {
	assert(newIndices.size() == m_indices.size());

	m_indices.assign(newIndices.begin(), newIndices.end());
	m_dirty = true;
}

void Mesh::upload() {
	if (!m_dirty) {
		return;
	}

	auto device = EmberDevice::getDevice();

	m_waitForUpload->reset();

	Command cmd(*device, EmberDevice::getUploadQueue());

	cmd.begin();

	cmd.updateBuffer(*m_vertexBuffer, m_vertices.data());
	cmd.updateBuffer(*m_indexBuffer, m_indices.data());

	cmd.end();

	device->submitCommands({{&cmd}}, *m_waitForUpload);

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
