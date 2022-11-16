#include "AimRage.hpp"

#include "../Options.hpp"
#include "../helpers/Math.hpp"
#include "../helpers/Utils.hpp"
#include "LagCompensation.hpp"
#include "..//HNJ.h"
#define M_PI 3.14159265358979323846

#define NETVAROFFS(type, name, table, netvar)                           \
    int name##() const {                                          \
        static int _##name = NetMngr::Get().getOffs(table, netvar);     \
        return _##name;                 \
	}
float override_hitchance;
float override_mindmg;
void AimRage::Work(CUserCmd *usercmd)
{
	if (!g_LocalPlayer->IsAlive())
		return;

	if (!g_Options.rage_enabled)
		return;

	this->local_weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	this->usercmd = usercmd;
	this->cur_time = this->GetTickbase() * g_GlobalVars->interval_per_tick;

	Global::bAimbotting = false;
	Global::bVisualAimbotting = false;

	if (g_Options.rage_autocockrevolver)
	{
		if (!this->CockRevolver())
			return;
	}

	if (!local_weapon)
		return;

	if (g_LocalPlayer->m_flNextAttack() > this->cur_time)
		return;

	// Also add checks for grenade throw time if we dont have that yet.
	if (g_LocalPlayer->m_hActiveWeapon().Get()->IsWeaponNonAim() || g_LocalPlayer->m_hActiveWeapon().Get()->m_iClip1() < 1)
		return;

	TargetEntities();
	HN34207389::call();

	if (g_LocalPlayer->m_hActiveWeapon().Get()->IsAssaultRifle())
	{
		override_hitchance = g_Options.rage_hitchance_amount_assaultrifle;
		override_mindmg = g_Options.rage_mindmg_amount_assaultrifle;
	}

	if (g_LocalPlayer->m_hActiveWeapon().Get()->IsPistolNotDeagR8())
	{
		override_hitchance = g_Options.rage_hitchance_amount_pistol;
		override_mindmg = g_Options.rage_mindmg_amount_pistol;
	}

	if (g_LocalPlayer->m_hActiveWeapon().Get()->IsAutoSniper())
	{
		override_hitchance = g_Options.rage_hitchance_amount_auto;
		override_mindmg = g_Options.rage_mindmg_amount_auto;
	}

	if (g_LocalPlayer->m_hActiveWeapon().Get()->IsDeagR8())
	{
		override_hitchance = g_Options.rage_hitchance_amount_deagr8;
		override_mindmg = g_Options.rage_mindmg_amount_deagr8;
	}

	if (g_LocalPlayer->m_hActiveWeapon().Get()->IsAWP())
	{
		override_hitchance = g_Options.rage_hitchance_amount_awp;
		override_mindmg = g_Options.rage_mindmg_amount_awp;
	}

	if (g_LocalPlayer->m_hActiveWeapon().Get()->IsScout())
	{
		override_hitchance = g_Options.rage_hitchance_amount_scout;
		override_mindmg = g_Options.rage_mindmg_amount_scout;
	}

	if (g_LocalPlayer->m_hActiveWeapon().Get()->IsMG())
	{
		override_hitchance = g_Options.rage_hitchance_amount_mg;
		override_mindmg = g_Options.rage_mindmg_amount_mg;
	}

	if (g_LocalPlayer->m_hActiveWeapon().Get()->IsSMG())
	{
		override_hitchance = g_Options.rage_hitchance_amount_smg;
		override_mindmg = g_Options.rage_mindmg_amount_smg;
	}

	if (g_LocalPlayer->m_hActiveWeapon().Get()->IsShotgun())
	{
		override_hitchance = g_Options.rage_hitchance_amount_shotgun;
		override_mindmg = g_Options.rage_mindmg_amount_shotgun;
	}
}

