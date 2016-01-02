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
*		            Bounty Hunter Module    			   *   
*                    (  and area capturing as well  )                      * 
****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "mud.h"


BOUNTY_DATA * first_bounty;
BOUNTY_DATA * last_bounty;
BOUNTY_DATA * first_disintigration;
BOUNTY_DATA * last_disintigration;


void   disintigration args ( ( CHAR_DATA *ch , char * name , long amount, char * reason ) );
void nodisintigration args ( ( CHAR_DATA *ch , CHAR_DATA *victim , long amount ) );
int xp_compute( CHAR_DATA *ch , CHAR_DATA *victim);

void save_disintigrations()
{    
    BOUNTY_DATA *tbounty;
    FILE *fpout;
    char filename[256];
    
    sprintf( filename, "%s%s", SYSTEM_DIR, DISINTIGRATION_LIST );
    fpout = fopen( filename, "w" );
    if ( !fpout )
    {
         bug( "FATAL: cannot open disintigration.lst for writing!\n\r", 0 );
         return;
    }
    for ( tbounty = first_disintigration; tbounty; tbounty = tbounty->next )
    {
        fprintf( fpout, "%s\n", tbounty->target );
        fprintf( fpout, "%ld\n", tbounty->amount );
    }
    fprintf( fpout, "$\n" );
    fclose( fpout );
    
}
                                                                    

bool is_disintigration( CHAR_DATA *victim )
{
    BOUNTY_DATA *bounty;
    
    for ( bounty = first_disintigration; bounty; bounty = bounty->next )
    if ( !str_cmp( victim->name , bounty->target ) )
             return TRUE;
    return FALSE;                           
}

BOUNTY_DATA *get_disintigration( char *target )
{
    BOUNTY_DATA *bounty;
    
    for ( bounty = first_disintigration; bounty; bounty = bounty->next )
       if ( !str_cmp( target, bounty->target ) )
         return bounty;
    return NULL;
}

void load_bounties( )
{
    FILE *fpList;
    char *target;
    char bountylist[256];
    BOUNTY_DATA *bounty;
    long int  amount;
     
    first_disintigration = NULL;
    last_disintigration	= NULL;

    log_string( "Loading disintigrations..." );

    sprintf( bountylist, "%s%s", SYSTEM_DIR, DISINTIGRATION_LIST );
    fclose( fpReserve );
    if ( ( fpList = fopen( bountylist, "r" ) ) == NULL )
    {
	perror( bountylist );
	exit( 1 );
    }

    for ( ; ; )
    {
        target = feof( fpList ) ? "$" : fread_word( fpList );
        if ( target[0] == '$' )
        break;                                  
	CREATE( bounty, BOUNTY_DATA, 1 );
        LINK( bounty, first_disintigration, last_disintigration, next, prev );
	bounty->target = STRALLOC(target);
	amount = fread_number( fpList );
	bounty->amount = amount;
    }
    fclose( fpList );
    log_string(" Done bounties " );
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}

