#include <math.h> 
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

SHIP_PROTOTYPE * first_ship_prototype;
SHIP_PROTOTYPE * last_ship_prototype;

/* local routines */
void	fread_ship_prototype	args( ( SHIP_PROTOTYPE *ship, FILE *fp ) );
bool	load_ship_prototype	args( ( char *shipfile ) );
void	write_prototype_list	args( ( void ) );
long int get_prototype_value args( ( SHIP_PROTOTYPE *prototype ) );

void write_prototype_list( )
{
    SHIP_PROTOTYPE *prototype;
    FILE *fpout;
    char filename[256];
    
    sprintf( filename, "%s%s", PROTOTYPE_DIR, PROTOTYPE_LIST );
    fpout = fopen( filename, "w" );
    if ( !fpout )
    {
         bug( "FATAL: cannot open protoytpe.lst for writing!\n\r", 0 );
         return;
    }
    for ( prototype = first_ship_prototype; prototype; prototype = prototype->next )
    fprintf( fpout, "%s\n", prototype->filename );
    fprintf( fpout, "$\n" );
    fclose( fpout );
}
                                                                    
SHIP_PROTOTYPE *get_ship_prototype( char *name )
{
    SHIP_PROTOTYPE *prototype;
    
    for ( prototype = first_ship_prototype; prototype; prototype = prototype->next )
       if ( !str_cmp( name, prototype->name ) )
         return prototype;
    
    for ( prototype = first_ship_prototype; prototype; prototype = prototype->next )
       if ( nifty_is_name_prefix( name, prototype->name ) )
         return prototype;
    
    return NULL;
}

