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
*			     Special clan module			    *
****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
/* #include <stdlib.h> */
#include <time.h>
#include "mud.h"

#define MAX_NEST	100
static	OBJ_DATA *	rgObjNest	[MAX_NEST];

CLAN_DATA * first_clan;
CLAN_DATA * last_clan;

PLANET_DATA * first_planet;
PLANET_DATA * last_planet;

GUARD_DATA * first_guard;
GUARD_DATA * last_guard;

/* local routines */
void	fread_clan	args( ( CLAN_DATA *clan, FILE *fp ) );
bool	load_clan_file	args( ( char *clanfile ) );
void	write_clan_list	args( ( void ) );
void	fread_planet	args( ( PLANET_DATA *planet, FILE *fp ) );
bool	load_planet_file	args( ( char *planetfile ) );
void	write_planet_list	args( ( void ) );

void do_carry( CHAR_DATA *ch, char *argument );
void clear_carry (CHAR_DATA *ch);


/*
 * Get pointer to clan structure from clan name.
 */
CLAN_DATA *get_clan( char *name )
{
    CLAN_DATA *clan;
    
    for ( clan = first_clan; clan; clan = clan->next )
       if ( !str_cmp( name, clan->name ) )
         return clan;
    
    for ( clan = first_clan; clan; clan = clan->next )
       if ( nifty_is_name( name, clan->name ) )
         return clan;

    for ( clan = first_clan; clan; clan = clan->next )
       if ( !str_prefix( name, clan->name ) )
         return clan;
    
    for ( clan = first_clan; clan; clan = clan->next )
       if ( nifty_is_name_prefix( name, clan->name ) )
         return clan;
    
    return NULL;
}

PLANET_DATA *get_planet( char *name )
{
	PLANET_DATA *planet;
    
    for ( planet = first_planet; planet; planet = planet->next )
       if ( !str_cmp( name, planet->name ) )
         return planet;
    
    for ( planet = first_planet; planet; planet = planet->next )
       if ( nifty_is_name( name, planet->name ) )
         return planet;

    for ( planet = first_planet; planet; planet = planet->next )
       if ( !str_prefix( name, planet->name ) )
         return planet;
    
    for ( planet = first_planet; planet; planet = planet->next )
       if ( nifty_is_name_prefix( name, planet->name ) )
         return planet;
    
    return NULL;
}

void write_clan_list( )
{
    CLAN_DATA *tclan;
    FILE *fpout;
    char filename[256];

    sprintf( filename, "%s%s", CLAN_DIR, CLAN_LIST );
    fpout = fopen( filename, "w" );
    if ( !fpout )
    {
	bug( "FATAL: cannot open clan.lst for writing!\n\r", 0 );
 	return;
    }	  
    for ( tclan = first_clan; tclan; tclan = tclan->next )
	fprintf( fpout, "%s\n", tclan->filename );
    fprintf( fpout, "$\n" );
    fclose( fpout );
}

void write_planet_list( )
{
    PLANET_DATA *tplanet;
    FILE *fpout;
    char filename[256];

    sprintf( filename, "%s%s", PLANET_DIR, PLANET_LIST );
    fpout = fopen( filename, "w" );
    if ( !fpout )
    {
	bug( "FATAL: cannot open planet.lst for writing!\n\r", 0 );
 	return;
    }	  
    for ( tplanet = first_planet; tplanet; tplanet = tplanet->next )
	fprintf( fpout, "%s\n", tplanet->filename );
    fprintf( fpout, "$\n" );
    fclose( fpout );
}

/*
 * Save a clan's data to its data file
 */
void save_clan( CLAN_DATA *clan )
{
    FILE *fp;
    char filename[256];
    char buf[MAX_STRING_LENGTH];
	int rank;

    if ( !clan )
    {
	bug( "save_clan: null clan pointer!", 0 );
	return;
    }
        
    if ( !clan->filename || clan->filename[0] == '\0' )
    {
	sprintf( buf, "save_clan: %s has no filename", clan->name );
	bug( buf, 0 );
	return;
    }
 
    sprintf( filename, "%s%s", CLAN_DIR, clan->filename );
    
    fclose( fpReserve );
    if ( ( fp = fopen( filename, "w" ) ) == NULL )
    {
    	bug( "save_clan: fopen", 0 );
    	perror( filename );
    }
    else
    {
	fprintf( fp, "#CLAN\n" );
	if ( clan->name)
		fprintf( fp, "Name         %s~\n",	clan->name		);
	if ( clan->filename )
		fprintf( fp, "Filename     %s~\n",	clan->filename		);
	if ( clan->description )
		fprintf( fp, "Description  %s~\n",	clan->description	);
	fprintf( fp, "Leader       %s~\n",	clan->leader		);
	fprintf( fp, "NumberOne    %s~\n",	clan->number1		);
	fprintf( fp, "NumberTwo    %s~\n",	clan->number2		);
	fprintf( fp, "Type         %d\n",	clan->clan_type		);
	fprintf( fp, "Members      %d\n",	clan->members		);
	fprintf( fp, "Memberslist %s~\n",	clan->memberslist	);
	fprintf( fp, "Storeroom    %d\n",	clan->storeroom		);
	fprintf( fp, "Funds        %ld\n",	clan->funds		);
	fprintf( fp, "Jail         %d\n",       clan->jail            	);
	fprintf( fp, "Enlist         %d\n", clan->enlistroom );
	fprintf( fp, "Atwar        %s~\n",	clan->atwar		);
	for( rank = 0 ; rank <= ( MAX_RANK - 1 ) ; rank++ )
		fprintf( fp, "Rank         %d %d %s~\n", rank,
		clan->ranks[rank].wage, clan->ranks[rank].name );
	fprintf( fp, "End\n\n"						);
	fprintf( fp, "#END\n"						);
    }
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void save_planet( PLANET_DATA *planet )
{
    FILE *fp;
    char filename[256];
    char buf[MAX_STRING_LENGTH];

    if ( !planet )
    {
	bug( "save_planet: null planet pointer!", 0 );
	return;
    }
        
    if ( !planet->filename || planet->filename[0] == '\0' )
    {
	sprintf( buf, "save_planet: %s has no filename", planet->name );
	bug( buf, 0 );
	return;
    }
 
    sprintf( filename, "%s%s", PLANET_DIR, planet->filename );
    
    fclose( fpReserve );
    if ( ( fp = fopen( filename, "w" ) ) == NULL )
    {
    	bug( "save_planet: fopen", 0 );
    	perror( filename );
    }
    else
    {
        AREA_DATA *pArea;
        
	fprintf( fp, "#PLANET\n" );
	if ( planet->name)
		fprintf( fp, "Name         %s~\n",	planet->name		);
	if ( planet->filename )
		fprintf( fp, "Filename     %s~\n",	planet->filename        );
	fprintf( fp, "BaseValue    %ld\n",	planet->base_value      );
	fprintf( fp, "Flags        %d\n",	planet->flags           );
	fprintf( fp, "PopSupport   %f\n",	planet->pop_support      );
	fprintf( fp, "X            %d\n",	planet->x                );
	fprintf( fp, "Y            %d\n",	planet->y                );
	fprintf( fp, "Z            %d\n",	planet->z                );
	fprintf( fp, "Code         %d\n",   planet->code			 );
	if ( planet->controls )
		fprintf( fp, "Control      %d\n",	planet->controls);
	if ( planet->shields )
		fprintf( fp, "Shield       %d\n",	planet->shields);
	if ( planet->barracks )
		fprintf( fp, "Barracks     %d\n",	planet->barracks);

	if ( planet->starsystem && planet->starsystem->name )
        	fprintf( fp, "Starsystem   %s~\n",	planet->starsystem->name);
	if ( planet->governed_by && planet->governed_by->name )
        	fprintf( fp, "GovernedBy   %s~\n",	planet->governed_by->name);
	for( pArea = planet->first_area ; pArea ; pArea = pArea->next_on_planet )
	    if (pArea->filename)
         	fprintf( fp, "Area         %s~\n",	pArea->filename  );
	fprintf( fp, "End\n\n"						);
	fprintf( fp, "#END\n"						);
    }
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


/*
 * Read in actual clan data.
 */

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

void fread_clan( CLAN_DATA *clan, FILE *fp )
{
    char buf[MAX_STRING_LENGTH], *word, *tmp;
	char arg[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    bool fMatch;
	int place, loop;

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
	    KEY( "Atwar",	clan->atwar,	fread_string( fp ) );
		break;

	case 'D':
	    KEY( "Description",	clan->description,	fread_string( fp ) );
	    break;

	case 'E':
		KEY( "Enlist",	clan->enlistroom,		fread_number( fp ) );
		if ( !str_cmp( word, "End" ) )
	    {
			if (!clan->name)
				clan->name		= STRALLOC( "" );
			if (!clan->leader)
				clan->leader		= STRALLOC( "" );
			if (!clan->description)
				clan->description 	= STRALLOC( "" );
			if (!clan->number1)
				clan->number1		= STRALLOC( "" );
			if (!clan->number2)
				clan->number2		= STRALLOC( "" );
			if (!clan->memberslist)
				clan->memberslist	= STRALLOC( "" );
			if (!clan->atwar)
				clan->atwar		= STRALLOC( "" );
				for( loop = 0 ; loop >= ( MAX_RANK - 1 ) ; loop++ )
					if( !clan->ranks[loop].name )
						clan->ranks[loop].name = STRALLOC( "" );

				return;
	    	}

	    break;
	    
	case 'F':
	    KEY( "Funds",	clan->funds,		fread_number( fp ) );
	    KEY( "Filename",	clan->filename,		fread_string_nohash( fp ) );
	    break;

	case 'J':
	    KEY( "Jail",	clan->jail,		fread_number( fp ) );
	    break;

	case 'L':
	    KEY( "Leader",	clan->leader,		fread_string( fp ) );
	    break;

	case 'M':
	    KEY( "Members",	clan->members,		fread_number( fp ) );
		KEY( "Memberslist",	clan->memberslist,		fread_string( fp ) );
	    break;
 
	case 'N':
	    KEY( "Name",	clan->name,		fread_string( fp ) );
	    KEY( "NumberOne",	clan->number1,		fread_string( fp ) );
	    KEY( "NumberTwo",	clan->number2,		fread_string( fp ) );
	    break;

	case 'R':
		if( !str_cmp( word, "Rank" ) )
		{
			tmp = fread_string( fp );
			tmp = one_argument( tmp, arg );
			place = atoi( arg );
			clan->ranks[place].place = place;
			tmp = one_argument( tmp, arg2 );
			clan->ranks[place].wage = atoi( arg2 );
			clan->ranks[place].name = STRALLOC( tmp );
			fMatch = TRUE;
			break;
		}
		break;

	case 'S':
	    KEY( "Storeroom",	clan->storeroom,	fread_number( fp ) );
	    break;

	case 'T':
	    KEY( "Type",	clan->clan_type,	fread_number( fp ) );
	    break;
	}
	
	if ( !fMatch )
	{
	    sprintf( buf, "Fread_clan: no match: %s", word );
	    bug( buf, 0 );
	}
	
    }
}

void fread_planet( PLANET_DATA *planet, FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;

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
	    if ( !str_cmp( word, "Area" ) )
	    {
	        char aName[MAX_STRING_LENGTH];
                AREA_DATA *pArea;
                	        
	     	sprintf (aName, fread_string(fp));
		for( pArea = first_area ; pArea ; pArea = pArea->next )
	          if (pArea->filename && !str_cmp(pArea->filename , aName ) )
	          {
	             pArea->planet = planet; 
	             LINK( pArea, planet->first_area, planet->last_area, next_on_planet, prev_on_planet);
	          }      
                fMatch = TRUE;
	    }
	    break;

	case 'B':
		KEY( "Barracks", planet->barracks, fread_number( fp ) );
	    KEY( "BaseValue",	planet->base_value,		fread_number( fp ) );
	    break;

	case 'C':
		KEY( "Code", planet->code, fread_number( fp ) );
		KEY( "Control", planet->controls, fread_number( fp ) );
		break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
			if (!planet->name)
				planet->name		= STRALLOC( "un-named planet" );
			if ( !planet->controls )
				planet->controls = 0;
			if ( !planet->shields )
				planet->shields = 0;
			if ( planet->barracks )
				planet->barracks = 0;
			fMatch = TRUE;
			return;
	    }
	    break;

	case 'F':
	    KEY( "Filename",	planet->filename,		fread_string_nohash( fp ) );
	    KEY( "Flags",	planet->flags,		        fread_number( fp ) );
	    break;
	
	case 'G':
	    if ( !str_cmp( word, "GovernedBy" ) )
	    {
	     	planet->governed_by = get_clan ( fread_string(fp) );
                fMatch = TRUE;
	    }
	    break;
	
	case 'N':
	    KEY( "Name",	planet->name,		fread_string( fp ) );
	    break;
	
	case 'P':
	    KEY( "PopSupport",	planet->pop_support,		fread_number( fp ) );
	    break;

	case 'S':
		KEY( "Shield", planet->shields, fread_number( fp ));
	    if ( !str_cmp( word, "Starsystem" ) )
	    {
	     	planet->starsystem = starsystem_from_name ( fread_string(fp) );
                if (planet->starsystem)
                {
                     SPACE_DATA *starsystem = planet->starsystem;
                     
                     LINK( planet , starsystem->first_planet, starsystem->last_planet, next_in_system, prev_in_system );
                }
                fMatch = TRUE;
	    }
	    break;
	
	case 'T':
	    KEY( "Taxes",	planet->base_value,		fread_number( fp ) );
	    break;

	case 'X':
		KEY( "X", planet->x,			fread_number(fp) );
		break;
		
	case 'Y':
		KEY( "Y", planet->y,			fread_number(fp) );
		break;
	
	case 'Z':
		KEY( "Z", planet->z,			fread_number(fp) );
		break;
    
	}
	
	if ( !fMatch )
	{
	    sprintf( buf, "Fread_planet: no match: %s", word );
	    bug( buf, 0 );
	}
	
    }
}


