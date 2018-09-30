#include "d3d9_interface.h"
#include "d9vr.h"
#include <string>

HMODULE D3D9OriginalModule = 0;

extern void CreateD9VR();

void LoadD3DModule()
{
	if (D3D9OriginalModule)
		return;

	CreateD9VR();

	D3D9OriginalModule = LoadLibraryA("C:/Windows/System32/d3d9.dll");
	if (!D3D9OriginalModule)
		D3D9OriginalModule = LoadLibraryA("C:/Windows/SystemWOW64/d3d9.dll");
}

typedef IDirect3D9* (WINAPI *Direct3DCreate9Proc)(UINT);
typedef HRESULT (WINAPI *Direct3DCreate9ExProc)(UINT, IDirect3D9Ex**);

D9VR_EXPORT IDirect3D9* WINAPI Direct3DCreate9(UINT nSDKVersion)
{
	LoadD3DModule();

	Direct3DCreate9Proc Proc = (Direct3DCreate9Proc)GetProcAddress(D3D9OriginalModule, "Direct3DCreate9");
	IDirect3D9* OriginalInterface = Proc(nSDKVersion);
	if (!OriginalInterface)
		return nullptr;

	return new d9vr::D3D9ProxyInterface(OriginalInterface);
}

D9VR_EXPORT HRESULT WINAPI Direct3DCreate9Ex(UINT nSDKVersion, IDirect3D9Ex **ppDirect3D9Ex)
{
	LoadD3DModule();

	if (ppDirect3D9Ex)
		*ppDirect3D9Ex = nullptr;

	Direct3DCreate9ExProc Proc = (Direct3DCreate9ExProc)GetProcAddress(D3D9OriginalModule, "Direct3DCreate9Ex");

	IDirect3D9Ex* OriginalInterface = nullptr;
	HRESULT result = Proc(nSDKVersion, &OriginalInterface);

	if (!OriginalInterface)
		return result;

	if (ppDirect3D9Ex)
		*ppDirect3D9Ex = new d9vr::D3D9ProxyInterfaceEx(OriginalInterface);

	return result;
}

D9VR_EXPORT int WINAPI D3DPERF_BeginEvent(D3DCOLOR col, LPCWSTR wszName)
{
	return 0;
}

D9VR_EXPORT int WINAPI D3DPERF_EndEvent(void)
{
	return 0;
}

D9VR_EXPORT void WINAPI D3DPERF_SetMarker(D3DCOLOR col, LPCWSTR wszName)
{
}

D9VR_EXPORT void WINAPI D3DPERF_SetRegion(D3DCOLOR col, LPCWSTR wszName)
{
}

D9VR_EXPORT BOOL WINAPI D3DPERF_QueryRepeatFrame(void)
{
	return FALSE;
}

D9VR_EXPORT void WINAPI D3DPERF_SetOptions(DWORD dwOptions)
{
}

D9VR_EXPORT DWORD WINAPI D3DPERF_GetStatus(void)
{
	return 0;
}