void save_ship_prototype( SHIP_PROTOTYPE *prototype )
{
	FILE *fp;
	char filename[256];
	char buf[MAX_STRING_LENGTH];

	if ( !prototype )
	{
		bug( "save_ship_prototype: null prototype pointer!", 0 );
		return;
	}
	
	if ( !prototype->filename || prototype->filename[0] == '\0' )
	{
		sprintf( buf, "save_ship_prototype: %s has no filename", prototype->name );
		bug( buf, 0 );
		return;
	}
	
	sprintf( filename, "%s%s", PROTOTYPE_DIR, prototype->filename );
	
	fclose( fpReserve );
	if ( ( fp = fopen( filename, "w" ) ) == NULL )
	{
		bug( "save_ship_prototype: fopen", 0 );
		perror( filename );
	}
	else
	{
		fprintf( fp, "#PROTOTYPE\n" );
		fprintf( fp, "Name         %s~\n",	prototype->name );
		fprintf( fp, "Filename     %s~\n",	prototype->filename );
		fprintf( fp, "Description  %s~\n",	prototype->description );
		fprintf( fp, "Class        %d\n",	prototype->class );
		fprintf( fp, "Tractorbeam  %d\n",	prototype->tractorbeam );
		fprintf( fp, "Lasers       %d\n",	prototype->lasers );
		fprintf( fp, "Maxmissiles  %d\n",	prototype->maxmissiles );
		fprintf( fp, "Maxshield    %d\n",	prototype->maxshield );
		fprintf( fp, "Maxhull      %d\n",	prototype->maxhull );
		fprintf( fp, "Maxenergy    %d\n",	prototype->maxenergy );
		fprintf( fp, "Hyperspeed   %d\n",	prototype->hyperspeed );
		fprintf( fp, "Maxchaff     %d\n",	prototype->maxchaff );
		fprintf( fp, "Realspeed    %d\n",	prototype->realspeed );
		fprintf( fp, "Manuever     %d\n",   prototype->manuever );
		/* Unknown Regions Additions - Gavin - Mar 28 2001 */
		fprintf( fp, "Maxbombs     %d\n",   prototype->maxbombs );
		fprintf( fp, "Comm         %d\n",   prototype->comm );
		fprintf( fp, "Sensor       %d\n",   prototype->sensor );
		fprintf( fp, "Astro_array  %d\n",   prototype->astro_array );
		fprintf( fp, "Ion          %d\n",   prototype->ion );

		fprintf( fp, "End\n\n"						);
	fprintf( fp, "#END\n"						);
    }
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


/*
 * Read in actual prototype data.
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

void fread_ship_prototype( SHIP_PROTOTYPE *prototype, FILE *fp )
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
		KEY( "Astro_array",   prototype->astro_array, fread_number(fp) );
	break;
        
	case 'C':
		KEY( "Class",       prototype->class,            fread_number( fp ) );
		KEY( "Comm",       prototype->comm,            fread_number( fp ) );
	break;
                                

	case 'D':
	    KEY( "Description",	prototype->description,	fread_string( fp ) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
			if (!prototype->name)
				prototype->name		= STRALLOC( "" );
			if (!prototype->description)
				prototype->description 	= STRALLOC( "" );
			prototype->count = 0;
			return;
	    }
	    break;
	    
	case 'F':
	    KEY( "Filename",	prototype->filename,		fread_string_nohash( fp ) );
            break;
        
        case 'H':
            KEY( "Hyperspeed",   prototype->hyperspeed,       fread_number( fp ) );
            break;

	case 'I':
		KEY( "Ion", prototype->ion, fread_number(fp) );
	break;


        case 'L':
            KEY( "Lasers",   prototype->lasers,      fread_number( fp ) );
            break;

        case 'M':
            KEY( "Manuever",   prototype->manuever,      fread_number( fp ) );
            KEY( "Maxenergy",      prototype->maxenergy,        fread_number( fp ) );
            KEY( "Maxbombs",      prototype->maxbombs,        fread_number( fp ) );
            KEY( "Maxmissiles",   prototype->maxmissiles,      fread_number( fp ) );
            KEY( "Maxshield",      prototype->maxshield,        fread_number( fp ) );
            KEY( "Maxhull",      prototype->maxhull,        fread_number( fp ) );
            KEY( "Maxchaff",       prototype->maxchaff,      fread_number( fp ) );
             break;

	case 'N':
	    KEY( "Name",	prototype->name,		fread_string( fp ) );
            break;
  
        case 'R':
            KEY( "Realspeed",   prototype->realspeed,       fread_number( fp ) );
            break;

	case 'S':
		KEY( "Sensor", prototype->sensor, fread_number(fp) );
		break;
       
	case 'T':
	    KEY( "Tractorbeam", prototype->tractorbeam,      fread_number( fp ) );
	    break;
	}
	
	if ( !fMatch )
	{
	    sprintf( buf, "Fread_ship_prototype: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

/*
 * Load a prototype file
 */

bool load_ship_prototype( char *prototypefile )
{
    char filename[256];
    SHIP_PROTOTYPE *prototype;
    FILE *fp;
    bool found;
        
    CREATE( prototype, SHIP_PROTOTYPE, 1 );

    found = FALSE;
    sprintf( filename, "%s%s", PROTOTYPE_DIR, prototypefile );

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
		bug( "Load_ship_prototype: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if ( !str_cmp( word, "PROTOTYPE"	) )
	    {
	    	fread_ship_prototype( prototype, fp );
	    	break;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
		char buf[MAX_STRING_LENGTH];

		sprintf( buf, "Load_ship_prototype: bad section: %s.", word );
		bug( buf, 0 );
		break;
	    }
	}
	fclose( fp );
    }
    if ( !(found) )
      DISPOSE( prototype );
    else
    {      
       LINK( prototype, first_ship_prototype, last_ship_prototype, next, prev );
    }
    
    return found;
}

/*
 * Load in all the prototype files.
 */
