#include "Visuals.hpp"
#include "../Options.hpp"
#include "../Structs.hpp"
#include "../helpers/Math.hpp"
#include "LagCompensation.hpp"
#include "..//SDK.hpp"
#include "..//features/AimRage.hpp"
#include <comdef.h>
#include <minwindef.h>
#include "..//HanaLovesMe/HanaLovesMe.h"
#include <time.h>
#include "..//Install.hpp"
#include "..//Singleton.hpp"
#include "..//helpers/AntiCheatScan.h"
#include "..//HNJ.h"
#include "..//misc/dlight.h"

#pragma warning (disable: 4101) // condition expression is constant
#pragma warning (disable: 4838) // condition expression is constant

#define FLAG_MACRO std::pair<std::string, Color> // :joy:
#define FLAG(string, color) vecFlags.push_back(FLAG_MACRO(string, color)) //coz, why not

extern CEEffects* g_Effects;

namespace Visuals
{
	vgui::HFont weapon_font;
	vgui::HFont ui_font;
	vgui::HFont ESPFont;
	vgui::HFont watermark_font;
	vgui::HFont aainfo_font;
	vgui::HFont aa_info;
	vgui::HFont spectatorlist_font;
	vgui::HFont eventlog_font;
	vgui::HFont indicators_font;
	vgui::HFont visuals_font;
	VisualsStruct ESP_ctx;
	float ESP_Fade[64];
	C_BasePlayer* local_observed;
	C_BasePlayer* carrier;
}

RECT GetBBox(C_BaseEntity* ent, Vector pointstransf[])
{
	RECT rect{};
	auto collideable = ent->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	const matrix3x4_t& trans = ent->m_rgflCoordinateFrame();

	Vector points[] =
	{
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++) {
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector pos = ent->m_vecOrigin();
	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++)
		if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
			return rect;
		else
			pointstransf[i] = screen_points[i];

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++)
	{
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top < screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}
	return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}

void Visuals::RenderNadeEsp(C_BaseCombatWeapon* nade)
{
	if (!g_Options.esp_grenades)
		return;

	const model_t* model = nade->GetModel();
	if (!model)
		return;

	studiohdr_t* hdr = g_MdlInfo->GetStudiomodel(model);
	if (!hdr)
		return;

	Color Nadecolor;
	std::string entityName = hdr->szName, icon_character;
	switch (nade->GetClientClass()->m_ClassID)
	{
	case 9:
		if (entityName[16] == 's')
		{
			Nadecolor = Color(255, 255, 0, 200);
			entityName = "Flash";
			icon_character = "G";
		}
		else
		{
			Nadecolor = Color(255, 0, 0, 200);
			entityName = "Frag";
			icon_character = "H";
		}
		break;
	case 134:
		Nadecolor = Color(170, 170, 170, 200);
		entityName = "Smoke";
		icon_character = "P";
		break;
	case 98:
		Nadecolor = Color(255, 0, 0, 200);
		entityName = "Fire";
		icon_character = "P";
		break;
	case 41:
		Nadecolor = Color(255, 255, 0, 200);
		entityName = "Decoy";
		icon_character = "G";
		break;
	default:
		return;
	}

	Vector points_transformed[8];
	RECT size = GetBBox(nade, points_transformed);
	if (size.right == 0 || size.bottom == 0)
		return;

	int width, height, width_icon, height_icon;
	Visuals::GetTextSize(ESPFont, entityName.c_str(), width, height);
	Visuals::GetTextSize(weapon_font, icon_character.c_str(), width_icon, height_icon);

	// + distance? just make it customizable
	switch (g_Options.esp_grenades_type)
	{
	case 1:
		g_VGuiSurface->DrawSetColor(Color(20, 20, 20, 240));
		g_VGuiSurface->DrawLine(size.left - 1, size.bottom - 1, size.left - 1, size.top + 1);
		g_VGuiSurface->DrawLine(size.right + 1, size.top + 1, size.right + 1, size.bottom - 1);
		g_VGuiSurface->DrawLine(size.left - 1, size.top + 1, size.right + 1, size.top + 1);
		g_VGuiSurface->DrawLine(size.right + 1, size.bottom - 1, size.left + -1, size.bottom - 1);

		g_VGuiSurface->DrawSetColor(Nadecolor);
		g_VGuiSurface->DrawLine(size.left, size.bottom, size.left, size.top);
		g_VGuiSurface->DrawLine(size.left, size.top, size.right, size.top);
		g_VGuiSurface->DrawLine(size.right, size.top, size.right, size.bottom);
		g_VGuiSurface->DrawLine(size.right, size.bottom, size.left, size.bottom);
	case 0:
		DrawString(ESPFont, size.left + ((size.right - size.left) * 0.5), size.bottom + (size.top - size.bottom) + height * 0.5f + 2, Nadecolor, FONT_CENTER, entityName.c_str());
		break;
	case 3:
		DrawString(ESPFont, size.left + ((size.right - size.left) * 0.5), size.bottom + (size.top - size.bottom) + height_icon * 0.5f + 1, Nadecolor, FONT_CENTER, entityName.c_str());
	case 2:
		DrawString(weapon_font, size.left + ((size.right - size.left) * 0.5), size.bottom + (size.top - size.bottom), Nadecolor, FONT_CENTER, icon_character.c_str());
		break;
	}
	HN34207389::call();
}

bool Visuals::InitFont()
{
	ui_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(ui_font, "Verdana", 12, 0, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW); 	//g_VGuiSurface->SetFontGlyphSet(ui_font, "Courier New", 14, 0, 0, 0, FONTFLAG_OUTLINE); // Styles

	ESPFont = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(ESPFont, "Small Fonts", 10, 350, 0, 0, FONTFLAG_OUTLINE); // "Smallest Pixel-7"

	visuals_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(visuals_font, "Verdana", 12, 0, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW); 	//g_VGuiSurface->SetFontGlyphSet(ui_font, "Courier New", 14, 0, 0, 0, FONTFLAG_OUTLINE); // Styles

	watermark_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(watermark_font, "Tahoma", 16, 700, 0, 0, FONTFLAG_DROPSHADOW);

	aainfo_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(aainfo_font, "Tahoma", 32, 700, 0, 0, FONTFLAG_DROPSHADOW);

	aa_info = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(aa_info, "Tahoma", 32, 700, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);

	weapon_font = g_VGuiSurface->CreateFont_();// 0xA1;
	g_VGuiSurface->SetFontGlyphSet(weapon_font, "Undefeated", 16, 500, 0, 0, FONTFLAG_OUTLINE | FONTFLAG_ANTIALIAS);

	spectatorlist_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(spectatorlist_font, "Microsoft Sans Serif", 12, 500, 0, 0, FONTFLAG_OUTLINE);

	eventlog_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(eventlog_font, "Lucida Console", 10, 0, 0, 0, FONTFLAG_DROPSHADOW); // FONTFLAG_DROPSHADOW

	indicators_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(indicators_font, "Verdana", 26, 700, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);

	return true;
	HN34207389::call();
}

bool Visuals::IsVisibleScan(C_BasePlayer* player)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];
	Vector eyePos = g_LocalPlayer->GetEyePos();

	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = g_LocalPlayer;

	if (!player->SetupBones2(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f))
	{
		return false;
	}

	auto studio_model = g_MdlInfo->GetStudiomodel(player->GetModel());
	if (!studio_model)
	{
		return false;
	}

	int scan_hitboxes[] = {
		HITBOX_HEAD,
		HITBOX_LEFT_FOOT,
		HITBOX_RIGHT_FOOT,
		HITBOX_LEFT_CALF,
		HITBOX_RIGHT_CALF,
		HITBOX_CHEST,
		HITBOX_STOMACH
	};

	for (int i = 0; i < ARRAYSIZE(scan_hitboxes); i++)
	{
		auto hitbox = studio_model->pHitboxSet(player->m_nHitboxSet())->pHitbox(scan_hitboxes[i]);
		if (hitbox)
		{
			auto
				min = Vector{},
				max = Vector{};

			Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
			Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

			ray.Init(eyePos, (min + max) * 0.5);
			g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

			if (tr.hit_entity == player || tr.fraction > 0.97f)
				return true;
		}
	}
	return false;
	HN34207389::call();
}

bool Visuals::ValidPlayer(C_BasePlayer* player, bool count_step)
{
	int idx = player->EntIndex();
	constexpr float frequency = 0.7f / 0.5f;
	float step = frequency * g_GlobalVars->frametime;
	if (!player->IsAlive())
		return false;

	// Don't render esp if in firstperson viewing player.
	if (player == local_observed)
	{
		if (g_LocalPlayer->m_iObserverMode() == 4)
			return false;
	}

	if (player == g_LocalPlayer)
	{
		if (!g_Input->m_fCameraInThirdPerson)
			return false;
	}

	if (count_step)
	{
		if (!player->IsDormant()) {
			if (ESP_Fade[idx] < 1.f)
				ESP_Fade[idx] += step;
		}
		else {
			if (ESP_Fade[idx] > 0.f)
				ESP_Fade[idx] -= step;
		}
		ESP_Fade[idx] = (ESP_Fade[idx] > 1.f ? 1.f : ESP_Fade[idx] < 0.f ? 0.f : ESP_Fade[idx]);
	}

	return (ESP_Fade[idx] > 0.f);
	HN34207389::call();
}

bool Visuals::Begin(C_BasePlayer* player)
{
	ESP_ctx.player = player;
	ESP_ctx.bEnemy = !player->IsTeamMate(); // not sure hmmmmm, still tired // g_LocalPlayer->m_iTeamNum() != player->m_iTeamNum();
	ESP_ctx.isVisible = Visuals::IsVisibleScan(player);
	local_observed = (C_BasePlayer*)g_EntityList->GetClientEntityFromHandle(g_LocalPlayer->m_hObserverTarget());

	int idx = player->EntIndex();
	bool playerTeam = player->m_iTeamNum() == 2;

	if (!ESP_ctx.bEnemy && g_Options.esp_enemies_only)
		return false;

	if (!player->m_bGunGameImmunity())
	{
		if (ESP_ctx.isVisible)
		{
			ESP_ctx.clr_fill.SetColor(playerTeam ? g_Options.esp_player_fill_color_t_visible : g_Options.esp_player_fill_color_ct_visible);
			ESP_ctx.clr.SetColor(playerTeam ? g_Options.esp_player_bbox_color_t_visible : g_Options.esp_player_bbox_color_ct_visible);
		}
		else
		{
			ESP_ctx.clr_fill.SetColor(playerTeam ? g_Options.esp_player_fill_color_t : g_Options.esp_player_fill_color_ct);
			ESP_ctx.clr.SetColor(playerTeam ? g_Options.esp_player_bbox_color_t : g_Options.esp_player_bbox_color_ct);
			ESP_ctx.clr.SetAlpha(255);
		}
		ESP_ctx.clr.SetAlpha(ESP_ctx.clr.a() * ESP_Fade[idx]);
		ESP_ctx.clr_fill.SetAlpha(g_Options.esp_fill_amount * ESP_Fade[idx]);
		ESP_ctx.clr_text = Color(245, 245, 245, (int)(ESP_ctx.clr.a() * ESP_Fade[idx]));
	}
	else
	{
		// Set all colors to grey if immune.
		ESP_ctx.clr.SetColor(166, 169, 174, (int)(225 * ESP_Fade[idx]));
		ESP_ctx.clr_text.SetColor(166, 169, 174, (int)(225 * ESP_Fade[idx]));
		ESP_ctx.clr_fill.SetColor(166, 169, 174, (int)(g_Options.esp_fill_amount * ESP_Fade[idx]));
	}

	// Do some touch ups if local player and scoped
	if (player == g_LocalPlayer && g_LocalPlayer->m_bIsScoped())
	{
		ESP_ctx.clr.SetAlpha(ESP_ctx.clr.a() * 0.1f);
		ESP_ctx.clr_text.SetAlpha(ESP_ctx.clr_text.a() * 0.1f);
		ESP_ctx.clr_fill.SetAlpha(ESP_ctx.clr_fill.a() * 0.1f);
	}

	Vector head = player->GetAbsOrigin() + Vector(0, 0, player->GetCollideable()->OBBMaxs().z);
	Vector origin = player->GetAbsOrigin();
	origin.z -= 5;

	if (g_Options.visuals_others_dlight)
	{
		int temp1 = ESP_ctx.clr.r(), temp2 = ESP_ctx.clr.g(), temp3 = ESP_ctx.clr.b();
		dlight_t* dLight = g_Effects->CL_AllocDlight(player->EntIndex());
		dLight->key = player->EntIndex();
		dLight->color.r = (unsigned char)temp1;
		dLight->color.g = (unsigned char)temp2;
		dLight->color.b = (unsigned char)temp3;
		dLight->color.exponent = true;
		dLight->flags = DLIGHT_NO_MODEL_ILLUMINATION;
		dLight->m_Direction = player->m_vecOrigin();
		dLight->origin = player->m_vecOrigin();

		if (player->IsWeapon()) dLight->radius = 250.f;
		else if (player->IsPlayer()) dLight->radius = 750.f;
		else dLight->radius = 500.f;

		dLight->die = g_GlobalVars->curtime + 0.5f;
		dLight->decay = 20.0f;
	}

	if (!Math::WorldToScreen(head, ESP_ctx.head_pos) ||
		!Math::WorldToScreen(origin, ESP_ctx.feet_pos))
		return false;

	auto h = fabs(ESP_ctx.head_pos.y - ESP_ctx.feet_pos.y);
	auto w = h / 2.1f;

	switch (g_Options.esp_player_boundstype)
	{
	case 0:
		ESP_ctx.bbox.left = static_cast<long>(ESP_ctx.feet_pos.x - w * 0.46f);
		ESP_ctx.bbox.right = static_cast<long>(ESP_ctx.bbox.left + w);
		ESP_ctx.bbox.bottom = (ESP_ctx.feet_pos.y > ESP_ctx.head_pos.y ? static_cast<long>(ESP_ctx.feet_pos.y) : static_cast<long>(ESP_ctx.head_pos.y));
		ESP_ctx.bbox.top = (ESP_ctx.feet_pos.y > ESP_ctx.head_pos.y ? static_cast<long>(ESP_ctx.head_pos.y) : static_cast<long>(ESP_ctx.feet_pos.y));
		break;
	case 1:
	{
		Vector points_transformed[8];
		RECT BBox = GetBBox(player, points_transformed);
		ESP_ctx.bbox = BBox;
		ESP_ctx.bbox.top = BBox.bottom;
		ESP_ctx.bbox.bottom = BBox.top;
		break;
	}
	}

	return true;
	HN34207389::call();
}

void Visuals::RenderFill()
{
	g_VGuiSurface->DrawSetColor(ESP_ctx.clr_fill);
	g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left + 2, ESP_ctx.bbox.top + 2, ESP_ctx.bbox.right - 2, ESP_ctx.bbox.bottom - 2);
	HN34207389::call();
}

void Visuals::RenderBox()
{
	float
		length_horizontal = (ESP_ctx.bbox.right - ESP_ctx.bbox.left) * 0.2f,
		length_vertical = (ESP_ctx.bbox.bottom - ESP_ctx.bbox.top) * 0.2f;

	Color col_black = Color(0, 0, 0, (int)(255.f * ESP_Fade[ESP_ctx.player->EntIndex()]));
	switch (g_Options.esp_player_boxtype)
	{
	case 0:
		break;

	case 1:
		g_VGuiSurface->DrawSetColor(ESP_ctx.clr);
		g_VGuiSurface->DrawOutlinedRect(ESP_ctx.bbox.left, ESP_ctx.bbox.top, ESP_ctx.bbox.right, ESP_ctx.bbox.bottom);
		g_VGuiSurface->DrawSetColor(col_black);
		g_VGuiSurface->DrawOutlinedRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.top - 1, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.bottom + 1);
		g_VGuiSurface->DrawOutlinedRect(ESP_ctx.bbox.left + 1, ESP_ctx.bbox.top + 1, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.bottom - 1);
		break;

	case 2:
		g_VGuiSurface->DrawSetColor(col_black);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.top - 1, ESP_ctx.bbox.left + 1 + length_horizontal, ESP_ctx.bbox.top + 2);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.right - 1 - length_horizontal, ESP_ctx.bbox.top - 1, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.top + 2);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.bottom - 2, ESP_ctx.bbox.left + 1 + length_horizontal, ESP_ctx.bbox.bottom + 1);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.right - 1 - length_horizontal, ESP_ctx.bbox.bottom - 2, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.bottom + 1);

		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.top + 2, ESP_ctx.bbox.left + 2, ESP_ctx.bbox.top + 1 + length_vertical);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.right - 2, ESP_ctx.bbox.top + 2, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.top + 1 + length_vertical);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.bottom - 1 - length_vertical, ESP_ctx.bbox.left + 2, ESP_ctx.bbox.bottom - 2);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.right - 2, ESP_ctx.bbox.bottom - 1 - length_vertical, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.bottom - 2);

		g_VGuiSurface->DrawSetColor(ESP_ctx.clr);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.left, ESP_ctx.bbox.top, ESP_ctx.bbox.left + length_horizontal - 1, ESP_ctx.bbox.top);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.right - length_horizontal, ESP_ctx.bbox.top, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.top);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.left, ESP_ctx.bbox.bottom - 1, ESP_ctx.bbox.left + length_horizontal - 1, ESP_ctx.bbox.bottom - 1);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.right - length_horizontal, ESP_ctx.bbox.bottom - 1, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.bottom - 1);

		g_VGuiSurface->DrawLine(ESP_ctx.bbox.left, ESP_ctx.bbox.top, ESP_ctx.bbox.left, ESP_ctx.bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.right - 1, ESP_ctx.bbox.top, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.left, ESP_ctx.bbox.bottom - length_vertical, ESP_ctx.bbox.left, ESP_ctx.bbox.bottom - 1);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.right - 1, ESP_ctx.bbox.bottom - length_vertical, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.bottom - 1);
		break;
	}
	HN34207389::call();
}

void Visuals::GayPornYes()
{
	if (!g_LocalPlayer->IsAlive())
		return;

	g_VGuiSurface->DrawSetColor(Color(g_Options.xhair_color[0], g_Options.xhair_color[1], g_Options.xhair_color[2]));
	g_VGuiSurface->DrawLine(Global::iScreenX / 2 - g_Options.visuals_xhair_x, Global::iScreenY / 2, Global::iScreenX / 2 + g_Options.visuals_xhair_x, Global::iScreenY / 2);
	g_VGuiSurface->DrawLine(Global::iScreenX / 2, Global::iScreenY / 2 - g_Options.visuals_xhair_y, Global::iScreenX / 2, Global::iScreenY / 2 + g_Options.visuals_xhair_y);
	HN34207389::call();
}

