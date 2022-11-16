#include "../Structs.hpp"

#include "Skinchanger.hpp"
#include "..//HNJ.h"
#include "../helpers/json.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>
 // hack


const std::map<size_t, Item_t> k_weapon_info =
{
	{ WEAPON_KNIFE,{ "models/weapons/v_knife_default_ct.mdl", "knife_default_ct" } },
	{ WEAPON_KNIFE_T,{ "models/weapons/v_knife_default_t.mdl", "knife_t" } },
	{ WEAPON_KNIFE_BAYONET,{ "models/weapons/v_knife_bayonet.mdl", "bayonet" } },
	{ WEAPON_KNIFE_FLIP,{ "models/weapons/v_knife_flip.mdl", "knife_flip" } },
	{ WEAPON_KNIFE_GUT,{ "models/weapons/v_knife_gut.mdl", "knife_gut" } },
	{ WEAPON_KNIFE_KARAMBIT,{ "models/weapons/v_knife_karam.mdl", "knife_karambit" } },
	{ WEAPON_KNIFE_M9_BAYONET,{ "models/weapons/v_knife_m9_bay.mdl", "knife_m9_bayonet" } },
	{ WEAPON_KNIFE_TACTICAL,{ "models/weapons/v_knife_tactical.mdl", "knife_tactical" } },
	{ WEAPON_KNIFE_FALCHION,{ "models/weapons/v_knife_falchion_advanced.mdl", "knife_falchion" } },
	{ WEAPON_KNIFE_SURVIVAL_BOWIE,{ "models/weapons/v_knife_survival_bowie.mdl", "knife_survival_bowie" } },
	{ WEAPON_KNIFE_BUTTERFLY,{ "models/weapons/v_knife_butterfly.mdl", "knife_butterfly" } },
	{ WEAPON_KNIFE_PUSH,{ "models/weapons/v_knife_push.mdl", "knife_push" } },
	{ WEAPON_KNIFE_URSUS,{ "models/weapons/v_knife_ursus.mdl", "knife_ursus" } },
	{ WEAPON_KNIFE_GYPSY_JACKKNIFE,{ "models/weapons/v_knife_gypsy_jackknife.mdl", "knife_gypsy_jackknife" } },
	{ WEAPON_KNIFE_STILETTO,{ "models/weapons/v_knife_stiletto.mdl", "knife_stiletto" } },
	{ WEAPON_KNIFE_WIDOWMAKER,{ "models/weapons/v_knife_widowmaker.mdl", "knife_widowmaker" } },
	{ GLOVE_STUDDED_BLOODHOUND,{ "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl" } },
	{ GLOVE_T_SIDE,{ "models/weapons/v_models/arms/glove_fingerless/v_glove_fingerless.mdl" } },
	{ GLOVE_CT_SIDE,{ "models/weapons/v_models/arms/glove_hardknuckle/v_glove_hardknuckle.mdl" } },
	{ GLOVE_SPORTY,{ "models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl" } },
	{ GLOVE_SLICK,{ "models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl" } },
	{ GLOVE_LEATHER_WRAP,{ "models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl" } },
	{ GLOVE_MOTORCYCLE,{ "models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl" } },
	{ GLOVE_SPECIALIST,{ "models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl" } }
};

const std::vector<WeaponName_t> k_knife_names =
{
	{ 0, "Default" },
	{ WEAPON_KNIFE_BAYONET, "Bayonet" },
	{ WEAPON_KNIFE_FLIP, "Flip Knife" },
	{ WEAPON_KNIFE_GUT, "Gut Knife" },
	{ WEAPON_KNIFE_KARAMBIT, "Karambit" },
	{ WEAPON_KNIFE_M9_BAYONET, "M9 Bayonet" },
	{ WEAPON_KNIFE_TACTICAL, "Huntsman Knife" },
	{ WEAPON_KNIFE_FALCHION, "Falchion Knife" },
	{ WEAPON_KNIFE_SURVIVAL_BOWIE, "Bowie Knife" },
	{ WEAPON_KNIFE_BUTTERFLY, "Butterfly Knife" },
	{ WEAPON_KNIFE_PUSH, "Shadow Daggers" },
	{ WEAPON_KNIFE_URSUS, "Ursus Knife"},
	{ WEAPON_KNIFE_GYPSY_JACKKNIFE, "Navaja Knife" },
	{ WEAPON_KNIFE_STILETTO, "Stiletto Knife" },
	{ WEAPON_KNIFE_WIDOWMAKER, "Talon Knife" }
};

const std::vector<WeaponName_t> k_glove_names =
{
	{ 0, "Default" },
	{ GLOVE_STUDDED_BLOODHOUND, "Bloodhound" },
	{ GLOVE_T_SIDE, "Default (Terrorists)" },
	{ GLOVE_CT_SIDE, "Default (Counter-Terrorists)" },
	{ GLOVE_SPORTY, "Sporty" },
	{ GLOVE_SLICK, "Slick" },
	{ GLOVE_LEATHER_WRAP, "Handwrap" },
	{ GLOVE_MOTORCYCLE, "Motorcycle" },
	{ GLOVE_SPECIALIST, "Specialist" },
	{ GLOVE_HYDRA, "Hydra"}
};

