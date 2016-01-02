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
*			   Wizard/god command module      		   *
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "mud.h"
#ifdef WIN32
   #include <io.h>
   #define F_OK 0
#endif

#ifndef RESTORE_ADDON
#define RESTORE_INTERVAL 21600
#endif

char * const save_flag[] =
{ "death", "kill", "passwd", "drop", "put", "give", "auto", "zap",
"auction", "get", "receive", "idle", "backup", "r13", "r14", "r15", "r16",
"r17", "r18", "r19", "r20", "r21", "r22", "r23", "r24", "r25", "r26", "r27",
"r28", "r29", "r30", "r31" };

/* from reset.c */
int generate_itemlevel  args ( ( AREA_DATA *pArea, OBJ_INDEX_DATA *pObjIndex ));

/* from comm.c */
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
void	pager_printf_color	args( (CHAR_DATA *ch, char *fmt, ...) );
bool     check_parse_name        args( ( char *name, bool newchar ) );

/* from build.c */
int     get_risflag( char *flag );
int get_defenseflag( char *flag );
int  get_attackflag( char *flag );
int get_langflag( char *flag );

/* from tables.c */
void	write_race_file( int ra );


/*
 * Local functions.
 */
ROOM_INDEX_DATA * find_location	args( ( CHAR_DATA *ch, char *arg ) );
void              save_watchlist  args( ( void ) );
void              save_banlist  args( ( void ) );
void              close_area    args( ( AREA_DATA *pArea ) );

int               get_color (char *argument); /* function proto */

void		  sort_reserved	args( ( RESERVE_DATA *pRes ) );

/*
 * Global variables.
 */

char reboot_time[50];
time_t new_boot_time_t;
extern struct tm new_boot_struct;
extern OBJ_INDEX_DATA *obj_index_hash[MAX_KEY_HASH];
extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];

int get_saveflag( char *name )
{
    int x;

    for ( x = 0; x < sizeof(save_flag) / sizeof(save_flag[0]); x++ )
      if ( !str_cmp( name, save_flag[x] ) )
        return x;
    return -1;
}

/*
 * Toggle "Do Not Disturb" flag. Used to prevent lower level imms from
 * using commands like "trans" and "goto" on higher level imms.
 */
void do_dnd( CHAR_DATA *ch, char *argument )
{
   if ( !IS_NPC(ch) && ch->pcdata )
      if ( IS_SET(ch->pcdata->flags, PCFLAG_DND) )
      {
          REMOVE_BIT(ch->pcdata->flags, PCFLAG_DND);
          send_to_char( "Your 'do not disturb' flag is now off.\n\r", ch );
      }
      else
      {
          SET_BIT(ch->pcdata->flags, PCFLAG_DND);
          send_to_char( "Your 'do not disturb' flag is now on.\n\r", ch );
      }
   else
   send_to_char( "huh?\n\r", ch );
}

/*
 * The "watch" facility allows imms to specify the name of a player or
 * the name of a site to be watched. It is like "logging" a player except
 * the results are written to a file in the "watch" directory named with
 * the same name as the imm. The idea is to allow lower level imms to 
 * watch players or sites without having to have access to the log files.
 */
void do_watch( CHAR_DATA *ch, char *argument )
{
char arg[MAX_INPUT_LENGTH];
char arg2[MAX_INPUT_LENGTH];
char arg3[MAX_INPUT_LENGTH];
WATCH_DATA *pw;

if ( IS_NPC(ch) )
     return;

argument = one_argument( argument, arg  );
set_pager_color( AT_IMMORT, ch );

if ( arg[0] == '\0' || !str_cmp(arg, "help") )
{
   send_to_pager("Syntax Examples:\n\r", ch);
   /*
    * Only IMP+ can see all the watches. The rest can just see their own.
    */
   if ( get_trust(ch) >= LEVEL_IMPLEMENTOR )
      send_to_pager(
       "   watch show all          show all watches\n\r", ch);
   send_to_pager(
       "   watch show              show all my watches\n\r"
       "   watch size              show the size of my watch file\n\r"
       "   watch player joe        add a new player watch\n\r"
       "   watch site 2.3.123      add a new site watch\n\r"
       "   watch command make      add a new command watch\n\r"
       "   watch site 2.3.12       matches 2.3.12x\n\r"
       "   watch site 2.3.12.      matches 2.3.12.x\n\r"
       "   watch delete n          delete my nth watch\n\r"
       "   watch print 500         print watch file starting at line 500\n\r"
       "   watch print 500 1000    print 1000 lines starting at line 500\n\r"
       "   watch clear             clear my watch file\n\r",
           ch);
   return;
}
    
set_pager_color( AT_PLAIN, ch );
argument = one_argument(argument, arg2);
argument = one_argument(argument, arg3);

/*
 * Clear watch file
 */
if ( !str_cmp(arg, "clear") )
{
   char fname[MAX_INPUT_LENGTH];

   sprintf( fname, "%s%s", WATCH_DIR, strlower( ch->name ) );
   if ( 0==remove( fname ) )
   {
      send_to_pager( "Ok. Your watch file has been cleared.\n\r", ch);
      return;
   }
   send_to_pager( "You have no valid watch file to clear.\n\r", ch);
   return;
}

/*
 * Display size of watch file
 */
if ( !str_cmp(arg, "size") )
{
   FILE *fp;
   char fname[MAX_INPUT_LENGTH], s[MAX_STRING_LENGTH];
   int rec_count=0;

   sprintf( fname, "%s%s", WATCH_DIR, strlower( ch->name ) );

   if ( !(fp = fopen( fname, "r" )) )
   {
      send_to_pager(
          "You have no watch file. Perhaps you cleared it?\n\r", ch );
      return;
   }

   fgets( s, MAX_STRING_LENGTH, fp );
   while( !feof(fp) )
   { 
       rec_count++;
       fgets( s, MAX_STRING_LENGTH, fp );
   }
   pager_printf( ch, "You have %d lines in your watch file.\n\r", rec_count);
   fclose(fp);
   return;
}

/*
 * Print watch file
 */
if ( !str_cmp(arg, "print") )
{
   FILE *fp;
   char fname[MAX_INPUT_LENGTH], s[MAX_STRING_LENGTH];
   const int MAX_DISPLAY_LINES = 1000;
   int start, limit, disp_count=0, rec_count=0;

   if ( arg2[0] == '\0' )
   {
      send_to_pager(
          "Sorry. You must specify a starting line number.\n\r", ch);
      return;
   }

   start = atoi(arg2);
   limit = (arg3[0]=='\0') ? MAX_DISPLAY_LINES : atoi(arg3);
   limit = UMIN( limit, MAX_DISPLAY_LINES);
 
   sprintf( fname, "%s%s", WATCH_DIR, strlower( ch->name ) );
   if ( !(fp = fopen( fname, "r" )) )
      return;
   fgets( s, MAX_STRING_LENGTH, fp );

   while( (disp_count<limit) && (!feof(fp)) )
   {
      if ( ++rec_count >= start )
      {
         send_to_pager( s, ch );
         disp_count++;
      }
      fgets( s, MAX_STRING_LENGTH, fp );
   }
   send_to_pager( "\n\r", ch );
   if ( disp_count >= MAX_DISPLAY_LINES )
      send_to_pager(
          "Maximum display lines exceeded. List is terminated.\n\r"
          "Type 'help watch' to see how to print the rest of the list.\n\r"
          "\n\r"
          "Your watch file is large. Perhaps you should clear it?\n\r", ch);

   fclose(fp);
   return;
}

/*
 * Display all watches
 * Only IMP+ can see all the watches. The rest can just see their own.
 */
if ( get_trust(ch) >= LEVEL_IMPLEMENTOR
&&   !str_cmp(arg, "show") && !str_cmp(arg2, "all") )
{
   pager_printf( ch, "%-12s %-14s %-15s\n\r",
      "Imm Name", "Player/Command", "Player Site");
   if (first_watch)
      for ( pw = first_watch; pw; pw = pw->next )
          if ( get_trust(ch) >= pw->imm_level )
             pager_printf( ch, "%-14s %-12s %-15s\n\r",
                  pw->imm_name, pw->target_name?pw->target_name:" ", 
		  pw->player_site?pw->player_site:" " );
   return;
}

/*
 * Display only those watches belonging to the requesting imm 
 */
if ( !str_cmp(arg, "show") && arg2[0]=='\0'  )
{
   int cou = 0;
   pager_printf( ch, "%-3s %-12s %-14s %-15s\n\r",
     " ", "Imm Name", "Player/Command", "Player Site");
   if (first_watch)
      for ( pw = first_watch; pw; pw = pw->next )
          if ( !str_cmp(ch->name, pw->imm_name) )
             pager_printf( ch, "%3d %-12s %-14s %-15s\n\r",
                 ++cou, pw->imm_name, pw->target_name?pw->target_name:" ", 
		 pw->player_site?pw->player_site:" " );
   return;
}

/*
 * Delete a watch belonging to the requesting imm
 */
if ( !str_cmp(arg, "delete") && isdigit(*arg2) )
{
   int cou = 0;
   int num;

   num = atoi(arg2);
   if (first_watch)
      for ( pw = first_watch; pw; pw = pw->next )
          if ( !str_cmp(ch->name, pw->imm_name) )
             if ( num == ++cou )
             {
                /* Oops someone forgot to clear up the memory --Shaddai */
                if ( pw->imm_name ) DISPOSE ( pw->imm_name );
                if ( pw->player_site ) DISPOSE( pw->player_site );
                if ( pw->target_name ) DISPOSE( pw->target_name );
                /* Now we can unlink and then clear up that final
                 * pointer -- Shaddai 
                 */
                UNLINK( pw, first_watch, last_watch, next, prev );
                DISPOSE( pw );
                save_watchlist( );
                send_to_pager ("Deleted.\n\r", ch);
                return;
             }
   send_to_pager ("Sorry. I found nothing to delete.\n\r", ch);
   return;
}

/*
 * Watch a specific player
 */
if ( !str_cmp(arg, "player") && *arg2 )
{
   WATCH_DATA *pinsert;
   CHAR_DATA  *vic;
   char buf [MAX_INPUT_LENGTH];

   if (first_watch)                            /* check for dups */
      for ( pw = first_watch; pw; pw = pw->next )
          if ( !str_cmp(ch->name, pw->imm_name)
	  && pw->target_name
          &&   !str_cmp(arg2    , pw->target_name) )
          {
             send_to_pager("You are already watching that player.\n\r", ch);
             return;
          }

   CREATE( pinsert, WATCH_DATA, 1 );           /* create new watch */
   pinsert->imm_level    = get_trust(ch);
   pinsert->imm_name     = str_dup( strlower( ch->name ) );
   pinsert->target_name  = str_dup( strlower( arg2 ) );
   pinsert->player_site  = NULL;

  /* stupid get_char_world returns ptr to "samantha" when given "sam" */
  /* so I do a str_cmp to make sure it finds the right player --Gorog */

   sprintf( buf, "0.%s", arg2 );
   if ( (vic = get_char_world( ch, buf)) )    /* if vic is in game now */
      if ( (!IS_NPC(vic)) && !str_cmp(arg2, vic->name) )
         SET_BIT( vic->pcdata->flags, PCFLAG_WATCH );

   if (first_watch)                            /* ins new watch if app */
      for ( pw = first_watch; pw; pw = pw->next )
          if ( strcmp(pinsert->imm_name, pw->imm_name) < 0 )
          {
             INSERT( pinsert, pw, first_watch, next, prev );
             save_watchlist( );
             send_to_pager(
                "Ok. That player will be watched.\n\r", ch);
             return;
          }

   LINK( pinsert, first_watch, last_watch, next, prev );  /* link new watch */
   save_watchlist( );
   send_to_pager("Ok. That player will be watched.\n\r", ch);
   return;
}

/*
 * Watch a specific site
 */
if ( !str_cmp(arg, "site") && *arg2 )
{
   WATCH_DATA *pinsert;
   CHAR_DATA  *vic;

   if (first_watch)                            /* check for dups */
      for ( pw = first_watch; pw; pw = pw->next )
          if ( !str_cmp(ch->name, pw->imm_name)
	  &&   pw->player_site
          &&   !str_cmp(arg2    , pw->player_site) )
          {
             send_to_pager("You are already watching that site.\n\r", ch);
             return;
          }

   CREATE( pinsert, WATCH_DATA, 1 );           /* create new watch */
   pinsert->imm_level    = get_trust(ch);
   pinsert->imm_name     = str_dup( strlower( ch->name ) );
   pinsert->player_site  = str_dup( strlower( arg2 ) );
   pinsert->target_name  = NULL;

   for ( vic = first_char; vic; vic = vic->next )
       if ( !IS_NPC(vic) && vic->desc && *pinsert->player_site
       && !str_prefix( pinsert->player_site, vic->desc->host )
       && get_trust(vic) < pinsert->imm_level )
          SET_BIT( vic->pcdata->flags, PCFLAG_WATCH );

   if (first_watch)                            /* ins new watch if app */
      for ( pw = first_watch; pw; pw = pw->next )
          if ( strcmp(pinsert->imm_name, pw->imm_name) < 0 )
          {
             INSERT( pinsert, pw, first_watch, next, prev );
             save_watchlist( );
             send_to_pager(
                "Ok. That site will be watched.\n\r", ch);
             return;
          }

   LINK( pinsert, first_watch, last_watch, next, prev );
   save_watchlist( );
   send_to_pager("Ok. That site will be watched.\n\r", ch);
   return;
}

/*
 * Watch a specific command - FB
 */
if(!str_cmp(arg, "command") && *arg2)
{
	WATCH_DATA *pinsert;
	CMDTYPE *cmd;
	bool found = FALSE;
	
	for(pw = first_watch; pw; pw = pw->next)
	{
		if(!str_cmp(ch->name, pw->imm_name)
			&& pw->target_name
			&& !str_cmp(arg2, pw->target_name))
		{
			send_to_pager("You are already watching that command.\n\r", ch);
			return;
		}
	}
	
	for(cmd = command_hash[LOWER(arg2[0])%126]; cmd; cmd = cmd->next )
	{
		if(!strcmp(arg2, cmd->name))
		{
			found = TRUE;
			break;
		}
	}
	
	if(!found)
	{
		send_to_pager("No such command exists.\n\r", ch);
		return;
	}
	else
	{
		SET_BIT(cmd->flags, CMD_WATCH);
	}
	
	CREATE(pinsert, WATCH_DATA, 1);
	pinsert->imm_level = get_trust(ch);
	pinsert->imm_name = str_dup(strlower(ch->name));
	pinsert->player_site = NULL;
	pinsert->target_name = str_dup(arg2);

	for(pw = first_watch; pw; pw = pw->next)
	{
		if(strcmp(pinsert->imm_name, pw->imm_name) < 0)
		{
			INSERT(pinsert, pw, first_watch, next, prev);
			save_watchlist();
			send_to_pager("Ok, That command will be watched.\n\r", ch);
			return;
		}
	}
	
	LINK(pinsert, first_watch, last_watch, next, prev);
	save_watchlist();
	send_to_pager("Ok. That site will be watched.\n\r", ch);
	return;
}

send_to_pager("Sorry. I can't do anything with that. "
              "Please read the help file.\n\r", ch);
return;
}

void do_wizhelp( CHAR_DATA *ch, char *argument )
{
    CMDTYPE * cmd;
    int col, hash;

    col = 0;
    set_pager_color( AT_PLAIN, ch );
    for ( hash = 0; hash < 126; hash++ )
	for ( cmd = command_hash[hash]; cmd; cmd = cmd->next )
	    if ( cmd->level >= LEVEL_HERO && cmd->level <= get_trust( ch ) )
			{
				pager_printf( ch, "&G[&W%d&G] &W%-14s", cmd->level, cmd->name );
			if ( ++col % 5 == 0 )
				send_to_pager( "\n\r", ch );
			}
    if ( col % 5 != 0 )
	send_to_pager( "\n\r", ch );
    return;
}

void do_wizhelp2( CHAR_DATA *ch, char *argument )
{
	CMDTYPE * cmd;
    int col, hash;
	int level2;
	char buf[MAX_STRING_LENGTH];

    level2 = 0;
    col = 0;
    set_pager_color( AT_PLAIN, ch );
	send_to_pager( "\n\r", ch );
	for ( level2 = LEVEL_AVATAR ; level2 <= LEVEL_SUPREME ; level2++ )
	{
		sprintf( buf, "\n\r&W--------------------------[ &BLEVEL %d ]&W--------------------------",
			level2 );
		col = 0;
		send_to_char(buf,ch);
		send_to_pager( "\n\r", ch );

		for ( hash = 0; hash < 126; hash++ )
		{
			for ( cmd = command_hash[hash]; cmd; cmd = cmd->next )
			{
				if ( cmd->level >= LEVEL_HERO && cmd->level <= get_trust( ch ) && cmd->level == level2 )
				{
					pager_printf( ch, "%-10.10s ", cmd->name );
					if ( ++col % 6 == 0 )
						send_to_pager( "\n\r", ch );
				}
			}
		}
	}
	if ( col % 6 != 0 )
		send_to_pager( "\n\r", ch );
	return;
}

void do_restrict( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    sh_int level, hash;
    CMDTYPE *cmd;
    bool found;

    found = FALSE;
	set_char_color( AT_IMMORT, ch );

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Restrict which command?\n\r", ch );
	return;
    }

    argument = one_argument ( argument, arg2 );
    if ( arg2[0] == '\0' )
      level = get_trust( ch );
    else
      level = atoi( arg2 );

    level = UMAX( UMIN( get_trust( ch ), level ), 0 );

    hash = arg[0] % 126;
    for ( cmd = command_hash[hash]; cmd; cmd = cmd->next )
    {
	if ( !str_prefix( arg, cmd->name )
	&&    cmd->level <= get_trust( ch ) )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( found )
    {
    	if ( !str_prefix( arg2, "show" ) )
    	{
    		sprintf(buf, "%s show", cmd->name);
    		do_cedit(ch, buf);
/*    		ch_printf( ch, "%s is at level %d.\n\r", cmd->name, cmd->top_level );*/
    		return;
    	}
	cmd->level = level;
	ch_printf( ch, "You restrict %s to level %d\n\r",
	   cmd->name, level );
	sprintf( buf, "%s restricting %s to level %d",
	     ch->name, cmd->name, level );
	log_string( buf );
    }
    else
    	send_to_char( "You may not restrict that command.\n\r", ch );

    return;
}

/* 
 * Check if the name prefix uniquely identifies a char descriptor
 */ 
CHAR_DATA *get_waiting_desc( CHAR_DATA *ch, char *name ) 
{ 
  DESCRIPTOR_DATA *d; 
  CHAR_DATA       *ret_char; 
  static unsigned int number_of_hits; 
  
  number_of_hits = 0; 
  for ( d = first_descriptor; d; d = d->next ) 
    { 
    if ( d->character && (!str_prefix( name, d->character->name )) &&
         IS_WAITING_FOR_AUTH(d->character) )
      { 
      if ( ++number_of_hits > 1 ) 
         { 
         ch_printf( ch, "%s does not uniquely identify a char.\n\r", name ); 
         return NULL; 
         } 
      ret_char = d->character;       /* return current char on exit */
      } 
    } 
  if ( number_of_hits==1 ) 
     return ret_char; 
  else 
     { 
     send_to_char( "No one like that waiting for authorization.\n\r", ch ); 
     return NULL; 
     } 
} 
/* 02-07-99  New auth messages --Mystaric */
void do_authorize( CHAR_DATA *ch, char *argument )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	DESCRIPTOR_DATA *d;
	
	set_char_color( AT_LOG, ch );
	
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	if ( arg1[0] == '\0' )
	{
		send_to_char( "Usage:  authorize <player> <yes|name|immsim|mobsim|swear|plain|unpronu|no/deny>\n\r", ch );
		send_to_char( "Pending authorizations:\n\r", ch );
		send_to_char( " Chosen Character Name\n\r", ch );
		send_to_char( "---------------------------------------------\n\r", ch );
		for ( d = first_descriptor; d; d = d->next )
		{
			if ( (victim = d->character) != NULL && IS_WAITING_FOR_AUTH(victim) )
			{
				ch_printf( ch, "%s@%s new %s %s %s applying...\n\r", 
					victim->name, victim->desc->host, 
					victim->sex == SEX_MALE ? "Male" : victim->sex == SEX_FEMALE ? "Female" : "Neutral",
					race_table[victim->race]->race_name, ability_name[victim->main_ability] );
			}
		}
		return;
	}
	victim = get_waiting_desc( ch, arg1 );
	if ( victim == NULL )
		return;
	set_char_color( AT_IMMORT, victim );
	if ( arg2[0]=='\0' || !str_cmp( arg2,"accept" ) || !str_cmp( arg2,"yes" ))
	{
		victim->pcdata->auth_state = 3;
		if ( victim->pcdata->authed_by )
			STRFREE( victim->pcdata->authed_by );
		victim->pcdata->authed_by = QUICKLINK( ch->name );
		sprintf( buf, "%s: authorized", victim->name);
		to_channel( buf, CHANNEL_AUTH, "Auth", LEVEL_NEOPHYTE );

		ch_printf( ch, "You have authorized %s.\n\r", victim->name);
		REMOVE_BIT(victim->pcdata->flags, PCFLAG_UNAUTHED);
		/* Below sends a message to player when name is accepted - Brittany */
        ch_printf_color( victim,                                            
			"\n\r&GThe MUD Administrators have accepted the name %s.\n\r"
			"You are authorized to enter the Realms at the end of "
			"this area.\n\r", victim->name);
			return;
     }
	 else if ( !str_cmp( arg2, "immsim" ) || !str_cmp( arg2, "i" ) )
	 {
		victim->pcdata->auth_state = 2;
		sprintf( buf, "%s: name denied - similar to Imm name", victim->name);
		to_channel( buf, CHANNEL_AUTH, "Auth", LEVEL_NEOPHYTE );
		send_to_char_color(
			"&RThe name you have chosen is too similar to that of a current immortal. \n\r"
			"We ask you to please choose another name using the name command.\n\r", victim);
		ch_printf( ch, "You requested %s change names.\n\r", victim->name);
		return;
	}
	else if ( !str_cmp( arg2, "mobsim" ) || !str_cmp( arg2, "m" ) )
	{
		victim->pcdata->auth_state = 2;
		sprintf( buf, "%s: name denied - similar to mob name", victim->name);
		to_channel( buf, CHANNEL_AUTH, "Auth", LEVEL_NEOPHYTE );
		send_to_char_color(
			"&RThe name you have chosen is too similar to that of certain\n\r"
			"monsters in the game, in the long run this could cause problems\n\r"
			"and therefore we are unable to authorize them.  Please choose\n\r"
			"another name using the name command.\n\r", victim);
		ch_printf( ch, "You requested %s change names.\n\r", victim->name);
		return;
	}
	else if ( !str_cmp( arg2, "swear" ) || !str_cmp( arg2, "s" ) )
	{
		victim->pcdata->auth_state = 2;
		sprintf( buf, "%s: name denied - swear word", victim->name);
		to_channel( buf, CHANNEL_AUTH, "Auth", LEVEL_NEOPHYTE );
		
		send_to_char_color(
			"&RWe will not authorize names containing swear words, in any language.\n\r"
			"Please choose another name using the name command.\n\r",victim);
		ch_printf( ch, "You requested %s change names.\n\r", victim->name);
		return;
	}
	else if ( !str_cmp( arg2, "plain" ) || !str_cmp( arg2, "p" ) )
	{
		victim->pcdata->auth_state = 2;
		sprintf( buf, "%s: name denied", victim->name);
		to_channel( buf, CHANNEL_AUTH, "Auth", LEVEL_NEOPHYTE );
		send_to_char_color(
			"&RWe would ask you to please attempt to choose a name that is more\n\r"
			"starwarsish in nature.  Please choose another name using the name\n\r"
			"command.\n\r",victim);
		ch_printf( ch, "You requested %s change names.\n\r", victim->name);
		return;
	}
	else if ( !str_cmp( arg2, "unprou" ) || !str_cmp( arg2, "u" ) )
	{
		victim->pcdata->auth_state = 2;
		sprintf( buf, "%s: name denied - unpronouncable", victim->name);
		to_channel( buf, CHANNEL_AUTH, "Auth", LEVEL_NEOPHYTE );
		send_to_char_color(
			"&RThe name you have chosen is unpronouncable.\n\r"
			"Please choose another name using the name\n\r"
			"command.\n\r",victim);
		ch_printf( ch, "You requested %s change names.\n\r", victim->name);
		return;
	}
	else if ( !str_cmp( arg2, "no" ) || !str_cmp( arg2, "deny" ) )
	{
		send_to_char_color(
			"&RThe name you have chosen and/or the actions you have taken have\n\r"
			"been deemed grossly unacceptable to the administration of this mud.\n\r"
			"We ask you to discontinue such behaviour, or suffer possible banishmemt\n\r"
			"from this mud.\n\r", victim);
		sprintf( buf, "%s: denied authorization", victim->name);
		to_channel( buf, CHANNEL_AUTH, "Auth", LEVEL_NEOPHYTE );
		ch_printf( ch, "You have denied %s.\n\r", victim->name);
		do_quit(victim, "");
	}
	else if ( !str_cmp( arg2, "name" ) || !str_cmp(arg2, "n" ) )
	{
		victim->pcdata->auth_state = 2;
		sprintf( buf, "%s: name denied", victim->name);
		to_channel( buf, CHANNEL_AUTH, "Auth", LEVEL_NEOPHYTE );
		ch_printf_color(victim,
			"&R\n\rThe MUD Administrators have found the name %s "
			"to be unacceptable.\n\r"
			"You may choose a new name when you reach "
			"the end of this area.\n\r"
			"The name you choose must be starwarsish and original.\n\r"
			"No titles, descriptive words, or names close to any existing "
			"Immortal's name.\n\r", victim->name);
		ch_printf( ch, "You requested %s change names.\n\r", victim->name);
		return;
	}
	else
	{
		send_to_char("Invalid argument.\n\r", ch);
		return;
	}
}

void do_bamfin( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC(ch) )
    {
	smash_tilde( argument );
	DISPOSE( ch->pcdata->bamfin );
	ch->pcdata->bamfin = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
    }
    return;
}

void do_bamfout( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC(ch) )
    {
	smash_tilde( argument );
	DISPOSE( ch->pcdata->bamfout );
	ch->pcdata->bamfout = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
    }
    return;
}

void do_rank( CHAR_DATA *ch, char *argument )
{

  set_char_color( AT_IMMORT, ch );

  if ( IS_NPC(ch) )
    return;
  if ( !argument || argument[0] == '\0' )
  {
    send_to_char( "Usage:  rank <string>.\n\r", ch );
    send_to_char( "   or:  rank none.\n\r", ch );
    return;
  }
  smash_tilde( argument );
  DISPOSE( ch->pcdata->rank );
  if ( !str_cmp( argument, "none" ) )
    ch->pcdata->rank = str_dup( "" );
  else
    ch->pcdata->rank = str_dup( argument );
  send_to_char( "Ok.\n\r", ch );
  return;
}

void do_retire( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

	set_char_color( AT_IMMORT, ch );
    
	one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Retire whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
    if ( victim->top_level < LEVEL_SAVIOR )
    {
	send_to_char( "The minimum level for retirement is savior.\n\r", ch );
	return;
    }
    if ( IS_RETIRED( victim ) )
    {
      REMOVE_BIT( victim->pcdata->flags, PCFLAG_RETIRED );
      ch_printf( ch, "%s returns from retirement.\n\r", victim->name );
      ch_printf( victim, "%s brings you back from retirement.\n\r", ch->name );
    }
    else
    { 
      SET_BIT( victim->pcdata->flags, PCFLAG_RETIRED );
      ch_printf( ch, "%s is now a retired immortal.\n\r", victim->name );
      ch_printf( victim, "Courtesy of %s, you are now a retired immortal.\n\r", ch->name );
    }
    return;
}

void do_delay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    int delay;

    set_char_color( AT_IMMORT, ch );
 
    argument = one_argument( argument, arg );
    if ( !*arg ) {
      send_to_char( "Syntax:  delay <victim> <# of rounds>\n\r", ch );
      return;
    }
    if ( !( victim = get_char_world( ch, arg ) ) ) {
      send_to_char( "No such character online.\n\r", ch );
      return;
    }
    if ( IS_NPC( victim ) ) {
      send_to_char( "Mobiles are unaffected by lag.\n\r", ch );
      return;
    }
    if ( !IS_NPC(victim) && get_trust( victim ) >= get_trust( ch ) ) {
      send_to_char( "You haven't the power to succeed against them.\n\r", ch );
      return;
    }
    argument = one_argument(argument, arg);
    if ( !*arg ) {
      send_to_char( "For how long do you wish to delay them?\n\r", ch );
      return;
    }
    if ( !str_cmp( arg, "none" ) ) {
      send_to_char( "All character delay removed.\n\r", ch );
      victim->wait = 0;
      return;
    }
    delay = atoi( arg );
    if ( delay < 1 ) {
      send_to_char( "Pointless.  Try a positive number.\n\r", ch );
      return;
    }
    if ( delay > 999 ) {
      send_to_char( "You cruel bastard.  Just kill them.\n\r", ch );
      return;
    }
    WAIT_STATE( victim, delay * PULSE_VIOLENCE );
    ch_printf( ch, "You've delayed %s for %d rounds.\n\r", victim->name, delay );
    return;
}

