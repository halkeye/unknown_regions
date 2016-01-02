/***************************************************************************
*                           STAR WARS REALITY 1.0                          *
*--------------------------------------------------------------------------*
* Star Wars Reality Code Additions and changes from the Smaug Code         *
* copyright (c) 1997 by Sean Cooper                                        *
* -------------------------------------------------------------------------*
* Starwars and Starwars Names copyright(c) Lucas Film Ltd.                 *
*--------------------------------------------------------------------------*
* SMAUG 1.0 (C) 1994, 1995, 1996 by Derek Snider                           *
* SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,                    *
* Scryn, Rennard, Swordbearer, Gorog, Grishnakh and Tricops                *
* ------------------------------------------------------------------------ *
* Merc 2.1 Diku Mud improvments copyright (C) 1992, 1993 by Michael        *
* Chastain, Michael Quan, and Mitchell Tse.                                *
* Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,          *
* Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.     *
* ------------------------------------------------------------------------ *
* 			Table load/save Module				   *
****************************************************************************/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include "mud.h"

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

bool load_race_file( char *fname );
void write_race_file( int ra );


/* global variables */
int top_sn;
int top_herb;
int MAX_PC_RACE;

SKILLTYPE *		skill_table	[MAX_SKILL];
SKILLTYPE *		herb_table	[MAX_HERB];
SKILLTYPE *		disease_table	[MAX_DISEASE];
RACE_TYPE *		race_table	[MAX_RACE];

char * const skill_tname[] = { "unknown", "Spell", "Skill", "Weapon", "Tongue", "Herb", "Racial", "Disease" };

SPELL_FUN *spell_function( char *name )
{
    if ( !str_cmp( name, "spell_smaug" ))	     return spell_smaug;
    if ( !str_cmp( name, "spell_blindness" ))	     return spell_blindness;
    if ( !str_cmp( name, "spell_cause_critical" ))   return spell_cause_critical;
    if ( !str_cmp( name, "spell_cause_light" ))	     return spell_cause_light;
    if ( !str_cmp( name, "spell_cause_serious" ))    return spell_cause_serious;
    if ( !str_cmp( name, "spell_charm_person" ))     return spell_charm_person;
    if ( !str_cmp( name, "spell_cure_blindness" ))   return spell_cure_blindness;
    if ( !str_cmp( name, "spell_cure_poison" ))	     return spell_cure_poison;
    if ( !str_cmp( name, "spell_dispel_magic" ))     return spell_dispel_magic;
    if ( !str_cmp( name, "spell_dream" ))	     return spell_dream;
    if ( !str_cmp( name, "spell_earthquake" ))	     return spell_earthquake;
    if ( !str_cmp( name, "spell_energy_drain" ))     return spell_energy_drain;
    if ( !str_cmp( name, "spell_farsight" ))	     return spell_farsight;
    if ( !str_cmp( name, "spell_fireball" ))	     return spell_fireball;
    if ( !str_cmp( name, "spell_flamestrike" ))	     return spell_flamestrike;
    if ( !str_cmp( name, "spell_harm" ))	     return spell_harm;
    if ( !str_cmp( name, "spell_identify" ))	     return spell_identify;
    if ( !str_cmp( name, "spell_know_alignment" ))   return spell_know_alignment;
    if ( !str_cmp( name, "spell_lightning_bolt" ))   return spell_lightning_bolt;
    if ( !str_cmp( name, "spell_poison" ))	     return spell_poison;
    if ( !str_cmp( name, "spell_remove_invis" ))     return spell_remove_invis;
    if ( !str_cmp( name, "spell_shocking_grasp" ))   return spell_shocking_grasp;
    if ( !str_cmp( name, "spell_sleep" ))	     return spell_sleep;
    if ( !str_cmp( name, "spell_ventriloquate" ))    return spell_ventriloquate;
    if ( !str_cmp( name, "spell_weaken" ))	     return spell_weaken;
    if ( !str_cmp( name, "spell_scorching_surge" ))  return spell_scorching_surge;

    if ( !str_cmp( name, "spell_hand_of_chaos" ))    return spell_hand_of_chaos;
    if ( !str_cmp( name, "spell_black_lightning" ))  return spell_black_lightning;

    if ( !str_cmp( name, "reserved" ))		     return NULL;
    if ( !str_cmp( name, "spell_null" ))	     return spell_null;
    return spell_notfound;
}

