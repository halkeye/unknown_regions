/***************************************************************************
*                          UNKNOWN REGIONS Ver 1.0                         *
*--------------------------------------------------------------------------*

* Star Wars Reality Code Additions and changes from the Smaug Code         *
* copyright (c) 1997 by Sean Cooper                                        *
* -------------------------------------------------------------------------*
* Starwars and Starwars Names copyright(c) Lucasfilm Ltd.                  *
* -------------------------------------------------------------------------*
* SMAUG 1.0 (C) 1994, 1995, 1996 by Derek Snider                           *
* SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,                    *
* Scryn, Rennard, Swordbearer, Gorog, Grishnakh and Tricops                *
* ------------------------------------------------------------------------ *
* Merc 2.1 Diku Mud improvments copyright (C) 1992, 1993 by Michael        *
* Chastain, Michael Quan, and Mitchell Tse.                                *
* Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,          *
* Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.     *
* ------------------------------------------------------------------------ *
*                          UNKNOWN REGIONS ADDONS    					   *   
****************************************************************************/

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "mud.h"

#if defined(__FreeBSD__)
	#include <dirent.h>
#else
	#include <sys/dir.h>
#endif

/* comm.c */
bool     check_parse_name        args( ( char *name, bool newchar ) );

/*
 * True If mob is a follower
 */

bool is_pet(CHAR_DATA *mob)
{
	if (!mob) return FALSE;
	if (!IS_NPC(mob))
		return FALSE;
	if (mob->master != NULL && mob->master->in_room == mob->in_room);
		return TRUE;
}


/*
 * True if char can see victim.
 */
bool can_hear_chan( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if (!victim) return FALSE; 
	if ( !ch ) {
		if ( IS_SET(victim->act, PLR_WIZINVIS) ) { return FALSE; }
		else { return TRUE; } 
	}
    if ( ch == victim ) return TRUE;
    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_WIZINVIS) && get_trust( ch ) < victim->pcdata->wizinvis ) return FALSE;
    if ( IS_NPC(victim) && IS_SET(victim->act, ACT_MOBINVIS) && get_trust( ch ) < victim->mobinvis ) return FALSE;
    return TRUE;
}

/****************************
*Upgrade renameship Command	*
*Added By Gavin				*
*08/03/99					*
*****************************/

void do_renameship( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
    SHIP_DATA *ship;

	if ( IS_NPC(ch) )
		return;

    argument = one_argument( argument, arg1 );

	ship = ship_in_room( ch->in_room , arg1 );
	if ( !ship )
	{
		send_to_char("No Such Ship",ch);
		return;
	}
	
	if ( ship->class > SHIP_PLATFORM )
	{
		send_to_char("&RThis isn't a spacecraft!\n\r",ch);
		return;
	}

	if (ship->shipstate != SHIP_DOCKED)
	{
		send_to_char("&RYour Name Can Only Be Changed When Your Docked!\n\r",ch);
		return;
	}

	if ( arg1[0] == '\0' )
	{
		send_to_char("&RSyntax: renameship <newname>\n\r",ch);
		return;
	}
	argument = stripclr(argument);
	if ( !ship->owner || !ship->pilot || !ship->copilot || check_pilot(ch,ship )) 
	{
		if ( strlen(argument) <= 3)
		{
			send_to_char("That name is too short.\n\r",ch);
			return;
		}
		if (ch->pcdata->quest_curr >= 150  )
		{
			ch->pcdata->quest_curr -= 150;
			send_to_char(buf,ch);
			STRFREE( ship->name );
			ship->name = STRALLOC( argument );
			save_ship( ship );
			sprintf(buf, "Ok, %s, Your Ship's Name is Now %s.",ch->name,ship->name);
			return;
		}
		else
		{
			sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.\n\r",ch->name);
			send_to_char(buf,ch);
			sprintf(buf, "You Need 150 quest points to Rename your ship, and you only have %d.\n\r",ch->pcdata->quest_curr);
			send_to_char(buf,ch);
			return;
		}
	} 
	else
	{
		sprintf(buf, "Sorry, %s, You Don't Own That Ship.",ch->name);
		send_to_char(buf,ch);
		return;
	}
    do_setship( ch, "" );
    return;
}

