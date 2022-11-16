#include "Menu.h"
#include "MenuControls.h"
#include "Dropdown.h"
#include "../HNJ.h"
#include "../HanaLovesMe/HanaLovesMe.h"
#include "../HanaLovesMe/HanaLovesMe_impl_dx9.h"
#include "../HanaLovesMe/HanaLovesMe_impl_win32.h"
#include "../HanaLovesMe/HanaLovesMe_internal.h"
#include <d3d9.h>
#include <string>
#include "../globals/HanaSong.h"
#include "../Options.hpp"
 // hack
#include "..//helpers/Config.hpp"
#include "..//Install.hpp"
#include "..//Structs.hpp"
#include "..//features/Miscellaneous.hpp"
#include "..//features/KitParser.hpp"
#include "..//features/Skinchanger.hpp"
#include "..//BASS/bass.h"
#include "..//BASS/API.h"
#include "..//Install.hpp"
//#include "..//Handlers.h"
#include "..//Gamehooking.hpp"
#include "..//helpers/Utils.hpp"
#include "..//helpers/HanaLovesYou.h"
#include  <functional>
static int tab = 0;
static int weapontab = 0;


bool input_shouldListen;
ButtonCode_t* input_receivedKeyval;

extern bool unload;

Menu _menu;

typedef unsigned int uint;

std::string HanaLovesMehook(uint l = 15, std::string charIndex = "abcdefghijklmnaoqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890<>![]")
{

	uint length = rand() % l + 1;

	uint ri[15];

	for (uint i = 0; i < length; ++i)
		ri[i] = rand() % charIndex.length();


	std::string rs = "HanaLovesMe - " "";


	for (uint i = 0; i < length; ++i)
		rs += charIndex[ri[i]];

	if (rs.empty()) HanaLovesMehook(l, charIndex);


	else return rs;

}

