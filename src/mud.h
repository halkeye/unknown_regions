/***************************************************************************
*                           STAR WARS REALITY 1.0                          *
*--------------------------------------------------------------------------*
* Star Wars Reality Code Additions and changes from the Smaug Code         *
* copyright (c) 1997, 1998 by Sean Cooper                                  *
* -------------------------------------------------------------------------*
* Starwars and Starwars Names copyright (c) Lucasfilm Ltd.                 *
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
*			    Main mud header file			   *
****************************************************************************/

#include <stdlib.h>
#include <limits.h>
#include <sys/cdefs.h>
#include <sys/time.h>
#include <math.h>

#include <stdarg.h>
#include <stdio.h>

#ifdef WIN32
  #include <winsock.h>
  #include <sys/types.h>
  #pragma warning( disable: 4018 4244 4761)
  #define NOCRYPT
  #define index strchr
  #define rindex strrchr
#else
  #include <unistd.h>
#ifndef SYSV
  #include <sys/cdefs.h>
#else
  #include <re_comp.h>
#endif
  #include <sys/time.h>
#endif

typedef	int				ch_ret;
typedef	int				obj_ret;

/*
 * Accommodate old non-Ansi compilers.
 */
#if defined(TRADITIONAL)
#define const
#define args( list )			( )
#define DECLARE_DO_FUN( fun )		void fun( )
#define DECLARE_SPEC_FUN( fun )		bool fun( )
#define DECLARE_SPELL_FUN( fun )	ch_ret fun( )
#else
#define args( list )			list
#define DECLARE_DO_FUN( fun )		DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )		SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#endif




/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if	!defined(FALSE)
#define FALSE	 0
#endif

#if	!defined(TRUE)
#define TRUE	 1
#endif

#if	!defined(BERR)
#define BERR	 255
#endif

#if	defined(_AIX)
#if	!defined(const)
#define const
#endif
typedef int				sh_int;
typedef int				bool;
#define unix
#else
typedef short    int			sh_int;
typedef unsigned char			bool;
#endif

/*
 * Structure types.
 */
typedef struct	affect_data		AFFECT_DATA;
typedef struct	area_data		AREA_DATA;
typedef struct  auction_data            AUCTION_DATA; /* auction data */
typedef struct	watch_data		WATCH_DATA;
typedef struct	ban_data		BAN_DATA;
typedef struct	extracted_char_data	EXTRACT_CHAR_DATA;
typedef struct	char_data		CHAR_DATA;
typedef struct	hunt_hate_fear		HHF_DATA;
typedef struct	fighting_data		FIGHT_DATA;
typedef struct	descriptor_data		DESCRIPTOR_DATA;
typedef struct	exit_data		EXIT_DATA;
typedef struct	extra_descr_data	EXTRA_DESCR_DATA;
typedef struct	help_data		HELP_DATA;
typedef struct	mob_index_data		MOB_INDEX_DATA;
typedef struct  nuisance_data		NUISANCE_DATA;
typedef struct	note_data		NOTE_DATA;
typedef struct	comment_data		COMMENT_DATA;
typedef struct	board_data		BOARD_DATA;
typedef struct	obj_data		OBJ_DATA;
typedef struct	obj_index_data		OBJ_INDEX_DATA;
typedef struct	pc_data			PC_DATA;
typedef struct	reset_data		RESET_DATA;
typedef struct	room_index_data		ROOM_INDEX_DATA;
typedef struct	shop_data		SHOP_DATA;
typedef struct	race_type		RACE_TYPE;
typedef struct	repairshop_data		REPAIR_DATA;
typedef struct	reserve_data		RESERVE_DATA;
typedef struct	time_info_data		TIME_INFO_DATA;
typedef struct	hour_min_sec		HOUR_MIN_SEC;
typedef struct	weather_data		WEATHER_DATA;
typedef struct	neighbor_data		NEIGHBOR_DATA; /* FB */
typedef struct  bounty_data             BOUNTY_DATA;
typedef struct  planet_data		PLANET_DATA;
typedef struct  guard_data		GUARD_DATA;
typedef struct  space_data              SPACE_DATA;
typedef	struct	clan_data		CLAN_DATA;
typedef struct  council_data 		COUNCIL_DATA;
typedef struct  ship_data               SHIP_DATA;
typedef struct  missile_data            MISSILE_DATA;
typedef struct	mob_prog_data		MPROG_DATA;
typedef struct	mob_prog_act_list	MPROG_ACT_LIST;
typedef	struct	editor_data		EDITOR_DATA;
typedef struct	timer_data		TIMER;
typedef struct  godlist_data		GOD_DATA;
typedef struct	system_data		SYSTEM_DATA;
typedef	struct	smaug_affect		SMAUG_AFF;
typedef struct  who_data                WHO_DATA;
typedef	struct	skill_type		SKILLTYPE;
typedef	struct	social_type		SOCIALTYPE;
typedef	struct	xsocial_type	XSOCIALTYPE;

typedef	struct	cmd_type		CMDTYPE;
typedef	struct	killed_data		KILLED_DATA;
typedef struct	wizent			WIZENT;
typedef struct  ignore_data		IGNORE_DATA;
typedef struct	extended_bitvector	EXT_BV;
typedef	struct	rank_data		RANK_DATA;

typedef struct  ship_prototype_data     SHIP_PROTOTYPE;

/*
 * Function types.
 */
typedef	void	DO_FUN		args( ( CHAR_DATA *ch, char *argument ) );
typedef bool	SPEC_FUN	args( ( CHAR_DATA *ch ) );
typedef ch_ret	SPELL_FUN	args( ( int sn, int level, CHAR_DATA *ch, void *vo ) );

#define DUR_CONV	23.333333333333333333333333
#define HIDDEN_TILDE	'*'

#define BV00		(1 <<  0)
#define BV01		(1 <<  1)
#define BV02		(1 <<  2)
#define BV03		(1 <<  3)
#define BV04		(1 <<  4)
#define BV05		(1 <<  5)
#define BV06		(1 <<  6)
#define BV07		(1 <<  7)
#define BV08		(1 <<  8)
#define BV09		(1 <<  9)
#define BV10		(1 << 10)
#define BV11		(1 << 11)
#define BV12		(1 << 12)
#define BV13		(1 << 13)
#define BV14		(1 << 14)
#define BV15		(1 << 15)
#define BV16		(1 << 16)
#define BV17		(1 << 17)
#define BV18		(1 << 18)
#define BV19		(1 << 19)
#define BV20		(1 << 20)
#define BV21		(1 << 21)
#define BV22		(1 << 22)
#define BV23		(1 << 23)
#define BV24		(1 << 24)
#define BV25		(1 << 25)
#define BV26		(1 << 26)
#define BV27		(1 << 27)
#define BV28		(1 << 28)
#define BV29		(1 << 29)
#define BV30		(1 << 30)
#define BV31		(1 << 31)
/* 32 USED! DO NOT ADD MORE! SB */

/*
 * String and memory management parameters.
 */
#define MAX_KEY_HASH		 2048
#define MAX_STRING_LENGTH	 4096  /* buf */
#define MAX_INPUT_LENGTH	 1024  /* arg */
#define MAX_INBUF_SIZE		 1024

#define HASHSTR			 /* use string hashing */

#define	MAX_LAYERS		 8	/* maximum clothing layers */
#define MAX_NEST	       100	/* maximum container nesting */

#define MAX_KILLTRACK		20	/* track mob vnums killed */

/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_EXP_WORTH	       500000
#define MIN_EXP_WORTH		   25

#define MAX_REXITS		   20	/* Maximum exits allowed in 1 room */
#define MAX_SKILL		  276
#define MAX_ABILITY		    8
#define MAX_CLAN		   50
#define MAX_PLANET		  100
#define MAX_SHIP                 300
#define MAX_BOUNTY                255
#define MAX_RANK                  9

#define	MAX_HERB		   20
#define	MAX_DISEASE		   20

#define MAX_RACE                   25
#define MAX_NPC_RACE		   91

#define MAX_WHERE_NAME             29

extern int MAX_PC_RACE;

#define MAX_LEVEL	200
#define LEVEL_HERO		   (MAX_LEVEL - 15)
#define LEVEL_IMMORTAL		   (MAX_LEVEL - 15)
#define LEVEL_SUPREME		   (MAX_LEVEL - 15)
#define LEVEL_INFINITE		   (MAX_LEVEL - 15)
#define LEVEL_ETERNAL		   (MAX_LEVEL - 15)
#define LEVEL_IMPLEMENTOR	   (MAX_LEVEL - 15)
#define LEVEL_SUB_IMPLEM	   (MAX_LEVEL - 15)
#define LEVEL_ASCENDANT		   (MAX_LEVEL - 15)
#define LEVEL_GREATER		   (MAX_LEVEL - 15)
#define LEVEL_GOD		   (MAX_LEVEL - 15)
#define LEVEL_LESSER		   (MAX_LEVEL - 15)
#define LEVEL_TRUEIMM		   (MAX_LEVEL - 15)
#define LEVEL_DEMI		   (MAX_LEVEL - 15)
#define LEVEL_SAVIOR		   (MAX_LEVEL - 15)
#define LEVEL_CREATOR		   (MAX_LEVEL - 15)
#define LEVEL_ACOLYTE		   (MAX_LEVEL - 15)
#define LEVEL_NEOPHYTE		   (MAX_LEVEL - 15)
#define LEVEL_AVATAR		   (MAX_LEVEL - 15)

#include "restore.h"
#include "slay.h"
#include "homes.h"
#include "defines.h"

#define LEVEL_LOG		    LEVEL_LESSER
#define LEVEL_HIGOD		    LEVEL_GOD

/* This is to tell if act uses uppercasestring or not --Shaddai */
bool DONT_UPPER;

#define PULSE_PER_SECOND	    4
#define PULSE_MINUTE              ( 60 * PULSE_PER_SECOND)
#define PULSE_VIOLENCE		  (  3 * PULSE_PER_SECOND)
#define PULSE_MOBILE		  (  4 * PULSE_PER_SECOND)
#define PULSE_TICK		  ( 70 * PULSE_PER_SECOND)
#define PULSE_AREA		  ( 60 * PULSE_PER_SECOND)
#define PULSE_AUCTION             ( 10 * PULSE_PER_SECOND)
#define PULSE_SPACE               ( 10 * PULSE_PER_SECOND)
#define PULSE_TAXES               ( 60 * PULSE_MINUTE)

/* 
 * Stuff for area versions --Shaddai
 */
int     area_version;
#define HAS_SPELL_INDEX     -1
#define AREA_VERSION_WRITE 1

/*
 * Command logging types.
 */
typedef enum
{
  LOG_NORMAL, LOG_ALWAYS, LOG_NEVER, LOG_PROGBUG, LOG_BUILD, LOG_HIGH, LOG_COMM, 
  LOG_WARN, LOG_ALL
} log_types;


/*
 * Return types for move_char, damage, greet_trigger, etc, etc
 * Added by Thoric to get rid of bugs
 */
typedef enum
{
  rNONE, rCHAR_DIED, rVICT_DIED, rBOTH_DIED, rCHAR_QUIT, rVICT_QUIT,
  rBOTH_QUIT, rSPELL_FAILED, rOBJ_SCRAPPED, rOBJ_EATEN, rOBJ_EXPIRED,
  rOBJ_TIMER, rOBJ_SACCED, rOBJ_QUAFFED, rOBJ_USED, rOBJ_EXTRACTED,
  rOBJ_DRUNK, rCHAR_IMMUNE, rVICT_IMMUNE, rCHAR_AND_OBJ_EXTRACTED = 128,
  rERROR, rSTOP = 255 /*rSTOP added 01/21/01 by Drraagh */
} ret_types;

/* Echo types for echo_to_all */
#define ECHOTAR_ALL	0
#define ECHOTAR_PC	1
#define ECHOTAR_IMM	2

/* defines for new do_who */
#define WT_MORTAL 0
#define WT_IMM    2
#define WT_AVATAR 1
#define WT_NEWBIE 3
#define WT_GROUPED	4
#define WT_GROUPWHO	5


/*
 * Defines for extended bitvectors
 */
#ifndef INTBITS
  #define INTBITS	32
#endif
#define XBM		31	/* extended bitmask   ( INTBITS - 1 )	*/
#define RSV		5	/* right-shift value  ( sqrt(XBM+1) )	*/
#define XBI		4	/* integers in an extended bitvector	*/
#define MAX_BITS	XBI * INTBITS
/*
 * Structure for extended bitvectors -- Thoric
 */
struct extended_bitvector
{
    int		bits[XBI];
};

#include "copyover.h"

/* short cut crash bug fix provided by gfinello@mail.karmanet.it*/
typedef enum {
   relMSET_ON, relOSET_ON
} relation_type;

typedef struct rel_data REL_DATA; 
struct rel_data {
   void *Actor;
   void *Subject;
   REL_DATA  *next;
   REL_DATA  *prev;
   relation_type Type;
};

/*
 * do_who output structure -- Narn
 */ 
struct who_data
{
  WHO_DATA *prev;
  WHO_DATA *next;
  char *text;
  int  type;
};

/*
 * Player watch data structure  --Gorog
 */
struct  watch_data
{
    WATCH_DATA *        next;
    WATCH_DATA *        prev;
    sh_int              imm_level;
    char *              imm_name;        /* imm doing the watching */
    char *              target_name;     /* player or command being watched   */
    char *              player_site;     /* site being watched     */
};

/*
 * Nuisance structure
 */

#define MAX_NUISANCE_STAGE 10	/* How many nuisance stages */
struct nuisance_data 
{
    long int            time;     /* The time nuisance flag was set */
    long int            max_time; /* Time for max penalties */
    int                 flags;    /* Stage of nuisance */
    int                 power;    /* Power of nuisance */
};

/*
 * Ban Types --- Shaddai
 */
#define BAN_SITE        1
#define BAN_CLASS       2
#define BAN_RACE        3
#define BAN_WARN        -1

/*
 * Site ban structure.
 */
struct	ban_data
{
    BAN_DATA *next;
    BAN_DATA *prev;
    char     *name;     /* Name of site/class/race banned */
    char     *user;     /* Name of user from site */
    char     *note;     /* Why it was banned */
    char     *ban_by;   /* Who banned this site */
    char     *ban_time; /* Time it was banned */
    int      flag;      /* Class or Race number */
    int      unban_date;/* When ban expires */
    sh_int   duration;  /* How long it is banned for */
    sh_int   level;     /* Level that is banned */
    bool     warn;      /* Echo on warn channel */
    bool     prefix;    /* Use of *site */
    bool     suffix;    /* Use of site* */
};

/*
 * Yeesh.. remind us of the old MERC ban structure? :)
 */
struct	reserve_data
{
    RESERVE_DATA *next;
    RESERVE_DATA *prev;
    char *name;
};

/*
 * Time and weather stuff.
 */
typedef enum
{
  SUN_DARK, SUN_RISE, SUN_LIGHT, SUN_SET
} sun_positions;

typedef enum
{
  SKY_CLOUDLESS, SKY_CLOUDY, SKY_RAINING, SKY_LIGHTNING
} sky_conditions;

struct	time_info_data
{
    int		hour;
    int		day;
    int		month;
    int		year;
    int		sunlight;
};

struct hour_min_sec
{
  int hour;
  int min;
  int sec;
  int manual;
};

/* Define maximum number of climate settings - FB */
#define MAX_CLIMATE 5

struct	weather_data
{
    int 		temp;		/* temperature */
    int			precip;		/* precipitation */
    int			wind;		/* umm... wind */
    int			temp_vector;	/* vectors controlling */
    int			precip_vector;	/* rate of change */
    int			wind_vector;
    int			climate_temp;	/* climate of the area */
    int			climate_precip;
    int			climate_wind;
    NEIGHBOR_DATA *	first_neighbor;	/* areas which affect weather sys */
    NEIGHBOR_DATA *	last_neighbor;
    char *		echo;		/* echo string */
    int			echo_color;	/* color for the echo */
};

struct neighbor_data
{
    NEIGHBOR_DATA *next;
    NEIGHBOR_DATA *prev;
    char *name;
    AREA_DATA *address;
};

/*
 * Structure used to build wizlist
 */
struct	wizent
{
    WIZENT *		next;
    WIZENT *		last;
    char *		name;
    sh_int		level;
};

typedef enum
{
	CON_PLAYING,			CON_GET_NAME,			CON_GET_OLD_PASSWORD,
	CON_CONFIRM_NEW_NAME,	CON_GET_NEW_PASSWORD,	CON_CONFIRM_NEW_PASSWORD,
	CON_GET_WANT_RIPANSI,	CON_GET_NEW_SEX,		CON_READ_MOTD,
	CON_GET_NEW_RACE,		CON_GET_EMULATION,		CON_EDITING,			
	CON_TITLE,				CON_PRESS_ENTER,		CON_WAIT_1,
	CON_WAIT_2,				CON_WAIT_3,				CON_ACCEPTED,
	CON_READ_IMOTD,			CON_GET_MSP,            CON_GET_NEW_CLASS,
	CON_GET_NEW_HAIR,		CON_GET_NEW_EYE,		CON_GET_NEW_BUILD,
	CON_GET_NEW_COMPLEX,	CON_GET_CHILD_BOOL,
	CON_COPYOVER_RECOVER
} connection_types;

/*
 * Character substates
 */
typedef enum
{
  SUB_NONE, SUB_PAUSE, SUB_PERSONAL_DESC, SUB_OBJ_SHORT, SUB_OBJ_LONG,
  SUB_OBJ_EXTRA, SUB_MOB_LONG, SUB_MOB_DESC,SUB_BAN_DESC, SUB_ROOM_DESC, SUB_ROOM_EXTRA,
  SUB_ROOM_EXIT_DESC, SUB_WRITING_NOTE, SUB_MPROG_EDIT, SUB_HELP_EDIT,
  SUB_PERSONAL_BIO, SUB_REPEATCMD, SUB_RESTRICTED,
  SUB_SLAYCMSG, SUB_SLAYVMSG, SUB_SLAYRMSG,SUB_SHIPDESC,
  /* timer types ONLY below this point */
  SUB_TIMER_DO_ABORT = 128, SUB_TIMER_CANT_ABORT
} char_substates;

struct rank_data
{
	char * name;
	int place;
	int wage;
};

/*
 * Descriptor (channel) structure.
 */
struct	descriptor_data
{
    DESCRIPTOR_DATA *	next;
    DESCRIPTOR_DATA *	prev;
    DESCRIPTOR_DATA *	snoop_by;
    CHAR_DATA *		character;
    CHAR_DATA *		original;
    char *		host;
    char *              hostip;
    int			port;
    int			descriptor;
    sh_int		connected;
    sh_int		idle;
    sh_int		lines;
    sh_int		scrlen;
    bool		fcommand;
    char		inbuf		[MAX_INBUF_SIZE];
    char		incomm		[MAX_INPUT_LENGTH];
    char		inlast		[MAX_INPUT_LENGTH];
    int			repeat;
    char *		outbuf;
    unsigned long	outsize;
    int			outtop;
    char *		pagebuf;
    unsigned long	pagesize;
    int			pagetop;
    char *		pagepoint;
    char		pagecmd;
    char		pagecolor;
    int			auth_inc;
    int			auth_state;
    char		abuf[ 256 ];
    int			auth_fd;
    char *		user;
    int 		atimes;
    int			newstate;
    unsigned char	prevcolor;
};



/*
 * Attribute bonus structures.
 */
struct	str_app_type
{
    sh_int	tohit;
    sh_int	todam;
    sh_int	carry;
    sh_int	wield;
};

struct	int_app_type
{
    sh_int	learn;
};

struct	wis_app_type
{
    sh_int	practice;
};

struct	dex_app_type
{
    sh_int	defensive;
};

struct	con_app_type
{
    sh_int	hitp;
    sh_int	shock;
};

struct	cha_app_type
{
    sh_int	charm;
};

struct  lck_app_type
{
    sh_int	luck;
};

/* ability classes */

#define FORCE_ABILITY		-2
#define ABILITY_NONE		-1
#define COMBAT_ABILITY 		0
#define PILOTING_ABILITY	1
#define ENGINEERING_ABILITY	2
#define HUNTING_ABILITY		3
#define SMUGGLING_ABILITY	4
#define DIPLOMACY_ABILITY	5
#define PIRACY_ABILITY		6
#define MEDIC_ABILITY		7


/* the races */
typedef enum {
  RACE_HUMAN, RACE_WOOKIEE, RACE_TWI_LEK, RACE_RODIAN, RACE_HUTT, RACE_MON_CALAMARI, 
  RACE_NOGHRI, RACE_GAMORREAN, RACE_JAWA, RACE_ADARIAN, RACE_EWOK, 
  RACE_VERPINE, RACE_DEFEL, RACE_TRANDOSHAN, RACE_CHADRA_FAN, RACE_QUARREN, RACE_DUINUOGWUIN,
  RACE_GOTAL, RACE_DEVARONIAN, RACE_ITHORIAN, RACE_R20, RACE_R21, RACE_R22, RACE_R23, RACE_R24,
  RACE_R25, RACE_R26, RACE_R27, RACE_R28, RACE_R29, RACE_R30
} race_types;

/* npc races */
#define	RACE_DRAGON	    31

/*
 * Languages -- Altrag
 */
#define SCRAMBLE

#define LANG_UNKNOWN     0  /* Anything that doesnt fit a category */
#define LANG_COMMON      BV00  /* Human base language */
#define LANG_WOOKIEE     BV01  
#define LANG_TWI_LEK     BV02  
#define LANG_RODIAN      BV03  
#define LANG_HUTT        BV04  
#define LANG_MON_CALAMARI   BV05 
#define LANG_NOGHRI      BV06 
#define LANG_GAMORREAN   BV07 
#define LANG_JAWA        BV08 
#define LANG_ADARIAN	 BV09 
#define LANG_EWOK        BV10 
#define LANG_VERPINE	 BV11
#define LANG_DEFEL       BV12
#define LANG_TRANDOSHAN  BV13
#define LANG_CHADRA_FAN  BV14
#define LANG_QUARREN     BV15
#define LANG_DUINUOGWUIN BV16
#define LANG_GOTAL       BV17
#define LANG_DEVARONIAN  BV18
#define LANG_ITHORIAN    BV19
#define LANG_BOTHAN		 BV20
#define LANG_ASKANI		 BV21
#define LANG_R22		 BV22
#define LANG_R23		 BV23
#define LANG_R24		 BV24
#define LANG_R25		 BV25
#define LANG_DROID       BV26 
#define LANG_SPIRITUAL   BV27 
#define LANG_MAGICAL     BV28 
#define LANG_GOD         BV29 
#define LANG_ANCIENT     BV30
#define LANG_CLAN		 BV31	



#define VALID_LANGS (	LANG_COMMON | LANG_WOOKIEE | LANG_TWI_LEK | LANG_RODIAN | \
						LANG_HUTT | LANG_MON_CALAMARI | LANG_NOGHRI | LANG_GAMORREAN | \
						LANG_JAWA | LANG_ADARIAN | LANG_EWOK | LANG_VERPINE | \
						LANG_DEFEL | LANG_TRANDOSHAN | LANG_CHADRA_FAN | LANG_QUARREN | \
						LANG_DUINUOGWUIN | LANG_GOTAL | LANG_DEVARONIAN | LANG_ITHORIAN | \
						LANG_BOTHAN | LANG_ASKANI )
						

/*  26 Languages */

/*
 * TO types for act.
 */
typedef enum { TO_ROOM, TO_NOTVICT, TO_VICT, TO_CHAR, TO_CANSEE } to_types;

/*
 * Real action "TYPES" for act.
 */
#define AT_BLACK	    0
#define AT_BLOOD	    1
#define AT_DGREEN           2
#define AT_ORANGE	    3
#define AT_DBLUE	    4
#define AT_PURPLE	    5
#define AT_CYAN	  	    6
#define AT_GREY		    7
#define AT_DGREY	    8
#define AT_RED		    9
#define AT_GREEN	   10
#define AT_YELLOW	   11
#define AT_BLUE		   12
#define AT_PINK		   13
#define AT_LBLUE	   14
#define AT_WHITE	   15
#define AT_BLINK	   16
#define AT_PLAIN	   AT_GREY
#define AT_ACTION	   AT_GREY
#define AT_SAY		   AT_LBLUE
#define AT_GOSSIP	   AT_LBLUE
#define AT_YELL	           AT_WHITE
#define AT_TELL		   AT_BLUE
#define AT_WHISPER	   AT_WHITE
#define AT_HIT		   AT_WHITE
#define AT_HITME	   AT_YELLOW
#define AT_OOC             AT_YELLOW
#define AT_IMMORT	   AT_WHITE
#define AT_AVATAR	   AT_BLUE
#define AT_HURT		   AT_RED
#define AT_FALLING	   AT_WHITE + AT_BLINK
#define AT_DANGER	   AT_RED + AT_BLINK
#define AT_MAGIC	   AT_BLUE
#define AT_CONSIDER	   AT_GREY
#define AT_REPORT	   AT_GREY
#define AT_POISON	   AT_GREEN
#define AT_SOCIAL	   AT_CYAN
#define AT_DYING	   AT_YELLOW
#define AT_DEAD		   AT_RED
#define AT_SKILL	   AT_GREEN
#define AT_CARNAGE	   AT_BLOOD
#define AT_DAMAGE	   AT_WHITE
#define AT_FLEE		   AT_YELLOW
#define AT_RMNAME	   AT_WHITE
#define AT_RMDESC	   AT_GREY
#define AT_OBJECT	   AT_GREEN
#define AT_PERSON	   AT_PINK
#define AT_LIST		   AT_BLUE
#define AT_BYE		   AT_GREEN
#define AT_GOLD		   AT_YELLOW
#define AT_GTELL	   AT_BLUE
#define AT_NOTE		   AT_GREEN
#define AT_HUNGRY	   AT_ORANGE
#define AT_THIRSTY	   AT_BLUE
#define	AT_FIRE		   AT_RED
#define AT_SOBER	   AT_WHITE
#define AT_WEAROFF	   AT_YELLOW
#define AT_EXITS	   AT_WHITE
#define AT_SCORE	   AT_LBLUE
#define AT_RESET	   AT_DGREEN
#define AT_LOG		   AT_PURPLE
#define AT_DIEMSG	   AT_WHITE
#define AT_WARTALK         AT_RED
#define AT_SHIP            AT_PINK
#define AT_CLAN            AT_PINK
#define AT_DIVIDER	   AT_PLAIN
#define AT_IGNORE	   AT_GREEN
#define AT_INFO			AT_RED
#define AT_XSOCIAL	   AT_CYAN

#define INIT_WEAPON_CONDITION    12
#define MAX_ITEM_IMPACT		 30

/*
 * Help table types.
 */
struct	help_data
{
    HELP_DATA *	next;
    HELP_DATA * prev;
    sh_int	level;
    char *	keyword;
    char *	text;
};

/*
 * Shop types.
 */
#define MAX_TRADE	 5

struct	shop_data
{
    SHOP_DATA *	next;			/* Next shop in list		*/
    SHOP_DATA * prev;			/* Previous shop in list	*/
    int		keeper;			/* Vnum of shop keeper mob	*/
    sh_int	buy_type [MAX_TRADE];	/* Item types shop will buy	*/
    sh_int	profit_buy;		/* Cost multiplier for buying	*/
    sh_int	profit_sell;		/* Cost multiplier for selling	*/
    sh_int	open_hour;		/* First opening hour		*/
    sh_int	close_hour;		/* First closing hour		*/
};

#define MAX_FIX		3
#define SHOP_FIX	1
#define SHOP_RECHARGE	2