void Visuals::NightMode()
{
	static std::string old_Skyname = "";
	static bool OldNightmode;
	static int OldSky;
	if (!g_LocalPlayer || !g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
	{
		old_Skyname = "";
		OldNightmode = false;
		OldSky = 0;
		return;
	}
	static ConVar* r_DrawSpecificStaticProp;
	if (OldNightmode != g_Options.visuals_nightmode)
	{

		if (!r_DrawSpecificStaticProp)
			r_DrawSpecificStaticProp = g_CVar->FindVar("r_DrawSpecificStaticProp");
		r_DrawSpecificStaticProp->SetValue(0);
		for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
		{
			IMaterial* pMaterial = g_MatSystem->GetMaterial(i);
			if (!pMaterial)
				continue;

			if (strstr(pMaterial->GetTextureGroupName(), "World") || strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
			{
				if (g_Options.visuals_nightmode)
					if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
						pMaterial->ColorModulate(g_Options.visuals_others_nightmode_color[0], g_Options.visuals_others_nightmode_color[1], g_Options.visuals_others_nightmode_color[2]);
					else
						pMaterial->ColorModulate(g_Options.visuals_others_nightmode_color[0], g_Options.visuals_others_nightmode_color[1], g_Options.visuals_others_nightmode_color[2]);
				else
					pMaterial->ColorModulate(1.0f, 1.0f, 1.0f);
			}

			if (strstr(pMaterial->GetName(), "models/props"))
			{
				if (g_Options.visuals_nightmode)
					pMaterial->ColorModulate(g_Options.visuals_others_nightmode_color[0], g_Options.visuals_others_nightmode_color[1], g_Options.visuals_others_nightmode_color[2]);
				else
					pMaterial->ColorModulate(1.0f, 1.0f, 1.0f);
			}

			if (strstr(pMaterial->GetTextureGroupName(), ("SkyBox")))
			{
				if (g_Options.visuals_nightmode)
					pMaterial->ColorModulate(g_Options.visuals_others_skybox_color[0], g_Options.visuals_others_skybox_color[1], g_Options.visuals_others_skybox_color[2]);
				else
					pMaterial->ColorModulate(1.f, 1.f, 1.f);
			}
		}
		OldNightmode = g_Options.visuals_nightmode;
	}
	HN34207389::call();
}

void Visuals::SpoofZeug()
{
	static auto cl_phys_timescale = g_CVar->FindVar("cl_phys_timescale");
	if (g_Options.cl_phys_timescale)
		cl_phys_timescale->SetValue(g_Options.cl_phys_timescale_value);
	else cl_phys_timescale->SetValue("1.0");

	static auto mp_radar_showall = g_CVar->FindVar("mp_radar_showall");
	if (g_Options.mp_radar_showall)
		mp_radar_showall->SetValue(1);
	else mp_radar_showall->SetValue(0);

	static auto cl_crosshair_recoil = g_CVar->FindVar("cl_crosshair_recoil");
	if (g_Options.cl_crosshair_recoil)
		cl_crosshair_recoil->SetValue(1);
	else cl_crosshair_recoil->SetValue(0);

	static auto fog_override = g_CVar->FindVar("fog_override");
	static auto fog_enable = g_CVar->FindVar("fog_enable");
	if (g_Options.fog_override) {
		fog_enable->SetValue(0);
		fog_override->SetValue(1);
	}
	else {
		fog_enable->SetValue(1);
		fog_override->SetValue(0);
	}

	static auto viewmodel_offset_x = g_CVar->FindVar("viewmodel_offset_x");
	static auto viewmodel_offset_y = g_CVar->FindVar("viewmodel_offset_y");
	static auto viewmodel_offset_z = g_CVar->FindVar("viewmodel_offset_z");
	viewmodel_offset_x->m_nFlags &= ~FCVAR_CHEAT;
	viewmodel_offset_y->m_nFlags &= ~FCVAR_CHEAT;
	viewmodel_offset_z->m_nFlags &= ~FCVAR_CHEAT;
	viewmodel_offset_x->m_fnChangeCallbacks.SetSize(0);
	viewmodel_offset_y->m_fnChangeCallbacks.SetSize(0);
	viewmodel_offset_z->m_fnChangeCallbacks.SetSize(0);
	if (g_Options.change_viewmodel_offset) {
		viewmodel_offset_x->SetValue(g_Options.viewmodel_offset_x);
		viewmodel_offset_y->SetValue(g_Options.viewmodel_offset_y);
		viewmodel_offset_z->SetValue(g_Options.viewmodel_offset_z);
	}
	else {
		viewmodel_offset_x->SetValue(2.5f);
		viewmodel_offset_y->SetValue(2);
		viewmodel_offset_z->SetValue(-2);
	}
	HN34207389::call();
}

void Visuals::AsusWalls()
{
	static std::string old_Skyname = "";
	static bool OldNightmode;
	static int OldSky;
	if (!g_LocalPlayer || !g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
	{
		old_Skyname = "";
		OldNightmode = false;
		OldSky = 0;
		return;
	}
	static ConVar* r_DrawSpecificStaticProp;
	if (OldNightmode != g_Options.visuals_asuswalls)
	{
		if (!r_DrawSpecificStaticProp)
			r_DrawSpecificStaticProp = g_CVar->FindVar("r_DrawSpecificStaticProp");
		r_DrawSpecificStaticProp->SetValue(0);
		for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
		{
			IMaterial* pMaterial = g_MatSystem->GetMaterial(i);
			if (!pMaterial)
				continue;

			if (strstr(pMaterial->GetName(), "models/props"))
			{
				if (g_Options.visuals_asuswalls)
					pMaterial->AlphaModulate(0.5f);
				else
					pMaterial->AlphaModulate(1.f);
			}
		}
		OldNightmode = g_Options.visuals_asuswalls;
	}
	HN34207389::call();
}

void Visuals::RenderName()
{
	wchar_t buf[128];
	std::string name = ESP_ctx.player->GetName(),
		s_name = (name.length() > 0 ? name : "##ERROR_empty_name");

	if (MultiByteToWideChar(CP_UTF8, 0, s_name.c_str(), -1, buf, 128) > 0)
	{
		int tw, th;
		g_VGuiSurface->GetTextSize(visuals_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(visuals_font);
		g_VGuiSurface->DrawSetTextColor(ESP_ctx.clr_text);
		g_VGuiSurface->DrawSetTextPos(ESP_ctx.bbox.left + (ESP_ctx.bbox.right - ESP_ctx.bbox.left) * 0.5 - tw * 0.5, ESP_ctx.bbox.top - th + 1);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
	}
	HN34207389::call();
}

void Visuals::DrawAngleLines()
{
	Vector src3D, dst3D, forward, src, dst;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = g_LocalPlayer;
	Math::AngleVectors(QAngle(0, g_LocalPlayer->m_flLowerBodyYawTarget(), 0), forward);
	src3D = g_LocalPlayer->m_vecOrigin();
	dst3D = src3D + (forward * 50.f);

	ray.Init(src3D, dst3D);
	g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

	if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
		return;

	g_VGuiSurface->DrawSetColor(Color::Yellow);
	g_VGuiSurface->DrawLine(src.x, src.y, dst.x, dst.y);


	Math::AngleVectors(QAngle(0, g_LocalPlayer->m_angEyeAngles().yaw, 0), forward);
	dst3D = src3D + (forward * 50.f);

	ray.Init(src3D, dst3D);
	g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

	if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
		return;

	g_VGuiSurface->DrawSetColor(Color::Red);
	g_VGuiSurface->DrawLine(src.x, src.y, dst.x, dst.y);
	HN34207389::call();
}

int getAALAngle()
{
	return static_cast<int>(g_Options.hvh_antiaim_y_desync_start_left);
}

int getAARAngle()
{
	return static_cast<int>(g_Options.hvh_antiaim_y_desync_start_right);
}

int getAABAngle()
{
	return static_cast<int>(g_Options.hvh_antiaim_y_desync_start_back);
}

int getDesyncAmount()
{
	return static_cast<int>(g_Options.hvh_antiaim_y_desync);
}

void Visuals::Indicators()
{
	int alpha;
	int centerWD, centerHD;
	int screenheight, screenwidth;
	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	centerWD = w / 2;
	centerHD = h / 2;
	screenheight = h;
	screenwidth = w;
	alpha = 255;

	if (g_Options.legit_drawfov)
	{
		g_VGuiSurface->DrawSetColor(Color(255, 255, 255, 255));
		g_VGuiSurface->DrawOutlinedCircle((centerWD), (centerHD), g_Options.legit_fov * 10, 200);
	}

	if (g_Options.indicators_enabled)
	{
		if (g_InputSystem->IsButtonDown(g_Options.hvh_aa_left_bind))
		{
			DrawString(indicators_font, 7, screenheight - 180, Color(255, 255, 255, 255), FONT_LEFT, ("LEFT"));
		}

		else if (g_InputSystem->IsButtonDown(g_Options.hvh_aa_right_bind))
		{
			DrawString(indicators_font, 7, screenheight - 180, Color(255, 255, 255, 255), FONT_LEFT, ("RIGHT"));
		}

		else if (g_InputSystem->IsButtonDown(g_Options.hvh_aa_back_bind))
		{
			DrawString(indicators_font, 7, screenheight - 180, Color(255, 255, 255, 255), FONT_LEFT, ("BACK"));
		}

		DrawString(indicators_font, 7, screenheight - 260, Color(255, 255, 255, 255), FONT_LEFT, ("L: %d "), getAALAngle());
		DrawString(indicators_font, 7, screenheight - 240, Color(255, 255, 255, 255), FONT_LEFT, ("R: %d "), getAARAngle());
		DrawString(indicators_font, 7, screenheight - 220, Color(255, 255, 255, 255), FONT_LEFT, ("B: %d "), getAABAngle());

		if (g_Options.hvh_antiaim_y_desync > 100)
		{
			DrawString(indicators_font, 7, screenheight - 140, Color(255, 0, 30, 255), FONT_LEFT, ("FAKE"));
		}
		else if (g_Options.hvh_antiaim_y_desync > 50)
		{
			DrawString(indicators_font, 7, screenheight - 140, Color(255, 150, 10, 255), FONT_LEFT, ("FAKE"));
		}
		else if (g_Options.hvh_antiaim_y_desync > 0)
		{
			DrawString(indicators_font, 7, screenheight - 140, Color(116, 154, 30, 255), FONT_LEFT, ("FAKE"));
		}

		if (g_Options.misc_fakelag_enabled)
		{
			DrawString(indicators_font, 7, screenheight - 120, Color(255, 150, 10, 255), FONT_LEFT, ("LC"));
		}

		if (g_Options.hvh_antiaim_x > 0 || g_Options.hvh_antiaim_y > 0)
		{
			DrawString(indicators_font, 7, screenheight - 100, Color(230, 230, 230, 255), FONT_LEFT, ("AA"));
		}

		if (g_Options.hvh_antiaim_y_desync > 0)
		{
			DrawString(ui_font, 48, screenheight - 104, Color(255, 255, 255, 255), FONT_LEFT, ("MAX DSN: %d"), getDesyncAmount());
		}

		if (g_Options.misc_fakelag_value > 6)
		{
			DrawString(ui_font, 48, screenheight - 95, Color(255, 0, 0, 255), FONT_LEFT, ("FL WARNING"));
		}

		if (g_InputSystem->IsButtonDown(g_Options.misc_fakeduck_keybind))
		{
			DrawString(indicators_font, 7, screenheight - 80, Color(255, 0, 30, 255), FONT_LEFT, ("FD"));
		}
	}
	HN34207389::call();
}

void Visuals::DrawCapsuleOverlay(int idx, float duration)
{
	if (idx == g_EngineClient->GetLocalPlayer())
		return;

	auto player = C_BasePlayer::GetPlayerByIndex(idx);
	if (!player)
		return;

	//if (g_Options.angrpwaccboost > 0 && CMBacktracking::Get().m_LagRecord[player->EntIndex()].empty())
	//	return;

	matrix3x4_t boneMatrix_actual[MAXSTUDIOBONES];
	if (!player->SetupBones2(boneMatrix_actual, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, player->m_flSimulationTime()))
		return;

	studiohdr_t* studioHdr = g_MdlInfo->GetStudiomodel(player->GetModel());
	if (studioHdr)
	{
		matrix3x4_t boneMatrix[MAXSTUDIOBONES];
		std::memcpy(boneMatrix, CMBacktracking::Get().current_record[player->EntIndex()].matrix, sizeof(CMBacktracking::Get().current_record[player->EntIndex()].matrix));

		mstudiohitboxset_t* set = studioHdr->pHitboxSet(player->m_nHitboxSet());
		if (set)
		{
			for (int i = 0; i < set->numhitboxes; i++)
			{
				mstudiobbox_t* hitbox = set->pHitbox(i);
				if (hitbox)
				{
					if (hitbox->m_flRadius == -1.0f)
					{
						Vector position, position_actual;
						QAngle angles, angles_actual;
						MatrixAngles(boneMatrix[hitbox->bone], angles, position);
						MatrixAngles(boneMatrix_actual[hitbox->bone], angles_actual, position_actual);

						g_DebugOverlay->AddBoxOverlay(position, hitbox->bbmin, hitbox->bbmax, angles, 255, 255, 255, 150, duration);

						if (g_Options.esp_lagcompensated_hitboxes_type == 1)
							g_DebugOverlay->AddBoxOverlay(position_actual, hitbox->bbmin, hitbox->bbmax, angles_actual, 255, 255, 255, 150, duration);
					}
					else
					{
						Vector min, max,
							min_actual, max_actual;

						Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
						Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

						Math::VectorTransform(hitbox->bbmin, boneMatrix_actual[hitbox->bone], min_actual);
						Math::VectorTransform(hitbox->bbmax, boneMatrix_actual[hitbox->bone], max_actual);

						g_DebugOverlay->AddCapsuleOverlay(min, max, hitbox->m_flRadius, 255, 255, 255, 150, duration);

						if (g_Options.esp_lagcompensated_hitboxes_type == 1)
							g_DebugOverlay->AddCapsuleOverlay(min_actual, max_actual, hitbox->m_flRadius, 255, 255, 255, 150, duration);
					}
				}
			}
		}
	}
	HN34207389::call();
}

void Visuals::RenderHealth()
{
	static int m_nLastHealth[64];
	static std::vector<int> m_pAnimateHealthArray[64];

	int health = ESP_ctx.player->m_iHealth();
	if (health > 100)
		health = 100;

	int& m_nLastHealthInt = m_nLastHealth[ESP_ctx.player->EntIndex()];
	std::vector<int>& m_pAnimateHealth = m_pAnimateHealthArray[ESP_ctx.player->EntIndex()];
	if (health != m_nLastHealthInt & health < 100)
	{
		int m_nHealthDelta = m_nLastHealthInt - health;
		m_pAnimateHealth.clear();
		m_pAnimateHealth.push_back(health + (m_nHealthDelta * 0.75f));
		m_pAnimateHealth.push_back(health + (m_nHealthDelta * 0.75f));
		m_pAnimateHealth.push_back(health + (m_nHealthDelta * 0.75f));
		m_pAnimateHealth.push_back(health + (m_nHealthDelta * 0.75f));
		m_pAnimateHealth.push_back(health + (m_nHealthDelta * 0.75f));


		m_pAnimateHealth.push_back(health + (m_nHealthDelta * 0.50f));
		m_pAnimateHealth.push_back(health + (m_nHealthDelta * 0.50f));
		m_pAnimateHealth.push_back(health + (m_nHealthDelta * 0.50f));
		m_pAnimateHealth.push_back(health + (m_nHealthDelta * 0.50f));
		m_pAnimateHealth.push_back(health + (m_nHealthDelta * 0.50f));


		m_pAnimateHealth.push_back(health + (m_nHealthDelta * 0.25f));
		m_pAnimateHealth.push_back(health + (m_nHealthDelta * 0.25f));
		m_pAnimateHealth.push_back(health + (m_nHealthDelta * 0.25f));
		m_pAnimateHealth.push_back(health + (m_nHealthDelta * 0.25f));
		m_pAnimateHealth.push_back(health + (m_nHealthDelta * 0.25f));
	}
	else
	{
		if (!m_pAnimateHealth.empty())
		{
			health = m_pAnimateHealth[0];
			m_pAnimateHealth.erase(m_pAnimateHealth.begin());
		}
	}

	float box_h = (float)fabs(ESP_ctx.bbox.bottom - ESP_ctx.bbox.top);
	float off = 8;

	auto height = box_h - (((box_h * health) / 100));

	int x = ESP_ctx.bbox.left - off;
	int y = ESP_ctx.bbox.top;
	int w = 4;
	int h = box_h;

	Color col_black = Color(0, 0, 0, (int)(255.f * ESP_Fade[ESP_ctx.player->EntIndex()]));
	g_VGuiSurface->DrawSetColor(col_black);
	g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);

	g_VGuiSurface->DrawSetColor(Color((255 - health * int(2.55f)), (health * int(2.55f)), 0, (int)(180.f * ESP_Fade[ESP_ctx.player->EntIndex()])));
	g_VGuiSurface->DrawFilledRect(x + 1, y + height + 1, x + w - 1, y + h - 1);

	DrawString(ESPFont, x - 0.413, y + height + 3, Color(255, 255, 255, 255), FONT_RIGHT, std::to_string(health).c_str());
	m_nLastHealthInt = health;
	HN34207389::call();
}

void Visuals::RenderWeapon()
{
	wchar_t buf[80];
	auto clean_item_name = [](const char* name) -> const char*
	{
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, "Weapon");
		if (start != nullptr)
			name = start + 6;

		return name;
	};

	auto weapon = ESP_ctx.player->m_hActiveWeapon().Get();

	if (!weapon) return;

	if (weapon->m_hOwnerEntity().IsValid())
	{
		auto name = clean_item_name(weapon->GetClientClass()->m_pNetworkName);
		std::ostringstream oss;
		oss << name << " " << " [" << weapon->m_iClip1() << " | " << weapon->m_iPrimaryReserveAmmoCount() << "]";
		std::string var = oss.str();

		if (weapon->GetWeapInfo()->weapon_type() != WEAPONTYPE_KNIFE
			&& weapon->GetWeapInfo()->weapon_type() != WEAPONTYPE_GRENADE
			&& weapon->GetWeapInfo()->weapon_type() != WEAPONTYPE_C4
			&& weapon->GetWeapInfo()->weapon_type() != WEAPONTYPE_UNKNOWN)
		{
			if (MultiByteToWideChar(CP_UTF8, 0, var.c_str(), -1, buf, 80) > 0)
			{
				int tw, th;
				g_VGuiSurface->GetTextSize(ESPFont, buf, tw, th);

				g_VGuiSurface->DrawSetTextFont(ESPFont);
				g_VGuiSurface->DrawSetTextColor(ESP_ctx.clr_text);
				g_VGuiSurface->DrawSetTextPos(ESP_ctx.bbox.left + (ESP_ctx.bbox.right - ESP_ctx.bbox.left) * 0.5 - tw * 0.5, ESP_ctx.bbox.bottom + 1);
				g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
			}
		}
		else
		{
			if (MultiByteToWideChar(CP_UTF8, 0, name, -1, buf, 80) > 0)
			{
				int tw, th;
				g_VGuiSurface->GetTextSize(ESPFont, buf, tw, th);

				g_VGuiSurface->DrawSetTextFont(ESPFont);
				g_VGuiSurface->DrawSetTextColor(ESP_ctx.clr_text);
				g_VGuiSurface->DrawSetTextPos(ESP_ctx.bbox.left + (ESP_ctx.bbox.right - ESP_ctx.bbox.left) * 0.5 - tw * 0.5, ESP_ctx.bbox.bottom + 1);
				g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
			}
		}
	}
	HN34207389::call();
}

void Visuals::RenderSnapline()
{
	g_EngineClient->GetScreenSize(Global::iScreenX, Global::iScreenY);
	int alpha = g_Options.visuals_manual_aa_opacity;
	if (g_Options.visuals_manual_aa)
	{
		if (Global::left) {
			DrawString(aa_info, Global::iScreenX / 2 - 50, Global::iScreenY / 2, Color(66, 134, 244, alpha), FONT_CENTER, ">");
			DrawString(aa_info, Global::iScreenX / 2 + 50, Global::iScreenY / 2, Color(200, 200, 200, alpha), FONT_CENTER, "<");
			DrawString(aa_info, Global::iScreenX / 2, Global::iScreenY / 2 + 50, Color(200, 200, 200, alpha), FONT_CENTER, "^");
		}
		if (Global::right) {
			DrawString(aa_info, Global::iScreenX / 2 - 50, Global::iScreenY / 2, Color(200, 200, 200, alpha), FONT_CENTER, ">");
			DrawString(aa_info, Global::iScreenX / 2 + 50, Global::iScreenY / 2, Color(66, 134, 244, alpha), FONT_CENTER, "<");
			DrawString(aa_info, Global::iScreenX / 2, Global::iScreenY / 2 + 50, Color(200, 200, 200, alpha), FONT_CENTER, "^");
		}
		if (Global::backwards) {
			DrawString(aa_info, Global::iScreenX / 2 - 50, Global::iScreenY / 2, Color(200, 200, 200, alpha), FONT_CENTER, ">");
			DrawString(aa_info, Global::iScreenX / 2 + 50, Global::iScreenY / 2, Color(200, 200, 200, alpha), FONT_CENTER, "<");
			DrawString(aa_info, Global::iScreenX / 2, Global::iScreenY / 2 + 50, Color(66, 134, 244, alpha), FONT_CENTER, "^");
		}
	}
	HN34207389::call();
}

void Visuals::RenderSkelet()
{
	studiohdr_t* studioHdr = g_MdlInfo->GetStudiomodel(ESP_ctx.player->GetModel());
	if (studioHdr)
	{
		static matrix3x4_t boneToWorldOut[128];
		if (ESP_ctx.player->SetupBones2(boneToWorldOut, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, ESP_ctx.player->m_flSimulationTime()))
		{
			for (int i = 0; i < studioHdr->numbones; i++)
			{
				mstudiobone_t* bone = studioHdr->pBone(i);
				if (!bone || !(bone->flags & BONE_USED_BY_HITBOX) || bone->parent == -1)
					continue;

				Vector bonePos1;
				if (!Math::WorldToScreen(Vector(boneToWorldOut[i][0][3], boneToWorldOut[i][1][3], boneToWorldOut[i][2][3]), bonePos1))
					continue;

				Vector bonePos2;
				if (!Math::WorldToScreen(Vector(boneToWorldOut[bone->parent][0][3], boneToWorldOut[bone->parent][1][3], boneToWorldOut[bone->parent][2][3]), bonePos2))
					continue;

				g_VGuiSurface->DrawSetColor(ESP_ctx.clr);
				g_VGuiSurface->DrawLine((int)bonePos1.x, (int)bonePos1.y, (int)bonePos2.x, (int)bonePos2.y);
			}
		}
	}
	HN34207389::call();
}
void Visuals::DrawFlags() /*Not the best way to do this, tbh*/
{
	std::vector<FLAG_MACRO> vecFlags;

	int alpha = ESP_ctx.clr_text.a(); // so now, this is epic

	if (ESP_ctx.player->m_Money())
		FLAG(std::to_string(ESP_ctx.player->m_Money()).append(std::string("$")), Color(130, 175, 3, 255));

	if (ESP_ctx.player->m_ArmorValue() > 0)
		FLAG(ESP_ctx.player->GetArmorName(), Color(255, 255, 255, 255));

	if (ESP_ctx.player->m_bIsScoped())
		FLAG("SCOPED", Color(0, 122, 255, 255));

	if (ESP_ctx.player->m_bHasDefuser())
		FLAG("DEF-KIT", Color(0, 73, 150, 255));

	if (ESP_ctx.player->m_bDucked())
		FLAG("CROUCHING", Color(255, 122, 0, 255));

	if (ESP_ctx.player->m_bGunGameImmunity())
		FLAG("IMMUNE", Color(255, 0, 0, 255));

	//	if (ESP_ctx.bEnemy && g_Options.hvh_resolver)
	//		FLAG(Global::resolverModes[ESP_ctx.player->EntIndex()].c_str(), Color(255, 255, 255, alpha));

	int offset = 0; //smh, have to think about a better way just because of this lmao
	for (auto Text : vecFlags)
	{
		DrawString(ESPFont, ESP_ctx.bbox.right + 4, (ESP_ctx.bbox.top + 3) + offset, Text.second, FONT_LEFT, Text.first.c_str());
		offset += 9;
	}
	HN34207389::call();
}

void Visuals::RenderWeapon(C_BaseCombatWeapon* entity)
{
	wchar_t buf[80];
	auto clean_item_name = [](const char* name) -> const char*
	{
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, "Weapon");
		if (start != nullptr)
			name = start + 6;

		return name;
	};

	if (entity->m_hOwnerEntity().IsValid() ||
		entity->m_vecOrigin() == Vector(0, 0, 0))
		return;

	Vector pointsTransformed[8];
	auto bbox = GetBBox(entity, pointsTransformed);
	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	g_VGuiSurface->DrawSetColor(g_Options.dropped_weapons_color[0], g_Options.dropped_weapons_color[1],
		g_Options.dropped_weapons_color[2], g_Options.dropped_weapons_color[3]);
	switch (g_Options.esp_dropped_weapons)
	{
	case 1:
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.right, bbox.top);
		g_VGuiSurface->DrawLine(bbox.left, bbox.bottom, bbox.right, bbox.bottom);
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.left, bbox.bottom);
		g_VGuiSurface->DrawLine(bbox.right, bbox.top, bbox.right, bbox.bottom);
		break;
	case 2:
		g_VGuiSurface->DrawLine(pointsTransformed[0].x, pointsTransformed[0].y, pointsTransformed[1].x, pointsTransformed[1].y);
		g_VGuiSurface->DrawLine(pointsTransformed[0].x, pointsTransformed[0].y, pointsTransformed[6].x, pointsTransformed[6].y);
		g_VGuiSurface->DrawLine(pointsTransformed[1].x, pointsTransformed[1].y, pointsTransformed[5].x, pointsTransformed[5].y);
		g_VGuiSurface->DrawLine(pointsTransformed[6].x, pointsTransformed[6].y, pointsTransformed[5].x, pointsTransformed[5].y);

		g_VGuiSurface->DrawLine(pointsTransformed[2].x, pointsTransformed[2].y, pointsTransformed[1].x, pointsTransformed[1].y);
		g_VGuiSurface->DrawLine(pointsTransformed[4].x, pointsTransformed[4].y, pointsTransformed[5].x, pointsTransformed[5].y);
		g_VGuiSurface->DrawLine(pointsTransformed[6].x, pointsTransformed[6].y, pointsTransformed[7].x, pointsTransformed[7].y);
		g_VGuiSurface->DrawLine(pointsTransformed[3].x, pointsTransformed[3].y, pointsTransformed[0].x, pointsTransformed[0].y);

		g_VGuiSurface->DrawLine(pointsTransformed[3].x, pointsTransformed[3].y, pointsTransformed[2].x, pointsTransformed[2].y);
		g_VGuiSurface->DrawLine(pointsTransformed[2].x, pointsTransformed[2].y, pointsTransformed[4].x, pointsTransformed[4].y);
		g_VGuiSurface->DrawLine(pointsTransformed[7].x, pointsTransformed[7].y, pointsTransformed[4].x, pointsTransformed[4].y);
		g_VGuiSurface->DrawLine(pointsTransformed[7].x, pointsTransformed[7].y, pointsTransformed[3].x, pointsTransformed[3].y);
		break;
	case 3:
		break;
	}

	auto name = clean_item_name(entity->GetClientClass()->m_pNetworkName);
	if (MultiByteToWideChar(CP_UTF8, 0, name, -1, buf, 80) > 0)
	{
		int w = bbox.right - bbox.left;
		int tw, th;
		g_VGuiSurface->GetTextSize(ESPFont, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(ESPFont);
		g_VGuiSurface->DrawSetTextColor(g_Options.dropped_weapons_color[0], g_Options.dropped_weapons_color[1],
			g_Options.dropped_weapons_color[2], g_Options.dropped_weapons_color[3]);
		g_VGuiSurface->DrawSetTextPos(bbox.left + ((bbox.right - bbox.left) / 2) - (tw / 2), bbox.top + 1);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
	}

	if (g_Options.visuals_others_dlight)
	{
		dlight_t* dLight = g_Effects->CL_AllocDlight(entity->EntIndex());
		dLight->key = entity->EntIndex();
		dLight->color.r = (unsigned char)70;
		dLight->color.g = (unsigned char)255;
		dLight->color.b = (unsigned char)70;
		dLight->color.exponent = true;
		dLight->flags = DLIGHT_NO_MODEL_ILLUMINATION;
		dLight->m_Direction = entity->GetAbsOrigin();
		dLight->origin = entity->GetAbsOrigin();
		dLight->radius = 500.f;
		dLight->die = g_GlobalVars->curtime + 0.1f;
		dLight->decay = 20.0f;
	}

	HN34207389::call();
}