DO_FUN *skill_function( char *name )
{
    switch( name[3] )
    {
    case 'a':
	if ( !str_cmp( name, "do_alertstatus" ))		return do_alertstatus;
	if ( !str_cmp( name, "do_aassign" ))		return do_aassign;
	if ( !str_cmp( name, "do_abort" ))		return do_abort;
	if ( !str_cmp( name, "do_addbounty" ))          return do_addbounty;
	if ( !str_cmp( name, "do_addpilot" ))  		return do_addpilot;
	if ( !str_cmp( name, "do_add_patrol" ))  	return do_add_patrol;
	if ( !str_cmp( name, "do_accelerate" ))         return do_accelerate;
	if ( !str_cmp( name, "do_accept" ))         return do_accept;
	if ( !str_cmp( name, "do_advance" ))		return do_advance;
	if ( !str_cmp( name, "do_affected" ))		return do_affected;
	if ( !str_cmp( name, "do_afk" ))		return do_afk;
	if ( !str_cmp( name, "do_ahelp" ))             return do_ahelp;
	if ( !str_cmp( name, "do_aid" ))		return do_aid;
	if ( !str_cmp( name, "do_allow" ))		return do_allow;
	if ( !str_cmp( name, "do_allships" ))		return do_allships;
	if ( !str_cmp( name, "do_ammo" ))		return do_ammo;
	if ( !str_cmp( name, "do_announce" ))		return do_announce;
	if ( !str_cmp( name, "do_ansi" ))		return do_ansi;
	if ( !str_cmp( name, "do_apply" ))		return do_apply;
	if ( !str_cmp( name, "do_appraise" ))		return do_appraise;
	if ( !str_cmp( name, "do_appoint" ))		return do_appoint;
	if ( !str_cmp( name, "do_aquest" ))             return do_aquest;
	if ( !str_cmp( name, "do_areas" ))		return do_areas;
	if ( !str_cmp( name, "do_arm" ))		return do_arm;
	if ( !str_cmp( name, "do_aset" ))		return do_aset;
	if ( !str_cmp( name, "do_astat" ))		return do_astat;
	if ( !str_cmp( name, "do_at" ))			return do_at;
	if ( !str_cmp( name, "do_atobj" ))		return do_atobj;
	if ( !str_cmp( name, "do_auction" ))		return do_auction;
	if ( !str_cmp( name, "do_authorize" ))		return do_authorize;
	if ( !str_cmp( name, "do_autorecharge" ))       return do_autorecharge; 
	if ( !str_cmp( name, "do_autotrack"))           return do_autotrack;
	if ( !str_cmp( name, "do_autopilot"))           return do_autopilot;
	if ( !str_cmp( name, "do_autopsy"))           return do_autopsy; /* Added by Gavin Oct 19 2000 */
	if ( !str_cmp( name, "do_avtalk" ))		return do_avtalk;
	break;
    case 'b':
	if ( !str_cmp( name, "do_backstab" ))		return do_backstab;
	if ( !str_cmp( name, "do_balzhur" ))		return do_balzhur;
	if ( !str_cmp( name, "do_bamfin" ))		return do_bamfin;
	if ( !str_cmp( name, "do_bamfout" ))		return do_bamfout;
	if ( !str_cmp( name, "do_ban" ))		return do_ban;
	if ( !str_cmp( name, "do_bank" ))		return do_bank;
	if ( !str_cmp( name, "do_bash" ))		return do_bash;
	if ( !str_cmp( name, "do_bashdoor" ))		return do_bashdoor;
	if ( !str_cmp( name, "do_beep" ))               return do_beep;
	if ( !str_cmp( name, "do_beg" ))                return do_beg;
	if ( !str_cmp( name, "do_berserk" ))		return do_berserk;
	if ( !str_cmp( name, "do_bestow" ))		return do_bestow;
	if ( !str_cmp( name, "do_bestowarea" ))		return do_bestowarea;
	if ( !str_cmp( name, "do_bio" ))		return do_bio;
	if ( !str_cmp( name, "do_birth" ))		return do_birth;
	if ( !str_cmp( name, "do_blackjack" ))		return do_blackjack;
	if ( !str_cmp( name, "do_board" ))              return do_board;
	if ( !str_cmp( name, "do_boards" ))		return do_boards;
	if ( !str_cmp( name, "do_bodybag" ))		return do_bodybag;
	if ( !str_cmp( name, "do_bomb" ))		return do_bomb;
	if ( !str_cmp( name, "do_bounties" ))             return do_bounties;
	if ( !str_cmp( name, "do_breakup" ))               return do_breakup;
	if ( !str_cmp( name, "do_bribe" ))		return do_bribe;
	if ( !str_cmp( name, "do_bset" ))		return do_bset;
	if ( !str_cmp( name, "do_bstat" ))		return do_bstat;
	if ( !str_cmp( name, "do_bug" ))		return do_bug;
	if ( !str_cmp( name, "do_bugs" ))		return do_bugs;
	if ( !str_cmp( name, "do_bury" ))		return do_bury;
	if ( !str_cmp( name, "do_butcher" ))	return do_butcher;
	if ( !str_cmp( name, "do_buy" ))		return do_buy;
	if ( !str_cmp( name, "do_buyhome" ))            return do_buyhome; 
	if ( !str_cmp( name, "do_buyship" ))            return do_buyship; 
	if ( !str_cmp( name, "do_buyvendor" ))		return do_buyvendor; /* Added by Gavin - July 18 2000 */
	break;
    case 'c':
	if ( !str_cmp( name, "do_calculate" ))          return do_calculate; 
	if ( !str_cmp( name, "do_copyover" ))          return do_copyover; 
	if ( !str_cmp( name, "do_call" ))		return do_call; /* Added by Gavin - Mar 11 2001 */
	if ( !str_cmp( name, "do_capture" ))            return do_capture; 
	if ( !str_cmp( name, "do_carry" ))            return do_carry;  /* Added by Gavin - August 24 2000 */
	if ( !str_cmp( name, "do_cast" ))		return do_cast;
	if ( !str_cmp( name, "do_cedit" ))		return do_cedit;
	if ( !str_cmp( name, "do_chaff" ))		return do_chaff;
	if ( !str_cmp( name, "do_channels" ))		return do_channels;
	if ( !str_cmp( name, "do_chanuser" ))		return do_chanuser;
	if ( !str_cmp( name, "do_chat" ))		return do_chat;
	if ( !str_cmp( name, "do_check_vnums" ))	return do_check_vnums;
	if ( !str_cmp( name, "do_clans" ))		return do_clans;
	if ( !str_cmp( name, "do_clan_donate" ))	return do_clan_donate;
	if ( !str_cmp( name, "do_clan_withdraw" ))	return do_clan_withdraw;
	if ( !str_cmp( name, "do_clantalk" ))		return do_clantalk;
	if ( !str_cmp( name, "do_clanbuyship" ))        return do_clanbuyship; 
	if ( !str_cmp( name, "do_clansellship" ))           return do_clansellship;
	if ( !str_cmp( name, "do_climate" ))		return do_climate;
	if ( !str_cmp( name, "do_climb" ))		return do_climb;
	if ( !str_cmp( name, "do_close" ))		return do_close;
	if ( !str_cmp( name, "do_clone" ))		return do_clone;
	if ( !str_cmp( name, "do_clones" ))		return do_clones;
	if ( !str_cmp( name, "do_closebay" ))  		return do_closebay;
	if ( !str_cmp( name, "do_closehatch"  ))        return do_closehatch; 
	if ( !str_cmp( name, "do_cmdtable" ))		return do_cmdtable;
	if ( !str_cmp( name, "do_collectgold" ))		return do_collectgold; /* Added by Gavin - July 18 2000 */
	if ( !str_cmp( name, "do_commands" ))		return do_commands;
	if ( !str_cmp( name, "do_comment" ))		return do_comment;
	if ( !str_cmp( name, "do_compare" ))		return do_compare;
	if ( !str_cmp( name, "do_config" ))		return do_config;
	if ( !str_cmp( name, "do_consent" ))		return do_consent;
	if ( !str_cmp( name, "do_consider" ))		return do_consider;
	if ( !str_cmp( name, "do_cosmetic" ))		return do_cosmetic; /* Added by Gavin Oct 1 2000 */
	if ( !str_cmp( name, "do_copyship" ))		return do_copyship;
	if ( !str_cmp( name, "do_council_induct" ))		return do_council_induct;
	if ( !str_cmp( name, "do_council_outcast" ))		return do_council_outcast;
	if ( !str_cmp( name, "do_councilbuyhome" ))		return do_councilbuyhome;
	if ( !str_cmp( name, "do_councils" ))		return do_councils;
	if ( !str_cmp( name, "do_councilsellhome" ))		return do_councilsellhome;
	if ( !str_cmp( name, "do_credits" ))		return do_credits;
	if ( !str_cmp( name, "do_cset" ))		return do_cset;
	if ( !str_cmp( name, "do_cyber" ))		return do_cyber;
	break;
    case 'd':
	if ( !str_cmp( name, "do_demote" ))		return do_demote;
	if ( !str_cmp( name, "do_delay"	))		return do_delay;
	if ( !str_cmp( name, "do_deny" ))		return do_deny;
	if ( !str_cmp( name, "do_description" ))	return do_description;
	if ( !str_cmp( name, "do_destro" ))		return do_destro;
	if ( !str_cmp( name, "do_destroy" ))		return do_destroy;
#ifdef RESTORE_ADDON
	if ( !str_cmp( name, "do_destroyrestore" ))		return do_destroyrestore; /* May 27 2000 - Gavin */
#endif
	if ( !str_cmp( name, "do_destroyslay" ))		return do_destroyslay;
	if ( !str_cmp( name, "do_diagnose" ))		return do_diagnose;
	if ( !str_cmp( name, "do_dig" ))		return do_dig;
	if ( !str_cmp( name, "do_disarm" ))		return do_disarm;
	if ( !str_cmp( name, "do_disconnect" ))		return do_disconnect;
	if ( !str_cmp( name, "do_disguise" ))           return do_disguise;
	if ( !str_cmp( name, "do_dismount" ))		return do_dismount;
	if ( !str_cmp( name, "do_divorce" ))		return do_divorce;
	if ( !str_cmp( name, "do_dmesg" ))		return do_dmesg;
	if ( !str_cmp( name, "do_dnd" ))		return do_dnd;
	if ( !str_cmp( name, "do_dock" ))		return do_dock;
	if ( !str_cmp( name, "do_down" ))		return do_down;
	if ( !str_cmp( name, "do_drag" ))		return do_drag;
	if ( !str_cmp( name, "do_drink" ))		return do_drink;
	if ( !str_cmp( name, "do_drive" ))		return do_drive;
	if ( !str_cmp( name, "do_drop" ))		return do_drop;
	break;
    case 'e':
	if ( !str_cmp( name, "do_east" ))		return do_east;
	if ( !str_cmp( name, "do_endsimulator" ))	return do_endsimulator; 
	if ( !str_cmp( name, "do_eat" ))		return do_eat;
	if ( !str_cmp( name, "do_ech" ))		return do_ech;
	if ( !str_cmp( name, "do_echo" ))		return do_echo;
	if ( !str_cmp( name, "do_elite_guard" ))	return do_elite_guard;
	if ( !str_cmp( name, "do_emote" ))		return do_emote;
	if ( !str_cmp( name, "do_empty" ))		return do_empty;
	if ( !str_cmp( name, "do_empower" ))		return do_empower;
	if ( !str_cmp( name, "do_enlist" ))		return do_enlist;
	if ( !str_cmp( name, "do_enter" ))		return do_enter;
	if ( !str_cmp( name, "do_equipment" ))		return do_equipment;
	if ( !str_cmp( name, "do_examine" ))		return do_examine;
	if ( !str_cmp( name, "do_exits" ))		return do_exits;
	break;
    case 'f':
	if ( !str_cmp( name, "do_fill" ))		return do_fill;
	if ( !str_cmp( name, "do_fire" ))               return do_fire; 
	if ( !str_cmp( name, "do_first_aid" ))          return do_first_aid; 
	if ( !str_cmp( name, "do_fixchar" ))		return do_fixchar;
	if ( !str_cmp( name, "do_flee" ))		return do_flee;
	if ( !str_cmp( name, "do_foldarea" ))		return do_foldarea;
	if ( !str_cmp( name, "do_follow" ))		return do_follow;
	if ( !str_cmp( name, "do_for" ))		return do_for;
	if ( !str_cmp( name, "do_force" ))		return do_force;
	if ( !str_cmp( name, "do_forceclose" ))		return do_forceclose;
	if ( !str_cmp( name, "do_form_password" ))	return do_form_password;
	if ( !str_cmp( name, "do_fprompt" ))		return do_fprompt;
	if ( !str_cmp( name, "do_fquit" ))		return do_fquit;
	if ( !str_cmp( name, "do_freeze" ))		return do_freeze;
	if ( !str_cmp( name, "do_freehomes" ))		return do_freehomes; /* Added by Gavin - Nov 05 2000 */
	if ( !str_cmp( name, "do_freeships" ))		return do_freeships;
	if ( !str_cmp( name, "do_free_vnums" ))		return do_free_vnums;
	if ( !str_cmp( name, "do_fullname" ))		return do_fullname; /* Added by Gavin - Sept 09 2000 */
	break;
    case 'g':
	if ( !str_cmp( name, "do_gather_intelligence" )) return do_gather_intelligence;
	if ( !str_cmp( name, "do_get" ))		return do_get;
	if ( !str_cmp( name, "do_getcode" ))		return do_getcode;
	if ( !str_cmp( name, "do_gemcutting" ))		return do_gemcutting;
	if ( !str_cmp( name, "do_gemote" ))			return do_gemote;
	if ( !str_cmp( name, "do_gfighting" ))		return do_gfighting;
	if ( !str_cmp( name, "do_give" ))		return do_give;
	if ( !str_cmp( name, "do_glance" ))		return do_glance;
        if ( !str_cmp( name, "do_gold" ))               return do_gold;
	if ( !str_cmp( name, "do_goto" ))		return do_goto;
	if ( !str_cmp( name, "do_gouge" ))		return do_gouge;
	if ( !str_cmp( name, "do_group" ))		return do_group;
	if ( !str_cmp( name, "do_gtell" ))		return do_gtell;
	if ( !str_cmp( name, "do_gwhere" ))		return do_gwhere;
	break;
    case 'h':
	if ( !str_cmp( name, "do_hail" ))		return do_hail;
	if ( !str_cmp( name, "do_hedit" ))		return do_hedit;
	if ( !str_cmp( name, "do_hell" ))		return do_hell;
	if ( !str_cmp( name, "do_help" ))		return do_help;
	if ( !str_cmp( name, "do_hide" ))		return do_hide;
	if ( !str_cmp( name, "do_hijack" ))		return do_hijack;
	if ( !str_cmp( name, "do_hitall" ))		return do_hitall;
	if ( !str_cmp( name, "do_hlist" ))		return do_hlist;
	if ( !str_cmp( name, "do_holylight" ))		return do_holylight;
	if ( !str_cmp( name, "do_homeid" ))		return do_homeid;
	if ( !str_cmp( name, "do_homes" ))		return do_homes;
	if ( !str_cmp( name, "do_hset" ))		return do_hset;
	if ( !str_cmp( name, "do_hyperspace" ))         return do_hyperspace; 
	break;
    case 'i':
	if ( !str_cmp( name, "do_ide" ))		return do_ide;
	if ( !str_cmp( name, "do_idea" ))		return do_idea;
	if ( !str_cmp( name, "do_ideas" ))		return do_ideas;
	if ( !str_cmp( name, "do_ignore" ))		return do_ignore;
	if ( !str_cmp( name, "do_immortalize" ))	return do_immortalize;
	if ( !str_cmp( name, "do_immtalk" ))		return do_immtalk;
	if ( !str_cmp( name, "do_induct" ))		return do_induct;
	if ( !str_cmp( name, "do_info" ))               return do_info; 
	if ( !str_cmp( name, "do_inject" ))			return do_inject; /* March 24 '01 - Gavin */
	if ( !str_cmp( name, "do_installarea" ))	return do_installarea;
	if ( !str_cmp( name, "do_instaroom" ))		return do_instaroom;
	if ( !str_cmp( name, "do_instazone" ))		return do_instazone;
	if ( !str_cmp( name, "do_inventory" ))		return do_inventory;
	if ( !str_cmp( name, "do_invis" ))		return do_invis;

	if ( !str_cmp( name, "do_invite" ))		return do_invite;
	if ( !str_cmp( name, "do_ipcompare" )) return do_ipcompare;
	break;
    case 'j':
	if ( !str_cmp( name, "do_jail" ))  	        return do_jail;
	break;
    case 'k':
	if ( !str_cmp( name, "do_kick" ))		return do_kick;
	if ( !str_cmp( name, "do_kill" ))		return do_kill;
	if ( !str_cmp( name, "do_khistory" ))		return do_khistory;
	if ( !str_cmp( name, "do_knock" ))		return do_knock;
	break;
    case 'l':
	if ( !str_cmp( name, "do_languages" ))		return do_languages;
	if ( !str_cmp( name, "do_last" ))		return do_last;
	if ( !str_cmp( name, "do_land"))                return do_land;
	if ( !str_cmp( name, "do_launch"))              return do_launch;
	if ( !str_cmp( name, "do_leave" ))		return do_leave;
	if ( !str_cmp( name, "do_leaveship" ))		return do_leaveship;
	if ( !str_cmp( name, "do_level" ))		return do_level;
	if ( !str_cmp( name, "do_light" ))		return do_light;
	if ( !str_cmp( name, "do_list" ))		return do_list;
	if ( !str_cmp( name, "do_listships" ))	return do_listships;
	if ( !str_cmp( name, "do_listen" ))		return do_listen;
	if ( !str_cmp( name, "do_litterbug" ))		return do_litterbug;
	if ( !str_cmp( name, "do_loadarea" ))		return do_loadarea;
	if ( !str_cmp( name, "do_loadup" ))		return do_loadup;
	if ( !str_cmp( name, "do_lock" ))		return do_lock;
	if ( !str_cmp( name, "do_log" ))		return do_log;
	if ( !str_cmp( name, "do_look" ))		return do_look;
	if ( !str_cmp( name, "do_low_purge" ))		return do_low_purge;
	break;
    case 'm':
	if ( !str_cmp( name, "do_mailroom" ))		return do_mailroom;
	if ( !str_cmp( name, "do_make" ))		return do_make;
	if ( !str_cmp( name, "do_make_infect_med" )) return do_make_infect_med; /* Mar 23 '01 - Gavin */
	if ( !str_cmp( name, "do_makearmor" ))		return do_makearmor;
	if ( !str_cmp( name, "do_makeblade" ))          return do_makeblade;
	if ( !str_cmp( name, "do_makeblaster" ))        return do_makeblaster;
	if ( !str_cmp( name, "do_makeboard" ))		return do_makeboard;
	if ( !str_cmp( name, "do_makecomlink" ))	return do_makecomlink;
	if ( !str_cmp( name, "do_makecontainer" ))	return do_makecontainer;
	if ( !str_cmp( name, "do_makeclan" ))		return do_makeclan;
	if ( !str_cmp( name, "do_makecouncil" ))		return do_makecouncil;
	if ( !str_cmp( name, "do_makedroid" ))        return do_makedroid;
	if ( !str_cmp( name, "do_makefurniture" ))        return do_makefurniture;
	if ( !str_cmp( name, "do_makegrenade" ))	return do_makegrenade;
	if ( !str_cmp( name, "do_makehome" ))		return do_makehome;
	if ( !str_cmp( name, "do_makejewelry" ))	return do_makejewelry;
	if ( !str_cmp( name, "do_makekeyring" ))	return do_makekeyring;
	if ( !str_cmp( name, "do_makelight" ))		return do_makelight;
	if ( !str_cmp( name, "do_makelandmine" ))	return do_makelandmine;
	if ( !str_cmp( name, "do_makemissile" ))	return do_makemissile;
	if ( !str_cmp( name, "do_makemedkit" ))		return do_makemedkit; /* Added By Gavin - August 08 2000 */
	if ( !str_cmp( name, "do_makeplanet" ))	        return do_makeplanet;
	if ( !str_cmp( name, "do_makeprototype" ))	        return do_makeprototype; /* Mar 28 '01 - Gavin */
	if ( !str_cmp( name, "do_makerepair" ))		return do_makerepair;
#ifdef RESTORE_ADDON
	if ( !str_cmp( name, "do_makerestore" ))		return do_makerestore; /* May 27 2000 - Gavin */
#endif
	if ( !str_cmp( name, "do_makespice" ))          return do_makespice;
	if ( !str_cmp( name, "do_makeshield" ))		return do_makeshield;
	if ( !str_cmp( name, "do_makeship" ))           return do_makeship;
	if ( !str_cmp( name, "do_makeslay" ))           return do_makeslay;
	if ( !str_cmp( name, "do_makeshop" ))		return do_makeshop;
	if ( !str_cmp( name, "do_makestarsystem" ))     return do_makestarsystem;
	if ( !str_cmp( name, "do_makewizlist" ))	return do_makewizlist;
	if ( !str_cmp( name, "do_marry" ))	        return do_marry;
	if ( !str_cmp( name, "do_massign" ))		return do_massign;
	if ( !str_cmp( name, "do_mass_propeganda" ))	return do_mass_propeganda;
	if ( !str_cmp( name, "do_mcreate" ))		return do_mcreate;
	if ( !str_cmp( name, "do_mdelete" ))		return do_mdelete;
	if ( !str_cmp( name, "do_memory" ))		return do_memory;
	if ( !str_cmp( name, "do_mfind" ))		return do_mfind;
	if ( !str_cmp( name, "do_minvoke" ))		return do_minvoke;
	if ( !str_cmp( name, "do_mine" ))		return do_mine;
	if ( !str_cmp( name, "do_mlist" ))		return do_mlist;
	if ( !str_cmp( name, "do_mount" ))		return do_mount;
	if ( !str_cmp( name, "do_mp_close_passage" ))	return do_mp_close_passage;
	if ( !str_cmp( name, "do_mp_damage" ))		return do_mp_damage;
	if ( !str_cmp( name, "do_mp_deposit" ))		return do_mp_deposit;
	if ( !str_cmp( name, "do_mp_open_passage" ))	return do_mp_open_passage;
	if ( !str_cmp( name, "do_mp_practice" ))	return do_mp_practice;
	if ( !str_cmp( name, "do_mp_restore" ))		return do_mp_restore;
	if ( !str_cmp( name, "do_mp_slay" ))		return do_mp_slay;
	if ( !str_cmp( name, "do_mp_withdraw" ))	return do_mp_withdraw;
	if ( !str_cmp( name, "do_mpadvance" ))		return do_mpadvance;
	if ( !str_cmp( name, "do_mpapply" ))		return do_mpapply;
	if ( !str_cmp( name, "do_mpapplyb" ))		return do_mpapplyb;
	if ( !str_cmp( name, "do_mpasound" ))		return do_mpasound;
	if ( !str_cmp( name, "do_mpat" ))		return do_mpat;
	if ( !str_cmp( name, "do_mpbodybag") )		return do_mpbodybag;
	if ( !str_cmp( name, "do_mpcopy" ))		return do_mpcopy;
	if ( !str_cmp( name, "do_mpdelay" ))		return do_mpdelay;
	if ( !str_cmp( name, "do_mpdream" ))		return do_mpdream;
	if ( !str_cmp( name, "do_mpecho" ))		return do_mpecho;
	if ( !str_cmp( name, "do_mprat" ))		return do_mprat;
	if ( !str_cmp( name, "do_mpechoaround" ))	return do_mpechoaround;
	if ( !str_cmp( name, "do_mpechoat" ))		return do_mpechoat;
	if ( !str_cmp( name, "do_mpedit" ))		return do_mpedit;
	if ( !str_cmp( name, "do_mpgain" ))		return do_mpgain;
	if ( !str_cmp( name, "do_mpforce" ))		return do_mpforce;
	if ( !str_cmp( name, "do_mpgoto" ))		return do_mpgoto;
	if ( !str_cmp( name, "do_mpinvis" ))		return do_mpinvis;
	if ( !str_cmp( name, "do_mpjunk" ))		return do_mpjunk;
	if ( !str_cmp( name, "do_mpkill" ))		return do_mpkill;
	if ( !str_cmp( name, "do_mpmload" ))		return do_mpmload;
	if ( !str_cmp( name, "do_mpnothing" ))		return do_mpnothing;
	if ( !str_cmp( name, "do_mpnuisance"))		return do_mpnuisance;
	if ( !str_cmp( name, "do_mpoload" ))		return do_mpoload;
	if ( !str_cmp( name, "do_mppkset" ))		return do_mppkset;
	if ( !str_cmp( name, "do_mpscatter" ))          return do_mpscatter;
	if ( !str_cmp( name, "do_mppurge" ))		return do_mppurge;
	if ( !str_cmp( name, "do_mpstat" ))		return do_mpstat;
	if ( !str_cmp( name, "do_mptransfer" ))		return do_mptransfer;
	if ( !str_cmp( name, "do_mpunnuisance" ) )	return do_mpunnuisance;
	if ( !str_cmp( name, "do_mpfind" ))		return do_mpfind;
	if ( !str_cmp( name, "do_mset" ))		return do_mset;
	if ( !str_cmp( name, "do_mstat" ))		return do_mstat;
	if ( !str_cmp( name, "do_murde" ))		return do_murde;
	if ( !str_cmp( name, "do_murder" ))		return do_murder;
	if ( !str_cmp( name, "do_music" ))		return do_music;
	if ( !str_cmp( name, "do_mwhere" ))		return do_mwhere;
	if ( !str_cmp( name, "do_myclan" ))		return do_myclan;
	if ( !str_cmp( name, "do_mxp" ))		return do_mxp;

	break;
    case 'n':
	if ( !str_cmp( name, "do_name" ))		return do_name;
	if ( !str_cmp( name, "do_newbiechat" ))		return do_newbiechat;
	if ( !str_cmp( name, "do_newbieset" ))		return do_newbieset;
	if ( !str_cmp( name, "do_news" ))		return do_news;
	if ( !str_cmp( name, "do_newzones" ))		return do_newzones;
	if ( !str_cmp( name, "do_noemote" ))		return do_noemote;
	if ( !str_cmp( name, "do_noresolve" ))		return do_noresolve;
	if ( !str_cmp( name, "do_north" ))		return do_north;
	if ( !str_cmp( name, "do_northeast" ))		return do_northeast;
	if ( !str_cmp( name, "do_northwest" ))		return do_northwest;
	if ( !str_cmp( name, "do_notell" ))		return do_notell;
        if ( !str_cmp( name, "do_notitle" ))            return do_notitle;
	if ( !str_cmp( name, "do_noteroom" ))		return do_noteroom;
	if ( !str_cmp( name, "do_nuisance" ))		return do_nuisance;
	break;
    case 'o':
	if ( !str_cmp( name, "do_oassign" ))		return do_oassign;
	if ( !str_cmp( name, "do_ocreate" ))		return do_ocreate;
	if ( !str_cmp( name, "do_odelete" ))		return do_odelete;
	if ( !str_cmp( name, "do_ofind" ))		return do_ofind;
	if ( !str_cmp( name, "do_oinvoke" ))		return do_oinvoke;
	if ( !str_cmp( name, "do_oldscore" ))		return do_oldscore;
	if ( !str_cmp( name, "do_olist" ))		return do_olist;
	if ( !str_cmp( name, "do_ooc" ))		return do_ooc;
	if ( !str_cmp( name, "do_opcopy" ))		return do_opcopy;
	if ( !str_cmp( name, "do_opedit" ))		return do_opedit;
	if ( !str_cmp( name, "do_open" ))		return do_open;
	if ( !str_cmp( name, "do_opfind" ))		return do_opfind;
	if ( !str_cmp( name, "do_openbay" ))  		return do_openbay;
	if ( !str_cmp( name, "do_openhatch" ))          return do_openhatch; 
	if ( !str_cmp( name, "do_opstat" ))		return do_opstat;
	if ( !str_cmp( name, "do_order" ))		return do_order;
	if ( !str_cmp( name, "do_order_planet" )) return do_order_planet;
	if ( !str_cmp( name, "do_oset" ))		return do_oset;
	if ( !str_cmp( name, "do_ostat" ))		return do_ostat;
	if ( !str_cmp( name, "do_outcast" ))		return do_outcast;
	if ( !str_cmp( name, "do_overthrow" ))		return do_overthrow;
	if ( !str_cmp( name, "do_owhere" ))		return do_owhere;
	break;
    case 'p':
	if ( !str_cmp( name, "do_pager" ))		return do_pager;
	if ( !str_cmp( name, "do_paint" ))		return do_paint;
	if ( !str_cmp( name, "do_password" ))		return do_password;
	if ( !str_cmp( name, "do_payfine" ))		return do_payfine;
	if ( !str_cmp( name, "do_pcrename" ))		return do_pcrename;
	if ( !str_cmp( name, "do_peace" ))		return do_peace;
	if ( !str_cmp( name, "do_petname" ))		return do_petname; /*Added By Gavin - July 14 2000 */
	if ( !str_cmp( name, "do_pick" ))		return do_pick;
	if ( !str_cmp( name, "do_pickshiplock" ))	return do_pickshiplock;
	if ( !str_cmp( name, "do_placevendor" ))		return do_placevendor; /* Added by Gavin - July 18 2000 */
	if ( !str_cmp( name, "do_planets" ))		return do_planets;
	if ( !str_cmp( name, "do_pluogus" ))		return do_pluogus;
	if ( !str_cmp( name, "do_poison_weapon" ))	return do_poison_weapon;
	if ( !str_cmp( name, "do_postguard" ))		return do_postguard;
	if ( !str_cmp( name, "do_practice" ))		return do_practice;
	if ( !str_cmp( name, "do_pricevendor" ))		return do_pricevendor; /* Added by Gavin - July 18 2000 */
	if ( !str_cmp( name, "do_prompt" ))		return do_prompt;
	if ( !str_cmp( name, "do_promote" ))		return do_promote; 
	if ( !str_cmp( name, "do_propeganda" ))		return do_propeganda;
	if ( !str_cmp( name, "do_propose" ))		return do_propose;
	if ( !str_cmp( name, "do_prototypes" ))		return do_prototypes; /* Mar 28 '01 - Gavin */
	if ( !str_cmp( name, "do_pshield" ))		return do_pshield; /* Added by Gavin - July 22 2000 */

	if ( !str_cmp( name, "do_pull" ))		return do_pull;
	if ( !str_cmp( name, "do_punch" ))		return do_punch;
	if ( !str_cmp( name, "do_purge" ))		return do_purge;
	if ( !str_cmp( name, "do_push" ))		return do_push;
	if ( !str_cmp( name, "do_put" ))		return do_put;
	break;
    case 'q':
	if ( !str_cmp( name, "do_qpset" ))		return do_qpset;
	if ( !str_cmp( name, "do_qpstat" ))		return do_qpstat;
	if ( !str_cmp( name, "do_quaff" ))		return do_quaff;
	if ( !str_cmp( name, "do_quest" ))		return do_quest;
	if ( !str_cmp( name, "do_qui" ))		return do_qui;
	if ( !str_cmp( name, "do_quit" ))		return do_quit;
	break;
    case 'r':
	if ( !str_cmp( name, "do_radar" )) 		return do_radar;
	if ( !str_cmp( name, "do_rank" ))		return do_rank;
	if ( !str_cmp( name, "do_rassign" ))		return do_rassign;
	if ( !str_cmp( name, "do_rap" ))		return do_rap;
	if ( !str_cmp( name, "do_rat" ))		return do_rat;
	if ( !str_cmp( name, "do_rcreate" ))		return do_rcreate; /* Added By Gavin - May 11 2001 */
	if ( !str_cmp( name, "do_rdelete" ))		return do_rdelete;
	if ( !str_cmp( name, "do_reboo" ))		return do_reboo;
	if ( !str_cmp( name, "do_reboot" ))		return do_reboot;
	if ( !str_cmp( name, "do_recharge" ))           return do_recharge; 
	if ( !str_cmp( name, "do_recho" ))		return do_recho;
	if ( !str_cmp( name, "do_recall" ))              return do_recall;
    if ( !str_cmp( name, "do_redit" ))              return do_redit;
	if ( !str_cmp( name, "do_refuel" ))             return do_refuel; 
	if ( !str_cmp( name, "do_reinforcements" ))	return do_reinforcements;
	if ( !str_cmp( name, "do_regoto" ))		return do_regoto;
	if ( !str_cmp( name, "do_rembounty" ))  		return do_rembounty;
	if ( !str_cmp( name, "do_remove" ))		return do_remove;
	if ( !str_cmp( name, "do_rempilot" ))           return do_rempilot; 
	if ( !str_cmp( name, "do_renameship" ))		return do_renameship;
	if ( !str_cmp( name, "do_rent" ))		return do_rent;
	if ( !str_cmp( name, "do_repair" ))		return do_repair;
	if ( !str_cmp( name, "do_repairset" ))		return do_repairset;
	if ( !str_cmp( name, "do_repairship" ))         return do_repairship; 
	if ( !str_cmp( name, "do_repairshops" ))	return do_repairshops;
	if ( !str_cmp( name, "do_repairstat" ))		return do_repairstat;
	if ( !str_cmp( name, "do_reply" ))		return do_reply;
	if ( !str_cmp( name, "do_report" ))		return do_report;
	if ( !str_cmp( name, "do_repulsor" ))	return do_repulsor;
	if ( !str_cmp( name, "do_rescue" ))		return do_rescue;
	if ( !str_cmp( name, "do_reserve" ))		return do_reserve;
	if ( !str_cmp( name, "do_reset" ))		return do_reset;
	if ( !str_cmp( name, "do_resethome" ))		return do_resethome;
	if ( !str_cmp( name, "do_resetship" ))		return do_resetship;
	if ( !str_cmp( name, "do_resign" ))		return do_resign;
	if ( !str_cmp( name, "do_rest" ))		return do_rest;
	if ( !str_cmp( name, "do_restore" ))		return do_restore;
	if ( !str_cmp( name, "do_restoretime" ))	return do_restoretime;
	if ( !str_cmp( name, "do_restrict" ))		return do_restrict;
	if ( !str_cmp( name, "do_retell" ))		return do_retell;
	if ( !str_cmp( name, "do_retire" ))		return do_retire;
	if ( !str_cmp( name, "do_retran" ))		return do_retran;
	if ( !str_cmp( name, "do_return" ))		return do_return;
	if ( !str_cmp( name, "do_rlist" ))		return do_rlist;
	if ( !str_cmp( name, "do_roommate" ))	return do_roommate; /* Added by Gavin - Sept 10 '00 */
	if ( !str_cmp( name, "do_rpcopy" ))		return do_rpcopy;
	if ( !str_cmp( name, "do_rpedit" ))		return do_rpedit;
	if ( !str_cmp( name, "do_rpfind" ))		return do_rpfind;
	if ( !str_cmp( name, "do_rpstat" ))		return do_rpstat;
	if ( !str_cmp( name, "do_rreset" ))		return do_rreset;
	if ( !str_cmp( name, "do_rstat" ))		return do_rstat;
	if ( !str_cmp( name, "do_run" ))		return do_run; /* Added 01/21/01 by Drraagh */
	break;
    case 's':
	if ( !str_cmp( name, "do_sacrifice" ))		return do_sacrifice;
	if ( !str_cmp( name, "do_save" ))		return do_save;
	if ( !str_cmp( name, "do_saveall" ))	return do_saveall;
	if ( !str_cmp( name, "do_savearea" ))		return do_savearea;
	if ( !str_cmp( name, "do_say" ))		return do_say;
	if ( !str_cmp( name, "do_scan" ))		return do_scan;
	if ( !str_cmp( name, "do_scatter" ))	return do_scatter;
	if ( !str_cmp( name, "do_score" ))		return do_score;
	if ( !str_cmp( name, "do_scribe" ))		return do_scribe;
	if ( !str_cmp( name, "do_search" ))		return do_search;
	if ( !str_cmp( name, "do_sedit" ))		return do_sedit;
	if ( !str_cmp( name, "do_seduce" ))		return do_seduce;
	if ( !str_cmp( name, "do_sell" ))		return do_sell;
	if ( !str_cmp( name, "do_sellhome" ))			return do_sellhome; 
	if ( !str_cmp( name, "do_sellship" ))			return do_sellship;
	if ( !str_cmp( name, "do_sendcode" ))			return do_sendcode;	
	if ( !str_cmp( name, "do_set_boot_time" ))		return do_set_boot_time;
	if ( !str_cmp( name, "do_setblaster" ))			return do_setblaster;
	if ( !str_cmp( name, "do_setclan" ))			return do_setclan;
	if ( !str_cmp( name, "do_setcode" ))			return do_setcode;
	if ( !str_cmp( name, "do_setcouncil" ))			return do_setcouncil;
	if ( !str_cmp( name, "do_sethome" ))			return do_sethome;
	if ( !str_cmp( name, "do_setplanet" ))	        return do_setplanet;
	if ( !str_cmp( name, "do_setprototype" ))	return do_setprototype; /* Mar 28 '01 - Gavin */
	if ( !str_cmp( name, "do_setship" ))            return do_setship;
	if ( !str_cmp( name, "do_setslay" ))            return do_setslay;
	if ( !str_cmp( name, "do_setstarsystem" ))      return do_setstarsystem;
	if ( !str_cmp( name, "do_setsystemcode" ))		return do_setsystemcode;
	if ( !str_cmp( name, "do_setrace" ))			return do_setrace;
#ifdef RESTORE_ADDON
	if ( !str_cmp( name, "do_setrestore" ))			return do_setrestore; /* May 27 2000 - Gavin */
#endif
	if ( !str_cmp( name, "do_setweather" ))			return do_setweather;
	if ( !str_cmp( name, "do_sexes" ))	        return do_sexes;
	if ( !str_cmp( name, "do_ship_masking" ))              return do_ship_masking; /* Mar 18 2001 - By Gavin */
	if ( !str_cmp( name, "do_ships" ))              return do_ships;
	if ( !str_cmp( name, "do_shops" ))		return do_shops;
	if ( !str_cmp( name, "do_shopset" ))		return do_shopset;
	if ( !str_cmp( name, "do_shopstat" ))		return do_shopstat;
	if ( !str_cmp( name, "do_shout" ))		return do_shout;
	if ( !str_cmp( name, "do_shove" ))		return do_shove;
	if ( !str_cmp( name, "do_showclan" ))		return do_showclan;
	if ( !str_cmp( name, "do_showcouncil" ))		return do_showcouncil;
	if ( !str_cmp( name, "do_showrace" ))		return do_showrace;
	if ( !str_cmp( name, "do_showhome" ))		return do_showhome;
	if ( !str_cmp( name, "do_showplanet" ))           return do_showplanet;
	if ( !str_cmp( name, "do_showprototype" ))	return do_showprototype; /* Mar 28 '01 - Gavin */
	if ( !str_cmp( name, "do_showship" ))           return do_showship;
	if ( !str_cmp( name, "do_showslay" ))           return do_showslay;
	if ( !str_cmp( name, "do_showstarsystem" ))     return do_showstarsystem;
#ifdef RESTORE_ADDON
	if ( !str_cmp( name, "do_showrestore" ))		return do_showrestore; /* May 27 2000 - Gavin */
#endif
	if ( !str_cmp( name, "do_showweather" ))		return do_showweather;
	if ( !str_cmp( name, "do_shutdow" ))		return do_shutdow;
	if ( !str_cmp( name, "do_shutdown" ))		return do_shutdown;
	if ( !str_cmp( name, "do_silence" ))		return do_silence;
	if ( !str_cmp( name, "do_sit" ))		return do_sit;
	if ( !str_cmp( name, "do_sla" ))		return do_sla;
	if ( !str_cmp( name, "do_slay" ))		return do_slay;
	if ( !str_cmp( name, "do_sleep" ))		return do_sleep;
	if ( !str_cmp( name, "do_slice" ))		return do_slice;
	if ( !str_cmp( name, "do_slist" ))		return do_slist;
	if ( !str_cmp( name, "do_slookup" ))		return do_slookup;
	if ( !str_cmp( name, "do_smalltalk" ))		return do_smalltalk;
	if ( !str_cmp( name, "do_smoke" ))		return do_smoke;
	if ( !str_cmp( name, "do_snipe" ))		return do_snipe;
	if ( !str_cmp( name, "do_snoop" ))		return do_snoop;
	if ( !str_cmp( name, "do_sober" ))		return do_sober;
	if ( !str_cmp( name, "do_socials" ))		return do_socials;
	if ( !str_cmp( name, "do_south" ))		return do_south;
	if ( !str_cmp( name, "do_sound" ))		return do_sound;
	if ( !str_cmp( name, "do_southeast" ))		return do_southeast;
	if ( !str_cmp( name, "do_southwest" ))		return do_southwest;
	if ( !str_cmp( name, "do_speak" ))		return do_speak;
	if ( !str_cmp( name, "do_split" ))		return do_split;
	if ( !str_cmp( name, "do_splint" ))		return do_splint;
	if ( !str_cmp( name, "do_sset" ))		return do_sset;
	if ( !str_cmp( name, "do_stand" ))		return do_stand;
	if ( !str_cmp( name, "do_starsystems" ))        return do_starsystems;
	if ( !str_cmp( name, "do_statshield" ))		return do_statshield;
        if ( !str_cmp( name, "do_status" ))             return do_status; 
	if ( !str_cmp( name, "do_steal" ))		return do_steal;
	if ( !str_cmp( name, "do_strew" ))		return do_strew;
	if ( !str_cmp( name, "do_strip" ))		return do_strip;
	if ( !str_cmp( name, "do_stun" ))		return do_stun;
	if ( !str_cmp( name, "do_switch" ))		return do_switch;
	if ( !str_cmp( name, "do_shiptalk" ))		return do_shiptalk;
	if ( !str_cmp( name, "do_showlayers" ))		return do_showlayers;
	if ( !str_cmp( name, "do_spacetalk" ))		return do_spacetalk;
	if ( !str_cmp( name, "do_special_forces" ))	return do_special_forces;
	if ( !str_cmp( name, "do_suicide" ))		return do_suicide;
	if ( !str_cmp( name, "do_systemtalk" ))		return do_systemtalk;
	break;
    case 't':
	if ( !str_cmp( name, "do_takedrug" ))		return do_takedrug;
	if ( !str_cmp( name, "do_talk" ))		return do_talk;
	if ( !str_cmp( name, "do_tamp" ))		return do_tamp;
	if ( !str_cmp( name, "do_target" ))             return do_target; 
	if ( !str_cmp( name, "do_teach" ))		return do_teach;
	if ( !str_cmp( name, "do_tell" ))		return do_tell;
	if ( !str_cmp( name, "do_throw" ))		return do_throw;
	if ( !str_cmp( name, "do_time" ))		return do_time;
	if ( !str_cmp( name, "do_timecmd" ))		return do_timecmd;
	if ( !str_cmp( name, "do_title" ))		return do_title;
	if ( !str_cmp( name, "do_torture" ))		return do_torture;
	if ( !str_cmp( name, "do_track" ))		return do_track;
	if ( !str_cmp( name, "do_tractorbeam" ))	return do_tractorbeam;
	if ( !str_cmp( name, "do_train" ))		return do_train;
	if ( !str_cmp( name, "do_trajectory" ))		return do_trajectory;
	if ( !str_cmp( name, "do_transfer" ))		return do_transfer;
	if ( !str_cmp( name, "do_transship" ))		return do_transship;
	if ( !str_cmp( name, "do_trip" ))		return do_trip; /* Added by Gavin - Sept 09 2000 */
	if ( !str_cmp( name, "do_trust" ))		return do_trust;
	if ( !str_cmp( name, "do_tune" ))		return do_tune;
	if ( !str_cmp( name, "do_typo" ))		return do_typo;
	if ( !str_cmp( name, "do_typos" ))		return do_typos;
	break;
    case 'u':
	if ( !str_cmp( name, "do_undock"     ))    return do_undock;
	if ( !str_cmp( name, "do_unfoldarea" ))		return do_unfoldarea;
	if ( !str_cmp( name, "do_unhell" ))		return do_unhell;
	if ( !str_cmp( name, "do_unlock" ))		return do_unlock;
	if ( !str_cmp( name, "do_unnuisance" )) return do_unnuisance;
        if ( !str_cmp( name, "do_unsilence" ))          return do_unsilence;
	if ( !str_cmp( name, "do_up" ))			return do_up;
	if ( !str_cmp( name, "do_upgradeship" )) return do_upgradeship; /* Oct 29 2000 - Gavin */
	if ( !str_cmp( name, "do_uptime" ))		return do_uptime;
	if ( !str_cmp( name, "do_use" ))		return do_use;
	if ( !str_cmp( name, "do_users" ))		return do_users;
	break;
    case 'v':
	if ( !str_cmp( name, "do_value" ))		return do_value;
	if ( !str_cmp( name, "do_vandalize" ))		return do_vandalize;
	if ( !str_cmp( name, "do_vassign" ))            return do_vassign;
	if ( !str_cmp( name, "do_version" ))		return do_version;
	if ( !str_cmp( name, "do_viewskills" ))		return do_viewskills;
	if ( !str_cmp( name, "do_visible" ))		return do_visible;
	if ( !str_cmp( name, "do_vnums" ))		return do_vnums;
	if ( !str_cmp( name, "do_vsearch" ))		return do_vsearch;
	break;
    case 'w':
	if ( !str_cmp( name, "do_wake" ))		return do_wake;
	if ( !str_cmp( name, "do_war" ))		return do_war;
	if ( !str_cmp( name, "do_warn" ))		return do_warn;
	if ( !str_cmp( name, "do_wartalk" ))		return do_wartalk;
	if ( !str_cmp( name, "do_washup" ))		return do_washup; /* Mar 22 '01 - Gavin */
	if ( !str_cmp( name, "do_watch" ))		return do_watch;
	if ( !str_cmp( name, "do_wear" ))		return do_wear;
	if ( !str_cmp( name, "do_weather" ))		return do_weather;
	if ( !str_cmp( name, "do_west" ))		return do_west;
	if ( !str_cmp( name, "do_where" ))		return do_where;
	if ( !str_cmp( name, "do_whisper"))		return do_whisper;	/* June 04 '00 */
	if ( !str_cmp( name, "do_who" ))		return do_who;
	if ( !str_cmp( name, "do_whois" ))		return do_whois;
	if ( !str_cmp( name, "do_wimpy" ))		return do_wimpy;
	if ( !str_cmp( name, "do_wizhelp" ))		return do_wizhelp;
	if ( !str_cmp( name, "do_wizhelp2" ))		return do_wizhelp2;
	if ( !str_cmp( name, "do_wizlist" ))		return do_wizlist;
	if ( !str_cmp( name, "do_wizlock" ))		return do_wizlock;
	break;
	case 'x':
		if ( !str_cmp( name, "do_xsedit" ))		return do_xsedit; /* Oct 24 2000 */
		if ( !str_cmp( name, "do_xsocials" ))		return do_xsocials;
	break;
    case 'y':
	if ( !str_cmp( name, "do_yell" ))		return do_yell;
	break;
    case 'z':
	if ( !str_cmp( name, "do_zones" ))		return do_zones;
    }
    return skill_notfound;
}

