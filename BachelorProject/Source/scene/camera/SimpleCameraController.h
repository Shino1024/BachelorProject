#pragma once
#include <inputDispatcher/InputDispatcher.h>
#include <glm/glm.hpp>
#include "Camera.h"
#include "CameraController.h"


class SimpleCameraController
	: public MousePositionListener
	, public MouseScrollListener 
	, public KeyPressListener
	, public CameraController
	, public KeyInputListener
{
public:
	SimpleCameraController(InputDispatcher& inputDispatcher, ViewPort & viewPort, const glm::vec3 & position);

	void handleMouseMove(float x, float y) override;
	void handleMouseScroll(float scroll) override;
	void handleKeyPress(int key, KeyState action, float deltaTime) override;

	const std::vector<const Scene::Camera*> provideCameras() const override;

private:
	inline static const double MOUSE_SENSITIVITY = 0.1;
	inline static const float VELOCITY = 20.0f;

	Scene::Camera camera;
};