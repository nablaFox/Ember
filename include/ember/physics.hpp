#pragma once

#include "renderer.hpp"
#include "transform.hpp"

namespace ember {

struct PhysicalObject {
	float mass{1};
	Vec3 force{};
	Vec3 acc{};
	Vec3 vel{};
	Vec3 pos{};	 // position in physics world

	WorldTransform transform;  // position in render world

	Drawable* shape{nullptr};
};

class PhysicalSystem {
public:
	struct CreateInfo {
		float timeStep{1.f / 60.f};
		float worldUnitPerMeter{1};
	};

	PhysicalSystem(CreateInfo info)
		: m_timeStep(info.timeStep), m_wuPerMeter(info.worldUnitPerMeter) {}

	void addObject(PhysicalObject* obj) { m_objects.push_back(obj); }

	float meterToWu(float meter) { return meter * m_wuPerMeter; }
	float wuToMeter(float worldUnit) { return worldUnit / m_wuPerMeter; }

	Vec3 meterToWu(Vec3 meter) { return meter * m_wuPerMeter; }
	Vec3 wuToMeter(Vec3 worldUnit) { return worldUnit / m_wuPerMeter; }

	void update(float dt) {
		if (dt > 0.25f) {
			dt = 0.25f;
		}

		m_timeAccumulator += dt;

		for (auto& obj : m_objects) {
			obj->pos = wuToMeter(obj->transform.position);
		}

		while (m_timeAccumulator >= m_timeStep) {
			integrate();
			m_time += m_timeStep;
			m_timeAccumulator -= m_timeStep;
		}

		for (auto& obj : m_objects) {
			obj->force = {};
			obj->transform.position = meterToWu(obj->pos);
		}
	}

	// verlet integration
	void integrate() {
		for (auto& obj : m_objects) {
			Vec3 oldAcc = obj->acc;

			obj->pos += obj->vel * m_timeStep + oldAcc * square(m_timeStep) * 0.5f;

			Vec3 newAcc = (m_globalForce + obj->force) / obj->mass;

			obj->vel += (oldAcc + newAcc) * m_timeStep * 0.5f;

			obj->acc = std::move(newAcc);
		}
	}

	// TEMP
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

	Vec3 m_globalForce{};

	float m_timeAccumulator{0};

	float m_wuPerMeter;

	float m_timeStep;

	float m_time{0};
};

}  // namespace ember
