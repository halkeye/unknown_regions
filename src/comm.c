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
*			 Low-level communication module			   *
****************************************************************************/

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include "mud.h"

/*
 * Socket and TCP/IP stuff.
 */
#ifdef WIN32
  #include <io.h>
  #undef EINTR
  #undef EMFILE
  #define EINTR WSAEINTR
  #define EMFILE WSAEMFILE
  #define EWOULDBLOCK WSAEWOULDBLOCK
  #define MAXHOSTNAMELEN 32

  #define  TELOPT_ECHO        '\x01'
  #define  GA                 '\xF9'
  #define  SB                 '\xFA'
  #define  WILL               '\xFB'
  #define  WONT               '\xFC'
  #define  DO                 '\xFD'
  #define  DONT               '\xFE'
  #define  IAC                '\xFF'
  void bailout(void);
  void shutdown_checkpoint (void);
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netinet/in_systm.h>
  #include <netinet/ip.h>
  #include <arpa/inet.h>
  #include <arpa/telnet.h>
  #include <netdb.h>
  #define closesocket close
#endif

#ifdef sun
int gethostname ( char *name, int namelen );
#endif

#define MAX_NEST	100
static	OBJ_DATA *	rgObjNest	[MAX_NEST];

const	char	echo_off_str	[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const	char	echo_on_str	[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const	char 	go_ahead_str	[] = { IAC, GA, '\0' };

void	auth_maxdesc	args( ( int *md, fd_set *ins, fd_set *outs,
				fd_set *excs ) );
void	auth_check	args( ( fd_set *ins, fd_set *outs, fd_set *excs ) );
void	set_auth	args( ( DESCRIPTOR_DATA *d ) );
void	kill_auth	args( ( DESCRIPTOR_DATA *d ) );


void    save_sysdata args( ( SYSTEM_DATA sys ) );


/*  from act_info?  */
void    show_condition		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

/*	from new_auth.c */
void	save_auth_list	args( ( ) );

/*
 * Global variables.
 */
DESCRIPTOR_DATA *   first_descriptor;	/* First descriptor		*/
DESCRIPTOR_DATA *   last_descriptor;	/* Last descriptor		*/
DESCRIPTOR_DATA *   d_next;		/* Next descriptor in loop	*/
int		    num_descriptors;
FILE *		    fpReserve;		/* Reserved file handle		*/
bool		    mud_down;		/* Shutdown			*/
bool		    service_shut_down;  /* Shutdown by operator closing down service */
bool		    wizlock;		/* Game is wizlocked		*/
time_t              boot_time;
HOUR_MIN_SEC  	    set_boot_time_struct;
HOUR_MIN_SEC *      set_boot_time;
struct tm *         new_boot_time;
struct tm           new_boot_struct;
char		    str_boot_time[MAX_INPUT_LENGTH];
char		    lastplayercmd[MAX_INPUT_LENGTH*2];
time_t		    current_time;	/* Time of this pulse		*/
int		    control;		/* Controlling descriptor	*/
int		    control2;		/* Controlling descriptor	*/
int		    newdesc;		/* New descriptor		*/
fd_set		    in_set;		/* Set of desc's for reading	*/
fd_set		    out_set;		/* Set of desc's for writing	*/
fd_set		    exc_set;		/* Set of desc's with errors	*/
int 		    maxdesc;
char *		    alarm_section = "(unknown)";

/*
 * OS-dependent local functions.
 */
void	game_loop		args( ( ) );
int	init_socket		args( ( int port ) );
void	new_descriptor		args( ( int new_desc ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );


/*
 * Other local functions (OS-independent).
 */
bool	check_parse_name	args( ( char *name, bool newchar ) );
bool	check_reconnect		args( ( DESCRIPTOR_DATA *d, char *name,
				    bool fConn ) );
bool	check_playing		args( ( DESCRIPTOR_DATA *d, char *name, bool kick ) );
bool	check_multi		args( ( DESCRIPTOR_DATA *d, char *name ) );
int	main			args( ( int argc, char **argv ) );
void	nanny			args( ( DESCRIPTOR_DATA *d, char *argument ) );
bool	flush_buffer		args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void	read_from_buffer	args( ( DESCRIPTOR_DATA *d ) );
void	stop_idling		args( ( CHAR_DATA *ch ) );
void	free_desc		args( ( DESCRIPTOR_DATA *d ) );
void	display_prompt		args( ( DESCRIPTOR_DATA *d ) );
int	make_color_sequence	args( ( const char *col, char *buf,
					DESCRIPTOR_DATA *d ) );
void	set_pager_input		args( ( DESCRIPTOR_DATA *d,
					char *argument ) );
bool	pager_output		args( ( DESCRIPTOR_DATA *d ) );

void	mail_count		args( ( CHAR_DATA *ch ) );

void    tax_player		args( ( CHAR_DATA *ch ) );

int port;

#ifdef WIN32
  int mainthread( int argc, char **argv )
#else
  int main( int argc, char **argv )
#endif
{
    struct timeval now_time;
	char hostn[128];
	bool fCopyOver = !TRUE;
    /*
     * Memory debugging if needed.
     */
#if defined(MALLOC_DEBUG)
    malloc_debug( 2 );
#endif

    DONT_UPPER			= FALSE;
	num_descriptors		= 0;
    first_descriptor		= NULL;
    last_descriptor		= NULL;
    sysdata.NO_NAME_RESOLVING	= TRUE;
    sysdata.WAIT_FOR_AUTH	= TRUE;

    /*
     * Init time.
     */
    gettimeofday( &now_time, NULL );
    current_time = (time_t) now_time.tv_sec;
/*  gettimeofday( &boot_time, NULL);   okay, so it's kludgy, sue me :) */
    boot_time = time(0);         /*  <-- I think this is what you wanted */
    strcpy( str_boot_time, ctime( &current_time ) );

    /*
     * Init boot time.
     */
    set_boot_time = &set_boot_time_struct;
    set_boot_time->manual = 0;
    
    new_boot_time = update_time(localtime(&current_time));
    /* Copies *new_boot_time to new_boot_struct, and then points
       new_boot_time to new_boot_struct again. -- Alty */
    new_boot_struct = *new_boot_time;
    new_boot_time = &new_boot_struct;
    new_boot_time->tm_mday += 1;
    if(new_boot_time->tm_hour > 12)
		new_boot_time->tm_mday += 1;
    new_boot_time->tm_sec = 0;
    new_boot_time->tm_min = 0;
    new_boot_time->tm_hour = 6;

    /* Update new_boot_time (due to day increment) */
    new_boot_time = update_time(new_boot_time);
    new_boot_struct = *new_boot_time;
    new_boot_time = &new_boot_struct;
    /* Bug fix submitted by Gabe Yoder */
    new_boot_time_t = mktime(new_boot_time);
    reboot_check(mktime(new_boot_time));
    /* Set reboot time string for do_time */
    get_reboot_string();

    /*
     * Reserve two channels for our use.
     */
    if ( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
    {
	perror( NULL_FILE );
	exit( 1 );
    }
    if ( ( fpLOG = fopen( NULL_FILE, "r" ) ) == NULL )
    {
	perror( NULL_FILE );
	exit( 1 );
    }

    /*
     * Get the port number.
     */
    port = 4000;
    if ( argc > 1 )
    {
	if ( !is_number( argv[1] ) )
	{
	    fprintf( stderr, "Usage: %s [port #]\n", argv[0] );
	    exit( 1 );
	}
	else if ( ( port = atoi( argv[1] ) ) <= 1024 )
	{
	    fprintf( stderr, "Port number must be above 1024.\n" );
	    exit( 1 );
	}
	if (argv[2] && argv[2][0])
        {
           fCopyOver = TRUE;
           control = atoi(argv[3]);
           control2 = atoi(argv[4]);
        } 
       else
           fCopyOver = FALSE;	
	}


    /*
     * Run the game.
     */
#ifdef WIN32
    {
	/* Initialise Windows sockets library */

	unsigned short wVersionRequested = MAKEWORD(1, 1);
	WSADATA wsadata;
	int err;

	/* Need to include library: wsock32.lib for Windows Sockets */
	err = WSAStartup(wVersionRequested, &wsadata);
	if (err)
	{
	    fprintf(stderr, "Error %i on WSAStartup\n", err);
	    exit(1);
	}

	/* standard termination signals */
	signal(SIGINT, (void *) bailout);
	signal(SIGTERM, (void *) bailout);
  }
#endif /* WIN32 */

    log_string("Booting Database");
    boot_db( fCopyOver );
    log_string("Initializing socket");
    if (!fCopyOver) /* We have already the port if copyover'ed */
     {
        control = init_socket (port);
     }

    /* I don't know how well this will work on an unnamed machine as I don't
    have one handy, and the man pages are ever-so-helpful.. -- Alty */
    if (gethostname(hostn, sizeof(hostn)) < 0)
    {
      perror("main: gethostname");
      strcpy(hostn, "unresolved");
    }

    sprintf( log_buf, "Unknown Regions ready on port %d.", port );
    log_string( log_buf );
    game_loop( );
    close( control  );

#ifdef WIN32
    if (service_shut_down)
    {
	CHAR_DATA *vch;

	/* Save all characters before booting. */
	for ( vch = first_char; vch; vch = vch->next )
	    if ( !IS_NPC( vch ) )
	    {
		shutdown_checkpoint ();
		save_char_obj( vch );
	    }
	}
    /* Shut down Windows sockets */
    
    WSACleanup();                 /* clean up */
    kill_timer();                 /* stop timer thread */
#endif


    /*
     * That's all, folks.
     */
    log_string( "Normal termination of game." );
    exit( 0 );
    return 0;
}

int init_socket( int port )
{
    char hostname[64];
    struct sockaddr_in	 sa;
    int x = 1;
    int fd;

    gethostname(hostname, sizeof(hostname));
    

    if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
	perror( "Init_socket: socket" );
	exit( 1 );
    }

    if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
		    (void *) &x, sizeof(x) ) < 0 )
    {
	perror( "Init_socket: SO_REUSEADDR" );
	close( fd );
	exit( 1 );
    }

#if defined(SO_DONTLINGER) && !defined(SYSV)
    {
	struct	linger	ld;

	ld.l_onoff  = 1;
	ld.l_linger = 1000;

	if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
			(void *) &ld, sizeof(ld) ) < 0 )
	{
	    perror( "Init_socket: SO_DONTLINGER" );
	    closesocket( fd );
	    exit( 1 );
	}
    }
#endif

    memset(&sa, '\0', sizeof(sa));
    sa.sin_family   = AF_INET;
    sa.sin_port	    = htons( port );

    if ( bind( fd, (struct sockaddr *) &sa, sizeof(sa) ) == -1 )
    {
	perror( "Init_socket: bind" );
	close( fd );
	exit( 1 );
    }

    if ( listen( fd, 50 ) < 0 )
    {
	perror( "Init_socket: listen" );
	close( fd );
	exit( 1 );
    }

    return fd;
}

/*
 * LAG alarm!							-Thoric
 */
static void caught_alarm()
{
    char buf[MAX_STRING_LENGTH];

    sprintf( buf, "ALARM CLOCK!  In section %s", alarm_section );
    bug( buf );
    strcpy( buf, "Alas, the hideous malevalent entity known only as 'Lag' rises once more!\n\r" );
    echo_to_all( AT_IMMORT, buf, ECHOTAR_ALL );
    if ( newdesc )
    {
	FD_CLR( newdesc, &in_set );
	FD_CLR( newdesc, &out_set );
	FD_CLR( newdesc, &exc_set );
	log_string( "clearing newdesc" );
    }
}


bool check_bad_desc( int desc )
{
    if ( FD_ISSET( desc, &exc_set ) )
    {
	FD_CLR( desc, &in_set );
	FD_CLR( desc, &out_set );
	log_string( "Bad FD caught and disposed." );
	return TRUE;
    }
    return FALSE;
}

/*
 * Determine whether this player is to be watched  --Gorog
 */
bool chk_watch(sh_int player_level, char *player_name, char *player_site)
{
    WATCH_DATA *pw;
/*
    char buf[MAX_INPUT_LENGTH];
    sprintf( buf, "che_watch entry: plev=%d pname=%s psite=%s",
                  player_level, player_name, player_site);
    log_string(buf);
*/
    if ( !first_watch ) return FALSE;

    for ( pw = first_watch; pw; pw = pw->next )
    {
        if ( pw->target_name )
        {
           if ( !str_cmp(pw->target_name, player_name)
           &&   player_level < pw->imm_level )
                 return TRUE;
        }
        else 
        if ( pw->player_site )
        {
           if ( !str_prefix(pw->player_site, player_site)
           &&   player_level < pw->imm_level )
                 return TRUE;
        }
    }
    return FALSE; 
}


void accept_new( int ctrl )
{
	static struct timeval null_time;
	DESCRIPTOR_DATA *d;
	/* int maxdesc; Moved up for use with id.c as extern */

#if defined(MALLOC_DEBUG)
	if ( malloc_verify( ) != 1 )
	    abort( );
#endif

	/*
	 * Poll all active descriptors.
	 */
	FD_ZERO( &in_set  );
	FD_ZERO( &out_set );
	FD_ZERO( &exc_set );
	FD_SET( ctrl, &in_set );
	maxdesc	= ctrl;
	newdesc = 0;
	for ( d = first_descriptor; d; d = d->next )
	{
	    maxdesc = UMAX( maxdesc, d->descriptor );
	    FD_SET( d->descriptor, &in_set  );
	    FD_SET( d->descriptor, &out_set );
	    FD_SET( d->descriptor, &exc_set );
	    if ( d == last_descriptor )
	      break;
	}
	auth_maxdesc(&maxdesc, &in_set, &out_set, &exc_set);

	if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
	{
	    perror( "accept_new: select: poll" );
	    exit( 1 );
	}

	if ( FD_ISSET( ctrl, &exc_set ) )
	{
	    bug( "Exception raise on controlling descriptor %d", ctrl,0 );
	    FD_CLR( ctrl, &in_set );
	    FD_CLR( ctrl, &out_set );
	}
	else
	if ( FD_ISSET( ctrl, &in_set ) )
	{
	    newdesc = ctrl;
	    new_descriptor( newdesc );
	}
}

