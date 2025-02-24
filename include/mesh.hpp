#pragma once

#include <span>
#include "ignis/buffer.hpp"
#include "ignis/device.hpp"
#include "ignis/fence.hpp"
#include "types.hpp"
#include "material.hpp"

using namespace ignis;

class Mesh {
public:
	Mesh(uint32_t verticesCount,
		 uint32_t indicesCount,
		 Material* material = nullptr);

	void update(std::span<Vertex>, std::span<Index>);

	void upload();

	const auto& getVertices() const { return m_vertices; }
	const auto& getIndices() const { return m_indices; }
	auto& getMaterial() const { return *m_material; }

	auto getIndexBuffer() -> ignis::Buffer&;

	auto getVertexBufferAddress() -> VkDeviceAddress;

	auto setMaterial(Material* material) -> void { m_material = material; }

protected:
	bool m_dirty = true;

	std::vector<Vertex> m_vertices;
	std::vector<Index> m_indices;
	Material* m_material{nullptr};

	Buffer* m_vertexBuffer{nullptr};
	Buffer* m_indexBuffer{nullptr};

	Fence* m_waitForUpload{nullptr};
};