/* Check the add helpfile file -- Xerves 8/24/99 */
void do_ahelp( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_PLAIN, ch );
    if ( argument[0] == '\0' )
    {
        send_to_char_color( "\n\rUsage:  'ahelp list' or 'ahelp clear now'\n\r", ch);
        return;
    }
    if ( !str_cmp( argument, "clear now" ) )
    {
        FILE *fp = fopen( HELP_FILE, "w" );
        if ( fp )
			fclose( fp );
        send_to_char_color( "Add Help file cleared.\n\r", ch);
        return;
    }
    if ( !str_cmp( argument, "list" ) )
    {
		send_to_char_color( "\n\r VNUM \n\r.......\n\r", ch );
        show_file( ch, HELP_FILE );
    }
    else
    {
        send_to_char_color( "\n\rUsage:  'ahelp list' or 'ahelp clear now'\n\r", ch);
        return;
    }
    return;
}

void do_pcrename( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char newname[MAX_STRING_LENGTH];
    char oldname[MAX_STRING_LENGTH];
    char backname[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );
    smash_tilde( arg2 );


    if ( IS_NPC(ch) )
	return;

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char("Syntax: rename <victim> <new name>\n\r", ch );
	return;
    }

    if  (!check_parse_name( arg2, 1) )
    {
	send_to_char("Illegal name.\n\r", ch );
	return;
    }
    /* Just a security precaution so you don't rename someone you don't mean 
     * too --Shaddai
     */
    if ( ( victim = get_char_room ( ch, arg1 ) ) == NULL )
    {
	send_to_char("That person is not in the room.\n\r", ch );
	return;
    }
    if ( IS_NPC(victim ) )
    {
	send_to_char("You can't rename NPC's.\n\r", ch );
	return;
    }

    if ( get_trust(ch) < get_trust(victim) )
    {
	send_to_char("I don't think they would like that!\n\r", ch );
	return;
    }
    sprintf( newname, "%s%c/%s", PLAYER_DIR, tolower(arg2[0]),
                                 capitalize( arg2 ) );
	if ( victim->pcdata->filename ) {
	sprintf( oldname, "%s%c/%s",PLAYER_DIR,tolower(victim->pcdata->filename[0]),
                                 capitalize( victim->pcdata->filename ) );
    sprintf( backname,"%s%c/%s",BACKUP_DIR,tolower(victim->pcdata->filename[0]),
                                 capitalize( victim->pcdata->filename ) );
	}
	else {	
		sprintf( oldname, "%s%c/%s",PLAYER_DIR,tolower(ch->name[0]),
				 capitalize( ch->name ) );
		sprintf( backname,"%s%c/%s",BACKUP_DIR,tolower(ch->name[0]),
			capitalize( ch->name ) );
	}
    
    if ( access( newname, F_OK ) == 0 )
    {
	send_to_char("That name already exists.\n\r", ch );
	return;
    }

    /* Have to remove the old god entry in the directories */
    if ( IS_IMMORTAL( victim ) )
    {
	char godname[MAX_STRING_LENGTH];
	sprintf(godname, "%s%s", GOD_DIR, capitalize(victim->pcdata->filename));
	remove( godname );	
    }

    /* Remember to change the names of the areas */
    if ( ch->pcdata->area )
    {
       char filename[MAX_STRING_LENGTH];
       char newfilename[MAX_STRING_LENGTH];

       sprintf( filename, "%s%s.are", BUILD_DIR, victim->name);
       sprintf( newfilename, "%s%s.are", BUILD_DIR, capitalize(arg2));
       rename(filename, newfilename);
       sprintf( filename, "%s%s.are.bak", BUILD_DIR, victim->name);
       sprintf( newfilename, "%s%s.are.bak", BUILD_DIR, capitalize(arg2));
       rename(filename, newfilename);
    }
       
    STRFREE( victim->name );
    victim->name = STRALLOC( capitalize(arg2) );    
    STRFREE( victim->full_name );
    victim->full_name = STRALLOC( capitalize(arg2) );    
    STRFREE( victim->pcdata->filename );
    victim->pcdata->filename = STRALLOC( capitalize(arg2) );
    remove( backname );
    if ( remove( oldname ) )
    {
	sprintf(buf, "Error: Couldn't delete file %s in do_rename.", oldname);
	send_to_char("Couldn't delete the old file!\n\r", ch );
	log_string( oldname );
    }	
    /* Time to save to force the affects to take place */
    save_char_obj( victim );

    /* Now lets update the wizlist */
    if ( IS_IMMORTAL( victim ) )
	make_wizlist();
    send_to_char("Character was renamed.\n\r", ch );
    return;
}

