#include "../SDK.hpp"

#include "Config.hpp"
#include "json.hpp"

#include "../Options.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>
// hack

nlohmann::json config;



void Config::SaveConfig(const std::string path)
{
	std::ofstream output_file(path);

	if (!output_file.good())
		return;

	
	Save(g_Options.legit_drawfov, "legitdrawfov");
	Save(g_Options.rage_fieldofview, "ragefov");
	Save(g_Options.indicators_enabled, "aaindicators");
	Save(g_Options.zeusrange_enabled, "zeusrange");
	Save(g_Options.esp_flags, "playerflags");
	Save(g_Options.fullbright, "fullbright");
	Save(g_Options.misc_thirdperson_distance, "tpdistance");
	Save(g_Options.legit_backtrack, "lgtbacktrack");
	Save(g_Options.legit_backtrackticks, "lgtbacktrackticks");
	Save(g_Options.misc_fakeduck, "fakeduckenable");
	Save(g_Options.misc_fakeduck_chokedpackets, "fdchoked");
	Save(g_Options.misc_fakeduck_keybind, "fdkeybind");
	Save(g_Options.legit_aim_keybind1, "lgtkey1");
	Save(g_Options.legit_aim_keybind2, "lgkey2");
	SaveArray(g_Options.glow_others_color, "glowothers");
	Save(g_Options.hvh_antiaim_lby_breaker, "lbybreaker");
	Save(g_Options.michaeljackson, "moonwalking");
	Save(g_Options.rage_nospread, "nospreadfix");
	Save(g_Options.visuals_others_dlight, "dlightz");

	Save(g_Options.misc_autoaccept, "misc_autoaccept");
	Save(g_Options.misc_revealAllRanks, "misc_revealAllRanks");
	Save(g_Options.misc_bhop, "misc_bhop");
	Save(g_Options.misc_autostrafe, "misc_autostrafe");
	Save(g_Options.misc_auto_pistol, "misc_auto_pistol");
	Save(g_Options.misc_chatspamer, "misc_chatspamer");
	Save(g_Options.misc_thirdperson, "misc_thirdperson");
	Save(g_Options.misc_thirdperson_bind, "misc_thirdperson_bind");
	Save(g_Options.misc_fakewalk, "misc_fakewalk");
	Save(g_Options.misc_fakewalk_bind, "misc_fakewalk_bind");
	Save(g_Options.misc_fakelag_enabled, "misc_fakelag_enabled");
	Save(g_Options.misc_fakelag_value, "misc_fakelag_value");
	Save(g_Options.misc_fakelag_activation_type, "misc_fakelag_activation_type");
	Save(g_Options.misc_fakelag_adaptive, "misc_fakelag_adaptive");
	Save(g_Options.misc_animated_clantag, "misc_animated_clantag");
	Save(g_Options.misc_spectatorlist, "misc_spectatorlist");
	Save(g_Options.misc_logevents, "misc_logevents");
	Save(g_Options.hvh_resolver_custom, "hvh_resolver_custom");
	Save(g_Options.resolver_eyelby, "resolver_eyelby");
	Save(g_Options.resolver_eyelby_always, "resolver_eyelby_always");
	Save(g_Options.resolver_eyelby_running, "resolver_eyelby_running");
	Save(g_Options.resolver_eyelby_notrunning, "resolver_eyelby");
	Save(g_Options.resolver_eyelby_running_value, "resolver_eyelby_running_value");
	Save(g_Options.removals_flash, "removals_flash");
	Save(g_Options.removals_smoke, "removals_smoke");
	Save(g_Options.removals_smoke_type, "removals_smoke_type");
	Save(g_Options.removals_scope, "removals_scope");
	Save(g_Options.removals_novisualrecoil, "removals_novisualrecoil");
	Save(g_Options.removals_postprocessing, "removals_postprocessing");
	Save(g_Options.removals_crosshair, "removals_crosshair");
	Save(g_Options.backtrack_bhd_wall_only, "backtrack_bhd_wall_only");
	Save(g_Options.esp_farther, "esp_farther");
	Save(g_Options.esp_fill_amount, "esp_fill_amount");
	SaveArray(g_Options.esp_player_fill_color_t, "esp_player_fill_color_t");
	SaveArray(g_Options.esp_player_fill_color_ct, "esp_player_fill_color_ct");
	SaveArray(g_Options.esp_player_fill_color_t_visible, "esp_player_fill_color_t_visible");
	SaveArray(g_Options.esp_player_fill_color_ct_visible, "esp_player_fill_color_ct_visible");
	Save(g_Options.esp_player_boundstype, "esp_player_boundstype");
	Save(g_Options.esp_player_boxtype, "esp_player_boxtype");
	SaveArray(g_Options.esp_player_bbox_color_t, "esp_player_bbox_color_t");
	SaveArray(g_Options.esp_player_bbox_color_ct, "esp_player_bbox_color_ct");
	SaveArray(g_Options.esp_player_bbox_color_t_visible, "esp_player_bbox_color_t_visible");
	SaveArray(g_Options.esp_player_bbox_color_ct_visible, "esp_player_bbox_color_ct_visible");
	Save(g_Options.esp_player_name, "esp_player_name");
	Save(g_Options.esp_player_health, "esp_player_health");
	Save(g_Options.esp_player_weapons, "esp_player_weapons");
	Save(g_Options.esp_player_snaplines, "esp_player_snaplines");
	Save(g_Options.esp_player_chams, "esp_player_chams");
	Save(g_Options.esp_player_chams_type, "esp_player_chams_type");
	SaveArray(g_Options.esp_player_chams_color_t_visible, "esp_player_chams_color_t_visible");
	SaveArray(g_Options.esp_player_chams_color_ct_visible, "esp_player_chams_color_ct_visible");
	SaveArray(g_Options.esp_player_chams_color_t, "esp_player_chams_color_t");
	SaveArray(g_Options.esp_player_chams_color_ct, "esp_player_chams_color_ct");
	Save(g_Options.esp_player_skelet, "esp_player_skelet");
	Save(g_Options.esp_player_anglelines, "esp_player_anglelines");
	Save(g_Options.esp_dropped_weapons, "esp_dropped_weapons");
	Save(g_Options.esp_planted_c4, "esp_planted_c4");
	Save(g_Options.esp_grenades, "esp_grenades");
	Save(g_Options.esp_grenades_type, "esp_grenades_type");
	Save(g_Options.esp_backtracked_player_skelet, "esp_backtracked_player_skelet");
	Save(g_Options.esp_lagcompensated_hitboxes, "esp_lagcompensated_hitboxes");
	Save(g_Options.esp_lagcompensated_hitboxes_type, "esp_lagcompensated_hitboxes_type");
	Save(g_Options.visuals_manual_aa, "visuals_manual_aa");
	Save(g_Options.visuals_others_player_fov, "visuals_others_player_fov");
	Save(g_Options.visuals_others_player_fov_viewmodel, "visuals_others_player_fov_viewmodel");
	Save(g_Options.watermark, "visuals_others_watermark");
	Save(g_Options.visuals_others_grenade_pred, "visuals_others_grenade_pred");
	Save(g_Options.visuals_others_hitmarker, "visuals_others_hitmarker");
	Save(g_Options.visuals_others_bulletimpacts, "visuals_others_bulletimpacts");
	SaveArray(g_Options.visuals_others_bulletimpacts_color, "visuals_others_bulletimpacts_color");
	Save(g_Options.visuals_others_sky, "visuals_others_sky");
	Save(g_Options.glow_enabled, "glow_enabled");
	Save(g_Options.glow_players, "glow_players");
	SaveArray(g_Options.glow_player_color_t, "glow_player_color_t");
	SaveArray(g_Options.glow_player_color_ct, "glow_player_color_ct");
	SaveArray(g_Options.glow_player_color_t_visible, "glow_player_color_t_visible");
	SaveArray(g_Options.glow_player_color_ct_visible, "glow_player_color_ct_visible");
	Save(g_Options.glow_players_style, "glow_players_style");
	Save(g_Options.glow_others, "glow_others");
	Save(g_Options.glow_others_style, "glow_others_style");
	Save(g_Options.legit_enabled, "legit_enabled");
	Save(g_Options.legit_hsonly, "legit_hsonly");
	Save(g_Options.legit_rcs, "legit_rcs");
	Save(g_Options.legit_trigger, "legit_trigger");
	Save(g_Options.legit_on_press, "legit_on_press");
	Save(g_Options.trigger_on_press, "trigger_on_press");
	Save(g_Options.rcs_on_press, "rcs_on_press");
	Save(g_Options.legit_aim_keybind, "legit_aim_keybind");
	Save(g_Options.rcs_keybind, "rcs_keybind");
	Save(g_Options.trigger_keybind, "trigger_keybind");
	Save(g_Options.legit_preaim, "legit_preaim");
	Save(g_Options.legit_aftershots, "legit_aftershots");
	Save(g_Options.legit_afteraim, "legit_afteraim");
	Save(g_Options.legit_smooth_factor, "legit_smooth_factor");
	Save(g_Options.legit_fov, "legit_fov");
	Save(g_Options.rage_enabled, "rage_enabled");
	Save(g_Options.rage_aimkey, "rage_aimkey");
	Save(g_Options.rage_silent, "rage_silent");
	Save(g_Options.rage_norecoil, "rage_norecoil");
	Save(g_Options.rage_autoshoot, "rage_autoshoot");
	Save(g_Options.rage_autoscope, "rage_autoscope");
	Save(g_Options.rage_autocrouch, "rage_autocrouch");
	Save(g_Options.rage_autostop, "rage_autostop");
	Save(g_Options.rage_autobaim, "rage_autobaim");
	Save(g_Options.visuals_draw_xhair, "visuals_draw_xhair");
	Save(g_Options.visuals_xhair_x, "visuals_xhair_x");
	Save(g_Options.visuals_xhair_y, "visuals_xhair_y");
	Save(g_Options.rage_autocockrevolver, "rage_autocockrevolver");
	Save(g_Options.rage_baim_after_x_shots, "rage_baim_after_x_shots");
	Save(g_Options.rage_lagcompensation, "rage_lagcompensation");
	Save(g_Options.esp_localplayer_chams_xyz, "esp_localplayer_chams_xyz");
	Save(g_Options.rage_fixup_entities, "rage_fixup_entities");
	Save(g_Options.rage_mindmg, "rage_mindmg");
	Save(g_Options.rage_hitchance_amount, "rage_hitchance_amount");
	Save(g_Options.hvh_antiaim_x, "hvh_antiaim_x");
	Save(g_Options.hvh_antiaim_y, "hvh_antiaim_y");
	Save(g_Options.hvh_show_real_angles, "hvh_show_real_angles");
	Save(g_Options.hvh_resolver, "hvh_resolver");
	Save(g_Options.visuals_manual_aa_opacity, "visuals_manual_aa_opacity");
	Save(g_Options.hvh_resolver_disabled_draw, "hvh_resolver_disabled_draw");
	Save(g_Options.hvh_resolver_override, "hvh_resolver_override");
	Save(g_Options.hvh_resolver_override_key, "hvh_resolver_override_key");
	Save(g_Options.hvh_resolver_disable_key, "hvh_resolver_disable_key");
	Save(g_Options.skinchanger_enabled, "skinchanger_enabled");
	Save(g_Options.visuals_asuswalls_world, "visuals_asuswalls_world");
	Save(g_Options.visuals_asuswalls_staticprops, "visuals_asuswalls_staticprops");
	Save(g_Options.hvh_antiaim_y_desync, "hvh_antiaim_y_desync");
	Save(g_Options.menu_open_alternative, "menu_open_alternative");
	Save(g_Options.esp_localplayer_chams, "esp_localplayer_chams");
	Save(g_Options.esp_localplayer_chams_type, "esp_localplayer_chams_type");
	SaveArray(g_Options.esp_localplayer_chams_color_invisible, "esp_localplayer_chams_color_invisible");
	SaveArray(g_Options.esp_localplayer_chams_color_visible, "esp_localplayer_chams_color_visible");
	Save(g_Options.esp_localplayer_fakechams_type, "esp_localplayer_fakechams_type");
	SaveArray(g_Options.esp_localplayer_fakechams_color_invisible, "esp_localplayer_fakechams_color_invisible");
	SaveArray(g_Options.esp_localplayer_fakechams_color_visible, "esp_localplayer_fakechams_color_visible");
	Save(g_Options.rage_hitchance_amount_auto, "rage_hitchance_amount_auto");
	Save(g_Options.rage_hitchance_amount_awp, "rage_hitchance_amount_awp");
	Save(g_Options.rage_hitchance_amount_scout, "rage_hitchance_amount_scout");
	Save(g_Options.rage_hitchance_amount_pistol, "rage_hitchance_amount_pistol");
	Save(g_Options.rage_hitchance_amount_deagr8, "rage_hitchance_amount_deagr8");
	Save(g_Options.rage_hitchance_amount_shotgun, "rage_hitchance_amount_shotgun");
	Save(g_Options.rage_hitchance_amount_mg, "rage_hitchance_amount_mg");
	Save(g_Options.rage_hitchance_amount_smg, "rage_hitchance_amount_smg");
	Save(g_Options.rage_hitchance_amount_assaultrifle, "rage_hitchance_amount_assaultrifle");
	Save(g_Options.rage_mindmg_amount_auto, "rage_mindmg_amount_auto");
	Save(g_Options.rage_mindmg_amount_awp, "rage_mindmg_amount_awp");
	Save(g_Options.rage_mindmg_amount_scout, "rage_mindmg_amount_scout");
	Save(g_Options.dropped_weapons_color[0], "dropped_weapons_color_red");
	Save(g_Options.dropped_weapons_color[1], "dropped_weapons_color_green");
	Save(g_Options.dropped_weapons_color[2], "dropped_weapons_color_blue");
	Save(g_Options.rage_mindmg_amount_pistol, "rage_mindmg_amount_pistol");
	Save(g_Options.fake_chams, "fake_chams");
	Save(g_Options.rage_mindmg_amount_deagr8, "rage_mindmg_amount_deagr8");
	Save(g_Options.rage_mindmg_amount_shotgun, "rage_mindmg_amount_shotgun");
	Save(g_Options.rage_mindmg_amount_mg, "rage_mindmg_amount_mg");
	Save(g_Options.rage_mindmg_amount_smg, "rage_mindmg_amount_smg");
	Save(g_Options.rage_mindmg_amount_assaultrifle, "rage_mindmg_amount_assaultrifle");
	Save(g_Options.hvh_antiaim_y_custom_left, "hvh_antiaim_y_custom_left");
	Save(g_Options.visuals_nightmode, "visuals_nightmode");
	SaveArray(g_Options.visuals_others_nightmode_color, "visuals_others_nightmode_color");
	Save(g_Options.visuals_nightmode_type, "visuals_nightmode_type");
	Save(g_Options.cl_phys_timescale, "cl_phys_timescale");
	Save(g_Options.cl_phys_timescale_value, "cl_phys_timescale_value");
	Save(g_Options.hvh_aa_left_bind, "hvh_aa_left_bind");
	Save(g_Options.hvh_aa_right_bind, "hvh_aa_right_bind");
	Save(g_Options.hvh_aa_back_bind, "hvh_aa_back_bind");
	Save(g_Options.removals_zoom, "removals_zoom");
	Save(g_Options.visuals_asuswalls, "visuals_asuswalls");
	Save(g_Options.visuals_asuswalls_value, "visuals_asuswalls_value");
	Save(g_Options.hvh_antiaim_y_custom_right, "hvh_antiaim_y_custom_right");
	Save(g_Options.hvh_antiaim_y_custom_back, "hvh_antiaim_y_custom_back");
	Save(g_Options.mp_radar_showall, "mp_radar_showall");
	Save(g_Options.hvh_antiaim_y_desync_start_right, "hvh_antiaim_y_desync_start_right");
	Save(g_Options.hvh_antiaim_y_desync_start_left, "hvh_antiaim_y_desync_start_left");
	Save(g_Options.hvh_antiaim_y_desync_start_back, "hvh_antiaim_y_desync_start_back");
	Save(g_Options.legit_on_key, "legit_on_key");
	Save(g_Options.xhair_color[0], "xhair_color_red");
	Save(g_Options.xhair_color[1], "xhair_color_green");
	Save(g_Options.xhair_color[2], "xhair_color_blue");
	Save(g_Options.rage_pointscale_amount_auto, "rage_pointscale_amount_auto");
	Save(g_Options.rage_pointscale_amount_scout, "rage_pointscale_amount_scout");
	Save(g_Options.rage_pointscale_amount_assaultrifle, "rage_pointscale_amount_assaultrifle");
	Save(g_Options.rage_pointscale_amount_mg, "rage_pointscale_amount_mg");
	Save(g_Options.rage_pointscale_amount_smg, "rage_pointscale_amount_smg");
	Save(g_Options.rage_pointscale_amount_pistol, "rage_pointscale_amount_pistol");
	Save(g_Options.rage_pointscale_amount_deagr8, "rage_pointscale_amount_deagr8");
	Save(g_Options.rage_pointscale_amount_awp, "rage_pointscale_amount_awp");
	Save(g_Options.rage_pointscale_amount_shotgun, "rage_pointscale_amount_shotgun");
	SaveArray(g_Options.visuals_others_nightmode_color, "visuals_others_nightmode_color");
	SaveArray(g_Options.visuals_others_skybox_color, "visuals_others_skybox_color");
	Save(g_Options.fog_override, "fog_override");
	Save(g_Options.change_viewmodel_offset, "change_viewmodel_offset");
	Save(g_Options.viewmodel_offset_x, "viewmodel_offset_x");
	Save(g_Options.viewmodel_offset_y, "viewmodel_offset_y");
	Save(g_Options.viewmodel_offset_z, "viewmodel_offset_z");
	Save(g_Options.iRage_hitbox_auto, "iRage_hitbox_auto");
	Save(g_Options.iRage_hitbox_scout, "iRage_hitbox_scout");
	Save(g_Options.iRage_hitbox_assaultrifle, "iRage_hitbox_assaultrifle");
	Save(g_Options.iRage_hitbox_mg, "iRage_hitbox_mg");
	Save(g_Options.iRage_hitbox_smg, "iRage_hitbox_smg");
	Save(g_Options.iRage_hitbox_pistol, "iRage_hitbox_pistol");
	Save(g_Options.iRage_hitbox_deagr8, "iRage_hitbox_deagr8");
	Save(g_Options.iRage_hitbox_awp, "iRage_hitbox_awp");
	Save(g_Options.iRage_hitbox_shotgun, "iRage_hitbox_shotgun");
	Save(g_Options.misc_fakewalk_speed, "misc_fakewalk_speed");
	Save(g_Options.bRage_multipoint_auto, "bRage_multipoint_auto");
	Save(g_Options.bRage_multipoint_scout, "bRage_multipoint_scout");
	Save(g_Options.bRage_multipoint_assaultrifle, "bRage_multipoint_assaultrifle");
	Save(g_Options.bRage_multipoint_mg, "bRage_multipoint_mg");
	Save(g_Options.bRage_multipoint_smg, "bRage_multipoint_smg");
	Save(g_Options.bRage_multipoint_pistol, "bRage_multipoint_pistol");
	Save(g_Options.bRage_multipoint_deagr8, "bRage_multipoint_deagr8");
	Save(g_Options.bRage_multipoint_awp, "bRage_multipoint_awp");
	Save(g_Options.bRage_multipoint_shotgun, "bRage_multipoint_shotgun");
	Save(g_Options.hvh_antiaim_y_move, "hvh_antiaim_y_move");
	Save(g_Options.hvh_antiaim_y_custom_realmove_left, "hvh_antiaim_y_custom_realmove_left");
	Save(g_Options.hvh_antiaim_y_custom_realmove_right, "hvh_antiaim_y_custom_realmove_right");
	Save(g_Options.hvh_antiaim_y_custom_realmove_back, "hvh_antiaim_y_custom_realmove_back");
	Save(g_Options.hvh_antiaim_y_move_trigger_speed, "hvh_antiaim_y_move_trigger_speed");
	Save(g_Options.bRage_prioritize_auto, "bRage_prioritize_auto");
	Save(g_Options.bRage_prioritize_scout, "bRage_prioritize_scout");
	Save(g_Options.bRage_prioritize_assaultrifle, "bRage_prioritize_assaultrifle");
	Save(g_Options.bRage_prioritize_mg, "bRage_prioritize_mg");
	Save(g_Options.bRage_prioritize_smg, "bRage_prioritize_smg");
	Save(g_Options.bRage_prioritize_pistol, "bRage_prioritize_pistol");
	Save(g_Options.bRage_prioritize_deagr8, "bRage_prioritize_deagr8");
	Save(g_Options.bRage_prioritize_awp, "bRage_prioritize_awp");
	Save(g_Options.bRage_prioritize_shotgun, "bRage_prioritize_shotgun");
	Save(g_Options.cl_crosshair_recoil, "cl_crosshair_recoil");
	SaveArray(g_Options.rage_multiHitboxesAuto, "rage_multiHitboxesAuto");
	SaveArray(g_Options.rage_multiHitboxesScout, "rage_multiHitboxesScout");
	SaveArray(g_Options.rage_multiHitboxesAssaultRifle, "rage_multiHitboxesAssaultRifle");
	SaveArray(g_Options.rage_multiHitboxesSMG, "rage_multiHitboxesSMG");
	SaveArray(g_Options.rage_multiHitboxesMG, "rage_multiHitboxesMG");
	SaveArray(g_Options.rage_multiHitboxesPistol, "rage_multiHitboxesPistol");
	SaveArray(g_Options.rage_multiHitboxesDeagR8, "rage_multiHitboxesDeagR8");
	SaveArray(g_Options.rage_multiHitboxesAWP, "rage_multiHitboxesAWP");
	SaveArray(g_Options.rage_multiHitboxesShotgun, "rage_multiHitboxesShotgun");
	Save(g_Options.misc_infinite_duck, "misc_infinite_duck");
	Save(g_Options.esp_localplayer_viewmodel_chams, "esp_localplayer_viewmodel_chams");
	Save(g_Options.esp_localplayer_viewmodel_materials, "esp_localplayer_viewmodel_materials");

	output_file << std::setw(4) << config << std::endl;
	output_file.close();
}

