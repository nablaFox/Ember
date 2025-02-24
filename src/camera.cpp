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

Mat4 Camera::getViewMatrix() {
	Mat4 viewTranslation = WorldTransform::getTransMatrix(transform.position * -1);

	// CHECK shouldn't be transposed?
	Mat4 viewRotation = transform.getRotMatrix();

	return viewRotation * viewTranslation;
}

Mat4 Camera::getProjMatrix() {
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
