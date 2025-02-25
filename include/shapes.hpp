#pragma once

#include "mesh.hpp"

struct Square : Mesh {
	Square(Material* material = nullptr) : Mesh(4, 6, material) {
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
	Cube(Material* material = nullptr) : Mesh(8, 36, nullptr) {
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