void game_loop( )
{
    struct timeval	  last_time;
    char cmdline[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
/*  time_t	last_check = 0;  */

#ifndef WIN32
    signal( SIGPIPE, SIG_IGN );
    signal( SIGALRM, caught_alarm );
#endif

    /* signal( SIGSEGV, SegVio ); */
	gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /* Main loop */
    while ( !mud_down )
    {
	accept_new( control  );

	auth_check(&in_set, &out_set, &exc_set);

	/*
	 * Kick out descriptors with raised exceptions
	 * or have been idle, then check for input.
	 */
	for ( d = first_descriptor; d; d = d_next )
	{
	    if ( d == d->next )
	    {
	      bug( "descriptor_loop: loop found & fixed",0 );
	      d->next = NULL;
	    }
 	    d_next = d->next;   

	    d->idle++;	/* make it so a descriptor can idle out */
		if ( d->idle > 1200 && !IS_SET(d->character->act, PLR_AFK))
		{
			write_to_descriptor( d->descriptor,
				"AFK Check... You are now Afk.\n\r", 0 );
			SET_BIT(d->character->act, PLR_AFK);
		}
	    if ( FD_ISSET( d->descriptor, &exc_set ) )
	    {
		FD_CLR( d->descriptor, &in_set  );
		FD_CLR( d->descriptor, &out_set );
		if ( d->character
		&& ( d->connected == CON_PLAYING
		||   d->connected == CON_EDITING ) )
		    save_char_obj( d->character );
		d->outtop	= 0;
		close_socket( d, TRUE );
		continue;
	    }
	    else 
	    if ( (!d->character && d->idle > 360)		  /* 2 mins */
            ||   ( d->connected != CON_PLAYING && d->idle > 1200) /* 5 mins */
	    ||     d->idle > 28800 )				  /* 2 hrs  */
	    {
		write_to_descriptor( d->descriptor,
		 "Idle timeout... disconnecting.\n\r", 0 );
		d->outtop	= 0;
		close_socket( d, TRUE );
		continue;
	    }
	    else
	    {
		d->fcommand	= FALSE;

		if ( FD_ISSET( d->descriptor, &in_set ) )
		{
			d->idle = 0;
			if ( d->character )
			  d->character->timer = 0;
			if ( !read_from_descriptor( d ) )
			{
			    FD_CLR( d->descriptor, &out_set );
			    if ( d->character
			    && ( d->connected == CON_PLAYING
			    ||   d->connected == CON_EDITING ) )
				save_char_obj( d->character );
			    d->outtop	= 0;
			    close_socket( d, FALSE );
			    continue;
			}
		}

		if ( d->character && d->character->wait > 0 )
		{
			--d->character->wait;
			continue;
		}

		read_from_buffer( d );
		if ( d->incomm[0] != '\0' )
		{
			d->fcommand	= TRUE;
			stop_idling( d->character );

			strcpy( cmdline, d->incomm );
			d->incomm[0] = '\0';
			
			if ( d->character )
			  set_cur_char( d->character );

			if ( d->pagepoint )
			  set_pager_input(d, cmdline);
			else
			  switch( d->connected )
			  {
			   default:
 				nanny( d, cmdline );
				break;
			   case CON_PLAYING:
				interpret( d->character, cmdline );
				break;
			   case CON_EDITING:
				edit_buffer( d->character, cmdline );
				break;
			  }
		}
	    }
	    if ( d == last_descriptor )
	      break;
	}

	/*
	 * Autonomous game motion.
	 */
	update_handler( );

	/*
	 * Check REQUESTS pipe
	 */
        check_requests( );

	/*
	 * Output.
	 */
	for ( d = first_descriptor; d; d = d_next )
	{
	    d_next = d->next;   

	    if ( ( d->fcommand || d->outtop > 0 )
	    &&   FD_ISSET(d->descriptor, &out_set) )
	    {
	        if ( d->pagepoint )
	        {
	          if ( !pager_output(d) )
	          {
	            if ( d->character
	            && ( d->connected == CON_PLAYING
	            ||   d->connected == CON_EDITING ) )
	                save_char_obj( d->character );
	            d->outtop = 0;
	            close_socket(d, FALSE);
	          }
	        }
		else if ( !flush_buffer( d, TRUE ) )
		{
		    if ( d->character
		    && ( d->connected == CON_PLAYING
		    ||   d->connected == CON_EDITING ) )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d, FALSE );
		}
	    }
	    if ( d == last_descriptor )
	      break;
	}

	/*
	 * Synchronize to a clock.
	 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
	 * Careful here of signed versus unsigned arithmetic.
	 */
	{
	    struct timeval now_time;
	    long secDelta;
	    long usecDelta;

	    gettimeofday( &now_time, NULL );
	    usecDelta	= ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
			+ 1000000 / PULSE_PER_SECOND;
	    secDelta	= ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );
	    while ( usecDelta < 0 )
	    {
		usecDelta += 1000000;
		secDelta  -= 1;
	    }

	    while ( usecDelta >= 1000000 )
	    {
		usecDelta -= 1000000;
		secDelta  += 1;
	    }

	    if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
	    {
		struct timeval stall_time;

		stall_time.tv_usec = usecDelta;
		stall_time.tv_sec  = secDelta;
		if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 )
		{
		    perror( "game_loop: select: stall" );
		    exit( 1 );
		}
	    }
	}

	gettimeofday( &last_time, NULL );
	current_time = (time_t) last_time.tv_sec;

        /* Check every 5 seconds...  (don't need it right now)
	if ( last_check+5 < current_time )
	{
	  CHECK_LINKS(first_descriptor, last_descriptor, next, prev,
	      DESCRIPTOR_DATA);
	  last_check = current_time;
	}
	*/
    }
	fflush(stderr);	/* make sure strerr is flushed */
    return;
}


void new_descriptor( int new_desc )
{               
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *dnew;
    struct sockaddr_in sock;  
    struct hostent *from;
    int desc;
    int size;
/*    char bugbuf[MAX_STRING_LENGTH];*/
#ifdef WIN32
    unsigned long arg = 1;
#endif
    
    size = sizeof(sock);
    if ( check_bad_desc( new_desc ) )
    {
      set_alarm( 0 );
      return;   
    }
    set_alarm( 20 );
    alarm_section = "new_descriptor::accept";
    if ( ( desc = accept( new_desc, (struct sockaddr *) &sock, &size) ) < 0 )
    {
        set_alarm( 0 );
        return;
    }
    if ( check_bad_desc( new_desc ) )
    { 
      set_alarm( 0 );
      return;
    }
#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif
     
    set_alarm( 20 );
    alarm_section = "new_descriptor: after accept";
    
#ifdef WIN32
    if ( ioctlsocket(desc, FIONBIO, &arg) == -1 )
#else
    if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
#endif
    { 
        perror( "New_descriptor: fcntl: FNDELAY" );
        set_alarm( 0 );
        return;
    }
    if ( check_bad_desc( new_desc ) )
      return;
     
    CREATE( dnew, DESCRIPTOR_DATA, 1 );
    init_descriptor(dnew, desc );
    dnew->port = ntohs(sock.sin_port);
    strcpy( buf, inet_ntoa( sock.sin_addr ) );
    sprintf( log_buf, "Sock.sinaddr:  %s, port %hd.",
                buf, dnew->port );
    log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
    dnew->host = STRALLOC( buf );
    from = gethostbyaddr( (char *) &sock.sin_addr,
        sizeof(sock.sin_addr), AF_INET );
   CREATE( dnew->outbuf, char, dnew->outsize );
    
    strcpy( buf, inet_ntoa( sock.sin_addr ) );
    sprintf( log_buf, "Sock.sinaddr:  %s, port %hd.",
                buf, dnew->port );
    log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
    if ( sysdata.NO_NAME_RESOLVING )   
      dnew->host = STRALLOC( buf );
    else
    {
       from = gethostbyaddr( (char *) &sock.sin_addr,
                sizeof(sock.sin_addr), AF_INET );
       dnew->host = STRALLOC( (char *)( from ? from->h_name : buf) );
    }
    if ( check_total_bans( dnew ) )
    {
          write_to_descriptor (desc,
                         "Your site has been banned from this Mud.\n\r", 0);
          free_desc (dnew);
          set_alarm (0);
          return;
     }
     /*
     * Init descriptor data.
     */ 
     
    if ( !last_descriptor && first_descriptor )
    {
        DESCRIPTOR_DATA *d;
     
        bug( "New_descriptor: last_desc is NULL, but first_desc is not! ...fixing" );
        for ( d = first_descriptor; d; d = d->next )
           if ( !d->next )
                last_descriptor = d;
    }
          
    LINK( dnew, first_descriptor, last_descriptor, next, prev );
     /*
     * Send the greeting.   
     */ 
    {
        extern char * help_greeting;
        if ( help_greeting[0] == '.' )
            write_to_buffer( dnew, help_greeting+1, 0 );
        else
            write_to_buffer( dnew, help_greeting  , 0 );
    }
           
    alarm_section = "new_descriptor: set_auth";
    set_auth(dnew);
    alarm_section = "new_descriptor: after set_auth";
    
    if ( ++num_descriptors > sysdata.maxplayers )
       sysdata.maxplayers = num_descriptors;
    if ( sysdata.maxplayers > sysdata.alltimemax )
    {   
        if ( sysdata.time_of_max )
          DISPOSE(sysdata.time_of_max);
        sprintf(buf, "%24.24s", ctime(&current_time));
        sysdata.time_of_max = str_dup(buf);
        sysdata.alltimemax = sysdata.maxplayers;
        sprintf( log_buf, "Broke all-time maximum player record: %d", sysdata.alltimemax );
        log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
        to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
        save_sysdata( sysdata );
    }
    set_alarm(0);
    return;
}

void free_desc( DESCRIPTOR_DATA *d )
{
	kill_auth(d);
    closesocket( d->descriptor );
    STRFREE( d->host );
    DISPOSE( d->outbuf );
    STRFREE( d->user );    /* identd */
    if ( d->pagebuf )
	DISPOSE( d->pagebuf );
    DISPOSE( d );
    /*    --num_descriptors;  This is called from more than close_socket -- Alty */
    return;
}

void close_socket( DESCRIPTOR_DATA *dclose, bool force )
{
    CHAR_DATA *ch;
    DESCRIPTOR_DATA *d;
    bool DoNotUnlink = FALSE;

    /* flush outbuf */
    if ( !force && dclose->outtop > 0 )
	flush_buffer( dclose, FALSE );

    /* say bye to whoever's snooping this descriptor */
    if ( dclose->snoop_by )
	write_to_buffer( dclose->snoop_by,
	    "Your victim has left the game.\n\r", 0 );

    /* stop snooping everyone else */
    for ( d = first_descriptor; d; d = d->next )
	if ( d->snoop_by == dclose )
	  d->snoop_by = NULL;

    /* Check for switched people who go link-dead. -- Altrag */
    if ( dclose->original )
    {
	if ( ( ch = dclose->character ) != NULL )
	  do_return(ch, "");
	else
	{
	  bug( "Close_socket: dclose->original without character %s",
		(dclose->original->name ? dclose->original->name : "unknown") ,0 );
	  dclose->character = dclose->original;
	  dclose->original = NULL;
	}
    }
    
    ch = dclose->character;

    /* sanity check :( */
    if ( !dclose->prev && dclose != first_descriptor )
    {
	DESCRIPTOR_DATA *dp, *dn;
	bug( "Close_socket: %s desc:%p != first_desc:%p and desc->prev = NULL!",
		ch ? ch->name : d->host, dclose, first_descriptor,0 );
	dp = NULL;
	for ( d = first_descriptor; d; d = dn )
	{
	   dn = d->next;
	   if ( d == dclose )
	   {
		bug( "Close_socket: %s desc:%p found, prev should be:%p, fixing.",
		    ch ? ch->name : d->host, dclose, dp ,0 );
		dclose->prev = dp;
		break;
	   }
	   dp = d;
	}
	if ( !dclose->prev )
	{
	    bug( "Close_socket: %s desc:%p could not be found!.",
		    ch ? ch->name : dclose->host, dclose,0 );
	    DoNotUnlink = TRUE;
	}
    }
    if ( !dclose->next && dclose != last_descriptor )
    {
	DESCRIPTOR_DATA *dp, *dn;
	bug( "Close_socket: %s desc:%p != last_desc:%p and desc->next = NULL!",
		ch ? ch->name : d->host, dclose, last_descriptor,0 );
	dn = NULL;
	for ( d = last_descriptor; d; d = dp )
	{
	   dp = d->prev;
	   if ( d == dclose )
	   {
		bug( "Close_socket: %s desc:%p found, next should be:%p, fixing.",
		    ch ? ch->name : d->host, dclose, dn,0 );
		dclose->next = dn;
		break;
	   }
	   dn = d;
	}
	if ( !dclose->next )
	{
	    bug( "Close_socket: %s desc:%p could not be found!.",
		    ch ? ch->name : dclose->host, dclose,0 );
	    DoNotUnlink = TRUE;
	}
    }

    if ( dclose->character )
    {
	sprintf( log_buf, "Closing link to %s.", ch->name );
	log_string_plus( log_buf, LOG_COMM, UMAX( sysdata.log_level, ch->top_level ) );

/*
	if ( ch->top_level < LEVEL_DEMI )
	  to_channel( log_buf, CHANNEL_MONITOR, "Monitor", ch->top_level );
*/
	if ( dclose->connected == CON_PLAYING
	||   dclose->connected == CON_EDITING )
	{
	    act( AT_ACTION, "$g has lost $s link.", ch, NULL, NULL, TO_ROOM );
	    ch->desc = NULL;
	}
	else
	{
	    /* clear descriptor pointer to get rid of bug message in log */
	    dclose->character->desc = NULL;
	    free_char( dclose->character );
	}
    }


    if ( !DoNotUnlink )
    {
	/* make sure loop doesn't get messed up */
	if ( d_next == dclose )
	  d_next = d_next->next;
	UNLINK( dclose, first_descriptor, last_descriptor, next, prev );
    }

    if ( dclose->descriptor == maxdesc )
      --maxdesc;

    free_desc( dclose );
    --num_descriptors;
	return;
}


bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
    int iStart, iErr;

    /* Hold horses if pending command already. */
    if ( d->incomm[0] != '\0' )
	return TRUE;

    /* Check for overflow. */
    iStart = strlen(d->inbuf);
    if ( iStart >= sizeof(d->inbuf) - 10 )
    {
	sprintf( log_buf, "%s input overflow!", d->host );
	log_string( log_buf );
	write_to_descriptor( d->descriptor,
	    "\n\r*** PUT A LID ON IT!!! ***\n\rYou cannot enter the same command more than 20 consecutive times!\n\r", 0 );
	return FALSE;
    }

    for ( ; ; )
    {
	int nRead;

	nRead = read( d->descriptor, d->inbuf + iStart,
	    sizeof(d->inbuf) - 10 - iStart );
#ifdef WIN32
	iErr = WSAGetLastError ();
#else
	iErr = errno;
#endif
	if ( nRead > 0 )
	{
	    iStart += nRead;
	    if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
		break;
	}
	else if ( nRead == 0 )
	{
	    log_string_plus( "EOF encountered on read.", LOG_COMM, sysdata.log_level );
	    return FALSE;
	}
	else if ( errno == EWOULDBLOCK )
	    break;
	else
	{
	    perror( "Read_from_descriptor" );
	    return FALSE;
	}
    }

    d->inbuf[iStart] = '\0';
    return TRUE;
}



/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer( DESCRIPTOR_DATA *d )
{
    int i, j, k;

    /*
     * Hold horses if pending command already.
     */
    if ( d->incomm[0] != '\0' )
	return;

    /*
     * Look for at least one new line.
     */
    for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r' && i<MAX_INBUF_SIZE;
	  i++ )
    {
	if ( d->inbuf[i] == '\0' )
	    return;
    }

    /*
     * Canonical input processing.
     */
    for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
