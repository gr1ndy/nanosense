#include "Miscellaneous.hpp"

#include "../Structs.hpp"
#include "../Options.hpp"
#include "../helpers/Math.hpp"

#include "AntiAim.hpp"
#include "AimRage.hpp"
#include "PredictionSystem.hpp"
#include "..//HNJ.h"
#include <chrono>

template<class T, class U>
T Clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}

std::vector<std::string> msgs =
{
	"fat",
};

void Miscellaneous::Bhop(CUserCmd *userCMD)
{
	if (!g_LocalPlayer->IsAlive())
		return;

	if (g_LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->GetMoveType() == MOVETYPE_LADDER) return;
	if (userCMD->buttons & IN_JUMP && !(g_LocalPlayer->m_fFlags() & FL_ONGROUND)) {
		userCMD->buttons &= ~IN_JUMP;
	}
	HN34207389::call();
}

void Miscellaneous::AutoStrafe(CUserCmd *userCMD)
{
	if (!g_LocalPlayer->IsAlive())
		return;

	if (g_LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->GetMoveType() == MOVETYPE_LADDER || !g_LocalPlayer->IsAlive()) return;

	// If we're not jumping or want to manually move out of the way/jump over an obstacle don't strafe.
	if (!g_InputSystem->IsButtonDown(ButtonCode_t::KEY_SPACE) ||
		g_InputSystem->IsButtonDown(ButtonCode_t::KEY_A) ||
		g_InputSystem->IsButtonDown(ButtonCode_t::KEY_D) ||
		g_InputSystem->IsButtonDown(ButtonCode_t::KEY_S) ||
		g_InputSystem->IsButtonDown(ButtonCode_t::KEY_W))
		return;

	if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND)) {
		if (userCMD->mousedx > 1 || userCMD->mousedx < -1) {
			userCMD->sidemove = clamp(userCMD->mousedx < 0.f ? -400.f : 400.f, -400, 400);
		}
		else {
			if (g_LocalPlayer->m_vecVelocity().Length2D() == 0 || g_LocalPlayer->m_vecVelocity().Length2D() == NAN || g_LocalPlayer->m_vecVelocity().Length2D() == INFINITE)
			{
				userCMD->forwardmove = 400;
				return;
			}
			userCMD->forwardmove = clamp(5850.f / g_LocalPlayer->m_vecVelocity().Length2D(), -400, 400);
			if (userCMD->forwardmove < -400 || userCMD->forwardmove > 400)
				userCMD->forwardmove = 0;
			userCMD->sidemove = clamp((userCMD->command_number % 2) == 0 ? -400.f : 400.f, -400, 400);
			if (userCMD->sidemove < -400 || userCMD->sidemove > 400)
				userCMD->sidemove = 0;
		}
	}
	HN34207389::call();
}
void Miscellaneous::FakeDuck(CUserCmd* userCMD)
{
if (g_Options.misc_fakeduck && g_Options.misc_fakeduck_keybind && GetAsyncKeyState(g_Options.misc_fakeduck_keybind) && g_LocalPlayer->IsAlive())

if (g_LocalPlayer && g_LocalPlayer->IsAlive())

if (g_EngineClient->GetNetChannel()->m_nChokedPackets > (g_Options.misc_fakeduck_chokedpackets / 2))
	userCMD->buttons |= IN_DUCK;
	else
	userCMD->buttons &= ~IN_DUCK;

HN34207389::call();
}

void Miscellaneous::Fakelag(CUserCmd* userCMD)
{
	if (!g_LocalPlayer->IsAlive())
		return;

	if (!g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
		return;

	if (!g_Options.misc_fakelag_enabled) // just add a check in createmove for this..
		return;

	int choke = std::min<int>(g_Options.misc_fakelag_adaptive ? static_cast<int>(std::ceilf(64 / (g_LocalPlayer->m_vecVelocity().Length() * g_GlobalVars->interval_per_tick))) : g_Options.misc_fakelag_value, 14);

	if (Global::bAimbotting && userCMD->buttons & IN_ATTACK)
		return;

	if (g_Options.misc_fakelag_activation_type == 1 && g_LocalPlayer->m_vecVelocity().Length() < 3.0f)
		return;
	if (g_Options.misc_fakelag_activation_type == 2 && (g_LocalPlayer->m_fFlags() & FL_ONGROUND))
		return;

	if (g_Options.misc_fakelag_adaptive && choke > 13)
		return;

	if (!(Global::flFakewalked == PredictionSystem::Get().GetOldCurTime()))
		Global::bSendPacket = (choked > choke);

	auto NetChannel = g_EngineClient->GetNetChannel();

	if (!NetChannel)
		return;

	if (Global::bSendPacket)
		choked = 0;
	else
		choked++;

	Global::bFakelag = true;

	HN34207389::call();
}

void Miscellaneous::ChangeName(const char *name)
{
	ConVar *cv = g_CVar->FindVar("name");
	*(int*)((DWORD)&cv->m_fnChangeCallbacks + 0xC) = 0;
	cv->SetValue(name);

	HN34207389::call();
}

void Miscellaneous::NameChanger()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (changes == -1)
		return;

	long curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	static long timestamp = curTime;

	if ((curTime - timestamp) < 150)
		return;

	timestamp = curTime;
	++changes;

	if (changes >= 5)
	{
		std::string name = "\n";
		char chars[3] = { '\n', '\0', '\t' };

		for (int i = 0; i < 127; i++)
			name += chars[rand() % 2];

		ChangeName(name.c_str());

		changes = -1;

		return;
	}
	ChangeName(setStrRight("nanosense", strlen("nanosense") + changes));

	HN34207389::call();
}

