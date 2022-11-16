#pragma warning(disable: 4244)

// dear HanaLovesMe, v1.70 WIP
// (widgets code)

/*

Index of this file:

// [SECTION] Forward Declarations
// [SECTION] Widgets: Text, etc.
// [SECTION] Widgets: Main (Button, Image, Checkbox, RadioButton, ProgressBar, Bullet, etc.)
// [SECTION] Widgets: Low-level Layout helpers (Spacing, Dummy, NewLine, Separator, etc.)
// [SECTION] Widgets: ComboBox
// [SECTION] Data Type and Data Formatting Helpers
// [SECTION] Widgets: DragScalar, DragFloat, DragInt, etc.
// [SECTION] Widgets: SliderScalar, SliderFloat, SliderInt, etc.
// [SECTION] Widgets: InputScalar, InputFloat, InputInt, etc.
// [SECTION] Widgets: InputText, InputTextMultiline
// [SECTION] Widgets: ColorEdit, ColorPicker, ColorButton, etc.
// [SECTION] Widgets: TreeNode, CollapsingHeader, etc.
// [SECTION] Widgets: Selectable
// [SECTION] Widgets: ListBox
// [SECTION] Widgets: PlotLines, PlotHistogram
// [SECTION] Widgets: Value helpers
// [SECTION] Widgets: MenuItem, BeginMenu, EndMenu, etc.
// [SECTION] Widgets: BeginTabBar, EndTabBar, etc.
// [SECTION] Widgets: BeginTabItem, EndTabItem, etc.

*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "HanaLovesMe.h"
#ifndef HanaLovesMe_DEFINE_MATH_OPERATORS
#define HanaLovesMe_DEFINE_MATH_OPERATORS
#endif
#include "HanaLovesMe_internal.h"

#include <ctype.h>      // toupper
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>     // intptr_t
#else
#include <stdint.h>     // intptr_t
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4127) // condition expression is constant
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#endif

// Clang/GCC warnings with -Weverything
#ifdef __clang__
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"            // warning : comparing floating point with == or != is unsafe   // storing and comparing against same constants (typically 0.0f) is ok.
#pragma clang diagnostic ignored "-Wformat-nonliteral"      // warning : format string is not a string literal              // passing non-literal to vsnformat(). yes, user passing incorrect format strings can crash the code.
#pragma clang diagnostic ignored "-Wsign-conversion"        // warning : implicit conversion changes signedness             //
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning : zero as null pointer constant              // some standard header variations use #define NULL 0
#endif
#if __has_warning("-Wdouble-promotion")
#pragma clang diagnostic ignored "-Wdouble-promotion"       // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#endif
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wformat-nonliteral"        // warning: format not a string literal, format string not checked
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"          // warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif
#endif

//-------------------------------------------------------------------------
// Data
//-------------------------------------------------------------------------

// Those MIN/MAX values are not define because we need to point to them
static const signed char    IM_S8_MIN = -128;
static const signed char    IM_S8_MAX = 127;
static const unsigned char  IM_U8_MIN = 0;
static const unsigned char  IM_U8_MAX = 0xFF;
static const signed short   IM_S16_MIN = -32768;
static const signed short   IM_S16_MAX = 32767;
static const unsigned short IM_U16_MIN = 0;
static const unsigned short IM_U16_MAX = 0xFFFF;
static const ImS32          IM_S32_MIN = INT_MIN;    // (-2147483647 - 1), (0x80000000);
static const ImS32          IM_S32_MAX = INT_MAX;    // (2147483647), (0x7FFFFFFF)
static const ImU32          IM_U32_MIN = 0;
static const ImU32          IM_U32_MAX = UINT_MAX;   // (0xFFFFFFFF)
#ifdef LLONG_MIN
static const ImS64          IM_S64_MIN = LLONG_MIN;  // (-9223372036854775807ll - 1ll);
static const ImS64          IM_S64_MAX = LLONG_MAX;  // (9223372036854775807ll);
#else
static const ImS64          IM_S64_MIN = -9223372036854775807LL - 1;
static const ImS64          IM_S64_MAX = 9223372036854775807LL;
#endif
static const ImU64          IM_U64_MIN = 0;
#ifdef ULLONG_MAX
static const ImU64          IM_U64_MAX = ULLONG_MAX; // (0xFFFFFFFFFFFFFFFFull);
#else
static const ImU64          IM_U64_MAX = (2ULL * 9223372036854775807LL + 1);
#endif

//-------------------------------------------------------------------------
// [SECTION] Forward Declarations
//-------------------------------------------------------------------------

// Data Type helpers
static inline int       DataTypeFormatString(char* buf, int buf_size, HanaLovesMeDataType data_type, const void* data_ptr, const char* format);
static void             DataTypeApplyOp(HanaLovesMeDataType data_type, int op, void* output, void* arg_1, const void* arg_2);
static bool             DataTypeApplyOpFromText(const char* buf, const char* initial_value_buf, HanaLovesMeDataType data_type, void* data_ptr, const char* format);

// For InputTextEx()
static bool             InputTextFilterCharacter(unsigned int* p_char, HanaLovesMeInputTextFlags flags, HanaLovesMeInputTextCallback callback, void* user_data);
static int              InputTextCalcTextLenAndLineCount(const char* text_begin, const char** out_text_end);
static vsize           InputTextCalcTextSizeW(const ImWchar * text_begin, const ImWchar * text_end, const ImWchar * *remaining = NULL, vsize * out_offset = NULL, bool stop_on_new_line = false);

//-------------------------------------------------------------------------
// [SECTION] Widgets: Text, etc.
//-------------------------------------------------------------------------
// - TextUnformatted()
// - Text()
// - TextV()
// - TextColored()
// - TextColoredV()
// - TextDisabled()
// - TextDisabledV()
// - TextWrapped()
// - TextWrappedV()
// - LabelText()
// - LabelTextV()
// - BulletText()
// - BulletTextV()
//-------------------------------------------------------------------------

void HanaLovesMe::TextEx(const char* text, const char* text_end, HanaLovesMeTextFlags flags)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    HanaLovesMeContext& g = *GHanaLovesMe;
    IM_ASSERT(text != NULL);
    const char* text_begin = text;
    if (text_end == NULL)
        text_end = text + strlen(text); // FIXME-OPT

    const vsize text_pos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrentLineTextBaseOffset);
    const float wrap_pos_x = window->DC.TextWrapPos;
    const bool wrap_enabled = (wrap_pos_x >= 0.0f);
    if (text_end - text > 2000 && !wrap_enabled)
    {
        // Long text!
        // Perform manual coarse clipping to optimize for long multi-line text
        // - From this point we will only compute the width of lines that are visible. Optimization only available when word-wrapping is disabled.
        // - We also don't vertically center the text within the line full height, which is unlikely to matter because we are likely the biggest and only item on the line.
        // - We use memchr(), pay attention that well optimized versions of those str/mem functions are much faster than a casually written loop.
        const char* line = text;
        const float line_height = GetTextLineHeight();
        vsize text_size(0, 0);

        // Lines to skip (can't skip when logging text)
        vsize pos = text_pos;
        if (!g.LogEnabled)
        {
            int lines_skippable = (int)((window->ClipRect.Min.y - text_pos.y) / line_height);
            if (lines_skippable > 0)
            {
                int lines_skipped = 0;
                while (line < text_end && lines_skipped < lines_skippable)
                {
                    const char* line_end = (const char*)memchr(line, '\n', text_end - line);
                    if (!line_end)
                        line_end = text_end;
                    if ((flags & HanaLovesMeTextFlags_NoWidthForLargeClippedText) == 0)
                        text_size.x = ImMax(text_size.x, CalcTextSize(line, line_end).x);
                    line = line_end + 1;
                    lines_skipped++;
                }
                pos.y += lines_skipped * line_height;
            }
        }

        // Lines to render
        if (line < text_end)
        {
            vsrect line_rect(pos, pos + vsize(FLT_MAX, line_height));
            while (line < text_end)
            {
                if (IsClippedEx(line_rect, 0, false))
                    break;

                const char* line_end = (const char*)memchr(line, '\n', text_end - line);
                if (!line_end)
                    line_end = text_end;
                text_size.x = ImMax(text_size.x, CalcTextSize(line, line_end).x);
                RenderText(pos, line, line_end, false);
                line = line_end + 1;
                line_rect.Min.y += line_height;
                line_rect.Max.y += line_height;
                pos.y += line_height;
            }

            // Count remaining lines
            int lines_skipped = 0;
            while (line < text_end)
            {
                const char* line_end = (const char*)memchr(line, '\n', text_end - line);
                if (!line_end)
                    line_end = text_end;
                if ((flags & HanaLovesMeTextFlags_NoWidthForLargeClippedText) == 0)
                    text_size.x = ImMax(text_size.x, CalcTextSize(line, line_end).x);
                line = line_end + 1;
                lines_skipped++;
            }
            pos.y += lines_skipped * line_height;
        }
        text_size.y = (pos - text_pos).y;

        vsrect bb(text_pos, text_pos + text_size);
        ItemSize(text_size);
        ItemAdd(bb, 0);
    } else
    {
        const float wrap_width = wrap_enabled ? CalcWrapWidthForPos(window->DC.CursorPos, wrap_pos_x) : 0.0f;
        const vsize text_size = CalcTextSize(text_begin, text_end, false, wrap_width);

        vsrect bb(text_pos, text_pos + text_size);
        ItemSize(text_size);
        if (!ItemAdd(bb, 0))
            return;

        // Render (we don't hide text after ## in this end-user function)
		PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f));
		RenderTextWrapped(bb.Min + vsize(1, 1), text_begin, text_end, wrap_width);
		PopStyleColor();

        RenderTextWrapped(bb.Min, text_begin, text_end, wrap_width);
    }
}

void HanaLovesMe::TextUnformatted(const char* text, const char* text_end)
{
    TextEx(text, text_end, HanaLovesMeTextFlags_NoWidthForLargeClippedText);
}

void HanaLovesMe::Text(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextV(fmt, args);
    va_end(args);
}

void HanaLovesMe::TextV(const char* fmt, va_list args)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    HanaLovesMeContext& g = *GHanaLovesMe;
    const char* text_end = g.TempBuffer + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    TextEx(g.TempBuffer, text_end, HanaLovesMeTextFlags_NoWidthForLargeClippedText);
}

void HanaLovesMe::TextColored(const xuifloatcolor & col, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextColoredV(col, fmt, args);
    va_end(args);
}

void HanaLovesMe::TextColoredV(const xuifloatcolor & col, const char* fmt, va_list args)
{
    PushStyleColor(HanaLovesMeCol_Text, col);
    TextV(fmt, args);
    PopStyleColor();
}

void HanaLovesMe::TextDisabled(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextDisabledV(fmt, args);
    va_end(args);
}

void HanaLovesMe::TextDisabledV(const char* fmt, va_list args)
{
    PushStyleColor(HanaLovesMeCol_Text, GHanaLovesMe->Style.Colors[HanaLovesMeCol_TextDisabled]);
    TextV(fmt, args);
    PopStyleColor();
}

void HanaLovesMe::TextWrapped(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextWrappedV(fmt, args);
    va_end(args);
}

void HanaLovesMe::TextWrappedV(const char* fmt, va_list args)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    bool need_backup = (window->DC.TextWrapPos < 0.0f);  // Keep existing wrap position if one is already set
    if (need_backup)
        PushTextWrapPos(0.0f);
    TextV(fmt, args);
    if (need_backup)
        PopTextWrapPos();
}

void HanaLovesMe::LabelText(const char* label, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LabelTextV(label, fmt, args);
    va_end(args);
}

// Add a label+text combo aligned to other label+value widgets
void HanaLovesMe::LabelTextV(const char* label, const char* fmt, va_list args)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    HanaLovesMeContext& g = *GHanaLovesMe;
    const HanaLovesMeStyle& style = g.Style;
    const float w = CalcItemWidth();

    const vsize label_size = CalcTextSize(label, NULL, true);
    const vsrect value_bb(window->DC.CursorPos, window->DC.CursorPos + vsize(w, label_size.y + style.FramePadding.y * 2));
    const vsrect total_bb(window->DC.CursorPos, window->DC.CursorPos + vsize(w + (label_size.x > 0.0f ? style.ItemInnerSpacing.x : 0.0f), style.FramePadding.y * 2) + label_size);
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, 0))
        return;

    // Render
    const char* value_text_begin = &g.TempBuffer[0];
    const char* value_text_end = value_text_begin + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    RenderTextClipped(value_bb.Min, value_bb.Max, value_text_begin, value_text_end, NULL, vsize(0.0f, 0.5f));
    if (label_size.x > 0.0f)
        RenderText(vsize(value_bb.Max.x + style.ItemInnerSpacing.x, value_bb.Min.y + style.FramePadding.y), label);
}

void HanaLovesMe::BulletText(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    BulletTextV(fmt, args);
    va_end(args);
}

// Text with a little bullet aligned to the typical tree node.
void HanaLovesMe::BulletTextV(const char* fmt, va_list args)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    HanaLovesMeContext& g = *GHanaLovesMe;
    const HanaLovesMeStyle& style = g.Style;

    const char* text_begin = g.TempBuffer;
    const char* text_end = text_begin + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    const vsize label_size = CalcTextSize(text_begin, text_end, false);
    const float text_base_offset_y = ImMax(0.0f, window->DC.CurrentLineTextBaseOffset); // Latch before ItemSize changes it
    const float line_height = ImMax(ImMin(window->DC.CurrentLineSize.y, g.FontSize + g.Style.FramePadding.y * 2), g.FontSize);
    const vsrect bb(window->DC.CursorPos, window->DC.CursorPos + vsize(g.FontSize + (label_size.x > 0.0f ? (label_size.x + style.FramePadding.x * 2) : 0.0f), ImMax(line_height, label_size.y)));  // Empty text doesn't add padding
    ItemSize(bb);
    if (!ItemAdd(bb, 0))
        return;

    // Render
    RenderBullet(bb.Min + vsize(style.FramePadding.x + g.FontSize * 0.5f, line_height * 0.5f));
    RenderText(bb.Min + vsize(g.FontSize + style.FramePadding.x * 2, text_base_offset_y), text_begin, text_end, false);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Tabs
//-------------------------------------------------------------------------
// - SelectedTab() 
// - Tab()
// - TabButton()
//-------------------------------------------------------------------------
#include "../globals/HanaSong.h"



bool HanaLovesMe::SelectedTabEx(const char* label, const vsize& size_arg, HanaLovesMeButtonFlags flags)
{
	HanaLovesMeWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	HanaLovesMeContext& g = *GHanaLovesMe;

	const HanaLovesMeStyle& style = g.Style;
	const HanaLovesMeID id = window->GetID(label);
	const vsize label_size = CalcTextSize(label, NULL, true);

	vsize pos = window->DC.CursorPos;

	if ((flags & HanaLovesMeButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;

	vsize size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const vsrect bb(pos, pos + size);

	ItemSize(bb, style.FramePadding.y);

	if (!ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & HanaLovesMeItemFlags_ButtonRepeat)
		flags |= HanaLovesMeButtonFlags_Repeat;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	if (pressed)
		MarkItemEdited(id);

	// =========================
	// - Render
	// =========================

	auto innerBorderColor = xuicolor(0, 0, 0);
	auto outerBorderColor = xuicolor(48, 48, 48);
	
	window->DrawList->AddRect(bb.Min + vsize(-1.f, 1.f), bb.Max - vsize(0.f, 62.f), outerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRect(bb.Min - vsize(1.f, 0.f), bb.Max - vsize(0.5f, 62.f), innerBorderColor, 0.f, NULL, 1.f);

	//window->DrawList->AddRect(bb.Min + vsize(0.f, 74.f), bb.Max, outerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRect(bb.Min + vsize(0.f, 59.f), bb.Max, outerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRect(bb.Min + vsize(0.f, 60.f), bb.Max - vsize(1.f, 0.f), innerBorderColor, 0.f, NULL, 1.f);

	// Text
	PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(1.0f, 1.0f, 1.0f, 1.0f));
	RenderTextClipped(bb.Min + style.FramePadding + vsize(1, 1), bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	PopStyleColor();

	PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TabTextHovered, xuifloatcolor(1.0f, 1.0f, 1.0f, 1.0f));
	RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	PopStyleColor();

	return pressed;
}

bool HanaLovesMe::SelectedTab(const char* label, const vsize& size_arg)
{
	return SelectedTabEx(label, size_arg, 0);
}

bool HanaLovesMe::TabEx(const char* label, const vsize& size_arg, HanaLovesMeButtonFlags flags)
{
	HanaLovesMeWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	HanaLovesMeContext& g = *GHanaLovesMe;

	const HanaLovesMeStyle& style = g.Style;
	const HanaLovesMeID id = window->GetID(label);
	const vsize label_size = CalcTextSize(label, NULL, true);

	vsize pos = window->DC.CursorPos;

	if ((flags & HanaLovesMeButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;

	vsize size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const vsrect bb(pos, pos + size);

	ItemSize(bb, style.FramePadding.y);

	if (!ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & HanaLovesMeItemFlags_ButtonRepeat)
		flags |= HanaLovesMeButtonFlags_Repeat;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	if (pressed)
		MarkItemEdited(id);

	// =========================
	// - Render
	// =========================

	auto backgroundColor = xuicolor(12, 12, 12);
	auto innerBorderColor = xuicolor(0, 0, 0);
	auto outerBorderColor = xuicolor(48, 48, 48);

	window->DrawList->AddRectFilledMultiColor(bb.Min - vsize(0, 2), bb.Max, backgroundColor, backgroundColor, backgroundColor, backgroundColor);
	window->DrawList->AddRect(bb.Min - vsize(0, 2), bb.Max, outerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRect(bb.Min - vsize(0, 2), bb.Max - vsize(1, 0), innerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRectFilledMultiColor(bb.Min - vsize(0, 2), bb.Max - vsize(2, 0), backgroundColor, backgroundColor, backgroundColor, backgroundColor);

	// Text
	PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(1.0f, 1.0f, 1.0f, 1.0f));
	RenderTextClipped(bb.Min + style.FramePadding + vsize(1, 1), bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	PopStyleColor();

	if (hovered) {

		PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TabTextHovered, xuifloatcolor(1.0f, 1.0f, 1.0f, 1.0f));
		RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
		PopStyleColor();
	}
	else {

		PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TabText, xuifloatcolor(1.0f, 1.0f, 1.0f, 1.0f));
		RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
		PopStyleColor();
	}

	return pressed;
}

bool HanaLovesMe::Tab(const char* label, const vsize& size_arg)
{
	return TabEx(label, size_arg, 0);
}

bool HanaLovesMe::TabSpacerEx(const char* label, const vsize& size_arg, HanaLovesMeButtonFlags flags)
{
	HanaLovesMeWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	HanaLovesMeContext& g = *GHanaLovesMe;

	const HanaLovesMeStyle& style = g.Style;
	const HanaLovesMeID id = window->GetID(label);
	const vsize label_size = CalcTextSize(label, NULL, true);

	vsize pos = window->DC.CursorPos;

	if ((flags & HanaLovesMeButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;

	vsize size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const vsrect bb(pos, pos + size);

	ItemSize(bb, style.FramePadding.y);

	if (!ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & HanaLovesMeItemFlags_ButtonRepeat)
		flags |= HanaLovesMeButtonFlags_Repeat;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	if (pressed)
		MarkItemEdited(id);

	// =========================
	// - Render
	// =========================

	auto backgroundColor = xuicolor(12, 12, 12);
	auto innerBorderColor = xuicolor(0, 0, 0);
	auto outerBorderColor = xuicolor(48, 48, 48);

	window->DrawList->AddRectFilledMultiColor(bb.Min - vsize(0, 3), bb.Max, backgroundColor, backgroundColor, backgroundColor, backgroundColor);
	window->DrawList->AddRect(bb.Min - vsize(0, 3), bb.Max + vsize(0, 1), outerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRect(bb.Min - vsize(0, 3), bb.Max - vsize(1, 0), innerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRectFilledMultiColor(bb.Min - vsize(0, 3), bb.Max - vsize(2, 0), backgroundColor, backgroundColor, backgroundColor, backgroundColor);

	return pressed;
}

bool HanaLovesMe::TabSpacer(const char* label, const vsize& size_arg)
{
	return TabSpacerEx(label, size_arg, 0);
}

bool HanaLovesMe::TabSpacer2Ex(const char* label, const vsize& size_arg, HanaLovesMeButtonFlags flags)
{
	HanaLovesMeWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	HanaLovesMeContext& g = *GHanaLovesMe;

	const HanaLovesMeStyle& style = g.Style;
	const HanaLovesMeID id = window->GetID(label);
	const vsize label_size = CalcTextSize(label, NULL, true);

	vsize pos = window->DC.CursorPos;

	if ((flags & HanaLovesMeButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;

	vsize size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const vsrect bb(pos, pos + size);

	ItemSize(bb, style.FramePadding.y);

	if (!ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & HanaLovesMeItemFlags_ButtonRepeat)
		flags |= HanaLovesMeButtonFlags_Repeat;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	if (pressed)
		MarkItemEdited(id);

	// =========================
	// - Render
	// =========================

	auto backgroundColor = xuicolor(12, 12, 12);
	auto innerBorderColor = xuicolor(0, 0, 0);
	auto outerBorderColor = xuicolor(48, 48, 48);

	window->DrawList->AddRectFilledMultiColor(bb.Min - vsize(0, 2), bb.Max, backgroundColor, backgroundColor, backgroundColor, backgroundColor);
	window->DrawList->AddRect(bb.Min - vsize(0, 2), bb.Max, outerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRect(bb.Min - vsize(0, 2), bb.Max - vsize(1, 0), innerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRectFilledMultiColor(bb.Min - vsize(0, 2), bb.Max - vsize(2, 0), backgroundColor, backgroundColor, backgroundColor, backgroundColor);

	return pressed;
}

bool HanaLovesMe::TabSpacer2(const char* label, const vsize& size_arg)
{
	return TabSpacer2Ex(label, size_arg, 0);
}

bool HanaLovesMe::TabButtonEx(const char* label, const vsize& size_arg, HanaLovesMeButtonFlags flags)
{
	HanaLovesMeWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	HanaLovesMeContext& g = *GHanaLovesMe;

	const HanaLovesMeStyle& style = g.Style;
	const HanaLovesMeID id = window->GetID(label);
	const vsize label_size = CalcTextSize(label, NULL, true);

	vsize pos = window->DC.CursorPos;

	if ((flags & HanaLovesMeButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;

	vsize size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const vsrect bb(pos, pos + size);

	ItemSize(bb, style.FramePadding.y);

	if (!ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & HanaLovesMeItemFlags_ButtonRepeat)
		flags |= HanaLovesMeButtonFlags_Repeat;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	if (pressed)
		MarkItemEdited(id);

	// =========================
	// - Render
	// =========================

	if (hovered) {

		RenderFrame(bb.Min + vsize(2, 3), bb.Max + vsize(1, 0), xuicolor(56, 54, 58, 0), false, 4.f);
	}
	else {

		RenderFrame(bb.Min, bb.Max, xuicolor(56, 54, 58, 0), false, 4.f);
	}

	// Text
	PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(1.0f, 1.0f, 1.0f, 1.0f));
	RenderTextClipped(bb.Min + style.FramePadding + vsize(1, 1), bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	PopStyleColor();

	if (hovered) {

		PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TabTextHovered, xuifloatcolor(1.0f, 1.0f, 1.0f, 1.0f));
		RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
		PopStyleColor();
	}
	else {

		PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TabText, xuifloatcolor(1.0f, 1.0f, 1.0f, 1.0f));
		RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
		PopStyleColor();
	}

	return pressed;
}

bool HanaLovesMe::TabButton(const char* label, const vsize& size_arg)
{
	return TabButtonEx(label, size_arg, 0);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Main
//-------------------------------------------------------------------------
// - ButtonBehavior() [Internal]
// - Button()
// - SmallButton()
// - InvisibleButton()
// - ArrowButton()
// - CloseButton() [Internal]
// - CollapseButton() [Internal]
// - Scrollbar() [Internal]
// - Image()
// - ImageButton()
// - Checkbox()
// - CheckboxFlags()
// - RadioButton()
// - ProgressBar()
// - Bullet()
//-------------------------------------------------------------------------

// The ButtonBehavior() function is key to many interactions and used by many/most widgets.
// Because we handle so many cases (keyboard/gamepad navigation, drag and drop) and many specific behavior (via HanaLovesMeButtonFlags_),
// this code is a little complex.
// By far the most common path is interacting with the Mouse using the default HanaLovesMeButtonFlags_PressedOnClickRelease button behavior.
// See the series of events below and the corresponding state reported by dear HanaLovesMe:
//------------------------------------------------------------------------------------------------------------------------------------------------
// with PressedOnClickRelease:             return-value  IsItemHovered()  IsItemActive()  IsItemActivated()  IsItemDeactivated()  IsItemClicked()
//   Frame N+0 (mouse is outside bb)        -             -                -               -                  -                    -    
//   Frame N+1 (mouse moves inside bb)      -             true             -               -                  -                    -    
//   Frame N+2 (mouse button is down)       -             true             true            true               -                    true
//   Frame N+3 (mouse button is down)       -             true             true            -                  -                    -    
//   Frame N+4 (mouse moves outside bb)     -             -                true            -                  -                    -
//   Frame N+5 (mouse moves inside bb)      -             true             true            -                  -                    -
//   Frame N+6 (mouse button is released)   true          true             -               -                  true                 -    
//   Frame N+7 (mouse button is released)   -             true             -               -                  -                    -    
//   Frame N+8 (mouse moves outside bb)     -             -                -               -                  -                    -    
//------------------------------------------------------------------------------------------------------------------------------------------------
// with PressedOnClick:                    return-value  IsItemHovered()  IsItemActive()  IsItemActivated()  IsItemDeactivated()  IsItemClicked()
//   Frame N+2 (mouse button is down)       true          true             true            true               -                    true
//   Frame N+3 (mouse button is down)       -             true             true            -                  -                    -    
//   Frame N+6 (mouse button is released)   -             true             -               -                  true                 -    
//   Frame N+7 (mouse button is released)   -             true             -               -                  -                    -    
//------------------------------------------------------------------------------------------------------------------------------------------------
// with PressedOnRelease:                  return-value  IsItemHovered()  IsItemActive()  IsItemActivated()  IsItemDeactivated()  IsItemClicked()
//   Frame N+2 (mouse button is down)       -             true             -               -                  -                    true
//   Frame N+3 (mouse button is down)       -             true             -               -                  -                    -    
//   Frame N+6 (mouse button is released)   true          true             -               -                  -                    -
//   Frame N+7 (mouse button is released)   -             true             -               -                  -                    -    
//------------------------------------------------------------------------------------------------------------------------------------------------
// with PressedOnDoubleClick:              return-value  IsItemHovered()  IsItemActive()  IsItemActivated()  IsItemDeactivated()  IsItemClicked()
//   Frame N+0 (mouse button is down)       -             true             -               -                  -                    true
//   Frame N+1 (mouse button is down)       -             true             -               -                  -                    -    
//   Frame N+2 (mouse button is released)   -             true             -               -                  -                    -
//   Frame N+3 (mouse button is released)   -             true             -               -                  -                    -    
//   Frame N+4 (mouse button is down)       true          true             true            true               -                    true
//   Frame N+5 (mouse button is down)       -             true             true            -                  -                    -    
//   Frame N+6 (mouse button is released)   -             true             -               -                  true                 -
//   Frame N+7 (mouse button is released)   -             true             -               -                  -                    -    
//------------------------------------------------------------------------------------------------------------------------------------------------
// The behavior of the return-value changes when HanaLovesMeButtonFlags_Repeat is set:
//                                         Repeat+                  Repeat+           Repeat+             Repeat+
//                                         PressedOnClickRelease    PressedOnClick    PressedOnRelease    PressedOnDoubleClick
//-------------------------------------------------------------------------------------------------------------------------------------------------
//   Frame N+0 (mouse button is down)       -                        true              -                   true 
//   ...                                    -                        -                 -                   -
//   Frame N + RepeatDelay                  true                     true              -                   true
//   ...                                    -                        -                 -                   -
//   Frame N + RepeatDelay + RepeatRate*N   true                     true              -                   true
//-------------------------------------------------------------------------------------------------------------------------------------------------

bool HanaLovesMe::ButtonBehavior(const vsrect & bb, HanaLovesMeID id, bool* out_hovered, bool* out_held, HanaLovesMeButtonFlags flags)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    HanaLovesMeWindow* window = GetCurrentWindow();

    if (flags & HanaLovesMeButtonFlags_Disabled)
    {
        if (out_hovered)* out_hovered = false;
        if (out_held)* out_held = false;
        if (g.ActiveId == id) ClearActiveID();
        return false;
    }

    // Default behavior requires click+release on same spot
    if ((flags & (HanaLovesMeButtonFlags_PressedOnClickRelease | HanaLovesMeButtonFlags_PressedOnClick | HanaLovesMeButtonFlags_PressedOnRelease | HanaLovesMeButtonFlags_PressedOnDoubleClick)) == 0)
        flags |= HanaLovesMeButtonFlags_PressedOnClickRelease;

    HanaLovesMeWindow * backup_hovered_window = g.HoveredWindow;
    if ((flags & HanaLovesMeButtonFlags_FlattenChildren) && g.HoveredRootWindow == window)
        g.HoveredWindow = window;

#ifdef HanaLovesMe_ENABLE_TEST_ENGINE
    if (id != 0 && window->DC.LastItemId != id)
        HanaLovesMeTestEngineHook_ItemAdd(&g, bb, id);
#endif

    bool pressed = false;
    bool hovered = ItemHoverable(bb, id);

    // Drag source doesn't report as hovered
    if (hovered && g.DragDropActive && g.DragDropPayload.SourceId == id && !(g.DragDropSourceFlags & HanaLovesMeDragDropFlags_SourceNoDisableHover))
        hovered = false;

    // Special mode for Drag and Drop where holding button pressed for a long time while dragging another item triggers the button
    if (g.DragDropActive && (flags & HanaLovesMeButtonFlags_PressedOnDragDropHold) && !(g.DragDropSourceFlags & HanaLovesMeDragDropFlags_SourceNoHoldToOpenOthers))
        if (IsItemHovered(HanaLovesMeHoveredFlags_AllowWhenBlockedByActiveItem))
        {
            hovered = true;
            SetHoveredID(id);
            if (CalcTypematicPressedRepeatAmount(g.HoveredIdTimer + 0.0001f, g.HoveredIdTimer + 0.0001f - g.IO.DeltaTime, 0.01f, 0.70f)) // FIXME: Our formula for CalcTypematicPressedRepeatAmount() is fishy
            {
                pressed = true;
                FocusWindow(window);
            }
        }

    if ((flags & HanaLovesMeButtonFlags_FlattenChildren) && g.HoveredRootWindow == window)
        g.HoveredWindow = backup_hovered_window;

    // AllowOverlap mode (rarely used) requires previous frame HoveredId to be null or to match. This allows using patterns where a later submitted widget overlaps a previous one.
    if (hovered && (flags & HanaLovesMeButtonFlags_AllowItemOverlap) && (g.HoveredIdPreviousFrame != id && g.HoveredIdPreviousFrame != 0))
        hovered = false;

    // Mouse
    if (hovered)
    {
        if (!(flags & HanaLovesMeButtonFlags_NoKeyModifiers) || (!g.IO.KeyCtrl && !g.IO.KeyShift && !g.IO.KeyAlt))
        {
            if ((flags & HanaLovesMeButtonFlags_PressedOnClickRelease) && g.IO.MouseClicked[0])
            {
                SetActiveID(id, window);
                if (!(flags & HanaLovesMeButtonFlags_NoNavFocus))
                    SetFocusID(id, window);
                FocusWindow(window);
            }
            if (((flags & HanaLovesMeButtonFlags_PressedOnClick) && g.IO.MouseClicked[0]) || ((flags & HanaLovesMeButtonFlags_PressedOnDoubleClick) && g.IO.MouseDoubleClicked[0]))
            {
                pressed = true;
                if (flags & HanaLovesMeButtonFlags_NoHoldingActiveID)
                    ClearActiveID();
                else
                    SetActiveID(id, window); // Hold on ID
                FocusWindow(window);
            }
            if ((flags & HanaLovesMeButtonFlags_PressedOnRelease) && g.IO.MouseReleased[0])
            {
                if (!((flags & HanaLovesMeButtonFlags_Repeat) && g.IO.MouseDownDurationPrev[0] >= g.IO.KeyRepeatDelay))  // Repeat mode trumps <on release>
                    pressed = true;
                ClearActiveID();
            }

            // 'Repeat' mode acts when held regardless of _PressedOn flags (see table above).
            // Relies on repeat logic of IsMouseClicked() but we may as well do it ourselves if we end up exposing finer RepeatDelay/RepeatRate settings.
            if ((flags & HanaLovesMeButtonFlags_Repeat) && g.ActiveId == id && g.IO.MouseDownDuration[0] > 0.0f && IsMouseClicked(0, true))
                pressed = true;
        }

        if (pressed)
            g.NavDisableHighlight = true;
    }

    // Gamepad/Keyboard navigation
    // We report navigated item as hovered but we don't set g.HoveredId to not interfere with mouse.
    if (g.NavId == id && !g.NavDisableHighlight && g.NavDisableMouseHover && (g.ActiveId == 0 || g.ActiveId == id || g.ActiveId == window->MoveId))
        if (!(flags & HanaLovesMeButtonFlags_NoHoveredOnNav))
            hovered = true;

    if (g.NavActivateDownId == id)
    {
        bool nav_activated_by_code = (g.NavActivateId == id);
        bool nav_activated_by_inputs = IsNavInputPressed(HanaLovesMeNavInput_Activate, (flags & HanaLovesMeButtonFlags_Repeat) ? HanaLovesMeInputReadMode_Repeat : HanaLovesMeInputReadMode_Pressed);
        if (nav_activated_by_code || nav_activated_by_inputs)
            pressed = true;
        if (nav_activated_by_code || nav_activated_by_inputs || g.ActiveId == id)
        {
            // Set active id so it can be queried by user via IsItemActive(), equivalent of holding the mouse button.
            g.NavActivateId = id; // This is so SetActiveId assign a Nav source
            SetActiveID(id, window);
            if ((nav_activated_by_code || nav_activated_by_inputs) && !(flags & HanaLovesMeButtonFlags_NoNavFocus))
                SetFocusID(id, window);
            g.ActiveIdAllowNavDirFlags = (1 << HanaLovesMeDir_Left) | (1 << HanaLovesMeDir_Right) | (1 << HanaLovesMeDir_Up) | (1 << HanaLovesMeDir_Down);
        }
    }

    bool held = false;
    if (g.ActiveId == id)
    {
        if (pressed)
            g.ActiveIdHasBeenPressed = true;
        if (g.ActiveIdSource == HanaLovesMeInputSource_Mouse)
        {
            if (g.ActiveIdIsJustActivated)
                g.ActiveIdClickOffset = g.IO.MousePos - bb.Min;
            if (g.IO.MouseDown[0])
            {
                held = true;
            } else
            {
                if (hovered && (flags & HanaLovesMeButtonFlags_PressedOnClickRelease))
                    if (!((flags & HanaLovesMeButtonFlags_Repeat) && g.IO.MouseDownDurationPrev[0] >= g.IO.KeyRepeatDelay))  // Repeat mode trumps <on release>
                        if (!g.DragDropActive)
                            pressed = true;
                ClearActiveID();
            }
            if (!(flags & HanaLovesMeButtonFlags_NoNavFocus))
                g.NavDisableHighlight = true;
        } else if (g.ActiveIdSource == HanaLovesMeInputSource_Nav)
        {
            if (g.NavActivateDownId != id)
                ClearActiveID();
        }
    }

    if (out_hovered)* out_hovered = hovered;
    if (out_held)* out_held = held;

    return pressed;
}

bool HanaLovesMe::ButtonEx(const char* label, const vsize & size_arg, HanaLovesMeButtonFlags flags)
{
	HanaLovesMeContext& g = *GHanaLovesMe;
	HanaLovesMeCond backupNextWindowSizeConstraint = g.NextWindowData.SizeConstraintCond;
	g.NextWindowData.SizeConstraintCond = 0;

	HanaLovesMeWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	const HanaLovesMeStyle& style = g.Style;
	const HanaLovesMeID id = window->GetID(label);
	const float w = CalcItemWidth();

	const vsize labelSize = CalcTextSize(label, NULL, true);
	const vsrect framez_bb(window->DC.CursorPos, window->DC.CursorPos + vsize(w, labelSize.y + style.FramePadding.y * 2.0f) + vsize(0, 4));
	const vsrect totalz_bb(framez_bb.Min, framez_bb.Max + vsize(labelSize.x > 0.0f ? style.ItemInnerSpacing.x + labelSize.x : 0.0f, 0.0f));

	ItemSize(totalz_bb, style.FramePadding.y);

	if (!ItemAdd(totalz_bb, id))
		return false;

	bool hovered;
	bool held;
	bool pressed = ButtonBehavior(framez_bb, id, &hovered, &held);
	bool popupOpen = IsPopupOpen(id);
	const float arrowSize = GetFrameHeight();
	const vsrect value_bb(framez_bb.Min, framez_bb.Max + vsize(arrowSize, 0.0f));

    auto borderColorBlack = xuicolor(0, 0, 0, 255); // drawbuttonoutline
	auto borderColor = xuicolor(50, 50, 50, 255); // WAS 10 10 10
	auto topColor = xuicolor(35, 35, 35, 255);
	auto bottomColor = xuicolor(25, 25, 25, 255);
	auto topColorHovered = xuicolor(45, 45, 45, 255);
	auto bottomColorHovered = xuicolor(35, 35, 35, 255);

    auto topColorHeld= xuicolor(25, 25, 25, 255);
    auto bottomColorHeld = xuicolor(15, 15, 15, 255);

    // Render
    const ImU32 col = GetColorU32((held && hovered) ? HanaLovesMeCol_ButtonActive : hovered ? HanaLovesMeCol_ButtonHovered : HanaLovesMeCol_Button);

	if (hovered) {

		window->DrawList->AddRectFilledMultiColor(framez_bb.Min - vsize(0, 4), framez_bb.Max, topColorHovered, topColorHovered, bottomColorHovered, bottomColorHovered);
		window->DrawList->AddRect(framez_bb.Min - vsize(-1, 3), framez_bb.Max - vsize(1, 1), borderColor, 0, false, 1);
        window->DrawList->AddRect(framez_bb.Min - vsize(0, 4), framez_bb.Max, borderColorBlack, 0, false, 1);

	}
	else {

		window->DrawList->AddRectFilledMultiColor(framez_bb.Min - vsize(0, 4), framez_bb.Max, topColor, topColor, bottomColor, bottomColor);
		window->DrawList->AddRect(framez_bb.Min - vsize(-1, 3), framez_bb.Max - vsize(1, 1), borderColor, 0, false, 1);
        window->DrawList->AddRect(framez_bb.Min - vsize(0, 4), framez_bb.Max, borderColorBlack, 0, false, 1);

	}

    if (held) {
        window->DrawList->AddRectFilledMultiColor(framez_bb.Min - vsize(0, 4), framez_bb.Max, topColorHeld, topColorHeld, bottomColorHeld, bottomColorHeld);
        window->DrawList->AddRect(framez_bb.Min - vsize(-1, 3), framez_bb.Max - vsize(1, 1), borderColor, 0, false, 1);
        window->DrawList->AddRect(framez_bb.Min - vsize(0, 4), framez_bb.Max, borderColorBlack, 0, false, 1);


    }
    else {
        window->DrawList->AddRectFilledMultiColor(framez_bb.Min - vsize(0, 4), framez_bb.Max, topColor, topColor, bottomColor, bottomColor);
        window->DrawList->AddRect(framez_bb.Min - vsize(-1, 3), framez_bb.Max - vsize(1, 1), borderColor, 0, false, 1);
        window->DrawList->AddRect(framez_bb.Min - vsize(0, 4), framez_bb.Max, borderColorBlack, 0, false, 1);

    }


	PopFont();
	PushFont(globals::menuFont);

	if (labelSize.x > 0) {

		PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_Text, xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f));
		RenderTextClipped(framez_bb.Min + style.FramePadding, framez_bb.Max - style.FramePadding, label, NULL, &labelSize, style.ButtonTextAlign, &framez_bb); // imvec was 4
		PopStyleColor();
	}

    HanaLovesMe_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
    return pressed;
}

bool HanaLovesMe::Button(const char* label, const vsize & size_arg)
{
    return ButtonEx(label, size_arg, 0);
}

bool HanaLovesMe::ColorBarEx(const char* label, const vsize& size_arg, HanaLovesMeButtonFlags flags)
{
	HanaLovesMeWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	HanaLovesMeContext& g = *GHanaLovesMe;

	const HanaLovesMeStyle& style = g.Style;
	const HanaLovesMeID id = window->GetID(label);
	const vsize label_size = CalcTextSize(label, NULL, true);

	vsize pos = window->DC.CursorPos;

	if ((flags & HanaLovesMeButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;

	vsize size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const vsrect bb(pos, pos + size);

	ItemSize(bb, style.FramePadding.y);

	if (!ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & HanaLovesMeItemFlags_ButtonRepeat)
		flags |= HanaLovesMeButtonFlags_Repeat;

	// Render
	auto black = HanaLovesMe::GetColorU32(xuifloatcolor(0.0, 0.0, 0.0, 0.5)); // 0.43
	vsize p = HanaLovesMe::GetCursorScreenPos();

	window->DrawList->AddRectFilledMultiColor(vsize(p.x, p.y - 6), vsize(p.x + HanaLovesMe::GetWindowWidth() / 2, p.y - 4), xuicolor(12, 12, 12), xuicolor(12, 12, 12), xuicolor(12, 12, 12), xuicolor(12, 12, 12));
	window->DrawList->AddRectFilledMultiColor(vsize(p.x + HanaLovesMe::GetWindowWidth() / 2, p.y - 6), vsize(p.x + HanaLovesMe::GetWindowWidth(), p.y - 4), xuicolor(12, 12, 12), xuicolor(12, 12, 12), xuicolor(12, 12, 12), xuicolor(12, 12, 12));

	window->DrawList->AddRectFilledMultiColor(vsize(p.x + 1, p.y - 5), vsize(p.x + HanaLovesMe::GetWindowWidth() / 2, p.y - 3), xuicolor(55, 177, 218), xuicolor(201, 84, 192), xuicolor(201, 84, 192), xuicolor(55, 177, 218));
	window->DrawList->AddRectFilledMultiColor(vsize(p.x + HanaLovesMe::GetWindowWidth() / 2, p.y - 5), vsize(p.x + HanaLovesMe::GetWindowWidth() - 1, p.y - 3), xuicolor(201, 84, 192), xuicolor(204, 227, 54), xuicolor(204, 227, 54), xuicolor(201, 84, 192));

	window->DrawList->AddRectFilledMultiColor(vsize(p.x + 1, p.y - 4), vsize(p.x + HanaLovesMe::GetWindowWidth() / 2, p.y - 2), black, black, black, black);
	window->DrawList->AddRectFilledMultiColor(vsize(p.x + HanaLovesMe::GetWindowWidth() / 2, p.y - 4), vsize(p.x + HanaLovesMe::GetWindowWidth() - 1, p.y - 2), black, black, black, black);

	return false;
}

bool HanaLovesMe::ColorBar(const char* label, const vsize& size_arg)
{
	return ColorBarEx(label, size_arg, 0);
}

bool HanaLovesMe::QuitButtonEx(const char* label, const vsize & size_arg, HanaLovesMeButtonFlags flags)
{
	HanaLovesMeWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	HanaLovesMeContext& g = *GHanaLovesMe;
	const HanaLovesMeStyle& style = g.Style;
	const HanaLovesMeID id = window->GetID(label);
	const vsize label_size = CalcTextSize(label, NULL, true);

	vsize pos = window->DC.CursorPos;
	if ((flags & HanaLovesMeButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;
	vsize size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const vsrect bb(pos, pos + size);
	ItemSize(size, style.FramePadding.y);
	if (!ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & HanaLovesMeItemFlags_ButtonRepeat)
		flags |= HanaLovesMeButtonFlags_Repeat;
	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);
	if (pressed)
		MarkItemEdited(id);

	// Text
	PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(1.0f, 1.0f, 1.0f, 1.0f));
	RenderTextClipped(bb.Min + style.FramePadding + vsize(1, -1), bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	PopStyleColor();

	if (hovered) {

		PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TabTextHovered, xuifloatcolor(1.0f, 1.0f, 1.0f, 1.0f));
		RenderTextClipped(bb.Min + style.FramePadding - vsize(0, 2), bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
		PopStyleColor();
	}
	else {

		PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TabText, xuifloatcolor(1.0f, 1.0f, 1.0f, 1.0f));
		RenderTextClipped(bb.Min + style.FramePadding - vsize(0, 2), bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
		PopStyleColor();
	}

	HanaLovesMe_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
	return pressed;
}

bool HanaLovesMe::QuitButton(const char* label, const vsize & size_arg)
{
	return QuitButtonEx(label, size_arg, 0);
}

bool HanaLovesMe::NoInputButtonEx(const char* label, const vsize& size_arg, HanaLovesMeButtonFlags flags) {

	HanaLovesMeWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	HanaLovesMeContext& g = *GHanaLovesMe;

	const HanaLovesMeStyle& style = g.Style;
	const HanaLovesMeID id = window->GetID(label);
	const vsize label_size = CalcTextSize(label, NULL, true);

	vsize pos = window->DC.CursorPos;

	if ((flags & HanaLovesMeButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;

	vsize size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const vsrect bb(pos, pos + size);

	ItemSize(bb, style.FramePadding.y);

	if (!ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & HanaLovesMeItemFlags_ButtonRepeat)
		flags |= HanaLovesMeButtonFlags_Repeat;

	// Render
	const ImU32 col = xuicolor(0, 0, 0, 0);
	RenderNavHighlight(bb, id);
	RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

	PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(0.f, 0.f, 0.f, 0.f));
	RenderTextClipped(bb.Min + vsize(1, 1), bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	PopStyleColor();

	RenderTextClipped(bb.Min, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

	return false;
}

bool HanaLovesMe::NoInputButton(const char* label, const vsize& size_arg) {

	return NoInputButtonEx(label, size_arg, 0);
}

// Small buttons fits within text without additional vertical spacing.
bool HanaLovesMe::SmallButton(const char* label)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    float backup_padding_y = g.Style.FramePadding.y;
    g.Style.FramePadding.y = 0.0f;
    bool pressed = ButtonEx(label, vsize(0, 0), HanaLovesMeButtonFlags_AlignTextBaseLine);
    g.Style.FramePadding.y = backup_padding_y;
    return pressed;
}

// Tip: use HanaLovesMe::PushID()/PopID() to push indices or pointers in the ID stack.
// Then you can keep 'str_id' empty or the same for all your buttons (instead of creating a string based on a non-string id)
bool HanaLovesMe::InvisibleButton(const char* str_id, const vsize & size_arg)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    // Cannot use zero-size for InvisibleButton(). Unlike Button() there is not way to fallback using the label size.
    IM_ASSERT(size_arg.x != 0.0f && size_arg.y != 0.0f);

    const HanaLovesMeID id = window->GetID(str_id);
    vsize size = CalcItemSize(size_arg, 0.0f, 0.0f);
    const vsrect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(size);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    return pressed;
}

bool HanaLovesMe::ArrowButtonEx(const char* str_id, HanaLovesMeDir dir, vsize size, HanaLovesMeButtonFlags flags)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    HanaLovesMeContext& g = *GHanaLovesMe;
    const HanaLovesMeID id = window->GetID(str_id);
    const vsrect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    const float default_size = GetFrameHeight();
    ItemSize(size, (size.y >= default_size) ? g.Style.FramePadding.y : 0.0f);
    if (!ItemAdd(bb, id))
        return false;

    if (window->DC.ItemFlags & HanaLovesMeItemFlags_ButtonRepeat)
        flags |= HanaLovesMeButtonFlags_Repeat;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    // Render
    const ImU32 col = GetColorU32((held && hovered) ? HanaLovesMeCol_ButtonActive : hovered ? HanaLovesMeCol_ButtonHovered : HanaLovesMeCol_Button);
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, col, true, g.Style.FrameRounding);
    RenderArrow(bb.Min + vsize(ImMax(0.0f, (size.x - g.FontSize) * 0.5f), ImMax(0.0f, (size.y - g.FontSize) * 0.5f)), dir);

    return pressed;
}

bool HanaLovesMe::ArrowButton(const char* str_id, HanaLovesMeDir dir)
{
    float sz = GetFrameHeight();
    return ArrowButtonEx(str_id, dir, vsize(sz, sz), 0);
}

// Button to close a window
bool HanaLovesMe::CloseButton(HanaLovesMeID id, const vsize & pos, float radius)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    HanaLovesMeWindow* window = g.CurrentWindow;

    // We intentionally allow interaction when clipped so that a mechanical Alt,Right,Validate sequence close a window.
    // (this isn't the regular behavior of buttons, but it doesn't affect the user much because navigation tends to keep items visible).
    const vsrect bb(pos - vsize(radius, radius), pos + vsize(radius, radius));
    bool is_clipped = !ItemAdd(bb, id);

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    if (is_clipped)
        return pressed;

    // Render
    vsize center = bb.GetCenter();
    if (hovered)
        window->DrawList->AddCircleFilled(center, ImMax(2.0f, radius), GetColorU32(held ? HanaLovesMeCol_ButtonActive : HanaLovesMeCol_ButtonHovered), 9);

    float cross_extent = (radius * 0.7071f) - 1.0f;
    ImU32 cross_col = GetColorU32(HanaLovesMeCol_Text);
    center -= vsize(0.5f, 0.5f);
    window->DrawList->AddLine(center + vsize(+cross_extent, +cross_extent), center + vsize(-cross_extent, -cross_extent), cross_col, 1.0f);
    window->DrawList->AddLine(center + vsize(+cross_extent, -cross_extent), center + vsize(-cross_extent, +cross_extent), cross_col, 1.0f);

    return pressed;
}

bool HanaLovesMe::CollapseButton(HanaLovesMeID id, const vsize & pos)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    HanaLovesMeWindow* window = g.CurrentWindow;

    vsrect bb(pos, pos + vsize(g.FontSize, g.FontSize) + g.Style.FramePadding * 2.0f);
    ItemAdd(bb, id);
    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, HanaLovesMeButtonFlags_None);

    ImU32 col = GetColorU32((held && hovered) ? HanaLovesMeCol_ButtonActive : hovered ? HanaLovesMeCol_ButtonHovered : HanaLovesMeCol_Button);
    if (hovered || held)
        window->DrawList->AddCircleFilled(bb.GetCenter() + vsize(0.0f, -0.5f), g.FontSize * 0.5f + 1.0f, col, 9);
    RenderArrow(bb.Min + g.Style.FramePadding, window->Collapsed ? HanaLovesMeDir_Right : HanaLovesMeDir_Down, 1.0f);

    // Switch to moving the window after mouse is moved beyond the initial drag threshold
    if (IsItemActive() && IsMouseDragging())
        StartMouseMovingWindow(window);

    return pressed;
}

HanaLovesMeID HanaLovesMe::GetScrollbarID(HanaLovesMeWindow * window, HanaLovesMeAxis axis)
{
    return window->GetIDNoKeepAlive(axis == HanaLovesMeAxis_X ? "#SCROLLX" : "#SCROLLY");
}

void HanaLovesMe::Scrollbar(HanaLovesMeAxis axis)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    HanaLovesMeWindow* window = g.CurrentWindow;

    const bool horizontal = (axis == HanaLovesMeAxis_X);
    const HanaLovesMeStyle & style = g.Style;
    const HanaLovesMeID id = GetScrollbarID(window, axis);
    KeepAliveID(id);

    // Render background
    bool other_scrollbar = (horizontal ? window->ScrollbarY : window->ScrollbarX);
    float other_scrollbar_size_w = other_scrollbar ? style.ScrollbarSize : 0.0f;
    const vsrect host_rect = window->Rect();
    const float border_size = window->WindowBorderSize;

    vsrect bb = horizontal ? vsrect(host_rect.Min.x + border_size - 2, host_rect.Max.y - style.ScrollbarSize, host_rect.Max.x - other_scrollbar_size_w - border_size, host_rect.Max.y - border_size) : vsrect(host_rect.Max.x - style.ScrollbarSize, host_rect.Min.y + border_size - 2, host_rect.Max.x - border_size, host_rect.Max.y - other_scrollbar_size_w - border_size);
    bb.Min.x = ImMax(host_rect.Min.x, bb.Min.x);
    bb.Min.y = ImMax(host_rect.Min.y, bb.Min.y);

    if (!horizontal)
        bb.Min.y += window->TitleBarHeight() + ((window->Flags & HanaLovesMeWindowFlags_MenuBar) ? window->MenuBarHeight() : 0.0f); // FIXME: InnerRect?

    const float bb_width = bb.GetWidth();
    const float bb_height = bb.GetHeight();

    if (bb_width <= 0.0f || bb_height <= 0.0f)
        return;

    float alpha = 1.0f;

    if ((axis == HanaLovesMeAxis_Y) && bb_height < g.FontSize + g.Style.FramePadding.y * 2.0f) {

        alpha = ImSaturate((bb_height - g.FontSize) / (g.Style.FramePadding.y * 2.0f));

        if (alpha <= 0.0f)
            return;
    }

    const bool allow_interaction = (alpha >= 1.0f);
    int window_rounding_corners;

    if (horizontal)
        window_rounding_corners = ImDrawCornerFlags_BotLeft | (other_scrollbar ? 0 : ImDrawCornerFlags_BotRight);
    else
        window_rounding_corners = (((window->Flags & HanaLovesMeWindowFlags_NoTitleBar) && !(window->Flags & HanaLovesMeWindowFlags_MenuBar)) ? ImDrawCornerFlags_TopRight : 0) | (other_scrollbar ? 0 : ImDrawCornerFlags_BotRight);

    window->DrawList->AddRectFilled(bb.Min + vsize(-2, 1), bb.Max - vsize(1, 1), GetColorU32(HanaLovesMeCol_ScrollbarBg), window->WindowRounding, window_rounding_corners);
    bb.Expand(vsize(-ImClamp((float)(int)((bb_width - 2.0f) * 0.5f), 0.0f, 3.0f), -ImClamp((float)(int)((bb_height - 2.0f) * 0.5f), 0.0f, 3.0f)));

    float scrollbar_size_v = horizontal ? bb.GetWidth() : bb.GetHeight();
    float scroll_v = horizontal ? window->Scroll.x : window->Scroll.y;
    float win_size_avail_v = (horizontal ? window->SizeFull.x : window->SizeFull.y) - other_scrollbar_size_w;
    float win_size_contents_v = horizontal ? window->SizeContents.x : window->SizeContents.y;

    IM_ASSERT(ImMax(win_size_contents_v, win_size_avail_v) > 0.0f); // Adding this assert to check if the ImMax(XXX,1.0f) is still needed. PLEASE CONTACT ME if this triggers.
    const float win_size_v = ImMax(ImMax(win_size_contents_v, win_size_avail_v), 1.0f);
    const float grab_h_pixels = ImClamp(scrollbar_size_v * (win_size_avail_v / win_size_v), style.GrabMinSize, scrollbar_size_v);
    const float grab_h_norm = grab_h_pixels / scrollbar_size_v;

    bool held = false;
    bool hovered = false;
    const bool previously_held = (g.ActiveId == id);
    ButtonBehavior(bb, id, &hovered, &held, HanaLovesMeButtonFlags_NoNavFocus);

    float scroll_max = ImMax(1.0f, win_size_contents_v - win_size_avail_v);
    float scroll_ratio = ImSaturate(scroll_v / scroll_max);
    float grab_v_norm = scroll_ratio * (scrollbar_size_v - grab_h_pixels) / scrollbar_size_v;

    if (held && allow_interaction && grab_h_norm < 1.0f) {

        float scrollbar_pos_v = horizontal ? bb.Min.x : bb.Min.y;
        float mouse_pos_v = horizontal ? g.IO.MousePos.x : g.IO.MousePos.y;
        float* click_delta_to_grab_center_v = horizontal ? &g.ScrollbarClickDeltaToGrabCenter.x : &g.ScrollbarClickDeltaToGrabCenter.y;
        const float clicked_v_norm = ImSaturate((mouse_pos_v - scrollbar_pos_v) / scrollbar_size_v);
        SetHoveredID(id);

        bool seek_absolute = false;

        if (!previously_held) {

            if (clicked_v_norm >= grab_v_norm && clicked_v_norm <= grab_v_norm + grab_h_norm) {

                *click_delta_to_grab_center_v = clicked_v_norm - grab_v_norm - grab_h_norm * 0.5f;
            }
			else {

                seek_absolute = true;
                *click_delta_to_grab_center_v = 0.0f;
            }
        }

        const float scroll_v_norm = ImSaturate((clicked_v_norm - *click_delta_to_grab_center_v - grab_h_norm * 0.5f) / (1.0f - grab_h_norm));
        scroll_v = (float)(int)(0.5f + scroll_v_norm * scroll_max);

        if (horizontal)
            window->Scroll.x = scroll_v;
        else
            window->Scroll.y = scroll_v;

        scroll_ratio = ImSaturate(scroll_v / scroll_max);
        grab_v_norm = scroll_ratio * (scrollbar_size_v - grab_h_pixels) / scrollbar_size_v;

        if (seek_absolute)
            * click_delta_to_grab_center_v = clicked_v_norm - grab_v_norm - grab_h_norm * 0.5f;
    }

    // Render grab
    const ImU32 grab_col = GetColorU32(held ? HanaLovesMeCol_ScrollbarGrabActive : hovered ? HanaLovesMeCol_ScrollbarGrabHovered : HanaLovesMeCol_ScrollbarGrab, alpha);
    vsrect grab_rect;

    if (horizontal)
        grab_rect = vsrect(ImLerp(bb.Min.x, bb.Max.x, grab_v_norm), bb.Min.y, ImMin(ImLerp(bb.Min.x, bb.Max.x, grab_v_norm) + grab_h_pixels, host_rect.Max.x), bb.Max.y);
    else
        grab_rect = vsrect(bb.Min.x, ImLerp(bb.Min.y, bb.Max.y, grab_v_norm), bb.Max.x, ImMin(ImLerp(bb.Min.y, bb.Max.y, grab_v_norm) + grab_h_pixels, host_rect.Max.y));

    window->DrawList->AddRectFilled(grab_rect.Min - vsize(2, 1), grab_rect.Max + vsize(-1, 1), grab_col, style.ScrollbarRounding);
}

void HanaLovesMe::Image(ImTextureID user_texture_id, const vsize & size, const vsize & uv0, const vsize & uv1, const xuifloatcolor & tint_col, const xuifloatcolor & border_col)
{
    HanaLovesMeWindow* window = GetCurrentWindow();

    if (window->SkipItems)
        return;

    vsrect bb(window->DC.CursorPos, window->DC.CursorPos + size);

    if (border_col.w > 0.0f)
        bb.Max += vsize(2, 2);

    ItemSize(bb);

    if (!ItemAdd(bb, 0))
        return;

    if (border_col.w > 0.0f) {

        window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(border_col), 0.0f);
        window->DrawList->AddImage(user_texture_id, bb.Min + vsize(1, 1), bb.Max - vsize(1, 1), uv0, uv1, GetColorU32(tint_col));
    }
	else
        window->DrawList->AddImage(user_texture_id, bb.Min, bb.Max, uv0, uv1, GetColorU32(tint_col));
}

// frame_padding < 0: uses FramePadding from style (default)
// frame_padding = 0: no framing
// frame_padding > 0: set framing size
// The color used are the button colors.
bool HanaLovesMe::ImageButton(ImTextureID user_texture_id, const vsize & size, const vsize & uv0, const vsize & uv1, int frame_padding, const xuifloatcolor & bg_col, const xuifloatcolor & tint_col)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    HanaLovesMeContext& g = *GHanaLovesMe;
    const HanaLovesMeStyle& style = g.Style;

    // Default to using texture ID as ID. User can still push string/integer prefixes.
    // We could hash the size/uv to create a unique ID but that would prevent the user from animating UV.
    PushID((void*)(intptr_t)user_texture_id);
    const HanaLovesMeID id = window->GetID("#image");
    PopID();

    const vsize padding = (frame_padding >= 0) ? vsize((float)frame_padding, (float)frame_padding) : style.FramePadding;
    const vsrect bb(window->DC.CursorPos, window->DC.CursorPos + size + padding * 2);
    const vsrect image_bb(window->DC.CursorPos + padding, window->DC.CursorPos + padding + size);
    ItemSize(bb);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    // Render
    const ImU32 col = GetColorU32((held && hovered) ? HanaLovesMeCol_ButtonActive : hovered ? HanaLovesMeCol_ButtonHovered : HanaLovesMeCol_Button);
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));
    if (bg_col.w > 0.0f)
        window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, GetColorU32(bg_col));
    window->DrawList->AddImage(user_texture_id, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(tint_col));

    return pressed;
}

bool HanaLovesMe::Checkbox(const char* label, bool* v)
{
	HanaLovesMeWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	HanaLovesMeContext& g = *GHanaLovesMe;

	const HanaLovesMeStyle& style = g.Style;
	const HanaLovesMeID id = window->GetID(label);
	const vsize label_size = CalcTextSize(label, NULL, true);
	const vsrect check_bb(window->DC.CursorPos, window->DC.CursorPos + vsize(label_size.y + style.FramePadding.y * 1.f, label_size.y + style.FramePadding.y * 1.f)); // We want a square shape to we use Y twice

	ItemSize(check_bb, style.FramePadding.y);

	vsrect total_bb = check_bb;

	if (label_size.x > 0)
		SameLine(0, style.ItemInnerSpacing.x);

	const vsrect text_bb(window->DC.CursorPos + vsize(0, style.FramePadding.y), window->DC.CursorPos + vsize(0, style.FramePadding.y) + label_size);

	if (label_size.x > 0) {

		ItemSize(vsize(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
		total_bb = vsrect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
	}

	if (!ItemAdd(total_bb, id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);

	if (pressed) {

		*v = !(*v);
		MarkItemEdited(id);
	}

	RenderNavHighlight(total_bb, id);

	// colors
	auto borderColor = xuicolor(0, 0, 0, 255);
	auto topColor = xuicolor(55, 55, 55, 255);
	auto bottomColor = xuicolor(45, 45, 45, 255);
	auto topColorHovered = xuicolor(65, 65, 65, 255);
	auto bottomColorHovered = xuicolor(55, 55, 55, 255);
	auto checkedTopColor = GetColorU32(HanaLovesMeCol_MenuTheme);
	auto checkedBottomColor = GetColorU32(HanaLovesMeCol_MenuTheme) - xuicolor(0, 0, 0, 120);

	if (*v) {

		window->DrawList->AddRectFilledMultiColor(check_bb.Min + vsize(3, 3), check_bb.Max - vsize(3, 3), checkedTopColor, checkedTopColor, checkedBottomColor, checkedBottomColor);
		window->DrawList->AddRect(check_bb.Min + vsize(3, 3), check_bb.Max - vsize(3, 3), borderColor, 0, false, 1);
	}
	else {

		if (hovered) {

			window->DrawList->AddRectFilledMultiColor(check_bb.Min + vsize(3, 3), check_bb.Max - vsize(3, 3), topColorHovered, topColorHovered, bottomColorHovered, bottomColorHovered);
			window->DrawList->AddRect(check_bb.Min + vsize(3, 3), check_bb.Max - vsize(3, 3), borderColor, 0, false, 1);
		}
		else {

			window->DrawList->AddRectFilledMultiColor(check_bb.Min + vsize(3, 3), check_bb.Max - vsize(3, 3), topColor, topColor, bottomColor, bottomColor);
			window->DrawList->AddRect(check_bb.Min + vsize(3, 3), check_bb.Max - vsize(3, 3), borderColor, 0, false, 1);
		}
	}

	if (g.LogEnabled)
		LogRenderedText(&text_bb.Min, *v ? "[x]" : "[ ]");

	if (label_size.x > 0.0f) {

		//PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f));
		//RenderText(text_bb.Min + vsize(6.f, -2.f), label);
		//PopStyleColor();
		RenderText(text_bb.Min + vsize(5.f, -3.f), label);
	}

	HanaLovesMe_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags | HanaLovesMeItemStatusFlags_Checkable | (*v ? HanaLovesMeItemStatusFlags_Checked : 0));

	return pressed;
}

bool HanaLovesMe::CheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value)
{
    bool v = ((*flags & flags_value) == flags_value);
    bool pressed = Checkbox(label, &v);
    if (pressed)
    {
        if (v)
            * flags |= flags_value;
        else
            *flags &= ~flags_value;
    }

    return pressed;
}

bool HanaLovesMe::RadioButton(const char* label, bool active)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    HanaLovesMeContext& g = *GHanaLovesMe;
    const HanaLovesMeStyle& style = g.Style;
    const HanaLovesMeID id = window->GetID(label);
    const vsize label_size = CalcTextSize(label, NULL, true);

    const float square_sz = GetFrameHeight();
    const vsize pos = window->DC.CursorPos;
    const vsrect check_bb(pos, pos + vsize(square_sz, square_sz));
    const vsrect total_bb(pos, pos + vsize(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    vsize center = check_bb.GetCenter();
    center.x = (float)(int)center.x + 0.5f;
    center.y = (float)(int)center.y + 0.5f;
    const float radius = (square_sz - 1.0f) * 0.5f;

    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed)
        MarkItemEdited(id);

    RenderNavHighlight(total_bb, id);
    window->DrawList->AddCircleFilled(center, radius, GetColorU32((held && hovered) ? HanaLovesMeCol_FrameBgActive : hovered ? HanaLovesMeCol_FrameBgHovered : HanaLovesMeCol_FrameBg), 16);
    if (active)
    {
        const float pad = ImMax(1.0f, (float)(int)(square_sz / 6.0f));
        window->DrawList->AddCircleFilled(center, radius - pad, GetColorU32(HanaLovesMeCol_CheckMark), 16);
    }

    if (style.FrameBorderSize > 0.0f)
    {
        window->DrawList->AddCircle(center + vsize(1, 1), radius, GetColorU32(HanaLovesMeCol_BorderShadow), 16, style.FrameBorderSize);
        window->DrawList->AddCircle(center, radius, GetColorU32(HanaLovesMeCol_Border), 16, style.FrameBorderSize);
    }

    if (g.LogEnabled)
        LogRenderedText(&total_bb.Min, active ? "(x)" : "( )");
    if (label_size.x > 0.0f)
        RenderText(vsize(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y), label);

    return pressed;
}

bool HanaLovesMe::RadioButton(const char* label, int* v, int v_button)
{
    const bool pressed = RadioButton(label, *v == v_button);
    if (pressed)
        * v = v_button;
    return pressed;
}

// size_arg (for each axis) < 0.0f: align to end, 0.0f: auto, > 0.0f: specified size
void HanaLovesMe::ProgressBar(float fraction, const vsize & size_arg, const char* overlay)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    HanaLovesMeContext& g = *GHanaLovesMe;
    const HanaLovesMeStyle& style = g.Style;

    vsize pos = window->DC.CursorPos;
    vsize size = CalcItemSize(size_arg, CalcItemWidth(), g.FontSize + style.FramePadding.y * 2.0f);
    vsrect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, 0))
        return;

    // Render
    fraction = ImSaturate(fraction);
    RenderFrame(bb.Min, bb.Max, GetColorU32(HanaLovesMeCol_FrameBg), true, style.FrameRounding);
    bb.Expand(vsize(-style.FrameBorderSize, -style.FrameBorderSize));
    const vsize fill_br = vsize(ImLerp(bb.Min.x, bb.Max.x, fraction), bb.Max.y);
    RenderRectFilledRangeH(window->DrawList, bb, GetColorU32(HanaLovesMeCol_PlotHistogram), 0.0f, fraction, style.FrameRounding);

    // Default displaying the fraction as percentage string, but user can override it
    char overlay_buf[32];
    if (!overlay)
    {
        ImFormatString(overlay_buf, IM_ARRAYSIZE(overlay_buf), "%.0f%%", fraction * 100 + 0.01f);
        overlay = overlay_buf;
    }

    vsize overlay_size = CalcTextSize(overlay, NULL);
    if (overlay_size.x > 0.0f)
        RenderTextClipped(vsize(ImClamp(fill_br.x + style.ItemSpacing.x, bb.Min.x, bb.Max.x - overlay_size.x - style.ItemInnerSpacing.x), bb.Min.y), bb.Max, overlay, NULL, &overlay_size, vsize(0.0f, 0.5f), &bb);
}

void HanaLovesMe::Bullet()
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    HanaLovesMeContext& g = *GHanaLovesMe;
    const HanaLovesMeStyle& style = g.Style;
    const float line_height = ImMax(ImMin(window->DC.CurrentLineSize.y, g.FontSize + g.Style.FramePadding.y * 2), g.FontSize);
    const vsrect bb(window->DC.CursorPos, window->DC.CursorPos + vsize(g.FontSize, line_height));
    ItemSize(bb);
    if (!ItemAdd(bb, 0))
    {
        SameLine(0, style.FramePadding.x * 2);
        return;
    }

    // Render and stay on same line
    RenderBullet(bb.Min + vsize(style.FramePadding.x + g.FontSize * 0.5f, line_height * 0.5f));
    SameLine(0, style.FramePadding.x * 2);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Low-level Layout helpers
//-------------------------------------------------------------------------
// - Spacing()
// - Dummy()
// - NewLine()
// - AlignTextToFramePadding()
// - Separator()
// - VerticalSeparator() [Internal]
// - SplitterBehavior() [Internal]
//-------------------------------------------------------------------------

void HanaLovesMe::Spacing()
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    ItemSize(vsize(0, 0));
}

void HanaLovesMe::CustomSpacing(float h) {

	HanaLovesMeWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return;

	ItemSize(vsize(0, h));
}

void HanaLovesMe::Dummy(const vsize & size)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const vsrect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(size);
    ItemAdd(bb, 0);
}

void HanaLovesMe::NewLine()
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    HanaLovesMeContext& g = *GHanaLovesMe;
    const HanaLovesMeLayoutType backup_layout_type = window->DC.LayoutType;
    window->DC.LayoutType = HanaLovesMeLayoutType_Vertical;
    if (window->DC.CurrentLineSize.y > 0.0f)     // In the event that we are on a line with items that is smaller that FontSize high, we will preserve its height.
        ItemSize(vsize(0, 0));
    else
        ItemSize(vsize(0.0f, g.FontSize));
    window->DC.LayoutType = backup_layout_type;
}

void HanaLovesMe::AlignTextToFramePadding()
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    HanaLovesMeContext& g = *GHanaLovesMe;
    window->DC.CurrentLineSize.y = ImMax(window->DC.CurrentLineSize.y, g.FontSize + g.Style.FramePadding.y * 2);
    window->DC.CurrentLineTextBaseOffset = ImMax(window->DC.CurrentLineTextBaseOffset, g.Style.FramePadding.y);
}

// Horizontal/vertical separating line
void HanaLovesMe::Separator()
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    HanaLovesMeContext& g = *GHanaLovesMe;

    // Those flags should eventually be overridable by the user
    HanaLovesMeSeparatorFlags flags = (window->DC.LayoutType == HanaLovesMeLayoutType_Horizontal) ? HanaLovesMeSeparatorFlags_Vertical : HanaLovesMeSeparatorFlags_Horizontal;
    IM_ASSERT(ImIsPowerOfTwo(flags & (HanaLovesMeSeparatorFlags_Horizontal | HanaLovesMeSeparatorFlags_Vertical)));   // Check that only 1 option is selected
    if (flags & HanaLovesMeSeparatorFlags_Vertical)
    {
        VerticalSeparator();
        return;
    }

    // Horizontal Separator
    if (window->DC.CurrentColumns)
        PopClipRect();

    float x1 = window->Pos.x;
    float x2 = window->Pos.x + window->Size.x;
    if (!window->DC.GroupStack.empty())
        x1 += window->DC.Indent.x;

    const vsrect bb(vsize(x1, window->DC.CursorPos.y), vsize(x2, window->DC.CursorPos.y + 1.0f));
    ItemSize(vsize(0.0f, 0.0f)); // NB: we don't provide our width so that it doesn't get feed back into AutoFit, we don't provide height to not alter layout.
    if (!ItemAdd(bb, 0))
    {
        if (window->DC.CurrentColumns)
            PushColumnClipRect();
        return;
    }

    window->DrawList->AddLine(bb.Min, vsize(bb.Max.x, bb.Min.y), GetColorU32(HanaLovesMeCol_Separator));

    if (g.LogEnabled)
        LogRenderedText(&bb.Min, "--------------------------------");

    if (window->DC.CurrentColumns)
    {
        PushColumnClipRect();
        window->DC.CurrentColumns->LineMinY = window->DC.CursorPos.y;
    }
}

void HanaLovesMe::VerticalSeparator()
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    HanaLovesMeContext& g = *GHanaLovesMe;

    float y1 = window->DC.CursorPos.y;
    float y2 = window->DC.CursorPos.y + window->DC.CurrentLineSize.y;
    const vsrect bb(vsize(window->DC.CursorPos.x, y1), vsize(window->DC.CursorPos.x + 1.0f, y2));
    ItemSize(vsize(bb.GetWidth(), 0.0f));
    if (!ItemAdd(bb, 0))
        return;

    window->DrawList->AddLine(vsize(bb.Min.x, bb.Min.y), vsize(bb.Min.x, bb.Max.y), GetColorU32(HanaLovesMeCol_Separator));
    if (g.LogEnabled)
        LogText(" |");
}

// Using 'hover_visibility_delay' allows us to hide the highlight and mouse cursor for a short time, which can be convenient to reduce visual noise.
bool HanaLovesMe::SplitterBehavior(const vsrect & bb, HanaLovesMeID id, HanaLovesMeAxis axis, float* size1, float* size2, float min_size1, float min_size2, float hover_extend, float hover_visibility_delay)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    HanaLovesMeWindow* window = g.CurrentWindow;

    const HanaLovesMeItemFlags item_flags_backup = window->DC.ItemFlags;
    window->DC.ItemFlags |= HanaLovesMeItemFlags_NoNav | HanaLovesMeItemFlags_NoNavDefaultFocus;
    bool item_add = ItemAdd(bb, id);
    window->DC.ItemFlags = item_flags_backup;
    if (!item_add)
        return false;

    bool hovered, held;
    vsrect bb_interact = bb;
    bb_interact.Expand(axis == HanaLovesMeAxis_Y ? vsize(0.0f, hover_extend) : vsize(hover_extend, 0.0f));
    ButtonBehavior(bb_interact, id, &hovered, &held, HanaLovesMeButtonFlags_FlattenChildren | HanaLovesMeButtonFlags_AllowItemOverlap);
    if (g.ActiveId != id)
        SetItemAllowOverlap();

    if (held || (g.HoveredId == id && g.HoveredIdPreviousFrame == id && g.HoveredIdTimer >= hover_visibility_delay))
        SetMouseCursor(axis == HanaLovesMeAxis_Y ? HanaLovesMeMouseCursor_ResizeNS : HanaLovesMeMouseCursor_ResizeEW);

    vsrect bb_render = bb;
    if (held)
    {
        vsize mouse_delta_2d = g.IO.MousePos - g.ActiveIdClickOffset - bb_interact.Min;
        float mouse_delta = (axis == HanaLovesMeAxis_Y) ? mouse_delta_2d.y : mouse_delta_2d.x;

        // Minimum pane size
        float size_1_maximum_delta = ImMax(0.0f, *size1 - min_size1);
        float size_2_maximum_delta = ImMax(0.0f, *size2 - min_size2);
        if (mouse_delta < -size_1_maximum_delta)
            mouse_delta = -size_1_maximum_delta;
        if (mouse_delta > size_2_maximum_delta)
            mouse_delta = size_2_maximum_delta;

        // Apply resize
        if (mouse_delta != 0.0f)
        {
            if (mouse_delta < 0.0f)
                IM_ASSERT(*size1 + mouse_delta >= min_size1);
            if (mouse_delta > 0.0f)
                IM_ASSERT(*size2 - mouse_delta >= min_size2);
            *size1 += mouse_delta;
            *size2 -= mouse_delta;
            bb_render.Translate((axis == HanaLovesMeAxis_X) ? vsize(mouse_delta, 0.0f) : vsize(0.0f, mouse_delta));
            MarkItemEdited(id);
        }
    }

    // Render
    const ImU32 col = GetColorU32(held ? HanaLovesMeCol_SeparatorActive : (hovered && g.HoveredIdTimer >= hover_visibility_delay) ? HanaLovesMeCol_SeparatorHovered : HanaLovesMeCol_Separator);
    window->DrawList->AddRectFilled(bb_render.Min, bb_render.Max, col, g.Style.FrameRounding);

    return held;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: ComboBox
//-------------------------------------------------------------------------
// - BeginCombo()
// - MultiCombo()
// - EndCombo()
// - Combo()
//-------------------------------------------------------------------------



static float CalcMaxPopupHeightFromItemCount(int items_count)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    if (items_count <= 0)
        return FLT_MAX;
    return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

bool HanaLovesMe::MultiCombo(const char* name, const char** displayName, bool* data, int dataSize) {

	HanaLovesMe::PushID(name);

	char previewText[1024] = { 0 };
	char buf[1024] = { 0 };
	char buf2[1024] = { 0 };
	int currentPreviewTextLen = 0;
	float multicomboWidth = 115.f;

	for (int i = 0; i < dataSize; i++) {

		if (data[i] == true) {

			if (currentPreviewTextLen == 0)
				sprintf(buf, "%s", displayName[i]);
			else
				sprintf(buf, ", %s", displayName[i]);

			strcpy(buf2, previewText);
			sprintf(buf2 + currentPreviewTextLen, buf);
			vsize textSize = HanaLovesMe::CalcTextSize(buf2);

			if (textSize.x > multicomboWidth) {

				sprintf(previewText + currentPreviewTextLen, "...");
				currentPreviewTextLen += (int)strlen("...");
				break;
			}

			sprintf(previewText + currentPreviewTextLen, buf);
			currentPreviewTextLen += (int)strlen(buf);
		}
	}

	if (currentPreviewTextLen > 0)
		previewText[currentPreviewTextLen] = NULL;
	else
		sprintf(previewText, " -");

	bool isDataChanged = false;

	if (HanaLovesMe::BeginCombo(name, previewText)) {

		for (int i = 0; i < dataSize; i++) {

			sprintf(buf, displayName[i]);

			if (HanaLovesMe::Selectable(buf, data[i], HanaLovesMeSelectableFlags_DontClosePopups)) {

				data[i] = !data[i];
				isDataChanged = true;
			}
		}

		HanaLovesMe::EndCombo();
	}

	HanaLovesMe::PopID();
	return isDataChanged;
}



bool HanaLovesMe::BeginCombo(const char* label, const char* preview_value, HanaLovesMeComboFlags flags)
{
	HanaLovesMeContext& g = *GHanaLovesMe;
	HanaLovesMeCond backupNextWindowSizeConstraint = g.NextWindowData.SizeConstraintCond;
	g.NextWindowData.SizeConstraintCond = 0;

	HanaLovesMeWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	const HanaLovesMeStyle& style = g.Style;
	const HanaLovesMeID id = window->GetID(label);
	const float w = CalcItemWidth();

	const vsize labelSize = CalcTextSize(label, NULL, true);
	const vsrect frame_bb(window->DC.CursorPos, window->DC.CursorPos + vsize(w, labelSize.y + style.FramePadding.y * 2.0f) + vsize(0, 2));
	const vsrect total_bb(frame_bb.Min, frame_bb.Max + vsize(labelSize.x > 0.0f ? style.ItemInnerSpacing.x + labelSize.x : 0.0f, 0.0f));

	ItemSize(total_bb, style.FramePadding.y);

	if (!ItemAdd(total_bb, id))
		return false;

	bool hovered;
	bool held;
	bool pressed = ButtonBehavior(frame_bb, id, &hovered, &held);
	bool popupOpen = IsPopupOpen(id);
	const float arrowSize = GetFrameHeight();
	const vsrect value_bb(frame_bb.Min, frame_bb.Max + vsize(arrowSize, 0.0f));

	auto borderColor = xuicolor(0, 0, 0, 255);
	auto topColor = xuicolor(30, 30, 30, 255);
	auto bottomColor = xuicolor(37, 37, 37, 255);
	auto topColorHovered = xuicolor(30, 30, 30, 255);
	auto bottomColorHovered = xuicolor(37, 37, 37, 255);

	if (hovered || popupOpen) {

		window->DrawList->AddRectFilledMultiColor(frame_bb.Min - vsize(0, 1), frame_bb.Max, topColorHovered, topColorHovered, bottomColorHovered, bottomColorHovered);
		window->DrawList->AddRect(frame_bb.Min - vsize(0, 1), frame_bb.Max, borderColor, 0, false, 1);
	}
	else {

		window->DrawList->AddRectFilledMultiColor(frame_bb.Min - vsize(0, 1), frame_bb.Max, topColor, topColor, bottomColor, bottomColor);
		window->DrawList->AddRect(frame_bb.Min - vsize(0, 1), frame_bb.Max, borderColor, 0, false, 1);
	}

	PopFont();
	PushFont(globals::controlFont);

	PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(0.635f, 0.635f, 0.635f, 1.0f));
	RenderTextClipped(frame_bb.Min + style.FramePadding + vsize(144, -5), value_bb.Max, "A", NULL, NULL, vsize(0.0f, 0.0f));
	PopStyleColor();

	PushStyleColor(HanaLovesMeCol_Text, xuifloatcolor(0.635f, 0.635f, 0.635f, 1.0f));
	RenderTextClipped(frame_bb.Min + style.FramePadding + vsize(144, -4), value_bb.Max, "A", NULL, NULL, vsize(0.0f, 0.0f));
	PopStyleColor();

	PopFont();
	PushFont(globals::menuFont);

	if (preview_value != NULL) {

		//PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f));
		//RenderTextClipped(frame_bb.Min + style.FramePadding + vsize(5, -1), value_bb.Max + vsize(1, 1), preview_value, NULL, NULL, vsize(0.0f, 0.0f));
		//PopStyleColor();

		PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_Text, xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f));
		RenderTextClipped(frame_bb.Min + style.FramePadding + vsize(4, 0), value_bb.Max + vsize(1, 1), preview_value, NULL, NULL, vsize(0.0f, 0.0f));
		PopStyleColor();
	}

	if (labelSize.x > 0) {

		//PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f));
		//RenderText(vsize(frame_bb.Max.x + style.ItemInnerSpacing.x - 161, frame_bb.Min.y + style.FramePadding.y - 16), label);
		//PopStyleColor();

		RenderText(vsize(frame_bb.Max.x + style.ItemInnerSpacing.x - 162, frame_bb.Min.y + style.FramePadding.y - 17), label);
	}

	if ((pressed || g.NavActivateId == id) && !popupOpen) {

		if (window->DC.NavLayerCurrent == 0)
			window->NavLastIds[0] = id;

		HanaLovesMeStyle* fuckingStyle = &HanaLovesMe::GetStyle();
		fuckingStyle->ItemSpacing = vsize(4, 5);

		OpenPopupEx(id);
		popupOpen = true;
	}

	if (!popupOpen)
		return false;

	if (backupNextWindowSizeConstraint) {

		g.NextWindowData.SizeConstraintCond = backupNextWindowSizeConstraint;
		g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
	}
	else {

		if ((flags & HanaLovesMeComboFlags_HeightMask_) == 0)
			flags |= HanaLovesMeComboFlags_HeightRegular;

		IM_ASSERT(ImIsPowerOfTwo(flags & HanaLovesMeComboFlags_HeightMask_));    // Only one
		int popup_max_height_in_items = 24;

		if (flags & HanaLovesMeComboFlags_HeightRegular)
			popup_max_height_in_items = 24;
		else if (flags & HanaLovesMeComboFlags_HeightSmall)
			popup_max_height_in_items = 24;
		else if (flags & HanaLovesMeComboFlags_HeightLarge)
			popup_max_height_in_items = 24;

		SetNextWindowSizeConstraints(vsize(w, 0.0f), vsize(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));
	}

	char name[16];
	ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

	if (HanaLovesMeWindow* popup_window = FindWindowByName(name)) {

		if (popup_window->WasActive) {

			vsize size_expected = CalcWindowExpectedSize(popup_window);

			if (flags & HanaLovesMeComboFlags_PopupAlignLeft)
				popup_window->AutoPosLastDirection = HanaLovesMeDir_Left;

			vsrect r_outer = GetWindowAllowedExtentRect(popup_window);
			vsize pos = FindBestWindowPosForPopupEx(frame_bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, frame_bb, HanaLovesMePopupPositionPolicy_ComboBox);
			SetNextWindowPos(pos);
		}
	}

	HanaLovesMeWindowFlags window_flags = HanaLovesMeWindowFlags_AlwaysAutoResize | HanaLovesMeWindowFlags_Popup | HanaLovesMeWindowFlags_NoTitleBar | HanaLovesMeWindowFlags_NoResize | HanaLovesMeWindowFlags_NoSavedSettings;

	PushStyleVar(HanaLovesMeStyleVar_WindowPadding, vsize(style.FramePadding.x, style.WindowPadding.y));

	bool ret = BeginComboBackground(name, NULL, window_flags);

	PopStyleVar();

	if (!ret) {

		EndPopup();
		IM_ASSERT(0);   // This should never happen as we tested for IsPopupOpen() above
		return false;
	}

	return true;
}

void HanaLovesMe::EndCombo()
{
    EndPopup();
}

// Getter for the old Combo() API: const char*[]
static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
    const char* const* items = (const char* const*)data;
    if (out_text)
        * out_text = items[idx];
    return true;
}

// Getter for the old Combo() API: "item1\0item2\0item3\0"
static bool Items_SingleStringGetter(void* data, int idx, const char** out_text)
{
    // FIXME-OPT: we could pre-compute the indices to fasten this. But only 1 active combo means the waste is limited.
    const char* items_separated_by_zeros = (const char*)data;
    int items_count = 0;
    const char* p = items_separated_by_zeros;
    while (*p)
    {
        if (idx == items_count)
            break;
        p += strlen(p) + 1;
        items_count++;
    }
    if (!*p)
        return false;
    if (out_text)
        * out_text = p;
    return true;
}

// Old API, prefer using BeginCombo() nowadays if you can.
bool HanaLovesMe::Combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items)
{
    HanaLovesMeContext& g = *GHanaLovesMe;

    // Call the getter to obtain the preview string which is a parameter to BeginCombo()
    const char* preview_value = NULL;
    if (*current_item >= 0 && *current_item < items_count)
        items_getter(data, *current_item, &preview_value);

    // The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
    if (popup_max_height_in_items != -1 && !g.NextWindowData.SizeConstraintCond)
        SetNextWindowSizeConstraints(vsize(0, 0), vsize(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

    if (!BeginCombo(label, preview_value, HanaLovesMeComboFlags_None))
        return false;

    // Display items
    // FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
    bool value_changed = false;
    for (int i = 0; i < items_count; i++)
    {
        PushID((void*)(intptr_t)i);
        const bool item_selected = (i == *current_item);
        const char* item_text;
        if (!items_getter(data, i, &item_text))
            item_text = "*Unknown item*";
        if (Selectable(item_text, item_selected))
        {
            value_changed = true;
            *current_item = i;
        }
        if (item_selected)
            SetItemDefaultFocus();
        PopID();
    }

    EndCombo();
    return value_changed;
}

// Combo box helper allowing to pass an array of strings.
bool HanaLovesMe::Combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
{
    const bool value_changed = Combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
    return value_changed;
}

// Combo box helper allowing to pass all items in a single string literal holding multiple zero-terminated items "item1\0item2\0"
bool HanaLovesMe::Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items)
{
    int items_count = 0;
    const char* p = items_separated_by_zeros;       // FIXME-OPT: Avoid computing this, or at least only when combo is open
    while (*p)
    {
        p += strlen(p) + 1;
        items_count++;
    }
    bool value_changed = Combo(label, current_item, Items_SingleStringGetter, (void*)items_separated_by_zeros, items_count, height_in_items);
    return value_changed;
}

//-------------------------------------------------------------------------
// [SECTION] Data Type and Data Formatting Helpers [Internal]
//-------------------------------------------------------------------------
// - PatchFormatStringFloatToInt()
// - DataTypeFormatString()
// - DataTypeApplyOp()
// - DataTypeApplyOpFromText()
// - GetMinimumStepAtDecimalPrecision
// - RoundScalarWithFormat<>()
//-------------------------------------------------------------------------

struct HanaLovesMeDataTypeInfo
{
    size_t      Size;
    const char* PrintFmt;   // Unused
    const char* ScanFmt;
};

static const HanaLovesMeDataTypeInfo GDataTypeInfo[] =
{
    { sizeof(char),             "%d",   "%d"    },  // HanaLovesMeDataType_S8
    { sizeof(unsigned char),    "%u",   "%u"    },
    { sizeof(short),            "%d",   "%d"    },  // HanaLovesMeDataType_S16
    { sizeof(unsigned short),   "%u",   "%u"    },
    { sizeof(int),              "%d",   "%d"    },  // HanaLovesMeDataType_S32
    { sizeof(unsigned int),     "%u",   "%u"    },
#ifdef _MSC_VER
    { sizeof(ImS64),            "%I64d","%I64d" },  // HanaLovesMeDataType_S64
    { sizeof(ImU64),            "%I64u","%I64u" },
#else
    { sizeof(ImS64),            "%lld", "%lld"  },  // HanaLovesMeDataType_S64
    { sizeof(ImU64),            "%llu", "%llu"  },
#endif
    { sizeof(float),            "%f",   "%f"    },  // HanaLovesMeDataType_Float (float are promoted to double in va_arg)
    { sizeof(double),           "%f",   "%lf"   },  // HanaLovesMeDataType_Double
};
IM_STATIC_ASSERT(IM_ARRAYSIZE(GDataTypeInfo) == HanaLovesMeDataType_COUNT);

// FIXME-LEGACY: Prior to 1.61 our DragInt() function internally used floats and because of this the compile-time default value for format was "%.0f".
// Even though we changed the compile-time default, we expect users to have carried %f around, which would break the display of DragInt() calls.
// To honor backward compatibility we are rewriting the format string, unless HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS is enabled. What could possibly go wrong?!
static const char* PatchFormatStringFloatToInt(const char* fmt)
{
    if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0) // Fast legacy path for "%.0f" which is expected to be the most common case.
        return "%d";
    const char* fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
    const char* fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness (because snprintf is dependent on libc or user).
    if (fmt_end > fmt_start && fmt_end[-1] == 'f')
    {
#ifndef HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS
        if (fmt_start == fmt && fmt_end[0] == 0)
            return "%d";
        HanaLovesMeContext & g = *GHanaLovesMe;
        ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end); // Honor leading and trailing decorations, but lose alignment/precision.
        return g.TempBuffer;
#else
        IM_ASSERT(0 && "DragInt(): Invalid format string!"); // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
#endif
    }
    return fmt;
}

static inline int DataTypeFormatString(char* buf, int buf_size, HanaLovesMeDataType data_type, const void* data_ptr, const char* format)
{
    // Signedness doesn't matter when pushing integer arguments
    if (data_type == HanaLovesMeDataType_S32 || data_type == HanaLovesMeDataType_U32)
        return ImFormatString(buf, buf_size, format, *(const ImU32*)data_ptr);
    if (data_type == HanaLovesMeDataType_S64 || data_type == HanaLovesMeDataType_U64)
        return ImFormatString(buf, buf_size, format, *(const ImU64*)data_ptr);
    if (data_type == HanaLovesMeDataType_Float)
        return ImFormatString(buf, buf_size, format, *(const float*)data_ptr);
    if (data_type == HanaLovesMeDataType_Double)
        return ImFormatString(buf, buf_size, format, *(const double*)data_ptr);
    if (data_type == HanaLovesMeDataType_S8)
        return ImFormatString(buf, buf_size, format, *(const ImS8*)data_ptr);
    if (data_type == HanaLovesMeDataType_U8)
        return ImFormatString(buf, buf_size, format, *(const ImU8*)data_ptr);
    if (data_type == HanaLovesMeDataType_S16)
        return ImFormatString(buf, buf_size, format, *(const ImS16*)data_ptr);
    if (data_type == HanaLovesMeDataType_U16)
        return ImFormatString(buf, buf_size, format, *(const ImU16*)data_ptr);
    IM_ASSERT(0);
    return 0;
}

static void DataTypeApplyOp(HanaLovesMeDataType data_type, int op, void* output, void* arg1, const void* arg2)
{
    IM_ASSERT(op == '+' || op == '-');
    switch (data_type)
    {
    case HanaLovesMeDataType_S8:
        if (op == '+') { *(ImS8*)output = ImAddClampOverflow(*(const ImS8*)arg1, *(const ImS8*)arg2, IM_S8_MIN, IM_S8_MAX); }
        if (op == '-') { *(ImS8*)output = ImSubClampOverflow(*(const ImS8*)arg1, *(const ImS8*)arg2, IM_S8_MIN, IM_S8_MAX); }
        return;
    case HanaLovesMeDataType_U8:
        if (op == '+') { *(ImU8*)output = ImAddClampOverflow(*(const ImU8*)arg1, *(const ImU8*)arg2, IM_U8_MIN, IM_U8_MAX); }
        if (op == '-') { *(ImU8*)output = ImSubClampOverflow(*(const ImU8*)arg1, *(const ImU8*)arg2, IM_U8_MIN, IM_U8_MAX); }
        return;
    case HanaLovesMeDataType_S16:
        if (op == '+') { *(ImS16*)output = ImAddClampOverflow(*(const ImS16*)arg1, *(const ImS16*)arg2, IM_S16_MIN, IM_S16_MAX); }
        if (op == '-') { *(ImS16*)output = ImSubClampOverflow(*(const ImS16*)arg1, *(const ImS16*)arg2, IM_S16_MIN, IM_S16_MAX); }
        return;
    case HanaLovesMeDataType_U16:
        if (op == '+') { *(ImU16*)output = ImAddClampOverflow(*(const ImU16*)arg1, *(const ImU16*)arg2, IM_U16_MIN, IM_U16_MAX); }
        if (op == '-') { *(ImU16*)output = ImSubClampOverflow(*(const ImU16*)arg1, *(const ImU16*)arg2, IM_U16_MIN, IM_U16_MAX); }
        return;
    case HanaLovesMeDataType_S32:
        if (op == '+') { *(ImS32*)output = ImAddClampOverflow(*(const ImS32*)arg1, *(const ImS32*)arg2, IM_S32_MIN, IM_S32_MAX); }
        if (op == '-') { *(ImS32*)output = ImSubClampOverflow(*(const ImS32*)arg1, *(const ImS32*)arg2, IM_S32_MIN, IM_S32_MAX); }
        return;
    case HanaLovesMeDataType_U32:
        if (op == '+') { *(ImU32*)output = ImAddClampOverflow(*(const ImU32*)arg1, *(const ImU32*)arg2, IM_U32_MIN, IM_U32_MAX); }
        if (op == '-') { *(ImU32*)output = ImSubClampOverflow(*(const ImU32*)arg1, *(const ImU32*)arg2, IM_U32_MIN, IM_U32_MAX); }
        return;
    case HanaLovesMeDataType_S64:
        if (op == '+') { *(ImS64*)output = ImAddClampOverflow(*(const ImS64*)arg1, *(const ImS64*)arg2, IM_S64_MIN, IM_S64_MAX); }
        if (op == '-') { *(ImS64*)output = ImSubClampOverflow(*(const ImS64*)arg1, *(const ImS64*)arg2, IM_S64_MIN, IM_S64_MAX); }
        return;
    case HanaLovesMeDataType_U64:
        if (op == '+') { *(ImU64*)output = ImAddClampOverflow(*(const ImU64*)arg1, *(const ImU64*)arg2, IM_U64_MIN, IM_U64_MAX); }
        if (op == '-') { *(ImU64*)output = ImSubClampOverflow(*(const ImU64*)arg1, *(const ImU64*)arg2, IM_U64_MIN, IM_U64_MAX); }
        return;
    case HanaLovesMeDataType_Float:
        if (op == '+') { *(float*)output = *(const float*)arg1 + *(const float*)arg2; }
        if (op == '-') { *(float*)output = *(const float*)arg1 - *(const float*)arg2; }
        return;
    case HanaLovesMeDataType_Double:
        if (op == '+') { *(double*)output = *(const double*)arg1 + *(const double*)arg2; }
        if (op == '-') { *(double*)output = *(const double*)arg1 - *(const double*)arg2; }
        return;
    case HanaLovesMeDataType_COUNT: break;
    }
    IM_ASSERT(0);
}

// User can input math operators (e.g. +100) to edit a numerical values.
// NB: This is _not_ a full expression evaluator. We should probably add one and replace this dumb mess..
static bool DataTypeApplyOpFromText(const char* buf, const char* initial_value_buf, HanaLovesMeDataType data_type, void* data_ptr, const char* format)
{
    while (ImCharIsBlankA(*buf))
        buf++;

    // We don't support '-' op because it would conflict with inputing negative value.
    // Instead you can use +-100 to subtract from an existing value
    char op = buf[0];
    if (op == '+' || op == '*' || op == '/')
    {
        buf++;
        while (ImCharIsBlankA(*buf))
            buf++;
    } else
    {
        op = 0;
    }
    if (!buf[0])
        return false;

    // Copy the value in an opaque buffer so we can compare at the end of the function if it changed at all.
    IM_ASSERT(data_type < HanaLovesMeDataType_COUNT);
    int data_backup[2];
    IM_ASSERT(GDataTypeInfo[data_type].Size <= sizeof(data_backup));
    memcpy(data_backup, data_ptr, GDataTypeInfo[data_type].Size);

    if (format == NULL)
        format = GDataTypeInfo[data_type].ScanFmt;

    // FIXME-LEGACY: The aim is to remove those operators and write a proper expression evaluator at some point..
    int arg1i = 0;
    if (data_type == HanaLovesMeDataType_S32)
    {
        int* v = (int*)data_ptr;
        int arg0i = *v;
        float arg1f = 0.0f;
        if (op && sscanf(initial_value_buf, format, &arg0i) < 1)
            return false;
        // Store operand in a float so we can use fractional value for multipliers (*1.1), but constant always parsed as integer so we can fit big integers (e.g. 2000000003) past float precision
        if (op == '+') { if (sscanf(buf, "%d", &arg1i))* v = (int)(arg0i + arg1i); }                   // Add (use "+-" to subtract)
        else if (op == '*') { if (sscanf(buf, "%f", &arg1f))* v = (int)(arg0i * arg1f); }                   // Multiply
        else if (op == '/') { if (sscanf(buf, "%f", &arg1f) && arg1f != 0.0f) * v = (int)(arg0i / arg1f); }  // Divide
        else { if (sscanf(buf, format, &arg1i) == 1) * v = arg1i; }                           // Assign constant
    } else if (data_type == HanaLovesMeDataType_Float)
    {
        // For floats we have to ignore format with precision (e.g. "%.2f") because sscanf doesn't take them in
        format = "%f";
        float* v = (float*)data_ptr;
        float arg0f = *v, arg1f = 0.0f;
        if (op && sscanf(initial_value_buf, format, &arg0f) < 1)
            return false;
        if (sscanf(buf, format, &arg1f) < 1)
            return false;
        if (op == '+') { *v = arg0f + arg1f; }                    // Add (use "+-" to subtract)
        else if (op == '*') { *v = arg0f * arg1f; }                    // Multiply
        else if (op == '/') { if (arg1f != 0.0f) * v = arg0f / arg1f; } // Divide
        else { *v = arg1f; }                            // Assign constant
    } else if (data_type == HanaLovesMeDataType_Double)
    {
        format = "%lf"; // scanf differentiate float/double unlike printf which forces everything to double because of ellipsis
        double* v = (double*)data_ptr;
        double arg0f = *v, arg1f = 0.0;
        if (op && sscanf(initial_value_buf, format, &arg0f) < 1)
            return false;
        if (sscanf(buf, format, &arg1f) < 1)
            return false;
        if (op == '+') { *v = arg0f + arg1f; }                    // Add (use "+-" to subtract)
        else if (op == '*') { *v = arg0f * arg1f; }                    // Multiply
        else if (op == '/') { if (arg1f != 0.0f) * v = arg0f / arg1f; } // Divide
        else { *v = arg1f; }                            // Assign constant
    } else if (data_type == HanaLovesMeDataType_U32 || data_type == HanaLovesMeDataType_S64 || data_type == HanaLovesMeDataType_U64)
    {
        // All other types assign constant
        // We don't bother handling support for legacy operators since they are a little too crappy. Instead we will later implement a proper expression evaluator in the future.
        sscanf(buf, format, data_ptr);
    } else
    {
        // Small types need a 32-bit buffer to receive the result from scanf()
        int v32;
        sscanf(buf, format, &v32);
        if (data_type == HanaLovesMeDataType_S8)
            * (ImS8*)data_ptr = (ImS8)ImClamp(v32, (int)IM_S8_MIN, (int)IM_S8_MAX);
        else if (data_type == HanaLovesMeDataType_U8)
            * (ImU8*)data_ptr = (ImU8)ImClamp(v32, (int)IM_U8_MIN, (int)IM_U8_MAX);
        else if (data_type == HanaLovesMeDataType_S16)
            * (ImS16*)data_ptr = (ImS16)ImClamp(v32, (int)IM_S16_MIN, (int)IM_S16_MAX);
        else if (data_type == HanaLovesMeDataType_U16)
            * (ImU16*)data_ptr = (ImU16)ImClamp(v32, (int)IM_U16_MIN, (int)IM_U16_MAX);
        else
            IM_ASSERT(0);
    }

    return memcmp(data_backup, data_ptr, GDataTypeInfo[data_type].Size) != 0;
}

static float GetMinimumStepAtDecimalPrecision(int decimal_precision)
{
    static const float min_steps[10] = { 1.0f, 0.1f, 0.01f, 0.001f, 0.0001f, 0.00001f, 0.000001f, 0.0000001f, 0.00000001f, 0.000000001f };
    if (decimal_precision < 0)
        return FLT_MIN;
    return (decimal_precision < IM_ARRAYSIZE(min_steps)) ? min_steps[decimal_precision] : ImPow(10.0f, (float)-decimal_precision);
}

template<typename TYPE>
static const char* ImAtoi(const char* src, TYPE * output)
{
    int negative = 0;
    if (*src == '-') { negative = 1; src++; }
    if (*src == '+') { src++; }
    TYPE v = 0;
    while (*src >= '0' && *src <= '9')
        v = (v * 10) + (*src++ - '0');
    *output = negative ? -v : v;
    return src;
}

template<typename TYPE, typename SIGNEDTYPE>
TYPE HanaLovesMe::RoundScalarWithFormatT(const char* format, HanaLovesMeDataType data_type, TYPE v)
{
    const char* fmt_start = ImParseFormatFindStart(format);
    if (fmt_start[0] != '%' || fmt_start[1] == '%') // Don't apply if the value is not visible in the format string
        return v;
    char v_str[64];
    ImFormatString(v_str, IM_ARRAYSIZE(v_str), fmt_start, v);
    const char* p = v_str;
    while (*p == ' ')
        p++;
    if (data_type == HanaLovesMeDataType_Float || data_type == HanaLovesMeDataType_Double)
        v = (TYPE)ImAtof(p);
    else
        ImAtoi(p, (SIGNEDTYPE*)& v);
    return v;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: DragScalar, DragFloat, DragInt, etc.
//-------------------------------------------------------------------------
// - DragBehaviorT<>() [Internal]
// - DragBehavior() [Internal]
// - DragScalar()
// - DragScalarN()
// - DragFloat()
// - DragFloat2()
// - DragFloat3()
// - DragFloat4()
// - DragFloatRange2()
// - DragInt()
// - DragInt2()
// - DragInt3()
// - DragInt4()
// - DragIntRange2()
//-------------------------------------------------------------------------

// This is called by DragBehavior() when the widget is active (held by mouse or being manipulated with Nav controls)
template<typename TYPE, typename SIGNEDTYPE, typename FLOATTYPE>
bool HanaLovesMe::DragBehaviorT(HanaLovesMeDataType data_type, TYPE * v, float v_speed, const TYPE v_min, const TYPE v_max, const char* format, float power, HanaLovesMeDragFlags flags)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    const HanaLovesMeAxis axis = (flags & HanaLovesMeDragFlags_Vertical) ? HanaLovesMeAxis_Y : HanaLovesMeAxis_X;
    const bool is_decimal = (data_type == HanaLovesMeDataType_Float) || (data_type == HanaLovesMeDataType_Double);
    const bool has_min_max = (v_min != v_max);
    const bool is_power = (power != 1.0f && is_decimal && has_min_max && (v_max - v_min < FLT_MAX));

    // Default tweak speed
    if (v_speed == 0.0f && has_min_max && (v_max - v_min < FLT_MAX))
        v_speed = (float)((v_max - v_min) * g.DragSpeedDefaultRatio);

    // Inputs accumulates into g.DragCurrentAccum, which is flushed into the current value as soon as it makes a difference with our precision settings
    float adjust_delta = 0.0f;
    if (g.ActiveIdSource == HanaLovesMeInputSource_Mouse && IsMousePosValid() && g.IO.MouseDragMaxDistanceSqr[0] > 1.0f * 1.0f)
    {
        adjust_delta = g.IO.MouseDelta[axis];
        if (g.IO.KeyAlt)
            adjust_delta *= 1.0f / 100.0f;
        if (g.IO.KeyShift)
            adjust_delta *= 10.0f;
    } else if (g.ActiveIdSource == HanaLovesMeInputSource_Nav)
    {
        int decimal_precision = is_decimal ? ImParseFormatPrecision(format, 3) : 0;
        adjust_delta = GetNavInputAmount2d(HanaLovesMeNavDirSourceFlags_Keyboard | HanaLovesMeNavDirSourceFlags_PadDPad, HanaLovesMeInputReadMode_RepeatFast, 1.0f / 10.0f, 10.0f)[axis];
        v_speed = ImMax(v_speed, GetMinimumStepAtDecimalPrecision(decimal_precision));
    }
    adjust_delta *= v_speed;

    // For vertical drag we currently assume that Up=higher value (like we do with vertical sliders). This may become a parameter.
    if (axis == HanaLovesMeAxis_Y)
        adjust_delta = -adjust_delta;

    // Clear current value on activation
    // Avoid altering values and clamping when we are _already_ past the limits and heading in the same direction, so e.g. if range is 0..255, current value is 300 and we are pushing to the right side, keep the 300.
    bool is_just_activated = g.ActiveIdIsJustActivated;
    bool is_already_past_limits_and_pushing_outward = has_min_max && ((*v >= v_max && adjust_delta > 0.0f) || (*v <= v_min && adjust_delta < 0.0f));
    bool is_drag_direction_change_with_power = is_power && ((adjust_delta < 0 && g.DragCurrentAccum > 0) || (adjust_delta > 0 && g.DragCurrentAccum < 0));
    if (is_just_activated || is_already_past_limits_and_pushing_outward || is_drag_direction_change_with_power)
    {
        g.DragCurrentAccum = 0.0f;
        g.DragCurrentAccumDirty = false;
    } else if (adjust_delta != 0.0f)
    {
        g.DragCurrentAccum += adjust_delta;
        g.DragCurrentAccumDirty = true;
    }

    if (!g.DragCurrentAccumDirty)
        return false;

    TYPE v_cur = *v;
    FLOATTYPE v_old_ref_for_accum_remainder = (FLOATTYPE)0.0f;

    if (is_power)
    {
        // Offset + round to user desired precision, with a curve on the v_min..v_max range to get more precision on one side of the range
        FLOATTYPE v_old_norm_curved = ImPow((FLOATTYPE)(v_cur - v_min) / (FLOATTYPE)(v_max - v_min), (FLOATTYPE)1.0f / power);
        FLOATTYPE v_new_norm_curved = v_old_norm_curved + (g.DragCurrentAccum / (v_max - v_min));
        v_cur = v_min + (TYPE)ImPow(ImSaturate((float)v_new_norm_curved), power) * (v_max - v_min);
        v_old_ref_for_accum_remainder = v_old_norm_curved;
    } else
    {
        v_cur += (TYPE)g.DragCurrentAccum;
    }

    // Round to user desired precision based on format string
    v_cur = RoundScalarWithFormatT<TYPE, SIGNEDTYPE>(format, data_type, v_cur);

    // Preserve remainder after rounding has been applied. This also allow slow tweaking of values.
    g.DragCurrentAccumDirty = false;
    if (is_power)
    {
        FLOATTYPE v_cur_norm_curved = ImPow((FLOATTYPE)(v_cur - v_min) / (FLOATTYPE)(v_max - v_min), (FLOATTYPE)1.0f / power);
        g.DragCurrentAccum -= (float)(v_cur_norm_curved - v_old_ref_for_accum_remainder);
    } else
    {
        g.DragCurrentAccum -= (float)((SIGNEDTYPE)v_cur - (SIGNEDTYPE)* v);
    }

    // Lose zero sign for float/double
    if (v_cur == (TYPE)-0)
        v_cur = (TYPE)0;

    // Clamp values (+ handle overflow/wrap-around for integer types)
    if (*v != v_cur && has_min_max)
    {
        if (v_cur < v_min || (v_cur > * v && adjust_delta < 0.0f && !is_decimal))
            v_cur = v_min;
        if (v_cur > v_max || (v_cur < *v && adjust_delta > 0.0f && !is_decimal))
            v_cur = v_max;
    }

    // Apply result
    if (*v == v_cur)
        return false;
    *v = v_cur;
    return true;
}

bool HanaLovesMe::DragBehavior(HanaLovesMeID id, HanaLovesMeDataType data_type, void* v, float v_speed, const void* v_min, const void* v_max, const char* format, float power, HanaLovesMeDragFlags flags)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    if (g.ActiveId == id)
    {
        if (g.ActiveIdSource == HanaLovesMeInputSource_Mouse && !g.IO.MouseDown[0])
            ClearActiveID();
        else if (g.ActiveIdSource == HanaLovesMeInputSource_Nav && g.NavActivatePressedId == id && !g.ActiveIdIsJustActivated)
            ClearActiveID();
    }
    if (g.ActiveId != id)
        return false;

    switch (data_type)
    {
    case HanaLovesMeDataType_S8: { ImS32 v32 = (ImS32) * (ImS8*)v;  bool r = DragBehaviorT<ImS32, ImS32, float >(HanaLovesMeDataType_S32, &v32, v_speed, v_min ? *(const ImS8*)v_min : IM_S8_MIN, v_max ? *(const ImS8*)v_max : IM_S8_MAX, format, power, flags); if (r)* (ImS8*)v = (ImS8)v32; return r; }
    case HanaLovesMeDataType_U8: { ImU32 v32 = (ImU32) * (ImU8*)v;  bool r = DragBehaviorT<ImU32, ImS32, float >(HanaLovesMeDataType_U32, &v32, v_speed, v_min ? *(const ImU8*)v_min : IM_U8_MIN, v_max ? *(const ImU8*)v_max : IM_U8_MAX, format, power, flags); if (r)* (ImU8*)v = (ImU8)v32; return r; }
    case HanaLovesMeDataType_S16: { ImS32 v32 = (ImS32) * (ImS16*)v; bool r = DragBehaviorT<ImS32, ImS32, float >(HanaLovesMeDataType_S32, &v32, v_speed, v_min ? *(const ImS16*)v_min : IM_S16_MIN, v_max ? *(const ImS16*)v_max : IM_S16_MAX, format, power, flags); if (r)* (ImS16*)v = (ImS16)v32; return r; }
    case HanaLovesMeDataType_U16: { ImU32 v32 = (ImU32) * (ImU16*)v; bool r = DragBehaviorT<ImU32, ImS32, float >(HanaLovesMeDataType_U32, &v32, v_speed, v_min ? *(const ImU16*)v_min : IM_U16_MIN, v_max ? *(const ImU16*)v_max : IM_U16_MAX, format, power, flags); if (r)* (ImU16*)v = (ImU16)v32; return r; }
    case HanaLovesMeDataType_S32:    return DragBehaviorT<ImS32, ImS32, float >(data_type, (ImS32*)v, v_speed, v_min ? *(const ImS32*)v_min : IM_S32_MIN, v_max ? *(const ImS32*)v_max : IM_S32_MAX, format, power, flags);
    case HanaLovesMeDataType_U32:    return DragBehaviorT<ImU32, ImS32, float >(data_type, (ImU32*)v, v_speed, v_min ? *(const ImU32*)v_min : IM_U32_MIN, v_max ? *(const ImU32*)v_max : IM_U32_MAX, format, power, flags);
    case HanaLovesMeDataType_S64:    return DragBehaviorT<ImS64, ImS64, double>(data_type, (ImS64*)v, v_speed, v_min ? *(const ImS64*)v_min : IM_S64_MIN, v_max ? *(const ImS64*)v_max : IM_S64_MAX, format, power, flags);
    case HanaLovesMeDataType_U64:    return DragBehaviorT<ImU64, ImS64, double>(data_type, (ImU64*)v, v_speed, v_min ? *(const ImU64*)v_min : IM_U64_MIN, v_max ? *(const ImU64*)v_max : IM_U64_MAX, format, power, flags);
    case HanaLovesMeDataType_Float:  return DragBehaviorT<float, float, float >(data_type, (float*)v, v_speed, v_min ? *(const float*)v_min : -FLT_MAX, v_max ? *(const float*)v_max : FLT_MAX, format, power, flags);
    case HanaLovesMeDataType_Double: return DragBehaviorT<double, double, double>(data_type, (double*)v, v_speed, v_min ? *(const double*)v_min : -DBL_MAX, v_max ? *(const double*)v_max : DBL_MAX, format, power, flags);
    case HanaLovesMeDataType_COUNT:  break;
    }
    IM_ASSERT(0);
    return false;
}

bool HanaLovesMe::DragScalar(const char* label, HanaLovesMeDataType data_type, void* v, float v_speed, const void* v_min, const void* v_max, const char* format, float power)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    if (power != 1.0f)
        IM_ASSERT(v_min != NULL && v_max != NULL); // When using a power curve the drag needs to have known bounds

    HanaLovesMeContext & g = *GHanaLovesMe;
    const HanaLovesMeStyle & style = g.Style;
    const HanaLovesMeID id = window->GetID(label);
    const float w = CalcItemWidth();

    const vsize label_size = CalcTextSize(label, NULL, true);
    const vsrect frame_bb(window->DC.CursorPos, window->DC.CursorPos + vsize(w, label_size.y + style.FramePadding.y * 2.0f));
    const vsrect total_bb(frame_bb.Min, frame_bb.Max + vsize(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id, &frame_bb))
        return false;

    const bool hovered = ItemHoverable(frame_bb, id);

    // Default format string when passing NULL
    // Patch old "%.0f" format string to use "%d", read function comments for more details.
    IM_ASSERT(data_type >= 0 && data_type < HanaLovesMeDataType_COUNT);
    if (format == NULL)
        format = GDataTypeInfo[data_type].PrintFmt;
    else if (data_type == HanaLovesMeDataType_S32 && strcmp(format, "%d") != 0)
        format = PatchFormatStringFloatToInt(format);

    // Tabbing or CTRL-clicking on Drag turns it into an input box
    bool start_text_input = false;
    const bool focus_requested = FocusableItemRegister(window, id);
    if (focus_requested || (hovered && (g.IO.MouseClicked[0] || g.IO.MouseDoubleClicked[0])) || g.NavActivateId == id || (g.NavInputId == id && g.ScalarAsInputTextId != id))
    {
        SetActiveID(id, window);
        SetFocusID(id, window);
        FocusWindow(window);
        g.ActiveIdAllowNavDirFlags = (1 << HanaLovesMeDir_Up) | (1 << HanaLovesMeDir_Down);
        if (focus_requested || g.IO.KeyCtrl || g.IO.MouseDoubleClicked[0] || g.NavInputId == id)
        {
            start_text_input = true;
            g.ScalarAsInputTextId = 0;
        }
    }
    if (start_text_input || (g.ActiveId == id && g.ScalarAsInputTextId == id))
    {
        window->DC.CursorPos = frame_bb.Min;
        FocusableItemUnregister(window);
        return InputScalarAsWidgetReplacement(frame_bb, id, label, data_type, v, format);
    }

    // Actual drag behavior
    const bool value_changed = DragBehavior(id, data_type, v, v_speed, v_min, v_max, format, power, HanaLovesMeDragFlags_None);
    if (value_changed)
        MarkItemEdited(id);

    // Draw frame
    const ImU32 frame_col = GetColorU32(g.ActiveId == id ? HanaLovesMeCol_FrameBgActive : g.HoveredId == id ? HanaLovesMeCol_FrameBgHovered : HanaLovesMeCol_FrameBg);
    RenderNavHighlight(frame_bb, id);
    RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding);

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    char value_buf[64];
    const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, v, format);
    RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, vsize(0.5f, 0.5f));

    if (label_size.x > 0.0f)
        RenderText(vsize(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    HanaLovesMe_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
    return value_changed;
}

bool HanaLovesMe::DragScalarN(const char* label, HanaLovesMeDataType data_type, void* v, int components, float v_speed, const void* v_min, const void* v_max, const char* format, float power)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    HanaLovesMeContext& g = *GHanaLovesMe;
    bool value_changed = false;
    BeginGroup();
    PushID(label);
    PushMultiItemsWidths(components);
    size_t type_size = GDataTypeInfo[data_type].Size;
    for (int i = 0; i < components; i++)
    {
        PushID(i);
        value_changed |= DragScalar("", data_type, v, v_speed, v_min, v_max, format, power);
        SameLine(0, g.Style.ItemInnerSpacing.x);
        PopID();
        PopItemWidth();
        v = (void*)((char*)v + type_size);
    }
    PopID();

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    return value_changed;
}

bool HanaLovesMe::DragFloat(const char* label, float* v, float v_speed, float v_min, float v_max, const char* format, float power)
{
    return DragScalar(label, HanaLovesMeDataType_Float, v, v_speed, &v_min, &v_max, format, power);
}

bool HanaLovesMe::DragFloat2(const char* label, float v[2], float v_speed, float v_min, float v_max, const char* format, float power)
{
    return DragScalarN(label, HanaLovesMeDataType_Float, v, 2, v_speed, &v_min, &v_max, format, power);
}

bool HanaLovesMe::DragFloat3(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* format, float power)
{
    return DragScalarN(label, HanaLovesMeDataType_Float, v, 3, v_speed, &v_min, &v_max, format, power);
}

bool HanaLovesMe::DragFloat4(const char* label, float v[4], float v_speed, float v_min, float v_max, const char* format, float power)
{
    return DragScalarN(label, HanaLovesMeDataType_Float, v, 4, v_speed, &v_min, &v_max, format, power);
}

bool HanaLovesMe::DragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed, float v_min, float v_max, const char* format, const char* format_max, float power)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    HanaLovesMeContext& g = *GHanaLovesMe;
    PushID(label);
    BeginGroup();
    PushMultiItemsWidths(2);

    bool value_changed = DragFloat("##min", v_current_min, v_speed, (v_min >= v_max) ? -FLT_MAX : v_min, (v_min >= v_max) ? *v_current_max : ImMin(v_max, *v_current_max), format, power);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);
    value_changed |= DragFloat("##max", v_current_max, v_speed, (v_min >= v_max) ? *v_current_min : ImMax(v_min, *v_current_min), (v_min >= v_max) ? FLT_MAX : v_max, format_max ? format_max : format, power);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    PopID();
    return value_changed;
}

// NB: v_speed is float to allow adjusting the drag speed with more precision
bool HanaLovesMe::DragInt(const char* label, int* v, float v_speed, int v_min, int v_max, const char* format)
{
    return DragScalar(label, HanaLovesMeDataType_S32, v, v_speed, &v_min, &v_max, format);
}

bool HanaLovesMe::DragInt2(const char* label, int v[2], float v_speed, int v_min, int v_max, const char* format)
{
    return DragScalarN(label, HanaLovesMeDataType_S32, v, 2, v_speed, &v_min, &v_max, format);
}

bool HanaLovesMe::DragInt3(const char* label, int v[3], float v_speed, int v_min, int v_max, const char* format)
{
    return DragScalarN(label, HanaLovesMeDataType_S32, v, 3, v_speed, &v_min, &v_max, format);
}

bool HanaLovesMe::DragInt4(const char* label, int v[4], float v_speed, int v_min, int v_max, const char* format)
{
    return DragScalarN(label, HanaLovesMeDataType_S32, v, 4, v_speed, &v_min, &v_max, format);
}

bool HanaLovesMe::DragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed, int v_min, int v_max, const char* format, const char* format_max)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    HanaLovesMeContext& g = *GHanaLovesMe;
    PushID(label);
    BeginGroup();
    PushMultiItemsWidths(2);

    bool value_changed = DragInt("##min", v_current_min, v_speed, (v_min >= v_max) ? INT_MIN : v_min, (v_min >= v_max) ? *v_current_max : ImMin(v_max, *v_current_max), format);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);
    value_changed |= DragInt("##max", v_current_max, v_speed, (v_min >= v_max) ? *v_current_min : ImMax(v_min, *v_current_min), (v_min >= v_max) ? INT_MAX : v_max, format_max ? format_max : format);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    PopID();

    return value_changed;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: SliderScalar, SliderFloat, SliderInt, etc.
//-------------------------------------------------------------------------
// - SliderBehaviorT<>() [Internal]
// - SliderBehavior() [Internal]
// - SliderScalar()
// - SliderScalarN()
// - SliderFloat()
// - SliderFloat2()
// - SliderFloat3()
// - SliderFloat4()
// - SliderAngle()
// - SliderInt()
// - SliderInt2()
// - SliderInt3()
// - SliderInt4()
// - VSliderScalar()
// - VSliderFloat()
// - VSliderInt()
//-------------------------------------------------------------------------

template<typename TYPE, typename FLOATTYPE>
float HanaLovesMe::SliderCalcRatioFromValueT(HanaLovesMeDataType data_type, TYPE v, TYPE v_min, TYPE v_max, float power, float linear_zero_pos)
{
    if (v_min == v_max)
        return 0.0f;

    const bool is_power = (power != 1.0f) && (data_type == HanaLovesMeDataType_Float || data_type == HanaLovesMeDataType_Double);
    const TYPE v_clamped = (v_min < v_max) ? ImClamp(v, v_min, v_max) : ImClamp(v, v_max, v_min);
    if (is_power)
    {
        if (v_clamped < 0.0f)
        {
            const float f = 1.0f - (float)((v_clamped - v_min) / (ImMin((TYPE)0, v_max) - v_min));
            return (1.0f - ImPow(f, 1.0f / power)) * linear_zero_pos;
        } else
        {
            const float f = (float)((v_clamped - ImMax((TYPE)0, v_min)) / (v_max - ImMax((TYPE)0, v_min)));
            return linear_zero_pos + ImPow(f, 1.0f / power) * (1.0f - linear_zero_pos);
        }
    }

    // Linear slider
    return (float)((FLOATTYPE)(v_clamped - v_min) / (FLOATTYPE)(v_max - v_min));
}

// FIXME: Move some of the code into SliderBehavior(). Current responsability is larger than what the equivalent DragBehaviorT<> does, we also do some rendering, etc.
template<typename TYPE, typename SIGNEDTYPE, typename FLOATTYPE>
bool HanaLovesMe::SliderBehaviorT(const vsrect & bb, HanaLovesMeID id, HanaLovesMeDataType data_type, TYPE * v, const TYPE v_min, const TYPE v_max, const char* format, float power, HanaLovesMeSliderFlags flags, vsrect * out_grab_bb)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    const HanaLovesMeStyle& style = g.Style;

    const HanaLovesMeAxis axis = (flags & HanaLovesMeSliderFlags_Vertical) ? HanaLovesMeAxis_Y : HanaLovesMeAxis_X;
    const bool is_decimal = (data_type == HanaLovesMeDataType_Float) || (data_type == HanaLovesMeDataType_Double);
    const bool is_power = (power != 1.0f) && is_decimal;

    const float grab_padding = 2.0f;
    const float slider_sz = (bb.Max[axis] - bb.Min[axis]) - grab_padding * 2.0f;
    float grab_sz = style.GrabMinSize;
    SIGNEDTYPE v_range = (v_min < v_max ? v_max - v_min : v_min - v_max);
    if (!is_decimal && v_range >= 0)                                             // v_range < 0 may happen on integer overflows
        grab_sz = ImMax((float)(slider_sz / (v_range + 1)), style.GrabMinSize);  // For integer sliders: if possible have the grab size represent 1 unit
    grab_sz = ImMin(grab_sz, slider_sz);
    const float slider_usable_sz = slider_sz - grab_sz;
    const float slider_usable_pos_min = bb.Min[axis] + grab_padding + grab_sz * 0.5f;
    const float slider_usable_pos_max = bb.Max[axis] - grab_padding - grab_sz * 0.5f;

    // For power curve sliders that cross over sign boundary we want the curve to be symmetric around 0.0f
    float linear_zero_pos;   // 0.0->1.0f
    if (is_power && v_min * v_max < 0.0f)
    {
        // Different sign
        const FLOATTYPE linear_dist_min_to_0 = ImPow(v_min >= 0 ? (FLOATTYPE)v_min : -(FLOATTYPE)v_min, (FLOATTYPE)1.0f / power);
        const FLOATTYPE linear_dist_max_to_0 = ImPow(v_max >= 0 ? (FLOATTYPE)v_max : -(FLOATTYPE)v_max, (FLOATTYPE)1.0f / power);
        linear_zero_pos = (float)(linear_dist_min_to_0 / (linear_dist_min_to_0 + linear_dist_max_to_0));
    } else
    {
        // Same sign
        linear_zero_pos = v_min < 0.0f ? 1.0f : 0.0f;
    }

    // Process interacting with the slider
    bool value_changed = false;
    if (g.ActiveId == id)
    {
        bool set_new_value = false;
        float clicked_t = 0.0f;
        if (g.ActiveIdSource == HanaLovesMeInputSource_Mouse)
        {
            if (!g.IO.MouseDown[0])
            {
                ClearActiveID();
            } else
            {
                const float mouse_abs_pos = g.IO.MousePos[axis];
                clicked_t = (slider_usable_sz > 0.0f) ? ImClamp((mouse_abs_pos - slider_usable_pos_min) / slider_usable_sz, 0.0f, 1.0f) : 0.0f;
                if (axis == HanaLovesMeAxis_Y)
                    clicked_t = 1.0f - clicked_t;
                set_new_value = true;
            }
        } else if (g.ActiveIdSource == HanaLovesMeInputSource_Nav)
        {
            const vsize delta2 = GetNavInputAmount2d(HanaLovesMeNavDirSourceFlags_Keyboard | HanaLovesMeNavDirSourceFlags_PadDPad, HanaLovesMeInputReadMode_RepeatFast, 0.0f, 0.0f);
            float delta = (axis == HanaLovesMeAxis_X) ? delta2.x : -delta2.y;
            if (g.NavActivatePressedId == id && !g.ActiveIdIsJustActivated)
            {
                ClearActiveID();
            } else if (delta != 0.0f)
            {
                clicked_t = SliderCalcRatioFromValueT<TYPE, FLOATTYPE>(data_type, *v, v_min, v_max, power, linear_zero_pos);
                const int decimal_precision = is_decimal ? ImParseFormatPrecision(format, 3) : 0;
                if ((decimal_precision > 0) || is_power)
                {
                    delta /= 100.0f;    // Gamepad/keyboard tweak speeds in % of slider bounds
                    if (IsNavInputDown(HanaLovesMeNavInput_TweakSlow))
                        delta /= 10.0f;
                } else
                {
                    if ((v_range >= -100.0f && v_range <= 100.0f) || IsNavInputDown(HanaLovesMeNavInput_TweakSlow))
                        delta = ((delta < 0.0f) ? -1.0f : +1.0f) / (float)v_range; // Gamepad/keyboard tweak speeds in integer steps
                    else
                        delta /= 100.0f;
                }
                if (IsNavInputDown(HanaLovesMeNavInput_TweakFast))
                    delta *= 10.0f;
                set_new_value = true;
                if ((clicked_t >= 1.0f && delta > 0.0f) || (clicked_t <= 0.0f && delta < 0.0f)) // This is to avoid applying the saturation when already past the limits
                    set_new_value = false;
                else
                    clicked_t = ImSaturate(clicked_t + delta);
            }
        }

        if (set_new_value)
        {
            TYPE v_new;
            if (is_power)
            {
                // Account for power curve scale on both sides of the zero
                if (clicked_t < linear_zero_pos)
                {
                    // Negative: rescale to the negative range before powering
                    float a = 1.0f - (clicked_t / linear_zero_pos);
                    a = ImPow(a, power);
                    v_new = ImLerp(ImMin(v_max, (TYPE)0), v_min, a);
                } else
                {
                    // Positive: rescale to the positive range before powering
                    float a;
                    if (ImFabs(linear_zero_pos - 1.0f) > 1.e-6f)
                        a = (clicked_t - linear_zero_pos) / (1.0f - linear_zero_pos);
                    else
                        a = clicked_t;
                    a = ImPow(a, power);
                    v_new = ImLerp(ImMax(v_min, (TYPE)0), v_max, a);
                }
            } else
            {
                // Linear slider
                if (is_decimal)
                {
                    v_new = ImLerp(v_min, v_max, clicked_t);
                } else
                {
                    // For integer values we want the clicking position to match the grab box so we round above
                    // This code is carefully tuned to work with large values (e.g. high ranges of U64) while preserving this property..
                    FLOATTYPE v_new_off_f = (v_max - v_min) * clicked_t;
                    TYPE v_new_off_floor = (TYPE)(v_new_off_f);
                    TYPE v_new_off_round = (TYPE)(v_new_off_f + (FLOATTYPE)0.5);
                    if (!is_decimal && v_new_off_floor < v_new_off_round)
                        v_new = v_min + v_new_off_round;
                    else
                        v_new = v_min + v_new_off_floor;
                }
            }

            // Round to user desired precision based on format string
            v_new = RoundScalarWithFormatT<TYPE, SIGNEDTYPE>(format, data_type, v_new);

            // Apply result
            if (*v != v_new)
            {
                *v = v_new;
                value_changed = true;
            }
        }
    }

    if (slider_sz < 1.0f)
    {
        *out_grab_bb = vsrect(bb.Min, bb.Min);
    } else
    {
        // Output grab position so it can be displayed by the caller
        float grab_t = SliderCalcRatioFromValueT<TYPE, FLOATTYPE>(data_type, *v, v_min, v_max, power, linear_zero_pos);
        if (axis == HanaLovesMeAxis_Y)
            grab_t = 1.0f - grab_t;
        const float grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
        if (axis == HanaLovesMeAxis_X)
            * out_grab_bb = vsrect(grab_pos - grab_sz * 0.5f, bb.Min.y + grab_padding, grab_pos + grab_sz * 0.5f, bb.Max.y - grab_padding);
        else
            *out_grab_bb = vsrect(bb.Min.x + grab_padding, grab_pos - grab_sz * 0.5f, bb.Max.x - grab_padding, grab_pos + grab_sz * 0.5f);
    }

    return value_changed;
}

// For 32-bits and larger types, slider bounds are limited to half the natural type range.
// So e.g. an integer Slider between INT_MAX-10 and INT_MAX will fail, but an integer Slider between INT_MAX/2-10 and INT_MAX/2 will be ok.
// It would be possible to lift that limitation with some work but it doesn't seem to be worth it for sliders.
bool HanaLovesMe::SliderBehavior(const vsrect & bb, HanaLovesMeID id, HanaLovesMeDataType data_type, void* v, const void* v_min, const void* v_max, const char* format, float power, HanaLovesMeSliderFlags flags, vsrect * out_grab_bb)
{
    switch (data_type)
    {
    case HanaLovesMeDataType_S8: { ImS32 v32 = (ImS32) * (ImS8*)v;  bool r = SliderBehaviorT<ImS32, ImS32, float >(bb, id, HanaLovesMeDataType_S32, &v32, *(const ImS8*)v_min, *(const ImS8*)v_max, format, power, flags, out_grab_bb); if (r)* (ImS8*)v = (ImS8)v32;  return r; }
    case HanaLovesMeDataType_U8: { ImU32 v32 = (ImU32) * (ImU8*)v;  bool r = SliderBehaviorT<ImU32, ImS32, float >(bb, id, HanaLovesMeDataType_U32, &v32, *(const ImU8*)v_min, *(const ImU8*)v_max, format, power, flags, out_grab_bb); if (r)* (ImU8*)v = (ImU8)v32;  return r; }
    case HanaLovesMeDataType_S16: { ImS32 v32 = (ImS32) * (ImS16*)v; bool r = SliderBehaviorT<ImS32, ImS32, float >(bb, id, HanaLovesMeDataType_S32, &v32, *(const ImS16*)v_min, *(const ImS16*)v_max, format, power, flags, out_grab_bb); if (r)* (ImS16*)v = (ImS16)v32; return r; }
    case HanaLovesMeDataType_U16: { ImU32 v32 = (ImU32) * (ImU16*)v; bool r = SliderBehaviorT<ImU32, ImS32, float >(bb, id, HanaLovesMeDataType_U32, &v32, *(const ImU16*)v_min, *(const ImU16*)v_max, format, power, flags, out_grab_bb); if (r)* (ImU16*)v = (ImU16)v32; return r; }
    case HanaLovesMeDataType_S32:
        IM_ASSERT(*(const ImS32*)v_min >= IM_S32_MIN / 2 && *(const ImS32*)v_max <= IM_S32_MAX / 2);
        return SliderBehaviorT<ImS32, ImS32, float >(bb, id, data_type, (ImS32*)v, *(const ImS32*)v_min, *(const ImS32*)v_max, format, power, flags, out_grab_bb);
    case HanaLovesMeDataType_U32:
        IM_ASSERT(*(const ImU32*)v_min <= IM_U32_MAX / 2);
        return SliderBehaviorT<ImU32, ImS32, float >(bb, id, data_type, (ImU32*)v, *(const ImU32*)v_min, *(const ImU32*)v_max, format, power, flags, out_grab_bb);
    case HanaLovesMeDataType_S64:
        IM_ASSERT(*(const ImS64*)v_min >= IM_S64_MIN / 2 && *(const ImS64*)v_max <= IM_S64_MAX / 2);
        return SliderBehaviorT<ImS64, ImS64, double>(bb, id, data_type, (ImS64*)v, *(const ImS64*)v_min, *(const ImS64*)v_max, format, power, flags, out_grab_bb);
    case HanaLovesMeDataType_U64:
        IM_ASSERT(*(const ImU64*)v_min <= IM_U64_MAX / 2);
        return SliderBehaviorT<ImU64, ImS64, double>(bb, id, data_type, (ImU64*)v, *(const ImU64*)v_min, *(const ImU64*)v_max, format, power, flags, out_grab_bb);
    case HanaLovesMeDataType_Float:
        IM_ASSERT(*(const float*)v_min >= -FLT_MAX / 2.0f && *(const float*)v_max <= FLT_MAX / 2.0f);
        return SliderBehaviorT<float, float, float >(bb, id, data_type, (float*)v, *(const float*)v_min, *(const float*)v_max, format, power, flags, out_grab_bb);
    case HanaLovesMeDataType_Double:
        IM_ASSERT(*(const double*)v_min >= -DBL_MAX / 2.0f && *(const double*)v_max <= DBL_MAX / 2.0f);
        return SliderBehaviorT<double, double, double>(bb, id, data_type, (double*)v, *(const double*)v_min, *(const double*)v_max, format, power, flags, out_grab_bb);
    case HanaLovesMeDataType_COUNT: break;
    }
    IM_ASSERT(0);
    return false;
}

bool HanaLovesMe::SliderScalar(const char* label, HanaLovesMeDataType data_type, void* v, const void* v_min, const void* v_max, const char* format, float power)
{
	HanaLovesMeWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	HanaLovesMeContext& g = *GHanaLovesMe;
	const HanaLovesMeStyle& style = g.Style;

	const HanaLovesMeID id = window->GetID(label);
	const float w = CalcItemWidth();

	const vsize label_size = CalcTextSize(label, NULL, true);
	const vsrect frame_bb(window->DC.CursorPos, window->DC.CursorPos + vsize(w, label_size.y + style.FramePadding.y - 6)); // 6
	const vsrect total_bb(frame_bb.Min, frame_bb.Max + vsize(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, -0.0f));

	if (!ItemAdd(total_bb, id, &frame_bb)) {

		ItemSize(total_bb, style.FramePadding.y);
		return false;
	}

	IM_ASSERT(data_type >= 0 && data_type < HanaLovesMeDataType_COUNT);

	if (format == NULL)
		format = GDataTypeInfo[data_type].PrintFmt;
	else if (data_type == HanaLovesMeDataType_S32 && strcmp(format, "%d") != 0)
		format = PatchFormatStringFloatToInt(format);

	// Tabbing or CTRL-clicking on Slider turns it into an input box
	const bool tab_focus_requested = FocusableItemRegister(window, id);
	const bool hovered = ItemHoverable(frame_bb, id);

	if (tab_focus_requested || (hovered && g.IO.MouseClicked[0]) || g.NavActivateId == id || (g.NavInputId == id && g.ScalarAsInputTextId != id)) {

		SetActiveID(id, window);
		SetFocusID(id, window);
		FocusWindow(window);
		g.ActiveIdAllowNavDirFlags = (1 << HanaLovesMeDir_Up) | (1 << HanaLovesMeDir_Down);

		if (tab_focus_requested || g.IO.KeyCtrl || g.NavInputId == id)
			g.ScalarAsInputTextId = 0;
	}

	ItemSize(total_bb, style.FramePadding.y);

	vsrect grab_bb;
	const bool value_changed = SliderBehavior(frame_bb, id, data_type, v, v_min, v_max, format, power, HanaLovesMeSliderFlags_None, &grab_bb);

	if (value_changed)
		MarkItemEdited(id);

	// Draw our shit
	auto borderColor = xuicolor(0, 0, 0, 255);
	auto topColor = xuicolor(52, 52, 52, 255);
	auto bottomColor = xuicolor(68, 68, 68, 255);
	auto topColorHovered = xuicolor(62, 62, 62, 255);
	auto bottomColorHovered = xuicolor(78, 78, 78, 255);
	auto grabTopColor = GetColorU32(HanaLovesMeCol_MenuTheme);
	auto grabBottomColor = GetColorU32(HanaLovesMeCol_MenuTheme) - xuicolor(0, 0, 0, 125);

	if (hovered || IsItemActive())
		window->DrawList->AddRectFilledMultiColor(frame_bb.Min + vsize(0, 1), frame_bb.Max, topColorHovered, topColorHovered, bottomColorHovered, bottomColorHovered);
	else
		window->DrawList->AddRectFilledMultiColor(frame_bb.Min + vsize(0, 1), frame_bb.Max, topColor, topColor, bottomColor, bottomColor);

    window->DrawList->AddRect(frame_bb.Min - vsize(0,-1), frame_bb.Max - vsize(0, 0), borderColor, 0, false, 1); // drawslideroutline
	window->DrawList->AddRectFilledMultiColor(frame_bb.Min + vsize(0, 1), grab_bb.Max + vsize(1, 1), grabTopColor, grabTopColor, grabBottomColor, grabBottomColor);
	window->DrawList->AddRect(frame_bb.Min + vsize(0, 1), frame_bb.Max, borderColor, 0, false, 0.4);


	char value_buf[64];
	const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, v, format);

	PopFont();
	PushFont(globals::SliderFont);
	/*
	PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f));
	RenderTextClipped(vsize(grab_bb.Min.x - 101, grab_bb.Max.y - 5), vsize(grab_bb.Max.x + 101, grab_bb.Max.y + 8), value_buf, value_buf_end, NULL, vsize(0.51f, 0.5f));
	PopStyleColor();

	PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f));
	RenderTextClipped(vsize(grab_bb.Min.x - 105, grab_bb.Max.y - 5), vsize(grab_bb.Max.x + 101, grab_bb.Max.y + 8), value_buf, value_buf_end, NULL, vsize(0.51f, 0.5f));
	PopStyleColor();

	PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f));
	RenderTextClipped(vsize(grab_bb.Min.x - 101, grab_bb.Max.y - 7), vsize(grab_bb.Max.x + 101, grab_bb.Max.y + 7), value_buf, value_buf_end, NULL, vsize(0.51f, 0.5f));
	PopStyleColor();

	PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f));
	RenderTextClipped(vsize(grab_bb.Min.x - 105, grab_bb.Max.y - 7), vsize(grab_bb.Max.x + 101, grab_bb.Max.y + 7), value_buf, value_buf_end, NULL, vsize(0.51f, 0.5f));
	PopStyleColor();
	*/

	PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f));
	RenderTextClipped(vsize(grab_bb.Min.x - 101, grab_bb.Max.y - 5), vsize(grab_bb.Max.x + 101, grab_bb.Max.y + 7), value_buf, value_buf_end, NULL, vsize(0.51f, 0.5f));
	PopStyleColor();

	PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f));
	RenderTextClipped(vsize(grab_bb.Min.x - 105, grab_bb.Max.y - 5), vsize(grab_bb.Max.x + 101, grab_bb.Max.y + 7), value_buf, value_buf_end, NULL, vsize(0.51f, 0.5f));
	PopStyleColor();

	PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f));
	RenderTextClipped(vsize(grab_bb.Min.x - 103, grab_bb.Max.y - 7), vsize(grab_bb.Max.x + 101, grab_bb.Max.y + 7), value_buf, value_buf_end, NULL, vsize(0.51f, 0.5f));
	PopStyleColor();

	PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_Text, xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f));
	RenderTextClipped(vsize(grab_bb.Min.x - 103, grab_bb.Max.y - 6), vsize(grab_bb.Max.x + 101, grab_bb.Max.y + 8), value_buf, value_buf_end, NULL, vsize(0.51f, 0.5f));
	PopStyleColor();

	PopFont();
	PushFont(globals::menuFont);

	const vsrect text_bb(window->DC.CursorPos + vsize(-2, style.FramePadding.y), window->DC.CursorPos + vsize(0, style.FramePadding.y - 2) + label_size);

	if (label_size.x > 0) {

		//PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextShadow, xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f));
		//RenderText(vsize(frame_bb.Max.x + style.ItemInnerSpacing.x - 162, frame_bb.Min.y + style.FramePadding.y - 17), label);
		//PopStyleColor();

		RenderText(vsize(frame_bb.Max.x + style.ItemInnerSpacing.x - 163, frame_bb.Min.y + style.FramePadding.y - 18), label);
	}

	return value_changed;
}

// Add multiple sliders on 1 line for compact edition of multiple components
bool HanaLovesMe::SliderScalarN(const char* label, HanaLovesMeDataType data_type, void* v, int components, const void* v_min, const void* v_max, const char* format, float power)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    HanaLovesMeContext& g = *GHanaLovesMe;
    bool value_changed = false;
    BeginGroup();
    PushID(label);
    PushMultiItemsWidths(components);
    size_t type_size = GDataTypeInfo[data_type].Size;
    for (int i = 0; i < components; i++)
    {
        PushID(i);
        value_changed |= SliderScalar("", data_type, v, v_min, v_max, format, power);
        SameLine(0, g.Style.ItemInnerSpacing.x);
        PopID();
        PopItemWidth();
        v = (void*)((char*)v + type_size);
    }
    PopID();

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    return value_changed;
}

bool HanaLovesMe::SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, float power)
{
    return SliderScalar(label, HanaLovesMeDataType_Float, v, &v_min, &v_max, format, power);
}

bool HanaLovesMe::SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format, float power)
{
    return SliderScalarN(label, HanaLovesMeDataType_Float, v, 2, &v_min, &v_max, format, power);
}

bool HanaLovesMe::SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format, float power)
{
    return SliderScalarN(label, HanaLovesMeDataType_Float, v, 3, &v_min, &v_max, format, power);
}

bool HanaLovesMe::SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format, float power)
{
    return SliderScalarN(label, HanaLovesMeDataType_Float, v, 4, &v_min, &v_max, format, power);
}

bool HanaLovesMe::SliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max, const char* format)
{
    if (format == NULL)
        format = "%.0f deg";
    float v_deg = (*v_rad) * 360.0f / (2 * IM_PI);
    bool value_changed = SliderFloat(label, &v_deg, v_degrees_min, v_degrees_max, format, 1.0f);
    *v_rad = v_deg * (2 * IM_PI) / 360.0f;
    return value_changed;
}

bool HanaLovesMe::SliderInt(const char* label, int* v, int v_min, int v_max, const char* format)
{
    return SliderScalar(label, HanaLovesMeDataType_S32, v, &v_min, &v_max, format);
}

bool HanaLovesMe::SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format)
{
    return SliderScalarN(label, HanaLovesMeDataType_S32, v, 2, &v_min, &v_max, format);
}

bool HanaLovesMe::SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format)
{
    return SliderScalarN(label, HanaLovesMeDataType_S32, v, 3, &v_min, &v_max, format);
}

bool HanaLovesMe::SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format)
{
    return SliderScalarN(label, HanaLovesMeDataType_S32, v, 4, &v_min, &v_max, format);
}

bool HanaLovesMe::VSliderScalar(const char* label, const vsize & size, HanaLovesMeDataType data_type, void* v, const void* v_min, const void* v_max, const char* format, float power)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    HanaLovesMeContext& g = *GHanaLovesMe;
    const HanaLovesMeStyle& style = g.Style;
    const HanaLovesMeID id = window->GetID(label);

    const vsize label_size = CalcTextSize(label, NULL, true);
    const vsrect frame_bb(window->DC.CursorPos, window->DC.CursorPos + size);
    const vsrect bb(frame_bb.Min, frame_bb.Max + vsize(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

    ItemSize(bb, style.FramePadding.y);
    if (!ItemAdd(frame_bb, id))
        return false;

    // Default format string when passing NULL
    // Patch old "%.0f" format string to use "%d", read function comments for more details.
    IM_ASSERT(data_type >= 0 && data_type < HanaLovesMeDataType_COUNT);
    if (format == NULL)
        format = GDataTypeInfo[data_type].PrintFmt;
    else if (data_type == HanaLovesMeDataType_S32 && strcmp(format, "%d") != 0)
        format = PatchFormatStringFloatToInt(format);

    const bool hovered = ItemHoverable(frame_bb, id);
    if ((hovered && g.IO.MouseClicked[0]) || g.NavActivateId == id || g.NavInputId == id)
    {
        SetActiveID(id, window);
        SetFocusID(id, window);
        FocusWindow(window);
        g.ActiveIdAllowNavDirFlags = (1 << HanaLovesMeDir_Left) | (1 << HanaLovesMeDir_Right);
    }

    // Draw frame
    const ImU32 frame_col = GetColorU32(g.ActiveId == id ? HanaLovesMeCol_FrameBgActive : g.HoveredId == id ? HanaLovesMeCol_FrameBgHovered : HanaLovesMeCol_FrameBg);
    RenderNavHighlight(frame_bb, id);
    RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

    // Slider behavior
    vsrect grab_bb;
    const bool value_changed = SliderBehavior(frame_bb, id, data_type, v, v_min, v_max, format, power, HanaLovesMeSliderFlags_Vertical, &grab_bb);
    if (value_changed)
        MarkItemEdited(id);

    // Render grab
    if (grab_bb.Max.y > grab_bb.Min.y)
        window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, GetColorU32(g.ActiveId == id ? HanaLovesMeCol_SliderGrabActive : HanaLovesMeCol_SliderGrab), style.GrabRounding);

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    // For the vertical slider we allow centered text to overlap the frame padding
    char value_buf[64];
    const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, v, format);
    RenderTextClipped(vsize(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, value_buf, value_buf_end, NULL, vsize(0.5f, 0.0f));
    if (label_size.x > 0.0f)
        RenderText(vsize(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    return value_changed;
}

bool HanaLovesMe::VSliderFloat(const char* label, const vsize & size, float* v, float v_min, float v_max, const char* format, float power)
{
    return VSliderScalar(label, size, HanaLovesMeDataType_Float, v, &v_min, &v_max, format, power);
}

bool HanaLovesMe::VSliderInt(const char* label, const vsize & size, int* v, int v_min, int v_max, const char* format)
{
    return VSliderScalar(label, size, HanaLovesMeDataType_S32, v, &v_min, &v_max, format);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: InputScalar, InputFloat, InputInt, etc.
//-------------------------------------------------------------------------
// - ImParseFormatFindStart() [Internal]
// - ImParseFormatFindEnd() [Internal]
// - ImParseFormatTrimDecorations() [Internal]
// - ImParseFormatPrecision() [Internal]
// - InputScalarAsWidgetReplacement() [Internal]
// - InputScalar()
// - InputScalarN()
// - InputFloat()
// - InputFloat2()
// - InputFloat3()
// - InputFloat4()
// - InputInt()
// - InputInt2()
// - InputInt3()
// - InputInt4()
// - InputDouble()
//-------------------------------------------------------------------------

// We don't use strchr() because our strings are usually very short and often start with '%'
const char* ImParseFormatFindStart(const char* fmt)
{
    while (char c = fmt[0])
    {
        if (c == '%' && fmt[1] != '%')
            return fmt;
        else if (c == '%')
            fmt++;
        fmt++;
    }
    return fmt;
}

const char* ImParseFormatFindEnd(const char* fmt)
{
    // Printf/scanf types modifiers: I/L/h/j/l/t/w/z. Other uppercase letters qualify as types aka end of the format.
    if (fmt[0] != '%')
        return fmt;
    const unsigned int ignored_uppercase_mask = (1 << ('I' - 'A')) | (1 << ('L' - 'A'));
    const unsigned int ignored_lowercase_mask = (1 << ('h' - 'a')) | (1 << ('j' - 'a')) | (1 << ('l' - 'a')) | (1 << ('t' - 'a')) | (1 << ('w' - 'a')) | (1 << ('z' - 'a'));
    for (char c; (c = *fmt) != 0; fmt++)
    {
        if (c >= 'A' && c <= 'Z' && ((1 << (c - 'A')) & ignored_uppercase_mask) == 0)
            return fmt + 1;
        if (c >= 'a' && c <= 'z' && ((1 << (c - 'a')) & ignored_lowercase_mask) == 0)
            return fmt + 1;
    }
    return fmt;
}

// Extract the format out of a format string with leading or trailing decorations
//  fmt = "blah blah"  -> return fmt
//  fmt = "%.3f"       -> return fmt
//  fmt = "hello %.3f" -> return fmt + 6
//  fmt = "%.3f hello" -> return buf written with "%.3f"
const char* ImParseFormatTrimDecorations(const char* fmt, char* buf, size_t buf_size)
{
    const char* fmt_start = ImParseFormatFindStart(fmt);
    if (fmt_start[0] != '%')
        return fmt;
    const char* fmt_end = ImParseFormatFindEnd(fmt_start);
    if (fmt_end[0] == 0) // If we only have leading decoration, we don't need to copy the data.
        return fmt_start;
    ImStrncpy(buf, fmt_start, ImMin((size_t)(fmt_end - fmt_start) + 1, buf_size));
    return buf;
}

// Parse display precision back from the display format string
// FIXME: This is still used by some navigation code path to infer a minimum tweak step, but we should aim to rework widgets so it isn't needed.
int ImParseFormatPrecision(const char* fmt, int default_precision)
{
    fmt = ImParseFormatFindStart(fmt);
    if (fmt[0] != '%')
        return default_precision;
    fmt++;
    while (*fmt >= '0' && *fmt <= '9')
        fmt++;
    int precision = INT_MAX;
    if (*fmt == '.')
    {
        fmt = ImAtoi<int>(fmt + 1, &precision);
        if (precision < 0 || precision > 99)
            precision = default_precision;
    }
    if (*fmt == 'e' || *fmt == 'E') // Maximum precision with scientific notation
        precision = -1;
    if ((*fmt == 'g' || *fmt == 'G') && precision == INT_MAX)
        precision = -1;
    return (precision == INT_MAX) ? default_precision : precision;
}

// Create text input in place of an active drag/slider (used when doing a CTRL+Click on drag/slider widgets)
// FIXME: Facilitate using this in variety of other situations.
bool HanaLovesMe::InputScalarAsWidgetReplacement(const vsrect & bb, HanaLovesMeID id, const char* label, HanaLovesMeDataType data_type, void* data_ptr, const char* format)
{
    IM_UNUSED(id);
    HanaLovesMeContext& g = *GHanaLovesMe;

    // On the first frame, g.ScalarAsInputTextId == 0, then on subsequent frames it becomes == id.
    // We clear ActiveID on the first frame to allow the InputText() taking it back.
    if (g.ScalarAsInputTextId == 0)
        ClearActiveID();

    char fmt_buf[32];
    char data_buf[32];
    format = ImParseFormatTrimDecorations(format, fmt_buf, IM_ARRAYSIZE(fmt_buf));
    DataTypeFormatString(data_buf, IM_ARRAYSIZE(data_buf), data_type, data_ptr, format);
    ImStrTrimBlanks(data_buf);
    HanaLovesMeInputTextFlags flags = HanaLovesMeInputTextFlags_AutoSelectAll | ((data_type == HanaLovesMeDataType_Float || data_type == HanaLovesMeDataType_Double) ? HanaLovesMeInputTextFlags_CharsScientific : HanaLovesMeInputTextFlags_CharsDecimal);
    bool value_changed = InputTextEx(label, NULL, data_buf, IM_ARRAYSIZE(data_buf), bb.GetSize(), flags);
    if (g.ScalarAsInputTextId == 0)
    {
        // First frame we started displaying the InputText widget, we expect it to take the active id.
        IM_ASSERT(g.ActiveId == id);
        g.ScalarAsInputTextId = g.ActiveId;
    }
    if (value_changed)
        return DataTypeApplyOpFromText(data_buf, g.InputTextState.InitialTextA.Data, data_type, data_ptr, NULL);
    return false;
}

bool HanaLovesMe::InputScalar(const char* label, HanaLovesMeDataType data_type, void* data_ptr, const void* step, const void* step_fast, const char* format, HanaLovesMeInputTextFlags flags)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    HanaLovesMeContext& g = *GHanaLovesMe;
    HanaLovesMeStyle& style = g.Style;

    IM_ASSERT(data_type >= 0 && data_type < HanaLovesMeDataType_COUNT);
    if (format == NULL)
        format = GDataTypeInfo[data_type].PrintFmt;

    char buf[64];
    DataTypeFormatString(buf, IM_ARRAYSIZE(buf), data_type, data_ptr, format);

    bool value_changed = false;
    if ((flags & (HanaLovesMeInputTextFlags_CharsHexadecimal | HanaLovesMeInputTextFlags_CharsScientific)) == 0)
        flags |= HanaLovesMeInputTextFlags_CharsDecimal;
    flags |= HanaLovesMeInputTextFlags_AutoSelectAll;

    if (step != NULL)
    {
        const float button_size = GetFrameHeight();

        BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
        PushID(label);
        PushItemWidth(ImMax(1.0f, CalcItemWidth() - (button_size + style.ItemInnerSpacing.x) * 2));
        if (InputText("", buf, IM_ARRAYSIZE(buf), flags)) // PushId(label) + "" gives us the expected ID from outside point of view
            value_changed = DataTypeApplyOpFromText(buf, g.InputTextState.InitialTextA.Data, data_type, data_ptr, format);
        PopItemWidth();

        // Step buttons
        const vsize backup_frame_padding = style.FramePadding;
        style.FramePadding.x = style.FramePadding.y;
        HanaLovesMeButtonFlags button_flags = HanaLovesMeButtonFlags_Repeat | HanaLovesMeButtonFlags_DontClosePopups;
        if (flags & HanaLovesMeInputTextFlags_ReadOnly)
            button_flags |= HanaLovesMeButtonFlags_Disabled;
        SameLine(0, style.ItemInnerSpacing.x);
        if (ButtonEx("-", vsize(button_size, button_size), button_flags))
        {
            DataTypeApplyOp(data_type, '-', data_ptr, data_ptr, g.IO.KeyCtrl && step_fast ? step_fast : step);
            value_changed = true;
        }
        SameLine(0, style.ItemInnerSpacing.x);
        if (ButtonEx("+", vsize(button_size, button_size), button_flags))
        {
            DataTypeApplyOp(data_type, '+', data_ptr, data_ptr, g.IO.KeyCtrl && step_fast ? step_fast : step);
            value_changed = true;
        }
        SameLine(0, style.ItemInnerSpacing.x);
        TextEx(label, FindRenderedTextEnd(label));
        style.FramePadding = backup_frame_padding;

        PopID();
        EndGroup();
    } else
    {
        if (InputText(label, buf, IM_ARRAYSIZE(buf), flags))
            value_changed = DataTypeApplyOpFromText(buf, g.InputTextState.InitialTextA.Data, data_type, data_ptr, format);
    }

    return value_changed;
}

bool HanaLovesMe::InputScalarN(const char* label, HanaLovesMeDataType data_type, void* v, int components, const void* step, const void* step_fast, const char* format, HanaLovesMeInputTextFlags flags)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    HanaLovesMeContext& g = *GHanaLovesMe;
    bool value_changed = false;
    BeginGroup();
    PushID(label);
    PushMultiItemsWidths(components);
    size_t type_size = GDataTypeInfo[data_type].Size;
    for (int i = 0; i < components; i++)
    {
        PushID(i);
        value_changed |= InputScalar("", data_type, v, step, step_fast, format, flags);
        SameLine(0, g.Style.ItemInnerSpacing.x);
        PopID();
        PopItemWidth();
        v = (void*)((char*)v + type_size);
    }
    PopID();

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    return value_changed;
}

bool HanaLovesMe::InputFloat(const char* label, float* v, float step, float step_fast, const char* format, HanaLovesMeInputTextFlags flags)
{
    flags |= HanaLovesMeInputTextFlags_CharsScientific;
    return InputScalar(label, HanaLovesMeDataType_Float, (void*)v, (void*)(step > 0.0f ? &step : NULL), (void*)(step_fast > 0.0f ? &step_fast : NULL), format, flags);
}

bool HanaLovesMe::InputFloat2(const char* label, float v[2], const char* format, HanaLovesMeInputTextFlags flags)
{
    return InputScalarN(label, HanaLovesMeDataType_Float, v, 2, NULL, NULL, format, flags);
}

bool HanaLovesMe::InputFloat3(const char* label, float v[3], const char* format, HanaLovesMeInputTextFlags flags)
{
    return InputScalarN(label, HanaLovesMeDataType_Float, v, 3, NULL, NULL, format, flags);
}

bool HanaLovesMe::InputFloat4(const char* label, float v[4], const char* format, HanaLovesMeInputTextFlags flags)
{
    return InputScalarN(label, HanaLovesMeDataType_Float, v, 4, NULL, NULL, format, flags);
}

// Prefer using "const char* format" directly, which is more flexible and consistent with other API.
#ifndef HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS
bool HanaLovesMe::InputFloat(const char* label, float* v, float step, float step_fast, int decimal_precision, HanaLovesMeInputTextFlags flags)
{
    char format[16] = "%f";
    if (decimal_precision >= 0)
        ImFormatString(format, IM_ARRAYSIZE(format), "%%.%df", decimal_precision);
    return InputFloat(label, v, step, step_fast, format, flags);
}

bool HanaLovesMe::InputFloat2(const char* label, float v[2], int decimal_precision, HanaLovesMeInputTextFlags flags)
{
    char format[16] = "%f";
    if (decimal_precision >= 0)
        ImFormatString(format, IM_ARRAYSIZE(format), "%%.%df", decimal_precision);
    return InputScalarN(label, HanaLovesMeDataType_Float, v, 2, NULL, NULL, format, flags);
}

bool HanaLovesMe::InputFloat3(const char* label, float v[3], int decimal_precision, HanaLovesMeInputTextFlags flags)
{
    char format[16] = "%f";
    if (decimal_precision >= 0)
        ImFormatString(format, IM_ARRAYSIZE(format), "%%.%df", decimal_precision);
    return InputScalarN(label, HanaLovesMeDataType_Float, v, 3, NULL, NULL, format, flags);
}

bool HanaLovesMe::InputFloat4(const char* label, float v[4], int decimal_precision, HanaLovesMeInputTextFlags flags)
{
    char format[16] = "%f";
    if (decimal_precision >= 0)
        ImFormatString(format, IM_ARRAYSIZE(format), "%%.%df", decimal_precision);
    return InputScalarN(label, HanaLovesMeDataType_Float, v, 4, NULL, NULL, format, flags);
}
#endif // HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS

bool HanaLovesMe::InputInt(const char* label, int* v, int step, int step_fast, HanaLovesMeInputTextFlags flags)
{
    // Hexadecimal input provided as a convenience but the flag name is awkward. Typically you'd use InputText() to parse your own data, if you want to handle prefixes.
    const char* format = (flags & HanaLovesMeInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
    return InputScalar(label, HanaLovesMeDataType_S32, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
}

bool HanaLovesMe::InputInt2(const char* label, int v[2], HanaLovesMeInputTextFlags flags)
{
    return InputScalarN(label, HanaLovesMeDataType_S32, v, 2, NULL, NULL, "%d", flags);
}

bool HanaLovesMe::InputInt3(const char* label, int v[3], HanaLovesMeInputTextFlags flags)
{
    return InputScalarN(label, HanaLovesMeDataType_S32, v, 3, NULL, NULL, "%d", flags);
}

bool HanaLovesMe::InputInt4(const char* label, int v[4], HanaLovesMeInputTextFlags flags)
{
    return InputScalarN(label, HanaLovesMeDataType_S32, v, 4, NULL, NULL, "%d", flags);
}

bool HanaLovesMe::InputDouble(const char* label, double* v, double step, double step_fast, const char* format, HanaLovesMeInputTextFlags flags)
{
    flags |= HanaLovesMeInputTextFlags_CharsScientific;
    return InputScalar(label, HanaLovesMeDataType_Double, (void*)v, (void*)(step > 0.0 ? &step : NULL), (void*)(step_fast > 0.0 ? &step_fast : NULL), format, flags);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: InputText, InputTextMultiline, InputTextWithHint
//-------------------------------------------------------------------------
// - InputText()
// - InputTextWithHint()
// - InputTextMultiline()
// - InputTextEx() [Internal]
//-------------------------------------------------------------------------

bool HanaLovesMe::InputText(const char* label, char* buf, size_t buf_size, HanaLovesMeInputTextFlags flags, HanaLovesMeInputTextCallback callback, void* user_data)
{
    IM_ASSERT(!(flags & HanaLovesMeInputTextFlags_Multiline)); // call InputTextMultiline()
    return InputTextEx(label, NULL, buf, (int)buf_size, vsize(0, 0), flags, callback, user_data);
}

bool HanaLovesMe::InputTextMultiline(const char* label, char* buf, size_t buf_size, const vsize & size, HanaLovesMeInputTextFlags flags, HanaLovesMeInputTextCallback callback, void* user_data)
{
    return InputTextEx(label, NULL, buf, (int)buf_size, size, flags | HanaLovesMeInputTextFlags_Multiline, callback, user_data);
}

bool HanaLovesMe::InputTextWithHint(const char* label, const char* hint, char* buf, size_t buf_size, HanaLovesMeInputTextFlags flags, HanaLovesMeInputTextCallback callback, void* user_data)
{
    IM_ASSERT(!(flags & HanaLovesMeInputTextFlags_Multiline)); // call InputTextMultiline()
    return InputTextEx(label, hint, buf, (int)buf_size, vsize(0, 0), flags, callback, user_data);
}

static int InputTextCalcTextLenAndLineCount(const char* text_begin, const char** out_text_end)
{
    int line_count = 0;
    const char* s = text_begin;
    while (char c = *s++) // We are only matching for \n so we can ignore UTF-8 decoding
        if (c == '\n')
            line_count++;
    s--;
    if (s[0] != '\n' && s[0] != '\r')
        line_count++;
    *out_text_end = s;
    return line_count;
}

static vsize InputTextCalcTextSizeW(const ImWchar * text_begin, const ImWchar * text_end, const ImWchar * *remaining, vsize * out_offset, bool stop_on_new_line)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    ImFont* font = g.Font;
    const float line_height = g.FontSize;
    const float scale = line_height / font->FontSize;

    vsize text_size = vsize(0, 0);
    float line_width = 0.0f;

    const ImWchar* s = text_begin;
    while (s < text_end)
    {
        unsigned int c = (unsigned int)(*s++);
        if (c == '\n')
        {
            text_size.x = ImMax(text_size.x, line_width);
            text_size.y += line_height;
            line_width = 0.0f;
            if (stop_on_new_line)
                break;
            continue;
        }
        if (c == '\r')
            continue;

        const float char_width = font->GetCharAdvance((ImWchar)c) * scale;
        line_width += char_width;
    }

    if (text_size.x < line_width)
        text_size.x = line_width;

    if (out_offset)
        * out_offset = vsize(line_width, text_size.y + line_height);  // offset allow for the possibility of sitting after a trailing \n

    if (line_width > 0 || text_size.y == 0.0f)                        // whereas size.y will ignore the trailing \n
        text_size.y += line_height;

    if (remaining)
        * remaining = s;

    return text_size;
}

// Wrapper for stb_textedit.h to edit text (our wrapper is for: statically sized buffer, single-line, wchar characters. InputText converts between UTF-8 and wchar)
namespace x07dns
{

    static int     STB_TEXTEDIT_STRINGLEN(const STB_TEXTEDIT_STRING* obj) { return obj->CurLenW; }
    static ImWchar STB_TEXTEDIT_GETCHAR(const STB_TEXTEDIT_STRING* obj, int idx) { return obj->TextW[idx]; }
    static float   STB_TEXTEDIT_GETWIDTH(STB_TEXTEDIT_STRING* obj, int line_start_idx, int char_idx) { ImWchar c = obj->TextW[line_start_idx + char_idx]; if (c == '\n') return STB_TEXTEDIT_GETWIDTH_NEWLINE; return GHanaLovesMe->Font->GetCharAdvance(c) * (GHanaLovesMe->FontSize / GHanaLovesMe->Font->FontSize); }
    static int     STB_TEXTEDIT_KEYTOTEXT(int key) { return key >= 0x10000 ? 0 : key; }
    static ImWchar STB_TEXTEDIT_NEWLINE = '\n';
    static void    STB_TEXTEDIT_LAYOUTROW(StbTexteditRow * r, STB_TEXTEDIT_STRING * obj, int line_start_idx)
    {
        const ImWchar* text = obj->TextW.Data;
        const ImWchar* text_remaining = NULL;
        const vsize size = InputTextCalcTextSizeW(text + line_start_idx, text + obj->CurLenW, &text_remaining, NULL, true);
        r->x0 = 0.0f;
        r->x1 = size.x;
        r->baseline_y_delta = size.y;
        r->ymin = 0.0f;
        r->ymax = size.y;
        r->num_chars = (int)(text_remaining - (text + line_start_idx));
    }

    static bool is_separator(unsigned int c) { return ImCharIsBlankW(c) || c == ',' || c == ';' || c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == '|'; }
    static int  is_word_boundary_from_right(STB_TEXTEDIT_STRING * obj, int idx) { return idx > 0 ? (is_separator(obj->TextW[idx - 1]) && !is_separator(obj->TextW[idx])) : 1; }
    static int  STB_TEXTEDIT_MOVEWORDLEFT_IMPL(STB_TEXTEDIT_STRING * obj, int idx) { idx--; while (idx >= 0 && !is_word_boundary_from_right(obj, idx)) idx--; return idx < 0 ? 0 : idx; }
#ifdef __APPLE__    // FIXME: Move setting to IO structure
    static int  is_word_boundary_from_left(STB_TEXTEDIT_STRING * obj, int idx) { return idx > 0 ? (!is_separator(obj->TextW[idx - 1]) && is_separator(obj->TextW[idx])) : 1; }
    static int  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL(STB_TEXTEDIT_STRING * obj, int idx) { idx++; int len = obj->CurLenW; while (idx < len && !is_word_boundary_from_left(obj, idx)) idx++; return idx > len ? len : idx; }
#else
    static int  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL(STB_TEXTEDIT_STRING * obj, int idx) { idx++; int len = obj->CurLenW; while (idx < len && !is_word_boundary_from_right(obj, idx)) idx++; return idx > len ? len : idx; }
#endif
#define STB_TEXTEDIT_MOVEWORDLEFT   STB_TEXTEDIT_MOVEWORDLEFT_IMPL    // They need to be #define for stb_textedit.h
#define STB_TEXTEDIT_MOVEWORDRIGHT  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL

    static void STB_TEXTEDIT_DELETECHARS(STB_TEXTEDIT_STRING * obj, int pos, int n)
    {
        ImWchar* dst = obj->TextW.Data + pos;

        // We maintain our buffer length in both UTF-8 and wchar formats
        obj->CurLenA -= ImTextCountUtf8BytesFromStr(dst, dst + n);
        obj->CurLenW -= n;

        // Offset remaining text (FIXME-OPT: Use memmove)
        const ImWchar * src = obj->TextW.Data + pos + n;
        while (ImWchar c = *src++)
            * dst++ = c;
        *dst = '\0';
    }

    static bool STB_TEXTEDIT_INSERTCHARS(STB_TEXTEDIT_STRING * obj, int pos, const ImWchar * new_text, int new_text_len)
    {
        const bool is_resizable = (obj->UserFlags & HanaLovesMeInputTextFlags_CallbackResize) != 0;
        const int text_len = obj->CurLenW;
        IM_ASSERT(pos <= text_len);

        const int new_text_len_utf8 = ImTextCountUtf8BytesFromStr(new_text, new_text + new_text_len);
        if (!is_resizable && (new_text_len_utf8 + obj->CurLenA + 1 > obj->BufCapacityA))
            return false;

        // Grow internal buffer if needed
        if (new_text_len + text_len + 1 > obj->TextW.Size)
        {
            if (!is_resizable)
                return false;
            IM_ASSERT(text_len < obj->TextW.Size);
            obj->TextW.resize(text_len + ImClamp(new_text_len * 4, 32, ImMax(256, new_text_len)) + 1);
        }

        ImWchar* text = obj->TextW.Data;
        if (pos != text_len)
            memmove(text + pos + new_text_len, text + pos, (size_t)(text_len - pos) * sizeof(ImWchar));
        memcpy(text + pos, new_text, (size_t)new_text_len * sizeof(ImWchar));

        obj->CurLenW += new_text_len;
        obj->CurLenA += new_text_len_utf8;
        obj->TextW[obj->CurLenW] = '\0';

        return true;
    }

    // We don't use an enum so we can build even with conflicting symbols (if another user of stb_textedit.h leak their STB_TEXTEDIT_K_* symbols)
#define STB_TEXTEDIT_K_LEFT         0x10000 // keyboard input to move cursor left
#define STB_TEXTEDIT_K_RIGHT        0x10001 // keyboard input to move cursor right
#define STB_TEXTEDIT_K_UP           0x10002 // keyboard input to move cursor up
#define STB_TEXTEDIT_K_DOWN         0x10003 // keyboard input to move cursor down
#define STB_TEXTEDIT_K_LINESTART    0x10004 // keyboard input to move cursor to start of line
#define STB_TEXTEDIT_K_LINEEND      0x10005 // keyboard input to move cursor to end of line
#define STB_TEXTEDIT_K_TEXTSTART    0x10006 // keyboard input to move cursor to start of text
#define STB_TEXTEDIT_K_TEXTEND      0x10007 // keyboard input to move cursor to end of text
#define STB_TEXTEDIT_K_DELETE       0x10008 // keyboard input to delete selection or character under cursor
#define STB_TEXTEDIT_K_BACKSPACE    0x10009 // keyboard input to delete selection or character left of cursor
#define STB_TEXTEDIT_K_UNDO         0x1000A // keyboard input to perform undo
#define STB_TEXTEDIT_K_REDO         0x1000B // keyboard input to perform redo
#define STB_TEXTEDIT_K_WORDLEFT     0x1000C // keyboard input to move cursor left one word
#define STB_TEXTEDIT_K_WORDRIGHT    0x1000D // keyboard input to move cursor right one word
#define STB_TEXTEDIT_K_SHIFT        0x20000

#define STB_TEXTEDIT_IMPLEMENTATION
#include "x07dns_textedit.h"

}

void HanaLovesMeInputTextState::OnKeyPressed(int key)
{
    stb_textedit_key(this, &Stb, key);
    CursorFollow = true;
    CursorAnimReset();
}

HanaLovesMeInputTextCallbackData::HanaLovesMeInputTextCallbackData()
{
    memset(this, 0, sizeof(*this));
}

// Public API to manipulate UTF-8 text
// We expose UTF-8 to the user (unlike the STB_TEXTEDIT_* functions which are manipulating wchar)
// FIXME: The existence of this rarely exercised code path is a bit of a nuisance.
void HanaLovesMeInputTextCallbackData::DeleteChars(int pos, int bytes_count)
{
    IM_ASSERT(pos + bytes_count <= BufTextLen);
    char* dst = Buf + pos;
    const char* src = Buf + pos + bytes_count;
    while (char c = *src++)
        * dst++ = c;
    *dst = '\0';

    if (CursorPos + bytes_count >= pos)
        CursorPos -= bytes_count;
    else if (CursorPos >= pos)
        CursorPos = pos;
    SelectionStart = SelectionEnd = CursorPos;
    BufDirty = true;
    BufTextLen -= bytes_count;
}

void HanaLovesMeInputTextCallbackData::InsertChars(int pos, const char* new_text, const char* new_text_end)
{
    const bool is_resizable = (Flags & HanaLovesMeInputTextFlags_CallbackResize) != 0;
    const int new_text_len = new_text_end ? (int)(new_text_end - new_text) : (int)strlen(new_text);
    if (new_text_len + BufTextLen >= BufSize)
    {
        if (!is_resizable)
            return;

        // Contrary to STB_TEXTEDIT_INSERTCHARS() this is working in the UTF8 buffer, hence the midly similar code (until we remove the U16 buffer alltogether!)
        HanaLovesMeContext& g = *GHanaLovesMe;
        HanaLovesMeInputTextState* edit_state = &g.InputTextState;
        IM_ASSERT(edit_state->ID != 0 && g.ActiveId == edit_state->ID);
        IM_ASSERT(Buf == edit_state->TextA.Data);
        int new_buf_size = BufTextLen + ImClamp(new_text_len * 4, 32, ImMax(256, new_text_len)) + 1;
        edit_state->TextA.reserve(new_buf_size + 1);
        Buf = edit_state->TextA.Data;
        BufSize = edit_state->BufCapacityA = new_buf_size;
    }

    if (BufTextLen != pos)
        memmove(Buf + pos + new_text_len, Buf + pos, (size_t)(BufTextLen - pos));
    memcpy(Buf + pos, new_text, (size_t)new_text_len * sizeof(char));
    Buf[BufTextLen + new_text_len] = '\0';

    if (CursorPos >= pos)
        CursorPos += new_text_len;
    SelectionStart = SelectionEnd = CursorPos;
    BufDirty = true;
    BufTextLen += new_text_len;
}

// Return false to discard a character.
static bool InputTextFilterCharacter(unsigned int* p_char, HanaLovesMeInputTextFlags flags, HanaLovesMeInputTextCallback callback, void* user_data)
{
    unsigned int c = *p_char;

    // Filter non-printable (NB: isprint is unreliable! see #2467)
    if (c < 0x20)
    {
        bool pass = false;
        pass |= (c == '\n' && (flags & HanaLovesMeInputTextFlags_Multiline));
        pass |= (c == '\t' && (flags & HanaLovesMeInputTextFlags_AllowTabInput));
        if (!pass)
            return false;
    }

    // Filter private Unicode range. GLFW on OSX seems to send private characters for special keys like arrow keys (FIXME)
    if (c >= 0xE000 && c <= 0xF8FF)
        return false;

    // Generic named filters
    if (flags & (HanaLovesMeInputTextFlags_CharsDecimal | HanaLovesMeInputTextFlags_CharsHexadecimal | HanaLovesMeInputTextFlags_CharsUppercase | HanaLovesMeInputTextFlags_CharsNoBlank | HanaLovesMeInputTextFlags_CharsScientific))
    {
        if (flags & HanaLovesMeInputTextFlags_CharsDecimal)
            if (!(c >= '0' && c <= '9') && (c != '.') && (c != '-') && (c != '+') && (c != '*') && (c != '/'))
                return false;

        if (flags & HanaLovesMeInputTextFlags_CharsScientific)
            if (!(c >= '0' && c <= '9') && (c != '.') && (c != '-') && (c != '+') && (c != '*') && (c != '/') && (c != 'e') && (c != 'E'))
                return false;

        if (flags & HanaLovesMeInputTextFlags_CharsHexadecimal)
            if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F'))
                return false;

        if (flags & HanaLovesMeInputTextFlags_CharsUppercase)
            if (c >= 'a' && c <= 'z')
                * p_char = (c += (unsigned int)('A' - 'a'));

        if (flags & HanaLovesMeInputTextFlags_CharsNoBlank)
            if (ImCharIsBlankW(c))
                return false;
    }

    // Custom callback filter
    if (flags & HanaLovesMeInputTextFlags_CallbackCharFilter)
    {
        HanaLovesMeInputTextCallbackData callback_data;
        memset(&callback_data, 0, sizeof(HanaLovesMeInputTextCallbackData));
        callback_data.EventFlag = HanaLovesMeInputTextFlags_CallbackCharFilter;
        callback_data.EventChar = (ImWchar)c;
        callback_data.Flags = flags;
        callback_data.UserData = user_data;
        if (callback(&callback_data) != 0)
            return false;
        *p_char = callback_data.EventChar;
        if (!callback_data.EventChar)
            return false;
    }

    return true;
}

// Edit a string of text
// - buf_size account for the zero-terminator, so a buf_size of 6 can hold "Hello" but not "Hello!".
//   This is so we can easily call InputText() on static arrays using ARRAYSIZE() and to match
//   Note that in std::string world, capacity() would omit 1 byte used by the zero-terminator.
// - When active, hold on a privately held copy of the text (and apply back to 'buf'). So changing 'buf' while the InputText is active has no effect.
// - If you want to use HanaLovesMe::InputText() with std::string, see misc/cpp/HanaLovesMe_stdlib.h
// (FIXME: Rather confusing and messy function, among the worse part of our codebase, expecting to rewrite a V2 at some point.. Partly because we are
//  doing UTF8 > U16 > UTF8 conversions on the go to easily interface with stb_textedit. Ideally should stay in UTF-8 all the time. See https://github.com/nothings/stb/issues/188)
bool HanaLovesMe::InputTextEx(const char* label, const char* hint, char* buf, int buf_size, const vsize & size_arg, HanaLovesMeInputTextFlags flags, HanaLovesMeInputTextCallback callback, void* callback_user_data)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    IM_ASSERT(!((flags & HanaLovesMeInputTextFlags_CallbackHistory) && (flags & HanaLovesMeInputTextFlags_Multiline)));        // Can't use both together (they both use up/down keys)
    IM_ASSERT(!((flags & HanaLovesMeInputTextFlags_CallbackCompletion) && (flags & HanaLovesMeInputTextFlags_AllowTabInput))); // Can't use both together (they both use tab key)

    HanaLovesMeContext& g = *GHanaLovesMe;
    HanaLovesMeIO& io = g.IO;
    const HanaLovesMeStyle& style = g.Style;

    const bool RENDER_SELECTION_WHEN_INACTIVE = false;
    const bool is_multiline = (flags & HanaLovesMeInputTextFlags_Multiline) != 0;
    const bool is_readonly = (flags & HanaLovesMeInputTextFlags_ReadOnly) != 0;
    const bool is_password = (flags & HanaLovesMeInputTextFlags_Password) != 0;
    const bool is_undoable = (flags & HanaLovesMeInputTextFlags_NoUndoRedo) == 0;
    const bool is_resizable = (flags & HanaLovesMeInputTextFlags_CallbackResize) != 0;
    if (is_resizable)
        IM_ASSERT(callback != NULL); // Must provide a callback if you set the HanaLovesMeInputTextFlags_CallbackResize flag!

    if (is_multiline) // Open group before calling GetID() because groups tracks id created within their scope,
        BeginGroup();
    const HanaLovesMeID id = window->GetID(label);
    const vsize label_size = CalcTextSize(label, NULL, true);
    vsize size = CalcItemSize(size_arg, CalcItemWidth(), (is_multiline ? GetTextLineHeight() * 8.0f : label_size.y) + style.FramePadding.y * 2.0f); // Arbitrary default of 8 lines high for multi-line
    const vsrect frame_bb(window->DC.CursorPos, window->DC.CursorPos + size);
    const vsrect total_bb(frame_bb.Min, frame_bb.Max + vsize(label_size.x > 0.0f ? (style.ItemInnerSpacing.x + label_size.x) : 0.0f, 0.0f));

    HanaLovesMeWindow * draw_window = window;
    if (is_multiline)
    {
        if (!ItemAdd(total_bb, id, &frame_bb))
        {
            ItemSize(total_bb, style.FramePadding.y);
            EndGroup();
            return false;
        }
        if (!BeginChildFrame(id, frame_bb.GetSize()))
        {
            EndChildFrame();
            EndGroup();
            return false;
        }
        draw_window = GetCurrentWindow();
        draw_window->DC.NavLayerActiveMaskNext |= draw_window->DC.NavLayerCurrentMask; // This is to ensure that EndChild() will display a navigation highlight
        size.x -= draw_window->ScrollbarSizes.x;
    } else
    {
        ItemSize(total_bb, style.FramePadding.y);
        if (!ItemAdd(total_bb, id, &frame_bb))
            return false;
    }
    const bool hovered = ItemHoverable(frame_bb, id);
    if (hovered)
        g.MouseCursor = HanaLovesMeMouseCursor_TextInput;

    // NB: we are only allowed to access 'edit_state' if we are the active widget.
    HanaLovesMeInputTextState* state = NULL;
    if (g.InputTextState.ID == id)
        state = &g.InputTextState;

    const bool focus_requested = FocusableItemRegister(window, id);
    const bool focus_requested_by_code = focus_requested && (g.FocusRequestCurrWindow == window && g.FocusRequestCurrCounterAll == window->DC.FocusCounterAll);
    const bool focus_requested_by_tab = focus_requested && !focus_requested_by_code;

    const bool user_clicked = hovered && io.MouseClicked[0];
    const bool user_nav_input_start = (g.ActiveId != id) && ((g.NavInputId == id) || (g.NavActivateId == id && g.NavInputSource == HanaLovesMeInputSource_NavKeyboard));
    const bool user_scroll_finish = is_multiline && state != NULL && g.ActiveId == 0 && g.ActiveIdPreviousFrame == GetScrollbarID(draw_window, HanaLovesMeAxis_Y);
    const bool user_scroll_active = is_multiline && state != NULL && g.ActiveId == GetScrollbarID(draw_window, HanaLovesMeAxis_Y);

    bool clear_active_id = false;
    bool select_all = (g.ActiveId != id) && ((flags & HanaLovesMeInputTextFlags_AutoSelectAll) != 0 || user_nav_input_start) && (!is_multiline);

    const bool init_make_active = (focus_requested || user_clicked || user_scroll_finish || user_nav_input_start);
    const bool init_state = (init_make_active || user_scroll_active);
    if (init_state && g.ActiveId != id)
    {
        // Access state even if we don't own it yet.
        state = &g.InputTextState;
        state->CursorAnimReset();

        // Take a copy of the initial buffer value (both in original UTF-8 format and converted to wchar)
        // From the moment we focused we are ignoring the content of 'buf' (unless we are in read-only mode)
        const int buf_len = (int)strlen(buf);
        state->InitialTextA.resize(buf_len + 1);    // UTF-8. we use +1 to make sure that .Data is always pointing to at least an empty string.
        memcpy(state->InitialTextA.Data, buf, buf_len + 1);

        // Start edition
        const char* buf_end = NULL;
        state->TextW.resize(buf_size + 1);          // wchar count <= UTF-8 count. we use +1 to make sure that .Data is always pointing to at least an empty string.
        state->TextA.resize(0);
        state->TextAIsValid = false;                // TextA is not valid yet (we will display buf until then)
        state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, buf_size, buf, NULL, &buf_end);
        state->CurLenA = (int)(buf_end - buf);      // We can't get the result from ImStrncpy() above because it is not UTF-8 aware. Here we'll cut off malformed UTF-8.

        // Preserve cursor position and undo/redo stack if we come back to same widget
        // FIXME: For non-readonly widgets we might be able to require that TextAIsValid && TextA == buf ? (untested) and discard undo stack if user buffer has changed.
        const bool recycle_state = (state->ID == id);
        if (recycle_state)
        {
            // Recycle existing cursor/selection/undo stack but clamp position
            // Note a single mouse click will override the cursor/position immediately by calling stb_textedit_click handler.
            state->CursorClamp();
        } else
        {
            state->ID = id;
            state->ScrollX = 0.0f;
            stb_textedit_initialize_state(&state->Stb, !is_multiline);
            if (!is_multiline && focus_requested_by_code)
                select_all = true;
        }
        if (flags & HanaLovesMeInputTextFlags_AlwaysInsertMode)
            state->Stb.insert_mode = 1;
        if (!is_multiline && (focus_requested_by_tab || (user_clicked && io.KeyCtrl)))
            select_all = true;
    }

    if (g.ActiveId != id && init_make_active)
    {
        IM_ASSERT(state && state->ID == id);
        SetActiveID(id, window);
        SetFocusID(id, window);
        FocusWindow(window);
        IM_ASSERT(HanaLovesMeNavInput_COUNT < 32);
        g.ActiveIdBlockNavInputFlags = (1 << HanaLovesMeNavInput_Cancel);
        if (flags & (HanaLovesMeInputTextFlags_CallbackCompletion | HanaLovesMeInputTextFlags_AllowTabInput))  // Disable keyboard tabbing out as we will use the \t character.
            g.ActiveIdBlockNavInputFlags |= (1 << HanaLovesMeNavInput_KeyTab_);
        if (!is_multiline && !(flags & HanaLovesMeInputTextFlags_CallbackHistory))
            g.ActiveIdAllowNavDirFlags = ((1 << HanaLovesMeDir_Up) | (1 << HanaLovesMeDir_Down));
    }

    // We have an edge case if ActiveId was set through another widget (e.g. widget being swapped), clear id immediately (don't wait until the end of the function)
    if (g.ActiveId == id && state == NULL)
        ClearActiveID();

    // Release focus when we click outside
    if (g.ActiveId == id && io.MouseClicked[0] && !init_state && !init_make_active) //-V560
        clear_active_id = true;

    // Lock the decision of whether we are going to take the path displaying the cursor or selection
    const bool render_cursor = (g.ActiveId == id) || (state && user_scroll_active);
    bool render_selection = state && state->HasSelection() && (RENDER_SELECTION_WHEN_INACTIVE || render_cursor);
    bool value_changed = false;
    bool enter_pressed = false;

    // When read-only we always use the live data passed to the function
    // FIXME-OPT: Because our selection/cursor code currently needs the wide text we need to convert it when active, which is not ideal :(
    if (is_readonly && state != NULL && (render_cursor || render_selection))
    {
        const char* buf_end = NULL;
        state->TextW.resize(buf_size + 1);
        state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, state->TextW.Size, buf, NULL, &buf_end);
        state->CurLenA = (int)(buf_end - buf);
        state->CursorClamp();
        render_selection &= state->HasSelection();
    }

    // Select the buffer to render.
    const bool buf_display_from_state = (render_cursor || render_selection || g.ActiveId == id) && !is_readonly && state && state->TextAIsValid;
    const bool is_displaying_hint = (hint != NULL && (buf_display_from_state ? state->TextA.Data : buf)[0] == 0);

    // Password pushes a temporary font with only a fallback glyph
    if (is_password && !is_displaying_hint)
    {
        const ImFontGlyph* glyph = g.Font->FindGlyph('*');
        ImFont* password_font = &g.InputTextPasswordFont;
        password_font->FontSize = g.Font->FontSize;
        password_font->Scale = g.Font->Scale;
        password_font->DisplayOffset = g.Font->DisplayOffset;
        password_font->Ascent = g.Font->Ascent;
        password_font->Descent = g.Font->Descent;
        password_font->ContainerAtlas = g.Font->ContainerAtlas;
        password_font->FallbackGlyph = glyph;
        password_font->FallbackAdvanceX = glyph->AdvanceX;
        IM_ASSERT(password_font->Glyphs.empty() && password_font->IndexAdvanceX.empty() && password_font->IndexLookup.empty());
        PushFont(password_font);
    }

    // Process mouse inputs and character inputs
    int backup_current_text_length = 0;
    if (g.ActiveId == id)
    {
        IM_ASSERT(state != NULL);
        backup_current_text_length = state->CurLenA;
        state->BufCapacityA = buf_size;
        state->UserFlags = flags;
        state->UserCallback = callback;
        state->UserCallbackData = callback_user_data;

        // Although we are active we don't prevent mouse from hovering other elements unless we are interacting right now with the widget.
        // Down the line we should have a cleaner library-wide concept of Selected vs Active.
        g.ActiveIdAllowOverlap = !io.MouseDown[0];
        g.WantTextInputNextFrame = 1;

        // Edit in progress
        const float mouse_x = (io.MousePos.x - frame_bb.Min.x - style.FramePadding.x) + state->ScrollX;
        const float mouse_y = (is_multiline ? (io.MousePos.y - draw_window->DC.CursorPos.y - style.FramePadding.y) : (g.FontSize * 0.5f));

        const bool is_osx = io.ConfigMacOSXBehaviors;
        if (select_all || (hovered && !is_osx && io.MouseDoubleClicked[0]))
        {
            state->SelectAll();
            state->SelectedAllMouseLock = true;
        } else if (hovered && is_osx && io.MouseDoubleClicked[0])
        {
            // Double-click select a word only, OS X style (by simulating keystrokes)
            state->OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT);
            state->OnKeyPressed(STB_TEXTEDIT_K_WORDRIGHT | STB_TEXTEDIT_K_SHIFT);
        } else if (io.MouseClicked[0] && !state->SelectedAllMouseLock)
        {
            if (hovered)
            {
                stb_textedit_click(state, &state->Stb, mouse_x, mouse_y);
                state->CursorAnimReset();
            }
        } else if (io.MouseDown[0] && !state->SelectedAllMouseLock && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f))
        {
            stb_textedit_drag(state, &state->Stb, mouse_x, mouse_y);
            state->CursorAnimReset();
            state->CursorFollow = true;
        }
        if (state->SelectedAllMouseLock && !io.MouseDown[0])
            state->SelectedAllMouseLock = false;

        // It is ill-defined whether the back-end needs to send a \t character when pressing the TAB keys.
        // Win32 and GLFW naturally do it but not SDL.
        const bool ignore_char_inputs = (io.KeyCtrl && !io.KeyAlt) || (is_osx && io.KeySuper);
        if ((flags & HanaLovesMeInputTextFlags_AllowTabInput) && IsKeyPressedMap(HanaLovesMeKey_Tab) && !ignore_char_inputs && !io.KeyShift && !is_readonly)
            if (!io.InputQueueCharacters.contains('\t'))
            {
                unsigned int c = '\t'; // Insert TAB
                if (InputTextFilterCharacter(&c, flags, callback, callback_user_data))
                    state->OnKeyPressed((int)c);
            }

        // Process regular text input (before we check for Return because using some IME will effectively send a Return?)
        // We ignore CTRL inputs, but need to allow ALT+CTRL as some keyboards (e.g. German) use AltGR (which _is_ Alt+Ctrl) to input certain characters.
        if (io.InputQueueCharacters.Size > 0)
        {
            if (!ignore_char_inputs && !is_readonly && !user_nav_input_start)
                for (int n = 0; n < io.InputQueueCharacters.Size; n++)
                {
                    // Insert character if they pass filtering
                    unsigned int c = (unsigned int)io.InputQueueCharacters[n];
                    if (c == '\t' && io.KeyShift)
                        continue;
                    if (InputTextFilterCharacter(&c, flags, callback, callback_user_data))
                        state->OnKeyPressed((int)c);
                }

            // Consume characters
            io.InputQueueCharacters.resize(0);
        }
    }

    // Process other shortcuts/key-presses
    bool cancel_edit = false;
    if (g.ActiveId == id && !g.ActiveIdIsJustActivated && !clear_active_id)
    {
        IM_ASSERT(state != NULL);
        const int k_mask = (io.KeyShift ? STB_TEXTEDIT_K_SHIFT : 0);
        const bool is_osx = io.ConfigMacOSXBehaviors;
        const bool is_shortcut_key = (is_osx ? (io.KeySuper && !io.KeyCtrl) : (io.KeyCtrl && !io.KeySuper)) && !io.KeyAlt && !io.KeyShift; // OS X style: Shortcuts using Cmd/Super instead of Ctrl
        const bool is_osx_shift_shortcut = is_osx && io.KeySuper && io.KeyShift && !io.KeyCtrl && !io.KeyAlt;
        const bool is_wordmove_key_down = is_osx ? io.KeyAlt : io.KeyCtrl;                     // OS X style: Text editing cursor movement using Alt instead of Ctrl
        const bool is_startend_key_down = is_osx && io.KeySuper && !io.KeyCtrl && !io.KeyAlt;  // OS X style: Line/Text Start and End using Cmd+Arrows instead of Home/End
        const bool is_ctrl_key_only = io.KeyCtrl && !io.KeyShift && !io.KeyAlt && !io.KeySuper;
        const bool is_shift_key_only = io.KeyShift && !io.KeyCtrl && !io.KeyAlt && !io.KeySuper;

        const bool is_cut = ((is_shortcut_key && IsKeyPressedMap(HanaLovesMeKey_X)) || (is_shift_key_only && IsKeyPressedMap(HanaLovesMeKey_Delete))) && !is_readonly && !is_password && (!is_multiline || state->HasSelection());
        const bool is_copy = ((is_shortcut_key && IsKeyPressedMap(HanaLovesMeKey_C)) || (is_ctrl_key_only && IsKeyPressedMap(HanaLovesMeKey_Insert))) && !is_password && (!is_multiline || state->HasSelection());
        const bool is_paste = ((is_shortcut_key && IsKeyPressedMap(HanaLovesMeKey_V)) || (is_shift_key_only && IsKeyPressedMap(HanaLovesMeKey_Insert))) && !is_readonly;
        const bool is_undo = ((is_shortcut_key && IsKeyPressedMap(HanaLovesMeKey_Z)) && !is_readonly && is_undoable);
        const bool is_redo = ((is_shortcut_key && IsKeyPressedMap(HanaLovesMeKey_Y)) || (is_osx_shift_shortcut && IsKeyPressedMap(HanaLovesMeKey_Z))) && !is_readonly && is_undoable;

        if (IsKeyPressedMap(HanaLovesMeKey_LeftArrow)) { state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINESTART : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDLEFT : STB_TEXTEDIT_K_LEFT) | k_mask); } else if (IsKeyPressedMap(HanaLovesMeKey_RightArrow)) { state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINEEND : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDRIGHT : STB_TEXTEDIT_K_RIGHT) | k_mask); } else if (IsKeyPressedMap(HanaLovesMeKey_UpArrow) && is_multiline) { if (io.KeyCtrl) SetWindowScrollY(draw_window, ImMax(draw_window->Scroll.y - g.FontSize, 0.0f)); else state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTSTART : STB_TEXTEDIT_K_UP) | k_mask); } else if (IsKeyPressedMap(HanaLovesMeKey_DownArrow) && is_multiline) { if (io.KeyCtrl) SetWindowScrollY(draw_window, ImMin(draw_window->Scroll.y + g.FontSize, GetScrollMaxY())); else state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTEND : STB_TEXTEDIT_K_DOWN) | k_mask); } else if (IsKeyPressedMap(HanaLovesMeKey_Home)) { state->OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTSTART | k_mask : STB_TEXTEDIT_K_LINESTART | k_mask); } else if (IsKeyPressedMap(HanaLovesMeKey_End)) { state->OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTEND | k_mask : STB_TEXTEDIT_K_LINEEND | k_mask); } else if (IsKeyPressedMap(HanaLovesMeKey_Delete) && !is_readonly) { state->OnKeyPressed(STB_TEXTEDIT_K_DELETE | k_mask); } else if (IsKeyPressedMap(HanaLovesMeKey_Backspace) && !is_readonly)
        {
            if (!state->HasSelection())
            {
                if (is_wordmove_key_down)
                    state->OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT | STB_TEXTEDIT_K_SHIFT);
                else if (is_osx && io.KeySuper && !io.KeyAlt && !io.KeyCtrl)
                    state->OnKeyPressed(STB_TEXTEDIT_K_LINESTART | STB_TEXTEDIT_K_SHIFT);
            }
            state->OnKeyPressed(STB_TEXTEDIT_K_BACKSPACE | k_mask);
        } else if (IsKeyPressedMap(HanaLovesMeKey_Enter))
        {
            bool ctrl_enter_for_new_line = (flags & HanaLovesMeInputTextFlags_CtrlEnterForNewLine) != 0;
            if (!is_multiline || (ctrl_enter_for_new_line && !io.KeyCtrl) || (!ctrl_enter_for_new_line && io.KeyCtrl))
            {
                enter_pressed = clear_active_id = true;
            } else if (!is_readonly)
            {
                unsigned int c = '\n'; // Insert new line
                if (InputTextFilterCharacter(&c, flags, callback, callback_user_data))
                    state->OnKeyPressed((int)c);
            }
        } else if (IsKeyPressedMap(HanaLovesMeKey_Escape))
        {
            clear_active_id = cancel_edit = true;
        } else if (is_undo || is_redo)
        {
            state->OnKeyPressed(is_undo ? STB_TEXTEDIT_K_UNDO : STB_TEXTEDIT_K_REDO);
            state->ClearSelection();
        } else if (is_shortcut_key && IsKeyPressedMap(HanaLovesMeKey_A))
        {
            state->SelectAll();
            state->CursorFollow = true;
        } else if (is_cut || is_copy)
        {
            // Cut, Copy
            if (io.SetClipboardTextFn)
            {
                const int ib = state->HasSelection() ? ImMin(state->Stb.select_start, state->Stb.select_end) : 0;
                const int ie = state->HasSelection() ? ImMax(state->Stb.select_start, state->Stb.select_end) : state->CurLenW;
                const int clipboard_data_len = ImTextCountUtf8BytesFromStr(state->TextW.Data + ib, state->TextW.Data + ie) + 1;
                char* clipboard_data = (char*)IM_ALLOC(clipboard_data_len * sizeof(char));
                ImTextStrToUtf8(clipboard_data, clipboard_data_len, state->TextW.Data + ib, state->TextW.Data + ie);
                SetClipboardText(clipboard_data);
                MemFree(clipboard_data);
            }
            if (is_cut)
            {
                if (!state->HasSelection())
                    state->SelectAll();
                state->CursorFollow = true;
                stb_textedit_cut(state, &state->Stb);
            }
        } else if (is_paste)
        {
            if (const char* clipboard = GetClipboardText())
            {
                // Filter pasted buffer
                const int clipboard_len = (int)strlen(clipboard);
                ImWchar* clipboard_filtered = (ImWchar*)IM_ALLOC((clipboard_len + 1) * sizeof(ImWchar));
                int clipboard_filtered_len = 0;
                for (const char* s = clipboard; *s; )
                {
                    unsigned int c;
                    s += ImTextCharFromUtf8(&c, s, NULL);
                    if (c == 0)
                        break;
                    if (c >= 0x10000 || !InputTextFilterCharacter(&c, flags, callback, callback_user_data))
                        continue;
                    clipboard_filtered[clipboard_filtered_len++] = (ImWchar)c;
                }
                clipboard_filtered[clipboard_filtered_len] = 0;
                if (clipboard_filtered_len > 0) // If everything was filtered, ignore the pasting operation
                {
                    stb_textedit_paste(state, &state->Stb, clipboard_filtered, clipboard_filtered_len);
                    state->CursorFollow = true;
                }
                MemFree(clipboard_filtered);
            }
        }

        // Update render selection flag after events have been handled, so selection highlight can be displayed during the same frame.
        render_selection |= state->HasSelection() && (RENDER_SELECTION_WHEN_INACTIVE || render_cursor);
    }

    // Process callbacks and apply result back to user's buffer.
    if (g.ActiveId == id)
    {
        IM_ASSERT(state != NULL);
        const char* apply_new_text = NULL;
        int apply_new_text_length = 0;
        if (cancel_edit)
        {
            // Restore initial value. Only return true if restoring to the initial value changes the current buffer contents.
            if (!is_readonly && strcmp(buf, state->InitialTextA.Data) != 0)
            {
                apply_new_text = state->InitialTextA.Data;
                apply_new_text_length = state->InitialTextA.Size - 1;
            }
        }

        // When using 'HanaLovesMeInputTextFlags_EnterReturnsTrue' as a special case we reapply the live buffer back to the input buffer before clearing ActiveId, even though strictly speaking it wasn't modified on this frame.
        // If we didn't do that, code like InputInt() with HanaLovesMeInputTextFlags_EnterReturnsTrue would fail. Also this allows the user to use InputText() with HanaLovesMeInputTextFlags_EnterReturnsTrue without maintaining any user-side stohnly.
        bool apply_edit_back_to_user_buffer = !cancel_edit || (enter_pressed && (flags & HanaLovesMeInputTextFlags_EnterReturnsTrue) != 0);
        if (apply_edit_back_to_user_buffer)
        {
            // Apply new value immediately - copy modified buffer back
            // Note that as soon as the input box is active, the in-widget value gets priority over any underlying modification of the input buffer
            // FIXME: We actually always render 'buf' when calling DrawList->AddText, making the comment above incorrect.
            // FIXME-OPT: CPU waste to do this every time the widget is active, should mark dirty state from the stb_textedit callbacks.
            if (!is_readonly)
            {
                state->TextAIsValid = true;
                state->TextA.resize(state->TextW.Size * 4 + 1);
                ImTextStrToUtf8(state->TextA.Data, state->TextA.Size, state->TextW.Data, NULL);
            }

            // User callback
            if ((flags & (HanaLovesMeInputTextFlags_CallbackCompletion | HanaLovesMeInputTextFlags_CallbackHistory | HanaLovesMeInputTextFlags_CallbackAlways)) != 0)
            {
                IM_ASSERT(callback != NULL);

                // The reason we specify the usage semantic (Completion/History) is that Completion needs to disable keyboard TABBING at the moment.
                HanaLovesMeInputTextFlags event_flag = 0;
                HanaLovesMeKey event_key = HanaLovesMeKey_COUNT;
                if ((flags & HanaLovesMeInputTextFlags_CallbackCompletion) != 0 && IsKeyPressedMap(HanaLovesMeKey_Tab))
                {
                    event_flag = HanaLovesMeInputTextFlags_CallbackCompletion;
                    event_key = HanaLovesMeKey_Tab;
                } else if ((flags & HanaLovesMeInputTextFlags_CallbackHistory) != 0 && IsKeyPressedMap(HanaLovesMeKey_UpArrow))
                {
                    event_flag = HanaLovesMeInputTextFlags_CallbackHistory;
                    event_key = HanaLovesMeKey_UpArrow;
                } else if ((flags & HanaLovesMeInputTextFlags_CallbackHistory) != 0 && IsKeyPressedMap(HanaLovesMeKey_DownArrow))
                {
                    event_flag = HanaLovesMeInputTextFlags_CallbackHistory;
                    event_key = HanaLovesMeKey_DownArrow;
                } else if (flags & HanaLovesMeInputTextFlags_CallbackAlways)
                    event_flag = HanaLovesMeInputTextFlags_CallbackAlways;

                if (event_flag)
                {
                    HanaLovesMeInputTextCallbackData callback_data;
                    memset(&callback_data, 0, sizeof(HanaLovesMeInputTextCallbackData));
                    callback_data.EventFlag = event_flag;
                    callback_data.Flags = flags;
                    callback_data.UserData = callback_user_data;

                    callback_data.EventKey = event_key;
                    callback_data.Buf = state->TextA.Data;
                    callback_data.BufTextLen = state->CurLenA;
                    callback_data.BufSize = state->BufCapacityA;
                    callback_data.BufDirty = false;

                    // We have to convert from wchar-positions to UTF-8-positions, which can be pretty slow (an incentive to ditch the ImWchar buffer, see https://github.com/nothings/stb/issues/188)
                    ImWchar* text = state->TextW.Data;
                    const int utf8_cursor_pos = callback_data.CursorPos = ImTextCountUtf8BytesFromStr(text, text + state->Stb.cursor);
                    const int utf8_selection_start = callback_data.SelectionStart = ImTextCountUtf8BytesFromStr(text, text + state->Stb.select_start);
                    const int utf8_selection_end = callback_data.SelectionEnd = ImTextCountUtf8BytesFromStr(text, text + state->Stb.select_end);

                    // Call user code
                    callback(&callback_data);

                    // Read back what user may have modified
                    IM_ASSERT(callback_data.Buf == state->TextA.Data);  // Invalid to modify those fields
                    IM_ASSERT(callback_data.BufSize == state->BufCapacityA);
                    IM_ASSERT(callback_data.Flags == flags);
                    if (callback_data.CursorPos != utf8_cursor_pos) { state->Stb.cursor = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.CursorPos); state->CursorFollow = true; }
                    if (callback_data.SelectionStart != utf8_selection_start) { state->Stb.select_start = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionStart); }
                    if (callback_data.SelectionEnd != utf8_selection_end) { state->Stb.select_end = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionEnd); }
                    if (callback_data.BufDirty)
                    {
                        IM_ASSERT(callback_data.BufTextLen == (int)strlen(callback_data.Buf)); // You need to maintain BufTextLen if you change the text!
                        if (callback_data.BufTextLen > backup_current_text_length && is_resizable)
                            state->TextW.resize(state->TextW.Size + (callback_data.BufTextLen - backup_current_text_length));
                        state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, state->TextW.Size, callback_data.Buf, NULL);
                        state->CurLenA = callback_data.BufTextLen;  // Assume correct length and valid UTF-8 from user, saves us an extra strlen()
                        state->CursorAnimReset();
                    }
                }
            }

            // Will copy result string if modified
            if (!is_readonly && strcmp(state->TextA.Data, buf) != 0)
            {
                apply_new_text = state->TextA.Data;
                apply_new_text_length = state->CurLenA;
            }
        }

        // Copy result to user buffer
        if (apply_new_text)
        {
            IM_ASSERT(apply_new_text_length >= 0);
            if (backup_current_text_length != apply_new_text_length && is_resizable)
            {
                HanaLovesMeInputTextCallbackData callback_data;
                callback_data.EventFlag = HanaLovesMeInputTextFlags_CallbackResize;
                callback_data.Flags = flags;
                callback_data.Buf = buf;
                callback_data.BufTextLen = apply_new_text_length;
                callback_data.BufSize = ImMax(buf_size, apply_new_text_length + 1);
                callback_data.UserData = callback_user_data;
                callback(&callback_data);
                buf = callback_data.Buf;
                buf_size = callback_data.BufSize;
                apply_new_text_length = ImMin(callback_data.BufTextLen, buf_size - 1);
                IM_ASSERT(apply_new_text_length <= buf_size);
            }

            // If the underlying buffer resize was denied or not carried to the next frame, apply_new_text_length+1 may be >= buf_size.
            ImStrncpy(buf, apply_new_text, ImMin(apply_new_text_length + 1, buf_size));
            value_changed = true;
        }

        // Clear temporary user stohnly
        state->UserFlags = 0;
        state->UserCallback = NULL;
        state->UserCallbackData = NULL;
    }

    // Release active ID at the end of the function (so e.g. pressing Return still does a final application of the value)
    if (clear_active_id && g.ActiveId == id)
        ClearActiveID();

    // Render frame
    if (!is_multiline)
    {
        RenderNavHighlight(frame_bb, id);

        auto borderszColorBlack = xuicolor(0, 0, 0, 255);
        auto borderszColor = xuicolor(50, 50, 50, 255);
        auto borderszColorMiddle = xuicolor(25, 25, 25, 255);
        /// inputboxstyle
        window->DrawList->AddRectFilled(frame_bb.Min - vsize(0, 4), frame_bb.Max, borderszColorMiddle, 0, false);
        window->DrawList->AddRect(frame_bb.Min - vsize(-2, 2), frame_bb.Max - vsize(2, 2), borderszColorBlack, 0, false, 1);
        window->DrawList->AddRect(frame_bb.Min - vsize(-1, 3), frame_bb.Max - vsize(1, 1), borderszColor, 0, false, 1);
        window->DrawList->AddRect(frame_bb.Min - vsize(0, 4), frame_bb.Max, borderszColorBlack, 0, false, 1);
    }

    const xuifloatcolor clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x, frame_bb.Min.y + size.y); // Not using frame_bb.Max because we have adjusted size
    vsize draw_pos = is_multiline ? draw_window->DC.CursorPos : frame_bb.Min + style.FramePadding;
    vsize text_size(0.0f, 0.0f);

    // Set upper limit of single-line InputTextEx() at 2 million characters strings. The current pathological worst case is a long line
    // without any carriage return, which would makes ImFont::RenderText() reserve too many vertices and probably crash. Avoid it altogether.
    // Note that we only use this limit on single-line InputText(), so a pathologically large line on a InputTextMultiline() would still crash.
    const int buf_display_max_length = 2 * 1024 * 1024;
    const char* buf_display = buf_display_from_state ? state->TextA.Data : buf; //-V595
    const char* buf_display_end = NULL; // We have specialized paths below for setting the length
    if (is_displaying_hint)
    {
        buf_display = hint;
        buf_display_end = hint + strlen(hint);
    }

    // Render text. We currently only render selection when the widget is active or while scrolling.
    // FIXME: We could remove the '&& render_cursor' to keep rendering selection when inactive.
    if (render_cursor || render_selection)
    {
        IM_ASSERT(state != NULL);
        if (!is_displaying_hint)
            buf_display_end = buf_display + state->CurLenA;

        // Render text (with cursor and selection)
        // This is going to be messy. We need to:
        // - Display the text (this alone can be more easily clipped)
        // - Handle scrolling, highlight selection, display cursor (those all requires some form of 1d->2d cursor position calculation)
        // - Measure text height (for scrollbar)
        // We are attempting to do most of that in **one main pass** to minimize the computation cost (non-negligible for large amount of text) + 2nd pass for selection rendering (we could merge them by an extra refactoring effort)
        // FIXME: This should occur on buf_display but we'd need to maintain cursor/select_start/select_end for UTF-8.
        const ImWchar * text_begin = state->TextW.Data;
        vsize cursor_offset, select_start_offset;

        {
            // Find lines numbers straddling 'cursor' (slot 0) and 'select_start' (slot 1) positions.
            const ImWchar* searches_input_ptr[2] = { NULL, NULL };
            int searches_result_line_no[2] = { -1000, -1000 };
            int searches_remaining = 0;
            if (render_cursor)
            {
                searches_input_ptr[0] = text_begin + state->Stb.cursor;
                searches_result_line_no[0] = -1;
                searches_remaining++;
            }
            if (render_selection)
            {
                searches_input_ptr[1] = text_begin + ImMin(state->Stb.select_start, state->Stb.select_end);
                searches_result_line_no[1] = -1;
                searches_remaining++;
            }

            // Iterate all lines to find our line numbers
            // In multi-line mode, we never exit the loop until all lines are counted, so add one extra to the searches_remaining counter.
            searches_remaining += is_multiline ? 1 : 0;
            int line_count = 0;
            //for (const ImWchar* s = text_begin; (s = (const ImWchar*)wcschr((const wchar_t*)s, (wchar_t)'\n')) != NULL; s++)  // FIXME-OPT: Could use this when wchar_t are 16-bits
            for (const ImWchar* s = text_begin; *s != 0; s++)
                if (*s == '\n')
                {
                    line_count++;
                    if (searches_result_line_no[0] == -1 && s >= searches_input_ptr[0]) { searches_result_line_no[0] = line_count; if (--searches_remaining <= 0) break; }
                    if (searches_result_line_no[1] == -1 && s >= searches_input_ptr[1]) { searches_result_line_no[1] = line_count; if (--searches_remaining <= 0) break; }
                }
            line_count++;
            if (searches_result_line_no[0] == -1)
                searches_result_line_no[0] = line_count;
            if (searches_result_line_no[1] == -1)
                searches_result_line_no[1] = line_count;

            // Calculate 2d position by finding the beginning of the line and measuring distance
            cursor_offset.x = InputTextCalcTextSizeW(ImStrbolW(searches_input_ptr[0], text_begin), searches_input_ptr[0]).x;
            cursor_offset.y = searches_result_line_no[0] * g.FontSize;
            if (searches_result_line_no[1] >= 0)
            {
                select_start_offset.x = InputTextCalcTextSizeW(ImStrbolW(searches_input_ptr[1], text_begin), searches_input_ptr[1]).x;
                select_start_offset.y = searches_result_line_no[1] * g.FontSize;
            }

            // Store text height (note that we haven't calculated text width at all, see GitHub issues #383, #1224)
            if (is_multiline)
                text_size = vsize(size.x, line_count * g.FontSize);
        }

        // Scroll
        if (render_cursor && state->CursorFollow)
        {
            // Horizontal scroll in chunks of quarter width
            if (!(flags & HanaLovesMeInputTextFlags_NoHorizontalScroll))
            {
                const float scroll_increment_x = size.x * 0.25f;
                if (cursor_offset.x < state->ScrollX)
                    state->ScrollX = (float)(int)ImMax(0.0f, cursor_offset.x - scroll_increment_x);
                else if (cursor_offset.x - size.x >= state->ScrollX)
                    state->ScrollX = (float)(int)(cursor_offset.x - size.x + scroll_increment_x);
            } else
            {
                state->ScrollX = 0.0f;
            }

            // Vertical scroll
            if (is_multiline)
            {
                float scroll_y = draw_window->Scroll.y;
                if (cursor_offset.y - g.FontSize < scroll_y)
                    scroll_y = ImMax(0.0f, cursor_offset.y - g.FontSize);
                else if (cursor_offset.y - size.y >= scroll_y)
                    scroll_y = cursor_offset.y - size.y;
                draw_window->DC.CursorPos.y += (draw_window->Scroll.y - scroll_y);   // Manipulate cursor pos immediately avoid a frame of lag
                draw_window->Scroll.y = scroll_y;
                draw_pos.y = draw_window->DC.CursorPos.y;
            }

            state->CursorFollow = false;
        }

        // Draw selection
        const vsize draw_scroll = vsize(state->ScrollX, 0.0f);
        if (render_selection)
        {
            const ImWchar* text_selected_begin = text_begin + ImMin(state->Stb.select_start, state->Stb.select_end);
            const ImWchar* text_selected_end = text_begin + ImMax(state->Stb.select_start, state->Stb.select_end);

            ImU32 bg_color = GetColorU32(HanaLovesMeCol_TextSelectedBg, render_cursor ? 1.0f : 0.6f); // FIXME: current code flow mandate that render_cursor is always true here, we are leaving the transparent one for tests.
            float bg_offy_up = is_multiline ? 0.0f : -1.0f;    // FIXME: those offsets should be part of the style? they don't play so well with multi-line selection.
            float bg_offy_dn = is_multiline ? 0.0f : 2.0f;
            vsize rect_pos = draw_pos + select_start_offset - draw_scroll;
            for (const ImWchar* p = text_selected_begin; p < text_selected_end; )
            {
                if (rect_pos.y > clip_rect.w + g.FontSize)
                    break;
                if (rect_pos.y < clip_rect.y)
                {
                    //p = (const ImWchar*)wmemchr((const wchar_t*)p, '\n', text_selected_end - p);  // FIXME-OPT: Could use this when wchar_t are 16-bits
                    //p = p ? p + 1 : text_selected_end;
                    while (p < text_selected_end)
                        if (*p++ == '\n')
                            break;
                } else
                {
                    vsize rect_size = InputTextCalcTextSizeW(p, text_selected_end, &p, NULL, true);
                    if (rect_size.x <= 0.0f) rect_size.x = (float)(int)(g.Font->GetCharAdvance((ImWchar)' ') * 0.50f); // So we can see selected empty lines
                    vsrect rect(rect_pos + vsize(0.0f, bg_offy_up - g.FontSize), rect_pos + vsize(rect_size.x, bg_offy_dn));
                    rect.ClipWith(clip_rect);
                    if (rect.Overlaps(clip_rect))
                        draw_window->DrawList->AddRectFilled(rect.Min, rect.Max, bg_color);
                }
                rect_pos.x = draw_pos.x - draw_scroll.x;
                rect_pos.y += g.FontSize;
            }
        }

        // We test for 'buf_display_max_length' as a way to avoid some pathological cases (e.g. single-line 1 MB string) which would make ImDrawList crash.
        if (is_multiline || (buf_display_end - buf_display) < buf_display_max_length)
        {
            ImU32 col = GetColorU32(is_displaying_hint ? HanaLovesMeCol_TextDisabled : HanaLovesMeCol_Text);
            draw_window->DrawList->AddText(g.Font, g.FontSize, (draw_pos - vsize(0, 3)) - draw_scroll, col, buf_display, buf_display_end, 0.0f, is_multiline ? NULL : &clip_rect); /// textipozicija
        }

        // Draw blinking cursor
        if (render_cursor)
        {
            ImU32 colz = GetColorU32(is_displaying_hint ? HanaLovesMeCol_TextDisabled : HanaLovesMeCol_Text);

            state->CursorAnim += io.DeltaTime;
            bool cursor_is_visible = (!g.IO.ConfigInputTextCursorBlink) || (state->CursorAnim <= 0.0f) || ImFmod(state->CursorAnim, 1.20f) <= 0.80f;
            vsize cursor_screen_pos = draw_pos + cursor_offset - draw_scroll;
            vsrect cursor_screen_rect(cursor_screen_pos.x, cursor_screen_pos.y - g.FontSize + 0.5f, cursor_screen_pos.x + 1.0f, cursor_screen_pos.y - 1.5f);
            if (cursor_is_visible && cursor_screen_rect.Overlaps(clip_rect))
             draw_window->DrawList->AddLine(cursor_screen_rect.Min, cursor_screen_rect.GetBL() - vsize(0, 3), GetColorU32(HanaLovesMeCol_Text)); /// donjacrtaovde
             // Notify OS of text input position for advanced IME (-1 x offset so that Windows IME can cover our cursor. Bit of an extra nicety.)
            if (!is_readonly)
                g.PlatformImePos = vsize(cursor_screen_pos.x - 1.0f, cursor_screen_pos.y - g.FontSize);
        }
    } else
    {
        // Render text only (no selection, no cursor)
        if (is_multiline)
            text_size = vsize(size.x, InputTextCalcTextLenAndLineCount(buf_display, &buf_display_end) * g.FontSize); // We don't need width
        else if (!is_displaying_hint && g.ActiveId == id)
            buf_display_end = buf_display + state->CurLenA;
        else if (!is_displaying_hint)
            buf_display_end = buf_display + strlen(buf_display);

        if (is_multiline || (buf_display_end - buf_display) < buf_display_max_length)
        {
            ImU32 col = GetColorU32(is_displaying_hint ? HanaLovesMeCol_TextDisabled : HanaLovesMeCol_Text);
            draw_window->DrawList->AddText(g.Font, g.FontSize, draw_pos - vsize(0,3), col, buf_display, buf_display_end, 0.0f, is_multiline ? NULL : &clip_rect);
        }
    }

    if (is_multiline)
    {
        Dummy(text_size + vsize(0.0f, g.FontSize)); // Always add room to scroll an extra line
        EndChildFrame();
        EndGroup();
    }

    if (is_password && !is_displaying_hint)
        PopFont();

    // Log as text
    if (g.LogEnabled && !(is_password && !is_displaying_hint))
        LogRenderedText(&draw_pos, buf_display, buf_display_end);

    if (label_size.x > 0)
        RenderText(vsize(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    if (value_changed)
        MarkItemEdited(id);

    HanaLovesMe_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
    if ((flags & HanaLovesMeInputTextFlags_EnterReturnsTrue) != 0)
        return enter_pressed;
    else
        return value_changed;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: ColorEdit, ColorPicker, ColorButton, etc.
//-------------------------------------------------------------------------
// - ColorEdit3()
// - ColorEdit4()
// - ColorPicker3()
// - RenderColorRectWithAlphaCheckerboard() [Internal]
// - ColorPicker4()
// - ColorButton()
// - SetColorEditOptions()
// - ColorTooltip() [Internal]
// - ColorEditOptionsPopup() [Internal]
// - ColorPickerOptionsPopup() [Internal]
//-------------------------------------------------------------------------

bool HanaLovesMe::ColorEdit3(const char* label, float col[3], HanaLovesMeColorEditFlags flags)
{
    return ColorEdit4(label, col, flags | HanaLovesMeColorEditFlags_NoAlpha);
}

// Edit colors components (each component in 0.0f..1.0f range).
// See enum HanaLovesMeColorEditFlags_ for available options. e.g. Only access 3 floats if HanaLovesMeColorEditFlags_NoAlpha flag is set.
// With typical options: Left-click on colored square to open color picker. Right-click to open option menu. CTRL-Click over input fields to edit them and TAB to go to next item.

bool HanaLovesMe::ColorEdit4(const char* label, float col[4], HanaLovesMeColorEditFlags flags) {

    HanaLovesMeWindow* window = GetCurrentWindow();

    if (window->SkipItems)
        return false;

    HanaLovesMeContext& g = *GHanaLovesMe;
    const HanaLovesMeStyle& style = g.Style;
    const float square_sz = GetFrameHeight();
    const float w_extra = (flags & HanaLovesMeColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
    const float w_items_all = CalcItemWidth() - w_extra;
    const char* label_display_end = FindRenderedTextEnd(label);

    BeginGroup();
    PushID(label);

    const HanaLovesMeColorEditFlags flags_untouched = flags;

    if (flags & HanaLovesMeColorEditFlags_NoInputs)
        flags = (flags & (~HanaLovesMeColorEditFlags__DisplayMask)) | HanaLovesMeColorEditFlags_DisplayRGB | HanaLovesMeColorEditFlags_NoOptions;

    if (!(flags & HanaLovesMeColorEditFlags_NoOptions))
        ColorEditOptionsPopup(col, flags);

    if (!(flags & HanaLovesMeColorEditFlags__DisplayMask))
        flags |= (g.ColorEditOptions & HanaLovesMeColorEditFlags__DisplayMask);

    if (!(flags & HanaLovesMeColorEditFlags__DataTypeMask))
        flags |= (g.ColorEditOptions & HanaLovesMeColorEditFlags__DataTypeMask);

    if (!(flags & HanaLovesMeColorEditFlags__PickerMask))
        flags |= (g.ColorEditOptions & HanaLovesMeColorEditFlags__PickerMask);

    if (!(flags & HanaLovesMeColorEditFlags__InputMask))
        flags |= (g.ColorEditOptions & HanaLovesMeColorEditFlags__InputMask);

    flags |= (g.ColorEditOptions & ~(HanaLovesMeColorEditFlags__DisplayMask | HanaLovesMeColorEditFlags__DataTypeMask | HanaLovesMeColorEditFlags__PickerMask | HanaLovesMeColorEditFlags__InputMask));

    IM_ASSERT(ImIsPowerOfTwo(flags & HanaLovesMeColorEditFlags__DisplayMask)); // Check that only 1 is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & HanaLovesMeColorEditFlags__InputMask));   // Check that only 1 is selected

    const bool alpha = (flags & HanaLovesMeColorEditFlags_NoAlpha) == 0;
    const bool hdr = (flags & HanaLovesMeColorEditFlags_HDR) != 0;
    const int components = alpha ? 4 : 3;

    float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };

    if ((flags & HanaLovesMeColorEditFlags_InputHSV) && (flags & HanaLovesMeColorEditFlags_DisplayRGB))
        ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
    else if ((flags & HanaLovesMeColorEditFlags_InputRGB) && (flags & HanaLovesMeColorEditFlags_DisplayHSV))
        ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

    int i[4] = { IM_F32_TO_INT8_UNBOUND(f[0]), IM_F32_TO_INT8_UNBOUND(f[1]), IM_F32_TO_INT8_UNBOUND(f[2]), IM_F32_TO_INT8_UNBOUND(f[3]) };

    bool value_changed = false;
    bool value_changed_as_float = false;

    if ((flags & (HanaLovesMeColorEditFlags_DisplayRGB | HanaLovesMeColorEditFlags_DisplayHSV)) != 0 && (flags & HanaLovesMeColorEditFlags_NoInputs) == 0) {

        const float w_item_one = ImMax(1.0f, (float)(int)((w_items_all - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
        const float w_item_last = ImMax(1.0f, (float)(int)(w_items_all - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));

        const bool hide_prefix = (w_item_one <= CalcTextSize((flags & HanaLovesMeColorEditFlags_Float) ? "M:0.000" : "M:000").x);

        static const char* ids[4] = { "##X", "##Y", "##Z", "##W" };

        static const char* fmt_table_int[3][4] = {

            {   "%3d",   "%3d",   "%3d",   "%3d" }, // Short display
            { "R:%3d", "G:%3d", "B:%3d", "A:%3d" }, // Long display for RGBA
            { "H:%3d", "S:%3d", "V:%3d", "A:%3d" }  // Long display for HSVA
        };

        static const char* fmt_table_float[3][4] = {

            {   "%0.3f",   "%0.3f",   "%0.3f",   "%0.3f" }, // Short display
            { "R:%0.3f", "G:%0.3f", "B:%0.3f", "A:%0.3f" }, // Long display for RGBA
            { "H:%0.3f", "S:%0.3f", "V:%0.3f", "A:%0.3f" }  // Long display for HSVA
        };

        const int fmt_idx = hide_prefix ? 0 : (flags & HanaLovesMeColorEditFlags_DisplayHSV) ? 2 : 1;

        PushItemWidth(w_item_one);

        for (int n = 0; n < components; n++) {

            if (n > 0)
                SameLine(0, style.ItemInnerSpacing.x);

            if (n + 1 == components)
                PushItemWidth(w_item_last);

            if (flags & HanaLovesMeColorEditFlags_Float) {

                value_changed |= DragFloat(ids[n], &f[n], 1.0f / 255.0f, 0.0f, hdr ? 0.0f : 1.0f, fmt_table_float[fmt_idx][n]);
                value_changed_as_float |= value_changed;
            }
			else
                value_changed |= DragInt(ids[n], &i[n], 1.0f, 0, hdr ? 0 : 255, fmt_table_int[fmt_idx][n]);

			if (!(flags & HanaLovesMeColorEditFlags_NoOptions))
                OpenPopupOnItemClick("context");
        }

        PopItemWidth();
        PopItemWidth();

    }
	else if ((flags & HanaLovesMeColorEditFlags_DisplayHex) != 0 && (flags & HanaLovesMeColorEditFlags_NoInputs) == 0) {

        char buf[64];

        if (alpha)
            ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255), ImClamp(i[3], 0, 255));
        else
            ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255));

        PushItemWidth(w_items_all);

        if (InputText("##Text", buf, IM_ARRAYSIZE(buf), HanaLovesMeInputTextFlags_CharsHexadecimal | HanaLovesMeInputTextFlags_CharsUppercase)) {

            value_changed = true;
            char* p = buf;

            while (*p == '#' || ImCharIsBlankA(*p))
                p++;

            i[0] = i[1] = i[2] = i[3] = 0;

            if (alpha)
                sscanf(p, "%02X%02X%02X%02X", (unsigned int*)& i[0], (unsigned int*)& i[1], (unsigned int*)& i[2], (unsigned int*)& i[3]); // Treat at unsigned (%X is unsigned)
            else
                sscanf(p, "%02X%02X%02X", (unsigned int*)& i[0], (unsigned int*)& i[1], (unsigned int*)& i[2]);
        }

        if (!(flags & HanaLovesMeColorEditFlags_NoOptions))
            OpenPopupOnItemClick("context");

        PopItemWidth();
    }

    HanaLovesMeWindow* picker_active_window = NULL;

    if (!(flags & HanaLovesMeColorEditFlags_NoSmallPreview)) {

        if (!(flags & HanaLovesMeColorEditFlags_NoInputs))
            SameLine(0, style.ItemInnerSpacing.x);

        const xuifloatcolor col_v4(col[0], col[1], col[2], alpha ? col[3] : 1.0f);

        if (ColorButton("##ColorButton", col_v4, flags)) {

            if (!(flags & HanaLovesMeColorEditFlags_NoPicker)) {

                g.ColorPickerRef = col_v4;
                OpenPopup("picker");
                SetNextWindowPos(window->DC.LastItemRect.GetBL() + vsize(-1, style.ItemSpacing.y));
            }
        }

        if (!(flags & HanaLovesMeColorEditFlags_NoOptions))
            OpenPopupOnItemClick("context");

        if (BeginPopup("picker")) {

            picker_active_window = g.CurrentWindow;

            if (label != label_display_end) {

                TextEx(label, label_display_end);
                Spacing();
            }

            HanaLovesMeColorEditFlags picker_flags_to_forward = HanaLovesMeColorEditFlags__DataTypeMask | HanaLovesMeColorEditFlags__PickerMask | HanaLovesMeColorEditFlags__InputMask | HanaLovesMeColorEditFlags_HDR | HanaLovesMeColorEditFlags_NoAlpha | HanaLovesMeColorEditFlags_AlphaBar;
            HanaLovesMeColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | HanaLovesMeColorEditFlags_DisplayHSV | HanaLovesMeColorEditFlags_NoLabel | HanaLovesMeColorEditFlags_AlphaPreviewHalf;

            PushItemWidth(square_sz * 12.0f); // Use 256 + bar sizes?
            value_changed |= ColorPicker4("##picker", col, picker_flags, &g.ColorPickerRef.x);
            PopItemWidth();

            EndPopup();
        }
    }

    if (label != label_display_end && !(flags & HanaLovesMeColorEditFlags_NoLabel)) {

        SameLine(0, style.ItemInnerSpacing.x);
        TextEx(label, label_display_end);
    }

    if (value_changed && picker_active_window == NULL) {

		if (!value_changed_as_float) {

			for (int n = 0; n < 4; n++)
				f[n] = i[n] / 255.0f;
		}
            
        if ((flags & HanaLovesMeColorEditFlags_DisplayHSV) && (flags & HanaLovesMeColorEditFlags_InputRGB))
            ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
        if ((flags & HanaLovesMeColorEditFlags_DisplayRGB) && (flags & HanaLovesMeColorEditFlags_InputHSV))
            ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

        col[0] = f[0];
        col[1] = f[1];
        col[2] = f[2];

        if (alpha)
            col[3] = f[3];
    }

    PopID();
    EndGroup();

    if ((window->DC.LastItemStatusFlags & HanaLovesMeItemStatusFlags_HoveredRect) && !(flags & HanaLovesMeColorEditFlags_NoDragDrop) && BeginDragDropTarget()) {

        bool accepted_drag_drop = false;

        if (const HanaLovesMePayload * payload = AcceptDragDropPayload(HanaLovesMe_PAYLOAD_TYPE_COLOR_3F)) {

            memcpy((float*)col, payload->Data, sizeof(float) * 3); // Preserve alpha if any //-V512
            value_changed = accepted_drag_drop = true;
        }

        if (const HanaLovesMePayload * payload = AcceptDragDropPayload(HanaLovesMe_PAYLOAD_TYPE_COLOR_4F)) {

            memcpy((float*)col, payload->Data, sizeof(float) * components);
            value_changed = accepted_drag_drop = true;
        }

        if (accepted_drag_drop && (flags & HanaLovesMeColorEditFlags_InputHSV))
            ColorConvertRGBtoHSV(col[0], col[1], col[2], col[0], col[1], col[2]);

        EndDragDropTarget();
    }

    if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window)
        window->DC.LastItemId = g.ActiveId;

    if (value_changed)
        MarkItemEdited(window->DC.LastItemId);

    return value_changed;
}

bool HanaLovesMe::ColorPicker3(const char* label, float col[3], HanaLovesMeColorEditFlags flags)
{
    float col4[4] = { col[0], col[1], col[2], 1.0f };
    if (!ColorPicker4(label, col4, flags | HanaLovesMeColorEditFlags_NoAlpha))
        return false;
    col[0] = col4[0]; col[1] = col4[1]; col[2] = col4[2];
    return true;
}

static inline ImU32 ImAlphaBlendColor(ImU32 col_a, ImU32 col_b)
{
    float t = ((col_b >> IM_COL32_A_SHIFT) & 0xFF) / 255.f;
    int r = ImLerp((int)(col_a >> IM_COL32_R_SHIFT) & 0xFF, (int)(col_b >> IM_COL32_R_SHIFT) & 0xFF, t);
    int g = ImLerp((int)(col_a >> IM_COL32_G_SHIFT) & 0xFF, (int)(col_b >> IM_COL32_G_SHIFT) & 0xFF, t);
    int b = ImLerp((int)(col_a >> IM_COL32_B_SHIFT) & 0xFF, (int)(col_b >> IM_COL32_B_SHIFT) & 0xFF, t);
    return IM_COL32(r, g, b, 0xFF);
}

// Helper for ColorPicker4()
// NB: This is rather brittle and will show artifact when rounding this enabled if rounded corners overlap multiple cells. Caller currently responsible for avoiding that.
// I spent a non reasonable amount of time trying to getting this right for ColorButton with rounding+anti-aliasing+HanaLovesMeColorEditFlags_HalfAlphaPreview flag + various grid sizes and offsets, and eventually gave up... probably more reasonable to disable rounding alltogether.
void HanaLovesMe::RenderColorRectWithAlphaCheckerboard(vsize p_min, vsize p_max, ImU32 col, float grid_step, vsize grid_off, float rounding, int rounding_corners_flags)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (((col & IM_COL32_A_MASK) >> IM_COL32_A_SHIFT) < 0xFF)
    {
        ImU32 col_bg1 = GetColorU32(ImAlphaBlendColor(IM_COL32(204, 204, 204, 255), col));
        ImU32 col_bg2 = GetColorU32(ImAlphaBlendColor(IM_COL32(128, 128, 128, 255), col));
        window->DrawList->AddRectFilled(p_min, p_max, col_bg1, rounding, rounding_corners_flags);

        int yi = 0;
        for (float y = p_min.y + grid_off.y; y < p_max.y; y += grid_step, yi++)
        {
            float y1 = ImClamp(y, p_min.y, p_max.y), y2 = ImMin(y + grid_step, p_max.y);
            if (y2 <= y1)
                continue;
            for (float x = p_min.x + grid_off.x + (yi & 1) * grid_step; x < p_max.x; x += grid_step * 2.0f)
            {
                float x1 = ImClamp(x, p_min.x, p_max.x), x2 = ImMin(x + grid_step, p_max.x);
                if (x2 <= x1)
                    continue;
                int rounding_corners_flags_cell = 0;
                if (y1 <= p_min.y) { if (x1 <= p_min.x) rounding_corners_flags_cell |= ImDrawCornerFlags_TopLeft; if (x2 >= p_max.x) rounding_corners_flags_cell |= ImDrawCornerFlags_TopRight; }
                if (y2 >= p_max.y) { if (x1 <= p_min.x) rounding_corners_flags_cell |= ImDrawCornerFlags_BotLeft; if (x2 >= p_max.x) rounding_corners_flags_cell |= ImDrawCornerFlags_BotRight; }
                rounding_corners_flags_cell &= rounding_corners_flags;
                window->DrawList->AddRectFilled(vsize(x1, y1), vsize(x2, y2), col_bg2, rounding_corners_flags_cell ? rounding : 0.0f, rounding_corners_flags_cell);
            }
        }
    } else
    {
        window->DrawList->AddRectFilled(p_min, p_max, col, rounding, rounding_corners_flags);
    }
}

// Helper for ColorPicker4()
static void RenderArrowsForVerticalBar(ImDrawList * draw_list, vsize pos, vsize half_sz, float bar_w)
{
    HanaLovesMe::RenderArrowPointingAt(draw_list, vsize(pos.x + half_sz.x + 1, pos.y), vsize(half_sz.x + 2, half_sz.y + 1), HanaLovesMeDir_Right, IM_COL32_BLACK);
    HanaLovesMe::RenderArrowPointingAt(draw_list, vsize(pos.x + half_sz.x, pos.y), half_sz, HanaLovesMeDir_Right, IM_COL32_WHITE);
    HanaLovesMe::RenderArrowPointingAt(draw_list, vsize(pos.x + bar_w - half_sz.x - 1, pos.y), vsize(half_sz.x + 2, half_sz.y + 1), HanaLovesMeDir_Left, IM_COL32_BLACK);
    HanaLovesMe::RenderArrowPointingAt(draw_list, vsize(pos.x + bar_w - half_sz.x, pos.y), half_sz, HanaLovesMeDir_Left, IM_COL32_WHITE);
}

static inline float ColorSquareSize2() {

	HanaLovesMeContext& g = *GHanaLovesMe;
	return g.FontSize + g.Style.FramePadding.y * 2.0f;
}

static void ColorPickerOptionsPopup2(HanaLovesMeColorEditFlags flags, float* ref_col) {

	bool allow_opt_picker = !(flags & HanaLovesMeColorEditFlags__PickerMask);
	bool allow_opt_alpha_bar = !(flags & HanaLovesMeColorEditFlags_NoAlpha) && !(flags & HanaLovesMeColorEditFlags_AlphaBar);

	if ((!allow_opt_picker && !allow_opt_alpha_bar) || !HanaLovesMe::BeginPopup("context"))
		return;

	HanaLovesMeContext& g = *GHanaLovesMe;

	if (allow_opt_picker) {

		vsize picker_size(g.FontSize * 8, ImMax(g.FontSize * 8 - (ColorSquareSize2() + g.Style.ItemInnerSpacing.x), 1.0f)); // FIXME: Picker size copied from main picker function
		HanaLovesMe::PushItemWidth(picker_size.x);

		for (int picker_type = 0; picker_type < 2; picker_type++) {

			if (picker_type > 0)
				HanaLovesMe::Separator();

			HanaLovesMe::PushID(picker_type);
			HanaLovesMeColorEditFlags picker_flags = HanaLovesMeColorEditFlags_NoInputs | HanaLovesMeColorEditFlags_NoOptions | HanaLovesMeColorEditFlags_NoLabel | HanaLovesMeColorEditFlags_NoSidePreview | (flags & HanaLovesMeColorEditFlags_NoAlpha);

			if (picker_type == 0)
				picker_flags |= HanaLovesMeColorEditFlags_PickerHueBar;

			if (picker_type == 1)
				picker_flags |= HanaLovesMeColorEditFlags_PickerHueWheel;

			vsize backup_pos = HanaLovesMe::GetCursorScreenPos();

			if (HanaLovesMe::Selectable("##selectable", false, 0, picker_size)) // By default, Selectable() is closing popup
				g.ColorEditOptions = (g.ColorEditOptions & ~HanaLovesMeColorEditFlags__PickerMask) | (picker_flags & HanaLovesMeColorEditFlags__PickerMask);

			HanaLovesMe::SetCursorScreenPos(backup_pos);
			xuifloatcolor dummy_ref_col;
			memcpy(&dummy_ref_col.x, ref_col, sizeof(float) * (picker_flags & HanaLovesMeColorEditFlags_NoAlpha ? 3 : 4));
			HanaLovesMe::ColorPicker4("##dummypicker", &dummy_ref_col.x, picker_flags);
			HanaLovesMe::PopID();
		}
		HanaLovesMe::PopItemWidth();
	}

	if (allow_opt_alpha_bar) {

		if (allow_opt_picker)
			HanaLovesMe::Separator();

		HanaLovesMe::CheckboxFlags("Alpha Bar", (unsigned int*)&g.ColorEditOptions, HanaLovesMeColorEditFlags_AlphaBar);
	}

	HanaLovesMe::EndPopup();
}

// Note: ColorPicker4() only accesses 3 floats if HanaLovesMeColorEditFlags_NoAlpha flag is set.
// (In C++ the 'float col[4]' notation for a function argument is equivalent to 'float* col', we only specify a size to facilitate understanding of the code.)
// FIXME: we adjust the big color square height based on item width, which may cause a flickering feedback loop (if automatic height makes a vertical scrollbar appears, affecting automatic width..)
bool HanaLovesMe::ColorPicker4(const char* label, float col[4], HanaLovesMeColorEditFlags flags, const float* ref_col)
{
	HanaLovesMeContext& g = *GHanaLovesMe;
	HanaLovesMeWindow* window = GetCurrentWindow();
	ImDrawList* draw_list = window->DrawList;
	HanaLovesMeStyle& style = g.Style;
	HanaLovesMeIO& io = g.IO;

	PushID(label);
	BeginGroup();

	if (!(flags & HanaLovesMeColorEditFlags_NoSidePreview))
		flags |= HanaLovesMeColorEditFlags_NoSmallPreview;

	if (!(flags & HanaLovesMeColorEditFlags_NoOptions))
		ColorPickerOptionsPopup2(flags, col);

	if (!(flags & HanaLovesMeColorEditFlags__PickerMask))
		flags |= ((g.ColorEditOptions & HanaLovesMeColorEditFlags__PickerMask) ? g.ColorEditOptions : HanaLovesMeColorEditFlags__OptionsDefault) & HanaLovesMeColorEditFlags__PickerMask;

	IM_ASSERT(ImIsPowerOfTwo((int)(flags & HanaLovesMeColorEditFlags__PickerMask))); // Check that only 1 is selected

	if (!(flags & HanaLovesMeColorEditFlags_NoOptions))
		flags |= (g.ColorEditOptions & HanaLovesMeColorEditFlags_AlphaBar);

	// Setup
	bool alpha_bar;

	if (flags & HanaLovesMeColorEditFlags_AlphaBar)
		alpha_bar = true;
	else if (flags & HanaLovesMeColorEditFlags_NoAlpha)
		alpha_bar = false;

	vsize picker_pos = window->DC.CursorPos + vsize(1, 1);
	float bars_width = 12; // Arbitrary smallish width of Hue/Alpha picking bars
	float sv_picker_size = 152; // ImMax( bars_width * 1, CalcItemWidth( ) - ( alpha_bar ? 2 : 1 ) * ( bars_width + style.ItemInnerSpacing.x ) ); // Saturation/Value picking box
	float bar0_pos_x = picker_pos.x + sv_picker_size + style.ItemInnerSpacing.x;
	float bar1_pos_x = bar0_pos_x + bars_width + style.ItemInnerSpacing.x;
	float bars_triangles_half_sz = (float)(int)(bars_width * 0.20f);

	float wheel_thickness = sv_picker_size * 0.08f;
	float wheel_r_outer = sv_picker_size * 0.50f;
	float wheel_r_inner = wheel_r_outer - wheel_thickness;
	vsize wheel_center(picker_pos.x + (sv_picker_size + bars_width)*0.5f, picker_pos.y + sv_picker_size * 0.5f);

	// Note: the triangle is displayed rotated with triangle_pa pointing to Hue, but most coordinates stays unrotated for logic.
	float triangle_r = wheel_r_inner - (int)(sv_picker_size * 0.027f);
	vsize triangle_pa = vsize(triangle_r, 0.0f); // Hue point.
	vsize triangle_pb = vsize(triangle_r * -0.5f, triangle_r * -0.866025f); // Black point.
	vsize triangle_pc = vsize(triangle_r * -0.5f, triangle_r * +0.866025f); // White point.

	float H, S, V;
	ColorConvertRGBtoHSV(col[0], col[1], col[2], H, S, V);

	bool value_changed = false, value_changed_h = false, value_changed_sv = false;

	// SV rectangle logic
	InvisibleButton("sv", vsize(sv_picker_size, sv_picker_size));
	if (IsItemActive()) {
		S = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size - 1));
		V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
		value_changed = value_changed_sv = true;
	}

	// Hue bar logic
	SetCursorScreenPos(vsize(bar0_pos_x, picker_pos.y));
	InvisibleButton("hue", vsize(bars_width, sv_picker_size));
	if (IsItemActive()) {
		H = ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
		value_changed = value_changed_h = true;
	}

	if (!(flags & HanaLovesMeColorEditFlags_NoLabel)) {
		const char* label_display_end = FindRenderedTextEnd(label);
		if (label != label_display_end) {
			if ((flags & HanaLovesMeColorEditFlags_NoSidePreview))
				SameLine(0, style.ItemInnerSpacing.x);
		}
	}

	// Convert back color to RGB
	if (value_changed_h || value_changed_sv)
		ColorConvertHSVtoRGB(H >= 1.0f ? H - 10 * 1e-6f : H, S > 0.0f ? S : 10 * 1e-6f, V > 0.0f ? V : 1e-6f, col[0], col[1], col[2]);

	// Try to cancel hue wrap (after ColorEdit), if any
	if (value_changed) {
		float new_H, new_S, new_V;
		ColorConvertRGBtoHSV(col[0], col[1], col[2], new_H, new_S, new_V);
		if (new_H <= 0 && H > 0) {
			if (new_V <= 0 && V != new_V)
				ColorConvertHSVtoRGB(H, S, new_V <= 0 ? V * 0.5f : new_V, col[0], col[1], col[2]);
			else if (new_S <= 0)
				ColorConvertHSVtoRGB(H, new_S <= 0 ? S * 0.5f : new_S, new_V, col[0], col[1], col[2]);
		}
	}

	xuifloatcolor hue_color_f(1, 1, 1, 1); ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
	ImU32 hue_color32 = ColorConvertFloat4ToU32(hue_color_f);
	ImU32 col32_no_alpha = ColorConvertFloat4ToU32(xuifloatcolor(col[0], col[1], col[2], 1.0f));

	const ImU32 hue_colors[6 + 1] = { IM_COL32(255,0,0,255), IM_COL32(255,255,0,255), IM_COL32(0,255,0,255), IM_COL32(0,255,255,255), IM_COL32(0,0,255,255), IM_COL32(255,0,255,255), IM_COL32(255,0,0,255) };
	vsize sv_cursor_pos;

	if (flags & HanaLovesMeColorEditFlags_PickerHueBar) {
		// Alpha bar logic
		if (alpha_bar) {
			SetCursorScreenPos(vsize(bar1_pos_x, picker_pos.y));
			InvisibleButton("alpha", vsize(bars_width, sv_picker_size));
			if (IsItemActive()) {
				col[3] = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
				value_changed = true;
			}
		}

		// Render SV Square
		draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + vsize(sv_picker_size, sv_picker_size), IM_COL32_WHITE, hue_color32, hue_color32, IM_COL32_WHITE);
		draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + vsize(sv_picker_size, sv_picker_size), IM_COL32_BLACK_TRANS, IM_COL32_BLACK_TRANS, IM_COL32_BLACK, IM_COL32_BLACK);
		draw_list->AddRect(picker_pos, picker_pos + vsize(sv_picker_size, sv_picker_size), IM_COL32_BLACK);

		sv_cursor_pos.x = ImClamp((float)(int)(picker_pos.x + ImSaturate(S) * sv_picker_size + 0.5f), picker_pos.x + 2, picker_pos.x + sv_picker_size - 2); // Sneakily prevent the circle to stick out too much
		sv_cursor_pos.y = ImClamp((float)(int)(picker_pos.y + ImSaturate(1 - V) * sv_picker_size + 0.5f), picker_pos.y + 2, picker_pos.y + sv_picker_size - 2);

		// Render Hue Bar
		for (int i = 0; i < 6; ++i)
			draw_list->AddRectFilledMultiColor(vsize(bar0_pos_x, picker_pos.y + i * (sv_picker_size / 6)), vsize(bar0_pos_x + bars_width, picker_pos.y + (i + 1) * (sv_picker_size / 6)), hue_colors[i], hue_colors[i], hue_colors[i + 1], hue_colors[i + 1]);

		float bar0_line_y = (float)(int)(picker_pos.y + H * (sv_picker_size - 4.0f + 0.5f));
		draw_list->AddRectFilled(vsize(bar0_pos_x, bar0_line_y), vsize(bar0_pos_x + bars_width, bar0_line_y + 4), IM_COL32_BLACK);
		draw_list->AddRectFilled(vsize(bar0_pos_x + 0, bar0_line_y + 1), vsize(bar0_pos_x + bars_width - 1, bar0_line_y + 3), hue_color32);

		draw_list->AddRect(vsize(bar0_pos_x - 1, picker_pos.y - 0), vsize(bar0_pos_x + bars_width, picker_pos.y + sv_picker_size + 0), IM_COL32(0, 0, 0, 255));
	}

	// Render cursor/preview circle (clamp S/V within 0..1 range because floating points colors may lead HSV values to be out of range)
	float sv_cursor_rad = 3;
	draw_list->AddRectFilled(sv_cursor_pos - vsize(2, 2), sv_cursor_pos + vsize(2, 2), IM_COL32(0, 0, 0, 255));
	draw_list->AddRectFilled(sv_cursor_pos - vsize(1, 1), sv_cursor_pos + vsize(1, 1), col32_no_alpha);

	if (alpha_bar) {

		float alpha = ImSaturate(col[3]);

		vsrect bar1_bb(bar1_pos_x, picker_pos.y, bar1_pos_x + bars_width, picker_pos.y + sv_picker_size);

		RenderColorRectWithAlphaCheckerboard(bar1_bb.Min, bar1_bb.Max, IM_COL32(0, 0, 0, 255), bar1_bb.GetWidth() / 2.0f, vsize(0.0f, 0.0f));

		draw_list->AddRectFilledMultiColor(bar1_bb.Min, bar1_bb.Max, col32_no_alpha, col32_no_alpha, col32_no_alpha & ~IM_COL32_A_MASK, col32_no_alpha & ~IM_COL32_A_MASK);
		draw_list->AddRect(bar1_bb.Min - vsize(1, 0), bar1_bb.Max, IM_COL32_BLACK);

		float bar1_line_y = (float)(int)(picker_pos.y + (1.0f - alpha) * (sv_picker_size - 4.0f + 0.5f));

		RenderFrameBorder(bar1_bb.Min, bar1_bb.Max, 0.0f);

		draw_list->AddRect(vsize(bar1_pos_x - 1, bar1_line_y), vsize(bar1_pos_x + bars_width, bar1_line_y + 4), IM_COL32_BLACK);
		draw_list->AddRectFilled(vsize(bar1_pos_x + 0, bar1_line_y + 1), vsize(bar1_pos_x + bars_width - 1, bar1_line_y + 3), IM_COL32(col[0], col[1], col[2], col[3]));
	}

	EndGroup();
	PopID();

	return value_changed;
}

// A little colored square. Return true when clicked.
// FIXME: May want to display/ignore the alpha component in the color display? Yet show it in the tooltip.
// 'desc_id' is not called 'label' because we don't display it next to the button, but only in the tooltip.
// Note that 'col' may be encoded in HSV if HanaLovesMeColorEditFlags_InputHSV is set.
bool HanaLovesMe::ColorButton(const char* desc_id, const xuifloatcolor & col, HanaLovesMeColorEditFlags flags, vsize size)
{
	HanaLovesMeWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	HanaLovesMeContext& g = *GHanaLovesMe;

	const HanaLovesMeID id = window->GetID(desc_id);
	float default_size = GetFrameHeight();

	if (size.x == 0.0f)
		size.x = default_size;

	if (size.y == 0.0f)
		size.y = default_size;

	const vsrect bb(window->DC.CursorPos + vsize(0, 3), window->DC.CursorPos + size - vsize(0, 5));
	ItemSize(bb, (size.y >= default_size) ? g.Style.FramePadding.y : 0.0f);

	if (!ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held);

	if (flags & HanaLovesMeColorEditFlags_NoAlpha)
		flags &= ~(HanaLovesMeColorEditFlags_AlphaPreview | HanaLovesMeColorEditFlags_AlphaPreviewHalf);

	xuifloatcolor col_without_alpha(col.x, col.y, col.z, 1.0f);

	float grid_step = ImMin(size.x, size.y) / 2.99f;
	float rounding = ImMin(g.Style.FrameRounding, grid_step * 0.5f);

	vsrect bb_inner = bb;

	xuifloatcolor col_source = (flags & HanaLovesMeColorEditFlags_AlphaPreview) ? col : col_without_alpha;
	auto gradient = GetColorU32(col_source) - xuicolor(0, 0, 0, 100);
	window->DrawList->AddRectFilledMultiColor(bb_inner.Min, bb_inner.Max, GetColorU32(col_source), GetColorU32(col_source), gradient, gradient);

	RenderNavHighlight(bb, id);

	if (g.Style.FrameBorderSize > 0.0f)
		RenderFrameBorder(bb.Min, bb.Max, rounding);
	else
		window->DrawList->AddRect(bb.Min, bb.Max, xuicolor(0, 0, 0, 255), rounding); // Color button are often in need of some sort of border

	// Tooltip
	if (!(flags & HanaLovesMeColorEditFlags_NoTooltip) && hovered)
		ColorTooltip(desc_id, &col.x, flags & (HanaLovesMeColorEditFlags_NoAlpha | HanaLovesMeColorEditFlags_AlphaPreview | HanaLovesMeColorEditFlags_AlphaPreviewHalf));

	if (pressed)
		MarkItemEdited(id);

	return pressed;
}

// Initialize/override default color options
void HanaLovesMe::SetColorEditOptions(HanaLovesMeColorEditFlags flags)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    if ((flags & HanaLovesMeColorEditFlags__DisplayMask) == 0)
        flags |= HanaLovesMeColorEditFlags__OptionsDefault & HanaLovesMeColorEditFlags__DisplayMask;
    if ((flags & HanaLovesMeColorEditFlags__DataTypeMask) == 0)
        flags |= HanaLovesMeColorEditFlags__OptionsDefault & HanaLovesMeColorEditFlags__DataTypeMask;
    if ((flags & HanaLovesMeColorEditFlags__PickerMask) == 0)
        flags |= HanaLovesMeColorEditFlags__OptionsDefault & HanaLovesMeColorEditFlags__PickerMask;
    if ((flags & HanaLovesMeColorEditFlags__InputMask) == 0)
        flags |= HanaLovesMeColorEditFlags__OptionsDefault & HanaLovesMeColorEditFlags__InputMask;
    IM_ASSERT(ImIsPowerOfTwo(flags & HanaLovesMeColorEditFlags__DisplayMask));    // Check only 1 option is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & HanaLovesMeColorEditFlags__DataTypeMask));   // Check only 1 option is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & HanaLovesMeColorEditFlags__PickerMask));     // Check only 1 option is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & HanaLovesMeColorEditFlags__InputMask));      // Check only 1 option is selected
    g.ColorEditOptions = flags;
}

// Note: only access 3 floats if HanaLovesMeColorEditFlags_NoAlpha flag is set.
void HanaLovesMe::ColorTooltip(const char* text, const float* col, HanaLovesMeColorEditFlags flags)
{
    HanaLovesMeContext& g = *GHanaLovesMe;

    BeginTooltipEx(0, true);
    const char* text_end = text ? FindRenderedTextEnd(text, NULL) : text;
    if (text_end > text)
    {
        TextEx(text, text_end);
        Separator();
    }

    vsize sz(g.FontSize * 3 + g.Style.FramePadding.y * 2, g.FontSize * 3 + g.Style.FramePadding.y * 2);
    xuifloatcolor cf(col[0], col[1], col[2], (flags & HanaLovesMeColorEditFlags_NoAlpha) ? 1.0f : col[3]);
    int cr = IM_F32_TO_INT8_SAT(col[0]), cg = IM_F32_TO_INT8_SAT(col[1]), cb = IM_F32_TO_INT8_SAT(col[2]), ca = (flags & HanaLovesMeColorEditFlags_NoAlpha) ? 255 : IM_F32_TO_INT8_SAT(col[3]);
    ColorButton("##preview", cf, (flags & (HanaLovesMeColorEditFlags__InputMask | HanaLovesMeColorEditFlags_NoAlpha | HanaLovesMeColorEditFlags_AlphaPreview | HanaLovesMeColorEditFlags_AlphaPreviewHalf)) | HanaLovesMeColorEditFlags_NoTooltip, sz);
    SameLine();
    if ((flags & HanaLovesMeColorEditFlags_InputRGB) || !(flags & HanaLovesMeColorEditFlags__InputMask))
    {
        if (flags & HanaLovesMeColorEditFlags_NoAlpha)
            Text("#%02X%02X%02X\nR: %d, G: %d, B: %d\n(%.3f, %.3f, %.3f)", cr, cg, cb, cr, cg, cb, col[0], col[1], col[2]);
        else
            Text("#%02X%02X%02X%02X\nR:%d, G:%d, B:%d, A:%d\n(%.3f, %.3f, %.3f, %.3f)", cr, cg, cb, ca, cr, cg, cb, ca, col[0], col[1], col[2], col[3]);
    } else if (flags & HanaLovesMeColorEditFlags_InputHSV)
    {
        if (flags & HanaLovesMeColorEditFlags_NoAlpha)
            Text("H: %.3f, S: %.3f, V: %.3f", col[0], col[1], col[2]);
        else
            Text("H: %.3f, S: %.3f, V: %.3f, A: %.3f", col[0], col[1], col[2], col[3]);
    }
    EndTooltip();
}

void HanaLovesMe::ColorEditOptionsPopup(const float* col, HanaLovesMeColorEditFlags flags)
{
    bool allow_opt_inputs = !(flags & HanaLovesMeColorEditFlags__DisplayMask);
    bool allow_opt_datatype = !(flags & HanaLovesMeColorEditFlags__DataTypeMask);
    if ((!allow_opt_inputs && !allow_opt_datatype) || !BeginPopup("context"))
        return;
    HanaLovesMeContext & g = *GHanaLovesMe;
    HanaLovesMeColorEditFlags opts = g.ColorEditOptions;
    if (allow_opt_inputs)
    {
        if (RadioButton("RGB", (opts & HanaLovesMeColorEditFlags_DisplayRGB) != 0)) opts = (opts & ~HanaLovesMeColorEditFlags__DisplayMask) | HanaLovesMeColorEditFlags_DisplayRGB;
        if (RadioButton("HSV", (opts & HanaLovesMeColorEditFlags_DisplayHSV) != 0)) opts = (opts & ~HanaLovesMeColorEditFlags__DisplayMask) | HanaLovesMeColorEditFlags_DisplayHSV;
        if (RadioButton("Hex", (opts & HanaLovesMeColorEditFlags_DisplayHex) != 0)) opts = (opts & ~HanaLovesMeColorEditFlags__DisplayMask) | HanaLovesMeColorEditFlags_DisplayHex;
    }
    if (allow_opt_datatype)
    {
        if (allow_opt_inputs) Separator();
        if (RadioButton("0..255", (opts & HanaLovesMeColorEditFlags_Uint8) != 0)) opts = (opts & ~HanaLovesMeColorEditFlags__DataTypeMask) | HanaLovesMeColorEditFlags_Uint8;
        if (RadioButton("0.00..1.00", (opts & HanaLovesMeColorEditFlags_Float) != 0)) opts = (opts & ~HanaLovesMeColorEditFlags__DataTypeMask) | HanaLovesMeColorEditFlags_Float;
    }

    if (allow_opt_inputs || allow_opt_datatype)
        Separator();
    if (Button("Copy as..", vsize(-1, 0)))
        OpenPopup("Copy");
    if (BeginPopup("Copy"))
    {
        int cr = IM_F32_TO_INT8_SAT(col[0]), cg = IM_F32_TO_INT8_SAT(col[1]), cb = IM_F32_TO_INT8_SAT(col[2]), ca = (flags & HanaLovesMeColorEditFlags_NoAlpha) ? 255 : IM_F32_TO_INT8_SAT(col[3]);
        char buf[64];
        ImFormatString(buf, IM_ARRAYSIZE(buf), "(%.3ff, %.3ff, %.3ff, %.3ff)", col[0], col[1], col[2], (flags & HanaLovesMeColorEditFlags_NoAlpha) ? 1.0f : col[3]);
        if (Selectable(buf))
            SetClipboardText(buf);
        ImFormatString(buf, IM_ARRAYSIZE(buf), "(%d,%d,%d,%d)", cr, cg, cb, ca);
        if (Selectable(buf))
            SetClipboardText(buf);
        if (flags & HanaLovesMeColorEditFlags_NoAlpha)
            ImFormatString(buf, IM_ARRAYSIZE(buf), "0x%02X%02X%02X", cr, cg, cb);
        else
            ImFormatString(buf, IM_ARRAYSIZE(buf), "0x%02X%02X%02X%02X", cr, cg, cb, ca);
        if (Selectable(buf))
            SetClipboardText(buf);
        EndPopup();
    }

    g.ColorEditOptions = opts;
    EndPopup();
}

void HanaLovesMe::ColorPickerOptionsPopup(const float* ref_col, HanaLovesMeColorEditFlags flags)
{
    bool allow_opt_picker = !(flags & HanaLovesMeColorEditFlags__PickerMask);
    bool allow_opt_alpha_bar = !(flags & HanaLovesMeColorEditFlags_NoAlpha) && !(flags & HanaLovesMeColorEditFlags_AlphaBar);
    if ((!allow_opt_picker && !allow_opt_alpha_bar) || !BeginPopup("context"))
        return;
    HanaLovesMeContext & g = *GHanaLovesMe;
    if (allow_opt_picker)
    {
        vsize picker_size(g.FontSize * 8, ImMax(g.FontSize * 8 - (GetFrameHeight() + g.Style.ItemInnerSpacing.x), 1.0f)); // FIXME: Picker size copied from main picker function
        PushItemWidth(picker_size.x);
        for (int picker_type = 0; picker_type < 2; picker_type++)
        {
            // Draw small/thumbnail version of each picker type (over an invisible button for selection)
            if (picker_type > 0) Separator();
            PushID(picker_type);
            HanaLovesMeColorEditFlags picker_flags = HanaLovesMeColorEditFlags_NoInputs | HanaLovesMeColorEditFlags_NoOptions | HanaLovesMeColorEditFlags_NoLabel | HanaLovesMeColorEditFlags_NoSidePreview | (flags & HanaLovesMeColorEditFlags_NoAlpha);
            if (picker_type == 0) picker_flags |= HanaLovesMeColorEditFlags_PickerHueBar;
            if (picker_type == 1) picker_flags |= HanaLovesMeColorEditFlags_PickerHueWheel;
            vsize backup_pos = GetCursorScreenPos();
            if (Selectable("##selectable", false, 0, picker_size)) // By default, Selectable() is closing popup
                g.ColorEditOptions = (g.ColorEditOptions & ~HanaLovesMeColorEditFlags__PickerMask) | (picker_flags & HanaLovesMeColorEditFlags__PickerMask);
            SetCursorScreenPos(backup_pos);
            xuifloatcolor dummy_ref_col;
            memcpy(&dummy_ref_col, ref_col, sizeof(float) * ((picker_flags & HanaLovesMeColorEditFlags_NoAlpha) ? 3 : 4));
            ColorPicker4("##dummypicker", &dummy_ref_col.x, picker_flags);
            PopID();
        }
        PopItemWidth();
    }
    if (allow_opt_alpha_bar)
    {
        if (allow_opt_picker) Separator();
        CheckboxFlags("Alpha Bar", (unsigned int*)& g.ColorEditOptions, HanaLovesMeColorEditFlags_AlphaBar);
    }
    EndPopup();
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: TreeNode, CollapsingHeader, etc.
//-------------------------------------------------------------------------
// - TreeNode()
// - TreeNodeV()
// - TreeNodeEx()
// - TreeNodeExV()
// - TreeNodeBehavior() [Internal]
// - TreePush()
// - TreePop()
// - TreeAdvanceToLabelPos()
// - GetTreeNodeToLabelSpacing()
// - SetNextTreeNodeOpen()
// - CollapsingHeader()
//-------------------------------------------------------------------------

bool HanaLovesMe::TreeNode(const char* str_id, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeExV(str_id, 0, fmt, args);
    va_end(args);
    return is_open;
}

bool HanaLovesMe::TreeNode(const void* ptr_id, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeExV(ptr_id, 0, fmt, args);
    va_end(args);
    return is_open;
}

bool HanaLovesMe::TreeNode(const char* label)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;
    return TreeNodeBehavior(window->GetID(label), 0, label, NULL);
}

bool HanaLovesMe::TreeNodeV(const char* str_id, const char* fmt, va_list args)
{
    return TreeNodeExV(str_id, 0, fmt, args);
}

bool HanaLovesMe::TreeNodeV(const void* ptr_id, const char* fmt, va_list args)
{
    return TreeNodeExV(ptr_id, 0, fmt, args);
}

bool HanaLovesMe::TreeNodeEx(const char* label, HanaLovesMeTreeNodeFlags flags)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    return TreeNodeBehavior(window->GetID(label), flags, label, NULL);
}

bool HanaLovesMe::TreeNodeEx(const char* str_id, HanaLovesMeTreeNodeFlags flags, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeExV(str_id, flags, fmt, args);
    va_end(args);
    return is_open;
}

bool HanaLovesMe::TreeNodeEx(const void* ptr_id, HanaLovesMeTreeNodeFlags flags, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeExV(ptr_id, flags, fmt, args);
    va_end(args);
    return is_open;
}

bool HanaLovesMe::TreeNodeExV(const char* str_id, HanaLovesMeTreeNodeFlags flags, const char* fmt, va_list args)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    HanaLovesMeContext& g = *GHanaLovesMe;
    const char* label_end = g.TempBuffer + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    return TreeNodeBehavior(window->GetID(str_id), flags, g.TempBuffer, label_end);
}

bool HanaLovesMe::TreeNodeExV(const void* ptr_id, HanaLovesMeTreeNodeFlags flags, const char* fmt, va_list args)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    HanaLovesMeContext& g = *GHanaLovesMe;
    const char* label_end = g.TempBuffer + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    return TreeNodeBehavior(window->GetID(ptr_id), flags, g.TempBuffer, label_end);
}

bool HanaLovesMe::TreeNodeBehaviorIsOpen(HanaLovesMeID id, HanaLovesMeTreeNodeFlags flags)
{
    if (flags & HanaLovesMeTreeNodeFlags_Leaf)
        return true;

    // We only write to the tree stohnly if the user clicks (or explicitly use SetNextTreeNode*** functions)
    HanaLovesMeContext& g = *GHanaLovesMe;
    HanaLovesMeWindow* window = g.CurrentWindow;
    HanaLovesMeStohnly* stohnly = window->DC.StateStohnly;

    bool is_open;
    if (g.NextTreeNodeOpenCond != 0)
    {
        if (g.NextTreeNodeOpenCond & HanaLovesMeCond_Always)
        {
            is_open = g.NextTreeNodeOpenVal;
            stohnly->SetInt(id, is_open);
        } else
        {
            // We treat HanaLovesMeCond_Once and HanaLovesMeCond_FirstUseEver the same because tree node state are not saved persistently.
            const int stored_value = stohnly->GetInt(id, -1);
            if (stored_value == -1)
            {
                is_open = g.NextTreeNodeOpenVal;
                stohnly->SetInt(id, is_open);
            } else
            {
                is_open = stored_value != 0;
            }
        }
        g.NextTreeNodeOpenCond = 0;
    } else
    {
        is_open = stohnly->GetInt(id, (flags & HanaLovesMeTreeNodeFlags_DefaultOpen) ? 1 : 0) != 0;
    }

    // When logging is enabled, we automatically expand tree nodes (but *NOT* collapsing headers.. seems like sensible behavior).
    // NB- If we are above max depth we still allow manually opened nodes to be logged.
    if (g.LogEnabled && !(flags & HanaLovesMeTreeNodeFlags_NoAutoOpenOnLog) && (window->DC.TreeDepth - g.LogDepthRef) < g.LogDepthToExpand)
        is_open = true;

    return is_open;
}

bool HanaLovesMe::TreeNodeBehavior(HanaLovesMeID id, HanaLovesMeTreeNodeFlags flags, const char* label, const char* label_end)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    HanaLovesMeContext& g = *GHanaLovesMe;
    const HanaLovesMeStyle& style = g.Style;
    const bool display_frame = (flags & HanaLovesMeTreeNodeFlags_Framed) != 0;
    const vsize padding = (display_frame || (flags & HanaLovesMeTreeNodeFlags_FramePadding)) ? style.FramePadding : vsize(style.FramePadding.x, 0.0f);

    if (!label_end)
        label_end = FindRenderedTextEnd(label);
    const vsize label_size = CalcTextSize(label, label_end, false);

    // We vertically grow up to current line height up the typical widget height.
    const float text_base_offset_y = ImMax(padding.y, window->DC.CurrentLineTextBaseOffset); // Latch before ItemSize changes it
    const float frame_height = ImMax(ImMin(window->DC.CurrentLineSize.y, g.FontSize + style.FramePadding.y * 2), label_size.y + padding.y * 2);
    vsrect frame_bb = vsrect(window->DC.CursorPos, vsize(GetContentRegionMaxScreen().x, window->DC.CursorPos.y + frame_height));
    if (display_frame)
    {
        // Framed header expand a little outside the default padding
        frame_bb.Min.x -= (float)(int)(window->WindowPadding.x * 0.5f) - 1;
        frame_bb.Max.x += (float)(int)(window->WindowPadding.x * 0.5f) - 1;
    }

    const float text_offset_x = (g.FontSize + (display_frame ? padding.x * 3 : padding.x * 2));   // Collapser arrow width + Spacing
    const float text_width = g.FontSize + (label_size.x > 0.0f ? label_size.x + padding.x * 2 : 0.0f);   // Include collapser
    ItemSize(vsize(text_width, frame_height), text_base_offset_y);

    // For regular tree nodes, we arbitrary allow to click past 2 worth of ItemSpacing
    // (Ideally we'd want to add a flag for the user to specify if we want the hit test to be done up to the right side of the content or not)
    const vsrect interact_bb = display_frame ? frame_bb : vsrect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + text_width + style.ItemSpacing.x * 2, frame_bb.Max.y);
    bool is_open = TreeNodeBehaviorIsOpen(id, flags);
    bool is_leaf = (flags & HanaLovesMeTreeNodeFlags_Leaf) != 0;

    // Store a flag for the current depth to tell if we will allow closing this node when navigating one of its child.
    // For this purpose we essentially compare if g.NavIdIsAlive went from 0 to 1 between TreeNode() and TreePop().
    // This is currently only support 32 level deep and we are fine with (1 << Depth) overflowing into a zero.
    if (is_open && !g.NavIdIsAlive && (flags & HanaLovesMeTreeNodeFlags_NavLeftJumpsBackHere) && !(flags & HanaLovesMeTreeNodeFlags_NoTreePushOnOpen))
        window->DC.TreeDepthMayJumpToParentOnPop |= (1 << window->DC.TreeDepth);

    bool item_add = ItemAdd(interact_bb, id);
    window->DC.LastItemStatusFlags |= HanaLovesMeItemStatusFlags_HasDisplayRect;
    window->DC.LastItemDisplayRect = frame_bb;

    if (!item_add)
    {
        if (is_open && !(flags & HanaLovesMeTreeNodeFlags_NoTreePushOnOpen))
            TreePushRawID(id);
        HanaLovesMe_TEST_ENGINE_ITEM_INFO(window->DC.LastItemId, label, window->DC.ItemFlags | (is_leaf ? 0 : HanaLovesMeItemStatusFlags_Openable) | (is_open ? HanaLovesMeItemStatusFlags_Opened : 0));
        return is_open;
    }

    // Flags that affects opening behavior:
    // - 0 (default) .................... single-click anywhere to open
    // - OpenOnDoubleClick .............. double-click anywhere to open
    // - OpenOnArrow .................... single-click on arrow to open
    // - OpenOnDoubleClick|OpenOnArrow .. single-click on arrow or double-click anywhere to open
    HanaLovesMeButtonFlags button_flags = HanaLovesMeButtonFlags_NoKeyModifiers;
    if (flags & HanaLovesMeTreeNodeFlags_AllowItemOverlap)
        button_flags |= HanaLovesMeButtonFlags_AllowItemOverlap;
    if (flags & HanaLovesMeTreeNodeFlags_OpenOnDoubleClick)
        button_flags |= HanaLovesMeButtonFlags_PressedOnDoubleClick | ((flags & HanaLovesMeTreeNodeFlags_OpenOnArrow) ? HanaLovesMeButtonFlags_PressedOnClickRelease : 0);
    if (!is_leaf)
        button_flags |= HanaLovesMeButtonFlags_PressedOnDragDropHold;

    bool selected = (flags & HanaLovesMeTreeNodeFlags_Selected) != 0;
    const bool was_selected = selected;

    bool hovered, held;
    bool pressed = ButtonBehavior(interact_bb, id, &hovered, &held, button_flags);
    bool toggled = false;
    if (!is_leaf)
    {
        if (pressed)
        {
            toggled = !(flags & (HanaLovesMeTreeNodeFlags_OpenOnArrow | HanaLovesMeTreeNodeFlags_OpenOnDoubleClick)) || (g.NavActivateId == id);
            if (flags & HanaLovesMeTreeNodeFlags_OpenOnArrow)
                toggled |= IsMouseHoveringRect(interact_bb.Min, vsize(interact_bb.Min.x + text_offset_x, interact_bb.Max.y)) && (!g.NavDisableMouseHover);
            if (flags & HanaLovesMeTreeNodeFlags_OpenOnDoubleClick)
                toggled |= g.IO.MouseDoubleClicked[0];
            if (g.DragDropActive && is_open) // When using Drag and Drop "hold to open" we keep the node highlighted after opening, but never close it again.
                toggled = false;
        }

        if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == HanaLovesMeDir_Left && is_open)
        {
            toggled = true;
            NavMoveRequestCancel();
        }
        if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == HanaLovesMeDir_Right && !is_open) // If there's something upcoming on the line we may want to give it the priority?
        {
            toggled = true;
            NavMoveRequestCancel();
        }

        if (toggled)
        {
            is_open = !is_open;
            window->DC.StateStohnly->SetInt(id, is_open);
        }
    }
    if (flags & HanaLovesMeTreeNodeFlags_AllowItemOverlap)
        SetItemAllowOverlap();

    // In this branch, TreeNodeBehavior() cannot toggle the selection so this will never trigger.
    if (selected != was_selected) //-V547
        window->DC.LastItemStatusFlags |= HanaLovesMeItemStatusFlags_ToggledSelection;

    // Render
    const ImU32 col = GetColorU32((held && hovered) ? HanaLovesMeCol_HeaderActive : hovered ? HanaLovesMeCol_HeaderHovered : HanaLovesMeCol_Header);
    const vsize text_pos = frame_bb.Min + vsize(text_offset_x, text_base_offset_y);
    HanaLovesMeNavHighlightFlags nav_highlight_flags = HanaLovesMeNavHighlightFlags_TypeThin;
    if (display_frame)
    {
        // Framed type
        RenderFrame(frame_bb.Min, frame_bb.Max, col, true, style.FrameRounding);
        RenderNavHighlight(frame_bb, id, nav_highlight_flags);
        RenderArrow(frame_bb.Min + vsize(padding.x, text_base_offset_y), is_open ? HanaLovesMeDir_Down : HanaLovesMeDir_Right, 1.0f);
        if (g.LogEnabled)
        {
            // NB: '##' is normally used to hide text (as a library-wide feature), so we need to specify the text range to make sure the ## aren't stripped out here.
            const char log_prefix[] = "\n##";
            const char log_suffix[] = "##";
            LogRenderedText(&text_pos, log_prefix, log_prefix + 3);
            RenderTextClipped(text_pos, frame_bb.Max, label, label_end, &label_size);
            LogRenderedText(&text_pos, log_suffix, log_suffix + 2);
        } else
        {
            RenderTextClipped(text_pos, frame_bb.Max, label, label_end, &label_size);
        }
    } else
    {
        // Unframed typed for tree nodes
        if (hovered || selected)
        {
            RenderFrame(frame_bb.Min, frame_bb.Max, col, false);
            RenderNavHighlight(frame_bb, id, nav_highlight_flags);
        }

        if (flags & HanaLovesMeTreeNodeFlags_Bullet)
            RenderBullet(frame_bb.Min + vsize(text_offset_x * 0.5f, g.FontSize * 0.50f + text_base_offset_y));
        else if (!is_leaf)
            RenderArrow(frame_bb.Min + vsize(padding.x, g.FontSize * 0.15f + text_base_offset_y), is_open ? HanaLovesMeDir_Down : HanaLovesMeDir_Right, 0.70f);
        if (g.LogEnabled)
            LogRenderedText(&text_pos, ">");
        RenderText(text_pos, label, label_end, false);
    }

    if (is_open && !(flags & HanaLovesMeTreeNodeFlags_NoTreePushOnOpen))
        TreePushRawID(id);
    HanaLovesMe_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags | (is_leaf ? 0 : HanaLovesMeItemStatusFlags_Openable) | (is_open ? HanaLovesMeItemStatusFlags_Opened : 0));
    return is_open;
}

void HanaLovesMe::TreePush(const char* str_id)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    Indent();
    window->DC.TreeDepth++;
    PushID(str_id ? str_id : "#TreePush");
}

void HanaLovesMe::TreePush(const void* ptr_id)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    Indent();
    window->DC.TreeDepth++;
    PushID(ptr_id ? ptr_id : (const void*)"#TreePush");
}

void HanaLovesMe::TreePushRawID(HanaLovesMeID id)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    Indent();
    window->DC.TreeDepth++;
    window->IDStack.push_back(id);
}

void HanaLovesMe::TreePop()
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    HanaLovesMeWindow* window = g.CurrentWindow;
    Unindent();

    window->DC.TreeDepth--;
    if (g.NavMoveDir == HanaLovesMeDir_Left && g.NavWindow == window && NavMoveRequestButNoResultYet())
        if (g.NavIdIsAlive && (window->DC.TreeDepthMayJumpToParentOnPop & (1 << window->DC.TreeDepth)))
        {
            SetNavID(window->IDStack.back(), g.NavLayer);
            NavMoveRequestCancel();
        }
    window->DC.TreeDepthMayJumpToParentOnPop &= (1 << window->DC.TreeDepth) - 1;

    IM_ASSERT(window->IDStack.Size > 1); // There should always be 1 element in the IDStack (pushed during window creation). If this triggers you called TreePop/PopID too much.
    PopID();
}

void HanaLovesMe::TreeAdvanceToLabelPos()
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    g.CurrentWindow->DC.CursorPos.x += GetTreeNodeToLabelSpacing();
}

// Horizontal distance preceding label when using TreeNode() or Bullet()
float HanaLovesMe::GetTreeNodeToLabelSpacing()
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    return g.FontSize + (g.Style.FramePadding.x * 2.0f);
}

void HanaLovesMe::SetNextTreeNodeOpen(bool is_open, HanaLovesMeCond cond)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    if (g.CurrentWindow->SkipItems)
        return;
    g.NextTreeNodeOpenVal = is_open;
    g.NextTreeNodeOpenCond = cond ? cond : HanaLovesMeCond_Always;
}

// CollapsingHeader returns true when opened but do not indent nor push into the ID stack (because of the HanaLovesMeTreeNodeFlags_NoTreePushOnOpen flag).
// This is basically the same as calling TreeNodeEx(label, HanaLovesMeTreeNodeFlags_CollapsingHeader). You can remove the _NoTreePushOnOpen flag if you want behavior closer to normal TreeNode().
bool HanaLovesMe::CollapsingHeader(const char* label, HanaLovesMeTreeNodeFlags flags)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    return TreeNodeBehavior(window->GetID(label), flags | HanaLovesMeTreeNodeFlags_CollapsingHeader, label);
}

bool HanaLovesMe::CollapsingHeader(const char* label, bool* p_open, HanaLovesMeTreeNodeFlags flags)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    if (p_open && !*p_open)
        return false;

    HanaLovesMeID id = window->GetID(label);
    bool is_open = TreeNodeBehavior(id, flags | HanaLovesMeTreeNodeFlags_CollapsingHeader | (p_open ? HanaLovesMeTreeNodeFlags_AllowItemOverlap : 0), label);
    if (p_open)
    {
        // Create a small overlapping close button // FIXME: We can evolve this into user accessible helpers to add extra buttons on title bars, headers, etc.
        HanaLovesMeContext& g = *GHanaLovesMe;
        HanaLovesMeItemHoveredDataBackup last_item_backup;
        float button_radius = g.FontSize * 0.5f;
        vsize button_center = vsize(ImMin(window->DC.LastItemRect.Max.x, window->ClipRect.Max.x) - g.Style.FramePadding.x - button_radius, window->DC.LastItemRect.GetCenter().y);
        if (CloseButton(window->GetID((void*)((intptr_t)id + 1)), button_center, button_radius))
            * p_open = false;
        last_item_backup.Restore();
    }

    return is_open;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Selectable
//-------------------------------------------------------------------------
// - Selectable()
//-------------------------------------------------------------------------

// Tip: pass a non-visible label (e.g. "##dummy") then you can use the space to draw other text or image.
// But you need to make sure the ID is unique, e.g. enclose calls in PushID/PopID or use ##unique_id.
bool HanaLovesMe::Selectable(const char* label, bool selected, HanaLovesMeSelectableFlags flags, const vsize & size_arg)
{
	HanaLovesMeWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	HanaLovesMeContext& g = *GHanaLovesMe;
	const HanaLovesMeStyle& style = g.Style;

	if ((flags & HanaLovesMeSelectableFlags_SpanAllColumns) && window->DC.CurrentColumns) // FIXME-OPT: Avoid if vertically clipped.
		PopClipRect();

	HanaLovesMeID id = window->GetID(label);
	vsize label_size = CalcTextSize(label, NULL, true);
	vsize size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);
	vsize pos = window->DC.CursorPos;
	pos.y += window->DC.CurrentLineTextBaseOffset;
	vsrect bb_inner(pos, pos + size);
	ItemSize(size);

	// Fill horizontal space.
	vsize window_padding = window->WindowPadding;
	float max_x = (flags & HanaLovesMeSelectableFlags_SpanAllColumns) ? GetWindowContentRegionMax().x : GetContentRegionMax().x;
	float w_draw = ImMax(label_size.x, window->Pos.x + max_x - window_padding.x - pos.x);
	vsize size_draw((size_arg.x != 0 && !(flags & HanaLovesMeSelectableFlags_DrawFillAvailWidth)) ? size_arg.x : w_draw, size_arg.y != 0.0f ? size_arg.y : size.y);
	vsrect bb(pos, pos + size_draw);
	if (size_arg.x == 0.0f || (flags & HanaLovesMeSelectableFlags_DrawFillAvailWidth))
		bb.Max.x += window_padding.x;

	// Selectables are tightly packed together so we extend the box to cover spacing between selectable.
	const float spacing_x = style.ItemSpacing.x;
	const float spacing_y = style.ItemSpacing.y;
	const float spacing_L = (float)(int)(spacing_x * 0.50f);
	const float spacing_U = (float)(int)(spacing_y * 0.50f);
	bb.Min.x -= spacing_L;
	bb.Min.y -= spacing_U;
	bb.Max.x += (spacing_x - spacing_L);
	bb.Max.y += (spacing_y - spacing_U);

	bool item_add;
	if (flags & HanaLovesMeSelectableFlags_Disabled)
	{
		HanaLovesMeItemFlags backup_item_flags = window->DC.ItemFlags;
		window->DC.ItemFlags |= HanaLovesMeItemFlags_Disabled | HanaLovesMeItemFlags_NoNavDefaultFocus;
		item_add = ItemAdd(bb, id);
		window->DC.ItemFlags = backup_item_flags;
	}
	else
	{
		item_add = ItemAdd(bb, id);
	}
	if (!item_add)
	{
		if ((flags & HanaLovesMeSelectableFlags_SpanAllColumns) && window->DC.CurrentColumns)
			PushColumnClipRect();
		return false;
	}

	// We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
	HanaLovesMeButtonFlags button_flags = 0;
	if (flags & HanaLovesMeSelectableFlags_NoHoldingActiveID) button_flags |= HanaLovesMeButtonFlags_NoHoldingActiveID;
	if (flags & HanaLovesMeSelectableFlags_PressedOnClick) button_flags |= HanaLovesMeButtonFlags_PressedOnClick;
	if (flags & HanaLovesMeSelectableFlags_PressedOnRelease) button_flags |= HanaLovesMeButtonFlags_PressedOnRelease;
	if (flags & HanaLovesMeSelectableFlags_Disabled) button_flags |= HanaLovesMeButtonFlags_Disabled;
	if (flags & HanaLovesMeSelectableFlags_AllowDoubleClick) button_flags |= HanaLovesMeButtonFlags_PressedOnClickRelease | HanaLovesMeButtonFlags_PressedOnDoubleClick;
	if (flags & HanaLovesMeSelectableFlags_AllowItemOverlap) button_flags |= HanaLovesMeButtonFlags_AllowItemOverlap;

	if (flags & HanaLovesMeSelectableFlags_Disabled)
		selected = false;

	const bool was_selected = selected;
	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, button_flags);
	// Hovering selectable with mouse updates NavId accordingly so navigation can be resumed with gamepad/keyboard (this doesn't happen on most widgets)
	if (pressed || hovered)
		if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
		{
			g.NavDisableHighlight = true;
			SetNavID(id, window->DC.NavLayerCurrent);
		}
	if (pressed)
		MarkItemEdited(id);

	if (flags & HanaLovesMeSelectableFlags_AllowItemOverlap)
		SetItemAllowOverlap();

	// In this branch, Selectable() cannot toggle the selection so this will never trigger.
	if (selected != was_selected) //-V547
		window->DC.LastItemStatusFlags |= HanaLovesMeItemStatusFlags_ToggledSelection;

	// Render
	if (hovered)
		window->DrawList->AddRectFilled(bb.Min, bb.Max + vsize(0, 1), xuicolor(24, 24, 24, 255));

	if ((flags & HanaLovesMeSelectableFlags_SpanAllColumns) && window->DC.CurrentColumns) {

		PushColumnClipRect();
		bb.Max.x -= (GetContentRegionMax().x - max_x);
	}

	if (flags & HanaLovesMeSelectableFlags_Disabled)
		PushStyleColor(HanaLovesMeCol_Text, g.Style.Colors[HanaLovesMeCol_TextDisabled]);

	if (selected || hovered)
		PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_MenuTheme, xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f));
	else
		PushColor(HanaLovesMeCol_Text, HanaLovesMeCol_TextDisabled, xuifloatcolor(0.0f, 0.0f, 0.0f, 0.0f));

	if (selected || hovered) {

		PopFont();
		PushFont(globals::boldMenuFont);

		RenderTextClipped(bb_inner.Min + vsize(4, 0), bb.Max, label, NULL, &label_size, vsize(0.0f, 0.0f));

		PopFont();
		PushFont(globals::menuFont);
	}
	else
		RenderTextClipped(bb_inner.Min + vsize(4, 0), bb.Max, label, NULL, &label_size, vsize(0.0f, 0.0f));	

	PopStyleColor();

	// Automatically close popups
	if (pressed && (window->Flags & HanaLovesMeWindowFlags_Popup) && !(flags & HanaLovesMeSelectableFlags_DontClosePopups) && !(window->DC.ItemFlags & HanaLovesMeItemFlags_SelectableDontClosePopup))
		CloseCurrentPopup();

	HanaLovesMe_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
	return pressed;
}

bool HanaLovesMe::Selectable(const char* label, bool* p_selected, HanaLovesMeSelectableFlags flags, const vsize & size_arg)
{
    if (Selectable(label, *p_selected, flags, size_arg))
    {
        *p_selected = !*p_selected;
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: ListBox
//-------------------------------------------------------------------------
// - ListBox()
// - ListBoxHeader()
// - ListBoxFooter()
//-------------------------------------------------------------------------
// FIXME: This is an old API. We should redesign some of it, rename ListBoxHeader->BeginListBox, ListBoxFooter->EndListBox
// and promote using them over existing ListBox() functions, similarly to change with combo boxes.
//-------------------------------------------------------------------------

// FIXME: In principle this function should be called BeginListBox(). We should rename it after re-evaluating if we want to keep the same signature.
// Helper to calculate the size of a listbox and display a label on the right.
// Tip: To have a list filling the entire window width, PushItemWidth(-1) and pass an non-visible label e.g. "##empty"
bool HanaLovesMe::ListBoxHeader(const char* label, const vsize & size_arg)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const HanaLovesMeStyle& style = GetStyle();
    const HanaLovesMeID id = GetID(label);
    const vsize label_size = CalcTextSize(label, NULL, true);

    // Size default to hold ~7 items. Fractional number of items helps seeing that we can scroll down/up without looking at scrollbar.
    vsize size = CalcItemSize(size_arg, CalcItemWidth(), GetTextLineHeightWithSpacing() * 7.4f + style.ItemSpacing.y);
    vsize frame_size = vsize(size.x, ImMax(size.y, label_size.y));
    vsrect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
    vsrect bb(frame_bb.Min, frame_bb.Max + vsize(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
    window->DC.LastItemRect = bb; // Forward stohnly for ListBoxFooter.. dodgy.

    if (!IsRectVisible(bb.Min, bb.Max))
    {
        ItemSize(bb.GetSize(), style.FramePadding.y);
        ItemAdd(bb, 0, &frame_bb);
        return false;
    }

    BeginGroup();
    if (label_size.x > 0)
        RenderText(vsize(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    BeginChildFrame(id, frame_bb.GetSize());
    return true;
}

// FIXME: In principle this function should be called EndListBox(). We should rename it after re-evaluating if we want to keep the same signature.
bool HanaLovesMe::ListBoxHeader(const char* label, int items_count, int height_in_items)
{
    // Size default to hold ~7.25 items.
    // We add +25% worth of item height to allow the user to see at a glance if there are more items up/down, without looking at the scrollbar.
    // We don't add this extra bit if items_count <= height_in_items. It is slightly dodgy, because it means a dynamic list of items will make the widget resize occasionally when it crosses that size.
    // I am expecting that someone will come and complain about this behavior in a remote future, then we can advise on a better solution.
    if (height_in_items < 0)
        height_in_items = ImMin(items_count, 7);
    const HanaLovesMeStyle& style = GetStyle();
    float height_in_items_f = (height_in_items < items_count) ? (height_in_items + 0.25f) : (height_in_items + 0.00f);

    // We include ItemSpacing.y so that a list sized for the exact number of items doesn't make a scrollbar appears. We could also enforce that by passing a flag to BeginChild().
    vsize size;
    size.x = 0.0f;
    size.y = GetTextLineHeightWithSpacing() * height_in_items_f + style.FramePadding.y * 2.0f;
    return ListBoxHeader(label, size);
}

// FIXME: In principle this function should be called EndListBox(). We should rename it after re-evaluating if we want to keep the same signature.
void HanaLovesMe::ListBoxFooter()
{
    HanaLovesMeWindow* parent_window = GetCurrentWindow()->ParentWindow;
    const vsrect bb = parent_window->DC.LastItemRect;
    const HanaLovesMeStyle& style = GetStyle();

    EndChildFrame();

    // Redeclare item size so that it includes the label (we have stored the full size in LastItemRect)
    // We call SameLine() to restore DC.CurrentLine* data
    SameLine();
    parent_window->DC.CursorPos = bb.Min;
    ItemSize(bb, style.FramePadding.y);
    EndGroup();
}

bool HanaLovesMe::ListBox(const char* label, int* current_item, const char* const items[], int items_count, int height_items)
{
    const bool value_changed = ListBox(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_items);
    return value_changed;
}

bool HanaLovesMe::ListBox(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int height_in_items)
{
    if (!ListBoxHeader(label, items_count, height_in_items))
        return false;

    // Assume all items have even height (= 1 line of text). If you need items of different or variable sizes you can create a custom version of ListBox() in your code without using the clipper.
    HanaLovesMeContext& g = *GHanaLovesMe;
    bool value_changed = false;
    HanaLovesMeListClipper clipper(items_count, GetTextLineHeightWithSpacing()); // We know exactly our line height here so we pass it as a minor optimization, but generally you don't need to.
    while (clipper.Step())
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            const bool item_selected = (i == *current_item);
            const char* item_text;
            if (!items_getter(data, i, &item_text))
                item_text = "*Unknown item*";

            PushID(i);
            if (Selectable(item_text, item_selected))
            {
                *current_item = i;
                value_changed = true;
            }
            if (item_selected)
                SetItemDefaultFocus();
            PopID();
        }
    ListBoxFooter();
    if (value_changed)
        MarkItemEdited(g.CurrentWindow->DC.LastItemId);

    return value_changed;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: PlotLines, PlotHistogram
//-------------------------------------------------------------------------
// - PlotEx() [Internal]
// - PlotLines()
// - PlotHistogram()
//-------------------------------------------------------------------------

void HanaLovesMe::PlotEx(HanaLovesMePlotType plot_type, const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, vsize frame_size)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    HanaLovesMeContext& g = *GHanaLovesMe;
    const HanaLovesMeStyle& style = g.Style;
    const HanaLovesMeID id = window->GetID(label);

    const vsize label_size = CalcTextSize(label, NULL, true);
    if (frame_size.x == 0.0f)
        frame_size.x = CalcItemWidth();
    if (frame_size.y == 0.0f)
        frame_size.y = label_size.y + (style.FramePadding.y * 2);

    const vsrect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
    const vsrect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
    const vsrect total_bb(frame_bb.Min, frame_bb.Max + vsize(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, 0, &frame_bb))
        return;
    const bool hovered = ItemHoverable(frame_bb, id);

    // Determine scale from values if not specified
    if (scale_min == FLT_MAX || scale_max == FLT_MAX)
    {
        float v_min = FLT_MAX;
        float v_max = -FLT_MAX;
        for (int i = 0; i < values_count; i++)
        {
            const float v = values_getter(data, i);
            if (v != v) // Ignore NaN values
                continue;
            v_min = ImMin(v_min, v);
            v_max = ImMax(v_max, v);
        }
        if (scale_min == FLT_MAX)
            scale_min = v_min;
        if (scale_max == FLT_MAX)
            scale_max = v_max;
    }

    RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(HanaLovesMeCol_FrameBg), true, style.FrameRounding);

    const int values_count_min = (plot_type == HanaLovesMePlotType_Lines) ? 2 : 1;
    if (values_count >= values_count_min)
    {
        int res_w = ImMin((int)frame_size.x, values_count) + ((plot_type == HanaLovesMePlotType_Lines) ? -1 : 0);
        int item_count = values_count + ((plot_type == HanaLovesMePlotType_Lines) ? -1 : 0);

        // Tooltip on hover
        int v_hovered = -1;
        if (hovered && inner_bb.Contains(g.IO.MousePos))
        {
            const float t = ImClamp((g.IO.MousePos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x), 0.0f, 0.9999f);
            const int v_idx = (int)(t * item_count);
            IM_ASSERT(v_idx >= 0 && v_idx < values_count);

            const float v0 = values_getter(data, (v_idx + values_offset) % values_count);
            const float v1 = values_getter(data, (v_idx + 1 + values_offset) % values_count);
            if (plot_type == HanaLovesMePlotType_Lines)
                SetTooltip("%d: %8.4g\n%d: %8.4g", v_idx, v0, v_idx + 1, v1);
            else if (plot_type == HanaLovesMePlotType_Histogram)
                SetTooltip("%d: %8.4g", v_idx, v0);
            v_hovered = v_idx;
        }

        const float t_step = 1.0f / (float)res_w;
        const float inv_scale = (scale_min == scale_max) ? 0.0f : (1.0f / (scale_max - scale_min));

        float v0 = values_getter(data, (0 + values_offset) % values_count);
        float t0 = 0.0f;
        vsize tp0 = vsize(t0, 1.0f - ImSaturate((v0 - scale_min) * inv_scale));                       // Point in the normalized space of our target rectangle
        float histogram_zero_line_t = (scale_min * scale_max < 0.0f) ? (-scale_min * inv_scale) : (scale_min < 0.0f ? 0.0f : 1.0f);   // Where does the zero line stands

        const ImU32 col_base = GetColorU32((plot_type == HanaLovesMePlotType_Lines) ? HanaLovesMeCol_PlotLines : HanaLovesMeCol_PlotHistogram);
        const ImU32 col_hovered = GetColorU32((plot_type == HanaLovesMePlotType_Lines) ? HanaLovesMeCol_PlotLinesHovered : HanaLovesMeCol_PlotHistogramHovered);

        for (int n = 0; n < res_w; n++)
        {
            const float t1 = t0 + t_step;
            const int v1_idx = (int)(t0 * item_count + 0.5f);
            IM_ASSERT(v1_idx >= 0 && v1_idx < values_count);
            const float v1 = values_getter(data, (v1_idx + values_offset + 1) % values_count);
            const vsize tp1 = vsize(t1, 1.0f - ImSaturate((v1 - scale_min) * inv_scale));

            // NB: Draw calls are merged together by the DrawList system. Still, we should render our batch are lower level to save a bit of CPU.
            vsize pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
            vsize pos1 = ImLerp(inner_bb.Min, inner_bb.Max, (plot_type == HanaLovesMePlotType_Lines) ? tp1 : vsize(tp1.x, histogram_zero_line_t));
            if (plot_type == HanaLovesMePlotType_Lines)
            {
                window->DrawList->AddLine(pos0, pos1, v_hovered == v1_idx ? col_hovered : col_base);
            } else if (plot_type == HanaLovesMePlotType_Histogram)
            {
                if (pos1.x >= pos0.x + 2.0f)
                    pos1.x -= 1.0f;
                window->DrawList->AddRectFilled(pos0, pos1, v_hovered == v1_idx ? col_hovered : col_base);
            }

            t0 = t1;
            tp0 = tp1;
        }
    }

    // Text overlay
    if (overlay_text)
        RenderTextClipped(vsize(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, overlay_text, NULL, NULL, vsize(0.5f, 0.0f));

    if (label_size.x > 0.0f)
        RenderText(vsize(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);
}

struct HanaLovesMePlotArrayGetterData
{
    const float* Values;
    int Stride;

    HanaLovesMePlotArrayGetterData(const float* values, int stride) { Values = values; Stride = stride; }
};

static float Plot_ArrayGetter(void* data, int idx)
{
    HanaLovesMePlotArrayGetterData* plot_data = (HanaLovesMePlotArrayGetterData*)data;
    const float v = *(const float*)(const void*)((const unsigned char*)plot_data->Values + (size_t)idx * plot_data->Stride);
    return v;
}

void HanaLovesMe::PlotLines(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, vsize graph_size, int stride)
{
    HanaLovesMePlotArrayGetterData data(values, stride);
    PlotEx(HanaLovesMePlotType_Lines, label, &Plot_ArrayGetter, (void*)& data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void HanaLovesMe::PlotLines(const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, vsize graph_size)
{
    PlotEx(HanaLovesMePlotType_Lines, label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void HanaLovesMe::PlotHistogram(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, vsize graph_size, int stride)
{
    HanaLovesMePlotArrayGetterData data(values, stride);
    PlotEx(HanaLovesMePlotType_Histogram, label, &Plot_ArrayGetter, (void*)& data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void HanaLovesMe::PlotHistogram(const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, vsize graph_size)
{
    PlotEx(HanaLovesMePlotType_Histogram, label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Value helpers
// Those is not very useful, legacy API.
//-------------------------------------------------------------------------
// - Value()
//-------------------------------------------------------------------------

void HanaLovesMe::Value(const char* prefix, bool b)
{
    Text("%s: %s", prefix, (b ? "true" : "false"));
}

void HanaLovesMe::Value(const char* prefix, int v)
{
    Text("%s: %d", prefix, v);
}

void HanaLovesMe::Value(const char* prefix, unsigned int v)
{
    Text("%s: %d", prefix, v);
}

void HanaLovesMe::Value(const char* prefix, float v, const char* float_format)
{
    if (float_format)
    {
        char fmt[64];
        ImFormatString(fmt, IM_ARRAYSIZE(fmt), "%%s: %s", float_format);
        Text(fmt, prefix, v);
    } else
    {
        Text("%s: %.3f", prefix, v);
    }
}

//-------------------------------------------------------------------------
// [SECTION] MenuItem, BeginMenu, EndMenu, etc.
//-------------------------------------------------------------------------
// - HanaLovesMeMenuColumns [Internal]
// - BeginMainMenuBar()
// - EndMainMenuBar()
// - BeginMenuBar()
// - EndMenuBar()
// - BeginMenu()
// - EndMenu()
// - MenuItem()
//-------------------------------------------------------------------------

// Helpers for internal use
HanaLovesMeMenuColumns::HanaLovesMeMenuColumns()
{
    Spacing = Width = NextWidth = 0.0f;
    memset(Pos, 0, sizeof(Pos));
    memset(NextWidths, 0, sizeof(NextWidths));
}

void HanaLovesMeMenuColumns::Update(int count, float spacing, bool clear)
{
    IM_ASSERT(count == IM_ARRAYSIZE(Pos));
    IM_UNUSED(count);
    Width = NextWidth = 0.0f;
    Spacing = spacing;
    if (clear)
        memset(NextWidths, 0, sizeof(NextWidths));
    for (int i = 0; i < IM_ARRAYSIZE(Pos); i++)
    {
        if (i > 0 && NextWidths[i] > 0.0f)
            Width += Spacing;
        Pos[i] = (float)(int)Width;
        Width += NextWidths[i];
        NextWidths[i] = 0.0f;
    }
}

float HanaLovesMeMenuColumns::DeclColumns(float w0, float w1, float w2) // not using va_arg because they promote float to double
{
    NextWidth = 0.0f;
    NextWidths[0] = ImMax(NextWidths[0], w0);
    NextWidths[1] = ImMax(NextWidths[1], w1);
    NextWidths[2] = ImMax(NextWidths[2], w2);
    for (int i = 0; i < IM_ARRAYSIZE(Pos); i++)
        NextWidth += NextWidths[i] + ((i > 0 && NextWidths[i] > 0.0f) ? Spacing : 0.0f);
    return ImMax(Width, NextWidth);
}

float HanaLovesMeMenuColumns::CalcExtraSpace(float avail_w)
{
    return ImMax(0.0f, avail_w - Width);
}

// For the main menu bar, which cannot be moved, we honor g.Style.DisplaySafeAreaPadding to ensure text can be visible on a TV set.
bool HanaLovesMe::BeginMainMenuBar()
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    g.NextWindowData.MenuBarOffsetMinVal = vsize(g.Style.DisplaySafeAreaPadding.x, ImMax(g.Style.DisplaySafeAreaPadding.y - g.Style.FramePadding.y, 0.0f));
    SetNextWindowPos(vsize(0.0f, 0.0f));
    SetNextWindowSize(vsize(g.IO.DisplaySize.x, g.NextWindowData.MenuBarOffsetMinVal.y + g.FontBaseSize + g.Style.FramePadding.y));
    PushStyleVar(HanaLovesMeStyleVar_WindowRounding, 0.0f);
    PushStyleVar(HanaLovesMeStyleVar_WindowMinSize, vsize(0, 0));
    HanaLovesMeWindowFlags window_flags = HanaLovesMeWindowFlags_NoTitleBar | HanaLovesMeWindowFlags_NoResize | HanaLovesMeWindowFlags_NoMove | HanaLovesMeWindowFlags_NoScrollbar | HanaLovesMeWindowFlags_NoSavedSettings | HanaLovesMeWindowFlags_MenuBar;
    bool is_open = Begin("##MainMenuBar", NULL, window_flags) && BeginMenuBar();
    PopStyleVar(2);
    g.NextWindowData.MenuBarOffsetMinVal = vsize(0.0f, 0.0f);
    if (!is_open)
    {
        End();
        return false;
    }
    return true; //-V1020
}

void HanaLovesMe::EndMainMenuBar()
{
    EndMenuBar();

    // When the user has left the menu layer (typically: closed menus through activation of an item), we restore focus to the previous window
    HanaLovesMeContext& g = *GHanaLovesMe;
    if (g.CurrentWindow == g.NavWindow && g.NavLayer == 0)
        FocusPreviousWindowIgnoringOne(g.NavWindow);

    End();
}

bool HanaLovesMe::BeginMenuBar()
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;
    if (!(window->Flags & HanaLovesMeWindowFlags_MenuBar))
        return false;

    IM_ASSERT(!window->DC.MenuBarAppending);
    BeginGroup(); // Backup position on layer 0
    PushID("##menubar");

    // We don't clip with current window clipping rectangle as it is already set to the area below. However we clip with window full rect.
    // We remove 1 worth of rounding to Max.x to that text in long menus and small windows don't tend to display over the lower-right rounded area, which looks particularly glitchy.
    vsrect bar_rect = window->MenuBarRect();
    vsrect clip_rect(ImFloor(bar_rect.Min.x + 0.5f), ImFloor(bar_rect.Min.y + window->WindowBorderSize + 0.5f), ImFloor(ImMax(bar_rect.Min.x, bar_rect.Max.x - window->WindowRounding) + 0.5f), ImFloor(bar_rect.Max.y + 0.5f));
    clip_rect.ClipWith(window->OuterRectClipped);
    PushClipRect(clip_rect.Min, clip_rect.Max, false);

    window->DC.CursorPos = vsize(bar_rect.Min.x + window->DC.MenuBarOffset.x, bar_rect.Min.y + window->DC.MenuBarOffset.y);
    window->DC.LayoutType = HanaLovesMeLayoutType_Horizontal;
    window->DC.NavLayerCurrent = HanaLovesMeNavLayer_Menu;
    window->DC.NavLayerCurrentMask = (1 << HanaLovesMeNavLayer_Menu);
    window->DC.MenuBarAppending = true;
    AlignTextToFramePadding();
    return true;
}

void HanaLovesMe::EndMenuBar()
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    HanaLovesMeContext& g = *GHanaLovesMe;

    // Nav: When a move request within one of our child menu failed, capture the request to navigate among our siblings.
    if (NavMoveRequestButNoResultYet() && (g.NavMoveDir == HanaLovesMeDir_Left || g.NavMoveDir == HanaLovesMeDir_Right) && (g.NavWindow->Flags & HanaLovesMeWindowFlags_ChildMenu))
    {
        HanaLovesMeWindow* nav_earliest_child = g.NavWindow;
        while (nav_earliest_child->ParentWindow && (nav_earliest_child->ParentWindow->Flags & HanaLovesMeWindowFlags_ChildMenu))
            nav_earliest_child = nav_earliest_child->ParentWindow;
        if (nav_earliest_child->ParentWindow == window && nav_earliest_child->DC.ParentLayoutType == HanaLovesMeLayoutType_Horizontal && g.NavMoveRequestForward == HanaLovesMeNavForward_None)
        {
            // To do so we claim focus back, restore NavId and then process the movement request for yet another frame.
            // This involve a one-frame delay which isn't very problematic in this situation. We could remove it by scoring in advance for multiple window (probably not worth the hassle/cost)
            IM_ASSERT(window->DC.NavLayerActiveMaskNext & 0x02); // Sanity check
            FocusWindow(window);
            SetNavIDWithRectRel(window->NavLastIds[1], 1, window->NavRectRel[1]);
            g.NavLayer = HanaLovesMeNavLayer_Menu;
            g.NavDisableHighlight = true; // Hide highlight for the current frame so we don't see the intermediary selection.
            g.NavMoveRequestForward = HanaLovesMeNavForward_ForwardQueued;
            NavMoveRequestCancel();
        }
    }

    IM_ASSERT(window->Flags & HanaLovesMeWindowFlags_MenuBar);
    IM_ASSERT(window->DC.MenuBarAppending);
    PopClipRect();
    PopID();
    window->DC.MenuBarOffset.x = window->DC.CursorPos.x - window->MenuBarRect().Min.x; // Save horizontal position so next append can reuse it. This is kinda equivalent to a per-layer CursorPos.
    window->DC.GroupStack.back().AdvanceCursor = false;
    EndGroup(); // Restore position on layer 0
    window->DC.LayoutType = HanaLovesMeLayoutType_Vertical;
    window->DC.NavLayerCurrent = HanaLovesMeNavLayer_Main;
    window->DC.NavLayerCurrentMask = (1 << HanaLovesMeNavLayer_Main);
    window->DC.MenuBarAppending = false;
}

bool HanaLovesMe::BeginMenu(const char* label, bool enabled)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    HanaLovesMeContext& g = *GHanaLovesMe;
    const HanaLovesMeStyle& style = g.Style;
    const HanaLovesMeID id = window->GetID(label);

    vsize label_size = CalcTextSize(label, NULL, true);

    bool pressed;
    bool menu_is_open = IsPopupOpen(id);
    bool menuset_is_open = !(window->Flags & HanaLovesMeWindowFlags_Popup) && (g.OpenPopupStack.Size > g.BeginPopupStack.Size && g.OpenPopupStack[g.BeginPopupStack.Size].OpenParentId == window->IDStack.back());
    HanaLovesMeWindow * backed_nav_window = g.NavWindow;
    if (menuset_is_open)
        g.NavWindow = window;  // Odd hack to allow hovering across menus of a same menu-set (otherwise we wouldn't be able to hover parent)

    // The reference position stored in popup_pos will be used by Begin() to find a suitable position for the child menu,
    // However the final position is going to be different! It is choosen by FindBestWindowPosForPopup().
    // e.g. Menus tend to overlap each other horizontally to amplify relative Z-ordering.
    vsize popup_pos, pos = window->DC.CursorPos;
    if (window->DC.LayoutType == HanaLovesMeLayoutType_Horizontal)
    {
        // Menu inside an horizontal menu bar
        // Selectable extend their highlight by half ItemSpacing in each direction.
        // For ChildMenu, the popup position will be overwritten by the call to FindBestWindowPosForPopup() in Begin()
        popup_pos = vsize(pos.x - 1.0f - (float)(int)(style.ItemSpacing.x * 0.5f), pos.y - style.FramePadding.y + window->MenuBarHeight());
        window->DC.CursorPos.x += (float)(int)(style.ItemSpacing.x * 0.5f);
        PushStyleVar(HanaLovesMeStyleVar_ItemSpacing, style.ItemSpacing * 2.0f);
        float w = label_size.x;
        pressed = Selectable(label, menu_is_open, HanaLovesMeSelectableFlags_NoHoldingActiveID | HanaLovesMeSelectableFlags_PressedOnClick | HanaLovesMeSelectableFlags_DontClosePopups | (!enabled ? HanaLovesMeSelectableFlags_Disabled : 0), vsize(w, 0.0f));
        PopStyleVar();
        window->DC.CursorPos.x += (float)(int)(style.ItemSpacing.x * (-1.0f + 0.5f)); // -1 spacing to compensate the spacing added when Selectable() did a SameLine(). It would also work to call SameLine() ourselves after the PopStyleVar().
    } else
    {
        // Menu inside a menu
        popup_pos = vsize(pos.x, pos.y - style.WindowPadding.y);
        float w = window->MenuColumns.DeclColumns(label_size.x, 0.0f, (float)(int)(g.FontSize * 1.20f)); // Feedback to next frame
        float extra_w = ImMax(0.0f, GetContentRegionAvail().x - w);
        pressed = Selectable(label, menu_is_open, HanaLovesMeSelectableFlags_NoHoldingActiveID | HanaLovesMeSelectableFlags_PressedOnClick | HanaLovesMeSelectableFlags_DontClosePopups | HanaLovesMeSelectableFlags_DrawFillAvailWidth | (!enabled ? HanaLovesMeSelectableFlags_Disabled : 0), vsize(w, 0.0f));
        if (!enabled) PushStyleColor(HanaLovesMeCol_Text, g.Style.Colors[HanaLovesMeCol_TextDisabled]);
        RenderArrow(pos + vsize(window->MenuColumns.Pos[2] + extra_w + g.FontSize * 0.30f, 0.0f), HanaLovesMeDir_Right);
        if (!enabled) PopStyleColor();
    }

    const bool hovered = enabled && ItemHoverable(window->DC.LastItemRect, id);
    if (menuset_is_open)
        g.NavWindow = backed_nav_window;

    bool want_open = false, want_close = false;
    if (window->DC.LayoutType == HanaLovesMeLayoutType_Vertical) // (window->Flags & (HanaLovesMeWindowFlags_Popup|HanaLovesMeWindowFlags_ChildMenu))
    {
        // Implement http://bjk5.com/post/44698559168/breaking-down-amazons-mega-dropdown to avoid using timers, so menus feels more reactive.
        bool moving_within_opened_triangle = false;
        if (g.HoveredWindow == window && g.OpenPopupStack.Size > g.BeginPopupStack.Size && g.OpenPopupStack[g.BeginPopupStack.Size].ParentWindow == window && !(window->Flags & HanaLovesMeWindowFlags_MenuBar))
        {
            if (HanaLovesMeWindow * next_window = g.OpenPopupStack[g.BeginPopupStack.Size].Window)
            {
                // FIXME-DPI: Values should be derived from a master "scale" factor.
                vsrect next_window_rect = next_window->Rect();
                vsize ta = g.IO.MousePos - g.IO.MouseDelta;
                vsize tb = (window->Pos.x < next_window->Pos.x) ? next_window_rect.GetTL() : next_window_rect.GetTR();
                vsize tc = (window->Pos.x < next_window->Pos.x) ? next_window_rect.GetBL() : next_window_rect.GetBR();
                float extra = ImClamp(ImFabs(ta.x - tb.x) * 0.30f, 5.0f, 30.0f); // add a bit of extra slack.
                ta.x += (window->Pos.x < next_window->Pos.x) ? -0.5f : +0.5f;    // to avoid numerical issues
                tb.y = ta.y + ImMax((tb.y - extra) - ta.y, -100.0f);             // triangle is maximum 200 high to limit the slope and the bias toward large sub-menus // FIXME: Multiply by fb_scale?
                tc.y = ta.y + ImMin((tc.y + extra) - ta.y, +100.0f);
                moving_within_opened_triangle = ImTriangleContainsPoint(ta, tb, tc, g.IO.MousePos);
                //window->DrawList->PushClipRectFullScreen(); window->DrawList->AddTriangleFilled(ta, tb, tc, moving_within_opened_triangle ? IM_COL32(0,128,0,128) : IM_COL32(128,0,0,128)); window->DrawList->PopClipRect(); // Debug
            }
        }

        want_close = (menu_is_open && !hovered && g.HoveredWindow == window && g.HoveredIdPreviousFrame != 0 && g.HoveredIdPreviousFrame != id && !moving_within_opened_triangle);
        want_open = (!menu_is_open && hovered && !moving_within_opened_triangle) || (!menu_is_open && hovered && pressed);

        if (g.NavActivateId == id)
        {
            want_close = menu_is_open;
            want_open = !menu_is_open;
        }
        if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == HanaLovesMeDir_Right) // Nav-Right to open
        {
            want_open = true;
            NavMoveRequestCancel();
        }
    } else
    {
        // Menu bar
        if (menu_is_open && pressed && menuset_is_open) // Click an open menu again to close it
        {
            want_close = true;
            want_open = menu_is_open = false;
        } else if (pressed || (hovered && menuset_is_open && !menu_is_open)) // First click to open, then hover to open others
        {
            want_open = true;
        } else if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == HanaLovesMeDir_Down) // Nav-Down to open
        {
            want_open = true;
            NavMoveRequestCancel();
        }
    }

    if (!enabled) // explicitly close if an open menu becomes disabled, facilitate users code a lot in pattern such as 'if (BeginMenu("options", has_object)) { ..use object.. }'
        want_close = true;
    if (want_close && IsPopupOpen(id))
        ClosePopupToLevel(g.BeginPopupStack.Size, true);

    HanaLovesMe_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags | HanaLovesMeItemStatusFlags_Openable | (menu_is_open ? HanaLovesMeItemStatusFlags_Opened : 0));

    if (!menu_is_open && want_open && g.OpenPopupStack.Size > g.BeginPopupStack.Size)
    {
        // Don't recycle same menu level in the same frame, first close the other menu and yield for a frame.
        OpenPopup(label);
        return false;
    }

    menu_is_open |= want_open;
    if (want_open)
        OpenPopup(label);

    if (menu_is_open)
    {
        // Sub-menus are ChildWindow so that mouse can be hovering across them (otherwise top-most popup menu would steal focus and not allow hovering on parent menu)
        SetNextWindowPos(popup_pos, HanaLovesMeCond_Always);
        HanaLovesMeWindowFlags flags = HanaLovesMeWindowFlags_ChildMenu | HanaLovesMeWindowFlags_AlwaysAutoResize | HanaLovesMeWindowFlags_NoMove | HanaLovesMeWindowFlags_NoTitleBar | HanaLovesMeWindowFlags_NoSavedSettings | HanaLovesMeWindowFlags_NoNavFocus;
        if (window->Flags & (HanaLovesMeWindowFlags_Popup | HanaLovesMeWindowFlags_ChildMenu))
            flags |= HanaLovesMeWindowFlags_ChildWindow;
        menu_is_open = BeginPopupEx(id, flags); // menu_is_open can be 'false' when the popup is completely clipped (e.g. zero size display)
    }

    return menu_is_open;
}

void HanaLovesMe::EndMenu()
{
    // Nav: When a left move request _within our child menu_ failed, close ourselves (the _parent_ menu).
    // A menu doesn't close itself because EndMenuBar() wants the catch the last Left<>Right inputs.
    // However, it means that with the current code, a BeginMenu() from outside another menu or a menu-bar won't be closable with the Left direction.
    HanaLovesMeContext& g = *GHanaLovesMe;
    HanaLovesMeWindow* window = g.CurrentWindow;
    if (g.NavWindow && g.NavWindow->ParentWindow == window && g.NavMoveDir == HanaLovesMeDir_Left && NavMoveRequestButNoResultYet() && window->DC.LayoutType == HanaLovesMeLayoutType_Vertical)
    {
        ClosePopupToLevel(g.BeginPopupStack.Size, true);
        NavMoveRequestCancel();
    }

    EndPopup();
}

bool HanaLovesMe::MenuItem(const char* label, const char* shortcut, bool selected, bool enabled)
{
    HanaLovesMeWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    HanaLovesMeContext& g = *GHanaLovesMe;
    HanaLovesMeStyle& style = g.Style;
    vsize pos = window->DC.CursorPos;
    vsize label_size = CalcTextSize(label, NULL, true);

    HanaLovesMeSelectableFlags flags = HanaLovesMeSelectableFlags_PressedOnRelease | (enabled ? 0 : HanaLovesMeSelectableFlags_Disabled);
    bool pressed;
    if (window->DC.LayoutType == HanaLovesMeLayoutType_Horizontal)
    {
        // Mimic the exact layout spacing of BeginMenu() to allow MenuItem() inside a menu bar, which is a little misleading but may be useful
        // Note that in this situation we render neither the shortcut neither the selected tick mark
        float w = label_size.x;
        window->DC.CursorPos.x += (float)(int)(style.ItemSpacing.x * 0.5f);
        PushStyleVar(HanaLovesMeStyleVar_ItemSpacing, style.ItemSpacing * 2.0f);
        pressed = Selectable(label, false, flags, vsize(w, 0.0f));
        PopStyleVar();
        window->DC.CursorPos.x += (float)(int)(style.ItemSpacing.x * (-1.0f + 0.5f)); // -1 spacing to compensate the spacing added when Selectable() did a SameLine(). It would also work to call SameLine() ourselves after the PopStyleVar().
    } else
    {
        vsize shortcut_size = shortcut ? CalcTextSize(shortcut, NULL) : vsize(0.0f, 0.0f);
        float w = window->MenuColumns.DeclColumns(label_size.x, shortcut_size.x, (float)(int)(g.FontSize * 1.20f)); // Feedback for next frame
        float extra_w = ImMax(0.0f, GetContentRegionAvail().x - w);
        pressed = Selectable(label, false, flags | HanaLovesMeSelectableFlags_DrawFillAvailWidth, vsize(w, 0.0f));
        if (shortcut_size.x > 0.0f)
        {
            PushStyleColor(HanaLovesMeCol_Text, g.Style.Colors[HanaLovesMeCol_TextDisabled]);
            RenderText(pos + vsize(window->MenuColumns.Pos[1] + extra_w, 0.0f), shortcut, NULL, false);
            PopStyleColor();
        }
        if (selected)
            RenderCheckMark(pos + vsize(window->MenuColumns.Pos[2] + extra_w + g.FontSize * 0.40f, g.FontSize * 0.134f * 0.5f), GetColorU32(enabled ? HanaLovesMeCol_Text : HanaLovesMeCol_TextDisabled), g.FontSize * 0.866f);
    }

    HanaLovesMe_TEST_ENGINE_ITEM_INFO(window->DC.LastItemId, label, window->DC.ItemFlags | HanaLovesMeItemStatusFlags_Checkable | (selected ? HanaLovesMeItemStatusFlags_Checked : 0));
    return pressed;
}

bool HanaLovesMe::MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled)
{
    if (MenuItem(label, shortcut, p_selected ? *p_selected : false, enabled))
    {
        if (p_selected)
            * p_selected = !*p_selected;
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: BeginTabBar, EndTabBar, etc.
//-------------------------------------------------------------------------
// [BETA API] API may evolve! This code has been extracted out of the Docking branch,
// and some of the construct which are not used in Master may be left here to facilitate merging.
//-------------------------------------------------------------------------
// - BeginTabBar()
// - BeginTabBarEx() [Internal]
// - EndTabBar()
// - TabBarLayout() [Internal]
// - TabBarCalcTabID() [Internal]
// - TabBarCalcMaxTabWidth() [Internal]
// - TabBarFindTabById() [Internal]
// - TabBarRemoveTab() [Internal]
// - TabBarCloseTab() [Internal]
// - TabBarScrollClamp()v
// - TabBarScrollToTab() [Internal]
// - TabBarQueueChangeTabOrder() [Internal]
// - TabBarScrollingButtons() [Internal]
// - TabBarTabListPopupButton() [Internal]
//-------------------------------------------------------------------------

namespace HanaLovesMe
{
    static void             TabBarLayout(HanaLovesMeTabBar* tab_bar);
    static ImU32            TabBarCalcTabID(HanaLovesMeTabBar* tab_bar, const char* label);
    static float            TabBarCalcMaxTabWidth();
    static float            TabBarScrollClamp(HanaLovesMeTabBar* tab_bar, float scrolling);
    static void             TabBarScrollToTab(HanaLovesMeTabBar* tab_bar, HanaLovesMeTabItem* tab);
    static HanaLovesMeTabItem* TabBarScrollingButtons(HanaLovesMeTabBar* tab_bar);
    static HanaLovesMeTabItem* TabBarTabListPopupButton(HanaLovesMeTabBar* tab_bar);
}

HanaLovesMeTabBar::HanaLovesMeTabBar()
{
    ID = 0;
    SelectedTabId = NextSelectedTabId = VisibleTabId = 0;
    CurrFrameVisible = PrevFrameVisible = -1;
    ContentsHeight = 0.0f;
    OffsetMax = OffsetNextTab = 0.0f;
    ScrollingAnim = ScrollingTarget = ScrollingTargetDistToVisibility = ScrollingSpeed = 0.0f;
    Flags = HanaLovesMeTabBarFlags_None;
    ReorderRequestTabId = 0;
    ReorderRequestDir = 0;
    WantLayout = VisibleTabWasSubmitted = false;
    LastTabItemIdx = -1;
}

static int HanaLovesMe_CDECL TabItemComparerByVisibleOffset(const void* lhs, const void* rhs)
{
    const HanaLovesMeTabItem* a = (const HanaLovesMeTabItem*)lhs;
    const HanaLovesMeTabItem* b = (const HanaLovesMeTabItem*)rhs;
    return (int)(a->Offset - b->Offset);
}

static int HanaLovesMe_CDECL TabBarSortItemComparer(const void* lhs, const void* rhs)
{
    const HanaLovesMeTabBarSortItem* a = (const HanaLovesMeTabBarSortItem*)lhs;
    const HanaLovesMeTabBarSortItem* b = (const HanaLovesMeTabBarSortItem*)rhs;
    if (int d = (int)(b->Width - a->Width))
        return d;
    return (b->Index - a->Index);
}

static HanaLovesMeTabBar * GetTabBarFromTabBarRef(const HanaLovesMeTabBarRef & ref)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    return ref.Ptr ? ref.Ptr : g.TabBars.GetByIndex(ref.IndexInMainPool);
}

static HanaLovesMeTabBarRef GetTabBarRefFromTabBar(HanaLovesMeTabBar * tab_bar)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    if (g.TabBars.Contains(tab_bar))
        return HanaLovesMeTabBarRef(g.TabBars.GetIndex(tab_bar));
    return HanaLovesMeTabBarRef(tab_bar);
}

bool    HanaLovesMe::BeginTabBar(const char* str_id, HanaLovesMeTabBarFlags flags)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    HanaLovesMeWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    HanaLovesMeID id = window->GetID(str_id);
    HanaLovesMeTabBar* tab_bar = g.TabBars.GetOrAddByKey(id);
    vsrect tab_bar_bb = vsrect(window->DC.CursorPos.x, window->DC.CursorPos.y, window->InnerClipRect.Max.x, window->DC.CursorPos.y + g.FontSize + g.Style.FramePadding.y * 2);
    tab_bar->ID = id;
    return BeginTabBarEx(tab_bar, tab_bar_bb, flags | HanaLovesMeTabBarFlags_IsFocused);
}

bool    HanaLovesMe::BeginTabBarEx(HanaLovesMeTabBar * tab_bar, const vsrect & tab_bar_bb, HanaLovesMeTabBarFlags flags)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    HanaLovesMeWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    if ((flags & HanaLovesMeTabBarFlags_DockNode) == 0)
        window->IDStack.push_back(tab_bar->ID);

    // Add to stack
    g.CurrentTabBarStack.push_back(GetTabBarRefFromTabBar(tab_bar));
    g.CurrentTabBar = tab_bar;

    if (tab_bar->CurrFrameVisible == g.FrameCount)
    {
        //HanaLovesMe_DEBUG_LOG("BeginTabBarEx already called this frame\n", g.FrameCount);
        IM_ASSERT(0);
        return true;
    }

    // When toggling back from ordered to manually-reorderable, shuffle tabs to enforce the last visible order.
    // Otherwise, the most recently inserted tabs would move at the end of visible list which can be a little too confusing or magic for the user.
    if ((flags & HanaLovesMeTabBarFlags_Reorderable) && !(tab_bar->Flags & HanaLovesMeTabBarFlags_Reorderable) && tab_bar->Tabs.Size > 1 && tab_bar->PrevFrameVisible != -1)
        ImQsort(tab_bar->Tabs.Data, tab_bar->Tabs.Size, sizeof(HanaLovesMeTabItem), TabItemComparerByVisibleOffset);

    // Flags
    if ((flags & HanaLovesMeTabBarFlags_FittingPolicyMask_) == 0)
        flags |= HanaLovesMeTabBarFlags_FittingPolicyDefault_;

    tab_bar->Flags = flags;
    tab_bar->BarRect = tab_bar_bb;
    tab_bar->WantLayout = true; // Layout will be done on the first call to ItemTab()
    tab_bar->PrevFrameVisible = tab_bar->CurrFrameVisible;
    tab_bar->CurrFrameVisible = g.FrameCount;
    tab_bar->FramePadding = g.Style.FramePadding;

    // Layout
    ItemSize(vsize(tab_bar->OffsetMax, tab_bar->BarRect.GetHeight()));
    window->DC.CursorPos.x = tab_bar->BarRect.Min.x;

    // Draw separator
    const ImU32 col = GetColorU32((flags & HanaLovesMeTabBarFlags_IsFocused) ? HanaLovesMeCol_TabActive : HanaLovesMeCol_Tab);
    const float y = tab_bar->BarRect.Max.y - 1.0f;
    {
        const float separator_min_x = tab_bar->BarRect.Min.x - window->WindowPadding.x;
        const float separator_max_x = tab_bar->BarRect.Max.x + window->WindowPadding.x;
        window->DrawList->AddLine(vsize(separator_min_x, y), vsize(separator_max_x, y), col, 1.0f);
    }
    return true;
}

void    HanaLovesMe::EndTabBar()
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    HanaLovesMeWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return;

    HanaLovesMeTabBar* tab_bar = g.CurrentTabBar;
    if (tab_bar == NULL)
    {
        IM_ASSERT(tab_bar != NULL && "Mismatched BeginTabBar()/EndTabBar()!");
        return; // FIXME-ERRORHANDLING
    }
    if (tab_bar->WantLayout)
        TabBarLayout(tab_bar);

    // Restore the last visible height if no tab is visible, this reduce vertical flicker/movement when a tabs gets removed without calling SetTabItemClosed().
    const bool tab_bar_appearing = (tab_bar->PrevFrameVisible + 1 < g.FrameCount);
    if (tab_bar->VisibleTabWasSubmitted || tab_bar->VisibleTabId == 0 || tab_bar_appearing)
        tab_bar->ContentsHeight = ImMax(window->DC.CursorPos.y - tab_bar->BarRect.Max.y, 0.0f);
    else
        window->DC.CursorPos.y = tab_bar->BarRect.Max.y + tab_bar->ContentsHeight;

    if ((tab_bar->Flags & HanaLovesMeTabBarFlags_DockNode) == 0)
        PopID();

    g.CurrentTabBarStack.pop_back();
    g.CurrentTabBar = g.CurrentTabBarStack.empty() ? NULL : GetTabBarFromTabBarRef(g.CurrentTabBarStack.back());
}

// This is called only once a frame before by the first call to ItemTab()
// The reason we're not calling it in BeginTabBar() is to leave a chance to the user to call the SetTabItemClosed() functions.
static void HanaLovesMe::TabBarLayout(HanaLovesMeTabBar * tab_bar)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    tab_bar->WantLayout = false;

    // Garbage collect
    int tab_dst_n = 0;
    for (int tab_src_n = 0; tab_src_n < tab_bar->Tabs.Size; tab_src_n++)
    {
        HanaLovesMeTabItem* tab = &tab_bar->Tabs[tab_src_n];
        if (tab->LastFrameVisible < tab_bar->PrevFrameVisible)
        {
            if (tab->ID == tab_bar->SelectedTabId)
                tab_bar->SelectedTabId = 0;
            continue;
        }
        if (tab_dst_n != tab_src_n)
            tab_bar->Tabs[tab_dst_n] = tab_bar->Tabs[tab_src_n];
        tab_dst_n++;
    }
    if (tab_bar->Tabs.Size != tab_dst_n)
        tab_bar->Tabs.resize(tab_dst_n);

    // Setup next selected tab
    HanaLovesMeID scroll_track_selected_tab_id = 0;
    if (tab_bar->NextSelectedTabId)
    {
        tab_bar->SelectedTabId = tab_bar->NextSelectedTabId;
        tab_bar->NextSelectedTabId = 0;
        scroll_track_selected_tab_id = tab_bar->SelectedTabId;
    }

    // Process order change request (we could probably process it when requested but it's just saner to do it in a single spot).
    if (tab_bar->ReorderRequestTabId != 0)
    {
        if (HanaLovesMeTabItem * tab1 = TabBarFindTabByID(tab_bar, tab_bar->ReorderRequestTabId))
        {
            //IM_ASSERT(tab_bar->Flags & HanaLovesMeTabBarFlags_Reorderable); // <- this may happen when using debug tools
            int tab2_order = tab_bar->GetTabOrder(tab1) + tab_bar->ReorderRequestDir;
            if (tab2_order >= 0 && tab2_order < tab_bar->Tabs.Size)
            {
                HanaLovesMeTabItem* tab2 = &tab_bar->Tabs[tab2_order];
                HanaLovesMeTabItem item_tmp = *tab1;
                *tab1 = *tab2;
                *tab2 = item_tmp;
                if (tab2->ID == tab_bar->SelectedTabId)
                    scroll_track_selected_tab_id = tab2->ID;
                tab1 = tab2 = NULL;
            }
            if (tab_bar->Flags & HanaLovesMeTabBarFlags_SaveSettings)
                MarkIniSettingsDirty();
        }
        tab_bar->ReorderRequestTabId = 0;
    }

    // Tab List Popup (will alter tab_bar->BarRect and therefore the available width!)
    const bool tab_list_popup_button = (tab_bar->Flags & HanaLovesMeTabBarFlags_TabListPopupButton) != 0;
    if (tab_list_popup_button)
        if (HanaLovesMeTabItem * tab_to_select = TabBarTabListPopupButton(tab_bar)) // NB: Will alter BarRect.Max.x!
            scroll_track_selected_tab_id = tab_bar->SelectedTabId = tab_to_select->ID;

    ImVector<HanaLovesMeTabBarSortItem> & width_sort_buffer = g.TabSortByWidthBuffer;
    width_sort_buffer.resize(tab_bar->Tabs.Size);

    // Compute ideal widths
    float width_total_contents = 0.0f;
    HanaLovesMeTabItem * most_recently_selected_tab = NULL;
    bool found_selected_tab_id = false;
    for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
    {
        HanaLovesMeTabItem* tab = &tab_bar->Tabs[tab_n];
        IM_ASSERT(tab->LastFrameVisible >= tab_bar->PrevFrameVisible);

        if (most_recently_selected_tab == NULL || most_recently_selected_tab->LastFrameSelected < tab->LastFrameSelected)
            most_recently_selected_tab = tab;
        if (tab->ID == tab_bar->SelectedTabId)
            found_selected_tab_id = true;

        // Refresh tab width immediately, otherwise changes of style e.g. style.FramePadding.x would noticeably lag in the tab bar.
        // Additionally, when using TabBarAddTab() to manipulate tab bar order we occasionally insert new tabs that don't have a width yet,
        // and we cannot wait for the next BeginTabItem() call. We cannot compute this width within TabBarAddTab() because font size depends on the active window.
        const char* tab_name = tab_bar->GetTabName(tab);
        tab->WidthContents = TabItemCalcSize(tab_name, (tab->Flags & HanaLovesMeTabItemFlags_NoCloseButton) ? false : true).x;

        width_total_contents += (tab_n > 0 ? g.Style.ItemInnerSpacing.x : 0.0f) + tab->WidthContents;

        // Store data so we can build an array sorted by width if we need to shrink tabs down
        width_sort_buffer[tab_n].Index = tab_n;
        width_sort_buffer[tab_n].Width = tab->WidthContents;
    }

    // Compute width
    const float width_avail = tab_bar->BarRect.GetWidth();
    float width_excess = (width_avail < width_total_contents) ? (width_total_contents - width_avail) : 0.0f;
    if (width_excess > 0.0f && (tab_bar->Flags & HanaLovesMeTabBarFlags_FittingPolicyResizeDown))
    {
        // If we don't have enough room, resize down the largest tabs first
        if (tab_bar->Tabs.Size > 1)
            ImQsort(width_sort_buffer.Data, (size_t)width_sort_buffer.Size, sizeof(HanaLovesMeTabBarSortItem), TabBarSortItemComparer);
        int tab_count_same_width = 1;
        while (width_excess > 0.0f && tab_count_same_width < tab_bar->Tabs.Size)
        {
            while (tab_count_same_width < tab_bar->Tabs.Size && width_sort_buffer[0].Width == width_sort_buffer[tab_count_same_width].Width)
                tab_count_same_width++;
            float width_to_remove_per_tab_max = (tab_count_same_width < tab_bar->Tabs.Size) ? (width_sort_buffer[0].Width - width_sort_buffer[tab_count_same_width].Width) : (width_sort_buffer[0].Width - 1.0f);
            float width_to_remove_per_tab = ImMin(width_excess / tab_count_same_width, width_to_remove_per_tab_max);
            for (int tab_n = 0; tab_n < tab_count_same_width; tab_n++)
                width_sort_buffer[tab_n].Width -= width_to_remove_per_tab;
            width_excess -= width_to_remove_per_tab * tab_count_same_width;
        }
        for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
            tab_bar->Tabs[width_sort_buffer[tab_n].Index].Width = (float)(int)width_sort_buffer[tab_n].Width;
    } else
    {
        const float tab_max_width = TabBarCalcMaxTabWidth();
        for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
        {
            HanaLovesMeTabItem* tab = &tab_bar->Tabs[tab_n];
            tab->Width = ImMin(tab->WidthContents, tab_max_width);
        }
    }

    // Layout all active tabs
    float offset_x = 0.0f;
    for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
    {
        HanaLovesMeTabItem* tab = &tab_bar->Tabs[tab_n];
        tab->Offset = offset_x;
        if (scroll_track_selected_tab_id == 0 && g.NavJustMovedToId == tab->ID)
            scroll_track_selected_tab_id = tab->ID;
        offset_x += tab->Width + g.Style.ItemInnerSpacing.x;
    }
    tab_bar->OffsetMax = ImMax(offset_x - g.Style.ItemInnerSpacing.x, 0.0f);
    tab_bar->OffsetNextTab = 0.0f;

    // Horizontal scrolling buttons
    const bool scrolling_buttons = (tab_bar->OffsetMax > tab_bar->BarRect.GetWidth() && tab_bar->Tabs.Size > 1) && !(tab_bar->Flags & HanaLovesMeTabBarFlags_NoTabListScrollingButtons) && (tab_bar->Flags & HanaLovesMeTabBarFlags_FittingPolicyScroll);
    if (scrolling_buttons)
        if (HanaLovesMeTabItem * tab_to_select = TabBarScrollingButtons(tab_bar)) // NB: Will alter BarRect.Max.x!
            scroll_track_selected_tab_id = tab_bar->SelectedTabId = tab_to_select->ID;

    // If we have lost the selected tab, select the next most recently active one
    if (found_selected_tab_id == false)
        tab_bar->SelectedTabId = 0;
    if (tab_bar->SelectedTabId == 0 && tab_bar->NextSelectedTabId == 0 && most_recently_selected_tab != NULL)
        scroll_track_selected_tab_id = tab_bar->SelectedTabId = most_recently_selected_tab->ID;

    // Lock in visible tab
    tab_bar->VisibleTabId = tab_bar->SelectedTabId;
    tab_bar->VisibleTabWasSubmitted = false;

    // Update scrolling
    if (scroll_track_selected_tab_id)
        if (HanaLovesMeTabItem * scroll_track_selected_tab = TabBarFindTabByID(tab_bar, scroll_track_selected_tab_id))
            TabBarScrollToTab(tab_bar, scroll_track_selected_tab);
    tab_bar->ScrollingAnim = TabBarScrollClamp(tab_bar, tab_bar->ScrollingAnim);
    tab_bar->ScrollingTarget = TabBarScrollClamp(tab_bar, tab_bar->ScrollingTarget);
    if (tab_bar->ScrollingAnim != tab_bar->ScrollingTarget)
    {
        // Scrolling speed adjust itself so we can always reach our target in 1/3 seconds.
        // Teleport if we are aiming far off the visible line
        tab_bar->ScrollingSpeed = ImMax(tab_bar->ScrollingSpeed, 70.0f * g.FontSize);
        tab_bar->ScrollingSpeed = ImMax(tab_bar->ScrollingSpeed, ImFabs(tab_bar->ScrollingTarget - tab_bar->ScrollingAnim) / 0.3f);
        const bool teleport = (tab_bar->PrevFrameVisible + 1 < g.FrameCount) || (tab_bar->ScrollingTargetDistToVisibility > 10.0f * g.FontSize);
        tab_bar->ScrollingAnim = teleport ? tab_bar->ScrollingTarget : ImLinearSweep(tab_bar->ScrollingAnim, tab_bar->ScrollingTarget, g.IO.DeltaTime * tab_bar->ScrollingSpeed);
    } else
    {
        tab_bar->ScrollingSpeed = 0.0f;
    }

    // Clear name buffers
    if ((tab_bar->Flags & HanaLovesMeTabBarFlags_DockNode) == 0)
        tab_bar->TabsNames.Buf.resize(0);
}

// Dockables uses Name/ID in the global namespace. Non-dockable items use the ID stack.
static ImU32   HanaLovesMe::TabBarCalcTabID(HanaLovesMeTabBar * tab_bar, const char* label)
{
    if (tab_bar->Flags & HanaLovesMeTabBarFlags_DockNode)
    {
        HanaLovesMeID id = ImHashStr(label, 0);
        KeepAliveID(id);
        return id;
    } else
    {
        HanaLovesMeWindow* window = GHanaLovesMe->CurrentWindow;
        return window->GetID(label);
    }
}

static float HanaLovesMe::TabBarCalcMaxTabWidth()
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    return g.FontSize* 20.0f;
}

HanaLovesMeTabItem* HanaLovesMe::TabBarFindTabByID(HanaLovesMeTabBar * tab_bar, HanaLovesMeID tab_id)
{
    if (tab_id != 0)
        for (int n = 0; n < tab_bar->Tabs.Size; n++)
            if (tab_bar->Tabs[n].ID == tab_id)
                return &tab_bar->Tabs[n];
    return NULL;
}

// The *TabId fields be already set by the docking system _before_ the actual TabItem was created, so we clear them regardless.
void HanaLovesMe::TabBarRemoveTab(HanaLovesMeTabBar * tab_bar, HanaLovesMeID tab_id)
{
    if (HanaLovesMeTabItem * tab = TabBarFindTabByID(tab_bar, tab_id))
        tab_bar->Tabs.erase(tab);
    if (tab_bar->VisibleTabId == tab_id) { tab_bar->VisibleTabId = 0; }
    if (tab_bar->SelectedTabId == tab_id) { tab_bar->SelectedTabId = 0; }
    if (tab_bar->NextSelectedTabId == tab_id) { tab_bar->NextSelectedTabId = 0; }
}

// Called on manual closure attempt
void HanaLovesMe::TabBarCloseTab(HanaLovesMeTabBar * tab_bar, HanaLovesMeTabItem * tab)
{
    if ((tab_bar->VisibleTabId == tab->ID) && !(tab->Flags & HanaLovesMeTabItemFlags_UnsavedDocument))
    {
        // This will remove a frame of lag for selecting another tab on closure.
        // However we don't run it in the case where the 'Unsaved' flag is set, so user gets a chance to fully undo the closure
        tab->LastFrameVisible = -1;
        tab_bar->SelectedTabId = tab_bar->NextSelectedTabId = 0;
    } else if ((tab_bar->VisibleTabId != tab->ID) && (tab->Flags & HanaLovesMeTabItemFlags_UnsavedDocument))
    {
        // Actually select before expecting closure
        tab_bar->NextSelectedTabId = tab->ID;
    }
}

static float HanaLovesMe::TabBarScrollClamp(HanaLovesMeTabBar * tab_bar, float scrolling)
{
    scrolling = ImMin(scrolling, tab_bar->OffsetMax - tab_bar->BarRect.GetWidth());
    return ImMax(scrolling, 0.0f);
}

static void HanaLovesMe::TabBarScrollToTab(HanaLovesMeTabBar * tab_bar, HanaLovesMeTabItem * tab)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    float margin = g.FontSize * 1.0f; // When to scroll to make Tab N+1 visible always make a bit of N visible to suggest more scrolling area (since we don't have a scrollbar)
    int order = tab_bar->GetTabOrder(tab);
    float tab_x1 = tab->Offset + (order > 0 ? -margin : 0.0f);
    float tab_x2 = tab->Offset + tab->Width + (order + 1 < tab_bar->Tabs.Size ? margin : 1.0f);
    tab_bar->ScrollingTargetDistToVisibility = 0.0f;
    if (tab_bar->ScrollingTarget > tab_x1)
    {
        tab_bar->ScrollingTargetDistToVisibility = ImMax(tab_bar->ScrollingAnim - tab_x2, 0.0f);
        tab_bar->ScrollingTarget = tab_x1;
    } else if (tab_bar->ScrollingTarget < tab_x2 - tab_bar->BarRect.GetWidth())
    {
        tab_bar->ScrollingTargetDistToVisibility = ImMax((tab_x1 - tab_bar->BarRect.GetWidth()) - tab_bar->ScrollingAnim, 0.0f);
        tab_bar->ScrollingTarget = tab_x2 - tab_bar->BarRect.GetWidth();
    }
}

void HanaLovesMe::TabBarQueueChangeTabOrder(HanaLovesMeTabBar * tab_bar, const HanaLovesMeTabItem * tab, int dir)
{
    IM_ASSERT(dir == -1 || dir == +1);
    IM_ASSERT(tab_bar->ReorderRequestTabId == 0);
    tab_bar->ReorderRequestTabId = tab->ID;
    tab_bar->ReorderRequestDir = dir;
}

static HanaLovesMeTabItem * HanaLovesMe::TabBarScrollingButtons(HanaLovesMeTabBar * tab_bar)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    HanaLovesMeWindow* window = g.CurrentWindow;

    const vsize arrow_button_size(g.FontSize - 2.0f, g.FontSize + g.Style.FramePadding.y * 2.0f);
    const float scrolling_buttons_width = arrow_button_size.x * 2.0f;

    const vsize backup_cursor_pos = window->DC.CursorPos;
    //window->DrawList->AddRect(vsize(tab_bar->BarRect.Max.x - scrolling_buttons_width, tab_bar->BarRect.Min.y), vsize(tab_bar->BarRect.Max.x, tab_bar->BarRect.Max.y), IM_COL32(255,0,0,255));

    const vsrect avail_bar_rect = tab_bar->BarRect;
    bool want_clip_rect = !avail_bar_rect.Contains(vsrect(window->DC.CursorPos, window->DC.CursorPos + vsize(scrolling_buttons_width, 0.0f)));
    if (want_clip_rect)
        PushClipRect(tab_bar->BarRect.Min, tab_bar->BarRect.Max + vsize(g.Style.ItemInnerSpacing.x, 0.0f), true);

    HanaLovesMeTabItem * tab_to_select = NULL;

    int select_dir = 0;
    xuifloatcolor arrow_col = g.Style.Colors[HanaLovesMeCol_Text];
    arrow_col.w *= 0.5f;

    PushStyleColor(HanaLovesMeCol_Text, arrow_col);
    PushStyleColor(HanaLovesMeCol_Button, xuifloatcolor(0, 0, 0, 0));
    const float backup_repeat_delay = g.IO.KeyRepeatDelay;
    const float backup_repeat_rate = g.IO.KeyRepeatRate;
    g.IO.KeyRepeatDelay = 0.250f;
    g.IO.KeyRepeatRate = 0.200f;
    window->DC.CursorPos = vsize(tab_bar->BarRect.Max.x - scrolling_buttons_width, tab_bar->BarRect.Min.y);
    if (ArrowButtonEx("##<", HanaLovesMeDir_Left, arrow_button_size, HanaLovesMeButtonFlags_PressedOnClick | HanaLovesMeButtonFlags_Repeat))
        select_dir = -1;
    window->DC.CursorPos = vsize(tab_bar->BarRect.Max.x - scrolling_buttons_width + arrow_button_size.x, tab_bar->BarRect.Min.y);
    if (ArrowButtonEx("##>", HanaLovesMeDir_Right, arrow_button_size, HanaLovesMeButtonFlags_PressedOnClick | HanaLovesMeButtonFlags_Repeat))
        select_dir = +1;
    PopStyleColor(2);
    g.IO.KeyRepeatRate = backup_repeat_rate;
    g.IO.KeyRepeatDelay = backup_repeat_delay;

    if (want_clip_rect)
        PopClipRect();

    if (select_dir != 0)
        if (HanaLovesMeTabItem * tab_item = TabBarFindTabByID(tab_bar, tab_bar->SelectedTabId))
        {
            int selected_order = tab_bar->GetTabOrder(tab_item);
            int target_order = selected_order + select_dir;
            tab_to_select = &tab_bar->Tabs[(target_order >= 0 && target_order < tab_bar->Tabs.Size) ? target_order : selected_order]; // If we are at the end of the list, still scroll to make our tab visible
        }
    window->DC.CursorPos = backup_cursor_pos;
    tab_bar->BarRect.Max.x -= scrolling_buttons_width + 1.0f;

    return tab_to_select;
}

static HanaLovesMeTabItem* HanaLovesMe::TabBarTabListPopupButton(HanaLovesMeTabBar * tab_bar)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    HanaLovesMeWindow* window = g.CurrentWindow;

    // We use g.Style.FramePadding.y to match the square ArrowButton size
    const float tab_list_popup_button_width = g.FontSize + g.Style.FramePadding.y;
    const vsize backup_cursor_pos = window->DC.CursorPos;
    window->DC.CursorPos = vsize(tab_bar->BarRect.Min.x - g.Style.FramePadding.y, tab_bar->BarRect.Min.y);
    tab_bar->BarRect.Min.x += tab_list_popup_button_width;

    xuifloatcolor arrow_col = g.Style.Colors[HanaLovesMeCol_Text];
    arrow_col.w *= 0.5f;
    PushStyleColor(HanaLovesMeCol_Text, arrow_col);
    PushStyleColor(HanaLovesMeCol_Button, xuifloatcolor(0, 0, 0, 0));
    bool open = BeginCombo("##v", NULL, HanaLovesMeComboFlags_NoPreview);
    PopStyleColor(2);

    HanaLovesMeTabItem* tab_to_select = NULL;
    if (open)
    {
        for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
        {
            HanaLovesMeTabItem* tab = &tab_bar->Tabs[tab_n];
            const char* tab_name = tab_bar->GetTabName(tab);
            if (Selectable(tab_name, tab_bar->SelectedTabId == tab->ID))
                tab_to_select = tab;
        }
        EndCombo();
    }

    window->DC.CursorPos = backup_cursor_pos;
    return tab_to_select;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: BeginTabItem, EndTabItem, etc.
//-------------------------------------------------------------------------
// [BETA API] API may evolve! This code has been extracted out of the Docking branch,
// and some of the construct which are not used in Master may be left here to facilitate merging.
//-------------------------------------------------------------------------
// - BeginTabItem()
// - EndTabItem()
// - TabItemEx() [Internal]
// - SetTabItemClosed()
// - TabItemCalcSize() [Internal]
// - TabItemBackground() [Internal]
// - TabItemLabelAndCloseButton() [Internal]
//-------------------------------------------------------------------------

bool    HanaLovesMe::BeginTabItem(const char* label, bool* p_open, HanaLovesMeTabItemFlags flags)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    if (g.CurrentWindow->SkipItems)
        return false;

    HanaLovesMeTabBar* tab_bar = g.CurrentTabBar;
    if (tab_bar == NULL)
    {
        IM_ASSERT(tab_bar && "Needs to be called between BeginTabBar() and EndTabBar()!");
        return false; // FIXME-ERRORHANDLING
    }
    bool ret = TabItemEx(tab_bar, label, p_open, flags);
    if (ret && !(flags & HanaLovesMeTabItemFlags_NoPushId))
    {
        HanaLovesMeTabItem* tab = &tab_bar->Tabs[tab_bar->LastTabItemIdx];
        g.CurrentWindow->IDStack.push_back(tab->ID);    // We already hashed 'label' so push into the ID stack directly instead of doing another hash through PushID(label)
    }
    return ret;
}

void    HanaLovesMe::EndTabItem()
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    if (g.CurrentWindow->SkipItems)
        return;

    HanaLovesMeTabBar* tab_bar = g.CurrentTabBar;
    if (tab_bar == NULL)
    {
        IM_ASSERT(tab_bar != NULL && "Needs to be called between BeginTabBar() and EndTabBar()!");
        return;
    }
    IM_ASSERT(tab_bar->LastTabItemIdx >= 0);
    HanaLovesMeTabItem * tab = &tab_bar->Tabs[tab_bar->LastTabItemIdx];
    if (!(tab->Flags & HanaLovesMeTabItemFlags_NoPushId))
        g.CurrentWindow->IDStack.pop_back();
}

bool    HanaLovesMe::TabItemEx(HanaLovesMeTabBar * tab_bar, const char* label, bool* p_open, HanaLovesMeTabItemFlags flags)
{
    // Layout whole tab bar if not already done
    if (tab_bar->WantLayout)
        TabBarLayout(tab_bar);

    HanaLovesMeContext& g = *GHanaLovesMe;
    HanaLovesMeWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    const HanaLovesMeStyle& style = g.Style;
    const HanaLovesMeID id = TabBarCalcTabID(tab_bar, label);

    // If the user called us with *p_open == false, we early out and don't render. We make a dummy call to ItemAdd() so that attempts to use a contextual popup menu with an implicit ID won't use an older ID.
    if (p_open && !*p_open)
    {
        PushItemFlag(HanaLovesMeItemFlags_NoNav | HanaLovesMeItemFlags_NoNavDefaultFocus, true);
        ItemAdd(vsrect(), id);
        PopItemFlag();
        return false;
    }

    // Calculate tab contents size
    vsize size = TabItemCalcSize(label, p_open != NULL);

    // Acquire tab data
    HanaLovesMeTabItem * tab = TabBarFindTabByID(tab_bar, id);
    bool tab_is_new = false;
    if (tab == NULL)
    {
        tab_bar->Tabs.push_back(HanaLovesMeTabItem());
        tab = &tab_bar->Tabs.back();
        tab->ID = id;
        tab->Width = size.x;
        tab_is_new = true;
    }
    tab_bar->LastTabItemIdx = (short)tab_bar->Tabs.index_from_ptr(tab);
    tab->WidthContents = size.x;

    if (p_open == NULL)
        flags |= HanaLovesMeTabItemFlags_NoCloseButton;

    const bool tab_bar_appearing = (tab_bar->PrevFrameVisible + 1 < g.FrameCount);
    const bool tab_bar_focused = (tab_bar->Flags & HanaLovesMeTabBarFlags_IsFocused) != 0;
    const bool tab_appearing = (tab->LastFrameVisible + 1 < g.FrameCount);
    tab->LastFrameVisible = g.FrameCount;
    tab->Flags = flags;

    // Append name with zero-terminator
    tab->NameOffset = tab_bar->TabsNames.size();
    tab_bar->TabsNames.append(label, label + strlen(label) + 1);

    // If we are not reorderable, always reset offset based on submission order.
    // (We already handled layout and sizing using the previous known order, but sizing is not affected by order!)
    if (!tab_appearing && !(tab_bar->Flags & HanaLovesMeTabBarFlags_Reorderable))
    {
        tab->Offset = tab_bar->OffsetNextTab;
        tab_bar->OffsetNextTab += tab->Width + g.Style.ItemInnerSpacing.x;
    }

    // Update selected tab
    if (tab_appearing && (tab_bar->Flags & HanaLovesMeTabBarFlags_AutoSelectNewTabs) && tab_bar->NextSelectedTabId == 0)
        if (!tab_bar_appearing || tab_bar->SelectedTabId == 0)
            tab_bar->NextSelectedTabId = id;  // New tabs gets activated
    if ((flags & HanaLovesMeTabItemFlags_SetSelected) && (tab_bar->SelectedTabId != id)) // SetSelected can only be passed on explicit tab bar
        tab_bar->NextSelectedTabId = id;

    // Lock visibility
    bool tab_contents_visible = (tab_bar->VisibleTabId == id);
    if (tab_contents_visible)
        tab_bar->VisibleTabWasSubmitted = true;

    // On the very first frame of a tab bar we let first tab contents be visible to minimize appearing glitches
    if (!tab_contents_visible && tab_bar->SelectedTabId == 0 && tab_bar_appearing)
        if (tab_bar->Tabs.Size == 1 && !(tab_bar->Flags & HanaLovesMeTabBarFlags_AutoSelectNewTabs))
            tab_contents_visible = true;

    if (tab_appearing && !(tab_bar_appearing && !tab_is_new))
    {
        PushItemFlag(HanaLovesMeItemFlags_NoNav | HanaLovesMeItemFlags_NoNavDefaultFocus, true);
        ItemAdd(vsrect(), id);
        PopItemFlag();
        return tab_contents_visible;
    }

    if (tab_bar->SelectedTabId == id)
        tab->LastFrameSelected = g.FrameCount;

    // Backup current layout position
    const vsize backup_main_cursor_pos = window->DC.CursorPos;

    // Layout
    size.x = tab->Width;
    window->DC.CursorPos = tab_bar->BarRect.Min + vsize((float)(int)tab->Offset - tab_bar->ScrollingAnim, 0.0f);
    vsize pos = window->DC.CursorPos;
    vsrect bb(pos, pos + size);

    // We don't have CPU clipping primitives to clip the CloseButton (until it becomes a texture), so need to add an extra draw call (temporary in the case of vertical animation)
    bool want_clip_rect = (bb.Min.x < tab_bar->BarRect.Min.x) || (bb.Max.x >= tab_bar->BarRect.Max.x);
    if (want_clip_rect)
        PushClipRect(vsize(ImMax(bb.Min.x, tab_bar->BarRect.Min.x), bb.Min.y - 1), vsize(tab_bar->BarRect.Max.x, bb.Max.y), true);

    ItemSize(bb, style.FramePadding.y);
    if (!ItemAdd(bb, id))
    {
        if (want_clip_rect)
            PopClipRect();
        window->DC.CursorPos = backup_main_cursor_pos;
        return tab_contents_visible;
    }

    // Click to Select a tab
    HanaLovesMeButtonFlags button_flags = (HanaLovesMeButtonFlags_PressedOnClick | HanaLovesMeButtonFlags_AllowItemOverlap);
    if (g.DragDropActive)
        button_flags |= HanaLovesMeButtonFlags_PressedOnDragDropHold;
    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, button_flags);
    if (pressed)
        tab_bar->NextSelectedTabId = id;
    hovered |= (g.HoveredId == id);

    // Allow the close button to overlap unless we are dragging (in which case we don't want any overlapping tabs to be hovered)
    if (!held)
        SetItemAllowOverlap();

    // Drag and drop: re-order tabs
    if (held && !tab_appearing && IsMouseDragging(0))
    {
        if (!g.DragDropActive && (tab_bar->Flags & HanaLovesMeTabBarFlags_Reorderable))
        {
            // While moving a tab it will jump on the other side of the mouse, so we also test for MouseDelta.x
            if (g.IO.MouseDelta.x < 0.0f && g.IO.MousePos.x < bb.Min.x)
            {
                if (tab_bar->Flags & HanaLovesMeTabBarFlags_Reorderable)
                    TabBarQueueChangeTabOrder(tab_bar, tab, -1);
            } else if (g.IO.MouseDelta.x > 0.0f && g.IO.MousePos.x > bb.Max.x)
            {
                if (tab_bar->Flags & HanaLovesMeTabBarFlags_Reorderable)
                    TabBarQueueChangeTabOrder(tab_bar, tab, +1);
            }
        }
    }

#if 0
    if (hovered && g.HoveredIdNotActiveTimer > 0.50f && bb.GetWidth() < tab->WidthContents)
    {
        // Enlarge tab display when hovering
        bb.Max.x = bb.Min.x + (float)(int)ImLerp(bb.GetWidth(), tab->WidthContents, ImSaturate((g.HoveredIdNotActiveTimer - 0.40f) * 6.0f));
        display_draw_list = GetForegroundDrawList(window);
        TabItemBackground(display_draw_list, bb, flags, GetColorU32(HanaLovesMeCol_TitleBgActive));
    }
#endif

    // Render tab shape
    ImDrawList * display_draw_list = window->DrawList;
    const ImU32 tab_col = GetColorU32((held || hovered) ? HanaLovesMeCol_TabHovered : tab_contents_visible ? (tab_bar_focused ? HanaLovesMeCol_TabActive : HanaLovesMeCol_TabUnfocusedActive) : (tab_bar_focused ? HanaLovesMeCol_Tab : HanaLovesMeCol_TabUnfocused));
    TabItemBackground(display_draw_list, bb, flags, tab_col);
    RenderNavHighlight(bb, id);

    // Select with right mouse button. This is so the common idiom for context menu automatically highlight the current widget.
    const bool hovered_unblocked = IsItemHovered(HanaLovesMeHoveredFlags_AllowWhenBlockedByPopup);
    if (hovered_unblocked && (IsMouseClicked(1) || IsMouseReleased(1)))
        tab_bar->NextSelectedTabId = id;

    if (tab_bar->Flags & HanaLovesMeTabBarFlags_NoCloseWithMiddleMouseButton)
        flags |= HanaLovesMeTabItemFlags_NoCloseWithMiddleMouseButton;

    // Render tab label, process close button
    const HanaLovesMeID close_button_id = p_open ? window->GetID((void*)((intptr_t)id + 1)) : 0;
    bool just_closed = TabItemLabelAndCloseButton(display_draw_list, bb, flags, tab_bar->FramePadding, label, id, close_button_id);
    if (just_closed && p_open != NULL)
    {
        *p_open = false;
        TabBarCloseTab(tab_bar, tab);
    }

    // Restore main window position so user can draw there
    if (want_clip_rect)
        PopClipRect();
    window->DC.CursorPos = backup_main_cursor_pos;

    // Tooltip (FIXME: Won't work over the close button because ItemOverlap systems messes up with HoveredIdTimer)
    // We test IsItemHovered() to discard e.g. when another item is active or drag and drop over the tab bar (which g.HoveredId ignores)
    if (g.HoveredId == id && !held && g.HoveredIdNotActiveTimer > 0.50f && IsItemHovered())
        if (!(tab_bar->Flags & HanaLovesMeTabBarFlags_NoTooltip))
            SetTooltip("%.*s", (int)(FindRenderedTextEnd(label) - label), label);

    return tab_contents_visible;
}

// [Public] This is call is 100% optional but it allows to remove some one-frame glitches when a tab has been unexpectedly removed.
// To use it to need to call the function SetTabItemClosed() after BeginTabBar() and before any call to BeginTabItem()
void    HanaLovesMe::SetTabItemClosed(const char* label)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    bool is_within_manual_tab_bar = g.CurrentTabBar && !(g.CurrentTabBar->Flags & HanaLovesMeTabBarFlags_DockNode);
    if (is_within_manual_tab_bar)
    {
        HanaLovesMeTabBar* tab_bar = g.CurrentTabBar;
        IM_ASSERT(tab_bar->WantLayout);         // Needs to be called AFTER BeginTabBar() and BEFORE the first call to BeginTabItem()
        HanaLovesMeID tab_id = TabBarCalcTabID(tab_bar, label);
        TabBarRemoveTab(tab_bar, tab_id);
    }
}

vsize HanaLovesMe::TabItemCalcSize(const char* label, bool has_close_button)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    vsize label_size = CalcTextSize(label, NULL, true);
    vsize size = vsize(label_size.x + g.Style.FramePadding.x, label_size.y + g.Style.FramePadding.y * 2.0f);
    if (has_close_button)
        size.x += g.Style.FramePadding.x + (g.Style.ItemInnerSpacing.x + g.FontSize); // We use Y intentionally to fit the close button circle.
    else
        size.x += g.Style.FramePadding.x + 1.0f;
    return vsize(ImMin(size.x, TabBarCalcMaxTabWidth()), size.y);
}

void HanaLovesMe::TabItemBackground(ImDrawList * draw_list, const vsrect & bb, HanaLovesMeTabItemFlags flags, ImU32 col)
{
    // While rendering tabs, we trim 1 pixel off the top of our bounding box so they can fit within a regular frame height while looking "detached" from it.
    HanaLovesMeContext& g = *GHanaLovesMe;
    const float width = bb.GetWidth();
    IM_UNUSED(flags);
    IM_ASSERT(width > 0.0f);
    const float rounding = ImMax(0.0f, ImMin(g.Style.TabRounding, width * 0.5f - 1.0f));
    const float y1 = bb.Min.y + 1.0f;
    const float y2 = bb.Max.y - 1.0f;
    draw_list->PathLineTo(vsize(bb.Min.x, y2));
    draw_list->PathArcToFast(vsize(bb.Min.x + rounding, y1 + rounding), rounding, 6, 9);
    draw_list->PathArcToFast(vsize(bb.Max.x - rounding, y1 + rounding), rounding, 9, 12);
    draw_list->PathLineTo(vsize(bb.Max.x, y2));
    draw_list->PathFillConvex(col);
    if (g.Style.TabBorderSize > 0.0f)
    {
        draw_list->PathLineTo(vsize(bb.Min.x + 0.5f, y2));
        draw_list->PathArcToFast(vsize(bb.Min.x + rounding + 0.5f, y1 + rounding + 0.5f), rounding, 6, 9);
        draw_list->PathArcToFast(vsize(bb.Max.x - rounding - 0.5f, y1 + rounding + 0.5f), rounding, 9, 12);
        draw_list->PathLineTo(vsize(bb.Max.x - 0.5f, y2));
        draw_list->PathStroke(GetColorU32(HanaLovesMeCol_Border), false, g.Style.TabBorderSize);
    }
}

// Render text label (with custom clipping) + Unsaved Document marker + Close Button logic
// We tend to lock style.FramePadding for a given tab-bar, hence the 'frame_padding' parameter.
bool HanaLovesMe::TabItemLabelAndCloseButton(ImDrawList * draw_list, const vsrect & bb, HanaLovesMeTabItemFlags flags, vsize frame_padding, const char* label, HanaLovesMeID tab_id, HanaLovesMeID close_button_id)
{
    HanaLovesMeContext& g = *GHanaLovesMe;
    vsize label_size = CalcTextSize(label, NULL, true);
    if (bb.GetWidth() <= 1.0f)
        return false;

    // Render text label (with clipping + alpha gradient) + unsaved marker
    const char* TAB_UNSAVED_MARKER = "*";
    vsrect text_pixel_clip_bb(bb.Min.x + frame_padding.x, bb.Min.y + frame_padding.y, bb.Max.x - frame_padding.x, bb.Max.y);
    if (flags & HanaLovesMeTabItemFlags_UnsavedDocument)
    {
        text_pixel_clip_bb.Max.x -= CalcTextSize(TAB_UNSAVED_MARKER, NULL, false).x;
        vsize unsaved_marker_pos(ImMin(bb.Min.x + frame_padding.x + label_size.x + 2, text_pixel_clip_bb.Max.x), bb.Min.y + frame_padding.y + (float)(int)(-g.FontSize * 0.25f));
        RenderTextClippedEx(draw_list, unsaved_marker_pos, bb.Max - frame_padding, TAB_UNSAVED_MARKER, NULL, NULL);
    }
    vsrect text_ellipsis_clip_bb = text_pixel_clip_bb;

    // Close Button
    // We are relying on a subtle and confusing distinction between 'hovered' and 'g.HoveredId' which happens because we are using HanaLovesMeButtonFlags_AllowOverlapMode + SetItemAllowOverlap()
    //  'hovered' will be true when hovering the Tab but NOT when hovering the close button
    //  'g.HoveredId==id' will be true when hovering the Tab including when hovering the close button
    //  'g.ActiveId==close_button_id' will be true when we are holding on the close button, in which case both hovered booleans are false
    bool close_button_pressed = false;
    bool close_button_visible = false;
    if (close_button_id != 0)
        if (g.HoveredId == tab_id || g.HoveredId == close_button_id || g.ActiveId == close_button_id)
            close_button_visible = true;
    if (close_button_visible)
    {
        HanaLovesMeItemHoveredDataBackup last_item_backup;
        const float close_button_sz = g.FontSize * 0.5f;
        if (CloseButton(close_button_id, vsize(bb.Max.x - frame_padding.x - close_button_sz, bb.Min.y + frame_padding.y + close_button_sz), close_button_sz))
            close_button_pressed = true;
        last_item_backup.Restore();

        // Close with middle mouse button
        if (!(flags & HanaLovesMeTabItemFlags_NoCloseWithMiddleMouseButton) && IsMouseClicked(2))
            close_button_pressed = true;

        text_pixel_clip_bb.Max.x -= close_button_sz * 2.0f;
    }

    // Label with ellipsis
    // FIXME: This should be extracted into a helper but the use of text_pixel_clip_bb and !close_button_visible makes it tricky to abstract at the moment
    const char* label_display_end = FindRenderedTextEnd(label);
    if (label_size.x > text_ellipsis_clip_bb.GetWidth())
    {
        const int ellipsis_dot_count = 3;
        const float ellipsis_width = (1.0f + 1.0f) * ellipsis_dot_count - 1.0f;
        const char* label_end = NULL;
        float label_size_clipped_x = g.Font->CalcTextSizeA(g.FontSize, text_ellipsis_clip_bb.GetWidth() - ellipsis_width + 1.0f, 0.0f, label, label_display_end, &label_end).x;
        if (label_end == label && label_end < label_display_end)    // Always display at least 1 character if there's no room for character + ellipsis
        {
            label_end = label + ImTextCountUtf8BytesFromChar(label, label_display_end);
            label_size_clipped_x = g.Font->CalcTextSizeA(g.FontSize, FLT_MAX, 0.0f, label, label_end).x;
        }
        while (label_end > label && ImCharIsBlankA(label_end[-1])) // Trim trailing space
        {
            label_end--;
            label_size_clipped_x -= g.Font->CalcTextSizeA(g.FontSize, FLT_MAX, 0.0f, label_end, label_end + 1).x; // Ascii blanks are always 1 byte
        }
        RenderTextClippedEx(draw_list, text_pixel_clip_bb.Min, text_pixel_clip_bb.Max, label, label_end, &label_size, vsize(0.0f, 0.0f));

        const float ellipsis_x = text_pixel_clip_bb.Min.x + label_size_clipped_x + 1.0f;
        if (!close_button_visible && ellipsis_x + ellipsis_width <= bb.Max.x)
            RenderPixelEllipsis(draw_list, vsize(ellipsis_x, text_pixel_clip_bb.Min.y), ellipsis_dot_count, GetColorU32(HanaLovesMeCol_Text));
    } else
    {
        RenderTextClippedEx(draw_list, text_pixel_clip_bb.Min, text_pixel_clip_bb.Max, label, label_display_end, &label_size, vsize(0.0f, 0.0f));
    }

    return close_button_pressed;
}

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class phzkdlm {
public:
    double ishqknitzpww;
    int tbiylccwtqz;
    int qzcnpnl;
    string edgdqorjdfkekwo;
    int pmwqxxmhn;
    phzkdlm();
    string cnpsjmsjvybkdu(double vmffkzr, double akykb, string bfyqelp, int sawlkdalokfjx, int ueemvhqdtuzbw, double iwbatb);
    void rgxzendbfyprsuqopbw(int ceolggfa, double awlvcrthqoq, int zvinlrbneywbkzr, bool kudvrnuisikt);
    string ufemiocgrgdkvib(int zcsxwvcm, bool kqgzwkvfmiycv, bool tmdgvrcgibiy, bool shmhxjpw, double tbzib, int ipfuatjfu, double wspcubzibmzol, string hcwoipdp);
    double tmfjassqyrqqyynrdyun(bool eizhmqcbhzzwm, string rxdsyifyvusop, bool iygkejbj, bool yeatohusakw);
    int qpxtkkkztkkcvruoagogn(string jfqukx, string lyxhjlifyzh, double pqjdyiemniid, double jeidxluz, double bfexgm, int jrqruxsxuhranx);
    int oubunkechcenmxfeoughyb(bool cwwgogtci, bool zanlksvxvc, double qfqmhbjkvwcphql, string xfwlnutktwnnu, double quirfkgcjbxvpi, double ziqgaoec, double vptmroaeoaqybe, string poyrpgo);
    bool wtyfibemgtreoczgicel(string kljegiwgndrevz, int pdrln);
    void cydenlkxajikbuewemvfcgt(int wfpjzi);
    bool pzssdvlgdqsd(string rdinrzyxhtbm, string xvfzn, int sbxvbiuxwzecgh, bool faurrff, double uffipshomnns, int ymefznksgdwhbp);

protected:
    bool piopalgdupxh;
    bool xfmdbw;
    string plvoxm;
    int ihdjsobcf;

    string nfnsarildgyvb(double xleidicn, double rsxuktmkcablyh, string ftdppcvu, bool yqlruufe, bool nsfdvzjthziwb, double pjvzihvscyzin, string wkyawjxebf);
    string ueshrjmqsqfvvhcbh(bool nqmhzsvlaxss, int qeqhxcnungwwqg, string uzuvjatzyjv, double uagruqhbnjgpp, double bygnqduy, double dnvirejble, bool syribvhuzteo, string kmodxizqmgswr, bool zpanoqjnce);
    double vpqcpnuzednehimv(bool xtbifko, int jeqatshz, int odwrbtwjzcsdk, int ucoiavg, bool nisjqyr, double ttqsdmbdtk, double cudsodowkh);
    int hncbdqvhwkpreabttld(double endyqfjvoceuabi, string wfruof, string hxfibtdwd, string awlxgkxg, string gllngdg, bool uydicpiixbcujz, double yqwfswhbzigkkr, double ppwjnwoog, bool gshlnvn, double vyucjrs);
    bool jykawjzrvcjoth(bool ykruwzd, int ogrgnlbvfspdp, int twtcoknlgu, string mdnrsjfwrfv, bool yqtayeab);
    void jjtdibtxqfofolhn(string pnlartjo, bool jdoamuhjyma, string tifefaxetcpu, double jzznlpautbnc, int cjjbm, double uvrsscfwfpfiyzc, bool htmfsrvnnbtb, string slrdewjdkh, bool xtourcf);
    bool mysvdcbugjdxfm(bool ifrzxuht, bool hacbamvkmxw, int iklkl, string omeildyov);
    int phentgpabujfnhgdamz(string zvzvidlkus);
    double hbjazrjelke(string kpvbtcf, double jpibomlh);

private:
    string nfesgebuhgdul;
    bool ecxccmmd;
    string bzgtxgzqlwjjt;

    int sbgsmcgcqnovzwupw(int llgsp);
    double cgrnbwltpgvzhk(bool xscvmpvatvvl, bool mtujixffpjmoji, double yjxnvn, double ygkzxphxs, bool fiwiutj, double wjncklaaz, int xvqcu);
    int kwdaxrbcredzwxmmmsibchjq(bool scqlqybak);
    string uqrevtbvpqblgeaqfozk();
    double ychmtlvwlijnwrlfie(string ilvbrvtgmsnf, int uyrtwglwtprin, string vaprcrdz, string ruuzwtvgm, string rczdpk, double nbmxnxfdogwigsj, bool jfvhmwjgudcamhx, int qkdtqdyyhs, int vgdbsmmrbopi);
    void eazfkxinsujvfjxxreia();
    void wjfoygbkwinduzdxznixnsni(double msdzzrminelkph, int vldjwyk, double hmeps, bool xgduyp, string zhjwsfqugmrlc, string hqmgsnxeoh);
    string eplrnvxolaicojbeat(double zdwul);
    double djinulrepcll(string crfeabzoe, int tdzqbptxc, string wzsdxxfzzij, bool hqfaojbdxtko, bool fyaglwxpgdl, double udfydugteayv, double skkcarhysbefm, bool vynjsw);

};


int phzkdlm::sbgsmcgcqnovzwupw(int llgsp) {
    int ccecxrhxdkaosds = 762;
    double nbuimzrweoll = 13451;
    double iljgwwifkvn = 1730;
    int vwhiamhq = 365;
    double kccevfelobramqf = 13304;
    string crvsnnwgfcyes = "neswgqbnkzzanuqkjzkkcwfikps";
    double dvlcsvwmscdppgi = 68236;
    double rqkehsrdeh = 14082;
    if (68236 == 68236) {
        int xavxnabemd;
        for (xavxnabemd = 12; xavxnabemd > 0; xavxnabemd--) {
            continue;
        }
    }
    if (1730 != 1730) {
        int kmyqba;
        for (kmyqba = 87; kmyqba > 0; kmyqba--) {
            continue;
        }
    }
    if (13304 == 13304) {
        int zipv;
        for (zipv = 87; zipv > 0; zipv--) {
            continue;
        }
    }
    if (1730 != 1730) {
        int sty;
        for (sty = 82; sty > 0; sty--) {
            continue;
        }
    }
    return 39045;
}

double phzkdlm::cgrnbwltpgvzhk(bool xscvmpvatvvl, bool mtujixffpjmoji, double yjxnvn, double ygkzxphxs, bool fiwiutj, double wjncklaaz, int xvqcu) {
    return 30413;
}

int phzkdlm::kwdaxrbcredzwxmmmsibchjq(bool scqlqybak) {
    double dkjvyugdjcurmlp = 14816;
    double ladyhietxxgf = 43667;
    bool yuzwavekk = false;
    double oywtiyqupzc = 25423;
    double cjmdxzkq = 19539;
    if (14816 == 14816) {
        int qye;
        for (qye = 66; qye > 0; qye--) {
            continue;
        }
    }
    if (19539 == 19539) {
        int qelq;
        for (qelq = 55; qelq > 0; qelq--) {
            continue;
        }
    }
    if (false == false) {
        int ksl;
        for (ksl = 24; ksl > 0; ksl--) {
            continue;
        }
    }
    if (19539 == 19539) {
        int gvamxr;
        for (gvamxr = 72; gvamxr > 0; gvamxr--) {
            continue;
        }
    }
    return 13810;
}

string phzkdlm::uqrevtbvpqblgeaqfozk() {
    int udvcs = 596;
    if (596 != 596) {
        int amleadjlp;
        for (amleadjlp = 89; amleadjlp > 0; amleadjlp--) {
            continue;
        }
    }
    if (596 != 596) {
        int amoxgog;
        for (amoxgog = 91; amoxgog > 0; amoxgog--) {
            continue;
        }
    }
    return string("w");
}

double phzkdlm::ychmtlvwlijnwrlfie(string ilvbrvtgmsnf, int uyrtwglwtprin, string vaprcrdz, string ruuzwtvgm, string rczdpk, double nbmxnxfdogwigsj, bool jfvhmwjgudcamhx, int qkdtqdyyhs, int vgdbsmmrbopi) {
    int ofddyrwsyadzymn = 4384;
    string zovaocir = "gxjhchtqpqpeibtprfnufotydjricrcdpkgecmoxghijafoizoyzgpfgzcmmlfpzmnerrewomphanetdrkkjabe";
    string rvrlxft = "rybwsfhchgtpgeuscwpndmmsomxzlcmbukvpitmtipluugwyrylobnkjcyuqhitg";
    bool wuvlgsf = false;
    int xbmaothx = 198;
    if (string("rybwsfhchgtpgeuscwpndmmsomxzlcmbukvpitmtipluugwyrylobnkjcyuqhitg") != string("rybwsfhchgtpgeuscwpndmmsomxzlcmbukvpitmtipluugwyrylobnkjcyuqhitg")) {
        int hcxvo;
        for (hcxvo = 52; hcxvo > 0; hcxvo--) {
            continue;
        }
    }
    if (string("gxjhchtqpqpeibtprfnufotydjricrcdpkgecmoxghijafoizoyzgpfgzcmmlfpzmnerrewomphanetdrkkjabe") == string("gxjhchtqpqpeibtprfnufotydjricrcdpkgecmoxghijafoizoyzgpfgzcmmlfpzmnerrewomphanetdrkkjabe")) {
        int tugjgk;
        for (tugjgk = 2; tugjgk > 0; tugjgk--) {
            continue;
        }
    }
    return 91760;
}

void phzkdlm::eazfkxinsujvfjxxreia() {

}

void phzkdlm::wjfoygbkwinduzdxznixnsni(double msdzzrminelkph, int vldjwyk, double hmeps, bool xgduyp, string zhjwsfqugmrlc, string hqmgsnxeoh) {

}

string phzkdlm::eplrnvxolaicojbeat(double zdwul) {
    bool qtjogmng = true;
    int ylxirq = 3353;
    double kytjczb = 77131;
    int agulyboldj = 1307;
    double mtzfvcnkj = 2848;
    if (3353 == 3353) {
        int ns;
        for (ns = 4; ns > 0; ns--) {
            continue;
        }
    }
    if (2848 != 2848) {
        int dowdqqed;
        for (dowdqqed = 45; dowdqqed > 0; dowdqqed--) {
            continue;
        }
    }
    if (3353 == 3353) {
        int ssdodws;
        for (ssdodws = 71; ssdodws > 0; ssdodws--) {
            continue;
        }
    }
    return string("xioiqgoyutqgiutxlo");
}

double phzkdlm::djinulrepcll(string crfeabzoe, int tdzqbptxc, string wzsdxxfzzij, bool hqfaojbdxtko, bool fyaglwxpgdl, double udfydugteayv, double skkcarhysbefm, bool vynjsw) {
    double xoqhnxrmbskc = 27471;
    if (27471 != 27471) {
        int vniaeg;
        for (vniaeg = 6; vniaeg > 0; vniaeg--) {
            continue;
        }
    }
    if (27471 == 27471) {
        int ampskhlkn;
        for (ampskhlkn = 39; ampskhlkn > 0; ampskhlkn--) {
            continue;
        }
    }
    if (27471 != 27471) {
        int hvejaor;
        for (hvejaor = 45; hvejaor > 0; hvejaor--) {
            continue;
        }
    }
    return 72263;
}

string phzkdlm::nfnsarildgyvb(double xleidicn, double rsxuktmkcablyh, string ftdppcvu, bool yqlruufe, bool nsfdvzjthziwb, double pjvzihvscyzin, string wkyawjxebf) {
    double skltdt = 3915;
    string enwgljvcdbwueae = "mspnmbafpifqbuco";
    string lqcwbzq = "atfrycfwrjmsbndjakksoafmckafgwgziecyubmqdeaajht";
    double qswftg = 3072;
    string sdwpblfasjthti = "zwatsjwxwtlevkflcayxjrugzzqlpdqaezzgqfowoojsiqrhhxjdlzdpfxvtbqivwipvjtpcarykwwjyswtxjploitld";
    bool mzlcyrlyamknvoi = false;
    bool mxlcssjdtyjlxj = false;
    return string("jdjmsbfheopgw");
}

string phzkdlm::ueshrjmqsqfvvhcbh(bool nqmhzsvlaxss, int qeqhxcnungwwqg, string uzuvjatzyjv, double uagruqhbnjgpp, double bygnqduy, double dnvirejble, bool syribvhuzteo, string kmodxizqmgswr, bool zpanoqjnce) {
    double bvsjuucgdlgy = 38498;
    string tcxegvibhhym = "mutehefudhcbqibndrkrsjrjylmkkisgaipezy";
    bool gvrjmctbb = true;
    bool lzdlcpehznc = false;
    double tesxpazgqxmrj = 6808;
    string cxeivadpnionkv = "wduepvutxintkgcrgqynnxytwovjyiihddspgguebsrekzchqobthfkilbwhzvelxbmvzflybvwlcbrsqvtfswihnmpowqvoany";
    string bdyabduzrgdyp = "meozwchtlceqnafbkmkayiz";
    int dxiop = 1408;
    int zyxgqmpder = 7419;
    bool amoacrfldlzq = false;
    return string("nwqhkjatprkqajqto");
}

double phzkdlm::vpqcpnuzednehimv(bool xtbifko, int jeqatshz, int odwrbtwjzcsdk, int ucoiavg, bool nisjqyr, double ttqsdmbdtk, double cudsodowkh) {
    double tfauga = 13831;
    int thhihnmleogpiom = 1447;
    bool vmnvbgkxznqkq = false;
    string qrqenmz = "fnvfyt";
    double mbmiejnn = 44579;
    double vqdkcb = 6788;
    int rsosx = 1602;
    int fxdjgbbcduiej = 2541;
    int kresc = 520;
    if (13831 == 13831) {
        int pwo;
        for (pwo = 91; pwo > 0; pwo--) {
            continue;
        }
    }
    if (1447 != 1447) {
        int xzefroyjd;
        for (xzefroyjd = 81; xzefroyjd > 0; xzefroyjd--) {
            continue;
        }
    }
    if (6788 != 6788) {
        int pp;
        for (pp = 70; pp > 0; pp--) {
            continue;
        }
    }
    if (2541 == 2541) {
        int rpfoq;
        for (rpfoq = 65; rpfoq > 0; rpfoq--) {
            continue;
        }
    }
    return 59123;
}

int phzkdlm::hncbdqvhwkpreabttld(double endyqfjvoceuabi, string wfruof, string hxfibtdwd, string awlxgkxg, string gllngdg, bool uydicpiixbcujz, double yqwfswhbzigkkr, double ppwjnwoog, bool gshlnvn, double vyucjrs) {
    int qavnkydwcdx = 4457;
    string llgpdtil = "egqtyeaimbcpbqtdmpvwbkrohtsbxwwg";
    int zxkpegbjrlborpo = 7514;
    double ipurbck = 41949;
    bool pzeyatw = true;
    double jnjdbge = 43245;
    string qijqjz = "vutokdimgknu";
    string jyxcxditagdj = "rvtbfjluenaptikfctslzxqinloxeytlrvhlbfvtyvfgzvpzwaxfucsrudjshkekci";
    double ilydsreimvvzoo = 4941;
    string xsgjbbjzd = "lbajrvdkrcouspqhbutmqwrfrjdwbsamilyxdcrqdncgkvbyvmpfgbmvlo";
    return 92070;
}

bool phzkdlm::jykawjzrvcjoth(bool ykruwzd, int ogrgnlbvfspdp, int twtcoknlgu, string mdnrsjfwrfv, bool yqtayeab) {
    bool dufjctyiwuegs = true;
    int isreqwbhecilc = 1716;
    int smvcftv = 4805;
    bool llmpvnfji = true;
    int nysnpvjqfieb = 61;
    double juywcdc = 24096;
    int dpghznuirtet = 295;
    if (4805 != 4805) {
        int qu;
        for (qu = 84; qu > 0; qu--) {
            continue;
        }
    }
    if (24096 == 24096) {
        int trvkijp;
        for (trvkijp = 28; trvkijp > 0; trvkijp--) {
            continue;
        }
    }
    if (1716 != 1716) {
        int ipudfmwr;
        for (ipudfmwr = 83; ipudfmwr > 0; ipudfmwr--) {
            continue;
        }
    }
    if (24096 != 24096) {
        int iwk;
        for (iwk = 68; iwk > 0; iwk--) {
            continue;
        }
    }
    if (295 != 295) {
        int ojgpbamfj;
        for (ojgpbamfj = 84; ojgpbamfj > 0; ojgpbamfj--) {
            continue;
        }
    }
    return true;
}

void phzkdlm::jjtdibtxqfofolhn(string pnlartjo, bool jdoamuhjyma, string tifefaxetcpu, double jzznlpautbnc, int cjjbm, double uvrsscfwfpfiyzc, bool htmfsrvnnbtb, string slrdewjdkh, bool xtourcf) {
    double srfaqmtvcbxtj = 85809;
    string kgrssb = "ciuarmlhvvimdfiodjkgmmjqeqeinvnchyaerovvuyusiqpifzp";
    string nwydpgeb = "rjmvibgwzfipugiwpgbfyfbmkzplizfoelmnlidjnkuyskgcjgkwqwnmqwoywmtpnqaelqeecrvkuzcwxab";
    double hdzbfxk = 21798;
    string wczzlihcm = "gbepaqzvofrmygvkhulbxaubvddqhrtyvpbibvotnzlgwjhxttgritzrrqvcoxuwrdy";
    string tnwdygfkl = "dhygyhrmgudhgfayojibnvnotjxdpotqrllvwnklyfsjimahkjxhsnxtqwmtmrucyxxjmolvptkvwsd";
    bool grvabohcilvmthk = true;
    bool jrgix = false;
    int kzkpt = 1273;
    bool rmdumbc = true;
    if (string("gbepaqzvofrmygvkhulbxaubvddqhrtyvpbibvotnzlgwjhxttgritzrrqvcoxuwrdy") != string("gbepaqzvofrmygvkhulbxaubvddqhrtyvpbibvotnzlgwjhxttgritzrrqvcoxuwrdy")) {
        int oeh;
        for (oeh = 19; oeh > 0; oeh--) {
            continue;
        }
    }
    if (string("dhygyhrmgudhgfayojibnvnotjxdpotqrllvwnklyfsjimahkjxhsnxtqwmtmrucyxxjmolvptkvwsd") != string("dhygyhrmgudhgfayojibnvnotjxdpotqrllvwnklyfsjimahkjxhsnxtqwmtmrucyxxjmolvptkvwsd")) {
        int drfgqo;
        for (drfgqo = 13; drfgqo > 0; drfgqo--) {
            continue;
        }
    }

}

bool phzkdlm::mysvdcbugjdxfm(bool ifrzxuht, bool hacbamvkmxw, int iklkl, string omeildyov) {
    string bkufkcvfwn = "rorszgqomugpfajnqutphcugalofiyyucmqcunsdjfw";
    string kbzompahjribif = "erdmnjixneajdhyimhtdispqljzspdsiumapvetyojpaocdzthezrrksmiwixsjgsllplebtqq";
    string yxzknbdhclcux = "iusvujpesfker";
    string wzojya = "fthgbbrisciwacnzmvehjrwk";
    int jgtsjxrkcxelqzq = 8081;
    string ojghazblhlrijwg = "kha";
    double xnbyushfgoufwd = 88121;
    double uatnm = 41356;
    if (string("rorszgqomugpfajnqutphcugalofiyyucmqcunsdjfw") != string("rorszgqomugpfajnqutphcugalofiyyucmqcunsdjfw")) {
        int nzmdemi;
        for (nzmdemi = 64; nzmdemi > 0; nzmdemi--) {
            continue;
        }
    }
    if (string("kha") != string("kha")) {
        int ggrwjawcar;
        for (ggrwjawcar = 15; ggrwjawcar > 0; ggrwjawcar--) {
            continue;
        }
    }
    if (string("fthgbbrisciwacnzmvehjrwk") == string("fthgbbrisciwacnzmvehjrwk")) {
        int lilh;
        for (lilh = 33; lilh > 0; lilh--) {
            continue;
        }
    }
    if (8081 == 8081) {
        int fberpcwnc;
        for (fberpcwnc = 68; fberpcwnc > 0; fberpcwnc--) {
            continue;
        }
    }
    if (string("kha") != string("kha")) {
        int qobaqu;
        for (qobaqu = 44; qobaqu > 0; qobaqu--) {
            continue;
        }
    }
    return false;
}

int phzkdlm::phentgpabujfnhgdamz(string zvzvidlkus) {
    bool zywsrpp = true;
    string rwxjmodinwcfse = "tnwrzsuymm";
    string tudhjtgjhko = "sueduganofrfybcndqibrgldprtdnphxhklmbjsqlocqxniwxxjhcspvapquyiwec";
    double vfkcrcxzgb = 4732;
    string jwackj = "pvdflfgbdaqnfwxhbopmefrfvsxozzttgzhmcwwapzcpnusxqedx";
    string dqeqg = "lpeffsolsqujmzpvttyiaauhrujfpqkdxnnbpmiknqcudoinyripsaiyvkoargkhrvvecmajpfihksatytjs";
    double pwcubohkn = 6766;
    double frrzkhuopvaoq = 24850;
    return 52291;
}

double phzkdlm::hbjazrjelke(string kpvbtcf, double jpibomlh) {
    bool xlkxtitmzvnagqz = false;
    int lpsolvkn = 7529;
    string dtawsuxitzlzqd = "dtpqyecngsbssoagcyaanwepblzjaiglfmaetarvnttpqgeijtoamkhuxxaqnhbvl";
    string ggshxdfafpx = "dsakalgol";
    if (false == false) {
        int ghrmywkjca;
        for (ghrmywkjca = 42; ghrmywkjca > 0; ghrmywkjca--) {
            continue;
        }
    }
    if (false == false) {
        int ktcn;
        for (ktcn = 59; ktcn > 0; ktcn--) {
            continue;
        }
    }
    if (string("dsakalgol") != string("dsakalgol")) {
        int igyjiciwhr;
        for (igyjiciwhr = 21; igyjiciwhr > 0; igyjiciwhr--) {
            continue;
        }
    }
    if (7529 == 7529) {
        int ngxmfbms;
        for (ngxmfbms = 43; ngxmfbms > 0; ngxmfbms--) {
            continue;
        }
    }
    if (string("dtpqyecngsbssoagcyaanwepblzjaiglfmaetarvnttpqgeijtoamkhuxxaqnhbvl") == string("dtpqyecngsbssoagcyaanwepblzjaiglfmaetarvnttpqgeijtoamkhuxxaqnhbvl")) {
        int ssgmfol;
        for (ssgmfol = 53; ssgmfol > 0; ssgmfol--) {
            continue;
        }
    }
    return 14789;
}

string phzkdlm::cnpsjmsjvybkdu(double vmffkzr, double akykb, string bfyqelp, int sawlkdalokfjx, int ueemvhqdtuzbw, double iwbatb) {
    return string("an");
}

void phzkdlm::rgxzendbfyprsuqopbw(int ceolggfa, double awlvcrthqoq, int zvinlrbneywbkzr, bool kudvrnuisikt) {

}

string phzkdlm::ufemiocgrgdkvib(int zcsxwvcm, bool kqgzwkvfmiycv, bool tmdgvrcgibiy, bool shmhxjpw, double tbzib, int ipfuatjfu, double wspcubzibmzol, string hcwoipdp) {
    string vynvksjilfgw = "vkrunueazhrxmdstnikxfncoxozkivihoqcmocobehlgjkiomnqwtzsoefdleluicwfdafqbwsrlsgt";
    string kqtndoxaaarb = "rlxpjuauofosgvcwdxzhvglyahmiuwnmdsgwnrwlsrunb";
    double iyrrajiatph = 77137;
    if (77137 == 77137) {
        int jantrv;
        for (jantrv = 21; jantrv > 0; jantrv--) {
            continue;
        }
    }
    if (string("rlxpjuauofosgvcwdxzhvglyahmiuwnmdsgwnrwlsrunb") != string("rlxpjuauofosgvcwdxzhvglyahmiuwnmdsgwnrwlsrunb")) {
        int xrutmzecm;
        for (xrutmzecm = 86; xrutmzecm > 0; xrutmzecm--) {
            continue;
        }
    }
    if (string("rlxpjuauofosgvcwdxzhvglyahmiuwnmdsgwnrwlsrunb") == string("rlxpjuauofosgvcwdxzhvglyahmiuwnmdsgwnrwlsrunb")) {
        int gsdkccv;
        for (gsdkccv = 7; gsdkccv > 0; gsdkccv--) {
            continue;
        }
    }
    if (string("vkrunueazhrxmdstnikxfncoxozkivihoqcmocobehlgjkiomnqwtzsoefdleluicwfdafqbwsrlsgt") == string("vkrunueazhrxmdstnikxfncoxozkivihoqcmocobehlgjkiomnqwtzsoefdleluicwfdafqbwsrlsgt")) {
        int qrm;
        for (qrm = 49; qrm > 0; qrm--) {
            continue;
        }
    }
    return string("pj");
}

double phzkdlm::tmfjassqyrqqyynrdyun(bool eizhmqcbhzzwm, string rxdsyifyvusop, bool iygkejbj, bool yeatohusakw) {
    double djuwnhlyzned = 29765;
    double knnoi = 65513;
    bool ltwngighbn = false;
    string ipsoa = "edbultustcasyimsdfyiferqzsrkvndogodzsogyphpfhniqrkvbchzox";
    bool lpdhfp = true;
    int nefwew = 3764;
    string qungazhfrwlnwwc = "vzqztyyjsxnerf";
    int wbuzznowqlktid = 1092;
    string kslgbx = "qhr";
    if (string("vzqztyyjsxnerf") != string("vzqztyyjsxnerf")) {
        int yjivctoptj;
        for (yjivctoptj = 48; yjivctoptj > 0; yjivctoptj--) {
            continue;
        }
    }
    if (string("edbultustcasyimsdfyiferqzsrkvndogodzsogyphpfhniqrkvbchzox") == string("edbultustcasyimsdfyiferqzsrkvndogodzsogyphpfhniqrkvbchzox")) {
        int fghtketst;
        for (fghtketst = 32; fghtketst > 0; fghtketst--) {
            continue;
        }
    }
    if (string("edbultustcasyimsdfyiferqzsrkvndogodzsogyphpfhniqrkvbchzox") != string("edbultustcasyimsdfyiferqzsrkvndogodzsogyphpfhniqrkvbchzox")) {
        int mjeigxlck;
        for (mjeigxlck = 30; mjeigxlck > 0; mjeigxlck--) {
            continue;
        }
    }
    if (false == false) {
        int myezu;
        for (myezu = 15; myezu > 0; myezu--) {
            continue;
        }
    }
    if (false == false) {
        int viphpdter;
        for (viphpdter = 27; viphpdter > 0; viphpdter--) {
            continue;
        }
    }
    return 63933;
}

int phzkdlm::qpxtkkkztkkcvruoagogn(string jfqukx, string lyxhjlifyzh, double pqjdyiemniid, double jeidxluz, double bfexgm, int jrqruxsxuhranx) {
    double adrkixoljojmik = 20952;
    double tysuxnkbgdu = 27933;
    double mhdfcegsrzavci = 2362;
    int fhnuhittvo = 3615;
    int riinyenyz = 712;
    double gmusbizkcx = 8682;
    int pmixkdykukfqtpa = 2887;
    double gmzqy = 26472;
    if (3615 != 3615) {
        int pjrhutqs;
        for (pjrhutqs = 78; pjrhutqs > 0; pjrhutqs--) {
            continue;
        }
    }
    if (712 != 712) {
        int iynusn;
        for (iynusn = 92; iynusn > 0; iynusn--) {
            continue;
        }
    }
    return 51236;
}

int phzkdlm::oubunkechcenmxfeoughyb(bool cwwgogtci, bool zanlksvxvc, double qfqmhbjkvwcphql, string xfwlnutktwnnu, double quirfkgcjbxvpi, double ziqgaoec, double vptmroaeoaqybe, string poyrpgo) {
    double bbwtae = 8279;
    double trzkpqoohmgnebn = 82314;
    double wordgesndsme = 23614;
    if (82314 == 82314) {
        int emwcb;
        for (emwcb = 91; emwcb > 0; emwcb--) {
            continue;
        }
    }
    if (8279 != 8279) {
        int ddn;
        for (ddn = 53; ddn > 0; ddn--) {
            continue;
        }
    }
    if (23614 == 23614) {
        int edw;
        for (edw = 81; edw > 0; edw--) {
            continue;
        }
    }
    if (8279 == 8279) {
        int kcumrir;
        for (kcumrir = 25; kcumrir > 0; kcumrir--) {
            continue;
        }
    }
    if (23614 == 23614) {
        int ftlet;
        for (ftlet = 93; ftlet > 0; ftlet--) {
            continue;
        }
    }
    return 23476;
}

bool phzkdlm::wtyfibemgtreoczgicel(string kljegiwgndrevz, int pdrln) {
    int xgruiq = 2228;
    double jgvsnrwewji = 72499;
    if (2228 == 2228) {
        int lxgygaypx;
        for (lxgygaypx = 84; lxgygaypx > 0; lxgygaypx--) {
            continue;
        }
    }
    if (2228 != 2228) {
        int rcjzdb;
        for (rcjzdb = 29; rcjzdb > 0; rcjzdb--) {
            continue;
        }
    }
    if (72499 != 72499) {
        int iuv;
        for (iuv = 45; iuv > 0; iuv--) {
            continue;
        }
    }
    return true;
}

void phzkdlm::cydenlkxajikbuewemvfcgt(int wfpjzi) {
    string ifghpcfbiogvmgz = "mcpynbgpzjjlbmlltuqkzqlgwtlqx";
    double dnbddlxek = 62154;
    if (string("mcpynbgpzjjlbmlltuqkzqlgwtlqx") != string("mcpynbgpzjjlbmlltuqkzqlgwtlqx")) {
        int krabqkk;
        for (krabqkk = 93; krabqkk > 0; krabqkk--) {
            continue;
        }
    }
    if (string("mcpynbgpzjjlbmlltuqkzqlgwtlqx") == string("mcpynbgpzjjlbmlltuqkzqlgwtlqx")) {
        int lwifhuvd;
        for (lwifhuvd = 80; lwifhuvd > 0; lwifhuvd--) {
            continue;
        }
    }
    if (string("mcpynbgpzjjlbmlltuqkzqlgwtlqx") != string("mcpynbgpzjjlbmlltuqkzqlgwtlqx")) {
        int qzdf;
        for (qzdf = 44; qzdf > 0; qzdf--) {
            continue;
        }
    }
    if (62154 != 62154) {
        int sgu;
        for (sgu = 12; sgu > 0; sgu--) {
            continue;
        }
    }
    if (62154 != 62154) {
        int klsywayb;
        for (klsywayb = 66; klsywayb > 0; klsywayb--) {
            continue;
        }
    }

}

bool phzkdlm::pzssdvlgdqsd(string rdinrzyxhtbm, string xvfzn, int sbxvbiuxwzecgh, bool faurrff, double uffipshomnns, int ymefznksgdwhbp) {
    bool rtsrtfqdj = false;
    int ymohrnqpiya = 539;
    string pidrnrictlxy = "tsotgqdvhuprqfoostlcesfyxdcsegpxrudufoomnvlcyfr";
    bool nuyzvtaltznui = true;
    int qpxnzqrha = 161;
    double bmhuktsxnohvs = 30423;
    string yoisup = "haboxtbknflknvacidqmprzpktmkarqtjdzxottkwexlhobhmw";
    if (539 == 539) {
        int uilhfxrc;
        for (uilhfxrc = 76; uilhfxrc > 0; uilhfxrc--) {
            continue;
        }
    }
    return true;
}

phzkdlm::phzkdlm() {
    this->cnpsjmsjvybkdu(5835, 3535, string("weiwubllzmaspwpqwyfmeifwmnlnoevuvwsovqoiqokddlzczpqzchqnyyuhphamraklqdzlruzdqbxtrgxs"), 2571, 3695, 16898);
    this->rgxzendbfyprsuqopbw(2194, 12875, 778, false);
    this->ufemiocgrgdkvib(2716, false, false, true, 24516, 5883, 8568, string("oqzwsaulptomzycvvaicidmpislgtbg"));
    this->tmfjassqyrqqyynrdyun(false, string("wfjwxjwkfxfrrgdmsewrhvesvtosglilfhuhg"), true, true);
    this->qpxtkkkztkkcvruoagogn(string("pllzsdlwmvjjauudpfpbqmggxpayjiyzikwjhxucwniboannzrwisvexpoow"), string("ppxlrikdusaplqmessuulfeecjfwpvjbzacarxmcaschdasjrzxxqdkactnqgnjlaajnroxbkcdehzrcpiggphjhyltcekorcfb"), 23930, 32593, 34151, 1146);
    this->oubunkechcenmxfeoughyb(true, false, 55805, string("hxzhuyncucudeosahhllqvarfjkygfgujhmogcuhueekksshewxvxyzmhjb"), 34614, 6968, 30886, string("vfxxyzzayhgconnvwptbytuzxsqwtnqeljbibecjbqrocqjisz"));
    this->wtyfibemgtreoczgicel(string("xbzrugteybeaqgyjytxdykooxrprtdkelvfugkgnlbutyrkgiohtddsiqoteynikbbpuzgqgoissxr"), 1080);
    this->cydenlkxajikbuewemvfcgt(821);
    this->pzssdvlgdqsd(string("ekzvsuconwepsquicwmhlvlzgjnidqvpbyhndrmgeutueo"), string("jnijuyffimyrdpachlpniikzbtuvkiokdqmkdayeftqafwovfvtxqapwhsnjarmvwwzwpdqulvjkw"), 181, false, 46182, 5761);
    this->nfnsarildgyvb(27795, 29736, string("zpggzfdnzcxzfzhpnsxhcyrvoeenoiudnomjmk"), true, true, 45155, string("osxlcvneifpwedpekcrjjcdshmsttgioqtianccbbxslxudewlmhkvhcghjqlrbdxrnqexvtnnvqswkdofumsdfelmnbg"));
    this->ueshrjmqsqfvvhcbh(false, 350, string("odddqeujjjveyawvhwoytwhyhtnaqtpczkdfltllpziuwyibvmohicgxgbnnsveovupmxdbglzsdovppyrbdp"), 18052, 30187, 10520, false, string("yabdmos"), false);
    this->vpqcpnuzednehimv(true, 1056, 2553, 1533, true, 60477, 15017);
    this->hncbdqvhwkpreabttld(7053, string("npnwmiexplwbvqsrmkoxqeorskwjosxmairunz"), string("kkhggarlaiuinqhvxkxvwmvgielgurphzppmrukcyxvtgokmynbawktdscrfipdoxcdnarfpbxrimioqawapcnefwjqibtiopv"), string("caslywpegwatrdazmygftublrerkoyfwkmjozxagvdrtsrbdtyu"), string("nlfwnoxtpnsukiwceddewrwqnsnlwwpyxv"), false, 36224, 318, false, 39974);
    this->jykawjzrvcjoth(true, 4200, 7577, string("fycmgzhfgjngdlympmwzgha"), false);
    this->jjtdibtxqfofolhn(string("louunncewysbnrxoelakdyoratkletwuernp"), false, string("tqaihnwfvwnbxh"), 33501, 8276, 8027, false, string("stqkdrdcjqssueurylzohanwsvuhjoqqkdrryntdas"), false);
    this->mysvdcbugjdxfm(true, true, 6509, string("nbjpbgcnfotwpjmucednyzftdpxobzvmthlcdxpmmltaexxmbtwjlhdswzmhuvzqdnldexjhynivv"));
    this->phentgpabujfnhgdamz(string("ijvwtslzrhijznyptmbmuemwpgflckayfsfolfsssqsrxmzdjjnek"));
    this->hbjazrjelke(string("btjypcpsgopgdjrjyzhmyhfgnswuplqyvryooinehpmyacmerfbucntzyhliuuoaavfaojhuolajlekngshpjgjmj"), 4777);
    this->sbgsmcgcqnovzwupw(7737);
    this->cgrnbwltpgvzhk(false, false, 8964, 11986, true, 8905, 4211);
    this->kwdaxrbcredzwxmmmsibchjq(false);
    this->uqrevtbvpqblgeaqfozk();
    this->ychmtlvwlijnwrlfie(string(""), 2313, string("lfxclbzgseihvlbwuhovsjerqsgrumjgxeyhdyegklfkzuk"), string("srrolzqacyabiebjadfpcgsvhz"), string("ljawmcryypvsxnaksodjfhlrsbdxgbjwufifqbqwislvvsz"), 24467, false, 6221, 2135);
    this->eazfkxinsujvfjxxreia();
    this->wjfoygbkwinduzdxznixnsni(23511, 314, 7728, false, string("hntypvaostlprwzvwztlfgchgiwhqnllxmiwcmwsjyxuocaacwwxfroixvoypaiuvebdburxonkgjfxxagdvydensnhhxy"), string("qrfhwrazmyqxjswtprtcclvurakyunacegshvlthhryqog"));
    this->eplrnvxolaicojbeat(9134);
    this->djinulrepcll(string("faihzga"), 655, string("xutjcktouksukrsvtqckfabdjnecgcoaplrigyuemevhphilvhsbmlmiewcedxuctrovhrcacilftomjgsug"), false, true, 22268, 29530, false);
}

















































































// Junk Code By Troll Face & Thaisen's Gen
void jrjhmYAAAGldFTzilrQFvhcYNyqjzzKCiSvXlYGUEpqnDxbolOZvzFVCPLToiQBAvivmXbwMkwAFqoVQXBykaPCdxtIaE67749711() {     long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe36604438 = -533287452;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe51697994 = 6373582;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe68413503 = -914309571;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe99842784 = -511981614;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe17512453 = -993761268;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe54625037 = -719629443;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe36609066 = -445616201;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe90395619 = -890789523;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe80020293 = -74835599;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe16468452 = -40105926;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe53152154 = -145244065;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe83645049 = 54115194;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe76575723 = -543434863;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe63067251 = -506225806;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe12282535 = -169735971;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe5101284 = -259181477;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe22689542 = -157723187;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe29084205 = -619299557;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe75929995 = 717035;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe35334772 = -510765289;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe11376489 = -892664347;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe43750424 = -253257230;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe87995991 = -815498797;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe852937 = -533952647;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe56984914 = -668804208;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe1005041 = -739518552;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe25679264 = -292929843;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe21874187 = -251818141;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe18482035 = -728399317;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe68294819 = -473087525;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe81343879 = 86330808;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe69580094 = 81002013;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe67057855 = -88371943;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe8088055 = -616887454;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe6776663 = -389050167;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe72937914 = -455056079;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe24490859 = -408105440;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe634154 = 31788461;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe88787542 = -595889750;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe90092399 = 47892625;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe1084258 = -557248216;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe42266363 = -297253478;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe28601494 = -289448045;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe20122643 = -291030924;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe74798291 = -601606813;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe59015607 = -852520165;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe59976176 = -830331919;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe64080628 = -858235084;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe83709928 = 64817400;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe81177581 = 31794350;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe47543621 = -892433638;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe90695190 = -606819875;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe95765821 = -715080822;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe18356369 = -554255935;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe56373223 = -103847105;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe92854014 = -180030223;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe63702002 = -178127621;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe67560566 = -280356924;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe42857871 = -843177406;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe16507413 = -154242717;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe28945774 = -326699601;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe14734879 = -93798060;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe71913584 = -62390206;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe11725475 = -601748075;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe35124572 = -26436734;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe83572059 = -126246078;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe16587194 = -857512864;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe68487668 = -926547410;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe56290589 = -17175640;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe39344620 = -714679892;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe80610424 = -851076038;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe22055388 = -89511649;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe40296663 = 76590192;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe85837596 = 52824410;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe34250514 = -953517074;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe69110125 = -495410869;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe15148930 = -963809185;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe67873348 = -424467874;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe26054645 = -932345835;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe97969307 = -816284044;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe41028864 = -909186633;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe61598635 = -434694759;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe38164259 = -216635541;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe37304453 = -660193668;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe20751199 = -580653887;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe90648689 = -306849318;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe73814273 = -203917166;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe48701487 = -534116009;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe51714832 = -413040349;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe13922648 = -109019945;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe9235912 = -176928458;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe56930292 = -27748516;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe57776283 = -125034133;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe72280130 = -341647034;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe61146625 = -625407774;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe86349379 = -363450156;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe70352779 = -134863272;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe16876020 = -687699971;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe84998070 = -164594190;    long SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe91226231 = -533287452;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe36604438 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe51697994;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe51697994 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe68413503;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe68413503 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe99842784;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe99842784 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe17512453;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe17512453 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe54625037;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe54625037 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe36609066;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe36609066 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe90395619;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe90395619 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe80020293;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe80020293 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe16468452;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe16468452 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe53152154;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe53152154 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe83645049;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe83645049 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe76575723;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe76575723 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe63067251;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe63067251 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe12282535;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe12282535 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe5101284;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe5101284 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe22689542;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe22689542 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe29084205;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe29084205 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe75929995;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe75929995 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe35334772;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe35334772 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe11376489;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe11376489 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe43750424;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe43750424 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe87995991;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe87995991 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe852937;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe852937 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe56984914;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe56984914 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe1005041;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe1005041 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe25679264;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe25679264 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe21874187;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe21874187 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe18482035;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe18482035 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe68294819;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe68294819 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe81343879;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe81343879 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe69580094;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe69580094 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe67057855;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe67057855 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe8088055;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe8088055 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe6776663;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe6776663 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe72937914;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe72937914 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe24490859;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe24490859 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe634154;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe634154 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe88787542;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe88787542 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe90092399;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe90092399 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe1084258;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe1084258 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe42266363;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe42266363 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe28601494;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe28601494 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe20122643;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe20122643 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe74798291;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe74798291 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe59015607;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe59015607 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe59976176;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe59976176 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe64080628;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe64080628 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe83709928;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe83709928 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe81177581;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe81177581 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe47543621;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe47543621 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe90695190;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe90695190 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe95765821;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe95765821 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe18356369;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe18356369 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe56373223;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe56373223 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe92854014;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe92854014 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe63702002;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe63702002 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe67560566;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe67560566 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe42857871;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe42857871 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe16507413;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe16507413 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe28945774;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe28945774 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe14734879;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe14734879 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe71913584;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe71913584 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe11725475;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe11725475 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe35124572;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe35124572 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe83572059;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe83572059 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe16587194;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe16587194 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe68487668;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe68487668 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe56290589;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe56290589 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe39344620;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe39344620 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe80610424;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe80610424 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe22055388;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe22055388 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe40296663;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe40296663 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe85837596;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe85837596 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe34250514;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe34250514 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe69110125;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe69110125 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe15148930;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe15148930 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe67873348;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe67873348 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe26054645;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe26054645 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe97969307;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe97969307 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe41028864;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe41028864 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe61598635;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe61598635 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe38164259;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe38164259 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe37304453;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe37304453 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe20751199;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe20751199 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe90648689;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe90648689 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe73814273;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe73814273 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe48701487;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe48701487 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe51714832;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe51714832 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe13922648;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe13922648 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe9235912;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe9235912 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe56930292;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe56930292 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe57776283;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe57776283 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe72280130;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe72280130 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe61146625;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe61146625 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe86349379;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe86349379 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe70352779;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe70352779 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe16876020;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe16876020 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe84998070;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe84998070 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe91226231;     SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe91226231 = SmooqsUBoRJRNKDFQLapDQgSQagfSpeTWHSJLQAwAOikoSMEyXghikBBGWXQLojzmqvFsutzKdopivdyNbhuIwkmGQDpskkMe36604438;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void CATmZbHipAeUtXABihldaHXMrwlKIIRCwnrvtcKLJPYaZnRRSmkTSTvudHvdqwagicYGgFHGXZTKiwmDfTrVYykdrwRmh86293250() {     long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS65387875 = -848949691;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS60144780 = -556038917;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS5402576 = -279578799;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS76279955 = -520219869;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS58022208 = -496548841;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS73760473 = -68983072;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS55095015 = -688171309;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS44782009 = -81556637;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS33515748 = -83087649;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS63021632 = -966487640;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS44277402 = -769475438;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS98377411 = -582557326;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS62729098 = -553214274;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS3571933 = 72434363;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS88085033 = -319350173;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS76287456 = -749896889;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS72462422 = -357082956;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS65339597 = -100111588;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS57577806 = -330128555;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS79141421 = -207009587;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS12511912 = -724576600;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS82941418 = -587956776;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS92230264 = -727602415;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS77781234 = -787657749;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS31250946 = -182237368;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS71680319 = -524525411;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS43365661 = -766969619;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS43760337 = -922688152;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS27343899 = -856729899;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS65782621 = -370610782;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS81146343 = -489647102;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS86418768 = -914518300;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS28393289 = -454670722;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS55873510 = -405050547;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS33357274 = -628165567;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS40828746 = -74850135;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS30341918 = -536181377;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS98153425 = -898015717;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS22326568 = -11520362;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS87474032 = -755823984;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS66629449 = -104190929;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS41777152 = -457421795;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS21451741 = -459130224;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS62265082 = -130498473;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS90776776 = -670505570;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS83715969 = -615386144;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS47215687 = -316859387;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS35707495 = -388065786;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS90872663 = -184818355;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS91713826 = -622231697;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS58946899 = 65325435;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS71361092 = -532158952;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS53598625 = 21033311;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS95867640 = -908702689;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS73781338 = -913323824;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS82446456 = -160992916;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS67914516 = -828436502;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS27621342 = -491921050;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS45029010 = -237982501;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS86341888 = -972023430;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS30394813 = -302013454;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS11334679 = -765483157;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS17438110 = -224826738;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS67733126 = -712476868;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS81875288 = -376840538;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS57858634 = -854957138;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS69984123 = -27886605;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS6855589 = -48163728;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS70214658 = -299400070;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS47256288 = -144500039;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS45945539 = -113715512;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS74308996 = -459067239;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS43013029 = 11408773;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS70103774 = -574304571;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS12511973 = -2818658;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS70734760 = -167154806;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS61489678 = -28826552;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS29965182 = -497103943;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS87004457 = -17152180;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS47534977 = -566851224;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS24464632 = -107666024;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS7658166 = -278903833;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS52887674 = -637869798;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS35630073 = -134498202;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS6835723 = -335936217;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS9785252 = -957488151;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS32820143 = -835551612;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS32525648 = -545968034;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS82092171 = -491726723;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS50910818 = -367172652;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS72914230 = -246413633;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS2720577 = 55739672;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS53124416 = -560033216;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS35984679 = -39496932;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS57079219 = -353810531;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS55294771 = -338707773;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS24339042 = -132595057;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS53718614 = -746653357;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS80389793 = -753657935;    long hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS32918143 = -848949691;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS65387875 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS60144780;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS60144780 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS5402576;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS5402576 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS76279955;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS76279955 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS58022208;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS58022208 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS73760473;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS73760473 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS55095015;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS55095015 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS44782009;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS44782009 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS33515748;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS33515748 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS63021632;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS63021632 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS44277402;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS44277402 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS98377411;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS98377411 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS62729098;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS62729098 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS3571933;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS3571933 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS88085033;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS88085033 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS76287456;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS76287456 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS72462422;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS72462422 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS65339597;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS65339597 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS57577806;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS57577806 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS79141421;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS79141421 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS12511912;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS12511912 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS82941418;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS82941418 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS92230264;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS92230264 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS77781234;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS77781234 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS31250946;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS31250946 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS71680319;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS71680319 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS43365661;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS43365661 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS43760337;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS43760337 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS27343899;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS27343899 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS65782621;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS65782621 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS81146343;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS81146343 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS86418768;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS86418768 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS28393289;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS28393289 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS55873510;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS55873510 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS33357274;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS33357274 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS40828746;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS40828746 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS30341918;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS30341918 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS98153425;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS98153425 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS22326568;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS22326568 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS87474032;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS87474032 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS66629449;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS66629449 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS41777152;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS41777152 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS21451741;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS21451741 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS62265082;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS62265082 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS90776776;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS90776776 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS83715969;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS83715969 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS47215687;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS47215687 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS35707495;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS35707495 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS90872663;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS90872663 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS91713826;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS91713826 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS58946899;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS58946899 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS71361092;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS71361092 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS53598625;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS53598625 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS95867640;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS95867640 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS73781338;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS73781338 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS82446456;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS82446456 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS67914516;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS67914516 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS27621342;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS27621342 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS45029010;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS45029010 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS86341888;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS86341888 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS30394813;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS30394813 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS11334679;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS11334679 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS17438110;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS17438110 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS67733126;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS67733126 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS81875288;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS81875288 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS57858634;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS57858634 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS69984123;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS69984123 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS6855589;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS6855589 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS70214658;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS70214658 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS47256288;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS47256288 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS45945539;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS45945539 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS74308996;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS74308996 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS43013029;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS43013029 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS70103774;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS70103774 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS12511973;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS12511973 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS70734760;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS70734760 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS61489678;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS61489678 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS29965182;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS29965182 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS87004457;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS87004457 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS47534977;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS47534977 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS24464632;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS24464632 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS7658166;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS7658166 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS52887674;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS52887674 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS35630073;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS35630073 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS6835723;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS6835723 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS9785252;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS9785252 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS32820143;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS32820143 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS32525648;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS32525648 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS82092171;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS82092171 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS50910818;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS50910818 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS72914230;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS72914230 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS2720577;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS2720577 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS53124416;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS53124416 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS35984679;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS35984679 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS57079219;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS57079219 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS55294771;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS55294771 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS24339042;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS24339042 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS53718614;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS53718614 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS80389793;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS80389793 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS32918143;     hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS32918143 = hVMpssxPKlNeevszlMhVrxWFFHksuPjnVYqEHVqEpnUiequCreGkkQpjyGjkHEUvBZJFEnrQVcfEdobPNiAQIzDFjHGAlGxkS65387875;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ktnzstjnElnNcvJcldBeeySGtUJVRSHiMLRuKCqfYKixKttrMwEutwElbgFYkNWmeEBRdPRimVFTPUwAbRJUONpPGfSew35153217() {     double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB61904031 = -361488308;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB5941448 = -487429656;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB10428281 = -599117258;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB96288873 = -528763244;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB22254547 = -917958176;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB93604629 = -249794243;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB44636001 = -450821050;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB45627154 = -342352162;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB96399922 = 71317632;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB18706412 = -256809417;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB94333214 = -764974639;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB6248011 = -183551051;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB52073339 = -278170701;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB93724935 = -264510646;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB70398736 = -189320457;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB1962007 = -892120279;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB72226891 = -360122715;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB39974819 = 30898158;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB79286647 = -510264722;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB83829799 = -910522191;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB80356055 = 20106988;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB34695043 = -160978527;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB63288028 = -229043205;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB79780950 = -806314892;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB45304608 = -44316201;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB89417645 = -749717710;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB6151555 = -443751609;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB18308938 = -273960757;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB3200647 = -949072724;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB77992193 = -997671937;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB73534083 = -598068638;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB29807023 = 49386560;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB3111517 = -793795382;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB68391759 = -837219680;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB1663094 = -713174131;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB63085905 = -862043970;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB54928201 = -546778646;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB36321561 = -721516346;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB53404077 = -546248404;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB81054984 = -937456023;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB4972611 = -41761151;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB93121673 = -867966716;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB28851997 = -675837669;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB94857241 = -493650005;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB14754466 = -416030206;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB61183012 = -491691605;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB30278884 = -354739725;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB28505728 = -796779107;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB50152537 = -484440620;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB54492154 = -78258709;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB4105855 = -774776268;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB36496102 = -373251328;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB58017830 = -274848330;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB76249701 = -542943767;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB54797161 = -367595978;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB27208989 = -100509782;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB42653419 = -158386452;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB30647330 = -792802366;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB50984265 = -384447044;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB32836902 = -68240467;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB87453074 = -806042634;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB26327063 = -76860293;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB42426507 = -393279438;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB18407729 = 68989569;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB45172328 = -658740779;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB64526192 = -714361199;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB3136495 = -389755669;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB83681579 = -440951021;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB92061842 = -551336516;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB7312832 = -327276488;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB47033806 = -245341633;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB35905331 = -638606370;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB86570742 = -422853439;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB98231662 = -572808699;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB78857189 = -768761041;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB87234381 = -111926296;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB5843047 = -485140858;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB68430787 = -735393201;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB65026485 = -290284686;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB84121596 = -552624596;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB59138761 = -294977985;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB77645827 = -646972503;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB68156401 = -789520138;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB48708493 = -770814016;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB73886339 = -122895670;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB37037982 = -124817311;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB71789193 = -675765111;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB26861815 = -150851615;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB13594599 = -369623703;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB74454105 = -512664349;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB20432486 = -603657519;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB24280871 = -753976281;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB85337295 = -237069303;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB20567175 = -378007938;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB93601910 = -31413390;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB78645547 = -964900858;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB50695166 = -374687279;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB10444269 = -644827239;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB49684913 = -834909226;    double SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB50228274 = -361488308;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB61904031 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB5941448;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB5941448 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB10428281;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB10428281 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB96288873;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB96288873 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB22254547;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB22254547 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB93604629;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB93604629 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB44636001;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB44636001 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB45627154;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB45627154 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB96399922;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB96399922 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB18706412;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB18706412 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB94333214;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB94333214 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB6248011;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB6248011 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB52073339;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB52073339 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB93724935;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB93724935 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB70398736;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB70398736 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB1962007;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB1962007 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB72226891;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB72226891 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB39974819;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB39974819 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB79286647;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB79286647 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB83829799;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB83829799 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB80356055;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB80356055 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB34695043;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB34695043 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB63288028;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB63288028 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB79780950;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB79780950 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB45304608;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB45304608 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB89417645;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB89417645 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB6151555;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB6151555 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB18308938;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB18308938 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB3200647;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB3200647 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB77992193;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB77992193 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB73534083;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB73534083 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB29807023;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB29807023 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB3111517;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB3111517 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB68391759;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB68391759 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB1663094;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB1663094 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB63085905;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB63085905 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB54928201;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB54928201 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB36321561;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB36321561 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB53404077;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB53404077 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB81054984;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB81054984 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB4972611;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB4972611 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB93121673;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB93121673 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB28851997;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB28851997 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB94857241;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB94857241 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB14754466;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB14754466 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB61183012;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB61183012 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB30278884;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB30278884 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB28505728;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB28505728 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB50152537;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB50152537 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB54492154;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB54492154 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB4105855;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB4105855 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB36496102;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB36496102 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB58017830;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB58017830 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB76249701;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB76249701 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB54797161;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB54797161 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB27208989;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB27208989 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB42653419;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB42653419 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB30647330;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB30647330 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB50984265;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB50984265 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB32836902;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB32836902 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB87453074;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB87453074 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB26327063;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB26327063 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB42426507;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB42426507 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB18407729;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB18407729 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB45172328;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB45172328 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB64526192;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB64526192 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB3136495;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB3136495 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB83681579;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB83681579 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB92061842;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB92061842 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB7312832;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB7312832 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB47033806;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB47033806 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB35905331;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB35905331 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB86570742;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB86570742 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB98231662;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB98231662 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB78857189;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB78857189 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB87234381;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB87234381 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB5843047;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB5843047 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB68430787;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB68430787 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB65026485;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB65026485 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB84121596;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB84121596 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB59138761;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB59138761 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB77645827;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB77645827 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB68156401;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB68156401 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB48708493;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB48708493 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB73886339;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB73886339 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB37037982;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB37037982 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB71789193;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB71789193 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB26861815;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB26861815 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB13594599;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB13594599 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB74454105;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB74454105 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB20432486;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB20432486 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB24280871;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB24280871 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB85337295;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB85337295 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB20567175;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB20567175 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB93601910;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB93601910 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB78645547;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB78645547 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB50695166;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB50695166 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB10444269;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB10444269 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB49684913;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB49684913 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB50228274;     SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB50228274 = SvLowTFrcNxeVHXrlWHkFwSEBYRPlKywnmUateTBqIzDLptFRYVJLtqWcHmYCpGRPBrimxxGCFtxxPsMfLyNooktvyxmsvyBB61904031;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void pvBSRBHpkjXMCMSTfcmHWmBXVOmQWNAalRcGemfXxqmUiTtoXcgtVMvDudxCOrSXjxblkLptwOKgzgpBVxtugHFYWZMAo53696755() {     double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS90687468 = -677150546;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS14388234 = 50157845;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS47417353 = 35613514;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS72726044 = -537001499;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS62764302 = -420745749;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS12740066 = -699147871;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS63121951 = -693376158;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS13544 = -633119276;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS49895377 = 63065582;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS65259592 = -83191131;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS85458462 = -289206011;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS20980374 = -820223571;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS38226714 = -287950112;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS34229617 = -785850476;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS46201235 = -338934660;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS73148180 = -282835690;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS21999772 = -559482484;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS76230211 = -549913873;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS60934458 = -841110311;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS27636450 = -606766489;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS81491478 = -911805265;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS73886038 = -495678072;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS67522301 = -141146823;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS56709248 = 39980006;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS19570640 = -657749361;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS60092924 = -534724569;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS23837953 = -917791386;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS40195088 = -944830769;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS12062511 = 22596694;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS75479996 = -895195194;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS73336547 = -74046548;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS46645697 = -946133753;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS64446949 = -60094161;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS16177215 = -625382773;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS28243705 = -952289531;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS30976737 = -481838025;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS60779259 = -674854584;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS33840833 = -551320525;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS86943102 = 38120984;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS78436618 = -641172632;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS70517801 = -688703864;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS92632462 = 71864967;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS21702244 = -845519849;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS36999680 = -333117553;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS30732952 = -484928963;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS85883374 = -254557585;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS17518395 = -941267193;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS132596 = -326609810;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS57315271 = -734076375;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS65028398 = -732284756;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS15509133 = -917017196;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS17162004 = -298590405;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS15850634 = -638734197;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS53760974 = -897390521;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS72205277 = -77072698;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS16801430 = -81472475;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS46865933 = -808695332;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS90708105 = 95633508;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS53155404 = -879252139;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS2671379 = -886021181;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS88902112 = -781356486;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS22926863 = -748545389;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS87951033 = -555715971;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS74415380 = -41739224;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS91923044 = 90855417;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS38812766 = -343072259;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS56533424 = -660129410;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS22049500 = -662567339;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS5985912 = -833560946;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS15224499 = -857096635;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS12368921 = -607981107;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS88158939 = 91838040;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS89287109 = -488034857;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS82497840 = -99937679;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS57118648 = -918062625;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS88859016 = -883670233;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS52183795 = -650158224;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS30522621 = -808029270;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS25976297 = -475091031;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS33687266 = -303191776;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS42574529 = -593457377;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS23705358 = -491181577;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS82879816 = -110754395;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS47034112 = -245118550;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS59970863 = -978177999;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS56174543 = -775456143;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS30795063 = -207399557;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS10685976 = -162703640;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS43971938 = -448310076;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS11442276 = -770817057;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS84110804 = -673142693;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS70071154 = -670488092;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS80685428 = -672068386;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS84271724 = -75857836;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS89534505 = -859816147;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS47590938 = -940158475;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS4681430 = -372419063;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS47286863 = -703780625;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS45076636 = -323972970;    double dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS91920185 = -677150546;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS90687468 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS14388234;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS14388234 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS47417353;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS47417353 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS72726044;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS72726044 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS62764302;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS62764302 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS12740066;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS12740066 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS63121951;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS63121951 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS13544;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS13544 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS49895377;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS49895377 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS65259592;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS65259592 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS85458462;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS85458462 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS20980374;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS20980374 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS38226714;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS38226714 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS34229617;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS34229617 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS46201235;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS46201235 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS73148180;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS73148180 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS21999772;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS21999772 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS76230211;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS76230211 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS60934458;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS60934458 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS27636450;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS27636450 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS81491478;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS81491478 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS73886038;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS73886038 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS67522301;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS67522301 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS56709248;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS56709248 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS19570640;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS19570640 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS60092924;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS60092924 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS23837953;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS23837953 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS40195088;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS40195088 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS12062511;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS12062511 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS75479996;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS75479996 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS73336547;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS73336547 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS46645697;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS46645697 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS64446949;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS64446949 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS16177215;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS16177215 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS28243705;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS28243705 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS30976737;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS30976737 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS60779259;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS60779259 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS33840833;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS33840833 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS86943102;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS86943102 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS78436618;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS78436618 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS70517801;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS70517801 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS92632462;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS92632462 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS21702244;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS21702244 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS36999680;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS36999680 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS30732952;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS30732952 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS85883374;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS85883374 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS17518395;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS17518395 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS132596;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS132596 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS57315271;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS57315271 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS65028398;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS65028398 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS15509133;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS15509133 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS17162004;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS17162004 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS15850634;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS15850634 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS53760974;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS53760974 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS72205277;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS72205277 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS16801430;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS16801430 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS46865933;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS46865933 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS90708105;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS90708105 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS53155404;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS53155404 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS2671379;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS2671379 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS88902112;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS88902112 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS22926863;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS22926863 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS87951033;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS87951033 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS74415380;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS74415380 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS91923044;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS91923044 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS38812766;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS38812766 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS56533424;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS56533424 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS22049500;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS22049500 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS5985912;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS5985912 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS15224499;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS15224499 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS12368921;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS12368921 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS88158939;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS88158939 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS89287109;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS89287109 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS82497840;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS82497840 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS57118648;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS57118648 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS88859016;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS88859016 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS52183795;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS52183795 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS30522621;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS30522621 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS25976297;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS25976297 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS33687266;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS33687266 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS42574529;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS42574529 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS23705358;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS23705358 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS82879816;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS82879816 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS47034112;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS47034112 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS59970863;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS59970863 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS56174543;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS56174543 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS30795063;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS30795063 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS10685976;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS10685976 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS43971938;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS43971938 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS11442276;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS11442276 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS84110804;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS84110804 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS70071154;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS70071154 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS80685428;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS80685428 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS84271724;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS84271724 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS89534505;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS89534505 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS47590938;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS47590938 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS4681430;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS4681430 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS47286863;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS47286863 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS45076636;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS45076636 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS91920185;     dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS91920185 = dIGYPlksIELnmwGCOENowhQxYqcrCOLzoFTFDHjvEINtiGtKyWHVyMsvsIzYJtqttMyhdByHUSjnUXPRwRtgGqAcNXNuHLmLS90687468;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void qbLlPgCATukLQxPShbbAXhpGXATAwLZtCJfsfGHLGgFywPhJjVrvnWaImFOONBDHVqJHRXmbTQtqbqJHTJRaTdGfiXFar2556722() {     float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ87203624 = -189689164;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ60184901 = -981232895;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ52443058 = -283924945;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ92734962 = -545544875;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ26996641 = -842155084;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ32584222 = -879959042;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ52662936 = -456025899;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ858689 = -893914802;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ12779552 = -882529137;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ20944372 = -473512908;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ35514276 = -284705212;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ28850972 = -421217295;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ27570955 = -12906538;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ24382620 = -22795486;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ28514938 = -208904944;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ98822729 = -425059080;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ21764241 = -562522244;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ50865433 = -418904128;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ82643299 = 78753522;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ32324827 = -210279094;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ49335621 = -167121677;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ25639663 = -68699823;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ38580065 = -742587612;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ58708964 = 21322863;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ33624303 = -519828193;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ77830249 = -759916867;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ86623846 = -594573376;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ14743688 = -296103374;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ87919258 = -69746132;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ87689568 = -422256350;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ65724287 = -182468084;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ90033951 = 17771107;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ39165177 = -399218821;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ28695465 = 42448093;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ96549524 = 62701906;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ53233896 = -169031861;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ85365542 = -685451852;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ72008967 = -374821154;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ18020611 = -496607059;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ72017570 = -822804671;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ8860963 = -626274086;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ43976984 = -338679955;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ29102500 = 37772706;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ69591839 = -696269085;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ54710640 = -230453600;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ63350417 = -130863045;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ581592 = -979147530;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ92930828 = -735323131;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ16595145 = 66301361;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ27806726 = -188311768;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ60668088 = -657118898;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ82297013 = -139682781;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ20269838 = -934615837;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ34143034 = -531631600;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ53221100 = -631344851;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ61563962 = -20989341;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ21604836 = -138645283;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ93734093 = -205247808;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ59110660 = 74283318;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ49166391 = 17761783;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ45960375 = -185385666;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ37919248 = -59922526;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ12939430 = -724168671;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ25089983 = -360272788;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ55220084 = -191044825;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ45480324 = -202476320;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ89685795 = 78001525;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ98875490 = 44645368;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ27833096 = 14502608;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ75281042 = 60126916;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ13457188 = -739607229;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ49755274 = -87701090;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ32844822 = -922297069;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ10625730 = -98441807;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ23463865 = -584005009;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ5358638 = -828441722;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ96537163 = -6472530;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ68988226 = 53681472;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ3998325 = -748223538;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ70273886 = -288965149;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ77248658 = -780769338;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ93693018 = -859250246;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ98148542 = -262404735;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ60112532 = -881434364;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ27021481 = -765137452;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ83427274 = 57214697;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ69764113 = -47613056;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ5022143 = -867587222;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ75474364 = -326207056;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ34985563 = -916308754;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ31629060 = 69613421;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ91631448 = -380204045;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ12898308 = -349104473;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ68854220 = -414368842;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ26057196 = -537419006;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ70941714 = -466351560;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ31037554 = -614511285;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ4012517 = -601954506;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ14371756 = -405224261;    float tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ9230316 = -189689164;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ87203624 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ60184901;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ60184901 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ52443058;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ52443058 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ92734962;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ92734962 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ26996641;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ26996641 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ32584222;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ32584222 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ52662936;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ52662936 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ858689;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ858689 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ12779552;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ12779552 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ20944372;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ20944372 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ35514276;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ35514276 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ28850972;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ28850972 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ27570955;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ27570955 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ24382620;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ24382620 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ28514938;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ28514938 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ98822729;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ98822729 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ21764241;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ21764241 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ50865433;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ50865433 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ82643299;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ82643299 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ32324827;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ32324827 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ49335621;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ49335621 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ25639663;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ25639663 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ38580065;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ38580065 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ58708964;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ58708964 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ33624303;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ33624303 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ77830249;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ77830249 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ86623846;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ86623846 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ14743688;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ14743688 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ87919258;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ87919258 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ87689568;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ87689568 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ65724287;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ65724287 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ90033951;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ90033951 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ39165177;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ39165177 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ28695465;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ28695465 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ96549524;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ96549524 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ53233896;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ53233896 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ85365542;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ85365542 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ72008967;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ72008967 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ18020611;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ18020611 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ72017570;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ72017570 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ8860963;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ8860963 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ43976984;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ43976984 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ29102500;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ29102500 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ69591839;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ69591839 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ54710640;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ54710640 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ63350417;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ63350417 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ581592;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ581592 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ92930828;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ92930828 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ16595145;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ16595145 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ27806726;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ27806726 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ60668088;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ60668088 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ82297013;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ82297013 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ20269838;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ20269838 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ34143034;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ34143034 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ53221100;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ53221100 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ61563962;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ61563962 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ21604836;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ21604836 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ93734093;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ93734093 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ59110660;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ59110660 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ49166391;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ49166391 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ45960375;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ45960375 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ37919248;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ37919248 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ12939430;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ12939430 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ25089983;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ25089983 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ55220084;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ55220084 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ45480324;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ45480324 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ89685795;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ89685795 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ98875490;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ98875490 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ27833096;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ27833096 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ75281042;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ75281042 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ13457188;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ13457188 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ49755274;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ49755274 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ32844822;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ32844822 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ10625730;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ10625730 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ23463865;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ23463865 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ5358638;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ5358638 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ96537163;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ96537163 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ68988226;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ68988226 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ3998325;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ3998325 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ70273886;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ70273886 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ77248658;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ77248658 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ93693018;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ93693018 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ98148542;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ98148542 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ60112532;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ60112532 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ27021481;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ27021481 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ83427274;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ83427274 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ69764113;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ69764113 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ5022143;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ5022143 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ75474364;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ75474364 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ34985563;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ34985563 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ31629060;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ31629060 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ91631448;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ91631448 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ12898308;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ12898308 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ68854220;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ68854220 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ26057196;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ26057196 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ70941714;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ70941714 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ31037554;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ31037554 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ4012517;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ4012517 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ14371756;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ14371756 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ9230316;     tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ9230316 = tyEdydFFZsLjQcWcjgwvGuDkcnrkdYuSMjZSoNdNEBuRGrDBjDzqJelyuUALmVcAytXhCEoVDirEAlelkpAaZDIhxbXTNSaiZ87203624;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void CiCQmZfGVgUfFxUJjrAXstyScNAKmkSWrebDzZKKZoGsFypFmmPYlxYJjknYYVTlOxnaVRJJgpxDmFMeYvYzVtluGSTPm21100261() {     float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC15987062 = -505351402;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC68631687 = -443645393;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC89432130 = -749194173;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC69172133 = -553783130;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC67506396 = -344942657;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC51719658 = -229312671;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC71148886 = -698581007;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC55245078 = -84681916;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC66275005 = -890781187;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC67497551 = -299894622;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC26639524 = -908936585;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC43583335 = 42110185;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC13724330 = -22685949;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC64887301 = -544135316;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC4317438 = -358519146;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC70008903 = -915774492;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC71537122 = -761882012;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC87120825 = -999716158;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC64291111 = -252092068;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC76131477 = 93476609;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC50471045 = 966070;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC64830658 = -403399369;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC42814338 = -654691231;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC35637262 = -232382240;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC7890335 = -33261354;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC48505528 = -544923727;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC4310245 = 31386847;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC36629838 = -966973385;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC96781122 = -198076713;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC85177370 = -319779607;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC65526751 = -758445993;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC6872626 = -977749206;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC500611 = -765517600;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC76480919 = -845715000;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC23130136 = -176413495;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC21124728 = -888825916;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC91216600 = -813527790;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC69528240 = -204625332;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC51559637 = 87762329;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC69399203 = -526521281;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC74406153 = -173216799;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC43487773 = -498848272;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC21952747 = -131909473;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC11734279 = -535736634;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC70689126 = -299352357;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC88050779 = -993729025;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC87821102 = -465674998;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC64557695 = -265153834;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC23757880 = -183334394;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC38342970 = -842337816;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC72071366 = -799359826;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC62962915 = -65021858;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC78102642 = -198501704;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC11654307 = -886078354;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC70629215 = -340821571;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC51156404 = -1952034;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC25817349 = -788954163;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC53794868 = -416811934;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC61281799 = -420521777;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC19000868 = -800018931;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC47409413 = -160699518;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC34519048 = -731607622;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC58463956 = -886605203;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC81097634 = -471001581;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC1970801 = -541448629;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC19766899 = -931187380;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC43082725 = -192372215;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC37243411 = -176970950;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC41757166 = -267721822;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC83192709 = -469693231;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC78792302 = -2246703;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC2008882 = -457256680;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC35561189 = -987478488;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC94891907 = -725570788;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC1725324 = -733306593;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC6983272 = -500185659;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC42877911 = -171489897;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC31080060 = -18954597;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC64948136 = -933029883;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC19839555 = -39532329;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC60684426 = 20751271;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC39752549 = -703459320;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC12871959 = -683638992;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC58438152 = -355738898;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC13106005 = -520419781;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC2563836 = -593424136;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC28769983 = -679247502;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC88846303 = -879439247;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC5851705 = -404893429;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC71973732 = -74461462;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC95307378 = 128246;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC37421732 = -296715857;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC8246441 = -784103556;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC32558769 = -112218740;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC21989791 = -265821763;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC39887106 = -441609177;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC85023816 = -612243069;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC40855112 = -660907892;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC9763478 = -994288006;    float lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC50922227 = -505351402;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC15987062 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC68631687;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC68631687 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC89432130;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC89432130 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC69172133;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC69172133 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC67506396;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC67506396 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC51719658;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC51719658 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC71148886;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC71148886 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC55245078;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC55245078 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC66275005;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC66275005 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC67497551;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC67497551 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC26639524;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC26639524 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC43583335;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC43583335 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC13724330;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC13724330 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC64887301;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC64887301 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC4317438;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC4317438 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC70008903;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC70008903 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC71537122;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC71537122 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC87120825;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC87120825 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC64291111;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC64291111 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC76131477;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC76131477 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC50471045;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC50471045 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC64830658;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC64830658 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC42814338;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC42814338 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC35637262;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC35637262 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC7890335;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC7890335 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC48505528;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC48505528 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC4310245;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC4310245 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC36629838;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC36629838 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC96781122;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC96781122 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC85177370;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC85177370 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC65526751;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC65526751 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC6872626;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC6872626 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC500611;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC500611 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC76480919;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC76480919 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC23130136;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC23130136 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC21124728;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC21124728 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC91216600;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC91216600 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC69528240;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC69528240 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC51559637;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC51559637 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC69399203;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC69399203 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC74406153;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC74406153 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC43487773;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC43487773 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC21952747;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC21952747 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC11734279;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC11734279 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC70689126;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC70689126 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC88050779;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC88050779 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC87821102;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC87821102 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC64557695;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC64557695 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC23757880;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC23757880 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC38342970;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC38342970 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC72071366;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC72071366 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC62962915;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC62962915 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC78102642;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC78102642 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC11654307;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC11654307 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC70629215;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC70629215 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC51156404;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC51156404 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC25817349;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC25817349 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC53794868;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC53794868 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC61281799;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC61281799 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC19000868;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC19000868 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC47409413;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC47409413 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC34519048;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC34519048 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC58463956;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC58463956 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC81097634;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC81097634 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC1970801;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC1970801 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC19766899;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC19766899 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC43082725;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC43082725 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC37243411;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC37243411 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC41757166;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC41757166 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC83192709;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC83192709 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC78792302;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC78792302 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC2008882;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC2008882 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC35561189;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC35561189 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC94891907;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC94891907 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC1725324;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC1725324 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC6983272;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC6983272 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC42877911;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC42877911 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC31080060;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC31080060 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC64948136;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC64948136 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC19839555;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC19839555 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC60684426;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC60684426 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC39752549;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC39752549 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC12871959;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC12871959 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC58438152;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC58438152 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC13106005;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC13106005 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC2563836;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC2563836 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC28769983;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC28769983 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC88846303;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC88846303 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC5851705;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC5851705 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC71973732;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC71973732 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC95307378;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC95307378 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC37421732;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC37421732 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC8246441;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC8246441 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC32558769;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC32558769 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC21989791;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC21989791 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC39887106;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC39887106 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC85023816;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC85023816 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC40855112;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC40855112 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC9763478;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC9763478 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC50922227;     lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC50922227 = lmEUUeHKOjUdFsvAUWdXJAdwUPVMtTDxtXuDtjSbETKLDVPApumwqrclIHAEIuRugXjhysHlEBSqpRtyAUXzhHHCvIuljNSJC15987062;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void WImHPRpfNzCvXalAdrYfunsOrDBQHMDHcIfSWxiBhpUeAXkibknjiYjjPYQLPZXAvvTPuSegJXyESNNUSToXZUYEYwCyg39643799() {     int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba44770498 = -821013640;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba77078473 = 93942108;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba26421203 = -114463401;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba45609304 = -562021384;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba8016152 = -947730230;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba70855093 = -678666299;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba89634836 = -941136115;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba9631468 = -375449030;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba19770460 = -899033237;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba14050732 = -126276336;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba17764772 = -433167957;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba58315698 = -594562335;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba99877705 = -32465360;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba5391984 = 34524854;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba80119936 = -508133349;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba41195077 = -306489904;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba21310003 = -961241780;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba23376218 = -480528189;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba45938922 = -582937657;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba19938127 = -702767689;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba51606468 = -930946184;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba4021653 = -738098914;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba47048610 = -566794849;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba12565560 = -486087342;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba82156366 = -646694514;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba19180808 = -329930586;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba21996642 = -442652929;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba58515988 = -537843397;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba5642987 = -326407295;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba82665173 = -217302864;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba65329215 = -234423903;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba23711300 = -873269519;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba61836043 = -31816379;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba24266375 = -633878092;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba49710748 = -415528895;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba89015559 = -508619972;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba97067659 = -941603728;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba67047512 = -34429510;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba85098663 = -427868283;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba66780837 = -230237890;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba39951345 = -820159513;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba42998562 = -659016589;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba14802994 = -301591652;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba53876717 = -375204183;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba86667611 = -368251114;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba12751142 = -756595005;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba75060613 = 47797534;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba36184563 = -894984536;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba30920615 = -432970149;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba48879215 = -396363863;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba83474644 = -941600754;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba43628818 = 9639065;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba35935447 = -562387572;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba89165578 = -140525108;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba88037330 = -50298290;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba40748845 = 17085274;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba30029863 = -339263044;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba13855644 = -628376060;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba63452938 = -915326871;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba88835344 = -517799645;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba48858452 = -136013371;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba31118848 = -303292719;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba3988482 = 50958265;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba37105287 = -581730374;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba48721517 = -891852433;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba94053472 = -559898439;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba96479654 = -462745956;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba75611330 = -398587268;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba55681235 = -549946252;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba91104377 = -999513378;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba44127417 = -364886177;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba54262490 = -826812271;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba38277555 = 47340093;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba79158085 = -252699768;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba79986782 = -882608177;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba8607907 = -171929596;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba89218659 = -336507263;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba93171893 = -91590667;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba25897949 = -17836228;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba69405224 = -890099509;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba44120194 = -277728121;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba85812079 = -547668394;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba27595374 = -4873249;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba56763772 = -930043433;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba99190528 = -275702111;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba21700398 = -144062969;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba87775853 = -210881948;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba72670464 = -891291272;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba36229044 = -483579803;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba8961903 = -332614169;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba58985696 = -69356929;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba83212016 = -213227668;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba3594575 = -119102639;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba96263318 = -910068639;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba17922385 = 5775480;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba8832497 = -416866795;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba39010080 = -609974854;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba77697706 = -719861278;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba5155201 = -483351750;    int WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba92614138 = -821013640;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba44770498 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba77078473;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba77078473 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba26421203;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba26421203 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba45609304;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba45609304 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba8016152;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba8016152 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba70855093;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba70855093 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba89634836;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba89634836 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba9631468;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba9631468 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba19770460;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba19770460 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba14050732;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba14050732 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba17764772;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba17764772 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba58315698;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba58315698 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba99877705;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba99877705 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba5391984;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba5391984 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba80119936;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba80119936 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba41195077;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba41195077 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba21310003;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba21310003 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba23376218;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba23376218 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba45938922;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba45938922 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba19938127;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba19938127 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba51606468;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba51606468 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba4021653;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba4021653 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba47048610;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba47048610 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba12565560;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba12565560 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba82156366;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba82156366 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba19180808;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba19180808 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba21996642;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba21996642 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba58515988;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba58515988 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba5642987;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba5642987 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba82665173;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba82665173 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba65329215;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba65329215 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba23711300;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba23711300 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba61836043;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba61836043 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba24266375;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba24266375 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba49710748;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba49710748 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba89015559;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba89015559 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba97067659;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba97067659 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba67047512;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba67047512 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba85098663;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba85098663 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba66780837;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba66780837 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba39951345;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba39951345 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba42998562;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba42998562 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba14802994;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba14802994 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba53876717;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba53876717 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba86667611;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba86667611 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba12751142;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba12751142 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba75060613;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba75060613 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba36184563;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba36184563 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba30920615;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba30920615 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba48879215;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba48879215 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba83474644;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba83474644 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba43628818;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba43628818 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba35935447;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba35935447 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba89165578;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba89165578 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba88037330;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba88037330 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba40748845;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba40748845 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba30029863;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba30029863 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba13855644;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba13855644 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba63452938;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba63452938 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba88835344;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba88835344 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba48858452;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba48858452 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba31118848;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba31118848 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba3988482;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba3988482 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba37105287;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba37105287 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba48721517;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba48721517 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba94053472;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba94053472 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba96479654;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba96479654 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba75611330;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba75611330 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba55681235;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba55681235 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba91104377;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba91104377 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba44127417;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba44127417 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba54262490;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba54262490 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba38277555;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba38277555 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba79158085;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba79158085 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba79986782;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba79986782 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba8607907;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba8607907 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba89218659;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba89218659 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba93171893;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba93171893 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba25897949;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba25897949 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba69405224;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba69405224 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba44120194;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba44120194 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba85812079;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba85812079 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba27595374;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba27595374 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba56763772;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba56763772 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba99190528;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba99190528 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba21700398;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba21700398 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba87775853;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba87775853 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba72670464;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba72670464 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba36229044;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba36229044 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba8961903;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba8961903 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba58985696;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba58985696 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba83212016;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba83212016 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba3594575;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba3594575 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba96263318;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba96263318 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba17922385;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba17922385 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba8832497;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba8832497 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba39010080;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba39010080 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba77697706;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba77697706 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba5155201;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba5155201 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba92614138;     WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba92614138 = WlHRTjEEVnIFHaLZeQBKwtvkzbaXOqyPgzmlrmyeIQxfgRRuNeufAnkdqWSsJhpepsZHoZisRXBgvPEhJgKkwSOsOIvUUGVba44770498;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void orPSiPgSKGpmQxMDCiugnGjRxKtEOkhNRkNxztAMiRbnrDqlOpAWNcThHxUYWcEZZbUfkODIiixVfwauTHqYkKzoQgpCY58187338() {     int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI73553934 = -36675878;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI85525259 = -468470391;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI63410276 = -579732629;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI22046475 = -570259639;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI48525907 = -450517803;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI89990529 = -28019928;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI8120786 = -83691223;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI64017858 = -666216144;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI73265914 = -907285288;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI60603912 = 47341951;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI8890021 = 42600670;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI73048061 = -131234855;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI86031080 = -42244771;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI45896665 = -486814977;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI55922435 = -657747551;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI12381250 = -797205315;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI71082883 = -60601549;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI59631610 = 38659780;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI27586734 = -913783247;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI63744777 = -399011986;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI52741892 = -762858438;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI43212648 = 27201540;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI51282883 = -478898468;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI89493857 = -739792444;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI56422397 = -160127674;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI89856086 = -114937445;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI39683040 = -916692706;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI80402138 = -108713409;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI14504851 = -454737877;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI80152975 = -114826121;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI65131679 = -810401813;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI40549973 = -768789832;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI23171477 = -398115159;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI72051830 = -422041185;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI76291359 = -654644295;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI56906391 = -128414027;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI2918718 = 30320335;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI64566785 = -964233688;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI18637689 = -943498895;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI64162470 = 66045501;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI5496536 = -367102226;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI42509350 = -819184906;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI7653241 = -471273831;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI96019156 = -214671731;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI2646098 = -437149871;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI37451504 = -519460985;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI62300124 = -538729934;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI7811430 = -424815239;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI38083350 = -682605904;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI59415459 = 49610090;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI94877923 = 16158319;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI24294720 = 84299988;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI93768250 = -926273439;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI66676851 = -494971862;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI5445447 = -859775010;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI30341287 = 36122581;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI34242376 = -989571924;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI73916418 = -839940186;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI65624077 = -310131966;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI58669821 = -235580359;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI50307490 = -111327223;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI27718648 = -974977815;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI49513008 = -111478267;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI93112938 = -692459167;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI95472233 = -142256237;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI68340047 = -188609498;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI49876584 = -733119697;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI13979251 = -620203586;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI69605305 = -832170682;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI99016044 = -429333525;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI9462532 = -727525651;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI6516099 = -96367861;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI40993922 = -17841325;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI63424263 = -879828749;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI58248241 = 68090239;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI10232542 = -943673532;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI35559408 = -501524630;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI55263727 = -164226737;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI86847760 = -202642573;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI18970894 = -640666690;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI27555962 = -576207512;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI31871610 = -391877468;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI42318789 = -426107505;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI55089391 = -404347967;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI85275052 = -30984440;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI40836959 = -794701802;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI46781723 = -842516393;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI56494626 = -903143297;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI66606383 = -562266176;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI45950073 = -590766877;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI22664015 = -138842103;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI29002300 = -129739480;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI98942707 = -554101723;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI59967868 = -607918537;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI13854980 = -822627276;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI77777888 = -392124412;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI92996342 = -607706639;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI14540302 = -778814664;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI546923 = 27584505;    int cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI34306050 = -36675878;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI73553934 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI85525259;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI85525259 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI63410276;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI63410276 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI22046475;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI22046475 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI48525907;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI48525907 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI89990529;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI89990529 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI8120786;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI8120786 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI64017858;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI64017858 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI73265914;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI73265914 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI60603912;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI60603912 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI8890021;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI8890021 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI73048061;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI73048061 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI86031080;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI86031080 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI45896665;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI45896665 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI55922435;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI55922435 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI12381250;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI12381250 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI71082883;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI71082883 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI59631610;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI59631610 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI27586734;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI27586734 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI63744777;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI63744777 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI52741892;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI52741892 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI43212648;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI43212648 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI51282883;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI51282883 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI89493857;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI89493857 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI56422397;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI56422397 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI89856086;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI89856086 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI39683040;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI39683040 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI80402138;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI80402138 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI14504851;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI14504851 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI80152975;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI80152975 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI65131679;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI65131679 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI40549973;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI40549973 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI23171477;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI23171477 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI72051830;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI72051830 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI76291359;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI76291359 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI56906391;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI56906391 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI2918718;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI2918718 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI64566785;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI64566785 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI18637689;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI18637689 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI64162470;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI64162470 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI5496536;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI5496536 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI42509350;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI42509350 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI7653241;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI7653241 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI96019156;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI96019156 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI2646098;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI2646098 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI37451504;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI37451504 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI62300124;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI62300124 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI7811430;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI7811430 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI38083350;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI38083350 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI59415459;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI59415459 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI94877923;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI94877923 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI24294720;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI24294720 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI93768250;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI93768250 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI66676851;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI66676851 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI5445447;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI5445447 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI30341287;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI30341287 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI34242376;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI34242376 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI73916418;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI73916418 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI65624077;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI65624077 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI58669821;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI58669821 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI50307490;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI50307490 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI27718648;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI27718648 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI49513008;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI49513008 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI93112938;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI93112938 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI95472233;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI95472233 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI68340047;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI68340047 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI49876584;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI49876584 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI13979251;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI13979251 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI69605305;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI69605305 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI99016044;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI99016044 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI9462532;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI9462532 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI6516099;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI6516099 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI40993922;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI40993922 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI63424263;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI63424263 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI58248241;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI58248241 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI10232542;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI10232542 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI35559408;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI35559408 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI55263727;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI55263727 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI86847760;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI86847760 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI18970894;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI18970894 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI27555962;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI27555962 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI31871610;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI31871610 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI42318789;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI42318789 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI55089391;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI55089391 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI85275052;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI85275052 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI40836959;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI40836959 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI46781723;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI46781723 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI56494626;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI56494626 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI66606383;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI66606383 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI45950073;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI45950073 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI22664015;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI22664015 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI29002300;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI29002300 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI98942707;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI98942707 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI59967868;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI59967868 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI13854980;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI13854980 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI77777888;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI77777888 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI92996342;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI92996342 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI14540302;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI14540302 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI546923;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI546923 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI34306050;     cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI34306050 = cqGNBOPiKBzlroffyBKXaEOlfpObfPEZBjGsqHeiKLJGqmsWUJSefeyXbUiLzoCvxakHinoqXcNPvHvsxsHVxZtXliQVZSiWI73553934;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void DnzUPMgGRVWFPGyKmvIliuKKHBydannOxkiikwEuUDnhy79249395() {     int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs74882566 = -825906241;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs78592405 = 64170787;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs47059511 = -829135873;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs70863255 = -577021039;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs47448485 = -749356726;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs91023476 = -955778444;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs59936007 = -944190897;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs23186434 = -333526006;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs28618461 = -957046189;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs33339794 = -787225921;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs16180953 = -476714005;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs52037135 = 81620431;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs67611634 = -952090324;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs69732598 = -93927416;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs12709978 = 44251881;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs34621863 = -651920321;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs29911646 = -546311639;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs77270107 = -427196366;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs25449719 = -278517023;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs7442779 = -999571399;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs90978627 = 5553848;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs57767016 = -536427171;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs22081040 = -659606876;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs47441029 = -125507582;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs48607666 = -422013341;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs27201071 = -111454065;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs20485575 = -450897007;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs5757836 = -39611696;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs25894636 = -552453372;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs97918247 = -387262287;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs5269873 = -514146374;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs66577224 = -867764700;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs59141530 = -754113128;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs62468786 = 19576951;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs3674419 = -735354856;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs58338551 = -685701119;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs76731721 = -743026898;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs1124391 = -180707429;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs5104747 = -753237124;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs77650781 = -328002189;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs2958761 = -747468572;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs22050957 = 73613168;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs88222919 = -484674983;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs63898805 = -396595879;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs52177972 = -571138448;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs8722311 = -961521397;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs1885647 = -199664048;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs81591281 = 61484224;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs92824059 = -10821221;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs80675894 = -169837998;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs32807396 = -917101887;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs77238915 = -200452891;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs8387483 = 11266785;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs8388910 = -606310884;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs88270768 = -877237173;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs17115551 = -189479070;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs56511366 = -276222338;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs99618482 = -603628291;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs22255590 = -55007699;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs20247414 = -537902662;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs70537901 = -404881437;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs54178172 = -804579202;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs97291798 = -781072635;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs30700213 = -469783903;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs28069921 = -173079547;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs49603728 = -608949305;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs92895605 = -164266441;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs5142848 = -871667276;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs66058179 = -358572561;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs54371427 = -270047001;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs57890142 = -908893424;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs28787255 = -265604210;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs72165360 = -673959242;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs47798937 = -950514835;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs4484018 = -152102828;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs68927670 = 31940680;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs69544096 = 48247811;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs58182235 = -163010998;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs95263057 = -554369135;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs39885355 = -460491945;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs25315425 = -911790017;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs38894294 = -412381232;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs12933776 = 71209525;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs45218741 = -282615375;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs65110852 = -470160401;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs28030958 = -213693483;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs58189742 = -779031485;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs50752621 = -47802245;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs74198018 = -103185877;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs86558868 = -445875786;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs1827186 = -309478781;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs77113239 = -39398607;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs78868801 = -25699731;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs84857332 = -115334462;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs7112881 = -923120753;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs48780589 = -942889371;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs24759159 = -145314198;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs57522706 = 85108919;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs35828884 = -123516332;    int gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs2574244 = -825906241;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs74882566 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs78592405;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs78592405 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs47059511;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs47059511 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs70863255;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs70863255 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs47448485;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs47448485 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs91023476;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs91023476 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs59936007;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs59936007 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs23186434;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs23186434 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs28618461;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs28618461 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs33339794;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs33339794 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs16180953;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs16180953 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs52037135;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs52037135 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs67611634;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs67611634 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs69732598;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs69732598 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs12709978;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs12709978 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs34621863;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs34621863 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs29911646;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs29911646 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs77270107;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs77270107 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs25449719;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs25449719 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs7442779;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs7442779 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs90978627;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs90978627 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs57767016;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs57767016 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs22081040;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs22081040 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs47441029;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs47441029 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs48607666;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs48607666 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs27201071;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs27201071 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs20485575;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs20485575 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs5757836;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs5757836 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs25894636;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs25894636 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs97918247;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs97918247 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs5269873;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs5269873 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs66577224;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs66577224 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs59141530;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs59141530 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs62468786;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs62468786 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs3674419;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs3674419 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs58338551;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs58338551 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs76731721;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs76731721 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs1124391;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs1124391 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs5104747;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs5104747 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs77650781;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs77650781 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs2958761;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs2958761 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs22050957;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs22050957 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs88222919;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs88222919 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs63898805;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs63898805 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs52177972;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs52177972 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs8722311;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs8722311 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs1885647;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs1885647 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs81591281;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs81591281 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs92824059;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs92824059 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs80675894;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs80675894 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs32807396;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs32807396 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs77238915;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs77238915 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs8387483;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs8387483 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs8388910;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs8388910 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs88270768;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs88270768 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs17115551;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs17115551 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs56511366;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs56511366 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs99618482;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs99618482 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs22255590;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs22255590 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs20247414;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs20247414 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs70537901;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs70537901 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs54178172;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs54178172 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs97291798;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs97291798 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs30700213;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs30700213 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs28069921;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs28069921 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs49603728;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs49603728 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs92895605;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs92895605 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs5142848;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs5142848 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs66058179;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs66058179 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs54371427;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs54371427 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs57890142;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs57890142 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs28787255;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs28787255 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs72165360;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs72165360 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs47798937;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs47798937 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs4484018;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs4484018 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs68927670;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs68927670 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs69544096;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs69544096 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs58182235;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs58182235 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs95263057;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs95263057 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs39885355;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs39885355 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs25315425;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs25315425 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs38894294;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs38894294 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs12933776;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs12933776 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs45218741;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs45218741 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs65110852;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs65110852 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs28030958;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs28030958 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs58189742;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs58189742 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs50752621;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs50752621 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs74198018;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs74198018 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs86558868;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs86558868 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs1827186;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs1827186 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs77113239;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs77113239 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs78868801;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs78868801 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs84857332;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs84857332 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs7112881;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs7112881 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs48780589;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs48780589 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs24759159;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs24759159 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs57522706;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs57522706 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs35828884;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs35828884 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs2574244;     gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs2574244 = gLLTfqvcNNIdoMOOCfdhIIfYPaprNReHcMbpnKvxs74882566;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void xszlxCNWeBdYmjBqLZHygmXQHJdvfwlreAOquXmkTdraymfQgaPnfHqLUienYRqhPmulVLvnnWeCuwSVeMOF48512041() {     float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm51355949 = -117911227;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm56343987 = -919290443;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm4176034 = -690302325;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm66213273 = -148178041;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm86454537 = -533220548;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm46618905 = -946188844;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm94330963 = -611669927;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm73831192 = -694363746;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm54286593 = -296518013;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm75574762 = -204649023;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm75868818 = -637564316;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm38129570 = -75724445;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm80344348 = -754493655;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm6587606 = -728684317;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm60777901 = -871068095;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm59642399 = -340844252;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm51118848 = -215543470;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm68892263 = -61017562;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm91521678 = -946455361;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm42223858 = 57805173;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm22142012 = -149841192;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm62569716 = -478618385;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm86461474 = -481856573;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm7972074 = -175107525;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm82512293 = -369170692;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm16304911 = -396451341;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm80622894 = -682945187;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm12288424 = -216595284;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm20028449 = -372679323;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm26215670 = -891950136;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm24847007 = -562293265;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm8822089 = -151060940;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm94843082 = -604206733;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm29805215 = -924704352;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm60191807 = -132693680;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm36656148 = -437995568;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm50489149 = -142897304;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm52998331 = -633810547;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm33911640 = -713931715;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm244269 = -798575632;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm7221842 = -788129937;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm68076617 = -318281037;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm6732181 = -648712243;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm604012 = -657563935;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm34086872 = -576974375;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm23476376 = -89765577;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm57876366 = -648369263;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm74550133 = -509044815;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm1560114 = -427549763;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm51587745 = -182424920;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm3012440 = -407484218;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm70283357 = -681969829;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm19062524 = -188270816;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm98954924 = -409166354;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm26370030 = -12717242;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm88786233 = -639292842;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm69882513 = -337433870;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm96203960 = -415194800;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm83700979 = -779007350;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm70149627 = -36769207;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm65996011 = -163243657;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm82042540 = -295074644;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm53802743 = -221684423;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm28070924 = -404567877;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm50727756 = -642355759;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm67046730 = -386503376;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm43286487 = -471517713;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm50539134 = -829789303;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm46395799 = -495990638;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm24121754 = -333072527;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm9153250 = -97946948;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm98120516 = -581732924;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm34980624 = -347085848;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm91277409 = -47879729;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm35002016 = -154064890;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm54065395 = -831560155;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm55837535 = -829906143;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm85857462 = -824292638;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm73885201 = -598133150;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm59035918 = -179405116;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm58428544 = -748082079;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm6072762 = -73900373;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm10728311 = -789045521;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm68440703 = -90254404;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm23203230 = -384465919;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm54563649 = -880323436;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm89759565 = -962790124;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm95888158 = -95040380;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm3435186 = -811987111;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm71405573 = -493400838;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm66773634 = -561699;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm54285189 = -727702504;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm69297352 = -854803198;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm63762013 = -577162508;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm34248257 = -535331976;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm25179301 = -393055294;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm14273874 = 3403386;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm78661256 = -144144367;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm49876256 = 84791823;    float hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm67040142 = -117911227;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm51355949 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm56343987;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm56343987 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm4176034;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm4176034 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm66213273;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm66213273 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm86454537;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm86454537 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm46618905;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm46618905 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm94330963;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm94330963 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm73831192;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm73831192 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm54286593;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm54286593 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm75574762;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm75574762 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm75868818;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm75868818 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm38129570;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm38129570 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm80344348;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm80344348 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm6587606;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm6587606 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm60777901;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm60777901 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm59642399;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm59642399 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm51118848;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm51118848 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm68892263;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm68892263 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm91521678;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm91521678 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm42223858;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm42223858 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm22142012;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm22142012 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm62569716;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm62569716 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm86461474;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm86461474 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm7972074;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm7972074 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm82512293;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm82512293 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm16304911;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm16304911 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm80622894;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm80622894 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm12288424;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm12288424 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm20028449;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm20028449 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm26215670;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm26215670 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm24847007;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm24847007 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm8822089;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm8822089 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm94843082;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm94843082 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm29805215;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm29805215 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm60191807;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm60191807 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm36656148;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm36656148 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm50489149;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm50489149 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm52998331;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm52998331 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm33911640;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm33911640 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm244269;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm244269 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm7221842;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm7221842 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm68076617;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm68076617 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm6732181;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm6732181 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm604012;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm604012 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm34086872;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm34086872 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm23476376;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm23476376 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm57876366;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm57876366 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm74550133;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm74550133 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm1560114;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm1560114 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm51587745;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm51587745 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm3012440;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm3012440 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm70283357;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm70283357 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm19062524;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm19062524 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm98954924;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm98954924 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm26370030;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm26370030 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm88786233;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm88786233 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm69882513;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm69882513 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm96203960;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm96203960 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm83700979;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm83700979 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm70149627;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm70149627 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm65996011;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm65996011 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm82042540;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm82042540 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm53802743;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm53802743 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm28070924;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm28070924 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm50727756;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm50727756 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm67046730;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm67046730 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm43286487;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm43286487 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm50539134;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm50539134 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm46395799;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm46395799 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm24121754;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm24121754 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm9153250;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm9153250 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm98120516;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm98120516 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm34980624;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm34980624 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm91277409;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm91277409 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm35002016;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm35002016 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm54065395;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm54065395 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm55837535;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm55837535 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm85857462;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm85857462 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm73885201;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm73885201 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm59035918;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm59035918 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm58428544;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm58428544 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm6072762;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm6072762 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm10728311;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm10728311 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm68440703;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm68440703 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm23203230;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm23203230 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm54563649;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm54563649 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm89759565;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm89759565 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm95888158;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm95888158 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm3435186;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm3435186 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm71405573;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm71405573 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm66773634;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm66773634 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm54285189;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm54285189 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm69297352;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm69297352 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm63762013;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm63762013 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm34248257;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm34248257 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm25179301;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm25179301 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm14273874;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm14273874 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm78661256;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm78661256 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm49876256;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm49876256 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm67040142;     hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm67040142 = hyLWKCVMtyLBmKLMrtvVebAIdnQYQJJLCkzRbXbSpjtoLcQklGdkQtbGxCtWlxwkpzyBKtkTSdfviMxAWgmJfYuJJtTYeEgsm51355949;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void VSmaHjgtGWcgqxwjnnyVyyBwQuGLqhaZqRZBLTwsXDCOaqMCXvC52560521() {     long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA99042659 = -680516044;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA94934941 = -261636730;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA74960089 = -614763856;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA77666050 = -171367206;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA89370886 = 51525551;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA481615 = -336962021;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA94513638 = -438862082;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA4696586 = -616523014;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA67829353 = -34560820;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA69576306 = -164093847;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA54591739 = -153919289;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA16635481 = -564135974;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA22850145 = -7946812;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA79860042 = 85322087;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA55629380 = -518130276;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA759035 = -255450596;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA50479550 = -695222818;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA85759294 = -962562534;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA36159961 = -492539244;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA69235168 = -594586183;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA92004685 = -328557160;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA17329554 = -891105995;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA65046834 = -71481573;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA13399875 = 88537373;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA6372235 = -466241797;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA93020509 = -850544727;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA8184606 = -591353447;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA14634624 = -27192347;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA11639621 = -937609865;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA45070224 = -236830415;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA61328015 = -228008862;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA26590209 = -834747750;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA11935414 = -738973670;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA49497607 = -369163429;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA31307603 = -520574069;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA68497007 = -60378854;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA74366203 = -14518462;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA42311839 = -154740823;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA46836306 = 34663587;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA11392570 = -820148310;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA39867565 = -775820538;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA7440319 = -646902987;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA12532876 = -922632453;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA74782729 = -857546665;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA42026313 = -200541248;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA33744063 = 88833869;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA40476472 = -751187321;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA83573907 = -204123830;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA62462627 = -926524505;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA21986063 = -434498233;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA68443890 = -644903125;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA89935526 = -879220568;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA59628936 = -519949551;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA17134802 = -673534995;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA60555835 = -417170236;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA81713106 = -789410049;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA29888107 = -90155157;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA61560215 = -603301229;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA71293816 = -705125410;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA96350376 = -97929723;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA92297008 = -745608575;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA79879014 = -311669735;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA93056964 = -678913150;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA22759130 = -797730405;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA8248291 = -936084985;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA28001531 = -319171539;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA4700067 = -825162304;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA73352538 = -638783384;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA48552440 = -394103844;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA87132372 = -357751423;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA26392831 = -140932135;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA8167711 = -440481995;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA38922989 = -897226121;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA24767392 = -672390935;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA29367604 = -818765645;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA84564367 = -681654174;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA4796678 = -968473543;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA90264104 = -213934909;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA71373561 = -710921380;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA72628171 = -455075667;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA52544038 = 642593;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA24610699 = -287229617;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA52171997 = -100667843;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA89653558 = -403111632;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA76626333 = -591927290;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA71392489 = -348788295;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA66961273 = -214798200;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA94800611 = 34561324;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA88941771 = -951993194;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA49594497 = -731164021;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA38608900 = -970223697;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA12805988 = -411217233;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA71018023 = -449615414;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA50485929 = -867406691;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA19095561 = 25460264;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA59988551 = -364150803;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA14383355 = -967989838;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA89773746 = -24902048;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA66534438 = -921461680;    long uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA14024783 = -680516044;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA99042659 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA94934941;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA94934941 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA74960089;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA74960089 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA77666050;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA77666050 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA89370886;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA89370886 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA481615;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA481615 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA94513638;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA94513638 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA4696586;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA4696586 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA67829353;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA67829353 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA69576306;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA69576306 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA54591739;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA54591739 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA16635481;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA16635481 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA22850145;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA22850145 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA79860042;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA79860042 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA55629380;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA55629380 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA759035;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA759035 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA50479550;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA50479550 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA85759294;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA85759294 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA36159961;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA36159961 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA69235168;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA69235168 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA92004685;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA92004685 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA17329554;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA17329554 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA65046834;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA65046834 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA13399875;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA13399875 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA6372235;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA6372235 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA93020509;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA93020509 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA8184606;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA8184606 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA14634624;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA14634624 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA11639621;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA11639621 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA45070224;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA45070224 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA61328015;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA61328015 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA26590209;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA26590209 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA11935414;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA11935414 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA49497607;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA49497607 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA31307603;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA31307603 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA68497007;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA68497007 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA74366203;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA74366203 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA42311839;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA42311839 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA46836306;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA46836306 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA11392570;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA11392570 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA39867565;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA39867565 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA7440319;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA7440319 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA12532876;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA12532876 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA74782729;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA74782729 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA42026313;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA42026313 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA33744063;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA33744063 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA40476472;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA40476472 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA83573907;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA83573907 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA62462627;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA62462627 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA21986063;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA21986063 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA68443890;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA68443890 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA89935526;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA89935526 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA59628936;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA59628936 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA17134802;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA17134802 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA60555835;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA60555835 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA81713106;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA81713106 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA29888107;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA29888107 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA61560215;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA61560215 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA71293816;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA71293816 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA96350376;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA96350376 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA92297008;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA92297008 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA79879014;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA79879014 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA93056964;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA93056964 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA22759130;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA22759130 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA8248291;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA8248291 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA28001531;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA28001531 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA4700067;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA4700067 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA73352538;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA73352538 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA48552440;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA48552440 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA87132372;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA87132372 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA26392831;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA26392831 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA8167711;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA8167711 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA38922989;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA38922989 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA24767392;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA24767392 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA29367604;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA29367604 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA84564367;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA84564367 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA4796678;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA4796678 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA90264104;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA90264104 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA71373561;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA71373561 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA72628171;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA72628171 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA52544038;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA52544038 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA24610699;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA24610699 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA52171997;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA52171997 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA89653558;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA89653558 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA76626333;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA76626333 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA71392489;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA71392489 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA66961273;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA66961273 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA94800611;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA94800611 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA88941771;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA88941771 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA49594497;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA49594497 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA38608900;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA38608900 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA12805988;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA12805988 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA71018023;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA71018023 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA50485929;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA50485929 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA19095561;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA19095561 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA59988551;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA59988551 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA14383355;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA14383355 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA89773746;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA89773746 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA66534438;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA66534438 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA14024783;     uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA14024783 = uJLxWQqJNdUDYvDpZYvXTqGxuvmVahDouefQfTCkVJjODAdTXA99042659;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void lZLZCXjTbBoIcwKrUNlEKASwagouwnoqgDWlJYZAxXOYqcUztGkPQmskHPzVipWoMxbYaYivokGmmtFxMpBpCZauKl93253975() {     double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj69116488 = -877524826;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj17873396 = -652526077;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj74513424 = -778879231;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj4125516 = -200963900;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj90461488 = 45214640;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj69227440 = -688343576;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj86852051 = -88041544;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj11195838 = 90721058;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj78535244 = -756799667;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj55341435 = -966280004;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj70856520 = -766898664;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj51044340 = -203292805;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj32364122 = -980117288;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj95747231 = -728380266;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj87216135 = -892670189;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj86131582 = -551724482;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj99663602 = -352181986;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj8602741 = -115850200;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj22079875 = -173725251;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj74762763 = -399611992;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj62750466 = -498760442;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj35904610 = -708359918;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj46925517 = -937187165;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj45327463 = -211810587;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj65772422 = -184872039;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj58039103 = -648532333;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj82835738 = -257348199;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj22892275 = -726243871;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj10143355 = -746797510;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj73081957 = -602006561;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj42099830 = 24959388;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj1899519 = 29494088;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj10066416 = -303084099;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj28578687 = -137749355;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj78652763 = -972210878;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj42030736 = 1842503;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj91682968 = -719087571;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj85251448 = -643296575;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj8069103 = -717787131;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj42726584 = -977945016;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj1270659 = -166688805;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj35312412 = -733433608;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj23883763 = 15916756;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj48405566 = 45106956;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj17949021 = -733251597;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj52112031 = 85204238;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj35373974 = -332415632;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj37196357 = -755737835;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj17825045 = -275215921;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj89468126 = -828591810;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj42744557 = -137398310;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj83438952 = -407290585;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj78509750 = -523539519;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj95601225 = -113584445;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj30503509 = -962327339;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj33211878 = -69164908;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj70947878 = -715338913;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj29185962 = -467068645;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj38353094 = 83908139;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj32422385 = -306253028;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj86391701 = -330995377;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj63959776 = -361797673;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj1052483 = -162481433;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj5453288 = -54793106;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj13241606 = -891239392;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj68957001 = -696392753;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj40977925 = -900208707;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj3785436 = -742367934;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj17094469 = -756169389;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj45185400 = -794512692;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj94448613 = -832636912;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj14412155 = -708885412;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj533639 = -398063070;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj79353290 = -195780235;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj73492104 = -132923188;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj27438055 = -765326835;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj12020848 = -624276674;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj98519951 = -882294122;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj27378442 = -478558990;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj13660391 = -170076278;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj22665130 = -216116701;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj45639382 = -501610364;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj5067230 = -351027950;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj20675228 = -918205701;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj30337401 = -364608252;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj58660877 = -567750027;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj23389769 = -446966394;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj14465190 = -89499654;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj98075177 = -175422017;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj45440885 = -803045971;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj71082857 = -282818585;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj62497007 = -152018927;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj46898355 = -627204714;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj75646717 = -311534104;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj56334883 = -546949640;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj37310882 = -804891132;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj34259929 = -470952176;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj18430476 = -929290138;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj35163960 = -63653652;    double etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj48992020 = -877524826;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj69116488 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj17873396;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj17873396 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj74513424;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj74513424 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj4125516;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj4125516 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj90461488;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj90461488 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj69227440;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj69227440 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj86852051;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj86852051 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj11195838;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj11195838 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj78535244;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj78535244 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj55341435;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj55341435 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj70856520;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj70856520 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj51044340;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj51044340 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj32364122;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj32364122 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj95747231;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj95747231 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj87216135;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj87216135 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj86131582;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj86131582 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj99663602;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj99663602 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj8602741;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj8602741 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj22079875;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj22079875 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj74762763;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj74762763 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj62750466;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj62750466 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj35904610;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj35904610 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj46925517;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj46925517 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj45327463;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj45327463 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj65772422;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj65772422 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj58039103;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj58039103 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj82835738;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj82835738 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj22892275;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj22892275 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj10143355;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj10143355 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj73081957;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj73081957 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj42099830;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj42099830 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj1899519;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj1899519 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj10066416;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj10066416 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj28578687;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj28578687 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj78652763;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj78652763 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj42030736;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj42030736 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj91682968;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj91682968 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj85251448;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj85251448 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj8069103;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj8069103 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj42726584;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj42726584 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj1270659;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj1270659 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj35312412;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj35312412 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj23883763;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj23883763 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj48405566;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj48405566 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj17949021;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj17949021 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj52112031;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj52112031 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj35373974;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj35373974 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj37196357;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj37196357 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj17825045;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj17825045 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj89468126;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj89468126 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj42744557;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj42744557 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj83438952;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj83438952 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj78509750;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj78509750 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj95601225;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj95601225 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj30503509;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj30503509 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj33211878;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj33211878 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj70947878;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj70947878 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj29185962;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj29185962 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj38353094;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj38353094 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj32422385;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj32422385 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj86391701;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj86391701 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj63959776;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj63959776 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj1052483;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj1052483 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj5453288;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj5453288 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj13241606;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj13241606 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj68957001;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj68957001 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj40977925;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj40977925 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj3785436;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj3785436 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj17094469;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj17094469 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj45185400;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj45185400 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj94448613;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj94448613 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj14412155;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj14412155 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj533639;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj533639 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj79353290;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj79353290 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj73492104;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj73492104 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj27438055;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj27438055 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj12020848;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj12020848 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj98519951;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj98519951 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj27378442;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj27378442 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj13660391;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj13660391 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj22665130;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj22665130 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj45639382;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj45639382 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj5067230;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj5067230 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj20675228;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj20675228 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj30337401;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj30337401 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj58660877;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj58660877 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj23389769;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj23389769 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj14465190;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj14465190 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj98075177;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj98075177 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj45440885;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj45440885 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj71082857;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj71082857 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj62497007;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj62497007 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj46898355;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj46898355 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj75646717;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj75646717 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj56334883;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj56334883 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj37310882;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj37310882 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj34259929;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj34259929 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj18430476;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj18430476 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj35163960;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj35163960 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj48992020;     etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj48992020 = etZDRIBZMsAvYUYpagfGxNJBItogmnXhNotoVkSxlMgNwQQgRMytOEoRjYvYUUnZUThkbj69116488;}
// Junk Finished
