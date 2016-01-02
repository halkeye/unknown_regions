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
*			     Mud constants module			   *
****************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include "mud.h"

/* undef these at EOF */
#define AM 95
#define AC 95
#define AT 85
#define AW 85
#define AV 95
#define AD 95
#define AR 90
#define AA 95


char * const he_she  [3] = { "it",  "he",  "she" };
char * const him_her [3] = { "it",  "him", "her" };
char * const his_her [3] = { "its", "his", "her" };

/*
 * Race table.
 */

char *	const	npc_race	[MAX_NPC_RACE] =
{
"Human", "Wookiee", "Twi'lek", "Rodian", "Hutt", "Mon Calamari", "Noghri",
"Gamorrean", "Jawa", "Adarian", "Ewok", "Verpine", "Defel", "Trandoshan", 
"Chadra-Fan", "Quarren", "Duinuogwuin", "r17", "r18", "r19", "r20", "r21", 
"r22", "r23", "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31", 
"r32", "r33", "r34", "r35", "r36", "r37", "r38", "r39", "r40", "r41", 
"r42", "r43", "r44", "r45", "r46", "r47", "r48", "r49", "r50", "r51", 
"r52", "r53", "r54", "r55", "r56", "r57", "r58", "r59", "r60", "r61", 
"r62", "r63", "r64", "r65", "r66", "r67", "r68", "r69", "r70", "r71", 
"r72", "r73", "r74", "r75", "r76", "r77", "r78", "r79", "r80", "r81", 
"r82", "r83", "r84", "r85", "r86", "r87", "r88", "r89", "r90"
};


char *	const	ability_name	[MAX_ABILITY] =
{
	"combat",				/* Combat Skills		*/
	"piloting",				/* Piloting Skills		*/
	"engineering",			/* Engineering Skills	*/
	"hunting",				/* Bounty Hunting Skills*/
	"smuggling",			/* Smuggling Skills		*/
	"diplomacy",			/* Diplomacy Skills		*/
	"piracy",				/* Piracy Skills		*/
	"medical"				/* Medical Skills		*/
};


char *	const	hair_color	[MAX_HAIR] =
{
"Bald", "Hairless" , "Black", "Brown",
"Red", "Blonde", "Strawberry Blonde", "Argent",
"Golden Blonde", "Platinum Blonde", "Light Brown", "Midnite Black",
"Charcoal", "Expresso", "Chocolate", "Walnut Brown",
"Pecan Brown", "Blonde", "Argent","Irish Red",
"Auburn", "Paprika", "Mahogany","Flame",
"Grey", "Pewter", "Silver","Snowflake", 
"Purple", "None", "Navy", "Blue",
"Hunter Green", "Cyan", "Magenta","Jet Black"
};

char *	const	complex_type	[MAX_COMPLEX] =
{
"Olive", "Pale", "Tanned", "Freckled", "Rosy",
"Molteded", "Dark", "Scaled"
};

char *	const	build_type	[MAX_BUILD] =
{
"Thin", "Lean", "Muscular", "Bulky", "Fat", "Frail",
"Toned", "Slender", "Willowy", "Curvaceous"
};

char *	const	eye_color	[MAX_EYE] =
{
"Blue", "Green", "Brown", "Grey", "Black", "Hazel",
"Aqua", "Royal Blue", "Baby Blue", "Evergreen",
"Jade Green", "Muddy Brown", "Sable Brown",
"Midnite Black", "Purple", "Blood Red", "Charcoal Grey",
"Sky Blue", "Lavender", "Amber Gold"
};

/*
 * Attribute bonus tables.
 */
