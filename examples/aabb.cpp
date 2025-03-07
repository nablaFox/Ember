#include "shared.hpp"
#include "ember/physics.hpp"

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;

struct AABB {
	Vec3 min;
	Vec3 max;

	static AABB compute(AABB localAABB, const WorldTransform& transform) {
		Vec3 halfSize = (localAABB.max - localAABB.min) / 2;

		Vec3 center = transform.position + localAABB.center();

		Mat3 rotation = transform.getRotationMat3();

		Vec3 newHalfSize;

		newHalfSize[0] = std::abs(rotation(0, 0)) * halfSize[0] +
						 std::abs(rotation(0, 1)) * halfSize[1] +
						 std::abs(rotation(0, 2)) * halfSize[2];

		newHalfSize[1] = std::abs(rotation(1, 0)) * halfSize[0] +
						 std::abs(rotation(1, 1)) * halfSize[1] +
						 std::abs(rotation(1, 2)) * halfSize[2];

		newHalfSize[2] = std::abs(rotation(2, 0)) * halfSize[0] +
						 std::abs(rotation(2, 1)) * halfSize[1] +
						 std::abs(rotation(2, 2)) * halfSize[2];

		newHalfSize *= transform.scale;

		return {
			.min = center - newHalfSize,
			.max = center + newHalfSize,
		};
	}

	bool intersects(const AABB& other) const {
		return min[0] <= other.max[0] && max[0] >= other.min[0] &&
			   min[1] <= other.max[1] && max[1] >= other.min[1] &&
			   min[2] <= other.max[2] && max[2] >= other.min[2];
	}

	Vec3 center() const { return (max + min) / 2; }

	std::array<Vec3, 8> getVertices() const {
		float width = max[0] - min[0];
		float height = max[1] - min[1];
		float depth = max[2] - min[2];

		Vec3 center = (max + min) / 2;

		return {
			Vec3{width / 2.f, -height / 2.f, -depth / 2.f} + center,
			Vec3{-width / 2.f, -height / 2.f, -depth / 2.f} + center,
			Vec3{-width / 2.f, height / 2.f, -depth / 2.f} + center,
			Vec3{width / 2.f, height / 2.f, -depth / 2.f} + center,

			Vec3{width / 2.f, -height / 2.f, depth / 2.f} + center,
			Vec3{-width / 2.f, -height / 2.f, depth / 2.f} + center,
			Vec3{-width / 2.f, height / 2.f, depth / 2.f} + center,
			Vec3{width / 2.f, height / 2.f, depth / 2.f} + center,
		};
	}
};

struct BoundedObject : PhysicalObject {
	AABB aabb;
	AABB getWorldAABB() { return AABB::compute(aabb, transform); }
};

class BoxSystem : public PhysicalSystem {
public:
	struct CreateInfo {
		float dimension{1};
		float worldUnitPerMeter{1};
		Vec3 position{0, 0, 0};
		Vec3 gravity{0, -9.8, 0};
	};

	BoxSystem(CreateInfo info)
		: PhysicalSystem({.worldUnitPerMeter = info.worldUnitPerMeter}),
		  m_box(info.dimension),
		  m_transform({.position = info.position}),
		  m_boxAABB({-info.dimension / 2, -info.dimension / 2, -info.dimension / 2},
					{info.dimension / 2, info.dimension / 2, info.dimension / 2}),
		  m_dimension(info.dimension) {
		applyGlobalForce(info.gravity);
	}

	void applyConstraint() {
		auto boxAABB = AABB::compute(m_boxAABB, m_transform);

		for (auto& object : m_objects) {
			auto objAABB = ((BoundedObject*)object)->getWorldAABB();

			for (int axis{0}; axis < 3; axis++) {
				if (objAABB.min[axis] < boxAABB.min[axis]) {
					object->transform.position[axis] +=
						boxAABB.min[axis] - objAABB.min[axis];

					object->vel[axis] *= -0.2;
				}

				if (objAABB.max[axis] > boxAABB.max[axis]) {
					object->transform.position[axis] -=
						objAABB.max[axis] - boxAABB.max[axis];

					object->vel[axis] *= -0.2;
				}
			}
		}
	}

	void addObject(BoundedObject* object) { PhysicalSystem::addObject(object); }