/*	if ( k >= 254 ) */
	if ( k >= 1000 )
	{
	    write_to_descriptor( d->descriptor, "Line too long.\n\r", 0 );

	    /* skip the rest of the line */
	    /*
	    for ( ; d->inbuf[i] != '\0' || i>= MAX_INBUF_SIZE ; i++ )
	    {
		if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
		    break;
	    }
	    */
	    d->inbuf[i]   = '\n';
	    d->inbuf[i+1] = '\0';
	    break;
	}

	if ( d->inbuf[i] == '\b' && k > 0 )
	    --k;
	else if ( isascii(d->inbuf[i]) && isprint(d->inbuf[i]) )
	    d->incomm[k++] = d->inbuf[i];
    }

    /*
     * Finish off the line.
     */
    if ( k == 0 )
	d->incomm[k++] = ' ';
    d->incomm[k] = '\0';

    /*
     * Deal with bozos with #repeat 1000 ...
     */
    if ( k > 1 || d->incomm[0] == '!' )
    {
	if ( d->incomm[0] != '!' && strcmp( d->incomm, d->inlast ) )
	{
	    d->repeat = 0;
	}
	else
	{
	    if ( ++d->repeat >= 20 )
	    {
		write_to_descriptor( d->descriptor,
		    "\n\r*** PUT A LID ON IT!!! ***\n\rYou cannot enter the same command more than 20 consecutive times!\n\r", 0 );
		/*strcpy( d->incomm, "quit" );*/
	    }
	}
    }

    /*
     * Do '!' substitution.
     */
    if ( d->incomm[0] == '!' )
	strcpy( d->incomm, d->inlast );
    else
	strcpy( d->inlast, d->incomm );

    /*
     * Shift the input buffer.
     */
    while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
	i++;
    for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ) != '\0'; j++ )
	;
    return;
}



/*
 * Low level output function.
 */
bool flush_buffer( DESCRIPTOR_DATA *d, bool fPrompt )
{
    char buf[MAX_INPUT_LENGTH];
    extern bool mud_down;

    /*
     * If buffer has more than 4K inside, spit out .5K at a time   -Thoric
     */
    if ( !mud_down && d->outtop > 4096 )
    {
        memcpy( buf, d->outbuf, 512 );
        d->outtop -= 512;
		memmove( d->outbuf, d->outbuf + 512, d->outtop );
	if ( d->snoop_by )
	{
	    char snoopbuf[MAX_INPUT_LENGTH];

	    buf[512] = '\0';
	    if ( d->character && d->character->name )
	    {
		if (d->original && d->original->name)
		    sprintf( snoopbuf, "%s (%s)", d->character->name, d->original->name );
		else          
		    sprintf( snoopbuf, "%s", d->character->name);
		write_to_buffer( d->snoop_by, snoopbuf, 0);
	    }
	    write_to_buffer( d->snoop_by, "% ", 2 );
	    write_to_buffer( d->snoop_by, buf, 0 );
	}
        if ( !write_to_descriptor( d->descriptor, buf, 512 ) )
        {
	    d->outtop = 0;
	    return FALSE;
        }
        return TRUE;
    }
                                                                                        

    /*
     * Bust a prompt.
     */
    if ( fPrompt && !mud_down && d->connected == CON_PLAYING )
    {
	CHAR_DATA *ch;

	ch = d->original ? d->original : d->character;
	if ( IS_SET(ch->act, PLR_BLANK) )
	    write_to_buffer( d, "\n\r", 2 );


	if ( IS_SET(ch->act, PLR_PROMPT) )
	    display_prompt(d);
	if ( IS_SET(ch->act, PLR_TELNET_GA) )
	    write_to_buffer( d, go_ahead_str, 0 );
    }

    /*
     * Short-circuit if nothing to write.
     */
    if ( d->outtop == 0 )
	return TRUE;

    /*
     * Snoop-o-rama.
     */
    if ( d->snoop_by )
    {
        /* without check, 'force mortal quit' while snooped caused crash, -h */
	if ( d->character && d->character->name )
	{
	    /* Show original snooped names. -- Altrag */
	    if ( d->original && d->original->name )
		sprintf( buf, "%s (%s)", d->character->name, d->original->name );
	    else
		sprintf( buf, "%s", d->character->name);
	    write_to_buffer( d->snoop_by, buf, 0);
	}
	write_to_buffer( d->snoop_by, "% ", 2 );
	write_to_buffer( d->snoop_by, d->outbuf, d->outtop );
    }

    /*
     * OS-dependent output.
     */
    if ( !write_to_descriptor( d->descriptor, d->outbuf, d->outtop ) )
    {
	d->outtop = 0;
	return FALSE;
    }
    else
    {
	d->outtop = 0;
	return TRUE;
    }
}



/*
 * Append onto an output buffer.
 */
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
    if ( !d )
    {
	bug( "Write_to_buffer: NULL descriptor" ,0);
	return;
    }

    /*
     * Normally a bug... but can happen if loadup is used.
     */
    if ( !d->outbuf )
    	return;

    /*
     * Find length in case caller didn't.
     */
    if ( length <= 0 )
	length = strlen(txt);

/* Uncomment if debugging or something
    if ( length != strlen(txt) )
    {
	bug( "Write_to_buffer: length(%d) != strlen(txt)!", length );
	length = strlen(txt);
    }
*/

    /*
     * Initial \n\r if needed.
     */
    if ( d->outtop == 0 && !d->fcommand )
    {
	d->outbuf[0]	= '\n';
	d->outbuf[1]	= '\r';
	d->outtop	= 2;
    }

    /*
     * Expand the buffer as needed.
     */
    while ( d->outtop + length >= d->outsize )
    {
        if (d->outsize > 32000)
	{
	    /* empty buffer */
	    d->outtop = 0;
	    close_socket(d, TRUE);
	    bug("Buffer overflow. Closing (%s).", d->character ? d->character->name : "???" ,0);
	    return;
 	}
	d->outsize *= 2;
	RECREATE( d->outbuf, char, d->outsize );
    }

    /*
     * Copy.
     */
    strncpy( d->outbuf + d->outtop, txt, length );
    d->outtop += length;
    d->outbuf[d->outtop] = '\0';
    return;
}


/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor( int desc, char *txt, int length )
{
    int iStart;
    int nWrite;
    int nBlock;

    if ( length <= 0 )
	length = strlen(txt);

    for ( iStart = 0; iStart < length; iStart += nWrite )
    {
	nBlock = UMIN( length - iStart, 4096 );
	if ( ( nWrite = write( desc, txt + iStart, nBlock ) ) < 0 )
	    { perror( "Write_to_descriptor" ); return FALSE; }
    }

    return TRUE;
}



void show_title( DESCRIPTOR_DATA *d )
{
    CHAR_DATA *ch;

    ch = d->character;

	set_char_color( AT_RED, ch );
	write_to_buffer( d, "---------------------------------------\n\r", 0 ); /* Added By Gavin*/
	set_char_color( AT_WHITE, ch );
	write_to_buffer( d, "Welcome to ", 0 ); /* SWR: THE Unknown Regions!!! */
	set_char_color( AT_RED, ch );
	write_to_buffer( d, "++(", 0 ); /* SWR: THE Unknown Regions!!! */
	set_char_color( AT_WHITE, ch );
	write_to_buffer( d, " SWR: The Unknown Regions ", 0 ); /* SWR: THE Unknown Regions!!! */
	set_char_color( AT_RED, ch );
	write_to_buffer( d, ")++ \n\r", 0 ); /* SWR: THE Unknown Regions!!! */
	set_char_color( AT_RED, ch );
	write_to_buffer( d, "---------------------------------------\n\r", 0 );
	set_char_color( AT_WHITE, ch );
	write_to_buffer( d, "Press enter...\n\r", 0 );
    d->connected = CON_PRESS_ENTER;
}

