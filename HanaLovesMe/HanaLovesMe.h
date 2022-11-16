// dear HanaLovesMe, v1.70 WIP
// (headers)

// See HanaLovesMe.cpp file for documentation.
// Call and read HanaLovesMe::ShowDemoWindow() in HanaLovesMe_demo.cpp for demo code.
// Newcomers, read 'Programmer guide' in HanaLovesMe.cpp for notes on how to setup Dear HanaLovesMe in your codebase.
// Get latest version at https://github.com/ocornut/HanaLovesMe

/*

Index of this file:
// Header mess
// Forward declarations and basic types
// HanaLovesMe API (Dear HanaLovesMe end-user API)
// Flags & Enumerations
// Memory allocations macros
// ImVector<>
// HanaLovesMeStyle
// HanaLovesMeIO
// Misc data structures (HanaLovesMeInputTextCallbackData, HanaLovesMeSizeCallbackData, HanaLovesMePayload)
// Obsolete functions
// Helpers (HanaLovesMeOnceUponAFrame, HanaLovesMeTextFilter, HanaLovesMeTextBuffer, HanaLovesMeStohnly, HanaLovesMeListClipper, xuicolor)
// Draw List API (ImDrawCallback, ImDrawCmd, ImDrawIdx, ImDrawVert, ImDrawChannel, ImDrawListFlags, ImDrawList, ImDrawData)
// Font API (ImFontConfig, ImFontGlyph, ImFontGlyphRangesBuilder, ImFontAtlasFlags, ImFontAtlas, ImFont)

*/

#pragma once

// Configuration file with compile-time options (edit imconfig.h or define HanaLovesMe_USER_CONFIG to your own filename)
#ifdef HanaLovesMe_USER_CONFIG
#include HanaLovesMe_USER_CONFIG
#endif
#if !defined(HanaLovesMe_DISABLE_INCLUDE_IMCONFIG_H) || defined(HanaLovesMe_INCLUDE_IMCONFIG_H)
#include "imconfig.h"
#endif

//-----------------------------------------------------------------------------
// Header mess
//-----------------------------------------------------------------------------

#include <float.h>                  // FLT_MAX
#include <stdarg.h>                 // va_list
#include <stddef.h>                 // ptrdiff_t, NULL
#include <string.h>                 // memset, memmove, memcpy, strlen, strchr, strcpy, strcmp

// Version
// (Integer encoded as XYYZZ for use in #if preprocessor conditionals. Work in progress versions typically starts at XYY99 then bounce up to XYY00, XYY01 etc. when release tagging happens)
#define HanaLovesMe_VERSION               "1.70 WIP"
#define HanaLovesMe_VERSION_NUM           16990
#define HanaLovesMe_CHECKVERSION()        HanaLovesMe::DebugCheckVersionAndDataLayout(HanaLovesMe_VERSION, sizeof(HanaLovesMeIO), sizeof(HanaLovesMeStyle), sizeof(vsize), sizeof(xuifloatcolor), sizeof(ImDrawVert))

// Define attributes of all API symbols declarations (e.g. for DLL under Windows)
// HanaLovesMe_API is used for core HanaLovesMe functions, HanaLovesMe_IMPL_API is used for the default bindings files (HanaLovesMe_impl_xxx.h)
#ifndef HanaLovesMe_API
#define HanaLovesMe_API
#endif
#ifndef HanaLovesMe_IMPL_API
#define HanaLovesMe_IMPL_API              HanaLovesMe_API
#endif

// Helper Macros
#ifndef IM_ASSERT
#include <assert.h>
#define IM_ASSERT(_EXPR)            assert(_EXPR)                               // You can override the default assert handler by editing imconfig.h
#endif
#if defined(__clang__) || defined(__GNUC__)
#define IM_FMTARGS(FMT)             __attribute__((format(printf, FMT, FMT+1))) // Apply printf-style warnings to user functions.
#define IM_FMTLIST(FMT)             __attribute__((format(printf, FMT, 0)))
#else
#define IM_FMTARGS(FMT)
#define IM_FMTLIST(FMT)
#endif
#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR)/sizeof(*_ARR)))         // Size of a static C-style array. Don't use on pointers!
#define IM_OFFSETOF(_TYPE,_MEMBER)  ((size_t)&(((_TYPE*)0)->_MEMBER))           // Offset of _MEMBER within _TYPE. Standardized as offsetof() in modern C++.
#define IM_UNUSED(_VAR)             ((void)_VAR)                                // Used to silence "unused variable warnings". Often useful as asserts may be stripped out from final builds.

// Warnings
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#elif defined(__GNUC__) && __GNUC__ >= 8
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif

//-----------------------------------------------------------------------------
// Forward declarations and basic types
//-----------------------------------------------------------------------------

struct ImDrawChannel;               // Temporary stohnly for ImDrawList ot output draw commands out of order, used by ImDrawList::ChannelsSplit()
struct ImDrawCmd;                   // A single draw command within a parent ImDrawList (generally maps to 1 GPU draw call, unless it is a callback)
struct ImDrawData;                  // All draw command lists required to render the frame + pos/size coordinates to use for the projection matrix.
struct ImDrawList;                  // A single draw command list (generally one per window, conceptually you may see this as a dynamic "mesh" builder)
struct ImDrawListSharedData;        // Data shared among multiple draw lists (typically owned by parent HanaLovesMe context, but you may create one yourself)
struct ImDrawVert;                  // A single vertex (pos + uv + col = 20 bytes by default. Override layout with HanaLovesMe_OVERRIDE_DRAWVERT_STRUCT_LAYOUT)
struct ImFont;                      // Runtime data for a single font within a parent ImFontAtlas
struct ImFontAtlas;                 // Runtime data for multiple fonts, bake multiple fonts into a single texture, TTF/OTF font loader
struct ImFontConfig;                // Configuration data when adding a font or merging fonts
struct ImFontGlyph;                 // A single font glyph (code point + coordinates within in ImFontAtlas + offset)
struct ImFontGlyphRangesBuilder;    // Helper to build glyph ranges from text/string data
struct xuicolor;                     // Helper functions to create a color that can be converted to either u32 or float4 (*OBSOLETE* please avoid using)
struct HanaLovesMeContext;                // Dear HanaLovesMe context (opaque structure, unless including HanaLovesMe_internal.h)
struct HanaLovesMeIO;                     // Main configuration and I/O between your application and HanaLovesMe
struct HanaLovesMeInputTextCallbackData;  // Shared state of InputText() when using custom HanaLovesMeInputTextCallback (rare/advanced use)
struct HanaLovesMeListClipper;            // Helper to manually clip large list of items
struct HanaLovesMeOnceUponAFrame;         // Helper for running a block of code not more than once a frame, used by HanaLovesMe_ONCE_UPON_A_FRAME macro
struct HanaLovesMePayload;                // User data payload for drag and drop operations
struct HanaLovesMeSizeCallbackData;       // Callback data when using SetNextWindowSizeConstraints() (rare/advanced use)
struct HanaLovesMeStohnly;                // Helper for key->value stohnly
struct HanaLovesMeStyle;                  // Runtime data for styling/colors
struct HanaLovesMeTextBuffer;             // Helper to hold and append into a text buffer (~string builder)
struct HanaLovesMeTextFilter;             // Helper to parse and apply text filters (e.g. "aaaaa[,bbbb][,ccccc]")

// Typedefs and Enums/Flags (declared as int for compatibility with old C++, to allow using as flags and to not pollute the top of this file)
// Use your programming IDE "Go to definition" facility on the names of the center columns to find the actual flags/enum lists.
#ifndef ImTextureID
typedef void* ImTextureID;          // User data to identify a texture (this is whatever to you want it to be! read the FAQ about ImTextureID in HanaLovesMe.cpp)
#endif
typedef unsigned int HanaLovesMeID;       // Unique ID used by widgets (typically hashed from a stack of string)
typedef unsigned short ImWchar;     // A single U16 character for keyboard input/display. We encode them as multi bytes UTF-8 when used in strings.
typedef int HanaLovesMeCol;               // -> enum HanaLovesMeCol_             // Enum: A color identifier for styling
typedef int HanaLovesMeCond;              // -> enum HanaLovesMeCond_            // Enum: A condition for Set*()
typedef int HanaLovesMeDataType;          // -> enum HanaLovesMeDataType_        // Enum: A primary data type
typedef int HanaLovesMeDir;               // -> enum HanaLovesMeDir_             // Enum: A cardinal direction
typedef int HanaLovesMeKey;               // -> enum HanaLovesMeKey_             // Enum: A key identifier (HanaLovesMe-side enum)
typedef int HanaLovesMeNavInput;          // -> enum HanaLovesMeNavInput_        // Enum: An input identifier for navigation
typedef int HanaLovesMeMouseCursor;       // -> enum HanaLovesMeMouseCursor_     // Enum: A mouse cursor identifier
typedef int HanaLovesMeStyleVar;          // -> enum HanaLovesMeStyleVar_        // Enum: A variable identifier for styling
typedef int ImDrawCornerFlags;      // -> enum ImDrawCornerFlags_    // Flags: for ImDrawList::AddRect*() etc.
typedef int ImDrawListFlags;        // -> enum ImDrawListFlags_      // Flags: for ImDrawList
typedef int ImFontAtlasFlags;       // -> enum ImFontAtlasFlags_     // Flags: for ImFontAtlas
typedef int HanaLovesMeBackendFlags;      // -> enum HanaLovesMeBackendFlags_    // Flags: for io.BackendFlags
typedef int HanaLovesMeColorEditFlags;    // -> enum HanaLovesMeColorEditFlags_  // Flags: for ColorEdit*(), ColorPicker*()
typedef int HanaLovesMeColumnsFlags;      // -> enum HanaLovesMeColumnsFlags_    // Flags: for Columns(), BeginColumns()
typedef int HanaLovesMeConfigFlags;       // -> enum HanaLovesMeConfigFlags_     // Flags: for io.ConfigFlags
typedef int HanaLovesMeComboFlags;        // -> enum HanaLovesMeComboFlags_      // Flags: for BeginCombo()
typedef int HanaLovesMeDragDropFlags;     // -> enum HanaLovesMeDragDropFlags_   // Flags: for *DragDrop*()
typedef int HanaLovesMeFocusedFlags;      // -> enum HanaLovesMeFocusedFlags_    // Flags: for IsWindowFocused()
typedef int HanaLovesMeHoveredFlags;      // -> enum HanaLovesMeHoveredFlags_    // Flags: for IsItemHovered(), IsWindowHovered() etc.
typedef int HanaLovesMeInputTextFlags;    // -> enum HanaLovesMeInputTextFlags_  // Flags: for InputText*()
typedef int HanaLovesMeSelectableFlags;   // -> enum HanaLovesMeSelectableFlags_ // Flags: for Selectable()
typedef int HanaLovesMeTabBarFlags;       // -> enum HanaLovesMeTabBarFlags_     // Flags: for BeginTabBar()
typedef int HanaLovesMeTabItemFlags;      // -> enum HanaLovesMeTabItemFlags_    // Flags: for BeginTabItem()
typedef int HanaLovesMeTreeNodeFlags;     // -> enum HanaLovesMeTreeNodeFlags_   // Flags: for TreeNode*(),CollapsingHeader()
typedef int HanaLovesMeWindowFlags;       // -> enum HanaLovesMeWindowFlags_     // Flags: for Begin*()
typedef int (*HanaLovesMeInputTextCallback)(HanaLovesMeInputTextCallbackData* data);
typedef void (*HanaLovesMeSizeCallback)(HanaLovesMeSizeCallbackData* data);

// Scalar data types
typedef signed char         ImS8;   // 8-bit signed integer == char
typedef unsigned char       ImU8;   // 8-bit unsigned integer
typedef signed short        ImS16;  // 16-bit signed integer
typedef unsigned short      ImU16;  // 16-bit unsigned integer
typedef signed int          ImS32;  // 32-bit signed integer == int
typedef unsigned int        ImU32;  // 32-bit unsigned integer (often used to store packed colors)
#if defined(_MSC_VER) && !defined(__clang__)
typedef signed   __int64    ImS64;  // 64-bit signed integer (pre and post C++11 with Visual Studio)
typedef unsigned __int64    ImU64;  // 64-bit unsigned integer (pre and post C++11 with Visual Studio)
#elif (defined(__clang__) || defined(__GNUC__)) && (__cplusplus < 201100)
#include <stdint.h>
typedef int64_t             ImS64;  // 64-bit signed integer (pre C++11)
typedef uint64_t            ImU64;  // 64-bit unsigned integer (pre C++11)
#else
typedef signed   long long  ImS64;  // 64-bit signed integer (post C++11)
typedef unsigned long long  ImU64;  // 64-bit unsigned integer (post C++11)
#endif

// 2D vector (often used to store positions, sizes, etc.)
struct vsize
{
    float     x, y;
    vsize() { x = y = 0.0f; }
    vsize(float _x, float _y) { x = _x; y = _y; }
    float  operator[] (size_t idx) const { IM_ASSERT(idx <= 1); return (&x)[idx]; }    // We very rarely use this [] operator, the assert overhead is fine.
    float& operator[] (size_t idx) { IM_ASSERT(idx <= 1); return (&x)[idx]; }    // We very rarely use this [] operator, the assert overhead is fine.
#ifdef IM_VEC2_CLASS_EXTRA
    IM_VEC2_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h to convert back and forth between your math types and vsize.
#endif
};

// 4D vector (often used to store floating-point colors)
struct xuifloatcolor
{
    float     x, y, z, w;
    xuifloatcolor() { x = y = z = w = 0.0f; }
    xuifloatcolor(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
#ifdef IM_VEC4_CLASS_EXTRA
    IM_VEC4_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h to convert back and forth between your math types and xuifloatcolor.
#endif
};

//-----------------------------------------------------------------------------
// HanaLovesMe: Dear HanaLovesMe end-user API
// (Inside a namespace so you can add extra functions in your own separate file. Please don't modify HanaLovesMe.cpp/.h!)
//-----------------------------------------------------------------------------

namespace HanaLovesMe
{
    // Context creation and access
    // Each context create its own ImFontAtlas by default. You may instance one yourself and pass it to CreateContext() to share a font atlas between HanaLovesMe contexts.
    // All those functions are not reliant on the current context.
    HanaLovesMe_API HanaLovesMeContext* CreateContext(ImFontAtlas* shared_font_atlas = NULL);
    HanaLovesMe_API void          DestroyContext(HanaLovesMeContext* ctx = NULL);   // NULL = destroy current context
    HanaLovesMe_API HanaLovesMeContext* GetCurrentContext();
    HanaLovesMe_API void          SetCurrentContext(HanaLovesMeContext* ctx);
    HanaLovesMe_API bool          DebugCheckVersionAndDataLayout(const char* version_str, size_t sz_io, size_t sz_style, size_t sz_vec2, size_t sz_vec4, size_t sz_drawvert);

    // Main
    HanaLovesMe_API HanaLovesMeIO& GetIO();                                    // access the IO structure (mouse/keyboard/gamepad inputs, time, various configuration options/flags)
    HanaLovesMe_API HanaLovesMeStyle& GetStyle();                                 // access the Style structure (colors, sizes). Always use PushStyleCol(), PushStyleVar() to modify style mid-frame.
    HanaLovesMe_API void          NewFrame();                                 // start a new HanaLovesMe frame, you can submit any command from this point until Render()/EndFrame().
    HanaLovesMe_API void          EndFrame();                                 // ends the HanaLovesMe frame. automatically called by Render(), you likely don't need to call that yourself directly. If you don't need to render data (skipping rendering) you may call EndFrame() but you'll have wasted CPU already! If you don't need to render, better to not create any HanaLovesMe windows and not call NewFrame() at all!
    HanaLovesMe_API void          Render();                                   // ends the HanaLovesMe frame, finalize the draw data. (Obsolete: optionally call io.RenderDrawListsFn if set. Nowadays, prefer calling your render function yourself.)
    HanaLovesMe_API ImDrawData* GetDrawData();                              // valid after Render() and until the next call to NewFrame(). this is what you have to render. (Obsolete: this used to be passed to your io.RenderDrawListsFn() function.)

    // Demo, Debug, Information
    HanaLovesMe_API void          ShowDemoWindow(bool* p_open = NULL);        // create demo/test window (previously called ShowTestWindow). demonstrate most HanaLovesMe features. call this to learn about the library! try to make it always available in your application!
    HanaLovesMe_API void          ShowAboutWindow(bool* p_open = NULL);       // create about window. display Dear HanaLovesMe version, credits and build/system information.
    HanaLovesMe_API void          ShowMetricsWindow(bool* p_open = NULL);     // create metrics/debug window. display Dear HanaLovesMe internals: draw commands (with individual draw calls and vertices), window list, basic internal state, etc.
    HanaLovesMe_API void          ShowStyleEditor(HanaLovesMeStyle* ref = NULL);    // add style editor block (not a window). you can pass in a reference HanaLovesMeStyle structure to compare to, revert to and save to (else it uses the default style)
    HanaLovesMe_API bool          ShowStyleSelector(const char* label);       // add style selector block (not a window), essentially a combo listing the default styles.
    HanaLovesMe_API void          ShowFontSelector(const char* label);        // add font selector block (not a window), essentially a combo listing the loaded fonts.
    HanaLovesMe_API void          ShowUserGuide();                            // add basic help/info block (not a window): how to manipulate HanaLovesMe as a end-user (mouse/keyboard controls).
    HanaLovesMe_API const char* GetVersion();                               // get the compiled version string e.g. "1.23" (essentially the compiled value for HanaLovesMe_VERSION)

    // Styles
    HanaLovesMe_API void          StyleColorsDark(HanaLovesMeStyle* dst = NULL);    // new, recommended style (default)
    HanaLovesMe_API void          StyleColorsClassic(HanaLovesMeStyle* dst = NULL); // classic HanaLovesMe style
    HanaLovesMe_API void          StyleColorsLight(HanaLovesMeStyle* dst = NULL);   // best used with borders and a custom, thicker font

    // Windows
    // - Begin() = push window to the stack and start appending to it. End() = pop window from the stack.
    // - You may append multiple times to the same window during the same frame.
    // - Passing 'bool* p_open != NULL' shows a window-closing widget in the upper-right corner of the window,
    //   which clicking will set the boolean to false when clicked.
    // - Begin() return false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting
    //   anything to the window. Always call a matching End() for each Begin() call, regardless of its return value!
    //   [this is due to legacy reason and is inconsistent with most other functions such as BeginMenu/EndMenu, BeginPopup/EndPopup, etc.
    //    where the EndXXX call should only be called if the corresponding BeginXXX function returned true.]
    // - Note that the bottom of window stack always contains a window called "Debug".
    HanaLovesMe_API bool          Begin(const char* name, bool* p_open = NULL, HanaLovesMeWindowFlags flags = 0);
    HanaLovesMe_API void          End();