void do_sexes(CHAR_DATA *ch, char *argument) 
{
	int count= 0;
	DESCRIPTOR_DATA	*d;
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];
	for( d = first_descriptor; d; d = d->next )
	{
		if ( d->connected == CON_PLAYING 
			&& !IS_NPC(d->character) )
		{
			if (count == 0 )
				ch_printf(ch,"\n\r&B[&WName       &B] &B[&WSex        &B]&W\n\r");
			victim = d->character;
			strcpy(buf, "ERROR");
			if ( victim->sex == SEX_MALE ) { strcpy(buf, "male       " ); }
			else if ( victim->sex == SEX_FEMALE ) { strcpy(buf,"female     "); }
			else if ( victim->sex == SEX_NEUTRAL ) { strcpy(buf, "neutral    " ); }
			ch_printf(ch,"&B[&W%-11s&B] &B[&W%-15s&B]&W\n\r", victim->name, buf );
			count++;
		}
	}
	return;
}

#define ALL_SHIP -1
void do_freeships(CHAR_DATA *ch, char *argument) 
{
	SHIP_DATA *ship;
	int count = 0;
	int ship_class;

	if ( argument[0] == '\0' )
	{
		send_to_char("Syntax:\n\r  freeships [all/fighter/midsize/capital/platform]\n\r",ch);
		return;
	}
	else if ( !str_cmp(argument,"all" ) )
		ship_class = ALL_SHIP;
	else if ( !str_cmp(argument,"fighter") )
		ship_class = FIGHTER_SHIP;
	else if ( !str_cmp(argument,"midsize") )
		ship_class = MIDSIZE_SHIP;
	else if ( !str_cmp(argument,"capital") )
		ship_class = CAPITAL_SHIP;
	else if ( !str_cmp(argument,"platform") )
		ship_class = SHIP_PLATFORM;

	send_to_char( "&Y\n\rThe following ships are for sale:\n\r", ch );
	send_to_char( "&WShip                               Price                Location\n\r", ch );
	count = 0;
	for ( ship = first_ship; ship; ship = ship->next )
	{   
		if ( ship->class > SHIP_PLATFORM ) 
			continue; 
		if (IS_SET(ship->flags,SHIP_MOB))
			continue;
		if ( ship_class != ALL_SHIP && ship->class != ship_class )
			continue;
		set_char_color( AT_BLUE, ch );

		if ( !str_cmp(ship->owner, "unowned") )
		{
			if  ( ship->in_room )       
				ch_printf( ch, "%-34s %ld to buy \t%s.\n\r",ship->name, get_ship_value(ship), ship->in_room->name);
			else if (ship->starsystem && ship->starsystem->name)
				ch_printf( ch, "%-34s %ld to buy \t%s.\n\r",ship->name, get_ship_value(ship), ship->starsystem->name);
			count++;
		}
		else
			continue;
	}
	if ( !count )
		send_to_char( "There are no ships currently for sale.\n\r", ch );
	send_to_char( "\n\r", ch );
}
#undef ALL_SHIP

