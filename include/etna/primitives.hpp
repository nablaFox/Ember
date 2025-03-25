#include "mesh.hpp"

namespace etna::utils {

Mesh createSphere(Color color = WHITE, uint32_t precision = 32);

Mesh createTexturedBrick(float width,
						 float height,
						 float depth,
						 Color color = WHITE);

Mesh createRectangle(float width, float height, Color color = WHITE);

Mesh createQuad(Color color = WHITE);

}  // namespace etna::utils
