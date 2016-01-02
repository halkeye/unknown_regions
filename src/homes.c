#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "mud.h"


HOME_DATA * first_home;
HOME_DATA * last_home;
static	OBJ_DATA *	rgObjNest	[MAX_NEST];
int file_ver;

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				      field = value;			\
				      fMatch = TRUE;			\
				      break;				\
				}

char *	const	home_flags	[] =
{
"basic","r1","r2","r3","r4","r5","r6","r7","r8",
"r9","r10","r11", "r12", "r13", "r14","r15","r16",
"r17","r18", "r19", "r20", "r21", "r22","r23","r24",
"r25", "r26", "r27", "r28", "r29", "r30", "r31" , "r32",
"r33","r34"
};

HOME_DATA *home_from_entrance( int vnum )
{
	HOME_DATA *home;
	
	for ( home = first_home; home; home = home->next )
	{
		if ( vnum == home->entrance )
			return home;
	}
	return NULL;
}

/*
 * Get pointer to home structure from the home vnun.
 */
HOME_DATA *home_from_vnum( int vnum )
{
    HOME_DATA *home;
    
    for ( home = first_home; home; home = home->next )
	{
       if ( vnum >= home->firstroom && vnum <= home->lastroom )
            return home;
	}
    return NULL;
}

HOME_DATA *get_home( char *name )
{
	HOME_DATA *home;

    for ( home = first_home; home; home = home->next )
		if ( !str_cmp( name, home->name ) )
			return home;
    
    for ( home = first_home; home; home = home->next )
		if ( nifty_is_name( name, home->name ) )
			return home;

    for ( home = first_home; home; home = home->next )
		if ( !str_prefix( name, home->name ) )
			return home;
    
    for ( home = first_home; home; home = home->next )
		if ( nifty_is_name_prefix( name, home->name ) )
			return home;
	
	return NULL;
}



int get_homeflag( char *type )
{
	int x;

    for ( x = 0; x < (sizeof(home_flags) / sizeof(home_flags[0]) ); x++ )
      if ( !str_cmp( type, home_flags[x] ) )
        return x;
    return -1;
}

void fread_home( HOME_DATA *home, FILE *fp )
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
			
			case 'C':
				KEY( "Class",       home->class,            fread_number( fp ) );
				KEY( "Copilot",     home->copilot,          fread_string( fp ) );
				break;
			
			case 'D':
				KEY( "Description",	home->description,	fread_string( fp ) );
				break;
				
			case 'E':
				if ( !str_cmp( word, "End" ) )
				{
					if (!home->flags)
						home->flags = 0;
					if (!home->name)
						home->name		= STRALLOC( home->filename );
					if (!home->origname)
						home->origname	= STRALLOC( home->name );
					if (!home->owner)
						home->owner			= STRALLOC( "Unowned" );
					if (!home->copilot)
						home->copilot 	= STRALLOC( "" );
					if (!home->pilot)
						home->pilot   	= STRALLOC( "" );  		
					if ( home->firstroom && !home->entrance)
						home->entrance = home->firstroom;
					home->in_room=NULL;
					home->next_in_room=NULL;
					home->prev_in_room=NULL;
					return;
				}
				KEY( "Entrance",   home->entrance,        fread_number( fp ) );
				break;
				
			case 'F':
				KEY( "Filename",	home->filename,		fread_string_nohash( fp ) );
				KEY( "Firstroom",   home->firstroom,        fread_number( fp ) );
				KEY( "Flags",       home->flags,			fread_number( fp ) );
				break;
			
			case 'L':
				KEY( "Lastroom",   home->lastroom,        fread_number( fp ) );
				break;
				
			case 'N':
				KEY( "Name",	home->name,		fread_string( fp ) );
				break;
			
			case 'O':
				KEY( "OrigName",		 home->origname,       fread_string( fp ) );
				KEY( "Owner",	home->owner,		fread_string( fp ) );
				break;
				
			case 'P':	
				KEY( "Pilot",            home->pilot,            fread_string( fp ) ); 
				KEY( "Price",            home->price,            fread_number( fp ) ); 
				break;
			
			case 'T':
				KEY( "Type",		home->type,		fread_number( fp ) );
				break;
		}
		
		if ( !fMatch )
		{
			sprintf( buf, "Fread_home: no match: %s", word );
			bug( buf, 0 );
		}
	}
}

/*
 * Load in all the home files.
 */