/*
 * Load a clan file
 */

bool load_clan_file( char *clanfile )
{
    char filename[256];
    CLAN_DATA *clan;
    FILE *fp;
    bool found;

    CREATE( clan, CLAN_DATA, 1 );
    
    found = FALSE;
    sprintf( filename, "%s%s", CLAN_DIR, clanfile );

    if ( ( fp = fopen( filename, "r" ) ) != NULL )
    {

	found = TRUE;
	for ( ; ; )
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
		bug( "Load_clan_file: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if ( !str_cmp( word, "CLAN"	) )
	    {
	    	fread_clan( clan, fp );
	    	break;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
		char buf[MAX_STRING_LENGTH];

		sprintf( buf, "Load_clan_file: bad section: %s.", word );
		bug( buf, 0 );
		break;
	    }
	}
	fclose( fp );
    }

    if ( found )
    {
		ROOM_INDEX_DATA *storeroom;

		LINK( clan, first_clan, last_clan, next, prev );

		if ( clan->storeroom == 0
			|| (storeroom = get_room_index( clan->storeroom )) == NULL )
		{
			log_string( "Storeroom not found" );
			return found;
		}
		
		sprintf( filename, "%s%s.vault", CLAN_DIR, clan->filename );
		if ( ( fp = fopen( filename, "r" ) ) != NULL )
		{
			int iNest;
			bool found;
			OBJ_DATA *tobj, *tobj_next;

			log_string( "Loading clan storage room" );
			rset_supermob(storeroom);
			for ( iNest = 0; iNest < MAX_NEST; iNest++ )
				rgObjNest[iNest] = NULL;

			found = TRUE;
			for ( ; ; )
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
					bug( "Load_clan_vault: # not found.", 0 );
					bug( clan->name, 0 );
					break;
				}
				word = fread_word( fp );
				if ( !str_cmp( word, "OBJECT" ) )	/* Objects	*/
					fread_obj  ( supermob, fp, OS_CARRY );
				else if ( !str_cmp( word, "END"    ) )	/* Done		*/
					break;
				else
				{
					bug( "Load_clan_vault: bad section.", 0 );
					bug( clan->name, 0 );
					break;
				}
			}
			fclose( fp );
			for ( tobj = supermob->first_carrying; tobj; tobj = tobj_next )
			{
				tobj_next = tobj->next_content;
				obj_from_char( tobj );
				obj_to_room( tobj, storeroom );
			}
			release_supermob();
		}
		else
			log_string( "Cannot open clan vault" );
    }
    else
		DISPOSE( clan );

    return found;
}

bool load_planet_file( char *planetfile )
{
    char filename[256];
    PLANET_DATA *planet;
    FILE *fp;
    bool found;

    CREATE( planet, PLANET_DATA, 1 );
    
    planet->governed_by = NULL;
    planet->next_in_system = NULL;
    planet->prev_in_system = NULL;
    planet->starsystem = NULL ;
    planet->first_area = NULL;
    planet->last_area = NULL;
    planet->first_guard = NULL;
    planet->last_guard = NULL;
    
    found = FALSE;
    sprintf( filename, "%s%s", PLANET_DIR, planetfile );

    if ( ( fp = fopen( filename, "r" ) ) != NULL )
    {

	found = TRUE;
	for ( ; ; )
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
		bug( "Load_planet_file: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if ( !str_cmp( word, "PLANET"	) )
	    {
	    	fread_planet( planet, fp );
	    	break;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
		char buf[MAX_STRING_LENGTH];

		sprintf( buf, "Load_planet_file: bad section: %s.", word );
		bug( buf, 0 );
		break;
	    }
	}
	fclose( fp );
    }

    if ( !found )
      DISPOSE( planet );
    else
      LINK( planet, first_planet, last_planet, next, prev );

    return found;
}


/*
 * Load in all the clan files.
 */
void load_clans( )
{
    FILE *fpList;
    char *filename;
    char clanlist[256];
    char buf[MAX_STRING_LENGTH];
    
    first_clan	= NULL;
    last_clan	= NULL;

    log_string( "Loading clans..." );

    sprintf( clanlist, "%s%s", CLAN_DIR, CLAN_LIST );
    fclose( fpReserve );
    if ( ( fpList = fopen( clanlist, "r" ) ) == NULL )
    {
	perror( clanlist );
	exit( 1 );
    }

    for ( ; ; )
    {
	filename = feof( fpList ) ? "$" : fread_word( fpList );
	log_string( filename );
	if ( filename[0] == '$' )
	  break;

	if ( !load_clan_file( filename ) )
	{
	  sprintf( buf, "Cannot load clan file: %s", filename );
	  bug( buf, 0 );
	}
    }
    fclose( fpList );
    log_string(" Done clans\n\rSorting clans...." );
    fpReserve = fopen( NULL_FILE, "r" );
    
    log_string(" Done sorting" );
    return;
}

void load_planets( )
{
    FILE *fpList;
    char *filename;
    char planetlist[256];
    char buf[MAX_STRING_LENGTH];
    
    first_planet	= NULL;
    last_planet	= NULL;

    log_string( "Loading planets..." );

    sprintf( planetlist, "%s%s", PLANET_DIR, PLANET_LIST );
    fclose( fpReserve );
    if ( ( fpList = fopen( planetlist, "r" ) ) == NULL )
    {
	perror( planetlist );
	exit( 1 );
    }

    for ( ; ; )
    {
	filename = feof( fpList ) ? "$" : fread_word( fpList );
	log_string( filename );
	if ( filename[0] == '$' )
	  break;

	if ( !load_planet_file( filename ) )
	{
	  sprintf( buf, "Cannot load planet file: %s", filename );
	  bug( buf, 0 );
	}
    }
    fclose( fpList );
    log_string(" Done planets " );  
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void do_make( CHAR_DATA *ch, char *argument )
{
	send_to_char( "Huh?\n\r", ch );
	return;
}

void do_induct( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CLAN_DATA *clan;

    if ( IS_NPC( ch ) || !ch->pcdata->clan )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    clan = ch->pcdata->clan;
    
    if ( (ch->pcdata && ch->pcdata->bestowments
    &&    is_name("induct", ch->pcdata->bestowments))
    ||   !str_cmp( ch->name, clan->leader  )
    ||   !str_cmp( ch->name, clan->number1 )
    ||   !str_cmp( ch->name, clan->number2 ) )
	;
    else
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Induct whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if( IS_NPC(victim) )
	{
		do_say(victim,"Why would *I* want to join your stinking clan?");
		do_say(victim,"Whats in it for me?");
		return;
	}

    if ( victim->pcdata->clan )
    {
      if ( victim->pcdata->clan->clan_type == CLAN_CRIME )
      {
	if ( victim->pcdata->clan == clan )
	  send_to_char( "This player already belongs to your crime family!\n\r", ch );
	else
	  send_to_char( "This player already belongs to an organization!\n\r", ch );
	return;
      }
      else if ( victim->pcdata->clan->clan_type == CLAN_GUILD )
      {
	if ( victim->pcdata->clan == clan )
	  send_to_char( "This player already belongs to your guild!\n\r", ch );
	else
	  send_to_char( "This player already belongs to an organization!\n\r", ch );
	return;
      }
      else
      {
	if ( victim->pcdata->clan == clan )
	  send_to_char( "This player already belongs to your organization!\n\r", ch );
	else
	  send_to_char( "This player already belongs to an organization!\n\r", ch );
	return;
      }
      
    }

	sprintf( buf, "%s %s", clan->memberslist, victim->name );
	STRFREE( clan->memberslist );
	clan->memberslist = STRALLOC( buf );
    
	clan->members++;
	victim->pcdata->clan_rank = 0;
    victim->pcdata->clan = clan;
    STRFREE(victim->pcdata->clan_name);
    victim->pcdata->clan_name = QUICKLINK( clan->name );
    act( AT_MAGIC, "You induct $N into $t", ch, clan->name, victim, TO_CHAR );
    act( AT_MAGIC, "$n inducts $N into $t", ch, clan->name, victim, TO_NOTVICT );
    act( AT_MAGIC, "$n inducts you into $t", ch, clan->name, victim, TO_VICT );
    save_char_obj( victim );
    return;
}

void do_outcast( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CLAN_DATA *clan;
    char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;

    if ( IS_NPC( ch ) || !ch->pcdata->clan )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    clan = ch->pcdata->clan;

    if ( (ch->pcdata && ch->pcdata->bestowments
    &&    is_name("outcast", ch->pcdata->bestowments))
    ||   !str_cmp( ch->name, clan->leader  )
    ||   !str_cmp( ch->name, clan->number1 )
    ||   !str_cmp( ch->name, clan->number2 ) )
	;
    else
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }


    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Outcast whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	    send_to_char( "Kick yourself out of your own clan?\n\r", ch );
	    return;
    }
 
    if ( victim->pcdata->clan != ch->pcdata->clan )
    {
	    send_to_char( "This player does not belong to your clan!\n\r", ch );
	    return;
    }


    if ( victim->speaking & LANG_CLAN )
        victim->speaking = LANG_COMMON;
    REMOVE_BIT( victim->speaks, LANG_CLAN );
    --clan->members;
    if ( !str_cmp( victim->name, ch->pcdata->clan->number1 ) )
    {
	STRFREE( ch->pcdata->clan->number1 );
	ch->pcdata->clan->number1 = STRALLOC( "" );
    }
    if ( !str_cmp( victim->name, ch->pcdata->clan->number2 ) )
    {
	STRFREE( ch->pcdata->clan->number2 );
	ch->pcdata->clan->number2 = STRALLOC( "" );
    }
    victim->pcdata->clan = NULL;
    STRFREE(victim->pcdata->clan_name);
    victim->pcdata->clan_name = STRALLOC( "" );
    act( AT_MAGIC, "You outcast $N from $t", ch, clan->name, victim, TO_CHAR );
    act( AT_MAGIC, "$n outcasts $N from $t", ch, clan->name, victim, TO_ROOM );
    act( AT_MAGIC, "$n outcasts you from $t", ch, clan->name, victim, TO_VICT );
	for( d = first_descriptor; d; d = d->next )
	{
		CHAR_DATA *vch;
		vch = d->character;
		if( d->connected == CON_PLAYING && vch != victim && !IS_NPC(vch) )
		{
			if( !vch->pcdata->clan ) continue;
			if( (vch->pcdata->clan == clan) )
				ch_printf( vch, "&BYou get word that &W%s&B has been outcast from the clan.&w\n\r", victim->name );
		}
	}
	if ( clan->memberslist[0] != '\0' )
	{
		char arg[MAX_STRING_LENGTH];
		char memberslist[MAX_STRING_LENGTH];
		char * members;
		members = str_alloc(clan->memberslist);
		strcpy(memberslist,"");

		while ( members[0] != '\0' )
		{
			members = one_argument(members,arg);
			if (str_cmp(victim->name,arg) )
			{
				arg[0] = UPPER(arg[0]);
				sprintf(buf, "%s %s",memberslist, arg);
				sprintf(memberslist,"%s",buf);
			}
		}
		STRFREE( clan->memberslist );
		clan->memberslist = STRALLOC( memberslist );
	}

    DISPOSE( victim->pcdata->bestowments );
    victim->pcdata->bestowments = str_dup("");
	victim->pcdata->clan_rank = 0;
	
	save_clan( clan );    
    save_char_obj( victim );	/* clan gets saved when pfile is saved */
    return;
}

