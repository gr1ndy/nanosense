// dear HanaLovesMe, v1.70 WIP
// (drawing and font code)

/*

Index of this file:

// [SECTION] STB libraries implementation
// [SECTION] Style functions
// [SECTION] ImDrawList
// [SECTION] ImDrawData
// [SECTION] Helpers ShadeVertsXXX functions
// [SECTION] ImFontConfig
// [SECTION] ImFontAtlas
// [SECTION] ImFontAtlas glyph ranges helpers
// [SECTION] ImFontGlyphRangesBuilder
// [SECTION] ImFont
// [SECTION] Internal Render Helpers
// [SECTION] Decompression code
// [SECTION] Default font data (ProggyClean.ttf)

*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "HanaLovesMe.h"
#ifndef HanaLovesMe_DEFINE_MATH_OPERATORS
#define HanaLovesMe_DEFINE_MATH_OPERATORS
#endif
#include "HanaLovesMe_internal.h"

#include <stdio.h>      // vsnprintf, sscanf, printf
#if !defined(alloca)
#if defined(__GLIBC__) || defined(__sun) || defined(__CYGWIN__) || defined(__APPLE__)
#include <alloca.h>     // alloca (glibc uses <alloca.h>. Note that Cygwin may have _WIN32 defined, so the order matters here)
#elif defined(_WIN32)
#include <malloc.h>     // alloca
#if !defined(alloca)
#define alloca _alloca  // for clang with MS Codegen
#endif
#else
#include <stdlib.h>     // alloca
#endif
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed (stb stuff)
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#endif

// Clang/GCC warnings with -Weverything
#ifdef __clang__
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"            // warning : comparing floating point with == or != is unsafe   // storing and comparing against same constants ok.
#pragma clang diagnostic ignored "-Wglobal-constructors"    // warning : declaration requires a global destructor           // similar to above, not sure what the exact difference is.
#pragma clang diagnostic ignored "-Wsign-conversion"        // warning : implicit conversion changes signedness             //
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning : zero as null pointer constant              // some standard header variations use #define NULL 0
#endif
#if __has_warning("-Wcomma")
#pragma clang diagnostic ignored "-Wcomma"                  // warning : possible misuse of comma operator here             //
#endif
#if __has_warning("-Wreserved-id-macro")
#pragma clang diagnostic ignored "-Wreserved-id-macro"      // warning : macro name is a reserved identifier                //
#endif
#if __has_warning("-Wdouble-promotion")
#pragma clang diagnostic ignored "-Wdouble-promotion"       // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#endif
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-function"          // warning: 'xxxx' defined but not used
#pragma GCC diagnostic ignored "-Wdouble-promotion"         // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"               // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#pragma GCC diagnostic ignored "-Wstack-protector"          // warning: stack protector not protecting local variables: variable length buffer
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"          // warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif
#endif

//-------------------------------------------------------------------------
// [SECTION] STB libraries implementation
//-------------------------------------------------------------------------

// Compile time options:
//#define HanaLovesMe_STB_NAMESPACE           x07dns
//#define HanaLovesMe_STB_TRUETYPE_FILENAME   "my_folder/stb_truetype.h"
//#define HanaLovesMe_STB_RECT_PACK_FILENAME  "my_folder/stb_rect_pack.h"
//#define HanaLovesMe_DISABLE_STB_TRUETYPE_IMPLEMENTATION
//#define HanaLovesMe_DISABLE_STB_RECT_PACK_IMPLEMENTATION

#ifdef HanaLovesMe_STB_NAMESPACE
namespace HanaLovesMe_STB_NAMESPACE
{
#endif

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4456)                             // declaration of 'xx' hides previous local declaration
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wcast-qual"              // warning : cast from 'const xxxx *' to 'xxx *' drops const qualifier //
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"              // warning: comparison is always true due to limited range of data type [-Wtype-limits]
#pragma GCC diagnostic ignored "-Wcast-qual"                // warning: cast from type 'const xxxx *' to type 'xxxx *' casts away qualifiers
#endif

#ifndef STB_RECT_PACK_IMPLEMENTATION                        // in case the user already have an implementation in the _same_ compilation unit (e.g. unity builds)
#ifndef HanaLovesMe_DISABLE_STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#define STBRP_ASSERT(x)     IM_ASSERT(x)
#define STBRP_SORT          ImQsort
#define STB_RECT_PACK_IMPLEMENTATION
#endif
#ifdef HanaLovesMe_STB_RECT_PACK_FILENAME
#include HanaLovesMe_STB_RECT_PACK_FILENAME
#else
#include "x07dns_rectpack.h"
#endif
#endif

#ifndef STB_TRUETYPE_IMPLEMENTATION                         // in case the user already have an implementation in the _same_ compilation unit (e.g. unity builds)
#ifndef HanaLovesMe_DISABLE_STB_TRUETYPE_IMPLEMENTATION
#define STBTT_malloc(x,u)   ((void)(u), IM_ALLOC(x))
#define STBTT_free(x,u)     ((void)(u), IM_FREE(x))
#define STBTT_assert(x)     IM_ASSERT(x)
#define STBTT_fmod(x,y)     ImFmod(x,y)
#define STBTT_sqrt(x)       ImSqrt(x)
#define STBTT_pow(x,y)      ImPow(x,y)
#define STBTT_fabs(x)       ImFabs(x)
#define STBTT_ifloor(x)     ((int)ImFloorStd(x))
#define STBTT_iceil(x)      ((int)ImCeil(x))
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#else
#define STBTT_DEF extern
#endif
#ifdef HanaLovesMe_STB_TRUETYPE_FILENAME
#include HanaLovesMe_STB_TRUETYPE_FILENAME
#else
#include "x07dns_truetype.h"
#endif
#endif

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#ifdef HanaLovesMe_STB_NAMESPACE
} // namespace x07dns
using namespace HanaLovesMe_STB_NAMESPACE;
#endif

//-----------------------------------------------------------------------------
// [SECTION] Style functions
//-----------------------------------------------------------------------------


void HanaLovesMe::StyleColorsDark(HanaLovesMeStyle* dst)
{
    HanaLovesMeStyle* style = dst ? dst : &HanaLovesMe::GetStyle();
    xuifloatcolor* colors = style->Colors;

	colors[HanaLovesMeCol_Text] = xuicolor(213, 213, 213, 255);
	colors[HanaLovesMeCol_MenuTheme] = xuicolor(238, 75, 181, 255); // menuthemecolor
	colors[HanaLovesMeCol_TabText] = xuicolor(100, 100, 100, 255);
	colors[HanaLovesMeCol_TextShadow] = xuicolor(2, 2, 2, 255);
	colors[HanaLovesMeCol_TabTextHovered] = xuicolor(185, 185, 185, 255);
	colors[HanaLovesMeCol_TextDisabled] = xuicolor(213, 213, 213, 255);
	colors[HanaLovesMeCol_WindowBg] = xuicolor(40, 40, 40, 255);
	colors[HanaLovesMeCol_ChildBg] = xuicolor(23, 23, 23, 255);
	colors[HanaLovesMeCol_PopupBg] = xuicolor(65, 65, 65, 255);
	colors[HanaLovesMeCol_Border] = xuicolor(0, 0, 0, 255);
	colors[HanaLovesMeCol_BorderShadow] = xuifloatcolor(0.00f, 0.00f, 0.00f, 0.00f);
	colors[HanaLovesMeCol_FrameBg] = xuicolor(32, 32, 38, 255);
	colors[HanaLovesMeCol_FrameBgHovered] = xuicolor(32, 32, 38, 255);
	colors[HanaLovesMeCol_FrameBgActive] = xuicolor(32, 32, 38, 255);
	colors[HanaLovesMeCol_TitleBg] = xuicolor(38, 31, 71, 255);
	colors[HanaLovesMeCol_TitleBgActive] = xuicolor(38, 31, 71, 255);
	colors[HanaLovesMeCol_TitleBgCollapsed] = xuicolor(38, 31, 71, 255);
	colors[HanaLovesMeCol_MenuBarBg] = xuifloatcolor(0.14f, 0.14f, 0.14f, 1.00f);
	colors[HanaLovesMeCol_ScrollbarBg] = xuicolor(45, 45, 45, 255);
	colors[HanaLovesMeCol_ScrollbarGrab] = xuicolor(65, 65, 65);
	colors[HanaLovesMeCol_ScrollbarGrabHovered] = xuicolor(75, 75, 75, 255);
	colors[HanaLovesMeCol_ScrollbarGrabActive] = xuicolor(75, 75, 75, 255);
	colors[HanaLovesMeCol_CheckMark] = xuifloatcolor(0.26f, 0.59f, 0.98f, 1.00f);
	colors[HanaLovesMeCol_SliderGrab] = xuifloatcolor(0.24f, 0.52f, 0.88f, 1.00f);
	colors[HanaLovesMeCol_SliderGrabActive] = xuifloatcolor(0.26f, 0.59f, 0.98f, 1.00f);
	colors[HanaLovesMeCol_Button] = xuifloatcolor(0.26f, 0.59f, 0.98f, 0.40f);
	colors[HanaLovesMeCol_TitleButton] = xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f);
	colors[HanaLovesMeCol_ButtonHovered] = xuifloatcolor(0.26f, 0.59f, 0.98f, 1.00f);
	colors[HanaLovesMeCol_ButtonActive] = xuifloatcolor(0.06f, 0.06f, 0.06f, 1.00f);
	colors[HanaLovesMeCol_Header] = xuifloatcolor(0.26f, 0.59f, 0.98f, 0.31f);
	colors[HanaLovesMeCol_HeaderHovered] = xuifloatcolor(0.26f, 0.59f, 0.98f, 0.80f);
	colors[HanaLovesMeCol_HeaderActive] = xuifloatcolor(0.26f, 0.59f, 0.98f, 1.00f);
	colors[HanaLovesMeCol_Separator] = colors[HanaLovesMeCol_Border];
	colors[HanaLovesMeCol_SeparatorHovered] = xuifloatcolor(0.10f, 0.40f, 0.75f, 0.78f);
	colors[HanaLovesMeCol_SeparatorActive] = xuifloatcolor(0.10f, 0.40f, 0.75f, 1.00f);
	colors[HanaLovesMeCol_ResizeGrip] = xuifloatcolor(0.20f, 0.20f, 0.20f, 1.00f);
	colors[HanaLovesMeCol_ResizeGripHovered] = xuifloatcolor(0.20f, 0.20f, 0.20f, 1.00f);
	colors[HanaLovesMeCol_ResizeGripActive] = xuifloatcolor(0.20f, 0.20f, 0.20f, 1.00f);
	colors[HanaLovesMeCol_Tab] = xuicolor(34, 34, 34, 0);
	colors[HanaLovesMeCol_TabHovered] = xuicolor(51, 51, 51, 0);
	colors[HanaLovesMeCol_TabActive] = xuicolor(51, 51, 51, 0);
	colors[HanaLovesMeCol_TabUnfocused] = ImLerp(colors[HanaLovesMeCol_Tab], colors[HanaLovesMeCol_TitleBg], 0.80f);
	colors[HanaLovesMeCol_TabUnfocusedActive] = ImLerp(colors[HanaLovesMeCol_TabActive], colors[HanaLovesMeCol_TitleBg], 0.40f);
	colors[HanaLovesMeCol_PlotLines] = xuifloatcolor(0.61f, 0.61f, 0.61f, 1.00f);
	colors[HanaLovesMeCol_PlotLinesHovered] = xuifloatcolor(1.00f, 0.43f, 0.35f, 1.00f);
	colors[HanaLovesMeCol_PlotHistogram] = xuifloatcolor(0.90f, 0.70f, 0.00f, 1.00f);
	colors[HanaLovesMeCol_PlotHistogramHovered] = xuifloatcolor(1.00f, 0.60f, 0.00f, 1.00f);
	colors[HanaLovesMeCol_TextSelectedBg] = xuifloatcolor(1.00f, 0.60f, 0.00f, 1.00f);
	colors[HanaLovesMeCol_DragDropTarget] = xuifloatcolor(1.00f, 1.00f, 0.00f, 0.90f);
	colors[HanaLovesMeCol_NavHighlight] = xuifloatcolor(0.26f, 0.59f, 0.98f, 1.00f);
	colors[HanaLovesMeCol_NavWindowingHighlight] = xuifloatcolor(1.00f, 1.00f, 1.00f, 0.70f);
	colors[HanaLovesMeCol_NavWindowingDimBg] = xuifloatcolor(0.80f, 0.80f, 0.80f, 0.20f);
	colors[HanaLovesMeCol_ModalWindowDimBg] = xuifloatcolor(0.80f, 0.80f, 0.80f, 0.35f);
}

void HanaLovesMe::StyleColorsClassic(HanaLovesMeStyle* dst)
{
    HanaLovesMeStyle* style = dst ? dst : &HanaLovesMe::GetStyle();
    xuifloatcolor* colors = style->Colors;

    colors[HanaLovesMeCol_Text] = xuifloatcolor(0.90f, 0.90f, 0.90f, 1.00f);
    colors[HanaLovesMeCol_TextDisabled] = xuifloatcolor(0.60f, 0.60f, 0.60f, 1.00f);
    colors[HanaLovesMeCol_WindowBg] = xuifloatcolor(0.00f, 0.00f, 0.00f, 0.70f);
    colors[HanaLovesMeCol_ChildBg] = xuifloatcolor(0.00f, 0.00f, 0.00f, 0.00f);
    colors[HanaLovesMeCol_PopupBg] = xuifloatcolor(0.11f, 0.11f, 0.14f, 0.92f);
    colors[HanaLovesMeCol_Border] = xuifloatcolor(0.50f, 0.50f, 0.50f, 0.50f);
    colors[HanaLovesMeCol_BorderShadow] = xuifloatcolor(0.00f, 0.00f, 0.00f, 0.00f);
    colors[HanaLovesMeCol_FrameBg] = xuifloatcolor(0.43f, 0.43f, 0.43f, 0.39f);
    colors[HanaLovesMeCol_FrameBgHovered] = xuifloatcolor(0.47f, 0.47f, 0.69f, 0.40f);
    colors[HanaLovesMeCol_FrameBgActive] = xuifloatcolor(0.42f, 0.41f, 0.64f, 0.69f);
    colors[HanaLovesMeCol_TitleBg] = xuifloatcolor(0.27f, 0.27f, 0.54f, 0.83f);
    colors[HanaLovesMeCol_TitleBgActive] = xuifloatcolor(0.32f, 0.32f, 0.63f, 0.87f);
    colors[HanaLovesMeCol_TitleBgCollapsed] = xuifloatcolor(0.40f, 0.40f, 0.80f, 0.20f);
    colors[HanaLovesMeCol_MenuBarBg] = xuifloatcolor(0.40f, 0.40f, 0.55f, 0.80f);
    colors[HanaLovesMeCol_ScrollbarBg] = xuifloatcolor(0.20f, 0.25f, 0.30f, 0.60f);
    colors[HanaLovesMeCol_ScrollbarGrab] = xuifloatcolor(0.40f, 0.40f, 0.80f, 0.30f);
    colors[HanaLovesMeCol_ScrollbarGrabHovered] = xuifloatcolor(0.40f, 0.40f, 0.80f, 0.40f);
    colors[HanaLovesMeCol_ScrollbarGrabActive] = xuifloatcolor(0.41f, 0.39f, 0.80f, 0.60f);
    colors[HanaLovesMeCol_CheckMark] = xuifloatcolor(0.90f, 0.90f, 0.90f, 0.50f);
    colors[HanaLovesMeCol_SliderGrab] = xuifloatcolor(1.00f, 1.00f, 1.00f, 0.30f);
    colors[HanaLovesMeCol_SliderGrabActive] = xuifloatcolor(0.41f, 0.39f, 0.80f, 0.60f);
    colors[HanaLovesMeCol_Button] = xuifloatcolor(0.35f, 0.40f, 0.61f, 0.62f);
    colors[HanaLovesMeCol_ButtonHovered] = xuifloatcolor(0.40f, 0.48f, 0.71f, 0.79f);
    colors[HanaLovesMeCol_ButtonActive] = xuifloatcolor(0.46f, 0.54f, 0.80f, 1.00f);
    colors[HanaLovesMeCol_Header] = xuifloatcolor(0.40f, 0.40f, 0.90f, 0.45f);
    colors[HanaLovesMeCol_HeaderHovered] = xuifloatcolor(0.45f, 0.45f, 0.90f, 0.80f);
    colors[HanaLovesMeCol_HeaderActive] = xuifloatcolor(0.53f, 0.53f, 0.87f, 0.80f);
    colors[HanaLovesMeCol_Separator] = xuifloatcolor(0.50f, 0.50f, 0.50f, 1.00f);
    colors[HanaLovesMeCol_SeparatorHovered] = xuifloatcolor(0.60f, 0.60f, 0.70f, 1.00f);
    colors[HanaLovesMeCol_SeparatorActive] = xuifloatcolor(0.70f, 0.70f, 0.90f, 1.00f);
    colors[HanaLovesMeCol_ResizeGrip] = xuifloatcolor(1.00f, 1.00f, 1.00f, 0.16f);
    colors[HanaLovesMeCol_ResizeGripHovered] = xuifloatcolor(0.78f, 0.82f, 1.00f, 0.60f);
    colors[HanaLovesMeCol_ResizeGripActive] = xuifloatcolor(0.78f, 0.82f, 1.00f, 0.90f);
    colors[HanaLovesMeCol_Tab] = ImLerp(colors[HanaLovesMeCol_Header], colors[HanaLovesMeCol_TitleBgActive], 0.80f);
    colors[HanaLovesMeCol_TabHovered] = colors[HanaLovesMeCol_HeaderHovered];
    colors[HanaLovesMeCol_TabActive] = ImLerp(colors[HanaLovesMeCol_HeaderActive], colors[HanaLovesMeCol_TitleBgActive], 0.60f);
    colors[HanaLovesMeCol_TabUnfocused] = ImLerp(colors[HanaLovesMeCol_Tab], colors[HanaLovesMeCol_TitleBg], 0.80f);
    colors[HanaLovesMeCol_TabUnfocusedActive] = ImLerp(colors[HanaLovesMeCol_TabActive], colors[HanaLovesMeCol_TitleBg], 0.40f);
    colors[HanaLovesMeCol_PlotLines] = xuifloatcolor(1.00f, 1.00f, 1.00f, 1.00f);
    colors[HanaLovesMeCol_PlotLinesHovered] = xuifloatcolor(0.90f, 0.70f, 0.00f, 1.00f);
    colors[HanaLovesMeCol_PlotHistogram] = xuifloatcolor(0.90f, 0.70f, 0.00f, 1.00f);
    colors[HanaLovesMeCol_PlotHistogramHovered] = xuifloatcolor(1.00f, 0.60f, 0.00f, 1.00f);
    colors[HanaLovesMeCol_TextSelectedBg] = xuifloatcolor(0.00f, 0.00f, 1.00f, 0.35f);
    colors[HanaLovesMeCol_DragDropTarget] = xuifloatcolor(1.00f, 1.00f, 0.00f, 0.90f);
    colors[HanaLovesMeCol_NavHighlight] = colors[HanaLovesMeCol_HeaderHovered];
    colors[HanaLovesMeCol_NavWindowingHighlight] = xuifloatcolor(1.00f, 1.00f, 1.00f, 0.70f);
    colors[HanaLovesMeCol_NavWindowingDimBg] = xuifloatcolor(0.80f, 0.80f, 0.80f, 0.20f);
    colors[HanaLovesMeCol_ModalWindowDimBg] = xuifloatcolor(0.20f, 0.20f, 0.20f, 0.35f);
}

// Those light colors are better suited with a thicker font than the default one + FrameBorder
void HanaLovesMe::StyleColorsLight(HanaLovesMeStyle* dst)
{
    HanaLovesMeStyle* style = dst ? dst : &HanaLovesMe::GetStyle();
    xuifloatcolor* colors = style->Colors;

    colors[HanaLovesMeCol_Text] = xuifloatcolor(0.00f, 0.00f, 0.00f, 1.00f);
    colors[HanaLovesMeCol_TextDisabled] = xuifloatcolor(0.60f, 0.60f, 0.60f, 1.00f);
    colors[HanaLovesMeCol_WindowBg] = xuifloatcolor(0.94f, 0.94f, 0.94f, 1.00f);
    colors[HanaLovesMeCol_ChildBg] = xuifloatcolor(0.00f, 0.00f, 0.00f, 0.00f);
    colors[HanaLovesMeCol_PopupBg] = xuifloatcolor(1.00f, 1.00f, 1.00f, 0.98f);
    colors[HanaLovesMeCol_Border] = xuifloatcolor(0.00f, 0.00f, 0.00f, 0.30f);
    colors[HanaLovesMeCol_BorderShadow] = xuifloatcolor(0.00f, 0.00f, 0.00f, 0.00f);
    colors[HanaLovesMeCol_FrameBg] = xuifloatcolor(1.00f, 1.00f, 1.00f, 1.00f);
    colors[HanaLovesMeCol_FrameBgHovered] = xuifloatcolor(0.26f, 0.59f, 0.98f, 0.40f);
    colors[HanaLovesMeCol_FrameBgActive] = xuifloatcolor(0.26f, 0.59f, 0.98f, 0.67f);
    colors[HanaLovesMeCol_TitleBg] = xuifloatcolor(0.96f, 0.96f, 0.96f, 1.00f);
    colors[HanaLovesMeCol_TitleBgActive] = xuifloatcolor(0.82f, 0.82f, 0.82f, 1.00f);
    colors[HanaLovesMeCol_TitleBgCollapsed] = xuifloatcolor(1.00f, 1.00f, 1.00f, 0.51f);
    colors[HanaLovesMeCol_MenuBarBg] = xuifloatcolor(0.86f, 0.86f, 0.86f, 1.00f);
    colors[HanaLovesMeCol_ScrollbarBg] = xuifloatcolor(0.98f, 0.98f, 0.98f, 0.53f);
    colors[HanaLovesMeCol_ScrollbarGrab] = xuifloatcolor(0.69f, 0.69f, 0.69f, 0.80f);
    colors[HanaLovesMeCol_ScrollbarGrabHovered] = xuifloatcolor(0.49f, 0.49f, 0.49f, 0.80f);
    colors[HanaLovesMeCol_ScrollbarGrabActive] = xuifloatcolor(0.49f, 0.49f, 0.49f, 1.00f);
    colors[HanaLovesMeCol_CheckMark] = xuifloatcolor(0.26f, 0.59f, 0.98f, 1.00f);
    colors[HanaLovesMeCol_SliderGrab] = xuifloatcolor(0.26f, 0.59f, 0.98f, 0.78f);
    colors[HanaLovesMeCol_SliderGrabActive] = xuifloatcolor(0.46f, 0.54f, 0.80f, 0.60f);
    colors[HanaLovesMeCol_Button] = xuifloatcolor(0.26f, 0.59f, 0.98f, 0.40f);
    colors[HanaLovesMeCol_ButtonHovered] = xuifloatcolor(0.26f, 0.59f, 0.98f, 1.00f);
    colors[HanaLovesMeCol_ButtonActive] = xuifloatcolor(0.06f, 0.53f, 0.98f, 1.00f);
    colors[HanaLovesMeCol_Header] = xuifloatcolor(0.26f, 0.59f, 0.98f, 0.31f);
    colors[HanaLovesMeCol_HeaderHovered] = xuifloatcolor(0.26f, 0.59f, 0.98f, 0.80f);
    colors[HanaLovesMeCol_HeaderActive] = xuifloatcolor(0.26f, 0.59f, 0.98f, 1.00f);
    colors[HanaLovesMeCol_Separator] = xuifloatcolor(0.39f, 0.39f, 0.39f, 1.00f);
    colors[HanaLovesMeCol_SeparatorHovered] = xuifloatcolor(0.14f, 0.44f, 0.80f, 0.78f);
    colors[HanaLovesMeCol_SeparatorActive] = xuifloatcolor(0.14f, 0.44f, 0.80f, 1.00f);
    colors[HanaLovesMeCol_ResizeGrip] = xuifloatcolor(0.80f, 0.80f, 0.80f, 0.56f);
    colors[HanaLovesMeCol_ResizeGripHovered] = xuifloatcolor(0.26f, 0.59f, 0.98f, 0.67f);
    colors[HanaLovesMeCol_ResizeGripActive] = xuifloatcolor(0.26f, 0.59f, 0.98f, 0.95f);
    colors[HanaLovesMeCol_Tab] = ImLerp(colors[HanaLovesMeCol_Header], colors[HanaLovesMeCol_TitleBgActive], 0.90f);
    colors[HanaLovesMeCol_TabHovered] = colors[HanaLovesMeCol_HeaderHovered];
    colors[HanaLovesMeCol_TabActive] = ImLerp(colors[HanaLovesMeCol_HeaderActive], colors[HanaLovesMeCol_TitleBgActive], 0.60f);
    colors[HanaLovesMeCol_TabUnfocused] = ImLerp(colors[HanaLovesMeCol_Tab], colors[HanaLovesMeCol_TitleBg], 0.80f);
    colors[HanaLovesMeCol_TabUnfocusedActive] = ImLerp(colors[HanaLovesMeCol_TabActive], colors[HanaLovesMeCol_TitleBg], 0.40f);
    colors[HanaLovesMeCol_PlotLines] = xuifloatcolor(0.39f, 0.39f, 0.39f, 1.00f);
    colors[HanaLovesMeCol_PlotLinesHovered] = xuifloatcolor(1.00f, 0.43f, 0.35f, 1.00f);
    colors[HanaLovesMeCol_PlotHistogram] = xuifloatcolor(0.90f, 0.70f, 0.00f, 1.00f);
    colors[HanaLovesMeCol_PlotHistogramHovered] = xuifloatcolor(1.00f, 0.45f, 0.00f, 1.00f);
    colors[HanaLovesMeCol_TextSelectedBg] = xuifloatcolor(0.26f, 0.59f, 0.98f, 0.35f);
    colors[HanaLovesMeCol_DragDropTarget] = xuifloatcolor(0.26f, 0.59f, 0.98f, 0.95f);
    colors[HanaLovesMeCol_NavHighlight] = colors[HanaLovesMeCol_HeaderHovered];
    colors[HanaLovesMeCol_NavWindowingHighlight] = xuifloatcolor(0.70f, 0.70f, 0.70f, 0.70f);
    colors[HanaLovesMeCol_NavWindowingDimBg] = xuifloatcolor(0.20f, 0.20f, 0.20f, 0.20f);
    colors[HanaLovesMeCol_ModalWindowDimBg] = xuifloatcolor(0.20f, 0.20f, 0.20f, 0.35f);
}

//-----------------------------------------------------------------------------
// ImDrawList
//-----------------------------------------------------------------------------

ImDrawListSharedData::ImDrawListSharedData()
{
    Font = NULL;
    FontSize = 0.0f;
    CurveTessellationTol = 0.0f;
    ClipRectFullscreen = xuifloatcolor(-8192.0f, -8192.0f, +8192.0f, +8192.0f);

    // Const data
    for (int i = 0; i < IM_ARRAYSIZE(CircleVtx12); i++)
    {
        const float a = ((float)i * 2 * IM_PI) / (float)IM_ARRAYSIZE(CircleVtx12);
        CircleVtx12[i] = vsize(ImCos(a), ImSin(a));
    }
}

void ImDrawList::Clear()
{
    CmdBuffer.resize(0);
    IdxBuffer.resize(0);
    VtxBuffer.resize(0);
    Flags = ImDrawListFlags_AntiAliasedLines | ImDrawListFlags_AntiAliasedFill;
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.resize(0);
    _TextureIdStack.resize(0);
    _Path.resize(0);
    _ChannelsCurrent = 0;
    _ChannelsCount = 1;
    // NB: Do not clear channels so our allocations are re-used after the first frame.
}

void ImDrawList::ClearFreeMemory()
{
    CmdBuffer.clear();
    IdxBuffer.clear();
    VtxBuffer.clear();
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.clear();
    _TextureIdStack.clear();
    _Path.clear();
    _ChannelsCurrent = 0;
    _ChannelsCount = 1;
    for (int i = 0; i < _Channels.Size; i++)
    {
        if (i == 0) memset(&_Channels[0], 0, sizeof(_Channels[0]));  // channel 0 is a copy of CmdBuffer/IdxBuffer, don't destruct again
        _Channels[i].CmdBuffer.clear();
        _Channels[i].IdxBuffer.clear();
    }
    _Channels.clear();
}

ImDrawList* ImDrawList::CloneOutput() const
{
    ImDrawList* dst = IM_NEW(ImDrawList(NULL));
    dst->CmdBuffer = CmdBuffer;
    dst->IdxBuffer = IdxBuffer;
    dst->VtxBuffer = VtxBuffer;
    dst->Flags = Flags;
    return dst;
}

// Using macros because C++ is a terrible language, we want guaranteed inline, no code in header, and no overhead in Debug builds
#define GetCurrentClipRect()    (_ClipRectStack.Size ? _ClipRectStack.Data[_ClipRectStack.Size-1]  : _Data->ClipRectFullscreen)
#define GetCurrentTextureId()   (_TextureIdStack.Size ? _TextureIdStack.Data[_TextureIdStack.Size-1] : (ImTextureID)NULL)

void ImDrawList::AddDrawCmd()
{
    ImDrawCmd draw_cmd;
    draw_cmd.ClipRect = GetCurrentClipRect();
    draw_cmd.TextureId = GetCurrentTextureId();

    IM_ASSERT(draw_cmd.ClipRect.x <= draw_cmd.ClipRect.z && draw_cmd.ClipRect.y <= draw_cmd.ClipRect.w);
    CmdBuffer.push_back(draw_cmd);
}

void ImDrawList::AddCallback(ImDrawCallback callback, void* callback_data)
{
    ImDrawCmd* current_cmd = CmdBuffer.Size ? &CmdBuffer.back() : NULL;
    if (!current_cmd || current_cmd->ElemCount != 0 || current_cmd->UserCallback != NULL)
    {
        AddDrawCmd();
        current_cmd = &CmdBuffer.back();
    }
    current_cmd->UserCallback = callback;
    current_cmd->UserCallbackData = callback_data;

    AddDrawCmd(); // Force a new command after us (see comment below)
}

// Our scheme may appears a bit unusual, basically we want the most-common calls AddLine AddRect etc. to not have to perform any check so we always have a command ready in the stack.
// The cost of figuring out if a new command has to be added or if we can merge is paid in those Update** functions only.
void ImDrawList::UpdateClipRect()
{
    // If current command is used with different settings we need to add a new command
    const xuifloatcolor curr_clip_rect = GetCurrentClipRect();
    ImDrawCmd* curr_cmd = CmdBuffer.Size > 0 ? &CmdBuffer.Data[CmdBuffer.Size - 1] : NULL;
    if (!curr_cmd || (curr_cmd->ElemCount != 0 && memcmp(&curr_cmd->ClipRect, &curr_clip_rect, sizeof(xuifloatcolor)) != 0) || curr_cmd->UserCallback != NULL)
    {
        AddDrawCmd();
        return;
    }

    // Try to merge with previous command if it matches, else use current command
    ImDrawCmd* prev_cmd = CmdBuffer.Size > 1 ? curr_cmd - 1 : NULL;
    if (curr_cmd->ElemCount == 0 && prev_cmd && memcmp(&prev_cmd->ClipRect, &curr_clip_rect, sizeof(xuifloatcolor)) == 0 && prev_cmd->TextureId == GetCurrentTextureId() && prev_cmd->UserCallback == NULL)
        CmdBuffer.pop_back();
    else
        curr_cmd->ClipRect = curr_clip_rect;
}

void ImDrawList::UpdateTextureID()
{
    // If current command is used with different settings we need to add a new command
    const ImTextureID curr_texture_id = GetCurrentTextureId();
    ImDrawCmd* curr_cmd = CmdBuffer.Size ? &CmdBuffer.back() : NULL;
    if (!curr_cmd || (curr_cmd->ElemCount != 0 && curr_cmd->TextureId != curr_texture_id) || curr_cmd->UserCallback != NULL)
    {
        AddDrawCmd();
        return;
    }

    // Try to merge with previous command if it matches, else use current command
    ImDrawCmd* prev_cmd = CmdBuffer.Size > 1 ? curr_cmd - 1 : NULL;
    if (curr_cmd->ElemCount == 0 && prev_cmd && prev_cmd->TextureId == curr_texture_id && memcmp(&prev_cmd->ClipRect, &GetCurrentClipRect(), sizeof(xuifloatcolor)) == 0 && prev_cmd->UserCallback == NULL)
        CmdBuffer.pop_back();
    else
        curr_cmd->TextureId = curr_texture_id;
}

#undef GetCurrentClipRect
#undef GetCurrentTextureId

// Render-level scissoring. This is passed down to your render function but not used for CPU-side coarse clipping. Prefer using higher-level HanaLovesMe::PushClipRect() to affect logic (hit-testing and widget culling)
void ImDrawList::PushClipRect(vsize cr_min, vsize cr_max, bool intersect_with_current_clip_rect)
{
    xuifloatcolor cr(cr_min.x, cr_min.y, cr_max.x, cr_max.y);
    if (intersect_with_current_clip_rect && _ClipRectStack.Size)
    {
        xuifloatcolor current = _ClipRectStack.Data[_ClipRectStack.Size - 1];
        if (cr.x < current.x) cr.x = current.x;
        if (cr.y < current.y) cr.y = current.y;
        if (cr.z > current.z) cr.z = current.z;
        if (cr.w > current.w) cr.w = current.w;
    }
    cr.z = ImMax(cr.x, cr.z);
    cr.w = ImMax(cr.y, cr.w);

    _ClipRectStack.push_back(cr);
    UpdateClipRect();
}

void ImDrawList::PushClipRectFullScreen()
{
    PushClipRect(vsize(_Data->ClipRectFullscreen.x, _Data->ClipRectFullscreen.y), vsize(_Data->ClipRectFullscreen.z, _Data->ClipRectFullscreen.w));
}

void ImDrawList::PopClipRect()
{
    IM_ASSERT(_ClipRectStack.Size > 0);
    _ClipRectStack.pop_back();
    UpdateClipRect();
}

void ImDrawList::PushTextureID(ImTextureID texture_id)
{
    _TextureIdStack.push_back(texture_id);
    UpdateTextureID();
}

void ImDrawList::PopTextureID()
{
    IM_ASSERT(_TextureIdStack.Size > 0);
    _TextureIdStack.pop_back();
    UpdateTextureID();
}

void ImDrawList::ChannelsSplit(int channels_count)
{
    IM_ASSERT(_ChannelsCurrent == 0 && _ChannelsCount == 1);
    int old_channels_count = _Channels.Size;
    if (old_channels_count < channels_count)
        _Channels.resize(channels_count);
    _ChannelsCount = channels_count;

    // _Channels[] (24/32 bytes each) hold stohnly that we'll swap with this->_CmdBuffer/_IdxBuffer
    // The content of _Channels[0] at this point doesn't matter. We clear it to make state tidy in a debugger but we don't strictly need to.
    // When we switch to the next channel, we'll copy _CmdBuffer/_IdxBuffer into _Channels[0] and then _Channels[1] into _CmdBuffer/_IdxBuffer
    memset(&_Channels[0], 0, sizeof(ImDrawChannel));
    for (int i = 1; i < channels_count; i++)
    {
        if (i >= old_channels_count)
        {
            IM_PLACEMENT_NEW(&_Channels[i]) ImDrawChannel();
        } else
        {
            _Channels[i].CmdBuffer.resize(0);
            _Channels[i].IdxBuffer.resize(0);
        }
        if (_Channels[i].CmdBuffer.Size == 0)
        {
            ImDrawCmd draw_cmd;
            draw_cmd.ClipRect = _ClipRectStack.back();
            draw_cmd.TextureId = _TextureIdStack.back();
            _Channels[i].CmdBuffer.push_back(draw_cmd);
        }
    }
}

void ImDrawList::ChannelsMerge()
{
    // Note that we never use or rely on channels.Size because it is merely a buffer that we never shrink back to 0 to keep all sub-buffers ready for use.
    if (_ChannelsCount <= 1)
        return;

    ChannelsSetCurrent(0);
    if (CmdBuffer.Size && CmdBuffer.back().ElemCount == 0)
        CmdBuffer.pop_back();

    int new_cmd_buffer_count = 0, new_idx_buffer_count = 0;
    for (int i = 1; i < _ChannelsCount; i++)
    {
        ImDrawChannel& ch = _Channels[i];
        if (ch.CmdBuffer.Size && ch.CmdBuffer.back().ElemCount == 0)
            ch.CmdBuffer.pop_back();
        new_cmd_buffer_count += ch.CmdBuffer.Size;
        new_idx_buffer_count += ch.IdxBuffer.Size;
    }
    CmdBuffer.resize(CmdBuffer.Size + new_cmd_buffer_count);
    IdxBuffer.resize(IdxBuffer.Size + new_idx_buffer_count);

    ImDrawCmd * cmd_write = CmdBuffer.Data + CmdBuffer.Size - new_cmd_buffer_count;
    _IdxWritePtr = IdxBuffer.Data + IdxBuffer.Size - new_idx_buffer_count;
    for (int i = 1; i < _ChannelsCount; i++)
    {
        ImDrawChannel& ch = _Channels[i];
        if (int sz = ch.CmdBuffer.Size) { memcpy(cmd_write, ch.CmdBuffer.Data, sz * sizeof(ImDrawCmd)); cmd_write += sz; }
        if (int sz = ch.IdxBuffer.Size) { memcpy(_IdxWritePtr, ch.IdxBuffer.Data, sz * sizeof(ImDrawIdx)); _IdxWritePtr += sz; }
    }
    UpdateClipRect(); // We call this instead of AddDrawCmd(), so that empty channels won't produce an extra draw call.
    _ChannelsCount = 1;
}

void ImDrawList::ChannelsSetCurrent(int idx)
{
    IM_ASSERT(idx < _ChannelsCount);
    if (_ChannelsCurrent == idx) return;
    memcpy(&_Channels.Data[_ChannelsCurrent].CmdBuffer, &CmdBuffer, sizeof(CmdBuffer)); // copy 12 bytes, four times
    memcpy(&_Channels.Data[_ChannelsCurrent].IdxBuffer, &IdxBuffer, sizeof(IdxBuffer));
    _ChannelsCurrent = idx;
    memcpy(&CmdBuffer, &_Channels.Data[_ChannelsCurrent].CmdBuffer, sizeof(CmdBuffer));
    memcpy(&IdxBuffer, &_Channels.Data[_ChannelsCurrent].IdxBuffer, sizeof(IdxBuffer));
    _IdxWritePtr = IdxBuffer.Data + IdxBuffer.Size;
}

// NB: this can be called with negative count for removing primitives (as long as the result does not underflow)
void ImDrawList::PrimReserve(int idx_count, int vtx_count)
{
    ImDrawCmd& draw_cmd = CmdBuffer.Data[CmdBuffer.Size - 1];
    draw_cmd.ElemCount += idx_count;

    int vtx_buffer_old_size = VtxBuffer.Size;
    VtxBuffer.resize(vtx_buffer_old_size + vtx_count);
    _VtxWritePtr = VtxBuffer.Data + vtx_buffer_old_size;

    int idx_buffer_old_size = IdxBuffer.Size;
    IdxBuffer.resize(idx_buffer_old_size + idx_count);
    _IdxWritePtr = IdxBuffer.Data + idx_buffer_old_size;
}

// Fully unrolled with inline call to keep our debug builds decently fast.
void ImDrawList::Prvsrect(const vsize & a, const vsize & c, ImU32 col)
{
    vsize b(c.x, a.y), d(a.x, c.y), uv(_Data->TexUvWhitePixel);
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx + 1); _IdxWritePtr[2] = (ImDrawIdx)(idx + 2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx + 3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

void ImDrawList::PrvsrectUV(const vsize & a, const vsize & c, const vsize & uv_a, const vsize & uv_c, ImU32 col)
{
    vsize b(c.x, a.y), d(a.x, c.y), uv_b(uv_c.x, uv_a.y), uv_d(uv_a.x, uv_c.y);
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx + 1); _IdxWritePtr[2] = (ImDrawIdx)(idx + 2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx + 3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

void ImDrawList::PrimQuadUV(const vsize & a, const vsize & b, const vsize & c, const vsize & d, const vsize & uv_a, const vsize & uv_b, const vsize & uv_c, const vsize & uv_d, ImU32 col)
{
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx + 1); _IdxWritePtr[2] = (ImDrawIdx)(idx + 2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx + 3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

// On AddPolyline() and AddConvexPolyFilled() we intentionally avoid using vsize and superflous function calls to optimize debug/non-inlined builds.
// Those macros expects l-values.
#define IM_NORMALIZE2F_OVER_ZERO(VX,VY)                         { float d2 = VX*VX + VY*VY; if (d2 > 0.0f) { float inv_len = 1.0f / ImSqrt(d2); VX *= inv_len; VY *= inv_len; } }
#define IM_NORMALIZE2F_OVER_EPSILON_CLAMP(VX,VY,EPS,INVLENMAX)  { float d2 = VX*VX + VY*VY; if (d2 > EPS)  { float inv_len = 1.0f / ImSqrt(d2); if (inv_len > INVLENMAX) inv_len = INVLENMAX; VX *= inv_len; VY *= inv_len; } }

// TODO: Thickness anti-aliased lines cap are missing their AA fringe.
// We avoid using the vsize math operators here to reduce cost to a minimum for debug/non-inlined builds.
void ImDrawList::AddPolyline(const vsize * points, const int points_count, ImU32 col, bool closed, float thickness)
{
    if (points_count < 2)
        return;

    const vsize uv = _Data->TexUvWhitePixel;

    int count = points_count;
    if (!closed)
        count = points_count - 1;

    const bool thick_line = thickness > 1.0f;
    if (Flags & ImDrawListFlags_AntiAliasedLines)
    {
        // Anti-aliased stroke
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & ~IM_COL32_A_MASK;

        const int idx_count = thick_line ? count * 18 : count * 12;
        const int vtx_count = thick_line ? points_count * 4 : points_count * 3;
        PrimReserve(idx_count, vtx_count);

        // Temporary buffer
        vsize* temp_normals = (vsize*)alloca(points_count * (thick_line ? 5 : 3) * sizeof(vsize)); //-V630
        vsize * temp_points = temp_normals + points_count;

        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
            float dx = points[i2].x - points[i1].x;
            float dy = points[i2].y - points[i1].y;
            IM_NORMALIZE2F_OVER_ZERO(dx, dy);
            temp_normals[i1].x = dy;
            temp_normals[i1].y = -dx;
        }
        if (!closed)
            temp_normals[points_count - 1] = temp_normals[points_count - 2];

        if (!thick_line)
        {
            if (!closed)
            {
                temp_points[0] = points[0] + temp_normals[0] * AA_SIZE;
                temp_points[1] = points[0] - temp_normals[0] * AA_SIZE;
                temp_points[(points_count - 1) * 2 + 0] = points[points_count - 1] + temp_normals[points_count - 1] * AA_SIZE;
                temp_points[(points_count - 1) * 2 + 1] = points[points_count - 1] - temp_normals[points_count - 1] * AA_SIZE;
            }

            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx;
            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
                unsigned int idx2 = (i1 + 1) == points_count ? _VtxCurrentIdx : idx1 + 3;

                // Avehnly normals
                float dm_x = (temp_normals[i1].x + temp_normals[i2].x) * 0.5f;
                float dm_y = (temp_normals[i1].y + temp_normals[i2].y) * 0.5f;
                IM_NORMALIZE2F_OVER_EPSILON_CLAMP(dm_x, dm_y, 0.000001f, 100.0f)
                    dm_x *= AA_SIZE;
                dm_y *= AA_SIZE;

                // Add temporary vertexes
                vsize * out_vtx = &temp_points[i2 * 2];
                out_vtx[0].x = points[i2].x + dm_x;
                out_vtx[0].y = points[i2].y + dm_y;
                out_vtx[1].x = points[i2].x - dm_x;
                out_vtx[1].y = points[i2].y - dm_y;

                // Add indexes
                _IdxWritePtr[0] = (ImDrawIdx)(idx2 + 0); _IdxWritePtr[1] = (ImDrawIdx)(idx1 + 0); _IdxWritePtr[2] = (ImDrawIdx)(idx1 + 2);
                _IdxWritePtr[3] = (ImDrawIdx)(idx1 + 2); _IdxWritePtr[4] = (ImDrawIdx)(idx2 + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx2 + 0);
                _IdxWritePtr[6] = (ImDrawIdx)(idx2 + 1); _IdxWritePtr[7] = (ImDrawIdx)(idx1 + 1); _IdxWritePtr[8] = (ImDrawIdx)(idx1 + 0);
                _IdxWritePtr[9] = (ImDrawIdx)(idx1 + 0); _IdxWritePtr[10] = (ImDrawIdx)(idx2 + 0); _IdxWritePtr[11] = (ImDrawIdx)(idx2 + 1);
                _IdxWritePtr += 12;

                idx1 = idx2;
            }

            // Add vertexes
            for (int i = 0; i < points_count; i++)
            {
                _VtxWritePtr[0].pos = points[i];          _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
                _VtxWritePtr[1].pos = temp_points[i * 2 + 0]; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;
                _VtxWritePtr[2].pos = temp_points[i * 2 + 1]; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col_trans;
                _VtxWritePtr += 3;
            }
        } else
        {
            const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;
            if (!closed)
            {
                temp_points[0] = points[0] + temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[1] = points[0] + temp_normals[0] * (half_inner_thickness);
                temp_points[2] = points[0] - temp_normals[0] * (half_inner_thickness);
                temp_points[3] = points[0] - temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[(points_count - 1) * 4 + 0] = points[points_count - 1] + temp_normals[points_count - 1] * (half_inner_thickness + AA_SIZE);
                temp_points[(points_count - 1) * 4 + 1] = points[points_count - 1] + temp_normals[points_count - 1] * (half_inner_thickness);
                temp_points[(points_count - 1) * 4 + 2] = points[points_count - 1] - temp_normals[points_count - 1] * (half_inner_thickness);
                temp_points[(points_count - 1) * 4 + 3] = points[points_count - 1] - temp_normals[points_count - 1] * (half_inner_thickness + AA_SIZE);
            }

            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx;
            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
                unsigned int idx2 = (i1 + 1) == points_count ? _VtxCurrentIdx : idx1 + 4;

                // Avehnly normals
                float dm_x = (temp_normals[i1].x + temp_normals[i2].x) * 0.5f;
                float dm_y = (temp_normals[i1].y + temp_normals[i2].y) * 0.5f;
                IM_NORMALIZE2F_OVER_EPSILON_CLAMP(dm_x, dm_y, 0.000001f, 100.0f);
                float dm_out_x = dm_x * (half_inner_thickness + AA_SIZE);
                float dm_out_y = dm_y * (half_inner_thickness + AA_SIZE);
                float dm_in_x = dm_x * half_inner_thickness;
                float dm_in_y = dm_y * half_inner_thickness;

                // Add temporary vertexes
                vsize * out_vtx = &temp_points[i2 * 4];
                out_vtx[0].x = points[i2].x + dm_out_x;
                out_vtx[0].y = points[i2].y + dm_out_y;
                out_vtx[1].x = points[i2].x + dm_in_x;
                out_vtx[1].y = points[i2].y + dm_in_y;
                out_vtx[2].x = points[i2].x - dm_in_x;
                out_vtx[2].y = points[i2].y - dm_in_y;
                out_vtx[3].x = points[i2].x - dm_out_x;
                out_vtx[3].y = points[i2].y - dm_out_y;

                // Add indexes
                _IdxWritePtr[0] = (ImDrawIdx)(idx2 + 1); _IdxWritePtr[1] = (ImDrawIdx)(idx1 + 1); _IdxWritePtr[2] = (ImDrawIdx)(idx1 + 2);
                _IdxWritePtr[3] = (ImDrawIdx)(idx1 + 2); _IdxWritePtr[4] = (ImDrawIdx)(idx2 + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx2 + 1);
                _IdxWritePtr[6] = (ImDrawIdx)(idx2 + 1); _IdxWritePtr[7] = (ImDrawIdx)(idx1 + 1); _IdxWritePtr[8] = (ImDrawIdx)(idx1 + 0);
                _IdxWritePtr[9] = (ImDrawIdx)(idx1 + 0); _IdxWritePtr[10] = (ImDrawIdx)(idx2 + 0); _IdxWritePtr[11] = (ImDrawIdx)(idx2 + 1);
                _IdxWritePtr[12] = (ImDrawIdx)(idx2 + 2); _IdxWritePtr[13] = (ImDrawIdx)(idx1 + 2); _IdxWritePtr[14] = (ImDrawIdx)(idx1 + 3);
                _IdxWritePtr[15] = (ImDrawIdx)(idx1 + 3); _IdxWritePtr[16] = (ImDrawIdx)(idx2 + 3); _IdxWritePtr[17] = (ImDrawIdx)(idx2 + 2);
                _IdxWritePtr += 18;

                idx1 = idx2;
            }

            // Add vertexes
            for (int i = 0; i < points_count; i++)
            {
                _VtxWritePtr[0].pos = temp_points[i * 4 + 0]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col_trans;
                _VtxWritePtr[1].pos = temp_points[i * 4 + 1]; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
                _VtxWritePtr[2].pos = temp_points[i * 4 + 2]; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
                _VtxWritePtr[3].pos = temp_points[i * 4 + 3]; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col_trans;
                _VtxWritePtr += 4;
            }
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    } else
    {
        // Non Anti-aliased Stroke
        const int idx_count = count * 6;
        const int vtx_count = count * 4;      // FIXME-OPT: Not sharing edges
        PrimReserve(idx_count, vtx_count);

        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
            const vsize & p1 = points[i1];
            const vsize & p2 = points[i2];

            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            IM_NORMALIZE2F_OVER_ZERO(dx, dy);
            dx *= (thickness * 0.5f);
            dy *= (thickness * 0.5f);

            _VtxWritePtr[0].pos.x = p1.x + dy; _VtxWritePtr[0].pos.y = p1.y - dx; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr[1].pos.x = p2.x + dy; _VtxWritePtr[1].pos.y = p2.y - dx; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
            _VtxWritePtr[2].pos.x = p2.x - dy; _VtxWritePtr[2].pos.y = p2.y + dx; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
            _VtxWritePtr[3].pos.x = p1.x - dy; _VtxWritePtr[3].pos.y = p1.y + dx; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
            _VtxWritePtr += 4;

            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx + 1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx + 2);
            _IdxWritePtr[3] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[4] = (ImDrawIdx)(_VtxCurrentIdx + 2); _IdxWritePtr[5] = (ImDrawIdx)(_VtxCurrentIdx + 3);
            _IdxWritePtr += 6;
            _VtxCurrentIdx += 4;
        }
    }
}

// We intentionally avoid using vsize and its math operators here to reduce cost to a minimum for debug/non-inlined builds.
void ImDrawList::AddConvexPolyFilled(const vsize * points, const int points_count, ImU32 col)
{
    if (points_count < 3)
        return;

    const vsize uv = _Data->TexUvWhitePixel;

    if (Flags & ImDrawListFlags_AntiAliasedFill)
    {
        // Anti-aliased Fill
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & ~IM_COL32_A_MASK;
        const int idx_count = (points_count - 2) * 3 + points_count * 6;
        const int vtx_count = (points_count * 2);
        PrimReserve(idx_count, vtx_count);

        // Add indexes for fill
        unsigned int vtx_inner_idx = _VtxCurrentIdx;
        unsigned int vtx_outer_idx = _VtxCurrentIdx + 1;
        for (int i = 2; i < points_count; i++)
        {
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + ((i - 1) << 1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_inner_idx + (i << 1));
            _IdxWritePtr += 3;
        }

        // Compute normals
        vsize * temp_normals = (vsize*)alloca(points_count * sizeof(vsize)); //-V630
        for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            const vsize& p0 = points[i0];
            const vsize& p1 = points[i1];
            float dx = p1.x - p0.x;
            float dy = p1.y - p0.y;
            IM_NORMALIZE2F_OVER_ZERO(dx, dy);
            temp_normals[i0].x = dy;
            temp_normals[i0].y = -dx;
        }

        for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            // Avehnly normals
            const vsize& n0 = temp_normals[i0];
            const vsize& n1 = temp_normals[i1];
            float dm_x = (n0.x + n1.x) * 0.5f;
            float dm_y = (n0.y + n1.y) * 0.5f;
            IM_NORMALIZE2F_OVER_EPSILON_CLAMP(dm_x, dm_y, 0.000001f, 100.0f);
            dm_x *= AA_SIZE * 0.5f;
            dm_y *= AA_SIZE * 0.5f;

            // Add vertices
            _VtxWritePtr[0].pos.x = (points[i1].x - dm_x); _VtxWritePtr[0].pos.y = (points[i1].y - dm_y); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;        // Inner
            _VtxWritePtr[1].pos.x = (points[i1].x + dm_x); _VtxWritePtr[1].pos.y = (points[i1].y + dm_y); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;  // Outer
            _VtxWritePtr += 2;

            // Add indexes for fringes
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1)); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + (i0 << 1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1));
            _IdxWritePtr[3] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1)); _IdxWritePtr[4] = (ImDrawIdx)(vtx_outer_idx + (i1 << 1)); _IdxWritePtr[5] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1));
            _IdxWritePtr += 6;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    } else
    {
        // Non Anti-aliased Fill
        const int idx_count = (points_count - 2) * 3;
        const int vtx_count = points_count;
        PrimReserve(idx_count, vtx_count);
        for (int i = 0; i < vtx_count; i++)
        {
            _VtxWritePtr[0].pos = points[i]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr++;
        }
        for (int i = 2; i < points_count; i++)
        {
            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx + i - 1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx + i);
            _IdxWritePtr += 3;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
}

void ImDrawList::PathArcToFast(const vsize & centre, float radius, int a_min_of_12, int a_max_of_12)
{
    if (radius == 0.0f || a_min_of_12 > a_max_of_12)
    {
        _Path.push_back(centre);
        return;
    }
    _Path.reserve(_Path.Size + (a_max_of_12 - a_min_of_12 + 1));
    for (int a = a_min_of_12; a <= a_max_of_12; a++)
    {
        const vsize& c = _Data->CircleVtx12[a % IM_ARRAYSIZE(_Data->CircleVtx12)];
        _Path.push_back(vsize(centre.x + c.x * radius, centre.y + c.y * radius));
    }
}

void ImDrawList::PathArcTo(const vsize & centre, float radius, float a_min, float a_max, int num_segments)
{
    if (radius == 0.0f)
    {
        _Path.push_back(centre);
        return;
    }

    // Note that we are adding a point at both a_min and a_max.
    // If you are trying to draw a full closed circle you don't want the overlapping points!
    _Path.reserve(_Path.Size + (num_segments + 1));
    for (int i = 0; i <= num_segments; i++)
    {
        const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
        _Path.push_back(vsize(centre.x + ImCos(a) * radius, centre.y + ImSin(a) * radius));
    }
}

static void PathBezierToCasteljau(ImVector<vsize> * path, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tess_tol, int level)
{
    float dx = x4 - x1;
    float dy = y4 - y1;
    float d2 = ((x2 - x4) * dy - (y2 - y4) * dx);
    float d3 = ((x3 - x4) * dy - (y3 - y4) * dx);
    d2 = (d2 >= 0) ? d2 : -d2;
    d3 = (d3 >= 0) ? d3 : -d3;
    if ((d2 + d3) * (d2 + d3) < tess_tol * (dx * dx + dy * dy))
    {
        path->push_back(vsize(x4, y4));
    } else if (level < 10)
    {
        float x12 = (x1 + x2) * 0.5f, y12 = (y1 + y2) * 0.5f;
        float x23 = (x2 + x3) * 0.5f, y23 = (y2 + y3) * 0.5f;
        float x34 = (x3 + x4) * 0.5f, y34 = (y3 + y4) * 0.5f;
        float x123 = (x12 + x23) * 0.5f, y123 = (y12 + y23) * 0.5f;
        float x234 = (x23 + x34) * 0.5f, y234 = (y23 + y34) * 0.5f;
        float x1234 = (x123 + x234) * 0.5f, y1234 = (y123 + y234) * 0.5f;

        PathBezierToCasteljau(path, x1, y1, x12, y12, x123, y123, x1234, y1234, tess_tol, level + 1);
        PathBezierToCasteljau(path, x1234, y1234, x234, y234, x34, y34, x4, y4, tess_tol, level + 1);
    }
}

void ImDrawList::PathBezierCurveTo(const vsize & p2, const vsize & p3, const vsize & p4, int num_segments)
{
    vsize p1 = _Path.back();
    if (num_segments == 0)
    {
        // Auto-tessellated
        PathBezierToCasteljau(&_Path, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, _Data->CurveTessellationTol, 0);
    } else
    {
        float t_step = 1.0f / (float)num_segments;
        for (int i_step = 1; i_step <= num_segments; i_step++)
        {
            float t = t_step * i_step;
            float u = 1.0f - t;
            float w1 = u * u * u;
            float w2 = 3 * u * u * t;
            float w3 = 3 * u * t * t;
            float w4 = t * t * t;
            _Path.push_back(vsize(w1 * p1.x + w2 * p2.x + w3 * p3.x + w4 * p4.x, w1 * p1.y + w2 * p2.y + w3 * p3.y + w4 * p4.y));
        }
    }
}

void ImDrawList::PathRect(const vsize & a, const vsize & b, float rounding, int rounding_corners)
{
    rounding = ImMin(rounding, ImFabs(b.x - a.x) * (((rounding_corners & ImDrawCornerFlags_Top) == ImDrawCornerFlags_Top) || ((rounding_corners & ImDrawCornerFlags_Bot) == ImDrawCornerFlags_Bot) ? 0.5f : 1.0f) - 1.0f);
    rounding = ImMin(rounding, ImFabs(b.y - a.y) * (((rounding_corners & ImDrawCornerFlags_Left) == ImDrawCornerFlags_Left) || ((rounding_corners & ImDrawCornerFlags_Right) == ImDrawCornerFlags_Right) ? 0.5f : 1.0f) - 1.0f);

    if (rounding <= 0.0f || rounding_corners == 0)
    {
        PathLineTo(a);
        PathLineTo(vsize(b.x, a.y));
        PathLineTo(b);
        PathLineTo(vsize(a.x, b.y));
    } else
    {
        const float rounding_tl = (rounding_corners & ImDrawCornerFlags_TopLeft) ? rounding : 0.0f;
        const float rounding_tr = (rounding_corners & ImDrawCornerFlags_TopRight) ? rounding : 0.0f;
        const float rounding_br = (rounding_corners & ImDrawCornerFlags_BotRight) ? rounding : 0.0f;
        const float rounding_bl = (rounding_corners & ImDrawCornerFlags_BotLeft) ? rounding : 0.0f;
        PathArcToFast(vsize(a.x + rounding_tl, a.y + rounding_tl), rounding_tl, 6, 9);
        PathArcToFast(vsize(b.x - rounding_tr, a.y + rounding_tr), rounding_tr, 9, 12);
        PathArcToFast(vsize(b.x - rounding_br, b.y - rounding_br), rounding_br, 0, 3);
        PathArcToFast(vsize(a.x + rounding_bl, b.y - rounding_bl), rounding_bl, 3, 6);
    }
}

void ImDrawList::AddLine(const vsize & a, const vsize & b, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    PathLineTo(a + vsize(0.5f, 0.5f));
    PathLineTo(b + vsize(0.5f, 0.5f));
    PathStroke(col, false, thickness);
}

// a: upper-left, b: lower-right. we don't render 1 px sized rectangles properly.
void ImDrawList::AddRect(const vsize & a, const vsize & b, ImU32 col, float rounding, int rounding_corners_flags, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    if (Flags & ImDrawListFlags_AntiAliasedLines)
        PathRect(a + vsize(0.5f, 0.5f), b - vsize(0.50f, 0.50f), rounding, rounding_corners_flags);
    else
        PathRect(a + vsize(0.5f, 0.5f), b - vsize(0.49f, 0.49f), rounding, rounding_corners_flags); // Better looking lower-right corner and rounded non-AA shapes.
    PathStroke(col, true, thickness);
}

void ImDrawList::AddRectFilled(const vsize & a, const vsize & b, ImU32 col, float rounding, int rounding_corners_flags)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    if (rounding > 0.0f)
    {
        PathRect(a, b, rounding, rounding_corners_flags);
        PathFillConvex(col);
    } else
    {
        PrimReserve(6, 4);
        Prvsrect(a, b, col);
    }
}

void ImDrawList::AddRectFilledMultiColor(const vsize & a, const vsize & c, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left)
{
    if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & IM_COL32_A_MASK) == 0)
        return;

    const vsize uv = _Data->TexUvWhitePixel;
    PrimReserve(6, 4);
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 1)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 2));
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 2)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 3));
    PrimWriteVtx(a, uv, col_upr_left);
    PrimWriteVtx(vsize(c.x, a.y), uv, col_upr_right);
    PrimWriteVtx(c, uv, col_bot_right);
    PrimWriteVtx(vsize(a.x, c.y), uv, col_bot_left);
}

void ImDrawList::AddQuad(const vsize & a, const vsize & b, const vsize & c, const vsize & d, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathLineTo(d);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddQuadFilled(const vsize & a, const vsize & b, const vsize & c, const vsize & d, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathLineTo(d);
    PathFillConvex(col);
}

void ImDrawList::AddTriangle(const vsize & a, const vsize & b, const vsize & c, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddTriangleFilled(const vsize & a, const vsize & b, const vsize & c, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathFillConvex(col);
}

void ImDrawList::AddCircle(const vsize & centre, float radius, ImU32 col, int num_segments, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0 || num_segments <= 2)
        return;

    // Because we are filling a closed shape we remove 1 from the count of segments/points
    const float a_max = IM_PI * 2.0f * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(centre, radius - 0.5f, 0.0f, a_max, num_segments - 1);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddCircleFilled(const vsize & centre, float radius, ImU32 col, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0 || num_segments <= 2)
        return;

    // Because we are filling a closed shape we remove 1 from the count of segments/points
    const float a_max = IM_PI * 2.0f * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(centre, radius, 0.0f, a_max, num_segments - 1);
    PathFillConvex(col);
}

void ImDrawList::AddBezierCurve(const vsize & pos0, const vsize & cp0, const vsize & cp1, const vsize & pos1, ImU32 col, float thickness, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(pos0);
    PathBezierCurveTo(cp0, cp1, pos1, num_segments);
    PathStroke(col, false, thickness);
}

void ImDrawList::AddText(const ImFont * font, float font_size, const vsize & pos, ImU32 col, const char* text_begin, const char* text_end, float wrap_width, const xuifloatcolor * cpu_fine_clip_rect)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    if (text_end == NULL)
        text_end = text_begin + strlen(text_begin);
    if (text_begin == text_end)
        return;

    // Pull default font/size from the shared ImDrawListSharedData instance
    if (font == NULL)
        font = _Data->Font;
    if (font_size == 0.0f)
        font_size = _Data->FontSize;

    IM_ASSERT(font->ContainerAtlas->TexID == _TextureIdStack.back());  // Use high-level HanaLovesMe::PushFont() or low-level ImDrawList::PushTextureId() to change font.

    xuifloatcolor clip_rect = _ClipRectStack.back();
    if (cpu_fine_clip_rect)
    {
        clip_rect.x = ImMax(clip_rect.x, cpu_fine_clip_rect->x);
        clip_rect.y = ImMax(clip_rect.y, cpu_fine_clip_rect->y);
        clip_rect.z = ImMin(clip_rect.z, cpu_fine_clip_rect->z);
        clip_rect.w = ImMin(clip_rect.w, cpu_fine_clip_rect->w);
    }
    font->RenderText(this, font_size, pos, col, clip_rect, text_begin, text_end, wrap_width, cpu_fine_clip_rect != NULL);
}

void ImDrawList::AddText(const vsize & pos, ImU32 col, const char* text_begin, const char* text_end)
{
    AddText(NULL, 0.0f, pos, col, text_begin, text_end);
}

void ImDrawList::AddImage(ImTextureID user_texture_id, const vsize & a, const vsize & b, const vsize & uv_a, const vsize & uv_b, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    const bool push_texture_id = _TextureIdStack.empty() || user_texture_id != _TextureIdStack.back();
    if (push_texture_id)
        PushTextureID(user_texture_id);

    PrimReserve(6, 4);
    PrvsrectUV(a, b, uv_a, uv_b, col);

    if (push_texture_id)
        PopTextureID();
}

void ImDrawList::AddImageQuad(ImTextureID user_texture_id, const vsize & a, const vsize & b, const vsize & c, const vsize & d, const vsize & uv_a, const vsize & uv_b, const vsize & uv_c, const vsize & uv_d, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    const bool push_texture_id = _TextureIdStack.empty() || user_texture_id != _TextureIdStack.back();
    if (push_texture_id)
        PushTextureID(user_texture_id);

    PrimReserve(6, 4);
    PrimQuadUV(a, b, c, d, uv_a, uv_b, uv_c, uv_d, col);

    if (push_texture_id)
        PopTextureID();
}

void ImDrawList::AddImageRounded(ImTextureID user_texture_id, const vsize & a, const vsize & b, const vsize & uv_a, const vsize & uv_b, ImU32 col, float rounding, int rounding_corners)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    if (rounding <= 0.0f || (rounding_corners & ImDrawCornerFlags_All) == 0)
    {
        AddImage(user_texture_id, a, b, uv_a, uv_b, col);
        return;
    }

    const bool push_texture_id = _TextureIdStack.empty() || user_texture_id != _TextureIdStack.back();
    if (push_texture_id)
        PushTextureID(user_texture_id);

    int vert_start_idx = VtxBuffer.Size;
    PathRect(a, b, rounding, rounding_corners);
    PathFillConvex(col);
    int vert_end_idx = VtxBuffer.Size;
    HanaLovesMe::ShadeVertsLinearUV(this, vert_start_idx, vert_end_idx, a, b, uv_a, uv_b, true);

    if (push_texture_id)
        PopTextureID();
}

//-----------------------------------------------------------------------------
// [SECTION] ImDrawData
//-----------------------------------------------------------------------------

// For backward compatibility: convert all buffers from indexed to de-indexed, in case you cannot render indexed. Note: this is slow and most likely a waste of resources. Always prefer indexed rendering!
void ImDrawData::DeIndexAllBuffers()
{
    ImVector<ImDrawVert> new_vtx_buffer;
    TotalVtxCount = TotalIdxCount = 0;
    for (int i = 0; i < CmdListsCount; i++)
    {
        ImDrawList* cmd_list = CmdLists[i];
        if (cmd_list->IdxBuffer.empty())
            continue;
        new_vtx_buffer.resize(cmd_list->IdxBuffer.Size);
        for (int j = 0; j < cmd_list->IdxBuffer.Size; j++)
            new_vtx_buffer[j] = cmd_list->VtxBuffer[cmd_list->IdxBuffer[j]];
        cmd_list->VtxBuffer.swap(new_vtx_buffer);
        cmd_list->IdxBuffer.resize(0);
        TotalVtxCount += cmd_list->VtxBuffer.Size;
    }
}

// Helper to scale the ClipRect field of each ImDrawCmd.
// Use if your final output buffer is at a different scale than draw_data->DisplaySize,
// or if there is a difference between your window resolution and framebuffer resolution.
void ImDrawData::ScaleClipRects(const vsize & fb_scale)
{
    for (int i = 0; i < CmdListsCount; i++)
    {
        ImDrawList* cmd_list = CmdLists[i];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            ImDrawCmd* cmd = &cmd_list->CmdBuffer[cmd_i];
            cmd->ClipRect = xuifloatcolor(cmd->ClipRect.x * fb_scale.x, cmd->ClipRect.y * fb_scale.y, cmd->ClipRect.z * fb_scale.x, cmd->ClipRect.w * fb_scale.y);
        }
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Helpers ShadeVertsXXX functions
//-----------------------------------------------------------------------------

// Generic linear color gradient, write to RGB fields, leave A untouched.
void HanaLovesMe::ShadeVertsLinearColorGradientKeepAlpha(ImDrawList * draw_list, int vert_start_idx, int vert_end_idx, vsize gradient_p0, vsize gradient_p1, ImU32 col0, ImU32 col1)
{
    vsize gradient_extent = gradient_p1 - gradient_p0;
    float gradient_inv_length2 = 1.0f / ImLengthSqr(gradient_extent);
    ImDrawVert* vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
    ImDrawVert* vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
    for (ImDrawVert* vert = vert_start; vert < vert_end; vert++)
    {
        float d = ImDot(vert->pos - gradient_p0, gradient_extent);
        float t = ImClamp(d * gradient_inv_length2, 0.0f, 1.0f);
        int r = ImLerp((int)(col0 >> IM_COL32_R_SHIFT) & 0xFF, (int)(col1 >> IM_COL32_R_SHIFT) & 0xFF, t);
        int g = ImLerp((int)(col0 >> IM_COL32_G_SHIFT) & 0xFF, (int)(col1 >> IM_COL32_G_SHIFT) & 0xFF, t);
        int b = ImLerp((int)(col0 >> IM_COL32_B_SHIFT) & 0xFF, (int)(col1 >> IM_COL32_B_SHIFT) & 0xFF, t);
        vert->col = (r << IM_COL32_R_SHIFT) | (g << IM_COL32_G_SHIFT) | (b << IM_COL32_B_SHIFT) | (vert->col & IM_COL32_A_MASK);
    }
}

// Distribute UV over (a, b) rectangle
void HanaLovesMe::ShadeVertsLinearUV(ImDrawList * draw_list, int vert_start_idx, int vert_end_idx, const vsize & a, const vsize & b, const vsize & uv_a, const vsize & uv_b, bool clamp)
{
    const vsize size = b - a;
    const vsize uv_size = uv_b - uv_a;
    const vsize scale = vsize(
        size.x != 0.0f ? (uv_size.x / size.x) : 0.0f,
        size.y != 0.0f ? (uv_size.y / size.y) : 0.0f);

    ImDrawVert * vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
    ImDrawVert * vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
    if (clamp)
    {
        const vsize min = ImMin(uv_a, uv_b);
        const vsize max = ImMax(uv_a, uv_b);
        for (ImDrawVert* vertex = vert_start; vertex < vert_end; ++vertex)
            vertex->uv = ImClamp(uv_a + ImMul(vsize(vertex->pos.x, vertex->pos.y) - a, scale), min, max);
    } else
    {
        for (ImDrawVert* vertex = vert_start; vertex < vert_end; ++vertex)
            vertex->uv = uv_a + ImMul(vsize(vertex->pos.x, vertex->pos.y) - a, scale);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontConfig
//-----------------------------------------------------------------------------

ImFontConfig::ImFontConfig()
{
    FontData = NULL;
    FontDataSize = 0;
    FontDataOwnedByAtlas = true;
    FontNo = 0;
    SizePixels = 0.0f;
    OversampleH = 3; // FIXME: 2 may be a better default?
    OversampleV = 1;
    PixelSnapH = true;
    GlyphExtraSpacing = vsize(0.25f, 0.0f);
    GlyphOffset = vsize(0.15f, 0.0f);
    GlyphRanges = NULL;
    GlyphMinAdvanceX = 0.0f;
    GlyphMaxAdvanceX = FLT_MAX;
    MergeMode = false;
    RasterizerFlags = 0x00;
    RasterizerMultiply = 1.15f;
    memset(Name, 0, sizeof(Name));
    DstFont = NULL;
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontAtlas
//-----------------------------------------------------------------------------

// A work of art lies ahead! (. = white layer, X = black layer, others are blank)
// The white texels on the top left are the ones we'll use everywhere in HanaLovesMe to render filled shapes.
const int FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF = 108;
const int FONT_ATLAS_DEFAULT_TEX_DATA_H = 27;
const unsigned int FONT_ATLAS_DEFAULT_TEX_DATA_ID = 0x80000000;
static const char FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS[FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF * FONT_ATLAS_DEFAULT_TEX_DATA_H + 1] =
{
    "..-         -XXXXXXX-    X    -           X           -XXXXXXX          -          XXXXXXX-     XX          "
    "..-         -X.....X-   X.X   -          X.X          -X.....X          -          X.....X-    X..X         "
    "---         -XXX.XXX-  X...X  -         X...X         -X....X           -           X....X-    X..X         "
    "X           -  X.X  - X.....X -        X.....X        -X...X            -            X...X-    X..X         "
    "XX          -  X.X  -X.......X-       X.......X       -X..X.X           -           X.X..X-    X..X         "
    "X.X         -  X.X  -XXXX.XXXX-       XXXX.XXXX       -X.X X.X          -          X.X X.X-    X..XXX       "
    "X..X        -  X.X  -   X.X   -          X.X          -XX   X.X         -         X.X   XX-    X..X..XXX    "
    "X...X       -  X.X  -   X.X   -    XX    X.X    XX    -      X.X        -        X.X      -    X..X..X..XX  "
    "X....X      -  X.X  -   X.X   -   X.X    X.X    X.X   -       X.X       -       X.X       -    X..X..X..X.X "
    "X.....X     -  X.X  -   X.X   -  X..X    X.X    X..X  -        X.X      -      X.X        -XXX X..X..X..X..X"
    "X......X    -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -         X.X   XX-XX   X.X         -X..XX........X..X"
    "X.......X   -  X.X  -   X.X   -X.....................X-          X.X X.X-X.X X.X          -X...X...........X"
    "X........X  -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -           X.X..X-X..X.X           - X..............X"
    "X.........X -XXX.XXX-   X.X   -  X..X    X.X    X..X  -            X...X-X...X            -  X.............X"
    "X..........X-X.....X-   X.X   -   X.X    X.X    X.X   -           X....X-X....X           -  X.............X"
    "X......XXXXX-XXXXXXX-   X.X   -    XX    X.X    XX    -          X.....X-X.....X          -   X............X"
    "X...X..X    ---------   X.X   -          X.X          -          XXXXXXX-XXXXXXX          -   X...........X "
    "X..X X..X   -       -XXXX.XXXX-       XXXX.XXXX       -------------------------------------    X..........X "
    "X.X  X..X   -       -X.......X-       X.......X       -    XX           XX    -           -    X..........X "
    "XX    X..X  -       - X.....X -        X.....X        -   X.X           X.X   -           -     X........X  "
    "      X..X          -  X...X  -         X...X         -  X..X           X..X  -           -     X........X  "
    "       XX           -   X.X   -          X.X          - X...XXXXXXXXXXXXX...X -           -     XXXXXXXXXX  "
    "------------        -    X    -           X           -X.....................X-           ------------------"
    "                    ----------------------------------- X...XXXXXXXXXXXXX...X -                             "
    "                                                      -  X..X           X..X  -                             "
    "                                                      -   X.X           X.X   -                             "
    "                                                      -    XX           XX    -                             "
};

static const vsize FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[HanaLovesMeMouseCursor_COUNT][3] =
{
    // Pos ........ Size ......... Offset ......
    { vsize(0,3), vsize(12,19), vsize(0, 0) }, // HanaLovesMeMouseCursor_Arrow
    { vsize(13,0), vsize(7,16), vsize(1, 8) }, // HanaLovesMeMouseCursor_TextInput
    { vsize(31,0), vsize(23,23), vsize(11,11) }, // HanaLovesMeMouseCursor_ResizeAll
    { vsize(21,0), vsize(9,23), vsize(4,11) }, // HanaLovesMeMouseCursor_ResizeNS
    { vsize(55,18),vsize(23, 9), vsize(11, 4) }, // HanaLovesMeMouseCursor_ResizeEW
    { vsize(73,0), vsize(17,17), vsize(8, 8) }, // HanaLovesMeMouseCursor_ResizeNESW
    { vsize(55,0), vsize(17,17), vsize(8, 8) }, // HanaLovesMeMouseCursor_ResizeNWSE
    { vsize(91,0), vsize(17,22), vsize(5, 0) }, // HanaLovesMeMouseCursor_Hand
};

ImFontAtlas::ImFontAtlas()
{
    Locked = false;
    Flags = ImFontAtlasFlags_None;
    TexID = (ImTextureID)NULL;
    TexDesiredWidth = 0;
    TexGlyphPadding = 1;

    TexPixelsAlpha8 = NULL;
    TexPixelsRGBA32 = NULL;
    TexWidth = TexHeight = 0;
    TexUvScale = vsize(0.0f, 0.0f);
    TexUvWhitePixel = vsize(0.0f, 0.0f);
    for (int n = 0; n < IM_ARRAYSIZE(CustomRectIds); n++)
        CustomRectIds[n] = -1;
}

ImFontAtlas::~ImFontAtlas()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    Clear();
}

void    ImFontAtlas::ClearInputData()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    for (int i = 0; i < ConfigData.Size; i++)
        if (ConfigData[i].FontData && ConfigData[i].FontDataOwnedByAtlas)
        {
            IM_FREE(ConfigData[i].FontData);
            ConfigData[i].FontData = NULL;
        }

    // When clearing this we lose access to the font name and other information used to build the font.
    for (int i = 0; i < Fonts.Size; i++)
        if (Fonts[i]->ConfigData >= ConfigData.Data && Fonts[i]->ConfigData < ConfigData.Data + ConfigData.Size)
        {
            Fonts[i]->ConfigData = NULL;
            Fonts[i]->ConfigDataCount = 0;
        }
    ConfigData.clear();
    CustomRects.clear();
    for (int n = 0; n < IM_ARRAYSIZE(CustomRectIds); n++)
        CustomRectIds[n] = -1;
}

void    ImFontAtlas::ClearTexData()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    if (TexPixelsAlpha8)
        IM_FREE(TexPixelsAlpha8);
    if (TexPixelsRGBA32)
        IM_FREE(TexPixelsRGBA32);
    TexPixelsAlpha8 = NULL;
    TexPixelsRGBA32 = NULL;
}

void    ImFontAtlas::ClearFonts()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    for (int i = 0; i < Fonts.Size; i++)
        IM_DELETE(Fonts[i]);
    Fonts.clear();
}

void    ImFontAtlas::Clear()
{
    ClearInputData();
    ClearTexData();
    ClearFonts();
}

void    ImFontAtlas::GetTexDataAsAlpha8(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
    // Build atlas on demand
    if (TexPixelsAlpha8 == NULL)
    {
        if (ConfigData.empty())
            AddFontDefault();
        Build();
    }

    *out_pixels = TexPixelsAlpha8;
    if (out_width)* out_width = TexWidth;
    if (out_height)* out_height = TexHeight;
    if (out_bytes_per_pixel)* out_bytes_per_pixel = 1;
}

void    ImFontAtlas::GetTexDataAsRGBA32(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
    // Convert to RGBA32 format on demand
    // Although it is likely to be the most commonly used format, our font rendering is 1 channel / 8 bpp
    if (!TexPixelsRGBA32)
    {
        unsigned char* pixels = NULL;
        GetTexDataAsAlpha8(&pixels, NULL, NULL);
        if (pixels)
        {
            TexPixelsRGBA32 = (unsigned int*)IM_ALLOC((size_t)TexWidth * (size_t)TexHeight * 4);
            const unsigned char* src = pixels;
            unsigned int* dst = TexPixelsRGBA32;
            for (int n = TexWidth * TexHeight; n > 0; n--)
                * dst++ = IM_COL32(255, 255, 255, (unsigned int)(*src++));
        }
    }

    *out_pixels = (unsigned char*)TexPixelsRGBA32;
    if (out_width)* out_width = TexWidth;
    if (out_height)* out_height = TexHeight;
    if (out_bytes_per_pixel)* out_bytes_per_pixel = 4;
}

ImFont* ImFontAtlas::AddFont(const ImFontConfig * font_cfg)
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    IM_ASSERT(font_cfg->FontData != NULL && font_cfg->FontDataSize > 0);
    IM_ASSERT(font_cfg->SizePixels > 0.0f);

    // Create new font
    if (!font_cfg->MergeMode)
        Fonts.push_back(IM_NEW(ImFont));
    else
        IM_ASSERT(!Fonts.empty() && "Cannot use MergeMode for the first font"); // When using MergeMode make sure that a font has already been added before. You can use HanaLovesMe::GetIO().Fonts->AddFontDefault() to add the default HanaLovesMe font.

    ConfigData.push_back(*font_cfg);
    ImFontConfig & new_font_cfg = ConfigData.back();
    if (new_font_cfg.DstFont == NULL)
        new_font_cfg.DstFont = Fonts.back();
    if (!new_font_cfg.FontDataOwnedByAtlas)
    {
        new_font_cfg.FontData = IM_ALLOC(new_font_cfg.FontDataSize);
        new_font_cfg.FontDataOwnedByAtlas = true;
        memcpy(new_font_cfg.FontData, font_cfg->FontData, (size_t)new_font_cfg.FontDataSize);
    }

    // Invalidate texture
    ClearTexData();
    return new_font_cfg.DstFont;
}

// Default font TTF is compressed with stb_compress then base85 encoded (see misc/fonts/binary_to_compressed_c.cpp for encoder)
static unsigned int stb_decompress_length(const unsigned char* input);
static unsigned int stb_decompress(unsigned char* output, const unsigned char* input, unsigned int length);
static const char* GetDefaultCompressedFontDataTTFBase85();
static unsigned int Decode85Byte(char c) { return c >= '\\' ? c - 36 : c - 35; }
static void         Decode85(const unsigned char* src, unsigned char* dst)
{
    while (*src)
    {
        unsigned int tmp = Decode85Byte(src[0]) + 85 * (Decode85Byte(src[1]) + 85 * (Decode85Byte(src[2]) + 85 * (Decode85Byte(src[3]) + 85 * Decode85Byte(src[4]))));
        dst[0] = ((tmp >> 0) & 0xFF); dst[1] = ((tmp >> 8) & 0xFF); dst[2] = ((tmp >> 16) & 0xFF); dst[3] = ((tmp >> 24) & 0xFF);   // We can't assume little-endianness.
        src += 5;
        dst += 4;
    }
}

// Load embedded ProggyClean.ttf at size 13, disable oversampling
ImFont * ImFontAtlas::AddFontDefault(const ImFontConfig * font_cfg_template)
{
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    if (!font_cfg_template)
    {
        font_cfg.OversampleH = font_cfg.OversampleV = 1;
        font_cfg.PixelSnapH = true;
    }
    if (font_cfg.SizePixels <= 0.0f)
        font_cfg.SizePixels = 13.0f * 1.0f;
    if (font_cfg.Name[0] == '\0')
        ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "ProggyClean.ttf, %dpx", (int)font_cfg.SizePixels);

    const char* ttf_compressed_base85 = GetDefaultCompressedFontDataTTFBase85();
    const ImWchar * glyph_ranges = font_cfg.GlyphRanges != NULL ? font_cfg.GlyphRanges : GetGlyphRangesDefault();
    ImFont * font = AddFontFromMemoryCompressedBase85TTF(ttf_compressed_base85, font_cfg.SizePixels, &font_cfg, glyph_ranges);
    font->DisplayOffset.y = 1.0f;
    return font;
}

ImFont * ImFontAtlas::AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig * font_cfg_template, const ImWchar * glyph_ranges)
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    size_t data_size = 0;
    void* data = ImFileLoadToMemory(filename, "rb", &data_size, 0);
    if (!data)
    {
        IM_ASSERT(0); // Could not load file.
        return NULL;
    }
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    if (font_cfg.Name[0] == '\0')
    {
        // Store a short copy of filename into into the font name for convenience
        const char* p;
        for (p = filename + strlen(filename); p > filename && p[-1] != '/' && p[-1] != '\\'; p--) {}
        ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "%s, %.0fpx", p, size_pixels);
    }
    return AddFontFromMemoryTTF(data, (int)data_size, size_pixels, &font_cfg, glyph_ranges);
}

// NB: Transfer ownership of 'ttf_data' to ImFontAtlas, unless font_cfg_template->FontDataOwnedByAtlas == false. Owned TTF buffer will be deleted after Build().
ImFont* ImFontAtlas::AddFontFromMemoryTTF(void* ttf_data, int ttf_size, float size_pixels, const ImFontConfig * font_cfg_template, const ImWchar * glyph_ranges)
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    IM_ASSERT(font_cfg.FontData == NULL);
    font_cfg.FontData = ttf_data;
    font_cfg.FontDataSize = ttf_size;
    font_cfg.SizePixels = size_pixels;
    if (glyph_ranges)
        font_cfg.GlyphRanges = glyph_ranges;
    return AddFont(&font_cfg);
}

ImFont* ImFontAtlas::AddFontFromMemoryCompressedTTF(const void* compressed_ttf_data, int compressed_ttf_size, float size_pixels, const ImFontConfig * font_cfg_template, const ImWchar * glyph_ranges)
{
    const unsigned int buf_decompressed_size = stb_decompress_length((const unsigned char*)compressed_ttf_data);
    unsigned char* buf_decompressed_data = (unsigned char*)IM_ALLOC(buf_decompressed_size);
    stb_decompress(buf_decompressed_data, (const unsigned char*)compressed_ttf_data, (unsigned int)compressed_ttf_size);

    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    IM_ASSERT(font_cfg.FontData == NULL);
    font_cfg.FontDataOwnedByAtlas = true;
    return AddFontFromMemoryTTF(buf_decompressed_data, (int)buf_decompressed_size, size_pixels, &font_cfg, glyph_ranges);
}

ImFont* ImFontAtlas::AddFontFromMemoryCompressedBase85TTF(const char* compressed_ttf_data_base85, float size_pixels, const ImFontConfig * font_cfg, const ImWchar * glyph_ranges)
{
    int compressed_ttf_size = (((int)strlen(compressed_ttf_data_base85) + 4) / 5) * 4;
    void* compressed_ttf = IM_ALLOC((size_t)compressed_ttf_size);
    Decode85((const unsigned char*)compressed_ttf_data_base85, (unsigned char*)compressed_ttf);
    ImFont * font = AddFontFromMemoryCompressedTTF(compressed_ttf, compressed_ttf_size, size_pixels, font_cfg, glyph_ranges);
    IM_FREE(compressed_ttf);
    return font;
}

int ImFontAtlas::AddCustomRectRegular(unsigned int id, int width, int height)
{
    IM_ASSERT(id >= 0x10000);
    IM_ASSERT(width > 0 && width <= 0xFFFF);
    IM_ASSERT(height > 0 && height <= 0xFFFF);
    CustomRect r;
    r.ID = id;
    r.Width = (unsigned short)width;
    r.Height = (unsigned short)height;
    CustomRects.push_back(r);
    return CustomRects.Size - 1; // Return index
}

int ImFontAtlas::AddCustomRectFontGlyph(ImFont * font, ImWchar id, int width, int height, float advance_x, const vsize & offset)
{
    IM_ASSERT(font != NULL);
    IM_ASSERT(width > 0 && width <= 0xFFFF);
    IM_ASSERT(height > 0 && height <= 0xFFFF);
    CustomRect r;
    r.ID = id;
    r.Width = (unsigned short)width;
    r.Height = (unsigned short)height;
    r.GlyphAdvanceX = advance_x;
    r.GlyphOffset = offset;
    r.Font = font;
    CustomRects.push_back(r);
    return CustomRects.Size - 1; // Return index
}

void ImFontAtlas::CalcCustomRectUV(const CustomRect * rect, vsize * out_uv_min, vsize * out_uv_max)
{
    IM_ASSERT(TexWidth > 0 && TexHeight > 0);   // Font atlas needs to be built before we can calculate UV coordinates
    IM_ASSERT(rect->IsPacked());                // Make sure the rectangle has been packed
    *out_uv_min = vsize((float)rect->X * TexUvScale.x, (float)rect->Y * TexUvScale.y);
    *out_uv_max = vsize((float)(rect->X + rect->Width) * TexUvScale.x, (float)(rect->Y + rect->Height) * TexUvScale.y);
}

bool ImFontAtlas::GetMouseCursorTexData(HanaLovesMeMouseCursor cursor_type, vsize * out_offset, vsize * out_size, vsize out_uv_border[2], vsize out_uv_fill[2])
{
    if (cursor_type <= HanaLovesMeMouseCursor_None || cursor_type >= HanaLovesMeMouseCursor_COUNT)
        return false;
    if (Flags & ImFontAtlasFlags_NoMouseCursors)
        return false;

    IM_ASSERT(CustomRectIds[0] != -1);
    ImFontAtlas::CustomRect & r = CustomRects[CustomRectIds[0]];
    IM_ASSERT(r.ID == FONT_ATLAS_DEFAULT_TEX_DATA_ID);
    vsize pos = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][0] + vsize((float)r.X, (float)r.Y);
    vsize size = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][1];
    *out_size = size;
    *out_offset = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][2];
    out_uv_border[0] = (pos)* TexUvScale;
    out_uv_border[1] = (pos + size) * TexUvScale;
    pos.x += FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF + 1;
    out_uv_fill[0] = (pos)* TexUvScale;
    out_uv_fill[1] = (pos + size) * TexUvScale;
    return true;
}

bool    ImFontAtlas::Build()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    return ImFontAtlasBuildWithStbTruetype(this);
}

void    ImFontAtlasBuildMultiplyCalcLookupTable(unsigned char out_table[256], float in_brighten_factor)
{
    for (unsigned int i = 0; i < 256; i++)
    {
        unsigned int value = (unsigned int)(i * in_brighten_factor);
        out_table[i] = value > 255 ? 255 : (value & 0xFF);
    }
}

void    ImFontAtlasBuildMultiplyRectAlpha8(const unsigned char table[256], unsigned char* pixels, int x, int y, int w, int h, int stride)
{
    unsigned char* data = pixels + x + y * stride;
    for (int j = h; j > 0; j--, data += stride)
        for (int i = 0; i < w; i++)
            data[i] = table[data[i]];
}

// Temporary data for one source font (multiple source fonts can be merged into one destination ImFont)
// (C++03 doesn't allow instancing ImVector<> with function-local types so we declare the type here.)
struct ImFontBuildSrcData
{
    stbtt_fontinfo      FontInfo;
    stbtt_pack_range    PackRange;          // Hold the list of codepoints to pack (essentially points to Codepoints.Data)
    stbrp_rect* Rects;              // Rectangle to pack. We first fill in their size and the packer will give us their position.
    stbtt_packedchar* PackedChars;        // Output glyphs
    const ImWchar* SrcRanges;          // Ranges as requested by user (user is allowed to request too much, e.g. 0x0020..0xFFFF)
    int                 DstIndex;           // Index into atlas->Fonts[] and dst_tmp_array[]
    int                 GlyphsHighest;      // Highest requested codepoint
    int                 GlyphsCount;        // Glyph count (excluding missing glyphs and glyphs already set by an earlier source font)
    ImBoolVector        GlyphsSet;          // Glyph bit map (random access, 1-bit per codepoint. This will be a maximum of 8KB)
    ImVector<int>       GlyphsList;         // Glyph codepoints list (flattened version of GlyphsMap)
};

// Temporary data for one destination ImFont* (multiple source fonts can be merged into one destination ImFont)
struct ImFontBuildDstData
{
    int                 SrcCount;           // Number of source fonts targeting this destination font.
    int                 GlyphsHighest;
    int                 GlyphsCount;
    ImBoolVector        GlyphsSet;          // This is used to resolve collision when multiple sources are merged into a same destination font.
};

static void UnpackBoolVectorToFlatIndexList(const ImBoolVector * in, ImVector<int> * out)
{
    IM_ASSERT(sizeof(in->Stohnly.Data[0]) == sizeof(int));
    const int* it_begin = in->Stohnly.begin();
    const int* it_end = in->Stohnly.end();
    for (const int* it = it_begin; it < it_end; it++)
        if (int entries_32 = *it)
            for (int bit_n = 0; bit_n < 32; bit_n++)
                if (entries_32 & (1 << bit_n))
                    out->push_back((int)((it - it_begin) << 5) + bit_n);
}

bool    ImFontAtlasBuildWithStbTruetype(ImFontAtlas * atlas)
{
    IM_ASSERT(atlas->ConfigData.Size > 0);

    ImFontAtlasBuildRegisterDefaultCustomRects(atlas);

    // Clear atlas
    atlas->TexID = (ImTextureID)NULL;
    atlas->TexWidth = atlas->TexHeight = 0;
    atlas->TexUvScale = vsize(0.0f, 0.0f);
    atlas->TexUvWhitePixel = vsize(0.0f, 0.0f);
    atlas->ClearTexData();

    // Temporary stohnly for building
    ImVector<ImFontBuildSrcData> src_tmp_array;
    ImVector<ImFontBuildDstData> dst_tmp_array;
    src_tmp_array.resize(atlas->ConfigData.Size);
    dst_tmp_array.resize(atlas->Fonts.Size);
    memset(src_tmp_array.Data, 0, (size_t)src_tmp_array.size_in_bytes());
    memset(dst_tmp_array.Data, 0, (size_t)dst_tmp_array.size_in_bytes());

    // 1. Initialize font loading structure, check font data validity
    for (int src_i = 0; src_i < atlas->ConfigData.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        ImFontConfig& cfg = atlas->ConfigData[src_i];
        IM_ASSERT(cfg.DstFont && (!cfg.DstFont->IsLoaded() || cfg.DstFont->ContainerAtlas == atlas));

        // Find index from cfg.DstFont (we allow the user to set cfg.DstFont. Also it makes casual debugging nicer than when storing indices)
        src_tmp.DstIndex = -1;
        for (int output_i = 0; output_i < atlas->Fonts.Size && src_tmp.DstIndex == -1; output_i++)
            if (cfg.DstFont == atlas->Fonts[output_i])
                src_tmp.DstIndex = output_i;
        IM_ASSERT(src_tmp.DstIndex != -1); // cfg.DstFont not pointing within atlas->Fonts[] array?
        if (src_tmp.DstIndex == -1)
            return false;

        // Initialize helper structure for font loading and verify that the TTF/OTF data is correct
        const int font_offset = stbtt_GetFontOffsetForIndex((unsigned char*)cfg.FontData, cfg.FontNo);
        IM_ASSERT(font_offset >= 0 && "FontData is incorrect, or FontNo cannot be found.");
        if (!stbtt_InitFont(&src_tmp.FontInfo, (unsigned char*)cfg.FontData, font_offset))
            return false;

        // Measure highest codepoints
        ImFontBuildDstData & dst_tmp = dst_tmp_array[src_tmp.DstIndex];
        src_tmp.SrcRanges = cfg.GlyphRanges ? cfg.GlyphRanges : atlas->GetGlyphRangesDefault();
        for (const ImWchar* src_range = src_tmp.SrcRanges; src_range[0] && src_range[1]; src_range += 2)
            src_tmp.GlyphsHighest = ImMax(src_tmp.GlyphsHighest, (int)src_range[1]);
        dst_tmp.SrcCount++;
        dst_tmp.GlyphsHighest = ImMax(dst_tmp.GlyphsHighest, src_tmp.GlyphsHighest);
    }

    // 2. For every requested codepoint, check for their presence in the font data, and handle redundancy or overlaps between source fonts to avoid unused glyphs.
    int total_glyphs_count = 0;
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        ImFontBuildDstData& dst_tmp = dst_tmp_array[src_tmp.DstIndex];
        src_tmp.GlyphsSet.Resize(src_tmp.GlyphsHighest + 1);
        if (dst_tmp.GlyphsSet.Stohnly.empty())
            dst_tmp.GlyphsSet.Resize(dst_tmp.GlyphsHighest + 1);

        for (const ImWchar* src_range = src_tmp.SrcRanges; src_range[0] && src_range[1]; src_range += 2)
            for (int codepoint = src_range[0]; codepoint <= src_range[1]; codepoint++)
            {
                if (dst_tmp.GlyphsSet.GetBit(codepoint))    // Don't overwrite existing glyphs. We could make this an option for MergeMode (e.g. MergeOverwrite==true)
                    continue;
                if (!stbtt_FindGlyphIndex(&src_tmp.FontInfo, codepoint))    // It is actually in the font?
                    continue;

                // Add to avail set/counters
                src_tmp.GlyphsCount++;
                dst_tmp.GlyphsCount++;
                src_tmp.GlyphsSet.SetBit(codepoint, true);
                dst_tmp.GlyphsSet.SetBit(codepoint, true);
                total_glyphs_count++;
            }
    }

    // 3. Unpack our bit map into a flat list (we now have all the Unicode points that we know are requested _and_ available _and_ not overlapping another)
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        src_tmp.GlyphsList.reserve(src_tmp.GlyphsCount);
        UnpackBoolVectorToFlatIndexList(&src_tmp.GlyphsSet, &src_tmp.GlyphsList);
        src_tmp.GlyphsSet.Clear();
        IM_ASSERT(src_tmp.GlyphsList.Size == src_tmp.GlyphsCount);
    }
    for (int dst_i = 0; dst_i < dst_tmp_array.Size; dst_i++)
        dst_tmp_array[dst_i].GlyphsSet.Clear();
    dst_tmp_array.clear();

    // Allocate packing character data and flag packed characters buffer as non-packed (x0=y0=x1=y1=0)
    // (We technically don't need to zero-clear buf_rects, but let's do it for the sake of sanity)
    ImVector<stbrp_rect> buf_rects;
    ImVector<stbtt_packedchar> buf_packedchars;
    buf_rects.resize(total_glyphs_count);
    buf_packedchars.resize(total_glyphs_count);
    memset(buf_rects.Data, 0, (size_t)buf_rects.size_in_bytes());
    memset(buf_packedchars.Data, 0, (size_t)buf_packedchars.size_in_bytes());

    // 4. Gather glyphs sizes so we can pack them in our virtual canvas.
    int total_surface = 0;
    int buf_rects_out_n = 0;
    int buf_packedchars_out_n = 0;
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        src_tmp.Rects = &buf_rects[buf_rects_out_n];
        src_tmp.PackedChars = &buf_packedchars[buf_packedchars_out_n];
        buf_rects_out_n += src_tmp.GlyphsCount;
        buf_packedchars_out_n += src_tmp.GlyphsCount;

        // Convert our ranges in the format stb_truetype wants
        ImFontConfig & cfg = atlas->ConfigData[src_i];
        src_tmp.PackRange.font_size = cfg.SizePixels;
        src_tmp.PackRange.first_unicode_codepoint_in_range = 0;
        src_tmp.PackRange.array_of_unicode_codepoints = src_tmp.GlyphsList.Data;
        src_tmp.PackRange.num_chars = src_tmp.GlyphsList.Size;
        src_tmp.PackRange.chardata_for_range = src_tmp.PackedChars;
        src_tmp.PackRange.h_oversample = (unsigned char)cfg.OversampleH;
        src_tmp.PackRange.v_oversample = (unsigned char)cfg.OversampleV;

        // Gather the sizes of all rectangles we will need to pack (this loop is based on stbtt_PackFontRangesGatherRects)
        const float scale = (cfg.SizePixels > 0) ? stbtt_ScaleForPixelHeight(&src_tmp.FontInfo, cfg.SizePixels) : stbtt_ScaleForMappingEmToPixels(&src_tmp.FontInfo, -cfg.SizePixels);
        const int padding = atlas->TexGlyphPadding;
        for (int glyph_i = 0; glyph_i < src_tmp.GlyphsList.Size; glyph_i++)
        {
            int x0, y0, x1, y1;
            const int glyph_index_in_font = stbtt_FindGlyphIndex(&src_tmp.FontInfo, src_tmp.GlyphsList[glyph_i]);
            IM_ASSERT(glyph_index_in_font != 0);
            stbtt_GetGlyphBitmapBoxSubpixel(&src_tmp.FontInfo, glyph_index_in_font, scale * cfg.OversampleH, scale * cfg.OversampleV, 0, 0, &x0, &y0, &x1, &y1);
            src_tmp.Rects[glyph_i].w = (stbrp_coord)(x1 - x0 + padding + cfg.OversampleH - 1);
            src_tmp.Rects[glyph_i].h = (stbrp_coord)(y1 - y0 + padding + cfg.OversampleV - 1);
            total_surface += src_tmp.Rects[glyph_i].w * src_tmp.Rects[glyph_i].h;
        }
    }

    // We need a width for the skyline algorithm, any width!
    // The exact width doesn't really matter much, but some API/GPU have texture size limitations and increasing width can decrease height.
    // User can override TexDesiredWidth and TexGlyphPadding if they wish, otherwise we use a simple heuristic to select the width based on expected surface.
    const int surface_sqrt = (int)ImSqrt((float)total_surface) + 1;
    atlas->TexHeight = 0;
    if (atlas->TexDesiredWidth > 0)
        atlas->TexWidth = atlas->TexDesiredWidth;
    else
        atlas->TexWidth = (surface_sqrt >= 4096 * 0.7f) ? 4096 : (surface_sqrt >= 2048 * 0.7f) ? 2048 : (surface_sqrt >= 1024 * 0.7f) ? 1024 : 512;

    // 5. Start packing
    // Pack our extra data rectangles first, so it will be on the upper-left corner of our texture (UV will have small values).
    const int TEX_HEIGHT_MAX = 1024 * 32;
    stbtt_pack_context spc = {};
    stbtt_PackBegin(&spc, NULL, atlas->TexWidth, TEX_HEIGHT_MAX, 0, atlas->TexGlyphPadding, NULL);
    ImFontAtlasBuildPackCustomRects(atlas, spc.pack_info);

    // 6. Pack each source font. No rendering yet, we are working with rectangles in an infinitely tall texture at this point.
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        stbrp_pack_rects((stbrp_context*)spc.pack_info, src_tmp.Rects, src_tmp.GlyphsCount);

        // Extend texture height and mark missing glyphs as non-packed so we won't render them.
        // FIXME: We are not handling packing failure here (would happen if we got off TEX_HEIGHT_MAX or if a single if larger than TexWidth?)
        for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++)
            if (src_tmp.Rects[glyph_i].was_packed)
                atlas->TexHeight = ImMax(atlas->TexHeight, src_tmp.Rects[glyph_i].y + src_tmp.Rects[glyph_i].h);
    }

    // 7. Allocate texture
    atlas->TexHeight = (atlas->Flags & ImFontAtlasFlags_NoPowerOfTwoHeight) ? (atlas->TexHeight + 1) : ImUpperPowerOfTwo(atlas->TexHeight);
    atlas->TexUvScale = vsize(1.0f / atlas->TexWidth, 1.0f / atlas->TexHeight);
    atlas->TexPixelsAlpha8 = (unsigned char*)IM_ALLOC(atlas->TexWidth * atlas->TexHeight);
    memset(atlas->TexPixelsAlpha8, 0, atlas->TexWidth * atlas->TexHeight);
    spc.pixels = atlas->TexPixelsAlpha8;
    spc.height = atlas->TexHeight;

    // 8. Render/rasterize font characters into the texture
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontConfig& cfg = atlas->ConfigData[src_i];
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        stbtt_PackFontRangesRenderIntoRects(&spc, &src_tmp.FontInfo, &src_tmp.PackRange, 1, src_tmp.Rects);

        // Apply multiply operator
        if (cfg.RasterizerMultiply != 1.0f)
        {
            unsigned char multiply_table[256];
            ImFontAtlasBuildMultiplyCalcLookupTable(multiply_table, cfg.RasterizerMultiply);
            stbrp_rect* r = &src_tmp.Rects[0];
            for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++, r++)
                if (r->was_packed)
                    ImFontAtlasBuildMultiplyRectAlpha8(multiply_table, atlas->TexPixelsAlpha8, r->x, r->y, r->w, r->h, atlas->TexWidth * 1);
        }
        src_tmp.Rects = NULL;
    }

    // End packing
    stbtt_PackEnd(&spc);
    buf_rects.clear();

    // 9. Setup ImFont and glyphs for runtime
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        ImFontConfig & cfg = atlas->ConfigData[src_i];
        ImFont * dst_font = cfg.DstFont; // We can have multiple input fonts writing into a same destination font (when using MergeMode=true)

        const float font_scale = stbtt_ScaleForPixelHeight(&src_tmp.FontInfo, cfg.SizePixels);
        int unscaled_ascent, unscaled_descent, unscaled_line_gap;
        stbtt_GetFontVMetrics(&src_tmp.FontInfo, &unscaled_ascent, &unscaled_descent, &unscaled_line_gap);

        const float ascent = ImFloor(unscaled_ascent * font_scale + ((unscaled_ascent > 0.0f) ? +1 : -1));
        const float descent = ImFloor(unscaled_descent * font_scale + ((unscaled_descent > 0.0f) ? +1 : -1));
        ImFontAtlasBuildSetupFont(atlas, dst_font, &cfg, ascent, descent);
        const float font_off_x = cfg.GlyphOffset.x;
        const float font_off_y = cfg.GlyphOffset.y + (float)(int)(dst_font->Ascent + 0.5f);

        for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++)
        {
            const int codepoint = src_tmp.GlyphsList[glyph_i];
            const stbtt_packedchar& pc = src_tmp.PackedChars[glyph_i];

            const float char_advance_x_org = pc.xadvance;
            const float char_advance_x_mod = ImClamp(char_advance_x_org, cfg.GlyphMinAdvanceX, cfg.GlyphMaxAdvanceX);
            float char_off_x = font_off_x;
            if (char_advance_x_org != char_advance_x_mod)
                char_off_x += cfg.PixelSnapH ? (float)(int)((char_advance_x_mod - char_advance_x_org) * 0.5f) : (char_advance_x_mod - char_advance_x_org) * 0.5f;

            // Register glyph
            stbtt_aligned_quad q;
            float dummy_x = 0.0f, dummy_y = 0.0f;
            stbtt_GetPackedQuad(src_tmp.PackedChars, atlas->TexWidth, atlas->TexHeight, glyph_i, &dummy_x, &dummy_y, &q, 0);
            dst_font->AddGlyph((ImWchar)codepoint, q.x0 + char_off_x, q.y0 + font_off_y, q.x1 + char_off_x, q.y1 + font_off_y, q.s0, q.t0, q.s1, q.t1, char_advance_x_mod);
        }
    }

    // Cleanup temporary (ImVector doesn't honor destructor)
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
        src_tmp_array[src_i].~ImFontBuildSrcData();

    ImFontAtlasBuildFinish(atlas);
    return true;
}

void ImFontAtlasBuildRegisterDefaultCustomRects(ImFontAtlas * atlas)
{
    if (atlas->CustomRectIds[0] >= 0)
        return;
    if (!(atlas->Flags & ImFontAtlasFlags_NoMouseCursors))
        atlas->CustomRectIds[0] = atlas->AddCustomRectRegular(FONT_ATLAS_DEFAULT_TEX_DATA_ID, FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF * 2 + 1, FONT_ATLAS_DEFAULT_TEX_DATA_H);
    else
        atlas->CustomRectIds[0] = atlas->AddCustomRectRegular(FONT_ATLAS_DEFAULT_TEX_DATA_ID, 2, 2);
}

void ImFontAtlasBuildSetupFont(ImFontAtlas * atlas, ImFont * font, ImFontConfig * font_config, float ascent, float descent)
{
    if (!font_config->MergeMode)
    {
        font->ClearOutputData();
        font->FontSize = font_config->SizePixels;
        font->ConfigData = font_config;
        font->ContainerAtlas = atlas;
        font->Ascent = ascent;
        font->Descent = descent;
    }
    font->ConfigDataCount++;
}

void ImFontAtlasBuildPackCustomRects(ImFontAtlas * atlas, void* stbrp_context_opaque)
{
    stbrp_context* pack_context = (stbrp_context*)stbrp_context_opaque;
    IM_ASSERT(pack_context != NULL);

    ImVector<ImFontAtlas::CustomRect> & user_rects = atlas->CustomRects;
    IM_ASSERT(user_rects.Size >= 1); // We expect at least the default custom rects to be registered, else something went wrong.

    ImVector<stbrp_rect> pack_rects;
    pack_rects.resize(user_rects.Size);
    memset(pack_rects.Data, 0, (size_t)pack_rects.size_in_bytes());
    for (int i = 0; i < user_rects.Size; i++)
    {
        pack_rects[i].w = user_rects[i].Width;
        pack_rects[i].h = user_rects[i].Height;
    }
    stbrp_pack_rects(pack_context, &pack_rects[0], pack_rects.Size);
    for (int i = 0; i < pack_rects.Size; i++)
        if (pack_rects[i].was_packed)
        {
            user_rects[i].X = pack_rects[i].x;
            user_rects[i].Y = pack_rects[i].y;
            IM_ASSERT(pack_rects[i].w == user_rects[i].Width && pack_rects[i].h == user_rects[i].Height);
            atlas->TexHeight = ImMax(atlas->TexHeight, pack_rects[i].y + pack_rects[i].h);
        }
}

static void ImFontAtlasBuildRenderDefaultTexData(ImFontAtlas * atlas)
{
    IM_ASSERT(atlas->CustomRectIds[0] >= 0);
    IM_ASSERT(atlas->TexPixelsAlpha8 != NULL);
    ImFontAtlas::CustomRect & r = atlas->CustomRects[atlas->CustomRectIds[0]];
    IM_ASSERT(r.ID == FONT_ATLAS_DEFAULT_TEX_DATA_ID);
    IM_ASSERT(r.IsPacked());

    const int w = atlas->TexWidth;
    if (!(atlas->Flags & ImFontAtlasFlags_NoMouseCursors))
    {
        // Render/copy pixels
        IM_ASSERT(r.Width == FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF * 2 + 1 && r.Height == FONT_ATLAS_DEFAULT_TEX_DATA_H);
        for (int y = 0, n = 0; y < FONT_ATLAS_DEFAULT_TEX_DATA_H; y++)
            for (int x = 0; x < FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF; x++, n++)
            {
                const int offset0 = (int)(r.X + x) + (int)(r.Y + y) * w;
                const int offset1 = offset0 + FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF + 1;
                atlas->TexPixelsAlpha8[offset0] = FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS[n] == '.' ? 0xFF : 0x00;
                atlas->TexPixelsAlpha8[offset1] = FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS[n] == 'X' ? 0xFF : 0x00;
            }
    } else
    {
        IM_ASSERT(r.Width == 2 && r.Height == 2);
        const int offset = (int)(r.X) + (int)(r.Y) * w;
        atlas->TexPixelsAlpha8[offset] = atlas->TexPixelsAlpha8[offset + 1] = atlas->TexPixelsAlpha8[offset + w] = atlas->TexPixelsAlpha8[offset + w + 1] = 0xFF;
    }
    atlas->TexUvWhitePixel = vsize((r.X + 0.5f) * atlas->TexUvScale.x, (r.Y + 0.5f) * atlas->TexUvScale.y);
}

void ImFontAtlasBuildFinish(ImFontAtlas * atlas)
{
    // Render into our custom data block
    ImFontAtlasBuildRenderDefaultTexData(atlas);

    // Register custom rectangle glyphs
    for (int i = 0; i < atlas->CustomRects.Size; i++)
    {
        const ImFontAtlas::CustomRect& r = atlas->CustomRects[i];
        if (r.Font == NULL || r.ID > 0x10000)
            continue;

        IM_ASSERT(r.Font->ContainerAtlas == atlas);
        vsize uv0, uv1;
        atlas->CalcCustomRectUV(&r, &uv0, &uv1);
        r.Font->AddGlyph((ImWchar)r.ID, r.GlyphOffset.x, r.GlyphOffset.y, r.GlyphOffset.x + r.Width, r.GlyphOffset.y + r.Height, uv0.x, uv0.y, uv1.x, uv1.y, r.GlyphAdvanceX);
    }

    // Build all fonts lookup tables
    for (int i = 0; i < atlas->Fonts.Size; i++)
        if (atlas->Fonts[i]->DirtyLookupTables)
            atlas->Fonts[i]->BuildLookupTable();
}

// Retrieve list of range (2 int per range, values are inclusive)
const ImWchar * ImFontAtlas::GetGlyphRangesDefault()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x2122, 0x2122, // �
		0xC2, 0xB0, // �
        0,
    };
    return &ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesKorean()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x3131, 0x3163, // Korean alphabets
        0xAC00, 0xD79D, // Korean characters
        0,
    };
    return &ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesChineseFull()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x2000, 0x206F, // General Punctuation
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width characters
        0x4e00, 0x9FAF, // CJK Ideograms
        0,
    };
    return &ranges[0];
}

static void UnpackAccumulativeOffsetsIntoRanges(int base_codepoint, const short* accumulative_offsets, int accumulative_offsets_count, ImWchar * out_ranges)
{
    for (int n = 0; n < accumulative_offsets_count; n++, out_ranges += 2)
    {
        out_ranges[0] = out_ranges[1] = (ImWchar)(base_codepoint + accumulative_offsets[n]);
        base_codepoint += accumulative_offsets[n];
    }
    out_ranges[0] = 0;
}

//-------------------------------------------------------------------------
// [SECTION] ImFontAtlas glyph ranges helpers
//-------------------------------------------------------------------------

const ImWchar* ImFontAtlas::GetGlyphRangesChineseSimplifiedCommon()
{
    // Store 2500 regularly used characters for Simplified Chinese.
    // Sourced from https://zh.wiktionary.org/wiki/%E9%99%84%E5%BD%95:%E7%8E%B0%E4%BB%A3%E6%B1%89%E8%AF%AD%E5%B8%B8%E7%94%A8%E5%AD%97%E8%A1%A8
    // This table covers 97.97% of all characters used during the month in July, 1987.
    // You can use ImFontGlyphRangesBuilder to create your own ranges derived from this, by merging existing ranges or adding new characters.
    // (Stored as accumulative offsets from the initial unicode codepoint 0x4E00. This encoding is designed to helps us compact the source code size.)
    static const short accumulative_offsets_from_0x4E00[] =
    {
        0,1,2,4,1,1,1,1,2,1,3,2,1,2,2,1,1,1,1,1,5,2,1,2,3,3,3,2,2,4,1,1,1,2,1,5,2,3,1,2,1,2,1,1,2,1,1,2,2,1,4,1,1,1,1,5,10,1,2,19,2,1,2,1,2,1,2,1,2,
        1,5,1,6,3,2,1,2,2,1,1,1,4,8,5,1,1,4,1,1,3,1,2,1,5,1,2,1,1,1,10,1,1,5,2,4,6,1,4,2,2,2,12,2,1,1,6,1,1,1,4,1,1,4,6,5,1,4,2,2,4,10,7,1,1,4,2,4,
        2,1,4,3,6,10,12,5,7,2,14,2,9,1,1,6,7,10,4,7,13,1,5,4,8,4,1,1,2,28,5,6,1,1,5,2,5,20,2,2,9,8,11,2,9,17,1,8,6,8,27,4,6,9,20,11,27,6,68,2,2,1,1,
        1,2,1,2,2,7,6,11,3,3,1,1,3,1,2,1,1,1,1,1,3,1,1,8,3,4,1,5,7,2,1,4,4,8,4,2,1,2,1,1,4,5,6,3,6,2,12,3,1,3,9,2,4,3,4,1,5,3,3,1,3,7,1,5,1,1,1,1,2,
        3,4,5,2,3,2,6,1,1,2,1,7,1,7,3,4,5,15,2,2,1,5,3,22,19,2,1,1,1,1,2,5,1,1,1,6,1,1,12,8,2,9,18,22,4,1,1,5,1,16,1,2,7,10,15,1,1,6,2,4,1,2,4,1,6,
        1,1,3,2,4,1,6,4,5,1,2,1,1,2,1,10,3,1,3,2,1,9,3,2,5,7,2,19,4,3,6,1,1,1,1,1,4,3,2,1,1,1,2,5,3,1,1,1,2,2,1,1,2,1,1,2,1,3,1,1,1,3,7,1,4,1,1,2,1,
        1,2,1,2,4,4,3,8,1,1,1,2,1,3,5,1,3,1,3,4,6,2,2,14,4,6,6,11,9,1,15,3,1,28,5,2,5,5,3,1,3,4,5,4,6,14,3,2,3,5,21,2,7,20,10,1,2,19,2,4,28,28,2,3,
        2,1,14,4,1,26,28,42,12,40,3,52,79,5,14,17,3,2,2,11,3,4,6,3,1,8,2,23,4,5,8,10,4,2,7,3,5,1,1,6,3,1,2,2,2,5,28,1,1,7,7,20,5,3,29,3,17,26,1,8,4,
        27,3,6,11,23,5,3,4,6,13,24,16,6,5,10,25,35,7,3,2,3,3,14,3,6,2,6,1,4,2,3,8,2,1,1,3,3,3,4,1,1,13,2,2,4,5,2,1,14,14,1,2,2,1,4,5,2,3,1,14,3,12,
        3,17,2,16,5,1,2,1,8,9,3,19,4,2,2,4,17,25,21,20,28,75,1,10,29,103,4,1,2,1,1,4,2,4,1,2,3,24,2,2,2,1,1,2,1,3,8,1,1,1,2,1,1,3,1,1,1,6,1,5,3,1,1,
        1,3,4,1,1,5,2,1,5,6,13,9,16,1,1,1,1,3,2,3,2,4,5,2,5,2,2,3,7,13,7,2,2,1,1,1,1,2,3,3,2,1,6,4,9,2,1,14,2,14,2,1,18,3,4,14,4,11,41,15,23,15,23,
        176,1,3,4,1,1,1,1,5,3,1,2,3,7,3,1,1,2,1,2,4,4,6,2,4,1,9,7,1,10,5,8,16,29,1,1,2,2,3,1,3,5,2,4,5,4,1,1,2,2,3,3,7,1,6,10,1,17,1,44,4,6,2,1,1,6,
        5,4,2,10,1,6,9,2,8,1,24,1,2,13,7,8,8,2,1,4,1,3,1,3,3,5,2,5,10,9,4,9,12,2,1,6,1,10,1,1,7,7,4,10,8,3,1,13,4,3,1,6,1,3,5,2,1,2,17,16,5,2,16,6,
        1,4,2,1,3,3,6,8,5,11,11,1,3,3,2,4,6,10,9,5,7,4,7,4,7,1,1,4,2,1,3,6,8,7,1,6,11,5,5,3,24,9,4,2,7,13,5,1,8,82,16,61,1,1,1,4,2,2,16,10,3,8,1,1,
        6,4,2,1,3,1,1,1,4,3,8,4,2,2,1,1,1,1,1,6,3,5,1,1,4,6,9,2,1,1,1,2,1,7,2,1,6,1,5,4,4,3,1,8,1,3,3,1,3,2,2,2,2,3,1,6,1,2,1,2,1,3,7,1,8,2,1,2,1,5,
        2,5,3,5,10,1,2,1,1,3,2,5,11,3,9,3,5,1,1,5,9,1,2,1,5,7,9,9,8,1,3,3,3,6,8,2,3,2,1,1,32,6,1,2,15,9,3,7,13,1,3,10,13,2,14,1,13,10,2,1,3,10,4,15,
        2,15,15,10,1,3,9,6,9,32,25,26,47,7,3,2,3,1,6,3,4,3,2,8,5,4,1,9,4,2,2,19,10,6,2,3,8,1,2,2,4,2,1,9,4,4,4,6,4,8,9,2,3,1,1,1,1,3,5,5,1,3,8,4,6,
        2,1,4,12,1,5,3,7,13,2,5,8,1,6,1,2,5,14,6,1,5,2,4,8,15,5,1,23,6,62,2,10,1,1,8,1,2,2,10,4,2,2,9,2,1,1,3,2,3,1,5,3,3,2,1,3,8,1,1,1,11,3,1,1,4,
        3,7,1,14,1,2,3,12,5,2,5,1,6,7,5,7,14,11,1,3,1,8,9,12,2,1,11,8,4,4,2,6,10,9,13,1,1,3,1,5,1,3,2,4,4,1,18,2,3,14,11,4,29,4,2,7,1,3,13,9,2,2,5,
        3,5,20,7,16,8,5,72,34,6,4,22,12,12,28,45,36,9,7,39,9,191,1,1,1,4,11,8,4,9,2,3,22,1,1,1,1,4,17,1,7,7,1,11,31,10,2,4,8,2,3,2,1,4,2,16,4,32,2,
        3,19,13,4,9,1,5,2,14,8,1,1,3,6,19,6,5,1,16,6,2,10,8,5,1,2,3,1,5,5,1,11,6,6,1,3,3,2,6,3,8,1,1,4,10,7,5,7,7,5,8,9,2,1,3,4,1,1,3,1,3,3,2,6,16,
        1,4,6,3,1,10,6,1,3,15,2,9,2,10,25,13,9,16,6,2,2,10,11,4,3,9,1,2,6,6,5,4,30,40,1,10,7,12,14,33,6,3,6,7,3,1,3,1,11,14,4,9,5,12,11,49,18,51,31,
        140,31,2,2,1,5,1,8,1,10,1,4,4,3,24,1,10,1,3,6,6,16,3,4,5,2,1,4,2,57,10,6,22,2,22,3,7,22,6,10,11,36,18,16,33,36,2,5,5,1,1,1,4,10,1,4,13,2,7,
        5,2,9,3,4,1,7,43,3,7,3,9,14,7,9,1,11,1,1,3,7,4,18,13,1,14,1,3,6,10,73,2,2,30,6,1,11,18,19,13,22,3,46,42,37,89,7,3,16,34,2,2,3,9,1,7,1,1,1,2,
        2,4,10,7,3,10,3,9,5,28,9,2,6,13,7,3,1,3,10,2,7,2,11,3,6,21,54,85,2,1,4,2,2,1,39,3,21,2,2,5,1,1,1,4,1,1,3,4,15,1,3,2,4,4,2,3,8,2,20,1,8,7,13,
        4,1,26,6,2,9,34,4,21,52,10,4,4,1,5,12,2,11,1,7,2,30,12,44,2,30,1,1,3,6,16,9,17,39,82,2,2,24,7,1,7,3,16,9,14,44,2,1,2,1,2,3,5,2,4,1,6,7,5,3,
        2,6,1,11,5,11,2,1,18,19,8,1,3,24,29,2,1,3,5,2,2,1,13,6,5,1,46,11,3,5,1,1,5,8,2,10,6,12,6,3,7,11,2,4,16,13,2,5,1,1,2,2,5,2,28,5,2,23,10,8,4,
        4,22,39,95,38,8,14,9,5,1,13,5,4,3,13,12,11,1,9,1,27,37,2,5,4,4,63,211,95,2,2,2,1,3,5,2,1,1,2,2,1,1,1,3,2,4,1,2,1,1,5,2,2,1,1,2,3,1,3,1,1,1,
        3,1,4,2,1,3,6,1,1,3,7,15,5,3,2,5,3,9,11,4,2,22,1,6,3,8,7,1,4,28,4,16,3,3,25,4,4,27,27,1,4,1,2,2,7,1,3,5,2,28,8,2,14,1,8,6,16,25,3,3,3,14,3,
        3,1,1,2,1,4,6,3,8,4,1,1,1,2,3,6,10,6,2,3,18,3,2,5,5,4,3,1,5,2,5,4,23,7,6,12,6,4,17,11,9,5,1,1,10,5,12,1,1,11,26,33,7,3,6,1,17,7,1,5,12,1,11,
        2,4,1,8,14,17,23,1,2,1,7,8,16,11,9,6,5,2,6,4,16,2,8,14,1,11,8,9,1,1,1,9,25,4,11,19,7,2,15,2,12,8,52,7,5,19,2,16,4,36,8,1,16,8,24,26,4,6,2,9,
        5,4,36,3,28,12,25,15,37,27,17,12,59,38,5,32,127,1,2,9,17,14,4,1,2,1,1,8,11,50,4,14,2,19,16,4,17,5,4,5,26,12,45,2,23,45,104,30,12,8,3,10,2,2,
        3,3,1,4,20,7,2,9,6,15,2,20,1,3,16,4,11,15,6,134,2,5,59,1,2,2,2,1,9,17,3,26,137,10,211,59,1,2,4,1,4,1,1,1,2,6,2,3,1,1,2,3,2,3,1,3,4,4,2,3,3,
        1,4,3,1,7,2,2,3,1,2,1,3,3,3,2,2,3,2,1,3,14,6,1,3,2,9,6,15,27,9,34,145,1,1,2,1,1,1,1,2,1,1,1,1,2,2,2,3,1,2,1,1,1,2,3,5,8,3,5,2,4,1,3,2,2,2,12,
        4,1,1,1,10,4,5,1,20,4,16,1,15,9,5,12,2,9,2,5,4,2,26,19,7,1,26,4,30,12,15,42,1,6,8,172,1,1,4,2,1,1,11,2,2,4,2,1,2,1,10,8,1,2,1,4,5,1,2,5,1,8,
        4,1,3,4,2,1,6,2,1,3,4,1,2,1,1,1,1,12,5,7,2,4,3,1,1,1,3,3,6,1,2,2,3,3,3,2,1,2,12,14,11,6,6,4,12,2,8,1,7,10,1,35,7,4,13,15,4,3,23,21,28,52,5,
        26,5,6,1,7,10,2,7,53,3,2,1,1,1,2,163,532,1,10,11,1,3,3,4,8,2,8,6,2,2,23,22,4,2,2,4,2,1,3,1,3,3,5,9,8,2,1,2,8,1,10,2,12,21,20,15,105,2,3,1,1,
        3,2,3,1,1,2,5,1,4,15,11,19,1,1,1,1,5,4,5,1,1,2,5,3,5,12,1,2,5,1,11,1,1,15,9,1,4,5,3,26,8,2,1,3,1,1,15,19,2,12,1,2,5,2,7,2,19,2,20,6,26,7,5,
        2,2,7,34,21,13,70,2,128,1,1,2,1,1,2,1,1,3,2,2,2,15,1,4,1,3,4,42,10,6,1,49,85,8,1,2,1,1,4,4,2,3,6,1,5,7,4,3,211,4,1,2,1,2,5,1,2,4,2,2,6,5,6,
        10,3,4,48,100,6,2,16,296,5,27,387,2,2,3,7,16,8,5,38,15,39,21,9,10,3,7,59,13,27,21,47,5,21,6
    };
    static ImWchar base_ranges[] = // not zero-terminated
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x2000, 0x206F, // General Punctuation
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF  // Half-width characters
    };
    static ImWchar full_ranges[IM_ARRAYSIZE(base_ranges) + IM_ARRAYSIZE(accumulative_offsets_from_0x4E00) * 2 + 1] = { 0 };
    if (!full_ranges[0])
    {
        memcpy(full_ranges, base_ranges, sizeof(base_ranges));
        UnpackAccumulativeOffsetsIntoRanges(0x4E00, accumulative_offsets_from_0x4E00, IM_ARRAYSIZE(accumulative_offsets_from_0x4E00), full_ranges + IM_ARRAYSIZE(base_ranges));
    }
    return &full_ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesJapanese()
{
    // 1946 common ideograms code points for Japanese
    // Sourced from http://theinstructionlimit.com/common-kanji-character-ranges-for-xna-spritefont-rendering
    // FIXME: Source a list of the revised 2136 Joyo Kanji list from 2010 and rebuild this.
    // You can use ImFontGlyphRangesBuilder to create your own ranges derived from this, by merging existing ranges or adding new characters.
    // (Stored as accumulative offsets from the initial unicode codepoint 0x4E00. This encoding is designed to helps us compact the source code size.)
    static const short accumulative_offsets_from_0x4E00[] =
    {
        0,1,2,4,1,1,1,1,2,1,6,2,2,1,8,5,7,11,1,2,10,10,8,2,4,20,2,11,8,2,1,2,1,6,2,1,7,5,3,7,1,1,13,7,9,1,4,6,1,2,1,10,1,1,9,2,2,4,5,6,14,1,1,9,3,18,
        5,4,2,2,10,7,1,1,1,3,2,4,3,23,2,10,12,2,14,2,4,13,1,6,10,3,1,7,13,6,4,13,5,2,3,17,2,2,5,7,6,4,1,7,14,16,6,13,9,15,1,1,7,16,4,7,1,19,9,2,7,15,
        2,6,5,13,25,4,14,13,11,25,1,1,1,2,1,2,2,3,10,11,3,3,1,1,4,4,2,1,4,9,1,4,3,5,5,2,7,12,11,15,7,16,4,5,16,2,1,1,6,3,3,1,1,2,7,6,6,7,1,4,7,6,1,1,
        2,1,12,3,3,9,5,8,1,11,1,2,3,18,20,4,1,3,6,1,7,3,5,5,7,2,2,12,3,1,4,2,3,2,3,11,8,7,4,17,1,9,25,1,1,4,2,2,4,1,2,7,1,1,1,3,1,2,6,16,1,2,1,1,3,12,
        20,2,5,20,8,7,6,2,1,1,1,1,6,2,1,2,10,1,1,6,1,3,1,2,1,4,1,12,4,1,3,1,1,1,1,1,10,4,7,5,13,1,15,1,1,30,11,9,1,15,38,14,1,32,17,20,1,9,31,2,21,9,
        4,49,22,2,1,13,1,11,45,35,43,55,12,19,83,1,3,2,3,13,2,1,7,3,18,3,13,8,1,8,18,5,3,7,25,24,9,24,40,3,17,24,2,1,6,2,3,16,15,6,7,3,12,1,9,7,3,3,
        3,15,21,5,16,4,5,12,11,11,3,6,3,2,31,3,2,1,1,23,6,6,1,4,2,6,5,2,1,1,3,3,22,2,6,2,3,17,3,2,4,5,1,9,5,1,1,6,15,12,3,17,2,14,2,8,1,23,16,4,2,23,
        8,15,23,20,12,25,19,47,11,21,65,46,4,3,1,5,6,1,2,5,26,2,1,1,3,11,1,1,1,2,1,2,3,1,1,10,2,3,1,1,1,3,6,3,2,2,6,6,9,2,2,2,6,2,5,10,2,4,1,2,1,2,2,
        3,1,1,3,1,2,9,23,9,2,1,1,1,1,5,3,2,1,10,9,6,1,10,2,31,25,3,7,5,40,1,15,6,17,7,27,180,1,3,2,2,1,1,1,6,3,10,7,1,3,6,17,8,6,2,2,1,3,5,5,8,16,14,
        15,1,1,4,1,2,1,1,1,3,2,7,5,6,2,5,10,1,4,2,9,1,1,11,6,1,44,1,3,7,9,5,1,3,1,1,10,7,1,10,4,2,7,21,15,7,2,5,1,8,3,4,1,3,1,6,1,4,2,1,4,10,8,1,4,5,
        1,5,10,2,7,1,10,1,1,3,4,11,10,29,4,7,3,5,2,3,33,5,2,19,3,1,4,2,6,31,11,1,3,3,3,1,8,10,9,12,11,12,8,3,14,8,6,11,1,4,41,3,1,2,7,13,1,5,6,2,6,12,
        12,22,5,9,4,8,9,9,34,6,24,1,1,20,9,9,3,4,1,7,2,2,2,6,2,28,5,3,6,1,4,6,7,4,2,1,4,2,13,6,4,4,3,1,8,8,3,2,1,5,1,2,2,3,1,11,11,7,3,6,10,8,6,16,16,
        22,7,12,6,21,5,4,6,6,3,6,1,3,2,1,2,8,29,1,10,1,6,13,6,6,19,31,1,13,4,4,22,17,26,33,10,4,15,12,25,6,67,10,2,3,1,6,10,2,6,2,9,1,9,4,4,1,2,16,2,
        5,9,2,3,8,1,8,3,9,4,8,6,4,8,11,3,2,1,1,3,26,1,7,5,1,11,1,5,3,5,2,13,6,39,5,1,5,2,11,6,10,5,1,15,5,3,6,19,21,22,2,4,1,6,1,8,1,4,8,2,4,2,2,9,2,
        1,1,1,4,3,6,3,12,7,1,14,2,4,10,2,13,1,17,7,3,2,1,3,2,13,7,14,12,3,1,29,2,8,9,15,14,9,14,1,3,1,6,5,9,11,3,38,43,20,7,7,8,5,15,12,19,15,81,8,7,
        1,5,73,13,37,28,8,8,1,15,18,20,165,28,1,6,11,8,4,14,7,15,1,3,3,6,4,1,7,14,1,1,11,30,1,5,1,4,14,1,4,2,7,52,2,6,29,3,1,9,1,21,3,5,1,26,3,11,14,
        11,1,17,5,1,2,1,3,2,8,1,2,9,12,1,1,2,3,8,3,24,12,7,7,5,17,3,3,3,1,23,10,4,4,6,3,1,16,17,22,3,10,21,16,16,6,4,10,2,1,1,2,8,8,6,5,3,3,3,39,25,
        15,1,1,16,6,7,25,15,6,6,12,1,22,13,1,4,9,5,12,2,9,1,12,28,8,3,5,10,22,60,1,2,40,4,61,63,4,1,13,12,1,4,31,12,1,14,89,5,16,6,29,14,2,5,49,18,18,
        5,29,33,47,1,17,1,19,12,2,9,7,39,12,3,7,12,39,3,1,46,4,12,3,8,9,5,31,15,18,3,2,2,66,19,13,17,5,3,46,124,13,57,34,2,5,4,5,8,1,1,1,4,3,1,17,5,
        3,5,3,1,8,5,6,3,27,3,26,7,12,7,2,17,3,7,18,78,16,4,36,1,2,1,6,2,1,39,17,7,4,13,4,4,4,1,10,4,2,4,6,3,10,1,19,1,26,2,4,33,2,73,47,7,3,8,2,4,15,
        18,1,29,2,41,14,1,21,16,41,7,39,25,13,44,2,2,10,1,13,7,1,7,3,5,20,4,8,2,49,1,10,6,1,6,7,10,7,11,16,3,12,20,4,10,3,1,2,11,2,28,9,2,4,7,2,15,1,
        27,1,28,17,4,5,10,7,3,24,10,11,6,26,3,2,7,2,2,49,16,10,16,15,4,5,27,61,30,14,38,22,2,7,5,1,3,12,23,24,17,17,3,3,2,4,1,6,2,7,5,1,1,5,1,1,9,4,
        1,3,6,1,8,2,8,4,14,3,5,11,4,1,3,32,1,19,4,1,13,11,5,2,1,8,6,8,1,6,5,13,3,23,11,5,3,16,3,9,10,1,24,3,198,52,4,2,2,5,14,5,4,22,5,20,4,11,6,41,
        1,5,2,2,11,5,2,28,35,8,22,3,18,3,10,7,5,3,4,1,5,3,8,9,3,6,2,16,22,4,5,5,3,3,18,23,2,6,23,5,27,8,1,33,2,12,43,16,5,2,3,6,1,20,4,2,9,7,1,11,2,
        10,3,14,31,9,3,25,18,20,2,5,5,26,14,1,11,17,12,40,19,9,6,31,83,2,7,9,19,78,12,14,21,76,12,113,79,34,4,1,1,61,18,85,10,2,2,13,31,11,50,6,33,159,
        179,6,6,7,4,4,2,4,2,5,8,7,20,32,22,1,3,10,6,7,28,5,10,9,2,77,19,13,2,5,1,4,4,7,4,13,3,9,31,17,3,26,2,6,6,5,4,1,7,11,3,4,2,1,6,2,20,4,1,9,2,6,
        3,7,1,1,1,20,2,3,1,6,2,3,6,2,4,8,1,5,13,8,4,11,23,1,10,6,2,1,3,21,2,2,4,24,31,4,10,10,2,5,192,15,4,16,7,9,51,1,2,1,1,5,1,1,2,1,3,5,3,1,3,4,1,
        3,1,3,3,9,8,1,2,2,2,4,4,18,12,92,2,10,4,3,14,5,25,16,42,4,14,4,2,21,5,126,30,31,2,1,5,13,3,22,5,6,6,20,12,1,14,12,87,3,19,1,8,2,9,9,3,3,23,2,
        3,7,6,3,1,2,3,9,1,3,1,6,3,2,1,3,11,3,1,6,10,3,2,3,1,2,1,5,1,1,11,3,6,4,1,7,2,1,2,5,5,34,4,14,18,4,19,7,5,8,2,6,79,1,5,2,14,8,2,9,2,1,36,28,16,
        4,1,1,1,2,12,6,42,39,16,23,7,15,15,3,2,12,7,21,64,6,9,28,8,12,3,3,41,59,24,51,55,57,294,9,9,2,6,2,15,1,2,13,38,90,9,9,9,3,11,7,1,1,1,5,6,3,2,
        1,2,2,3,8,1,4,4,1,5,7,1,4,3,20,4,9,1,1,1,5,5,17,1,5,2,6,2,4,1,4,5,7,3,18,11,11,32,7,5,4,7,11,127,8,4,3,3,1,10,1,1,6,21,14,1,16,1,7,1,3,6,9,65,
        51,4,3,13,3,10,1,1,12,9,21,110,3,19,24,1,1,10,62,4,1,29,42,78,28,20,18,82,6,3,15,6,84,58,253,15,155,264,15,21,9,14,7,58,40,39,
    };
    static ImWchar base_ranges[] = // not zero-terminated
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF  // Half-width characters
    };
    static ImWchar full_ranges[IM_ARRAYSIZE(base_ranges) + IM_ARRAYSIZE(accumulative_offsets_from_0x4E00) * 2 + 1] = { 0 };
    if (!full_ranges[0])
    {
        memcpy(full_ranges, base_ranges, sizeof(base_ranges));
        UnpackAccumulativeOffsetsIntoRanges(0x4E00, accumulative_offsets_from_0x4E00, IM_ARRAYSIZE(accumulative_offsets_from_0x4E00), full_ranges + IM_ARRAYSIZE(base_ranges));
    }
    return &full_ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesCyrillic()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
        0,
    };
    return &ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesThai()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin
        0x2010, 0x205E, // Punctuations
        0x0E00, 0x0E7F, // Thai
        0,
    };
    return &ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesVietnamese()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin
        0x0102, 0x0103,
        0x0110, 0x0111,
        0x0128, 0x0129,
        0x0168, 0x0169,
        0x01A0, 0x01A1,
        0x01AF, 0x01B0,
        0x1EA0, 0x1EF9,
        0,
    };
    return &ranges[0];
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontGlyphRangesBuilder
//-----------------------------------------------------------------------------

void ImFontGlyphRangesBuilder::AddText(const char* text, const char* text_end)
{
    while (text_end ? (text < text_end) : *text)
    {
        unsigned int c = 0;
        int c_len = ImTextCharFromUtf8(&c, text, text_end);
        text += c_len;
        if (c_len == 0)
            break;
        if (c < 0x10000)
            AddChar((ImWchar)c);
    }
}

void ImFontGlyphRangesBuilder::AddRanges(const ImWchar * ranges)
{
    for (; ranges[0]; ranges += 2)
        for (ImWchar c = ranges[0]; c <= ranges[1]; c++)
            AddChar(c);
}

void ImFontGlyphRangesBuilder::BuildRanges(ImVector<ImWchar> * out_ranges)
{
    for (int n = 0; n < 0x10000; n++)
        if (GetBit(n))
        {
            out_ranges->push_back((ImWchar)n);
            while (n < 0x10000 && GetBit(n + 1))
                n++;
            out_ranges->push_back((ImWchar)n);
        }
    out_ranges->push_back(0);
}

//-----------------------------------------------------------------------------
// [SECTION] ImFont
//-----------------------------------------------------------------------------

ImFont::ImFont()
{
    FontSize = 0.0f;
    FallbackAdvanceX = 0.0f;
    FallbackChar = (ImWchar)'?';
    DisplayOffset = vsize(0.0f, 0.0f);
    FallbackGlyph = NULL;
    ContainerAtlas = NULL;
    ConfigData = NULL;
    ConfigDataCount = 0;
    DirtyLookupTables = false;
    Scale = 1.0f;
    Ascent = Descent = 0.0f;
    MetricsTotalSurface = 0;
}

ImFont::~ImFont()
{
    ClearOutputData();
}

void    ImFont::ClearOutputData()
{
    FontSize = 0.0f;
    FallbackAdvanceX = 0.0f;
    Glyphs.clear();
    IndexAdvanceX.clear();
    IndexLookup.clear();
    FallbackGlyph = NULL;
    ContainerAtlas = NULL;
    DirtyLookupTables = true;
    Ascent = Descent = 0.0f;
    MetricsTotalSurface = 0;
}

void ImFont::BuildLookupTable()
{
    int max_codepoint = 0;
    for (int i = 0; i != Glyphs.Size; i++)
        max_codepoint = ImMax(max_codepoint, (int)Glyphs[i].Codepoint);

    IM_ASSERT(Glyphs.Size < 0xFFFF); // -1 is reserved
    IndexAdvanceX.clear();
    IndexLookup.clear();
    DirtyLookupTables = false;
    GrowIndex(max_codepoint + 1);
    for (int i = 0; i < Glyphs.Size; i++)
    {
        int codepoint = (int)Glyphs[i].Codepoint;
        IndexAdvanceX[codepoint] = Glyphs[i].AdvanceX;
        IndexLookup[codepoint] = (ImWchar)i;
    }

    // Create a glyph to handle TAB
    // FIXME: Needs proper TAB handling but it needs to be contextualized (or we could arbitrary say that each string starts at "column 0" ?)
    if (FindGlyph((ImWchar)' '))
    {
        if (Glyphs.back().Codepoint != '\t')   // So we can call this function multiple times
            Glyphs.resize(Glyphs.Size + 1);
        ImFontGlyph & tab_glyph = Glyphs.back();
        tab_glyph = *FindGlyph((ImWchar)' ');
        tab_glyph.Codepoint = '\t';
        tab_glyph.AdvanceX *= IM_TABSIZE;
        IndexAdvanceX[(int)tab_glyph.Codepoint] = (float)tab_glyph.AdvanceX;
        IndexLookup[(int)tab_glyph.Codepoint] = (ImWchar)(Glyphs.Size - 1);
    }

    FallbackGlyph = FindGlyphNoFallback(FallbackChar);
    FallbackAdvanceX = FallbackGlyph ? FallbackGlyph->AdvanceX : 0.0f;
    for (int i = 0; i < max_codepoint + 1; i++)
        if (IndexAdvanceX[i] < 0.0f)
            IndexAdvanceX[i] = FallbackAdvanceX;
}

void ImFont::SetFallbackChar(ImWchar c)
{
    FallbackChar = c;
    BuildLookupTable();
}

void ImFont::GrowIndex(int new_size)
{
    IM_ASSERT(IndexAdvanceX.Size == IndexLookup.Size);
    if (new_size <= IndexLookup.Size)
        return;
    IndexAdvanceX.resize(new_size, -1.0f);
    IndexLookup.resize(new_size, (ImWchar)-1);
}

// x0/y0/x1/y1 are offset from the character upper-left layout position, in pixels. Therefore x0/y0 are often fairly close to zero.
// Not to be mistaken with texture coordinates, which are held by u0/v0/u1/v1 in normalized format (0.0..1.0 on each texture axis).
void ImFont::AddGlyph(ImWchar codepoint, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, float advance_x)
{
    Glyphs.resize(Glyphs.Size + 1);
    ImFontGlyph & glyph = Glyphs.back();
    glyph.Codepoint = (ImWchar)codepoint;
    glyph.X0 = x0;
    glyph.Y0 = y0;
    glyph.X1 = x1;
    glyph.Y1 = y1;
    glyph.U0 = u0;
    glyph.V0 = v0;
    glyph.U1 = u1;
    glyph.V1 = v1;
    glyph.AdvanceX = advance_x + ConfigData->GlyphExtraSpacing.x;  // Bake spacing into AdvanceX

    if (ConfigData->PixelSnapH)
        glyph.AdvanceX = (float)(int)(glyph.AdvanceX + 0.5f);

    // Compute rough surface usage metrics (+1 to account for avehnly padding, +0.99 to round)
    DirtyLookupTables = true;
    MetricsTotalSurface += (int)((glyph.U1 - glyph.U0) * ContainerAtlas->TexWidth + 1.99f) * (int)((glyph.V1 - glyph.V0) * ContainerAtlas->TexHeight + 1.99f);
}

void ImFont::AddRemapChar(ImWchar dst, ImWchar src, bool overwrite_dst)
{
    IM_ASSERT(IndexLookup.Size > 0);    // Currently this can only be called AFTER the font has been built, aka after calling ImFontAtlas::GetTexDataAs*() function.
    int index_size = IndexLookup.Size;

    if (dst < index_size && IndexLookup.Data[dst] == (ImWchar)-1 && !overwrite_dst) // 'dst' already exists
        return;
    if (src >= index_size && dst >= index_size) // both 'dst' and 'src' don't exist -> no-op
        return;

    GrowIndex(dst + 1);
    IndexLookup[dst] = (src < index_size) ? IndexLookup.Data[src] : (ImWchar)-1;
    IndexAdvanceX[dst] = (src < index_size) ? IndexAdvanceX.Data[src] : 1.0f;
}

const ImFontGlyph * ImFont::FindGlyph(ImWchar c) const
{
    if (c >= IndexLookup.Size)
        return FallbackGlyph;
    const ImWchar i = IndexLookup.Data[c];
    if (i == (ImWchar)-1)
        return FallbackGlyph;
    return &Glyphs.Data[i];
}

const ImFontGlyph * ImFont::FindGlyphNoFallback(ImWchar c) const
{
    if (c >= IndexLookup.Size)
        return NULL;
    const ImWchar i = IndexLookup.Data[c];
    if (i == (ImWchar)-1)
        return NULL;
    return &Glyphs.Data[i];
}

const char* ImFont::CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width) const
{
    // Simple word-wrapping for English, not full-featured. Please submit failing cases!
    // FIXME: Much possible improvements (don't cut things like "word !", "word!!!" but cut within "word,,,,", more sensible support for punctuations, support for Unicode punctuations, etc.)

    // For references, possible wrap point marked with ^
    //  "aaa bbb, ccc,ddd. eee   fff. ggg!"
    //      ^    ^    ^   ^   ^__    ^    ^

    // List of hardcoded separators: .,;!?'"

    // Skip extra blanks after a line returns (that includes not counting them in width computation)
    // e.g. "Hello    world" --> "Hello" "World"

    // Cut words that cannot possibly fit within one line.
    // e.g.: "The tropical fish" with ~5 characters worth of width --> "The tr" "opical" "fish"

    float line_width = 0.0f;
    float word_width = 0.0f;
    float blank_width = 0.0f;
    wrap_width /= scale; // We work with unscaled widths to avoid scaling every characters

    const char* word_end = text;
    const char* prev_word_end = NULL;
    bool inside_word = true;

    const char* s = text;
    while (s < text_end)
    {
        unsigned int c = (unsigned int)* s;
        const char* next_s;
        if (c < 0x80)
            next_s = s + 1;
        else
            next_s = s + ImTextCharFromUtf8(&c, s, text_end);
        if (c == 0)
            break;

        if (c < 32)
        {
            if (c == '\n')
            {
                line_width = word_width = blank_width = 0.0f;
                inside_word = true;
                s = next_s;
                continue;
            }
            if (c == '\r')
            {
                s = next_s;
                continue;
            }
        }

        const float char_width = ((int)c < IndexAdvanceX.Size ? IndexAdvanceX.Data[c] : FallbackAdvanceX);
        if (ImCharIsBlankW(c))
        {
            if (inside_word)
            {
                line_width += blank_width;
                blank_width = 0.0f;
                word_end = s;
            }
            blank_width += char_width;
            inside_word = false;
        } else
        {
            word_width += char_width;
            if (inside_word)
            {
                word_end = next_s;
            } else
            {
                prev_word_end = word_end;
                line_width += word_width + blank_width;
                word_width = blank_width = 0.0f;
            }

            // Allow wrapping after punctuation.
            inside_word = !(c == '.' || c == ',' || c == ';' || c == '!' || c == '?' || c == '\"');
        }

        // We ignore blank width at the end of the line (they can be skipped)
        if (line_width + word_width >= wrap_width)
        {
            // Words that cannot possibly fit within an entire line will be cut anywhere.
            if (word_width < wrap_width)
                s = prev_word_end ? prev_word_end : word_end;
            break;
        }

        s = next_s;
    }

    return s;
}

vsize ImFont::CalcTextSizeA(float size, float max_width, float wrap_width, const char* text_begin, const char* text_end, const char** remaining) const
{
    if (!text_end)
        text_end = text_begin + strlen(text_begin); // FIXME-OPT: Need to avoid this.

    const float line_height = size;
    const float scale = size / FontSize;

    vsize text_size = vsize(0, 0);
    float line_width = 0.0f;

    const bool word_wrap_enabled = (wrap_width > 0.0f);
    const char* word_wrap_eol = NULL;

    const char* s = text_begin;
    while (s < text_end)
    {
        if (word_wrap_enabled)
        {
            // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
            if (!word_wrap_eol)
            {
                word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - line_width);
                if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
                    word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
            }

            if (s >= word_wrap_eol)
            {
                if (text_size.x < line_width)
                    text_size.x = line_width;
                text_size.y += line_height;
                line_width = 0.0f;
                word_wrap_eol = NULL;

                // Wrapping skips upcoming blanks
                while (s < text_end)
                {
                    const char c = *s;
                    if (ImCharIsBlankA(c)) { s++; } else if (c == '\n') { s++; break; } else { break; }
                }
                continue;
            }
        }

        // Decode and advance source
        const char* prev_s = s;
        unsigned int c = (unsigned int)* s;
        if (c < 0x80)
        {
            s += 1;
        } else
        {
            s += ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0) // Malformed UTF-8?
                break;
        }

        if (c < 32)
        {
            if (c == '\n')
            {
                text_size.x = ImMax(text_size.x, line_width);
                text_size.y += line_height;
                line_width = 0.0f;
                continue;
            }
            if (c == '\r')
                continue;
        }

        const float char_width = ((int)c < IndexAdvanceX.Size ? IndexAdvanceX.Data[c] : FallbackAdvanceX) * scale;
        if (line_width + char_width >= max_width)
        {
            s = prev_s;
            break;
        }

        line_width += char_width;
    }

    if (text_size.x < line_width)
        text_size.x = line_width;

    if (line_width > 0 || text_size.y == 0.0f)
        text_size.y += line_height;

    if (remaining)
        * remaining = s;

    return text_size;
}

void ImFont::RenderChar(ImDrawList * draw_list, float size, vsize pos, ImU32 col, ImWchar c) const
{
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r') // Match behavior of RenderText(), those 4 codepoints are hard-coded.
        return;
    if (const ImFontGlyph * glyph = FindGlyph(c))
    {
        float scale = (size >= 0.0f) ? (size / FontSize) : 1.0f;
        pos.x = (float)(int)pos.x + DisplayOffset.x;
        pos.y = (float)(int)pos.y + DisplayOffset.y;
        draw_list->PrimReserve(6, 4);
        draw_list->PrvsrectUV(vsize(pos.x + glyph->X0 * scale, pos.y + glyph->Y0 * scale), vsize(pos.x + glyph->X1 * scale, pos.y + glyph->Y1 * scale), vsize(glyph->U0, glyph->V0), vsize(glyph->U1, glyph->V1), col);
    }
}

void ImFont::RenderText(ImDrawList * draw_list, float size, vsize pos, ImU32 col, const xuifloatcolor & clip_rect, const char* text_begin, const char* text_end, float wrap_width, bool cpu_fine_clip) const
{
    if (!text_end)
        text_end = text_begin + strlen(text_begin); // HanaLovesMe functions generally already provides a valid text_end, so this is merely to handle direct calls.

    // Align to be pixel perfect
    pos.x = (float)(int)pos.x + DisplayOffset.x;
    pos.y = (float)(int)pos.y + DisplayOffset.y;
    float x = pos.x;
    float y = pos.y;
    if (y > clip_rect.w)
        return;

    const float scale = size / FontSize;
    const float line_height = FontSize * scale;
    const bool word_wrap_enabled = (wrap_width > 0.0f);
    const char* word_wrap_eol = NULL;

    // Fast-forward to first visible line
    const char* s = text_begin;
    if (y + line_height < clip_rect.y && !word_wrap_enabled)
        while (y + line_height < clip_rect.y && s < text_end)
        {
            s = (const char*)memchr(s, '\n', text_end - s);
            s = s ? s + 1 : text_end;
            y += line_height;
        }

    // For large text, scan for the last visible line in order to avoid over-reserving in the call to PrimReserve()
    // Note that very large horizontal line will still be affected by the issue (e.g. a one megabyte string buffer without a newline will likely crash atm)
    if (text_end - s > 10000 && !word_wrap_enabled)
    {
        const char* s_end = s;
        float y_end = y;
        while (y_end < clip_rect.w && s_end < text_end)
        {
            s_end = (const char*)memchr(s_end, '\n', text_end - s_end);
            s_end = s_end ? s_end + 1 : text_end;
            y_end += line_height;
        }
        text_end = s_end;
    }
    if (s == text_end)
        return;

    // Reserve vertices for remaining worse case (over-reserving is useful and easily amortized)
    const int vtx_count_max = (int)(text_end - s) * 4;
    const int idx_count_max = (int)(text_end - s) * 6;
    const int idx_expected_size = draw_list->IdxBuffer.Size + idx_count_max;
    draw_list->PrimReserve(idx_count_max, vtx_count_max);

    ImDrawVert * vtx_write = draw_list->_VtxWritePtr;
    ImDrawIdx * idx_write = draw_list->_IdxWritePtr;
    unsigned int vtx_current_idx = draw_list->_VtxCurrentIdx;

    while (s < text_end)
    {
        if (word_wrap_enabled)
        {
            // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
            if (!word_wrap_eol)
            {
                word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - (x - pos.x));
                if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
                    word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
            }

            if (s >= word_wrap_eol)
            {
                x = pos.x;
                y += line_height;
                word_wrap_eol = NULL;

                // Wrapping skips upcoming blanks
                while (s < text_end)
                {
                    const char c = *s;
                    if (ImCharIsBlankA(c)) { s++; } else if (c == '\n') { s++; break; } else { break; }
                }
                continue;
            }
        }

        // Decode and advance source
        unsigned int c = (unsigned int)* s;
        if (c < 0x80)
        {
            s += 1;
        } else
        {
            s += ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0) // Malformed UTF-8?
                break;
        }

        if (c < 32)
        {
            if (c == '\n')
            {
                x = pos.x;
                y += line_height;
                if (y > clip_rect.w)
                    break; // break out of main loop
                continue;
            }
            if (c == '\r')
                continue;
        }

        float char_width = 0.0f;
        if (const ImFontGlyph * glyph = FindGlyph((ImWchar)c))
        {
            char_width = glyph->AdvanceX * scale;

            // Arbitrarily assume that both space and tabs are empty glyphs as an optimization
            if (c != ' ' && c != '\t')
            {
                // We don't do a second finer clipping test on the Y axis as we've already skipped anything before clip_rect.y and exit once we pass clip_rect.w
                float x1 = x + glyph->X0 * scale;
                float x2 = x + glyph->X1 * scale;
                float y1 = y + glyph->Y0 * scale;
                float y2 = y + glyph->Y1 * scale;
                if (x1 <= clip_rect.z && x2 >= clip_rect.x)
                {
                    // Render a character
                    float u1 = glyph->U0;
                    float v1 = glyph->V0;
                    float u2 = glyph->U1;
                    float v2 = glyph->V1;

                    // CPU side clipping used to fit text in their frame when the frame is too small. Only does clipping for axis aligned quads.
                    if (cpu_fine_clip)
                    {
                        if (x1 < clip_rect.x)
                        {
                            u1 = u1 + (1.0f - (x2 - clip_rect.x) / (x2 - x1)) * (u2 - u1);
                            x1 = clip_rect.x;
                        }
                        if (y1 < clip_rect.y)
                        {
                            v1 = v1 + (1.0f - (y2 - clip_rect.y) / (y2 - y1)) * (v2 - v1);
                            y1 = clip_rect.y;
                        }
                        if (x2 > clip_rect.z)
                        {
                            u2 = u1 + ((clip_rect.z - x1) / (x2 - x1)) * (u2 - u1);
                            x2 = clip_rect.z;
                        }
                        if (y2 > clip_rect.w)
                        {
                            v2 = v1 + ((clip_rect.w - y1) / (y2 - y1)) * (v2 - v1);
                            y2 = clip_rect.w;
                        }
                        if (y1 >= y2)
                        {
                            x += char_width;
                            continue;
                        }
                    }

                    // We are NOT calling PrvsrectUV() here because non-inlined causes too much overhead in a debug builds. Inlined here:
                    {
                        idx_write[0] = (ImDrawIdx)(vtx_current_idx); idx_write[1] = (ImDrawIdx)(vtx_current_idx + 1); idx_write[2] = (ImDrawIdx)(vtx_current_idx + 2);
                        idx_write[3] = (ImDrawIdx)(vtx_current_idx); idx_write[4] = (ImDrawIdx)(vtx_current_idx + 2); idx_write[5] = (ImDrawIdx)(vtx_current_idx + 3);
                        vtx_write[0].pos.x = x1; vtx_write[0].pos.y = y1; vtx_write[0].col = col; vtx_write[0].uv.x = u1; vtx_write[0].uv.y = v1;
                        vtx_write[1].pos.x = x2; vtx_write[1].pos.y = y1; vtx_write[1].col = col; vtx_write[1].uv.x = u2; vtx_write[1].uv.y = v1;
                        vtx_write[2].pos.x = x2; vtx_write[2].pos.y = y2; vtx_write[2].col = col; vtx_write[2].uv.x = u2; vtx_write[2].uv.y = v2;
                        vtx_write[3].pos.x = x1; vtx_write[3].pos.y = y2; vtx_write[3].col = col; vtx_write[3].uv.x = u1; vtx_write[3].uv.y = v2;
                        vtx_write += 4;
                        vtx_current_idx += 4;
                        idx_write += 6;
                    }
                }
            }
        }

        x += char_width;
    }

    // Give back unused vertices
    draw_list->VtxBuffer.resize((int)(vtx_write - draw_list->VtxBuffer.Data));
    draw_list->IdxBuffer.resize((int)(idx_write - draw_list->IdxBuffer.Data));
    draw_list->CmdBuffer[draw_list->CmdBuffer.Size - 1].ElemCount -= (idx_expected_size - draw_list->IdxBuffer.Size);
    draw_list->_VtxWritePtr = vtx_write;
    draw_list->_IdxWritePtr = idx_write;
    draw_list->_VtxCurrentIdx = (unsigned int)draw_list->VtxBuffer.Size;
}

//-----------------------------------------------------------------------------
// [SECTION] Internal Render Helpers
// (progressively moved from HanaLovesMe.cpp to here when they are redesigned to stop accessing HanaLovesMe global state)
//-----------------------------------------------------------------------------
// - RenderMouseCursor()
// - RenderArrowPointingAt()
// - RenderRectFilledRangeH()
// - RenderPixelEllipsis()
//-----------------------------------------------------------------------------

void HanaLovesMe::RenderMouseCursor(ImDrawList * draw_list, vsize pos, float scale, HanaLovesMeMouseCursor mouse_cursor)
{
    if (mouse_cursor == HanaLovesMeMouseCursor_None)
        return;
    IM_ASSERT(mouse_cursor > HanaLovesMeMouseCursor_None && mouse_cursor < HanaLovesMeMouseCursor_COUNT);

    const ImU32 col_shadow = IM_COL32(0, 0, 0, 48);
    const ImU32 col_border = IM_COL32(0, 0, 0, 255);          // Black
    const ImU32 col_fill = IM_COL32(255, 255, 255, 255);    // White

    ImFontAtlas * font_atlas = draw_list->_Data->Font->ContainerAtlas;
    vsize offset, size, uv[4];
    if (font_atlas->GetMouseCursorTexData(mouse_cursor, &offset, &size, &uv[0], &uv[2]))
    {
        pos -= offset;
        const ImTextureID tex_id = font_atlas->TexID;
        draw_list->PushTextureID(tex_id);
        draw_list->AddImage(tex_id, pos + vsize(1, 0) * scale, pos + vsize(1, 0) * scale + size * scale, uv[2], uv[3], col_shadow);
        draw_list->AddImage(tex_id, pos + vsize(2, 0) * scale, pos + vsize(2, 0) * scale + size * scale, uv[2], uv[3], col_shadow);
        draw_list->AddImage(tex_id, pos, pos + size * scale, uv[2], uv[3], col_border);
        draw_list->AddImage(tex_id, pos, pos + size * scale, uv[0], uv[1], col_fill);
        draw_list->PopTextureID();
    }
}

// Render an arrow. 'pos' is position of the arrow tip. half_sz.x is length from base to tip. half_sz.y is length on each side.
void HanaLovesMe::RenderArrowPointingAt(ImDrawList * draw_list, vsize pos, vsize half_sz, HanaLovesMeDir direction, ImU32 col)
{
    switch (direction)
    {
    case HanaLovesMeDir_Left:  draw_list->AddTriangleFilled(vsize(pos.x + half_sz.x, pos.y - half_sz.y), vsize(pos.x + half_sz.x, pos.y + half_sz.y), pos, col); return;
    case HanaLovesMeDir_Right: draw_list->AddTriangleFilled(vsize(pos.x - half_sz.x, pos.y + half_sz.y), vsize(pos.x - half_sz.x, pos.y - half_sz.y), pos, col); return;
    case HanaLovesMeDir_Up:    draw_list->AddTriangleFilled(vsize(pos.x + half_sz.x, pos.y + half_sz.y), vsize(pos.x - half_sz.x, pos.y + half_sz.y), pos, col); return;
    case HanaLovesMeDir_Down:  draw_list->AddTriangleFilled(vsize(pos.x - half_sz.x, pos.y - half_sz.y), vsize(pos.x + half_sz.x, pos.y - half_sz.y), pos, col); return;
    case HanaLovesMeDir_None: case HanaLovesMeDir_COUNT: break; // Fix warnings
    }
}

static inline float ImAcos01(float x)
{
    if (x <= 0.0f) return IM_PI * 0.5f;
    if (x >= 1.0f) return 0.0f;
    return ImAcos(x);
    //return (-0.69813170079773212f * x * x - 0.87266462599716477f) * x + 1.5707963267948966f; // Cheap approximation, may be enough for what we do.
}

// FIXME: Cleanup and move code to ImDrawList.
void HanaLovesMe::RenderRectFilledRangeH(ImDrawList * draw_list, const vsrect & rect, ImU32 col, float x_start_norm, float x_end_norm, float rounding)
{
    if (x_end_norm == x_start_norm)
        return;
    if (x_start_norm > x_end_norm)
        ImSwap(x_start_norm, x_end_norm);

    vsize p0 = vsize(ImLerp(rect.Min.x, rect.Max.x, x_start_norm), rect.Min.y);
    vsize p1 = vsize(ImLerp(rect.Min.x, rect.Max.x, x_end_norm), rect.Max.y);
    if (rounding == 0.0f)
    {
        draw_list->AddRectFilled(p0, p1, col, 0.0f);
        return;
    }

    rounding = ImClamp(ImMin((rect.Max.x - rect.Min.x) * 0.5f, (rect.Max.y - rect.Min.y) * 0.5f) - 1.0f, 0.0f, rounding);
    const float inv_rounding = 1.0f / rounding;
    const float arc0_b = ImAcos01(1.0f - (p0.x - rect.Min.x) * inv_rounding);
    const float arc0_e = ImAcos01(1.0f - (p1.x - rect.Min.x) * inv_rounding);
    const float half_pi = IM_PI * 0.5f; // We will == compare to this because we know this is the exact value ImAcos01 can return.
    const float x0 = ImMax(p0.x, rect.Min.x + rounding);
    if (arc0_b == arc0_e)
    {
        draw_list->PathLineTo(vsize(x0, p1.y));
        draw_list->PathLineTo(vsize(x0, p0.y));
    } else if (arc0_b == 0.0f && arc0_e == half_pi)
    {
        draw_list->PathArcToFast(vsize(x0, p1.y - rounding), rounding, 3, 6); // BL
        draw_list->PathArcToFast(vsize(x0, p0.y + rounding), rounding, 6, 9); // TR
    } else
    {
        draw_list->PathArcTo(vsize(x0, p1.y - rounding), rounding, IM_PI - arc0_e, IM_PI - arc0_b, 3); // BL
        draw_list->PathArcTo(vsize(x0, p0.y + rounding), rounding, IM_PI + arc0_b, IM_PI + arc0_e, 3); // TR
    }
    if (p1.x > rect.Min.x + rounding)
    {
        const float arc1_b = ImAcos01(1.0f - (rect.Max.x - p1.x) * inv_rounding);
        const float arc1_e = ImAcos01(1.0f - (rect.Max.x - p0.x) * inv_rounding);
        const float x1 = ImMin(p1.x, rect.Max.x - rounding);
        if (arc1_b == arc1_e)
        {
            draw_list->PathLineTo(vsize(x1, p0.y));
            draw_list->PathLineTo(vsize(x1, p1.y));
        } else if (arc1_b == 0.0f && arc1_e == half_pi)
        {
            draw_list->PathArcToFast(vsize(x1, p0.y + rounding), rounding, 9, 12); // TR
            draw_list->PathArcToFast(vsize(x1, p1.y - rounding), rounding, 0, 3);  // BR
        } else
        {
            draw_list->PathArcTo(vsize(x1, p0.y + rounding), rounding, -arc1_e, -arc1_b, 3); // TR
            draw_list->PathArcTo(vsize(x1, p1.y - rounding), rounding, +arc1_b, +arc1_e, 3); // BR
        }
    }
    draw_list->PathFillConvex(col);
}

// FIXME: Rendering an ellipsis "..." is a surprisingly tricky problem for us... we cannot rely on font glyph having it,
// and regular dot are typically too wide. If we render a dot/shape ourselves it comes with the risk that it wouldn't match
// the boldness or positioning of what the font uses...
void HanaLovesMe::RenderPixelEllipsis(ImDrawList * draw_list, vsize pos, int count, ImU32 col)
{
    ImFont* font = draw_list->_Data->Font;
    const float font_scale = draw_list->_Data->FontSize / font->FontSize;
    pos.y += (float)(int)(font->DisplayOffset.y + font->Ascent * font_scale + 0.5f - 1.0f);
    for (int dot_n = 0; dot_n < count; dot_n++)
        draw_list->AddRectFilled(vsize(pos.x + dot_n * 2.0f, pos.y), vsize(pos.x + dot_n * 2.0f + 1.0f, pos.y + 1.0f), col);
}

//-----------------------------------------------------------------------------
// [SECTION] Decompression code
//-----------------------------------------------------------------------------
// Compressed with stb_compress() then converted to a C array and encoded as base85.
// Use the program in misc/fonts/binary_to_compressed_c.cpp to create the array from a TTF file.
// The purpose of encoding as base85 instead of "0x00,0x01,..." style is only save on _source code_ size.
// Decompression from stb.h (public domain) by Sean Barrett https://github.com/nothings/stb/blob/master/stb.h
//-----------------------------------------------------------------------------

static unsigned int stb_decompress_length(const unsigned char* input)
{
    return (input[8] << 24) + (input[9] << 16) + (input[10] << 8) + input[11];
}

static unsigned char* stb__barrier_out_e, *stb__barrier_out_b;
static const unsigned char* stb__barrier_in_b;
static unsigned char* stb__dout;
static void stb__match(const unsigned char* data, unsigned int length)
{
    // INVERSE of memmove... write each byte before copying the next...
    IM_ASSERT(stb__dout + length <= stb__barrier_out_e);
    if (stb__dout + length > stb__barrier_out_e) { stb__dout += length; return; }
    if (data < stb__barrier_out_b) { stb__dout = stb__barrier_out_e + 1; return; }
    while (length--)* stb__dout++ = *data++;
}

static void stb__lit(const unsigned char* data, unsigned int length)
{
    IM_ASSERT(stb__dout + length <= stb__barrier_out_e);
    if (stb__dout + length > stb__barrier_out_e) { stb__dout += length; return; }
    if (data < stb__barrier_in_b) { stb__dout = stb__barrier_out_e + 1; return; }
    memcpy(stb__dout, data, length);
    stb__dout += length;
}

#define stb__in2(x)   ((i[x] << 8) + i[(x)+1])
#define stb__in3(x)   ((i[x] << 16) + stb__in2((x)+1))
#define stb__in4(x)   ((i[x] << 24) + stb__in3((x)+1))

static const unsigned char* stb_decompress_token(const unsigned char* i)
{
    if (*i >= 0x20) { // use fewer if's for cases that expand small
        if (*i >= 0x80)       stb__match(stb__dout - i[1] - 1, i[0] - 0x80 + 1), i += 2;
        else if (*i >= 0x40)  stb__match(stb__dout - (stb__in2(0) - 0x4000 + 1), i[2] + 1), i += 3;
        else /* *i >= 0x20 */ stb__lit(i + 1, i[0] - 0x20 + 1), i += 1 + (i[0] - 0x20 + 1);
    } else { // more ifs for cases that expand large, since overhead is amortized
        if (*i >= 0x18)       stb__match(stb__dout - (stb__in3(0) - 0x180000 + 1), i[3] + 1), i += 4;
        else if (*i >= 0x10)  stb__match(stb__dout - (stb__in3(0) - 0x100000 + 1), stb__in2(3) + 1), i += 5;
        else if (*i >= 0x08)  stb__lit(i + 2, stb__in2(0) - 0x0800 + 1), i += 2 + (stb__in2(0) - 0x0800 + 1);
        else if (*i == 0x07)  stb__lit(i + 3, stb__in2(1) + 1), i += 3 + (stb__in2(1) + 1);
        else if (*i == 0x06)  stb__match(stb__dout - (stb__in3(1) + 1), i[4] + 1), i += 5;
        else if (*i == 0x04)  stb__match(stb__dout - (stb__in3(1) + 1), stb__in2(4) + 1), i += 6;
    }
    return i;
}

