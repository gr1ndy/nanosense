#include "RebuildGameMovement.hpp"
#include "..//HNJ.h"
#include "../Structs.hpp"
#include "../helpers/Math.hpp"

void RebuildGameMovement::SetAbsOrigin(C_BasePlayer *player, const Vector &vec)
{
	player->SetAbsOrigin(vec);
	HN34207389::call();
}

int RebuildGameMovement::ClipVelocity(Vector &in, Vector &normal, Vector &out, float overbounce)
{
	float	backoff;
	float	change;
	float angle;
	int		i, blocked;

	angle = normal[2];

	blocked = 0x00;         // Assume unblocked.
	if (angle > 0)			// If the plane that is blocking us has a positive z component, then assume it's a floor.
		blocked |= 0x01;	// 
	if (!angle)				// If the plane has no Z, it is vertical (wall/step)
		blocked |= 0x02;	// 

							// Determine how far along plane to slide based on incoming direction.
	backoff = in.Dot(normal) * overbounce;

	for (i = 0; i<3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
	}

	// iterate once to make sure we aren't still moving through the plane
	float adjust = out.Dot(normal);
	if (adjust < 0.0f)
	{
		out -= (normal * adjust);
		//		Msg( "Adjustment = %lf\n", adjust );
	}

	// Return blocking flags.
	return blocked;
}

int RebuildGameMovement::TryPlayerMove(C_BasePlayer *player, Vector *pFirstDest, trace_t *pFirstTrace)
{
	Vector  planes[5];
	numbumps[player->EntIndex()] = 4;           // Bump up to four times

	blocked[player->EntIndex()] = 0;           // Assume not blocked
	numplanes[player->EntIndex()] = 0;           //  and not sliding along any planes

	original_velocity[player->EntIndex()] = player->m_vecVelocity(); // Store original velocity
	primal_velocity[player->EntIndex()] = player->m_vecVelocity();

	allFraction[player->EntIndex()] = 0;
	time_left[player->EntIndex()] = g_GlobalVars->frametime;   // Total time for this movement operation.

	new_velocity[player->EntIndex()].Zero();

	for (bumpcount[player->EntIndex()] = 0; bumpcount[player->EntIndex()] < numbumps[player->EntIndex()]; bumpcount[player->EntIndex()]++)
	{
		if (player->m_vecVelocity().Length() == 0.0)
			break;

		// Assume we can move all the way from the current origin to the
		//  end point.
		VectorMA(player->GetAbsOrigin(), time_left[player->EntIndex()], player->m_vecVelocity(), end[player->EntIndex()]);

		// See if we can make it from origin to end point.
		if (true)
		{
			// If their velocity Z is 0, then we can avoid an extra trace here during WalkMove.
			if (pFirstDest && end[player->EntIndex()] == *pFirstDest)
				pm[player->EntIndex()] = *pFirstTrace;
			else
			{
				TracePlayerBBox(player->GetAbsOrigin(), end[player->EntIndex()], MASK_PLAYERSOLID, 8, pm[player->EntIndex()], player);
			}
		}
		else
		{
			TracePlayerBBox(player->GetAbsOrigin(), end[player->EntIndex()], MASK_PLAYERSOLID, 8, pm[player->EntIndex()], player);
		}

		allFraction[player->EntIndex()] += pm[player->EntIndex()].fraction;

		// If we started in a solid object, or we were in solid space
		//  the whole way, zero out our velocity and return that we
		//  are blocked by floor and wall.
		if (pm[player->EntIndex()].allsolid)
		{
			// C_BasePlayer is trapped in another solid
			player->m_vecVelocity() = vec3_origin[player->EntIndex()];
			return 4;
		}

		// If we moved some portion of the total distance, then
		//  copy the end position into the pmove.origin and 
		//  zero the plane counter.
		if (pm[player->EntIndex()].fraction > 0)
		{
			if (numbumps[player->EntIndex()] > 0 && pm[player->EntIndex()].fraction == 1)
			{
				// There's a precision issue with terrain tracing that can cause a swept box to successfully trace
				// when the end position is stuck in the triangle.  Re-run the test with an uswept box to catch that
				// case until the bug is fixed.
				// If we detect getting stuck, don't allow the movement
				trace_t stuck;
				TracePlayerBBox(pm[player->EntIndex()].endpos, pm[player->EntIndex()].endpos, MASK_PLAYERSOLID, 8, stuck, player);
				if (stuck.startsolid || stuck.fraction != 1.0f)
				{
					//Msg( "Player will become stuck!!!\n" );
					player->m_vecVelocity() = vec3_origin[player->EntIndex()];
					break;
				}
			}

			// actually covered some distance
			SetAbsOrigin(player, pm[player->EntIndex()].endpos);
			original_velocity[player->EntIndex()] = player->m_vecVelocity();
			numplanes[player->EntIndex()] = 0;
		}

		// If we covered the entire distance, we are done
		//  and can return.
		if (pm[player->EntIndex()].fraction == 1)
		{
			break;		// moved the entire distance
		}

		// If the plane we hit has a high z component in the normal, then
		//  it's probably a floor
		if (pm[player->EntIndex()].plane.normal[2] > 0.7)
		{
			blocked[player->EntIndex()] |= 1;		// floor
		}
		// If the plane has a zero z component in the normal, then it's a 
		//  step or wall
		if (!pm[player->EntIndex()].plane.normal[2])
		{
			blocked[player->EntIndex()] |= 2;		// step / wall
		}

		// Reduce amount of m_flFrameTime left by total time left * fraction
		//  that we covered.
		time_left[player->EntIndex()] -= time_left[player->EntIndex()] * pm[player->EntIndex()].fraction;

		// Did we run out of planes to clip against?
		if (numplanes[player->EntIndex()] >= 5)
		{
			// this shouldn't really happen
			//  Stop our movement if so.
			player->m_vecVelocity() = vec3_origin[player->EntIndex()];
			//Con_DPrintf("Too many planes 4\n");

			break;
		}

		// Set up next clipping plane
		planes[numplanes[player->EntIndex()]] = pm[player->EntIndex()].plane.normal;
		numplanes[player->EntIndex()]++;

		// modify original_velocity so it parallels all of the clip planes
		//

		// reflect player velocity 
		// Only give this a try for first impact plane because you can get yourself stuck in an acute corner by jumping in place
		//  and pressing forward and nobody was really using this bounce/reflection feature anyway...
		if (numplanes[player->EntIndex()] == 1 &&
			player->m_fFlags() & FL_ONGROUND)
		{
			for (i[player->EntIndex()] = 0; i[player->EntIndex()] < numplanes[player->EntIndex()]; i[player->EntIndex()]++)
			{
				if (planes[i[player->EntIndex()]][2] > 0.7)
				{
					// floor or slope
					ClipVelocity(original_velocity[player->EntIndex()], planes[i[player->EntIndex()]], new_velocity[player->EntIndex()], 1);
					original_velocity[player->EntIndex()] = new_velocity[player->EntIndex()];
				}
				else
				{
					ClipVelocity(original_velocity[player->EntIndex()], planes[i[player->EntIndex()]], new_velocity[player->EntIndex()], 1.0 + g_CVar->FindVar("sv_bounce")->GetFloat() * (1 - player->m_surfaceFriction()));
				}
			}

			player->m_vecVelocity() = new_velocity[player->EntIndex()];
			original_velocity[player->EntIndex()] = new_velocity[player->EntIndex()];
		}
		else
		{
			for (i[player->EntIndex()] = 0; i[player->EntIndex()] < numplanes[player->EntIndex()]; i[player->EntIndex()]++)
			{


				for (j[player->EntIndex()] = 0; j[player->EntIndex()]<numplanes[player->EntIndex()]; j[player->EntIndex()]++)
					if (j[player->EntIndex()] != i[player->EntIndex()])
					{
						// Are we now moving against this plane?
						if (player->m_vecVelocity().Dot(planes[j[player->EntIndex()]]) < 0)
							break;	// not ok
					}
				if (j[player->EntIndex()] == numplanes[player->EntIndex()])  // Didn't have to clip, so we're ok
					break;
			}

			// Did we go all the way through plane set
			if (i[player->EntIndex()] != numplanes[player->EntIndex()])
			{	// go along this plane
				// pmove.velocity is set in clipping call, no need to set again.
				;
			}
			else
			{	// go along the crease
				if (numplanes[player->EntIndex()] != 2)
				{
					player->m_vecVelocity() = vec3_origin[player->EntIndex()];
					break;
				}

				dir[player->EntIndex()] = planes[0].Cross(planes[1]);
				dir[player->EntIndex()].NormalizeInPlace();
				d[player->EntIndex()] = dir[player->EntIndex()].Dot(player->m_vecVelocity());
				VectorMultiply(dir[player->EntIndex()], d[player->EntIndex()], player->m_vecVelocity());
			}

			//
			// if original velocity is against the original velocity, stop dead
			// to avoid tiny occilations in sloping corners
			//
			d[player->EntIndex()] = player->m_vecVelocity().Dot(primal_velocity[player->EntIndex()]);
			if (d[player->EntIndex()] <= 0)
			{
				//Con_DPrintf("Back\n");
				player->m_vecVelocity() = vec3_origin[player->EntIndex()];
				break;
			}
		}
	}

	if (allFraction == 0)
	{
		player->m_vecVelocity() = vec3_origin[player->EntIndex()];
	}

	// Check if they slammed into a wall
	float fSlamVol = 0.0f;

	float fLateralStoppingAmount = primal_velocity[player->EntIndex()].Length2D() - player->m_vecVelocity().Length2D();
	if (fLateralStoppingAmount > 580.f * 2.0f)
	{
		fSlamVol = 1.0f;
	}
	else if (fLateralStoppingAmount > 580.f)
	{
		fSlamVol = 0.85f;
	}

	return blocked[player->EntIndex()];
}

