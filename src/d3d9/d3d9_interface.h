#pragma once
#include "d3d9_base.h"
#include "d3d9_device.h"

namespace d9vr
{
	template <typename InterfaceType, typename ProxyDeviceType>
	class D3D9BaseInterfaceGeneric : public D3D9Unknown<InterfaceType>
	{
	public:
		D3D9BaseInterfaceGeneric(InterfaceType* base)
			: D3D9Unknown<InterfaceType>(base)
		{
		}

		HRESULT WINAPI QueryInterface(REFIID riid, LPVOID* ppv)
		{
			if (riid == __uuidof(InterfaceType) || riid == __uuidof(IDirect3D9) || riid == __uuidof(IUnknown))
			{
				if (ppv)
					*ppv = this;

				return S_OK;
			}

			return m_base->QueryInterface(riid, ppv);
		}

		HRESULT	 WINAPI RegisterSoftwareDevice(void* pInitializeFunction)
		{
			return m_base->RegisterSoftwareDevice(pInitializeFunction);
		}
		UINT     WINAPI GetAdapterCount()
		{
			return m_base->GetAdapterCount();
		}
		HRESULT	 WINAPI GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier)
		{
			return m_base->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
		}
		UINT     WINAPI GetAdapterModeCount(UINT Adapter, D3DFORMAT Format)
		{
			return m_base->GetAdapterModeCount(Adapter, Format);
		}
		HRESULT	 WINAPI GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode)
		{
			return m_base->GetAdapterDisplayMode(Adapter, pMode);
		}
		HRESULT	 WINAPI CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed)
		{
			return m_base->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
		}
		HRESULT	 WINAPI CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
		{
			return m_base->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
		}
		HRESULT	 WINAPI CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels)
		{
			return m_base->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
		}
		HRESULT	 WINAPI CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)
		{
			return m_base->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
		}
		HRESULT	 WINAPI CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat)
		{
			return m_base->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
		}
		HRESULT	 WINAPI GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps)
		{
			return m_base->GetDeviceCaps(Adapter, DeviceType, pCaps);
		}
		HMONITOR WINAPI GetAdapterMonitor(UINT Adapter)
		{
			return m_base->GetAdapterMonitor(Adapter);
		}
		HRESULT	 WINAPI CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
		{
			if (ppReturnedDeviceInterface)
				*ppReturnedDeviceInterface = nullptr;

			IDirect3DDevice9* pDevice = nullptr;
			HRESULT result = m_base->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, &pDevice);

			if (!pDevice || FAILED(result))
				return result;

			if (ppReturnedDeviceInterface)
				*ppReturnedDeviceInterface = new ProxyDeviceType(pDevice, this);

			return result;
		}
		HRESULT	 WINAPI EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode)
		{
			return m_base->EnumAdapterModes(Adapter, Format, Mode, pMode);
		}
	};

	template <typename ProxyDeviceType, typename ProxyDeviceTypeEx>
	class D3D9BaseInterfaceEx : public D3D9BaseInterfaceGeneric<IDirect3D9Ex, ProxyDeviceType>
	{
	public:
		D3D9BaseInterfaceEx(IDirect3D9Ex* base)
			: D3D9BaseInterfaceGeneric<IDirect3D9Ex, ProxyDeviceType>(base)
		{}
		UINT     WINAPI GetAdapterModeCountEx(UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter)
		{
			return m_base->GetAdapterModeCountEx(Adapter, pFilter);
		}
		HRESULT  WINAPI EnumAdapterModesEx(UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode)
		{
			return m_base->EnumAdapterModesEx(Adapter, pFilter, Mode, pMode);
		}
		HRESULT  WINAPI GetAdapterDisplayModeEx(UINT Adapter, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation)
		{
			return m_base->GetAdapterDisplayModeEx(Adapter, pMode, pRotation);
		}
		HRESULT  WINAPI CreateDeviceEx(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface)
		{
			if (ppReturnedDeviceInterface)
				*ppReturnedDeviceInterface = nullptr;

			IDirect3DDevice9Ex* pDevice = nullptr;
			HRESULT result = m_base->CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, &pDevice);

			if (!pDevice || FAILED(result))
				return result;

			if (ppReturnedDeviceInterface)
				*ppReturnedDeviceInterface = new ProxyDeviceTypeEx(pDevice, this);

			return result;
		}
		HRESULT  WINAPI GetAdapterLUID(UINT Adapter, LUID * pLUID)
		{
			return m_base->GetAdapterLUID(Adapter, pLUID);
		}
	};

	template <typename ProxyDeviceType>
	using D3D9BaseInterface = D3D9BaseInterfaceGeneric<IDirect3D9, ProxyDeviceType>;

	using D3D9ProxyInterface = D3D9BaseInterface<D3D9BaseDevice>;
	using D3D9ProxyInterfaceEx = D3D9BaseInterfaceEx<D3D9BaseDevice, D3D9BaseDeviceEx>;
}