const	struct	str_app_type	str_app		[30]		=
{
    { -5, -4,   0,  0 },  /* 0  */
    { -5, -4,   3,  1 },  /* 1  */
    { -3, -2,   3,  2 },
    { -3, -1,  10,  3 },  /* 3  */
    { -2, -1,  25,  4 },
    { -2, -1,  55,  5 },  /* 5  */
    { -1,  0,  80,  6 },
    { -1,  0,  90,  8 },
    {  0,  0, 100, 10 },
    {  0,  0, 100, 12 },
    {  0,  0, 115, 14 }, /* 10  */
    {  0,  0, 115, 15 },
    {  0,  0, 140, 16 },
    {  0,  0, 140, 17 }, /* 13  */
    {  0,  1, 170, 18 },
    {  1,  1, 170, 19 }, /* 15  */
    {  1,  2, 195, 20 },
    {  2,  3, 220, 22 },
    {  2,  4, 250, 25 }, /* 18  */
    {  3,  5, 400, 30 },
    {  3,  6, 500, 35 }, /* 20  */
    {  4,  7, 600, 40 },
    {  5,  7, 700, 45 },
    {  6,  8, 800, 50 },
    {  8, 10, 900, 55 },
    { 10, 12, 999, 60 }, /* 25   */
	{ 10, 12, 999, 65 },  /* 26   */
	{ 10, 12, 999, 70 },  /* 27   */
	{ 10, 12, 999, 75 },  /* 28   */
	{ 10, 12, 999, 80 }  /* 29   */
};



const	struct	int_app_type	int_app		[30]		=
{
    {  3 },	/*  0 */
    {  5 },	/*  1 */
    {  7 },
    {  8 },	/*  3 */
    {  9 },
    { 10 },	/*  5 */
    { 11 },
    { 12 },
    { 13 },
    { 15 },
    { 17 },	/* 10 */
    { 19 },
    { 22 },
    { 25 },
    { 28 },
    { 31 },	/* 15 */
    { 34 },
    { 37 },
    { 40 },	/* 18 */
    { 44 },
    { 49 },	/* 20 */
    { 55 },
    { 60 },
    { 70 },
    { 85 },
    { 99 },	/* 25 */
	{ 99 },	/* 26 */
	{ 99 },	/* 27 */
	{ 99 },	/* 28 */
	{ 99 }	/* 29 */
};



const	struct	wis_app_type	wis_app		[30]		=
{
    { 0 },	/*  0 */
    { 0 },	/*  1 */
    { 0 },
    { 0 },	/*  3 */
    { 0 },
    { 1 },	/*  5 */
    { 1 },
    { 1 },
    { 1 },
    { 2 },
    { 2 },	/* 10 */
    { 2 },
    { 2 },
    { 2 },
    { 2 },
    { 3 },	/* 15 */
    { 3 },
    { 4 },
    { 5 },	/* 18 */
    { 5 },
    { 5 },	/* 20 */
    { 6 },
    { 6 },
    { 6 },
    { 6 },
    { 7 },	/* 25 */
	{ 7 },	/* 26 */
	{ 7 },	/* 27 */
	{ 7 },	/* 28 */
	{ 7 }	/* 29 */
};



const	struct	dex_app_type	dex_app		[30]		=
{
    {   60 },   /* 0 */
    {   50 },   /* 1 */
    {   50 },
    {   40 },
    {   30 },
    {   20 },   /* 5 */
    {   10 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 60 },
    { - 75 },
    { - 90 },
    { -105 },
    { -120 },    /* 25 */
	{ -120 },    /* 26 */
	{ -120 },    /* 27 */
	{ -120 },    /* 28 */
	{ -120 }    /* 29 */
};



const	struct	con_app_type	con_app		[30]		=
{
    { -4, 20 },   /*  0 */
    { -3, 25 },   /*  1 */
    { -2, 30 },
    { -2, 35 },	  /*  3 */
    { -1, 40 },
    { -1, 45 },   /*  5 */
    { -1, 50 },
    {  0, 55 },
    {  0, 60 },
    {  0, 65 },
    {  0, 70 },   /* 10 */
    {  0, 75 },
    {  0, 80 },
    {  0, 85 },
    {  0, 88 },
    {  1, 90 },   /* 15 */
    {  2, 95 },
    {  2, 97 },
    {  3, 99 },   /* 18 */
    {  3, 99 },
    {  4, 99 },   /* 20 */
    {  4, 99 },
    {  5, 99 },
    {  6, 99 },
    {  7, 99 },
    {  8, 99 },    /* 25 */
	{  9, 99 },    /* 26 */
    {  9, 99 },    /* 27 */
    {  9, 99 },    /* 28 */
    {  9, 99 }    /* 29 */
};


