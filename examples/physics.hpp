#pragma once

#include <vector>
#include "etna/math.hpp"

struct PhysicalObject {
	float mass{1};
	etna::Vec3 force{};
	etna::Vec3 acc{};
	etna::Vec3 vel{};
	etna::Vec3 pos{};  // position in physics world
};

class PhysicalSystem {
public:
	struct CreateInfo {
		float timeStep{1.f / 60.f};
	};

	PhysicalSystem(const CreateInfo& info) : m_timeStep(info.timeStep) {}

	// TODO: removing objects
	void addObject(PhysicalObject* obj) { m_objects.push_back(obj); }

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

		for (auto& obj : m_objects) {
			obj->force = {};
		}

		m_globalForce = {};
		m_globalAcc = {};
	}

	// verlet integration
	void integrate() {
		for (auto& obj : m_objects) {
			etna::Vec3 oldAcc = obj->acc;

			obj->pos +=
				obj->vel * m_timeStep + oldAcc * etna::square(m_timeStep) * 0.5f;

			etna::Vec3 newAcc =
				m_globalAcc + (m_globalForce + obj->force) / obj->mass;

			obj->vel += (oldAcc + newAcc) * m_timeStep * 0.5f;

			obj->acc = std::move(newAcc);
		}
	}

	void applyGlobalAcc(const etna::Vec3& acc) { m_globalAcc += acc; }

	void applyGlobalForce(const etna::Vec3& force) { m_globalForce += force; }

	void applyGravity(float gravity) { applyGlobalAcc({0, -gravity, 0}); }

	float getSimulationTime() const { return m_time; }

	float getSimultionStep() const { return m_timeStep; }

protected:
	std::vector<PhysicalObject*> m_objects;

	etna::Vec3 m_globalAcc{};
	etna::Vec3 m_globalForce{};

	float m_timeAccumulator{0};

	float m_timeStep;

	float m_time{0};
};
