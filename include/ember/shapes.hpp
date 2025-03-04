#pragma once

#include "mesh.hpp"

namespace ember {

struct Square : Mesh {
	Square() : Mesh(4, 6) {
		m_vertices[0] = {
			.position = {0.5, -0.5, 0},
			.uv = {1, 0},
		};

		m_vertices[1] = {
			.position = {-0.5, -0.5, 0},
			.uv = {0, 0},
		};

		m_vertices[2] = {
			.position = {-0.5, 0.5, 0},
			.uv = {0, 1},
		};

		m_vertices[3] = {
			.position = {0.5, 0.5, 0},
			.uv = {1, 1},
		};

		m_indices = {0, 1, 2, 2, 3, 0};
	}
};

// TODO: maybe a non-textured variant with just 8 vertices?
struct Brick : Mesh {
	Brick(float width, float height, float depth) : Mesh(24, 36) {
		for (uint32_t j = 0; j < 3; j++) {
			for (uint32_t i = 0; i < 2; i++) {
				float zShift = (depth / 2.f) - i * depth;
				uint32_t base = i * 4 + 8 * j;

				m_vertices[base + 0].position = {width / 2.f, -height / 2.f, zShift};
				m_vertices[base + 1].position = {-width / 2.f, -height / 2.f,
												 zShift};
				m_vertices[base + 2].position = {-width / 2.f, height / 2.f, zShift};
				m_vertices[base + 3].position = {width / 2.f, height / 2.f, zShift};
			}
		}

		// front and back
		for (uint32_t face = 0; face < 2; ++face) {
			uint32_t offset = face * 4;
			m_vertices[offset + 0].uv = {1.0f, 0.0f};
			m_vertices[offset + 1].uv = {0.0f, 0.0f};
			m_vertices[offset + 2].uv = {0.0f, 1.0f};
			m_vertices[offset + 3].uv = {1.0f, 1.0f};
		}

		// bottom
		m_vertices[8].uv = {0.0f, 1.0f};
		m_vertices[9].uv = {0.0f, 0.0f};
		m_vertices[13].uv = {1.0f, 0.0f};
		m_vertices[12].uv = {1.0f, 1.0f};

		// top
		m_vertices[10].uv = {0.0f, 1.0f};
		m_vertices[14].uv = {0.0f, 0.0f};
		m_vertices[15].uv = {1.0f, 0.0f};
		m_vertices[11].uv = {1.0f, 1.0f};

		// right
		m_vertices[16].uv = {0.0f, 1.0f};
		m_vertices[20].uv = {0.0f, 0.0f};
		m_vertices[23].uv = {1.0f, 0.0f};
		m_vertices[19].uv = {1.0f, 1.0f};

		// left
		m_vertices[17].uv = {0.0f, 1.0f};
		m_vertices[18].uv = {0.0f, 0.0f};
		m_vertices[22].uv = {1.0f, 0.0f};
		m_vertices[21].uv = {1.0f, 1.0f};

		m_indices = {
			0,	1,	2,	2,	3,	0,	 // Front
			4,	5,	6,	6,	7,	4,	 // Back
			10, 14, 15, 15, 11, 10,	 // Top
			8,	9,	13, 13, 12, 8,	 // Bottom
			16, 20, 23, 23, 19, 16,	 // Right
			17, 18, 22, 22, 21, 17	 // Left
		};
	}
};

struct Cube : Brick {
	Cube(float sideLength = 1) : Brick(sideLength, sideLength, sideLength) {}
};

struct Circle : Mesh {
	Circle(uint32_t vertices = 100) : Mesh(vertices + 1, 3 * vertices - 1) {
		for (uint32_t i = 0; i <= vertices; i++) {
			float angle = (float)i / vertices * 2 * M_PI;
			float x = cosf(angle);
			float y = sinf(angle);
			float u = 0.5f + 0.5f * x;
			float v = 0.5f + 0.5f * y;

			m_vertices[i] = Vertex{
				.position = {x, y, 0},
				.uv = {u, v},
			};
		}

		std::vector<Index> indices;
		indices.reserve(m_indices.size());

		for (uint32_t i = 0; i < vertices - 1; i++) {
			indices.push_back(i);
			indices.push_back(i + 1);
			indices.push_back(vertices);
		}

		for (int i = 0; i < m_indices.size(); i++) {
			m_indices[i] = indices[i];
		}
	}
};

// TODO: maybe also an icosphere variant?
struct Sphere : Mesh {
	Sphere(uint32_t precision = 100)
		: Mesh(square(precision + 1), 6 * square(precision)) {
		for (uint32_t i = 0; i <= precision; i++) {
			float phi = (float)i / precision * 2 * M_PI;  // Azimuthal angle [0, 2π]
			for (uint32_t j = 0; j <= precision; j++) {
				float theta = (float)j / precision * M_PI;	// Polar angle [0, π]

				float x = sinf(theta) * cosf(phi);
				float y = sinf(theta) * sinf(phi);
				float z = cosf(theta);

				m_vertices[i * (precision + 1) + j] = Vertex{.position = {x, y, z}};
			}
		}

		std::vector<Index> indices;
		indices.reserve(m_indices.size());

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

		for (int i = 0; i < m_indices.size(); i++) {
			m_indices[i] = indices[i];
		}

		for (uint32_t i = 0; i <= precision; i++) {
			for (uint32_t j = 0; j <= precision; j++) {
				float u = (float)j / precision;
				float v = (float)i / precision;

				m_vertices[i * (precision + 1) + j].uv = {u, v};
			}
		}
	}
};

struct Pyramid : Mesh {
	Pyramid(float height = 1, float sideLength = 1) : Mesh(5, 18) {
		m_vertices[0].position = {0, height, 0};

		m_vertices[1].position = {sideLength / 2, 0, sideLength / 2};
		m_vertices[2].position = {-sideLength / 2, 0, sideLength / 2};
		m_vertices[3].position = {-sideLength / 2, 0, -sideLength / 2};
		m_vertices[4].position = {sideLength / 2, 0, -sideLength / 2};

		m_indices = {
			0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1, 1, 2, 3, 1, 3, 4,
		};
	}
};

};	// namespace ember