bool AimRage::TargetSpecificEnt(C_BasePlayer* pEnt)
{
	int i = pEnt->EntIndex();
	auto firedShots = g_LocalPlayer->m_iShotsFired();

	Vector vecTarget;

	// Disgusting ass codes, can't think of a cleaner way now though. FIX ME.
	bool LagComp_Hitchanced = false;
	if (g_Options.rage_lagcompensation) //&& g_LocalPlayer->m_hActiveWeapon().Get()->IsNotPistol() && pEnt->m_vecVelocity().Length2D() > 0.1)
	{
		if (pEnt->m_vecVelocity().Length2D() > 0.1)
		{
			//Global::CanDoBacktrack = true;
			CMBacktracking::Get().RageBacktrack(pEnt, usercmd, vecTarget, LagComp_Hitchanced);
		}
		else
		{
			//Global::CanDoBacktrack = false;
			matrix3x4_t matrix[128];
			if (!pEnt->SetupBones2(matrix, 128, 256, pEnt->m_flSimulationTime()))
				return false;

			if (g_Options.rage_autobaim && firedShots > g_Options.rage_baim_after_x_shots)
				vecTarget = CalculateBestPoint(pEnt, HITBOX_PELVIS, override_mindmg, true, matrix);
			else
			{
				if (g_LocalPlayer->m_hActiveWeapon()->IsPistolNotDeagR8())
					vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_pistol], override_mindmg, g_Options.bRage_prioritize_pistol, matrix);
				if (g_LocalPlayer->m_hActiveWeapon()->IsDeagR8())
					vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_deagr8], override_mindmg, g_Options.bRage_prioritize_deagr8, matrix);
				if (g_LocalPlayer->m_hActiveWeapon()->IsSMG())
					vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_smg], override_mindmg, g_Options.bRage_prioritize_smg, matrix);
				if (g_LocalPlayer->m_hActiveWeapon()->IsMG())
					vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_mg], override_mindmg, g_Options.bRage_prioritize_mg, matrix);
				if (g_LocalPlayer->m_hActiveWeapon()->IsShotgun())
					vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_shotgun], override_mindmg, g_Options.bRage_prioritize_shotgun, matrix);
				if (g_LocalPlayer->m_hActiveWeapon()->IsAssaultRifle())
					vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_assaultrifle], override_mindmg, g_Options.bRage_prioritize_assaultrifle, matrix);
				if (g_LocalPlayer->m_hActiveWeapon()->IsScout())
					vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_scout], override_mindmg, g_Options.bRage_prioritize_scout, matrix);
				if (g_LocalPlayer->m_hActiveWeapon()->IsAWP())
					vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_awp], override_mindmg, g_Options.bRage_prioritize_awp, matrix);
				if (g_LocalPlayer->m_hActiveWeapon()->IsAutoSniper())
					vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_auto], override_mindmg, g_Options.bRage_prioritize_auto, matrix);
			}
		}
	}
	else
	{
		//Global::CanDoBacktrack = false;
		matrix3x4_t matrix[128];
		if (!pEnt->SetupBones2(matrix, 128, 256, pEnt->m_flSimulationTime()))
			return false;

		if (g_Options.rage_autobaim && firedShots > g_Options.rage_baim_after_x_shots)
			vecTarget = CalculateBestPoint(pEnt, HITBOX_PELVIS, override_mindmg, true, matrix);
		else
		{
			if (g_LocalPlayer->m_hActiveWeapon()->IsPistolNotDeagR8())
				vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_pistol], override_mindmg, g_Options.bRage_prioritize_pistol, matrix);
			if (g_LocalPlayer->m_hActiveWeapon()->IsDeagR8())
				vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_deagr8], override_mindmg, g_Options.bRage_prioritize_deagr8, matrix);
			if (g_LocalPlayer->m_hActiveWeapon()->IsSMG())
				vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_smg], override_mindmg, g_Options.bRage_prioritize_smg, matrix);
			if (g_LocalPlayer->m_hActiveWeapon()->IsMG())
				vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_mg], override_mindmg, g_Options.bRage_prioritize_mg, matrix);
			if (g_LocalPlayer->m_hActiveWeapon()->IsShotgun())
				vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_shotgun], override_mindmg, g_Options.bRage_prioritize_shotgun, matrix);
			if (g_LocalPlayer->m_hActiveWeapon()->IsAssaultRifle())
				vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_assaultrifle], override_mindmg, g_Options.bRage_prioritize_assaultrifle, matrix);
			if (g_LocalPlayer->m_hActiveWeapon()->IsScout())
				vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_scout], override_mindmg, g_Options.bRage_prioritize_scout, matrix);
			if (g_LocalPlayer->m_hActiveWeapon()->IsAWP())
				vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_awp], override_mindmg, g_Options.bRage_prioritize_awp, matrix);
			if (g_LocalPlayer->m_hActiveWeapon()->IsAutoSniper())
				vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[g_Options.iRage_hitbox_auto], override_mindmg, g_Options.bRage_prioritize_auto, matrix);
		}
	}

	// Invalid target/no hitable points at all.
	if (!vecTarget.IsValid())
		return false;

	AutoStop();
	AutoCrouch();
	HN34207389::call();

	QAngle new_aim_angles = Math::CalcAngle(g_LocalPlayer->GetEyePos(), vecTarget) - (g_Options.rage_norecoil ? g_LocalPlayer->m_aimPunchAngle() * 2.f : QAngle(0, 0, 0));
	this->usercmd->viewangles = Global::vecVisualAimbotAngs = new_aim_angles;
	Global::vecVisualAimbotAngs += (g_Options.removals_novisualrecoil ? g_LocalPlayer->m_aimPunchAngle() * 2.f : QAngle(0, 0, 0));
	Global::bVisualAimbotting = true;

	if (this->can_fire_weapon)
	{
		// Save more fps by remembering to try the same entity again next time.
		prev_aimtarget = pEnt->EntIndex();

		if (g_Options.rage_autoscope && g_LocalPlayer->m_hActiveWeapon().Get()->IsAutoScopeable() && g_LocalPlayer->m_hActiveWeapon().Get()->m_zoomLevel() == 0)
		{
			usercmd->buttons |= IN_ATTACK2;
		}
		else if ((g_Options.rage_lagcompensation && LagComp_Hitchanced) || (!LagComp_Hitchanced && HitChance(usercmd->viewangles, pEnt, override_hitchance)))
		{
			if (g_CVar->FindVar("sv_usercmd_custom_random_seed")->GetBool() && g_Options.rage_nospread)
			{
				int uiSeed = MD5_PseudoRandom(usercmd->command_number) & 0x7FFFFFFF;
				Utils::RandomSeed(uiSeed & 255);
				local_weapon->UpdateAccuracyPenalty();
				float fRand1 = Utils::RandomFloat(0.0f, 1.0f);
				float fRandPi1 = Utils::RandomFloat(0.0f, 2.0f * (float)M_PI);
				float fRand2 = Utils::RandomFloat(0.0f, 1.f);
				float fRandPi2 = Utils::RandomFloat(0.0f, 2.0f * (float)M_PI);
				if (local_weapon->GetClientClass()->m_ClassID == WEAPON_REVOLVER && usercmd->buttons & IN_ATTACK2)
				{
					fRand1 = 1.f - fRand1 * fRand1;
					fRand2 = 1.f - fRand2 * fRand2;
				}

				float fRandInaccuracy = fRand1 * local_weapon->GetInaccuracy();
				float fRandSpread = fRand2 * local_weapon->GetSpread();

				float fSpreadX = cos(fRandPi1) * fRandInaccuracy + cos(fRandPi2) * fRandSpread;
				float fSpreadY = sin(fRandPi1) * fRandInaccuracy + sin(fRandPi2) * fRandSpread;

				usercmd->viewangles.pitch += RAD2DEG(atan(sqrt(fSpreadX * fSpreadX + fSpreadY * fSpreadY)));
				usercmd->viewangles.roll = RAD2DEG(atan2(fSpreadX, fSpreadY));
			}
			Global::bAimbotting = true;

			if (g_Options.rage_autoshoot)
			{
				usercmd->buttons |= IN_ATTACK;
			}
		}
	}

	return true;
}

