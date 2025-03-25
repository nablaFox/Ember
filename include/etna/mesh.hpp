#pragma once

#include <memory>
#include <vector>
#include "ignis/types.hpp"
#include "math.hpp"
#include "color.hpp"

namespace etna {

struct Vertex {
	Vec3 position;
	float padding;
	Color color;
	Vec2 uv;
	float padding2[2];
};

typedef uint32_t Index;

class _Mesh {
public:
	struct CreateInfo {
		std::vector<Vertex> vertices;
		std::vector<Index> indices;
	};

	_Mesh(const CreateInfo&);

	~_Mesh();

	auto indexCount() const { return m_indexCount; }

	auto vertexCount() const { return m_vertexCount; }

	auto getVertexBuffer() const { return m_vertexBuffer; }

	auto getIndexBuffer() const { return m_indexBuffer; }

private:
	ignis::BufferId m_vertexBuffer{IGNIS_INVALID_BUFFER_ID};
	ignis::BufferId m_indexBuffer{IGNIS_INVALID_BUFFER_ID};

	uint32_t m_indexCount{0};
	uint32_t m_vertexCount{0};

public:
	_Mesh(const _Mesh&) = delete;
	_Mesh(_Mesh&&) = delete;
	_Mesh& operator=(const _Mesh&) = delete;
	_Mesh& operator=(_Mesh&&) = delete;
};

using Mesh = std::shared_ptr<_Mesh>;

}  // namespace etna