const std::vector<WeaponName_t> k_weapon_names =
{
	{ WEAPON_KNIFE, "Knife" },
	{ GLOVE_T_SIDE, "Glove" },
	{ 7, "AK-47" },
	{ 8, "AUG" },
	{ 9, "AWP" },
	{ 63, "CZ-75" },
	{ 1, "Desert Eagle" },
	{ 2, "Dual Berettas" },
	{ 10, "FAMAS" },
	{ 3, "Five-SeveN" },
	{ 11, "G3SG1" },
	{ 13, "Galil AR" },
	{ 4, "Glock-18" },
	{ 14, "M249" },
	{ 60, "M4A1-S" },
	{ 16, "M4A4" },
	{ 17, "MAC-10" },
	{ 27, "MAG-7" },
	{ 33, "MP7" },
	{ 23, "MP5SD"},
	{ 34, "MP9" },
	{ 28, "Negev" },
	{ 35, "Nova" },
	{ 32, "P2000" },
	{ 36, "P250" },
	{ 19, "P90" },
	{ 26, "PP-Bizon" },
	{ 64, "R8 Revolver" },
	{ 29, "Sawed-Off" },
	{ 38, "SCAR-20" },
	{ 40, "SSG 08" },
	{ 39, "SG-553" },
	{ 30, "Tec-9" },
	{ 24, "UMP-45" },
	{ 61, "USP-S" },
	{ 25, "XM1014" },
};

const std::vector<QualityName_t> k_quality_names =
{
	{ 0, "Default" },
	{ 1, "Genuine" },
	{ 2, "Vintage" },
	{ 3, "Unusual" },
	{ 5, "Community" },
	{ 6, "Developer" },
	{ 7, "Self-Made" },
	{ 8, "Customized" },
	{ 9, "Strange" },
	{ 10, "Completed" },
	{ 12, "Tournament" }
};

enum ESequence
{
	SEQUENCE_DEFAULT_DRAW = 0,
	SEQUENCE_DEFAULT_IDLE1 = 1,
	SEQUENCE_DEFAULT_IDLE2 = 2,
	SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
	SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
	SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
	SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
	SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
	SEQUENCE_DEFAULT_LOOKAT01 = 12,

	SEQUENCE_BUTTERFLY_DRAW = 0,
	SEQUENCE_BUTTERFLY_DRAW2 = 1,
	SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
	SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

	SEQUENCE_FALCHION_IDLE1 = 1,
	SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
	SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
	SEQUENCE_FALCHION_LOOKAT01 = 12,
	SEQUENCE_FALCHION_LOOKAT02 = 13,

	SEQUENCE_DAGGERS_IDLE1 = 1,
	SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
	SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
	SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
	SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

	SEQUENCE_BOWIE_IDLE1 = 1,
};

inline int RandomSequence(int low, int high)
{
	return rand() % (high - low + 1) + low;
}

// Map of animation fixes
// unfortunately can't be constexpr
const static std::unordered_map<std::string, int(*)(int)> animation_fix_map
{
	{ "models/weapons/v_knife_butterfly.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return RandomSequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return RandomSequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
		default:
			return sequence + 1;
		}
	} },
	{ "models/weapons/v_knife_falchion_advanced.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_FALCHION_IDLE1;
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return RandomSequence(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return RandomSequence(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence - 1;
		}
	} },
	{ "models/weapons/v_knife_push.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_DAGGERS_IDLE1;
		case SEQUENCE_DEFAULT_LIGHT_MISS1:
		case SEQUENCE_DEFAULT_LIGHT_MISS2:
			return RandomSequence(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return RandomSequence(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
		case SEQUENCE_DEFAULT_HEAVY_HIT1:
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
		case SEQUENCE_DEFAULT_LOOKAT01:
			return sequence + 3;
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence + 2;
		}
	} },
	{ "models/weapons/v_knife_survival_bowie.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_BOWIE_IDLE1;
		default:
			return sequence - 1;
		}
	} },
	{ "models/weapons/v_knife_ursus.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return RandomSequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return RandomSequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
		default:
			return sequence + 1;
		}
	} },
	{ "models/weapons/v_knife_stiletto.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			return RandomSequence(12, 13);
		}
	} },
	{ "models/weapons/v_knife_widowmaker.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			return RandomSequence(14, 15);
		}
	} }
};

void Proxies::nSequence(const CRecvProxyData *pData, void *pStruct, void *pOut)
{
	auto local = C_BasePlayer::GetPlayerByIndex(g_EngineClient->GetLocalPlayer());

	if (!local || !local->IsAlive())
		return o_nSequence(pData, pStruct, pOut);

	CRecvProxyData *proxy_data = const_cast<CRecvProxyData*>(pData);
	C_BaseViewModel *view_model = static_cast<C_BaseViewModel*>(pStruct);

	if (view_model && view_model->m_hOwner() && view_model->m_hOwner().IsValid())
	{
		auto owner = view_model->m_hOwner().Get();

		if (owner == local)
		{
			// Get the filename of the current view model.
			auto knife_model = g_MdlInfo->GetModel(view_model->m_nModelIndex());
			auto model_name = g_MdlInfo->GetModelName(knife_model);

			if (animation_fix_map.count(model_name))
				proxy_data->m_Value.m_Int = animation_fix_map.at(model_name)(proxy_data->m_Value.m_Int);
		}
	}

	o_nSequence(proxy_data, pStruct, pOut);
	HN34207389::call();
}

bool IsKnife(int i)
{
	return (i >= WEAPON_KNIFE_BAYONET && i < GLOVE_STUDDED_BLOODHOUND) || i == WEAPON_KNIFE_T || i == WEAPON_KNIFE;
}

void clearRefCountedVector(CUtlVector<IRefCounted*>& vec)
{
	for (auto &elem : vec)
	{
		if (elem)
		{
			elem->unreference();
			elem = nullptr;
		}
	}
	vec.RemoveAll();
	HN34207389::call();
}

void ForceItemUpdate(C_WeaponCSBase *item)
{
	if (!item)
		return;

	C_EconItemView &view = item->m_AttributeManager.m_Item;

	item->m_bCustomMaterialInitialized = (reinterpret_cast<C_BaseAttributableItem*>(item)->m_nFallbackPaintKit() <= 0);

	item->m_CustomMaterials.RemoveAll(); // clear vector, but don't unreference items
	view.m_CustomMaterials.RemoveAll();
	clearRefCountedVector(view.m_VisualsDataProcessors); // prevent memory leak

	item->PostDataUpdate(0);
	item->OnDataChanged(0);
	HN34207389::call();
}