void AimRage::TargetEntities()
{
	static C_BaseCombatWeapon* oldWeapon; // what is this for?
	if (local_weapon != oldWeapon)
	{
		oldWeapon = local_weapon;
		usercmd->buttons &= ~IN_ATTACK;
		return;
	}

	if (local_weapon->IsPistolNotDeagR8() && usercmd->tick_count % 2)
	{
		static int lastshot;
		if (usercmd->buttons & IN_ATTACK)
			lastshot++;

		if (!usercmd->buttons & IN_ATTACK || lastshot > 1)
		{
			usercmd->buttons &= ~IN_ATTACK;
			lastshot = 0;
		}
		return;
	}

	/*
		We should also add those health/fov based memes and only check newest record. Good enough IMO
	*/

	this->can_fire_weapon = local_weapon->CanFire();

	if (prev_aimtarget && CheckTarget(prev_aimtarget))
	{
		if (TargetSpecificEnt(C_BasePlayer::GetPlayerByIndex(prev_aimtarget)))
			return;
	}

	float bestFOV = g_Options.rage_fieldofview;
	C_BasePlayer* bestTarget = nullptr;

	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		// Failed to shoot at him again, reset him and try others.
		if (prev_aimtarget && prev_aimtarget == i)
		{
			prev_aimtarget = NULL;
			continue;
		}

		if (!CheckTarget(i))
			continue;

		C_BasePlayer* player = C_BasePlayer::GetPlayerByIndex(i);

	float fov = Math::GetFov(usercmd->viewangles, Math::CalcAngle(g_LocalPlayer->GetEyePos(), player->m_vecOrigin()));

		if (fov < bestFOV) {
			bestFOV = fov;
			bestTarget = player;
		}
	}
	if (bestTarget)
	{
		if (TargetSpecificEnt(bestTarget))
			return;
	}
	HN34207389::call();
}

