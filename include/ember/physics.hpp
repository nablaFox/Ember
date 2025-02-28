#pragma once

#include <functional>
#include "renderer.hpp"
#include "types.hpp"

namespace ember {

struct Drawable {
	virtual void draw(Renderer& renderer, WorldTransform transform) = 0;
};

struct PhysicalObject {
	float mass{1};
	Vec3 force{};
	Vec3 acc{};
	Vec3 vel{};
	Vec3 pos{};

	float roll{0};
	float pitch{0};
	float yaw{0};

	float width{1};
	float height{1};
	float depth{1};

	Drawable* shape{nullptr};

	WorldTransform getWorldTransform() const {
		return {
			.yaw = yaw,
			.pitch = pitch,
			.roll = roll,
			.position = pos,
		};
	}
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
		object->pos *= m_pixelsPerMeter;
		objects.push_back(object);
	}

	void update(float dt) {
		if (dt > 0.25f) {
			dt = 0.25f;
		}

		m_timeAccumulator += dt;

		updateBoundingBox();

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
			obj->pos += obj->vel * m_timeStep * m_pixelsPerMeter;
		}
	}

	void updateBoundingBox() {
		for (auto& obj : objects) {
			Mat3 rollRot = {
				{std::abs(cosf(obj->roll)), std::abs(sinf(obj->roll)), 0},
				{std::abs(sinf(obj->roll)), std::abs(cosf(obj->roll)), 0},
				{0, 0, 1},
			};

			Mat3 pitchRot = {
				{1, 0, 0},
				{0, std::abs(cosf(obj->pitch)), std::abs(sinf(obj->pitch))},
				{0, std::abs(sinf(obj->pitch)), std::abs(cosf(obj->pitch))},
			};

			Mat3 yawRot = {
				{std::cosf(obj->yaw), 0, std::sinf(obj->yaw)},
				{0, 1, 0},
				{-std::sinf(obj->yaw), 0, std::cosf(obj->yaw)},
			};

			static Vec3 originalSize = {obj->width, obj->height, obj->depth};

			Vec3 rotatedHeight = pitchRot * rollRot * originalSize;
			Vec3 rotatedWidth = yawRot * rollRot * originalSize;
			Vec3 rotatedDepth = yawRot * pitchRot * originalSize;

			obj->width = rotatedWidth[0];
			obj->height = rotatedHeight[1];
			obj->depth = rotatedDepth[2];
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

			obj->shape->draw(renderer, obj->getWorldTransform());
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