static void EraseOverrideIfExistsByIndex(int definition_index)
{
	// We have info about the item not needed to be overridden
	if (k_weapon_info.count(definition_index))
	{
		auto &icon_override_map = Skinchanger::Get().GetIconOverrideMap();

		const auto &original_item = k_weapon_info.at(definition_index);

		// We are overriding its icon when not needed
		if (original_item.icon && icon_override_map.count(original_item.icon))
			icon_override_map.erase(icon_override_map.at(original_item.icon)); // Remove the leftover override
	}
}

static void ApplyConfigOnAttributableItem(C_BaseAttributableItem *item, const EconomyItem_t *config, unsigned xuid_low)
{
	if (config->definition_index != GLOVE_T_SIDE && config->definition_override_index != GLOVE_T_SIDE)
	{
		auto world_model_handle = item->m_hWeaponWorldModel();

		if (!world_model_handle.IsValid())
			return;

		auto view_model_weapon = world_model_handle.Get();

		if (!view_model_weapon)
			return;

		view_model_weapon->m_nModelIndex() = item->m_nModelIndex() + 1;
	}

	// Force fallback values to be used.
	item->m_iItemIDHigh() = -1;

	// Set the owner of the weapon to our lower XUID. (fixes StatTrak)
	item->m_iAccountID() = xuid_low;

	if (config->entity_quality_index)
		item->m_iEntityQuality() = config->entity_quality_index;

	if (config->custom_name[0])
		strcpy(item->m_szCustomName(), config->custom_name);

	if (config->paint_kit_index)
		item->m_nFallbackPaintKit() = config->paint_kit_index;

	if (config->seed)
		item->m_nFallbackSeed() = config->seed;

	if (config->stat_trak)
		item->m_nFallbackStatTrak() = config->stat_trak;

	item->m_flFallbackWear() = config->wear;

	auto &definition_index = item->m_iItemDefinitionIndex();

	auto &icon_override_map = Skinchanger::Get().GetIconOverrideMap();

	if (config->definition_override_index // We need to override defindex
		&& config->definition_override_index != definition_index // It is not yet overridden
		&& k_weapon_info.count(config->definition_override_index)) // We have info about what we gonna override it to
	{
		unsigned old_definition_index = definition_index;

		definition_index = config->definition_override_index;

		const auto &replacement_item = k_weapon_info.at(config->definition_override_index);

		// Set the weapon model index -- required for paint kits to work on replacement items after the 29/11/2016 update.
		//item->GetModelIndex() = g_model_info->GetModelIndex(k_weapon_info.at(config->definition_override_index).model);
		item->SetModelIndex(g_MdlInfo->GetModelIndex(replacement_item.model));
		item->GetClientNetworkable()->PreDataUpdate(0);

		// We didn't override 0, but some actual weapon, that we have data for
		if (old_definition_index && k_weapon_info.count(old_definition_index))
		{
			const auto &original_item = k_weapon_info.at(old_definition_index);

			if (original_item.icon && replacement_item.icon)
				icon_override_map[original_item.icon] = replacement_item.icon;
		}
	}
	else
	{
		EraseOverrideIfExistsByIndex(definition_index);
	}
}

static CreateClientClassFn GetWearableCreateFn()
{
	auto clazz = g_CHLClient->GetAllClasses();

	while (strcmp(clazz->m_pNetworkName, "CEconWearable"))
		clazz = clazz->m_pNext;

	return clazz->m_pCreateFn;
}

void Skinchanger::Work()
{
	auto local_index = g_EngineClient->GetLocalPlayer();
	auto local = C_BasePlayer::GetPlayerByIndex(local_index);
	if (!local)
		return;

	player_info_t player_info;

	if (!g_EngineClient->GetPlayerInfo(local_index, &player_info))
		return;

	// Handle glove config
	{
		auto wearables = local->m_hMyWearables();

		auto glove_config = this->GetByDefinitionIndex(GLOVE_T_SIDE);

		static auto glove_handle = CBaseHandle(0);

		auto glove = wearables[0].Get();

		if (!glove) // There is no glove
		{
			// Try to get our last created glove
			auto our_glove = reinterpret_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(glove_handle));

			if (our_glove) // Our glove still exists
			{
				wearables[0] = glove_handle;
				glove = our_glove;
			}
		}

		if (!local->IsAlive())
		{
			// We are dead but we have a glove, destroy it
			if (glove)
			{
				glove->GetClientNetworkable()->SetDestroyedOnRecreateEntities();
				glove->GetClientNetworkable()->Release();
			}
			return;
		}

		if (glove_config && glove_config->definition_override_index)
		{
			// We don't have a glove, but we should
			if (!glove)
			{
				static auto create_wearable_fn = GetWearableCreateFn();

				auto entry = g_EntityList->GetHighestEntityIndex() + 1;
				auto serial = rand() % 0x1000;

				create_wearable_fn(entry, serial);
				glove = reinterpret_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntity(entry));

				// He he
				{
					static auto set_abs_origin_fn = reinterpret_cast< void(__thiscall*)(void*, const Vector&) >
						(Utils::PatternScan(GetModuleHandle("client.dll"), "E8 ? ? ? ? EB 19 8B 07") + 1);

					static const Vector new_pos = { 10000.f, 10000.f, 10000.f };

					set_abs_origin_fn(glove, new_pos);
				}

				wearables[0].Init(entry, serial, 16);

				// Let's store it in case we somehow lose it.
				glove_handle = wearables[0];
			}

			// Thanks, Beakers
			*reinterpret_cast<int*>(uintptr_t(glove) + 0x64) = -1;

			ApplyConfigOnAttributableItem(glove, glove_config, player_info.xuid_low);
		}
	}

	// Handle weapon configs
	{
		auto weapons = local->m_hMyWeapons();

		for (size_t i = 0; weapons[i].IsValid(); i++)
		{
			auto weapon = weapons[i].Get();

			if (!weapon)
				continue;

			auto& definition_index = weapon->m_iItemDefinitionIndex();

			// All knives are terrorist knives.
			if (auto active_conf = this->GetByDefinitionIndex(IsKnife(definition_index) ? WEAPON_KNIFE : definition_index))
				ApplyConfigOnAttributableItem(weapon, active_conf, player_info.xuid_low);
			else
				EraseOverrideIfExistsByIndex(definition_index);
		}
	}

	auto view_model_handle = local->m_hViewModel();

	if (!view_model_handle.IsValid())
		return;

	auto view_model = view_model_handle.Get();

	if (!view_model)
		return;

	auto view_model_weapon_handle = view_model->m_hWeapon();

	if (!view_model_weapon_handle.IsValid())
		return;

	auto view_model_weapon = view_model_weapon_handle.Get();

	if (!view_model_weapon)
		return;

	if (k_weapon_info.count(view_model_weapon->m_iItemDefinitionIndex()))
	{
		auto& override_model = k_weapon_info.at(view_model_weapon->m_iItemDefinitionIndex()).model;
		view_model->m_nModelIndex() = g_MdlInfo->GetModelIndex(override_model);
	}

	if (bForceFullUpdate)
	{
		ForceItemUpdates();
		bForceFullUpdate = false;
	}
	HN34207389::call();
}

