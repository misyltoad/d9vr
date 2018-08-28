#include "d9vr.h"
#include "d9vr_interfaces.h"
#include "openvr.h"
#include <vector>

namespace d9vr
{
	inline vr::EVREye ConvertEye(Eye nEye)
	{
		if (nEye == Eyes::Left)
			return vr::Eye_Left;
		else
			return vr::Eye_Right;
	}

	class D9VRHMD_OpenVR : public ID9VRHMD
	{
	public:
		D9VRHMD_OpenVR(D9VRInterface_OpenVR* pParent, vr::IVRSystem* pHMD)
			: m_Poses(vr::k_unMaxTrackedDeviceCount)
			, m_pParent(pParent)
		{
			m_zNear = 0.1f;
			m_zFar = 1024.0f;
			m_pHMD = pHMD;

			UpdateIPD();

			m_pHMD->GetRecommendedRenderTargetSize(&m_RenderSize.Data[0], &m_RenderSize.Data[1]);

			vr::VRCompositor();
		}

		float GetIPD() override
		{
			return m_IPD;
		}

		void GetMidEyePose(Matrix* pOutMatrix) override
		{
			*pOutMatrix = m_MidEyePose;
		}

		void GetEyeRTSize(Size* pOutSize) override
		{
			pOutSize->Data[0] = m_RenderSize.Data[0];
			pOutSize->Data[1] = m_RenderSize.Data[1];
		}
		void GetStereoRTSize(Size* pOutSize) override
		{
			pOutSize->Data[0] = m_RenderSize.Data[0] * 2;
			pOutSize->Data[1] = m_RenderSize.Data[1];
		}

		void GetProjectionMatrix(Eye nEye, Matrix* pOutMatrix) override
		{
			*pOutMatrix = nEye == Eyes::Left ? m_LeftEyeProjection : m_RightEyeProjection;
		}

		void SetZRange(float zNear, float zFar) override
		{
			m_zNear = zNear;
			m_zFar = zFar;
		}

		void GetMidToEyePose(Eye nEye, Matrix* pOutMatrix) override
		{
			vr::HmdMatrix34_t matrix = m_pHMD->GetEyeToHeadTransform(ConvertEye(nEye));
			ConvertMatrix(&matrix, pOutMatrix);

			GetInternalInterface()->ConvertMatrix(pOutMatrix);

			// We should do this eventually.
			//if (UserMatrixInv)
			//UserMatrixInv(pOutMatrix);
		}

	protected:
		friend class D9VRInterface_OpenVR;

		void PollEvents()
		{
			float fSecondsSinceLastVsync;
			m_pHMD->GetTimeSinceLastVsync(&fSecondsSinceLastVsync, NULL);

			float fDisplayFrequency = m_pHMD->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_DisplayFrequency_Float);
			float fFrameDuration = 1.f / fDisplayFrequency;
			float fVsyncToPhotons = m_pHMD->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SecondsFromVsyncToPhotons_Float);

			float fPredictedSecondsFromNow = 0.0f; //fFrameDuration - fSecondsSinceLastVsync + fVsyncToPhotons;

			vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount];
			m_pHMD->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, fPredictedSecondsFromNow, poses, vr::k_unMaxTrackedDeviceCount);

			uint32_t validPoses = 0;
			for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
			{
				if (poses[i].bPoseIsValid)
					validPoses++;
			}
			m_Poses.resize(validPoses);

			for (uint32_t i = 0; i < validPoses; i++)
			{
				Matrix absTracking;
				ConvertMatrix(&poses[i].mDeviceToAbsoluteTracking, &absTracking);

				GetInternalInterface()->ConvertMatrix(&absTracking);

				m_Poses[i].PoseMatrix = absTracking;

				switch (m_pHMD->GetTrackedDeviceClass(i))
				{
					case vr::TrackedDeviceClass_Controller:        m_Poses[i].Class = DeviceClasses::Controller; break;
					case vr::TrackedDeviceClass_HMD:
					{
						m_MidEyePose = absTracking;

						m_Poses[i].Class = DeviceClasses::HMD; break;
					}
					case vr::TrackedDeviceClass_GenericTracker:    m_Poses[i].Class = DeviceClasses::GenericTracker; break;
					case vr::TrackedDeviceClass_TrackingReference: m_Poses[i].Class = DeviceClasses::TrackingReference; break;
					case vr::TrackedDeviceClass_Invalid:
					default:                                       m_Poses[i].Class = DeviceClasses::Invalid; break;
				}
			}

			auto leftProj = m_pHMD->GetProjectionMatrix(vr::Eye_Left, m_zNear, m_zFar);
			auto rightProj = m_pHMD->GetProjectionMatrix(vr::Eye_Right, m_zNear, m_zFar);
			ConvertMatrix(&leftProj, &m_LeftEyeProjection);
			ConvertMatrix(&rightProj, &m_RightEyeProjection);

			vr::VREvent_t event;
			while (m_pHMD->PollNextEvent(&event, sizeof(event)))
				ProcessVREvent(event);

			UpdateIPD();
		}
	private:
		void ProcessVREvent(const vr::VREvent_t & event)
		{
			switch (event.eventType)
			{
				case vr::VREvent_TrackedDeviceDeactivated:
				{
					Msg("ProcessVREvent: Device %u detatched.", event.trackedDeviceIndex);
				}
				break;
				case vr::VREvent_TrackedDeviceUpdated:
				{
					Msg("ProcessVREvent: Device %u updated.", event.trackedDeviceIndex);
				}
				break;
			}
		}

		bool UpdateIPD()
		{
			vr::TrackedPropertyError trackError = vr::TrackedProp_Success;
			m_IPD = m_pHMD->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_UserIpdMeters_Float, &trackError);

			return trackError == vr::TrackedProp_Success;
		}

		void UpdateTrackingProperties()
		{
			if (m_Driver.empty() || m_Display.empty())
			{
				vr::TrackedPropertyError trackError = vr::TrackedProp_Success;

				m_Driver = GetTrackedDeviceString(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String, &trackError);
				m_Display = GetTrackedDeviceString(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String, &trackError);

				Msg("Driver: %s.", m_Driver.c_str());
				Msg("Display: %s.", m_Display.c_str());
			}
		}

		std::string GetTrackedDeviceString(vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError)
		{
			uint32_t bufferLength = m_pHMD->GetStringTrackedDeviceProperty(unDevice, prop, nullptr, 0, peError);
			if (bufferLength == 0)
				return "";

			char* buffer = new char[bufferLength];

			m_pHMD->GetStringTrackedDeviceProperty(unDevice, prop, buffer, bufferLength, peError);
			std::string str = buffer;
			delete buffer;
			return str;
		}

		void Submit()
		{
			//vr::Texture_t EyeTexture[2];
			//EyeTexture[0].handle
			//vr::VRCompositor()->Submit()
		}

		vr::IVRSystem* m_pHMD;

		std::string m_Driver;

		std::string m_Display;
		Matrix m_LeftEyeProjection;
		Matrix m_RightEyeProjection;

		Matrix m_MidEyePose;

		float m_zNear, m_zFar;

		float m_IPD;

		Size m_RenderSize;

		std::vector<Pose> m_Poses;

		D9VRInterface_OpenVR* m_pParent;
	};

	class D9VRInterface_OpenVR : public BaseD9VRInterface
	{
	public:
		D9VRInterface_OpenVR()
			: m_Initted{ false }
		{
			TryInit();
		}

		void TryInit()
		{
			if (m_Initted)
				return;

			if (!vr::VR_IsHmdPresent())
				return;

			vr::EVRInitError error = vr::VRInitError_None;

			vr::IVRSystem* pHMD = vr::VR_Init(&error, vr::VRApplication_Scene);
			if (error != vr::VRInitError_None || !pHMD)
				return;

			m_pRenderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &error);
			if (error != vr::VRInitError_None || !m_pRenderModels)
				return;

			m_Initted = true;

			D9VRHMD_OpenVR* pHMDWrapper = new D9VRHMD_OpenVR(this, pHMD);
			m_HMDs.push_back(pHMDWrapper);
		}

		bool IsVR() override
		{
			return !m_HMDs.empty() && m_Initted;
		}

		ID9VRHMD* GetHMD(uint32_t Index) override
		{
			if (Index >= m_HMDs.size())
				return nullptr;

			return m_HMDs[Index];
		}
		uint32_t GetHMDCount() override
		{
			return m_HMDs.size();
		}

		void PollEvents() override
		{
			TryInit();

			for (D9VRHMD_OpenVR* pHMD : m_HMDs)
				pHMD->PollEvents();
		}
	private:
		bool m_Initted;
		vr::IVRRenderModels* m_pRenderModels;
		std::vector<D9VRHMD_OpenVR*> m_HMDs;
	};

	D9VRInterface_OpenVR* CreateOpenVRInterface()
	{
		return new D9VRInterface_OpenVR;
	}
}