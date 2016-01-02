/***************************************************************************
*                           STAR WARS UNKNOWN REGIONS                      *
*--------------------------------------------------------------------------*
* Star Wars Unknown Regions Code Addtions and changes from the SWR Code    *
* copyright (c) 2000 by Gavin Mogan                                        *
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
*                       Homes V1.0 header information			           *
****************************************************************************/

#define HALK_HOMES

typedef struct  home_data               HOME_DATA;
extern          HOME_DATA         *     first_home;
extern          HOME_DATA         *     last_home;
extern	char *	const	home_flags		[];

typedef enum {APARTMENT_HOME, MIDSIZE_HOME, GIANT_HOME} home_classes;
typedef enum {MOB_HOME, PLAYER_HOME, EMPTY_HOME} home_types;
#define MAX_HOMES                1000

#define HOMEDIR		"../homes/"
#define HOME_LIST       "homes.lst"

/* homes.c */
HOME_DATA * get_home			args( ( char *name		) );
HOME_DATA *	home_from_vnum		args( ( int vnum		) );
HOME_DATA *	home_from_entrance	args( ( int vnum		) );

void		save_home2			args( ( HOME_DATA *home ) );
void		write_home_list		args( ( void			) );
void		load_homes			args( ( void			) );
void		fread_home			args( ( HOME_DATA *home, FILE *fp ) );
void		echo_to_home		args( ( int color , HOME_DATA *home , char *argument ) );
void		resethome			args( ( HOME_DATA *home ) );
void		load_homes			args( ( void			) );
void		fread_obj_home		args( ( HOME_DATA *home, FILE *fp, sh_int os_type ) );
void		fwrite_obj_home		args( ( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest, sh_int os_type ) );

long		get_home_value      args( ( HOME_DATA *home ) );
int			get_homeflag		args( ( char *flag		) );

bool		load_home_file		args( ( char *homefile	) );
bool		extract_home		args( ( HOME_DATA *home	) );
bool		check_member		args( ( CHAR_DATA *ch , HOME_DATA *home ) );
bool		is_hotel			args( ( CHAR_DATA *ch , HOME_DATA *home ) );
bool		home_to_room		args( ( HOME_DATA *home , int vnum ) );

struct home_data
{
    HOME_DATA * next;
    HOME_DATA * prev;
    HOME_DATA * next_in_room;
    HOME_DATA * prev_in_room;
    ROOM_INDEX_DATA *in_room;
    char *      filename;
    char *      name;
    char *      description;
    char *      owner;
    char *      pilot;
    char *      copilot;
	char *		origname;
    sh_int      type;
    sh_int      class;
	int			flags;
    int         firstroom;
    int         lastroom;
	int			entrance;
	long int	price;
};

DECLARE_DO_FUN(do_homes);
DECLARE_DO_FUN(do_makehome);
DECLARE_DO_FUN(do_showhome);
DECLARE_DO_FUN(do_sethome);
DECLARE_DO_FUN(do_resethome);
DECLARE_DO_FUN(do_councilbuyhome);
DECLARE_DO_FUN(do_councilsellhome);
DECLARE_DO_FUN(do_mxp);
DECLARE_DO_FUN(do_freehomes);