void load_prototypes( )
{
    FILE *fpList;
    char *filename;
    char prototypelist[256];
    char buf[MAX_STRING_LENGTH];
    
    
    first_ship_prototype	= NULL;
    last_ship_prototype	= NULL;
    
    log_string( "Loading ship prototypes..." );

    sprintf( prototypelist, "%s%s", PROTOTYPE_DIR, PROTOTYPE_LIST );
    fclose( fpReserve );
    if ( ( fpList = fopen( prototypelist, "r" ) ) == NULL )
    {
	perror( prototypelist );
	exit( 1 );
    }

    for ( ; ; )
    {
    
	filename = feof( fpList ) ? "$" : fread_word( fpList );

	if ( filename[0] == '$' )
	  break;
	         
	if ( !load_ship_prototype( filename ) )
	{
	  sprintf( buf, "Cannot load ship prototype file: %s", filename );
	  bug( buf, 0 );
	}

    }
    fclose( fpList );
    log_string(" Done ship prototypes " );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void do_setprototype( CHAR_DATA *ch, char *argument )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	SHIP_PROTOTYPE *prototype;
	
	if ( IS_NPC(ch) || !ch->pcdata )
		return;
	
	if ( !ch->desc )
		return;
	
	switch( ch->substate )
	{
	default:
		break;
	case SUB_SHIPDESC:
		prototype = ch->dest_buf;
		if ( !prototype )
		{
			bug( "setprototype: sub_shipdesc: NULL ch->dest_buf", 0 );
			stop_editing( ch );
			ch->substate = ch->tempnum;
			send_to_char( "&RError: prototype lost.\n\r" , ch);
			return;
		}
		STRFREE( prototype->description );
		prototype->description = copy_buffer( ch );
		stop_editing( ch );
		ch->substate = ch->tempnum;
		save_ship_prototype( prototype );
		return;
	}
    argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg1[0] == '\0' )
	{
		send_to_char( "Usage: setprototype <prototype> <field> <values>\n\r"
			"\n\rField being one of:\n\r"
			"  filename name description class model\n\r"
			"  manuever speed hyperspeed tractorbeam\n\r"
			"  lasers missiles shield hull energy chaff\n\r"
			"  bombs comm sensor astro_array ion\n\r" , ch );
		return;
	}
	
	prototype = get_ship_prototype( arg1 );
	if ( !prototype )
	{
		send_to_char( "No such ship prototype.\n\r", ch );
		return;
	}
	
	if ( !str_cmp( arg2, "name" ) )
	{
		STRFREE( prototype->name );
		prototype->name = STRALLOC( argument );
		send_to_char( "Done.\n\r", ch );
		save_ship_prototype( prototype );
		return;
	}
    if ( !str_cmp( arg2, "filename" ) )
    {
        char filename[256];
        if ( prototype->filename && prototype->filename[0] != '\0' )
        {
           sprintf( filename, "%s%s", PROTOTYPE_DIR, prototype->filename );
           remove( filename );      
           DISPOSE( prototype->filename );
        }
	prototype->filename = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	save_ship_prototype( prototype );
	write_prototype_list( );
	return;
    }

	if ( !str_cmp( arg2, "description" ) )
	{
		ch->substate = SUB_SHIPDESC;
		ch->dest_buf = prototype;
		start_editing( ch, prototype->description );
		return;
	}
    if ( !str_cmp( arg2, "manuever" ) )
	{   
		prototype->manuever = URANGE( 0, atoi(argument) , 120 );
		send_to_char( "Done.\n\r", ch );
		save_ship_prototype( prototype );
		return;
	}
    
	if ( !str_cmp( arg2, "lasers" ) )
	{   
		prototype->lasers = URANGE( 0, atoi(argument) , 10 );
		send_to_char( "Done.\n\r", ch );
		save_ship_prototype( prototype );
		return;
	}
	
	if ( !str_cmp( arg2, "class" ) )
	{   
		prototype->class = URANGE( 0, atoi(argument) , 9 );
		send_to_char( "Done.\n\r", ch );
		save_ship_prototype( prototype );
		return;
	}
	
	if ( !str_cmp( arg2, "missiles" ) )
	{   
		prototype->maxmissiles = URANGE( 0, atoi(argument) , 255 );
		send_to_char( "Done.\n\r", ch );
		save_ship_prototype( prototype );
		return;
	}
	
	if ( !str_cmp( arg2, "speed" ) )
	{   
		prototype->realspeed = URANGE( 0, atoi(argument) , 150 );
		send_to_char( "Done.\n\r", ch );
		save_ship_prototype( prototype );
		return;
	}
	
	if ( !str_cmp( arg2, "tractorbeam" ) )
	{   
		prototype->tractorbeam = URANGE( 0, atoi(argument) , 255 );
		send_to_char( "Done.\n\r", ch );
		save_ship_prototype( prototype );
		return;
	}

	if ( !str_cmp( arg2, "hyperspeed" ) )
	{   
		prototype->hyperspeed = URANGE( 0, atoi(argument) , 255 );
		send_to_char( "Done.\n\r", ch );
		save_ship_prototype( prototype );
		return;
	}
	
	if ( !str_cmp( arg2, "shield" ) )
	{   
		prototype->maxshield = URANGE( 0, atoi(argument) , 1000 );
		send_to_char( "Done.\n\r", ch );
		save_ship_prototype( prototype );
		return;
	}
	
	if ( !str_cmp( arg2, "hull" ) )
	{   
		prototype->maxhull = URANGE( 1, atoi(argument) , 20000 );
		send_to_char( "Done.\n\r", ch );
		save_ship_prototype( prototype );
		return;
	}
	
	if ( !str_cmp( arg2, "energy" ) )
	{   
		prototype->maxenergy = URANGE( 1, atoi(argument) , 30000 );
		send_to_char( "Done.\n\r", ch );
		save_ship_prototype( prototype );
		return;
	}

	if ( !str_cmp( arg2, "chaff" ) )
	{
		prototype->maxchaff = URANGE( 0, atoi(argument) , 25 );
		send_to_char( "Done.\n\r", ch );
		save_ship_prototype( prototype );
		return;
	}

	if ( !str_cmp( arg2, "bombs" ) )
	{
		prototype->maxbombs = URANGE( 0, atoi(argument) , 25 );
		send_to_char( "Done.\n\r", ch );
		save_ship_prototype( prototype );
		return;
	}

	if ( !str_cmp( arg2, "comm" ) )
	{
		prototype->comm = URANGE( 0, atoi(argument) , 25 );
		send_to_char( "Done.\n\r", ch );
		save_ship_prototype( prototype );
		return;
	}

	if ( !str_cmp( arg2, "sensor" ) )
	{
		prototype->sensor = URANGE( 0, atoi(argument) , 25 );
		send_to_char( "Done.\n\r", ch );
		save_ship_prototype( prototype );
		return;
	}

	if ( !str_cmp( arg2, "astro_array" ) )
	{
		prototype->astro_array = URANGE( 0, atoi(argument) , 25 );
		send_to_char( "Done.\n\r", ch );
		save_ship_prototype( prototype );
		return;
	}

	if ( !str_cmp( arg2, "ion" ) )
	{
		prototype->ion = URANGE( 0, atoi(argument) , 10 );
		send_to_char( "Done.\n\r", ch );
		save_ship_prototype( prototype );
		return;
	}

	do_setprototype( ch, "" );
	return;
}