void do_deny( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    set_char_color( AT_IMMORT, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Deny whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
    SET_BIT(victim->act, PLR_DENY);
	set_char_color( AT_IMMORT, victim );
    send_to_char( "You are denied access!\n\r", victim );
    ch_printf( ch, "You have denied access to %s.\n\r", victim->name );
    if ( victim->fighting )
      stop_fighting( victim, TRUE ); /* Blodkai, 97 */
    do_quit( victim, "" );
    return;
}

void do_disconnect( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    set_char_color( AT_IMMORT, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Disconnect whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( victim->desc == NULL )
    {
	act( AT_PLAIN, "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
	return;
    }
    if ( get_trust(ch) <= get_trust( victim ) )
    {
	send_to_char( "They might not like that...\n\r", ch );
	return;
    }

    for ( d = first_descriptor; d; d = d->next )
    {
	if ( d == victim->desc )
	{
	    close_socket( d, FALSE );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
    }
    bug( "Do_disconnect: *** desc not found ***.", 0 );
    send_to_char( "Descriptor not found!\n\r", ch );
    return;
}


/*
 * Force a level one player to quit.             Gorog
 */
void do_fquit( CHAR_DATA *ch, char *argument ) 
{ 
  CHAR_DATA *victim; 
  char arg1[MAX_INPUT_LENGTH]; 

  set_char_color( AT_IMMORT, ch );

  argument = one_argument( argument, arg1 ); 
  if ( arg1[0] == '\0' ) 
  { 
     send_to_char( "Force whom to quit?\n\r", ch ); 
     return; 
  } 
  if ( !( victim = get_char_world( ch, arg1 ) ) )
  { 
     send_to_char( "They aren't here.\n\r", ch ); 
     return; 
  } 
  if ( victim->top_level != 1 )  
  { 
     send_to_char( "They are not level one!\n\r", ch ); 
     return; 
  }
  set_char_color( AT_IMMORT, victim );
  send_to_char( "The MUD administrators force you to quit...\n\r", victim );
  if ( victim->fighting )
    stop_fighting( victim, TRUE );
  do_quit (victim, "");
  ch_printf( ch, "You have forced %s to quit.\n\r", victim->name ); 
  return; 
} 

void do_forceclose( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    int desc;

    set_char_color( AT_IMMORT, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Usage: forceclose <descriptor#>\n\r", ch );
	return;
    }

    desc = atoi( arg );
    for ( d = first_descriptor; d; d = d->next )
    {
	if ( d->descriptor == desc )
	{
	    if ( d->character && get_trust(d->character) >= get_trust(ch) )
	    {
		send_to_char( "They might not like that...\n\r", ch );
		return;
	    }
	    close_socket( d, FALSE );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
    }
    send_to_char( "Not found!\n\r", ch );
    return;
}

void echo_to_all( sh_int AT_COLOR, char *argument, sh_int tar )
{
    DESCRIPTOR_DATA *d;

    if ( !argument || argument[0] == '\0' )
	return;

    for ( d = first_descriptor; d; d = d->next )
    {
        /* Added showing echoes to players who are editing, so they won't
           miss out on important info like upcoming reboots. --Narn */ 
	if ( d->connected == CON_PLAYING || d->connected == CON_EDITING )
	{
	    /* This one is kinda useless except for switched.. */
	    if ( tar == ECHOTAR_PC && IS_NPC(d->character) )
	      continue;
	    else if ( tar == ECHOTAR_IMM && !IS_IMMORTAL(d->character) )
	      continue;
	    set_char_color( AT_COLOR, d->character );
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r",   d->character );
	}
    }
    return;
}

void do_ech( CHAR_DATA *ch, char *argument )
{
    send_to_char_color( "&YIf you want to echo something, use 'echo'.\n\r", ch );
    return;
}

void do_echo( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
    sh_int color;
    int target;
    char *parg;

    set_char_color( AT_IMMORT, ch );

    if ( IS_SET(ch->act, PLR_NO_EMOTE) ) {
        send_to_char( "You can't do that right now.\n\r", ch );
	return;
    }
    if ( argument[0] == '\0' ) {
	send_to_char( "Echo what?\n\r", ch );
	return;
    }

    if ( (color = get_color(argument)) )
      argument = one_argument(argument, arg);
    parg = argument;
    argument = one_argument(argument, arg);
    if ( !str_cmp( arg, "PC" )
    ||   !str_cmp( arg, "player" ) )
      target = ECHOTAR_PC;
    else if ( !str_cmp( arg, "imm" ) )
      target = ECHOTAR_IMM;
    else
    {
      target = ECHOTAR_ALL;
      argument = parg;
    }
    if ( !color && (color = get_color(argument)) )
      argument = one_argument(argument, arg);
    if ( !color )
      color = AT_IMMORT;
    one_argument(argument, arg);
    echo_to_all ( color, argument, target );
	sprintf( buf, "%s Echoed : %s", ch->name, argument );
	log_string( buf );
}

void echo_to_room( sh_int AT_COLOR, ROOM_INDEX_DATA *room, char *argument )
{
    CHAR_DATA *vic;

	if ( room == NULL )
		return;

    for ( vic = room->first_person; vic; vic = vic->next_in_room )
    {
	set_char_color( AT_COLOR, vic );
	send_to_char( argument, vic );
	send_to_char( "\n\r",   vic );
    }
}

void do_recho( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    sh_int color;

    set_char_color( AT_IMMORT, ch );

    if ( IS_SET(ch->act, PLR_NO_EMOTE) ) {
        send_to_char( "You can't do that right now.\n\r", ch );
	return;
    }
    if ( argument[0] == '\0' ) {
	send_to_char( "Recho what?\n\r", ch );
	return;
    }

    one_argument( argument, arg );
    if ( (color = get_color ( argument )) )
    {
       argument = one_argument ( argument, arg );
       echo_to_room ( color, ch->in_room, argument );
    }
    else
       echo_to_room ( AT_IMMORT, ch->in_room, argument );
}

ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    if ( is_number(arg) )
	return get_room_index( atoi( arg ) );

    if ( ( victim = get_char_world( ch, arg ) ) != NULL )
	return victim->in_room;

    if ( ( obj = get_obj_world( ch, arg ) ) != NULL )
	return obj->in_room;

    return NULL;
}

void do_transfer( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
	CHAR_DATA *victim2;

    set_char_color( AT_IMMORT, ch );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' ) { 
	send_to_char( "Transfer whom (and where)?\n\r", ch );
	return;
    }
    if ( !str_cmp( arg1, "all" ) && get_trust(ch) >= LEVEL_GREATER) 
    {
	for ( d = first_descriptor; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   d->character != ch
	    &&   d->character->in_room
	    &&   d->newstate != 2
	    &&   can_see( ch, d->character ) )
	    {
		char buf[MAX_STRING_LENGTH];
		sprintf( buf, "%s %s", d->character->name, arg2 );
		do_transfer( ch, buf );
	    }
	}
	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */

    if ( arg2[0] == '\0' )
    {
	location = ch->in_room;
    }
    else
    {
	if ( ( location = find_location( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "No such location.\n\r", ch );
	    return;
	}
	if ( room_is_private( ch,location ) && get_trust( ch ) < sysdata.level_override_private )
	{
	    send_to_char( "That room is private right now.\n\r", ch );
	    return;
	}
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( NOT_AUTHED( victim ) )
    {
	send_to_char( "They are not authorized yet!\n\r", ch);
	return;
    }
    if ( !victim->in_room )
    {
	send_to_char( "They have no physical location!\n\r", ch );
	return;
    }
    /* modification to prevent a low level imm from transferring a */
    /* higher level imm with the DND flag on.  - Gorog             */
    if ( !IS_NPC(victim) && get_trust(ch) < get_trust(victim)
    &&   victim->desc
    &&   (victim->desc->connected == CON_PLAYING
    ||    victim->desc->connected == CON_EDITING )
    &&   IS_SET(victim->pcdata->flags, PCFLAG_DND) ) 
    {
       pager_printf( ch,  
          "Sorry. %s does not wish to be disturbed.\n\r", victim->name);
       pager_printf( victim,
          "Your DND flag just foiled %s's transfer command.\n\r", ch->name);
       return;
    }
    /* end of modification                                         */

    if ( (victim2 = room_is_dnd( victim, location)) )
    {
       pager_printf( victim, "That room is \"do not disturb\" right now.\n\r");
	   pager_printf( ch, "That room is \"do not disturb\" right now.\n\r");
       pager_printf( victim2,
         "Your DND flag just foiled %s's transfer command.\n\r", ch->name );
       return;
    }


    if ( victim->fighting )
	stop_fighting( victim, TRUE );
    act( AT_MAGIC, "$n disappears in a cloud of swirling colors.", victim, NULL, NULL, TO_ROOM );
    victim->retran = victim->in_room->vnum;
    char_from_room( victim );
    char_to_room( victim, location );
	if ( victim->on )
    {
       victim->on = NULL;
       victim->position = POS_STANDING;
    }
    act( AT_MAGIC, "$n arrives from a puff of smoke.", victim, NULL, NULL, TO_ROOM );
    if ( ch != victim )
      act( AT_IMMORT, "$n has transferred you.", ch, NULL, victim, TO_VICT );
    do_look( victim, "auto" );
    if ( !IS_IMMORTAL( victim ) && !IS_NPC( victim )
    &&   !in_hard_range( victim, location->area ) )
      act( AT_DANGER, "Warning:  this player's level is not within the area's level range.", ch, NULL, NULL, TO_CHAR );
}

void do_retran( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];
	
	set_char_color( AT_IMMORT, ch );

	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char("Retransfer whom?\n\r", ch );
		return;
	}
	if ( !(victim = get_char_world(ch, arg)) )
	{
		send_to_char("They aren't here.\n\r", ch );
		return;
	}
	sprintf(buf, "'%s' %d", victim->name, victim->retran);
	do_transfer(ch, buf);
	return;
}

void do_regoto( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	
	sprintf(buf, "%d", ch->regoto);
	do_goto(ch, buf);
	return;
}

/*  Added do_at and do_atobj to reduce lag associated with at
 *  --Shaddai
 */
void do_at( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location = NULL;
    ROOM_INDEX_DATA *original;
    CHAR_DATA *wch=NULL, *victim;

    set_char_color( AT_IMMORT, ch );

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "At where what?\n\r", ch );
	return;
    }
    if ( is_number( arg ) )
    	location = get_room_index( atoi( arg ) );
    else if ( ( wch = get_char_world( ch, arg ) ) == NULL 
    	 || wch->in_room == NULL)
    {
	send_to_char( "No such mobile or player in existance.\n\r", ch );
	return;
    }
    if ( !location && wch )
    	location = wch->in_room;

    if ( !location )
    {
    	send_to_char( "No such location exists.\n\r", ch );
	return;
    }

    /* The following mod is used to prevent players from using the */
    /* at command on a higher level immortal who has a DND flag    */
    if ( wch && !IS_NPC(wch)
    &&   IS_SET(wch->pcdata->flags, PCFLAG_DND)
    &&   get_trust(ch) < get_trust(wch) ) 
    {
         pager_printf( ch,
           "Sorry. %s does not wish to be disturbed.\n\r",
           wch->name);
         pager_printf( wch,
           "Your DND flag just foiled %s's at command.\n\r", ch->name);
         return;
    }
    /* End of modification  -- Gorog */


    if ( room_is_private( ch,location ) )
    {
      if ( get_trust( ch ) < LEVEL_GREATER )
      {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
      }
      else
	send_to_char( "Overriding private flag!\n\r", ch );
    }

    if ( (victim=room_is_dnd(ch, location)) )
    {
       pager_printf( ch, "That room is \"do not disturb\" right now.\n\r" );
       pager_printf( victim,
          "Your DND flag just foiled %s's atmob command\n\r", ch->name );
       return;
    }

    set_char_color( AT_PLAIN, ch );
    original = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, location );
    interpret( ch, argument );

    if ( !char_died(ch) )
    {
      char_from_room( ch );
      char_to_room( ch, original );
    }
    return;
}

void do_atobj( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    OBJ_DATA *obj;
    CHAR_DATA *victim;

    set_char_color( AT_IMMORT, ch );

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "At where what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_world( ch, arg ) ) == NULL 
    	 || !obj->in_room )
    {
	send_to_char( "No such object in existance.\n\r", ch );
	return;
    }
    location = obj->in_room;
    if ( room_is_private( ch,location ) )
    {
      if ( get_trust( ch ) < LEVEL_GREATER )
      {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
      }
      else
	send_to_char( "Overriding private flag!\n\r", ch );
    }

    if ( (victim=room_is_dnd(ch, location)) )
    {
       pager_printf( ch, "That room is \"do not disturb\" right now.\n\r" );
       pager_printf( victim,
          "Your DND flag just foiled %s's atobj command\n\r", ch->name );
       return;
    }

    set_char_color( AT_PLAIN, ch );
    original = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, location );
    interpret( ch, argument );

    if ( !char_died(ch) )
    {
      char_from_room( ch );
      char_to_room( ch, original );
    }
    return;
}

void do_rat( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    int Start, End, vnum;

	set_char_color( AT_IMMORT, ch );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' || arg2[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Syntax: rat <start> <end> <command>\n\r", ch );
	return;
    }

    Start = atoi( arg1 );	End = atoi( arg2 );
    /*if ( Start < 1 || End < Start || Start > End || Start == End || End > 1048576000 )*/
	if ( Start < 1 || End < Start || Start > End || Start == End || End > MAX_VNUMS )
    {
	send_to_char( "Invalid range.\n\r", ch );
	return;
    }
    if ( !str_cmp( argument, "quit" ) )
    {
	send_to_char( "I don't think so!\n\r", ch );
	return;
    }

    original = ch->in_room;
    for ( vnum = Start; vnum <= End; vnum++ )
    {
	if ( (location = get_room_index(vnum)) == NULL )
	  continue;
	char_from_room( ch );
	char_to_room( ch, location );
	interpret( ch, argument );
    }

    char_from_room( ch );
    char_to_room( ch, original );
    send_to_char( "Done.\n\r", ch );
    return;
}

void do_rstat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char *sect;
    ROOM_INDEX_DATA *location;
    OBJ_DATA *obj;
    CHAR_DATA *rch;
    EXIT_DATA *pexit;
    AFFECT_DATA *paf;
    int cnt;
    static char *dir_text[] = { "n", "e", "s", "w", "u", "d", "ne", "nw", "se", "sw", "?" };

    one_argument( argument, arg );
    if ( !str_cmp( arg, "ex" ) || !str_cmp( arg, "exits" ) )
    {
	location = ch->in_room;

	ch_printf_color( ch, "&cExits for room '&W%s&c'  Vnum &W%d\n\r",
		location->name, location->vnum );
	for ( cnt = 0, pexit = location->first_exit; pexit; pexit = pexit->next ) 
	    ch_printf_color( ch,
		"&W%2d) &w%2s to %-5d  &cKey: &w%d  &cFlags: &w%d  &cKeywords: '&w%s&c'\n\r     Exdesc: &w%s     &cBack link: &w%d  &cVnum: &w%d  &cDistance: &w%d\n\r",
		++cnt,
		dir_text[pexit->vdir],
		pexit->to_room ? pexit->to_room->vnum : 0,
		pexit->key,
		pexit->exit_info,
		pexit->keyword,
		pexit->description[0] != '\0'
		    ? pexit->description : "(none).\n\r",
		pexit->rexit ? pexit->rexit->vnum : 0,
		pexit->rvnum,
		pexit->distance );
	return;
    }
    location = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
    if ( !location )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( ch->in_room != location && room_is_private( ch,location ) )
    {
	if ( get_trust( ch ) < LEVEL_GREATER )
	{
	    send_to_char( "That room is private right now.\n\r", ch );
	    return;
	}
	else
	    send_to_char( "Overriding private flag!\n\r", ch );
    }

    ch_printf_color( ch, "&cName: &w%s\n\r&cArea: &w%s  &cFilename: &w%s\n\r",
	location->name,
	location->area ? location->area->name : "None\?\?\?\?",
	location->area ? location->area->filename : "None\?\?\?\?" );

    switch( ch->in_room->sector_type ) {
	default:			sect = "\?\?\?!";		break;
	case SECT_INSIDE:		sect = "Inside";	break;
	case SECT_CITY:			sect = "City";		break;
	case SECT_FIELD:		sect = "Field";		break;
	case SECT_FOREST:		sect = "Forest";	break;
	case SECT_HILLS:		sect = "Hills";		break;
	case SECT_MOUNTAIN:		sect = "Mountains";	break;
	case SECT_WATER_SWIM:		sect = "Swim";		break;
	case SECT_WATER_NOSWIM:		sect = "Noswim";	break;
	case SECT_UNDERWATER:		sect = "Underwater";	break;
	case SECT_AIR:			sect = "Air";		break;
	case SECT_DESERT:		sect = "Desert";	break;
	case SECT_OCEANFLOOR:		sect = "Oceanfloor";	break;
	case SECT_UNDERGROUND:		sect = "Underground";	break;
	case SECT_LAVA:			sect = "Lava";		break;
	case SECT_SWAMP:		sect = "Swamp";		break;
    }

    ch_printf_color( ch, "&cVnum: &w%d   &cSector: &w%d (%s)   &cLight: &w%d",
	location->vnum,
	location->sector_type,
	sect,
	location->light );
    if ( location->tunnel > 0 )
	ch_printf_color( ch, "   &cTunnel: &W%d", location->tunnel );
    send_to_char( "\n\r", ch );
    ch_printf_color( ch, "&cRoom flags: &w%s\n\r",
	ext_flag_string(&location->room_flags, r_flags) );
    ch_printf_color( ch, "&cDescription:\n\r&w%s", location->description );
    if ( location->first_extradesc )
    {
	EXTRA_DESCR_DATA *ed;

	send_to_char_color( "&cExtra description keywords: &w'", ch );
	for ( ed = location->first_extradesc; ed; ed = ed->next )
	{
	    send_to_char( ed->keyword, ch );
	    if ( ed->next )
		send_to_char( " ", ch );
	}
	send_to_char( "'\n\r", ch );
    }
    for ( paf = location->first_affect; paf; paf = paf->next )
	ch_printf_color( ch, "&cAffect: &w%s &cby &w%d.\n\r",
	    affect_loc_name( paf->location ), paf->modifier );

    send_to_char_color( "&cCharacters: &w", ch );
    for ( rch = location->first_person; rch; rch = rch->next_in_room )
    {
	if ( can_see( ch, rch ) )
	{
	  send_to_char( " ", ch );
	  one_argument( rch->name, buf );
	  send_to_char( buf, ch );
	}
    }

    send_to_char_color( "\n\r&cObjects:    &w", ch );
    for ( obj = location->first_content; obj; obj = obj->next_content )
    {
	send_to_char( " ", ch );
	one_argument( obj->name, buf );
	send_to_char( buf, ch );
    }
    send_to_char( "\n\r", ch );

    if ( location->first_exit )
	send_to_char_color(  "&c------------------- &wEXITS &c-------------------\n\r", ch );
    for ( cnt = 0, pexit = location->first_exit; pexit; pexit = pexit->next )
	ch_printf( ch,
	  "%2d) %-2s to %-5d.  Key: %d  Flags: %d  Keywords: %s.\n\r",
		++cnt,
		dir_text[pexit->vdir],
		pexit->to_room ? pexit->to_room->vnum : 0,
		pexit->key,
		pexit->exit_info,
		pexit->keyword[0] != '\0' ? pexit->keyword : "(none)" );
    return;
}

/* Face-lift by Demora */
void do_ostat( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    OBJ_DATA *obj;

    set_char_color( AT_CYAN, ch );
 
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Ostat what?\n\r", ch );
        return;
    }
	if ( arg[0] != '\'' && arg[0] != '"' && strlen(argument) > strlen(arg) )
		strcpy( arg, argument );
	
	if ( ( obj = get_obj_world( ch, arg ) ) == NULL )
	{
		send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
		return;
	}
	ch_printf_color( ch, "&cName: &C%s\n\r", obj->name );
	ch_printf_color( ch, "&cVnum: &w%d  ", obj->pIndexData->vnum );
	ch_printf_color( ch, "&cType: &w%s  ", item_type_name( obj ) );
	ch_printf_color( ch, "&cCount:  &w%d  ", obj->pIndexData->count );
	ch_printf_color( ch, "&cGcount: &w%d\n\r", obj->count );
	ch_printf_color( ch, "&cSerial#: &w%d  ", obj->serial );
	ch_printf_color( ch, "&cTopIdxSerial#: &w%d  ", obj->pIndexData->serial );
	ch_printf_color( ch, "&cTopSerial#: &w%d\n\r", cur_obj_serial );
	ch_printf_color( ch, "&cShort description: &C%s\n\r", obj->short_descr);
	ch_printf_color( ch, "&cLong description : &C%s\n\r", obj->description);
	if ( obj->action_desc[0] != '\0' )
		ch_printf_color( ch, "&cAction description: &w%s\n\r", obj->action_desc );
	ch_printf_color( ch, "&cWear flags : &w%s\n\r", flag_string(obj->wear_flags,w_flags) );
	ch_printf_color( ch, "&cExtra flags: &w%s\n\r", flag_string(obj->extra_flags, o_flags) );
	ch_printf_color( ch, "&cMagic flags: &w%s\n\r", magic_bit_name( obj->magic_flags ) );
	ch_printf_color( ch, "&cNumber: &w%d/%d   ", 1, get_obj_number(obj) );
	ch_printf_color( ch, "&cWeight: &w%d/%d   ", obj->weight, get_obj_weight(obj) );
	ch_printf_color( ch, "&cLayers: &w%d   ", obj->pIndexData->layers );
	ch_printf_color( ch, "&cWear_loc: &w%d\n\r", obj->wear_loc );
	ch_printf_color( ch, "&cCost: &Y%d  ", obj->cost );
	ch_printf_color( ch, "&cRent: &w%d  ", obj->pIndexData->rent );
	send_to_char_color( "&cTimer: ", ch );
	if ( obj->timer > 0 )
		ch_printf_color( ch, "&R%d  ", obj->timer );
	else
		ch_printf_color( ch, "&w%d  ", obj->timer );
	ch_printf_color( ch, "&cLevel: &P%d\n\r", obj->level );
	ch_printf_color( ch, "&cIn room: &w%d  ", obj->in_room == NULL ? 0 : obj->in_room->vnum );
	ch_printf_color( ch, "&cIn object: &w%s  ", obj->in_obj == NULL ? "(none)" : obj->in_obj->short_descr );
	ch_printf_color( ch, "&cCarried by: &C%s\n\r", obj->carried_by == NULL ? "(none)" : obj->carried_by->name );
	ch_printf_color( ch, "&cIndex Values : &w%d %d %d %d %d %d.\n\r",
		obj->pIndexData->value[0], obj->pIndexData->value[1],
		obj->pIndexData->value[2], obj->pIndexData->value[3],
		obj->pIndexData->value[4], obj->pIndexData->value[5] );
	ch_printf_color( ch, "&cObject Values: &w%d %d %d %d %d %d.\n\r",
		obj->value[0], obj->value[1], obj->value[2], obj->value[3],
		obj->value[4], obj->value[5] );
	if ( obj->pIndexData->first_extradesc )
	{
		EXTRA_DESCR_DATA *ed;
		send_to_char( "Primary description keywords:   '", ch );
		for ( ed = obj->pIndexData->first_extradesc; ed; ed = ed->next )
		{
			send_to_char( ed->keyword, ch );
			if ( ed->next )
				send_to_char( " ", ch );
		}
		send_to_char( "'.\n\r", ch );
	}
	if ( obj->first_extradesc )
	{
		EXTRA_DESCR_DATA *ed;
		send_to_char( "Secondary description keywords: '", ch );
		for ( ed = obj->first_extradesc; ed; ed = ed->next )
		{
			send_to_char( ed->keyword, ch );
			if ( ed->next )
				send_to_char( " ", ch );
		}
		send_to_char( "'.\n\r", ch );
	}
	for ( paf = obj->first_affect; paf; paf = paf->next )
		ch_printf_color( ch, "&cAffects &w%s &cby &w%d. (extra)\n\r",
			affect_loc_name( paf->location ), paf->modifier );
	for ( paf = obj->pIndexData->first_affect; paf; paf = paf->next )
		ch_printf_color( ch, "&cAffects &w%s &cby &w%d.\n\r",
			affect_loc_name( paf->location ), paf->modifier );
	return;
}

void do_mstat( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    CHAR_DATA *victim;
    SKILLTYPE *skill;
    int x;

    set_char_color( AT_PLAIN, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Mstat whom?\n\r", ch );
	return;
    }
    if ( arg[0] != '\'' && arg[0] != '"' && strlen(argument) > strlen(arg) )
	strcpy( arg, argument );

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( get_trust( ch ) < get_trust( victim ) && !IS_NPC(victim) )
    {
	set_char_color( AT_IMMORT, ch );
	send_to_char( "Their godly glow prevents you from getting a good look.\n\r", ch );
	return;
    }

		ch_printf( ch, "Name: %s     Organization: %s\n\r",
	victim->name,
	( IS_NPC( victim ) || !victim->pcdata->clan ) ? "(none)" 
	: victim->pcdata->clan->name );

    if( get_trust(ch) >= LEVEL_GOD && !IS_NPC(victim) && victim->desc )
	ch_printf( ch, "User: %s@%s   Descriptor: %d   Trust: %d   AuthedBy: %s\n\r",
		victim->desc->user, victim->desc->host, victim->desc->descriptor,
		victim->trust, victim->pcdata->authed_by[0] != '\0'
		? victim->pcdata->authed_by : "(unknown)" );
    if ( !IS_NPC(victim) && victim->pcdata->release_date != 0 )
      ch_printf(ch, "Helled until %24.24s by %s.\n\r",
              ctime(&victim->pcdata->release_date),
              victim->pcdata->helled_by);
	if ( !IS_NPC(victim) && IS_SET(victim->act2, EXTRA_DONE) )
	ch_printf(ch,"%s is no longer a virgin.\n\r",victim->name);
	else if ( !IS_NPC(victim) && !IS_SET(victim->act2, EXTRA_DONE) ) ch_printf(ch, "%s is a virgin!.\n\r",victim->name);
	if ( !IS_NPC(victim) && IS_SET(victim->act2, EXTRA_PREGNANT) )
	ch_printf(ch,"%s is pregnant.\n\r",victim->name);
	if ( !IS_NPC(victim) && IS_SET(victim->act2, EXTRA_LABOUR) )
	ch_printf(ch,"%s is in labour.\n\r",victim->name);

    ch_printf( ch, "Vnum: %d   Sex: %s   Room: %d   Count: %d  Killed: %d\n\r",
	IS_NPC(victim) ? victim->pIndexData->vnum : 0,
	victim->sex == SEX_MALE    ? "male"   :
	victim->sex == SEX_FEMALE  ? "female" : "neutral",
	victim->in_room == NULL    ?        0 : victim->in_room->vnum,
	IS_NPC(victim) ? victim->pIndexData->count : 1,
	IS_NPC(victim) ? victim->pIndexData->killed
		       : victim->pcdata->mdeaths + victim->pcdata->pdeaths
	);
    ch_printf( ch, "Str: %d  Int: %d  Wis: %d  Dex: %d  Con: %d  Cha: %d  Lck: %d \n\r",
	get_curr_str(victim),
	get_curr_int(victim),
	get_curr_wis(victim),
	get_curr_dex(victim),
	get_curr_con(victim),
	get_curr_cha(victim),
	get_curr_lck(victim) );
    ch_printf( ch, "Hps: %d/%d  Move: %d/%d\n\r",
        victim->hit,         victim->max_hit,
        victim->move,        victim->max_move );  
    if ( !IS_NPC( victim ) )
    { 
       int ability;
    
       for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )
            ch_printf( ch, "%-15s   Level: %-3d   Max: %-3d   Exp: %-10ld   Next: %-10ld\n\r",
            ability_name[ability], victim->skill_level[ability], max_level(victim, ability), victim->experience[ability],
            exp_level( victim->skill_level[ability]+1 ) );
    }
	ch_printf( ch,
	"Top Level: %d     Race: %d  Align: %d  AC: %d  Gold: %d\n\r",
	victim->top_level,  victim->race,   victim->alignment,
	GET_AC(victim),      victim->gold );
    if ( IS_NPC(victim) && victim->race  < MAX_NPC_RACE  && victim->race  >= 0 )
	ch_printf( ch, "Race: %s\n\r", npc_race[victim->race] );
    else 
        ch_printf(ch, "Race: %d\n\r", race_table[victim->race]->race_name );
    ch_printf( ch, "Hitroll: %d   Damroll: %d   Position: %d   Wimpy: %d \n\r",
	GET_HITROLL(victim), GET_DAMROLL(victim),
	victim->position,    victim->wimpy );
    ch_printf( ch, "Fighting: %s    Master: %s    Leader: %s\n\r",
	victim->fighting ? victim->fighting->who->name : "(none)",
        victim->master      ? victim->master->name   : "(none)",
        victim->leader      ? victim->leader->name   : "(none)" );
    if ( !IS_NPC(victim) )
	ch_printf( ch,
	    "Thirst: %d   Full: %d   Drunk: %d     Glory: %d\n\r",
	    victim->pcdata->condition[COND_THIRST],
	    victim->pcdata->condition[COND_FULL],
	    victim->pcdata->condition[COND_DRUNK],
	    victim->pcdata->quest_curr) ;
    else
	ch_printf( ch, "Hit dice: %dd%d+%d.  Damage dice: %dd%d+%d.\n\r",
		victim->pIndexData->hitnodice,
		victim->pIndexData->hitsizedice,
		victim->pIndexData->hitplus,
		victim->pIndexData->damnodice,
		victim->pIndexData->damsizedice,
		victim->pIndexData->damplus );
    ch_printf( ch, "MentalState: %d   EmotionalState: %d\n\r",
 		victim->mental_state, victim->emotional_state );
/*    ch_printf( ch, "Saving throws: %d %d %d %d %d.\n\r",
		victim->saving_poison_death,
		victim->saving_wand,
		victim->saving_para_petri,
		victim->saving_breath,
		victim->saving_spell_staff  );
*/
    ch_printf( ch, "Carry figures: items (%d/%d)  weight (%d/%d)   Numattacks: %d\n\r",
	victim->carry_number, can_carry_n(victim), victim->carry_weight, can_carry_w(victim), victim->numattacks );
    ch_printf( ch, "Years: %d   Seconds Played: %d   Timer: %d   Act: %d\n\r",
	get_age( victim ), (int) victim->played, victim->timer, victim->act );
    if ( IS_NPC( victim ) )
    {	
	ch_printf( ch, "Act flags: %s\n\r", flag_string(victim->act, act_flags) );
        ch_printf( ch, "VIP flags: %s\n\r", flag_string(victim->vip_flags, planet_flags) );
    }    
    else
    { 
	ch_printf( ch, "Player flags: %s\n\r",
		flag_string(victim->act, plr_flags) );
	ch_printf( ch, "Pcflags: %s\n\r",
		flag_string(victim->pcdata->flags, pc_flags) );
	ch_printf( ch, "Wanted flags: %s\n\r",
		flag_string(victim->pcdata->wanted_flags, planet_flags) );
    }
    ch_printf( ch, "Affected by: %s\n\r",
	affect_bit_name( victim->affected_by ) );
    ch_printf( ch, "Speaks: %d   Speaking: %d\n\r",
    	victim->speaks, victim->speaking );
    send_to_char( "Languages: ", ch );
    for ( x = 0; lang_array[x] != LANG_UNKNOWN; x++ )
    	if ( knows_language( victim, lang_array[x], victim )
    	||  (IS_NPC(victim) && victim->speaks == 0) )
    	{
		if ( IS_SET(lang_array[x], victim->speaking)
		||  (IS_NPC(victim) && !victim->speaking) )
    		   set_char_color( AT_RED, ch );
 		send_to_char( lang_names[x], ch );
 		send_to_char( " ", ch );
 		set_char_color( AT_PLAIN, ch );
 	}
 	else
 	if ( IS_SET(lang_array[x], victim->speaking)
 	||  (IS_NPC(victim) && !victim->speaking) )
 	{
 		set_char_color( AT_PINK, ch );
 		send_to_char( lang_names[x], ch );
 		send_to_char( " ", ch );
 		set_char_color( AT_PLAIN, ch );
 	}
    send_to_char( "\n\r", ch );
    if ( victim->pcdata && victim->pcdata->bestowments 
         && victim->pcdata->bestowments[0] != '\0' )
      ch_printf( ch, "Bestowments: %s\n\r", victim->pcdata->bestowments );
	if ( IS_NPC( victim ) )	{
		ch_printf( ch, "Short description: %s\n\rLong  description: %s",
			victim->short_descr,
			victim->long_descr[0] != '\0' ? victim->long_descr : "(none)\n\r" );
		if ( IS_NPC(victim) && ( victim->spec_fun || victim->spec_2 ) )
			ch_printf( ch, "Mobile has spec fun: %s %s\n\r",
		lookup_spec( victim->spec_fun ),
		victim->spec_2 ? lookup_spec( victim->spec_2 ) : "" );
		ch_printf( ch, "Body Parts : %s\n\r",
			flag_string(victim->xflags, part_flags) );
		ch_printf( ch, "Resistant  : %s\n\r",
			flag_string(victim->resistant, ris_flags) );
		ch_printf( ch, "Immune     : %s\n\r",
			flag_string(victim->immune, ris_flags) );
		ch_printf( ch, "Susceptible: %s\n\r",
			flag_string(victim->susceptible, ris_flags) );
		ch_printf( ch, "Attacks    : %s\n\r",
			flag_string(victim->attacks, attack_flags) );
		ch_printf( ch, "Defenses   : %s\n\r",
			flag_string(victim->defenses, defense_flags) );
	}
    for ( paf = victim->first_affect; paf; paf = paf->next )
	if ( (skill=get_skilltype(paf->type)) != NULL )
	  ch_printf( ch,
	    "%s: '%s' modifies %s by %d for %d rounds with bits %s.\n\r",
	    skill_tname[skill->type],
	    skill->name,
	    affect_loc_name( paf->location ),
	    paf->modifier,
	    paf->duration,
	    affect_bit_name( paf->bitvector )
	    );
	if (victim->char_carriedby != NULL)
		ch_printf( ch, "Carried by: %s\n\r", victim->char_carriedby->name);
	if (victim->char_carrying != NULL)
		ch_printf( ch, "Carrying: %s\n\r", victim->char_carrying->name);
    return;
}

