#include "camera.h"
#include <algorithm>

Camera::Camera(glm::vec3 startPos, const int WINH, const int WINW)
	: H(WINH), W(WINW)
{
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
}

glm::mat4 Camera::ViewMat()
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}
glm::mat4 Camera::ProjectionMat()
{
	return glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
}

void Camera::ChangeFov(const int y, bool constraint)
{
	fov -= y;
	if (constraint)
		fov = std::clamp(fov, 10.0f, 45.0f);
}

void Camera::MoveCamera(const bool w, const bool s, const bool a, const bool d, const float deltaTime)
{
	const float cameraSpeed = deltaTime * MOVEMENTSPEED;
	if (w)
		cameraPos += cameraSpeed * cameraFront;
	if (s)
		cameraPos -= cameraSpeed * cameraFront;
	if (a)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (d)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void Camera::RotateCamera(const int x, const int y)
{
	yaw += x * MOUSESENSITIVITY;
	pitch += y * -MOUSESENSITIVITY;
	pitch = std::clamp(pitch, -89.0f, 89.0f);
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}