void do_bounties( CHAR_DATA *ch, char *argument )
{
    BOUNTY_DATA *bounty;
    int count = 0;
    
    set_char_color( AT_WHITE, ch );
	if ( !str_cmp("list",argument) && IS_IMMORTAL(ch) )
	{
	    set_pager_color( AT_WHITE, ch );
		show_file( ch, BOUNTY_LOG_FILE );
		return;
	}
	else if ( !str_cmp("clear", argument) && IS_IMMORTAL(ch) )
	{
		if ( 0==remove( BOUNTY_LOG_FILE ) )
		{
			send_to_pager( "Ok. Bounty File has been cleared.\n\r", ch);
			return;
		}
		send_to_pager( "There is no bounty file to clear.\n\r", ch);
		return;
	}
	else
	{
		ch_printf( ch, "&P+&B--------------------------------------------------------------&P+&B\n\r");
		ch_printf( ch, "&B| &RBounty                      &RAmount                           &B|\n\r" );
		for ( bounty = first_disintigration; bounty; bounty = bounty->next )
		{
			ch_printf( ch, "&B| &R%-26s %-14.14s                    &B|\n\r", 
				bounty->target, num_punct(bounty->amount) );
			count++;
		}
		if ( !count )
		{
			ch_printf( ch, "&B| &YThere are no bounties set at this time.                      &B|\n\r", ch );
			ch_printf( ch, "&P+&B--------------------------------------------------------------&P+&B\n\r");
			return;
		}
		else
			ch_printf( ch, "&P+&B--------------------------------------------------------------&P+&B\n\r");
	}
}
void disintigration ( CHAR_DATA *ch, char * name , long amount, char * reason )
{
    BOUNTY_DATA *bounty;
	DESCRIPTOR_DATA *d;
    bool found;
    char buf[MAX_STRING_LENGTH];

    found = FALSE;
    
    if ( IS_NPC(ch) || NEW_AUTH(ch) )
	{
		send_to_char("You must be authorized to post a bounty.\n\r",ch);
		return;
	}
	if ( get_age(ch) <= 10 )
	{
		send_to_char("You are not old enough to post a bounty.\n\r",ch);
		return;
	}

	for ( bounty = first_disintigration; bounty; bounty = bounty->next )
    {
    	if ( !str_cmp( bounty->target , name ))
    	{
    		found = TRUE;
    		break;
    	}
    }
    
    if (! found)                                            
    {
        CREATE( bounty, BOUNTY_DATA, 1 );
        LINK( bounty, first_disintigration, last_disintigration, next, prev );
    
        bounty->target      = STRALLOC( name );
        bounty->amount      = 0;
    }
        
    bounty->amount      = bounty->amount + amount;
    save_disintigrations();

	for( d = first_descriptor; d; d = d->next )
	{
		if (d->connected == CON_PLAYING  && !IS_IMMORTAL(d->character) )
		{
			sprintf( buf, "&R%s has added %s credits to the bounty on %s.", "Someone", num_punct(amount) , name );
	           	send_to_char( buf, d->character );
		}
		else if ( d->connected == CON_PLAYING && IS_IMMORTAL(d->character) 
			&& get_trust(d->character) >= (MAX_LEVEL - 9) )
		{
			sprintf( buf, "&R%s has added %s credits to the bounty on %s.", ch->name, num_punct(amount) , name );
	           	send_to_char( buf, d->character );
		}
	}    
	sprintf( buf, "&R%s has added %ld credits to the bounty on %s.", ch->name, amount , name );
	log_string(buf);
	{
		char    buf2[MAX_STRING_LENGTH];
		struct  tm *t = localtime(&current_time);
		set_char_color( AT_PLAIN, ch ); 
		sprintf( buf2, "(%-2.2d/%-2.2d):  %s - added %ld to %s - [%s]",
		t->tm_mon+1, t->tm_mday, ch->name, amount, name, reason );
		append_file( ch, BOUNTY_LOG_FILE, buf2 );
	}
}