void load_homes( )
{
    FILE *fpList;
    char *filename;
    char homelist[256];
    char buf[MAX_STRING_LENGTH];
    first_home	= NULL;
    last_home	= NULL;
    
    log_string( "Loading homes..." );

    sprintf( homelist, "%s%s", HOMEDIR, HOME_LIST );
    fclose( fpReserve );

    if ( ( fpList = fopen( homelist, "r" ) ) == NULL )
    {
	perror( homelist );
	exit( 1 );
    }

    for ( ; ; )
    {
    
	filename = feof( fpList ) ? "$" : fread_word( fpList );

	if ( filename[0] == '$' )
	  break;
	         
	if ( !load_home_file( filename ) )
	{
	  sprintf( buf, "Cannot load home file: %s", filename );
	  bug( buf, 0 );
	}

    }
    fclose( fpList );
    log_string(" Done homes " );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

/*
 * Load a home file
 */

bool load_home_file( char *homefile )
{
    char filename[256];
    HOME_DATA *home;
    FILE *fp;
    bool found;
        
    CREATE( home, HOME_DATA, 1 );

    found = FALSE;
    sprintf( filename, "%s%s", HOMEDIR, homefile );

    if ( ( fp = fopen( filename, "r" ) ) != NULL )
    {

		found = TRUE;
		for ( ; ; )
		{
			char letter;
			char *word;
			int iNest;

			for ( iNest = 0; iNest < MAX_NEST; iNest++ )
				rgObjNest[iNest] = NULL;

			letter = fread_letter( fp );
			if ( letter == '*' )
			{
				fread_to_eol( fp );
				continue;
			}

			if ( letter != '#' )
			{
				bug( "Load_home_file: # not found.", 0 );
				break;
			}

			word = fread_word( fp );
			if ( !str_cmp( word, "HOME"	) )
				fread_home( home, fp );
			else if ( !str_cmp( word, "OBJECT" ) )
				fread_obj_home( home, fp, OS_CARRY);
			else if ( !str_cmp( word, "CORPSE" ) )
				fread_obj_home( home, fp, OS_CORPSE);
			
			else if ( !str_cmp( word, "END"	) )	       
				break;
			
			else
			{
				char buf[MAX_STRING_LENGTH];

				sprintf( buf, "Load_home_file: bad section: %s.", word );
				bug( buf, 0 );
				break;
			}
		}
		fclose( fp );
    }
    if ( !(found) )
		DISPOSE( home );
    else
    {      
		LINK( home, first_home, last_home, next, prev );
    }
    
    return found;
}

void save_home2( HOME_DATA *home )
{
    FILE *fp;
    char filename[256];
    char buf[MAX_STRING_LENGTH];
	OBJ_DATA *contents;

    if ( !home || home == NULL )
    {
		bug( "save_home2: null home pointer!", 0 );
		return;
    }
        
    if ( !home->filename || home->filename[0] == '\0' )
    {
		sprintf( buf, "save_home2: %s has no filename", home->name );
		bug( buf, 0 );
		return;
	}
 
	sprintf( filename, "%s%s", HOMEDIR, home->filename );
    
	fclose( fpReserve );
    if ( ( fp = fopen( filename, "w" ) ) == NULL )
    {
    	bug( "save_home2: fopen", 0 );
    	perror( filename );
    }
    else
    {
		fprintf( fp, "#HOME\n" );
		fprintf( fp, "Name         %s~\n",	home->name		    );
		fprintf( fp, "Filename     %s~\n",	home->filename		);
		fprintf( fp, "Description  %s~\n",	home->description	);
		fprintf( fp, "Owner        %s~\n",	home->owner			);
		fprintf( fp, "Pilot        %s~\n",      home->pilot     );
		fprintf( fp, "Copilot      %s~\n",      home->copilot   );
		fprintf( fp, "Class        %d\n",	home->class			);
		fprintf( fp, "Firstroom    %d\n",	home->firstroom		);
		fprintf( fp, "Lastroom     %d\n",	home->lastroom		);
		fprintf( fp, "Type         %d\n",	home->type			);
		fprintf( fp, "Flags        %d\n",	home->flags    );
		fprintf( fp, "Price        %ld\n", home->price );
		fprintf( fp, "Entrance    %d\n",  home->entrance );
		fprintf( fp, "End\n\n"						);
		{
			int room;
			ROOM_INDEX_DATA *pRoomIndex;
			if ( home->firstroom == home->lastroom )
			{
				room = home->firstroom;
				if ( (pRoomIndex = get_room_index(room)) == NULL )
				{
					sprintf( buf, "save_home2: 'OBJ': bad room vnum %d.", room );
					bug ( buf, 0 );
				}
				else 
				{
					contents = pRoomIndex->last_content;
					if (contents)
						fwrite_obj_home(supermob, contents, fp, 0, OS_CORPSE );
					/*if (contents)
						fwrite_obj_home(supermob, contents, fp, 0, OS_CARRY );*/
				}
			}
			else
			{
				for ( room = home->firstroom; room <= home->lastroom; room++ )
				{
					if ( (pRoomIndex = get_room_index(room)) == NULL )
					{
						sprintf( buf, "save_home2: 'OBJ': bad room vnum %d.", room );
						bug ( buf, 0 );
						continue;
					}
					else 
					{
						contents = pRoomIndex->last_content;
						if (contents)
							fwrite_obj_home(supermob, contents, fp, 0, OS_CORPSE );
						/*if (contents)
							fwrite_obj_home(supermob, contents, fp, 0, OS_CARRY );*/
					}
				}
			}
		}
		fprintf( fp, "#END\n"						);
	}
	fclose( fp );
	fpReserve = fopen( NULL_FILE, "r" );
    return;
}


void do_sethome( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
    HOME_DATA *home;
    int  tempnum,value;
    ROOM_INDEX_DATA *roomindex;
    
    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
	strcpy( arg3, argument );

    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

    if ( atoi(arg3) < -1 && value == -1 )
      value = atoi(arg3);


    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg1[0] == '\0' )
    {
	send_to_char( "Usage: sethome <home> <field> <values>\n\r", ch );
	send_to_char( "\n\rField being one of:\n\r", ch );
	send_to_char( "filename name owner copilot pilot description \n\r", ch );
	send_to_char( "firstroom lastroom entrance\n\r", ch );
	return;
    }

    home = get_home( arg1 );
    if ( !home )
	{
		send_to_char( "No such home.\n\r", ch );
		return;
    }
    
    if ( !str_cmp( arg2, "owner" ) )
	{
		STRFREE( home->owner );
		home->owner = STRALLOC( argument );
		send_to_char( "Done.\n\r", ch );
		save_home2( home );
		return;
	}
	
	if ( !str_cmp( arg2, "pilot" ) )
	{
		STRFREE( home->pilot );
		home->pilot = STRALLOC( argument );
		send_to_char( "Done.\n\r", ch );
		save_home2( home );
		return;
	}

	if ( !str_cmp( arg2, "copilot" ) )
	{
		STRFREE( home->copilot );
		home->copilot = STRALLOC( argument );
		send_to_char( "Done.\n\r", ch );
		save_home2( home );
		return;
	}
	if ( !str_cmp( arg2, "firstroom" ) )
	{   
		tempnum = atoi(argument); 
		roomindex = get_room_index(tempnum);
		if (roomindex == NULL)
		{
			send_to_char("That room doesn't exist.\n\r",ch);
			return;
		} 
		home->firstroom = tempnum;
		home->lastroom = tempnum;
		send_to_char( "You will now need to set the other rooms in the home.\n\r", ch );
		save_home2( home );
		return;
	}
	
	if ( !str_cmp( arg2, "lastroom" ) )
	{   
		tempnum = atoi(argument); 
		roomindex = get_room_index(tempnum);
		if (roomindex == NULL)
		{
			send_to_char("That room doesn't exist.\n\r",ch);
			return;
		}
		if ( tempnum < home->firstroom )
		{
			send_to_char("The last room on a home must be greater than or equal to the first room.\n\r",ch);
			return;
    	}
		home->lastroom = tempnum;
		send_to_char( "Done.\n\r", ch );
		save_home2( home );
		return;
	}
	
	if ( !str_cmp( arg2, "entrance" ) )
	{   
		tempnum = atoi(argument); 
		roomindex = get_room_index(tempnum);
		if (roomindex == NULL)
		{
			send_to_char("That room doesn't exist.\n\r",ch);
			return;
		}
		if ( tempnum < home->firstroom || tempnum > home->lastroom )
		{
			send_to_char("The entrance must be between the first and last room.\n\r",ch);
			return;
    	}
		home->entrance = tempnum;
		send_to_char( "Done.\n\r", ch );
		save_home2( home );
		return;
	}

    if ( !str_cmp( arg2, "type" ) )
    {
	if ( !str_cmp( argument, "mob" ) )
	  home->type = MOB_HOME;
	else
	{
	   send_to_char( "home type must be either: hapan, imperial, civilian or mob.\n\r", ch );
	   return;
	}
	send_to_char( "Done.\n\r", ch );
	save_home2( home );
	return;
    }

    if ( !str_cmp( arg2, "name" ) )
    {
	STRFREE( home->name );
	home->name = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_home2( home );
	return;
    }

    if ( !str_cmp( arg2, "filename" ) )
    {
	DISPOSE( home->filename );
	home->filename = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	save_home2( home );
	write_home_list( );
	return;
    }
 
    if ( !str_cmp( arg2, "desc" ) )
    {
	STRFREE( home->description );
	home->description = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_home2( home );
	return;
    }


	if ( !str_cmp( arg2, "class" ) )
    {   
		if ( !str_cmp( argument, "apartment" ) )
			home->class = APARTMENT_HOME;
		else if ( !str_cmp( argument, "midsize" ) )
			home->class = MIDSIZE_HOME;
		else if ( !str_cmp( argument, "giant" ) )
			home->class = GIANT_HOME;
		else
		{
			send_to_char( "home class must be either: aparment, midsize or giant.\n\r", ch );
			return;
		}
		send_to_char( "Done.\n\r", ch );
		save_home2( home );
		return;
    }
	
	if ( !str_cmp( arg2, "flags" ) )
    {

	if ( !argument || argument[0] == '\0' )
	{
	   send_to_char( "Usage: sethome <home> flags <flag> [flag]...\n\r", ch );
	   send_to_char( "canflyindoors\n\r", ch );
	   return;
	}
	while ( argument[0] != '\0' )
	{
	   argument = one_argument( argument, arg3 );
	   value = get_homeflag( arg3 );
	   if ( value < 0 || value > 31 )
	     ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
	   else
	   {
	       TOGGLE_BIT(home->flags, 1 << value);
	   }
	}
	send_to_char( "Done.\n\r", ch );
	save_home2( home );
	return;
    }

	if ( !str_cmp( arg2, "price" ) ) 
	{
		long int price;
		price = atoi(arg3);
		if (price > 0) {
			home->price = price;
			ch_printf(ch,"%s is now %ld gold",home->name, home->price);
		}
		else {
			send_to_char("Bad Price",ch);
			return;
		}
		return;
	}
    do_sethome( ch, "" );
    return;
}