struct	repairshop_data
{
    REPAIR_DATA * next;			/* Next shop in list		*/
    REPAIR_DATA * prev;			/* Previous shop in list	*/
    int		  keeper;		/* Vnum of shop keeper mob	*/
    sh_int	  fix_type [MAX_FIX];	/* Item types shop will fix	*/
    sh_int	  profit_fix;		/* Cost multiplier for fixing	*/
    sh_int	  shop_type;		/* Repair shop type		*/
    sh_int	  open_hour;		/* First opening hour		*/
    sh_int	  close_hour;		/* First closing hour		*/
};


/* Mob program structures */
struct  act_prog_data
{
    struct act_prog_data *next;
    void *vo;
};

struct	mob_prog_act_list
{
    MPROG_ACT_LIST * next;
    char *	     buf;
    CHAR_DATA *      ch;
    OBJ_DATA *	     obj;
    void *	     vo;
};

struct	mob_prog_data
{
    MPROG_DATA * next;
    int		 type;
    bool	 triggered;
    int		 resetdelay;
    char *	 arglist;
    char *	 comlist;
};

bool	MOBtrigger;

/* race dedicated stuff */
struct	race_type
{
    char 		race_name	[16];	/* Race name			*/
	int			affected;		/* Default affect bitvectors	*/
	sh_int		str_plus;		/* Str bonus/penalty		*/
    sh_int		dex_plus;		/* Dex      "			*/
    sh_int		wis_plus;		/* Wis      "			*/
    sh_int		int_plus;		/* Int      "			*/
    sh_int		con_plus;		/* Con      "			*/
    sh_int		cha_plus;		/* Cha      "			*/
    sh_int		lck_plus;		/* Lck 	    "			*/
	sh_int		ability[MAX_ABILITY]; /* Ability bonus/penalty */
    sh_int      resist;
    sh_int      suscept;
    int			class_restriction;	/* Flags for illegal classes	*/
    int         language;               /* Default racial language      */
	int         nospeak;			/* Flags for illegal Speaking Languages */
    sh_int      ac_plus;
    sh_int      height;
    sh_int      weight;
    sh_int      hunger_mod;
    sh_int      thirst_mod;
	int			death_age;
	sh_int		consent;
};

typedef enum { CLAN_PLAIN, CLAN_CRIME, CLAN_GUILD } clan_types;
typedef enum {SHIP_DOCKED, SHIP_READY, SHIP_BUSY, SHIP_BUSY_2, SHIP_BUSY_3, SHIP_REFUEL,
              SHIP_LAUNCH, SHIP_LAUNCH_2, SHIP_LAND, SHIP_LAND_2, SHIP_HYPERSPACE, SHIP_DISABLED, 
			  SHIP_FLYING, SHIP_SHIP2SHIP, SHIP_SHIP2SHIP_2} ship_states;
typedef enum {MISSILE_READY, MISSILE_FIRED, MISSILE_RELOAD, MISSILE_RELOAD_2, MISSILE_DAMAGED} missile_states;
typedef enum {CONCUSSION_MISSILE, PROTON_TORPEDO, HEAVY_ROCKET, HEAVY_BOMB} missile_types;

typedef enum {FIGHTER_SHIP, MIDSIZE_SHIP, CAPITAL_SHIP, SHIP_PLATFORM } ship_classes;

#define LASER_DAMAGED    -1
#define LASER_READY       0

struct space_data
{
    SPACE_DATA * next;
    SPACE_DATA * prev;
    SHIP_DATA  * first_ship;
    SHIP_DATA  * last_ship;
    MISSILE_DATA  * first_missile;
    MISSILE_DATA  * last_missile;
    PLANET_DATA * first_planet;
    PLANET_DATA * last_planet;
    char	*	filename;
    char	*	name;
    char	*	star1;
    char	*	star2;
    char	*	planet1;
    char	*	planet2;
    char	*	planet3;
    char	*	location1a;
    char	*	location2a;
	char	*	location3a;
    char	*	location1b;
    char	*	location2b;
    char	*	location3b;
    char	*	location1c;
    char	*	location2c;
    char	*	location3c;
    int			xpos;
    int			ypos;
    int			s1x;
    int			s1y;
    int			s1z;
    int			s2x;
    int			s2y;
    int			s2z;
    int			doc1a;
    int			doc2a;
    int			doc3a;
    int			doc1b;
    int			doc2b;
    int			doc3b;
    int			doc1c;
    int			doc2c;
    int			doc3c;
    int			p1x;
    int			p1y;
    int			p1z;
    int			p2x;
    int			p2y;
    int			p2z;
    int			p3x;
    int			p3y;    
    int			p3z;
	int			flags;
	int			fcz;
	CLAN_DATA *	governed_by;
	int			controls;
	int			soldiers;
	int			alertstatus;
	sh_int		code;
};

#define SYSTEM_SECRET  BV00

struct  bounty_data
{
    BOUNTY_DATA * next;
    BOUNTY_DATA * prev;
    char * 	  target;
    long int      amount;
};

struct guard_data
{
    GUARD_DATA * next;
    GUARD_DATA * prev;
    GUARD_DATA * next_on_planet;
    GUARD_DATA * prev_on_planet;
    PLANET_DATA * planet;
};

struct  planet_data
{
    PLANET_DATA    * next;
    PLANET_DATA    * prev;
    PLANET_DATA    * next_in_system;
    PLANET_DATA    * prev_in_system;
    GUARD_DATA     * first_guard;
    GUARD_DATA     * last_guard;
    SPACE_DATA     * starsystem;
    AREA_DATA      * first_area;
    AREA_DATA      * last_area;
    char           * name;
    char           * filename;
    long             base_value;
    CLAN_DATA      * governed_by;
    int 	     population;
    bool 	     flags;
    float            pop_support;
	int			x;
	int			y;
	int			z;
	int			controls;
	int			shields;
	int			barracks;
	int			code;
};

#define PLANET_NOCAPTURE  BV00
#define PLANET_NEUTRAL  BV01
#define PLANET_UNINHABITED  BV02
#define PLANET_PSHIELD  BV03

struct	clan_data
{
    CLAN_DATA * next;		/* next clan in list			*/
    CLAN_DATA * prev;		/* previous clan in list		*/
	RANK_DATA ranks[MAX_RANK];
    char *	filename;	/* Clan filename			*/
    char *	name;		/* Clan name				*/
    char *	leader;		/* Head clan leader			*/
    char *	number1;	/* First officer			*/
    char *	number2;	/* Second officer			*/
	char *	memberslist; /* Self Explanitory, Members list of the clan */
    char *	atwar;		/* Clan name				*/
    sh_int	clan_type;	/* See clan type defines		*/
    sh_int	members;	/* Number of clan members		*/
    int		storeroom;	/* Vnum of clan's store room		*/
    long int    funds;
    int         spacecraft;
    int		vehicles;
    int         jail;
	int         enlistroom;

    char *	description;	/* A brief description of the clan	*/
};

struct	council_data
{
    COUNCIL_DATA * next;	/* next council in list			*/
    COUNCIL_DATA * prev;	/* previous council in list		*/
    char *	filename;	/* Council filename			*/
    char *	name;		/* Council name				*/
    char *	description;	/* A brief description of the council	*/
    char *	head;		/* Council head 			*/
    char *	head2;          /* Council co-head                      */
    char *	powers;		/* Council powers			*/
    sh_int	members;	/* Number of council members		*/
};

#define SHIP_CANFLYINDOORS  BV00
#define SHIP_BAYOPEN		BV01
#define SHIP_AUTOPILOT		BV02
#define SHIP_MASKED			BV03
#define SHIP_MOB			BV04
#define SHIP_SIMULATOR			BV05

struct ship_prototype_data
{
    SHIP_PROTOTYPE * next;
    SHIP_PROTOTYPE * prev;
    char *      filename;
    char *      name; /* Must be unique, maybe should be int id. As its how the ship stats get set. */
    char *      description;
    sh_int      class;
    sh_int      hyperspeed;
    sh_int      realspeed;
    sh_int      maxmissiles;
    sh_int      lasers;
    sh_int      tractorbeam;
    sh_int      manuever;
    int         maxenergy;
    int         maxshield;
    int         maxhull;
    sh_int      maxchaff;
	/* Unknown Regions Additions - Gavin - Mar 28 2001 */
	sh_int		maxbombs;
	sh_int      comm;
    sh_int      sensor;
    sh_int      astro_array;
	sh_int		ion;
	sh_int		count;
};

struct ship_data
{
    SHIP_DATA * next;
    SHIP_DATA * prev;
    SHIP_DATA * next_in_starsystem;
    SHIP_DATA * prev_in_starsystem;
    SHIP_DATA * next_in_room;
    SHIP_DATA * prev_in_room;
    ROOM_INDEX_DATA *in_room;
    SPACE_DATA * starsystem;
	SHIP_PROTOTYPE * prototype;
    char *      filename;
    char *      name;
	char *      origname;
    char *      home;
    char *      description;
    char *      owner;
    char *      pilot;
    char *      copilot;
    char *      dest;
    sh_int      class;
    int		sim_vnum;
    int         hyperdistance;
    sh_int		currspeed;
    sh_int      shipstate;
    sh_int      statet0;
    sh_int      statet1;
    sh_int      statet2;
	sh_int      statet3;
    sh_int      missiletype;
    sh_int      missilestate;
    sh_int      missiles;
    bool        bayopen;
    bool        hatchopen;
    bool 	autorecharge;
    bool        autotrack;
    bool 	autospeed;
    float       vx, vy, vz;
    float       hx, hy, hz;
    float       jx, jy, jz;
    int         energy;
    int         shield;
    int         hull;
    int         cockpit;
    int         turret1;
    int         turret2;
    int         location;
    int         lastdoc;
    int         docked2;
	int         shipyard;
    int         entrance;
    int         hanger;
    int         engineroom;
    int         firstroom;
    int         lastroom;
    int         navseat;
    int         pilotseat;
    int         coseat;
    int         gunseat;
    long        collision;
    SHIP_DATA  *target0;
    SHIP_DATA  *target1;
    SHIP_DATA  *target2;
    SPACE_DATA *currjump;
    sh_int      chaff;
    bool        chaff_released;
    bool        autopilot;
    sh_int		repulsor;
	char *		color;
	int			flags;
	char *		allowdock;
	int			code;
	sh_int		model;

    sh_int      comm;
    sh_int      sensor;
    sh_int      astro_array;
    sh_int      hyperspeed;
	sh_int      realspeed;
	sh_int      lasers;
	sh_int      laserdamage;
    sh_int      tractorbeam;
    sh_int      manuever;
	sh_int		ion;
	sh_int		alarm;
	sh_int		bombs;

	sh_int      maxmissiles;
	int         maxenergy;
    int         maxshield;
    int         maxhull;
    sh_int      maxchaff;
	sh_int		maxbombs;
};

struct missile_data
{
    MISSILE_DATA * next;
    MISSILE_DATA * prev;
    MISSILE_DATA * next_in_starsystem;
    MISSILE_DATA * prev_in_starsystem;
    SPACE_DATA * starsystem;
    SHIP_DATA * target;
    SHIP_DATA * fired_from;
    char      * fired_by;
    sh_int      missiletype;
    sh_int      age;
    int         speed;
    int         mx, my, mz;
};


/*
 * Data structure for notes.
 */
struct	note_data
{
    NOTE_DATA *	next;
    NOTE_DATA * prev;
    char *	sender;
    char *	date;
    char *	to_list;
    char *	subject;
    int         voting;
    char *	yesvotes;
    char *	novotes;
    char *	abstentions;
    char *	text;
};

struct	board_data
{
    BOARD_DATA * next;			/* Next board in list		   */
    BOARD_DATA * prev;			/* Previous board in list	   */
    NOTE_DATA *  first_note;		/* First note on board		   */
    NOTE_DATA *  last_note;		/* Last note on board		   */
    char *	 note_file;		/* Filename to save notes to	   */
    char *	 read_group;		/* Can restrict a board to a       */
    char *	 post_group;		/* council, clan, guild etc        */
    char *	 extra_readers;		/* Can give read rights to players */
    char *       extra_removers;        /* Can give remove rights to players */
    int		 board_obj;		/* Vnum of board object		   */
    sh_int	 num_posts;		/* Number of notes on this board   */
    sh_int	 min_read_level;	/* Minimum level to read a note	   */
    sh_int	 min_post_level;	/* Minimum level to post a note    */
    sh_int	 min_remove_level;	/* Minimum level to remove a note  */
    sh_int	 max_posts;		/* Maximum amount of notes allowed */
    int          type;                  /* Normal board or mail board? */
};


/*
 * An affect.
 */
struct	affect_data
{
    AFFECT_DATA *	next;
    AFFECT_DATA *	prev;
    sh_int		type;
    sh_int		duration;
    sh_int		location;
    int			modifier;
    int			bitvector;
};


/*
 * A SMAUG spell
 */
struct	smaug_affect
{
    SMAUG_AFF *		next;
    char *		duration;
    sh_int		location;
    char *		modifier;
    int			bitvector;
};


/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */
#define MOB_VNUM_GUARD		21
#define	MOB_VNUM_PATROL		23
#define MOB_VNUM_MERCINARY	24
#define MOB_VNUM_DROID		25


/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC		 BV00		/* Auto set for mobs	*/
#define ACT_SENTINEL		 BV01		/* Stays in one room	*/
#define ACT_SCAVENGER		 BV02		/* Picks up objects	*/
#define ACT_IS_SURGEON		 BV03		/* Installs Cybernetics */
#define	ACT_DEALER			 BV04		/* Dealer */
#define ACT_AGGRESSIVE		 BV05		/* Attacks PC's		*/
#define ACT_STAY_AREA		 BV06		/* Won't leave area	*/
#define ACT_WIMPY		 BV07		/* Flees when hurt	*/
#define ACT_PET			 BV08		/* Auto set for pets	*/
#define ACT_TRAIN		 BV09		/* Can train PC's	*/
#define ACT_PRACTICE		 BV10		/* Can practice PC's	*/
#define ACT_IMMORTAL		 BV11		/* Cannot be killed	*/
#define ACT_DEADLY		 BV12		/* Has a deadly poison  */

#define ACT_META_AGGR		 BV14		/* Extremely aggressive */
#define ACT_GUARDIAN		 BV15		/* Protects master	*/
#define ACT_RUNNING		 BV16		/* Hunts quickly	*/
#define ACT_NOWANDER		 BV17		/* Doesn't wander	*/
#define ACT_MOUNTABLE		 BV18		/* Can be mounted	*/
#define ACT_MOUNTED		 BV19		/* Is mounted		*/
#define ACT_SCHOLAR              BV20           /* Can teach languages  */
#define ACT_SECRETIVE		 BV21		/* actions aren't seen	*/

#define ACT_RES13			 BV13		/* FREE ACT FLAG */
#define ACT_RES22			 BV22		/* FREE ACT FLAG */

#define ACT_MOBINVIS		 BV23		/* Like wizinvis	*/
#define ACT_NOASSIST		 BV24		/* Doesn't assist mobs	*/
#define ACT_NOKILL               BV25           /* Mob can't die */
#define ACT_DROID                BV26           /* mob is a droid */
#define ACT_NOCORPSE             BV27     
#define ACT_CITIZEN				BV28		/* mob is a citizen */      
#define ACT_NOQUEST				BV29		/* mob won't be called upon for a quest */      
#define ACT_PROTOTYPE		 BV30		/* A prototype mob	*/
#define ACT_VENDOR			BV31
/* 32 acts */

typedef enum
{
	ACT_STATSHIELD, ACT_DISGUISE, MAX_ACT_FLAG
} new_act_flags;

/* bits for vip flags */

#define VIP_R00					BV00
#define VIP_R01					BV01
#define VIP_R02					BV02
#define VIP_R03					BV03
#define VIP_R04					BV04
#define VIP_R05					BV05
#define VIP_R06					BV06
#define VIP_R07					BV07
#define VIP_R08					BV08
#define VIP_R09		           	BV09
#define VIP_R10					BV10
#define VIP_R11					BV11
#define VIP_R12					BV12
#define VIP_R13					BV13
#define VIP_R14					BV14
#define VIP_R15					BV15
#define VIP_R16                 BV16
#define VIP_R17                 BV17
#define VIP_R18                 BV18
#define VIP_R19                 BV19
#define VIP_R20                 BV20
#define VIP_R21                 BV21
#define VIP_R22                 BV22
#define VIP_R23                 BV23
#define VIP_R24                 BV24
#define VIP_R25                 BV25
#define VIP_R26                 BV26
#define VIP_R27                 BV27
#define VIP_R28                 BV28
#define VIP_R29                 BV29
#define VIP_R30                 BV30
#define VIP_R31                 BV31
/* player wanted bits */

#define WANTED_MON_CALAMARI   	VIP_MON_CALAMARI
#define WANTED_CORUSCANT   	VIP_CORUSCANT
#define WANTED_ADARI   		VIP_ADARI
#define WANTED_RODIA   		VIP_RODIA
#define WANTED_RYLOTH   	VIP_RYLOTH
#define WANTED_GAMORR   	VIP_GAMORR
#define WANTED_TATOOINE   	VIP_TATOOINE
#define WANTED_BYSS   		VIP_BYSS
#define WANTED_NAL_HUTTA   	VIP_NAL_HUTTA
#define WANTED_KASHYYYK   	VIP_KASHYYYK
#define WANTED_HONOGHR   	VIP_HONOGHR
#define WANTED_ENDOR		BV11
#define WANTED_ROCHE		BV12
#define WANTED_AF_EL		BV13
#define WANTED_TRANDOSH		BV14
#define WANTED_CHAD		BV15
#define WANTED_R16                 BV16
#define WANTED_R17                 BV17
#define WANTED_R18                 BV18
#define WANTED_R19                 BV19
#define WANTED_R20                 BV20
#define WANTED_R21                 BV21
#define WANTED_R22                 BV22
#define WANTED_R23                 BV23
#define WANTED_R24                 BV24
#define WANTED_R25                 BV25
#define WANTED_R26                 BV26
#define WANTED_R27                 BV27
#define WANTED_R28                 BV28  
#define WANTED_R29                 BV29  
#define WANTED_R30                 BV30  
#define WANTED_R31                 BV31


/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_NONE                  0 
 
#define AFF_BLIND		  BV00
#define AFF_INVISIBLE		  BV01
#define AFF_DETECT_EVIL		  BV02
#define AFF_DETECT_INVIS	  BV03
#define AFF_DETECT_MAGIC	  BV04
#define AFF_DETECT_HIDDEN	  BV05
#define AFF_WEAKEN		  BV06		
#define AFF_SANCTUARY		  BV07
#define AFF_FAERIE_FIRE		  BV08
#define AFF_INFRARED		  BV09
#define AFF_CURSE		  BV10
#define AFF_FLAMING		  BV11		/* Unused	*/
#define AFF_POISON		  BV12
#define AFF_PROTECT		  BV13
#define AFF_PARALYSIS		  BV14
#define AFF_SNEAK		  BV15
#define AFF_HIDE		  BV16
#define AFF_SLEEP		  BV17
#define AFF_CHARM		  BV18
#define AFF_FLYING		  BV19
#define AFF_PASS_DOOR		  BV20
#define AFF_FLOATING		  BV21
#define AFF_TRUESIGHT		  BV22
#define AFF_DETECTTRAPS		  BV23
#define AFF_SCRYING	          BV24
#define AFF_FIRESHIELD	          BV25
#define AFF_SHOCKSHIELD	          BV26
#define AFF_CONTRACEPTION         BV27        /* Cannot get pregnant - KaVir */
#define AFF_ICESHIELD  		  BV28
#define AFF_POSSESS		  BV29
#define AFF_BERSERK		  BV30
#define AFF_AQUA_BREATH		  BV31
/* 31 aff's (1 left.. :P) */
/* make that none - ugh - time for another field? :P */
/*
 * Resistant Immune Susceptible flags
 */
#define RIS_FIRE		  BV00
#define RIS_COLD		  BV01
#define RIS_ELECTRICITY		  BV02
#define RIS_ENERGY		  BV03
#define RIS_BLUNT		  BV04
#define RIS_PIERCE		  BV05
#define RIS_SLASH		  BV06
#define RIS_ACID		  BV07
#define RIS_POISON		  BV08
#define RIS_DRAIN		  BV09
#define RIS_SLEEP		  BV10
#define RIS_CHARM		  BV11
#define RIS_HOLD		  BV12
#define RIS_NONMAGIC		  BV13
#define RIS_PLUS1		  BV14
#define RIS_PLUS2		  BV15
#define RIS_PLUS3		  BV16
#define RIS_PLUS4		  BV17
#define RIS_PLUS5		  BV18
#define RIS_PLUS6		  BV19
#define RIS_MAGIC		  BV20
#define RIS_PARALYSIS		  BV21
/* 21 RIS's*/

/* 
 * Attack types
 */
#define ATCK_BITE		  BV00
#define ATCK_CLAWS		  BV01
#define ATCK_TAIL		  BV02
#define ATCK_STING		  BV03
#define ATCK_PUNCH		  BV04
#define ATCK_KICK		  BV05
#define ATCK_TRIP		  BV06
#define ATCK_BASH		  BV07
#define ATCK_STUN		  BV08
#define ATCK_GOUGE		  BV09
#define ATCK_BACKSTAB		  BV10
#define ATCK_FEED		  BV11
#define ATCK_DRAIN		  BV12
#define ATCK_FIREBREATH		  BV13
#define ATCK_FROSTBREATH	  BV14
#define ATCK_ACIDBREATH		  BV15
#define ATCK_LIGHTNBREATH	  BV16
#define ATCK_GASBREATH		  BV17
#define ATCK_POISON		  BV18
#define ATCK_NASTYPOISON	  BV19
#define ATCK_GAZE		  BV20
#define ATCK_BLINDNESS		  BV21
#define ATCK_CAUSESERIOUS	  BV22
#define ATCK_EARTHQUAKE		  BV23
#define ATCK_CAUSECRITICAL	  BV24
#define ATCK_CURSE		  BV25
#define ATCK_FLAMESTRIKE	  BV26
#define ATCK_HARM		  BV27
#define ATCK_FIREBALL		  BV28
#define ATCK_COLORSPRAY		  BV29
#define ATCK_WEAKEN		  BV30
#define ATCK_SPIRALBLAST	  BV31
/* 32 USED! DO NOT ADD MORE! SB */

/*
 * Defense types
 */
#define DFND_PARRY		  BV00
#define DFND_DODGE		  BV01
#define DFND_HEAL		  BV02
#define DFND_CURELIGHT		  BV03
#define DFND_CURESERIOUS	  BV04
#define DFND_CURECRITICAL	  BV05
#define DFND_DISPELMAGIC	  BV06
#define DFND_DISPELEVIL		  BV07
#define DFND_SANCTUARY		  BV08
#define DFND_FIRESHIELD		  BV09
#define DFND_SHOCKSHIELD	  BV10
#define DFND_SHIELD		  BV11
#define DFND_BLESS		  BV12
#define DFND_STONESKIN		  BV13
#define DFND_TELEPORT		  BV14
#define DFND_MONSUM1		  BV15
#define DFND_MONSUM2		  BV16
#define DFND_MONSUM3		  BV17
#define DFND_MONSUM4		  BV18
#define DFND_DISARM		  BV19
#define DFND_ICESHIELD 		  BV20
#define DFND_GRIP		  BV21
/* 21 def's */

/* defines the cybernetic parts */
#define CYBER_COMM				BV01
#define CYBER_EYES				BV02
#define CYBER_LEGS				BV03
#define CYBER_CHEST				BV04
#define CYBER_REFLEXES			BV05
#define CYBER_MIND				BV06
#define CYBER_STRENGTH			BV07
#define CYBER_REACTOR			BV08
#define CYBER_STERILE			BV09

/*
 * Body parts
 */
#define PART_HEAD		  BV00
#define PART_ARMS		  BV01
#define PART_LEGS		  BV02
#define PART_HEART		  BV03
#define PART_BRAINS		  BV04
#define PART_GUTS		  BV05
#define PART_HANDS		  BV06
#define PART_FEET		  BV07
#define PART_FINGERS		  BV08
#define PART_EAR		  BV09
#define PART_EYE		  BV10
#define PART_LONG_TONGUE	  BV11
#define PART_EYESTALKS		  BV12
#define PART_TENTACLES		  BV13
#define PART_FINS		  BV14
#define PART_WINGS		  BV15
#define PART_TAIL		  BV16
#define PART_SCALES		  BV17
/* for combat */
#define PART_CLAWS		  BV18
#define PART_FANGS		  BV19
#define PART_HORNS		  BV20
#define PART_TUSKS		  BV21
#define PART_TAILATTACK		  BV22
#define PART_SHARPSCALES	  BV23
#define PART_BEAK		  BV24

#define PART_HAUNCH		  BV25
#define PART_HOOVES		  BV26
#define PART_PAWS		  BV27
#define PART_FORELEGS		  BV28
#define PART_FEATHERS		  BV29

/*
 * Autosave flags
 */
#define SV_DEATH		  BV00
#define SV_KILL			  BV01
#define SV_PASSCHG		  BV02
#define SV_DROP			  BV03
#define SV_PUT			  BV04
#define SV_GIVE			  BV05
#define SV_AUTO			  BV06
#define SV_ZAPDROP		  BV07
#define SV_AUCTION		  BV08
#define SV_GET			  BV09
#define SV_RECEIVE		  BV10
#define SV_IDLE			  BV11
#define SV_BACKUP		  BV12

/*
 * Pipe flags
 */
#define PIPE_TAMPED		  BV01
#define PIPE_LIT		  BV02
#define PIPE_HOT		  BV03
#define PIPE_DIRTY		  BV04
#define PIPE_FILTHY		  BV05
#define PIPE_GOINGOUT		  BV06
#define PIPE_BURNT		  BV07
#define PIPE_FULLOFASH		  BV08

/*
 * Flags for act_string -- Shaddai
 */
#define STRING_NONE               0
#define STRING_IMM                BV01

/*
 * Skill/Spell flags	The minimum BV *MUST* be 11!
 */
#define SF_WATER		  BV11
#define SF_EARTH		  BV12
#define SF_AIR			  BV13
#define SF_ASTRAL		  BV14
#define SF_AREA			  BV15  /* is an area spell		*/
#define SF_DISTANT		  BV16  /* affects something far away	*/
#define SF_REVERSE		  BV17
#define SF_SAVE_HALF_DAMAGE	  BV18  /* save for half damage		*/
#define SF_SAVE_NEGATES		  BV19  /* save negates affect		*/
#define SF_ACCUMULATIVE		  BV20  /* is accumulative		*/
#define SF_RECASTABLE		  BV21  /* can be refreshed		*/
#define SF_NOSCRIBE		  BV22  /* cannot be scribed		*/
#define SF_NOBREW		  BV23  /* cannot be brewed		*/
#define SF_GROUPSPELL		  BV24  /* only affects group members	*/
#define SF_OBJECT		  BV25	/* directed at an object	*/
#define SF_CHARACTER		  BV26  /* directed at a character	*/
#define SF_SECRETSKILL		  BV27	/* hidden unless learned	*/
#define SF_PKSENSITIVE		  BV28	/* much harder for plr vs. plr	*/
#define SF_STOPONFAIL		  BV29	/* stops spell on first failure */

typedef enum { SS_NONE, SS_POISON_DEATH, SS_ROD_WANDS, SS_PARA_PETRI,
	       SS_BREATH, SS_SPELL_STAFF } save_types;

#define ALL_BITS		INT_MAX
#define SDAM_MASK		ALL_BITS & ~(BV00 | BV01 | BV02)
#define SACT_MASK		ALL_BITS & ~(BV03 | BV04 | BV05)
#define SCLA_MASK		ALL_BITS & ~(BV06 | BV07 | BV08)
#define SPOW_MASK		ALL_BITS & ~(BV09 | BV10)

