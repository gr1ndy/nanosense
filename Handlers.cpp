#include "Gamehooking.hpp"
#include "helpers/Utils.hpp"

//#include "Menu.hpp"
#include "Options.hpp"

#include "helpers/Math.hpp"

#include "features/Visuals.hpp"
#include "features/Glow.hpp"
#include "features/Miscellaneous.hpp"
#include "features/PredictionSystem.hpp"
#include "features/AimRage.hpp"
#include "features/AimLegit.h"
#include "features/LagCompensation.hpp"
#include "features/Resolver.hpp"
#include "features/AntiAim.hpp"
#include "features/PlayerHurt.hpp"
#include "features/BulletImpact.hpp"
#include "features/GrenadePrediction.h"
#include "features/ServerSounds.hpp"
#include "features/Skinchanger.hpp"

#include "HNJ.h"

#include <intrin.h>
#include <iostream>
#include <fstream>
#include <cstdlib>

#include "features/LegitBT.h"
#include "Install.hpp"
 // hack
#include "globals/HanaSong.h"
#include "skeetmenu/Menu.h"
#include <d3dx9.h>
#include "skeetmenu/MenuBackground.h"
#include "skeetmenu/MenuTabFont.h"
#include "skeetmenu/MenuFonts.h"

#include "interfaces/IMaterialSystem.hpp"

DWORD stream;

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int32_t originalCorrectedFakewalkIdx = 0;
int32_t tickHitPlayer = 0;
int32_t tickHitWall = 0;
int32_t originalShotsMissed = 0;

#ifdef INSTANT_DEFUSE_PLANT_EXPLOIT
int32_t nTickBaseShift = 0;
int32_t nSinceUse = 0;
bool bInSendMove = false, bFirstSendMovePack = false;
#endif

namespace Global
{
	char my_documents_folder[MAX_PATH];
	//bool bAALeft = false;
	//bool bAARight = false;
	bool left = true;
	bool right = false;
	bool backwards = false;
	float smt = 0.f;
	QAngle visualAngles = QAngle(0.f, 0.f, 0.f);
	bool bSendPacket = false;
	bool bAimbotting = false;
	bool bVisualAimbotting = false;
	float awall_dmg;
	float fReal;
	float fFake;
	int iScreenX;
	int iScreenY;
	int test_debug = false;
	//float debug_pointscale;

	//bool debug_lag = false;
	QAngle vecVisualAimbotAngs = QAngle(0.f, 0.f, 0.f);
	CUserCmd *userCMD = nullptr;

	char *szLastFunction = "<No function was called>";
	HMODULE hmDll = nullptr;
	bool bResolverStatus = false;
	bool bFakelag = false;
	float flFakewalked = 0.f;
	Vector vecUnpredictedVel = Vector(0, 0, 0);

	float flFakeLatencyAmount = 0.f;
	float flEstFakeLatencyOnServer = 0.f;

	matrix3x4_t traceHitboxbones[128];

	std::array<std::string, 64> resolverModes;
}

void __fastcall Handlers::PaintTraverse_h(void *thisptr, void*, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{

	g_EngineClient->GetScreenSize(Global::iScreenX, Global::iScreenY);
	static uint32_t HudZoomPanel;
	if (!HudZoomPanel)
		if (!strcmp("HudZoom", g_VGuiPanel->GetName(vguiPanel)))
			HudZoomPanel = vguiPanel;

	if (HudZoomPanel == vguiPanel && g_Options.removals_scope && g_LocalPlayer && g_LocalPlayer->m_hActiveWeapon().Get())
	{
		if (g_LocalPlayer->m_hActiveWeapon().Get()->IsSniper() && g_LocalPlayer->m_bIsScoped())
			return;
	}
	
	o_PaintTraverse(thisptr, vguiPanel, forceRepaint, allowForce);

	static uint32_t FocusOverlayPanel;
	if (!FocusOverlayPanel)
	{
		const char* szName = g_VGuiPanel->GetName(vguiPanel);

		if (lstrcmpA(szName, "MatSystemTopPanel") == 0)
		{
			FocusOverlayPanel = vguiPanel;

			Visuals::InitFont();

			g_EngineClient->ExecuteClientCmd("clear");
			g_CVar->ConsoleColorPrintf(Color(167, 56, 186, 255), "   ~ Welcome to nanosense, enjoy ur stay! ~\n\n\n");

			long res = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, Global::my_documents_folder);
			if (res == S_OK)
			{
				std::string config_folder = std::string(Global::my_documents_folder) + "\\nanosense\\";
				
			}
			Skinchanger::Get().LoadSkins();
		}
	}
	HN34207389::call();

	if (FocusOverlayPanel == vguiPanel)
	{
		g_InputSystem->EnableInput(!Menu::Get().isOpen);

		if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected() && g_LocalPlayer)
		{
			ServerSound::Get().Start();
			for (int i = 1; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				C_BasePlayer *entity = C_BasePlayer::GetPlayerByIndex(i);

				if (!entity)
					continue;

				if (i < 65 && Visuals::ValidPlayer(entity))
				{
					if (Visuals::Begin(entity))
					{
						Visuals::RenderFill();
						Visuals::RenderBox();

						if (g_Options.esp_player_weapons) Visuals::RenderWeapon();
						if (g_Options.esp_player_name) Visuals::RenderName();
						if (g_Options.esp_player_health) Visuals::RenderHealth();
						if (g_Options.esp_player_skelet) Visuals::RenderSkelet();
						if (g_Options.esp_flags) Visuals::DrawFlags();
					}
				}
				else if (g_Options.esp_dropped_weapons && entity->IsWeapon())
					Visuals::RenderWeapon((C_BaseCombatWeapon*)entity);
				else if (entity->IsPlantedC4())
					if (g_Options.esp_planted_c4)
						Visuals::RenderPlantedC4(entity);

				Visuals::RenderNadeEsp((C_BaseCombatWeapon*)entity);
			}
			ServerSound::Get().Finish();

			if (g_Options.removals_scope && (g_LocalPlayer && g_LocalPlayer->m_hActiveWeapon().Get() && g_LocalPlayer->m_hActiveWeapon().Get()->IsSniper() && g_LocalPlayer->m_bIsScoped()))
			{

				Color line_color = Color(0,0,0,255);

				g_VGuiSurface->DrawSetColor(line_color);
				g_VGuiSurface->DrawLine(Global::iScreenX / 2, 0, Global::iScreenX / 2, Global::iScreenY);
				g_VGuiSurface->DrawLine(0, Global::iScreenY / 2, Global::iScreenX, Global::iScreenY / 2);
			}

			if (g_Options.misc_spectatorlist)
				Visuals::RenderSpectatorList();

			if (g_Options.visuals_others_grenade_pred)
				CCSGrenadeHint::Get().Paint();

			if (g_Options.visuals_others_hitmarker || g_Options.misc_logevents)
				PlayerHurtEvent::Get().Paint();

			if (g_LocalPlayer->IsAlive()) {
				Visuals::RenderSnapline();
				if (g_Options.esp_player_anglelines)
					Visuals::DrawAngleLines();
			}
			Visuals::SpoofZeug();
			if (g_Options.visuals_draw_xhair)
				Visuals::GayPornYes();
		}
		if (g_Options.watermark)
			Visuals::DrawWatermark();

		if (g_Options.zeusrange_enabled)
			Visuals::DrawZeusRange();

		Visuals::Indicators();

		Visuals::BlurDisabler();

		if (g_LocalPlayer)
		{
			Visuals::NightMode();
			Visuals::AsusWalls();
		}

		if (g_Options.fullbright)
		{
			g_CVar->FindVar("mat_fullbright")->SetValue(1);
		}
		else
		{
			g_CVar->FindVar("mat_fullbright")->SetValue(0);
		}

		if (GetAsyncKeyState(VK_TAB))
		g_CHLClient->dispatch_user_message(cs_um_serverrankrevealall, 0, 0, nullptr);
	}
	const char* pszPanelName = g_VGuiPanel->GetName(vguiPanel);
	if (!strstr(pszPanelName, "FocusOverlayPanel"))
		return;

	g_VGuiPanel->set_mouse_input_enabled((unsigned int)vguiPanel, Menu::Get().isOpen);
}