void Menu::ColorPicker(const char* name, float* color, bool alpha) {

	HanaLovesMeWindow* window = HanaLovesMe::GetCurrentWindow();
	HanaLovesMeStyle* style = &HanaLovesMe::GetStyle();

	auto alphaSliderFlag = alpha ? HanaLovesMeColorEditFlags_AlphaBar : HanaLovesMeColorEditFlags_NoAlpha;

	HanaLovesMe::SameLine(219.f);
	HanaLovesMe::ColorEdit4(std::string{ "##" }.append(name).append("Picker").c_str(), color, alphaSliderFlag | HanaLovesMeColorEditFlags_NoInputs | HanaLovesMeColorEditFlags_NoTooltip);
}
//void sendmes()
//{
//	_menu.MessageSend();
//}
namespace HanaLovesMe
{

	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values, int height_in_items = -1)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size(), height_in_items);
	}

	static bool ListBox(const char* label, int* current_item, std::function<const char* (int)> lambda, int items_count, int height_in_items)
	{
		return HanaLovesMe::ListBox(label, current_item, [](void* data, int idx, const char** out_text)
			{
				*out_text = (*reinterpret_cast<std::function<const char* (int)>*>(data))(idx);
				return true;
			}, &lambda, items_count, height_in_items);
	}

	bool LabelClick(const char* concatoff, const char* concaton, const char* label, bool* v, const char* unique_id)
	{
		HanaLovesMeWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		// The concatoff/on thingies were for my weapon config system so if we're going to make that, we still need this aids.
		char Buf[64];
		_snprintf(Buf, 62, "%s%s", ((*v) ? concatoff : concaton), label);

		HanaLovesMeContext& g = *GHanaLovesMe;
		const HanaLovesMeStyle& style = g.Style;
		const HanaLovesMeID id = window->GetID(unique_id);
		const vsize label_size = CalcTextSize(label, NULL, true);

		const vsrect check_bb(window->DC.CursorPos, window->DC.CursorPos + vsize(label_size.y + style.FramePadding.y * 2, label_size.y + style.FramePadding.y * 2));
		ItemSize(check_bb, style.FramePadding.y);

		vsrect total_bb = check_bb;
		if (label_size.x > 0)
			SameLine(0, style.ItemInnerSpacing.x);

		const vsrect text_bb(window->DC.CursorPos + vsize(0, style.FramePadding.y), window->DC.CursorPos + vsize(0, style.FramePadding.y) + label_size);
		if (label_size.x > 0)
		{
			ItemSize(vsize(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
			total_bb = vsrect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
		}

		if (!ItemAdd(total_bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
		if (pressed)
			* v = !(*v);

		if (label_size.x > 0.0f)
			RenderText(check_bb.GetTL(), Buf);

		return pressed;
	}

	void KeyBindButton(ButtonCode_t* key)
	{
		bool clicked = false;

		std::string text = g_InputSystem->ButtonCodeToString(*key);
		std::string unique_id = std::to_string((int)key);

		if (*key <= BUTTON_CODE_NONE)
			text = "Key not set";

		if (input_shouldListen && input_receivedKeyval == key) {
			clicked = true;
			text = "...";
		}
		text += "]";

		HanaLovesMe::SameLine(); HanaLovesMe::SameLine(170.f);
		HanaLovesMe::LabelClick("[", "[", text.c_str(), &clicked, unique_id.c_str());

		if (clicked)
		{
			input_shouldListen = true;
			input_receivedKeyval = key;
		}


		if (*key == KEY_DELETE)
		{
			*key = BUTTON_CODE_NONE;
		}

		if (HanaLovesMe::IsItemHovered())
			HanaLovesMe::SetTooltip("Bind the \"del\" key to remove current bind.");
	}

	void SelectTabs(int* selected, const char* items[], int item_count, vsize size = vsize(0, 0))
	{
		auto color_grayblue = GetColorU32(xuifloatcolor(0.15, 0.15, 0.15, 1));
		auto color_deepblue = GetColorU32(xuifloatcolor(0.09, 0.09, 0.09, 1));
		auto color_shade_hover = GetColorU32(xuifloatcolor(0.70, 0.70, 0.70, 0.05));
		auto color_shade_clicked = GetColorU32(xuifloatcolor(0.38, 0.38, 0.38, 0.1));
		auto color_black_outlines = GetColorU32(xuifloatcolor(0, 0, 0, 1));

		HanaLovesMeStyle& style = GetStyle();
		HanaLovesMeWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return;

		std::string names;
		for (int32_t i = 0; i < item_count; i++)
			names += items[i];

		HanaLovesMeContext* g = GHanaLovesMe;
		const HanaLovesMeID id = window->GetID(names.c_str());
		const vsize label_size = CalcTextSize(names.c_str(), NULL, true);

		vsize Min = window->DC.CursorPos;
		vsize Max = ((size.x <= 0 || size.y <= 0) ? vsize(Min.x + GetContentRegionMax().x - style.WindowPadding.x, Min.y + label_size.y * 2) : Min + size);

		vsrect bb(Min, Max);
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return;

		PushClipRect(vsize(Min.x, Min.y - 1), vsize(Max.x, Max.y + 1), false);

		window->DrawList->AddRectFilledMultiColor(Min, Max, color_grayblue, color_grayblue, color_deepblue, color_deepblue); // Main gradient.

		vsize mouse_pos = GetMousePos();
		bool mouse_click = g->IO.MouseClicked[0];

		float TabSize = ceil((Max.x - Min.x) / item_count);

		for (int32_t i = 0; i < item_count; i++)
		{
			vsize Min_cur_label = vsize(Min.x + (int)TabSize * i, Min.y);
			vsize Max_cur_label = vsize(Min.x + (int)TabSize * i + (int)TabSize, Max.y);

			// Imprecision clamping. gay but works :^)
			Max_cur_label.x = (Max_cur_label.x >= Max.x ? Max.x : Max_cur_label.x);

			if (mouse_pos.x > Min_cur_label.x && mouse_pos.x < Max_cur_label.x &&
				mouse_pos.y > Min_cur_label.y && mouse_pos.y < Max_cur_label.y)
			{
				if (mouse_click)
					*selected = i;
				else if (i != *selected)
					window->DrawList->AddRectFilled(Min_cur_label, Max_cur_label, color_shade_hover);
			}

			if (i == *selected) {
				window->DrawList->AddRectFilled(Min_cur_label, Max_cur_label, color_shade_clicked);
				window->DrawList->AddRectFilledMultiColor(Min_cur_label, Max_cur_label, color_deepblue, color_deepblue, color_grayblue, color_grayblue);
				window->DrawList->AddLine(vsize(Min_cur_label.x - 1.5f, Min_cur_label.y - 1), vsize(Max_cur_label.x - 0.5f, Min_cur_label.y - 1), color_black_outlines);
			}
			else
				window->DrawList->AddLine(vsize(Min_cur_label.x - 1, Min_cur_label.y), vsize(Max_cur_label.x, Min_cur_label.y), color_black_outlines);
			window->DrawList->AddLine(vsize(Max_cur_label.x - 1, Max_cur_label.y), vsize(Max_cur_label.x - 1, Min_cur_label.y - 0.5f), color_black_outlines);

			const vsize text_size = CalcTextSize(items[i], NULL, true);
			float pad_ = style.FramePadding.x + g->FontSize + style.ItemInnerSpacing.x;
			vsrect tab_rect(Min_cur_label, Max_cur_label);
			RenderTextClipped(Min_cur_label, Max_cur_label, items[i], NULL, &text_size, style.WindowTitleAlign, &tab_rect);
		}

		window->DrawList->AddLine(vsize(Min.x, Min.y - 0.5f), vsize(Min.x, Max.y), color_black_outlines);
		window->DrawList->AddLine(vsize(Min.x, Max.y), Max, color_black_outlines);
		PopClipRect();
	}
	
}

void Menu::Render() {

	HanaLovesMeStyle* style = &HanaLovesMe::GetStyle();

	style->WindowPadding = vsize(6, 6);

	HanaLovesMe::PushFont(globals::menuFont);

	HanaLovesMe::SetNextWindowSize(vsize(660.f, 610.f));
	HanaLovesMe::BeginMenuBackground("ChainMail", &Menu::Get().isOpen, HanaLovesMeWindowFlags_NoCollapse | HanaLovesMeWindowFlags_NoResize | HanaLovesMeWindowFlags_NoScrollbar | HanaLovesMeWindowFlags_NoScrollWithMouse | HanaLovesMeWindowFlags_NoTitleBar); {

		HanaLovesMe::BeginChild("Complete Border", vsize(648.f, 598.f), false); {

			HanaLovesMe::Image(globals::menuBg, vsize(648.f, 598.f));

		} HanaLovesMe::EndChild();

		HanaLovesMe::SameLine(6.f);

		style->Colors[HanaLovesMeCol_ChildBg] = xuicolor(0, 0, 0, 0);

		HanaLovesMe::BeginChild("Menu Contents", vsize(648.f, 598.f), false); {

			HanaLovesMe::ColorBar("unicorn", vsize(648.f, 2.f));

			style->ItemSpacing = vsize(0.f, -1.f);

			HanaLovesMe::BeginTabs("Tabs", vsize(75.f, 592), false); {

				style->ItemSpacing = vsize(0.f, 0.f);

				style->ButtonTextAlign = vsize(0.5f, 0.47f);

				HanaLovesMe::PopFont();
				HanaLovesMe::PushFont(globals::tabFont);

				switch (tab) {

				case 0:
					HanaLovesMe::TabSpacer("##Top Spacer", vsize(75.f, 11.f));

					if (HanaLovesMe::SelectedTab("A", vsize(75.f, 63.f))) tab = 0;
					if (HanaLovesMe::Tab("G", vsize(75.f, 63.f))) tab = 1;
					if (HanaLovesMe::Tab("B", vsize(75.f, 63.f))) tab = 2;
					if (HanaLovesMe::Tab("C", vsize(75.f, 63.f))) tab = 3;
					if (HanaLovesMe::Tab("D", vsize(75.f, 63.f))) tab = 4;
					if (HanaLovesMe::Tab("E", vsize(75.f, 63.f))) tab = 5;
					if (HanaLovesMe::Tab("F", vsize(75.f, 63.f))) tab = 6;
					if (HanaLovesMe::Tab("H", vsize(75.f, 63.f))) tab = 7;
					if (HanaLovesMe::Tab("I", vsize(75.f, 63.f))) tab = 8;

					HanaLovesMe::TabSpacer2("##Bottom Spacer", vsize(75.f, 14.f));
					break;
				case 1:
					HanaLovesMe::TabSpacer("##Top Spacer", vsize(75.f, 11.f));

					if (HanaLovesMe::Tab("A", vsize(75.f, 63.f))) tab = 0;
					if (HanaLovesMe::SelectedTab("G", vsize(75.f, 63.f))) tab = 1;
					if (HanaLovesMe::Tab("B", vsize(75.f, 63.f))) tab = 2;
					if (HanaLovesMe::Tab("C", vsize(75.f, 63.f))) tab = 3;
					if (HanaLovesMe::Tab("D", vsize(75.f, 63.f))) tab = 4;
					if (HanaLovesMe::Tab("E", vsize(75.f, 63.f))) tab = 5;
					if (HanaLovesMe::Tab("F", vsize(75.f, 63.f))) tab = 6;
					if (HanaLovesMe::Tab("H", vsize(75.f, 63.f))) tab = 7;
					if (HanaLovesMe::Tab("I", vsize(75.f, 63.f))) tab = 8;

					HanaLovesMe::TabSpacer2("##Bottom Spacer", vsize(75.f, 14.f));
					break;
				case 2:
					HanaLovesMe::TabSpacer("##Top Spacer", vsize(75.f, 11.f));

					if (HanaLovesMe::Tab("A", vsize(75.f, 63.f))) tab = 0;
					if (HanaLovesMe::Tab("G", vsize(75.f, 63.f))) tab = 1;
					if (HanaLovesMe::SelectedTab("B", vsize(75.f, 63.f))) tab = 2;
					if (HanaLovesMe::Tab("C", vsize(75.f, 63.f))) tab = 3;
					if (HanaLovesMe::Tab("D", vsize(75.f, 63.f))) tab = 4;
					if (HanaLovesMe::Tab("E", vsize(75.f, 63.f))) tab = 5;
					if (HanaLovesMe::Tab("F", vsize(75.f, 63.f))) tab = 6;
					if (HanaLovesMe::Tab("H", vsize(75.f, 63.f))) tab = 7;
					if (HanaLovesMe::Tab("I", vsize(75.f, 63.f))) tab = 8;

					HanaLovesMe::TabSpacer2("##Bottom Spacer", vsize(75.f, 14.f));
					break;
				case 3:
					HanaLovesMe::TabSpacer("##Top Spacer", vsize(75.f, 11.f));

					if (HanaLovesMe::Tab("A", vsize(75.f, 63.f))) tab = 0;
					if (HanaLovesMe::Tab("G", vsize(75.f, 63.f))) tab = 1;
					if (HanaLovesMe::Tab("B", vsize(75.f, 63.f))) tab = 2;
					if (HanaLovesMe::SelectedTab("C", vsize(75.f, 63.f))) tab = 3;
					if (HanaLovesMe::Tab("D", vsize(75.f, 63.f))) tab = 4;
					if (HanaLovesMe::Tab("E", vsize(75.f, 63.f))) tab = 5;
					if (HanaLovesMe::Tab("F", vsize(75.f, 63.f))) tab = 6;
					if (HanaLovesMe::Tab("H", vsize(75.f, 63.f))) tab = 7;
					if (HanaLovesMe::Tab("I", vsize(75.f, 63.f))) tab = 8;

					HanaLovesMe::TabSpacer2("##Bottom Spacer", vsize(75.f, 14.f));
					break;
				case 4:
					HanaLovesMe::TabSpacer("##Top Spacer", vsize(75.f, 11.f));

					if (HanaLovesMe::Tab("A", vsize(75.f, 63.f))) tab = 0;
					if (HanaLovesMe::Tab("G", vsize(75.f, 63.f))) tab = 1;
					if (HanaLovesMe::Tab("B", vsize(75.f, 63.f))) tab = 2;
					if (HanaLovesMe::Tab("C", vsize(75.f, 63.f))) tab = 3;
					if (HanaLovesMe::SelectedTab("D", vsize(75.f, 63.f))) tab = 4;
					if (HanaLovesMe::Tab("E", vsize(75.f, 63.f))) tab = 5;
					if (HanaLovesMe::Tab("F", vsize(75.f, 63.f))) tab = 6;
					if (HanaLovesMe::Tab("H", vsize(75.f, 63.f))) tab = 7;
					if (HanaLovesMe::Tab("I", vsize(75.f, 63.f))) tab = 8;

					HanaLovesMe::TabSpacer2("##Bottom Spacer", vsize(75.f, 14.f));
					break;
				case 5:
					HanaLovesMe::TabSpacer("##Top Spacer", vsize(75.f, 11.f));

					if (HanaLovesMe::Tab("A", vsize(75.f, 63.f))) tab = 0;
					if (HanaLovesMe::Tab("G", vsize(75.f, 63.f))) tab = 1;
					if (HanaLovesMe::Tab("B", vsize(75.f, 63.f))) tab = 2;
					if (HanaLovesMe::Tab("C", vsize(75.f, 63.f))) tab = 3;
					if (HanaLovesMe::Tab("D", vsize(75.f, 63.f))) tab = 4;
					if (HanaLovesMe::SelectedTab("E", vsize(75.f, 63.f))) tab = 5;
					if (HanaLovesMe::Tab("F", vsize(75.f, 63.f))) tab = 6;
					if (HanaLovesMe::Tab("H", vsize(75.f, 63.f))) tab = 7;
					if (HanaLovesMe::Tab("I", vsize(75.f, 63.f))) tab = 8;

					HanaLovesMe::TabSpacer2("##Bottom Spacer", vsize(75.f, 14.f));
					break;
				case 6:
					HanaLovesMe::TabSpacer("##Top Spacer", vsize(75.f, 11.f));

					if (HanaLovesMe::Tab("A", vsize(75.f, 63.f))) tab = 0;
					if (HanaLovesMe::Tab("G", vsize(75.f, 63.f))) tab = 1;
					if (HanaLovesMe::Tab("B", vsize(75.f, 63.f))) tab = 2;
					if (HanaLovesMe::Tab("C", vsize(75.f, 63.f))) tab = 3;
					if (HanaLovesMe::Tab("D", vsize(75.f, 63.f))) tab = 4;
					if (HanaLovesMe::Tab("E", vsize(75.f, 63.f))) tab = 5;
					if (HanaLovesMe::SelectedTab("F", vsize(75.f, 63.f))) tab = 6;
					if (HanaLovesMe::Tab("H", vsize(75.f, 63.f))) tab = 7;
					if (HanaLovesMe::Tab("I", vsize(75.f, 63.f))) tab = 8;

					HanaLovesMe::TabSpacer2("##Bottom Spacer", vsize(75.f, 14.f));
					break;
				case 7:
					HanaLovesMe::TabSpacer("##Top Spacer", vsize(75.f, 11.f));

					if (HanaLovesMe::Tab("A", vsize(75.f, 63.f))) tab = 0;
					if (HanaLovesMe::Tab("G", vsize(75.f, 63.f))) tab = 1;
					if (HanaLovesMe::Tab("B", vsize(75.f, 63.f))) tab = 2;
					if (HanaLovesMe::Tab("C", vsize(75.f, 63.f))) tab = 3;
					if (HanaLovesMe::Tab("D", vsize(75.f, 63.f))) tab = 4;
					if (HanaLovesMe::Tab("E", vsize(75.f, 63.f))) tab = 5;
					if (HanaLovesMe::Tab("F", vsize(75.f, 63.f))) tab = 6;
					if (HanaLovesMe::SelectedTab("H", vsize(75.f, 63.f))) tab = 7;
					if (HanaLovesMe::Tab("I", vsize(75.f, 63.f))) tab = 8;

					HanaLovesMe::TabSpacer2("##Bottom Spacer", vsize(75.f, 14.f));
					break;
				case 8:
					HanaLovesMe::TabSpacer("##Top Spacer", vsize(75.f, 11.f));

					if (HanaLovesMe::Tab("A", vsize(75.f, 63.f))) tab = 0;
					if (HanaLovesMe::Tab("G", vsize(75.f, 63.f))) tab = 1;
					if (HanaLovesMe::Tab("B", vsize(75.f, 63.f))) tab = 2;
					if (HanaLovesMe::Tab("C", vsize(75.f, 63.f))) tab = 3;
					if (HanaLovesMe::Tab("D", vsize(75.f, 63.f))) tab = 4;
					if (HanaLovesMe::Tab("E", vsize(75.f, 63.f))) tab = 5;
					if (HanaLovesMe::Tab("F", vsize(75.f, 63.f))) tab = 6;
					if (HanaLovesMe::Tab("H", vsize(75.f, 63.f))) tab = 7;
					if (HanaLovesMe::SelectedTab("I", vsize(75.f, 63.f))) tab = 8;

					HanaLovesMe::TabSpacer2("##Bottom Spacer", vsize(75.f, 14.f));
					break;
				}

				HanaLovesMe::PopFont();
				HanaLovesMe::PushFont(globals::menuFont);

				style->ButtonTextAlign = vsize(0.5f, 0.5f);

			} HanaLovesMe::EndTabs();

			HanaLovesMe::SameLine(75.f); // Nothing in this CPP is wrong.... 

			HanaLovesMe::BeginChild("Tab Contents", vsize(572.f, 592), false); {

				style->Colors[HanaLovesMeCol_Border] = xuicolor(0, 0, 0, 0);

				switch (tab) {

				case 0:
					Aimbot();
					break;
				case 1:
					Antiaim();
					break;
				case 2:
					lgtpew();
					break;
				case 3:
					Visuals();
					break;
				case 4:
					Misc();
					break;
				case 5:
					Skins();
					break;
				case 6:
					Players();
					break;
				case 7:
					ConfigTab();
					break;
				case 8:
					LuaTab();

				}

				style->Colors[HanaLovesMeCol_Border] = xuicolor(0, 0, 0, 255);

			} HanaLovesMe::EndChild();

			style->ItemSpacing = vsize(4.f, 4.f);
			style->Colors[HanaLovesMeCol_ChildBg] = xuicolor(23, 23, 23, 255);

		} HanaLovesMe::EndChild();
		HanaLovesMe::PopFont();

	} HanaLovesMe::End();
}

void Menu::Shutdown() {

	HanaLovesMe_ImplDX9_Shutdown();
	HanaLovesMe_ImplWin32_Shutdown();
}

void Menu::Aimbot() {

	HanaLovesMeStyle* style = &HanaLovesMe::GetStyle();
	InsertSpacer(HanaLovesYou("Top Spacer"));

	static int weapontab_rage = 0;


	InsertGroupBoxTop(HanaLovesYou("Weapon Selection"), vsize(535.f, 61.f)); {

		HanaLovesMe::Columns(9, nullptr, false);
		HanaLovesMe::PopFont();
		HanaLovesMe::PushFont(globals::weaponzFont);
		style->ButtonTextAlign = vsize(0.4f, 0.75f);
		HanaLovesMe::PushItemWidth(158.f);

		style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
		style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
		style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
		style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
		style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

		if (HanaLovesMe::TabButton("J", vsize(50, 45))) weapontab_rage = 0;

		HanaLovesMe::NextColumn();
		{
			if (HanaLovesMe::TabButton("C", vsize(50, 45))) weapontab_rage = 1;
		}
		HanaLovesMe::NextColumn();
		{
			if (HanaLovesMe::TabButton("N", vsize(50, 45))) weapontab_rage = 2;
		}
		HanaLovesMe::NextColumn();
		{
			if (HanaLovesMe::TabButton("d", vsize(62, 45))) weapontab_rage = 3;
		}
		HanaLovesMe::NextColumn();
		{
			if (HanaLovesMe::TabButton("f", vsize(62, 45))) weapontab_rage = 4;
		}
		HanaLovesMe::NextColumn();
		{
			if (HanaLovesMe::TabButton("W", vsize(62, 45))) weapontab_rage = 5;
		}
		HanaLovesMe::NextColumn();
		{
			if (HanaLovesMe::TabButton("a", vsize(62, 45))) weapontab_rage = 6;
		}
		HanaLovesMe::NextColumn();
		{
			if (HanaLovesMe::TabButton("Z", vsize(62, 45))) weapontab_rage = 7;
		}
		HanaLovesMe::NextColumn();
		{
			if (HanaLovesMe::TabButton("X", vsize(62, 45))) weapontab_rage = 8;
		}

	} InsertEndGroupBoxTop(HanaLovesYou("Weapon Selection Cover"), HanaLovesYou("Weapon Selection"), vsize(536.f, 11.f));

	InsertSpacer(HanaLovesYou("Weapon Selection - Main Group boxes Spacer"));

	HanaLovesMe::Columns(2, NULL, false); {

		InsertGroupBoxLeft(HanaLovesYou("Aimbot"), 477.f); {


			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

			InsertCheckbox(HanaLovesYou("Enable"), g_Options.rage_enabled);
			InsertCheckbox(HanaLovesYou("Automatic fire"), g_Options.rage_autoshoot);
			InsertCheckbox(HanaLovesYou("Automatic scope"), g_Options.rage_autoscope);
			InsertCheckbox(HanaLovesYou("Automatic crouch"), g_Options.rage_autocrouch);
			InsertCheckbox(HanaLovesYou("Automatic stop"), g_Options.rage_autostop);
			InsertCheckbox(HanaLovesYou("Silent aim"), g_Options.rage_silent);
			InsertCheckbox(HanaLovesYou("Override R8"), g_Options.rage_autocockrevolver);
			InsertSlider(HanaLovesYou("Field of view"), g_Options.rage_fieldofview, 0.f, 180.0f, "%1.f%");
			InsertCheckbox(HanaLovesYou("Anti-aim correction"), g_Options.hvh_resolver);
			HanaLovesMe::PushStyleColor(HanaLovesMeCol_Text, xuifloatcolor(0.95f, 0.76f, 0.06f, 1.00f));
			InsertCheckbox(HanaLovesYou("Remove recoil"), g_Options.rage_norecoil);
			HanaLovesMe::PopStyleColor();
			HanaLovesMe::PushStyleColor(HanaLovesMeCol_Text, xuifloatcolor(0.95f, 0.76f, 0.06f, 1.00f));
			InsertCheckbox(HanaLovesYou("No-spread fix"), g_Options.rage_nospread);
			HanaLovesMe::PopStyleColor();
			InsertCheckbox("Position adjustment (backtrack)(RIP FPS)", g_Options.rage_lagcompensation);
			InsertCheckbox("Only shoot backtrack if needed", g_Options.backtrack_bhd_wall_only);
			InsertCheckbox(HanaLovesYou("Force body aim"), g_Options.rage_autobaim);
			InsertCheckbox(HanaLovesYou("Override resolver"), g_Options.hvh_resolver_override);
			InsertCheckbox(HanaLovesYou("Reverse resolver"), g_Options.hvh_resolver_custom);
			InsertCheckbox(HanaLovesYou("Resolver flip"), g_Options.resolver_flip_after_x_shots);
			InsertSlider(HanaLovesYou("Shots to flip after:"), g_Options.resolver_flip_after_missed_shots, 0.f, 10.f, "%1.f%");
			InsertCheckbox(HanaLovesYou("Automatic resolver flip"), g_Options.resolver_autoflip);

			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);



		} InsertEndGroupBoxLeft(HanaLovesYou("Aimbot Cover"), HanaLovesYou("Aimbot"));
	}
	HanaLovesMe::NextColumn(); {

		InsertGroupBoxRight(HanaLovesYou("Settings"), 477.f); {


			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);
			
			switch (weapontab_rage)
			{
			case 0:
				InsertSlider(HanaLovesYou("R8/Deagle minimum damage"), g_Options.rage_mindmg_amount_deagr8, 0.f, 120.f, "%1.fhp%");
				InsertSlider(HanaLovesYou("R8/Deagle hitchance"), g_Options.rage_hitchance_amount_deagr8, 0.f, 100.f, "%1.f%%");
				InsertCheckbox(HanaLovesYou("R8/Deagle hitbox priority"), g_Options.bRage_prioritize_deagr8);
				InsertCombo(HanaLovesYou("##R8HIT"), g_Options.iRage_hitbox_deagr8, rgbthitboxes);
				InsertSlider(HanaLovesYou("R8/Deagle hitbox scale"), g_Options.rage_pointscale_amount_deagr8, 0.1f, 1.0f, "%0.1fx%");
				InsertCheckbox(HanaLovesYou("R8/Deagle multi-point"), g_Options.bRage_multipoint_deagr8);

				HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				HanaLovesMe::BeginChild(HanaLovesYou("#MULTIPOINTSR8"), vsize(158, 160), true, HanaLovesMeWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					HanaLovesMe::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesDeagR8[i]);
				}
				HanaLovesMe::EndChild();
			break;

			case 1:
				InsertSlider(HanaLovesYou("Pistol minimum damage"), g_Options.rage_mindmg_amount_pistol, 0.f, 120.f, "%1.fhp%");
				InsertSlider(HanaLovesYou("Pistol hitchance"), g_Options.rage_hitchance_amount_pistol, 0.f, 100.f, "%1.f%%");
				InsertCheckbox(HanaLovesYou("Pistol hitbox priority"), g_Options.bRage_prioritize_pistol);
				InsertCombo(HanaLovesYou("##PISTOLHIT"), g_Options.iRage_hitbox_pistol, rgbthitboxes);
				InsertSlider(HanaLovesYou("Pistol hitbox scale"), g_Options.rage_pointscale_amount_pistol, 0.1f, 1.0f, "%0.1fx%");
				InsertCheckbox(HanaLovesYou("Pistol multi-point"), g_Options.bRage_multipoint_pistol);

				HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				HanaLovesMe::BeginChild(HanaLovesYou("#MULTIPOINTSPISTOL"), vsize(158, 160), true, HanaLovesMeWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					HanaLovesMe::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesPistol[i]);
				}
				HanaLovesMe::EndChild();
			break;

			case 2:
				InsertSlider(HanaLovesYou("SMG minimum damage"), g_Options.rage_mindmg_amount_smg, 0.f, 120.f, "%1.fhp%");
				InsertSlider(HanaLovesYou("SMG hitchance"), g_Options.rage_hitchance_amount_smg, 0.f, 100.f, "%1.f%%");
				InsertCheckbox(HanaLovesYou("SMG hitbox priority"), g_Options.bRage_prioritize_smg);
				InsertCombo(HanaLovesYou("##SMGHIT"), g_Options.iRage_hitbox_smg, rgbthitboxes);
				InsertSlider(HanaLovesYou("SMG hitbox scale"), g_Options.rage_pointscale_amount_smg, 0.1f, 1.0f, "%0.1fx%");
				InsertCheckbox(HanaLovesYou("SMG multi-point"), g_Options.bRage_multipoint_smg);

				HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				HanaLovesMe::BeginChild(HanaLovesYou("#MULTIPOINTSSMG"), vsize(158, 160), true, HanaLovesMeWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					HanaLovesMe::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesSMG[i]);
				}
				HanaLovesMe::EndChild();
			break;

			case 3:
				InsertSlider(HanaLovesYou("Shotgun minimum damage"), g_Options.rage_mindmg_amount_shotgun, 0.f, 120.f, "%1.fhp%");
				InsertSlider(HanaLovesYou("Shotgun hitchance"), g_Options.rage_hitchance_amount_shotgun, 0.f, 100.f, "%1.f%%");
				InsertCheckbox(HanaLovesYou("Shotgun hitbox priority"), g_Options.bRage_prioritize_shotgun);
				InsertCombo(HanaLovesYou("##SHOTGUNHIT"), g_Options.iRage_hitbox_shotgun, rgbthitboxes);
				InsertSlider(HanaLovesYou("Shotgun hitbox scale"), g_Options.rage_pointscale_amount_shotgun, 0.1f, 1.0f, "%0.1fx%");
				InsertCheckbox(HanaLovesYou("Shotgun multi-point"), g_Options.bRage_multipoint_shotgun);

				HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				HanaLovesMe::BeginChild(HanaLovesYou("#MULTIPOINTSSHOTGUN"), vsize(158, 160), true, HanaLovesMeWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					HanaLovesMe::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesShotgun[i]);
				}
				HanaLovesMe::EndChild();
			break;

			case 4:
				InsertSlider(HanaLovesYou("Machinegun minimum damage"), g_Options.rage_mindmg_amount_mg, 0.f, 120.f, "%1.fhp%");
				InsertSlider(HanaLovesYou("Machinegun hitchance"), g_Options.rage_hitchance_amount_mg, 0.f, 100.f, "%1.f%%");
				InsertCheckbox(HanaLovesYou("Machinegun hitbox priority"), g_Options.bRage_prioritize_mg);
				InsertCombo(HanaLovesYou("##MACHINEGUNHIT"), g_Options.iRage_hitbox_mg, rgbthitboxes);
				InsertSlider(HanaLovesYou("Machinegun hitbox scale"), g_Options.rage_pointscale_amount_mg, 0.1f, 1.0f, "%0.1fx%");
				InsertCheckbox(HanaLovesYou("Machinegun multi-point"), g_Options.bRage_multipoint_mg);

				HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				HanaLovesMe::BeginChild(HanaLovesYou("#MULTIPOINTSMACHINEGUN"), vsize(158, 160), true, HanaLovesMeWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					HanaLovesMe::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesMG[i]);
				}
				HanaLovesMe::EndChild();
			break;

			case 5:
				InsertSlider(HanaLovesYou("Rifle minimum damage"), g_Options.rage_mindmg_amount_assaultrifle, 0.f, 120.f, "%1.fhp%");
				InsertSlider(HanaLovesYou("Rifle hitchance"), g_Options.rage_hitchance_amount_assaultrifle, 0.f, 100.f, "%1.f%%");
				InsertCheckbox(HanaLovesYou("Rifle hitbox priority"), g_Options.bRage_prioritize_assaultrifle);
				InsertCombo(HanaLovesYou("##ARHIT"), g_Options.iRage_hitbox_assaultrifle, rgbthitboxes);
				InsertSlider(HanaLovesYou("Rifle hitbox scale"), g_Options.rage_pointscale_amount_assaultrifle, 0.1f, 1.0f, "%0.1fx%");
				InsertCheckbox(HanaLovesYou("Rifle multi-point"), g_Options.bRage_multipoint_assaultrifle);

				HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				HanaLovesMe::BeginChild(HanaLovesYou("#MULTIPOINTSAR"), vsize(158, 160), true, HanaLovesMeWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					HanaLovesMe::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesAssaultRifle[i]);
				}
				HanaLovesMe::EndChild();
			break;

			case 6:
				InsertSlider(HanaLovesYou("Scout minimum damage"), g_Options.rage_mindmg_amount_scout, 0.f, 120.f, "%1.fhp%");
				InsertSlider(HanaLovesYou("Scout hitchance"), g_Options.rage_hitchance_amount_scout, 0.f, 100.f, "%1.f%%");
				InsertCheckbox(HanaLovesYou("Scout hitbox priority"), g_Options.bRage_prioritize_scout);
				InsertCombo(HanaLovesYou("##SCOUTHIT"), g_Options.iRage_hitbox_scout, rgbthitboxes);
				InsertSlider(HanaLovesYou("Scout hitbox scale"), g_Options.rage_pointscale_amount_scout, 0.1f, 1.0f, "%0.1fx%");
				InsertCheckbox(HanaLovesYou("Scout multi-point"), g_Options.bRage_multipoint_scout);

				HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				HanaLovesMe::BeginChild(HanaLovesYou("#MULTIPOINTSSCOUT"), vsize(158, 160), true, HanaLovesMeWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					HanaLovesMe::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesScout[i]);
				}
				HanaLovesMe::EndChild();
			break;

			case 7:
				InsertSlider(HanaLovesYou("AWP minimum damage"), g_Options.rage_mindmg_amount_awp, 0.f, 120.f, "%1.fhp%");
				InsertSlider(HanaLovesYou("AWP hitchance"), g_Options.rage_hitchance_amount_awp, 0.f, 100.f, "%1.f%%");
				InsertCheckbox(HanaLovesYou("AWP hitbox priority"), g_Options.bRage_prioritize_awp);
				InsertCombo(HanaLovesYou("##AWPHIT"), g_Options.iRage_hitbox_awp, rgbthitboxes);
				InsertSlider(HanaLovesYou("AWP hitbox scale"), g_Options.rage_pointscale_amount_awp, 0.1f, 1.0f, "%0.1fx%");
				InsertCheckbox(HanaLovesYou("AWP multi-point"), g_Options.bRage_multipoint_awp);

				HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				HanaLovesMe::BeginChild(HanaLovesYou("#MULTIPOINTSAWP"), vsize(158, 160), true, HanaLovesMeWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					HanaLovesMe::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesAWP[i]);
				}
				HanaLovesMe::EndChild();
			break;

			case 8:
				InsertSlider(HanaLovesYou("Auto-sniper minimum damage"), g_Options.rage_mindmg_amount_auto, 0.f, 120.f, "%1.fhp%");
				InsertSlider(HanaLovesYou("Auto-sniper hitchance"), g_Options.rage_hitchance_amount_auto, 0.f, 100.f, "%1.f%%");
				InsertCheckbox(HanaLovesYou("Auto-sniper hitbox priority"), g_Options.bRage_prioritize_auto);
				InsertCombo(HanaLovesYou("##AUTOSNIPERHIT"), g_Options.iRage_hitbox_auto, rgbthitboxes);
				InsertSlider(HanaLovesYou("Auto-sniper hitbox scale"), g_Options.rage_pointscale_amount_auto, 0.1f, 1.0f, "%0.1fx%");
				InsertCheckbox(HanaLovesYou("Auto-sniper multi-point"), g_Options.bRage_multipoint_auto);

				HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				HanaLovesMe::BeginChild(HanaLovesYou("#MULTIPOINTSAUTOSNIPER"), vsize(158, 160), true, HanaLovesMeWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					HanaLovesMe::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesAuto[i]);
				}
				HanaLovesMe::EndChild();
			break;
			}

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);


		} InsertEndGroupBoxRight(HanaLovesYou("Settings Cover"), HanaLovesYou("Other"));
	}
}

