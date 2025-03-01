#pragma once

#include <span>
#include <vector>
#include "ignis/fence.hpp"
#include "ignis/buffer.hpp"
#include "color.hpp"
#include "math.hpp"

namespace ember {

struct Vertex {
	Vec3 position;
	float padding;
	Color color;
	Vec2 uv;
	float padding2[2];
};

typedef uint32_t Index;

class Mesh {
public:
	Mesh(uint32_t verticesCount, uint32_t indicesCount);

	// Note: meshes are static in vertices/indices size
	// will be copied only the size of already allocated member vector
	void updateVertices(std::span<Vertex> vertices);
	void updateIndices(std::span<Index> indices);

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

}  // namespace ember