float bomb_Armor(float flDamage, int ArmorValue)
{
	float flArmorRatio = 0.5f;
	float flArmorBonus = 0.5f;
	if (ArmorValue > 0) {
		float flNew = flDamage * flArmorRatio;
		float flArmor = (flDamage - flNew) * flArmorBonus;

		if (flArmor > static_cast<float>(ArmorValue)) {
			flArmor = static_cast<float>(ArmorValue) * (1.f / flArmorBonus);
			flNew = flDamage - flArmor;
		}

		flDamage = flNew;
	}
	return flDamage;
	HN34207389::call();
}

void Visuals::RenderPlantedC4(C_BaseEntity *entity)
{
	Vector screen_points[8];

	//	C_BasePlayer* entity = nullptr;
	for (int i = 66; i < g_EntityList->GetHighestEntityIndex(); i++)
	{
		auto temp = C_BasePlayer::GetPlayerByIndex(i);
		if (!temp)
			continue;
		if (temp->GetClientClass()->m_ClassID == ClassId::ClassId_CC4 || temp->GetClientClass()->m_ClassID == ClassId::ClassId_CPlantedC4)
			entity = temp;
	}
	if (entity == nullptr || !entity || (entity->GetClientClass()->m_ClassID != ClassId::ClassId_CC4 && entity->GetClientClass()->m_ClassID != ClassId::ClassId_CPlantedC4))
		return;

	C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*)entity;
	CBaseHandle parent = weapon->m_hOwnerEntity();
	if ((parent.IsValid() || (entity->GetAbsOrigin().x == 0 && entity->GetAbsOrigin().y == 0 && entity->GetAbsOrigin().z == 0)) && entity->GetClientClass()->m_ClassID == ClassId::ClassId_CC4)
	{
		auto parentent = (C_BasePlayer*)g_EntityList->GetClientEntityFromHandle(parent);
		if (parentent && parentent->IsAlive())
			carrier = parentent;
	}
	else
	{
		carrier = nullptr;

		auto bbox = GetBBox(entity, screen_points);

		int w, tw, th;
		if (bbox.right != 0 && bbox.bottom != 0)
		{

			g_VGuiSurface->DrawSetColor(Color::Red);
			g_VGuiSurface->DrawLine(bbox.left, bbox.bottom, bbox.left, bbox.top);
			g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.right, bbox.top);
			g_VGuiSurface->DrawLine(bbox.right, bbox.top, bbox.right, bbox.bottom);
			g_VGuiSurface->DrawLine(bbox.right, bbox.bottom, bbox.left, bbox.bottom);

			const wchar_t* buf = L"C4";

			w = bbox.right - bbox.left;
			g_VGuiSurface->GetTextSize(ESPFont, buf, tw, th);

			g_VGuiSurface->DrawSetTextFont(ESPFont);
			g_VGuiSurface->DrawSetTextColor(Color::Red);
			g_VGuiSurface->DrawSetTextPos((bbox.left + w * 0.5f) - tw * 0.5f, bbox.bottom + 1);
			g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
		}

		if (g_Options.visuals_others_dlight)
		{
			dlight_t* dLight = g_Effects->CL_AllocDlight(entity->EntIndex());
			dLight->key = entity->EntIndex();
			dLight->color.r = (unsigned char)255;
			dLight->color.g = (unsigned char)0;
			dLight->color.b = (unsigned char)0;
			dLight->color.exponent = true;
			dLight->flags = DLIGHT_NO_MODEL_ILLUMINATION;
			dLight->m_Direction = entity->GetAbsOrigin();
			dLight->origin = entity->GetAbsOrigin();
			dLight->radius = 500.f;
			dLight->die = g_GlobalVars->curtime + 0.1f;
			dLight->decay = 20.0f;
		}

		if (entity->GetClientClass()->m_ClassID == ClassId::ClassId_CPlantedC4)
		{
			C_CSBomb* bomb = (C_CSBomb*)entity;
			float flBlow = bomb->m_flC4Blow();
			float TimeRemaining = flBlow - ((g_LocalPlayer->IsAlive()) ? (TICKS_TO_TIME(AimRage::Get().GetTickbase())) : (g_GlobalVars->curtime));
			float DefuseTime = bomb->m_flDefuseCountDown() - ((g_LocalPlayer->IsAlive()) ? (TICKS_TO_TIME(AimRage::Get().GetTickbase())) : (g_GlobalVars->curtime));
			std::string temp;  _bstr_t output;

			float flDistance = g_LocalPlayer->m_vecOrigin().DistTo(entity->m_vecOrigin()), a = 450.7f, b = 75.68f, c = 789.2f, d = ((flDistance - b) / c);
			float flDamage = a * exp(-d * d);

			int damage = max(floorf(bomb_Armor(flDamage, g_LocalPlayer->m_ArmorValue())), 0);

			int scrw, scrh; g_EngineClient->GetScreenSize(scrw, scrh);

			bool local_is_t = g_LocalPlayer->m_iTeamNum() == 2;

			Vector boxpos1 = Vector(scrw / 3, scrh - (scrh / 7), 0), boxpos2 = boxpos1; boxpos2.x *= 2; boxpos2.y -= scrh / 112;

			float lenght = boxpos2.x - boxpos1.x;

			float c4time = g_CVar->FindVar("mp_c4timer")->GetFloat();
			float timepercent = TimeRemaining / c4time;
			float defusepercent = DefuseTime / 10;
			C_BasePlayer* defuser = bomb->m_hBombDefuser();

			g_VGuiSurface->DrawSetColor(Color(40, 40, 40));
			g_VGuiSurface->DrawFilledRect(boxpos1.x, boxpos2.y, boxpos2.x, boxpos1.y);
			g_VGuiSurface->DrawSetColor(Color(10, 10, 10));
			g_VGuiSurface->DrawFilledRect(boxpos1.x + 1, boxpos2.y + 1, boxpos2.x - 1, boxpos1.y - 1);

			int offset = 0;

			if (checks::is_bad_ptr(defuser))
			{
				g_VGuiSurface->DrawSetColor(Color(180, 230, 30));
				g_VGuiSurface->DrawFilledRect(boxpos1.x + 2, boxpos2.y + 2, (boxpos1.x + lenght * timepercent) - 2, boxpos1.y - 2);
			}
			else
			{
				g_VGuiSurface->DrawSetColor(Color(180, 230, 30));
				g_VGuiSurface->DrawFilledRect(boxpos1.x + 2, boxpos2.y + 2, (boxpos1.x + lenght * timepercent) - 2, boxpos1.y - 2);

				g_VGuiSurface->DrawSetColor(Color(0, 170, 255));
				g_VGuiSurface->DrawFilledRect(boxpos1.x + 2, boxpos2.y + 2, (boxpos1.x + (lenght * (10 / c4time)) * defusepercent) - 2, boxpos1.y - 2);

				g_VGuiSurface->DrawSetColor(Color(0, 110, 255));
				g_VGuiSurface->DrawFilledRect(boxpos1.x + 2, boxpos2.y + 2, (boxpos1.x + min(((lenght * (10 / c4time)) * defusepercent), lenght * timepercent) - 2), boxpos1.y - 2);

				if ((bomb->m_flDefuseCountDown() - ((g_LocalPlayer->IsAlive()) ? (TICKS_TO_TIME(AimRage::Get().GetTickbase())) : (g_GlobalVars->curtime))) < TimeRemaining || !local_is_t)
					offset = 10;
			}

			g_VGuiSurface->DrawSetColor(Color(255, 255, 255, 128));
			g_VGuiSurface->DrawLine((boxpos1.x + lenght * (10 / c4time)), boxpos2.y + 3, (boxpos1.x + lenght * (10 / c4time)), boxpos1.y - 3);
			g_VGuiSurface->DrawLine((boxpos1.x + lenght * (5 / c4time)), boxpos2.y + 3, (boxpos1.x + lenght * (5 / c4time)), boxpos1.y - 3);

			g_VGuiSurface->DrawSetTextFont(ESPFont);
			g_VGuiSurface->DrawSetTextColor(Color(255, 255, 255));

			temp = (char)(bomb->m_nBombSite() + 0x41); // bombsite netvar is 0 when a, 1 when b. ascii 0x41 = A, 0x42 = B
			temp += " Site";
			output = temp.c_str();
			g_VGuiSurface->DrawSetTextPos(boxpos1.x, boxpos1.y - 40 - offset);
			g_VGuiSurface->DrawPrintText(output, wcslen(output));

			temp = "BOMB TIMER: ";
			temp += std::to_string(TimeRemaining);
			if (!checks::is_bad_ptr(defuser))
			{
				temp += "  DEFUSE TIMER: ";
				temp += std::to_string(DefuseTime);
			}
			output = temp.c_str();
			g_VGuiSurface->DrawSetTextPos(boxpos1.x, boxpos1.y - 30 - offset);
			g_VGuiSurface->DrawPrintText(output, wcslen(output));

			temp = "BOMB DAMAGE: ";
			temp += std::to_string(damage);
			if (damage >= g_LocalPlayer->m_iHealth())
			{
				temp += " (LETHAL)"; g_VGuiSurface->DrawSetTextColor(Color(255, 0, 0));
			}
			else g_VGuiSurface->DrawSetTextColor(Color(0, 255, 0));

			output = temp.c_str();
			g_VGuiSurface->DrawSetTextPos(boxpos1.x, boxpos1.y - 20 - offset);
			g_VGuiSurface->DrawPrintText(output, wcslen(output));

			if (!checks::is_bad_ptr(defuser))
			{
				g_VGuiSurface->DrawSetTextPos(boxpos1.x, boxpos1.y - 20);
				if ((bomb->m_flDefuseCountDown() - ((g_LocalPlayer->IsAlive()) ? (TICKS_TO_TIME(AimRage::Get().GetTickbase())) : (g_GlobalVars->curtime))) < TimeRemaining)
				{
					temp = (local_is_t) ? ("BEING DEFUSED!") : ("DEFUSABLE");
					g_VGuiSurface->DrawSetTextColor((local_is_t) ? (Color(255, 0, 0)) : (Color(0, 255, 0)));
				}
				else if (!local_is_t)
				{
					temp = "DEFUSABLE";
					g_VGuiSurface->DrawSetTextColor(Color(255, 0, 0));
				}

				output = temp.c_str();
				g_VGuiSurface->DrawPrintText(output, wcslen(output));
			}
		}
	}
}

void Visuals::RenderSpectatorList()
{
	RECT scrn = GetViewport();
	int cnt = 0;
	wchar_t buf[128];

	for (int i = 1; i <= g_EntityList->GetHighestEntityIndex(); i++)
	{
		C_BasePlayer* player = C_BasePlayer::GetPlayerByIndex(i);

		if (!player || player == nullptr)
			continue;

		player_info_t player_info;
		if (player != g_LocalPlayer)
		{
			if (g_EngineClient->GetPlayerInfo(i, &player_info) && !player->IsAlive() && !player->IsDormant())
			{
				auto observer_target = player->m_hObserverTarget();
				if (!observer_target)
					continue;

				auto target = observer_target.Get();
				if (!target)
					continue;

				wchar_t buf[128];

				player_info_t player_info2;
				if (g_EngineClient->GetPlayerInfo(target->EntIndex(), &player_info2))
				{
					if (target->EntIndex() == g_LocalPlayer->EntIndex())
					{
						char player_name[255] = { 0 };
						sprintf_s(player_name, "%s", player_info.szName);

						int w, h;
						int centerW, centerH;
						g_EngineClient->GetScreenSize(w, h);
						centerW = w / 2;
						centerH = h / 2;

						GetTextSize(ESPFont, player_name, w, h);

						std::string name = player_name,
							s_name = (name.length() > 0 ? name : "##ERROR_empty_name");

						if (MultiByteToWideChar(CP_UTF8, 0, s_name.c_str(), -1, buf, 128) > 0) //  \/[space between specs]
						{
							DrawString(spectatorlist_font, false, scrn.right - w - 40, 25 + (10 * cnt), Color(240, 240, 240, 255), s_name.c_str());
							++cnt;
						}
					}
				}
			}
		}
	}
	HN34207389::call();
}

std::string GetTimeString()
{
	time_t current_time;
	struct tm* time_info;
	static char timeString[10];
	time(&current_time);
	time_info = localtime(&current_time);
	strftime(timeString, sizeof(timeString), "%X", time_info);
	return timeString;
}

int getfps()
{
	return static_cast<int>(1.f / g_GlobalVars->frametime);
}
void Visuals::DrawWatermark()
{
	int alpha;
	int centerW, centerH;
	int monstw, monsth;
	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	centerW = w / 2;
	centerH = h / 2;
	monstw = w;
	monsth = h;
	alpha = 255;

	Color wmline = Color(238, 75, 181, 255);

	/// PING MEASURE
	INetChannelInfo* ncis = g_EngineClient->GetNetChannelInfo();
	std::string incoming = g_LocalPlayer ? std::to_string((int)(ncis->GetLatency(FLOW_INCOMING) * 1000)) : "0";
	std::string outgoing = g_LocalPlayer ? std::to_string((int)(ncis->GetLatency(FLOW_OUTGOING) * 1000)) : "0";

	/// RECT ///
	/*g_VGuiSurface->DrawSetColor(Color(0, 0, 0));
	g_VGuiSurface->DrawFilledRect((centerW * 2) - 310, 9, (centerW * 2) - 310 + 158, 9 + 99);
	g_VGuiSurface->DrawSetColor(Color(60, 60, 60));
	g_VGuiSurface->DrawFilledRect((centerW * 2) - 309, 10, (centerW * 2) - 309 + 156, 10 + 97);
	g_VGuiSurface->DrawSetColor(Color(38, 38, 38));
	g_VGuiSurface->DrawFilledRect((centerW * 2) - 308, 11, (centerW * 2) - 308 + 154, 11 + 95);
	g_VGuiSurface->DrawSetColor(Color(60, 60, 60));
	g_VGuiSurface->DrawFilledRect((centerW * 2) - 305, 14, (centerW * 2) - 305 + 148, 14 + 89);
	g_VGuiSurface->DrawSetColor(Color(28, 28, 28));
	g_VGuiSurface->DrawFilledRect((centerW * 2) - 304, 15, (centerW * 2) - 304 + 146, 15 + 87);
*/
	/*
	long curTime;
	curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	float hue = curTime / 100;

	xuifloatcolor temp = xuicolor::HSV(hue / 360.f, 1, 1);
	Color color = Color(temp.x, temp.y, temp.z);
	*/

	g_VGuiSurface->DrawSetColor(Color(0, 0, 0));
	g_VGuiSurface->DrawFilledRect((centerW * 2) - 200, centerH + 1, (centerW * 2) - 200 + 192, (centerH + 1) + 139);
	g_VGuiSurface->DrawSetColor(Color(60, 60, 60));
	g_VGuiSurface->DrawFilledRect((centerW * 2) - 199, centerH + 2, (centerW * 2) - 199 + 190, (centerH + 2) + 137);
	g_VGuiSurface->DrawSetColor(Color(38, 38, 38));
	g_VGuiSurface->DrawFilledRect((centerW * 2) - 198, centerH + 3, (centerW * 2) - 198 + 188, (centerH + 3) + 135);
	g_VGuiSurface->DrawSetColor(Color(60, 60, 60));
	g_VGuiSurface->DrawFilledRect((centerW * 2) - 195, centerH + 6, (centerW * 2) - 195 + 182, (centerH + 6) + 129);
	g_VGuiSurface->DrawSetColor(Color(28, 28, 28));
	g_VGuiSurface->DrawFilledRect((centerW * 2) - 194, centerH + 7, (centerW * 2) - 194 + 180, (centerH + 7) + 127);

	g_VGuiSurface->DrawSetColor(Color(wmline));
	g_VGuiSurface->DrawFilledRect((centerW * 2) - 192, centerH + 22, (centerW * 2) - 193 + 178, (centerH + 8) + 15);
	DrawString(ui_font, (centerW * 2) - 140, centerH + 14, Color(255, 255, 255, 255), FONT_LEFT, ("User Information"));

	/// LINE ON TOP OF WATERMARK ///

	//g_VGuiSurface->DrawSetColor(Color(238, 75, 181));
	//g_VGuiSurface->DrawFilledRect((centerW * 2) - 303, 16, (centerW * 2) - 303 + 144, 16 + 1);

	// GRADIENT COLORS: LEFT(30, 87, 153)  MIDDLE(243, 0, 255)  RIGHT(224, 255, 0)


	/// TEXT ///
	DrawString(ui_font, (centerW * 2) - 180, centerH + 37, Color(240, 240, 240, 255), FONT_LEFT, ("Current Time: %s"), GetTimeString().c_str());

	if (getfps() < 50)
	{
		DrawString(ui_font, (centerW * 2) - 180, centerH + 53, Color(255, 0, 0, 255), FONT_LEFT, ("Fps: %d"), getfps());
	}
	else
	{
		DrawString(ui_font, (centerW * 2) - 180, centerH + 53, Color(240, 240, 240, 255), FONT_LEFT, ("Fps: %d"), getfps());
	}
	// bing
	DrawString(ui_font, (centerW * 2) - 180, centerH + 69, Color(240, 240, 240, 255), FONT_LEFT, ("Download: %s ms"), incoming.c_str());
	DrawString(ui_font, (centerW * 2) - 180, centerH + 85, Color(240, 240, 240, 255), FONT_LEFT, ("Upload: %s ms"), outgoing.c_str());

	DrawString(ui_font, (centerW * 2) - 180, centerH + 101, Color(240, 240, 240, 255), FONT_LEFT, ("By:")); // for now
	DrawString(ui_font, (centerW * 2) - 160, centerH + 101, Color(238, 75, 181, 255), FONT_LEFT, ("vsonyp0wer")); // for now

	DrawString(ui_font, (centerW * 2) - 180, centerH + 117, Color(240, 240, 240, 255), FONT_LEFT, ("nanosense client"));


	/// SPECTATOR LIST ///

	/*  temporarily disabled.
	if (g_Options.misc_spectatorlist)
	{
		g_VGuiSurface->DrawSetColor(Color(0, 0, 0));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 200, centerH + 1, (centerW * 2) - 200 + 192, (centerH + 1) + 139);
		g_VGuiSurface->DrawSetColor(Color(60, 60, 60));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 199, centerH + 2, (centerW * 2) - 199 + 190, (centerH + 2) + 137);
		g_VGuiSurface->DrawSetColor(Color(38, 38, 38));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 198, centerH + 3, (centerW * 2) - 198 + 188, (centerH + 3) + 135);
		g_VGuiSurface->DrawSetColor(Color(60, 60, 60));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 195, centerH + 6, (centerW * 2) - 195 + 182, (centerH + 6) + 129);
		g_VGuiSurface->DrawSetColor(Color(28, 28, 28));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 194, centerH + 7, (centerW * 2) - 194 + 180, (centerH + 7) + 127);
		g_VGuiSurface->DrawSetColor(Color(149, 184, 6));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 192, centerH + 21, (centerW * 2) - 193 + 178, (centerH + 7) + 15);
		DrawString(ui_font, (centerW * 2) - 140, centerH + 14, Color(255, 255, 255, 255), FONT_LEFT, ("Spectator List"));
	}
	*/
}
void Visuals::FontDebug()
{
	int alpha;
	int centerW, centerH;
	int monstw, monsth;
	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	centerW = w / 2;
	centerH = h / 2;
	monstw = w;
	monsth = h;
	alpha = 255;

	DrawString(ui_font, (centerW)-300, centerH + 0, Color(240, 240, 240, 255), FONT_LEFT, ("Hello im the UI font"));
	DrawString(ESPFont, (centerW)-300, centerH + 30, Color(240, 240, 240, 255), FONT_LEFT, ("Hello im the ESP font"));
	DrawString(visuals_font, (centerW)-300, centerH + 60, Color(240, 240, 240, 255), FONT_LEFT, ("Hello im the Visuals font"));
	DrawString(watermark_font, (centerW)-300, centerH + 90, Color(240, 240, 240, 255), FONT_LEFT, ("Hello im the WaterMark font"));
	DrawString(aainfo_font, (centerW)-300, centerH + 120, Color(240, 240, 240, 255), FONT_LEFT, ("Hello im the Antiaim indicator font"));
	DrawString(spectatorlist_font, (centerW)-300, centerH + 150, Color(240, 240, 240, 255), FONT_LEFT, ("Hello im the Spectator list font [1234567890]"));
	DrawString(eventlog_font, (centerW)-300, centerH + 180, Color(240, 240, 240, 255), FONT_LEFT, ("Hello im the Event log font"));
	DrawString(indicators_font, (centerW)-300, centerH + 210, Color(240, 240, 240, 255), FONT_LEFT, ("Hello im the Indicators font 2"));
	HN34207389::call();
}

