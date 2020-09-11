#include "Camera.hpp"

void Camera::update() {
	viewMatrix = glm::lookAt(position, lookPoint, up);
}

void Camera::updateOrbit() {
	position.x = orbitRadius * cos(lookAngle.x * (pi / 180.0f)) * sin(lookAngle.y * (pi / 180.0f)) + lookPoint.x;
	position.y = orbitRadius * cos(lookAngle.y * (pi / 180.0f)) + lookPoint.y;
	position.z = orbitRadius * sin(lookAngle.x * (pi / 180.0f)) * sin(lookAngle.y * (pi / 180.0f)) + lookPoint.z;
	viewMatrix = glm::lookAt(position, lookPoint, up);
}

void Camera::orthographic(int w, int h) {
	GLdouble aspect = (GLdouble)w / (GLdouble)h;
	if (w >= h) {
		// Aspect >= 1, set width larger
		projectionMatrix = glm::ortho(-orthoScale * aspect, orthoScale * aspect,
			-orthoScale, orthoScale, 0.0, 2.0);
	} else {
		// Aspect < 1, set height larger
		projectionMatrix = glm::ortho(-orthoScale, orthoScale,
			-orthoScale / aspect, orthoScale / aspect, 0.0, 100.0);
	}
}

void Camera::perspective(int w, int h) {
	GLdouble aspect = (GLdouble)w / (GLdouble)h;
	projectionMatrix = glm::perspective(65.0, aspect, 0.01, 100.0);
}