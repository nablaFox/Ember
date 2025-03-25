#pragma once

#include "ignis/types.hpp"
#include "material.hpp"
#include "mesh.hpp"

namespace etna {

class Scene {
public:
	Scene();
	~Scene();

	void addMesh(const Mesh, const Material);

private:
	BufferId sceneDataUBO;
};

}  // namespace etna
