#include "camera.hpp"

void Camera::rotate(Rot rotation) {
	transform.rotation.yaw += rotation.yaw;
	transform.rotation.pitch += rotation.pitch;

	if (transform.rotation.pitch > M_PI / 2) {
		transform.rotation.pitch = M_PI / 2;
	} else if (transform.rotation.pitch < -M_PI / 2) {
		transform.rotation.pitch = -M_PI / 2;
	}
}

void Camera::translate(Vec3 translation) {
	Rot rot = transform.rotation;

	Vec3 forward = {
		sinf(-rot.yaw) * cosf(rot.pitch),
		sinf(rot.pitch),
		cosf(-rot.yaw) * cosf(rot.pitch),
	};

	Vec3 right = forward.cross({0, 1, 0}).normalize();
	right *= -1;

	Vec3 up = right.cross(forward).normalize();
	up *= -1;

	transform.translation +=
		forward * translation[0] + right * translation[1] + up * translation[2];
}

Mat4 Camera::getViewMatrix() {
	Mat4 viewTranslation = {
		{1, 0, 0, -transform.translation[0]},
		{0, 1, 0, -transform.translation[1]},
		{0, 0, 1, -transform.translation[2]},
		{0, 0, 0, 1},
	};

	Mat4 viewRotation = transform.getRotMatrix().transpose();

	return viewRotation.transpose() * viewTranslation;
}

Mat4 Camera::getProjMatrix() {
	const float near = 1.f;
	const float far = 1e2f;

	const float fovAngle = fov * M_PI / 180;

	const float top = near * tanf(fovAngle / 2);

	const float right = top * aspect;

	return {
		{near / right, 0, 0, 0},
		{0, -near / top, 0, 0},
		{0, 0, far / (near - far), near * far / (near - far)},
		{0, 0, -1, 0},
	};
}