void do_blackjack( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument,arg);

	if ( IS_NPC(ch) )
	{
		send_to_char("Mobs can't use blackjack atm\n\r",ch);
		return;
	}

    if (arg[0] == '\0')
	{
		send_to_char("&BBlackjack &B<&Rvictim&B>&R\n\r",ch);
		return;
	}

	if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
		send_to_char("That person is not in the room.\n\r", ch );
		return;
    }

    if (IS_AFFECTED(victim,AFF_PARALYSIS))
    {
		act(AT_RED, "$E's already been blackjacked.",ch,NULL,victim,TO_CHAR);
		return;
    }

    if (victim == ch)
    {
		send_to_char("Very funny.\n\r",ch);
		return;
    }

    if (is_safe(ch,victim))
	{
		send_to_char("No can do.\n\r",ch);
		return;
	}

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
		act(AT_RED, "But $N is such a good friend!",ch,NULL,victim,TO_CHAR);
		return;
    }
	WAIT_STATE( ch, skill_table[gsn_blackjack]->beats );
	chance = IS_NPC(ch) ? ch->top_level : (int)  (ch->pcdata->learned[gsn_blackjack]) ;

	if (chance == 0)
    {
		send_to_char("You failed horrbly!.\n\r",ch);
		return;
    }

    if ( ( number_percent() + victim->top_level ) < ( chance + ch->top_level ) )
    {
		AFFECT_DATA af;
		act(AT_RED, "$n is knocked out cold!",victim,NULL,NULL,TO_ROOM);
		act(AT_RED, "$n wacks you upside the head!",ch,NULL,victim,TO_VICT);
		
		/* This damaged the victim too, but i couldnt get it the numbers right.
			damage(ch,victim,number_range(2,5),gsn_blackjack,DAM_NONE,FALSE);
		*/

		send_to_char("You are knocked out cold!\n\r",victim);
		learn_from_success( ch, gsn_blackjack);

		if ( !IS_AFFECTED( victim, AFF_PARALYSIS ) )
		{
			af.type      = gsn_stun;
			af.location  = APPLY_AC;
			af.modifier  = 20;
			af.duration  = 7;
			af.bitvector = AFF_PARALYSIS;
			affect_to_char( victim, &af );
			update_pos( victim );
			if ( IS_NPC(victim) )
			{
				start_hating( victim, ch );
				start_hunting( victim, ch );
				victim->was_stunned = 10;
			}
		}
	}
	else
	{
		act(AT_RED, "$n completely misses $N!",ch,NULL,victim,TO_ROOM);
		act(AT_RED, "$n misses you!",ch,NULL,victim,TO_VICT);
		learn_from_failure( ch, gsn_blackjack );
		if ( IS_NPC(victim) )
		{
			start_hating( victim, ch );
			start_hunting( victim, ch );
		}
    }
}