	void draw(Renderer& renderer) {
		m_box.draw(renderer, m_transform);
		PhysicalSystem::draw(renderer);
	}

	void update(float dt) {
		PhysicalSystem::update(dt);
		applyConstraint();
	}

private:
	BoxShape m_box;
	AABB m_boxAABB;
	WorldTransform m_transform;
	float m_dimension;
};

class BoundingVolume {
public:
	BoundingVolume(BoundedObject* object) : m_object(object) {
		m_volume = new Mesh(8, 36);

		std::vector<Index> indices = {
			0, 1, 2, 2, 3, 0,  // Front
			4, 5, 6, 6, 7, 4,  // Back
			0, 4, 5, 5, 1, 0,  // Bottom
			3, 7, 6, 6, 2, 3,  // Top
			0, 4, 7, 7, 3, 0,  // Right
			1, 5, 6, 6, 2, 1,  // Left
		};

		m_volume->updateIndices(indices);

		m_volume->setColor(GREEN);
		setVertices();
	}

	void draw(Renderer& renderer) {
		setVertices();
		renderer.draw(*m_volume, {}, &lineMaterial);
	}

	void setVertices() {
		std::vector<Vertex> vertices;

		for (auto& vertex : m_object->getWorldAABB().getVertices()) {
			vertices.push_back({
				.position = vertex,
				.color = RED,
			});
		}

		m_volume->updateVertices(vertices);
	}

private:
	BoundedObject* m_object;
	Mesh* m_volume;
};

auto main(int argc, char* argv[]) -> int {
	Window window({
		.windowName = "AABB Demo",
		.width = WINDOW_WIDTH,
		.height = WINDOW_HEIGHT,
		.captureMouse = true,
	});

	Renderer renderer(window);

	constexpr float boxDimension = 10;

	FirstPersonCamera playerCamera({
		.fov = 90,
		.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
		.cameraSpeed = 7.f,
		.flyAround = true,
		.transform = {.position = {0, 4.5, 11}},
	});

	Floor floor({
		.color = PURPLE.setAlpha(0.3),
	});

	BoxSystem boxSystem({
		.dimension = boxDimension,
		.position = {0, boxDimension / 2.f, 0},
		.gravity = {0, -9.8, 0},
	});

	OutlinedBrick brick({
		.width = 1,
		.height = 0.5,
		.depth = 0.5,
	});

	BoundedObject boundedObject{
		.aabb = AABB({-0.5, -0.25, -0.25}, {0.5, 0.25, 0.25}),
	};

	boundedObject.shape = &brick;
	boundedObject.transform.scale = 2;

	boxSystem.addObject(&boundedObject);

	BoundingVolume objectBoundingVolume(&boundedObject);

	while (!window.shouldClose()) {
		if (window.isKeyClicked(GLFW_KEY_T)) {
			boundedObject.transform.position[1] =
				boxDimension - boundedObject.getWorldAABB().max[1] / 2;
		}

		if (window.isKeyPressed(GLFW_KEY_P)) {
			boundedObject.transform.pitch += 0.01;
		}

		if (window.isKeyPressed(GLFW_KEY_Y)) {
			boundedObject.transform.yaw += 0.01;
		}

		if (window.isKeyPressed(GLFW_KEY_R)) {
			boundedObject.transform.roll += 0.01;
		}

		const float speed = 2 * renderer.getDeltaTime();

		if (window.isKeyPressed(GLFW_KEY_LEFT)) {
			boundedObject.vel[0] -= speed;
		}

		if (window.isKeyPressed(GLFW_KEY_RIGHT)) {
			boundedObject.vel[0] += speed;
		}

		if (window.isKeyPressed(GLFW_KEY_DOWN)) {
			boundedObject.vel[2] += speed;
		}

		if (window.isKeyPressed(GLFW_KEY_UP)) {
			boundedObject.vel[2] -= speed;
		}

		playerCamera.update(window, renderer.getDeltaTime());
		boxSystem.update(renderer.getDeltaTime());

		renderer.beginScene(playerCamera, {});

		floor.draw(renderer);

		objectBoundingVolume.draw(renderer);

		boxSystem.draw(renderer);

		renderer.endScene();

		debug(window, renderer, playerCamera);
	}

	return 0;
}