void Menu::Antiaim() {

	HanaLovesMeStyle* style = &HanaLovesMe::GetStyle();
	InsertSpacer(HanaLovesYou("Top Spacer"));

	HanaLovesMe::Columns(2, NULL, false); {

		InsertGroupBoxLeft(HanaLovesYou("Anti-aimbot angles"), 556.f); {

			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);


		HanaLovesMe::PushStyleColor(HanaLovesMeCol_Text, xuifloatcolor(0.95f, 0.76f, 0.06f, 1.00f));
		InsertCheckbox("LBY breaking", g_Options.hvh_antiaim_lby_breaker);
		HanaLovesMe::PopStyleColor();
		
		InsertCombo(HanaLovesYou("Pitch"), g_Options.hvh_antiaim_x, aaxaxis);
		InsertCombo(HanaLovesYou("Yaw"), g_Options.hvh_antiaim_y, aayaxis);
	//	InsertCombo(HanaLovesYou("Yaw fake"), g_Options.hvh_antiaim_y_fake, aafakeaxis);

		InsertSlider(HanaLovesYou("Desync angle left"), g_Options.hvh_antiaim_y_desync_start_left, -180.f, 180.f, "%0.f%");
		InsertSlider(HanaLovesYou("Desync angle right"), g_Options.hvh_antiaim_y_desync_start_right, -180.f, 180.f, "%0.f%");
		InsertSlider(HanaLovesYou("Desync angle back"), g_Options.hvh_antiaim_y_desync_start_back, -180.f, 180.f, "%0.f%");
		InsertSlider(HanaLovesYou("Desync amount"), g_Options.hvh_antiaim_y_desync, -180.f, 180.f, "%0.f%");

		InsertSlider(HanaLovesYou("Custom yaw left"), g_Options.hvh_antiaim_y_custom_left, -180.f, 180.f, "%0.f%");
		InsertSlider(HanaLovesYou("Custom yaw right"), g_Options.hvh_antiaim_y_custom_right, -180.f, 180.f, "%0.f%");
		InsertSlider(HanaLovesYou("Custom Yaw Back"), g_Options.hvh_antiaim_y_custom_back, -180.f, 180.f, "%0.f%");

		InsertCombo(HanaLovesYou("Yaw move"), g_Options.hvh_antiaim_y_move, aayaxismove);
		InsertSlider(HanaLovesYou("Do when velocity is over"), g_Options.hvh_antiaim_y_move_trigger_speed, 0.1f, 130.f, "%0.1f%");
		InsertSlider(HanaLovesYou("Custom yaw move left"), g_Options.hvh_antiaim_y_custom_realmove_left, -180.f, 180.f, "%0.f%");
		InsertSlider(HanaLovesYou("Custom yaw move right"), g_Options.hvh_antiaim_y_custom_realmove_right, -180.f, 180.f, "%0.f%");
		InsertSlider(HanaLovesYou("Custom yaw move back"), g_Options.hvh_antiaim_y_custom_realmove_back, -180.f, 180.f, "%0.f%");

		style->ItemSpacing = vsize(0, 0);
		style->WindowPadding = vsize(6, 6);


		} InsertEndGroupBoxLeft(HanaLovesYou("Anti-aimbot angles Cover"), HanaLovesYou("Anti-aimbot angles"));

	}
	HanaLovesMe::NextColumn(); {

		InsertGroupBoxRight(HanaLovesYou("Custom"), 269.f); {

			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

			InsertCheckbox(HanaLovesYou("Enable fakelag"), g_Options.misc_fakelag_enabled);
			InsertCheckbox(HanaLovesYou("Enable adaptive fakelag"), g_Options.misc_fakelag_adaptive);
			InsertCombo(HanaLovesYou("Activation type"), g_Options.misc_fakelag_activation_type, fakelagact);
			InsertSlider(HanaLovesYou("Fakelag amount"), g_Options.misc_fakelag_value, 0.f, 14.f, "%0.ft"); // ovde
			InsertCheckbox(HanaLovesYou("Infinite duck"), g_Options.misc_infinite_duck);

		//	InsertCheckbox(HanaLovesYou("Fakeduck"), g_Options.misc_fakeduck);
		//	HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
		////	HanaLovesMe::Text("Fakeduck key"); HanaLovesMe::KeyBindButton(&g_Options.misc_fakeduck_keybind);
		///	InsertSlider("Fakeduck choked packets", g_Options.misc_fakeduck_chokedpackets, 0.f, 7.f, "%0.ft");
			InsertCheckbox(HanaLovesYou("Slow-walk"), g_Options.misc_fakewalk);
			HanaLovesMe::KeyBindButton(&g_Options.misc_fakewalk_bind);
			InsertSlider(HanaLovesYou("Slow-walk speed"), g_Options.misc_fakewalk_speed, 0.f, 130.f, "%0.f%");

			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);


		} InsertEndGroupBoxRight(HanaLovesYou("Custom Cover"), HanaLovesYou("Custom"));

		InsertSpacer(HanaLovesYou("Custom - Other Spacer"));

		InsertGroupBoxRight(HanaLovesYou("Manual AA"), 269.f); {


			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Left AA Bind")); HanaLovesMe::KeyBindButton(&g_Options.hvh_aa_left_bind);
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Right AA Bind")); HanaLovesMe::KeyBindButton(&g_Options.hvh_aa_right_bind);
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Back AA Bind")); HanaLovesMe::KeyBindButton(&g_Options.hvh_aa_back_bind);

			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);


		} InsertEndGroupBoxRight(HanaLovesYou("Manual AA Cover"), HanaLovesYou("Manual AA"));
	}
}