char *spell_name( SPELL_FUN *spell )
{
    if ( spell == spell_smaug )		    return "spell_smaug";
    if ( spell == spell_blindness )	    return "spell_blindness";
    if ( spell == spell_cause_critical )    return "spell_cause_critical";
    if ( spell == spell_cause_light )	    return "spell_cause_light";
    if ( spell == spell_cause_serious )	    return "spell_cause_serious";
    if ( spell == spell_charm_person )	    return "spell_charm_person";
    if ( spell == spell_cure_blindness )    return "spell_cure_blindness";
    if ( spell == spell_cure_poison )	    return "spell_cure_poison";
    if ( spell == spell_dispel_magic )	    return "spell_dispel_magic";
    if ( spell == spell_dream )		    return "spell_dream";
    if ( spell == spell_earthquake )	    return "spell_earthquake";
    if ( spell == spell_energy_drain )	    return "spell_energy_drain";
    if ( spell == spell_farsight )	    return "spell_farsight";
    if ( spell == spell_fireball )	    return "spell_fireball";
    if ( spell == spell_flamestrike )	    return "spell_flamestrike";
    if ( spell == spell_harm )		    return "spell_harm";
    if ( spell == spell_identify )	    return "spell_identify";
    if ( spell == spell_know_alignment )    return "spell_know_alignment";
    if ( spell == spell_lightning_bolt )    return "spell_lightning_bolt";
    if ( spell == spell_poison )	    return "spell_poison";
    if ( spell == spell_remove_invis )	    return "spell_remove_invis";
    if ( spell == spell_shocking_grasp )    return "spell_shocking_grasp";
    if ( spell == spell_sleep )		    return "spell_sleep";
    if ( spell == spell_ventriloquate )	    return "spell_ventriloquate";
    if ( spell == spell_weaken )	    return "spell_weaken";
    if ( spell == spell_scorching_surge )   return "spell_scorching_surge";

    if ( spell == spell_hand_of_chaos )	    return "spell_hand_of_chaos";
    if ( spell == spell_black_lightning )   return "spell_black_lightning";

    if ( spell == spell_null )		    return "spell_null";
    return "reserved";
}

