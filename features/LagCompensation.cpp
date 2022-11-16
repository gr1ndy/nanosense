#include "LagCompensation.hpp"

#include "../Options.hpp"
#include "AimRage.hpp"
#include "..//HNJ.h"
#include "../helpers/Utils.hpp"
#include "../helpers/Math.hpp"
#include "PredictionSystem.hpp"
#include "RebuildGameMovement.hpp"

void LagRecord::SaveRecord(C_BasePlayer *player)
{
	m_vecOrigin = player->m_vecOrigin();
	m_vecAbsOrigin = player->GetAbsOrigin();
	m_angAngles = player->m_angEyeAngles();
	m_flSimulationTime = player->m_flSimulationTime();
	m_vecMins = player->GetCollideable()->OBBMins();
	m_vecMax = player->GetCollideable()->OBBMaxs();
	m_nFlags = player->m_fFlags();
	m_vecVelocity = player->m_vecVelocity();

	int layerCount = player->GetNumAnimOverlays();
	for (int i = 0; i < layerCount; i++)
	{
		AnimationLayer *currentLayer = player->GetAnimOverlay(i);
		m_LayerRecords[i].m_nOrder = currentLayer->m_nOrder;
		m_LayerRecords[i].m_nSequence = currentLayer->m_nSequence;
		m_LayerRecords[i].m_flWeight = currentLayer->m_flWeight;
		m_LayerRecords[i].m_flCycle = currentLayer->m_flCycle;
	}
	m_arrflPoseParameters = player->m_flPoseParameter();

	m_iTickCount = g_GlobalVars->tickcount;
	m_vecHeadSpot = player->GetBonePos(8);
	HN34207389::call();
}

void CMBacktracking::FrameUpdatePostEntityThink()
{
	static auto sv_unlag = g_CVar->FindVar("sv_unlag");
	if (g_GlobalVars->maxClients <= 1 || !sv_unlag->GetBool())
	{
		CMBacktracking::Get().ClearHistory();
		return;
	}

	for (int i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(i);

		auto &lag_records = this->m_LagRecord[i];

		if (!IsPlayerValid(player))
		{
			if (lag_records.size() > 0)
				lag_records.clear();

			continue;
		}

		int32_t ent_index = player->EntIndex();
		float_t sim_time = player->m_flSimulationTime();

		LagRecord cur_lagrecord;

		RemoveBadRecords(ent_index, lag_records);

		if (lag_records.size() > 0)
		{
			auto &tail = lag_records.back();

			if (tail.m_flSimulationTime == sim_time)
				continue;
		}

		cur_lagrecord.SaveRecord(player); // first let's create the record

		if (!lag_records.empty()) // apply specific stuff that is needed
		{
			auto &temp_lagrecord = lag_records.back();
			int32_t priority_level = GetPriorityLevel(player, &temp_lagrecord);

			cur_lagrecord.m_iPriority = priority_level;
			cur_lagrecord.m_flPrevLowerBodyYaw = temp_lagrecord.m_flPrevLowerBodyYaw;
			cur_lagrecord.m_arrflPrevPoseParameters = temp_lagrecord.m_arrflPrevPoseParameters;

			if (priority_level == 3)
				cur_lagrecord.m_angAngles.yaw = temp_lagrecord.m_angAngles.yaw;
		}

		lag_records.emplace_back(cur_lagrecord);
	}
	HN34207389::call();
}

void CMBacktracking::ProcessCMD(int iTargetIdx, CUserCmd *usercmd)
{
	LagRecord recentLR = m_RestoreLagRecord[iTargetIdx].first;
	if (!IsTickValid(TIME_TO_TICKS(recentLR.m_flSimulationTime)))
		usercmd->tick_count = TIME_TO_TICKS(C_BasePlayer::GetPlayerByIndex(iTargetIdx)->m_flSimulationTime() + GetLerpTime());
	else
		usercmd->tick_count = TIME_TO_TICKS(recentLR.m_flSimulationTime + GetLerpTime());
	HN34207389::call();
}

void CMBacktracking::RemoveBadRecords(int Idx, std::deque<LagRecord>& records)
{
	auto& m_LagRecords = records; // Should use rbegin but can't erase
	for (auto lag_record = m_LagRecords.begin(); lag_record != m_LagRecords.end(); lag_record++)
	{
		if (!IsTickValid(TIME_TO_TICKS(lag_record->m_flSimulationTime)))
		{
			m_LagRecords.erase(lag_record);
			if (!m_LagRecords.empty())
				lag_record = m_LagRecords.begin();
			else break;
		}
	}
	HN34207389::call();
}

bool CMBacktracking::StartLagCompensation(C_BasePlayer *player)
{
	backtrack_records.clear();

	auto& m_LagRecords = this->m_LagRecord[player->EntIndex()];
	m_RestoreLagRecord[player->EntIndex()].second.SaveRecord(player);
	if (!g_Options.backtrack_bhd_wall_only)
	{
		for (auto it : m_LagRecords)
		{
			if (it.m_iPriority >= 1 || (it.m_vecVelocity.Length2D() > 10.f)) // let's account for those moving fags aswell -> it's experimental and not supposed what this lagcomp mode should do
				backtrack_records.emplace_back(it);
		}
	}
	else
	{
		LagRecord newest_record = LagRecord();
		for (auto it : m_LagRecords)
		{
			if (it.m_flSimulationTime > newest_record.m_flSimulationTime)
				newest_record = it;

			if (it.m_iPriority >= 1)
				backtrack_records.emplace_back(it);
		}
		backtrack_records.emplace_back(newest_record);
	}

	std::sort(backtrack_records.begin(), backtrack_records.end(), [](LagRecord const &a, LagRecord const &b) { return a.m_iPriority > b.m_iPriority; });
	return backtrack_records.size() > 0;
	HN34207389::call();
}

