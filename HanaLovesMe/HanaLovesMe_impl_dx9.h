// dear HanaLovesMe: Renderer for DirectX9
// This needs to be used along with a Platform Binding (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'LPDIRECT3DTEXTURE9' as ImTextureID. Read the FAQ about ImTextureID in HanaLovesMe.cpp.

// You can copy and use unmodified HanaLovesMe_impl_* files in your project. See main.cpp for an example of using this.
// If you are new to dear HanaLovesMe, read examples/README.txt and read the documentation at the top of HanaLovesMe.cpp.
// https://github.com/ocornut/HanaLovesMe

#pragma once

struct IDirect3DDevice9;

HanaLovesMe_IMPL_API bool     HanaLovesMe_ImplDX9_Init(IDirect3DDevice9* device);
HanaLovesMe_IMPL_API void     HanaLovesMe_ImplDX9_Shutdown();
HanaLovesMe_IMPL_API void     HanaLovesMe_ImplDX9_NewFrame();
HanaLovesMe_IMPL_API void     HanaLovesMe_ImplDX9_RenderDrawData(ImDrawData* draw_data);

// Use if you want to reset your rendering device without losing HanaLovesMe state.
HanaLovesMe_IMPL_API void     HanaLovesMe_ImplDX9_InvalidateDeviceObjects();
HanaLovesMe_IMPL_API bool     HanaLovesMe_ImplDX9_CreateDeviceObjects();