void do_setclan( CHAR_DATA *ch, char *argument )
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], arg3[MAX_INPUT_LENGTH];
    CLAN_DATA *clan;
	int rank;

    if ( IS_NPC( ch ) )
    {
		send_to_char( "Huh?\n\r", ch );
		return;
	}

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

	set_char_color( AT_WHITE, ch);
    if ( arg1[0] == '\0' )
	{
		send_to_char( "Usage: setclan <clan> <field> <leader|number1|number2> <player>\n\r", ch );
		send_to_char( "\n\rField being one of:\n\r", ch );
		send_to_char( " leader number1 number2 enlistroom\n\r", ch );
		send_to_char( " members storage funds\n\r", ch );
		send_to_char( " rank wage                  \n\r", ch );
		if ( get_trust( ch ) >= LEVEL_SUB_IMPLEM )
			send_to_char( " name filename desc\n\r", ch );
		if ( get_trust(ch) == MAX_LEVEL )
			send_to_char( " memberslist\n\r", ch);
		return;
	}
	
	clan = get_clan( arg1 );
	if ( !clan )
	{
		send_to_char( "No such clan.\n\r", ch );
		return;
	}

	if( !strcmp( arg2, "wage" ) )
	{
		int wage;
		argument = one_argument( argument, arg3 );

		if( arg3[0] == '\0' )
		{
			send_to_char( "syntax: setclan <clan> wage <rank number> <wage>", ch );
			return;
		}
		wage = atoi(argument);

		if ( wage >= 10000 )
		{
			send_to_char( "wages must be less then 10000", ch);
			return;
		}
		rank = atoi( arg3 );
		clan->ranks[rank].wage = wage;
		send_to_char( "Done.\n\r", ch );
		save_clan( clan );
		return;
	}
	if( !strcmp( arg2, "rank" ) )
	{
		argument = one_argument( argument, arg3 );

		if( arg3[0] == '\0' )
		{
			send_to_char( "syntax: setclan <clan> rank <rank number> <name>", ch );
			return;
		}

		rank = atoi( arg3 );

		if ( rank >= MAX_RANK )
			return;

		if( clan->ranks[rank].name );
			STRFREE( clan->ranks[rank].name );

		clan->ranks[rank].name = STRALLOC( argument );
		send_to_char( "Done.\n\r", ch );
		save_clan( clan );
		return;
	}
	
	if ( !strcmp( arg2, "leader" ) )
	{
		STRFREE( clan->leader );
		clan->leader = STRALLOC( argument );
		send_to_char( "Done.\n\r", ch );
		save_clan( clan );
		return;
	}
	if ( !strcmp( arg2, "enlistroom" ) )
	{
		clan->enlistroom = atoi( argument );
		send_to_char( "Done.\n\r", ch );
		save_clan( clan );
		return;
	}
	if ( !strcmp( arg2, "number1" ) )
	{
		STRFREE( clan->number1 );
		clan->number1 = STRALLOC( argument );
		send_to_char( "Done.\n\r", ch );
		save_clan( clan );
		return;
	}
	if ( !strcmp( arg2, "number2" ) )
	{
		STRFREE( clan->number2 );
		clan->number2 = STRALLOC( argument );
		send_to_char( "Done.\n\r", ch );
		save_clan( clan );
		return;
	}
	if ( !strcmp( arg2, "members" ) )
	{
		clan->members = atoi( argument );
		send_to_char( "Done.\n\r", ch );
		save_clan( clan );
		return;
	}
	if ( !strcmp( arg2, "funds" ) )
	{
		clan->funds = atoi( argument );
		send_to_char( "Done.\n\r", ch );
		save_clan( clan );
		return;
	}
	if ( !strcmp( arg2, "storage" ) )
	{
		clan->storeroom = atoi( argument );
		send_to_char( "Done.\n\r", ch );
		save_clan( clan );
		return;
	}
	if ( !strcmp( arg2, "jail" ) )
	{
		clan->jail = atoi( argument );
		send_to_char( "Done.\n\r", ch );
		save_clan( clan );
		return;
	}
	if ( get_trust( ch ) < LEVEL_SUB_IMPLEM )
	{
		do_setclan( ch, "" );
		return;
	}
    
	if ( !strcmp( arg2, "type" ) )
	{
		if ( !str_cmp( argument, "crime" ) )
			clan->clan_type = CLAN_CRIME;
		else if ( !str_cmp( argument, "crime family" ) )
			clan->clan_type = CLAN_CRIME;
		else if ( !str_cmp( argument, "guild" ) )
			clan->clan_type = CLAN_GUILD;
		else
			clan->clan_type = 0;
		send_to_char( "Done.\n\r", ch );
		save_clan( clan );
		return;
	}
	
	if ( !strcmp( arg2, "name" ) )
	{
		if (strlen(argument) > 25 )
		{
			send_to_char("Too Long, Must Be Under 18 Characters Long\n\r",ch);
			return;
		}
		STRFREE( clan->name );
		clan->name = STRALLOC( argument );
		send_to_char( "Done.\n\r", ch );
		save_clan( clan );
		return;
	}
	if ( !strcmp( arg2, "filename" ) )
	{
		if ( clan->filename && clan->filename[0] != '\0' )
			DISPOSE( clan->filename );
		clan->filename = str_dup( argument );
		send_to_char( "Done.\n\r", ch );
		save_clan( clan );
		write_clan_list( );
		return;
	}
	if ( !str_cmp( arg2, "memberslist") )
	{
		STRFREE( clan->memberslist );
		clan->memberslist = STRALLOC( argument );
		send_to_char( "Done.\n\r",ch);
		save_clan( clan );
	}
	
	if ( !strcmp( arg2, "desc" ) )
	{
		STRFREE( clan->description );
		clan->description = STRALLOC( argument );
		send_to_char( "Done.\n\r", ch );
		save_clan( clan );
		return;
	}
	
	do_setclan( ch, "" );
    return;
}

void do_setplanet( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    PLANET_DATA *planet;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Usage: setplanet <planet> <field> [value]\n\r", ch );
	send_to_char( "\n\rField being one of:\n\r", ch );
	send_to_char( " base_value flags\n\r", ch ); 
	send_to_char( " name filename starsystem governed_by\n\r", ch );
	send_to_char( " x y z\n\r", ch );
	return;
    }

    planet = get_planet( arg1 );
    if ( !planet )
    {
	send_to_char( "No such planet.\n\r", ch );
	return;
    }


    if ( !strcmp( arg2, "name" ) )
    {
	STRFREE( planet->name );
	planet->name = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_planet( planet );
	return;
    }

    if ( !strcmp( arg2, "governed_by" ) )
    {
		CLAN_DATA *clan;
        clan = get_clan( argument );
		if ( !strcmp( argument, "none" ) ) {
           planet->governed_by = NULL;
		   send_to_char( "Done.\n\r", ch ); 
       	   save_planet( planet );
		   return;
		}
        if ( clan )
        { 
           planet->governed_by = clan;
           send_to_char( "Done.\n\r", ch ); 
       	   save_planet( planet );
        }
        else
           send_to_char( "No such clan.\n\r", ch ); 
	return;
    }

    if ( !strcmp( arg2, "x" ) )
    {
	planet->x = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_planet( planet );
	return;
    }

	if ( !strcmp( arg2, "y" ) )
    {
	planet->y = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_planet( planet );
	return;
    }

	if ( !strcmp( arg2, "z" ) )
    {
	planet->z = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_planet( planet );
	return;
    }


    if ( !strcmp( arg2, "starsystem" ) )
    {
        SPACE_DATA *starsystem;
        
        if ((starsystem=planet->starsystem) != NULL)
          UNLINK(planet, starsystem->first_planet, starsystem->last_planet, next_in_system, prev_in_system);
	if ( (planet->starsystem = starsystem_from_name(argument)) )
        {
           starsystem = planet->starsystem;
           LINK(planet, starsystem->first_planet, starsystem->last_planet, next_in_system, prev_in_system);	
           send_to_char( "Done.\n\r", ch );
	}
	else 
	       	send_to_char( "No such starsystem.\n\r", ch );
	save_planet( planet );
	return;
    }

    if ( !strcmp( arg2, "filename" ) )
    {
	if ( planet->filename && planet->filename[0] != '\0' )
		DISPOSE( planet->filename );
	planet->filename = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	save_planet( planet );
	write_planet_list( );
	return;
    }

    if ( !strcmp( arg2, "base_value" ) )
    {
	planet->base_value = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_planet( planet );
	return;
    }

    if ( !strcmp( arg2, "flags" ) )
    {
        char farg[MAX_INPUT_LENGTH];
        
        argument = one_argument( argument, farg); 
        
        if ( farg[0] == '\0' )
        {
           send_to_char( "Possible flags: nocapture neutral uninhabited\n\r", ch );
           return;
        }
        
        for ( ; farg[0] != '\0' ; argument = one_argument( argument, farg) )
        {
            if ( !str_cmp( farg, "nocapture" ) )
               TOGGLE_BIT( planet->flags, PLANET_NOCAPTURE);
			else if ( !str_cmp( farg, "neutral" ) )
			{	
				TOGGLE_BIT( planet->flags, PLANET_NEUTRAL );
				planet->governed_by = NULL; 
			}
			else if ( !str_cmp( farg, "uninhabited" ) )
			{	
				TOGGLE_BIT( planet->flags, PLANET_UNINHABITED );
				planet->governed_by = NULL; 
			}
			else if ( !str_cmp( farg, "uninhabited" ) )
			{	TOGGLE_BIT( planet->flags, PLANET_UNINHABITED );
				planet->governed_by = NULL; 
			}
			else if ( !str_cmp( farg, "shield" ) )
			{
				TOGGLE_BIT( planet->flags,PLANET_PSHIELD );
			}
            else
               ch_printf( ch , "No such flag: %s\n\r" , farg );
	}
	send_to_char( "Done.\n\r", ch );
	save_planet( planet );
	return;
    }

    do_setplanet( ch, "" );
    return;
}

void do_showclan( CHAR_DATA *ch, char *argument )
{   
    CLAN_DATA *clan;
	int counter;

    if ( IS_NPC( ch ) )
    {
		send_to_char( "Huh?\n\r", ch );
		return;
    }

    if ( argument[0] == '\0' )
    {
		send_to_char( "Usage: showclan <clan>\n\r", ch );
		return;
    }

    clan = get_clan( argument );
    if ( !clan )
    {
		send_to_char( "No such clan.\n\r", ch );
		return;
    }

	ch_printf( ch, "&G&W");
    ch_printf( ch, "%s      : %s\n\rFilename: %s\n\r",
		clan->clan_type == CLAN_CRIME ? "Crime Family " : 
		clan->clan_type == CLAN_GUILD ? "Guild " : "Organization ",
		clan->name,
		clan->filename);
	ch_printf( ch, "&G&W");
	ch_printf( ch, "Description: %s\n\r", clan->description );
	ch_printf( ch, "Leader: %-10.10s Number1: %-10.10s Number2: %-10.10s\n\r",
		clan->leader, clan->number1, clan->number2 );
    ch_printf( ch, "Type: %d\n\r",
		clan->clan_type );
    ch_printf( ch, "Members: %-3.0d Spacecraft: %-3.0d\n\r",
		clan->members, clan->spacecraft );
    ch_printf( ch, "Jail: %5d\n\r", clan->jail);
    ch_printf( ch, "Funds: %-8.0ld Enlist Room: %ld\n\r",
		clan->funds, clan->enlistroom );
	if( IS_IMMORTAL(ch) || ch->pcdata->clan == clan )
	{
		ch_printf( ch, "Members list:\n\r %s\n\r", clan->memberslist );
		ch_printf( ch, "Ranks:\n\r&B------&G&W\n\r");
		for( counter = 0 ; counter <= ( MAX_RANK - 1 ) ; counter++ )
			ch_printf( ch, "&WRank:&G %s  &WWage:&G %d\n\r", 
			clan->ranks[counter].name, clan->ranks[counter].wage );
	}
    return;
}

void do_showplanet( CHAR_DATA *ch, char *argument )
{   
    PLANET_DATA *planet;
	AREA_DATA *area;
	int num_guards = 0;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Usage: showplanet <planet>\n\r", ch );
	return;
    }

    planet = get_planet( argument );
    if ( !planet )
    {
	send_to_char( "No such planet.\n\r", ch );
	return;
    }

    ch_printf( ch, "%s\n\rFilename: %s\n\r",
    			planet->name,
    			planet->filename);
	ch_printf( ch, "Governed By:%s\n\r", planet->governed_by ? planet->governed_by->name : "");
	ch_printf( ch, "Worth: %-10ld/%-10ld\n\r", get_taxes(planet),planet->base_value);
	ch_printf( ch, "Pop Support: %.1f\n\r", planet->pop_support);
	ch_printf( ch, "Population: %d\n\r", planet->population);
	ch_printf( ch, "Starsystem: %s\n\r",planet->starsystem ? planet->starsystem->name : "" );
	ch_printf( ch, "Controls: %d\n\r",planet->controls ? planet->controls : 0 );
	ch_printf( ch, "&WBarracks: &G%d\n\r", planet->barracks );
	ch_printf( ch, "&WPatrols: &G%d&W/%d\n\r", num_guards , planet->barracks*5 );
	ch_printf( ch, "X: %d\tY: %d\tZ: %d\n\r",
		planet->x ? planet->x : 0,
		planet->y ? planet->y : 0,
		planet->z ? planet->z : 0);
    ch_printf( ch, "Areas: ");
	for ( area = planet->first_area ; area ; area = area->next_on_planet )
		ch_printf( ch , "%s,  ", area->filename );
	ch_printf( ch, "\n\r" );


    return;
}

void do_makeclan( CHAR_DATA *ch, char *argument )
{
    char filename[256];
    CLAN_DATA *clan;
    bool found;
	int counter;

    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage: makeclan <clan name>\n\r", ch );
	return;
    }

	if (strlen(argument) > 25 )
	{
		send_to_char("Too Long, Must Be Under 18 Characters Long\n\r",ch);
		return;
	}

    found = FALSE;
    sprintf( filename, "%s%s", CLAN_DIR, strlower(argument) );

    CREATE( clan, CLAN_DATA, 1 );
    LINK( clan, first_clan, last_clan, next, prev );
    clan->name		= STRALLOC( argument );
    clan->description	= STRALLOC( "" );
    clan->leader	= STRALLOC( "" );
	clan->atwar		= STRALLOC( "" );
	clan->memberslist = STRALLOC( "" );
    clan->number1	= STRALLOC( "" );
    clan->number2	= STRALLOC( "" );

	for( counter = 0 ; counter <= ( MAX_RANK - 1 ) ; counter++ )
	{
		clan->ranks[counter].name = STRALLOC( "" );
		clan->ranks[counter].wage = 0;
	}
}

void do_makeplanet( CHAR_DATA *ch, char *argument )
{
    char filename[256];
    PLANET_DATA *planet;
    bool found;

    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage: makeplanet <planet name>\n\r", ch );
	return;
    }

    found = FALSE;
    sprintf( filename, "%s%s", PLANET_DIR, strlower(argument) );

    CREATE( planet, PLANET_DATA, 1 );
    LINK( planet, first_planet, last_planet, next, prev );
    planet->governed_by = NULL;
    planet->next_in_system = NULL;
    planet->prev_in_system = NULL;
    planet->starsystem = NULL ;
    planet->first_area = NULL;
    planet->last_area = NULL;
    planet->first_guard = NULL;
    planet->last_guard = NULL;
    planet->name		= STRALLOC( argument );
    planet->flags               = 0;
}

void do_clans( CHAR_DATA *ch, char *argument )
{
	CLAN_DATA *clan;

	clan = get_clan( argument );
	if ( argument[0] == '\0' )
	{
		PLANET_DATA *planet;
		int count = 0;
		int small_count = 0;
		int pCount = 0;
		int support;
		long revenue;
		
		if ( IS_NPC( ch ) )
		{
			send_to_char( "Huh?\n\r", ch );
			return;
		}
		
		for ( clan = first_clan; clan; clan = clan->next )
		{
			if ( clan->clan_type == CLAN_CRIME || clan->clan_type == CLAN_GUILD )
				continue;
			
			pCount = 0;
			support = 0;
			revenue = 0;
			
			for ( planet = first_planet ; planet ; planet = planet->next )
			{
				if ( clan == planet->governed_by )
				{
					support += planet->pop_support;
					pCount++;
					revenue += get_taxes(planet);
				}
			}
			if ( pCount > 1 )
				support /= pCount;
			ch_printf( ch, "&CC&clan &CN&came&C: &W%-30.30s &CL&ceader&C: &W%-10.10s &CM&cembers&C: &W%-2d\n\r", 
				clan->name,clan->leader,clan->members);
			ch_printf( ch , "\n\r" );
			count++;
		}
		for ( clan = first_clan; clan; clan = clan->next )
		{
			if ( clan->clan_type != CLAN_CRIME && clan->clan_type != CLAN_GUILD )
				continue;
			else
				small_count++;
			if (small_count == 1)
				ch_printf( ch, "&CL&cesser &CK&cnown &CC&clans             &CL&ceaders              &CM&cembers\n\r");
			ch_printf( ch, "&W%-30s &W%-20s &W%-2d\n\r", clan->name, clan->leader, clan->members );
			count++;
		}
		if ( !count )
		{
			set_char_color( AT_BLOOD, ch);
			send_to_char( "There are no organizations currently formed.\n\r", ch );
		}
		set_char_color( AT_WHITE, ch );
		send_to_char( "\n\rSee also: Planets\n\r", ch );
	}
	else if ( clan != NULL )
	{
		PLANET_DATA *planet;
		int pCount = 0;

		send_to_char("&W[&R^zAccessing File...&G^x&G&W]\n\r",ch);
		send_to_char("&W[&R^zBegining Printout&G^x&G&W]\n\r",ch);
		ch_printf( ch, "&W%s      : %s&W\n\r",
			clan->clan_type == CLAN_CRIME ? "Crime Family " : 
			clan->clan_type == CLAN_GUILD ? "Guild " : "Organization ",
			clan->name);
		ch_printf( ch, "Description: %s\n\r", clan->description );
	    ch_printf( ch, "Members: %-3.0d Spacecraft: %-3.0d\n\r",
			clan->members, clan->spacecraft );
		for ( planet = first_planet ; planet ; planet = planet->next )
		{
			if ( clan == planet->governed_by )
			{
				if ( pCount == 0 )
				{
					ch_printf(ch,"| Planets Currently Governed by %-20.20s: |\n\r"
						"+-----------------------------------------------------+\n\r",clan->name);
				}
				pCount++;
				ch_printf(ch, "| Name: %-45.45s |\n\r", planet->name );
			}
		}
		if ( pCount != 0 )
		{
			send_to_char("+-----------------------------------------------------+\n\r",ch);
		}
		else
		{
			send_to_char("+-----------------------------------------------------+\n\r",ch);
			send_to_char("| This clan currently does not govern any planets     |\n\r",ch);
			send_to_char("+-----------------------------------------------------+\n\r",ch);
		}
		send_to_char("&W[&R^zEnd Of File&G^x&G&W]\n\r",ch);
		return;
	}
	else if ( clan == NULL )
	{
		send_to_char("Syntax:\n\r"
			"  clans [by it self will list all the clans]\n\r"
			"  clans (clan name) [ will list info about that clan ]\n\r",ch);
		return;
	}
}

void do_planets( CHAR_DATA *ch, char *argument )
{
    PLANET_DATA *planet;
    int count = 0;
    AREA_DATA   *area;

    set_char_color( AT_WHITE, ch );
    for ( planet = first_planet; planet; planet = planet->next )
    {
		if ( IS_SET(planet->flags, PLANET_NEUTRAL ) )
		{
			ch_printf( ch, "&CP&clanet: &R%-15s   &GN&geutral &GP&glanet\n\r", 
                   planet->name );
		}
		else if ( IS_SET(planet->flags, PLANET_UNINHABITED ) )
		{
			ch_printf( ch, "&WPlanet: &G%-15s   &RUninhabited\n\r", planet->name);
		}
		else
		{
			ch_printf( ch, "&CP&clanet: &R%-15s   &CG&coverned &CB&cy: &G%s %s\n\r", 
				planet->name ,
				planet->governed_by ? planet->governed_by->name : "",
				IS_SET(planet->flags, PLANET_NOCAPTURE ) ? "(permanent)" : "" );
			ch_printf( ch, "&CV&calue: &R%-10ld&C/&R%-10d   ", 
				get_taxes(planet) , planet->base_value);
			ch_printf( ch, "&CP&copulation: &R%-5d   &CP&cop &CS&cupport: &R%.1f\n\r", 
				planet->population , planet->pop_support );
		}
        if ( IS_IMMORTAL(ch) )
        {
          ch_printf( ch, "&CA&creas: &G");
          for ( area = planet->first_area ; area ; area = area->next_on_planet )
             ch_printf( ch , "%s,  ", area->filename );
          ch_printf( ch, "\n\r" );
        }         
        ch_printf( ch, "\n\r" );
        
        count++;
    }

    if ( !count )
    {
	set_char_color( AT_BLOOD, ch);
        send_to_char( "&CT&chere &Ca&cre &Cn&co &Cp&clanets &Cc&currently &Cf&cormed.\n\r", ch );
    }
    
}

void do_shove( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int exit_dir;
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    bool nogo;
    ROOM_INDEX_DATA *to_room;    
    int chance;  

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Shove whom?\n\r", ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch);
	return;
    }

    if (victim == ch)
    {
	send_to_char("You shove yourself around, to no avail.\n\r", ch);
	return;
    }
    
    if ( (victim->position) != POS_STANDING )
    {
	act( AT_PLAIN, "$N isn't standing up.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	send_to_char( "Shove them in which direction?\n\r", ch);
	return;
    }

    exit_dir = get_dir( arg2 );
    if ( xIS_SET(victim->in_room->room_flags, ROOM_SAFE)
    &&  get_timer(victim, TIMER_SHOVEDRAG) <= 0)
    {
	send_to_char("That character cannot be shoved right now.\n\r", ch);
	return;
    }
    victim->position = POS_SHOVE;
    nogo = FALSE;
    if ((pexit = get_exit(ch->in_room, exit_dir)) == NULL )
      nogo = TRUE;
    else
    if ( IS_SET(pexit->exit_info, EX_CLOSED)
    && (!IS_AFFECTED(victim, AFF_PASS_DOOR)
    ||   IS_SET(pexit->exit_info, EX_NOPASSDOOR)) )
      nogo = TRUE;
    if ( nogo )
    {
	send_to_char( "There's no exit in that direction.\n\r", ch );
        victim->position = POS_STANDING;
	return;
    }
    to_room = pexit->to_room;

    if ( IS_NPC(victim) )
    {
	send_to_char("You can only shove player characters.\n\r", ch);
	return;
    }
    
    if (ch->in_room->area != to_room->area
    &&  !in_hard_range( victim, to_room->area ) )
    {
      send_to_char("That character cannot enter that area.\n\r", ch);
      victim->position = POS_STANDING;
      return;
    }

chance = 50;

/* Add 3 points to chance for every str point above 15, subtract for 
below 15 */

chance += ((get_curr_str(ch) - 15) * 3);

chance += (ch->top_level - victim->top_level);
 
/* Debugging purposes - show percentage for testing */

/* sprintf(buf, "Shove percentage of %s = %d", ch->name, chance);
act( AT_ACTION, buf, ch, NULL, NULL, TO_ROOM );
*/

if (chance < number_percent( ))
{
  send_to_char("You failed.\n\r", ch);
  victim->position = POS_STANDING;
  return;
}
    act( AT_ACTION, "You shove $M.", ch, NULL, victim, TO_CHAR );
    act( AT_ACTION, "$n shoves you.", ch, NULL, victim, TO_VICT );
    move_char( victim, get_exit(ch->in_room,exit_dir), 0);
    if ( !char_died(victim) )
      victim->position = POS_STANDING;
    WAIT_STATE(ch, 12);
    /* Remove protection from shove/drag if char shoves -- Blodkai */
    if ( xIS_SET(ch->in_room->room_flags, ROOM_SAFE)   
    &&   get_timer(ch, TIMER_SHOVEDRAG) <= 0 )
      add_timer( ch, TIMER_SHOVEDRAG, 10, NULL, 0 );
}

void do_drag( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int exit_dir;
    CHAR_DATA *victim;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *to_room;
    bool nogo;
    int chance;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Drag whom?\n\r", ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch);
	return;
    }

    if ( victim == ch )
    {
	send_to_char("You take yourself by the scruff of your neck, but go nowhere.\n\r", ch);
	return; 
    }

    if ( IS_NPC(victim) )
    {
	send_to_char("You can only drag player characters.\n\r", ch);
	return;
    }

    if ( victim->fighting )
    {
        send_to_char( "You try, but can't get close enough.\n\r", ch);
        return;
    }
          
    if ( arg2[0] == '\0' )
    {
	send_to_char( "Drag them in which direction?\n\r", ch);
	return;
    }

    exit_dir = get_dir( arg2 );

    if ( xIS_SET(victim->in_room->room_flags, ROOM_SAFE)
    &&   get_timer( victim, TIMER_SHOVEDRAG ) <= 0)
    {
	send_to_char("That character cannot be dragged right now.\n\r", ch);
	return;
    }

    nogo = FALSE;
    if ((pexit = get_exit(ch->in_room, exit_dir)) == NULL )
      nogo = TRUE;
    else
    if ( IS_SET(pexit->exit_info, EX_CLOSED)
    && (!IS_AFFECTED(victim, AFF_PASS_DOOR)
    ||   IS_SET(pexit->exit_info, EX_NOPASSDOOR)) )
      nogo = TRUE;
    if ( nogo )
    {
	send_to_char( "There's no exit in that direction.\n\r", ch );
	return;
    }

    to_room = pexit->to_room;

    if (ch->in_room->area != to_room->area
    && !in_hard_range( victim, to_room->area ) )
    {
      send_to_char("That character cannot enter that area.\n\r", ch);
      victim->position = POS_STANDING;
      return;
    }
    
    chance = 50;
    