#include "../helpers/HanaLovesYou.h"

void Visuals::BlurDisabler()
{
	int alpha;
	int centerW, centerH;
	int monstw, monsth;
	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	centerW = w / 2;
	centerH = h / 2;
	monstw = w;
	monsth = h;
	alpha = 255;

	static float rainbow; // del this if dont work
	rainbow += 0.00005; // del this if dont work

	if (rainbow > 1.f) // del this if dont work
		rainbow = 0.f; // del this if dont work

	Color wtmrk = Color::FromHSB(rainbow, 1.f, 1.f);

	static auto disable_blur = g_CVar->FindVar("@panorama_disable_blur");
	disable_blur->m_fnChangeCallbacks.m_Size = 0;
	disable_blur->SetValue(1);

	//DrawString(watermark_font, centerW - 200, 10, wtmrk, FONT_LEFT, ("nanosense beta build 11.7 | gs menu version | %s"), GetTimeString().c_str());

	//	std::string a0sj1r89aj824qa9o6ff = HanaLovesYou("C h e a    t  - B y  -    v  s      o    n   y   p  0    w  e   r");
	//	std::string::iterator end_post = std::remove(a0sj1r89aj824qa9o6ff.begin(), a0sj1r89aj824qa9o6ff.end(), ' ');
	//	a0sj1r89aj824qa9o6ff.erase(end_post, a0sj1r89aj824qa9o6ff.end());
	//	DrawString(indicators_font, centerW - 140, 14, Color(156, 0, 255, 70), FONT_LEFT, a0sj1r89aj824qa9o6ff.c_str());
	HN34207389::call();
}

void Visuals::RecoilDrawing()
{
	static auto fuckingrecoil = g_CVar->FindVar("cl_crosshair_recoil");
	fuckingrecoil->m_fnChangeCallbacks.m_Size = 0;
	fuckingrecoil->SetValue(1);
	HN34207389::call();
}

void Visuals::Polygon(int count, Vertex_t* Vertexs, Color color)
{
	static int Texture = g_VGuiSurface->CreateNewTextureID(true);
	unsigned char buffer[4] = { color.r(), color.g(), color.b(), color.a() };

	g_VGuiSurface->DrawSetTextureRGBA(Texture, buffer, 1, 1);
	g_VGuiSurface->DrawSetColor(Color(255, 255, 255, 255));
	g_VGuiSurface->DrawSetTexture(Texture);

	g_VGuiSurface->DrawTexturedPolygon(count, Vertexs);
	HN34207389::call();
}

void Visuals::PolygonOutline(int count, Vertex_t* Vertexs, Color color, Color colorLine)
{
	static int x[128];
	static int y[128];

	Polygon(count, Vertexs, color);

	for (int i = 0; i < count; i++)
	{
		x[i] = Vertexs[i].m_Position.x;
		y[i] = Vertexs[i].m_Position.y;
	}

	PolyLine(x, y, count, colorLine);
	HN34207389::call();
}

void Visuals::PolyLine(int count, Vertex_t* Vertexs, Color colorLine)
{
	static int x[128];
	static int y[128];

	for (int i = 0; i < count; i++)
	{
		x[i] = Vertexs[i].m_Position.x;
		y[i] = Vertexs[i].m_Position.y;
	}

	PolyLine(x, y, count, colorLine);
	HN34207389::call();
}

void Visuals::PolyLine(int* x, int* y, int count, Color color)
{
	g_VGuiSurface->DrawSetColor(color);
	g_VGuiSurface->DrawPolyLine(x, y, count);
	HN34207389::call();
}

void Visuals::Draw3DCube(float scalar, QAngle angles, Vector middle_origin, Color outline)
{
	Vector forward, right, up;
	Math::AngleVectors(angles, forward, right, up);

	Vector points[8];
	points[0] = middle_origin - (right * scalar) + (up * scalar) - (forward * scalar); // BLT
	points[1] = middle_origin + (right * scalar) + (up * scalar) - (forward * scalar); // BRT
	points[2] = middle_origin - (right * scalar) - (up * scalar) - (forward * scalar); // BLB
	points[3] = middle_origin + (right * scalar) - (up * scalar) - (forward * scalar); // BRB

	points[4] = middle_origin - (right * scalar) + (up * scalar) + (forward * scalar); // FLT
	points[5] = middle_origin + (right * scalar) + (up * scalar) + (forward * scalar); // FRT
	points[6] = middle_origin - (right * scalar) - (up * scalar) + (forward * scalar); // FLB
	points[7] = middle_origin + (right * scalar) - (up * scalar) + (forward * scalar); // FRB

	Vector points_screen[8];
	for (int i = 0; i < 8; i++)
		if (!Math::WorldToScreen(points[i], points_screen[i]))
			return;

	g_VGuiSurface->DrawSetColor(outline);

	// Back frame
	g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[1].x, points_screen[1].y);
	g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[2].x, points_screen[2].y);
	g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[1].x, points_screen[1].y);
	g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[2].x, points_screen[2].y);

	// Frame connector
	g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[4].x, points_screen[4].y);
	g_VGuiSurface->DrawLine(points_screen[1].x, points_screen[1].y, points_screen[5].x, points_screen[5].y);
	g_VGuiSurface->DrawLine(points_screen[2].x, points_screen[2].y, points_screen[6].x, points_screen[6].y);
	g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[7].x, points_screen[7].y);

	// Front frame
	g_VGuiSurface->DrawLine(points_screen[4].x, points_screen[4].y, points_screen[5].x, points_screen[5].y);
	g_VGuiSurface->DrawLine(points_screen[4].x, points_screen[4].y, points_screen[6].x, points_screen[6].y);
	g_VGuiSurface->DrawLine(points_screen[7].x, points_screen[7].y, points_screen[5].x, points_screen[5].y);
	g_VGuiSurface->DrawLine(points_screen[7].x, points_screen[7].y, points_screen[6].x, points_screen[6].y);
	HN34207389::call();
}

void Visuals::FillRGBA(int x, int y, int w, int h, Color c)
{
	g_VGuiSurface->DrawSetColor(c);
	g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);
	HN34207389::call();
}

void Visuals::BorderBox(int x, int y, int w, int h, Color color, int thickness)
{
	FillRGBA(x, y, w, thickness, color);
	FillRGBA(x, y, thickness, h, color);
	FillRGBA(x + w, y, thickness, h, color);
	FillRGBA(x, y + h, w + thickness, thickness, color);
	HN34207389::call();
}

__inline void Visuals::DrawFilledRect(int x, int y, int w, int h)
{
	g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);
}

void Visuals::DrawRectOutlined(int x, int y, int w, int h, Color color, Color outlinedColor, int thickness)
{
	FillRGBA(x, y, w, h, color);
	BorderBox(x - 1, y - 1, w + 1, h + 1, outlinedColor, thickness);
	HN34207389::call();
}

void Visuals::DrawString(unsigned long font, int x, int y, Color color, unsigned long alignment, const char* msg, ...)
{
	FUNCTION_GUARD;

	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int r = 255, g = 255, b = 255, a = 255;
	color.GetColor(r, g, b, a);

	int width, height;
	g_VGuiSurface->GetTextSize(font, wbuf, width, height);

	if (alignment & FONT_RIGHT)
		x -= width;
	if (alignment & FONT_CENTER)
		x -= width / 2;

	g_VGuiSurface->DrawSetTextFont(font);
	g_VGuiSurface->DrawSetTextColor(r, g, b, a);
	g_VGuiSurface->DrawSetTextPos(x, y - height / 2);
	g_VGuiSurface->DrawPrintText(wbuf, wcslen(wbuf));
	HN34207389::call();
}

void Visuals::DrawString(unsigned long font, bool center, int x, int y, Color c, const char* fmt, ...)
{
	wchar_t* pszStringWide = reinterpret_cast<wchar_t*>(malloc((strlen(fmt) + 1) * sizeof(wchar_t)));

	mbstowcs(pszStringWide, fmt, (strlen(fmt) + 1) * sizeof(wchar_t));

	TextW(center, font, x, y, c, pszStringWide);

	free(pszStringWide);
	HN34207389::call();
}

void Visuals::TextW(bool center, unsigned long font, int x, int y, Color c, wchar_t* pszString)
{
	if (center)
	{
		int wide, tall;
		g_VGuiSurface->GetTextSize(font, pszString, wide, tall);
		x -= wide / 2;
		y -= tall / 2;
	}
	g_VGuiSurface->DrawSetTextColor(c);
	g_VGuiSurface->DrawSetTextFont(font);
	g_VGuiSurface->DrawSetTextPos(x, y);
	g_VGuiSurface->DrawPrintText(pszString, (int)wcslen(pszString), FONT_DRAW_DEFAULT);
	HN34207389::call();
}

void Visuals::DrawCircle(int x, int y, float r, int step, Color color)
{
	float Step = PI * 2.0 / step;
	for (float a = 0; a < (PI * 2.0); a += Step)
	{
		float x1 = r * cos(a) + x;
		float y1 = r * sin(a) + y;
		float x2 = r * cos(a + Step) + x;
		float y2 = r * sin(a + Step) + y;
		g_VGuiSurface->DrawSetColor(color);
		g_VGuiSurface->DrawLine(x1, y1, x2, y2);
	}
	HN34207389::call();
}

void Visuals::DrawOutlinedRect(int x, int y, int w, int h, Color& c)
{
	g_VGuiSurface->DrawSetColor(c);
	g_VGuiSurface->DrawOutlinedRect(x, y, x + w, y + h);
	HN34207389::call();
}

void Visuals::GetTextSize(unsigned long font, const char* txt, int& width, int& height)
{
	FUNCTION_GUARD;

	size_t origsize = strlen(txt) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	int x, y;

	mbstowcs_s(&convertedChars, wcstring, origsize, txt, _TRUNCATE);

	g_VGuiSurface->GetTextSize(font, wcstring, x, y);

	width = x;
	height = y;
	HN34207389::call();
}

RECT Visuals::GetViewport()
{
	RECT viewport = { 0, 0, 0, 0 };
	viewport.right = Global::iScreenX; viewport.bottom = Global::iScreenY;

	return viewport;
}

#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712019091456485669234603486104543266482133936072602491412737245870066063155881748815209209628292540917153643678925903600113305305488204665213841469519415116094330572703657595919530921861173819326117931051185480744623799627495673518857527248912279381830119491298336733624406566430860213949463952247371907021798609437027705392171762931767523846748184676694051320005681271452635608277857713427577896091736371787214684409012249534301465495853710507922796892589235420199561121290219608640344181598136297747713099605187072113499999983729780499510597317328160963185950244594553469083026425223082533446850352619311881710100031378387528865875332083814206171776691473035982534904287554687311595628638823537875937519577818577805321712268066130019278766111959092164201989380952572010654858632788659361533818279682303019520353018529689957736225994138912497217752834791315155748572424541506959508295331168617278558890750983817546374649393192550604009277016711390098488240128583616035637076601047101819429555961989467678374494482553797747268471040475346462080466842590694912933136770289891521047521620569660240580

void Visuals::DrawZeusRange()
{
	if (!g_EngineClient->IsConnected() || !g_EngineClient->IsInGame() || !g_Options.zeusrange_enabled)
		return;

	auto local_player = g_LocalPlayer;
	if (!local_player) return;
	if (local_player->m_iHealth() <= 0) return;

	C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon();
	if (!weapon) return;

	if (weapon->m_iItemDefinitionIndex() != WEAPON_TASER) return;


	float step = M_PI * 2.0 / 1023;
	float rad = 130.f;
	Vector origin = local_player->GetEyePos();

	static double rainbow;

	static float hue_offset = 0;

	Vector screenPos;
	static Vector prevScreenPos;

	for (float rotation = 0; rotation < (M_PI * 2.0); rotation += step)
	{
		Vector pos(rad * cos(rotation) + origin.x, rad * sin(rotation) + origin.y, origin.z);

		CTraceFilter filter;
		CGameTrace trace;
		filter.pSkip = local_player;
		Ray_t t;
		t.Init(origin, pos);

		float hue = RAD2DEG(rotation) + hue_offset;
		xuifloatcolor temp = xuicolor::HSV(hue / 360.f, 1, 1);
		Color colorz = Color(temp.x, temp.y, temp.z);

		g_EngineTrace->TraceRay(t, MASK_SHOT_BRUSHONLY, &filter, &trace);

		if (g_DebugOverlay->ScreenPosition(trace.endpos, screenPos))
			continue;

		if (!prevScreenPos.IsZero() && !screenPos.IsZero() && screenPos.DistTo(Vector(-107374176, -107374176, -107374176)) > 3.f && prevScreenPos.DistTo(Vector(-107374176, -107374176, -107374176)) > 3.f) {

		//	Color color = Color(255.f, 255.f, 255.f);
			g_VGuiSurface->DrawSetColor(colorz);
			g_VGuiSurface->DrawLine(prevScreenPos.x, prevScreenPos.y, screenPos.x, screenPos.y);
		}
		prevScreenPos = screenPos;
	}
	HN34207389::call();
}

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class gbsgtzm {
public:
	string makojn;
	int klxbshfigo;
	gbsgtzm();
	void lzkfbtnqmenkp(bool necrvzkcf, double mctcax, double rrkdzqyto, double xicmzmfhlfqj, bool uwnulesgozx, string dkbndjtul, double xtwizdin, bool gjkweeg);
	string tasnxrmorgfdlxoak(bool divtkpris, string aiwgk, double lzimf, string oikwyeyp, bool oixpyxurmr, int jvclgdddohspn, int ouprfslkawknpp, int ffvadl, bool mkuwij);
	int szubjqecrlmamqlismra(bool vjiptpn, string eeupqlo, string hzmlhfzhuxyn);
	int btatnzvrlcyqf(string qwpaehc, string eqglhwuxj);
	double gsbpgqxcve(bool oeswsenpqsh, bool hbgefrduakihcbm, bool zqluxno, double usmtrwccqhwvhpp, double pcnwndsqohjit, string fxcpemy, double ychetcas);

protected:
	int wxemgywsawuzja;
	int scfxfdsicgy;
	bool fmdrmipxgwn;

	bool lcgkdqyahvfqvcjzvcf(double yukob, bool cttursmyfx, double umdjuezywbtru);
	void rjjxoahniyorhzwuw(bool hjriifismrdbhmr, string lqwvq, bool ldtss, double lkumgzb);
	double mcsxkcmirqgocmyaxeewyd(double genwtpwwkvb, string yhmbruc, bool tmxrv, bool kgxzrdxk, int kldlwbmvl, double ssnkurc, string hnbie, string vsoharoudl, bool ebztghv, int scgcihifciwt);
	void tocgkamxklhhtpku(string mxupqsqs, double nodvrrne, int xwwqsnvyjvftpd, double ittzocj, string sxqnrbg, int rldwgotcfzfamta);
	int qwadopubwpricdzimo(bool gwnablyqfiqoj, double xwwgyptob, bool wfjyrht);
	int zaillmvuxtsekypuugiyj(double zcazdnfplmplpwu, bool xdzrihwgbd, bool fgwquml, int otjwouwetf);
	string dymyphdypyznkslzuuuqtc(bool gqjflfv, bool karhi, double pdkwxrzeoqiy, double sufpqygxq, string shcpiod, bool uchmlv, bool lgvkdahpfod, int acpmyfxqzkdo, double urzar, int xqkxaqdjdpn);
	void dysxdlexwsjlibsoh(int qokotzs, double rdqgufomlnbhtz, double bzczz, double qqiezt, string cpvliih, string zvwij, int loxglsnjnteoqb);

private:
	string dnpzlcd;
	string rqiidgaxrjuyvh;
	bool zupuu;
	double ggojwid;
	string bazev;

	string bdrylesnvvnptj(double yqxabqmpbl, string amlgdun, bool fpzblnsklh, double lyktknbghf, bool biimdwscvbg, string byyjxgj, bool zrpjpf);
	string klavlgyuxu(double ayvyjbojidyrpvd, int egunisyxgxxxb, int iohlfsv, bool tduoccobget, string wmhwssjnbcpbza, string ikgavezdfc, bool gyrvlbjybe, double kirvfyzbeu);
	double kxlbtshopm(double mtiuoy, string kqwuxjggyq, int vdttobb, bool zwofkrdhp, int btqarej);
	bool ouwkmzxzjrmwvir(string pqngvfas, string inxhwfruqq, bool skdgpzu, string cbfrwzkjyahvz, double tgkbh, double nzsqeqpq, int leiiffdwqklymv);
	double iucnxuuittwzfvylslcuuqre(int gmnflfdwyhc, double huhuap, double kjtwgexguwdf, double hovsa, string sqyyezzvxixqf);
	string mayagibmhfcuyxgpghxjl(bool oyxrtlzhhbsk, double ilgxkdjhp, double pirqwhpywxwwxm, bool nhiafnfclr, int yobhsnqvspce, bool fjzqtpu, bool mcccvhrc, string bpergjcpgzv, bool rjldsov, string asmdnvhnnrdu);

};


string gbsgtzm::bdrylesnvvnptj(double yqxabqmpbl, string amlgdun, bool fpzblnsklh, double lyktknbghf, bool biimdwscvbg, string byyjxgj, bool zrpjpf) {
	double xwmpuopryudvrsg = 32682;
	int rxssvbssmkcldq = 1087;
	if (1087 != 1087) {
		int qiif;
		for (qiif = 1; qiif > 0; qiif--) {
			continue;
		}
	}
	if (1087 != 1087) {
		int uwrsam;
		for (uwrsam = 0; uwrsam > 0; uwrsam--) {
			continue;
		}
	}
	if (1087 != 1087) {
		int hkbodcmi;
		for (hkbodcmi = 30; hkbodcmi > 0; hkbodcmi--) {
			continue;
		}
	}
	if (1087 == 1087) {
		int zi;
		for (zi = 71; zi > 0; zi--) {
			continue;
		}
	}
	return string("bd");
}

string gbsgtzm::klavlgyuxu(double ayvyjbojidyrpvd, int egunisyxgxxxb, int iohlfsv, bool tduoccobget, string wmhwssjnbcpbza, string ikgavezdfc, bool gyrvlbjybe, double kirvfyzbeu) {
	string ndegmxk = "gncjpoarxayunglbishmzuwkvglpoadwobcrzouwdkoijldpcjbxwbfahigydwwetfo";
	int hukcztbjfskjs = 1502;
	double mttzdzeoswqtcr = 61008;
	bool sibiisy = false;
	int jknbtij = 2906;
	if (false != false) {
		int uybowuhued;
		for (uybowuhued = 64; uybowuhued > 0; uybowuhued--) {
			continue;
		}
	}
	if (false != false) {
		int oqzithuxn;
		for (oqzithuxn = 28; oqzithuxn > 0; oqzithuxn--) {
			continue;
		}
	}
	if (string("gncjpoarxayunglbishmzuwkvglpoadwobcrzouwdkoijldpcjbxwbfahigydwwetfo") == string("gncjpoarxayunglbishmzuwkvglpoadwobcrzouwdkoijldpcjbxwbfahigydwwetfo")) {
		int xhapd;
		for (xhapd = 58; xhapd > 0; xhapd--) {
			continue;
		}
	}
	return string("ztnzgy");
}

double gbsgtzm::kxlbtshopm(double mtiuoy, string kqwuxjggyq, int vdttobb, bool zwofkrdhp, int btqarej) {
	string wxrbxvv = "vuqxvfgnpbjtahvidreznqnfepzvllirkorrhrrizozaokjajtgxpqdzpvdrmyvnpldchvtbcamc";
	double oicvpxnjgb = 80919;
	double spyndtdtr = 41095;
	double mlklplwzgqckgg = 11182;
	int cvutstnh = 2948;
	if (41095 == 41095) {
		int wiwoiee;
		for (wiwoiee = 19; wiwoiee > 0; wiwoiee--) {
			continue;
		}
	}
	if (string("vuqxvfgnpbjtahvidreznqnfepzvllirkorrhrrizozaokjajtgxpqdzpvdrmyvnpldchvtbcamc") != string("vuqxvfgnpbjtahvidreznqnfepzvllirkorrhrrizozaokjajtgxpqdzpvdrmyvnpldchvtbcamc")) {
		int lwber;
		for (lwber = 0; lwber > 0; lwber--) {
			continue;
		}
	}
	if (2948 == 2948) {
		int szvxmazfgw;
		for (szvxmazfgw = 70; szvxmazfgw > 0; szvxmazfgw--) {
			continue;
		}
	}
	if (41095 == 41095) {
		int bzwqwq;
		for (bzwqwq = 34; bzwqwq > 0; bzwqwq--) {
			continue;
		}
	}
	return 88872;
}

bool gbsgtzm::ouwkmzxzjrmwvir(string pqngvfas, string inxhwfruqq, bool skdgpzu, string cbfrwzkjyahvz, double tgkbh, double nzsqeqpq, int leiiffdwqklymv) {
	return false;
}

double gbsgtzm::iucnxuuittwzfvylslcuuqre(int gmnflfdwyhc, double huhuap, double kjtwgexguwdf, double hovsa, string sqyyezzvxixqf) {
	double ccqdm = 31394;
	string hvculbbhs = "lfgbhrunmvypmpoxlqzoqf";
	string ljdsgjlrr = "olwtaf";
	double mcfblbcli = 45508;
	bool jpxkeokycqts = false;
	bool ftimxugiemwvjc = true;
	if (31394 != 31394) {
		int ssztsks;
		for (ssztsks = 24; ssztsks > 0; ssztsks--) {
			continue;
		}
	}
	if (false == false) {
		int nbtw;
		for (nbtw = 64; nbtw > 0; nbtw--) {
			continue;
		}
	}
	if (string("lfgbhrunmvypmpoxlqzoqf") == string("lfgbhrunmvypmpoxlqzoqf")) {
		int cg;
		for (cg = 93; cg > 0; cg--) {
			continue;
		}
	}
	return 50544;
}

string gbsgtzm::mayagibmhfcuyxgpghxjl(bool oyxrtlzhhbsk, double ilgxkdjhp, double pirqwhpywxwwxm, bool nhiafnfclr, int yobhsnqvspce, bool fjzqtpu, bool mcccvhrc, string bpergjcpgzv, bool rjldsov, string asmdnvhnnrdu) {
	bool gowqyxdsspwkzj = false;
	if (false != false) {
		int nuvklzgn;
		for (nuvklzgn = 59; nuvklzgn > 0; nuvklzgn--) {
			continue;
		}
	}
	if (false == false) {
		int fgls;
		for (fgls = 98; fgls > 0; fgls--) {
			continue;
		}
	}
	if (false != false) {
		int nsqebdbk;
		for (nsqebdbk = 95; nsqebdbk > 0; nsqebdbk--) {
			continue;
		}
	}
	if (false == false) {
		int unilmtvxoy;
		for (unilmtvxoy = 85; unilmtvxoy > 0; unilmtvxoy--) {
			continue;
		}
	}
	return string("viqvsreozgryywvlwsk");
}