void Menu::lgtpew() {

	HanaLovesMeStyle* style = &HanaLovesMe::GetStyle();
	InsertSpacer(HanaLovesYou("Top Spacer"));

	HanaLovesMe::Columns(2, NULL, false); {

		InsertGroupBoxLeft(HanaLovesYou("Aimbot"), 556.f); {

			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

			

				InsertCheckbox(HanaLovesYou("Enable"), g_Options.legit_enabled);
				if (HanaLovesMe::IsItemHovered())
					HanaLovesMe::SetTooltip(HanaLovesYou("Enables your legitness"));
				HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				HanaLovesMe::Text(HanaLovesYou("Aimkey"));
				HanaLovesMe::KeyBindButton(&g_Options.legit_aim_keybind1);
				HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				HanaLovesMe::Text(HanaLovesYou("Aimkey 2"));
				HanaLovesMe::KeyBindButton(&g_Options.legit_aim_keybind2);
				InsertCheckbox(HanaLovesYou("Silent mode"), g_Options.rage_silent);
				if (HanaLovesMe::IsItemHovered())
					HanaLovesMe::SetTooltip(HanaLovesYou("Allows you to aim without your screen locking on [ONLY CLIENTSIDE!]"));
				InsertCheckbox(HanaLovesYou("Use recoil control system"), g_Options.legit_rcs);
				InsertCheckbox(HanaLovesYou("Draw fov"), g_Options.legit_drawfov);
				InsertSlider(HanaLovesYou("Field of view"), g_Options.legit_fov, 0.1f, 25.f, "%0.1f%");
				if (HanaLovesMe::IsItemHovered())
					HanaLovesMe::SetTooltip(HanaLovesYou("Distance between your crosshair and enemy required to lock on."));
				InsertSlider(HanaLovesYou("Smooth factor"), g_Options.legit_smooth_factor, 1.f, 20.f, "%1.f%");
				if (HanaLovesMe::IsItemHovered())
					HanaLovesMe::SetTooltip(HanaLovesYou("How smooth does it flick to the enemy?"));
				InsertCheckbox(HanaLovesYou("Auto pistol"), g_Options.misc_auto_pistol);

			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);

		} InsertEndGroupBoxLeft(HanaLovesYou("Aimbot Cover"), HanaLovesYou("Aimbot"));
	}
	HanaLovesMe::NextColumn(); {

		InsertGroupBoxRight(HanaLovesYou("Other"), 556.f); {

			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

			InsertCheckbox("Accuracy boost", g_Options.legit_backtrack);
			InsertSlider("Accuracy boost ticks", g_Options.legit_backtrackticks, 0, 12, "%1.f%");
			InsertCheckbox(HanaLovesYou("Trigger"), g_Options.legit_trigger);
			InsertCheckbox(HanaLovesYou("Trigger on key only"), g_Options.trigger_on_press);
			HanaLovesMe::KeyBindButton(&g_Options.trigger_keybind);

			InsertCheckbox(HanaLovesYou("Headshot only"), g_Options.legit_hsonly);
			InsertCombo(HanaLovesYou("Pre-aim spot"), g_Options.legit_preaim, lgtpewpreaim);
			InsertCombo(HanaLovesYou("After-aim spot"), g_Options.legit_afteraim, lgtpewafteraim);

			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);

		} InsertEndGroupBoxRight(HanaLovesYou("Other Cover"), HanaLovesYou("Other"));
	}
}

void Menu::Visuals() {

	HanaLovesMeStyle* style = &HanaLovesMe::GetStyle();
	InsertSpacer(HanaLovesYou("Top Spacer"));

	HanaLovesMe::Columns(2, NULL, false); {

		InsertGroupBoxLeft2(HanaLovesYou("Player ESP"), 269.f); {

			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Visible T Color"));
			InsertColorPicker(HanaLovesYou("##colortvb"), g_Options.esp_player_bbox_color_t_visible, true);//
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Visible CT Color"));
			InsertColorPicker(HanaLovesYou("##colorctvb"), g_Options.esp_player_bbox_color_ct_visible, true);//
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Invisible T Color"));
			InsertColorPicker(HanaLovesYou("##colortbw"), g_Options.esp_player_bbox_color_t, true);//
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Invisible CT Color"));
			InsertColorPicker(HanaLovesYou("##colorctbw"), g_Options.esp_player_bbox_color_ct, true);//
			InsertCombo(HanaLovesYou("Box Type"), g_Options.esp_player_boxtype, espType);
			InsertCombo(HanaLovesYou("Box Bound Type"), g_Options.esp_player_boundstype, espBounds);

			InsertSlider(HanaLovesYou("Box Fill Amount"), g_Options.esp_fill_amount, 0.f, 100.f, "%1.f%%");
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Fill Color T Visible"));
			InsertColorPicker(HanaLovesYou("##colortfv"), g_Options.esp_player_fill_color_t_visible, true);//
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Fill Color CT Visible"));
			InsertColorPicker(HanaLovesYou("##colorctfv"), g_Options.esp_player_fill_color_ct_visible, true);//
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Fill Color T Invisible"));
			InsertColorPicker(HanaLovesYou("##colortbfw"), g_Options.esp_player_fill_color_t, true); //
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Fill Color CT Invisible"));
			InsertColorPicker(HanaLovesYou("##colorctbfw"), g_Options.esp_player_fill_color_ct, true);//

			InsertCheckbox(HanaLovesYou("Dormant"), g_Options.esp_farther);
			InsertCheckbox(HanaLovesYou("Enemies only"), g_Options.esp_enemies_only);
			InsertCheckbox(HanaLovesYou("Player flags"), g_Options.esp_flags);
			InsertCheckbox(HanaLovesYou("Player name"), g_Options.esp_player_name);
			InsertCheckbox(HanaLovesYou("Player health"), g_Options.esp_player_health);
			InsertCheckbox(HanaLovesYou("Player weapons"), g_Options.esp_player_weapons);
			InsertCheckbox(HanaLovesYou("Player dlights"), g_Options.visuals_others_dlight);
			
			InsertCheckbox(HanaLovesYou("Lag compensated hitboxes"), g_Options.esp_lagcompensated_hitboxes);
			InsertCombo(HanaLovesYou("Lagcomp hitboxes type"), g_Options.esp_lagcompensated_hitboxes_type, lagcomphitboxez);
			InsertCheckbox(HanaLovesYou("Skeleton"), g_Options.esp_player_skelet);
		//	InsertCheckbox("Backtrack trail", g_Options.esp_backtracked_player_skelet);
			InsertCheckbox(HanaLovesYou("Antiaim lines"), g_Options.esp_player_anglelines);



			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);

		} InsertEndGroupBoxLeft(HanaLovesYou("Player ESP Cover"), HanaLovesYou("Player ESP"));

		InsertSpacer(HanaLovesYou("Player ESP - Colored models Spacer"));

		InsertGroupBoxLeft2(HanaLovesYou("Colored models"), 269.f); {

			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

			InsertCheckbox(HanaLovesYou("Enable Chams"), g_Options.esp_player_chams);
			InsertCombo(HanaLovesYou("Chams Type"), g_Options.esp_player_chams_type, espChams);
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Chams Color T Visible"));
			InsertColorPicker(HanaLovesYou("##colorCHAMST"), g_Options.esp_player_chams_color_t_visible, true);
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Chams Color CT Visible"));
			InsertColorPicker(HanaLovesYou("##colorCHAMSCT"), g_Options.esp_player_chams_color_ct_visible, true);
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Chams Color T Invisible"));
			InsertColorPicker(HanaLovesYou("##colorCHAMSTINVIS"), g_Options.esp_player_chams_color_t, true);
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Chams Color CT Invisible"));
			InsertColorPicker(HanaLovesYou("##colorCHAMSCTINVIS"), g_Options.esp_player_chams_color_ct, true);

			InsertCheckbox(HanaLovesYou("Enable localplayer chams"), g_Options.esp_localplayer_chams);
			InsertCombo(HanaLovesYou("Local chams material"), g_Options.esp_localplayer_chams_type, localChams);
			InsertCheckbox(HanaLovesYou("Localplayer chams xqz"), g_Options.esp_localplayer_chams_xyz);
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Local chams visible color"));
			InsertColorPicker(HanaLovesYou("##colorlocalvisible"), g_Options.esp_localplayer_chams_color_visible, true);
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Local chams invisible color"));
			InsertColorPicker(HanaLovesYou("##colorlocalinvisible"), g_Options.esp_localplayer_chams_color_invisible, true);

			InsertCheckbox(HanaLovesYou("Enable localplayer VM chams"), g_Options.esp_localplayer_viewmodel_chams);
			InsertColorPicker(HanaLovesYou("##colorlocalvc"), g_Options.esp_localplayer_viewmodel_chams_color, true);
			InsertCombo(HanaLovesYou("Localplayer VM material"), g_Options.esp_localplayer_viewmodel_materials, localChams);
			InsertCheckbox(HanaLovesYou("Enable fakeangle chams"), g_Options.fake_chams);

			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("FakeAngle visible color"));
			InsertColorPicker(HanaLovesYou("##colorfakevisible"), g_Options.esp_localplayer_fakechams_color_visible, true);
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("FakeAngle invisible color"));
			InsertColorPicker(HanaLovesYou("##colorfakeinvisible"), g_Options.esp_localplayer_fakechams_color_invisible, true);

			InsertCheckbox(HanaLovesYou("Enable localplayer wireframe"), g_Options.esp_localplayer_viewmodel_chams_wireframe);


			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);

		} InsertEndGroupBoxLeft(HanaLovesYou("Colored models Cover"), HanaLovesYou("Colored models"));
	}
	HanaLovesMe::NextColumn(); {

		InsertGroupBoxRight2(HanaLovesYou("Other ESP"), 269.f); {

			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

			InsertCheckbox(HanaLovesYou("AA Indicators enable"), g_Options.indicators_enabled);
			InsertCheckbox(HanaLovesYou("Draw zeus range"), g_Options.zeusrange_enabled);
			InsertCheckbox(HanaLovesYou("Planted C4"), g_Options.esp_planted_c4);
			InsertCheckbox(HanaLovesYou("Thrown grenades"), g_Options.esp_grenades);
			InsertCombo(HanaLovesYou("Grenade esp type"), g_Options.esp_grenades_type, grenadeesptype);
			InsertCheckbox(HanaLovesYou("Disable post processing"), g_Options.removals_postprocessing);
			InsertSlider(HanaLovesYou("Field of view"), g_Options.visuals_others_player_fov, 0.f, 60.f, "%0.f%");
			InsertSlider(HanaLovesYou("VM field of view"), g_Options.visuals_others_player_fov_viewmodel, 0.f, 60.f, "%0.f%");
			InsertCheckbox(HanaLovesYou("Enable nightmode"), g_Options.visuals_nightmode);
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("NightMode Color"));
			InsertColorPicker(HanaLovesYou("##nightmodecolor"), g_Options.visuals_others_nightmode_color, true);
			InsertCombo(HanaLovesYou("Nightmode type"), g_Options.visuals_nightmode_type, nightmodetype);
			InsertCombo(HanaLovesYou("Sky type"), g_Options.visuals_others_sky, skytype);
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Sky color"));
			InsertColorPicker(HanaLovesYou("##skycolor"), g_Options.visuals_others_skybox_color, true);
			InsertCheckbox(HanaLovesYou("Asus walls"), g_Options.visuals_asuswalls);
			InsertCheckbox(HanaLovesYou("Asus wall props"), g_Options.visuals_asuswalls_staticprops);
			InsertCheckbox(HanaLovesYou("Asus wall world"), g_Options.visuals_asuswalls_world);
			InsertSlider(HanaLovesYou("Asus walls transparency"), g_Options.visuals_asuswalls_value, 0.f, 100.f, "%1.f%%");
			InsertCombo(HanaLovesYou("Dropped weapons type"), g_Options.esp_dropped_weapons, wepBox);
			InsertSlider(HanaLovesYou("DW red"), g_Options.dropped_weapons_color[0], 0.f, 255.f, "%1.f%%");
			InsertSlider(HanaLovesYou("DW green"), g_Options.dropped_weapons_color[1], 0.f, 255.f, "%1.f%%");
			InsertSlider(HanaLovesYou("DW blue"), g_Options.dropped_weapons_color[2], 0.f, 255.f, "%1.f%%");



			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);

		} InsertEndGroupBoxRight(HanaLovesYou("Other ESP Cover"), HanaLovesYou("Other ESP"));

		InsertSpacer(HanaLovesYou("Other ESP - Effects Spacer"));

		InsertGroupBoxRight2(HanaLovesYou("Effects"), 269.f); {

			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

			InsertCheckbox(HanaLovesYou("Remove flashbang effects"), g_Options.removals_flash);
			InsertCheckbox(HanaLovesYou("Remove recoil"), g_Options.removals_novisualrecoil);
			InsertCheckbox(HanaLovesYou("Remove scope overlay"), g_Options.removals_scope);
			InsertCheckbox(HanaLovesYou("Remove scope zoom"), g_Options.removals_zoom);
			InsertCheckbox(HanaLovesYou("Remove smoke"), g_Options.removals_smoke);
			InsertCombo(HanaLovesYou("Smoke removal type"), g_Options.removals_smoke_type, nosmoketype);

			InsertCheckbox(HanaLovesYou("Hitsound"), g_Options.visuals_others_hitmarker);
			InsertCheckbox(HanaLovesYou("Bullet impacts"), g_Options.visuals_others_bulletimpacts);
			InsertColorPicker(HanaLovesYou("##bulletimpacts_color"), g_Options.visuals_others_bulletimpacts_color, true);
			
			InsertCheckbox(HanaLovesYou("Glow Enable"), g_Options.glow_enabled);
			InsertCheckbox(HanaLovesYou("Glow players"), g_Options.glow_players);
			//InsertCheckbox(HanaLovesYou("Glow others"), g_Options.glow_others);
			InsertCombo(HanaLovesYou("Glow type players"), g_Options.glow_players_style, glowstyles);
			//InsertCombo(HanaLovesYou("Glow Type others"), g_Options.glow_others_style, glowstyles);
			//HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			//HanaLovesMe::Text(HanaLovesYou("Glow others color"));
			//InsertColorPicker(HanaLovesYou("##colorglowothers"), g_Options.glow_others_color, true);
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Glow visible T Color"));
			InsertColorPicker(HanaLovesYou("##colorglowtinvisible"), g_Options.glow_player_color_t_visible, true);
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Glow visible CT Color"));
			InsertColorPicker(HanaLovesYou("##colorglowctinvisible"), g_Options.glow_player_color_ct_visible, true);
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Glow invisible T Color"));
			InsertColorPicker(HanaLovesYou("##colorglowtvisible"), g_Options.glow_player_color_t, true);
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Glow invisible CT Color"));
			InsertColorPicker(HanaLovesYou("##colorglowctvisible"), g_Options.glow_player_color_ct, true);
			
			InsertCheckbox(HanaLovesYou("Enable custom crosshair"), g_Options.visuals_draw_xhair);
			InsertColorPicker(HanaLovesYou("##crosshaircolor"), g_Options.xhair_color, true);
			InsertSlider(HanaLovesYou("X Length"), g_Options.visuals_xhair_x, 1.f, 15.f, "%1.f%");
			InsertSlider(HanaLovesYou("Y Length"), g_Options.visuals_xhair_y, 1.f, 15.f, "%1.f%");

			InsertCheckbox(HanaLovesYou("Enable viewmodel changer"), g_Options.change_viewmodel_offset);
			InsertSlider(HanaLovesYou("Viewmodel X"), g_Options.viewmodel_offset_x, -20.f, 20.f, "%1.f%");
			InsertSlider(HanaLovesYou("Viewmodel Y"), g_Options.viewmodel_offset_y, -20.f, 20.f, "%1.f%");
			InsertSlider(HanaLovesYou("Viewmodel Z"), g_Options.viewmodel_offset_z, -20.f, 20.f, "%1.f%");

			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);

		} InsertEndGroupBoxRight(HanaLovesYou("Effects Cover"), HanaLovesYou("Effects"));
	}
}

