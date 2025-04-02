#pragma once

#include "mesh.hpp"
#include "color.hpp"

namespace etna::engine {

MeshHandle createSphere(Color color = WHITE, uint32_t precision = 100);

MeshHandle createTexturedBrick(float width,
							   float height,
							   float depth,
							   Color color = WHITE);

MeshHandle createTexturedCube(Color color = WHITE);

MeshHandle createRectangle(float width, float height, Color color = WHITE);

MeshHandle createQuad(Color color = WHITE);

// TODO: implement
MeshHandle createCylinder(float radius, float height, Color color = WHITE);

// TODO: variant for empty cyilinder

// TODO: implement
MeshHandle createCone(float radius, float height, Color color = WHITE);

MeshHandle createPyramid(float height, float sideLength, Color color = WHITE);

}  // namespace etna::engine