/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
    char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
    char arg[MAX_STRING_LENGTH];
    CHAR_DATA *ch;
    char *pwdnew;
    char *p;
    int iRace, iClass;
    bool fOld, chk;

    while ( isspace(*argument) )
		argument++;

    ch = d->character;

    switch ( d->connected )
    {
	default:
		bug( "Nanny: bad d->connected %d.", d->connected ,0);
		close_socket( d, TRUE );
		return;

	case CON_GET_NAME:
		if ( argument[0] == '\0' )
		{
			close_socket( d, FALSE );
			return;
		}
		
		argument[0] = UPPER(argument[0]);
		
		/* Old players can keep their characters. -- Alty */
		if ( !check_parse_name( argument, (d->newstate != 0) ) )
		{
			write_to_buffer( d, "Illegal name, try another.\n\rName: ", 0 );
			return;
		}
		if ( !str_cmp( argument, "New" ) )
		{
			if (d->newstate == 0)
			{
				/* New player */
				/* Don't allow new players if DENY_NEW_PLAYERS is true */
				if (sysdata.DENY_NEW_PLAYERS == TRUE)
				{
					sprintf( buf, "The mud is currently preparing for a reboot.\n\r" );
					write_to_buffer( d, buf, 0 );
					sprintf( buf, "New players are not accepted during this time.\n\r" );
					write_to_buffer( d, buf, 0 );
					sprintf( buf, "Please try again in a few minutes.\n\r" );
					write_to_buffer( d, buf, 0 );
					close_socket( d, FALSE );
				}
				sprintf( buf, "\n\rChoosing a name is one of the most important parts of this game...\n\r"
					"Make sure to pick a name appropriate to the character you are going\n\r"
					"to role play, and be sure that it suits our Star Wars theme.\n\r"
					"If the name you select is not acceptable, you will be asked to choose\n\r"
					"another one.\n\r\n\rPlease choose a name for your character: ");
				write_to_buffer( d, buf, 0 );
				d->newstate++;
				d->connected = CON_GET_NAME;
				return;
			} /* if (d->newstate == 0) */
			else
			{
				write_to_buffer(d, "Illegal name, try another.\n\rName: ", 0);
				return;
			}
		} /* if ( !str_cmp( argument, "New" ) ) */
		
		if ( check_playing( d, argument, FALSE ) == BERR )
		{
			write_to_buffer( d, "Name: ", 0 );
			return;
		}
		
		fOld = load_char_obj( d, argument, TRUE );
		if ( !d->character )
		{
			sprintf( log_buf, "Bad player file %s@%s.", argument, d->host );
			log_string( log_buf );
			write_to_buffer( d, "Your playerfile is corrupt...Please notify ur_gavin@hotmail.com.\n\r", 0 );
			close_socket( d, FALSE );
			return;
		}
		ch   = d->character;
		if ( check_bans( ch, BAN_SITE ) )
		{
			write_to_buffer (d, "Your site has been banned from this Mud.\n\r", 0);
			close_socket (d, FALSE);
			return;
		}
		
		if ( ch ) 
		{
			if ( check_bans( ch, BAN_CLASS ) )
			{
				write_to_buffer (d, "Your class has been banned from this Mud.\n\r", 0);
				close_socket (d, FALSE);
				return;
			}
			if ( check_bans( ch, BAN_RACE ) )
			{
				write_to_buffer (d, "Your race has been banned from this Mud.\n\r", 0);
				close_socket (d, FALSE);
				return;
			}
		} /* if ( fOld ) */

		if ( IS_SET(ch->act, PLR_DENY) )
		{
			sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
			log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
			if (d->newstate != 0)
			{
				write_to_buffer( d, "That name is already taken.  Please choose another: ", 0 );
				d->connected = CON_GET_NAME;
				d->character->desc = NULL;
				free_char( d->character ); /* Big Memory Leak before --Shaddai */
				d->character = NULL;
				return;
			}
			write_to_buffer( d, "You are denied access.\n\r", 0 );
			close_socket( d, FALSE );
			return; 
		} /* if ( IS_SET(ch->act, PLR_DENY) ) */
		
		chk = check_reconnect( d, argument, FALSE );
		if ( chk == BERR )
			return;	
		
		if ( chk )
		{
			fOld = TRUE;
		}
		else
		{
			if ( wizlock && !IS_IMMORTAL(ch) )
			{
				write_to_buffer( d, "The game is wizlocked.  Only immortals can connect now.\n\r", 0 );
				write_to_buffer( d, "Please try back later.\n\r", 0 );
				close_socket( d, FALSE );
				return;
			}
		} /* else - if ( chk ) */
		
		if ( fOld )
		{
			if (d->newstate != 0)
			{
				write_to_buffer( d, "That name is already taken.  Please choose another: ", 0 );
				d->connected = CON_GET_NAME;
				d->character->desc = NULL;
				free_char( d->character ); /* Big Memory Leak before --Shaddai */
				d->character = NULL;
				return;
			}
			/* Old player */
			write_to_buffer( d, "Password: ", 0 );
			write_to_buffer( d, echo_off_str, 0 );
			d->connected = CON_GET_OLD_PASSWORD;
			return;
		} /* if ( fOld ) */
		else
		{
			if (d->newstate == 0)
			{
				write_to_buffer( d, "\n\rNo such player exists.\n\rPlease check your spelling, or type new to start a new player.\n\r\n\rName: ", 0 );
				d->connected = CON_GET_NAME;
				d->character->desc = NULL;
				free_char( d->character ); 
				d->character = NULL;
				return;
			}
			
			sprintf( buf, "Did I get that right, %s (Y/N)? ", argument );
			write_to_buffer( d, buf, 0 );
			d->connected = CON_CONFIRM_NEW_NAME;
			return;
		} /* else */
		break;
		case CON_GET_OLD_PASSWORD:
			write_to_buffer( d, "\n\r", 2 );
			
			if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
			{
				write_to_buffer( d, "Wrong password.\n\r", 0 );
				/* clear descriptor pointer to get rid of bug message in log */
				d->character->desc = NULL;
				close_socket( d, FALSE );
				return;
			}
			
			write_to_buffer( d, echo_on_str, 0 );
			
			if ( check_playing( d, ch->name, TRUE ) )
				return;
			
			chk = check_reconnect( d, ch->name, TRUE );
			if ( chk == BERR )
			{
				if ( d->character && d->character->desc )
					d->character->desc = NULL;
				close_socket( d, FALSE );
				return;
			}
			if ( chk == TRUE )
				return;
			
			if ( check_multi( d , ch->name  ) )
			{
				close_socket( d, FALSE );
				return;
			}
			
			sprintf( buf, ch->name );
			d->character->desc = NULL;
			free_char( d->character );
			d->character = NULL;
			fOld = load_char_obj( d, buf, FALSE );
			ch = d->character;

			sprintf( log_buf, "%s@%s(%s) has connected.", ch->name, d->host, d->user );
			if ( ch->top_level < LEVEL_DEMI )
			{
				/*to_channel( log_buf, CHANNEL_MONITOR, "Monitor", ch->top_level );*/
				log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
			}
			else
				log_string_plus( log_buf, LOG_COMM, ch->top_level );
			if ( !IS_IMMORTAL(ch) )
			{
				sh_int char_age = get_age(ch);
				if ( char_age < 3 )
				{
					char agebuf [MAX_INPUT_LENGTH];
					if (char_age == 3)
						sprintf(agebuf, "You should not play for another year.\n\r" );
					else
						sprintf(agebuf, "You should not play for another %d years.\n\r", 
							(3 - char_age) );
					write_to_buffer(d, agebuf, 0);
					close_socket( d, FALSE );
					return;
				}
			}
			show_title(d);
			if ( ch->pcdata->area )
				do_loadarea(ch , "" );
			break;
		case CON_CONFIRM_NEW_NAME:
			switch ( *argument )
			{
			case 'y': case 'Y':
				sprintf( buf, "\n\rMake sure to use a password that won't be easily guessed by someone else."
					"\n\rPick a good password for %s: %s",
					ch->name, echo_off_str );
				write_to_buffer( d, buf, 0 );
				d->connected = CON_GET_NEW_PASSWORD;
				break;

			case 'n': case 'N':
				write_to_buffer( d, "Ok, what IS it, then? ", 0 );
				/* clear descriptor pointer to get rid of bug message in log */
				d->character->desc = NULL;
				free_char( d->character );
				d->character = NULL;
				d->connected = CON_GET_NAME;
				break;

			default:
				write_to_buffer( d, "Please type Yes or No. ", 0 );
				break;
			}
			break;
			
		case CON_GET_NEW_PASSWORD:
			write_to_buffer( d, "\n\r", 2 );

			if ( strlen(argument) < 5 )
			{
				write_to_buffer( d, 
					"Password must be at least five characters long.\n\rPassword: ", 
					0 );
				return;
			}

			pwdnew = crypt( argument, ch->name );
			for ( p = pwdnew; *p != '\0'; p++ )
			{
				if ( *p == '~' )
				{
					write_to_buffer( d,
						"New password not acceptable, try again.\n\rPassword: ",
						0 );
					return;
				}
			}

			DISPOSE( ch->pcdata->pwd );
			ch->pcdata->pwd	= str_dup( pwdnew );
			write_to_buffer( d, "\n\rPlease retype the password to confirm: ", 0 );
			d->connected = CON_CONFIRM_NEW_PASSWORD;
			break;

		case CON_CONFIRM_NEW_PASSWORD:
			write_to_buffer( d, "\n\r", 2 );

			if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
			{
				write_to_buffer( d, "Passwords don't match.\n\rRetype password: ",
					0 );
				d->connected = CON_GET_NEW_PASSWORD;
				return;
			}

			write_to_buffer( d, echo_on_str, 0 );
			write_to_buffer( d, "\n\rWould you like ANSI or no graphic/color support [A|N]? ", 0 );
			d->connected = CON_GET_WANT_RIPANSI;
			break;
			
		case CON_GET_WANT_RIPANSI:
			switch ( argument[0] )
			{
			case 'a': case 'A': SET_BIT(ch->act,PLR_ANSI);  break;
			case 'n': case 'N': break;
			default:
				write_to_buffer( d, "Invalid selection.\n\rANSI or NONE? ", 0 );
				return;
			}
			send_to_char( "\n\r&rWhat is your sex (&R[M]&rale or &R[F]&remale)? ", ch );
			d->connected = CON_GET_NEW_SEX;
			break;

		case CON_GET_NEW_SEX:
			switch ( argument[0] )
			{
			case 'm': case 'M': ch->sex = SEX_MALE;    break;
			case 'f': case 'F': ch->sex = SEX_FEMALE;  break;
			default:
				send_to_char( "&RThat's not a sex.\n\rWhat IS your sex?&w ", ch );
				return;
			}

/* ------------------------------------------------------------------------------------- */

			send_to_char( "\n\r&rPlease choose a main ability from the folowing list:\n\r", ch ); 
			buf[0] = '\0';
			for ( iClass = 0; iClass < MAX_ABILITY ; iClass++ )
			{
				if (ability_name[iClass] && ability_name[iClass][0] != '\0' )
				{
					sprintf( buf, "&r[&R%2d&r] [&R%-15.15s&r]   ",iClass,ability_name[iClass]);
					send_to_char( buf, ch );
					buf[0] = '\0';
					if ( (iClass % 3) == 2)
						send_to_char("\n\r",ch);
				}
			}
			strcat( buf, "\n\r: " );
			send_to_char( buf, ch );
			
			d->connected = CON_GET_NEW_CLASS;
			break; 

		case CON_GET_NEW_CLASS:
			argument = one_argument(argument, arg);
			if (!str_cmp(arg, "help"))
			{
				
				for ( iClass = 0; iClass < MAX_ABILITY ; iClass++ )
				{
					if (ability_name[iClass] && ability_name[iClass][0] != '\0' )
					{
						if (toupper(argument[0])==toupper(ability_name[iClass][0])
							&&   !str_prefix( argument, ability_name[iClass] ) )
						{
							do_help(ch, argument);
							send_to_char( "&rPlease choose an ability: ", ch );
							return;
						}
					}
				}  
				send_to_char( "&RNo such help topic&r.  Please choose an ability: ", ch );
				return;
			}
			if ( is_number(arg) )
			{
				iClass = atoi(arg);
				if ( iClass < 0 || iClass >= MAX_ABILITY )
				{
					iClass = MAX_ABILITY;
					send_to_char( "&rThat is &Rnot&r an ability.\n\rPlease pick a proper one. ", ch );
					return;
				}
				else
					ch->main_ability = iClass;
			} 
			else
			{
				for ( iClass = 0; iClass < MAX_ABILITY; iClass++ )
				{
					if ( toupper(arg[0]) == toupper(ability_name[iClass][0])
						&& !str_prefix( arg, ability_name[iClass] ) )
					{
						ch->main_ability = iClass;
						break;
					}
				}
			}
			if ( iClass == (MAX_ABILITY) || !ability_name[iClass] || ability_name[iClass][0] == '\0' )
			{
				send_to_char( "&rThat is &Rnot&r an ability.\n\rPlease pick a proper one. ", ch );
				return;
			}
			if ( check_bans( ch, BAN_CLASS ) )
			{
				send_to_char("&rThat class is currently &Rnot avaiable&r.\n\rPlease pick another. ",ch);
				return;
			}

			send_to_char( "\n\r&rYou may choose from the following races, or type &Rhelp [race]&r to learn more:\n\r", ch );
			buf[0] = '\0';
			for ( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
			{
				if ( race_table[iRace]->race_name && race_table[iRace]->race_name[0] != '\0'
					&& !IS_SET(race_table[iRace]->class_restriction, 1 << ch->main_ability) 
					&& str_cmp(race_table[iRace]->race_name,"unused") )
				{
					sprintf( buf, "&r[&R%2d&r] [&R%-15.15s&r]   ",
						iRace , race_table[iRace]->race_name);
					send_to_char( buf, ch );
					buf[0] = '\0';
					if ( (iRace % 3) == 2)
						send_to_char("\n\r",ch);
				}
				else if ( race_table[iRace]->race_name && race_table[iRace]->race_name[0] != '\0'
					&& str_cmp(race_table[iRace]->race_name,"unused") )
				{
					send_to_char( "                         ",ch);
					if ( (iRace % 3) == 2)
						send_to_char("\n\r",ch);
				}
			}
			strcat( buf, "\n\r: " );
			write_to_buffer( d, buf, 0 );
			d->connected = CON_GET_NEW_RACE;
			break;

		case CON_GET_NEW_RACE:
			argument = one_argument(argument, arg);
			if (!str_cmp( arg, "help") )
			{
				for ( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
				{
					if ( toupper(argument[0]) == toupper(race_table[iRace]->race_name[0])
						&&  !str_prefix( argument, race_table[iRace]->race_name) )
					{
						do_help(ch, argument);
						send_to_char( "&rPlease choose a race: ", ch);
						return;
					}
				}
				send_to_char( "&rNo help on that topic.  Please choose a race: ", ch );
				return;
			}
			if ( is_number(arg) )
			{
				iRace = atoi(arg);
				if ( iClass < 0 || iRace >= MAX_PC_RACE )
				{
					iRace = MAX_PC_RACE;
					send_to_char( "&rThat is &Rnot&r a race.\n\rPlease pick a proper one. ", ch );
					return;
				}
				else if ( iRace == MAX_PC_RACE
					||  !race_table[iRace]->race_name || race_table[iRace]->race_name[0] == '\0'
					||   IS_SET(race_table[iRace]->class_restriction, 1 << ch->main_ability )
					||   !str_cmp(race_table[iRace]->race_name,"unused") )
				{
					send_to_char( "&rThat's &Rnot&r a race.\n\rWhat &RIS&r your race going to be? ", ch );
					return;
				}
				else
					ch->race = iRace;
			} 
			else
			{
				for ( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
				{
					if ( toupper(arg[0]) == toupper(race_table[iRace]->race_name[0])
						&&   !str_prefix( arg, race_table[iRace]->race_name ) )
					{
						ch->race = iRace;
						break;
					}
				}
			}
			if ( iRace == MAX_PC_RACE
				||  !race_table[iRace]->race_name || race_table[iRace]->race_name[0] == '\0'
				||   IS_SET(race_table[iRace]->class_restriction, 1 << ch->main_ability )
				||   !str_cmp(race_table[iRace]->race_name,"unused") )
			{
				send_to_char( "&rThat's &Rnot&r a race.\n\rWhat &RIS&r your race going to be? ", ch );
				return;
			}
			
		if ( check_bans( ch, BAN_RACE ) )
        {
			send_to_char("&rThat race is currently &Rnot available&r.\n\rWhat is your race? ", ch);
			return;
		}
		
		send_to_char( "\n\r&rPlease choose a &Rhair color&r from the following list:\n\r", ch ); 
		buf[0] = '\0';
		
		for ( iClass = 0; iClass < MAX_HAIR; iClass++ )
		{
			sprintf( buf, "&r[&R%2d&r] [&R%-15.15s&r]   ",
				iClass , hair_color[iClass]);
			send_to_char( buf, ch );
			buf[0] = '\0';
			if ( (iClass % 3) == 2)
				send_to_char("\n\r",ch);
		}
		strcat( buf, "\n\r: " );
		write_to_buffer( d, buf, 0 );
		d->connected = CON_GET_NEW_HAIR;
	break;
	case CON_GET_NEW_HAIR:
		argument = one_argument(argument, arg);
		if (!str_cmp( arg, "help") )
		{
			do_help(ch, argument);
			send_to_char( "&rPlease choose a hair color: ", ch);
			return;
		}
		if ( is_number(arg) )
		{
			iClass = atoi(arg);
			if ( iClass < 0 || iClass >= MAX_HAIR )
			{
				iClass = MAX_HAIR;
				send_to_char( "&rThat is not a hair color.\n\rWhat &RIS&r it going to be? ", ch );
				return;
			}
			else
				ch->pcdata->haircolor = iClass;
		} 
		else
		{
			for ( iClass = 0; iClass < MAX_HAIR; iClass++ )
			{
				if ( toupper(arg[0]) == toupper(hair_color[iClass][0])
					&& !str_prefix( arg, hair_color[iClass] ) )
				{
					ch->pcdata->haircolor = iClass;
					break;
				}
			}
		}

		if ( iClass == MAX_HAIR || !hair_color[iClass] || hair_color[iClass][0] == '\0' )
		{
			send_to_char( "&rThat's not a hair color.\n\rWhat &RIS&r it going to be? ", ch );
			return;
		}
/* ------------------------------------------------------------------------------------- */

		send_to_char( "\n\r&rPlease choose an &Reye color&r from the following list:\n\r", ch ); 
		buf[0] = '\0';
		buf2[0] = '\0';
		
		for ( iClass = 0; iClass < MAX_EYE; iClass++ )
		{
			sprintf( buf, "&r[&R%2d&r] [&R%-15.15s&r]   ",
				iClass , eye_color[iClass]);
			send_to_char( buf, ch );
			buf[0] = '\0';
			if ( (iClass % 3) == 2)
				send_to_char("\n\r",ch);
		}
		strcat( buf, "\n\r: " );
		write_to_buffer( d, buf, 0 );
		d->connected = CON_GET_NEW_EYE;
		break;

	case CON_GET_NEW_EYE:
		argument = one_argument(argument, arg);
		if (!str_cmp( arg, "help") )
		{
			do_help(ch, argument);
			send_to_char( "&rPlease choose a different eye color: ", ch);
			return;
		}
		if ( is_number(arg) )
		{
			iClass = atoi(arg);
			if ( iClass < 0 || iClass >= MAX_EYE )
			{
				iClass = MAX_EYE;
				send_to_char( "&rThat's not a eye color.\n\rWhat &RIS&r it going to be? ", ch );
				return;
			}
			else
				ch->pcdata->eyes = iClass;
		} 
		else
		{
			for ( iClass = 0; iClass < MAX_EYE; iClass++ )
			{
				if ( toupper(arg[0]) == toupper(eye_color[iClass][0])
					&& !str_prefix( arg, eye_color[iClass] ) )
				{
					ch->pcdata->eyes = iClass;
					break;
				}
			}
		}

		if ( iClass == MAX_EYE || !eye_color[iClass] || eye_color[iClass][0] == '\0' )
		{
			send_to_char( "*rThat is not a eye color.\n\rWhat &RIS&r it going to be? ", ch );
			return;
		}
/* ------------------------------------------------------------------------------------- */

	
		send_to_char( "\n\r&rPlease choose &Ra build type &rfrom the following list:\n\r", ch ); 
		for ( iClass = 0; iClass < MAX_BUILD; iClass++ )
		{
			sprintf( buf, "&r[&R%2d&r] [&R%-15.15s&r]   ",
				iClass , build_type[iClass]);
			send_to_char( buf, ch );
			buf[0] = '\0';
			if ( (iClass % 3) == 2)
				send_to_char("\n\r",ch);
		}
		send_to_char( "\n\r: ", ch);
		d->connected = CON_GET_NEW_BUILD;

		break;
	case CON_GET_NEW_BUILD:
		argument = one_argument(argument, arg);
		if ( is_number(arg) )
		{
			iClass = atoi(arg);
			if ( iClass < 0 || iClass >= MAX_BUILD )
			{
				iClass = MAX_BUILD;
				send_to_char( "&rThat is not a build type.\n\rWhat &RIS&r it going to be? ",ch );
				return;
			}
			else
				ch->pcdata->build = iClass;
		} 
		else
		{
			for ( iClass = 0; iClass < MAX_BUILD; iClass++ )
			{
				if ( toupper(arg[0]) == toupper(build_type[iClass][0])
					&& !str_prefix( arg, build_type[iClass] ) )
				{
					ch->pcdata->build = iClass;
					break;
				}
			}
		}
		if ( iClass == MAX_BUILD || !build_type[iClass] || build_type[iClass][0] == '\0' )
		{
			send_to_char( "&rThat is not a build type.\n\rWhat &RIS&r it going to be? ", ch );
			return;
		}
	/* ------------------------------------------------------------------------------------- */

	send_to_char( "\n\r&rPlease choose a &Rcomplexion type&r from the following list:\n\r", ch ); 

	for ( iClass = 0; iClass < MAX_COMPLEX; iClass++ )
	{
		sprintf( buf, "&r[&R%2d&r] [&R%-15.15s&r]   ",
			iClass , complex_type[iClass]);
		send_to_char( buf, ch );
		buf[0] = '\0';
		if ( (iClass % 3) == 2)
			send_to_char("\n\r",ch);	
	}
	send_to_char( "\n\r: ",ch );
	d->connected = CON_GET_NEW_COMPLEX;

	break;
	case CON_GET_NEW_COMPLEX:
		argument = one_argument(argument, arg);
		if ( is_number(arg) )
		{
			iClass = atoi(arg);
			if ( iClass < 0 || iClass >= MAX_COMPLEX )
			{
				iClass = MAX_COMPLEX;
				send_to_char( "&rThat's not a complex type.\n\r"
					"What &RIS&r it going to be? ", ch );
				return;
			}
			else
				ch->pcdata->complex = iClass;
		} 
		else
		{
			for ( iClass = 0; iClass < MAX_COMPLEX; iClass++ )
			{
				if ( toupper(arg[0]) == toupper(complex_type[iClass][0])
					&& !str_prefix( arg, complex_type[iClass] ) )
				{
					ch->pcdata->complex = iClass;
					break;
				}
			}
		}
		if ( iClass == MAX_COMPLEX || !complex_type[iClass] || complex_type[iClass][0] == '\0' )
		{
			send_to_char( "&rThat's not a complex type.\n\r"
				"What &RIS&r it going to be? ", ch );
			return;
		}

/* ------------------------------------------------------------------------------------- */

		send_to_char( "\n\r&rPlease Pick a starting age &R[blank = 17]&r:", ch );
		d->connected = CON_GET_CHILD_BOOL;
        break;
    

		case CON_GET_CHILD_BOOL:
			ch->pcdata->age = 17;
			ch->pcdata->birthday = time_info;
			ch->pcdata->birthday.hour = time_info.hour;
			ch->pcdata->birthday.day = time_info.day;
			ch->pcdata->birthday.month = time_info.month;
			ch->pcdata->birthday.year = time_info.year;
			if ( argument[0] != '\0' ) 
			{
				int age;
				if ( is_number(argument))
				{
					age = atoi(argument);
					if ( age > 700 )
					{
						send_to_char( "\n\r&rThat age is &Rtoo high&r, please pick again:", ch);
						d->connected = CON_GET_CHILD_BOOL;
						return;
					}
					else if ( age < 8 )
					{
						send_to_char( "\n\rThat age is &Rtoo low&r, please pick again:", ch);
						d->connected = CON_GET_CHILD_BOOL;
						return;
					}
					else
					{
						ch->pcdata->age = age;
						ch->pcdata->birthday = time_info;
						ch->pcdata->birthday.hour = time_info.hour;
						ch->pcdata->birthday.day = time_info.day;
						ch->pcdata->birthday.month = time_info.month;
						ch->pcdata->birthday.year = time_info.year;
					}
				}
				else
				{
					send_to_char( "\n\r&rThat is not an age, please pick again:", ch);
					d->connected = CON_GET_CHILD_BOOL;
					return;
				}
			}
			else
			{
				ch->pcdata->age = 17;
				ch->pcdata->birthday = time_info;
				ch->pcdata->birthday.hour = time_info.hour;
				ch->pcdata->birthday.day = time_info.day;
				ch->pcdata->birthday.month = time_info.month;
				ch->pcdata->birthday.year = time_info.year;
			}
			ch->perm_str = number_range(1, 6)+number_range(1, 6)+number_range(1, 6);
			ch->perm_int = number_range(3, 6)+number_range(1, 6)+number_range(1, 6);
			ch->perm_wis = number_range(3, 6)+number_range(1, 6)+number_range(1, 6);
			ch->perm_dex = number_range(3, 6)+number_range(1, 6)+number_range(1, 6);
			ch->perm_con = number_range(3, 6)+number_range(1, 6)+number_range(1, 6);
			ch->perm_cha = number_range(3, 6)+number_range(1, 6)+number_range(1, 6);
			ch->perm_lck = number_range(3, 6)+number_range(1, 6)+number_range(1, 6);

			ch->perm_str	 += race_table[ch->race]->str_plus;
			ch->perm_int	 += race_table[ch->race]->int_plus;
			ch->perm_wis	 += race_table[ch->race]->wis_plus;
			ch->perm_dex	 += race_table[ch->race]->dex_plus;
			ch->perm_con	 += race_table[ch->race]->con_plus;
			ch->perm_cha	 += race_table[ch->race]->cha_plus;
			ch->perm_lck	 += race_table[ch->race]->lck_plus;
			
			send_to_char("&rDoes your mud client have the &RMud Sound Protocol&r? ", ch );
			d->connected = CON_GET_MSP; 
			break;
		
		case CON_GET_MSP:
			switch ( argument[0] )
			{
			case 'y': case 'Y': SET_BIT(ch->act,PLR_SOUND);  break;
			case 'n': case 'N': break;
			default:
				send_to_char( "&rInvalid selection.\n\r&RYES&r or &RNO&r? ", ch );
				return;
			}
			sprintf( log_buf, "%s@%s new %s.", ch->name, d->host,
				race_table[ch->race]->race_name);
			log_string_plus( log_buf, LOG_COMM, sysdata.log_level);
			to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
			send_to_char( "Press [ENTER] ", ch );
			show_title(d);
			{
				int ability;
				
				for ( ability =0 ; ability <  MAX_ABILITY ; ability++ )
					ch->skill_level[ability] = 0;
			}
			ch->top_level = 0;
			ch->position = POS_STANDING;
			d->connected = CON_PRESS_ENTER;
			return;
			break;

			case CON_PRESS_ENTER:
				if ( chk_watch(get_trust(ch), ch->name, d->host) ) /*  --Gorog */
					SET_BIT( ch->pcdata->flags, PCFLAG_WATCH );
				else
					REMOVE_BIT( ch->pcdata->flags, PCFLAG_WATCH );
				if ( IS_SET(ch->act, PLR_ANSI) )
					send_to_pager( "\033[2J", ch );
				else
					send_to_pager( "\014", ch );
				if ( ch->top_level > 0 )
				{
					send_to_pager( "\n\r&G&WMessage of the Day&w\n\r", ch );
					send_to_pager( "&G&R---------------------------&w\n\r", ch );
					do_help( ch, "motd" );
				}
				if ( ch->top_level == 100)
				{
					send_to_pager( "\n\r&G&WAvatar Message of the Day&w\n\r", ch );
					send_to_pager( "&G&R---------------------------&w\n\r", ch );
					do_help( ch, "amotd" );
				}
				if ( IS_IMMORTAL(ch) )
				{
					send_to_pager( "&G&WImmortal Message of the Day&w\n\r", ch );
					send_to_pager( "&G&R---------------------------&w\n\r", ch );
					do_help( ch, "imotd" );
				}
				if ( ch->top_level == 0 )
				{
					send_to_pager( "&G&WNewbie Message of the Day&w\n\r", ch );
					send_to_pager( "&G&R---------------------------&w\n\r", ch );
					do_help( ch, "nmotd" );
				}
				send_to_pager( "\n\r&WPress [ENTER] &Y", ch );
				d->connected = CON_READ_MOTD;
				if ( IS_SET( ch->act2, PCFLAG_MXP) )
				{
					send_to_char( "\033[1z<!ELEMENT RName FLAG=\"RoomName\" TAG=10>\n",ch);
					send_to_char( "\033[1z<!ELEMENT RDesc FLAG=\"RoomDesc\" TAG=11>\n",ch);
					send_to_char( "\033[1z<!ELEMENT RExits FLAG=\"RoomExit\" TAG=12>\n",ch);
					send_to_char( "\033[1z<!ELEMENT RNum FLAG=\"RoomNum\">\n",ch);
				}
				break;
			
			case CON_READ_MOTD:
		{
			char motdbuf[MAX_STRING_LENGTH];
			sprintf( motdbuf, "\n\rWelcome to %s...\n\r", sysdata.mud_name);
			write_to_buffer( d, motdbuf, 0 );
		}
		add_char( ch );
		d->connected	= CON_PLAYING;
		if ( !ch->pcdata->birthday.hour )
			ch->pcdata->birthday.hour = time_info.hour;
		if ( !ch->pcdata->birthday.day )
			ch->pcdata->birthday.day = time_info.day;
		if ( !ch->pcdata->birthday.month )
			ch->pcdata->birthday.month = time_info.month;
		if ( !ch->pcdata->birthday.year )
			ch->pcdata->birthday.year = time_info.year;
		if ( ch->top_level == 0 )
		{
			OBJ_DATA *obj;
			int iLang;
			
			ch->pcdata->clan_name = STRALLOC( "" );
			ch->pcdata->clan	  = NULL;
			
			ch->affected_by	  = race_table[ch->race]->affected;

			if ( !ch->pcdata->birthday.hour )
				ch->pcdata->birthday.hour = time_info.hour;
			if ( !ch->pcdata->birthday.day )
				ch->pcdata->birthday.day = time_info.day;
			if ( !ch->pcdata->birthday.month )
				ch->pcdata->birthday.month = time_info.month;
			if ( !ch->pcdata->birthday.year )
				ch->pcdata->birthday.year = time_info.year;
			
			ch->height = number_range(race_table[ch->race]->height *.9, race_table[ch->race]->height *1.1);
			ch->weight = number_range(race_table[ch->race]->weight *.9, race_table[ch->race]->weight *1.1);
			
		{ /* Langs Start */
			int sn;
			for ( iLang = 0; lang_array[iLang] != LANG_UNKNOWN; iLang++ ) 
			{
				if ( lang_array[iLang] == lang_array[race_table[ch->race]->language] )
				{
					if ( (sn = skill_lookup(lang_names[iLang])) != -1 )
					{
						ch->pcdata->learned[sn] = 100;
						ch->speaks = lang_array[iLang];
						ch->speaking = lang_array[iLang];
					}
				}
			}
		} /* Langs End */
		
		name_stamp_stats( ch );
		
		{
			int ability;
			
			for ( ability =0 ; ability < MAX_ABILITY ; ability++ )
			{
				ch->skill_level[ability] = 1;
				ch->experience[ability] = 0;
			}
		}
		ch->top_level = 1;
		ch->full_name = STRALLOC( ch->name );
		ch->hit	= ch->max_hit;
		ch->move = ch->max_move;
		ch->max_mana = 0;
		ch->mana	= ch->max_mana;
		sprintf( buf, "%s the %s",ch->name, race_table[ch->race]->race_name );
	    set_title( ch, buf );
		
		/* Added by Narn.  Start new characters with autoexit and autgold
		already turned on.  Very few people don't use those. */
		SET_BIT( ch->act, PLR_AUTOGOLD ); 
		SET_BIT( ch->act, PLR_AUTOEXIT ); 
		
		/*Gives the player 3k credits */
		ch->gold += 3000;
		
		/* New players don't have to earn some eq */
		obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_BANNER), 0 );
		obj_to_char( obj, ch );
		equip_char( ch, obj, WEAR_LIGHT );

		obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_DAGGER), 0 );
		obj_to_char( obj, ch );
		equip_char( ch, obj, WEAR_WIELD );

		/* comlink */
		{
			OBJ_INDEX_DATA *obj_ind = get_obj_index( 10424 );
			if ( obj_ind != NULL )
			{
				obj = create_object( obj_ind, 0 );
				obj_to_char( obj, ch );
			}
		}
		
		if (!sysdata.WAIT_FOR_AUTH)
		{
			char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
			/*ch->pcdata->auth_state = 3; */
		}
		else
		{
			char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
			ch->pcdata->auth_state = 1;
			SET_BIT(ch->pcdata->flags, PCFLAG_UNAUTHED);
		}
		/* Display_prompt interprets blank as default */
		ch->pcdata->prompt = STRALLOC("");
		ch->pcdata->fprompt = STRALLOC("");
		ch->pcdata->comchan = 0;
	} /* if ( ch->top_level == 0 ) */
	else
	{
		if ( !IS_IMMORTAL(ch) && ch->pcdata->release_date > current_time )
		{
			char_to_room( ch, get_room_index(6) );
		}
		else if ( ch->in_room && !IS_IMMORTAL( ch ) 
			&& !xIS_SET( ch->in_room->room_flags, ROOM_SPACECRAFT )
			&& ch->in_room != get_room_index(6) )
		{
			char_to_room( ch, ch->in_room );
		}
		else if ( ch->in_room && !IS_IMMORTAL( ch )  
			&& xIS_SET( ch->in_room->room_flags, ROOM_SPACECRAFT )
			&& ch->in_room != get_room_index(6) )
		{
			SHIP_DATA *ship;
			
			for ( ship = first_ship; ship; ship = ship->next )
				if ( ch->in_room->vnum >= ship->firstroom && ch->in_room->vnum <= ship->lastroom )
					if ( ship->class != SHIP_PLATFORM || ship->starsystem ) 
						char_to_room( ch, ch->in_room );
		}
		else
		{
			char_to_room( ch, get_room_index( wherehome(ch) ) );
		}
		
		if ( get_timer( ch, TIMER_SHOVEDRAG ) > 0 )
			remove_timer( ch, TIMER_SHOVEDRAG );
			
		if ( get_timer( ch, TIMER_PKILLED ) > 0 )
			remove_timer( ch, TIMER_PKILLED );
		if ( ch->pcdata->plr_home != NULL )
		{
			char filename[256];
			FILE *fph;
			ROOM_INDEX_DATA *storeroom = ch->pcdata->plr_home;
			OBJ_DATA *obj;
			OBJ_DATA *obj_next;
			
			for ( obj = storeroom->first_content; obj; obj = obj_next )
			{
				obj_next = obj->next_content;
				extract_obj( obj );
			}
			
			sprintf( filename, "%s%c/%s.home", PLAYER_DIR, tolower(ch->name[0]), capitalize( ch->name ) );
			if ( ( fph = fopen( filename, "r" ) ) != NULL )
			{
				int iNest;
				bool found;
				OBJ_DATA *tobj, *tobj_next;
				
				rset_supermob(storeroom);
				for ( iNest = 0; iNest < MAX_NEST; iNest++ )
					rgObjNest[iNest] = NULL;
					
				found = TRUE;
				for ( ; ; )
				{
					char letter;
					char *word;
					
					letter = fread_letter( fph );
					if ( letter == '*' )
					{
						fread_to_eol( fph );
						continue;
					}
					
					if ( letter != '#' )
					{
						bug( "Load_plr_home: # not found.", 0 );
						bug( ch->name, 0 );
						break;
					}
					
					word = fread_word( fph );
					if ( !str_cmp( word, "OBJECT" ) )	/* Objects	*/
						fread_obj  ( supermob, fph, OS_CARRY );
					else if ( !str_cmp( word, "END"    ) )	/* Done		*/
						break;
					else
					{
						bug( "Load_plr_home: bad section.", 0 );
						bug( ch->name, 0 );
						break;
					}
				} /* for ( ; ; ) */
				
				fclose( fph );
				
				for ( tobj = supermob->first_carrying; tobj; tobj = tobj_next )
				{
					tobj_next = tobj->next_content;
					obj_from_char( tobj );
					obj_to_room( tobj, storeroom );
				}
				
				release_supermob();
			}	/* if ( ( fph = fopen( filename, "r" ) ) != NULL ) */
		} /* if ( ch->pcdata->plr_home != NULL ) */
		
		act( AT_ACTION, "$n has entered the game.", ch, NULL, NULL, TO_ROOM );
		if ( ch->pcdata->pet )
		{
			act( AT_ACTION, "$n returns to $s master from the Void.", ch->pcdata->pet, NULL, ch, TO_NOTVICT );
			act( AT_ACTION, "$N returns with you to the realms.", ch, NULL, ch->pcdata->pet, TO_CHAR );
		} 
		do_look( ch, "auto" );
		mail_count(ch);
		break;
	}
	}
	return;
}