typedef enum { SD_NONE, SD_FIRE, SD_COLD, SD_ELECTRICITY, SD_ENERGY, SD_ACID,
	       SD_POISON, SD_DRAIN } spell_dam_types;

typedef enum { SA_NONE, SA_CREATE, SA_DESTROY, SA_RESIST, SA_SUSCEPT,
	       SA_DIVINATE, SA_OBSCURE, SA_CHANGE } spell_act_types;

typedef enum { SP_NONE, SP_MINOR, SP_GREATER, SP_MAJOR } spell_power_types;

typedef enum { SC_NONE, SC_LUNAR, SC_SOLAR, SC_TRAVEL, SC_SUMMON,
	       SC_LIFE, SC_DEATH, SC_ILLUSION } spell_class_types;

typedef enum { MOB_PATROL = 0, MOB_DESTROYER = 6, MOB_CRUISER = 8, MOB_BATTLESHIP = 10 } mob_ship_models;

typedef enum { SYSTEM_ALLY = 0, SYSTEM_ENEMY = 1, SYSTEM_NEUTRAL =2 } system_status_types;

/*
 * Sex.
 * Used in #MOBILES.
 */
typedef enum { SEX_NEUTRAL, SEX_MALE, SEX_FEMALE } sex_types;

/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_MONEY_ONE	      2
#define OBJ_VNUM_MONEY_SOME	      3

#define OBJ_VNUM_DROID_CORPSE        9
#define OBJ_VNUM_CORPSE_NPC	     10
#define OBJ_VNUM_CORPSE_PC	     11
#define OBJ_VNUM_SCRAPS		     19
#define OBJ_VNUM_SHOPPING_BAG	     25
#define OBJ_VNUM_FIRE		     30

#define OBJ_VNUM_BLACK_POWDER	     33
#define OBJ_VNUM_NOTE		     36

#define OBJ_VNUM_BLASTECH_E11     50
#define OBJ_VNUM_STEAK	75

/* Academy eq */

#define OBJ_VNUM_SCHOOL_DAGGER	  10312 /* Vibro-Blade */
#define OBJ_VNUM_SCHOOL_SWORD	  10313 /* Ration */
#define OBJ_VNUM_SCHOOL_SHIELD	  10310 /* Shield */
#define OBJ_VNUM_SCHOOL_BANNER    10311 /* Light */


/*
 * Item types.
 * Used in #OBJECTS.
 */
typedef enum
{
	ITEM_NONE, ITEM_LIGHT, ITEM_WEAPON, ITEM_TREASURE, ITEM_ARMOR, 
	ITEM_POTION, ITEM_FURNITURE, ITEM_TRASH, ITEM_CONTAINER,
	ITEM_NOTE, ITEM_DRINK_CON, ITEM_KEY, ITEM_FOOD, ITEM_MONEY,
	ITEM_CORPSE_NPC, ITEM_CORPSE_PC, ITEM_FOUNTAIN, ITEM_PILL,
	ITEM_SCRAPS, ITEM_PIPE, ITEM_HERB_CON, ITEM_HERB, ITEM_FIRE,
	ITEM_SOAP, ITEM_SYRINGE, ITEM_AMMO, ITEM_BOLT, 
	ITEM_BUTTON, ITEM_PAPER,  ITEM_SHOVEL, ITEM_SALVE, 
	ITEM_RAWSPICE, ITEM_LENS, ITEM_CRYSTAL, ITEM_DURAPLAST,/*34*/
	ITEM_BATTERY, ITEM_TOOLKIT, ITEM_DURASTEEL, ITEM_OVEN, 
	ITEM_MIRROR,  ITEM_CIRCUIT, ITEM_SUPERCONDUCTOR, ITEM_COMLINK,
	ITEM_MEDPAC, ITEM_FABRIC, ITEM_RARE_METAL, ITEM_THREAD, 
	ITEM_SPICE, ITEM_SMUT, ITEM_DEVICE, ITEM_GRENADE,/*50*/
	ITEM_LANDMINE, ITEM_DROID_CORPSE, ITEM_CHEMICAL, ITEM_TIMBER, 
	ITEM_CONTRACEPTIVE, ITEM_KEYRING 
} item_types;

#define MAX_ITEM_TYPE		     ITEM_KEYRING

/*
 * Syringe Types
 */ 

#define SYRINGE_INFECT_CURE 0

/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW		BV00
#define ITEM_HUM		BV01
#define ITEM_DARK		BV02
#define ITEM_HUTT_SIZE		BV03
#define ITEM_CONTRABAND		BV04
#define ITEM_INVIS		BV05
#define ITEM_MAGIC		BV06
#define ITEM_NODROP		BV07
#define ITEM_BLESS		BV08
#define ITEM_ANTI_GOOD		BV09
#define ITEM_ANTI_EVIL		BV10
#define ITEM_ANTI_NEUTRAL	BV11
#define ITEM_NOREMOVE		BV12
#define ITEM_INVENTORY		BV13
#define ITEM_ANTI_SOLDIER	BV14
#define ITEM_ANTI_THIEF	        BV15
#define ITEM_ANTI_HUNTER	BV16
#define ITEM_ANTI_JEDI  	BV17
#define ITEM_SMALL_SIZE		BV18
#define ITEM_LARGE_SIZE		BV19
#define ITEM_DONATION		BV20
#define ITEM_CLANOBJECT		BV21
#define ITEM_ANTI_CITIZEN	BV22
#define ITEM_ANTI_SITH  	BV23
#define ITEM_ANTI_PILOT	        BV24
#define ITEM_HIDDEN		BV25
#define ITEM_POISONED		BV26
#define ITEM_COVERING		BV27
#define ITEM_DEATHROT		BV28
#define ITEM_BURRIED		BV29	/* item is underground */
#define ITEM_PROTOTYPE		BV30
#define ITEM_HUMAN_SIZE         BV31

/* Magic flags - extra extra_flags for objects that are used in spells */
#define ITEM_RETURNING		BV00
#define ITEM_BACKSTABBER  	BV01
#define ITEM_BANE		BV02
#define ITEM_LOYAL		BV03
#define ITEM_HASTE		BV04
#define ITEM_DRAIN		BV05
#define ITEM_LIGHTNING_BLADE  	BV06

/* Blaster settings - only saves on characters */
#define BLASTER_NORMAL          0
#define BLASTER_HALF		2
#define BLASTER_FULL            5
#define BLASTER_LOW		1	
#define	BLASTER_STUN		3
#define BLASTER_HIGH            4

/* Weapon Types */

#define WEAPON_NONE     	0
#define WEAPON_VIBRO_AXE	1
#define WEAPON_VIBRO_BLADE	2
#define WEAPON_LIGHTSABER	3
#define WEAPON_WHIP		4
#define WEAPON_CLAW		5
#define WEAPON_BLASTER		6
#define WEAPON_BLUDGEON		8
#define WEAPON_BOWCASTER        9
#define WEAPON_FORCE_PIKE	11

/* Lever/dial/switch/button/pullchain flags */
#define TRIG_UP			BV00
#define TRIG_UNLOCK		BV01
#define TRIG_LOCK		BV02
#define TRIG_D_NORTH		BV03
#define TRIG_D_SOUTH		BV04
#define TRIG_D_EAST		BV05
#define TRIG_D_WEST		BV06
#define TRIG_D_UP		BV07
#define TRIG_D_DOWN		BV08
#define TRIG_DOOR		BV09
#define TRIG_CONTAINER		BV10
#define TRIG_OPEN		BV11
#define TRIG_CLOSE		BV12
#define TRIG_PASSAGE		BV13
#define TRIG_OLOAD		BV14
#define TRIG_MLOAD		BV15
#define TRIG_TELEPORT		BV16
#define TRIG_TELEPORTALL	BV17
#define TRIG_TELEPORTPLUS	BV18
#define TRIG_DEATH		BV19
#define TRIG_CAST		BV20
#define TRIG_FAKEBLADE		BV21
#define TRIG_RAND4		BV22
#define TRIG_RAND6		BV23
#define TRIG_TRAPDOOR		BV24
#define TRIG_ANOTHEROOM		BV25
#define TRIG_USEDIAL		BV26
#define TRIG_ABSOLUTEVNUM	BV27
#define TRIG_SHOWROOMDESC	BV28
#define TRIG_AUTORETURN		BV29

#define TELE_SHOWDESC		BV00
#define TELE_TRANSALL		BV01
#define TELE_TRANSALLPLUS	BV02

/* drug types */
#define SPICE_GLITTERSTIM        0
#define SPICE_CARSANUM           1
#define SPICE_RYLL               2
#define SPICE_ANDRIS             3

/* crystal types */
#define GEM_NON_ADEGEN          0
#define GEM_KATHRACITE		1
#define GEM_RELACITE		2
#define GEM_DANITE		3
#define GEM_MEPHITE		4
#define GEM_PONITE		5
#define GEM_ILLUM               6
#define GEM_CORUSCA             7

/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE		BV00
#define ITEM_WEAR_FINGER	BV01
#define ITEM_WEAR_NECK		BV02
#define ITEM_WEAR_BODY		BV03
#define ITEM_WEAR_HEAD		BV04
#define ITEM_WEAR_LEGS		BV05
#define ITEM_WEAR_FEET		BV06
#define ITEM_WEAR_HANDS		BV07
#define ITEM_WEAR_ARMS		BV08
#define ITEM_WEAR_SHIELD	BV09
#define ITEM_WEAR_ABOUT		BV10
#define ITEM_WEAR_WAIST		BV11
#define ITEM_WEAR_WRIST		BV12
#define ITEM_WIELD		BV13
#define ITEM_HOLD		BV14
#define ITEM_DUAL_WIELD		BV15
#define ITEM_WEAR_EARS		BV16
#define ITEM_WEAR_EYES		BV17
#define ITEM_MISSILE_WIELD	BV18
#define ITEM_WEAR_BACK		BV19
#define ITEM_WEAR_FACE		BV20
#define ITEM_WEAR_ANKLE		BV21

/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
typedef enum
{
  APPLY_NONE, APPLY_STR, APPLY_DEX, APPLY_INT, APPLY_WIS, APPLY_CON, APPLY_CHA, APPLY_LCK, 
  APPLY_SEX, APPLY_LEVEL, APPLY_AGE, APPLY_HEIGHT, APPLY_WEIGHT,
  APPLY_HIT, APPLY_MOVE, APPLY_GOLD, APPLY_EXP, APPLY_AC,
  APPLY_HITROLL, APPLY_DAMROLL, 
  APPLY_AFFECT, APPLY_RESISTANT, APPLY_IMMUNE, APPLY_SUSCEPTIBLE,
  APPLY_WEAPONSPELL, APPLY_WEARSPELL, APPLY_REMOVESPELL,
  APPLY_EMOTION, APPLY_MENTALSTATE, APPLY_STRIPSN, APPLY_REMOVE, APPLY_DRUNK, 
  APPLY_CONTRACEPTIVE, MAX_APPLY_TYPE
} apply_types;

#define REVERSE_APPLY		   1000

/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		   BV00
#define CONT_PICKPROOF		   BV01
#define CONT_CLOSED		   BV02
#define CONT_LOCKED		   BV03
#define CONT_EATKEY		   BV04

/*
 * Sitting/Standing/Sleeping/Sitting on/in/at Objects - Xerves
 * Used for furniture (value[2]) in the #OBJECTS Section
 */
#define SIT_ON     BV00
#define SIT_IN     BV01
#define SIT_AT     BV02

#define STAND_ON   BV03
#define STAND_IN   BV04
#define STAND_AT   BV05

#define SLEEP_ON   BV06
#define SLEEP_IN   BV07
#define SLEEP_AT   BV08

#define REST_ON     BV09
#define REST_IN     BV10
#define REST_AT     BV11

/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO		      2
#define ROOM_VNUM_NURSERY		      92079
#define ROOM_VNUM_TEMPLE	  32144
#define ROOM_VNUM_ALTAR		  32144 
#define ROOM_VNUM_SCHOOL	  10300
#define ROOM_AUTH_LAND		  300
#define ROOM_START_IMMORTAL         100
#define ROOM_LIMBO_SHIPYARD          45


/*
 * New bit values for sector types.  Code by Mystaric
 */
 #define BVSECT_INSIDE 			BV00
 #define BVSECT_CITY 			BV01
 #define BVSECT_FIELD 			BV02
 #define BVSECT_FOREST 			BV03
 #define BVSECT_HILLS 			BV04
 #define BVSECT_MOUNTAIN 		BV05
 #define BVSECT_WATER_SWIM 		BV06
 #define BVSECT_WATER_NOSWIM 		BV07
 #define BVSECT_UNDERWATER 		BV08
 #define BVSECT_AIR 			BV09
 #define BVSECT_DESERT 			BV10
 #define BVSECT_DUNNO 			BV11
 #define BVSECT_OCEANFLOOR 		BV12
 #define BVSECT_UNDERGROUND 		BV13
 #define BVSECT_LAVA			BV14
 #define BVSECT_SWAMP			BV15
 #define MAX_SECFLAG 			15


/*
 * Room flags.           Holy cow!  Talked about stripped away..
 * Used in #ROOMS.       Those merc guys know how to strip code down.
 *			 Lets put it all back... ;)
 */
/* BFS_MARK is for track and hunt */
typedef enum
{
	ROOM_NONE, ROOM_DARK, BFS_MARK, ROOM_NO_MOB, ROOM_INDOORS, 
	ROOM_NO_DRIVING, ROOM_NO_MAGIC, ROOM_BANK,
	ROOM_PRIVATE, ROOM_SAFE, ROOM_EMPTYSHOP, ROOM_PET_SHOP,
	ROOM_NODROPALL, ROOM_SILENCE, ROOM_LOGSPEECH, 
	ROOM_NODROP, ROOM_CLANSTOREROOM, ROOM_PLR_HOME,
	ROOM_EMPTY_HOME, ROOM_HOTEL, ROOM_NOFLOOR, ROOM_REFINERY,
	ROOM_FACTORY, ROOM_RECRUIT, ROOM_BOUNTY, ROOM_SPACECRAFT, ROOM_PROTOTYPE,
	ROOM_HOUSE, ROOM_BARRACKS, ROOM_DND, ROOM_NODEATH,
	ROOM_CRASH, ROOM_ARENA, ROOM_SANITARY, ROOM_DIRTY,
	MAX_ROOM_FLAG
} more_room_flags;

/*
 * Directions.
 * Used in #ROOMS.
 */
typedef enum
{
  DIR_NORTH, DIR_EAST, DIR_SOUTH, DIR_WEST, DIR_UP, DIR_DOWN,
  DIR_NORTHEAST, DIR_NORTHWEST, DIR_SOUTHEAST, DIR_SOUTHWEST, DIR_SOMEWHERE
} dir_types;

#define MAX_DIR			DIR_SOUTHWEST	/* max for normal walking */
#define DIR_PORTAL		DIR_SOMEWHERE	/* portal direction	  */


/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR		  BV00
#define EX_CLOSED		  BV01
#define EX_LOCKED		  BV02
#define EX_SECRET		  BV03
#define EX_SWIM			  BV04
#define EX_PICKPROOF		  BV05
#define EX_FLY			  BV06
#define EX_CLIMB		  BV07
#define EX_DIG			  BV08
#define EX_EATKEY		  BV09
#define EX_NOPASSDOOR		  BV10
#define EX_HIDDEN		  BV11
#define EX_PASSAGE		  BV12
#define EX_PORTAL 		  BV13
#define EX_RES1			  BV14
#define EX_RES2			  BV15
#define EX_xCLIMB		  BV16
#define EX_xENTER		  BV17
#define EX_xLEAVE		  BV18
#define EX_xAUTO		  BV19
#define EX_RES4	  		  BV20
#define EX_xSEARCHABLE		  BV21
#define EX_BASHED                 BV22
#define EX_BASHPROOF              BV23
#define EX_NOMOB		  BV24
#define EX_WINDOW		  BV25
#define EX_xLOOK		  BV26
#define MAX_EXFLAG		  26

/*
 * Sector types.
 * Used in #ROOMS.
 */
typedef enum
{
  SECT_INSIDE, SECT_CITY, SECT_FIELD, SECT_FOREST, SECT_HILLS, SECT_MOUNTAIN,
  SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_UNDERWATER, SECT_AIR, SECT_DESERT,
  SECT_DUNNO, SECT_OCEANFLOOR, SECT_UNDERGROUND, SECT_LAVA, SECT_SWAMP, SECT_SHIP,
  SECT_MAX
} sector_types;

/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
typedef enum
{
  WEAR_NONE = -1, WEAR_LIGHT = 0, WEAR_FINGER_L, WEAR_FINGER_R, WEAR_NECK_1,
  WEAR_NECK_2, WEAR_BODY, WEAR_HEAD, WEAR_LEGS, WEAR_FEET, WEAR_HANDS,
  WEAR_ARMS, WEAR_SHIELD, WEAR_ABOUT, WEAR_WAIST, WEAR_WRIST_L, WEAR_WRIST_R,
  WEAR_WIELD, WEAR_HOLD, WEAR_DUAL_WIELD, WEAR_EARS, WEAR_EYES,
  WEAR_MISSILE_WIELD, WEAR_BACK, WEAR_FACE, WEAR_ANKLE_L, WEAR_ANKLE_R, MAX_WEAR
} wear_locations;

/* Board Types */
typedef enum { BOARD_NOTE, BOARD_MAIL, BOARD_CLAN } board_types;

/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
typedef enum
{
  COND_DRUNK, COND_FULL, COND_THIRST, MAX_CONDS
} conditions;

/*
 * Positions.
 */
typedef enum
{
  POS_DEAD, POS_MORTAL, POS_INCAP, POS_STUNNED, POS_SLEEPING, POS_RESTING,
  POS_SITTING, POS_FIGHTING, POS_STANDING, POS_MOUNTED, POS_SHOVE, POS_DRAG,
  POS_CARRIED, POS_CARRYING
} positions;

/*
 * ACT bits for players.
 */
#define PLR_IS_NPC				BV00	/* Don't EVER set.	*/
#define PLR_BOUGHT_PET			BV01
#define PLR_RES02				BV02 /* FREE */
#define PLR_AUTOEXIT			BV03
#define PLR_AUTOLOOT			BV04
#define PLR_AUTOSAC				BV05
#define PLR_BLANK				BV06
#define PLR_RES07				BV07 /* FREE */
#define PLR_BRIEF				BV08
#define PLR_COMBINE				BV09
#define PLR_PROMPT				BV10
#define PLR_TELNET_GA			BV11
#define PLR_HOLYLIGHT			BV12
#define PLR_WIZINVIS			BV13
#define PLR_RES14				BV14 /* FREE */
#define	PLR_SILENCE				BV15
#define PLR_NO_EMOTE			BV16
#define PLR_RES17				BV17 /* FREE */
#define PLR_NO_TELL				BV18
#define PLR_LOG					BV19
#define PLR_DENY				BV20
#define PLR_FREEZE				BV21
#define PLR_RES22				BV22 /* FREE */
#define PLR_QUESTOR				BV23
#define PLR_LITTERBUG			BV24
#define PLR_ANSI				BV25
#define PLR_SOUND				BV26
#define PLR_NICE				BV27
#define PLR_FLEE				BV28
#define PLR_AUTOGOLD			BV29
#define PLR_GAMBLER				BV30
#define PLR_AFK					BV31

/* Bits for pc_data->flags. */
#define PCFLAG_R1					BV00
#define PCFLAG_HELPSTART			BV01
#define PCFLAG_UNAUTHED				BV02
#define PCFLAG_RES03				BV03 /* FREE */
#define PCFLAG_RES04				BV04 /* FREE */
#define PCFLAG_GAG					BV05
#define PCFLAG_RETIRED				BV06
#define PCFLAG_GUEST				BV07
#define PCFLAG_RES08				BV08 /* FREE */
#define PCFLAG_PAGERON				BV09
#define PCFLAG_NOTITLE				BV10
#define PCFLAG_ROOM					BV11
#define PCFLAG_ROOMVNUM				BV12
#define PCFLAG_RES13				BV13 /* FREE */
#define PCFLAG_QUESTING				BV14
#define PCFLAG_RES15				BV15 /* FREE */
#define PCFLAG_RES16				BV16 /* FREE */
#define PCFLAG_RES17				BV17 /* FREE */
#define PCFLAG_GROUPWHO				BV18
#define PCFLAG_RES19				BV19 /* FREE */
#define PCFLAG_RES20				BV20 /* FREE */
#define PCFLAG_WATCH				BV21 /* see function "do_watch" */
#define PCFLAG_DND					BV22 /* Do Not Disturb flag */
#define PCFLAG_RES23				BV23 /* FREE */
#define PCFLAG_MXP					BV24
#define PCFLAG_RES25				BV25 /* FREE */

typedef enum
{
  TIMER_NONE, TIMER_RECENTFIGHT, TIMER_SHOVEDRAG, TIMER_DO_FUN, 
  TIMER_APPLIED, TIMER_PKILLED, TIMER_ASUPRESSED, TIMER_NUISANCE
} timer_types;

struct timer_data
{
    TIMER  *	prev;
    TIMER  *	next;
    DO_FUN *	do_fun;
    int		value;
    sh_int	type;
    sh_int	count;
};


/*
 * Channel bits. BV4,5,15,27,13
 */
#define	CHANNEL_AUCTION		   BV00
#define	CHANNEL_CHAT		   BV01
#define	CHANNEL_QUEST		   BV02
#define	CHANNEL_IMMTALK		   BV03
#define	CHANNEL_AUTH		   BV04
#define CHANNEL_WHISPER		   BV05 /* Added June 3 / 2000 By Gavin */

#define	CHANNEL_R29			   BV29
#define	CHANNEL_R30			   BV30
#define	CHANNEL_R31			   BV31

#define	CHANNEL_SHOUT		   BV06
#define	CHANNEL_YELL		   BV07
#define CHANNEL_MONITOR		   BV08
#define CHANNEL_LOG			   BV09
#define CHANNEL_MUSIC		   BV10
#define CHANNEL_CLAN		   BV11
#define CHANNEL_BUILD		   BV12
#define CHANNEL_AVTALK		   BV14
#define CHANNEL_COUNCIL 	   BV16
#define	CHANNEL_EMOTE		   BV15 /* Added By Gavin - July 08 2000 */
#define CHANNEL_GUILD		   BV17
#define CHANNEL_COMM		   BV18
#define CHANNEL_TELLS		   BV19
#define CHANNEL_ORDER          BV20
#define CHANNEL_NEWBIE         BV21
#define CHANNEL_WARTALK        BV22
#define CHANNEL_OOC            BV23
#define CHANNEL_SHIP           BV24
#define CHANNEL_SYSTEM         BV25
#define CHANNEL_SPACE          BV26
#define CHANNEL_ANNOUNCE	   BV28
#define CHANNEL_PROGBUG		   BV27 /* Added Oct 28 2000 by Gavin */

#define CHANNEL_CLANTALK	   CHANNEL_CLAN

/* Area defines - Scryn 8/11
 *
 */
#define AREA_DELETED		   BV00
#define AREA_LOADED                BV01

/* Area flags - Narn Mar/96 */
#define AFLAG_NOPKILL               BV00
#define AFLAG_NOQUEST               BV01
#define AFLAG_ARENA	                BV02

/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct	mob_index_data
{
    MOB_INDEX_DATA *	next;
    MOB_INDEX_DATA *	next_sort;
    SPEC_FUN *		spec_fun;
    SPEC_FUN *          spec_2;
    SHOP_DATA *		pShop;
    REPAIR_DATA *	rShop;
    MPROG_DATA *	mudprogs;
    int			progtypes;
    char *		player_name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    int			vnum;
    sh_int		count;
    sh_int		killed;
    sh_int		sex;
    sh_int		level;
    int			act;
    int			affected_by;
    sh_int		alignment;
    sh_int		mobthac0;		/* Unused */
    sh_int		ac;
    sh_int		hitnodice;
    sh_int		hitsizedice;
    sh_int		hitplus;
    sh_int		damnodice;
    sh_int		damsizedice;
    sh_int		damplus;
    sh_int		numattacks;
    int			gold;
    int			exp;
    int			xflags;
    int			resistant;
    int			immune;
    int			susceptible;
    int			attacks;
    int			defenses;
    int			speaks;
    int 		speaking;
    sh_int		position;
    sh_int		defposition;
	sh_int		style;
    sh_int		height;
    sh_int		weight;
    sh_int		race;
    sh_int		hitroll;
    sh_int		damroll;
    sh_int		perm_str;
    sh_int		perm_int;
    sh_int		perm_wis;
    sh_int		perm_dex;
    sh_int		perm_con;
    sh_int		perm_cha;
    sh_int		perm_lck;
    sh_int		saving_poison_death;
    sh_int		saving_wand;
    sh_int		saving_para_petri;
    sh_int		saving_breath;
    sh_int		saving_spell_staff;
    int                 vip_flags;
	CLAN_DATA       *   mob_clan;    /* for spec_clan_guard.. set by postguard */
};


struct hunt_hate_fear
{
    char *		name;
    CHAR_DATA *		who;
};

struct fighting_data
{
    CHAR_DATA *		who;
    int			xp;
    sh_int		align;
    sh_int		duration;
    sh_int		timeskilled;
};

struct	editor_data
{
    sh_int		numlines;
    sh_int		on_line;
    sh_int		size;
    char		line[49][81];
};

struct	extracted_char_data
{
    EXTRACT_CHAR_DATA *	next;
    CHAR_DATA *		ch;
    ROOM_INDEX_DATA *	room;
    ch_ret		retcode;
    bool		extract;
};

/*
 * One character (PC or NPC).
 * (Shouldn't most of that build interface stuff use substate, dest_buf,
 * spare_ptr and tempnum?  Seems a little redundant)
 */
