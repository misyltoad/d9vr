#pragma once
#include "d9vr.h"
#include "d9vr_interfaces.h"
#include "openvr.h"

namespace d9vr
{
	class OpenVRController : public IController
	{
	public:
		OpenVRController(vr::IVRSystem* pVRSystem, DeviceId Id);

		bool GetAxis(uint32_t AxisIndex, ControllerAxis* pOutAxis) override;
		bool GetButton(Button ButtonIndex) override;
		Hand GetHand() override;

		DeviceClass GetClass() override;
		Pose* GetPose() override;
		DeviceId GetDeviceId() override;

		void UpdateHand();

		void PollEvents();
	private:
		friend class D9VRInterface_OpenVR;

		DeviceId m_Id;
		Pose m_Pose;

		static const uint8_t ControllerAxisCount = 5;
		ControllerAxis m_Axes[ControllerAxisCount];
		vr::VRControllerState_t m_State;
		Hand m_Hand;

		vr::IVRSystem* m_pVRSystem;
	};

	inline OpenVRController* AsController(IGenericDevice* pDevice)
	{
		if (pDevice && pDevice->GetClass() == DeviceClasses::Controller)
			return static_cast<OpenVRController*>(pDevice);

		return nullptr;
	}
}