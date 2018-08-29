#pragma once
#include "d9vr.h"
#include "openvr.h"
#include <d3d9.h>

#include <vector>

namespace d9vr
{
	extern Eye NextRTEye;

	class D9VRInterface_OpenVR;
	D9VRInterface_OpenVR* CreateOpenVRInterface();

	class BaseD9VRInterface : public ID9VRInterface
	{
	public:
		void SetConversionCallback(MatrixConversionCallback MatrixConvert) override;

		void SetLoggingCallback(LoggingCallback Logger) override;
		void Log(const char* pMsg);

		inline void ConvertMatrix(Matrix* pMatrix)
		{
			if (m_UserMatrixConv)
				m_UserMatrixConv(pMatrix);
		}

	protected:

		MatrixConversionCallback m_UserMatrixConv = nullptr;
		LoggingCallback m_Logger = nullptr;
	};

	BaseD9VRInterface* GetInternalInterface();
	void Msg(const char* fmt, ...);

	inline void ConvertMatrix(const vr::HmdMatrix34_t* matPose, Matrix* pOutMatrix)
	{
		pOutMatrix->Init(
			matPose->m[0][0], matPose->m[1][0], matPose->m[2][0], 0.0,
			matPose->m[0][1], matPose->m[1][1], matPose->m[2][1], 0.0,
			matPose->m[0][2], matPose->m[1][2], matPose->m[2][2], 0.0,
			matPose->m[0][3], matPose->m[1][3], matPose->m[2][3], 1.0f
		);
	}

	inline void ConvertMatrix(const vr::HmdMatrix44_t* matPose, Matrix* pOutMatrix)
	{
		memcpy(pOutMatrix, matPose, sizeof(Matrix));
	}

	void SubmitGeneral();
	void SetNextTextureForEye(HANDLE SharedResource);
}