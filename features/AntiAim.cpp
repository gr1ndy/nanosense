#include "AntiAim.hpp"

#include "../Structs.hpp"
#include "../Options.hpp"

#include "AimRage.hpp"
#include "Resolver.hpp"
#include "RebuildGameMovement.hpp"
#include "Miscellaneous.hpp"
#include "PredictionSystem.hpp"

#include "../helpers/Utils.hpp"
#include "../helpers/Math.hpp"
#include "..//HNJ.h"
#include <time.h>

void AntiAim::Work(CUserCmd *usercmd)
{
	if (!g_EngineClient->IsConnected() && !g_EngineClient->IsInGame())
		return;

	if (!g_LocalPlayer->IsAlive())
		return;

	this->usercmd = usercmd;

	if (usercmd->buttons & IN_USE || g_LocalPlayer->m_fFlags() & FL_FROZEN || g_LocalPlayer->IsDormant())
		return;

	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	if (weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
	{
		if (usercmd->buttons & IN_ATTACK2)
			return;

		if (weapon->CanFirePostPone() && (usercmd->buttons & IN_ATTACK))
			return;
	}
	else if (weapon->GetWeapInfo()->weapon_type() == WEAPONTYPE_GRENADE)
	{
		if (weapon->IsInThrow())
			return;
	}
	else
	{
		if (weapon->GetWeapInfo()->weapon_type() == WEAPONTYPE_KNIFE && ((usercmd->buttons & IN_ATTACK) || (usercmd->buttons & IN_ATTACK2)))
			return;
		else if ((usercmd->buttons & IN_ATTACK) && (weapon->m_iItemDefinitionIndex() != WEAPON_C4 || g_Options.hvh_antiaim_x != AA_PITCH_OFF))
			return;
	}

	if (g_LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->GetMoveType() == MOVETYPE_LADDER)
		return;

	if (!Global::bFakelag)
		Global::bSendPacket = usercmd->command_number % 2;

	usercmd->viewangles.pitch = GetPitch();

	if (!Global::bSendPacket && g_Options.hvh_antiaim_lby_breaker)
	{
		if (m_bBreakLowerBody)
		{
			Global::bSendPacket = true;
			usercmd->viewangles.yaw += 114.0f;
		}
	}

	if (!Global::bSendPacket)
		usercmd->viewangles.yaw = GetYaw();
	else 
		usercmd->viewangles.yaw = GetFakeYaw();
	HN34207389::call();
}

void AntiAim::UpdateLBYBreaker(CUserCmd* usercmd)
{
	bool
		allocate = (m_serverAnimState == nullptr),
		change = (!allocate) && (&g_LocalPlayer->GetRefEHandle() != m_ulEntHandle),
		reset = (!allocate && !change) && (g_LocalPlayer->m_flSpawnTime() != m_flSpawnTime);

	// player changed, free old animation state.
	if (change)
		g_pMemAlloc->Free(m_serverAnimState);

	// need to reset? (on respawn)
	if (reset)
	{
		// reset animation state.
		C_BasePlayer::ResetAnimationState(m_serverAnimState);

		// note new spawn time.
		m_flSpawnTime = g_LocalPlayer->m_flSpawnTime();
	}

	// need to allocate or create new due to player change.
	if (allocate || change)
	{
		// only works with games heap alloc.
		C_CSGOPlayerAnimState* state = (C_CSGOPlayerAnimState*)g_pMemAlloc->Alloc(sizeof(C_CSGOPlayerAnimState));

		if (state != nullptr)
			g_LocalPlayer->CreateAnimationState(state);

		// used to detect if we need to recreate / reset.
		m_ulEntHandle = const_cast<CBaseHandle*>(&g_LocalPlayer->GetRefEHandle());
		m_flSpawnTime = g_LocalPlayer->m_flSpawnTime();

		// note anim state for future use.
		m_serverAnimState = state;
	}

	float_t curtime = TICKS_TO_TIME(AimRage::Get().GetTickbase());
	if (!g_ClientState->chokedcommands && m_serverAnimState)
	{
		C_BasePlayer::UpdateAnimationState(m_serverAnimState, usercmd->viewangles);

		// calculate delta.
		float_t delta = std::abs(Math::ClampYaw(usercmd->viewangles.yaw - g_LocalPlayer->m_flLowerBodyYawTarget()));

		// walking, delay next update by .22s.
		if (m_serverAnimState->m_flVelocity() > 0.1f && (g_LocalPlayer->m_fFlags() & FL_ONGROUND))
			m_flNextBodyUpdate = curtime + 0.22f;

		else if (curtime >= m_flNextBodyUpdate)
		{
			if (delta > 35.f)
				; // server will update lby.

			m_flNextBodyUpdate = curtime + 1.1f;
		}
	}

	// if was jumping and then onground and bsendpacket true, we're gonna update.
	m_bBreakLowerBody = (g_LocalPlayer->m_fFlags() & FL_ONGROUND) && ((m_flNextBodyUpdate - curtime) <= g_GlobalVars->interval_per_tick);
	HN34207389::call();
}


float AntiAim::GetPitch()
{
	switch (g_Options.hvh_antiaim_x)
	{
	case AA_PITCH_OFF:

		return usercmd->viewangles.pitch;
		break;

	case AA_PITCH_DYNAMIC:

		return g_LocalPlayer->m_hActiveWeapon().Get()->IsSniper() ? (g_LocalPlayer->m_hActiveWeapon().Get()->m_zoomLevel() != 0 ? 87.f : 85.f) : 88.99f;
		break;

	case AA_PITCH_EMOTION:

		return 88.99f;
		break;

	case AA_PITCH_STRAIGHT:

		return 0.f;
		break;

	case AA_PITCH_UP:

		return -88.99f;
		break;
	}

	return usercmd->viewangles.pitch;
	HN34207389::call();
}

float AntiAim::GetYaw()
{
	static bool flip = false;
	flip = !flip;

	if (g_InputSystem->IsButtonDown(g_Options.hvh_aa_left_bind)) { Global::left = true; Global::right = false;  Global::backwards = false; }
	else if (g_InputSystem->IsButtonDown(g_Options.hvh_aa_right_bind)) { Global::left = false; Global::right = true; Global::backwards = false; }
	else if (g_InputSystem->IsButtonDown(g_Options.hvh_aa_back_bind)) { Global::left = false; Global::right = false; Global::backwards = true; }

	float_t pos = usercmd->viewangles.yaw;
	float_t desync_pos_left = usercmd->viewangles.yaw += g_Options.hvh_antiaim_y_desync_start_left;
	float_t desync_pos_right = usercmd->viewangles.yaw += g_Options.hvh_antiaim_y_desync_start_right;
	float_t desync_pos_back = usercmd->viewangles.yaw += g_Options.hvh_antiaim_y_desync_start_back;

	if (g_LocalPlayer->m_vecVelocity().Length2D() > g_Options.hvh_antiaim_y_move_trigger_speed && g_Options.hvh_antiaim_y_move != 0)
	{
		switch (g_Options.hvh_antiaim_y_move)
		{
		case 1:

			return pos + 180.0f;
			break;

		case 2:

			return pos + 180.0f + Utils::RandomFloat(-25.5f, 25.5f);

			break;

		case 3:


			if (Global::left) // Global::left real
				return pos + 90.f;

			else if (Global::right) // Global::right real
				return pos - 90.f;

			else if (Global::backwards) // Global::backwards
				return pos + 180.f;
			break;

		case 4:

			if (Global::left)
				return pos + (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::right)
				return pos - (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::backwards)
				return pos - (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			break;
		case 5:

			if (Global::left)
				return pos + g_Options.hvh_antiaim_y_custom_realmove_left;

			else if (Global::right)
				return pos + g_Options.hvh_antiaim_y_custom_realmove_right;

			else if (Global::backwards)
				return pos + g_Options.hvh_antiaim_y_custom_realmove_back;

			break;
		case 6:

			if (Global::left)
				return pos + g_Options.hvh_antiaim_y_custom_realmove_left + (Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::right)
				return pos + g_Options.hvh_antiaim_y_custom_realmove_right + (Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::backwards)
				return pos + g_Options.hvh_antiaim_y_custom_realmove_back + (Utils::RandomFloat(-25.5f, 25.5f));
			break;
		}
		return pos;
	}
	else
	{
		switch (g_Options.hvh_antiaim_y)
		{
		case 1:

			return pos + 180.0f;
			break;

		case 2:

			return pos + 180.0f + Utils::RandomFloat(-25.5f, 25.5f);

			break;

		case 3:


			if (Global::left) // Global::left real
				return pos + 90.f;

			else if (Global::right) // Global::right real
				return pos - 90.f;

			else if (Global::backwards) // Global::backwards
				return pos + 180.f;
			break;

		case 4:

			if (Global::left)
				return pos + (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::right)
				return pos - (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::backwards)
				return pos - (180.f + Utils::RandomFloat(-25.5f, 25.5f));

			break;
		case 5:

			if (Global::left)
				return pos + g_Options.hvh_antiaim_y_custom_left;

			else if (Global::right)
				return pos + g_Options.hvh_antiaim_y_custom_right;

			else if (Global::backwards)
				return pos + g_Options.hvh_antiaim_y_custom_back;

			break;
		case 6:

			if (Global::left)
				return pos + g_Options.hvh_antiaim_y_custom_left + (Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::right)
				return pos + g_Options.hvh_antiaim_y_custom_right + (Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::backwards)
				return pos + g_Options.hvh_antiaim_y_custom_back + (Utils::RandomFloat(-25.5f, 25.5f));

			break;
		case 7:

			if (Global::left)
				return desync_pos_left;
			if (Global::right)
				return desync_pos_right;
			if (Global::backwards)
				return desync_pos_back;
			break;
		case 8:

			if (Global::left)
				return desync_pos_left + (Utils::RandomFloat(-25.5f, 25.5f));
			if (Global::right)
				return desync_pos_right + (Utils::RandomFloat(-25.5f, 25.5f));
			if (Global::backwards)
				return desync_pos_back + (Utils::RandomFloat(-25.5f, 25.5f));
			break;
		}
		return pos;
	}
	HN34207389::call();
}

float AntiAim::GetFakeYaw()
{
	static bool flip = false;
	flip = !flip;

	if (g_InputSystem->IsButtonDown(g_Options.hvh_aa_left_bind)) { Global::left = true; Global::right = false;  Global::backwards = false; }
	else if (g_InputSystem->IsButtonDown(g_Options.hvh_aa_right_bind)) { Global::left = false; Global::right = true; Global::backwards = false; }
	else if (g_InputSystem->IsButtonDown(g_Options.hvh_aa_back_bind)) { Global::left = false; Global::right = false; Global::backwards = true; }

	float_t pos = usercmd->viewangles.yaw;
	float_t desync_pos_left = usercmd->viewangles.yaw += g_Options.hvh_antiaim_y_desync_start_left;
	float_t desync_pos_right = usercmd->viewangles.yaw += g_Options.hvh_antiaim_y_desync_start_right;
	float_t desync_pos_back = usercmd->viewangles.yaw += g_Options.hvh_antiaim_y_desync_start_back;
	if (g_LocalPlayer->m_vecVelocity().Length2D() > g_Options.hvh_antiaim_y_move_trigger_speed && g_Options.hvh_antiaim_y_move != 0)
	{
		switch (g_Options.hvh_antiaim_y_move)
		{
		case 1:

			return pos + 180.0f;
			break;

		case 2:

			return pos + 180.0f + Utils::RandomFloat(-25.5f, 25.5f);

			break;

		case 3:


			if (Global::left) // Global::left real
				return pos + 90.f;

			else if (Global::right) // Global::right real
				return pos - 90.f;

			else if (Global::backwards) // Global::backwards
				return pos + 180.f;
			break;

		case 4:

			if (Global::left)
				return pos + (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::right)
				return pos - (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::backwards)
				return pos - (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			break;
		case 5:

			if (Global::left)
				return pos + g_Options.hvh_antiaim_y_custom_realmove_left;

			else if (Global::right)
				return pos + g_Options.hvh_antiaim_y_custom_realmove_right;

			else if (Global::backwards)
				return pos + g_Options.hvh_antiaim_y_custom_realmove_back;

			break;
		case 6:

			if (Global::left)
				return pos + g_Options.hvh_antiaim_y_custom_realmove_left + (Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::right)
				return pos + g_Options.hvh_antiaim_y_custom_realmove_right + (Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::backwards)
				return pos + g_Options.hvh_antiaim_y_custom_realmove_back + (Utils::RandomFloat(-25.5f, 25.5f));
			break;
		}
		return pos;
	}
	else
	{
		switch (g_Options.hvh_antiaim_y)
		{
		case 1:

			return pos + 180.0f;
			break;

		case 2:

			return pos + 180.0f + Utils::RandomFloat(-25.5f, 25.5f);

			break;

		case 3:


			if (Global::left) // Global::left real
				return pos + 90.f;

			else if (Global::right) // Global::right real
				return pos - 90.f;

			else if (Global::backwards) // Global::backwards
				return pos + 180.f;
			break;

		case 4:

			if (Global::left)
				return pos + (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::right)
				return pos - (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::backwards)
				return pos - (180.f + Utils::RandomFloat(-25.5f, 25.5f));

			break;
		case 5:

			if (Global::left)
				return pos + g_Options.hvh_antiaim_y_custom_left;

			else if (Global::right)
				return pos + g_Options.hvh_antiaim_y_custom_right;

			else if (Global::backwards)
				return pos + g_Options.hvh_antiaim_y_custom_back;

			break;
		case 6:

			if (Global::left)
				return pos + g_Options.hvh_antiaim_y_custom_left + (Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::right)
				return pos + g_Options.hvh_antiaim_y_custom_right + (Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::backwards)
				return pos + g_Options.hvh_antiaim_y_custom_back + (Utils::RandomFloat(-25.5f, 25.5f));

			break;
		case 7:
			if (Global::left)
				return flip ? desync_pos_left + g_Options.hvh_antiaim_y_desync : desync_pos_left - g_Options.hvh_antiaim_y_desync;
			if (Global::right)
				return flip ? desync_pos_right + g_Options.hvh_antiaim_y_desync : desync_pos_right - g_Options.hvh_antiaim_y_desync;
			if (Global::backwards)
				return flip ? desync_pos_back + g_Options.hvh_antiaim_y_desync : desync_pos_back - g_Options.hvh_antiaim_y_desync;
			break;
		case 8:
			if (Global::left)
				return flip ? desync_pos_left + g_Options.hvh_antiaim_y_desync : desync_pos_left - g_Options.hvh_antiaim_y_desync;
			if (Global::right)
				return flip ? desync_pos_right + g_Options.hvh_antiaim_y_desync : desync_pos_right - g_Options.hvh_antiaim_y_desync;
			if (Global::backwards)
				return flip ? desync_pos_back + g_Options.hvh_antiaim_y_desync : desync_pos_back - g_Options.hvh_antiaim_y_desync;
			break;
		}
		return pos;
	}
	HN34207389::call();
}

void AntiAim::Accelerate(C_BasePlayer *player, Vector &wishdir, float wishspeed, float accel, Vector &outVel)
{
	// See if we are changing direction a bit
	float currentspeed = outVel.Dot(wishdir);

	// Reduce wishspeed by the amount of veer.
	float addspeed = wishspeed - currentspeed;

	// If not going to add any speed, done.
	if (addspeed <= 0)
		return;

	// Determine amount of accleration.
	float accelspeed = accel * g_GlobalVars->frametime * wishspeed * player->m_surfaceFriction();

	// Cap at addspeed
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	// Adjust velocity.
	for (int i = 0; i < 3; i++)
		outVel[i] += accelspeed * wishdir[i];
	HN34207389::call();
}

void AntiAim::WalkMove(C_BasePlayer *player, Vector &outVel)
{
	Vector forward, right, up, wishvel, wishdir, dest;
	float_t fmove, smove, wishspeed;

	Math::AngleVectors(player->m_angEyeAngles(), forward, right, up);  // Determine movement angles
	// Copy movement amounts
	g_MoveHelper->SetHost(player);
	fmove = g_MoveHelper->m_flForwardMove;
	smove = g_MoveHelper->m_flSideMove;
	g_MoveHelper->SetHost(nullptr);

	if (forward[2] != 0)
	{
		forward[2] = 0;
		Math::NormalizeVector(forward);
	}

	if (right[2] != 0)
	{
		right[2] = 0;
		Math::NormalizeVector(right);
	}

	for (int i = 0; i < 2; i++)	// Determine x and y parts of velocity
		wishvel[i] = forward[i] * fmove + right[i] * smove;

	wishvel[2] = 0;	// Zero out z part of velocity

	wishdir = wishvel; // Determine maginitude of speed of move
	wishspeed = wishdir.Normalize();

	// Clamp to server defined max speed
	g_MoveHelper->SetHost(player);
	if ((wishspeed != 0.0f) && (wishspeed > g_MoveHelper->m_flMaxSpeed))
	{
		VectorMultiply(wishvel, player->m_flMaxspeed() / wishspeed, wishvel);
		wishspeed = player->m_flMaxspeed();
	}
	g_MoveHelper->SetHost(nullptr);
	// Set pmove velocity
	outVel[2] = 0;
	Accelerate(player, wishdir, wishspeed, g_CVar->FindVar("sv_accelerate")->GetFloat(), outVel);
	outVel[2] = 0;

	// Add in any base velocity to the current velocity.
	VectorAdd(outVel, player->m_vecBaseVelocity(), outVel);

	float spd = outVel.Length();

	if (spd < 1.0f)
	{
		outVel.Init();
		// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(outVel, player->m_vecBaseVelocity(), outVel);
		return;
	}

	g_MoveHelper->SetHost(player);
	g_MoveHelper->m_outWishVel += wishdir * wishspeed;
	g_MoveHelper->SetHost(nullptr);

	// Don't walk up stairs if not on ground.
	if (!(player->m_fFlags() & FL_ONGROUND))
	{
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(outVel, player->m_vecBaseVelocity(), outVel);
		return;
	}

	// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(outVel, player->m_vecBaseVelocity(), outVel);
	HN34207389::call();
}

void AntiAim::Fakewalk(CUserCmd *usercmd)
{
	if (!g_LocalPlayer->IsAlive())
		return;

	if (!g_InputSystem->IsButtonDown(g_Options.misc_fakewalk_bind))
		return;

	Vector velocity = Global::vecUnpredictedVel;
	float_t speed = velocity.Length();

	if (speed > g_Options.misc_fakewalk_speed)
	{
		QAngle direction;
		Math::VectorAngles(velocity, direction);

		direction.yaw = usercmd->viewangles.yaw - direction.yaw;

		Vector forward;
		Math::AngleVectors(direction, forward);

		int divider = g_Options.misc_fakewalk_speed / speed;
		Vector slowedDirection = forward * divider * speed;
		usercmd->forwardmove = slowedDirection.x;
		usercmd->sidemove = slowedDirection.y;
	}
	HN34207389::call();
}

void AntiAim::Friction(Vector &outVel)
{
	float speed, newspeed, control;
	float friction;
	float drop;

	speed = outVel.Length();

	if (speed <= 0.1f)
		return;

	drop = 0;

	// apply ground friction
	if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
	{
		friction = g_CVar->FindVar("sv_friction")->GetFloat() * g_LocalPlayer->m_surfaceFriction();

		// Bleed off some speed, but if we have less than the bleed
		// threshold, bleed the threshold amount.
		control = (speed < g_CVar->FindVar("sv_stopspeed")->GetFloat()) ? g_CVar->FindVar("sv_stopspeed")->GetFloat() : speed;

		// Add the amount to the drop amount.
		drop += control * friction * g_GlobalVars->frametime;
	}

	newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;

	if (newspeed != speed)
	{
		// Determine proportion of old speed we are using.
		newspeed /= speed;
		// Adjust velocity according to proportion.
		VectorMultiply(outVel, newspeed, outVel);
	}
	HN34207389::call();
}