void Menu::Misc() {

	HanaLovesMeStyle* style = &HanaLovesMe::GetStyle();
	InsertSpacer(HanaLovesYou("Top Spacer"));

	HanaLovesMe::Columns(2, NULL, false); {

		InsertGroupBoxLeft(HanaLovesYou("Miscellaneous"), 556.f); {

			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

			style->Colors[HanaLovesMeCol_PopupBg] = xuicolor(35, 35, 35, 255);
			style->PopupRounding = 0.0f;

			//style->ButtonTextAlign = vsize(0.5f, 0.5f);

			InsertCheckbox(HanaLovesYou("Watermark"), g_Options.watermark);
			if (HanaLovesMe::IsItemHovered())
				HanaLovesMe::SetTooltip(HanaLovesYou("See your ping, hours, etc..."));
			InsertCheckbox(HanaLovesYou("Move crosshair according to recoil"), g_Options.misc_autoaccept);
			if (HanaLovesMe::IsItemHovered())
				HanaLovesMe::SetTooltip(HanaLovesYou("You are a 0 skilled faggot"));
			InsertCheckbox(HanaLovesYou("Event logs"), g_Options.misc_logevents);
			if (HanaLovesMe::IsItemHovered())
				HanaLovesMe::SetTooltip(HanaLovesYou("Shows damage dealt to players top left"));
			InsertCheckbox(HanaLovesYou("Bunny hop"), g_Options.misc_bhop);
			if (HanaLovesMe::IsItemHovered())
				HanaLovesMe::SetTooltip(HanaLovesYou("Spams jump so you dont have to"));
			InsertCheckbox(HanaLovesYou("Grenade prediction"), g_Options.visuals_others_grenade_pred);
			if (HanaLovesMe::IsItemHovered())
				HanaLovesMe::SetTooltip(HanaLovesYou("Lets you see your grenade's path"));
			InsertCheckbox(HanaLovesYou("Auto strafe"), g_Options.misc_autostrafe);
			if (HanaLovesMe::IsItemHovered())
				HanaLovesMe::SetTooltip(HanaLovesYou("Lets u bhop easier"));
			InsertCheckbox(HanaLovesYou("Spectator list"), g_Options.misc_spectatorlist);
			HanaLovesMe::PushStyleColor(HanaLovesMeCol_Text, xuifloatcolor(0.95f, 0.76f, 0.06f, 1.00f));
			InsertCheckbox("Door spam [Disable if defusing]", g_Options.misc_doorspam);
			HanaLovesMe::PopStyleColor();
			InsertCheckbox(HanaLovesYou("Radar show all"), g_Options.mp_radar_showall);
			if (HanaLovesMe::IsItemHovered())
				HanaLovesMe::SetTooltip(HanaLovesYou("Shows all players on radar"));
			InsertCheckbox(HanaLovesYou("Override fog"), g_Options.fog_override);
			if (HanaLovesMe::IsItemHovered())
				HanaLovesMe::SetTooltip(HanaLovesYou("Removes Fog"));
			InsertCheckbox(HanaLovesYou("Chat spam -fat-"), g_Options.misc_chatspamer);
		//	InsertCheckbox(HanaLovesYou("Font debug mode"), g_Options.misc_antikick);
			InsertCheckbox(HanaLovesYou("Enable physics timescale"), g_Options.cl_phys_timescale);
			if (HanaLovesMe::IsItemHovered())
				HanaLovesMe::SetTooltip(HanaLovesYou("Allows you to change speed of ragdolls"));
			InsertSlider(HanaLovesYou("Physics Timescale Amount"), g_Options.cl_phys_timescale_value, 0.1f, 5.0f, "%0.1f%");
			InsertCheckbox(HanaLovesYou("Moon walk"), g_Options.michaeljackson);
			InsertCheckbox(HanaLovesYou("Full bight"), g_Options.fullbright);
			InsertSlider(HanaLovesYou("Aspect ratio changer"), g_Options.aspectratio, 0.f, 3.f, "%0.1f%");
			HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			if (HanaLovesMe::Button(HanaLovesYou("Change ratio")))
			{
				float ratio = (g_Options.aspectratio);
				if (ratio > 0.001)
					g_CVar->FindVar("r_aspectratio")->SetValue(ratio);
				else
					g_CVar->FindVar("r_aspectratio")->SetValue((35 * 0.1f) / 2);
				
			}

			HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Button(HanaLovesYou("Unlock hidden CVars##CUNLOCK"));
			HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::TextColored(xuifloatcolor(1.0f, 0.0f, 0.0f, 1.00f), HanaLovesYou("Bored of cheating?")); // green
			HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			if (HanaLovesMe::Button(HanaLovesYou("Unload##UNLOAD")))
			{
				g_Options.unload = true;
			//	Installer::UnLoadHanaLovesMe(); // here
			}
			
			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);

		} InsertEndGroupBoxLeft(HanaLovesYou("Miscellaneous Cover"), HanaLovesYou("Miscellaneous"));

	}
	HanaLovesMe::NextColumn(); {

		InsertGroupBoxRight(HanaLovesYou("Thirdperson"), 153.f); {

			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);


			InsertCheckbox(HanaLovesYou("Third person"), g_Options.misc_thirdperson);
			HanaLovesMe::KeyBindButton(&g_Options.misc_thirdperson_bind);
			InsertCombo(HanaLovesYou("Third person mode"), g_Options.hvh_show_real_angles, thirdpersonmode);
			if (HanaLovesMe::IsItemHovered())
				HanaLovesMe::SetTooltip(HanaLovesYou("Type of thirdperson"));
			InsertSlider(HanaLovesYou("Thirdperson distance"), g_Options.misc_thirdperson_distance, 20, 200, "%1.f%");
			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);

		} InsertEndGroupBoxRight(HanaLovesYou("Thirdperson Cover"), HanaLovesYou("Thirdperson"));

		InsertSpacer(HanaLovesYou("Settings - Other Spacer"));

		InsertGroupBoxRight(HanaLovesYou("Music"), 385.f); {


			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			std::string zstations[] = {
			"http://air.radiorecord.ru:805/synth_320",
			"https://shoutcast1.hardcoreradio.n1:9000/hcr-mp3",
			"http://air.radiorecord.ru:805/tm_320",
			"http://air.radiorecord.ru:805/mix_320",
			"http://air2.radiorecord.ru:805/rr_320",
			"http://air.radiorecord.ru:805/dub_320",
			"http://air.radiorecord.ru:805/teo_320",
			"http://air.radiorecord.ru:805/dc_320",
			"http://air.radiorecord.ru:805/goa_320",
			"http://air.radiorecord.ru:805/hbass_320",
			"http://air.radiorecord.ru:805/1980_320",
			"http://air.radiorecord.ru:805/rock_320",
			"http://air.radiorecord.ru:805/cadillac_320",
			"http://air.radiorecord.ru:805/eurodance_320",
			"http://antenaradio.org:4550/",
			"https://stream.playradio.rs:8443/play.mp3",
			};



			InsertCheckbox(HanaLovesYou("Radio"), g_Options.misc_radio);
			HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Radio station"));
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			if (HanaLovesMe::ListBox(HanaLovesYou(""), &g_Options.misc_radiostation, radio_name, IM_ARRAYSIZE(radio_name), 17))
			{
				BASS_ChannelStop(stream);
				stream = NULL;
				stream = BASS_StreamCreateURL(zstations[g_Options.misc_radiostation].c_str(), 0, 0, NULL, 0);
			}
			if (g_Options.misc_radio)
			{
				static bool radioInit = false;

				if (!radioInit) {
					BASS_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL);
					stream = BASS_StreamCreateURL(zstations[g_Options.misc_radiostation].c_str(), 0, 0, NULL, 0);
					radioInit = true;
				}
				static float vol = 0.1f;
				BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, vol / 100.0f);
				BASS_ChannelPlay(stream, false);
				InsertSlider(HanaLovesYou("Radio volume"), vol, 0.f, 100.0f, "%0.f%");
			}
			else if (!g_Options.misc_radio)
			{
				BASS_ChannelStop(stream);
				stream = NULL;
			}

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0,0,0,0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30,30,30,255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40,40,40,255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15,15,15,255);

			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);


		} InsertEndGroupBoxRight(HanaLovesYou("Music Cover"), HanaLovesYou("Music"));
	}
}

