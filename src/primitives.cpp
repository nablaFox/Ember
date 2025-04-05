#include "primitives.hpp"

using namespace etna;

MeshHandle engine::createSphere(float radius, uint32_t precision) {
	uint32_t vertexCount = square(precision + 1);
	uint32_t indexCount = 6 * square(precision);

	std::vector<Vertex> vertices(vertexCount);
	std::vector<Index> indices(indexCount);

	for (uint32_t i = 0; i <= precision; i++) {
		float phi = (float)i / precision * 2 * M_PI;  // Azimuthal angle [0, 2π]

		for (uint32_t j = 0; j <= precision; j++) {
			float theta = (float)j / precision * M_PI;	// Polar angle [0, π]

			float x = sinf(theta) * cosf(phi);
			float y = sinf(theta) * sinf(phi);
			float z = cosf(theta);

			vertices[i * (precision + 1) + j] =
				Vertex{.position = Vec3{x, y, z} * radius};
		}
	}

	for (uint32_t i = 0; i < precision; i++) {
		for (uint32_t j = 0; j < precision; j++) {
			uint32_t i0 = i * (precision + 1) + j;
			uint32_t i1 = i0 + 1;
			uint32_t i2 = (i + 1) * (precision + 1) + j;
			uint32_t i3 = i2 + 1;

			indices.push_back(i0);
			indices.push_back(i1);
			indices.push_back(i2);

			indices.push_back(i1);
			indices.push_back(i3);
			indices.push_back(i2);
		}
	}

	for (uint32_t i = 0; i <= precision; i++) {
		for (uint32_t j = 0; j <= precision; j++) {
			float u = (float)j / precision;
			float v = (float)i / precision;

			vertices[i * (precision + 1) + j].uv = {u, v};
		}
	}

	return Mesh::create({
		.vertices = std::move(vertices),
		.indices = std::move(indices),
	});
}

MeshHandle engine::createBrick(float width, float height, float depth) {
	std::vector<Vertex> vertices;
	std::vector<Index> indices;
	vertices.reserve(24);
	indices.reserve(36);

	const float hx = width * 0.5f;
	const float hy = height * 0.5f;
	const float hz = depth * 0.5f;

	// Front face (z+)
	vertices.push_back({{-hx, -hy, hz}, {0, 0, 1}, {0, 0}});
	vertices.push_back({{hx, -hy, hz}, {0, 0, 1}, {1, 0}});
	vertices.push_back({{hx, hy, hz}, {0, 0, 1}, {1, 1}});
	vertices.push_back({{-hx, hy, hz}, {0, 0, 1}, {0, 1}});

	// Back face (z-)
	vertices.push_back({{hx, -hy, -hz}, {0, 0, -1}, {0, 0}});
	vertices.push_back({{-hx, -hy, -hz}, {0, 0, -1}, {1, 0}});
	vertices.push_back({{-hx, hy, -hz}, {0, 0, -1}, {1, 1}});
	vertices.push_back({{hx, hy, -hz}, {0, 0, -1}, {0, 1}});

	// Left face (x-)
	vertices.push_back({{-hx, -hy, -hz}, {-1, 0, 0}, {0, 0}});
	vertices.push_back({{-hx, -hy, hz}, {-1, 0, 0}, {1, 0}});
	vertices.push_back({{-hx, hy, hz}, {-1, 0, 0}, {1, 1}});
	vertices.push_back({{-hx, hy, -hz}, {-1, 0, 0}, {0, 1}});

	// Right face (x+)
	vertices.push_back({{hx, -hy, hz}, {1, 0, 0}, {0, 0}});
	vertices.push_back({{hx, -hy, -hz}, {1, 0, 0}, {1, 0}});
	vertices.push_back({{hx, hy, -hz}, {1, 0, 0}, {1, 1}});
	vertices.push_back({{hx, hy, hz}, {1, 0, 0}, {0, 1}});

	// Top face (y+)
	vertices.push_back({{-hx, hy, hz}, {0, 1, 0}, {0, 0}});
	vertices.push_back({{hx, hy, hz}, {0, 1, 0}, {1, 0}});
	vertices.push_back({{hx, hy, -hz}, {0, 1, 0}, {1, 1}});
	vertices.push_back({{-hx, hy, -hz}, {0, 1, 0}, {0, 1}});

	// Bottom face (y-)
	vertices.push_back({{-hx, -hy, -hz}, {0, -1, 0}, {0, 0}});
	vertices.push_back({{hx, -hy, -hz}, {0, -1, 0}, {1, 0}});
	vertices.push_back({{hx, -hy, hz}, {0, -1, 0}, {1, 1}});
	vertices.push_back({{-hx, -hy, hz}, {0, -1, 0}, {0, 1}});

	for (uint32_t i = 0; i < 6; ++i) {
		Index start = i * 4;
		indices.push_back(start);
		indices.push_back(start + 1);
		indices.push_back(start + 2);

		indices.push_back(start);
		indices.push_back(start + 2);
		indices.push_back(start + 3);
	}

	return Mesh::create({
		.vertices = std::move(vertices),
		.indices = std::move(indices),
	});
}

MeshHandle engine::createCube(float side) {
	return createBrick(side, side, side);
}

MeshHandle engine::createQuad(float width, float height) {
	std::vector<Vertex> vertices;
	vertices.resize(4);

	vertices[0] = {
		.position = {0.5, -0.5, 0},
		.uv = {1, 0},
	};

	vertices[1] = {
		.position = {-0.5, -0.5, 0},
		.uv = {0, 0},
	};

	vertices[2] = {
		.position = {-0.5, 0.5, 0},
		.uv = {0, 1},
	};

	vertices[3] = {
		.position = {0.5, 0.5, 0},
		.uv = {1, 1},
	};

	std::vector<Index> indices = {0, 1, 2, 2, 3, 0};

	return Mesh::create({
		.vertices = std::move(vertices),
		.indices = std::move(indices),
	});
}

MeshHandle engine::createPyramid(float height, float baseWidth) {
	std::vector<Vertex> vertices(5);

	vertices[0].position = {0, height, 0};

	vertices[1].position = {baseWidth / 2, 0, baseWidth / 2};
	vertices[2].position = {-baseWidth / 2, 0, baseWidth / 2};
	vertices[3].position = {-baseWidth / 2, 0, -baseWidth / 2};
	vertices[4].position = {baseWidth / 2, 0, -baseWidth / 2};

	std::vector<Index> indices{
		0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1, 1, 2, 3, 1, 3, 4,
	};

	return Mesh::create({
		.vertices = std::move(vertices),
		.indices = std::move(indices),
	});
}
