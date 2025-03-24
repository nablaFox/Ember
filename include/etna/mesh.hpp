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
public:
	struct CreateInfo {
		std::vector<Vertex> vertices;
		std::vector<Index> indices;
	};

	struct CreateInfo2 {
		BufferId vertexBuffer;
		size_t vertexBufferOffset;
		BufferId indexBuffer;
		size_t indexBufferOffset;
	};

	~_Mesh();

private:
	_Mesh(const Engine, const CreateInfo&);
	_Mesh(const Engine, const CreateInfo2&);

	const Engine& engine;

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