void do_showhome( CHAR_DATA *ch, char *argument )
{   
    HOME_DATA *home;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Usage: showhome <home>\n\r", ch );
	return;
    }

    home = get_home( argument );
    if ( !home )
    {
	send_to_char( "No such home\n\r", ch );
	return;
    }
    set_char_color( AT_YELLOW, ch );
    ch_printf( ch, "%s %s : %s\n\rFilename: %s\n\r",
		home->type == PLAYER_HOME ? "PlayerHome" : (home->type == EMPTY_HOME ? "Emptyhome" : "Mob" ),
		home->class == APARTMENT_HOME ? "Apartment" :(home->class == MIDSIZE_HOME ? "Regular Home" : "Unknown" ), 
		home->name,	home->filename);
    ch_printf( ch, "Description: %s\n\rOwner: %s   Pilot: %s   Copilot: %s\n\r",
    			home->description,
    			home->owner, home->pilot,  home->copilot );
	ch_printf( ch, "Firstroom: %d   Lastroom: %d   Entrance: %d\n\r",
    			home->firstroom,
    			home->lastroom,
				home->entrance);
    if (get_trust(ch) == MAX_LEVEL)
		ch_printf( ch, "Home Flags: %s\n\r", flag_string(home->flags, home_flags));
	
    return;
}

void do_makehome( CHAR_DATA *ch, char *argument )
{
    HOME_DATA *home;
    char arg[MAX_INPUT_LENGTH];
    
    argument = one_argument( argument, arg );
    
    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage: makehome <filename> <home name>\n\r", ch );
	return;
    }

    CREATE( home, HOME_DATA, 1 );
    LINK( home, first_home, last_home, next, prev );

    home->name		= STRALLOC( argument );
	home->origname		= STRALLOC( argument );
    home->description	= STRALLOC( "" );
    home->owner 	= STRALLOC( "Unowned" );
    home->copilot       = STRALLOC( "" );
    home->pilot         = STRALLOC( "" );
    home->type          = EMPTY_HOME;
	home->firstroom		= 100;
	home->lastroom		= 100;
	home->entrance		= 100;
    home->in_room=NULL;
    home->next_in_room=NULL;
    home->prev_in_room=NULL;
    home->filename = str_dup( arg );
    save_home2( home );
    write_home_list( );
	
}