void Config::LoadConfig(const std::string path)
{
	std::ifstream input_file(path);

	if (!input_file.good())
		return;

	try
	{
		config << input_file;
	}
	catch (...)
	{
		input_file.close();
		return;
	}

	Load(g_Options.legit_drawfov, "legitdrawfov");
	Load(g_Options.rage_fieldofview, "ragefov");
	Load(g_Options.indicators_enabled, "aaindicators");
	Load(g_Options.zeusrange_enabled, "zeusrange");
	Load(g_Options.esp_flags, "playerflags");
	Load(g_Options.fullbright, "fullbright");
	Load(g_Options.misc_thirdperson_distance, "tpdistance");
	Load(g_Options.legit_backtrack, "lgtbacktrack");
	Load(g_Options.legit_backtrackticks, "lgtbacktrackticks");
	Load(g_Options.misc_fakeduck, "fakeduckenable");
	Load(g_Options.misc_fakeduck_chokedpackets, "fdchoked");
	Load(g_Options.misc_fakeduck_keybind, "fdkeybind");
	Load(g_Options.legit_aim_keybind1, "lgtkey1");
	Load(g_Options.legit_aim_keybind2, "lgkey2");
	LoadArray(g_Options.glow_others_color, "glowothers");
	Load(g_Options.hvh_antiaim_lby_breaker, "lbybreaker");
	Load(g_Options.michaeljackson, "moonwalking");
	Load(g_Options.rage_nospread, "nospreadfix");
	Load(g_Options.visuals_others_dlight, "dlightz");

	Load(g_Options.misc_autoaccept, "misc_autoaccept");
	Load(g_Options.misc_revealAllRanks, "misc_revealAllRanks");
	Load(g_Options.misc_bhop, "misc_bhop");
	Load(g_Options.misc_autostrafe, "misc_autostrafe");
	Load(g_Options.misc_auto_pistol, "misc_auto_pistol");
	Load(g_Options.misc_chatspamer, "misc_chatspamer");
	Load(g_Options.misc_thirdperson, "misc_thirdperson");
	Load(g_Options.misc_thirdperson_bind, "misc_thirdperson_bind");
	Load(g_Options.misc_fakewalk, "misc_fakewalk");
	Load(g_Options.misc_fakewalk_bind, "misc_fakewalk_bind");
	Load(g_Options.visuals_manual_aa_opacity, "visuals_manual_aa_opacity");
	Load(g_Options.hvh_aa_left_bind, "hvh_aa_left_bind");
	Load(g_Options.hvh_aa_right_bind, "hvh_aa_right_bind");
	Load(g_Options.hvh_aa_back_bind, "hvh_aa_back_bind");
	Load(g_Options.misc_fakelag_enabled, "misc_fakelag_enabled");
	Load(g_Options.misc_fakelag_value, "misc_fakelag_value");
	Load(g_Options.misc_fakelag_activation_type, "misc_fakelag_activation_type");
	Load(g_Options.misc_fakelag_adaptive, "misc_fakelag_adaptive");
	Load(g_Options.misc_animated_clantag, "misc_animated_clantag");
	Load(g_Options.misc_spectatorlist, "misc_spectatorlist");
	Load(g_Options.misc_logevents, "misc_logevents");
	Load(g_Options.removals_flash, "removals_flash");
	Load(g_Options.removals_smoke, "removals_smoke");
	Load(g_Options.removals_smoke_type, "removals_smoke_type");
	Load(g_Options.removals_scope, "removals_scope");
	Load(g_Options.removals_novisualrecoil, "removals_novisualrecoil");
	Load(g_Options.removals_postprocessing, "removals_postprocessing");
	Load(g_Options.legit_on_press, "legit_on_press");
	Load(g_Options.legit_aim_keybind, "legit_aim_keybind");
	Load(g_Options.trigger_on_press, "trigger_on_press");
	Load(g_Options.trigger_keybind, "trigger_keybind");
	Load(g_Options.rcs_on_press, "rcs_on_press");
	Load(g_Options.rcs_keybind, "rcs_keybind");
	Load(g_Options.removals_crosshair, "removals_crosshair");
	Load(g_Options.esp_farther, "esp_farther");
	Load(g_Options.cl_phys_timescale, "cl_phys_timescale");
	Load(g_Options.cl_phys_timescale_value, "cl_phys_timescale_value");
	Load(g_Options.esp_fill_amount, "esp_fill_amount");
	LoadArray(g_Options.esp_player_fill_color_t, "esp_player_fill_color_t");
	LoadArray(g_Options.esp_player_fill_color_ct, "esp_player_fill_color_ct");
	LoadArray(g_Options.esp_player_fill_color_t_visible, "esp_player_fill_color_t_visible");
	LoadArray(g_Options.esp_player_fill_color_ct_visible, "esp_player_fill_color_ct_visible");
	Load(g_Options.esp_player_boundstype, "esp_player_boundstype");
	Load(g_Options.esp_player_boxtype, "esp_player_boxtype");
	LoadArray(g_Options.esp_player_bbox_color_t, "esp_player_bbox_color_t");
	LoadArray(g_Options.esp_player_bbox_color_ct, "esp_player_bbox_color_ct");
	LoadArray(g_Options.esp_player_bbox_color_t_visible, "esp_player_bbox_color_t_visible");
	Load(g_Options.hvh_resolver_custom, "hvh_resolver_custom");
	Load(g_Options.resolver_eyelby, "resolver_eyelby");
	Load(g_Options.resolver_eyelby_always, "resolver_eyelby_always");
	Load(g_Options.resolver_eyelby_running, "resolver_eyelby_running");
	Load(g_Options.resolver_eyelby_notrunning, "resolver_eyelby");
	Load(g_Options.resolver_eyelby_running_value, "resolver_eyelby_running_value");
	LoadArray(g_Options.esp_player_bbox_color_ct_visible, "esp_player_bbox_color_ct_visible");
	Load(g_Options.esp_player_name, "esp_player_name");
	Load(g_Options.esp_player_health, "esp_player_health");
	Load(g_Options.esp_player_weapons, "esp_player_weapons");
	Load(g_Options.esp_player_snaplines, "esp_player_snaplines");
	Load(g_Options.esp_player_chams, "esp_player_chams");
	Load(g_Options.esp_player_chams_type, "esp_player_chams_type");
	LoadArray(g_Options.esp_player_chams_color_t_visible, "esp_player_chams_color_t_visible");
	LoadArray(g_Options.esp_player_chams_color_ct_visible, "esp_player_chams_color_ct_visible");
	LoadArray(g_Options.esp_player_chams_color_t, "esp_player_chams_color_t");
	LoadArray(g_Options.esp_player_chams_color_ct, "esp_player_chams_color_ct");
	Load(g_Options.esp_localplayer_chams, "esp_localplayer_chams");
	Load(g_Options.esp_localplayer_chams_type, "esp_localplayer_chams_type");
	LoadArray(g_Options.esp_localplayer_chams_color_invisible, "esp_localplayer_chams_color_invisible");
	LoadArray(g_Options.esp_localplayer_chams_color_visible, "esp_localplayer_chams_color_visible");
	Load(g_Options.esp_localplayer_fakechams_type, "esp_localplayer_fakechams_type");
	LoadArray(g_Options.esp_localplayer_fakechams_color_invisible, "esp_localplayer_fakechams_color_invisible");
	LoadArray(g_Options.esp_localplayer_fakechams_color_visible, "esp_localplayer_fakechams_color_visible");
	Load(g_Options.esp_player_skelet, "esp_player_skelet");
	Load(g_Options.esp_player_anglelines, "esp_player_anglelines");
	Load(g_Options.esp_dropped_weapons, "esp_dropped_weapons");
	Load(g_Options.dropped_weapons_color[0], "dropped_weapons_color_red");
	Load(g_Options.dropped_weapons_color[1], "dropped_weapons_color_green");
	Load(g_Options.dropped_weapons_color[2], "dropped_weapons_color_blue");
	Load(g_Options.xhair_color[0], "xhair_color_red");
	Load(g_Options.xhair_color[1], "xhair_color_green");
	Load(g_Options.xhair_color[2], "xhair_color_blue");
	Load(g_Options.change_viewmodel_offset, "change_viewmodel_offset");
	Load(g_Options.viewmodel_offset_x, "viewmodel_offset_x");
	Load(g_Options.viewmodel_offset_y, "viewmodel_offset_y");
	Load(g_Options.viewmodel_offset_z, "viewmodel_offset_z");
	Load(g_Options.fog_override, "fog_override");
	Load(g_Options.visuals_asuswalls_world, "visuals_asuswalls_world");
	Load(g_Options.visuals_asuswalls_staticprops, "visuals_asuswalls_staticprops");
	Load(g_Options.esp_planted_c4, "esp_planted_c4");
	Load(g_Options.esp_grenades, "esp_grenades");
	Load(g_Options.esp_grenades_type, "esp_grenades_type");
	Load(g_Options.esp_backtracked_player_skelet, "esp_backtracked_player_skelet");
	Load(g_Options.esp_lagcompensated_hitboxes, "esp_lagcompensated_hitboxes");
	Load(g_Options.esp_lagcompensated_hitboxes_type, "esp_lagcompensated_hitboxes_type");
	Load(g_Options.visuals_manual_aa, "visuals_manual_aa");
	Load(g_Options.visuals_others_player_fov, "visuals_others_player_fov");
	Load(g_Options.visuals_others_player_fov_viewmodel, "visuals_others_player_fov_viewmodel");
	Load(g_Options.watermark, "visuals_others_watermark");
	Load(g_Options.visuals_others_grenade_pred, "visuals_others_grenade_pred");
	Load(g_Options.visuals_draw_xhair, "visuals_draw_xhair");
	Load(g_Options.visuals_xhair_x, "visuals_xhair_x");
	Load(g_Options.visuals_xhair_y, "visuals_xhair_y");
	Load(g_Options.visuals_others_hitmarker, "visuals_others_hitmarker");
	Load(g_Options.visuals_others_bulletimpacts, "visuals_others_bulletimpacts");
	LoadArray(g_Options.visuals_others_bulletimpacts_color, "visuals_others_bulletimpacts_color");
	LoadArray(g_Options.visuals_others_nightmode_color, "visuals_others_nightmode_color");
	LoadArray(g_Options.visuals_others_skybox_color, "visuals_others_skybox_color");
	Load(g_Options.visuals_others_sky, "visuals_others_sky");
	Load(g_Options.removals_zoom, "removals_zoom");
	Load(g_Options.glow_enabled, "glow_enabled");
	Load(g_Options.glow_players, "glow_players");
	LoadArray(g_Options.glow_player_color_t, "glow_player_color_t");
	LoadArray(g_Options.glow_player_color_ct, "glow_player_color_ct");
	LoadArray(g_Options.glow_player_color_t_visible, "glow_player_color_t_visible");
	LoadArray(g_Options.glow_player_color_ct_visible, "glow_player_color_ct_visible");
	Load(g_Options.glow_players_style, "glow_players_style");
	Load(g_Options.glow_others, "glow_others");
	Load(g_Options.glow_others_style, "glow_others_style");
	Load(g_Options.legit_enabled, "legit_enabled");
	Load(g_Options.legit_hsonly, "legit_hsonly");
	Load(g_Options.legit_rcs, "legit_rcs");
	Load(g_Options.legit_trigger, "legit_trigger");
	Load(g_Options.legit_trigger_with_aimkey, "legit_trigger_with_aimkey");
	Load(g_Options.legit_preaim, "legit_preaim");
	Load(g_Options.legit_aftershots, "legit_aftershots");
	Load(g_Options.legit_afteraim, "legit_afteraim");
	Load(g_Options.legit_smooth_factor, "legit_smooth_factor");
	Load(g_Options.legit_fov, "legit_fov");
	Load(g_Options.rage_enabled, "rage_enabled");
	Load(g_Options.rage_aimkey, "rage_aimkey");
	Load(g_Options.rage_silent, "rage_silent");
	Load(g_Options.misc_infinite_duck, "misc_infinite_duck");
	Load(g_Options.rage_norecoil, "rage_norecoil");
	Load(g_Options.rage_autoshoot, "rage_autoshoot");
	Load(g_Options.backtrack_bhd_wall_only, "backtrack_bhd_wall_only");
	Load(g_Options.rage_autoscope, "rage_autoscope");
	Load(g_Options.rage_autocrouch, "rage_autocrouch");
	Load(g_Options.rage_autostop, "rage_autostop");
	Load(g_Options.rage_autobaim, "rage_autobaim");
	Load(g_Options.esp_localplayer_chams_xyz, "esp_localplayer_chams_xyz");
	Load(g_Options.rage_autocockrevolver, "rage_autocockrevolver");
	Load(g_Options.visuals_nightmode, "visuals_nightmode");
	LoadArray(g_Options.visuals_others_nightmode_color, "visuals_others_nightmode_color");
	Load(g_Options.visuals_nightmode_type, "visuals_nightmode_type");
	Load(g_Options.visuals_asuswalls, "visuals_asuswalls");
	Load(g_Options.visuals_asuswalls_value, "visuals_asuswalls_value");
	Load(g_Options.rage_baim_after_x_shots, "rage_baim_after_x_shots");
	Load(g_Options.rage_lagcompensation, "rage_lagcompensation");
	Load(g_Options.hvh_antiaim_y_custom_left, "hvh_antiaim_y_custom_left");
	Load(g_Options.hvh_antiaim_y_custom_right, "hvh_antiaim_y_custom_right");
	Load(g_Options.hvh_antiaim_y_custom_back, "hvh_antiaim_y_custom_back");
	Load(g_Options.mp_radar_showall, "mp_radar_showall");
	Load(g_Options.cl_crosshair_recoil, "cl_crosshair_recoil");
	Load(g_Options.rage_fixup_entities, "rage_fixup_entities");
	Load(g_Options.rage_mindmg, "rage_mindmg");
	Load(g_Options.rage_hitchance_amount, "rage_hitchance_amount");
	Load(g_Options.fake_chams, "fake_chams");
	Load(g_Options.hvh_antiaim_x, "hvh_antiaim_x");
	Load(g_Options.hvh_antiaim_y, "hvh_antiaim_y");
	Load(g_Options.hvh_antiaim_y_move, "hvh_antiaim_y_move");
	Load(g_Options.hvh_antiaim_y_custom_realmove_left, "hvh_antiaim_y_custom_realmove_left");
	Load(g_Options.hvh_antiaim_y_custom_realmove_right, "hvh_antiaim_y_custom_realmove_right");
	Load(g_Options.hvh_antiaim_y_custom_realmove_back, "hvh_antiaim_y_custom_realmove_back");
	Load(g_Options.hvh_antiaim_y_move_trigger_speed, "hvh_antiaim_y_move_trigger_speed");
	Load(g_Options.hvh_show_real_angles, "hvh_show_real_angles");
	Load(g_Options.hvh_resolver, "hvh_resolver");
	Load(g_Options.hvh_resolver_disabled_draw, "hvh_resolver_disabled_draw");
	Load(g_Options.hvh_resolver_override, "hvh_resolver_override");
	Load(g_Options.hvh_resolver_override_key, "hvh_resolver_override_key");
	Load(g_Options.hvh_resolver_disable_key, "hvh_resolver_disable_key");
	Load(g_Options.hvh_antiaim_y_desync_start_back, "hvh_antiaim_y_desync_start_back");
	Load(g_Options.hvh_antiaim_y_desync_start_right, "hvh_antiaim_y_desync_start_right");
	Load(g_Options.hvh_antiaim_y_desync_start_left, "hvh_antiaim_y_desync_start_left");
	Load(g_Options.skinchanger_enabled, "skinchanger_enabled");
	Load(g_Options.rage_hitchance_amount_auto, "rage_hitchance_amount_auto");
	Load(g_Options.rage_hitchance_amount_awp, "rage_hitchance_amount_awp");
	Load(g_Options.rage_hitchance_amount_scout, "rage_hitchance_amount_scout");
	Load(g_Options.rage_hitchance_amount_pistol, "rage_hitchance_amount_pistol");
	Load(g_Options.rage_hitchance_amount_deagr8, "rage_hitchance_amount_deagr8");
	Load(g_Options.rage_hitchance_amount_shotgun, "rage_hitchance_amount_shotgun");
	Load(g_Options.rage_hitchance_amount_mg, "rage_hitchance_amount_mg");
	Load(g_Options.rage_hitchance_amount_smg, "rage_hitchance_amount_smg");
	Load(g_Options.rage_hitchance_amount_assaultrifle, "rage_hitchance_amount_assaultrifle");
	Load(g_Options.rage_mindmg_amount_auto, "rage_mindmg_amount_auto");
	Load(g_Options.rage_mindmg_amount_awp, "rage_mindmg_amount_awp");
	Load(g_Options.rage_mindmg_amount_scout, "rage_mindmg_amount_scout");
	Load(g_Options.rage_mindmg_amount_pistol, "rage_mindmg_amount_pistol");
	Load(g_Options.rage_mindmg_amount_deagr8, "rage_mindmg_amount_deagr8");
	Load(g_Options.rage_mindmg_amount_shotgun, "rage_mindmg_amount_shotgun");
	Load(g_Options.rage_mindmg_amount_mg, "rage_mindmg_amount_mg");
	Load(g_Options.rage_mindmg_amount_smg, "rage_mindmg_amount_smg");
	Load(g_Options.rage_mindmg_amount_assaultrifle, "rage_mindmg_amount_assaultrifle");
	Load(g_Options.rage_pointscale_amount_auto, "rage_pointscale_amount_auto");
	Load(g_Options.rage_pointscale_amount_scout, "rage_pointscale_amount_scout");
	Load(g_Options.rage_pointscale_amount_assaultrifle, "rage_pointscale_amount_assaultrifle");
	Load(g_Options.rage_pointscale_amount_mg, "rage_pointscale_amount_mg");
	Load(g_Options.rage_pointscale_amount_smg, "rage_pointscale_amount_smg");
	Load(g_Options.rage_pointscale_amount_pistol, "rage_pointscale_amount_pistol");
	Load(g_Options.rage_pointscale_amount_deagr8, "rage_pointscale_amount_deagr8");
	Load(g_Options.rage_pointscale_amount_awp, "rage_pointscale_amount_awp");
	Load(g_Options.rage_pointscale_amount_shotgun, "rage_pointscale_amount_shotgun");
	Load(g_Options.iRage_hitbox_auto, "iRage_hitbox_auto");
	Load(g_Options.iRage_hitbox_scout, "iRage_hitbox_scout");
	Load(g_Options.iRage_hitbox_assaultrifle, "iRage_hitbox_assaultrifle");
	Load(g_Options.iRage_hitbox_mg, "iRage_hitbox_mg");
	Load(g_Options.iRage_hitbox_smg, "iRage_hitbox_smg");
	Load(g_Options.iRage_hitbox_pistol, "iRage_hitbox_pistol");
	Load(g_Options.iRage_hitbox_deagr8, "iRage_hitbox_deagr8");
	Load(g_Options.iRage_hitbox_awp, "iRage_hitbox_awp");
	Load(g_Options.iRage_hitbox_shotgun, "iRage_hitbox_shotgun");
	Load(g_Options.misc_fakewalk_speed, "misc_fakewalk_speed");
	Load(g_Options.bRage_multipoint_auto, "bRage_multipoint_auto");
	Load(g_Options.bRage_multipoint_scout, "bRage_multipoint_scout");
	Load(g_Options.bRage_multipoint_assaultrifle, "bRage_multipoint_assaultrifle");
	Load(g_Options.bRage_multipoint_mg, "bRage_multipoint_mg");
	Load(g_Options.bRage_multipoint_smg, "bRage_multipoint_smg");
	Load(g_Options.bRage_multipoint_pistol, "bRage_multipoint_pistol");
	Load(g_Options.bRage_multipoint_deagr8, "bRage_multipoint_deagr8");
	Load(g_Options.bRage_multipoint_awp, "bRage_multipoint_awp");
	Load(g_Options.bRage_multipoint_shotgun, "bRage_multipoint_shotgun");
	Load(g_Options.bRage_prioritize_auto, "bRage_prioritize_auto");
	Load(g_Options.bRage_prioritize_scout, "bRage_prioritize_scout");
	Load(g_Options.bRage_prioritize_assaultrifle, "bRage_prioritize_assaultrifle");
	Load(g_Options.bRage_prioritize_mg, "bRage_prioritize_mg");
	Load(g_Options.bRage_prioritize_smg, "bRage_prioritize_smg");
	Load(g_Options.bRage_prioritize_pistol, "bRage_prioritize_pistol");
	Load(g_Options.bRage_prioritize_deagr8, "bRage_prioritize_deagr8");
	Load(g_Options.bRage_prioritize_awp, "bRage_prioritize_awp");
	Load(g_Options.bRage_prioritize_shotgun, "bRage_prioritize_shotgun");
	LoadArray(g_Options.rage_multiHitboxesAuto, "rage_multiHitboxesAuto");
	LoadArray(g_Options.rage_multiHitboxesScout, "rage_multiHitboxesScout");
	LoadArray(g_Options.rage_multiHitboxesAssaultRifle, "rage_multiHitboxesAssaultRifle");
	LoadArray(g_Options.rage_multiHitboxesSMG, "rage_multiHitboxesSMG");
	LoadArray(g_Options.rage_multiHitboxesMG, "rage_multiHitboxesMG");
	LoadArray(g_Options.rage_multiHitboxesPistol, "rage_multiHitboxesPistol");
	LoadArray(g_Options.rage_multiHitboxesDeagR8, "rage_multiHitboxesDeagR8");
	LoadArray(g_Options.rage_multiHitboxesAWP, "rage_multiHitboxesAWP");
	LoadArray(g_Options.rage_multiHitboxesShotgun, "rage_multiHitboxesShotgun");
	Load(g_Options.legit_on_key, "legit_on_key");
	Load(g_Options.menu_open_alternative, "menu_open_alternative");
	Load(g_Options.hvh_antiaim_y_desync, "hvh_antiaim_y_desync");
	Load(g_Options.legit_on_press, "legit_on_press");
	Load(g_Options.trigger_on_press, "trigger_on_press");
	Load(g_Options.rcs_on_press, "rcs_on_press");
	Load(g_Options.legit_aim_keybind, "legit_aim_keybind");
	Load(g_Options.rcs_keybind, "rcs_keybind");
	Load(g_Options.trigger_keybind, "trigger_keybind");
	Load(g_Options.esp_localplayer_viewmodel_chams, "esp_localplayer_viewmodel_chams");
	Load(g_Options.esp_localplayer_viewmodel_materials, "esp_localplayer_viewmodel_materials");

	input_file.close();
}