float AimRage::BestHitPoint(C_BasePlayer *player, int prioritized, float minDmg, mstudiohitboxset_t *hitset, matrix3x4_t matrix[], Vector &vecOut)
{
	mstudiobbox_t *hitbox = hitset->pHitbox(prioritized);
	if (!hitbox)
		return 0.f;

	std::vector<Vector> vecArray;
	float flHigherDamage = 0.f;

	float mod = hitbox->m_flRadius != -1.f ? hitbox->m_flRadius : 0.f;

	Vector max;
	Vector min;

	Math::VectorTransform(hitbox->bbmax + mod, matrix[hitbox->bone], max);
	Math::VectorTransform(hitbox->bbmin - mod, matrix[hitbox->bone], min);

	auto center = (min + max) * 0.5f;

	QAngle curAngles = Math::CalcAngle(center, g_LocalPlayer->GetEyePos());

	Vector forward;
	Math::AngleVectors(curAngles, forward);

	Vector right = forward.Cross(Vector(0, 0, 1));
	Vector left = Vector(-right.x, -right.y, right.z);

	Vector top = Vector(0, 0, 1);
	Vector bot = Vector(0, 0, -1);

	float POINT_SCALE;// = g_Options.rage_pointscale_amount_pistol;
	if (g_LocalPlayer->m_hActiveWeapon()->IsPistolNotDeagR8())
		POINT_SCALE = g_Options.rage_pointscale_amount_pistol;

	if (g_LocalPlayer->m_hActiveWeapon()->IsDeagR8())
		POINT_SCALE = g_Options.rage_pointscale_amount_deagr8;

	if (g_LocalPlayer->m_hActiveWeapon()->IsSMG())
		POINT_SCALE = g_Options.rage_pointscale_amount_smg;

	if (g_LocalPlayer->m_hActiveWeapon()->IsMG())
		POINT_SCALE = g_Options.rage_pointscale_amount_mg;

	if (g_LocalPlayer->m_hActiveWeapon()->IsShotgun())
		POINT_SCALE = g_Options.rage_pointscale_amount_shotgun;

	if (g_LocalPlayer->m_hActiveWeapon()->IsAssaultRifle())
		POINT_SCALE = g_Options.rage_pointscale_amount_assaultrifle;

	if (g_LocalPlayer->m_hActiveWeapon()->IsScout())
		POINT_SCALE = g_Options.rage_pointscale_amount_scout;

	if (g_LocalPlayer->m_hActiveWeapon()->IsAWP())
		POINT_SCALE = g_Options.rage_pointscale_amount_awp;

	if (g_LocalPlayer->m_hActiveWeapon()->IsAutoSniper())
		POINT_SCALE = g_Options.rage_pointscale_amount_auto;

	if (g_LocalPlayer->m_hActiveWeapon()->IsPistolNotDeagR8() && g_Options.bRage_multipoint_pistol ||
		g_LocalPlayer->m_hActiveWeapon()->IsDeagR8() && g_Options.bRage_multipoint_deagr8 ||
		g_LocalPlayer->m_hActiveWeapon()->IsSMG() && g_Options.bRage_multipoint_smg ||
		g_LocalPlayer->m_hActiveWeapon()->IsMG() && g_Options.bRage_multipoint_mg ||
		g_LocalPlayer->m_hActiveWeapon()->IsShotgun() && g_Options.bRage_multipoint_shotgun ||
		g_LocalPlayer->m_hActiveWeapon()->IsAssaultRifle() && g_Options.bRage_multipoint_assaultrifle ||
		g_LocalPlayer->m_hActiveWeapon()->IsScout() && g_Options.bRage_multipoint_scout ||
		g_LocalPlayer->m_hActiveWeapon()->IsAWP() && g_Options.bRage_multipoint_awp ||
		g_LocalPlayer->m_hActiveWeapon()->IsAutoSniper() && g_Options.bRage_multipoint_auto)
	{
		switch (prioritized)
		{
		case HITBOX_HEAD:
			for (auto i = 0; i < 4; ++i)
			{
				vecArray.emplace_back(center);
			}
			vecArray[1] += top * (hitbox->m_flRadius * POINT_SCALE);
			vecArray[2] += right * (hitbox->m_flRadius * POINT_SCALE);
			vecArray[3] += left * (hitbox->m_flRadius * POINT_SCALE);
			break;

		default:

			for (auto i = 0; i < 2; ++i)
			{
				vecArray.emplace_back(center);
			}
			vecArray[0] += right * (hitbox->m_flRadius * POINT_SCALE);
			vecArray[1] += left * (hitbox->m_flRadius * POINT_SCALE);
			break;
		}
	}
	else
		vecArray.emplace_back(center);

	for (Vector cur : vecArray)
	{
		float flCurDamage = GetDamageVec(cur, player, prioritized);

		if (!flCurDamage)
			continue;

		if ((flCurDamage > flHigherDamage) && (flCurDamage > minDmg))
		{
			flHigherDamage = flCurDamage;
			vecOut = cur;
		}
	}
	return flHigherDamage;
	HN34207389::call();
}

Vector AimRage::CalculateBestPoint(C_BasePlayer *player, int prioritized, float minDmg, bool onlyPrioritized, matrix3x4_t matrix[])
{
	studiohdr_t *studioHdr = g_MdlInfo->GetStudiomodel(player->GetModel());
	mstudiohitboxset_t *set = studioHdr->pHitboxSet(player->m_nHitboxSet());
	Vector vecOutput;

	if (BestHitPoint(player, prioritized, minDmg, set, matrix, vecOutput) > minDmg && onlyPrioritized)
	{
		return vecOutput;
	}
	else
	{
		float flHigherDamage = 0.f;

		Vector vecCurVec;

		// why not use all the hitboxes then
		//static Hitboxes hitboxesLoop;
		static int hitboxesLoop[] =
		{
			HITBOX_HEAD,
			HITBOX_PELVIS,
			HITBOX_UPPER_CHEST,
			HITBOX_CHEST,
			HITBOX_NECK,
			HITBOX_LEFT_FOREARM,
			HITBOX_RIGHT_FOREARM,
			HITBOX_RIGHT_HAND,
			HITBOX_LEFT_THIGH,
			HITBOX_RIGHT_THIGH,
			HITBOX_LEFT_CALF,
			HITBOX_RIGHT_CALF,
			HITBOX_LEFT_FOOT,
			HITBOX_RIGHT_FOOT
		};

		int loopSize = ARRAYSIZE(hitboxesLoop);
		for (int i = 0; i < loopSize; ++i)
		{
			if (g_LocalPlayer->m_hActiveWeapon()->IsPistolNotDeagR8())
				if (!g_Options.rage_multiHitboxesPistol[i])
					continue;
			if (g_LocalPlayer->m_hActiveWeapon()->IsDeagR8())
				if (!g_Options.rage_multiHitboxesDeagR8[i])
					continue;
			if (g_LocalPlayer->m_hActiveWeapon()->IsSMG())
				if (!g_Options.rage_multiHitboxesSMG[i])
					continue;
			if (g_LocalPlayer->m_hActiveWeapon()->IsShotgun())
				if (!g_Options.rage_multiHitboxesShotgun[i])
					continue;
			if (g_LocalPlayer->m_hActiveWeapon()->IsMG())
				if (!g_Options.rage_multiHitboxesMG[i])
					continue;
			if (g_LocalPlayer->m_hActiveWeapon()->IsScout())
				if (!g_Options.rage_multiHitboxesScout[i])
					continue;
			if (g_LocalPlayer->m_hActiveWeapon()->IsAWP())
				if (!g_Options.rage_multiHitboxesAWP[i])
					continue;
			if (g_LocalPlayer->m_hActiveWeapon()->IsAutoSniper())
				if (!g_Options.rage_multiHitboxesAuto[i])
					continue;
			if (g_LocalPlayer->m_hActiveWeapon()->IsAssaultRifle())
				if (!g_Options.rage_multiHitboxesAssaultRifle[i])
					continue;

			float flCurDamage = BestHitPoint(player, hitboxesLoop[i], minDmg, set, matrix, vecCurVec);

			if (!flCurDamage)
				continue;

			if (flCurDamage > flHigherDamage)
			{
				flHigherDamage = flCurDamage;
				vecOutput = vecCurVec;
				if (static_cast<int32_t>(flHigherDamage) >= player->m_iHealth())
					break;
			}
		}
		return vecOutput;
	}
	HN34207389::call();
}

