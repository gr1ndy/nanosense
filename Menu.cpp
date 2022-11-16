#include "Menu.hpp"

#include "Options.hpp"

#include "Structs.hpp"

#include "features/Miscellaneous.hpp"
#include "features/KitParser.hpp"
#include "features/Skinchanger.hpp"

#include "imgui/imgui_internal.h"

#include <functional>
#include <filesystem> // hack
#include "features/AntiAim.hpp"
#include "Install.hpp"

namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values, int height_in_items = -1)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size(), height_in_items);
	}

	static bool ListBox(const char* label, int* current_item, std::function<const char*(int)> lambda, int items_count, int height_in_items)
	{
		return ImGui::ListBox(label, current_item, [](void* data, int idx, const char** out_text)
		{
			*out_text = (*reinterpret_cast< std::function<const char*(int)>* >(data))(idx);
			return true;
		}, &lambda, items_count, height_in_items);
	}

	bool LabelClick(const char* concatoff, const char* concaton, const char* label, bool* v, const char* unique_id)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		// The concatoff/on thingies were for my weapon config system so if we're going to make that, we still need this aids.
		char Buf[64];
		_snprintf(Buf, 62, "%s%s", ((*v) ? concatoff : concaton), label);

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(unique_id);
		const ImVec2 label_size = CalcTextSize(label, NULL, true);

		const ImRect check_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(label_size.y + style.FramePadding.y * 2, label_size.y + style.FramePadding.y * 2));
		ItemSize(check_bb, style.FramePadding.y);

		ImRect total_bb = check_bb;
		if (label_size.x > 0)
			SameLine(0, style.ItemInnerSpacing.x);

		const ImRect text_bb(window->DC.CursorPos + ImVec2(0, style.FramePadding.y), window->DC.CursorPos + ImVec2(0, style.FramePadding.y) + label_size);
		if (label_size.x > 0)
		{
			ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
			total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
		}

		if (!ItemAdd(total_bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
		if (pressed)
			*v = !(*v);

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

		ImGui::SameLine();
		ImGui::LabelClick("[", "[", text.c_str(), &clicked, unique_id.c_str());

		if (clicked)
		{
			input_shouldListen = true;
			input_receivedKeyval = key;
		}

		if (*key == KEY_DELETE)
		{
			*key = BUTTON_CODE_NONE;
		}

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Bind the \"del\" key to remove current bind.");
	}

	ImGuiID Colorpicker_Close = 0;
	__inline void CloseLeftoverPicker() { if (Colorpicker_Close) ImGui::ClosePopup(Colorpicker_Close); }
	void ColorPickerBox(const char* picker_idname, float col_ct[], float col_t[], float col_ct_invis[], float col_t_invis[], bool alpha = true)
	{
		ImGui::SameLine();
		static bool switch_entity_teams = false;
		static bool switch_color_vis = false;
		bool open_popup = ImGui::ColorButton(picker_idname, switch_entity_teams ? (switch_color_vis ? col_t : col_t_invis) : (switch_color_vis ? col_ct : col_ct_invis), ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
		if (open_popup)
		{
			ImGui::OpenPopup(picker_idname);
			Colorpicker_Close = ImGui::GetID(picker_idname);
		}

		if (ImGui::BeginPopup(picker_idname))
		{
			const char* button_name0 = switch_entity_teams ? "Terrorists" : "Counter-Terrorists";
			if (ImGui::Button(button_name0, ImVec2(-1, 0)))
				switch_entity_teams = !switch_entity_teams;

			const char* button_name1 = switch_color_vis ? "Invisible" : "Visible";
			if (ImGui::Button(button_name1, ImVec2(-1, 0)))
				switch_color_vis = !switch_color_vis;

			std::string id_new = picker_idname;
			id_new += "##pickeritself_";

			ImGui::ColorPicker4(id_new.c_str(), switch_entity_teams ? (switch_color_vis ? col_t : col_t_invis) : (switch_color_vis ? col_ct : col_ct_invis), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
			ImGui::EndPopup();
		}
	}

	void LocalPlayerColorPickerBox(const char* picker_idname, float col_ct[], float col_t[], bool alpha = false)
	{
		ImGui::SameLine();
		static bool switch_entity_teams = false;
		static bool switch_color_vis = false;
		bool open_popup = ImGui::ColorButton(picker_idname, switch_entity_teams ? (col_t) : (col_ct), ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
		if (open_popup)
		{
			ImGui::OpenPopup(picker_idname);
			Colorpicker_Close = ImGui::GetID(picker_idname);
		}

		if (ImGui::BeginPopup(picker_idname))
		{
			const char* button_name1 = switch_color_vis ? "Invisible" : "Visible";
			if (ImGui::Button(button_name1, ImVec2(-1, 0)))
				switch_color_vis = !switch_color_vis;

			std::string id_new = picker_idname;
			id_new += "##pickeritself_";

			ImGui::ColorPicker4(id_new.c_str(), switch_color_vis ? (col_t) : (col_ct), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
			ImGui::EndPopup();
		}
	}

	void CustomColorPickerBox(const char* picker_idname, float col_ct[], float col_t[], bool alpha = false)
	{
		ImGui::SameLine();
		static bool switch_entity_teams = false;
		static bool switch_color_vis = false;
		bool open_popup = ImGui::ColorButton(picker_idname, switch_entity_teams ? (col_t) : (col_ct), ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
		if (open_popup)
		{
			ImGui::OpenPopup(picker_idname);
			Colorpicker_Close = ImGui::GetID(picker_idname);
		}

		if (ImGui::BeginPopup(picker_idname))
		{
			const char* button_name1 = switch_color_vis ? "Skybox" : "World";
			if (ImGui::Button(button_name1, ImVec2(-1, 0)))
				switch_color_vis = !switch_color_vis;

			std::string id_new = picker_idname;
			id_new += "##pickeritself_";

			ImGui::ColorPicker4(id_new.c_str(), switch_color_vis ? (col_t) : (col_ct), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
			ImGui::EndPopup();
		}
	}

	void ColorPickerBox(const char* picker_idname, float col_n[], bool alpha = true)
	{
		ImGui::SameLine();
		bool open_popup = ImGui::ColorButton(picker_idname, col_n, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
		if (open_popup)
		{
			ImGui::OpenPopup(picker_idname);
			Colorpicker_Close = ImGui::GetID(picker_idname);
		}

		if (ImGui::BeginPopup(picker_idname))
		{
			std::string id_new = picker_idname;
			id_new += "##pickeritself_";

			ImGui::ColorPicker4(id_new.c_str(), col_n, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
			ImGui::EndPopup();
		}
	}

	// This can be used anywhere, in group boxes etc.
	void SelectTabs(int *selected, const char* items[], int item_count, ImVec2 size = ImVec2(0, 0))
	{
		auto color_grayblue = GetColorU32(ImVec4(0.54, 0.54, 0.54, 0.30));
		auto color_deepblue = GetColorU32(ImVec4(0.54, 0.54, 0.54, 0.25));
		auto color_shade_hover = GetColorU32(ImVec4(1, 1, 1, 0.05));
		auto color_shade_clicked = GetColorU32(ImVec4(1, 1, 1, 0.1));
		auto color_black_outlines = GetColorU32(ImVec4(0, 0, 0, 1));

		ImGuiStyle &style = GetStyle();
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return;

		std::string names;
		for (int32_t i = 0; i < item_count; i++)
			names += items[i];

		ImGuiContext* g = GImGui;
		const ImGuiID id = window->GetID(names.c_str());
		const ImVec2 label_size = CalcTextSize(names.c_str(), NULL, true);

		ImVec2 Min = window->DC.CursorPos;
		ImVec2 Max = ((size.x <= 0 || size.y <= 0) ? ImVec2(Min.x + GetContentRegionMax().x - style.WindowPadding.x, Min.y + label_size.y * 2) : Min + size);

		ImRect bb(Min, Max);
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return;

		PushClipRect(ImVec2(Min.x, Min.y - 1), ImVec2(Max.x, Max.y + 1), false);

		window->DrawList->AddRectFilledMultiColor(Min, Max, color_grayblue, color_grayblue, color_deepblue, color_deepblue); // Main gradient.

		ImVec2 mouse_pos = GetMousePos();
		bool mouse_click = g->IO.MouseClicked[0];

		float TabSize = ceil((Max.x - Min.x) / item_count);

		for (int32_t i = 0; i < item_count; i++)
		{
			ImVec2 Min_cur_label = ImVec2(Min.x + (int)TabSize * i, Min.y);
			ImVec2 Max_cur_label = ImVec2(Min.x + (int)TabSize * i + (int)TabSize, Max.y);

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
				window->DrawList->AddLine(ImVec2(Min_cur_label.x - 1.5f, Min_cur_label.y - 1), ImVec2(Max_cur_label.x - 0.5f, Min_cur_label.y - 1), color_black_outlines);
			}
			else
				window->DrawList->AddLine(ImVec2(Min_cur_label.x - 1, Min_cur_label.y), ImVec2(Max_cur_label.x, Min_cur_label.y), color_black_outlines);
			window->DrawList->AddLine(ImVec2(Max_cur_label.x - 1, Max_cur_label.y), ImVec2(Max_cur_label.x - 1, Min_cur_label.y - 0.5f), color_black_outlines);

			const ImVec2 text_size = CalcTextSize(items[i], NULL, true);
			float pad_ = style.FramePadding.x + g->FontSize + style.ItemInnerSpacing.x;
			ImRect tab_rect(Min_cur_label, Max_cur_label);
			RenderTextClipped(Min_cur_label, Max_cur_label, items[i], NULL, &text_size, style.WindowTitleAlign, &tab_rect);
		}

		window->DrawList->AddLine(ImVec2(Min.x, Min.y - 0.5f), ImVec2(Min.x, Max.y), color_black_outlines);
		window->DrawList->AddLine(ImVec2(Min.x, Max.y), Max, color_black_outlines);
		PopClipRect();
	}
}

namespace GladiatorMenu
{
	ImFont* cheat_font;
	ImFont* title_font;
	
	void GUI_Init(HWND window, IDirect3DDevice9 *pDevice)
	{
		static int hue = 140;

		ImGui_ImplDX9_Init(window, pDevice);

		ImGuiStyle& style = ImGui::GetStyle();

		ImVec4 col_text = ImColor::HSV(hue / 255.f, 20.f / 255.f, 235.f / 255.f);
		ImVec4 col_main = ImColor(147, 147, 147); //ImColor(9, 82, 128); button
		ImVec4 col_back = ImColor(89, 89, 89); // button non active
		ImVec4 col_area = ImColor(4, 32, 41); // button outline
		ImVec4 col_theme = ImColor(130, 177, 255); // keine ahnung xD

		ImColor mainColor = ImColor(int(54), int(54), int(54), 255);
		ImColor bodyColor = ImColor(int(24), int(24), int(24), 255);
		ImColor fontColor = ImColor(int(255), int(255), int(255), 255);



		ImVec4 mainColorHovered = ImVec4(mainColor.Value.x + 0.1f, mainColor.Value.y + 0.1f, mainColor.Value.z + 0.1f, mainColor.Value.w);
		ImVec4 mainColorActive = ImVec4(mainColor.Value.x + 0.2f, mainColor.Value.y + 0.2f, mainColor.Value.z + 0.2f, mainColor.Value.w);
		ImVec4 menubarColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w - 0.8f);
		ImVec4 frameBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .1f);
		ImVec4 tooltipBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .05f);
		ImVec4 col_title = ImColor(89, 89, 89);
		style.Colors[ImGuiCol_Text] = ImColor(254, 254, 254, 255);

		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImColor(26, 26, 26, 255);
		style.Colors[ImGuiCol_ChildWindowBg] = ImColor(18, 18, 18, 255);
		style.Colors[ImGuiCol_PopupBg] = tooltipBgColor;
		style.Colors[ImGuiCol_Border] = ImColor(18, 18, 18, 0);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImColor(14, 14, 14, 255);
		style.Colors[ImGuiCol_FrameBgHovered] = ImColor(15, 15, 15, 255);
		style.Colors[ImGuiCol_FrameBgActive] = ImColor(15, 15, 15, 255);
		style.Colors[ImGuiCol_TitleBg] = mainColor;
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		style.Colors[ImGuiCol_TitleBgActive] = mainColor;
		style.Colors[ImGuiCol_MenuBarBg] = menubarColor;
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(frameBgColor.x + .05f, frameBgColor.y + .05f, frameBgColor.z + .05f, frameBgColor.w);
		style.Colors[ImGuiCol_ScrollbarGrab] = mainColor;
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = mainColorHovered;
		style.Colors[ImGuiCol_ScrollbarGrabActive] = mainColorActive;
		style.Colors[ImGuiCol_ComboBg] = ImColor(14, 14, 14, 255);
		style.Colors[ImGuiCol_CheckMark] = ImColor(240, 240, 240, 255);
		style.Colors[ImGuiCol_SliderGrab] = mainColorHovered;
		style.Colors[ImGuiCol_SliderGrabActive] = mainColorActive;
		style.Colors[ImGuiCol_Button] = ImColor(18, 18, 18, 255);
		style.Colors[ImGuiCol_ButtonHovered] = ImColor(169, 79, 12);
		style.Colors[ImGuiCol_ButtonActive] = ImColor(199, 79, 13);
		style.Colors[ImGuiCol_Header] = mainColor;
		style.Colors[ImGuiCol_HeaderHovered] = mainColorHovered;
		style.Colors[ImGuiCol_HeaderActive] = mainColorActive;

		style.Colors[ImGuiCol_Column] = mainColor;
		style.Colors[ImGuiCol_ColumnHovered] = mainColorHovered;
		style.Colors[ImGuiCol_ColumnActive] = mainColorActive;

		style.Colors[ImGuiCol_ResizeGrip] = mainColor;
		style.Colors[ImGuiCol_ResizeGripHovered] = mainColorHovered;
		style.Colors[ImGuiCol_ResizeGripActive] = mainColorActive;
		style.Colors[ImGuiCol_CloseButton] = mainColor;
		style.Colors[ImGuiCol_CloseButtonHovered] = mainColorHovered;
		style.Colors[ImGuiCol_CloseButtonActive] = mainColorActive;
		style.Colors[ImGuiCol_PlotLines] = mainColor;
		style.Colors[ImGuiCol_PlotLinesHovered] = mainColorHovered;
		style.Colors[ImGuiCol_PlotHistogram] = mainColor;
		style.Colors[ImGuiCol_PlotHistogramHovered] = mainColorHovered;
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

		style.Alpha = 1.0f;
		style.WindowPadding = ImVec2(16, 8);
		style.WindowMinSize = ImVec2(32, 32);
		style.WindowRounding = 0.0f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.ChildWindowRounding = 0.0f;
		style.FramePadding = ImVec2(4, 3);
		style.FrameRounding = 0.0f;
		style.ItemSpacing = ImVec2(4, 3);
		style.ItemInnerSpacing = ImVec2(4, 4);
		style.TouchExtraPadding = ImVec2(0, 0);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 3.0f;
		style.ScrollbarSize = 8.f;
		style.ScrollbarRounding = 0.0f;
		style.GrabMinSize = 1.0f;
		style.GrabRounding = 0.0f;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.DisplayWindowPadding = ImVec2(22, 22);
		style.DisplaySafeAreaPadding = ImVec2(4, 4);
		style.AntiAliasedLines = true;
		style.AntiAliasedShapes = true;
		style.CurveTessellationTol = 1.25f;

		d3dinit = true;
		cheat_font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 13);
		title_font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 14);
	}

	void openMenu()
	{
		static bool bDown = false;
		static bool bClicked = false;
		static bool bPrevMenuState = menuOpen;

		if (pressedKey[VK_INSERT])
		{
			bClicked = false;
			bDown = true;
		}
		else if (!pressedKey[VK_INSERT] && bDown)
		{
			bClicked = true;
			bDown = false;
		}
		else
		{
			bClicked = false;
			bDown = false;
		}

		if (bClicked)
		{
			menuOpen = !menuOpen;
			ImGui::CloseLeftoverPicker();
		}

		if (bPrevMenuState != menuOpen)
		{
			// outdated (nkpl)
			//std::string msg = "cl_mouseenable " + std::to_string(!menuOpen);
			//g_EngineClient->ExecuteClientCmd(msg.c_str());
		}

		bPrevMenuState = menuOpen;
	}

	void mainWindow()
	{
		ImGui::SetNextWindowSize(ImVec2(860, 540), ImGuiSetCond_FirstUseEver);

		ImGui::PushFont(title_font);
		if (ImGui::Begin("www.lumisquad.xyz - Fixed by ghosty1337", &menuOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_OnlyDragByTitleBar))
		{
			ImGui::BeginGroup();

			static int selected_Tab = 0;
			static const char* items[7] = { "Rage","Legit", "Visuals", "Misc", "Anti-Aim", "Resolver" ,"Skins" };
			ImGui::SelectTabs(&selected_Tab, items, 7);

			ImGui::EndGroup();

			ImGui::BeginGroup();
			ImGui::PushID(selected_Tab);		

			ImGui::PushFont(cheat_font);
			switch (selected_Tab)
			{
			case 0:
				ragebotTab();
				break;
			case 1:
				legitTab();
				break;
			case 2:
				visualTab();
				break;
			case 3:
				miscTab();
				break;
			case 4:
				hvhTab();
				break;
			case 5:
				resolverTab();
				break;
			case 6:
				skinchangerTab();
				break;
			}
			ImGui::PopFont();

			ImGui::PopID();
			ImGui::EndGroup();

			ImGui::End();
		}
		ImGui::PopFont();
	}

	void aimbotTab()
	{
		ImGui::Separator();
		static int selected_Tab = 0;
		static const char* items[9] = { "Pistol", "Deagle/R8" ,"Submachine-Gun", "Shotgun" ,"Machine-Gun", "Assault-Rifle", "Scout", "AWP" ,"Auto-Sniper" };
		ImGui::SelectTabs(&selected_Tab, items, 9);

		ImGui::EndGroup();

		ImGui::BeginGroup();
		ImGui::PushID(selected_Tab);

		ImGui::PushFont(cheat_font);
		ImGui::Separator();
		switch (selected_Tab)
		{
		case 0: // Pistol
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Autowall");
				ImGui::SliderFloat("Pistol ##RagePistol", &g_Options.rage_mindmg_amount_pistol, 0.f, 120.f, "%.2f");
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitchance");
				ImGui::SliderFloat("Pistol %##RagePistol", &g_Options.rage_hitchance_amount_pistol, 0.f, 100.0f, "%.2f");
			}			
			ImGui::EndColumns();
			ImGui::Separator();
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Checkbox("Prioritize Selected Hitbox##RagePistol", &g_Options.bRage_prioritize_pistol);
				ImGui::Combo("Select Hitbox##RagePistol", &g_Options.iRage_hitbox_pistol, opt_AimHitboxSpot, 4);
				ImGui::Checkbox("Multipoint##RagePistol", &g_Options.bRage_multipoint_pistol);
				ImGui::SliderFloat("Pointscale##RagePistol", &g_Options.rage_pointscale_amount_pistol, 0.0f, 1.0f);
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitboxes To Scan:");
				ImGui::BeginChild("#MULTIPOINTPISTOL", ImVec2(0, 120), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					ImGui::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesPistol[i]);
				}
				ImGui::EndChild();
			}
			break;
		case 1: // Deagle
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Autowall");
				ImGui::SliderFloat("Deagle/R8 ##RageDeagR8", &g_Options.rage_mindmg_amount_deagr8, 0.f, 120.f, "%.2f");
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitchance");
				ImGui::SliderFloat("Deagle/R8 %##RageDeagR8", &g_Options.rage_hitchance_amount_deagr8, 0.f, 100.0f, "%.2f");
			}
			ImGui::EndColumns();
			ImGui::Separator();
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Checkbox("Prioritize Selected Hitbox##RageDeagR8", &g_Options.bRage_prioritize_deagr8);
				ImGui::Combo("Select Hitbox##RageDeagR8", &g_Options.iRage_hitbox_deagr8, opt_AimHitboxSpot, 4);
				ImGui::Checkbox("Multipoint##RageDeagR8", &g_Options.bRage_multipoint_deagr8);
				ImGui::SliderFloat("Pointscale##RageDeagR8", &g_Options.rage_pointscale_amount_deagr8, 0.0f, 1.0f);
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitboxes To Scan:");
				ImGui::BeginChild("#MULTIPOINTDEAGLER8", ImVec2(0, 120), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					ImGui::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesDeagR8[i]);
				}
				ImGui::EndChild();
			}
			break;

		case 2: // SMG
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Autowall");
				ImGui::SliderFloat("SMG ##RageSMG", &g_Options.rage_mindmg_amount_smg, 0.f, 120.f, "%.2f");
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitchance");
				ImGui::SliderFloat("SMG %##RageSMG", &g_Options.rage_hitchance_amount_smg, 0.f, 100.0f, "%.2f");
			}
			ImGui::EndColumns();
			ImGui::Separator();
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Checkbox("Prioritize Selected Hitbox##RageSMG", &g_Options.bRage_prioritize_smg);
				ImGui::Combo("Select Hitbox##RageSMG", &g_Options.iRage_hitbox_smg, opt_AimHitboxSpot, 4);
				ImGui::Checkbox("Multipoint##RageSMG", &g_Options.bRage_multipoint_smg);
				ImGui::SliderFloat("Pointscale##RageSMG", &g_Options.rage_pointscale_amount_smg, 0.0f, 1.0f);
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitboxes To Scan:");
				ImGui::BeginChild("#MULTIPOINTSMG", ImVec2(0, 120), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					ImGui::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesSMG[i]);
				}
				ImGui::EndChild();
			}
			break;

		case 3: // Shotgun
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Autowall");
				ImGui::SliderFloat("Shotgun ##RageSHOTGUN", &g_Options.rage_mindmg_amount_shotgun, 0.f, 120.f, "%.2f");
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitchance");
				ImGui::SliderFloat("Shotgun %##RageSHOTGUN", &g_Options.rage_hitchance_amount_shotgun, 0.f, 100.0f, "%.2f");
			}
			ImGui::EndColumns();
			ImGui::Separator();
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Checkbox("Prioritize Selected Hitbox##RageSHOTGUN", &g_Options.bRage_prioritize_shotgun);
				ImGui::Combo("Select Hitbox##RageSHOTGUN", &g_Options.iRage_hitbox_shotgun, opt_AimHitboxSpot, 4);
				ImGui::Checkbox("Multipoint##RageSHOTGUN", &g_Options.bRage_multipoint_shotgun);
				ImGui::SliderFloat("Pointscale##RageSHOTGUN", &g_Options.rage_pointscale_amount_shotgun, 0.0f, 1.0f);
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitboxes To Scan:");
				ImGui::BeginChild("#MULTIPOINTSHOTGUN", ImVec2(0, 120), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					ImGui::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesShotgun[i]);
				}
				ImGui::EndChild();
			}
			break;

		case 4: // MG
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Autowall");
				ImGui::SliderFloat("MG ##RageMG", &g_Options.rage_mindmg_amount_mg, 0.f, 120.f, "%.2f");
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitchance");
				ImGui::SliderFloat("MG %##RageMG", &g_Options.rage_hitchance_amount_mg, 0.f, 100.0f, "%.2f");
			}
			ImGui::EndColumns();
			ImGui::Separator();
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Checkbox("Prioritize Selected Hitbox##RageMG", &g_Options.bRage_prioritize_mg);
				ImGui::Combo("Select Hitbox##RageMG", &g_Options.iRage_hitbox_mg, opt_AimHitboxSpot, 4);
				ImGui::Checkbox("Multipoint##RageMG", &g_Options.bRage_multipoint_mg);
				ImGui::SliderFloat("Pointscale##RageMG", &g_Options.rage_pointscale_amount_mg, 0.0f, 1.0f);
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitboxes To Scan:");
				ImGui::BeginChild("#MULTIPOINTMG", ImVec2(0, 120), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					ImGui::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesMG[i]);
				}
				ImGui::EndChild();
			}
			break;

		case 5: // ASSAULT RIFLE
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Autowall");
				ImGui::SliderFloat("Assault Rifle ##RageAssaultRifle", &g_Options.rage_mindmg_amount_assaultrifle, 0.f, 120.f, "%.2f");
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitchance");
				ImGui::SliderFloat("Assault Rifle %##RageAssaultRifle", &g_Options.rage_hitchance_amount_assaultrifle, 0.f, 100.0f, "%.2f");
			}
			ImGui::EndColumns();
			ImGui::Separator();
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Checkbox("Prioritize Selected Hitbox##RageAssaultRifle", &g_Options.bRage_prioritize_assaultrifle);
				ImGui::Combo("Select Hitbox##RageAssaultRifle", &g_Options.iRage_hitbox_assaultrifle, opt_AimHitboxSpot, 4);
				ImGui::Checkbox("Multipoint##RageAssaultRifle", &g_Options.bRage_multipoint_assaultrifle);
				ImGui::SliderFloat("Pointscale##RageAssaultRifle", &g_Options.rage_pointscale_amount_assaultrifle, 0.0f, 1.0f);
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitboxes To Scan:");
				ImGui::BeginChild("#MULTIPOINTAssaultRifle", ImVec2(0, 120), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					ImGui::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesAssaultRifle[i]);
				}
				ImGui::EndChild();
			}
			break;
		case 6: // SCOUT
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Autowall");
				ImGui::SliderFloat("Scout ##RageScout", &g_Options.rage_mindmg_amount_scout, 0.f, 120.f, "%.2f");
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitchance");
				ImGui::SliderFloat("Scout %##RageScout", &g_Options.rage_hitchance_amount_scout, 0.f, 100.0f, "%.2f");
			}
			ImGui::EndColumns();
			ImGui::Separator();
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Checkbox("Prioritize Selected Hitbox##RageScout", &g_Options.bRage_prioritize_scout);
				ImGui::Combo("Select Hitbox##RageScout", &g_Options.iRage_hitbox_scout, opt_AimHitboxSpot, 4);
				ImGui::Checkbox("Multipoint##RageScout", &g_Options.bRage_multipoint_scout);
				ImGui::SliderFloat("Pointscale##RageScout", &g_Options.rage_pointscale_amount_scout, 0.0f, 1.0f);
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitboxes To Scan:");
				ImGui::BeginChild("#MULTIPOINTScout", ImVec2(0, 120), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					ImGui::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesScout[i]);
				}
				ImGui::EndChild();
			}
			break;
		case 7: // AWP
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Autowall");
				ImGui::SliderFloat("AWP ##RageAWP", &g_Options.rage_mindmg_amount_awp, 0.f, 120.f, "%.2f");
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitchance");
				ImGui::SliderFloat("AWP %##RageAWP", &g_Options.rage_hitchance_amount_awp, 0.f, 100.0f, "%.2f");
			}
			ImGui::EndColumns();
			ImGui::Separator();
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Checkbox("Prioritize Selected Hitbox##RageAWP", &g_Options.bRage_prioritize_awp);
				ImGui::Combo("Select Hitbox##RageAWP", &g_Options.iRage_hitbox_awp, opt_AimHitboxSpot, 4);
				ImGui::Checkbox("Multipoint##RageAWP", &g_Options.bRage_multipoint_awp);
				ImGui::SliderFloat("Pointscale##RageAWP", &g_Options.rage_pointscale_amount_awp, 0.0f, 1.0f);
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitboxes To Scan:");
				ImGui::BeginChild("#MULTIPOINTAWP", ImVec2(0, 120), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					ImGui::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesAWP[i]);
				}
				ImGui::EndChild();
			}
			break;
		case 8: // AUTO SNIPER
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Autowall");
				ImGui::SliderFloat("Auto ##RageAuto", &g_Options.rage_mindmg_amount_auto, 0.f, 120.f, "%.2f");
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitchance");
				ImGui::SliderFloat("Auto %##RageAuto", &g_Options.rage_hitchance_amount_auto, 0.f, 100.0f, "%.2f");
			}
			ImGui::EndColumns();
			ImGui::Separator();
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Checkbox("Prioritize Selected Hitbox##RageAuto", &g_Options.bRage_prioritize_auto);
				ImGui::Combo("Select Hitbox##RageAuto", &g_Options.iRage_hitbox_auto, opt_AimHitboxSpot, 4);
				ImGui::Checkbox("Multipoint##RageAuto", &g_Options.bRage_multipoint_auto);
				ImGui::SliderFloat("Pointscale##RageAuto", &g_Options.rage_pointscale_amount_auto, 0.0f, 1.0f);
			}
			ImGui::NextColumn();
			{
				ImGui::Text("Hitboxes To Scan:");
				ImGui::BeginChild("#MULTIPOINTAuto", ImVec2(0, 120), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
				{
					ImGui::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxesAuto[i]);
				}
				ImGui::EndChild();
			}
			break;
		}
		//ImGui::Separator();
		ImGui::PopFont();
		ImGui::PopID();
		ImGui::EndGroup();
	}

	void legitTab()
	{
		ImGui::BeginChild("LEGITBOTCHILD", ImVec2(0, 0), true);
		{
			ImGui::Text("Aim Assistance");
			ImGui::Separator();
			ImGui::Columns(1, NULL, true);
			{
				ImGui::Checkbox("Aim-Assist##Legit", &g_Options.legit_enabled);
				ImGui::Checkbox("Aim on key only", &g_Options.legit_on_press);
				ImGui::KeyBindButton(&g_Options.legit_aim_keybind);
				ImGui::Checkbox("Silent##Legit", &g_Options.rage_silent);
				ImGui::Checkbox("RCS##Legit", &g_Options.legit_rcs);
				ImGui::Checkbox("RCS on key only", &g_Options.rcs_on_press);
				ImGui::KeyBindButton(&g_Options.rcs_keybind);
				ImGui::Checkbox("Trigger##Legit", &g_Options.legit_trigger);
				ImGui::Checkbox("Trigger on key only", &g_Options.trigger_on_press);
				ImGui::KeyBindButton(&g_Options.trigger_keybind);
				ImGui::Checkbox("Head only", &g_Options.legit_hsonly);
			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text("Aim Spot");
			ImGui::Separator();
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Pre Aim Spot");
				ImGui::NewLine();
				ImGui::Text("Bullet After Aim");
				ImGui::NewLine();
				ImGui::Text("After Aim Spot");
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::Combo("##PREAIMSPOT", &g_Options.legit_preaim, opt_AimSpot, 4);
				ImGui::NewLine();
				ImGui::SliderInt("##BULLETAFTERAIM", &g_Options.legit_aftershots, 3, 15);
				ImGui::NewLine();
				ImGui::Combo("##AFTERAIMSPOT", &g_Options.legit_afteraim, opt_AimSpot, 4);
				ImGui::PopItemWidth();
			}

			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text("Fov & Smooth");
			ImGui::Separator();
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Fov");
				ImGui::NewLine();
				ImGui::Text("Smooth");
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat("##FOV", &g_Options.legit_fov, 0.f, 3.f, "%.2f");
				ImGui::NewLine();
				ImGui::SliderFloat("##SMOOTH", &g_Options.legit_smooth_factor, 1.f, 30.f, "%.2f");
				ImGui::PopItemWidth();
			}

			ImGui::Columns(1);
			ImGui::Separator();

			ImGui::EndChild();
		}
	}

	void ragebotTab()
	{
		ImGui::Columns(1, NULL, false);
		{
			ImGui::BeginChild("RAGEBOTCHILD", ImVec2(0, 0), true);
			{
				ImGui::Columns(1, NULL, true);
				{
					ImGui::Checkbox("Enable Rage##Rage", &g_Options.rage_enabled);
				}

				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Columns(2, NULL, true);
				{
					ImGui::Checkbox("Silent##Rage", &g_Options.rage_silent);
					ImGui::Checkbox("No Recoil##Rage", &g_Options.rage_norecoil);
					ImGui::Checkbox("Auto Shoot##Rage", &g_Options.rage_autoshoot);
					//ImGui::Checkbox("Resolver##Resolver", &g_Options.hvh_resolver);
					//ImGui::Checkbox("Resolver Flip##Resolver", &g_Options.hvh_resolver_override);
					//ImGui::KeyBindButton(&g_Options.hvh_resolver_override_key);
					ImGui::Checkbox("Position Adjustment (Backtrack)##Rage", &g_Options.rage_lagcompensation);
					ImGui::Checkbox("only shoot backtrack if needed##Rage", &g_Options.backtrack_bhd_wall_only);
				}
				ImGui::NextColumn();
				{
					ImGui::Checkbox("Auto Scope##Rage", &g_Options.rage_autoscope);
					ImGui::Checkbox("Auto Crouch##Rage", &g_Options.rage_autocrouch);
					ImGui::Checkbox("Auto Stop##Rage", &g_Options.rage_autostop);
					ImGui::Checkbox("Auto Revolver##Rage", &g_Options.rage_autocockrevolver);
					//ImGui::Checkbox("Aimstep##Rage", &g_Options.rage_aimstep);
					//ImGui::Checkbox("Auto Body-Aim##Rage", &g_Options.rage_autobaim);
					//ImGui::SliderInt("Body-Aim After Shots##Rage", &g_Options.rage_baim_after_x_shots, 0, 10);
				}

				ImGui::Columns(1);
				//ImGui::Separator();
				ImGui::Columns(1, NULL, false);
				{
					ImGui::PushItemWidth(235);
					//ImGui::Checkbox("Position Adjustment##Rage", &g_Options.rage_lagcompensation);
					ImGui::PopItemWidth();
				}
				ImGui::Columns(1);
				/*ImGui::Separator();
				ImGui::Columns(2, NULL, true);
				{
					ImGui::Text("Autowall");
					ImGui::SliderFloat("Auto ##Rage", &g_Options.rage_mindmg_amount_auto, 0.f, 120.f, "%.2f");
					ImGui::SliderFloat("AWP ##Rage", &g_Options.rage_mindmg_amount_awp, 0.f, 120.f, "%.2f");
					ImGui::SliderFloat("Scout ##Rage", &g_Options.rage_mindmg_amount_scout, 0.f, 120.f, "%.2f");
					ImGui::SliderFloat("Pistol ##Rage", &g_Options.rage_mindmg_amount_pistol, 0.f, 120.f, "%.2f");
					ImGui::SliderFloat("DeagR8 ##Rage", &g_Options.rage_mindmg_amount_deagr8, 0.f, 120.f, "%.2f");
					ImGui::SliderFloat("Shotgun ##Rage", &g_Options.rage_mindmg_amount_shotgun, 0.f, 120.f, "%.2f");
					ImGui::SliderFloat("MG ##Rage", &g_Options.rage_mindmg_amount_mg, 0.f, 120.f, "%.2f");
					ImGui::SliderFloat("SMG ##Rage", &g_Options.rage_mindmg_amount_smg, 0.f, 120.f, "%.2f");
					ImGui::SliderFloat("Assault Rifle ##Rage", &g_Options.rage_mindmg_amount_assaultrifle, 0.f, 120.f, "%.2f");
				}
				ImGui::NextColumn();
				{
					ImGui::Text("Hitchance");
					ImGui::SliderFloat("Auto %##Rage", &g_Options.rage_hitchance_amount_auto, 0.f, 100.0f, "%.2f");
					ImGui::SliderFloat("AWP %##Rage", &g_Options.rage_hitchance_amount_awp, 0.f, 100.0f, "%.2f");
					ImGui::SliderFloat("Scout %##Rage", &g_Options.rage_hitchance_amount_scout, 0.f, 100.0f, "%.2f");
					ImGui::SliderFloat("Pistol %##Rage", &g_Options.rage_hitchance_amount_pistol, 0.f, 100.0f, "%.2f");
					ImGui::SliderFloat("DeagR8 %##Rage", &g_Options.rage_hitchance_amount_deagr8, 0.f, 100.0f, "%.2f");
					ImGui::SliderFloat("Shotgun %##Rage", &g_Options.rage_hitchance_amount_shotgun, 0.f, 100.0f, "%.2f");
					ImGui::SliderFloat("MG %##Rage", &g_Options.rage_hitchance_amount_mg, 0.f, 100.0f, "%.2f");
					ImGui::SliderFloat("SMG %##Rage", &g_Options.rage_hitchance_amount_smg, 0.f, 100.0f, "%.2f");
					ImGui::SliderFloat("Assault Rifle %##Rage", &g_Options.rage_hitchance_amount_assaultrifle, 0.f, 100.0f, "%.2f");
				}*/
				

				ImGui::Columns(1);
				//ImGui::Separator();
				//ImGui::Text("Target");
				//ImGui::Separator();
				ImGui::BeginGroup();

				aimbotTab();

				ImGui::Columns(1);
				ImGui::Separator();

				ImGui::EndChild();
			}
		}
	}

	void visualTab()
	{
		ImGui::Columns(2, NULL, false);
		{
			ImGui::BeginChild("COL1", ImVec2(0, 0), true);
			{
				ImGui::Text("ESP");
				ImGui::Separator();
				ImGui::Columns(1);
				{
					ImGui::Combo("Box Type##BOXTYPE", &g_Options.esp_player_boxtype, opt_EspType, 3);
					ImGui::ColorPickerBox("##Picker_box", g_Options.esp_player_bbox_color_ct, g_Options.esp_player_bbox_color_t, g_Options.esp_player_bbox_color_ct_visible, g_Options.esp_player_bbox_color_t_visible, false);
					ImGui::Combo("Bounds Style##BOUNDSTYPE", &g_Options.esp_player_boundstype, opt_BoundsType, 2);
					ImGui::SliderFloat("Fill Alpha##ESP", &g_Options.esp_fill_amount, 0.f, 255.f);
					ImGui::ColorPickerBox("##Picker_fill", g_Options.esp_player_fill_color_ct, g_Options.esp_player_fill_color_t, g_Options.esp_player_fill_color_ct_visible, g_Options.esp_player_fill_color_t_visible, false);
					//ImGui::Checkbox("Ignore Team##ESP", &g_Options.esp_enemies_only);
					ImGui::Checkbox("Farther ESP##ESP", &g_Options.esp_farther);
					ImGui::Checkbox("Name##ESP", &g_Options.esp_player_name);
					ImGui::Checkbox("Health##ESP", &g_Options.esp_player_health);
					ImGui::Checkbox("Weapon Name##ESP", &g_Options.esp_player_weapons);
					ImGui::Checkbox("Skeleton##ESP", &g_Options.esp_player_skelet);
					ImGui::Checkbox("Change World Color##ESP", &g_Options.visuals_nightmode);
					ImGui::CustomColorPickerBox("##visuals_others_nightmode_color", g_Options.visuals_others_nightmode_color, g_Options.visuals_others_skybox_color, false);
					//ImGui::SameLine();
					//ImGui::ColorPickerBox("##visuals_others_skybox_color", g_Options.visuals_others_skybox_color, false);
					ImGui::Checkbox("Asus Walls##ESP", &g_Options.visuals_asuswalls);
					if (g_Options.visuals_draw_xhair)
						ImGui::Separator();
					ImGui::Checkbox("Draw Crosshair", &g_Options.visuals_draw_xhair);
					if (g_Options.visuals_draw_xhair)
					{
						ImGui::Separator();
						ImGui::SliderInt("Crosshair X length", &g_Options.visuals_xhair_x, 0, 15);
						ImGui::SliderInt("Crosshair Y length", &g_Options.visuals_xhair_y, 0, 15);
						ImGui::NewLine();
						ImGui::SliderInt("Crosshair - Red", &g_Options.xhair_color[0], 0, 255);
						ImGui::SliderInt("Crosshair - Green", &g_Options.xhair_color[1], 0, 255);
						ImGui::SliderInt("Crosshair - Blue", &g_Options.xhair_color[2], 0, 255);

						ImGui::Separator();
					}
					ImGui::Checkbox("Backtracked Trail##ESP", &g_Options.esp_backtracked_player_skelet);
					ImGui::Checkbox("Lag Compensated Hitboxes##ESP", &g_Options.esp_lagcompensated_hitboxes);
					if (g_Options.esp_lagcompensated_hitboxes)
						ImGui::Combo("Type##ESP", &g_Options.esp_lagcompensated_hitboxes_type, std::vector<std::string>{ "Only Lag Compensated", "Lag(Red) & Non(Blue) Lag Compensated" });
					ImGui::Checkbox("Angle Lines##ESP", &g_Options.esp_player_anglelines);
					ImGui::Checkbox("Chams##ESP", &g_Options.esp_player_chams);
					if (g_Options.esp_player_chams_type > 1 || g_Options.esp_localplayer_chams_type > 1)
						ImGui::Text("set shader details to atleast high, to get the full effect of the new materials!!");
					ImGui::Combo("Chams Type##ESP", &g_Options.esp_player_chams_type, opt_Chams, 10);
					ImGui::ColorPickerBox("##Picker_chams_players", g_Options.esp_player_chams_color_ct, g_Options.esp_player_chams_color_t, g_Options.esp_player_chams_color_ct_visible, g_Options.esp_player_chams_color_t_visible, false);
					ImGui::Separator();
					ImGui::Checkbox("Chams for LocalPlayer", &g_Options.esp_localplayer_chams);
					if (g_Options.esp_localplayer_chams)
					{
						ImGui::Checkbox("XQZ - Chams", &g_Options.esp_localplayer_chams_xyz);
						ImGui::Combo("Real Material##ESP", &g_Options.esp_localplayer_chams_type, opt_Chams_Mat, 9);
						ImGui::LocalPlayerColorPickerBox("##esp_localplayer_chams_color_picker", g_Options.esp_localplayer_chams_color_visible, g_Options.esp_localplayer_chams_color_invisible, false);

						ImGui::Checkbox("Fake Angle Chams", &g_Options.fake_chams);
						if (g_Options.fake_chams)
						{
							ImGui::Separator();
							ImGui::Combo("Fake Material##FakeChams", &g_Options.esp_localplayer_fakechams_type, opt_Chams_Mat, 9);
							ImGui::LocalPlayerColorPickerBox("##esp_localplayer_fakechams_color_picker", g_Options.esp_localplayer_fakechams_color_visible, g_Options.esp_localplayer_fakechams_color_invisible, false);
						}
						ImGui::Separator();
						ImGui::Checkbox("Viewmodel Chams", &g_Options.esp_localplayer_viewmodel_chams);
						if (g_Options.esp_localplayer_viewmodel_chams)
						{
							ImGui::Separator();
							//ImGui::Checkbox("Wireframe", &g_Options.esp_localplayer_viewmodel_chams_wireframe);
							ImGui::Combo("Viewmodel Material##ViewmodelChams", &g_Options.esp_localplayer_viewmodel_materials, opt_Chams_Mat, 9);
							//ImGui::ColorPickerBox("##esp_localplayer_viewmodel_chams_color", g_Options.esp_localplayer_viewmodel_chams_color, true);
						}
					}
				}

				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text("Glow");
				ImGui::Separator();
				ImGui::Columns(1, NULL, true);
				{
					ImGui::Checkbox("Enable Glow##Glow", &g_Options.glow_enabled);
					ImGui::Checkbox("Players##Glow", &g_Options.glow_players);
					ImGui::Combo("Glow Type##ESP_player", &g_Options.glow_players_style, opt_GlowStyles, 3);
					ImGui::ColorPickerBox("##Picker_glow_players", g_Options.glow_player_color_ct, g_Options.glow_player_color_t, g_Options.glow_player_color_ct_visible, g_Options.glow_player_color_t_visible);
					ImGui::Checkbox("Others##Glow", &g_Options.glow_others);
					ImGui::Combo("Glow type others##ESP_other", &g_Options.glow_others_style, opt_GlowStyles, 3);
				}
				
				ImGui::EndChild();
			}
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("COL2", ImVec2(0, 0), true);
			{
				ImGui::Text("Others");
				ImGui::Separator();
				ImGui::Columns(1, NULL, true);
				{
					ImGui::Checkbox("Hit Marker##Others", &g_Options.visuals_others_hitmarker);
					ImGui::Checkbox("Bullet Impacts##Others", &g_Options.visuals_others_bulletimpacts);
					ImGui::ColorPickerBox("##Picker_impacts", g_Options.visuals_others_bulletimpacts_color, false);
					ImGui::Separator();
					ImGui::Checkbox("Show Manual AA Status", &g_Options.visuals_manual_aa);
					ImGui::SliderFloat("Indicator Opacity", &g_Options.visuals_manual_aa_opacity, 0, 255);
					ImGui::Separator();
					ImGui::Checkbox("Grenade Prediction##Others", &g_Options.visuals_others_grenade_pred);
					//ImGui::Checkbox("Autothrow", &g_Options.autothrow_grenades);
					//ImGui::SliderInt("Autothrow Damage", &g_Options.autothrow_grenades_dmg, 0, 100);
					ImGui::Checkbox("Watermark##Others", &g_Options.visuals_others_watermark);
					ImGui::SliderFloat("Field of View##Others", &g_Options.visuals_others_player_fov, 0, 80);
					ImGui::SliderFloat("Viewmodel Field of View##Others", &g_Options.visuals_others_player_fov_viewmodel, 0, 80);
					ImGui::Separator();
					ImGui::Checkbox("Enable Custom Viewmodel Offset", &g_Options.change_viewmodel_offset);
					ImGui::SliderFloat("Viewmodel Offset X##ViewmodelX", &g_Options.viewmodel_offset_x, -30, 30);
					ImGui::SliderFloat("Viewmodel Offset Y##ViewmodelY", &g_Options.viewmodel_offset_y, -30, 30);
					ImGui::SliderFloat("Viewmodel Offset Z##ViewmodelZ", &g_Options.viewmodel_offset_z, -30, 30);
				}

				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text("Removals");
				ImGui::Separator();
				ImGui::Columns(1, NULL, true);
				{
					ImGui::Checkbox("Flash##Removals", &g_Options.removals_flash);
					ImGui::Checkbox("Smoke##Removals", &g_Options.removals_smoke);
					if (g_Options.removals_smoke)
					{
						ImGui::Combo("Smoke Mode##Removals", &g_Options.removals_smoke_type, opt_nosmoketype, 2);
					}
					ImGui::Checkbox("Scope##Removals", &g_Options.removals_scope);
					ImGui::Checkbox("Zoom##Removals", &g_Options.removals_zoom);
					ImGui::Checkbox("Recoil##Removals", &g_Options.removals_novisualrecoil);
					ImGui::Checkbox("Crosshair##Removals", &g_Options.removals_crosshair);
					ImGui::Checkbox("Fog##Removals", &g_Options.fog_override);
					ImGui::Checkbox("Post-Processing##Removals", &g_Options.removals_postprocessing);
				}

				ImGui::Separator();
				ImGui::Text("Other esp");
				ImGui::Separator();
				ImGui::Combo("Dropped Weapons##ESP", &g_Options.esp_dropped_weapons, opt_WeaponBoxType, 4);
				if (g_Options.esp_dropped_weapons > 0)
				{
					ImGui::SliderInt("Drpd. Wep. Red##ESP", &g_Options.dropped_weapons_color[0], 0, 255);
					ImGui::SliderInt("Drpd. Wep. Green##ESP", &g_Options.dropped_weapons_color[1], 0, 255);
					ImGui::SliderInt("Drpd. Wep. Blue##ESP", &g_Options.dropped_weapons_color[2], 0, 255);
				}
				ImGui::Separator();
				ImGui::Checkbox("Planted C4##ESP", &g_Options.esp_planted_c4);
				ImGui::Checkbox("Grenade ESP##ESP", &g_Options.esp_grenades);
				ImGui::Combo("Grenade ESP type##ESP", &g_Options.esp_grenades_type, opt_GrenadeESPType, 4);
				ImGui::EndChild();
			}
		}
	}

	void miscTab()
	{
		ImGui::Columns(2, NULL, false);
		{
			ImGui::BeginChild("COL1", ImVec2(0, 0), true);
			{
				if (ImGui::Button("Unload"))
					Installer::UnloadLumi();
				ImGui::Text("Movement");
				ImGui::Separator();
				ImGui::Columns(1, NULL, true);
				{
					ImGui::Checkbox("Bhop##Movement", &g_Options.misc_bhop);
					ImGui::Checkbox("Auto-Strafe##Movement", &g_Options.misc_autostrafe);
				}

				static char nName[127] = "";
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text("Nickname");
				ImGui::Separator();
				ImGui::Columns(2, NULL, true);
				{
					ImGui::PushItemWidth(-1);
					ImGui::InputText("##NNAMEINPUT", nName, 127);
					ImGui::PopItemWidth();
				}
				ImGui::NextColumn();
				{
					if (ImGui::Button("Set Nickname##Nichnamechanger"))
						Miscellaneous::Get().ChangeName(nName);

					ImGui::SameLine();
					if (ImGui::Button("No Name##Nichnamechanger", ImVec2(-1, 0)))
						Miscellaneous::Get().ChangeName("\n");
				}

				static char ClanChanger[127] = "";
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text("Clan Tag");
				ImGui::Separator();
				ImGui::Columns(2, NULL, true);
				{
					ImGui::PushItemWidth(-1);
					ImGui::InputText("##CLANINPUT", ClanChanger, 127);
					ImGui::PopItemWidth();
				}
				ImGui::NextColumn();
				{
					if (ImGui::Button("Set Clan-Tag"))
						Utils::SetClantag(ClanChanger);

				}
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text("Other");
				ImGui::Separator();
				ImGui::Columns(1, NULL, true);
				{
					ImGui::Checkbox("Auto-Pistol##Other", &g_Options.misc_auto_pistol);
					ImGui::Checkbox("Chat Spammer##Other", &g_Options.misc_chatspamer);
					ImGui::Checkbox("Event Logs##Other", &g_Options.misc_logevents);
					ImGui::Checkbox("Spectator List##Other", &g_Options.misc_spectatorlist);
					ImGui::Checkbox("LumiHook$ Clan-Tag##Other", &g_Options.misc_animated_clantag);
					ImGui::Checkbox("Thirdperson##Other", &g_Options.misc_thirdperson);
					ImGui::KeyBindButton(&g_Options.misc_thirdperson_bind);
					ImGui::Checkbox("Infinite Duck", &g_Options.misc_infinite_duck);
					ImGui::Combo("Thirdperson Mode##AntiAim", &g_Options.hvh_show_real_angles, std::vector<std::string>{ "Real Angles", "Fake Angles" });
					ImGui::Checkbox("Slowwalk##Other", &g_Options.misc_fakewalk);
					ImGui::KeyBindButton(&g_Options.misc_fakewalk_bind);
					ImGui::SliderInt("Slowwalk Speed##Other", &g_Options.misc_fakewalk_speed, 0, 130);
				}

				ImGui::EndChild();
			}
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("COL2", ImVec2(0, 0), true);
			{
				ImGui::Text("Fakelag");
				ImGui::Separator();
				ImGui::Columns(1, NULL, true);
				{
					ImGui::Checkbox("Enabled##Fakelag", &g_Options.misc_fakelag_enabled);
					ImGui::KeyBindButton(&g_Options.misc_fakeduck);
					ImGui::Checkbox("Adaptive##Fakelag", &g_Options.misc_fakelag_adaptive);
					ImGui::Combo("Activation Type##Fakelag", &g_Options.misc_fakelag_activation_type, std::vector<std::string>{ "Always", "While Moving", "In Air" });
					ImGui::SliderInt("Choke##Fakelag", &g_Options.misc_fakelag_value, 0, 14);
				}

				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text("Config");
				ImGui::Separator();
				static std::vector<std::string> configItems = Config::Get().GetAllConfigs();
				static int configItemCurrent = -1;

				static char fName[128] = "default";

				ImGui::Columns(1, NULL, true);
				{
					if (ImGui::Button("Refresh##Config"))
						configItems = Config::Get().GetAllConfigs();

					ImGui::SameLine();
					if (ImGui::Button("Save##Config"))
					{
						if (configItems.size() > 0 && (configItemCurrent >= 0 && configItemCurrent < (int)configItems.size()))
						{
							std::string fPath = std::string(Global::my_documents_folder) + "\\LumiHook$\\" + configItems[configItemCurrent] + ".1337";
							Config::Get().SaveConfig(fPath);
						}
					}

					ImGui::SameLine();
					if (ImGui::Button("Remove##Config"))
					{
						if (configItems.size() > 0 && (configItemCurrent >= 0 && configItemCurrent < (int)configItems.size()))
						{
							std::string fPath = std::string(Global::my_documents_folder) + "\\LumiHook$\\" + configItems[configItemCurrent] + ".1337";
							std::remove(fPath.c_str());

							configItems = Config::Get().GetAllConfigs();
							configItemCurrent = -1;
						}
					}

					ImGui::PushItemWidth(138);
					{
						ImGui::InputText("", fName, 128);
					}
					ImGui::PopItemWidth();

					ImGui::SameLine();
					if (ImGui::Button("Add##Config"))
					{
						std::string fPath = std::string(Global::my_documents_folder) + "\\LumiHook$\\" + fName + ".1337";
						Config::Get().SaveConfig(fPath);

						configItems = Config::Get().GetAllConfigs();
						configItemCurrent = -1;
					}

					ImGui::PushItemWidth(178);
					{
						if (ImGui::ListBox("", &configItemCurrent, configItems, 5))
						{
							std::string fPath = std::string(Global::my_documents_folder) + "\\LumiHook$\\" + configItems[configItemCurrent] + ".1337";
							Config::Get().LoadConfig(fPath);
						}
					}
					ImGui::PopItemWidth();
					ImGui::Separator();
					ImGui::Text("super retarded shit");
					ImGui::Separator();
					ImGui::Checkbox("spoof cl_phys_timescale", &g_Options.cl_phys_timescale);
					ImGui::SliderFloat("cl_phys_timescale_value##cl_phys_timescale", &g_Options.cl_phys_timescale_value, 0, 1, "%.1f");
					ImGui::Separator();
					ImGui::Checkbox("spoof mp_radar_showall", &g_Options.mp_radar_showall);
					ImGui::Separator();
					ImGui::Checkbox("spoof cl_crosshair_recoil", &g_Options.cl_crosshair_recoil);
					ImGui::Separator();
					ImGui::Text("you can request additonal commands that you want to be spoofed in #requests");
					ImGui::Separator();
				}

				ImGui::EndChild();
			}
		}
	}

	void hvhTab()
	{
		ImGui::Columns(1, NULL, true);
		{
			ImGui::BeginChild("COL1", ImVec2(0, 0), true);
			{

				ImGui::Text("Anti-Aim");
				ImGui::Separator();
				{
					ImGui::PushItemWidth(-1);
					ImGui::Text("Pitch");
					ImGui::Combo("Pitch", &g_Options.hvh_antiaim_x, opt_AApitch, 5);
					ImGui::Separator();
					ImGui::NewLine();
					ImGui::Separator();
					ImGui::Text("Yaw");
					ImGui::Combo("Yaw", &g_Options.hvh_antiaim_y, opt_AAyaw, 9);
					if (g_Options.hvh_antiaim_y == 5 || g_Options.hvh_antiaim_y == 6) {
						ImGui::Text("Custom Yaw Left");
						ImGui::SliderFloat("##AAY Custom Yaw Left", &g_Options.hvh_antiaim_y_custom_left, -180.0f, 180.0f, "%1.f");
						ImGui::Text("Custom Yaw Right");
						ImGui::SliderFloat("##AAY Custom Yaw Right", &g_Options.hvh_antiaim_y_custom_right, -180.0f, 180.0f, "%1.f");
						ImGui::Text("Custom Yaw Back");
						ImGui::SliderFloat("##AAY Custom Yaw Back", &g_Options.hvh_antiaim_y_custom_back, -180.0f, 180.0f, "%1.f");
					}
					if (g_Options.hvh_antiaim_y == 7 || g_Options.hvh_antiaim_y == 8) {
						ImGui::Text("Left Angle");
						ImGui::SliderFloat("##AAY Angle Left", &g_Options.hvh_antiaim_y_desync_start_left, -180, 180, "%1.f");
						ImGui::Text("Right Angle");
						ImGui::SliderFloat("##AAY Angle Right", &g_Options.hvh_antiaim_y_desync_start_right, -180, 180, "%1.f");
						ImGui::Text("Back Angle");
						ImGui::SliderFloat("##AAY Angle Back", &g_Options.hvh_antiaim_y_desync_start_back, -180, 180, "%1.f");
						ImGui::Text("Desync");
						ImGui::SliderFloat("##AAY Desync", &g_Options.hvh_antiaim_y_desync, -180, 180, "%1.f");
					}
					ImGui::Separator();
					ImGui::NewLine();
					ImGui::Separator();
					ImGui::Text("Yaw Move");
					ImGui::Combo("##AAY Move", &g_Options.hvh_antiaim_y_move, opt_AAyaw_move, 7);
					ImGui::Text("do when velocity over");
					ImGui::SliderFloat("##MOVEAATRIGGERSPEED", &g_Options.hvh_antiaim_y_move_trigger_speed, 0.1, 130);
					if (g_Options.hvh_antiaim_y_move == 5 || g_Options.hvh_antiaim_y_move == 6) {
						ImGui::Text("Custom Yaw Move Left");
						ImGui::SliderFloat("##AAY Custom Yaw Move Left", &g_Options.hvh_antiaim_y_custom_realmove_left, -180.0f, 180.0f, "%1.f");
						ImGui::Text("Custom Yaw Move Right");
						ImGui::SliderFloat("##AAY Custom Yaw Move Right", &g_Options.hvh_antiaim_y_custom_realmove_right, -180.0f, 180.0f, "%1.f");
						ImGui::Text("Custom Yaw Move Back");
						ImGui::SliderFloat("##AAY Custom Yaw Move Back", &g_Options.hvh_antiaim_y_custom_realmove_back, -180.0f, 180.0f, "%1.f");
					}
					ImGui::PopItemWidth();
				}
				ImGui::Separator();
				ImGui::Text("Left AA Bind"); ImGui::KeyBindButton(&g_Options.hvh_aa_left_bind);
				ImGui::Text("Right AA Bind"); ImGui::KeyBindButton(&g_Options.hvh_aa_right_bind);
				ImGui::Text("Back AA Bind"); ImGui::KeyBindButton(&g_Options.hvh_aa_back_bind);
				ImGui::Separator();
				ImGui::EndChild();
			}
		}
	}

	void skinchangerTab()
	{
		ImGui::BeginChild("SKINCHANGER", ImVec2(0, 0), true);
		{
			if (ImGui::Checkbox("Enabled##Skinchanger", &g_Options.skinchanger_enabled))
				Skinchanger::Get().bForceFullUpdate = true;
			std::vector<EconomyItem_t> &entries = Skinchanger::Get().GetItems();
			// If the user deleted the only config let's add one
			if (entries.size() == 0)
				entries.push_back(EconomyItem_t());
			static int selected_id = 0;
			ImGui::Columns(2, nullptr, false);
			// Config selection
			{
				ImGui::PushItemWidth(-1);
				char element_name[64];
				ImGui::ListBox("##skinchangerconfigs", &selected_id, [&element_name, &entries](int idx)
				{
					sprintf_s(element_name, "%s (%s)", entries.at(idx).name, k_weapon_names.at(entries.at(idx).definition_vector_index).name);
					return element_name;
				}, entries.size(), 15);
				ImVec2 button_size = ImVec2(ImGui::GetColumnWidth() / 2 - 12.8f, 25);
				if (ImGui::Button("Add Item", button_size))
				{
					entries.push_back(EconomyItem_t());
					selected_id = entries.size() - 1;
				}
				ImGui::SameLine();
				if (ImGui::Button("Remove Item", button_size))
					entries.erase(entries.begin() + selected_id);
				ImGui::PopItemWidth();
			}
			ImGui::NextColumn();
			selected_id = selected_id < int(entries.size()) ? selected_id : entries.size() - 1;
			EconomyItem_t &selected_entry = entries[selected_id];
			{
				// Name
				ImGui::InputText("Name", selected_entry.name, 32);
				ImGui::Dummy(ImVec2(1, 4));
				// Item to change skins for
				ImGui::Combo("Item", &selected_entry.definition_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = k_weapon_names[idx].name;
					return true;
				}, nullptr, k_weapon_names.size(), 5);
				ImGui::Dummy(ImVec2(1, 3));
				// Enabled
				ImGui::Checkbox("Enabled", &selected_entry.enabled);
				ImGui::Dummy(ImVec2(1, 3));
				// Pattern Seed
				ImGui::InputInt("Seed", &selected_entry.seed);
				ImGui::Dummy(ImVec2(1, 4));
				// Custom StatTrak number
				ImGui::InputInt("StatTrak", &selected_entry.stat_trak);
				ImGui::Dummy(ImVec2(1, 4));
				// Wear Float
				ImGui::SliderFloat("Wear", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);
				ImGui::Dummy(ImVec2(1, 4));
				// Paint kit
				if (selected_entry.definition_index != GLOVE_T_SIDE)
				{
					ImGui::Combo("PaintKit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = k_skins[idx].name.c_str();
						return true;
					}, nullptr, k_skins.size(), 10);
				}
				else
				{
					ImGui::Combo("PaintKit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = k_gloves[idx].name.c_str();
						return true;
					}, nullptr, k_gloves.size(), 10);
				}
				ImGui::Dummy(ImVec2(1, 4));
				// Quality
				ImGui::Combo("Quality", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = k_quality_names[idx].name;
					return true;
				}, nullptr, k_quality_names.size(), 5);
				ImGui::Dummy(ImVec2(1, 4));
				// Yes we do it twice to decide knifes
				selected_entry.UpdateValues();
				// Item defindex override
				if (selected_entry.definition_index == WEAPON_KNIFE)
				{
					ImGui::Combo("Knife", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = k_knife_names.at(idx).name;
						return true;
					}, nullptr, k_knife_names.size(), 5);
				}
				else if (selected_entry.definition_index == GLOVE_T_SIDE)
				{
					ImGui::Combo("Glove", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
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
					ImGui::Combo("Unavailable", &unused_value, "Only available for knives or gloves!\0");
				}
				ImGui::Dummy(ImVec2(1, 4));
				selected_entry.UpdateValues();
				// Custom Name tag
				ImGui::InputText("Nametag", selected_entry.custom_name, 32);
				ImGui::Dummy(ImVec2(1, 4));
			}
			ImGui::NextColumn();
			ImGui::Columns(1, nullptr, false);
			ImGui::Separator();
			ImGui::Dummy(ImVec2(1, 10));
			ImGui::Columns(3, nullptr, false);
			ImGui::PushItemWidth(-1);
			// Lower buttons for modifying items and saving
			{
				ImVec2 button_size = ImVec2(ImGui::GetColumnWidth() - 17.f, 25);
				if (ImGui::Button("Force Update##Skinchanger", button_size))
					Skinchanger::Get().bForceFullUpdate = true;
				ImGui::NextColumn();
				if (ImGui::Button("Save##Skinchanger", button_size))
					Skinchanger::Get().SaveSkins();
				ImGui::NextColumn();
				if (ImGui::Button("Load##Skinchanger", button_size))
					Skinchanger::Get().LoadSkins();
				ImGui::NextColumn();
			}
			ImGui::PopItemWidth();
			ImGui::Columns(1);
			
			ImGui::EndChild();
		}
	}

	void resolverTab()
	{
		if (g_Options.hvh_resolver)
			g_Options.hvh_resolver_custom = false;
		if (g_Options.hvh_resolver_custom)
			g_Options.hvh_resolver = false;

		ImGui::Columns(2, NULL, true);
		{
			ImGui::BeginChild("COL1", ImVec2(0, 0), true);
			{

				ImGui::Text("Auto. Resolver");
				ImGui::Separator();
				{
					ImGui::PushItemWidth(-1);
					ImGui::Checkbox("Resolver##Resolver", &g_Options.hvh_resolver);
					ImGui::Checkbox("Resolver Flip##Resolver", &g_Options.hvh_resolver_override);
					ImGui::KeyBindButton(&g_Options.hvh_resolver_override_key);
					ImGui::Separator();
					/*ImGui::Checkbox("EyePos = Lby", &g_Options.resolver_eyelby);
					ImGui::Checkbox("always", &g_Options.resolver_eyelby_always);
					ImGui::Checkbox("when velocity over", &g_Options.resolver_eyelby_running);
					ImGui::SameLine();
					ImGui::SliderFloat("##triggerwhenvelocityover", &g_Options.resolver_eyelby_running_value, 0.f, 300.f, "%.0f");
					ImGui::Checkbox("only when standing", &g_Options.resolver_eyelby_notrunning);
					ImGui::Separator();
					if (g_Options.resolver_eyelby_always) { g_Options.resolver_eyelby_running = false; g_Options.resolver_eyelby_notrunning = false; }
					if (g_Options.resolver_eyelby_running) { g_Options.resolver_eyelby_always = false; g_Options.resolver_eyelby_notrunning = false; }
					if (g_Options.resolver_eyelby_notrunning) { g_Options.resolver_eyelby_always = false; g_Options.resolver_eyelby_running = false; }*/
				}
				ImGui::EndChild();
			}
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("COL2", ImVec2(0, 0), true);
			{

				ImGui::Text("Custom Resolver");
				ImGui::Separator();
				{
					ImGui::Text("more will be following soon!");
					ImGui::Separator();
					ImGui::PushItemWidth(-1);
					ImGui::Checkbox("Custom Resolver", &g_Options.hvh_resolver_custom);
					ImGui::Checkbox("Resolver Flip##Resolver", &g_Options.hvh_resolver_override);
					ImGui::KeyBindButton(&g_Options.hvh_resolver_override_key);
					ImGui::Separator();
					ImGui::Checkbox("EyePos = Lby", &g_Options.resolver_eyelby);
					ImGui::Checkbox("always", &g_Options.resolver_eyelby_always);
					ImGui::Checkbox("when velocity over", &g_Options.resolver_eyelby_running);
					ImGui::SameLine();
					ImGui::SliderFloat("##triggerwhenvelocityover", &g_Options.resolver_eyelby_running_value, 0.f, 300.f, "%.0f");
					ImGui::Checkbox("only when standing", &g_Options.resolver_eyelby_notrunning);
					ImGui::Separator();
					if (g_Options.resolver_eyelby_always) { g_Options.resolver_eyelby_running = false; g_Options.resolver_eyelby_notrunning = false; }
					if (g_Options.resolver_eyelby_running) { g_Options.resolver_eyelby_always = false; g_Options.resolver_eyelby_notrunning = false; }
					if (g_Options.resolver_eyelby_notrunning) { g_Options.resolver_eyelby_always = false; g_Options.resolver_eyelby_running = false; }
				}
				ImGui::EndChild();
			}
		}
	}

	bool d3dinit = false;
}
// Junk Code By Troll Face & Thaisen's Gen
void oJpjDhoUpqrGaKcVgVcpnWZKkNXmMNZMRClLgIilnBFig33223506() {     int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN10354478 = -447134858;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN14719852 = -831190569;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN81819563 = -580545720;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN72926889 = -757090450;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN86053549 = 81755071;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN57635483 = -719260716;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN33498742 = -279843697;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN46889894 = -569405513;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN64655738 = -918085456;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN54963479 = -78299177;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN13721477 = -359777590;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN24021930 = -311543615;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN42140452 = -360785880;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN15185021 = -479984723;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN80560275 = -533747430;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN69034069 = -722140627;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN74042062 = -334057052;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN31029233 = -806481191;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN85143508 = -459115911;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN57132471 = -202083562;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN44726642 = -230788093;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN4596009 = -703079699;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN18849627 = -479323376;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN53933757 = -874491141;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN99740237 = -692610833;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN4751089 = 16380985;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN40462986 = -755581722;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN75679210 = -91449894;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN47205956 = -934407545;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN51271919 = 29093290;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN57027747 = -291940392;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN34952528 = -957092591;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN454695 = -497820674;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN91651538 = -291753398;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN4766797 = -167827257;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN24243016 = -740734932;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN2260803 = -408416182;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN12504435 = -172837652;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN64868115 = -78750568;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN98176527 = -574401363;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN60699033 = -340779689;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN40020762 = -233347958;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN57039577 = -19595261;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN85566346 = -786999402;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN84158325 = -551130806;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN73827702 = -276355687;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN65835225 = -821089164;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN48550431 = 12480343;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN78399947 = -547030379;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN65114637 = -386815041;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN87059380 = -534268430;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN31629873 = -749813741;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN26111934 = 39329247;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN3880215 = -833979681;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN88496863 = -609928763;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN5758469 = -744055159;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN95870224 = -251867194;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN27885806 = -706054580;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN73186651 = 35520383;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN81302461 = -934625915;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN17172498 = -963678994;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN57819532 = -88393804;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN99683937 = -634997968;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN13383820 = -847178747;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN97935732 = -786358785;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN78768949 = -402684999;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN23567235 = -813722942;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN50488913 = 30967517;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN10418225 = -212157466;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN56317260 = -793012498;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN66773267 = -213724446;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN61537627 = -61219400;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN66161118 = -627730623;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN86966980 = -884714549;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN96433437 = -861303874;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN4705880 = -997440135;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN47556431 = -583484439;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN33283281 = -692323975;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN69775431 = -223360336;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN25912535 = -316255147;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN38915864 = -162529851;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN91912554 = -668062066;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN97279263 = -544419516;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN82091318 = -447592505;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN64212538 = -436638281;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN25397874 = -542126651;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN8840594 = -896421839;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN96574480 = -663840993;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN3154675 = -681824635;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN99008327 = -423772098;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN28372791 = -388867739;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN74374996 = -702361602;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN39317784 = -108358035;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN83565654 = -144124654;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN81004030 = -610722370;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN2879502 = -152385886;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN40336825 = -598349991;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN43655758 = -172416515;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN87630614 = 99359382;    int RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN5389377 = -447134858;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN10354478 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN14719852;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN14719852 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN81819563;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN81819563 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN72926889;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN72926889 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN86053549;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN86053549 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN57635483;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN57635483 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN33498742;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN33498742 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN46889894;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN46889894 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN64655738;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN64655738 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN54963479;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN54963479 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN13721477;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN13721477 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN24021930;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN24021930 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN42140452;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN42140452 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN15185021;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN15185021 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN80560275;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN80560275 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN69034069;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN69034069 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN74042062;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN74042062 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN31029233;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN31029233 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN85143508;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN85143508 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN57132471;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN57132471 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN44726642;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN44726642 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN4596009;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN4596009 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN18849627;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN18849627 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN53933757;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN53933757 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN99740237;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN99740237 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN4751089;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN4751089 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN40462986;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN40462986 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN75679210;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN75679210 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN47205956;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN47205956 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN51271919;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN51271919 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN57027747;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN57027747 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN34952528;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN34952528 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN454695;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN454695 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN91651538;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN91651538 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN4766797;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN4766797 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN24243016;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN24243016 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN2260803;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN2260803 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN12504435;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN12504435 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN64868115;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN64868115 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN98176527;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN98176527 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN60699033;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN60699033 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN40020762;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN40020762 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN57039577;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN57039577 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN85566346;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN85566346 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN84158325;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN84158325 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN73827702;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN73827702 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN65835225;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN65835225 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN48550431;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN48550431 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN78399947;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN78399947 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN65114637;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN65114637 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN87059380;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN87059380 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN31629873;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN31629873 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN26111934;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN26111934 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN3880215;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN3880215 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN88496863;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN88496863 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN5758469;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN5758469 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN95870224;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN95870224 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN27885806;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN27885806 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN73186651;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN73186651 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN81302461;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN81302461 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN17172498;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN17172498 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN57819532;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN57819532 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN99683937;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN99683937 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN13383820;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN13383820 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN97935732;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN97935732 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN78768949;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN78768949 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN23567235;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN23567235 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN50488913;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN50488913 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN10418225;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN10418225 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN56317260;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN56317260 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN66773267;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN66773267 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN61537627;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN61537627 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN66161118;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN66161118 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN86966980;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN86966980 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN96433437;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN96433437 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN4705880;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN4705880 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN47556431;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN47556431 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN33283281;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN33283281 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN69775431;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN69775431 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN25912535;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN25912535 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN38915864;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN38915864 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN91912554;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN91912554 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN97279263;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN97279263 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN82091318;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN82091318 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN64212538;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN64212538 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN25397874;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN25397874 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN8840594;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN8840594 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN96574480;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN96574480 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN3154675;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN3154675 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN99008327;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN99008327 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN28372791;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN28372791 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN74374996;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN74374996 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN39317784;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN39317784 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN83565654;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN83565654 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN81004030;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN81004030 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN2879502;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN2879502 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN40336825;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN40336825 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN43655758;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN43655758 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN87630614;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN87630614 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN5389377;     RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN5389377 = RZPULhMkonSkfzqKCHcRepXYQoXmqCmQqdSnHRAfN10354478;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void yZMcLuOHLNfvgjVbFmiEaUdyuQdHQqUOusKrxNIUjCfpmuGRUwCdXGMdvOhvRGqUtQAnYddsdOUXpnMAKLnU2486152() {     float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM86827860 = -839139844;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM92471432 = -714651799;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM38936086 = -441712172;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM68276906 = -328247452;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM25059603 = -802108751;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM13230913 = -709671115;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM67893698 = 52677273;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM97534651 = -930243252;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM90323870 = -257557280;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM97198446 = -595722279;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM73409343 = -520627901;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM10114365 = -468888492;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM54873166 = -163189211;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM52040029 = -14741624;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM28628199 = -349067406;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM94054605 = -411064558;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM95249264 = -3288883;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM22651390 = -440302387;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM51215468 = -27054249;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM91913550 = -244706990;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM75890026 = -386183133;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM9398709 = -645270913;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM83230061 = -301573073;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM14464802 = -924091084;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM33644866 = -639768184;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM93854928 = -268616292;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM600305 = -987629902;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM82209798 = -268433481;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM41339769 = -754633496;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM79569340 = -475594559;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM76604880 = -340087283;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM77197392 = -240388831;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM36156247 = -347914279;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM58987967 = -136034701;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM61284185 = -665166081;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM2560612 = -493029382;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM76018230 = -908286588;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM64378376 = -625940770;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM93675008 = -39445159;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM20770015 = 55025194;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM64962114 = -381441054;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM86046421 = -625242164;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM75548839 = -183632521;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM22271554 = 52032541;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM66067226 = -556966734;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM88581767 = -504599867;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM21825945 = -169794379;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM41509283 = -558048696;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM87136000 = -963758921;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM36026488 = -399401963;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM57264424 = -24650761;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM24674315 = -131330679;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM36786976 = -160208354;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM94446229 = -636835151;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM26596125 = -845408832;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM77429151 = -93868932;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM9241372 = -313078726;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM24471284 = -517621089;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM34632041 = -688479268;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM31204674 = -433492460;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM12630608 = -722041214;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM85683899 = -678889246;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM56194882 = -75609757;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM10754530 = -781962721;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM20593567 = -155634997;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM96211950 = -180239070;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM73958117 = -20974213;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM95885199 = 72845489;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM90755844 = -349575543;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM26067587 = -856038025;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM18036375 = -502777970;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM30870889 = -377348114;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM28976381 = -300857229;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM30445453 = 17920556;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM26951436 = -863265936;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM89843604 = -760940970;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM33849870 = -361638393;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM60958507 = -253605615;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM48397576 = -267124351;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM45063098 = -35168317;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM72028983 = 1178087;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM59091022 = -329581207;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM95073798 = -304674561;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM5313282 = -255231533;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM22304917 = -350943799;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM51930565 = -108756604;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM40410417 = 19819522;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM41710018 = -711079128;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM32391842 = -290625869;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM83855033 = -471297150;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM93319240 = -79950656;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM51546946 = -290665499;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM29746335 = -937461502;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM62470334 = -605952699;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM8139408 = -222933593;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM79278214 = -702551809;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM29851540 = -449632407;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM64794309 = -401669800;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM1677988 = -792332462;    float orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM69855276 = -839139844;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM86827860 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM92471432;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM92471432 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM38936086;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM38936086 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM68276906;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM68276906 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM25059603;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM25059603 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM13230913;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM13230913 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM67893698;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM67893698 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM97534651;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM97534651 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM90323870;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM90323870 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM97198446;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM97198446 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM73409343;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM73409343 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM10114365;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM10114365 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM54873166;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM54873166 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM52040029;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM52040029 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM28628199;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM28628199 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM94054605;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM94054605 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM95249264;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM95249264 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM22651390;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM22651390 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM51215468;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM51215468 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM91913550;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM91913550 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM75890026;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM75890026 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM9398709;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM9398709 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM83230061;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM83230061 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM14464802;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM14464802 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM33644866;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM33644866 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM93854928;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM93854928 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM600305;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM600305 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM82209798;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM82209798 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM41339769;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM41339769 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM79569340;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM79569340 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM76604880;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM76604880 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM77197392;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM77197392 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM36156247;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM36156247 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM58987967;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM58987967 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM61284185;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM61284185 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM2560612;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM2560612 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM76018230;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM76018230 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM64378376;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM64378376 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM93675008;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM93675008 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM20770015;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM20770015 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM64962114;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM64962114 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM86046421;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM86046421 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM75548839;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM75548839 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM22271554;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM22271554 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM66067226;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM66067226 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM88581767;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM88581767 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM21825945;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM21825945 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM41509283;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM41509283 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM87136000;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM87136000 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM36026488;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM36026488 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM57264424;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM57264424 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM24674315;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM24674315 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM36786976;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM36786976 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM94446229;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM94446229 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM26596125;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM26596125 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM77429151;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM77429151 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM9241372;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM9241372 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM24471284;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM24471284 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM34632041;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM34632041 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM31204674;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM31204674 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM12630608;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM12630608 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM85683899;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM85683899 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM56194882;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM56194882 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM10754530;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM10754530 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM20593567;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM20593567 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM96211950;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM96211950 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM73958117;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM73958117 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM95885199;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM95885199 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM90755844;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM90755844 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM26067587;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM26067587 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM18036375;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM18036375 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM30870889;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM30870889 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM28976381;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM28976381 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM30445453;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM30445453 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM26951436;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM26951436 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM89843604;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM89843604 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM33849870;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM33849870 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM60958507;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM60958507 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM48397576;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM48397576 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM45063098;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM45063098 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM72028983;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM72028983 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM59091022;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM59091022 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM95073798;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM95073798 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM5313282;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM5313282 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM22304917;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM22304917 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM51930565;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM51930565 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM40410417;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM40410417 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM41710018;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM41710018 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM32391842;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM32391842 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM83855033;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM83855033 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM93319240;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM93319240 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM51546946;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM51546946 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM29746335;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM29746335 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM62470334;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM62470334 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM8139408;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM8139408 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM79278214;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM79278214 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM29851540;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM29851540 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM64794309;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM64794309 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM1677988;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM1677988 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM69855276;     orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM69855276 = orrofgvjfNDcMLwVtTsqSJAJlpoHBhkFSWfCYLHsiuMPKrKdrqGqSxBeriDqsnhxcxKqftxiLAIYxkytfKQLbtnVVAKqIRAuM86827860;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void xyxLipissmIKjRcECKrjpkGvLSRoEIOmLwnfvuvUtUEKkWRbIemRAnqNrCAPXtnkXWzndxIIOWpUgwKpBpgZ6534632() {     long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ34514571 = -301744661;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ31062387 = -56998086;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ9720143 = -366173703;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ79729684 = -351436617;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ27975951 = -217362652;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ67093621 = -100444293;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ68076372 = -874514883;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ28400045 = -852402520;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ3866631 = 4399913;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ91199991 = -555167103;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ52132264 = -36982874;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ88620274 = -957300020;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ97378962 = -516642368;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ25312466 = -300735219;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ23479678 = 3870412;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ35171240 = -325670902;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ94609966 = -482968231;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ39518420 = -241847359;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ95853751 = -673138132;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ18924861 = -897098346;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ45752700 = -564899102;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ64158547 = 42241477;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ61815421 = -991198073;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ19892604 = -660446186;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ57504806 = -736839289;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ70570527 = -722709677;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ28162017 = -896038162;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ84555999 = -79030545;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ32950942 = -219564037;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ98423894 = -920474838;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ13085890 = -5802880;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ94965512 = -924075641;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ53248578 = -482681216;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ78680359 = -680493777;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ32399981 = 46953530;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ34401472 = -115412667;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ99895284 = -779907745;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ53691884 = -146871046;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ6599674 = -390849857;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ31918316 = 33452516;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ97607837 = -369131655;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ25410124 = -953864114;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ81349534 = -457552730;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ96450270 = -147950188;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ74006667 = -180533606;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ98849454 = -326000422;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ4426051 = -272612437;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ50533057 = -253127711;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ48038514 = -362733663;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ6424807 = -651475276;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ22695875 = -262069668;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ44326484 = -328581418;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ77353388 = -491887088;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ12626107 = -901203792;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ60781931 = -149861826;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ70356024 = -243986139;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ69246966 = -65800013;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ89827539 = -705727518;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ22224878 = -614597329;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ57405424 = -494652975;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ38931605 = -204406131;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ83520373 = -695484338;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ95449103 = -532838483;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ5442736 = -75125249;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ78114101 = -449364223;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ57166751 = -112907233;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ35371697 = -374618805;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ18698604 = -836148591;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ92912485 = -247688749;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ89078206 = -880716921;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ35275956 = -545763157;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ40918083 = -236097185;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ32918746 = -850997502;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ63935435 = -606590649;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ21317024 = -427966691;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ20342577 = -611034989;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ82809012 = -500205793;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ65365150 = -743247885;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ45885936 = -379912581;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ58655352 = -310838868;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ66144477 = -350097241;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ77628959 = -542910451;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ36517485 = -716296883;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ26526136 = -568088762;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ75728020 = -558405171;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ68759406 = -677221463;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ17612125 = -332188554;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ40622471 = -581477424;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ17898428 = -430631952;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ62043956 = -709060332;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ65154505 = 50387345;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ10067745 = 25819772;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ31467007 = -532273718;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ49194250 = -896196883;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ92986711 = -762141353;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ14087464 = -673647318;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ29961021 = -321025631;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ75906798 = -282427482;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ18336170 = -698585966;    long VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ16839916 = -301744661;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ34514571 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ31062387;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ31062387 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ9720143;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ9720143 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ79729684;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ79729684 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ27975951;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ27975951 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ67093621;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ67093621 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ68076372;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ68076372 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ28400045;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ28400045 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ3866631;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ3866631 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ91199991;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ91199991 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ52132264;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ52132264 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ88620274;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ88620274 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ97378962;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ97378962 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ25312466;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ25312466 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ23479678;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ23479678 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ35171240;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ35171240 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ94609966;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ94609966 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ39518420;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ39518420 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ95853751;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ95853751 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ18924861;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ18924861 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ45752700;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ45752700 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ64158547;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ64158547 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ61815421;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ61815421 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ19892604;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ19892604 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ57504806;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ57504806 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ70570527;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ70570527 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ28162017;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ28162017 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ84555999;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ84555999 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ32950942;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ32950942 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ98423894;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ98423894 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ13085890;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ13085890 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ94965512;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ94965512 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ53248578;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ53248578 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ78680359;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ78680359 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ32399981;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ32399981 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ34401472;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ34401472 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ99895284;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ99895284 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ53691884;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ53691884 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ6599674;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ6599674 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ31918316;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ31918316 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ97607837;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ97607837 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ25410124;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ25410124 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ81349534;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ81349534 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ96450270;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ96450270 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ74006667;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ74006667 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ98849454;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ98849454 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ4426051;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ4426051 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ50533057;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ50533057 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ48038514;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ48038514 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ6424807;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ6424807 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ22695875;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ22695875 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ44326484;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ44326484 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ77353388;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ77353388 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ12626107;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ12626107 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ60781931;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ60781931 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ70356024;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ70356024 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ69246966;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ69246966 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ89827539;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ89827539 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ22224878;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ22224878 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ57405424;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ57405424 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ38931605;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ38931605 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ83520373;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ83520373 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ95449103;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ95449103 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ5442736;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ5442736 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ78114101;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ78114101 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ57166751;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ57166751 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ35371697;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ35371697 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ18698604;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ18698604 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ92912485;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ92912485 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ89078206;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ89078206 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ35275956;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ35275956 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ40918083;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ40918083 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ32918746;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ32918746 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ63935435;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ63935435 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ21317024;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ21317024 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ20342577;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ20342577 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ82809012;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ82809012 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ65365150;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ65365150 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ45885936;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ45885936 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ58655352;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ58655352 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ66144477;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ66144477 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ77628959;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ77628959 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ36517485;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ36517485 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ26526136;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ26526136 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ75728020;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ75728020 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ68759406;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ68759406 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ17612125;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ17612125 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ40622471;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ40622471 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ17898428;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ17898428 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ62043956;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ62043956 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ65154505;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ65154505 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ10067745;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ10067745 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ31467007;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ31467007 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ49194250;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ49194250 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ92986711;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ92986711 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ14087464;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ14087464 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ29961021;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ29961021 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ75906798;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ75906798 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ18336170;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ18336170 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ16839916;     VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ16839916 = VxlYFanstfxzhUmOWexLVeDdxWgusLOhNNOalahDJQOiZpnLJwSXrUEqtHbjtsAiRPmTFMcwwuCFQUEHiVlwtrptJXgdzxJQQ34514571;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void IzIHBrONYUGsftQOMPjmBqqslFIUivNSnPVOjFBBGnQyfLPKoXGgIxhSIQVvjtfdGoEPYBfHOigNZsMaqygLohJqhD31848821() {     double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf53132161 = -951854998;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf19052864 = -175257348;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf52650728 = -807712157;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf65469449 = -375846259;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf25782634 = -7103617;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf23791210 = -617047637;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf52479187 = -982085572;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf45100459 = -654675466;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf54964273 = -183013570;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf21727933 = -570372181;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf66577443 = -338409164;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf39679128 = -602996385;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf52648222 = -830803586;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf97178188 = -949149532;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf44375971 = -724616132;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf94241383 = 53690841;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf93937020 = -177367545;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf9904768 = -496105232;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf736155 = -716384322;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf89463083 = -78562931;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf82450251 = -637231710;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf69168902 = -623534954;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf7694748 = -38171757;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf25606078 = -556609452;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf40515271 = -28493109;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf35534317 = -737544812;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf36121714 = -915415277;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf97551999 = -111237993;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf92541649 = -640543523;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf76165531 = -983506711;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf19908005 = -1292983;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf18931954 = -370061751;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf23872085 = -508751669;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf57303930 = -500977016;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf70416607 = -824499505;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf83707640 = -7395035;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf98713235 = -181614227;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf5600840 = -742587133;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf9678270 = -818644251;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf27863895 = -799781881;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf21445441 = -819332288;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf72108757 = -83992440;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf45350265 = -919574000;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf32427868 = -242668851;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf13942921 = -710603996;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf20183862 = -601158862;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf70320897 = -380841972;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf44242294 = -163737199;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf17409581 = 38345606;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf85791456 = -511552458;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf23150033 = 9068324;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf1855083 = -188845346;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf4265400 = -551548921;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf42289136 = -484749730;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf49398569 = -633496546;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf83963258 = -228320045;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf11358117 = -37085591;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf27044650 = -151102706;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf24954179 = -247353150;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf90248317 = -269558806;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf87669496 = -701632360;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf54927188 = -770847580;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf52558809 = 85868057;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf78798742 = -199506860;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf1819929 = -469079198;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf47645490 = -968347413;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf15807044 = 5755284;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf95344292 = -229826570;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf26761581 = -24650027;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf60668331 = -617221097;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf95528148 = -764694932;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf88336181 = -434780412;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf226499 = -677460981;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf72872260 = -916602442;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf68017643 = -259230644;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf10341495 = -453239271;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf23818637 = -703960955;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf75266879 = -795502907;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf11663158 = -846005456;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf20331409 = -427334248;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf65213420 = -256702840;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf91879419 = -651678079;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf80142419 = -49583600;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf6750194 = -28991066;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf53015498 = -892575036;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf18052922 = -812447638;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf14666554 = -818512831;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf81582948 = 75998061;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf7905361 = -867480471;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf86453349 = -496179461;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf72349523 = -970309445;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf71668586 = 69488478;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf80646661 = -395233984;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf19429953 = -449085446;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf40194399 = 1850479;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf66518253 = 51515291;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf19549948 = -69860497;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf66551523 = -620067141;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf30607940 = -773589653;    double GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf66297431 = -951854998;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf53132161 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf19052864;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf19052864 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf52650728;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf52650728 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf65469449;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf65469449 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf25782634;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf25782634 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf23791210;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf23791210 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf52479187;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf52479187 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf45100459;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf45100459 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf54964273;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf54964273 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf21727933;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf21727933 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf66577443;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf66577443 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf39679128;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf39679128 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf52648222;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf52648222 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf97178188;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf97178188 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf44375971;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf44375971 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf94241383;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf94241383 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf93937020;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf93937020 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf9904768;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf9904768 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf736155;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf736155 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf89463083;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf89463083 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf82450251;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf82450251 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf69168902;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf69168902 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf7694748;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf7694748 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf25606078;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf25606078 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf40515271;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf40515271 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf35534317;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf35534317 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf36121714;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf36121714 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf97551999;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf97551999 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf92541649;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf92541649 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf76165531;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf76165531 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf19908005;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf19908005 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf18931954;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf18931954 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf23872085;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf23872085 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf57303930;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf57303930 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf70416607;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf70416607 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf83707640;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf83707640 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf98713235;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf98713235 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf5600840;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf5600840 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf9678270;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf9678270 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf27863895;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf27863895 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf21445441;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf21445441 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf72108757;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf72108757 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf45350265;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf45350265 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf32427868;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf32427868 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf13942921;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf13942921 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf20183862;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf20183862 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf70320897;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf70320897 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf44242294;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf44242294 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf17409581;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf17409581 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf85791456;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf85791456 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf23150033;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf23150033 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf1855083;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf1855083 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf4265400;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf4265400 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf42289136;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf42289136 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf49398569;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf49398569 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf83963258;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf83963258 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf11358117;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf11358117 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf27044650;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf27044650 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf24954179;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf24954179 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf90248317;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf90248317 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf87669496;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf87669496 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf54927188;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf54927188 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf52558809;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf52558809 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf78798742;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf78798742 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf1819929;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf1819929 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf47645490;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf47645490 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf15807044;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf15807044 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf95344292;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf95344292 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf26761581;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf26761581 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf60668331;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf60668331 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf95528148;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf95528148 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf88336181;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf88336181 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf226499;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf226499 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf72872260;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf72872260 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf68017643;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf68017643 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf10341495;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf10341495 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf23818637;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf23818637 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf75266879;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf75266879 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf11663158;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf11663158 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf20331409;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf20331409 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf65213420;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf65213420 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf91879419;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf91879419 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf80142419;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf80142419 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf6750194;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf6750194 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf53015498;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf53015498 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf18052922;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf18052922 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf14666554;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf14666554 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf81582948;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf81582948 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf7905361;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf7905361 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf86453349;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf86453349 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf72349523;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf72349523 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf71668586;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf71668586 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf80646661;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf80646661 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf19429953;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf19429953 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf40194399;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf40194399 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf66518253;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf66518253 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf19549948;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf19549948 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf66551523;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf66551523 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf30607940;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf30607940 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf66297431;     GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf66297431 = GTjFSNInrrGzlmsySaRUsQDvFkMUoZeOiVvMYIPSFmAVTrMtTpOjZNKqPnAiAoySNsQQrf53132161;}
// Junk Finished
