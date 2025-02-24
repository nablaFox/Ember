#pragma once

#include "types.hpp"
#include "mesh.hpp"

struct Square : Mesh {
	Square(Color color) : Mesh(4, 6) {
		m_vertices[0].position = {0.5, -0.5, 0.f};
		m_vertices[1].position = {-0.5, -0.5, 0.f};
		m_vertices[2].position = {-0.5, 0.5, 0.f};
		m_vertices[3].position = {0.5, 0.5, 0.f};

		for (auto& vertex : m_vertices) {
			vertex.color = color;
		}

		m_indices = {0, 1, 2, 2, 3, 0};
	}
};

struct Cube : Mesh {
	static constexpr int verticesCount = 8;
	static constexpr int indicesCount = 36;

	Cube(float sideLength, Color color) : Mesh(verticesCount, indicesCount) {
		for (uint32_t i = 0; i < 2; i++) {
			float zShift = 0.5f - i;

			m_vertices[0 + i * 4].position = {0.5f, -0.5f, zShift};
			m_vertices[1 + i * 4].position = {-0.5f, -0.5f, zShift};
			m_vertices[2 + i * 4].position = {-0.5f, 0.5f, zShift};
			m_vertices[3 + i * 4].position = {0.5f, 0.5f, zShift};
		}

		for (uint32_t i = 0; i < 8; i++) {
			m_vertices[i].color = color;
			m_vertices[i].position *= sideLength;
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

// TEMP this (probably) sucks
// an approach with effects and compute shaders would be better
struct Floor : Mesh {
	using FloorMaterial = MaterialTemplate<Color>;

	Floor(Color color) : Mesh(4, 6) {
		m_material =
			new FloorMaterial({"floor.vert.spv", "default.frag.spv"}, color);

		m_indices = {0, 1, 2, 0, 2, 3};

		m_vertices[0].position = {1, 1, 0.f};
		m_vertices[1].position = {-1, 1, 0.f};
		m_vertices[2].position = {-1, -1, 0.f};
		m_vertices[3].position = {1, -1, 0.f};
	}

	~Floor() { delete m_material; }
};