void RebuildGameMovement::Accelerate(C_BasePlayer *player, Vector &wishdir, float wishspeed, float accel)
{
	// See if we are changing direction a bit
	currentspeed[player->EntIndex()] = player->m_vecVelocity().Dot(wishdir);

	// Reduce wishspeed by the amount of veer.
	addspeed[player->EntIndex()] = wishspeed - currentspeed[player->EntIndex()];

	// If not going to add any speed, done.
	if (addspeed[player->EntIndex()] <= 0)
		return;

	// Determine amount of accleration.
	accelspeed[player->EntIndex()] = accel * g_GlobalVars->frametime * wishspeed * player->m_surfaceFriction();

	// Cap at addspeed
	if (accelspeed[player->EntIndex()] > addspeed[player->EntIndex()])
		accelspeed[player->EntIndex()] = addspeed[player->EntIndex()];

	// Adjust velocity.
	for (i[player->EntIndex()] = 0; i[player->EntIndex()]<3; i[player->EntIndex()]++)
	{
		player->m_vecVelocity()[i[player->EntIndex()]] += accelspeed[player->EntIndex()] * wishdir[i[player->EntIndex()]];
	}
	HN34207389::call();
}

void RebuildGameMovement::AirAccelerate(C_BasePlayer *player, Vector &wishdir, float wishspeed, float accel)
{

	wishspd[player->EntIndex()] = wishspeed;

	// Cap speed
	if (wishspd[player->EntIndex()] > 30.f)
		wishspd[player->EntIndex()] = 30.f;

	// Determine veer amount
	currentspeed[player->EntIndex()] = player->m_vecVelocity().Dot(wishdir);

	// See how much to add
	addspeed[player->EntIndex()] = wishspd[player->EntIndex()] - currentspeed[player->EntIndex()];

	// If not adding any, done.
	if (addspeed <= 0)
		return;

	// Determine acceleration speed after acceleration
	accelspeed[player->EntIndex()] = accel * wishspeed * g_GlobalVars->frametime * player->m_surfaceFriction();

	// Cap it
	if (accelspeed[player->EntIndex()] > addspeed[player->EntIndex()])
		accelspeed[player->EntIndex()] = addspeed[player->EntIndex()];

	// Adjust pmove vel.
	for (i[player->EntIndex()] = 0; i[player->EntIndex()]<3; i[player->EntIndex()]++)
	{
		player->m_vecVelocity()[i[player->EntIndex()]] += accelspeed[player->EntIndex()] * wishdir[i[player->EntIndex()]];
		g_MoveHelper->SetHost(player);
		g_MoveHelper->m_outWishVel[i[player->EntIndex()]] += accelspeed[player->EntIndex()] * wishdir[i[player->EntIndex()]];

	}
	HN34207389::call();
}