void Menu::Skins() {

	HanaLovesMeStyle* style = &HanaLovesMe::GetStyle();

	static int selected_id = 0;
	std::vector<EconomyItem_t>& entries = Skinchanger::Get().GetItems();
	// If the user deleted the only config let's add one
	if (entries.size() == 0)
		entries.push_back(EconomyItem_t());
	selected_id = selected_id < int(entries.size()) ? selected_id : entries.size() - 1;
	EconomyItem_t& selected_entry = entries[selected_id];


	InsertSpacer(HanaLovesYou("Top Spacer"));

	HanaLovesMe::Columns(2, NULL, false); {

		InsertGroupBoxLeft(HanaLovesYou("Skin Configs"), 556.f); {

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			if (HanaLovesMe::Checkbox(HanaLovesYou("Enabled##Skinchanger"), &g_Options.skinchanger_enabled))
				Skinchanger::Get().bForceFullUpdate = true;

			HanaLovesMe::Columns(1, nullptr, false);
			// Config selection
			{
				char element_name[64];
				HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
				HanaLovesMe::ListBox(HanaLovesYou("##skinchangerconfigs"), &selected_id, [&element_name, &entries](int idx)
					{
						sprintf_s(element_name, "%s (%s)", entries.at(idx).name, k_weapon_names.at(entries.at(idx).definition_vector_index).name);
						return element_name;
					}, entries.size(), 26);
				HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
				if (HanaLovesMe::Button(HanaLovesYou("Add item")))
				{
					entries.push_back(EconomyItem_t());
					selected_id = entries.size() - 1;
				}
				HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
				if (HanaLovesMe::Button(HanaLovesYou("Remove item")))
					entries.erase(entries.begin() + selected_id);

				HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
				if (HanaLovesMe::Button(HanaLovesYou("Force update##Skinchanger")))
					Skinchanger::Get().bForceFullUpdate = true;

				HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
				if (HanaLovesMe::Button(HanaLovesYou("Save##Skinchanger")))
					Skinchanger::Get().SaveSkins();
			}

			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);

		} InsertEndGroupBoxLeft(HanaLovesYou("Skin Configs Cover"), HanaLovesYou("Skin Configs"));

		HanaLovesMe::NextColumn();

		InsertGroupBoxRight(HanaLovesYou("Skin Selection"), 556.f);
		{

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			// Name
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Config name"));
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
			HanaLovesMe::InputText(HanaLovesYou("##CONFIGNAME"), selected_entry.name, 32);
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Weapon"));
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
			HanaLovesMe::ListBox(HanaLovesYou("##WEAPONSELECT"), &selected_entry.definition_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = k_weapon_names[idx].name;
					return true;
				}, nullptr, k_weapon_names.size(), 9);
			HanaLovesMe::Dummy(vsize(1, 3));
			// Enabled
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Checkbox(HanaLovesYou("Enabled"), &selected_entry.enabled);
			// Pattern Seed & Stattrak
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Seed                     Stattrak"));
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
			HanaLovesMe::InputInt2(HanaLovesYou(""), &selected_entry.seed);
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Quality"));
			// Wear Float
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
			HanaLovesMe::SliderFloat(HanaLovesYou("##KVALITI"), &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5); // was .10
			HanaLovesMe::Dummy(vsize(1, 3));
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Weapon skin"));
			HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
			HanaLovesMe::Text(HanaLovesYou("Paint kit"));
			// Paint kit
			if (selected_entry.definition_index != GLOVE_T_SIDE)
			{
				HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
				HanaLovesMe::ListBox(HanaLovesYou("##PEINTKIT"), &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text) // PAINTKIT LISTBOX
					{
						*out_text = k_skins[idx].name.c_str();
						return true;
					}, nullptr, k_skins.size(), 9);
			}
			else
			{
				HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
				HanaLovesMe::ListBox(HanaLovesYou("##PEINTKIT"), &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text) // PAINTKIT LISTBOX
					{
						*out_text = k_gloves[idx].name.c_str();
						return true;
					}, nullptr, k_gloves.size(), 9);
			}
			HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Quality"));
			HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
			HanaLovesMe::Combo(HanaLovesYou("##QVALITI"), &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = k_quality_names[idx].name;
					return true;
				}, nullptr, k_quality_names.size(), 5);
			// Yes we do it twice to decide knifes
			selected_entry.UpdateValues();
			// Item defindex override
			if (selected_entry.definition_index == WEAPON_KNIFE)
			{
				HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
				HanaLovesMe::Text(HanaLovesYou("Knife"));
				HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
				HanaLovesMe::Combo(HanaLovesYou("##KNEIF"), &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = k_knife_names.at(idx).name;
						return true;
					}, nullptr, k_knife_names.size(), 5);
			}
			else if (selected_entry.definition_index == GLOVE_T_SIDE)
			{
				HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
				HanaLovesMe::Text(HanaLovesYou("Glove"));
				HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
				HanaLovesMe::Combo(HanaLovesYou("##GLAV"), &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = k_glove_names.at(idx).name;
						return true;
					}, nullptr, k_glove_names.size(), 5);
			}
			else
			{
				// We don't want to override weapons other than knives or gloves
				static auto unused_value = 0;
				selected_entry.definition_override_vector_index = 0;
				HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				HanaLovesMe::Text(HanaLovesYou("Unavailable"));
				HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
				HanaLovesMe::Combo(HanaLovesYou("##UNEVEILEBL"), &unused_value, "Only knives or gloves!\0");
			}
			HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
			HanaLovesMe::Text(HanaLovesYou("Nametag"));
			selected_entry.UpdateValues();
			// Custom Name tag
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
			HanaLovesMe::InputText("", selected_entry.custom_name, 32);
			HanaLovesMe::Dummy(vsize(1, 4));

			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);

		} InsertEndGroupBoxRight(HanaLovesYou("Skin Selection Cover"), HanaLovesYou("Skin Selection"));
	}
}

void Menu::Players() {

	HanaLovesMeStyle* style = &HanaLovesMe::GetStyle();
	InsertSpacer(HanaLovesYou("Top Spacer"));

	HanaLovesMe::Columns(2, NULL, false); {

		InsertGroupBoxLeft(HanaLovesYou("Trolling Stuff"), 556.f); {


			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

			HanaLovesMe::NewLine();
			HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Sorry!"));
			HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::Text(HanaLovesYou("Fakes votes have been patched!"));
	
			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);

		} InsertEndGroupBoxLeft(HanaLovesYou("Trolling Stuff Cover"), HanaLovesYou("Trolling Stuff"));
	}
	HanaLovesMe::NextColumn();


	InsertGroupBoxRight(HanaLovesYou("Name Adjustments"), 269.f); {

		style->ItemSpacing = vsize(4, 2);
		style->WindowPadding = vsize(4, 4);
		style->ButtonTextAlign = vsize(0.5f, -2.4f);
		HanaLovesMe::CustomSpacing(9.f);
		HanaLovesMe::PushItemWidth(158.f);

		style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
		style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
		style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
		style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
		style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

		static char nName[127] = "";
		static char ClanChanger[127] = "";
		HanaLovesMe::Columns(1);
		HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
		HanaLovesMe::Text(HanaLovesYou("Custom clan tag"));
		HanaLovesMe::Separator();
		HanaLovesMe::Columns(1, NULL, true);
		{
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::InputText(HanaLovesYou("##CLANINPUT"), ClanChanger, 127);
		}
		HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
		if (HanaLovesMe::Button(HanaLovesYou("Set clan-tag")))
		Utils::SetClantag(ClanChanger);
		std::string allStringedUp = ClanChanger;

		Miscellaneous::Get().gladTag = allStringedUp;
		InsertCheckbox(HanaLovesYou("Animate clantag (If no text, crash!)"), g_Options.misc_animated_clantag);

		HanaLovesMe::Columns(1);
		HanaLovesMe::Separator();
		HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
		HanaLovesMe::Text(HanaLovesYou("Custom nickname"));
		HanaLovesMe::Separator();
		HanaLovesMe::Columns(1, NULL, true);
		{
			HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
			HanaLovesMe::InputText(HanaLovesYou("##NNAMEINPUT"), nName, 127);
		}

		HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
		if (HanaLovesMe::Button(HanaLovesYou("Set nickname##Nichnamechanger")))
			Miscellaneous::Get().ChangeName(nName);

		HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
		if (HanaLovesMe::Button(HanaLovesYou("No name##NNChanger")))
			Miscellaneous::Get().ChangeName("\n\xAD\xAD\xAD");

		style->ItemSpacing = vsize(0, 0);
		style->WindowPadding = vsize(6, 6);


	} InsertEndGroupBoxRight(HanaLovesYou("Name Adjustments Cover"), HanaLovesYou("Name Adjustments"));

	InsertSpacer(HanaLovesYou("Name Adjustments - Other Spacer"));

	InsertGroupBoxRight(HanaLovesYou("Credits"), 269.f); {


		style->ItemSpacing = vsize(4, 2);
		style->WindowPadding = vsize(4, 4);
		style->ButtonTextAlign = vsize(0.5f, -2.4f);
		HanaLovesMe::PushItemWidth(158.f);

		HanaLovesMe::CustomSpacing(9.f);
		HanaLovesMe::NewLine(); HanaLovesMe::SameLine(25.f);
		HanaLovesMe::Image(globals::byvsonyp0wer, vsize(210.f, 206.f));
		HanaLovesMe::NewLine();
		HanaLovesMe::NewLine(); HanaLovesMe::SameLine(20.f);
		HanaLovesMe::Text(HanaLovesYou("Client build version: 11.7 /// RV Beta test"));
		HanaLovesMe::NewLine(); HanaLovesMe::SameLine(20.f);
		HanaLovesMe::Text(HanaLovesYou("Info: nanosense's real (old) name is RXHook."));


		style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
		style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
		style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
		style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
		style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);


		style->ItemSpacing = vsize(0, 0);
		style->WindowPadding = vsize(6, 6);


	} 	InsertEndGroupBoxRight(HanaLovesYou("Credits Cover"), HanaLovesYou("Credits"));
}
void Menu::ConfigTab()
{
	HanaLovesMeStyle* style = &HanaLovesMe::GetStyle();
	InsertSpacer(HanaLovesYou("Top Spacer"));

	HanaLovesMe::Columns(2, NULL, false); {

		InsertGroupBoxLeft(HanaLovesYou("Matchmaking"), 556.f); {

			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);


			static std::vector<std::string> configItems = Config::Get().GetAllConfigs();
			static int configItemCurrent = -1;

			static char fName[128] = "unnamed config";

			HanaLovesMe::Columns(1, NULL, true);
			{

				{
					HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
					if (HanaLovesMe::ListBox("", &configItemCurrent, configItems, 8))
					{
						std::string fPath = std::string(Global::my_documents_folder) + "\\nanosense\\" + configItems[configItemCurrent] + ".0x000c7B";
						//Config::Get().LoadConfig(fPath);
					}
				}
				{
					HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
					HanaLovesMe::InputText("", fName, 128);
				}

				HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				if (HanaLovesMe::Button(HanaLovesYou("Add##Config")))
				{
					std::string fPath = std::string(Global::my_documents_folder) + "\\nanosense\\" + fName + ".0x000c7B";
					Config::Get().SaveConfig(fPath);

					configItems = Config::Get().GetAllConfigs();
					configItemCurrent = -1;
				}

				HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				if (HanaLovesMe::Button(HanaLovesYou("Load##Config")))
				{
					std::string fPath = std::string(Global::my_documents_folder) + "\\nanosense\\" + configItems[configItemCurrent] + ".0x000c7B";
					Config::Get().LoadConfig(fPath);
				}

				HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				if (HanaLovesMe::Button(HanaLovesYou("Save##Config")))
				{
					if (configItems.size() > 0 && (configItemCurrent >= 0 && configItemCurrent < (int)configItems.size()))
					{
						std::string fPath = std::string(Global::my_documents_folder) + "\\nanosense\\" + configItems[configItemCurrent] + ".0x000c7B";
						Config::Get().SaveConfig(fPath);
						g_CVar->ConsoleColorPrintf(Color(255, 0, 0), "[nanosense] Saved Config.\n");
					}
				}

				HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				if (HanaLovesMe::Button(HanaLovesYou("Delete##Config")))
				{
					if (configItems.size() > 0 && (configItemCurrent >= 0 && configItemCurrent < (int)configItems.size()))
					{
						std::string fPath = std::string(Global::my_documents_folder) + "\\nanosense\\" + configItems[configItemCurrent] + ".0x000c7B";
						std::remove(fPath.c_str());
						g_CVar->ConsoleColorPrintf(Color(255, 0, 0), "[nanosense] Removed Config.\n");

						configItems = Config::Get().GetAllConfigs();
						configItemCurrent = -1;
					}
				}
				HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				if (HanaLovesMe::Button(HanaLovesYou("Refresh##Config")))
					configItems = Config::Get().GetAllConfigs();

				HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
				if (HanaLovesMe::Button(HanaLovesYou("Load skinchanger##Skinchanger")))
					Skinchanger::Get().LoadSkins();
			}

			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);


		} InsertEndGroupBoxLeft(HanaLovesYou("Matchmaking Cover"), HanaLovesYou("Matchmaking"));
	}
	HanaLovesMe::NextColumn();

	InsertGroupBoxRight(HanaLovesYou("Config"), 556.f); {


		style->ItemSpacing = vsize(4, 2);
		style->WindowPadding = vsize(4, 4);
		style->ButtonTextAlign = vsize(0.5f, -2.4f);
		HanaLovesMe::CustomSpacing(9.f);
		HanaLovesMe::PushItemWidth(158.f);

		HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
		HanaLovesMe::Text(HanaLovesYou("Matchmaking region changer"));
		HanaLovesMe::Spacing(); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f); HanaLovesMe::PushItemWidth(158.f);
		HanaLovesMe::ListBox(HanaLovesYou(""), &g_Options.misc_region_changer, mmregions, 48, 28);
		HanaLovesMe::CustomSpacing(6.f); HanaLovesMe::NewLine(); HanaLovesMe::SameLine(42.f);
		if (HanaLovesMe::Button(HanaLovesYou("Apply region")))
		{
			Miscellaneous::Get().ChangeRegion();
		}

		style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
		style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
		style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
		style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
		style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

		
		style->ItemSpacing = vsize(0, 0);
		style->WindowPadding = vsize(6, 6);

	} InsertEndGroupBoxRight(HanaLovesYou("Config Cover"), HanaLovesYou("Config"));
}
void Menu::LuaTab()
{
	HanaLovesMeStyle* style = &HanaLovesMe::GetStyle();
	InsertSpacer(HanaLovesYou("Top Spacer"));

	HanaLovesMe::Columns(2, NULL, false); {

		InsertGroupBoxLeft(HanaLovesYou("A"), 556.f); {


			style->ItemSpacing = vsize(4, 2);
			style->WindowPadding = vsize(4, 4);
			style->ButtonTextAlign = vsize(0.5f, -2.4f);
			HanaLovesMe::CustomSpacing(9.f);
			HanaLovesMe::PushItemWidth(158.f);

			style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
			style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
			style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
			style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);

			style->ItemSpacing = vsize(0, 0);
			style->WindowPadding = vsize(6, 6);

		} InsertEndGroupBoxLeft(HanaLovesYou("A Cover"), HanaLovesYou("A"));
	}
	HanaLovesMe::NextColumn();


	InsertGroupBoxRight(HanaLovesYou("B"), 556.f); {

		style->ItemSpacing = vsize(4, 2);
		style->WindowPadding = vsize(4, 4);
		HanaLovesMe::CustomSpacing(9.f);
		HanaLovesMe::PushItemWidth(158.f);

		style->Colors[HanaLovesMeCol_Separator] = xuicolor(0, 0, 0, 0);
		style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(30, 30, 30, 255);
		style->Colors[HanaLovesMeCol_FrameBg] = xuicolor(35, 35, 35, 255);
		style->Colors[HanaLovesMeCol_ButtonHovered] = xuicolor(40, 40, 40, 255);
		style->Colors[HanaLovesMeCol_ButtonActive] = xuicolor(15, 15, 15, 255);




		style->ItemSpacing = vsize(0, 0);
		style->WindowPadding = vsize(6, 6);


	} InsertEndGroupBoxRight(HanaLovesYou("B Cover"), HanaLovesYou("B"));
}