/*
sprintf(buf, "Drag percentage of %s = %d", ch->name, chance);
act( AT_ACTION, buf, ch, NULL, NULL, TO_ROOM );
*/
if (chance < number_percent( ))
{
  send_to_char("You failed.\n\r", ch);
  victim->position = POS_STANDING;
  return;
}
    if ( victim->position < POS_STANDING )
    {
	sh_int temp;

	temp = victim->position;
	victim->position = POS_DRAG;
	act( AT_ACTION, "You drag $M into the next room.", ch, NULL, victim, TO_CHAR ); 
	act( AT_ACTION, "$n grabs your hair and drags you.", ch, NULL, victim, TO_VICT ); 
	move_char( victim, get_exit(ch->in_room,exit_dir), 0);
	if ( !char_died(victim) )
	  victim->position = temp;
/* Move ch to the room too.. they are doing dragging - Scryn */
	move_char( ch, get_exit(ch->in_room,exit_dir), 0);
	WAIT_STATE(ch, 12);
	return;
    }
    send_to_char("You cannot do that to someone who is standing.\n\r", ch);
    return;
}

void do_enlist( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_INPUT_LENGTH];
	CLAN_DATA *clan;
	DESCRIPTOR_DATA *d;	
        
	if ( IS_NPC(ch) || !ch->pcdata )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}
        
	if ( ch->pcdata->clan )
	{
		ch_printf( ch , "You will have to resign from %s before you can join a new organization.\n\r", ch->pcdata->clan->name );
		return;
	}
        
	if ( !xIS_SET( ch->in_room->room_flags , ROOM_RECRUIT ) )
	{
		send_to_char( "You don't seem to be in a recruitment office.\n\r", ch );
		return;
	}
	
	for ( clan = first_clan; clan; clan = clan->next )
	{
		if ( ch->in_room->vnum == clan->enlistroom )
		{
			SET_BIT( ch->speaks, LANG_CLAN );
			++clan->members;
						
			sprintf( buf, "%s %s", clan->memberslist, ch->name );
			STRFREE( clan->memberslist );
			clan->memberslist = STRALLOC( buf );
			
			ch->pcdata->clan_rank = 0;
			STRFREE( ch->pcdata->clan_name );
			ch->pcdata->clan_name = QUICKLINK( clan->name );
			ch->pcdata->clan = clan;
			ch_printf( ch, "Welcome to %s.\n\r", clan->name );
			save_clan ( clan );

			for( d = first_descriptor; d; d = d->next )
			{
				CHAR_DATA *vch;
				CLAN_DATA *clan;

				vch = d->character;

				if( d->connected == CON_PLAYING && !IS_NPC(vch) )
				{
					if( !vch->pcdata->clan )
						continue;

					if( vch->pcdata->clan == clan )
					{
						ch_printf( vch, "&BYou get word that &W%s&B has signed up for %s.&w\n\r", ch->name, clan->name );
						continue;
					}
				}
			}
			return;
		}
	}

	send_to_char( "They don't seem to be recruiting right now.\n\r", ch );
	return;

}

void do_resign( CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA *d;	
	CLAN_DATA *clan;  
	char buf[MAX_STRING_LENGTH];
            
	if ( IS_NPC(ch) || !ch->pcdata )
	{
		send_to_char( "You can't do that.\n\r", ch );
	    return;
	}
        
	clan =  ch->pcdata->clan;
        
	if ( clan == NULL )
	{
		send_to_char( "You have to join an organization before you can quit it.\n\r", ch );
		return;
	}
       
    if ( ch->speaking & LANG_CLAN )
		ch->speaking = LANG_COMMON;
	REMOVE_BIT( ch->speaks, LANG_CLAN );
	--clan->members;

	if ( !str_cmp( ch->name, ch->pcdata->clan->leader ) )
	{
		STRFREE( ch->pcdata->clan->leader );
		ch->pcdata->clan->leader = STRALLOC( "" );
    }
	if ( !str_cmp( ch->name, ch->pcdata->clan->number1 ) )
	{
		STRFREE( ch->pcdata->clan->number1 );
		ch->pcdata->clan->number1 = STRALLOC( "" );
    }
    if ( !str_cmp( ch->name, ch->pcdata->clan->number2 ) )
    {
		STRFREE( ch->pcdata->clan->number2 );
		ch->pcdata->clan->number2 = STRALLOC( "" );
    }
	ch->pcdata->clan = NULL;
    STRFREE(ch->pcdata->clan_name);
    ch->pcdata->clan_name = STRALLOC( "" );
	ch->pcdata->clan_rank = 0;

    act( AT_MAGIC, "You resign your position in $t", ch, clan->name, NULL , TO_CHAR );
	for( d = first_descriptor; d; d = d->next )
	{
		CHAR_DATA *vch;
		vch = d->character;
		if( d->connected == CON_PLAYING && vch != ch && !IS_NPC(vch) )
		{
			if( !vch->pcdata->clan ) continue;
			if( vch->pcdata->clan == clan )
				ch_printf( vch, "&BYou get word that &W%s&B has resigned %s position in %s.&w\n\r", ch->full_name, his_her[ch->sex], clan->name );
		}
	}

    DISPOSE( ch->pcdata->bestowments );
    ch->pcdata->bestowments = str_dup("");

	if ( clan->memberslist[0] != '\0' )
	{
		char arg[MAX_STRING_LENGTH];
		char memberslist[MAX_STRING_LENGTH];
		char * members;

		members = str_alloc(clan->memberslist);
		strcpy(memberslist,"");

		while ( members[0] != '\0' )
		{
			members = one_argument(members,arg);
			if (str_cmp(ch->name,arg) )
			{
				arg[0] = UPPER(arg[0]);
				sprintf(buf, "%s %s",memberslist, arg);
				sprintf(memberslist,"%s",buf);
			}
		}
		STRFREE( clan->memberslist );
		clan->memberslist = STRALLOC( memberslist );
	}
 
	save_clan( clan );
    save_char_obj( ch );	/* clan gets saved when pfile is saved */
    
    return;

}

void do_clan_withdraw( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *clan;
    long       amount;
    
    if ( IS_NPC( ch ) || !ch->pcdata->clan )
    {
	send_to_char( "You don't seem to belong to an organization to withdraw funds from...\n\r", ch );
	return;
    }

    if (!ch->in_room || !xIS_SET(ch->in_room->room_flags, ROOM_BANK) )
    {
       send_to_char( "You must be in a bank to do that!\n\r", ch );
       return;
    }
    
    if ( (ch->pcdata && ch->pcdata->bestowments
    &&    is_name("withdraw", ch->pcdata->bestowments))
    ||   !str_cmp( ch->name, ch->pcdata->clan->leader  ))
	;
    else
    {
   	send_to_char( "&RYour organization hasn't seen fit to bestow you with that ability." ,ch );
   	return;
    }

	clan = ch->pcdata->clan;
    
    amount = atoi( argument );
    
    if ( !amount )
    {
	send_to_char( "How much would you like to withdraw?\n\r", ch );
	return;
    }
    
    if ( amount > clan->funds )
    {
	ch_printf( ch,  "%s doesn't have that much!\n\r", clan->name );
	return;
    }
    
    if ( amount < 0 )
    {
	ch_printf( ch,  "Nice try...\n\r" );
	return;
    }
    
    ch_printf( ch,  "You withdraw %ld credits from %s's funds.\n\r", amount, clan->name );
    
    clan->funds -= amount;
    ch->gold += amount;
    save_clan ( clan );
            
}


void do_clan_donate( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *clan;
    long       amount;

    if ( IS_NPC( ch ) || !ch->pcdata->clan )
    {
	send_to_char( "You don't seem to belong to an organization to donate to...\n\r", ch );
	return;
    }

    if (!ch->in_room || !xIS_SET(ch->in_room->room_flags, ROOM_BANK) )
    {
       send_to_char( "You must be in a bank to do that!\n\r", ch );
       return;
    }

	clan = ch->pcdata->clan;
    
    amount = atoi( argument );
    
    if ( !amount )
    {
	send_to_char( "How much would you like to donate?\n\r", ch );
	return;
    }

    if ( amount < 0 )
    {
	ch_printf( ch,  "Nice try...\n\r" );
	return;
    }
    
    if ( amount > ch->gold )
    {
	send_to_char( "You don't have that much!\n\r", ch );
	return;
    }
    
    ch_printf( ch,  "You donate %ld credits to %s's funds.\n\r", amount, clan->name );
    
    clan->funds += amount;
    ch->gold -= amount;
    save_clan ( clan );
            
}