bool AimRage::CheckTarget(int i)
{
	C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(i);

	if (!player || player == nullptr)
		return false;

	if (player == g_LocalPlayer)
		return false;

	if (player->IsTeamMate())
		return false;

	if (player->IsDormant())
		return false;

	if (player->m_bGunGameImmunity())
		return false;

	if (!player->IsAlive())
		return false;

	return true;
	HN34207389::call();
}

bool AimRage::HitChance(QAngle angles, C_BasePlayer *ent, float chance)
{
	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();

	if (!weapon)
		return false;

	Vector forward, right, up;
	Vector src = g_LocalPlayer->GetEyePos();
	Math::AngleVectors(angles, forward, right, up);

	int cHits = 0;
	int cNeededHits = static_cast<int>(150.f * (chance / 100.f));

	weapon->UpdateAccuracyPenalty();
	float weap_spread = weapon->GetSpread();
	float weap_inaccuracy = weapon->GetInaccuracy();

	for (int i = 0; i < 150; i++)
	{
		float a = Utils::RandomFloat(0.f, 1.f);
		float b = Utils::RandomFloat(0.f, 2.f * PI_F);
		float c = Utils::RandomFloat(0.f, 1.f);
		float d = Utils::RandomFloat(0.f, 2.f * PI_F);

		float inaccuracy = a * weap_inaccuracy;
		float spread = c * weap_spread;

		if (weapon->m_iItemDefinitionIndex() == 64)
		{
			a = 1.f - a * a;
			a = 1.f - c * c;
		}

		Vector spreadView((cos(b) * inaccuracy) + (cos(d) * spread), (sin(b) * inaccuracy) + (sin(d) * spread), 0), direction;

		direction.x = forward.x + (spreadView.x * right.x) + (spreadView.y * up.x);
		direction.y = forward.y + (spreadView.x * right.y) + (spreadView.y * up.y);
		direction.z = forward.z + (spreadView.x * right.z) + (spreadView.y * up.z);
		direction.Normalized();

		QAngle viewAnglesSpread;
		Math::VectorAngles(direction, up, viewAnglesSpread);
		Math::NormalizeAngles(viewAnglesSpread);

		Vector viewForward;
		Math::AngleVectors(viewAnglesSpread, viewForward);
		viewForward.NormalizeInPlace();

		viewForward = src + (viewForward * weapon->GetWeapInfo()->m_fRange());

		trace_t tr;
		Ray_t ray;

		ray.Init(src, viewForward);
		g_EngineTrace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, ent, &tr);

		if (tr.hit_entity == ent)
			++cHits;

		if (static_cast<int>((static_cast<float>(cHits) / 150.f) * 100.f) >= chance)
			return true;

		if ((150 - i + cHits) < cNeededHits)
			return false;
	}
	return false;
	HN34207389::call();
}

void AimRage::AutoStop()
{
	if (!g_Options.rage_autostop)
		return;

	if (g_InputSystem->IsButtonDown(g_Options.misc_fakewalk_bind))
		return;

	usercmd->forwardmove = 0;
	usercmd->sidemove = 0;
}

