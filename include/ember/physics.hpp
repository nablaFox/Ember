#pragma once

#include "renderer.hpp"
#include "transform.hpp"

namespace ember {

struct PhysicalObject {
	float mass{1};
	Vec3 force{};
	Vec3 acc{};
	Vec3 vel{};

	WorldTransform transform;

	Drawable* shape{nullptr};
};

class PhysicalSystem {
public:
	struct CreateInfo {
		float pixelsPerMeter{5};
		float timeStep{1.f / 60};
	};

	PhysicalSystem(CreateInfo info)
		: m_pixelsPerMeter(info.pixelsPerMeter), m_timeStep(info.timeStep) {}

	void addObject(PhysicalObject* obj) {
		obj->transform.position *= m_pixelsPerMeter;
		obj->transform.scale *= m_pixelsPerMeter;
		m_objects.push_back(obj);
	}

	void update(float dt) {
		if (dt > 0.25f) {
			dt = 0.25f;
		}

		m_timeAccumulator += dt;

		while (m_timeAccumulator >= m_timeStep) {
			integrate();
			m_time += m_timeStep;
			m_timeAccumulator -= m_timeStep;
		}

		m_time = m_time > 1000 ? 0 : m_time;

		for (auto& obj : m_objects) {
			obj->force = {};
		}
	}

	void integrate() {
		for (auto& obj : m_objects) {
			obj->acc = (m_globalForce + obj->force) / obj->mass;
			obj->vel += obj->acc * m_timeStep;
			obj->transform.position += obj->vel * m_timeStep * m_pixelsPerMeter;
		}
	}

	void draw(Renderer& renderer) {
		for (auto& obj : m_objects) {
			assert(obj->shape != nullptr);

			obj->shape->draw(renderer, obj->transform);
		}
	}

	void applyGlobalForce(Vec3 force) { m_globalForce = std::move(force); }

	float getSimulationTime() const { return m_time; }

	float getSimultionStep() const { return m_timeStep; }

protected:
	std::vector<PhysicalObject*> m_objects;

	float m_pixelsPerMeter;

	Vec3 m_globalForce{};

	float m_timeAccumulator{0};

	float m_timeStep;

	float m_time{0};
};

}  // namespace ember