char *skill_name( DO_FUN *skill )
{   
	if ( skill == do_aassign )		return "do_aassign";
	if ( skill == do_abort )		return "do_abort"; /* Added by Gavin - July 18 2000 */
	if ( skill == do_accelerate )	return "do_accelerate";
	if ( skill == do_accept )		return "do_accept";
	if ( skill == do_add_patrol )     return "do_add_patrol";
	if ( skill == do_addbounty )        return "do_addbounty";
	if ( skill == do_addpilot )         return "do_addpilot";
	if ( skill == do_advance )		return "do_advance";
	if ( skill == do_affected )		return "do_affected";
	if ( skill == do_afk )		return "do_afk";
	if ( skill == do_ahelp )         return "do_ahelp";
	if ( skill == do_aid )		return "do_aid";
	if ( skill == do_alertstatus )		return "do_alertstatus";
	if ( skill == do_allow )		return "do_allow";
	if ( skill == do_allships )     return "do_allships";
	if ( skill == do_ammo )     return "do_ammo";
	if ( skill == do_announce )		return "do_announce";
	if ( skill == do_ansi )		return "do_ansi";
	if ( skill == do_apply )		return "do_apply";
	if ( skill == do_appoint )     return "do_appoint";
	if ( skill == do_appraise )		return "do_appraise";
	if ( skill == do_aquest )           return "do_aquest";
	if ( skill == do_areas )		return "do_areas";
	if ( skill == do_arm )     return "do_arm";
	if ( skill == do_aset )		return "do_aset";
	if ( skill == do_astat )		return "do_astat";
	if ( skill == do_at )		return "do_at";
	if ( skill == do_atobj )		return "do_atobj";
	if ( skill == do_auction )		return "do_auction";
	if ( skill == do_authorize )	return "do_authorize";
	if ( skill == do_autopilot )     return "do_autopilot";
	if ( skill == do_autopsy )     return "do_autopsy";	/* Added by Gavin Oct 3 2000 */
	if ( skill == do_autorecharge )     return "do_autorecharge";
	if ( skill == do_autotrack )     return "do_autotrack";
	if ( skill == do_avtalk )		return "do_avtalk";
	if ( skill == do_backstab )		return "do_backstab";
	if ( skill == do_balzhur )		return "do_balzhur";
	if ( skill == do_bamfin )		return "do_bamfin";
	if ( skill == do_bamfout )		return "do_bamfout";
	if ( skill == do_ban )		return "do_ban";
	if ( skill == do_bank )     return "do_bank";
	if ( skill == do_bash )		return "do_bash";
	if ( skill == do_bashdoor )		return "do_bashdoor";
	if ( skill == do_beep )             return "do_beep";
	if ( skill == do_beg )              return "do_beg";
	if ( skill == do_berserk )		return "do_berserk";
	if ( skill == do_bestow )		return "do_bestow";
	if ( skill == do_bestowarea )	return "do_bestowarea";
	if ( skill == do_bio )		return "do_bio";
	if ( skill == do_birth )		return "do_birth";
	if ( skill == do_blackjack )		return "do_blackjack";
	if ( skill == do_board )            return "do_board";
	if ( skill == do_boards )		return "do_boards";
	if ( skill == do_bodybag )		return "do_bodybag";
	if ( skill == do_bomb )     return "do_bomb";
	if ( skill == do_bounties )         return "do_bounties";
	if ( skill == do_breakup )                return "do_breakup";
	if ( skill == do_bribe )     return "do_bribe";
	if ( skill == do_bset )		return "do_bset";
	if ( skill == do_bstat )		return "do_bstat";
	if ( skill == do_bug )		return "do_bug";
	if ( skill == do_bugs )		return "do_bugs";
	if ( skill == do_bury )		return "do_bury";
	if ( skill == do_butcher )			return "do_butcher";
	if ( skill == do_buy )		return "do_buy";
	if ( skill == do_buyhome )		return "do_buyhome";
	if ( skill == do_buyship )		return "do_buyship";
	if ( skill == do_buyvendor )     return "do_buyvendor"; /* Added by Gavin - July 18 2000 */
	if ( skill == do_calculate )         return "do_calculate";
	if ( skill == do_call ) 		return "do_call"; /* Added by Gavin - Mar 11 2001 */
	if ( skill == do_capture )     return "do_capture";
	if ( skill == do_carry )     return "do_carry"; /* Added by Gavin - August 24 2000 */
	if ( skill == do_cast )		return "do_cast";
	if ( skill == do_cedit )		return "do_cedit";
	if ( skill == do_chaff )     return "do_chaff";
	if ( skill == do_channels )		return "do_channels";
	if ( skill == do_chanuser )		return "do_chanuser";
	if ( skill == do_chat )		return "do_chat";
	if ( skill == do_check_vnums )	return "do_check_vnums";
	if ( skill == do_clan_donate )	return "do_clan_donate";
	if ( skill == do_clan_withdraw )	return "do_clan_withdraw";
	if ( skill == do_clanbuyship )		return "do_clanbuyship";
	if ( skill == do_clans )		return "do_clans";
	if ( skill == do_clansellship )		return "do_clansellship";
	if ( skill == do_clantalk )		return "do_clantalk";
	if ( skill == do_climate )		return "do_climate";
	if ( skill == do_climb )		return "do_climb";
	if ( skill == do_clone )     return "do_clone";
	if ( skill == do_clones )			return "do_clones";
	if ( skill == do_close )		return "do_close";
	if ( skill == do_closebay )     return "do_openbay";
	if ( skill == do_closehatch )         return "do_closehatch";
	if ( skill == do_cmdtable )		return "do_cmdtable";
	if ( skill == do_collectgold )     return "do_collectgold"; /* Added by Gavin - July 18 2000 */
	if ( skill == do_commands )		return "do_commands";
	if ( skill == do_comment )		return "do_comment";
	if ( skill == do_compare )		return "do_compare";
	if ( skill == do_config )		return "do_config";
	if ( skill == do_consent )		return "do_consent";
	if ( skill == do_consider )		return "do_consider";
	if ( skill == do_cosmetic )		return "do_cosmetic"; /* Added by Gavin Oct 1 2000 */
	if ( skill == do_copyship )     return "do_copyship";
	if ( skill == do_council_induct )	return "do_council_induct";
	if ( skill == do_council_outcast )		return "do_council_outcast";
	if ( skill == do_councilbuyhome)	return "do_councilbuyhome";
	if ( skill == do_councils )		return "do_councils";
	if ( skill == do_councilsellhome)	return "do_councilsellhome";
	if ( skill == do_credits )		return "do_credits";
	if ( skill == do_cset )		return "do_cset";
	if ( skill == do_cyber )		return "do_cyber";
	if ( skill == do_delay )		return "do_delay";
	if ( skill == do_demote )     return "do_demote";
	if ( skill == do_deny )		return "do_deny";
	if ( skill == do_description )	return "do_description";
	if ( skill == do_destro )		return "do_destro";
	if ( skill == do_destroy )		return "do_destroy";
	if ( skill == do_destroyrestore )	return "do_destroyrestore"; /* May 27 2000 - Gavin */
	if ( skill == do_destroyslay )     return "do_destroyslay";
	if ( skill == do_diagnose )		return "do_diagnose";
	if ( skill == do_dig )		return "do_dig";
	if ( skill == do_disarm )		return "do_disarm";
	if ( skill == do_disconnect )	return "do_disconnect";
	if ( skill == do_disguise )     return "do_disguise";
	if ( skill == do_dismount )		return "do_dismount";
	if ( skill == do_divorce )		return "do_divorce";
	if ( skill == do_dmesg )		return "do_dmesg";
	if ( skill == do_dnd )		return "do_dnd"; /* Added May 31 2000 */
	if ( skill == do_dock )		return "do_dock";
	if ( skill == do_down )		return "do_down";
	if ( skill == do_drag )		return "do_drag";
	if ( skill == do_drink )		return "do_drink";
	if ( skill == do_drive )     return "do_drive";
	if ( skill == do_drop )		return "do_drop";
	if ( skill == do_east )		return "do_east";
	if ( skill == do_endsimulator )		return "do_endsimulator";
	if ( skill == do_eat )		return "do_eat";
	if ( skill == do_ech )		return "do_ech";
	if ( skill == do_echo )		return "do_echo";
	if ( skill == do_elite_guard )     return "do_elite_guard";
	if ( skill == do_emote )		return "do_emote";
	if ( skill == do_empower )     return "do_empower";
	if ( skill == do_empty )		return "do_empty";
	if ( skill == do_enlist )     return "do_enlist";
	if ( skill == do_enter )		return "do_enter";
	if ( skill == do_equipment )	return "do_equipment";
	if ( skill == do_examine )		return "do_examine";
	if ( skill == do_exits )		return "do_exits";
	if ( skill == do_fill )		return "do_fill";
	if ( skill == do_fire )         return "do_fire";
	if ( skill == do_first_aid )     return "do_first_aid";
	if ( skill == do_fixchar )		return "do_fixchar";
	if ( skill == do_flee )		return "do_flee";
	if ( skill == do_foldarea )		return "do_foldarea";
	if ( skill == do_follow )		return "do_follow";
	if ( skill == do_for )		return "do_for";
	if ( skill == do_force )		return "do_force";
	if ( skill == do_forceclose )	return "do_forceclose";
	if ( skill == do_form_password )	return "do_form_password";
	if ( skill == do_fprompt )			return "do_fprompt";
	if ( skill == do_fquit )		return "do_fquit";
	if ( skill == do_free_vnums )         return "do_free_vnums";
	if ( skill == do_freehomes )		return "do_freehomes"; /* Added by Gavin - Sept 09 2000 */
	if ( skill == do_freeships )		return "do_freeships";
	if ( skill == do_freeze )		return "do_freeze";
	if ( skill == do_fullname )		return "do_fullname"; /* Added by Gavin - Sept 09 2000 */
	if ( skill == do_gather_intelligence )     return "do_gather_intelligence";
	if ( skill == do_gemcutting )     return "do_gemcutting";
	if ( skill == do_gemote )     return "do_gemote"; /* Added By Gavin - June 08 2000 */
	if ( skill == do_get )		return "do_get";
	if ( skill == do_getcode )			return "do_getcode";
	if ( skill == do_gfighting )		return "do_gfighting";
	if ( skill == do_give )		return "do_give";
	if ( skill == do_glance )		return "do_glance";
	if ( skill == do_gold )             return "do_gold";
	if ( skill == do_goto )		return "do_goto";
	if ( skill == do_gouge )		return "do_gouge";
	if ( skill == do_group )		return "do_group";
	if ( skill == do_gtell )		return "do_gtell";
	if ( skill == do_gwhere )			return "do_gwhere";
	if ( skill == do_hail )     return "do_hail";
	if ( skill == do_hedit )		return "do_hedit";
	if ( skill == do_hell )		return "do_hell";
	if ( skill == do_help )		return "do_help";
	if ( skill == do_hide )		return "do_hide";
	if ( skill == do_hijack )     return "do_hijack";
	if ( skill == do_hitall )		return "do_hitall";
	if ( skill == do_copyover )		return "do_copyover";
	if ( skill == do_hlist )		return "do_hlist";
	if ( skill == do_holylight )	return "do_holylight";
	if ( skill == do_homeid )			return "do_homeid";
	if ( skill == do_homes )		return "do_homes";
	if ( skill == do_hset )		return "do_hset";
	if ( skill == do_hyperspace )         return "do_hyperspace";
	if ( skill == do_ide )		return "do_ide";
	if ( skill == do_idea )		return "do_idea";
	if ( skill == do_ideas )		return "do_ideas";
	if ( skill == do_ignore )		return "do_ignore";
	if ( skill == do_immortalize )	return "do_immortalize";
	if ( skill == do_immtalk )		return "do_immtalk";
	if ( skill == do_induct )		return "do_induct";
	if ( skill == do_info )            return "do_info";
	if ( skill == do_inject )		return "do_inject"; /* March 24 '01 - Gavin */
	if ( skill == do_installarea )	return "do_installarea";
	if ( skill == do_instaroom )	return "do_instaroom";
	if ( skill == do_instazone )	return "do_instazone";
	if ( skill == do_inventory )	return "do_inventory";
	if ( skill == do_invis )		return "do_invis";
	if ( skill == do_invite )		return "do_invite";
	if ( skill == do_ipcompare )		return "do_ipcompare"; /* June 04 2000 - Gavin */
	if ( skill == do_jail )     return "do_jail";
	if ( skill == do_khistory )		return "do_khistory";
	if ( skill == do_kick )		return "do_kick";
	if ( skill == do_kill )		return "do_kill";
	if ( skill == do_knock )		return "do_knock";
	if ( skill == do_land )             return "do_land";
	if ( skill == do_languages )	return "do_languages";
	if ( skill == do_last )		return "do_last";
	if ( skill == do_launch )		return "do_launch";
	if ( skill == do_leave )		return "do_leave";
	if ( skill == do_leaveship )	return "do_leaveship";
	if ( skill == do_level )		return "do_level";
	if ( skill == do_light )		return "do_light";
	if ( skill == do_list )		return "do_list";
	if ( skill == do_listen )			return "do_listen";
	if ( skill == do_listships )		return "do_listships";
	if ( skill == do_litterbug )	return "do_litterbug";
	if ( skill == do_loadarea )		return "do_loadarea";
	if ( skill == do_loadup )		return "do_loadup";
	if ( skill == do_lock )		return "do_lock";
	if ( skill == do_log )		return "do_log";
	if ( skill == do_look )		return "do_look";
	if ( skill == do_low_purge )	return "do_low_purge";
	if ( skill == do_mailroom )		return "do_mailroom";
	if ( skill == do_make )		return "do_make";
	if ( skill == do_make_infect_med ) return "do_make_infect_med"; /* Mar 23 '01 - Gavin */
	if ( skill == do_makearmor )     return "do_makearmor";
	if ( skill == do_makeblade )	return "do_makeblade";
	if ( skill == do_makeblaster )	return "do_makeblaster";
	if ( skill == do_makeboard )	return "do_makeboard";
	if ( skill == do_makeclan )		return "do_makeclan";
	if ( skill == do_makecomlink )	return "do_makecomlink";
	if ( skill == do_makecontainer )	return "do_makecontainer";
	if ( skill == do_makecouncil )		return "do_makecouncil";
	if ( skill == do_makedroid )     return "do_makedroid";
	if ( skill == do_makefurniture )		return "do_makefurniture";
	if ( skill == do_makegrenade )	return "do_makegrenade";
	if ( skill == do_makehome )		return "do_makehome";
	if ( skill == do_makejewelry )     return "do_makejewelry";
	if ( skill == do_makekeyring )  return "do_makekeyring";
	if ( skill == do_makelandmine )	return "do_makelandmine";
	if ( skill == do_makelight )	return "do_makelight";
	if ( skill == do_makemedkit )	return "do_makemedkit"; /* Added By Gavin - August 08 2000 */
	if ( skill == do_makemissile )     return "do_makemissile";
	if ( skill == do_makeplanet )     return "do_makeplanet";
	if ( skill == do_makeprototype )     return "do_makeprototype"; /* Mar 28 '01 - Gavin */
	if ( skill == do_makerepair )	return "do_makerepair";
	if ( skill == do_makerestore )		return "do_makerestore"; /* May 27 2000 - Gavin */
	if ( skill == do_makeshield )	return "do_makeshield";
	if ( skill == do_makeship )         return "do_makeship";
	if ( skill == do_makeshop )		return "do_makeshop";
	if ( skill == do_makeslay )		return "do_makeslay";
	if ( skill == do_makespice )	return "do_makespice";
	if ( skill == do_makestarsystem )   return "do_makestarsystem";
	if ( skill == do_makewizlist )	return "do_makewizlist";
	if ( skill == do_marry )		return "do_marry";
	if ( skill == do_mass_propeganda )     return "do_mass_propeganda";
	if ( skill == do_massign )		return "do_massign";
	if ( skill == do_mcreate )		return "do_mcreate";
	if ( skill == do_mdelete )		return "do_mdelete";
	if ( skill == do_memory )		return "do_memory";
	if ( skill == do_mfind )		return "do_mfind";
	if ( skill == do_mine )     return "do_mine";
	if ( skill == do_minvoke )		return "do_minvoke";
	if ( skill == do_mlist )		return "do_mlist";
	if ( skill == do_mount )		return "do_mount";
	if ( skill == do_mp_close_passage )	return "do_mp_close_passage";
	if ( skill == do_mp_damage )	return "do_mp_damage";
	if ( skill == do_mp_deposit )	return "do_mp_deposit";
	if ( skill == do_mp_open_passage )	return "do_mp_open_passage";
	if ( skill == do_mp_practice )	return "do_mp_practice";
	if ( skill == do_mp_restore )	return "do_mp_restore";
	if ( skill == do_mp_slay )		return "do_mp_slay";
	if ( skill == do_mp_withdraw )	return "do_mp_withdraw";
	if ( skill == do_mpadvance )	return "do_mpadvance";
	if ( skill == do_mpapply )		return "do_mpapply";
	if ( skill == do_mpapplyb )		return "do_mpapplyb";
	if ( skill == do_mpasound )		return "do_mpasound";
	if ( skill == do_mpat )		return "do_mpat";
	if ( skill == do_mpbodybag )	return "do_mpbodybag";
	if ( skill == do_mpcopy )		return "do_mpcopy";
	if ( skill == do_mpdelay )		return "do_mpdelay";
	if ( skill == do_mpdream )		return "do_mpdream";
	if ( skill == do_mpecho )		return "do_mpecho";
	if ( skill == do_mprat )		return "do_mprat";
	if ( skill == do_mpechoaround )	return "do_mpechoaround";
	if ( skill == do_mpechoat )		return "do_mpechoat";
	if ( skill == do_mpedit )		return "do_mpedit";
	if ( skill == do_mpfind )		return "do_mpfind";
	if ( skill == do_mpforce )		return "do_mpforce";
	if ( skill == do_mpgain )		return "do_mpgain";
	if ( skill == do_mpgoto )		return "do_mpgoto";
	if ( skill == do_mpinvis )		return "do_mpinvis";
	if ( skill == do_mpjunk )		return "do_mpjunk";
	if ( skill == do_mpkill )		return "do_mpkill";
	if ( skill == do_mpmload )		return "do_mpmload";
	if ( skill == do_mpnothing )	return "do_mpnothing";
	if ( skill == do_mpnuisance )       return "do_mpnuisance"; /* Added By Gavin - July 21 2000 */
	if ( skill == do_mpoload )		return "do_mpoload";
	if ( skill == do_mppkset )		return "do_mppkset";
	if ( skill == do_mppurge )		return "do_mppurge";
	if ( skill == do_mpscatter )        return "do_mpscatter";
	if ( skill == do_mpstat )		return "do_mpstat";
	if ( skill == do_mptransfer )	return "do_mptransfer";
	if ( skill == do_mpunnuisance )	return "do_mpunnuisance"; /* Added By Gavin - July 21 2000 */
	if ( skill == do_mset )		return "do_mset";
	if ( skill == do_mstat )		return "do_mstat";
	if ( skill == do_murde )		return "do_murde";
	if ( skill == do_murder )		return "do_murder";
	if ( skill == do_music )		return "do_music";
	if ( skill == do_mwhere )		return "do_mwhere";
	if ( skill == do_mxp)			return "do_mxp";
	if ( skill == do_myclan )         return "do_myclan";
	if ( skill == do_name )		return "do_name";
	if ( skill == do_newbiechat )	return "do_newbiechat";
	if ( skill == do_newbieset )	return "do_newbieset";
	if ( skill == do_news )		return "do_news";
	if ( skill == do_newzones )		return "do_newzones";
	if ( skill == do_noemote )		return "do_noemote";
	if ( skill == do_noresolve )	return "do_noresolve";
	if ( skill == do_north )		return "do_north";
	if ( skill == do_northeast )	return "do_northeast";
	if ( skill == do_northwest )	return "do_northwest";
	if ( skill == do_notell )		return "do_notell";
	if ( skill == do_noteroom )		return "do_noteroom";
	if ( skill == do_notitle )          return "do_notitle";
	if ( skill == do_nuisance )		return "do_nuisance";
	if ( skill == do_oassign )		return "do_oassign";
	if ( skill == do_ocreate )		return "do_ocreate";
	if ( skill == do_odelete )		return "do_odelete";
	if ( skill == do_ofind )		return "do_ofind";
	if ( skill == do_oinvoke )		return "do_oinvoke";
	if ( skill == do_oldscore )		return "do_oldscore";
	if ( skill == do_olist )		return "do_olist";
	if ( skill == do_ooc )		return "do_ooc";
	if ( skill == do_opcopy )		return "do_opcopy";
	if ( skill == do_opedit )		return "do_opedit";
	if ( skill == do_open )		return "do_open";
	if ( skill == do_openbay )     return "do_openbay";
	if ( skill == do_openhatch )         return "do_openhatch";
	if ( skill == do_opfind )		return "do_opfind";
	if ( skill == do_opstat )		return "do_opstat";
	if ( skill == do_order )		return "do_order";
	if ( skill == do_order_planet )		return "do_order_planet";
	if ( skill == do_oset )		return "do_oset";
	if ( skill == do_ostat )		return "do_ostat";
	if ( skill == do_outcast )		return "do_outcast";
	if ( skill == do_overthrow )         return "do_overthrow";
	if ( skill == do_owhere )		return "do_owhere";
	if ( skill == do_pager )		return "do_pager";
	if ( skill == do_paint )		return "do_paint";
	if ( skill == do_password )		return "do_password";
	if ( skill == do_payfine )     return "do_payfine";
	if ( skill == do_pcrename )         return "do_pcrename";
	if ( skill == do_peace )		return "do_peace";
	if ( skill == do_petname )		return "do_petname"; /* Added By Gavin - July 14 2000 */
	if ( skill == do_pick )		return "do_pick";
	if ( skill == do_pickshiplock )     return "do_pickshiplock";
	if ( skill == do_placevendor )     return "do_placevendor"; /* Added by Gavin - July 18 2000 */
	if ( skill == do_planets )     return "do_planets";
	if ( skill == do_pluogus )     return "do_pluogus";
	if ( skill == do_poison_weapon )	return "do_poison_weapon";
	if ( skill == do_postguard )     return "do_postguard";
	if ( skill == do_practice )		return "do_practice";
	if ( skill == do_pricevendor )     return "do_pricevendor"; /* Added by Gavin - July 18 2000 */
	if ( skill == do_promote )		return "do_promote"; 
	if ( skill == do_prompt )		return "do_prompt";
	if ( skill == do_propeganda )     return "do_propeganda";
	if ( skill == do_propose )		return "do_propose";
	if ( skill == do_prototypes )     return "do_prototypes"; /* Mar 28 '01 - Gavin */
	if ( skill == do_pshield )		return "do_pshield"; /* Added by Gavin - July 22 2000 */
	if ( skill == do_pull )		return "do_pull";
	if ( skill == do_punch )		return "do_punch";
	if ( skill == do_purge )		return "do_purge";
	if ( skill == do_push )		return "do_push";
	if ( skill == do_put )		return "do_put";
	if ( skill == do_qpset )     return "do_qpset";
	if ( skill == do_qpstat )	 return "do_qpstat"; /* June 04 2000 */
	if ( skill == do_quaff )		return "do_quaff";
	if ( skill == do_quest )		return "do_quest";
	if ( skill == do_qui )		return "do_qui";
	if ( skill == do_quit )		return "do_quit";
	if ( skill == do_radar )     return "do_radar";
	if ( skill == do_rank )		return "do_rank";
	if ( skill == do_rap )		return "do_rap";
	if ( skill == do_rassign )		return "do_rassign";
	if ( skill == do_rat )		return "do_rat";
	if ( skill == do_rcreate )	return "do_rcreate"; /* Created by Gavin - May 11 2001 */
	if ( skill == do_rdelete )		return "do_rdelete";
	if ( skill == do_reboo )		return "do_reboo";
	if ( skill == do_reboot )		return "do_reboot";
	if ( skill == do_recall )    return "do_recall";
	if ( skill == do_recharge )         return "do_recharge";
	if ( skill == do_recho )		return "do_recho";
	if ( skill == do_redit )		return "do_redit";
	if ( skill == do_refuel )         return "do_refuel";
	if ( skill == do_regoto )		return "do_regoto";
	if ( skill == do_reinforcements )     return "do_reinforcements";
	if ( skill == do_rembounty )     return "do_rembounty";
	if ( skill == do_remove )		return "do_remove";
	if ( skill == do_rempilot )         return "do_rempilot";
	if ( skill == do_renameship )		return "do_renameship";
	if ( skill == do_rent )		return "do_rent";
	if ( skill == do_repair )		return "do_repair";
	if ( skill == do_repairset )	return "do_repairset";
	if ( skill == do_repairship )         return "do_repairship";
	if ( skill == do_repairshops )	return "do_repairshops";
	if ( skill == do_repairstat )	return "do_repairstat";
	if ( skill == do_reply )		return "do_reply";
	if ( skill == do_report )		return "do_report";
	if ( skill == do_repulsor )			return "do_repulsor";
	if ( skill == do_rescue )		return "do_rescue";
	if ( skill == do_reserve )		return "do_reserve";
	if ( skill == do_reset )		return "do_reset";
	if ( skill == do_resetship )	return "do_resetship";
	if ( skill == do_resign )     return "do_resign";
	if ( skill == do_rest )		return "do_rest";
	if ( skill == do_restore )		return "do_restore";
	if ( skill == do_restoretime )	return "do_restoretime";
	if ( skill == do_restrict )		return "do_restrict";
	if ( skill == do_retell )		return "do_retell";
	if ( skill == do_retire )		return "do_retire";
	if ( skill == do_retran )		return "do_retran";
	if ( skill == do_return )		return "do_return";
	if ( skill == do_rlist )		return "do_rlist";
	if ( skill == do_roommate )		return "do_roommate"; /* Added by Gavin - Sept 7 '00 */
	if ( skill == do_rpcopy )		return "do_rpcopy";
	if ( skill == do_rpedit )		return "do_rpedit";
	if ( skill == do_rpfind )		return "do_rpfind";
	if ( skill == do_rpstat )		return "do_rpstat";
	if ( skill == do_rreset )		return "do_rreset";
	if ( skill == do_rstat )		return "do_rstat";
	if ( skill == do_run )                return "do_run"; /* Added 01/21/01 by Drraagh */
	if ( skill == do_sacrifice )	return "do_sacrifice";
	if ( skill == do_save )		return "do_save";
	if ( skill == do_saveall )		return "do_saveall";
	if ( skill == do_savearea )		return "do_savearea";
	if ( skill == do_say )		return "do_say";
	if ( skill == do_scan )		return "do_scan";
	if ( skill == do_scatter )			return "do_scatter";
	if ( skill == do_score )		return "do_score";
	if ( skill == do_scribe )		return "do_scribe";
	if ( skill == do_search )		return "do_search";
	if ( skill == do_sedit )		return "do_sedit";
	if ( skill == do_seduce )     return "do_seduce";
	if ( skill == do_sell )		return "do_sell";
	if ( skill == do_sellhome )		return "do_sellhome";
	if ( skill == do_sellship )         return "do_sellship";
	if ( skill == do_sendcode )		return "do_sendcode";
	if ( skill == do_set_boot_time )	return "do_set_boot_time";
	if ( skill == do_setblaster )     return "do_setblaster";
	if ( skill == do_setclan )		return "do_setclan";
	if ( skill == do_setcode )			return "do_setcode";
	if ( skill == do_setcouncil )		return "do_setcouncil";
	if ( skill == do_sethome )		return "do_sethome";
	if ( skill == do_setplanet )     return "do_setplanet";
	if ( skill == do_setprototype )     return "do_setprototype"; /* Mar 28 '01 - Gavin */
	if ( skill == do_setrace )		return "do_setrace";
	if ( skill == do_setrestore )		return "do_setrestore"; /* May 27 2000 - Gavin */
	if ( skill == do_setship )          return "do_setship";
	if ( skill == do_setslay )		return "do_setslay";
	if ( skill == do_setstarsystem )    return "do_setstarsystem";
	if ( skill == do_setsystemcode )		return "do_setsystemcode";
	if ( skill == do_setweather )	return "do_setweather";
	if ( skill == do_sexes )		return "do_sexes";
	if ( skill == do_ship_masking ) return "do_ship_masking"; /* Mar 18 2001 - By Gavin */
	if ( skill == do_ships )            return "do_ships";
	if ( skill == do_shiptalk )     return "do_shiptalk";
	if ( skill == do_shops )		return "do_shops";
	if ( skill == do_shopset )		return "do_shopset";
	if ( skill == do_shopstat )		return "do_shopstat";
	if ( skill == do_shout )		return "do_shout";
	if ( skill == do_shove )		return "do_shove";
	if ( skill == do_showclan )		return "do_showclan";
	if ( skill == do_showcouncil )		return "do_showcouncil";
	if ( skill == do_showhome )		return "do_showhome";
	if ( skill == do_showlayers )		return "do_showlayers";
	if ( skill == do_showplanet )		return "do_showplanet";
	if ( skill == do_showprototype )     return "do_showprototype"; /* Mar 28 '01 - Gavin */
	if ( skill == do_showrace )		return "do_showrace";
	if ( skill == do_showrestore )		return "do_showrestore"; /* May 27 2000 - Gavin */
	if ( skill == do_showship )         return "do_showship";
	if ( skill == do_showslay )		return "do_showslay";
	if ( skill == do_showstarsystem )   return "do_showstarsystem";
	if ( skill == do_showweather )		return "do_showweather";
	if ( skill == do_shutdow )		return "do_shutdow";
	if ( skill == do_shutdown )		return "do_shutdown";
	if ( skill == do_silence )		return "do_silence";
	if ( skill == do_sit )		return "do_sit";
	if ( skill == do_sla )		return "do_sla";
	if ( skill == do_slay )		return "do_slay";
	if ( skill == do_sleep )		return "do_sleep";
	if ( skill == do_slice )		return "do_slice";
	if ( skill == do_slist )		return "do_slist";
	if ( skill == do_slookup )		return "do_slookup";
	if ( skill == do_smalltalk )     return "do_smalltalk";
	if ( skill == do_smoke )		return "do_smoke";
	if ( skill == do_snipe )     return "do_snipe";
	if ( skill == do_snoop )		return "do_snoop";
	if ( skill == do_sober )		return "do_sober";
	if ( skill == do_socials )		return "do_socials";
	if ( skill == do_sound )		return "do_sound";
	if ( skill == do_south )		return "do_south";
	if ( skill == do_southeast )	return "do_southeast";
	if ( skill == do_southwest )	return "do_southwest";
	if ( skill == do_spacetalk )     return "do_spacetalk";
	if ( skill == do_speak )		return "do_speak";
	if ( skill == do_special_forces )     return "do_special_forces";
	if ( skill == do_splint )		return "do_splint";  /* Added by Gavin - Sept 7 '00 */
	if ( skill == do_split )		return "do_split";
	if ( skill == do_sset )		return "do_sset";
	if ( skill == do_stand )		return "do_stand";
	if ( skill == do_starsystems )      return "do_starsystems";
	if ( skill == do_statshield )	return "do_statshield";
	if ( skill == do_status )		return "do_status";
	if ( skill == do_steal )		return "do_steal";
	if ( skill == do_strew )		return "do_strew";
	if ( skill == do_strip )			return "do_strip";
	if ( skill == do_stun )		return "do_stun";
	if ( skill == do_suicide )     return "do_suicide";
	if ( skill == do_switch )		return "do_switch";
	if ( skill == do_systemtalk )     return "do_systemtalk";
	if ( skill == do_takedrug )     return "do_takedrug";
	if ( skill == do_talk )		return "do_talk";
	if ( skill == do_tamp )		return "do_tamp";
	if ( skill == do_target )         return "do_target";
	if ( skill == do_teach )     return "do_teach";
	if ( skill == do_tell )		return "do_tell";
	if ( skill == do_throw )     return "do_throw";
	if ( skill == do_time )		return "do_time";
	if ( skill == do_timecmd )		return "do_timecmd";
	if ( skill == do_title )		return "do_title";
	if ( skill == do_torture )     return "do_torture";
	if ( skill == do_track )		return "do_track";
	if ( skill == do_tractorbeam )     return "do_tractorbeam";
	if ( skill == do_train )     return "do_train";
	if ( skill == do_trajectory )	return "do_trajectory";
	if ( skill == do_transfer )		return "do_transfer";
	if ( skill == do_transship )	return "do_transship";
	if ( skill == do_trip )			return "do_trip";	/* Added by Gavin - Sept 09 2000 */
	if ( skill == do_trust )		return "do_trust";
	if ( skill == do_tune )		return "do_tune";
	if ( skill == do_typo )		return "do_typo";
	if ( skill == do_typos )		return "do_typos";
	if ( skill == do_undock )		return "do_undock";
	if ( skill == do_unfoldarea )	return "do_unfoldarea";
	if ( skill == do_unhell )		return "do_unhell";
	if ( skill == do_unlock )		return "do_unlock";
	if ( skill == do_unnuisance )		return "do_unnuisance";
	if ( skill == do_unsilence )        return "do_unsilence";
	if ( skill == do_up )		return "do_up";
	if ( skill == do_upgradeship )	return "do_upgradeship"; /* Oct 29 2000 - Gavin */
	if ( skill == do_uptime )		return "do_uptime";
	if ( skill == do_use )     return "do_use";
	if ( skill == do_users )		return "do_users";
	if ( skill == do_value )		return "do_value";
	if ( skill == do_vandalize )		return "do_vandalize";
	if ( skill == do_vassign )           return "do_vassign";
	if ( skill == do_version )		return "do_version";
	if ( skill == do_viewskills )		return "do_viewskills";
	if ( skill == do_visible )		return "do_visible";
	if ( skill == do_vnums )		return "do_vnums";
	if ( skill == do_vsearch )		return "do_vsearch";
	if ( skill == do_wake )		return "do_wake";
	if ( skill == do_war )		return "do_war";
	if ( skill == do_warn )		return "do_warn";
	if ( skill == do_wartalk )		return "do_wartalk";
	if ( skill == do_washup )		return "do_washup"; /* Mar 22 '01 - Gavin */
	if ( skill == do_watch )		return "do_watch";
	if ( skill == do_wear )		return "do_wear";
	if ( skill == do_weather )		return "do_weather";
	if ( skill == do_west )		return "do_west";
	if ( skill == do_where )		return "do_where";
	if ( skill == do_whisper )		return "do_whisper"; /* June 04 '00 */
	if ( skill == do_who )		return "do_who";
	if ( skill == do_whois )		return "do_whois";
	if ( skill == do_wimpy )		return "do_wimpy";
	if ( skill == do_wizhelp )		return "do_wizhelp";
	if ( skill == do_wizhelp2 )		return "do_wizhelp2";
	if ( skill == do_wizlist )		return "do_wizlist";
	if ( skill == do_wizlock )		return "do_wizlock";
	if ( skill == do_xsedit )		return "do_xsedit"; /* Oct 24 2000 */
	if ( skill == do_xsocials )		return "do_xsocials";
	if ( skill == do_yell )		return "do_yell";
	if ( skill == do_zones )		return "do_zones";

    return "reserved";
}