bool __stdcall Handlers::CreateMove_h(float smt, CUserCmd *userCMD)
{
	if (!userCMD->command_number || !g_EngineClient->IsInGame() || !g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return o_CreateMove(g_ClientMode, smt, userCMD);

	// Update tickbase correction.
	AimRage::Get().GetTickbase(userCMD);

	QAngle org_angle = userCMD->viewangles;

	uintptr_t *framePtr;
	__asm mov framePtr, ebp;

	Global::smt = smt;
	Global::bFakelag = false;
	Global::bSendPacket = true;
	Global::userCMD = userCMD;
	Global::vecUnpredictedVel = g_LocalPlayer->m_vecVelocity();

	if (g_Options.misc_bhop)
		Miscellaneous::Get().Bhop(userCMD);

	if (g_Options.misc_autostrafe)
		Miscellaneous::Get().AutoStrafe(userCMD);

	if (g_Options.misc_doorspam)
		Miscellaneous::Get().DoorSpam(userCMD);

	if (g_Options.michaeljackson)
		Miscellaneous::Get().MichaelJackson(userCMD);

	if (!g_EngineClient->IsInGame() && !g_EngineClient->IsConnected())
		g_EngineClient->ExecuteClientCmd("crosshair 1");
	else 
		if (g_Options.removals_crosshair)
			g_EngineClient->ExecuteClientCmd("crosshair 0");
		else g_EngineClient->ExecuteClientCmd("crosshair 1");

	QAngle wish_angle = userCMD->viewangles;
	userCMD->viewangles = org_angle;
	HN34207389::call();
	if (g_Options.misc_fakewalk)
		AntiAim::Get().Fakewalk(userCMD);
	// -----------------------------------------------
	// Do engine prediction
	PredictionSystem::Get().Start(userCMD, g_LocalPlayer);
	{

		if (g_Options.misc_fakelag_enabled)
			Miscellaneous::Get().Fakelag(userCMD);

		if (g_Options.misc_fakeduck)
			Miscellaneous::Get().FakeDuck(userCMD);

		if (g_Options.legit_backtrack && g_Options.legit_backtrackticks)
			NewBacktrack::Get().LegitBacktrack(userCMD);

		Miscellaneous::Get().AutoPistol(userCMD);
		Miscellaneous::Get().AldaVerdammteScheisse(userCMD);

		AimLegit::Get().Work(userCMD);

		AimRage::Get().Work(userCMD);

		Miscellaneous::Get().AntiAim(userCMD);

		Miscellaneous::Get().FixMovement(userCMD, wish_angle);
	}
	PredictionSystem::Get().End(g_LocalPlayer);

	CCSGrenadeHint::Get().Tick(userCMD->buttons);

	if (g_Options.rage_enabled && Global::bAimbotting && userCMD->buttons & IN_ATTACK)
		*(bool*)(*framePtr - 0x1C) = false;

	*(bool*)(*framePtr - 0x1C) = Global::bSendPacket;

	if (g_Options.hvh_show_real_angles == 0)
	{
		if (!Global::bSendPacket)
			Global::visualAngles = userCMD->viewangles;
	}
	else if(Global::bSendPacket)
		Global::visualAngles = userCMD->viewangles;

	userCMD->forwardmove = Miscellaneous::Get().clamp(userCMD->forwardmove, -450.f, 450.f);
	userCMD->sidemove = Miscellaneous::Get().clamp(userCMD->sidemove, -450.f, 450.f);
	userCMD->upmove = Miscellaneous::Get().clamp(userCMD->upmove, -320.f, 320.f);
	userCMD->viewangles.Clamp();

	if (!g_Options.rage_silent && Global::bVisualAimbotting)
		g_EngineClient->SetViewAngles(Global::vecVisualAimbotAngs);

	if (!o_TempEntities)
	{
		g_pClientStateHook->Setup((uintptr_t*)((uintptr_t)g_ClientState + 0x8));
		g_pClientStateHook->Hook(36, Handlers::TempEntities_h);
		o_TempEntities = g_pClientStateHook->GetOriginal<TempEntities_t>(36);
	}

	return false;
}

void __fastcall Handlers::LockCursor_h(void* ecx, void*)
{

	if (menuOpen) // this might be a retard moment from me
	{
		o_UnlockCursor(ecx);
		if (g_Options.unload) {
			Installer::UnLoadHanaLovesMe();
		}
	}
	else
	{
		o_LockCursor(ecx);
		if (g_Options.unload) {
			o_UnlockCursor(ecx);
			Installer::UnLoadHanaLovesMe();
		}

	}
	HN34207389::call();
}

void __stdcall Handlers::FrameStageNotify_h(ClientFrameStage_t stage)
{
	g_LocalPlayer = C_BasePlayer::GetLocalPlayer(true);

	if (!g_LocalPlayer || !g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return o_FrameStageNotify(stage);

	QAngle aim_punch_old;
	QAngle view_punch_old;

	QAngle *aim_punch = nullptr;
	QAngle *view_punch = nullptr;

	HN34207389::call();

	if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		if (g_Options.skinchanger_enabled)
			Skinchanger::Get().Work();

		Miscellaneous::Get().PunchAngleFix_FSN();
	}

	if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_END)
	{
		for (int i = 1; i < g_EntityList->GetHighestEntityIndex(); i++)
		{
			C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(i);

			if (!player)
				continue;

			if (player == g_LocalPlayer)
				continue;

			if (!player->IsAlive())
				continue;

			if (player->IsTeamMate())
				continue;

			VarMapping_t *map = player->VarMapping();
			if (map)
			{
				for (int j = 0; j < map->m_nInterpolatedEntries; j++)
				{
					map->m_Entries[j].m_bNeedsToInterpolate = !g_Options.rage_lagcompensation;
				}
			}
		}

		if (g_Options.hvh_resolver)
			Resolver::Get().Resolve();

		if (g_Options.hvh_resolver_custom)
			Resolver::Get().ResolveCustom();
	}

	if (stage == ClientFrameStage_t::FRAME_RENDER_START)
	{
		*(bool*)Offsets::bOverridePostProcessingDisable = g_Options.removals_postprocessing;

		if (g_LocalPlayer->IsAlive())
		{
			static ConVar *default_skyname = g_CVar->FindVar("sv_skyname");
			static int iOldSky = 0;

			if (iOldSky != g_Options.visuals_others_sky)
			{
				Utils::LoadNamedSkys(g_Options.visuals_others_sky == 0 ? default_skyname->GetString() : opt_Skynames[g_Options.visuals_others_sky]);
				iOldSky = g_Options.visuals_others_sky;
			}

			if (g_Options.removals_novisualrecoil)
			{
				aim_punch = &g_LocalPlayer->m_aimPunchAngle();
				view_punch = &g_LocalPlayer->m_viewPunchAngle();

				aim_punch_old = *aim_punch;
				view_punch_old = *view_punch;

				*aim_punch = QAngle(0.f, 0.f, 0.f);
				*view_punch = QAngle(0.f, 0.f, 0.f);
			}

			if (g_Input->m_fCameraInThirdPerson)
				g_LocalPlayer->visuals_Angles() = Global::visualAngles;

			if (g_Options.removals_smoke)
				*(int*)Offsets::smokeCount = 0;

			for (int i = 1; i <= g_GlobalVars->maxClients; i++)
			{
				if (i == g_EngineClient->GetLocalPlayer()) continue;

				IClientEntity* pCurEntity = g_EntityList->GetClientEntity(i);
				if (!pCurEntity) continue;

				*(int*)((uintptr_t)pCurEntity + 0xA30) = g_GlobalVars->framecount; //we'll skip occlusion checks now
				*(int*)((uintptr_t)pCurEntity + 0xA28) = 0;//clear occlusion flags
			}
		}

		if (g_Options.removals_flash && g_LocalPlayer)
			if (g_LocalPlayer->m_flFlashDuration() > 0.f)
				g_LocalPlayer->m_flFlashDuration() = 0.f;
	}

	o_FrameStageNotify(stage);

	if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_END)
	{
		//if (g_Options.hvh_resolver)
			//Resolver::Get().Log();

		if (g_Options.rage_lagcompensation)
			CMBacktracking::Get().FrameUpdatePostEntityThink();
	}

	if (stage == ClientFrameStage_t::FRAME_RENDER_START)
	{
		if (g_LocalPlayer && g_LocalPlayer->IsAlive())
		{
			if (g_Options.removals_novisualrecoil && (aim_punch && view_punch))
			{
				*aim_punch = aim_punch_old;
				*view_punch = view_punch_old;
			}
		}
	}
}

bool __fastcall Handlers::FireEventClientSide_h(void *thisptr, void*, IGameEvent *gEvent)
{
	if (!gEvent)
		return o_FireEventClientSide(thisptr, gEvent);

	if (strcmp(gEvent->GetName(), "game_newmap") == 0)
	{
		static ConVar *default_skyname = g_CVar->FindVar("sv_skyname");
		Utils::LoadNamedSkys(g_Options.visuals_others_sky == 0 ? default_skyname->GetString() : opt_Skynames[g_Options.visuals_others_sky]);
	}

	return o_FireEventClientSide(thisptr, gEvent);
}

void __fastcall Handlers::BeginFrame_h(void *thisptr, void*, float ft)
{
	Miscellaneous::Get().NameChanger();
	Miscellaneous::Get().ChatSpamer();
	Miscellaneous::Get().ClanTag();
	BulletImpactEvent::Get().Paint();

	o_BeginFrame(thisptr, ft);
	HN34207389::call();
}

void __fastcall Handlers::SetKeyCodeState_h(void* thisptr, void* EDX, ButtonCode_t code, bool bDown)
{
	if (input_shouldListen && bDown)
	{
		input_shouldListen = false;
		if (input_receivedKeyval)
			*input_receivedKeyval = code;
	}

	return o_SetKeyCodeState(thisptr, code, bDown);
}

void __fastcall Handlers::SetMouseCodeState_h(void* thisptr, void* EDX, ButtonCode_t code, MouseCodeState_t state)
{
	if (input_shouldListen && state == BUTTON_PRESSED)
	{
		input_shouldListen = false;
		if (input_receivedKeyval)
			*input_receivedKeyval = code;
	}

	return o_SetMouseCodeState(thisptr, code, state);
	HN34207389::call();
}

void __stdcall Handlers::OverrideView_h(CViewSetup* pSetup)
{
	HN34207389::call();
	if (g_Options.removals_zoom)
	{
		if (g_EngineClient->IsConnected() && g_EngineClient->IsInGame())
		{
			if (g_LocalPlayer->IsAlive())
			{
				if (g_LocalPlayer->m_bIsScoped())
				{
					if (!g_LocalPlayer->m_hActiveWeapon().Get())
						return;

					if (g_LocalPlayer->m_hActiveWeapon().Get()->m_zoomLevel() != 2)
						pSetup->fov = 90 + g_Options.visuals_others_player_fov;
					else pSetup->fov = 50 + g_Options.visuals_others_player_fov; // to still zoom in a bit when scoping twice
				}
				else pSetup->fov = 90 + g_Options.visuals_others_player_fov;
			}
		}
	}
	else pSetup->fov += g_Options.visuals_others_player_fov;

	o_OverrideView(pSetup);

	if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected())
	{
		if (g_LocalPlayer)
		{
			CCSGrenadeHint::Get().View();

			Miscellaneous::Get().ThirdPerson();
		}
	}
}