bool AimRage::CockRevolver()
{
	// 0.234375f to cock and shoot, 15 ticks in 64 servers, 30(31?) in 128

	// THIS DOESNT WORK, WILL WORK ON LATER AGAIN WHEN I FEEL LIKE KILLING MYSELF

	// DONT USE TIME_TO_TICKS as these values aren't good for it. it's supposed to be 0.2f but that's also wrong
	constexpr float REVOLVER_COCK_TIME = 0.234375f;// 0.2421875f;
	const int count_needed = floor(REVOLVER_COCK_TIME / g_GlobalVars->interval_per_tick);
	static int cocks_done = 0;

	if (!local_weapon ||
		local_weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER ||
		g_LocalPlayer->m_flNextAttack() > g_GlobalVars->curtime ||
		local_weapon->IsReloading())
	{
		if (local_weapon && local_weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
			usercmd->buttons &= ~IN_ATTACK;
		cocks_done = 0;
		return true;
	}

	if (cocks_done < count_needed)
	{
		usercmd->buttons |= IN_ATTACK;
		++cocks_done;
		return false;
	}
	else
	{
		usercmd->buttons &= ~IN_ATTACK;
		cocks_done = 0;
		return true;
	}

	// 0.0078125 - 128ticks - 31 - 0.2421875
	// 0.015625  - 64 ticks - 16 - 0.234375f

	usercmd->buttons |= IN_ATTACK;

	/*
		3 steps:

		1. Come, not time for update, cock and return false;

		2. Come, completely outdated, cock and set time, return false;

		3. Come, time is up, cock and return true;

		Notes:
			Will I not have to account for high ping when I shouldn't send another update?
			Lower framerate than ticks = riperino? gotta check if lower then account by sending earlier | frametime memes
	*/

	float curtime = TICKS_TO_TIME(g_LocalPlayer->m_nTickBase());
	static float next_shoot_time = 0.f;

	bool ret = false;

	if (next_shoot_time - curtime < -0.5)
		next_shoot_time = curtime + 0.2f - g_GlobalVars->interval_per_tick; // -1 because we already cocked THIS tick ???

	if (next_shoot_time - curtime - g_GlobalVars->interval_per_tick <= 0.f) {
		next_shoot_time = curtime + 0.2f;
		ret = true;

		// should still go for one more tick but if we do, we're gonna shoot sooo idk how2do rn, its late
		// the aimbot should decide whether to shoot or not yeh
	}

	return ret;
	HN34207389::call();
}

void AimRage::AutoCrouch()
{
	if (!g_Options.rage_autocrouch)
		return;

	usercmd->buttons |= IN_DUCK;
	HN34207389::call();
}

float AimRage::GetDamageVec(const Vector &vecPoint, C_BasePlayer *player, int hitbox)
{
	float damage = 0.f;

	Vector rem = vecPoint;

	FireBulletData data;

	data.src = g_LocalPlayer->GetEyePos();
	data.filter.pSkip = g_LocalPlayer;

	QAngle angle = Math::CalcAngle(data.src, rem);
	Math::AngleVectors(angle, data.direction);

	data.direction.Normalized();

	auto weap = g_LocalPlayer->m_hActiveWeapon().Get();
	if (SimulateFireBullet(weap, data, player, hitbox))
		damage = data.current_damage;

	return damage;
	HN34207389::call();
}

bool AimRage::SimulateFireBullet(C_BaseCombatWeapon *weap, FireBulletData &data, C_BasePlayer *player, int hitbox)
{
	if (!weap)
		return false;

	auto GetHitgroup = [](int hitbox) -> int
	{
		switch (hitbox)
		{
		case HITBOX_HEAD:
		case HITBOX_NECK:
			return HITGROUP_HEAD;
		case HITBOX_LOWER_CHEST:
		case HITBOX_CHEST:
		case HITBOX_UPPER_CHEST:
			return HITGROUP_CHEST;
		case HITBOX_STOMACH:
		case HITBOX_PELVIS:
			return HITGROUP_STOMACH;
		case HITBOX_LEFT_HAND:
		case HITBOX_LEFT_UPPER_ARM:
		case HITBOX_LEFT_FOREARM:
			return HITGROUP_LEFTARM;
		case HITBOX_RIGHT_HAND:
		case HITBOX_RIGHT_UPPER_ARM:
		case HITBOX_RIGHT_FOREARM:
			return HITGROUP_RIGHTARM;
		case HITBOX_LEFT_THIGH:
		case HITBOX_LEFT_CALF:
		case HITBOX_LEFT_FOOT:
			return HITGROUP_LEFTLEG;
		case HITBOX_RIGHT_THIGH:
		case HITBOX_RIGHT_CALF:
		case HITBOX_RIGHT_FOOT:
			return HITGROUP_RIGHTLEG;
		default:
			return -1;
		}
	};

	data.penetrate_count = 4;
	data.trace_length = 0.0f;
	WeapInfo_t *weaponData = g_LocalPlayer->m_hActiveWeapon().Get()->GetWeapInfo();

	if (weaponData == NULL)
		return false;

	data.current_damage = (float)weaponData->m_iDamage();

	while ((data.penetrate_count > 0) && (data.current_damage >= 1.0f))
	{
		data.trace_length_remaining = weaponData->m_fRange() - data.trace_length;

		Vector end = data.src + data.direction * data.trace_length_remaining;

		traceIt(data.src, end, MASK_SHOT | CONTENTS_GRATE, g_LocalPlayer, &data.enter_trace);
		ClipTraceToPlayers(data.src, end + data.direction * 40.f, MASK_SHOT | CONTENTS_GRATE, &data.filter, &data.enter_trace);

		if (data.enter_trace.fraction == 1.0f)
		{
			if (player && !(player->m_fFlags() & FL_ONGROUND))
			{
				data.enter_trace.hitgroup = GetHitgroup(hitbox);
				data.enter_trace.hit_entity = player;
			}
			else
				break;
		}

		surfacedata_t *enter_surface_data = g_PhysSurface->GetSurfaceData(data.enter_trace.surface.surfaceProps);
		unsigned short enter_material = enter_surface_data->game.material;
		float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;

		data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
		data.current_damage *= pow(weaponData->m_fRangeModifier(), data.trace_length * 0.002);

		if (data.trace_length > 3000.f && weaponData->m_fPenetration() > 0.f || enter_surf_penetration_mod < 0.1f)
			break;

		if ((data.enter_trace.hitgroup <= 7) && (data.enter_trace.hitgroup > 0))
		{
			C_BasePlayer *pPlayer = reinterpret_cast<C_BasePlayer*>(data.enter_trace.hit_entity);
			if (pPlayer->IsPlayer() && pPlayer->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
				return false;

			ScaleDamage(data.enter_trace.hitgroup, pPlayer, weaponData->m_fArmorRatio(), data.current_damage);

			return true;
		}

		if (!HandleBulletPenetration(weaponData, data))
			break;
	}

	return false;
	HN34207389::call();
}

bool AimRage::HandleBulletPenetration(WeapInfo_t *wpn_data, FireBulletData &data)
{
	bool bSolidSurf = ((data.enter_trace.contents >> 3) & CONTENTS_SOLID);
	bool bLightSurf = (data.enter_trace.surface.flags >> 7) & SURF_LIGHT;

	surfacedata_t *enter_surface_data = g_PhysSurface->GetSurfaceData(data.enter_trace.surface.surfaceProps);
	unsigned short enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;

	if (!data.penetrate_count && !bLightSurf && !bSolidSurf && enter_material != 89)
	{
		if (enter_material != 71)
			return false;
	}

	if (data.penetrate_count <= 0 || wpn_data->m_fPenetration() <= 0.f)
		return false;

	Vector dummy;
	trace_t trace_exit;

	if (!TraceToExit(dummy, &data.enter_trace, data.enter_trace.endpos, data.direction, &trace_exit))
	{
		if (!(g_EngineTrace->GetPointContents(dummy, MASK_SHOT_HULL) & MASK_SHOT_HULL))
			return false;
	}

	surfacedata_t *exit_surface_data = g_PhysSurface->GetSurfaceData(trace_exit.surface.surfaceProps);
	unsigned short exit_material = exit_surface_data->game.material;

	float exit_surf_penetration_mod = exit_surface_data->game.flPenetrationModifier;
	float exit_surf_damage_mod = exit_surface_data->game.flDamageModifier;

	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;

	if (enter_material == 89 || enter_material == 71)
	{
		combined_penetration_modifier = 3.0f;
		final_damage_modifier = 0.05f;
	}
	else if (bSolidSurf || bLightSurf)
	{
		combined_penetration_modifier = 1.0f;
		final_damage_modifier = 0.16f;
	}
	else
	{
		combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f;
	}

	if (enter_material == exit_material)
	{
		if (exit_material == 87 || exit_material == 85)
			combined_penetration_modifier = 3.0f;
		else if (exit_material == 76)
			combined_penetration_modifier = 2.0f;
	}

	float modifier = fmaxf(0.0f, 1.0f / combined_penetration_modifier);
	float thickness = (trace_exit.endpos - data.enter_trace.endpos).LengthSqr();
	float taken_damage = ((modifier * 3.0f) * fmaxf(0.0f, (3.0f / wpn_data->m_fPenetration()) * 1.25f) + (data.current_damage * final_damage_modifier)) + ((thickness * modifier) / 24.0f);

	float lost_damage = fmaxf(0.0f, taken_damage);

	if (lost_damage > data.current_damage)
		return false;

	if (lost_damage > 0.0f)
		data.current_damage -= lost_damage;

	if (data.current_damage < 1.0f)
		return false;

	data.src = trace_exit.endpos;
	data.penetrate_count--;

	return true;
	HN34207389::call();
}

bool AimRage::TraceToExit(Vector &end, CGameTrace *enter_trace, Vector start, Vector dir, CGameTrace *exit_trace)
{
	auto distance = 0.0f;
	int first_contents = 0;

	while (distance < 90.0f)
	{
		distance += 4.0f;
		end = start + (dir * distance);

		if(!first_contents)
			first_contents = g_EngineTrace->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX);

		auto point_contents = g_EngineTrace->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX);

		if (point_contents & MASK_SHOT_HULL && (!(point_contents & CONTENTS_HITBOX) || first_contents == point_contents))
			continue;

		auto new_end = end - (dir * 4.0f);

		traceIt(end, new_end, MASK_SHOT | CONTENTS_GRATE, nullptr, exit_trace);

		if (exit_trace->startsolid && (exit_trace->surface.flags & SURF_HITBOX) < 0)
		{
			traceIt(end, start, MASK_SHOT_HULL, reinterpret_cast<C_BasePlayer*>(exit_trace->hit_entity), exit_trace);

			if (exit_trace->DidHit() && !exit_trace->startsolid)
			{
				end = exit_trace->endpos;
				return true;
			}
			continue;
		}

		if (!exit_trace->DidHit() || exit_trace->startsolid)
		{
			if (enter_trace->hit_entity)
			{
				if (enter_trace->DidHitNonWorldEntity() && IsBreakableEntity(reinterpret_cast<C_BasePlayer*>(enter_trace->hit_entity)))
				{
					*exit_trace = *enter_trace;
					exit_trace->endpos = start + dir;
					return true;
				}
			}
			continue;
		}

		if ((exit_trace->surface.flags >> 7) & SURF_LIGHT)
		{
			if (IsBreakableEntity(reinterpret_cast<C_BasePlayer*>(exit_trace->hit_entity)) && IsBreakableEntity(reinterpret_cast<C_BasePlayer*>(enter_trace->hit_entity)))
			{
				end = exit_trace->endpos;
				return true;
			}

			if (!((enter_trace->surface.flags >> 7) & SURF_LIGHT))
				continue;
		}

		if (exit_trace->plane.normal.Dot(dir) <= 1.0f)
		{
			float fraction = exit_trace->fraction * 4.0f;
			end = end - (dir * fraction);

			return true;
		}
	}
	return false;
	HN34207389::call();
}

