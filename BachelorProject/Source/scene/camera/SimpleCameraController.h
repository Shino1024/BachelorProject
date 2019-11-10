#pragma once
#include <inputDispatcher/InputDispatcher.h>
#include <glm/glm.hpp>
#include "Camera.h"
#include "CameraController.h"
#include <simulationObjects/EmiterProvider.h>
#include <simulationObjects/EmiterManager.h>


class SimpleCameraController
	: public MousePositionListener
	, public MouseScrollListener
	, public CameraController
	, public KeyInputListener
{
public:
	SimpleCameraController(InputDispatcher& inputDispatcher, ViewPort & viewPort, const glm::vec3 & position);

	void handleMouseMove(float x, float y) override;
	void handleMouseScroll(float scroll) override;
	void handleKeyPress(int key, KeyState action, float deltaTime) override;

	const std::vector<const Scene::Camera*> provideCameras() const override;

	Emiter* emiter;
	glm::mat4 getEmiterMatrix() const override;
private:
	inline static const double MOUSE_SENSITIVITY = 0.1;
	inline static const float VELOCITY = 200.0f;

	Scene::Camera camera;
};