void do_mfind( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    int hash;
    int nMatch;
    bool fAll;

    set_pager_color( AT_PLAIN, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Mfind whom?\n\r", ch );
	return;
    }

    fAll	= !str_cmp( arg, "all" );
    nMatch	= 0;

    /*
     * This goes through all the hash entry points (1024), and is therefore
     * much faster, though you won't get your vnums in order... oh well. :)
     *
     * Tests show that Furey's method will usually loop 32,000 times, calling
     * get_mob_index()... which loops itself, an average of 1-2 times...
     * So theoretically, the above routine may loop well over 40,000 times,
     * and my routine bellow will loop for as many index_mobiles are on
     * your mud... likely under 3000 times.
     * -Thoric
     */
    for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
	for ( pMobIndex = mob_index_hash[hash];
	      pMobIndex;
	      pMobIndex = pMobIndex->next )
	    if ( fAll || nifty_is_name( arg, pMobIndex->player_name ) )
	    {
		nMatch++;
		pager_printf( ch, "[%5d] %s\n\r",
		    pMobIndex->vnum, capitalize( pMobIndex->short_descr ) );
	    }

    if ( nMatch )
	pager_printf( ch, "Number of matches: %d\n", nMatch );
    else
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
    return;
}

void do_ofind( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    int hash;
    int nMatch;
    bool fAll;

    set_pager_color( AT_PLAIN, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Ofind what?\n\r", ch );
	return;
    }

    fAll	= !str_cmp( arg, "all" );
    nMatch	= 0;

    /*
     * This goes through all the hash entry points (1024), and is therefore
     * much faster, though you won't get your vnums in order... oh well. :)
     *
     * Tests show that Furey's method will usually loop 32,000 times, calling
     * get_obj_index()... which loops itself, an average of 2-3 times...
     * So theoretically, the above routine may loop well over 50,000 times,
     * and my routine bellow will loop for as many index_objects are on
     * your mud... likely under 3000 times.
     * -Thoric
     */
    for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
	for ( pObjIndex = obj_index_hash[hash];
	      pObjIndex;
	      pObjIndex = pObjIndex->next )
	    if ( fAll || nifty_is_name( arg, pObjIndex->name ) )
	    {
		nMatch++;
		pager_printf( ch, "[%5d] %s\n\r",
		    pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
	    }

    if ( nMatch )
	pager_printf( ch, "Number of matches: %d\n", nMatch );
    else
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
    return;
}

void do_mwhere( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    bool found;

    set_pager_color( AT_PLAIN, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Mwhere whom?\n\r", ch );
	return;
    }

    found = FALSE;
    for ( victim = first_char; victim; victim = victim->next )
    {
	if ( IS_NPC(victim)
	&&   victim->in_room
	&&   nifty_is_name( arg, victim->name ) )
	{
	    found = TRUE;
	    pager_printf( ch, "[%5d] %-28s [%5d] %s\n\r",
		victim->pIndexData->vnum,
		victim->short_descr,
		victim->in_room->vnum,
		victim->in_room->name );
	}
    }

    if ( !found )
	act( AT_PLAIN, "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
    return;
}

void do_gwhere( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    bool found = FALSE, pmobs = FALSE;
    int low = 1, high = 200, count = 0; 

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] != '\0' )
    {
        if ( arg1[0] == '\0' || arg2[0] == '\0' )
        {
            send_to_pager( "\n\r&wSyntax:  gwhere | gwhere <low> <high> | gwhere <low> <high> mobs\n\r", ch );
            return;
        }
        low = atoi( arg1 );
        high = atoi( arg2 );
    }
    if ( low < 1 || high < low || low > high || high > 200 )
    {
        send_to_pager( "&wInvalid level range.\n\r", ch );
        return;
    }
    argument = one_argument( argument, arg3 );
    if ( !str_cmp( arg3, "mobs" ) )
      pmobs = TRUE;

    pager_printf( ch, "\n\r&cGlobal %s locations:&w\n\r", pmobs ? "mob" : "player" );
    if ( !pmobs )
    {
        for ( d = first_descriptor; d; d = d->next )
        if ( (d->connected == CON_PLAYING || d->connected == CON_EDITING )
        && ( victim = d->character ) != NULL && !IS_NPC(victim) && victim->in_room
        &&   can_see( ch, victim )
        &&   victim->top_level >= low && victim->top_level <= high )
        {
            found = TRUE;
            pager_printf( ch, "&c(&C%3d&c) &w%-12.12s   [%-5d - %-19.19s]   &c%-25.25s\n\r",
		victim->top_level, victim->name, victim->in_room->vnum, victim->in_room->area->name, victim->in_room->name );
	    count++;
        }
    }
    else
    {
        for ( victim = first_char; victim; victim = victim->next )
        if ( IS_NPC(victim)
        &&   victim->in_room && can_see( ch, victim )
        &&   victim->top_level >= low && victim->top_level <= high )
        {
            found = TRUE;
            pager_printf( ch, "&c(&C%-3d&c) &w%-12.12s   [%-5d - %-19.19s]   &c%-25.25s\n\r",
                victim->top_level, victim->name, victim->in_room->vnum, victim->in_room->area->name, victim->in_room->name );
	    count++;
        }
    }
    pager_printf( ch, "&c%d %s found.\n\r", count, pmobs ? "mobs" : "characters" );
    return;
}

void do_gfighting( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    bool found = FALSE;
    int low = 1, high = 105, count = 0;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] != '\0' )
    {
	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
        send_to_pager( "\n\r&wSyntax:  gfighting | gfighting <low> <high> | gfighting <low> <high> mobs\n\r", ch );
	    return;
	}
	low = atoi( arg1 );
	high = atoi( arg2 );
    }
    if ( low < 1 || high < low || low > high || high > 105 )
    {
        send_to_pager( "&wInvalid level range.\n\r", ch );
        return;
    }

    send_to_char( "&cGlobal conflicts:\n\r", ch );
        for ( d = first_descriptor; d; d = d->next )
        if ( (d->connected == CON_PLAYING || d->connected == CON_EDITING )
        && ( victim = d->character ) != NULL && !IS_NPC( victim ) && victim->in_room
        &&   can_see( ch, victim )
        &&   victim->fighting && victim->top_level >= low && victim->top_level <= high )
        {
            found = TRUE;
            pager_printf_color( ch, "&w%-12.12s &C|%2d &wvs &C%2d| &w%-16.16s [%5d]  &c%-20.20s [%5d]\n\r",
		victim->name, victim->top_level, victim->fighting->who->top_level,
		IS_NPC( victim->fighting->who ) ? victim->fighting->who->short_descr : victim->fighting->who->name,
		IS_NPC( victim->fighting->who ) ? victim->fighting->who->pIndexData->vnum : 0,
		victim->in_room->area->name, victim->in_room == NULL ? 0 : victim->in_room->vnum );
	    count++;
        }
    pager_printf( ch, "&c%d conflicts located.\n\r", count );
    return;
}

/* Added 'show' argument for lowbie imms without ostat -- Blodkai */
/* Made show the default action :) Shaddai */
/* Trimmed size, added vict info, put lipstick on the pig -- Blod */
void do_bodybag( CHAR_DATA *ch, char *argument )
{
    char buf2[MAX_STRING_LENGTH];
    char buf3[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *owner;
    OBJ_DATA *obj;
    bool found = FALSE, bag = FALSE;

    argument = one_argument( argument, arg1 );
    if ( arg1[0] == '\0' )
    {
	send_to_char_color( "&PSyntax:  bodybag <character> | bodybag <character> yes/bag/now\n\r", ch );
	return;
    }

    sprintf( buf3, " " );
    sprintf( buf2, "the corpse of %s", arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg2[0] !='\0' && ( str_cmp( arg2, "yes" )
    && str_cmp( arg2, "bag" ) && str_cmp( arg2, "now" ) ) )
    {
        send_to_char_color( "\n\r&PSyntax:  bodybag <character> | bodybag <character> yes/bag/now\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "yes" ) || !str_cmp( arg2, "bag" )
    ||   !str_cmp( arg2, "now" ) )
      bag = TRUE;

    pager_printf_color( ch, "\n\r&P%s remains of %s ... ",
	bag ? "Retrieving" : "Searching for",
	capitalize( arg1 ) );
    for ( obj = first_object; obj; obj = obj->next )
    {
        if ( obj->in_room 
        && !str_cmp( buf2, obj->short_descr ) 
        && (obj->pIndexData->vnum == 11 ) )
	{
	      send_to_pager( "\n\r", ch );
	      found = TRUE;
	      pager_printf_color( ch, "&P%s:  %s%-12.12s &PIn:  &w%-22.22s  &P[&w%5d&P]   &PTimer:  %s%2d",
		bag ? "Bagging" : "Corpse",
		bag ? "&Y"      : "&w",
                capitalize( arg1 ),
                obj->in_room->area->name,
                obj->in_room->vnum,
		obj->timer < 1 ? "&w" : obj->timer < 5 ? "&R" : obj->timer < 10 ? "&Y" : "&w",
                obj->timer );
	      if ( bag )
	      {
		obj_from_room( obj );
                obj = obj_to_char( obj, ch );
                obj->timer = -1;
                save_char_obj( ch );
	      }
	}
    }
    if ( !found )
    {
	send_to_pager_color( "&Pno corpse was found.\n\r", ch );
	return;
    }
    send_to_pager( "\n\r", ch );
    for ( owner = first_char; owner; owner = owner->next )
    {
	if ( IS_NPC( owner ) )
	  continue;
        if ( can_see( ch, owner) && !str_cmp( arg1, owner->name ) )
          break;
    }
    if ( owner == NULL )
    {
	pager_printf_color( ch, "&P%s is not currently online.\n\r",
	  capitalize( arg1 ) );
	return;
    }
        pager_printf_color( ch, "&P%s (%d) has no deity.\n\r",
	  owner->name, owner->top_level );
    return;
}

/*#if 0*/
/* New owhere by Altrag, 03/14/96 */
void do_owhere( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    bool found;
    int icnt = 0;

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Owhere what?\n\r", ch );
	return;
    }
    argument = one_argument(argument, arg1);
    
    set_pager_color( AT_PLAIN, ch );
    if ( arg1[0] != '\0' && !str_prefix(arg1, "nesthunt") )
    {
      if ( !(obj = get_obj_world(ch, arg)) )
      {
        send_to_char( "Nesthunt for what object?\n\r", ch );
        return;
      }
      for ( ; obj->in_obj; obj = obj->in_obj )
      {
	pager_printf(ch, "[%5d] %-28s in object [%5d] %s\n\r",
                obj->pIndexData->vnum, obj_short(obj),
                obj->in_obj->pIndexData->vnum, obj->in_obj->short_descr);
	++icnt;
      }
      sprintf(buf, "[%5d] %-28s in ", obj->pIndexData->vnum,
		obj_short(obj));
      if ( obj->carried_by )
        sprintf(buf+strlen(buf), "invent [%5d] %s\n\r",
                (IS_NPC(obj->carried_by) ? obj->carried_by->pIndexData->vnum
                : 0), PERS(obj->carried_by, ch));
      else if ( obj->in_room )
        sprintf(buf+strlen(buf), "room   [%5d] %s\n\r",
                obj->in_room->vnum, obj->in_room->name);
      else if ( obj->in_obj )
      {
        bug("do_owhere: obj->in_obj after NULL!",0);
        strcat(buf, "object??\n\r");
      }
      else
      {
        bug("do_owhere: object doesnt have location!",0);
        strcat(buf, "nowhere??\n\r");
      }
      send_to_pager(buf, ch);
      ++icnt;
      pager_printf(ch, "Nested %d levels deep.\n\r", icnt);
      return;
    }

    found = FALSE;
    for ( obj = first_object; obj; obj = obj->next )
    {
        if ( !nifty_is_name( arg, obj->name ) )
            continue;
        found = TRUE;
        
        sprintf(buf, "(%3d) [%5d] %-28s in ", ++icnt, obj->pIndexData->vnum,
                obj_short(obj));
        if ( obj->carried_by )
          sprintf(buf+strlen(buf), "invent [%5d] %s\n\r",
                  (IS_NPC(obj->carried_by) ? obj->carried_by->pIndexData->vnum
                  : 0), PERS(obj->carried_by, ch));
        else if ( obj->in_room )
          sprintf(buf+strlen(buf), "room   [%5d] %s\n\r",
                  obj->in_room->vnum, obj->in_room->name);
        else if ( obj->in_obj )
          sprintf(buf+strlen(buf), "object [%5d] %s\n\r",
                  obj->in_obj->pIndexData->vnum, obj_short(obj->in_obj));
        else
        {
          bug("do_owhere: object doesnt have location!",0);
          strcat(buf, "nowhere??\n\r");
        }
        send_to_pager(buf, ch);
    }

    if ( !found )
      act( AT_PLAIN, "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
    else
      pager_printf(ch, "%d matches.\n\r", icnt);

    return;
}
/*#endif*/

void do_reboo( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to REBOOT, spell it out.\n\r", ch );
    return;
}

void do_reboot( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    extern bool mud_down;
    CHAR_DATA *vch;

	set_char_color( AT_IMMORT, ch );

    if ( str_cmp( argument, "mud now" )
    &&   str_cmp( argument, "nosave" )
    &&   str_cmp( argument, "and sort skill table" ) )
    {
	send_to_char( "Syntax: 'reboot mud now' or 'reboot nosave'\n\r", ch );
	return;
    }

    if ( auction->item )
	do_auction( ch, "stop");
    sprintf( buf, "Reboot by %s.", ch->name );
    do_echo( ch, buf );

    if ( !str_cmp(argument, "and sort skill table") )
    {
	sort_skill_table();
	save_skill_table();
    }

    /* Save all characters before booting. */
    if ( str_cmp(argument, "nosave") )
	for ( vch = first_char; vch; vch = vch->next )
	    if ( !IS_NPC( vch ) )
		save_char_obj( vch );

    mud_down = TRUE;
    return;
}

void do_shutdow( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SHUTDOWN, spell it out.\n\r", ch );
    return;
}

void do_shutdown( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    extern bool mud_down;
    CHAR_DATA *vch;

	set_char_color( AT_IMMORT, ch );

    if ( str_cmp( argument, "mud now" ) && str_cmp(argument, "nosave") )
    {
	send_to_char( "Syntax: 'shutdown mud now' or 'shutdown nosave'\n\r", ch );
	return;
    }

    if ( auction->item )
	do_auction( ch, "stop");
    sprintf( buf, "Shutdown by %s.", ch->name );
    append_file( ch, SHUTDOWN_FILE, buf );
    strcat( buf, "\n\r" );
    do_echo( ch, buf );

    /* Save all characters before booting. */
    if ( str_cmp(argument, "nosave") )
	for ( vch = first_char; vch; vch = vch->next )
	    if ( !IS_NPC( vch ) )
		save_char_obj( vch );
    mud_down = TRUE;
    return;
}

void do_snoop( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

	set_char_color( AT_IMMORT, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Snoop whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( !victim->desc )
    {
	send_to_char( "No descriptor to snoop.\n\r", ch );
	return;
    }
    if ( victim == ch )
    {
	send_to_char( "Cancelling all snoops.\n\r", ch );
	for ( d = first_descriptor; d; d = d->next )
	    if ( d->snoop_by == ch->desc )
		d->snoop_by = NULL;
	return;
    }
    if ( victim->desc->snoop_by )
    {
	send_to_char( "Busy already.\n\r", ch );
	return;
    }

    /*
     * Minimum snoop level... a secret mset value
     * makes the snooper think that the victim is already being snooped
     */
    if ( get_trust( victim ) >= get_trust( ch )
    ||  (victim->pcdata && victim->pcdata->min_snoop > get_trust( ch )) )
    {
	send_to_char( "Busy already.\n\r", ch );
	return;
    }

    if ( ch->desc )
    {
	for ( d = ch->desc->snoop_by; d; d = d->snoop_by )
	    if ( d->character == victim || d->original == victim )
	    {
		send_to_char( "No snoop loops.\n\r", ch );
		return;
	    }
    }

/*  Snoop notification for higher imms, if desired, uncomment this
    if ( get_trust(victim) > LEVEL_GOD && get_trust(ch) < LEVEL_SUPREME )
      write_to_descriptor( victim->desc->descriptor, "\n\rYou feel like someone is watching your every move...\n\r", 0 );
*/
    victim->desc->snoop_by = ch->desc;
    send_to_char( "Ok.\n\r", ch );
    return;
}

void do_statshield( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    set_char_color( AT_IMMORT, ch );

    one_argument( argument, arg );
    if ( IS_NPC ( ch ) || get_trust( ch ) < LEVEL_GREATER )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    if ( arg[0] == '\0' )
    {
	send_to_char( "Statshield which mobile?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "No such mobile.\n\r", ch );
        return;
    }                  
    if ( !IS_NPC( victim ) )
    {
	send_to_char( "You can only statshield mobiles.\n\r", ch );
	return;
    }
    if ( xIS_SET( victim->new_act, ACT_STATSHIELD ) )
    {
	xREMOVE_BIT( victim->new_act, ACT_STATSHIELD );
	ch_printf( ch, "You have lifted the statshield on %s.\n\r", victim->short_descr );
    }
    else
    {
	xSET_BIT( victim->new_act, ACT_STATSHIELD );
	ch_printf( ch, "You have applied a statshield to %s.\n\r", victim->short_descr );
    }
    return;
}

void do_switch( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

	set_char_color( AT_IMMORT, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Switch into whom?\n\r", ch );
	return;
    }
    if ( !ch->desc )
	return;
    if ( ch->desc->original )
    {
	send_to_char( "You are already switched.\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( victim == ch )
    {
	send_to_char( "Ok.\n\r", ch );
	return;
    }
	if ( IS_NPC( victim ) && xIS_SET( victim->new_act, ACT_STATSHIELD )
    &&   get_trust( ch ) < LEVEL_GREATER )
    {
	set_pager_color( AT_IMMORT, ch );
	send_to_pager( "Their godly glow prevents you from getting close enough.\n\r", ch );
	return;
    }
    if ( victim->desc )
    {
	send_to_char( "Character in use.\n\r", ch );
	return;
    }
    if ( !IS_NPC(victim) && ch->top_level < LEVEL_GREATER )
    {
	send_to_char( "You cannot switch into a player!\n\r", ch );
	return;
    }
    if ( victim->switched )
    {
       send_to_char( "You can't switch into a player that is switched!\n\r",ch);
       return;
    }
    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_FREEZE) )
    {
       send_to_char( "You shouldn't switch into a player that is frozen!\n\r",ch);
       return;
    }
    ch->desc->character = victim;
    ch->desc->original  = ch;
    victim->desc        = ch->desc;
    ch->desc            = NULL;
    ch->switched	= victim;
    send_to_char( "Ok.\n\r", victim );
    return;
}

void do_return( CHAR_DATA *ch, char *argument )
{

	if ( !IS_NPC( ch ) && get_trust(ch) < LEVEL_IMMORTAL )
    {
	send_to_char("Huh?\n\r", ch );
	return;
    }
    set_char_color( AT_IMMORT, ch );

    if ( !ch->desc )
	return;
    if ( !ch->desc->original )
    {
	send_to_char( "You aren't switched.\n\r", ch );
	return;
    }

    send_to_char( "You return to your original body.\n\r", ch );

	if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_POSSESS ) )
	{
		affect_strip( ch, gsn_possess );
		REMOVE_BIT( ch->affected_by, AFF_POSSESS );
	}

    ch->desc->character       = ch->desc->original;
    ch->desc->original        = NULL;
    ch->desc->character->desc = ch->desc;
    ch->desc->character->switched = NULL;
    ch->desc                  = NULL;
    return;
}

void do_minvoke( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *victim;
    int vnum;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax: minvoke <vnum>.\n\r", ch );
	return;
    }

    if ( !is_number( arg ) )
    {
	char arg2[MAX_INPUT_LENGTH];
	int  hash, cnt;
	int  count = number_argument( arg, arg2 );

	vnum = -1;
	for ( hash = cnt = 0; hash < MAX_KEY_HASH; hash++ )
	    for ( pMobIndex = mob_index_hash[hash];
		  pMobIndex;
		  pMobIndex = pMobIndex->next )
	    if ( nifty_is_name( arg2, pMobIndex->player_name )
	    &&   ++cnt == count )
	    {
		vnum = pMobIndex->vnum;
		break;
	    }
	if ( vnum == -1 )
	{
	    send_to_char( "No such mobile exists.\n\r", ch );
	    return;
	}
    }
    else
	vnum = atoi( arg );

    if ( get_trust(ch) < LEVEL_DEMI )
    {
	AREA_DATA *pArea;

	if ( IS_NPC(ch) )
	{
	  send_to_char( "Huh?\n\r", ch );
	  return;
	}

	if ( !ch->pcdata || !(pArea=ch->pcdata->area) )
	{
	  send_to_char( "You must have an assigned area to invoke this mobile.\n\r", ch );
	  return;
	}
	if ( vnum < pArea->low_m_vnum
	&&   vnum > pArea->hi_m_vnum )
	{
	  send_to_char( "That number is not in your allocated range.\n\r", ch );
	  return;
	}
    }

    if ( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
    {
	send_to_char( "No mobile has that vnum.\n\r", ch );
	return;
    }

    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );
    act( AT_IMMORT, "$n invokes $N!", ch, NULL, victim, TO_ROOM );
    /*How about seeing what we're invoking for a change. -Blodkai*/
    ch_printf_color( ch, "&YYou invoke %s (&W#%d &Y- &W%s &Y- &Wlvl %d&Y)\n\r",
	pMobIndex->short_descr, pMobIndex->vnum, pMobIndex->player_name, victim->top_level );
    return;
}

void do_oinvoke( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
	int vnum, level;

	set_char_color( AT_IMMORT, ch );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax: oinvoke <vnum> <level>.\n\r", ch );
	return;
    }
    if ( arg2[0] == '\0' )
    {
	level = get_trust( ch );
    }
    else
    {
	if ( !is_number( arg2 ) )
	{
	    send_to_char( "Syntax: oinvoke <vnum> <level>.\n\r", ch );
	    return;
	}
	level = atoi( arg2 );
	if ( level < 0 || level > get_trust( ch ) )
	{
	    send_to_char( "Limited to your trust level.\n\r", ch );
	    return;
        }
    }
    if ( !is_number( arg1 ) )
    {
	char arg[MAX_INPUT_LENGTH];
	int  hash, cnt;
	int  count = number_argument( arg1, arg );

	vnum = -1;
	for ( hash = cnt = 0; hash < MAX_KEY_HASH; hash++ )
	    for ( pObjIndex = obj_index_hash[hash];
		  pObjIndex;
		  pObjIndex = pObjIndex->next )
	    if ( nifty_is_name( arg, pObjIndex->name )
	    &&   ++cnt == count )
	    {
		vnum = pObjIndex->vnum;
		break;
	    }
	if ( vnum == -1 )
	{
	    send_to_char( "No such object exists.\n\r", ch );
	    return;
	}
    }
    else
	vnum = atoi( arg1 );

    if ( get_trust(ch) < LEVEL_DEMI )
    {
	AREA_DATA *pArea;

	if ( IS_NPC(ch) )
	{
	  send_to_char( "Huh?\n\r", ch );
	  return;
	}
	if ( !ch->pcdata || !(pArea=ch->pcdata->area) )
	{
	  send_to_char( "You must have an assigned area to invoke this object.\n\r", ch );
	  return;
	}
	if ( vnum < pArea->low_o_vnum
	&&   vnum > pArea->hi_o_vnum )
	{
	  send_to_char( "That number is not in your allocated range.\n\r", ch );
	  return;
	}
    }
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
	send_to_char( "No object has that vnum.\n\r", ch );
	return;
    }

	if ( level == 0 )
    {
       AREA_DATA *temp_area;

       if ( (temp_area = get_area_obj(pObjIndex)) == NULL )
       	   level = ch->top_level;
       else {
           level = generate_itemlevel( temp_area, pObjIndex);
           level = URANGE(0, level, LEVEL_AVATAR);
       }
    }

    obj = create_object( pObjIndex, level );
    if ( CAN_WEAR(obj, ITEM_TAKE) )
    {
	obj = obj_to_char( obj, ch );
    }
    else
    {
	obj = obj_to_room( obj, ch->in_room );
	act( AT_IMMORT, "$n fashions $p from ether!", ch, obj, NULL, TO_ROOM );
    }
    /* I invoked what? --Blodkai */
    ch_printf_color( ch, "&YYou invoke %s (&W#%d &Y- &W%s &Y- &Wlvl %d&Y)\n\r",
	pObjIndex->short_descr, pObjIndex->vnum, pObjIndex->name, obj->level );
    return;
}

void do_purge( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    set_char_color( AT_IMMORT, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	/* 'purge' */
	CHAR_DATA *vnext;
	OBJ_DATA  *obj_next;

	for ( victim = ch->in_room->first_person; victim; victim = vnext )
	{
	    vnext = victim->next_in_room;
	    if ( IS_NPC(victim) && victim != ch)
		extract_char( victim, TRUE );
	}

	for ( obj = ch->in_room->first_content; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    extract_obj( obj );
	}

	act( AT_IMMORT, "$n purges the room!", ch, NULL, NULL, TO_ROOM);
	act( AT_IMMORT, "You have purged the room!", ch, NULL, NULL, TO_CHAR );
	return;
    }
    victim = NULL; obj = NULL;

    /* fixed to get things in room first -- i.e., purge portal (obj),
     * no more purging mobs with that keyword in another room first
     * -- Tri */
    if ( ( victim = get_char_room( ch, arg ) ) == NULL 
    && ( obj = get_obj_here( ch, arg ) ) == NULL )
    {
      if ( ( victim = get_char_world( ch, arg ) ) == NULL
      &&   ( obj = get_obj_world( ch, arg ) ) == NULL )  /* no get_obj_room */
      {
	send_to_char( "They aren't here.\n\r", ch );
	return;
      }
    }

/* Single object purge in room for high level purge - Scryn 8/12*/
    if ( obj )
    {
	separate_obj( obj );
	act( AT_IMMORT, "$n purges $p.", ch, obj, NULL, TO_ROOM);
	act( AT_IMMORT, "You make $p disappear in a puff of smoke!", ch, obj, NULL, TO_CHAR);          
	extract_obj( obj );
	return;
    }

    if ( !IS_NPC(victim) )
    {
	send_to_char( "Not on PC's.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
    	send_to_char( "You cannot purge yourself!\n\r", ch );
    	return;
    }

    act( AT_IMMORT, "$n purges $N.", ch, NULL, victim, TO_NOTVICT );
    act( AT_IMMORT, "You make $N disappear in a puff of smoke!", ch, NULL, victim, TO_CHAR );
    extract_char( victim, TRUE );
    return;
}

void do_low_purge( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    set_char_color( AT_IMMORT, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Purge what?\n\r", ch );
	return;
    }

    victim = NULL; obj = NULL;
    if ( ( victim = get_char_room( ch, arg ) ) == NULL
    &&	 ( obj    = get_obj_here ( ch, arg ) ) == NULL )
    {
	send_to_char( "You can't find that here.\n\r", ch );
	return;
    }

    if ( obj )
    {
	separate_obj( obj );
	act( AT_IMMORT, "$n purges $p!", ch, obj, NULL, TO_ROOM );
	act( AT_IMMORT, "You make $p disappear in a puff of smoke!", ch, obj, NULL, TO_CHAR );
	extract_obj( obj );
	return;
    }

    if ( !IS_NPC(victim) )
    {
	send_to_char( "Not on PC's.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
    	send_to_char( "You cannot purge yourself!\n\r", ch );
    	return;
    }

    act( AT_IMMORT, "$n purges $N.", ch, NULL, victim, TO_NOTVICT );
    act( AT_IMMORT, "You make $N disappear in a puff of smoke!", ch, NULL, victim, TO_CHAR );
    extract_char( victim, TRUE );
    return;
}

void do_balzhur( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char *name;
    CHAR_DATA *victim;
    AREA_DATA *pArea;   
    int sn;

    set_char_color( AT_BLOOD, ch );

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' ) {
	send_to_char( "Who is deserving of such a fate?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL ) {
	send_to_char( "They aren't currently playing.\n\r", ch);
	return;
    }
    if ( IS_NPC( victim ) ) {
	send_to_char( "This will do little good on mobiles.\n\r", ch );
	return;
    }
    if ( victim->top_level >= get_trust( ch ) ) {
	send_to_char( "I wouldn't even think of that if I were you...\n\r", ch );
	return;
    }

    set_char_color( AT_WHITE, ch );
    send_to_char( "You summon the demon Balzhur to wreak your wrath!\n\r", ch );
    send_to_char( "Balzhur sneers at you evilly, then vanishes in a puff of smoke.\n\r", ch );
    set_char_color( AT_IMMORT, victim );
    send_to_char( "You hear an ungodly sound in the distance that makes your blood run cold!\n\r", victim );
    sprintf( buf, "Balzhur screams, 'You are MINE %s!!!'", victim->name );
    echo_to_all( AT_IMMORT, buf, ECHOTAR_ALL );
	victim->top_level    = 2;
	victim->trust	 = 0;
	{
	   int ability;
	   
	   for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )
	   {
	       victim->experience[ability] = 1;
	       victim->skill_level[ability] = 1;
	   }
	}
	victim->max_hit  = 100;
	victim->max_mana = 0;
	victim->max_move = 1000;
	for ( sn = 0; sn < top_sn; sn++ )
	    victim->pcdata->learned[sn] = 0;
	victim->hit      = victim->max_hit;
	victim->mana     = victim->max_mana;
	victim->move     = victim->max_move;
    name = capitalize(victim->name);
    sprintf( buf, "%s%s", GOD_DIR, name );  

    set_char_color( AT_RED, ch ); 
    if ( !remove( buf ) )
      send_to_char( "Player's immortal data destroyed.\n\r", ch );
    else if ( errno != ENOENT )
    {
      ch_printf( ch, "Unknown error #%d - %s (immortal data).  Report to Thoric\n\r",
	errno, strerror( errno ) );
      sprintf( buf2, "%s balzhuring %s", ch->name, buf );
      perror( buf2 );
    }
    sprintf( buf2, "%s.are", name  );
    for ( pArea = first_build; pArea; pArea = pArea->next )
      if ( !str_cmp( pArea->filename, buf2 ) )
      {
        sprintf( buf, "%s%s", BUILD_DIR, buf2 );
        if ( IS_SET( pArea->status, AREA_LOADED ) )
          fold_area( pArea, buf, FALSE );
        close_area( pArea );
        sprintf( buf2, "%s.bak", buf );
        set_char_color( AT_RED, ch ); /* Log message changes colors */
        if ( !rename( buf, buf2 ) )
          send_to_char( "Player's area data destroyed.  Area saved as backup.\n\r", ch);
        else if ( errno != ENOENT )
        {
          ch_printf( ch, "Unknown error #%d - %s (area data).  Report to  Thoric.\n\r",
		errno, strerror( errno ) );
          sprintf( buf2, "%s destroying %s", ch->name, buf );
          perror( buf2 );   
        }
        break;
      }
 
    make_wizlist();
    advance_level( victim ,COMBAT_ABILITY);
    do_help(victim, "M_BALZHUR_" );
    set_char_color( AT_WHITE, victim );
    send_to_char( "You awake after a long period of time...\n\r", victim );
    while ( victim->first_carrying )
	extract_obj( victim->first_carrying );
    return;
}

void do_advance( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int level, ability;
    int iLevel, iAbility;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg3 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Syntax: advance <char> <ability> <level>.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    ability = -1;
    for ( iAbility = 0 ; iAbility < MAX_ABILITY ; iAbility++ )
    {
       if ( !str_prefix( arg3 , ability_name[iAbility] ) )
       {
         ability = iAbility;
         break;
       }
    }
    
    if ( ability == -1 )
    {
	send_to_char( "No Such Ability.\n\r", ch);
	do_advance(ch, "" );
	return;
    }

    
    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    /* You can demote yourself but not someone else at your own trust. -- Narn */
    if ( get_trust( ch ) <= get_trust( victim ) && ch != victim )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( level = atoi( arg2 ) ) < 1 || level > 500 )
    {
	send_to_char( "Level must be 1 to 500.\n\r", ch );
	return;
    }

    /*
     * Lower level:
     *   Reset to level 1.
     *   Then raise again.
     *   Currently, an imp can lower another imp.
     *   -- Swiftest
     */
    if ( level <= victim->skill_level[ability] )
    {
	send_to_char( "Lowering a player's level!\n\r", ch );
	set_char_color( AT_IMMORT, victim );
	send_to_char( "Cursed and forsaken! The gods have lowered your level.\n\r", victim );
        victim->experience[ability] = 0;
        victim->skill_level[ability] = 1;
        if ( ability == COMBAT_ABILITY )
          victim->max_hit = 500;
          victim->max_mana = 0;
    }
    else
    {
	send_to_char( "Raising a player's level!\n\r", ch );
	  send_to_char( "The gods feel fit to raise your level!\n\r", victim );
    }

    for ( iLevel = victim->skill_level[ability] ; iLevel < level; iLevel++ )
    {
	victim->experience[ability] = exp_level(iLevel+1);
	gain_exp( victim, 0 , ability );
    }
    return;
}