void do_homes( CHAR_DATA *ch, char *argument )
{
    HOME_DATA *home;
	ROOM_INDEX_DATA *room; 
    int count;
    
    if ( !IS_NPC(ch) )
    {
      count = 0;
      send_to_char( "&YThe following homes are owned by you or by your organization:\n\r", ch );
      send_to_char( "\n\r&WHome                               Owner\n\r",ch);
      for ( home = first_home; home; home = home->next )
      {   
		  room = get_room_index(home->firstroom);
		  set_char_color( AT_BLUE, ch );

		  if ( !str_cmp(home->owner, ch->name) )
		  {
			  if  ( room->area->planet )
				  ch_printf( ch, "&R[&WOwner&R]&B%-35s     %s.\n\r",home->name, room->area->planet->name);
			  else
				  ch_printf( ch, "&R[&WOwner&R]&B%-35s     %s.\n\r",home->name,"somewhere");
		  }
		  else if ( !str_cmp(home->pilot, ch->name) || !str_cmp(home->copilot, ch->name) )
		  {
			  if  ( room->area->planet )
				  ch_printf( ch, "&R[&WMember&R]&B%-35s     %s.\n\r",home->name, room->area->planet->name);
			  else
				  ch_printf( ch, "&R[&WMember&R]&B%-35s     %s.\n\r",home->name, "somewhere");
		  }
		  else if ( str_cmp(home->owner,"Unowned") )
		  {
			  if  ( room->area->planet )
				  ch_printf( ch, "&R[&WOwned&R]&B%-35s      %s.\n\r", home->name, room->area->planet->name );
			  else
				  ch_printf( ch, "&R[&WOwned&R]&B%-35s      %s.\n\r", home->name, "somewhere" );
		  }
		  else if ( !str_cmp(home->owner, "Unowned") )
			  ch_printf( ch, "&R[&WUnowned&R]&B%-35s      %ld to buy.\n\r", home->name, get_home_value(home) );
		  else continue;
		  count++;
      }

      if ( !count )
      {
        send_to_char( "There are no homes owned by you.\n\r", ch );
      }    
    }
}

void write_home_list( )
{
    HOME_DATA *thome;
    FILE *fpout;
    char filename[256];

    sprintf( filename, "%s%s", HOMEDIR, HOME_LIST );
    fpout = fopen( filename, "w" );
    if ( !fpout )
    {
         bug( "FATAL: cannot open home.lst for writing!\n\r", 0 );
         return;
    }
    for ( thome = first_home; thome; thome = thome->next )
    fprintf( fpout, "%s\n", thome->filename );
    fprintf( fpout, "$\n" );
    fclose( fpout );
}

long int get_home_value( HOME_DATA *home )
{
     long int price;
       
	 if ( !home->price )
	 {
		 if (home->class == APARTMENT_HOME)
			price = 5000;
		 else if (home->class == MIDSIZE_HOME)
     		price = 50000; 
		 else if (home->class == GIANT_HOME) 
			price = 500000;
		 else 
			price = 2000;
        
		 if ( (home->lastroom - home->firstroom) < 0 )
		 {
			 price += ((home->lastroom - home->firstroom) * 2 );
		 }

		 price *= 10;
	 }
	 else
		 price = home->price;
     
     return price;
}

bool is_hotel( CHAR_DATA *ch , HOME_DATA *home )
{
   if ( !str_cmp("Public",home->owner) )
          return TRUE;
             
   return FALSE; 
}

bool check_member( CHAR_DATA *ch , HOME_DATA *home )
{

	if ( IS_NPC(ch) && home->type == MOB_HOME )
		return TRUE;
	if ( !str_cmp("Unowned", home->owner ) )
		return TRUE;
	if ( IS_NPC(ch) && IS_SET( ch->act, ACT_PET) 
		&& ch->master 
		&&  ( !str_cmp(ch->master->name,home->owner) || !str_cmp(ch->master->name,home->pilot) 
		|| !str_cmp(ch->master->name,home->copilot) || !str_cmp("Public",home->owner) ) ) 
		return TRUE;

   if ( !str_cmp(ch->name,home->owner) || !str_cmp(ch->name,home->pilot) 
   || !str_cmp(ch->name,home->copilot) || !str_cmp("Public",home->owner) )
      return TRUE;
   
   return FALSE;
}

bool extract_home( HOME_DATA *home )
{   
    ROOM_INDEX_DATA *room;
    
    if ( ( room = home->in_room ) != NULL )
    {               
        UNLINK( home, room->first_home, room->last_home, next_in_room, prev_in_room );
        home->in_room = NULL;
    }
    return TRUE;
}

bool home_to_room(HOME_DATA *home , int vnum )
{
    ROOM_INDEX_DATA *hometo;
    
    if ( (hometo=get_room_index(vnum)) == NULL )
            return FALSE;
    LINK( home, hometo->first_home, hometo->last_home, next_in_room, prev_in_room );
    home->in_room = hometo; 
    return TRUE;
}

void echo_to_home( int color , HOME_DATA *home , char *argument )
{
     int room;
     for ( room = home->firstroom ; room <= home->lastroom ;room++ )
     {
         echo_to_room( color , get_room_index(room) , argument );
     }  
     
}

void resethome( HOME_DATA *home )
{
	 if (home->origname)
		home->name = home->origname;
     
     if ( str_cmp("Public",home->owner) && home->type != MOB_HOME )
     {
        STRFREE( home->owner );
        home->owner = STRALLOC( "Unowned" );
        STRFREE( home->pilot );
        home->pilot = STRALLOC( "" );
        STRFREE( home->copilot );
        home->copilot = STRALLOC( "" );
     }
     
     save_home2(home);               
}

void do_resethome( CHAR_DATA *ch, char *argument )
{    
     HOME_DATA *home;
     
     home = get_home( argument );
     if (home == NULL)
     {
        send_to_char("&RNo such home!",ch);
        return;
     } 
     
     resethome( home );         
}

/*
 * Write an object and its contents.
 */