struct	char_data
{
	char *			full_name;
        EXT_BV          bodyparts;
    CHAR_DATA *		char_carriedby;
    CHAR_DATA *		char_carrying;
    CHAR_DATA *		next;
    CHAR_DATA *		prev;
    CHAR_DATA *		next_in_room;
    CHAR_DATA *		prev_in_room;
    CHAR_DATA *		master;
    CHAR_DATA *		leader;
	CHAR_DATA *		retell;
    FIGHT_DATA *	fighting;
    CHAR_DATA *		reply;
    CHAR_DATA *		switched;
    CHAR_DATA *		mount;
    HHF_DATA *		hunting;
    HHF_DATA *		fearing;
    HHF_DATA *		hating;
    SPEC_FUN *		spec_fun;
    SPEC_FUN *		spec_2;
    MPROG_ACT_LIST *	mpact;
    int			mpactnum;
    sh_int		mpscriptpos;
    MOB_INDEX_DATA *	pIndexData;
    DESCRIPTOR_DATA *	desc;
    AFFECT_DATA *	first_affect;
    AFFECT_DATA *	last_affect;
    NOTE_DATA *		pnote;
    NOTE_DATA *		comments;
    OBJ_DATA *		first_carrying;
    OBJ_DATA *		last_carrying;
	OBJ_DATA *		on;
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	was_in_room;
    ROOM_INDEX_DATA *   was_sentinel;
    PC_DATA *		pcdata;
    DO_FUN *		last_cmd;
    DO_FUN *		prev_cmd;   /* mapping */
    void *		dest_buf;
    void *		dest_buf_2;
    void *		spare_ptr;
    int			tempnum;
    EDITOR_DATA *	editor;
    TIMER	*	first_timer;
    TIMER	*	last_timer;
    char *		name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    sh_int		num_fighting;
    sh_int		substate;
    sh_int		sex;
    sh_int		race;
    sh_int              top_level;
    sh_int              skill_level[MAX_ABILITY];
    sh_int		trust;
    int			played;
    time_t		logon;
    time_t		save_time;
    sh_int		timer;
    sh_int		wait;
    sh_int		hit;
    sh_int		max_hit;
    sh_int		mana;
    sh_int		max_mana;
    sh_int		move;
    sh_int		max_move;
    sh_int		numattacks;
    int			gold;
    long		experience[MAX_ABILITY];
    int 		act;
	int 		act2;
	EXT_BV		new_act;
    int			affected_by;
    int			carry_weight;
    int			carry_number;
    int			xflags;
    int			resistant;
    int			immune;
    int			susceptible;
    int			attacks;
    int			defenses;
    int			speaks;
    int			speaking;
    sh_int		saving_poison_death;
    sh_int		saving_wand;
    sh_int		saving_para_petri;
    sh_int		saving_breath;
    sh_int		saving_spell_staff;
    sh_int		alignment;
    sh_int		barenumdie;
    sh_int		baresizedie;
    sh_int		mobthac0;
    sh_int		hitroll;
    sh_int		damroll;
    sh_int		hitplus;
    sh_int		damplus;
    sh_int		position;
    sh_int		defposition;
    sh_int		height;
    sh_int		weight;
    sh_int		armor;
    sh_int		wimpy;
    int			deaf;
    sh_int		perm_str;
    sh_int		perm_int;
    sh_int		perm_wis;
    sh_int		perm_dex;
    sh_int		perm_con;
    sh_int		perm_cha;
    sh_int		perm_lck;
    sh_int		mod_str;
    sh_int		mod_int;
    sh_int		mod_wis;
    sh_int		mod_dex;
    sh_int		mod_con;
    sh_int		mod_cha;
    sh_int		mod_lck;
    sh_int		mental_state;		/* simplified */
    sh_int		emotional_state;	/* simplified */
    sh_int		inter_substate;                 /* BUILD INTERFACE */
    int			retran;
    int			regoto;
    sh_int		mobinvis;	/* Mobinvis level SB */
    int                 vip_flags;
    sh_int              backup_wait;	/* reinforcements */
    int                 backup_mob;     /* reinforcements */
    sh_int              was_stunned;
    GUARD_DATA      *   guard_data;
    sh_int              main_ability;
	CHAR_DATA *         questgiver; /* Vassago */
    sh_int              nextquest; /* Vassago */
    sh_int              countdown; /* Vassago */
    sh_int              questobj; /* Vassago */
    sh_int              questmob; /* Vassago */
	CLAN_DATA       *   mob_clan;    /* for spec_clan_guard.. set by postguard */
	int			buzzed;
	int			buzzedfrom;
	int			exp;
	char *   owner;
	ROOM_INDEX_DATA *home;
};


struct killed_data
{
    int			vnum;
    char		count;
};

/*
 * EXTRA bits for players. (KaVir)
 */

#define EXTRA_PREGNANT			BV00 /* 1   */
#define EXTRA_MARRIED			BV01 /* 2   */
#define EXTRA_NON_NEWBIE		BV02 /* 4   */
#define EXTRA_LABOUR			BV03 /* 8   */
#define EXTRA_BORN				BV04 /* 16  */
#define EXTRA_EXP				BV05 /* 32  */
#define EXTRA_DONE				BV06 /* 64  */
#define EXTRA_PURGED			BV07 /* 128 */
#define EXTRA_INFECTION		        BV08 /* 256 */
#define EXTRA_R3				512
#define EXTRA_R4				1024
#define EXTRA_R5				2048
#define EXTRA_R6				4096
#define EXTRA_R7				8192
#define EXTRA_R8				16384
#define EXTRA_R9				32768
#define EXTRA_R10				65536
#define EXTRA_R11				131072
#define EXTRA_R12				262144
#define EXTRA_R13				524288
#define EXTRA_R14				1048576
#define EXTRA_R15				2097152 
#define EXTRA_R16				4194304
#define EXTRA_R17				8388608
#define EXTRA_R18				16777216 
#define EXTRA_R19				33554432 
#define EXTRA_R20				67108864 
#define EXTRA_R21				134217728 
#define EXTRA_R22				268435456 


/* Structure for link list of ignored players */
struct ignore_data
{
    IGNORE_DATA *next;
    IGNORE_DATA *prev;
    char *name;
};

/* Max number of people you can ignore at once */
#define MAX_IGN		6

/*
 * Data which only PC's have.
 */
struct	pc_data
{
	sh_int          infect_amount[MAX_BODY_PARTS]; /* how much and if its infected */
	sh_int          clean_amount; /* timer/countdown saying if ch's hands are clean */
	TIME_INFO_DATA  pregnacy;
	char *			partner;
	char *			x_partner;
    CHAR_DATA *		propose;
	char *			parents;
    char *			cparents;
    char *			marriage;
	char *			pob;
	sh_int			stage[4];
	int				genes[10];
	CHAR_DATA *		pet;
    CLAN_DATA *		clan;
    AREA_DATA *		area;
    COUNCIL_DATA *	council;
    char *			council_name;
    char *			pwd;
    char *			bamfin;
    char *			bamfout;
    char *			rank;
    char *			title;
    char *			bestowments;	/* Special bestowed commands	   */
	char *			filename;       /* For the safe mset name -Shaddai */
    int				flags;		/* Whether the player is deadly and whatever else we add.      */
	NUISANCE_DATA	*nuisance;       /* New Nuisance structure */
    long int		restore_time;	/* The last time the char did a restore all */
    int				r_range_lo;	/* room range */
	int				r_range_hi;
    int				m_range_lo;	/* mob range  */
    int				m_range_hi;
    int				o_range_lo;	/* obj range  */
    int				o_range_hi;		
    sh_int			wizinvis;	/* wizinvis level */
    sh_int			min_snoop;	/* minimum snoop level */
    sh_int			condition	[MAX_CONDS];
    sh_int			learned		[MAX_SKILL];
    KILLED_DATA		killed		[MAX_KILLTRACK];
    int				quest_number;	/* current *QUEST BEING DONE* DON'T REMOVE! */
    int				quest_curr;	/* current number of quest points */
    sh_int			auth_state;
    time_t			release_date;	/* Auto-helling.. Altrag */
    char *			helled_by;
    char *			bio;		/* Personal Bio */
    char *			authed_by;	/* what crazy imm authed this name ;) */
    char *			prompt;		/* User config prompts */
	char *			fprompt;	/* Fight prompts */
    char *			subprompt;	/* Substate prompt */
    sh_int			pagerlen;	/* For pager (NOT menus) */
	sh_int			addiction[10];
    sh_int			drug_level[10];
    int				wanted_flags;
    long			bank;
	sh_int			clan_rank;		/* 0 for unguilded and loner 1-10 otherwise */
	sh_int			haircolor;		/* For Hair Color Type */
	sh_int			eyes;			/* For Eye Color Type */
	sh_int			build;			/* For Build Type */
	sh_int			complex;		/* For Complex Type */
    IGNORE_DATA	*	first_ignored; 	/* keep track of who to ignore */
    IGNORE_DATA	*	last_ignored;    
	sh_int		charmies;	/* Number of Charmies */
	    int			pkills;		/* Number of pkills on behalf of clan */
    int			pdeaths;	/* Number of times pkilled (legally)  */
    int			mkills;		/* Number of mobs killed		   */
    int			mdeaths;	/* Number of deaths due to mobs       */
    int			illegal_pk;	/* Number of illegal pk's committed   */
    char *			clan_name;
    ROOM_INDEX_DATA *   plr_home;
    MOB_INDEX_DATA * vendor;
        sh_int          birth_wait;     /* time it takes to have the child*/
        char *          birth_name; /* babies name */
        TIME_INFO_DATA  birthday;
        int             cyber;
        sh_int          oldage;  
        sh_int          comchan; 
        sh_int          age;
};



/*
 * Liquids.
 */
#define LIQ_WATER        0
#define LIQ_MAX		20

struct	liq_type
{
    char *	liq_name;
    char *	liq_color;
    sh_int	liq_affect[3];
};



/*
 * Extra description data for a room or object.
 */
struct	extra_descr_data
{
    EXTRA_DESCR_DATA *next;	/* Next in list                     */
    EXTRA_DESCR_DATA *prev;	/* Previous in list                 */
    char *keyword;              /* Keyword in look/examine          */
    char *description;          /* What to see                      */
};



/*
 * Prototype for an object.
 */
struct	obj_index_data
{
    OBJ_INDEX_DATA *	next;
    OBJ_INDEX_DATA *	next_sort;
    EXTRA_DESCR_DATA *	first_extradesc;
    EXTRA_DESCR_DATA *	last_extradesc;
    AFFECT_DATA *	first_affect;
    AFFECT_DATA *	last_affect;
    MPROG_DATA *	mudprogs;               /* objprogs */
    int			progtypes;              /* objprogs */
    char *		name;
    char *		short_descr;
    char *		description;
    char *		action_desc;
    int			vnum;
    sh_int              level;
    sh_int		item_type;
    int			extra_flags;
    int			magic_flags; /*Need more bitvectors for spells - Scryn*/
    int			wear_flags;
    sh_int		count;
    sh_int		weight;
    int			cost;
    int			value	[6];
    int			serial;
    sh_int		layers;
    int			rent;			/* Unused */
};


/*
 * One object.
 */
struct	obj_data
{
    OBJ_DATA *		next;
    OBJ_DATA *		prev;
    OBJ_DATA *		next_content;
    OBJ_DATA *		prev_content;
    OBJ_DATA *		first_content;
    OBJ_DATA *		last_content;
    OBJ_DATA *		in_obj;
    CHAR_DATA *		carried_by;
    EXTRA_DESCR_DATA *	first_extradesc;
    EXTRA_DESCR_DATA *	last_extradesc;
    AFFECT_DATA *	first_affect;
    AFFECT_DATA *	last_affect;
    OBJ_INDEX_DATA *	pIndexData;
    ROOM_INDEX_DATA *	in_room;
    char *		armed_by;	/* who armed for grenades, who killed for corpses */
    char *		name;
    char *		short_descr;
    char *		description;
    char *		action_desc;
    sh_int		item_type;
    sh_int		mpscriptpos;
    int			extra_flags;
    int			magic_flags; /*Need more bitvectors for spells - Scryn*/
    int			wear_flags; 
    int                 blaster_setting;
    MPROG_ACT_LIST *	mpact;		/* mudprogs */
    int			mpactnum;	/* mudprogs */
    sh_int		wear_loc;
    sh_int		weight;
    int			cost;
    sh_int		level;
    sh_int		timer;
    int			value	[6];
    sh_int		count;		/* support for object grouping */
    int			serial;		/* serial number	       */
};


/*
 * Exit data.
 */
struct	exit_data
{
    EXIT_DATA *		prev;		/* previous exit in linked list	*/
    EXIT_DATA *		next;		/* next exit in linked list	*/
    EXIT_DATA *		rexit;		/* Reverse exit pointer		*/
    ROOM_INDEX_DATA *	to_room;	/* Pointer to destination room	*/
    char *		keyword;	/* Keywords for exit or door	*/
    char *		description;	/* Description of exit		*/
    int			vnum;		/* Vnum of room exit leads to	*/
    int			rvnum;		/* Vnum of room in opposite dir	*/
    int			exit_info;	/* door states & other flags	*/
    int			key;		/* Key vnum			*/
    sh_int		vdir;		/* Physical "direction"		*/
    sh_int		distance;	/* how far to the next room	*/
};



/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'H': hide an object
 *   'B': set a bitvector
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct	reset_data
{
    RESET_DATA *	next;
    RESET_DATA *	prev;
    char		command;
    int			extra;
    int			arg1;
    int			arg2;
    int			arg3;
};

/* Constants for arg2 of 'B' resets. */
#define	BIT_RESET_DOOR			0
#define BIT_RESET_OBJECT		1
#define BIT_RESET_MOBILE		2
#define BIT_RESET_ROOM			3
#define BIT_RESET_TYPE_MASK		0xFF	/* 256 should be enough */
#define BIT_RESET_DOOR_THRESHOLD	8
#define BIT_RESET_DOOR_MASK		0xFF00	/* 256 should be enough */
#define BIT_RESET_SET			BV30
#define BIT_RESET_TOGGLE		BV31
#define BIT_RESET_FREEBITS	  0x3FFF0000	/* For reference */



/*
 * Area definition.
 */
struct	area_data
{
    AREA_DATA *		next;
    AREA_DATA *		prev;
    AREA_DATA *		next_sort;
    AREA_DATA *		prev_sort;
	AREA_DATA *         next_sort_name; /* Used for alphanum. sort */
    AREA_DATA *         prev_sort_name; /* Ditto, Fireblade */
    RESET_DATA *	first_reset;
    RESET_DATA *	last_reset;
    PLANET_DATA *       planet;
    AREA_DATA *   	next_on_planet;
    AREA_DATA *    	prev_on_planet;
    char *		name;
    char *		filename;
    int                 flags;
    sh_int              status;  /* h, 8/11 */
    sh_int		age;
    sh_int		nplayer;
    sh_int		reset_frequency;
    int			low_r_vnum;
    int			hi_r_vnum;
    int			low_o_vnum;
    int			hi_o_vnum;
    int			low_m_vnum;
    int			hi_m_vnum;
    int			low_soft_range;
    int			hi_soft_range;
    int			low_hard_range;
    int			hi_hard_range;
    char *		author; /* Scryn */
    char *              resetmsg; /* Rennard */
    RESET_DATA *	last_mob_reset;
    RESET_DATA *	last_obj_reset;
    sh_int		max_players;
   
    int			high_economy;
    int			low_economy;
	int			version;
	WEATHER_DATA *	weather; /* FB */
};



/*
 * Load in the gods building data. -- Altrag
 */
struct	godlist_data
{
    GOD_DATA *		next;
    GOD_DATA *		prev;
    int			level;
    int			low_r_vnum;
    int			hi_r_vnum;
    int			low_o_vnum;
    int			hi_o_vnum;
    int			low_m_vnum;
    int			hi_m_vnum;
};


/*
 * Used to keep track of system settings and statistics		-Thoric
 */
struct	system_data
{
    int		maxplayers;		/* Maximum players this boot   */
    int		alltimemax;		/* Maximum players ever	  */
    char *	time_of_max;		/* Time of max ever */
	char *	mud_name;		/* Name of mud */
    bool	NO_NAME_RESOLVING;	/* Hostnames are not resolved  */
    bool    	DENY_NEW_PLAYERS;	/* New players cannot connect  */
    bool	WAIT_FOR_AUTH;		/* New players must be auth'ed */
    sh_int	read_all_mail;		/* Read all player mail(was 54)*/
    sh_int	read_mail_free;		/* Read mail for free (was 51) */
    sh_int	write_mail_free;	/* Write mail for free(was 51) */
    sh_int	take_others_mail;	/* Take others mail (was 54)   */
    sh_int	build_level;		/* Level of build channel LEVEL_BUILD*/
    sh_int	log_level;		/* Level of log channel LEVEL LOG*/
    sh_int	level_modify_proto;	/* Level to modify prototype stuff LEVEL_LESSER */
    sh_int	level_override_private;	/* override private flag */
    sh_int	level_mset_player;	/* Level to mset a player */
    sh_int	stun_plr_vs_plr;	/* Stun mod player vs. player */
    sh_int	stun_regular;		/* Stun difficult */
    sh_int	dam_plr_vs_plr;		/* Damage mod player vs. player */
    sh_int	dam_plr_vs_mob;		/* Damage mod player vs. mobile */
    sh_int	dam_mob_vs_plr;		/* Damage mod mobile vs. player */
    sh_int	dam_mob_vs_mob;		/* Damage mod mobile vs. mobile */
    sh_int	level_getobjnotake;     /* Get objects without take flag */
    sh_int  level_forcepc;          /* The level at which you can use force on players. */
    int		save_flags;		/* Toggles for saving conditions */
    sh_int	save_frequency;		/* How old to autosave someone */
	sh_int save_pets;		/* Do pets save? */
	sh_int ban_site_level;      /* Level to ban sites */
    sh_int ban_class_level;     /* Level to ban classes */
    sh_int ban_race_level;      /* Level to ban races */
	    sh_int ident_retries;	/* Number of times to retry broken pipes. */
};


/*
 * Room type.
 */
struct	room_index_data
{
    ROOM_INDEX_DATA *	next;
    ROOM_INDEX_DATA *	next_sort;
    CHAR_DATA *		first_person;
    CHAR_DATA *		last_person;
    OBJ_DATA *		first_content;
    OBJ_DATA *		last_content;
    EXTRA_DESCR_DATA *	first_extradesc;
    EXTRA_DESCR_DATA *	last_extradesc;
    AREA_DATA *		area;
    EXIT_DATA *		first_exit;
    EXIT_DATA *		last_exit;
    SHIP_DATA * 	first_ship;
    SHIP_DATA * 	last_ship;	
	HOME_DATA * 	first_home;
    HOME_DATA * 	last_home;
	AFFECT_DATA *	first_affect;	    /* effects on the room	*/
    AFFECT_DATA *	last_affect;	    /*		..		*/
    char *		name;
    char *		description;
    int			vnum;
    EXT_BV		room_flags;
    MPROG_ACT_LIST *	mpact;               /* mudprogs */
    int			mpactnum;            /* mudprogs */
    MPROG_DATA *	mudprogs;            /* mudprogs */
    sh_int		mpscriptpos;
    int			progtypes;           /* mudprogs */
    sh_int		light;
    sh_int		sector_type;
    sh_int		tunnel;		     /* max people that will fit */
};

/*
 * Types of skill numbers.  Used to keep separate lists of sn's
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000  /* allows for 1000 skills/spells */
#define TYPE_HERB		     2000  /* allows for 1000 attack types  */
#define TYPE_PERSONAL		     3000  /* allows for 1000 herb types    */
#define TYPE_RACIAL		     4000  /* allows for 1000 personal types*/
#define TYPE_DISEASE		     5000  /* allows for 1000 racial types  */


/*
 *  Target types.
 */
typedef enum
{
  TAR_IGNORE, TAR_CHAR_OFFENSIVE, TAR_CHAR_DEFENSIVE, TAR_CHAR_SELF,
  TAR_OBJ_INV
} target_types;

typedef enum
{
  SKILL_UNKNOWN, SKILL_SPELL, SKILL_SKILL, SKILL_WEAPON, SKILL_TONGUE,
  SKILL_HERB, SKILL_RACIAL, SKILL_DISEASE
} skill_types;




struct timerset
{
  int num_uses;
  struct timeval total_time;
  struct timeval min_time;
  struct timeval max_time;
};



/*
 * Skills include spells as a particular case.
 */
struct	skill_type
{
    char *	name;			/* Name of skill		*/
    SPELL_FUN *	spell_fun;		/* Spell pointer (for spells)	*/
    DO_FUN *	skill_fun;		/* Skill pointer (for skills)	*/
    sh_int	target;			/* Legal targets		*/
    sh_int	minimum_position;	/* Position for caster / user	*/
    sh_int	slot;			/* Slot for #OBJECT loading	*/
    sh_int	min_mana;		/* Minimum mana used		*/
    sh_int	beats;			/* Rounds required to use skill	*/
    char *	noun_damage;		/* Damage message		*/
    char *	msg_off;		/* Wear off message		*/
    sh_int	guild;			/* Which guild the skill belongs to */
    sh_int	min_level;		/* Minimum level to be able to cast */
    sh_int	type;			/* Spell/Skill/Weapon/Tongue	*/
    int		flags;			/* extra stuff			*/
    char *	hit_char;		/* Success message to caster	*/
    char *	hit_vict;		/* Success message to victim	*/
    char *	hit_room;		/* Success message to room	*/
    char *	miss_char;		/* Failure message to caster	*/
    char *	miss_vict;		/* Failure message to victim	*/
    char *	miss_room;		/* Failure message to room	*/
    char *	die_char;		/* Victim death msg to caster	*/
    char *	die_vict;		/* Victim death msg to victim	*/
    char *	die_room;		/* Victim death msg to room	*/
    char *	imm_char;		/* Victim immune msg to caster	*/
    char *	imm_vict;		/* Victim immune msg to victim	*/
    char *	imm_room;		/* Victim immune msg to room	*/
    char *	dice;			/* Dice roll			*/
    int		value;			/* Misc value			*/
	int		spell_sector;		/* Sector Spell work	 	*/
    char	saves;			/* What saving spell applies	*/
    SMAUG_AFF *	affects;		/* Spell affects, if any	*/
    char *	teachers;		/* Skill requires a special teacher */
    char	participants;		/* # of required participants	*/
    struct	timerset	userec;	/* Usage record			*/
    int         alignment;              /* for jedi powers */
	char *  made_char;        /* Made by??? -- Xerves 8/7/99 */
        sh_int  prototype;        /* Check for new spells/skills -- Xerves */
};

/* how many items to track.... prevent repeat auctions */
#define AUCTION_MEM 3

struct  auction_data
{
    OBJ_DATA  * item;   /* a pointer to the item */
    CHAR_DATA * seller; /* a pointer to the seller - which may NOT quit */
    CHAR_DATA * buyer;  /* a pointer to the buyer - which may NOT quit */
    int         bet;    /* last bet - or 0 if noone has bet anything */
    sh_int      going;  /* 1,2, sold */
    sh_int      pulse;  /* how many pulses (.25 sec) until another call-out ? */
    int 	starting;
	    OBJ_INDEX_DATA *	history[AUCTION_MEM];	/* store auction history */
    sh_int	hist_timer;		/* clear out history buffer if auction is idle */	

};

/*
 * So we can have different configs for different ports -- Shaddai
 */
extern int port;

/*
 * These are skill_lookup return values for common skills and spells.
 */

/*
 * Engineering Skills
 */
extern sh_int   gsn_shipmaintenance; 
extern sh_int   gsn_makeblade;
extern sh_int   gsn_makejewelry;
extern sh_int   gsn_makeblaster;
extern sh_int   gsn_makelight;
extern sh_int   gsn_makecomlink;
extern sh_int   gsn_makegrenade;
extern sh_int   gsn_makelandmine;
extern sh_int   gsn_makearmor;
extern sh_int   gsn_makeshield;
extern sh_int   gsn_makecontainer;
extern sh_int   gsn_makedroid;
extern sh_int   gsn_makefurniture;
extern sh_int   gsn_makekeyring;
extern sh_int   gsn_makemedkit;
extern sh_int   gsn_make_infect_med;

/*
 * Piloting Skills
 */
extern sh_int   gsn_starfighters;
extern sh_int   gsn_midships;
extern sh_int   gsn_capitalships;
extern sh_int   gsn_weaponsystems;
extern sh_int   gsn_navigation;
extern sh_int   gsn_shipsystems;
extern sh_int   gsn_shipdocking;
extern sh_int   gsn_tractorbeams;
extern sh_int   gsn_spacecombat;
extern sh_int   gsn_spacecombat2;
extern sh_int   gsn_spacecombat3;
extern sh_int	gsn_bomb;
extern sh_int	gsn_evasive;


/*
 * Diplomacy Skills
 */
extern sh_int   gsn_reinforcements;
extern sh_int   gsn_postguard;
extern sh_int   gsn_addpatrol;
extern sh_int   gsn_eliteguard;
extern sh_int   gsn_specialforces;
extern sh_int   gsn_smalltalk;
extern sh_int   gsn_propeganda;
extern sh_int   gsn_masspropeganda;
extern sh_int   gsn_gather_intelligence;
extern	sh_int	gsn_peek;

/*
 * Combat Skills
 */
extern	sh_int	gsn_disarm;
extern	sh_int	gsn_enhanced_damage;
extern	sh_int	gsn_kick;
extern	sh_int	gsn_rescue;
extern	sh_int	gsn_second_attack;
extern	sh_int	gsn_third_attack;
extern	sh_int	gsn_dual_wield;
extern	sh_int	gsn_trip;
extern	sh_int	gsn_butcher;
extern sh_int   gsn_mine;
extern sh_int   gsn_grenades;
extern sh_int   gsn_snipe;
extern sh_int   gsn_throw;
extern	sh_int	gsn_dodge;
extern  sh_int  gsn_scan;
extern	sh_int	gsn_gouge;
extern	sh_int	gsn_mount;
extern  sh_int  gsn_bashdoor;
extern	sh_int	gsn_berserk;
extern	sh_int	gsn_hitall;

/*
 * Bounty Hunting Skills
 */
extern sh_int	gsn_track;
extern sh_int	gsn_listen;
extern sh_int   gsn_torture;
extern sh_int	gsn_blackjack;
extern sh_int	gsn_backstab;


/*
 * Smuggling Skills
 */
extern  sh_int  gsn_dig;
extern	sh_int	gsn_search;
extern	sh_int	gsn_steal;
extern	sh_int	gsn_sneak;
extern	sh_int	gsn_pick_lock;
extern	sh_int	gsn_hide;
extern sh_int	gsn_pickshiplock;
extern sh_int	gsn_hijack;
extern sh_int   gsn_beg;
extern sh_int   gsn_jail;
extern sh_int   gsn_bribe;
extern sh_int   gsn_seduce;
extern sh_int   gsn_disguise;

/*
 * Piracy Skills
 */
extern sh_int   gsn_gemcutting;
extern sh_int   gsn_spice_refining;
extern sh_int	gsn_stealcode;

/*
 * Medical Skills
 */
extern sh_int	gsn_aid;
extern sh_int   gsn_first_aid;
extern sh_int	gsn_splint;
extern sh_int	gsn_birth;
extern sh_int	gsn_abort;
extern sh_int	gsn_cosmetic;
extern sh_int	gsn_diagnose;
extern sh_int	gsn_autopsy;
extern sh_int   gsn_washup;
extern sh_int   gsn_inject;

/*
 * Force Skills
 */
extern	sh_int	gsn_parry;

/* undecided */
extern sh_int   gsn_ship_masking; 

/* used to do specific lookups */
extern	sh_int	gsn_first_spell;
extern	sh_int	gsn_first_skill;
extern	sh_int	gsn_first_weapon;
extern	sh_int	gsn_first_tongue;
extern	sh_int	gsn_top_sn;

/* spells */
extern	sh_int	gsn_blindness;
extern	sh_int	gsn_charm_person;
extern  sh_int  gsn_aqua_breath;
extern	sh_int	gsn_invis;
extern	sh_int	gsn_mass_invis;
extern	sh_int	gsn_poison;
extern	sh_int	gsn_sleep;
extern  sh_int  gsn_possess;
extern	sh_int	gsn_fireball;		/* for fireshield  */
extern	sh_int	gsn_lightning_bolt;	/* for shockshield */

/* newer attack skills */
extern	sh_int	gsn_punch;
extern	sh_int	gsn_bash;
extern	sh_int	gsn_stun;

extern  sh_int  gsn_poison_weapon;
extern	sh_int	gsn_climb;

extern	sh_int	gsn_blasters;
extern	sh_int	gsn_force_pikes;
extern	sh_int	gsn_bowcasters;
extern	sh_int	gsn_lightsabers;
extern	sh_int	gsn_vibro_blades;
extern  sh_int	gsn_vandalize;
extern	sh_int	gsn_talonous_arms;
extern	sh_int	gsn_bludgeons;

extern  sh_int  gsn_grip;

/* 
 * Languages
 */
