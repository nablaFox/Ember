#pragma once

#include <span>
#include "ignis/buffer.hpp"
#include "math.hpp"

// RGBA16 bit color
struct Color {
	uint16_t r, g, b, a;
};

struct Vertex {
	vec3 position;
	Color color;
};

typedef uint32_t Index;

class Mesh {
public:
	Mesh(std::span<Vertex>, std::span<Index>) {};

	void update(std::span<Vertex>, std::span<Index>);

	const auto& getVertices() const { return vertices; }
	const auto& getIndices() const { return indices; }
	const auto& getIndexBuffer() const { return indexBuffer; }

	auto getVertexBufferAddress() const -> VkDeviceAddress {
		return vertexBuffer->getDeviceAddress();
	}

private:
	std::span<Vertex> vertices;
	std::span<Index> indices;

	ignis::Buffer* vertexBuffer;
	ignis::Buffer* indexBuffer;
};