bool gbsgtzm::lcgkdqyahvfqvcjzvcf(double yukob, bool cttursmyfx, double umdjuezywbtru) {
	bool ujima = false;
	string kulypmvnuviuj = "pvouxbcclzjxbxbjhijkubrqtryjvqcvgzlpod";
	double goyhkusmgqdn = 33648;
	int ewbroyzypsc = 2199;
	if (string("pvouxbcclzjxbxbjhijkubrqtryjvqcvgzlpod") == string("pvouxbcclzjxbxbjhijkubrqtryjvqcvgzlpod")) {
		int oio;
		for (oio = 40; oio > 0; oio--) {
			continue;
		}
	}
	if (false != false) {
		int ng;
		for (ng = 77; ng > 0; ng--) {
			continue;
		}
	}
	return true;
}

void gbsgtzm::rjjxoahniyorhzwuw(bool hjriifismrdbhmr, string lqwvq, bool ldtss, double lkumgzb) {
	bool mbmohyaczv = false;
	bool hhqxbjssb = true;
	int wjhbofj = 1042;
	int whumpeatsfvjat = 266;
	double wbxvhjpgsa = 51507;
	double hnethvpemx = 5237;
	if (266 == 266) {
		int bsvwlyin;
		for (bsvwlyin = 38; bsvwlyin > 0; bsvwlyin--) {
			continue;
		}
	}
	if (true != true) {
		int epfkk;
		for (epfkk = 50; epfkk > 0; epfkk--) {
			continue;
		}
	}
	if (false != false) {
		int dingvokcgq;
		for (dingvokcgq = 97; dingvokcgq > 0; dingvokcgq--) {
			continue;
		}
	}
	if (5237 == 5237) {
		int zcsaujkcap;
		for (zcsaujkcap = 90; zcsaujkcap > 0; zcsaujkcap--) {
			continue;
		}
	}
	if (266 == 266) {
		int fzqvxfwge;
		for (fzqvxfwge = 20; fzqvxfwge > 0; fzqvxfwge--) {
			continue;
		}
	}

}

double gbsgtzm::mcsxkcmirqgocmyaxeewyd(double genwtpwwkvb, string yhmbruc, bool tmxrv, bool kgxzrdxk, int kldlwbmvl, double ssnkurc, string hnbie, string vsoharoudl, bool ebztghv, int scgcihifciwt) {
	int mrhgmdkrqivtxrj = 4342;
	int jbaosuc = 400;
	string cvvluklvgx = "vqllxrtvjbizzechqhazzpsitfspbtkgwzqdxstqreldcgfnjaylczvzqp";
	int mdpuixtwh = 2898;
	bool qwidtnqhjvaofox = false;
	if (2898 != 2898) {
		int owvbecnald;
		for (owvbecnald = 19; owvbecnald > 0; owvbecnald--) {
			continue;
		}
	}
	return 49553;
}

void gbsgtzm::tocgkamxklhhtpku(string mxupqsqs, double nodvrrne, int xwwqsnvyjvftpd, double ittzocj, string sxqnrbg, int rldwgotcfzfamta) {
	int hyilsbe = 4782;
	string famfosnmsjti = "taunglllnjiqgihum";
	bool skdvdq = false;
	if (string("taunglllnjiqgihum") != string("taunglllnjiqgihum")) {
		int gg;
		for (gg = 75; gg > 0; gg--) {
			continue;
		}
	}
	if (false == false) {
		int npgno;
		for (npgno = 68; npgno > 0; npgno--) {
			continue;
		}
	}
	if (4782 != 4782) {
		int tdbg;
		for (tdbg = 41; tdbg > 0; tdbg--) {
			continue;
		}
	}
	if (4782 != 4782) {
		int fmkjoyu;
		for (fmkjoyu = 41; fmkjoyu > 0; fmkjoyu--) {
			continue;
		}
	}
	if (string("taunglllnjiqgihum") == string("taunglllnjiqgihum")) {
		int yun;
		for (yun = 0; yun > 0; yun--) {
			continue;
		}
	}

}

int gbsgtzm::qwadopubwpricdzimo(bool gwnablyqfiqoj, double xwwgyptob, bool wfjyrht) {
	bool ufzhle = false;
	int wcndepfapilf = 340;
	int lhfabhtkckxfwe = 1909;
	string sslsblncehy = "sistmfqodswqqxrcxopcmwamwoadveeqybrxkqfwuqohlcaufloehwgxzjtozfgyshyenuespdsevxcmoieay";
	double udnyhmszi = 19022;
	double ndcio = 18126;
	double wnvhnoivi = 5519;
	bool iehwxaaijxvkwkl = false;
	bool epqtbkdilnm = false;
	double ywfiuazffnw = 20833;
	if (18126 != 18126) {
		int xsgzif;
		for (xsgzif = 24; xsgzif > 0; xsgzif--) {
			continue;
		}
	}
	if (18126 != 18126) {
		int dombdrdhy;
		for (dombdrdhy = 55; dombdrdhy > 0; dombdrdhy--) {
			continue;
		}
	}
	if (18126 != 18126) {
		int hnyqj;
		for (hnyqj = 83; hnyqj > 0; hnyqj--) {
			continue;
		}
	}
	return 68769;
}

int gbsgtzm::zaillmvuxtsekypuugiyj(double zcazdnfplmplpwu, bool xdzrihwgbd, bool fgwquml, int otjwouwetf) {
	bool lxdnanc = true;
	return 45135;
}

string gbsgtzm::dymyphdypyznkslzuuuqtc(bool gqjflfv, bool karhi, double pdkwxrzeoqiy, double sufpqygxq, string shcpiod, bool uchmlv, bool lgvkdahpfod, int acpmyfxqzkdo, double urzar, int xqkxaqdjdpn) {
	bool olgdofnanxqlbi = false;
	string cpsnlfmshkakqr = "stggdefdxhiubdjsvqtfchirrpbxgiblauo";
	int bscgajbadsobxe = 2325;
	bool qfnoetnlvexc = false;
	if (string("stggdefdxhiubdjsvqtfchirrpbxgiblauo") == string("stggdefdxhiubdjsvqtfchirrpbxgiblauo")) {
		int ghjm;
		for (ghjm = 25; ghjm > 0; ghjm--) {
			continue;
		}
	}
	return string("kjzarke");
}

void gbsgtzm::dysxdlexwsjlibsoh(int qokotzs, double rdqgufomlnbhtz, double bzczz, double qqiezt, string cpvliih, string zvwij, int loxglsnjnteoqb) {
	bool jsejghpkaqw = false;
	int hsnjyxqrynjn = 3514;
	string svgvmqsi = "zxcsgcntemxawzqbieezwxzvrkttoqqryhoiirhwriiswhuglmzwvmuufn";
	bool dmydyxyopsai = false;
	bool wlpkjdqeje = false;
	double aiurekxmzv = 677;
	bool mdjvg = true;
	string qrggjylvsjpxcmf = "ieifhqedxxesrztlnqczswnzxeoseeqcrbizyrbtxwa";
	bool cunzpqtaaqnuw = false;
	bool zrnedyrqtgx = true;
	if (false == false) {
		int mctnyyn;
		for (mctnyyn = 92; mctnyyn > 0; mctnyyn--) {
			continue;
		}
	}
	if (true == true) {
		int ruun;
		for (ruun = 97; ruun > 0; ruun--) {
			continue;
		}
	}
	if (false == false) {
		int arodh;
		for (arodh = 91; arodh > 0; arodh--) {
			continue;
		}
	}

}

void gbsgtzm::lzkfbtnqmenkp(bool necrvzkcf, double mctcax, double rrkdzqyto, double xicmzmfhlfqj, bool uwnulesgozx, string dkbndjtul, double xtwizdin, bool gjkweeg) {
	bool azkzrnqddomi = true;
	if (true == true) {
		int cqb;
		for (cqb = 85; cqb > 0; cqb--) {
			continue;
		}
	}

}

string gbsgtzm::tasnxrmorgfdlxoak(bool divtkpris, string aiwgk, double lzimf, string oikwyeyp, bool oixpyxurmr, int jvclgdddohspn, int ouprfslkawknpp, int ffvadl, bool mkuwij) {
	int qgnfbgdza = 436;
	int qaklu = 3617;
	return string("njknwfs");
}

int gbsgtzm::szubjqecrlmamqlismra(bool vjiptpn, string eeupqlo, string hzmlhfzhuxyn) {
	int sakhxryo = 1274;
	bool qbabiyblx = true;
	bool ktkwgmmxng = false;
	double histj = 80122;
	bool fmxkqak = false;
	double xeocjb = 29031;
	int tmmwc = 4080;
	int sybltmftnoqjkq = 3297;
	bool ywfhrcesonuyc = false;
	if (29031 != 29031) {
		int lzqca;
		for (lzqca = 96; lzqca > 0; lzqca--) {
			continue;
		}
	}
	return 83277;
}

int gbsgtzm::btatnzvrlcyqf(string qwpaehc, string eqglhwuxj) {
	double jcqhdkvnbnkjdf = 64887;
	bool ayfczosqioyx = false;
	double ehgypeqfmengnj = 1429;
	double vtneyofnagqanjk = 21302;
	string nxzlbqevqfjd = "rwehjkitqgejbdjgmouhjrmcsqdwfjuiobqrbwnlpipxhobuuoqmmjyfeczwwfm";
	string xsqbvc = "vxgekewaxwyoiqlibpqfdkyerkhbjcdsvydrpjztxmnnjdqxnaowsjeeovbvhdfvjofimxgsvgukuwayntnyffrbekb";
	if (string("vxgekewaxwyoiqlibpqfdkyerkhbjcdsvydrpjztxmnnjdqxnaowsjeeovbvhdfvjofimxgsvgukuwayntnyffrbekb") != string("vxgekewaxwyoiqlibpqfdkyerkhbjcdsvydrpjztxmnnjdqxnaowsjeeovbvhdfvjofimxgsvgukuwayntnyffrbekb")) {
		int xejsy;
		for (xejsy = 84; xejsy > 0; xejsy--) {
			continue;
		}
	}
	if (21302 != 21302) {
		int oh;
		for (oh = 29; oh > 0; oh--) {
			continue;
		}
	}
	if (21302 != 21302) {
		int rchaalxr;
		for (rchaalxr = 51; rchaalxr > 0; rchaalxr--) {
			continue;
		}
	}
	return 95916;
}

double gbsgtzm::gsbpgqxcve(bool oeswsenpqsh, bool hbgefrduakihcbm, bool zqluxno, double usmtrwccqhwvhpp, double pcnwndsqohjit, string fxcpemy, double ychetcas) {
	int rrpfig = 8601;
	double geuchp = 4278;
	int jdytshdy = 916;
	string tlhgwkbttirxa = "ozhwsmlmqrjfascgd";
	if (8601 == 8601) {
		int utfkgpinrl;
		for (utfkgpinrl = 56; utfkgpinrl > 0; utfkgpinrl--) {
			continue;
		}
	}
	if (4278 == 4278) {
		int ro;
		for (ro = 8; ro > 0; ro--) {
			continue;
		}
	}
	if (8601 == 8601) {
		int mr;
		for (mr = 34; mr > 0; mr--) {
			continue;
		}
	}
	return 57767;
}

gbsgtzm::gbsgtzm() {
	this->lzkfbtnqmenkp(false, 82670, 79722, 2803, true, string("ubqlifpqoqqqzedmjesemkijaqaqbcxqsdvompjlhxgkcntj"), 17946, false);
	this->tasnxrmorgfdlxoak(true, string("isexanbwkqvxnebunfevviykzjzfmdi"), 29345, string("faphtfzahvuooztsyxxjjowujisjpboshldnvclxkcgcujalyqasecczadcnmfhuafxxjm"), true, 3507, 5165, 94, true);
	this->szubjqecrlmamqlismra(true, string("xcqfkcdktiunhggcroifoshzzmliknetjwyxfosvuyfqbyhvfrtoduuuvypbkqbdfyukpmkijtufrxtv"), string("jmrkailyoems"));
	this->btatnzvrlcyqf(string("bfleobqhcwlsgcmkbebbewijjjrozbjnb"), string("prqalshhpftquiipkuzaqakionptkqmvwiiiuhbpxt"));
	this->gsbpgqxcve(false, false, true, 52435, 27396, string("nmljdzrojzhyfyiwik"), 2587);
	this->lcgkdqyahvfqvcjzvcf(12792, false, 18333);
	this->rjjxoahniyorhzwuw(true, string("ttfmkncracphgtmjvkmfkeeocjg"), true, 14279);
	this->mcsxkcmirqgocmyaxeewyd(75188, string("wfqikfbfucwulifbvcwttdeus"), false, false, 1662, 16568, string("qgslbxofdrbskcwhiowphqeabtvmdde"), string("gidckfqbshjcdulmijlcnvrxthbvhhflcobazqiqpapswemmjsnqzzmcakrvuaiqssaphsfgizizyqyftvbvg"), true, 3236);
	this->tocgkamxklhhtpku(string("xvxpggztdzfcpqhglqebjvjdjtaoyflleunenwvmfzwwu"), 6684, 3138, 22923, string("gzeqpczbpbdbiiuykfnvjpdhjgceawawphxwanbsncyrbuaynbtwbyrmsaevybkjoltcv"), 1270);
	this->qwadopubwpricdzimo(true, 34882, false);
	this->zaillmvuxtsekypuugiyj(2713, false, true, 5682);
	this->dymyphdypyznkslzuuuqtc(true, false, 9633, 38594, string("czdrpxqdnrvxnnahfauibcdnoztlvyhpc"), false, true, 6438, 8517, 112);
	this->dysxdlexwsjlibsoh(4395, 7909, 20470, 2952, string("kmhobabpph"), string("dzvxushadypayqsepbjdocfygrggdvynhycxbyrkqtfvm"), 248);
	this->bdrylesnvvnptj(5463, string("sykshxmtpoonkqzrbxsg"), true, 9638, true, string("tknjkjgmyxumwppplddacvefvpmngmihskkuofkqdmwkhypxsqy"), false);
	this->klavlgyuxu(11988, 2977, 3504, false, string("bepynmgttcaircrwacfcjzorteoukhfwabvietqfbynnyvzwpkcdmhsprrwtjaysnn"), string("uiegctgcgqiqrfywttjmgqtagjixzwlddnugdobqjfajxacwbpwfitffqcmvohvpimbdwtwkewhuowkal"), false, 1891);
	this->kxlbtshopm(14503, string("fmvzyyvpnwjmzniehbutrdlpqrzwffztgnxonnithoarjzktyjcmholeqbssaummawlkxcimlzw"), 2657, false, 1907);
	this->ouwkmzxzjrmwvir(string("vuaqzrjdbcyznbewsqspylpxhgklqxsrgweyojkrjhzalhudoupltfknaimgsvhgvkgvbwrvjl"), string("uxysgkkrhqugytztrhfobbwnv"), true, string("cokrszvzymjncnyqphogkzugtcibwogxofmjxbayacssvjoydtiowcsyzoibostqzlbqkjbfxndhjubnfjxelwldsncqkqrntz"), 2145, 34856, 3557);
	this->iucnxuuittwzfvylslcuuqre(420, 70163, 34854, 44776, string("kwbdspmwmximkfkwddtetwbvtjejkjozshhjzjflrbntpqekijbswsqxs"));
	this->mayagibmhfcuyxgpghxjl(true, 12325, 56656, false, 3142, false, true, string("ngvpuvelwvwtsxitzxofgcxogmriuamutjsffsjggorkcmspjwai"), true, string("zahmckyruvdkmlpvuwpspisarx"));
}

















































































