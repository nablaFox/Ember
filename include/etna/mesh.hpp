#pragma once

#include <memory>
#include <vector>
#include "device.hpp"
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

	struct CreateInfo2 {
		BufferId vertexBuffer{IGNIS_INVALID_BUFFER_ID};
		BufferId indexBuffer{IGNIS_INVALID_BUFFER_ID};
		size_t firstIndex{0};
		size_t indexCount{0};  // 0 means all
	};

	~_Mesh();  // if no engine it will do nothing

private:
	_Mesh(const Engine*, const CreateInfo&);
	_Mesh(const CreateInfo2&);

	ignis::Device* m_device;

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
