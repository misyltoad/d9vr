#include "d9vr_interfaces.h"
#include <vadefs.h>
#include <varargs.h>

namespace d9vr
{
	void BaseD9VRInterface::SetNextTextureForEye(Eye nEye)
	{
		NextRTEye = nEye;
	}

	void BaseD9VRInterface::SetConversionCallback(MatrixConversionCallback MatrixConvert)
	{
		m_UserMatrixConv = MatrixConvert;
	}

	void BaseD9VRInterface::SetLoggingCallback(LoggingCallback Logger)
	{
		m_Logger = Logger;
	}

	void BaseD9VRInterface::Log(const char* pMsg)
	{
		if (m_Logger)
			m_Logger(pMsg);
	}

	BaseD9VRInterface* GetInternalInterface()
	{
		return static_cast<BaseD9VRInterface*>(GetD9VRInterface());
	}

	void Msg(const char* fmt, ...)
	{
		char Buffer[1024];

		va_list list;
		va_start(list, fmt);
		vsnprintf(Buffer, sizeof(Buffer), fmt, list);
		va_end(list);

		GetInternalInterface()->Log(Buffer);
	}
}