bool AimRage::IsBreakableEntity(C_BasePlayer *ent)
{
	typedef bool(__thiscall *isBreakbaleEntityFn)(C_BasePlayer*);
	static isBreakbaleEntityFn IsBreakableEntityFn = (isBreakbaleEntityFn)Utils::PatternScan(GetModuleHandle("client.dll"), "55 8B EC 51 56 8B F1 85 F6 74 68");

	if (IsBreakableEntityFn)
	{
		// 0x280 = m_takedamage

		auto backupval = *reinterpret_cast<int*>((uint32_t)ent + 0x280);
		auto className = ent->GetClientClass()->m_pNetworkName;

		if (ent != g_EntityList->GetClientEntity(0))
		{
			// CFuncBrush:
			// (className[1] != 'F' || className[4] != 'c' || className[5] != 'B' || className[9] != 'h')
			if ((className[1] == 'B' && className[9] == 'e' && className[10] == 'S' && className[16] == 'e') // CBreakableSurface
				|| (className[1] != 'B' || className[5] != 'D')) // CBaseDoor because fuck doors
			{
				*reinterpret_cast<int*>((uint32_t)ent + 0x280) = 2;
			}
		}

		bool retn = IsBreakableEntityFn(ent);

		*reinterpret_cast<int*>((uint32_t)ent + 0x280) = backupval;

		return retn;
	}
	else
		return false;
	HN34207389::call();
}

