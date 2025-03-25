#pragma once

#include <memory>
#include <vector>
#include "ignis/types.hpp"
#include "ignis/buffer.hpp"
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

	uint32_t indexCount() const;

	uint32_t vertexCount() const;

	auto getVertexBuffer() const { return m_vertexBuffer; }

	auto getIndexBuffer() const { return m_indexBuffer; }

private:
	ignis::BufferId m_vertexBuffer{IGNIS_INVALID_BUFFER_ID};
	ignis::Buffer* m_indexBuffer{nullptr};

public:
	_Mesh(const _Mesh&) = delete;
	_Mesh(_Mesh&&) = delete;
	_Mesh& operator=(const _Mesh&) = delete;
	_Mesh& operator=(_Mesh&&) = delete;
};

using Mesh = std::shared_ptr<_Mesh>;

}  // namespace etna