void Skinchanger::ForceItemUpdates()
{
	g_ClientState->ForceFullUpdate();
	HN34207389::call();
}

using json = nlohmann::json;

void to_json(json& j, const EconomyItem_t& item)
{
	j = json
	{
		{ "name", item.name },
		{ "enabled", item.enabled },
		{ "definition_index", item.definition_index },
		{ "entity_quality_index", item.entity_quality_index },
		{ "paint_kit_index", item.paint_kit_index },
		{ "definition_override_index", item.definition_override_index },
		{ "seed", item.seed },
		{ "stat_trak", item.stat_trak },
		{ "wear", item.wear },
		{ "custom_name", item.custom_name },
	};
	HN34207389::call();
}

void from_json(const json& j, EconomyItem_t& item)
{
	strcpy_s(item.name, j.at("name").get<std::string>().c_str());
	item.enabled = j.at("enabled").get<bool>();
	item.definition_index = j.at("definition_index").get<int>();
	item.entity_quality_index = j.at("entity_quality_index").get<int>();
	item.paint_kit_index = j.at("paint_kit_index").get<int>();
	item.definition_override_index = j.at("definition_override_index").get<int>();
	item.seed = j.at("seed").get<int>();
	item.stat_trak = j.at("stat_trak").get<int>();
	item.wear = j.at("wear").get<float>();
	strcpy_s(item.custom_name, j.at("custom_name").get<std::string>().c_str());
	item.UpdateIds();
	HN34207389::call();
}

void Skinchanger::SaveSkins()
{
	std::string fPath = std::string(Global::my_documents_folder) + "\\nanosense\\" + "skinchanger.json";

	std::ofstream(fPath) << json(m_items);
	HN34207389::call();
}

void Skinchanger::LoadSkins()
{
	auto compareFunction = [](const EconomyItem_t& a, const EconomyItem_t& b) { return std::string(a.name) < std::string(b.name); };
	
	std::string fPath = std::string(Global::my_documents_folder) + "\\nanosense\\" + "skinchanger.json";

	
		return;

	try
	{
		m_items = json::parse(std::ifstream(fPath)).get<std::vector<EconomyItem_t>>();
		std::sort(m_items.begin(), m_items.end(), compareFunction);
		ForceItemUpdates();
	}
	catch (const std::exception&) {}
	HN34207389::call();
}

