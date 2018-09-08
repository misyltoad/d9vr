#include "d9vr.h"
#include "d9vr_interfaces.h"
#include "openvr.h"
#include <list>

namespace d9vr
{
	inline vr::EVREye ConvertEyeToSteamVR(Eye nEye)
	{
		if (nEye == Eyes::Left)
			return vr::Eye_Left;
		else
			return vr::Eye_Right;
	}

	inline DeviceClass ConvertDeviceClassToD9VR(vr::ETrackedDeviceClass Class)
	{
		switch (Class)
		{
		default:
		case vr::TrackedDeviceClass_Invalid: return DeviceClasses::Invalid;
		case vr::TrackedDeviceClass_HMD: return DeviceClasses::HMD;
		case vr::TrackedDeviceClass_Controller: return DeviceClasses::Controller;
		case vr::TrackedDeviceClass_GenericTracker: return DeviceClasses::GenericTracker;
		case vr::TrackedDeviceClass_TrackingReference: return DeviceClasses::TrackingReference;
		case vr::TrackedDeviceClass_DisplayRedirect: return DeviceClasses::DisplayRedirect;
		}
	}

	class D9VRHMD_OpenVR : public IHMD
	{
	public:
		D9VRHMD_OpenVR(vr::IVRSystem* pHMD, DeviceId Id)
		{
			m_zNear = 0.1f;
			m_zFar = 1024.0f;
			m_pHMD = pHMD;
			m_Id = Id;

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
			*pOutMatrix = m_Pose.PoseMatrix;
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
			vr::HmdMatrix34_t matrix = m_pHMD->GetEyeToHeadTransform(ConvertEyeToSteamVR(nEye));
			ConvertFromSteamVRMatrix(&matrix, pOutMatrix);
			GetInternalInterface()->ConvertMatrix(pOutMatrix);

			// We should do this eventually.
			//if (UserMatrixInv)
			//UserMatrixInv(pOutMatrix);
		}

		DeviceClass GetClass() override
		{
			return DeviceClass::HMD;
		}

		Pose* GetPose() override
		{
			return &m_Pose;
		}

		DeviceId GetDeviceId() override
		{
			return m_Id;
		}

	protected:
		friend class D9VRInterface_OpenVR;

		void PollEvents()
		{
			auto leftProj = m_pHMD->GetProjectionMatrix(vr::Eye_Left, m_zNear, m_zFar);
			auto rightProj = m_pHMD->GetProjectionMatrix(vr::Eye_Right, m_zNear, m_zFar);
			ConvertFromSteamVRMatrix(&leftProj, &m_LeftEyeProjection);
			ConvertFromSteamVRMatrix(&rightProj, &m_RightEyeProjection);

			vr::VREvent_t event;
			while (m_pHMD->PollNextEvent(&event, sizeof(event)))
				ProcessVREvent(event);

			// New VR Input is a pain... Just gonna use this for now.
			/*for (vr::TrackedDeviceIndex_t unDevice = 0; unDevice < vr::k_unMaxTrackedDeviceCount; unDevice++)
			{
				vr::VRControllerState_t state;
				if (m_pHMD->GetControllerState(unDevice, &state, sizeof(state)))
				{
					state.rAxis
				}
			}*/

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
			SubmitGeneral();
		}

		void QueueTextureCreationForEye(Eye nEye)
		{
			NextRTEye = nEye;
		}

		DeviceId m_Id;

		Pose m_Pose;

		vr::IVRSystem* m_pHMD;

		std::string m_Driver;

		std::string m_Display;
		Matrix m_LeftEyeProjection;
		Matrix m_RightEyeProjection;

		float m_zNear, m_zFar;

		float m_IPD;

		Size m_RenderSize;
	};

	D9VRHMD_OpenVR* AsHMD(IGenericDevice* pDevice)
	{
		if (pDevice && pDevice->GetClass() == DeviceClasses::HMD)
			return static_cast<D9VRHMD_OpenVR*>(pDevice);

		return nullptr;
	}

	class D9VRInterface_OpenVR : public BaseD9VRInterface
	{
	public:
		D9VRInterface_OpenVR()
			: m_Initted{ false }
			, m_pVRSystem{ nullptr }
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

			m_pVRSystem = pHMD;

			D9VRHMD_OpenVR* pHMDWrapper = new D9VRHMD_OpenVR(pHMD, 0);
			m_Devices.push_back(pHMDWrapper);
		}