// Junk Code By Troll Face & Thaisen's Gen
void kQQpDgJhvkgGkWXHmmwEJPKaoLMlzWhXYuBlYwIWChveg76197027() {     int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT87395992 = -519066403;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT46064971 = -259298432;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT71321488 = -105584178;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT59368519 = -769600396;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT58679475 = -181737161;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT86692997 = -709019929;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT46755185 = -403723676;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT51698856 = -440570374;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT49593280 = -849134865;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT29359049 = -99841779;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT29874632 = -981758562;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT42689592 = -748712988;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT72965947 = -783043504;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT50766204 = -619797057;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT47519626 = -68346756;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT3057518 = -733967734;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT23697178 = 15063300;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT4602237 = -221788345;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT27645740 = -880029585;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT53283309 = -800084162;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT79784136 = -790358547;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT19663816 = -274290120;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT58612781 = -142147389;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT81861913 = -37524814;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT31033101 = -687083398;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT34295030 = -431222027;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT89542330 = 31987506;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT34839661 = -987956202;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT93996194 = -558909555;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT54864507 = -181960545;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT53024081 = -922129070;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT23485329 = -920660477;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT49149241 = -483681786;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT45696118 = -296001057;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT15500318 = -449446942;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT53262426 = -204125924;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT70405002 = 8209247;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT79107775 = 85607858;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT63945895 = -861745214;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT42348638 = -368933991;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT45415805 = -956507513;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT65203811 = -598788756;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT53589952 = -847631164;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT94004865 = -258042717;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT12125656 = -492791882;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT37261587 = -527374416;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT94606335 = -326556801;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT16576415 = -821707021;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT15202619 = -396477290;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT7040047 = -768854589;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT71042136 = -546560209;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT94863279 = -595699010;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT36154341 = -472497437;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT21582517 = 94453025;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT96412890 = -46541565;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT67732176 = -144776283;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT87452190 = -17151043;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT89459575 = 31940636;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT28335418 = 17483001;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT24384445 = -750515134;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT97150666 = -641007436;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT11915524 = -415767475;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT57702662 = -881660820;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT94728772 = -567174321;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT76334968 = -177712709;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT6389303 = 38901915;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT93540350 = -165031203;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT27269829 = -387042448;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT35265886 = -70350115;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT94257200 = -864220833;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT32652516 = -642176981;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT44589403 = 29455441;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT40656341 = -360043132;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT85297102 = -411095594;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT7867505 = -843576649;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT14580325 = -91569791;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT66073864 = -426658956;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT64607916 = -884104673;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT69736257 = -544732933;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT93771514 = -59708982;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT39688695 = -4665227;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT72965915 = -146305474;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT19637042 = -491478912;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT86956147 = -790054966;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT83822371 = -635400337;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT58160801 = -226430060;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT87330988 = -348163041;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT27566725 = -478134812;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT49283228 = -149459493;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT47768142 = -204670659;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT65810236 = -86974881;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT80945427 = 76268610;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT50772357 = -931875143;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT39561451 = -11230080;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT45197971 = -727926556;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT33500281 = -440740039;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT7501150 = -717127937;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT58861180 = -180456844;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT17669897 = 19669992;    int aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT5736354 = -519066403;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT87395992 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT46064971;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT46064971 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT71321488;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT71321488 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT59368519;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT59368519 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT58679475;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT58679475 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT86692997;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT86692997 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT46755185;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT46755185 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT51698856;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT51698856 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT49593280;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT49593280 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT29359049;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT29359049 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT29874632;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT29874632 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT42689592;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT42689592 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT72965947;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT72965947 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT50766204;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT50766204 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT47519626;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT47519626 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT3057518;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT3057518 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT23697178;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT23697178 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT4602237;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT4602237 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT27645740;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT27645740 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT53283309;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT53283309 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT79784136;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT79784136 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT19663816;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT19663816 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT58612781;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT58612781 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT81861913;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT81861913 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT31033101;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT31033101 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT34295030;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT34295030 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT89542330;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT89542330 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT34839661;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT34839661 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT93996194;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT93996194 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT54864507;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT54864507 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT53024081;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT53024081 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT23485329;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT23485329 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT49149241;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT49149241 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT45696118;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT45696118 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT15500318;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT15500318 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT53262426;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT53262426 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT70405002;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT70405002 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT79107775;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT79107775 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT63945895;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT63945895 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT42348638;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT42348638 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT45415805;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT45415805 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT65203811;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT65203811 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT53589952;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT53589952 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT94004865;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT94004865 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT12125656;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT12125656 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT37261587;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT37261587 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT94606335;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT94606335 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT16576415;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT16576415 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT15202619;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT15202619 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT7040047;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT7040047 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT71042136;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT71042136 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT94863279;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT94863279 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT36154341;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT36154341 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT21582517;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT21582517 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT96412890;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT96412890 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT67732176;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT67732176 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT87452190;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT87452190 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT89459575;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT89459575 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT28335418;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT28335418 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT24384445;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT24384445 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT97150666;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT97150666 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT11915524;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT11915524 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT57702662;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT57702662 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT94728772;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT94728772 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT76334968;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT76334968 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT6389303;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT6389303 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT93540350;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT93540350 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT27269829;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT27269829 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT35265886;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT35265886 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT94257200;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT94257200 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT32652516;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT32652516 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT44589403;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT44589403 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT40656341;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT40656341 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT85297102;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT85297102 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT7867505;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT7867505 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT14580325;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT14580325 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT66073864;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT66073864 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT64607916;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT64607916 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT69736257;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT69736257 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT93771514;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT93771514 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT39688695;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT39688695 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT72965915;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT72965915 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT19637042;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT19637042 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT86956147;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT86956147 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT83822371;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT83822371 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT58160801;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT58160801 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT87330988;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT87330988 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT27566725;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT27566725 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT49283228;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT49283228 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT47768142;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT47768142 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT65810236;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT65810236 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT80945427;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT80945427 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT50772357;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT50772357 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT39561451;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT39561451 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT45197971;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT45197971 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT33500281;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT33500281 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT7501150;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT7501150 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT58861180;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT58861180 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT17669897;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT17669897 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT5736354;     aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT5736354 = aqHRnIOpzFVGpBrUYhYCJhqUmjpeZBcUfuNUFULOT87395992;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void wmTYbDFnRjKjExogxoULIuRVbQoenGjPgTJMeZZHMQSrbzdwqUOIOEXCUxwLPaALzuDkocBYONzrbZtbmroT33244682() {     float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr89740336 = 67063263;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr62367386 = -159911982;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr52181585 = -986866015;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr49716307 = -338621551;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr31627443 = -135248658;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr37327388 = -379227537;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr83764894 = -130540270;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr77132327 = 88790753;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr9540369 = -227208010;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr7672822 = 30315563;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr77048544 = -43734074;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr76814377 = -455809443;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr63362601 = -379207729;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr40082961 = -345317706;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr50009124 = -741174180;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr46659416 = -662335817;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr94963263 = 71591409;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr27565587 = -438361981;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr13290490 = -402933881;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr61892294 = -391829439;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr43986486 = -581924490;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr36528110 = -48225896;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr55228775 = -639036889;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr66893029 = -632460472;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr86424312 = -393721053;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr93964538 = -934921226;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr33983176 = -280865176;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr72733099 = -502121644;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr19165820 = -631049784;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr80109535 = 20116895;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr24504280 = -943170576;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr79883130 = -719932929;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr91171237 = -523994223;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr59902985 = -32240077;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr79941251 = -925533622;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr51015733 = -584621896;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr13015860 = -214011843;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr46439682 = -411620106;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr9983412 = -688757781;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr91546886 = -794099425;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr40093095 = -462776323;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr48393341 = -338046711;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr70249150 = -682491561;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr97562032 = -153222890;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr38040134 = -562246650;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr82648890 = 38457788;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr79831255 = -215791932;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr61335708 = -740057728;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr59118704 = -463300242;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr12257315 = -367434763;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr4957442 = -651917114;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr96623969 = -566942850;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr20724581 = -48064631;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr92053016 = -349842175;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr89258196 = -968453590;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr53212226 = -884710841;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr7138612 = -520875094;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr85288556 = -254405544;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr63291994 = -944900499;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr37662904 = -200327433;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr3344212 = 1637639;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr11031796 = -628418627;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr57966507 = -280159464;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr29430833 = -147324905;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr83168542 = -26513861;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr97165413 = -323801145;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr85643139 = -931815220;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr3459617 = -246967652;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr60141710 = -419784085;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr98993390 = -56552284;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr33643556 = -348323975;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr48523581 = -516788486;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr17582176 = -749604200;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr21743604 = -608834456;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr21799199 = -929053116;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr95593144 = -143877779;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr66278960 = -365734336;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr57666742 = -385813999;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr28852895 = 29786177;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr3775422 = -332178841;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr14133284 = -619129294;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr72647467 = -540807448;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr13614395 = 61178597;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr6908506 = -163615021;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr75152094 = -327965991;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr27880310 = -276227727;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr59158549 = -571868298;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr99118220 = -74152049;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr70644788 = -63786487;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr26729025 = 59177218;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr43877122 = 36253198;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr27727304 = -959606299;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr83147687 = -466719607;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr72320507 = -388430349;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr88202674 = -695737064;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr29061300 = -834357697;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr90426833 = 42112753;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr40818317 = -435166656;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr14393490 = -26709030;    float rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr40874720 = 67063263;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr89740336 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr62367386;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr62367386 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr52181585;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr52181585 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr49716307;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr49716307 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr31627443;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr31627443 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr37327388;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr37327388 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr83764894;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr83764894 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr77132327;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr77132327 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr9540369;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr9540369 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr7672822;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr7672822 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr77048544;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr77048544 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr76814377;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr76814377 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr63362601;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr63362601 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr40082961;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr40082961 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr50009124;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr50009124 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr46659416;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr46659416 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr94963263;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr94963263 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr27565587;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr27565587 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr13290490;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr13290490 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr61892294;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr61892294 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr43986486;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr43986486 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr36528110;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr36528110 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr55228775;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr55228775 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr66893029;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr66893029 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr86424312;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr86424312 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr93964538;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr93964538 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr33983176;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr33983176 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr72733099;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr72733099 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr19165820;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr19165820 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr80109535;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr80109535 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr24504280;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr24504280 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr79883130;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr79883130 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr91171237;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr91171237 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr59902985;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr59902985 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr79941251;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr79941251 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr51015733;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr51015733 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr13015860;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr13015860 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr46439682;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr46439682 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr9983412;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr9983412 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr91546886;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr91546886 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr40093095;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr40093095 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr48393341;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr48393341 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr70249150;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr70249150 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr97562032;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr97562032 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr38040134;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr38040134 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr82648890;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr82648890 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr79831255;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr79831255 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr61335708;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr61335708 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr59118704;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr59118704 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr12257315;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr12257315 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr4957442;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr4957442 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr96623969;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr96623969 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr20724581;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr20724581 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr92053016;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr92053016 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr89258196;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr89258196 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr53212226;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr53212226 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr7138612;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr7138612 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr85288556;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr85288556 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr63291994;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr63291994 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr37662904;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr37662904 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr3344212;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr3344212 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr11031796;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr11031796 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr57966507;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr57966507 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr29430833;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr29430833 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr83168542;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr83168542 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr97165413;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr97165413 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr85643139;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr85643139 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr3459617;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr3459617 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr60141710;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr60141710 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr98993390;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr98993390 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr33643556;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr33643556 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr48523581;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr48523581 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr17582176;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr17582176 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr21743604;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr21743604 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr21799199;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr21799199 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr95593144;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr95593144 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr66278960;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr66278960 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr57666742;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr57666742 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr28852895;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr28852895 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr3775422;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr3775422 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr14133284;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr14133284 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr72647467;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr72647467 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr13614395;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr13614395 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr6908506;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr6908506 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr75152094;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr75152094 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr27880310;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr27880310 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr59158549;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr59158549 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr99118220;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr99118220 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr70644788;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr70644788 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr26729025;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr26729025 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr43877122;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr43877122 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr27727304;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr27727304 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr83147687;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr83147687 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr72320507;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr72320507 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr88202674;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr88202674 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr29061300;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr29061300 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr90426833;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr90426833 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr40818317;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr40818317 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr14393490;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr14393490 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr40874720;     rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr40874720 = rbBbwGrhcNrlQHsgwtIOSqtAFfbuDUZYmhTYtboikUwBVNaThNftiaABiAWQewdHoTzjnTnUFaexurfsFHNqofpMgSqsFXDnr89740336;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void LgsZzfkjTzEcrURQAFITBhQcjimBofVhqsbThkxSqEmMuzgyDwO16357650() {     long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN11835299 = -943032594;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN43133513 = -389375618;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN91399677 = 34005376;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN527016 = -175028653;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN74041888 = 28064407;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN8986253 = -414452522;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN47174060 = -179997686;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN62201647 = -256863954;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN80494000 = -282672842;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN79155497 = -331374609;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN61758516 = -309133232;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN34294309 = -235990427;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN61140534 = -990070995;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN75639901 = 98059940;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN33763824 = -462403255;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN54619555 = -473546334;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN50378608 = -539382715;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN46317246 = -120701214;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN16892322 = -884026173;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN99815901 = -581805870;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN92509318 = -9407051;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN68081107 = -550972460;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN66928733 = -643527626;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN14256896 = -390887117;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN83823804 = -249989868;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN57765078 = -632769998;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN49378561 = -924260014;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN46584024 = -692023464;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN15578228 = -505756790;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN21731470 = -191285196;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN72351333 = -117332378;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN45185175 = -421645667;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN72528940 = -412884239;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN26291143 = -397235915;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN32010097 = -871292025;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN20892932 = -869176212;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN99188895 = -804774434;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN30098183 = -79098236;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN17298095 = -194505574;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN65784406 = 44866531;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN13443206 = 36649367;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN29445114 = -351422239;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN87132985 = -386935644;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN60179369 = -541754464;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN38016751 = -720051807;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN66944224 = -172439900;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN90360698 = -767421751;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN37630293 = -850715253;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN87868286 = -426362619;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN48891060 = -358509810;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN73512014 = -219232426;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN3564817 = -968260158;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN18665738 = -803898825;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN51584257 = -831066886;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN23848331 = -654715444;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN43754191 = -292060135;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN76204780 = -745847993;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN77142781 = -575107507;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN16703211 = -925038785;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN16276811 = -339165595;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN59607692 = -490192509;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN590037 = -487974223;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN46623420 = -751107164;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN58762531 = 8612354;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN6804165 = -114042231;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN16573342 = -887487566;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN61765369 = -823106189;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN34849391 = -492835080;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN43629804 = -30648035;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN12870892 = -593227044;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN55430660 = -668771901;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN20280425 = -360284480;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN29019151 = -926195640;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN51107915 = -828892704;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN86372696 = -518455238;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN63064205 = -657984812;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN80948121 = -64036817;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN6749365 = -1773162;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN76240144 = -670835311;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN16879581 = 22450031;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN67404379 = -865348247;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN11748268 = 26455239;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN58715737 = -165660846;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN66687167 = -47246981;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN48219455 = -972052770;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN68786516 = -149072220;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN26519437 = -617746842;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN20944684 = -581817353;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN2442812 = -742520471;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN88255905 = -479231891;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN44688152 = -23328219;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN22046114 = -129666927;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN13394972 = -154059451;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN1021285 = -855339979;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN6176715 = -464940961;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN12853169 = -475376411;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN82821693 = -600315075;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN28370455 = -295547998;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN53375204 = -327712233;    long JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN21443410 = -943032594;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN11835299 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN43133513;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN43133513 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN91399677;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN91399677 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN527016;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN527016 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN74041888;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN74041888 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN8986253;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN8986253 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN47174060;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN47174060 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN62201647;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN62201647 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN80494000;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN80494000 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN79155497;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN79155497 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN61758516;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN61758516 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN34294309;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN34294309 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN61140534;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN61140534 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN75639901;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN75639901 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN33763824;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN33763824 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN54619555;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN54619555 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN50378608;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN50378608 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN46317246;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN46317246 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN16892322;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN16892322 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN99815901;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN99815901 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN92509318;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN92509318 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN68081107;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN68081107 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN66928733;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN66928733 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN14256896;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN14256896 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN83823804;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN83823804 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN57765078;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN57765078 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN49378561;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN49378561 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN46584024;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN46584024 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN15578228;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN15578228 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN21731470;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN21731470 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN72351333;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN72351333 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN45185175;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN45185175 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN72528940;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN72528940 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN26291143;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN26291143 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN32010097;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN32010097 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN20892932;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN20892932 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN99188895;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN99188895 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN30098183;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN30098183 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN17298095;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN17298095 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN65784406;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN65784406 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN13443206;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN13443206 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN29445114;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN29445114 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN87132985;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN87132985 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN60179369;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN60179369 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN38016751;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN38016751 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN66944224;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN66944224 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN90360698;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN90360698 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN37630293;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN37630293 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN87868286;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN87868286 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN48891060;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN48891060 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN73512014;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN73512014 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN3564817;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN3564817 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN18665738;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN18665738 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN51584257;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN51584257 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN23848331;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN23848331 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN43754191;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN43754191 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN76204780;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN76204780 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN77142781;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN77142781 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN16703211;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN16703211 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN16276811;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN16276811 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN59607692;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN59607692 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN590037;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN590037 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN46623420;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN46623420 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN58762531;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN58762531 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN6804165;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN6804165 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN16573342;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN16573342 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN61765369;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN61765369 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN34849391;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN34849391 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN43629804;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN43629804 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN12870892;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN12870892 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN55430660;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN55430660 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN20280425;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN20280425 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN29019151;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN29019151 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN51107915;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN51107915 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN86372696;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN86372696 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN63064205;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN63064205 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN80948121;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN80948121 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN6749365;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN6749365 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN76240144;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN76240144 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN16879581;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN16879581 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN67404379;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN67404379 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN11748268;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN11748268 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN58715737;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN58715737 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN66687167;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN66687167 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN48219455;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN48219455 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN68786516;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN68786516 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN26519437;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN26519437 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN20944684;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN20944684 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN2442812;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN2442812 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN88255905;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN88255905 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN44688152;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN44688152 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN22046114;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN22046114 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN13394972;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN13394972 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN1021285;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN1021285 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN6176715;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN6176715 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN12853169;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN12853169 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN82821693;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN82821693 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN28370455;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN28370455 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN53375204;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN53375204 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN21443410;     JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN21443410 = JhEydPPqQsiuoeWmztxiPIpdwvDhjCoxrQLAmtxESugZHKxhYN11835299;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void dpOMAGMshicaxKSnppkCZDVIHqUynDvqxjEsPOouynMLAdexhdEhSLqaUFFKmNFdnmdNGaYbAqDmrKHeLWRJyNXltd97835287() {     double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr62437720 = -113172235;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr82334261 = -255684532;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr24772627 = 92954309;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr879263 = -848321499;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr79106026 = -686151749;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr69542720 = -725065715;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr78954262 = -888139151;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr38057691 = -407900574;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr71883176 = -683543345;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr85311660 = -396378155;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr33960336 = -468858344;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr72354393 = -843583735;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr96209308 = -998713432;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr60033160 = -345933213;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr27410173 = -520215803;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr61058728 = 76973888;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr21526928 = 65625513;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr6598208 = -271769973;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr63391491 = -496998552;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr60769076 = 24692551;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr90633739 = -528631194;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr60822213 = -661419112;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr99073608 = -891137520;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr50287870 = -995151346;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr96241847 = -437666928;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr76303249 = -699065350;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr25212353 = 24496309;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr21751763 = -431825202;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr78676227 = -74068447;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr53081284 = -302738334;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr40025402 = -192584652;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr73277103 = -288965692;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr1910050 = -72759009;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr46757134 = -349559501;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr80603940 = -289882990;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr79774818 = 6730880;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr4092895 = -482294157;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr66514157 = -488076511;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr28874366 = -288882175;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr61773131 = -471114923;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr27292076 = -538404553;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr38306904 = -295952675;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr7492312 = -880149765;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr30032616 = -68510404;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr75920074 = -662268894;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr51065050 = -94760445;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr17117218 = -675597051;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr78788702 = -973422623;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr61424309 = -92996727;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr85497493 = -785103485;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr80006796 = -194640183;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr87893010 = -860050190;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr55445580 = -527406836;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr62245786 = 7517375;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr7201549 = -203763552;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr1615508 = -451753124;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr83260653 = -364547013;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr74484756 = 88105655;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr4637415 = -310654572;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr2802777 = -987086400;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr44330367 = -649562024;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr57202500 = -356313949;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr59381464 = -233832128;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr18801892 = -280805011;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr45286259 = -103793504;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr60683232 = -79892653;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr70444343 = -670824727;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr49452175 = -549153931;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr79429220 = 43949776;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr47635354 = -426946683;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr56965834 = -440731956;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr55012770 = -446297976;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr77723842 = -982887799;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr1618360 = 74116370;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr33477000 = -436902896;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr52326835 = -132678520;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr53329902 = -781269347;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr69040992 = -722627117;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr74367796 = -232882453;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr45176798 = -242906483;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr59186032 = 76531701;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr46423650 = -2081069;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr60327454 = -238828476;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr93178734 = -288964963;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr73074487 = -8098151;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr52132391 = -332534463;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr17831523 = -761558856;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr39664263 = 19723616;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr39555586 = -45795950;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr78988433 = -838129867;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr96514165 = -628722108;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr29608138 = -470399813;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr61876742 = -77421940;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr26071589 = -301795776;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr17442764 = -821552899;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr70089575 = -82090605;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr78925440 = 37879453;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr88690419 = -499344754;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr84746357 = -964716901;    double YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr15236842 = -113172235;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr62437720 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr82334261;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr82334261 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr24772627;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr24772627 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr879263;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr879263 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr79106026;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr79106026 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr69542720;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr69542720 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr78954262;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr78954262 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr38057691;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr38057691 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr71883176;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr71883176 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr85311660;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr85311660 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr33960336;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr33960336 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr72354393;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr72354393 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr96209308;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr96209308 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr60033160;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr60033160 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr27410173;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr27410173 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr61058728;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr61058728 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr21526928;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr21526928 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr6598208;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr6598208 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr63391491;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr63391491 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr60769076;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr60769076 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr90633739;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr90633739 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr60822213;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr60822213 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr99073608;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr99073608 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr50287870;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr50287870 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr96241847;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr96241847 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr76303249;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr76303249 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr25212353;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr25212353 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr21751763;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr21751763 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr78676227;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr78676227 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr53081284;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr53081284 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr40025402;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr40025402 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr73277103;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr73277103 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr1910050;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr1910050 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr46757134;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr46757134 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr80603940;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr80603940 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr79774818;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr79774818 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr4092895;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr4092895 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr66514157;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr66514157 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr28874366;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr28874366 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr61773131;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr61773131 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr27292076;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr27292076 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr38306904;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr38306904 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr7492312;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr7492312 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr30032616;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr30032616 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr75920074;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr75920074 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr51065050;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr51065050 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr17117218;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr17117218 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr78788702;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr78788702 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr61424309;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr61424309 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr85497493;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr85497493 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr80006796;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr80006796 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr87893010;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr87893010 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr55445580;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr55445580 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr62245786;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr62245786 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr7201549;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr7201549 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr1615508;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr1615508 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr83260653;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr83260653 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr74484756;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr74484756 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr4637415;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr4637415 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr2802777;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr2802777 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr44330367;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr44330367 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr57202500;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr57202500 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr59381464;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr59381464 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr18801892;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr18801892 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr45286259;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr45286259 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr60683232;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr60683232 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr70444343;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr70444343 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr49452175;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr49452175 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr79429220;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr79429220 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr47635354;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr47635354 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr56965834;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr56965834 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr55012770;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr55012770 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr77723842;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr77723842 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr1618360;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr1618360 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr33477000;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr33477000 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr52326835;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr52326835 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr53329902;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr53329902 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr69040992;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr69040992 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr74367796;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr74367796 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr45176798;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr45176798 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr59186032;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr59186032 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr46423650;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr46423650 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr60327454;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr60327454 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr93178734;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr93178734 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr73074487;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr73074487 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr52132391;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr52132391 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr17831523;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr17831523 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr39664263;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr39664263 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr39555586;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr39555586 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr78988433;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr78988433 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr96514165;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr96514165 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr29608138;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr29608138 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr61876742;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr61876742 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr26071589;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr26071589 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr17442764;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr17442764 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr70089575;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr70089575 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr78925440;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr78925440 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr88690419;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr88690419 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr84746357;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr84746357 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr15236842;     YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr15236842 = YKJeHkhiYOzRrHFdgcIDiJxayQwvfZOsTcCNRWiSFJmteueKDUgvdqvOMifrnlDyzHxPEr62437720;}
// Junk Finished