extern  sh_int			gsn_common;
extern  sh_int			gsn_wookiee;
extern  sh_int			gsn_twilek;
extern  sh_int			gsn_rodian;
extern  sh_int			gsn_hutt;
extern  sh_int			gsn_mon_calamari;
extern  sh_int			gsn_noghri;
extern  sh_int			gsn_gamorrean;
extern  sh_int			gsn_jawa;
extern  sh_int                  gsn_adarian;
extern  sh_int                  gsn_ewok;
extern  sh_int                  gsn_verpine;
extern  sh_int                  gsn_defel;
extern  sh_int                  gsn_trandoshan;
extern  sh_int                  gsn_chadra_fan;
extern  sh_int                  gsn_quarren;
extern  sh_int                  gsn_duinduogwuin;
extern  sh_int                  gsn_ithorian;
extern  sh_int                  gsn_gotal;
extern  sh_int                  gsn_devaronian;
extern	sh_int			gsn_bothan;
extern	sh_int			gsn_askani;

/*
 * Cmd flag names --Shaddai
 */
extern char *const cmd_flags[];

/*
 * Utility macros.
 */
#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))

 /*
 * Old-style Bit manipulation macros
 *
 * The bit passed is the actual value of the bit (Use the BV## defines)
 */ 
#define IS_SET(flag, bit)		((flag) & (bit))
#define SET_BIT(var, bit)		((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define TOGGLE_BIT(var, bit)	((var) ^= (bit))
#define CH(d)                  ((d)->original ? (d)->original : (d)->character)
#define CLEAR_BITS(var)			((var) = 0)
#define CH(d)					((d)->original ? (d)->original : (d)->character)

/*
 * Macros for accessing virtually unlimited bitvectors.		-Thoric
 *
 * Note that these macros use the bit number rather than the bit value
 * itself -- which means that you can only access _one_ bit at a time
 *
 * This code uses an array of integers
 */

/*
 * The functions for these prototypes can be found in misc.c
 * They are up here because they are used by the macros below
 */
bool	ext_is_empty		args( ( EXT_BV *bits ) );
void	ext_clear_bits		args( ( EXT_BV *bits ) );
int	ext_has_bits		args( ( EXT_BV *var, EXT_BV *bits) );
bool	ext_same_bits		args( ( EXT_BV *var, EXT_BV *bits) );
void	ext_set_bits		args( ( EXT_BV *var, EXT_BV *bits) );
void	ext_remove_bits		args( ( EXT_BV *var, EXT_BV *bits) );
void	ext_toggle_bits		args( ( EXT_BV *var, EXT_BV *bits) );

/*
 * Here are the extended bitvector macros:
 */
#define xIS_SET(var, bit)	((var).bits[(bit) >> RSV] & 1 << ((bit) & XBM))
#define xSET_BIT(var, bit)	((var).bits[(bit) >> RSV] |= 1 << ((bit) & XBM))
#define xSET_BITS(var, bit)	(ext_set_bits(&(var), &(bit)))
#define xREMOVE_BIT(var, bit)	((var).bits[(bit) >> RSV] &= ~(1 << ((bit) & XBM)))
#define xREMOVE_BITS(var, bit)	(ext_remove_bits(&(var), &(bit)))
#define xTOGGLE_BIT(var, bit)	((var).bits[(bit) >> RSV] ^= 1 << ((bit) & XBM))
#define xTOGGLE_BITS(var, bit)	(ext_toggle_bits(&(var), &(bit)))
#define xCLEAR_BITS(var)	(ext_clear_bits(&(var)))
#define xIS_EMPTY(var)		(ext_is_empty(&(var)))
#define xHAS_BITS(var, bit)	(ext_has_bits(&(var), &(bit)))
#define xSAME_BITS(var, bit)	(ext_same_bits(&(var), &(bit)))

/*
 * Memory allocation macros.
 */

#define CREATE(result, type, number)				\
do								\
{								\
    if (!((result) = (type *) calloc ((number), sizeof(type))))	\
    {								\
	perror("malloc failure");				\
	fprintf(stderr, "Malloc failure @ %s:%d\n", __FILE__, __LINE__ ); \
	abort();						\
    }								\
} while(0)

#define RECREATE(result,type,number)				\
do								\
{								\
    if (!((result) = (type *) realloc ((result), sizeof(type) * (number))))\
    {								\
	perror("realloc failure");				\
	fprintf(stderr, "Realloc failure @ %s:%d\n", __FILE__, __LINE__ ); \
	abort();						\
    }								\
} while(0)


#define DISPOSE(point) 						\
do								\
{								\
  if (!(point))							\
  {								\
	bug( "Freeing null pointer %s:%d", __FILE__, __LINE__ ); \
	fprintf( stderr, "DISPOSEing NULL in %s, line %d\n", __FILE__, __LINE__ ); \
  }								\
  else free(point);						\
  point = NULL;							\
} while(0)

#ifdef HASHSTR
#define STRALLOC(point)		str_alloc((point))
#define QUICKLINK(point)	quick_link((point))
#define QUICKMATCH(p1, p2)	(int) (p1) == (int) (p2)
#define STRFREE(point)						\
do								\
{								\
  if (!(point))							\
  {								\
	bug( "Freeing null pointer %s:%d", __FILE__, __LINE__ ); \
	fprintf( stderr, "STRFREEing NULL in %s, line %d\n", __FILE__, __LINE__ ); \
  }								\
  else if (str_free((point))==-1) 				\
    fprintf( stderr, "STRFREEing bad pointer in %s, line %d\n", __FILE__, __LINE__ ); \
  point = NULL;							\
} while(0)
#else
#define STRALLOC(point)		str_dup((point))
#define QUICKLINK(point)	str_dup((point))
#define QUICKMATCH(p1, p2)	strcmp((p1), (p2)) == 0
#define STRFREE(point)						\
do								\
{								\
  if (!(point))							\
  {								\
	bug( "Freeing null pointer %s:%d", __FILE__, __LINE__ ); \
	fprintf( stderr, "STRFREEing NULL in %s, line %d\n", __FILE__, __LINE__ ); \
  }								\
  else free((point));						\
  point = NULL;							\
} while(0)
#endif

/* double-linked list handling macros -Thoric */

#define LINK(link, first, last, next, prev)			\
do								\
{								\
    if ( !(first) )						\
      (first)			= (link);			\
    else							\
      (last)->next		= (link);			\
    (link)->next		= NULL;				\
    (link)->prev		= (last);			\
    (last)			= (link);			\
} while(0)

#define INSERT(link, insert, first, next, prev)			\
do								\
{								\
    (link)->prev		= (insert)->prev;		\
    if ( !(insert)->prev )					\
      (first)			= (link);			\
    else							\
      (insert)->prev->next	= (link);			\
    (insert)->prev		= (link);			\
    (link)->next		= (insert);			\
} while(0)

#define UNLINK(link, first, last, next, prev)			\
do								\
{								\
    if ( !(link)->prev )					\
      (first)			= (link)->next;			\
    else							\
      (link)->prev->next	= (link)->next;			\
    if ( !(link)->next )					\
      (last)			= (link)->prev;			\
    else							\
      (link)->next->prev	= (link)->prev;			\
} while(0)


#define CHECK_LINKS(first, last, next, prev, type)		\
do {								\
  type *ptr, *pptr = NULL;					\
  if ( !(first) && !(last) )					\
    break;							\
  if ( !(first) )						\
  {								\
    bug( "CHECK_LINKS: last with NULL first!  %s.",		\
        __STRING(first) );					\
    for ( ptr = (last); ptr->prev; ptr = ptr->prev );		\
    (first) = ptr;						\
  }								\
  else if ( !(last) )						\
  {								\
    bug( "CHECK_LINKS: first with NULL last!  %s.",		\
        __STRING(first) );					\
    for ( ptr = (first); ptr->next; ptr = ptr->next );		\
    (last) = ptr;						\
  }								\
  if ( (first) )						\
  {								\
    for ( ptr = (first); ptr; ptr = ptr->next )			\
    {								\
      if ( ptr->prev != pptr )					\
      {								\
        bug( "CHECK_LINKS(%s): %p:->prev != %p.  Fixing.",	\
            __STRING(first), ptr, pptr );			\
        ptr->prev = pptr;					\
      }								\
      if ( ptr->prev && ptr->prev->next != ptr )		\
      {								\
        bug( "CHECK_LINKS(%s): %p:->prev->next != %p.  Fixing.",\
            __STRING(first), ptr, ptr );			\
        ptr->prev->next = ptr;					\
      }								\
      pptr = ptr;						\
    }								\
    pptr = NULL;						\
  }								\
  if ( (last) )							\
  {								\
    for ( ptr = (last); ptr; ptr = ptr->prev )			\
    {								\
      if ( ptr->next != pptr )					\
      {								\
        bug( "CHECK_LINKS (%s): %p:->next != %p.  Fixing.",	\
            __STRING(first), ptr, pptr );			\
        ptr->next = pptr;					\
      }								\
      if ( ptr->next && ptr->next->prev != ptr )		\
      {								\
        bug( "CHECK_LINKS(%s): %p:->next->prev != %p.  Fixing.",\
            __STRING(first), ptr, ptr );			\
        ptr->next->prev = ptr;					\
      }								\
      pptr = ptr;						\
    }								\
  }								\
} while(0)


#define ASSIGN_GSN(gsn, skill)					\
do								\
{								\
    if ( ((gsn) = skill_lookup((skill))) == -1 )		\
	fprintf( stderr, "ASSIGN_GSN: Skill %s not found.\n",	\
		(skill) );					\
} while(0)

#define CHECK_SUBRESTRICTED(ch)					\
do								\
{								\
    if ( (ch)->substate == SUB_RESTRICTED )			\
    {								\
	send_to_char( "You cannot use this command from within another command.\n\r", ch );	\
	return;							\
    }								\
} while(0)

/*
 * Character macros.
 */
#define GET_MAX_STR(CH) ( IS_NPC((ch)) ? 20 : ( 20 + race_table[(ch)->race]->str_plus + \
		( IS_SET((ch)->pcdata->cyber, CYBER_STRENGTH) ? 1 : 0 ) ) )
#define GET_MAX_DEX(CH) ( IS_NPC((ch)) ? 20 : ( 20 + race_table[(ch)->race]->dex_plus + \
		( IS_SET((ch)->pcdata->cyber, CYBER_REFLEXES) ? 1 : 0 ) ) )
#define GET_MAX_INT(CH) ( IS_NPC((ch)) ? 20 : ( 20 + race_table[(ch)->race]->int_plus + \
		( IS_SET((ch)->pcdata->cyber, CYBER_MIND) ? 1 : 0 ) ) )
#define GET_MAX_WIS(CH) ( IS_NPC((ch)) ? 20 : ( 20 + race_table[(ch)->race]->wis_plus ) )
#define GET_MAX_CON(CH) ( IS_NPC((ch)) ? 20 : ( 20 + race_table[(ch)->race]->con_plus ) )
#define GET_MAX_CHA(CH) ( IS_NPC((ch)) ? 20 : ( 20 + race_table[(ch)->race]->cha_plus ) )
#define GET_MAX_LCK(CH) ( IS_NPC((ch)) ? 20 : ( 20 + race_table[(ch)->race]->lck_plus ) )

#define IS_QUESTOR(ch)     (IS_SET((ch)->act, PLR_QUESTOR))
#define IS_NPC(ch)		(IS_SET((ch)->act, ACT_IS_NPC))
#define IS_IMMORTAL(ch)		(get_trust((ch)) >= LEVEL_IMMORTAL)
#define IS_HERO(ch)		(get_trust((ch)) >= 100)
#define IS_AFFECTED(ch, sn)	(IS_SET((ch)->affected_by, (sn)))

#define HAS_BODYPART(ch, part)	((ch)->xflags == 0 || IS_SET((ch)->xflags, (part)))
#define IS_IMMUNE(ch, sn)	(IS_SET((ch)->immune, (sn)))
#define IS_GOOD(ch)		((ch)->alignment >= 350)
#define IS_EVIL(ch)		((ch)->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_MALE(ch)			( (ch)->sex == SEX_MALE ? TRUE : FALSE )
#define IS_FEMALE(ch)		( (ch)->sex == SEX_FEMALE ? TRUE : FALSE )
#define IS_S_NEUTRAL(ch)	( (ch)->sex == SEX_NEUTRAL  ? TRUE : FALSE )

#define IS_AWAKE(ch)		((ch)->position > POS_SLEEPING)

/*#define GET_AC(ch)		( IS_NPC(ch) ? GET_AC_NPC(ch) : GET_AC_CH(ch) )*/

#define GET_AC(ch)	( (ch)->armor + ( IS_AWAKE(ch) ? dex_app[get_curr_dex(ch)].defensive : 0 ) \
				-  ( (ch)->skill_level[COMBAT_ABILITY]/2 ) \
				- ( IS_NPC(ch) ? 0 : ( (!IS_SET(ch->pcdata->cyber , CYBER_CHEST) ? 0 : 15) \
				+ ( race_table[(ch)->race]->ac_plus ) ) ) )


#define GET_AC_CH(ch)		( (ch)->armor + ( IS_AWAKE(ch) ? dex_app[get_curr_dex(ch)].defensive : 0 ) \
				- ( (ch)->race == RACE_DEFEL ? (ch)->skill_level[COMBAT_ABILITY]*2+5 : (ch)->skill_level[COMBAT_ABILITY]/2 ) \
				- ( !IS_SET(ch->pcdata->cyber , CYBER_CHEST) ? 0 : 15 ) ) 

#define GET_AC_NPC(ch)		( (ch)->armor + ( IS_AWAKE(ch) ? dex_app[get_curr_dex(ch)].defensive : 0 ) \
				- ( (ch)->race == RACE_DEFEL ? (ch)->skill_level[COMBAT_ABILITY]*2+5 : (ch)->skill_level[COMBAT_ABILITY]/2 ) )


#define GET_HITROLL(ch)		((ch)->hitroll				    \
				    +str_app[get_curr_str(ch)].tohit	    \
				    +(2-(abs((ch)->mental_state)/10)))
#define GET_DAMROLL(ch)		((ch)->damroll                              \
				    +str_app[get_curr_str(ch)].todam	    \
				    +(((ch)->mental_state > 5		    \
				    &&(ch)->mental_state < 15) ? 1 : 0) )

#define IS_OUTSIDE(ch)		(!xIS_SET(				    \
				    (ch)->in_room->room_flags,		    \
				    ROOM_INDOORS) && !xIS_SET(               \
				    (ch)->in_room->room_flags,              \
				    ROOM_SPACECRAFT) )

#define NO_WEATHER_SECT(sect)  (  sect == SECT_INSIDE || 	           \
				  sect == SECT_UNDERWATER ||               \
                                  sect == SECT_OCEANFLOOR ||               \
                                  sect == SECT_UNDERGROUND )

#define IS_DRUNK(ch, drunk)     (number_percent() < \
			        ( (ch)->pcdata->condition[COND_DRUNK] \
				* 2 / (drunk) ) )

/* Addition to make people with nuisance flag have more wait */

#define WAIT_STATE(ch, npulse) ((ch)->wait=(!IS_NPC(ch)&&ch->pcdata->nuisance&&\
			      (ch->pcdata->nuisance->flags>4))?UMAX((ch)->wait,\
			      (npulse+((ch)->pcdata->nuisance->flags-4)+ \
               		      ch->pcdata->nuisance->power)): \
			      UMAX((ch)->wait, (npulse)))


#define EXIT(ch, door)		( get_exit( (ch)->in_room, door ) )

#define CAN_GO(ch, door)	(EXIT((ch),(door))			 \
				&& (EXIT((ch),(door))->to_room != NULL)  \
                          	&& !IS_SET(EXIT((ch), (door))->exit_info, EX_CLOSED))

#define IS_FLOATING(ch)		( IS_AFFECTED((ch), AFF_FLYING) || IS_AFFECTED((ch), AFF_FLOATING) )


#define IS_VALID_SN(sn)		( (sn) >=0 && (sn) < MAX_SKILL		     \
				&& skill_table[(sn)]			     \
				&& skill_table[(sn)]->name )

#define IS_VALID_HERB(sn)	( (sn) >=0 && (sn) < MAX_HERB		     \
				&& herb_table[(sn)]			     \
				&& herb_table[(sn)]->name )

#define IS_VALID_DISEASE(sn)	( (sn) >=0 && (sn) < MAX_DISEASE	     \
				&& disease_table[(sn)]			     \
				&& disease_table[(sn)]->name )

#define IS_DISGUISE(ch) ( !IS_NPC(ch) && xIS_SET(ch->new_act, ACT_DISGUISE ) )

#define SPELL_FLAG(skill, flag)	( IS_SET((skill)->flags, (flag)) )
#define SPELL_DAMAGE(skill)	( ((skill)->flags     ) & 7 )
#define SPELL_ACTION(skill)	( ((skill)->flags >> 3) & 7 )
#define SPELL_CLASS(skill)	( ((skill)->flags >> 6) & 7 )
#define SPELL_POWER(skill)	( ((skill)->flags >> 9) & 3 )
#define SET_SDAM(skill, val)	( (skill)->flags =  ((skill)->flags & SDAM_MASK) + ((val) & 7) )
#define SET_SACT(skill, val)	( (skill)->flags =  ((skill)->flags & SACT_MASK) + (((val) & 7) << 3) )
#define SET_SCLA(skill, val)	( (skill)->flags =  ((skill)->flags & SCLA_MASK) + (((val) & 7) << 6) )
#define SET_SPOW(skill, val)	( (skill)->flags =  ((skill)->flags & SPOW_MASK) + (((val) & 3) << 9) )

/* Retired and guest imms. */
#define IS_RETIRED(ch) (ch->pcdata && IS_SET(ch->pcdata->flags,PCFLAG_RETIRED))
#define IS_GUEST(ch) (ch->pcdata && IS_SET(ch->pcdata->flags,PCFLAG_GUEST))

/* RIS by gsn lookups. -- Altrag.
   Will need to add some || stuff for spells that need a special GSN. */

#define IS_FIRE(dt)		( IS_VALID_SN(dt) &&			     \
				SPELL_DAMAGE(skill_table[(dt)]) == SD_FIRE )
#define IS_COLD(dt)		( IS_VALID_SN(dt) &&			     \
				SPELL_DAMAGE(skill_table[(dt)]) == SD_COLD )
#define IS_ACID(dt)		( IS_VALID_SN(dt) &&			     \
				SPELL_DAMAGE(skill_table[(dt)]) == SD_ACID )
#define IS_ELECTRICITY(dt)	( IS_VALID_SN(dt) &&			     \
				SPELL_DAMAGE(skill_table[(dt)]) == SD_ELECTRICITY )
#define IS_ENERGY(dt)		( IS_VALID_SN(dt) &&			     \
				SPELL_DAMAGE(skill_table[(dt)]) == SD_ENERGY )

#define IS_DRAIN(dt)		( IS_VALID_SN(dt) &&			     \
				SPELL_DAMAGE(skill_table[(dt)]) == SD_DRAIN )

#define IS_POISON(dt)		( IS_VALID_SN(dt) &&			     \
				SPELL_DAMAGE(skill_table[(dt)]) == SD_POISON )

#define NEW_AUTH( ch )     IS_WAITING_FOR_AUTH( ch )

#define NOT_AUTHED(ch)		(!IS_NPC(ch) && ch->pcdata->auth_state <= 3  \
			      && IS_SET(ch->pcdata->flags, PCFLAG_UNAUTHED) )

#define IS_WAITING_FOR_AUTH(ch) (!IS_NPC(ch) && ch->desc		     \
			      && ch->pcdata->auth_state == 1		     \
			      && IS_SET(ch->pcdata->flags, PCFLAG_UNAUTHED) ) 

/*
 * Object macros.
 */
#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))



/*
 * Description macros.
 */

/*#define PERS(ch, looker)	( can_see( (looker), (ch) ) ?		\
				( IS_NPC(ch) ? (ch)->short_descr	\
				: (ch)->name ) : ( (ch)->sex == SEX_MALE ? "A Male" : (ch)->sex == SEX_FEMALE ? "A Female" : "Someone" ) )*/
#define PERS(ch, looker)	( can_see( (looker), (ch) ) ?		\
				( IS_NPC(ch) ? (ch)->short_descr	\
				: (ch)->full_name ) : ( (ch)->sex == SEX_MALE ? "A Male" : (ch)->sex == SEX_FEMALE ? "A Female" : "Someone" ) )


#define log_string( txt )	( log_string_plus( (txt), LOG_NORMAL, LEVEL_LOG ) )


/*
 *  Defines for the command flags. --Shaddai
 */
#define	CMD_FLAG_POSSESS	BV00
#define CMD_FLAG_POLYMORPHED	BV01
#define CMD_WATCH		BV02	/* FB */

/*
 * Structure for a command in the command lookup table.
 */
struct	cmd_type
{
    CMDTYPE *		next;
    char *		name;
    DO_FUN *		do_fun;
    int                 flags;  /* Added for Checking interpret stuff -Shaddai*/
    sh_int		position;
    sh_int		level;
    sh_int		log;
    struct		timerset	userec;
    int			lag_count;	/* count lag flags for this cmd - FB */
};



/*
 * Structure for a social in the socials table.
 */
struct	social_type
{
    SOCIALTYPE *	next;
    char *		name;
    char *		char_no_arg;
    char *		others_no_arg;
    char *		char_found;
    char *		others_found;
    char *		vict_found;
    char *		char_auto;
    char *		others_auto;
};

/*
 * Structure for an X-social in the xsocials table.
 */
struct	xsocial_type
{
	XSOCIALTYPE *	next;
    char *			name;
    
	char *      	char_no_arg;
    char *      	others_no_arg;
    
	char *      	char_found;
    char *      	others_found;
    char *      	vict_found;
    
	char *      	char_auto;
    char *      	others_auto;
    
	sh_int		genderto;
	sh_int		gender;
    sh_int		stage;
    sh_int		position;
    sh_int		self;
    sh_int		other;
    bool		chance;
};

/*
 * Global constants.
 */
extern  time_t last_restore_all_time;
extern  time_t boot_time;  /* this should be moved down */
extern  HOUR_MIN_SEC * set_boot_time; 
extern  struct  tm *new_boot_time;
extern  time_t new_boot_time_t;

extern	const	struct	str_app_type	str_app		[30];
extern	const	struct	int_app_type	int_app		[30];
extern	const	struct	wis_app_type	wis_app		[30];
extern	const	struct	dex_app_type	dex_app		[30];
extern	const	struct	con_app_type	con_app		[30];
extern	const	struct	cha_app_type	cha_app		[30];
extern  const	struct	lck_app_type	lck_app		[30];

extern	const struct	race_type _race_table	[MAX_RACE];
extern	struct	race_type *	race_table	[MAX_RACE];

extern	const	struct	liq_type	liq_table	[LIQ_MAX];
extern	char *	const			attack_table	[13];
extern	char *  const	        	ability_name	[MAX_ABILITY];
extern	char *  const	        	hair_color  	[MAX_HAIR];
extern	char *  const	        	complex_type  	[MAX_COMPLEX];
extern	char *  const	        	build_type  	[MAX_BUILD];
extern	char *  const	        	eye_color  		[MAX_EYE];

extern	char *  const   cont_flags [];
extern	char *	const	furniture_flags [];
extern	char *	const	skill_tname	[];
extern	sh_int	const	movement_loss	[SECT_MAX];
extern	char *	const	dir_name	[];
extern	char *	const	where_name	[];
extern	const	sh_int	rev_dir		[];
extern	char *	const	r_flags		[];
extern	char *	const	item_w_flags	[];
extern	char *	const	w_flags		[];
extern	char *	const	o_flags		[];
extern	char *	const	a_flags		[];
extern	char *	const	o_types		[];
extern	char *	const	a_types		[];
extern	char *	const	act_flags	[];
extern  char *  const   planet_flags    [];
extern  char *  const   weapon_table    [13];
extern  char *  const   spice_table     [];
extern	char *	const	plr_flags	[];
extern	char *	const	pc_flags	[];
extern	char *	const	ris_flags	[];
extern	char *	const	trig_flags	[];
extern	char *	const	part_flags	[];
extern	char *	const	npc_race	[];
extern	char *	const	defense_flags	[];
extern	char *	const	attack_flags	[];
extern	char *	const	area_flags	[];
extern	char *	const	ship_flags	[];
extern	char *	const	system_flags	[];
extern        int     const   lang_array      [];
extern        char *  const   lang_names      [];

extern	char *	const	temp_settings	[]; /* FB */
extern	char *	const	precip_settings	[];
extern	char *	const	wind_settings	[];
extern	char *	const	preciptemp_msg	[6][6];
extern	char *	const	windtemp_msg	[6][6];
extern	char *	const	precip_msg	[];
extern	char *	const	wind_msg	[];

extern	char *	const	day_name	[];

extern	char * const he_she  [];
extern	char * const him_her [];
extern	char * const his_her [];

/*
 * Global variables.
 */
extern char *   bigregex;
extern char *   preg;

extern	int	numobjsloaded;
extern	int	nummobsloaded;
extern	int	physicalobjects;
extern	int	num_descriptors;
extern	struct	system_data		sysdata;
extern	int	top_sn;
extern	int	top_vroom;
extern	int	top_herb;

extern		CMDTYPE		  *	command_hash	[126];

extern		SKILLTYPE	  *	skill_table	[MAX_SKILL];
extern		SOCIALTYPE	  *	social_index	[27];
extern      XSOCIALTYPE   * xsocial_index   [27];

extern		CHAR_DATA	  *	cur_char;
extern		ROOM_INDEX_DATA	  *	cur_room;
extern		bool			cur_char_died;
extern		ch_ret			global_retcode;
extern		SKILLTYPE	  *	herb_table	[MAX_HERB];
extern		SKILLTYPE	  *	disease_table	[MAX_DISEASE];

extern		int			cur_obj;
extern		int			cur_obj_serial;
extern		bool			cur_obj_extracted;
extern		obj_ret			global_objcode;

extern		HELP_DATA	  *	first_help;
extern		HELP_DATA	  *	last_help;
extern		SHOP_DATA	  *	first_shop;
extern		SHOP_DATA	  *	last_shop;
extern		REPAIR_DATA	  *	first_repair;
extern		REPAIR_DATA	  *	last_repair;



extern SHIP_PROTOTYPE * first_ship_prototype;
extern SHIP_PROTOTYPE * last_ship_prototype;

extern		WATCH_DATA	  *	first_watch;
extern		WATCH_DATA	  *	last_watch;
extern		BAN_DATA	  *	first_ban;
extern		BAN_DATA	  *	last_ban;
extern 		BAN_DATA 	  *     first_ban_class;
extern 		BAN_DATA 	  *	last_ban_class;
extern 		BAN_DATA 	  *	first_ban_race;
extern 		BAN_DATA 	  *	last_ban_race;
extern		RESERVE_DATA	  *	first_reserved;
extern		RESERVE_DATA	  *	last_reserved;
extern		CHAR_DATA	  *	first_char;
extern		CHAR_DATA	  *	last_char;
extern		DESCRIPTOR_DATA   *	first_descriptor;
extern		DESCRIPTOR_DATA   *	last_descriptor;
extern		BOARD_DATA	  *	first_board;
extern		BOARD_DATA	  *	last_board;
extern		OBJ_DATA	  *	first_object;
extern		OBJ_DATA	  *	last_object;
extern		CLAN_DATA	  *	first_clan;
extern		CLAN_DATA	  *	last_clan;
extern 		COUNCIL_DATA 	  *	first_council;
extern		COUNCIL_DATA	  * 	last_council;
extern		GUARD_DATA	  *	first_guard;
extern		GUARD_DATA	  *	last_guard;
extern          SHIP_DATA         *     first_ship;
extern          SHIP_DATA         *     last_ship;
extern          SPACE_DATA        *     first_starsystem;
extern          SPACE_DATA        *     last_starsystem;
extern          PLANET_DATA       *     first_planet;
extern          PLANET_DATA       *     last_planet;
extern          BOUNTY_DATA       *     first_bounty;
extern          BOUNTY_DATA       *     last_bounty;
extern          BOUNTY_DATA       *     first_disintigration;
extern          BOUNTY_DATA       *     last_disintigration;
extern		AREA_DATA	  *	first_area;
extern		AREA_DATA	  *	last_area;
extern		AREA_DATA	  *	first_build;
extern		AREA_DATA	  *	last_build;
extern		AREA_DATA	  *	first_asort;
extern		AREA_DATA	  *	last_asort;
extern		AREA_DATA	  *	first_bsort;
extern		AREA_DATA	  *	last_bsort;
extern          AREA_DATA         *     first_area_name; /*alphanum. sort*/
extern          AREA_DATA         *     last_area_name;  /* Fireblade */