void fwrite_obj_home( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest,
		 sh_int os_type )
{
    EXTRA_DESCR_DATA *ed;
    AFFECT_DATA *paf;
    sh_int wear, wear_loc, x;

    if ( iNest >= MAX_NEST )
    {
	bug( "fwrite_obj: iNest hit MAX_NEST %d", iNest );
	return;
    }

    /*
     * Slick recursion to write lists backwards,
     *   so loading them will load in forwards order.
     */
    if ( obj->prev_content )
	fwrite_obj( ch, obj->prev_content, fp, iNest, OS_CORPSE );

    /*
     * Castrate storage characters.
     */
/*    if ( obj->item_type == ITEM_KEY && !IS_OBJ_STAT(obj, ITEM_CLANOBJECT ))
	return;*/

    /*
     * Catch deleted objects					-Thoric
     */
    if ( obj_extracted(obj) )
	return;

    /*
     * Do NOT save prototype items!				-Thoric
     */
    if ( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
	return;

    /* Corpse saving. -- Altrag */
    fprintf( fp, (os_type == OS_CORPSE ? "#CORPSE\n" : "#OBJECT\n") );

    if ( iNest )
	fprintf( fp, "Nest         %d\n",	iNest		     );
    if ( obj->count > 1 )
	fprintf( fp, "Count        %d\n",	obj->count	     );
    if ( QUICKMATCH( obj->name, obj->pIndexData->name ) == 0 )
	fprintf( fp, "Name         %s~\n",	obj->name	     );
    if ( QUICKMATCH( obj->short_descr, obj->pIndexData->short_descr ) == 0 )
	fprintf( fp, "ShortDescr   %s~\n",	obj->short_descr     );
    if ( QUICKMATCH( obj->description, obj->pIndexData->description ) == 0 )
	fprintf( fp, "Description  %s~\n",	obj->description     );
    if ( QUICKMATCH( obj->action_desc, obj->pIndexData->action_desc ) == 0 )
	fprintf( fp, "ActionDesc   %s~\n",	obj->action_desc     );
    fprintf( fp, "Vnum         %d\n",	obj->pIndexData->vnum	     );
    if ( os_type == OS_CORPSE && obj->in_room )
      fprintf( fp, "Room         %d\n",   obj->in_room->vnum         );
    if ( obj->extra_flags != obj->pIndexData->extra_flags )
	fprintf( fp, "ExtraFlags   %d\n",	obj->extra_flags     );
    if ( obj->wear_flags != obj->pIndexData->wear_flags )
	fprintf( fp, "WearFlags    %d\n",	obj->wear_flags	     );
    wear_loc = -1;
    for ( wear = 0; wear < MAX_WEAR; wear++ )
	for ( x = 0; x < MAX_LAYERS; x++ )
	   if ( obj == save_equipment[wear][x] )
	   {
		wear_loc = wear;
		break;
	   }
	   else
	   if ( !save_equipment[wear][x] )
		break;
    if ( wear_loc != -1 )
	fprintf( fp, "WearLoc      %d\n",	wear_loc	     );
    if ( obj->item_type != obj->pIndexData->item_type )
	fprintf( fp, "ItemType     %d\n",	obj->item_type	     );
    if ( obj->weight != obj->pIndexData->weight )
      fprintf( fp, "Weight       %d\n",	obj->weight		     );
    if ( obj->level )
      fprintf( fp, "Level        %d\n",	obj->level		     );
    if ( obj->timer )
      fprintf( fp, "Timer        %d\n",	obj->timer		     );
    if ( obj->cost != obj->pIndexData->cost )
      fprintf( fp, "Cost         %d\n",	obj->cost		     );
    if ( obj->value[0] || obj->value[1] || obj->value[2]
    ||   obj->value[3] || obj->value[4] || obj->value[5] )
      fprintf( fp, "Values       %d %d %d %d %d %d\n",
	obj->value[0], obj->value[1], obj->value[2],
	obj->value[3], obj->value[4], obj->value[5]     );

    switch ( obj->item_type )
    {
    case ITEM_PILL: /* was down there with staff and wand, wrongly - Scryn */
    case ITEM_POTION:
	if ( IS_VALID_SN(obj->value[1]) )
	    fprintf( fp, "Spell 1      '%s'\n",
		skill_table[obj->value[1]]->name );

	if ( IS_VALID_SN(obj->value[2]) )
	    fprintf( fp, "Spell 2      '%s'\n",
		skill_table[obj->value[2]]->name );

	if ( IS_VALID_SN(obj->value[3]) )
	    fprintf( fp, "Spell 3      '%s'\n",
		skill_table[obj->value[3]]->name );

	break;

    case ITEM_DEVICE:
	if ( IS_VALID_SN(obj->value[3]) )
	    fprintf( fp, "Spell 3      '%s'\n",
		skill_table[obj->value[3]]->name );

	break;
    case ITEM_SALVE:
	if ( IS_VALID_SN(obj->value[4]) )
	    fprintf( fp, "Spell 4      '%s'\n",
		skill_table[obj->value[4]]->name );

	if ( IS_VALID_SN(obj->value[5]) )
	    fprintf( fp, "Spell 5      '%s'\n",
		skill_table[obj->value[5]]->name );
	break;
    }

    for ( paf = obj->first_affect; paf; paf = paf->next )
    {
	/*
	 * Save extra object affects				-Thoric
	 */
	if ( paf->type < 0 || paf->type >= top_sn )
	{
	  fprintf( fp, "Affect       %d %d %d %d %d\n",
	    paf->type,
	    paf->duration,
	     ((paf->location == APPLY_WEAPONSPELL
	    || paf->location == APPLY_WEARSPELL
	    || paf->location == APPLY_REMOVESPELL
	    || paf->location == APPLY_STRIPSN)
	    && IS_VALID_SN(paf->modifier))
	    ? skill_table[paf->modifier]->slot : paf->modifier,
	    paf->location,
	    paf->bitvector
	    );
	}
	else
	  fprintf( fp, "AffectData   '%s' %d %d %d %d\n",
	    skill_table[paf->type]->name,
	    paf->duration,
	     ((paf->location == APPLY_WEAPONSPELL
	    || paf->location == APPLY_WEARSPELL
	    || paf->location == APPLY_REMOVESPELL
	    || paf->location == APPLY_STRIPSN)
	    && IS_VALID_SN(paf->modifier))
	    ? skill_table[paf->modifier]->slot : paf->modifier,
	    paf->location,
	    paf->bitvector
	    );
    }

    for ( ed = obj->first_extradesc; ed; ed = ed->next )
	fprintf( fp, "ExtraDescr   %s~ %s~\n",
	    ed->keyword, ed->description );

    fprintf( fp, "End\n\n" );

    if ( obj->first_content )
	fwrite_obj_home( ch, obj->last_content, fp, iNest + 1, OS_CORPSE );

    return;
}



/***************************************************************************
**      MORTAL COMMANDS TODO WITH THE NEW HOMES CODE                      **
**      This involves things like buyhome and sell home                   **
**                                                                        **
**                                                                        **
**                                                                        **
**                                                                        **
****************************************************************************/

void do_councilsellhome(CHAR_DATA *ch, char *argument )
{
	long         price;
	HOME_DATA   *home;
	ROOM_INDEX_DATA *room;
	
	if ( IS_NPC(ch) || !ch->pcdata )
	{
		send_to_char( "&ROnly players can do that!\n\r" ,ch );
		return;
	}
	
	{
		home = home_from_entrance( ch->in_room->vnum);
		if ( !home )
		{    
			act( AT_PLAIN, "I see no home here.", ch, NULL, argument, TO_CHAR );
			return;
		}
		room = ch->in_room;
		
		if ( str_cmp(home->owner,ch->name)
			&& str_cmp(home->pilot,ch->name)
			&& str_cmp(home->copilot,ch->name))    	        
		{
			send_to_char("&RHey, thats not your home!\n\r",ch);
			return;
		}
		
		price = get_home_value( home );
		
		ch->gold += ( price - price/10 );
		ch_printf(ch, "&GYou receive %ld credits from selling your home.\n\r" , price - price/10 );
		
		act( AT_PLAIN, "$n walks over to a terminal and makes a credit transaction.",ch, NULL, argument , TO_ROOM );
		
		STRFREE( home->owner );
		home->owner = STRALLOC( "Unowned" );
		save_home2( home );
	}
}


void do_councilbuyhome(CHAR_DATA *ch, char *argument )
{
	long         price;
	HOME_DATA   *home;
	ROOM_INDEX_DATA *room;
	AREA_DATA *pArea;
	
	if ( IS_NPC(ch) || !ch->pcdata )
	{
		send_to_char( "&ROnly players can do that!\n\r" ,ch );
		return;
	}

	{
		home = home_from_entrance( ch->in_room->vnum);
		if ( !home )
		{
			act( AT_PLAIN, "I see no home here, please goto the entrance.", ch, NULL, argument, TO_CHAR );
			return;
		}
		room = ch->in_room;
		
		for ( pArea = first_bsort; pArea; pArea = pArea->next_sort )
		{
			if ( room->area == pArea )
			{
				send_to_char( "&RThis area isn't installed yet!\n\r&w", ch);
				return;
			}
		}

		if ( str_cmp( home->owner , "Unowned" ) )
		{
			send_to_char( "&RThat home isn't for sale!\n\r" ,ch );
			return;
		}
		price = get_home_value( home );
		if ( ch->gold < price )
		{
			ch_printf(ch, "&RThis home costs %ld. You don't have enough credits!\n\r" , price );
			return;
		}
		if ( argument[0] == '\0' )
		{
			send_to_char( "Set the room name.  A very brief single line room description.\n\r", ch );
			send_to_char( "Usage: Buyhome <Room Name>\n\r", ch );
			return;
		}
		STRFREE( room->name );
		room->name = STRALLOC( argument );

		ch->gold -= price;
		ch_printf(ch, "&G%s pays %ld credits to purchace this fine home.\n\r", ch->name , price );
		
		act( AT_PLAIN, "$n walks over to a terminal and makes a credit transaction.",ch, NULL, argument , TO_ROOM );
		STRFREE( home->owner );
		home->owner = STRALLOC( ch->name );

		save_home2( home );
		fold_area( room->area, room->area->filename, FALSE );
	}
}

void do_roommate(CHAR_DATA *ch, char *argument )
{
    HOME_DATA *home;
    char arg1[MAX_INPUT_LENGTH];
	ROOM_INDEX_DATA	*location;
	AREA_DATA *pArea;

    argument = one_argument( argument, arg1 );
    if ( !ch->desc )
	{
		send_to_char( "You have no descriptor.\n\r", ch );
		return;
	}
	switch( ch->substate )
	{
	default:
		break;
	case SUB_ROOM_DESC:
		location = ch->dest_buf;
		if ( !location )
		{
			bug( "redit: sub_room_desc: NULL ch->dest_buf", 0 );
			location = ch->in_room;
		}
		STRFREE( location->description );
		location->description = copy_buffer( ch );
		stop_editing( ch );
		ch->substate = ch->tempnum;
		for ( pArea = first_bsort; pArea; pArea = pArea->next_sort )
		{
			if ( location->area == pArea )
			{
				send_to_char( "&RThis area isn't installed yet!\n\r&w", ch);
				return;
			}
		}
		fold_area( location->area, location->area->filename, FALSE );
		return;
	}

    location = ch->in_room;
	if ( arg1[0] == '\0' || !str_cmp( arg1, "?" ) )
	{
		send_to_char( "Syntax: roommate <field> value\n\r",		ch );
		send_to_char( "\n\r",ch );
		send_to_char( "Field being one of:\n\r",ch );
		send_to_char( "  add remove desc info\n\r",ch );
		return;
    }
	if ( ( home = home_from_entrance(ch->in_room->vnum) ) == NULL )
	{
		send_to_char("&G&WYou must be in the entrance of the home to do that!\n\r",ch);
		return;
	}
	
	if ( str_cmp( home->owner , ch->name ) )
	{
		{
			send_to_char( "&RThis isn't your house!" ,ch );
			return;
		}
	}
	if ( arg1[0] == '\0' )
	{
		send_to_char( "Syntax:\n\r\tRoommate [add/remove] [roommate name]\n\r",ch);
		return;
	}

	if ( !str_cmp("add", arg1) )
	{
		if (argument[0] == '\0')
		{
			send_to_char( "&RAdd whom?\n\r" ,ch );
			return;
		}

		if ( str_cmp( home->pilot , "" ) )
		{
			if ( str_cmp( home->copilot , "" ) )
			{
				send_to_char( "&RYou allready two roomates..\n\rTry Removing one first\n\r" ,ch );
				return;
			}
			STRFREE( home->copilot );
			home->copilot = STRALLOC( argument );
			send_to_char( "Second Roommate Added.\n\r", ch );
			save_home2( home );
			return;
		}
		STRFREE( home->pilot );
		home->pilot = STRALLOC( argument );
		send_to_char( "First Roommate Added.\n\r", ch );
		save_home2( home );
		return;
	}
	if ( !str_cmp("remove", arg1) )
	{
		if (argument[0] == '\0')
		{
			send_to_char( "&RRemove which roommate?\n\r" ,ch );
			return;
		}

		if ( !str_cmp( home->pilot , argument ) )
		{
			STRFREE( home->pilot );
			home->pilot = STRALLOC( "" );
			send_to_char( "First Roommate Removed.\n\r", ch );
			save_home2( home );
			return;
		}
		if ( !str_cmp( home->copilot , argument ) )
		{
			STRFREE( home->copilot );
			home->copilot = STRALLOC( "" );
			send_to_char( "Second Roommate Removed.\n\r", ch );
			save_home2( home );
			return;
		}
		
		send_to_char( "&RThat person isn't listed as a roommate.\n\r" ,ch );
		return;
	}
	if ( !str_cmp( arg1, "desc" ) )
    {
		if ( ch->substate == SUB_REPEATCMD )
			ch->tempnum = SUB_REPEATCMD;
		else
			ch->tempnum = SUB_NONE;
		ch->substate = SUB_ROOM_DESC;
		ch->dest_buf = location;
		start_editing( ch, location->description );
		return;
    }
	if ( !str_cmp( arg1, "info" ) )
	{
		ch_printf( ch, "&WOwner: &B%-15.15s   &WRoommate1: &B%-15.15s   &WRoommate2: &B%-15.15s\n\r",
			home->owner, home->pilot,  home->copilot );
		return;
	}
}

void fread_obj_home( HOME_DATA *home, FILE *fp, sh_int os_type )
{
    OBJ_DATA *obj;
    char *word;
    int iNest;
    bool fMatch;
    bool fNest;
    bool fVnum;
    ROOM_INDEX_DATA *room;

	room = NULL;
    CREATE( obj, OBJ_DATA, 1 );
    obj->count		= 1;
    obj->wear_loc	= -1;
    obj->weight		= 1;

    fNest		= TRUE;		/* Requiring a Nest 0 is a waste */
    fVnum		= TRUE;
    iNest		= 0;

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
			if ( !str_cmp( word, "Affect" ) || !str_cmp( word, "AffectData" ) )
			{
				AFFECT_DATA *paf;
				int pafmod;

				CREATE( paf, AFFECT_DATA, 1 );
				if ( !str_cmp( word, "Affect" ) )
					paf->type	= fread_number( fp );
				else
				{
					int sn;
					
					sn = skill_lookup( fread_word( fp ) );
					if ( sn < 0 )
						bug( "Fread_obj_home: unknown skill.", 0 );
					else
						paf->type = sn;
				}
				paf->duration	= fread_number( fp );
				pafmod		= fread_number( fp );
				paf->location	= fread_number( fp );
				paf->bitvector	= fread_number( fp );
				if ( paf->location == APPLY_WEAPONSPELL	|| paf->location == APPLY_WEARSPELL
					|| paf->location == APPLY_REMOVESPELL )
					paf->modifier		= slot_lookup( pafmod );
				else
					paf->modifier		= pafmod;
				LINK(paf, obj->first_affect, obj->last_affect, next, prev );
				fMatch				= TRUE;
				break;
			}
			KEY( "Actiondesc",	obj->action_desc,	fread_string( fp ) );
			break;

		case 'C':
			KEY( "Cost",	obj->cost,		fread_number( fp ) );
/*			KEY( "Count",	obj->count,		fread_number( fp ) ); Should Not Be Loaded */
			break;

		case 'D':
			KEY( "Description",	obj->description,	fread_string( fp ) );
			break;

		case 'E':
			KEY( "ExtraFlags",	obj->extra_flags,	fread_number( fp ) );

			if ( !str_cmp( word, "ExtraDescr" ) )
			{
				EXTRA_DESCR_DATA *ed;

				CREATE( ed, EXTRA_DESCR_DATA, 1 );
				ed->keyword		= fread_string( fp );
				ed->description		= fread_string( fp );
				LINK(ed, obj->first_extradesc, obj->last_extradesc, next, prev );
				fMatch 				= TRUE;
			}

			if ( !str_cmp( word, "End" ) )
			{
				if ( !fNest || !fVnum )
				{
					bug( "Fread_obj_home: incomplete object.", 0 );
					if ( obj->name )
						STRFREE( obj->name        );
					if ( obj->description )
						STRFREE( obj->description );
					if ( obj->short_descr )
						STRFREE( obj->short_descr );
					DISPOSE( obj );
					return;
				}
				else
				{
					sh_int wear_loc = obj->wear_loc;
					if ( !obj->name )
						obj->name = QUICKLINK( obj->pIndexData->name );
					if ( !obj->description )
						obj->description = QUICKLINK( obj->pIndexData->description );
					if ( !obj->short_descr )
						obj->short_descr = QUICKLINK( obj->pIndexData->short_descr );
					if ( !obj->action_desc )
						obj->action_desc = QUICKLINK( obj->pIndexData->action_desc );
					LINK(obj, first_object, last_object, next, prev );
					obj->pIndexData->count += obj->count;
					if ( !obj->serial )
					{
						cur_obj_serial = UMAX((cur_obj_serial + 1 ) & (BV30-1), 1);
						obj->serial = obj->pIndexData->serial = cur_obj_serial;
					}
					if ( fNest )
						rgObjNest[iNest] = obj;
					numobjsloaded += obj->count;
					++physicalobjects;
					if ( file_ver > 1 || obj->wear_loc < -1
						||   obj->wear_loc >= MAX_WEAR )
						obj->wear_loc = -1;
				    /* Corpse saving. -- Altrag */
					if ( os_type == OS_CORPSE )
					{
						if ( !room )
						{
							bug( "Fread_obj: Corpse without room", 0);
							room = get_room_index(ROOM_VNUM_LIMBO);
						}
						obj = obj_to_room( obj, room );
					}
					else if ( iNest == 0 || rgObjNest[iNest] == NULL )
					{
						int slot;
						bool reslot = FALSE;
						
						if ( file_ver > 1 && wear_loc > -1 && wear_loc < MAX_WEAR )
						{
							int x;

							for ( x = 0; x < MAX_LAYERS; x++ )
							{
								if ( !save_equipment[wear_loc][x] )
								{
									save_equipment[wear_loc][x] = obj;
									slot = x;
									reslot = TRUE;
									break;
								}
							}
							if ( x == MAX_LAYERS )
								bug( "Fread_obj_home: too many layers %d", wear_loc );
						}
						if ( reslot )
							save_equipment[wear_loc][slot] = obj;
					}
					else
					{
						if ( rgObjNest[iNest-1] )
						{
							separate_obj( rgObjNest[iNest-1] );
							obj = obj_to_obj( obj, rgObjNest[iNest-1] );
						}
						else
							bug( "Fread_obj_home: nest layer missing %d", iNest-1 );
					}
					if ( fNest )
						rgObjNest[iNest] = obj;
					return;
				}
			}
			break;
			
			case 'I':
				KEY( "ItemType",	obj->item_type,		fread_number( fp ) );
				break;
			case 'L':
				KEY( "Level",	obj->level,		fread_number( fp ) );
				break;

			case 'N':
				KEY( "Name",	obj->name,		fread_string( fp ) );
				if ( !str_cmp( word, "Nest" ) )
				{
					iNest = fread_number( fp );
					if ( iNest < 0 || iNest >= MAX_NEST )
					{
						bug( "Fread_obj_home: bad nest %d.", iNest );
						iNest = 0;
						fNest = FALSE;
					}
					fMatch = TRUE;
				}
				break;
				
			case 'R':
			    if ( !str_cmp( word, "Room" ) )
				{
					int room_vnum;
					room_vnum = fread_number(fp);
					room = get_room_index( room_vnum );
					obj = obj_to_room( obj, room );
					fMatch = TRUE;
					break;
				}


			case 'S':
				KEY( "ShortDescr",	obj->short_descr,	fread_string( fp ) );
				if ( !str_cmp( word, "Spell" ) )
				{
					int iValue;
					int sn;

					iValue = fread_number( fp );
					sn     = skill_lookup( fread_word( fp ) );

					if ( iValue < 0 || iValue > 5 )
						bug( "Fread_obj_home: bad iValue %d.", iValue );
					else if ( sn < 0 )
						bug( "Fread_obj_home: unknown skill.", 0 );
					else
						obj->value[iValue] = sn;
					fMatch = TRUE;
					break;
				}

				break;

			case 'T':
				KEY( "Timer",	obj->timer,		fread_number( fp ) );
				break;

			case 'V':
				
				if ( !str_cmp( word, "Values" ) )
				{
					int x1,x2,x3,x4,x5,x6;
					char *ln = fread_line( fp );

					x1=x2=x3=x4=x5=x6=0;
					sscanf( ln, "%d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6 );
					/* clean up some garbage */
					if ( file_ver < 3 )
						x5=x6=0;

					obj->value[0]	= x1;
					obj->value[1]	= x2;
					obj->value[2]	= x3;
					obj->value[3]	= x4;
					obj->value[4]	= x5;
					obj->value[5]	= x6;
					fMatch		= TRUE;
					break;

				}
				
				if ( !str_cmp( word, "Vnum" ) )
				{
					int vnum;

					vnum = fread_number( fp );
					if ( ( obj->pIndexData = get_obj_index( vnum ) ) == NULL )
					{
						fVnum = FALSE;
						bug( "Fread_obj_home: bad vnum %d.", vnum ); 
					}
					else
					{
						fVnum = TRUE;
						obj->cost = obj->pIndexData->cost;
						obj->weight = obj->pIndexData->weight;
						obj->item_type = obj->pIndexData->item_type;
						obj->wear_flags = obj->pIndexData->wear_flags;
						obj->extra_flags = obj->pIndexData->extra_flags;
					}
					fMatch = TRUE;
					break;
				}
				break;

			case 'W':		KEY( "WearFlags",	obj->wear_flags,	fread_number( fp ) );
				KEY( "WearLoc",	obj->wear_loc,		fread_number( fp ) );

				KEY( "Weight",	obj->weight,		fread_number( fp ) );
				break;
		}

		if ( !fMatch )
		{
			EXTRA_DESCR_DATA *ed;
			AFFECT_DATA *paf;

			bug( "Fread_obj_home: no match.", 0 );

			bug( word, 0 );
			fread_to_eol( fp );
			if ( obj->name )
				STRFREE( obj->name        );
			if ( obj->description )
				STRFREE( obj->description );
			if ( obj->short_descr )
				STRFREE( obj->short_descr );
			while ( (ed=obj->first_extradesc) != NULL )
			{
				STRFREE( ed->keyword );
				STRFREE( ed->description );
				UNLINK( ed, obj->first_extradesc, obj->last_extradesc, next, prev );
				DISPOSE( ed );
			}
			while ( (paf=obj->first_affect) != NULL )
			{
				UNLINK( paf, obj->first_affect, obj->last_affect, next, prev );
				DISPOSE( paf );
			}
			DISPOSE( obj );
			return;
		}
    }
}