bool is_reserved_name( char *name )
{
  RESERVE_DATA *res;
  
  for (res = first_reserved; res; res = res->next)
    if ((*res->name == '*' && !str_infix(res->name+1, name)) ||
        !str_cmp(res->name, name))
      return TRUE; 
  return FALSE;
}


bool check_parse_name( char *name, bool newchar )
{
 /*
  * Names checking should really only be done on new characters, otherwise
  * we could end up with people who can't access their characters.  Would
  * have also provided for that new area havoc mentioned below, while still
  * disallowing current area mobnames.  I personally think that if we can
  * have more than one mob with the same keyword, then may as well have
  * players too though, so I don't mind that removal.  -- Alty
  */
  
/*    if ( is_name( name, "all auto someone immortal self god supreme demigod dog guard cityguard cat cornholio spock hicaine hithoric death ass fuck shit piss crap quit " ) && newchar )
	return FALSE;
	
	if ( is_name( name, "chewbacca chewy " ) && newchar )
	return FALSE; */

	if ( is_reserved_name(name) && newchar )
	return FALSE;
 
    /*
     * Length restrictions.
     */
    if ( strlen(name) <  3 )
	return FALSE;

    if ( strlen(name) > 12 )
	return FALSE;

    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
	char *pc;
	bool fIll;

	fIll = TRUE;
	for ( pc = name; *pc != '\0'; pc++ )
	{
	    if ( !isalpha(*pc) )
		return FALSE;
	    if ( LOWER(*pc) != 'i' && LOWER(*pc) != 'l' )
		fIll = FALSE;
	}

	if ( fIll )
	    return FALSE;
    }

    /*
     * Code that followed here used to prevent players from naming
     * themselves after mobs... this caused much havoc when new areas
     * would go in...
     */

    return TRUE;
}


