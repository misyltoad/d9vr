#include "d9vr.h"
#include "d9vr_interfaces.h"
#include "d9vr_openvr_hmd.h"
#include "d9vr_openvr_controller.h"
#include "d9vr_openvr_util.h"
#include "openvr.h"
#include <list>

namespace d9vr
{
	class D9VRInterface_OpenVR : public BaseD9VRInterface
	{
	public:
		D9VRInterface_OpenVR()
			: m_Initted{ false }
			, m_pVRSystem{ nullptr }
			, m_Devices{}
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

			OpenVRHMD* pHMDWrapper = new OpenVRHMD(pHMD, 0);
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

		void ProcessVREvent(const vr::VREvent_t & event)
		{
			switch (event.eventType)
			{
			case vr::VREvent_TrackedDeviceDeactivated:
			{
				RemoveDevice(event.trackedDeviceIndex);
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

		void PollEvents() override
		{
			TryInit();

			if (!m_pVRSystem)
				return;

			vr::VREvent_t event;
			while (m_pVRSystem->PollNextEvent(&event, sizeof(event)))
				ProcessVREvent(event);

			vr::TrackedDevicePose_t SteamPoses[vr::k_unMaxTrackedDeviceCount];

			vr::VRCompositor()->WaitGetPoses(SteamPoses, vr::k_unMaxTrackedDeviceCount, NULL, 0);

			for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
			{
				if (!SteamPoses[i].bPoseIsValid)
					continue;

				DeviceClass Class = ConvertDeviceClassToD9VR(m_pVRSystem->GetTrackedDeviceClass(i));
				IGenericDevice* pGenericDevice = FindOrCreateDevice(Class, i);

				// Fix me to be nicer
				OpenVRHMD* pHMD = AsHMD(pGenericDevice);
				OpenVRController* pController = AsController(pGenericDevice);

				if (pHMD)
					ConvertSteamVRPose(&SteamPoses[i], &pHMD->m_Pose);

				if (pController)
					ConvertSteamVRPose(&SteamPoses[i], &pController->m_Pose);
			}

			std::list<IGenericDevice*>::iterator iter;
			for (iter = m_Devices.begin(); iter != m_Devices.end(); iter++)
			{
				OpenVRHMD* pHMD = AsHMD(*iter);
				OpenVRController* pController = AsController(*iter);

				if (pHMD)
					pHMD->PollEvents();

				if (pController)
					pController->PollEvents();
			}
		}

		IHMD* GetPrimaryHMD() override
		{
			return GetHMD(0);
		}

		IHMD* GetHMD(uint32_t Index) override
		{
			return static_cast<IHMD*>(FindDeviceByType(DeviceClasses::HMD, Index));
		}

		IController* GetControllerByHand(Hand eHand) override
		{
			std::list<IGenericDevice*>::iterator iter;
			for (iter = m_Devices.begin(); iter != m_Devices.end(); iter++)
			{
				if ((*iter)->GetClass() == DeviceClasses::Controller)
				{
					IController* pController = static_cast<IController*>(*iter);
					if (pController->GetHand() == eHand)
						return pController;
				}
			}

			return nullptr;
		}

		IController* GetControllerByIndex(uint32_t Index) override
		{
			return static_cast<IController*>(FindDeviceByType(DeviceClasses::Controller, Index));
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
		IGenericDevice* FindDeviceByType(DeviceClass Class, uint32_t Index)
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
				case DeviceClasses::HMD: pDevice = new OpenVRHMD(m_pVRSystem, Id); break;
				case DeviceClasses::Controller: pDevice = new OpenVRController(m_pVRSystem, Id); break;

				case DeviceClasses::GenericTracker:
				case DeviceClasses::TrackingReference:
				default: pDevice = nullptr;
			}

			if (pDevice)
				m_Devices.push_back(pDevice);

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
			// Quick out for the unimplemented classes
			if (Class != DeviceClasses::HMD && Class != DeviceClasses::Controller)
				return nullptr;

			IGenericDevice* pFoundDevice = FindDevice(Id);

			if (!pFoundDevice)
				pFoundDevice = CreateDevice(Class, Id);

			if (pFoundDevice && pFoundDevice->GetClass() != Class)
			{
				Msg("Removing and recreating device %d as its a bad type.", Id);
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