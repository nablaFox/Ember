#pragma once

#include <memory>
#include <vector>
#include "ignis/types.hpp"
#include "math.hpp"
#include "color.hpp"

namespace etna {

class Engine;

struct Vertex {
	Vec3 position;
	float padding;
	Color color;
	Vec2 uv;
	float padding2[2];
};

typedef uint32_t Index;

class _Mesh {
	friend class Engine;

public:
	struct CreateInfo {
		std::vector<Vertex> vertices;
		std::vector<Index> indices;
	};

	~_Mesh();

	uint32_t indexCount() const;

private:
	_Mesh(const CreateInfo&);

	BufferId vertexBuffer;
	BufferId indexBuffer;

public:
	_Mesh(const _Mesh&) = delete;
	_Mesh(_Mesh&&) = delete;
	_Mesh& operator=(const _Mesh&) = delete;
	_Mesh& operator=(_Mesh&&) = delete;
};

using Mesh = std::shared_ptr<_Mesh>;

}  // namespace etna