std::vector<std::string> Config::GetAllConfigs()
{
	namespace fs = std::filesystem;

	std::string fPath = std::string(Global::my_documents_folder) + "\\nanosense\\";

	std::vector<ConfigFile> config_files = GetAllConfigsInFolder(fPath, ".0x000c7B");
	std::vector<std::string> config_file_names;

	for (auto config = config_files.begin(); config != config_files.end(); config++)
		config_file_names.emplace_back(config->GetName());

	std::sort(config_file_names.begin(), config_file_names.end());

	return config_file_names;
}

std::vector<ConfigFile> Config::GetAllConfigsInFolder(const std::string path, const std::string ext)
{
	

	std::vector<ConfigFile> config_files;

	
	return config_files;
}

template<typename T>
void Config::Load(T &value, std::string str)
{
	if (config[str].empty())
		return;

	value = config[str].get<T>();
}

void Config::LoadArray(float_t value[4], std::string str)
{
	if (config[str]["0"].empty() || config[str]["1"].empty() || config[str]["2"].empty() || config[str]["3"].empty())
		return;

	value[0] = config[str]["0"].get<float_t>();
	value[1] = config[str]["1"].get<float_t>();
	value[2] = config[str]["2"].get<float_t>();
	value[3] = config[str]["3"].get<float_t>();
}