/*
 * Function used by qsort to sort skills
 */
int skill_comp( SKILLTYPE **sk1, SKILLTYPE **sk2 )
{
    SKILLTYPE *skill1 = (*sk1);
    SKILLTYPE *skill2 = (*sk2);

    if ( !skill1 && skill2 )
	return 1;
    if ( skill1 && !skill2 )
	return -1;
    if ( !skill1 && !skill2 )
	return 0;
    if ( skill1->type < skill2->type )
	return -1;
    if ( skill1->type > skill2->type )
	return 1;
    return strcmp( skill1->name, skill2->name );
}

/*
 * Sort the skill table with qsort
 */
void sort_skill_table()
{
    log_string( "Sorting skill table..." );
    qsort( &skill_table[1], top_sn-1, sizeof( SKILLTYPE * ),
		(int(*)(const void *, const void *)) skill_comp );
}

/*
 * Remap slot numbers to sn values
 */
void remap_slot_numbers()
{
    SKILLTYPE *skill;
    SMAUG_AFF *aff;
    char tmp[32];
    int sn;

    log_string( "Remapping slots to sns" );

    for ( sn = 0; sn <= top_sn; sn++ )
    {
	if ( (skill=skill_table[sn]) != NULL )
	{
	    for ( aff = skill->affects; aff; aff = aff->next )
		if ( aff->location == APPLY_WEAPONSPELL
		||   aff->location == APPLY_WEARSPELL
		||   aff->location == APPLY_REMOVESPELL
		||   aff->location == APPLY_STRIPSN
/*		||   aff->location == APPLY_RECURRINGSPELL */)
		{
		    sprintf( tmp, "%d", slot_lookup(atoi(aff->modifier)) );
		    DISPOSE(aff->modifier);
		    aff->modifier = str_dup(tmp);
		}
	}
    }
}


