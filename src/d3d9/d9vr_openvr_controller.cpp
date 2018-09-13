#include "d9vr_openvr_controller.h"
#include "openvr.h"

namespace d9vr
{
	OpenVRController::OpenVRController(vr::IVRSystem* pVRSystem, DeviceId Id)
	{
		m_pVRSystem = pVRSystem;
		m_Id = Id;
		UpdateHand();
	}

	bool OpenVRController::GetButton(Button ButtonIndex)
	{
		if (m_State.ulButtonPressed & vr::ButtonMaskFromId((vr::EVRButtonId)ButtonIndex))
			return true;

		return false;
	}

	Hand OpenVRController::GetHand()
	{
		return m_Hand;
	}

	bool OpenVRController::GetAxis(uint32_t AxisIndex, ControllerAxis* pOutAxis)
	{
		if (AxisIndex >= ControllerAxisCount)
			return false;

		*pOutAxis = m_Axes[AxisIndex];

		return true;
	}

	DeviceClass OpenVRController::GetClass()
	{
		return DeviceClasses::Controller;
	}
	Pose* OpenVRController::GetPose()
	{
		return &m_Pose;
	}
	DeviceId OpenVRController::GetDeviceId()
	{
		return m_Id;
	}

	void OpenVRController::UpdateHand()
	{
		vr::ETrackedControllerRole Role = m_pVRSystem->GetControllerRoleForTrackedDeviceIndex(m_Id);
		m_Hand = Hands::Invalid;
		if (Role == vr::TrackedControllerRole_LeftHand)
			m_Hand = Hands::Left;

		if (Role == vr::TrackedControllerRole_RightHand)
			m_Hand = Hands::Right;
	}

	void OpenVRController::PollEvents()
	{
		UpdateHand();

		if (m_pVRSystem->GetControllerState(m_Id, &m_State, sizeof(m_State)))
		{
			for (uint8_t i = 0; i < ControllerAxisCount; i++)
			{
				m_Axes[i].Data[0] = m_State.rAxis[i].x;
				m_Axes[i].Data[1] = m_State.rAxis[i].y;
			}
		}
		else
			Msg("Controller %d isn't actually a controller?", m_Id);
	}
}