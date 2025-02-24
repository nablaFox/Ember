#pragma once

#include <span>
#include "ignis/buffer.hpp"
#include "ignis/device.hpp"
#include "ignis/fence.hpp"
#include "types.hpp"

using namespace ignis;

class Mesh {
public:
	Mesh(uint32_t verticesSize, uint32_t indicesSize);

	void update(std::span<Vertex>, std::span<Index>);

	void upload();

	const auto& getVertices() const { return m_vertices; }
	const auto& getIndices() const { return m_indices; }

	auto getIndexBuffer() -> ignis::Buffer&;

	auto getVertexBufferAddress() -> VkDeviceAddress;

protected:
	bool m_dirty = true;

	std::vector<Vertex> m_vertices;
	std::vector<Index> m_indices;

	Buffer* m_vertexBuffer;
	Buffer* m_indexBuffer;

	Fence* m_waitForUpload;
};