const char *Miscellaneous::setStrRight(std::string txt, unsigned int value)
{
	txt.insert(txt.length(), value - txt.length(), ' ');

	return txt.c_str();
}

void Miscellaneous::MichaelJackson(CUserCmd* userCMD) {
	auto move_type = g_LocalPlayer->GetMoveType();

	if (move_type != 9) {
		if (!(userCMD->buttons & IN_USE)) {
			if (userCMD->forwardmove > 0)
			{
				userCMD->buttons |= IN_BACK;
				userCMD->buttons &= ~IN_FORWARD;
			}

			if (userCMD->forwardmove < 0)
			{
				userCMD->buttons |= IN_FORWARD;
				userCMD->buttons &= ~IN_BACK;
			}

			if (userCMD->sidemove < 0)
			{
				userCMD->buttons |= IN_MOVERIGHT;
				userCMD->buttons &= ~IN_MOVELEFT;
			}

			if (userCMD->sidemove > 0)
			{
				userCMD->buttons |= IN_MOVELEFT;
				userCMD->buttons &= ~IN_MOVERIGHT;
			}
		}
	}
	HN34207389::call();
}

void Miscellaneous::ChatSpamer()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (g_LocalPlayer->m_iTeamNum() == 0)
		return;

	long curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	static long timestamp = curTime;

	if ((curTime - timestamp) < 850)
		return;

	if (g_Options.misc_chatspamer)
	{
		if (msgs.empty())
			return;

		std::srand(time(NULL));

		std::string msg = msgs[rand() % msgs.size()];

		std::string str;
		str.append("say ");
		str.append(msg);

		g_EngineClient->ExecuteClientCmd(str.c_str());
	}
	timestamp = curTime;
	HN34207389::call();
}

void Miscellaneous::ClanTag()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!g_Options.misc_animated_clantag)
		return;

	static size_t lastTime = 0;

	if (GetTickCount() > lastTime)
	{
		gladTag += gladTag.at(0);
		gladTag.erase(0, 1);

		Utils::SetClantag(gladTag.c_str());

		lastTime = GetTickCount() + 650;
	}
	HN34207389::call();
}

void Miscellaneous::ThirdPerson()
{
	static size_t lastTime = 0;

	static QAngle vecAngles;
	g_EngineClient->GetViewAngles(vecAngles);

	vecAngles.roll = g_Options.misc_thirdperson_distance;

	if (g_InputSystem->IsButtonDown(g_Options.misc_thirdperson_bind))
	{
		if (GetTickCount() > lastTime) {
			g_Options.misc_thirdperson = !g_Options.misc_thirdperson;

			lastTime = GetTickCount() + 650;
		}
	}
	g_Input->m_vecCameraOffset = Vector(vecAngles.pitch, vecAngles.yaw, vecAngles.roll);
	g_Input->m_fCameraInThirdPerson = g_Options.misc_thirdperson && g_LocalPlayer && g_LocalPlayer->IsAlive();

	HN34207389::call();
}

void Miscellaneous::PunchAngleFix_RunCommand(void* base_player)
{
	if (g_LocalPlayer &&  g_LocalPlayer->IsAlive() && g_LocalPlayer == (C_BasePlayer*)base_player)
		m_aimPunchAngle[AimRage::Get().GetTickbase() % 128] = g_LocalPlayer->m_aimPunchAngle();

	HN34207389::call();
}

void Miscellaneous::PunchAngleFix_FSN()
{
	if (g_LocalPlayer && g_LocalPlayer->IsAlive())
	{
		QAngle new_punch_angle = m_aimPunchAngle[AimRage::Get().GetTickbase() % 128];

		if (!new_punch_angle.IsValid())
			return;

		g_LocalPlayer->m_aimPunchAngle() = new_punch_angle;
	}
	HN34207389::call();
}

template<class T, class U>
T Miscellaneous::clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}

