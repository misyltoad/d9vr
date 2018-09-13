#pragma once
#include "d9vr.h"
#include "openvr.h"
#include <d3d9.h>

#include <vector>

namespace d9vr
{
	extern Eye NextRTEye;

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

	void SubmitGeneral();
	void SetNextTextureForEye(HANDLE SharedResource);
}