extern		OBJ_DATA	  *	extracted_obj_queue;
extern		EXTRACT_CHAR_DATA *	extracted_char_queue;
extern		OBJ_DATA	  *	save_equipment[MAX_WEAR][MAX_LAYERS];
extern		CHAR_DATA	  *	quitting_char;
extern		CHAR_DATA	  *	loading_char;
extern		CHAR_DATA	  *	saving_char;
extern		OBJ_DATA	  *	all_obj;

extern		char			bug_buf		[];
extern		time_t			current_time;
extern		bool			fLogAll;
extern		FILE *			fpReserve;
extern		FILE *			fpLOG;
extern		char			log_buf		[];
extern		TIME_INFO_DATA		time_info;
extern		WEATHER_DATA		weather_info;
extern		int			weath_unit;
extern		int			rand_factor;
extern		int			climate_factor;
extern		int			neigh_factor;
extern		int			max_vector;

extern          AUCTION_DATA      *     auction;
extern		struct act_prog_data *	mob_act_list;


/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */

/*
 * Sorted Alphabeticly - Gavin
 * Started June 04 2000
 */

DECLARE_DO_FUN( skill_notfound	);
DECLARE_DO_FUN( do_aassign );
DECLARE_DO_FUN( do_abort );
DECLARE_DO_FUN( do_accelerate );
DECLARE_DO_FUN( do_accept );
DECLARE_DO_FUN( do_add_imm_host );
DECLARE_DO_FUN( do_add_patrol );
DECLARE_DO_FUN( do_addbounty );
DECLARE_DO_FUN( do_addpilot );
DECLARE_DO_FUN( do_advance );
DECLARE_DO_FUN( do_affected );
DECLARE_DO_FUN( do_afk );
DECLARE_DO_FUN( do_ahelp );
DECLARE_DO_FUN( do_aid );
DECLARE_DO_FUN( do_alertstatus );
DECLARE_DO_FUN( do_allow );
DECLARE_DO_FUN( do_allships );
DECLARE_DO_FUN( do_ammo );
DECLARE_DO_FUN( do_announce );
DECLARE_DO_FUN( do_ansi );
DECLARE_DO_FUN( do_apply );
DECLARE_DO_FUN( do_appoint );
DECLARE_DO_FUN( do_appraise );
DECLARE_DO_FUN( do_aquest );
DECLARE_DO_FUN( do_areas );
DECLARE_DO_FUN( do_arm );
DECLARE_DO_FUN( do_aset );
DECLARE_DO_FUN( do_astat );
DECLARE_DO_FUN( do_at );
DECLARE_DO_FUN( do_atobj );
DECLARE_DO_FUN( do_auction );
DECLARE_DO_FUN( do_authorize );
DECLARE_DO_FUN( do_autopilot );
DECLARE_DO_FUN( do_autopsy ); /* Added by Gavin Oct 3 2000 */
DECLARE_DO_FUN( do_autorecharge );
DECLARE_DO_FUN( do_autotrack );
DECLARE_DO_FUN( do_avtalk );
DECLARE_DO_FUN( do_backstab );
DECLARE_DO_FUN( do_balzhur );
DECLARE_DO_FUN( do_bamfin );
DECLARE_DO_FUN( do_bamfout );
DECLARE_DO_FUN( do_ban );
DECLARE_DO_FUN( do_bank );
DECLARE_DO_FUN( do_bash );
DECLARE_DO_FUN( do_bashdoor );
DECLARE_DO_FUN( do_beep );
DECLARE_DO_FUN( do_beg );
DECLARE_DO_FUN( do_berserk );
DECLARE_DO_FUN( do_bestow );
DECLARE_DO_FUN( do_bestowarea );
DECLARE_DO_FUN( do_bio );
DECLARE_DO_FUN( do_birth );
DECLARE_DO_FUN( do_bite );
DECLARE_DO_FUN( do_blackjack );
DECLARE_DO_FUN( do_board );
DECLARE_DO_FUN( do_boards );
DECLARE_DO_FUN( do_bodybag );
DECLARE_DO_FUN( do_bomb );
DECLARE_DO_FUN( do_bounties );
DECLARE_DO_FUN( do_breakup );
DECLARE_DO_FUN( do_bribe );
DECLARE_DO_FUN( do_bset );
DECLARE_DO_FUN( do_bstat );
DECLARE_DO_FUN( do_bug );
DECLARE_DO_FUN( do_bugs  );
DECLARE_DO_FUN( do_bury );
DECLARE_DO_FUN( do_butcher );
DECLARE_DO_FUN( do_buy );
DECLARE_DO_FUN( do_buyhome );
DECLARE_DO_FUN( do_buyship );
DECLARE_DO_FUN( do_buyvendor ); /* Added by Gavin - July 18 2000 */
DECLARE_DO_FUN( do_calculate );
DECLARE_DO_FUN( do_call ); /* Added by Gavin - Mar 11 2001 */
DECLARE_DO_FUN( do_capture );
DECLARE_DO_FUN( do_carry ); /* Added by Gavin - August 24 2000 */
DECLARE_DO_FUN( do_cast );
DECLARE_DO_FUN( do_cedit );
DECLARE_DO_FUN( do_chaff );
DECLARE_DO_FUN( do_channels );
DECLARE_DO_FUN( do_chanuser 	);
DECLARE_DO_FUN( do_chat );
DECLARE_DO_FUN( do_check_vnums );
DECLARE_DO_FUN( do_clan_donate );
DECLARE_DO_FUN( do_clan_withdraw );
DECLARE_DO_FUN( do_clanbuyship );
DECLARE_DO_FUN( do_clans );
DECLARE_DO_FUN( do_clansellship );
DECLARE_DO_FUN( do_clantalk );
DECLARE_DO_FUN( do_claw );
DECLARE_DO_FUN( do_climate );
DECLARE_DO_FUN( do_climb );
DECLARE_DO_FUN( do_clone );
DECLARE_DO_FUN( do_clones );
DECLARE_DO_FUN( do_close );
DECLARE_DO_FUN( do_closebay );
DECLARE_DO_FUN( do_closehatch );
DECLARE_DO_FUN( do_cmdtable );
DECLARE_DO_FUN( do_collectgold ); /* Added by Gavin - July 18 2000 */
DECLARE_DO_FUN( do_commands );
DECLARE_DO_FUN( do_comment );
DECLARE_DO_FUN( do_compare );
DECLARE_DO_FUN( do_config );
DECLARE_DO_FUN( do_consent );
DECLARE_DO_FUN( do_consider );
DECLARE_DO_FUN( do_cosmetic	); /* Added by Gavin - Oct 1 2000 */
DECLARE_DO_FUN( do_copyship );
DECLARE_DO_FUN( do_council_induct );
DECLARE_DO_FUN( do_council_outcast );
DECLARE_DO_FUN( do_councils );
DECLARE_DO_FUN( do_credits );
DECLARE_DO_FUN( do_cset );
DECLARE_DO_FUN( do_cyber );
DECLARE_DO_FUN( do_delay ); /* June 04 '00 */
DECLARE_DO_FUN( do_demote );
DECLARE_DO_FUN( do_deny );
DECLARE_DO_FUN( do_description );
DECLARE_DO_FUN( do_destro );
DECLARE_DO_FUN( do_destroy );
DECLARE_DO_FUN( do_diagnose );
DECLARE_DO_FUN( do_dig );
DECLARE_DO_FUN( do_disarm );
DECLARE_DO_FUN( do_disconnect );
DECLARE_DO_FUN( do_disguise );
DECLARE_DO_FUN( do_dismount );
DECLARE_DO_FUN( do_divorce );
DECLARE_DO_FUN( do_dmesg );
DECLARE_DO_FUN( do_dnd );
DECLARE_DO_FUN( do_dock );
DECLARE_DO_FUN( do_down );
DECLARE_DO_FUN( do_drag );
DECLARE_DO_FUN( do_drink );
DECLARE_DO_FUN( do_drive );
DECLARE_DO_FUN( do_drop );
DECLARE_DO_FUN( do_east );
DECLARE_DO_FUN( do_eat );
DECLARE_DO_FUN( do_ech ); /* June 04 '00 */
DECLARE_DO_FUN( do_echo );
DECLARE_DO_FUN( do_elite_guard );
DECLARE_DO_FUN( do_emote );
DECLARE_DO_FUN( do_empower );
DECLARE_DO_FUN( do_empty );
DECLARE_DO_FUN( do_enlist );
DECLARE_DO_FUN( do_enter );
DECLARE_DO_FUN( do_equipment );
DECLARE_DO_FUN( do_examine );
DECLARE_DO_FUN( do_exits );
DECLARE_DO_FUN( do_fill );
DECLARE_DO_FUN( do_fire );
DECLARE_DO_FUN( do_first_aid );
DECLARE_DO_FUN( do_fixchar );
DECLARE_DO_FUN( do_flee );
DECLARE_DO_FUN( do_foldarea );
DECLARE_DO_FUN( do_follow );
DECLARE_DO_FUN( do_for );
DECLARE_DO_FUN( do_force );
DECLARE_DO_FUN( do_forceclose );
DECLARE_DO_FUN( do_form_password );
DECLARE_DO_FUN( do_fprompt	);
DECLARE_DO_FUN( do_fquit );
DECLARE_DO_FUN( do_free_vnums );
DECLARE_DO_FUN( do_freeships	);
DECLARE_DO_FUN( do_freeze );
DECLARE_DO_FUN( do_fullname ); /* Added by Gavin - Sept 09 2000 */
DECLARE_DO_FUN( do_gain );
DECLARE_DO_FUN( do_gather_intelligence );
DECLARE_DO_FUN( do_gemcutting );
DECLARE_DO_FUN( do_gemote ); /* Added By Gavin - June 08 2000 */
DECLARE_DO_FUN( do_get );
DECLARE_DO_FUN( do_getcode );
DECLARE_DO_FUN( do_gfighting );
DECLARE_DO_FUN( do_give );
DECLARE_DO_FUN( do_glance );
DECLARE_DO_FUN( do_gold );
DECLARE_DO_FUN( do_goto );
DECLARE_DO_FUN( do_gouge );
DECLARE_DO_FUN( do_group );
DECLARE_DO_FUN( do_gtell );
DECLARE_DO_FUN( do_gwhere );
DECLARE_DO_FUN( do_hail );
DECLARE_DO_FUN( do_hedit );
DECLARE_DO_FUN( do_hell );
DECLARE_DO_FUN( do_help );
DECLARE_DO_FUN( do_hide );
DECLARE_DO_FUN( do_hijack );
DECLARE_DO_FUN( do_hitall );
DECLARE_DO_FUN( do_hlist );
DECLARE_DO_FUN( do_holylight );
DECLARE_DO_FUN( do_homeid );
DECLARE_DO_FUN( do_hset );
DECLARE_DO_FUN( do_hyperspace );
DECLARE_DO_FUN( do_ide );
DECLARE_DO_FUN( do_idea );
DECLARE_DO_FUN( do_ideas );
DECLARE_DO_FUN( do_ignore );
DECLARE_DO_FUN( do_immortalize );
DECLARE_DO_FUN( do_immtalk );
DECLARE_DO_FUN( do_induct );
DECLARE_DO_FUN( do_info );
DECLARE_DO_FUN( do_inject ); /* March 24 2001 - Gavin */
DECLARE_DO_FUN( do_installarea );
DECLARE_DO_FUN( do_instaroom );
DECLARE_DO_FUN( do_instazone );
DECLARE_DO_FUN( do_inventory );
DECLARE_DO_FUN( do_invis );
DECLARE_DO_FUN( do_invite );
DECLARE_DO_FUN( do_ipcompare ); /* June 04 '00 */
DECLARE_DO_FUN( do_jail );
DECLARE_DO_FUN( do_khistory );
DECLARE_DO_FUN( do_kick );
DECLARE_DO_FUN( do_kill );
DECLARE_DO_FUN( do_knock ); /* July 30 '00 */
DECLARE_DO_FUN( do_land ); 
DECLARE_DO_FUN( do_languages );
DECLARE_DO_FUN( do_last );
DECLARE_DO_FUN( do_launch );
DECLARE_DO_FUN( do_leave );
DECLARE_DO_FUN( do_leaveship ); 
DECLARE_DO_FUN( do_level );
DECLARE_DO_FUN( do_light );
DECLARE_DO_FUN( do_list );
DECLARE_DO_FUN( do_listen );
DECLARE_DO_FUN( do_listships );
DECLARE_DO_FUN( do_litterbug );
DECLARE_DO_FUN( do_loadarea );
DECLARE_DO_FUN( do_loadup );
DECLARE_DO_FUN( do_lock );
DECLARE_DO_FUN( do_log );
DECLARE_DO_FUN( do_look );
DECLARE_DO_FUN( do_low_purge );
DECLARE_DO_FUN( do_mailroom );
DECLARE_DO_FUN( do_make );
DECLARE_DO_FUN( do_makearmor );
DECLARE_DO_FUN( do_makeblade ); 
DECLARE_DO_FUN( do_makeblaster );
DECLARE_DO_FUN( do_makeboard );
DECLARE_DO_FUN( do_makeclan	 );
DECLARE_DO_FUN( do_makecomlink );
DECLARE_DO_FUN( do_makecontainer );
DECLARE_DO_FUN( do_makecouncil );
DECLARE_DO_FUN( do_makedroid );
DECLARE_DO_FUN( do_makefurniture );
DECLARE_DO_FUN( do_makegrenade );
DECLARE_DO_FUN( do_makeguild );
DECLARE_DO_FUN( do_makejewelry );
DECLARE_DO_FUN( do_makekeyring );
DECLARE_DO_FUN( do_makelandmine );
DECLARE_DO_FUN( do_makelight );
DECLARE_DO_FUN( do_makemedkit  ); /* Added By Gavin - August 08 2000 */
DECLARE_DO_FUN( do_make_infect_med ); /* Mar 23 '01 - Gavin */
DECLARE_DO_FUN( do_makemissile );
DECLARE_DO_FUN( do_makeplanet );
DECLARE_DO_FUN( do_makeprototype        ); /* Mar 28 '01 - Gavin */
DECLARE_DO_FUN( do_makerepair );
DECLARE_DO_FUN( do_makeshield );
DECLARE_DO_FUN( do_makeship );
DECLARE_DO_FUN( do_makeshop );
DECLARE_DO_FUN( do_makespice );
DECLARE_DO_FUN( do_makestarsystem );
DECLARE_DO_FUN( do_makewizlist );
DECLARE_DO_FUN( do_marry );
DECLARE_DO_FUN( do_mass_propeganda );
DECLARE_DO_FUN( do_massign );
DECLARE_DO_FUN( do_mcreate );
DECLARE_DO_FUN( do_mdelete );
DECLARE_DO_FUN( do_memory );
DECLARE_DO_FUN( do_mfind );
DECLARE_DO_FUN( do_mine );
DECLARE_DO_FUN( do_minvoke );
DECLARE_DO_FUN( do_mlist );
DECLARE_DO_FUN( do_mount );
DECLARE_DO_FUN( do_mset );
DECLARE_DO_FUN( do_mstat );
DECLARE_DO_FUN( do_murde );
DECLARE_DO_FUN( do_murder );
DECLARE_DO_FUN( do_music );
DECLARE_DO_FUN( do_mwhere );
DECLARE_DO_FUN( do_myclan );
DECLARE_DO_FUN( do_name );
DECLARE_DO_FUN( do_newbiechat );
DECLARE_DO_FUN( do_newbieset );
DECLARE_DO_FUN( do_news );
DECLARE_DO_FUN( do_newzones );
DECLARE_DO_FUN( do_noemote );
DECLARE_DO_FUN( do_noresolve );
DECLARE_DO_FUN( do_north );
DECLARE_DO_FUN( do_northeast );
DECLARE_DO_FUN( do_northwest );
DECLARE_DO_FUN( do_notell );
DECLARE_DO_FUN( do_noteroom );
DECLARE_DO_FUN( do_notitle );
DECLARE_DO_FUN( do_nuisance ); /* June 04 '00 */
DECLARE_DO_FUN( do_oassign );
DECLARE_DO_FUN( do_ocreate );
DECLARE_DO_FUN( do_odelete );
DECLARE_DO_FUN( do_ofind );
DECLARE_DO_FUN( do_oinvoke );
DECLARE_DO_FUN( do_oldscore );
DECLARE_DO_FUN( do_olist );
DECLARE_DO_FUN( do_ooc );
DECLARE_DO_FUN( do_open );
DECLARE_DO_FUN( do_openbay );
DECLARE_DO_FUN( do_openhatch );
DECLARE_DO_FUN( do_order );
DECLARE_DO_FUN( do_order_planet );
DECLARE_DO_FUN( do_oset );
DECLARE_DO_FUN( do_ostat );
DECLARE_DO_FUN( do_outcast );
DECLARE_DO_FUN( do_overthrow );
DECLARE_DO_FUN( do_owhere );
DECLARE_DO_FUN( do_pager );
DECLARE_DO_FUN( do_paint	);
DECLARE_DO_FUN( do_password	);
DECLARE_DO_FUN( do_payfine );
DECLARE_DO_FUN( do_pcrename );
DECLARE_DO_FUN( do_peace );
DECLARE_DO_FUN( do_petname ); /* Added By Gavin - July 14 2000 */
DECLARE_DO_FUN( do_pick );
DECLARE_DO_FUN( do_pickshiplock );
DECLARE_DO_FUN( do_placevendor ); /* Added by Gavin - July 18 2000 */
DECLARE_DO_FUN( do_planets );
DECLARE_DO_FUN( do_pluogus ); 
DECLARE_DO_FUN( do_poison_weapon);
DECLARE_DO_FUN( do_pose );
DECLARE_DO_FUN( do_postguard );
DECLARE_DO_FUN( do_practice );
DECLARE_DO_FUN( do_pricevendor ); /* Added by Gavin - July 18 2000 */
DECLARE_DO_FUN( do_promote );
DECLARE_DO_FUN( do_prompt );
DECLARE_DO_FUN( do_propeganda );
DECLARE_DO_FUN( do_propose );
DECLARE_DO_FUN( do_prototypes );
DECLARE_DO_FUN( do_pshield ); /* Added by Gavin - July 23 2000 */
DECLARE_DO_FUN( do_pull );
DECLARE_DO_FUN( do_punch );
DECLARE_DO_FUN( do_purge );
DECLARE_DO_FUN( do_push );
DECLARE_DO_FUN( do_put );
DECLARE_DO_FUN( do_qpset );
DECLARE_DO_FUN( do_qpstat ); /* June 04 '00 */
DECLARE_DO_FUN( do_quaff );
DECLARE_DO_FUN( do_quest );
DECLARE_DO_FUN( do_qui );
DECLARE_DO_FUN( do_quit );
DECLARE_DO_FUN( do_radar );
DECLARE_DO_FUN( do_rank );
DECLARE_DO_FUN( do_rap );
DECLARE_DO_FUN( do_rassign );
DECLARE_DO_FUN( do_rat );
DECLARE_DO_FUN( do_rcreate ); /* Created By Gavin - May 11 2001 */
DECLARE_DO_FUN( do_rdelete );
DECLARE_DO_FUN( do_reboo );
DECLARE_DO_FUN( do_reboot );
DECLARE_DO_FUN( do_recall ); 
DECLARE_DO_FUN( do_recall );
DECLARE_DO_FUN( do_recharge );
DECLARE_DO_FUN( do_recho );
DECLARE_DO_FUN( do_redit );
DECLARE_DO_FUN( do_refuel );
DECLARE_DO_FUN( do_regoto );
DECLARE_DO_FUN( do_reinforcements );
DECLARE_DO_FUN( do_rembounty );
DECLARE_DO_FUN( do_remove );
DECLARE_DO_FUN( do_rempilot );
DECLARE_DO_FUN( do_renameship );
DECLARE_DO_FUN( do_rent );
DECLARE_DO_FUN( do_repair );
DECLARE_DO_FUN( do_repairset );
DECLARE_DO_FUN( do_repairship );
DECLARE_DO_FUN( do_repairshops );
DECLARE_DO_FUN( do_repairstat );
DECLARE_DO_FUN( do_reply );
DECLARE_DO_FUN( do_report );
DECLARE_DO_FUN( do_repulsor );
DECLARE_DO_FUN( do_rescue );
DECLARE_DO_FUN( do_reserve );
DECLARE_DO_FUN( do_reset );
DECLARE_DO_FUN( do_resetship );
DECLARE_DO_FUN( do_resign );
DECLARE_DO_FUN( do_rest );
DECLARE_DO_FUN( do_restore );
DECLARE_DO_FUN( do_restoretime );
DECLARE_DO_FUN( do_restrict );
DECLARE_DO_FUN( do_retell ); /* June 04 '00 */
DECLARE_DO_FUN( do_retire );
DECLARE_DO_FUN( do_retran );
DECLARE_DO_FUN( do_return );
DECLARE_DO_FUN( do_rlist );
DECLARE_DO_FUN( do_roommate ); /* Added by Gavin - Sept 7 '00 */
DECLARE_DO_FUN( do_rreset );
DECLARE_DO_FUN( do_rstat );
DECLARE_DO_FUN( do_run );/* Added 01/21/01 by Drraagh */
DECLARE_DO_FUN( do_sacrifice );
DECLARE_DO_FUN( do_save );
DECLARE_DO_FUN( do_saveall );
DECLARE_DO_FUN( do_savearea );
DECLARE_DO_FUN( do_say );
DECLARE_DO_FUN( do_scan );
DECLARE_DO_FUN( do_scatter );
DECLARE_DO_FUN( do_score );
DECLARE_DO_FUN( do_scribe );
DECLARE_DO_FUN( do_search );
DECLARE_DO_FUN( do_sedit );
DECLARE_DO_FUN( do_seduce );
DECLARE_DO_FUN( do_sell );
DECLARE_DO_FUN( do_sellhome );
DECLARE_DO_FUN( do_sellship );
DECLARE_DO_FUN( do_sendcode );
DECLARE_DO_FUN( do_set_boot_time );
DECLARE_DO_FUN( do_setblaster );
DECLARE_DO_FUN( do_setclan );
DECLARE_DO_FUN( do_setcode );
DECLARE_DO_FUN( do_setcouncil );
DECLARE_DO_FUN( do_setplanet );
DECLARE_DO_FUN( do_setprototype );
DECLARE_DO_FUN( do_setrace	);
DECLARE_DO_FUN( do_setship );
DECLARE_DO_FUN( do_setstarsystem );
DECLARE_DO_FUN( do_setsystemcode );
DECLARE_DO_FUN( do_setweather ); /* June 04 '00 */
DECLARE_DO_FUN( do_sexes );
DECLARE_DO_FUN( do_ship_masking ); /* Mar 18 2001 - By Gavin */
DECLARE_DO_FUN( do_ships );
DECLARE_DO_FUN( do_shiptalk  );
DECLARE_DO_FUN( do_shops );
DECLARE_DO_FUN( do_shopset );
DECLARE_DO_FUN( do_shopstat );
DECLARE_DO_FUN( do_shout );
DECLARE_DO_FUN( do_shove );
DECLARE_DO_FUN( do_showclan	 );
DECLARE_DO_FUN( do_showcouncil );
DECLARE_DO_FUN( do_showlayers );
DECLARE_DO_FUN( do_showplanet );
DECLARE_DO_FUN( do_showprototype );
DECLARE_DO_FUN( do_showrace	);
DECLARE_DO_FUN( do_showship );
DECLARE_DO_FUN( do_showstarsystem );
DECLARE_DO_FUN( do_showweather );	/* June 04 '00 */
DECLARE_DO_FUN( do_shutdow );
DECLARE_DO_FUN( do_shutdown );
DECLARE_DO_FUN( do_silence );
DECLARE_DO_FUN( do_sit );
DECLARE_DO_FUN(	do_endsimulator	);
DECLARE_DO_FUN( do_sla );
DECLARE_DO_FUN( do_slay );
DECLARE_DO_FUN( do_sleep );
DECLARE_DO_FUN( do_slice );
DECLARE_DO_FUN( do_slist );
DECLARE_DO_FUN( do_slookup );
DECLARE_DO_FUN( do_smalltalk );
DECLARE_DO_FUN( do_smoke );
DECLARE_DO_FUN( do_sneak );
DECLARE_DO_FUN( do_snipe );
DECLARE_DO_FUN( do_snoop );
DECLARE_DO_FUN( do_sober );
DECLARE_DO_FUN( do_socials );
DECLARE_DO_FUN( do_sound );
DECLARE_DO_FUN( do_south );
DECLARE_DO_FUN( do_southeast	 );
DECLARE_DO_FUN( do_southwest );
DECLARE_DO_FUN( do_spacetalk );
DECLARE_DO_FUN( do_speak );
DECLARE_DO_FUN( do_special_forces );
DECLARE_DO_FUN( do_splint ); /* Sept 07 '00 */
DECLARE_DO_FUN( do_split );
DECLARE_DO_FUN( do_sset );
DECLARE_DO_FUN( do_stand );
DECLARE_DO_FUN( do_starsystems ); 
DECLARE_DO_FUN( do_statshield	); /* June 04 '00 */
DECLARE_DO_FUN( do_status );
DECLARE_DO_FUN( do_steal );
DECLARE_DO_FUN( do_sting );
DECLARE_DO_FUN( do_strew );
DECLARE_DO_FUN( do_strip );
DECLARE_DO_FUN( do_stun );
DECLARE_DO_FUN( do_suicide );
DECLARE_DO_FUN( do_supplicate );
DECLARE_DO_FUN( do_switch );
DECLARE_DO_FUN( do_systemtalk );
DECLARE_DO_FUN( do_tail );
DECLARE_DO_FUN( do_takedrug );
DECLARE_DO_FUN( do_talk );
DECLARE_DO_FUN( do_tamp );
DECLARE_DO_FUN( do_target );
DECLARE_DO_FUN( do_teach );
DECLARE_DO_FUN( do_tell );
DECLARE_DO_FUN( do_throw );
DECLARE_DO_FUN( do_time );
DECLARE_DO_FUN( do_timecmd	);
DECLARE_DO_FUN( do_title );
DECLARE_DO_FUN( do_torture );
DECLARE_DO_FUN( do_track );
DECLARE_DO_FUN( do_tractorbeam ); 
DECLARE_DO_FUN( do_train );
DECLARE_DO_FUN( do_trajectory );
DECLARE_DO_FUN( do_transfer );
DECLARE_DO_FUN( do_transship );
DECLARE_DO_FUN( do_trip ); /* Added by Gavin - Sept 09 2000 */
DECLARE_DO_FUN( do_trust );
DECLARE_DO_FUN( do_tune );
DECLARE_DO_FUN( do_typo );
DECLARE_DO_FUN( do_typos );
DECLARE_DO_FUN( do_undock );
DECLARE_DO_FUN( do_unfoldarea );
DECLARE_DO_FUN( do_unhell );
DECLARE_DO_FUN( do_unlock );
DECLARE_DO_FUN( do_unnuisance ); /* June 04 '00 */
DECLARE_DO_FUN( do_unsilence );
DECLARE_DO_FUN( do_up );
DECLARE_DO_FUN( do_upgradeship ); /* Oct 29 2000 - Gavin */
DECLARE_DO_FUN( do_uptime );
DECLARE_DO_FUN( do_use ); 
DECLARE_DO_FUN( do_users );
DECLARE_DO_FUN( do_value );
DECLARE_DO_FUN( do_vandalize	);
DECLARE_DO_FUN( do_vassign );
DECLARE_DO_FUN( do_version ); /* June 04 '00 */
DECLARE_DO_FUN( do_viewskills );
DECLARE_DO_FUN( do_visible );
DECLARE_DO_FUN( do_vnums );
DECLARE_DO_FUN( do_vsearch );
DECLARE_DO_FUN( do_wake );
DECLARE_DO_FUN( do_war );
DECLARE_DO_FUN( do_warn ); /* June 04 '00 */
DECLARE_DO_FUN( do_wartalk );
DECLARE_DO_FUN( do_washup ); /* Mar 22 '01 - Gavin */
DECLARE_DO_FUN( do_watch ); /* June 04 '00 */
DECLARE_DO_FUN( do_wear );
DECLARE_DO_FUN( do_weather );
DECLARE_DO_FUN( do_west );
DECLARE_DO_FUN( do_where );
DECLARE_DO_FUN( do_whisper ); /* June 04 '00 */
DECLARE_DO_FUN( do_who );
DECLARE_DO_FUN( do_whois );
DECLARE_DO_FUN( do_wimpy );
DECLARE_DO_FUN( do_wizhelp );
DECLARE_DO_FUN( do_wizhelp2 );
DECLARE_DO_FUN( do_wizlist );
DECLARE_DO_FUN( do_wizlock );
DECLARE_DO_FUN( do_xsedit ); /* Oct 26 2000 */
DECLARE_DO_FUN( do_xsocials );
DECLARE_DO_FUN( do_yell );
DECLARE_DO_FUN( do_zones );