void do_listships(CHAR_DATA *ch, char *argument) 
{
	SHIP_DATA *ship;
	int count = 0;
	CHAR_DATA * victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax: listship <player>\n\r", ch );
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

	count = 0;
	ch_printf( ch, "The following ships are owned by %s or his organization:\n\r", victim->name );
	send_to_char( "\n\r&WShip                               Location\n\r", ch );

      for ( ship = first_ship; ship; ship = ship->next )
      {   
        if ( ship->class > SHIP_PLATFORM ) 
           continue; 
        if (IS_SET(ship->flags,SHIP_MOB))
           continue;
		set_char_color( AT_BLUE, ch );

		if ( !str_cmp(ship->owner, victim->name) )
		{
			if  ( ship->in_room )       
				ch_printf( ch, "&R[&WOwner&R]&B%-35s     %s [%d].\n\r",ship->name, ship->in_room->name, ship->location);
			else if (ship->starsystem && ship->starsystem->name)
				ch_printf( ch, "&R[&WOwner&R]&B%-35s     %s.\n\r",ship->name, ship->starsystem->name);
		}
		else if ( !str_cmp(ship->pilot, victim->name) )
		{
			if  ( ship->in_room )       
				ch_printf( ch, "&R[&WPilot&R]&B%-35s     %s [%d].\n\r",ship->name, ship->in_room->name, ship->location);
			else if (ship->starsystem && ship->starsystem->name)
				ch_printf( ch, "&R[&WPilot&R]&B%-35s     %s.\n\r",ship->name, ship->starsystem->name);
		}
		else if ( !str_cmp(ship->copilot, victim->name) )
		{
			if  ( ship->in_room )       
				ch_printf( ch, "&R[&WCo-Pilot&R]&B%-35s     %s [%d].\n\r",ship->name, ship->in_room->name, ship->location);
			else if (ship->starsystem && ship->starsystem->name)
				ch_printf( ch, "&R[&WCo-Pilot&R]&B%-35s     %s.\n\r",ship->name, ship->starsystem->name);
		}
		else if ( victim->pcdata->clan && !str_cmp(ship->owner, victim->pcdata->clan->name) )
		{
			if  ( ship->in_room )       
				ch_printf( ch, "&R[&WClan&R]&B%-35s     %s [%d].\n\r",ship->name, ship->in_room->name, ship->location);
			else if (ship->starsystem && ship->starsystem->name)
				ch_printf( ch, "&R[&WClan&R]&B%-35s     %s.\n\r",ship->name, ship->starsystem->name);
		}
		else
			continue;
        
        count++;
      }
    
      if ( !count )
      {
        send_to_char( "They don't own any ships.\n\r", ch );
		return;
      }
}

/** Function: do_petname
  * Descr   : renames pets, also allows changing of short/long and 
  *           descritpion variables.
  * Returns : (void)
  * Syntax  : petname ([name|short|long|sex|desc]) (argument)
  * Written : v1.1 1/98
  * Author  : Gary McNickle <gary@dharvest.com>
  * Updated : Gavin Mogan <ur_gavin@hotmail.com>
  */