void do_immortalize( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	int arg3;
    CHAR_DATA *victim;

	set_char_color( AT_IMMORT, ch );

    argument = one_argument( argument, arg );
	argument = one_argument( argument, arg2 );
	if (arg2[0] == '\0' )
		sprintf( arg2, "101");

	arg3 = atoi( arg2 );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax: immortalize <char> <level>\n\r", ch );
	return;
    }

	if (arg3 > get_trust(ch) ) {
		send_to_char("your not high enought to do that",ch);
		return;
	}

    if ( ( victim = get_char_room( ch, arg ) ) == NULL ) {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }
    if ( IS_NPC( victim ) ) {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    send_to_char( "Immortalizing a player...\n\r", ch );
    set_char_color( AT_IMMORT, victim );
    if ( victim->top_level <= LEVEL_AVATAR && IS_IMMORTAL(victim)) 
    {
        char buf[MAX_INPUT_LENGTH];
        if ( victim->pcdata->bestowments )
              DISPOSE( victim->pcdata->bestowments );
        victim->pcdata->bestowments = str_dup("");
        REMOVE_BIT( victim->act, PLR_HOLYLIGHT );
        if ( !IS_RETIRED( victim ) ) {
              sprintf( buf, "%s%s", GOD_DIR, capitalize(ch->name) );
              remove( buf );
        }
    }
    act( AT_IMMORT, "$n begins to chant softly... then raises $s arms to the sky...",
	 ch, NULL, NULL, TO_ROOM );
    victim->top_level = arg3;
    advance_level( victim ,COMBAT_ABILITY);
 
    victim->trust = 0;
    save_char_obj (victim);
    return;
}

void do_trust( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int level;

	set_char_color( AT_IMMORT, ch );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) ) {
	send_to_char( "Syntax: trust <char> <level>.\n\r", ch );
	return;
    }
    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL ) {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }
    if ( ( level = atoi( arg2 ) ) < 0 || level > MAX_LEVEL ) {
	send_to_char( "Level must be 0 (reset) or 1 to 60.\n\r", ch );
	return;
    }
    if ( level > get_trust( ch ) ) {
	send_to_char( "Limited to your own trust.\n\r", ch );
	return;
    }
    if ( get_trust( victim ) >= get_trust( ch ) ) {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    victim->trust = level;
    send_to_char( "Ok.\n\r", ch );
    return;
}

void do_restoretime( CHAR_DATA *ch, char *argument )
{
  long int time_passed;
  int hour, minute;

  if ( !last_restore_all_time )
     ch_printf( ch, "There has been no restore all since reboot\n\r");
  else
     {
     time_passed = current_time - last_restore_all_time;
     hour = (int) ( time_passed / 3600 );
     minute = (int) ( ( time_passed - ( hour * 3600 ) ) / 60 );
     ch_printf( ch, "The  last restore all was %d hours and %d minutes ago.\n\r", 
                  hour, minute );
     }

  if ( !ch->pcdata )
    return;

  if ( !ch->pcdata->restore_time )
  {
    send_to_char( "You have never done a restore all.\n\r", ch );
    return;
  }

  time_passed = current_time - ch->pcdata->restore_time;
  hour = (int) ( time_passed / 3600 );
  minute = (int) ( ( time_passed - ( hour * 3600 ) ) / 60 );
  ch_printf( ch, "Your last restore all was %d hours and %d minutes ago.\n\r", 
                  hour, minute ); 
  return;
}

