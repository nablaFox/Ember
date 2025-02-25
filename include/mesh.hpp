#pragma once

#include <span>
#include "types.hpp"
#include "ignis/ignis.hpp"

using namespace ignis;

class Mesh {
public:
	Mesh(uint32_t verticesCount, uint32_t indicesCount);

	void update(std::span<Vertex>, std::span<Index>);

	void upload();

	void setColor(Color color) {
		for (auto& vertex : m_vertices) {
			vertex.color = color;
		}

		m_dirty = true;
	}

	const auto& getVertices() const { return m_vertices; }
	const auto& getIndices() const { return m_indices; }

	auto getIndexBuffer() -> ignis::Buffer&;

	auto getVertexBufferAddress() -> VkDeviceAddress;

protected:
	bool m_dirty = true;

	std::vector<Vertex> m_vertices;
	std::vector<Index> m_indices;

	Buffer* m_vertexBuffer{nullptr};
	Buffer* m_indexBuffer{nullptr};

	Fence* m_waitForUpload{nullptr};
};