void do_petname(CHAR_DATA *ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char command[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	CHAR_DATA *		pet;

	/* smash all tilde's right away. */
	smash_tilde(argument);

	if (IS_NPC(ch))
		return;

	if ( ch->pcdata->pet == NULL )
	{
		send_to_char("You dont have a pet!\n\r", ch);
		return;
	}

    if ( !ch->desc )
	{
		send_to_char( "You have no descriptor.\n\r", ch );
		return;
	}

	switch( ch->substate )
	{
	default:
		break;
	case SUB_MOB_DESC:
		pet = ch->dest_buf;
		if ( !pet )
		{
			bug( "redit: sub_mob_desc: NULL ch->dest_buf", 0 );
			pet = ch->pcdata->pet;
		}
		STRFREE( pet->description );
		pet->description = copy_buffer( ch );
		stop_editing( ch );
		ch->substate = ch->tempnum;
		return;
	}

	if ( ch->in_room != ch->pcdata->pet->in_room )
	{
		send_to_char("Kinda hard for your pet to learn his new name\n\r",ch);
		send_to_char("if he's not even with you! *boggle*\n\r", ch);
		return;
	}

	argument = one_argument(argument, command);

	if ( command[0] == '\0' ||  argument[0] == '\0' )
	{
		send_to_char( "&Bsyntax: &Wpetname [name|short|long|sex|desc] <argument>\n\r",ch);
		send_to_char( "\t&Bexample:\t\"&Wpetname name fido\n\r", ch);
		send_to_char( "\t\t\t\"&Wpetname short is hungry!\"\n\r",ch);
		send_to_char("&BTry using color in the descriptions!\n\r", ch);
		send_to_char("&BSee &W\"help petname\" &Band &W\"help color\" &Bfor more information.\n\r",ch);
		return;
	} /* syntax */

	if ( !str_prefix(command, "name") )
	{
		if ( argument[0] == ANSI_KEY ) /* ack! */
		{
			argument++; argument++;
		}
		argument = one_argument(argument, arg2);
		arg2[0] = UPPER(arg2[0]);
		STRFREE(ch->pcdata->pet->name);
		ch->pcdata->pet->name = STRALLOC(arg2);
		sprintf(buf, "Your pet has been renamed to: %s\n\r", ch->pcdata->pet->name);
		send_to_char(buf, ch);
	} /* name */

	else if ( !str_prefix(command, "short") )
	{
		if ( argument[0] == '\0' ) return;
		STRFREE(ch->pcdata->pet->short_descr);
		/* short description should not have an "\n\r" appended to it. */
		ch->pcdata->pet->short_descr = STRALLOC(argument);

		sprintf(buf, "%s's short description set to: \n\r%s\n\r",
			ch->pcdata->pet->name, ch->pcdata->pet->short_descr);
		send_to_char(buf, ch);
	} /* short */
	
	else if ( !str_prefix(command, "sex") )
	{
		sh_int value;
		if ( argument[0] == '\0' ) 
			return;
		value = is_number( argument ) ? atoi( argument ) : -1;
		if ( atoi(argument) < -1 && value == -1 )
			value = atoi(argument);
		if ( value < 0 || value > 2 )
		{
			send_to_char("Valid choices are 0 (Neutral), 1 (Male) or 2 (Female).\n\r",ch);
			return;
		}
		ch->pcdata->pet->sex = value;
		sprintf(buf, "%s's sex is now %s\n\r",
			ch->pcdata->pet->name, 
			( ch->pcdata->pet->sex == SEX_MALE ? "A Male" : 
			ch->pcdata->pet->sex == SEX_FEMALE ? "A Female" : "Neutral" )	);
		send_to_char(buf, ch);
	} /* sex */

	else if ( !str_prefix(command, "long") )
	{
		if ( argument[0] == '\0' ) 
			return;
		STRFREE(ch->pcdata->pet->long_descr);

		/* long descr needs "\n\r" appended to it. */
		sprintf(buf, "%s\n\r", argument);
		ch->pcdata->pet->long_descr = STRALLOC(buf);

		sprintf(buf, "%s's long description set to: \n\r%s\n\r",
			ch->pcdata->pet->name, ch->pcdata->pet->long_descr);
		send_to_char(buf, ch);
	} /* long */
	else if ( !str_prefix(command, "description") )
	{
		if ( argument[0] )
		{
			STRFREE( ch->pcdata->pet->description );
			ch->pcdata->pet->description = STRALLOC( argument );
			return;
		}
		CHECK_SUBRESTRICTED( ch );
		if ( ch->substate == SUB_REPEATCMD )
			ch->tempnum = SUB_REPEATCMD;
		else
			ch->tempnum = SUB_NONE;
		ch->substate = SUB_MOB_DESC;
		ch->dest_buf = ch->pcdata->pet;
		start_editing( ch, ch->pcdata->pet->description );
		return;
	} /* desc */
	else
		do_help(ch, "petname");
	return;
} /* end of do_petname */

char *stripclr( char *text )
{
	int i = 0, j = 0;

	if (!text || text[0] == '\0')
	{
		return NULL;
	}
	else
	{
		char *buf;
		static char done[MAX_INPUT_LENGTH*2];

		done[0] = '\0';

		if ( (buf = (char *)malloc( strlen(text) * sizeof(text) )) == NULL)
			return text;      

		/* Loop through until you've hit your terminating 0 */
		while (text[i] != '\0')
		{
			while (text[i] == '&')
			{
				i += 2;
			}
			if ( text[i] != '\0' )
			{
				if ( isspace(text[i]) )
				{
					buf[j] = ' ';
					i++;
					j++;
				}
				else
				{
					buf[j] = text[i];
					i++;
					j++;
				}
			}
			else
				buf[j] = '\0';
		}

		buf[j] = '\0';

		sprintf(done, "%s", buf);
		buf = realloc(buf, j*sizeof(char));
		free( buf);

		return done;
	}
}

void do_mxp( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
		send_to_char( "MXP ON or OFF?\n\r", ch );
		return;
    }
    if ( !str_cmp(arg,"on") || !str_cmp(arg,"ON") )
	{
		SET_BIT(ch->act2,PCFLAG_MXP);
		set_char_color( AT_WHITE + AT_BLINK, ch);
		send_to_char( "MXP ON!!!\n\r", ch);
		return;
    }

    if ( !str_cmp(arg,"off") ) 
	{
		REMOVE_BIT(ch->act2,PCFLAG_MXP);
		send_to_char( "Okay... MXP support is now off\n\r", ch );
		return;
    }
	if ( !str_cmp(arg, "bday") )
	{
		DESCRIPTOR_DATA *d;

		ch_printf(ch, "&P+&B--------------------------------------------------------------&P+\n\r");
		
		for( d = first_descriptor; d; d = d->next )
		{
			if (d->connected == CON_PLAYING && !IS_NPC(d->character) &&
				d->character->pcdata->birthday.day && d->character->pcdata->birthday.month )
			{
				ch_printf(ch, "&P| &G&WName: %-20.20s Age: %-3d  Birthday %-3d %-3d %-4d   &B|\n\r",
					d->character->name,
					( !IS_SET(d->character->act2, EXTRA_BORN) ? (  time_info.year - d->character->pcdata->birthday.year ) + d->character->pcdata->age : (  time_info.year - d->character->pcdata->birthday.year )  ),
					d->character->pcdata->birthday.day,	d->character->pcdata->birthday.month, d->character->pcdata->birthday.year ); 
			}
		}
		ch_printf(ch, "&P+&B--------------------------------------------------------------&P+\n\r");
	}
	if ( !str_cmp(arg,"title") )
	{
		if ( argument[0] == '\0' )
		{
			send_to_char("title what?\n\r",ch);
			return;
		}
		ch_printf(ch,"&G&WBefore: %s\n\r",argument);
		ch_printf(ch,"&G&WAfter: %s\n\r",stripclr( argument ) );
		return;
	}

	if ( !str_cmp(arg,"test") )
	{
		ch_printf(ch,"\e[1z<!-- Elements to support the zMUD Automapper -->\n");
		ch_printf(ch,"\e[1z<!ELEMENT RName \'<FONT COLOR=Red><B>\' FLAG=\"RoomName\">\n");
		ch_printf(ch,"\e[1z<!ELEMENT RDesc FLAG=\'RoomDesc\'>\n");
		ch_printf(ch,"\e[1z<!ELEMENT RExits \'<FONT COLOR=Blue>\' FLAG=\'RoomExit\'>\n");
		ch_printf(ch,"\e[1z<!-- The next element is used to define a room exit link that sends the exit direction to the MUD if the user clicks on it -->\n");
		ch_printf(ch,"\e[1z<!ELEMENT Ex \'<SEND>\'>");
		ch_printf(ch,"\e[1z<!ELEMENT Chat \'<FONT COLOR=Gray>\' OPEN>\n");
		ch_printf(ch,"\e[1z<!ELEMENT Gossip \'<FONT COLOR=Cyan>\' OPEN>\n");
		ch_printf(ch,"\e[1z<!-- in addition to standard HTML Color specifications, you can use zMUD color attribute names such as blink -->\n");
		ch_printf(ch,"\e[1z<!ELEMENT ImmChan \'<FONT COLOR=Red,Blink>\'>\n");
		ch_printf(ch,"\e[1z<!ELEMENT Auction \'<FONT COLOR=Purple>\' OPEN>\n");
		ch_printf(ch,"\e[1z<!ELEMENT Group \'<FONT COLOR=Blue>\' OPEN>\n");
		ch_printf(ch,"\e[1z<!-- the next elements deal with the MUD prompt -->\n");
		ch_printf(ch,"\e[1z<!ELEMENT Prompt FLAG=\"Prompt\">\n");
		ch_printf(ch,"\e[1z<!ELEMENT Hp FLAG=\"Set hp\">\n");
		ch_printf(ch,"\e[1z<!ELEMENT MaxHp FLAG=\"Set maxhp\">\n");
		ch_printf(ch,"\e[1z<!ELEMENT Mana FLAG=\"Set mana\">\n");
		ch_printf(ch,"\e[1z<!ELEMENT MaxMana FLAG=\"Set maxmana\">\n");
		ch_printf(ch,"\e[1z<!-- now the MUD text -->\n");
		ch_printf(ch,"\e[1z<RName>The Main Temple</RName>\n");
		ch_printf(ch,"\e[1z<RDesc>This is the main hall of the MUD where everyone starts.\n");
		ch_printf(ch,"\e[1zMarble arches lead south into the town, and there is a <i>lovely</i>\n");
		ch_printf(ch,"\e[1z<send \"drink &&text;\">fountain</send> in the center of the temple,</RDesc>\n");
		ch_printf(ch,"\e[1z<RExits>Exits: <Ex>N</Ex>, <Ex>S</Ex>, <Ex>E</Ex>, <Ex>W</Ex></RExits>\n");
	}
}


