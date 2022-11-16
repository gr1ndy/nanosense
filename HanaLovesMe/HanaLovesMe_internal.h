// dear HanaLovesMe, v1.70 WIP
// (internal structures/api)

// You may use this file to debug, understand or extend HanaLovesMe features but we don't provide any guarantee of forward compatibility!
// Set:
//   #define HanaLovesMe_DEFINE_MATH_OPERATORS
// To implement maths operators for vsize (disabled by default to not collide with using IM_VEC2_CLASS_EXTRA along with your own math types+operators)
#define HanaLovesMe_DEFINE_MATH_OPERATORS
/*

Index of this file:
// Header mess
// Forward declarations
// STB libraries includes
// Context pointer
// Generic helpers
// Misc data structures
// Main HanaLovesMe context
// Tab bar, tab item
// Internal API

*/

#pragma once

//-----------------------------------------------------------------------------
// Header mess
//-----------------------------------------------------------------------------

#ifndef HanaLovesMe_VERSION
#error Must include HanaLovesMe.h before HanaLovesMe_internal.h
#endif

#include <stdio.h>      // FILE*
#include <stdlib.h>     // NULL, malloc, free, qsort, atoi, atof
#include <math.h>       // sqrtf, fabsf, fmodf, powf, floorf, ceilf, cosf, sinf
#include <limits.h>     // INT_MIN, INT_MAX

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4251) // class 'xxx' needs to have dll-interface to be used by clients of struct 'xxx' // when HanaLovesMe_API is set to__declspec(dllexport)
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"                // for stb_textedit.h
#pragma clang diagnostic ignored "-Wmissing-prototypes"             // for stb_textedit.h
#pragma clang diagnostic ignored "-Wold-style-cast"
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#if __has_warning("-Wdouble-promotion")
#pragma clang diagnostic ignored "-Wdouble-promotion"
#endif
#endif

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------

struct vsrect;                      // An axis-aligned rectangle (2 points)
struct ImDrawDataBuilder;           // Helper to build a ImDrawData instance
struct ImDrawListSharedData;        // Data shared between all ImDrawList instances
struct HanaLovesMeColorMod;               // Stacked color modifier, backup of modified data so we can restore it
struct HanaLovesMeColumnData;             // Stohnly data for a single column
struct HanaLovesMeColumns;                // Stohnly data for a columns set
struct HanaLovesMeContext;                // Main HanaLovesMe context
struct HanaLovesMeGroupData;              // Stacked stohnly data for BeginGroup()/EndGroup()
struct HanaLovesMeInputTextState;         // Internal state of the currently focused/edited text input box
struct HanaLovesMeItemHoveredDataBackup;  // Backup and restore IsItemHovered() internal data
struct HanaLovesMeMenuColumns;            // Simple column measurement, currently used for MenuItem() only
struct HanaLovesMeNavMoveResult;          // Result of a directional navigation move query result
struct HanaLovesMeNextWindowData;         // Stohnly for SetNexWindow** functions
struct HanaLovesMePopupRef;               // Stohnly for current popup stack
struct HanaLovesMeSettingsHandler;        // Stohnly for one type registered in the .ini file
struct HanaLovesMeStyleMod;               // Stacked style modifier, backup of modified data so we can restore it
struct HanaLovesMeTabBar;                 // Stohnly for a tab bar
struct HanaLovesMeTabItem;                // Stohnly for a tab item (within a tab bar)
struct HanaLovesMeWindow;                 // Stohnly for one window
struct HanaLovesMeWindowTempData;         // Temporary stohnly for one window (that's the data which in theory we could ditch at the end of the frame)
struct HanaLovesMeWindowSettings;         // Stohnly for window settings stored in .ini file (we keep one of those even if the actual window wasn't instanced during this session)

// Use your programming IDE "Go to definition" facility on the names of the center columns to find the actual flags/enum lists.
typedef int HanaLovesMeLayoutType;        // -> enum HanaLovesMeLayoutType_        // Enum: Horizontal or vertical
typedef int HanaLovesMeButtonFlags;       // -> enum HanaLovesMeButtonFlags_       // Flags: for ButtonEx(), ButtonBehavior()
typedef int HanaLovesMeDragFlags;         // -> enum HanaLovesMeDragFlags_         // Flags: for DragBehavior()
typedef int HanaLovesMeItemFlags;         // -> enum HanaLovesMeItemFlags_         // Flags: for PushItemFlag()
typedef int HanaLovesMeItemStatusFlags;   // -> enum HanaLovesMeItemStatusFlags_   // Flags: for DC.LastItemStatusFlags
typedef int HanaLovesMeNavHighlightFlags; // -> enum HanaLovesMeNavHighlightFlags_ // Flags: for RenderNavHighlight()
typedef int HanaLovesMeNavDirSourceFlags; // -> enum HanaLovesMeNavDirSourceFlags_ // Flags: for GetNavInputAmount2d()
typedef int HanaLovesMeNavMoveFlags;      // -> enum HanaLovesMeNavMoveFlags_      // Flags: for navigation requests
typedef int HanaLovesMeSeparatorFlags;    // -> enum HanaLovesMeSeparatorFlags_    // Flags: for Separator() - internal
typedef int HanaLovesMeSliderFlags;       // -> enum HanaLovesMeSliderFlags_       // Flags: for SliderBehavior()
typedef int HanaLovesMeTextFlags;         // -> enum HanaLovesMeTextFlags_         // Flags: for TextEx()

//-------------------------------------------------------------------------
// STB libraries includes
//-------------------------------------------------------------------------

namespace x07dns
{

#undef STB_TEXTEDIT_STRING
#undef STB_TEXTEDIT_CHARTYPE
#define STB_TEXTEDIT_STRING             HanaLovesMeInputTextState
#define STB_TEXTEDIT_CHARTYPE           ImWchar
#define STB_TEXTEDIT_GETWIDTH_NEWLINE   -1.0f
#define STB_TEXTEDIT_UNDOSTATECOUNT     99
#define STB_TEXTEDIT_UNDOCHARCOUNT      999
#include "x07dns_textedit.h"

} // namespace x07dns

//-----------------------------------------------------------------------------
// Context pointer
//-----------------------------------------------------------------------------

#ifndef GHanaLovesMe
extern HanaLovesMe_API HanaLovesMeContext* GHanaLovesMe;  // Current implicit HanaLovesMe context pointer
#endif

//-----------------------------------------------------------------------------
// Generic helpers
//-----------------------------------------------------------------------------

#define IM_PI           3.14159265358979323846f
#ifdef _WIN32
#define IM_NEWLINE      "\r\n"   // Play it nice with Windows users (2018/05 news: Microsoft announced that Notepad will finally display Unix-style carriage returns!)
#else
#define IM_NEWLINE      "\n"
#endif
#define IM_TABSIZE      (4)

#define HanaLovesMe_DEBUG_LOG(_FMT,...)       printf("[%05d] " _FMT, GHanaLovesMe->FrameCount, __VA_ARGS__)
#define IM_STATIC_ASSERT(_COND)         typedef char static_assertion_##__line__[(_COND)?1:-1]
#define IM_F32_TO_INT8_UNBOUND(_VAL)    ((int)((_VAL) * 255.0f + ((_VAL)>=0 ? 0.5f : -0.5f)))   // Unsaturated, for display purpose
#define IM_F32_TO_INT8_SAT(_VAL)        ((int)(ImSaturate(_VAL) * 255.0f + 0.5f))               // Saturated, always output 0..255

// Enforce cdecl calling convention for functions called by the standard library, in case compilation settings changed the default to e.g. __vectorcall
#ifdef _MSC_VER
#define HanaLovesMe_CDECL __cdecl
#else
#define HanaLovesMe_CDECL
#endif

// Helpers: UTF-8 <> wchar
HanaLovesMe_API int           ImTextStrToUtf8(char* buf, int buf_size, const ImWchar * in_text, const ImWchar * in_text_end);      // return output UTF-8 bytes count
HanaLovesMe_API int           ImTextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end);          // read one character. return input UTF-8 bytes count
HanaLovesMe_API int           ImTextStrFromUtf8(ImWchar * buf, int buf_size, const char* in_text, const char* in_text_end, const char** in_remaining = NULL);   // return input UTF-8 bytes count
HanaLovesMe_API int           ImTextCountCharsFromUtf8(const char* in_text, const char* in_text_end);                            // return number of UTF-8 code-points (NOT bytes count)
HanaLovesMe_API int           ImTextCountUtf8BytesFromChar(const char* in_text, const char* in_text_end);                        // return number of bytes to express one char in UTF-8
HanaLovesMe_API int           ImTextCountUtf8BytesFromStr(const ImWchar * in_text, const ImWchar * in_text_end);                   // return number of bytes to express string in UTF-8

// Helpers: Misc
HanaLovesMe_API ImU32         ImHashData(const void* data, size_t data_size, ImU32 seed = 0);
HanaLovesMe_API ImU32         ImHashStr(const char* data, size_t data_size, ImU32 seed = 0);
HanaLovesMe_API void* ImFileLoadToMemory(const char* filename, const char* file_open_mode, size_t * out_file_size = NULL, int padding_bytes = 0);
HanaLovesMe_API FILE * ImFileOpen(const char* filename, const char* file_open_mode);
static inline bool      ImCharIsBlankA(char c) { return c == ' ' || c == '\t'; }
static inline bool      ImCharIsBlankW(unsigned int c) { return c == ' ' || c == '\t' || c == 0x3000; }
static inline bool      ImIsPowerOfTwo(int v) { return v != 0 && (v & (v - 1)) == 0; }
static inline int       ImUpperPowerOfTwo(int v) { v--; v |= v >> 1; v |= v >> 2; v |= v >> 4; v |= v >> 8; v |= v >> 16; v++; return v; }
#define ImQsort         qsort
#ifndef HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS
static inline ImU32     ImHash(const void* data, int size, ImU32 seed = 0) { return size ? ImHashData(data, (size_t)size, seed) : ImHashStr((const char*)data, 0, seed); } // [moved to ImHashStr/ImHashData in 1.68]
#endif

// Helpers: Geometry
HanaLovesMe_API vsize        ImLineClosestPoint(const vsize & a, const vsize & b, const vsize & p);
HanaLovesMe_API bool          ImTriangleContainsPoint(const vsize & a, const vsize & b, const vsize & c, const vsize & p);
HanaLovesMe_API vsize        ImTriangleClosestPoint(const vsize & a, const vsize & b, const vsize & c, const vsize & p);
HanaLovesMe_API void          ImTriangleBarycentricCoords(const vsize & a, const vsize & b, const vsize & c, const vsize & p, float& out_u, float& out_v, float& out_w);
HanaLovesMe_API HanaLovesMeDir      ImGetDirQuadrantFromDelta(float dx, float dy);

// Helpers: String
HanaLovesMe_API int           ImStricmp(const char* str1, const char* str2);
HanaLovesMe_API int           ImStrnicmp(const char* str1, const char* str2, size_t count);
HanaLovesMe_API void          ImStrncpy(char* dst, const char* src, size_t count);
HanaLovesMe_API char* ImStrdup(const char* str);
HanaLovesMe_API char* ImStrdupcpy(char* dst, size_t * p_dst_size, const char* str);
HanaLovesMe_API const char* ImStrchrRange(const char* str_begin, const char* str_end, char c);
HanaLovesMe_API int           ImStrlenW(const ImWchar * str);
HanaLovesMe_API const char* ImStreolRange(const char* str, const char* str_end);                // End end-of-line
HanaLovesMe_API const ImWchar* ImStrbolW(const ImWchar * buf_mid_line, const ImWchar * buf_begin);   // Find beginning-of-line
HanaLovesMe_API const char* ImStristr(const char* haystack, const char* haystack_end, const char* needle, const char* needle_end);
HanaLovesMe_API void          ImStrTrimBlanks(char* str);
HanaLovesMe_API int           ImFormatString(char* buf, size_t buf_size, const char* fmt, ...) IM_FMTARGS(3);
HanaLovesMe_API int           ImFormatStringV(char* buf, size_t buf_size, const char* fmt, va_list args) IM_FMTLIST(3);
HanaLovesMe_API const char* ImParseFormatFindStart(const char* format);
HanaLovesMe_API const char* ImParseFormatFindEnd(const char* format);
HanaLovesMe_API const char* ImParseFormatTrimDecorations(const char* format, char* buf, size_t buf_size);
HanaLovesMe_API int           ImParseFormatPrecision(const char* format, int default_value);