void do_freehomes(CHAR_DATA *ch, char *argument) 
{
	HOME_DATA *home;
	ROOM_INDEX_DATA *room; 
	int count = 0;
	home_classes category;

	if ( IS_NPC(ch) )
		return;

	send_to_char( "&Y\n\rThe following homes are for sale:\n\r", ch );

	for (category = APARTMENT_HOME; category <= GIANT_HOME; category++)
	{
		if (category == APARTMENT_HOME)
			send_to_char( "\n\r&BOne Or Two Room Apartments\n\r", ch );
		if (category == MIDSIZE_HOME)
			send_to_char( "\n\r&RMid Sized Houses\n\r", ch );
		if (category == GIANT_HOME)
			send_to_char( "\n\r&GGiant Sized Estates\n\r", ch );
		send_to_char( "&WHome                               Price                Location\n\r", ch );
		
		set_char_color( AT_BLUE, ch );

		for ( home = first_home; home; home = home->next )
		{   
			room = get_room_index(home->firstroom);
			if ( !room )
				continue;
			if (home->type == MOB_HOME)
				continue;
			if (home->class != category)
				continue;
			set_char_color( AT_BLUE, ch );

			if ( !str_cmp(home->owner,"Unowned") )
			{
				if ( room->area->planet )
				{
					ch_printf( ch, "%-34s %ld to buy \t%s.\n\r",
						home->name, get_home_value(home), room->area->planet->name);
				}
				else
				{
					ch_printf( ch, "%-34s %ld to buy \t%s.\n\r",
						home->name, get_home_value(home), "somewhere");
				}
				count++;
			}
			else
				continue;
		}
		if ( !count )
			send_to_char( "There are no homes currently for sale.\n\r", ch );
	}
	send_to_char( "\n\r", ch );
}
