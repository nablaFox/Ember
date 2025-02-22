#pragma once

#include <span>
#include "ignis/buffer.hpp"
#include "ignis/device.hpp"
#include "ignis/fence.hpp"
#include "renderer.hpp"

struct Vertex {
	Vec3 position;
	float padding;
	Color color;
};

typedef uint32_t Index;

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

	ignis::Device* m_device;

	std::vector<Vertex> m_vertices;
	std::vector<Index> m_indices;

	ignis::Buffer* m_vertexBuffer;
	ignis::Buffer* m_indexBuffer;

	ignis::Fence* m_waitForUpload;
};