// Helpers: vsize/xuifloatcolor operators
// We are keeping those disabled by default so they don't leak in user space, to allow user enabling implicit cast operators between vsize and their own types (using IM_VEC2_CLASS_EXTRA etc.)
// We unfortunately don't have a unary- operator for vsize because this would needs to be defined inside the class itself.
#ifdef HanaLovesMe_DEFINE_MATH_OPERATORS
static inline vsize operator*(const vsize & lhs, const float rhs) { return vsize(lhs.x * rhs, lhs.y * rhs); }
static inline vsize operator/(const vsize & lhs, const float rhs) { return vsize(lhs.x / rhs, lhs.y / rhs); }
static inline vsize operator+(const vsize & lhs, const vsize & rhs) { return vsize(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline vsize operator-(const vsize & lhs, const vsize & rhs) { return vsize(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline vsize operator*(const vsize & lhs, const vsize & rhs) { return vsize(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline vsize operator/(const vsize & lhs, const vsize & rhs) { return vsize(lhs.x / rhs.x, lhs.y / rhs.y); }
static inline vsize& operator+=(vsize & lhs, const vsize & rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline vsize& operator-=(vsize & lhs, const vsize & rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline vsize& operator*=(vsize & lhs, const float rhs) { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline vsize& operator/=(vsize & lhs, const float rhs) { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static inline xuifloatcolor operator+(const xuifloatcolor & lhs, const xuifloatcolor & rhs) { return xuifloatcolor(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
static inline xuifloatcolor operator-(const xuifloatcolor & lhs, const xuifloatcolor & rhs) { return xuifloatcolor(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
static inline xuifloatcolor operator*(const xuifloatcolor & lhs, const xuifloatcolor & rhs) { return xuifloatcolor(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
#endif

// Helpers: Maths
// - Wrapper for standard libs functions. (Note that HanaLovesMe_demo.cpp does _not_ use them to keep the code easy to copy)
#ifndef HanaLovesMe_DISABLE_MATH_FUNCTIONS
static inline float  ImFabs(float x) { return fabsf(x); }
static inline float  ImSqrt(float x) { return sqrtf(x); }
static inline float  ImPow(float x, float y) { return powf(x, y); }
static inline double ImPow(double x, double y) { return pow(x, y); }
static inline float  ImFmod(float x, float y) { return fmodf(x, y); }
static inline double ImFmod(double x, double y) { return fmod(x, y); }
static inline float  ImCos(float x) { return cosf(x); }
static inline float  ImSin(float x) { return sinf(x); }
static inline float  ImAcos(float x) { return acosf(x); }
static inline float  ImAtan2(float y, float x) { return atan2f(y, x); }
static inline double ImAtof(const char* s) { return atof(s); }
static inline float  ImFloorStd(float x) { return floorf(x); }   // we already uses our own ImFloor() { return (float)(int)v } internally so the standard one wrapper is named differently (it's used by stb_truetype)
static inline float  ImCeil(float x) { return ceilf(x); }
#endif
// - ImMin/ImMax/ImClamp/ImLerp/ImSwap are used by widgets which support for variety of types: signed/unsigned int/long long float/double
// (Exceptionally using templates here but we could also redefine them for variety of types)
template<typename T> static inline T ImMin(T lhs, T rhs) { return lhs < rhs ? lhs : rhs; }
template<typename T> static inline T ImMax(T lhs, T rhs) { return lhs >= rhs ? lhs : rhs; }
template<typename T> static inline T ImClamp(T v, T mn, T mx) { return (v < mn) ? mn : (v > mx) ? mx : v; }
template<typename T> static inline T ImLerp(T a, T b, float t) { return (T)(a + (b - a) * t); }
template<typename T> static inline void ImSwap(T & a, T & b) { T tmp = a; a = b; b = tmp; }
template<typename T> static inline T ImAddClampOverflow(T a, T b, T mn, T mx) { if (b < 0 && (a < mn - b)) return mn; if (b > 0 && (a > mx - b)) return mx; return a + b; }
template<typename T> static inline T ImSubClampOverflow(T a, T b, T mn, T mx) { if (b > 0 && (a < mn + b)) return mn; if (b < 0 && (a > mx + b)) return mx; return a - b; }
// - Misc maths helpers
static inline vsize ImMin(const vsize & lhs, const vsize & rhs) { return vsize(lhs.x < rhs.x ? lhs.x : rhs.x, lhs.y < rhs.y ? lhs.y : rhs.y); }
static inline vsize ImMax(const vsize & lhs, const vsize & rhs) { return vsize(lhs.x >= rhs.x ? lhs.x : rhs.x, lhs.y >= rhs.y ? lhs.y : rhs.y); }
static inline vsize ImClamp(const vsize & v, const vsize & mn, vsize mx) { return vsize((v.x < mn.x) ? mn.x : (v.x > mx.x) ? mx.x : v.x, (v.y < mn.y) ? mn.y : (v.y > mx.y) ? mx.y : v.y); }
static inline vsize ImLerp(const vsize & a, const vsize & b, float t) { return vsize(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t); }
static inline vsize ImLerp(const vsize & a, const vsize & b, const vsize & t) { return vsize(a.x + (b.x - a.x) * t.x, a.y + (b.y - a.y) * t.y); }
static inline xuifloatcolor ImLerp(const xuifloatcolor & a, const xuifloatcolor & b, float t) { return xuifloatcolor(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t); }
static inline float  ImSaturate(float f) { return (f < 0.0f) ? 0.0f : (f > 1.0f) ? 1.0f : f; }
static inline float  ImLengthSqr(const vsize & lhs) { return lhs.x* lhs.x + lhs.y * lhs.y; }
static inline float  ImLengthSqr(const xuifloatcolor & lhs) { return lhs.x* lhs.x + lhs.y * lhs.y + lhs.z * lhs.z + lhs.w * lhs.w; }
static inline float  ImInvLength(const vsize & lhs, float fail_value) { float d = lhs.x * lhs.x + lhs.y * lhs.y; if (d > 0.0f) return 1.0f / ImSqrt(d); return fail_value; }
static inline float  ImFloor(float f) { return (float)(int)f; }
static inline vsize ImFloor(const vsize & v) { return vsize((float)(int)v.x, (float)(int)v.y); }
static inline int    ImModPositive(int a, int b) { return (a + b) % b; }
static inline float  ImDot(const vsize & a, const vsize & b) { return a.x* b.x + a.y * b.y; }
static inline vsize ImRotate(const vsize & v, float cos_a, float sin_a) { return vsize(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a); }
static inline float  ImLinearSweep(float current, float target, float speed) { if (current < target) return ImMin(current + speed, target); if (current > target) return ImMax(current - speed, target); return current; }
static inline vsize ImMul(const vsize & lhs, const vsize & rhs) { return vsize(lhs.x * rhs.x, lhs.y * rhs.y); }

// Helper: ImBoolVector. Store 1-bit per value.
// Note that Resize() currently clears the whole vector.
struct ImBoolVector
{
    ImVector<int>   Stohnly;
    ImBoolVector() { }
    void            Resize(int sz) { Stohnly.resize((sz + 31) >> 5); memset(Stohnly.Data, 0, (size_t)Stohnly.Size * sizeof(Stohnly.Data[0])); }
    void            Clear() { Stohnly.clear(); }
    bool            GetBit(int n) const { int off = (n >> 5); int mask = 1 << (n & 31); return (Stohnly[off] & mask) != 0; }
    void            SetBit(int n, bool v) { int off = (n >> 5); int mask = 1 << (n & 31); if (v) Stohnly[off] |= mask; else Stohnly[off] &= ~mask; }
};

// Helper: ImPool<>. Basic keyed stohnly for contiguous instances, slow/amortized insertion, O(1) indexable, O(Log N) queries by ID over a dense/hot buffer,
// Honor constructor/destructor. Add/remove invalidate all pointers. Indexes have the same lifetime as the associated object.
typedef int ImPoolIdx;
template<typename T>
struct HanaLovesMe_API ImPool
{
    ImVector<T>     Data;       // Contiguous data
    HanaLovesMeStohnly    Map;        // ID->Index
    ImPoolIdx       FreeIdx;    // Next free idx to use

    ImPool() { FreeIdx = 0; }
    ~ImPool() { Clear(); }
    T* GetByKey(HanaLovesMeID key) { int idx = Map.GetInt(key, -1); return (idx != -1) ? &Data[idx] : NULL; }
    T * GetByIndex(ImPoolIdx n) { return &Data[n]; }
    ImPoolIdx   GetIndex(const T * p) const { IM_ASSERT(p >= Data.Data && p < Data.Data + Data.Size); return (ImPoolIdx)(p - Data.Data); }
    T * GetOrAddByKey(HanaLovesMeID key) { int* p_idx = Map.GetIntRef(key, -1); if (*p_idx != -1) return &Data[*p_idx]; *p_idx = FreeIdx; return Add(); }
    bool        Contains(const T * p) const { return (p >= Data.Data && p < Data.Data + Data.Size); }
    void        Clear() { for (int n = 0; n < Map.Data.Size; n++) { int idx = Map.Data[n].val_i; if (idx != -1) Data[idx].~T(); } Map.Clear(); Data.clear(); FreeIdx = 0; }
    T* Add() { int idx = FreeIdx; if (idx == Data.Size) { Data.resize(Data.Size + 1); FreeIdx++; } else { FreeIdx = *(int*)& Data[idx]; } IM_PLACEMENT_NEW(&Data[idx]) T(); return &Data[idx]; }
    void        Remove(HanaLovesMeID key, const T * p) { Remove(key, GetIndex(p)); }
    void        Remove(HanaLovesMeID key, ImPoolIdx idx) { Data[idx].~T(); *(int*)& Data[idx] = FreeIdx; FreeIdx = idx; Map.SetInt(key, -1); }
    void        Reserve(int capacity) { Data.reserve(capacity); Map.Data.reserve(capacity); }
    int         GetSize() const { return Data.Size; }
};

//-----------------------------------------------------------------------------
// Misc data structures
//-----------------------------------------------------------------------------

enum HanaLovesMeButtonFlags_
{
    HanaLovesMeButtonFlags_None = 0,
    HanaLovesMeButtonFlags_Repeat = 1 << 0,   // hold to repeat
    HanaLovesMeButtonFlags_PressedOnClickRelease = 1 << 1,   // [Default] return true on click + release on same item
    HanaLovesMeButtonFlags_PressedOnClick = 1 << 2,   // return true on click (default requires click+release)
    HanaLovesMeButtonFlags_PressedOnRelease = 1 << 3,   // return true on release (default requires click+release)
    HanaLovesMeButtonFlags_PressedOnDoubleClick = 1 << 4,   // return true on double-click (default requires click+release)
    HanaLovesMeButtonFlags_FlattenChildren = 1 << 5,   // allow interactions even if a child window is overlapping
    HanaLovesMeButtonFlags_AllowItemOverlap = 1 << 6,   // require previous frame HoveredId to either match id or be null before being usable, use along with SetItemAllowOverlap()
    HanaLovesMeButtonFlags_DontClosePopups = 1 << 7,   // disable automatically closing parent popup on press // [UNUSED]
    HanaLovesMeButtonFlags_Disabled = 1 << 8,   // disable interactions
    HanaLovesMeButtonFlags_AlignTextBaseLine = 1 << 9,   // vertically align button to match text baseline - ButtonEx() only // FIXME: Should be removed and handled by SmallButton(), not possible currently because of DC.CursorPosPrevLine
    HanaLovesMeButtonFlags_NoKeyModifiers = 1 << 10,  // disable interaction if a key modifier is held
    HanaLovesMeButtonFlags_NoHoldingActiveID = 1 << 11,  // don't set ActiveId while holding the mouse (HanaLovesMeButtonFlags_PressedOnClick only)
    HanaLovesMeButtonFlags_PressedOnDragDropHold = 1 << 12,  // press when held into while we are drag and dropping another item (used by e.g. tree nodes, collapsing headers)
    HanaLovesMeButtonFlags_NoNavFocus = 1 << 13,  // don't override navigation focus when activated
    HanaLovesMeButtonFlags_NoHoveredOnNav = 1 << 14   // don't report as hovered when navigated on
};

enum HanaLovesMeSliderFlags_
{
    HanaLovesMeSliderFlags_None = 0,
    HanaLovesMeSliderFlags_Vertical = 1 << 0
};

enum HanaLovesMeDragFlags_
{
    HanaLovesMeDragFlags_None = 0,
    HanaLovesMeDragFlags_Vertical = 1 << 0
};

enum HanaLovesMeColumnsFlags_
{
    // Default: 0
    HanaLovesMeColumnsFlags_None = 0,
    HanaLovesMeColumnsFlags_NoBorder = 1 << 0,   // Disable column dividers
    HanaLovesMeColumnsFlags_NoResize = 1 << 1,   // Disable resizing columns when clicking on the dividers
    HanaLovesMeColumnsFlags_NoPreserveWidths = 1 << 2,   // Disable column width preservation when adjusting columns
    HanaLovesMeColumnsFlags_NoForceWithinWindow = 1 << 3,   // Disable forcing columns to fit within window
    HanaLovesMeColumnsFlags_GrowParentContentsSize = 1 << 4    // (WIP) Restore pre-1.51 behavior of extending the parent window contents size but _without affecting the columns width at all_. Will eventually remove.
};

enum HanaLovesMeSelectableFlagsPrivate_
{
    // NB: need to be in sync with last value of HanaLovesMeSelectableFlags_
    HanaLovesMeSelectableFlags_NoHoldingActiveID = 1 << 10,
    HanaLovesMeSelectableFlags_PressedOnClick = 1 << 11,
    HanaLovesMeSelectableFlags_PressedOnRelease = 1 << 12,
    HanaLovesMeSelectableFlags_DrawFillAvailWidth = 1 << 13,
    HanaLovesMeSelectableFlags_AllowItemOverlap = 1 << 14
};

enum HanaLovesMeSeparatorFlags_
{
    HanaLovesMeSeparatorFlags_None = 0,
    HanaLovesMeSeparatorFlags_Horizontal = 1 << 0,   // Axis default to current layout type, so generally Horizontal unless e.g. in a menu bar
    HanaLovesMeSeparatorFlags_Vertical = 1 << 1
};

// Transient per-window flags, reset at the beginning of the frame. For child window, inherited from parent on first Begin().
// This is going to be exposed in HanaLovesMe.h when stabilized enough.
enum HanaLovesMeItemFlags_
{
    HanaLovesMeItemFlags_NoTabStop = 1 << 0,  // false
    HanaLovesMeItemFlags_ButtonRepeat = 1 << 1,  // false    // Button() will return true multiple times based on io.KeyRepeatDelay and io.KeyRepeatRate settings.
    HanaLovesMeItemFlags_Disabled = 1 << 2,  // false    // [BETA] Disable interactions but doesn't affect visuals yet. See github.com/ocornut/HanaLovesMe/issues/211
    HanaLovesMeItemFlags_NoNav = 1 << 3,  // false
    HanaLovesMeItemFlags_NoNavDefaultFocus = 1 << 4,  // false
    HanaLovesMeItemFlags_SelectableDontClosePopup = 1 << 5,  // false    // MenuItem/Selectable() automatically closes current Popup window
    HanaLovesMeItemFlags_Default_ = 0
};

// Stohnly for LastItem data
enum HanaLovesMeItemStatusFlags_
{
    HanaLovesMeItemStatusFlags_None = 0,
    HanaLovesMeItemStatusFlags_HoveredRect = 1 << 0,
    HanaLovesMeItemStatusFlags_HasDisplayRect = 1 << 1,
    HanaLovesMeItemStatusFlags_Edited = 1 << 2,   // Value exposed by item was edited in the current frame (should match the bool return value of most widgets)
    HanaLovesMeItemStatusFlags_ToggledSelection = 1 << 3    // Set when Selectable(), TreeNode() reports toggling a selection. We can't report "Selected" because reporting the change allows us to handle clipping with less issues.

#ifdef HanaLovesMe_ENABLE_TEST_ENGINE
    , // [HanaLovesMe-test only]
    HanaLovesMeItemStatusFlags_Openable = 1 << 10,  //
    HanaLovesMeItemStatusFlags_Opened = 1 << 11,  //
    HanaLovesMeItemStatusFlags_Checkable = 1 << 12,  //
    HanaLovesMeItemStatusFlags_Checked = 1 << 13   //
#endif
};

enum HanaLovesMeTextFlags_
{
    HanaLovesMeTextFlags_None = 0,
    HanaLovesMeTextFlags_NoWidthForLargeClippedText = 1 << 0
};

// FIXME: this is in development, not exposed/functional as a generic feature yet.
// Horizontal/Vertical enums are fixed to 0/1 so they may be used to index vsize
enum HanaLovesMeLayoutType_
{
    HanaLovesMeLayoutType_Horizontal = 0,
    HanaLovesMeLayoutType_Vertical = 1
};

enum HanaLovesMeLogType
{
    HanaLovesMeLogType_None = 0,
    HanaLovesMeLogType_TTY,
    HanaLovesMeLogType_File,
    HanaLovesMeLogType_Buffer,
    HanaLovesMeLogType_Clipboard
};

// X/Y enums are fixed to 0/1 so they may be used to index vsize
enum HanaLovesMeAxis
{
    HanaLovesMeAxis_None = -1,
    HanaLovesMeAxis_X = 0,
    HanaLovesMeAxis_Y = 1
};

enum HanaLovesMePlotType
{
    HanaLovesMePlotType_Lines,
    HanaLovesMePlotType_Histogram
};

enum HanaLovesMeInputSource
{
    HanaLovesMeInputSource_None = 0,
    HanaLovesMeInputSource_Mouse,
    HanaLovesMeInputSource_Nav,
    HanaLovesMeInputSource_NavKeyboard,   // Only used occasionally for stohnly, not tested/handled by most code
    HanaLovesMeInputSource_NavGamepad,    // "
    HanaLovesMeInputSource_COUNT
};

// FIXME-NAV: Clarify/expose various repeat delay/rate
enum HanaLovesMeInputReadMode
{
    HanaLovesMeInputReadMode_Down,
    HanaLovesMeInputReadMode_Pressed,
    HanaLovesMeInputReadMode_Released,
    HanaLovesMeInputReadMode_Repeat,
    HanaLovesMeInputReadMode_RepeatSlow,
    HanaLovesMeInputReadMode_RepeatFast
};

enum HanaLovesMeNavHighlightFlags_
{
    HanaLovesMeNavHighlightFlags_None = 0,
    HanaLovesMeNavHighlightFlags_TypeDefault = 1 << 0,
    HanaLovesMeNavHighlightFlags_TypeThin = 1 << 1,
    HanaLovesMeNavHighlightFlags_AlwaysDraw = 1 << 2,       // Draw rectangular highlight if (g.NavId == id) _even_ when using the mouse.
    HanaLovesMeNavHighlightFlags_NoRounding = 1 << 3
};

enum HanaLovesMeNavDirSourceFlags_
{
    HanaLovesMeNavDirSourceFlags_None = 0,
    HanaLovesMeNavDirSourceFlags_Keyboard = 1 << 0,
    HanaLovesMeNavDirSourceFlags_PadDPad = 1 << 1,
    HanaLovesMeNavDirSourceFlags_PadLStick = 1 << 2
};

enum HanaLovesMeNavMoveFlags_
{
    HanaLovesMeNavMoveFlags_None = 0,
    HanaLovesMeNavMoveFlags_LoopX = 1 << 0,   // On failed request, restart from opposite side
    HanaLovesMeNavMoveFlags_LoopY = 1 << 1,
    HanaLovesMeNavMoveFlags_WrapX = 1 << 2,   // On failed request, request from opposite side one line down (when NavDir==right) or one line up (when NavDir==left)
    HanaLovesMeNavMoveFlags_WrapY = 1 << 3,   // This is not super useful for provided for completeness
    HanaLovesMeNavMoveFlags_AllowCurrentNavId = 1 << 4,   // Allow scoring and considering the current NavId as a move target candidate. This is used when the move source is offset (e.g. pressing PageDown actually needs to send a Up move request, if we are pressing PageDown from the bottom-most item we need to stay in place)
    HanaLovesMeNavMoveFlags_AlsoScoreVisibleSet = 1 << 5    // Store alternate result in NavMoveResultLocalVisibleSet that only comprise elements that are already fully visible.
};

enum HanaLovesMeNavForward
{
    HanaLovesMeNavForward_None,
    HanaLovesMeNavForward_ForwardQueued,
    HanaLovesMeNavForward_ForwardActive
};

enum HanaLovesMeNavLayer
{
    HanaLovesMeNavLayer_Main = 0,    // Main scrolling layer
    HanaLovesMeNavLayer_Menu = 1,    // Menu layer (access with Alt/HanaLovesMeNavInput_Menu)
    HanaLovesMeNavLayer_COUNT
};

enum HanaLovesMePopupPositionPolicy
{
    HanaLovesMePopupPositionPolicy_Default,
    HanaLovesMePopupPositionPolicy_ComboBox
};

// 1D vector (this odd construct is used to facilitate the transition between 1D and 2D, and the maintenance of some branches/patches)
struct ImVec1
{
    float   x;
    ImVec1() { x = 0.0f; }
    ImVec1(float _x) { x = _x; }
};


// 2D axis aligned bounding-box
// NB: we can't rely on vsize math operators being available here
struct HanaLovesMe_API vsrect
{
    vsize      Min;    // Upper-left
    vsize      Max;    // Lower-right

    vsrect() : Min(FLT_MAX, FLT_MAX), Max(-FLT_MAX, -FLT_MAX) {}
    vsrect(const vsize& min, const vsize& max) : Min(min), Max(max) {}
    vsrect(const xuifloatcolor& v) : Min(v.x, v.y), Max(v.z, v.w) {}
    vsrect(float x1, float y1, float x2, float y2) : Min(x1, y1), Max(x2, y2) {}

    vsize      GetCenter() const { return vsize((Min.x + Max.x) * 0.5f, (Min.y + Max.y) * 0.5f); }
    vsize      GetSize() const { return vsize(Max.x - Min.x, Max.y - Min.y); }
    float       GetWidth() const { return Max.x - Min.x; }
    float       GetHeight() const { return Max.y - Min.y; }
    vsize      GetTL() const { return Min; }                   // Top-left
    vsize      GetTR() const { return vsize(Max.x, Min.y); }  // Top-right
    vsize      GetBL() const { return vsize(Min.x, Max.y); }  // Bottom-left
    vsize      GetBR() const { return Max; }                   // Bottom-right
    bool        Contains(const vsize & p) const { return p.x >= Min.x && p.y >= Min.y && p.x < Max.x && p.y < Max.y; }
    bool        Contains(const vsrect & r) const { return r.Min.x >= Min.x && r.Min.y >= Min.y && r.Max.x <= Max.x && r.Max.y <= Max.y; }
    bool        Overlaps(const vsrect & r) const { return r.Min.y <  Max.y&& r.Max.y >  Min.y&& r.Min.x <  Max.x&& r.Max.x >  Min.x; }
    void        Add(const vsize & p) { if (Min.x > p.x)     Min.x = p.x;     if (Min.y > p.y)     Min.y = p.y;     if (Max.x < p.x)     Max.x = p.x;     if (Max.y < p.y)     Max.y = p.y; }
    void        Add(const vsrect & r) { if (Min.x > r.Min.x) Min.x = r.Min.x; if (Min.y > r.Min.y) Min.y = r.Min.y; if (Max.x < r.Max.x) Max.x = r.Max.x; if (Max.y < r.Max.y) Max.y = r.Max.y; }
    void        Expand(const float amount) { Min.x -= amount;   Min.y -= amount;   Max.x += amount;   Max.y += amount; }
    void        Expand(const vsize & amount) { Min.x -= amount.x; Min.y -= amount.y; Max.x += amount.x; Max.y += amount.y; }
    void        Translate(const vsize & d) { Min.x += d.x; Min.y += d.y; Max.x += d.x; Max.y += d.y; }
    void        TranslateX(float dx) { Min.x += dx; Max.x += dx; }
    void        TranslateY(float dy) { Min.y += dy; Max.y += dy; }
    void        ClipWith(const vsrect & r) { Min = ImMax(Min, r.Min); Max = ImMin(Max, r.Max); }                   // Simple version, may lead to an inverted rectangle, which is fine for Contains/Overlaps test but not for display.
    void        ClipWithFull(const vsrect & r) { Min = ImClamp(Min, r.Min, r.Max); Max = ImClamp(Max, r.Min, r.Max); } // Full version, ensure both points are fully clipped.
    void        Floor() { Min.x = (float)(int)Min.x; Min.y = (float)(int)Min.y; Max.x = (float)(int)Max.x; Max.y = (float)(int)Max.y; }
    bool        IsInverted() const { return Min.x > Max.x || Min.y > Max.y; }
};

// Stacked color modifier, backup of modified data so we can restore it
struct HanaLovesMeColorMod
{
    HanaLovesMeCol    Col;
    xuifloatcolor      BackupValue;
};

// Stacked style modifier, backup of modified data so we can restore it. Data type inferred from the variable.
struct HanaLovesMeStyleMod
{
    HanaLovesMeStyleVar   VarIdx;
    union { int BackupInt[2]; float BackupFloat[2]; };
    HanaLovesMeStyleMod(HanaLovesMeStyleVar idx, int v) { VarIdx = idx; BackupInt[0] = v; }
    HanaLovesMeStyleMod(HanaLovesMeStyleVar idx, float v) { VarIdx = idx; BackupFloat[0] = v; }
    HanaLovesMeStyleMod(HanaLovesMeStyleVar idx, vsize v) { VarIdx = idx; BackupFloat[0] = v.x; BackupFloat[1] = v.y; }
};

// Stacked stohnly data for BeginGroup()/EndGroup()
struct HanaLovesMeGroupData
{
    vsize      BackupCursorPos;
    vsize      BackupCursorMaxPos;
    ImVec1      BackupIndent;
    ImVec1      BackupGroupOffset;
    vsize      BackupCurrentLineSize;
    float       BackupCurrentLineTextBaseOffset;
    HanaLovesMeID     BackupActiveIdIsAlive;
    bool        BackupActiveIdPreviousFrameIsAlive;
    bool        AdvanceCursor;
};

// Simple column measurement, currently used for MenuItem() only.. This is very short-sighted/throw-away code and NOT a generic helper.
struct HanaLovesMe_API HanaLovesMeMenuColumns
{
    float       Spacing;
    float       Width, NextWidth;
    float       Pos[3], NextWidths[3];

    HanaLovesMeMenuColumns();
    void        Update(int count, float spacing, bool clear);
    float       DeclColumns(float w0, float w1, float w2);
    float       CalcExtraSpace(float avail_w);
};

// Internal state of the currently focused/edited text input box
struct HanaLovesMe_API HanaLovesMeInputTextState
{
    HanaLovesMeID                 ID;                     // widget id owning the text state
    int                     CurLenW, CurLenA;       // we need to maintain our buffer length in both UTF-8 and wchar format. UTF-8 len is valid even if TextA is not.
    ImVector<ImWchar>       TextW;                  // edit buffer, we need to persist but can't guarantee the persistence of the user-provided buffer. so we copy into own buffer.
    ImVector<char>          TextA;                  // temporary UTF8 buffer for callbacks and other operations. this is not updated in every code-path! size=capacity.
    ImVector<char>          InitialTextA;           // backup of end-user buffer at the time of focus (in UTF-8, unaltered)
    bool                    TextAIsValid;           // temporary UTF8 buffer is not initially valid before we make the widget active (until then we pull the data from user argument)
    int                     BufCapacityA;           // end-user buffer capacity
    float                   ScrollX;                // horizontal scrolling/offset
    x07dns::STB_TexteditState Stb;                   // state for stb_textedit.h
    float                   CursorAnim;             // timer for cursor blink, reset on every user action so the cursor reappears immediately
    bool                    CursorFollow;           // set when we want scrolling to follow the current cursor position (not always!)
    bool                    SelectedAllMouseLock;   // after a double-click to select all, we ignore further mouse drags to update selection

    // Temporarily set when active
    HanaLovesMeInputTextFlags     UserFlags;
    HanaLovesMeInputTextCallback  UserCallback;
    void* UserCallbackData;

    HanaLovesMeInputTextState() { memset(this, 0, sizeof(*this)); }
    void                ClearFreeMemory() { TextW.clear(); TextA.clear(); InitialTextA.clear(); }
    void                CursorAnimReset() { CursorAnim = -0.30f; }                                   // After a user-input the cursor stays on for a while without blinking
    void                CursorClamp() { Stb.cursor = ImMin(Stb.cursor, CurLenW); Stb.select_start = ImMin(Stb.select_start, CurLenW); Stb.select_end = ImMin(Stb.select_end, CurLenW); }
    bool                HasSelection() const { return Stb.select_start != Stb.select_end; }
    void                ClearSelection() { Stb.select_start = Stb.select_end = Stb.cursor; }
    void                SelectAll() { Stb.select_start = 0; Stb.cursor = Stb.select_end = CurLenW; Stb.has_preferred_x = 0; }
    int                 GetUndoAvailCount() const { return Stb.undostate.undo_point; }
    int                 GetRedoAvailCount() const { return STB_TEXTEDIT_UNDOSTATECOUNT - Stb.undostate.redo_point; }
    void                OnKeyPressed(int key);      // Cannot be inline because we call in code in stb_textedit.h implementation
};

// Windows data saved in HanaLovesMe.ini file
struct HanaLovesMeWindowSettings
{
    char* Name;
    HanaLovesMeID     ID;
    vsize      Pos;
    vsize      Size;
    bool        Collapsed;

    HanaLovesMeWindowSettings() { Name = NULL; ID = 0; Pos = Size = vsize(0, 0); Collapsed = false; }
};

struct HanaLovesMeSettingsHandler
{
    const char* TypeName;       // Short description stored in .ini file. Disallowed characters: '[' ']'
    HanaLovesMeID     TypeHash;       // == ImHashStr(TypeName, 0, 0)
    void* (*ReadOpenFn)(HanaLovesMeContext* ctx, HanaLovesMeSettingsHandler* handler, const char* name);              // Read: Called when entering into a new ini entry e.g. "[Window][Name]"
    void        (*ReadLineFn)(HanaLovesMeContext* ctx, HanaLovesMeSettingsHandler* handler, void* entry, const char* line); // Read: Called for every line of text within an ini entry
    void        (*WriteAllFn)(HanaLovesMeContext* ctx, HanaLovesMeSettingsHandler* handler, HanaLovesMeTextBuffer* out_buf);      // Write: Output every entries into 'out_buf'
    void* UserData;

    HanaLovesMeSettingsHandler() { memset(this, 0, sizeof(*this)); }
};

// Stohnly for current popup stack
struct HanaLovesMePopupRef
{
    HanaLovesMeID             PopupId;        // Set on OpenPopup()
    HanaLovesMeWindow* Window;         // Resolved on BeginPopup() - may stay unresolved if user never calls OpenPopup()
    HanaLovesMeWindow* ParentWindow;   // Set on OpenPopup()
    int                 OpenFrameCount; // Set on OpenPopup()
    HanaLovesMeID             OpenParentId;   // Set on OpenPopup(), we need this to differentiate multiple menu sets from each others (e.g. inside menu bar vs loose menu items)
    vsize              OpenPopupPos;   // Set on OpenPopup(), preferred popup position (typically == OpenMousePos when using mouse)
    vsize              OpenMousePos;   // Set on OpenPopup(), copy of mouse position at the time of opening popup
};

struct HanaLovesMeColumnData
{
    float               OffsetNorm;         // Column start offset, normalized 0.0 (far left) -> 1.0 (far right)
    float               OffsetNormBeforeResize;
    HanaLovesMeColumnsFlags   Flags;              // Not exposed
    vsrect              ClipRect;

    HanaLovesMeColumnData() { OffsetNorm = OffsetNormBeforeResize = 0.0f; Flags = HanaLovesMeColumnsFlags_None; }
};

struct HanaLovesMeColumns
{
    HanaLovesMeID             ID;
    HanaLovesMeColumnsFlags   Flags;
    bool                IsFirstFrame;
    bool                IsBeingResized;
    int                 Current;
    int                 Count;
    float               MinX, MaxX;
    float               LineMinY, LineMaxY;
    float               BackupCursorPosY;       // Backup of CursorPos at the time of BeginColumns()
    float               BackupCursorMaxPosX;    // Backup of CursorMaxPos at the time of BeginColumns()
    ImVector<HanaLovesMeColumnData> Columns;

    HanaLovesMeColumns() { Clear(); }
    void Clear()
    {
        ID = 0;
        Flags = HanaLovesMeColumnsFlags_None;
        IsFirstFrame = false;
        IsBeingResized = false;
        Current = 0;
        Count = 1;
        MinX = MaxX = 0.0f;
        LineMinY = LineMaxY = 0.0f;
        BackupCursorPosY = 0.0f;
        BackupCursorMaxPosX = 0.0f;
        Columns.clear();
    }
};

// Data shared between all ImDrawList instances
struct HanaLovesMe_API ImDrawListSharedData
{
    vsize          TexUvWhitePixel;            // UV of white pixel in the atlas
    ImFont* Font;                       // Current/default font (optional, for simplified AddText overload)
    float           FontSize;                   // Current/default font size (optional, for simplified AddText overload)
    float           CurveTessellationTol;
    xuifloatcolor          ClipRectFullscreen;         // Value for PushClipRectFullscreen()

    // Const data
    // FIXME: Bake rounded corners fill/borders in atlas
    vsize          CircleVtx12[12];

    ImDrawListSharedData();
};

struct ImDrawDataBuilder
{
    ImVector<ImDrawList*>   Layers[2];           // Global layers for: regular, tooltip

    void Clear() { for (int n = 0; n < IM_ARRAYSIZE(Layers); n++) Layers[n].resize(0); }
    void ClearFreeMemory() { for (int n = 0; n < IM_ARRAYSIZE(Layers); n++) Layers[n].clear(); }
    HanaLovesMe_API void FlattenIntoSingleLayer();
};

struct HanaLovesMeNavMoveResult
{
    HanaLovesMeID       ID;           // Best candidate
    HanaLovesMeID       SelectScopeId;// Best candidate window current selectable group ID
    HanaLovesMeWindow* Window;       // Best candidate window
    float         DistBox;      // Best candidate box distance to current NavId
    float         DistCenter;   // Best candidate center distance to current NavId
    float         DistAxial;
    vsrect        RectRel;      // Best candidate bounding box in window relative space

    HanaLovesMeNavMoveResult() { Clear(); }
    void Clear() { ID = SelectScopeId = 0; Window = NULL; DistBox = DistCenter = DistAxial = FLT_MAX; RectRel = vsrect(); }
};

// Stohnly for SetNexWindow** functions
struct HanaLovesMeNextWindowData
{
    HanaLovesMeCond               PosCond;
    HanaLovesMeCond               SizeCond;
    HanaLovesMeCond               ContentSizeCond;
    HanaLovesMeCond               CollapsedCond;
    HanaLovesMeCond               SizeConstraintCond;
    HanaLovesMeCond               FocusCond;
    HanaLovesMeCond               BgAlphaCond;
    vsize                  PosVal;
    vsize                  PosPivotVal;
    vsize                  SizeVal;
    vsize                  ContentSizeVal;
    bool                    CollapsedVal;
    vsrect                  SizeConstraintRect;
    HanaLovesMeSizeCallback       SizeCallback;
    void* SizeCallbackUserData;
    float                   BgAlphaVal;
    vsize                  MenuBarOffsetMinVal;                // This is not exposed publicly, so we don't clear it.

    HanaLovesMeNextWindowData()
    {
        PosCond = SizeCond = ContentSizeCond = CollapsedCond = SizeConstraintCond = FocusCond = BgAlphaCond = 0;
        PosVal = PosPivotVal = SizeVal = vsize(0.0f, 0.0f);
        ContentSizeVal = vsize(0.0f, 0.0f);
        CollapsedVal = false;
        SizeConstraintRect = vsrect();
        SizeCallback = NULL;
        SizeCallbackUserData = NULL;
        BgAlphaVal = FLT_MAX;
        MenuBarOffsetMinVal = vsize(0.0f, 0.0f);
    }

    void    Clear()
    {
        PosCond = SizeCond = ContentSizeCond = CollapsedCond = SizeConstraintCond = FocusCond = BgAlphaCond = 0;
    }
};

//-----------------------------------------------------------------------------
// Tabs
//-----------------------------------------------------------------------------

struct HanaLovesMeTabBarSortItem
{
    int             Index;
    float           Width;
};

struct HanaLovesMeTabBarRef
{
    HanaLovesMeTabBar* Ptr;                    // Either field can be set, not both. Dock node tab bars are loose while BeginTabBar() ones are in a pool.
    int             IndexInMainPool;

    HanaLovesMeTabBarRef(HanaLovesMeTabBar* ptr) { Ptr = ptr; IndexInMainPool = -1; }
    HanaLovesMeTabBarRef(int index_in_main_pool) { Ptr = NULL; IndexInMainPool = index_in_main_pool; }
};

//-----------------------------------------------------------------------------
// Main HanaLovesMe context
//-----------------------------------------------------------------------------

struct HanaLovesMeContext
{
    bool                    Initialized;
    bool                    FrameScopeActive;                   // Set by NewFrame(), cleared by EndFrame()
    bool                    FrameScopePushedImplicitWindow;     // Set by NewFrame(), cleared by EndFrame()
    bool                    FontAtlasOwnedByContext;            // Io.Fonts-> is owned by the HanaLovesMeContext and will be destructed along with it.
    HanaLovesMeIO                 IO;
    HanaLovesMeStyle              Style;
    ImFont* Font;                               // (Shortcut) == FontStack.empty() ? IO.Font : FontStack.back()
    float                   FontSize;                           // (Shortcut) == FontBaseSize * g.CurrentWindow->FontWindowScale == window->FontSize(). Text height for current window.
    float                   FontBaseSize;                       // (Shortcut) == IO.FontGlobalScale * Font->Scale * Font->FontSize. Base text height.
    ImDrawListSharedData    DrawListSharedData;

    double                  Time;
    int                     FrameCount;
    int                     FrameCountEnded;
    int                     FrameCountRendered;
    ImVector<HanaLovesMeWindow*>  Windows;                            // Windows, sorted in display order, back to front
    ImVector<HanaLovesMeWindow*>  WindowsFocusOrder;                  // Windows, sorted in focus order, back to front
    ImVector<HanaLovesMeWindow*>  WindowsSortBuffer;
    ImVector<HanaLovesMeWindow*>  CurrentWindowStack;
    HanaLovesMeStohnly            WindowsById;
    int                     WindowsActiveCount;
    HanaLovesMeWindow* CurrentWindow;                      // Being drawn into
    HanaLovesMeWindow* HoveredWindow;                      // Will catch mouse inputs
    HanaLovesMeWindow* HoveredRootWindow;                  // Will catch mouse inputs (for focus/move only)
    HanaLovesMeID                 HoveredId;                          // Hovered widget
    bool                    HoveredIdAllowOverlap;
    HanaLovesMeID                 HoveredIdPreviousFrame;
    float                   HoveredIdTimer;                     // Measure contiguous hovering time
    float                   HoveredIdNotActiveTimer;            // Measure contiguous hovering time where the item has not been active
    HanaLovesMeID                 ActiveId;                           // Active widget
    HanaLovesMeID                 ActiveIdPreviousFrame;
    HanaLovesMeID                 ActiveIdIsAlive;                    // Active widget has been seen this frame (we can't use a bool as the ActiveId may change within the frame)
    float                   ActiveIdTimer;
    bool                    ActiveIdIsJustActivated;            // Set at the time of activation for one frame
    bool                    ActiveIdAllowOverlap;               // Active widget allows another widget to steal active id (generally for overlapping widgets, but not always)
    bool                    ActiveIdHasBeenPressed;             // Track whether the active id led to a press (this is to allow changing between PressOnClick and PressOnRelease without pressing twice). Used by range_select branch.
    bool                    ActiveIdHasBeenEdited;              // Was the value associated to the widget Edited over the course of the Active state.
    bool                    ActiveIdPreviousFrameIsAlive;
    bool                    ActiveIdPreviousFrameHasBeenEdited;
    int                     ActiveIdAllowNavDirFlags;           // Active widget allows using directional navigation (e.g. can activate a button and move away from it)
    int                     ActiveIdBlockNavInputFlags;
    vsize                  ActiveIdClickOffset;                // Clicked offset from upper-left corner, if applicable (currently only set by ButtonBehavior)
    HanaLovesMeWindow* ActiveIdWindow;
    HanaLovesMeWindow* ActiveIdPreviousFrameWindow;
    HanaLovesMeInputSource        ActiveIdSource;                     // Activating with mouse or nav (gamepad/keyboard)
    HanaLovesMeID                 LastActiveId;                       // Store the last non-zero ActiveId, useful for animation.
    float                   LastActiveIdTimer;                  // Store the last non-zero ActiveId timer since the beginning of activation, useful for animation.
    vsize                  LastValidMousePos;
    HanaLovesMeWindow* MovingWindow;                       // Track the window we clicked on (in order to preserve focus). The actually window that is moved is generally MovingWindow->RootWindow.
    ImVector<HanaLovesMeColorMod> ColorModifiers;                     // Stack for PushStyleColor()/PopStyleColor()
    ImVector<HanaLovesMeStyleMod> StyleModifiers;                     // Stack for PushStyleVar()/PopStyleVar()
    ImVector<ImFont*>       FontStack;                          // Stack for PushFont()/PopFont()
    ImVector<HanaLovesMePopupRef> OpenPopupStack;                     // Which popups are open (persistent)
    ImVector<HanaLovesMePopupRef> BeginPopupStack;                    // Which level of BeginPopup() we are in (reset every frame)
    HanaLovesMeNextWindowData     NextWindowData;                     // Stohnly for SetNextWindow** functions
    bool                    NextTreeNodeOpenVal;                // Stohnly for SetNextTreeNode** functions
    HanaLovesMeCond               NextTreeNodeOpenCond;

    // Navigation data (for gamepad/keyboard)
    HanaLovesMeWindow* NavWindow;                          // Focused window for navigation. Could be called 'FocusWindow'
    HanaLovesMeID                 NavId;                              // Focused item for navigation
    HanaLovesMeID                 NavActivateId;                      // ~~ (g.ActiveId == 0) && IsNavInputPressed(HanaLovesMeNavInput_Activate) ? NavId : 0, also set when calling ActivateItem()
    HanaLovesMeID                 NavActivateDownId;                  // ~~ IsNavInputDown(HanaLovesMeNavInput_Activate) ? NavId : 0
    HanaLovesMeID                 NavActivatePressedId;               // ~~ IsNavInputPressed(HanaLovesMeNavInput_Activate) ? NavId : 0
    HanaLovesMeID                 NavInputId;                         // ~~ IsNavInputPressed(HanaLovesMeNavInput_Input) ? NavId : 0
    HanaLovesMeID                 NavJustTabbedId;                    // Just tabbed to this id.
    HanaLovesMeID                 NavJustMovedToId;                   // Just navigated to this id (result of a successfully MoveRequest).
    HanaLovesMeID                 NavJustMovedToMultiSelectScopeId;   // Just navigated to this select scope id (result of a successfully MoveRequest).
    HanaLovesMeID                 NavNextActivateId;                  // Set by ActivateItem(), queued until next frame.
    HanaLovesMeInputSource        NavInputSource;                     // Keyboard or Gamepad mode? THIS WILL ONLY BE None or NavGamepad or NavKeyboard.
    vsrect                  NavScoringRectScreen;               // Rectangle used for scoring, in screen space. Based of window->DC.NavRefRectRel[], modified for directional navigation scoring.
    int                     NavScoringCount;                    // Metrics for debugging
    HanaLovesMeWindow* NavWindowingTarget;                 // When selecting a window (holding Menu+FocusPrev/Next, or equivalent of CTRL-TAB) this window is temporarily displayed front-most.
    HanaLovesMeWindow* NavWindowingTargetAnim;             // Record of last valid NavWindowingTarget until DimBgRatio and NavWindowingHighlightAlpha becomes 0.0f
    HanaLovesMeWindow* NavWindowingList;
    float                   NavWindowingTimer;
    float                   NavWindowingHighlightAlpha;
    bool                    NavWindowingToggleLayer;
    HanaLovesMeNavLayer           NavLayer;                           // Layer we are navigating on. For now the system is hard-coded for 0=main contents and 1=menu/title bar, may expose layers later.
    int                     NavIdTabCounter;                    // == NavWindow->DC.FocusIdxTabCounter at time of NavId processing
    bool                    NavIdIsAlive;                       // Nav widget has been seen this frame ~~ NavRefRectRel is valid
    bool                    NavMousePosDirty;                   // When set we will update mouse position if (io.ConfigFlags & HanaLovesMeConfigFlags_NavEnableSetMousePos) if set (NB: this not enabled by default)
    bool                    NavDisableHighlight;                // When user starts using mouse, we hide gamepad/keyboard highlight (NB: but they are still available, which is why NavDisableHighlight isn't always != NavDisableMouseHover)
    bool                    NavDisableMouseHover;               // When user starts using gamepad/keyboard, we hide mouse hovering highlight until mouse is touched again.
    bool                    NavAnyRequest;                      // ~~ NavMoveRequest || NavInitRequest
    bool                    NavInitRequest;                     // Init request for appearing window to select first item
    bool                    NavInitRequestFromMove;
    HanaLovesMeID                 NavInitResultId;
    vsrect                  NavInitResultRectRel;
    bool                    NavMoveFromClampedRefRect;          // Set by manual scrolling, if we scroll to a point where NavId isn't visible we reset navigation from visible items
    bool                    NavMoveRequest;                     // Move request for this frame
    HanaLovesMeNavMoveFlags       NavMoveRequestFlags;
    HanaLovesMeNavForward         NavMoveRequestForward;              // None / ForwardQueued / ForwardActive (this is used to navigate sibling parent menus from a child menu)
    HanaLovesMeDir                NavMoveDir, NavMoveDirLast;         // Direction of the move request (left/right/up/down), direction of the previous move request
    HanaLovesMeDir                NavMoveClipDir;
    HanaLovesMeNavMoveResult      NavMoveResultLocal;                 // Best move request candidate within NavWindow
    HanaLovesMeNavMoveResult      NavMoveResultLocalVisibleSet;       // Best move request candidate within NavWindow that are mostly visible (when using HanaLovesMeNavMoveFlags_AlsoScoreVisibleSet flag)
    HanaLovesMeNavMoveResult      NavMoveResultOther;                 // Best move request candidate within NavWindow's flattened hierarchy (when using HanaLovesMeWindowFlags_NavFlattened flag)

    // Tabbing system (older than Nav, active even if Nav is disabled. FIXME-NAV: This needs a redesign!)
    HanaLovesMeWindow* FocusRequestCurrWindow;             //
    HanaLovesMeWindow* FocusRequestNextWindow;             //
    int                     FocusRequestCurrCounterAll;         // Any item being requested for focus, stored as an index (we on layout to be stable between the frame pressing TAB and the next frame, semi-ouch)
    int                     FocusRequestCurrCounterTab;         // Tab item being requested for focus, stored as an index
    int                     FocusRequestNextCounterAll;         // Stored for next frame
    int                     FocusRequestNextCounterTab;         // "
    bool                    FocusTabPressed;                    //

    // Render
    ImDrawData              DrawData;                           // Main ImDrawData instance to pass render information to the user
    ImDrawDataBuilder       DrawDataBuilder;
    float                   DimBgRatio;                         // 0.0..1.0 animation when fading in a dimming background (for modal window and CTRL+TAB list)
    ImDrawList              BackgroundDrawList;                 // First draw list to be rendered.
    ImDrawList              ForegroundDrawList;                 // Last draw list to be rendered. This is where we the render software mouse cursor (if io.MouseDrawCursor is set) and most debug overlays.
    HanaLovesMeMouseCursor        MouseCursor;

    // Drag and Drop
    bool                    DragDropActive;
    bool                    DragDropWithinSourceOrTarget;
    HanaLovesMeDragDropFlags      DragDropSourceFlags;
    int                     DragDropSourceFrameCount;
    int                     DragDropMouseButton;
    HanaLovesMePayload            DragDropPayload;
    vsrect                  DragDropTargetRect;
    HanaLovesMeID                 DragDropTargetId;
    HanaLovesMeDragDropFlags      DragDropAcceptFlags;
    float                   DragDropAcceptIdCurrRectSurface;    // Target item surface (we resolve overlapping targets by prioritizing the smaller surface)
    HanaLovesMeID                 DragDropAcceptIdCurr;               // Target item id (set at the time of accepting the payload)
    HanaLovesMeID                 DragDropAcceptIdPrev;               // Target item id from previous frame (we need to store this to allow for overlapping drag and drop targets)
    int                     DragDropAcceptFrameCount;           // Last time a target expressed a desire to accept the source
    ImVector<unsigned char> DragDropPayloadBufHeap;             // We don't expose the ImVector<> directly
    unsigned char           DragDropPayloadBufLocal[8];         // Local buffer for small payloads

    // Tab bars
    ImPool<HanaLovesMeTabBar>             TabBars;
    HanaLovesMeTabBar* CurrentTabBar;
    ImVector<HanaLovesMeTabBarRef>        CurrentTabBarStack;
    ImVector<HanaLovesMeTabBarSortItem>   TabSortByWidthBuffer;

    // Widget state
    HanaLovesMeInputTextState     InputTextState;
    ImFont                  InputTextPasswordFont;
    HanaLovesMeID                 ScalarAsInputTextId;                // Temporary text input when CTRL+clicking on a slider, etc.
    HanaLovesMeColorEditFlags     ColorEditOptions;                   // Store user options for color edit widgets
    xuifloatcolor                  ColorPickerRef;
    bool                    DragCurrentAccumDirty;
    float                   DragCurrentAccum;                   // Accumulator for dragging modification. Always high-precision, not rounded by end-user precision settings
    float                   DragSpeedDefaultRatio;              // If speed == 0.0f, uses (max-min) * DragSpeedDefaultRatio
    vsize                  ScrollbarClickDeltaToGrabCenter;    // Distance between mouse and center of grab box, normalized in parent space. Use stohnly?
    int                     TooltipOverrideCount;
    ImVector<char>          PrivateClipboard;                   // If no custom clipboard handler is defined

    // Range-Select/Multi-Select
    // [This is unused in this branch, but left here to facilitate merging/syncing multiple branches]
    HanaLovesMeID                 MultiSelectScopeId;

    // Platform support
    vsize                  PlatformImePos;                     // Cursor position request & last passed to the OS Input Method Editor
    vsize                  PlatformImeLastPos;

    // Settings
    bool                           SettingsLoaded;
    float                          SettingsDirtyTimer;          // Save .ini Settings to memory when time reaches zero
    HanaLovesMeTextBuffer                SettingsIniData;             // In memory .ini settings
    ImVector<HanaLovesMeSettingsHandler> SettingsHandlers;            // List of .ini settings handlers
    ImVector<HanaLovesMeWindowSettings>  SettingsWindows;             // HanaLovesMeWindow .ini settings entries (parsed from the last loaded .ini file and maintained on saving)

    // Logging
    bool                    LogEnabled;
    HanaLovesMeLogType            LogType;
    FILE* LogFile;                            // If != NULL log to stdout/ file
    HanaLovesMeTextBuffer         LogBuffer;                          // Accumulation buffer when log to clipboard. This is pointer so our GHanaLovesMe static constructor doesn't call heap allocators.
    float                   LogLinePosY;
    bool                    LogLineFirstItem;
    int                     LogDepthRef;
    int                     LogDepthToExpand;
    int                     LogDepthToExpandDefault;            // Default/stored value for LogDepthMaxExpand if not specified in the LogXXX function call.

    // Misc
    float                   FramerateSecPerFrame[120];          // Calculate estimate of framerate for user over the last 2 seconds.
    int                     FramerateSecPerFrameIdx;
    float                   FramerateSecPerFrameAccum;
    int                     WantCaptureMouseNextFrame;          // Explicit capture via CaptureKeyboardFromApp()/CaptureMouseFromApp() sets those flags
    int                     WantCaptureKeyboardNextFrame;
    int                     WantTextInputNextFrame;
    char                    TempBuffer[1024 * 3 + 1];               // Temporary text buffer

    HanaLovesMeContext(ImFontAtlas* shared_font_atlas) : BackgroundDrawList(NULL), ForegroundDrawList(NULL)
    {
        Initialized = false;
        FrameScopeActive = FrameScopePushedImplicitWindow = false;
        Font = NULL;
        FontSize = FontBaseSize = 0.0f;
        FontAtlasOwnedByContext = shared_font_atlas ? false : true;
        IO.Fonts = shared_font_atlas ? shared_font_atlas : IM_NEW(ImFontAtlas)();

        Time = 0.0f;
        FrameCount = 0;
        FrameCountEnded = FrameCountRendered = -1;
        WindowsActiveCount = 0;
        CurrentWindow = NULL;
        HoveredWindow = NULL;
        HoveredRootWindow = NULL;
        HoveredId = 0;
        HoveredIdAllowOverlap = false;
        HoveredIdPreviousFrame = 0;
        HoveredIdTimer = HoveredIdNotActiveTimer = 0.0f;
        ActiveId = 0;
        ActiveIdPreviousFrame = 0;
        ActiveIdIsAlive = 0;
        ActiveIdTimer = 0.0f;
        ActiveIdIsJustActivated = false;
        ActiveIdAllowOverlap = false;
        ActiveIdHasBeenPressed = false;
        ActiveIdHasBeenEdited = false;
        ActiveIdPreviousFrameIsAlive = false;
        ActiveIdPreviousFrameHasBeenEdited = false;
        ActiveIdAllowNavDirFlags = 0x00;
        ActiveIdBlockNavInputFlags = 0x00;
        ActiveIdClickOffset = vsize(-1, -1);
        ActiveIdWindow = ActiveIdPreviousFrameWindow = NULL;
        ActiveIdSource = HanaLovesMeInputSource_None;
        LastActiveId = 0;
        LastActiveIdTimer = 0.0f;
        LastValidMousePos = vsize(0.0f, 0.0f);
        MovingWindow = NULL;
        NextTreeNodeOpenVal = false;
        NextTreeNodeOpenCond = 0;

        NavWindow = NULL;
        NavId = NavActivateId = NavActivateDownId = NavActivatePressedId = NavInputId = 0;
        NavJustTabbedId = NavJustMovedToId = NavJustMovedToMultiSelectScopeId = NavNextActivateId = 0;
        NavInputSource = HanaLovesMeInputSource_None;
        NavScoringRectScreen = vsrect();
        NavScoringCount = 0;
        NavWindowingTarget = NavWindowingTargetAnim = NavWindowingList = NULL;
        NavWindowingTimer = NavWindowingHighlightAlpha = 0.0f;
        NavWindowingToggleLayer = false;
        NavLayer = HanaLovesMeNavLayer_Main;
        NavIdTabCounter = INT_MAX;
        NavIdIsAlive = false;
        NavMousePosDirty = false;
        NavDisableHighlight = true;
        NavDisableMouseHover = false;
        NavAnyRequest = false;
        NavInitRequest = false;
        NavInitRequestFromMove = false;
        NavInitResultId = 0;
        NavMoveFromClampedRefRect = false;
        NavMoveRequest = false;
        NavMoveRequestFlags = 0;
        NavMoveRequestForward = HanaLovesMeNavForward_None;
        NavMoveDir = NavMoveDirLast = NavMoveClipDir = HanaLovesMeDir_None;

        FocusRequestCurrWindow = FocusRequestNextWindow = NULL;
        FocusRequestCurrCounterAll = FocusRequestCurrCounterTab = INT_MAX;
        FocusRequestNextCounterAll = FocusRequestNextCounterTab = INT_MAX;
        FocusTabPressed = false;

        DimBgRatio = 0.0f;
        BackgroundDrawList._Data = &DrawListSharedData;
        BackgroundDrawList._OwnerName = "##Background"; // Give it a name for debugging
        ForegroundDrawList._Data = &DrawListSharedData;
        ForegroundDrawList._OwnerName = "##Foreground"; // Give it a name for debugging
        MouseCursor = HanaLovesMeMouseCursor_Arrow;

        DragDropActive = DragDropWithinSourceOrTarget = false;
        DragDropSourceFlags = 0;
        DragDropSourceFrameCount = -1;
        DragDropMouseButton = -1;
        DragDropTargetId = 0;
        DragDropAcceptFlags = 0;
        DragDropAcceptIdCurrRectSurface = 0.0f;
        DragDropAcceptIdPrev = DragDropAcceptIdCurr = 0;
        DragDropAcceptFrameCount = -1;
        memset(DragDropPayloadBufLocal, 0, sizeof(DragDropPayloadBufLocal));

        CurrentTabBar = NULL;

        ScalarAsInputTextId = 0;
        ColorEditOptions = HanaLovesMeColorEditFlags__OptionsDefault;
        DragCurrentAccumDirty = false;
        DragCurrentAccum = 0.0f;
        DragSpeedDefaultRatio = 1.0f / 100.0f;
        ScrollbarClickDeltaToGrabCenter = vsize(0.0f, 0.0f);
        TooltipOverrideCount = 0;

        MultiSelectScopeId = 0;

        PlatformImePos = PlatformImeLastPos = vsize(FLT_MAX, FLT_MAX);

        SettingsLoaded = false;
        SettingsDirtyTimer = 0.0f;

        LogEnabled = false;
        LogType = HanaLovesMeLogType_None;
        LogFile = NULL;
        LogLinePosY = FLT_MAX;
        LogLineFirstItem = false;
        LogDepthRef = 0;
        LogDepthToExpand = LogDepthToExpandDefault = 2;

        memset(FramerateSecPerFrame, 0, sizeof(FramerateSecPerFrame));
        FramerateSecPerFrameIdx = 0;
        FramerateSecPerFrameAccum = 0.0f;
        WantCaptureMouseNextFrame = WantCaptureKeyboardNextFrame = WantTextInputNextFrame = -1;
        memset(TempBuffer, 0, sizeof(TempBuffer));
    }
};

//-----------------------------------------------------------------------------
// HanaLovesMeWindow
//-----------------------------------------------------------------------------

// Transient per-window data, reset at the beginning of the frame. This used to be called HanaLovesMeDrawContext, hence the DC variable name in HanaLovesMeWindow.
// FIXME: That's theory, in practice the delimitation between HanaLovesMeWindow and HanaLovesMeWindowTempData is quite tenuous and could be reconsidered.
struct HanaLovesMe_API HanaLovesMeWindowTempData
{
    vsize                  CursorPos;
    vsize                  CursorPosPrevLine;
    vsize                  CursorStartPos;         // Initial position in client area with padding
    vsize                  CursorMaxPos;           // Used to implicitly calculate the size of our contents, always growing during the frame. Turned into window->SizeContents at the beginning of next frame
    vsize                  CurrentLineSize;
    float                   CurrentLineTextBaseOffset;
    vsize                  PrevLineSize;
    float                   PrevLineTextBaseOffset;
    int                     TreeDepth;
    ImU32                   TreeDepthMayJumpToParentOnPop; // Store a copy of !g.NavIdIsAlive for TreeDepth 0..31
    HanaLovesMeID                 LastItemId;
    HanaLovesMeItemStatusFlags    LastItemStatusFlags;
    vsrect                  LastItemRect;           // Interaction rect
    vsrect                  LastItemDisplayRect;    // End-user display rect (only valid if LastItemStatusFlags & HanaLovesMeItemStatusFlags_HasDisplayRect)
    HanaLovesMeNavLayer           NavLayerCurrent;        // Current layer, 0..31 (we currently only use 0..1)
    int                     NavLayerCurrentMask;    // = (1 << NavLayerCurrent) used by ItemAdd prior to clipping.
    int                     NavLayerActiveMask;     // Which layer have been written to (result from previous frame)
    int                     NavLayerActiveMaskNext; // Which layer have been written to (buffer for current frame)
    bool                    NavHideHighlightOneFrame;
    bool                    NavHasScroll;           // Set when scrolling can be used (ScrollMax > 0.0f)
    bool                    MenuBarAppending;       // FIXME: Remove this
    vsize                  MenuBarOffset;          // MenuBarOffset.x is sort of equivalent of a per-layer CursorPos.x, saved/restored as we switch to the menu bar. The only situation when MenuBarOffset.y is > 0 if when (SafeAreaPadding.y > FramePadding.y), often used on TVs.
    ImVector<HanaLovesMeWindow*>  ChildWindows;
    HanaLovesMeStohnly* StateStohnly;
    HanaLovesMeLayoutType         LayoutType;
    HanaLovesMeLayoutType         ParentLayoutType;       // Layout type of parent window at the time of Begin()
    int                     FocusCounterAll;        // Counter for focus/tabbing system. Start at -1 and increase as assigned via FocusableItemRegister() (FIXME-NAV: Needs redesign)
    int                     FocusCounterTab;        // (same, but only count widgets which you can Tab through)

    // We store the current settings outside of the vectors to increase memory locality (reduce cache misses). The vectors are rarely modified. Also it allows us to not heap allocate for short-lived windows which are not using those settings.
    HanaLovesMeItemFlags          ItemFlags;              // == ItemFlagsStack.back() [empty == HanaLovesMeItemFlags_Default]
    float                   ItemWidth;              // == ItemWidthStack.back(). 0.0: default, >0.0: width in pixels, <0.0: align xx pixels to the right of window
    float                   TextWrapPos;            // == TextWrapPosStack.back() [empty == -1.0f]
    ImVector<HanaLovesMeItemFlags>ItemFlagsStack;
    ImVector<float>         ItemWidthStack;
    ImVector<float>         TextWrapPosStack;
    ImVector<HanaLovesMeGroupData>GroupStack;
    short                   StackSizesBackup[6];    // Store size of various stacks for asserting

    ImVec1                  Indent;                 // Indentation / start position from left of window (increased by TreePush/TreePop, etc.)
    ImVec1                  GroupOffset;
    ImVec1                  ColumnsOffset;          // Offset to the current column (if ColumnsCurrent > 0). FIXME: This and the above should be a stack to allow use cases like Tree->Column->Tree. Need revamp columns API.
    HanaLovesMeColumns* CurrentColumns;         // Current columns set

    HanaLovesMeWindowTempData()
    {
        CursorPos = CursorPosPrevLine = CursorStartPos = CursorMaxPos = vsize(0.0f, 0.0f);
        CurrentLineSize = PrevLineSize = vsize(0.0f, 0.0f);
        CurrentLineTextBaseOffset = PrevLineTextBaseOffset = 0.0f;
        TreeDepth = 0;
        TreeDepthMayJumpToParentOnPop = 0x00;
        LastItemId = 0;
        LastItemStatusFlags = 0;
        LastItemRect = LastItemDisplayRect = vsrect();
        NavLayerActiveMask = NavLayerActiveMaskNext = 0x00;
        NavLayerCurrent = HanaLovesMeNavLayer_Main;
        NavLayerCurrentMask = (1 << HanaLovesMeNavLayer_Main);
        NavHideHighlightOneFrame = false;
        NavHasScroll = false;
        MenuBarAppending = false;
        MenuBarOffset = vsize(0.0f, 0.0f);
        StateStohnly = NULL;
        LayoutType = ParentLayoutType = HanaLovesMeLayoutType_Vertical;
        FocusCounterAll = FocusCounterTab = -1;

        ItemFlags = HanaLovesMeItemFlags_Default_;
        ItemWidth = 0.0f;
        TextWrapPos = -1.0f;
        memset(StackSizesBackup, 0, sizeof(StackSizesBackup));

        Indent = ImVec1(0.0f);
        GroupOffset = ImVec1(0.0f);
        ColumnsOffset = ImVec1(0.0f);
        CurrentColumns = NULL;
    }
};

// Stohnly for one window
struct HanaLovesMe_API HanaLovesMeWindow
{
    char* Name;
    HanaLovesMeID                 ID;                                 // == ImHash(Name)
    HanaLovesMeWindowFlags        Flags;                              // See enum HanaLovesMeWindowFlags_
    vsize                  Pos;                                // Position (always rounded-up to nearest pixel)
    vsize                  Size;                               // Current size (==SizeFull or collapsed title bar size)
    vsize                  SizeFull;                           // Size when non collapsed
    vsize                  SizeFullAtLastBegin;                // Copy of SizeFull at the end of Begin. This is the reference value we'll use on the next frame to decide if we need scrollbars.
    vsize                  SizeContents;                       // Size of contents (== extents reach of the drawing cursor) from previous frame. Include decoration, window title, border, menu, etc.
    vsize                  SizeContentsExplicit;               // Size of contents explicitly set by the user via SetNextWindowContentSize()
    vsize                  WindowPadding;                      // Window padding at the time of begin.
    float                   WindowRounding;                     // Window rounding at the time of begin.
    float                   WindowBorderSize;                   // Window border size at the time of begin.
    int                     NameBufLen;                         // Size of buffer storing Name. May be larger than strlen(Name)!
    HanaLovesMeID                 MoveId;                             // == window->GetID("#MOVE")
    HanaLovesMeID                 ChildId;                            // ID of corresponding item in parent window (for navigation to return from child window to parent window)
    vsize                  Scroll;
    vsize                  ScrollTarget;                       // target scroll position. stored as cursor position with scrolling canceled out, so the highest point is always 0.0f. (FLT_MAX for no change)
    vsize                  ScrollTargetCenterRatio;            // 0.0f = scroll so that target position is at top, 0.5f = scroll so that target position is centered
    vsize                  ScrollbarSizes;                     // Size taken by scrollbars on each axis
    bool                    ScrollbarX, ScrollbarY;
    bool                    Active;                             // Set to true on Begin(), unless Collapsed
    bool                    WasActive;
    bool                    WriteAccessed;                      // Set to true when any widget access the current window
    bool                    Collapsed;                          // Set when collapsing window to become only title-bar
    bool                    WantCollapseToggle;
    bool                    SkipItems;                          // Set when items can safely be all clipped (e.g. window not visible or collapsed)
    bool                    Appearing;                          // Set during the frame where the window is appearing (or re-appearing)
    bool                    Hidden;                             // Do not display (== (HiddenFrames*** > 0))
    bool                    HasCloseButton;                     // Set when the window has a close button (p_open != NULL)
    signed char             ResizeBorderHeld;                   // Current border being held for resize (-1: none, otherwise 0-3)
    short                   BeginCount;                         // Number of Begin() during the current frame (generally 0 or 1, 1+ if appending via multiple Begin/End pairs)
    short                   BeginOrderWithinParent;             // Order within immediate parent window, if we are a child window. Otherwise 0.
    short                   BeginOrderWithinContext;            // Order within entire HanaLovesMe context. This is mostly used for debugging submission order related issues.
    HanaLovesMeID                 PopupId;                            // ID in the popup stack when this window is used as a popup/menu (because we use generic Name/ID for recycling)
    int                     AutoFitFramesX, AutoFitFramesY;
    bool                    AutoFitOnlyGrows;
    int                     AutoFitChildAxises;
    HanaLovesMeDir                AutoPosLastDirection;
    int                     HiddenFramesCanSkipItems;           // Hide the window for N frames
    int                     HiddenFramesCannotSkipItems;        // Hide the window for N frames while allowing items to be submitted so we can measure their size
    HanaLovesMeCond               SetWindowPosAllowFlags;             // store acceptable condition flags for SetNextWindowPos() use.
    HanaLovesMeCond               SetWindowSizeAllowFlags;            // store acceptable condition flags for SetNextWindowSize() use.
    HanaLovesMeCond               SetWindowCollapsedAllowFlags;       // store acceptable condition flags for SetNextWindowCollapsed() use.
    vsize                  SetWindowPosVal;                    // store window position when using a non-zero Pivot (position set needs to be processed when we know the window size)
    vsize                  SetWindowPosPivot;                  // store window pivot for positioning. vsize(0,0) when positioning from top-left corner; vsize(0.5f,0.5f) for centering; vsize(1,1) for bottom right.

    HanaLovesMeWindowTempData     DC;                                 // Temporary per-window data, reset at the beginning of the frame. This used to be called HanaLovesMeDrawContext, hence the "DC" variable name.
    ImVector<HanaLovesMeID>       IDStack;                            // ID stack. ID are hashes seeded with the value at the top of the stack
    vsrect                  ClipRect;                           // Current clipping rectangle. = DrawList->clip_rect_stack.back(). Scissoring / clipping rectangle. x1, y1, x2, y2.
    vsrect                  OuterRectClipped;                   // = WindowRect just after setup in Begin(). == window->Rect() for root window.
    vsrect                  InnerMainRect, InnerClipRect;
    vsrect                  ContentsRegionRect;                 // FIXME: This is currently confusing/misleading. Maximum visible content position ~~ Pos + (SizeContentsExplicit ? SizeContentsExplicit : Size - ScrollbarSizes) - CursorStartPos, per axis
    int                     LastFrameActive;                    // Last frame number the window was Active.
    float                   ItemWidthDefault;
    HanaLovesMeMenuColumns        MenuColumns;                        // Simplified columns stohnly for menu items
    HanaLovesMeStohnly            StateStohnly;
    ImVector<HanaLovesMeColumns>  ColumnsStohnly;
    float                   FontWindowScale;                    // User scale multiplier per-window
    int                     SettingsIdx;                        // Index into SettingsWindow[] (indices are always valid as we only grow the array from the back)

    ImDrawList* DrawList;                           // == &DrawListInst (for backward compatibility reason with code using HanaLovesMe_internal.h we keep this a pointer)
    ImDrawList              DrawListInst;
    HanaLovesMeWindow* ParentWindow;                       // If we are a child _or_ popup window, this is pointing to our parent. Otherwise NULL.
    HanaLovesMeWindow* RootWindow;                         // Point to ourself or first ancestor that is not a child window.
    HanaLovesMeWindow* RootWindowForTitleBarHighlight;     // Point to ourself or first ancestor which will display TitleBgActive color when this window is active.
    HanaLovesMeWindow* RootWindowForNav;                   // Point to ourself or first ancestor which doesn't have the NavFlattened flag.

    HanaLovesMeWindow* NavLastChildNavWindow;              // When going to the menu bar, we remember the child window we came from. (This could probably be made implicit if we kept g.Windows sorted by last focused including child window.)
    HanaLovesMeID                 NavLastIds[HanaLovesMeNavLayer_COUNT];    // Last known NavId for this window, per layer (0/1)
    vsrect                  NavRectRel[HanaLovesMeNavLayer_COUNT];    // Reference rectangle, in window relative space

public:
    HanaLovesMeWindow(HanaLovesMeContext* context, const char* name);
    ~HanaLovesMeWindow();

    HanaLovesMeID     GetID(const char* str, const char* str_end = NULL);
    HanaLovesMeID     GetID(const void* ptr);
    HanaLovesMeID     GetIDNoKeepAlive(const char* str, const char* str_end = NULL);
    HanaLovesMeID     GetIDNoKeepAlive(const void* ptr);
    HanaLovesMeID     GetIDFromRectangle(const vsrect& r_abs);

    // We don't use g.FontSize because the window may be != g.CurrentWidow.
    vsrect      Rect() const { return vsrect(Pos.x, Pos.y, Pos.x + Size.x, Pos.y + Size.y); }
    float       CalcFontSize() const { return GHanaLovesMe->FontBaseSize* FontWindowScale; }
    float       TitleBarHeight() const { return (Flags & HanaLovesMeWindowFlags_NoTitleBar) ? 0.0f : CalcFontSize() + GHanaLovesMe->Style.FramePadding.y * 2.0f; }
    vsrect      TitleBarRect() const { return vsrect(Pos, vsize(Pos.x + SizeFull.x, Pos.y + TitleBarHeight())); }
    float       MenuBarHeight() const { return (Flags & HanaLovesMeWindowFlags_MenuBar) ? DC.MenuBarOffset.y + CalcFontSize() + GHanaLovesMe->Style.FramePadding.y * 2.0f : 0.0f; }
    vsrect      MenuBarRect() const { float y1 = Pos.y + TitleBarHeight(); return vsrect(Pos.x, y1, Pos.x + SizeFull.x, y1 + MenuBarHeight()); }
};

// Backup and restore just enough data to be able to use IsItemHovered() on item A after another B in the same window has overwritten the data.
struct HanaLovesMeItemHoveredDataBackup
{
    HanaLovesMeID                 LastItemId;
    HanaLovesMeItemStatusFlags    LastItemStatusFlags;
    vsrect                  LastItemRect;
    vsrect                  LastItemDisplayRect;

    HanaLovesMeItemHoveredDataBackup() { Backup(); }
    void Backup() { HanaLovesMeWindow* window = GHanaLovesMe->CurrentWindow; LastItemId = window->DC.LastItemId; LastItemStatusFlags = window->DC.LastItemStatusFlags; LastItemRect = window->DC.LastItemRect; LastItemDisplayRect = window->DC.LastItemDisplayRect; }
    void Restore() const { HanaLovesMeWindow* window = GHanaLovesMe->CurrentWindow; window->DC.LastItemId = LastItemId; window->DC.LastItemStatusFlags = LastItemStatusFlags; window->DC.LastItemRect = LastItemRect; window->DC.LastItemDisplayRect = LastItemDisplayRect; }
};

//-----------------------------------------------------------------------------
// Tab bar, tab item
//-----------------------------------------------------------------------------

enum HanaLovesMeTabBarFlagsPrivate_
{
    HanaLovesMeTabBarFlags_DockNode = 1 << 20,  // Part of a dock node [we don't use this in the master branch but it facilitate branch syncing to keep this around]
    HanaLovesMeTabBarFlags_IsFocused = 1 << 21,
    HanaLovesMeTabBarFlags_SaveSettings = 1 << 22   // FIXME: Settings are handled by the docking system, this only request the tab bar to mark settings dirty when reordering tabs
};

enum HanaLovesMeTabItemFlagsPrivate_
{
    HanaLovesMeTabItemFlags_NoCloseButton = 1 << 20   // Store whether p_open is set or not, which we need to recompute WidthContents during layout.
};

// Stohnly for one active tab item (sizeof() 26~32 bytes)
struct HanaLovesMeTabItem
{
    HanaLovesMeID             ID;
    HanaLovesMeTabItemFlags   Flags;
    int                 LastFrameVisible;
    int                 LastFrameSelected;      // This allows us to infer an ordered list of the last activated tabs with little maintenance
    int                 NameOffset;             // When Window==NULL, offset to name within parent HanaLovesMeTabBar::TabsNames
    float               Offset;                 // Position relative to beginning of tab
    float               Width;                  // Width currently displayed
    float               WidthContents;          // Width of actual contents, stored during BeginTabItem() call

    HanaLovesMeTabItem() { ID = Flags = 0; LastFrameVisible = LastFrameSelected = -1; NameOffset = -1; Offset = Width = WidthContents = 0.0f; }
};

// Stohnly for a tab bar (sizeof() 92~96 bytes)
struct HanaLovesMeTabBar
{
    ImVector<HanaLovesMeTabItem> Tabs;
    HanaLovesMeID             ID;                     // Zero for tab-bars used by docking
    HanaLovesMeID             SelectedTabId;          // Selected tab
    HanaLovesMeID             NextSelectedTabId;
    HanaLovesMeID             VisibleTabId;           // Can occasionally be != SelectedTabId (e.g. when previewing contents for CTRL+TAB preview)
    int                 CurrFrameVisible;
    int                 PrevFrameVisible;
    vsrect              BarRect;
    float               ContentsHeight;
    float               OffsetMax;              // Distance from BarRect.Min.x, locked during layout
    float               OffsetNextTab;          // Distance from BarRect.Min.x, incremented with each BeginTabItem() call, not used if HanaLovesMeTabBarFlags_Reorderable if set.
    float               ScrollingAnim;
    float               ScrollingTarget;
    float               ScrollingTargetDistToVisibility;
    float               ScrollingSpeed;
    HanaLovesMeTabBarFlags    Flags;
    HanaLovesMeID             ReorderRequestTabId;
    int                 ReorderRequestDir;
    bool                WantLayout;
    bool                VisibleTabWasSubmitted;
    short               LastTabItemIdx;         // For BeginTabItem()/EndTabItem()
    vsize              FramePadding;           // style.FramePadding locked at the time of BeginTabBar()
    HanaLovesMeTextBuffer     TabsNames;              // For non-docking tab bar we re-append names in a contiguous buffer.

    HanaLovesMeTabBar();
    int                 GetTabOrder(const HanaLovesMeTabItem* tab) const { return Tabs.index_from_ptr(tab); }
    const char* GetTabName(const HanaLovesMeTabItem* tab) const
    {
        IM_ASSERT(tab->NameOffset != -1 && tab->NameOffset < TabsNames.Buf.Size);
        return TabsNames.Buf.Data + tab->NameOffset;
    }
};

//-----------------------------------------------------------------------------
// Internal API
// No guarantee of forward compatibility here.
//-----------------------------------------------------------------------------

namespace HanaLovesMe
{
    // We should always have a CurrentWindow in the stack (there is an implicit "Debug" window)
    // If this ever crash because g.CurrentWindow is NULL it means that either
    // - HanaLovesMe::NewFrame() has never been called, which is illegal.
    // - You are calling HanaLovesMe functions after HanaLovesMe::EndFrame()/HanaLovesMe::Render() and before the next HanaLovesMe::NewFrame(), which is also illegal.
    inline    HanaLovesMeWindow* GetCurrentWindowRead() { HanaLovesMeContext& g = *GHanaLovesMe; return g.CurrentWindow; }
    inline    HanaLovesMeWindow* GetCurrentWindow() { HanaLovesMeContext& g = *GHanaLovesMe; g.CurrentWindow->WriteAccessed = true; return g.CurrentWindow; }
    HanaLovesMe_API HanaLovesMeWindow* FindWindowByID(HanaLovesMeID id);
    HanaLovesMe_API HanaLovesMeWindow* FindWindowByName(const char* name);
    HanaLovesMe_API void          FocusWindow(HanaLovesMeWindow* window);
    HanaLovesMe_API void          FocusPreviousWindowIgnoringOne(HanaLovesMeWindow* ignore_window);
    HanaLovesMe_API void          BringWindowToFocusFront(HanaLovesMeWindow* window);
    HanaLovesMe_API void          BringWindowToDisplayFront(HanaLovesMeWindow* window);
    HanaLovesMe_API void          BringWindowToDisplayBack(HanaLovesMeWindow* window);
    HanaLovesMe_API void          UpdateWindowParentAndRootLinks(HanaLovesMeWindow* window, HanaLovesMeWindowFlags flags, HanaLovesMeWindow* parent_window);
    HanaLovesMe_API vsize        CalcWindowExpectedSize(HanaLovesMeWindow* window);
    HanaLovesMe_API bool          IsWindowChildOf(HanaLovesMeWindow* window, HanaLovesMeWindow* potential_parent);
    HanaLovesMe_API bool          IsWindowNavFocusable(HanaLovesMeWindow* window);
    HanaLovesMe_API void          SetWindowScrollX(HanaLovesMeWindow* window, float new_scroll_x);
    HanaLovesMe_API void          SetWindowScrollY(HanaLovesMeWindow* window, float new_scroll_y);
    HanaLovesMe_API float         GetWindowScrollMaxX(HanaLovesMeWindow* window);
    HanaLovesMe_API float         GetWindowScrollMaxY(HanaLovesMeWindow* window);
    HanaLovesMe_API vsrect        GetWindowAllowedExtentRect(HanaLovesMeWindow* window);
    HanaLovesMe_API void          SetWindowPos(HanaLovesMeWindow* window, const vsize& pos, HanaLovesMeCond cond);
    HanaLovesMe_API void          SetWindowSize(HanaLovesMeWindow* window, const vsize& size, HanaLovesMeCond cond);
    HanaLovesMe_API void          SetWindowCollapsed(HanaLovesMeWindow* window, bool collapsed, HanaLovesMeCond cond);

    HanaLovesMe_API void          SetCurrentFont(ImFont* font);
    inline ImFont* GetDefaultFont() { HanaLovesMeContext& g = *GHanaLovesMe; return g.IO.FontDefault ? g.IO.FontDefault : g.IO.Fonts->Fonts[0]; }

    // Init
    HanaLovesMe_API void          Initialize(HanaLovesMeContext* context);
    HanaLovesMe_API void          Shutdown(HanaLovesMeContext* context);    // Since 1.60 this is a _private_ function. You can call DestroyContext() to destroy the context created by CreateContext().

    // NewFrame
    HanaLovesMe_API void          UpdateHoveredWindowAndCaptureFlags();
    HanaLovesMe_API void          StartMouseMovingWindow(HanaLovesMeWindow* window);
    HanaLovesMe_API void          UpdateMouseMovingWindowNewFrame();
    HanaLovesMe_API void          UpdateMouseMovingWindowEndFrame();

    // Settings
    HanaLovesMe_API void                  MarkIniSettingsDirty();
    HanaLovesMe_API void                  MarkIniSettingsDirty(HanaLovesMeWindow* window);
    HanaLovesMe_API HanaLovesMeWindowSettings* CreateNewWindowSettings(const char* name);
    HanaLovesMe_API HanaLovesMeWindowSettings* FindWindowSettings(HanaLovesMeID id);
    HanaLovesMe_API HanaLovesMeWindowSettings* FindOrCreateWindowSettings(const char* name);
    HanaLovesMe_API HanaLovesMeSettingsHandler* FindSettingsHandler(const char* type_name);

    // Basic Accessors
    inline HanaLovesMeID          GetItemID() { HanaLovesMeContext& g = *GHanaLovesMe; return g.CurrentWindow->DC.LastItemId; }
    inline HanaLovesMeID          GetActiveID() { HanaLovesMeContext& g = *GHanaLovesMe; return g.ActiveId; }
    inline HanaLovesMeID          GetFocusID() { HanaLovesMeContext& g = *GHanaLovesMe; return g.NavId; }
    HanaLovesMe_API void          SetActiveID(HanaLovesMeID id, HanaLovesMeWindow* window);
    HanaLovesMe_API void          SetFocusID(HanaLovesMeID id, HanaLovesMeWindow* window);
    HanaLovesMe_API void          ClearActiveID();
    HanaLovesMe_API HanaLovesMeID       GetHoveredID();
    HanaLovesMe_API void          SetHoveredID(HanaLovesMeID id);
    HanaLovesMe_API void          KeepAliveID(HanaLovesMeID id);
    HanaLovesMe_API void          MarkItemEdited(HanaLovesMeID id);

    // Basic Helpers for widget code
    HanaLovesMe_API void          ItemSize(const vsize& size, float text_offset_y = 0.0f);
    HanaLovesMe_API void          ItemSize(const vsrect& bb, float text_offset_y = 0.0f);
    HanaLovesMe_API bool          ItemAdd(const vsrect& bb, HanaLovesMeID id, const vsrect* nav_bb = NULL);
    HanaLovesMe_API bool          ItemHoverable(const vsrect& bb, HanaLovesMeID id);
    HanaLovesMe_API bool          IsClippedEx(const vsrect& bb, HanaLovesMeID id, bool clip_even_when_logged);
    HanaLovesMe_API bool          FocusableItemRegister(HanaLovesMeWindow* window, HanaLovesMeID id);   // Return true if focus is requested
    HanaLovesMe_API void          FocusableItemUnregister(HanaLovesMeWindow* window);
    HanaLovesMe_API vsize        CalcItemSize(vsize size, float default_w, float default_h);
    HanaLovesMe_API float         CalcWrapWidthForPos(const vsize& pos, float wrap_pos_x);
    HanaLovesMe_API void          PushMultiItemsWidths(int components, float width_full = 0.0f);
    HanaLovesMe_API void          PushItemFlag(HanaLovesMeItemFlags option, bool enabled);
    HanaLovesMe_API void          PopItemFlag();
    HanaLovesMe_API bool          IsItemToggledSelection();                                           // was the last item selection toggled? (after Selectable(), TreeNode() etc. We only returns toggle _event_ in order to handle clipping correctly)
    HanaLovesMe_API vsize        GetContentRegionMaxScreen();

    // Logging/Capture
    HanaLovesMe_API void          LogBegin(HanaLovesMeLogType type, int auto_open_depth);   // -> BeginCapture() when we design v2 api, for now stay under the radar by using the old name.
    HanaLovesMe_API void          LogToBuffer(int auto_open_depth = -1);              // Start logging/capturing to internal buffer

    // Popups, Modals, Tooltips
    HanaLovesMe_API void          OpenPopupEx(HanaLovesMeID id);
    HanaLovesMe_API void          ClosePopupToLevel(int remaining, bool apply_focus_to_window_under);
    HanaLovesMe_API void          ClosePopupsOverWindow(HanaLovesMeWindow* ref_window);
    HanaLovesMe_API bool          IsPopupOpen(HanaLovesMeID id); // Test for id within current popup stack level (currently begin-ed into); this doesn't scan the whole popup stack!
    HanaLovesMe_API bool          BeginPopupEx(HanaLovesMeID id, HanaLovesMeWindowFlags extra_flags);
    HanaLovesMe_API void          BeginTooltipEx(HanaLovesMeWindowFlags extra_flags, bool override_previous_tooltip = true);
    HanaLovesMe_API HanaLovesMeWindow* GetFrontMostPopupModal();
    HanaLovesMe_API vsize        FindBestWindowPosForPopup(HanaLovesMeWindow* window);
    HanaLovesMe_API vsize        FindBestWindowPosForPopupEx(const vsize& ref_pos, const vsize& size, HanaLovesMeDir* last_dir, const vsrect& r_outer, const vsrect& r_avoid, HanaLovesMePopupPositionPolicy policy = HanaLovesMePopupPositionPolicy_Default);

    // Navigation
    HanaLovesMe_API void          NavInitWindow(HanaLovesMeWindow* window, bool force_reinit);
    HanaLovesMe_API bool          NavMoveRequestButNoResultYet();
    HanaLovesMe_API void          NavMoveRequestCancel();
    HanaLovesMe_API void          NavMoveRequestForward(HanaLovesMeDir move_dir, HanaLovesMeDir clip_dir, const vsrect& bb_rel, HanaLovesMeNavMoveFlags move_flags);
    HanaLovesMe_API void          NavMoveRequestTryWrapping(HanaLovesMeWindow* window, HanaLovesMeNavMoveFlags move_flags);
    HanaLovesMe_API float         GetNavInputAmount(HanaLovesMeNavInput n, HanaLovesMeInputReadMode mode);
    HanaLovesMe_API vsize        GetNavInputAmount2d(HanaLovesMeNavDirSourceFlags dir_sources, HanaLovesMeInputReadMode mode, float slow_factor = 0.0f, float fast_factor = 0.0f);
    HanaLovesMe_API int           CalcTypematicPressedRepeatAmount(float t, float t_prev, float repeat_delay, float repeat_rate);
    HanaLovesMe_API void          ActivateItem(HanaLovesMeID id);   // Remotely activate a button, checkbox, tree node etc. given its unique ID. activation is queued and processed on the next frame when the item is encountered again.
    HanaLovesMe_API void          SetNavID(HanaLovesMeID id, int nav_layer);
    HanaLovesMe_API void          SetNavIDWithRectRel(HanaLovesMeID id, int nav_layer, const vsrect& rect_rel);

    // Inputs
    inline bool             IsKeyPressedMap(HanaLovesMeKey key, bool repeat = true) { const int key_index = GHanaLovesMe->IO.KeyMap[key]; return (key_index >= 0) ? IsKeyPressed(key_index, repeat) : false; }
    inline bool             IsNavInputDown(HanaLovesMeNavInput n) { return GHanaLovesMe->IO.NavInputs[n] > 0.0f; }
    inline bool             IsNavInputPressed(HanaLovesMeNavInput n, HanaLovesMeInputReadMode mode) { return GetNavInputAmount(n, mode) > 0.0f; }
    inline bool             IsNavInputPressedAnyOfTwo(HanaLovesMeNavInput n1, HanaLovesMeNavInput n2, HanaLovesMeInputReadMode mode) { return (GetNavInputAmount(n1, mode) + GetNavInputAmount(n2, mode)) > 0.0f; }

    // Drag and Drop
    HanaLovesMe_API bool          BeginDragDropTargetCustom(const vsrect & bb, HanaLovesMeID id);
    HanaLovesMe_API void          ClearDragDrop();
    HanaLovesMe_API bool          IsDragDropPayloadBeingAccepted();

    // New Columns API (FIXME-WIP)
    HanaLovesMe_API void          BeginColumns(const char* str_id, int count, HanaLovesMeColumnsFlags flags = 0); // setup number of columns. use an identifier to distinguish multiple column sets. close with EndColumns().
    HanaLovesMe_API void          EndColumns();                                                             // close columns
    HanaLovesMe_API void          PushColumnClipRect(int column_index = -1);
    HanaLovesMe_API HanaLovesMeID       GetColumnsID(const char* str_id, int count);
    HanaLovesMe_API HanaLovesMeColumns * FindOrCreateColumns(HanaLovesMeWindow * window, HanaLovesMeID id);

    // Tab Bars
    HanaLovesMe_API bool          BeginTabBarEx(HanaLovesMeTabBar * tab_bar, const vsrect & bb, HanaLovesMeTabBarFlags flags);
    HanaLovesMe_API HanaLovesMeTabItem * TabBarFindTabByID(HanaLovesMeTabBar * tab_bar, HanaLovesMeID tab_id);
    HanaLovesMe_API void          TabBarRemoveTab(HanaLovesMeTabBar * tab_bar, HanaLovesMeID tab_id);
    HanaLovesMe_API void          TabBarCloseTab(HanaLovesMeTabBar * tab_bar, HanaLovesMeTabItem * tab);
    HanaLovesMe_API void          TabBarQueueChangeTabOrder(HanaLovesMeTabBar * tab_bar, const HanaLovesMeTabItem * tab, int dir);
    HanaLovesMe_API bool          TabItemEx(HanaLovesMeTabBar * tab_bar, const char* label, bool* p_open, HanaLovesMeTabItemFlags flags);
    HanaLovesMe_API vsize        TabItemCalcSize(const char* label, bool has_close_button);
    HanaLovesMe_API void          TabItemBackground(ImDrawList * draw_list, const vsrect & bb, HanaLovesMeTabItemFlags flags, ImU32 col);
    HanaLovesMe_API bool          TabItemLabelAndCloseButton(ImDrawList * draw_list, const vsrect & bb, HanaLovesMeTabItemFlags flags, vsize frame_padding, const char* label, HanaLovesMeID tab_id, HanaLovesMeID close_button_id);

    // Render helpers
    // AVOID USING OUTSIDE OF HanaLovesMe.CPP! NOT FOR PUBLIC CONSUMPTION. THOSE FUNCTIONS ARE A MESS. THEIR SIGNATURE AND BEHAVIOR WILL CHANGE, THEY NEED TO BE REFACTORED INTO SOMETHING DECENT.
    // NB: All position are in absolute pixels coordinates (we are never using window coordinates internally)
    HanaLovesMe_API void          RenderText(vsize pos, const char* text, const char* text_end = NULL, bool hide_text_after_hash = true);
    HanaLovesMe_API void          RenderTextWrapped(vsize pos, const char* text, const char* text_end, float wrap_width);
    HanaLovesMe_API void          RenderTextClipped(const vsize & pos_min, const vsize & pos_max, const char* text, const char* text_end, const vsize * text_size_if_known, const vsize & align = vsize(0, 0), const vsrect * clip_rect = NULL);
    HanaLovesMe_API void          RenderTextClippedEx(ImDrawList * draw_list, const vsize & pos_min, const vsize & pos_max, const char* text, const char* text_end, const vsize * text_size_if_known, const vsize & align = vsize(0, 0), const vsrect * clip_rect = NULL);
    HanaLovesMe_API void          RenderFrame(vsize p_min, vsize p_max, ImU32 fill_col, bool border = true, float rounding = 0.0f);
    HanaLovesMe_API void          RenderFrameBorder(vsize p_min, vsize p_max, float rounding = 0.0f);
    HanaLovesMe_API void          RenderColorRectWithAlphaCheckerboard(vsize p_min, vsize p_max, ImU32 fill_col, float grid_step, vsize grid_off, float rounding = 0.0f, int rounding_corners_flags = ~0);
    HanaLovesMe_API void          RenderArrow(vsize pos, HanaLovesMeDir dir, float scale = 1.0f);
    HanaLovesMe_API void          RenderBullet(vsize pos);
    HanaLovesMe_API void          RenderCheckMark(vsize pos, ImU32 col, float sz);
    HanaLovesMe_API void          RenderNavHighlight(const vsrect & bb, HanaLovesMeID id, HanaLovesMeNavHighlightFlags flags = HanaLovesMeNavHighlightFlags_TypeDefault); // Navigation highlight
    HanaLovesMe_API const char* FindRenderedTextEnd(const char* text, const char* text_end = NULL); // Find the optional ## from which we stop displaying text.
    HanaLovesMe_API void          LogRenderedText(const vsize * ref_pos, const char* text, const char* text_end = NULL);

    // Render helpers (those functions don't access any HanaLovesMe state!)
    HanaLovesMe_API void          RenderMouseCursor(ImDrawList * draw_list, vsize pos, float scale, HanaLovesMeMouseCursor mouse_cursor = HanaLovesMeMouseCursor_Arrow);
    HanaLovesMe_API void          RenderArrowPointingAt(ImDrawList * draw_list, vsize pos, vsize half_sz, HanaLovesMeDir direction, ImU32 col);
    HanaLovesMe_API void          RenderRectFilledRangeH(ImDrawList * draw_list, const vsrect & rect, ImU32 col, float x_start_norm, float x_end_norm, float rounding);
    HanaLovesMe_API void          RenderPixelEllipsis(ImDrawList * draw_list, vsize pos, int count, ImU32 col);

    // Widgets
    HanaLovesMe_API void          TextEx(const char* text, const char* text_end = NULL, HanaLovesMeTextFlags flags = 0);
    HanaLovesMe_API bool          ButtonEx(const char* label, const vsize & size_arg = vsize(0, 0), HanaLovesMeButtonFlags flags = 0);
	HanaLovesMe_API bool          ColorBarEx(const char* label, const vsize & size_arg = vsize(0, 0), HanaLovesMeButtonFlags flags = 0);
	HanaLovesMe_API bool          QuitButtonEx(const char* label, const vsize & size_arg = vsize(0, 0), HanaLovesMeButtonFlags flags = 0);
	HanaLovesMe_API bool          SelectedTabEx(const char* label, const vsize & size_arg = vsize(0, 0), HanaLovesMeButtonFlags flags = 0);
	HanaLovesMe_API bool          TabEx(const char* label, const vsize & size_arg = vsize(0, 0), HanaLovesMeButtonFlags flags = 0);
	HanaLovesMe_API bool          TabSpacerEx(const char* label, const vsize & size_arg = vsize(0, 0), HanaLovesMeButtonFlags flags = 0);
	HanaLovesMe_API bool          TabSpacer2Ex(const char* label, const vsize & size_arg = vsize(0, 0), HanaLovesMeButtonFlags flags = 0);
	HanaLovesMe_API bool          TabButtonEx(const char* label, const vsize & size_arg = vsize(0, 0), HanaLovesMeButtonFlags flags = 0);
	HanaLovesMe_API bool          GroupBoxTitleEx(const char* label, const vsize& size_arg = vsize(0, 0), HanaLovesMeButtonFlags flags = 0);
	HanaLovesMe_API bool			NoInputButtonEx(const char* label, const vsize & size_arg = vsize(0, 0), HanaLovesMeButtonFlags flags = 0);
    HanaLovesMe_API bool          CloseButton(HanaLovesMeID id, const vsize & pos, float radius);
    HanaLovesMe_API bool          CollapseButton(HanaLovesMeID id, const vsize & pos);
    HanaLovesMe_API bool          ArrowButtonEx(const char* str_id, HanaLovesMeDir dir, vsize size_arg, HanaLovesMeButtonFlags flags);
    HanaLovesMe_API void          Scrollbar(HanaLovesMeAxis axis);
    HanaLovesMe_API HanaLovesMeID       GetScrollbarID(HanaLovesMeWindow * window, HanaLovesMeAxis axis);
    HanaLovesMe_API void          VerticalSeparator();        // Vertical separator, for menu bars (use current line height). Not exposed because it is misleading and it doesn't have an effect on regular layout.

    // Widgets low-level behaviors
    HanaLovesMe_API bool          ButtonBehavior(const vsrect & bb, HanaLovesMeID id, bool* out_hovered, bool* out_held, HanaLovesMeButtonFlags flags = 0);
    HanaLovesMe_API bool          DragBehavior(HanaLovesMeID id, HanaLovesMeDataType data_type, void* v, float v_speed, const void* v_min, const void* v_max, const char* format, float power, HanaLovesMeDragFlags flags);
    HanaLovesMe_API bool          SliderBehavior(const vsrect & bb, HanaLovesMeID id, HanaLovesMeDataType data_type, void* v, const void* v_min, const void* v_max, const char* format, float power, HanaLovesMeSliderFlags flags, vsrect * out_grab_bb);
    HanaLovesMe_API bool          SplitterBehavior(const vsrect & bb, HanaLovesMeID id, HanaLovesMeAxis axis, float* size1, float* size2, float min_size1, float min_size2, float hover_extend = 0.0f, float hover_visibility_delay = 0.0f);
    HanaLovesMe_API bool          TreeNodeBehavior(HanaLovesMeID id, HanaLovesMeTreeNodeFlags flags, const char* label, const char* label_end = NULL);
    HanaLovesMe_API bool          TreeNodeBehaviorIsOpen(HanaLovesMeID id, HanaLovesMeTreeNodeFlags flags = 0);                     // Consume previous SetNextTreeNodeOpened() data, if any. May return true when logging
    HanaLovesMe_API void          TreePushRawID(HanaLovesMeID id);

    // Template functions are instantiated in HanaLovesMe_widgets.cpp for a finite number of types.
    // To use them externally (for custom widget) you may need an "extern template" statement in your code in order to link to existing instances and silence Clang warnings (see #2036).
    // e.g. " extern template HanaLovesMe_API float RoundScalarWithFormatT<float, float>(const char* format, HanaLovesMeDataType data_type, float v); "
    template<typename T, typename SIGNED_T, typename FLOAT_T>   HanaLovesMe_API bool  DragBehaviorT(HanaLovesMeDataType data_type, T * v, float v_speed, T v_min, T v_max, const char* format, float power, HanaLovesMeDragFlags flags);
    template<typename T, typename SIGNED_T, typename FLOAT_T>   HanaLovesMe_API bool  SliderBehaviorT(const vsrect & bb, HanaLovesMeID id, HanaLovesMeDataType data_type, T * v, T v_min, T v_max, const char* format, float power, HanaLovesMeSliderFlags flags, vsrect * out_grab_bb);
    template<typename T, typename FLOAT_T>                      HanaLovesMe_API float SliderCalcRatioFromValueT(HanaLovesMeDataType data_type, T v, T v_min, T v_max, float power, float linear_zero_pos);
    template<typename T, typename SIGNED_T>                     HanaLovesMe_API T     RoundScalarWithFormatT(const char* format, HanaLovesMeDataType data_type, T v);

    // InputText
    HanaLovesMe_API bool          InputTextEx(const char* label, const char* hint, char* buf, int buf_size, const vsize & size_arg, HanaLovesMeInputTextFlags flags, HanaLovesMeInputTextCallback callback = NULL, void* user_data = NULL);
    HanaLovesMe_API bool          InputScalarAsWidgetReplacement(const vsrect & bb, HanaLovesMeID id, const char* label, HanaLovesMeDataType data_type, void* data_ptr, const char* format);

    // Color
    HanaLovesMe_API void          ColorTooltip(const char* text, const float* col, HanaLovesMeColorEditFlags flags);
    HanaLovesMe_API void          ColorEditOptionsPopup(const float* col, HanaLovesMeColorEditFlags flags);
    HanaLovesMe_API void          ColorPickerOptionsPopup(const float* ref_col, HanaLovesMeColorEditFlags flags);

    // Plot
    HanaLovesMe_API void          PlotEx(HanaLovesMePlotType plot_type, const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, vsize frame_size);

    // Shade functions (write over already created vertices)
    HanaLovesMe_API void          ShadeVertsLinearColorGradientKeepAlpha(ImDrawList * draw_list, int vert_start_idx, int vert_end_idx, vsize gradient_p0, vsize gradient_p1, ImU32 col0, ImU32 col1);
    HanaLovesMe_API void          ShadeVertsLinearUV(ImDrawList * draw_list, int vert_start_idx, int vert_end_idx, const vsize & a, const vsize & b, const vsize & uv_a, const vsize & uv_b, bool clamp);

} // namespace HanaLovesMe

// ImFontAtlas internals
HanaLovesMe_API bool              ImFontAtlasBuildWithStbTruetype(ImFontAtlas * atlas);
HanaLovesMe_API void              ImFontAtlasBuildRegisterDefaultCustomRects(ImFontAtlas * atlas);
HanaLovesMe_API void              ImFontAtlasBuildSetupFont(ImFontAtlas * atlas, ImFont * font, ImFontConfig * font_config, float ascent, float descent);
HanaLovesMe_API void              ImFontAtlasBuildPackCustomRects(ImFontAtlas * atlas, void* stbrp_context_opaque);
HanaLovesMe_API void              ImFontAtlasBuildFinish(ImFontAtlas * atlas);
HanaLovesMe_API void              ImFontAtlasBuildMultiplyCalcLookupTable(unsigned char out_table[256], float in_multiply_factor);
HanaLovesMe_API void              ImFontAtlasBuildMultiplyRectAlpha8(const unsigned char table[256], unsigned char* pixels, int x, int y, int w, int h, int stride);

// Test engine hooks (HanaLovesMe-test)
//#define HanaLovesMe_ENABLE_TEST_ENGINE
#ifdef HanaLovesMe_ENABLE_TEST_ENGINE
extern void                 HanaLovesMeTestEngineHook_PreNewFrame(HanaLovesMeContext * ctx);
extern void                 HanaLovesMeTestEngineHook_PostNewFrame(HanaLovesMeContext * ctx);
extern void                 HanaLovesMeTestEngineHook_ItemAdd(HanaLovesMeContext * ctx, const vsrect & bb, HanaLovesMeID id);
extern void                 HanaLovesMeTestEngineHook_ItemInfo(HanaLovesMeContext * ctx, HanaLovesMeID id, const char* label, HanaLovesMeItemStatusFlags flags);
#define HanaLovesMe_TEST_ENGINE_ITEM_ADD(_BB, _ID)                HanaLovesMeTestEngineHook_ItemAdd(&g, _BB, _ID)               // Register status flags
#define HanaLovesMe_TEST_ENGINE_ITEM_INFO(_ID, _LABEL, _FLAGS)  HanaLovesMeTestEngineHook_ItemInfo(&g, _ID, _LABEL, _FLAGS)   // Register status flags
#else
#define HanaLovesMe_TEST_ENGINE_ITEM_ADD(_BB, _ID)                do { } while (0)
#define HanaLovesMe_TEST_ENGINE_ITEM_INFO(_ID, _LABEL, _FLAGS)  do { } while (0)
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif
