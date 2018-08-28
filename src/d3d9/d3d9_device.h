#pragma once
#include "d3d9_base.h"
#include "d9vr_interfaces.h"

namespace d9vr
{
	template <typename DeviceType>
	class D3D9BaseDeviceGeneric : public D3D9Unknown<DeviceType>
	{
	public:
		D3D9BaseDeviceGeneric(DeviceType* base, IDirect3D9* parent)
			: D3D9Unknown<DeviceType>(base)
			, m_parent(parent)
		{
		}

		HRESULT WINAPI QueryInterface(REFIID riid, LPVOID* ppv)
		{
			if (riid == __uuidof(DeviceType) || riid == __uuidof(IDirect3DDevice9) || riid == __uuidof(IUnknown))
			{
				if (ppv)
					*ppv = this;

				return S_OK;
			}

			return m_base->QueryInterface(riid, ppv);
		}

		HRESULT WINAPI TestCooperativeLevel()
		{
			return m_base->TestCooperativeLevel();
		}
		UINT    WINAPI GetAvailableTextureMem()
		{
			return m_base->GetAvailableTextureMem();
		}
		HRESULT WINAPI EvictManagedResources()
		{
			return m_base->EvictManagedResources();
		}
		HRESULT WINAPI GetDirect3D(IDirect3D9** ppD3D9)
		{
			return m_base->GetDirect3D(ppD3D9);
		}
		HRESULT WINAPI GetDeviceCaps(D3DCAPS9* pCaps)
		{
			return m_base->GetDeviceCaps(pCaps);
		}
		HRESULT WINAPI GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode)
		{
			return m_base->GetDisplayMode(iSwapChain, pMode);
		}
		HRESULT WINAPI GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
		{
			return m_base->GetCreationParameters(pParameters);
		}
		HRESULT WINAPI SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap)
		{
			return m_base->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
		}
		void    WINAPI SetCursorPosition(int X, int Y, DWORD Flags)
		{
			return m_base->SetCursorPosition(X, Y, Flags);
		}
		BOOL    WINAPI ShowCursor(BOOL bShow)
		{
			return m_base->ShowCursor(bShow);
		}
		HRESULT WINAPI CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain)
		{
			return m_base->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
		}
		HRESULT WINAPI GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain)
		{
			return m_base->GetSwapChain(iSwapChain, pSwapChain);
		}
		UINT    WINAPI GetNumberOfSwapChains()
		{
			return m_base->GetNumberOfSwapChains();
		}
		HRESULT WINAPI Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
		{
			return m_base->Reset(pPresentationParameters);
		}
		HRESULT WINAPI Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
		{
			return m_base->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		}
		HRESULT WINAPI GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer)
		{
			return m_base->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
		}
		HRESULT WINAPI GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
		{
			return m_base->GetRasterStatus(iSwapChain, pRasterStatus);
		}
		HRESULT WINAPI SetDialogBoxMode(BOOL bEnableDialogs)
		{
			return m_base->SetDialogBoxMode(bEnableDialogs);
		}
		void    WINAPI SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp)
		{
			return m_base->SetGammaRamp(iSwapChain, Flags, pRamp);
		}
		void    WINAPI GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp)
		{
			return m_base->GetGammaRamp(iSwapChain, pRamp);
		}
		HRESULT WINAPI CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
		{
			// VR Edited
			HANDLE rtHandle = NULL;
			HRESULT result = m_base->CreateTexture(Width, Height, Levels, Usage, Format, NextRTEye != Eyes::Invalid ? D3DPOOL_MANAGED : Pool, ppTexture, NextRTEye != Eyes::Invalid ? &rtHandle : pSharedHandle);

			if (NextRTEye != Eyes::Invalid)
			{
				if (rtHandle != 0 && !FAILED(result))
				{
					SetNextTextureForEye(NextRTEye, rtHandle);

					if (pSharedHandle)
						*pSharedHandle = rtHandle;
				}
			}

			NextRTEye = Eyes::Invalid;

			return result;
		}
		HRESULT WINAPI CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
		{
			return m_base->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
		}
		HRESULT WINAPI CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
		{
			return m_base->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
		}
		HRESULT WINAPI CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
		{
			return m_base->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
		}
		HRESULT WINAPI CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle)
		{
			return m_base->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
		}
		HRESULT WINAPI CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
		{
			return m_base->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
		}
		HRESULT WINAPI CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
		{
			return m_base->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
		}
		HRESULT WINAPI UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint)
		{
			return m_base->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
		}
		HRESULT WINAPI UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
		{
			return m_base->UpdateTexture(pSourceTexture, pDestinationTexture);
		}
		HRESULT WINAPI GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface)
		{
			return m_base->GetRenderTargetData(pRenderTarget, pDestSurface);
		}
		HRESULT WINAPI GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface)
		{
			return m_base->GetFrontBufferData(iSwapChain, pDestSurface);
		}
		HRESULT WINAPI StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
		{
			return m_base->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
		}
		HRESULT WINAPI ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color)
		{
			return m_base->ColorFill(pSurface, pRect, color);
		}
		HRESULT WINAPI CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
		{
			return m_base->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
		}
		HRESULT WINAPI SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
		{
			return m_base->SetRenderTarget(RenderTargetIndex, pRenderTarget);
		}
		HRESULT WINAPI GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
		{
			return m_base->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
		}
		HRESULT WINAPI SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil)
		{
			return m_base->SetDepthStencilSurface(pNewZStencil);
		}
		HRESULT WINAPI GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface)
		{
			return m_base->GetDepthStencilSurface(ppZStencilSurface);
		}
		HRESULT WINAPI BeginScene()
		{
			return m_base->BeginScene();
		}
		HRESULT WINAPI EndScene()
		{
			return m_base->EndScene();
		}
		HRESULT WINAPI Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
		{
			return m_base->Clear(Count, pRects, Flags, Color, Z, Stencil);
		}
		HRESULT WINAPI SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
		{
			return m_base->SetTransform(State, pMatrix);
		}
		HRESULT WINAPI GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
		{
			return m_base->GetTransform(State, pMatrix);
		}
		HRESULT WINAPI MultiplyTransform(D3DTRANSFORMSTATETYPE TransformState, CONST D3DMATRIX* pMatrix)
		{
			return m_base->MultiplyTransform(TransformState, pMatrix);
		}
		HRESULT WINAPI SetViewport(CONST D3DVIEWPORT9* pViewport)
		{
			return m_base->SetViewport(pViewport);
		}
		HRESULT WINAPI GetViewport(D3DVIEWPORT9* pViewport)
		{
			return m_base->GetViewport(pViewport);
		}
		HRESULT WINAPI SetMaterial(CONST D3DMATERIAL9* pMaterial)
		{
			return m_base->SetMaterial(pMaterial);
		}
		HRESULT WINAPI GetMaterial(D3DMATERIAL9* pMaterial)
		{
			return m_base->GetMaterial(pMaterial);
		}
		HRESULT WINAPI SetLight(DWORD Index, CONST D3DLIGHT9* pLight)
		{
			return m_base->SetLight(Index, pLight);
		}
		HRESULT WINAPI GetLight(DWORD Index, D3DLIGHT9* pLight)
		{
			return m_base->GetLight(Index, pLight);
		}
		HRESULT WINAPI LightEnable(DWORD Index, BOOL Enable)
		{
			return m_base->LightEnable(Index, Enable);
		}
		HRESULT WINAPI GetLightEnable(DWORD Index, BOOL* pEnable)
		{
			return m_base->GetLightEnable(Index, pEnable);
		}
		HRESULT WINAPI SetClipPlane(DWORD Index, CONST float* pPlane)
		{
			return m_base->SetClipPlane(Index, pPlane);
		}
		HRESULT WINAPI GetClipPlane(DWORD Index, float* pPlane)
		{
			return m_base->GetClipPlane(Index, pPlane);
		}
		HRESULT WINAPI SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
		{
			return m_base->SetRenderState(State, Value);
		}
		HRESULT WINAPI GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue)
		{
			return m_base->GetRenderState(State, pValue);
		}
		HRESULT WINAPI CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)
		{
			return m_base->CreateStateBlock(Type, ppSB);
		}
		HRESULT WINAPI BeginStateBlock()
		{
			return m_base->BeginStateBlock();
		}
		HRESULT WINAPI EndStateBlock(IDirect3DStateBlock9** ppSB)
		{
			return m_base->EndStateBlock(ppSB);
		}
		HRESULT WINAPI SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus)
		{
			return m_base->SetClipStatus(pClipStatus);
		}
		HRESULT WINAPI GetClipStatus(D3DCLIPSTATUS9* pClipStatus)
		{
			return m_base->GetClipStatus(pClipStatus);
		}
		HRESULT WINAPI GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture)
		{
			return m_base->GetTexture(Stage, ppTexture);
		}
		HRESULT WINAPI SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
		{
			return m_base->SetTexture(Stage, pTexture);
		}
		HRESULT WINAPI GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
		{
			return m_base->GetTextureStageState(Stage, Type, pValue);
		}
		HRESULT WINAPI SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
		{
			return m_base->SetTextureStageState(Stage, Type, Value);
		}
		HRESULT WINAPI GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
		{
			return m_base->GetSamplerState(Sampler, Type, pValue);
		}
		HRESULT WINAPI SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
		{
			return m_base->SetSamplerState(Sampler, Type, Value);
		}
		HRESULT WINAPI ValidateDevice(DWORD* pNumPasses)
		{
			return m_base->ValidateDevice(pNumPasses);
		}
		HRESULT WINAPI SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries)
		{
			return m_base->SetPaletteEntries(PaletteNumber, pEntries);
		}
		HRESULT WINAPI GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries)
		{
			return m_base->GetPaletteEntries(PaletteNumber, pEntries);
		}
		HRESULT WINAPI SetCurrentTexturePalette(UINT PaletteNumber)
		{
			return m_base->SetCurrentTexturePalette(PaletteNumber);
		}
		HRESULT WINAPI GetCurrentTexturePalette(UINT *PaletteNumber)
		{
			return m_base->GetCurrentTexturePalette(PaletteNumber);
		}
		HRESULT WINAPI SetScissorRect(CONST RECT* pRect)
		{
			return m_base->SetScissorRect(pRect);
		}
		HRESULT WINAPI GetScissorRect(RECT* pRect)
		{
			return m_base->GetScissorRect(pRect);
		}
		HRESULT WINAPI SetSoftwareVertexProcessing(BOOL bSoftware)
		{
			return m_base->SetSoftwareVertexProcessing(bSoftware);
		}
		BOOL    WINAPI GetSoftwareVertexProcessing()
		{
			return m_base->GetSoftwareVertexProcessing();
		}
		HRESULT WINAPI SetNPatchMode(float nSegments)
		{
			return m_base->SetNPatchMode(nSegments);
		}
		float   WINAPI GetNPatchMode()
		{
			return m_base->GetNPatchMode();
		}
		HRESULT WINAPI DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
		{
			return m_base->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
		}
		HRESULT WINAPI DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
		{
			return m_base->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
		}
		HRESULT WINAPI DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
		{
			return m_base->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
		}
		HRESULT WINAPI DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
		{
			return m_base->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
		}
		HRESULT WINAPI ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
		{
			return m_base->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
		}
		HRESULT WINAPI CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl)
		{
			return m_base->CreateVertexDeclaration(pVertexElements, ppDecl);
		}
		HRESULT WINAPI SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
		{
			return m_base->SetVertexDeclaration(pDecl);
		}
		HRESULT WINAPI GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl)
		{
			return m_base->GetVertexDeclaration(ppDecl);
		}
		HRESULT WINAPI SetFVF(DWORD FVF)
		{
			return m_base->SetFVF(FVF);
		}
		HRESULT WINAPI GetFVF(DWORD* pFVF)
		{
			return m_base->GetFVF(pFVF);
		}
		HRESULT WINAPI CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader)
		{
			return m_base->CreateVertexShader(pFunction, ppShader);
		}
		HRESULT WINAPI SetVertexShader(IDirect3DVertexShader9* pShader)
		{
			return m_base->SetVertexShader(pShader);
		}
		HRESULT WINAPI GetVertexShader(IDirect3DVertexShader9** ppShader)
		{
			return m_base->GetVertexShader(ppShader);
		}
		HRESULT WINAPI SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
		{
			return m_base->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
		}
		HRESULT WINAPI GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
		{
			return m_base->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
		}
		HRESULT WINAPI SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
		{
			return m_base->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
		}
		HRESULT WINAPI GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
		{
			return m_base->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
		}
		HRESULT WINAPI SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
		{
			return m_base->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
		}
		HRESULT WINAPI GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
		{
			return m_base->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
		}
		HRESULT WINAPI SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
		{
			return m_base->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
		}
		HRESULT WINAPI GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride)
		{
			return m_base->GetStreamSource(StreamNumber, ppStreamData, pOffsetInBytes, pStride);
		}
		HRESULT WINAPI SetStreamSourceFreq(UINT StreamNumber, UINT Setting)
		{
			return m_base->SetStreamSourceFreq(StreamNumber, Setting);
		}
		HRESULT WINAPI GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting)
		{
			return m_base->GetStreamSourceFreq(StreamNumber, pSetting);
		}
		HRESULT WINAPI SetIndices(IDirect3DIndexBuffer9* pIndexData)
		{
			return m_base->SetIndices(pIndexData);
		}
		HRESULT WINAPI GetIndices(IDirect3DIndexBuffer9** ppIndexData)
		{
			return m_base->GetIndices(ppIndexData);
		}
		HRESULT WINAPI CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader)
		{
			return m_base->CreatePixelShader(pFunction, ppShader);
		}
		HRESULT WINAPI SetPixelShader(IDirect3DPixelShader9* pShader)
		{
			return m_base->SetPixelShader(pShader);
		}
		HRESULT WINAPI GetPixelShader(IDirect3DPixelShader9** ppShader)
		{
			return m_base->GetPixelShader(ppShader);
		}
		HRESULT WINAPI SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
		{
			return m_base->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
		}
		HRESULT WINAPI GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
		{
			return m_base->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
		}
		HRESULT WINAPI SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
		{
			return m_base->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
		}
		HRESULT WINAPI GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
		{
			return m_base->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
		}
		HRESULT WINAPI SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
		{
			return m_base->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
		}
		HRESULT WINAPI GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
		{
			return m_base->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
		}
		HRESULT WINAPI DrawRectPatch(UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)
		{
			return m_base->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
		}
		HRESULT WINAPI DrawTriPatch(UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)
		{
			return m_base->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
		}
		HRESULT WINAPI DeletePatch(UINT Handle)
		{
			return m_base->DeletePatch(Handle);
		}
		HRESULT WINAPI CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
		{
			return m_base->CreateQuery(Type, ppQuery);
		}
	protected:
		IDirect3D9* m_parent;
	};

	class D3D9BaseDeviceEx : public D3D9BaseDeviceGeneric<IDirect3DDevice9Ex>
	{
	public:
		D3D9BaseDeviceEx(IDirect3DDevice9Ex* base, IDirect3D9* parent)
			: D3D9BaseDeviceGeneric<IDirect3DDevice9Ex>(base, parent)
		{
		}

		HRESULT WINAPI SetConvolutionMonoKernel(UINT width, UINT height, float* rows, float* columns)
		{
			return m_base->SetConvolutionMonoKernel(width, height, rows, columns);
		}
		HRESULT WINAPI ComposeRects(IDirect3DSurface9* pSrc, IDirect3DSurface9* pDst, IDirect3DVertexBuffer9* pSrcRectDescs, UINT NumRects, IDirect3DVertexBuffer9* pDstRectDescs, D3DCOMPOSERECTSOP Operation, int Xoffset, int Yoffset)
		{
			return m_base->ComposeRects(pSrc, pDst, pSrcRectDescs, NumRects, pDstRectDescs, Operation, Xoffset, Yoffset);
		}
		HRESULT WINAPI GetGPUThreadPriority(INT* pPriority)
		{
			return m_base->GetGPUThreadPriority(pPriority);
		}
		HRESULT WINAPI SetGPUThreadPriority(INT Priority)
		{
			return m_base->SetGPUThreadPriority(Priority);
		}
		HRESULT WINAPI WaitForVBlank(UINT iSwapChain)
		{
			return m_base->WaitForVBlank(iSwapChain);
		}
		HRESULT WINAPI CheckResourceResidency(IDirect3DResource9** pResourceArray, UINT32 NumResources)
		{
			return m_base->CheckResourceResidency(pResourceArray, NumResources);
		}
		HRESULT WINAPI SetMaximumFrameLatency(UINT MaxLatency)
		{
			return m_base->SetMaximumFrameLatency(MaxLatency);
		}
		HRESULT WINAPI GetMaximumFrameLatency(UINT* pMaxLatency)
		{
			return m_base->GetMaximumFrameLatency(pMaxLatency);
		}
		HRESULT WINAPI CheckDeviceState(HWND hDestinationWindow)
		{
			return m_base->CheckDeviceState(hDestinationWindow);
		}

		HRESULT WINAPI PresentEx(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags)
		{
			return m_base->PresentEx(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
		}
		HRESULT WINAPI CreateRenderTargetEx(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage)
		{
			return m_base->CreateRenderTargetEx(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle, Usage);
		}
		HRESULT WINAPI CreateOffscreenPlainSurfaceEx(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage)
		{
			return m_base->CreateOffscreenPlainSurfaceEx(Width, Height, Format, Pool, ppSurface, pSharedHandle, Usage);
		}
		HRESULT WINAPI CreateDepthStencilSurfaceEx(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage)
		{
			return m_base->CreateDepthStencilSurfaceEx(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle, Usage);
		}
		HRESULT WINAPI ResetEx(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode)
		{
			return m_base->ResetEx(pPresentationParameters, pFullscreenDisplayMode);
		}
		HRESULT WINAPI GetDisplayModeEx(UINT iSwapChain, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation)
		{
			return m_base->GetDisplayModeEx(iSwapChain, pMode, pRotation);
		}
	};

	using D3D9BaseDevice = D3D9BaseDeviceGeneric<IDirect3DDevice9>;
}