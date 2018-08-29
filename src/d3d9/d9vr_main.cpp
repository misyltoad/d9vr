#include "d9vr.h"
#include "d9vr_interfaces.h"
#include "d3d11.h"

d9vr::ID9VRInterface* D9VR = nullptr;
ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pContext = nullptr;

void CreateD9VR()
{
	if (!D9VR)
	{
		D9VR = (d9vr::ID9VRInterface*) d9vr::CreateOpenVRInterface();
		D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &pDevice, NULL, &pContext);
	}
}

extern "C" d9vr::ID9VRInterface* GetD9VRInterface()
{
	if (!D9VR)
		CreateD9VR();

	return D9VR;
}

namespace d9vr
{
	HANDLE EyeResources[2] = { 0, 0 };
	ID3D11Texture2D* EyeTextures[2] = { nullptr, nullptr };

	void SetNextTextureForEye(HANDLE SharedResource)
	{
		if (EyeTextures[0])
			EyeTextures[0]->Release();

		EyeTextures[0] = EyeTextures[1];
		EyeResources[0] = EyeResources[1];

		EyeTextures[1] = nullptr;
		EyeResources[1] = SharedResource;

		ID3D11Resource* pResource = nullptr;
		ID3D11Texture2D* pTexture = nullptr;
		HRESULT hResult = pDevice->OpenSharedResource(SharedResource, __uuidof(ID3D11Resource), (void**) &pResource);

		if (!pResource || FAILED(hResult))
			return;

		hResult = pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture);
		pResource->Release();

		if (!pTexture || FAILED(hResult))
			return;

		EyeTextures[1] = pTexture;
	}

	Eye NextRTEye = Eyes::Invalid;

	void SubmitGeneral()
	{
		if (!EyeTextures[0] || !EyeTextures[1])
			return;

		vr::Texture_t VREyeTexture[2];
		VREyeTexture[0].handle = EyeTextures[0];
		VREyeTexture[0].eColorSpace = vr::ColorSpace_Auto;
		VREyeTexture[0].eType = vr::TextureType_DirectX;

		VREyeTexture[1].handle = EyeTextures[1];
		VREyeTexture[1].eColorSpace = vr::ColorSpace_Auto;
		VREyeTexture[1].eType = vr::TextureType_DirectX;

		vr::EVRCompositorError eError = vr::VRCompositor()->Submit(vr::Eye_Left, &VREyeTexture[0]);
		if (eError != vr::VRCompositorError_None)
			Msg("[LEFT] Compositor Error: %d\n", eError);

		eError = vr::VRCompositor()->Submit(vr::Eye_Right, &VREyeTexture[1]);
		if (eError != vr::VRCompositorError_None)
			Msg("[RIGHT] Compositor Error: %d\n", eError);
	}
}