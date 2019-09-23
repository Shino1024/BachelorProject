#include <filesystem>
#include <array>

#include "InputConfig.h"

#include "VRInput.h"

namespace VR {
	bool VRInput::InitializeVRInput(std::string PathToActions) {
		//

		return true;
	}
	
	std::vector<std::string> VRInput::DetectPressedButtons() {
		std::vector<std::string> PressedButtonsVector;

		for (auto &ActionName : ActionNameArray) {
			auto Action = this->ActionLoader->GetActionHandle(ActionName);
			if (!this->GetDigitalActionState(Action)) {
				PressedButtonsVector.push_back(ActionName);
			}
		}

		return PressedButtonsVector;
	}

	bool VRInput::GetDigitalActionRisingEdge(vr::VRActionHandle_t action, vr::VRInputValueHandle_t* pDevicePath) {
		vr::InputDigitalActionData_t actionData;
		vr::VRInput()->GetDigitalActionData(action, &actionData, sizeof(actionData), vr::k_ulInvalidInputValueHandle);
		if (pDevicePath) {
			*pDevicePath = vr::k_ulInvalidInputValueHandle;
			if (actionData.bActive) {
				vr::InputOriginInfo_t originInfo;
				if (vr::VRInputError_None == vr::VRInput()->GetOriginTrackedDeviceInfo(actionData.activeOrigin, &originInfo, sizeof(originInfo))) {
					*pDevicePath = originInfo.devicePath;
				}
			}
		}

		return actionData.bActive && actionData.bChanged && actionData.bState;
	}

	bool VRInput::GetDigitalActionState(vr::VRActionHandle_t action, vr::VRInputValueHandle_t* pDevicePath) {
		vr::InputDigitalActionData_t actionData;
		vr::VRInput()->GetDigitalActionData(action, &actionData, sizeof(actionData), vr::k_ulInvalidInputValueHandle);
		if (pDevicePath) {
			*pDevicePath = vr::k_ulInvalidInputValueHandle;
			if (actionData.bActive) {
				vr::InputOriginInfo_t originInfo;
				if (vr::VRInputError_None == vr::VRInput()->GetOriginTrackedDeviceInfo(actionData.activeOrigin, &originInfo, sizeof(originInfo))) {
					*pDevicePath = originInfo.devicePath;
				}
			}
		}

		return actionData.bActive && actionData.bState;
	}

	bool VRInput::HandleInput() {
		vr::VRActiveActionSet_t ActionSet = { 0 };
		ActionSet.ulActionSet = this->ActionLoader->GetActionSet();
		vr::VRInput()->UpdateActionState(&ActionSet, sizeof(ActionSet), 1);
		auto DetectedPressedButtons = this->DetectPressedButtons();
		if (vr::VRInput()->GetAnalogActionData(this->ActionLoader->GetActionHandle("trackpad"), &this->AnalogData, sizeof(this->AnalogData), vr::k_ulInvalidInputValueHandle) == vr::VRInputError_None && this->AnalogData.bActive)
		{
			this->AnalogDataArray[0] = AnalogData.x;
			this->AnalogDataArray[1] = AnalogData.y;
		}

		return true;
	}

	ActionLoader::ActionLoader(std::string PathToActionsFile) {
		this->PathToActionsFile = PathToActionsFile;
		vr::VRInput()->SetActionManifestPath(std::filesystem::absolute(this->PathToActionsFile.c_str()).string().c_str());
		InitActionsMap();
	}

	bool ActionLoader::InitActionsMap() {
		std::string ActionsPrefix = std::string(ACTIONS_PREFIX) + "in/";
		for (std::string ActionName : ActionNameArray) {
			vr::VRActionHandle_t ActionHandle;
			vr::VRInput()->GetActionHandle((ActionsPrefix + ActionName).c_str(), &ActionHandle);
			this->ActionMap.insert(std::pair<std::string, vr::VRActionHandle_t>(ActionName, std::move(ActionHandle)));
		}

		return true;
	}

	vr::VRActionSetHandle_t ActionLoader::GetActionSet() {
		return this->ActionSet;
	}

	vr::VRActionHandle_t ActionLoader::GetActionHandle(std::string Name) {
		return this->ActionMap[Name];
	}
}