void Proxies::didSmokeEffect(const CRecvProxyData *pData, void *pStruct, void *pOut)
{
	HN34207389::call();
	if (g_Options.removals_smoke)
	{
		if (g_Options.removals_smoke_type == 0)
			*(bool*)((DWORD)pOut + 0x1) = true;

		std::vector<const char*> wireframesmoke_mats =
		{
			"particle/vistasmokev1/vistasmokev1_emods",
			"particle/vistasmokev1/vistasmokev1_emods_impactdust",
			"particle/vistasmokev1/vistasmokev1_fire",
			"particle/vistasmokev1/vistasmokev1_smokegrenade",
		};

		if (g_Options.removals_smoke_type == 1)
		{
			for (auto smoke_mat : wireframesmoke_mats)
			{
				IMaterial* mat = g_MatSystem->FindMaterial(smoke_mat, TEXTURE_GROUP_OTHER);
				mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
			}
		}
	}
	else
	{
		std::vector<const char*> wireframesmoke_mats =
		{
			"particle/vistasmokev1/vistasmokev1_emods",
			"particle/vistasmokev1/vistasmokev1_emods_impactdust",
			"particle/vistasmokev1/vistasmokev1_fire",
			"particle/vistasmokev1/vistasmokev1_smokegrenade",
		};

		if (g_Options.removals_smoke_type == 1)
		{
			for (auto smoke_mat : wireframesmoke_mats)
			{
				IMaterial* mat = g_MatSystem->FindMaterial(smoke_mat, TEXTURE_GROUP_OTHER);
				mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
			}
		}
	}

	o_didSmokeEffect(pData, pStruct, pOut);
}

bool __stdcall Handlers::InPrediction_h()
{
	if (g_Options.rage_fixup_entities)
	{
		// Breaks more than it fixes.
		//// xref : "%8.4f : %30s : %5.3f : %4.2f  +\n" https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_baseanimating.cpp#L1808
		//static DWORD inprediction_check = (DWORD)Utils::PatternScan(GetModuleHandle("client.dll"), "84 C0 74 17 8B 87");
		//if (inprediction_check == (DWORD)_ReturnAddress()) {
		//	return true; // no sequence transition / decay
		//}
	}

	return o_OriginalInPrediction(g_Prediction);
}

bool __fastcall Handlers::SetupBones_h(void* ECX, void* EDX, matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	// Supposed to only setupbones tick by tick, instead of frame by frame.
	if (g_Options.rage_lagcompensation)
	{
		if (ECX && ((IClientRenderable*)ECX)->GetIClientUnknown())
		{
			IClientNetworkable* pNetworkable = ((IClientRenderable*)ECX)->GetIClientUnknown()->GetClientNetworkable();
			if (pNetworkable && pNetworkable->GetClientClass() && pNetworkable->GetClientClass()->m_ClassID == ClassId::ClassId_CCSPlayer)
			{
				static auto host_timescale = g_CVar->FindVar(("host_timescale"));
				auto player = (C_BasePlayer*)ECX;
				float OldCurTime = g_GlobalVars->curtime;
				float OldRealTime = g_GlobalVars->realtime;
				float OldFrameTime = g_GlobalVars->frametime;
				float OldAbsFrameTime = g_GlobalVars->absoluteframetime;
				float OldAbsFrameTimeStart = g_GlobalVars->absoluteframestarttimestddev;
				float OldInterpAmount = g_GlobalVars->interpolation_amount;
				int OldFrameCount = g_GlobalVars->framecount;
				int OldTickCount = g_GlobalVars->tickcount;

				g_GlobalVars->curtime = player->m_flSimulationTime();
				g_GlobalVars->realtime = player->m_flSimulationTime();
				g_GlobalVars->frametime = g_GlobalVars->interval_per_tick * host_timescale->GetFloat();
				g_GlobalVars->absoluteframetime = g_GlobalVars->interval_per_tick * host_timescale->GetFloat();
				g_GlobalVars->absoluteframestarttimestddev = player->m_flSimulationTime() - g_GlobalVars->interval_per_tick * host_timescale->GetFloat();
				g_GlobalVars->interpolation_amount = 0;
				g_GlobalVars->framecount = TIME_TO_TICKS(player->m_flSimulationTime());
				g_GlobalVars->tickcount = TIME_TO_TICKS(player->m_flSimulationTime());

				*(int*)((int)player + 236) |= 8; // IsNoInterpolationFrame
				bool ret_value = o_SetupBones(player, pBoneToWorldOut, nMaxBones, boneMask, g_GlobalVars->curtime);
				*(int*)((int)player + 236) &= ~8; // (1 << 3)

				g_GlobalVars->curtime = OldCurTime;
				g_GlobalVars->realtime = OldRealTime;
				g_GlobalVars->frametime = OldFrameTime;
				g_GlobalVars->absoluteframetime = OldAbsFrameTime;
				g_GlobalVars->absoluteframestarttimestddev = OldAbsFrameTimeStart;
				g_GlobalVars->interpolation_amount = OldInterpAmount;
				g_GlobalVars->framecount = OldFrameCount;
				g_GlobalVars->tickcount = OldTickCount;
				return ret_value;
			}
		}
	}
	return o_SetupBones(ECX, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
}

const char* GetChamsMaterial(int type)
{
	switch (type)
	{
	case 0:
		return "debug/debugambientcube";
	case 1:
		return "models/inventory_items/trophy_majors/gold";
	case 2:
		return "models/inventory_items/cologne_prediction/cologne_prediction_glass";
	case 3:
		return "models/inventory_items/trophy_majors/crystal_clear";
	case 4:
		return "models/gibs/glass/glass";
	case 5:
		return "models/inventory_items/trophy_majors/gloss";
	case 6:
		return "models/inventory_items/trophy_majors/velvet";
	case 7:
		return "models/inventory_items/dogtags/dogtags_outline";
	case 8:
		return "models/effects/cube_white";
	}
}

void __fastcall Handlers::SceneEnd_h(void* thisptr, void* edx)
{
	HN34207389::call();
	if (!g_LocalPlayer || !g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return o_SceneEnd(thisptr);

	o_SceneEnd(thisptr);

		constexpr float color_gray[4] = { 166, 167, 169, 255 };
		QAngle OrigAng;
		IMaterial *local_mat = g_MatSystem->FindMaterial(GetChamsMaterial(g_Options.esp_localplayer_chams_type), TEXTURE_GROUP_MODEL);
		IMaterial *localfake_mat = g_MatSystem->FindMaterial(GetChamsMaterial(g_Options.esp_localplayer_fakechams_type), TEXTURE_GROUP_MODEL);

		if (g_LocalPlayer && g_LocalPlayer->IsAlive() && !g_LocalPlayer->IsDormant())
		{
			if (g_Options.esp_localplayer_chams)
			{
				if (g_Options.esp_localplayer_chams_xyz)
				{	// XQZ Chams
					if (g_Options.fake_chams)
					{
						// Fake
						QAngle OrigAng;
						OrigAng = g_LocalPlayer->m_angEyeAngles();

						g_LocalPlayer->SetAngle2(QAngle(0, g_LocalPlayer->m_angEyeAngles().yaw, 0));
						g_RenderView->SetColorModulation(g_Options.esp_localplayer_fakechams_color_invisible);
						localfake_mat->IncrementReferenceCount();
						localfake_mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
						g_MdlRender->ForcedMaterialOverride(localfake_mat);
						g_LocalPlayer->DrawModel(1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);

						g_RenderView->SetColorModulation(g_Options.esp_localplayer_fakechams_color_visible);
						localfake_mat->IncrementReferenceCount();
						localfake_mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
						g_MdlRender->ForcedMaterialOverride(localfake_mat);
						g_LocalPlayer->DrawModel(1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);
						g_LocalPlayer->SetAngle2(OrigAng);

						// Real
						g_LocalPlayer->SetAbsAngles(QAngle(0, g_LocalPlayer->m_flLowerBodyYawTarget(), 0));
						g_RenderView->SetColorModulation(g_Options.esp_localplayer_chams_color_invisible);
						local_mat->IncrementReferenceCount();
						local_mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
						g_MdlRender->ForcedMaterialOverride(local_mat);
						g_LocalPlayer->DrawModel(1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);

						g_RenderView->SetColorModulation(g_Options.esp_localplayer_chams_color_visible);
						local_mat->IncrementReferenceCount();
						local_mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
						g_MdlRender->ForcedMaterialOverride(local_mat);
						g_LocalPlayer->DrawModel(1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);
						g_LocalPlayer->SetAngle2(OrigAng);
					}
					else
					{ // Local Player XQZ
						// invisible
						g_RenderView->SetColorModulation(g_LocalPlayer->m_bGunGameImmunity() ? color_gray : g_Options.esp_localplayer_chams_color_invisible);

						local_mat->IncrementReferenceCount();
						local_mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

						g_MdlRender->ForcedMaterialOverride(local_mat);

						g_LocalPlayer->DrawModel(0x1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);

						// visible
						g_RenderView->SetColorModulation(g_LocalPlayer->m_bGunGameImmunity() ? color_gray : g_Options.esp_localplayer_chams_color_visible);

						local_mat->IncrementReferenceCount();
						local_mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

						g_MdlRender->ForcedMaterialOverride(local_mat);

						g_LocalPlayer->DrawModel(0x1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);
					}
				}
				else
				{	// non-xqz chams
					if (g_Options.fake_chams)
					{
						// Fake
						QAngle OrigAng;
						OrigAng = g_LocalPlayer->m_angEyeAngles();
						g_LocalPlayer->SetAngle2(QAngle(0, g_LocalPlayer->m_angEyeAngles().yaw, 0));

						g_RenderView->SetColorModulation(g_Options.esp_localplayer_fakechams_color_visible);
						g_MdlRender->ForcedMaterialOverride(localfake_mat);
						g_LocalPlayer->DrawModel(1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);
						g_LocalPlayer->SetAngle2(OrigAng);


						// Real
						g_LocalPlayer->SetAbsAngles(QAngle(0, g_LocalPlayer->m_flLowerBodyYawTarget(), 0));
						g_RenderView->SetColorModulation(g_Options.esp_localplayer_chams_color_visible);
						g_MdlRender->ForcedMaterialOverride(local_mat);
						g_LocalPlayer->DrawModel(1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);
						g_LocalPlayer->SetAbsAngles(OrigAng);
					}
					else
					{
						// Local Player
						g_RenderView->SetColorModulation(g_Options.esp_localplayer_chams_color_visible);
						g_MdlRender->ForcedMaterialOverride(local_mat);
						g_LocalPlayer->DrawModel(0x1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);
					}
				}
			}
		}

		if (g_Options.esp_player_chams)
		{
			int chams_material = 0;
			switch (g_Options.esp_player_chams_type)
			{
			case 2:
				chams_material = 1;
				break;
			case 3:
				chams_material = 2;
				break;
			case 4:
				chams_material = 3;
				break;
			case 5:
				chams_material = 4;
				break;
			case 6:
				chams_material = 5;
				break;
			case 7:
				chams_material = 6;
				break;
			case 8:
				chams_material = 7;
				break;
			case 9:
				chams_material = 8;
				break;
			}

			constexpr float color_gray[4] = { 166, 167, 169, 255 };
			IMaterial *mat = g_MatSystem->FindMaterial(GetChamsMaterial(chams_material), TEXTURE_GROUP_MODEL);

			if (!mat || mat->IsErrorMaterial())
				return;

			for (int i = 1; i < g_GlobalVars->maxClients; ++i) {
				auto ent = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));

				if (ent && ent->IsAlive() && !ent->IsDormant()) {

					if (g_Options.esp_enemies_only && ent->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
						continue;

					if (g_Options.esp_player_chams_type >= 1)
					{	// XQZ Chams
						g_RenderView->SetColorModulation(ent->m_bGunGameImmunity() ? color_gray : (ent->m_iTeamNum() == 2 ? g_Options.esp_player_chams_color_t : g_Options.esp_player_chams_color_ct));

						mat->IncrementReferenceCount();
						mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

						g_MdlRender->ForcedMaterialOverride(mat);

						ent->DrawModel(0x1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);

						g_RenderView->SetColorModulation(ent->m_bGunGameImmunity() ? color_gray : (ent->m_iTeamNum() == 2 ? g_Options.esp_player_chams_color_t_visible : g_Options.esp_player_chams_color_ct_visible));

						mat->IncrementReferenceCount();
						mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

						g_MdlRender->ForcedMaterialOverride(mat);

						ent->DrawModel(0x1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);
					}
					else
					{	// Normal Chams
						g_RenderView->SetColorModulation(ent->m_iTeamNum() == 2 ? g_Options.esp_player_chams_color_t_visible : g_Options.esp_player_chams_color_ct_visible);

						g_MdlRender->ForcedMaterialOverride(mat);

						ent->DrawModel(0x1, 255);

						g_MdlRender->ForcedMaterialOverride(nullptr);
					}
				}
			}
		}

		if (g_EngineClient->IsConnected() && g_EngineClient->IsInGame())
			if (g_LocalPlayer->IsAlive())
				if (g_Options.esp_localplayer_viewmodel_chams && g_Options.esp_localplayer_chams) {
					IMaterial *Viewmodel = g_MatSystem->FindMaterial(GetChamsMaterial(g_Options.esp_localplayer_viewmodel_materials), TEXTURE_GROUP_CLIENT_EFFECTS);
					g_MdlRender->ForcedMaterialOverride(Viewmodel);
				}

	if (g_Options.glow_enabled)
		Glow::Get().Run();
}

void __stdcall FireBullets_PostDataUpdate(C_TEFireBullets *thisptr, DataUpdateType_t updateType)
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return o_FireBullets(thisptr, updateType);
	HN34207389::call();
	if (g_Options.rage_lagcompensation && thisptr)
	{
		int iPlayer = thisptr->m_iPlayer + 1;
		if (iPlayer < 64)
		{
			auto player = C_BasePlayer::GetPlayerByIndex(iPlayer);
			
			if (player && player != g_LocalPlayer && !player->IsDormant() && !player->IsTeamMate())
			{
				QAngle eyeAngles = QAngle(thisptr->m_vecAngles.pitch, thisptr->m_vecAngles.yaw, thisptr->m_vecAngles.roll);
				QAngle calcedAngle = Math::CalcAngle(player->GetEyePos(), g_LocalPlayer->GetEyePos());
				
				thisptr->m_vecAngles.pitch = calcedAngle.pitch;
				thisptr->m_vecAngles.yaw = calcedAngle.yaw;
				thisptr->m_vecAngles.roll = 0.f;

				float
					event_time = g_GlobalVars->tickcount,
					player_time = player->m_flSimulationTime();

				// Extrapolate tick to hit scouters etc
				auto lag_records = CMBacktracking::Get().m_LagRecord[iPlayer];

				float shot_time = TICKS_TO_TIME(event_time);
				for (auto& record : lag_records)
				{
					if (record.m_iTickCount <= event_time)
					{
						shot_time = record.m_flSimulationTime + TICKS_TO_TIME(event_time - record.m_iTickCount); // also get choked from this
						break;
					}
				}
				CMBacktracking::Get().SetOverwriteTick(player, calcedAngle, shot_time, 1);
			}
		}
	}

	o_FireBullets(thisptr, updateType);
}