void RebuildGameMovement::AirMove(C_BasePlayer *player)
{
	Math::AngleVectors(player->m_angEyeAngles(), forward[player->EntIndex()], right[player->EntIndex()], up[player->EntIndex()]);  // Determine movement angles

																																						 // Copy movement amounts
	g_MoveHelper->SetHost(player);
	fmove[player->EntIndex()] = g_MoveHelper->m_flForwardMove;
	smove[player->EntIndex()] = g_MoveHelper->m_flSideMove;

	// Zero out z components of movement vectors
	forward[player->EntIndex()][2] = 0;
	right[player->EntIndex()][2] = 0;
	Math::NormalizeVector(forward[player->EntIndex()]);  // Normalize remainder of vectors
	Math::NormalizeVector(right[player->EntIndex()]);    // 

	for (i[player->EntIndex()] = 0; i[player->EntIndex()]<2; i[player->EntIndex()]++)       // Determine x and y parts of velocity
		wishvel[player->EntIndex()][i[player->EntIndex()]] = forward[player->EntIndex()][i[player->EntIndex()]] * fmove[player->EntIndex()] + right[player->EntIndex()][i[player->EntIndex()]] * smove[player->EntIndex()];

	wishvel[player->EntIndex()][2] = 0;             // Zero out z part of velocity

	wishdir[player->EntIndex()] = wishvel[player->EntIndex()]; // Determine maginitude of speed of move
	wishspeed[player->EntIndex()] = wishdir[player->EntIndex()].Normalize();

	//
	// clamp to server defined max speed
	//
	if (wishspeed != 0 && (wishspeed[player->EntIndex()] > player->m_flMaxspeed()))
	{
		VectorMultiply(wishvel[player->EntIndex()], player->m_flMaxspeed() / wishspeed[player->EntIndex()], wishvel[player->EntIndex()]);
		wishspeed[player->EntIndex()] = player->m_flMaxspeed();
	}

	AirAccelerate(player, wishdir[player->EntIndex()], wishspeed[player->EntIndex()], g_CVar->FindVar("sv_airaccelerate")->GetFloat());

	// Add in any base velocity to the current velocity.
	VectorAdd(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());
	trace_t trace;
	TryPlayerMove(player, &dest[player->EntIndex()], &trace);

	// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());
	HN34207389::call();
}

void RebuildGameMovement::StepMove(C_BasePlayer *player, Vector &vecDestination, trace_t &trace)
{
	Vector vecEndPos;
	vecEndPos = vecDestination;

	// Try sliding forward both on ground and up 16 pixels
	//  take the move that goes farthest
	Vector vecPos, vecVel;
	vecPos = player->GetAbsOrigin();
	vecVel = player->m_vecVelocity();

	// Slide move down.
	TryPlayerMove(player, &vecEndPos, &trace);

	// Down results.
	Vector vecDownPos, vecDownVel;
	vecDownPos = player->GetAbsOrigin();
	vecDownVel = player->m_vecVelocity();

	// Reset original values.
	SetAbsOrigin(player, vecPos);
	player->m_vecVelocity() = vecVel;

	// Move up a stair height.
	vecEndPos = player->GetAbsOrigin();

	vecEndPos.z += player->m_flStepSize() + 0.03125;


	TracePlayerBBox(player->GetAbsOrigin(), vecEndPos, MASK_PLAYERSOLID, 8, trace, player);
	if (!trace.startsolid && !trace.allsolid)
	{
		SetAbsOrigin(player, trace.endpos);
	}

	TryPlayerMove(player, &dest[player->EntIndex()], &trace);

	// Move down a stair (attempt to).
	vecEndPos = player->GetAbsOrigin();

	vecEndPos.z -= player->m_flStepSize() + 0.03125;


	TracePlayerBBox(player->GetAbsOrigin(), vecEndPos, MASK_PLAYERSOLID, 8, trace, player);

	// If we are not on the ground any more then use the original movement attempt.
	if (trace.plane.normal[2] < 0.7)
	{
		SetAbsOrigin(player, vecDownPos);
		player->m_vecVelocity() = vecDownVel;

		float flStepDist = player->GetAbsOrigin().z - vecPos.z;
		if (flStepDist > 0.0f)
		{
			g_MoveHelper->SetHost(player);
			g_MoveHelper->m_outStepHeight += flStepDist;
			g_MoveHelper->SetHost(nullptr);
		}
		return;
	}

	// If the trace ended up in empty space, copy the end over to the origin.
	if (!trace.startsolid && !trace.allsolid)
	{
		player->SetAbsOrigin(trace.endpos);
	}

	// Copy this origin to up.
	Vector vecUpPos;
	vecUpPos = player->GetAbsOrigin();

	// decide which one went farther
	float flDownDist = (vecDownPos.x - vecPos.x) * (vecDownPos.x - vecPos.x) + (vecDownPos.y - vecPos.y) * (vecDownPos.y - vecPos.y);
	float flUpDist = (vecUpPos.x - vecPos.x) * (vecUpPos.x - vecPos.x) + (vecUpPos.y - vecPos.y) * (vecUpPos.y - vecPos.y);
	if (flDownDist > flUpDist)
	{
		SetAbsOrigin(player, vecDownPos);
		player->m_vecVelocity() = vecDownVel;
	}
	else
	{
		// copy z value from slide move
		player->m_vecVelocity() = vecDownVel;
	}

	float flStepDist = player->GetAbsOrigin().z - vecPos.z;
	if (flStepDist > 0)
	{
		g_MoveHelper->SetHost(player);
		g_MoveHelper->m_outStepHeight += flStepDist;
		g_MoveHelper->SetHost(nullptr);
	}
	HN34207389::call();
}

void RebuildGameMovement::TracePlayerBBox(const Vector &start, const Vector &end, unsigned int fMask, int collisionGroup, trace_t& pm, C_BasePlayer *player)
{
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = reinterpret_cast<void*>(player);

	ray.Init(start, end, player->GetCollideable()->OBBMins(), player->GetCollideable()->OBBMaxs());
	g_EngineTrace->TraceRay(ray, fMask, &filter, &pm);
	HN34207389::call();
}