		bool IsVR() override
		{
			return m_Initted && !m_Devices.empty();
		}

		const std::list<IGenericDevice*>& GetDevices() override
		{
			return m_Devices;
		}

		void PollEvents() override
		{
			TryInit();

			if (!m_pVRSystem)
				return;

			vr::TrackedDevicePose_t SteamPoses[vr::k_unMaxTrackedDeviceCount];

			vr::VRCompositor()->WaitGetPoses(SteamPoses, vr::k_unMaxTrackedDeviceCount, NULL, 0);

			uint32_t ValidPoses = 0;
			for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
			{
				if (SteamPoses[i].bPoseIsValid)
					ValidPoses++;
			}


			for (uint32_t i = 0; i < ValidPoses; i++)
			{
				DeviceClass Class = ConvertDeviceClassToD9VR(m_pVRSystem->GetTrackedDeviceClass(i));
				IGenericDevice* pGenericDevice = FindOrCreateDevice(Class, i);

				// Fix me to be nicer
				D9VRHMD_OpenVR* pHMD = AsHMD(pGenericDevice);
				if (pHMD)
					ConvertSteamVRPose(&SteamPoses[i], &pHMD->m_Pose);
			}

			std::list<IGenericDevice*>::iterator iter;
			for (iter = m_Devices.begin(); iter != m_Devices.end(); iter++)
			{
				D9VRHMD_OpenVR* pHMD = AsHMD(*iter);

				if (pHMD)
					pHMD->PollEvents();
			}
		}

		IHMD* GetPrimaryHMD() override
		{
			return GetHMD(0);
		}

		IHMD* GetHMD(uint32_t Index) override
		{
			return static_cast<IHMD*>(FindDevice(DeviceClasses::HMD, Index));
		}
		IController* GetController(uint32_t Index) override
		{
			return static_cast<IController*>(FindDevice(DeviceClasses::Controller, Index));
		}

		IGenericDevice* FindDevice(DeviceId Id) override
		{
			std::list<IGenericDevice*>::iterator iter;
			for (iter = m_Devices.begin(); iter != m_Devices.end(); iter++)
			{
				if ((*iter)->GetDeviceId() == Id)
					return *iter;
			}

			return nullptr;
		}
	private:
		IGenericDevice* FindDevice(DeviceClass Class, uint32_t Index)
		{
			uint32_t i = 0;
			std::list<IGenericDevice*>::iterator iter;
			for (iter = m_Devices.begin(); iter != m_Devices.end(); iter++)
			{
				if ((*iter)->GetClass() == Class)
				{
					i++;
					if (Index + 1 == i)
						return (*iter);
				}
			}

			return nullptr;
		}

		IGenericDevice* CreateDevice(DeviceClass Class, DeviceId Id)
		{
			IGenericDevice* pDevice;
			switch (Class)
			{
				case DeviceClasses::HMD: pDevice = new D9VRHMD_OpenVR(m_pVRSystem, Id);
				//case DeviceClasses::Controller: m_Devices.push_back
				//case DeviceClasses::GenericTracker: m_Devices.push_back
				//case DeviceClasses::TrackingReference: m_Devices.push_back
				default: pDevice = nullptr;
			}

			return pDevice;
		}

		void RemoveDevice(DeviceId Id)
		{
			std::list<IGenericDevice*>::iterator iter;
			for (iter = m_Devices.begin(); iter != m_Devices.end();)
			{
				if ((*iter)->GetDeviceId() == Id)
				{
					delete *iter;
					iter = m_Devices.erase(iter);
				}
				else
					++iter;
			}
		}

		IGenericDevice* FindOrCreateDevice(DeviceClass Class, DeviceId Id)
		{
			IGenericDevice* pFoundDevice = FindDevice(Id);

			if (!pFoundDevice)
				pFoundDevice = CreateDevice(Class, Id);

			if (pFoundDevice->GetClass() != Class)
			{
				RemoveDevice(Id);
				pFoundDevice = CreateDevice(Class, Id);
			}

			return pFoundDevice;
		}

		bool m_Initted;
		vr::IVRRenderModels* m_pRenderModels;
		vr::IVRSystem* m_pVRSystem;
		std::list<IGenericDevice*> m_Devices;
	};

	D9VRInterface_OpenVR* CreateOpenVRInterface()
	{
		return new D9VRInterface_OpenVR;
	}
}