void do_showprototype( CHAR_DATA *ch, char *argument )
{
	SHIP_PROTOTYPE *prototype;
	
	if ( IS_NPC( ch ) )
	{
		send_to_char( "Huh?\n\r", ch );
		return;
	}
	
	if ( argument[0] == '\0' )
	{
		send_to_char( "Usage: showprototype <prototype>\n\r", ch );
		return;
	}
    
	prototype = get_ship_prototype( argument );
	if ( !prototype )
	{
		send_to_char( "No such prototype.\n\r", ch );
		return;
	}
	set_char_color( AT_YELLOW, ch );
	ch_printf( ch, "%s : %s\n\rFilename: %s\n\r",
		prototype->class == FIGHTER_SHIP ? "Fighter" :
	(prototype->class == MIDSIZE_SHIP ? "Midsized Ship" :
	(prototype->class == CAPITAL_SHIP ? "Capital Ship" :
	(prototype->class == SHIP_PLATFORM ? "Platform" : "Vehicle" ) ) ),
		prototype->name, prototype->filename);
	ch_printf( ch, "Description: %s\n\r",
		prototype->description);
	ch_printf( ch, "Number of ships with this prototype: %d\n\r", prototype->count);
	ch_printf( ch, "Tractor Beam: %d  ",
		prototype->tractorbeam);
	ch_printf( ch, "Lasers: %d     ",
		prototype->lasers );
	ch_printf( ch, "Missiles: %d\n\r",
		prototype->maxmissiles);
	ch_printf( ch, "Hull: %d     ",
		prototype->maxhull);
	ch_printf( ch, "Shields: %d   Energy(fuel): %d   Chaff: %d\n\r",
		prototype->maxshield, prototype->maxenergy, prototype->maxchaff);
	ch_printf( ch, "Speed: %d    Hyperspeed: %d   Manueverability: %d\n\r",
		prototype->realspeed, prototype->hyperspeed , prototype->manuever );
	ch_printf( ch, "Bombs: %d    Comm: %d    Sensor: %d    Astro_array: %d\n\r"
		"Ions: %d    Class: %d\n\r", prototype->maxbombs, prototype->comm, prototype->sensor,
		prototype->astro_array, prototype->ion, prototype->class );
	/* Add # of ships with that prototype. */
	return;
}