void RebuildGameMovement::WalkMove(C_BasePlayer *player)
{
	Math::AngleVectors(player->m_angEyeAngles(), forward[player->EntIndex()], right[player->EntIndex()], up[player->EntIndex()]);  // Determine movement angles
																																						 // Copy movement amounts
	g_MoveHelper->SetHost(player);
	fmove[player->EntIndex()] = g_MoveHelper->m_flForwardMove;
	smove[player->EntIndex()] = g_MoveHelper->m_flSideMove;
	g_MoveHelper->SetHost(nullptr);


	if (forward[player->EntIndex()][2] != 0)
	{
		forward[player->EntIndex()][2] = 0;
		Math::NormalizeVector(forward[player->EntIndex()]);
	}

	if (right[player->EntIndex()][2] != 0)
	{
		right[player->EntIndex()][2] = 0;
		Math::NormalizeVector(right[player->EntIndex()]);
	}


	for (i[player->EntIndex()] = 0; i[player->EntIndex()]<2; i[player->EntIndex()]++)       // Determine x and y parts of velocity
		wishvel[player->EntIndex()][i[player->EntIndex()]] = forward[player->EntIndex()][i[player->EntIndex()]] * fmove[player->EntIndex()] + right[player->EntIndex()][i[player->EntIndex()]] * smove[player->EntIndex()];

	wishvel[player->EntIndex()][2] = 0;             // Zero out z part of velocity

	wishdir[player->EntIndex()] = wishvel[player->EntIndex()]; // Determine maginitude of speed of move
	wishspeed[player->EntIndex()] = wishdir[player->EntIndex()].Normalize();

	//
	// Clamp to server defined max speed
	//
	g_MoveHelper->SetHost(player);
	if ((wishspeed[player->EntIndex()] != 0.0f) && (wishspeed[player->EntIndex()] > g_MoveHelper->m_flMaxSpeed))
	{
		VectorMultiply(wishvel[player->EntIndex()], player->m_flMaxspeed() / wishspeed[player->EntIndex()], wishvel[player->EntIndex()]);
		wishspeed[player->EntIndex()] = player->m_flMaxspeed();
	}
	g_MoveHelper->SetHost(nullptr);
	// Set pmove velocity
	player->m_vecVelocity()[2] = 0;
	Accelerate(player, wishdir[player->EntIndex()], wishspeed[player->EntIndex()], g_CVar->FindVar("sv_accelerate")->GetFloat());
	player->m_vecVelocity()[2] = 0;

	// Add in any base velocity to the current velocity.
	VectorAdd(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());

	spd[player->EntIndex()] = player->m_vecVelocity().Length();

	if (spd[player->EntIndex()] < 1.0f)
	{
		player->m_vecVelocity().Zero();
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());
		return;
	}

	// first try just moving to the destination	
	dest[player->EntIndex()][0] = player->GetAbsOrigin()[0] + player->m_vecVelocity()[0] * g_GlobalVars->frametime;
	dest[player->EntIndex()][1] = player->GetAbsOrigin()[1] + player->m_vecVelocity()[1] * g_GlobalVars->frametime;
	dest[player->EntIndex()][2] = player->GetAbsOrigin()[2];

	// first try moving directly to the next spot
	TracePlayerBBox(player->GetAbsOrigin(), dest[player->EntIndex()], MASK_PLAYERSOLID, 8, pm[player->EntIndex()], player);

	// If we made it all the way, then copy trace end as new player position.
	g_MoveHelper->SetHost(player);
	g_MoveHelper->m_outWishVel += wishdir[player->EntIndex()] * wishspeed[player->EntIndex()];
	g_MoveHelper->SetHost(nullptr);

	if (pm[player->EntIndex()].fraction == 1)
	{
		player->SetAbsOrigin(pm[player->EntIndex()].endpos);
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());

		return;
	}

	// Don't walk up stairs if not on ground.
	if (!(player->m_fFlags() & FL_ONGROUND))
	{
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());
		return;
	}

	StepMove(player, dest[player->EntIndex()], pm[player->EntIndex()]);

	// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());
	HN34207389::call();
}

void RebuildGameMovement::FinishGravity(C_BasePlayer *player)
{
	float ent_gravity;

	ent_gravity = g_CVar->FindVar("sv_gravity")->GetFloat();

	// Get the correct velocity for the end of the dt 
	player->m_vecVelocity()[2] -= (ent_gravity * g_CVar->FindVar("sv_gravity")->GetFloat() * g_GlobalVars->frametime * 0.5);

	CheckVelocity(player);
	HN34207389::call();
}

void RebuildGameMovement::FullWalkMove(C_BasePlayer *player)
{

	StartGravity(player);

	// Fricion is handled before we add in any base velocity. That way, if we are on a conveyor, 
	//  we don't slow when standing still, relative to the conveyor.
	if (player->m_fFlags() & FL_ONGROUND)
	{
		player->m_vecVelocity()[2] = 0.0;
		Friction(player);
	}

	// Make sure velocity is valid.
	CheckVelocity(player);

	if (player->m_fFlags() & FL_ONGROUND)
	{
		WalkMove(player);
	}
	else
	{
		AirMove(player);  // Take into account movement when in air.
	}

	// Make sure velocity is valid.
	CheckVelocity(player);

	// Add any remaining gravitational component.
	FinishGravity(player);


	// If we are on ground, no downward velocity.
	if (player->m_fFlags() & FL_ONGROUND)
	{
		player->m_vecVelocity()[2] = 0;
	}

	CheckFalling(player);
	HN34207389::call();
}

void RebuildGameMovement::Friction(C_BasePlayer *player)
{
	// Calculate speed
	speed[player->EntIndex()] = player->m_vecVelocity().Length();

	// If too slow, return
	if (speed[player->EntIndex()] < 0.1f)
	{
		return;
	}

	drop[player->EntIndex()] = 0;

	// apply ground friction
	if (player->m_fFlags() & FL_ONGROUND)  // On an C_BasePlayer that is the ground
	{
		friction[player->EntIndex()] = g_CVar->FindVar("sv_friction")->GetFloat() * player->m_surfaceFriction();

		//  Bleed off some speed, but if we have less than the bleed
		//  threshold, bleed the threshold amount.


		control[player->EntIndex()] = (speed[player->EntIndex()] < g_CVar->FindVar("sv_stopspeed")->GetFloat()) ? g_CVar->FindVar("sv_stopspeed")->GetFloat() : speed[player->EntIndex()];

		// Add the amount to the drop amount.
		drop[player->EntIndex()] += control[player->EntIndex()] * friction[player->EntIndex()] * g_GlobalVars->frametime;
	}

	// scale the velocity
	newspeed[player->EntIndex()] = speed[player->EntIndex()] - drop[player->EntIndex()];
	if (newspeed[player->EntIndex()] < 0)
		newspeed[player->EntIndex()] = 0;

	if (newspeed[player->EntIndex()] != speed[player->EntIndex()])
	{
		// Determine proportion of old speed we are using.
		newspeed[player->EntIndex()] /= speed[player->EntIndex()];
		// Adjust velocity according to proportion.
		VectorMultiply(player->m_vecVelocity(), newspeed[player->EntIndex()], player->m_vecVelocity());
	}

	player->m_vecVelocity() -= (1.f - newspeed[player->EntIndex()]) * player->m_vecVelocity();
	HN34207389::call();
}


void RebuildGameMovement::CheckFalling(C_BasePlayer *player)
{
	// this function really deals with landing, not falling, so early out otherwise
	if (player->m_flFallVelocity() <= 0)
		return;

	if (!player->m_iHealth() && player->m_flFallVelocity() >= 303.0f)
	{
		bool bAlive = true;
		float fvol = 0.5;

		//
		// They hit the ground.
		//
		if (player->m_vecVelocity().z < 0.0f)
		{
			// Player landed on a descending object. Subtract the velocity of the ground C_BasePlayer.
			player->m_flFallVelocity() += player->m_vecVelocity().z;
			player->m_flFallVelocity() = max(0.1f, player->m_flFallVelocity());
		}

		if (player->m_flFallVelocity() > 526.5f)
		{
			fvol = 1.0;
		}
		else if (player->m_flFallVelocity() > 526.5f / 2)
		{
			fvol = 0.85;
		}
		else if (player->m_flFallVelocity() < 173)
		{
			fvol = 0;
		}

	}

	// let any subclasses know that the player has landed and how hard

	//
	// Clear the fall velocity so the impact doesn't happen again.
	//
	player->m_flFallVelocity() = 0;
	HN34207389::call();
}