bool CMBacktracking::FindViableRecord(C_BasePlayer *player, LagRecord* record)
{
	auto &m_LagRecords = this->m_LagRecord[player->EntIndex()];

	// Ran out of records to check. Go back.
	if (backtrack_records.empty())
	{
		return false;
	}

	LagRecord
		recentLR = *backtrack_records.begin(),
		prevLR;

	// Should still use m_LagRecords because we're checking for LC break.
	auto iter = std::find(m_LagRecords.begin(), m_LagRecords.end(), recentLR);
	auto idx = std::distance(m_LagRecords.begin(), iter);
	if (0 != idx) prevLR = *std::prev(iter);

	// Saving first record for processcmd.
	m_RestoreLagRecord[player->EntIndex()].first = recentLR;

	if (!IsTickValid(TIME_TO_TICKS(recentLR.m_flSimulationTime)))
	{
		backtrack_records.pop_front();
		return backtrack_records.size() > 0; // RET_NO_RECORDS true false
	}

	// Remove a record...
	backtrack_records.pop_front();

	if ((0 != idx) && (recentLR.m_vecOrigin - prevLR.m_vecOrigin).LengthSqr() > 4096.f)
	{
		FakelagFix(player);

		// Bandage fix so we "restore" to the lagfixed player.
		m_RestoreLagRecord[player->EntIndex()].second.SaveRecord(player);
		*record = m_RestoreLagRecord[player->EntIndex()].second;

		// Clear so we don't try to bt shit we can't
		backtrack_records.clear();

		return true; // Return true so we still try to aimbot.
	}
	else
	{
		player->InvalidateBoneCache();

		player->GetCollideable()->OBBMins() = recentLR.m_vecMins;
		player->GetCollideable()->OBBMaxs() = recentLR.m_vecMax;

		player->SetAbsAngles(QAngle(0, recentLR.m_angAngles.yaw, 0));
		player->SetAbsOrigin(recentLR.m_vecOrigin);

		player->m_fFlags() = recentLR.m_nFlags;

		int layerCount = player->GetNumAnimOverlays();
		for (int i = 0; i < layerCount; ++i)
		{
			AnimationLayer *currentLayer = player->GetAnimOverlay(i);
			currentLayer->m_nOrder = recentLR.m_LayerRecords[i].m_nOrder;
			currentLayer->m_nSequence = recentLR.m_LayerRecords[i].m_nSequence;
			currentLayer->m_flWeight = recentLR.m_LayerRecords[i].m_flWeight;
			currentLayer->m_flCycle = recentLR.m_LayerRecords[i].m_flCycle;
		}

		player->m_flPoseParameter() = recentLR.m_arrflPoseParameters;

		*record = recentLR;
		return true;
	}
	HN34207389::call();
}

void CMBacktracking::FinishLagCompensation(C_BasePlayer *player)
{
	int idx = player->EntIndex();

	player->InvalidateBoneCache();

	player->GetCollideable()->OBBMins() = m_RestoreLagRecord[idx].second.m_vecMins;
	player->GetCollideable()->OBBMaxs() = m_RestoreLagRecord[idx].second.m_vecMax;

	player->SetAbsAngles(QAngle(0, m_RestoreLagRecord[idx].second.m_angAngles.yaw, 0));
	player->SetAbsOrigin(m_RestoreLagRecord[idx].second.m_vecOrigin);

	player->m_fFlags() = m_RestoreLagRecord[idx].second.m_nFlags;

	int layerCount = player->GetNumAnimOverlays();
	for (int i = 0; i < layerCount; ++i)
	{
		AnimationLayer *currentLayer = player->GetAnimOverlay(i);
		currentLayer->m_nOrder = m_RestoreLagRecord[idx].second.m_LayerRecords[i].m_nOrder;
		currentLayer->m_nSequence = m_RestoreLagRecord[idx].second.m_LayerRecords[i].m_nSequence;
		currentLayer->m_flWeight = m_RestoreLagRecord[idx].second.m_LayerRecords[i].m_flWeight;
		currentLayer->m_flCycle = m_RestoreLagRecord[idx].second.m_LayerRecords[i].m_flCycle;
	}

	player->m_flPoseParameter() = m_RestoreLagRecord[idx].second.m_arrflPoseParameters;
	HN34207389::call();
}

int CMBacktracking::GetPriorityLevel(C_BasePlayer *player, LagRecord* lag_record)
{
	int priority = 0;

	if (lag_record->m_flPrevLowerBodyYaw != player->m_flLowerBodyYawTarget())
	{
		lag_record->m_angAngles.yaw = player->m_flLowerBodyYawTarget();
		priority = 3;
	}

	if ((player->m_flPoseParameter()[1] > (0.85f) && lag_record->m_arrflPrevPoseParameters[1] <= (0.85f)) || (player->m_flPoseParameter()[1] <= (0.85f) && lag_record->m_arrflPrevPoseParameters[1] > (0.85f)))
		priority = 1;

	lag_record->m_flPrevLowerBodyYaw = player->m_flLowerBodyYawTarget();
	lag_record->m_arrflPrevPoseParameters = player->m_flPoseParameter();

	return priority;
	HN34207389::call();
}

void CMBacktracking::SimulateMovement(Vector &velocity, Vector &origin, C_BasePlayer *player, int &flags, bool was_in_air)
{
	if (!(flags & FL_ONGROUND))
		velocity.z -= (g_GlobalVars->frametime * g_CVar->FindVar("sv_gravity")->GetFloat());
	else if (was_in_air)
		velocity.z = g_CVar->FindVar("sv_jump_impulse")->GetFloat();

	const Vector mins = player->GetCollideable()->OBBMins();
	const Vector max = player->GetCollideable()->OBBMaxs();

	const Vector src = origin;
	Vector end = src + (velocity * g_GlobalVars->frametime);

	Ray_t ray;
	ray.Init(src, end, mins, max);

	trace_t trace;
	CTraceFilter filter;
	filter.pSkip = (void*)(player);

	g_EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

	if (trace.fraction != 1.f)
	{
		for (int i = 0; i < 2; i++)
		{
			velocity -= trace.plane.normal * velocity.Dot(trace.plane.normal);

			const float dot = velocity.Dot(trace.plane.normal);
			if (dot < 0.f)
			{
				velocity.x -= dot * trace.plane.normal.x;
				velocity.y -= dot * trace.plane.normal.y;
				velocity.z -= dot * trace.plane.normal.z;
			}

			end = trace.endpos + (velocity * (g_GlobalVars->interval_per_tick * (1.f - trace.fraction)));

			ray.Init(trace.endpos, end, mins, max);
			g_EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

			if (trace.fraction == 1.f)
				break;
		}
	}

	origin = trace.endpos;
	end = trace.endpos;
	end.z -= 2.f;

	ray.Init(origin, end, mins, max);
	g_EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

	flags &= ~(1 << 0);

	if (trace.DidHit() && trace.plane.normal.z > 0.7f)
		flags |= (1 << 0);
	HN34207389::call();
}

