#pragma once
#include "d9vr.h"
#include "d9vr_interfaces.h"
#include "openvr.h"

namespace d9vr
{
	class OpenVRHMD : public IHMD
	{
	public:
		OpenVRHMD(vr::IVRSystem* pHMD, DeviceId Id);

		float GetIPD() override;

		void GetMidEyePose(Matrix* pOutMatrix) override;

		void GetEyeRTSize(Size* pOutSize) override;
		void GetStereoRTSize(Size* pOutSize) override;

		void GetProjectionMatrix(Eye nEye, Matrix* pOutMatrix) override;

		void SetZRange(float zNear, float zFar) override;

		void GetMidToEyePose(Eye nEye, Matrix* pOutMatrix) override;
		void QueueTextureCreationForEye(Eye nEye) override;
		void CleanupEyeTextures() override;

		DeviceClass GetClass() override;
		Pose* GetPose() override;
		DeviceId GetDeviceId() override;

		void PollEvents();
	private:
		friend class D9VRInterface_OpenVR;

		bool UpdateIPD();

		void UpdateTrackingProperties();

		std::string GetTrackedDeviceString(vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError);

		void Submit();


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

	inline OpenVRHMD* AsHMD(IGenericDevice* pDevice)
	{
		if (pDevice && pDevice->GetClass() == DeviceClasses::HMD)
			return static_cast<OpenVRHMD*>(pDevice);

		return nullptr;
	}
}