/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect( DESCRIPTOR_DATA *d, char *name, bool fConn )
{
    CHAR_DATA *ch;

    for ( ch = first_char; ch; ch = ch->next )
    {
	if ( !IS_NPC(ch)
	&& ( !fConn || !ch->desc )
	&&    ch->name
	&&   !str_cmp( name, ch->name ) )
	{
	    if ( fConn && ch->switched )
	    {
	      write_to_buffer( d, "Already playing.\n\rName: ", 0 );
	      d->connected = CON_GET_NAME;
	      if ( d->character )
	      {
		 /* clear descriptor pointer to get rid of bug message in log */
		 d->character->desc = NULL;
		 free_char( d->character );
		 d->character = NULL;
	      }
	      return BERR;
	    }
	    if ( fConn == FALSE )
	    {
		DISPOSE( d->character->pcdata->pwd );
		d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
	    }
	    else
	    {
		/* clear descriptor pointer to get rid of bug message in log */
		d->character->desc = NULL;
		free_char( d->character );
		d->character = ch;
		ch->desc	 = d;
		ch->timer	 = 0;
		send_to_char( "Reconnecting.\n\r", ch );
		act( AT_ACTION, "$n has reconnected.", ch, NULL, NULL, TO_ROOM );
		sprintf( log_buf, "%s@%s(%s) reconnected.", ch->name, d->host, d->user );
		log_string_plus( log_buf, LOG_COMM, UMAX( sysdata.log_level, ch->top_level ) );
/*
		if ( ch->top_level < LEVEL_SAVIOR )
		  to_channel( log_buf, CHANNEL_MONITOR, "Monitor", ch->top_level );
*/
		if ( IS_SET( ch->act2, PCFLAG_MXP) )
		{
			ch_printf( ch, "\033[1z<!ELEMENT RName FLAG=\"RoomName\" TAG=10>\n");
			ch_printf( ch, "\033[1z<!ELEMENT RDesc FLAG=\"RoomDesc\" TAG=11>\n");
			ch_printf( ch, "\033[1z<!ELEMENT RExits FLAG=\"RoomExit\" TAG=12>\n");
			ch_printf( ch, "\033[1z<!ELEMENT RNum FLAG=\"RoomNum\">\n");
		}		
		d->connected = CON_PLAYING;
	    }
	    return TRUE;
	}
    }

    return FALSE;
}



/*
 * Check if already playing.
 */
 
bool check_multi( DESCRIPTOR_DATA *d , char *name )
{
	DESCRIPTOR_DATA *dold;
	
	for ( dold = first_descriptor; dold; dold = dold->next )
	{
		if ( dold != d && (  dold->character || dold->original )
			&& str_cmp( name, dold->original ? dold->original->name : dold->character->name ) 
			&& !str_cmp(dold->host , d->host ) )
		{
			const char *ok = "64.59.146.78";
			const char *ok2 = "24.113.40.59";
			int iloop;
			
			if ( get_trust( d->character ) >= LEVEL_IMPLEMENTOR
				|| get_trust( dold->original ? dold->original : dold->character ) >= LEVEL_IMPLEMENTOR )
				return FALSE;
			for ( iloop = 0 ; iloop < 11 ; iloop++ )
			{
				if ( ok[iloop] != d->host[iloop] )
					break;
			}
			if ( iloop >= 10 )
				return FALSE; 
			for ( iloop = 0 ; iloop < 11 ; iloop++ )
			{
				if ( ok2[iloop] != d->host[iloop] )
				break;
			}
			if ( iloop >= 10 )
				return FALSE; 
			write_to_buffer( d, "Sorry multi-playing is not allowed ... have you other character quit first.\n\r", 0 );
			sprintf( log_buf, "%s attempting to multiplay with %s.", dold->original ? dold->original->name : dold->character->name , d->character->name );
			log_string_plus( log_buf, LOG_COMM, sysdata.log_level );	
			d->character = NULL;
			free_char( d->character );
			return TRUE;
		}
	}
	return FALSE;
}                

bool check_playing( DESCRIPTOR_DATA *d, char *name, bool kick )
{
    CHAR_DATA *ch;

    DESCRIPTOR_DATA *dold;
    int	cstate;

    for ( dold = first_descriptor; dold; dold = dold->next )
    {
	if ( dold != d
	&& (  dold->character || dold->original )
	&&   !str_cmp( name, dold->original
		 ? dold->original->name : dold->character->name ) )
	{
	    cstate = dold->connected;
	    ch = dold->original ? dold->original : dold->character;
	    if ( !ch->name
	    || ( cstate != CON_PLAYING && cstate != CON_EDITING ) )
	    {
		write_to_buffer( d, "Already connected - try again.\n\r", 0 );
		sprintf( log_buf, "%s already connected.", ch->name );
		log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
		return BERR;
	    }
	    if ( !kick )
	      return TRUE;
	    write_to_buffer( d, "Already playing... Kicking off old connection.\n\r", 0 );
	    write_to_buffer( dold, "Kicking off old connection... bye!\n\r", 0 );
	    close_socket( dold, FALSE );
	    /* clear descriptor pointer to get rid of bug message in log */
	    d->character->desc = NULL;
	    free_char( d->character );
	    d->character = ch;
	    ch->desc	 = d;
	    ch->timer	 = 0;
	    if ( ch->switched )
	      do_return( ch->switched, "" );
	    ch->switched = NULL;
	    send_to_char( "Reconnecting.\n\r", ch );
	    act( AT_ACTION, "$n has reconnected, kicking off old link.", ch, NULL, NULL, TO_ROOM );
	    sprintf( log_buf, "%s@%s reconnected, kicking off old link.", ch->name, d->host );
	    log_string_plus( log_buf, LOG_COMM, UMAX( sysdata.log_level, ch->top_level ) );
		if ( IS_SET( ch->act2, PCFLAG_MXP) )
		{
			ch_printf( ch, "\033[1z<!ELEMENT RName FLAG=\"RoomName\" TAG=10>\n");
			ch_printf( ch, "\033[1z<!ELEMENT RDesc FLAG=\"RoomDesc\" TAG=11>\n");
			ch_printf( ch, "\033[1z<!ELEMENT RExits FLAG=\"RoomExit\" TAG=12>\n");
			ch_printf( ch, "\033[1z<!ELEMENT RNum FLAG=\"RoomNum\">\n");
		}
/*
	    if ( ch->top_level < LEVEL_SAVIOR )
	      to_channel( log_buf, CHANNEL_MONITOR, "Monitor", ch->top_level );
*/
	    d->connected = cstate;
	    return TRUE;
	}
    }

    return FALSE;
}



void stop_idling( CHAR_DATA *ch )
{
    if ( !ch
    ||   !ch->desc
    ||    ch->desc->connected != CON_PLAYING
    ||   !ch->was_in_room
    ||    ch->in_room != get_room_index( ROOM_VNUM_LIMBO ) )
	return;

    ch->timer = 0;
    char_from_room( ch );
    char_to_room( ch, ch->was_in_room );
    ch->was_in_room	= NULL;
    act( AT_ACTION, "$n has returned from the void.", ch, NULL, NULL, TO_ROOM );
    return;
}

/*
 * Same as above, but converts &color codes to ANSI sequences..
 */
void send_to_char_color( const char *txt, CHAR_DATA *ch )
{
  DESCRIPTOR_DATA *d;
  char *colstr;
  const char *prevstr = txt;
  char colbuf[20];
  int ln;
  
  if ( !ch )
  {
    bug( "Send_to_char_color: NULL *ch",0 );
    return;
  }
  if ( !txt || !ch->desc )
    return;
  d = ch->desc;
  /* Clear out old color stuff */
/*  make_color_sequence(NULL, NULL, NULL);*/
  while ( (colstr = strpbrk(prevstr, "&^")) != NULL )
  {
    if (colstr > prevstr)
       write_to_buffer(d, prevstr, (colstr-prevstr));
    ln = make_color_sequence(colstr, colbuf, d);
    if ( ln < 0 )
    {
      prevstr = colstr+1;
      break;
    }
    else if ( ln > 0 )
      write_to_buffer(d, colbuf, ln);
    prevstr = colstr+2;
  }
  if ( *prevstr )
    write_to_buffer(d, prevstr, 0);
  return;
}

void write_to_pager( DESCRIPTOR_DATA *d, const char *txt, int length )
{

  int pageroffset;	/* Pager fix by thoric */

  if ( length <= 0 )
    length = strlen(txt);
  if ( length == 0 )
    return;
  if ( !d->pagebuf )
  {
    d->pagesize = MAX_STRING_LENGTH;
    CREATE( d->pagebuf, char, d->pagesize );
  }
  if ( !d->pagepoint )
  {
    d->pagepoint = d->pagebuf;
    d->pagetop = 0;
    d->pagecmd = '\0';
  }
  if ( d->pagetop == 0 && !d->fcommand )
  {
    d->pagebuf[0] = '\n';
    d->pagebuf[1] = '\r';
    d->pagetop = 2;
  }
  pageroffset = d->pagepoint - d->pagebuf;	/* pager fix (goofup fixed 08/21/97) */
  while ( d->pagetop + length >= d->pagesize )
  {
    if ( d->pagesize > 32000 )
    {
      bug( "Pager overflow.  Ignoring.\n\r",0 );
      d->pagetop = 0;
      d->pagepoint = NULL;
      DISPOSE(d->pagebuf);
      d->pagesize = MAX_STRING_LENGTH;
      return;
    }
    d->pagesize *= 2;
    RECREATE(d->pagebuf, char, d->pagesize);
  }
  d->pagepoint = d->pagebuf + pageroffset;	/* pager fix (goofup fixed 08/21/97) */
  strncpy(d->pagebuf+d->pagetop, txt, length);
  d->pagetop += length;
  d->pagebuf[d->pagetop] = '\0';
  return;
}

void send_to_pager_color( const char *txt, CHAR_DATA *ch )
{
  DESCRIPTOR_DATA *d;
  char *colstr;
  const char *prevstr = txt;
  char colbuf[20];
  int ln;
  
  if ( !ch )
  {
    bug( "Send_to_pager_color: NULL *ch",0 );
    return;
  }
  if ( !txt || !ch->desc )
    return;
  d = ch->desc;
  ch = d->original ? d->original : d->character;
  if ( IS_NPC(ch) || !IS_SET(ch->pcdata->flags, PCFLAG_PAGERON) )
  {
    send_to_char_color(txt, d->character);
    return;
  }
  /* Clear out old color stuff */
/*  make_color_sequence(NULL, NULL, NULL);*/
  while ( (colstr = strpbrk(prevstr, "&^")) != NULL )
  {
    if ( colstr > prevstr )
      write_to_pager(d, prevstr, (colstr-prevstr));
    ln = make_color_sequence(colstr, colbuf, d);
    if ( ln < 0 )
    {
      prevstr = colstr+1;
      break;
    }
    else if ( ln > 0 )
      write_to_pager(d, colbuf, ln);
    prevstr = colstr+2;
  }
  if ( *prevstr )
    write_to_pager(d, prevstr, 0);
  return;
}

void set_char_color( sh_int AType, CHAR_DATA *ch )
{
    char buf[16];
    CHAR_DATA *och;
    
    if ( !ch || !ch->desc )
      return;
    
    och = (ch->desc->original ? ch->desc->original : ch);
    if ( !IS_NPC(och) && IS_SET(och->act, PLR_ANSI) )
    {
	if ( AType == 7 )
	  strcpy( buf, "\033[m" );
	else
	  sprintf(buf, "\033[0;%d;%s%dm", (AType & 8) == 8,
	        (AType > 15 ? "5;" : ""), (AType & 7)+30);
	write_to_buffer( ch->desc, buf, strlen(buf) );
    }
    return;
}

void set_pager_color( sh_int AType, CHAR_DATA *ch )
{
    char buf[16];
    CHAR_DATA *och;
    
    if ( !ch || !ch->desc )
      return;
    
    och = (ch->desc->original ? ch->desc->original : ch);
    if ( !IS_NPC(och) && IS_SET(och->act, PLR_ANSI) )
    {
	if ( AType == 7 )
	  strcpy( buf, "\033[m" );
	else
	  sprintf(buf, "\033[0;%d;%s%dm", (AType & 8) == 8,
	        (AType > 15 ? "5;" : ""), (AType & 7)+30);
	send_to_pager( buf, ch );
	ch->desc->pagecolor = AType;
    }
    return;
}