void do_addbounty( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    long int amount;
    CHAR_DATA *victim;
	char name[256];
             
    if ( !argument || argument[0] == '\0' )
    {
         do_bounties( ch , argument );
         return;
    }
    
    argument = one_argument(argument, arg);
	argument = one_argument(argument, arg2);
    
    if (arg[0] == '\0' || arg2[0] == '\0' || argument[0] == '\0' )
    {
    	send_to_char( "Usage: Addbounty <target> <amount> <Reason, for imm purposes>\n\r", ch );
    	return;
    }

    /*if ( ch->pcdata && ch->pcdata->clan && !str_cmp(ch->pcdata->clan->name, "the hunters guild"))
    {
    	send_to_char( "Your job is to collect bounties not post them.", ch );
    	return;
    }*/
    
    if ( !ch->in_room || !xIS_SET(ch->in_room->room_flags, ROOM_BOUNTY ) )
    {
    	send_to_char( "You will have to go to a Bounty Office to add a new bounty.", ch );
    	return;
    }
    
    if (arg2[0] == '\0' )
        amount = 0;
    else
    	amount = atoi(arg2); 

     if ( amount < 5000 )
     {
    	send_to_char( "A bounty should be at least 5000 credits.\n\r", ch );
    	return;
    }
    if ( !(victim = get_char_world( ch, arg )) )
    {
	    char fname[1024];
		struct stat fst;

		arg[0] = UPPER(arg[0]);	
		sprintf( fname, "%s%c/%s", PLAYER_DIR, tolower(arg[0]), capitalize( arg ) );
		
		if ( stat( fname, &fst ) != -1 )
		{                              
			strcpy(name, arg);
		}
		else
		{
			send_to_char("No player exists by that name.\n\r",ch);
			return;
		}
	}
	else
	{
		strcpy(name, victim->name);
		if ( IS_NPC(victim) )
		{
			send_to_char( "You can only set bounties on other players .. not mobs!\n\r", ch );
			return;
		}
	}
                                        
    if (amount <= 0)
    {
        send_to_char( "Nice try! How about 1 or more credits instead...\n\r", ch );
        return;
    }
    
    if (ch->gold < amount)
    {
    	send_to_char( "You don't have that many credits!\n\r", ch );
    	return;
    }
    
    ch->gold = ch->gold - amount;
    
    disintigration( ch, name, amount, argument);
}

void remove_disintigration( BOUNTY_DATA *bounty )
{
	UNLINK( bounty, first_disintigration, last_disintigration, next, prev );
	STRFREE( bounty->target );
	DISPOSE( bounty );
	
	save_disintigrations();
}

void claim_disintigration( CHAR_DATA *ch , CHAR_DATA *victim )
{
	BOUNTY_DATA *bounty;
	long int     exp;
	char buf[MAX_STRING_LENGTH];

        if ( IS_NPC(victim) )
            return;

	bounty = get_disintigration( victim->name );
            
        if ( ch == victim )
        {
            if ( bounty != NULL )
               remove_disintigration(bounty);
            return;
	}
		
/*	if (bounty && 
		(!ch->pcdata || !ch->pcdata->clan || str_cmp(ch->pcdata->clan->name, "the hunters guild") ) ) 
	{
	   remove_disintigration(bounty);                	
	   bounty = NULL;
	}*/
	
	if (bounty == NULL)
	{
		sprintf( buf, "OOC - %s is Dead!", victim->name );
		echo_to_all ( AT_RED , buf, 0 );
		return; 
	}
		
	ch->gold += bounty->amount;	
	
        exp = URANGE(1, bounty->amount + xp_compute(ch, victim) , ( exp_level(ch->skill_level[HUNTING_ABILITY]+1) - exp_level(ch->skill_level[HUNTING_ABILITY]) ));	
	gain_exp( ch , exp , HUNTING_ABILITY );
        	
	set_char_color( AT_BLOOD, ch );
	ch_printf( ch, "You receive %ld experience and %s credits,\n\r from the bounty on %s\n\r", exp, num_punct(bounty->amount), bounty->target );
	
	sprintf( buf, "OOC - %s has claimed the disintigration bounty on %s!",ch->name, victim->name );
	echo_to_all ( AT_RED , buf, 0 );
	sprintf( buf, "OOC - %s is Dead!", victim->name );
	echo_to_all ( AT_RED , buf, 0 );
	
	remove_disintigration(bounty);                	
}

void do_rembounty( CHAR_DATA *ch, char *argument )
{
	BOUNTY_DATA *bounty;
             
    if ( !argument || argument[0] == '\0' )
    {
         do_bounties( ch , argument );
         return;
    }
    
    if (argument[0] == '\0' )
    {
    	send_to_char( "Usage: rembounty <target> <amount>\n\r", ch );
    	return;
    }
        
	argument[0] = UPPER(argument[0]);
	bounty = get_disintigration( argument );
	if ( bounty)
	{
		remove_disintigration(bounty);     
		ch_printf(ch, "Done\n\r");
	}
	else
	{
		ch_printf(ch, "No Such Bounty\n\r");
		return;
	}
}
