#include "shared.hpp"

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
constexpr int PIXELS_PER_METER = 2;

class Box {
public:
	struct CreateInfo {
		float width{30};
		float height{30};
		float depth{50};
		Vec3 position{0, 0, 0};
		Color wallColor{PURPLE.setAlpha(0.1)};
		Color borderColor{BLACK.setAlpha(0.5)};
	};

	Box(CreateInfo info)
		: m_outlineMaterial(
			  outlineMaterialTemplate.create({.color = info.borderColor},
											 {.transparency = true})),
		  m_brick(info.width, info.height, info.depth),
		  m_width(info.width),
		  m_height(info.height),
		  m_depth(info.depth),
		  m_position({.position = info.position + Vec3{0, info.height / 2, 0}}) {
		m_brick.setColor(info.wallColor);
	}

	void draw(Renderer& renderer) {
		renderer.draw(m_brick, m_position, &m_outlineMaterial);
	}

	Constraint getBoxConstraint() {
		return [this](PhysicalObject& obj) {
			Vec3 boxMax =
				m_position.position + Vec3{m_width / 2, m_height / 2, m_depth / 2};

			Vec3 boxMin =
				m_position.position - Vec3{m_width / 2, m_height / 2, m_depth / 2};

			AABB objAABB = obj.getWorldAABB();

			for (int axis = 0; axis < 3; axis++) {
				if (objAABB.min[axis] < boxMin[axis]) {
					obj.transform.position[axis] += boxMin[axis] - objAABB.min[axis];
					obj.vel[axis] *= -0.5;
				}

				if (objAABB.max[axis] > boxMax[axis]) {
					obj.transform.position[axis] -= objAABB.max[axis] - boxMax[axis];
					obj.vel[axis] *= -0.5;
				}
			}
		};
	};

private:
	Brick m_brick;
	WorldTransform m_position;
	Material m_outlineMaterial;
	float m_width;
	float m_height;
	float m_depth;
};

// TODO: find a neat way to handle drawable without transforms
// TEMP: just for debug
class BoundingVolume {
public:
	BoundingVolume(PhysicalObject* object) : m_object(object) {
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
	PhysicalObject* m_object;
	Mesh* m_volume;
};

auto main(int argc, char* argv[]) -> int {
	Window window("Basic Ember", WINDOW_WIDTH, WINDOW_HEIGHT);
	Renderer renderer(window);

	FirstPersonCamera playerCamera({
		.fov = 90,
		.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
		.cameraSpeed = 0.2f,
		.flyAround = true,
		.transform = {.position = {1, 17, 28}},
	});

	OutlinedBrick brick({
		.width = 2,
		.height = 1,
		.depth = 1,
	});

	const float boxHeight = 15;
	const float boxWidth = 20;
	const float boxDepth = 10;
	const float boxY = 1;

	Box box({
		.width = boxWidth * PIXELS_PER_METER,
		.height = boxHeight * PIXELS_PER_METER,
		.depth = boxDepth * PIXELS_PER_METER,
		.position = {0, boxY * PIXELS_PER_METER, 0},
	});

	Floor floor(PURPLE.setAlpha(0.3));

	PhysicalObject physicalObject{
		.transform =
			{
				.yaw = M_PI / 4,
				.roll = M_PI / 3,
				.position = {0, boxY + boxHeight, 0},
			},
		.aabb = AABB({-1, -0.5, -0.5}, {1, 0.5, 0.5}),
		.shape = &brick,
	};

	PhysicalSystem system({
		.pixelsPerMeter =
			PIXELS_PER_METER,  // TODO: find a neat way to handle meters
	});

	const Vec3 gravity{0, -1.62, 0};

	system.applyGlobalForce(gravity);

	system.addConstraint(box.getBoxConstraint());

	BoundingVolume objectBoundingVolume(&physicalObject);

	system.addObject(&physicalObject);

	window.caputureMouse(true);

	bool drawBoundingVolume = false;

	while (!window.shouldClose()) {
		renderer.beginScene(playerCamera, {});

		floor.draw(renderer);

		system.draw(renderer);

		box.draw(renderer);

		system.update(renderer.getDeltaTime());

		if (drawBoundingVolume)
			objectBoundingVolume.draw(renderer);

		playerCamera.update(window, renderer.getDeltaTime());

		renderer.endScene();

		if (window.isKeyPressed(GLFW_KEY_UP)) {
			physicalObject.transform.position[1] =
				(boxHeight + boxY) * PIXELS_PER_METER;
		}

		if (window.isKeyPressed(GLFW_KEY_N)) {
			drawBoundingVolume = !drawBoundingVolume;
		}

		if (window.isKeyPressed(GLFW_KEY_P)) {
			physicalObject.transform.pitch += 0.01;
		}

		if (window.isKeyPressed(GLFW_KEY_Y)) {
			physicalObject.transform.yaw += 0.01;
		}

		if (window.isKeyPressed(GLFW_KEY_R)) {
			physicalObject.transform.roll += 0.01;
		}

		debug(window, renderer, playerCamera);
	}

	return 0;
}
