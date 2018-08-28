#pragma once
#include <d3d9.h>

namespace d9vr
{
	template <typename Base>
	class D3D9Unknown : public Base
	{
	public:
		D3D9Unknown(Base* base)
			: m_base(base)
		{
		}

		ULONG WINAPI AddRef()
		{
			return m_base->AddRef();
		}

		ULONG WINAPI Release()
		{
			ULONG refCount = m_base->Release();
			if (refCount == 0)
				delete this;

			return refCount;
		}

		Base* GetD3D9Interface()
		{
			return m_base;
		}

		const Base* GetD3D9Interface() const
		{
			return m_base;
		}
	protected:
		Base* m_base;
	};
}