#pragma once

#include <string>

#include "../Singleton.hpp"
#include "../helpers/Utils.hpp"
#include "../math/QAngle.hpp"

class CUserCmd;
class QAngle;
class CViewSetup;

struct acinfo
{
	acinfo()
	{
		smac_aimbot = smac_autotrigger = smac_client = smac_commands = smac_core = smac_cvars = smac_eyetest = smac_speedhack = smac_spinhack = false;
	}

	bool smac_core;
	bool smac_aimbot;
	bool smac_autotrigger;
	bool smac_client;
	bool smac_commands;
	bool smac_cvars;
	bool smac_eyetest;
	bool smac_speedhack;
	//bool smac_wallhack;	2016-03-09	GoD-Tony	Remove Anti-Wallhack support for CS:GO. Already exists with sv_occlude_players. default tip
	bool smac_spinhack;
};

class Miscellaneous : public Singleton<Miscellaneous>
{
public:

	void Bhop(CUserCmd *userCMD);
	void AutoStrafe(CUserCmd *userCMD);
	void Fakelag(CUserCmd *userCMD);
	void FakeDuck(CUserCmd* userCMD);
	inline int32_t GetChocked() { return choked; }
	void ChangeName(const char *name);
	//void RemoveCrosshair();
	void NameChanger();
	void ChatSpamer();
	void ClanTag();
	void DetectAC(acinfo& output);
	//void AimwareClantag();
	//void AimwareChatSpamer();
	void ThirdPerson();
	void ChangeRegion();
	void DoorSpam(CUserCmd* usercmd);
	void FixMovement(CUserCmd *usercmd, QAngle &wish_angle);
	void AntiAim(CUserCmd *usercmd);
	void AutoPistol(CUserCmd *usercmd);
	void AldaVerdammteScheisse(CUserCmd *usercmd);
	void MichaelJackson(CUserCmd* usercmd);
	void PunchAngleFix_RunCommand(void* base_player);
	void PunchAngleFix_FSN();

	int changes = -1;

	template<class T, class U>
	T clamp(T in, U low, U high);

	std::string gladTag = "";

private:

	const char *setStrRight(std::string txt, unsigned int value);
	//std::string awTag = "AIMWARE.net ";
	QAngle m_aimPunchAngle[128];

	int32_t choked = 0;
};