#pragma once

#include <memory>
#include "ignis/types.hpp"
#include "transform.hpp"
#include "math.hpp"

namespace etna {

class Camera {
public:
	struct CreateInfo {
		float fov{60.f};
		float near{0.1f};
		float far{100.f};
		float aspect{1.f};
		Transform transform{};
	};

	Camera(const CreateInfo&);

	~Camera();

	Mat4 getProjMatrix() const { return m_projMatrix; }

	Mat4 getViewMatrix() const { return m_viewMatrix; };

	Mat4 getViewProjMatrix() const { return m_projMatrix * m_viewMatrix; }

	ignis::BufferId getDataBuffer() const { return m_cameraData; }

	void updateTransform(const Transform&);
	void updateTransform(const Mat4&);

	void updateFov(float);
	void updateAspect(float);
	void updateNear(float);
	void updateFar(float);

private:
	struct CameraData {
		Mat4 viewproj;
		Mat4 view;
		Mat4 proj;
	};

	float m_fov;
	float m_near;
	float m_far;
	float m_aspect;

	Mat4 m_worldMatrix;
	Mat4 m_projMatrix;
	Mat4 m_viewMatrix;

	ignis::BufferId m_cameraData{IGNIS_INVALID_BUFFER_ID};
};

using CameraHandle = std::shared_ptr<Camera>;

}  // namespace etna