/*
 * Write skill data to a file
 */
void fwrite_skill( FILE *fpout, SKILLTYPE *skill )
{
	SMAUG_AFF *aff;

	fprintf( fpout, "Name         %s~\n",	skill->name	);
	fprintf( fpout, "Type         %s\n",	skill_tname[skill->type]);
	fprintf( fpout, "Flags        %d\n",	skill->flags	);
	if ( skill->made_char && skill->made_char[0] != '\0' )
          fprintf( fpout, "MadeBy       %s~\n",  skill->made_char );
	if ( skill->prototype )
          fprintf( fpout, "Prototype    %d\n",  skill->prototype );
	if ( skill->target )
	  fprintf( fpout, "Target       %d\n",	skill->target	);
	if ( skill->minimum_position )
	  fprintf( fpout, "Minpos       %d\n",	skill->minimum_position );
	if ( skill->saves )
	  fprintf( fpout, "Saves        %d\n",	skill->saves	);
	if ( skill->slot )
	  fprintf( fpout, "Slot         %d\n",	skill->slot	);
	if ( skill->min_mana )
	  fprintf( fpout, "Mana         %d\n",	skill->min_mana );
	if ( skill->beats )
	  fprintf( fpout, "Rounds       %d\n",	skill->beats	);
	if ( skill->guild != -1 )
	  fprintf( fpout, "Guild        %d\n",	skill->guild	);
	if ( skill->skill_fun )
	  fprintf( fpout, "Code         %s\n",	skill_name(skill->skill_fun) );
	else
	if ( skill->spell_fun )
	  fprintf( fpout, "Code         %s\n",	spell_name(skill->spell_fun) );
	fprintf( fpout, "Dammsg       %s~\n",	skill->noun_damage );
	if ( skill->msg_off && skill->msg_off[0] != '\0' )
	  fprintf( fpout, "Wearoff      %s~\n",	skill->msg_off	);

	if ( skill->hit_char && skill->hit_char[0] != '\0' )
	  fprintf( fpout, "Hitchar      %s~\n",	skill->hit_char );
	if ( skill->hit_vict && skill->hit_vict[0] != '\0' )
	  fprintf( fpout, "Hitvict      %s~\n",	skill->hit_vict );
	if ( skill->hit_room && skill->hit_room[0] != '\0' )
	  fprintf( fpout, "Hitroom      %s~\n",	skill->hit_room );

	if ( skill->miss_char && skill->miss_char[0] != '\0' )
	  fprintf( fpout, "Misschar     %s~\n",	skill->miss_char );
	if ( skill->miss_vict && skill->miss_vict[0] != '\0' )
	  fprintf( fpout, "Missvict     %s~\n",	skill->miss_vict );
	if ( skill->miss_room && skill->miss_room[0] != '\0' )
	  fprintf( fpout, "Missroom     %s~\n",	skill->miss_room );

	if ( skill->die_char && skill->die_char[0] != '\0' )
	  fprintf( fpout, "Diechar      %s~\n",	skill->die_char );
	if ( skill->die_vict && skill->die_vict[0] != '\0' )
	  fprintf( fpout, "Dievict      %s~\n",	skill->die_vict );
	if ( skill->die_room && skill->die_room[0] != '\0' )
	  fprintf( fpout, "Dieroom      %s~\n",	skill->die_room );

	if ( skill->imm_char && skill->imm_char[0] != '\0' )
	  fprintf( fpout, "Immchar      %s~\n",	skill->imm_char );
	if ( skill->imm_vict && skill->imm_vict[0] != '\0' )
	  fprintf( fpout, "Immvict      %s~\n",	skill->imm_vict );
	if ( skill->imm_room && skill->imm_room[0] != '\0' )
	  fprintf( fpout, "Immroom      %s~\n",	skill->imm_room );

	if ( skill->dice && skill->dice[0] != '\0' )
	  fprintf( fpout, "Dice         %s~\n",	skill->dice );
	if ( skill->value )
	  fprintf( fpout, "Value        %d\n",	skill->value );
	if ( skill->participants )
	  fprintf( fpout, "Participants %d\n",	skill->participants );
	if ( skill->teachers && skill->teachers[0] != '\0' )
	  fprintf( fpout, "Teachers     %s~\n",	skill->teachers );
	for ( aff = skill->affects; aff; aff = aff->next )
	  fprintf( fpout, "Affect       '%s' %d '%s' %d\n",
	  	aff->duration, aff->location, aff->modifier, aff->bitvector );
	if ( skill->alignment )
	  fprintf( fpout, "Alignment   %d\n",	skill->alignment );
	
	if ( skill->type != SKILL_HERB )
	{
	    fprintf( fpout, "Minlevel     %d\n",	skill->min_level	);
	}
	fprintf( fpout, "End\n\n" );
}

