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

MeshHandle engine::createUVBrick(float width, float height, float depth) {
	std::vector<Vertex> vertices(24);

	for (uint32_t j = 0; j < 3; j++) {
		for (uint32_t i = 0; i < 2; i++) {
			float zShift = (depth / 2.f) - i * depth;
			uint32_t base = i * 4 + 8 * j;

			vertices[base + 0].position = {width / 2.f, -height / 2.f, zShift};
			vertices[base + 1].position = {-width / 2.f, -height / 2.f, zShift};
			vertices[base + 2].position = {-width / 2.f, height / 2.f, zShift};
			vertices[base + 3].position = {width / 2.f, height / 2.f, zShift};
		}
	}

	// front and back
	for (uint32_t face = 0; face < 2; ++face) {
		uint32_t offset = face * 4;
		vertices[offset + 0].uv = {1.0f, 0.0f};
		vertices[offset + 1].uv = {0.0f, 0.0f};
		vertices[offset + 2].uv = {0.0f, 1.0f};
		vertices[offset + 3].uv = {1.0f, 1.0f};
	}

	// bottom
	vertices[8].uv = {0.0f, 1.0f};
	vertices[9].uv = {0.0f, 0.0f};
	vertices[13].uv = {1.0f, 0.0f};
	vertices[12].uv = {1.0f, 1.0f};

	// top
	vertices[10].uv = {0.0f, 1.0f};
	vertices[14].uv = {0.0f, 0.0f};
	vertices[15].uv = {1.0f, 0.0f};
	vertices[11].uv = {1.0f, 1.0f};

	// right
	vertices[16].uv = {0.0f, 1.0f};
	vertices[20].uv = {0.0f, 0.0f};
	vertices[23].uv = {1.0f, 0.0f};
	vertices[19].uv = {1.0f, 1.0f};

	// left
	vertices[17].uv = {0.0f, 1.0f};
	vertices[18].uv = {0.0f, 0.0f};
	vertices[22].uv = {1.0f, 0.0f};
	vertices[21].uv = {1.0f, 1.0f};

	std::vector<Index> indices = {
		0,	1,	2,	2,	3,	0,	 // Front
		4,	5,	6,	6,	7,	4,	 // Back
		10, 14, 15, 15, 11, 10,	 // Top
		8,	9,	13, 13, 12, 8,	 // Bottom
		16, 20, 23, 23, 19, 16,	 // Right
		17, 18, 22, 22, 21, 17	 // Left
	};

	return Mesh::create({
		.vertices = std::move(vertices),
		.indices = std::move(indices),
	});
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
