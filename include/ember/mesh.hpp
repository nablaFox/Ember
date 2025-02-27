#pragma once

#include <span>
#include "types.hpp"
#include "ignis/ignis.hpp"

class Mesh {
public:
	Mesh(uint32_t verticesCount, uint32_t indicesCount);

	// Note: meshes are static in vertices/indices size
	// will be copied only the size of already allocated member vector
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

	ignis::Buffer* m_vertexBuffer{nullptr};
	ignis::Buffer* m_indexBuffer{nullptr};

	ignis::Fence* m_waitForUpload{nullptr};
};