/*
 * Save the skill table to disk
 */
void save_skill_table()
{
    int x;
    FILE *fpout;

    if ( (fpout=fopen( SKILL_FILE, "w" )) == NULL )
    {
	bug( "Cannot open skills.dat for writting", 0 );
	perror( SKILL_FILE );
	return;
    }

    for ( x = 0; x < top_sn; x++ )
    {
	if ( !skill_table[x]->name || skill_table[x]->name[0] == '\0' )
	   break;
	fprintf( fpout, "#SKILL\n" );
	fwrite_skill( fpout, skill_table[x] );
    }
    fprintf( fpout, "#END\n" );
    fclose( fpout );
}

/*
 * Save the herb table to disk
 */
void save_herb_table()
{
    int x;
    FILE *fpout;

    if ( (fpout=fopen( HERB_FILE, "w" )) == NULL )
    {
	bug( "Cannot open herbs.dat for writting", 0 );
	perror( HERB_FILE );
	return;
    }

    for ( x = 0; x < top_herb; x++ )
    {
	if ( !herb_table[x]->name || herb_table[x]->name[0] == '\0' )
	   break;
	fprintf( fpout, "#HERB\n" );
	fwrite_skill( fpout, herb_table[x] );
    }
    fprintf( fpout, "#END\n" );
    fclose( fpout );
}

/*
 * Save the socials to disk
 */
void save_socials()
{
    FILE *fpout;
    SOCIALTYPE *social;
    int x;

    if ( (fpout=fopen( SOCIAL_FILE, "w" )) == NULL )
    {
	bug( "Cannot open socials.dat for writting", 0 );
	perror( SOCIAL_FILE );
	return;
    }

    for ( x = 0; x < 27; x++ )
    {
	for ( social = social_index[x]; social; social = social->next )
	{
	    if ( !social->name || social->name[0] == '\0' )
	    {
		bug( "Save_socials: blank social in hash bucket %d", x );
		continue;
	    }
	    fprintf( fpout, "#SOCIAL\n" );
	    fprintf( fpout, "Name        %s~\n",	social->name );
	    if ( social->char_no_arg )
		fprintf( fpout, "CharNoArg   %s~\n",	social->char_no_arg );
	    else
	        bug( "Save_socials: NULL char_no_arg in hash bucket %d", x );
	    if ( social->others_no_arg )
		fprintf( fpout, "OthersNoArg %s~\n",	social->others_no_arg );
	    if ( social->char_found )
		fprintf( fpout, "CharFound   %s~\n",	social->char_found );
	    if ( social->others_found )
		fprintf( fpout, "OthersFound %s~\n",	social->others_found );
	    if ( social->vict_found )
		fprintf( fpout, "VictFound   %s~\n",	social->vict_found );
	    if ( social->char_auto )
		fprintf( fpout, "CharAuto    %s~\n",	social->char_auto );
	    if ( social->others_auto )
		fprintf( fpout, "OthersAuto  %s~\n",	social->others_auto );
	    fprintf( fpout, "End\n\n" );
	}
    }
    fprintf( fpout, "#END\n" );
    fclose( fpout );
}

int get_skill( char *skilltype )
{
	if ( !str_cmp( skilltype, "Race" ) )
      return SKILL_RACIAL;
    if ( !str_cmp( skilltype, "Spell" ) )
      return SKILL_SPELL;
    if ( !str_cmp( skilltype, "Skill" ) )
      return SKILL_SKILL;
    if ( !str_cmp( skilltype, "Weapon" ) )
      return SKILL_WEAPON;
    if ( !str_cmp( skilltype, "Tongue" ) )
      return SKILL_TONGUE;
    if ( !str_cmp( skilltype, "Herb" ) )
      return SKILL_HERB;
    return SKILL_UNKNOWN;
}

/*
 * Save the commands to disk
 */
void save_commands()
{
    FILE *fpout;
    CMDTYPE *command;
    int x;

    if ( (fpout=fopen( COMMAND_FILE, "w" )) == NULL )
    {
	bug( "Cannot open commands.dat for writing", 0 );
	perror( COMMAND_FILE );
	return;
    }

    for ( x = 0; x < 126; x++ )
    {
	for ( command = command_hash[x]; command; command = command->next )
	{
	    if ( !command->name || command->name[0] == '\0' )
	    {
		bug( "Save_commands: blank command in hash bucket %d", x );
		continue;
	    }
	    fprintf( fpout, "#COMMAND\n" );
	    fprintf( fpout, "Name        %s~\n", command->name		);
	    fprintf( fpout, "Code        %s\n",	 skill_name(command->do_fun) );
	    fprintf( fpout, "Position    %d\n",	 command->position	);
	    fprintf( fpout, "Level       %d\n",	 command->level		);
	    fprintf( fpout, "Log         %d\n",	 command->log		);
	    fprintf( fpout, "End\n\n" );
	}
    }
    fprintf( fpout, "#END\n" );
    fclose( fpout );
}

