#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "mud.h"
#ifdef USE_IMC
	#include "imc.h"
  	#include "icec.h"
#endif

/* Globals */
int		control;
int		control2;
int		conjava;
int		conclient;


/* Externals */
char *help_fix		args( ( char *text ) );

/*
 * Copyover "do" Function		-Nopey
 * 6/11/01
 */
void do_copyover( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument( argument, arg );
	
	/* if there is no arg supplied */
	if( arg[0] == '\0' )
	{
		send_to_char_color( "\n\r&YSyntax: copyover saveall\n\r", ch );
		send_to_char_color( "Syntax: copyover nosave&g\n\r", ch );
		return;
	}

	/* saves all data */
	if( !str_cmp( arg, "saveall" ) )
	{
		log_string( "Copyover: Saving Game Data..." );
		save_gamedata();
		log_string( "Copyover Save: Game Data Saved. Performing Copyover..." );
		copyover();
		return;
	}

	/* saves nothing */
	if( !str_cmp( arg, "nosave" ) )
	{
		log_string( "Copyover NoSave: Performing Copyover..." );
		copyover();
		return;
	}
}

/*
 * Actual Copyover "Beef" Function	-Nopey
 */
void copyover()
{
	FILE *fp;
	DESCRIPTOR_DATA *d = NULL;
	char buf2[100], buf3[100], buf4[100], buf5[100], buf6[100];
	int port = 5555;
	fp = fopen( COPYOVER_FILE, "w" );
	
#ifdef USE_IMC
	log_string( "Copyover: Initiating IMC2 shutdown..." );
	imc_shutdown();
	log_string( "Copyover: IMC2 Shutdown Complete." );
#endif
	for( d = first_descriptor; d; d = d->next )
	{	
		write_to_buffer( d, "Copyover: Copyover has been initiated. Hold on.\n\r", 0 );
		/* drop those logging on */
		if( !d->character || d->connected != CON_PLAYING )
        	{
			write_to_buffer( d, "Sorry the game is currently in a copyover. Please come back in a minute or so.\n\r", 0 );
			close_socket( d, FALSE );
		}
		else
		{
#ifdef I3
			I3_close_char( d );
#endif
			if( d->character->top_level == 1 )
			{
				write_to_descriptor( d->descriptor, "Since you are level one, and level ones are not saved, you gain a level!\n\r", 0 );
				fprintf( fp, "%d %d %d %d %s %s %s\n", d->descriptor, d->port, ROOM_VNUM_TEMPLE, d->idle, d->character->name, d->user, d->host );
			        advance_level(d->character, 1);
                  		d->character->top_level++;
			} else 
				fprintf( fp, "%d %d %d %d %s %s %s\n", d->descriptor, d->port, d->character->in_room->vnum, d->idle, d->character->name, d->user, d->host );
		}		
	}
	fprintf (fp, "-1\n");
	fclose( fp );

#ifdef I3
    log_string( "Initiating Intermud-3 shutdown...." );	
    I3_shutdown( 0 );
    log_string( "Intermud-3 shutdown completed." );
#endif

	log_string( "Initiating Copyover." );
	fclose( fpReserve );
	fclose( fpLOG );

	/* exec - descriptors are inherited */
        sprintf( buf2, "%d", port );
        sprintf( buf3, "%d", control );
        sprintf( buf4, "%d", control2 );
        sprintf( buf5, "%d", conclient );
        sprintf( buf6, "%d", conjava );
	
	/* execute the file */
	execl( EXE_FILE, "unknown", buf2, "copyover", buf3, buf4, buf5, buf6, (char*)NULL );
	
        /* Here you might want to reopen fpReserve */
        /* Since I'm a neophyte type guy, I'll assume
	 * this is a good idea and cut and past from main() */
        if( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
        {
        	perror( NULL_FILE );
                exit( 1 );
        }
        if( ( fpLOG = fopen( NULL_FILE, "r" ) ) == NULL )
        {
                perror( NULL_FILE );
                exit( 1 );
        }
	log_string( "Copyover FAILED!" );
	perror( "Copyover: Execl" ); 
}

/*
 * Copyover Recover
 * Rewritten 6/14/01		-Nopey
 */
void copyover_recover()
{
	DESCRIPTOR_DATA *d = NULL;
	FILE *fp;
	char name [100];   
  	char host[MAX_STRING_LENGTH];
	char user[MAX_INPUT_LENGTH]; /* 6/24/01 - User fix - Nopey */
	char buf[MAX_INPUT_LENGTH];
  	int desc;
	int dport; /* 6/24/01 - Port fix - Nopey */
	int idle; /* 6/24/01 - Idle fix - Nopey */
  	bool fOld;
	int room;
	fp = fopen( COPYOVER_FILE, "r" );
	
	/* there are some descriptors open which will hang forever then ? */
	if( !fp )
        {
                  perror("Copyver: copyover_recover:fopen");
                  log_string( "Copyover File not Found. Exiting.");
                  exit( 1 ); 
        }

	log_string( "Copyover: Recovering Character Links..." );

	/* In case something crashes - doesn't prevent reading */  
	unlink( COPYOVER_FILE );
/*	unlink( MOBSAVE_FILE ); 
	unlink( OBJSAVE_FILE ); */  

	for( ; ; )
	{
		fscanf( fp, "%d %d %d %d %s %s %s\n", &desc, &dport, &room, &idle, name, user, host );
		if( desc == -1 )
       		break;

		/* Write something, and check if it goes error-free */
     		if(!write_to_descriptor(desc, "\n\rCopyover: Restoring your link.\n\r", 0))
       		       	close( desc ); /* nope */
                

		/* boot up the descriptor! */
                CREATE( d, DESCRIPTOR_DATA, 1 );
                d->next		= NULL;
                d->descriptor	= desc;
                d->connected	= CON_GET_NAME;
                d->outsize	= 2000;
                d->idle		= 0;
                d->lines	= 0;
                d->scrlen	= 24;
                d->newstate	= 0;
                d->prevcolor	= 0x07;
                CREATE( d->outbuf, char, d->outsize );
                d->user = STRALLOC( user );
                d->host = STRALLOC( host );
                d->port = dport;
                d->idle = idle;

                LINK( d, first_descriptor, last_descriptor, next, prev );
		d->connected = CON_COPYOVER_RECOVER;

		/* Now, find the pfile */
		fOld = load_char_obj( d, name, FALSE );
	
		/* No pfile? */
		if( !fOld )
		{	
			write_to_descriptor( desc, "Copyover: Somehow your Character was lost during the Copyover. Please contact an Immortal.\n\r", 0 );
			close_socket (d, FALSE);
		}
		else /* ok! */
		{
			d->character->in_room = get_room_index( room );
			if( !d->character->in_room )
				d->character->in_room = get_room_index( ROOM_VNUM_TEMPLE );
			 /* Insert in the char_list */
           		LINK( d->character, first_char, last_char, next, prev );
			num_descriptors++;
           		sysdata.maxplayers++;
			/* send to the room */
			char_to_room( d->character, d->character->in_room );
#ifdef I3
	      		I3_char_login( d->character );
#endif
			d->connected = CON_PLAYING;
			do_look( d->character, "auto noprog" );
			act( AT_YELLOW, "$n magically arrives in a puff of grey smoke.", d->character, NULL, NULL, TO_ROOM);
			sprintf( buf, "\n\rWelcome back, %s.\n\r", d->character->name );
			write_to_buffer( d, buf, 0 );
		}
	}
	log_string( "Copyover Sucessful. Resuming Play." );
	fclose( fp );
	return;
}

/*
 * Not sure if this is even used anymore... oh well
 */

/*
 * Decided to Clean the code up even more	-Nopey
 * Left here for upgrades so that the people
 * don't need to get old init_descriptor.
 * **NOTE** If your NOT upgrading, comment this out.
 */
void init_descriptor( DESCRIPTOR_DATA *d, int desc)
{
        d->next		= NULL;
        d->descriptor	= desc;
        d->connected	= CON_GET_NAME;
        d->outsize	= 2000;
        d->idle		= 0;
        d->lines	= 0;
        d->scrlen	= 24;
        d->newstate	= 0;
        d->prevcolor	= 0x07;
    CREATE( d->outbuf, char, d->outsize );
}


void save_gamedata()
{
	char filename[256];
	FILE *fpout;
	AREA_DATA *tarea;
	CHAR_DATA *victim;
	HELP_DATA *pHelp;
	
	        for( victim = first_char; victim; victim = victim->next )
        	{
			/* save character */
        		save_char_obj( victim );
        		pager_printf_color( victim, "&YYou have been manually due to a copyover.\n\r" );
        	}
	
		/* save area files */
		for( tarea = first_build; tarea; tarea = tarea->next )
		{
        		if( !IS_SET( tarea->status, AREA_LOADED ) )
        		{
        			continue;
        		}
        		sprintf( filename, "%s%s", BUILD_DIR, tarea->filename );
        		fold_area( tarea, filename, FALSE );
		}
		rename( "help.are", "help.are.bak" );
        	log_string( "Saving help.are..." );
		fclose( fpReserve );
        	if ( ( fpout = fopen( "help.are", "w" ) ) == NULL )
        	{
        	   bug( "hset save: fopen", 0 );
        	   perror( "help.are" );
        	   fpReserve = fopen( NULL_FILE, "r" );
	   	   return;
		}
		fprintf( fpout, "#HELPS\n\n" );
        	for ( pHelp = first_help; pHelp; pHelp = pHelp->next )
        	    fprintf( fpout, "%d %s~\n%s~\n\n",
        		pHelp->level, pHelp->keyword, help_fix(pHelp->text) );
        	fprintf( fpout, "0 $~\n\n\n#$\n" );
        	fclose( fpout );
        	fpReserve = fopen( NULL_FILE, "r" );
        	//save_commands();
        	save_socials();
        	save_commands();
        	save_skill_table();
        	save_herb_table();
		return;
}