/*
 * The primary output interface for formatted output.
 */
/* Major overhaul. -- Alty */

void ch_printf_color(CHAR_DATA *ch, char *fmt, ...)
{
     char buf[MAX_STRING_LENGTH*2];
     va_list args;
 
     va_start(args, fmt);
     vsprintf(buf, fmt, args);
     va_end(args);
 
     send_to_char_color(buf, ch);
}
 
void pager_printf_color(CHAR_DATA *ch, char *fmt, ...)
{
     char buf[MAX_STRING_LENGTH*2];
     va_list args;
 
     va_start(args, fmt);
     vsprintf(buf, fmt, args);
     va_end(args);
 
     send_to_pager_color(buf, ch);
}

void ch_printf(CHAR_DATA *ch, char *fmt, ...)
{
     char buf[MAX_STRING_LENGTH*2];
     va_list args;
 
     va_start(args, fmt);
     vsprintf(buf, fmt, args);
     va_end(args);
 
     send_to_char_color(buf, ch);
}
 
void pager_printf(CHAR_DATA *ch, char *fmt, ...)
{
     char buf[MAX_STRING_LENGTH*2];
     va_list args;
 
     va_start(args, fmt);
     vsprintf(buf, fmt, args);
     va_end(args);
 
     send_to_pager_color(buf, ch);
}

/*  From Erwin  */

void log_printf(char *fmt, ...)
{
  char buf[MAX_STRING_LENGTH*2];
  va_list args;

  va_start(args, fmt);
  vsprintf(buf, fmt, args);
  va_end(args);

  log_string(buf);
}


char *obj_short( OBJ_DATA *obj )
{
    static char buf[MAX_STRING_LENGTH];

    if ( obj->count > 1 )
    {
	sprintf( buf, "%s (%d)", obj->short_descr, obj->count );
	return buf;
    }
    return obj->short_descr;
}

/*
 * The primary output interface for formatted output.
 */
/* Major overhaul. -- Alty */
#define NAME(ch)	(IS_NPC(ch) ? ch->short_descr : ch->full_name[0] != '\0' ? ch->full_name : ch->name )
#define NAME2(ch)	(IS_NPC(ch) ? ch->short_descr : ch->name )
char *act_string(const char *format, CHAR_DATA *to, CHAR_DATA *ch,
		 const void *arg1, const void *arg2, int flags)
{
	static char * const he_she  [] = { "it",  "he",  "she" };
	static char * const him_her [] = { "it",  "him", "her" };
	static char * const his_her [] = { "its", "his", "her" };
	static char buf[MAX_STRING_LENGTH];
	char fname[MAX_INPUT_LENGTH];
	char *point = buf;
	const char *str = format;
	const char *i;
	CHAR_DATA *vch = (CHAR_DATA *) arg2;
	OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
	OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;

    if ( str[0] == '$' )
		DONT_UPPER = FALSE;

	while ( *str != '\0' )
	{
		if ( *str != '$' )
		{
			*point++ = *str++;
			continue;
		}
		++str;
		if ( !arg2 && *str >= 'A' && *str <= 'Z' )
		{
			bug( "Act: missing arg2 for code %c:", *str,0 );
			bug( format,0 );
			i = " <@@@> ";
		}
		else
		{
			switch ( *str )
			{
				default:  bug( "Act: bad code %c.", *str,0 );
				i = " <@@@> ";						break;
				case 't': i = (char *) arg1;					break;
				case 'T': i = (char *) arg2;					break;
				case 'V': case 'v': i = (char *) ch->name;				break;
				case 'n': i = (to ? PERS( ch, to) : NAME( ch));		  break;
				case 'N': i = (to ? PERS(vch, to) : NAME(vch));			break;
				case 'G': i = (can_hear_chan(to, ch) ? NAME2(ch) : "someone");			break;
				case 'g': i = (to ? PERS( ch, to) : NAME2( ch));		  break;
				case 'e': if (ch->sex > 2 || ch->sex < 0)
				{
					bug("act_string: player %s has sex set at %d!", ch->name, ch->sex,0);
					i = "it";
				}
				else
					i = he_she [URANGE(0,  ch->sex, 2)];
				break;
				case 'E': if (vch->sex > 2 || vch->sex < 0)
				{
					bug("act_string: player %s has sex set at %d!", vch->name, vch->sex,0);
					i = "it";
				}
				else
					i = he_she [URANGE(0, vch->sex, 2)];
				break;
				case 'm': if (ch->sex > 2 || ch->sex < 0)
				{
					bug("act_string: player %s has sex set at %d!", ch->name, ch->sex,0);
					i = "it";
				}
				else
					i = him_her[URANGE(0,  ch->sex, 2)];
				break;
				case 'M': if (vch->sex > 2 || vch->sex < 0)
				{
					bug("act_string: player %s has sex set at %d!", vch->name, vch->sex,0);
					i = "it";
				}
				else
					i = him_her[URANGE(0, vch->sex, 2)];
				break;
				case 's': if (ch->sex > 2 || ch->sex < 0)
				{
					bug("act_string: player %s has sex set at %d!", ch->name, ch->sex,0);
					i = "its";
				}
				else
					i = his_her[URANGE(0,  ch->sex, 2)];
				break;
				case 'S': if (vch->sex > 2 || vch->sex < 0)
				{
					bug("act_string: player %s has sex set at %d!", vch->name, vch->sex,0);
					i = "its";
				}
				else
					i = his_her[URANGE(0, vch->sex, 2)];
				break;
				case 'q': i = (to == ch) ? "" : "s";				break;
				case 'Q': i = (to == ch) ? "your" : his_her[URANGE(0,  ch->sex, 2)];			break;
				case 'p': i = (!to || can_see_obj(to, obj1) ? obj_short(obj1) : "something");			break;
				case 'P': i = (!to || can_see_obj(to, obj2) ? obj_short(obj2) : "something");			break;
				case 'd':
					if ( !arg2 || ((char *) arg2)[0] == '\0' )
						i = "door";
					else
					{
						one_argument((char *) arg2, fname);
						i = fname;
					}
					break;
			}
		}
		++str;
		while ( (*point = *i) != '\0' )
			++point, ++i;
	}
	strcpy(point, "\n\r");
	if ( !DONT_UPPER )
		buf[0] = UPPER(buf[0]);
	return buf;
}
#undef NAME
#undef NAME2
  
void act( sh_int AType, const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type )
{
    char *txt;
    CHAR_DATA *to;
    CHAR_DATA *vch = (CHAR_DATA *)arg2;

    /*
     * Discard null and zero-length messages.
     */
    if ( !format || format[0] == '\0' )
	return;

    if ( !ch )
    {
	bug( "Act: null ch. (%s)", format,0 );
	return;
    }

    if ( !ch->in_room )
      to = NULL;
    else if ( type == TO_CHAR )
      to = ch;
    else
      to = ch->in_room->first_person;

    /*
     * ACT_SECRETIVE handling
     */
    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_SECRETIVE) && type != TO_CHAR )
	return;

    if ( type == TO_VICT )
    {
	if ( !vch )
	{
	    bug( "Act: null vch with TO_VICT.",0 );
	    bug( "%s (%s)", ch->name, format ,0);
	    return;
	}
	if ( !vch->in_room )
	{
	    bug( "Act: vch in NULL room!",0 );
	    bug( "%s -> %s (%s)", ch->name, vch->name, format,0 );
	    return;
	}
	to = vch;
/*	to = vch->in_room->first_person;*/
    }

    if ( MOBtrigger && type != TO_CHAR && type != TO_VICT && to )
    {
      OBJ_DATA *to_obj;
      
            txt = act_string(format, NULL, ch, arg1, arg2, STRING_IMM);
      if ( IS_SET(to->in_room->progtypes, ACT_PROG) )
        rprog_act_trigger(txt, to->in_room, ch, (OBJ_DATA *)arg1, (void *)arg2);
      for ( to_obj = to->in_room->first_content; to_obj;
            to_obj = to_obj->next_content )
        if ( IS_SET(to_obj->pIndexData->progtypes, ACT_PROG) )
          oprog_act_trigger(txt, to_obj, ch, (OBJ_DATA *)arg1, (void *)arg2);
    }

    /* Anyone feel like telling me the point of looping through the whole
       room when we're only sending to one char anyways..? -- Alty */
    for ( ; to; to = (type == TO_CHAR || type == TO_VICT)
                     ? NULL : to->next_in_room )
    {
	if ((!to->desc 
	&& (  IS_NPC(to) && !IS_SET(to->pIndexData->progtypes, ACT_PROG) ))
	||   !IS_AWAKE(to) )
	    continue;
	
	if ( type == TO_CHAR && to != ch )
	    continue;
	if ( type == TO_VICT && ( to != vch || to == ch ) )
	    continue;
	if ( type == TO_ROOM && to == ch )
	    continue;
	if ( type == TO_NOTVICT && (to == ch || to == vch) )
	    continue;
	if ( type == TO_CANSEE && ( to == ch || 
	    (!IS_IMMORTAL(to) && !IS_NPC(ch) && (IS_SET(ch->act, PLR_WIZINVIS) 
	    && (get_trust(to) < (ch->pcdata ? ch->pcdata->wizinvis : 0) ) ) ) ) )
	    continue;

        if ( IS_IMMORTAL(to) )
            txt = act_string (format, to, ch, arg1, arg2, STRING_IMM);
        else
       	    txt = act_string (format, to, ch, arg1, arg2, STRING_NONE);

	if (to->desc)
	{
	 set_char_color(AType, to);
	  send_to_char_color( txt, to );
	}
	if (MOBtrigger)
        {
          /* Note: use original string, not string with ANSI. -- Alty */
	  mprog_act_trigger( txt, to, ch, (OBJ_DATA *)arg1, (void *)arg2 );
        }
    }
    MOBtrigger = TRUE;
    return;
}
void do_name( CHAR_DATA *ch, char *argument )
{
	char fname[1024];
	struct stat fst;
	CHAR_DATA *tmp;
	char log_buf[MAX_STRING_LENGTH];

	if ( !NOT_AUTHED(ch) || ch->pcdata->auth_state != 2)
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	argument[0] = UPPER(argument[0]);
	
	if (!check_parse_name(argument, TRUE))
	{
		send_to_char("Illegal name, try another.\n\r", ch);
		return;
	}
	if (!str_cmp(ch->name, argument))
	{
		send_to_char("That's already your name!\n\r", ch);
		return;
	}
	for ( tmp = first_char; tmp; tmp = tmp->next )
	{
		if (!str_cmp(argument, tmp->name))
			break;
	}
	if ( tmp )
	{
		send_to_char("That name is already taken.  Please choose another.\n\r", ch);
		return;
	}
	sprintf( fname, "%s%c/%s", PLAYER_DIR, tolower(argument[0]),
		capitalize( argument ) );
	if ( stat( fname, &fst ) != -1 )
	{
		send_to_char("That name is already taken.  Please choose another.\n\r", ch);
		return;
	}
	STRFREE( ch->name );
	ch->name = STRALLOC( argument );
	STRFREE( ch->full_name );
	ch->full_name = STRALLOC( argument );
	STRFREE( ch->pcdata->filename );
	ch->pcdata->filename = STRALLOC( argument ); 
	send_to_char("Your name has been changed.  Please apply again.\n\r", ch);
	sprintf( log_buf, "%s@%s new %s %s %s applying...", 
		ch->name, ch->desc->host, 
		ch->sex == SEX_MALE ? "Male" : ch->sex == SEX_FEMALE ? "Female" : "Neutral",
		race_table[ch->race]->race_name, ability_name[ch->main_ability] );
	log_string( log_buf );
	to_channel( log_buf, CHANNEL_AUTH, "Auth", LEVEL_IMMORTAL );
	add_timer(ch, TIMER_APPLIED, 10, NULL, 0);
	ch->pcdata->auth_state = 1;
	return;
}

char *default_prompt( CHAR_DATA *ch , char * argument )
{
	if ( !argument || argument[0] == '\0' || !str_cmp("default",argument) )
	{
		static char buf[MAX_STRING_LENGTH];
		strcpy( buf,"" );
		strcat(buf, "&P[&R%T&P] &PH&pealth&P:&R%h&B/&R%H &PM&pove&P:&R%v&B/&R%V");
		strcat(buf, "&C >&w");
		return buf;
	}
	else
	{
		static char buf[MAX_STRING_LENGTH];
		strcpy( buf,"" );
		strcat(buf, "&P[&R%T&P] &PH&pealth&P:&R%h&B/&R%H &PM&pove&P:&R%v&B/&R%V");
		strcat(buf, "&C >&w");
		return buf;
	}
	return "";
}

char *default_fprompt( CHAR_DATA *ch , char * argument )
{
	if ( !argument || argument[0] == '\0' || !str_cmp("default",argument))
	{
		static char buf[MAX_STRING_LENGTH];
		strcpy( buf,"" );
		strcat(buf,"&BHealth:&R%h&r/&R%H &BMove:&R%m&r/&R%M &BOpponent:&r[&R%c&r]");
		strcat(buf, "&C >&w");
		return buf;
	}
	else
	{
		static char buf[MAX_STRING_LENGTH];
		strcpy( buf,"" );
		strcat(buf,"&BHealth:&R%h&r/&R%H &BMove:&R%m&r/&R%M &BOpponent:&r[&R%c&r]");
		strcat(buf, "&C >&w");
		return buf;
	}
	return "";
}

int getcolor(char clr)
{
  static const char colors[16] = "xrgObpcwzRGYBPCW";
  int r;
  
  for ( r = 0; r < 16; r++ )
    if ( clr == colors[r] )
      return r;
  return -1;
}

