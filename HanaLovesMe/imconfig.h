//-----------------------------------------------------------------------------
// COMPILE-TIME OPTIONS FOR DEAR HanaLovesMe
// Runtime options (clipboard callbacks, enabling various features, etc.) can generally be set via the HanaLovesMeIO structure.
// You can use HanaLovesMe::SetAllocatorFunctions() before calling HanaLovesMe::CreateContext() to rewire memory allocation functions.
//-----------------------------------------------------------------------------
// A) You may edit imconfig.h (and not overwrite it when updating HanaLovesMe, or maintain a patch/branch with your modifications to imconfig.h)
// B) or add configuration directives in your own file and compile with #define HanaLovesMe_USER_CONFIG "myfilename.h"
// If you do so you need to make sure that configuration settings are defined consistently _everywhere_ dear HanaLovesMe is used, which include
// the HanaLovesMe*.cpp files but also _any_ of your code that uses HanaLovesMe. This is because some compile-time options have an affect on data structures.
// Defining those options in imconfig.h will ensure every compilation unit gets to see the same data structure layouts.
// Call HanaLovesMe_CHECKVERSION() from your .cpp files to verify that the data structures your files are using are matching the ones HanaLovesMe.cpp is using.
//-----------------------------------------------------------------------------

#pragma once

//---- Define assertion handler. Defaults to calling assert().
//#define IM_ASSERT(_EXPR)  MyAssert(_EXPR)
//#define IM_ASSERT(_EXPR)  ((void)(_EXPR))     // Disable asserts

//---- Define attributes of all API symbols declarations, e.g. for DLL under Windows.
//#define HanaLovesMe_API __declspec( dllexport )
//#define HanaLovesMe_API __declspec( dllimport )

//---- Don't define obsolete functions/enums names. Consider enabling from time to time after updating to avoid using soon-to-be obsolete function/names.
#define HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS

//---- Don't implement demo windows functionality (ShowDemoWindow()/ShowStyleEditor()/ShowUserGuide() methods will be empty)
//---- It is very strongly recommended to NOT disable the demo windows during development. Please read the comments in HanaLovesMe_demo.cpp.
//#define HanaLovesMe_DISABLE_DEMO_WINDOWS

//---- Don't implement some functions to reduce linkage requirements.
//#define HanaLovesMe_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS   // [Win32] Don't implement default clipboard handler. Won't use and link with OpenClipboard/GetClipboardData/CloseClipboard etc.
//#define HanaLovesMe_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS         // [Win32] Don't implement default IME handler. Won't use and link with ImmGetContext/ImmSetCompositionWindow.
//#define HanaLovesMe_DISABLE_WIN32_FUNCTIONS                     // [Win32] Won't use and link with any Win32 function.
//#define HanaLovesMe_DISABLE_FORMAT_STRING_FUNCTIONS             // Don't implement ImFormatString/ImFormatStringV so you can implement them yourself if you don't want to link with vsnprintf.
//#define HanaLovesMe_DISABLE_MATH_FUNCTIONS                      // Don't implement ImFabs/ImSqrt/ImPow/ImFmod/ImCos/ImSin/ImAcos/ImAtan2 wrapper so you can implement them yourself. Declare your prototypes in imconfig.h.
//#define HanaLovesMe_DISABLE_DEFAULT_ALLOCATORS                  // Don't implement default allocators calling malloc()/free() to avoid linking with them. You will need to call HanaLovesMe::SetAllocatorFunctions().

//---- Include HanaLovesMe_user.h at the end of HanaLovesMe.h as a convenience
//#define HanaLovesMe_INCLUDE_HanaLovesMe_USER_H

//---- Pack colors to BGRA8 instead of RGBA8 (to avoid converting from one to another)
//#define HanaLovesMe_USE_BGRA_PACKED_COLOR

//---- Avoid multiple STB libraries implementations, or redefine path/filenames to prioritize another version
// By default the embedded implementations are declared static and not available outside of HanaLovesMe cpp files.
//#define HanaLovesMe_STB_TRUETYPE_FILENAME   "my_folder/stb_truetype.h"
//#define HanaLovesMe_STB_RECT_PACK_FILENAME  "my_folder/stb_rect_pack.h"
//#define HanaLovesMe_DISABLE_STB_TRUETYPE_IMPLEMENTATION
//#define HanaLovesMe_DISABLE_STB_RECT_PACK_IMPLEMENTATION

//---- Define constructor and implicit cast operators to convert back<>forth between your math types and vsize/xuifloatcolor.
// This will be inlined as part of vsize and xuifloatcolor class declarations.
/*
#define IM_VEC2_CLASS_EXTRA                                                 \
        vsize(const MyVec2& f) { x = f.x; y = f.y; }                       \
        operator MyVec2() const { return MyVec2(x,y); }
*/
#define IM_VEC4_CLASS_EXTRA  explicit xuifloatcolor(float f[3]) noexcept { x = f[0]; y = f[1]; z = f[2]; w = 0.0f; }

//---- Use 32-bit vertex indices (default is 16-bit) to allow meshes with more than 64K vertices. Render function needs to support it.
//#define ImDrawIdx unsigned int

//---- Tip: You can add extra functions within the HanaLovesMe:: namespace, here or in your own headers files.
/*
namespace HanaLovesMe
{
    void MyFunction(const char* name, const MyMatrix44& v);
}
*/