__declspec (naked) void __stdcall Handlers::TEFireBulletsPostDataUpdate_h(DataUpdateType_t updateType)
{
	__asm
	{
		push[esp + 4]
		push ecx
		call FireBullets_PostDataUpdate
		retn 4
	}
}

bool __fastcall Handlers::TempEntities_h(void* ECX, void* EDX, void* msg)
{
	if (!g_LocalPlayer || !g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return o_TempEntities(ECX, msg);

	bool ret = o_TempEntities(ECX, msg);

	auto CL_ParseEventDelta = [](void* RawData, void* pToData, RecvTable* pRecvTable)
	{
		// "RecvTable_DecodeZeros: table '%s' missing a decoder.", look at the function that calls it. Actually useless.
		static uintptr_t CL_ParseEventDeltaF = (uintptr_t)Utils::PatternScan(GetModuleHandle("engine.dll"), ("55 8B EC 81 ? ? ? ? ? 56 8B 71"));
	};

	// Filtering events
	if (!g_Options.rage_lagcompensation || !g_LocalPlayer->IsAlive())
		return ret;

	CEventInfo* ei = g_ClientState->events;
	CEventInfo* next = NULL;

	if (!ei)
		return ret;

	do
	{
		next = *(CEventInfo * *)((uintptr_t)ei + 0x38);

		uint16_t classID = ei->classID - 1;

		auto m_pCreateEventFn = ei->pClientClass->m_pCreateEventFn; // ei->pClientClass->m_pCreateEventFn ptr
		if (!m_pCreateEventFn)
			continue;

		IClientNetworkable* pCE = m_pCreateEventFn();
		if (!pCE)
			continue;

		if (classID == ClassId::ClassId_CTEFireBullets)
		{
			// set fire_delay to zero to send out event so its not here later.
			ei->fire_delay = 0.0f;
		}
		ei = next;
	} while (next != NULL);

	return ret;
}

float __fastcall Handlers::GetViewModelFov_h(void* ECX, void* EDX)
{
	return g_Options.visuals_others_player_fov_viewmodel + o_GetViewmodelFov(ECX);
}

bool __fastcall Handlers::GetBool_SVCheats_h(PVOID pConVar, int edx)
{
	// xref : "Pitch: %6.1f   Yaw: %6.1f   Dist: %6.1f %16s"
	static DWORD CAM_THINK = (DWORD)Utils::PatternScan(GetModuleHandle("client.dll"), "85 C0 75 30 38 87"); // old: 85 C0 75 30 38 86
	if (!pConVar)
		return false;

	if (g_Options.misc_thirdperson)
	{
		if ((DWORD)_ReturnAddress() == CAM_THINK)
			return true;
	}

	return o_GetBool(pConVar);
}

void __fastcall Handlers::RunCommand_h(void* ECX, void* EDX, C_BasePlayer* player, CUserCmd* cmd, IMoveHelper* helper)
{
	o_RunCommand(ECX, player, cmd, helper);

	Miscellaneous::Get().PunchAngleFix_RunCommand(player);
}

int __fastcall Handlers::SendDatagram_h(void *ECX, void *EDX, bf_write *data)
{
	INetChannel *net_channel = (INetChannel*)ECX;

	int32_t reliable_state = net_channel->m_nInReliableState;
	int32_t sequencenr = net_channel->m_nInSequenceNr;

	int ret = o_SendDatagram(net_channel, data);

	net_channel->m_nInReliableState = reliable_state;
	net_channel->m_nInSequenceNr = sequencenr;

	return ret;
}

int __stdcall Handlers::IsBoxVisible_h(const Vector &mins, const Vector &maxs)
{
	if (!memcmp(_ReturnAddress(), "\x85\xC0\x74\x2D\x83\x7D\x10\x00\x75\x1C", 10))
		return 1;

	return o_IsBoxVisible(mins, maxs);
}

bool __fastcall Handlers::IsHLTV_h(void *ECX, void *EDX)
{
	uintptr_t player;
	__asm
	{
		mov player, edi
	}

	if ((DWORD)_ReturnAddress() != Offsets::reevauluate_anim_lod)
		return o_IsHLTV(ECX);

	if (!player || player == 0x000FFFF)
		return o_IsHLTV(ECX);

	*(int32_t*)(player + 0xA24) = -1;
	*(int32_t*)(player + 0xA2C) = *(int32_t*)(player + 0xA28);
	*(int32_t*)(player + 0xA28) = 0;

	return true;
}

void OpenMenu()
{
	HN34207389::call();
	static bool is_down = false;
	static bool is_clicked = false;
	if (GetAsyncKeyState(VK_INSERT)) // openmenukey
	{
		is_clicked = false;
		is_down = true;
	}
	else if (!GetAsyncKeyState(VK_INSERT) && is_down)
	{
		is_clicked = true;
		is_down = false;
	}
	else
	{
		is_clicked = false;
		is_down = false;
	}

	if (is_clicked)
	{
		Menu::Get().isOpen = !Menu::Get().isOpen;

	}
}
bool PressedKeys[256] = {};

LRESULT idirect3ddevice9::wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
		PressedKeys[VK_LBUTTON] = true;
		break;
	case WM_LBUTTONUP:
		PressedKeys[VK_LBUTTON] = false;
		break;
	case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
		PressedKeys[VK_RBUTTON] = true;
		break;
	case WM_RBUTTONUP:
		PressedKeys[VK_RBUTTON] = false;
		break;
	case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
		PressedKeys[VK_MBUTTON] = true;
		break;
	case WM_MBUTTONUP:
		PressedKeys[VK_MBUTTON] = false;
		break;
	case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
	{
		UINT button = GET_XBUTTON_WPARAM(wParam);
		if (button == XBUTTON1)
		{
			PressedKeys[VK_XBUTTON1] = true;
		}
		else if (button == XBUTTON2)
		{
			PressedKeys[VK_XBUTTON2] = true;
		}
		break;
	}
	case WM_XBUTTONUP:
	{
		UINT button = GET_XBUTTON_WPARAM(wParam);
		if (button == XBUTTON1)
		{
			PressedKeys[VK_XBUTTON1] = false;
		}
		else if (button == XBUTTON2)
		{
			PressedKeys[VK_XBUTTON2] = false;
		}
		break;
	}
	case WM_KEYDOWN:
		PressedKeys[wParam] = true;
		break;
	case WM_KEYUP:
		PressedKeys[wParam] = false;
		break;
	default: break;
	}

	OpenMenu();

	if (Menu::Get().d3d9 && Menu::Get().isOpen && HanaLovesMe_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam) && !input_shouldListen)
		return true;

	return CallWindowProc(idirect3ddevice9::Get().original_proc, hWnd, uMsg, wParam, lParam);
}
#include "AI218S039877HG/AI218S039877HG.h"
#include "helpers/HanaLovesYou.h"