static unsigned int stb_adler32(unsigned int adler32, unsigned char* buffer, unsigned int buflen)
{
    const unsigned long ADLER_MOD = 65521;
    unsigned long s1 = adler32 & 0xffff, s2 = adler32 >> 16;
    unsigned long blocklen, i;

    blocklen = buflen % 5552;
    while (buflen) {
        for (i = 0; i + 7 < blocklen; i += 8) {
            s1 += buffer[0], s2 += s1;
            s1 += buffer[1], s2 += s1;
            s1 += buffer[2], s2 += s1;
            s1 += buffer[3], s2 += s1;
            s1 += buffer[4], s2 += s1;
            s1 += buffer[5], s2 += s1;
            s1 += buffer[6], s2 += s1;
            s1 += buffer[7], s2 += s1;

            buffer += 8;
        }

        for (; i < blocklen; ++i)
            s1 += *buffer++, s2 += s1;

        s1 %= ADLER_MOD, s2 %= ADLER_MOD;
        buflen -= blocklen;
        blocklen = 5552;
    }
    return (unsigned int)(s2 << 16) + (unsigned int)s1;
}

static unsigned int stb_decompress(unsigned char* output, const unsigned char* i, unsigned int /*length*/)
{
    unsigned int olen;
    if (stb__in4(0) != 0x57bC0000) return 0;
    if (stb__in4(4) != 0)          return 0; // error! stream is > 4GB
    olen = stb_decompress_length(i);
    stb__barrier_in_b = i;
    stb__barrier_out_e = output + olen;
    stb__barrier_out_b = output;
    i += 16;

    stb__dout = output;
    for (;;) {
        const unsigned char* old_i = i;
        i = stb_decompress_token(i);
        if (i == old_i) {
            if (*i == 0x05 && i[1] == 0xfa) {
                IM_ASSERT(stb__dout == output + olen);
                if (stb__dout != output + olen) return 0;
                if (stb_adler32(1, output, olen) != (unsigned int)stb__in4(2))
                    return 0;
                return olen;
            } else {
                IM_ASSERT(0); /* NOTREACHED */
                return 0;
            }
        }
        IM_ASSERT(stb__dout <= output + olen);
        if (stb__dout > output + olen)
            return 0;
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Default font data (ProggyClean.ttf)
//-----------------------------------------------------------------------------
// ProggyClean.ttf
// Copyright (c) 2004, 2005 Tristan Grimmer
// MIT license (see License.txt in http://www.upperbounds.net/download/ProggyClean.ttf.zip)
// Download and more information at http://upperbounds.net
//-----------------------------------------------------------------------------
// File: 'ProggyClean.ttf' (41208 bytes)
// Exported using misc/fonts/binary_to_compressed_c.cpp (with compression + base85 string encoding).
// The purpose of encoding as base85 instead of "0x00,0x01,..." style is only save on _source code_ size.
//-----------------------------------------------------------------------------
static const char proggy_clean_ttf_compressed_data_base85[11980 + 1] =
"7])#######hV0qs'/###[),##/l:$#Q6>##5[n42>c-TH`->>#/e>11NNV=Bv(*:.F?uu#(gRU.o0XGH`$vhLG1hxt9?W`#,5LsCp#-i>.r$<$6pD>Lb';9Crc6tgXmKVeU2cD4Eo3R/"
"2*>]b(MC;$jPfY.;h^`IWM9<Lh2TlS+f-s$o6Q<BWH`YiU.xfLq$N;$0iR/GX:U(jcW2p/W*q?-qmnUCI;jHSAiFWM.R*kU@C=GH?a9wp8f$e.-4^Qg1)Q-GL(lf(r/7GrRgwV%MS=C#"
"`8ND>Qo#t'X#(v#Y9w0#1D$CIf;W'#pWUPXOuxXuU(H9M(1<q-UE31#^-V'8IRUo7Qf./L>=Ke$$'5F%)]0^#0X@U.a<r:QLtFsLcL6##lOj)#.Y5<-R&KgLwqJfLgN&;Q?gI^#DY2uL"
"i@^rMl9t=cWq6##weg>$FBjVQTSDgEKnIS7EM9>ZY9w0#L;>>#Mx&4Mvt//L[MkA#W@lK.N'[0#7RL_&#w+F%HtG9M#XL`N&.,GM4Pg;-<nLENhvx>-VsM.M0rJfLH2eTM`*oJMHRC`N"
"kfimM2J,W-jXS:)r0wK#@Fge$U>`w'N7G#$#fB#$E^$#:9:hk+eOe--6x)F7*E%?76%^GMHePW-Z5l'&GiF#$956:rS?dA#fiK:)Yr+`&#0j@'DbG&#^$PG.Ll+DNa<XCMKEV*N)LN/N"
"*b=%Q6pia-Xg8I$<MR&,VdJe$<(7G;Ckl'&hF;;$<_=X(b.RS%%)###MPBuuE1V:v&cX&#2m#(&cV]`k9OhLMbn%s$G2,B$BfD3X*sp5#l,$R#]x_X1xKX%b5U*[r5iMfUo9U`N99hG)"
"tm+/Us9pG)XPu`<0s-)WTt(gCRxIg(%6sfh=ktMKn3j)<6<b5Sk_/0(^]AaN#(p/L>&VZ>1i%h1S9u5o@YaaW$e+b<TWFn/Z:Oh(Cx2$lNEoN^e)#CFY@@I;BOQ*sRwZtZxRcU7uW6CX"
"ow0i(?$Q[cjOd[P4d)]>ROPOpxTO7Stwi1::iB1q)C_=dV26J;2,]7op$]uQr@_V7$q^%lQwtuHY]=DX,n3L#0PHDO4f9>dC@O>HBuKPpP*E,N+b3L#lpR/MrTEH.IAQk.a>D[.e;mc."
"x]Ip.PH^'/aqUO/$1WxLoW0[iLA<QT;5HKD+@qQ'NQ(3_PLhE48R.qAPSwQ0/WK?Z,[x?-J;jQTWA0X@KJ(_Y8N-:/M74:/-ZpKrUss?d#dZq]DAbkU*JqkL+nwX@@47`5>w=4h(9.`G"
"CRUxHPeR`5Mjol(dUWxZa(>STrPkrJiWx`5U7F#.g*jrohGg`cg:lSTvEY/EV_7H4Q9[Z%cnv;JQYZ5q.l7Zeas:HOIZOB?G<Nald$qs]@]L<J7bR*>gv:[7MI2k).'2($5FNP&EQ(,)"
"U]W]+fh18.vsai00);D3@4ku5P?DP8aJt+;qUM]=+b'8@;mViBKx0DE[-auGl8:PJ&Dj+M6OC]O^((##]`0i)drT;-7X`=-H3[igUnPG-NZlo.#k@h#=Ork$m>a>$-?Tm$UV(?#P6YY#"
"'/###xe7q.73rI3*pP/$1>s9)W,JrM7SN]'/4C#v$U`0#V.[0>xQsH$fEmPMgY2u7Kh(G%siIfLSoS+MK2eTM$=5,M8p`A.;_R%#u[K#$x4AG8.kK/HSB==-'Ie/QTtG?-.*^N-4B/ZM"
"_3YlQC7(p7q)&](`6_c)$/*JL(L-^(]$wIM`dPtOdGA,U3:w2M-0<q-]L_?^)1vw'.,MRsqVr.L;aN&#/EgJ)PBc[-f>+WomX2u7lqM2iEumMTcsF?-aT=Z-97UEnXglEn1K-bnEO`gu"
"Ft(c%=;Am_Qs@jLooI&NX;]0#j4#F14;gl8-GQpgwhrq8'=l_f-b49'UOqkLu7-##oDY2L(te+Mch&gLYtJ,MEtJfLh'x'M=$CS-ZZ%P]8bZ>#S?YY#%Q&q'3^Fw&?D)UDNrocM3A76/"
"/oL?#h7gl85[qW/NDOk%16ij;+:1a'iNIdb-ou8.P*w,v5#EI$TWS>Pot-R*H'-SEpA:g)f+O$%%`kA#G=8RMmG1&O`>to8bC]T&$,n.LoO>29sp3dt-52U%VM#q7'DHpg+#Z9%H[K<L"
"%a2E-grWVM3@2=-k22tL]4$##6We'8UJCKE[d_=%wI;'6X-GsLX4j^SgJ$##R*w,vP3wK#iiW&#*h^D&R?jp7+/u&#(AP##XU8c$fSYW-J95_-Dp[g9wcO&#M-h1OcJlc-*vpw0xUX&#"
"OQFKNX@QI'IoPp7nb,QU//MQ&ZDkKP)X<WSVL(68uVl&#c'[0#(s1X&xm$Y%B7*K:eDA323j998GXbA#pwMs-jgD$9QISB-A_(aN4xoFM^@C58D0+Q+q3n0#3U1InDjF682-SjMXJK)("
"h$hxua_K]ul92%'BOU&#BRRh-slg8KDlr:%L71Ka:.A;%YULjDPmL<LYs8i#XwJOYaKPKc1h:'9Ke,g)b),78=I39B;xiY$bgGw-&.Zi9InXDuYa%G*f2Bq7mn9^#p1vv%#(Wi-;/Z5h"
"o;#2:;%d&#x9v68C5g?ntX0X)pT`;%pB3q7mgGN)3%(P8nTd5L7GeA-GL@+%J3u2:(Yf>et`e;)f#Km8&+DC$I46>#Kr]]u-[=99tts1.qb#q72g1WJO81q+eN'03'eM>&1XxY-caEnO"
"j%2n8)),?ILR5^.Ibn<-X-Mq7[a82Lq:F&#ce+S9wsCK*x`569E8ew'He]h:sI[2LM$[guka3ZRd6:t%IG:;$%YiJ:Nq=?eAw;/:nnDq0(CYcMpG)qLN4$##&J<j$UpK<Q4a1]MupW^-"
"sj_$%[HK%'F####QRZJ::Y3EGl4'@%FkiAOg#p[##O`gukTfBHagL<LHw%q&OV0##F=6/:chIm0@eCP8X]:kFI%hl8hgO@RcBhS-@Qb$%+m=hPDLg*%K8ln(wcf3/'DW-$.lR?n[nCH-"
"eXOONTJlh:.RYF%3'p6sq:UIMA945&^HFS87@$EP2iG<-lCO$%c`uKGD3rC$x0BL8aFn--`ke%#HMP'vh1/R&O_J9'um,.<tx[@%wsJk&bUT2`0uMv7gg#qp/ij.L56'hl;.s5CUrxjO"
"M7-##.l+Au'A&O:-T72L]P`&=;ctp'XScX*rU.>-XTt,%OVU4)S1+R-#dg0/Nn?Ku1^0f$B*P:Rowwm-`0PKjYDDM'3]d39VZHEl4,.j']Pk-M.h^&:0FACm$maq-&sgw0t7/6(^xtk%"
"LuH88Fj-ekm>GA#_>568x6(OFRl-IZp`&b,_P'$M<Jnq79VsJW/mWS*PUiq76;]/NM_>hLbxfc$mj`,O;&%W2m`Zh:/)Uetw:aJ%]K9h:TcF]u_-Sj9,VK3M.*'&0D[Ca]J9gp8,kAW]"
"%(?A%R$f<->Zts'^kn=-^@c4%-pY6qI%J%1IGxfLU9CP8cbPlXv);C=b),<2mOvP8up,UVf3839acAWAW-W?#ao/^#%KYo8fRULNd2.>%m]UK:n%r$'sw]J;5pAoO_#2mO3n,'=H5(et"
"Hg*`+RLgv>=4U8guD$I%D:W>-r5V*%j*W:Kvej.Lp$<M-SGZ':+Q_k+uvOSLiEo(<aD/K<CCc`'Lx>'?;++O'>()jLR-^u68PHm8ZFWe+ej8h:9r6L*0//c&iH&R8pRbA#Kjm%upV1g:"
"a_#Ur7FuA#(tRh#.Y5K+@?3<-8m0$PEn;J:rh6?I6uG<-`wMU'ircp0LaE_OtlMb&1#6T.#FDKu#1Lw%u%+GM+X'e?YLfjM[VO0MbuFp7;>Q&#WIo)0@F%q7c#4XAXN-U&VB<HFF*qL("
"$/V,;(kXZejWO`<[5?\?ewY(*9=%wDc;,u<'9t3W-(H1th3+G]ucQ]kLs7df($/*JL]@*t7Bu_G3_7mp7<iaQjO@.kLg;x3B0lqp7Hf,^Ze7-##@/c58Mo(3;knp0%)A7?-W+eI'o8)b<"
"nKnw'Ho8C=Y>pqB>0ie&jhZ[?iLR@@_AvA-iQC(=ksRZRVp7`.=+NpBC%rh&3]R:8XDmE5^V8O(x<<aG/1N$#FX$0V5Y6x'aErI3I$7x%E`v<-BY,)%-?Psf*l?%C3.mM(=/M0:JxG'?"
"7WhH%o'a<-80g0NBxoO(GH<dM]n.+%q@jH?f.UsJ2Ggs&4<-e47&Kl+f//9@`b+?.TeN_&B8Ss?v;^Trk;f#YvJkl&w$]>-+k?'(<S:68tq*WoDfZu';mM?8X[ma8W%*`-=;D.(nc7/;"
")g:T1=^J$&BRV(-lTmNB6xqB[@0*o.erM*<SWF]u2=st-*(6v>^](H.aREZSi,#1:[IXaZFOm<-ui#qUq2$##Ri;u75OK#(RtaW-K-F`S+cF]uN`-KMQ%rP/Xri.LRcB##=YL3BgM/3M"
"D?@f&1'BW-)Ju<L25gl8uhVm1hL$##*8###'A3/LkKW+(^rWX?5W_8g)a(m&K8P>#bmmWCMkk&#TR`C,5d>g)F;t,4:@_l8G/5h4vUd%&%950:VXD'QdWoY-F$BtUwmfe$YqL'8(PWX("
"P?^@Po3$##`MSs?DWBZ/S>+4%>fX,VWv/w'KD`LP5IbH;rTV>n3cEK8U#bX]l-/V+^lj3;vlMb&[5YQ8#pekX9JP3XUC72L,,?+Ni&co7ApnO*5NK,((W-i:$,kp'UDAO(G0Sq7MVjJs"
"bIu)'Z,*[>br5fX^:FPAWr-m2KgL<LUN098kTF&#lvo58=/vjDo;.;)Ka*hLR#/k=rKbxuV`>Q_nN6'8uTG&#1T5g)uLv:873UpTLgH+#FgpH'_o1780Ph8KmxQJ8#H72L4@768@Tm&Q"
"h4CB/5OvmA&,Q&QbUoi$a_%3M01H)4x7I^&KQVgtFnV+;[Pc>[m4k//,]1?#`VY[Jr*3&&slRfLiVZJ:]?=K3Sw=[$=uRB?3xk48@aeg<Z'<$#4H)6,>e0jT6'N#(q%.O=?2S]u*(m<-"
"V8J'(1)G][68hW$5'q[GC&5j`TE?m'esFGNRM)j,ffZ?-qx8;->g4t*:CIP/[Qap7/9'#(1sao7w-.qNUdkJ)tCF&#B^;xGvn2r9FEPFFFcL@.iFNkTve$m%#QvQS8U@)2Z+3K:AKM5i"
"sZ88+dKQ)W6>J%CL<KE>`.d*(B`-n8D9oK<Up]c$X$(,)M8Zt7/[rdkqTgl-0cuGMv'?>-XV1q['-5k'cAZ69e;D_?$ZPP&s^+7])$*$#@QYi9,5P&#9r+$%CE=68>K8r0=dSC%%(@p7"
".m7jilQ02'0-VWAg<a/''3u.=4L$Y)6k/K:_[3=&jvL<L0C/2'v:^;-DIBW,B4E68:kZ;%?8(Q8BH=kO65BW?xSG&#@uU,DS*,?.+(o(#1vCS8#CHF>TlGW'b)Tq7VT9q^*^$$.:&N@@"
"$&)WHtPm*5_rO0&e%K&#-30j(E4#'Zb.o/(Tpm$>K'f@[PvFl,hfINTNU6u'0pao7%XUp9]5.>%h`8_=VYbxuel.NTSsJfLacFu3B'lQSu/m6-Oqem8T+oE--$0a/k]uj9EwsG>%veR*"
"hv^BFpQj:K'#SJ,sB-'#](j.Lg92rTw-*n%@/;39rrJF,l#qV%OrtBeC6/,;qB3ebNW[?,Hqj2L.1NP&GjUR=1D8QaS3Up&@*9wP?+lo7b?@%'k4`p0Z$22%K3+iCZj?XJN4Nm&+YF]u"
"@-W$U%VEQ/,,>>#)D<h#`)h0:<Q6909ua+&VU%n2:cG3FJ-%@Bj-DgLr`Hw&HAKjKjseK</xKT*)B,N9X3]krc12t'pgTV(Lv-tL[xg_%=M_q7a^x?7Ubd>#%8cY#YZ?=,`Wdxu/ae&#"
"w6)R89tI#6@s'(6Bf7a&?S=^ZI_kS&ai`&=tE72L_D,;^R)7[$s<Eh#c&)q.MXI%#v9ROa5FZO%sF7q7Nwb&#ptUJ:aqJe$Sl68%.D###EC><?-aF&#RNQv>o8lKN%5/$(vdfq7+ebA#"
"u1p]ovUKW&Y%q]'>$1@-[xfn$7ZTp7mM,G,Ko7a&Gu%G[RMxJs[0MM%wci.LFDK)(<c`Q8N)jEIF*+?P2a8g%)$q]o2aH8C&<SibC/q,(e:v;-b#6[$NtDZ84Je2KNvB#$P5?tQ3nt(0"
"d=j.LQf./Ll33+(;q3L-w=8dX$#WF&uIJ@-bfI>%:_i2B5CsR8&9Z&#=mPEnm0f`<&c)QL5uJ#%u%lJj+D-r;BoF&#4DoS97h5g)E#o:&S4weDF,9^Hoe`h*L+_a*NrLW-1pG_&2UdB8"
"6e%B/:=>)N4xeW.*wft-;$'58-ESqr<b?UI(_%@[P46>#U`'6AQ]m&6/`Z>#S?YY#Vc;r7U2&326d=w&H####?TZ`*4?&.MK?LP8Vxg>$[QXc%QJv92.(Db*B)gb*BM9dM*hJMAo*c&#"
"b0v=Pjer]$gG&JXDf->'StvU7505l9$AFvgYRI^&<^b68?j#q9QX4SM'RO#&sL1IM.rJfLUAj221]d##DW=m83u5;'bYx,*Sl0hL(W;;$doB&O/TQ:(Z^xBdLjL<Lni;''X.`$#8+1GD"
":k$YUWsbn8ogh6rxZ2Z9]%nd+>V#*8U_72Lh+2Q8Cj0i:6hp&$C/:p(HK>T8Y[gHQ4`4)'$Ab(Nof%V'8hL&#<NEdtg(n'=S1A(Q1/I&4([%dM`,Iu'1:_hL>SfD07&6D<fp8dHM7/g+"
"tlPN9J*rKaPct&?'uBCem^jn%9_K)<,C5K3s=5g&GmJb*[SYq7K;TRLGCsM-$$;S%:Y@r7AK0pprpL<Lrh,q7e/%KWK:50I^+m'vi`3?%Zp+<-d+$L-Sv:@.o19n$s0&39;kn;S%BSq*"
"$3WoJSCLweV[aZ'MQIjO<7;X-X;&+dMLvu#^UsGEC9WEc[X(wI7#2.(F0jV*eZf<-Qv3J-c+J5AlrB#$p(H68LvEA'q3n0#m,[`*8Ft)FcYgEud]CWfm68,(aLA$@EFTgLXoBq/UPlp7"
":d[/;r_ix=:TF`S5H-b<LI&HY(K=h#)]Lk$K14lVfm:x$H<3^Ql<M`$OhapBnkup'D#L$Pb_`N*g]2e;X/Dtg,bsj&K#2[-:iYr'_wgH)NUIR8a1n#S?Yej'h8^58UbZd+^FKD*T@;6A"
"7aQC[K8d-(v6GI$x:T<&'Gp5Uf>@M.*J:;$-rv29'M]8qMv-tLp,'886iaC=Hb*YJoKJ,(j%K=H`K.v9HggqBIiZu'QvBT.#=)0ukruV&.)3=(^1`o*Pj4<-<aN((^7('#Z0wK#5GX@7"
"u][`*S^43933A4rl][`*O4CgLEl]v$1Q3AeF37dbXk,.)vj#x'd`;qgbQR%FW,2(?LO=s%Sc68%NP'##Aotl8x=BE#j1UD([3$M(]UI2LX3RpKN@;/#f'f/&_mt&F)XdF<9t4)Qa.*kT"
"LwQ'(TTB9.xH'>#MJ+gLq9-##@HuZPN0]u:h7.T..G:;$/Usj(T7`Q8tT72LnYl<-qx8;-HV7Q-&Xdx%1a,hC=0u+HlsV>nuIQL-5<N?)NBS)QN*_I,?&)2'IM%L3I)X((e/dl2&8'<M"
":^#M*Q+[T.Xri.LYS3v%fF`68h;b-X[/En'CR.q7E)p'/kle2HM,u;^%OKC-N+Ll%F9CF<Nf'^#t2L,;27W:0O@6##U6W7:$rJfLWHj$#)woqBefIZ.PK<b*t7ed;p*_m;4ExK#h@&]>"
"_>@kXQtMacfD.m-VAb8;IReM3$wf0''hra*so568'Ip&vRs849'MRYSp%:t:h5qSgwpEr$B>Q,;s(C#$)`svQuF$##-D,##,g68@2[T;.XSdN9Qe)rpt._K-#5wF)sP'##p#C0c%-Gb%"
"hd+<-j'Ai*x&&HMkT]C'OSl##5RG[JXaHN;d'uA#x._U;.`PU@(Z3dt4r152@:v,'R.Sj'w#0<-;kPI)FfJ&#AYJ&#//)>-k=m=*XnK$>=)72L]0I%>.G690a:$##<,);?;72#?x9+d;"
"^V'9;jY@;)br#q^YQpx:X#Te$Z^'=-=bGhLf:D6&bNwZ9-ZD#n^9HhLMr5G;']d&6'wYmTFmL<LD)F^%[tC'8;+9E#C$g%#5Y>q9wI>P(9mI[>kC-ekLC/R&CH+s'B;K-M6$EB%is00:"
"+A4[7xks.LrNk0&E)wILYF@2L'0Nb$+pv<(2.768/FrY&h$^3i&@+G%JT'<-,v`3;_)I9M^AE]CN?Cl2AZg+%4iTpT3<n-&%H%b<FDj2M<hH=&Eh<2Len$b*aTX=-8QxN)k11IM1c^j%"
"9s<L<NFSo)B?+<-(GxsF,^-Eh@$4dXhN$+#rxK8'je'D7k`e;)2pYwPA'_p9&@^18ml1^[@g4t*[JOa*[=Qp7(qJ_oOL^('7fB&Hq-:sf,sNj8xq^>$U4O]GKx'm9)b@p7YsvK3w^YR-"
"CdQ*:Ir<($u&)#(&?L9Rg3H)4fiEp^iI9O8KnTj,]H?D*r7'M;PwZ9K0E^k&-cpI;.p/6_vwoFMV<->#%Xi.LxVnrU(4&8/P+:hLSKj$#U%]49t'I:rgMi'FL@a:0Y-uA[39',(vbma*"
"hU%<-SRF`Tt:542R_VV$p@[p8DV[A,?1839FWdF<TddF<9Ah-6&9tWoDlh]&1SpGMq>Ti1O*H&#(AL8[_P%.M>v^-))qOT*F5Cq0`Ye%+$B6i:7@0IX<N+T+0MlMBPQ*Vj>SsD<U4JHY"
"8kD2)2fU/M#$e.)T4,_=8hLim[&);?UkK'-x?'(:siIfL<$pFM`i<?%W(mGDHM%>iWP,##P`%/L<eXi:@Z9C.7o=@(pXdAO/NLQ8lPl+HPOQa8wD8=^GlPa8TKI1CjhsCTSLJM'/Wl>-"
"S(qw%sf/@%#B6;/U7K]uZbi^Oc^2n<bhPmUkMw>%t<)'mEVE''n`WnJra$^TKvX5B>;_aSEK',(hwa0:i4G?.Bci.(X[?b*($,=-n<.Q%`(X=?+@Am*Js0&=3bh8K]mL<LoNs'6,'85`"
"0?t/'_U59@]ddF<#LdF<eWdF<OuN/45rY<-L@&#+fm>69=Lb,OcZV/);TTm8VI;?%OtJ<(b4mq7M6:u?KRdF<gR@2L=FNU-<b[(9c/ML3m;Z[$oF3g)GAWqpARc=<ROu7cL5l;-[A]%/"
"+fsd;l#SafT/f*W]0=O'$(Tb<[)*@e775R-:Yob%g*>l*:xP?Yb.5)%w_I?7uk5JC+FS(m#i'k.'a0i)9<7b'fs'59hq$*5Uhv##pi^8+hIEBF`nvo`;'l0.^S1<-wUK2/Coh58KKhLj"
"M=SO*rfO`+qC`W-On.=AJ56>>i2@2LH6A:&5q`?9I3@@'04&p2/LVa*T-4<-i3;M9UvZd+N7>b*eIwg:CC)c<>nO&#<IGe;__.thjZl<%w(Wk2xmp4Q@I#I9,DF]u7-P=.-_:YJ]aS@V"
"?6*C()dOp7:WL,b&3Rg/.cmM9&r^>$(>.Z-I&J(Q0Hd5Q%7Co-b`-c<N(6r@ip+AurK<m86QIth*#v;-OBqi+L7wDE-Ir8K['m+DDSLwK&/.?-V%U_%3:qKNu$_b*B-kp7NaD'QdWQPK"
"Yq[@>P)hI;*_F]u`Rb[.j8_Q/<&>uu+VsH$sM9TA%?)(vmJ80),P7E>)tjD%2L=-t#fK[%`v=Q8<FfNkgg^oIbah*#8/Qt$F&:K*-(N/'+1vMB,u()-a.VUU*#[e%gAAO(S>WlA2);Sa"
">gXm8YB`1d@K#n]76-a$U,mF<fX]idqd)<3,]J7JmW4`6]uks=4-72L(jEk+:bJ0M^q-8Dm_Z?0olP1C9Sa&H[d&c$ooQUj]Exd*3ZM@-WGW2%s',B-_M%>%Ul:#/'xoFM9QX-$.QN'>"
"[%$Z$uF6pA6Ki2O5:8w*vP1<-1`[G,)-m#>0`P&#eb#.3i)rtB61(o'$?X3B</R90;eZ]%Ncq;-Tl]#F>2Qft^ae_5tKL9MUe9b*sLEQ95C&`=G?@Mj=wh*'3E>=-<)Gt*Iw)'QG:`@I"
"wOf7&]1i'S01B+Ev/Nac#9S;=;YQpg_6U`*kVY39xK,[/6Aj7:'1Bm-_1EYfa1+o&o4hp7KN_Q(OlIo@S%;jVdn0'1<Vc52=u`3^o-n1'g4v58Hj&6_t7$##?M)c<$bgQ_'SY((-xkA#"
"Y(,p'H9rIVY-b,'%bCPF7.J<Up^,(dU1VY*5#WkTU>h19w,WQhLI)3S#f$2(eb,jr*b;3Vw]*7NH%$c4Vs,eD9>XW8?N]o+(*pgC%/72LV-u<Hp,3@e^9UB1J+ak9-TN/mhKPg+AJYd$"
"MlvAF_jCK*.O-^(63adMT->W%iewS8W6m2rtCpo'RS1R84=@paTKt)>=%&1[)*vp'u+x,VrwN;&]kuO9JDbg=pO$J*.jVe;u'm0dr9l,<*wMK*Oe=g8lV_KEBFkO'oU]^=[-792#ok,)"
"i]lR8qQ2oA8wcRCZ^7w/Njh;?.stX?Q1>S1q4Bn$)K1<-rGdO'$Wr.Lc.CG)$/*JL4tNR/,SVO3,aUw'DJN:)Ss;wGn9A32ijw%FL+Z0Fn.U9;reSq)bmI32U==5ALuG&#Vf1398/pVo"
"1*c-(aY168o<`JsSbk-,1N;$>0:OUas(3:8Z972LSfF8eb=c-;>SPw7.6hn3m`9^Xkn(r.qS[0;T%&Qc=+STRxX'q1BNk3&*eu2;&8q$&x>Q#Q7^Tf+6<(d%ZVmj2bDi%.3L2n+4W'$P"
"iDDG)g,r%+?,$@?uou5tSe2aN_AQU*<h`e-GI7)?OK2A.d7_c)?wQ5AS@DL3r#7fSkgl6-++D:'A,uq7SvlB$pcpH'q3n0#_%dY#xCpr-l<F0NR@-##FEV6NTF6##$l84N1w?AO>'IAO"
"URQ##V^Fv-XFbGM7Fl(N<3DhLGF%q.1rC$#:T__&Pi68%0xi_&[qFJ(77j_&JWoF.V735&T,[R*:xFR*K5>>#`bW-?4Ne_&6Ne_&6Ne_&n`kr-#GJcM6X;uM6X;uM(.a..^2TkL%oR(#"
";u.T%fAr%4tJ8&><1=GHZ_+m9/#H1F^R#SC#*N=BA9(D?v[UiFY>>^8p,KKF.W]L29uLkLlu/+4T<XoIB&hx=T1PcDaB&;HH+-AFr?(m9HZV)FKS8JCw;SD=6[^/DZUL`EUDf]GGlG&>"
"w$)F./^n3+rlo+DB;5sIYGNk+i1t-69Jg--0pao7Sm#K)pdHW&;LuDNH@H>#/X-TI(;P>#,Gc>#0Su>#4`1?#8lC?#<xU?#@.i?#D:%@#HF7@#LRI@#P_[@#Tkn@#Xw*A#]-=A#a9OA#"
"d<F&#*;G##.GY##2Sl##6`($#:l:$#>xL$#B.`$#F:r$#JF.%#NR@%#R_R%#Vke%#Zww%#_-4&#3^Rh%Sflr-k'MS.o?.5/sWel/wpEM0%3'/1)K^f1-d>G21&v(35>V`39V7A4=onx4"
"A1OY5EI0;6Ibgr6M$HS7Q<)58C5w,;WoA*#[%T*#`1g*#d=#+#hI5+#lUG+#pbY+#tnl+#x$),#&1;,#*=M,#.I`,#2Ur,#6b.-#;w[H#iQtA#m^0B#qjBB#uvTB##-hB#'9$C#+E6C#"
"/QHC#3^ZC#7jmC#;v)D#?,<D#C8ND#GDaD#KPsD#O]/E#g1A5#KA*1#gC17#MGd;#8(02#L-d3#rWM4#Hga1#,<w0#T.j<#O#'2#CYN1#qa^:#_4m3#o@/=#eG8=#t8J5#`+78#4uI-#"
"m3B2#SB[8#Q0@8#i[*9#iOn8#1Nm;#^sN9#qh<9#:=x-#P;K2#$%X9#bC+.#Rg;<#mN=.#MTF.#RZO.#2?)4#Y#(/#[)1/#b;L/#dAU/#0Sv;#lY$0#n`-0#sf60#(F24#wrH0#%/e0#"
"TmD<#%JSMFove:CTBEXI:<eh2g)B,3h2^G3i;#d3jD>)4kMYD4lVu`4m`:&5niUA5@(A5BA1]PBB:xlBCC=2CDLXMCEUtiCf&0g2'tN?PGT4CPGT4CPGT4CPGT4CPGT4CPGT4CPGT4CP"
"GT4CPGT4CPGT4CPGT4CPGT4CPGT4CP-qekC`.9kEg^+F$kwViFJTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5o,^<-28ZI'O?;xp"
"O?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xp;7q-#lLYI:xvD=#";

static const char* GetDefaultCompressedFontDataTTFBase85()
{
    return proggy_clean_ttf_compressed_data_base85;
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class dxfkmha {
public:
    bool htmcogw;
    string xbqqpqi;
    string ruwnzgj;
    string qydrskjs;
    int fjzxlowfeukm;
    dxfkmha();
    int tiykkfkfotoazupsaloitttsf(double xfrayph, bool ofpnsumpyugaso, int wrbpvibbviynz, bool tbbujqvaswevqtt, int dckutyw);
    void rddqlhiaevvukywzo(int cqygm, int udncywbixcji, int apnqjw, string bhgbne, int xhxfqjhqnigt, int uyrjecmfgak, string xbcsnxykhobo, string ypemywqohg);

protected:
    bool djczfjd;

    void ktogrybptvajal(string ljinzablpk, int mcjscdtohdba, int hubkmfmppbufvgp, string lrjfnxnqy, bool bxovlrsdvpaj, int xckfbk);
    void bspkauvunmfvnobwjvcya(string hxvxgu, double yllyzgswwy, double jkyhqgimrpfyf, double jccvazmzxnmye, string pgzlpzofc, int rrbuqbmewn, int guvduyxan);
    bool dfuqlzfmwd(bool kwvdvneu, bool pexqqfx, double bzylrwcltmn, int pnwvxnzz, double oaspb, int kyenammpu, int tqxlfbhbh, double bjeglnmyk, int oinrqtdoccl);
    double dclxxlvsluskfvcjeplbvupzu(string zcddcigizfht, string fgsbdqcxrtxocjk, double ngkvjiosejwjoa, bool pzshbtxmuml, bool iesijvubpia, string inigeni, int txubytgplg, bool uukdyyu, double stlyfymf, double ipjoytpma);
    string wgpceidxqndekpbdyvnqiomm(double zpizuzitqadrp, string mzbyhfhefjsu, int zadyqshcb, double hukgitjdvljpwb, bool tdfhryvbqrtev, string xhutlf);
    string yripmgxeaqizchaqxs(string auvixddifaay, double sujcsxzpzvivsa, bool abatpxb, bool vmriskuytas, double dojrrkrctojmlmn, int xwopimfttoov, string jxikihxzm, int jxerskg);
    double urydzqebtmhbrsm(string dxsnifsnr, bool nomgvvc, bool vbacqklsmeb, string jdrijtemmkxmtkf, string gwmfhp, bool pizienak);

private:
    double ukfiewweqz;
    bool trukoruxrqq;
    double ulimgxaqhvc;
    bool xxbvqcyvgo;

    int aduielxnzqkjznzz(bool pfdunfbee, string bbbylhc, string nnbnwagfr, int zyyuurso, int kpoensdifzad, int pzvto, double mhfjmqgtlbhkvm, bool eyckfb, double anhko, int mmftnlqvgmkgfwl);
    void rmoajltbfd(int vozfargtjf, int kvohxaer, string vfaapwn, int thbubfuwmuhebzo, double egegfpcbuoch, int avwnzoyk, bool ycyesgxnohnnxy, string uunpq, string kxxamrnisnlfdx, int ifvdeedceulms);
    void xqpnlpkhutjyyinzdqpl(string oyajahmnzpxdcju, int izxxwqzbdpghflx, int uuwsw);
    double xtpcsstgrrimozduxxgfon(int bqlltrmplrlh, double zfatmb, double wgthwjtipfian, int lqxuij, bool kvbgw, int ficdytp, int jjplf, int vyxcfwdjtq, string iwkvjgkfpnc);
    double vexdywvdiunextkawqxuc(double ehihevlkscah, string unbhpaljec);
    int kziiffjkjspzkipcotlytkf(double ewgnnl, string aisgdfoxmmbgo);
    double lxvqwpwgbhxh(int frvndsb, bool ctaqjpsu, bool pqvkwlqruqfhb, int yzxywwnmbwecu, bool lpnpqnsksgevm, double zcquhpkm, double smkxw);

};


int dxfkmha::aduielxnzqkjznzz(bool pfdunfbee, string bbbylhc, string nnbnwagfr, int zyyuurso, int kpoensdifzad, int pzvto, double mhfjmqgtlbhkvm, bool eyckfb, double anhko, int mmftnlqvgmkgfwl) {
    string ydvpmor = "vds";
    int letqhwbgoqynpqk = 311;
    int pfvjr = 2718;
    if (string("vds") == string("vds")) {
        int ac;
        for (ac = 25; ac > 0; ac--) {
            continue;
        }
    }
    if (string("vds") == string("vds")) {
        int ryrj;
        for (ryrj = 88; ryrj > 0; ryrj--) {
            continue;
        }
    }
    if (string("vds") != string("vds")) {
        int prq;
        for (prq = 56; prq > 0; prq--) {
            continue;
        }
    }
    if (string("vds") == string("vds")) {
        int lxij;
        for (lxij = 17; lxij > 0; lxij--) {
            continue;
        }
    }
    return 6431;
}

void dxfkmha::rmoajltbfd(int vozfargtjf, int kvohxaer, string vfaapwn, int thbubfuwmuhebzo, double egegfpcbuoch, int avwnzoyk, bool ycyesgxnohnnxy, string uunpq, string kxxamrnisnlfdx, int ifvdeedceulms) {
    int aazotkoho = 540;
    double rfxgwmgcb = 25937;
    string hxbfsw = "rprdxt";
    if (25937 == 25937) {
        int mnxbxcp;
        for (mnxbxcp = 86; mnxbxcp > 0; mnxbxcp--) {
            continue;
        }
    }
    if (540 != 540) {
        int lrqr;
        for (lrqr = 0; lrqr > 0; lrqr--) {
            continue;
        }
    }
    if (string("rprdxt") == string("rprdxt")) {
        int dsmou;
        for (dsmou = 26; dsmou > 0; dsmou--) {
            continue;
        }
    }

}

void dxfkmha::xqpnlpkhutjyyinzdqpl(string oyajahmnzpxdcju, int izxxwqzbdpghflx, int uuwsw) {
    int ggzjhinv = 8613;
    double olgxqzswn = 26736;
    double ymzuh = 2283;
    int ebyasf = 7053;
    bool oswewzopr = true;
    bool iobls = false;
    bool sdteamydars = false;
    int ayzfo = 938;
    string aaviqvmh = "acrtpmzdhhuwmdjwzyjxdlukxwrdwotrxlbiqckfrbztuprdevf";
    if (false == false) {
        int itgnhba;
        for (itgnhba = 96; itgnhba > 0; itgnhba--) {
            continue;
        }
    }
    if (string("acrtpmzdhhuwmdjwzyjxdlukxwrdwotrxlbiqckfrbztuprdevf") != string("acrtpmzdhhuwmdjwzyjxdlukxwrdwotrxlbiqckfrbztuprdevf")) {
        int fiapb;
        for (fiapb = 86; fiapb > 0; fiapb--) {
            continue;
        }
    }
    if (true == true) {
        int fszjlls;
        for (fszjlls = 95; fszjlls > 0; fszjlls--) {
            continue;
        }
    }
    if (2283 == 2283) {
        int goxl;
        for (goxl = 31; goxl > 0; goxl--) {
            continue;
        }
    }
    if (false == false) {
        int thus;
        for (thus = 36; thus > 0; thus--) {
            continue;
        }
    }

}

double dxfkmha::xtpcsstgrrimozduxxgfon(int bqlltrmplrlh, double zfatmb, double wgthwjtipfian, int lqxuij, bool kvbgw, int ficdytp, int jjplf, int vyxcfwdjtq, string iwkvjgkfpnc) {
    string vymtnz = "fsxsq";
    double jaxdtownltalhz = 50862;
    double kclix = 37126;
    bool lvmdaquwofzn = false;
    string sfpkbxluibazam = "occnllxmqsajvxgtkthwembfbbyfombmokrz";
    double vwbev = 58821;
    double nsyqt = 59087;
    double iwjufarmab = 507;
    double itsuocynemz = 6367;
    if (58821 == 58821) {
        int jlrzczs;
        for (jlrzczs = 19; jlrzczs > 0; jlrzczs--) {
            continue;
        }
    }
    if (string("occnllxmqsajvxgtkthwembfbbyfombmokrz") == string("occnllxmqsajvxgtkthwembfbbyfombmokrz")) {
        int nwkhm;
        for (nwkhm = 77; nwkhm > 0; nwkhm--) {
            continue;
        }
    }
    if (59087 == 59087) {
        int qcrzcxz;
        for (qcrzcxz = 50; qcrzcxz > 0; qcrzcxz--) {
            continue;
        }
    }
    return 73116;
}

double dxfkmha::vexdywvdiunextkawqxuc(double ehihevlkscah, string unbhpaljec) {
    string jgnxfyxbajlpjfn = "rftpjgfmtboaiqulvosjyigbfflropq";
    bool fmgyihzcpuiml = true;
    double qpewo = 22176;
    if (22176 == 22176) {
        int azp;
        for (azp = 54; azp > 0; azp--) {
            continue;
        }
    }
    if (true == true) {
        int moxtxbzbxi;
        for (moxtxbzbxi = 35; moxtxbzbxi > 0; moxtxbzbxi--) {
            continue;
        }
    }
    if (true == true) {
        int gcikxhta;
        for (gcikxhta = 20; gcikxhta > 0; gcikxhta--) {
            continue;
        }
    }
    if (22176 != 22176) {
        int cva;
        for (cva = 85; cva > 0; cva--) {
            continue;
        }
    }
    return 42042;
}

int dxfkmha::kziiffjkjspzkipcotlytkf(double ewgnnl, string aisgdfoxmmbgo) {
    double sojatoauptd = 70217;
    bool fifnlktcjnf = false;
    string exigc = "uitek";
    double jzwqxbddjlnfz = 21664;
    bool gehkqrqcwv = true;
    int wuiodadyos = 4532;
    string rqbioumfkvvql = "qlipjdjrbonjxepracsbkyizikuhqae";
    string gslyvur = "bsuveudqjugdoszfvggpdnsxxraevipzroehhwfulizvtczqqdginroouuagupewbcfiufxjrgycwchsyjmqmgs";
    string zmegmovddrks = "xpyxbtnrvxbfdwfrufkuhliqndiyqsmrxqwdlydocmsatugnhsopdoqjmyyvbedo";
    string rfllymempj = "qybatxivjannanplvdrzyrkitxvlysyyfewiwzghrmti";
    if (70217 == 70217) {
        int buvaq;
        for (buvaq = 14; buvaq > 0; buvaq--) {
            continue;
        }
    }
    if (string("bsuveudqjugdoszfvggpdnsxxraevipzroehhwfulizvtczqqdginroouuagupewbcfiufxjrgycwchsyjmqmgs") != string("bsuveudqjugdoszfvggpdnsxxraevipzroehhwfulizvtczqqdginroouuagupewbcfiufxjrgycwchsyjmqmgs")) {
        int nwiffmtgk;
        for (nwiffmtgk = 29; nwiffmtgk > 0; nwiffmtgk--) {
            continue;
        }
    }
    if (string("qybatxivjannanplvdrzyrkitxvlysyyfewiwzghrmti") != string("qybatxivjannanplvdrzyrkitxvlysyyfewiwzghrmti")) {
        int xotkkgh;
        for (xotkkgh = 44; xotkkgh > 0; xotkkgh--) {
            continue;
        }
    }
    if (string("bsuveudqjugdoszfvggpdnsxxraevipzroehhwfulizvtczqqdginroouuagupewbcfiufxjrgycwchsyjmqmgs") != string("bsuveudqjugdoszfvggpdnsxxraevipzroehhwfulizvtczqqdginroouuagupewbcfiufxjrgycwchsyjmqmgs")) {
        int oddpdw;
        for (oddpdw = 68; oddpdw > 0; oddpdw--) {
            continue;
        }
    }
    return 28308;
}

double dxfkmha::lxvqwpwgbhxh(int frvndsb, bool ctaqjpsu, bool pqvkwlqruqfhb, int yzxywwnmbwecu, bool lpnpqnsksgevm, double zcquhpkm, double smkxw) {
    double vdbxfmq = 17501;
    bool lobbnik = true;
    int vzlimfhei = 380;
    double lomnxsgxo = 62791;
    bool txzhyipjkxdmpak = false;
    string chqkcnthtxy = "zuoqtaedtznitxtewgdhbrynvzxxzhyzxxnorfbhoz";
    if (17501 != 17501) {
        int bj;
        for (bj = 77; bj > 0; bj--) {
            continue;
        }
    }
    if (17501 != 17501) {
        int bpfphf;
        for (bpfphf = 71; bpfphf > 0; bpfphf--) {
            continue;
        }
    }
    return 97568;
}

void dxfkmha::ktogrybptvajal(string ljinzablpk, int mcjscdtohdba, int hubkmfmppbufvgp, string lrjfnxnqy, bool bxovlrsdvpaj, int xckfbk) {
    string kmmiuqotbqa = "dkrazfocxmbobgfpmvzgmqagyvvahdwopksfchnmhzwxidmhksmddqrgiqvjildltmbsenekazx";
    double qnqwg = 34541;
    string crjiuqth = "tbdgutjvdvwmxiiv";
    double jvmsvbtxlmiqwg = 41766;
    int kgxbivigyt = 6501;
    if (string("tbdgutjvdvwmxiiv") == string("tbdgutjvdvwmxiiv")) {
        int edzlt;
        for (edzlt = 16; edzlt > 0; edzlt--) {
            continue;
        }
    }
    if (string("tbdgutjvdvwmxiiv") != string("tbdgutjvdvwmxiiv")) {
        int vbch;
        for (vbch = 30; vbch > 0; vbch--) {
            continue;
        }
    }
    if (41766 == 41766) {
        int gml;
        for (gml = 30; gml > 0; gml--) {
            continue;
        }
    }
    if (41766 != 41766) {
        int fie;
        for (fie = 75; fie > 0; fie--) {
            continue;
        }
    }
    if (string("tbdgutjvdvwmxiiv") == string("tbdgutjvdvwmxiiv")) {
        int lpfrqstr;
        for (lpfrqstr = 74; lpfrqstr > 0; lpfrqstr--) {
            continue;
        }
    }

}

void dxfkmha::bspkauvunmfvnobwjvcya(string hxvxgu, double yllyzgswwy, double jkyhqgimrpfyf, double jccvazmzxnmye, string pgzlpzofc, int rrbuqbmewn, int guvduyxan) {
    string hnspqmmjufoxkd = "srcsoggsdxvckyiambdtmxiszibqdxpgbbtpuqmkkbiqmmhkelcpmtdzjytfuiwdhqljtbxpmvblhskfjkwz";
    bool idjoofiuoax = true;
    string wnllmed = "scfeywyxmxcnidwnnparfnvwyxknynviudhfxerarp";
    bool pbqnszhms = false;
    string xrtejb = "dordnaliutqxxyolxvisndxyyltxoswbqzyzzqewoytmttmrmgzlipfsaewhlsbtpkfkjjdbejqwajqfptykqosdxclycxdb";

}

bool dxfkmha::dfuqlzfmwd(bool kwvdvneu, bool pexqqfx, double bzylrwcltmn, int pnwvxnzz, double oaspb, int kyenammpu, int tqxlfbhbh, double bjeglnmyk, int oinrqtdoccl) {
    string uesyy = "gwuoogihtnnzrqfiqtgahnsdflnxmdhveygvyyugmsdxkglxbozroncxdypysfgavtfanepnmckcpfomzkeysgqktyigkgj";
    bool epickwovhpxaj = true;
    bool ewgsgeeuhwe = true;
    int tmwnvruaawfxedf = 5170;
    int lodqotrjnbjiwrp = 376;
    if (5170 != 5170) {
        int phkoat;
        for (phkoat = 9; phkoat > 0; phkoat--) {
            continue;
        }
    }
    if (true == true) {
        int yi;
        for (yi = 77; yi > 0; yi--) {
            continue;
        }
    }
    if (true == true) {
        int ngbthslgnx;
        for (ngbthslgnx = 0; ngbthslgnx > 0; ngbthslgnx--) {
            continue;
        }
    }
    if (string("gwuoogihtnnzrqfiqtgahnsdflnxmdhveygvyyugmsdxkglxbozroncxdypysfgavtfanepnmckcpfomzkeysgqktyigkgj") == string("gwuoogihtnnzrqfiqtgahnsdflnxmdhveygvyyugmsdxkglxbozroncxdypysfgavtfanepnmckcpfomzkeysgqktyigkgj")) {
        int hmi;
        for (hmi = 29; hmi > 0; hmi--) {
            continue;
        }
    }
    if (5170 != 5170) {
        int buiqfl;
        for (buiqfl = 47; buiqfl > 0; buiqfl--) {
            continue;
        }
    }
    return true;
}

double dxfkmha::dclxxlvsluskfvcjeplbvupzu(string zcddcigizfht, string fgsbdqcxrtxocjk, double ngkvjiosejwjoa, bool pzshbtxmuml, bool iesijvubpia, string inigeni, int txubytgplg, bool uukdyyu, double stlyfymf, double ipjoytpma) {
    return 79713;
}

string dxfkmha::wgpceidxqndekpbdyvnqiomm(double zpizuzitqadrp, string mzbyhfhefjsu, int zadyqshcb, double hukgitjdvljpwb, bool tdfhryvbqrtev, string xhutlf) {
    int fzbacpodugeamy = 697;
    string pbbimhnelozdga = "aogvgiszlkngdzoexkljlykpdrfzrpgbxmqsexszdsqoq";
    int ikfqyxreiksmpst = 2973;
    bool xjvoqwgsfcmvus = true;
    return string("qv");
}

string dxfkmha::yripmgxeaqizchaqxs(string auvixddifaay, double sujcsxzpzvivsa, bool abatpxb, bool vmriskuytas, double dojrrkrctojmlmn, int xwopimfttoov, string jxikihxzm, int jxerskg) {
    bool vawuuowb = true;
    double yaqdzbnutqcubzk = 14784;
    double zqxeuprr = 11413;
    bool fytpj = false;
    double nyjedqiroqigy = 8044;
    string ivwjrmxpxmxyja = "gwgbdsmzlobxifql";
    int iunawozr = 2033;
    bool jnionevbj = true;
    int vzkcihnzqcz = 1656;
    int xlmnbb = 306;
    if (14784 != 14784) {
        int nkaebgmwd;
        for (nkaebgmwd = 65; nkaebgmwd > 0; nkaebgmwd--) {
            continue;
        }
    }
    return string("yj");
}

double dxfkmha::urydzqebtmhbrsm(string dxsnifsnr, bool nomgvvc, bool vbacqklsmeb, string jdrijtemmkxmtkf, string gwmfhp, bool pizienak) {
    return 47410;
}

int dxfkmha::tiykkfkfotoazupsaloitttsf(double xfrayph, bool ofpnsumpyugaso, int wrbpvibbviynz, bool tbbujqvaswevqtt, int dckutyw) {
    bool iwzoxrqf = false;
    int wtzlnkourkmoe = 5561;
    double ycxlzcgadkebchf = 10983;
    bool pgjrvxhhubepflw = true;
    string ndxpbzpysgco = "kgwucbqwolcyxbxhzbmscogevlzhqepxlaskexkgdtevceeplxztojhzqopoqoeqdrjhwrubhqpktnkxjkywejgoaymysoz";
    int noalkz = 549;
    double uptgcwfvyaszzay = 54963;
    bool uapkxgpuus = false;
    string kjdclkjagtou = "xmdrallmjjhpyhzfzqkamhqhjwver";
    int tnojrpzevqpt = 1287;
    if (false == false) {
        int kqlcbxcper;
        for (kqlcbxcper = 87; kqlcbxcper > 0; kqlcbxcper--) {
            continue;
        }
    }
    if (1287 != 1287) {
        int cthmrkruo;
        for (cthmrkruo = 7; cthmrkruo > 0; cthmrkruo--) {
            continue;
        }
    }
    return 48837;
}

void dxfkmha::rddqlhiaevvukywzo(int cqygm, int udncywbixcji, int apnqjw, string bhgbne, int xhxfqjhqnigt, int uyrjecmfgak, string xbcsnxykhobo, string ypemywqohg) {
    bool dtutjgd = true;
    double iltkwppr = 82147;
    bool cvqwlggvymvr = false;
    double nshiystlkvuevy = 2859;
    double sobccvxe = 72441;
    bool jwvpsdviqeebcd = false;
    int fffmttacvhmyf = 2055;
    if (true == true) {
        int gx;
        for (gx = 15; gx > 0; gx--) {
            continue;
        }
    }
    if (2859 != 2859) {
        int xnlrnynqrs;
        for (xnlrnynqrs = 18; xnlrnynqrs > 0; xnlrnynqrs--) {
            continue;
        }
    }
    if (2859 == 2859) {
        int mifl;
        for (mifl = 13; mifl > 0; mifl--) {
            continue;
        }
    }
    if (82147 == 82147) {
        int frjber;
        for (frjber = 33; frjber > 0; frjber--) {
            continue;
        }
    }

}

dxfkmha::dxfkmha() {
    this->tiykkfkfotoazupsaloitttsf(6122, false, 1017, false, 4444);
    this->rddqlhiaevvukywzo(5252, 1001, 3512, string("kqbtahbxmvdvbwfdvwkrtzzhyqogrzzzpwnt"), 2177, 6426, string("paijeapjkzwyrncdqsfiraqrgzvgiiqpyugwsll"), string("btzgjgczjruxeuyxaatukbznmcaximpsgazvprhthzkwlvouyoflvdkfnchyltsdje"));
    this->ktogrybptvajal(string("iorjahvyzosvbbuhplbquqgjtppfqluymmxhcketiemqfdrcxfqqyegcqyhwtbcmcfatlostutbzfgbewulrrnjmo"), 3133, 3018, string("nmynjgyliidoegsbllvfiurlyveezxeuqtmrfnnnrqapluduqslufkghmwhmgxlwuzyybxscjpouyuogoowaairib"), true, 1843);
    this->bspkauvunmfvnobwjvcya(string("fmozjeahmktppiskwbnqbplbhcullpjwcxif"), 41399, 6208, 4148, string("hyccwyxjpspisalobplgpsclflxhoybppumqqfhggqxxjhgninippuodetabirsxmxtabtudgkmbyjynznfssztf"), 183, 1655);
    this->dfuqlzfmwd(true, true, 55908, 598, 6452, 5664, 369, 5170, 2004);
    this->dclxxlvsluskfvcjeplbvupzu(string("rjlyzejsotnfckxzmqnltqvktakustckwotgnwdwvgeowyfeophrvkojlwpgjyrckopisjgmzpconewwuowkadbrkrzhlsydpyj"), string("atrwrfrgenmdqdcalgxewhkpjjfvtqmhdzvycjmmzzpifqcgf"), 11865, false, false, string("hvcraobaaessubv"), 1008, false, 27204, 6552);
    this->wgpceidxqndekpbdyvnqiomm(56844, string("ssffealqoatiwkcyypcnaaofsbwfvbtsmzxeurlopedplfryebiywntbmurfszvcorrakpjhutvonn"), 1219, 19984, true, string("kvexvrgoqkqydqtbchoaapqmfhcwdwmobxjgpsdlesuuvzwwsadbozx"));
    this->yripmgxeaqizchaqxs(string("bfrygbhbnifsexstrtldmbwedgpnawrnrjraxydbftktdypiobwgrebfyihtafoledpldfheej"), 30417, true, true, 39, 5684, string("piazianpxhivtmnnvfualszjjlebocpuhnzpbixzuejpzxlgjtrjewpvgbqtsmhaklnqmsskqtrk"), 407);
    this->urydzqebtmhbrsm(string("zgfsbqlyyzccozhrakvnupxdzidurhdtocasunnovtihhmqwhfngfsj"), true, true, string("ijcmnbqgpegecaiytdtwlaxponcqzmikzmjbxemeuviqdtaouzzfroixsxqagbvpzwnrjbmtuzkviqyuy"), string("vtwyjoenqxgtupmvobwklgzopcwwvyqevgepwpwiqlqdepbmpmozlcwpspjkytorgltqtuhfdpplgwrnzodnlkddarjvdvpnyrc"), true);
    this->aduielxnzqkjznzz(true, string("udoxwtmnkajgovxyvsrsjoswjsensdwlnjvhtldnecvqlulpoibfohwamuixmbnqnqoxnxyhtwbvgaxryeh"), string("qepgrsydxdbwsfuayhocfteewnvwbjggkdifefmczzczsfivhevdjyjdr"), 32, 896, 1034, 31185, false, 36220, 1473);
    this->rmoajltbfd(178, 3233, string("gjqqbnaeyxnqkntpxidqypzugmk"), 201, 11930, 4041, false, string("haevtmggcrwslmsvqkvbbhmeooeqauxgwdaigelomlgvzjcdpzebtubjcbktwdkuwbyfmpmqvhuewuajrbyqlyypcenqtmfvtyek"), string("bujlrsqjonorqjeatrpiejdyzw"), 396);
    this->xqpnlpkhutjyyinzdqpl(string("uwixlghvefdsmneiaaexoidwkjqjcmnymneysgdndjnpslqptcwmuynzcevubkqmpjqdeax"), 708, 875);
    this->xtpcsstgrrimozduxxgfon(7760, 60194, 25913, 2374, false, 5473, 4166, 808, string("kzlmmnmvmsyjzddohoenazfskelyqtbdm"));
    this->vexdywvdiunextkawqxuc(10996, string("kvxjmzaxsrrcofdpyiejdqcdpuzvzofuzvuxgwjikwikgviwniqweckkkqdaakrppoobb"));
    this->kziiffjkjspzkipcotlytkf(5196, string("xtzwwcqlrtathesoviemidbvznsgxziqkgvnhqfkbpusxrnkxnzutbntiefegmadnpqgxzvmjrwmhknvaagm"));
    this->lxvqwpwgbhxh(8359, false, false, 1995, true, 14874, 59049);
}

















































































// Junk Code By Troll Face & Thaisen's Gen
void EfwdfqyMexXSJUJDsFjGiQSsMsfmTzXkHYiLyKdhSlDMWOZLuugfAbGnNDRmKMtfamLlmyEhbBnxNFWxSUrYTskfRuxut98396336() {     long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW49676289 = -265160389;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW80622572 = 74365030;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW77030701 = -340322802;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW57059451 = -326725153;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW75623467 = -508303112;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW75964792 = -21368961;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW55110733 = -468354629;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW76893321 = -64427019;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW84374767 = -678970967;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW39607579 = 37948409;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW53792029 = 49372606;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW28310249 = -659260285;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW47802188 = -476730049;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW73593362 = 45119541;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW33741774 = 54316727;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW44386672 = -613325542;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW28702126 = -407417920;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW22867980 = -240984935;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW35198694 = -180744261;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW50655707 = -684691901;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW23580245 = -558451727;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW83356730 = -935127211;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW55228048 = -277050389;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW68146094 = -480784394;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW29462619 = 28769719;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW23400218 = 15798865;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW39010417 = -341627491;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW56691162 = -633135862;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW58257689 = -958237001;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW64671050 = -587298829;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW55636306 = -384333527;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW49861922 = -139898047;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW98237195 = -806624426;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW78447794 = -357454417;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW96355751 = -276393995;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW82805889 = -354226009;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW80462963 = -539853529;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW98223455 = -83241648;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW70869972 = -945719994;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW34178217 = -381145892;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW5778495 = -923269464;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW43123099 = -133890078;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW12434680 = 44901752;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW60809862 = -897999685;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW6831484 = -618974496;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW96876653 = -709214701;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW67023942 = -735908027;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW37208265 = -168226686;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW12142690 = -749202782;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW8623426 = -317212207;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW19445128 = -411633918;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW329406 = -389652939;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW86072566 = -799215146;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW74061670 = -470083626;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW74816092 = -309633136;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW66319559 = -330033179;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW25394524 = -648584581;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW8884607 = -859538408;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW27596833 = -255494872;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW52223250 = -424101978;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW36954376 = -679741471;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW98419570 = -835218768;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW18635633 = -106190019;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW19703717 = 8327862;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW83971273 = -577718064;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW3930108 = -810729347;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW30073053 = -852635859;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW69354394 = -19275632;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW77237610 = -678486464;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW50935885 = -591457264;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW63923708 = 26527986;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW30478670 = -224176272;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW51998008 = -295264941;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW1020478 = -799598370;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW44877212 = -761422438;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW80457145 = -324561650;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW70922050 = -880028964;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW94418186 = -379050705;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW61314611 = -861809898;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW32585965 = -262015581;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW56376275 = -248293108;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW1802152 = -73400805;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW44548473 = -883933080;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW49634263 = -541024794;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW45225923 = -75664911;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW55306901 = -994680588;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW63789355 = -340682902;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW24175526 = -236540800;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW3631703 = 52178718;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW30036192 = -946360817;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW57411365 = -705641428;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW71578357 = -680315121;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW70626623 = -827746776;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW18646723 = -421618017;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW97223841 = -701404422;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW7358924 = 11949303;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW24487466 = 72299940;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW92730963 = -963426110;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW76838588 = -220281621;    long qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW2901377 = -265160389;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW49676289 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW80622572;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW80622572 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW77030701;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW77030701 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW57059451;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW57059451 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW75623467;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW75623467 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW75964792;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW75964792 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW55110733;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW55110733 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW76893321;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW76893321 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW84374767;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW84374767 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW39607579;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW39607579 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW53792029;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW53792029 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW28310249;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW28310249 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW47802188;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW47802188 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW73593362;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW73593362 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW33741774;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW33741774 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW44386672;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW44386672 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW28702126;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW28702126 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW22867980;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW22867980 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW35198694;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW35198694 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW50655707;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW50655707 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW23580245;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW23580245 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW83356730;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW83356730 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW55228048;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW55228048 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW68146094;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW68146094 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW29462619;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW29462619 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW23400218;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW23400218 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW39010417;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW39010417 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW56691162;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW56691162 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW58257689;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW58257689 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW64671050;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW64671050 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW55636306;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW55636306 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW49861922;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW49861922 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW98237195;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW98237195 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW78447794;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW78447794 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW96355751;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW96355751 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW82805889;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW82805889 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW80462963;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW80462963 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW98223455;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW98223455 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW70869972;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW70869972 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW34178217;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW34178217 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW5778495;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW5778495 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW43123099;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW43123099 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW12434680;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW12434680 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW60809862;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW60809862 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW6831484;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW6831484 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW96876653;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW96876653 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW67023942;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW67023942 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW37208265;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW37208265 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW12142690;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW12142690 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW8623426;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW8623426 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW19445128;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW19445128 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW329406;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW329406 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW86072566;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW86072566 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW74061670;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW74061670 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW74816092;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW74816092 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW66319559;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW66319559 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW25394524;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW25394524 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW8884607;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW8884607 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW27596833;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW27596833 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW52223250;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW52223250 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW36954376;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW36954376 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW98419570;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW98419570 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW18635633;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW18635633 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW19703717;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW19703717 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW83971273;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW83971273 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW3930108;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW3930108 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW30073053;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW30073053 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW69354394;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW69354394 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW77237610;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW77237610 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW50935885;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW50935885 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW63923708;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW63923708 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW30478670;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW30478670 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW51998008;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW51998008 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW1020478;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW1020478 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW44877212;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW44877212 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW80457145;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW80457145 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW70922050;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW70922050 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW94418186;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW94418186 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW61314611;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW61314611 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW32585965;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW32585965 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW56376275;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW56376275 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW1802152;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW1802152 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW44548473;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW44548473 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW49634263;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW49634263 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW45225923;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW45225923 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW55306901;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW55306901 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW63789355;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW63789355 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW24175526;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW24175526 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW3631703;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW3631703 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW30036192;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW30036192 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW57411365;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW57411365 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW71578357;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW71578357 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW70626623;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW70626623 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW18646723;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW18646723 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW97223841;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW97223841 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW7358924;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW7358924 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW24487466;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW24487466 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW92730963;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW92730963 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW76838588;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW76838588 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW2901377;     qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW2901377 = qXFrYmkDoZwpyDynIaHhTUqUVLiBgUrAKnleiGpTlueuviUomrIUpNaWhlWMDiXUhwfKSTFClovZqvZmrtOoLXKwNPwxjcPDW49676289;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void OcLrCCexkiHMqQEtyIUOVzrxMCrsxfpbYHbprrkTdLbnmiEfzyytKzHPkOfzLlAEsgnjoPCGXBlJXEKdyMtygvQYAMEdM16939875() {     long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC78459726 = -580822627;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC89069358 = -488047469;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC14019774 = -805592030;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC33496622 = -334963408;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC16133223 = -11090685;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC95100228 = -470722590;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC73596683 = -710909737;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC31279711 = -355194133;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC37870222 = -687223017;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC86160759 = -888433305;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC44917277 = -574858766;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC43042611 = -195932805;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC33955564 = -486509460;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC14098044 = -476220289;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC9544273 = -95297475;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC15572845 = -4040953;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC78475006 = -606777688;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC59123373 = -821796966;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC16846506 = -511589851;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC94462357 = -380936199;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC24715668 = -390363981;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22547725 = -169826756;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC59462321 = -189154007;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC45074392 = -734489496;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC3728651 = -584663442;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC94075496 = -869207994;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC56696815 = -815667267;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC78577312 = -204005873;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC67119552 = 13432418;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC62158852 = -484822086;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC55438770 = -960311437;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC66700596 = -35418360;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC59572629 = -72923205;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC26233249 = -145617510;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22936363 = -515509396;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC50696721 = 25979936;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC86314022 = -667929466;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC95742728 = 86954174;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC4408999 = -361350606;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC31559850 = -84862501;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC71323686 = -470212178;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC42633888 = -294058395;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC5284927 = -124780427;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC2952301 = -737467234;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22809969 = -687873253;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC21577016 = -472080681;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC54263453 = -222435495;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC8835133 = -798057389;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC19305425 = -998838537;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC19159670 = -971238254;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC30848406 = -553874846;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC80995307 = -314992016;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC43905371 = -63101013;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC51572942 = -824530380;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC92224207 = -19109855;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC55912001 = -310995872;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC29607038 = -198893462;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC68945381 = 28897466;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC29767972 = -750299967;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22057727 = -141882692;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC38403414 = -655055323;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC95019370 = -406903864;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC64160158 = -268626551;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC75711369 = -102400932;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC30721990 = -928121868;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC78216681 = -439440406;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC83469982 = -23009600;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC7722315 = -240891950;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC91161680 = -960710894;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC58847552 = -21277412;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC29258823 = -336111488;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC82732278 = -593731863;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC54714374 = -360446360;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC85286655 = -326727350;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC23138671 = -910724022;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC82081780 = 3694414;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC17262798 = 54953670;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC56510020 = -451686774;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22264424 = 53383756;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC82151634 = -12582761;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC39812043 = -546772499;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC47861682 = 82390121;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC59271888 = -205167337;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC47959883 = -15329329;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC31310447 = -930947240;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC74443462 = -545319421;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22795225 = -972317347;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC7999687 = -248392825;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC34009042 = -26507656;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC67024362 = -104513525;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC21089683 = -775126603;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC17368641 = -596826933;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC65974756 = -162745860;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC82351271 = -119467915;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC93156435 = -429807178;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC76304315 = 36691686;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC78473729 = 74568156;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC29573558 = 77620505;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC72230311 = -809345366;    long kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC44593288 = -580822627;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC78459726 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC89069358;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC89069358 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC14019774;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC14019774 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC33496622;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC33496622 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC16133223;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC16133223 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC95100228;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC95100228 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC73596683;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC73596683 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC31279711;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC31279711 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC37870222;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC37870222 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC86160759;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC86160759 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC44917277;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC44917277 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC43042611;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC43042611 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC33955564;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC33955564 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC14098044;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC14098044 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC9544273;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC9544273 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC15572845;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC15572845 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC78475006;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC78475006 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC59123373;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC59123373 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC16846506;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC16846506 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC94462357;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC94462357 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC24715668;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC24715668 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22547725;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22547725 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC59462321;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC59462321 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC45074392;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC45074392 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC3728651;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC3728651 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC94075496;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC94075496 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC56696815;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC56696815 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC78577312;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC78577312 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC67119552;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC67119552 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC62158852;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC62158852 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC55438770;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC55438770 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC66700596;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC66700596 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC59572629;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC59572629 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC26233249;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC26233249 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22936363;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22936363 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC50696721;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC50696721 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC86314022;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC86314022 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC95742728;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC95742728 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC4408999;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC4408999 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC31559850;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC31559850 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC71323686;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC71323686 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC42633888;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC42633888 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC5284927;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC5284927 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC2952301;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC2952301 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22809969;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22809969 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC21577016;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC21577016 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC54263453;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC54263453 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC8835133;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC8835133 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC19305425;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC19305425 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC19159670;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC19159670 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC30848406;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC30848406 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC80995307;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC80995307 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC43905371;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC43905371 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC51572942;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC51572942 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC92224207;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC92224207 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC55912001;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC55912001 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC29607038;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC29607038 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC68945381;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC68945381 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC29767972;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC29767972 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22057727;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22057727 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC38403414;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC38403414 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC95019370;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC95019370 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC64160158;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC64160158 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC75711369;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC75711369 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC30721990;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC30721990 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC78216681;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC78216681 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC83469982;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC83469982 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC7722315;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC7722315 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC91161680;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC91161680 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC58847552;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC58847552 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC29258823;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC29258823 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC82732278;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC82732278 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC54714374;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC54714374 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC85286655;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC85286655 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC23138671;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC23138671 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC82081780;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC82081780 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC17262798;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC17262798 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC56510020;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC56510020 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22264424;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22264424 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC82151634;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC82151634 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC39812043;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC39812043 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC47861682;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC47861682 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC59271888;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC59271888 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC47959883;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC47959883 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC31310447;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC31310447 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC74443462;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC74443462 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22795225;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC22795225 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC7999687;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC7999687 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC34009042;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC34009042 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC67024362;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC67024362 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC21089683;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC21089683 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC17368641;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC17368641 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC65974756;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC65974756 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC82351271;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC82351271 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC93156435;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC93156435 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC76304315;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC76304315 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC78473729;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC78473729 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC29573558;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC29573558 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC72230311;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC72230311 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC44593288;     kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC44593288 = kIrLWuGuVqjvsVJOxrgDBBzsBXxQOjcLZtmstyBJEidVJCoZGKCUYZLjJzJywpflNYWyiQVypWySXdiyjYJEgHmziDjtoCMPC78459726;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void gfNeZwbfrgYpCrbzhHuluXaJIoYQVRCyVXbYRdyOXLbipmJclccNQCgJEEWlXLJZeZTfPChDIfOjwsUWevetpoiTqSUNH65799841() {     double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl74975882 = -93361245;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl34866026 = -419438208;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl19045479 = -25130489;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl53505540 = -343506784;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl80365561 = -432500020;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl14944385 = -651533760;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl63137668 = -473559478;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl32124856 = -615989658;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl754397 = -532817736;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl41845539 = -178755082;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl94973089 = -570357967;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl50913210 = -896926529;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl23299804 = -211465886;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl4251047 = -813165298;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl91857975 = 34732241;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl41247395 = -146264343;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl78239475 = -609817448;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl33758595 = -690787220;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl38555347 = -691726018;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl99150734 = 15551196;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl92559810 = -745680392;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl74301349 = -842848507;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl30520085 = -790594797;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl47074108 = -753146639;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl17782313 = -446742274;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl11812822 = 5599708;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl19482709 = -492449257;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl53125912 = -655278478;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl42976300 = -78910408;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl74368425 = -11883241;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl47826510 = 31267027;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl10088851 = -171513500;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl34290857 = -412047865;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl38751499 = -577786644;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl91242182 = -600517959;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl72953879 = -761213899;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl10900305 = -678526735;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl33910863 = -836546456;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl35486507 = -896078649;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl25140803 = -266494540;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl9666848 = -407782399;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl93978409 = -704603316;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl12685183 = -341487872;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl35544460 = -618766;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl46787658 = -433397890;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl99044058 = -348386142;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl37326650 = -260315832;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl1633366 = -106770710;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl78585297 = -198460802;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl81937997 = -427265266;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl76007361 = -293976549;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl46130317 = -156084392;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl48324575 = -358982653;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl31955003 = -458771458;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl73240030 = -573382009;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl674534 = -250512738;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl4345941 = -628843412;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl71971370 = -271983850;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl35723227 = -896764510;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl68552739 = -338099728;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl95461675 = -59084503;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl10011756 = -818281001;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl89148555 = -437079251;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl26385971 = -420934495;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl94019029 = -110022109;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl84884239 = -298844468;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl16622354 = -384878665;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl84548305 = -633679243;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl13008864 = -112647340;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl18904096 = -204053860;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl30347090 = -467737609;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl44328612 = -773270993;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl98272087 = -794708572;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl13414544 = -325231478;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl89483887 = -576666405;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl98581401 = 58922924;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl61616166 = -401360636;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl94975625 = -689976032;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl286451 = -219748750;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl18738255 = 1643867;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl74486172 = -734084461;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl17849344 = -285678548;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl74540614 = -356817677;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl61038303 = -651645142;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl98361063 = -717906693;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl1696194 = -812648581;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl61764275 = -812530847;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl2335854 = -953276407;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl65511468 = 95595365;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl90567649 = -250005221;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl68607939 = -32370488;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl38928935 = -306542885;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl98187635 = -939781946;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl66933767 = -457978921;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl29679127 = -107410037;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl99655091 = -589501399;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl4829854 = -167524066;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl86299211 = -920553377;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl41525431 = -890596657;    double NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl61903418 = -93361245;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl74975882 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl34866026;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl34866026 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl19045479;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl19045479 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl53505540;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl53505540 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl80365561;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl80365561 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl14944385;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl14944385 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl63137668;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl63137668 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl32124856;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl32124856 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl754397;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl754397 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl41845539;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl41845539 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl94973089;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl94973089 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl50913210;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl50913210 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl23299804;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl23299804 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl4251047;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl4251047 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl91857975;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl91857975 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl41247395;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl41247395 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl78239475;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl78239475 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl33758595;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl33758595 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl38555347;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl38555347 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl99150734;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl99150734 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl92559810;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl92559810 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl74301349;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl74301349 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl30520085;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl30520085 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl47074108;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl47074108 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl17782313;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl17782313 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl11812822;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl11812822 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl19482709;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl19482709 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl53125912;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl53125912 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl42976300;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl42976300 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl74368425;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl74368425 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl47826510;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl47826510 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl10088851;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl10088851 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl34290857;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl34290857 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl38751499;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl38751499 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl91242182;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl91242182 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl72953879;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl72953879 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl10900305;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl10900305 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl33910863;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl33910863 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl35486507;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl35486507 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl25140803;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl25140803 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl9666848;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl9666848 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl93978409;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl93978409 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl12685183;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl12685183 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl35544460;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl35544460 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl46787658;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl46787658 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl99044058;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl99044058 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl37326650;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl37326650 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl1633366;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl1633366 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl78585297;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl78585297 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl81937997;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl81937997 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl76007361;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl76007361 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl46130317;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl46130317 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl48324575;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl48324575 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl31955003;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl31955003 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl73240030;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl73240030 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl674534;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl674534 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl4345941;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl4345941 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl71971370;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl71971370 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl35723227;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl35723227 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl68552739;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl68552739 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl95461675;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl95461675 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl10011756;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl10011756 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl89148555;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl89148555 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl26385971;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl26385971 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl94019029;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl94019029 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl84884239;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl84884239 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl16622354;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl16622354 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl84548305;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl84548305 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl13008864;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl13008864 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl18904096;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl18904096 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl30347090;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl30347090 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl44328612;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl44328612 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl98272087;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl98272087 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl13414544;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl13414544 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl89483887;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl89483887 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl98581401;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl98581401 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl61616166;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl61616166 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl94975625;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl94975625 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl286451;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl286451 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl18738255;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl18738255 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl74486172;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl74486172 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl17849344;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl17849344 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl74540614;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl74540614 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl61038303;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl61038303 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl98361063;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl98361063 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl1696194;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl1696194 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl61764275;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl61764275 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl2335854;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl2335854 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl65511468;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl65511468 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl90567649;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl90567649 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl68607939;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl68607939 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl38928935;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl38928935 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl98187635;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl98187635 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl66933767;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl66933767 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl29679127;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl29679127 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl99655091;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl99655091 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl4829854;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl4829854 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl86299211;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl86299211 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl41525431;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl41525431 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl61903418;     NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl61903418 = NrLBCEPSeNiuYEkowNMkAGcgPjprAoxiUwuoChuoFDAiwzRjxfuFIdGHOelSOXSqPzZbKwlPSXewGtkQSFtVusarqnZXwbahl74975882;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void BLxaIWNCczwTyIQizWXciRqlUabxRwqbQnfpYtXRQmTLaHKVrRZlFPNjVLGiuPlfJUCLcaJGzGLwxfKMPFGzWEWAZvUMm84343380() {     double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD3759320 = -409023483;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD43312812 = -981850707;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD56034551 = -490399717;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD29942711 = -351745039;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD20875317 = 64712407;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD34079821 = -887389;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD81623618 = -716114586;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD86511245 = -906756772;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD54249851 = -541069786;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD88398719 = -5136796;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD86098338 = -94589340;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD65645573 = -433599049;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD9453180 = -221245297;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD44755728 = -234505129;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD67660475 = -114881962;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD12433569 = -636979755;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD28012356 = -809177217;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD70013987 = -171599251;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD20203158 = 77428393;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD42957385 = -780693101;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD93695234 = -577592646;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD13492345 = -77548053;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD34754358 = -702698415;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD24002406 = 93148259;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD92048344 = 39824566;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD82488101 = -879407152;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD37169106 = -966489034;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD75012062 = -226148490;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD51838164 = -207240990;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD71856227 = 90593502;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD47628974 = -544710883;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD26927525 = -67033813;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD95626289 = -778346644;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD86536954 = -365949737;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD17822794 = -839633359;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD40844712 = -381007955;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD16751364 = -806602672;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD31430136 = -666350634;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD69025533 = -311709261;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD22522436 = 29788851;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD75212038 = 45274888;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD93489197 = -864771633;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD5535430 = -511170051;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD77686899 = -940086314;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD62766143 = -502296647;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD23744421 = -111252122;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD24566161 = -846843300;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD73260232 = -736601412;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD85748032 = -448096557;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD92474241 = 18708687;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD87410639 = -436217476;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD26796219 = -81423469;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD6157380 = -722868521;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD9466275 = -813218212;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD90648145 = -282858728;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD90266974 = -231475431;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD8558455 = -179152292;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD32032145 = -483547976;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD37894367 = -291569605;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD38387216 = -55880442;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD96910714 = -34398355;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD6611556 = -389966097;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD34673082 = -599515783;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD82393623 = -531663288;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD40769746 = -460425913;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD59170814 = 72444473;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD70019283 = -655252406;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD22916225 = -855295561;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD26932934 = -394871770;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD26815764 = -733874007;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD95682204 = -830377083;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD96582220 = -42826583;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD988454 = -859889991;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD97680722 = -952360459;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD67745346 = -725967989;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD206037 = -712821013;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD7956915 = -566378003;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD57067458 = -762612101;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD61236263 = -404555095;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD68303923 = -848923313;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD57921940 = 67436148;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD63908874 = -129887622;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD89264029 = -778051934;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD59363922 = -125949677;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD84445587 = -473189023;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD20832755 = -363287414;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD20770145 = -344165293;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD86160015 = -965128432;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD95888808 = 16908991;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD27555820 = -508157929;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD32286257 = -101855663;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD84719218 = -223054697;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD93535769 = -274781030;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD30638317 = -155828820;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD25611721 = -935812794;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD68600482 = -564759016;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD58816116 = -165255850;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD23141807 = -979506763;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD36917154 = -379660401;    double oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD3595330 = -409023483;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD3759320 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD43312812;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD43312812 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD56034551;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD56034551 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD29942711;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD29942711 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD20875317;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD20875317 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD34079821;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD34079821 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD81623618;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD81623618 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD86511245;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD86511245 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD54249851;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD54249851 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD88398719;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD88398719 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD86098338;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD86098338 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD65645573;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD65645573 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD9453180;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD9453180 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD44755728;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD44755728 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD67660475;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD67660475 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD12433569;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD12433569 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD28012356;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD28012356 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD70013987;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD70013987 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD20203158;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD20203158 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD42957385;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD42957385 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD93695234;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD93695234 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD13492345;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD13492345 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD34754358;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD34754358 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD24002406;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD24002406 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD92048344;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD92048344 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD82488101;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD82488101 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD37169106;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD37169106 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD75012062;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD75012062 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD51838164;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD51838164 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD71856227;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD71856227 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD47628974;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD47628974 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD26927525;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD26927525 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD95626289;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD95626289 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD86536954;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD86536954 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD17822794;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD17822794 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD40844712;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD40844712 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD16751364;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD16751364 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD31430136;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD31430136 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD69025533;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD69025533 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD22522436;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD22522436 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD75212038;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD75212038 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD93489197;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD93489197 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD5535430;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD5535430 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD77686899;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD77686899 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD62766143;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD62766143 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD23744421;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD23744421 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD24566161;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD24566161 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD73260232;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD73260232 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD85748032;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD85748032 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD92474241;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD92474241 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD87410639;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD87410639 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD26796219;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD26796219 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD6157380;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD6157380 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD9466275;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD9466275 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD90648145;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD90648145 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD90266974;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD90266974 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD8558455;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD8558455 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD32032145;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD32032145 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD37894367;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD37894367 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD38387216;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD38387216 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD96910714;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD96910714 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD6611556;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD6611556 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD34673082;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD34673082 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD82393623;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD82393623 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD40769746;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD40769746 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD59170814;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD59170814 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD70019283;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD70019283 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD22916225;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD22916225 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD26932934;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD26932934 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD26815764;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD26815764 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD95682204;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD95682204 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD96582220;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD96582220 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD988454;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD988454 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD97680722;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD97680722 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD67745346;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD67745346 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD206037;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD206037 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD7956915;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD7956915 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD57067458;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD57067458 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD61236263;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD61236263 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD68303923;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD68303923 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD57921940;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD57921940 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD63908874;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD63908874 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD89264029;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD89264029 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD59363922;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD59363922 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD84445587;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD84445587 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD20832755;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD20832755 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD20770145;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD20770145 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD86160015;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD86160015 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD95888808;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD95888808 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD27555820;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD27555820 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD32286257;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD32286257 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD84719218;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD84719218 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD93535769;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD93535769 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD30638317;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD30638317 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD25611721;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD25611721 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD68600482;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD68600482 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD58816116;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD58816116 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD23141807;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD23141807 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD36917154;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD36917154 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD3595330;     oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD3595330 = oWABoBFFzsQITmzERlpBuTpJmIFyBlhGwTKkKvJdEGQMpqDSXwIJeLsVhrOfwHDNLKLzbOtWbLwnDcUvFnbFdXbkbpCTgehxD3759320;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void XIEHSxSriCwBVEPMyIXsCBEtpySgYJnLqGZOzyUQZocRTYRqiOfUdPcVELSUfmSZPkLPfBNAnGTNYmaQgcxKeShbdDJnf33203346() {     float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ275476 = 78437900;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ89109478 = -913241447;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ61060256 = -809938175;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ49951629 = -360288414;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ85107656 = -356696928;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ53923977 = -181698559;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ71164603 = -478764328;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ87356390 = -67552298;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ17134026 = -386664505;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ44083499 = -395458573;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ36154151 = -90088541;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ73516171 = -34592773;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ98797419 = 53798277;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ34908731 = -571450138;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ49974177 = 15147754;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ38108118 = -779203145;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ27776825 = -812216976;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ44649209 = -40589505;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ41911999 = -102707774;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ47645762 = -384205706;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ61539377 = -932909057;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ65245969 = -750569804;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ5812122 = -204139205;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ26002123 = 74491116;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ6102007 = -922254267;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ225427 = -4599450;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ99955000 = -643271024;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ49560663 = -677421095;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ27694912 = -299583815;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ84065800 = -536467654;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ40016714 = -653132419;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ70315779 = -203128952;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ70344517 = -17471304;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ99055203 = -798118870;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ86128613 = -924641923;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ63101870 = -68201790;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ41337647 = -817199941;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ69598270 = -489851263;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ103042 = -846437303;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ16103389 = -151843188;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ13555200 = -992295334;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ44833720 = -175316555;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ12935686 = -727877496;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ10279058 = -203237846;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ86743832 = -247821284;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ1211464 = 12442418;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ7629358 = -884723637;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ66058465 = -45314734;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ45027906 = -747718821;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ55252569 = -537318325;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ32569595 = -176319179;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ91931228 = 77484155;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ10576584 = 81249839;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ89848335 = -447459291;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ71663969 = -837130882;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ35029507 = -170992297;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ83297357 = -609102242;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ35058134 = -784429292;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ43849622 = -438034148;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ84882229 = -252097478;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ53968976 = -538427536;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ21603941 = -801343233;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ59661479 = -767968484;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ33068226 = -850196852;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ4066785 = -742326155;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ65838372 = -886959589;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ3171655 = 82878530;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ99742216 = -148082854;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ48780117 = -646808216;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ86872307 = -916650456;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ96770471 = -962003204;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ58178555 = -222365714;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ44546167 = -194152203;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ25808611 = -950864587;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ34090563 = -391910373;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ16705658 = -657592503;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ52310283 = 77307691;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ95533063 = 99098641;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ39258290 = -677687601;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ4890544 = -834696686;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ92596069 = -119875813;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ33896535 = -497956291;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ4532757 = -929702274;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ72442342 = -762265490;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ51496205 = -260148475;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ48085485 = -630616574;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ59739195 = -184378792;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ80496182 = -570012013;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ27391235 = -960987989;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ51099107 = -653649626;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ79804513 = -459099548;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ6279513 = 67229350;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ25748648 = 48182884;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ15220813 = -494339825;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ62134412 = -613415653;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ91951258 = -90952101;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ85172241 = -407348072;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ79867460 = -877680645;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ6212274 = -460911692;    float AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ20905460 = 78437900;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ275476 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ89109478;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ89109478 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ61060256;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ61060256 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ49951629;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ49951629 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ85107656;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ85107656 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ53923977;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ53923977 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ71164603;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ71164603 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ87356390;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ87356390 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ17134026;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ17134026 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ44083499;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ44083499 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ36154151;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ36154151 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ73516171;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ73516171 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ98797419;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ98797419 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ34908731;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ34908731 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ49974177;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ49974177 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ38108118;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ38108118 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ27776825;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ27776825 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ44649209;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ44649209 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ41911999;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ41911999 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ47645762;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ47645762 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ61539377;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ61539377 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ65245969;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ65245969 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ5812122;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ5812122 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ26002123;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ26002123 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ6102007;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ6102007 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ225427;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ225427 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ99955000;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ99955000 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ49560663;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ49560663 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ27694912;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ27694912 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ84065800;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ84065800 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ40016714;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ40016714 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ70315779;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ70315779 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ70344517;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ70344517 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ99055203;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ99055203 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ86128613;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ86128613 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ63101870;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ63101870 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ41337647;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ41337647 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ69598270;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ69598270 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ103042;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ103042 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ16103389;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ16103389 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ13555200;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ13555200 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ44833720;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ44833720 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ12935686;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ12935686 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ10279058;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ10279058 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ86743832;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ86743832 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ1211464;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ1211464 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ7629358;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ7629358 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ66058465;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ66058465 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ45027906;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ45027906 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ55252569;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ55252569 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ32569595;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ32569595 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ91931228;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ91931228 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ10576584;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ10576584 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ89848335;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ89848335 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ71663969;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ71663969 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ35029507;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ35029507 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ83297357;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ83297357 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ35058134;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ35058134 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ43849622;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ43849622 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ84882229;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ84882229 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ53968976;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ53968976 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ21603941;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ21603941 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ59661479;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ59661479 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ33068226;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ33068226 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ4066785;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ4066785 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ65838372;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ65838372 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ3171655;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ3171655 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ99742216;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ99742216 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ48780117;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ48780117 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ86872307;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ86872307 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ96770471;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ96770471 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ58178555;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ58178555 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ44546167;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ44546167 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ25808611;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ25808611 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ34090563;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ34090563 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ16705658;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ16705658 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ52310283;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ52310283 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ95533063;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ95533063 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ39258290;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ39258290 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ4890544;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ4890544 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ92596069;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ92596069 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ33896535;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ33896535 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ4532757;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ4532757 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ72442342;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ72442342 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ51496205;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ51496205 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ48085485;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ48085485 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ59739195;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ59739195 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ80496182;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ80496182 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ27391235;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ27391235 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ51099107;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ51099107 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ79804513;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ79804513 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ6279513;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ6279513 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ25748648;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ25748648 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ15220813;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ15220813 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ62134412;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ62134412 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ91951258;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ91951258 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ85172241;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ85172241 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ79867460;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ79867460 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ6212274;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ6212274 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ20905460;     AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ20905460 = AkmVDCiNCKhBfhXwPxSkBoDpIuwDjmcrZUnVgrUjrCdRQIylKeOFqCeMhxNJGedQipcLtBPXcjtzKmYAvKsEXWAWqbojeLcMZ275476;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void bpvSgMVXgDXrThVFITXjfdnBrgfWgVItIWKspiadGXmaEYkjptUnuQPxBubAgzqKvdiwWcQattdHrMwSvvgnBVzsGNqvH51746885() {     float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb29058913 = -237224339;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb97556265 = -375653945;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb98049328 = -175207404;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb26388800 = -368526669;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb25617412 = -959484501;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb73059412 = -631052188;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb89650553 = -721319435;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb41742781 = -358319412;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb70629479 = -394916555;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb90636679 = -221840287;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb27279399 = -714319913;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb88248534 = -671265293;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb84950795 = 44018866;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb75413412 = 7210032;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb25776677 = -134466448;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb9294292 = -169918557;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb77549706 = 88423255;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb80904601 = -621401536;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb23559810 = -433553364;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb91452412 = -80450004;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb62674800 = -764821311;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb4436965 = 14730650;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb10046395 = -116242823;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb2930421 = -179213986;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb80368038 = -435687427;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb70900705 = -889606310;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb17641398 = -17310801;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb71446813 = -248291106;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb36556776 = -427914397;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb81553602 = -433990911;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb39819178 = -129110329;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb87154452 = -98649265;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb31679951 = -383770083;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb46840659 = -586281963;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb12709226 = -63757323;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb30992702 = -787995846;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb47188705 = -945275879;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb67117542 = -319655441;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb33642067 = -262067915;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb13485022 = -955559797;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb79100390 = -539238047;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb44344508 = -335484872;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb5785933 = -897559675;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb52421497 = -42705395;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb2722318 = -316720041;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb25911826 = -850423562;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb94868868 = -371251105;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb37685333 = -675145436;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb52190641 = -997354576;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb65788814 = -91344372;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb43972874 = -318560106;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb72597130 = -947854922;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb68409387 = -282636028;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb67359608 = -801906045;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb89072084 = -546607602;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb24621949 = -151954990;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb87509870 = -159411123;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb95118908 = -995993418;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb46020761 = -932839242;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb54716706 = 30121808;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb55418015 = -513741388;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb18203741 = -373028330;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb5186005 = -930405016;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb89075877 = -960925645;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb50817501 = 7270041;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb40124946 = -515670648;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb56568584 = -187495211;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb38110136 = -369699172;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb62704187 = -929032646;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb94783974 = -346470603;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb62105586 = -224642678;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb10432164 = -591921304;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb47262534 = -259333621;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb10074789 = -477993567;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb12352022 = -541211957;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb18330293 = -329336439;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb98651031 = -87709675;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb57624897 = 26462571;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb208103 = -862493946;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb54456213 = -585263866;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb76031837 = -418355205;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb79956065 = -342165365;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb19256172 = -250936531;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb70767962 = -236570025;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb37580729 = -15430805;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb67222047 = -181255407;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb18745066 = -816013238;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb64320343 = -581864038;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb57768574 = 60325638;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb88087277 = -911802334;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb43482831 = -528584723;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb52069796 = -949282462;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb21096782 = -386816199;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb78925361 = -192189724;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb58067006 = -341818410;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb60896650 = -66209718;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb39158504 = -405079856;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb16710056 = -936634031;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb1603996 = 50024563;    float awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb62597372 = -237224339;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb29058913 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb97556265;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb97556265 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb98049328;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb98049328 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb26388800;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb26388800 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb25617412;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb25617412 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb73059412;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb73059412 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb89650553;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb89650553 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb41742781;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb41742781 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb70629479;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb70629479 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb90636679;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb90636679 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb27279399;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb27279399 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb88248534;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb88248534 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb84950795;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb84950795 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb75413412;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb75413412 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb25776677;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb25776677 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb9294292;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb9294292 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb77549706;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb77549706 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb80904601;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb80904601 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb23559810;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb23559810 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb91452412;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb91452412 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb62674800;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb62674800 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb4436965;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb4436965 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb10046395;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb10046395 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb2930421;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb2930421 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb80368038;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb80368038 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb70900705;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb70900705 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb17641398;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb17641398 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb71446813;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb71446813 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb36556776;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb36556776 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb81553602;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb81553602 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb39819178;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb39819178 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb87154452;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb87154452 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb31679951;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb31679951 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb46840659;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb46840659 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb12709226;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb12709226 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb30992702;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb30992702 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb47188705;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb47188705 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb67117542;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb67117542 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb33642067;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb33642067 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb13485022;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb13485022 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb79100390;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb79100390 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb44344508;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb44344508 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb5785933;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb5785933 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb52421497;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb52421497 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb2722318;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb2722318 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb25911826;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb25911826 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb94868868;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb94868868 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb37685333;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb37685333 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb52190641;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb52190641 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb65788814;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb65788814 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb43972874;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb43972874 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb72597130;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb72597130 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb68409387;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb68409387 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb67359608;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb67359608 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb89072084;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb89072084 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb24621949;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb24621949 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb87509870;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb87509870 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb95118908;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb95118908 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb46020761;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb46020761 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb54716706;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb54716706 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb55418015;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb55418015 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb18203741;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb18203741 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb5186005;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb5186005 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb89075877;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb89075877 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb50817501;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb50817501 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb40124946;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb40124946 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb56568584;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb56568584 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb38110136;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb38110136 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb62704187;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb62704187 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb94783974;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb94783974 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb62105586;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb62105586 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb10432164;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb10432164 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb47262534;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb47262534 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb10074789;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb10074789 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb12352022;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb12352022 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb18330293;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb18330293 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb98651031;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb98651031 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb57624897;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb57624897 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb208103;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb208103 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb54456213;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb54456213 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb76031837;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb76031837 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb79956065;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb79956065 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb19256172;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb19256172 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb70767962;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb70767962 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb37580729;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb37580729 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb67222047;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb67222047 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb18745066;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb18745066 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb64320343;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb64320343 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb57768574;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb57768574 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb88087277;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb88087277 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb43482831;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb43482831 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb52069796;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb52069796 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb21096782;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb21096782 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb78925361;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb78925361 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb58067006;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb58067006 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb60896650;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb60896650 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb39158504;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb39158504 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb16710056;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb16710056 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb1603996;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb1603996 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb62597372;     awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb62597372 = awGhbfaqnNNcutAnIGpxJElTDJZeqqJmVSSxWaPgVbHhTLHbyFyGAxyWKneSGwAxEaGbbDQhqJchwEArCxgkrClYUwmjwxaZb29058913;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void neMgwUBTvUBelhEBauXRAujKyRDhLHBSlCYdToRXqGsxtpZsTfQLTQttuCifQUIoSitiJaildAbuBlQIrLNgqqzUIfiRG70290424() {     int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq57842349 = -552886577;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq6003052 = -938066444;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq35038401 = -640476632;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq2825971 = -376764924;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq66127166 = -462272074;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq92194848 = 19594183;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq8136504 = -963874543;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq96129170 = -649086526;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq24124934 = -403168605;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq37189859 = -48222001;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq18404647 = -238551286;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq2980898 = -207937813;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq71104170 = 34239455;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq15918094 = -514129799;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq1579176 = -284080651;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq80480464 = -660633968;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq27322587 = -110936513;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq17159994 = -102213567;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq5207622 = -764398954;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq35259062 = -876694301;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq63810224 = -596733564;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq43627959 = -319968895;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq14280667 = -28346441;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq79858718 = -432919089;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq54634070 = 50879413;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq41575984 = -674613169;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq35327796 = -491350577;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq93332963 = -919161118;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq45418640 = -556244979;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq79041404 = -331514168;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq39621642 = -705088238;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq3993127 = 5830421;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq93015384 = -750068862;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq94626114 = -374445056;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq39289837 = -302872723;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq98883533 = -407789901;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq53039763 = 26648184;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq64636815 = -149459619;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq67181093 = -777698528;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq10866655 = -659276406;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq44645582 = -86180761;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq43855297 = -495653189;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq98636179 = 32758146;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq94563936 = -982172944;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq18700804 = -385618798;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq50612188 = -613289542;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq82108379 = -957778573;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq9312200 = -204976139;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq59353376 = -146990331;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq76325058 = -745370420;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq55376152 = -460801034;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq53263033 = -873193999;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq26242192 = -646521896;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq44870880 = -56352799;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq6480200 = -256084321;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq14214390 = -132917682;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq91722384 = -809720003;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq55179683 = -107557544;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq48191900 = -327644337;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq24551183 = -787658906;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq56867053 = -489055240;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq14803541 = 55286574;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq50710530 = 7158452;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq45083530 = 28345562;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq97568217 = -343133763;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq14411521 = -144381708;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq9965514 = -457868952;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq76478056 = -591315490;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq76628257 = -111257076;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq2695642 = -876290750;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq27440702 = -587282153;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq62685771 = -961476894;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq49978900 = -324515040;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq94340966 = -5122548;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq90613480 = -690513541;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq19954927 = -1080376;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq44991779 = -252727042;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq19716731 = -46173498;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq61157914 = 52699709;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq4021882 = -335831046;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq59467605 = -716834596;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq26015596 = -186374439;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq33979587 = -672170788;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq69093582 = -810874559;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq23665253 = -870713134;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq86358608 = -831894240;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq77750935 = -347647683;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq48144504 = -593716063;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq88145914 = -18360735;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq25075447 = -69955042;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq7161150 = -598069898;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq97860080 = -865794273;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq16444915 = -821815283;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq42629911 = -990039622;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq53999601 = -70221167;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq29842041 = -41467336;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq93144766 = -402811641;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq53552650 = -995587416;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq96995718 = -539039181;    int lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq4289284 = -552886577;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq57842349 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq6003052;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq6003052 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq35038401;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq35038401 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq2825971;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq2825971 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq66127166;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq66127166 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq92194848;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq92194848 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq8136504;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq8136504 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq96129170;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq96129170 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq24124934;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq24124934 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq37189859;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq37189859 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq18404647;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq18404647 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq2980898;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq2980898 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq71104170;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq71104170 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq15918094;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq15918094 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq1579176;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq1579176 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq80480464;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq80480464 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq27322587;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq27322587 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq17159994;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq17159994 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq5207622;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq5207622 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq35259062;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq35259062 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq63810224;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq63810224 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq43627959;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq43627959 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq14280667;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq14280667 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq79858718;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq79858718 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq54634070;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq54634070 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq41575984;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq41575984 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq35327796;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq35327796 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq93332963;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq93332963 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq45418640;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq45418640 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq79041404;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq79041404 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq39621642;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq39621642 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq3993127;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq3993127 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq93015384;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq93015384 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq94626114;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq94626114 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq39289837;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq39289837 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq98883533;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq98883533 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq53039763;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq53039763 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq64636815;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq64636815 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq67181093;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq67181093 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq10866655;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq10866655 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq44645582;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq44645582 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq43855297;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq43855297 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq98636179;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq98636179 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq94563936;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq94563936 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq18700804;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq18700804 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq50612188;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq50612188 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq82108379;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq82108379 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq9312200;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq9312200 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq59353376;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq59353376 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq76325058;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq76325058 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq55376152;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq55376152 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq53263033;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq53263033 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq26242192;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq26242192 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq44870880;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq44870880 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq6480200;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq6480200 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq14214390;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq14214390 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq91722384;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq91722384 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq55179683;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq55179683 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq48191900;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq48191900 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq24551183;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq24551183 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq56867053;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq56867053 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq14803541;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq14803541 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq50710530;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq50710530 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq45083530;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq45083530 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq97568217;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq97568217 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq14411521;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq14411521 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq9965514;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq9965514 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq76478056;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq76478056 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq76628257;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq76628257 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq2695642;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq2695642 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq27440702;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq27440702 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq62685771;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq62685771 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq49978900;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq49978900 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq94340966;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq94340966 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq90613480;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq90613480 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq19954927;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq19954927 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq44991779;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq44991779 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq19716731;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq19716731 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq61157914;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq61157914 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq4021882;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq4021882 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq59467605;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq59467605 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq26015596;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq26015596 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq33979587;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq33979587 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq69093582;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq69093582 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq23665253;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq23665253 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq86358608;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq86358608 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq77750935;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq77750935 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq48144504;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq48144504 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq88145914;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq88145914 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq25075447;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq25075447 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq7161150;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq7161150 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq97860080;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq97860080 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq16444915;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq16444915 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq42629911;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq42629911 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq53999601;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq53999601 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq29842041;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq29842041 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq93144766;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq93144766 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq53552650;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq53552650 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq96995718;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq96995718 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq4289284;     lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq4289284 = lGVxEeLvqitwfiTpyrIlZnFHcycjYaerRfrtFxxHFNBcUjKIffJnZfgyBOUGCbWPEBfzqZjBtBfMlcntnUDtQqSlztsqcrEdq57842349;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void YqraZnRfiSklUEdoIYGxEObcnCnbfiQKAdRSFtJEaWmiBqDQCeAdOZiKOOnXzqzzBalqnLjPPCWibtbwzWYBimvbvGEqF88833962() {     int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw86625785 = -868548815;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw14449838 = -400478943;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw72027474 = -5745860;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw79263141 = -385003179;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw6636922 = 34940353;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw11330285 = -429759446;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw26622454 = -106429651;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw50515560 = -939853640;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw77620388 = -411420656;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw83743039 = -974603714;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw9529896 = -862782658;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw17713261 = -844610333;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw57257545 = 24460044;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw56422775 = 64530371;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw77381674 = -433694853;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw51666638 = -51349380;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw77095467 = -310296282;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw53415386 = -683025598;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw86855432 = 4755457;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw79065712 = -572938599;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw64945647 = -428645818;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw82818954 = -654668441;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw18514940 = 59549940;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw56787016 = -686624191;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw28900102 = -562553748;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw12251263 = -459620028;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw53014193 = -965390354;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw15219113 = -490031130;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw54280504 = -684575560;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw76529206 = -229037425;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw39424105 = -181066148;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw20831801 = -989689892;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw54350817 = -16367641;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw42411569 = -162608149;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw65870448 = -541988124;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw66774365 = -27583956;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw58890822 = -101427754;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw62156088 = 20736202;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw720120 = -193329140;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw8248288 = -362993015;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw10190773 = -733123474;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw43366086 = -655821506;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw91486426 = -136924033;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw36706375 = -821640492;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw34679289 = -454517555;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw75312550 = -376155522;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw69347890 = -444306041;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw80939067 = -834806841;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw66516111 = -396626086;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw86861302 = -299396467;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw66779430 = -603041962;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw33928935 = -798533076;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw84074996 = 89592237;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw22382153 = -410799553;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw23888315 = 34438959;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw3806832 = -113880375;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw95934897 = -360028884;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw15240458 = -319121670;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw50363039 = -822449432;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw94385659 = -505439620;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw58316092 = -464369092;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw11403341 = -616398522;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw96235056 = -155278080;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw1091182 = -82383231;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw44318934 = -693537567;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw88698094 = -873092767;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw63362443 = -728242693;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw14845976 = -812931808;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw90552327 = -393481506;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw10607309 = -306110897;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw92775816 = -949921627;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw14939380 = -231032484;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw52695267 = -389696459;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw78607145 = -632251528;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw68874939 = -839815125;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw21579562 = -772824313;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw91332527 = -417744408;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw81808564 = -118809568;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw22107727 = -132106636;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw53587551 = -86398227;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw42903373 = 84686013;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw72075126 = -30583513;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw48703002 = 6594956;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw67419201 = -285179094;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw9749777 = -625995464;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw5495171 = -382533073;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw36756805 = -979282129;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw31968665 = -605568088;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw18523254 = -97047109;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw62063617 = -328107749;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw70839468 = -667555073;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw43650364 = -782306085;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw11793049 = -156814366;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw6334461 = -687889521;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw49932196 = -898623924;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw98787432 = -16724953;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw47131030 = -400543426;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw90395245 = 45459198;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw92387440 = -28102926;    int lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw45981195 = -868548815;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw86625785 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw14449838;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw14449838 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw72027474;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw72027474 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw79263141;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw79263141 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw6636922;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw6636922 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw11330285;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw11330285 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw26622454;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw26622454 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw50515560;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw50515560 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw77620388;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw77620388 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw83743039;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw83743039 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw9529896;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw9529896 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw17713261;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw17713261 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw57257545;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw57257545 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw56422775;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw56422775 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw77381674;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw77381674 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw51666638;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw51666638 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw77095467;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw77095467 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw53415386;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw53415386 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw86855432;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw86855432 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw79065712;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw79065712 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw64945647;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw64945647 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw82818954;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw82818954 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw18514940;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw18514940 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw56787016;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw56787016 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw28900102;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw28900102 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw12251263;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw12251263 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw53014193;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw53014193 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw15219113;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw15219113 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw54280504;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw54280504 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw76529206;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw76529206 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw39424105;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw39424105 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw20831801;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw20831801 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw54350817;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw54350817 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw42411569;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw42411569 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw65870448;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw65870448 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw66774365;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw66774365 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw58890822;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw58890822 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw62156088;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw62156088 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw720120;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw720120 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw8248288;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw8248288 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw10190773;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw10190773 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw43366086;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw43366086 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw91486426;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw91486426 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw36706375;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw36706375 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw34679289;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw34679289 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw75312550;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw75312550 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw69347890;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw69347890 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw80939067;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw80939067 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw66516111;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw66516111 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw86861302;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw86861302 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw66779430;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw66779430 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw33928935;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw33928935 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw84074996;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw84074996 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw22382153;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw22382153 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw23888315;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw23888315 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw3806832;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw3806832 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw95934897;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw95934897 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw15240458;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw15240458 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw50363039;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw50363039 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw94385659;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw94385659 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw58316092;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw58316092 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw11403341;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw11403341 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw96235056;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw96235056 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw1091182;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw1091182 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw44318934;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw44318934 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw88698094;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw88698094 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw63362443;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw63362443 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw14845976;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw14845976 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw90552327;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw90552327 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw10607309;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw10607309 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw92775816;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw92775816 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw14939380;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw14939380 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw52695267;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw52695267 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw78607145;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw78607145 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw68874939;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw68874939 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw21579562;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw21579562 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw91332527;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw91332527 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw81808564;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw81808564 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw22107727;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw22107727 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw53587551;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw53587551 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw42903373;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw42903373 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw72075126;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw72075126 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw48703002;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw48703002 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw67419201;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw67419201 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw9749777;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw9749777 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw5495171;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw5495171 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw36756805;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw36756805 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw31968665;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw31968665 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw18523254;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw18523254 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw62063617;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw62063617 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw70839468;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw70839468 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw43650364;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw43650364 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw11793049;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw11793049 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw6334461;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw6334461 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw49932196;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw49932196 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw98787432;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw98787432 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw47131030;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw47131030 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw90395245;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw90395245 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw92387440;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw92387440 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw45981195;     lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw45981195 = lEkWngbFMdqukOMCAiUnqYcRyROeJvgHsJdxlLoMArmtiGxYiYvYjDPMcTYVRwvRxjCbkPRouInzmijrTqvlzIYpuQUSnSvDw86625785;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void BFinlfhJbZXXyIPTQMbamqUNUeiFGtSyGOZYcXvSSGLVy57653488() {     int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu58612556 = -203404164;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu37618185 = -796885933;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu34332416 = -740314950;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu82931348 = -761362141;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu18169720 = -678949588;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu67557561 = -259666301;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu28269235 = -161168568;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu97312466 = -149803260;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu96097825 = -840882815;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu82805869 = -273460065;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu38749383 = -357527189;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu27957229 = -112040468;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu86812572 = -773264093;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu10261523 = -98457226;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu71717127 = 81267446;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu31871344 = -243252322;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu73924297 = -885576931;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu68346844 = -740976314;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu45997929 = -549183996;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu9476660 = -3839865;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu78648713 = -958446294;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu80472821 = 60409426;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu54378509 = -230043771;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu4933616 = -883819712;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu56767069 = -73650237;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu63619751 = -646215168;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu71855933 = -593972717;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu12953511 = -317086190;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu85134330 = -430578973;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu57376705 = -284437288;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu53221617 = -346151160;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu6646656 = 74859836;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu87813808 = -117383007;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu97910663 = -507837964;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu88919706 = -210331542;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu85371594 = -584331868;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu64553943 = -963714816;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu81588502 = -84587964;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu30406869 = -346114602;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu44967005 = -665217382;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu79870614 = -309564800;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu65693023 = -438620439;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu60739705 = -677948985;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu51862426 = -418575168;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu96147170 = -423893125;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu12561224 = -764508436;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu7366825 = -840029333;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu44949548 = -191876318;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu8039884 = -146841535;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu96503801 = -114828542;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu59638858 = -404319281;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu14197378 = -670359934;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu78321537 = -108611570;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu44071244 = -651100221;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu79004775 = -337064845;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu78139735 = -163813591;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu83239676 = -466842163;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu29398801 = -856495238;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu26164279 = -587711904;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu54549968 = 67265579;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu95701628 = -665693584;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu15315724 = -844082379;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu12178137 = -719224287;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu38721120 = -456445527;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu29584252 = -927308905;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu32102728 = -332387026;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu40143421 = -994657462;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu88901908 = -165426130;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu21341816 = -888125685;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu86345532 = -334400686;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu67317400 = -279537507;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu92335794 = -700988968;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu37939975 = -294861713;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu1030925 = -883966614;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu29606046 = -694275065;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu12955691 = -419825855;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu19733116 = -261641590;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu2516083 = -811468603;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu8786446 = -359926588;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu44205845 = -309141802;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu56252927 = -806185836;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu26906385 = -302096400;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu4913627 = -70244655;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu88630528 = -215750432;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu97737847 = -880118007;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu39024239 = -675791227;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu28325119 = -816528595;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu43742564 = -466282787;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu18905888 = -70773120;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu10779972 = 53482048;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu2131918 = -17489706;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu35155143 = -7219579;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu55424223 = -496876060;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu75856901 = -313380182;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu49265376 = -999523799;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu64554890 = -465482422;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu53514887 = -719396153;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu22018585 = -121503458;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu22278174 = -491266263;    int oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu64044442 = -203404164;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu58612556 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu37618185;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu37618185 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu34332416;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu34332416 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu82931348;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu82931348 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu18169720;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu18169720 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu67557561;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu67557561 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu28269235;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu28269235 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu97312466;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu97312466 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu96097825;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu96097825 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu82805869;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu82805869 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu38749383;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu38749383 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu27957229;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu27957229 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu86812572;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu86812572 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu10261523;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu10261523 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu71717127;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu71717127 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu31871344;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu31871344 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu73924297;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu73924297 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu68346844;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu68346844 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu45997929;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu45997929 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu9476660;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu9476660 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu78648713;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu78648713 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu80472821;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu80472821 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu54378509;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu54378509 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu4933616;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu4933616 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu56767069;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu56767069 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu63619751;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu63619751 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu71855933;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu71855933 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu12953511;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu12953511 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu85134330;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu85134330 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu57376705;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu57376705 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu53221617;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu53221617 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu6646656;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu6646656 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu87813808;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu87813808 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu97910663;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu97910663 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu88919706;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu88919706 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu85371594;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu85371594 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu64553943;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu64553943 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu81588502;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu81588502 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu30406869;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu30406869 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu44967005;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu44967005 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu79870614;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu79870614 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu65693023;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu65693023 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu60739705;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu60739705 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu51862426;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu51862426 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu96147170;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu96147170 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu12561224;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu12561224 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu7366825;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu7366825 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu44949548;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu44949548 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu8039884;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu8039884 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu96503801;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu96503801 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu59638858;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu59638858 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu14197378;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu14197378 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu78321537;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu78321537 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu44071244;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu44071244 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu79004775;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu79004775 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu78139735;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu78139735 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu83239676;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu83239676 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu29398801;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu29398801 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu26164279;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu26164279 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu54549968;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu54549968 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu95701628;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu95701628 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu15315724;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu15315724 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu12178137;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu12178137 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu38721120;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu38721120 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu29584252;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu29584252 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu32102728;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu32102728 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu40143421;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu40143421 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu88901908;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu88901908 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu21341816;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu21341816 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu86345532;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu86345532 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu67317400;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu67317400 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu92335794;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu92335794 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu37939975;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu37939975 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu1030925;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu1030925 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu29606046;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu29606046 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu12955691;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu12955691 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu19733116;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu19733116 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu2516083;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu2516083 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu8786446;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu8786446 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu44205845;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu44205845 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu56252927;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu56252927 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu26906385;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu26906385 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu4913627;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu4913627 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu88630528;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu88630528 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu97737847;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu97737847 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu39024239;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu39024239 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu28325119;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu28325119 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu43742564;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu43742564 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu18905888;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu18905888 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu10779972;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu10779972 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu2131918;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu2131918 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu35155143;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu35155143 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu55424223;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu55424223 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu75856901;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu75856901 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu49265376;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu49265376 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu64554890;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu64554890 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu53514887;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu53514887 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu22018585;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu22018585 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu22278174;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu22278174 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu64044442;     oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu64044442 = oZoQMzXokpKkUSGxcChHYRkjqrtQIIffzPgFtpuuu58612556;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void EjvckMqkzojnIwlVyaeSmOzctUEOSPjEJfKVJaNePpCKuKAaiCLKMEhpjTjQIbSkFeedlPLXNzDxLphiTdgP26916135() {     float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR35085939 = -595409151;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR15369767 = -680347162;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR91448938 = -601481402;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR78281365 = -332519143;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR57175772 = -462813410;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR23152991 = -250076701;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR62664191 = -928647598;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR47957224 = -510640999;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR21765958 = -180354638;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR25040837 = -790883168;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR98437249 = -518377500;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR14049664 = -269385345;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR99545286 = -575667424;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR47116530 = -733214127;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR19785051 = -834052530;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR56891880 = 67823747;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR95131499 = -554808763;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR59969000 = -374797510;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR12069889 = -117122334;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR44257739 = -46463292;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR9812098 = -13841334;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR85275521 = -981781788;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR18758943 = -52293468;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR65464660 = -933419655;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR90671696 = -20807588;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR52723591 = -931212445;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR31993252 = -826020897;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR19484099 = -494069777;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR79268143 = -250804924;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR85674126 = -789125137;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR72798750 = -394298051;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR48891520 = -308436404;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR23515361 = 32523388;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR65247092 = -352119267;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR45437095 = -707670366;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR63689191 = -336626318;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR38311372 = -363585222;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR33462443 = -537691082;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR59213762 = -306809192;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR67560492 = -35790826;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR84133694 = -350226165;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR11718683 = -830514645;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR79248967 = -841986245;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR88567633 = -679543225;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR78056070 = -429729053;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR27315289 = -992752616;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR63357543 = -188734548;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR37908399 = -762405357;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR16775938 = -563570077;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR67415652 = -127415464;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR29843902 = -994701612;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR7241820 = -51876871;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR88996578 = -308149171;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR34637259 = -453955690;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR17104037 = -572544914;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR49810418 = -613627363;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR96610823 = -528053695;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR25984279 = -668061747;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR87609668 = -211711555;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR4452182 = -531600966;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR91159738 = -424055804;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR43180092 = -334577821;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR68689081 = -159836076;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR36091831 = -391229502;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR52242086 = -296585117;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR49545729 = -109941097;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR90534302 = -201908733;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR34298195 = -123548157;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR1679436 = 74456238;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR56095859 = -397426213;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR18580508 = -568591031;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR61669056 = 82882318;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR755238 = 32011682;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR44509397 = 18668491;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR60124044 = -696237128;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR98093415 = -183326690;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR6026554 = -39795544;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR30191310 = -372750244;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR87408589 = -403690603;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR63356408 = -28054973;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR89366047 = -642477898;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR94084852 = 36384460;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR2708162 = -930499701;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR11852491 = -23389460;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR55830225 = -794423525;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR65556931 = -242421180;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR59894941 = 99712766;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR88878101 = -513520922;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR48143055 = -779574354;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR95626677 = 5956996;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR67078367 = -808572623;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR12327094 = -695523476;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR45852774 = -225979527;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR54761581 = -775208227;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR76400753 = -611735022;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR40953602 = 84351655;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR43029602 = -570678569;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR43157136 = -350756744;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR36325547 = -282958108;    float DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR28510341 = -595409151;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR35085939 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR15369767;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR15369767 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR91448938;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR91448938 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR78281365;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR78281365 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR57175772;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR57175772 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR23152991;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR23152991 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR62664191;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR62664191 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR47957224;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR47957224 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR21765958;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR21765958 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR25040837;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR25040837 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR98437249;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR98437249 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR14049664;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR14049664 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR99545286;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR99545286 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR47116530;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR47116530 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR19785051;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR19785051 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR56891880;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR56891880 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR95131499;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR95131499 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR59969000;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR59969000 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR12069889;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR12069889 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR44257739;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR44257739 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR9812098;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR9812098 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR85275521;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR85275521 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR18758943;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR18758943 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR65464660;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR65464660 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR90671696;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR90671696 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR52723591;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR52723591 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR31993252;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR31993252 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR19484099;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR19484099 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR79268143;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR79268143 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR85674126;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR85674126 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR72798750;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR72798750 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR48891520;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR48891520 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR23515361;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR23515361 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR65247092;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR65247092 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR45437095;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR45437095 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR63689191;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR63689191 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR38311372;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR38311372 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR33462443;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR33462443 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR59213762;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR59213762 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR67560492;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR67560492 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR84133694;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR84133694 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR11718683;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR11718683 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR79248967;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR79248967 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR88567633;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR88567633 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR78056070;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR78056070 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR27315289;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR27315289 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR63357543;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR63357543 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR37908399;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR37908399 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR16775938;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR16775938 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR67415652;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR67415652 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR29843902;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR29843902 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR7241820;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR7241820 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR88996578;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR88996578 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR34637259;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR34637259 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR17104037;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR17104037 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR49810418;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR49810418 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR96610823;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR96610823 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR25984279;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR25984279 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR87609668;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR87609668 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR4452182;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR4452182 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR91159738;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR91159738 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR43180092;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR43180092 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR68689081;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR68689081 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR36091831;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR36091831 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR52242086;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR52242086 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR49545729;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR49545729 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR90534302;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR90534302 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR34298195;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR34298195 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR1679436;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR1679436 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR56095859;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR56095859 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR18580508;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR18580508 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR61669056;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR61669056 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR755238;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR755238 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR44509397;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR44509397 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR60124044;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR60124044 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR98093415;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR98093415 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR6026554;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR6026554 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR30191310;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR30191310 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR87408589;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR87408589 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR63356408;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR63356408 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR89366047;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR89366047 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR94084852;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR94084852 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR2708162;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR2708162 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR11852491;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR11852491 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR55830225;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR55830225 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR65556931;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR65556931 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR59894941;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR59894941 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR88878101;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR88878101 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR48143055;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR48143055 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR95626677;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR95626677 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR67078367;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR67078367 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR12327094;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR12327094 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR45852774;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR45852774 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR54761581;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR54761581 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR76400753;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR76400753 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR40953602;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR40953602 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR43029602;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR43029602 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR43157136;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR43157136 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR36325547;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR36325547 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR28510341;     DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR28510341 = DYsFyUmprxWnWSFPgXoSrnxMBkdtmAwevCggmZfRcVgPdPatkRCwoNYPbgDNqtDqAssTlZdHsXqVGhilJkJUxzpheQEFVRajR35085939;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void LVFUYbEOhWLLobIEsXjNcvzDQSMapEkRGRReLZoTKuOajOgxSfs30964615() {     long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ82772649 = -58013971;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ53960720 = -22693462;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ62232995 = -525942933;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ89734142 = -355708302;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ60092120 = -978067328;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ77015699 = -740849878;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ62846865 = -755839753;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ78822617 = -432800298;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ35308718 = 81602553;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ19042381 = -750327992;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ77160170 = -34732476;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ92555574 = -757796892;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ42051083 = -929120581;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ20388967 = 80792275;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ14636530 = -481114748;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ98008515 = -946782597;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ94492201 = 65511889;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ76836030 = -176342490;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ56708172 = -763206215;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ71269050 = -698854648;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ79674771 = -192557312;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ40035359 = -294269398;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ97344303 = -741918468;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ70892461 = -669774758;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ14531638 = -117878718;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ29439191 = -285305823;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ59554964 = -734429157;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ21830300 = -304666854;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ70879315 = -815735435;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ4528682 = -134005416;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ9279760 = -60013648;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ66659639 = -992123208;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ40607692 = -102243543;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ84939484 = -896578344;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ16552891 = 4449251;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ95530051 = 40990434;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ62188425 = -235206380;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ22775951 = -58621364;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ72138428 = -658213866;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ78708791 = -57363503;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ16779418 = -337916766;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ51082384 = -59136554;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ85049662 = -15906451;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ62746350 = -879525954;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ85995511 = -53295924;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ37582976 = -814153134;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ45957648 = -291552606;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ46932174 = -457484371;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ77678450 = 37455229;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ37813970 = -379488787;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ95275352 = -132120520;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ26893989 = -249127602;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ29562990 = -639827912;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ52817137 = -718324332;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ51289843 = -976997898;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ42737290 = -763744574;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ56616417 = -280774994;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ91340533 = -856168176;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ75202505 = -137829585;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ30652930 = -592761506;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ17460736 = 93579279;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ41016566 = -351172900;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ7943302 = -617064864;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ30780037 = -784392032;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ9762622 = -590314344;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ10500531 = -42609268;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ51947882 = -555553350;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ57111599 = 67457762;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ3836077 = -923656976;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ19106478 = -422105182;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ35820090 = -611576217;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ71716250 = -875866747;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ4697603 = -518128624;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ77999380 = -605842712;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ54489632 = -260937883;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ28592388 = -33420759;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ54985697 = -178362948;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ34597953 = -862392514;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ84896949 = -516478835;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ76948662 = -303725585;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ83481543 = -993753217;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ12622790 = -176944787;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ44151849 = -242122084;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ33065346 = -336246648;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ9253329 = 98115103;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ82385771 = -810886047;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ37096650 = -252295297;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ87790554 = -383919212;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ33649642 = -919580447;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ73815600 = -231806176;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ38913634 = -678234573;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ70847892 = -379038205;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ47573446 = -920791780;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ41485498 = 34547639;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ61248056 = -50942782;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ75762852 = -986743866;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ43139083 = -442071690;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ54269625 = -231514420;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ52983729 = -189211611;    long jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ75494981 = -58013971;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ82772649 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ53960720;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ53960720 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ62232995;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ62232995 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ89734142;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ89734142 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ60092120;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ60092120 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ77015699;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ77015699 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ62846865;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ62846865 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ78822617;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ78822617 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ35308718;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ35308718 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ19042381;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ19042381 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ77160170;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ77160170 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ92555574;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ92555574 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ42051083;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ42051083 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ20388967;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ20388967 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ14636530;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ14636530 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ98008515;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ98008515 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ94492201;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ94492201 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ76836030;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ76836030 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ56708172;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ56708172 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ71269050;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ71269050 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ79674771;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ79674771 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ40035359;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ40035359 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ97344303;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ97344303 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ70892461;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ70892461 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ14531638;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ14531638 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ29439191;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ29439191 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ59554964;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ59554964 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ21830300;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ21830300 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ70879315;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ70879315 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ4528682;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ4528682 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ9279760;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ9279760 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ66659639;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ66659639 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ40607692;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ40607692 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ84939484;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ84939484 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ16552891;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ16552891 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ95530051;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ95530051 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ62188425;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ62188425 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ22775951;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ22775951 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ72138428;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ72138428 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ78708791;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ78708791 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ16779418;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ16779418 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ51082384;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ51082384 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ85049662;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ85049662 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ62746350;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ62746350 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ85995511;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ85995511 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ37582976;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ37582976 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ45957648;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ45957648 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ46932174;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ46932174 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ77678450;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ77678450 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ37813970;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ37813970 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ95275352;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ95275352 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ26893989;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ26893989 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ29562990;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ29562990 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ52817137;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ52817137 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ51289843;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ51289843 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ42737290;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ42737290 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ56616417;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ56616417 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ91340533;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ91340533 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ75202505;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ75202505 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ30652930;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ30652930 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ17460736;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ17460736 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ41016566;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ41016566 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ7943302;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ7943302 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ30780037;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ30780037 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ9762622;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ9762622 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ10500531;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ10500531 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ51947882;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ51947882 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ57111599;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ57111599 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ3836077;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ3836077 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ19106478;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ19106478 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ35820090;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ35820090 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ71716250;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ71716250 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ4697603;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ4697603 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ77999380;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ77999380 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ54489632;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ54489632 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ28592388;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ28592388 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ54985697;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ54985697 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ34597953;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ34597953 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ84896949;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ84896949 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ76948662;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ76948662 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ83481543;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ83481543 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ12622790;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ12622790 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ44151849;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ44151849 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ33065346;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ33065346 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ9253329;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ9253329 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ82385771;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ82385771 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ37096650;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ37096650 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ87790554;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ87790554 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ33649642;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ33649642 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ73815600;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ73815600 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ38913634;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ38913634 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ70847892;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ70847892 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ47573446;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ47573446 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ41485498;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ41485498 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ61248056;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ61248056 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ75762852;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ75762852 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ43139083;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ43139083 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ54269625;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ54269625 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ52983729;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ52983729 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ75494981;     jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ75494981 = jvsBQWBDGOdZcltEWJdtSjQDigYTCmPHJLAWNQBIJWiEgVmpqJ82772649;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void uvgKmHdrUFgBddqyVMWTYEbOyGgYoUQYWhsLmeiVQOzZwgdjLFdnRSDqTSMfaFCzoIewlfLrdBCJFCRRcwMjrcGDeB86272862() {     double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC51385386 = -337517413;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC45880435 = -172899061;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC31148489 = -110327847;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC64283804 = 29476431;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC64528880 = -569074776;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC59817362 = 67972169;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC73660182 = -916812793;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC97433569 = -563206217;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC98461483 = -716045482;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC47049958 = -768138223;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC6845804 = -855762819;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC54333185 = -755102019;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC71210028 = -434525655;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC57637855 = -276486826;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC19411905 = -81635913;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC38002069 = -542017592;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC35708825 = -43598352;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC89472778 = 96595925;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC1911778 = -719939106;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC45862594 = -889337572;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC76124391 = -597739696;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC78767914 = -517539742;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC70882427 = -408110533;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC59343517 = 73371585;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC43874119 = -554552916;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC81833537 = -781295407;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC45180847 = -979406057;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC61378943 = -243599537;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC89978858 = -896267672;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC64337720 = 46389181;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC41161384 = -895823677;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC79168974 = -181975642;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC25320516 = -554772161;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC63775183 = -899572882;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC66338046 = 38510769;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC76957361 = -735816263;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC94962402 = 13347105;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC77123166 = -650784457;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC51702221 = -245519335;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC53559041 = -872671665;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC31508772 = -29075322;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC87054263 = -434608962;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC19262914 = -347275413;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC18147872 = -641967950;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC38945305 = -113310866;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC89238359 = -123345095;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC82625839 = -210742607;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC72179096 = 335123;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC36043309 = -59288422;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC82214687 = 70165371;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC55674037 = -644898783;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC45333031 = -121000845;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC88061263 = -343530247;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC40167870 = -975810158;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC10148471 = -178307081;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC72617471 = -819977671;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC74998008 = -764788529;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC71804971 = -83699432;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC20409685 = -415970654;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC82695342 = -787779369;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC14636516 = 47378225;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC12281239 = -573213257;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC7454711 = -666938545;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC34123764 = -662434663;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC5888574 = -872314546;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC27676830 = -573787177;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC29012670 = -100329858;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC7434845 = -534952773;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC91299809 = -214997595;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC42454543 = -345819651;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC43039667 = -455364697;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC58585658 = -392813896;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC37770557 = -657884741;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC48352737 = -847267441;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC14353823 = -760262250;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC89070127 = -63130735;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC59505001 = -70264330;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC52734555 = -766142584;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC20398213 = -813317550;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC54635760 = -331207821;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC99207697 = -470552801;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC73001751 = -879741181;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC25335634 = -84311115;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC7764172 = -866433044;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC8663683 = -308712037;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC95828353 = -674822833;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC91107711 = -838445396;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC85152646 = -578962004;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC53626712 = -621265802;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC93720574 = -141511560;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC1959354 = -971027735;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC23157431 = -902953464;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC56713482 = -134813804;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC69006879 = -457739966;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC38922526 = -820049891;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC19227534 = -455862066;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC79599552 = -767670418;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC85139149 = -684840750;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC12259298 = -769653404;    double rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC11268475 = -337517413;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC51385386 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC45880435;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC45880435 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC31148489;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC31148489 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC64283804;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC64283804 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC64528880;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC64528880 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC59817362;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC59817362 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC73660182;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC73660182 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC97433569;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC97433569 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC98461483;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC98461483 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC47049958;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC47049958 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC6845804;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC6845804 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC54333185;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC54333185 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC71210028;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC71210028 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC57637855;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC57637855 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC19411905;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC19411905 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC38002069;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC38002069 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC35708825;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC35708825 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC89472778;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC89472778 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC1911778;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC1911778 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC45862594;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC45862594 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC76124391;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC76124391 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC78767914;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC78767914 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC70882427;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC70882427 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC59343517;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC59343517 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC43874119;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC43874119 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC81833537;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC81833537 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC45180847;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC45180847 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC61378943;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC61378943 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC89978858;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC89978858 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC64337720;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC64337720 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC41161384;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC41161384 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC79168974;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC79168974 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC25320516;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC25320516 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC63775183;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC63775183 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC66338046;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC66338046 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC76957361;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC76957361 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC94962402;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC94962402 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC77123166;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC77123166 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC51702221;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC51702221 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC53559041;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC53559041 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC31508772;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC31508772 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC87054263;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC87054263 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC19262914;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC19262914 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC18147872;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC18147872 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC38945305;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC38945305 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC89238359;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC89238359 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC82625839;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC82625839 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC72179096;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC72179096 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC36043309;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC36043309 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC82214687;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC82214687 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC55674037;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC55674037 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC45333031;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC45333031 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC88061263;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC88061263 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC40167870;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC40167870 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC10148471;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC10148471 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC72617471;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC72617471 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC74998008;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC74998008 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC71804971;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC71804971 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC20409685;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC20409685 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC82695342;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC82695342 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC14636516;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC14636516 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC12281239;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC12281239 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC7454711;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC7454711 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC34123764;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC34123764 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC5888574;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC5888574 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC27676830;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC27676830 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC29012670;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC29012670 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC7434845;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC7434845 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC91299809;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC91299809 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC42454543;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC42454543 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC43039667;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC43039667 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC58585658;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC58585658 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC37770557;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC37770557 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC48352737;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC48352737 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC14353823;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC14353823 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC89070127;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC89070127 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC59505001;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC59505001 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC52734555;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC52734555 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC20398213;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC20398213 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC54635760;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC54635760 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC99207697;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC99207697 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC73001751;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC73001751 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC25335634;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC25335634 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC7764172;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC7764172 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC8663683;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC8663683 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC95828353;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC95828353 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC91107711;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC91107711 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC85152646;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC85152646 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC53626712;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC53626712 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC93720574;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC93720574 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC1959354;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC1959354 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC23157431;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC23157431 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC56713482;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC56713482 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC69006879;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC69006879 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC38922526;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC38922526 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC19227534;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC19227534 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC79599552;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC79599552 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC85139149;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC85139149 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC12259298;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC12259298 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC11268475;     rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC11268475 = rUSCvLLzknChygtlJQPtkRAuDdsTILBaSjnrZbFrxFRePvygpGSqokQaUXzRSsrYmbOOsC51385386;}
// Junk Finished
