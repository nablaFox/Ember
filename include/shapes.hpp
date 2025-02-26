#pragma once

#include "mesh.hpp"

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

struct Cube : Mesh {
	Cube() : Mesh(8, 36) {
		for (uint32_t i = 0; i < 2; i++) {
			float zShift = 0.5f - i;

			m_vertices[0 + i * 4].position = {0.5f, -0.5f, zShift};
			m_vertices[1 + i * 4].position = {-0.5f, -0.5f, zShift};
			m_vertices[2 + i * 4].position = {-0.5f, 0.5f, zShift};
			m_vertices[3 + i * 4].position = {0.5f, 0.5f, zShift};
		}

		m_indices = {
			0, 1, 2, 2, 3, 0,  // Front face
			4, 5, 6, 6, 7, 4,  // Back face
			1, 5, 4, 4, 0, 1,  // Top face
			3, 7, 6, 6, 2, 3,  // Bottom face
			1, 2, 6, 6, 5, 1,  // Left face
			0, 4, 7, 7, 3, 0,  // Right face
		};
	}
};

struct Circle : Mesh {
	Circle(uint32_t vertices = 100) : Mesh(vertices + 1, 3 * vertices) {
		for (uint32_t i = 0; i <= vertices; i++) {
			float angle = (float)i / vertices * 2 * M_PI;
			float x = cosf(angle);
			float y = sinf(angle);
			m_vertices[i] = Vertex{.position = {x, y, 0}};
		}

		std::vector<Index> indices;
		indices.reserve(m_indices.size());

		for (uint32_t i = 0; i < vertices; i++) {
			indices.push_back(i);
			indices.push_back(i + 1);
			indices.push_back(vertices);
		}

		for (int i = 0; i < m_indices.size(); i++) {
			m_indices[i] = indices[i];
		}
	}
};

struct Sphere : Mesh {
	Sphere(uint32_t precision = 100)
		: Mesh(square(precision + 1) + 1, 6 * square(precision)) {
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
	}
};
