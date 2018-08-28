#include "d9vr.h"
#include "d9vr_interfaces.h"
#include "d3d11.h"

d9vr::ID9VRInterface* D9VR = nullptr;
ID3D11Device* pDevice = nullptr;

void CreateD9VR()
{
	if (!D9VR)
	{
		D9VR = (d9vr::ID9VRInterface*) d9vr::CreateOpenVRInterface();
		D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &pDevice, NULL, NULL);
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
	Eye NextRTEye = Eyes::Invalid;
	HANDLE EyeResources[2] = { 0, 0 };
	ID3D11Texture2D* EyeTextures[2] = { nullptr, nullptr };

	void SetNextTextureForEye(Eye nEye, HANDLE SharedResource)
	{
		EyeTextures[nEye] = nullptr;
		EyeResources[nEye] = SharedResource;

		ID3D11Resource* pResource = nullptr;
		ID3D11Texture2D* pTexture = nullptr;
		pDevice->OpenSharedResource(EyeResources[nEye], __uuidof(ID3D11Resource), (void**) &pResource);

		if (!pResource)
			return;

		pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture);
		pResource->Release();

		if (!pTexture)
			return;

		EyeTextures[nEye] = pTexture;
	}
}