void do_appoint ( CHAR_DATA *ch , char *argument )
{
	char arg[MAX_STRING_LENGTH], arg2[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	CLAN_DATA *clan;
	int counter, rank;
	char type[MAX_STRING_LENGTH];

	if( IS_NPC(ch) )
	{
		send_to_char( "NPCs cant be in clans.\n\r", ch );
		return;
	}

	if( !ch->pcdata->clan )
	{
		send_to_char( "Join a clan first.\n\r", ch );
		return;
	}

	if( ch->pcdata->clan_rank <= 2 )
	{
		send_to_char( "Your not high enough of a rank to demote anyone.\n\r", ch );
		return;
	}

	argument = one_argument( argument, type );

	if( type[0] == '\0' )
	{
		send_to_char( "Rank or Position(first/second)?\n\r", ch );
		return;
	}

	if ( !str_cmp("rank", type ) )
	{

		if( IS_NPC(ch) )
		{
			send_to_char( "NPCs cant be in clans.\n\r", ch );
			return;
		}

		if( !ch->pcdata->clan )
		{
			send_to_char( "Join a clan first.\n\r", ch );
			return;
		}

		if( ch->pcdata->clan_rank >= 1 || !str_cmp(ch->pcdata->clan->leader,ch->name));
		else
		{
			send_to_char( "Your not high enough of a rank to promote anyone.\n\r", ch );
			return;
		}

		argument = one_argument( argument, arg );
		argument = one_argument( argument, arg2 );

		if( arg[0] == '\0' )
		{
			send_to_char( "Promote who?\n\r", ch );
			return;
		}

		if( ( victim = get_char_room( ch, arg ) ) == NULL )
		{
			send_to_char( "That person is not here.\n\r", ch);
			return;
		}

		if( IS_NPC(victim) )
		{
			do_say(victim,"Oh my god, your promoting me? When did I join?");
			return;
		}

		if( victim == ch )
		{
			send_to_char( "You can't promote yourself.\n\r", ch);
			return;
		}

		if( ( clan = victim->pcdata->clan ) == NULL )
		{
			send_to_char( "They're not in your clan.\n\r", ch );
			return;
		}

		if( clan != ch->pcdata->clan )
		{
			send_to_char( "They're not in your clan.\n\r", ch);
			return;
		}

		if( arg2[0] == '\0' )
		{
			send_to_char( "What rank should they be promoted to:\n\r", ch);

			for( counter = 0 ; counter < ch->pcdata->clan_rank ; counter++ )
				ch_printf( ch, " %d - %s  wage: %d\n\r", counter + 1,
							clan->ranks[counter].name,
							clan->ranks[counter].wage );

			ch_printf( ch, "Their current rank is %s.\n\r", clan->ranks[victim->pcdata->clan_rank].name );

			return;
		}

		if( (ch->pcdata->clan_rank - 1) == victim->pcdata->clan_rank )
		{
			send_to_char( "You cannot promote them any further.\n\r", ch );
			return;
		}

		rank = atoi( arg2) - 1;

		if( rank > ( MAX_RANK - 1 ) || rank < 0 )
		{
			send_to_char( "Invalid rank.\n\r", ch );
			return;
		}

		if( rank == victim->pcdata->clan_rank )
		{
			send_to_char( "They're already at that rank!\n\r", ch );
			return;
		}

		if( rank < victim->pcdata->clan_rank )
		{
			send_to_char( "To lower their rank demote them.\n\r", ch);
			return;
		}

		victim->pcdata->clan_rank = rank;
		ch_printf( victim, "You have been promoted to %s.\n\r", clan->ranks[rank].name );
		send_to_char( "Done.\n\r", ch);
		return;
	}
	else if ( !str_cmp("position", type ) )
	{
		char name[MAX_STRING_LENGTH];
    
		argument = one_argument( argument, name );
    
		if ( IS_NPC( ch ) || !ch->pcdata )
			return;

		if ( !ch->pcdata->clan )
		{
			send_to_char( "Huh?\n\r", ch );
			return;
		}

		if (  str_cmp( ch->name, ch->pcdata->clan->leader  )  )
		{
			send_to_char( "Only your leader can do that!\n\r", ch );
			return;
		}

		if ( argument[0] == '\0' )
		{
			send_to_char( "Useage: appoint <name> < first | second >\n\r", ch );
			return;
		}
    
		if ( !str_cmp( argument , "first" )  )
		{
			if ( ch->pcdata->clan->number1 && str_cmp( ch->pcdata->clan->number1 , "" ) )
			{
				send_to_char( "You already have someone in that position ... demote them first.\n\r", ch );
				return;
			}
         
			STRFREE( ch->pcdata->clan->number1 );
			ch->pcdata->clan->number1 = STRALLOC( name );
		}        
		else if ( !str_cmp( argument , "second" )  )
		{
			if ( ch->pcdata->clan->number2 && str_cmp( ch->pcdata->clan->number2 , "" ))
			{
				send_to_char( "You already have someone in that position ... demote them first.\n\r", ch );
				return;
			}
         
			STRFREE( ch->pcdata->clan->number2 );
			ch->pcdata->clan->number2 = STRALLOC( name );
		}
		else do_appoint( ch , "" );
		save_clan ( ch->pcdata->clan );
       }
	else
	{
		send_to_char("For Rank:\n\r\tSyntax: Appoint Rank <name> <rank#>\n\r",ch);
		send_to_char("For Position\n\r\tSyntax: Appoint Position <name>\n\r",ch);
		return;
	}
}

void do_demote ( CHAR_DATA *ch , char *argument )
{
	char arg[MAX_STRING_LENGTH], arg2[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	CLAN_DATA *clan;
	int counter, rank;
	char type[MAX_STRING_LENGTH];

	if( IS_NPC(ch) )
	{
		send_to_char( "NPCs cant be in clans.\n\r", ch );
		return;
	}

	if( !ch->pcdata->clan )
	{
		send_to_char( "Join a clan first.\n\r", ch );
		return;
	}

	if( ch->pcdata->clan_rank <= 1 )
	{
		send_to_char( "Your not high enough of a rank to demote anyone.\n\r", ch );
		return;
	}

	argument = one_argument( argument, type );

	if( type[0] == '\0' )
	{
		send_to_char( "Rank or Position(first/second)?\n\r", ch );
		return;
	}

	if ( !str_cmp("rank", type ) )
	{
		argument = one_argument( argument, arg );
		argument = one_argument( argument, arg2 );

		if( arg[0] == '\0' )
		{
			send_to_char( "Demote who?\n\r", ch );
			return;
		}

		if( ( victim = get_char_room( ch, arg ) ) == NULL )
		{
			send_to_char( "That person is not here.\n\r", ch );
			return;
		}

		if( victim == ch )
		{
			send_to_char( "You can't demote yourself.\n\r", ch );
			return;
		}

		if( ( clan = victim->pcdata->clan ) == NULL )
		{
			send_to_char( "They're not in your clan.\n\r", ch );
			return;
		}

		if( clan != ch->pcdata->clan )
		{
			send_to_char( "They're not in your clan.\n\r", ch);
			return;
		}

		if( arg2[0] == '\0' )
		{
			send_to_char( "What rank should they be demoted to:\n\r", ch);

			for( counter = 0 ; counter <= ch->pcdata->clan_rank ; counter++ )
				ch_printf( ch, " %d - %s  wage: %d\n\r", counter + 1,
							clan->ranks[counter].name,
							clan->ranks[counter].wage );

			ch_printf( ch, "Their current rank is %s.\n\r", clan->ranks[victim->pcdata->clan_rank].name );

			return;
		}

		if( ch->pcdata->clan_rank >= victim->pcdata->clan_rank )
		{
			send_to_char( "You cannot demote them.\n\r", ch );
			return;
		}

		if( victim->pcdata->clan_rank == 0 )
		{
			send_to_char( "You cannot demote them any further.\n\r", ch );
			return;
		}

		rank = atoi( arg2) - 1;

		if( rank > ( MAX_RANK - 1 ) || rank < 0 )
		{
			send_to_char( "Invalid rank.\n\r", ch );
			return;
		}

		if( rank == victim->pcdata->clan_rank )
		{
			send_to_char( "They're already at that rank!\n\r", ch );
			return;
		}

		if( rank > victim->pcdata->clan_rank )
		{
			send_to_char( "To raise their rank promote them.\n\r", ch);
			return;
		}

		victim->pcdata->clan_rank = rank;
		ch_printf( victim, "You have been demoted to %s.\n\r", clan->ranks[rank].name );
		send_to_char( "Done.\n\r", ch);
		return;
	}
	else if ( !str_cmp("position", type ) )
	{
		if ( IS_NPC( ch ) || !ch->pcdata )
			return;

		if ( !ch->pcdata->clan )
		{
			send_to_char( "Huh?\n\r", ch );
			return;
		}

		if (  str_cmp( ch->name, ch->pcdata->clan->leader  )  )
		{
			send_to_char( "Only your leader can do that!\n\r", ch );
			return;
		}

		if ( argument[0] == '\0' )
		{
			send_to_char( "Demote who?\n\r", ch );
			return;
		}
		if ( !str_cmp( argument , ch->pcdata->clan->number1 )  )
		{
			send_to_char( "Player Demoted!", ch );
         
			STRFREE( ch->pcdata->clan->number1 );
			ch->pcdata->clan->number1 = STRALLOC( "" );
		}        
		else if ( !str_cmp( argument , ch->pcdata->clan->number2 )  )
		{
			send_to_char( "Player Demoted!", ch );
         
			STRFREE( ch->pcdata->clan->number2 );
			ch->pcdata->clan->number2 = STRALLOC( "" );
		}        
		else
		{
			send_to_char( "They seem to have been demoted already.\n\r", ch );
			return;
		}
		save_clan ( ch->pcdata->clan );
	}
	else
	{
		send_to_char("For Rank:\n\r\tSyntax: Demote Rank <name> <rank#>\n\r",ch);
		send_to_char("For Position\n\r\tSyntax: Demote Position <name>\n\r",ch);
		return;
	}
}

void do_capture ( CHAR_DATA *ch , char *argument )
{
   CLAN_DATA *clan;
   PLANET_DATA *planet;
   PLANET_DATA *cPlanet;
   float support = 0.0;
   int pCount = 0;   
   char buf[MAX_STRING_LENGTH];
   
   if ( !ch->in_room || !ch->in_room->area)
      return;   

   if ( IS_NPC(ch) || !ch->pcdata )
   {
       send_to_char ( "huh?\n\r" , ch );
       return;
   }
   
   if ( !ch->pcdata->clan )
   {
       send_to_char ( "You need to be a member of an organization to do that!\n\r" , ch );
       return;
   }
   clan = ch->pcdata->clan;
      
   if ( clan->clan_type == CLAN_CRIME )
   {
       send_to_char ( "Crime fimilies aren't in the business of controlling worlds.\n\r" , ch );
       return;
   }

   if ( clan->clan_type == CLAN_GUILD )
   {
       send_to_char ( "Your organization serves a much greater purpose.\n\r" , ch );
       return;
   }

   if ( ( planet = ch->in_room->area->planet ) == NULL )
   {
       send_to_char ( "You must be on a planet to capture it.\n\r" , ch );
       return;
   }   
   
   if ( clan == planet->governed_by )
   {
       send_to_char ( "Your organization already controls this planet.\n\r" , ch );
       return;
   }
   
   if ( planet->starsystem )
   {
       SHIP_DATA *ship;
       CLAN_DATA *sClan;
              
       for ( ship = planet->starsystem->first_ship ; ship ; ship = ship->next_in_starsystem )
       {
		   if (!IS_SET(ship->flags,SHIP_MOB) )
			   continue;
		   sClan = get_clan(ship->owner);
		   if ( !sClan ) 
			   continue;
		   if ( sClan == planet->governed_by )
		   {
			   send_to_char ( "A planet cannot be captured while protected by orbiting spacecraft.\n\r" , ch );
			   return;
		   }
       }
   }

   if ( IS_SET( planet->flags, PLANET_NOCAPTURE ) )
   {
       send_to_char ( "This planet cannot be captured.\n\r" , ch);
       return;
   }

   if ( IS_SET( planet->flags, PLANET_NEUTRAL ) )
   {
	   send_to_char ( "This planet canot be captured.\n\r" , ch);
	   return;
   }

   if ( planet->pop_support > 0 )
   {
       send_to_char ( "The population is not in favour of changing leaders right now.\n\r" , ch );
       return;
   }
   
   for ( cPlanet = first_planet ; cPlanet ; cPlanet = cPlanet->next )
        if ( clan == cPlanet->governed_by )
        {
            pCount++;
            support += cPlanet->pop_support;
        }
   
   if ( support < 0 )
   {
       send_to_char ( "There is not enough popular support for your organization!\n\rTry improving loyalty on the planets that you already control.\n\r" , ch );
       return;
   }
   
   planet->governed_by = clan;
   if ( planet->starsystem )
   {
	   planet->starsystem->governed_by = clan;
	   planet->starsystem->controls = 0;
	   planet->starsystem->fcz = 0;
	   planet->starsystem->soldiers = 0;
	   save_starsystem(planet->starsystem);
   }
   planet->pop_support = 50;
   
   sprintf(buf, "%s is now ours!",planet->name/*,clan->name*/);
   do_chat(ch,buf);
   
   save_planet( planet );
      
   return; 
}

void do_empower ( CHAR_DATA *ch , char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CLAN_DATA *clan;
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC( ch ) || !ch->pcdata->clan )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    clan = ch->pcdata->clan;

	if ( (ch->pcdata && ch->pcdata->bestowments
		&& is_name("empower", ch->pcdata->bestowments))
		|| !str_cmp( ch->name, clan->leader  ) ) 
		;
    else
    {
		send_to_char( "You clan hasn't seen fit to bestow that ability to you!\n\r", ch );
		return;
    }

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Empower whom to do what?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	    send_to_char( "Nice try.\n\r", ch );
	    return;
    }
 
    if ( victim->pcdata->clan != ch->pcdata->clan )
    {
	    send_to_char( "This player does not belong to your clan!\n\r", ch );
	    return;
    }

    if (!victim->pcdata->bestowments)
      victim->pcdata->bestowments = str_dup("");

    if ( arg2[0] == '\0' || !str_cmp( arg2, "list" ) )
    {
        ch_printf( ch, "Current bestowed commands on %s: %s.\n\r",
                      victim->name, victim->pcdata->bestowments );
        return;
    }

    if ( !str_cmp( arg2, "none" ) )
    {
        DISPOSE( victim->pcdata->bestowments );
	victim->pcdata->bestowments = str_dup("");
        ch_printf( ch, "Bestowments removed from %s.\n\r", victim->name );
        ch_printf( victim, "%s has removed your bestowed clan abilities.\n\r", ch->name );
        return;
    }
    else if ( !str_cmp( arg2, "pilot" ) )
    {
      sprintf( buf, "%s %s", victim->pcdata->bestowments, arg2 );
      DISPOSE( victim->pcdata->bestowments );
      victim->pcdata->bestowments = str_dup( buf );
      ch_printf( victim, "%s has given you permission to fly clan ships.\n\r", 
             ch->name );
      send_to_char( "Ok, they now have the ability to fly clan ships.\n\r", ch );
    }
    else if ( !str_cmp( arg2, "withdraw" ) )
    {
      sprintf( buf, "%s %s", victim->pcdata->bestowments, arg2 );
      DISPOSE( victim->pcdata->bestowments );
      victim->pcdata->bestowments = str_dup( buf );
      ch_printf( victim, "%s has given you permission to withdraw clan funds.\n\r", 
             ch->name );
      send_to_char( "Ok, they now have the ablitity to withdraw clan funds.\n\r", ch );
    }
    else if ( !str_cmp( arg2, "clanbuyship" ) )
    {
      sprintf( buf, "%s %s", victim->pcdata->bestowments, arg2 );
      DISPOSE( victim->pcdata->bestowments );
      victim->pcdata->bestowments = str_dup( buf );
      ch_printf( victim, "%s has given you permission to buy clan ships.\n\r", 
             ch->name );
      send_to_char( "Ok, they now have the ablitity to use clanbuyship.\n\r", ch );
    }
    else if ( !str_cmp( arg2, "induct" ) )
    {
      sprintf( buf, "%s %s", victim->pcdata->bestowments, arg2 );
      DISPOSE( victim->pcdata->bestowments );
      victim->pcdata->bestowments = str_dup( buf );
      ch_printf( victim, "%s has given you permission to induct new members.\n\r", 
             ch->name );
      send_to_char( "Ok, they now have the ablitity to induct new members.\n\r", ch );
    }
	else if ( !str_cmp( arg2, "war" ) )
    {
      sprintf( buf, "%s %s", victim->pcdata->bestowments, arg2 );
      DISPOSE( victim->pcdata->bestowments );
      victim->pcdata->bestowments = str_dup( buf );
      ch_printf( victim, "%s has given you permission to declare war on other clans.\n\r", 
             ch->name );
      send_to_char( "Ok, they now have the ablitity to declare war on other clans.\n\r", ch );
    }
	else if( !str_cmp( arg2, "sellclanship" ) )
    {
      	sprintf( buf, "%s %s", victim->pcdata->bestowments, arg2 );
      	DISPOSE( victim->pcdata->bestowments );
      	victim->pcdata->bestowments = str_dup( buf );
      	ch_printf( victim, "%s has given you permission to sell clan ships.\n\r",
             ch->name );
      	send_to_char( "Ok, they now have the ablitity to sell clan ships.\n\r", ch );
    }
	else if( !str_cmp( arg2, "setsystemcode" ) )
    {
      	sprintf( buf, "%s %s", victim->pcdata->bestowments, arg2 );
      	DISPOSE( victim->pcdata->bestowments );
      	victim->pcdata->bestowments = str_dup( buf );
      	ch_printf( victim, "%s has given you permission to set starsystem codes.\n\r",
             ch->name );
      	send_to_char( "Ok, they now have the ability to set starsystem codes.\n\r", ch );
    }
	else if( !str_cmp( arg2, "alertstatus" ) )
    {
      	sprintf( buf, "%s %s", victim->pcdata->bestowments, arg2 );
      	DISPOSE( victim->pcdata->bestowments );
      	victim->pcdata->bestowments = str_dup( buf );
      	ch_printf( victim, "%s has given you permission to change starsystem alert status.\n\r",
             ch->name );
      	send_to_char( "Ok, they now have the ability to change starsystem alert status.\n\r", ch );
    }
	else if ( !str_cmp( arg2, "psheild" ) )
	{
		sprintf( buf, "%s %s", victim->pcdata->bestowments, arg2 );
		DISPOSE( victim->pcdata->bestowments );
		victim->pcdata->bestowments = str_dup( buf );
		ch_printf( victim, "%s has given you permission to toggle the planetary shield.\n\r",
			ch->name );
		send_to_char( "Ok, they now have the ability to toggle the planetary shield.\n\r",ch);
	}
	else if ( !str_cmp( arg2, "empower" ) )
	{
		sprintf( buf, "%s %s", victim->pcdata->bestowments, arg2 );
		DISPOSE( victim->pcdata->bestowments );
		victim->pcdata->bestowments = str_dup( buf );
		ch_printf( victim, "%s has given you permission to empower members with power.\n\r",
			ch->name );
		send_to_char( "Ok, they now have the ability to empower members with power.\n\r",ch);
	}
	else if ( !str_cmp( arg2, "setcode" ) )
	{
		sprintf( buf, "%s %s", victim->pcdata->bestowments, arg2 );
		DISPOSE( victim->pcdata->bestowments );
		victim->pcdata->bestowments = str_dup( buf );
		ch_printf( victim, "%s has given you permission to set the planetary code.\n\r",
			ch->name );
		send_to_char( "Ok, they now have the ability to set the planetary code.\n\r",ch);
	}
    else
	{
		send_to_char( "Currently you may empower members with only the following:\n\r\n\r", ch ); 
		send_to_char( "  pilot:          ability to fly clan ships\n\r", ch );
		send_to_char( "  withdraw:       ability to withdraw clan funds\n\r", ch );
		send_to_char( "  clanbuyship:    ability to buy clan ships\n\r", ch );    
		send_to_char( "  clansellship:   ability to sell clan ships\n\r", ch );    
		send_to_char( "  induct:         ability to induct new members\n\r", ch );    
		send_to_char( "  war:            ability to declare war on onther clans\n\r", ch );
		send_to_char( "  setsystemcode:  ability to set starsystem codes\n\r", ch );
		send_to_char( "  alertstatus:    ability to change starsystem alert status\n\r", ch );    
		send_to_char( "  pshield:        ability to change starsystem alert status\n\r", ch );    
		send_to_char( "  empower:        ability to empower members\n\r", ch );    
		send_to_char( "  setcode:        ability to set the planet code\n\r", ch );
		send_to_char( "  none:           removes bestowed abilities\n\r", ch );    
    }
    
    save_char_obj( victim );	/* clan gets saved when pfile is saved */
    return;


}