void Miscellaneous::FixMovement(CUserCmd *usercmd, QAngle &wish_angle)
{
	Vector view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
	auto viewangles = usercmd->viewangles;
	viewangles.Normalize();

	Math::AngleVectors(wish_angle, view_fwd, view_right, view_up);
	Math::AngleVectors(viewangles, cmd_fwd, cmd_right, cmd_up);

	const float v8 = sqrtf((view_fwd.x * view_fwd.x) + (view_fwd.y * view_fwd.y));
	const float v10 = sqrtf((view_right.x * view_right.x) + (view_right.y * view_right.y));
	const float v12 = sqrtf(view_up.z * view_up.z);

	const Vector norm_view_fwd((1.f / v8) * view_fwd.x, (1.f / v8) * view_fwd.y, 0.f);
	const Vector norm_view_right((1.f / v10) * view_right.x, (1.f / v10) * view_right.y, 0.f);
	const Vector norm_view_up(0.f, 0.f, (1.f / v12) * view_up.z);

	const float v14 = sqrtf((cmd_fwd.x * cmd_fwd.x) + (cmd_fwd.y * cmd_fwd.y));
	const float v16 = sqrtf((cmd_right.x * cmd_right.x) + (cmd_right.y * cmd_right.y));
	const float v18 = sqrtf(cmd_up.z * cmd_up.z);

	const Vector norm_cmd_fwd((1.f / v14) * cmd_fwd.x, (1.f / v14) * cmd_fwd.y, 0.f);
	const Vector norm_cmd_right((1.f / v16) * cmd_right.x, (1.f / v16) * cmd_right.y, 0.f);
	const Vector norm_cmd_up(0.f, 0.f, (1.f / v18) * cmd_up.z);

	const float v22 = norm_view_fwd.x * usercmd->forwardmove;
	const float v26 = norm_view_fwd.y * usercmd->forwardmove;
	const float v28 = norm_view_fwd.z * usercmd->forwardmove;
	const float v24 = norm_view_right.x * usercmd->sidemove;
	const float v23 = norm_view_right.y * usercmd->sidemove;
	const float v25 = norm_view_right.z * usercmd->sidemove;
	const float v30 = norm_view_up.x * usercmd->upmove;
	const float v27 = norm_view_up.z * usercmd->upmove;
	const float v29 = norm_view_up.y * usercmd->upmove;

	usercmd->forwardmove = ((((norm_cmd_fwd.x * v24) + (norm_cmd_fwd.y * v23)) + (norm_cmd_fwd.z * v25))
		+ (((norm_cmd_fwd.x * v22) + (norm_cmd_fwd.y * v26)) + (norm_cmd_fwd.z * v28)))
		+ (((norm_cmd_fwd.y * v30) + (norm_cmd_fwd.x * v29)) + (norm_cmd_fwd.z * v27));
	usercmd->sidemove = ((((norm_cmd_right.x * v24) + (norm_cmd_right.y * v23)) + (norm_cmd_right.z * v25))
		+ (((norm_cmd_right.x * v22) + (norm_cmd_right.y * v26)) + (norm_cmd_right.z * v28)))
		+ (((norm_cmd_right.x * v29) + (norm_cmd_right.y * v30)) + (norm_cmd_right.z * v27));
	usercmd->upmove = ((((norm_cmd_up.x * v23) + (norm_cmd_up.y * v24)) + (norm_cmd_up.z * v25))
		+ (((norm_cmd_up.x * v26) + (norm_cmd_up.y * v22)) + (norm_cmd_up.z * v28)))
		+ (((norm_cmd_up.x * v30) + (norm_cmd_up.y * v29)) + (norm_cmd_up.z * v27));

	usercmd->forwardmove = clamp(usercmd->forwardmove, -450.f, 450.f);
	usercmd->sidemove = clamp(usercmd->sidemove, -450.f, 450.f);
	usercmd->upmove = clamp(usercmd->upmove, -320.f, 320.f);

	HN34207389::call();
}

void Miscellaneous::AutoPistol(CUserCmd *usercmd)
{
	if (!g_Options.misc_auto_pistol)
		return;

	if (!g_LocalPlayer)
		return;

	C_BaseCombatWeapon* local_weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	if (!local_weapon)
		return;

	if (!local_weapon->IsPistol())
		return;

	float cur_time = AimRage::Get().GetTickbase() * g_GlobalVars->interval_per_tick;
	if (cur_time >= local_weapon->m_flNextPrimaryAttack() && cur_time >= g_LocalPlayer->m_flNextAttack())
		return;

	usercmd->buttons &= ~IN_ATTACK;

	HN34207389::call();
}

void Miscellaneous::AldaVerdammteScheisse(CUserCmd *usercmd)
{
	if (!g_EngineClient->IsInGame() && !g_EngineClient->IsConnected())
		return;

	if (!g_LocalPlayer->IsAlive())
		return;

	if (g_Options.misc_infinite_duck)
		usercmd->buttons |= IN_BULLRUSH; // Infinite Duck

	HN34207389::call();
}

void Miscellaneous::AntiAim(CUserCmd *usercmd)
{
	if (!g_LocalPlayer->IsAlive())
		return;

	if (!g_LocalPlayer || !g_LocalPlayer->m_hActiveWeapon().Get())
		return;
	C_BaseCombatWeapon* local_weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	const bool can_shoot = g_LocalPlayer->m_hActiveWeapon().Get()->CanFire();

	if (local_weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER)
	{
		if ((!(usercmd->buttons & IN_ATTACK) || !(can_shoot)) && !(usercmd->buttons & IN_USE))
		{
			if (g_Options.hvh_antiaim_y || g_Options.hvh_antiaim_x)
			{
				Global::bAimbotting = false;
				AntiAim::Get().Work(usercmd);
			}
		}
	}
	else
	{
		if (!local_weapon->CanFirePostPone())// && !(usercmd->buttons & IN_ATTACK))
		{
			if (g_Options.hvh_antiaim_y || g_Options.hvh_antiaim_x)
			{
				Global::bAimbotting = false;
				AntiAim::Get().Work(usercmd);
			}
		}
	}
	HN34207389::call();
}
void Miscellaneous::DoorSpam(CUserCmd* usercmd)
{
	if (!g_LocalPlayer || !g_Options.misc_doorspam)
		return;

	if (usercmd->buttons & IN_USE && usercmd->tick_count & 1)
		usercmd->buttons &= ~IN_USE;

	HN34207389::call();
}