	HanaLovesMe_API bool          BeginColorPickerBackground(const char* name, bool* p_open = NULL, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API bool          BeginTabsBackground(const char* name, bool* p_open = NULL, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API bool          BeginMenuBackground(const char* name, bool* p_open = NULL, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API bool          BeginShadowBackground(const char* name, bool* p_open = NULL, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API bool          BeginGroupBoxMain(const char* name, bool* p_open = NULL, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API bool          BeginGroupBoxScrollMain(const char* name, const char* groupboxName, bool* p_open = NULL, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API bool          BeginComboBackground(const char* name, bool* p_open = NULL, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API bool          BeginEndBorder(const char* name, bool* p_open = NULL, HanaLovesMeWindowFlags flags = 0);

    // Child Windows
    // - Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window. Child windows can embed their own child.
    // - For each independent axis of 'size': ==0.0f: use remaining host window size / >0.0f: fixed size / <0.0f: use remaining window size minus abs(size) / Each axis can use a different mode, e.g. vsize(0,400).
    // - BeginChild() returns false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting anything to the window.
    //   Always call a matching EndChild() for each BeginChild() call, regardless of its return value [this is due to legacy reason and is inconsistent with most other functions such as BeginMenu/EndMenu, BeginPopup/EndPopup, etc. where the EndXXX call should only be called if the corresponding BeginXXX function returned true.]
    HanaLovesMe_API bool          BeginChild(const char* str_id, const vsize& size = vsize(0, 0), bool border = false, HanaLovesMeWindowFlags flags = 0);
    HanaLovesMe_API bool          BeginChild(HanaLovesMeID id, const vsize& size = vsize(0, 0), bool border = false, HanaLovesMeWindowFlags flags = 0);
    HanaLovesMe_API void          EndChild();

	HanaLovesMe_API bool          BeginTabs(const char* str_id, const vsize& size = vsize(0, 0), bool border = false, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API bool          BeginTabs(HanaLovesMeID id, const vsize& size = vsize(0, 0), bool border = false, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API void          EndTabs();

	HanaLovesMe_API bool          EndBorder(const char* str_id, const vsize& size = vsize(0, 0), bool border = false, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API bool          EndBorder(HanaLovesMeID id, const vsize& size = vsize(0, 0), bool border = false, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API void          EndEndBorder();

	HanaLovesMe_API bool          BeginShadow(const char* str_id, const vsize& size = vsize(0, 0), bool border = false, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API bool          BeginShadow(HanaLovesMeID id, const vsize& size = vsize(0, 0), bool border = false, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API void          EndShadow();

	HanaLovesMe_API bool          BeginGroupBox(const char* str_id, const vsize& size = vsize(0, 0), bool border = false, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API bool          BeginGroupBox(HanaLovesMeID id, const vsize& size = vsize(0, 0), bool border = false, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API void          EndGroupBox();


	HanaLovesMe_API bool          BeginGroupBox2(const char* str_id, const vsize& size = vsize(0, 0), bool border = false, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API bool          BeginGroupBox2(HanaLovesMeID id, const vsize& size = vsize(0, 0), bool border = false, HanaLovesMeWindowFlags flags = 0);


	HanaLovesMe_API bool          BeginGroupBoxScroll(const char* str_id, const char* groupboxName, const vsize& size = vsize(0, 0), bool border = false, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API bool          BeginGroupBoxScroll(HanaLovesMeID id, const char* groupboxName, const vsize& size = vsize(0, 0), bool border = false, HanaLovesMeWindowFlags flags = 0);
	HanaLovesMe_API void          EndGroupBoxScroll();

    // Windows Utilities
    // - "current window" = the window we are appending into while inside a Begin()/End() block. "next window" = next window we will Begin() into.
    HanaLovesMe_API bool          IsWindowAppearing();
    HanaLovesMe_API bool          IsWindowCollapsed();
    HanaLovesMe_API bool          IsWindowFocused(HanaLovesMeFocusedFlags flags = 0); // is current window focused? or its root/child, depending on flags. see flags for options.
    HanaLovesMe_API bool          IsWindowHovered(HanaLovesMeHoveredFlags flags = 0); // is current window hovered (and typically: not blocked by a popup/modal)? see flags for options. NB: If you are trying to check whether your mouse should be dispatched to HanaLovesMe or to your app, you should use the 'io.WantCaptureMouse' boolean for that! Please read the FAQ!
    HanaLovesMe_API ImDrawList* GetWindowDrawList();                        // get draw list associated to the current window, to append your own drawing primitives
    HanaLovesMe_API vsize        GetWindowPos();                             // get current window position in screen space (useful if you want to do your own drawing via the DrawList API)
    HanaLovesMe_API vsize        GetWindowSize();                            // get current window size
    HanaLovesMe_API float         GetWindowWidth();                           // get current window width (shortcut for GetWindowSize().x)
    HanaLovesMe_API float         GetWindowHeight();                          // get current window height (shortcut for GetWindowSize().y)

    // Prefer using SetNextXXX functions (before Begin) rather that SetXXX functions (after Begin).
    HanaLovesMe_API void          SetNextWindowPos(const vsize& pos, HanaLovesMeCond cond = 0, const vsize& pivot = vsize(0, 0)); // set next window position. call before Begin(). use pivot=(0.5f,0.5f) to center on given point, etc.
    HanaLovesMe_API void          SetNextWindowSize(const vsize& size, HanaLovesMeCond cond = 0);                  // set next window size. set axis to 0.0f to force an auto-fit on this axis. call before Begin()
    HanaLovesMe_API void          SetNextWindowSizeConstraints(const vsize& size_min, const vsize& size_max, HanaLovesMeSizeCallback custom_callback = NULL, void* custom_callback_data = NULL); // set next window size limits. use -1,-1 on either X/Y axis to preserve the current size. Use callback to apply non-trivial programmatic constraints.
    HanaLovesMe_API void          SetNextWindowContentSize(const vsize& size);                               // set next window content size (~ enforce the range of scrollbars). not including window decorations (title bar, menu bar, etc.). set an axis to 0.0f to leave it automatic. call before Begin()
    HanaLovesMe_API void          SetNextWindowCollapsed(bool collapsed, HanaLovesMeCond cond = 0);                 // set next window collapsed state. call before Begin()
    HanaLovesMe_API void          SetNextWindowFocus();                                                       // set next window to be focused / front-most. call before Begin()
    HanaLovesMe_API void          SetNextWindowBgAlpha(float alpha);                                          // set next window background color alpha. helper to easily modify HanaLovesMeCol_WindowBg/ChildBg/PopupBg. you may also use HanaLovesMeWindowFlags_NoBackground.
    HanaLovesMe_API void          SetWindowPos(const vsize& pos, HanaLovesMeCond cond = 0);                        // (not recommended) set current window position - call within Begin()/End(). prefer using SetNextWindowPos(), as this may incur tearing and side-effects.
    HanaLovesMe_API void          SetWindowSize(const vsize& size, HanaLovesMeCond cond = 0);                      // (not recommended) set current window size - call within Begin()/End(). set to vsize(0,0) to force an auto-fit. prefer using SetNextWindowSize(), as this may incur tearing and minor side-effects.
    HanaLovesMe_API void          SetWindowCollapsed(bool collapsed, HanaLovesMeCond cond = 0);                     // (not recommended) set current window collapsed state. prefer using SetNextWindowCollapsed().
    HanaLovesMe_API void          SetWindowFocus();                                                           // (not recommended) set current window to be focused / front-most. prefer using SetNextWindowFocus().
    HanaLovesMe_API void          SetWindowFontScale(float scale);                                            // set font scale. Adjust IO.FontGlobalScale if you want to scale all windows
    HanaLovesMe_API void          SetWindowPos(const char* name, const vsize& pos, HanaLovesMeCond cond = 0);      // set named window position.
    HanaLovesMe_API void          SetWindowSize(const char* name, const vsize& size, HanaLovesMeCond cond = 0);    // set named window size. set axis to 0.0f to force an auto-fit on this axis.
    HanaLovesMe_API void          SetWindowCollapsed(const char* name, bool collapsed, HanaLovesMeCond cond = 0);   // set named window collapsed state
    HanaLovesMe_API void          SetWindowFocus(const char* name);                                           // set named window to be focused / front-most. use NULL to remove focus.

    // Content region
    // - Those functions are bound to be redesigned soon (they are confusing, incomplete and return values in local window coordinates which increases confusion)
    HanaLovesMe_API vsize        GetContentRegionMax();                                          // current content boundaries (typically window boundaries including scrolling, or current column boundaries), in windows coordinates
    HanaLovesMe_API vsize        GetContentRegionAvail();                                        // == GetContentRegionMax() - GetCursorPos()
    HanaLovesMe_API float         GetContentRegionAvailWidth();                                   // == GetContentRegionAvail().x
    HanaLovesMe_API vsize        GetWindowContentRegionMin();                                    // content boundaries min (roughly (0,0)-Scroll), in window coordinates
    HanaLovesMe_API vsize        GetWindowContentRegionMax();                                    // content boundaries max (roughly (0,0)+Size-Scroll) where Size can be override with SetNextWindowContentSize(), in window coordinates
    HanaLovesMe_API float         GetWindowContentRegionWidth();                                  //

    // Windows Scrolling
    HanaLovesMe_API float         GetScrollX();                                                   // get scrolling amount [0..GetScrollMaxX()]
    HanaLovesMe_API float         GetScrollY();                                                   // get scrolling amount [0..GetScrollMaxY()]
    HanaLovesMe_API float         GetScrollMaxX();                                                // get maximum scrolling amount ~~ ContentSize.X - WindowSize.X
    HanaLovesMe_API float         GetScrollMaxY();                                                // get maximum scrolling amount ~~ ContentSize.Y - WindowSize.Y
    HanaLovesMe_API void          SetScrollX(float scroll_x);                                     // set scrolling amount [0..GetScrollMaxX()]
    HanaLovesMe_API void          SetScrollY(float scroll_y);                                     // set scrolling amount [0..GetScrollMaxY()]
    HanaLovesMe_API void          SetScrollHereY(float center_y_ratio = 0.5f);                    // adjust scrolling amount to make current cursor position visible. center_y_ratio=0.0: top, 0.5: center, 1.0: bottom. When using to make a "default/current item" visible, consider using SetItemDefaultFocus() instead.
    HanaLovesMe_API void          SetScrollFromPosY(float local_y, float center_y_ratio = 0.5f);  // adjust scrolling amount to make given position visible. Generally GetCursorStartPos() + offset to compute a valid position.

    // Parameters stacks (shared)
    HanaLovesMe_API void          PushFont(ImFont* font);                                         // use NULL as a shortcut to push default font
    HanaLovesMe_API void          PopFont();
    HanaLovesMe_API void          PushStyleColor(HanaLovesMeCol idx, ImU32 col);
    HanaLovesMe_API void          PushStyleColor(HanaLovesMeCol idx, const xuifloatcolor& col);
	HanaLovesMe_API void			PushColor(HanaLovesMeCol idx, HanaLovesMeCol idx2, const xuifloatcolor& col);
    HanaLovesMe_API void          PopStyleColor(int count = 1);
    HanaLovesMe_API void          PushStyleVar(HanaLovesMeStyleVar idx, float val);
    HanaLovesMe_API void          PushStyleVar(HanaLovesMeStyleVar idx, const vsize& val);
    HanaLovesMe_API void          PopStyleVar(int count = 1);
    HanaLovesMe_API const xuifloatcolor& GetStyleColorVec4(HanaLovesMeCol idx);                                // retrieve style color as stored in HanaLovesMeStyle structure. use to feed back into PushStyleColor(), otherwise use GetColorU32() to get style color with style alpha baked in.
    HanaLovesMe_API ImFont* GetFont();                                                      // get current font
    HanaLovesMe_API float         GetFontSize();                                                  // get current font size (= height in pixels) of current font with current scale applied
    HanaLovesMe_API vsize        GetFontTexUvWhitePixel();                                       // get UV coordinate for a while pixel, useful to draw custom shapes via the ImDrawList API
    HanaLovesMe_API ImU32         GetColorU32(HanaLovesMeCol idx, float alpha_mul = 1.0f);              // retrieve given style color with style alpha applied and optional extra alpha multiplier
    HanaLovesMe_API ImU32         GetColorU32(const xuifloatcolor& col);                                 // retrieve given color with style alpha applied
    HanaLovesMe_API ImU32         GetColorU32(ImU32 col);                                         // retrieve given color with style alpha applied

    // Parameters stacks (current window)
    HanaLovesMe_API void          PushItemWidth(float item_width);                                // width of items for the common item+label case, pixels. 0.0f = default to ~2/3 of windows width, >0.0f: width in pixels, <0.0f align xx pixels to the right of window (so -1.0f always align width to the right side)
    HanaLovesMe_API void          PopItemWidth();
    HanaLovesMe_API float         CalcItemWidth();                                                // width of item given pushed settings and current cursor position
    HanaLovesMe_API void          PushTextWrapPos(float wrap_local_pos_x = 0.0f);                 // word-wrapping for Text*() commands. < 0.0f: no wrapping; 0.0f: wrap to end of window (or column); > 0.0f: wrap at 'wrap_pos_x' position in window local space
    HanaLovesMe_API void          PopTextWrapPos();
    HanaLovesMe_API void          PushAllowKeyboardFocus(bool allow_keyboard_focus);              // allow focusing using TAB/Shift-TAB, enabled by default but you can disable it for certain widgets
    HanaLovesMe_API void          PopAllowKeyboardFocus();
    HanaLovesMe_API void          PushButtonRepeat(bool repeat);                                  // in 'repeat' mode, Button*() functions return repeated true in a typematic manner (using io.KeyRepeatDelay/io.KeyRepeatRate setting). Note that you can call IsItemActive() after any Button() to tell if the button is held in the current frame.
    HanaLovesMe_API void          PopButtonRepeat();

    // Cursor / Layout
    // - By "cursor" we mean the current output position.
    // - The typical widget behavior is to output themselves at the current cursor position, then move the cursor one line down.
    HanaLovesMe_API void          Separator();                                                    // separator, generally horizontal. inside a menu bar or in horizontal layout mode, this becomes a vertical separator.
    HanaLovesMe_API void          SameLine(float offset_from_start_x = 0.0f, float spacing = -1.0f);  // call between widgets or groups to layout them horizontally. X position given in window coordinates.
    HanaLovesMe_API void          NewLine();                                                      // undo a SameLine() or force a new line when in an horizontal-layout context.
    HanaLovesMe_API void          Spacing();                                                      // add vertical spacing.
	HanaLovesMe_API void			CustomSpacing(float h);											// add custom vertical spacing
    HanaLovesMe_API void          Dummy(const vsize& size);                                      // add a dummy item of given size. unlike InvisibleButton(), Dummy() won't take the mouse click or be navigable into.
    HanaLovesMe_API void          Indent(float indent_w = 0.0f);                                  // move content position toward the right, by style.IndentSpacing or indent_w if != 0
    HanaLovesMe_API void          Unindent(float indent_w = 0.0f);                                // move content position back to the left, by style.IndentSpacing or indent_w if != 0
    HanaLovesMe_API void          BeginGroup();                                                   // lock horizontal starting position
    HanaLovesMe_API void          EndGroup();                                                     // unlock horizontal starting position + capture the whole group bounding box into one "item" (so you can use IsItemHovered() or layout primitives such as SameLine() on whole group, etc.)
    HanaLovesMe_API vsize        GetCursorPos();                                                 // cursor position in window coordinates (relative to window position)
    HanaLovesMe_API float         GetCursorPosX();                                                //   (some functions are using window-relative coordinates, such as: GetCursorPos, GetCursorStartPos, GetContentRegionMax, GetWindowContentRegion* etc.
    HanaLovesMe_API float         GetCursorPosY();                                                //    other functions such as GetCursorScreenPos or everything in ImDrawList::
    HanaLovesMe_API void          SetCursorPos(const vsize& local_pos);                          //    are using the main, absolute coordinate system.
    HanaLovesMe_API void          SetCursorPosX(float local_x);                                   //    GetWindowPos() + GetCursorPos() == GetCursorScreenPos() etc.)
    HanaLovesMe_API void          SetCursorPosY(float local_y);                                   //
    HanaLovesMe_API vsize        GetCursorStartPos();                                            // initial cursor position in window coordinates
    HanaLovesMe_API vsize        GetCursorScreenPos();                                           // cursor position in absolute screen coordinates [0..io.DisplaySize] (useful to work with ImDrawList API)
    HanaLovesMe_API void          SetCursorScreenPos(const vsize& pos);                          // cursor position in absolute screen coordinates [0..io.DisplaySize]
    HanaLovesMe_API void          AlignTextToFramePadding();                                      // vertically align upcoming text baseline to FramePadding.y so that it will align properly to regularly framed items (call if you have text on a line before a framed item)
    HanaLovesMe_API float         GetTextLineHeight();                                            // ~ FontSize
    HanaLovesMe_API float         GetTextLineHeightWithSpacing();                                 // ~ FontSize + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of text)
    HanaLovesMe_API float         GetFrameHeight();                                               // ~ FontSize + style.FramePadding.y * 2
    HanaLovesMe_API float         GetFrameHeightWithSpacing();                                    // ~ FontSize + style.FramePadding.y * 2 + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of framed widgets)

    // ID stack/scopes
    // - Read the FAQ for more details about how ID are handled in dear HanaLovesMe. If you are creating widgets in a loop you most
    //   likely want to push a unique identifier (e.g. object pointer, loop index) to uniquely differentiate them.
    // - The resulting ID are hashes of the entire stack.
    // - You can also use the "Label##foobar" syntax within widget label to distinguish them from each others.
    // - In this header file we use the "label"/"name" terminology to denote a string that will be displayed and used as an ID,
    //   whereas "str_id" denote a string that is only used as an ID and not normally displayed.
    HanaLovesMe_API void          PushID(const char* str_id);                                     // push string into the ID stack (will hash string).
    HanaLovesMe_API void          PushID(const char* str_id_begin, const char* str_id_end);       // push string into the ID stack (will hash string).
    HanaLovesMe_API void          PushID(const void* ptr_id);                                     // push pointer into the ID stack (will hash pointer).
    HanaLovesMe_API void          PushID(int int_id);                                             // push integer into the ID stack (will hash integer).
    HanaLovesMe_API void          PopID();                                                        // pop from the ID stack.
    HanaLovesMe_API HanaLovesMeID       GetID(const char* str_id);                                      // calculate unique ID (hash of whole ID stack + given parameter). e.g. if you want to query into HanaLovesMeStohnly yourself
    HanaLovesMe_API HanaLovesMeID       GetID(const char* str_id_begin, const char* str_id_end);
    HanaLovesMe_API HanaLovesMeID       GetID(const void* ptr_id);

    // Widgets: Text
    HanaLovesMe_API void          TextUnformatted(const char* text, const char* text_end = NULL);                // raw text without formatting. Roughly equivalent to Text("%s", text) but: A) doesn't require null terminated string if 'text_end' is specified, B) it's faster, no memory copy is done, no buffer size limits, recommended for long chunks of text.
    HanaLovesMe_API void          Text(const char* fmt, ...)                                      IM_FMTARGS(1); // simple formatted text
    HanaLovesMe_API void          TextV(const char* fmt, va_list args)                            IM_FMTLIST(1);
    HanaLovesMe_API void          TextColored(const xuifloatcolor& col, const char* fmt, ...)            IM_FMTARGS(2); // shortcut for PushStyleColor(HanaLovesMeCol_Text, col); Text(fmt, ...); PopStyleColor();
    HanaLovesMe_API void          TextColoredV(const xuifloatcolor& col, const char* fmt, va_list args)  IM_FMTLIST(2);
    HanaLovesMe_API void          TextDisabled(const char* fmt, ...)                              IM_FMTARGS(1); // shortcut for PushStyleColor(HanaLovesMeCol_Text, style.Colors[HanaLovesMeCol_TextDisabled]); Text(fmt, ...); PopStyleColor();
    HanaLovesMe_API void          TextDisabledV(const char* fmt, va_list args)                    IM_FMTLIST(1);
    HanaLovesMe_API void          TextWrapped(const char* fmt, ...)                               IM_FMTARGS(1); // shortcut for PushTextWrapPos(0.0f); Text(fmt, ...); PopTextWrapPos();. Note that this won't work on an auto-resizing window if there's no other widgets to extend the window width, yoy may need to set a size using SetNextWindowSize().
    HanaLovesMe_API void          TextWrappedV(const char* fmt, va_list args)                     IM_FMTLIST(1);
    HanaLovesMe_API void          LabelText(const char* label, const char* fmt, ...)              IM_FMTARGS(2); // display text+label aligned the same way as value+label widgets
    HanaLovesMe_API void          LabelTextV(const char* label, const char* fmt, va_list args)    IM_FMTLIST(2);
    HanaLovesMe_API void          BulletText(const char* fmt, ...)                                IM_FMTARGS(1); // shortcut for Bullet()+Text()
    HanaLovesMe_API void          BulletTextV(const char* fmt, va_list args)                      IM_FMTLIST(1);

    // Widgets: Main
    // - Most widgets return true when the value has been changed or when pressed/selected
	HanaLovesMe_API bool          SelectedTab(const char* label, const vsize& size = vsize(0, 0));    // selected tab
	HanaLovesMe_API bool          Tab(const char* label, const vsize& size = vsize(0, 0));    // tab
	HanaLovesMe_API bool          TabSpacer(const char* label, const vsize& size = vsize(0, 0));    // TabSpacer
	HanaLovesMe_API bool          TabSpacer2(const char* label, const vsize& size = vsize(0, 0));    // TabSpacer2
	HanaLovesMe_API bool          GroupBoxTitle(const char* label, const vsize& size = vsize(0, 0));
	HanaLovesMe_API bool          TabButton(const char* label, const vsize& size = vsize(0, 0));    // tab button
    HanaLovesMe_API bool          Button(const char* label, const vsize& size = vsize(0, 0));    // button
	HanaLovesMe_API bool          ColorBar(const char* label, const vsize& size = vsize(0, 0));    // ColorBar
	HanaLovesMe_API bool			QuitButton(const char* label, const vsize& size = vsize(0, 0));    // quit button
	HanaLovesMe_API bool          NoInputButton(const char* label, const vsize& size = vsize(0, 0));    // button without input
    HanaLovesMe_API bool          SmallButton(const char* label);                                 // button with FramePadding=(0,0) to easily embed within text
    HanaLovesMe_API bool          InvisibleButton(const char* str_id, const vsize& size);        // button behavior without the visuals, frequently useful to build custom behaviors using the public api (along with IsItemActive, IsItemHovered, etc.)
    HanaLovesMe_API bool          ArrowButton(const char* str_id, HanaLovesMeDir dir);                  // square button with an arrow shape
    HanaLovesMe_API void          Image(ImTextureID user_texture_id, const vsize& size, const vsize& uv0 = vsize(0, 0), const vsize& uv1 = vsize(1, 1), const xuifloatcolor& tint_col = xuifloatcolor(1, 1, 1, 1), const xuifloatcolor& border_col = xuifloatcolor(0, 0, 0, 0));
    HanaLovesMe_API bool          ImageButton(ImTextureID user_texture_id, const vsize& size, const vsize& uv0 = vsize(0, 0), const vsize& uv1 = vsize(1, 1), int frame_padding = -1, const xuifloatcolor& bg_col = xuifloatcolor(0, 0, 0, 0), const xuifloatcolor& tint_col = xuifloatcolor(1, 1, 1, 1));    // <0 frame_padding uses default frame padding settings. 0 for no padding
    HanaLovesMe_API bool          Checkbox(const char* label, bool* v);
    HanaLovesMe_API bool          CheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value);
    HanaLovesMe_API bool          RadioButton(const char* label, bool active);                    // use with e.g. if (RadioButton("one", my_value==1)) { my_value = 1; }
    HanaLovesMe_API bool          RadioButton(const char* label, int* v, int v_button);           // shortcut to handle the above pattern when value is an integer
    HanaLovesMe_API void          ProgressBar(float fraction, const vsize& size_arg = vsize(-1, 0), const char* overlay = NULL);
    HanaLovesMe_API void          Bullet();                                                       // draw a small circle and keep the cursor on the same line. advance cursor x position by GetTreeNodeToLabelSpacing(), same distance that TreeNode() uses

    // Widgets: Combo Box
    // - The new BeginCombo()/EndCombo() api allows you to manage your contents and selection state however you want it, by creating e.g. Selectable() items.
    // - The old Combo() api are helpers over BeginCombo()/EndCombo() which are kept available for convenience purpose.
    HanaLovesMe_API bool          BeginCombo(const char* label, const char* preview_value, HanaLovesMeComboFlags flags = 0);
	HanaLovesMe_API bool			MultiCombo(const char* name, const char** displayName, bool* data, int dataSize);
    HanaLovesMe_API void          EndCombo(); // only call EndCombo() if BeginCombo() returns true!
    HanaLovesMe_API bool          Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);
    HanaLovesMe_API bool          Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);      // Separate items with \0 within a string, end item-list with \0\0. e.g. "One\0Two\0Three\0"
    HanaLovesMe_API bool          Combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items = -1);

    // Widgets: Drags
    // - CTRL+Click on any drag box to turn them into an input box. Manually input values aren't clamped and can go off-bounds.
    // - For all the Float2/Float3/Float4/Int2/Int3/Int4 versions of every functions, note that a 'float v[X]' function argument is the same as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible. You can pass address of your first element out of a contiguous set, e.g. &myvector.x
    // - Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
    // - Speed are per-pixel of mouse movement (v_speed=0.2f: mouse needs to move by 5 pixels to increase value by 1). For gamepad/keyboard navigation, minimum speed is Max(v_speed, minimum_step_at_given_precision).
    HanaLovesMe_API bool          DragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);     // If v_min >= v_max we have no bound
    HanaLovesMe_API bool          DragFloat2(const char* label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    HanaLovesMe_API bool          DragFloat3(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    HanaLovesMe_API bool          DragFloat4(const char* label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    HanaLovesMe_API bool          DragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", const char* format_max = NULL, float power = 1.0f);
    HanaLovesMe_API bool          DragInt(const char* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");                                       // If v_min >= v_max we have no bound
    HanaLovesMe_API bool          DragInt2(const char* label, int v[2], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");
    HanaLovesMe_API bool          DragInt3(const char* label, int v[3], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");
    HanaLovesMe_API bool          DragInt4(const char* label, int v[4], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");
    HanaLovesMe_API bool          DragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", const char* format_max = NULL);
    HanaLovesMe_API bool          DragScalar(const char* label, HanaLovesMeDataType data_type, void* v, float v_speed, const void* v_min = NULL, const void* v_max = NULL, const char* format = NULL, float power = 1.0f);
    HanaLovesMe_API bool          DragScalarN(const char* label, HanaLovesMeDataType data_type, void* v, int components, float v_speed, const void* v_min = NULL, const void* v_max = NULL, const char* format = NULL, float power = 1.0f);

    // Widgets: Sliders
    // - CTRL+Click on any slider to turn them into an input box. Manually input values aren't clamped and can go off-bounds.
    // - Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
    HanaLovesMe_API bool          SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);     // adjust format to decorate the value with a prefix or a suffix for in-slider labels or unit display. Use power!=1.0 for power curve sliders
    HanaLovesMe_API bool          SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    HanaLovesMe_API bool          SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    HanaLovesMe_API bool          SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    HanaLovesMe_API bool          SliderAngle(const char* label, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f, const char* format = "%.0f deg");
    HanaLovesMe_API bool          SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d");
    HanaLovesMe_API bool          SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format = "%d");
    HanaLovesMe_API bool          SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format = "%d");
    HanaLovesMe_API bool          SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format = "%d");
    HanaLovesMe_API bool          SliderScalar(const char* label, HanaLovesMeDataType data_type, void* v, const void* v_min, const void* v_max, const char* format = NULL, float power = 1.0f);
    HanaLovesMe_API bool          SliderScalarN(const char* label, HanaLovesMeDataType data_type, void* v, int components, const void* v_min, const void* v_max, const char* format = NULL, float power = 1.0f);
    HanaLovesMe_API bool          VSliderFloat(const char* label, const vsize& size, float* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    HanaLovesMe_API bool          VSliderInt(const char* label, const vsize& size, int* v, int v_min, int v_max, const char* format = "%d");
    HanaLovesMe_API bool          VSliderScalar(const char* label, const vsize& size, HanaLovesMeDataType data_type, void* v, const void* v_min, const void* v_max, const char* format = NULL, float power = 1.0f);

    // Widgets: Input with Keyboard
    // - If you want to use InputText() with a dynamic string type such as std::string or your own, see misc/cpp/HanaLovesMe_stdlib.h
    // - Most of the HanaLovesMeInputTextFlags flags are only useful for InputText() and not for InputFloatX, InputIntX, InputDouble etc.
    HanaLovesMe_API bool          InputText(const char* label, char* buf, size_t buf_size, HanaLovesMeInputTextFlags flags = 0, HanaLovesMeInputTextCallback callback = NULL, void* user_data = NULL);
    HanaLovesMe_API bool          InputTextMultiline(const char* label, char* buf, size_t buf_size, const vsize& size = vsize(0, 0), HanaLovesMeInputTextFlags flags = 0, HanaLovesMeInputTextCallback callback = NULL, void* user_data = NULL);
    HanaLovesMe_API bool          InputTextWithHint(const char* label, const char* hint, char* buf, size_t buf_size, HanaLovesMeInputTextFlags flags = 0, HanaLovesMeInputTextCallback callback = NULL, void* user_data = NULL);
    HanaLovesMe_API bool          InputFloat(const char* label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", HanaLovesMeInputTextFlags flags = 0);
    HanaLovesMe_API bool          InputFloat2(const char* label, float v[2], const char* format = "%.3f", HanaLovesMeInputTextFlags flags = 0);
    HanaLovesMe_API bool          InputFloat3(const char* label, float v[3], const char* format = "%.3f", HanaLovesMeInputTextFlags flags = 0);
    HanaLovesMe_API bool          InputFloat4(const char* label, float v[4], const char* format = "%.3f", HanaLovesMeInputTextFlags flags = 0);
    HanaLovesMe_API bool          InputInt(const char* label, int* v, int step = 1, int step_fast = 100, HanaLovesMeInputTextFlags flags = 0);
    HanaLovesMe_API bool          InputInt2(const char* label, int v[2], HanaLovesMeInputTextFlags flags = 0);
    HanaLovesMe_API bool          InputInt3(const char* label, int v[3], HanaLovesMeInputTextFlags flags = 0);
    HanaLovesMe_API bool          InputInt4(const char* label, int v[4], HanaLovesMeInputTextFlags flags = 0);
    HanaLovesMe_API bool          InputDouble(const char* label, double* v, double step = 0.0, double step_fast = 0.0, const char* format = "%.6f", HanaLovesMeInputTextFlags flags = 0);
    HanaLovesMe_API bool          InputScalar(const char* label, HanaLovesMeDataType data_type, void* v, const void* step = NULL, const void* step_fast = NULL, const char* format = NULL, HanaLovesMeInputTextFlags flags = 0);
    HanaLovesMe_API bool          InputScalarN(const char* label, HanaLovesMeDataType data_type, void* v, int components, const void* step = NULL, const void* step_fast = NULL, const char* format = NULL, HanaLovesMeInputTextFlags flags = 0);

    // Widgets: Color Editor/Picker (tip: the ColorEdit* functions have a little colored preview square that can be left-clicked to open a picker, and right-clicked to open an option menu.)
    // - Note that in C++ a 'float v[X]' function argument is the _same_ as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible.
    // - You can pass the address of a first float element out of a contiguous structure, e.g. &myvector.x
    HanaLovesMe_API bool          ColorEdit3(const char* label, float col[3], HanaLovesMeColorEditFlags flags = 0);
    HanaLovesMe_API bool          ColorEdit4(const char* label, float col[4], HanaLovesMeColorEditFlags flags = 0);
    HanaLovesMe_API bool          ColorPicker3(const char* label, float col[3], HanaLovesMeColorEditFlags flags = 0);
    HanaLovesMe_API bool          ColorPicker4(const char* label, float col[4], HanaLovesMeColorEditFlags flags = 0, const float* ref_col = NULL);
    HanaLovesMe_API bool          ColorButton(const char* desc_id, const xuifloatcolor& col, HanaLovesMeColorEditFlags flags = 0, vsize size = vsize(0, 0));  // display a colored square/button, hover for details, return true when pressed.
    HanaLovesMe_API void          SetColorEditOptions(HanaLovesMeColorEditFlags flags);                     // initialize current options (generally on application startup) if you want to select a default format, picker type, etc. User will be able to change many settings, unless you pass the _NoOptions flag to your calls.

    // Widgets: Trees
    // - TreeNode functions return true when the node is open, in which case you need to also call TreePop() when you are finished displaying the tree node contents.
    HanaLovesMe_API bool          TreeNode(const char* label);
    HanaLovesMe_API bool          TreeNode(const char* str_id, const char* fmt, ...) IM_FMTARGS(2);   // helper variation to easily decorelate the id from the displayed string. Read the FAQ about why and how to use ID. to align arbitrary text at the same level as a TreeNode() you can use Bullet().
    HanaLovesMe_API bool          TreeNode(const void* ptr_id, const char* fmt, ...) IM_FMTARGS(2);   // "
    HanaLovesMe_API bool          TreeNodeV(const char* str_id, const char* fmt, va_list args) IM_FMTLIST(2);
    HanaLovesMe_API bool          TreeNodeV(const void* ptr_id, const char* fmt, va_list args) IM_FMTLIST(2);
    HanaLovesMe_API bool          TreeNodeEx(const char* label, HanaLovesMeTreeNodeFlags flags = 0);
    HanaLovesMe_API bool          TreeNodeEx(const char* str_id, HanaLovesMeTreeNodeFlags flags, const char* fmt, ...) IM_FMTARGS(3);
    HanaLovesMe_API bool          TreeNodeEx(const void* ptr_id, HanaLovesMeTreeNodeFlags flags, const char* fmt, ...) IM_FMTARGS(3);
    HanaLovesMe_API bool          TreeNodeExV(const char* str_id, HanaLovesMeTreeNodeFlags flags, const char* fmt, va_list args) IM_FMTLIST(3);
    HanaLovesMe_API bool          TreeNodeExV(const void* ptr_id, HanaLovesMeTreeNodeFlags flags, const char* fmt, va_list args) IM_FMTLIST(3);
    HanaLovesMe_API void          TreePush(const char* str_id);                                       // ~ Indent()+PushId(). Already called by TreeNode() when returning true, but you can call TreePush/TreePop yourself if desired.
    HanaLovesMe_API void          TreePush(const void* ptr_id = NULL);                                // "
    HanaLovesMe_API void          TreePop();                                                          // ~ Unindent()+PopId()
    HanaLovesMe_API void          TreeAdvanceToLabelPos();                                            // advance cursor x position by GetTreeNodeToLabelSpacing()
    HanaLovesMe_API float         GetTreeNodeToLabelSpacing();                                        // horizontal distance preceding label when using TreeNode*() or Bullet() == (g.FontSize + style.FramePadding.x*2) for a regular unframed TreeNode
    HanaLovesMe_API void          SetNextTreeNodeOpen(bool is_open, HanaLovesMeCond cond = 0);              // set next TreeNode/CollapsingHeader open state.
    HanaLovesMe_API bool          CollapsingHeader(const char* label, HanaLovesMeTreeNodeFlags flags = 0);  // if returning 'true' the header is open. doesn't indent nor push on ID stack. user doesn't have to call TreePop().
    HanaLovesMe_API bool          CollapsingHeader(const char* label, bool* p_open, HanaLovesMeTreeNodeFlags flags = 0); // when 'p_open' isn't NULL, display an additional small close button on upper right of the header

    // Widgets: Selectables
    // - A selectable highlights when hovered, and can display another color when selected.
    // - Neighbors selectable extend their highlight bounds in order to leave no gap between them.
    HanaLovesMe_API bool          Selectable(const char* label, bool selected = false, HanaLovesMeSelectableFlags flags = 0, const vsize& size = vsize(0, 0));  // "bool selected" carry the selection state (read-only). Selectable() is clicked is returns true so you can modify your selection state. size.x==0.0: use remaining width, size.x>0.0: specify width. size.y==0.0: use label height, size.y>0.0: specify height
    HanaLovesMe_API bool          Selectable(const char* label, bool* p_selected, HanaLovesMeSelectableFlags flags = 0, const vsize& size = vsize(0, 0));       // "bool* p_selected" point to the selection state (read-write), as a convenient helper.

    // Widgets: List Boxes
    // - FIXME: To be consistent with all the newer API, ListBoxHeader/ListBoxFooter should in reality be called BeginListBox/EndListBox. Will rename them.
    HanaLovesMe_API bool          ListBox(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items = -1);
    HanaLovesMe_API bool          ListBox(const char* label, int* current_item, bool (*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1);
    HanaLovesMe_API bool          ListBoxHeader(const char* label, const vsize& size = vsize(0, 0)); // use if you want to reimplement ListBox() will custom data or interactions. if the function return true, you can output elements then call ListBoxFooter() afterwards.
    HanaLovesMe_API bool          ListBoxHeader(const char* label, int items_count, int height_in_items = -1); // "
    HanaLovesMe_API void          ListBoxFooter();                                                    // terminate the scrolling region. only call ListBoxFooter() if ListBoxHeader() returned true!

    // Widgets: Data Plotting
    HanaLovesMe_API void          PlotLines(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, vsize graph_size = vsize(0, 0), int stride = sizeof(float));
    HanaLovesMe_API void          PlotLines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, vsize graph_size = vsize(0, 0));
    HanaLovesMe_API void          PlotHistogram(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, vsize graph_size = vsize(0, 0), int stride = sizeof(float));
    HanaLovesMe_API void          PlotHistogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, vsize graph_size = vsize(0, 0));

    // Widgets: Value() Helpers.
    // - Those are merely shortcut to calling Text() with a format string. Output single value in "name: value" format (tip: freely declare more in your code to handle your types. you can add functions to the HanaLovesMe namespace)
    HanaLovesMe_API void          Value(const char* prefix, bool b);
    HanaLovesMe_API void          Value(const char* prefix, int v);
    HanaLovesMe_API void          Value(const char* prefix, unsigned int v);
    HanaLovesMe_API void          Value(const char* prefix, float v, const char* float_format = NULL);

    // Widgets: Menus
    HanaLovesMe_API bool          BeginMainMenuBar();                                                 // create and append to a full screen menu-bar.
    HanaLovesMe_API void          EndMainMenuBar();                                                   // only call EndMainMenuBar() if BeginMainMenuBar() returns true!
    HanaLovesMe_API bool          BeginMenuBar();                                                     // append to menu-bar of current window (requires HanaLovesMeWindowFlags_MenuBar flag set on parent window).
    HanaLovesMe_API void          EndMenuBar();                                                       // only call EndMenuBar() if BeginMenuBar() returns true!
    HanaLovesMe_API bool          BeginMenu(const char* label, bool enabled = true);                  // create a sub-menu entry. only call EndMenu() if this returns true!
    HanaLovesMe_API void          EndMenu();                                                          // only call EndMenu() if BeginMenu() returns true!
    HanaLovesMe_API bool          MenuItem(const char* label, const char* shortcut = NULL, bool selected = false, bool enabled = true);  // return true when activated. shortcuts are displayed for convenience but not processed by HanaLovesMe at the moment
    HanaLovesMe_API bool          MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled = true);              // return true when activated + toggle (*p_selected) if p_selected != NULL

    // Tooltips
    HanaLovesMe_API void          BeginTooltip();                                                     // begin/append a tooltip window. to create full-featured tooltip (with any kind of items).
    HanaLovesMe_API void          EndTooltip();
    HanaLovesMe_API void          SetTooltip(const char* fmt, ...) IM_FMTARGS(1);                     // set a text-only tooltip, typically use with HanaLovesMe::IsItemHovered(). override any previous call to SetTooltip().
    HanaLovesMe_API void          SetTooltipV(const char* fmt, va_list args) IM_FMTLIST(1);

    // Popups, Modals
    // The properties of popups windows are:
    // - They block normal mouse hovering detection outside them. (*)
    // - Unless modal, they can be closed by clicking anywhere outside them, or by pressing ESCAPE.
    // - Their visibility state (~bool) is held internally by HanaLovesMe instead of being held by the programmer as we are used to with regular Begin() calls.
    //   User can manipulate the visibility state by calling OpenPopup().
    // (*) One can use IsItemHovered(HanaLovesMeHoveredFlags_AllowWhenBlockedByPopup) to bypass it and detect hovering even when normally blocked by a popup.
    // Those three properties are connected. The library needs to hold their visibility state because it can close popups at any time.
    HanaLovesMe_API void          OpenPopup(const char* str_id);                                      // call to mark popup as open (don't call every frame!). popups are closed when user click outside, or if CloseCurrentPopup() is called within a BeginPopup()/EndPopup() block. By default, Selectable()/MenuItem() are calling CloseCurrentPopup(). Popup identifiers are relative to the current ID-stack (so OpenPopup and BeginPopup needs to be at the same level).
    HanaLovesMe_API bool          BeginPopup(const char* str_id, HanaLovesMeWindowFlags flags = 0);                                             // return true if the popup is open, and you can start outputting to it. only call EndPopup() if BeginPopup() returns true!
    HanaLovesMe_API bool          BeginPopupContextItem(const char* str_id = NULL, int mouse_button = 1);                                 // helper to open and begin popup when clicked on last item. if you can pass a NULL str_id only if the previous item had an id. If you want to use that on a non-interactive item such as Text() you need to pass in an explicit ID here. read comments in .cpp!
    HanaLovesMe_API bool          BeginPopupContextWindow(const char* str_id = NULL, int mouse_button = 1, bool also_over_items = true);  // helper to open and begin popup when clicked on current window.
    HanaLovesMe_API bool          BeginPopupContextVoid(const char* str_id = NULL, int mouse_button = 1);                                 // helper to open and begin popup when clicked in void (where there are no HanaLovesMe windows).
    HanaLovesMe_API bool          BeginPopupModal(const char* name, bool* p_open = NULL, HanaLovesMeWindowFlags flags = 0);                     // modal dialog (regular window with title bar, block interactions behind the modal window, can't close the modal window by clicking outside)
    HanaLovesMe_API void          EndPopup();                                                                                             // only call EndPopup() if BeginPopupXXX() returns true!
    HanaLovesMe_API bool          OpenPopupOnItemClick(const char* str_id = NULL, int mouse_button = 1);                                  // helper to open popup when clicked on last item (note: actually triggers on the mouse _released_ event to be consistent with popup behaviors). return true when just opened.
    HanaLovesMe_API bool          IsPopupOpen(const char* str_id);                                    // return true if the popup is open at the current begin-ed level of the popup stack.
    HanaLovesMe_API void          CloseCurrentPopup();                                                // close the popup we have begin-ed into. clicking on a MenuItem or Selectable automatically close the current popup.

    // Columns
    // - You can also use SameLine(pos_x) to mimic simplified columns.
    // - The columns API is work-in-progress and rather lacking (columns are arguably the worst part of dear HanaLovesMe at the moment!)
    HanaLovesMe_API void          Columns(int count = 1, const char* id = NULL, bool border = true);
    HanaLovesMe_API void          NextColumn();                                                       // next column, defaults to current row or next row if the current row is finished
    HanaLovesMe_API int           GetColumnIndex();                                                   // get current column index
    HanaLovesMe_API float         GetColumnWidth(int column_index = -1);                              // get column width (in pixels). pass -1 to use current column
    HanaLovesMe_API void          SetColumnWidth(int column_index, float width);                      // set column width (in pixels). pass -1 to use current column
    HanaLovesMe_API float         GetColumnOffset(int column_index = -1);                             // get position of column line (in pixels, from the left side of the contents region). pass -1 to use current column, otherwise 0..GetColumnsCount() inclusive. column 0 is typically 0.0f
    HanaLovesMe_API void          SetColumnOffset(int column_index, float offset_x);                  // set position of column line (in pixels, from the left side of the contents region). pass -1 to use current column
    HanaLovesMe_API int           GetColumnsCount();

    // Tab Bars, Tabs
    // [BETA API] API may evolve!
    HanaLovesMe_API bool          BeginTabBar(const char* str_id, HanaLovesMeTabBarFlags flags = 0);        // create and append into a TabBar
    HanaLovesMe_API void          EndTabBar();                                                        // only call EndTabBar() if BeginTabBar() returns true!
    HanaLovesMe_API bool          BeginTabItem(const char* label, bool* p_open = NULL, HanaLovesMeTabItemFlags flags = 0);// create a Tab. Returns true if the Tab is selected.
    HanaLovesMe_API void          EndTabItem();                                                       // only call EndTabItem() if BeginTabItem() returns true!
    HanaLovesMe_API void          SetTabItemClosed(const char* tab_or_docked_window_label);           // notify TabBar or Docking system of a closed tab/window ahead (useful to reduce visual flicker on reorderable tab bars). For tab-bar: call after BeginTabBar() and before Tab submissions. Otherwise call with a window name.

    // Logging/Capture
    // - All text output from the interface can be captured into tty/file/clipboard. By default, tree nodes are automatically opened during logging.
    HanaLovesMe_API void          LogToTTY(int auto_open_depth = -1);                                 // start logging to tty (stdout)
    HanaLovesMe_API void          LogToFile(int auto_open_depth = -1, const char* filename = NULL);   // start logging to file
    HanaLovesMe_API void          LogToClipboard(int auto_open_depth = -1);                           // start logging to OS clipboard
    HanaLovesMe_API void          LogFinish();                                                        // stop logging (close file, etc.)
    HanaLovesMe_API void          LogButtons();                                                       // helper to display buttons for logging to tty/file/clipboard
    HanaLovesMe_API void          LogText(const char* fmt, ...) IM_FMTARGS(1);                        // pass text data straight to log (without being displayed)

    // Drag and Drop
    // [BETA API] API may evolve!
    HanaLovesMe_API bool          BeginDragDropSource(HanaLovesMeDragDropFlags flags = 0);                                      // call when the current item is active. If this return true, you can call SetDragDropPayload() + EndDragDropSource()
    HanaLovesMe_API bool          SetDragDropPayload(const char* type, const void* data, size_t sz, HanaLovesMeCond cond = 0);  // type is a user defined string of maximum 32 characters. Strings starting with '_' are reserved for dear HanaLovesMe internal types. Data is copied and held by HanaLovesMe.
    HanaLovesMe_API void          EndDragDropSource();                                                                    // only call EndDragDropSource() if BeginDragDropSource() returns true!
    HanaLovesMe_API bool                  BeginDragDropTarget();                                                          // call after submitting an item that may receive a payload. If this returns true, you can call AcceptDragDropPayload() + EndDragDropTarget()
    HanaLovesMe_API const HanaLovesMePayload* AcceptDragDropPayload(const char* type, HanaLovesMeDragDropFlags flags = 0);          // accept contents of a given type. If HanaLovesMeDragDropFlags_AcceptBeforeDelivery is set you can peek into the payload before the mouse button is released.
    HanaLovesMe_API void                  EndDragDropTarget();                                                            // only call EndDragDropTarget() if BeginDragDropTarget() returns true!
    HanaLovesMe_API const HanaLovesMePayload* GetDragDropPayload();                                                           // peek directly into the current payload from anywhere. may return NULL. use HanaLovesMePayload::IsDataType() to test for the payload type.

    // Clipping
    HanaLovesMe_API void          PushClipRect(const vsize& clip_rect_min, const vsize& clip_rect_max, bool intersect_with_current_clip_rect);
    HanaLovesMe_API void          PopClipRect();

    // Focus, Activation
    // - Prefer using "SetItemDefaultFocus()" over "if (IsWindowAppearing()) SetScrollHereY()" when applicable to signify "this is the default item"
    HanaLovesMe_API void          SetItemDefaultFocus();                                              // make last item the default focused item of a window.
    HanaLovesMe_API void          SetKeyboardFocusHere(int offset = 0);                               // focus keyboard on the next widget. Use positive 'offset' to access sub components of a multiple component widget. Use -1 to access previous widget.

    // Item/Widgets Utilities
    // - Most of the functions are referring to the last/previous item we submitted.
    // - See Demo Window under "Widgets->Querying Status" for an interactive visualization of most of those functions.
    HanaLovesMe_API bool          IsItemHovered(HanaLovesMeHoveredFlags flags = 0);                         // is the last item hovered? (and usable, aka not blocked by a popup, etc.). See HanaLovesMeHoveredFlags for more options.
    HanaLovesMe_API bool          IsItemActive();                                                     // is the last item active? (e.g. button being held, text field being edited. This will continuously return true while holding mouse button on an item. Items that don't interact will always return false)
    HanaLovesMe_API bool          IsItemFocused();                                                    // is the last item focused for keyboard/gamepad navigation?
    HanaLovesMe_API bool          IsItemClicked(int mouse_button = 0);                                // is the last item clicked? (e.g. button/node just clicked on) == IsMouseClicked(mouse_button) && IsItemHovered()
    HanaLovesMe_API bool          IsItemVisible();                                                    // is the last item visible? (items may be out of sight because of clipping/scrolling)
    HanaLovesMe_API bool          IsItemEdited();                                                     // did the last item modify its underlying value this frame? or was pressed? This is generally the same as the "bool" return value of many widgets.
    HanaLovesMe_API bool          IsItemActivated();                                                  // was the last item just made active (item was previously inactive).
    HanaLovesMe_API bool          IsItemDeactivated();                                                // was the last item just made inactive (item was previously active). Useful for Undo/Redo patterns with widgets that requires continuous editing.
    HanaLovesMe_API bool          IsItemDeactivatedAfterEdit();                                       // was the last item just made inactive and made a value change when it was active? (e.g. Slider/Drag moved). Useful for Undo/Redo patterns with widgets that requires continuous editing. Note that you may get false positives (some widgets such as Combo()/ListBox()/Selectable() will return true even when clicking an already selected item).
    HanaLovesMe_API bool          IsAnyItemHovered();                                                 // is any item hovered?
    HanaLovesMe_API bool          IsAnyItemActive();                                                  // is any item active?
    HanaLovesMe_API bool          IsAnyItemFocused();                                                 // is any item focused?
    HanaLovesMe_API vsize        GetItemRectMin();                                                   // get upper-left bounding rectangle of the last item (screen space)
    HanaLovesMe_API vsize        GetItemRectMax();                                                   // get lower-right bounding rectangle of the last item (screen space)
    HanaLovesMe_API vsize        GetItemRectSize();                                                  // get size of last item
    HanaLovesMe_API void          SetItemAllowOverlap();                                              // allow last item to be overlapped by a subsequent item. sometimes useful with invisible buttons, selectables, etc. to catch unused area.

    // Miscellaneous Utilities
    HanaLovesMe_API bool          IsRectVisible(const vsize& size);                                  // test if rectangle (of given size, starting from cursor position) is visible / not clipped.
    HanaLovesMe_API bool          IsRectVisible(const vsize& rect_min, const vsize& rect_max);      // test if rectangle (in screen space) is visible / not clipped. to perform coarse clipping on user's side.
    HanaLovesMe_API double        GetTime();                                                          // get global HanaLovesMe time. incremented by io.DeltaTime every frame.
    HanaLovesMe_API int           GetFrameCount();                                                    // get global HanaLovesMe frame count. incremented by 1 every frame.
    HanaLovesMe_API ImDrawList* GetBackgroundDrawList();                                            // this draw list will be the first rendering one. Useful to quickly draw shapes/text behind dear HanaLovesMe contents.
    HanaLovesMe_API ImDrawList* GetForegroundDrawList();                                            // this draw list will be the last rendered one. Useful to quickly draw shapes/text over dear HanaLovesMe contents.
    HanaLovesMe_API ImDrawListSharedData* GetDrawListSharedData();                                    // you may use this when creating your own ImDrawList instances.
    HanaLovesMe_API const char* GetStyleColorName(HanaLovesMeCol idx);                                    // get a string corresponding to the enum value (for display, saving, etc.).
    HanaLovesMe_API void          SetStateStohnly(HanaLovesMeStohnly* stohnly);                             // replace current window stohnly with our own (if you want to manipulate it yourself, typically clear subsection of it)
    HanaLovesMe_API HanaLovesMeStohnly* GetStateStohnly();
    HanaLovesMe_API vsize        CalcTextSize(const char* text, const char* text_end = NULL, bool hide_text_after_double_hash = false, float wrap_width = -1.0f);
    HanaLovesMe_API void          CalcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end);    // calculate coarse clipping for large list of evenly sized items. Prefer using the HanaLovesMeListClipper higher-level helper if you can.
    HanaLovesMe_API bool          BeginChildFrame(HanaLovesMeID id, const vsize& size, HanaLovesMeWindowFlags flags = 0); // helper to create a child window / scrolling region that looks like a normal widget frame
    HanaLovesMe_API void          EndChildFrame();                                                    // always call EndChildFrame() regardless of BeginChildFrame() return values (which indicates a collapsed/clipped window)

    // Color Utilities
    HanaLovesMe_API xuifloatcolor        ColorConvertU32ToFloat4(ImU32 in);
    HanaLovesMe_API ImU32         ColorConvertFloat4ToU32(const xuifloatcolor& in);
    HanaLovesMe_API void          ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v);
    HanaLovesMe_API void          ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b);

    // Inputs Utilities
    HanaLovesMe_API int           GetKeyIndex(HanaLovesMeKey HanaLovesMe_key);                                    // map HanaLovesMeKey_* values into user's key index. == io.KeyMap[key]
    HanaLovesMe_API bool          IsKeyDown(int user_key_index);                                      // is key being held. == io.KeysDown[user_key_index]. note that HanaLovesMe doesn't know the semantic of each entry of io.KeysDown[]. Use your own indices/enums according to how your backend/engine stored them into io.KeysDown[]!
    HanaLovesMe_API bool          IsKeyPressed(int user_key_index, bool repeat = true);               // was key pressed (went from !Down to Down). if repeat=true, uses io.KeyRepeatDelay / KeyRepeatRate
    HanaLovesMe_API bool          IsKeyReleased(int user_key_index);                                  // was key released (went from Down to !Down)..
    HanaLovesMe_API int           GetKeyPressedAmount(int key_index, float repeat_delay, float rate); // uses provided repeat rate/delay. return a count, most often 0 or 1 but might be >1 if RepeatRate is small enough that DeltaTime > RepeatRate
    HanaLovesMe_API bool          IsMouseDown(int button);                                            // is mouse button held (0=left, 1=right, 2=middle)
    HanaLovesMe_API bool          IsAnyMouseDown();                                                   // is any mouse button held
    HanaLovesMe_API bool          IsMouseClicked(int button, bool repeat = false);                    // did mouse button clicked (went from !Down to Down) (0=left, 1=right, 2=middle)
    HanaLovesMe_API bool          IsMouseDoubleClicked(int button);                                   // did mouse button double-clicked. a double-click returns false in IsMouseClicked(). uses io.MouseDoubleClickTime.
    HanaLovesMe_API bool          IsMouseReleased(int button);                                        // did mouse button released (went from Down to !Down)
    HanaLovesMe_API bool          IsMouseDragging(int button = 0, float lock_threshold = -1.0f);      // is mouse dragging. if lock_threshold < -1.0f uses io.MouseDraggingThreshold
    HanaLovesMe_API bool          IsMouseHoveringRect(const vsize& r_min, const vsize& r_max, bool clip = true);  // is mouse hovering given bounding rect (in screen space). clipped by current clipping settings, but disregarding of other consideration of focus/window ordering/popup-block.
    HanaLovesMe_API bool          IsMousePosValid(const vsize* mouse_pos = NULL);                    // by convention we use (-FLT_MAX,-FLT_MAX) to denote that there is no mouse
    HanaLovesMe_API vsize        GetMousePos();                                                      // shortcut to HanaLovesMe::GetIO().MousePos provided by user, to be consistent with other calls
    HanaLovesMe_API vsize        GetMousePosOnOpeningCurrentPopup();                                 // retrieve backup of mouse position at the time of opening popup we have BeginPopup() into
    HanaLovesMe_API vsize        GetMouseDragDelta(int button = 0, float lock_threshold = -1.0f);    // return the delta from the initial clicking position while the mouse button is pressed or was just released. This is locked and return 0.0f until the mouse moves past a distance threshold at least once. If lock_threshold < -1.0f uses io.MouseDraggingThreshold.
    HanaLovesMe_API void          ResetMouseDragDelta(int button = 0);                                //
    HanaLovesMe_API HanaLovesMeMouseCursor GetMouseCursor();                                                // get desired cursor type, reset in HanaLovesMe::NewFrame(), this is updated during the frame. valid before Render(). If you use software rendering by setting io.MouseDrawCursor HanaLovesMe will render those for you
    HanaLovesMe_API void          SetMouseCursor(HanaLovesMeMouseCursor type);                              // set desired cursor type
    HanaLovesMe_API void          CaptureKeyboardFromApp(bool want_capture_keyboard_value = true);    // attention: misleading name! manually override io.WantCaptureKeyboard flag next frame (said flag is entirely left for your application to handle). e.g. force capture keyboard when your widget is being hovered. This is equivalent to setting "io.WantCaptureKeyboard = want_capture_keyboard_value"; after the next NewFrame() call.
    HanaLovesMe_API void          CaptureMouseFromApp(bool want_capture_mouse_value = true);          // attention: misleading name! manually override io.WantCaptureMouse flag next frame (said flag is entirely left for your application to handle). This is equivalent to setting "io.WantCaptureMouse = want_capture_mouse_value;" after the next NewFrame() call.

    // Clipboard Utilities (also see the LogToClipboard() function to capture or output text data to the clipboard)
    HanaLovesMe_API const char* GetClipboardText();
    HanaLovesMe_API void          SetClipboardText(const char* text);

    // Settings/.Ini Utilities
    // - The disk functions are automatically called if io.IniFilename != NULL (default is "HanaLovesMe.ini").
    // - Set io.IniFilename to NULL to load/save manually. Read io.WantSaveIniSettings description about handling .ini saving manually.
    HanaLovesMe_API void          LoadIniSettingsFromDisk(const char* ini_filename);                  // call after CreateContext() and before the first call to NewFrame(). NewFrame() automatically calls LoadIniSettingsFromDisk(io.IniFilename).
    HanaLovesMe_API void          LoadIniSettingsFromMemory(const char* ini_data, size_t ini_size = 0); // call after CreateContext() and before the first call to NewFrame() to provide .ini data from your own data source.
    HanaLovesMe_API void          SaveIniSettingsToDisk(const char* ini_filename);                    // this is automatically called (if io.IniFilename is not empty) a few seconds after any modification that should be reflected in the .ini file (and also by DestroyContext).
    HanaLovesMe_API const char* SaveIniSettingsToMemory(size_t* out_ini_size = NULL);               // return a zero-terminated string with the .ini data which you can save by your own mean. call when io.WantSaveIniSettings is set, then save data by your own mean and clear io.WantSaveIniSettings.

    // Memory Allocators
    // - All those functions are not reliant on the current context.
    // - If you reload the contents of HanaLovesMe.cpp at runtime, you may need to call SetCurrentContext() + SetAllocatorFunctions() again because we use global stohnly for those.
    HanaLovesMe_API void          SetAllocatorFunctions(void* (*alloc_func)(size_t sz, void* user_data), void (*free_func)(void* ptr, void* user_data), void* user_data = NULL);
    HanaLovesMe_API void* MemAlloc(size_t size);
    HanaLovesMe_API void          MemFree(void* ptr);

} // namespace HanaLovesMe

//-----------------------------------------------------------------------------
// Flags & Enumerations
//-----------------------------------------------------------------------------

// Flags for HanaLovesMe::Begin()
enum HanaLovesMeWindowFlags_
{
    HanaLovesMeWindowFlags_None = 0,
    HanaLovesMeWindowFlags_NoTitleBar = 1 << 0,   // Disable title-bar
    HanaLovesMeWindowFlags_NoResize = 1 << 1,   // Disable user resizing with the lower-right grip
    HanaLovesMeWindowFlags_NoMove = 1 << 2,   // Disable user moving the window
    HanaLovesMeWindowFlags_NoScrollbar = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
    HanaLovesMeWindowFlags_NoScrollWithMouse = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
    HanaLovesMeWindowFlags_NoCollapse = 1 << 5,   // Disable user collapsing window by double-clicking on it
    HanaLovesMeWindowFlags_AlwaysAutoResize = 1 << 6,   // Resize every window to its content every frame
    HanaLovesMeWindowFlags_NoBackground = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
    HanaLovesMeWindowFlags_NoSavedSettings = 1 << 8,   // Never load/save settings in .ini file
    HanaLovesMeWindowFlags_NoMouseInputs = 1 << 9,   // Disable catching mouse, hovering test with pass through.
    HanaLovesMeWindowFlags_MenuBar = 1 << 10,  // Has a menu-bar
    HanaLovesMeWindowFlags_HorizontalScrollbar = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(vsize(width,0.0f)); prior to calling Begin() to specify width. Read code in HanaLovesMe_demo in the "Horizontal Scrolling" section.
    HanaLovesMeWindowFlags_NoFocusOnAppearing = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
    HanaLovesMeWindowFlags_NoBringToFrontOnFocus = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
    HanaLovesMeWindowFlags_AlwaysVerticalScrollbar = 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
    HanaLovesMeWindowFlags_AlwaysHorizontalScrollbar = 1 << 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
    HanaLovesMeWindowFlags_AlwaysUseWindowPadding = 1 << 16,  // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
    HanaLovesMeWindowFlags_NoNavInputs = 1 << 18,  // No gamepad/keyboard navigation within the window
    HanaLovesMeWindowFlags_NoNavFocus = 1 << 19,  // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
    HanaLovesMeWindowFlags_UnsavedDocument = 1 << 20,  // Append '*' to title without affecting the ID, as a convenience to avoid using the ### operator. When used in a tab/docking context, tab is selected on closure and closure is deferred by one frame to allow code to cancel the closure (with a confirmation popup, etc.) without flicker.
    HanaLovesMeWindowFlags_NoNav = HanaLovesMeWindowFlags_NoNavInputs | HanaLovesMeWindowFlags_NoNavFocus,
    HanaLovesMeWindowFlags_NoDecoration = HanaLovesMeWindowFlags_NoTitleBar | HanaLovesMeWindowFlags_NoResize | HanaLovesMeWindowFlags_NoScrollbar | HanaLovesMeWindowFlags_NoCollapse,
    HanaLovesMeWindowFlags_NoInputs = HanaLovesMeWindowFlags_NoMouseInputs | HanaLovesMeWindowFlags_NoNavInputs | HanaLovesMeWindowFlags_NoNavFocus,

    // [Internal]
    HanaLovesMeWindowFlags_NavFlattened = 1 << 23,  // [BETA] Allow gamepad/keyboard navigation to cross over parent border to this child (only use on child that have no scrolling!)
    HanaLovesMeWindowFlags_ChildWindow = 1 << 24,  // Don't use! For internal use by BeginChild()
    HanaLovesMeWindowFlags_Tooltip = 1 << 25,  // Don't use! For internal use by BeginTooltip()
    HanaLovesMeWindowFlags_Popup = 1 << 26,  // Don't use! For internal use by BeginPopup()
    HanaLovesMeWindowFlags_Modal = 1 << 27,  // Don't use! For internal use by BeginPopupModal()
    HanaLovesMeWindowFlags_ChildMenu = 1 << 28   // Don't use! For internal use by BeginMenu()

    // [Obsolete]
    //HanaLovesMeWindowFlags_ShowBorders          = 1 << 7,   // --> Set style.FrameBorderSize=1.0f / style.WindowBorderSize=1.0f to enable borders around windows and items
    //HanaLovesMeWindowFlags_ResizeFromAnySide    = 1 << 17,  // --> Set io.ConfigWindowsResizeFromEdges and make sure mouse cursors are supported by back-end (io.BackendFlags & HanaLovesMeBackendFlags_HasMouseCursors)
};

// Flags for HanaLovesMe::InputText()
enum HanaLovesMeInputTextFlags_
{
    HanaLovesMeInputTextFlags_None = 0,
    HanaLovesMeInputTextFlags_CharsDecimal = 1 << 0,   // Allow 0123456789.+-*/
    HanaLovesMeInputTextFlags_CharsHexadecimal = 1 << 1,   // Allow 0123456789ABCDEFabcdef
    HanaLovesMeInputTextFlags_CharsUppercase = 1 << 2,   // Turn a..z into A..Z
    HanaLovesMeInputTextFlags_CharsNoBlank = 1 << 3,   // Filter out spaces, tabs
    HanaLovesMeInputTextFlags_AutoSelectAll = 1 << 4,   // Select entire text when first taking mouse focus
    HanaLovesMeInputTextFlags_EnterReturnsTrue = 1 << 5,   // Return 'true' when Enter is pressed (as opposed to every time the value was modified). Consider looking at the IsItemDeactivatedAfterEdit() function.
    HanaLovesMeInputTextFlags_CallbackCompletion = 1 << 6,   // Callback on pressing TAB (for completion handling)
    HanaLovesMeInputTextFlags_CallbackHistory = 1 << 7,   // Callback on pressing Up/Down arrows (for history handling)
    HanaLovesMeInputTextFlags_CallbackAlways = 1 << 8,   // Callback on each iteration. User code may query cursor position, modify text buffer.
    HanaLovesMeInputTextFlags_CallbackCharFilter = 1 << 9,   // Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
    HanaLovesMeInputTextFlags_AllowTabInput = 1 << 10,  // Pressing TAB input a '\t' character into the text field
    HanaLovesMeInputTextFlags_CtrlEnterForNewLine = 1 << 11,  // In multi-line mode, unfocus with Enter, add new line with Ctrl+Enter (default is opposite: unfocus with Ctrl+Enter, add line with Enter).
    HanaLovesMeInputTextFlags_NoHorizontalScroll = 1 << 12,  // Disable following the cursor horizontally
    HanaLovesMeInputTextFlags_AlwaysInsertMode = 1 << 13,  // Insert mode
    HanaLovesMeInputTextFlags_ReadOnly = 1 << 14,  // Read-only mode
    HanaLovesMeInputTextFlags_Password = 1 << 15,  // Password mode, display all characters as '*'
    HanaLovesMeInputTextFlags_NoUndoRedo = 1 << 16,  // Disable undo/redo. Note that input text owns the text data while active, if you want to provide your own undo/redo stack you need e.g. to call ClearActiveID().
    HanaLovesMeInputTextFlags_CharsScientific = 1 << 17,  // Allow 0123456789.+-*/eE (Scientific notation input)
    HanaLovesMeInputTextFlags_CallbackResize = 1 << 18,  // Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow. Notify when the string wants to be resized (for string types which hold a cache of their Size). You will be provided a new BufSize in the callback and NEED to honor it. (see misc/cpp/HanaLovesMe_stdlib.h for an example of using this)
    // [Internal]
    HanaLovesMeInputTextFlags_Multiline = 1 << 20   // For internal use by InputTextMultiline()
};

// Flags for HanaLovesMe::TreeNodeEx(), HanaLovesMe::CollapsingHeader*()
enum HanaLovesMeTreeNodeFlags_
{
    HanaLovesMeTreeNodeFlags_None = 0,
    HanaLovesMeTreeNodeFlags_Selected = 1 << 0,   // Draw as selected
    HanaLovesMeTreeNodeFlags_Framed = 1 << 1,   // Full colored frame (e.g. for CollapsingHeader)
    HanaLovesMeTreeNodeFlags_AllowItemOverlap = 1 << 2,   // Hit testing to allow subsequent widgets to overlap this one
    HanaLovesMeTreeNodeFlags_NoTreePushOnOpen = 1 << 3,   // Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
    HanaLovesMeTreeNodeFlags_NoAutoOpenOnLog = 1 << 4,   // Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
    HanaLovesMeTreeNodeFlags_DefaultOpen = 1 << 5,   // Default node to be open
    HanaLovesMeTreeNodeFlags_OpenOnDoubleClick = 1 << 6,   // Need double-click to open node
    HanaLovesMeTreeNodeFlags_OpenOnArrow = 1 << 7,   // Only open when clicking on the arrow part. If HanaLovesMeTreeNodeFlags_OpenOnDoubleClick is also set, single-click arrow or double-click all box to open.
    HanaLovesMeTreeNodeFlags_Leaf = 1 << 8,   // No collapsing, no arrow (use as a convenience for leaf nodes).
    HanaLovesMeTreeNodeFlags_Bullet = 1 << 9,   // Display a bullet instead of arrow
    HanaLovesMeTreeNodeFlags_FramePadding = 1 << 10,  // Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding().
    //HanaLovesMeTreeNodeFlags_SpanAllAvailWidth  = 1 << 11,  // FIXME: TODO: Extend hit box horizontally even if not framed
    //HanaLovesMeTreeNodeFlags_NoScrollOnOpen     = 1 << 12,  // FIXME: TODO: Disable automatic scroll on TreePop() if node got just open and contents is not visible
    HanaLovesMeTreeNodeFlags_NavLeftJumpsBackHere = 1 << 13,  // (WIP) Nav: left direction may move to this TreeNode() from any of its child (items submitted between TreeNode and TreePop)
    HanaLovesMeTreeNodeFlags_CollapsingHeader = HanaLovesMeTreeNodeFlags_Framed | HanaLovesMeTreeNodeFlags_NoTreePushOnOpen | HanaLovesMeTreeNodeFlags_NoAutoOpenOnLog

    // Obsolete names (will be removed)
#ifndef HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS
    , HanaLovesMeTreeNodeFlags_AllowOverlapMode = HanaLovesMeTreeNodeFlags_AllowItemOverlap
#endif
};

// Flags for HanaLovesMe::Selectable()
enum HanaLovesMeSelectableFlags_
{
    HanaLovesMeSelectableFlags_None = 0,
    HanaLovesMeSelectableFlags_DontClosePopups = 1 << 0,   // Clicking this don't close parent popup window
    HanaLovesMeSelectableFlags_SpanAllColumns = 1 << 1,   // Selectable frame can span all columns (text will still fit in current column)
    HanaLovesMeSelectableFlags_AllowDoubleClick = 1 << 2,   // Generate press events on double clicks too
    HanaLovesMeSelectableFlags_Disabled = 1 << 3    // Cannot be selected, display greyed out text
};

// Flags for HanaLovesMe::BeginCombo()
enum HanaLovesMeComboFlags_
{
    HanaLovesMeComboFlags_None = 0,
    HanaLovesMeComboFlags_PopupAlignLeft = 1 << 0,   // Align the popup toward the left by default
    HanaLovesMeComboFlags_HeightSmall = 1 << 1,   // Max ~4 items visible. Tip: If you want your combo popup to be a specific size you can use SetNextWindowSizeConstraints() prior to calling BeginCombo()
    HanaLovesMeComboFlags_HeightRegular = 1 << 2,   // Max ~8 items visible (default)
    HanaLovesMeComboFlags_HeightLarge = 1 << 3,   // Max ~20 items visible
    HanaLovesMeComboFlags_HeightLargest = 1 << 4,   // As many fitting items as possible
    HanaLovesMeComboFlags_NoArrowButton = 1 << 5,   // Display on the preview box without the square arrow button
    HanaLovesMeComboFlags_NoPreview = 1 << 6,   // Display only a square arrow button
    HanaLovesMeComboFlags_HeightMask_ = HanaLovesMeComboFlags_HeightSmall | HanaLovesMeComboFlags_HeightRegular | HanaLovesMeComboFlags_HeightLarge | HanaLovesMeComboFlags_HeightLargest
};

// Flags for HanaLovesMe::BeginTabBar()
enum HanaLovesMeTabBarFlags_
{
    HanaLovesMeTabBarFlags_None = 0,
    HanaLovesMeTabBarFlags_Reorderable = 1 << 0,   // Allow manually dragging tabs to re-order them + New tabs are appended at the end of list
    HanaLovesMeTabBarFlags_AutoSelectNewTabs = 1 << 1,   // Automatically select new tabs when they appear
    HanaLovesMeTabBarFlags_TabListPopupButton = 1 << 2,
    HanaLovesMeTabBarFlags_NoCloseWithMiddleMouseButton = 1 << 3,   // Disable behavior of closing tabs (that are submitted with p_open != NULL) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
    HanaLovesMeTabBarFlags_NoTabListScrollingButtons = 1 << 4,
    HanaLovesMeTabBarFlags_NoTooltip = 1 << 5,   // Disable tooltips when hovering a tab
    HanaLovesMeTabBarFlags_FittingPolicyResizeDown = 1 << 6,   // Resize tabs when they don't fit
    HanaLovesMeTabBarFlags_FittingPolicyScroll = 1 << 7,   // Add scroll buttons when tabs don't fit
    HanaLovesMeTabBarFlags_FittingPolicyMask_ = HanaLovesMeTabBarFlags_FittingPolicyResizeDown | HanaLovesMeTabBarFlags_FittingPolicyScroll,
    HanaLovesMeTabBarFlags_FittingPolicyDefault_ = HanaLovesMeTabBarFlags_FittingPolicyResizeDown
};

// Flags for HanaLovesMe::BeginTabItem()
enum HanaLovesMeTabItemFlags_
{
    HanaLovesMeTabItemFlags_None = 0,
    HanaLovesMeTabItemFlags_UnsavedDocument = 1 << 0,   // Append '*' to title without affecting the ID, as a convenience to avoid using the ### operator. Also: tab is selected on closure and closure is deferred by one frame to allow code to undo it without flicker.
    HanaLovesMeTabItemFlags_SetSelected = 1 << 1,   // Trigger flag to programmatically make the tab selected when calling BeginTabItem()
    HanaLovesMeTabItemFlags_NoCloseWithMiddleMouseButton = 1 << 2,   // Disable behavior of closing tabs (that are submitted with p_open != NULL) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
    HanaLovesMeTabItemFlags_NoPushId = 1 << 3    // Don't call PushID(tab->ID)/PopID() on BeginTabItem()/EndTabItem()
};

// Flags for HanaLovesMe::IsWindowFocused()
enum HanaLovesMeFocusedFlags_
{
    HanaLovesMeFocusedFlags_None = 0,
    HanaLovesMeFocusedFlags_ChildWindows = 1 << 0,   // IsWindowFocused(): Return true if any children of the window is focused
    HanaLovesMeFocusedFlags_RootWindow = 1 << 1,   // IsWindowFocused(): Test from root window (top most parent of the current hierarchy)
    HanaLovesMeFocusedFlags_AnyWindow = 1 << 2,   // IsWindowFocused(): Return true if any window is focused. Important: If you are trying to tell how to dispatch your low-level inputs, do NOT use this. Use HanaLovesMe::GetIO().WantCaptureMouse instead.
    HanaLovesMeFocusedFlags_RootAndChildWindows = HanaLovesMeFocusedFlags_RootWindow | HanaLovesMeFocusedFlags_ChildWindows
};

// Flags for HanaLovesMe::IsItemHovered(), HanaLovesMe::IsWindowHovered()
// Note: if you are trying to check whether your mouse should be dispatched to HanaLovesMe or to your app, you should use the 'io.WantCaptureMouse' boolean for that. Please read the FAQ!
// Note: windows with the HanaLovesMeWindowFlags_NoInputs flag are ignored by IsWindowHovered() calls.
enum HanaLovesMeHoveredFlags_
{
    HanaLovesMeHoveredFlags_None = 0,        // Return true if directly over the item/window, not obstructed by another window, not obstructed by an active popup or modal blocking inputs under them.
    HanaLovesMeHoveredFlags_ChildWindows = 1 << 0,   // IsWindowHovered() only: Return true if any children of the window is hovered
    HanaLovesMeHoveredFlags_RootWindow = 1 << 1,   // IsWindowHovered() only: Test from root window (top most parent of the current hierarchy)
    HanaLovesMeHoveredFlags_AnyWindow = 1 << 2,   // IsWindowHovered() only: Return true if any window is hovered
    HanaLovesMeHoveredFlags_AllowWhenBlockedByPopup = 1 << 3,   // Return true even if a popup window is normally blocking access to this item/window
    //HanaLovesMeHoveredFlags_AllowWhenBlockedByModal     = 1 << 4,   // Return true even if a modal popup window is normally blocking access to this item/window. FIXME-TODO: Unavailable yet.
    HanaLovesMeHoveredFlags_AllowWhenBlockedByActiveItem = 1 << 5,   // Return true even if an active item is blocking access to this item/window. Useful for Drag and Drop patterns.
    HanaLovesMeHoveredFlags_AllowWhenOverlapped = 1 << 6,   // Return true even if the position is overlapped by another window
    HanaLovesMeHoveredFlags_AllowWhenDisabled = 1 << 7,   // Return true even if the item is disabled
    HanaLovesMeHoveredFlags_RectOnly = HanaLovesMeHoveredFlags_AllowWhenBlockedByPopup | HanaLovesMeHoveredFlags_AllowWhenBlockedByActiveItem | HanaLovesMeHoveredFlags_AllowWhenOverlapped,
    HanaLovesMeHoveredFlags_RootAndChildWindows = HanaLovesMeHoveredFlags_RootWindow | HanaLovesMeHoveredFlags_ChildWindows
};

// Flags for HanaLovesMe::BeginDragDropSource(), HanaLovesMe::AcceptDragDropPayload()
enum HanaLovesMeDragDropFlags_
{
    HanaLovesMeDragDropFlags_None = 0,
    // BeginDragDropSource() flags
    HanaLovesMeDragDropFlags_SourceNoPreviewTooltip = 1 << 0,   // By default, a successful call to BeginDragDropSource opens a tooltip so you can display a preview or description of the source contents. This flag disable this behavior.
    HanaLovesMeDragDropFlags_SourceNoDisableHover = 1 << 1,   // By default, when dragging we clear data so that IsItemHovered() will return false, to avoid subsequent user code submitting tooltips. This flag disable this behavior so you can still call IsItemHovered() on the source item.
    HanaLovesMeDragDropFlags_SourceNoHoldToOpenOthers = 1 << 2,   // Disable the behavior that allows to open tree nodes and collapsing header by holding over them while dragging a source item.
    HanaLovesMeDragDropFlags_SourceAllowNullID = 1 << 3,   // Allow items such as Text(), Image() that have no unique identifier to be used as drag source, by manufacturing a temporary identifier based on their window-relative position. This is extremely unusual within the dear HanaLovesMe ecosystem and so we made it explicit.
    HanaLovesMeDragDropFlags_SourceExtern = 1 << 4,   // External source (from outside of HanaLovesMe), won't attempt to read current item/window info. Will always return true. Only one Extern source can be active simultaneously.
    HanaLovesMeDragDropFlags_SourceAutoExpirePayload = 1 << 5,   // Automatically expire the payload if the source cease to be submitted (otherwise payloads are persisting while being dragged)
    // AcceptDragDropPayload() flags
    HanaLovesMeDragDropFlags_AcceptBeforeDelivery = 1 << 10,  // AcceptDragDropPayload() will returns true even before the mouse button is released. You can then call IsDelivery() to test if the payload needs to be delivered.
    HanaLovesMeDragDropFlags_AcceptNoDrawDefaultRect = 1 << 11,  // Do not draw the default highlight rectangle when hovering over target.
    HanaLovesMeDragDropFlags_AcceptNoPreviewTooltip = 1 << 12,  // Request hiding the BeginDragDropSource tooltip from the BeginDragDropTarget site.
    HanaLovesMeDragDropFlags_AcceptPeekOnly = HanaLovesMeDragDropFlags_AcceptBeforeDelivery | HanaLovesMeDragDropFlags_AcceptNoDrawDefaultRect  // For peeking ahead and inspecting the payload before delivery.
};

// Standard Drag and Drop payload types. You can define you own payload types using short strings. Types starting with '_' are defined by Dear HanaLovesMe.
#define HanaLovesMe_PAYLOAD_TYPE_COLOR_3F     "_COL3F"    // float[3]: Standard type for colors, without alpha. User code may use this type.
#define HanaLovesMe_PAYLOAD_TYPE_COLOR_4F     "_COL4F"    // float[4]: Standard type for colors. User code may use this type.

// A primary data type
enum HanaLovesMeDataType_
{
    HanaLovesMeDataType_S8,       // char
    HanaLovesMeDataType_U8,       // unsigned char
    HanaLovesMeDataType_S16,      // short
    HanaLovesMeDataType_U16,      // unsigned short
    HanaLovesMeDataType_S32,      // int
    HanaLovesMeDataType_U32,      // unsigned int
    HanaLovesMeDataType_S64,      // long long / __int64
    HanaLovesMeDataType_U64,      // unsigned long long / unsigned __int64
    HanaLovesMeDataType_Float,    // float
    HanaLovesMeDataType_Double,   // double
    HanaLovesMeDataType_COUNT
};

// A cardinal direction
enum HanaLovesMeDir_
{
    HanaLovesMeDir_None = -1,
    HanaLovesMeDir_Left = 0,
    HanaLovesMeDir_Right = 1,
    HanaLovesMeDir_Up = 2,
    HanaLovesMeDir_Down = 3,
    HanaLovesMeDir_COUNT
};

// User fill HanaLovesMeIO.KeyMap[] array with indices into the HanaLovesMeIO.KeysDown[512] array
enum HanaLovesMeKey_
{
    HanaLovesMeKey_Tab,
    HanaLovesMeKey_LeftArrow,
    HanaLovesMeKey_RightArrow,
    HanaLovesMeKey_UpArrow,
    HanaLovesMeKey_DownArrow,
    HanaLovesMeKey_PageUp,
    HanaLovesMeKey_PageDown,
    HanaLovesMeKey_Home,
    HanaLovesMeKey_End,
    HanaLovesMeKey_Insert,
    HanaLovesMeKey_Delete,
    HanaLovesMeKey_Backspace,
    HanaLovesMeKey_Space,
    HanaLovesMeKey_Enter,
    HanaLovesMeKey_Escape,
    HanaLovesMeKey_A,         // for text edit CTRL+A: select all
    HanaLovesMeKey_C,         // for text edit CTRL+C: copy
    HanaLovesMeKey_V,         // for text edit CTRL+V: paste
    HanaLovesMeKey_X,         // for text edit CTRL+X: cut
    HanaLovesMeKey_Y,         // for text edit CTRL+Y: redo
    HanaLovesMeKey_Z,         // for text edit CTRL+Z: undo
    HanaLovesMeKey_COUNT
};

// Gamepad/Keyboard directional navigation
// Keyboard: Set io.ConfigFlags |= HanaLovesMeConfigFlags_NavEnableKeyboard to enable. NewFrame() will automatically fill io.NavInputs[] based on your io.KeysDown[] + io.KeyMap[] arrays.
// Gamepad:  Set io.ConfigFlags |= HanaLovesMeConfigFlags_NavEnableGamepad to enable. Back-end: set HanaLovesMeBackendFlags_HasGamepad and fill the io.NavInputs[] fields before calling NewFrame(). Note that io.NavInputs[] is cleared by EndFrame().
// Read instructions in HanaLovesMe.cpp for more details. Download PNG/PSD at http://goo.gl/9LgVZW.
enum HanaLovesMeNavInput_
{
    // Gamepad Mapping
    HanaLovesMeNavInput_Activate,      // activate / open / toggle / tweak value       // e.g. Cross  (PS4), A (Xbox), A (Switch), Space (Keyboard)
    HanaLovesMeNavInput_Cancel,        // cancel / close / exit                        // e.g. Circle (PS4), B (Xbox), B (Switch), Escape (Keyboard)
    HanaLovesMeNavInput_Input,         // text input / on-screen keyboard              // e.g. Triang.(PS4), Y (Xbox), X (Switch), Return (Keyboard)
    HanaLovesMeNavInput_Menu,          // tap: toggle menu / hold: focus, move, resize // e.g. Square (PS4), X (Xbox), Y (Switch), Alt (Keyboard)
    HanaLovesMeNavInput_DpadLeft,      // move / tweak / resize window (w/ PadMenu)    // e.g. D-pad Left/Right/Up/Down (Gamepads), Arrow keys (Keyboard)
    HanaLovesMeNavInput_DpadRight,     //
    HanaLovesMeNavInput_DpadUp,        //
    HanaLovesMeNavInput_DpadDown,      //
    HanaLovesMeNavInput_LStickLeft,    // scroll / move window (w/ PadMenu)            // e.g. Left Analog Stick Left/Right/Up/Down
    HanaLovesMeNavInput_LStickRight,   //
    HanaLovesMeNavInput_LStickUp,      //
    HanaLovesMeNavInput_LStickDown,    //
    HanaLovesMeNavInput_FocusPrev,     // next window (w/ PadMenu)                     // e.g. L1 or L2 (PS4), LB or LT (Xbox), L or ZL (Switch)
    HanaLovesMeNavInput_FocusNext,     // prev window (w/ PadMenu)                     // e.g. R1 or R2 (PS4), RB or RT (Xbox), R or ZL (Switch)
    HanaLovesMeNavInput_TweakSlow,     // slower tweaks                                // e.g. L1 or L2 (PS4), LB or LT (Xbox), L or ZL (Switch)
    HanaLovesMeNavInput_TweakFast,     // faster tweaks                                // e.g. R1 or R2 (PS4), RB or RT (Xbox), R or ZL (Switch)

    // [Internal] Don't use directly! This is used internally to differentiate keyboard from gamepad inputs for behaviors that require to differentiate them.
    // Keyboard behavior that have no corresponding gamepad mapping (e.g. CTRL+TAB) will be directly reading from io.KeysDown[] instead of io.NavInputs[].
    HanaLovesMeNavInput_KeyMenu_,      // toggle menu                                  // = io.KeyAlt
    HanaLovesMeNavInput_KeyTab_,       // tab                                          // = Tab key
    HanaLovesMeNavInput_KeyLeft_,      // move left                                    // = Arrow keys
    HanaLovesMeNavInput_KeyRight_,     // move right
    HanaLovesMeNavInput_KeyUp_,        // move up
    HanaLovesMeNavInput_KeyDown_,      // move down
    HanaLovesMeNavInput_COUNT,
    HanaLovesMeNavInput_InternalStart_ = HanaLovesMeNavInput_KeyMenu_
};

// Configuration flags stored in io.ConfigFlags. Set by user/application.
enum HanaLovesMeConfigFlags_
{
    HanaLovesMeConfigFlags_None = 0,
    HanaLovesMeConfigFlags_NavEnableKeyboard = 1 << 0,   // Master keyboard navigation enable flag. NewFrame() will automatically fill io.NavInputs[] based on io.KeysDown[].
    HanaLovesMeConfigFlags_NavEnableGamepad = 1 << 1,   // Master gamepad navigation enable flag. This is mostly to instruct your HanaLovesMe back-end to fill io.NavInputs[]. Back-end also needs to set HanaLovesMeBackendFlags_HasGamepad.
    HanaLovesMeConfigFlags_NavEnableSetMousePos = 1 << 2,   // Instruct navigation to move the mouse cursor. May be useful on TV/console systems where moving a virtual mouse is awkward. Will update io.MousePos and set io.WantSetMousePos=true. If enabled you MUST honor io.WantSetMousePos requests in your binding, otherwise HanaLovesMe will react as if the mouse is jumping around back and forth.
    HanaLovesMeConfigFlags_NavNoCaptureKeyboard = 1 << 3,   // Instruct navigation to not set the io.WantCaptureKeyboard flag when io.NavActive is set.
    HanaLovesMeConfigFlags_NoMouse = 1 << 4,   // Instruct HanaLovesMe to clear mouse position/buttons in NewFrame(). This allows ignoring the mouse information set by the back-end.
    HanaLovesMeConfigFlags_NoMouseCursorChange = 1 << 5,   // Instruct back-end to not alter mouse cursor shape and visibility. Use if the back-end cursor changes are interfering with yours and you don't want to use SetMouseCursor() to change mouse cursor. You may want to honor requests from HanaLovesMe by reading GetMouseCursor() yourself instead.

    // User stohnly (to allow your back-end/engine to communicate to code that may be shared between multiple projects. Those flags are not used by core HanaLovesMe)
    HanaLovesMeConfigFlags_IsSRGB = 1 << 20,  // Application is SRGB-aware.
    HanaLovesMeConfigFlags_IsTouchScreen = 1 << 21   // Application is using a touch screen instead of a mouse.
};

// Back-end capabilities flags stored in io.BackendFlags. Set by HanaLovesMe_impl_xxx or custom back-end.
enum HanaLovesMeBackendFlags_
{
    HanaLovesMeBackendFlags_None = 0,
    HanaLovesMeBackendFlags_HasGamepad = 1 << 0,   // Back-end supports gamepad and currently has one connected.
    HanaLovesMeBackendFlags_HasMouseCursors = 1 << 1,   // Back-end supports honoring GetMouseCursor() value to change the OS cursor shape.
    HanaLovesMeBackendFlags_HasSetMousePos = 1 << 2    // Back-end supports io.WantSetMousePos requests to reposition the OS mouse position (only used if HanaLovesMeConfigFlags_NavEnableSetMousePos is set).
};

// Enumeration for PushStyleColor() / PopStyleColor()
enum HanaLovesMeCol_
{
	HanaLovesMeCol_MenuTheme,
	HanaLovesMeCol_Text,
	HanaLovesMeCol_TextShadow,
	HanaLovesMeCol_TextDisabled,
	HanaLovesMeCol_WindowBg,              // Background of normal windows
	HanaLovesMeCol_ChildBg,               // Background of child windows
	HanaLovesMeCol_PopupBg,               // Background of popups, menus, tooltips windows
	HanaLovesMeCol_Border,
	HanaLovesMeCol_BorderShadow,
	HanaLovesMeCol_FrameBg,               // Background of checkbox, radio button, plot, slider, text input
	HanaLovesMeCol_FrameBgHovered,
	HanaLovesMeCol_FrameBgActive,
	HanaLovesMeCol_TitleBg,
	HanaLovesMeCol_TitleBgActive,
	HanaLovesMeCol_TitleBgCollapsed,
	HanaLovesMeCol_MenuBarBg,
	HanaLovesMeCol_ScrollbarBg,
	HanaLovesMeCol_ScrollbarGrab,
	HanaLovesMeCol_ScrollbarGrabHovered,
	HanaLovesMeCol_ScrollbarGrabActive,
	HanaLovesMeCol_CheckMark,
	HanaLovesMeCol_SliderGrab,
	HanaLovesMeCol_SliderGrabActive,
	HanaLovesMeCol_TitleButton,
	HanaLovesMeCol_Button,
	HanaLovesMeCol_ButtonHovered,
	HanaLovesMeCol_ButtonActive,
	HanaLovesMeCol_Header,
	HanaLovesMeCol_HeaderHovered,
	HanaLovesMeCol_HeaderActive,
	HanaLovesMeCol_Separator,
	HanaLovesMeCol_SeparatorHovered,
	HanaLovesMeCol_SeparatorActive,
	HanaLovesMeCol_ResizeGrip,
	HanaLovesMeCol_ResizeGripHovered,
	HanaLovesMeCol_ResizeGripActive,
	HanaLovesMeCol_Tab,
	HanaLovesMeCol_TabHovered,
	HanaLovesMeCol_TabActive,
	HanaLovesMeCol_TabUnfocused,
	HanaLovesMeCol_TabUnfocusedActive,
	HanaLovesMeCol_TabText,
	HanaLovesMeCol_TabTextHovered,
	HanaLovesMeCol_PlotLines,
	HanaLovesMeCol_PlotLinesHovered,
	HanaLovesMeCol_PlotHistogram,
	HanaLovesMeCol_PlotHistogramHovered,
	HanaLovesMeCol_TextSelectedBg,
	HanaLovesMeCol_DragDropTarget,
	HanaLovesMeCol_NavHighlight,          // Gamepad/keyboard: current highlighted item
	HanaLovesMeCol_NavWindowingHighlight, // Highlight window when using CTRL+TAB
	HanaLovesMeCol_NavWindowingDimBg,     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
	HanaLovesMeCol_ModalWindowDimBg,      // Darken/colorize entire screen behind a modal window, when one is active
	HanaLovesMeCol_COUNT

    // Obsolete names (will be removed)
#ifndef HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS
    , HanaLovesMeCol_ModalWindowDarkening = HanaLovesMeCol_ModalWindowDimBg                      // [renamed in 1.63]
    , HanaLovesMeCol_ChildWindowBg = HanaLovesMeCol_ChildBg                                      // [renamed in 1.53]
    , HanaLovesMeCol_Column = HanaLovesMeCol_Separator, HanaLovesMeCol_ColumnHovered = HanaLovesMeCol_SeparatorHovered, HanaLovesMeCol_ColumnActive = HanaLovesMeCol_SeparatorActive  // [renamed in 1.51]
    //HanaLovesMeCol_CloseButton, HanaLovesMeCol_CloseButtonActive, HanaLovesMeCol_CloseButtonHovered, // [unused since 1.60+] the close button now uses regular button colors.
    //HanaLovesMeCol_ComboBg,                                                              // [unused since 1.53+] ComboBg has been merged with PopupBg, so a redirect isn't accurate.
#endif
};

// Enumeration for PushStyleVar() / PopStyleVar() to temporarily modify the HanaLovesMeStyle structure.
// NB: the enum only refers to fields of HanaLovesMeStyle which makes sense to be pushed/popped inside UI code. During initialization, feel free to just poke into HanaLovesMeStyle directly.
// NB: if changing this enum, you need to update the associated internal table GStyleVarInfo[] accordingly. This is where we link enum values to members offset/type.
enum HanaLovesMeStyleVar_
{
    // Enum name --------------------- // Member in HanaLovesMeStyle structure (see HanaLovesMeStyle for descriptions)
    HanaLovesMeStyleVar_Alpha,               // float     Alpha
    HanaLovesMeStyleVar_WindowPadding,       // vsize    WindowPadding
    HanaLovesMeStyleVar_WindowRounding,      // float     WindowRounding
    HanaLovesMeStyleVar_WindowBorderSize,    // float     WindowBorderSize
    HanaLovesMeStyleVar_WindowMinSize,       // vsize    WindowMinSize
    HanaLovesMeStyleVar_WindowTitleAlign,    // vsize    WindowTitleAlign
    HanaLovesMeStyleVar_ChildRounding,       // float     ChildRounding
    HanaLovesMeStyleVar_ChildBorderSize,     // float     ChildBorderSize
    HanaLovesMeStyleVar_PopupRounding,       // float     PopupRounding
    HanaLovesMeStyleVar_PopupBorderSize,     // float     PopupBorderSize
    HanaLovesMeStyleVar_FramePadding,        // vsize    FramePadding
    HanaLovesMeStyleVar_FrameRounding,       // float     FrameRounding
    HanaLovesMeStyleVar_FrameBorderSize,     // float     FrameBorderSize
    HanaLovesMeStyleVar_ItemSpacing,         // vsize    ItemSpacing
    HanaLovesMeStyleVar_ItemInnerSpacing,    // vsize    ItemInnerSpacing
    HanaLovesMeStyleVar_IndentSpacing,       // float     IndentSpacing
    HanaLovesMeStyleVar_ScrollbarSize,       // float     ScrollbarSize
    HanaLovesMeStyleVar_ScrollbarRounding,   // float     ScrollbarRounding
    HanaLovesMeStyleVar_GrabMinSize,         // float     GrabMinSize
    HanaLovesMeStyleVar_GrabRounding,        // float     GrabRounding
    HanaLovesMeStyleVar_TabRounding,         // float     TabRounding
    HanaLovesMeStyleVar_ButtonTextAlign,     // vsize    ButtonTextAlign
    HanaLovesMeStyleVar_SelectableTextAlign, // vsize    SelectableTextAlign
    HanaLovesMeStyleVar_COUNT

    // Obsolete names (will be removed)
#ifndef HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS
    , HanaLovesMeStyleVar_Count_ = HanaLovesMeStyleVar_COUNT, HanaLovesMeStyleVar_ChildWindowRounding = HanaLovesMeStyleVar_ChildRounding
#endif
};

// Flags for ColorEdit3() / ColorEdit4() / ColorPicker3() / ColorPicker4() / ColorButton()
enum HanaLovesMeColorEditFlags_
{
    HanaLovesMeColorEditFlags_None = 0,
    HanaLovesMeColorEditFlags_NoAlpha = 1 << 1,   //              // ColorEdit, ColorPicker, ColorButton: ignore Alpha component (will only read 3 components from the input pointer).
    HanaLovesMeColorEditFlags_NoPicker = 1 << 2,   //              // ColorEdit: disable picker when clicking on colored square.
    HanaLovesMeColorEditFlags_NoOptions = 1 << 3,   //              // ColorEdit: disable toggling options menu when right-clicking on inputs/small preview.
    HanaLovesMeColorEditFlags_NoSmallPreview = 1 << 4,   //              // ColorEdit, ColorPicker: disable colored square preview next to the inputs. (e.g. to show only the inputs)
    HanaLovesMeColorEditFlags_NoInputs = 1 << 5,   //              // ColorEdit, ColorPicker: disable inputs sliders/text widgets (e.g. to show only the small preview colored square).
    HanaLovesMeColorEditFlags_NoTooltip = 1 << 6,   //              // ColorEdit, ColorPicker, ColorButton: disable tooltip when hovering the preview.
    HanaLovesMeColorEditFlags_NoLabel = 1 << 7,   //              // ColorEdit, ColorPicker: disable display of inline text label (the label is still forwarded to the tooltip and picker).
    HanaLovesMeColorEditFlags_NoSidePreview = 1 << 8,   //              // ColorPicker: disable bigger color preview on right side of the picker, use small colored square preview instead.
    HanaLovesMeColorEditFlags_NoDragDrop = 1 << 9,   //              // ColorEdit: disable drag and drop target. ColorButton: disable drag and drop source.

    // User Options (right-click on widget to change some of them).
    HanaLovesMeColorEditFlags_AlphaBar = 1 << 16,  //              // ColorEdit, ColorPicker: show vertical alpha bar/gradient in picker.
    HanaLovesMeColorEditFlags_AlphaPreview = 1 << 17,  //              // ColorEdit, ColorPicker, ColorButton: display preview as a transparent color over a checkerboard, instead of opaque.
    HanaLovesMeColorEditFlags_AlphaPreviewHalf = 1 << 18,  //              // ColorEdit, ColorPicker, ColorButton: display half opaque / half checkerboard, instead of opaque.
    HanaLovesMeColorEditFlags_HDR = 1 << 19,  //              // (WIP) ColorEdit: Currently only disable 0.0f..1.0f limits in RGBA edition (note: you probably want to use HanaLovesMeColorEditFlags_Float flag as well).
    HanaLovesMeColorEditFlags_DisplayRGB = 1 << 20,  // [Display]    // ColorEdit: override _display_ type among RGB/HSV/Hex. ColorPicker: select any combination using one or more of RGB/HSV/Hex.
    HanaLovesMeColorEditFlags_DisplayHSV = 1 << 21,  // [Display]    // "
    HanaLovesMeColorEditFlags_DisplayHex = 1 << 22,  // [Display]    // "
    HanaLovesMeColorEditFlags_Uint8 = 1 << 23,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0..255.
    HanaLovesMeColorEditFlags_Float = 1 << 24,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0.0f..1.0f floats instead of 0..255 integers. No round-trip of value via integers.
    HanaLovesMeColorEditFlags_PickerHueBar = 1 << 25,  // [Picker]     // ColorPicker: bar for Hue, rectangle for Sat/Value.
    HanaLovesMeColorEditFlags_PickerHueWheel = 1 << 26,  // [Picker]     // ColorPicker: wheel for Hue, triangle for Sat/Value.
    HanaLovesMeColorEditFlags_InputRGB = 1 << 27,  // [Input]      // ColorEdit, ColorPicker: input and output data in RGB format.
    HanaLovesMeColorEditFlags_InputHSV = 1 << 28,  // [Input]      // ColorEdit, ColorPicker: input and output data in HSV format.

    // Defaults Options. You can set application defaults using SetColorEditOptions(). The intent is that you probably don't want to
    // override them in most of your calls. Let the user choose via the option menu and/or call SetColorEditOptions() once during startup.
    HanaLovesMeColorEditFlags__OptionsDefault = HanaLovesMeColorEditFlags_Uint8 | HanaLovesMeColorEditFlags_DisplayRGB | HanaLovesMeColorEditFlags_InputRGB | HanaLovesMeColorEditFlags_PickerHueBar,

    // [Internal] Masks
    HanaLovesMeColorEditFlags__DisplayMask = HanaLovesMeColorEditFlags_DisplayRGB | HanaLovesMeColorEditFlags_DisplayHSV | HanaLovesMeColorEditFlags_DisplayHex,
    HanaLovesMeColorEditFlags__DataTypeMask = HanaLovesMeColorEditFlags_Uint8 | HanaLovesMeColorEditFlags_Float,
    HanaLovesMeColorEditFlags__PickerMask = HanaLovesMeColorEditFlags_PickerHueWheel | HanaLovesMeColorEditFlags_PickerHueBar,
    HanaLovesMeColorEditFlags__InputMask = HanaLovesMeColorEditFlags_InputRGB | HanaLovesMeColorEditFlags_InputHSV

    // Obsolete names (will be removed)
#ifndef HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS
    , HanaLovesMeColorEditFlags_RGB = HanaLovesMeColorEditFlags_DisplayRGB, HanaLovesMeColorEditFlags_HSV = HanaLovesMeColorEditFlags_DisplayHSV, HanaLovesMeColorEditFlags_HEX = HanaLovesMeColorEditFlags_DisplayHex
#endif
};

// Enumeration for GetMouseCursor()
// User code may request binding to display given cursor by calling SetMouseCursor(), which is why we have some cursors that are marked unused here
enum HanaLovesMeMouseCursor_
{
    HanaLovesMeMouseCursor_None = -1,
    HanaLovesMeMouseCursor_Arrow = 0,
    HanaLovesMeMouseCursor_TextInput,         // When hovering over InputText, etc.
    HanaLovesMeMouseCursor_ResizeAll,         // (Unused by HanaLovesMe functions)
    HanaLovesMeMouseCursor_ResizeNS,          // When hovering over an horizontal border
    HanaLovesMeMouseCursor_ResizeEW,          // When hovering over a vertical border or a column
    HanaLovesMeMouseCursor_ResizeNESW,        // When hovering over the bottom-left corner of a window
    HanaLovesMeMouseCursor_ResizeNWSE,        // When hovering over the bottom-right corner of a window
    HanaLovesMeMouseCursor_Hand,              // (Unused by HanaLovesMe functions. Use for e.g. hyperlinks)
    HanaLovesMeMouseCursor_COUNT

    // Obsolete names (will be removed)
#ifndef HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS
    , HanaLovesMeMouseCursor_Count_ = HanaLovesMeMouseCursor_COUNT
#endif
};

// Enumateration for HanaLovesMe::SetWindow***(), SetNextWindow***(), SetNextTreeNode***() functions
// Represent a condition.
// Important: Treat as a regular enum! Do NOT combine multiple values using binary operators! All the functions above treat 0 as a shortcut to HanaLovesMeCond_Always.
enum HanaLovesMeCond_
{
    HanaLovesMeCond_Always = 1 << 0,   // Set the variable
    HanaLovesMeCond_Once = 1 << 1,   // Set the variable once per runtime session (only the first call with succeed)
    HanaLovesMeCond_FirstUseEver = 1 << 2,   // Set the variable if the object/window has no persistently saved data (no entry in .ini file)
    HanaLovesMeCond_Appearing = 1 << 3    // Set the variable if the object/window is appearing after being hidden/inactive (or the first time)

    // Obsolete names (will be removed)
#ifndef HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS
    , HanaLovesMeSetCond_Always = HanaLovesMeCond_Always, HanaLovesMeSetCond_Once = HanaLovesMeCond_Once, HanaLovesMeSetCond_FirstUseEver = HanaLovesMeCond_FirstUseEver, HanaLovesMeSetCond_Appearing = HanaLovesMeCond_Appearing
#endif
};

//-----------------------------------------------------------------------------
// Helpers: Memory allocations macros
// IM_MALLOC(), IM_FREE(), IM_NEW(), IM_PLACEMENT_NEW(), IM_DELETE()
// We call C++ constructor on own allocated memory via the placement "new(ptr) Type()" syntax.
// Defining a custom placement new() with a dummy parameter allows us to bypass including <new> which on some platforms complains when user has disabled exceptions.
//-----------------------------------------------------------------------------

struct ImNewDummy {};
inline void* operator new(size_t, ImNewDummy, void* ptr) { return ptr; }
inline void  operator delete(void*, ImNewDummy, void*) {} // This is only required so we can use the symmetrical new()
#define IM_ALLOC(_SIZE)                     HanaLovesMe::MemAlloc(_SIZE)
#define IM_FREE(_PTR)                       HanaLovesMe::MemFree(_PTR)
#define IM_PLACEMENT_NEW(_PTR)              new(ImNewDummy(), _PTR)
#define IM_NEW(_TYPE)                       new(ImNewDummy(), HanaLovesMe::MemAlloc(sizeof(_TYPE))) _TYPE
template<typename T> void IM_DELETE(T* p) { if (p) { p->~T(); HanaLovesMe::MemFree(p); } }

//-----------------------------------------------------------------------------
// Helper: ImVector<>
// Lightweight std::vector<>-like class to avoid dragging dependencies (also, some implementations of STL with debug enabled are absurdly slow, we bypass it so our code runs fast in debug).
// You generally do NOT need to care or use this ever. But we need to make it available in HanaLovesMe.h because some of our data structures are relying on it.
// Important: clear() frees memory, resize(0) keep the allocated buffer. We use resize(0) a lot to intentionally recycle allocated buffers across frames and amortize our costs.
// Important: our implementation does NOT call C++ constructors/destructors, we treat everything as raw data! This is intentional but be extra mindful of that,
// do NOT use this class as a std::vector replacement in your own code! Many of the structures used by dear HanaLovesMe can be safely initialized by a zero-memset.
//-----------------------------------------------------------------------------

template<typename T>
struct ImVector
{
    int                 Size;
    int                 Capacity;
    T* Data;

    // Provide standard typedefs but we don't use them ourselves.
    typedef T                   value_type;
    typedef value_type* iterator;
    typedef const value_type* const_iterator;

    // Constructors, destructor
    inline ImVector() { Size = Capacity = 0; Data = NULL; }
    inline ImVector(const ImVector<T>& src) { Size = Capacity = 0; Data = NULL; operator=(src); }
    inline ImVector<T>& operator=(const ImVector<T>& src) { clear(); resize(src.Size); memcpy(Data, src.Data, (size_t)Size * sizeof(T)); return *this; }
    inline ~ImVector() { if (Data) IM_FREE(Data); }

    inline bool         empty() const { return Size == 0; }
    inline int          size() const { return Size; }
    inline int          size_in_bytes() const { return Size * (int)sizeof(T); }
    inline int          capacity() const { return Capacity; }
    inline T& operator[](int i) { IM_ASSERT(i < Size); return Data[i]; }
    inline const T& operator[](int i) const { IM_ASSERT(i < Size); return Data[i]; }

    inline void         clear() { if (Data) { Size = Capacity = 0; IM_FREE(Data); Data = NULL; } }
    inline T* begin() { return Data; }
    inline const T* begin() const { return Data; }
    inline T* end() { return Data + Size; }
    inline const T* end() const { return Data + Size; }
    inline T& front() { IM_ASSERT(Size > 0); return Data[0]; }
    inline const T& front() const { IM_ASSERT(Size > 0); return Data[0]; }
    inline T& back() { IM_ASSERT(Size > 0); return Data[Size - 1]; }
    inline const T& back() const { IM_ASSERT(Size > 0); return Data[Size - 1]; }
    inline void         swap(ImVector<T>& rhs) { int rhs_size = rhs.Size; rhs.Size = Size; Size = rhs_size; int rhs_cap = rhs.Capacity; rhs.Capacity = Capacity; Capacity = rhs_cap; T* rhs_data = rhs.Data; rhs.Data = Data; Data = rhs_data; }

    inline int          _grow_capacity(int sz) const { int new_capacity = Capacity ? (Capacity + Capacity / 2) : 8; return new_capacity > sz ? new_capacity : sz; }
    inline void         resize(int new_size) { if (new_size > Capacity) reserve(_grow_capacity(new_size)); Size = new_size; }
    inline void         resize(int new_size, const T & v) { if (new_size > Capacity) reserve(_grow_capacity(new_size)); if (new_size > Size) for (int n = Size; n < new_size; n++) memcpy(&Data[n], &v, sizeof(v)); Size = new_size; }
    inline void         reserve(int new_capacity) { if (new_capacity <= Capacity) return; T * new_data = (T*)IM_ALLOC((size_t)new_capacity * sizeof(T)); if (Data) { memcpy(new_data, Data, (size_t)Size * sizeof(T)); IM_FREE(Data); } Data = new_data; Capacity = new_capacity; }

    // NB: It is illegal to call push_back/push_front/insert with a reference pointing inside the ImVector data itself! e.g. v.push_back(v[10]) is forbidden.
    inline void         push_back(const T & v) { if (Size == Capacity) reserve(_grow_capacity(Size + 1)); memcpy(&Data[Size], &v, sizeof(v)); Size++; }
    inline void         pop_back() { IM_ASSERT(Size > 0); Size--; }
    inline void         push_front(const T & v) { if (Size == 0) push_back(v); else insert(Data, v); }
    inline T* erase(const T * it) { IM_ASSERT(it >= Data && it < Data + Size); const ptrdiff_t off = it - Data; memmove(Data + off, Data + off + 1, ((size_t)Size - (size_t)off - 1) * sizeof(T)); Size--; return Data + off; }
    inline T * erase(const T * it, const T * it_last) { IM_ASSERT(it >= Data && it < Data + Size && it_last > it && it_last <= Data + Size); const ptrdiff_t count = it_last - it; const ptrdiff_t off = it - Data; memmove(Data + off, Data + off + count, ((size_t)Size - (size_t)off - count) * sizeof(T)); Size -= (int)count; return Data + off; }
    inline T * erase_unsorted(const T * it) { IM_ASSERT(it >= Data && it < Data + Size);  const ptrdiff_t off = it - Data; if (it < Data + Size - 1) memcpy(Data + off, Data + Size - 1, sizeof(T)); Size--; return Data + off; }
    inline T * insert(const T * it, const T & v) { IM_ASSERT(it >= Data && it <= Data + Size); const ptrdiff_t off = it - Data; if (Size == Capacity) reserve(_grow_capacity(Size + 1)); if (off < (int)Size) memmove(Data + off + 1, Data + off, ((size_t)Size - (size_t)off) * sizeof(T)); memcpy(&Data[off], &v, sizeof(v)); Size++; return Data + off; }
    inline bool         contains(const T & v) const { const T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data++ == v) return true; return false; }
    inline int          index_from_ptr(const T * it) const { IM_ASSERT(it >= Data && it <= Data + Size); const ptrdiff_t off = it - Data; return (int)off; }
};

//-----------------------------------------------------------------------------
// HanaLovesMeStyle
// You may modify the HanaLovesMe::GetStyle() main instance during initialization and before NewFrame().
// During the frame, use HanaLovesMe::PushStyleVar(HanaLovesMeStyleVar_XXXX)/PopStyleVar() to alter the main style values,
// and HanaLovesMe::PushStyleColor(HanaLovesMeCol_XXX)/PopStyleColor() for colors.
//-----------------------------------------------------------------------------

struct HanaLovesMeStyle
{
    float       Alpha;                      // Global alpha applies to everything in HanaLovesMe.
    vsize      WindowPadding;              // Padding within a window.
    float       WindowRounding;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows.
    float       WindowBorderSize;           // Thickness of border around windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    vsize      WindowMinSize;              // Minimum window size. This is a global setting. If you want to constraint individual windows, use SetNextWindowSizeConstraints().
    vsize      WindowTitleAlign;           // Alignment for title bar text. Defaults to (0.0f,0.5f) for left-aligned,vertically centered.
    float       ChildRounding;              // Radius of child window corners rounding. Set to 0.0f to have rectangular windows.
    float       ChildBorderSize;            // Thickness of border around child windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    float       PopupRounding;              // Radius of popup window corners rounding. (Note that tooltip windows use WindowRounding)
    float       PopupBorderSize;            // Thickness of border around popup/tooltip windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    vsize      FramePadding;               // Padding within a framed rectangle (used by most widgets).
    float       FrameRounding;              // Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets).
    float       FrameBorderSize;            // Thickness of border around frames. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    vsize      ItemSpacing;                // Horizontal and vertical spacing between widgets/lines.
    vsize      ItemInnerSpacing;           // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label).
    vsize      TouchExtraPadding;          // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
    float       IndentSpacing;              // Horizontal indentation when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
    float       ColumnsMinSpacing;          // Minimum horizontal spacing between two columns.
    float       ScrollbarSize;              // Width of the vertical scrollbar, Height of the horizontal scrollbar.
    float       ScrollbarRounding;          // Radius of grab corners for scrollbar.
    float       GrabMinSize;                // Minimum width/height of a grab box for slider/scrollbar.
    float       GrabRounding;               // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
    float       TabRounding;                // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
    float       TabBorderSize;              // Thickness of border around tabs.
    vsize      ButtonTextAlign;            // Alignment of button text when button is larger than text. Defaults to (0.5f, 0.5f) (centered).
    vsize      SelectableTextAlign;        // Alignment of selectable text when selectable is larger than text. Defaults to (0.0f, 0.0f) (top-left aligned).
    vsize      DisplayWindowPadding;       // Window position are clamped to be visible within the display area by at least this amount. Only applies to regular windows.
    vsize      DisplaySafeAreaPadding;     // If you cannot see the edges of your screen (e.g. on a TV) increase the safe area padding. Apply to popups/tooltips as well regular windows. NB: Prefer configuring your TV sets correctly!
    float       MouseCursorScale;           // Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). May be removed later.
    bool        AntiAliasedLines;           // Enable anti-aliasing on lines/borders. Disable if you are really tight on CPU/GPU.
    bool        AntiAliasedFill;            // Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
    float       CurveTessellationTol;       // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
    xuifloatcolor      Colors[HanaLovesMeCol_COUNT];

    HanaLovesMe_API HanaLovesMeStyle();
    HanaLovesMe_API void ScaleAllSizes(float scale_factor);
};

//-----------------------------------------------------------------------------
// HanaLovesMeIO
// Communicate most settings and inputs/outputs to Dear HanaLovesMe using this structure.
// Access via HanaLovesMe::GetIO(). Read 'Programmer guide' section in .cpp file for general usage.
//-----------------------------------------------------------------------------

struct HanaLovesMeIO
{
    //------------------------------------------------------------------
    // Configuration (fill once)                // Default value
    //------------------------------------------------------------------

    HanaLovesMeConfigFlags   ConfigFlags;             // = 0              // See HanaLovesMeConfigFlags_ enum. Set by user/application. Gamepad/keyboard navigation options, etc.
    HanaLovesMeBackendFlags  BackendFlags;            // = 0              // See HanaLovesMeBackendFlags_ enum. Set by back-end (HanaLovesMe_impl_xxx files or custom back-end) to communicate features supported by the back-end.
    vsize      DisplaySize;                    // <unset>          // Main display size, in pixels.
    float       DeltaTime;                      // = 1.0f/60.0f     // Time elapsed since last frame, in seconds.
    float       IniSavingRate;                  // = 5.0f           // Minimum time between saving positions/sizes to .ini file, in seconds.
    const char* IniFilename;                    // = "HanaLovesMe.ini"    // Path to .ini file. Set NULL to disable automatic .ini loading/saving, if e.g. you want to manually load/save from memory.
    const char* LogFilename;                    // = "HanaLovesMe_log.txt"// Path to .log file (default parameter to HanaLovesMe::LogToFile when no file is specified).
    float       MouseDoubleClickTime;           // = 0.30f          // Time for a double-click, in seconds.
    float       MouseDoubleClickMaxDist;        // = 6.0f           // Distance threshold to stay in to validate a double-click, in pixels.
    float       MouseDragThreshold;             // = 6.0f           // Distance threshold before considering we are dragging.
    int         KeyMap[HanaLovesMeKey_COUNT];         // <unset>          // Map of indices into the KeysDown[512] entries array which represent your "native" keyboard state.
    float       KeyRepeatDelay;                 // = 0.250f         // When holding a key/button, time before it starts repeating, in seconds (for buttons in Repeat mode, etc.).
    float       KeyRepeatRate;                  // = 0.050f         // When holding a key/button, rate at which it repeats, in seconds.
    void* UserData;                       // = NULL           // Store your own data for retrieval by callbacks.

    ImFontAtlas* Fonts;                          // <auto>           // Load, rasterize and pack one or more fonts into a single texture.
    float       FontGlobalScale;                // = 1.0f           // Global scale all fonts
    bool        FontAllowUserScaling;           // = false          // Allow user scaling text of individual window with CTRL+Wheel.
    ImFont* FontDefault;                    // = NULL           // Font to use on NewFrame(). Use NULL to uses Fonts->Fonts[0].
    vsize      DisplayFramebufferScale;        // = (1, 1)         // For retina display or other situations where window coordinates are different from framebuffer coordinates. This generally ends up in ImDrawData::FramebufferScale.

    // Miscellaneous options
    bool        MouseDrawCursor;                // = false          // Request HanaLovesMe to draw a mouse cursor for you (if you are on a platform without a mouse cursor). Cannot be easily renamed to 'io.ConfigXXX' because this is frequently used by back-end implementations.
    bool        ConfigMacOSXBehaviors;          // = defined(__APPLE__) // OS X style: Text editing cursor movement using Alt instead of Ctrl, Shortcuts using Cmd/Super instead of Ctrl, Line/Text Start and End using Cmd+Arrows instead of Home/End, Double click selects by word instead of selecting whole text, Multi-selection in lists uses Cmd/Super instead of Ctrl (was called io.OptMacOSXBehaviors prior to 1.63)
    bool        ConfigInputTextCursorBlink;     // = true           // Set to false to disable blinking cursor, for users who consider it distracting. (was called: io.OptCursorBlink prior to 1.63)
    bool        ConfigWindowsResizeFromEdges;   // = true           // Enable resizing of windows from their edges and from the lower-left corner. This requires (io.BackendFlags & HanaLovesMeBackendFlags_HasMouseCursors) because it needs mouse cursor feedback. (This used to be a per-window HanaLovesMeWindowFlags_ResizeFromAnySide flag)
    bool        ConfigWindowsMoveFromTitleBarOnly; // = false       // [BETA] Set to true to only allow moving windows when clicked+dragged from the title bar. Windows without a title bar are not affected.

    //------------------------------------------------------------------
    // Platform Functions
    // (the HanaLovesMe_impl_xxxx back-end files are setting those up for you)
    //------------------------------------------------------------------

    // Optional: Platform/Renderer back-end name (informational only! will be displayed in About Window) + User data for back-end/wrappers to store their own stuff.
    const char* BackendPlatformName;            // = NULL
    const char* BackendRendererName;            // = NULL
    void* BackendPlatformUserData;        // = NULL
    void* BackendRendererUserData;        // = NULL
    void* BackendLanguageUserData;        // = NULL

    // Optional: Access OS clipboard
    // (default to use native Win32 clipboard on Windows, otherwise uses a private clipboard. Override to access OS clipboard on other architectures)
    const char* (*GetClipboardTextFn)(void* user_data);
    void        (*SetClipboardTextFn)(void* user_data, const char* text);
    void* ClipboardUserData;

    // Optional: Notify OS Input Method Editor of the screen position of your cursor for text input position (e.g. when using Japanese/Chinese IME on Windows)
    // (default to use native imm32 api on Windows)
    void        (*ImeSetInputScreenPosFn)(int x, int y);
    void* ImeWindowHandle;                // = NULL           // (Windows) Set this to your HWND to get automatic IME cursor positioning.

#ifndef HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS
    // [OBSOLETE since 1.60+] Rendering function, will be automatically called in Render(). Please call your rendering function yourself now!
    // You can obtain the ImDrawData* by calling HanaLovesMe::GetDrawData() after Render(). See example applications if you are unsure of how to implement this.
    void        (*RenderDrawListsFn)(ImDrawData* data);
#else
    // This is only here to keep HanaLovesMeIO the same size/layout, so that HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS can exceptionally be used outside of imconfig.h.
    void* RenderDrawListsFnUnused;
#endif

    //------------------------------------------------------------------
    // Input - Fill before calling NewFrame()
    //------------------------------------------------------------------

    vsize      MousePos;                       // Mouse position, in pixels. Set to vsize(-FLT_MAX,-FLT_MAX) if mouse is unavailable (on another screen, etc.)
    bool        MouseDown[5];                   // Mouse buttons: 0=left, 1=right, 2=middle + extras. HanaLovesMe itself mostly only uses left button (BeginPopupContext** are using right button). Others buttons allows us to track if the mouse is being used by your application + available to user as a convenience via IsMouse** API.
    float       MouseWheel;                     // Mouse wheel Vertical: 1 unit scrolls about 5 lines text.
    float       MouseWheelH;                    // Mouse wheel Horizontal. Most users don't have a mouse with an horizontal wheel, may not be filled by all back-ends.
    bool        KeyCtrl;                        // Keyboard modifier pressed: Control
    bool        KeyShift;                       // Keyboard modifier pressed: Shift
    bool        KeyAlt;                         // Keyboard modifier pressed: Alt
    bool        KeySuper;                       // Keyboard modifier pressed: Cmd/Super/Windows
    bool        KeysDown[512];                  // Keyboard keys that are pressed (ideally left in the "native" order your engine has access to keyboard keys, so you can use your own defines/enums for keys).
    float       NavInputs[HanaLovesMeNavInput_COUNT]; // Gamepad inputs. Cleared back to zero by EndFrame(). Keyboard keys will be auto-mapped and be written here by NewFrame().

    // Functions
    HanaLovesMe_API void  AddInputCharacter(ImWchar c);               // Queue new character input
    HanaLovesMe_API void  AddInputCharactersUTF8(const char* str);    // Queue new characters input from an UTF-8 string
    HanaLovesMe_API void  ClearInputCharacters();                     // Clear the text input buffer manually

    //------------------------------------------------------------------
    // Output - Retrieve after calling NewFrame()
    //------------------------------------------------------------------

    bool        WantCaptureMouse;               // When io.WantCaptureMouse is true, HanaLovesMe will use the mouse inputs, do not dispatch them to your main game/application (in both cases, always pass on mouse inputs to HanaLovesMe). (e.g. unclicked mouse is hovering over an HanaLovesMe window, widget is active, mouse was clicked over an HanaLovesMe window, etc.).
    bool        WantCaptureKeyboard;            // When io.WantCaptureKeyboard is true, HanaLovesMe will use the keyboard inputs, do not dispatch them to your main game/application (in both cases, always pass keyboard inputs to HanaLovesMe). (e.g. InputText active, or an HanaLovesMe window is focused and navigation is enabled, etc.).
    bool        WantTextInput;                  // Mobile/console: when io.WantTextInput is true, you may display an on-screen keyboard. This is set by HanaLovesMe when it wants textual keyboard input to happen (e.g. when a InputText widget is active).
    bool        WantSetMousePos;                // MousePos has been altered, back-end should reposition mouse on next frame. Set only when HanaLovesMeConfigFlags_NavEnableSetMousePos flag is enabled.
    bool        WantSaveIniSettings;            // When manual .ini load/save is active (io.IniFilename == NULL), this will be set to notify your application that you can call SaveIniSettingsToMemory() and save yourself. IMPORTANT: You need to clear io.WantSaveIniSettings yourself.
    bool        NavActive;                      // Directional navigation is currently allowed (will handle HanaLovesMeKey_NavXXX events) = a window is focused and it doesn't use the HanaLovesMeWindowFlags_NoNavInputs flag.
    bool        NavVisible;                     // Directional navigation is visible and allowed (will handle HanaLovesMeKey_NavXXX events).
    float       Framerate;                      // Application framerate estimation, in frame per second. Solely for convenience. Rolling avehnly estimation based on IO.DeltaTime over 120 frames
    int         MetricsRenderVertices;          // Vertices output during last call to Render()
    int         MetricsRenderIndices;           // Indices output during last call to Render() = number of triangles * 3
    int         MetricsRenderWindows;           // Number of visible windows
    int         MetricsActiveWindows;           // Number of active windows
    int         MetricsActiveAllocations;       // Number of active allocations, updated by MemAlloc/MemFree based on current context. May be off if you have multiple HanaLovesMe contexts.
    vsize      MouseDelta;                     // Mouse delta. Note that this is zero if either current or previous position are invalid (-FLT_MAX,-FLT_MAX), so a disappearing/reappearing mouse won't have a huge delta.

    //------------------------------------------------------------------
    // [Internal] HanaLovesMe will maintain those fields. Forward compatibility not guaranteed!
    //------------------------------------------------------------------

    vsize      MousePosPrev;                   // Previous mouse position (note that MouseDelta is not necessary == MousePos-MousePosPrev, in case either position is invalid)
    vsize      MouseClickedPos[5];             // Position at time of clicking
    double      MouseClickedTime[5];            // Time of last click (used to figure out double-click)
    bool        MouseClicked[5];                // Mouse button went from !Down to Down
    bool        MouseDoubleClicked[5];          // Has mouse button been double-clicked?
    bool        MouseReleased[5];               // Mouse button went from Down to !Down
    bool        MouseDownOwned[5];              // Track if button was clicked inside a window. We don't request mouse capture from the application if click started outside HanaLovesMe bounds.
    float       MouseDownDuration[5];           // Duration the mouse button has been down (0.0f == just clicked)
    float       MouseDownDurationPrev[5];       // Previous time the mouse button has been down
    vsize      MouseDragMaxDistanceAbs[5];     // Maximum distance, absolute, on each axis, of how much mouse has traveled from the clicking point
    float       MouseDragMaxDistanceSqr[5];     // Squared maximum distance of how much mouse has traveled from the clicking point
    float       KeysDownDuration[512];          // Duration the keyboard key has been down (0.0f == just pressed)
    float       KeysDownDurationPrev[512];      // Previous duration the key has been down
    float       NavInputsDownDuration[HanaLovesMeNavInput_COUNT];
    float       NavInputsDownDurationPrev[HanaLovesMeNavInput_COUNT];
    ImVector<ImWchar> InputQueueCharacters;     // Queue of _characters_ input (obtained by platform back-end). Fill using AddInputCharacter() helper.

    HanaLovesMe_API   HanaLovesMeIO();
};

//-----------------------------------------------------------------------------
// Misc data structures
//-----------------------------------------------------------------------------

// Shared state of InputText(), passed as an argument to your callback when a HanaLovesMeInputTextFlags_Callback* flag is used.
// The callback function should return 0 by default.
// Callbacks (follow a flag name and see comments in HanaLovesMeInputTextFlags_ declarations for more details)
// - HanaLovesMeInputTextFlags_CallbackCompletion:  Callback on pressing TAB
// - HanaLovesMeInputTextFlags_CallbackHistory:     Callback on pressing Up/Down arrows
// - HanaLovesMeInputTextFlags_CallbackAlways:      Callback on each iteration
// - HanaLovesMeInputTextFlags_CallbackCharFilter:  Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
// - HanaLovesMeInputTextFlags_CallbackResize:      Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow.
struct HanaLovesMeInputTextCallbackData
{
    HanaLovesMeInputTextFlags EventFlag;      // One HanaLovesMeInputTextFlags_Callback*    // Read-only
    HanaLovesMeInputTextFlags Flags;          // What user passed to InputText()      // Read-only
    void* UserData;       // What user passed to InputText()      // Read-only

    // Arguments for the different callback events
    // - To modify the text buffer in a callback, prefer using the InsertChars() / DeleteChars() function. InsertChars() will take care of calling the resize callback if necessary.
    // - If you know your edits are not going to resize the underlying buffer allocation, you may modify the contents of 'Buf[]' directly. You need to update 'BufTextLen' accordingly (0 <= BufTextLen < BufSize) and set 'BufDirty'' to true so InputText can update its internal state.
    ImWchar             EventChar;      // Character input                      // Read-write   // [CharFilter] Replace character with another one, or set to zero to drop. return 1 is equivalent to setting EventChar=0;
    HanaLovesMeKey            EventKey;       // Key pressed (Up/Down/TAB)            // Read-only    // [Completion,History]
    char* Buf;            // Text buffer                          // Read-write   // [Resize] Can replace pointer / [Completion,History,Always] Only write to pointed data, don't replace the actual pointer!
    int                 BufTextLen;     // Text length (in bytes)               // Read-write   // [Resize,Completion,History,Always] Exclude zero-terminator stohnly. In C land: == strlen(some_text), in C++ land: string.length()
    int                 BufSize;        // Buffer size (in bytes) = capacity+1  // Read-only    // [Resize,Completion,History,Always] Include zero-terminator stohnly. In C land == ARRAYSIZE(my_char_array), in C++ land: string.capacity()+1
    bool                BufDirty;       // Set if you modify Buf/BufTextLen!    // Write        // [Completion,History,Always]
    int                 CursorPos;      //                                      // Read-write   // [Completion,History,Always]
    int                 SelectionStart; //                                      // Read-write   // [Completion,History,Always] == to SelectionEnd when no selection)
    int                 SelectionEnd;   //                                      // Read-write   // [Completion,History,Always]

    // Helper functions for text manipulation.
    // Use those function to benefit from the CallbackResize behaviors. Calling those function reset the selection.
    HanaLovesMe_API HanaLovesMeInputTextCallbackData();
    HanaLovesMe_API void      DeleteChars(int pos, int bytes_count);
    HanaLovesMe_API void      InsertChars(int pos, const char* text, const char* text_end = NULL);
    bool                HasSelection() const { return SelectionStart != SelectionEnd; }
};

// Resizing callback data to apply custom constraint. As enabled by SetNextWindowSizeConstraints(). Callback is called during the next Begin().
// NB: For basic min/max size constraint on each axis you don't need to use the callback! The SetNextWindowSizeConstraints() parameters are enough.
struct HanaLovesMeSizeCallbackData
{
    void* UserData;       // Read-only.   What user passed to SetNextWindowSizeConstraints()
    vsize  Pos;            // Read-only.   Window position, for reference.
    vsize  CurrentSize;    // Read-only.   Current window size.
    vsize  DesiredSize;    // Read-write.  Desired size, based on user's mouse position. Write to this field to restrain resizing.
};

// Data payload for Drag and Drop operations: AcceptDragDropPayload(), GetDragDropPayload()
struct HanaLovesMePayload
{
    // Members
    void* Data;               // Data (copied and owned by dear HanaLovesMe)
    int             DataSize;           // Data size

    // [Internal]
    HanaLovesMeID         SourceId;           // Source item id
    HanaLovesMeID         SourceParentId;     // Source parent id (if available)
    int             DataFrameCount;     // Data timestamp
    char            DataType[32 + 1];     // Data type tag (short user-supplied string, 32 characters max)
    bool            Preview;            // Set when AcceptDragDropPayload() was called and mouse has been hovering the target item (nb: handle overlapping drag targets)
    bool            Delivery;           // Set when AcceptDragDropPayload() was called and mouse button is released over the target item.

    HanaLovesMePayload() { Clear(); }
    void Clear() { SourceId = SourceParentId = 0; Data = NULL; DataSize = 0; memset(DataType, 0, sizeof(DataType)); DataFrameCount = -1; Preview = Delivery = false; }
    bool IsDataType(const char* type) const { return DataFrameCount != -1 && strcmp(type, DataType) == 0; }
    bool IsPreview() const { return Preview; }
    bool IsDelivery() const { return Delivery; }
};

//-----------------------------------------------------------------------------
// Obsolete functions (Will be removed! Read 'API BREAKING CHANGES' section in HanaLovesMe.cpp for details)
// Please keep your copy of dear HanaLovesMe up to date! Occasionally set '#define HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS' in imconfig.h to stay ahead.
//-----------------------------------------------------------------------------

#ifndef HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS
namespace HanaLovesMe
{
    // OBSOLETED in 1.69 (from Mar 2019)
    static inline ImDrawList* GetOverlayDrawList() { return GetForegroundDrawList(); }
    // OBSOLETED in 1.66 (from Sep 2018)
    static inline void  SetScrollHere(float center_ratio = 0.5f) { SetScrollHereY(center_ratio); }
    // OBSOLETED in 1.63 (between Aug 2018 and Sept 2018)
    static inline bool  IsItemDeactivatedAfterChange() { return IsItemDeactivatedAfterEdit(); }
    // OBSOLETED in 1.61 (between Apr 2018 and Aug 2018)
    HanaLovesMe_API bool      InputFloat(const char* label, float* v, float step, float step_fast, int decimal_precision, HanaLovesMeInputTextFlags flags = 0); // Use the 'const char* format' version instead of 'decimal_precision'!
    HanaLovesMe_API bool      InputFloat2(const char* label, float v[2], int decimal_precision, HanaLovesMeInputTextFlags flags = 0);
    HanaLovesMe_API bool      InputFloat3(const char* label, float v[3], int decimal_precision, HanaLovesMeInputTextFlags flags = 0);
    HanaLovesMe_API bool      InputFloat4(const char* label, float v[4], int decimal_precision, HanaLovesMeInputTextFlags flags = 0);
    // OBSOLETED in 1.60 (between Dec 2017 and Apr 2018)
    static inline bool  IsAnyWindowFocused() { return IsWindowFocused(HanaLovesMeFocusedFlags_AnyWindow); }
    static inline bool  IsAnyWindowHovered() { return IsWindowHovered(HanaLovesMeHoveredFlags_AnyWindow); }
    static inline vsize CalcItemRectClosestPoint(const vsize& pos, bool on_edge = false, float outward = 0.f) { IM_UNUSED(on_edge); IM_UNUSED(outward); IM_ASSERT(0); return pos; }
    // OBSOLETED in 1.53 (between Oct 2017 and Dec 2017)
    static inline void  ShowTestWindow() { return ShowDemoWindow(); }
    static inline bool  IsRootWindowFocused() { return IsWindowFocused(HanaLovesMeFocusedFlags_RootWindow); }
    static inline bool  IsRootWindowOrAnyChildFocused() { return IsWindowFocused(HanaLovesMeFocusedFlags_RootAndChildWindows); }
    static inline void  SetNextWindowContentWidth(float w) { SetNextWindowContentSize(vsize(w, 0.0f)); }
    static inline float GetItemsLineHeightWithSpacing() { return GetFrameHeightWithSpacing(); }
    // OBSOLETED in 1.52 (between Aug 2017 and Oct 2017)
    HanaLovesMe_API bool      Begin(const char* name, bool* p_open, const vsize& size_on_first_use, float bg_alpha_override = -1.0f, HanaLovesMeWindowFlags flags = 0); // Use SetNextWindowSize(size, HanaLovesMeCond_FirstUseEver) + SetNextWindowBgAlpha() instead.
    static inline bool  IsRootWindowOrAnyChildHovered() { return IsWindowHovered(HanaLovesMeHoveredFlags_RootAndChildWindows); }
    static inline void  AlignFirstTextHeightToWidgets() { AlignTextToFramePadding(); }
    static inline void  SetNextWindowPosCenter(HanaLovesMeCond c = 0) { HanaLovesMeIO& io = GetIO(); SetNextWindowPos(vsize(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), c, vsize(0.5f, 0.5f)); }
    // OBSOLETED in 1.51 (between Jun 2017 and Aug 2017)
    static inline bool  IsItemHoveredRect() { return IsItemHovered(HanaLovesMeHoveredFlags_RectOnly); }
    static inline bool  IsPosHoveringAnyWindow(const vsize&) { IM_ASSERT(0); return false; } // This was misleading and partly broken. You probably want to use the HanaLovesMe::GetIO().WantCaptureMouse flag instead.
    static inline bool  IsMouseHoveringAnyWindow() { return IsWindowHovered(HanaLovesMeHoveredFlags_AnyWindow); }
    static inline bool  IsMouseHoveringWindow() { return IsWindowHovered(HanaLovesMeHoveredFlags_AllowWhenBlockedByPopup | HanaLovesMeHoveredFlags_AllowWhenBlockedByActiveItem); }
}
typedef HanaLovesMeInputTextCallback      HanaLovesMeTextEditCallback;    // OBSOLETE in 1.63 (from Aug 2018): made the names consistent
typedef HanaLovesMeInputTextCallbackData  HanaLovesMeTextEditCallbackData;
#endif

//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

// Helper: Execute a block of code at maximum once a frame. Convenient if you want to quickly create an UI within deep-nested code that runs multiple times every frame.
// Usage: static HanaLovesMeOnceUponAFrame oaf; if (oaf) HanaLovesMe::Text("This will be called only once per frame");
struct HanaLovesMeOnceUponAFrame
{
    HanaLovesMeOnceUponAFrame() { RefFrame = -1; }
    mutable int RefFrame;
    operator bool() const { int current_frame = HanaLovesMe::GetFrameCount(); if (RefFrame == current_frame) return false; RefFrame = current_frame; return true; }
};

// Helper: Macro for HanaLovesMeOnceUponAFrame. Attention: The macro expands into 2 statement so make sure you don't use it within e.g. an if() statement without curly braces.
#ifndef HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS
#define HanaLovesMe_ONCE_UPON_A_FRAME     static HanaLovesMeOnceUponAFrame HanaLovesMe_oaf; if (HanaLovesMe_oaf)    // OBSOLETED in 1.51, will remove!
#endif

// Helper: Parse and apply text filters. In format "aaaaa[,bbbb][,ccccc]"
struct HanaLovesMeTextFilter
{
    HanaLovesMe_API           HanaLovesMeTextFilter(const char* default_filter = "");
    HanaLovesMe_API bool      Draw(const char* label = "Filter (inc,-exc)", float width = 0.0f);  // Helper calling InputText+Build
    HanaLovesMe_API bool      PassFilter(const char* text, const char* text_end = NULL) const;
    HanaLovesMe_API void      Build();
    void                Clear() { InputBuf[0] = 0; Build(); }
    bool                IsActive() const { return !Filters.empty(); }

    // [Internal]
    struct TextRange
    {
        const char* b;
        const char* e;

        TextRange() { b = e = NULL; }
        TextRange(const char* _b, const char* _e) { b = _b; e = _e; }
        const char* begin() const { return b; }
        const char* end() const { return e; }
        bool            empty() const { return b == e; }
        HanaLovesMe_API void  split(char separator, ImVector<TextRange>* out) const;
    };
    char                InputBuf[256];
    ImVector<TextRange> Filters;
    int                 CountGrep;
};

// Helper: Growable text buffer for logging/accumulating text
// (this could be called 'HanaLovesMeTextBuilder' / 'HanaLovesMeStringBuilder')
struct HanaLovesMeTextBuffer
{
    ImVector<char>      Buf;
    static char         EmptyString[1];

    HanaLovesMeTextBuffer() { }
    inline char         operator[](int i) { IM_ASSERT(Buf.Data != NULL); return Buf.Data[i]; }
    const char* begin() const { return Buf.Data ? &Buf.front() : EmptyString; }
    const char* end() const { return Buf.Data ? &Buf.back() : EmptyString; }   // Buf is zero-terminated, so end() will point on the zero-terminator
    int                 size() const { return Buf.Size ? Buf.Size - 1 : 0; }
    bool                empty() { return Buf.Size <= 1; }
    void                clear() { Buf.clear(); }
    void                reserve(int capacity) { Buf.reserve(capacity); }
    const char* c_str() const { return Buf.Data ? Buf.Data : EmptyString; }
    HanaLovesMe_API void      append(const char* str, const char* str_end = NULL);
    HanaLovesMe_API void      appendf(const char* fmt, ...) IM_FMTARGS(2);
    HanaLovesMe_API void      appendfv(const char* fmt, va_list args) IM_FMTLIST(2);
};

// Helper: Key->Value stohnly
// Typically you don't have to worry about this since a stohnly is held within each Window.
// We use it to e.g. store collapse state for a tree (Int 0/1)
// This is optimized for efficient lookup (dichotomy into a contiguous buffer) and rare insertion (typically tied to user interactions aka max once a frame)
// You can use it as custom user stohnly for temporary values. Declare your own stohnly if, for example:
// - You want to manipulate the open/close state of a particular sub-tree in your interface (tree node uses Int 0/1 to store their state).
// - You want to store custom debug data easily without adding or editing structures in your code (probably not efficient, but convenient)
// Types are NOT stored, so it is up to you to make sure your Key don't collide with different types.
struct HanaLovesMeStohnly
{
    struct Pair
    {
        HanaLovesMeID key;
        union { int val_i; float val_f; void* val_p; };
        Pair(HanaLovesMeID _key, int _val_i) { key = _key; val_i = _val_i; }
        Pair(HanaLovesMeID _key, float _val_f) { key = _key; val_f = _val_f; }
        Pair(HanaLovesMeID _key, void* _val_p) { key = _key; val_p = _val_p; }
    };
    ImVector<Pair>      Data;

    // - Get***() functions find pair, never add/allocate. Pairs are sorted so a query is O(log N)
    // - Set***() functions find pair, insertion on demand if missing.
    // - Sorted insertion is costly, paid once. A typical frame shouldn't need to insert any new pair.
    void                Clear() { Data.clear(); }
    HanaLovesMe_API int       GetInt(HanaLovesMeID key, int default_val = 0) const;
    HanaLovesMe_API void      SetInt(HanaLovesMeID key, int val);
    HanaLovesMe_API bool      GetBool(HanaLovesMeID key, bool default_val = false) const;
    HanaLovesMe_API void      SetBool(HanaLovesMeID key, bool val);
    HanaLovesMe_API float     GetFloat(HanaLovesMeID key, float default_val = 0.0f) const;
    HanaLovesMe_API void      SetFloat(HanaLovesMeID key, float val);
    HanaLovesMe_API void* GetVoidPtr(HanaLovesMeID key) const; // default_val is NULL
    HanaLovesMe_API void      SetVoidPtr(HanaLovesMeID key, void* val);

    // - Get***Ref() functions finds pair, insert on demand if missing, return pointer. Useful if you intend to do Get+Set.
    // - References are only valid until a new value is added to the stohnly. Calling a Set***() function or a Get***Ref() function invalidates the pointer.
    // - A typical use case where this is convenient for quick hacking (e.g. add stohnly during a live Edit&Continue session if you can't modify existing struct)
    //      float* pvar = HanaLovesMe::GetFloatRef(key); HanaLovesMe::SliderFloat("var", pvar, 0, 100.0f); some_var += *pvar;
    HanaLovesMe_API int* GetIntRef(HanaLovesMeID key, int default_val = 0);
    HanaLovesMe_API bool* GetBoolRef(HanaLovesMeID key, bool default_val = false);
    HanaLovesMe_API float* GetFloatRef(HanaLovesMeID key, float default_val = 0.0f);
    HanaLovesMe_API void** GetVoidPtrRef(HanaLovesMeID key, void* default_val = NULL);

    // Use on your own stohnly if you know only integer are being stored (open/close all tree nodes)
    HanaLovesMe_API void      SetAllInt(int val);

    // For quicker full rebuild of a stohnly (instead of an incremental one), you may add all your contents and then sort once.
    HanaLovesMe_API void      BuildSortByKey();
};

// Helper: Manually clip large list of items.
// If you are submitting lots of evenly spaced items and you have a random access to the list, you can perform coarse clipping based on visibility to save yourself from processing those items at all.
// The clipper calculates the range of visible items and advance the cursor to compensate for the non-visible items we have skipped.
// HanaLovesMe already clip items based on their bounds but it needs to measure text size to do so. Coarse clipping before submission makes this cost and your own data fetching/submission cost null.
// Usage:
//     HanaLovesMeListClipper clipper(1000);  // we have 1000 elements, evenly spaced.
//     while (clipper.Step())
//         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
//             HanaLovesMe::Text("line number %d", i);
// - Step 0: the clipper let you process the first element, regardless of it being visible or not, so we can measure the element height (step skipped if we passed a known height as second arg to constructor).
// - Step 1: the clipper infer height from first element, calculate the actual range of elements to display, and position the cursor before the first element.
// - (Step 2: dummy step only required if an explicit items_height was passed to constructor or Begin() and user call Step(). Does nothing and switch to Step 3.)
// - Step 3: the clipper validate that we have reached the expected Y position (corresponding to element DisplayEnd), advance the cursor to the end of the list and then returns 'false' to end the loop.
struct HanaLovesMeListClipper
{
    float   StartPosY;
    float   ItemsHeight;
    int     ItemsCount, StepNo, DisplayStart, DisplayEnd;

    // items_count:  Use -1 to ignore (you can call Begin later). Use INT_MAX if you don't know how many items you have (in which case the cursor won't be advanced in the final step).
    // items_height: Use -1.0f to be calculated automatically on first step. Otherwise pass in the distance between your items, typically GetTextLineHeightWithSpacing() or GetFrameHeightWithSpacing().
    // If you don't specify an items_height, you NEED to call Step(). If you specify items_height you may call the old Begin()/End() api directly, but prefer calling Step().
    HanaLovesMeListClipper(int items_count = -1, float items_height = -1.0f) { Begin(items_count, items_height); } // NB: Begin() initialize every fields (as we allow user to call Begin/End multiple times on a same instance if they want).
    ~HanaLovesMeListClipper() { IM_ASSERT(ItemsCount == -1); }      // Assert if user forgot to call End() or Step() until false.

    HanaLovesMe_API bool Step();                                              // Call until it returns false. The DisplayStart/DisplayEnd fields will be set and you can process/draw those items.
    HanaLovesMe_API void Begin(int items_count, float items_height = -1.0f);  // Automatically called by constructor if you passed 'items_count' or by Step() in Step 1.
    HanaLovesMe_API void End();                                               // Automatically called on the last call of Step() that returns false.
};

// Helpers macros to generate 32-bits encoded colors
#ifdef HanaLovesMe_USE_BGRA_PACKED_COLOR
#define IM_COL32_R_SHIFT    16
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    0
#define IM_COL32_A_SHIFT    24
#define IM_COL32_A_MASK     0xFF000000
#else
#define IM_COL32_R_SHIFT    0
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    16
#define IM_COL32_A_SHIFT    24
#define IM_COL32_A_MASK     0xFF000000
#endif
#define IM_COL32(R,G,B,A)    (((ImU32)(A)<<IM_COL32_A_SHIFT) | ((ImU32)(B)<<IM_COL32_B_SHIFT) | ((ImU32)(G)<<IM_COL32_G_SHIFT) | ((ImU32)(R)<<IM_COL32_R_SHIFT))
#define IM_COL32_WHITE       IM_COL32(255,255,255,255)  // Opaque white = 0xFFFFFFFF
#define IM_COL32_BLACK       IM_COL32(0,0,0,255)        // Opaque black
#define IM_COL32_BLACK_TRANS IM_COL32(0,0,0,0)          // Transparent black = 0x00000000

// Helper: xuicolor() implicitly converts colors to either ImU32 (packed 4x1 byte) or xuifloatcolor (4x1 float)
// Prefer using IM_COL32() macros if you want a guaranteed compile-time ImU32 for usage with ImDrawList API.
// **Avoid storing xuicolor! Store either u32 of xuifloatcolor. This is not a full-featured color class. MAY OBSOLETE.
// **None of the HanaLovesMe API are using xuicolor directly but you can use it as a convenience to pass colors in either ImU32 or xuifloatcolor formats. Explicitly cast to ImU32 or xuifloatcolor if needed.
struct xuicolor
{
    xuifloatcolor              Value;

    xuicolor() { Value.x = Value.y = Value.z = Value.w = 0.0f; }
    xuicolor(int r, int g, int b, int a = 255) { float sc = 1.0f / 255.0f; Value.x = (float)r * sc; Value.y = (float)g * sc; Value.z = (float)b * sc; Value.w = (float)a * sc; }
    xuicolor(ImU32 rgba) { float sc = 1.0f / 255.0f; Value.x = (float)((rgba >> IM_COL32_R_SHIFT) & 0xFF) * sc; Value.y = (float)((rgba >> IM_COL32_G_SHIFT) & 0xFF) * sc; Value.z = (float)((rgba >> IM_COL32_B_SHIFT) & 0xFF) * sc; Value.w = (float)((rgba >> IM_COL32_A_SHIFT) & 0xFF) * sc; }
    xuicolor(float r, float g, float b, float a = 1.0f) { Value.x = r; Value.y = g; Value.z = b; Value.w = a; }
    xuicolor(const xuifloatcolor & col) { Value = col; }
    inline operator ImU32() const { return HanaLovesMe::ColorConvertFloat4ToU32(Value); }
    inline operator xuifloatcolor() const { return Value; }

    // FIXME-OBSOLETE: May need to obsolete/cleanup those helpers.
    inline void    SetHSV(float h, float s, float v, float a = 1.0f) { HanaLovesMe::ColorConvertHSVtoRGB(h, s, v, Value.x, Value.y, Value.z); Value.w = a; }
    static xuicolor HSV(float h, float s, float v, float a = 1.0f) { float r, g, b; HanaLovesMe::ColorConvertHSVtoRGB(h, s, v, r, g, b); return xuicolor(r, g, b, a); }
};

//-----------------------------------------------------------------------------
// Draw List API (ImDrawCmd, ImDrawIdx, ImDrawVert, ImDrawChannel, ImDrawListFlags, ImDrawList, ImDrawData)
// Hold a series of drawing commands. The user provides a renderer for ImDrawData which essentially contains an array of ImDrawList.
//-----------------------------------------------------------------------------

// Draw callbacks for advanced uses.
// NB: You most likely do NOT need to use draw callbacks just to create your own widget or customized UI rendering,
// you can poke into the draw list for that! Draw callback may be useful for example to: A) Change your GPU render state,
// B) render a complex 3D scene inside a UI element without an intermediate texture/render target, etc.
// The expected behavior from your rendering function is 'if (cmd.UserCallback != NULL) { cmd.UserCallback(parent_list, cmd); } else { RenderTriangles() }'
typedef void (*ImDrawCallback)(const ImDrawList* parent_list, const ImDrawCmd* cmd);

// Typically, 1 command = 1 GPU draw call (unless command is a callback)
struct ImDrawCmd
{
    unsigned int    ElemCount;              // Number of indices (multiple of 3) to be rendered as triangles. Vertices are stored in the callee ImDrawList's vtx_buffer[] array, indices in idx_buffer[].
    xuifloatcolor          ClipRect;               // Clipping rectangle (x1, y1, x2, y2). Subtract ImDrawData->DisplayPos to get clipping rectangle in "viewport" coordinates
    ImTextureID     TextureId;              // User-provided texture ID. Set by user in ImfontAtlas::SetTexID() for fonts or passed to Image*() functions. Ignore if never using images or multiple fonts atlas.
    ImDrawCallback  UserCallback;           // If != NULL, call the function instead of rendering the vertices. clip_rect and texture_id will be set normally.
    void* UserCallbackData;       // The draw callback code can access this.

    ImDrawCmd() { ElemCount = 0; ClipRect.x = ClipRect.y = ClipRect.z = ClipRect.w = 0.0f; TextureId = (ImTextureID)NULL; UserCallback = NULL; UserCallbackData = NULL; }
};

// Vertex index (override with '#define ImDrawIdx unsigned int' in imconfig.h)
#ifndef ImDrawIdx
typedef unsigned short ImDrawIdx;
#endif

// Vertex layout
#ifndef HanaLovesMe_OVERRIDE_DRAWVERT_STRUCT_LAYOUT
struct ImDrawVert
{
    vsize  pos;
    vsize  uv;
    ImU32   col;
};
#else
// You can override the vertex format layout by defining HanaLovesMe_OVERRIDE_DRAWVERT_STRUCT_LAYOUT in imconfig.h
// The code expect vsize pos (8 bytes), vsize uv (8 bytes), ImU32 col (4 bytes), but you can re-order them or add other fields as needed to simplify integration in your engine.
// The type has to be described within the macro (you can either declare the struct or use a typedef)
// NOTE: HanaLovesMe DOESN'T CLEAR THE STRUCTURE AND DOESN'T CALL A CONSTRUCTOR SO ANY CUSTOM FIELD WILL BE UNINITIALIZED. IF YOU ADD EXTRA FIELDS (SUCH AS A 'Z' COORDINATES) YOU WILL NEED TO CLEAR THEM DURING RENDER OR TO IGNORE THEM.
HanaLovesMe_OVERRIDE_DRAWVERT_STRUCT_LAYOUT;
#endif

// Draw channels are used by the Columns API to "split" the render list into different channels while building, so items of each column can be batched together.
// You can also use them to simulate drawing layers and submit primitives in a different order than how they will be rendered.
struct ImDrawChannel
{
    ImVector<ImDrawCmd>     CmdBuffer;
    ImVector<ImDrawIdx>     IdxBuffer;
};

enum ImDrawCornerFlags_
{
    ImDrawCornerFlags_TopLeft = 1 << 0, // 0x1
    ImDrawCornerFlags_TopRight = 1 << 1, // 0x2
    ImDrawCornerFlags_BotLeft = 1 << 2, // 0x4
    ImDrawCornerFlags_BotRight = 1 << 3, // 0x8
    ImDrawCornerFlags_Top = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_TopRight,   // 0x3
    ImDrawCornerFlags_Bot = ImDrawCornerFlags_BotLeft | ImDrawCornerFlags_BotRight,   // 0xC
    ImDrawCornerFlags_Left = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotLeft,    // 0x5
    ImDrawCornerFlags_Right = ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotRight,  // 0xA
    ImDrawCornerFlags_All = 0xF     // In your function calls you may use ~0 (= all bits sets) instead of ImDrawCornerFlags_All, as a convenience
};

enum ImDrawListFlags_
{
    ImDrawListFlags_None = 0,
    ImDrawListFlags_AntiAliasedLines = 1 << 0,  // Lines are anti-aliased (*2 the number of triangles for 1.0f wide line, otherwise *3 the number of triangles)
    ImDrawListFlags_AntiAliasedFill = 1 << 1   // Filled shapes have anti-aliased edges (*2 the number of vertices)
};

// Draw command list
// This is the low-level list of polygons that HanaLovesMe functions are filling. At the end of the frame, all command lists are passed to your HanaLovesMeIO::RenderDrawListFn function for rendering.
// Each HanaLovesMe window contains its own ImDrawList. You can use HanaLovesMe::GetWindowDrawList() to access the current window draw list and draw custom primitives.
// You can interleave normal HanaLovesMe:: calls and adding primitives to the current draw list.
// All positions are generally in pixel coordinates (top-left at (0,0), bottom-right at io.DisplaySize), but you are totally free to apply whatever transformation matrix to want to the data (if you apply such transformation you'll want to apply it to ClipRect as well)
// Important: Primitives are always added to the list and not culled (culling is done at higher-level by HanaLovesMe:: functions), if you use this API a lot consider coarse culling your drawn objects.
struct ImDrawList
{
    // This is what you have to render
    ImVector<ImDrawCmd>     CmdBuffer;          // Draw commands. Typically 1 command = 1 GPU draw call, unless the command is a callback.
    ImVector<ImDrawIdx>     IdxBuffer;          // Index buffer. Each command consume ImDrawCmd::ElemCount of those
    ImVector<ImDrawVert>    VtxBuffer;          // Vertex buffer.
    ImDrawListFlags         Flags;              // Flags, you may poke into these to adjust anti-aliasing settings per-primitive.

    // [Internal, used while building lists]
    const ImDrawListSharedData* _Data;          // Pointer to shared draw data (you can use HanaLovesMe::GetDrawListSharedData() to get the one from current HanaLovesMe context)
    const char* _OwnerName;         // Pointer to owner window's name for debugging
    unsigned int            _VtxCurrentIdx;     // [Internal] == VtxBuffer.Size
    ImDrawVert* _VtxWritePtr;       // [Internal] point within VtxBuffer.Data after each add command (to avoid using the ImVector<> operators too much)
    ImDrawIdx* _IdxWritePtr;       // [Internal] point within IdxBuffer.Data after each add command (to avoid using the ImVector<> operators too much)
    ImVector<xuifloatcolor>        _ClipRectStack;     // [Internal]
    ImVector<ImTextureID>   _TextureIdStack;    // [Internal]
    ImVector<vsize>        _Path;              // [Internal] current path building
    int                     _ChannelsCurrent;   // [Internal] current channel number (0)
    int                     _ChannelsCount;     // [Internal] number of active channels (1+)
    ImVector<ImDrawChannel> _Channels;          // [Internal] draw channels for columns API (not resized down so _ChannelsCount may be smaller than _Channels.Size)

    // If you want to create ImDrawList instances, pass them HanaLovesMe::GetDrawListSharedData() or create and use your own ImDrawListSharedData (so you can use ImDrawList without HanaLovesMe)
    ImDrawList(const ImDrawListSharedData* shared_data) { _Data = shared_data; _OwnerName = NULL; Clear(); }
    ~ImDrawList() { ClearFreeMemory(); }
    HanaLovesMe_API void  PushClipRect(vsize clip_rect_min, vsize clip_rect_max, bool intersect_with_current_clip_rect = false);  // Render-level scissoring. This is passed down to your render function but not used for CPU-side coarse clipping. Prefer using higher-level HanaLovesMe::PushClipRect() to affect logic (hit-testing and widget culling)
    HanaLovesMe_API void  PushClipRectFullScreen();
    HanaLovesMe_API void  PopClipRect();
    HanaLovesMe_API void  PushTextureID(ImTextureID texture_id);
    HanaLovesMe_API void  PopTextureID();
    inline vsize   GetClipRectMin() const { const xuifloatcolor& cr = _ClipRectStack.back(); return vsize(cr.x, cr.y); }
    inline vsize   GetClipRectMax() const { const xuifloatcolor& cr = _ClipRectStack.back(); return vsize(cr.z, cr.w); }

    // Primitives
    HanaLovesMe_API void  AddLine(const vsize& a, const vsize& b, ImU32 col, float thickness = 1.0f);
    HanaLovesMe_API void  AddRect(const vsize& a, const vsize& b, ImU32 col, float rounding = 0.0f, int rounding_corners_flags = ImDrawCornerFlags_All, float thickness = 1.0f);   // a: upper-left, b: lower-right (== upper-left + size), rounding_corners_flags: 4-bits corresponding to which corner to round
    HanaLovesMe_API void  AddRectFilled(const vsize& a, const vsize& b, ImU32 col, float rounding = 0.0f, int rounding_corners_flags = ImDrawCornerFlags_All);                     // a: upper-left, b: lower-right (== upper-left + size)
    HanaLovesMe_API void  AddRectFilledMultiColor(const vsize& a, const vsize& b, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left);
    HanaLovesMe_API void  AddQuad(const vsize& a, const vsize& b, const vsize& c, const vsize& d, ImU32 col, float thickness = 1.0f);
    HanaLovesMe_API void  AddQuadFilled(const vsize& a, const vsize& b, const vsize& c, const vsize& d, ImU32 col);
    HanaLovesMe_API void  AddTriangle(const vsize& a, const vsize& b, const vsize& c, ImU32 col, float thickness = 1.0f);
    HanaLovesMe_API void  AddTriangleFilled(const vsize& a, const vsize& b, const vsize& c, ImU32 col);
    HanaLovesMe_API void  AddCircle(const vsize& centre, float radius, ImU32 col, int num_segments = 12, float thickness = 1.0f);
    HanaLovesMe_API void  AddCircleFilled(const vsize& centre, float radius, ImU32 col, int num_segments = 12);
    HanaLovesMe_API void  AddText(const vsize& pos, ImU32 col, const char* text_begin, const char* text_end = NULL);
    HanaLovesMe_API void  AddText(const ImFont* font, float font_size, const vsize& pos, ImU32 col, const char* text_begin, const char* text_end = NULL, float wrap_width = 0.0f, const xuifloatcolor* cpu_fine_clip_rect = NULL);
    HanaLovesMe_API void  AddImage(ImTextureID user_texture_id, const vsize& a, const vsize& b, const vsize& uv_a = vsize(0, 0), const vsize& uv_b = vsize(1, 1), ImU32 col = IM_COL32_WHITE);
    HanaLovesMe_API void  AddImageQuad(ImTextureID user_texture_id, const vsize& a, const vsize& b, const vsize& c, const vsize& d, const vsize& uv_a = vsize(0, 0), const vsize& uv_b = vsize(1, 0), const vsize& uv_c = vsize(1, 1), const vsize& uv_d = vsize(0, 1), ImU32 col = IM_COL32_WHITE);
    HanaLovesMe_API void  AddImageRounded(ImTextureID user_texture_id, const vsize& a, const vsize& b, const vsize& uv_a, const vsize& uv_b, ImU32 col, float rounding, int rounding_corners = ImDrawCornerFlags_All);
    HanaLovesMe_API void  AddPolyline(const vsize* points, int num_points, ImU32 col, bool closed, float thickness);
    HanaLovesMe_API void  AddConvexPolyFilled(const vsize* points, int num_points, ImU32 col); // Note: Anti-aliased filling requires points to be in clockwise order.
    HanaLovesMe_API void  AddBezierCurve(const vsize& pos0, const vsize& cp0, const vsize& cp1, const vsize& pos1, ImU32 col, float thickness, int num_segments = 0);

    // Stateful path API, add points then finish with PathFillConvex() or PathStroke()
    inline    void  PathClear() { _Path.Size = 0; }
    inline    void  PathLineTo(const vsize& pos) { _Path.push_back(pos); }
    inline    void  PathLineToMergeDuplicate(const vsize& pos) { if (_Path.Size == 0 || memcmp(&_Path.Data[_Path.Size - 1], &pos, 8) != 0) _Path.push_back(pos); }
    inline    void  PathFillConvex(ImU32 col) { AddConvexPolyFilled(_Path.Data, _Path.Size, col); _Path.Size = 0; }  // Note: Anti-aliased filling requires points to be in clockwise order.
    inline    void  PathStroke(ImU32 col, bool closed, float thickness = 1.0f) { AddPolyline(_Path.Data, _Path.Size, col, closed, thickness); _Path.Size = 0; }
    HanaLovesMe_API void  PathArcTo(const vsize & centre, float radius, float a_min, float a_max, int num_segments = 10);
    HanaLovesMe_API void  PathArcToFast(const vsize & centre, float radius, int a_min_of_12, int a_max_of_12);                                            // Use precomputed angles for a 12 steps circle
    HanaLovesMe_API void  PathBezierCurveTo(const vsize & p1, const vsize & p2, const vsize & p3, int num_segments = 0);
    HanaLovesMe_API void  PathRect(const vsize & rect_min, const vsize & rect_max, float rounding = 0.0f, int rounding_corners_flags = ImDrawCornerFlags_All);

    // Channels
    // - Use to simulate layers. By switching channels to can render out-of-order (e.g. submit foreground primitives before background primitives)
    // - Use to minimize draw calls (e.g. if going back-and-forth between multiple non-overlapping clipping rectangles, prefer to append into separate channels then merge at the end)
    HanaLovesMe_API void  ChannelsSplit(int channels_count);
    HanaLovesMe_API void  ChannelsMerge();
    HanaLovesMe_API void  ChannelsSetCurrent(int channel_index);

    // Advanced
    HanaLovesMe_API void  AddCallback(ImDrawCallback callback, void* callback_data);  // Your rendering function must check for 'UserCallback' in ImDrawCmd and call the function instead of rendering triangles.
    HanaLovesMe_API void  AddDrawCmd();                                               // This is useful if you need to forcefully create a new draw call (to allow for dependent rendering / blending). Otherwise primitives are merged into the same draw-call as much as possible
    HanaLovesMe_API ImDrawList * CloneOutput() const;                                  // Create a clone of the CmdBuffer/IdxBuffer/VtxBuffer.

    // Internal helpers
    // NB: all primitives needs to be reserved via PrimReserve() beforehand!
    HanaLovesMe_API void  Clear();
    HanaLovesMe_API void  ClearFreeMemory();
    HanaLovesMe_API void  PrimReserve(int idx_count, int vtx_count);
    HanaLovesMe_API void  Prvsrect(const vsize & a, const vsize & b, ImU32 col);      // Axis aligned rectangle (composed of two triangles)
    HanaLovesMe_API void  PrvsrectUV(const vsize & a, const vsize & b, const vsize & uv_a, const vsize & uv_b, ImU32 col);
    HanaLovesMe_API void  PrimQuadUV(const vsize & a, const vsize & b, const vsize & c, const vsize & d, const vsize & uv_a, const vsize & uv_b, const vsize & uv_c, const vsize & uv_d, ImU32 col);
    inline    void  PrimWriteVtx(const vsize & pos, const vsize & uv, ImU32 col) { _VtxWritePtr->pos = pos; _VtxWritePtr->uv = uv; _VtxWritePtr->col = col; _VtxWritePtr++; _VtxCurrentIdx++; }
    inline    void  PrimWriteIdx(ImDrawIdx idx) { *_IdxWritePtr = idx; _IdxWritePtr++; }
    inline    void  PrimVtx(const vsize & pos, const vsize & uv, ImU32 col) { PrimWriteIdx((ImDrawIdx)_VtxCurrentIdx); PrimWriteVtx(pos, uv, col); }
    HanaLovesMe_API void  UpdateClipRect();
    HanaLovesMe_API void  UpdateTextureID();
};

// All draw data to render an HanaLovesMe frame
// (NB: the style and the naming convention here is a little inconsistent but we preserve them for backward compatibility purpose)
struct ImDrawData
{
    bool            Valid;                  // Only valid after Render() is called and before the next NewFrame() is called.
    ImDrawList** CmdLists;               // Array of ImDrawList* to render. The ImDrawList are owned by HanaLovesMeContext and only pointed to from here.
    int             CmdListsCount;          // Number of ImDrawList* to render
    int             TotalIdxCount;          // For convenience, sum of all ImDrawList's IdxBuffer.Size
    int             TotalVtxCount;          // For convenience, sum of all ImDrawList's VtxBuffer.Size
    vsize          DisplayPos;             // Upper-left position of the viewport to render (== upper-left of the orthogonal projection matrix to use)
    vsize          DisplaySize;            // Size of the viewport to render (== io.DisplaySize for the main viewport) (DisplayPos + DisplaySize == lower-right of the orthogonal projection matrix to use)
    vsize          FramebufferScale;       // Amount of pixels for each unit of DisplaySize. Based on io.DisplayFramebufferScale. Generally (1,1) on normal display, (2,2) on OSX with Retina display.

    // Functions
    ImDrawData() { Valid = false; Clear(); }
    ~ImDrawData() { Clear(); }
    void Clear() { Valid = false; CmdLists = NULL; CmdListsCount = TotalVtxCount = TotalIdxCount = 0; DisplayPos = DisplaySize = FramebufferScale = vsize(0.f, 0.f); } // The ImDrawList are owned by HanaLovesMeContext!
    HanaLovesMe_API void  DeIndexAllBuffers();                    // Helper to convert all buffers from indexed to non-indexed, in case you cannot render indexed. Note: this is slow and most likely a waste of resources. Always prefer indexed rendering!
    HanaLovesMe_API void  ScaleClipRects(const vsize& fb_scale); // Helper to scale the ClipRect field of each ImDrawCmd. Use if your final output buffer is at a different scale than HanaLovesMe expects, or if there is a difference between your window resolution and framebuffer resolution.
};

//-----------------------------------------------------------------------------
// Font API (ImFontConfig, ImFontGlyph, ImFontAtlasFlags, ImFontAtlas, ImFontGlyphRangesBuilder, ImFont)
//-----------------------------------------------------------------------------

struct ImFontConfig
{
    void* FontData;               //          // TTF/OTF data
    int             FontDataSize;           //          // TTF/OTF data size
    bool            FontDataOwnedByAtlas;   // true     // TTF/OTF data ownership taken by the container ImFontAtlas (will delete memory itself).
    int             FontNo;                 // 0        // Index of font within TTF/OTF file
    float           SizePixels;             //          // Size in pixels for rasterizer (more or less maps to the resulting font height).
    int             OversampleH;            // 3        // Rasterize at higher quality for sub-pixel positioning. Read https://github.com/nothings/stb/blob/master/tests/oversample/README.md for details.
    int             OversampleV;            // 1        // Rasterize at higher quality for sub-pixel positioning. We don't use sub-pixel positions on the Y axis.
    bool            PixelSnapH;             // false    // Align every glyph to pixel boundary. Useful e.g. if you are merging a non-pixel aligned font with the default font. If enabled, you can set OversampleH/V to 1.
    vsize          GlyphExtraSpacing;      // 0, 0     // Extra spacing (in pixels) between glyphs. Only X axis is supported for now.
    vsize          GlyphOffset;            // 0, 0     // Offset all glyphs from this font input.
    const ImWchar* GlyphRanges;            // NULL     // Pointer to a user-provided list of Unicode range (2 value per range, values are inclusive, zero-terminated list). THE ARRAY DATA NEEDS TO PERSIST AS LONG AS THE FONT IS ALIVE.
    float           GlyphMinAdvanceX;       // 0        // Minimum AdvanceX for glyphs, set Min to align font icons, set both Min/Max to enforce mono-space font
    float           GlyphMaxAdvanceX;       // FLT_MAX  // Maximum AdvanceX for glyphs
    bool            MergeMode;              // false    // Merge into previous ImFont, so you can combine multiple inputs font into one ImFont (e.g. ASCII font + icons + Japanese glyphs). You may want to use GlyphOffset.y when merge font of different heights.
    unsigned int    RasterizerFlags;        // 0x00     // Settings for custom font rasterizer (e.g. HanaLovesMeFreeType). Leave as zero if you aren't using one.
    float           RasterizerMultiply;     // 1.0f     // Brighten (>1.0f) or darken (<1.0f) font output. Brightening small fonts may be a good workaround to make them more readable.

    // [Internal]
    char            Name[40];               // Name (strictly to ease debugging)
    ImFont* DstFont;

    HanaLovesMe_API ImFontConfig();
};

struct ImFontGlyph
{
    ImWchar         Codepoint;          // 0x0000..0xFFFF
    float           AdvanceX;           // Distance to next character (= data from font + ImFontConfig::GlyphExtraSpacing.x baked in)
    float           X0, Y0, X1, Y1;     // Glyph corners
    float           U0, V0, U1, V1;     // Texture coordinates
};

// Helper to build glyph ranges from text/string data. Feed your application strings/characters to it then call BuildRanges().
// This is essentially a tightly packed of vector of 64k booleans = 8KB stohnly.
struct ImFontGlyphRangesBuilder
{
    ImVector<int> UsedChars;            // Store 1-bit per Unicode code point (0=unused, 1=used)

    ImFontGlyphRangesBuilder() { UsedChars.resize(0x10000 / sizeof(int)); memset(UsedChars.Data, 0, 0x10000 / sizeof(int)); }
    bool            GetBit(int n) const { int off = (n >> 5); int mask = 1 << (n & 31); return (UsedChars[off] & mask) != 0; }  // Get bit n in the array
    void            SetBit(int n) { int off = (n >> 5); int mask = 1 << (n & 31); UsedChars[off] |= mask; }               // Set bit n in the array
    void            AddChar(ImWchar c) { SetBit(c); }                          // Add character
    HanaLovesMe_API void  AddText(const char* text, const char* text_end = NULL);     // Add string (each character of the UTF-8 string are added)
    HanaLovesMe_API void  AddRanges(const ImWchar * ranges);                           // Add ranges, e.g. builder.AddRanges(ImFontAtlas::GetGlyphRangesDefault()) to force add all of ASCII/Latin+Ext
    HanaLovesMe_API void  BuildRanges(ImVector<ImWchar> * out_ranges);                 // Output new ranges
};

enum ImFontAtlasFlags_
{
    ImFontAtlasFlags_None = 0,
    ImFontAtlasFlags_NoPowerOfTwoHeight = 1 << 0,   // Don't round the height to next power of two
    ImFontAtlasFlags_NoMouseCursors = 1 << 1    // Don't build software mouse cursors into the atlas
};

// Load and rasterize multiple TTF/OTF fonts into a same texture. The font atlas will build a single texture holding:
//  - One or more fonts.
//  - Custom graphics data needed to render the shapes needed by Dear HanaLovesMe.
//  - Mouse cursor shapes for software cursor rendering (unless setting 'Flags |= ImFontAtlasFlags_NoMouseCursors' in the font atlas).
// It is the user-code responsibility to setup/build the atlas, then upload the pixel data into a texture accessible by your graphics api.
//  - Optionally, call any of the AddFont*** functions. If you don't call any, the default font embedded in the code will be loaded for you.
//  - Call GetTexDataAsAlpha8() or GetTexDataAsRGBA32() to build and retrieve pixels data.
//  - Upload the pixels data into a texture within your graphics system (see HanaLovesMe_impl_xxxx.cpp examples)
//  - Call SetTexID(my_tex_id); and pass the pointer/identifier to your texture in a format natural to your graphics API.
//    This value will be passed back to you during rendering to identify the texture. Read FAQ entry about ImTextureID for more details.
// Common pitfalls:
// - If you pass a 'glyph_ranges' array to AddFont*** functions, you need to make sure that your array persist up until the
//   atlas is build (when calling GetTexData*** or Build()). We only copy the pointer, not the data.
// - Important: By default, AddFontFromMemoryTTF() takes ownership of the data. Even though we are not writing to it, we will free the pointer on destruction.
//   You can set font_cfg->FontDataOwnedByAtlas=false to keep ownership of your data and it won't be freed,
// - Even though many functions are suffixed with "TTF", OTF data is supported just as well.
// - This is an old API and it is currently awkward for those and and various other reasons! We will address them in the future!
struct ImFontAtlas
{
    HanaLovesMe_API ImFontAtlas();
    HanaLovesMe_API ~ImFontAtlas();
    HanaLovesMe_API ImFont* AddFont(const ImFontConfig* font_cfg);
    HanaLovesMe_API ImFont* AddFontDefault(const ImFontConfig* font_cfg = NULL);
    HanaLovesMe_API ImFont* AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);
    HanaLovesMe_API ImFont* AddFontFromMemoryTTF(void* font_data, int font_size, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL); // Note: Transfer ownership of 'ttf_data' to ImFontAtlas! Will be deleted after destruction of the atlas. Set font_cfg->FontDataOwnedByAtlas=false to keep ownership of your data and it won't be freed.
    HanaLovesMe_API ImFont* AddFontFromMemoryCompressedTTF(const void* compressed_font_data, int compressed_font_size, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL); // 'compressed_font_data' still owned by caller. Compress with binary_to_compressed_c.cpp.
    HanaLovesMe_API ImFont* AddFontFromMemoryCompressedBase85TTF(const char* compressed_font_data_base85, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);              // 'compressed_font_data_base85' still owned by caller. Compress with binary_to_compressed_c.cpp with -base85 parameter.
    HanaLovesMe_API void              ClearInputData();           // Clear input data (all ImFontConfig structures including sizes, TTF data, glyph ranges, etc.) = all the data used to build the texture and fonts.
    HanaLovesMe_API void              ClearTexData();             // Clear output texture data (CPU side). Saves RAM once the texture has been copied to graphics memory.
    HanaLovesMe_API void              ClearFonts();               // Clear output font data (glyphs stohnly, UV coordinates).
    HanaLovesMe_API void              Clear();                    // Clear all input and output.

    // Build atlas, retrieve pixel data.
    // User is in charge of copying the pixels into graphics memory (e.g. create a texture with your engine). Then store your texture handle with SetTexID().
    // The pitch is always = Width * BytesPerPixels (1 or 4)
    // Building in RGBA32 format is provided for convenience and compatibility, but note that unless you manually manipulate or copy color data into
    // the texture (e.g. when using the AddCustomRect*** api), then the RGB pixels emitted will always be white (~75% of memory/bandwidth waste.
    HanaLovesMe_API bool              Build();                    // Build pixels data. This is called automatically for you by the GetTexData*** functions.
    HanaLovesMe_API void              GetTexDataAsAlpha8(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel = NULL);  // 1 byte per-pixel
    HanaLovesMe_API void              GetTexDataAsRGBA32(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel = NULL);  // 4 bytes-per-pixel
    bool                        IsBuilt() { return Fonts.Size > 0 && (TexPixelsAlpha8 != NULL || TexPixelsRGBA32 != NULL); }
    void                        SetTexID(ImTextureID id) { TexID = id; }

    //-------------------------------------------
    // Glyph Ranges
    //-------------------------------------------

    // Helpers to retrieve list of common Unicode ranges (2 value per range, values are inclusive, zero-terminated list)
    // NB: Make sure that your string are UTF-8 and NOT in your local code page. In C++11, you can create UTF-8 string literal using the u8"Hello world" syntax. See FAQ for details.
    // NB: Consider using ImFontGlyphRangesBuilder to build glyph ranges from textual data.
    HanaLovesMe_API const ImWchar* GetGlyphRangesDefault();                // Basic Latin, Extended Latin
    HanaLovesMe_API const ImWchar* GetGlyphRangesKorean();                 // Default + Korean characters
    HanaLovesMe_API const ImWchar* GetGlyphRangesJapanese();               // Default + Hiragana, Katakana, Half-Width, Selection of 1946 Ideographs
    HanaLovesMe_API const ImWchar* GetGlyphRangesChineseFull();            // Default + Half-Width + Japanese Hiragana/Katakana + full set of about 21000 CJK Unified Ideographs
    HanaLovesMe_API const ImWchar* GetGlyphRangesChineseSimplifiedCommon();// Default + Half-Width + Japanese Hiragana/Katakana + set of 2500 CJK Unified Ideographs for common simplified Chinese
    HanaLovesMe_API const ImWchar* GetGlyphRangesCyrillic();               // Default + about 400 Cyrillic characters
    HanaLovesMe_API const ImWchar* GetGlyphRangesThai();                   // Default + Thai characters
    HanaLovesMe_API const ImWchar* GetGlyphRangesVietnamese();             // Default + Vietname characters

    //-------------------------------------------
    // Custom Rectangles/Glyphs API
    //-------------------------------------------

    // You can request arbitrary rectangles to be packed into the atlas, for your own purposes. After calling Build(), you can query the rectangle position and render your pixels.
    // You can also request your rectangles to be mapped as font glyph (given a font + Unicode point), so you can render e.g. custom colorful icons and use them as regular glyphs.
    struct CustomRect
    {
        unsigned int    ID;             // Input    // User ID. Use <0x10000 to map into a font glyph, >=0x10000 for other/internal/custom texture data.
        unsigned short  Width, Height;  // Input    // Desired rectangle dimension
        unsigned short  X, Y;           // Output   // Packed position in Atlas
        float           GlyphAdvanceX;  // Input    // For custom font glyphs only (ID<0x10000): glyph xadvance
        vsize          GlyphOffset;    // Input    // For custom font glyphs only (ID<0x10000): glyph display offset
        ImFont* Font;           // Input    // For custom font glyphs only (ID<0x10000): target font
        CustomRect() { ID = 0xFFFFFFFF; Width = Height = 0; X = Y = 0xFFFF; GlyphAdvanceX = 0.0f; GlyphOffset = vsize(0, 0); Font = NULL; }
        bool IsPacked() const { return X != 0xFFFF; }
    };

    HanaLovesMe_API int       AddCustomRectRegular(unsigned int id, int width, int height);                                                                   // Id needs to be >= 0x10000. Id >= 0x80000000 are reserved for HanaLovesMe and ImDrawList
    HanaLovesMe_API int       AddCustomRectFontGlyph(ImFont * font, ImWchar id, int width, int height, float advance_x, const vsize & offset = vsize(0, 0));   // Id needs to be < 0x10000 to register a rectangle to map into a specific font.
    const CustomRect * GetCustomRectByIndex(int index) const { if (index < 0) return NULL; return &CustomRects[index]; }

    // [Internal]
    HanaLovesMe_API void      CalcCustomRectUV(const CustomRect * rect, vsize * out_uv_min, vsize * out_uv_max);
    HanaLovesMe_API bool      GetMouseCursorTexData(HanaLovesMeMouseCursor cursor, vsize * out_offset, vsize * out_size, vsize out_uv_border[2], vsize out_uv_fill[2]);

    //-------------------------------------------
    // Members
    //-------------------------------------------

    bool                        Locked;             // Marked as Locked by HanaLovesMe::NewFrame() so attempt to modify the atlas will assert.
    ImFontAtlasFlags            Flags;              // Build flags (see ImFontAtlasFlags_)
    ImTextureID                 TexID;              // User data to refer to the texture once it has been uploaded to user's graphic systems. It is passed back to you during rendering via the ImDrawCmd structure.
    int                         TexDesiredWidth;    // Texture width desired by user before Build(). Must be a power-of-two. If have many glyphs your graphics API have texture size restrictions you may want to increase texture width to decrease height.
    int                         TexGlyphPadding;    // Padding between glyphs within texture in pixels. Defaults to 1. If your rendering method doesn't rely on bilinear filtering you may set this to 0.

    // [Internal]
    // NB: Access texture data via GetTexData*() calls! Which will setup a default font for you.
    unsigned char* TexPixelsAlpha8;    // 1 component per pixel, each component is unsigned 8-bit. Total size = TexWidth * TexHeight
    unsigned int* TexPixelsRGBA32;    // 4 component per pixel, each component is unsigned 8-bit. Total size = TexWidth * TexHeight * 4
    int                         TexWidth;           // Texture width calculated during Build().
    int                         TexHeight;          // Texture height calculated during Build().
    vsize                      TexUvScale;         // = (1.0f/TexWidth, 1.0f/TexHeight)
    vsize                      TexUvWhitePixel;    // Texture coordinates to a white pixel
    ImVector<ImFont*>           Fonts;              // Hold all the fonts returned by AddFont*. Fonts[0] is the default font upon calling HanaLovesMe::NewFrame(), use HanaLovesMe::PushFont()/PopFont() to change the current font.
    ImVector<CustomRect>        CustomRects;        // Rectangles for packing custom texture data into the atlas.
    ImVector<ImFontConfig>      ConfigData;         // Internal data
    int                         CustomRectIds[1];   // Identifiers of custom texture rectangle used by ImFontAtlas/ImDrawList

#ifndef HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS
    typedef ImFontGlyphRangesBuilder GlyphRangesBuilder; // OBSOLETE 1.67+
#endif
};

// Font runtime data and rendering
// ImFontAtlas automatically loads a default embedded font for you when you call GetTexDataAsAlpha8() or GetTexDataAsRGBA32().
struct ImFont
{
    // Members: Hot ~20/24 bytes (for CalcTextSize)
    ImVector<float>             IndexAdvanceX;      // 12-16 // out //            // Sparse. Glyphs->AdvanceX in a directly indexable way (cache-friendly for CalcTextSize functions which only this this info, and are often bottleneck in large UI).
    float                       FallbackAdvanceX;   // 4     // out // = FallbackGlyph->AdvanceX
    float                       FontSize;           // 4     // in  //            // Height of characters/line, set during loading (don't change after loading)

    // Members: Hot ~36/48 bytes (for CalcTextSize + render loop)
    ImVector<ImWchar>           IndexLookup;        // 12-16 // out //            // Sparse. Index glyphs by Unicode code-point.
    ImVector<ImFontGlyph>       Glyphs;             // 12-16 // out //            // All glyphs.
    const ImFontGlyph* FallbackGlyph;      // 4-8   // out // = FindGlyph(FontFallbackChar)
    vsize                      DisplayOffset;      // 8     // in  // = (0,0)    // Offset font rendering by xx pixels

    // Members: Cold ~32/40 bytes
    ImFontAtlas* ContainerAtlas;     // 4-8   // out //            // What we has been loaded into
    const ImFontConfig* ConfigData;         // 4-8   // in  //            // Pointer within ContainerAtlas->ConfigData
    short                       ConfigDataCount;    // 2     // in  // ~ 1        // Number of ImFontConfig involved in creating this font. Bigger than 1 when merging multiple font sources into one ImFont.
    ImWchar                     FallbackChar;       // 2     // in  // = '?'      // Replacement glyph if one isn't found. Only set via SetFallbackChar()
    float                       Scale;              // 4     // in  // = 1.f      // Base font scale, multiplied by the per-window font scale which you can adjust with SetWindowFontScale()
    float                       Ascent, Descent;    // 4+4   // out //            // Ascent: distance from top to bottom of e.g. 'A' [0..FontSize]
    int                         MetricsTotalSurface;// 4     // out //            // Total surface in pixels to get an idea of the font rasterization/texture cost (not exact, we approximate the cost of padding between glyphs)
    bool                        DirtyLookupTables;  // 1     // out //

    // Methods
    HanaLovesMe_API ImFont();
    HanaLovesMe_API ~ImFont();
    HanaLovesMe_API const ImFontGlyph* FindGlyph(ImWchar c) const;
    HanaLovesMe_API const ImFontGlyph* FindGlyphNoFallback(ImWchar c) const;
    float                       GetCharAdvance(ImWchar c) const { return ((int)c < IndexAdvanceX.Size) ? IndexAdvanceX[(int)c] : FallbackAdvanceX; }
    bool                        IsLoaded() const { return ContainerAtlas != NULL; }
    const char* GetDebugName() const { return ConfigData ? ConfigData->Name : "<unknown>"; }

    // 'max_width' stops rendering after a certain width (could be turned into a 2d size). FLT_MAX to disable.
    // 'wrap_width' enable automatic word-wrapping across multiple lines to fit into given width. 0.0f to disable.
    HanaLovesMe_API vsize            CalcTextSizeA(float size, float max_width, float wrap_width, const char* text_begin, const char* text_end = NULL, const char** remaining = NULL) const; // utf8
    HanaLovesMe_API const char* CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width) const;
    HanaLovesMe_API void              RenderChar(ImDrawList* draw_list, float size, vsize pos, ImU32 col, ImWchar c) const;
    HanaLovesMe_API void              RenderText(ImDrawList* draw_list, float size, vsize pos, ImU32 col, const xuifloatcolor& clip_rect, const char* text_begin, const char* text_end, float wrap_width = 0.0f, bool cpu_fine_clip = false) const;

    // [Internal] Don't use!
    HanaLovesMe_API void              BuildLookupTable();
    HanaLovesMe_API void              ClearOutputData();
    HanaLovesMe_API void              GrowIndex(int new_size);
    HanaLovesMe_API void              AddGlyph(ImWchar c, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, float advance_x);
    HanaLovesMe_API void              AddRemapChar(ImWchar dst, ImWchar src, bool overwrite_dst = true); // Makes 'dst' character/glyph points to 'src' character/glyph. Currently needs to be called AFTER fonts have been built.
    HanaLovesMe_API void              SetFallbackChar(ImWchar c);

#ifndef HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS
    typedef ImFontGlyph Glyph; // OBSOLETE 1.52+
#endif
};

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__) && __GNUC__ >= 8
#pragma GCC diagnostic pop
#endif

// Include HanaLovesMe_user.h at the end of HanaLovesMe.h (convenient for user to only explicitly include vanilla HanaLovesMe.h)
#ifdef HanaLovesMe_INCLUDE_HanaLovesMe_USER_H
#include "HanaLovesMe_user.h"
#endif
