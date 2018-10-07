#include "d9vr_openvr_hmd.h"
#include "d9vr_openvr_util.h"

namespace d9vr
{
	OpenVRHMD::OpenVRHMD(vr::IVRSystem* pHMD, DeviceId Id)
	{
		m_zNear = 0.1f;
		m_zFar = 1024.0f;
		m_pHMD = pHMD;
		m_Id = Id;

		UpdateIPD();

		m_pHMD->GetRecommendedRenderTargetSize(&m_RenderSize.Data[0], &m_RenderSize.Data[1]);

		vr::VRCompositor();
	}
	
	float OpenVRHMD::GetIPD()
	{
		return m_IPD;
	}

	void OpenVRHMD::GetMidEyePose(Matrix* pOutMatrix)
	{
		*pOutMatrix = m_Pose.PoseMatrix;
	}

	void OpenVRHMD::GetEyeRTSize(Size* pOutSize)
	{
		pOutSize->Data[0] = m_RenderSize.Data[0];
		pOutSize->Data[1] = m_RenderSize.Data[1];
	}
	void OpenVRHMD::GetStereoRTSize(Size* pOutSize)
	{
		pOutSize->Data[0] = m_RenderSize.Data[0] * 2;
		pOutSize->Data[1] = m_RenderSize.Data[1];
	}

	void OpenVRHMD::GetProjectionMatrix(Eye nEye, Matrix* pOutMatrix)
	{
		*pOutMatrix = nEye == Eyes::Left ? m_LeftEyeProjection : m_RightEyeProjection;
	}

	void OpenVRHMD::SetZRange(float zNear, float zFar)
	{
		m_zNear = zNear;
		m_zFar = zFar;
	}

	void OpenVRHMD::GetMidToEyePose(Eye nEye, Matrix* pOutMatrix)
	{
		vr::HmdMatrix34_t matrix = m_pHMD->GetEyeToHeadTransform(ConvertEyeToSteamVR(nEye));
		ConvertFromSteamVRMatrix(&matrix, pOutMatrix);
		GetInternalInterface()->ConvertMatrix(pOutMatrix);

		// We should do this eventually.
		//if (UserMatrixInv)
		//UserMatrixInv(pOutMatrix);
	}

	DeviceClass OpenVRHMD::GetClass()
	{
		return DeviceClass::HMD;
	}

	Pose* OpenVRHMD::GetPose()
	{
		return &m_Pose;
	}

	DeviceId OpenVRHMD::GetDeviceId()
	{
		return m_Id;
	}

	void OpenVRHMD::PollEvents()
	{
		auto leftProj = m_pHMD->GetProjectionMatrix(vr::Eye_Left, m_zNear, m_zFar);
		auto rightProj = m_pHMD->GetProjectionMatrix(vr::Eye_Right, m_zNear, m_zFar);
		ConvertFromSteamVRMatrix(&leftProj, &m_LeftEyeProjection);
		ConvertFromSteamVRMatrix(&rightProj, &m_RightEyeProjection);

		UpdateIPD();
	}

	bool OpenVRHMD::UpdateIPD()
	{
		vr::TrackedPropertyError trackError = vr::TrackedProp_Success;
		m_IPD = m_pHMD->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_UserIpdMeters_Float, &trackError);

		return trackError == vr::TrackedProp_Success;
	}

	void OpenVRHMD::UpdateTrackingProperties()
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

	std::string OpenVRHMD::GetTrackedDeviceString(vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError)
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

	void OpenVRHMD::Submit()
	{
		SubmitGeneral();
	}

	void OpenVRHMD::QueueTextureCreationForEye(Eye nEye)
	{
		NextRTEye = nEye;
	}

	void OpenVRHMD::CleanupEyeTextures()
	{
		CleanupGeneral();
	}
}