/* mob prog stuff */
DECLARE_DO_FUN( do_mp_close_passage );
DECLARE_DO_FUN( do_mp_damage );
DECLARE_DO_FUN( do_mp_restore );
DECLARE_DO_FUN( do_mp_open_passage );
DECLARE_DO_FUN( do_mp_practice );
DECLARE_DO_FUN( do_mp_slay);
DECLARE_DO_FUN( do_mpadvance    );
DECLARE_DO_FUN( do_mpasound     );
DECLARE_DO_FUN( do_mpat         );
DECLARE_DO_FUN( do_mpdream	);
DECLARE_DO_FUN( do_mp_deposit	);
DECLARE_DO_FUN( do_mp_withdraw	);
DECLARE_DO_FUN( do_mpecho       );
DECLARE_DO_FUN( do_mprat        );
DECLARE_DO_FUN( do_mpechoaround );
DECLARE_DO_FUN( do_mpechoat     );
DECLARE_DO_FUN( do_mpedit       );
DECLARE_DO_FUN( do_mpforce      );
DECLARE_DO_FUN( do_mpinvis	);
DECLARE_DO_FUN( do_mpgoto       );
DECLARE_DO_FUN( do_mpjunk       );
DECLARE_DO_FUN( do_mpkill       );
DECLARE_DO_FUN( do_mpmload      );
DECLARE_DO_FUN( do_mpmset	);
DECLARE_DO_FUN( do_mpnothing	);
DECLARE_DO_FUN( do_mpoload      );
DECLARE_DO_FUN( do_mposet	);
DECLARE_DO_FUN( do_mppurge      );
DECLARE_DO_FUN( do_mptransfer   );
DECLARE_DO_FUN( do_mpapply	);
DECLARE_DO_FUN( do_mpapplyb  	);
DECLARE_DO_FUN( do_mppkset	);
DECLARE_DO_FUN( do_mpgain	);
DECLARE_DO_FUN( do_mpbodybag    );
DECLARE_DO_FUN( do_mpscatter    );
DECLARE_DO_FUN( do_mpdelay      );
DECLARE_DO_FUN( do_mpnuisance   );
DECLARE_DO_FUN( do_mpunnuisance );


DECLARE_DO_FUN( do_opedit       );
DECLARE_DO_FUN( do_rpedit       );
DECLARE_DO_FUN( do_mpstat       );
DECLARE_DO_FUN( do_opstat       );
DECLARE_DO_FUN( do_rpstat       );
DECLARE_DO_FUN( do_mpcopy	);
DECLARE_DO_FUN( do_rpcopy	);
DECLARE_DO_FUN( do_opcopy	);
DECLARE_DO_FUN(	do_opfind	);
DECLARE_DO_FUN( do_mpfind	);
DECLARE_DO_FUN( do_rpfind	);

/*
 * Spell functions.
 * Defined in magic.c.
 */
DECLARE_SPELL_FUN(	spell_null		);
DECLARE_SPELL_FUN(	spell_notfound		);
DECLARE_SPELL_FUN(	spell_blindness		);
DECLARE_SPELL_FUN(	spell_cause_critical	);
DECLARE_SPELL_FUN(	spell_cause_light	);
DECLARE_SPELL_FUN(	spell_cause_serious	);
DECLARE_SPELL_FUN(	spell_charm_person	);
DECLARE_SPELL_FUN(	spell_cure_blindness	);
DECLARE_SPELL_FUN(	spell_cure_poison	);
DECLARE_SPELL_FUN(	spell_dispel_magic	);
DECLARE_SPELL_FUN(      spell_dream             );
DECLARE_SPELL_FUN(	spell_earthquake	);
DECLARE_SPELL_FUN(	spell_energy_drain	);
DECLARE_SPELL_FUN(	spell_farsight		);
DECLARE_SPELL_FUN(	spell_fireball		);
DECLARE_SPELL_FUN(	spell_flamestrike	);
DECLARE_SPELL_FUN(	spell_harm		);
DECLARE_SPELL_FUN(	spell_identify		);
DECLARE_SPELL_FUN(	spell_know_alignment	);
DECLARE_SPELL_FUN(	spell_lightning_bolt	);
DECLARE_SPELL_FUN(	spell_poison		);
DECLARE_SPELL_FUN(	spell_remove_invis	);
DECLARE_SPELL_FUN(	spell_shocking_grasp	);
DECLARE_SPELL_FUN(	spell_sleep		);
DECLARE_SPELL_FUN(	spell_smaug		);
DECLARE_SPELL_FUN(	spell_ventriloquate	);
DECLARE_SPELL_FUN(	spell_weaken		);
DECLARE_SPELL_FUN(	spell_scorching_surge	);

DECLARE_SPELL_FUN( spell_hand_of_chaos );
DECLARE_SPELL_FUN( spell_black_lightning );


/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if	defined(_AIX)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(apollo)
int	atoi		args( ( const char *string ) );
void *	calloc		args( ( unsigned nelem, size_t size ) );
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(hpux)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(interactive)
#endif

#if	defined(linux)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(MIPS_OS)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(NeXT)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(sequent)
char *	crypt		args( ( const char *key, const char *salt ) );
int	fclose		args( ( FILE *stream ) );
int	fprintf		args( ( FILE *stream, const char *format, ... ) );
int	fread		args( ( void *ptr, int size, int n, FILE *stream ) );
int	fseek		args( ( FILE *stream, long offset, int ptrname ) );
void	perror		args( ( const char *s ) );
int	ungetc		args( ( int c, FILE *stream ) );
#endif

#if	defined(sun)
char *	crypt		args( ( const char *key, const char *salt ) );
int	fclose		args( ( FILE *stream ) );
int	fprintf		args( ( FILE *stream, const char *format, ... ) );
#if 	defined(SYSV)
size_t 	fread		args( ( void *ptr, size_t size, size_t n,
				FILE *stream ) );
#else
int	fread		args( ( void *ptr, int size, int n, FILE *stream ) );
#endif
int	fseek		args( ( FILE *stream, long offset, int ptrname ) );
void	perror		args( ( const char *s ) );
int	ungetc		args( ( int c, FILE *stream ) );
#endif

#if	defined(ultrix)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if	defined(NOCRYPT)
#define crypt(s1, s2)	(s1)
#endif

/*
 * File bits.
 */
#define FILE_PLAYER		      0

/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#define PLAYER_DIR		"../player/"		/* Player files				*/
#define BACKUP_DIR		"../player/backup/" /* Backup Player files		*/
#define GOD_DIR			"../gods/"			/* God Info Dir				*/
#define BOARD_DIR		"../boards/"		/* Board data dir			*/
#define CLAN_DIR		"../clans/"			/* Clan data dir			*/
#define SHIP_DIR		"../space/"
#define SPACE_DIR		"../space/"    
#define PLANET_DIR		"../planets/"  
#define BUILD_DIR		"../building/"		/* Online building save dir	*/
#define SYSTEM_DIR		"../system/"		/* Main system files		*/
#define PROG_DIR		"mudprogs/"			/* MUDProg files			*/
#define CORPSE_DIR		"../corpses/"		/* Corpses					*/
#define COUNCIL_DIR		"../councils/"		/* Council data dir			*/
#define RACEDIR			"../races/"
#define PROTOTYPE_DIR   "../prototypes/"

#ifdef WIN32
  #define NULL_FILE		"nul"				/* To reserve one stream	*/
#else
  #define NULL_FILE		"/dev/null"			/* To reserve one stream	*/
#endif

#define WATCH_DIR		"../watch/"			/* Imm watch files --Gorog	*/
/*
 * The watch directory contains a maximum of one file for each immortal
 * that contains output from "player watches". The name of each file
 * in this directory is the name of the immortal who requested the watch
 */

#define AREA_LIST		"area.lst"		/* List of areas			*/
#define WATCH_LIST      "watch.lst"     /* List of watches			*/
#define BAN_LIST		"ban.lst"       /* List of bans				*/
#define RESERVED_LIST	"reserved.lst"	/* List of reserved names	*/
#define CLAN_LIST		"clan.lst"		/* List of clans			*/
#define COUNCIL_LIST	"council.lst"	/* List of councils			*/
#define SHIP_LIST		"ship.lst"
#define PLANET_LIST		"planet.lst"
#define SPACE_LIST      "space.lst"
#define BOUNTY_LIST     "bounty.lst"      
#define GOD_LIST		"gods.lst"		/* List of gods				*/
#define BOARD_FILE		"boards.txt"	/* For bulletin boards		*/
#define SHUTDOWN_FILE	"shutdown.txt"	/* For 'shutdown'			*/
#define DISINTIGRATION_LIST	"disintigration.lst"
#define PROTOTYPE_LIST  "prototype.lst"

#define HELP_FILE		SYSTEM_DIR "help.txt"   /* For undefined helps */
#define BOOTLOG_FILE	SYSTEM_DIR "boot.txt"	  /* Boot up error file	 */
/*#define BUG_FILE		SYSTEM_DIR "systembugs.txt"*/	  /* For 'bug' and bug( )*/
#define BUG_FILE		NULL_FILE
#define BUG_FILE2		SYSTEM_DIR "bugs.txt"	  /* For 'bug' and bug( )*/
#define IDEA_FILE		SYSTEM_DIR "ideas.txt"	  /* For 'idea'		 */
#define TYPO_FILE		SYSTEM_DIR "typos.txt"	  /* For 'typo'		 */
#define LOG_FILE		SYSTEM_DIR "log.txt"	  /* For talking in logged rooms */
#define BUILD_FILE		SYSTEM_DIR "build.txt"	  /* Builders File */
#define WIZLIST_FILE	SYSTEM_DIR "WIZLIST"	  /* Wizlist		 */
#define WHO_FILE		SYSTEM_DIR "WHO"	  /* Who output file	 */
#define WEBWHO_FILE		SYSTEM_DIR "WEBWHO"	  /* WWW Who output file */
#define REQUEST_PIPE	SYSTEM_DIR "REQUESTS"	  /* Request FIFO	 */

#define SKILL_FILE		SYSTEM_DIR "skills.dat"   /* Skill table	 */
#define HERB_FILE		SYSTEM_DIR "herbs.dat"	  /* Herb table		 */
#define SOCIAL_FILE		SYSTEM_DIR "socials.dat"  /* Socials		 */
#define XSOCIAL_FILE	SYSTEM_DIR "xsocials.dat" /* XSocials		 */
#define COMMAND_FILE	SYSTEM_DIR "commands.dat" /* Commands		 */
#define USAGE_FILE		SYSTEM_DIR "usage.txt"    /* How many people are on 
 						     every half hour - trying to
						     determine best reboot time */
#define BOUNTY_LOG_FILE		SYSTEM_DIR "bounty_log.dat"   /* For bounty logging */

#define TEMP_FILE	SYSTEM_DIR "charsave.tmp" /* More char save protect */
#define	RACE_LIST	"race.lst"	/* List of races		*/

/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD	CHAR_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define OID	OBJ_INDEX_DATA
#define RID	ROOM_INDEX_DATA
#define SF	SPEC_FUN
#define BD	BOARD_DATA
#define CL	CLAN_DATA
#define EDD	EXTRA_DESCR_DATA
#define RD	RESET_DATA
#define ED	EXIT_DATA
#define	ST	SOCIALTYPE
#define	CO	COUNCIL_DATA
#define SK	SKILLTYPE
#define SH      SHIP_DATA


/* uk-additions.c */
char *	stripclr	args( ( char *text ) );

bool	check_parse_name	args( ( char *name, bool newchar ) );

/* act_comm.c */
void    sound_to_room( ROOM_INDEX_DATA *room , char *argument );

bool	circle_follow	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	add_follower	args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void	stop_follower	args( ( CHAR_DATA *ch ) );
void	die_follower	args( ( CHAR_DATA *ch ) );
bool	is_same_group	args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
void	to_channel	args( ( const char *argument, int channel,
				const char *verb, sh_int level ) );
void  	talk_auction    args( ( char *argument ) );
int    knows_language  args( ( CHAR_DATA *ch, int language,
				CHAR_DATA *cch ) );
bool    can_learn_lang  args( ( CHAR_DATA *ch, int language ) );
int     countlangs      args( ( int languages ) );
char *	obj_short	args( ( OBJ_DATA *obj ) );
char *	myobj		args( ( OBJ_DATA *obj ) );

void	talk_channel( CHAR_DATA *ch, char *argument, int channel, const char *verb );

/* act_info.c */
int	get_door	args( ( char *arg ) );
char *  num_punct	args( ( int foo ) );
char *	format_obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch,
				    bool fShort ) );
void	show_list_to_char	args( ( OBJ_DATA *list, CHAR_DATA *ch,
				    bool fShort, bool fShowNothing ) );
void	show_race_line	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_ignoring	args( (CHAR_DATA *ch, CHAR_DATA *ign_ch) );
void	show_preg_line	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

/* act_move.c */
void	clear_vrooms	args( ( void ) );
ED *	find_door	args( ( CHAR_DATA *ch, char *arg, bool quiet ) );
ED *	get_exit	args( ( ROOM_INDEX_DATA *room, sh_int dir ) );
ED *	get_exit_to	args( ( ROOM_INDEX_DATA *room, sh_int dir, int vnum ) );
ED *	get_exit_num	args( ( ROOM_INDEX_DATA *room, sh_int count ) );
ch_ret	move_char	args( ( CHAR_DATA *ch, EXIT_DATA *pexit, int fall ) );
sh_int	encumbrance	args( ( CHAR_DATA *ch, sh_int move ) );
bool	will_fall	args( ( CHAR_DATA *ch, int fall ) );
int     wherehome       args( ( CHAR_DATA *ch ) );

/* act_obj.c */

obj_ret	damage_obj	args( ( OBJ_DATA *obj ) );
sh_int	get_obj_resistance args( ( OBJ_DATA *obj ) );
void    save_clan_storeroom args( ( CHAR_DATA *ch, CLAN_DATA *clan ) );
void    obj_fall  	args( ( OBJ_DATA *obj, bool through ) );

/* act_wiz.c */
bool create_new_race	args( (int index, char *argument ) );
void              close_area    args( ( AREA_DATA *pArea ) );
RID *	find_location	args( ( CHAR_DATA *ch, char *arg ) );
void    echo_to_room    args( ( sh_int AT_COLOR, ROOM_INDEX_DATA *room, char *argument ) );
void	echo_to_all	args( ( sh_int AT_COLOR, char *argument,
				sh_int tar ) );
void   	get_reboot_string args( ( void ) );
struct tm *update_time  args( ( struct tm *old_time ) );
void	free_social	args( ( SOCIALTYPE *social ) );
void	add_social	args( ( SOCIALTYPE *social ) );
void	free_command	args( ( CMDTYPE *command ) );
void	unlink_command	args( ( CMDTYPE *command ) );
void	add_command	args( ( CMDTYPE *command ) );

void	free_xsocial	args( ( XSOCIALTYPE *xsocial ) );
void	add_xsocial	args( ( XSOCIALTYPE *xsocial ) );


/* boards.c */
void	load_boards	args( ( void ) );
BD *	get_board	args( ( OBJ_DATA *obj ) );
void	free_note	args( ( NOTE_DATA *pnote ) );

/* build.c */
void RelCreate( relation_type, void *, void * );
void RelDestroy( relation_type, void *, void * );
char *	flag_string	args( ( int bitvector, char * const flagarray[] ) );
char *	ext_flag_string	args( ( EXT_BV *bitvector, char * const flagarray[] ) );
int     get_cmdflag     args (( char *flag ));
int	get_mpflag	args( ( char *flag ) );
int	get_dir		args( ( char *txt  ) );
char *	strip_cr	args( ( char *str  ) );
int     get_vip_flag    args( ( char *flag ) );
int     get_wanted_flag args( ( char *flag ) );
void	init_area_weather args(( void ) );
void	save_weatherdata args( ( void ) );

/* clans.c */
CL *	get_clan		args( ( char *name ) );
void	load_clans		args( ( void ) );
void	save_clan		args( ( CLAN_DATA *clan ) );
PLANET_DATA *	get_planet	args( ( char *name ) );
void	load_planets		args( ( void ) );
void	save_planet		args( ( PLANET_DATA *planet ) );
long    get_taxes               args( ( PLANET_DATA *planet ) );
void	clear_carry		args( ( CHAR_DATA *ch ) );

/* councils.c */
CO *	get_council	args( ( char *name ) );
void	load_councils	args( ( void ) );
void 	save_council	args( ( COUNCIL_DATA *council ) );

/* bounty.c */
BOUNTY_DATA  * get_disintigration   args( ( char *target ) );
void        load_bounties   args( ( void ) );
void        save_bounties   args( ( void ) );
void        save_disintigrations   args( ( void ) );
void        remove_disintigration   args( ( BOUNTY_DATA *bounty ) );
void	    claim_disintigration    args( ( CHAR_DATA *ch , CHAR_DATA *victim ) );
bool        is_disintigration args( ( CHAR_DATA *victim ) );

/* space.c */
SH        *  get_ship          	    args( ( char *name ) );
/* void         load_ships        	    args( ( void ) );*/
void         save_ship      	    args( ( SHIP_DATA *ship ) );
void         load_space             args( ( void ) );
void         save_starsystem        args( ( SPACE_DATA *starsystem ) );
SPACE_DATA * starsystem_from_name   args( ( char *name ) );
SPACE_DATA * starsystem_from_vnum   args( ( int vnum ) );
SHIP_DATA  * ship_from_obj          args( ( int vnum ) );
SHIP_DATA  * ship_from_entrance     args( ( int vnum ) );
SHIP_DATA  * ship_from_hanger       args( ( int vnum ) );
SHIP_DATA  * ship_from_cockpit      args( ( int vnum ) );
SHIP_DATA  * ship_from_navseat      args( ( int vnum ) );
SHIP_DATA  * ship_from_coseat       args( ( int vnum ) );
SHIP_DATA  * ship_from_pilotseat    args( ( int vnum ) );
SHIP_DATA  * ship_from_gunseat      args( ( int vnum ) );
SHIP_DATA  * ship_from_turret       args( ( int vnum ) );
SHIP_DATA  * ship_from_engine       args( ( int vnum ) );
SHIP_DATA  * get_ship_here          args( ( char *name , SPACE_DATA *starsystem ) );
void         showstarsystem         args( ( CHAR_DATA *ch , SPACE_DATA *starsystem ) );
void         update_space           args( ( void ) );
void         recharge_ships         args( ( void ) );
void         move_ships             args( ( void ) );
void         update_bus             args( ( void ) );
void         update_traffic         args( ( void ) );
bool         check_pilot            args( ( CHAR_DATA *ch , SHIP_DATA *ship ) );
bool         is_rental              args( ( CHAR_DATA *ch , SHIP_DATA *ship ) );
void         echo_to_ship           args( ( int color , SHIP_DATA *ship , char *argument ) );
void         echo_to_cockpit        args( ( int color , SHIP_DATA *ship , char *argument ) );
void         echo_to_system         args( ( int color , SHIP_DATA *ship , char *argument , SHIP_DATA *ignore ) );
bool         extract_ship           args( ( SHIP_DATA *ship ) );
bool         ship_to_room           args( ( SHIP_DATA *ship , int vnum ) );
long         get_ship_value         args( ( SHIP_DATA *ship ) );
bool         rent_ship              args( ( CHAR_DATA *ch , SHIP_DATA *ship ) );
void         damage_ship            args( ( SHIP_DATA *ship , int min , int max ) );
void         damage_ship_ch         args( ( SHIP_DATA *ship , int min , int max , CHAR_DATA *ch ) );
void         destroy_ship           args( ( SHIP_DATA *ship , CHAR_DATA *ch ) );
void         ship_to_starsystem     args( ( SHIP_DATA *ship , SPACE_DATA *starsystem ) );
void         ship_from_starsystem   args( ( SHIP_DATA *ship , SPACE_DATA *starsystem ) );
void         new_missile            args( ( SHIP_DATA *ship , SHIP_DATA *target , CHAR_DATA *ch , int missiletype ) );
void         extract_missile        args( ( MISSILE_DATA *missile ) );
SHIP_DATA * ship_in_room            args( ( ROOM_INDEX_DATA *room, char *name ) ); 
bool		 autofly				args( ( SHIP_DATA *ship ) );

/* morespace.c */
SHIP_PROTOTYPE *  get_ship_prototype        args( ( char *name ) );

/* comm.c */
void	close_socket	args( ( DESCRIPTOR_DATA *dclose, bool force ) );
void	write_to_buffer	args( ( DESCRIPTOR_DATA *d, const char *txt,
				int length ) );
void	write_to_pager	args( ( DESCRIPTOR_DATA *d, const char *txt,
				int length ) );
void	send_to_char	args( ( const char *txt, CHAR_DATA *ch ) );
void	send_to_char_color	args( ( const char *txt, CHAR_DATA *ch ) );
void	send_to_pager	args( ( const char *txt, CHAR_DATA *ch ) );
void	send_to_pager_color	args( ( const char *txt, CHAR_DATA *ch ) );
void	set_char_color  args( ( sh_int AType, CHAR_DATA *ch ) );
void	set_pager_color	args( ( sh_int AType, CHAR_DATA *ch ) );
void	ch_printf	args( ( CHAR_DATA *ch, char *fmt, ... ) );
void	ch_printf_color	args( ( CHAR_DATA *ch, char *fmt, ... ) );
void	pager_printf	args( (CHAR_DATA *ch, char *fmt, ...) );
void	pager_printf_color	args( ( CHAR_DATA *ch, char *fmt, ... ) );
void	log_printf	args( (char *fmt, ...) );
void	act		args( ( sh_int AType, const char *format, CHAR_DATA *ch,
			    const void *arg1, const void *arg2, int type ) );

/* reset.c */
RD  *	make_reset	args( ( char letter, int extra, int arg1, int arg2, int arg3 ) );
RD  *	add_reset	args( ( AREA_DATA *tarea, char letter, int extra, int arg1, int arg2, int arg3 ) );
RD  *	place_reset	args( ( AREA_DATA *tarea, char letter, int extra, int arg1, int arg2, int arg3 ) );
void	reset_area	args( ( AREA_DATA * pArea ) );

/* db.c */
void	show_file	args( ( CHAR_DATA *ch, char *filename ) );
char *	str_dup		args( ( char const *str ) );
void	boot_db		args( ( bool fCopyOver ) );
void	area_update	args( ( void ) );
void	add_char	args( ( CHAR_DATA *ch ) );
CD *	create_mobile	args( ( MOB_INDEX_DATA *pMobIndex ) );
OD *	create_object	args( ( OBJ_INDEX_DATA *pObjIndex, int level ) );
void	clear_char	args( ( CHAR_DATA *ch ) );
void	free_char	args( ( CHAR_DATA *ch ) );
char *	get_extra_descr	args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
MID *	get_mob_index	args( ( int vnum ) );
OID *	get_obj_index	args( ( int vnum ) );
RID *	get_room_index	args( ( int vnum ) );
char	fread_letter	args( ( FILE *fp ) );
int	fread_number	args( ( FILE *fp ) );
EXT_BV  fread_bitvector	args( ( FILE *fp ) );
void	fwrite_bitvector args( ( EXT_BV *bits, FILE *fp ) );
char *	print_bitvector	args( ( EXT_BV *bits ) );
char	fread_letter	args( ( FILE *fp ) );
int	fread_number	args( ( FILE *fp ) );
char *	fread_string	args( ( FILE *fp ) );
char *	fread_string_nohash args( ( FILE *fp ) );
void	fread_to_eol	args( ( FILE *fp ) );
char *	fread_word	args( ( FILE *fp ) );
char *	fread_line	args( ( FILE *fp ) );
int	number_fuzzy	args( ( int number ) );
int	number_range	args( ( int from, int to ) );
int	number_percent	args( ( void ) );
int	number_door	args( ( void ) );
int	number_bits	args( ( int width ) );
int	number_mm	args( ( void ) );
int	dice		args( ( int number, int size ) );
int	interpolate	args( ( int level, int value_00, int value_32 ) );
void	smash_tilde	args( ( char *str ) );
void	hide_tilde	args( ( char *str ) );
char *	show_tilde	args( ( char *str ) );
bool	str_cmp		args( ( const char *astr, const char *bstr ) );
bool	str_prefix	args( ( const char *astr, const char *bstr ) );
bool	str_infix	args( ( const char *astr, const char *bstr ) );
bool	str_suffix	args( ( const char *astr, const char *bstr ) );
char *	capitalize	args( ( const char *str ) );
char *	strlower	args( ( const char *str ) );
char *	strupper	args( ( const char *str ) );
char *  aoran		args( ( const char *str ) );
void	append_file	args( ( CHAR_DATA *ch, char *file, char *str ) );
void	append_to_file	args( ( char *file, char *str ) );
void	bug		args( ( const char *str, ... ) );
void	log_string_plus	args( ( const char *str, sh_int log_type, sh_int level ) );
RID *	make_room	args( ( int vnum ) );
OID *	make_object	args( ( int vnum, int cvnum, char *name ) );
MID *	make_mobile	args( ( int vnum, int cvnum, char *name ) );
ED  *	make_exit	args( ( ROOM_INDEX_DATA *pRoomIndex, ROOM_INDEX_DATA *to_room, sh_int door ) );
void	add_help	args( ( HELP_DATA *pHelp ) );
void	fix_area_exits	args( ( AREA_DATA *tarea ) );
void	load_area_file	args( ( AREA_DATA *tarea, char *filename ) );
void	randomize_exits	args( ( ROOM_INDEX_DATA *room, sh_int maxdir ) );
void	make_wizlist	args( ( void ) );
void	tail_chain	args( ( void ) );
bool    delete_room     args( ( ROOM_INDEX_DATA *room ) );
bool    delete_obj      args( ( OBJ_INDEX_DATA *obj ) );
bool    delete_mob      args( ( MOB_INDEX_DATA *mob ) );
/* Functions to add to sorting lists. -- Altrag */
/*void	mob_sort	args( ( MOB_INDEX_DATA *pMob ) );
void	obj_sort	args( ( OBJ_INDEX_DATA *pObj ) );
void	room_sort	args( ( ROOM_INDEX_DATA *pRoom ) );*/
void	sort_area	args( ( AREA_DATA *pArea, bool proto ) );
void    sort_area_by_name  args( (AREA_DATA *pArea) ); /* Fireblade */