void do_freeze( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    set_char_color( AT_LBLUE, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' ) {
	send_to_char( "Freeze whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL ) {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC( victim ) ) {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    set_char_color( AT_LBLUE, victim );
    if ( get_trust( victim ) >= get_trust( ch ) ) {
	send_to_char( "You failed, and they saw...\n\r", ch );
        ch_printf( victim, "%s is attempting to freeze you.\n\r", ch->name );
	return;
    }
    if ( IS_SET(victim->act, PLR_FREEZE ) ) {
	REMOVE_BIT(victim->act, PLR_FREEZE );
	send_to_char( "Your frozen form suddenly thaws.\n\r", victim );
	ch_printf( ch, "%s is now unfrozen.\n\r", victim->name );
    } else {
	SET_BIT(victim->act, PLR_FREEZE );
	send_to_char( "A godly force turns your body to ice!\n\r", victim );
	ch_printf( ch, "You have frozen %s.\n\r", victim->name );
    }
    save_char_obj( victim );
    return;
}

void do_log( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

	set_char_color( AT_IMMORT, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Log whom?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	if ( fLogAll )
	{
	    fLogAll = FALSE;
	    send_to_char( "Log ALL off.\n\r", ch );
	}
	else
	{
	    fLogAll = TRUE;
	    send_to_char( "Log ALL on.\n\r", ch );
	}
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    /*
     * No level check, gods can log anyone.
     */

    if ( IS_SET(victim->act, PLR_LOG) )
    {
	REMOVE_BIT(victim->act, PLR_LOG);
	send_to_char( "LOG removed.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_LOG);
	send_to_char( "LOG set.\n\r", ch );
    }
    return;
}

void do_litterbug( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    set_char_color( AT_IMMORT, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Set litterbug flag on whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    set_char_color( AT_IMMORT, victim );
    if ( IS_SET(victim->act, PLR_LITTERBUG) )
    {
	REMOVE_BIT(victim->act, PLR_LITTERBUG);
	send_to_char( "You can drop items again.\n\r", victim );
	ch_printf( ch, "LITTERBUG removed from %s.\n\r", victim->name );
    }
    else
    {
	SET_BIT(victim->act, PLR_LITTERBUG);
	send_to_char( "A strange force prevents you from dropping any more items!\n\r", victim );
	ch_printf( ch, "LITTERBUG set on %s.\n\r", victim->name );
    }
    return;
}

void do_noemote( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    set_char_color( AT_IMMORT, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Noemote whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
    set_char_color( AT_IMMORT, victim );
    if ( IS_SET(victim->act, PLR_NO_EMOTE) )
    {
	REMOVE_BIT(victim->act, PLR_NO_EMOTE);
	send_to_char( "You can emote again.\n\r", victim );
	ch_printf( ch, "NOEMOTE removed from %s.\n\r", victim->name );
    }
    else
    {
	SET_BIT(victim->act, PLR_NO_EMOTE);
	send_to_char( "You can't emote!\n\r", victim );
	ch_printf( ch, "NOEMOTE applied to %s.\n\r", victim->name );
    }
    return;
}

void do_notell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    set_char_color( AT_IMMORT, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Notell whom?", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
    set_char_color( AT_IMMORT, victim );
    if ( IS_SET(victim->act, PLR_NO_TELL) )
    {
	REMOVE_BIT(victim->act, PLR_NO_TELL);
	send_to_char( "You can use tells again.\n\r", victim );
	ch_printf( ch, "NOTELL removed from %s.\n\r", victim->name );
    }
    else
    {
	SET_BIT(victim->act, PLR_NO_TELL);
	send_to_char( "You can't use tells!\n\r", victim );
	ch_printf( ch, "NOTELL applied to %s.\n\r", victim->name );
    }
    return;
}

void do_notitle( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
 
    set_char_color( AT_IMMORT, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Notitle whom?\n\r", ch );
        return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
    if ( IS_NPC( victim ) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }
    if ( get_trust( victim ) >= get_trust( ch ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }
    set_char_color( AT_IMMORT, victim );
    if ( IS_SET(victim->pcdata->flags, PCFLAG_NOTITLE) )
    {
        REMOVE_BIT(victim->pcdata->flags, PCFLAG_NOTITLE);
        send_to_char( "You can set your own title again.\n\r", victim );
	ch_printf( ch, "NOTITLE removed from %s.\n\r", victim->name );
    }
    else
    {
        SET_BIT(victim->pcdata->flags, PCFLAG_NOTITLE);
        sprintf( buf, "%s the %s",
                victim->name, race_table[victim->race]->race_name);
        set_title( victim, buf );   
        send_to_char( "You can't set your own title!\n\r", victim );
	ch_printf( ch, "NOTITLE set on %s.\n\r", victim->name );
    }
    return;
}

void do_silence( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    set_char_color( AT_IMMORT, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Silence whom?", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
    set_char_color( AT_IMMORT, victim );
    if ( IS_SET(victim->act, PLR_SILENCE) )
    {
	send_to_char( "Player already silenced, use unsilence to remove.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_SILENCE);
	send_to_char( "You can't use channels!\n\r", victim );
	ch_printf( ch, "You SILENCE %s.\n\r", victim->name );
    }
    return;
}

/* Much better than toggling this with do_silence, yech --Blodkai */
void do_unsilence( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    set_char_color( AT_IMMORT, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Unsilence whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
    set_char_color( AT_IMMORT, victim );
    if ( IS_SET(victim->act, PLR_SILENCE) )
    {
	REMOVE_BIT(victim->act, PLR_SILENCE);
	send_to_char( "You can use channels again.\n\r", victim );
	ch_printf( ch, "SILENCE removed from %s.\n\r", victim->name );
    }
    else
    {
	send_to_char( "That player is not silenced.\n\r", ch );
    }
    return;
}

void do_peace( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    act( AT_IMMORT, "$n booms, 'PEACE!'", ch, NULL, NULL, TO_ROOM );
    act( AT_IMMORT, "You boom, 'PEACE!'", ch, NULL, NULL, TO_CHAR );
    for ( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
    {
	if ( rch->fighting )
	{
	    stop_fighting( rch, TRUE );
	    do_sit( rch, "" );
	}
       
        /* Added by Narn, Nov 28/95 */
        stop_hating( rch );
        stop_hunting( rch );
        stop_fearing( rch );
    }

    send_to_char_color( "&YOk.\n\r", ch );
    return;
}

WATCH_DATA *		first_watch;
WATCH_DATA *		last_watch;

void save_watchlist( void )
{
  WATCH_DATA *pwatch;
  FILE *fp;

  fclose( fpReserve );
  if ( !(fp = fopen( SYSTEM_DIR WATCH_LIST, "w" )) )
  {
    bug( "Save_watchlist: Cannot open " WATCH_LIST, 0 );
    perror(WATCH_LIST);
    fpReserve = fopen( NULL_FILE, "r" );
    return;
  }

  for ( pwatch = first_watch; pwatch; pwatch = pwatch->next )
      fprintf( fp, "%d %s~%s~%s~\n", pwatch->imm_level, pwatch->imm_name, 
               pwatch->target_name?pwatch->target_name:" ", 
	       pwatch->player_site?pwatch->player_site:" " );
  fprintf( fp, "-1\n" );
  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );
  return;
}

void do_wizlock( CHAR_DATA *ch, char *argument )
{
    extern bool wizlock;
    wizlock = !wizlock;

	set_char_color( AT_DANGER, ch );

    if ( wizlock )
	send_to_char( "Game wizlocked.\n\r", ch );
    else
	send_to_char( "Game un-wizlocked.\n\r", ch );
    return;
}

void do_noresolve( CHAR_DATA *ch, char *argument )
{
    sysdata.NO_NAME_RESOLVING = !sysdata.NO_NAME_RESOLVING;

    if ( sysdata.NO_NAME_RESOLVING )
	send_to_char( "Name resolving disabled.\n\r", ch );
    else
	send_to_char( "Name resolving enabled.\n\r", ch );
    return;
}

void do_users( CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA *d;
	int count;
	char arg[MAX_INPUT_LENGTH];
    
	set_pager_color( AT_PLAIN, ch );
    
	one_argument (argument, arg);
	count	= 0;
	
	send_to_pager("\n\rDesc|Con|Idle| Port | Player      @HostIP           | Username\n\r"
		"----+---+----+------+-------------------------------+---------\n\r",ch);
	
	for ( d = first_descriptor; d; d = d->next )
	{
		if (arg[0] == '\0')
		{     
			if (  get_trust(ch) >= LEVEL_SUPREME
				||   (d->character && can_see( ch, d->character )) )
			{
				count++;
				pager_printf( ch,
					" %3d| %2d|%4d|%6d| %-12s@%-16s | %s\n\r",
					d->descriptor, d->connected, d->idle / 4,
					d->port, d->original  ? d->original->name  :
				d->character ? d->character->name : "(none)",
					d->host,d->user);
			}
		}
		else
		{
			if ( (get_trust(ch) >= LEVEL_SUPREME
				||   (d->character && can_see( ch, d->character )) )
				&&   ( !str_prefix( arg, d->host ) 
				||   ( d->character && !str_prefix( arg, d->character->name ) ) ) )
			{
				count++;
				pager_printf( ch,
					" %3d| %2d|%4d|%6d| %-12s@%-16s | %s\n\r",
					d->descriptor, d->connected, d->idle / 4,
					d->port, d->original  ? d->original->name  :
				d->character ? d->character->name : "(none)",
					d->host, d->user );
			}
		}
	}
	pager_printf( ch, "%d user%s.\n\r", count, count == 1 ? "" : "s" );
	return;
}

/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
void do_force( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_INPUT_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	bool mobsonly; 
    
	set_char_color( AT_IMMORT, ch );
    argument = one_argument( argument, arg );
	if ( arg[0] == '\0' || argument[0] == '\0' )
	{
		send_to_char( "Force whom to do what?\n\r", ch );
		return;
	}
	
	mobsonly = get_trust( ch ) < sysdata.level_forcepc; 

    if ( !str_cmp( arg, "all" ) )
	{
		CHAR_DATA *vch;
		CHAR_DATA *vch_next;
        
		if ( mobsonly )
		{
			send_to_char( "Force whom to do what?\n\r", ch );
			return;
		} 
		
		if ( !str_prefix("mp",argument) && get_trust( ch ) < LEVEL_IMPLEMENTOR )
		{
			sprintf( buf , "&RWARNING: &G&W %s is attempting to cheat.\n\r",ch->name );
			echo_to_all( AT_WHITE, buf, ECHOTAR_ALL );
			send_to_char( "But that's cheating!\n\r", ch );
			return;
		}
		
		for ( vch = first_char; vch; vch = vch_next )
		{
			vch_next = vch->next;
			if ( !IS_NPC(vch) && get_trust( vch ) < get_trust( ch ) )
			{
				act( AT_IMMORT, "$n forces you to '$t'.", ch, argument, vch, TO_VICT );
				interpret( vch, argument );
			}
		}
	}
	else
	{
		CHAR_DATA *victim;
		if ( ( victim = get_char_world( ch, arg ) ) == NULL )
		{
			send_to_char( "They aren't here.\n\r", ch );
			return;
		}
		if ( victim == ch )
		{
			send_to_char( "Aye aye, right away!\n\r", ch );
			return;
		}
		if ( ( get_trust( victim ) >= get_trust( ch ) ) 
			|| ( mobsonly && !IS_NPC( victim ) ) )
		{
			send_to_char( "Do it yourself!\n\r", ch );
			return;
		}
		if ( !str_prefix("mp",argument) && get_trust( ch ) < LEVEL_IMPLEMENTOR )
		{
			sprintf( buf , "&RWARNING: &G&W %s is attempting to cheat.\n\r",ch->name );
			echo_to_all( AT_WHITE, buf, ECHOTAR_ALL );
			send_to_char( "But that's cheating!\n\r", ch );
			return;
		}
		if ( get_trust( ch ) < LEVEL_GOD && IS_NPC(victim)
			&& !str_prefix( "mp", argument ) )
		{
			send_to_char("You can't force a mob to do that!\n\r", ch );
			return;
		}
		act( AT_IMMORT, "$n forces you to '$t'.", ch, argument, victim, TO_VICT );
		interpret( victim, argument );
	}
    send_to_char( "Ok.\n\r", ch );
	return;
}

void do_invis( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    sh_int level;

    set_char_color( AT_IMMORT, ch );
    
    /* if ( IS_NPC(ch)) return; */

    argument = one_argument( argument, arg );
    if ( arg && arg[0] != '\0' )
    {
	if ( !is_number( arg ) )
	{
	   send_to_char( "Usage: invis | invis <level>\n\r", ch );
	   return;
	}
	level = atoi( arg );
	if ( level < 2 || level > get_trust( ch ) )
	{
	    send_to_char( "Invalid level.\n\r", ch );
	    return;
	}

	if (!IS_NPC(ch))
        {
	  ch->pcdata->wizinvis = level;
	  ch_printf( ch, "Wizinvis level set to %d.\n\r", level );
        }
        
        if (IS_NPC(ch))       
        {
          ch->mobinvis = level;
          ch_printf( ch, "Mobinvis level set to %d.\n\r", level );
        }
	return;
    }
    
    if (!IS_NPC(ch))
    {
	if ( ch->pcdata->wizinvis < 2 )
	  ch->pcdata->wizinvis = ch->top_level;
    }
    if (IS_NPC(ch))
    {
	if ( ch->mobinvis < 2 )
	  ch->mobinvis = ch->top_level;
    }
    if ( IS_SET(ch->act, PLR_WIZINVIS) )
    {
	REMOVE_BIT(ch->act, PLR_WIZINVIS);
	act( AT_IMMORT, "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "You slowly fade back into existence.\n\r", ch );
    }
    else
    {
	act( AT_IMMORT, "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "You slowly vanish into thin air.\n\r", ch );
	SET_BIT(ch->act, PLR_WIZINVIS);
    }
    return;
}

void do_holylight( CHAR_DATA *ch, char *argument )
{

    set_char_color( AT_IMMORT, ch );

    if ( IS_NPC(ch) )
	return;

    if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
    {
	REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Holy light mode off.\n\r", ch );
    }
    else
    {
	SET_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Holy light mode on.\n\r", ch );
    }
    return;
}

void do_rassign( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    int  r_lo, r_hi;
    CHAR_DATA *victim;

	set_char_color( AT_IMMORT, ch );
    
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    r_lo = atoi( arg2 );  r_hi = atoi( arg3 );

    if ( arg1[0] == '\0' || r_lo < 0 || r_hi < 0 )
    {
	send_to_char( "Syntax: assign <who> <low> <high>\n\r", ch );
	return;
    }
    if ( (victim = get_char_world( ch, arg1 )) == NULL )
    {
	send_to_char( "They don't seem to be around.\n\r", ch );
	return;
    }
    if ( IS_NPC( victim ) || get_trust( victim ) < LEVEL_AVATAR )
    {
	send_to_char( "They wouldn't know what to do with a room range.\n\r", ch );
	return;
    }
    if ( r_lo > r_hi )
    {
	send_to_char( "Unacceptable room range.\n\r", ch );
	return;
    }
    if ( r_lo == 0 )
       r_hi = 0;
    victim->pcdata->r_range_lo = r_lo;
    victim->pcdata->r_range_hi = r_hi;                      
    assign_area( victim );
    send_to_char( "Done.\n\r", ch );
	set_char_color( AT_IMMORT, victim );
    ch_printf( victim, "%s has assigned you the room range %d - %d.\n\r",
		ch->name, r_lo, r_hi );
    assign_area( victim );	/* Put back by Thoric on 02/07/96 */
    if ( !victim->pcdata->area )
    {
	bug( "rassign: assign_area failed", 0 );
    	return;
    }

    if (r_lo == 0)				/* Scryn 8/12/95 */
    {
	REMOVE_BIT ( victim->pcdata->area->status, AREA_LOADED );
	SET_BIT( victim->pcdata->area->status, AREA_DELETED );
    }
    else
    {
        SET_BIT( victim->pcdata->area->status, AREA_LOADED );
	REMOVE_BIT( victim->pcdata->area->status, AREA_DELETED );
    }
    return;
}

void do_vassign( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
	int  r_lo, r_hi;
    CHAR_DATA *victim;
    
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    r_lo = atoi( arg2 );  r_hi = atoi( arg3 );

    if ( arg1[0] == '\0' || r_lo < 0 || r_hi < 0 )
    {
	send_to_char( "Syntax: vassign <who> <low> <high>\n\r", ch );
	return;
    }
    if ( (victim = get_char_world( ch, arg1 )) == NULL )
    {
	send_to_char( "They don't seem to be around.\n\r", ch );
	return;
    }
    if ( IS_NPC( victim ) || get_trust( victim ) < LEVEL_CREATOR )
    {
	send_to_char( "They wouldn't know what to do with a vnum range.\n\r", ch );
	return;
    }
    if ( r_lo > r_hi )
    {
	send_to_char( "Unacceptable room range.\n\r", ch );
	return;
    }
    if ( r_lo == 0 )
       r_hi = 0;
    victim->pcdata->r_range_lo = r_lo;
    victim->pcdata->r_range_hi = r_hi;
        victim->pcdata->o_range_lo = r_lo;
            victim->pcdata->o_range_hi = r_hi;
    victim->pcdata->m_range_lo = r_lo;
        victim->pcdata->m_range_hi = r_hi;
                     
    assign_area( victim );
    send_to_char( "Done.\n\r", ch );
    ch_printf( victim, "%s has assigned you the vnum range %d - %d.\n\r",
		ch->name, r_lo, r_hi );
    assign_area( victim );	/* Put back by Thoric on 02/07/96 */
    if ( !victim->pcdata->area )
    {
	bug( "rassign: assign_area failed", 0 );
    	return;
    }

    if (r_lo == 0)				/* Scryn 8/12/95 */
    {
	REMOVE_BIT ( victim->pcdata->area->status, AREA_LOADED );
	SET_BIT( victim->pcdata->area->status, AREA_DELETED );
    }
    else
    {
        SET_BIT( victim->pcdata->area->status, AREA_LOADED );
	REMOVE_BIT( victim->pcdata->area->status, AREA_DELETED );
    }
    return;
}

void do_oassign( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    int  o_lo, o_hi;
    CHAR_DATA *victim;
    
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    o_lo = atoi( arg2 );  o_hi = atoi( arg3 );

    if ( arg1[0] == '\0' || o_lo < 0 || o_hi < 0 )
    {
	send_to_char( "Syntax: oassign <who> <low> <high>\n\r", ch );
	return;
    }
    if ( (victim = get_char_world( ch, arg1 )) == NULL )
    {
	send_to_char( "They don't seem to be around.\n\r", ch );
	return;
    }
    if ( IS_NPC( victim ) || get_trust( victim ) < LEVEL_SAVIOR )
    {
	send_to_char( "They wouldn't know what to do with an object range.\n\r", ch );
	return;
    }
    if ( o_lo > o_hi )
    {
	send_to_char( "Unacceptable object range.\n\r", ch );
	return;
    }
    victim->pcdata->o_range_lo = o_lo;
    victim->pcdata->o_range_hi = o_hi;
    assign_area( victim );
    send_to_char( "Done.\n\r", ch );
    ch_printf( victim, "%s has assigned you the object vnum range %d - %d.\n\r",
		ch->name, o_lo, o_hi );
    return;
}

void do_massign( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    int  m_lo, m_hi;
    CHAR_DATA *victim;

    set_char_color( AT_IMMORT, ch );
    
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    m_lo = atoi( arg2 );  m_hi = atoi( arg3 );

    if ( arg1[0] == '\0' || m_lo < 0 || m_hi < 0 )
    {
	send_to_char( "Syntax: massign <who> <low> <high>\n\r", ch );
	return;
    }
    if ( (victim = get_char_world( ch, arg1 )) == NULL )
    {
	send_to_char( "They don't seem to be around.\n\r", ch );
	return;
    }
    if ( IS_NPC( victim ) || get_trust( victim ) < LEVEL_SAVIOR )
    {
	send_to_char( "They wouldn't know what to do with a monster range.\n\r", ch );
	return;
    }
    if ( m_lo > m_hi )
    {
	send_to_char( "Unacceptable monster range.\n\r", ch );
	return;
    }
    victim->pcdata->m_range_lo = m_lo;
    victim->pcdata->m_range_hi = m_hi;
    assign_area( victim );
    send_to_char( "Done.\n\r", ch );
    set_char_color( AT_IMMORT, victim );
    ch_printf( victim, "%s has assigned you the monster vnum range %d - %d.\n\r",
		ch->name, m_lo, m_hi );
    return;
}

void do_cmdtable( CHAR_DATA *ch, char *argument )
{
    int hash, cnt;
    CMDTYPE *cmd;
    char arg[MAX_INPUT_LENGTH];
    
    one_argument(argument, arg);
    
    if(strcmp(arg, "lag")) /* display normal command table */
    { 
    	set_pager_color( AT_IMMORT, ch );
    	send_to_pager("Commands and Number of Uses This Run\n\r", ch);
    	set_pager_color( AT_PLAIN, ch ); 
    	for ( cnt = hash = 0; hash < 126; hash++ )
		for ( cmd = command_hash[hash]; cmd; cmd = cmd->next )
		{
		    if ((++cnt)%4)
			pager_printf(ch,"%-6.6s %4d\t",cmd->name,cmd->userec.num_uses);
		    else
			pager_printf(ch,"%-6.6s %4d\n\r", cmd->name,cmd->userec.num_uses );
		}
	send_to_char( "\n\r", ch );
    }
    else	/* display commands causing lag */
    {
    	set_pager_color(AT_IMMORT, ch);
    	send_to_pager("Commands that have caused lag this run\n\r", ch);
    	set_pager_color(AT_PLAIN, ch);
    	for(cnt = hash = 0; hash < 126; hash++)
    		for(cmd = command_hash[hash]; cmd; cmd = cmd->next)
    		{
    			if(!cmd->lag_count)
    				continue;
    			else if((++cnt)%4)
    				pager_printf(ch,"%-6.6s %4d\t",cmd->name,cmd->lag_count);
		    	else
				pager_printf(ch,"%-6.6s %4d\n\r", cmd->name,cmd->lag_count);
		}
	send_to_char("\n\r", ch);
    }
    	
    return;
}

/*
 * Load up a player file
 */
void do_loadup( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *temp;
    char fname[1024];
    char name[256];
    struct stat fst;
    bool loaded;
    DESCRIPTOR_DATA *d;
    int old_room_vnum;
    char buf[MAX_STRING_LENGTH];

    set_char_color( AT_IMMORT, ch );

    one_argument( argument, name );
    if ( name[0] == '\0' )
    {
	send_to_char( "Usage: loadup <playername>\n\r", ch );
	return;
    }
    for ( temp = first_char; temp; temp = temp->next )
    {
          if (  IS_NPC(temp) ) continue;
          if ( can_see (ch, temp) && !str_cmp( name, temp->name ) )
                  break;
    }
    if ( temp != NULL )
    {
          send_to_char ("They are already playing.\n\r", ch );
          return;
    }
    name[0] = UPPER(name[0]);
    sprintf( fname, "%s%c/%s", PLAYER_DIR, tolower(name[0]),
			capitalize( name ) );

    if ( stat( fname, &fst ) != -1 )
    {
	CREATE( d, DESCRIPTOR_DATA, 1 );
	d->next = NULL;
	d->prev = NULL;
	d->connected = CON_GET_NAME;
	d->outsize = 2000;
	CREATE( d->outbuf, char, d->outsize );
	
	loaded = load_char_obj( d, name, FALSE );
	add_char( d->character );
        old_room_vnum = d->character->in_room->vnum;
	char_to_room( d->character, ch->in_room );
	if ( get_trust(d->character) >= get_trust( ch ) )
	{
	   do_say( d->character, "Do *NOT* disturb me again!" );
	   send_to_char( "I think you'd better leave that player alone!\n\r", ch );
	   d->character->desc	= NULL;
	   do_quit( d->character, "" );
	   return;	   
	}
	d->character->desc	= NULL;
	d->character->retran    = old_room_vnum;
	d->character		= NULL;	
	DISPOSE( d->outbuf );
	DISPOSE( d );
	ch_printf(ch, "Player %s loaded from room %d.\n\r", capitalize( name ),old_room_vnum );
	sprintf(buf, "%s appears from nowhere, eyes glazed over.\n\r", capitalize( name ) );
        act( AT_IMMORT, buf, ch, NULL, NULL, TO_ROOM );
	send_to_char( "Done.\n\r", ch );
	return;
    }
    /* else no player file */
    send_to_char( "No such player.\n\r", ch );
    return;
}

void do_fixchar( CHAR_DATA *ch, char *argument )
{
    char name[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, name );
    if ( name[0] == '\0' )
    {
	send_to_char( "Usage: fixchar <playername>\n\r", ch );
	return;
    }
    victim = get_char_room( ch, name );
    if ( !victim )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }
    fix_char( victim );
/*  victim->armor	= 100;
    victim->mod_str	= 0;
    victim->mod_dex	= 0;
    victim->mod_wis	= 0;
    victim->mod_int	= 0;
    victim->mod_con	= 0;
    victim->mod_cha	= 0;
    victim->mod_lck	= 0;
    victim->damroll	= 0;
    victim->hitroll	= 0;
    victim->alignment	= URANGE( -1000, victim->alignment, 1000 );
    victim->saving_spell_staff = 0; */
    send_to_char( "Done.\n\r", ch );
}

void do_newbieset( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *victim;
        
    argument = one_argument( argument, arg1 );
    argument = one_argument (argument, arg2);

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax: newbieset <char>.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ( victim->top_level < 1 ) || ( victim->top_level > 5 ) )
    {
     send_to_char( "Level of victim must be 1 to 5.\n\r", ch );
	return;
    }
     obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SHIELD), 1 );
     obj_to_char(obj, victim);

     obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_DAGGER), 1 );
     obj_to_char(obj, victim);
  
     /* Added by Brittany, on Nov. 24, 1996. The object is the adventurer's 
          guide to the realms of despair, part of academy.are. */
     {
       OBJ_INDEX_DATA *obj_ind = get_obj_index( 10333 );
       if ( obj_ind != NULL )
       {
         obj = create_object( obj_ind, 1 );
         obj_to_char( obj, victim );
       }
     }

/* Added the burlap sack to the newbieset.  The sack is part of sgate.are
   called Spectral Gate.  Brittany */

     {

       OBJ_INDEX_DATA *obj_ind = get_obj_index( 123 );
       if ( obj_ind != NULL )
       {
         obj = create_object( obj_ind, 1 );
         obj_to_char( obj, victim );
       }
     }

    act( AT_IMMORT, "$n has equipped you with a newbieset.", ch, NULL, victim, TO_VICT);
    ch_printf( ch, "You have re-equipped %s.\n\r", victim->name );
    return;
}

/*
 * Extract area names from "input" string and place result in "output" string
 * e.g. "aset joe.are sedit susan.are cset" --> "joe.are susan.are"
 * - Gorog
 */
void extract_area_names (char *inp, char *out)
{
char buf[MAX_INPUT_LENGTH], *pbuf=buf;
int  len;

*out='\0';
while (inp && *inp)
   {
   inp = one_argument(inp, buf);
   if ( (len=strlen(buf)) >= 5 && !strcmp(".are", pbuf+len-4) )
       {
       if (*out) strcat (out, " ");
       strcat (out, buf);
       }
   }
}

/*
 * Remove area names from "input" string and place result in "output" string
 * e.g. "aset joe.are sedit susan.are cset" --> "aset sedit cset"
 * - Gorog
 */
void remove_area_names (char *inp, char *out)
{
char buf[MAX_INPUT_LENGTH], *pbuf=buf;
int  len;

*out='\0';
while (inp && *inp)
   {
   inp = one_argument(inp, buf);
   if ( (len=strlen(buf)) < 5 || strcmp(".are", pbuf+len-4) )
       {
       if (*out) strcat (out, " ");
       strcat (out, buf);
       }
   }
}

/*
 * Allows members of the Area Council to add Area names to the bestow field.
 * Area names mus end with ".are" so that no commands can be bestowed.
 */
void do_bestowarea( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
	AREA_DATA * pArea;

    set_char_color( AT_IMMORT, ch );

    argument = one_argument( argument, arg );

    if ( !arg || arg[0] == '\0' || !argument || argument[0] == '\0' )
    {
        send_to_char(
        "Syntax:\n\r"
        "bestowarea <victim> <filename>.are\n\r"
        "bestowarea <victim> none             removes bestowed areas\n\r"
        "bestowarea <victim> list             lists bestowed areas\n\r"
        "bestowarea <victim>                  lists bestowed areas\n\r", ch);
        return;
    }
    if ( !(victim = get_char_world( ch, arg )) )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC( victim ) )
    {
        send_to_char( "You can't give special abilities to a mob!\n\r", ch );
        return;
    }
    if ( get_trust(victim) < LEVEL_IMMORTAL )
    {
        send_to_char( "They aren't an immortal.\n\r", ch );
        return;
    }

    if (!victim->pcdata->bestowments)
       victim->pcdata->bestowments = str_dup("");

    if ( !*argument || !str_cmp (argument, "list") )
    {
       extract_area_names (victim->pcdata->bestowments, buf);
       ch_printf( ch, "Bestowed areas: %s\n\r", buf);
       return;
    }
    if ( !str_cmp (argument, "none") )
    {
       remove_area_names (victim->pcdata->bestowments, buf);
       DISPOSE( victim->pcdata->bestowments );
       victim->pcdata->bestowments = str_dup( buf );
       send_to_char( "Done.\n\r", ch);
       return;
    }
	if ( ( pArea = get_area_by_filename(argument) ) == NULL )
	{
		send_to_char( "You can only bestow area names\n\r", ch);
		return;
	}
	sprintf( buf, "%s %s", victim->pcdata->bestowments, pArea->filename );
	DISPOSE( victim->pcdata->bestowments );
    victim->pcdata->bestowments = str_dup( buf );
    set_char_color( AT_IMMORT, victim );
    ch_printf( victim, "%s has bestowed on you the area: %s\n\r", 
             ch->name, pArea->filename );
    send_to_char( "Done.\n\r", ch );
}

void do_bestow( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    set_char_color( AT_IMMORT, ch );

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Bestow whom with what?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC( victim ) )
    {
	send_to_char( "You can't give special abilities to a mob!\n\r", ch );
	return;
    }
    if ( get_trust( victim ) > get_trust( ch ) )
    {
	send_to_char( "You aren't powerful enough...\n\r", ch );
	return;
    }

    if (!victim->pcdata->bestowments)
      victim->pcdata->bestowments = str_dup("");

    if ( argument[0] == '\0' || !str_cmp( argument, "list" ) )
    {
        ch_printf( ch, "Current bestowed commands on %s: %s.\n\r",
		victim->name, victim->pcdata->bestowments );
        return;
    }
    if ( !str_cmp( argument, "none" ) )
    {
        DISPOSE( victim->pcdata->bestowments );
	victim->pcdata->bestowments = str_dup("");
        ch_printf( ch, "Bestowments removed from %s.\n\r", victim->name );
        ch_printf( victim, "%s has removed your bestowed commands.\n\r", ch->name );
        return;
    }

    sprintf( buf, "%s %s", victim->pcdata->bestowments, argument );
    DISPOSE( victim->pcdata->bestowments );
    victim->pcdata->bestowments = str_dup( buf );
    set_char_color( AT_IMMORT, victim );
    ch_printf( victim, "%s has bestowed on you the command(s): %s\n\r",
	ch->name, argument );
    send_to_char( "Done.\n\r", ch );
}

struct tm *update_time ( struct tm *old_time )
{
   time_t time;

   time = mktime(old_time); 
   return localtime(&time);
}

void do_set_boot_time( CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   bool check;
 
   check = FALSE;
   set_char_color( AT_IMMORT, ch );

   argument = one_argument(argument, arg);
   if ( arg[0] == '\0' )
   {
	send_to_char( "Syntax: setboot time {hour minute <day> <month> <year>}\n\r", ch);
	send_to_char( "        setboot manual {0/1}\n\r", ch);
	send_to_char( "        setboot default\n\r", ch); 
        ch_printf( ch, "Boot time is currently set to %s, manual bit is set to %d\n\r",
		reboot_time, set_boot_time->manual );
	return;
    }

    if ( !str_cmp(arg, "time") )
    {
      struct tm *now_time;
      
      argument = one_argument(argument, arg);
      argument = one_argument(argument, arg1);
      if ( !*arg || !*arg1 || !is_number(arg) || !is_number(arg1) )
      {
	send_to_char("You must input a value for hour and minute.\n\r", ch);
 	return;
      }

      now_time = localtime(&current_time);
      if ( (now_time->tm_hour = atoi(arg)) < 0 || now_time->tm_hour > 23 )
      {
        send_to_char("Valid range for hour is 0 to 23.\n\r", ch);
        return;
      }
      if ( (now_time->tm_min = atoi(arg1)) < 0 || now_time->tm_min > 59 )
      {
        send_to_char("Valid range for minute is 0 to 59.\n\r", ch);
        return;
      }

      argument = one_argument(argument, arg);
      if ( *arg != '\0' && is_number(arg) )
      {
        if ( (now_time->tm_mday = atoi(arg)) < 1 || now_time->tm_mday > 31 )
        {
	  send_to_char("Valid range for day is 1 to 31.\n\r", ch);
	  return;
        }
        argument = one_argument(argument, arg);
        if ( *arg != '\0' && is_number(arg) )
        {
          if ( (now_time->tm_mon = atoi(arg)) < 1 || now_time->tm_mon > 12 )
          {
            send_to_char( "Valid range for month is 1 to 12.\n\r", ch );
            return;
          }
          now_time->tm_mon--;
          argument = one_argument(argument, arg);
          if ( (now_time->tm_year = atoi(arg)-1900) < 0 ||
                now_time->tm_year > 199 )
          {
            send_to_char( "Valid range for year is 1900 to 2099.\n\r", ch );
            return;
          }
        }
      }

      now_time->tm_sec = 0;
      if ( mktime(now_time) < current_time )
      {
        send_to_char( "You can't set a time previous to today!\n\r", ch );
        return;
      }
      if (set_boot_time->manual == 0)
 	set_boot_time->manual = 1;
      new_boot_time = update_time(now_time);
      new_boot_struct = *new_boot_time;
      new_boot_time = &new_boot_struct;
      reboot_check(mktime(new_boot_time));
      get_reboot_string();

      ch_printf(ch, "Boot time set to %s\n\r", reboot_time);
      check = TRUE;
    }  
    else if ( !str_cmp(arg, "manual") )
    {
      argument = one_argument(argument, arg1);
      if (arg1[0] == '\0')
      {
	send_to_char("Please enter a value for manual boot on/off\n\r", ch);
	return;
      }
      if ( !is_number(arg1))
      {
	send_to_char("Value for manual must be 0 (off) or 1 (on)\n\r", ch);
	return;
      }
      if (atoi(arg1) < 0 || atoi(arg1) > 1)
      {
	send_to_char("Value for manual must be 0 (off) or 1 (on)\n\r", ch);
	return;
      }
   
      set_boot_time->manual = atoi(arg1);
      ch_printf(ch, "Manual bit set to %s\n\r", arg1);
      check = TRUE;
      get_reboot_string();
      return;
    }

    else if (!str_cmp( arg, "default" ))
    {
      set_boot_time->manual = 0;
    /* Reinitialize new_boot_time */
      new_boot_time = localtime(&current_time);
      new_boot_time->tm_mday += 1;
      if (new_boot_time->tm_hour > 12)
      new_boot_time->tm_mday += 1; 
      new_boot_time->tm_hour = 6;
      new_boot_time->tm_min = 0;
      new_boot_time->tm_sec = 0;
      new_boot_time = update_time(new_boot_time);

      sysdata.DENY_NEW_PLAYERS = FALSE;

      send_to_char("Reboot time set back to normal.\n\r", ch);
      check = TRUE;
    }

    if (!check)
    {
      send_to_char("Invalid argument for setboot.\n\r", ch);
      return;
    }
    else
    {
      get_reboot_string();
      new_boot_time_t = mktime(new_boot_time);
    }
}

/* Online high level immortal command for displaying what the encryption
 * of a name/password would be, taking in 2 arguments - the name and the
 * password - can still only change the password if you have access to 
 * pfiles and the correct password
 */ 
void do_form_password( CHAR_DATA *ch, char *argument) 
{
   char arg[MAX_STRING_LENGTH];

   set_char_color( AT_IMMORT, ch );  

   argument = one_argument(argument, arg);
   ch_printf(ch, "Those two arguments encrypted result in:  %s\n\r",
	   crypt(arg, argument));
   return;
}

/*
 * Purge a player file.  No more player.  -- Altrag
 */
void do_destro( CHAR_DATA *ch, char *argument )
{
  set_char_color( AT_RED, ch );
  send_to_char("If you want to destroy a character, spell it out!\n\r",ch);
  return;
}

/*
 * This could have other applications too.. move if needed. -- Altrag
 */
void close_area( AREA_DATA *pArea )
{
  extern ROOM_INDEX_DATA *room_index_hash[MAX_KEY_HASH];
  extern OBJ_INDEX_DATA   *obj_index_hash[MAX_KEY_HASH];
  extern MOB_INDEX_DATA   *mob_index_hash[MAX_KEY_HASH];
  CHAR_DATA *ech;
  CHAR_DATA *ech_next;
  OBJ_DATA *eobj;
  OBJ_DATA *eobj_next;
  int icnt;
  ROOM_INDEX_DATA *rid;
  ROOM_INDEX_DATA *rid_next;
  OBJ_INDEX_DATA *oid;
  OBJ_INDEX_DATA *oid_next;
  MOB_INDEX_DATA *mid;
  MOB_INDEX_DATA *mid_next;
  RESET_DATA *ereset;
  RESET_DATA *ereset_next;
  EXTRA_DESCR_DATA *eed;
  EXTRA_DESCR_DATA *eed_next;
  EXIT_DATA *exit;
  EXIT_DATA *exit_next;
  MPROG_ACT_LIST *mpact;
  MPROG_ACT_LIST *mpact_next;
  MPROG_DATA *mprog;
  MPROG_DATA *mprog_next;
  AFFECT_DATA *paf;
  AFFECT_DATA *paf_next;
  
  for ( ech = first_char; ech; ech = ech_next )
  {
    ech_next = ech->next;
    
    if ( ech->fighting )
      stop_fighting( ech, TRUE );
    if ( IS_NPC(ech) )
    {
      /* if mob is in area, or part of area. */
      if ( URANGE(pArea->low_m_vnum, ech->pIndexData->vnum,
                  pArea->hi_m_vnum) == ech->pIndexData->vnum ||
          (ech->in_room && ech->in_room->area == pArea) )
        extract_char( ech, TRUE );
      continue;
    }
    if ( ech->in_room && ech->in_room->area == pArea )
      do_recall( ech, "" );
  }
  for ( eobj = first_object; eobj; eobj = eobj_next )
  {
    eobj_next = eobj->next;
    /* if obj is in area, or part of area. */
    if ( URANGE(pArea->low_o_vnum, eobj->pIndexData->vnum,
                pArea->hi_o_vnum) == eobj->pIndexData->vnum ||
        (eobj->in_room && eobj->in_room->area == pArea) )
      extract_obj( eobj );
  }
  for ( icnt = 0; icnt < MAX_KEY_HASH; icnt++ )
  {
    for ( rid = room_index_hash[icnt]; rid; rid = rid_next )
    {
      rid_next = rid->next;
      
      for ( exit = rid->first_exit; exit; exit = exit_next )
      {
        exit_next = exit->next;
        if ( rid->area == pArea || exit->to_room->area == pArea )
        {
          STRFREE( exit->keyword );
          STRFREE( exit->description );
          UNLINK( exit, rid->first_exit, rid->last_exit, next, prev );
          DISPOSE( exit );
	  /* Crash bug fix.  I know it could go from the start several times
	   * But you CAN NOT iterate over a link-list and DELETE from it or
	   * Nasty things can and will happen. --Shaddai 
	   */
	  exit = rid->first_exit;
        }
      }
      if ( rid->area != pArea )
        continue;
      STRFREE(rid->name);
      STRFREE(rid->description);
      if ( rid->first_person )
      {
        bug( "close_area: room with people #%d", rid->vnum );
        for ( ech = rid->first_person; ech; ech = ech_next )
        {
          ech_next = ech->next_in_room;
          if ( ech->fighting )
            stop_fighting( ech, TRUE );
          if ( IS_NPC(ech) )
            extract_char( ech, TRUE );
          else
            do_recall( ech, "" );
        }
      }
      if ( rid->first_content )
      {
        bug( "close_area: room with contents #%d", rid->vnum );
        for ( eobj = rid->first_content; eobj; eobj = eobj_next )
        {
          eobj_next = eobj->next_content;
          extract_obj( eobj );
        }
      }
      for ( eed = rid->first_extradesc; eed; eed = eed_next )
      {
        eed_next = eed->next;
        STRFREE( eed->keyword );
        STRFREE( eed->description );
        DISPOSE( eed );
      }
      for ( mpact = rid->mpact; mpact; mpact = mpact_next )
      {
        mpact_next = mpact->next;
        STRFREE( mpact->buf );
        DISPOSE( mpact );
      }
      for ( mprog = rid->mudprogs; mprog; mprog = mprog_next )
      {
        mprog_next = mprog->next;
        STRFREE( mprog->arglist );
        STRFREE( mprog->comlist );
        DISPOSE( mprog );
      }
      if ( rid == room_index_hash[icnt] )
        room_index_hash[icnt] = rid->next;
      else
      {
        ROOM_INDEX_DATA *trid;
        
        for ( trid = room_index_hash[icnt]; trid; trid = trid->next )
          if ( trid->next == rid )
            break;
        if ( !trid )
          bug( "Close_area: rid not in hash list %d", rid->vnum );
        else
          trid->next = rid->next;
      }
      DISPOSE(rid);
    }
    
    for ( mid = mob_index_hash[icnt]; mid; mid = mid_next )
    {
      mid_next = mid->next;
      
      if ( mid->vnum < pArea->low_m_vnum || mid->vnum > pArea->hi_m_vnum )
        continue;
      
      STRFREE( mid->player_name );
      STRFREE( mid->short_descr );
      STRFREE( mid->long_descr  );
      STRFREE( mid->description );
      if ( mid->pShop )
      {
        UNLINK( mid->pShop, first_shop, last_shop, next, prev );
        DISPOSE( mid->pShop );
      }
      if ( mid->rShop )
      {
        UNLINK( mid->rShop, first_repair, last_repair, next, prev );
        DISPOSE( mid->rShop );
      }
      for ( mprog = mid->mudprogs; mprog; mprog = mprog_next )
      {
        mprog_next = mprog->next;
        STRFREE(mprog->arglist);
        STRFREE(mprog->comlist);
        DISPOSE(mprog);
      }
      if ( mid == mob_index_hash[icnt] )
        mob_index_hash[icnt] = mid->next;
      else
      {
        MOB_INDEX_DATA *tmid;
        
        for ( tmid = mob_index_hash[icnt]; tmid; tmid = tmid->next )
          if ( tmid->next == mid )
            break;
        if ( !tmid )
          bug( "Close_area: mid not in hash list %d", mid->vnum );
        else
          tmid->next = mid->next;
      }
      DISPOSE(mid);
    }
    
    for ( oid = obj_index_hash[icnt]; oid; oid = oid_next )
    {
      oid_next = oid->next;
      
      if ( oid->vnum < pArea->low_o_vnum || oid->vnum > pArea->hi_o_vnum )
        continue;
      
      STRFREE(oid->name);
      STRFREE(oid->short_descr);
      STRFREE(oid->description);
      STRFREE(oid->action_desc);

      for ( eed = oid->first_extradesc; eed; eed = eed_next )
      {
        eed_next = eed->next;
        STRFREE(eed->keyword);
        STRFREE(eed->description);
        DISPOSE(eed);
      }
      for ( paf = oid->first_affect; paf; paf = paf_next )
      {
        paf_next = paf->next;
        DISPOSE(paf);
      }
      for ( mprog = oid->mudprogs; mprog; mprog = mprog_next )
      {
        mprog_next = mprog->next;
        STRFREE(mprog->arglist);
        STRFREE(mprog->comlist);
        DISPOSE(mprog);
      }
      if ( oid == obj_index_hash[icnt] )
        obj_index_hash[icnt] = oid->next;
      else
      {
        OBJ_INDEX_DATA *toid;
        
        for ( toid = obj_index_hash[icnt]; toid; toid = toid->next )
          if ( toid->next == oid )
            break;
        if ( !toid )
          bug( "Close_area: oid not in hash list %d", oid->vnum );
        else
          toid->next = oid->next;
      }
      DISPOSE(oid);
    }
  }
  for ( ereset = pArea->first_reset; ereset; ereset = ereset_next )
  {
    ereset_next = ereset->next;
    DISPOSE(ereset);
  }
  DISPOSE(pArea->name);
  DISPOSE(pArea->filename);
  STRFREE(pArea->author);
  UNLINK( pArea, first_build, last_build, next, prev );
  UNLINK( pArea, first_asort, last_asort, next_sort, prev_sort );
  DISPOSE( pArea );
}

void do_destroy( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  char *name;

  set_char_color( AT_RED, ch );

  one_argument( argument, arg );
  if ( arg[0] == '\0' )
  {
      send_to_char( "Destroy what player file?\n\r", ch );
      return;
  }

  for ( victim = first_char; victim; victim = victim->next )
    if ( !IS_NPC(victim) && !str_cmp(victim->name, arg) )
      break;

  if ( !victim )
  {
    DESCRIPTOR_DATA *d;
    
    /* Make sure they aren't halfway logged in. */
    for ( d = first_descriptor; d; d = d->next )
      if ( (victim = d->character) && !IS_NPC(victim) &&
          !str_cmp(victim->name, arg) )
        break;
    if ( d )
      close_socket( d, TRUE );
  }
  else        
  {
    int x, y;
    
    quitting_char = victim;
    save_char_obj( victim );
    saving_char = NULL;
    extract_char( victim, TRUE );
    for ( x = 0; x < MAX_WEAR; x++ )
	for ( y = 0; y < MAX_LAYERS; y++ )
	    save_equipment[x][y] = NULL;
  }
  
  name = capitalize( arg );
  sprintf( buf, "%s%c/%s", PLAYER_DIR, tolower(arg[0]), name );
  sprintf( buf2, "%s%c/%s", BACKUP_DIR, tolower(arg[0]), name );
  if ( !rename( buf, buf2 ) )
  {
    AREA_DATA *pArea;
    
    set_char_color( AT_RED, ch );
    ch_printf(ch, "Player %s destroyed.  Pfile saved in backup directory.\n\r", 
		name );
    sprintf( buf, "%s%s", GOD_DIR, name );
    if ( !remove( buf ) )
      send_to_char( "Player's immortal data destroyed.\n\r", ch );
    else if ( errno != ENOENT )
    {
      ch_printf( ch, "Unknown error #%d - %s (immortal data).  Report to Thoric.\n\r",
              errno, strerror( errno ) );
      sprintf( buf2, "%s destroying %s", ch->name, buf );
      perror( buf2 );
    }

    sprintf( buf2, "%s.are", name );
    for ( pArea = first_build; pArea; pArea = pArea->next )
      if ( !str_cmp( pArea->filename, buf2 ) )
      {
        sprintf( buf, "%s%s", BUILD_DIR, buf2 );
        if ( IS_SET( pArea->status, AREA_LOADED ) )
          fold_area( pArea, buf, FALSE );
        close_area( pArea );
        sprintf( buf2, "%s.bak", buf );
        set_char_color( AT_RED, ch ); /* Log message changes colors */
        if ( !rename( buf, buf2 ) )
          send_to_char( "Player's area data destroyed.  Area saved as backup.\n\r", ch );
        else if ( errno != ENOENT )
        {
          ch_printf( ch, "Unknown error #%d - %s (area data).  Report to Thoric.\n\r",
                  errno, strerror( errno ) );
          sprintf( buf2, "%s destroying %s", ch->name, buf );
          perror( buf2 );
        }
	break;
      }
  }
  else if ( errno == ENOENT )
  {
    set_char_color( AT_PLAIN, ch );
    send_to_char( "Player does not exist.\n\r", ch );
  }
  else
  {
    set_char_color( AT_WHITE, ch );
    ch_printf( ch, "Unknown error #%d - %s.  Report to Thoric.\n\r",
            errno, strerror( errno ) );
    sprintf( buf, "%s destroying %s", ch->name, arg );
    perror( buf );
  }
  return;
}

extern ROOM_INDEX_DATA *       room_index_hash         [MAX_KEY_HASH]; /* db.c */

/* Super-AT command:
FOR ALL <action>
FOR MORTALS <action>
FOR GODS <action>
FOR MOBS <action>
FOR EVERYWHERE <action>

Executes action several times, either on ALL players (not including yourself),
MORTALS (including trusted characters), GODS (characters with level higher than
L_HERO), MOBS (Not recommended) or every room (not recommended either!)

If you insert a # in the action, it will be replaced by the name of the target.

If # is a part of the action, the action will be executed for every target
in game. If there is no #, the action will be executed for every room containg
at least one target, but only once per room. # cannot be used with FOR EVERY-
WHERE. # can be anywhere in the action.

Example: 

FOR ALL SMILE -> you will only smile once in a room with 2 players.
FOR ALL TWIDDLE # -> In a room with A and B, you will twiddle A then B.

Destroying the characters this command acts upon MAY cause it to fail. Try to
avoid something like FOR MOBS PURGE (although it actually works at my MUD).

FOR MOBS TRANS 3054 (transfer ALL the mobs to Midgaard temple) does NOT work
though :)

The command works by transporting the character to each of the rooms with 
target in them. Private rooms are not violated.

*/

/* Expand the name of a character into a string that identifies THAT
   character within a room. E.g. the second 'guard' -> 2. guard
*/   
const char * name_expand (CHAR_DATA *ch)
{
	int count = 1;
	CHAR_DATA *rch;
	char name[MAX_INPUT_LENGTH]; /*  HOPEFULLY no mob has a name longer than THAT */

	static char outbuf[MAX_INPUT_LENGTH];	
	
	if (!IS_NPC(ch))
		return ch->name;
		
	one_argument (ch->name, name); /* copy the first word into name */
	
	if (!name[0]) /* weird mob .. no keywords */
	{
		strcpy (outbuf, ""); /* Do not return NULL, just an empty buffer */
		return outbuf;
	}
	
	/* ->people changed to ->first_person -- TRI */	
	for (rch = ch->in_room->first_person; rch && (rch != ch);rch = 
	    rch->next_in_room)
		if (is_name (name, rch->name))
			count++;
			

	sprintf (outbuf, "%d.%s", count, name);
	return outbuf;
}

void do_for (CHAR_DATA *ch, char *argument)
{
	char range[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	bool fGods = FALSE, fMortals = FALSE, fMobs = FALSE, fEverywhere = FALSE, found;
	ROOM_INDEX_DATA *room, *old_room;
	CHAR_DATA *p, *p_prev;  /* p_next to p_prev -- TRI */
	int i;

	set_char_color( AT_IMMORT, ch );

	argument = one_argument (argument, range);
	if (!range[0] || !argument[0]) /* invalid usage? */
	{
		do_help (ch, "for");
		return;
	}
	
	if (!str_prefix("quit", argument))
	{
		send_to_char ("Are you trying to crash the MUD or something?\n\r",ch);
		return;
	}
	
	
	if (!str_cmp (range, "all"))
	{
		fMortals = TRUE;
		fGods = TRUE;
	}
	else if (!str_cmp (range, "gods"))
		fGods = TRUE;
	else if (!str_cmp (range, "mortals"))
		fMortals = TRUE;
	else if (!str_cmp (range, "mobs"))
		fMobs = TRUE;
	else if (!str_cmp (range, "everywhere"))
		fEverywhere = TRUE;
	else
		do_help (ch, "for"); /* show syntax */

	/* do not allow # to make it easier */		
	if (fEverywhere && strchr (argument, '#'))
	{
		send_to_char ("Cannot use FOR EVERYWHERE with the # thingie.\n\r",ch);
		return;
	}

        set_char_color( AT_PLAIN, ch );		
	if (strchr (argument, '#')) /* replace # ? */
	{ 
		/* char_list - last_char, p_next - gch_prev -- TRI */
		for (p = last_char; p ; p = p_prev )
		{
			p_prev = p->prev;  /* TRI */
		/*	p_next = p->next; */ /* In case someone DOES try to AT MOBS SLAY # */
			found = FALSE;
			
			if (!(p->in_room) || room_is_private(ch,p->in_room) || (p == ch))
				continue;
			
			if (IS_NPC(p) && fMobs)
				found = TRUE;
			else if (!IS_NPC(p) && p->top_level >= LEVEL_IMMORTAL && fGods)
				found = TRUE;
			else if (!IS_NPC(p) && p->top_level < LEVEL_IMMORTAL && fMortals)
				found = TRUE;

			/* It looks ugly to me.. but it works :) */				
			if (found) /* p is 'appropriate' */
			{
				char *pSource = argument; /* head of buffer to be parsed */
				char *pDest = buf; /* parse into this */
				
				while (*pSource)
				{
					if (*pSource == '#') /* Replace # with name of target */
					{
						const char *namebuf = name_expand (p);
						
						if (namebuf) /* in case there is no mob name ?? */
							while (*namebuf) /* copy name over */
								*(pDest++) = *(namebuf++);

						pSource++;
					}
					else
						*(pDest++) = *(pSource++);
				} /* while */
				*pDest = '\0'; /* Terminate */
				
				/* Execute */
				old_room = ch->in_room;
				char_from_room (ch);
				char_to_room (ch,p->in_room);
				interpret (ch, buf);
				char_from_room (ch);
				char_to_room (ch,old_room);
				
			} /* if found */
		} /* for every char */
	}
	else /* just for every room with the appropriate people in it */
	{
		for (i = 0; i < MAX_KEY_HASH; i++) /* run through all the buckets */
			for (room = room_index_hash[i] ; room ; room = room->next)
			{
				found = FALSE;
				
				/* Anyone in here at all? */
				if (fEverywhere) /* Everywhere executes always */
					found = TRUE;
				else if (!room->first_person) /* Skip it if room is empty */
					continue;
				/* ->people changed to first_person -- TRI */
					
				/* Check if there is anyone here of the requried type */
				/* Stop as soon as a match is found or there are no more ppl in room */
				/* ->people to ->first_person -- TRI */
				for (p = room->first_person; p && !found; p = p->next_in_room)
				{

					if (p == ch) /* do not execute on oneself */
						continue;
						
					if (IS_NPC(p) && fMobs)
						found = TRUE;
					else if (!IS_NPC(p) && (p->top_level >= LEVEL_IMMORTAL) && fGods)
						found = TRUE;
					else if (!IS_NPC(p) && (p->top_level <= LEVEL_IMMORTAL) && fMortals)
						found = TRUE;
				} /* for everyone inside the room */
						
				if (found && !room_is_private(ch,room)) /* Any of the required type here AND room not private? */
				{
					/* This may be ineffective. Consider moving character out of old_room
					   once at beginning of command then moving back at the end.
					   This however, is more safe?
					*/
				
					old_room = ch->in_room;
					char_from_room (ch);
					char_to_room (ch, room);
					interpret (ch, argument);
					char_from_room (ch);
					char_to_room (ch, old_room);
				} /* if found */
			} /* for every room in a bucket */
	} /* if strchr */
} /* do_for */

void save_sysdata  args( ( SYSTEM_DATA sys ) );

void do_cset( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_STRING_LENGTH];
  sh_int level;

  set_char_color( AT_IMMORT, ch );

  if (argument[0] == '\0')
  {
    ch_printf(ch, "&BMail:&W\n\r  Read all mail: &R%d&W. Read mail for free: &R%d&W. Write mail for free: &R%d&W.\n\r",
	    sysdata.read_all_mail, sysdata.read_mail_free, sysdata.write_mail_free );
    ch_printf(ch, "  Take all mail: &R%d&W.\n\r",
	    sysdata.take_others_mail);
	ch_printf(ch, "&BChannels:&W\n\r  Log: &R%d&W. Build: &R%d&W.\n\r",
 	    sysdata.log_level, sysdata.build_level);
    ch_printf(ch, "&BBuilding:&W\n\r  Prototype modification: &R%d&W.  Player msetting: &R%d&W.\n\r",
	    sysdata.level_modify_proto, sysdata.level_mset_player );
    ch_printf(ch, "&BOther:&W\n\r  Force on players: &R%d&W.  ", sysdata.level_forcepc);
    ch_printf(ch, "Private room override: &R%d&W.\n\r", sysdata.level_override_private);
    ch_printf(ch, "  Penalty to regular stun chance: &R%d&W.  ", sysdata.stun_regular );
    ch_printf(ch, "Penalty to stun plr vs. plr: &R%d&W.\n\r", sysdata.stun_plr_vs_plr );
    ch_printf(ch, "  Percent damage plr vs. plr: &R%3d&W.  ", sysdata.dam_plr_vs_plr );
    ch_printf(ch, "Percent damage plr vs. mob: &R%d&W.\n\r", sysdata.dam_plr_vs_mob );
    ch_printf(ch, "  Percent damage mob vs. plr: &R%3d&W.  ", sysdata.dam_mob_vs_plr );
    ch_printf(ch, "Percent damage mob vs. mob: &R%d&W.\n\r", sysdata.dam_mob_vs_mob );
    ch_printf(ch, "  Get object without take flag: &R%d&W.  ", sysdata.level_getobjnotake);
    ch_printf(ch, "Autosave frequency (minutes): &R%d&W.\n\r", sysdata.save_frequency );
    ch_printf(ch, "  Save flags: &R%s&W\n\r", flag_string( sysdata.save_flags, save_flag ) );
	ch_printf(ch, "&BPets\n\r&WSaving Pets is:	&R%s\n\r",  (sysdata.save_pets) ? "ON" : "off" );
	ch_printf(ch, "&BAuth\n\r&WName authorization: &R%s\n\r", sysdata.WAIT_FOR_AUTH ? "On" : "Off" );
	pager_printf_color(ch, "  &wIdents retries: &W%d\n\r", sysdata.ident_retries);

    return;
  }

  argument = one_argument( argument, arg );

  if (!str_cmp(arg, "help"))
  {
      do_help(ch, "controls"); 
     return;
  }

   if ( !str_cmp( arg, "auth" ) )
    {

	sysdata.WAIT_FOR_AUTH = !sysdata.WAIT_FOR_AUTH;

      if ( sysdata.WAIT_FOR_AUTH )
    	  send_to_char( "Name authorization system enabled.\n\r", ch );
      else
	  send_to_char( "Name authorization system disabled.\n\r", ch );
      return;
    }

     if (!str_cmp(arg, "mudname"))
  {
	if (sysdata.mud_name)
		DISPOSE(sysdata.mud_name);
	sysdata.mud_name = str_dup( argument );	
	send_to_char("Name set.\n\r", ch );
	return;
  }


  if (!str_cmp(arg, "save"))
  {
     save_sysdata(sysdata);
	 send_to_char( "Cset functions saved.\n\r", ch );
     return;
  }

  if (!str_cmp(arg, "saveflag"))
  {
	int x = get_saveflag( argument );

	if ( x == -1 )
	    send_to_char( "Not a save flag.\n\r", ch );
	else
	{
	    TOGGLE_BIT( sysdata.save_flags, 1 << x );
	    send_to_char( "Ok.\n\r", ch );
	}
	return;
  }

  level = (sh_int) atoi(argument);

  if (!str_prefix( arg, "savefrequency" ) )
  {
    sysdata.save_frequency = level;
    send_to_char("Ok.\n\r", ch);
    return;
  }
   

  if (!str_cmp(arg, "stun"))
  {
    sysdata.stun_regular = level;
    send_to_char("Ok.\n\r", ch);      
    return;
  }

  if (!str_cmp(arg, "stun_pvp"))
  {
    sysdata.stun_plr_vs_plr = level;
    send_to_char("Ok.\n\r", ch);      
    return;
  }

  if (!str_cmp(arg, "dam_pvp"))
  {
    sysdata.dam_plr_vs_plr = level;
    send_to_char("Ok.\n\r", ch);      
    return;
  }

  if (!str_cmp(arg, "get_notake"))
  {
    sysdata.level_getobjnotake = level;
    send_to_char("Ok.\n\r", ch);      
    return;
  }

  if (!str_cmp(arg, "dam_pvm"))
  {
    sysdata.dam_plr_vs_mob = level;
    send_to_char("Ok.\n\r", ch);      
    return;
  }

  if (!str_cmp(arg, "dam_mvp"))
  {
    sysdata.dam_mob_vs_plr = level;
    send_to_char("Ok.\n\r", ch);      
    return;
  }

  if (!str_cmp(arg, "dam_mvm"))
  {
    sysdata.dam_mob_vs_mob = level;
    send_to_char("Ok.\n\r", ch);      
    return;
  }

  if (!str_cmp(arg, "ident_retries") || !str_cmp(arg, "ident"))
  {
    sysdata.ident_retries = level;
    if (level > 20)
      send_to_char("Caution:  This setting may cause the game to lag.\n\r", ch );
    else if (level <= 0)
      send_to_char("Ident lookups turned off.\n\r", ch );
    else
      send_to_char("Ok.\n\r", ch);
    return;
  }

  if (level < 0 || level > MAX_LEVEL)
  {
    send_to_char("Invalid value for new control.\n\r", ch);
    return;
  }

  if (!str_cmp(arg, "read_all"))
  {
    sysdata.read_all_mail = level;
    send_to_char("Ok.\n\r", ch);      
    return;
  }

  if (!str_cmp(arg, "read_free"))
  {
    sysdata.read_mail_free = level;
    send_to_char("Ok.\n\r", ch);      
    return;
  }
  if (!str_cmp(arg, "write_free"))
  {
    sysdata.write_mail_free = level;
    send_to_char("Ok.\n\r", ch);      
    return;
  }
  if (!str_cmp(arg, "take_all"))
  {
    sysdata.take_others_mail = level;
    send_to_char("Ok.\n\r", ch);      
    return;
  }
  if (!str_cmp(arg, "log"))
  {
    sysdata.log_level = level;
    send_to_char("Ok.\n\r", ch);      
    return;
  }
  if (!str_cmp(arg, "build"))
  {
    sysdata.build_level = level;
    send_to_char("Ok.\n\r", ch);      
    return;
  }
  if (!str_cmp(arg, "proto_modify"))
  {
    sysdata.level_modify_proto = level;
    send_to_char("Ok.\n\r", ch);      
    return;
  }
  if (!str_cmp(arg, "override_private"))
  {
    sysdata.level_override_private = level;
    send_to_char("Ok.\n\r", ch);      
    return;
  }
  if (!str_cmp(arg, "forcepc"))
  {
    sysdata.level_forcepc = level;
    send_to_char("Ok.\n\r", ch);      
    return;
  }
  if (!str_cmp(arg, "mset_player"))
  {
    sysdata.level_mset_player = level;
    send_to_char("Ok.\n\r", ch);      
    return;
  }
  else if ( !str_cmp ( arg, "petsave" ) )
  {
      if ( level )
	sysdata.save_pets = TRUE;
      else
	sysdata.save_pets = FALSE;
  }
  else
  {
    send_to_char("Invalid argument.\n\r", ch);
    return;
  }
}

void get_reboot_string(void)
{
  sprintf(reboot_time, "%s", asctime(new_boot_time));
}

void do_hell( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  sh_int time;
  bool h_d = FALSE;
  struct tm *tms;

  set_char_color( AT_IMMORT, ch );

  argument = one_argument(argument, arg);
  if ( !*arg )
  {
    send_to_char( "Hell who, and for how long?\n\r", ch );
    return;
  }
  if ( !(victim = get_char_world(ch, arg)) || IS_NPC(victim) )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if ( IS_IMMORTAL(victim) )
  {
    send_to_char( "There is no point in helling an immortal.\n\r", ch );
    return;
  }
  if ( victim->pcdata->release_date != 0 )
  {
    ch_printf(ch, "They are already in hell until %24.24s, by %s.\n\r",
            ctime(&victim->pcdata->release_date), victim->pcdata->helled_by);
    return;
  }

  argument = one_argument(argument, arg);
  if ( !*arg || !is_number(arg) )
  {
    send_to_char( "Hell them for how long?\n\r", ch );
    return;
  }

  time = atoi(arg);
  if ( time <= 0 )
  {
    send_to_char( "You cannot hell for zero or negative time.\n\r", ch );
    return;
  }

  argument = one_argument(argument, arg);
  if ( !*arg || !str_cmp(arg, "hours") )
    h_d = TRUE;
  else if ( str_cmp(arg, "days") )
  {
    send_to_char( "Is that value in hours or days?\n\r", ch );
    return;
  }
  else if ( time > 30 )
  {
    send_to_char( "You may not hell a person for more than 30 days at a time.\n\r", ch );
    return;
  }
  tms = localtime(&current_time);

  if ( h_d )
    tms->tm_hour += time;
  else
    tms->tm_mday += time;
  victim->pcdata->release_date = mktime(tms);
  victim->pcdata->helled_by = STRALLOC(ch->name);
  ch_printf(ch, "%s will be released from hell at %24.24s.\n\r", victim->name,
          ctime(&victim->pcdata->release_date));
  act(AT_MAGIC, "$n disappears in a cloud of hellish light.", victim, NULL, ch, TO_NOTVICT);
  char_from_room(victim);
  char_to_room(victim, get_room_index(6));
  act(AT_MAGIC, "$n appears in a could of hellish light.", victim, NULL, ch, TO_NOTVICT);
  do_look(victim, "auto");
  ch_printf(victim, "The immortals are not pleased with your actions.\n\r"
          "You shall remain in hell for %d %s%s.\n\r", time,
          (h_d ? "hour" : "day"), (time == 1 ? "" : "s"));
  save_char_obj(victim);	/* used to save ch, fixed by Thoric 09/17/96 */
  return;
}

void do_unhell( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA *location;

  set_char_color( AT_IMMORT, ch );
  
  argument = one_argument(argument, arg);
  if ( !*arg )
  {
    send_to_char( "Unhell whom..?\n\r", ch );
    return;
  }
  location = ch->in_room;
/*ch->in_room = get_room_index(8);*/
  victim = get_char_world(ch, arg);
/*ch->in_room = location;          The case of unhell self, etc.*/
  if ( !victim || IS_NPC(victim) )
  {
    send_to_char( "No such player character present.\n\r", ch );
    return;
  }
  if ( victim->in_room->vnum != 8
  &&   victim->in_room->vnum != 1206
  &&   victim->in_room->vnum != 6 )
  {
    send_to_char( "No one like that is in hell.\n\r", ch );
    return;
  }

  location = get_room_index(ROOM_VNUM_TEMPLE);
  if ( !location )
    location = ch->in_room;
  MOBtrigger = FALSE;
  act( AT_MAGIC, "$n disappears in a cloud of godly light.", victim, NULL, ch, TO_NOTVICT );
  char_from_room(victim);
  char_to_room(victim, location);
  send_to_char( "The gods have smiled on you and released you from hell early!\n\r", victim );
  do_look(victim, "auto");
  if ( victim != ch )
    send_to_char( "They have been released.\n\r", ch );
  if ( victim->pcdata->helled_by )
  {
    if( str_cmp(ch->name, victim->pcdata->helled_by) )
      ch_printf(ch, "(You should probably write a note to %s, explaining the early release.)\n\r",
            victim->pcdata->helled_by);
    STRFREE(victim->pcdata->helled_by);
    victim->pcdata->helled_by = NULL;
  }

  MOBtrigger = FALSE;
  act( AT_MAGIC, "$n appears in a cloud of godly light.", victim, NULL, ch, TO_NOTVICT );
  victim->pcdata->release_date = 0;
  save_char_obj(victim);
  return;
}

/* Vnum search command by Swordbearer */
void do_vsearch( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    bool found = FALSE;
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    int obj_counter = 1;
    int argi;

    set_pager_color( AT_PLAIN, ch );
 
    one_argument( argument, arg );
    if( arg[0] == '\0' )
    {
        send_to_char( "Syntax:  vsearch <vnum>.\n\r", ch );
        return;
    }

    argi=atoi(arg);
    if (argi<0 && argi>20000)
    {
	send_to_char( "Vnum out of range.\n\r", ch);
	return;
    }
    for ( obj = first_object; obj != NULL; obj = obj->next )
    {
	if ( !can_see_obj( ch, obj ) || !( argi == obj->pIndexData->vnum ))
	  continue;
 
	found = TRUE;
	for ( in_obj = obj; in_obj->in_obj != NULL;
	  in_obj = in_obj->in_obj );

	if ( in_obj->carried_by != NULL )
	  pager_printf( ch, "[%2d] Level %d %s carried by %s.\n\r", 
		obj_counter,
		obj->level, obj_short(obj),
		PERS( in_obj->carried_by, ch ) );
	else           
	  pager_printf( ch, "[%2d] [%-5d] %s in %s.\n\r", obj_counter,
		( ( in_obj->in_room ) ? in_obj->in_room->vnum : 0 ),
		obj_short(obj), ( in_obj->in_room == NULL ) ?
		"somewhere" : in_obj->in_room->name );

	obj_counter++;
    }

    if ( !found )
      send_to_char( "Nothing like that in hell, earth, or heaven.\n\r" , ch );
    return;
}

/* 
 * Simple function to let any imm make any player instantly sober.
 * Saw no need for level restrictions on this.
 * Written by Narn, Apr/96 
 */
void do_sober( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1 [MAX_INPUT_LENGTH];

  set_char_color( AT_IMMORT, ch );

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );
  if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if ( IS_NPC( victim ) )
  {
    send_to_char( "Not on mobs.\n\r", ch );
    return;    
  }

  if ( victim->pcdata ) 
    victim->pcdata->condition[COND_DRUNK] = 0;
  send_to_char( "Ok.\n\r", ch );
  set_char_color( AT_IMMORT, victim );
  send_to_char( "You feel sober again.\n\r", victim );
  return;    
}

/*
 * Free a social structure					-Thoric
 */
void free_social( SOCIALTYPE *social )
{
    if ( social->name )
      DISPOSE( social->name );
    if ( social->char_no_arg )
      DISPOSE( social->char_no_arg );
    if ( social->others_no_arg )
      DISPOSE( social->others_no_arg );
    if ( social->char_found )
      DISPOSE( social->char_found );
    if ( social->others_found )
      DISPOSE( social->others_found );
    if ( social->vict_found )
      DISPOSE( social->vict_found );
    if ( social->char_auto )
      DISPOSE( social->char_auto );
    if ( social->others_auto )
      DISPOSE( social->others_auto );
    DISPOSE( social );
}

/*
 * Remove a social from it's hash index				-Thoric
 */
void unlink_social( SOCIALTYPE *social )
{
    SOCIALTYPE *tmp, *tmp_next;
    int hash;

    if ( !social )
    {
	bug( "Unlink_social: NULL social", 0 );
	return;
    }

    if ( social->name[0] < 'a' || social->name[0] > 'z' )
	hash = 0;
    else
	hash = (social->name[0] - 'a') + 1;

    if ( social == (tmp=social_index[hash]) )
    {
	social_index[hash] = tmp->next;
	return;
    }
    for ( ; tmp; tmp = tmp_next )
    {
	tmp_next = tmp->next;
	if ( social == tmp_next )
	{
	    tmp->next = tmp_next->next;
	    return;
	}
    }
}

/*
 * Add a social to the social index table			-Thoric
 * Hashed and insert sorted
 */
void add_social( SOCIALTYPE *social )
{
    int hash, x;
    SOCIALTYPE *tmp, *prev;

    if ( !social )
    {
	bug( "Add_social: NULL social", 0 );
	return;
    }

    if ( !social->name )
    {
	bug( "Add_social: NULL social->name", 0 );
	return;
    }

    if ( !social->char_no_arg )
    {
	bug( "Add_social: NULL social->char_no_arg", 0 );
	return;
    }

    /* make sure the name is all lowercase */
    for ( x = 0; social->name[x] != '\0'; x++ )
	social->name[x] = LOWER(social->name[x]);

    if ( social->name[0] < 'a' || social->name[0] > 'z' )
	hash = 0;
    else
	hash = (social->name[0] - 'a') + 1;

    if ( (prev = tmp = social_index[hash]) == NULL )
    {
	social->next = social_index[hash];
	social_index[hash] = social;
	return;
    }

    for ( ; tmp; tmp = tmp->next )
    {
	if ( (x=strcmp(social->name, tmp->name)) == 0 )
	{
	    bug( "Add_social: trying to add duplicate name to bucket %d", hash );
	    free_social( social );
	    return;
	}
	else
	if ( x < 0 )
	{
	    if ( tmp == social_index[hash] )
	    {
		social->next = social_index[hash];
		social_index[hash] = social;
		return;
	    }
	    prev->next = social;
	    social->next = tmp;
	    return;
	}
	prev = tmp;
    }

    /* add to end */
    prev->next = social;
    social->next = NULL;
    return;
}

/*
 * Social editor/displayer/save/delete				-Thoric
 */
void do_sedit( CHAR_DATA *ch, char *argument )
{
    SOCIALTYPE *social;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    set_char_color( AT_SOCIAL, ch );

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax: sedit <social> [field]\n\r", ch );
	send_to_char( "Syntax: sedit <social> create\n\r", ch );
	if ( get_trust(ch) > LEVEL_GOD )
	    send_to_char( "Syntax: sedit <social> delete\n\r", ch );
	    send_to_char( "Syntax: sedit <save>\n\r", ch );
	send_to_char( "\n\rField being one of:\n\r", ch );
	send_to_char( "  cnoarg onoarg cfound ofound vfound cauto oauto\n\r", ch );
	return;
    }

    if ( !str_cmp( arg1, "save" ) )
    {
	save_socials();
	send_to_char( "Saved.\n\r", ch );
	return;
    }

    social = find_social( arg1 );
    if ( !str_cmp( arg2, "create" ) )
    {
	if ( social )
	{
	    send_to_char( "That social already exists!\n\r", ch );
	    return;
	}
	CREATE( social, SOCIALTYPE, 1 );
	social->name = str_dup( arg1 );
	sprintf( arg2, "You %s.", arg1 );
	social->char_no_arg = str_dup( arg2 );
	add_social( social );
	send_to_char( "Social added.\n\r", ch );
	return;
    }

    if ( !social )
    {
	send_to_char( "Social not found.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' || !str_cmp( arg2, "show" ) )
    {
	ch_printf( ch, "Social: %s\n\r\n\rCNoArg: %s\n\r",
	    social->name,	social->char_no_arg );
	ch_printf( ch, "ONoArg: %s\n\rCFound: %s\n\rOFound: %s\n\r",
	    social->others_no_arg	? social->others_no_arg	: "(not set)",
	    social->char_found		? social->char_found	: "(not set)",
	    social->others_found	? social->others_found	: "(not set)" );
	ch_printf( ch, "VFound: %s\n\rCAuto : %s\n\rOAuto : %s\n\r",
	    social->vict_found	? social->vict_found	: "(not set)",
	    social->char_auto	? social->char_auto	: "(not set)",
	    social->others_auto	? social->others_auto	: "(not set)" );
	return;
    }
    if ( get_trust(ch) > LEVEL_GOD && !str_cmp( arg2, "delete" ) )
    {
	unlink_social( social );
	free_social( social );
	send_to_char( "Deleted.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "cnoarg" ) )
    {
	if ( argument[0] == '\0' || !str_cmp( argument, "clear" ) )
	{
	    send_to_char( "You cannot clear this field.  It must have a message.\n\r", ch );
	    return;
	}
	if ( social->char_no_arg )
	    DISPOSE( social->char_no_arg );
	social->char_no_arg = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "onoarg" ) )
    {
	if ( social->others_no_arg )
	    DISPOSE( social->others_no_arg );
	if ( argument[0] != '\0' && str_cmp( argument, "clear" ) )
	    social->others_no_arg = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "cfound" ) )
    {
	if ( social->char_found )
	    DISPOSE( social->char_found );
	if ( argument[0] != '\0' && str_cmp( argument, "clear" ) )
	    social->char_found = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "ofound" ) )
    {
	if ( social->others_found )
	    DISPOSE( social->others_found );
	if ( argument[0] != '\0' && str_cmp( argument, "clear" ) )
	    social->others_found = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "vfound" ) )
    {
	if ( social->vict_found )
	    DISPOSE( social->vict_found );
	if ( argument[0] != '\0' && str_cmp( argument, "clear" ) )
	    social->vict_found = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "cauto" ) )
    {
	if ( social->char_auto )
	    DISPOSE( social->char_auto );
	if ( argument[0] != '\0' && str_cmp( argument, "clear" ) )
	    social->char_auto = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "oauto" ) )
    {
	if ( social->others_auto )
	    DISPOSE( social->others_auto );
	if ( argument[0] != '\0' && str_cmp( argument, "clear" ) )
	    social->others_auto = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	return;
    }
    if ( get_trust(ch) > LEVEL_GREATER && !str_cmp( arg2, "name" ) )
    {
	bool relocate;

	one_argument( argument, arg1 );
	if ( arg1[0] == '\0' )
	{
	    send_to_char( "Cannot clear name field!\n\r", ch );
	    return;
	}
	if ( arg1[0] != social->name[0] )
	{
	    unlink_social( social );
	    relocate = TRUE;
	}
	else
	    relocate = FALSE;
	if ( social->name )
	    DISPOSE( social->name );
	social->name = str_dup( arg1 );
	if ( relocate )
	    add_social( social );
	send_to_char( "Done.\n\r", ch );
	return;
    }

    /* display usage message */
    do_sedit( ch, "" );
}

/*
 * Free a command structure					-Thoric
 */
void free_command( CMDTYPE *command )
{
    if ( command->name )
      DISPOSE( command->name );
    DISPOSE( command );
}

/*
 * Remove a command from it's hash index			-Thoric
 */
void unlink_command( CMDTYPE *command )
{
    CMDTYPE *tmp, *tmp_next;
    int hash;

    if ( !command )
    {
	bug( "Unlink_command NULL command", 0 );
	return;
    }

    hash = command->name[0]%126;

    if ( command == (tmp=command_hash[hash]) )
    {
	command_hash[hash] = tmp->next;
	return;
    }
    for ( ; tmp; tmp = tmp_next )
    {
	tmp_next = tmp->next;
	if ( command == tmp_next )
	{
	    tmp->next = tmp_next->next;
	    return;
	}
    }
}

/*
 * Add a command to the command hash table			-Thoric
 */
void add_command( CMDTYPE *command )
{
    int hash, x;
    CMDTYPE *tmp, *prev;

    if ( !command )
    {
	bug( "Add_command: NULL command", 0 );
	return;
    }

    if ( !command->name )
    {
	bug( "Add_command: NULL command->name", 0 );
	return;
    }

    if ( !command->do_fun )
    {
	bug( "Add_command: NULL command->do_fun", 0 );
	return;
    }

    /* make sure the name is all lowercase */
    for ( x = 0; command->name[x] != '\0'; x++ )
	command->name[x] = LOWER(command->name[x]);

    hash = command->name[0] % 126;

    if ( (prev = tmp = command_hash[hash]) == NULL )
    {
	command->next = command_hash[hash];
	command_hash[hash] = command;
	return;
    }

    /* add to the END of the list */
    for ( ; tmp; tmp = tmp->next )
	if ( !tmp->next )
	{
	    tmp->next = command;
	    command->next = NULL;
	}
    return;
}

/*
 * Command editor/displayer/save/delete				-Thoric
 * Added support for interpret flags                            -Shaddai
 */
void do_cedit( CHAR_DATA *ch, char *argument )
{
    CMDTYPE *command;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    set_char_color( AT_IMMORT, ch );

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax: cedit save cmdtable\n\r", ch );
	if ( get_trust(ch) > LEVEL_SUB_IMPLEM  )
	{
	    send_to_char( "Syntax: cedit <command> create [code]\n\r", ch );
	    send_to_char( "Syntax: cedit <command> delete\n\r", ch );
	    send_to_char( "Syntax: cedit <command> show\n\r", ch );
	    send_to_char( "Syntax: cedit <command> raise\n\r", ch );
	    send_to_char( "Syntax: cedit <command> lower\n\r", ch );
	    send_to_char( "Syntax: cedit <command> list\n\r", ch );
	    send_to_char( "Syntax: cedit <command> [field]\n\r", ch );
	    send_to_char( "\n\rField being one of:\n\r", ch );
	    send_to_char( "  level position log code flags\n\r", ch );
	}
	return;
    }

    if ( !str_cmp( arg1, "save" ) && !str_cmp( arg2, "cmdtable") )
    {
	save_commands();
	send_to_char( "Saved.\n\r", ch );
	return;
    }

    command = find_command( arg1 );
    if ( get_trust(ch) > LEVEL_SUB_IMPLEM &&    !str_cmp( arg2, "create" ) )
    {
	if ( command )
	{
	    send_to_char( "That command already exists!\n\r", ch );
	    return;
	}
	CREATE( command, CMDTYPE, 1 );
	command->lag_count = 0; /* FB */
	command->name = str_dup( arg1 );
	command->level = get_trust(ch);
	if ( *argument )
	    one_argument(argument, arg2);
	else
	    sprintf( arg2, "do_%s", arg1 );
	command->do_fun = skill_function( arg2 );
	add_command( command );
	send_to_char( "Command added.\n\r", ch );
	if ( command->do_fun == skill_notfound )
	    ch_printf( ch, "Code %s not found.  Set to no code.\n\r", arg2 );
	return;
    }

    if ( !command )
    {
	send_to_char( "Command not found.\n\r", ch );
	return;
    }
    else
    if ( command->level > get_trust(ch) )
    {
	send_to_char( "You cannot touch this command.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' || !str_cmp( arg2, "show" ) )
    {
	ch_printf( ch, "Command:  %s\n\rLevel:    %d\n\rPosition: %d\n\rLog:      %d\n\rCode:     %s\n\rFlags:  %s\n\r",
	    command->name, command->level, command->position, command->log,
	    skill_name(command->do_fun),flag_string(command->flags, cmd_flags));
	if ( command->userec.num_uses )
	  send_timer(&command->userec, ch);
	return;
    }

    if ( get_trust(ch) <= LEVEL_SUB_IMPLEM )
    {
	do_cedit( ch, "" );
	return;
    }

    if ( !str_cmp( arg2, "raise" ) )
    {
	CMDTYPE *tmp, *tmp_next;
	int hash = command->name[0] % 126;

	if ( (tmp=command_hash[hash]) == command )
	{
	    send_to_char( "That command is already at the top.\n\r", ch );
	    return;
	}
	if ( tmp->next == command )
	{
	    command_hash[hash] = command;
	    tmp_next = tmp->next;
	    tmp->next = command->next;
	    command->next = tmp;
	    ch_printf( ch, "Moved %s above %s.\n\r", command->name, command->next->name );
	    return;
	}
	for ( ; tmp; tmp = tmp->next )
	{
	    tmp_next = tmp->next;
	    if ( tmp_next->next == command )
	    {
		tmp->next = command;
		tmp_next->next = command->next;
		command->next = tmp_next;
		ch_printf( ch, "Moved %s above %s.\n\r", command->name, command->next->name );
		return;
	    }
	}
	send_to_char( "ERROR -- Not Found!\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "lower" ) )
    {
	CMDTYPE *tmp, *tmp_next;
	int hash = command->name[0] % 126;

	if ( command->next == NULL )
	{
	    send_to_char( "That command is already at the bottom.\n\r", ch );
	    return;
	}
	tmp = command_hash[hash];
	if ( tmp == command )
	{
	    tmp_next = tmp->next;
	    command_hash[hash] = command->next;
	    command->next = tmp_next->next;
	    tmp_next->next = command;

	    ch_printf( ch, "Moved %s below %s.\n\r", command->name, tmp_next->name );
	    return;
	}
	for ( ; tmp; tmp = tmp->next )
	{
	    if ( tmp->next == command )
	    {
		tmp_next = command->next;
		tmp->next = tmp_next;
		command->next = tmp_next->next;
		tmp_next->next = command;
		
		ch_printf( ch, "Moved %s below %s.\n\r", command->name, tmp_next->name );
		return;
	    }
	}
	send_to_char( "ERROR -- Not Found!\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "list" ) )
    {
	CMDTYPE *tmp;
	int hash = command->name[0] % 126;

	pager_printf( ch, "Priority placement for [%s]:\n\r", command->name );
	for ( tmp = command_hash[hash]; tmp; tmp = tmp->next )
	{
	    if ( tmp == command )
		set_pager_color( AT_GREEN, ch );
	    else
		set_pager_color( AT_PLAIN, ch );
	    pager_printf( ch, "  %s\n\r", tmp->name );
	}
	return;
    }
    if ( !str_cmp( arg2, "delete" ) )
    {
	unlink_command( command );
	free_command( command );
	send_to_char( "Deleted.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "code" ) )
    {
	DO_FUN *fun = skill_function( argument );
	
	if ( fun == skill_notfound )
	{
	    send_to_char( "Code not found.\n\r", ch );
	    return;
	}
	command->do_fun = fun;
	send_to_char( "Done.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "level" ) )
    {
	int level = atoi( argument );

	if ( (level < 0 || level > get_trust(ch)) )
	{
	    send_to_char( "Level out of range.\n\r", ch );
	    return;
	}
	command->level = level;
	send_to_char( "Done.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "log" ) )
    {
	int log = atoi( argument );

	if ( log < 0 || log > LOG_COMM )
	{
	    send_to_char( "Log out of range.\n\r", ch );
	    return;
	}
	command->log = log;
	send_to_char( "Done.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "position" ) )
    {
	int position = atoi( argument );

	if ( position < 0 || position > POS_DRAG )
	{
	    send_to_char( "Position out of range.\n\r", ch );
	    return;
	}
	command->position = position;
	send_to_char( "Done.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "flags" ) )
    {
        int flag;
        if ( is_number(argument))
                flag = atoi ( argument );
        else
                flag = get_cmdflag ( argument );
        if ( flag < 0 || flag >= 32 )
        {
         if ( is_number( argument ) )
          ch_printf (ch, "Invalid flag: range is from 0 to 31.\n");
         else
          ch_printf (ch, "Unknown flag %s.\n", argument );
         return;
        }

        TOGGLE_BIT( command->flags, 1 << flag );
        send_to_char( "Done.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "name" ) )
    {
	bool relocate;

	one_argument( argument, arg1 );
	if ( arg1[0] == '\0' )
	{
	    send_to_char( "Cannot clear name field!\n\r", ch );
	    return;
	}
	if ( arg1[0] != command->name[0] )
	{
	    unlink_command( command );
	    relocate = TRUE;
	}
	else
	    relocate = FALSE;
	if ( command->name )
	    DISPOSE( command->name );
	command->name = str_dup( arg1 );
	if ( relocate )
	    add_command( command );
	send_to_char( "Done.\n\r", ch );
	return;
    }

    /* display usage message */
    do_cedit( ch, "" );
}

void do_qpset( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *victim;
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	int arg3;
	char buf[MAX_STRING_LENGTH];
	
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	arg3 = atoi( arg2 );
    
	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL 
		|| ( IS_NPC(victim) && victim->in_room != ch->in_room ) 
		|| (!NOT_AUTHED(ch) && NOT_AUTHED(victim) && !IS_IMMORTAL(ch) ) )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}
	
	victim->pcdata->quest_curr += arg3;
	sprintf(buf, "Ok, %s now has %d quest points.",victim->name,victim->pcdata->quest_curr);
	send_to_char(buf,ch);
	return;
}

/* Easy way to check a player's glory -- Blodkai, June 97 */
void do_qpstat( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  set_char_color( AT_IMMORT, ch );
 
  if ( IS_NPC(ch) )
    return;

  argument = one_argument( argument, arg );
  if ( arg[0] == '\0' )
  {
    send_to_char( "Syntax:  qpstat <character>\n\r", ch );
    return;
  }
  if ( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "No one by that name currently in the Realms.\n\r", ch );
    return;
  }
  if ( IS_NPC( victim ) )
  {
    send_to_char( "Mobs don't have glory.\n\r", ch );
    return;
  }
  ch_printf( ch, "%s has %d glory\n\r",
        victim->name,
        victim->pcdata->quest_curr );
  return;
}

void do_ideas( CHAR_DATA *ch, char *argument )
{
    set_pager_color( AT_RED, ch );
    show_file( ch, IDEA_FILE );
}

void do_bugs( CHAR_DATA *ch, char *argument )
{
    set_pager_color( AT_BLUE, ch );
    show_file( ch, BUG_FILE2 );
}

void do_typos( CHAR_DATA *ch, char *argument )
{
    set_pager_color( AT_GREEN, ch );
    show_file( ch, TYPO_FILE );
}

RESERVE_DATA *first_reserved;
RESERVE_DATA *last_reserved;
void save_reserved(void)
{
  RESERVE_DATA *res;
  FILE *fp;
  
  fclose(fpReserve);
  if (!(fp = fopen(SYSTEM_DIR RESERVED_LIST, "w")))
  {
    bug( "Save_reserved: cannot open " RESERVED_LIST, 0 );
    perror(RESERVED_LIST);
    fpReserve = fopen( NULL_FILE, "r" );
    return;
  }
  for (res = first_reserved; res; res = res->next)
    fprintf(fp, "%s~\n", res->name);
  fprintf(fp, "$~\n");
  fclose(fp);
  fpReserve = fopen(NULL_FILE, "r");
  return;
}

void do_reserve(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  RESERVE_DATA *res;

  set_char_color( AT_PLAIN, ch );
  
  argument = one_argument(argument, arg);
  if (!*arg)
  {
    int wid = 0;
    
    send_to_char("-- Reserved Names --\n\r", ch);
    for (res = first_reserved; res; res = res->next)
    {
      ch_printf(ch, "%c%-17s ", (*res->name == '*' ? '*' : ' '),
          (*res->name == '*' ? res->name+1 : res->name));
      if (++wid % 4 == 0)
        send_to_char("\n\r", ch);
    }
    if (wid % 4 != 0)
      send_to_char("\n\r", ch);
    return;
  }
  for (res = first_reserved; res; res = res->next)
    if (!str_cmp(arg, res->name))
    {
      UNLINK(res, first_reserved, last_reserved, next, prev);
      DISPOSE(res->name);
      DISPOSE(res);
      save_reserved();
      send_to_char("Name no longer reserved.\n\r", ch);
      return;
    }
  CREATE(res, RESERVE_DATA, 1);
  res->name = str_dup(arg);
  sort_reserved(res);
  save_reserved();
  send_to_char("Name reserved.\n\r", ch);
  return;
}

void do_saveall( CHAR_DATA *ch, char *argument )
{
	if ( !str_cmp("ships",argument))
	{
		SHIP_DATA *ship;
		for ( ship = first_ship; ship; ship = ship->next )
		{
			if (IS_SET(ship->flags,SHIP_MOB))
				continue;
			else
				save_ship(ship);
		}
	}
	else if ( !str_cmp("area", argument))
	{
		AREA_DATA *pArea;
		for ( pArea = first_asort; pArea; pArea = pArea->next_sort )
		{
			if ( IS_SET( pArea->status, AREA_DELETED ) )
				continue;
			if ( !IS_SET(pArea->status, AREA_LOADED ) )
				continue;
			else
				fold_area( pArea, pArea->filename, FALSE );
		}
	}
	else 
	{
		CHAR_DATA *vch;
		CHAR_DATA *vch_next;

		for ( vch = first_char; vch; vch = vch_next )
		{
			vch_next = vch->next;
			if ( !IS_NPC(vch) )
			{
				interpret( vch, "save");
			}
		}
		return;
	}
	return;
}

/*
 * Create an instance of a new race.			-Shaddai
 */

bool create_new_race( int index, char *argument ) 
{
	int i = 0;
	if ( index >= MAX_RACE || race_table[index] == NULL )
		return FALSE;
	if ( argument[0] != '\0' )
		argument[0]=UPPER(argument[0]);
	sprintf( race_table[index]->race_name,"%-.16s",argument);
	race_table[index]->class_restriction = 0;
	race_table[index]->nospeak = 0;
	race_table[index]->str_plus = 0;
	race_table[index]->dex_plus = 0;
	race_table[index]->wis_plus = 0;
	race_table[index]->int_plus = 0;
	race_table[index]->con_plus = 0;
	race_table[index]->cha_plus = 0;
	race_table[index]->lck_plus = 0;
	for ( i = 0; i < MAX_ABILITY ; i++ )
		race_table[index]->ability[i] = 0;
	race_table[index]->affected = 0;
	race_table[index]->resist = 0;
	race_table[index]->suscept = 0;
	race_table[index]->language = 0;
	race_table[index]->ac_plus = 0;
	race_table[index]->height = 0;
	race_table[index]->weight = 0;
	race_table[index]->hunger_mod = 0;
	race_table[index]->thirst_mod = 0;
	race_table[index]->consent = 16;
	return TRUE;
}

void do_setrace( CHAR_DATA *ch, char *argument )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3 [MAX_INPUT_LENGTH];
	FILE *fpList;
	char racelist[256];
	int value;
    
	char buf[MAX_STRING_LENGTH];
	struct race_type *race;
	int ra, i;
    
	set_char_color( AT_PLAIN, ch );
	
	if ( IS_NPC( ch ) )
	{
		send_to_char( "Mob's can't setrace\n\r", ch );
		return;
	}
	
	if ( !ch->desc )
	{
		send_to_char( "You have no descriptor\n\r", ch );
		return;
	}
	
	smash_tilde( argument );
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	strcpy( arg3, argument );
    
	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Syntax: setrace <race> <field> <value>\n\r"
			"Syntax: setrace <race> create	     \n\r"
			"\n\rField being one of:\n\r"
			"  name classes strplus dexplus wisplus\n\r"
			"  intplus conplus chaplus lckplus\n\r"
			"  affected resist suscept language nospeak\n\r"
			"  save attack acplus \n\r"
			"  height weight hungermod thirstmod \n\r"
			"  ability death_age\n\r", ch );
		return;
	}
	if ( is_number(arg1) && (ra=atoi(arg1)) >= 0 && ra < MAX_RACE )
		race = race_table[ra];
	else
	{
		race = NULL;
		for ( ra = 0; ra < MAX_RACE && race_table[ra]; ra++ )
		{
			if ( !race_table[ra]->race_name ) continue;
			if ( !str_cmp(race_table[ra]->race_name, arg1) )
			{
				race = race_table[ra];
				break;
			}
		}
	}
	if ( !str_cmp( arg2, "create" ) && race )
	{
		send_to_char( "That race already exists!\n\r", ch );
		return;
	}
	else if ( !race && str_cmp(arg2, "create") )
	{
		send_to_char( "No such race.\n\r", ch );
		return;
	}
	
	if ( !str_cmp( arg2, "save" ) )
	{
		write_race_file( ra );
		send_to_char( "Saved.\n\r", ch );
		return;
	}
    
	if ( !str_cmp( arg2, "create") )
	{
		if ( MAX_PC_RACE >= MAX_RACE )
		{
			send_to_char("You need to up MAX_RACE in mud.h and make clean.\n\r",ch);
			return;
		}
		if(  (create_new_race( MAX_PC_RACE, arg1 )) == FALSE )
		{
			send_to_char("Couldn't create a new race.\n\r", ch );
			return;
		}
		write_race_file( MAX_PC_RACE );
		MAX_PC_RACE++;
		sprintf( racelist, "%s%s", RACEDIR, RACE_LIST );
		if ( ( fpList = fopen( racelist, "w" ) ) == NULL )
		{
			bug( "Error opening racelist.", 0 );
			return;
		}
		for ( i = 0; i < MAX_PC_RACE; i++ )
			fprintf( fpList, "%s%s.race\n", RACEDIR, race_table[i]->race_name );
		fprintf( fpList, "$\n" );
		fclose( fpList );
		send_to_char("Done.\n\r", ch );
		return;
	}
	
	if( !argument)
	{
		send_to_char("You must specify an argument.\n\r",ch);
		return;
	}
	
	if ( !str_cmp( arg2, "name" ) )
	{
		sprintf(race->race_name,"%-.16s",argument);
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
    if ( !str_cmp( arg2, "strplus" ) )
	{
		race->str_plus = (sh_int) atoi( argument );
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
	if ( !str_cmp( arg2, "dexplus" ) )
	{
		race->dex_plus = (sh_int) atoi( argument );
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
	if ( !str_cmp( arg2, "wisplus" ) )
	{
		sprintf(buf,"attempting to set wisplus to %s\n\r",argument);
		send_to_char(buf,ch);
		race->wis_plus = (sh_int) atoi( argument );
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
	if ( !str_cmp( arg2, "intplus" ) )
	{
		race->int_plus = (sh_int) atoi( argument );
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
	if ( !str_cmp( arg2, "conplus" ) )
	{
		race->con_plus = (sh_int) atoi( argument );
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
	if ( !str_cmp( arg2, "chaplus" ) )
	{
		race->cha_plus = (sh_int) atoi( argument );
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
	if ( !str_cmp( arg2, "lckplus" ) )
	{
		race->lck_plus = (sh_int) atoi( argument );
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
	if ( !str_cmp( arg2, "affected" ) )
	{
		if ( !argument || argument[0] == '\0' )
		{
			send_to_char( "Usage: setrace <race> affected <flag> [flag]...\n\r", ch );
			return;
		}
		while ( argument[0] != '\0' )
		{
			argument = one_argument( argument, arg3 );
			value = get_aflag( arg3 );
			if ( value < 0 || value > 31 )
				ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
			else
				TOGGLE_BIT( race->affected, 1 << value );
		}
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
	
	if ( !str_cmp( arg2, "resist" ) )
	{
		if ( !argument || argument[0] == '\0' )
		{
			send_to_char( "Usage: setrace <race> resist <flag> [flag]...\n\r", ch );
			return;
		}
		while ( argument[0] != '\0' )
		{
			argument = one_argument( argument, arg3 );
			value = get_risflag( arg3 );
			if ( value < 0 || value > 31 )
				ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
			else
				TOGGLE_BIT( race->resist, 1 << value );
		}
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
    if ( !str_cmp( arg2, "suscept" ) )
	{
		if ( !argument || argument[0] == '\0' )
		{
			send_to_char( "Usage: setrace <race> suscept <flag> [flag]...\n\r", ch );
			return;
		}
		while ( argument[0] != '\0' )
		{
			argument = one_argument( argument, arg3 );
			value = get_risflag( arg3 );
			if ( value < 0 || value > 31 )
				ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
			else
				TOGGLE_BIT( race->suscept, 1 << value );
		}
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
	if ( !str_cmp( arg2, "nospeak" ) )
	{
		if ( !argument || argument[0] == '\0' )
		{
			send_to_char( "Usage: setrace <race> nospeak <language> [language]...\n\r",ch);
			return;
		}
		while ( argument[0] != '\0' )
		{
			argument = one_argument( argument, arg3 );
			value = get_langflag( arg3 );
			if ( value == LANG_UNKNOWN )
				ch_printf( ch, "Unknown language: %s\n\r", arg3 );
			else
				TOGGLE_BIT( race->nospeak, value );
		}
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}

	if ( !str_cmp( arg2, "language" ) )
	{
		if ( !argument || argument[0] == '\0' )
		{
			send_to_char( "Usage: setrace <race> language language#\n\r", ch );
			return;
		}
		race->language = atoi( argument );
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
	if ( !str_cmp( arg2, "classes" ) )
	{
		for(i=0;i<MAX_ABILITY;i++)
		{
			if( !str_cmp(argument,ability_name[i]) )
			{   
				TOGGLE_BIT( race->class_restriction, 1 << i);
				send_to_char( "Done.\n\r", ch );
				return;
			}
		}
		send_to_char( "No such class.\n\r", ch );
		return;
	}
	/* Setable Bonus/penatly for races per ability */
	if ( !str_cmp( arg2, "ability" ) )
	{
		char ability[MAX_INPUT_LENGTH];
		char plusmin[MAX_INPUT_LENGTH];
		sh_int abilnum;
		sh_int bonus;
		argument = one_argument( argument, ability );
		argument = one_argument( argument, plusmin );
		
		if ( plusmin[0] == '\0' || ability[0] == '\0' )
		{
			send_to_char( "Usage: setrace <race> ability <ability #> <+- #> \n\r", ch );
			ch_printf( ch, "Ability # must be between 0 and %d\n\r",(MAX_ABILITY-1));
			return;
		}
		
		abilnum = (sh_int) atoi( ability );
		bonus = (sh_int) atoi(plusmin);
		
		if ( ( abilnum < 0 || abilnum > MAX_ABILITY ) )
		{
			send_to_char( "Usage: setrace <race> ability <ability #> <+- #> \n\r", ch );
			ch_printf( ch, "Ability # must be between 0 and %d\n\r",(MAX_ABILITY-1));
			return;
		}
		
		race->ability[abilnum] = bonus;
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
    if ( !str_cmp( arg2, "acplus" ) )
    {
        race->ac_plus = atoi( argument );
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
	if ( !str_cmp( arg2, "height" ) )
	{
		race->height = atoi( argument );
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
	if ( !str_cmp( arg2, "weight" ) )
	{
		race->weight = atoi( argument );
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
	if ( !str_cmp( arg2, "thirstmod" ) )
	{
		race->thirst_mod = atoi( argument );
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
	if ( !str_cmp( arg2, "hungermod" ) )
	{
		race->hunger_mod = atoi( argument );
		send_to_char( "Done.\n\r", ch );
		write_race_file( ra );
		return;
	}
	if ( !str_cmp( arg2, "death_age" ) )
	{
		if ( is_number(argument) )
		{
			race->death_age = (int) atoi( argument );
			send_to_char( "Done. \n\r", ch);
			write_race_file( ra );
			return;
		}
		else
		{
			send_to_char( "You must enter a number.\n\r", ch);
			return;
		}
	}
	if ( !str_cmp(arg2, "consent" ) )
	{
		if ( is_number(argument) )
		{
			race->consent = (int) atoi(argument);
			send_to_char("Done.\n\r",ch);
			write_race_file(ra);
			return;
		}
		else
		{
			send_to_char("You must enter a number.\n\r",ch);
			return;
		}
	}
	do_setrace( ch, "");
}

void do_showrace( CHAR_DATA *ch, char *argument )
{
	char arg1[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	struct race_type *race;
	int ra,i, ct;
	
	set_pager_color( AT_PLAIN, ch );
	
	argument = one_argument( argument, arg1 );
	if ( arg1[0] == '\0' )
	{
		send_to_char( "Syntax: showrace <race> \n\r", ch );
		/* Show the races code addition by Blackmane */
		for ( i=0;i<MAX_RACE;i++)
		{
			if ( i>9 )
				pager_printf(ch, "%d> %-11s",i,race_table[i]->race_name );
			else 
				pager_printf(ch, "%d> %-12s",i,race_table[i]->race_name);
			if ( i%5==0 )
				send_to_pager("\n\r", ch );
		}
		send_to_pager("\n\r", ch );
		return;
	}
	if ( is_number(arg1) && (ra=atoi(arg1)) >= 0 && ra < MAX_RACE )
		race = race_table[ra];
	else
	{
		race = NULL;
		for ( ra = 0; ra < MAX_RACE && race_table[ra]; ra++ )
			if ( !str_cmp(race_table[ra]->race_name, arg1) )
			{
				race = race_table[ra];
				break;
			}
	}
	if ( !race )
	{
		send_to_char( "No such race.\n\r", ch );
		return;
	}

    sprintf( buf, "RACE: %s\n\r",race->race_name);
    send_to_char( buf, ch);
    ct=0;
    sprintf( buf, "Disallowed Classes: \n\r");
    send_to_char( buf, ch);
	
	for(i=0;i<MAX_ABILITY;i++)
	{
		if( IS_SET(race->class_restriction, 1 << i) )
		{
			ct++; 
			sprintf(buf,"%s ", ability_name[i]);
			send_to_char( buf, ch);
			if( ct%6==0) send_to_char("\n\r", ch);
		} 
	}
	if( (ct%6!=0) || (ct==0)) send_to_char("\n\r", ch);
    ct=0;
    sprintf( buf, "Allowed Classes: ");
    send_to_char( buf, ch);
    for(i=0;i<MAX_ABILITY;i++)
	{
		if( !IS_SET(race->class_restriction, 1 << i) )
		{
			ct++;
			sprintf(buf,"%s ", ability_name[i]);
			send_to_char( buf, ch);
			if( ct%6==0) send_to_char("\n\r", ch);
		} 
	}
    if( (ct%6!=0) || (ct==0)) send_to_char("\n\r", ch);

    sprintf( buf, "&B[&W+Str&B]&W: &R%-3d\t&B[&W+Dex&B]&W: &R%-3d\t&B[&W+Wis&B]&W: &R%-3d\t&B[&W+Int&B]&W: &R%-3d\t\n\r",
                           race->str_plus, race->dex_plus, race->wis_plus, race->int_plus);
    send_to_char( buf, ch);
    sprintf( buf, "&B[&W+Con&B]&W: &R%-3d\t&B[&W+Cha&B]&W: &R%-3d\t&B[&W+Lck&B]&W: &R%-3d\t&B[&WAC&B]&W: &R%-d\n\r",
                           race->con_plus, race->cha_plus, race->lck_plus, race->ac_plus);
    send_to_char( buf, ch);
    
   	sprintf( buf, "&B[&WHeight   &B]&W: &R%3d in.\t&B[&WWeight&B]&W: &R%4d lbs.\t&B[&WHungerMod&B]&W: &R%d\t\t&B[&WThirstMod&B]&W: &R%d\n\r",
                           race->height, race->weight, race->hunger_mod, race->thirst_mod);
    send_to_char( buf, ch);

	ch_printf( ch, "&B[&WAffected by&B]&W:&R %s\n\r", affect_bit_name( race->affected ) );

    send_to_char( "\t&B[&WResistant to&B]&W: &R ", ch);
    send_to_char( flag_string(race->resist, ris_flags), ch);
    send_to_char( "\n\r", ch);

    send_to_char( "\t&B[&WSusceptible to&B]&W: &R ", ch);
    send_to_char( flag_string(race->suscept, ris_flags), ch);
    send_to_char( "\n\r", ch);

	send_to_char( "&B[&WLanguages&B]&W: &R ", ch);
    send_to_char( lang_names[race->language], ch );
    send_to_char( "\n\r", ch);
	send_to_char( "&B[&WDisallowed Languages&B]&W: &R",ch);
	ct=0;
	for(i=0;i<32;i++)
	{
		if( IS_SET(race->nospeak, 1 << i) )
		{
			ct++; 
			send_to_char(lang_names[i],ch);
			send_to_char( " ", ch);
			if( ct%6==0) send_to_char("\n\r", ch);
		} 
	}
	if( (ct%6!=0) || (ct==0)) send_to_char("\n\r", ch);
    
	ch_printf(ch, "&B[&WDeath Age&B]&W:  &R %d\t&B[&WAge Of Consent&B]&W:  &R %d\n\r",
		race->death_age, race->consent);

	send_to_char( "&B[&WAbilities&B]&W: &R\n\r", ch);
	for ( i = 0; i < MAX_ABILITY ; i++ )
	{
		ch_printf(ch, "   &B[&W%-15s&B]: &R%d   ", ability_name[i],race->ability[i]);
		i++;
		if ( i < MAX_ABILITY ) 
			ch_printf(ch, "   &B[&W%-15s&B]: &R%d\n\r", ability_name[i],race->ability[i]);
	}
    send_to_char( "\n\r", ch);
}

void do_viewskills( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	int sn;
	int col;

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
	send_to_char( "&zSyntax: skills <player>.\n\r", ch );
	return;
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
	send_to_char("No such person in the game.\n\r", ch );
	return;
	}

	col = 0;

	if ( !IS_NPC( victim ) )
	{
	set_char_color( AT_MAGIC, ch );
	for ( sn = 0; sn < top_sn && skill_table[sn] && skill_table[sn]->name; sn++ )
	{
	if ( skill_table[sn]->name == NULL )
	break;
	if ( victim->pcdata->learned[sn] == 0 )
	continue;

	sprintf( buf, "&W%20s &R%3d%% ", skill_table[sn]->name,
	victim->pcdata->learned[sn]);
	send_to_char( buf, ch );

	if ( ++col % 3 == 0 )
	send_to_char( "\n\r", ch );
	}
	}
	return;
}

/*
 *  "Clones" immortal command
 *  Author: Cronel (supfly@geocities.com)
 *  of FrozenMUD (empire.digiunix.net 4000)
 *
 *  Permission to use and distribute this code is granted provided
 *  this header is retained and unaltered, and the distribution
 *  package contains all the original files unmodified.
 *  If you modify this code and use/distribute modified versions
 *  you must give credit to the original author(s).
 */


void do_clones(CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA *dsrc, *ddst, *dsrc_next, *ddst_next;
	DESCRIPTOR_DATA *dlistf, *dlistl;
	sh_int clone_count;

	set_pager_color( AT_PLAIN, ch );
	pager_printf( ch, " %-12.12s | %-12.12s | %-s\n\r", "characters", "user", "host" );
	pager_printf( ch, "--------------+--------------+---------------------------------------------\n\r" );

	dlistf = dlistl = NULL;

	for( dsrc = first_descriptor ; dsrc ; dsrc = dsrc_next )
	{
		if( (dsrc->character && !can_see(ch, dsrc->character)) 
		|| !dsrc->user || !dsrc->host )
		{
			dsrc_next = dsrc->next;
			continue;
		}

		pager_printf( ch, " %-12.12s |",
			dsrc->original ? dsrc->original->name : 
			(dsrc->character ? dsrc->character->name : "(No name)") );
		clone_count = 1;

		for( ddst = first_descriptor ; ddst ; ddst = ddst_next )
		{
			ddst_next = ddst->next;

			if( dsrc == ddst )
				continue;
			if( (ddst->character && !can_see(ch, dsrc->character)) 
			|| !ddst->user || !ddst->host )
				continue;

			if( !str_cmp( dsrc->user, ddst->user ) 
			&& !str_cmp( dsrc->host, ddst->host ) )
			{
				UNLINK(ddst, first_descriptor, last_descriptor, next, prev );
				LINK( ddst, dlistf, dlistl, next, prev );
				pager_printf( ch, "              |\n\r %-12.12s |",
					ddst->original ? ddst->original->name : 
					(ddst->character ? ddst->character->name : "(No name)") );
				clone_count++;
			}
		}

		pager_printf( ch, " %-12.12s | %s (%d clone%s)\n\r", 
			dsrc->user, dsrc->host,
			clone_count,
			clone_count > 1 ? "s" : "" );

		dsrc_next = dsrc->next;

		UNLINK(dsrc, first_descriptor, last_descriptor, next, prev );
		LINK( dsrc, dlistf, dlistl, next, prev );
	}


	for( dsrc = dlistf ; dsrc ; dsrc = dsrc_next )
	{
		dsrc_next = dsrc->next;
		UNLINK( dsrc, dlistf, dlistl, next, prev );
		LINK(dsrc, first_descriptor, last_descriptor, next, prev );
	}
}

/* Summer 1997 --Blod */
void do_scatter( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *pRoomIndex;

    set_char_color( AT_IMMORT, ch );

    one_argument( argument, arg );
    if ( arg[0] == '\0' ) {
      send_to_char( "Scatter whom?\n\r", ch );
      return; }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL ) {
      send_to_char( "They aren't here.\n\r", ch );
      return; }
    if ( !IS_NPC(victim) && get_trust( victim ) >= get_trust( ch ) ) {
      send_to_char( "You haven't the power to succeed against them.\n\r", ch );
      return; }
    for ( ; ; ) {
      pRoomIndex = get_room_index( number_range( 0, 1048576000 ) );
      if ( pRoomIndex )
      if ( !xIS_SET(pRoomIndex->room_flags, ROOM_PRIVATE)
      &&   !xIS_SET(pRoomIndex->room_flags, ROOM_PLR_HOME)
      &&   !xIS_SET(pRoomIndex->room_flags, ROOM_PROTOTYPE) )
      break; }
    if ( victim->fighting ) stop_fighting( victim, TRUE );
    act( AT_MAGIC, "With the sweep of an arm, $n flings $N to the winds.", ch, NULL, victim, TO_NOTVICT );
    act( AT_MAGIC, "With the sweep of an arm, $n flings you to the astral winds.", ch, NULL, victim, TO_VICT );
    act( AT_MAGIC, "With the sweep of an arm, you fling $N to the astral winds.", ch, NULL, victim, TO_CHAR );
    char_from_room( victim );
    char_to_room( victim, pRoomIndex );
    victim->position = POS_RESTING;
    act( AT_MAGIC, "$n staggers forth from a sudden gust of wind, and collapses.", victim, NULL, NULL, TO_ROOM );
    do_look( victim, "auto" );
    return;
}

void do_strew( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj_next;
    OBJ_DATA *obj_lose;
    ROOM_INDEX_DATA *pRoomIndex;

    set_char_color( AT_IMMORT, ch );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' || arg2[0] == '\0' ) {
      send_to_char( "Strew who, what?\n\r", ch );
      return;
    }
    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL ) {
      send_to_char( "It would work better if they were here.\n\r", ch );
      return;
    }
    if ( victim == ch ) {
      send_to_char( "Try taking it out on someone else first.\n\r", ch );
      return;
    }
    if ( !IS_NPC(victim) && get_trust( victim ) >= get_trust( ch ) ) {
      send_to_char( "You haven't the power to succeed against them.\n\r", ch );
      return;
    }
    if ( !str_cmp( arg2, "coins"  ) ) {
      if ( victim->gold < 1) {
        send_to_char( "Drat, this one's got no gold to start with.\n\r", ch );
        return;
      }
      victim->gold = 0;
      act( AT_MAGIC, "$n gestures and an unearthly gale sends $N's coins flying!", ch, NULL, victim, TO_NOTVICT );
      act( AT_MAGIC, "You gesture and an unearthly gale sends $N's coins flying!", ch, NULL, victim, TO_CHAR );
      act( AT_MAGIC, "As $n gestures, an unearthly gale sends your currency flying!", ch, NULL, victim, TO_VICT );
      return;
    }
    for ( ; ; ) {
      pRoomIndex = get_room_index( number_range( 0, 32767 ) );
        if ( pRoomIndex )
          if ( !xIS_SET(pRoomIndex->room_flags, ROOM_PRIVATE)
          &&   !xIS_SET(pRoomIndex->room_flags, ROOM_PLR_HOME)
          &&   !xIS_SET(pRoomIndex->room_flags, ROOM_PROTOTYPE) )
        break;
    }
    if ( !str_cmp( arg2, "inventory" ) ) {
      act( AT_MAGIC, "$n speaks a single word, sending $N's possessions flying!", ch, NULL, victim, TO_NOTVICT );
      act( AT_MAGIC, "You speak a single word, sending $N's possessions flying!", ch, NULL, victim, TO_CHAR );
      act( AT_MAGIC, "$n speaks a single word, sending your possessions flying!", ch, NULL, victim, TO_VICT );
      for ( obj_lose=victim->first_carrying; obj_lose; obj_lose=obj_next ) {
        obj_next = obj_lose->next_content;
        obj_from_char( obj_lose );
        obj_to_room( obj_lose, pRoomIndex );
        pager_printf( ch, "\t&w%s sent to %d\n\r", capitalize(obj_lose->short_descr), pRoomIndex->vnum );
      }
      return;
    }  
    send_to_char( "Strew their coins or inventory?\n\r", ch );
    return;
}

void do_strip( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj_next;
    OBJ_DATA *obj_lose;
    int count = 0;

    set_char_color( AT_OBJECT, ch );
    if ( !argument ) {
      send_to_char( "Strip who?\n\r", ch );
      return;
    }
    if ( ( victim = get_char_room( ch, argument ) ) == NULL ) {
      send_to_char( "They're not here.\n\r", ch );
      return;
    }
    if ( victim == ch ) {
      send_to_char( "Kinky.\n\r", ch );
      return;
    }
    if ( !IS_NPC(victim) && get_trust( victim ) >= get_trust( ch ) ) {
      send_to_char( "You haven't the power to succeed against them.\n\r", ch );
      return;
    }
    act( AT_OBJECT, "Searching $N ...", ch, NULL, victim, TO_CHAR );
    for ( obj_lose=victim->first_carrying; obj_lose; obj_lose=obj_next ) {
      obj_next = obj_lose->next_content;
      obj_from_char( obj_lose );
      obj_to_char( obj_lose, ch );
      pager_printf_color( ch, "  &G... %s (&g%s) &Gtaken.\n\r",
	capitalize(obj_lose->short_descr), obj_lose->name );
      count++;
    }
    if ( !count )
      pager_printf_color( ch, "&GNothing found to take.\n\r", ch );
    return;
}  

void do_khistory(CHAR_DATA *ch, char *argument)
{
	MOB_INDEX_DATA *tmob;
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *vch;
	int track;

	if(IS_NPC(ch) || !IS_IMMORTAL(ch))
	{
		ch_printf(ch, "Huh?\n\r");
		return;
	}
		
	one_argument(argument, arg);
	
	if(arg[0] == '\0')
	{
		ch_printf(ch, "syntax: khistory <player>\n\r");
		return;
	}
	
	vch = get_char_world(ch, arg);
	
	if(!vch || IS_NPC(vch))
	{
		ch_printf(ch, "They are not here.\n\r");
		return;
	}

	set_char_color(AT_BLOOD, ch);
	ch_printf(ch, "Kill history for %s:\n\r", vch->name);
	
	for(track = 0; track < MAX_KILLTRACK &&
			vch->pcdata->killed[track].vnum; track++)
	{
		tmob = get_mob_index(vch->pcdata->killed[track].vnum);
		
		if(!tmob)
		{
			bug("killhistory: unknown mob vnum",0);
			continue;
		}
		
		set_char_color(AT_RED, ch);
		ch_printf(ch, "   %-30s", capitalize(tmob->short_descr));
		set_char_color(AT_BLOOD, ch);
		ch_printf(ch, "(");
		set_char_color(AT_RED, ch);
		ch_printf(ch, "%-7d", tmob->vnum);
		set_char_color(AT_BLOOD, ch);
		ch_printf(ch, ")");
		set_char_color(AT_RED, ch);
		ch_printf(ch, "    - killed %d times.\n\r",
			vch->pcdata->killed[track].count);
	}
	
	return;
}

/*
 * Command to display the weather status of all the areas
 * Last Modified: July 21, 1997
 * Fireblade
 */
void do_showweather(CHAR_DATA *ch, char *argument)
{
	AREA_DATA *pArea;
	char arg[MAX_INPUT_LENGTH];
	
	if(!ch)
	{
		bug("do_showweather: NULL char data");
		return;
	}
	
	argument = one_argument(argument, arg);

	set_char_color(AT_BLUE, ch);
	ch_printf(ch, "%-40s%-8s %-8s %-8s\n\r",
		"Area Name:", "Temp:", "Precip:", "Wind:");
	
	for(pArea = first_area; pArea; pArea = pArea->next)
	{
		if(arg[0] == '\0' ||
			nifty_is_name_prefix(arg, pArea->name))
		{
			set_char_color(AT_BLUE, ch);
			ch_printf(ch, "%-40s", pArea->name);
			set_char_color(AT_WHITE, ch);
			ch_printf(ch, "%3d", pArea->weather->temp);
			set_char_color(AT_BLUE, ch);
			ch_printf(ch, "(");
			set_char_color(AT_LBLUE, ch);
			ch_printf(ch, "%3d", pArea->weather->temp_vector);
			set_char_color(AT_BLUE,ch);
			ch_printf(ch, ") ");
			set_char_color(AT_WHITE,ch);
			ch_printf(ch, "%3d", pArea->weather->precip);
			set_char_color(AT_BLUE, ch);
			ch_printf(ch, "(");
			set_char_color(AT_LBLUE, ch);
			ch_printf(ch, "%3d", pArea->weather->precip_vector);
			set_char_color(AT_BLUE, ch);
			ch_printf(ch, ") ");
			set_char_color(AT_WHITE, ch);
			ch_printf(ch, "%3d", pArea->weather->wind);
			set_char_color(AT_BLUE, ch);
			ch_printf(ch, "(");
			set_char_color(AT_LBLUE, ch);
			ch_printf(ch, "%3d", pArea->weather->wind_vector);
			set_char_color(AT_BLUE, ch);
			ch_printf(ch, ")\n\r");
		}
	}
	
	return;
}

/*
 * Command to control global weather variables and to reset weather
 * Last Modified: July 23, 1997
 * Fireblade
 */
void do_setweather(CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	set_char_color(AT_BLUE, ch);
	
	argument = one_argument(argument, arg);
	
	if(arg[0] == '\0')
	{
		ch_printf(ch, "%-15s%-6s\n\r",
			"Parameters:", "Value:");
		ch_printf(ch, "%-15s%-6d\n\r",
			"random", rand_factor);
		ch_printf(ch, "%-15s%-6d\n\r",
			"climate", climate_factor);
		ch_printf(ch, "%-15s%-6d\n\r",
			"neighbor", neigh_factor);
		ch_printf(ch, "%-15s%-6d\n\r",
			"unit", weath_unit);
		ch_printf(ch, "%-15s%-6d\n\r",
			"maxvector", max_vector);
		
		ch_printf(ch, "\n\rResulting values:\n\r");
		ch_printf(ch, "Weather variables range from "
			"%d to %d.\n\r", -3*weath_unit,
			3*weath_unit);
		ch_printf(ch, "Weather vectors range from "
			"%d to %d.\n\r", -1*max_vector,
			max_vector);
		ch_printf(ch, "The maximum a vector can "
			"change in one update is %d.\n\r",
			rand_factor + 2*climate_factor +
			(6*weath_unit/neigh_factor));
	}
	else if(!str_cmp(arg, "random"))
	{
		if(!is_number(argument))
		{
			ch_printf(ch, "Set maximum random "
				"change in vectors to what?\n\r");
		}
		else
		{
			rand_factor = atoi(argument);
			ch_printf(ch, "Maximum random "
				"change in vectors now "
				"equals %d.\n\r", rand_factor);
			save_weatherdata();
		}
	}
	else if(!str_cmp(arg, "climate"))
	{
		if(!is_number(argument))
		{
			ch_printf(ch, "Set climate effect "
				"coefficient to what?\n\r");
		}
		else
		{
			climate_factor = atoi(argument);
			ch_printf(ch, "Climate effect "
				"coefficient now equals "
				"%d.\n\r", climate_factor);
			save_weatherdata();
		}
	}
	else if(!str_cmp(arg, "neighbor"))
	{
		if(!is_number(argument))
		{	
			ch_printf(ch, "Set neighbor effect "
				"divisor to what?\n\r");
		}
		else
		{
			neigh_factor = atoi(argument);

			if(neigh_factor <= 0)
				neigh_factor = 1;

			ch_printf(ch, "Neighbor effect "
				"coefficient now equals "
				"1/%d.\n\r", neigh_factor);
			save_weatherdata();
		}
	}
	else if(!str_cmp(arg, "unit"))
	{
		if(!is_number(argument))
		{
			ch_printf(ch, "Set weather unit "
				"size to what?\n\r");
		}
		else
		{
			weath_unit = atoi(argument);
			ch_printf(ch, "Weather unit size "
				"now equals %d.\n\r",
				weath_unit);
			save_weatherdata();
		}
	}
	else if(!str_cmp(arg, "maxvector"))
	{
		if(!is_number(argument))
		{
			ch_printf(ch, "Set maximum vector "
				"size to what?\n\r");
		}
		else
		{
			max_vector = atoi(argument);
			ch_printf(ch, "Maximum vector size "
				"now equals %d.\n\r",
				max_vector);
			save_weatherdata();
		}
	}
	else if(!str_cmp(arg, "reset"))
	{
		init_area_weather();
		ch_printf(ch, "Weather system reinitialized.\n\r");
	}
	else if(!str_cmp(arg, "update"))
	{
		int i, number;
		
		number = atoi(argument);
				
		if(number < 1)
			number = 1;
		
		for(i = 0; i < number; i++)
			weather_update();
	
		ch_printf(ch, "Weather system updated.\n\r");
	}
	else
	{
		ch_printf(ch, "You may only use one of the "
			"following fields:\n\r");
		ch_printf(ch, "\trandom\n\r\tclimate\n\r"
			"\tneighbor\n\r\tunit\n\r\tmaxvector\n\r");
		ch_printf(ch, "You may also reset or update "
			"the system using the fields 'reset' "
			"and 'update' respectively.\n\r");
	}

	return;
}

/*
 * Command to check for multiple ip addresses in the mud.
 * --Shaddai
 */

 /*
  * Added this new struct to do matching
  * If ya think of a better way do it, easiest way I could think of at
  * 2 in the morning :) --Shaddai
  */

typedef struct ipcompare_data IPCOMPARE_DATA;
struct ipcompare_data {
        struct ipcompare_data *prev;
        struct ipcompare_data *next;
        char   *host;
        char   *name;
	char   *user;
        int    connected;
	int    count;
        int    descriptor;
        int    idle;
        int    port;
        bool   printed;
        };

void do_ipcompare ( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *victim;
	DESCRIPTOR_DATA *d;
	char arg[MAX_INPUT_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char *addie = NULL;
	bool prefix = FALSE, suffix = FALSE, inarea = FALSE, inroom = FALSE, inworld = FALSE;
	int count = 0, times = -1;
	bool fMatch;
	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg1);
	argument = one_argument( argument, arg2);

	set_pager_color (AT_PLAIN, ch);

	if ( IS_NPC(ch) )
	{
        send_to_char("Huh?\n\r", ch );
        return;
	}

	if ( arg[0] == '\0' )
	{
		send_to_char( "ipcompare total\n\r", ch );
		send_to_char( "ipcompare <person> [room|area|world] [#]\n\r", ch );
		send_to_char( "ipcompare <site>   [room|area|world] [#]\n\r", ch );
        return;
	}
	if ( !str_cmp ( arg, "total" ) )
	{
		IPCOMPARE_DATA *first_ip=NULL,*last_ip=NULL,*hmm,*hmm_next;
		for ( d = first_descriptor; d; d = d->next)
		{
			fMatch = FALSE;
			for ( hmm = first_ip; hmm; hmm = hmm->next )
				if ( !str_cmp( hmm->host, d->host ) )
					fMatch = TRUE;
				if ( !fMatch )
				{
					IPCOMPARE_DATA *temp;
					CREATE( temp, IPCOMPARE_DATA, 1);
					temp->host = str_dup ( d->host );
					LINK( temp, first_ip, last_ip, next, prev );
					count++;
                }
        }
        for ( hmm = first_ip; hmm; hmm = hmm_next )
        {
			hmm_next = hmm->next;
			UNLINK( hmm, first_ip, last_ip, next, prev );
			if ( hmm->host )
				DISPOSE( hmm->host );
			DISPOSE( hmm );
        }
        ch_printf(ch, "There were %d unique ip addresses found.\n\r",
			count );
        return;
	}
	if ( arg1[0] != '\0' )
	{
        if ( is_number( arg1 ) )
			times = atoi( arg1 );
        else
        {
			if ( !str_cmp ( arg1, "room" ) )
                inroom = TRUE;
			else if ( !str_cmp ( arg1, "area" ) )
                inarea = TRUE;
			else
                inworld = TRUE;
        }
        if ( arg2[0] != '\0' )
        {
			if ( is_number( arg2 ) )
                times = atoi( arg2 );
			else
			{
                send_to_char("Please see help ipcompare for more info.\n\r",ch);
                return;
			}
        }
	}
	if ( ( victim = get_char_world( ch, arg ) ) != NULL  && victim->desc)
	{
        if ( IS_NPC(victim) )
        {
			send_to_char ( "Not on NPC's.\n\r", ch );
			return;
        }
        addie = victim->desc->host;
	}
	else
	{
        addie = arg;
        if ( arg[0] == '*' )
        {
			prefix = TRUE;
			addie++;
        }
        if ( addie[strlen(addie) -1] == '*' )
        {
			suffix = TRUE;
			addie[strlen(addie)-1] = '\0';
        }
	}
	sprintf (buf, "\n\rDesc|Con|Idle| Port | Player      ");
	if (get_trust (ch) >= LEVEL_SAVIOR )
		strcat (buf, "@HostIP           ");
	if (get_trust (ch) >= LEVEL_GOD)
		strcat (buf, "| Username");
	strcat (buf, "\n\r");
	strcat (buf, "----+---+----+------+-------------");
	if ( get_trust ( ch ) >= LEVEL_SAVIOR )
		strcat (buf, "------------------" );
	if (get_trust (ch) >= LEVEL_GOD)
		strcat (buf, "+---------");
	strcat (buf, "\n\r");
	send_to_pager (buf, ch);
	for ( d = first_descriptor; d; d = d->next)
	{
		if ( !d->character || (d->connected != CON_PLAYING &&
			d->connected != CON_EDITING) || !can_see (ch, d->character) )
			continue;
		if ( inroom && ch->in_room != d->character->in_room )
			continue;
		if ( inarea && ch->in_room->area != d->character->in_room->area )
			continue;
		if ( times > 0 && count == (times - 1 ) )
			break;
		if ( prefix && suffix && strstr( addie, d->host ) )
			fMatch = TRUE;
		else if ( prefix && !str_suffix( addie , d->host) )
			fMatch = TRUE;
		else if ( suffix && !str_prefix( addie , d->host) )
			fMatch = TRUE;
		else if ( !str_cmp( d->host, addie ) )
			fMatch = TRUE;
		else
			fMatch = FALSE;
		if ( fMatch )
		{
			count++;
			sprintf (buf,
				" %3d| %2d|%4d|%6d| %-12s",
				d->descriptor,
				d->connected,
				d->idle / 4,
				d->port,
				d->original ? d->original->name :
			d->character ? d->character->name : "(none)");
			if (get_trust (ch) >= LEVEL_SAVIOR )
				sprintf(buf + strlen (buf), "@%-16s ", d->host );
			if (get_trust (ch) >= LEVEL_GOD)
				sprintf (buf + strlen (buf), "| %s", d->user);
			strcat (buf, "\n\r");
			send_to_pager (buf, ch);
		}
	}
	pager_printf (ch, "%d user%s.\n\r", count, count == 1 ? "" : "s");
	return;
}

/*
 * New nuisance flag to annoy people that deserve it :) --Shaddai
 */
void do_nuisance ( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  struct tm *now_time;
  int time = 0, max_time = 0, power = 1;
  bool minute = FALSE, day = FALSE, hour = FALSE;

  if ( IS_NPC(ch) )
  {
        send_to_char("Huh?\n\r", ch );
        return;
  }

  argument = one_argument ( argument, arg );

  if ( arg[0] == '\0' )
  {
	send_to_char("Syntax: nuisance <victim> [Options]\n\r", ch );
	send_to_char("Options:\n\r", ch );
	send_to_char("  power <level 1-10>\n\r", ch );
	send_to_char("  time  <days>\n\r", ch );
	send_to_char("  maxtime <#> <minutes/hours/days>\n\r", ch );
	send_to_char("Defaults: Time -- forever, power -- 1, maxtime 8 days.\n\r", ch );
	return;
  }

  if ( (victim  = get_char_world( ch, arg ) ) == NULL )
  {
        send_to_char("There is no one on with that name.\n\r", ch );
        return;
  }

  if ( IS_NPC(victim) )
  {
        send_to_char("You can't set a nuisance flag on a mob.\n\r", ch );
        return;
  }

  if ( get_trust( ch ) <= get_trust(victim) )
  {
        send_to_char("I don't think they would like that.\n\r", ch );
        return;
  }

  if ( victim->pcdata->nuisance )
  {
        send_to_char("That flag has already been set.\n\r", ch );
        return;
  }

  argument = one_argument ( argument, arg1 );

  while ( argument[0] != '\0' )
  {
        if ( !str_cmp( arg1, "power" ) )
        {
                argument = one_argument ( argument, arg1 );
                if ( arg1[0] == '\0' || !is_number(arg1) )
                {
                  send_to_char("Power option syntax: power <number>\n\r",ch);
                  return;
                }
                if ( (power = atoi(arg1)) < 1 || power > 10 )
                {
                  send_to_char("Power must be 1 - 10.\n\r", ch );
                  return;
                }
        }
        else if ( !str_cmp ( arg1, "time" ) )
        {
                argument = one_argument ( argument, arg1 );
                if ( arg1[0] == '\0' || !is_number(arg1) )
                {
                  send_to_char("Time option syntax: time <number> (In days)\n\r"
, ch );
                  return;
                }
                if ( ( time = atoi(arg1) ) < 1 )
                {
                  send_to_char("Time must be a positive number.\n\r", ch );
                  return;
                }
        }
        else if ( !str_cmp ( arg1, "maxtime" ) )
        {
                argument = one_argument ( argument, arg1 );
                argument = one_argument ( argument, arg2 );
                if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number(arg1) )
                {
                  send_to_char("Maxtime option syntax: maxtime <number> <minute|day|hour>\n\r", ch );
                  return;
                }
                if ( (max_time = atoi(arg1)) < 1 )
                {
                  send_to_char("Maxtime must be a positive number.\n\r", ch );
                  return;
                }
                if ( !str_cmp(arg2, "minutes" ) )
                        minute = TRUE;
                else if ( !str_cmp(arg2, "hours" ) )
                        hour = TRUE;
                else if ( !str_cmp(arg2, "days" ) )
                        day = TRUE;
        }
	else
	{
		ch_printf(ch, "Unknown option %s.\n\r", arg1 );
		return;
	}
	argument = one_argument ( argument, arg1 );
  }

  if ( minute && (max_time < 1 || max_time > 59) )
  {
    send_to_char("Minutes must be 1 to 59.\n\r", ch );
    return;
  }
  else if ( hour && (max_time < 1 || max_time > 23 ) )
  {
    send_to_char("Hours must be 1 - 23.\n\r", ch );
    return;
  }
  else if ( day && ( max_time < 1 || max_time > 999 ) )
  {
    send_to_char("Days must be 1 - 999.\n\r", ch );
    return;
  }
  else if ( !max_time )
  {
        day = TRUE;
        max_time = 7;
  }
  CREATE ( victim->pcdata->nuisance, NUISANCE_DATA, 1 );
  victim->pcdata->nuisance->time = current_time;
  victim->pcdata->nuisance->flags = 1;
  victim->pcdata->nuisance->power = power;
  now_time = localtime(&current_time);

  if ( minute )
        now_time->tm_min += max_time;
  else if ( hour )
        now_time->tm_hour+= max_time;
  else 
        now_time->tm_mday+= max_time;

  victim->pcdata->nuisance->max_time = mktime(now_time);
  if ( time )
  {
    add_timer( victim, TIMER_NUISANCE, (28800*time), NULL, 0 );
    ch_printf(ch, "Nuisance flag set for %d days.\n\r", time );
  }
  else
    send_to_char("Nuisance flag set forever\n\r", ch );
  return;
}

void do_unnuisance ( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  TIMER *timer, *timer_next;
  char arg[MAX_INPUT_LENGTH];

  if ( IS_NPC(ch) )
  {
        send_to_char("Huh?\n\r", ch );
        return;
  }
  one_argument ( argument, arg );

  if ( (victim  = get_char_world( ch, arg ) ) == NULL )
  {
        send_to_char("There is no one on with that name.\n\r", ch );
        return;
  }
  if ( IS_NPC(victim) )
  {
        send_to_char("You can't remove a nuisance flag from a mob.\n\r", ch );
        return;
  }
  if ( get_trust( ch ) <= get_trust(victim) )
  {
        send_to_char("You can't do that.\n\r", ch );
        return;
  }
  if ( !victim->pcdata->nuisance )
  {
        send_to_char("They do not have that flag set.\n\r", ch );
        return;
  }
  for (timer = victim->first_timer; timer; timer = timer_next)
  {
      timer_next = timer->next;
      if ( timer->type == TIMER_NUISANCE )
        extract_timer (victim, timer);
  }
  DISPOSE( victim->pcdata->nuisance );
  send_to_char("Nuisance flag removed.\n\r", ch );
  return;
}

/*
 * Free a social structure					-Thoric
 * Converted for xsocials - Gavin 
 */
void free_xsocial( XSOCIALTYPE *xsocial )
{
    if ( xsocial->name )
      DISPOSE( xsocial->name );
    if ( xsocial->char_no_arg )
      DISPOSE( xsocial->char_no_arg );
    if ( xsocial->others_no_arg )
      DISPOSE( xsocial->others_no_arg );
    if ( xsocial->char_found )
      DISPOSE( xsocial->char_found );
    if ( xsocial->others_found )
      DISPOSE( xsocial->others_found );
    if ( xsocial->vict_found )
      DISPOSE( xsocial->vict_found );
    if ( xsocial->char_auto )
      DISPOSE( xsocial->char_auto );
    if ( xsocial->others_auto )
      DISPOSE( xsocial->others_auto );
    DISPOSE( xsocial );
}

/*
 * Remove a xsocial from it's hash index - Thoric
 * Converted for xsocials - Gavin 
 */
void unlink_xsocial( XSOCIALTYPE *xsocial )
{
    XSOCIALTYPE *tmp, *tmp_next;
    int hash;

    if ( !xsocial )
    {
		bug( "Unlink_xsocial: NULL xsocial", 0 );
		return;
    }

    if ( xsocial->name[0] < 'a' || xsocial->name[0] > 'z' )
		hash = 0;
    else
		hash = (xsocial->name[0] - 'a') + 1;
    if ( xsocial == (tmp=xsocial_index[hash]) )
    {
		xsocial_index[hash] = tmp->next;
		return;
    }
    for ( ; tmp; tmp = tmp_next )
    {
		tmp_next = tmp->next;
		if ( xsocial == tmp_next )
		{
			tmp->next = tmp_next->next;
			return;
		}
    }
}

/*
 * Add a social to the social index table			-Thoric
 * Hashed and insert sorted
 * Converted for xsocials - Gavin 
 */
void add_xsocial( XSOCIALTYPE *xsocial )
{
    int hash, x;
    XSOCIALTYPE *tmp, *prev;

    if ( !xsocial )
    {
		bug( "Add_xsocial: NULL xsocial", 0 );
		return;
    }

    if ( !xsocial->name )
    {
		bug( "Add_xsocial: NULL xsocial->name", 0 );
		return;
    }

    if ( !xsocial->char_no_arg )
    {
		bug( "Add_xsocial: NULL xsocial->char_no_arg", 0 );
		return;
    }

    /* make sure the name is all lowercase */
    for ( x = 0; xsocial->name[x] != '\0'; x++ )
		xsocial->name[x] = LOWER(xsocial->name[x]);

    if ( xsocial->name[0] < 'a' || xsocial->name[0] > 'z' )
		hash = 0;
    else
		hash = (xsocial->name[0] - 'a') + 1;

    if ( (prev = tmp = xsocial_index[hash]) == NULL )
    {
		xsocial->next = xsocial_index[hash];
		xsocial_index[hash] = xsocial;
		return;
    }

    for ( ; tmp; tmp = tmp->next )
    {
		if ( (x=strcmp(xsocial->name, tmp->name)) == 0 )
		{
			bug( "Add_xsocial: trying to add duplicate name to bucket %d", hash );
			free_xsocial( xsocial );
			return;
		}
		else if ( x < 0 )
		{
			if ( tmp == xsocial_index[hash] )
			{
				xsocial->next = xsocial_index[hash];
				xsocial_index[hash] = xsocial;
				return;
			}
			prev->next = xsocial;
			xsocial->next = tmp;
			return;
		}
		prev = tmp;
    }

    /* add to end */
    prev->next = xsocial;
    xsocial->next = NULL;
    return;
}

/*
 * Social editor/displayer/save/delete				-Thoric
 * Converted for xsocials - Gavin 
 */
void do_xsedit( CHAR_DATA *ch, char *argument )
{
    XSOCIALTYPE * xsocial;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
	char * const male_female[3] = { "Both",  "Male",  "Female" };

    set_char_color( AT_SOCIAL, ch );

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
		send_to_char( "Syntax: xsedit <xsocial> [field]\n\r", ch );
		send_to_char( "Syntax: xsedit <xsocial> create\n\r", ch );
		if ( get_trust(ch) > LEVEL_GOD )
			send_to_char( "Syntax: xsedit <xsocial> delete\n\r", ch );
		send_to_char( "Syntax: xsedit <save>\n\r", ch );
		send_to_char( "\n\rField being one of:\n\r", ch );
		send_to_char( "  cnoarg onoarg cfound ofound vfound cauto oauto\n\r", ch );
		send_to_char( "  genderon genderuse stage position self other chance\n\r", ch);

		return;
    }

    if ( !str_cmp( arg1, "save" ) )
    {
		save_xsocials();
		send_to_char( "Saved.\n\r", ch );
		return;
    }

    xsocial = find_xsocial( arg1 );
    if ( !str_cmp( arg2, "create" ) )
    {
		if ( xsocial )
		{
			send_to_char( "That social already exists!\n\r", ch );
			return;
		}
		CREATE( xsocial, XSOCIALTYPE, 1 );
		xsocial->name = str_dup( arg1 );
		sprintf( arg2, "You %s.", arg1 );
		xsocial->char_no_arg = str_dup( arg2 );
		xsocial->genderto = SEX_FEMALE;
		xsocial->gender = SEX_MALE;
		xsocial->stage = 0;
		xsocial->position = 0;
		xsocial->self = 0;
		xsocial->other = 0;
		xsocial->chance = 0; 
		add_xsocial( xsocial );
		send_to_char( "Xsocial added.\n\r", ch );
		return;
    }

    if ( !xsocial )
    {
		send_to_char( "Xsocial not found.\n\r", ch );
		return;
    }

    if ( arg2[0] == '\0' || !str_cmp( arg2, "show" ) )
    {
		ch_printf( ch, "Xsocial: %s\n\r\n\rCNoArg: %s\n\r",
			xsocial->name,	xsocial->char_no_arg );
		ch_printf( ch, "ONoArg: %s\n\rCFound: %s\n\rOFound: %s\n\r",
			xsocial->others_no_arg	? xsocial->others_no_arg	: "(not set)",
			xsocial->char_found		? xsocial->char_found	: "(not set)",
			xsocial->others_found	? xsocial->others_found	: "(not set)" );
		ch_printf( ch, "VFound: %s\n\rCAuto : %s\n\rOAuto : %s\n\r",
			xsocial->vict_found	? xsocial->vict_found	: "(not set)",
			xsocial->char_auto	? xsocial->char_auto	: "(not set)",
			xsocial->others_auto ? xsocial->others_auto	: "(not set)" );
		ch_printf( ch, "&CGender-on: &W%s\n\r"
			"&CGender-Use :&W %s\n\r"
			"&CStage : &W%d\n\r",
			male_female[xsocial->genderto],	
			male_female[xsocial->gender],
			xsocial->stage );
		ch_printf( ch, "&CPosition:&W %d\n\r"
			"&CSelf: &W%d\n\r"
			"&COther: &W%d\n\r",
			xsocial->position, xsocial->self, xsocial->other );
		ch_printf( ch, "&CChance:&W %d\n\r", xsocial->chance );
		return;
    }

    if ( get_trust(ch) > LEVEL_GOD && !str_cmp( arg2, "delete" ) )
    {
		unlink_xsocial( xsocial );
		free_xsocial( xsocial );
		send_to_char( "Deleted.\n\r", ch );
		return;
    }

    if ( !str_cmp( arg2, "cnoarg" ) )
    {
		if ( argument[0] == '\0' || !str_cmp( argument, "clear" ) )
		{
			send_to_char( "You cannot clear this field.  It must have a message.\n\r", ch );
			return;
		}
		if ( xsocial->char_no_arg )
			DISPOSE( xsocial->char_no_arg );
		xsocial->char_no_arg = str_dup( argument );
		send_to_char( "Done.\n\r", ch );
		return;
    }

    if ( !str_cmp( arg2, "onoarg" ) )
    {
		if ( xsocial->others_no_arg )
			DISPOSE( xsocial->others_no_arg );
		if ( argument[0] != '\0' && str_cmp( argument, "clear" ) )
			xsocial->others_no_arg = str_dup( argument );
		send_to_char( "Done.\n\r", ch );
		return;
    }

    if ( !str_cmp( arg2, "cfound" ) )
    {
		if ( xsocial->char_found )
			DISPOSE( xsocial->char_found );
		if ( argument[0] != '\0' && str_cmp( argument, "clear" ) )
			xsocial->char_found = str_dup( argument );
		send_to_char( "Done.\n\r", ch );
		return;
    }
	if ( !str_cmp( arg2, "ofound" ) )
    {
		if ( xsocial->others_found )
			DISPOSE( xsocial->others_found );
		if ( argument[0] != '\0' && str_cmp( argument, "clear" ) )
			xsocial->others_found = str_dup( argument );
		send_to_char( "Done.\n\r", ch );
		return;
    }
    if ( !str_cmp( arg2, "vfound" ) )
    {
		if ( xsocial->vict_found )
			DISPOSE( xsocial->vict_found );
		if ( argument[0] != '\0' && str_cmp( argument, "clear" ) )
			xsocial->vict_found = str_dup( argument );
		send_to_char( "Done.\n\r", ch );
		return;
    }
    if ( !str_cmp( arg2, "cauto" ) )
    {
		if ( xsocial->char_auto )
			DISPOSE( xsocial->char_auto );
		if ( argument[0] != '\0' && str_cmp( argument, "clear" ) )
			xsocial->char_auto = str_dup( argument );
		send_to_char( "Done.\n\r", ch );
		return;
    }
    if ( !str_cmp( arg2, "oauto" ) )
    {
		if ( xsocial->others_auto )
			DISPOSE( xsocial->others_auto );
		if ( argument[0] != '\0' && str_cmp( argument, "clear" ) )
			xsocial->others_auto = str_dup( argument );
		send_to_char( "Done.\n\r", ch );
		return;
    }
    if ( get_trust(ch) > LEVEL_GREATER && !str_cmp( arg2, "name" ) )
    {
		bool relocate;

		one_argument( argument, arg1 );
		if ( arg1[0] == '\0' )
		{
			send_to_char( "Cannot clear name field!\n\r", ch );
			return;
		}
		if ( arg1[0] != xsocial->name[0] )
		{
			unlink_xsocial( xsocial );
			relocate = TRUE;
		}
		else
			relocate = FALSE;
		if ( xsocial->name )
			DISPOSE( xsocial->name );
		xsocial->name = str_dup( arg1 );
		if ( relocate )
			add_xsocial( xsocial );
		send_to_char( "Done.\n\r", ch );
		return;
    }

	if ( !str_cmp( arg2, "genderon" ) )
    {
		int value;
		value = is_number( argument ) ? atoi( argument ) : -1;
		if ( value < 0 || value > 3 )
		{
			send_to_char( "Sex range is 0 to 2 (or 3 for male and female).\n\r", ch );
			return;
		}
		xsocial->genderto = value;
		send_to_char( "Done.\n\r", ch );
		return;
    }
	if ( !str_cmp( arg2, "genderuse" ) )
    {
		int value;
		value = is_number( argument ) ? atoi( argument ) : -1;
		if ( value < 0 || value > 2 )
		{
			send_to_char( "Sex range is 0 to 2.\n\r", ch );
			return;
		}
		xsocial->gender = value;
		send_to_char( "Done.\n\r", ch );
		return;
    }
	if ( !str_cmp( arg2, "stage" ) )
    {
		int value;
		value = is_number( argument ) ? atoi( argument ) : -1;
		/*if ( value < 0 || value > 2 )
		{
			send_to_char( "Stage range is 0 to 2.\n\r", ch );
			return;
		}*/
		if ( value == -1 )
		{
			send_to_char( "Must be a valid number.\n\r", ch );
			return;
		}
		xsocial->stage = value;
		send_to_char( "Done.\n\r", ch );
		return;
    }

	if ( !str_cmp( arg2, "position" ) )
    {
		int value;
		value = is_number( argument ) ? atoi( argument ) : -1;
		/*if ( value < 0 || value > 2 )
		{
			send_to_char( "Position range is 0 to 2.\n\r", ch );
			return;
		}*/
		if ( value == -1 )
		{
			send_to_char( "Must be a valid number.\n\r", ch );
			return;
		}
		xsocial->position = value;
		send_to_char( "Done.\n\r", ch );
		return;
    }
	if ( !str_cmp( arg2, "self" ) )
    {
		int value;
		value = is_number( argument ) ? atoi( argument ) : -1;
		if ( value == -1 )
		{
			send_to_char( "Must be a valid number.\n\r", ch );
			return;
		}
		xsocial->self = value;
		send_to_char( "Done.\n\r", ch );
		return;
    }
	if ( !str_cmp( arg2, "other" ) )
    {
		int value;
		value = is_number( argument ) ? atoi( argument ) : -1;
		if ( value == -1 )
		{
			send_to_char( "Must be a valid number.\n\r", ch );
			return;
		}
		xsocial->other = value;
		send_to_char( "Done.\n\r", ch );
		return;
    }
	if ( !str_cmp( arg2, "chance" ) )
    {
		if ( !str_cmp( "true", argument ) )
			xsocial->chance = TRUE;
		else if ( !str_cmp( "false", argument ) )
			xsocial->chance = FALSE;
		else
		{
			send_to_char( "Must be true or false.\n\r",ch);
			return;
		}
		send_to_char( "Done.\n\r", ch );
		return;
    }

    /* display usage message */
    do_xsedit( ch, "" );
}