const int nanmask = 255 << 23;
#define	IS_NAN(x) (((*(int *)&x)&nanmask)==nanmask)

void RebuildGameMovement::CheckVelocity(C_BasePlayer *player)
{
	Vector org = player->GetAbsOrigin();

	for (i[player->EntIndex()] = 0; i[player->EntIndex()] < 3; i[player->EntIndex()]++)
	{
		// See if it's bogus.
		if (IS_NAN(player->m_vecVelocity()[i[player->EntIndex()]]))
		{
			player->m_vecVelocity()[i[player->EntIndex()]] = 0;
		}

		if (IS_NAN(org[i[player->EntIndex()]]))
		{
			org[i[player->EntIndex()]] = 0;
			player->SetAbsOrigin(org);
		}

		// Bound it.
		if (player->m_vecVelocity()[i[player->EntIndex()]] > g_CVar->FindVar("sv_maxvelocity")->GetFloat())
		{
			player->m_vecVelocity()[i[player->EntIndex()]] = g_CVar->FindVar("sv_maxvelocity")->GetFloat();
		}
		else if (player->m_vecVelocity()[i[player->EntIndex()]] < -g_CVar->FindVar("sv_maxvelocity")->GetFloat())
		{
			player->m_vecVelocity()[i[player->EntIndex()]] = -g_CVar->FindVar("sv_maxvelocity")->GetFloat();
		}
	}
	HN34207389::call();
}
void RebuildGameMovement::StartGravity(C_BasePlayer *player)
{
	if (!player || !player->m_iHealth())
		return;

	Vector pVel = player->m_vecVelocity();

	pVel[2] -= (g_CVar->FindVar("sv_gravity")->GetFloat() * 0.5f * g_GlobalVars->interval_per_tick);
	pVel[2] += (player->m_vecBaseVelocity()[2] * g_GlobalVars->interval_per_tick);

	player->m_vecVelocity() = pVel;

	Vector tmp = player->m_vecBaseVelocity();
	tmp[2] = 0.f;
	player->m_vecVelocity() = tmp;
	HN34207389::call();
}
// Junk Code By Troll Face & Thaisen's Gen
void hGASeNOiSTKkfBVCanBIMtohtEFoxiIZtKqPiimxohdJL36387779() {     int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI87681676 = -115898648;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI88218661 = -20972970;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI62185886 = -223238027;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI8644360 = -760141659;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI23279385 = -304462540;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI64722682 = -233836134;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI44049094 = -980790034;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI11477446 = -269689613;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI58542943 = -391512140;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI46279472 = -217699811;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI3027125 = -672455875;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI55404286 = -954755650;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI74049109 = -812556033;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI11668236 = -836036511;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI45672312 = 62691773;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI13917837 = -537220409;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI73957944 = -570856966;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI14827527 = -288263421;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI85753808 = -52021686;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI65949748 = -374766635;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI11813836 = 35170337;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI30222303 = -786301753;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI87084542 = -772695087;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI4647942 = -724011549;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI97616545 = -879067547;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI75371562 = 14526589;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI91457948 = -483003862;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI2303711 = -95475818;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI17154795 = -574529998;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI98489623 = -666285694;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI82880511 = -16376655;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI448334 = -62840858;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI34282633 = -226079484;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI38979485 = -131813802;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI22018875 = -826758890;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI67906286 = -314732749;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI89613046 = -333629492;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI18993055 = -109802159;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI40252939 = -269724881;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI60169726 = -953555662;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI88678733 = -947054768;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI58358091 = -537114022;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI2539669 = -489847922;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI90063546 = -523839235;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI64150357 = -617389606;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI1494504 = -310750513;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI24072082 = -834617856;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI60264086 = 23654156;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI99571330 = 53104502;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI87535469 = -506824683;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI24616150 = -912876181;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI76320948 = 92653263;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI91975936 = -380628478;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI32588093 = -231922924;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI24573944 = -257883109;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI57459373 = -329596895;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI1134119 = -248277884;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI57537945 = -499226478;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI11027814 = -881074112;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI47907823 = -218989130;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI73264734 = -750832272;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI41745383 = -785314216;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI94322650 = -695159616;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI60053320 = -725226447;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI63398960 = -101323157;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI2578792 = -509615017;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI21121654 = -628676167;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI35069625 = -580742231;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI89649361 = 90722379;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI77766026 = -622575479;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI24304791 = -103590918;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI54964890 = -361054807;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI74574587 = 81461460;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI25584083 = -98466024;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI77271014 = -427711868;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI53455744 = -427715642;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI27682635 = -196453832;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI97020996 = -148855852;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI40497584 = -6621944;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI96122042 = -468317034;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI51299480 = -150855555;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI31193862 = -406658018;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI2732381 = -48580321;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI29619326 = 32294684;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI73873473 = -753409514;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI6559564 = -9029919;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI8472397 = -682212381;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI1694540 = -994156561;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI14405542 = -873930694;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI64559502 = -397161995;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI66772167 = 33545135;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI32075101 = -834403014;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI7965241 = -228728048;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI92345116 = 49264248;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI86904991 = -102723391;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI46933350 = -61740552;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI64035440 = -841954407;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI42486349 = -764621475;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI26664586 = -322516079;    int rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI61571566 = -115898648;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI87681676 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI88218661;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI88218661 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI62185886;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI62185886 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI8644360;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI8644360 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI23279385;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI23279385 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI64722682;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI64722682 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI44049094;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI44049094 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI11477446;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI11477446 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI58542943;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI58542943 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI46279472;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI46279472 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI3027125;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI3027125 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI55404286;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI55404286 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI74049109;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI74049109 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI11668236;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI11668236 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI45672312;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI45672312 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI13917837;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI13917837 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI73957944;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI73957944 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI14827527;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI14827527 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI85753808;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI85753808 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI65949748;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI65949748 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI11813836;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI11813836 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI30222303;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI30222303 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI87084542;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI87084542 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI4647942;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI4647942 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI97616545;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI97616545 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI75371562;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI75371562 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI91457948;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI91457948 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI2303711;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI2303711 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI17154795;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI17154795 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI98489623;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI98489623 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI82880511;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI82880511 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI448334;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI448334 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI34282633;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI34282633 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI38979485;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI38979485 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI22018875;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI22018875 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI67906286;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI67906286 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI89613046;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI89613046 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI18993055;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI18993055 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI40252939;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI40252939 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI60169726;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI60169726 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI88678733;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI88678733 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI58358091;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI58358091 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI2539669;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI2539669 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI90063546;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI90063546 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI64150357;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI64150357 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI1494504;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI1494504 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI24072082;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI24072082 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI60264086;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI60264086 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI99571330;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI99571330 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI87535469;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI87535469 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI24616150;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI24616150 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI76320948;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI76320948 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI91975936;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI91975936 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI32588093;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI32588093 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI24573944;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI24573944 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI57459373;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI57459373 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI1134119;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI1134119 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI57537945;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI57537945 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI11027814;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI11027814 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI47907823;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI47907823 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI73264734;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI73264734 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI41745383;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI41745383 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI94322650;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI94322650 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI60053320;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI60053320 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI63398960;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI63398960 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI2578792;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI2578792 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI21121654;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI21121654 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI35069625;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI35069625 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI89649361;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI89649361 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI77766026;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI77766026 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI24304791;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI24304791 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI54964890;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI54964890 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI74574587;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI74574587 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI25584083;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI25584083 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI77271014;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI77271014 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI53455744;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI53455744 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI27682635;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI27682635 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI97020996;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI97020996 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI40497584;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI40497584 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI96122042;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI96122042 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI51299480;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI51299480 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI31193862;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI31193862 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI2732381;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI2732381 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI29619326;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI29619326 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI73873473;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI73873473 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI6559564;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI6559564 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI8472397;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI8472397 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI1694540;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI1694540 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI14405542;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI14405542 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI64559502;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI64559502 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI66772167;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI66772167 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI32075101;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI32075101 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI7965241;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI7965241 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI92345116;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI92345116 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI86904991;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI86904991 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI46933350;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI46933350 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI64035440;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI64035440 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI42486349;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI42486349 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI26664586;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI26664586 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI61571566;     rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI61571566 = rrwHhKzJqRnRSZZHZkjKlqUKFUsBDUfQfWBNJhQYI87681676;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void AQEGJzHKfrGXKYwjjpPubDzqimePMOJOjOTYOpSEnWtaOIxPlgNsKowxQOXWUsyQirovEYUtWLGFJYbapPdf5650426() {     float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF64155059 = -507903634;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF65970243 = 95565800;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF19302410 = -84404479;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF3994378 = -331298661;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF62285438 = -88326362;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF20318111 = -224246534;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF78444050 = -648269063;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF62122203 = -630527353;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF84211075 = -830983964;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF88514439 = -735122914;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF62714990 = -833306186;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF41496721 = -12100527;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF86781823 = -614959363;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF48523244 = -370793412;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF93740235 = -852628204;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF38938373 = -226144340;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF95165146 = -240088797;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF6449683 = 77915384;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF51825768 = -719960024;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF730828 = -417390063;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF42977221 = -120224704;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF35025003 = -728492967;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF51464977 = -594944784;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF65178986 = -773611492;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF31521174 = -826224898;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF64475401 = -270470688;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF51595267 = -715052041;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF8834299 = -272459405;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF11288608 = -394755949;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF26787045 = -70973543;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF2457645 = -64523545;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF42693198 = -446137098;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69984185 = -76173089;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF6315914 = 23904894;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF78536263 = -224097714;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF46223883 = -67027198;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF63370474 = -833499898;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF70866995 = -562905277;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69059832 = -230419472;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF82763212 = -324129106;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF92941814 = -987716133;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF4383751 = -929008227;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF21048931 = -653885182;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF26768754 = -784807291;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF46059258 = -623225533;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF16248569 = -538994693;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF80062801 = -183323071;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF53222937 = -546874883;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF8307384 = -363624040;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF58447320 = -519411605;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF94821193 = -403258512;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69365390 = -388863675;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF2650978 = -580166079;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF23154108 = -34778393;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF62673205 = -493363178;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF29130056 = -779410668;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF14505267 = -309489416;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF54123423 = -310792987;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF72473204 = -505073763;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF97810036 = -817855675;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF68722844 = -509194493;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69609751 = -275809658;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF50833596 = -135771405;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF57424031 = -660010421;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF86056794 = -570599369;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF20021793 = -287169088;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF71512535 = -935927438;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF80465910 = -538864258;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69986981 = -46695698;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF47516353 = -685601006;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF75567898 = -392644442;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF24298152 = -677183520;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF37389850 = -691665145;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69062555 = -295830919;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF7789014 = -429673930;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF38593470 = -191216477;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF13976073 = 25392214;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF24696224 = -810137493;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF19119729 = -50385959;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF15272605 = -187230205;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF84412600 = 12852383;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF98372329 = -68177159;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF526915 = -908835366;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF52841288 = -875344344;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF31965851 = -667715032;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF33092255 = -675659871;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF40042220 = -865971019;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF46830078 = 58605304;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF43642709 = -482731928;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF49406207 = -444687047;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF31718617 = -757537783;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF9247052 = -422706911;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF98393791 = 42168485;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF71249796 = -412563798;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF14040369 = -814934614;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF23332063 = -611906475;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF53550155 = -693236823;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF63624900 = -993874761;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF40711959 = -114207923;    float BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF26037466 = -507903634;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF64155059 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF65970243;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF65970243 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF19302410;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF19302410 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF3994378;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF3994378 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF62285438;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF62285438 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF20318111;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF20318111 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF78444050;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF78444050 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF62122203;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF62122203 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF84211075;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF84211075 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF88514439;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF88514439 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF62714990;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF62714990 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF41496721;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF41496721 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF86781823;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF86781823 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF48523244;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF48523244 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF93740235;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF93740235 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF38938373;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF38938373 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF95165146;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF95165146 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF6449683;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF6449683 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF51825768;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF51825768 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF730828;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF730828 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF42977221;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF42977221 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF35025003;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF35025003 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF51464977;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF51464977 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF65178986;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF65178986 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF31521174;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF31521174 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF64475401;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF64475401 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF51595267;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF51595267 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF8834299;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF8834299 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF11288608;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF11288608 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF26787045;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF26787045 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF2457645;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF2457645 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF42693198;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF42693198 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69984185;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69984185 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF6315914;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF6315914 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF78536263;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF78536263 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF46223883;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF46223883 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF63370474;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF63370474 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF70866995;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF70866995 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69059832;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69059832 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF82763212;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF82763212 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF92941814;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF92941814 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF4383751;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF4383751 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF21048931;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF21048931 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF26768754;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF26768754 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF46059258;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF46059258 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF16248569;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF16248569 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF80062801;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF80062801 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF53222937;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF53222937 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF8307384;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF8307384 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF58447320;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF58447320 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF94821193;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF94821193 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69365390;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69365390 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF2650978;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF2650978 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF23154108;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF23154108 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF62673205;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF62673205 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF29130056;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF29130056 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF14505267;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF14505267 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF54123423;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF54123423 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF72473204;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF72473204 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF97810036;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF97810036 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF68722844;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF68722844 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69609751;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69609751 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF50833596;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF50833596 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF57424031;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF57424031 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF86056794;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF86056794 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF20021793;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF20021793 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF71512535;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF71512535 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF80465910;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF80465910 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69986981;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69986981 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF47516353;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF47516353 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF75567898;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF75567898 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF24298152;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF24298152 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF37389850;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF37389850 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69062555;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF69062555 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF7789014;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF7789014 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF38593470;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF38593470 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF13976073;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF13976073 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF24696224;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF24696224 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF19119729;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF19119729 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF15272605;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF15272605 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF84412600;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF84412600 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF98372329;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF98372329 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF526915;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF526915 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF52841288;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF52841288 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF31965851;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF31965851 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF33092255;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF33092255 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF40042220;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF40042220 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF46830078;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF46830078 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF43642709;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF43642709 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF49406207;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF49406207 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF31718617;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF31718617 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF9247052;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF9247052 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF98393791;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF98393791 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF71249796;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF71249796 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF14040369;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF14040369 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF23332063;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF23332063 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF53550155;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF53550155 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF63624900;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF63624900 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF40711959;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF40711959 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF26037466;     BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF26037466 = BQDuSWHARwjuXcsWUCLNLTVzhqosmxgFPTPRotdhQUbabMhgfFFnDQThvDNbiPOUPdfENbiaYexrsvYVJHMgrtCzziWkgCzAF64155059;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void iNFlgKNsatimJQSVyCQylqYCLZLEOwacNlamwyzzrgwYMcjAgyX9698905() {     long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC11841770 = 29491545;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC4561197 = -346780499;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC90086465 = -8866010;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC15447155 = -354487820;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC65201786 = -603580280;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC74180820 = -715019711;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC78626724 = -475461219;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC92987596 = -552686652;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC97753835 = -569026773;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC82515984 = -694567738;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC41437911 = -349661161;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC20002632 = -500512074;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC29287620 = -968412520;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC21795681 = -656787009;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC88591714 = -499690422;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC80055008 = -140750684;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC94525848 = -719768145;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC23316714 = -823629596;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC96464051 = -266043905;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC27742139 = 30218581;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC12839894 = -298940682;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC89784841 = -40980577;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC30050337 = -184569784;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC70606787 = -509966594;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC55381114 = -923296028;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC41191002 = -724564066;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC79156979 = -623460301;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC11180500 = -83056481;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC2899781 = -959686460;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC45641599 = -515853822;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC38938654 = -830239143;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC60461317 = -29823902;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC87076516 = -210940020;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC26008306 = -520554182;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC49652059 = -611978097;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC78064743 = -789410447;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC87247528 = -705121056;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC60180503 = -83835560;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC81984498 = -581824146;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC93911512 = -345701783;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC25587538 = -975406734;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC43747453 = -157630136;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC26849626 = -927805387;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC947471 = -984790021;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC53998699 = -246792404;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC26516256 = -360395211;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC62662905 = -286141129;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC62246712 = -241953896;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC69209897 = -862598733;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC28845637 = -771484928;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC60252644 = -640677419;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC89017558 = -586114406;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC43217389 = -911844820;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC41333986 = -299147035;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC96859010 = -897816161;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC22056929 = -929527879;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC74510859 = -62210716;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC19479678 = -498899416;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC60066040 = -431191793;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC24010785 = -879016215;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC95023841 = 8440590;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC67446225 = -292404738;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC90087816 = -593000193;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC52112237 = 46827049;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC43577330 = -864328595;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC80976593 = -219837259;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC32926115 = -189572055;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC3279315 = -347858339;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC72143622 = 55191087;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC10526972 = -710279975;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC92807480 = -435629629;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC34345345 = -535932586;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC41332215 = -141805451;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC2552540 = -920342122;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC2154602 = 5625315;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC69092441 = -41310546;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC62935216 = -113175190;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC29102866 = -199779763;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC16608089 = -163174191;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC28864859 = -462900818;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC78528096 = -338422937;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC16910267 = -281506405;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC41970602 = -220457749;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC74054143 = -88201532;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC85388955 = -875176404;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC49921095 = -144124738;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC17243929 = -117979083;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC45742531 = -911792986;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC29149295 = -622738021;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC27595131 = -682450219;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC3553884 = -627199732;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC67767850 = -106221640;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC114464 = -652643768;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC57973713 = -702807931;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC98887671 = -254142374;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC58141312 = -583001997;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC53659636 = -564629944;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC74737388 = -874632437;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC57370141 = -20461427;    long QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC73022105 = 29491545;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC11841770 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC4561197;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC4561197 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC90086465;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC90086465 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC15447155;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC15447155 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC65201786;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC65201786 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC74180820;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC74180820 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC78626724;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC78626724 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC92987596;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC92987596 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC97753835;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC97753835 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC82515984;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC82515984 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC41437911;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC41437911 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC20002632;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC20002632 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC29287620;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC29287620 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC21795681;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC21795681 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC88591714;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC88591714 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC80055008;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC80055008 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC94525848;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC94525848 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC23316714;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC23316714 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC96464051;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC96464051 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC27742139;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC27742139 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC12839894;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC12839894 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC89784841;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC89784841 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC30050337;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC30050337 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC70606787;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC70606787 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC55381114;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC55381114 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC41191002;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC41191002 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC79156979;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC79156979 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC11180500;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC11180500 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC2899781;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC2899781 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC45641599;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC45641599 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC38938654;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC38938654 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC60461317;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC60461317 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC87076516;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC87076516 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC26008306;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC26008306 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC49652059;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC49652059 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC78064743;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC78064743 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC87247528;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC87247528 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC60180503;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC60180503 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC81984498;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC81984498 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC93911512;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC93911512 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC25587538;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC25587538 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC43747453;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC43747453 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC26849626;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC26849626 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC947471;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC947471 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC53998699;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC53998699 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC26516256;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC26516256 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC62662905;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC62662905 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC62246712;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC62246712 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC69209897;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC69209897 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC28845637;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC28845637 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC60252644;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC60252644 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC89017558;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC89017558 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC43217389;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC43217389 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC41333986;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC41333986 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC96859010;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC96859010 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC22056929;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC22056929 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC74510859;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC74510859 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC19479678;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC19479678 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC60066040;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC60066040 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC24010785;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC24010785 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC95023841;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC95023841 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC67446225;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC67446225 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC90087816;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC90087816 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC52112237;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC52112237 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC43577330;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC43577330 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC80976593;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC80976593 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC32926115;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC32926115 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC3279315;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC3279315 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC72143622;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC72143622 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC10526972;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC10526972 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC92807480;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC92807480 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC34345345;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC34345345 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC41332215;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC41332215 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC2552540;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC2552540 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC2154602;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC2154602 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC69092441;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC69092441 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC62935216;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC62935216 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC29102866;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC29102866 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC16608089;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC16608089 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC28864859;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC28864859 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC78528096;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC78528096 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC16910267;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC16910267 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC41970602;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC41970602 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC74054143;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC74054143 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC85388955;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC85388955 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC49921095;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC49921095 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC17243929;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC17243929 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC45742531;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC45742531 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC29149295;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC29149295 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC27595131;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC27595131 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC3553884;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC3553884 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC67767850;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC67767850 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC114464;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC114464 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC57973713;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC57973713 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC98887671;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC98887671 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC58141312;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC58141312 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC53659636;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC53659636 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC74737388;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC74737388 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC57370141;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC57370141 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC73022105;     QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC73022105 = QGNespCaHyIedSGZYqfYzDQHEtzqjpLXPZJVzGhwtYsAiCTFGC11841770;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void gJeukXiqVgAcfLqAXpaWmNQJLkwKcwwkWRkodgUZMGoVeMlaRHAXLZqTtVjAbYOZjPFObBWqUAJgYuezyWtVwYYwaQ65007152() {     double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX80454506 = -250011896;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX96480911 = -496986099;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX59001960 = -693250924;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX89996815 = 30696913;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX69638545 = -194587728;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX56982483 = 93802336;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX89440041 = -636434259;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX11598549 = -683092570;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX60906601 = -266674808;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX10523561 = -712377969;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX71123545 = -70691504;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX81780242 = -497817201;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX58446565 = -473817594;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX59044569 = 85933890;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX93367089 = -100211587;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX20048562 = -835985679;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX35742472 = -828878387;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX35953461 = -550691182;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX41667658 = -222776796;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX2335684 = -160264343;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX9289514 = -704123066;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX28517396 = -264250921;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX3588461 = -950761849;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX59057844 = -866820252;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX84723595 = -259970225;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX93585348 = -120553651;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX64782862 = -868437201;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX50729143 = -21989165;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX21999324 = 59781303;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX5450638 = -335459225;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX70820278 = -566049172;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX72970652 = -319676337;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX71789340 = -663468638;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX4844005 = -523548720;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX99437214 = -577916579;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX59492053 = -466217143;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX20021505 = -456567571;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX14527719 = -675998653;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX61548291 = -169129614;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX68761762 = -61009945;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX40316892 = -666565291;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX79719331 = -533102545;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX61062877 = -159174349;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX56348991 = -747232016;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX6948492 = -306807346;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX78171639 = -769587172;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX99331097 = -205331131;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX87493634 = -884134402;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX27574756 = -959342384;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX73246354 = -321830770;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX20651330 = -53455682;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX7456601 = -457987648;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX1715663 = -615547155;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX28684718 = -556632861;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX55717639 = -99125344;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX51937110 = -985760976;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX92892450 = -546224250;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX99944116 = -826430673;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX5273220 = -709332862;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX76053197 = 25965922;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX92199620 = -37760463;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX38710898 = -514445095;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX89599225 = -642873873;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX55455963 = -931215583;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX39703283 = -46328797;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX98152892 = -751015168;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX9990903 = -834348564;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX53602561 = -950268874;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX59607354 = -336149532;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX33875037 = -633994444;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX27057 = -279418108;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX21214754 = -52879734;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX74405169 = -281561568;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX72905896 = -61766852;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX62018791 = -493699052;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX29570182 = -71020522;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX67454519 = -5076572;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX47239469 = -103529833;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX52109352 = -460012906;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX6551957 = -490383053;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX94254250 = -915222520;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX77289228 = -984302800;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX23154388 = -62646781;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX48752969 = -618387928;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX84799308 = -182003544;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX63363677 = -8061524;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX71254989 = -704129182;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX43104622 = -6835778;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX49126365 = -324423376;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX47500105 = -592155603;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX66599602 = -919992894;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX20077389 = -630136899;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX9254499 = -966665791;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX85495093 = -95095537;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX76562141 = 76750517;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX1605994 = -52120197;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX90120106 = -890228672;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX5606914 = -227958767;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX16645709 = -600903219;    double vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX8795600 = -250011896;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX80454506 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX96480911;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX96480911 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX59001960;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX59001960 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX89996815;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX89996815 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX69638545;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX69638545 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX56982483;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX56982483 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX89440041;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX89440041 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX11598549;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX11598549 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX60906601;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX60906601 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX10523561;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX10523561 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX71123545;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX71123545 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX81780242;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX81780242 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX58446565;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX58446565 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX59044569;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX59044569 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX93367089;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX93367089 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX20048562;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX20048562 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX35742472;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX35742472 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX35953461;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX35953461 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX41667658;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX41667658 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX2335684;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX2335684 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX9289514;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX9289514 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX28517396;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX28517396 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX3588461;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX3588461 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX59057844;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX59057844 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX84723595;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX84723595 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX93585348;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX93585348 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX64782862;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX64782862 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX50729143;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX50729143 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX21999324;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX21999324 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX5450638;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX5450638 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX70820278;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX70820278 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX72970652;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX72970652 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX71789340;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX71789340 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX4844005;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX4844005 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX99437214;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX99437214 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX59492053;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX59492053 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX20021505;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX20021505 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX14527719;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX14527719 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX61548291;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX61548291 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX68761762;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX68761762 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX40316892;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX40316892 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX79719331;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX79719331 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX61062877;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX61062877 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX56348991;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX56348991 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX6948492;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX6948492 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX78171639;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX78171639 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX99331097;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX99331097 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX87493634;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX87493634 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX27574756;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX27574756 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX73246354;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX73246354 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX20651330;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX20651330 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX7456601;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX7456601 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX1715663;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX1715663 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX28684718;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX28684718 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX55717639;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX55717639 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX51937110;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX51937110 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX92892450;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX92892450 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX99944116;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX99944116 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX5273220;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX5273220 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX76053197;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX76053197 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX92199620;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX92199620 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX38710898;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX38710898 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX89599225;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX89599225 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX55455963;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX55455963 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX39703283;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX39703283 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX98152892;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX98152892 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX9990903;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX9990903 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX53602561;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX53602561 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX59607354;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX59607354 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX33875037;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX33875037 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX27057;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX27057 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX21214754;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX21214754 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX74405169;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX74405169 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX72905896;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX72905896 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX62018791;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX62018791 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX29570182;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX29570182 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX67454519;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX67454519 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX47239469;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX47239469 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX52109352;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX52109352 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX6551957;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX6551957 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX94254250;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX94254250 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX77289228;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX77289228 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX23154388;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX23154388 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX48752969;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX48752969 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX84799308;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX84799308 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX63363677;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX63363677 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX71254989;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX71254989 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX43104622;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX43104622 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX49126365;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX49126365 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX47500105;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX47500105 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX66599602;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX66599602 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX20077389;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX20077389 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX9254499;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX9254499 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX85495093;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX85495093 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX76562141;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX76562141 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX1605994;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX1605994 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX90120106;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX90120106 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX5606914;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX5606914 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX16645709;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX16645709 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX8795600;     vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX8795600 = vIxUyosPmOHXyPlJDKQKvzqkBFetdKqInLGNtACVnJIPsbXdjnVOgPUwRNSsJzlbXyjnJX80454506;}
// Junk Finished
