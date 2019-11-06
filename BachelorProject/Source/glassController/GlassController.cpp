#include "GlassController.h"

GlassController::GlassController(InputDispatcher& inputDispatcher, const Scene::Camera& camera, const ShaderProgram& shaderProgram, const ShaderProgram& selectedProgram)
	: camera{ camera }
	, shaderProgram{ shaderProgram }
	, selectedProgram{ selectedProgram }
{
	inputDispatcher.subscribeForKeyInput(this, std::vector<int>{
		GLFW_KEY_5,
		GLFW_KEY_6,
		GLFW_KEY_7,
		GLFW_KEY_8,
		GLFW_KEY_3,
		GLFW_MOUSE_BUTTON_LEFT,
	});
}

void GlassController::assignUntrackedObjects(Scene::Scene& scene)
{
	if(trackedObjects < ParticleObjectManager::m_numOfObjects)
		for (int i = trackedObjects; i < ParticleObjectManager::m_numOfObjects; i++) {
			GlassObject* object = new GlassObject{ shaderProgram, selectedProgram, *ParticleObjectManager::m_partObjectsVector[i] };
			glassObjects.push_back(object);
			scene.addMaterialObject(object);
			trackedObjects++;
		}
}

void GlassController::handleKeyPress(int key, KeyState action, float deltaTime)
{
	if (action == FALLING_EDGE) {
		switch (key) {
		case GLFW_MOUSE_BUTTON_LEFT:
			selectGlass();
			break;
		case GLFW_KEY_3:
			for (GlassObject* glassObject : glassObjects)
				glassObject->toggleRender();
			break;
		default:
			break;
		}
	}
	else if (action == PRESSED) {
		switch (key)
		{
		case GLFW_KEY_5:
			if (currentlySelected != -1)
				ParticleObjectManager::moveObject(currentlySelected, VELOCITY * deltaTime);
			break;
		case GLFW_KEY_6:
			if (currentlySelected != -1)
				ParticleObjectManager::moveObject(currentlySelected, -VELOCITY * deltaTime);
			break;
		case GLFW_KEY_7:
			if (currentlySelected != -1)
				ParticleObjectManager::m_partObjectsVector[currentlySelected]->m_destinationMatrix = glm::rotate(ParticleObjectManager::m_partObjectsVector[currentlySelected]->m_destinationMatrix, 0.5f * deltaTime, glm::vec3(1, 0, 0));
			break;
		case GLFW_KEY_8:
			if (currentlySelected != -1)
				ParticleObjectManager::m_partObjectsVector[currentlySelected]->m_destinationMatrix = glm::rotate(ParticleObjectManager::m_partObjectsVector[currentlySelected]->m_destinationMatrix, -0.5f * deltaTime, glm::vec3(1, 0, 0));
			break;
		default:
			break;
		}
	}
}

void GlassController::selectGlass() {
	float distanceFromCamera = -1.0f;
	int selectedParticleObjectIndex = -1;
	for (int i = 0; i < ParticleObjectManager::m_numOfObjects; i++) {
		const ParticleObject* currentParticleObject = ParticleObjectManager::m_partObjectsVector[i].get();
		glm::vec3 centerAsSeenFromCamera = camera.getViewMatrix() * glm::vec4(currentParticleObject->m_center, 1.0f);
		float distanceFromZAxis = glm::length(glm::vec2{ centerAsSeenFromCamera.x, centerAsSeenFromCamera.y });
		if (distanceFromZAxis < currentParticleObject->m_innerRadius)
			if (glm::length(centerAsSeenFromCamera) < distanceFromCamera || distanceFromCamera < 0.0f)
				selectedParticleObjectIndex = i;
	}

	if (selectedParticleObjectIndex >= 0) {
		if (currentlySelected == selectedParticleObjectIndex) {
			glassObjects[selectedParticleObjectIndex]->select(false);
			currentlySelected = -1;
		}
		else {
			glassObjects[selectedParticleObjectIndex]->select(true);

			if (currentlySelected != -1) {
				glassObjects[currentlySelected]->select(false);
			}
			currentlySelected = selectedParticleObjectIndex;
		}
	}
	else if (currentlySelected != -1) {
		glassObjects[currentlySelected]->select(false);
		currentlySelected = -1;
	}
}
