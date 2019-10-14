#pragma once

#include <GL/glew.h>
#include <memory>
#include <VR/InputConfig.h>
#include <VR/VRGeometry.h>
#include <VR/VRInput.h>
#include <VR/VRCore.h>
#include <scene/camera/VRCameraController.h>

namespace VR {
	class VRGLInterop {
	public:

		std::unique_ptr<VR::VRCore> VrCore = std::make_unique<VR::VRCore>();
		std::unique_ptr<VR::VRGeometry> VrGeometry = std::make_unique<VR::VRGeometry>();
		std::unique_ptr<VR::VRInput> VrInput = std::make_unique<VR::VRInput>();

		bool VRactive;

		const unsigned int SCR_WIDTH = 1600;
		const unsigned int SCR_HEIGHT = 900;
		GLuint m_nResolveFramebufferIdLeft;
		GLuint m_nRenderFramebufferIdLeft;
		GLuint m_nDepthBufferIdLeft;
		GLuint m_nRenderTextureIdLeft;
		GLuint m_nResolveTextureIdLeft;

		GLuint m_nResolveFramebufferIdRight;
		GLuint m_nRenderFramebufferIdRight;
		GLuint m_nDepthBufferIdRight;
		GLuint m_nRenderTextureIdRight;
		GLuint m_nResolveTextureIdRight;

		bool activate();
		bool hasVR();
		void phase1();
		void phase2(VRCameraController* cameraControler);

	protected:
		//

	private:
		//
	};
}