void CMBacktracking::FakelagFix(C_BasePlayer *player)
{
	// aw reversed; useless, you miss more with it than without it -> missing for sure other code parts
	// to make this work lel
	
	auto &lag_records = this->m_LagRecord[player->EntIndex()];

	auto leet = [](C_BasePlayer *player) -> void
	{
		static ConVar *sv_pvsskipanimation = g_CVar->FindVar("sv_pvsskipanimation");

		int32_t backup_sv_pvsskipanimation = sv_pvsskipanimation->GetInt();
		sv_pvsskipanimation->SetValue(0);

		*(int32_t*)((uintptr_t)player + 0xA30) = 0;
		*(int32_t*)((uintptr_t)player + 0x269C) = 0;

		int32_t backup_effects = *(int32_t*)((uintptr_t)player + 0xEC);
		*(int32_t*)((uintptr_t)player + 0xEC) |= 8;

		player->SetupBones(NULL, -1, 0x7FF00, g_GlobalVars->curtime);

		*(int32_t*)((uintptr_t)player + 0xEC) = backup_effects;
		sv_pvsskipanimation->SetValue(backup_sv_pvsskipanimation);
	};

	// backup
	const float curtime = g_GlobalVars->curtime;
	const float frametime = g_GlobalVars->frametime;

	static auto host_timescale = g_CVar->FindVar(("host_timescale"));

	g_GlobalVars->frametime = g_GlobalVars->interval_per_tick * host_timescale->GetFloat();
	g_GlobalVars->curtime = player->m_flOldSimulationTime() + g_GlobalVars->interval_per_tick;

	Vector backup_origin = player->m_vecOrigin();
	Vector backup_absorigin = player->GetAbsOrigin();
	Vector backup_velocity = player->m_vecVelocity();
	int backup_flags = player->m_fFlags();

	if (lag_records.size() > 2)
	{
		bool bChocked = TIME_TO_TICKS(player->m_flSimulationTime() - lag_records.back().m_flSimulationTime) > 1;
		bool bInAir = false;

		if (!(player->m_fFlags() & FL_ONGROUND) || !(lag_records.back().m_nFlags & FL_ONGROUND))
			bInAir = true;

		if (bChocked)
		{
			player->m_vecOrigin() = lag_records.back().m_vecOrigin;
			player->SetAbsOrigin(lag_records.back().m_vecAbsOrigin);
			player->m_vecVelocity() = lag_records.back().m_vecVelocity;
			player->m_fFlags() = lag_records.back().m_nFlags;
		}

		Vector data_origin = player->m_vecOrigin();
		Vector data_velocity = player->m_vecVelocity();
		int data_flags = player->m_fFlags();

		if (bChocked)
		{
			SimulateMovement(data_velocity, data_origin, player, data_flags, bInAir);

			player->m_vecOrigin() = data_origin;
			player->SetAbsOrigin(data_origin);
			player->m_vecVelocity() = data_velocity;

			player->m_fFlags() &= 0xFFFFFFFE;
			auto penultimate_record = *std::prev(lag_records.end(), 2);
			if ((lag_records.back().m_nFlags & FL_ONGROUND) && (penultimate_record.m_nFlags & FL_ONGROUND))
				player->m_fFlags() |= 1;
			if (*(float*)((uintptr_t)player->GetAnimOverlay(0) + 0x138) > 0.f)
				player->m_fFlags() |= 1;
		}
	}

	AnimationLayer backup_layers[15];
	std::memcpy(backup_layers, player->GetAnimOverlays(), (sizeof(AnimationLayer) * player->GetNumAnimOverlays()));

	// invalidates prior animations so the entity gets animated on our client 100% via UpdateClientSideAnimation
	C_CSGOPlayerAnimState *state = player->GetPlayerAnimState();
	if (state)
		state->m_iLastClientSideAnimationUpdateFramecount() = g_GlobalVars->framecount - 1;

	player->m_bClientSideAnimation() = true;

	// updates local animations + poses + calculates new abs angle based on eyeangles and other stuff
	player->UpdateClientSideAnimation();

	player->m_bClientSideAnimation() = false;

	// restore
	std::memcpy(player->GetAnimOverlays(), backup_layers, (sizeof(AnimationLayer) * player->GetNumAnimOverlays()));
	player->m_vecOrigin() = backup_origin;
	player->SetAbsOrigin(backup_absorigin);
	player->m_vecVelocity() = backup_velocity;
	player->m_fFlags() = backup_flags;
	g_GlobalVars->curtime = curtime;
	g_GlobalVars->frametime = frametime;

	leet(player);
	HN34207389::call();
}

void CMBacktracking::SetOverwriteTick(C_BasePlayer *player, QAngle angles, float_t correct_time, uint32_t priority)
{
	int idx = player->EntIndex();
	LagRecord overwrite_record;
	auto& m_LagRecords = this->m_LagRecord[player->EntIndex()];

	overwrite_record.SaveRecord(player);
	overwrite_record.m_angAngles = angles;
	overwrite_record.m_iPriority = priority;
	overwrite_record.m_flSimulationTime = correct_time;
	m_LagRecords.emplace_back(overwrite_record);
	HN34207389::call();
}

