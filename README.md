#D9VR

D9VR is a nice D3D9 wrapper that has some magic inside to allow you to do VR stuff.

## How do I use it?
Example:

```
HMODULE hD3D9 = LoadLibraryA("d3d9.dll");
GetD9VRInterfaceProc Proc = (GetD9VRInterfaceProc)GetProcAddress(hD3D9, "GetD9VRInterface");

m_pD9VR = Proc();

if (m_pD9VR)
{
	m_pD9VR->SetConversionCallback((d9vr::MatrixConversionCallback)MatrixConversionD9VR);
	m_pD9VR->SetLoggingCallback((d9vr::LoggingCallback)Msg);
	CreateRenderTargets(materials);
}
```

Then you can use m_pD9VR->whatever.
It has a very similar interface to OpenVR (in fact, it wraps it but should hopefully be expandable.)