x019302015vhkg_hook direct3d_hook;


void idirect3ddevice9::ini(IDirect3DDevice9* fix)
{

	HN34207389::call();

	D3DDEVICE_CREATION_PARAMETERS params;
	fix->GetCreationParameters(&params);
	idirect3ddevice9::Get().window = params.hFocusWindow;

	HanaLovesMe::CreateContext();
	HanaLovesMe_ImplWin32_Init(idirect3ddevice9::Get().window);
	HanaLovesMe_ImplDX9_Init(fix);
	if (window) original_proc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wndproc)));



	direct3d_hook.setup(fix);
	direct3d_hook.hook_index(16, reset);
	direct3d_hook.hook_index(17, end_scene);

}
IDirect3DDevice9* idirect3ddevice9::direct_device()
{
	static const auto direct_device = **reinterpret_cast<IDirect3DDevice9***>(Utils::PatternScan(HanaLovesYou("shaderapidx9.dll"), HanaLovesYou("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 1);
	//static const auto direct_device = **reinterpret_cast<IDirect3DDevice9 * **>(Utils::PatternScan("shaderapidx9.dll", "A1 ? ? ? ? 6A 00 53") + 1);

	//A1 ? ? ? ? 6A 00 53

	return direct_device;
}
HRESULT idirect3ddevice9::reset(IDirect3DDevice9* dev, D3DPRESENT_PARAMETERS* param)
{
	static auto oReset = direct3d_hook.get_original<decltype(&reset)>(16);
	HanaLovesMe_ImplDX9_InvalidateDeviceObjects();
	auto hr = oReset(dev, param);
	if (hr >= 0)
		HanaLovesMe_ImplDX9_CreateDeviceObjects();
	return hr;
}
typedef long(__stdcall* fnPresent)(IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);


HRESULT idirect3ddevice9::end_scene(IDirect3DDevice9* pDevice, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion)
{
	static auto oPresent = direct3d_hook.get_original<fnPresent>(17);
	IDirect3DStateBlock9* pixel_state = NULL; IDirect3DVertexDeclaration9* vertDec; IDirect3DVertexShader9* vertShader;
	pDevice->CreateStateBlock(D3DSBT_PIXELSTATE, &pixel_state);
	pDevice->GetVertexDeclaration(&vertDec);
	pDevice->GetVertexShader(&vertShader);
	static auto wanted_ret_address = _ReturnAddress();
	if (_ReturnAddress() == wanted_ret_address)
	{
		DWORD colorwrite, srgbwrite;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
		if (!Menu::Get().d3d9)
		{
			IDirect3DSwapChain9* pChain = nullptr;
			D3DPRESENT_PARAMETERS pp = {};
			D3DDEVICE_CREATION_PARAMETERS param = {};
			pDevice->GetCreationParameters(&param);
			pDevice->GetSwapChain(0, &pChain);
			if (pChain)
				pChain->GetPresentParameters(&pp);
			HanaLovesMe_ImplWin32_Init(param.hFocusWindow);
			HanaLovesMe_ImplDX9_Init(pDevice);


			ImFontConfig font_config;
			font_config.OversampleH = 1; //or 2 is the same
			font_config.OversampleV = 1;
			font_config.PixelSnapH = 1;
			static const ImWchar ranges[] =
			{
				0x0020, 0x00FF, // Basic Latin + Latin Supplement
				0x0400, 0x044F, // Cyrillic
				0,
			};
			HanaLovesMeIO& io = HanaLovesMe::GetIO();

			globals::tabFont = io.Fonts->AddFontFromMemoryTTF(&NanoX, sizeof NanoX, 50.0f, &font_config, ranges);
			//globals::boldMenuFont = HanaLovesMe::GetIO().Fonts->AddFontFromMemoryCompressedTTF(verdanab_compressed_data, verdanab_compressed_size, 11, NULL, HanaLovesMe::GetIO().Fonts->GetGlyphRangesCyrillic());
			globals::menuFont = HanaLovesMe::GetIO().Fonts->AddFontFromMemoryCompressedTTF(verdana_compressed_data, verdana_compressed_size, 11, NULL, HanaLovesMe::GetIO().Fonts->GetGlyphRangesCyrillic());
			globals::controlFont = HanaLovesMe::GetIO().Fonts->AddFontFromMemoryCompressedTTF(comboarrow_compressed_data, comboarrow_compressed_size, 12, NULL, HanaLovesMe::GetIO().Fonts->GetGlyphRangesCyrillic());
			globals::weaponFont = io.Fonts->AddFontFromMemoryTTF(&WeaponFont, sizeof WeaponFont, 26.0f, &font_config, ranges);
			globals::weaponzFont = io.Fonts->AddFontFromMemoryTTF(&WeaponzFont, sizeof WeaponzFont, 20.0f, &font_config, ranges);
			globals::SliderFont = io.Fonts->AddFontFromMemoryTTF(&SliderFontz, sizeof SliderFontz, 11.5, NULL, ranges);
			globals::boldMenuFont = io.Fonts->AddFontFromMemoryTTF(&tahomabd, sizeof tahomabd, 12.0f, &font_config, ranges);
			globals::pixelFont = io.Fonts->AddFontFromMemoryTTF(&CHLCFont, sizeof CHLCFont, 10.0f, &font_config, ranges);
		//	globals::TestFont = io.Fonts->AddFont
			//globals::menuFont = io.Fonts->AddFontFromMemoryTTF(&tahoma, sizeof tahoma, 12.0f, &font_config, ranges);



			//	forTabs_text = HanaLovesMe::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Ubuntu_compressed_data, Ubuntu_compressed_size, 15, NULL, HanaLovesMe::GetIO().Fonts->GetGlyphRangesCyrillic());


			if (globals::menuBg == nullptr)D3DXCreateTextureFromFileInMemoryEx(pDevice
				, &menuBackground2, sizeof(menuBackground2),
				648, 598, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, NULL, NULL, &globals::menuBg);

			if (globals::byvsonyp0wer == nullptr)D3DXCreateTextureFromFileInMemoryEx(pDevice
				, &r79r23rfb8e3qwriu3qw, sizeof(r79r23rfb8e3qwriu3qw),
				210, 206, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, NULL, NULL, &globals::byvsonyp0wer);

			if (globals::rainbowline == nullptr)D3DXCreateTextureFromFileInMemoryEx(pDevice
				, &skeetline, sizeof(skeetline),
				1280, 87, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, NULL, NULL, &globals::rainbowline);



			Menu::Get().d3d9 = true;

		}

		HanaLovesMe_ImplWin32_NewFrame();
		HanaLovesMe_ImplDX9_NewFrame();
		HanaLovesMe::NewFrame();

		int w, h;
		g_EngineClient->GetScreenSize(w, h);
		static bool visible = true;
		HanaLovesMeStyle* stylez = &HanaLovesMe::GetStyle();
		stylez->WindowPadding = vsize(6, 6);
		HanaLovesMe::Begin("fat", &visible, HanaLovesMeWindowFlags_NoInputs | HanaLovesMeWindowFlags_NoMove | HanaLovesMeWindowFlags_NoBackground | HanaLovesMeWindowFlags_NoCollapse | HanaLovesMeWindowFlags_NoResize | HanaLovesMeWindowFlags_NoScrollbar | HanaLovesMeWindowFlags_NoScrollWithMouse | HanaLovesMeWindowFlags_NoTitleBar);
		HanaLovesMe::SetWindowPos(vsize(-7, -6));
		HanaLovesMe::SetWindowSize(vsize(w + 10, h));
		HanaLovesMe::Image(globals::rainbowline, vsize(w + 1, 4.f));
		HanaLovesMe::NewLine(); HanaLovesMe::SameLine(w - 230);
		HanaLovesMe::PushFont(globals::pixelFont);
		HanaLovesMe::Text(HanaLovesYou("NanoSense beta client v12.0"));
		HanaLovesMe::PopFont();
		HanaLovesMe::End();

		if (Menu::Get().isOpen)
			Menu::Get().Render();

		HanaLovesMe::Render();
		HanaLovesMe_ImplDX9_RenderDrawData(HanaLovesMe::GetDrawData());


		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
	}

	pixel_state->Apply();
	pixel_state->Release();
	pDevice->SetVertexDeclaration(vertDec);
	pDevice->SetVertexShader(vertShader);
	return oPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

// Junk Code By Troll Face & Thaisen's Gen
void FdMBhWiDYEVQUxDrykqDekWQyeMGVtKxiixYLAlzoOJmH47838299() {     int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW8893387 = -529629517;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW83701112 = -590506821;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51181723 = -815259;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW21017085 = -342309023;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW89399706 = -602941467;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW71691320 = -659057114;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51973646 = -791637276;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW59001594 = -307055503;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW17102613 = -993494644;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW97205926 = -393923251;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW27142330 = -567828557;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51390681 = -669691912;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW61785420 = 5979523;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW36546720 = -23561470;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW53748895 = -859728682;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW23655075 = -21101735;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW66074633 = -786208125;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW20822534 = -280255110;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW44427200 = -734662796;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW26198421 = -587540676;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW70430480 = -465767195;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW24753508 = -9096120;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW10509068 = -379809850;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW10457225 = -930996839;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW69682531 = -310654789;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW92126841 = -681620994;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51437737 = -234563870;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW6970204 = -431331054;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW67801766 = -105752137;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW83069223 = -525335967;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW8137557 = -280718671;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW72152552 = 88813137;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW87036516 = -286238864;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW91406157 = -526162009;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW7206792 = -782128930;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW32136597 = -479762985;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW17718014 = -555293588;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW23912042 = -276444994;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW83199110 = -13605319;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW41692762 = -131912818;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW14025294 = -641069978;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW48120548 = -522289746;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW79901941 = -189513431;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW67345031 = -352095019;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW61185411 = -78435399;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW7115118 = -681918018;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW7605915 = -59050855;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW20174905 = -78086158;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW81402387 = -195082614;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW42033292 = -643067305;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW73157399 = -454551509;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW56565489 = 6383033;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW65729393 = -760783120;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW12764524 = -551416058;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW77407818 = -366080843;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW84139878 = -420533398;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW73192045 = -110696972;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW40724498 = -69818421;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51334553 = 68345765;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW97272864 = -921320473;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW20253584 = -324493244;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW45003442 = -260306223;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW91199827 = -101303366;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW34033389 = -368158677;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW89068370 = -13204580;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW54989778 = -556641695;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW64354164 = -283453048;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW70379262 = -467858469;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW29339928 = -241432540;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW21612299 = -279965698;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW5937062 = -465808148;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW42162591 = -409763132;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW37623423 = -166649792;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW2734439 = -502749978;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW12173128 = -946470698;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW22309932 = -943477449;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW44851566 = -819582689;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW43164037 = 72285168;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW49271814 = -752561440;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW62567414 = -628736772;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW84520926 = -522570140;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW31262832 = -56477713;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW25567817 = -136248440;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW25768474 = -462684832;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW9911825 = 29215541;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51572068 = -187101705;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW6423160 = -150403744;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW74271993 = -734822806;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW13998340 = -60081167;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW23066913 = -261595532;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW58944552 = -269066013;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW76993515 = -385475167;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW72577488 = -244790759;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW85926246 = -92284846;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW21439178 = -807419574;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW69021851 = -280763756;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW56920720 = -320986381;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW45868553 = -821354755;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW78276660 = -238890439;    int mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW6195633 = -529629517;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW8893387 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW83701112;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW83701112 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51181723;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51181723 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW21017085;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW21017085 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW89399706;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW89399706 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW71691320;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW71691320 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51973646;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51973646 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW59001594;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW59001594 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW17102613;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW17102613 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW97205926;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW97205926 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW27142330;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW27142330 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51390681;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51390681 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW61785420;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW61785420 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW36546720;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW36546720 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW53748895;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW53748895 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW23655075;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW23655075 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW66074633;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW66074633 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW20822534;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW20822534 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW44427200;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW44427200 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW26198421;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW26198421 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW70430480;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW70430480 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW24753508;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW24753508 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW10509068;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW10509068 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW10457225;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW10457225 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW69682531;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW69682531 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW92126841;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW92126841 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51437737;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51437737 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW6970204;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW6970204 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW67801766;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW67801766 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW83069223;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW83069223 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW8137557;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW8137557 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW72152552;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW72152552 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW87036516;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW87036516 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW91406157;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW91406157 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW7206792;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW7206792 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW32136597;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW32136597 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW17718014;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW17718014 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW23912042;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW23912042 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW83199110;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW83199110 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW41692762;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW41692762 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW14025294;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW14025294 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW48120548;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW48120548 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW79901941;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW79901941 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW67345031;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW67345031 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW61185411;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW61185411 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW7115118;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW7115118 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW7605915;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW7605915 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW20174905;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW20174905 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW81402387;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW81402387 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW42033292;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW42033292 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW73157399;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW73157399 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW56565489;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW56565489 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW65729393;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW65729393 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW12764524;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW12764524 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW77407818;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW77407818 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW84139878;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW84139878 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW73192045;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW73192045 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW40724498;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW40724498 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51334553;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51334553 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW97272864;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW97272864 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW20253584;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW20253584 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW45003442;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW45003442 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW91199827;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW91199827 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW34033389;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW34033389 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW89068370;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW89068370 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW54989778;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW54989778 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW64354164;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW64354164 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW70379262;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW70379262 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW29339928;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW29339928 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW21612299;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW21612299 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW5937062;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW5937062 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW42162591;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW42162591 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW37623423;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW37623423 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW2734439;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW2734439 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW12173128;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW12173128 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW22309932;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW22309932 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW44851566;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW44851566 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW43164037;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW43164037 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW49271814;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW49271814 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW62567414;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW62567414 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW84520926;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW84520926 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW31262832;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW31262832 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW25567817;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW25567817 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW25768474;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW25768474 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW9911825;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW9911825 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51572068;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW51572068 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW6423160;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW6423160 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW74271993;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW74271993 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW13998340;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW13998340 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW23066913;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW23066913 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW58944552;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW58944552 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW76993515;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW76993515 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW72577488;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW72577488 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW85926246;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW85926246 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW21439178;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW21439178 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW69021851;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW69021851 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW56920720;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW56920720 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW45868553;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW45868553 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW78276660;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW78276660 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW6195633;     mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW6195633 = mkyOsVnQSaUHwCCJcifuYimzfgxzrPQajIMuOscXW8893387;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void echMTDFnLBUhwZoPUxbqywzlVQNtblszLBTevtMZDwNXBTcdKLioDtghcvrCqMLaJUVvFJUakBnBickBnjiQ78063997() {     float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg66171340 = -950383818;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg27727153 = -874954837;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg84952076 = -242264173;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg17155516 = -828514689;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg10239358 = -819968886;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg49335224 = -782664573;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg75925304 = -1949367;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg42602809 = -911559134;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg56513939 = -774444503;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg71711621 = -297232082;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg46400636 = 94669976;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg75286551 = -504733462;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg48935465 = -544896008;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg33576161 = 77421799;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg29787262 = -171352024;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg42449375 = -815677709;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg43286008 = -508814681;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg44608295 = 76060097;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg9828889 = -333353008;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg37182163 = -711359024;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg80506340 = -810353768;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg81725143 = -971479520;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg8945050 = -352948310;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg63352846 = -692380570;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg15852495 = 91116126;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg50687905 = -651486297;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg70005705 = -818051912;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg980218 = -770034949;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg68706116 = -119828030;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg22464647 = -773773019;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg94592393 = -331973523;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg21228825 = -755957992;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg67300204 = -827410835;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg93835120 = -755016316;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg95028477 = 87295252;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg74688495 = -198627162;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg66182770 = 30566586;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg14024147 = -838274669;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg91626916 = -343227596;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg4571387 = -409440208;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg95780580 = -338380100;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg46836677 = 98802824;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg96794399 = -66226161;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg35187177 = -643715820;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg83126706 = -878264891;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg12496910 = -880296234;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg66468087 = -964685047;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg7432759 = -837489419;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg70688340 = -375817872;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg92633069 = -906661744;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg49019627 = -161994895;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg43713254 = -173720273;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg13297581 = -976450815;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg61906628 = -649893543;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg97557656 = -552142382;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg84446196 = -978904299;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg18782104 = -422006528;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg21599230 = -549883604;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg1303022 = -819630815;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg59551453 = -68482590;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg79329518 = -964612662;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg74945087 = -231914419;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg73896692 = -691731105;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg34049292 = 99328516;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg77119228 = -965258559;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg25171811 = -149372033;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg7986348 = -677322627;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg55100345 = -789879693;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg38547683 = 90126547;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg55098766 = -972724862;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg76266604 = -746244295;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg29261861 = -670540012;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg52981378 = -580712307;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg5257502 = -923912801;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg41401583 = -272978924;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg33669664 = -809156592;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg84930744 = -805253359;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg73757872 = -709232491;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg80226139 = -814115679;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg3355585 = -28587640;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg84219818 = -686801251;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg62572947 = -980562493;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg30291877 = -294217077;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg76073047 = -213166287;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg73445020 = -511778124;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg50879139 = -58253251;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg7931245 = -779507178;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg5393577 = -77517293;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg96277463 = -102873935;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg10582281 = 66199550;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg55906391 = -776620635;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg44583541 = -419549811;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg12721126 = 81356146;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg32075464 = -174745007;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg25241868 = -444827547;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg20835493 = -6465681;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg72939984 = -209466072;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg62745107 = -65554678;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg58067949 = -678457261;    float DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg57954895 = -950383818;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg66171340 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg27727153;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg27727153 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg84952076;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg84952076 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg17155516;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg17155516 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg10239358;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg10239358 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg49335224;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg49335224 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg75925304;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg75925304 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg42602809;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg42602809 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg56513939;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg56513939 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg71711621;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg71711621 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg46400636;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg46400636 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg75286551;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg75286551 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg48935465;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg48935465 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg33576161;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg33576161 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg29787262;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg29787262 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg42449375;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg42449375 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg43286008;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg43286008 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg44608295;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg44608295 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg9828889;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg9828889 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg37182163;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg37182163 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg80506340;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg80506340 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg81725143;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg81725143 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg8945050;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg8945050 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg63352846;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg63352846 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg15852495;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg15852495 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg50687905;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg50687905 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg70005705;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg70005705 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg980218;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg980218 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg68706116;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg68706116 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg22464647;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg22464647 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg94592393;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg94592393 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg21228825;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg21228825 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg67300204;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg67300204 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg93835120;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg93835120 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg95028477;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg95028477 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg74688495;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg74688495 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg66182770;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg66182770 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg14024147;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg14024147 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg91626916;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg91626916 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg4571387;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg4571387 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg95780580;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg95780580 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg46836677;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg46836677 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg96794399;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg96794399 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg35187177;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg35187177 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg83126706;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg83126706 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg12496910;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg12496910 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg66468087;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg66468087 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg7432759;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg7432759 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg70688340;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg70688340 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg92633069;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg92633069 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg49019627;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg49019627 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg43713254;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg43713254 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg13297581;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg13297581 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg61906628;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg61906628 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg97557656;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg97557656 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg84446196;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg84446196 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg18782104;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg18782104 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg21599230;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg21599230 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg1303022;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg1303022 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg59551453;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg59551453 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg79329518;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg79329518 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg74945087;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg74945087 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg73896692;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg73896692 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg34049292;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg34049292 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg77119228;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg77119228 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg25171811;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg25171811 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg7986348;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg7986348 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg55100345;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg55100345 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg38547683;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg38547683 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg55098766;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg55098766 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg76266604;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg76266604 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg29261861;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg29261861 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg52981378;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg52981378 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg5257502;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg5257502 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg41401583;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg41401583 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg33669664;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg33669664 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg84930744;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg84930744 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg73757872;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg73757872 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg80226139;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg80226139 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg3355585;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg3355585 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg84219818;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg84219818 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg62572947;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg62572947 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg30291877;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg30291877 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg76073047;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg76073047 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg73445020;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg73445020 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg50879139;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg50879139 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg7931245;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg7931245 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg5393577;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg5393577 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg96277463;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg96277463 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg10582281;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg10582281 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg55906391;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg55906391 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg44583541;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg44583541 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg12721126;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg12721126 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg32075464;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg32075464 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg25241868;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg25241868 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg20835493;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg20835493 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg72939984;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg72939984 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg62745107;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg62745107 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg58067949;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg58067949 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg57954895;     DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg57954895 = DDwSUmCzCKjrknNgygXAqHEBGezALpOGduXFXDbTHUYaRvRvODDGMiZvzuLpnCoMvdNqPHJvFDsWiekKNElImHuuUbGcrmgFg66171340;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void nEdYjBRDWUqLqOOIMceLKAVExZGFlHwjcaHtuztpSsKoMFXbgucLhnumjZaJuLSPXGiPHVNMjosSqgrrCeZq76218204() {     long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF66781851 = -4868282;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF93712505 = -688019845;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF41683510 = -511904473;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF36157937 = -351131497;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF4253368 = -398740890;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF66384901 = -368986751;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF97021337 = -254420249;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF81941289 = -882374109;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94477909 = -158257418;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF82068392 = 8772961;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF93201698 = -665715046;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF95482039 = -892978816;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94188096 = -801465353;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF75664144 = -485130040;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF16968475 = -275773506;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF80682863 = -674162923;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF44618378 = -679288240;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF1138591 = -953669135;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF55792721 = -823847555;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF58043133 = -989830039;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF79043981 = -41494944;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF51595917 = -719436317;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94991929 = -301860902;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94821184 = -895494145;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF17717176 = -388193617;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF23508480 = -282524238;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF83062520 = -593295948;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF31893549 = -298627952;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF65956058 = -255551794;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF83702124 = -190936940;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF20500614 = -473359254;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF68415931 = -683500815;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF39865785 = -509855335;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF13947565 = -36487737;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF90674772 = -107153307;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF80035144 = -48012887;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF81160059 = -897386414;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF13043022 = -153174595;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF9061192 = -371752427;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF35718996 = -588632054;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF24809867 = 21495853;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF73576390 = -703487509;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF66799525 = -410527465;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF6000551 = -724266205;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF66007464 = -503907726;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF46082775 = -212560941;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF8602365 = -821259568;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF29361692 = -474245093;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF95921375 = -312747153;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF54182723 = -749474311;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF88940197 = -664208893;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF59857376 = -412828119;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF30766988 = -559891315;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF9755320 = -521409468;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF97174223 = -405066392;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF15185934 = -285431965;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF98720576 = -286158943;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF46862325 = -616410328;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF18440761 = -962937881;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF80744887 = -16216652;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF83322381 = -775690804;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF65127788 = -955792297;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF15985232 = -526822316;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF10775786 = -967320479;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF61567779 = -517867786;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF24785767 = -982214231;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF55616255 = -283123481;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF80240532 = -664977616;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF84989371 = -277976733;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF36933330 = -127760619;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF99522803 = -776776510;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF31575357 = -426113645;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF92077399 = -481916709;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF20073725 = -135215502;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF33233266 = -911325892;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF5467591 = -338007435;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF84796392 = -208908853;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF88991378 = -577594698;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF28813721 = -291586420;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF71634401 = -75632676;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF14906116 = -461264670;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF53700828 = -19050856;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF35972174 = -985880799;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF11773335 = -506077483;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94761927 = -526728047;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF60643237 = 39468865;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF37648944 = -23609500;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF30110465 = -988445867;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF16773342 = -631421345;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF75488839 = -821721343;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF81314567 = -761853944;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF34297735 = -180976087;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94602260 = -190236715;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF28316304 = -255535775;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF52396615 = -812941251;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF59682079 = -22711851;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF57591159 = -76665194;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF56023739 = -443206986;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF44432772 = -106398420;    long flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF41221697 = -4868282;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF66781851 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF93712505;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF93712505 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF41683510;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF41683510 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF36157937;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF36157937 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF4253368;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF4253368 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF66384901;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF66384901 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF97021337;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF97021337 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF81941289;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF81941289 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94477909;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94477909 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF82068392;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF82068392 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF93201698;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF93201698 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF95482039;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF95482039 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94188096;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94188096 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF75664144;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF75664144 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF16968475;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF16968475 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF80682863;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF80682863 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF44618378;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF44618378 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF1138591;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF1138591 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF55792721;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF55792721 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF58043133;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF58043133 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF79043981;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF79043981 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF51595917;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF51595917 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94991929;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94991929 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94821184;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94821184 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF17717176;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF17717176 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF23508480;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF23508480 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF83062520;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF83062520 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF31893549;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF31893549 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF65956058;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF65956058 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF83702124;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF83702124 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF20500614;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF20500614 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF68415931;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF68415931 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF39865785;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF39865785 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF13947565;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF13947565 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF90674772;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF90674772 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF80035144;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF80035144 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF81160059;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF81160059 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF13043022;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF13043022 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF9061192;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF9061192 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF35718996;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF35718996 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF24809867;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF24809867 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF73576390;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF73576390 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF66799525;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF66799525 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF6000551;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF6000551 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF66007464;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF66007464 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF46082775;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF46082775 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF8602365;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF8602365 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF29361692;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF29361692 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF95921375;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF95921375 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF54182723;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF54182723 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF88940197;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF88940197 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF59857376;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF59857376 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF30766988;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF30766988 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF9755320;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF9755320 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF97174223;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF97174223 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF15185934;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF15185934 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF98720576;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF98720576 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF46862325;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF46862325 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF18440761;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF18440761 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF80744887;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF80744887 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF83322381;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF83322381 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF65127788;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF65127788 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF15985232;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF15985232 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF10775786;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF10775786 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF61567779;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF61567779 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF24785767;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF24785767 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF55616255;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF55616255 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF80240532;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF80240532 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF84989371;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF84989371 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF36933330;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF36933330 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF99522803;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF99522803 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF31575357;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF31575357 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF92077399;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF92077399 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF20073725;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF20073725 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF33233266;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF33233266 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF5467591;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF5467591 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF84796392;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF84796392 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF88991378;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF88991378 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF28813721;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF28813721 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF71634401;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF71634401 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF14906116;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF14906116 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF53700828;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF53700828 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF35972174;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF35972174 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF11773335;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF11773335 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94761927;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94761927 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF60643237;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF60643237 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF37648944;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF37648944 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF30110465;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF30110465 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF16773342;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF16773342 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF75488839;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF75488839 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF81314567;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF81314567 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF34297735;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF34297735 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94602260;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF94602260 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF28316304;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF28316304 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF52396615;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF52396615 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF59682079;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF59682079 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF57591159;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF57591159 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF56023739;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF56023739 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF44432772;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF44432772 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF41221697;     flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF41221697 = flBpgUKVZjuGauQFWHUKgPbsubvChiHPmbjmEtimKsnzumswQlVuKOKQJJuEYInqceTyLeAdYhAocbyIjzlvNMFYRTcvyeOCF66781851;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void mviDsebLBRhrhnIhuvUywkiAmfslhVHGlLKJSSqAUHJVoFUEIkCcHHQCNiQMlpefKaFVcyObETeoaMLhKndwTezduI32179018() {     double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs98471292 = -386851556;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs10627562 = -738287659;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs93231293 = -379456157;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs79114368 = -190284678;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs60171065 = -803023700;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs44422245 = -187329613;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs99925819 = -384729367;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs85139406 = -958284551;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs49930026 = -949806269;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs35735461 = 71622217;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs8286754 = -772524664;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs91206091 = -152050659;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs20683821 = 51078244;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs58055978 = -582199006;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs59324007 = -780207352;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs79038395 = -648945245;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs49958017 = -623382286;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs65308713 = -829612386;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs19943824 = 51444959;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs43902291 = -345221236;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs27945288 = -879614933;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs96212579 = -967082730;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs8103313 = 89613822;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs67827818 = -738489158;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs73205344 = -82273511;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs10867447 = -642041956;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs4353371 = -661370711;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs79706524 = -712153121;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs65322420 = -906368963;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs57819992 = -368180117;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs1615155 = -939513692;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs72664201 = -350386985;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs41668631 = -154178271;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs62930874 = -697537939;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs18270489 = -865950171;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs39209288 = -939165185;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs35950115 = -430840985;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs62541280 = -863920791;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs94222217 = -49377066;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs75750392 = -750904968;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs53341707 = -794726028;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs21131760 = -770252435;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs14633443 = -538198936;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs82665367 = -325953629;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs37976910 = 48654200;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs5278229 = -344413918;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs81544977 = -835065211;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs96198565 = -794846183;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs93725203 = -725688066;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs60995216 = -958558050;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs61295863 = 87728819;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs27020191 = -55925111;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs47985745 = -703687472;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs95123649 = -20783097;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs4233730 = 5512858;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs2258713 = -419768827;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs2524249 = -727901481;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs25403476 = -640967000;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs5909025 = -8011168;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs49303619 = -60981744;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs40068874 = -525958902;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs20219295 = -672576246;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs19816987 = 48084412;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs92110034 = -481626153;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs34120307 = 11135909;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs35622553 = -322137680;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs49537461 = -997872388;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs57752946 = -251383818;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs39785489 = -716248836;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs20114720 = -841042168;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs43088280 = -118104260;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs87416736 = -759461496;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs71086496 = -680235321;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs44193431 = -197650074;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs90560583 = -550495208;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs6813529 = -9362498;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs81579136 = -328883794;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs25437945 = -584432550;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs29850908 = -687143358;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs67927116 = -737859593;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs29322469 = -806976745;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs8154806 = -866524529;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs85981321 = -986465055;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs4327204 = -947810913;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs96524129 = -355908936;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs74594964 = -783588581;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs24678456 = -646699514;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs46544981 = -33395174;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs58697144 = -603050797;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs16011776 = -346181344;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs36685039 = -111263704;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs10546640 = -789873986;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs56632256 = -755909624;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs44918598 = -988395322;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs35681519 = -124946066;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs33122412 = -22149782;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs1314774 = -718336848;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs22523408 = 43427217;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs48545061 = -237089538;    double wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs2354357 = -386851556;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs98471292 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs10627562;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs10627562 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs93231293;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs93231293 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs79114368;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs79114368 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs60171065;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs60171065 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs44422245;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs44422245 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs99925819;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs99925819 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs85139406;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs85139406 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs49930026;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs49930026 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs35735461;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs35735461 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs8286754;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs8286754 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs91206091;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs91206091 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs20683821;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs20683821 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs58055978;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs58055978 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs59324007;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs59324007 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs79038395;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs79038395 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs49958017;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs49958017 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs65308713;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs65308713 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs19943824;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs19943824 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs43902291;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs43902291 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs27945288;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs27945288 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs96212579;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs96212579 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs8103313;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs8103313 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs67827818;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs67827818 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs73205344;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs73205344 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs10867447;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs10867447 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs4353371;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs4353371 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs79706524;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs79706524 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs65322420;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs65322420 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs57819992;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs57819992 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs1615155;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs1615155 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs72664201;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs72664201 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs41668631;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs41668631 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs62930874;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs62930874 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs18270489;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs18270489 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs39209288;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs39209288 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs35950115;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs35950115 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs62541280;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs62541280 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs94222217;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs94222217 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs75750392;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs75750392 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs53341707;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs53341707 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs21131760;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs21131760 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs14633443;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs14633443 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs82665367;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs82665367 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs37976910;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs37976910 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs5278229;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs5278229 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs81544977;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs81544977 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs96198565;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs96198565 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs93725203;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs93725203 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs60995216;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs60995216 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs61295863;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs61295863 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs27020191;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs27020191 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs47985745;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs47985745 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs95123649;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs95123649 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs4233730;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs4233730 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs2258713;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs2258713 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs2524249;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs2524249 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs25403476;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs25403476 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs5909025;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs5909025 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs49303619;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs49303619 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs40068874;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs40068874 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs20219295;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs20219295 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs19816987;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs19816987 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs92110034;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs92110034 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs34120307;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs34120307 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs35622553;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs35622553 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs49537461;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs49537461 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs57752946;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs57752946 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs39785489;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs39785489 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs20114720;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs20114720 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs43088280;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs43088280 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs87416736;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs87416736 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs71086496;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs71086496 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs44193431;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs44193431 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs90560583;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs90560583 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs6813529;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs6813529 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs81579136;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs81579136 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs25437945;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs25437945 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs29850908;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs29850908 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs67927116;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs67927116 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs29322469;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs29322469 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs8154806;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs8154806 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs85981321;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs85981321 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs4327204;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs4327204 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs96524129;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs96524129 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs74594964;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs74594964 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs24678456;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs24678456 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs46544981;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs46544981 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs58697144;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs58697144 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs16011776;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs16011776 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs36685039;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs36685039 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs10546640;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs10546640 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs56632256;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs56632256 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs44918598;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs44918598 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs35681519;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs35681519 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs33122412;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs33122412 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs1314774;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs1314774 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs22523408;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs22523408 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs48545061;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs48545061 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs2354357;     wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs2354357 = wkBeOeddLOzzcJWaYndcbAWiByMhMfacaTBeZaFAKwBxhLKTasGLzShwOScQSwRfnWYIIs98471292;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void TLMhAyIRWrnEAgijamVUnBOQhwNFLkLxgzukvlYBqNaRFCzoIHzPBwKSwwzEkDFmgqYAlynoOUdZbxLewZwEkwwjpb81150889() {     double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe84828073 = -361314127;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe97395603 = -182930024;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe2885300 = -718135228;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe23346021 = -394458615;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe72860229 = 56968911;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe67023121 = -735957687;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe86836333 = -307858224;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe79084524 = 73591440;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe27676225 = -160916349;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe78755488 = -870716053;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe61341893 = -485746708;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe21111504 = -126589847;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe47291032 = 43398486;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe25725802 = -701065444;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe41559395 = -166337090;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe18032368 = -688295830;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe43423900 = -301847022;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe51074358 = 25023147;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe44458987 = -323109544;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe3248478 = 78070323;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe51682134 = -664885316;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe35489299 = -361189483;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe83927733 = -287739191;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe54962603 = -518683942;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe67560749 = -395879126;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe6319205 = -88856609;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe87190982 = -682690328;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe9961450 = 84203838;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe79229564 = -645290409;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe74193528 = -385318514;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe67609867 = -80354187;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe38456366 = -745126165;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe40222508 = 48869604;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe6004403 = -185345485;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe15654286 = -223982450;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe53593 = -818781624;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe41561923 = -715415420;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe85181418 = -358070644;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe59525698 = -883587499;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe96022399 = -252623109;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe62121613 = -447610270;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe33966465 = 43034675;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe32900823 = -488115221;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe49860786 = -287391831;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe1894315 = -784782671;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe38951347 = -920967205;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe15565719 = 86633008;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe35695587 = -975576935;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe96555019 = -810831494;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe72558527 = -33611301;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe82246328 = -760438957;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe54470639 = -549796598;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe46035809 = -803291062;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe17407197 = -552203508;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe29468756 = -466018028;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe49338775 = 99875356;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe13467871 = -895190834;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe47922696 = -99451287;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe55785271 = -998579490;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe66541024 = -854174480;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe79832138 = 46732640;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe76874883 = -292062062;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe99854960 = -281118152;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe53482696 = -775597836;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe11145621 = -690361867;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe22885528 = -740620543;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe80888995 = -75459452;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe41286630 = -771256030;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe10071517 = -377082995;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe41505802 = -347555467;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe76470444 = -972880410;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe58242481 = -943776379;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe91548659 = -91389355;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe48436588 = 29513564;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe41126865 = -474319408;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe17715670 = -607919991;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe2588476 = -873074262;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe34066947 = 99652640;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe53068288 = -733901272;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe28609402 = -474911921;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe90753486 = -75489617;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe51495396 = -707113393;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe13406431 = -104964668;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe6671038 = -511679109;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe91947200 = -624879558;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe13139228 = -530557590;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe92420556 = -941835104;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe22815312 = -398926888;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe76535647 = -719327457;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe66315511 = -223857806;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe86585722 = -923590790;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe93639226 = -515964134;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe29396148 = -359491154;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe92984674 = 70586981;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe16190261 = -199355749;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe85246729 = -55548209;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe34111505 = -675847173;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe79418126 = -712517386;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe38715165 = -597029965;    double nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe79008787 = -361314127;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe84828073 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe97395603;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe97395603 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe2885300;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe2885300 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe23346021;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe23346021 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe72860229;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe72860229 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe67023121;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe67023121 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe86836333;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe86836333 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe79084524;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe79084524 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe27676225;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe27676225 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe78755488;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe78755488 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe61341893;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe61341893 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe21111504;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe21111504 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe47291032;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe47291032 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe25725802;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe25725802 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe41559395;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe41559395 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe18032368;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe18032368 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe43423900;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe43423900 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe51074358;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe51074358 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe44458987;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe44458987 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe3248478;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe3248478 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe51682134;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe51682134 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe35489299;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe35489299 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe83927733;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe83927733 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe54962603;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe54962603 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe67560749;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe67560749 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe6319205;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe6319205 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe87190982;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe87190982 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe9961450;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe9961450 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe79229564;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe79229564 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe74193528;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe74193528 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe67609867;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe67609867 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe38456366;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe38456366 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe40222508;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe40222508 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe6004403;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe6004403 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe15654286;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe15654286 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe53593;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe53593 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe41561923;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe41561923 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe85181418;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe85181418 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe59525698;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe59525698 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe96022399;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe96022399 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe62121613;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe62121613 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe33966465;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe33966465 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe32900823;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe32900823 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe49860786;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe49860786 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe1894315;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe1894315 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe38951347;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe38951347 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe15565719;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe15565719 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe35695587;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe35695587 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe96555019;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe96555019 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe72558527;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe72558527 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe82246328;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe82246328 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe54470639;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe54470639 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe46035809;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe46035809 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe17407197;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe17407197 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe29468756;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe29468756 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe49338775;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe49338775 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe13467871;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe13467871 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe47922696;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe47922696 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe55785271;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe55785271 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe66541024;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe66541024 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe79832138;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe79832138 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe76874883;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe76874883 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe99854960;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe99854960 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe53482696;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe53482696 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe11145621;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe11145621 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe22885528;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe22885528 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe80888995;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe80888995 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe41286630;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe41286630 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe10071517;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe10071517 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe41505802;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe41505802 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe76470444;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe76470444 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe58242481;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe58242481 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe91548659;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe91548659 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe48436588;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe48436588 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe41126865;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe41126865 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe17715670;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe17715670 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe2588476;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe2588476 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe34066947;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe34066947 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe53068288;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe53068288 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe28609402;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe28609402 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe90753486;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe90753486 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe51495396;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe51495396 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe13406431;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe13406431 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe6671038;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe6671038 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe91947200;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe91947200 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe13139228;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe13139228 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe92420556;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe92420556 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe22815312;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe22815312 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe76535647;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe76535647 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe66315511;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe66315511 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe86585722;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe86585722 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe93639226;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe93639226 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe29396148;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe29396148 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe92984674;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe92984674 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe16190261;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe16190261 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe85246729;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe85246729 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe34111505;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe34111505 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe79418126;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe79418126 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe38715165;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe38715165 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe79008787;     nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe79008787 = nzmUQnInkOezCKTKAgTkrykUHLGBBkuoLKKoCuEpRnMwAxXmlavPQEkoORMFhGmAUposAe84828073;}
// Junk Finished