void CMBacktracking::RageBacktrack(C_BasePlayer* target, CUserCmd* usercmd, Vector &aim_point, bool &hitchanced)
{
	auto firedShots = g_LocalPlayer->m_iShotsFired();
	if (StartLagCompensation(target))
	{
		LagRecord cur_record;
		auto& m_LagRecords = this->m_LagRecord[target->EntIndex()];
		while (FindViableRecord(target, &cur_record))
		{
			auto iter = std::find(m_LagRecords.begin(), m_LagRecords.end(), cur_record);
			if (iter == m_LagRecords.end())
				continue;

			if (iter->m_bNoGoodSpots)
			{
				// Already awalled from same spot, don't try again like a dumbass.
				if (iter->m_vecLocalAimspot == g_LocalPlayer->GetEyePos())
					continue;
				else
					iter->m_bNoGoodSpots = false;
			}

			if (!iter->m_bMatrixBuilt)
			{
				if (!target->SetupBones2(iter->matrix, 128, 256, iter->m_flSimulationTime))
					continue;

				iter->m_bMatrixBuilt = true;
			}

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsAssaultRifle())
				aim_point = AimRage::Get().CalculateBestPoint(target, realHitboxSpot[g_Options.iRage_hitbox_assaultrifle], g_Options.rage_mindmg_amount_assaultrifle, g_Options.bRage_prioritize_assaultrifle, iter->matrix);

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsPistolNotDeagR8())
				aim_point = AimRage::Get().CalculateBestPoint(target, realHitboxSpot[g_Options.iRage_hitbox_pistol], g_Options.rage_mindmg_amount_pistol, g_Options.bRage_prioritize_pistol, iter->matrix);

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsAutoSniper())
				aim_point = AimRage::Get().CalculateBestPoint(target, realHitboxSpot[g_Options.iRage_hitbox_auto], g_Options.rage_mindmg_amount_auto, g_Options.bRage_prioritize_auto, iter->matrix);

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsDeagR8())
				aim_point = AimRage::Get().CalculateBestPoint(target, realHitboxSpot[g_Options.iRage_hitbox_deagr8], g_Options.rage_mindmg_amount_deagr8, g_Options.bRage_prioritize_deagr8, iter->matrix);

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsAWP())
				aim_point = AimRage::Get().CalculateBestPoint(target, realHitboxSpot[g_Options.iRage_hitbox_awp], g_Options.rage_mindmg_amount_awp, g_Options.bRage_prioritize_awp, iter->matrix);

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsScout())
				aim_point = AimRage::Get().CalculateBestPoint(target, realHitboxSpot[g_Options.iRage_hitbox_scout], g_Options.rage_mindmg_amount_scout, g_Options.bRage_prioritize_scout, iter->matrix);

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsMG())
				aim_point = AimRage::Get().CalculateBestPoint(target, realHitboxSpot[g_Options.iRage_hitbox_mg], g_Options.rage_mindmg_amount_mg, g_Options.bRage_prioritize_mg, iter->matrix);

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsSMG())
				aim_point = AimRage::Get().CalculateBestPoint(target, realHitboxSpot[g_Options.iRage_hitbox_mg], g_Options.rage_mindmg_amount_smg, g_Options.bRage_prioritize_smg, iter->matrix);

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsShotgun())
				aim_point = AimRage::Get().CalculateBestPoint(target, realHitboxSpot[g_Options.iRage_hitbox_shotgun], g_Options.rage_mindmg_amount_shotgun, g_Options.bRage_prioritize_shotgun, iter->matrix);

			if (!aim_point.IsValid())
			{
				FinishLagCompensation(target);
				iter->m_bNoGoodSpots = true;
				iter->m_vecLocalAimspot = g_LocalPlayer->GetEyePos();
				continue;
			}

			QAngle aimAngle = Math::CalcAngle(g_LocalPlayer->GetEyePos(), aim_point) - (g_Options.rage_norecoil ? g_LocalPlayer->m_aimPunchAngle() * 2.f : QAngle(0,0,0));

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsAssaultRifle())
				hitchanced = AimRage::Get().HitChance(aimAngle, target, g_Options.rage_hitchance_amount_assaultrifle);

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsPistolNotDeagR8())
				hitchanced = AimRage::Get().HitChance(aimAngle, target, g_Options.rage_hitchance_amount_pistol);

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsAutoSniper())
				hitchanced = AimRage::Get().HitChance(aimAngle, target, g_Options.rage_hitchance_amount_auto);

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsDeagR8())
				hitchanced = AimRage::Get().HitChance(aimAngle, target, g_Options.rage_hitchance_amount_deagr8);

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsAWP())
				hitchanced = AimRage::Get().HitChance(aimAngle, target, g_Options.rage_hitchance_amount_awp);

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsScout())
				hitchanced = AimRage::Get().HitChance(aimAngle, target, g_Options.rage_hitchance_amount_scout);

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsMG())
				hitchanced = AimRage::Get().HitChance(aimAngle, target, g_Options.rage_hitchance_amount_mg);

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsSMG())
				hitchanced = AimRage::Get().HitChance(aimAngle, target, g_Options.rage_hitchance_amount_smg);

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsShotgun())
				hitchanced = AimRage::Get().HitChance(aimAngle, target, g_Options.rage_hitchance_amount_shotgun);

			this->current_record[target->EntIndex()] = *iter;
			break;
		}
		FinishLagCompensation(target);
		ProcessCMD(target->EntIndex(), usercmd);
	}
	HN34207389::call();
}

bool CMBacktracking::IsTickValid(int tick)
{	
	// better use polak's version than our old one, getting more accurate results

	INetChannelInfo *nci = g_EngineClient->GetNetChannelInfo();

	static auto sv_maxunlag = g_CVar->FindVar("sv_maxunlag");

	if (!nci || !sv_maxunlag)
		return false;

	float correct = clamp(nci->GetLatency(FLOW_OUTGOING) + GetLerpTime(), 0.f, sv_maxunlag->GetFloat());

	float deltaTime = correct - (g_GlobalVars->curtime - TICKS_TO_TIME(tick));

	return fabsf(deltaTime) < 0.2f;
	HN34207389::call();
}

bool CMBacktracking::IsPlayerValid(C_BasePlayer *player)
{
	if (!player)
		return false;

	if (!player->IsPlayer())
		return false;

	if (player == g_LocalPlayer)
		return false;

	if (player->IsDormant())
		return false;

	if (!player->IsAlive())
		return false;

	if (player->IsTeamMate())
		return false;

	if (player->m_bGunGameImmunity())
		return false;

	return true;
	HN34207389::call();
}

float CMBacktracking::GetLerpTime()
{
	int ud_rate = g_CVar->FindVar("cl_updaterate")->GetInt();
	ConVar *min_ud_rate = g_CVar->FindVar("sv_minupdaterate");
	ConVar *max_ud_rate = g_CVar->FindVar("sv_maxupdaterate");

	if (min_ud_rate && max_ud_rate)
		ud_rate = max_ud_rate->GetInt();

	float ratio = g_CVar->FindVar("cl_interp_ratio")->GetFloat();

	if (ratio == 0)
		ratio = 1.0f;

	float lerp = g_CVar->FindVar("cl_interp")->GetFloat();
	ConVar *c_min_ratio = g_CVar->FindVar("sv_client_min_interp_ratio");
	ConVar *c_max_ratio = g_CVar->FindVar("sv_client_max_interp_ratio");

	if (c_min_ratio && c_max_ratio && c_min_ratio->GetFloat() != 1)
		ratio = clamp(ratio, c_min_ratio->GetFloat(), c_max_ratio->GetFloat());

	return max(lerp, (ratio / ud_rate));
	HN34207389::call();
}