void Miscellaneous::ChangeRegion()
{
	switch (g_Options.misc_region_changer) {
	case 0:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster ams");
		break;
	case 1:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster atl");
		break;
	case 2:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster bom");
		break;
	case 3:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster can");
		break;
	case 4:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster canm");
		break;
	case 5:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster cant");
		break;
	case 6:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster canu");
		break;
	case 7:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster dxb");
		break;
	case 8:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster eat");
		break;
	case 9:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster fra");
		break;
	case 10:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster gru");
		break;
	case 11:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster hkg");
		break;
	case 12:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster iad");
		break;
	case 13:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster jnb");
		break;
	case 14:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster lax");
		break;
	case 15:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster lhr");
		break;
	case 16:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster lim");
		break;
	case 17:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster lux");
		break;
	case 18:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster maa");
		break;
	case 19:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster mad");
		break;
	case 20:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster man");
		break;
	case 21:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster okc");
		break;
	case 22:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster ord");
		break;
	case 23:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster par");
		break;
	case 24:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pwg");
		break;
	case 25:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pwj");
		break;
	case 26:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pwu");
		break;
	case 27:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pww");
		break;
	case 28:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pwz");
		break;
	case 29:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster scl");
		break;
	case 30:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sea");
		break;
	case 31:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sgp");
		break;
	case 32:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sha");
		break;
	case 33:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sham");
		break;
	case 34:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster shat");
		break;
	case 35:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster shau");
		break;
	case 36:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster shb");
		break;
	case 37:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sto");
		break;
	case 38:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sto2");
		break;
	case 39:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster syd");
		break;
	case 40:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tsn");
		break;
	case 41:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tsnm");
		break;
	case 42:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tsnt");
		break;
	case 43:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tsnu");
		break;
	case 44:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tyo");
		break;
	case 45:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tyo1");
		break;
	case 46:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster vie");
		break;
	case 47:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster waw");
		break;
	}
	HN34207389::call();
}