void display_prompt( DESCRIPTOR_DATA *d )
{
	CHAR_DATA *ch = d->character;
	CHAR_DATA *och = (d->original ? d->original : d->character);
	CHAR_DATA *victim;
	bool ansi = (!IS_NPC(och) && IS_SET(och->act, PLR_ANSI));
	const char *prompt;
	const char *helpstart = "<Type HELP START>";
	char buf[MAX_STRING_LENGTH];
	char *pbuf = buf;
	int stat, percent;

	if ( !ch )
	{
		bug( "display_prompt: NULL ch",0 );
		return;
	}

	if ( !IS_NPC(ch) && !IS_SET(ch->pcdata->flags, PCFLAG_HELPSTART ) )
		prompt = helpstart;
	else if ( !IS_NPC(ch) && ch->substate != SUB_NONE && ch->pcdata->subprompt
		&&   ch->pcdata->subprompt[0] != '\0' )
		prompt = ch->pcdata->subprompt;
	else if (IS_NPC (ch) || (!ch->fighting && (!ch->pcdata->prompt
		|| !*ch->pcdata->prompt) ) )
		prompt = default_prompt (ch,"default");
	else if ( ch->fighting )
	{
		if ( !ch->pcdata->fprompt || !*ch->pcdata->fprompt )
			prompt = default_fprompt ( ch, "default" );
		else
			prompt = ch->pcdata->fprompt;
	}
	else
		prompt = ch->pcdata->prompt;
	if ( ansi )
	{
	strcpy(pbuf, "\033[m");
	d->prevcolor = 0x07;
	pbuf += 3;
	}

	/* Clear out old color stuff */
	/*  make_color_sequence(NULL, NULL, NULL);*/
	for ( ; *prompt; prompt++ )
	{
	/*
     * '&' = foreground color/intensity bit
     * '^' = background color/blink bit
     * '%' = prompt commands
     * Note: foreground changes will revert background to 0 (black)
     */
		if ( *prompt != '&' && *prompt != '^' && *prompt != '%' )
		{
			*(pbuf++) = *prompt;
			continue;
		}
		++prompt;
		if ( !*prompt )
			break;
		if ( *prompt == *(prompt-1) )
		{
			*(pbuf++) = *prompt;
			continue;
		}
		switch(*(prompt-1))
		{
		default:
			bug( "Display_prompt: bad command char '%c'.", *(prompt-1),0 );
			break;
		case '&':
		case '^':
			stat = make_color_sequence(&prompt[-1], pbuf, d);
			if ( stat < 0 )
				--prompt;
			else if ( stat > 0 )
				pbuf += stat;
			break;
		case '%':
			*pbuf = '\0';
			stat = 0x80000000;
			switch(*prompt)
			{
			case '%':
				*pbuf++ = '%';
				*pbuf = '\0';
				break;
			case 'a':
				if ( IS_GOOD(ch) )
					strcpy(pbuf, "good");
				else if ( IS_EVIL(ch) )
					strcpy(pbuf, "evil");
				else
					strcpy(pbuf, "neutral");
				break;
			case 'A':
				sprintf( pbuf, "%s%s%s", IS_AFFECTED( ch, AFF_INVISIBLE ) ? "I" : "",
					IS_AFFECTED( ch, AFF_HIDE )      ? "H" : "",
					IS_AFFECTED( ch, AFF_SNEAK )     ? "S" : "" );
				break;
			case 'C':  /* Tank */
				if ( !ch->fighting || ( victim = ch->fighting->who ) == NULL )
					strcpy( pbuf, "N/A" );
				else if(!victim->fighting||(victim = victim->fighting->who)==NULL)
					strcpy( pbuf, "N/A" );
				else {
					if ( ch->max_hit > 0 )
						percent = (100 * ch->hit) / ch->max_hit;
					else
						percent = -1;
					if (percent >= 100)
						strcpy (pbuf, "perfect health");
					else if (percent >= 90)
						strcpy (pbuf, "slightly scratched");
					else if (percent >= 80)
						strcpy (pbuf, "few bruises");
                    else if (percent >= 70)
						strcpy (pbuf, "some cuts");
					else if (percent >= 60)
						strcpy (pbuf, "several wounds");
					else if (percent >= 50)
						strcpy (pbuf, "nasty wounds");
					else if (percent >= 40)
						strcpy (pbuf, "bleeding freely");
					else if (percent >= 30)
						strcpy (pbuf, "covered in blood");
                    else if (percent >= 20)
						strcpy (pbuf, "leaking guts");
					else if (percent >= 10)
						strcpy (pbuf, "almost dead");
					else
						strcpy (pbuf, "DYING");
				}
				break;
			case 'c':
				/*if ( !IS_IMMORTAL( ch ) ) break;*/
				if ( !ch->fighting || ( victim = ch->fighting->who ) == NULL )
					strcpy( pbuf, "N/A" );
				else {
					if ( victim->max_hit > 0 )
						percent = (100 * victim->hit) / victim->max_hit;
					else
						percent = -1;
					if (percent >= 100)
						strcpy (pbuf, "perfect health");
                    else if (percent >= 90)
						strcpy (pbuf, "slightly scratched");
                    else if (percent >= 80)
						strcpy (pbuf, "few bruises");
                    else if (percent >= 70)
						strcpy (pbuf, "some cuts");
                    else if (percent >= 60)
						strcpy (pbuf, "several wounds");
                    else if (percent >= 50)
						strcpy (pbuf, "nasty wounds");
                    else if (percent >= 40)
						strcpy (pbuf, "bleeding freely");
                    else if (percent >= 30)
						strcpy (pbuf, "covered in blood");
                    else if (percent >= 20)
						strcpy (pbuf, "leaking guts");
                    else if (percent >= 10)
						strcpy (pbuf, "almost dead");
                    else
						strcpy (pbuf, "DYING");
				}
				break;
			case 'h':
				stat = ch->hit;
				break;
			case 'H':
				stat = ch->max_hit;
				break;
			case 'N': /* Tank */
				if ( !IS_IMMORTAL(ch) ) break;
				if ( !ch->fighting || ( victim = ch->fighting->who ) == NULL )
					strcpy( pbuf, "N/A" );
				else if(!victim->fighting||(victim=victim->fighting->who)==NULL)
					strcpy( pbuf, "N/A" );
				else {
					if ( ch == victim )
						strcpy ( pbuf, "You" );
					else if ( IS_NPC(victim) )
						strcpy ( pbuf, victim->short_descr );
					else
						strcpy ( pbuf, victim->name );
					pbuf[0] = UPPER( pbuf[0] );
				}
				break;
			case 'n':
				if ( !IS_IMMORTAL(ch) ) break;
				if (!ch->fighting || (victim = ch->fighting->who) == NULL )
					strcpy( pbuf, "N/A" );
				else {
					if ( ch == victim )
						strcpy ( pbuf, "You" );
					else if ( IS_NPC(victim) )
						strcpy ( pbuf, victim->short_descr );
					else
						strcpy ( pbuf, victim->name );
					pbuf[0] = UPPER( pbuf[0] );
				}
				break;
			case 'T':
				if      ( time_info.hour <  5 ) strcpy( pbuf, "night" );
				else if ( time_info.hour <  6 ) strcpy( pbuf, "dawn" );
				else if ( time_info.hour < 19 ) strcpy( pbuf, "day" );
				else if ( time_info.hour < 21 ) strcpy( pbuf, "dusk" );
				else                            strcpy( pbuf, "night" );
				break;
			case 'u':
				stat = num_descriptors;
				break;
			case 'U':
				stat = sysdata.maxplayers;
				break;
			case 'v':
				stat = ch->move;
				break;
			case 'V':
				stat = ch->max_move;
				break;
			case 'g':
				stat = ch->gold;
				break;
			case 'G':
				if ( !IS_NPC(och) )
					stat = ch->pcdata->bank;
				break;
			case 'r':
				if ( IS_IMMORTAL(och) )
					stat = ch->in_room->vnum;
				break;
			case 'R':
				if ( IS_SET(och->pcdata->flags, PCFLAG_ROOMVNUM) )
					sprintf(pbuf, "<#%d> ", ch->in_room->vnum);
				break;
			case 'i':
				if ( (!IS_NPC(ch) && IS_SET(ch->act, PLR_WIZINVIS)) ||
					(IS_NPC(ch) && IS_SET(ch->act, ACT_MOBINVIS)) )
					sprintf(pbuf, "(Invis %d) ", (IS_NPC(ch) ? ch->mobinvis : ch->pcdata->wizinvis));
				else
					if ( IS_AFFECTED(ch, AFF_INVISIBLE) )
						sprintf(pbuf, "(Invis) " );
					break;
			case 'I':
				stat = (IS_NPC(ch) ? (IS_SET(ch->act, ACT_MOBINVIS) ? ch->mobinvis : 0)
					: (IS_SET(ch->act, PLR_WIZINVIS) ? ch->pcdata->wizinvis : 0));
				break;
			}
			if ( stat != 0x80000000 )
				sprintf(pbuf, "%d", stat);
			pbuf += strlen(pbuf);
			break;
		}
	}
	*pbuf = '\0';
	write_to_buffer(d, buf, (pbuf-buf));
	return;
}

int make_color_sequence(const char *col, char *buf, DESCRIPTOR_DATA *d)
{
  int ln;
  const char *ctype = col;
  unsigned char cl;
  CHAR_DATA *och;
  bool ansi;
  
  och = (d->original ? d->original : d->character);
  ansi = (!IS_NPC(och) && IS_SET(och->act, PLR_ANSI));
  col++;
  if ( !*col )
    ln = -1;
  else if ( *ctype != '&' && *ctype != '^' )
  {
    bug("Make_color_sequence: command '%c' not '&' or '^'.", *ctype,0);
    ln = -1;
  }
  else if ( *col == *ctype )
  {
    buf[0] = *col;
    buf[1] = '\0';
    ln = 1;
  }
  else if ( !ansi )
    ln = 0;
  else
  {
    cl = d->prevcolor;
    switch(*ctype)
    {
    default:
      bug( "Make_color_sequence: bad command char '%c'.", *ctype,0 );
      ln = -1;
      break;
    case '&':
      if ( *col == '-' )
      {
        buf[0] = '~';
        buf[1] = '\0';
        ln = 1;
        break;
      }
    case '^':
      {
        int newcol;
        
        if ( (newcol = getcolor(*col)) < 0 )
        {
          ln = 0;
          break;
        }
        else if ( *ctype == '&' )
          cl = (cl & 0xF0) | newcol;
        else
          cl = (cl & 0x0F) | (newcol << 4);
      }
      if ( cl == d->prevcolor )
      {
        ln = 0;
        break;
      }
      strcpy(buf, "\033[");
	  if ( (cl & 0x88) != (d->prevcolor & 0x88) )
      {
        strcat(buf, "m\033[");
        if ( (cl & 0x08) )
          strcat(buf, "1;");
        if ( (cl & 0x80) )
          strcat(buf, "5;");
        d->prevcolor = 0x07 | (cl & 0x88);
        ln = strlen(buf);
      }
      else
        ln = 2;
      if ( (cl & 0x07) != (d->prevcolor & 0x07) )
      {
        sprintf(buf+ln, "3%d;", cl & 0x07);
        ln += 3;
      }
      if ( (cl & 0x70) != (d->prevcolor & 0x70) )
      {
        sprintf(buf+ln, "4%d;", (cl & 0x70) >> 4);
        ln += 3;
      }
      if ( buf[ln-1] == ';' )
        buf[ln-1] = 'm';
      else
      {
        buf[ln++] = 'm';
        buf[ln] = '\0';
      }
      d->prevcolor = cl;
    }
  }
  if ( ln <= 0 )
    *buf = '\0';
  return ln;
}

void set_pager_input( DESCRIPTOR_DATA *d, char *argument )
{
  while ( isspace(*argument) )
    argument++;
  d->pagecmd = *argument;
  return;
}

bool pager_output( DESCRIPTOR_DATA *d )
{
  register char *last;
  CHAR_DATA *ch;
  int pclines;
  register int lines;
  bool ret;

  if ( !d || !d->pagepoint || d->pagecmd == -1 )
    return TRUE;
  ch = d->original ? d->original : d->character;
  pclines = UMAX(ch->pcdata->pagerlen, 5) - 1;
  switch(LOWER(d->pagecmd))
  {
  default:
    lines = 0;
    break;
  case 'b':
    lines = -1-(pclines*2);
    break;
  case 'r':
    lines = -1-pclines;
    break;
  case 'q':
    d->pagetop = 0;
    d->pagepoint = NULL;
    flush_buffer(d, TRUE);
    DISPOSE(d->pagebuf);
    d->pagesize = MAX_STRING_LENGTH;
    return TRUE;
  }
  while ( lines < 0 && d->pagepoint >= d->pagebuf )
    if ( *(--d->pagepoint) == '\n' )
      ++lines;
  if ( *d->pagepoint == '\n' && *(++d->pagepoint) == '\r' )
      ++d->pagepoint;
  if ( d->pagepoint < d->pagebuf )
    d->pagepoint = d->pagebuf;
  for ( lines = 0, last = d->pagepoint; lines < pclines; ++last )
    if ( !*last )
      break;
    else if ( *last == '\n' )
      ++lines;
  if ( *last == '\r' )
    ++last;
  if ( last != d->pagepoint )
  {
    if ( !write_to_descriptor(d->descriptor, d->pagepoint,
          (last-d->pagepoint)) )
      return FALSE;
    d->pagepoint = last;
  }
  while ( isspace(*last) )
    ++last;
  if ( !*last )
  {
    d->pagetop = 0;
    d->pagepoint = NULL;
    flush_buffer(d, TRUE);
    DISPOSE(d->pagebuf);
    d->pagesize = MAX_STRING_LENGTH;
    return TRUE;
  }
  d->pagecmd = -1;
  if ( IS_SET( ch->act, PLR_ANSI ) )
      if ( write_to_descriptor(d->descriptor, "\033[1;36m", 7) == FALSE )
	return FALSE;
  if ( (ret=write_to_descriptor(d->descriptor,
	"(C)ontinue, (R)efresh, (B)ack, (Q)uit: [C] ", 0)) == FALSE )
	return FALSE;
  if ( IS_SET( ch->act, PLR_ANSI ) )
  {
      char buf[32];

      if ( d->pagecolor == 7 )
	strcpy( buf, "\033[0m" );
      else
	sprintf(buf, "\033[0;%d;%s%dm", (d->pagecolor & 8) == 8,
		(d->pagecolor > 15 ? "5;" : ""), (d->pagecolor & 7)+30);
      ret = write_to_descriptor( d->descriptor, buf, 0 );
  }
  return ret;
}


void do_news( CHAR_DATA *ch, char *argument )
{
  do_help( ch, "news" );
}

/*
 * Function to strip off the "a" or "an" or "the" or "some" from an object's
 * short description for the purpose of using it in a sentence sent to
 * the owner of the object.  (Ie: an object with the short description
 * "a long dark blade" would return "long dark blade" for use in a sentence
 * like "Your long dark blade".  The object name isn't always appropriate
 * since it contains keywords that may not look proper.		-Thoric
 */
char *myobj( OBJ_DATA *obj )
{
    if ( !str_prefix("a ", obj->short_descr) )
	return obj->short_descr + 2;
    if ( !str_prefix("an ", obj->short_descr) )
	return obj->short_descr + 3;
    if ( !str_prefix("the ", obj->short_descr) )
	return obj->short_descr + 4;
    if ( !str_prefix("some ", obj->short_descr) )
	return obj->short_descr + 5;
    return obj->short_descr;
}

#ifdef WIN32

void shutdown_mud( char *reason );

void bailout(void)
{
    echo_to_all( AT_IMMORT, "MUD shutting down by system operator NOW!!", ECHOTAR_ALL );
    shutdown_mud( "MUD shutdown by system operator" );
    log_string ("MUD shutdown by system operator");
    Sleep (5000);		/* give "echo_to_all" time to display */
    mud_down = TRUE;		/* This will cause game_loop to exit */
    service_shut_down = TRUE;	/* This will cause characters to be saved */
    fflush(stderr);
    return;
}

#endif