/* build.c */
void	start_editing	args( ( CHAR_DATA *ch, char *data ) );
void	stop_editing	args( ( CHAR_DATA *ch ) );
void	edit_buffer	args( ( CHAR_DATA *ch, char *argument ) );
char *	copy_buffer	args( ( CHAR_DATA *ch ) );
bool	can_rmodify	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room ) );
bool	can_omodify	args( ( CHAR_DATA *ch, OBJ_DATA *obj  ) );
bool	can_mmodify	args( ( CHAR_DATA *ch, CHAR_DATA *mob ) );
bool	can_medit	args( ( CHAR_DATA *ch, MOB_INDEX_DATA *mob ) );
void	free_reset	args( ( AREA_DATA *are, RESET_DATA *res ) );
void	free_area	args( ( AREA_DATA *are ) );
void	assign_area	args( ( CHAR_DATA *ch ) );
EDD *	SetRExtra	args( ( ROOM_INDEX_DATA *room, char *keywords ) );
bool	DelRExtra	args( ( ROOM_INDEX_DATA *room, char *keywords ) );
EDD *	SetOExtra	args( ( OBJ_DATA *obj, char *keywords ) );
bool	DelOExtra	args( ( OBJ_DATA *obj, char *keywords ) );
EDD *	SetOExtraProto	args( ( OBJ_INDEX_DATA *obj, char *keywords ) );
bool	DelOExtraProto	args( ( OBJ_INDEX_DATA *obj, char *keywords ) );
void	fold_area	args( ( AREA_DATA *tarea, char *filename, bool install ) );
int	get_otype	args( ( char *type ) );
int	get_atype	args( ( char *type ) );
int	get_aflag	args( ( char *flag ) );
int	get_oflag	args( ( char *flag ) );
int	get_wflag	args( ( char *flag ) );

/* fight.c */
int	max_fight	args( ( CHAR_DATA *ch ) );
void	violence_update	args( ( void ) );
ch_ret	multi_hit	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
ch_ret	projectile_hit	args( ( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *wield,
			    OBJ_DATA *projectile, sh_int dist ) );
sh_int	ris_damage	args( ( CHAR_DATA *ch, sh_int dam, int ris ) );
ch_ret	damage		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			    int dt ) );
void	update_pos	args( ( CHAR_DATA *victim ) );
void	set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	stop_fighting	args( ( CHAR_DATA *ch, bool fBoth ) );
void	free_fight	args( ( CHAR_DATA *ch ) );
CD *	who_fighting	args( ( CHAR_DATA *ch ) );
void	check_killer	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	check_attacker	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	stop_hunting	args( ( CHAR_DATA *ch ) );
void	stop_hating	args( ( CHAR_DATA *ch ) );
void	stop_fearing	args( ( CHAR_DATA *ch ) );
void	start_hunting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	start_hating	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	start_fearing	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_hunting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_hating	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_fearing	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_safe		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_safe_nm	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	legal_loot	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_illegal_pk args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    raw_kill        args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );   

/* makeobjs.c */
void	make_corpse	args( ( CHAR_DATA *ch, CHAR_DATA *killer ) );
void	make_scraps	args( ( OBJ_DATA *obj ) );
void	make_fire	args( ( ROOM_INDEX_DATA *in_room, sh_int timer) );
OD *	create_money	args( ( int amount ) );
void	make_cum	args( ( CHAR_DATA *ch ) );
void	make_cumstain args( ( CHAR_DATA *ch ) );


/* misc.c */
void actiondesc args( ( CHAR_DATA *ch, OBJ_DATA *obj, void *vo ) );
EXT_BV	meb		args( ( int bit ) );
EXT_BV	multimeb	args( ( int bit, ... ) );


/* mud_comm.c */
char *	mprog_type_to_name	args( ( int type ) );
CHAR_DATA * get_char_room_mp args( ( CHAR_DATA *ch, char *argument ) );

/* mud_prog.c */
#ifdef DUNNO_STRSTR
char *  strstr                  args ( (const char *s1, const char *s2 ) );
#endif

void	mprog_wordlist_check    args ( ( char * arg, CHAR_DATA *mob,
                			CHAR_DATA* actor, OBJ_DATA* object,
					void* vo, int type ) );
void	mprog_percent_check     args ( ( CHAR_DATA *mob, CHAR_DATA* actor,
					OBJ_DATA* object, void* vo,
					int type ) );
void	mprog_act_trigger       args ( ( char* buf, CHAR_DATA* mob,
		                        CHAR_DATA* ch, OBJ_DATA* obj,
					void* vo ) );
void	mprog_bribe_trigger     args ( ( CHAR_DATA* mob, CHAR_DATA* ch,
		                        int amount ) );
void	mprog_entry_trigger     args ( ( CHAR_DATA* mob ) );
void	mprog_give_trigger      args ( ( CHAR_DATA* mob, CHAR_DATA* ch,
                		        OBJ_DATA* obj ) );
void	mprog_greet_trigger     args ( ( CHAR_DATA* mob ) );
void    mprog_fight_trigger     args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    mprog_hitprcnt_trigger  args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    mprog_death_trigger     args ( ( CHAR_DATA *killer, CHAR_DATA* mob ) );
void    mprog_random_trigger    args ( ( CHAR_DATA* mob ) );
void    mprog_speech_trigger    args ( ( char* txt, CHAR_DATA* mob ) );
void    mprog_script_trigger    args ( ( CHAR_DATA *mob ) );
void    mprog_hour_trigger      args ( ( CHAR_DATA *mob ) );
void    mprog_time_trigger      args ( ( CHAR_DATA *mob ) );
void    progbug                 args( ( char *str, CHAR_DATA *mob ) );
void	rset_supermob		args( ( ROOM_INDEX_DATA *room) );
void	release_supermob	args( ( void ) );

/* player.c */
void	set_title		args( ( CHAR_DATA *ch, char *title ) );
void	set_fullname	args( ( CHAR_DATA *ch, char *title ) );


/* skills.c */
bool	check_skill		args( ( CHAR_DATA *ch, char *command, char *argument ) );
void	learn_from_success	args( ( CHAR_DATA *ch, int sn ) );
void	learn_from_failure	args( ( CHAR_DATA *ch, int sn ) );
bool	check_parry		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_dodge		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool 	check_grip		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	disarm			args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
void	trip			args( ( CHAR_DATA *ch, CHAR_DATA *victim) );

/* ban.c */
int add_ban args( ( CHAR_DATA *ch, char *arg1, char *arg2,int time,int type ) );
void show_bans args ( ( CHAR_DATA *ch, int type ) );
void save_banlist args ( ( void ) );
void load_banlist args ( ( void ) );
bool check_total_bans args ( ( DESCRIPTOR_DATA *d ) );
bool check_bans args ( ( CHAR_DATA *ch, int type ) );

/* handler.c */
AREA_DATA *	get_area_obj	args( ( OBJ_INDEX_DATA * obj));
AREA_DATA *	get_area		args( ( char *name ) ); /* FB */
AREA_DATA *	get_area_by_filename args( ( char *name ) ); /* Gavin - March 14 */	
void    explode         args( ( OBJ_DATA *obj ) );
int	get_exp		args( ( CHAR_DATA *ch , int ability ) );
int	get_exp_worth	args( ( CHAR_DATA *ch ) );
int	exp_level	args( ( sh_int level ) );
sh_int	get_trust	args( ( CHAR_DATA *ch ) );
sh_int	get_age		args( ( CHAR_DATA *ch ) );
sh_int	get_curr_str	args( ( CHAR_DATA *ch ) );
sh_int	get_curr_int	args( ( CHAR_DATA *ch ) );
sh_int	get_curr_wis	args( ( CHAR_DATA *ch ) );
sh_int	get_curr_dex	args( ( CHAR_DATA *ch ) );
sh_int	get_curr_con	args( ( CHAR_DATA *ch ) );
sh_int	get_curr_cha	args( ( CHAR_DATA *ch ) );
sh_int  get_curr_lck	args( ( CHAR_DATA *ch ) );
bool	can_take_proto	args( ( CHAR_DATA *ch ) );
int	can_carry_n	args( ( CHAR_DATA *ch ) );
int	can_carry_w	args( ( CHAR_DATA *ch ) );
bool	is_name		args( ( const char *str, char *namelist ) );
bool	is_name_prefix	args( ( const char *str, char *namelist ) );
bool	nifty_is_name	args( ( char *str, char *namelist ) );
bool	nifty_is_name_prefix args( ( char *str, char *namelist ) );
void	affect_modify	args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd ) );
void	affect_to_char	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_remove	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_strip	args( ( CHAR_DATA *ch, int sn ) );
bool	is_affected	args( ( CHAR_DATA *ch, int sn ) );
void	affect_join	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	char_from_room	args( ( CHAR_DATA *ch ) );
void	char_to_room	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
OD *	obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void	obj_from_char	args( ( OBJ_DATA *obj ) );
int	apply_ac	args( ( OBJ_DATA *obj, int iWear ) );
OD *	get_eq_char	args( ( CHAR_DATA *ch, int iWear ) );
void	equip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void	unequip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int	count_obj_list	args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
void	obj_from_room	args( ( OBJ_DATA *obj ) );
OD *	obj_to_room	args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
OD *	obj_to_obj	args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void	obj_from_obj	args( ( OBJ_DATA *obj ) );
void	extract_obj	args( ( OBJ_DATA *obj ) );
void	extract_exit	args( ( ROOM_INDEX_DATA *room, EXIT_DATA *pexit ) );
void	extract_room	args( ( ROOM_INDEX_DATA *room ) );
void	clean_room	args( ( ROOM_INDEX_DATA *room ) );
void	clean_obj	args( ( OBJ_INDEX_DATA *obj ) );
void	clean_mob	args( ( MOB_INDEX_DATA *mob ) );
void	clean_resets	args( ( AREA_DATA *tarea ) );
void	extract_char	args( ( CHAR_DATA *ch, bool fPull ) );
CD *	get_char_room	args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char_world	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_type	args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *	get_obj_list	args( ( CHAR_DATA *ch, char *argument,
			    OBJ_DATA *list ) );
OD *	get_obj_list_rev args( ( CHAR_DATA *ch, char *argument,
			    OBJ_DATA *list ) );
OD *	get_obj_carry	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_wear	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_here	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_world	args( ( CHAR_DATA *ch, char *argument ) );
int	get_obj_number	args( ( OBJ_DATA *obj ) );
int	get_obj_weight	args( ( OBJ_DATA *obj ) );
bool	room_is_dark	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	room_is_private	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
CD	*room_is_dnd	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex) );
bool	can_see		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_see_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	can_hear_chan	args( ( CHAR_DATA *ch, CHAR_DATA *obj ) );
bool	can_drop_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
char *	item_type_name	args( ( OBJ_DATA *obj ) );
char *	affect_loc_name	args( ( int location ) );
char *	affect_bit_name	args( ( int vector ) );
char *	extra_bit_name	args( ( int extra_flags ) );
char *	magic_bit_name	args( ( int magic_flags ) );
char *  cyber_bit_name	args( ( int cyber_flags ) );
void	name_stamp_stats args( ( CHAR_DATA *ch ) );
void	fix_char	args( ( CHAR_DATA *ch ) );
void	showaffect	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	set_cur_obj	args( ( OBJ_DATA *obj ) );
bool	obj_extracted	args( ( OBJ_DATA *obj ) );
void	queue_extracted_obj	args( ( OBJ_DATA *obj ) );
void	clean_obj_queue	args( ( void ) );
void	set_cur_char	args( ( CHAR_DATA *ch ) );
bool	char_died	args( ( CHAR_DATA *ch ) );
void	queue_extracted_char	args( ( CHAR_DATA *ch, bool extract ) );
void	clean_char_queue	args( ( void ) );
void	add_timer	args( ( CHAR_DATA *ch, sh_int type, sh_int count, DO_FUN *fun, int value ) );
TIMER * get_timerptr	args( ( CHAR_DATA *ch, sh_int type ) );
sh_int	get_timer	args( ( CHAR_DATA *ch, sh_int type ) );
void	extract_timer	args( ( CHAR_DATA *ch, TIMER *timer ) );
void	remove_timer	args( ( CHAR_DATA *ch, sh_int type ) );
bool	in_soft_range	args( ( CHAR_DATA *ch, AREA_DATA *tarea ) );
bool	in_hard_range	args( ( CHAR_DATA *ch, AREA_DATA *tarea ) );
bool	chance  	args( ( CHAR_DATA *ch, sh_int percent ) );
OD *	clone_object	args( ( OBJ_DATA *obj ) );
void	split_obj	args( ( OBJ_DATA *obj, int num ) );
void	separate_obj	args( ( OBJ_DATA *obj ) );
bool	empty_obj	args( ( OBJ_DATA *obj, OBJ_DATA *destobj,
				ROOM_INDEX_DATA *destroom ) );
OD *	find_obj	args( ( CHAR_DATA *ch, char *argument,
				bool carryonly ) );
bool	ms_find_obj	args( ( CHAR_DATA *ch ) );
void	worsen_mental_state args( ( CHAR_DATA *ch, int mod ) );
void	better_mental_state args( ( CHAR_DATA *ch, int mod ) );
void	boost_economy	args( ( AREA_DATA *tarea, int gold ) );
void	lower_economy	args( ( AREA_DATA *tarea, int gold ) );
void	economize_mobgold args( ( CHAR_DATA *mob ) );
bool	economy_has	args( ( AREA_DATA *tarea, int gold ) );
void	add_kill	args( ( CHAR_DATA *ch, CHAR_DATA *mob ) );
int	times_killed	args( ( CHAR_DATA *ch, CHAR_DATA *mob ) );
OD *	get_objtype	args( ( CHAR_DATA *ch, sh_int type ) );


/* interp.c */
bool	check_pos	args( ( CHAR_DATA *ch, sh_int position ) );
void	interpret	args( ( CHAR_DATA *ch, char *argument ) );
bool	is_number	args( ( char *arg ) );
int	number_argument	args( ( char *argument, char *arg ) );
char *	one_argument	args( ( char *argument, char *arg_first ) );
char *	one_argument2	args( ( char *argument, char *arg_first ) );
ST *	find_social	args( ( char *command ) );
CMDTYPE *find_command	args( ( char *command ) );
void	hash_commands	args( ( void ) );
void	start_timer	args( ( struct timeval *stime ) );
time_t	end_timer	args( ( struct timeval *stime ) );
void	send_timer	args( ( struct timerset *vtime, CHAR_DATA *ch ) );
void	update_userec	args( ( struct timeval *time_used,
				struct timerset *userec ) );
void	stage_update	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int stage ) );
float	percent			args( (int part1, int part2) );

XSOCIALTYPE * find_xsocial args( ( char *command ) );

/* magic.c */
int	ch_slookup	args( ( CHAR_DATA *ch, const char *name ) );
int	find_spell	args( ( CHAR_DATA *ch, const char *name, bool know ) );
int	find_skill	args( ( CHAR_DATA *ch, const char *name, bool know ) );
int	find_weapon	args( ( CHAR_DATA *ch, const char *name, bool know ) );
int	find_tongue	args( ( CHAR_DATA *ch, const char *name, bool know ) );
int	skill_lookup	args( ( const char *name ) );
int	herb_lookup	args( ( const char *name ) );
int	personal_lookup	args( ( CHAR_DATA *ch, const char *name ) );
int	slot_lookup	args( ( int slot ) );
int	bsearch_skill	args( ( const char *name, int first, int top ) );
int	bsearch_skill_exact args( ( const char *name, int first, int top ) );
bool	saves_poison_death	args( ( int level, CHAR_DATA *victim ) );
bool	saves_wand		args( ( int level, CHAR_DATA *victim ) );
bool	saves_para_petri	args( ( int level, CHAR_DATA *victim ) );
bool	saves_breath		args( ( int level, CHAR_DATA *victim ) );
bool	saves_spell_staff	args( ( int level, CHAR_DATA *victim ) );
ch_ret	obj_cast_spell	args( ( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj ) );
int	dice_parse	args( (CHAR_DATA *ch, int level, char *exp) );
SK *	get_skilltype	args( ( int sn ) );

/* request.c */
void	init_request_pipe	args( ( void ) );
void	check_requests		args( ( void ) );

/* save.c */
/* object saving defines for fread/write_obj. -- Altrag */
#define OS_CARRY	0
#define OS_CORPSE	1
void	save_char_obj	args( ( CHAR_DATA *ch ) );
void	save_clone	args( ( CHAR_DATA *ch , int package ) );
bool	load_char_obj	args( ( DESCRIPTOR_DATA *d, char *name, bool preload ) );
void	set_alarm	args( ( long seconds ) );
void	requip_char	args( ( CHAR_DATA *ch ) );
void    fwrite_obj      args( ( CHAR_DATA *ch,  OBJ_DATA  *obj, FILE *fp, 
				int iNest, sh_int os_type ) );
void	fread_obj	args( ( CHAR_DATA *ch,  FILE *fp, sh_int os_type ) );
void	de_equip_char	args( ( CHAR_DATA *ch ) );
void	re_equip_char	args( ( CHAR_DATA *ch ) );
void	save_home	args( ( CHAR_DATA *ch ) );

void    read_char_mobile args( ( char *argument ) );
void    write_char_mobile args( (  CHAR_DATA *ch, char *argument ) );
CHAR_DATA * fread_mobile args( ( FILE *fp ) );
void    fwrite_mobile	args ( ( FILE *fp, CHAR_DATA *mob ) );
bool	char_exists	args( ( int file_dir, char *argument ) );

/* vendor.c*/
void fwrite_vendor args( ( FILE *fp, CHAR_DATA *mob ) );
CHAR_DATA *  fread_vendor  args ( ( FILE *fp ) );
void load_vendors args( ( void ) );
void save_vendor args (( CHAR_DATA *ch ));
void remove_vendor args (( CHAR_DATA *ch ));

/* shops.c */

/* special.c */
SF *	spec_lookup	args( ( const char *name ) );
char *	lookup_spec	args( ( SPEC_FUN *special ) );

/* tables.c */
int	get_skill	args( ( char *skilltype ) );
char *	spell_name	args( ( SPELL_FUN *spell ) );
char *	skill_name	args( ( DO_FUN *skill ) );
void	load_skill_table args( ( void ) );
void	save_skill_table args( ( void ) );
void	sort_skill_table args( ( void ) );
void	remap_slot_numbers args( ( void ) );
void	load_socials	args( ( void ) );
void	save_socials	args( ( void ) );
void	load_commands	args( ( void ) );
void	save_commands	args( ( void ) );
SPELL_FUN *spell_function args( ( char *name ) );
DO_FUN *skill_function  args( ( char *name ) );
void	load_herb_table	args( ( void ) );
void	save_herb_table	args( ( void ) );
void	load_races	args( ( void ) );
void	load_xsocials	args( ( void ) );
void	save_xsocials	args( ( void ) );

/* track.c */
void	found_prey	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	hunt_victim	args( ( CHAR_DATA *ch) );

/* update.c */
void	advance_level	args( ( CHAR_DATA *ch , int ability ) );
void	gain_exp	args( ( CHAR_DATA *ch, int gain , int ability) );
void	gain_condition	args( ( CHAR_DATA *ch, int iCond, int value ) );
void	update_handler	args( ( void ) );
void	reboot_check	args( ( time_t reset ) );
void    auction_update  args( ( void ) );
void    update_birthday  args( ( void ) );
int max_level( CHAR_DATA *ch, int ability);
void	weather_update	args( ( void ) );
void	update_planet	args( ( void ) );

/* hashstr.c */
char *	str_alloc	args( ( char *str ) );
char *	quick_link	args( ( char *str ) );
int	str_free	args( ( char *str ) );
void	show_hash	args( ( int count ) );
char *	hash_stats	args( ( void ) );
char *	check_hash	args( ( char *str ) );
void	hash_dump	args( ( int hash ) );
void	show_high_hash	args( ( int top ) );

/* newscore.c */
char *  get_race 	args( (CHAR_DATA *ch) );

/* space.c */
void	resetship				args	( ( SHIP_DATA *ship ) );

/* piloting-skills.c */
char *	get_colortext	args( ( char * color ) );

/* gav_info.c */
void	birth_date	args( ( CHAR_DATA *ch ) );

/* medical.c */
void    show_infect_line   args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

#undef	SK
#undef	CO
#undef	ST
#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	SF
#undef	BD 
#undef	CL
#undef	EDD
#undef	RD
#undef	ED

#define SUB_NORTH DIR_NORTH
#define SUB_EAST  DIR_EAST
#define SUB_SOUTH DIR_SOUTH
#define SUB_WEST  DIR_WEST
#define SUB_UP    DIR_UP
#define SUB_DOWN  DIR_DOWN
#define SUB_NE    DIR_NORTHEAST
#define SUB_NW    DIR_NORTHWEST
#define SUB_SE    DIR_SOUTHEAST
#define SUB_SW    DIR_SOUTHWEST

/*
 * defines for use with this get_affect function
 */

#define RIS_000		BV00
#define RIS_R00		BV01
#define RIS_0I0		BV02
#define RIS_RI0		BV03
#define RIS_00S		BV04
#define RIS_R0S		BV05
#define RIS_0IS		BV06
#define RIS_RIS		BV07

#define GA_AFFECTED	BV09
#define GA_RESISTANT	BV10
#define GA_IMMUNE	BV11
#define GA_SUSCEPTIBLE	BV12
#define GA_RIS          BV30


/*
 * mudprograms stuff
 */
extern	CHAR_DATA *supermob;

void oprog_speech_trigger( char *txt, CHAR_DATA *ch );
void oprog_random_trigger( OBJ_DATA *obj );
void oprog_wear_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
bool oprog_use_trigger( CHAR_DATA *ch, OBJ_DATA *obj, 
                        CHAR_DATA *vict, OBJ_DATA *targ, void *vo );
void oprog_remove_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_sac_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_damage_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_repair_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_drop_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_zap_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
char *oprog_type_to_name( int type );

/*
 * MUD_PROGS START HERE
 * (object stuff)
 */
void oprog_greet_trigger( CHAR_DATA *ch );
void oprog_speech_trigger( char *txt, CHAR_DATA *ch );
void oprog_random_trigger( OBJ_DATA *obj );
void oprog_random_trigger( OBJ_DATA *obj );
void oprog_remove_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_sac_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_get_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_damage_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_repair_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_drop_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_examine_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_zap_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_pull_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_push_trigger( CHAR_DATA *ch, OBJ_DATA *obj );


/* mud prog defines */

#define ERROR_PROG        -1
#define IN_FILE_PROG       0
#define ACT_PROG           BV00
#define SPEECH_PROG        BV01
#define RAND_PROG          BV02
#define FIGHT_PROG         BV03
#define RFIGHT_PROG        BV03
#define DEATH_PROG         BV04
#define RDEATH_PROG        BV04
#define HITPRCNT_PROG      BV05
#define ENTRY_PROG         BV06
#define ENTER_PROG         BV06
#define GREET_PROG         BV07
#define RGREET_PROG	   BV07
#define OGREET_PROG        BV07
#define ALL_GREET_PROG	   BV08
#define GIVE_PROG	   BV09
#define BRIBE_PROG	   BV10
#define HOUR_PROG	   BV11
#define TIME_PROG	   BV12
#define WEAR_PROG          BV13  
#define REMOVE_PROG        BV14  
#define SAC_PROG           BV15  
#define LOOK_PROG          BV16  
#define EXA_PROG           BV17  
#define ZAP_PROG           BV18  
#define GET_PROG 	   BV19  
#define DROP_PROG	   BV20  
#define DAMAGE_PROG	   BV21  
#define REPAIR_PROG	   BV22  
#define RANDIW_PROG	   BV23  
#define SPEECHIW_PROG	   BV24  
#define PULL_PROG	   BV25  
#define PUSH_PROG	   BV26  
#define SLEEP_PROG         BV27  
#define REST_PROG          BV28  
#define LEAVE_PROG         BV29
#define SCRIPT_PROG	   BV30
#define USE_PROG           BV31

void rprog_leave_trigger( CHAR_DATA *ch );
void rprog_enter_trigger( CHAR_DATA *ch );
void rprog_sleep_trigger( CHAR_DATA *ch );
void rprog_rest_trigger( CHAR_DATA *ch );
void rprog_rfight_trigger( CHAR_DATA *ch );
void rprog_death_trigger( CHAR_DATA *killer, CHAR_DATA *ch );
void rprog_speech_trigger( char *txt, CHAR_DATA *ch );
void rprog_random_trigger( CHAR_DATA *ch );
void rprog_time_trigger( CHAR_DATA *ch );
void rprog_hour_trigger( CHAR_DATA *ch );
char *rprog_type_to_name( int type );

#define OPROG_ACT_TRIGGER
#ifdef OPROG_ACT_TRIGGER
void oprog_act_trigger( char *buf, OBJ_DATA *mobj, CHAR_DATA *ch,
			OBJ_DATA *obj, void *vo );
#endif
#define RPROG_ACT_TRIGGER
#ifdef RPROG_ACT_TRIGGER
void rprog_act_trigger( char *buf, ROOM_INDEX_DATA *room, CHAR_DATA *ch,
			OBJ_DATA *obj, void *vo );
#endif

#define send_to_char  send_to_char_color
#define send_to_pager send_to_pager_color

/* Structure and macros for using long bit vectors */
#define CHAR_SIZE sizeof(char)

typedef char * LONG_VECTOR;

#define LV_CREATE(vector, bit_length)					\
do									\
{									\
	int i;								\
	CREATE(vector, char, 1 + bit_length/CHAR_SIZE);			\
									\
	for(i = 0; i <= bit_length/CHAR_SIZE; i++)			\
		*(vector + i) = 0;					\
}while(0)

#define LV_IS_SET(vector, index)					\
	(*(vector + index/CHAR_SIZE) & (1 << index%CHAR_SIZE))

#define LV_SET_BIT(vector, index)					\
	(*(vector + index/CHAR_SIZE) |= (1 << index%CHAR_SIZE))

#define LV_REMOVE_BIT(vector, index)					\
	(*(vector + index/CHAR_SIZE) &= ~(1 << index%CHAR_SIZE))
	
#define LV_TOGGLE_BIT(vector, index)					\
	(*(vector + index/CHAR_SIZE) ^= (1 << index%CHAR_SIZE))
	
#ifdef WIN32
void gettimeofday(struct timeval *tv, struct timezone *tz);
void kill_timer();

/* directory scanning stuff */

typedef struct dirent
{
    char *	d_name;
};

typedef struct  
{
    HANDLE		hDirectory;
    WIN32_FIND_DATA	Win32FindData;
    struct dirent	dirinfo;
    char		sDirName[MAX_PATH];
} DIR;


DIR *opendir(char * sDirName);
struct dirent *readdir (DIR * dp);
void closedir(DIR * dp);

/* --------------- Stuff for Win32 services ------------------ */
/*

   NJG:

   When "exit" is called to handle an error condition, we really want to
   terminate the game thread, not the whole process.

 */

#define exit(arg) Win32_Exit(arg)
void Win32_Exit(int exit_code);

#endif