SKILLTYPE *fread_skill( FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    SKILLTYPE *skill;

    CREATE( skill, SKILLTYPE, 1 );
    
    skill->guild = -1;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    KEY( "Alignment",	skill->alignment,	fread_number( fp ) );
	    if ( !str_cmp( word, "Affect" ) )
	    {
		SMAUG_AFF *aff;

		CREATE( aff, SMAUG_AFF, 1 );
		aff->duration = str_dup( fread_word( fp ) );
		aff->location = fread_number( fp );
		aff->modifier = str_dup( fread_word( fp ) );
		aff->bitvector = fread_number( fp );
		aff->next = skill->affects;
		skill->affects = aff;
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'C':
	    if ( !str_cmp( word, "Code" ) )
	    {
		SPELL_FUN *spellfun;
		DO_FUN	  *dofun;
		char	  *w = fread_word(fp);
		
		fMatch = TRUE;
		if ( (spellfun=spell_function(w)) != spell_notfound )
		   skill->spell_fun = spellfun;
		else
		if ( (dofun=skill_function(w)) != skill_notfound )
		   skill->skill_fun = dofun;
		else
		{
		   sprintf( buf, "fread_skill: unknown skill/spell %s", w );
		   bug( buf, 0 );
		   skill->spell_fun = spell_null;
		}
		break;
	    }
	    KEY( "Code",	skill->spell_fun, spell_function(fread_word(fp)) );
	    break;
 
	case 'D':
            KEY( "Dammsg",	skill->noun_damage,	fread_string_nohash( fp ) );
	    KEY( "Dice",	skill->dice,		fread_string_nohash( fp ) );
	    KEY( "Diechar",	skill->die_char,	fread_string_nohash( fp ) );
	    KEY( "Dieroom",	skill->die_room,	fread_string_nohash( fp ) );
	    KEY( "Dievict",	skill->die_vict,	fread_string_nohash( fp ) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
		return skill;
	    break;
	    
	case 'F':
	    KEY( "Flags",	skill->flags,		fread_number( fp ) );
	    break;

	case 'G':
	    KEY( "Guild",	skill->guild,		fread_number( fp ) );
	    break;

	case 'H':
	    KEY( "Hitchar",	skill->hit_char,	fread_string_nohash( fp ) );
	    KEY( "Hitroom",	skill->hit_room,	fread_string_nohash( fp ) );
	    KEY( "Hitvict",	skill->hit_vict,	fread_string_nohash( fp ) );
	    break;

	case 'I':
	    KEY( "Immchar",	skill->imm_char,	fread_string_nohash( fp ) );
	    KEY( "Immroom",	skill->imm_room,	fread_string_nohash( fp ) );
	    KEY( "Immvict",	skill->imm_vict,	fread_string_nohash( fp ) );
	    break;

	case 'M':
		KEY( "MadeBy", skill->made_char,  fread_string_nohash( fp ) );
	    KEY( "Mana",	skill->min_mana,	fread_number( fp ) );
	    KEY( "Minlevel",	skill->min_level,	fread_number( fp ) );
	    KEY( "Minpos",	skill->minimum_position, fread_number( fp ) );
	    KEY( "Misschar",	skill->miss_char,	fread_string_nohash( fp ) );
	    KEY( "Missroom",	skill->miss_room,	fread_string_nohash( fp ) );
	    KEY( "Missvict",	skill->miss_vict,	fread_string_nohash( fp ) );
	    break;
	
	case 'N':
            KEY( "Name",	skill->name,		fread_string_nohash( fp ) );
	    break;

	case 'P':
	    KEY( "Participants",skill->participants,	fread_number( fp ) );
		KEY( "Prototype", skill->prototype,         fread_number( fp ) );
	    break;

	case 'R':
	    KEY( "Rounds",	skill->beats,		fread_number( fp ) );
	    break;

	case 'S':
	    KEY( "Slot",	skill->slot,		fread_number( fp ) );
	    KEY( "Saves",	skill->saves,		fread_number( fp ) );
	    break;

	case 'T':
	    KEY( "Target",	skill->target,		fread_number( fp ) );
	    KEY( "Teachers",	skill->teachers,	fread_string_nohash( fp ) );
	    KEY( "Type",	skill->type,  get_skill(fread_word( fp ))  );
	    break;

	case 'V':
	    KEY( "Value",	skill->value,		fread_number( fp ) );
	    break;

	case 'W':
	    KEY( "Wearoff",	skill->msg_off,		fread_string_nohash( fp ) );
	    break;
	}
	
	if ( !fMatch )
	{
            sprintf( buf, "Fread_skill: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

void load_skill_table()
{
    FILE *fp;

    if ( ( fp = fopen( SKILL_FILE, "r" ) ) != NULL )
    {
	top_sn = 0;
	for ( ;; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
                bug( "Load_skill_table: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
            if ( !str_cmp( word, "SKILL"      ) )
	    {
		if ( top_sn >= MAX_SKILL )
		{
		    bug( "load_skill_table: more skills than MAX_SKILL %d", MAX_SKILL );
		    fclose( fp );
		    return;
		}
		skill_table[top_sn++] = fread_skill( fp );
		continue;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
                bug( "Load_skill_table: bad section.", 0 );
		continue;
	    }
	}
	fclose( fp );
    }
    else
    {
	bug( "Cannot open skills.dat", 0 );
 	exit(0);
    }
}

/*
 * Load in all the race files.
 */
void load_races( )
{
    FILE *fpList;
    char *filename;
    char racelist[256];
    char buf[MAX_STRING_LENGTH];
    int i; 

    MAX_PC_RACE = 0;
   /*
    * Pre-init the race_table with blank races
    */
    for(i=0;i<MAX_RACE;i++)
    	race_table[i] = NULL;

    sprintf( racelist, "%s%s", RACEDIR, RACE_LIST );
    if ( ( fpList = fopen( racelist, "r" ) ) == NULL )
    {
	perror( racelist );
	exit( 1 );
    }

    for ( ; ; )
    {
	filename = feof( fpList ) ? "$" : fread_word( fpList );
	if ( filename[0] == '$' )
	  break;

	if ( !load_race_file( filename ) )
	{
	  sprintf( buf, "Cannot load race file: %s", filename );
	  bug( buf, 0 );
	}
	else
	  MAX_PC_RACE++;
    }
    for ( i = 0; i < MAX_RACE; i++ )
    {
        if ( race_table[i] == NULL )
        {
          CREATE( race_table[i], struct race_type, 1 );
          sprintf( race_table[i]->race_name, "%s", "unused" );
        }
    }
    fclose( fpList );
    return;
}

void write_race_file( int ra )
{
    FILE *fpout;
    char buf[MAX_STRING_LENGTH];
    char filename[MAX_INPUT_LENGTH];
    struct race_type *race = race_table[ra];
    int i;

    if( !race->race_name)
    {
	sprintf( buf, "Race %d has null name, not writing .race file.", ra );
	bug( buf, 0 );
	return;
    }

    sprintf( filename, "%s%s.race", RACEDIR, race->race_name );
    if ( (fpout=fopen(filename, "w+")) == NULL )
    {
	sprintf( buf, "Cannot open: %s for writing", filename );
	bug( buf, 0 );
	return;
    }

    fprintf( fpout, "Name        %s~\n",	race->race_name		);
    fprintf( fpout, "Race        %d\n",		ra  			);
    fprintf( fpout, "Classes     %d\n",		race->class_restriction );
	fprintf( fpout, "Nospeak     %d\n",		race->nospeak );
    fprintf( fpout, "Str_Plus    %d\n",		race->str_plus		);
    fprintf( fpout, "Dex_Plus    %d\n",		race->dex_plus		);
    fprintf( fpout, "Wis_Plus    %d\n",		race->wis_plus		);
    fprintf( fpout, "Int_Plus    %d\n",		race->int_plus		);
    fprintf( fpout, "Con_Plus    %d\n",		race->con_plus		);
    fprintf( fpout, "Cha_Plus    %d\n",		race->cha_plus		);
    fprintf( fpout, "Lck_Plus    %d\n",		race->lck_plus		);
    fprintf( fpout, "Affected    %d\n",		race->affected	);
    fprintf( fpout, "Resist      %d\n",		race->resist		);
    fprintf( fpout, "Suscept     %d\n",		race->suscept		);
    fprintf( fpout, "Language    %d\n",		race->language		);
    fprintf( fpout, "AC_Plus    %d\n",		race->ac_plus		);
    fprintf( fpout, "Height     %d\n",		race->height		);
    fprintf( fpout, "Weight     %d\n",		race->weight		);
    fprintf( fpout, "Hunger_Mod  %d\n",		race->hunger_mod	);
    fprintf( fpout, "Thirst_mod  %d\n",		race->thirst_mod	);
	fprintf( fpout, "Death_age   %d\n",		race->death_age	);
	fprintf( fpout, "Consent     %d\n",		race->consent	);

	for ( i = 0; i < MAX_ABILITY; i++ )
	fprintf( fpout, "Ability  %d\n",
		race->ability[i] );

    fprintf( fpout, "End\n" );
    fclose( fpout );
}

bool load_race_file( char *fname )
{
	char buf[MAX_STRING_LENGTH];
	char *word;
	char *race_name = NULL;
	bool fMatch;
	struct race_type *race;
	int ra = -1;
	FILE *fp;
	int z=0;
    
	sprintf( buf, "%s%s", RACEDIR, fname );
	if ( ( fp = fopen( buf, "r" ) ) == NULL )
	{
		perror( buf );
		return FALSE;
	}
    CREATE( race, struct race_type, 1 );
    for ( ; ; )
	{
		word   = feof( fp ) ? "End" : fread_word( fp );
		fMatch = FALSE;
		
		switch ( UPPER(word[0]) )
		{
		
		case '*':
			fMatch = TRUE;
			fread_to_eol( fp );
			break;
		
		case 'A':
			if ( !str_cmp( word, "Ability" ) )
			{
				if ( ra < 0 || ra >= MAX_RACE )
				{
					sprintf( buf, "load_race_file: Title -- race bad/not found (%d)", ra );
					bug( buf, 0 );
				}
				else
				{
					if ( z < MAX_ABILITY+1 )
					{
						race->ability[z] = fread_number( fp );
						++z;
					}
					else
						bug( "load_race_file: Too many Abilities" );
				}
				fMatch = TRUE;
				break;
			}
			KEY( "AC_Plus",	race->ac_plus,		fread_number( fp )	);
			KEY( "Affected",	race->affected,		fread_number( fp )	);
			break;
		
		case 'C':
			KEY( "Con_Plus",	race->con_plus,		fread_number( fp )	);
			KEY( "Consent",		race->consent,		fread_number( fp )	);
			KEY( "Cha_Plus",	race->cha_plus,		fread_number( fp )	);
			KEY( "Classes",	race->class_restriction,		fread_number( fp )	);
			break;
		
		case 'D':
			KEY( "Dex_Plus",	race->dex_plus,		fread_number( fp )	);
			KEY( "Death_age",   race->death_age,	fread_number( fp ) );
			break;
		
		case 'E':
			if ( !str_cmp( word, "End" ) )
			{
				fclose( fp );
				if ( ra < 0 || ra >= MAX_RACE )
				{
					sprintf( buf, "Load_race_file: Race (%s) bad/not found (%d)",
						race->race_name ? race->race_name : "name not found", ra );
					bug( buf, 0 );
					DISPOSE( race );
					return FALSE;
				}
				if ( race->death_age < 15 )
					race->death_age = 150;
                race_table[ra] = race;
				return TRUE;
			}
			break;
		
		case 'I':
			KEY( "Int_Plus",	race->int_plus,		fread_number( fp )	);
			break;
		
		case 'H':
			KEY( "Height",	race->height,		fread_number( fp )	);
			KEY( "Hunger_Mod",	race->hunger_mod,	fread_number( fp )	);
			break;
		
		case 'L':
			KEY( "Language",	race->language,		fread_number( fp )	);
			KEY( "Lck_Plus",	race->lck_plus,		fread_number( fp )	);
			break;
				
		case 'N':
			KEY( "Name",	race_name,	fread_string( fp )	);
			KEY( "Nospeak",	race->nospeak,	fread_number( fp )	);
			break;
		
		case 'R':
			KEY( "Race",	ra,			fread_number( fp )	);
			KEY( "Resist",	race->resist,		fread_number( fp )	);
			break;
		
		case 'S':
			KEY( "Str_Plus",	race->str_plus,		fread_number( fp )	);
			KEY( "Suscept",	race->suscept,		fread_number( fp )	);
			break;
		
		case 'T':
			KEY( "Thirst_Mod",	race->thirst_mod,	fread_number( fp )	);
			break;
		
		case 'W':
			KEY( "Weight",	race->weight,		fread_number( fp )	);
			KEY( "Wis_Plus",	race->wis_plus,		fread_number( fp )	);
			break;
		}
        
		if ( race_name != NULL) 
            sprintf(race->race_name,"%-.16s",race_name);
		if ( !fMatch )
		{
			sprintf( buf, "load_race_file: no match: %s", word );
			bug( buf, 0 );
		}
   }
   return FALSE;
}

void load_herb_table()
{
    FILE *fp;

    if ( ( fp = fopen( HERB_FILE, "r" ) ) != NULL )
    {
	top_herb = 0;
	for ( ;; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_herb_table: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
            if ( !str_cmp( word, "HERB"      ) )
	    {
		if ( top_herb >= MAX_HERB )
		{
		    bug( "load_herb_table: more herbs than MAX_HERB %d", MAX_HERB );
		    fclose( fp );
		    return;
		}
		herb_table[top_herb++] = fread_skill( fp );
		if ( herb_table[top_herb-1]->slot == 0 )
		    herb_table[top_herb-1]->slot = top_herb-1;
		continue;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
                bug( "Load_herb_table: bad section.", 0 );
		continue;
	    }
	}
	fclose( fp );
    }
    else
    {
	bug( "Cannot open herbs.dat", 0 );
 	exit(0);
    }
}

void fread_social( FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    SOCIALTYPE *social;

    CREATE( social, SOCIALTYPE, 1 );

    for ( ;; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'C':
	    KEY( "CharNoArg",	social->char_no_arg,	fread_string_nohash(fp) );
	    KEY( "CharFound",	social->char_found,	fread_string_nohash(fp) );
	    KEY( "CharAuto",	social->char_auto,	fread_string_nohash(fp) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if ( !social->name )
		{
		    bug( "Fread_social: Name not found", 0 );
		    free_social( social );
		    return;
		}
		if ( !social->char_no_arg )
		{
		    bug( "Fread_social: CharNoArg not found", 0 );
		    free_social( social );
		    return;
		}
		add_social( social );
		return;
	    }
	    break;

	case 'N':
	    KEY( "Name",	social->name,		fread_string_nohash(fp) );
	    break;

	case 'O':
	    KEY( "OthersNoArg",	social->others_no_arg,	fread_string_nohash(fp) );
	    KEY( "OthersFound",	social->others_found,	fread_string_nohash(fp) );
	    KEY( "OthersAuto",	social->others_auto,	fread_string_nohash(fp) );
	    break;

	case 'V':
	    KEY( "VictFound",	social->vict_found,	fread_string_nohash(fp) );
	    break;
	}
	
	if ( !fMatch )
	{
            sprintf( buf, "Fread_social: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

void load_socials()
{
    FILE *fp;

    if ( ( fp = fopen( SOCIAL_FILE, "r" ) ) != NULL )
    {
	top_sn = 0;
	for ( ;; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
                bug( "Load_socials: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
            if ( !str_cmp( word, "SOCIAL"      ) )
	    {
                fread_social( fp );
	    	continue;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
                bug( "Load_socials: bad section.", 0 );
		continue;
	    }
	}
	fclose( fp );
    }
    else
    {
	bug( "Cannot open socials.dat", 0 );
 	exit(0);
    }
}

void fread_command( FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    CMDTYPE *command;

    CREATE( command, CMDTYPE, 1 );

    for ( ;; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'C':
	    KEY( "Code",	command->do_fun,	skill_function(fread_word(fp)) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if ( !command->name )
		{
		    bug( "Fread_command: Name not found", 0 );
		    free_command( command );
		    return;
		}
		if ( !command->do_fun )
		{
		    bug( "Fread_command: Function not found", 0 );
		    free_command( command );
		    return;
		}
		add_command( command );
		return;
	    }
	    break;

	case 'L':
	    KEY( "Level",	command->level,		fread_number(fp) );
	    KEY( "Log",		command->log,		fread_number(fp) );
	    break;

	case 'N':
	    KEY( "Name",	command->name,		fread_string_nohash(fp) );
	    break;

	case 'P':
	    KEY( "Position",	command->position,	fread_number(fp) );
	    break;
	}
	
	if ( !fMatch )
	{
            sprintf( buf, "Fread_command: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

void load_commands()
{
    FILE *fp;

    if ( ( fp = fopen( COMMAND_FILE, "r" ) ) != NULL )
    {
	top_sn = 0;
	for ( ;; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
                bug( "Load_commands: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
            if ( !str_cmp( word, "COMMAND"      ) )
	    {
                fread_command( fp );
	    	continue;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
                bug( "Load_commands: bad section.", 0 );
		continue;
	    }
	}
	fclose( fp );
    }
    else
    {
	bug( "Cannot open commands.dat", 0 );
 	exit(0);
    }

}

/*
 * Save the xsocials to disk
 */
void save_xsocials()
{
    FILE *fpout;
    XSOCIALTYPE *xsocial;
    int x;

    if ( (fpout=fopen( XSOCIAL_FILE, "w" )) == NULL )
    {
	bug( "Cannot open xsocials.dat for writting", 0 );
	perror( XSOCIAL_FILE );
	return;
    }

    for ( x = 0; x < 27; x++ )
    {
		for ( xsocial = xsocial_index[x]; xsocial; xsocial = xsocial->next )
		{
			if ( !xsocial->name || xsocial->name[0] == '\0' )
			{
				bug( "Save_xsocials: blank xsocial in hash bucket %d", x );
				continue;
			}
			fprintf( fpout, "#XSOCIAL\n" );
			fprintf( fpout, "Name        %s~\n",	xsocial->name );
			if ( xsocial->char_no_arg )
				fprintf( fpout, "CharNoArg   %s~\n",	xsocial->char_no_arg );
			else
				bug( "Save_xsocials: NULL char_no_arg in hash bucket %d", x );
			if ( xsocial->others_no_arg )
				fprintf( fpout, "OthersNoArg %s~\n",	xsocial->others_no_arg );
			if ( xsocial->char_found )
				fprintf( fpout, "CharFound   %s~\n",	xsocial->char_found );
			if ( xsocial->others_found )
				fprintf( fpout, "OthersFound %s~\n",	xsocial->others_found );
			if ( xsocial->vict_found )
				fprintf( fpout, "VictFound   %s~\n",	xsocial->vict_found );
			if ( xsocial->char_auto )
				fprintf( fpout, "CharAuto    %s~\n",	xsocial->char_auto );
			if ( xsocial->others_auto )
				fprintf( fpout, "OthersAuto  %s~\n",	xsocial->others_auto );
			fprintf( fpout, "GenderTo     %d\n",			xsocial->genderto );
			fprintf( fpout, "Gender       %d\n",			xsocial->gender );
			fprintf( fpout, "Stage        %d\n",			xsocial->stage );
			fprintf( fpout, "Position     %d\n",			xsocial->position );
			fprintf( fpout, "Self         %d\n",			xsocial->self );
			fprintf( fpout, "Other        %d\n",			xsocial->other );
			fprintf( fpout, "Chance       %d\n",			xsocial->chance );
			fprintf( fpout, "End\n\n" );
		}
    }
    fprintf( fpout, "#END\n" );
    fclose( fpout );
}

void fread_xsocial( FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    XSOCIALTYPE *xsocial;

    CREATE( xsocial, XSOCIALTYPE, 1 );

    for ( ;; )
    {
		word   = feof( fp ) ? "End" : fread_word( fp );
		fMatch = FALSE;

		switch ( UPPER(word[0]) )
		{
		case '*':
			fMatch = TRUE;
			fread_to_eol( fp );
			break;

		case 'C':
			KEY( "CharNoArg",	xsocial->char_no_arg,	fread_string_nohash(fp) );
			KEY( "CharFound",	xsocial->char_found,	fread_string_nohash(fp) );
			KEY( "CharAuto",	xsocial->char_auto,	fread_string_nohash(fp) );
			KEY( "Chance",		xsocial->chance,	fread_number(fp) );
			break;

		case 'E':
			if ( !str_cmp( word, "End" ) )
			{
				if ( !xsocial->name )
				{
					bug( "Fread_xsocial: Name not found", 0 );
					free_xsocial( xsocial );
					return;
				}
				if ( !xsocial->char_no_arg )
				{
					bug( "Fread_xsocial: CharNoArg not found", 0 );
					free_xsocial( xsocial );
					return;
				}
				add_xsocial( xsocial );
				return;
			}
			break;
		case 'G':
			KEY( "Gender",	xsocial->gender,		fread_number(fp) );
			KEY( "GenderTo", xsocial->genderto,		fread_number(fp) );
			break;

		case 'N':
			KEY( "Name",	xsocial->name,		fread_string_nohash(fp) );
			break;

		case 'O':
			KEY( "Other",		xsocial->other,			fread_number(fp) );
			KEY( "OthersNoArg",	xsocial->others_no_arg,	fread_string_nohash(fp) );
			KEY( "OthersFound",	xsocial->others_found,	fread_string_nohash(fp) );
			KEY( "OthersAuto",	xsocial->others_auto,	fread_string_nohash(fp) );
			break;

		case 'P':
			KEY( "Position", xsocial->position,			fread_number(fp) );
			break;
		
		case 'S':
			KEY( "Self", xsocial->self,			fread_number(fp) );
			KEY( "Stage", xsocial->stage,		fread_number(fp) );
			break;
		
		case 'V':
			KEY( "VictFound",	xsocial->vict_found,	fread_string_nohash(fp) );
			break;
		}
		
		if ( !fMatch )
		{
            sprintf( buf, "Fread_xsocial: no match: %s", word );
			bug( buf, 0 );
		}
    }
}

void load_xsocials()
{
    FILE *fp;

    if ( ( fp = fopen( XSOCIAL_FILE, "r" ) ) != NULL )
    {
		top_sn = 0;
		for ( ;; )
		{
			char letter;
			char *word;

			letter = fread_letter( fp );
			if ( letter == '*' )
			{
				fread_to_eol( fp );
				continue;
			}

			if ( letter != '#' )
			{
				bug( "Load_xsocials: # not found.", 0 );
				break;
			}

			word = fread_word( fp );
			if ( !str_cmp( word, "XSOCIAL"      ) )
			{
				fread_xsocial( fp );
				continue;
			}
			else if ( !str_cmp( word, "END"	) )
				break;
			else
			{
				bug( "Load_xsocials: bad section.", 0 );
				continue;
			}
		}
		fclose( fp );
    }
    else
    {
		bug( "Cannot open xsocials.dat", 0 );
		exit(0);
    }
}