void Config::LoadArray(bool value[14], std::string str)
{
	if (config[str]["0"].empty() || config[str]["1"].empty() || config[str]["2"].empty() || config[str]["3"].empty()
		|| config[str]["4"].empty() || config[str]["5"].empty() || config[str]["6"].empty() || config[str]["7"].empty()
		|| config[str]["8"].empty() || config[str]["9"].empty() || config[str]["10"].empty() || config[str]["11"].empty()
		|| config[str]["12"].empty() || config[str]["13"].empty())
		return;

	value[0] = config[str]["0"].get<bool>();
	value[1] = config[str]["1"].get<bool>();
	value[2] = config[str]["2"].get<bool>();
	value[3] = config[str]["3"].get<bool>();
	value[4] = config[str]["4"].get<bool>();
	value[5] = config[str]["5"].get<bool>();
	value[6] = config[str]["6"].get<bool>();
	value[7] = config[str]["7"].get<bool>();
	value[8] = config[str]["8"].get<bool>();
	value[9] = config[str]["9"].get<bool>();
	value[10] = config[str]["10"].get<bool>();
	value[11] = config[str]["11"].get<bool>();
	value[12] = config[str]["12"].get<bool>();
	value[13] = config[str]["13"].get<bool>();
}

template<typename T>
void Config::Save(T &value, std::string str)
{
	config[str] = value;
}