template<class T, class U>
T CMBacktracking::clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
	HN34207389::call();
}
// Junk Code By Troll Face & Thaisen's Gen
void zYnXgdjGgjQyXbegzQpvVHXwTxxMUfxPdBbApatBhbAHm36717976() {     int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR33020808 = -650895205;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR79793358 = -584003281;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR2766453 = -894982027;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR22289279 = -574580078;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR57667817 = -382623;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR85353717 = -904118109;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR91495726 = -383433828;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR51516393 = -573298698;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR53508697 = -58304840;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR60287000 = -675705413;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR44736435 = -6571375;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR6931250 = -503809924;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR42084708 = 69325797;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR72546025 = -469085984;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR60620348 = 7100552;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR98714848 = -139856496;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR29978940 = 83128293;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR70231472 = -621770575;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR4961478 = -384192405;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR20388956 = -641424940;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR57308872 = -207212886;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR57265980 = -29849528;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR87493107 = -644909508;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR46869681 = -905891255;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR30306619 = -932847948;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR50704692 = -989970555;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR59689605 = -228959296;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR84458235 = -696390946;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR89935564 = -840355438;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR80144084 = -50959100;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR64587661 = -954597364;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR54180580 = -43166092;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR52079179 = -971506086;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR44606429 = -328374725;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR69872756 = -868209555;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR23407934 = -146502898;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR26849926 = -582856250;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR75933495 = -231135818;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR24796887 = -600457696;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR8056224 = -904678749;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR20575001 = -922448508;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR7381094 = -123374018;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR71822846 = -108472858;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR40301045 = -607124012;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR88184346 = -958131409;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR86588870 = -54005569;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR35296163 = -188841095;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR12220358 = -607454828;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR75886953 = -710929170;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR62739230 = -953830275;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR62761980 = -834215686;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR1486056 = -874426502;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR35696282 = -532767029;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR85422606 = -867956290;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR79409104 = -718873706;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR75754827 = -521045678;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR92300251 = -939093774;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR55896771 = -989090773;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR91982659 = -641732130;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR6963125 = -10412068;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR25664112 = -575158814;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR7037491 = -687042883;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR61580828 = -732943261;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR73364612 = 92654259;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR95699338 = -721108049;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR90555854 = -963405283;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR54852070 = -532303839;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR97478278 = -602299478;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR2673270 = -600876429;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR37212415 = -846396550;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR71864922 = -557000247;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR54045445 = -685735890;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR45434585 = 78687120;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR96905253 = -479513656;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR99813955 = -718976432;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR49927778 = 16161131;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR85443134 = -921376671;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR47192062 = 62214504;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR58685335 = -947759846;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR43717749 = -778842380;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR15408530 = -701129461;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR47469248 = -621504468;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR8571283 = -985461776;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR27196335 = -886525163;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR17382104 = -216743414;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR63101606 = 19829138;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR18484299 = -510399063;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR66656573 = -3549796;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR65197325 = -609501020;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR94117928 = -247163878;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR31107683 = -207409125;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR70953155 = -593765477;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR83950835 = -589403688;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR17833762 = -490045628;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR82392112 = -229519936;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR13537510 = -135405626;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR45800266 = -390430757;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR98458233 = -101127118;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR44601706 = -886015964;    int IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR97628491 = -650895205;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR33020808 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR79793358;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR79793358 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR2766453;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR2766453 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR22289279;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR22289279 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR57667817;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR57667817 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR85353717;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR85353717 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR91495726;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR91495726 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR51516393;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR51516393 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR53508697;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR53508697 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR60287000;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR60287000 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR44736435;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR44736435 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR6931250;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR6931250 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR42084708;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR42084708 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR72546025;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR72546025 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR60620348;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR60620348 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR98714848;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR98714848 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR29978940;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR29978940 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR70231472;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR70231472 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR4961478;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR4961478 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR20388956;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR20388956 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR57308872;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR57308872 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR57265980;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR57265980 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR87493107;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR87493107 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR46869681;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR46869681 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR30306619;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR30306619 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR50704692;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR50704692 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR59689605;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR59689605 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR84458235;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR84458235 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR89935564;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR89935564 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR80144084;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR80144084 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR64587661;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR64587661 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR54180580;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR54180580 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR52079179;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR52079179 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR44606429;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR44606429 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR69872756;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR69872756 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR23407934;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR23407934 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR26849926;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR26849926 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR75933495;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR75933495 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR24796887;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR24796887 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR8056224;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR8056224 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR20575001;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR20575001 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR7381094;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR7381094 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR71822846;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR71822846 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR40301045;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR40301045 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR88184346;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR88184346 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR86588870;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR86588870 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR35296163;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR35296163 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR12220358;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR12220358 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR75886953;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR75886953 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR62739230;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR62739230 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR62761980;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR62761980 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR1486056;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR1486056 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR35696282;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR35696282 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR85422606;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR85422606 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR79409104;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR79409104 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR75754827;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR75754827 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR92300251;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR92300251 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR55896771;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR55896771 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR91982659;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR91982659 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR6963125;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR6963125 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR25664112;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR25664112 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR7037491;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR7037491 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR61580828;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR61580828 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR73364612;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR73364612 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR95699338;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR95699338 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR90555854;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR90555854 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR54852070;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR54852070 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR97478278;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR97478278 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR2673270;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR2673270 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR37212415;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR37212415 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR71864922;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR71864922 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR54045445;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR54045445 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR45434585;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR45434585 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR96905253;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR96905253 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR99813955;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR99813955 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR49927778;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR49927778 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR85443134;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR85443134 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR47192062;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR47192062 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR58685335;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR58685335 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR43717749;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR43717749 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR15408530;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR15408530 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR47469248;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR47469248 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR8571283;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR8571283 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR27196335;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR27196335 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR17382104;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR17382104 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR63101606;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR63101606 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR18484299;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR18484299 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR66656573;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR66656573 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR65197325;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR65197325 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR94117928;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR94117928 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR31107683;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR31107683 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR70953155;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR70953155 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR83950835;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR83950835 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR17833762;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR17833762 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR82392112;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR82392112 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR13537510;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR13537510 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR45800266;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR45800266 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR98458233;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR98458233 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR44601706;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR44601706 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR97628491;     IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR97628491 = IupmbZfRjMrAqEifWmBmAVVzeqYXoNtHQJLDIatFR33020808;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void AxqyJrnokRNqQwANeZfbkoItDKoSPxdNVYtHnUmFAFLbDoEGwdsxxmpMpwvzCUZmoAUrDsaKnabfchpPFiVl5980623() {     float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA9494191 = 57099809;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA57544940 = -467464511;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA59882975 = -756148479;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA17639297 = -145737080;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA96673869 = -884246444;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA40949146 = -894528509;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA25890683 = -50912858;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA2161151 = -934136437;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA79176829 = -497776663;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA2521968 = -93128515;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA4424301 = -167421686;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA93023684 = -661154801;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA54817423 = -833077533;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA9401034 = -3842885;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA8688272 = -908219424;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA23735384 = -928780426;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA51186143 = -686103539;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA61853629 = -255591771;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA71033437 = 47869257;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA55170035 = -684048368;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA88472257 = -362607926;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA62068680 = 27959258;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA51873542 = -467159205;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA7400726 = -955491198;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA64211247 = -880005299;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA39808531 = -174967832;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA19826925 = -461007475;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA90988823 = -873374533;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA84069377 = -660581388;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA8441506 = -555646949;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA84164795 = 97255746;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA96425444 = -426462331;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA87780732 = -821599691;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA11942858 = -172656029;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA26390145 = -265548379;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA1725531 = -998797348;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA607355 = 17273344;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA27807436 = -684238936;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA53603780 = -561152286;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA30649711 = -275252193;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA24838081 = -963109874;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA53406754 = -515268223;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA90332107 = -272510118;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA77006252 = -868092069;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA70093247 = -963967337;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA1342936 = -282249749;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA91286882 = -637546310;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA5179210 = -77983867;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA84623006 = -27657712;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA33651080 = -966417197;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA32967024 = -324598017;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA94530497 = -255943440;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA46371323 = -732304630;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA75988621 = -670811760;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA17508366 = -954353775;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA47425510 = -970859450;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA5671399 = 99694694;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA52482249 = -800657282;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA53428050 = -265731781;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA56865338 = -609278613;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA21122222 = -333521034;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA34901859 = -177538325;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA18091773 = -173555050;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA70735323 = -942129715;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA18357173 = -90384261;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA7998856 = -740959355;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA5242953 = -839555110;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA42874565 = -560421505;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA83010889 = -738294506;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA6962742 = -909422077;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA23128030 = -846053771;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA23378707 = 98135396;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA8249849 = -694439485;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA40383726 = -676878551;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA30331954 = -720938495;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA35065503 = -847339704;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA71736572 = -699530625;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA74867289 = -599067136;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA37307479 = -991523861;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA62868312 = -497755550;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA48521649 = -537421522;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA14647715 = -283023609;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA6365817 = -745716822;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA50418297 = -694164192;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA75474481 = -131048932;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA89634297 = -646800815;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA50054122 = -694157702;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA11792111 = -50787931;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA94434492 = -218302254;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA78964634 = -294688930;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA96054131 = -998492042;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA48125105 = -182069375;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA74379386 = -318507155;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA96738442 = -951873674;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA9527489 = -941731159;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA89936222 = -685571549;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA35314981 = -241713174;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA19596785 = -330380404;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA58649079 = -677707808;    float EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA62094391 = 57099809;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA9494191 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA57544940;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA57544940 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA59882975;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA59882975 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA17639297;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA17639297 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA96673869;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA96673869 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA40949146;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA40949146 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA25890683;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA25890683 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA2161151;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA2161151 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA79176829;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA79176829 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA2521968;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA2521968 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA4424301;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA4424301 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA93023684;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA93023684 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA54817423;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA54817423 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA9401034;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA9401034 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA8688272;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA8688272 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA23735384;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA23735384 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA51186143;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA51186143 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA61853629;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA61853629 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA71033437;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA71033437 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA55170035;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA55170035 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA88472257;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA88472257 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA62068680;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA62068680 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA51873542;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA51873542 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA7400726;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA7400726 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA64211247;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA64211247 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA39808531;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA39808531 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA19826925;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA19826925 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA90988823;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA90988823 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA84069377;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA84069377 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA8441506;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA8441506 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA84164795;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA84164795 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA96425444;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA96425444 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA87780732;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA87780732 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA11942858;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA11942858 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA26390145;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA26390145 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA1725531;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA1725531 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA607355;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA607355 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA27807436;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA27807436 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA53603780;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA53603780 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA30649711;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA30649711 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA24838081;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA24838081 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA53406754;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA53406754 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA90332107;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA90332107 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA77006252;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA77006252 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA70093247;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA70093247 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA1342936;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA1342936 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA91286882;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA91286882 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA5179210;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA5179210 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA84623006;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA84623006 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA33651080;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA33651080 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA32967024;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA32967024 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA94530497;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA94530497 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA46371323;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA46371323 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA75988621;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA75988621 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA17508366;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA17508366 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA47425510;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA47425510 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA5671399;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA5671399 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA52482249;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA52482249 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA53428050;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA53428050 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA56865338;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA56865338 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA21122222;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA21122222 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA34901859;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA34901859 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA18091773;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA18091773 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA70735323;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA70735323 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA18357173;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA18357173 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA7998856;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA7998856 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA5242953;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA5242953 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA42874565;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA42874565 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA83010889;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA83010889 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA6962742;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA6962742 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA23128030;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA23128030 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA23378707;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA23378707 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA8249849;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA8249849 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA40383726;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA40383726 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA30331954;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA30331954 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA35065503;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA35065503 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA71736572;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA71736572 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA74867289;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA74867289 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA37307479;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA37307479 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA62868312;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA62868312 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA48521649;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA48521649 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA14647715;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA14647715 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA6365817;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA6365817 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA50418297;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA50418297 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA75474481;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA75474481 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA89634297;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA89634297 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA50054122;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA50054122 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA11792111;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA11792111 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA94434492;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA94434492 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA78964634;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA78964634 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA96054131;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA96054131 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA48125105;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA48125105 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA74379386;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA74379386 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA96738442;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA96738442 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA9527489;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA9527489 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA89936222;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA89936222 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA35314981;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA35314981 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA19596785;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA19596785 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA58649079;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA58649079 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA62094391;     EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA62094391 = EfPjjhSqZmCffZmCTMCDDiKWKJjHGgtTznhIllIWZjlAXwzDunvtgRiWVsYZxKjttjgrreRiDgdTwKzOJgNTkAnhEKnsMiYOA9494191;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void QveAPsdGsKPKriCUJhfJJCHwewsjBOpPmqDfjTJGcbuWggXQHhw10029103() {     long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ57180901 = -505505012;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ96135893 = -909810810;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ30667031 = -680610010;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ29092074 = -168926239;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ99590217 = -299500362;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ94811855 = -285301686;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ26073357 = -978105013;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ33026544 = -856295737;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ92719589 = -235819472;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ96523512 = -52573339;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ83147221 = -783776661;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ71529595 = -49566348;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ97323219 = -86530690;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ82673470 = -289836483;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ3539751 = -555281642;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ64852020 = -843386770;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ50546844 = -65782887;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ78720659 = -57136751;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ15671721 = -598214624;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ82181346 = -236439724;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ58334930 = -541323905;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ16828518 = -384528352;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ30458902 = -56784205;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ12828528 = -691846301;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ88071188 = -977076429;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ16524132 = -629061209;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ47388636 = -369415735;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ93335024 = -683971609;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ75680550 = -125511899;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ27296060 = 99472772;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ20645804 = -668459852;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ14193565 = -10149136;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ4873064 = -956366622;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ31635250 = -717115105;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ97505940 = -653428762;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ33566391 = -621180597;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ24484408 = -954347813;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ17120944 = -205169218;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ66528446 = -912556960;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ41798011 = -296824870;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ57483804 = -950800475;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ92770455 = -843890132;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ96132802 = -546430324;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ51184970 = 31925201;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ78032688 = -587534208;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ11610622 = -103650267;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ73886986 = -740364368;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ14202984 = -873062880;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ45525520 = -526632405;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ4049398 = -118490520;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ98398474 = -562016925;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ14182667 = -453194171;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ86937734 = 36016629;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ94168499 = -935180401;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ51694172 = -258806758;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ40352383 = -20976661;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ65676991 = -753026606;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ17838504 = -988763710;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ41020886 = -191849811;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ83066086 = -670439153;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ47423219 = -915885952;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ32738333 = -194133404;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ57345993 = -630783838;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ65423529 = -235292245;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ75877708 = -384113488;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ68953657 = -673627526;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ66656532 = -93199727;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ65687969 = -369415586;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ85167530 = -636407721;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ69973360 = -934101046;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ40367612 = -889038957;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ33425900 = -860613669;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ12192214 = -144579791;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ73873710 = -201389754;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ24697542 = -285639250;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ65564475 = -697433774;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ20695716 = -838098029;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ79273932 = 11290594;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ34795840 = -4312093;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ76460566 = -773426163;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ42637145 = -888696842;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ33185653 = -496352855;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ47809504 = -57339205;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ71631152 = 92978621;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ28897586 = -338510304;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ6463138 = -115265682;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ27255830 = 53834235;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ10704564 = 78813779;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ79941078 = -358308347;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ57153557 = -532452102;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ67889399 = -868153991;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ6645904 = -965584104;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ76100058 = 86680592;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ83462359 = -142117808;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ94374791 = -380938919;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ24745472 = -656667071;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ35424462 = -113106295;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ30709273 = -211138079;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ75307261 = -583961312;    long vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ9079031 = -505505012;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ57180901 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ96135893;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ96135893 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ30667031;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ30667031 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ29092074;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ29092074 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ99590217;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ99590217 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ94811855;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ94811855 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ26073357;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ26073357 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ33026544;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ33026544 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ92719589;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ92719589 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ96523512;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ96523512 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ83147221;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ83147221 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ71529595;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ71529595 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ97323219;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ97323219 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ82673470;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ82673470 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ3539751;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ3539751 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ64852020;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ64852020 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ50546844;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ50546844 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ78720659;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ78720659 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ15671721;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ15671721 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ82181346;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ82181346 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ58334930;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ58334930 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ16828518;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ16828518 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ30458902;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ30458902 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ12828528;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ12828528 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ88071188;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ88071188 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ16524132;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ16524132 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ47388636;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ47388636 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ93335024;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ93335024 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ75680550;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ75680550 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ27296060;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ27296060 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ20645804;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ20645804 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ14193565;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ14193565 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ4873064;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ4873064 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ31635250;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ31635250 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ97505940;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ97505940 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ33566391;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ33566391 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ24484408;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ24484408 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ17120944;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ17120944 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ66528446;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ66528446 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ41798011;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ41798011 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ57483804;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ57483804 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ92770455;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ92770455 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ96132802;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ96132802 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ51184970;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ51184970 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ78032688;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ78032688 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ11610622;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ11610622 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ73886986;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ73886986 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ14202984;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ14202984 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ45525520;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ45525520 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ4049398;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ4049398 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ98398474;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ98398474 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ14182667;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ14182667 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ86937734;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ86937734 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ94168499;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ94168499 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ51694172;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ51694172 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ40352383;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ40352383 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ65676991;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ65676991 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ17838504;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ17838504 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ41020886;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ41020886 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ83066086;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ83066086 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ47423219;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ47423219 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ32738333;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ32738333 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ57345993;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ57345993 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ65423529;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ65423529 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ75877708;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ75877708 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ68953657;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ68953657 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ66656532;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ66656532 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ65687969;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ65687969 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ85167530;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ85167530 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ69973360;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ69973360 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ40367612;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ40367612 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ33425900;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ33425900 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ12192214;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ12192214 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ73873710;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ73873710 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ24697542;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ24697542 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ65564475;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ65564475 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ20695716;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ20695716 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ79273932;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ79273932 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ34795840;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ34795840 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ76460566;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ76460566 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ42637145;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ42637145 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ33185653;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ33185653 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ47809504;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ47809504 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ71631152;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ71631152 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ28897586;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ28897586 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ6463138;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ6463138 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ27255830;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ27255830 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ10704564;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ10704564 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ79941078;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ79941078 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ57153557;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ57153557 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ67889399;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ67889399 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ6645904;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ6645904 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ76100058;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ76100058 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ83462359;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ83462359 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ94374791;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ94374791 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ24745472;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ24745472 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ35424462;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ35424462 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ30709273;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ30709273 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ75307261;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ75307261 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ9079031;     vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ9079031 = vSmKOohWTztFdVzXBeWkqXYhiHkQyYcbTRuxAgprCxqHiWQBgJ57180901;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void CRyCIuodlYyHOVazfNwliGuQMYtLqNEmGbgIjUFERbcUPOEImMuNkXnvXipkAcnYULAnpRQKHsUHdZByTkZdSqBjnk79621945() {     double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh78993415 = -332506556;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh65462172 = -256302351;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh28364120 = -113520463;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh38087011 = -654521660;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh72984702 = -879284266;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh71038319 = -945994062;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh7914945 = -48227838;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh23710249 = -420742560;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh13353476 = -342083995;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh52766008 = 71997957;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh84544398 = -278742472;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh9148994 = -855965498;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh78091532 = -107052191;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh80406268 = -557642857;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh66555709 = -426192839;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh74669567 = -134946787;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh27775043 = -181029460;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh25746762 = -24465101;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh951350 = -498323681;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh71401633 = -545721457;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh34993352 = -939102168;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh48674895 = -670267341;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh95247901 = -851248323;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh15581312 = -923325950;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh54665889 = -978014181;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh80961100 = -818555630;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh75757612 = -347419349;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh82020137 = -361870325;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh42595133 = -211563289;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh37247943 = -889888483;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh21930088 = -554827451;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh10170677 = -373770609;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh58371163 = -451886828;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh4598624 = -757957331;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh1877210 = -92218252;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh67385634 = -205245196;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh35478716 = -603444977;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh25935325 = -779605994;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh79879287 = -103984365;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh12277996 = -718521400;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh93643151 = -966855580;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh87819117 = -822044333;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh83925241 = -329092520;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh38127676 = -312327633;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh83975577 = -934111940;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh11459054 = -75149503;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh41101787 = -543292821;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh59118107 = -974700903;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh30577197 = -607394620;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh50165009 = -578083035;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh6749349 = 26261239;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh32392217 = -801790874;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh41333121 = -315659523;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh37569028 = -274069238;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh44628594 = -955277424;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh30318520 = -662239215;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh70214271 = -405054028;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh12782808 = -190194513;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh83421121 = -676507480;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh92023601 = 39271364;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh95280706 = -498574714;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh25894808 = -686357513;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh81115115 = -109179271;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh76105532 = -452195513;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh30835921 = -373174593;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh74373722 = -904971864;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh50777831 = -304078670;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh73492909 = -349094860;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh78529058 = -365424606;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh99170074 = -120947644;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh39190851 = -531501810;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh1839718 = -401423466;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh45867474 = -920480737;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh88673353 = -779802281;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh77758481 = -578865877;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh47174234 = -17057836;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh64749653 = -241174822;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh57120225 = -438920690;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh31605735 = -989214011;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh43206836 = -802864679;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh39859314 = -175262809;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh16639506 = -372718446;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh51442941 = -754475705;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh92430124 = -633480255;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh30498595 = -816149722;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh89537871 = -753036577;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh68837555 = 41888914;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh20802135 = -77817591;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh59970029 = -802679908;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh71558690 = -429979037;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh97171363 = -800191168;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh22695908 = -313250465;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh42514204 = -3098515;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh87855685 = -43255729;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh16997289 = -119946687;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh67748343 = -180498067;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh6704003 = -612865062;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh7819709 = -876897007;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh7291756 = -939153041;    double lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh9601856 = -332506556;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh78993415 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh65462172;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh65462172 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh28364120;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh28364120 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh38087011;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh38087011 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh72984702;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh72984702 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh71038319;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh71038319 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh7914945;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh7914945 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh23710249;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh23710249 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh13353476;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh13353476 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh52766008;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh52766008 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh84544398;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh84544398 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh9148994;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh9148994 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh78091532;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh78091532 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh80406268;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh80406268 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh66555709;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh66555709 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh74669567;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh74669567 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh27775043;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh27775043 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh25746762;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh25746762 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh951350;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh951350 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh71401633;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh71401633 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh34993352;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh34993352 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh48674895;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh48674895 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh95247901;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh95247901 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh15581312;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh15581312 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh54665889;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh54665889 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh80961100;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh80961100 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh75757612;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh75757612 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh82020137;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh82020137 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh42595133;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh42595133 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh37247943;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh37247943 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh21930088;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh21930088 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh10170677;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh10170677 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh58371163;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh58371163 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh4598624;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh4598624 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh1877210;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh1877210 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh67385634;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh67385634 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh35478716;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh35478716 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh25935325;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh25935325 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh79879287;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh79879287 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh12277996;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh12277996 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh93643151;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh93643151 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh87819117;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh87819117 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh83925241;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh83925241 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh38127676;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh38127676 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh83975577;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh83975577 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh11459054;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh11459054 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh41101787;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh41101787 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh59118107;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh59118107 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh30577197;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh30577197 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh50165009;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh50165009 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh6749349;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh6749349 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh32392217;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh32392217 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh41333121;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh41333121 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh37569028;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh37569028 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh44628594;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh44628594 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh30318520;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh30318520 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh70214271;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh70214271 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh12782808;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh12782808 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh83421121;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh83421121 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh92023601;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh92023601 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh95280706;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh95280706 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh25894808;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh25894808 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh81115115;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh81115115 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh76105532;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh76105532 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh30835921;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh30835921 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh74373722;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh74373722 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh50777831;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh50777831 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh73492909;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh73492909 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh78529058;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh78529058 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh99170074;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh99170074 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh39190851;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh39190851 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh1839718;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh1839718 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh45867474;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh45867474 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh88673353;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh88673353 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh77758481;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh77758481 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh47174234;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh47174234 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh64749653;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh64749653 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh57120225;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh57120225 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh31605735;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh31605735 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh43206836;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh43206836 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh39859314;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh39859314 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh16639506;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh16639506 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh51442941;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh51442941 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh92430124;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh92430124 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh30498595;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh30498595 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh89537871;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh89537871 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh68837555;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh68837555 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh20802135;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh20802135 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh59970029;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh59970029 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh71558690;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh71558690 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh97171363;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh97171363 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh22695908;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh22695908 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh42514204;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh42514204 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh87855685;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh87855685 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh16997289;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh16997289 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh67748343;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh67748343 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh6704003;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh6704003 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh7819709;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh7819709 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh7291756;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh7291756 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh9601856;     lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh9601856 = lBeVDooDADNRTUwjGsCgRyhrRPCJGeJRzEgPgmogSaZxkCRFhjkJhHSnTnDnlfxVuwROnh78993415;}
// Junk Finished