long get_taxes( PLANET_DATA *planet )
{
      long gain;
      
      gain = planet->base_value;
      gain += planet->base_value*planet->pop_support/100;
      gain += UMAX(0, planet->pop_support/10 * planet->population);
      
      return gain;
}

void do_myclan( CHAR_DATA *ch, char *argument )
{   
    CLAN_DATA *clan;
	int counter;
	char listmembers[MAX_STRING_LENGTH];

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

	if ( !ch->pcdata || !ch->pcdata->clan ) {
		send_to_char( "You arn't in a clan\n\r", ch);
		return;
	}

    clan = ch->pcdata->clan;
    if ( !clan )
    {
	send_to_char( "No such clan.\n\r", ch );
	return;
    }
	listmembers[0] = '\0';

	if ( clan->memberslist[0] != '\0' )
	{
		char arg[MAX_STRING_LENGTH];
		char buf[MAX_STRING_LENGTH];
		char memberslist[MAX_STRING_LENGTH];
		char * members;
		int x,count;
		members = str_alloc(clan->memberslist);
		strcpy(memberslist,"");
		count = 1;

		while ( members[0] != '\0' )
		{
			members = one_argument(members,arg);
			if ( (strlen(memberslist)+(strlen(arg)+1)) > 60 )
			{
				for( x = strlen(memberslist) ; x < 60 ; x++)
				{
					strcat(memberslist, " ");                    
				}
				if ( strlen(memberslist) == 60 )
				{
					sprintf(buf,"%s %s&B|\n\r|&G&W ",listmembers ,memberslist);
					sprintf(listmembers,"%s", buf);
					strcpy(memberslist,"");
				}
			}
			arg[0] = UPPER(arg[0]);
			if ( count == 1 )
				sprintf(buf, "%s", arg);
			else
				sprintf(buf, "%s %s",memberslist, arg);
			sprintf(memberslist,"%s",buf);
			count++;
		}
		for( x = strlen(memberslist) ; x < 60 ; x++)
		{
			strcat(memberslist, " ");
		}
		if ( strlen(memberslist) == 60 )
		{
			sprintf(buf,"%s %s&B|\n\r",listmembers ,memberslist);
			sprintf(listmembers,"%s", buf);
			strcpy(memberslist,"");
		}
	}
	ch_printf( ch, "&P+&B--------------------------------------------------------------&P+&B\n\r");
	ch_printf( ch, "&B| &BL&beadership                                                   &B|\n\r");
	ch_printf( ch, "&B|     &PL&peader: &R%-10.10s&G&W &PF&pirst: &R%-10.10s &PS&pecond: &R%-10.10s  &B|\n\r",
		clan->leader,clan->number1,clan->number2);
	ch_printf( ch, "&B| &PN&pumber &Po&pf &PM&pembers&P: &R%-3d &PF&punds&P: &R%-20ld           &B|\n\r",
    			clan->members,clan->funds );
	ch_printf( ch, "&B| &PD&pescription&P: &R%-47.47s &B|\n\r", clan->description );
	ch_printf( ch, "&B| &PA&ptwar &Pw&pith&P: %-48.48s &B|\n\r", clan->atwar );
	ch_printf( ch, "&B| &PY&pour &PC&plan &PR&pank&P: &R%-2d &P-&R %-20.20s &P(&B$&R%-5d&P)           &B|\n\r", 
		ch->pcdata->clan_rank , clan->ranks[ch->pcdata->clan_rank].name, clan->ranks[ch->pcdata->clan_rank].wage);
	if ( !str_cmp( ch->name, clan->leader  ) || !str_cmp( ch->name, clan->number1 ) || !str_cmp( ch->name, clan->number2 ) )
	{
		for( counter = 0 ; counter <= ( MAX_RANK - 1 ) ; counter++ )
		{
			ch_printf( ch, "&B| &PR&pank&P:&R %-20.20s  &PW&page&P:&R %-5d                      &B|\n\r",
				clan->ranks[counter].name, clan->ranks[counter].wage );
		}
	}
	ch_printf( ch, "&P+&B--------------------------------------------------------------&P+&B\n\r");
	ch_printf( ch, "&B| &PM&pembers&P:                                                     &B|\n\r");
	if ( listmembers[0] != '\0' )
		ch_printf( ch, "&B| &W%s", listmembers );
	ch_printf( ch, "&P+&B--------------------------------------------------------------&P+&B\n\r");
    return;
}

void do_overthrow( CHAR_DATA *ch , char * argument )
{

	CLAN_DATA *clan;

    if ( IS_NPC( ch ) )
       return;
       
    if ( !ch->pcdata || !ch->pcdata->clan )
    {
       send_to_char( "You have to be part of an organization before you can claim leadership.\n\r", ch );
       return;
    }

	clan = ch->pcdata->clan;
    if ( !clan )
    {
	send_to_char( "No such clan.\n\r", ch );
	return;
    }

    if ( clan->leader[0] != '\0' )
    {
       send_to_char( "Your organization already has strong leadership...\n\r", ch );
       return;
    }

    ch_printf( ch, "OK. You are now a leader of %s.\n\r", ch->pcdata->clan->name );
    
	STRFREE( ch->pcdata->clan->leader );
	if ( !strcmp(ch->name,ch->pcdata->clan->number1) )
	{
		ch->pcdata->clan->leader = STRALLOC( ch->pcdata->clan->number1 );
		STRFREE( ch->pcdata->clan->number1 );
		ch->pcdata->clan->number1 = STRALLOC( "" );
	}
	else if ( !strcmp(ch->name,ch->pcdata->clan->number2) )
	{
		ch->pcdata->clan->leader = STRALLOC( ch->pcdata->clan->number2 );
		STRFREE( ch->pcdata->clan->number2 );
		ch->pcdata->clan->number2 = STRALLOC( "" );
	}
	else
		ch->pcdata->clan->leader = STRALLOC( ch->name );
	
	ch->pcdata->clan_rank = (MAX_RANK - 1);
    save_char_obj( ch );	/* clan gets saved when pfile is saved */
         
}

void do_war ( CHAR_DATA *ch , char *argument )
{
    CLAN_DATA *wclan;
    CLAN_DATA *clan;
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC( ch ) || !ch->pcdata || !ch->pcdata->clan )
    {
		send_to_char( "Huh?\n\r", ch );
		return;
    }

    clan = ch->pcdata->clan;

    if ( ( ch->pcdata->bestowments
		&&    is_name("war", ch->pcdata->bestowments))
		|| nifty_is_name( ch->name, clan->leader )
		|| nifty_is_name( ch->name, clan->number1 ) 
		|| nifty_is_name( ch->name, clan->number2 ) )
		;
    else
    {
		send_to_char( "You clan hasn't empowered you to declare war!\n\r", ch );
		return;
    }


    if ( argument[0] == '\0' )
    {
		send_to_char( "&G&WCurrently Atwar with:\n\r",ch );
		send_to_char( clan->atwar,ch );
		send_to_char( "\n\r" , ch);
		return;
    }

    if ( ( wclan = get_clan( argument ) ) == NULL )
    {
		send_to_char( "No such clan.\n\r", ch);
		return;
    }

    if ( wclan == clan )
    {
		send_to_char( "Declare war on yourself?!\n\r", ch);
		return;
    }   

    if ( nifty_is_name( wclan->name , clan->atwar ) )
    {
        CLAN_DATA *tclan;
        strcpy( buf, "" );
        
        for ( tclan = first_clan ; tclan ; tclan = tclan->next )
            if ( nifty_is_name( tclan->name , clan->atwar ) && tclan != wclan )
            {
                 strcat ( buf, "\n\r " );
                 strcat ( buf, tclan->name );
                 strcat ( buf, " " );
            }
        
        STRFREE ( clan->atwar );
        clan->atwar = STRALLOC( buf );
        
        sprintf( buf , "%s has declared a ceasefire with %s!" , clan->name , wclan->name );
        echo_to_all( AT_WHITE , buf , ECHOTAR_ALL );

        save_char_obj( ch );	/* clan gets saved when pfile is saved */
        
	return;
    }
    
    strcpy ( buf, clan->atwar );
    strcat ( buf, "\n\r " );
    strcat ( buf, wclan->name );
    strcat ( buf, " " );
    
    STRFREE ( clan->atwar );
    clan->atwar = STRALLOC( buf );
    
    sprintf( buf , "%s has declared war on %s!" , clan->name , wclan->name );
    echo_to_all( AT_RED , buf , ECHOTAR_ALL );

    save_char_obj( ch );	/* clan gets saved when pfile is saved */

}