void do_call ( CHAR_DATA * ch , char * argument )
{
	CHAR_DATA *		pet;
	if ( IS_NPC(ch) )
		return;
	if ( !ch->pcdata || !ch->pcdata->pet )
	{
		send_to_char("You don't have a pet to call.\n\r",ch);
		return;
	}
	pet = ch->pcdata->pet;
	if ( !IS_NPC(pet) )
	{
		send_to_char("Error. Why do you have a non-mob pet?\n\r",ch);
		return;
	}
	if ( !pet->in_room || !pet->in_room->area /*|| !pet->in_room->area->planet*/ )
	{
		send_to_char("Sorry, your pet is too far away to call.\n\r",ch);
		return;
	}
	if ( pet->in_room->area->planet && ch->in_room->area->planet)
	{
		if ( pet->in_room->area->planet == ch->in_room->area->planet )
		{
			/* bring pet to char... use some message */
			act(AT_ACTION, "You call for $N.", ch, NULL, pet, TO_CHAR );
			act(AT_ACTION, "$n calls out for $N.", ch, NULL, pet, TO_ROOM );
			char_from_room( pet );
			char_to_room( pet, ch->in_room );
			act(AT_ACTION, "$n appears obeying your call.", pet, NULL, ch, TO_VICT );
			act(AT_ACTION, "$n appears obeying its master's call.", pet, NULL, NULL, TO_ROOM );
			return;
		}
		else
		{
			act(AT_ACTION, "You call but $N does not come.", ch, NULL, pet, TO_CHAR );
			act(AT_ACTION, "$n calls out for $N.", ch, NULL, pet, TO_ROOM );
			return;
		}
	}
	else if ( pet->in_room->area == ch->in_room->area )
	{
		act(AT_ACTION, "You call for $N.", ch, NULL, pet, TO_CHAR );
		act(AT_ACTION, "$n calls out for $N.", ch, NULL, pet, TO_ROOM );
	    char_from_room( pet );
		char_to_room( pet, ch->in_room );
		act(AT_ACTION, "$n appears obeying your call.", pet, NULL, ch, TO_VICT );
		act(AT_ACTION, "$n appears obeying its master's call.", pet, NULL, NULL, TO_ROOM );
		return;
	}
	else
	{
		act(AT_ACTION, "You call but $N does not come.", ch, NULL, pet, TO_CHAR );
		act(AT_ACTION, "$n calls out for $N.", ch, NULL, pet, TO_ROOM );
		return;
	}
}