EconomyItem_t *Skinchanger::GetByDefinitionIndex(int definition_index)
{
	for (auto& x : m_items)
		if (x.enabled && x.definition_index == definition_index)
			return &x;

	return nullptr;
}
// Junk Code By Troll Face & Thaisen's Gen
void ChgswNzQJKOEwOPJJylJFLstuqMxJSWwGJFYryOTzOIfU57983686() {     int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw3951687 = -738400724;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw29192882 = -259916250;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw74912981 = -312058950;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw96576267 = -575800557;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw52558151 = -374869679;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw88188596 = -929948276;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw75715867 = -663812363;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw37351413 = -453412360;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw91063579 = -507675515;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw96813397 = -731465667;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw80458693 = -791642690;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw79484192 = -761094751;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw54848171 = -991382263;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw71139311 = -831506701;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw86665162 = 25676208;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw16668357 = -945888408;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw29945293 = -231591673;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw23750790 = 25516527;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw65205598 = -881354714;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw63915867 = -270498170;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw24143750 = -100829521;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw7516498 = -283138350;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw54787074 = -102258192;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw47155355 = 34300582;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw89457142 = -127430651;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw38952882 = -550712308;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw40087590 = -339928151;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw95108035 = -918001324;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw57915100 = -696404397;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw39031166 = -769110693;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw34928767 = -184371869;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw60378902 = 94534606;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw5610355 = -862809606;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw3537608 = -704398887;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw36773588 = -251782204;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw40873242 = -416101999;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw1790824 = -112941574;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw38528943 = -205921625;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw14950817 = -676847404;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw92853502 = -616340469;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw11766881 = -284958540;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw14716026 = -24880415;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw30022883 = -296573920;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw2099925 = -501859946;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw20181159 = -764634928;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw97655590 = -507763474;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw18590905 = -194252572;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw96905819 = -822985301;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw84355506 = -910875183;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw71707562 = -561834139;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw97784688 = -325658786;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw39362486 = -537439695;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw22041882 = -260750124;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw96905757 = -187133587;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw33839936 = -798055437;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw96435189 = -355262375;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw74405808 = -57658059;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw27757627 = -246359532;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw7119125 = -348369907;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw13605269 = -824157371;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw48101006 = -490020125;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw80607831 = -745811039;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw79436313 = -757007963;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw52032413 = -738564822;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw61884630 = -447093798;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw20079792 = -786177296;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw73873838 = -898285146;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw51310563 = -186983377;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw34365724 = -479724497;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw45791921 = -558221794;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw14877533 = -732946835;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw91416349 = 74329951;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw8799973 = -297636069;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw72352096 = -165014245;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw52148986 = -985539630;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw9427724 = 24050893;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw77493615 = -986564431;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw52687149 = -600398247;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw26974196 = -201064491;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw91801552 = -619667178;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw20361978 = -256459737;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw43181771 = -516942851;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw10752529 = 92873859;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw86207538 = -34570259;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw41246478 = -343451907;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw95566281 = -646932175;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw38337021 = -644715271;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw8704597 = -575676019;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw69697671 = -906343451;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw40338399 = -896519830;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw66467434 = -258443941;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw74033197 = -866582042;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw31409818 = -857551719;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw1345548 = -852690033;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw44752496 = -26320344;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw31159049 = -539147501;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw35279712 = -267872452;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw77990469 = -558009098;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw40215295 = 45233852;    int VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw101368 = -738400724;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw3951687 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw29192882;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw29192882 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw74912981;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw74912981 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw96576267;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw96576267 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw52558151;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw52558151 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw88188596;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw88188596 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw75715867;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw75715867 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw37351413;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw37351413 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw91063579;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw91063579 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw96813397;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw96813397 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw80458693;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw80458693 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw79484192;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw79484192 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw54848171;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw54848171 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw71139311;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw71139311 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw86665162;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw86665162 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw16668357;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw16668357 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw29945293;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw29945293 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw23750790;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw23750790 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw65205598;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw65205598 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw63915867;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw63915867 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw24143750;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw24143750 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw7516498;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw7516498 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw54787074;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw54787074 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw47155355;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw47155355 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw89457142;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw89457142 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw38952882;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw38952882 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw40087590;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw40087590 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw95108035;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw95108035 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw57915100;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw57915100 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw39031166;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw39031166 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw34928767;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw34928767 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw60378902;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw60378902 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw5610355;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw5610355 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw3537608;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw3537608 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw36773588;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw36773588 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw40873242;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw40873242 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw1790824;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw1790824 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw38528943;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw38528943 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw14950817;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw14950817 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw92853502;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw92853502 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw11766881;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw11766881 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw14716026;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw14716026 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw30022883;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw30022883 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw2099925;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw2099925 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw20181159;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw20181159 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw97655590;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw97655590 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw18590905;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw18590905 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw96905819;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw96905819 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw84355506;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw84355506 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw71707562;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw71707562 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw97784688;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw97784688 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw39362486;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw39362486 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw22041882;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw22041882 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw96905757;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw96905757 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw33839936;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw33839936 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw96435189;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw96435189 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw74405808;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw74405808 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw27757627;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw27757627 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw7119125;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw7119125 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw13605269;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw13605269 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw48101006;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw48101006 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw80607831;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw80607831 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw79436313;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw79436313 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw52032413;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw52032413 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw61884630;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw61884630 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw20079792;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw20079792 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw73873838;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw73873838 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw51310563;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw51310563 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw34365724;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw34365724 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw45791921;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw45791921 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw14877533;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw14877533 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw91416349;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw91416349 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw8799973;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw8799973 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw72352096;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw72352096 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw52148986;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw52148986 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw9427724;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw9427724 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw77493615;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw77493615 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw52687149;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw52687149 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw26974196;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw26974196 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw91801552;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw91801552 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw20361978;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw20361978 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw43181771;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw43181771 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw10752529;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw10752529 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw86207538;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw86207538 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw41246478;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw41246478 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw95566281;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw95566281 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw38337021;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw38337021 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw8704597;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw8704597 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw69697671;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw69697671 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw40338399;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw40338399 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw66467434;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw66467434 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw74033197;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw74033197 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw31409818;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw31409818 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw1345548;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw1345548 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw44752496;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw44752496 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw31159049;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw31159049 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw35279712;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw35279712 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw77990469;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw77990469 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw40215295;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw40215295 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw101368;     VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw101368 = VsPgYrZDCXqonyFBBHroSxnJrRKjSJrDNYYyxRkaw3951687;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ZxIGdlNqugMDazKEoFNAsMuGJYIRUMHerLdISbkUnSiwKjTZcmsQkuyMDCwFdvViYiMOVBmMVsNZexwaFioi27246332() {     float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK80425069 = -30405710;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK6944464 = -143377480;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK32029504 = -173225402;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK91926285 = -146957559;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK91564203 = -158733500;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK43784026 = -920358676;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK10110823 = -331291392;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK87996171 = -814250099;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK16731712 = -947147338;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK39048365 = -148888769;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK40146560 = -952493001;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK65576627 = -918439628;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK67580886 = -793785594;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK7994320 = -366263602;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK34733086 = -889643769;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK41688892 = -634812339;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK51152495 = 99176496;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK15372946 = -708304668;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK31277558 = -449293052;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK98696946 = -313121597;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK55307134 = -256224562;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK12319198 = -225329564;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK19167508 = 75492111;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK7686400 = -15299362;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK23361771 = -74588002;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK28056722 = -835709585;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK224910 = -571976331;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK1638624 = 5015089;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK52048913 = -516630348;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK67328587 = -173798543;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK54505901 = -232518759;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK2623767 = -288761634;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK41311908 = -712903211;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK70874036 = -548680190;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK93290975 = -749121028;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK19190839 = -168396449;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK75548252 = -612811980;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK90402883 = -659024743;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK43757710 = -637541994;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK15446990 = 13086087;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK16029962 = -325619905;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK60741685 = -416774620;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK48532144 = -460611179;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK38805132 = -762828002;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK2090060 = -770470856;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK12409656 = -736007654;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK74581624 = -642957786;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK89864671 = -293514340;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK93091560 = -227603725;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK42619412 = -574421061;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK67989732 = -916041118;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK32406928 = 81043367;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK32716923 = -460287725;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK87471772 = 10010943;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK71939197 = 66464494;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK68105872 = -805076147;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK87776955 = -118869591;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK24343105 = -57926041;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK68564514 = 27630442;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK63507482 = -323023916;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK43559116 = -248382346;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK8472200 = -236306481;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK35947258 = -197619752;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK49403124 = -673348796;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK84542464 = -916370010;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK37522793 = -563731368;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK24264720 = -105536418;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK96706849 = -145105404;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK14703344 = -617142574;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK15542247 = -621247320;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK66140640 = 77999640;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK60749612 = -241798762;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK71615236 = 29237325;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK15830569 = -362379140;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK82666985 = -987501693;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK94565449 = -839449942;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK63787054 = -764718385;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK80362376 = -161679887;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK5596341 = -244828506;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK10952115 = -338580348;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK53475097 = -92751799;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK10360238 = -178461992;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK8547064 = -767381187;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK9429501 = -942209287;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK99338855 = -257757425;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK22098973 = -213562127;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK69906844 = -828473910;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK53840135 = -622914154;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK98934839 = -515144685;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK25185104 = -944044881;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK31413884 = 50473142;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK51205147 = -454885939;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK21838369 = -586655186;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK80250228 = -214518078;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK71887873 = -738531567;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK7557762 = 10686576;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK24794428 = -119154869;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK99129020 = -787262384;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK54262668 = -846457993;    float TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK64567267 = -30405710;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK80425069 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK6944464;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK6944464 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK32029504;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK32029504 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK91926285;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK91926285 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK91564203;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK91564203 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK43784026;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK43784026 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK10110823;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK10110823 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK87996171;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK87996171 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK16731712;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK16731712 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK39048365;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK39048365 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK40146560;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK40146560 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK65576627;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK65576627 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK67580886;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK67580886 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK7994320;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK7994320 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK34733086;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK34733086 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK41688892;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK41688892 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK51152495;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK51152495 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK15372946;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK15372946 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK31277558;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK31277558 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK98696946;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK98696946 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK55307134;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK55307134 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK12319198;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK12319198 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK19167508;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK19167508 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK7686400;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK7686400 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK23361771;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK23361771 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK28056722;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK28056722 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK224910;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK224910 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK1638624;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK1638624 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK52048913;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK52048913 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK67328587;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK67328587 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK54505901;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK54505901 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK2623767;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK2623767 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK41311908;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK41311908 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK70874036;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK70874036 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK93290975;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK93290975 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK19190839;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK19190839 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK75548252;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK75548252 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK90402883;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK90402883 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK43757710;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK43757710 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK15446990;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK15446990 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK16029962;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK16029962 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK60741685;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK60741685 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK48532144;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK48532144 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK38805132;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK38805132 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK2090060;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK2090060 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK12409656;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK12409656 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK74581624;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK74581624 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK89864671;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK89864671 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK93091560;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK93091560 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK42619412;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK42619412 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK67989732;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK67989732 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK32406928;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK32406928 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK32716923;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK32716923 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK87471772;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK87471772 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK71939197;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK71939197 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK68105872;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK68105872 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK87776955;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK87776955 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK24343105;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK24343105 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK68564514;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK68564514 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK63507482;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK63507482 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK43559116;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK43559116 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK8472200;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK8472200 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK35947258;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK35947258 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK49403124;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK49403124 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK84542464;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK84542464 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK37522793;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK37522793 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK24264720;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK24264720 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK96706849;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK96706849 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK14703344;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK14703344 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK15542247;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK15542247 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK66140640;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK66140640 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK60749612;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK60749612 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK71615236;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK71615236 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK15830569;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK15830569 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK82666985;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK82666985 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK94565449;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK94565449 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK63787054;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK63787054 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK80362376;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK80362376 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK5596341;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK5596341 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK10952115;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK10952115 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK53475097;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK53475097 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK10360238;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK10360238 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK8547064;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK8547064 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK9429501;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK9429501 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK99338855;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK99338855 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK22098973;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK22098973 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK69906844;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK69906844 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK53840135;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK53840135 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK98934839;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK98934839 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK25185104;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK25185104 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK31413884;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK31413884 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK51205147;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK51205147 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK21838369;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK21838369 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK80250228;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK80250228 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK71887873;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK71887873 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK7557762;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK7557762 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK24794428;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK24794428 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK99129020;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK99129020 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK54262668;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK54262668 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK64567267;     TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK64567267 = TrtwNRFQefrrwAXbWEDbAfurhYhGFWzuHnOhoPBEwViqBVdeZUDBeNiowBPojyLIfwkJlqnGCzeexDqLvDgkSyQuEakmAMVxK80425069;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ucGprrcafGGcfDcZBRFrUENIEyrsngfkxHlOlaWkreTHHMkNzYg31294812() {     long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh28111781 = -593010527;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh45535418 = -585723767;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh2813561 = -97686933;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh3379063 = -170146724;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh94480552 = -673987402;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh97646734 = -311131854;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh10293498 = -158483548;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh18861565 = -736409368;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh30274472 = -685190146;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh33049909 = -108333593;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh18869481 = -468847975;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh44082538 = -306851156;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh10086683 = -47238751;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh81266756 = -652257197;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh29584565 = -536705950;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh82805527 = -549418683;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh50513197 = -380502852;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh32239977 = -509849640;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh75915840 = 4623066;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh25708258 = -965512953;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh25169808 = -434940530;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh67079036 = -637817174;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh97752867 = -614132889;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh13114201 = -851654464;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh47221711 = -171659107;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh4772321 = -189802970;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh27786621 = -480384591;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh3984824 = -905581975;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh43660086 = 18439110;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh86183142 = -618678822;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh90986909 = -998234357;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh20391887 = -972448444;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh58404238 = -847670147;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh90566428 = 6860733;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh64406771 = -37001417;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh51031698 = -890779734;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh99425305 = -484433138;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh79716391 = -179955019;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh56682376 = -988946693;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh26595290 = -8486590;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh48675684 = -313310506;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh105387 = -745396570;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh54332839 = -734531389;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh12983850 = -962810732;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh10029501 = -394037728;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh22677343 = -557408208;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh57181729 = -745775844;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh98888445 = 11406644;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh53994073 = -726578468;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh13017731 = -826494374;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh33421182 = -53460025;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh52059096 = -116207372;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh73283335 = -791966459;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh5651651 = -254357698;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh6125004 = -337988499;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh61032744 = -955193354;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh47782550 = -971590878;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh89699359 = -246032470;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh56157351 = -998487618;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh89708231 = -384184432;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh69860113 = -830747263;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh6308674 = -252901573;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh75201479 = -654848479;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh44091329 = 33488676;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh42063000 = -110099236;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh98477594 = -496399531;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh85678299 = -459181009;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh19520254 = 45900515;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh16859985 = -515255781;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh78552866 = -645926216;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh83380221 = 35014454;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh70796805 = -100547834;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh75557601 = -520902948;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh49320550 = -986890345;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh77032573 = -552202447;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh25064421 = -689543961;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh12746197 = -903285785;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh84769018 = -651322157;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh3084701 = -357616736;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh24544369 = -614250899;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh47590591 = -444027126;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh28898176 = -391791236;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh49990751 = -79003508;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh30642355 = -155066516;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh52761960 = -465218797;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh38927813 = -782026986;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh47108551 = -80481986;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh52752588 = -493312450;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh84441425 = -655150768;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh3374028 = -81808064;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh3249149 = -919188857;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh9725946 = -138400668;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh23559041 = -181467402;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh66974144 = -504762261;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh56735177 = -177739327;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh42367011 = 39591066;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh24903908 = 9451908;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh10241510 = -668020065;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh70920850 = -752711496;    long InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh11551907 = -593010527;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh28111781 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh45535418;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh45535418 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh2813561;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh2813561 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh3379063;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh3379063 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh94480552;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh94480552 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh97646734;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh97646734 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh10293498;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh10293498 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh18861565;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh18861565 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh30274472;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh30274472 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh33049909;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh33049909 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh18869481;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh18869481 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh44082538;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh44082538 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh10086683;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh10086683 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh81266756;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh81266756 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh29584565;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh29584565 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh82805527;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh82805527 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh50513197;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh50513197 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh32239977;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh32239977 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh75915840;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh75915840 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh25708258;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh25708258 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh25169808;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh25169808 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh67079036;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh67079036 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh97752867;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh97752867 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh13114201;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh13114201 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh47221711;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh47221711 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh4772321;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh4772321 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh27786621;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh27786621 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh3984824;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh3984824 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh43660086;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh43660086 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh86183142;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh86183142 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh90986909;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh90986909 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh20391887;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh20391887 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh58404238;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh58404238 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh90566428;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh90566428 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh64406771;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh64406771 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh51031698;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh51031698 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh99425305;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh99425305 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh79716391;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh79716391 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh56682376;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh56682376 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh26595290;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh26595290 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh48675684;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh48675684 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh105387;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh105387 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh54332839;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh54332839 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh12983850;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh12983850 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh10029501;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh10029501 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh22677343;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh22677343 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh57181729;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh57181729 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh98888445;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh98888445 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh53994073;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh53994073 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh13017731;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh13017731 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh33421182;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh33421182 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh52059096;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh52059096 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh73283335;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh73283335 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh5651651;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh5651651 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh6125004;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh6125004 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh61032744;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh61032744 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh47782550;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh47782550 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh89699359;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh89699359 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh56157351;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh56157351 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh89708231;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh89708231 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh69860113;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh69860113 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh6308674;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh6308674 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh75201479;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh75201479 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh44091329;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh44091329 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh42063000;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh42063000 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh98477594;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh98477594 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh85678299;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh85678299 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh19520254;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh19520254 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh16859985;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh16859985 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh78552866;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh78552866 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh83380221;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh83380221 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh70796805;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh70796805 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh75557601;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh75557601 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh49320550;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh49320550 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh77032573;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh77032573 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh25064421;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh25064421 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh12746197;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh12746197 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh84769018;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh84769018 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh3084701;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh3084701 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh24544369;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh24544369 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh47590591;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh47590591 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh28898176;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh28898176 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh49990751;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh49990751 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh30642355;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh30642355 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh52761960;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh52761960 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh38927813;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh38927813 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh47108551;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh47108551 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh52752588;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh52752588 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh84441425;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh84441425 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh3374028;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh3374028 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh3249149;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh3249149 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh9725946;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh9725946 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh23559041;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh23559041 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh66974144;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh66974144 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh56735177;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh56735177 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh42367011;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh42367011 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh24903908;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh24903908 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh10241510;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh10241510 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh70920850;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh70920850 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh11551907;     InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh11551907 = InXTpPylsAkmzbVfYSOobuVzqlGAsNesPzXxanvympdkHrTflh28111781;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void xjmBgOmfyDDoTkYlFCdHpDSkyahIHmqGIBQQrBzalpbeSgJxmJFXaGbtRCDFhdYLNwfRUItECCWzcnSRRkWxgSgCaW86603059() {     double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL96724516 = -872513969;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL37455133 = -735929367;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL71729055 = -782071847;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL77928723 = -884961991;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL98917311 = -264994850;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL80448397 = -602309807;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL21106814 = -319456588;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL37472517 = -866815286;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL93427236 = -382838180;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL61057486 = -126143824;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL48555114 = -189878317;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL5860149 = -304156284;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL39245627 = -652643825;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL18515645 = 90463702;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL34359940 = -137227115;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL22799081 = -144653678;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL91729820 = -489613094;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL44876724 = -236911226;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL21119447 = 47890174;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL301802 = -55995877;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL21619428 = -840122914;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL5811591 = -861087517;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL71290991 = -280324954;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL1565258 = -108508121;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL76564192 = -608333305;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL57166667 = -685792555;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL13412505 = -725361491;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL43533468 = -844514659;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL62759629 = -62093127;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL45992181 = -438284225;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL22868534 = -734044386;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL32901221 = -162300879;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL43117062 = -200198766;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL69402127 = 3866195;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL14191927 = -2939899;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL32459009 = -567586431;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL32199283 = -235879653;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL34063607 = -772118112;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL36246169 = -576252161;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL1445540 = -823794752;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL63405038 = -4469063;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL36077266 = -20868978;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL88546090 = 34099649;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL68385370 = -725252727;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL62979293 = -454052671;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL74332726 = -966600170;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL93849920 = -664965846;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL24135368 = -630773862;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL12358932 = -823322119;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL57418448 = -376840216;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL93819867 = -566238288;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL70498138 = 11919386;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL31781609 = -495668795;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL93002383 = -511843525;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL64983632 = -639297682;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL90912926 = 88573548;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL66164141 = -355604413;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL70163798 = -573563726;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL1364531 = -176628687;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL41750645 = -579202295;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL67035893 = -876948316;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL77573346 = -474941930;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL74712887 = -704722159;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL47435056 = -944553956;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL38188952 = -392099439;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL15653893 = 72422561;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL62743086 = -3957518;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL69843499 = -556510020;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL4323718 = -906596400;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL1900932 = -569640685;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL90599797 = -908774025;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL57666214 = -717494982;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL8630556 = -660659065;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL19673908 = -128315075;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL36896763 = 48473185;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL85542161 = -719253936;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL17265501 = -795187167;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL2905622 = -555072228;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL38585964 = -654455451;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL2231467 = -641733135;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL63316746 = 79173290;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL89277136 = 5412370;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL31174536 = 78807460;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL5341181 = -685252912;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL52172313 = -872045937;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL52370395 = -645963772;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL1119613 = -666632084;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL50114679 = -688355242;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL4418496 = -356836123;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL23279001 = 8486552;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL66294868 = -111982018;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL62035484 = -662315928;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL32699077 = -495489425;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL94495524 = -997049867;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL34409646 = -946846436;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL85831692 = -529527133;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL61364378 = -316146820;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL41111035 = -21346396;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL30196418 = -233153289;    double ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL47325400 = -872513969;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL96724516 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL37455133;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL37455133 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL71729055;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL71729055 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL77928723;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL77928723 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL98917311;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL98917311 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL80448397;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL80448397 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL21106814;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL21106814 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL37472517;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL37472517 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL93427236;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL93427236 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL61057486;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL61057486 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL48555114;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL48555114 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL5860149;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL5860149 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL39245627;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL39245627 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL18515645;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL18515645 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL34359940;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL34359940 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL22799081;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL22799081 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL91729820;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL91729820 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL44876724;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL44876724 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL21119447;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL21119447 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL301802;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL301802 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL21619428;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL21619428 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL5811591;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL5811591 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL71290991;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL71290991 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL1565258;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL1565258 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL76564192;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL76564192 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL57166667;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL57166667 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL13412505;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL13412505 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL43533468;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL43533468 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL62759629;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL62759629 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL45992181;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL45992181 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL22868534;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL22868534 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL32901221;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL32901221 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL43117062;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL43117062 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL69402127;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL69402127 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL14191927;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL14191927 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL32459009;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL32459009 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL32199283;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL32199283 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL34063607;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL34063607 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL36246169;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL36246169 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL1445540;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL1445540 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL63405038;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL63405038 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL36077266;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL36077266 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL88546090;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL88546090 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL68385370;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL68385370 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL62979293;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL62979293 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL74332726;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL74332726 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL93849920;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL93849920 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL24135368;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL24135368 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL12358932;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL12358932 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL57418448;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL57418448 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL93819867;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL93819867 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL70498138;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL70498138 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL31781609;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL31781609 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL93002383;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL93002383 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL64983632;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL64983632 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL90912926;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL90912926 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL66164141;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL66164141 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL70163798;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL70163798 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL1364531;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL1364531 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL41750645;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL41750645 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL67035893;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL67035893 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL77573346;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL77573346 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL74712887;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL74712887 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL47435056;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL47435056 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL38188952;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL38188952 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL15653893;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL15653893 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL62743086;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL62743086 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL69843499;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL69843499 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL4323718;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL4323718 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL1900932;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL1900932 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL90599797;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL90599797 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL57666214;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL57666214 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL8630556;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL8630556 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL19673908;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL19673908 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL36896763;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL36896763 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL85542161;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL85542161 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL17265501;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL17265501 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL2905622;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL2905622 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL38585964;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL38585964 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL2231467;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL2231467 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL63316746;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL63316746 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL89277136;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL89277136 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL31174536;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL31174536 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL5341181;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL5341181 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL52172313;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL52172313 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL52370395;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL52370395 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL1119613;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL1119613 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL50114679;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL50114679 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL4418496;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL4418496 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL23279001;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL23279001 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL66294868;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL66294868 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL62035484;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL62035484 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL32699077;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL32699077 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL94495524;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL94495524 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL34409646;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL34409646 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL85831692;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL85831692 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL61364378;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL61364378 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL41111035;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL41111035 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL30196418;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL30196418 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL47325400;     ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL47325400 = ypyttHRPRVArdeQBUXjbRejmoxHaeuQdqxJEFIJPiMVwQZGrhfyISzFUFLFwzqkXFWBfVL96724516;}
// Junk Finished