/* raises someones rank by one */
void do_promote( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_STRING_LENGTH], arg2[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	CLAN_DATA *clan;
	int counter, rank;

	send_to_char( "Use appoint instead. and contact an imm to fix this command\n\r",ch);
	return;


	if( IS_NPC(ch) )
	{
		send_to_char( "NPCs cant be in clans.\n\r", ch );
		return;
	}

	if( !ch->pcdata->clan )
	{
		send_to_char( "Join a clan first.\n\r", ch );
		return;
	}

	if( ch->pcdata->clan_rank >= 1 || !str_cmp(ch->pcdata->clan->leader,ch->name));
	else
	{
		send_to_char( "Your not high enough of a rank to promote anyone.\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg2 );

	if( arg[0] == '\0' )
	{
		send_to_char( "Promote who?\n\r", ch );
		return;
	}

	if( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
		send_to_char( "That person is not here.\n\r", ch);
		return;
    }

	if( IS_NPC(victim) )
	{
		do_say(victim,"Oh my god, your promoting me? When did I join?");
		return;
	}

	if( victim == ch )
	{
		send_to_char( "You can't promote yourself.\n\r", ch);
		return;
	}

	if( ( clan = victim->pcdata->clan ) == NULL )
	{
		send_to_char( "They're not in your clan.\n\r", ch );
		return;
	}

	if( clan != ch->pcdata->clan )
	{
		send_to_char( "They're not in your clan.\n\r", ch);
		return;
	}

	if( arg2[0] == '\0' )
	{
		send_to_char( "What rank should they be promoted to:\n\r", ch);

		for( counter = 0 ; counter < ch->pcdata->clan_rank ; counter++ )
			ch_printf( ch, " %d - %s  wage: %d\n\r", counter + 1,
						clan->ranks[counter].name,
						clan->ranks[counter].wage );

		ch_printf( ch, "Their current rank is %s.\n\r", clan->ranks[victim->pcdata->clan_rank].name );

		return;
	}

	if( ch->pcdata->clan_rank - 1 <= victim->pcdata->clan_rank )
	{
		send_to_char( "You cannot promote them any further.\n\r", ch );
		return;
	}

	rank = atoi( arg2) - 1;

	if( rank > ( MAX_RANK - 1 ) || rank < 0 )
	{
		send_to_char( "Invalid rank.\n\r", ch );
		return;
	}

	if( rank == victim->pcdata->clan_rank )
	{
		send_to_char( "They're already at that rank!\n\r", ch );
		return;
	}

	if( rank < victim->pcdata->clan_rank )
	{
		send_to_char( "To lower their rank demote them.\n\r", ch);
		return;
	}

	victim->pcdata->clan_rank = rank;
	ch_printf( victim, "You have been promoted to %s.\n\r", clan->ranks[rank].name );
	send_to_char( "Done.\n\r", ch);
	return;
}

void do_order_planet( CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *mob;
    char arg[MAX_INPUT_LENGTH];
    int cost;
	PLANET_DATA *planet;

    /* check for surgeon */
    for ( mob = ch->in_room->first_person; mob; mob = mob->next_in_room )
    {
        if (mob->spec_fun == spec_lookup( "spec_clan_order" ) || mob->spec_2 == spec_lookup( "spec_clan_order" ) )
            break;
    }

	if ( !ch->in_room->area || ( planet = ch->in_room->area->planet ) == NULL )
	{
		send_to_char("How do you expect to buy things for the planet when your not on a planet?\n\r",ch);
		return;
	}

    /* if there are store keeper*/ 
    if ( mob == NULL )
    {
		ch_printf(ch,"Current Planet Stats:\n\r");
		ch_printf(ch,"   Control Towers    : %d\n\r",planet->controls);
		ch_printf(ch,"   Shield Generators : %d\n\r",planet->shields);
		ch_printf(ch,"   Barracks          : %d\n\r",planet->barracks);
		return;
    }

    one_argument(argument,arg);

	/* if there are a surgeon, give a list*/
    if (arg[0] == '\0' && mob != NULL)
    {
        /* display price list */
		send_to_char("The following parts are available:",ch );
		send_to_char("  controls   : Control Towers      750000 creds\n\r",ch);
		send_to_char("  shield     : Shield Generators   100000 creds\n\r",ch);
		send_to_char("  barracks   : Barracks            100000 creds\n\r",ch);
		send_to_char(" Type order_planet <type> to buy one, or help order_planet to get more info.\n\r",ch);
		return;
    }

    /* Lets see what the character wants to have */
    if (!str_prefix(arg,"control"))
    {
		cost  = 750000;
		if (cost > (ch->gold))
		{
			do_say( mob, "You do not have enough creds for my services." );
			return;
		}
		planet->controls++;
    }

    else if (!str_prefix(arg,"shield"))
    {
		cost  = 100000;
		if (cost > (ch->gold))
		{
			do_say( mob, "You do not have enough creds for my services." );
			return;
		}
		planet->shields++;
    }
	else if (!str_prefix(arg,"barracks"))
    {
		cost  = 100000;
		if (cost > (ch->gold))
		{
			do_say( mob, "You do not have enough creds for my services." );
			return;
		}
			planet->barracks++;
    }
    else 
	{
		do_say( mob, "Type 'order_planet' for a list of planetary upgrades." );
		return;
    }

    WAIT_STATE(ch,PULSE_VIOLENCE);

    ch->gold -= cost;
    do_say( mob,"There we go, planetary defences upgraded." ); 
}    

void do_setsystemcode( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    SPACE_DATA *system;
    CLAN_DATA *clan;

    if ( IS_NPC( ch ) || !ch->pcdata->clan )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    clan = ch->pcdata->clan;
    
    if ( (ch->pcdata && ch->pcdata->bestowments
    &&    is_name("setsystemcode", ch->pcdata->bestowments))
    ||   !str_cmp( ch->name, clan->leader  ) )
	;
    else
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "syntax: setsystemcode <starsystem> <code>\n\r", ch );
	return;
    }

	if ( ( system = starsystem_from_name( arg ) ) == NULL )
	{
		send_to_char("No such starsystem.\n\r", ch);
		return;
	}

	if ( system->governed_by && str_cmp(system->governed_by->name,clan->name) )
	{
		send_to_char("You do not control that system.\n\r", ch);
		return;
	}

	if ( atoi(argument) < 0 || atoi(argument) > 99999999 )
	{
		send_to_char("Code must be between 0 and 99999999 (0 being safe for all).\n\r", ch);
		return;
	}

	send_to_char("Done.\n\r", ch);
	system->code = atoi(argument);
	save_starsystem(system);
	return;
}

void do_alertstatus( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    SPACE_DATA *system;
    CLAN_DATA *clan;
	int alertstatus = -1;

    if ( IS_NPC( ch ) || !ch->pcdata->clan )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    clan = ch->pcdata->clan;
    
    if ( (ch->pcdata && ch->pcdata->bestowments
    &&    is_name("alertstatus", ch->pcdata->bestowments))
    ||   !str_cmp( ch->name, clan->leader  ) )
	;
    else
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
		send_to_char( "syntax: alertstatus <starsystem> <code>\n\r", ch );
		send_to_char( "  Code being:\n\r"
					  "    Peace(attack nobody)\n\r"
					  "    Neutral(Attact At war clans)\n\r"
					  "    War (Attacts anything not in your clan)\n\r", ch );
		return;
    }

	if ( ( system = starsystem_from_name( arg ) ) == NULL )
	{
		send_to_char("No such starsystem.\n\r", ch);
		return;
	}

	if ( system->governed_by && str_cmp(system->governed_by->name,clan->name) )
	{
		send_to_char("You do not control that system.\n\r", ch);
		return;
	}
	if ( !str_cmp("peace", argument ) )
	{
		alertstatus = 0;
		set_char_color(AT_RED + AT_BLINK, ch);
		send_to_char("[System Status]: ",ch);
		set_char_color(AT_WHITE, ch);
		send_to_char("Peace\n\r",ch);
	}
	else if ( !str_cmp("neutral", argument ) )
	{
		alertstatus = 1;
		set_char_color(AT_RED + AT_BLINK, ch);
		send_to_char("[System Status]: ",ch);
		set_char_color(AT_WHITE, ch);
		send_to_char("Neutral\n\r",ch);
	}
	else if ( !str_cmp("war", argument ) )
	{
		alertstatus = 2;
		set_char_color(AT_RED + AT_BLINK, ch);
		send_to_char("[System Status]: ",ch);
		set_char_color(AT_WHITE, ch);
		send_to_char("War\n\r",ch);
	}
	else
	{
		send_to_char( "syntax: alertstatus <starsystem> <code>\n\r", ch );
		send_to_char( "  Code being:\n\r"
					  "    Peace(attack nobody)\n\r"
					  "    Neutral(Attact At war clans)\n\r"
					  "    War (Attacts anything not in your clan)\n\r", ch );
		return;
	}

	send_to_char("Done.\n\r", ch);
	system->alertstatus = alertstatus;
	save_starsystem(system);
	return;
}

/********************************************************************************************/

void do_carry( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	int chance;
	bool allow = FALSE;
	
	argument = one_argument( argument, arg );

	if ( IS_NPC(ch) )
	{
		send_to_char( "Huh?\n\r", ch);
		return;
	}
	
	if ( arg[0] == '\0' )
	{
		send_to_char( "Carry whom?\n\r", ch);
		return;
	}
	
	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch);
		return;
	}

	if (ch->char_carrying != NULL && ch->char_carrying != victim)
	{
		send_to_char( "You can't carry more than one person.\n\r", ch);
		return;
	}
	
	if (ch->char_carriedby != NULL)
	{
		send_to_char( "You can't carry while someone is carrying you.\n\r", ch);
		return;
	}
	
	if ( victim == ch )
	{
		send_to_char("You try to pick yourself up, but just end up tumbling to the ground.\n\r", ch);
		ch->position = POS_SITTING;
		return;
	}

	if ( ch->char_carrying == victim )
	{
		clear_carry(ch);
		clear_carry(victim);
		victim->position = POS_STANDING;
		ch->position = POS_STANDING;
		act( AT_ACTION, "You slowly and carefully put $N down.", ch, NULL, victim, TO_CHAR );
		act( AT_ACTION, "$n slowly and carefully puts you down.", ch, NULL, victim, TO_VICT );
		act( AT_ACTION, "$n slowly and carefully puts $N down.", ch, NULL, victim, TO_NOTVICT );
		return;
	}
	
	if (victim->char_carrying != NULL)
	{
		send_to_char( "That person is carrying another.\n\r", ch);
		return;
	}
	
	if (victim->char_carriedby != NULL)
	{
		send_to_char( "That person is already being carried by someone else.\n\r", ch);
		return;
	}

	if ( !IS_NPC(victim) && nifty_is_name( ch->name , victim->pcdata->partner))
		allow = TRUE;

    if ( victim->position > POS_STUNNED && allow != TRUE )
    {
		send_to_char("You can only pick up stunned characters.\n\r", ch);
		return;
    }

    chance = 60;
	if ( ch->carry_weight + victim->weight > can_carry_w( ch ) )
	/*if (chance < number_percent( ))*/
	{
		act( AT_ACTION, "You have trouble balancing $M on your person.", ch, NULL, victim, TO_CHAR );
		act( AT_ACTION, "$n tries to pick up you, but can't seem to manage the load.", ch, NULL, victim, TO_VICT );
		act( AT_ACTION, "$n tries to pick up $N, but can't seem to manage the load.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	victim->char_carriedby = ch;
	ch->char_carrying = victim;
	victim->position = POS_CARRIED;
	ch->position = POS_CARRYING;
	act( AT_ACTION, "You pick up $N and start carrying $M around.", ch, NULL, victim, TO_CHAR );
	act( AT_ACTION, "$n picks up you and starts to carry you around.", ch, NULL, victim, TO_VICT );
	act( AT_ACTION, "$n picks up $N and starts to carry $M around.", ch, NULL, victim, TO_NOTVICT );
}

void clear_carry ( CHAR_DATA * ch )
{
	if ( !ch || ch == NULL )
	{
		bug("clear_carry: Null ch",0);
		return;
	}
	
	ch->char_carriedby = NULL;
	ch->char_carrying = NULL;		
	return;
}

void do_pshield(CHAR_DATA *ch, char *argument)
{
	PLANET_DATA *planet;
	CLAN_DATA *clan;

	if ( !ch->in_room || !ch->in_room->area)
		return;   

	if ( IS_NPC(ch) || !ch->pcdata )
	{
		do_say( ch, "I'm not allowed to do this" );
		return;
	}

	if ( !ch->pcdata->clan )
	{
		send_to_char("Please Join A clan before attempting to lower the shields",ch );
		return;
	}
   
	clan = ch->pcdata->clan;

    if ( (ch->pcdata && ch->pcdata->bestowments
		&& is_name("pshield", ch->pcdata->bestowments))
		|| !str_cmp( ch->name, clan->leader  )
		|| !str_cmp( ch->name, clan->number1 )
		|| !str_cmp( ch->name, clan->number2 ) ) 
		;
	else
	{
		send_to_char( "Huh?\n\r", ch );
		return;
	}
      
	if ( ( planet = ch->in_room->area->planet ) == NULL )
	{
		ch_printf( ch, "Note to self, I must be on a planet to toggle the shield\n\r" );
		return;
	}   

	if ( planet->shields <= 0 )
	{
		send_to_char("&W[&R^zFAILURE&W^x] Shield Generators Are Unavailable or Destroyed\n\r",ch);
		return;
	}

	if ( planet->governed_by != clan )
	{
		send_to_char("&W[&R^zERROR&W^x] You do not have access to the shields\n\r",ch);
		return;
	}

	if ( IS_SET(planet->flags, PLANET_PSHIELD) )
	{
		do_say( ch, "Planetary Shield Off now!" );
		TOGGLE_BIT(planet->flags, PLANET_PSHIELD);
	}
	else
	{
		do_say( ch, "Planetary Shield On now!" );
		TOGGLE_BIT(planet->flags, PLANET_PSHIELD);
	}

	save_planet( planet );
    return;
}