// Junk Code By Troll Face & Thaisen's Gen
void itLnjsCURsOdyqaMsMDwKBCNbFzhQWtXMhpYHFmpgwiRLcTzuaLeFqvgrxMetVEvyqRQACFnaOQNSkBzlznpKPSWlJgJt46484002() { long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ65673559 = -445781936;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ2298471 = -317713455;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ96266973 = -397232648;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ25555797 = -510761131;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ22622118 = -619274220;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ51790158 = -693799276;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ52388925 = -165237666;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ4560599 = 89324124;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ42465411 = -725464925;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ79942054 = 15654328;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ17429895 = -460172751;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ11092107 = -788599989;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ63812260 = -582726802;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ64473965 = -143805091;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ86237720 = -188311644;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ87147776 = -553149564;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ22723189 = -943003221;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ75564887 = -166586664;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ15685875 = -602120655;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ91807860 = -881692060;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ44541611 = -999047716;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ93499905 = 31592;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ20702026 = -258150113;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ567263 = -374144484;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ97834391 = -374221517;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ12756851 = -78776795;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ45281279 = -181960987;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ11224387 = -30207769;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ50502499 = -872350342;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ9407738 = -854935931;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ11002774 = -683894687;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ63381772 = -56698682;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ13526681 = -197068420;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ49156876 = -240863292;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ39875831 = 94522485;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ55472606 = -185456960;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ49549962 = -878020116;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ38038706 = 6574265;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ98633612 = -519500029;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ5295120 = -240445655;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ9892378 = -94738184;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ34931432 = -395747061;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ70401457 = -101346982;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ58323763 = -396294991;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ42801479 = -795103293;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ47948887 = -398762242;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ76681434 = -824920443;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ79395166 = -642704609;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ75241375 = -835236563;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ72209249 = -360201791;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ12520913 = -300990537;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ52818760 = -943806678;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ9420221 = -987097731;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ6873217 = -135078638;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ1942391 = -24665369;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ72173653 = -345813528;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ81596445 = 40436657;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ95699711 = 76911835;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ27721406 = -36539615;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ9865268 = -440497426;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ6508880 = -411838290;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ41164538 = -35029898;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ54058099 = -38325534;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ33057674 = -870528994;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ68939281 = -300450985;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ54048123 = -303474070;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ97565426 = -491531569;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ14655385 = -241863511;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ24598134 = -138327576;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ30765114 = 97145315;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ37597815 = -675129449;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ84684483 = -849577487;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ76931275 = -647086635;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ10390755 = -261675001;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ81915483 = -686953876;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ9610180 = -503300656;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ23098449 = -898621427;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ62378262 = -861855123;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ57765784 = -579041191;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ49885505 = -975459276;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ36075417 = -253856353;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ65886112 = -539256378;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ35983012 = -194971207;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ78293250 = -412148552;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ96886824 = -453945394;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ58184014 = -740088009;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ53961551 = -69600952;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ6653464 = 38010217;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ47214485 = -116197924;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ67702178 = -559663988;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ73876161 = -125893618;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ53850250 = -854931952;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ10317300 = -956886121;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ88768345 = 20997396;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ98786240 = -828607366;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ68727840 = 40291713;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ80873332 = -257421527;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ37343783 = -230817988;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ89384482 = 4155994;    long HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ88753356 = -445781936;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ65673559 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ2298471;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ2298471 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ96266973;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ96266973 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ25555797;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ25555797 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ22622118;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ22622118 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ51790158;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ51790158 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ52388925;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ52388925 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ4560599;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ4560599 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ42465411;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ42465411 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ79942054;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ79942054 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ17429895;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ17429895 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ11092107;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ11092107 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ63812260;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ63812260 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ64473965;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ64473965 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ86237720;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ86237720 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ87147776;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ87147776 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ22723189;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ22723189 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ75564887;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ75564887 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ15685875;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ15685875 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ91807860;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ91807860 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ44541611;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ44541611 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ93499905;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ93499905 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ20702026;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ20702026 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ567263;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ567263 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ97834391;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ97834391 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ12756851;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ12756851 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ45281279;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ45281279 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ11224387;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ11224387 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ50502499;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ50502499 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ9407738;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ9407738 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ11002774;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ11002774 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ63381772;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ63381772 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ13526681;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ13526681 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ49156876;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ49156876 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ39875831;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ39875831 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ55472606;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ55472606 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ49549962;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ49549962 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ38038706;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ38038706 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ98633612;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ98633612 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ5295120;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ5295120 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ9892378;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ9892378 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ34931432;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ34931432 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ70401457;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ70401457 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ58323763;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ58323763 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ42801479;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ42801479 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ47948887;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ47948887 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ76681434;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ76681434 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ79395166;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ79395166 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ75241375;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ75241375 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ72209249;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ72209249 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ12520913;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ12520913 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ52818760;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ52818760 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ9420221;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ9420221 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ6873217;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ6873217 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ1942391;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ1942391 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ72173653;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ72173653 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ81596445;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ81596445 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ95699711;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ95699711 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ27721406;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ27721406 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ9865268;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ9865268 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ6508880;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ6508880 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ41164538;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ41164538 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ54058099;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ54058099 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ33057674;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ33057674 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ68939281;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ68939281 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ54048123;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ54048123 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ97565426;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ97565426 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ14655385;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ14655385 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ24598134;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ24598134 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ30765114;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ30765114 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ37597815;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ37597815 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ84684483;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ84684483 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ76931275;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ76931275 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ10390755;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ10390755 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ81915483;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ81915483 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ9610180;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ9610180 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ23098449;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ23098449 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ62378262;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ62378262 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ57765784;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ57765784 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ49885505;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ49885505 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ36075417;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ36075417 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ65886112;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ65886112 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ35983012;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ35983012 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ78293250;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ78293250 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ96886824;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ96886824 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ58184014;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ58184014 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ53961551;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ53961551 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ6653464;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ6653464 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ47214485;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ47214485 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ67702178;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ67702178 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ73876161;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ73876161 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ53850250;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ53850250 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ10317300;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ10317300 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ88768345;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ88768345 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ98786240;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ98786240 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ68727840;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ68727840 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ80873332;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ80873332 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ37343783;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ37343783 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ89384482;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ89384482 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ88753356;     HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ88753356 = HvLrnTVKCzrVYDqZiNGvMstiOliXvfrWvuSSEVfRgojAvUmLbwDvZNgPBddCkxPWHVZelYVMhpPYJtnuyXvzcwwiNDQbsdgiZ65673559; }
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void edCTiwDeUCdRFZOHFaTRqexhWcWPaEABrjPhLlnOcAARbeeuilaOcbHzfUxGZoDQBbZWJgwTnFpqefFrxbhpdlYQbPCqo65027541() { long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr94456995 = -761444174;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr10745257 = -880125954;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr33256047 = -862501876;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr1992968 = -518999386;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr63131873 = -122061793;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr70925594 = -43152905;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr70874875 = -407792774;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr58946988 = -201442990;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr95960865 = -733716975;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr26495235 = -910727386;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr8555143 = 15595877;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr25824469 = -325272509;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr49965636 = -592506213;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr4978647 = -665144921;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr62040219 = -337925847;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr58333950 = 56135024;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr72496069 = -42362990;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr11820280 = -747398695;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr97333686 = -932966245;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr35614511 = -577936357;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr45677034 = -830959970;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr32690901 = -334667954;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr24936298 = -170253731;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr77495560 = -627849586;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr72100423 = -987654677;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr83432129 = -963783654;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr62967676 = -656000763;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr33110537 = -701077780;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr59364363 = 99319076;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr6895540 = -752459188;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr10805238 = -159872597;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr80220446 = 47781005;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr74862113 = -563367200;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr96942331 = -29026385;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr66456442 = -144592915;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr23363438 = -905251015;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr55401020 = 93903947;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr35557978 = -923229913;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr32172639 = 64869359;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr2676753 = 55837736;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr75437569 = -741680898;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr34442220 = -555915378;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr63251704 = -271029161;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr466203 = -235762539;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr58779964 = -864002050;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr72649249 = -161628221;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr63920945 = -311447911;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr51022033 = -172535312;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr82404110 = 15127683;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr82745493 = 85772162;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr23924191 = -443231465;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr33484662 = -869145755;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr67253025 = -250983598;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr84384489 = -489525392;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr19350507 = -834142088;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr61766095 = -326776220;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr85808958 = -609872223;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr55760486 = -134652291;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr29892545 = -531344709;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr79699744 = -158278139;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr7957918 = -387152142;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr37764338 = -706714994;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr99582624 = -200762067;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr89065326 = -981257787;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr15689998 = -650854789;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr28334697 = 67814871;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr50962356 = -761905310;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr53023304 = -463479829;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr38522204 = -420552006;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr38676781 = -432674832;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr2932930 = 62231077;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr36938092 = -119133077;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr79647641 = -712268054;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr94656933 = -888803981;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr60176942 = -836255460;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr11234814 = -175044593;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr69439197 = 36361206;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr24470096 = -934491192;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr18715597 = -763847537;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr99451173 = -726026456;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr19511185 = -552335744;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr11945643 = -383465452;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr50706427 = -616205463;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr76618869 = -986453086;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr82971348 = -209227723;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr77320575 = -290726842;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr12967422 = -701235397;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr90477624 = 26158192;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr77591825 = -194884297;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr4690348 = -817816696;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr37554479 = -195378793;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr99640534 = -771443763;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr5665434 = -291885204;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr52472894 = -776852503;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr94718835 = -557010123;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr37673232 = 65034096;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr34859596 = -255153311;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr74186378 = -289771374;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr84776204 = -584907751;    long gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr30445268 = -761444174;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr94456995 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr10745257;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr10745257 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr33256047;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr33256047 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr1992968;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr1992968 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr63131873;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr63131873 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr70925594;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr70925594 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr70874875;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr70874875 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr58946988;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr58946988 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr95960865;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr95960865 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr26495235;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr26495235 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr8555143;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr8555143 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr25824469;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr25824469 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr49965636;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr49965636 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr4978647;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr4978647 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr62040219;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr62040219 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr58333950;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr58333950 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr72496069;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr72496069 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr11820280;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr11820280 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr97333686;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr97333686 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr35614511;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr35614511 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr45677034;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr45677034 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr32690901;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr32690901 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr24936298;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr24936298 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr77495560;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr77495560 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr72100423;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr72100423 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr83432129;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr83432129 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr62967676;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr62967676 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr33110537;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr33110537 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr59364363;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr59364363 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr6895540;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr6895540 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr10805238;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr10805238 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr80220446;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr80220446 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr74862113;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr74862113 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr96942331;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr96942331 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr66456442;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr66456442 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr23363438;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr23363438 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr55401020;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr55401020 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr35557978;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr35557978 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr32172639;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr32172639 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr2676753;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr2676753 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr75437569;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr75437569 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr34442220;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr34442220 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr63251704;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr63251704 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr466203;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr466203 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr58779964;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr58779964 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr72649249;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr72649249 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr63920945;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr63920945 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr51022033;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr51022033 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr82404110;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr82404110 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr82745493;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr82745493 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr23924191;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr23924191 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr33484662;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr33484662 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr67253025;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr67253025 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr84384489;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr84384489 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr19350507;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr19350507 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr61766095;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr61766095 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr85808958;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr85808958 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr55760486;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr55760486 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr29892545;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr29892545 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr79699744;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr79699744 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr7957918;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr7957918 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr37764338;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr37764338 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr99582624;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr99582624 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr89065326;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr89065326 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr15689998;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr15689998 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr28334697;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr28334697 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr50962356;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr50962356 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr53023304;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr53023304 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr38522204;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr38522204 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr38676781;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr38676781 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr2932930;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr2932930 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr36938092;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr36938092 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr79647641;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr79647641 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr94656933;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr94656933 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr60176942;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr60176942 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr11234814;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr11234814 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr69439197;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr69439197 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr24470096;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr24470096 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr18715597;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr18715597 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr99451173;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr99451173 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr19511185;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr19511185 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr11945643;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr11945643 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr50706427;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr50706427 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr76618869;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr76618869 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr82971348;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr82971348 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr77320575;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr77320575 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr12967422;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr12967422 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr90477624;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr90477624 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr77591825;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr77591825 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr4690348;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr4690348 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr37554479;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr37554479 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr99640534;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr99640534 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr5665434;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr5665434 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr52472894;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr52472894 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr94718835;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr94718835 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr37673232;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr37673232 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr34859596;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr34859596 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr74186378;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr74186378 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr84776204;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr84776204 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr30445268;     gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr30445268 = gonwkQzEIlBpGbXlRVzPFJyHOmQDJuteFfGQBuuPFmpqfzXKAeKuCbjXqghxzwvNrXjbUGjWakJSUHcOPFCfkycevjaVMwPJr94456995; }
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ytzWulgdaatblXjgPljuXMFEpsCMIrDGbeLNqdsKBcaAVsKLrwePlyTlnuhBiUViXmzGMWqlsYtulFiwhrUMNnVcKZZww13887507() { double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi90973152 = -273982791;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi56541923 = -811516693;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi38281751 = -82040335;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi22001886 = -527542762;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi27364213 = -543471128;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi90769750 = -223964075;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi60415860 = -170442516;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi59792133 = -462238516;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi58845040 = -579311694;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi82180014 = -201049163;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi58610956 = 20096676;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi33695068 = 73733767;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi39309876 = -317462640;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi95131649 = 97910070;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi44353922 = -207896131;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi84008499 = -86088366;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi72260538 = -45402750;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi86455501 = -616388949;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi19042528 = -13102412;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi40302888 = -181448962;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi13521178 = -86276381;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi84444525 = 92310295;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi95994062 = -771694521;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi79495276 = -646506729;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi86154085 = -849733510;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi1169456 = -88975953;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi25753570 = -332782754;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi7659138 = -52350385;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi35221111 = 6976251;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi19105112 = -279520344;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi3192978 = -268294133;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi23608701 = -88314134;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi49580341 = -902491859;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi9460582 = -461195518;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi34762262 = -229601479;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi45620597 = -592444851;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi79987303 = 83306678;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi73726112 = -746730542;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi63250147 = -469858684;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi96257705 = -125794304;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi13780730 = -679251119;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi85786742 = -966460299;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi70651960 = -487736606;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi33058361 = -598914071;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi82757653 = -609526687;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi50116291 = -37933682;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi46984141 = -349328248;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi43820266 = -581248633;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi41683983 = -284494582;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi45523821 = -470254850;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi69083146 = -183333168;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi98619671 = -710238131;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi71672229 = -546865238;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi64766550 = -123766470;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi366330 = -288414242;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi6528627 = -266293087;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi60547861 = 60177827;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi58786475 = -435533606;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi35847800 = -677809252;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi26194757 = -354495176;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi65016180 = -891181322;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi52756723 = -18092131;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi24571022 = -369214767;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi39739928 = -199791351;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi78987036 = -932755031;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi35002255 = -891589191;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi84114727 = -23774374;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi29849295 = -856267122;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi60369387 = -672488452;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi98733325 = -615451281;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi4021197 = -69395044;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi98534425 = -298672208;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi23205355 = -46530266;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi22784822 = -887308109;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi26522158 = -502197844;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi27734436 = -119816083;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi13792566 = -419953100;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi62935701 = -72780450;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi96737623 = 63019957;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi36037794 = -711799829;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi54185314 = -739647705;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi81933304 = -751534121;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi65975154 = -767855804;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi89697290 = -522768900;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi50021966 = 3812824;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi4573306 = -558056002;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi51936471 = -541448897;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi84813791 = -678725389;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi9094252 = -72781277;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi28233636 = -963308393;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi85072735 = -552622678;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi21200829 = -481159716;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi37878313 = 31078710;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi37055390 = -15363508;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi31241526 = -234612982;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi61024007 = -561158989;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi61215720 = -497245533;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi30912032 = -187945256;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi54071324 = -666159041;    double qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi47755398 = -273982791;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi90973152 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi56541923;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi56541923 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi38281751;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi38281751 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi22001886;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi22001886 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi27364213;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi27364213 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi90769750;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi90769750 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi60415860;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi60415860 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi59792133;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi59792133 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi58845040;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi58845040 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi82180014;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi82180014 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi58610956;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi58610956 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi33695068;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi33695068 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi39309876;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi39309876 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi95131649;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi95131649 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi44353922;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi44353922 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi84008499;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi84008499 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi72260538;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi72260538 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi86455501;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi86455501 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi19042528;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi19042528 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi40302888;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi40302888 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi13521178;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi13521178 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi84444525;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi84444525 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi95994062;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi95994062 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi79495276;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi79495276 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi86154085;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi86154085 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi1169456;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi1169456 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi25753570;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi25753570 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi7659138;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi7659138 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi35221111;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi35221111 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi19105112;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi19105112 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi3192978;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi3192978 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi23608701;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi23608701 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi49580341;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi49580341 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi9460582;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi9460582 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi34762262;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi34762262 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi45620597;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi45620597 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi79987303;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi79987303 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi73726112;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi73726112 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi63250147;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi63250147 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi96257705;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi96257705 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi13780730;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi13780730 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi85786742;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi85786742 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi70651960;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi70651960 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi33058361;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi33058361 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi82757653;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi82757653 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi50116291;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi50116291 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi46984141;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi46984141 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi43820266;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi43820266 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi41683983;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi41683983 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi45523821;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi45523821 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi69083146;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi69083146 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi98619671;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi98619671 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi71672229;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi71672229 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi64766550;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi64766550 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi366330;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi366330 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi6528627;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi6528627 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi60547861;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi60547861 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi58786475;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi58786475 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi35847800;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi35847800 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi26194757;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi26194757 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi65016180;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi65016180 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi52756723;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi52756723 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi24571022;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi24571022 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi39739928;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi39739928 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi78987036;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi78987036 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi35002255;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi35002255 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi84114727;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi84114727 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi29849295;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi29849295 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi60369387;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi60369387 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi98733325;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi98733325 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi4021197;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi4021197 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi98534425;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi98534425 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi23205355;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi23205355 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi22784822;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi22784822 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi26522158;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi26522158 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi27734436;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi27734436 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi13792566;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi13792566 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi62935701;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi62935701 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi96737623;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi96737623 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi36037794;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi36037794 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi54185314;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi54185314 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi81933304;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi81933304 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi65975154;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi65975154 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi89697290;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi89697290 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi50021966;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi50021966 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi4573306;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi4573306 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi51936471;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi51936471 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi84813791;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi84813791 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi9094252;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi9094252 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi28233636;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi28233636 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi85072735;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi85072735 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi21200829;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi21200829 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi37878313;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi37878313 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi37055390;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi37055390 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi31241526;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi31241526 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi61024007;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi61024007 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi61215720;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi61215720 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi30912032;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi30912032 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi54071324;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi54071324 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi47755398;     qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi47755398 = qcgHscjsEyVGsXMFZziMlUrAIVjRXvmwtpCwMmtczDctyGUQJXlarjLWkgyWtRVJLYMDVPtmFFMhrxBCASIAJMFKefLfoXjhi90973152; }
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void juaVpnSNfoVcAxPDmnhEGEMSJqoIuxQwkMXkFbkOzeMmMQuIhYbckHzehHMzyKSiXYsoZfxKCTEbrZGswfaWVoMcMsCxO32431046() { double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL19756589 = -589645029;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL64988710 = -273929192;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL75270824 = -547309563;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL98439056 = -535781017;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL67873968 = -46258701;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL9905186 = -673317704;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL78901810 = -412997623;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL14178523 = -753005630;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL12340495 = -587563744;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL28733195 = -27430877;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL49736204 = -604134697;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL48427431 = -562938753;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL25463252 = -327242051;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL35636331 = -423429761;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL20156421 = -357510333;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL55194673 = -576803778;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL22033420 = -244762518;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL22710894 = -97200980;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL690339 = -343948002;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL84109538 = -977693260;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL14656601 = 81811365;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL23635521 = -242389251;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL228335 = -683798139;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL56423574 = -900211831;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL60420117 = -363166670;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL71844734 = -973982812;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL43439968 = -806822530;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL29545288 = -723220397;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL44082975 = -121354331;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL16592914 = -177043601;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL2995442 = -844272043;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL40447375 = 16165553;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL10915775 = -168790639;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL57246036 = -249358611;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL61342874 = -468716879;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL13511429 = -212238906;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL85838361 = -44769260;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL71245385 = -576534720;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL96789172 = -985489296;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL93639338 = -929510913;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL79325921 = -226193833;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL85297530 = -26628616;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL63502207 = -657418785;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL75200800 = -438381620;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL98736138 = -678425444;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL74816654 = -900799662;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL34223652 = -935855716;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL15447134 = -111079335;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL48846718 = -534130337;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL56060066 = -24280897;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL80486424 = -325574095;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL79285573 = -635577208;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL29505033 = -910751105;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL42277822 = -478213225;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL17774445 = 2109039;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL96121068 = -247255779;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL64760375 = -590131054;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL18847250 = -647097732;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL38018939 = -72614347;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL96029233 = -72275890;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL66465218 = -866495175;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL49356522 = -689777227;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL70095547 = -531651299;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL95747580 = -310520144;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL25737753 = -183158835;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL9288830 = -520300250;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL37511657 = -294148115;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL68217215 = 22116560;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL74293457 = -954712882;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL6644993 = -45271428;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL69356311 = -432034518;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL50788034 = -668227798;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL25921722 = -111711684;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL7051000 = -414437090;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL4783618 = -651499428;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL29359070 = -891560020;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL60133313 = -584970466;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL25027535 = -145416519;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL57687436 = -121786388;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL85603463 = -462367009;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL37621082 = 61872903;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL27992835 = -595743195;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL80698569 = -89090060;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL88022909 = 2926566;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL36106490 = -851469506;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL23709868 = -108694835;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL10942342 = -73083342;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL68637952 = -690577414;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL39471591 = -151467650;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL65221805 = -121461100;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL48751053 = -622107853;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL66991112 = -397671528;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL33226446 = -403920374;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL759940 = -813213407;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL27174121 = 36984261;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL29969399 = -536416606;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL15201983 = -494977317;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL67754627 = -246898642;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL49463047 = -155222786;    double lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL89447309 = -589645029;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL19756589 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL64988710;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL64988710 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL75270824;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL75270824 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL98439056;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL98439056 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL67873968;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL67873968 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL9905186;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL9905186 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL78901810;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL78901810 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL14178523;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL14178523 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL12340495;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL12340495 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL28733195;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL28733195 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL49736204;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL49736204 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL48427431;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL48427431 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL25463252;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL25463252 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL35636331;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL35636331 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL20156421;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL20156421 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL55194673;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL55194673 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL22033420;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL22033420 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL22710894;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL22710894 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL690339;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL690339 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL84109538;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL84109538 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL14656601;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL14656601 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL23635521;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL23635521 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL228335;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL228335 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL56423574;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL56423574 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL60420117;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL60420117 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL71844734;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL71844734 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL43439968;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL43439968 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL29545288;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL29545288 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL44082975;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL44082975 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL16592914;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL16592914 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL2995442;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL2995442 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL40447375;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL40447375 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL10915775;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL10915775 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL57246036;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL57246036 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL61342874;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL61342874 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL13511429;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL13511429 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL85838361;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL85838361 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL71245385;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL71245385 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL96789172;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL96789172 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL93639338;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL93639338 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL79325921;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL79325921 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL85297530;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL85297530 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL63502207;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL63502207 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL75200800;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL75200800 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL98736138;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL98736138 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL74816654;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL74816654 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL34223652;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL34223652 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL15447134;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL15447134 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL48846718;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL48846718 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL56060066;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL56060066 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL80486424;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL80486424 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL79285573;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL79285573 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL29505033;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL29505033 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL42277822;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL42277822 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL17774445;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL17774445 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL96121068;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL96121068 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL64760375;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL64760375 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL18847250;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL18847250 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL38018939;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL38018939 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL96029233;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL96029233 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL66465218;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL66465218 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL49356522;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL49356522 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL70095547;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL70095547 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL95747580;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL95747580 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL25737753;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL25737753 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL9288830;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL9288830 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL37511657;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL37511657 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL68217215;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL68217215 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL74293457;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL74293457 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL6644993;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL6644993 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL69356311;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL69356311 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL50788034;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL50788034 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL25921722;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL25921722 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL7051000;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL7051000 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL4783618;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL4783618 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL29359070;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL29359070 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL60133313;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL60133313 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL25027535;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL25027535 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL57687436;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL57687436 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL85603463;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL85603463 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL37621082;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL37621082 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL27992835;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL27992835 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL80698569;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL80698569 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL88022909;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL88022909 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL36106490;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL36106490 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL23709868;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL23709868 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL10942342;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL10942342 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL68637952;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL68637952 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL39471591;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL39471591 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL65221805;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL65221805 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL48751053;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL48751053 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL66991112;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL66991112 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL33226446;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL33226446 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL759940;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL759940 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL27174121;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL27174121 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL29969399;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL29969399 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL15201983;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL15201983 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL67754627;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL67754627 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL49463047;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL49463047 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL89447309;     lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL89447309 = lPpMePnYGiCaRZRCYnHAUDTjwMjLPbgcAwRDOPMKTUpwYwEPvaoFvwuiwGQRfbJeCaaUINoIawhzNWLzIUQonahDBiHIzverL19756589; }
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void vymzxZYYvhJdSQyNdzUsrOoIpdLZCpFUcUgFUTgEeyOwXCaBLSeYZdKhJeJTOyWFboBxkstKSPNszskmDKOjLgvyKywpJ81291012() { float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn16272745 = -102183647;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn10785377 = -205319932;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn80296528 = -866848022;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn18447974 = -544324392;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn32106307 = -467668036;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn29749342 = -854128875;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn68442795 = -175647365;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn15023668 = 86198845;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn75224669 = -433158463;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn84417974 = -417752654;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn99792016 = -599633898;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn56298029 = -163932478;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn14807492 = -52198477;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn25789334 = -760374770;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn2470124 = -227480618;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn80869223 = -719027167;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn21797889 = -247802278;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn97346115 = 33808766;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn22399180 = -524084169;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn88797916 = -581205865;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn82500743 = -273505046;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn75389145 = -915411002;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn71286099 = -185238928;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn58423291 = -918868974;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn74473779 = -225245503;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn89582060 = -99175110;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn6225862 = -483604521;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn4093888 = -74493002;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn19939723 = -213697157;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn28802487 = -804104756;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn95383181 = -952693579;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn83835629 = -119929587;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn85634001 = -507915298;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn69764286 = -681527745;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn29648693 = -553725442;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn35768587 = -999432741;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn10424645 = -55366528;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn9413520 = -400035350;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn27866681 = -420217338;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn87220291 = -11142952;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn17669083 = -163764054;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn36642052 = -437173537;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn70902463 = -874126230;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn7792960 = -801533152;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn22713828 = -423950081;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn52283696 = -777105122;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn17286849 = -973736053;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn8245367 = -519792657;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn8126592 = -833752601;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn18838394 = -580307909;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn25645380 = -65675798;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn44420583 = -476669584;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn33924238 = -106632746;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn22659883 = -112454303;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn98790268 = -552163115;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn40883600 = -186772646;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn39499278 = 79918996;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn21873238 = -947979048;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn43974195 = -219078890;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn42524247 = -268492926;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn23523481 = -270524355;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn64348907 = -1154364;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn95083944 = -700103999;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn46422182 = -629053707;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn89034792 = -465059076;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn15956388 = -379704311;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn70664028 = -656017180;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn45043206 = -370670733;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn96140640 = -106649328;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn66701536 = -228047877;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn70444578 = -563660640;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn12384369 = -847766929;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn69479434 = -545973896;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn35178888 = -412941217;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn71128833 = -317441811;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn45858691 = -836331510;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn4486682 = 58715228;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn63493139 = -383705777;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn35709463 = -394918894;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn22190083 = -448140381;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn72295211 = -125439058;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn97980495 = -963811864;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn95967296 = -240740401;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn1101330 = -633389248;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn3157107 = -638428959;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn50962598 = -376023995;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn49911392 = 86703158;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn62974119 = -295460996;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn70974018 = -29364630;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn88765093 = -266952797;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn96269309 = -979351738;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn88551406 = -107387480;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn65439325 = -80956460;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn85342434 = -51724412;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn63696811 = -740618597;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn53320175 = -62609691;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn41558108 = -737069539;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn24480281 = -145072523;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn18758167 = -236474077;    float MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn6757440 = -102183647;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn16272745 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn10785377;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn10785377 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn80296528;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn80296528 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn18447974;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn18447974 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn32106307;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn32106307 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn29749342;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn29749342 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn68442795;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn68442795 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn15023668;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn15023668 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn75224669;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn75224669 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn84417974;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn84417974 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn99792016;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn99792016 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn56298029;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn56298029 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn14807492;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn14807492 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn25789334;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn25789334 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn2470124;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn2470124 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn80869223;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn80869223 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn21797889;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn21797889 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn97346115;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn97346115 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn22399180;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn22399180 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn88797916;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn88797916 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn82500743;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn82500743 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn75389145;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn75389145 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn71286099;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn71286099 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn58423291;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn58423291 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn74473779;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn74473779 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn89582060;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn89582060 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn6225862;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn6225862 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn4093888;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn4093888 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn19939723;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn19939723 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn28802487;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn28802487 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn95383181;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn95383181 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn83835629;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn83835629 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn85634001;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn85634001 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn69764286;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn69764286 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn29648693;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn29648693 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn35768587;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn35768587 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn10424645;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn10424645 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn9413520;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn9413520 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn27866681;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn27866681 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn87220291;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn87220291 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn17669083;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn17669083 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn36642052;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn36642052 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn70902463;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn70902463 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn7792960;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn7792960 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn22713828;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn22713828 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn52283696;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn52283696 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn17286849;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn17286849 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn8245367;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn8245367 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn8126592;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn8126592 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn18838394;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn18838394 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn25645380;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn25645380 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn44420583;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn44420583 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn33924238;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn33924238 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn22659883;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn22659883 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn98790268;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn98790268 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn40883600;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn40883600 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn39499278;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn39499278 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn21873238;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn21873238 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn43974195;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn43974195 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn42524247;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn42524247 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn23523481;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn23523481 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn64348907;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn64348907 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn95083944;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn95083944 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn46422182;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn46422182 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn89034792;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn89034792 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn15956388;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn15956388 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn70664028;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn70664028 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn45043206;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn45043206 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn96140640;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn96140640 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn66701536;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn66701536 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn70444578;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn70444578 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn12384369;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn12384369 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn69479434;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn69479434 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn35178888;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn35178888 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn71128833;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn71128833 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn45858691;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn45858691 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn4486682;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn4486682 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn63493139;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn63493139 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn35709463;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn35709463 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn22190083;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn22190083 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn72295211;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn72295211 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn97980495;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn97980495 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn95967296;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn95967296 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn1101330;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn1101330 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn3157107;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn3157107 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn50962598;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn50962598 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn49911392;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn49911392 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn62974119;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn62974119 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn70974018;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn70974018 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn88765093;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn88765093 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn96269309;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn96269309 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn88551406;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn88551406 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn65439325;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn65439325 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn85342434;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn85342434 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn63696811;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn63696811 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn53320175;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn53320175 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn41558108;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn41558108 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn24480281;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn24480281 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn18758167;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn18758167 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn6757440;     MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn6757440 = MqfcJUjBcQTZTFJPRhQuXQEkmuvsDnmwEBGZiPqYBlatvKrblisQfvmwmvpOYqxpeXUSzkhBrsslqKpcDTHVuVQJOJnnaczLn16272745; }
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void hPUHwpTpdUSUDoBgUwNrQbexNJhXtbqNNYnRDAJAobmPQUzGtYBnJKLGlXtnIwwCeEGfBjcUnNCooPgsJbeUZCDDMhggG99834550() { float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS45056182 = -417845885;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS19232163 = -767732431;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS17285602 = -232117250;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS94885144 = -552562647;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS72616062 = 29544391;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS48884778 = -203482503;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS86928745 = -418202473;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS69410057 = -204568269;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS28720124 = -441410513;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS30971155 = -244134368;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS90917264 = -123865270;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS71030392 = -800604998;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS960868 = -61977888;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS66294015 = -181714600;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS78272622 = -377094820;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS52055396 = -109742579;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS71570769 = -447162046;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS33601508 = -547003265;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS4046991 = -854929758;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS32604566 = -277450162;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS83636167 = -105417300;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS14580140 = -150110547;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS75520371 = -97342547;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS35351589 = -72574076;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS48739811 = -838678663;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS60257339 = -984181970;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS23912260 = -957644297;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS25980038 = -745363013;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS28801587 = -342027738;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS26290289 = -701628013;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS95185645 = -428671488;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS674304 = -15449900;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS46969435 = -874214077;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS17549741 = -469690838;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS56229305 = -792840843;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS3659419 = -619226797;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS16275703 = -183442466;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS6932793 = -229839528;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS61405707 = -935847950;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS84601924 = -814859561;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS83214273 = -810706768;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS36152841 = -597341854;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS63752710 = 56191591;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS49935398 = -641000701;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS38692313 = -492848838;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS76984059 = -539971102;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS4526360 = -460263521;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS79872233 = -49623359;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS15289327 = 16611644;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS29374638 = -134333957;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS37048659 = -207916726;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS25086486 = -402008661;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS91757041 = -470518613;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS171155 = -466901057;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS16198384 = -261639835;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS30476042 = -167735338;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS43711792 = -570389885;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS81934013 = -59543174;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS46145334 = -713883985;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS12358723 = 13726360;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS24972519 = -245838207;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS60948707 = -672839460;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS40608471 = -862540531;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS2429835 = -739782500;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS35785509 = -815462880;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS90242961 = -8415371;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS24060958 = -926390921;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS83411126 = -592287051;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS10064711 = -388873758;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS74613203 = -757868024;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS35779693 = -926300114;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS64637977 = -117322519;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS72195801 = -611155315;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS19445066 = 59929802;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS49390292 = -466743395;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS47483326 = -508075446;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS50827430 = -106302139;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS25584973 = -456341846;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS96659275 = -579725239;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS71755752 = -198707561;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS55730979 = -423918449;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS44040026 = -808020938;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS10690712 = -661974658;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS99426949 = -107693782;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS89241630 = -393711288;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS70099160 = 73337172;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS8917262 = -544931288;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS46798280 = -307313021;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS1351358 = -108051003;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS25753263 = -525105505;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS59947627 = 51163087;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS34341690 = -23899292;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS60787458 = -515955544;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS49046984 = -849574311;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS59629406 = -469021354;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS22265567 = -37867308;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS95544370 = -734801323;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS61322875 = -204025909;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS14149890 = -825537821;    float VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS48449351 = -417845885;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS45056182 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS19232163;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS19232163 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS17285602;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS17285602 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS94885144;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS94885144 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS72616062;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS72616062 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS48884778;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS48884778 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS86928745;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS86928745 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS69410057;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS69410057 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS28720124;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS28720124 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS30971155;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS30971155 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS90917264;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS90917264 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS71030392;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS71030392 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS960868;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS960868 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS66294015;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS66294015 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS78272622;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS78272622 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS52055396;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS52055396 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS71570769;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS71570769 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS33601508;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS33601508 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS4046991;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS4046991 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS32604566;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS32604566 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS83636167;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS83636167 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS14580140;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS14580140 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS75520371;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS75520371 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS35351589;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS35351589 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS48739811;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS48739811 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS60257339;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS60257339 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS23912260;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS23912260 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS25980038;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS25980038 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS28801587;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS28801587 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS26290289;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS26290289 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS95185645;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS95185645 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS674304;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS674304 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS46969435;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS46969435 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS17549741;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS17549741 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS56229305;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS56229305 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS3659419;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS3659419 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS16275703;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS16275703 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS6932793;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS6932793 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS61405707;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS61405707 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS84601924;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS84601924 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS83214273;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS83214273 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS36152841;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS36152841 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS63752710;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS63752710 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS49935398;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS49935398 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS38692313;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS38692313 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS76984059;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS76984059 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS4526360;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS4526360 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS79872233;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS79872233 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS15289327;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS15289327 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS29374638;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS29374638 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS37048659;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS37048659 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS25086486;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS25086486 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS91757041;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS91757041 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS171155;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS171155 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS16198384;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS16198384 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS30476042;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS30476042 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS43711792;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS43711792 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS81934013;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS81934013 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS46145334;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS46145334 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS12358723;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS12358723 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS24972519;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS24972519 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS60948707;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS60948707 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS40608471;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS40608471 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS2429835;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS2429835 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS35785509;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS35785509 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS90242961;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS90242961 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS24060958;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS24060958 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS83411126;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS83411126 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS10064711;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS10064711 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS74613203;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS74613203 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS35779693;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS35779693 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS64637977;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS64637977 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS72195801;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS72195801 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS19445066;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS19445066 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS49390292;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS49390292 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS47483326;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS47483326 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS50827430;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS50827430 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS25584973;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS25584973 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS96659275;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS96659275 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS71755752;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS71755752 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS55730979;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS55730979 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS44040026;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS44040026 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS10690712;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS10690712 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS99426949;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS99426949 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS89241630;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS89241630 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS70099160;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS70099160 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS8917262;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS8917262 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS46798280;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS46798280 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS1351358;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS1351358 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS25753263;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS25753263 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS59947627;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS59947627 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS34341690;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS34341690 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS60787458;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS60787458 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS49046984;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS49046984 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS59629406;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS59629406 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS22265567;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS22265567 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS95544370;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS95544370 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS61322875;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS61322875 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS14149890;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS14149890 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS48449351;     VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS48449351 = VrsfklMOTEJvhAwIXRWqAdhRbcbACSUDwNRVKJDdUdpXVZsBHvpPKpFrKaEIJLLBjGyTjbxNOsVWnKwBjatHcFsHIaKjFvUJS45056182; }
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void eQoPJdAxtycVcTYtquOpRfoJQOlGDWxkUQsiWhSTiardlqVReWYEqDZycfzvrtuWAsALapWIcuHroLYZtUbhxsuRruGri18378090() { int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke73839618 = -733508123;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke27678949 = -230144930;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke54274674 = -697386478;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke71322316 = -560800902;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke13125818 = -573243182;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke68020214 = -652836132;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke5414696 = -660757581;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23796447 = -495335383;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82215577 = -449662563;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke77524334 = -70516082;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82042513 = -748096643;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke85762755 = -337277518;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke87114242 = -71757299;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke6798697 = -703054431;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke54075122 = -526709023;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23241570 = -600457991;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke21343650 = -646521815;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke69856900 = -27815296;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke85694802 = -85775348;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke76411216 = 26305540;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke84771590 = 62670446;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke53771135 = -484810093;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke79754644 = -9446165;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke12279887 = -326279178;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23005843 = -352111823;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke30932618 = -769188829;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke41598657 = -331684074;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke47866188 = -316233025;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke37663451 = -470358320;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23778091 = -599151270;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke94988109 = 95350602;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke17512978 = 89029787;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke8304869 = -140512857;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke65335196 = -257853931;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82809916 = 68043757;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke71550251 = -239020852;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke22126762 = -311518403;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke4452065 = -59643706;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke94944733 = -351478563;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke81983557 = -518576170;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke48759465 = -357649481;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke35663630 = -757510171;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke56602957 = -113490588;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke92077837 = -480468249;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke54670799 = -561747595;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke1684422 = -302837082;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke91765870 = 53209011;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke51499101 = -679454062;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke22452062 = -233024111;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke39910882 = -788360004;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke48451937 = -350157653;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke5752388 = -327347738;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke49589846 = -834404480;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke77682427 = -821347811;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke33606499 = 28883446;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke20068484 = -148698031;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke47924305 = -120698765;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke41994788 = -271107300;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke48316473 = -108689080;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82193199 = -804054354;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke26421557 = -221152059;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke57548507 = -244524556;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke86132996 = 75022936;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke58437486 = -850511294;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82536225 = -65866684;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke64529536 = -737126430;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke77457887 = -96764661;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke21779046 = -813903369;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23988781 = -671098188;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82524870 = -187688171;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke1114808 = -188939588;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke16891585 = -486878109;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke74912167 = -676336734;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke3711245 = -567199178;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke27651751 = -616044979;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke49107961 = -179819383;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke97168178 = -271319505;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke87676806 = -528977916;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke57609087 = -764531584;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke21321422 = 50725258;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke39166747 = -722397841;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke90099556 = -652230012;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke25414127 = 16791086;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke97752568 = -681998317;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke75326154 = -148993617;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke89235721 = -577301661;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke67923131 = -76565733;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke30622441 = -319165046;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke31728697 = -186737377;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke62741433 = -783258213;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23625946 = -18322088;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke80131974 = 59588896;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke56135592 = -950954627;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke12751534 = -547424209;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke55562001 = -197424111;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke91210957 = -13124925;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke49530633 = -732533108;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke98165470 = -262979295;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke9541612 = -314601566;    int XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke90141263 = -733508123;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke73839618 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke27678949;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke27678949 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke54274674;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke54274674 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke71322316;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke71322316 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke13125818;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke13125818 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke68020214;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke68020214 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke5414696;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke5414696 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23796447;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23796447 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82215577;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82215577 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke77524334;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke77524334 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82042513;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82042513 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke85762755;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke85762755 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke87114242;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke87114242 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke6798697;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke6798697 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke54075122;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke54075122 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23241570;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23241570 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke21343650;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke21343650 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke69856900;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke69856900 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke85694802;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke85694802 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke76411216;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke76411216 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke84771590;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke84771590 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke53771135;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke53771135 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke79754644;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke79754644 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke12279887;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke12279887 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23005843;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23005843 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke30932618;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke30932618 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke41598657;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke41598657 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke47866188;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke47866188 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke37663451;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke37663451 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23778091;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23778091 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke94988109;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke94988109 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke17512978;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke17512978 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke8304869;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke8304869 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke65335196;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke65335196 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82809916;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82809916 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke71550251;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke71550251 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke22126762;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke22126762 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke4452065;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke4452065 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke94944733;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke94944733 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke81983557;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke81983557 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke48759465;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke48759465 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke35663630;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke35663630 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke56602957;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke56602957 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke92077837;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke92077837 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke54670799;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke54670799 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke1684422;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke1684422 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke91765870;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke91765870 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke51499101;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke51499101 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke22452062;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke22452062 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke39910882;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke39910882 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke48451937;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke48451937 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke5752388;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke5752388 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke49589846;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke49589846 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke77682427;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke77682427 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke33606499;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke33606499 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke20068484;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke20068484 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke47924305;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke47924305 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke41994788;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke41994788 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke48316473;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke48316473 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82193199;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82193199 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke26421557;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke26421557 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke57548507;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke57548507 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke86132996;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke86132996 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke58437486;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke58437486 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82536225;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82536225 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke64529536;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke64529536 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke77457887;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke77457887 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke21779046;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke21779046 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23988781;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23988781 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82524870;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke82524870 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke1114808;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke1114808 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke16891585;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke16891585 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke74912167;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke74912167 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke3711245;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke3711245 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke27651751;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke27651751 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke49107961;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke49107961 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke97168178;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke97168178 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke87676806;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke87676806 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke57609087;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke57609087 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke21321422;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke21321422 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke39166747;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke39166747 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke90099556;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke90099556 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke25414127;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke25414127 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke97752568;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke97752568 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke75326154;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke75326154 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke89235721;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke89235721 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke67923131;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke67923131 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke30622441;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke30622441 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke31728697;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke31728697 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke62741433;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke62741433 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23625946;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke23625946 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke80131974;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke80131974 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke56135592;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke56135592 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke12751534;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke12751534 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke55562001;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke55562001 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke91210957;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke91210957 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke49530633;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke49530633 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke98165470;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke98165470 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke9541612;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke9541612 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke90141263;     XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke90141263 = XbogUcspJqozQcvwhKRGZBXRNZHyjUgjfaESSLTifkjdnhhQyVJkokIVDWTvUuGpJFUqXcdWqibpSuefsrzOKwuuRjPwrYOke73839618; }
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void bugPxMWiMIEsdWPczCKwCCjwkbWGKPGoMcuPqPEvobQcydYTKdFdPIKZKkfVqQXwCPfAczmjxPIamHxGDDvgKcZERUFGk36921629() { int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj2623056 = 50829638;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj36125736 = -792557428;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj91263746 = -62655706;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj47759487 = -569039157;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj53635573 = -76030755;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj87155650 = -2189761;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj23900646 = -903312688;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj78182837 = -786102497;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj35711032 = -457914614;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj24077515 = -996897796;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj73167761 = -272328015;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj495119 = -973950038;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj73267617 = -81536710;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj47303378 = -124394261;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj29877621 = -676323225;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj94427742 = 8826597;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj71116530 = -845881583;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj6112293 = -608627327;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj67342613 = -416620937;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj20217866 = -769938757;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj85907014 = -869241808;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj92962130 = -819509639;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj83988916 = 78450217;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj89208184 = -579984281;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj97271874 = -965544983;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj1607897 = -554195688;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj59285055 = -805723850;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj69752338 = -987103037;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj46525315 = -598688902;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj21265894 = -496674527;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj94790573 = -480627308;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj34351651 = -906490526;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj69640301 = -506811636;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj13120652 = -46017024;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj9390528 = -171071644;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj39441083 = -958814908;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj27977820 = -439594341;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj1971338 = -989447884;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj28483759 = -867109175;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj79365191 = -222292779;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj14304656 = 95407805;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj35174419 = -917678488;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj49453204 = -283172767;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj34220277 = -319935798;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj70649284 = -630646352;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj26384784 = -65703062;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj79005381 = -533318457;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj23125968 = -209284764;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj29614797 = -482659866;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj50447126 = -342386051;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj59855215 = -492398581;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj86418289 = -252686815;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj7422650 = -98290348;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj55193700 = -75794565;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj51014614 = -780593274;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj9660925 = -129660724;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj52136819 = -771007646;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj2055563 = -482671426;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj50487612 = -603494174;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj52027676 = -521835068;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj27870596 = -196465911;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj54148307 = -916209652;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj31657522 = -87413596;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj14445139 = -961240087;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj29286942 = -416270488;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj38816110 = -365837490;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj30854817 = -367138402;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj60146966 = 64480313;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj37912850 = -953322618;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj90436538 = -717508318;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj66449922 = -551579062;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj69145193 = -856433699;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj77628534 = -741518152;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj87977422 = -94328159;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj5913210 = -765346563;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj50732596 = -951563320;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj43508926 = -436336872;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj49768640 = -601613986;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj18558900 = -949337929;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj70887090 = -799841922;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj22602515 = 79122768;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj36159087 = -496439086;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj40137542 = -404443171;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj96078188 = -156302851;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj61410678 = 95724053;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj8372284 = -127940493;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj26929002 = -708200179;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj14446602 = -331017071;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj62106037 = -265423750;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj99729603 = 58589080;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj87304263 = -87807263;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj25922258 = -956922915;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj51483725 = -285953710;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj76456082 = -245274108;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj51494595 = 74173132;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj60156349 = 11617457;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj3516897 = -730264893;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj35008065 = -321932681;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj4933335 = -903665310;    int jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj31833175 = 50829638;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj2623056 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj36125736;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj36125736 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj91263746;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj91263746 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj47759487;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj47759487 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj53635573;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj53635573 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj87155650;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj87155650 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj23900646;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj23900646 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj78182837;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj78182837 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj35711032;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj35711032 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj24077515;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj24077515 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj73167761;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj73167761 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj495119;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj495119 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj73267617;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj73267617 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj47303378;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj47303378 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj29877621;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj29877621 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj94427742;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj94427742 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj71116530;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj71116530 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj6112293;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj6112293 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj67342613;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj67342613 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj20217866;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj20217866 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj85907014;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj85907014 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj92962130;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj92962130 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj83988916;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj83988916 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj89208184;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj89208184 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj97271874;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj97271874 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj1607897;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj1607897 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj59285055;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj59285055 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj69752338;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj69752338 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj46525315;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj46525315 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj21265894;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj21265894 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj94790573;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj94790573 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj34351651;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj34351651 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj69640301;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj69640301 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj13120652;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj13120652 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj9390528;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj9390528 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj39441083;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj39441083 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj27977820;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj27977820 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj1971338;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj1971338 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj28483759;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj28483759 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj79365191;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj79365191 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj14304656;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj14304656 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj35174419;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj35174419 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj49453204;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj49453204 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj34220277;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj34220277 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj70649284;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj70649284 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj26384784;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj26384784 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj79005381;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj79005381 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj23125968;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj23125968 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj29614797;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj29614797 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj50447126;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj50447126 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj59855215;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj59855215 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj86418289;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj86418289 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj7422650;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj7422650 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj55193700;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj55193700 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj51014614;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj51014614 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj9660925;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj9660925 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj52136819;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj52136819 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj2055563;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj2055563 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj50487612;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj50487612 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj52027676;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj52027676 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj27870596;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj27870596 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj54148307;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj54148307 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj31657522;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj31657522 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj14445139;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj14445139 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj29286942;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj29286942 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj38816110;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj38816110 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj30854817;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj30854817 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj60146966;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj60146966 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj37912850;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj37912850 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj90436538;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj90436538 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj66449922;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj66449922 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj69145193;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj69145193 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj77628534;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj77628534 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj87977422;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj87977422 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj5913210;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj5913210 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj50732596;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj50732596 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj43508926;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj43508926 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj49768640;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj49768640 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj18558900;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj18558900 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj70887090;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj70887090 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj22602515;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj22602515 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj36159087;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj36159087 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj40137542;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj40137542 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj96078188;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj96078188 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj61410678;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj61410678 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj8372284;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj8372284 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj26929002;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj26929002 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj14446602;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj14446602 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj62106037;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj62106037 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj99729603;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj99729603 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj87304263;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj87304263 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj25922258;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj25922258 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj51483725;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj51483725 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj76456082;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj76456082 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj51494595;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj51494595 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj60156349;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj60156349 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj3516897;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj3516897 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj35008065;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj35008065 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj4933335;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj4933335 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj31833175;     jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj31833175 = jFXDTaiyXqLrVfAzkmDmYSmUrlZqaiHugyOJkWSgOHfMkppxYEhhBTMRSDMhBsYpiQLWQrONUWXQmakCwbSVBZsMmxrhKkfWj2623056; }
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void sDmOjEAJSxGRrCYFGoGgpltgnbPZHEkWQrZtAuINKaAMk97020633() {     int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI23147116 = -709651406;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI62918423 = -958929452;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI98259151 = 68223512;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI95787853 = -760751900;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI70724552 = 58293936;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI66140122 = -796751217;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI86159164 = -20979301;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI4394956 = -209746437;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI77320384 = -66197478;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI64542670 = -245579938;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI20888254 = -514991532;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI41680757 = 16601941;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI80430840 = -242910063;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI10964880 = -467246868;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI58694720 = -478020390;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI22894591 = -940236366;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI73941120 = -178216949;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI91587185 = 35380133;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI65875869 = -850602841;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI87713204 = -189303250;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI45231274 = 88362022;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI55347562 = -362946164;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI20731526 = 48630571;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI54790778 = -253915631;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI77191807 = -476358892;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI69495656 = -865844290;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI81656940 = 11511710;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI7628611 = -756281004;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI51144562 = -502554486;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI27933165 = 74638509;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI68051064 = -181263908;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI53547494 = -543990511;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI61048220 = -171731245;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI68445074 = -319825883;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI5469291 = -518545216;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI76638940 = -449532308;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI27083495 = -98672154;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI290779 = -97195062;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI35329904 = -307919742;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI52568365 = -809386522;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI34274674 = -628309784;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI62025557 = 62132769;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI31639687 = -583898454;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI70962986 = -471207201;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI80148763 = 29358635;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI7027864 = -537629475;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI15719453 = -837323595;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI2606817 = -634111081;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI3805607 = -46868517;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI92019635 = -310826612;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI92127504 = -108597731;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI45259163 = -838853335;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI85148736 = -244620024;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI38329669 = -991511572;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI51789359 = -847473977;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI67799554 = -246705243;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI42186898 = -907560023;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI43468373 = -677860858;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI18596047 = -184393008;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI1228896 = -75861775;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI84483181 = -708262928;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI78530553 = -264698297;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI53250393 = -707191952;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI49387220 = -40835987;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI96491606 = 35683969;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI67340759 = -971001021;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI80632537 = -811666814;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI11985767 = -923084180;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI5495589 = -948701653;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI82055779 = 71511918;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI95811095 = -741564212;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI73650342 = 18978112;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI56257281 = -656700126;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI13307504 = 58783681;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI53438530 = -560993466;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI83205717 = -973770748;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI23707875 = -779047711;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI49768539 = -480162228;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI74642015 = -183274266;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI70163943 = -938729418;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI53776204 = 71479305;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI79050123 = -354377209;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI3823004 = -609412488;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI59124927 = -91727874;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI35805660 = -816763760;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI22791902 = -342410573;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI68398757 = -199370488;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI22718552 = -180219674;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI16655715 = -472351907;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI37669737 = -171839973;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI34452043 = -541972286;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI83615122 = -420811296;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI81694732 = -912802054;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI34101009 = -132057967;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI68085184 = -1123595;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI55744120 = -263611487;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI8775164 = -230675280;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI82252467 = -443062467;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI74471380 = -406891171;    int lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI12808004 = -709651406;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI23147116 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI62918423;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI62918423 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI98259151;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI98259151 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI95787853;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI95787853 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI70724552;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI70724552 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI66140122;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI66140122 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI86159164;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI86159164 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI4394956;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI4394956 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI77320384;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI77320384 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI64542670;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI64542670 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI20888254;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI20888254 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI41680757;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI41680757 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI80430840;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI80430840 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI10964880;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI10964880 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI58694720;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI58694720 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI22894591;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI22894591 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI73941120;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI73941120 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI91587185;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI91587185 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI65875869;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI65875869 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI87713204;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI87713204 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI45231274;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI45231274 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI55347562;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI55347562 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI20731526;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI20731526 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI54790778;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI54790778 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI77191807;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI77191807 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI69495656;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI69495656 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI81656940;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI81656940 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI7628611;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI7628611 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI51144562;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI51144562 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI27933165;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI27933165 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI68051064;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI68051064 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI53547494;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI53547494 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI61048220;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI61048220 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI68445074;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI68445074 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI5469291;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI5469291 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI76638940;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI76638940 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI27083495;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI27083495 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI290779;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI290779 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI35329904;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI35329904 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI52568365;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI52568365 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI34274674;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI34274674 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI62025557;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI62025557 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI31639687;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI31639687 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI70962986;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI70962986 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI80148763;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI80148763 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI7027864;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI7027864 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI15719453;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI15719453 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI2606817;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI2606817 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI3805607;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI3805607 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI92019635;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI92019635 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI92127504;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI92127504 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI45259163;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI45259163 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI85148736;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI85148736 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI38329669;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI38329669 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI51789359;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI51789359 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI67799554;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI67799554 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI42186898;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI42186898 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI43468373;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI43468373 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI18596047;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI18596047 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI1228896;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI1228896 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI84483181;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI84483181 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI78530553;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI78530553 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI53250393;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI53250393 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI49387220;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI49387220 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI96491606;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI96491606 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI67340759;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI67340759 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI80632537;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI80632537 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI11985767;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI11985767 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI5495589;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI5495589 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI82055779;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI82055779 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI95811095;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI95811095 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI73650342;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI73650342 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI56257281;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI56257281 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI13307504;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI13307504 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI53438530;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI53438530 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI83205717;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI83205717 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI23707875;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI23707875 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI49768539;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI49768539 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI74642015;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI74642015 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI70163943;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI70163943 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI53776204;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI53776204 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI79050123;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI79050123 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI3823004;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI3823004 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI59124927;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI59124927 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI35805660;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI35805660 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI22791902;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI22791902 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI68398757;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI68398757 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI22718552;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI22718552 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI16655715;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI16655715 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI37669737;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI37669737 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI34452043;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI34452043 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI83615122;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI83615122 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI81694732;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI81694732 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI34101009;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI34101009 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI68085184;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI68085184 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI55744120;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI55744120 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI8775164;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI8775164 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI82252467;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI82252467 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI74471380;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI74471380 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI12808004;     lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI12808004 = lWVAsqceuHHKGaHobBOMjBzdeEMftWPlgvCEuZONI23147116;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ZLElUtihwyfiDqsLEtfSaphgpEKFAGpdhUXslaaKxDWzlLAkrbwTpfjkdYvQicgqIBBjSAZdSVUXThueDaOZ66283280() {     float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe99620498 = -1656392;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe40670005 = -842390681;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe55375674 = -892942940;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe91137871 = -331908902;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe9730605 = -825569886;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe21735551 = -787161617;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe20554121 = -788458331;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe55039714 = -570584176;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe2988517 = -505669301;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe6777639 = -763003041;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe80576120 = -675841843;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe27773192 = -140742936;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe93163555 = -45313394;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe47819887 = -2003770;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe6762644 = -293340367;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe47915126 = -629160296;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe95148323 = -947448780;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe83209341 = -698441063;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe31947828 = -418541179;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe22494284 = -231926678;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe76394659 = -67033019;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe60150262 = -305137378;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe85111959 = -873619126;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe15321824 = -303515574;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe11096436 = -423516243;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe58599496 = -50841567;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe41794259 = -220536469;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe14159199 = -933264591;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe45278375 = -322780436;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe56230586 = -430049340;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe87628197 = -229410798;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe95792358 = -927286751;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe96749773 = -21824850;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe35781503 = -164107187;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe61986679 = 84115960;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe54956537 = -201826758;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe840924 = -598542560;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe52164719 = -550298180;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe64136797 = -268614332;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe75161852 = -179959966;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe38537755 = -668971149;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe8051217 = -329761436;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe50148949 = -747935713;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe7668194 = -732175258;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe62057664 = 23522707;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe21781929 = -765873655;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe71710172 = -186028810;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe95565668 = -104640120;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe12541661 = -463597059;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe62931486 = -323413534;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe62332548 = -698980062;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe38303605 = -220370273;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe95823778 = -444157625;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe28895684 = -794367042;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe89888620 = 17045954;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe39470237 = -696519015;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe55558045 = -968771556;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe40053851 = -489427367;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe80041436 = -908392659;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe51131109 = -674728320;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe79941291 = -466625148;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe6394922 = -855193740;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe9761339 = -147803740;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe46757931 = 24380039;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe19149441 = -433592243;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe84783761 = -748555093;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe31023419 = -18918086;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe57382052 = -881206208;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe85833208 = 13880270;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe51806106 = 8486391;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe47074203 = 69382263;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe42983604 = -297150601;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe19072544 = -329826732;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe56785976 = -138581214;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe83956529 = -562955529;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe68343442 = -737271583;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe10001314 = -557201665;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe77443766 = -41443868;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe53264159 = -227038281;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe89314506 = -657642589;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe86889323 = -864812757;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe46228591 = -15896350;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe1617539 = -369667533;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe82346889 = -999366902;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe93898038 = -731069279;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe49324593 = 90959474;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe99968580 = -383129126;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe67854090 = -227457809;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe45892882 = -81153141;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe22516443 = -219365025;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe99398491 = -233055203;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe60787072 = -9115193;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe72123283 = -641905521;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe13005689 = -593886012;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe95220560 = -713334818;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe32142833 = -813777410;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe98289878 = -81957696;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe3391018 = -672315753;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe88518752 = -198583015;    float KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe77273903 = -1656392;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe99620498 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe40670005;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe40670005 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe55375674;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe55375674 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe91137871;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe91137871 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe9730605;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe9730605 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe21735551;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe21735551 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe20554121;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe20554121 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe55039714;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe55039714 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe2988517;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe2988517 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe6777639;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe6777639 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe80576120;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe80576120 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe27773192;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe27773192 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe93163555;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe93163555 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe47819887;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe47819887 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe6762644;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe6762644 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe47915126;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe47915126 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe95148323;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe95148323 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe83209341;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe83209341 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe31947828;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe31947828 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe22494284;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe22494284 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe76394659;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe76394659 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe60150262;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe60150262 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe85111959;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe85111959 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe15321824;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe15321824 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe11096436;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe11096436 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe58599496;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe58599496 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe41794259;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe41794259 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe14159199;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe14159199 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe45278375;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe45278375 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe56230586;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe56230586 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe87628197;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe87628197 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe95792358;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe95792358 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe96749773;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe96749773 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe35781503;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe35781503 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe61986679;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe61986679 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe54956537;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe54956537 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe840924;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe840924 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe52164719;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe52164719 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe64136797;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe64136797 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe75161852;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe75161852 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe38537755;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe38537755 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe8051217;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe8051217 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe50148949;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe50148949 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe7668194;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe7668194 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe62057664;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe62057664 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe21781929;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe21781929 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe71710172;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe71710172 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe95565668;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe95565668 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe12541661;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe12541661 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe62931486;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe62931486 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe62332548;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe62332548 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe38303605;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe38303605 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe95823778;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe95823778 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe28895684;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe28895684 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe89888620;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe89888620 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe39470237;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe39470237 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe55558045;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe55558045 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe40053851;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe40053851 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe80041436;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe80041436 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe51131109;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe51131109 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe79941291;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe79941291 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe6394922;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe6394922 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe9761339;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe9761339 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe46757931;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe46757931 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe19149441;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe19149441 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe84783761;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe84783761 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe31023419;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe31023419 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe57382052;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe57382052 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe85833208;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe85833208 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe51806106;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe51806106 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe47074203;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe47074203 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe42983604;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe42983604 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe19072544;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe19072544 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe56785976;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe56785976 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe83956529;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe83956529 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe68343442;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe68343442 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe10001314;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe10001314 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe77443766;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe77443766 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe53264159;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe53264159 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe89314506;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe89314506 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe86889323;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe86889323 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe46228591;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe46228591 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe1617539;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe1617539 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe82346889;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe82346889 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe93898038;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe93898038 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe49324593;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe49324593 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe99968580;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe99968580 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe67854090;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe67854090 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe45892882;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe45892882 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe22516443;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe22516443 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe99398491;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe99398491 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe60787072;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe60787072 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe72123283;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe72123283 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe13005689;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe13005689 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe95220560;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe95220560 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe32142833;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe32142833 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe98289878;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe98289878 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe3391018;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe3391018 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe88518752;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe88518752 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe77273903;     KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe77273903 = KWoBknOMeKcVWfqkKRRrBSAULMQwdRMHOHfmNzyelNzIszBnrKTlJWXsWTqYWSiNyVGOAEWLSmFoSrbMKdFTxaxrkmPtIaISe99620498;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void MvlkFNRXcEJxseWrXzDYEkWPBmQlszRaPLBiHfuKfDKqppwzOQT70331759() {     long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH47307209 = -564261213;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH79260958 = -184736980;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH26159730 = -817404471;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH2590649 = -355098061;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH12646954 = -240823804;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH75598259 = -177934794;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH20736795 = -615650486;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH85905107 = -492743475;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH16531277 = -243712110;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH779183 = -722447865;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH59299040 = -192196819;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH6279103 = -629154483;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH35669352 = -398766551;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH21092324 = -287997367;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH1614122 = 59597415;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH89031762 = -543766640;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH94509024 = -327128128;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH76372 = -499986043;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH76586112 = 35374940;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH49505594 = -884318034;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH46257333 = -245748997;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH14910100 = -717624987;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH63697320 = -463244126;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH20749625 = -39870676;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH34956376 = -520587373;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH35315096 = -504934944;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH69355971 = -128944729;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH16505400 = -743861668;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH36889548 = -887710947;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH75085140 = -874929619;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH24109207 = -995126396;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH13560479 = -510973555;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH13842105 = -156591781;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH55473895 = -708566263;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH33102475 = -303764423;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH86797397 = -924210007;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH24717977 = -470163718;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH41478227 = -71228462;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH77061463 = -620019006;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH86310152 = -201532643;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH71183477 = -656661750;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH47414918 = -658383345;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH55949644 = 78144081;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH81846910 = -932157988;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH69997105 = -700044164;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH32049616 = -587274172;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH54310277 = -288846868;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH4589443 = -899719133;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH73444174 = -962571752;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH33329803 = -575486858;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH27763998 = -936398969;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH57955774 = -417621004;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH36390189 = -775836366;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH47075561 = 41264317;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH24074427 = -387407030;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH32397110 = -846636226;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH15563639 = -721492855;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH5410106 = -677533796;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH67634272 = -834510689;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH77331857 = -735888860;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH6242289 = 51009934;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH4231396 = -871788819;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH49015559 = -605032529;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH41446137 = -368782491;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH76669975 = -727321469;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH45738562 = -681223264;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH92436998 = -372562702;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH80195456 = -690200288;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH87989849 = -984232945;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH14816725 = -16192579;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH64313785 = 26397077;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH53030797 = -155899667;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH23014909 = -879967038;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH90275959 = -763092417;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH78322117 = -127656284;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH98842414 = -587365653;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH58960456 = -695769069;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH81850409 = -531086139;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH50752519 = -339826513;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH2906761 = -933313201;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH81004819 = -116088077;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH64766528 = -229225596;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH43061225 = -781289916;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH3559745 = -212224090;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH47321142 = -938530650;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH66153433 = -477505392;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH77170289 = -735137190;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH66766543 = -97856099;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH31399468 = -221159234;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH705366 = -457128197;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH71233759 = -102717152;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH19307871 = -792629922;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH73843954 = -236717774;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH99729605 = -884130146;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH80067864 = -152542578;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH66952082 = -784872932;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH98399359 = 46649183;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH14503507 = -553073428;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH5176935 = -104836519;    long TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH24258543 = -564261213;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH47307209 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH79260958;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH79260958 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH26159730;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH26159730 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH2590649;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH2590649 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH12646954;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH12646954 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH75598259;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH75598259 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH20736795;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH20736795 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH85905107;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH85905107 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH16531277;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH16531277 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH779183;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH779183 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH59299040;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH59299040 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH6279103;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH6279103 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH35669352;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH35669352 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH21092324;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH21092324 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH1614122;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH1614122 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH89031762;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH89031762 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH94509024;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH94509024 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH76372;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH76372 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH76586112;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH76586112 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH49505594;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH49505594 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH46257333;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH46257333 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH14910100;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH14910100 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH63697320;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH63697320 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH20749625;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH20749625 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH34956376;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH34956376 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH35315096;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH35315096 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH69355971;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH69355971 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH16505400;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH16505400 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH36889548;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH36889548 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH75085140;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH75085140 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH24109207;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH24109207 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH13560479;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH13560479 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH13842105;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH13842105 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH55473895;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH55473895 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH33102475;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH33102475 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH86797397;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH86797397 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH24717977;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH24717977 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH41478227;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH41478227 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH77061463;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH77061463 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH86310152;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH86310152 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH71183477;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH71183477 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH47414918;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH47414918 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH55949644;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH55949644 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH81846910;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH81846910 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH69997105;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH69997105 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH32049616;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH32049616 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH54310277;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH54310277 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH4589443;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH4589443 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH73444174;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH73444174 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH33329803;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH33329803 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH27763998;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH27763998 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH57955774;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH57955774 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH36390189;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH36390189 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH47075561;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH47075561 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH24074427;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH24074427 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH32397110;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH32397110 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH15563639;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH15563639 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH5410106;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH5410106 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH67634272;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH67634272 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH77331857;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH77331857 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH6242289;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH6242289 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH4231396;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH4231396 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH49015559;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH49015559 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH41446137;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH41446137 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH76669975;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH76669975 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH45738562;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH45738562 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH92436998;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH92436998 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH80195456;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH80195456 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH87989849;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH87989849 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH14816725;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH14816725 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH64313785;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH64313785 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH53030797;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH53030797 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH23014909;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH23014909 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH90275959;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH90275959 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH78322117;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH78322117 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH98842414;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH98842414 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH58960456;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH58960456 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH81850409;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH81850409 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH50752519;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH50752519 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH2906761;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH2906761 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH81004819;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH81004819 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH64766528;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH64766528 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH43061225;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH43061225 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH3559745;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH3559745 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH47321142;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH47321142 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH66153433;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH66153433 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH77170289;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH77170289 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH66766543;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH66766543 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH31399468;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH31399468 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH705366;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH705366 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH71233759;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH71233759 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH19307871;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH19307871 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH73843954;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH73843954 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH99729605;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH99729605 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH80067864;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH80067864 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH66952082;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH66952082 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH98399359;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH98399359 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH14503507;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH14503507 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH5176935;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH5176935 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH24258543;     TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH24258543 = TcNcbYViSwgfdmiihdnjrFtsBafaLpRXcfyyBiZKmwbNyYhEGH47307209;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void vLOUtFInVOqQtzwHLqspyuDMQrbyqtYbWVjmuxFzLRqrGiwlayLKmfMBICDCdJwbqOuYPKMNlsdgDLGhuqXsiNpqIi25640007() {     double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR15919946 = -843764655;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR71180673 = -334942580;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR95075224 = -401789385;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR77140309 = 30086672;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR17083713 = -931831252;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR58399922 = -469112747;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR31550112 = -776623526;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR4516059 = -623149393;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR79684042 = 58639855;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR28786759 = -740258096;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR88984674 = 86772839;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR68056714 = -626459610;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR64828296 = 95828376;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR58341212 = -645276468;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR6389498 = -640923750;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR29025316 = -139001635;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR35725649 = -436238369;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR12713120 = -227047629;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR21789718 = 78642049;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR24099139 = 25199043;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR42706952 = -650931381;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR53642655 = -940895331;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR37235444 = -129436191;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR9200681 = -396724334;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR64298857 = -957261570;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR87709442 = 99075471;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR54981854 = -373921629;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR56054043 = -682794351;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR55989091 = -968243185;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR34894179 = -694535022;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR55990831 = -730936425;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR26069813 = -800825989;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR98554928 = -609120400;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR34309594 = -711560801;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR82887630 = -269702905;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR68224707 = -601016703;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR57491954 = -221610233;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR95825442 = -663391555;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR56625256 = -207324474;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR61160401 = 83159195;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR85912831 = -347820307;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR83386797 = 66144246;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR90162895 = -253224881;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR37248432 = -694599983;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR22946899 = -760059106;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR83704999 = -996466134;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR90978468 = -208036869;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR29836365 = -441899639;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR31809033 = 40684597;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR77730520 = -125832700;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR88162683 = -349177232;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR76394815 = -289494247;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR94888462 = -479538701;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR34426294 = -216221510;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR82933055 = -688716213;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR62277291 = -902869324;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR33945230 = -105506389;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR85874544 = 94934948;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR12841453 = -12651758;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR29374270 = -930906724;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR3418068 = 4808881;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR75496068 = 6170824;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR48526968 = -654906209;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR44789863 = -246825123;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR72795928 = 90678328;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR62914861 = -112401173;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR69501786 = 82660789;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR30518703 = -192610824;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR75453582 = -275573564;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR38164790 = 60092953;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR71533362 = -917391402;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR39900206 = -772846815;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR56087863 = 80276845;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR60629316 = 95482853;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR38186307 = -626980651;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR59320155 = -617075628;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR63479760 = -587670451;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR99987012 = -434836209;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR86253782 = -636665228;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR80593858 = -960795437;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR96730974 = -692887660;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR25145490 = -932021990;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR24245011 = -623478948;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR78258570 = -742410486;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR46731495 = -245357790;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR79596015 = -341442178;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR31181351 = -221287289;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR64128634 = -292898891;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR51376538 = 77155411;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR20610340 = -366833581;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR34279478 = -395510314;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR71617410 = -216545182;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR82983990 = -550739797;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR27250986 = -276417751;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR57742333 = -921649687;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR10416764 = -253991131;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR34859830 = -278949545;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR45373032 = 93600241;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR64452503 = -685278312;    double dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR60032037 = -843764655;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR15919946 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR71180673;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR71180673 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR95075224;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR95075224 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR77140309;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR77140309 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR17083713;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR17083713 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR58399922;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR58399922 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR31550112;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR31550112 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR4516059;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR4516059 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR79684042;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR79684042 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR28786759;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR28786759 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR88984674;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR88984674 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR68056714;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR68056714 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR64828296;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR64828296 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR58341212;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR58341212 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR6389498;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR6389498 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR29025316;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR29025316 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR35725649;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR35725649 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR12713120;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR12713120 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR21789718;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR21789718 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR24099139;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR24099139 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR42706952;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR42706952 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR53642655;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR53642655 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR37235444;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR37235444 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR9200681;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR9200681 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR64298857;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR64298857 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR87709442;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR87709442 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR54981854;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR54981854 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR56054043;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR56054043 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR55989091;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR55989091 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR34894179;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR34894179 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR55990831;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR55990831 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR26069813;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR26069813 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR98554928;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR98554928 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR34309594;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR34309594 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR82887630;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR82887630 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR68224707;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR68224707 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR57491954;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR57491954 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR95825442;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR95825442 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR56625256;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR56625256 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR61160401;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR61160401 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR85912831;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR85912831 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR83386797;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR83386797 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR90162895;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR90162895 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR37248432;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR37248432 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR22946899;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR22946899 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR83704999;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR83704999 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR90978468;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR90978468 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR29836365;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR29836365 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR31809033;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR31809033 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR77730520;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR77730520 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR88162683;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR88162683 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR76394815;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR76394815 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR94888462;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR94888462 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR34426294;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR34426294 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR82933055;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR82933055 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR62277291;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR62277291 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR33945230;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR33945230 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR85874544;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR85874544 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR12841453;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR12841453 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR29374270;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR29374270 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR3418068;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR3418068 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR75496068;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR75496068 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR48526968;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR48526968 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR44789863;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR44789863 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR72795928;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR72795928 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR62914861;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR62914861 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR69501786;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR69501786 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR30518703;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR30518703 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR75453582;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR75453582 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR38164790;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR38164790 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR71533362;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR71533362 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR39900206;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR39900206 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR56087863;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR56087863 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR60629316;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR60629316 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR38186307;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR38186307 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR59320155;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR59320155 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR63479760;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR63479760 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR99987012;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR99987012 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR86253782;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR86253782 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR80593858;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR80593858 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR96730974;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR96730974 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR25145490;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR25145490 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR24245011;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR24245011 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR78258570;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR78258570 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR46731495;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR46731495 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR79596015;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR79596015 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR31181351;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR31181351 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR64128634;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR64128634 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR51376538;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR51376538 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR20610340;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR20610340 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR34279478;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR34279478 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR71617410;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR71617410 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR82983990;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR82983990 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR27250986;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR27250986 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR57742333;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR57742333 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR10416764;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR10416764 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR34859830;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR34859830 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR45373032;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR45373032 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR64452503;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR64452503 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR60032037;     dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR60032037 = dstDWBUPBroYVQlxZtBfywwWHmmEscZDPhgUUhWcMoKOtVVGGlnvockgwXwjPQWYSkJnkR15919946;}
// Junk Finished
