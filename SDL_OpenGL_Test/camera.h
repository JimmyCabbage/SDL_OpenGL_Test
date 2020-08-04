#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
	float fov = 45.0f;

	//camera
	glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);
	glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

	const int H, W;

	const float MOUSESENSITIVITY = 0.1f, MOVEMENTSPEED = 2.5f;

	float yaw = -90.0f, pitch = 0.0f;

	int x, y;
	Camera(glm::vec3 startPos, const int WINH, const int WINW);

	void ChangeFov(const int y, bool constraint = true);

	void MoveCamera(const bool w, const bool s, const bool a, const bool d, const float deltaTime);

	void RotateCamera(const int x, const int y);

	glm::mat4 ViewMat();
	glm::mat4 ProjectionMat();

	glm::vec3 direction;
};

#endif