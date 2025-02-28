#pragma once

#include <functional>
#include "renderer.hpp"
#include "types.hpp"

namespace ember {

struct Drawable {
	virtual void draw(Renderer& renderer, WorldTransform transform) = 0;
};

struct AABB {
	Vec3 min;
	Vec3 max;

	static AABB compute(const AABB localAABB, const WorldTransform& transform) {
		Vec3 localCenter = (localAABB.min + localAABB.max) * 0.5f;
		Vec3 halfSize = (localAABB.max - localAABB.min) / 2;

		Vec3 center = transform.position + localCenter;

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

		return {
			.min = center - newHalfSize,
			.max = center + newHalfSize,
		};
	}

	// TEMP just for debug
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

struct PhysicalObject {
	float mass{1};
	Vec3 force{};
	Vec3 acc{};
	Vec3 vel{};

	WorldTransform transform;

	AABB aabb;

	AABB getWorldAABB() const { return AABB::compute(aabb, transform); }

	Drawable* shape{nullptr};
};

using Constraint = std::function<void(PhysicalObject&)>;

class PhysicalSystem {
public:
	struct CreateInfo {
		uint32_t pixelsPerMeter{5};
		float timeStep{1.f / 60};
	};

	PhysicalSystem(CreateInfo info)
		: m_pixelsPerMeter(info.pixelsPerMeter), m_timeStep(info.timeStep) {}

	void addObject(PhysicalObject* object) {
		objects.push_back(object);
		object->transform.position *= m_pixelsPerMeter;
	}

	void update(float dt) {
		if (dt > 0.25f) {
			dt = 0.25f;
		}

		m_timeAccumulator += dt;

		while (m_timeAccumulator >= m_timeStep) {
			integrate();
			applyConstraints();
			m_timeAccumulator -= m_timeStep;
		}
	}

	void integrate() {
		for (auto& obj : objects) {
			obj->acc = (m_globalForce + obj->force) / obj->mass;
			obj->vel += obj->acc * m_timeStep;
			obj->transform.position += obj->vel * m_timeStep * m_pixelsPerMeter;
		}
	}

	void applyConstraints() {
		for (auto& obj : objects) {
			for (auto& constraint : globalConstraints) {
				constraint(*obj);
			}
		}
	}

	void draw(Renderer& renderer) {
		for (auto& obj : objects) {
			assert(obj->shape != nullptr);

			obj->shape->draw(renderer, obj->transform);
		}
	}

	void applyGlobalForce(Vec3 force) { m_globalForce = std::move(force); }

	void addConstraint(std::function<void(PhysicalObject&)> constraint) {
		globalConstraints.push_back(std::move(constraint));
	}

private:
	std::vector<PhysicalObject*> objects;

	uint32_t m_pixelsPerMeter;

	Vec3 m_globalForce{};

	float m_timeAccumulator{0};

	float m_timeStep;

	std::vector<Constraint> globalConstraints;
};

}  // namespace ember
