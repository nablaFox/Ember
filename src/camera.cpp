#include "camera.hpp"
#include "engine.hpp"

using namespace etna;

static Mat4 calcProjMatrix(float aspect, float fov, float near, float far) {
	const float fovAngle = fov * M_PI / 180;

	const float top = near * tanf(fovAngle / 2);

	const float right = top * aspect;

	return {
		{near / right, 0, 0, 0},
		{0, -near / top, 0, 0},
		{0, 0, (far + near) / (near - far), (2 * far * near) / (near - far)},
		{0, 0, -1, 0},
	};
}

static Mat4 calcViewMatrix(const Mat4& M) {
	const Mat4 R{
		{M(0, 0), M(0, 1), M(0, 2), 0},
		{M(1, 0), M(1, 1), M(1, 2), 0},
		{M(2, 0), M(2, 1), M(2, 2), 0},
		{0, 0, 0, 1},
	};

	const Vec3 translation{M(0, 3), M(1, 3), M(2, 3)};

	const Mat4 T = Transform::getTransMatrix(translation * -1);

	return R * T;
}

Camera::Camera(const CreateInfo& info)
	: m_fov(info.fov),
	  m_near(info.near),
	  m_far(info.far),
	  m_aspect(info.aspect),
	  m_worldMatrix(info.transform.getWorldMatrix()) {
	m_projMatrix = calcProjMatrix(m_aspect, m_fov, m_near, m_far);
	m_viewMatrix = calcViewMatrix(m_worldMatrix);

	const CameraData cameraData{
		.viewproj = getViewProjMatrix(),
		.view = m_viewMatrix,
		.proj = m_projMatrix,
	};

	m_cameraData = _device.createUBO(sizeof(CameraData), &cameraData);
}

Camera::~Camera() {
	_device.destroyBuffer(m_cameraData);
}

void Camera::updateTransform(const Transform& transform) {
	updateTransform(transform.getWorldMatrix());
}

void Camera::updateTransform(const Mat4& transform) {
	if (transform == m_worldMatrix)
		return;

	m_worldMatrix = transform;
	m_viewMatrix = calcViewMatrix(m_worldMatrix);

	const CameraData cameraData{
		.viewproj = getViewProjMatrix(),
		.view = m_viewMatrix,
		.proj = m_projMatrix,
	};

	_device.updateBuffer(m_cameraData, &cameraData);
}

void Camera::updateFov(float fov) {
	if (fov == m_fov)
		return;

	m_fov = fov;
	m_projMatrix = calcProjMatrix(m_aspect, m_fov, m_near, m_far);

	const CameraData cameraData{
		.viewproj = getViewProjMatrix(),
		.view = m_viewMatrix,
		.proj = m_projMatrix,
	};

	_device.updateBuffer(m_cameraData, &cameraData);
}

void Camera::updateAspect(float aspect) {
	if (aspect == m_aspect)
		return;

	m_aspect = aspect;
	m_projMatrix = calcProjMatrix(m_aspect, m_fov, m_near, m_far);

	const CameraData cameraData{
		.viewproj = getViewProjMatrix(),
		.view = m_viewMatrix,
		.proj = m_projMatrix,
	};

	_device.updateBuffer(m_cameraData, &cameraData);
}

void Camera::updateNear(float near) {
	if (near == m_near)
		return;

	m_near = near;
	m_projMatrix = calcProjMatrix(m_aspect, m_fov, m_near, m_far);

	const CameraData cameraData{
		.viewproj = getViewProjMatrix(),
		.view = m_viewMatrix,
		.proj = m_projMatrix,
	};

	_device.updateBuffer(m_cameraData, &cameraData);
}

void Camera::updateFar(float far) {
	if (far == m_far)
		return;

	m_far = far;
	m_projMatrix = calcProjMatrix(m_aspect, m_fov, m_near, m_far);

	const CameraData cameraData{
		.viewproj = getViewProjMatrix(),
		.view = m_viewMatrix,
		.proj = m_projMatrix,
	};

	_device.updateBuffer(m_cameraData, &cameraData);
}