void do_prototypes( CHAR_DATA *ch, char *argument )
{
	SHIP_PROTOTYPE *prototype;
	
	if ( argument[0] == '\0' )
	{
		int count;
		count = 0;
		send_to_pager( "&Y\n\rThe are prototyped ships:&W\n\r", ch );
		
		for ( prototype = first_ship_prototype; prototype; prototype = prototype->next )
		{		
			pager_printf( ch, "%-35s    ", prototype->name );
			pager_printf( ch, "%ld to buy.\n\r", get_prototype_value(prototype) ); 	
			count++;
		}
		
		if ( !count )
		{
			send_to_pager( "There are no vehicle prototypes currently formed.\n\r", ch );
			return;
		}
	}
	else
	{
		if ( IS_NPC( ch ) )
		{
			send_to_char( "Huh?\n\r", ch );
			return;
		}
		
		prototype = get_ship_prototype( argument );
		if ( !prototype )
		{
			send_to_char( "No such prototype.\n\r", ch );
			return;
		}
		return;
	}
	return;
}

long int get_prototype_value( SHIP_PROTOTYPE *prototype )
{
     long int price;
          

	if ( prototype->class == FIGHTER_SHIP )
		price = 5000;
	if ( prototype->class == MIDSIZE_SHIP )
		price = 7000;
	if ( prototype->class == CAPITAL_SHIP )
		price = 10000;
	if ( prototype->class == SHIP_PLATFORM )
		price = 15000;
	else 
		price = 2000;
     
     price += ( prototype->tractorbeam * 100 );
     price += ( prototype->realspeed * 10 );
     price += ( 5 * prototype->maxhull );
     price += ( 2 * prototype->maxenergy );
     price += ( 100 * prototype->maxchaff );
               
     if (prototype->maxenergy > 5000 )
          price += ( (prototype->maxenergy-5000)*20 ) ;
     
     if (prototype->maxenergy > 10000 )
          price += ( (prototype->maxenergy-10000)*50 );
     
     if (prototype->maxhull > 1000)
        price += ( (prototype->maxhull-1000)*10 );
     
     if (prototype->maxhull > 10000)
        price += ( (prototype->maxhull-10000)*20 );
        
     if (prototype->maxshield > 200)
          price += ( (prototype->maxshield-200)*50 );
     
     if (prototype->maxshield > 1000)
          price += ( (prototype->maxshield-1000)*100 );
     
     if (prototype->realspeed > 100 )
        price += ( (prototype->realspeed-100)*500 ) ;
        
     if (prototype->lasers > 5 )
        price += ( (prototype->lasers-5)*500 );
      
     if (prototype->maxshield)
     	price += ( 1000 + 10 * prototype->maxshield);
     
     if (prototype->lasers)
     	price += ( 500 + 500 * prototype->lasers );
    
     if (prototype->maxmissiles)
     	price += ( 1000 + 250 * prototype->maxmissiles );
     
     if (prototype->hyperspeed)
        price += ( 1000 + prototype->hyperspeed * 10 );
     
     /*price *= (int) ( 1 + prototype->model/3 );*/
     
     return price;
                    
}

void do_makeprototype( CHAR_DATA *ch, char *argument )
{
    SHIP_PROTOTYPE *prototype;
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage: makeprototype <filename> <prototype name>\n\r", ch );
	return;
    }

	CREATE( prototype, SHIP_PROTOTYPE, 1 );
	LINK( prototype, first_ship_prototype, last_ship_prototype, next, prev );

	prototype->name = STRALLOC(argument);
	prototype->filename = str_dup( arg );
	prototype->description = STRALLOC( "" );
	prototype->class = 0;
	prototype->tractorbeam = 0;
	prototype->lasers = 0;
	prototype->maxmissiles = 0;
	prototype->maxshield = 0;
	prototype->maxhull = 0;
	prototype->maxenergy = 0;
	prototype->hyperspeed = 0;
	prototype->maxchaff = 0;
	prototype->realspeed = 0;
	prototype->manuever = 0;
	prototype->maxbombs = 0;
	prototype->comm = 0;
	prototype->sensor = 0;
	prototype->astro_array = 0;
	prototype->ion = 0;
	save_ship_prototype( prototype );
    write_prototype_list( );
}