void Config::SaveArray(float_t value[4], std::string str)
{
	config[str]["0"] = value[0];
	config[str]["1"] = value[1];
	config[str]["2"] = value[2];
	config[str]["3"] = value[3];
}

void Config::SaveArray(bool value[14], std::string str)
{
	config[str]["0"] = value[0];
	config[str]["1"] = value[1];
	config[str]["2"] = value[2];
	config[str]["3"] = value[3];
	config[str]["4"] = value[4];
	config[str]["5"] = value[5];
	config[str]["6"] = value[6];
	config[str]["7"] = value[7];
	config[str]["8"] = value[8];
	config[str]["9"] = value[9];
	config[str]["10"] = value[10];
	config[str]["11"] = value[11];
	config[str]["12"] = value[12];
	config[str]["13"] = value[13];
}
// Junk Code By Troll Face & Thaisen's Gen
void mjXZFvkpgRYKAqXgnDFAEOwiBBclYbskgpVZHyKeFBhzG18286344() {     int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm94077995 = -797156925;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm12317947 = -634842421;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm70405680 = -448853411;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm70074842 = -761972379;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm65614886 = -316193120;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm68975001 = -822581384;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm70379305 = -301357835;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm90229976 = -89860099;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm14875267 = -515568153;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm1069068 = -301340192;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm56610513 = -200062848;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm14233700 = -240682886;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm93194303 = -203618124;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm9558166 = -829667585;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm84739534 = -459444735;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm40848098 = -646268278;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm73907473 = -492936914;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm45106503 = -417332765;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm26119990 = -247765150;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm31240115 = -918376479;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm12066152 = -905254614;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm5598080 = -616234985;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm88025492 = -508718113;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm55076452 = -413723794;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm36342331 = -770941595;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm57743847 = -426586043;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm62054925 = -99457145;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm18278411 = -977891382;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm19124098 = -358603445;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm86820246 = -643513085;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm38392170 = -511038413;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm59745816 = -406289814;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm14579396 = -63034765;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm27376253 = -695850045;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm72370122 = 97882135;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm94104248 = -719131409;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm2024393 = -728757478;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm62886226 = -71980869;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm25483834 = -384309450;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm37365644 = -521048242;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm25466554 = 9180184;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm69360488 = -939373628;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm89839723 = -771999515;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm32761866 = -365943134;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm12145577 = -877144884;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm18094584 = -991387379;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm99014194 = -842735071;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm87292278 = -849641555;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm12274161 = -246814530;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm987968 = 81169524;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm27150212 = -700040831;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm83135592 = -501866528;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm71494336 = 27396881;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm49812820 = -310688869;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm6220192 = -926655708;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm88479915 = -80921940;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm24292455 = -26124308;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm15329229 = 64870382;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm33732512 = -991030785;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm7871040 = -889607078;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm6920076 = -623124239;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm52100894 = -323466454;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm71105879 = -731256654;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm28055021 = -872055069;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm62676897 = -790301780;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm96864696 = -793773034;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm99654304 = -77648122;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm65818051 = -507768079;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm37188043 = -827549721;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm90635285 = -740313326;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm38823706 = -917510801;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm11021247 = -320956046;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm19622669 = 66976684;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm88754345 = -726716908;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm5773561 = -827556664;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm42705663 = -965880987;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm15758357 = -844235471;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm55263626 = -42774979;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm42930876 = -536578910;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm18247747 = -779554216;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm58729652 = -583850972;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm74762646 = -249815591;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm6004251 = -631076853;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm18136130 = -339772970;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm59670034 = -943472254;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm55256577 = 90828114;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm88251479 = -333686696;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm64766576 = -752345897;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm21156062 = -769194337;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm83890206 = -821195925;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm69811794 = -593007102;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm86695164 = -693627861;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm29153715 = -80950085;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm17612795 = -494702372;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm30445568 = -897924003;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm73365659 = -667353362;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm98254609 = -108116974;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm61784703 = -899944447;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm70084968 = -575641355;    int xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm15280880 = -797156925;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm94077995 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm12317947;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm12317947 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm70405680;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm70405680 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm70074842;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm70074842 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm65614886;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm65614886 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm68975001;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm68975001 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm70379305;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm70379305 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm90229976;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm90229976 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm14875267;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm14875267 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm1069068;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm1069068 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm56610513;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm56610513 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm14233700;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm14233700 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm93194303;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm93194303 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm9558166;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm9558166 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm84739534;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm84739534 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm40848098;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm40848098 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm73907473;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm73907473 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm45106503;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm45106503 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm26119990;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm26119990 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm31240115;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm31240115 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm12066152;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm12066152 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm5598080;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm5598080 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm88025492;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm88025492 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm55076452;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm55076452 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm36342331;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm36342331 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm57743847;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm57743847 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm62054925;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm62054925 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm18278411;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm18278411 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm19124098;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm19124098 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm86820246;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm86820246 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm38392170;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm38392170 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm59745816;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm59745816 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm14579396;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm14579396 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm27376253;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm27376253 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm72370122;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm72370122 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm94104248;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm94104248 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm2024393;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm2024393 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm62886226;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm62886226 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm25483834;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm25483834 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm37365644;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm37365644 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm25466554;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm25466554 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm69360488;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm69360488 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm89839723;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm89839723 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm32761866;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm32761866 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm12145577;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm12145577 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm18094584;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm18094584 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm99014194;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm99014194 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm87292278;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm87292278 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm12274161;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm12274161 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm987968;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm987968 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm27150212;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm27150212 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm83135592;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm83135592 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm71494336;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm71494336 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm49812820;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm49812820 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm6220192;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm6220192 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm88479915;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm88479915 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm24292455;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm24292455 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm15329229;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm15329229 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm33732512;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm33732512 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm7871040;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm7871040 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm6920076;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm6920076 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm52100894;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm52100894 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm71105879;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm71105879 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm28055021;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm28055021 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm62676897;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm62676897 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm96864696;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm96864696 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm99654304;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm99654304 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm65818051;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm65818051 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm37188043;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm37188043 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm90635285;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm90635285 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm38823706;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm38823706 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm11021247;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm11021247 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm19622669;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm19622669 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm88754345;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm88754345 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm5773561;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm5773561 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm42705663;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm42705663 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm15758357;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm15758357 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm55263626;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm55263626 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm42930876;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm42930876 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm18247747;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm18247747 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm58729652;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm58729652 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm74762646;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm74762646 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm6004251;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm6004251 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm18136130;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm18136130 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm59670034;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm59670034 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm55256577;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm55256577 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm88251479;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm88251479 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm64766576;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm64766576 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm21156062;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm21156062 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm83890206;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm83890206 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm69811794;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm69811794 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm86695164;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm86695164 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm29153715;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm29153715 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm17612795;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm17612795 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm30445568;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm30445568 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm73365659;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm73365659 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm98254609;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm98254609 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm61784703;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm61784703 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm70084968;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm70084968 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm15280880;     xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm15280880 = xcVDJAGpxoYnpYETyRFYneKUyYotauDgwbEYGhObm94077995;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void JyLdOasVKRnNfYlFsAFKmFREprQZMpNxnIYOlWHFuoVGHvdViSlEaiAEgGoXKSIVEpaXzXTVszPxjKxtOurt87548989() {     float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU70551378 = -89161911;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU90069528 = -518303650;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU27522203 = -310019863;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU65424860 = -333129381;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU4620940 = -100056942;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU24570430 = -812991784;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU4774262 = 31163135;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU40874734 = -450697838;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU40543399 = -955039976;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU43304035 = -818763294;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU16298379 = -360913159;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU326136 = -398027763;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU5927019 = -6021455;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU46413173 = -364424486;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU32807458 = -274764711;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU65868633 = -335192209;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU95114675 = -162168746;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU36728659 = -51153961;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU92191948 = -915703488;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU66021194 = -960999907;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU43229537 = 39350346;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU10400780 = -558426199;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU52405926 = -330967810;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU15607497 = -463323737;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU70246958 = -718098946;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU46847686 = -711583319;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU22192244 = -331505325;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU24808999 = -54874970;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU13257911 = -178829396;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU15117668 = -48200934;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU57969303 = -559185303;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU1990681 = -789586054;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU50280949 = 86871630;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU94712681 = -540131348;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU28887511 = -399456689;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU72421845 = -471425859;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU75781820 = -128627884;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU14760167 = -525083987;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU54290727 = -345004040;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU59959131 = -991621686;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU29729635 = -31481181;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU15386149 = -231267833;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU8348986 = -936036775;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU69467073 = -626911191;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU94054476 = -882980812;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU32848649 = -119631559;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU55004914 = -191440286;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU80251130 = -320170593;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU21010214 = -663543072;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU71899818 = 68582602;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU97355255 = -190423162;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU76180035 = -983383466;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU82169378 = -172140720;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU40378835 = -113544339;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU44319453 = -62135777;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU60150598 = -530735713;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU37663602 = -87335840;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU11914707 = -846696127;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU95177901 = -615030436;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU57773253 = -388473623;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU2378186 = -381486460;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU79965262 = -913961896;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU27616824 = -171868442;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU25425732 = -806839043;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU85334731 = -159577992;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU14307698 = -571327106;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU50045186 = -384899393;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU11214337 = -465890107;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU17525663 = -964967798;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU60385612 = -803338853;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU90086812 = -106564325;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU80354508 = -637084760;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU82437931 = -706149921;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU32232818 = -924081802;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU36291560 = -829518727;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU27843389 = -729381822;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU2051795 = -622389425;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU82938853 = -704056619;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU21553020 = -580342926;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU37398309 = -498467387;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU91842772 = -420143034;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU41941114 = 88665268;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU3798785 = -391331898;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU41358093 = -147411998;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU17762412 = -857777772;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU81789268 = -575801838;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU19821303 = -517445335;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU9902114 = -799584032;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU50393229 = -377995572;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU68736912 = -868720977;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU34758244 = -284090019;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU63867114 = -281931758;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU19582266 = -910053552;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU96517474 = -956530417;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU57580945 = -510135226;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU49764372 = -117519285;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU87769325 = 40600609;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU82923254 = -29197733;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU84132341 = -367333200;    float DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU79746779 = -89161911;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU70551378 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU90069528;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU90069528 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU27522203;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU27522203 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU65424860;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU65424860 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU4620940;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU4620940 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU24570430;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU24570430 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU4774262;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU4774262 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU40874734;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU40874734 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU40543399;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU40543399 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU43304035;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU43304035 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU16298379;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU16298379 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU326136;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU326136 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU5927019;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU5927019 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU46413173;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU46413173 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU32807458;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU32807458 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU65868633;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU65868633 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU95114675;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU95114675 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU36728659;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU36728659 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU92191948;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU92191948 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU66021194;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU66021194 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU43229537;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU43229537 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU10400780;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU10400780 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU52405926;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU52405926 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU15607497;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU15607497 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU70246958;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU70246958 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU46847686;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU46847686 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU22192244;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU22192244 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU24808999;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU24808999 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU13257911;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU13257911 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU15117668;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU15117668 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU57969303;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU57969303 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU1990681;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU1990681 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU50280949;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU50280949 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU94712681;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU94712681 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU28887511;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU28887511 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU72421845;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU72421845 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU75781820;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU75781820 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU14760167;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU14760167 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU54290727;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU54290727 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU59959131;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU59959131 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU29729635;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU29729635 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU15386149;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU15386149 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU8348986;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU8348986 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU69467073;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU69467073 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU94054476;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU94054476 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU32848649;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU32848649 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU55004914;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU55004914 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU80251130;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU80251130 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU21010214;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU21010214 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU71899818;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU71899818 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU97355255;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU97355255 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU76180035;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU76180035 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU82169378;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU82169378 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU40378835;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU40378835 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU44319453;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU44319453 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU60150598;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU60150598 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU37663602;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU37663602 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU11914707;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU11914707 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU95177901;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU95177901 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU57773253;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU57773253 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU2378186;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU2378186 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU79965262;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU79965262 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU27616824;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU27616824 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU25425732;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU25425732 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU85334731;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU85334731 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU14307698;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU14307698 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU50045186;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU50045186 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU11214337;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU11214337 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU17525663;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU17525663 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU60385612;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU60385612 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU90086812;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU90086812 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU80354508;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU80354508 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU82437931;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU82437931 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU32232818;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU32232818 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU36291560;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU36291560 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU27843389;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU27843389 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU2051795;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU2051795 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU82938853;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU82938853 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU21553020;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU21553020 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU37398309;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU37398309 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU91842772;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU91842772 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU41941114;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU41941114 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU3798785;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU3798785 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU41358093;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU41358093 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU17762412;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU17762412 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU81789268;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU81789268 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU19821303;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU19821303 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU9902114;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU9902114 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU50393229;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU50393229 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU68736912;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU68736912 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU34758244;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU34758244 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU63867114;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU63867114 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU19582266;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU19582266 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU96517474;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU96517474 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU57580945;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU57580945 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU49764372;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU49764372 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU87769325;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU87769325 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU82923254;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU82923254 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU84132341;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU84132341 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU79746779;     DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU79746779 = DIWFIUheikvcfACRVGWKyQMaSCUWJuIQbPRvTVPeqpnHEnSjIfXHXhumwwmFAKUnbfBQiGyArsTJdFWfklOeWBstUfzXiJdfU70551378;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void mebXjEDNCbdUjfagIuQmKFWKCKHruuRTyZvZMWvbNRRaHBZDoWr91597469() {     long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe18238089 = -651766728;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe28660483 = -960649937;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe98306259 = -234481394;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe76877637 = -356318546;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe7537288 = -615310843;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe78433139 = -203764962;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe4956936 = -896029021;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe71740127 = -372857106;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe54086159 = -693082783;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe37305580 = -778208119;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe95021299 = -977268132;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe78832045 = -886439291;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe48432815 = -359474611;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe19685610 = -650418082;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe27658937 = 78173107;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe6985269 = -249798553;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe94475377 = -641848094;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe53595689 = -952698932;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe36830232 = -461787370;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe93032505 = -513391263;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe13092211 = -139365622;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe65160618 = -970913809;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe30991286 = 79407190;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe21035299 = -199678839;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe94106899 = -815170051;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe23563285 = -65676705;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe49753956 = -239913585;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe27155199 = -965472033;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe4869084 = -743759938;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe33972222 = -493081213;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe94450312 = -224900901;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe19758801 = -373272864;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe67373279 = -47895307;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe14405074 = 15409575;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe3307 = -787337078;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe4262706 = -93809145;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe99658874 = -249042;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe4073675 = -46014263;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe67215393 = -696408738;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe71107432 = 86805637;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe62375358 = -19171782;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe54749850 = -559889783;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe14149681 = -109956985;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe43645790 = -826893921;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe1993918 = -506547684;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe43116336 = 58967887;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe37605020 = -294258344;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe89274904 = -15249609;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe81912727 = -62517814;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe42298136 = -183490711;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe62786706 = -427842070;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe95832203 = -80634205;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe22735791 = -503819454;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe58558713 = -377912980;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe78505258 = -466588771;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe53077471 = -680852919;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe97669196 = -940057128;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe77270961 = 65197445;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe82770737 = -541148496;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe83974002 = -449634139;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe28679183 = -963851377;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe77801736 = -930556988;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe66871044 = -629097169;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe20113937 = -100001571;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe42855267 = -453307218;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe75262499 = -503995269;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe11458766 = -738543985;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe34027741 = -274884187;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe19682304 = -863081004;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe23396232 = -828017749;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe7326395 = -149549512;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe90401702 = -495833831;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe86380296 = -156290195;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe65722800 = -448593008;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe30657148 = -394219482;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe58342360 = -579475840;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe51010938 = -760956825;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe87345495 = -93698890;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe19041381 = -693131155;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe50990563 = -774137938;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe85958265 = -771418361;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe60479051 = -124663976;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe45242472 = -802954220;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe62570947 = -460269227;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe71185516 = 34760856;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe98618108 = -44266697;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe97023010 = -869453410;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe8814567 = -669982328;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe35899815 = -518001654;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe46925835 = -6484159;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe6593509 = -153752018;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe22387913 = 34553513;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe21302938 = -504865768;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe83241390 = -146774600;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe42428249 = 50657014;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe84573621 = -88614795;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe87878805 = -930792614;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe94035743 = 90044586;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe790524 = -273586703;    long jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe26731419 = -651766728;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe18238089 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe28660483;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe28660483 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe98306259;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe98306259 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe76877637;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe76877637 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe7537288;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe7537288 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe78433139;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe78433139 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe4956936;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe4956936 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe71740127;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe71740127 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe54086159;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe54086159 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe37305580;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe37305580 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe95021299;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe95021299 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe78832045;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe78832045 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe48432815;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe48432815 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe19685610;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe19685610 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe27658937;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe27658937 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe6985269;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe6985269 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe94475377;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe94475377 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe53595689;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe53595689 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe36830232;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe36830232 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe93032505;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe93032505 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe13092211;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe13092211 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe65160618;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe65160618 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe30991286;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe30991286 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe21035299;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe21035299 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe94106899;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe94106899 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe23563285;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe23563285 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe49753956;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe49753956 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe27155199;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe27155199 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe4869084;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe4869084 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe33972222;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe33972222 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe94450312;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe94450312 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe19758801;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe19758801 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe67373279;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe67373279 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe14405074;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe14405074 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe3307;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe3307 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe4262706;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe4262706 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe99658874;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe99658874 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe4073675;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe4073675 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe67215393;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe67215393 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe71107432;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe71107432 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe62375358;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe62375358 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe54749850;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe54749850 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe14149681;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe14149681 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe43645790;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe43645790 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe1993918;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe1993918 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe43116336;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe43116336 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe37605020;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe37605020 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe89274904;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe89274904 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe81912727;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe81912727 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe42298136;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe42298136 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe62786706;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe62786706 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe95832203;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe95832203 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe22735791;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe22735791 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe58558713;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe58558713 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe78505258;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe78505258 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe53077471;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe53077471 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe97669196;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe97669196 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe77270961;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe77270961 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe82770737;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe82770737 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe83974002;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe83974002 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe28679183;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe28679183 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe77801736;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe77801736 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe66871044;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe66871044 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe20113937;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe20113937 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe42855267;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe42855267 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe75262499;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe75262499 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe11458766;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe11458766 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe34027741;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe34027741 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe19682304;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe19682304 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe23396232;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe23396232 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe7326395;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe7326395 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe90401702;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe90401702 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe86380296;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe86380296 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe65722800;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe65722800 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe30657148;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe30657148 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe58342360;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe58342360 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe51010938;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe51010938 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe87345495;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe87345495 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe19041381;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe19041381 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe50990563;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe50990563 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe85958265;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe85958265 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe60479051;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe60479051 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe45242472;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe45242472 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe62570947;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe62570947 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe71185516;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe71185516 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe98618108;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe98618108 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe97023010;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe97023010 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe8814567;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe8814567 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe35899815;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe35899815 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe46925835;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe46925835 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe6593509;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe6593509 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe22387913;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe22387913 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe21302938;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe21302938 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe83241390;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe83241390 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe42428249;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe42428249 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe84573621;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe84573621 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe87878805;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe87878805 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe94035743;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe94035743 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe790524;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe790524 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe26731419;     jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe26731419 = jCkpgHQVZxLTIJJWJjAUmzoZtovDAkDDXDiRNjPRvAWeBxIRxe18238089;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void UpNiUlgwErcHPZodSJAcFwpudRYHsdigEhsJggkCknUDwMFOkqAYjzeXzTEDMZDbJwDhgfxlZxrRcmxQLKXCcKwFDU14519685() {     double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH40047367 = -965030342;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH67272919 = -328439040;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH46659954 = -195956153;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH78412504 = -202184382;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH85351822 = -329272408;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH72062319 = -714173743;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH71072192 = -368420078;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH97030857 = -889392589;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH16090127 = -106170341;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH91867832 = 77959742;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH6578779 = -451969979;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH23597283 = -460577623;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH45127585 = -940825349;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH94340517 = 9199018;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH13260950 = -874094515;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH4085090 = -257756395;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH99629955 = -666901952;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH62122058 = -568563094;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH82323995 = -670887561;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH18289674 = -28685221;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH29585344 = -392377072;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH86155127 = -961648740;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH14219484 = -394535849;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH45613137 = -371618750;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH24922946 = -479454730;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH46287293 = -209274090;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH63233723 = -368317055;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH33542075 = -947854243;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH78122890 = -602846485;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH31969040 = -220158155;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH12440936 = -304815117;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH8097841 = -932805218;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH63597590 = -194387622;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH87509865 = -513773517;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH45553595 = -355783530;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH59496044 = -267756617;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH66623866 = -249172895;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH47846896 = -618082379;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH98223032 = -794176851;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH27523863 = -689606736;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH92462538 = -629198837;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH42647343 = -634940025;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH82083799 = -172184308;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH10204446 = -949628977;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH49945834 = -539755117;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH63178752 = -368553685;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH18668716 = -337827109;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH21881819 = -971268310;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH26293599 = -475161959;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH98436458 = -436595669;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH77767264 = -728841410;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH21315383 = -70303782;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH64855351 = -251522610;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH7084377 = -532761742;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH84934341 = 58490924;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH53892239 = 96618397;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH53053436 = -933903191;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH1046818 = -824337404;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH53489559 = -722729653;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH39064530 = -19998319;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH8828597 = -245856689;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH37530117 = -420565836;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH18907968 = -186546104;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH84121087 = -886012187;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH79426897 = -617225141;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH98480937 = -519164762;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH59999692 = -166190002;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH57617720 = -327051832;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH48786923 = -635017453;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH53764906 = -506337899;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH37461223 = 91416499;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH51783059 = 51180427;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH63899026 = -774386243;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH54800132 = -981280825;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH25827135 = -399486385;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH86938000 = -757437048;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH4071329 = -689464432;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH4015038 = -444906873;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH95667303 = -831863634;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH61744193 = -10901045;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH27618577 = -871446981;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH41351905 = -397048746;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH7248477 = -372692284;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH79686431 = -66250817;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH54201775 = -491316745;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH91125553 = -134511336;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH43242490 = -581282608;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH56513214 = -661625880;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH2575525 = -472264442;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH91661355 = -352401927;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH6442609 = -333853426;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH65577049 = -424835492;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH94357337 = -895352726;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH59158502 = -674178533;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH18695267 = -343750048;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH54932422 = -108633001;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH23739375 = -348393921;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH97962711 = -286172122;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH30777549 = -232403836;    double JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH51464896 = -965030342;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH40047367 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH67272919;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH67272919 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH46659954;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH46659954 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH78412504;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH78412504 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH85351822;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH85351822 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH72062319;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH72062319 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH71072192;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH71072192 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH97030857;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH97030857 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH16090127;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH16090127 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH91867832;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH91867832 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH6578779;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH6578779 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH23597283;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH23597283 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH45127585;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH45127585 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH94340517;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH94340517 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH13260950;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH13260950 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH4085090;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH4085090 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH99629955;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH99629955 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH62122058;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH62122058 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH82323995;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH82323995 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH18289674;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH18289674 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH29585344;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH29585344 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH86155127;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH86155127 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH14219484;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH14219484 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH45613137;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH45613137 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH24922946;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH24922946 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH46287293;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH46287293 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH63233723;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH63233723 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH33542075;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH33542075 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH78122890;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH78122890 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH31969040;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH31969040 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH12440936;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH12440936 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH8097841;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH8097841 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH63597590;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH63597590 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH87509865;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH87509865 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH45553595;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH45553595 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH59496044;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH59496044 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH66623866;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH66623866 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH47846896;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH47846896 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH98223032;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH98223032 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH27523863;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH27523863 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH92462538;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH92462538 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH42647343;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH42647343 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH82083799;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH82083799 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH10204446;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH10204446 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH49945834;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH49945834 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH63178752;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH63178752 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH18668716;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH18668716 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH21881819;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH21881819 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH26293599;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH26293599 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH98436458;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH98436458 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH77767264;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH77767264 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH21315383;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH21315383 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH64855351;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH64855351 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH7084377;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH7084377 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH84934341;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH84934341 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH53892239;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH53892239 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH53053436;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH53053436 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH1046818;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH1046818 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH53489559;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH53489559 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH39064530;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH39064530 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH8828597;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH8828597 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH37530117;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH37530117 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH18907968;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH18907968 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH84121087;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH84121087 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH79426897;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH79426897 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH98480937;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH98480937 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH59999692;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH59999692 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH57617720;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH57617720 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH48786923;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH48786923 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH53764906;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH53764906 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH37461223;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH37461223 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH51783059;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH51783059 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH63899026;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH63899026 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH54800132;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH54800132 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH25827135;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH25827135 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH86938000;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH86938000 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH4071329;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH4071329 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH4015038;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH4015038 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH95667303;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH95667303 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH61744193;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH61744193 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH27618577;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH27618577 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH41351905;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH41351905 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH7248477;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH7248477 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH79686431;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH79686431 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH54201775;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH54201775 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH91125553;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH91125553 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH43242490;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH43242490 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH56513214;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH56513214 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH2575525;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH2575525 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH91661355;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH91661355 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH6442609;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH6442609 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH65577049;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH65577049 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH94357337;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH94357337 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH59158502;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH59158502 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH18695267;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH18695267 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH54932422;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH54932422 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH23739375;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH23739375 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH97962711;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH97962711 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH30777549;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH30777549 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH51464896;     JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH51464896 = JMbgqLktuzJXhEsOzmHNvNVorBBAyyzxeLWlKsHggjrxGncSJNKKxpZvqnooJjuvALtViH40047367;}
// Junk Finished
