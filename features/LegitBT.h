#pragma once
#include "../SDK.hpp"

struct backtrackData
{
	float simtime;
	Vector hitboxPos;
};

struct player_bt {

	matrix3x4_t bone[128];

};

class NewBacktrack : public Singleton<NewBacktrack>
{
public:
	void LegitBacktrack(CUserCmd* cmd);
private:

};

extern player_bt bt_data[64][12];
extern backtrackData headPositions[64][12];