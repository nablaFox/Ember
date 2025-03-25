#include "mesh.hpp"

namespace etna::utils {

constexpr static uint32_t SPHERE_DEFAULT_PRECISION = 32;

Mesh createSphere(uint32_t precision = SPHERE_DEFAULT_PRECISION);
Mesh createTexturedBrick(float width, float height, float depth);

}  // namespace etna::utils