const	struct	cha_app_type	cha_app		[30]		=
{
    { - 60 },   /* 0 */
    { - 50 },   /* 1 */
    { - 50 },
    { - 40 },
    { - 30 },
    { - 20 },   /* 5 */
    { - 10 },
    { -  5 },
    { -  1 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    1 },
    {    5 },   /* 15 */
    {   10 },
    {   20 },
    {   30 },
    {   40 },
    {   50 },   /* 20 */
    {   60 },
    {   70 },
    {   80 },
    {   90 },
    {   99 },    /* 25 */
	{   99 },    /* 26 */
	{   99 },    /* 27 */
	{   99 },    /* 28 */
	{   99 }    /* 29 */
};

/* Have to fix this up - not exactly sure how it works (Scryn) */
const	struct	lck_app_type	lck_app		[30]		=
{
    {   60 },   /* 0 */
    {   50 },   /* 1 */
    {   50 },
    {   40 },
    {   30 },
    {   20 },   /* 5 */
    {   10 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 60 },
    { - 75 },
    { - 90 },
    { -105 },
    { -120 },    /* 25 */
	{ -120 },    /* 26 */
	{ -120 },    /* 27 */
	{ -120 },    /* 28 */
	{ -120 }    /* 29 */
};

/*
 * Liquid properties.
 * Used in #OBJECT section of area file.
 */

const	struct	liq_type	liq_table	[LIQ_MAX]	=
{
/*	  "Liquid Name",		"Liquid Color",	{ COND_DRUNK, COND_FULL , COND_THIRST } */
    { "water",				"clear",	{  0, 1, 10 }	},  /*  0 */
    { "beer",				"amber",	{  3, 2,  5 }	},
    { "wine",				"rose",		{  5, 2,  5 }	},
    { "ale",				"brown",	{  2, 2,  5 }	},
    { "dark ale",			"dark",		{  1, 2,  5 }	},

    { "whisky",				"golden",	{  6, 1,  4 }	},  /*  5 */
    { "lemonade",			"pink",		{  0, 1,  8 }	},
    { "firebreather",		"boiling",	{ 10, 0,  0 }	},
    { "local specialty",	"everclear",	{  3, 3,  3 }	},
    { "slime mold juice",	"green",	{  0, 4, -8 }	},

    { "milk",				"white",	{  0, 3,  6 }	},  /* 10 */
    { "tea",				"tan",		{  0, 1,  6 }	},
    { "coffee",				"black",	{  0, 1,  6 }	},
    { "blood",				"red",		{  0, 2, -1 }	},
    { "salt water",			"clear",	{  0, 1, -2 }	},

    { "cola",				"cherry",	{  0, 1,  5 }	},  /* 15 */
    { "mead",				"honey color",	{  4, 2,  5 }	},  
    { "grog",				"thick brown",	{  3, 2,  5 }	},  
    { "milkshake",			"creamy",       {  0, 8,  5 }   },   
	{ "semen",				"creamy white",       {  0, 10,  10 } }   
};

char *	const	attack_table	[13] =
{
    "hit",
    "slice",  "stab",  "slash", "whip", "claw",
    "blast",  "pound", "crush", "shot", "bite",
    "pierce", "suction"
};

/* Weather constants - FB */
char * const temp_settings[MAX_CLIMATE] =
{
	"cold",
	"cool",
	"normal",
	"warm",
	"hot",
};

char * const precip_settings[MAX_CLIMATE] =
{
	"arid",
	"dry",
	"normal",
	"damp",
	"wet",
};

char * const wind_settings[MAX_CLIMATE] =
{
	"still",
	"calm",
	"normal",
	"breezy",
	"windy",
};