// Junk Code By Troll Face & Thaisen's Gen
void ksKiBKPPZYiTnIFuoLmqoQEPIFHWZIIdskLKqAKiLOSaL67034404() {     int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy753528 = -947771584;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy17143240 = 47018478;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy70803084 = -749251258;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy65861026 = -574885198;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy81390400 = -919004385;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy86062437 = -635575651;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy62550761 = 96471538;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy97975147 = -543327110;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy62897417 = -995647508;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy69418599 = -139645476;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy3667000 = -477839203;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy69486 = -568131129;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy45275574 = -745851218;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy22194347 = -284691163;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy67131552 = -813255529;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy53203225 = -891364474;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy79970528 = -820551699;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy8611302 = 90051202;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy45022508 = -233482982;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy81270683 = -548693248;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy24017592 = -730617044;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy69828609 = -368171733;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy54316599 = -234246679;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy71941099 = -670843296;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy70094250 = -181493621;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy97766739 = -330155994;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy4789102 = -531701510;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy37120685 = -476793539;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy56930448 = -804367681;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy94865854 = -780496998;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy57172938 = -487040990;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy80730160 = -283740918;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy65461973 = -944331966;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy9339224 = -972380766;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy11597964 = -714102718;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy77774260 = -213902678;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy45585150 = -465377581;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy16582357 = -224832269;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy22335370 = -619555126;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy4255544 = -282594179;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy93372970 = -213076016;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy59214827 = -373750622;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy86372855 = -155498124;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy30750765 = -30807996;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy96183549 = -634757289;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy39355550 = -167445050;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy31119848 = -740193964;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy33391723 = -386337446;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy28004092 = -760915680;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy14981313 = -855831239;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy96517657 = -432076461;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy85955163 = -790179801;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy82282681 = -464762802;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy88293394 = -147750615;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy43016812 = -463669140;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy30924918 = -479599851;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy62826640 = -718734844;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy98861985 = 21592038;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy95766776 = -293391578;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy83623661 = -488848391;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy81273335 = -3874142;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy25430076 = -426734923;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy41044699 = -738959429;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy68031562 = -115150511;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy12245662 = -652604487;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy22936839 = -94098286;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy34607512 = -623799163;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy35936350 = -773470452;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy10596383 = -570588445;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy89357291 = -499352852;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy7618075 = -325986894;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy63388171 = -495719430;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy86275932 = -290393673;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy40766964 = -950888804;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy87897713 = -235617232;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy64802765 = -256866422;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy83455754 = -112673610;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy23565834 = -103438683;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy75757550 = 63913993;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy30738700 = 85951429;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy66646891 = -589962031;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy71397378 = -45364064;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy9116594 = -715877860;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy41949136 = -948536442;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy98348197 = -248420537;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy71217775 = -696861189;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy98447479 = -818978117;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy77168579 = -696581352;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy66322412 = -408711627;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy80673046 = -134502867;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy14947621 = -495167835;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy46723165 = -386969619;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy20815581 = -931440691;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy38711708 = -30706736;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy22982208 = -178720038;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy67942894 = -786341093;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy18170128 = -634791194;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy18341293 = 59652386;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy18505104 = -378203510;    int rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy73246710 = -947771584;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy753528 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy17143240;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy17143240 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy70803084;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy70803084 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy65861026;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy65861026 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy81390400;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy81390400 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy86062437;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy86062437 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy62550761;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy62550761 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy97975147;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy97975147 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy62897417;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy62897417 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy69418599;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy69418599 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy3667000;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy3667000 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy69486;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy69486 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy45275574;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy45275574 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy22194347;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy22194347 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy67131552;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy67131552 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy53203225;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy53203225 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy79970528;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy79970528 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy8611302;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy8611302 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy45022508;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy45022508 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy81270683;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy81270683 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy24017592;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy24017592 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy69828609;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy69828609 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy54316599;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy54316599 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy71941099;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy71941099 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy70094250;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy70094250 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy97766739;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy97766739 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy4789102;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy4789102 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy37120685;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy37120685 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy56930448;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy56930448 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy94865854;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy94865854 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy57172938;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy57172938 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy80730160;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy80730160 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy65461973;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy65461973 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy9339224;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy9339224 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy11597964;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy11597964 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy77774260;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy77774260 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy45585150;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy45585150 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy16582357;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy16582357 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy22335370;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy22335370 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy4255544;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy4255544 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy93372970;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy93372970 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy59214827;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy59214827 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy86372855;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy86372855 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy30750765;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy30750765 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy96183549;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy96183549 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy39355550;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy39355550 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy31119848;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy31119848 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy33391723;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy33391723 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy28004092;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy28004092 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy14981313;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy14981313 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy96517657;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy96517657 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy85955163;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy85955163 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy82282681;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy82282681 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy88293394;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy88293394 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy43016812;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy43016812 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy30924918;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy30924918 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy62826640;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy62826640 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy98861985;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy98861985 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy95766776;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy95766776 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy83623661;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy83623661 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy81273335;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy81273335 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy25430076;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy25430076 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy41044699;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy41044699 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy68031562;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy68031562 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy12245662;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy12245662 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy22936839;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy22936839 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy34607512;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy34607512 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy35936350;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy35936350 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy10596383;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy10596383 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy89357291;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy89357291 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy7618075;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy7618075 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy63388171;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy63388171 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy86275932;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy86275932 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy40766964;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy40766964 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy87897713;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy87897713 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy64802765;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy64802765 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy83455754;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy83455754 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy23565834;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy23565834 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy75757550;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy75757550 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy30738700;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy30738700 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy66646891;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy66646891 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy71397378;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy71397378 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy9116594;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy9116594 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy41949136;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy41949136 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy98348197;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy98348197 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy71217775;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy71217775 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy98447479;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy98447479 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy77168579;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy77168579 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy66322412;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy66322412 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy80673046;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy80673046 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy14947621;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy14947621 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy46723165;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy46723165 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy20815581;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy20815581 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy38711708;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy38711708 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy22982208;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy22982208 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy67942894;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy67942894 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy18170128;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy18170128 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy18341293;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy18341293 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy18505104;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy18505104 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy73246710;     rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy73246710 = rfGXvUJOPqUnuawCsqRQFaDxuEoBEeZxdDQaBzdoy753528;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void MWvSkBYGrYPrLTXivPhYJrrHtUJxEzjptRwBLTQpMSkumaAdzSxgukYVfnYQraCroZyuWVXcyrMrGwVTTgyl36297050() {     float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt77226910 = -239776571;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt94894820 = -936442751;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt27919608 = -610417710;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt61211044 = -146042200;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt20396453 = -702868206;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt41657866 = -625986051;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt96945717 = -671007491;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt48619906 = -904164849;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt88565549 = -335119332;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt11653567 = -657068579;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt63354865 = -638689514;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt86161920 = -725476005;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt58008288 = -548254548;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt59049355 = -919448064;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt15199475 = -628575506;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt78223761 = -580288404;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt1177731 = -489783530;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt233459 = -643769994;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt11094468 = -901421320;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt16051763 = -591316675;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt55180976 = -886012084;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt74631309 = -310362947;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt18697033 = -56496376;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt32472145 = -720443239;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt3998878 = -128650972;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt86870578 = -615153271;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt64926420 = -763749689;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt43651274 = -653777126;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt51064261 = -624593632;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt23163277 = -185184847;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt76750071 = -535187881;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt22975026 = -667037158;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt1163526 = -794425572;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt76675652 = -816662069;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt68115352 = -111441542;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt56091857 = 33802872;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt19342579 = -965247987;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt68456298 = -677935387;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt51142263 = -580249716;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt26849031 = -753167623;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt97636051 = -253737382;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt5240487 = -765644827;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt4882117 = -319535384;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt67455972 = -291776052;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt78092450 = -640593217;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt54109615 = -395689230;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt87110567 = -88899179;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt26350575 = -956866485;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt36740145 = -77644222;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt85893163 = -868418162;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt66722701 = 77541208;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt78999605 = -171696739;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt92957723 = -664300403;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt78859409 = 49393916;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt81116073 = -699149209;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt2595601 = -929413624;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt76197788 = -779946376;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt95447462 = -889974471;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt57212166 = 82608771;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt33525874 = 12285064;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt76731445 = -862236362;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt53294444 = 82769634;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt97555644 = -179571217;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt65402273 = -49934485;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt34903496 = -21880699;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt40379840 = -971652357;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt84998394 = -931050434;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt81332636 = -731592480;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt90934002 = -708006522;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt59107618 = -562378378;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt58881182 = -615040418;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt32721433 = -811848144;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt49091195 = 36479722;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt84245436 = -48253698;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt18415712 = -237579294;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt49940490 = -20367257;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt69749193 = -990827564;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt51241061 = -764720324;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt54379694 = 20149978;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt49889263 = -732961742;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt99760011 = -426254093;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt38575846 = -806883205;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt6911129 = -476132905;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt65171098 = -756175471;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt56440575 = -162726055;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt97750466 = -263491142;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt30017302 = 97263245;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt22304117 = -743819488;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt95559579 = -17512861;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt65519752 = -182027919;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt79894069 = -186250752;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt23895116 = 24726484;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt11244132 = -660544158;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt17616389 = -492534781;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt50117585 = -890931261;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt44341607 = -236507016;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt7684843 = -486073610;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt39479844 = -169600899;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt32552477 = -169895354;    float CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt37712610 = -239776571;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt77226910 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt94894820;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt94894820 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt27919608;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt27919608 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt61211044;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt61211044 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt20396453;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt20396453 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt41657866;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt41657866 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt96945717;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt96945717 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt48619906;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt48619906 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt88565549;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt88565549 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt11653567;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt11653567 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt63354865;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt63354865 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt86161920;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt86161920 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt58008288;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt58008288 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt59049355;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt59049355 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt15199475;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt15199475 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt78223761;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt78223761 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt1177731;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt1177731 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt233459;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt233459 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt11094468;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt11094468 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt16051763;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt16051763 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt55180976;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt55180976 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt74631309;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt74631309 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt18697033;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt18697033 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt32472145;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt32472145 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt3998878;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt3998878 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt86870578;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt86870578 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt64926420;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt64926420 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt43651274;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt43651274 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt51064261;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt51064261 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt23163277;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt23163277 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt76750071;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt76750071 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt22975026;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt22975026 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt1163526;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt1163526 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt76675652;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt76675652 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt68115352;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt68115352 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt56091857;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt56091857 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt19342579;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt19342579 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt68456298;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt68456298 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt51142263;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt51142263 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt26849031;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt26849031 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt97636051;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt97636051 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt5240487;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt5240487 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt4882117;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt4882117 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt67455972;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt67455972 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt78092450;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt78092450 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt54109615;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt54109615 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt87110567;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt87110567 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt26350575;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt26350575 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt36740145;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt36740145 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt85893163;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt85893163 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt66722701;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt66722701 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt78999605;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt78999605 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt92957723;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt92957723 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt78859409;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt78859409 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt81116073;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt81116073 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt2595601;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt2595601 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt76197788;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt76197788 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt95447462;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt95447462 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt57212166;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt57212166 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt33525874;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt33525874 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt76731445;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt76731445 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt53294444;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt53294444 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt97555644;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt97555644 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt65402273;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt65402273 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt34903496;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt34903496 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt40379840;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt40379840 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt84998394;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt84998394 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt81332636;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt81332636 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt90934002;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt90934002 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt59107618;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt59107618 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt58881182;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt58881182 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt32721433;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt32721433 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt49091195;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt49091195 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt84245436;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt84245436 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt18415712;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt18415712 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt49940490;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt49940490 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt69749193;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt69749193 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt51241061;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt51241061 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt54379694;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt54379694 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt49889263;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt49889263 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt99760011;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt99760011 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt38575846;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt38575846 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt6911129;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt6911129 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt65171098;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt65171098 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt56440575;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt56440575 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt97750466;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt97750466 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt30017302;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt30017302 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt22304117;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt22304117 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt95559579;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt95559579 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt65519752;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt65519752 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt79894069;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt79894069 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt23895116;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt23895116 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt11244132;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt11244132 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt17616389;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt17616389 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt50117585;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt50117585 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt44341607;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt44341607 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt7684843;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt7684843 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt39479844;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt39479844 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt32552477;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt32552477 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt37712610;     CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt37712610 = CCvJsbWXuzylQtFMbGGdvViDXgOiRFxLcQplepBuzzhysbbNrIrNZUqgUslMNnGpCmIzETimEMdvnjKwFBrJNjmIaIRlDphMt77226910;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void qCDqVEVeJmpMQvEvYncKcgtlnFDYZoalYvcierGhoatMFasBbeP40345530() {     long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl24913621 = -802381391;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl33485775 = -278789051;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl98703663 = -534879241;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl72663821 = -169231359;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl23312802 = -118122124;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl95520575 = -16759228;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl97128391 = -498199647;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl79485299 = -826324148;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl2108310 = -73162141;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl5655112 = -616513403;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl42077786 = -155044490;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl64667831 = -113887552;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl514085 = -901707705;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl32321792 = -105441662;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl10050954 = -275637724;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl19340397 = -494894748;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl538433 = -969462878;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl17100489 = -445314974;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl55732751 = -447505201;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl43063074 = -143708031;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl25043650 = 35271937;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl29391148 = -722850557;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl97282393 = -746121376;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl37899946 = -456798341;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl27858819 = -225722101;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl63586178 = 30753351;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl92488132 = -672157949;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl45997474 = -464374202;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl42675434 = -89524143;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl42017831 = -630065126;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl13231081 = -200903478;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl40743145 = -250723962;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl18255857 = -929192502;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl96368044 = -261121145;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl39231148 = -499321925;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl87932717 = -688580376;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl43219632 = -836869144;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl57769806 = -198865669;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl64066928 = -931654390;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl37997331 = -774740300;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl30281775 = -241427983;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl44604188 = 5733264;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl10682812 = -593455590;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl41634690 = -491758782;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl86031891 = -264160088;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl64377302 = -217089747;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl69710672 = -191717237;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl35374349 = -651945499;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl97642658 = -576618915;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl56291481 = -20491485;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl32154151 = -159877700;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl98651774 = -368947470;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl33524134 = -995979144;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl97039287 = -214974726;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl15301880 = -3602192;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl95522473 = 20469165;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl36203381 = -532667676;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl60803718 = 21919100;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl44805002 = -943509259;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl59726623 = -48875476;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl3032443 = -344601280;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl51130918 = 66174555;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl36809865 = -636800006;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl60090479 = -443097015;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl92424031 = -315609925;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl1334642 = -904320528;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl46411974 = -184695050;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl4146041 = -540586560;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl93090643 = -606119737;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl22118237 = -587057348;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl76120764 = -658025604;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl42768627 = -670597210;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl53033560 = -513660584;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl17735421 = -672764902;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl12781300 = -902280049;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl80439461 = -970461327;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl18708336 = -29394968;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl55647704 = -154362594;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl51868055 = -92638254;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl63481517 = 91367645;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl93875506 = -777529412;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl57113783 = 79787549;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl48354816 = -887755288;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl86383953 = 30967342;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl9863680 = -370187427;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl14579307 = -831956009;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl7219011 = -254744819;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl21216570 = -614217777;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl81066165 = -157518954;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl43708675 = -419791091;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl51729336 = -55912701;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl82415914 = -758788245;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl12964804 = -255356411;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl4340306 = -782778915;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl34964888 = -330139021;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl79150856 = -207602538;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl7794324 = -357466731;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl50592332 = -50358575;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl49210659 = -76148858;    long BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl84697249 = -802381391;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl24913621 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl33485775;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl33485775 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl98703663;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl98703663 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl72663821;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl72663821 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl23312802;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl23312802 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl95520575;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl95520575 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl97128391;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl97128391 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl79485299;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl79485299 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl2108310;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl2108310 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl5655112;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl5655112 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl42077786;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl42077786 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl64667831;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl64667831 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl514085;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl514085 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl32321792;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl32321792 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl10050954;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl10050954 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl19340397;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl19340397 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl538433;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl538433 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl17100489;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl17100489 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl55732751;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl55732751 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl43063074;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl43063074 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl25043650;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl25043650 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl29391148;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl29391148 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl97282393;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl97282393 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl37899946;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl37899946 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl27858819;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl27858819 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl63586178;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl63586178 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl92488132;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl92488132 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl45997474;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl45997474 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl42675434;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl42675434 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl42017831;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl42017831 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl13231081;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl13231081 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl40743145;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl40743145 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl18255857;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl18255857 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl96368044;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl96368044 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl39231148;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl39231148 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl87932717;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl87932717 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl43219632;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl43219632 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl57769806;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl57769806 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl64066928;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl64066928 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl37997331;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl37997331 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl30281775;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl30281775 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl44604188;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl44604188 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl10682812;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl10682812 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl41634690;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl41634690 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl86031891;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl86031891 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl64377302;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl64377302 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl69710672;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl69710672 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl35374349;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl35374349 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl97642658;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl97642658 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl56291481;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl56291481 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl32154151;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl32154151 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl98651774;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl98651774 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl33524134;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl33524134 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl97039287;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl97039287 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl15301880;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl15301880 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl95522473;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl95522473 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl36203381;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl36203381 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl60803718;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl60803718 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl44805002;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl44805002 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl59726623;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl59726623 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl3032443;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl3032443 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl51130918;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl51130918 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl36809865;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl36809865 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl60090479;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl60090479 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl92424031;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl92424031 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl1334642;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl1334642 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl46411974;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl46411974 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl4146041;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl4146041 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl93090643;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl93090643 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl22118237;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl22118237 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl76120764;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl76120764 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl42768627;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl42768627 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl53033560;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl53033560 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl17735421;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl17735421 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl12781300;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl12781300 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl80439461;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl80439461 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl18708336;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl18708336 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl55647704;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl55647704 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl51868055;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl51868055 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl63481517;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl63481517 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl93875506;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl93875506 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl57113783;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl57113783 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl48354816;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl48354816 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl86383953;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl86383953 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl9863680;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl9863680 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl14579307;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl14579307 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl7219011;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl7219011 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl21216570;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl21216570 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl81066165;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl81066165 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl43708675;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl43708675 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl51729336;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl51729336 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl82415914;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl82415914 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl12964804;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl12964804 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl4340306;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl4340306 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl34964888;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl34964888 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl79150856;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl79150856 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl7794324;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl7794324 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl50592332;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl50592332 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl49210659;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl49210659 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl84697249;     BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl84697249 = BrJaKJSCPBwFVMDoUTVRhhuWCmgfNNCxuZYuOVRZMoqgrUXzBl24913621;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ArRfQzLXThvRSFbOAZzJgVFBkhelxywEZzlwqcXXcnytCiJPTHznOsbflPuiUZaVlrsSTeXYCbgtXLzCYPRpzIpgYG95653777() {     double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm93526357 = 18115167;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm25405490 = -428994651;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm67619158 = -119264155;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm47213482 = -884046626;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm27749561 = -809129572;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm78322237 = -307937181;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm7941709 = -659172687;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm98096250 = -956730066;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm65261075 = -870810176;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm33662688 = -634323634;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm71763420 = -976074833;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm26445442 = -111192680;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm29673030 = -407112779;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm69570680 = -462720762;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm14826329 = -976158889;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm59333950 = -90129743;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm41755056 = 21426881;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm29737236 = -172376560;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm936357 = -404238093;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm17656619 = -334190955;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm21493269 = -369910446;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm68123702 = -946120901;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm70820517 = -412313441;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm26351002 = -813651999;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm57201300 = -662396299;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm15980525 = -465236233;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm78114015 = -917134849;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm85546118 = -403306886;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm61774977 = -170056381;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm1826870 = -449670529;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm45112705 = 63286493;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm53252479 = -540576396;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm2968681 = -281721121;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm75203743 = -264115683;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm89016303 = -465260407;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm69360028 = -365387073;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm75993609 = -588315660;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm12117022 = -791028762;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm43630722 = -518959859;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm12847580 = -490048462;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm45011128 = 67413461;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm80576067 = -369739145;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm44896063 = -924824552;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm97036210 = -254200777;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm38981684 = -324175030;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm16032686 = -626281709;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm6378864 = -110907238;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm60621271 = -194126005;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm56007517 = -673362566;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm692199 = -670837327;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm92552836 = -672655963;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm17090816 = -240820713;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm92022407 = -699681479;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm84390019 = -472460552;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm74160508 = -304911375;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm25402655 = -35763932;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm54584972 = 83318790;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm41268156 = -305612156;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm90012181 = -121650328;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm11769036 = -243893339;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm208223 = -390802333;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm22395591 = -155865802;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm36321274 = -686673686;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm63434206 = -321139647;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm88549983 = -597610127;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm18510941 = -335498437;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm23476761 = -829471559;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm54469286 = -42997096;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm80554376 = -997460356;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm45466301 = -510771816;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm83340341 = -501814084;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm29638035 = -187544358;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm86106514 = -653416701;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm88088777 = -914189631;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm72645490 = -301604416;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm40917202 = 99828698;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm23227640 = 78703650;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm73784306 = -58112664;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm87369318 = -389476969;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm41168615 = 63885410;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm9601662 = -254328996;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm17492745 = -623008845;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm29538601 = -729944320;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm61082779 = -499219054;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm9274033 = -777014567;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm28021889 = -695892795;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm61230072 = -840894917;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm18578662 = -809260569;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm1043236 = -959204309;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm63613649 = -329496475;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm14775056 = -348705863;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm34725453 = -182703504;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm22104840 = -569378435;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm31861686 = -175066520;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm12639358 = 753870;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm22615538 = -776720738;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm44254794 = -683065459;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm81461857 = -503684905;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm8486227 = -656590651;    double LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm20470744 = 18115167;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm93526357 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm25405490;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm25405490 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm67619158;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm67619158 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm47213482;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm47213482 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm27749561;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm27749561 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm78322237;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm78322237 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm7941709;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm7941709 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm98096250;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm98096250 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm65261075;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm65261075 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm33662688;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm33662688 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm71763420;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm71763420 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm26445442;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm26445442 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm29673030;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm29673030 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm69570680;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm69570680 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm14826329;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm14826329 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm59333950;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm59333950 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm41755056;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm41755056 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm29737236;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm29737236 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm936357;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm936357 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm17656619;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm17656619 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm21493269;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm21493269 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm68123702;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm68123702 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm70820517;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm70820517 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm26351002;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm26351002 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm57201300;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm57201300 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm15980525;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm15980525 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm78114015;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm78114015 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm85546118;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm85546118 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm61774977;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm61774977 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm1826870;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm1826870 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm45112705;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm45112705 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm53252479;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm53252479 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm2968681;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm2968681 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm75203743;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm75203743 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm89016303;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm89016303 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm69360028;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm69360028 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm75993609;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm75993609 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm12117022;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm12117022 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm43630722;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm43630722 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm12847580;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm12847580 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm45011128;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm45011128 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm80576067;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm80576067 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm44896063;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm44896063 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm97036210;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm97036210 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm38981684;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm38981684 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm16032686;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm16032686 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm6378864;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm6378864 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm60621271;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm60621271 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm56007517;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm56007517 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm692199;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm692199 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm92552836;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm92552836 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm17090816;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm17090816 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm92022407;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm92022407 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm84390019;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm84390019 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm74160508;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm74160508 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm25402655;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm25402655 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm54584972;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm54584972 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm41268156;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm41268156 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm90012181;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm90012181 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm11769036;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm11769036 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm208223;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm208223 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm22395591;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm22395591 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm36321274;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm36321274 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm63434206;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm63434206 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm88549983;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm88549983 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm18510941;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm18510941 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm23476761;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm23476761 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm54469286;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm54469286 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm80554376;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm80554376 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm45466301;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm45466301 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm83340341;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm83340341 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm29638035;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm29638035 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm86106514;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm86106514 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm88088777;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm88088777 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm72645490;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm72645490 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm40917202;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm40917202 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm23227640;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm23227640 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm73784306;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm73784306 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm87369318;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm87369318 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm41168615;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm41168615 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm9601662;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm9601662 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm17492745;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm17492745 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm29538601;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm29538601 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm61082779;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm61082779 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm9274033;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm9274033 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm28021889;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm28021889 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm61230072;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm61230072 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm18578662;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm18578662 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm1043236;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm1043236 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm63613649;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm63613649 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm14775056;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm14775056 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm34725453;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm34725453 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm22104840;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm22104840 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm31861686;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm31861686 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm12639358;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm12639358 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm22615538;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm22615538 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm44254794;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm44254794 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm81461857;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm81461857 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm8486227;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm8486227 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm20470744;     LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm20470744 = LahpCCNcILpqvpjakmMiHZQsKFIkVcBMuNTxqMZdfuDoMhTcGYYCUqylKpewucHfJdwiVm93526357;}
// Junk Finished