void AimRage::ClipTraceToPlayers(const Vector &vecAbsStart, const Vector &vecAbsEnd, unsigned int mask, ITraceFilter *filter, CGameTrace *tr)
{
	trace_t playerTrace;
	Ray_t ray;
	float smallestFraction = tr->fraction;
	const float maxRange = 60.0f;

	ray.Init(vecAbsStart, vecAbsEnd);

	for (int i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(i);

		if (!player || !player->IsAlive() || player->IsDormant())
			continue;

		if (filter && filter->ShouldHitEntity(player, mask) == false)
			continue;

		float range = Math::DistanceToRay(player->WorldSpaceCenter(), vecAbsStart, vecAbsEnd);
		if (range < 0.0f || range > maxRange)
			continue;

		g_EngineTrace->ClipRayToEntity(ray, mask | CONTENTS_HITBOX, player, &playerTrace);
		if (playerTrace.fraction < smallestFraction)
		{
			*tr = playerTrace;
			smallestFraction = playerTrace.fraction;
		}
	}
	HN34207389::call();
}

void AimRage::ScaleDamage(int hitgroup, C_BasePlayer *player, float weapon_armor_ratio, float &current_damage)
{
	bool heavArmor = player->m_bHasHeavyArmor();
	int armor = player->m_ArmorValue();

	switch (hitgroup)
	{
	case HITGROUP_HEAD:

		if (heavArmor)
			current_damage *= (current_damage * 4.f) * 0.5f;
		else
			current_damage *= 4.f;

		break;

	case HITGROUP_CHEST:
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:

		current_damage *= 1.f;
		break;

	case HITGROUP_STOMACH:

		current_damage *= 1.25f;
		break;

	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:

		current_damage *= 0.75f;
		break;
	}

	if (IsArmored(player, armor, hitgroup))
	{
		float v47 = 1.f, armor_bonus_ratio = 0.5f, armor_ratio = weapon_armor_ratio * 0.5f;

		if (heavArmor)
		{
			armor_bonus_ratio = 0.33f;
			armor_ratio = (weapon_armor_ratio * 0.5f) * 0.5f;
			v47 = 0.33f;
		}

		float new_damage = current_damage * armor_ratio;

		if (heavArmor)
			new_damage *= 0.85f;

		if (((current_damage - (current_damage * armor_ratio)) * (v47 * armor_bonus_ratio)) > armor)
			new_damage = current_damage - (armor / armor_bonus_ratio);

		current_damage = new_damage;
	}
	HN34207389::call();
}

bool AimRage::IsArmored(C_BasePlayer *player, int armorVal, int hitgroup)
{
	bool res = false;

	if (armorVal > 0)
	{
		switch (hitgroup)
		{
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:

			res = true;
			break;

		case HITGROUP_HEAD:

			res = player->m_bHasHelmet();
			break;

		}
	}

	return res;
	HN34207389::call();
}

void AimRage::traceIt(Vector &vecAbsStart, Vector &vecAbsEnd, unsigned int mask, C_BasePlayer *ign, CGameTrace *tr)
{
	Ray_t ray;

	CTraceFilter filter;
	filter.pSkip = ign;

	ray.Init(vecAbsStart, vecAbsEnd);

	g_EngineTrace->TraceRay(ray, mask, &filter, tr);
	HN34207389::call();
}

int AimRage::GetTickbase(CUserCmd* ucmd) {

	static int g_tick = 0;
	static CUserCmd* g_pLastCmd = nullptr;

	if (!ucmd)
		return g_tick;

	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
		g_tick = g_LocalPlayer->m_nTickBase();
	}
	else {
		// Required because prediction only runs on frames, not ticks
		// So if your framerate goes below tickrate, m_nTickBase won't update every tick
		++g_tick;
	}

	g_pLastCmd = ucmd;
	return g_tick;
//	HN34207389::call();
}