char * const preciptemp_msg[6][6] =
{
	/* precip = 0 */
	{
		"Frigid temperatures settle over the land",
		"It is bitterly cold",
		"The weather is crisp and dry",
		"A comfortable warmth sets in",
		"A dry heat warms the land",
		"Seething heat bakes the land"
	 },
	 /* precip = 1 */
	 {
	 	"A few flurries drift from the high clouds",
	 	"Frozen drops of rain fall from the sky",
	 	"An occasional raindrop falls to the ground",
	 	"Mild drops of rain seep from the clouds",
	 	"It is very warm, and the sky is overcast",
	 	"High humidity intensifies the seering heat"
	 },
	 /* precip = 2 */
	 {
	 	"A brief snow squall dusts the earth",
	 	"A light flurry dusts the ground",
	 	"Light snow drifts down from the heavens",
	 	"A light drizzle mars an otherwise perfect day",
	 	"A few drops of rain fall to the warm ground",
	 	"A light rain falls through the sweltering sky"
	 },
	 /* precip = 3 */
	 {
	 	"Snowfall covers the frigid earth",
	 	"Light snow falls to the ground",
	 	"A brief shower moistens the crisp air",
	 	"A pleasant rain falls from the heavens",
	 	"The warm air is heavy with rain",
	 	"A refreshing shower eases the oppresive heat"
	 },
	 /* precip = 4 */
	 {
	 	"Sleet falls in sheets through the frosty air",
	 	"Snow falls quickly, piling upon the cold earth",
	 	"Rain pelts the ground on this crisp day",
	 	"Rain drums the ground rythmically",
	 	"A warm rain drums the ground loudly",
	 	"Tropical rain showers pelt the seering ground"
	 },
	 /* precip = 5 */
	 {
	 	"A downpour of frozen rain covers the land in ice",
	 	"A blizzard blankets everything in pristine white",
	 	"Torrents of rain fall from a cool sky",
	 	"A drenching downpour obscures the temperate day",
	 	"Warm rain pours from the sky",
	 	"A torrent of rain soaks the heated earth"
	 }
};

char * const windtemp_msg[6][6] =
{
	/* wind = 0 */
	{
		"The frigid air is completely still",
		"A cold temperature hangs over the area",
		"The crisp air is eerily calm",
		"The warm air is still",
		"No wind makes the day uncomfortably warm",
		"The stagnant heat is sweltering"
	},
	/* wind = 1 */
	{
		"A light breeze makes the frigid air seem colder",
		"A stirring of the air intensifies the cold",
		"A touch of wind makes the day cool",
		"It is a temperate day, with a slight breeze",
		"It is very warm, the air stirs slightly",
		"A faint breeze stirs the feverish air"
	},
	/* wind = 2 */
	{
		"A breeze gives the frigid air bite",
		"A breeze swirls the cold air",
		"A lively breeze cools the area",
		"It is a temperate day, with a pleasant breeze",
		"Very warm breezes buffet the area",
		"A breeze ciculates the sweltering air"
	},
	/* wind = 3 */
	{
		"Stiff gusts add cold to the frigid air",
		"The cold air is agitated by gusts of wind",
		"Wind blows in from the north, cooling the area",
		"Gusty winds mix the temperate air",
		"Brief gusts of wind punctuate the warm day",
		"Wind attempts to cut the sweltering heat"
	},
	/* wind = 4 */
	{
		"The frigid air whirls in gusts of wind",
		"A strong, cold wind blows in from the north",
		"Strong wind makes the cool air nip",
		"It is a pleasant day, with gusty winds",
		"Warm, gusty winds move through the area",
		"Blustering winds punctuate the seering heat"
	},
	/* wind = 5 */
	{
		"A frigid gale sets bones shivering",
		"Howling gusts of wind cut the cold air",
		"An angry wind whips the air into a frenzy",
		"Fierce winds tear through the tepid air",
		"Gale-like winds whip up the warm air",
		"Monsoon winds tear the feverish air"
	}
};

char * const precip_msg[3] =
{
	"there is not a cloud in the sky",
	"pristine white clouds are in the sky",
	"thick, grey clouds mask the sun"
};

char * const wind_msg[6] =
{
	"there is not a breath of wind in the air",
	"a slight breeze stirs the air",
	"a breeze wafts through the area",
	"brief gusts of wind punctuate the air",
	"angry gusts of wind blow",
	"howling winds whip the air into a frenzy"
};

/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n)	n
#define LI LEVEL_IMMORTAL

#undef AM 
#undef AC 
#undef AT 
#undef AW 
#undef AV 
#undef AD 
#undef AR
#undef AA

#undef LI
