#pragma once
#include "d9vr_interfaces.h"

namespace d9vr
{
	class D9VRInterface_OpenVR;
	D9VRInterface_OpenVR* CreateOpenVRInterface();

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

	inline void ConvertFromSteamVRMatrix(const vr::HmdMatrix34_t* matPose, Matrix* pOutMatrix)
	{
		pOutMatrix->Init(
			matPose->m[0][0], matPose->m[1][0], matPose->m[2][0], 0.0,
			matPose->m[0][1], matPose->m[1][1], matPose->m[2][1], 0.0,
			matPose->m[0][2], matPose->m[1][2], matPose->m[2][2], 0.0,
			matPose->m[0][3], matPose->m[1][3], matPose->m[2][3], 1.0f
		);
	}

	inline void ConvertFromSteamVRMatrix(const vr::HmdMatrix44_t* matPose, Matrix* pOutMatrix)
	{
		memcpy(pOutMatrix, matPose, sizeof(Matrix));
	}

	inline void ConvertSteamVRPose(vr::TrackedDevicePose_t* pSteamPose, Pose* pD9VRPose)
	{
		ConvertFromSteamVRMatrix(&pSteamPose->mDeviceToAbsoluteTracking, &pD9VRPose->PoseMatrix);
		GetInternalInterface()->ConvertMatrix(&pD9VRPose->PoseMatrix);

		memcpy(&pD9VRPose->AngularVelocity, &pSteamPose->vAngularVelocity, sizeof(pD9VRPose->AngularVelocity));
		memcpy(&pD9VRPose->Velocity, &pSteamPose->vVelocity, sizeof(pD9VRPose->Velocity));
	}
}