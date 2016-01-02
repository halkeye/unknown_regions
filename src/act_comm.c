/***************************************************************************
*                           Unknown Regions                                *
*--------------------------------------------------------------------------*
* Unknown Regions Code Additions and changes from the Star Wars Reality    *
* Code done by Gavin Mogan, with help from, Gendi, Blared, And Jarrel      *
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
*                      Player communication module                         *
****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef FREEBSD
	#include <unistd.h>
	#include <regex.h>
#endif
#include "mud.h"


/*
 *  Externals
 */
void send_obj_page_to_char(CHAR_DATA * ch, OBJ_INDEX_DATA * idx, char page);
void send_room_page_to_char(CHAR_DATA * ch, ROOM_INDEX_DATA * idx, char page);
void send_page_to_char(CHAR_DATA * ch, MOB_INDEX_DATA * idx, char page);
void send_control_page_to_char(CHAR_DATA * ch, char page);

/*
 * Local functions.
 */
void	talk_channel	args( ( CHAR_DATA *ch, char *argument,
			    int channel, const char *verb ) );
char *  scramble        args( ( const char *argument, int modifier ) );			    
char *  drunk_speech    args( ( const char *argument, CHAR_DATA *ch ) ); 
char * preg;

bool	has_comlink		args( ( CHAR_DATA *ch ) );

void sound_to_room( ROOM_INDEX_DATA *room , char *argument )
{
	CHAR_DATA *vic;

	if ( room == NULL ) return;        
	for ( vic = room->first_person; vic; vic = vic->next_in_room )
		if ( !IS_NPC(vic) && IS_SET( vic->act, PLR_SOUND ) )
			send_to_char( argument, vic );
}


void do_beep( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_STRING_LENGTH];
    
    argument = one_argument( argument, arg );
    
    REMOVE_BIT( ch->deaf, CHANNEL_TELLS );
    if ( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
        send_to_char( "You can't do that here.\n\r", ch );
        return;
    }
                                
    if (!IS_NPC(ch)
        && ( IS_SET(ch->act, PLR_SILENCE)
        ||   IS_SET(ch->act, PLR_NO_TELL) ) )
    {
         send_to_char( "You can't do that.\n\r", ch );
         return;
    }
                                    
    if ( arg[0] == '\0' )
    {
         send_to_char( "Beep who?\n\r", ch );
         return;
    }
                            
    if ( ( victim = get_char_world( ch, arg ) ) == NULL 
       || ( IS_NPC(victim) && victim->in_room != ch->in_room ) 
       || (!NEW_AUTH(ch) && NEW_AUTH(victim) && !IS_IMMORTAL(ch) ) )
    {
		send_to_char( "They aren't here.\n\r", ch );
		return;
    }
    
	if ( !has_comlink(ch) )
	{
		send_to_char( "You need a comlink to do that!\n\r", ch);
		return;
	}

	if ( !has_comlink(victim) )
	{
		send_to_char( "They don't seem to have a comlink!\n\r", ch);
		return;
	}

    
    if (NEW_AUTH(ch) && !NEW_AUTH(victim) && !IS_IMMORTAL(victim) )
    {
	send_to_char( "They can't hear you because you are not authorized.\n\r", ch);
	return;
    }

    if ( !IS_NPC( victim ) && ( victim->switched ) 
	&& ( get_trust( ch ) > LEVEL_AVATAR ) )
    {
      send_to_char( "That player is switched.\n\r", ch );
      return;
    }

   else if ( !IS_NPC( victim ) && ( !victim->desc ) )
    {
      send_to_char( "That player is link-dead.\n\r", ch );
      return;
    }

    if ( IS_SET( victim->deaf, CHANNEL_TELLS ) 
    && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
    {
      act( AT_PLAIN, "$E has $S tells turned off.", ch, NULL, victim,
		TO_CHAR );
      return;
    }

    if ( !IS_NPC (victim) && ( IS_SET (victim->act, PLR_SILENCE ) ) )
      {
      send_to_char( "That player is silenced.  They will receive your message but can not respond.\n\r", ch );
      }   

    if ( (!IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    || (!IS_NPC(victim)&& xIS_SET(victim->in_room->room_flags, ROOM_SILENCE ) ) )
    {
    act( AT_PLAIN, "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( victim->desc		/* make sure desc exists first  -Thoric */
    &&   victim->desc->connected == CON_EDITING 
    &&   get_trust(ch) < LEVEL_GOD )
    {
	act( AT_PLAIN, "$E is currently in a writing buffer.  Please try again in a few minutes.", ch, 0, victim, TO_CHAR );
        return;
    }

    ch_printf(ch , "&G&WYou beep %s: %s\n\r\a" , victim->name, argument );
    send_to_char("\a",victim);    

    if ( knows_language( victim, ch->speaking, ch ) || (IS_NPC(ch) && !ch->speaking) )
		act( AT_WHITE, "$G beeps: '$t'", ch, argument, victim, TO_VICT );
    else
		act( AT_WHITE, "$G beeps: '$t'", ch, scramble(argument, ch->speaking), victim, TO_VICT );
}

/* Text scrambler -- Altrag */
char *scramble( const char *argument, int modifier )
{
    static char arg[MAX_INPUT_LENGTH];
    sh_int position;
    sh_int conversion = 0;
    
	modifier %= number_range( 80, 300 ); /* Bitvectors get way too large #s */
    for ( position = 0; position < MAX_INPUT_LENGTH; position++ )
    {
    	if ( argument[position] == '\0' )
    	{
    		arg[position] = '\0';
    		return arg;
    	}
    	else if ( argument[position] >= 'A' && argument[position] <= 'Z' )
	    {
	    	conversion = -conversion + position - modifier + argument[position] - 'A';
	    	conversion = number_range( conversion - 5, conversion + 5 );
	    	while ( conversion > 25 )
	    		conversion -= 26;
	    	while ( conversion < 0 )
	    		conversion += 26;
	    	arg[position] = conversion + 'A';
	    }
	    else if ( argument[position] >= 'a' && argument[position] <= 'z' )
	    {
	    	conversion = -conversion + position - modifier + argument[position] - 'a';
	    	conversion = number_range( conversion - 5, conversion + 5 );
	    	while ( conversion > 25 )
	    		conversion -= 26;
	    	while ( conversion < 0 )
	    		conversion += 26;
	    	arg[position] = conversion + 'a';
	    }
	    else if ( argument[position] >= '0' && argument[position] <= '9' )
	    {
	    	conversion = -conversion + position - modifier + argument[position] - '0';
	    	conversion = number_range( conversion - 2, conversion + 2 );
	    	while ( conversion > 9 )
	    		conversion -= 10;
	    	while ( conversion < 0 )
	    		conversion += 10;
	    	arg[position] = conversion + '0';
	    }
	    else
	    	arg[position] = argument[position];
	}
	arg[position] = '\0';
	return arg;	     
}

char *drunk_speech( const char *argument, CHAR_DATA *ch )
{
  const char *arg = argument;
  static char buf[MAX_INPUT_LENGTH*2];
  char buf1[MAX_INPUT_LENGTH*2];
  sh_int drunk;
  char *txt;
  char *txt1;  

  if ( IS_NPC( ch ) || !ch->pcdata ) return (char *) argument;

  drunk = ch->pcdata->condition[COND_DRUNK];

  if ( drunk <= 0 )
    return (char *) argument;

  buf[0] = '\0';
  buf1[0] = '\0';

  if ( !argument )
  {
     bug( "Drunk_speech: NULL argument", 0 );
     return "";
  }

  /*
  if ( *arg == '\0' )
    return (char *) argument;
  */

  txt = buf;
  txt1 = buf1;

  while ( *arg != '\0' )
  {
    if ( toupper(*arg) == 'S' )
    {
	if ( number_percent() < ( drunk * 2 ) )		/* add 'h' after an 's' */
	{
	   *txt++ = *arg;
	   *txt++ = 'h';
	}
       else
	*txt++ = *arg;
    }
   else if ( toupper(*arg) == 'X' )
    {
	if ( number_percent() < ( drunk * 2 / 2 ) )
	{
	  *txt++ = 'c', *txt++ = 's', *txt++ = 'h';
	}
       else
	*txt++ = *arg;
    }
   else if ( number_percent() < ( drunk * 2 / 5 ) )  /* slurred letters */
    {
      sh_int slurn = number_range( 1, 2 );
      sh_int currslur = 0;	

      while ( currslur < slurn )
	*txt++ = *arg, currslur++;
    }
   else
    *txt++ = *arg;

    arg++;
  };

  *txt = '\0';

  txt = buf;

  while ( *txt != '\0' )   /* Let's mess with the string's caps */
  {
    if ( number_percent() < ( 2 * drunk / 2.5 ) )
    {
      if ( isupper(*txt) )
        *txt1 = tolower( *txt );
      else
      if ( islower(*txt) )
        *txt1 = toupper( *txt );
      else
        *txt1 = *txt;
    }
    else
      *txt1 = *txt;

    txt1++, txt++;
  };

  *txt1 = '\0';
  txt1 = buf1;
  txt = buf;

  while ( *txt1 != '\0' )   /* Let's make them stutter */
  {
    if ( *txt1 == ' ' )  /* If there's a space, then there's gotta be a */
    {			 /* along there somewhere soon */

      while ( *txt1 == ' ' )  /* Don't stutter on spaces */
        *txt++ = *txt1++;

      if ( ( number_percent() < ( 2 * drunk / 4 ) ) && *txt1 != '\0' )
      {
	sh_int offset = number_range( 0, 2 );
	sh_int pos = 0;

	while ( *txt1 != '\0' && pos < offset )
	  *txt++ = *txt1++, pos++;

	if ( *txt1 == ' ' )  /* Make sure not to stutter a space after */
	{		     /* the initial offset into the word */
	  *txt++ = *txt1++;
	  continue;
	}

	pos = 0;
	offset = number_range( 2, 4 );	
	while (	*txt1 != '\0' && pos < offset )
	{
	  *txt++ = *txt1;
	  pos++;
	  if ( *txt1 == ' ' || pos == offset )  /* Make sure we don't stick */ 
	  {		               /* A hyphen right before a space	*/
	    txt1--;
	    break;
	  }
	  *txt++ = '-';
	}
	if ( *txt1 != '\0' )
	  txt1++;
      }     
    }
   else
    *txt++ = *txt1++;
  }

  *txt = '\0';

  return buf;
}

/*
 * Generic channel function.
 */
bool has_comlink( CHAR_DATA *ch)
{
	OBJ_DATA *obj;
      
    if ( IS_IMMORTAL( ch ) || IS_NPC( ch ) )
        return TRUE;
    if ( ch->pcdata->cyber & CYBER_COMM )
		return TRUE;

    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
        if (obj->pIndexData->item_type == ITEM_COMLINK)
        return TRUE;
    }
	return FALSE;
}

void talk_channel( CHAR_DATA *ch, char *argument, int channel, const char *verb )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];

    DESCRIPTOR_DATA *d;
    int position,position2;
    CLAN_DATA *clan = NULL;
	PLANET_DATA * planet;        
        
    
    if ( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL && channel != CHANNEL_IMMTALK && channel != CHANNEL_OOC 
         && channel != CHANNEL_QUEST && channel != CHANNEL_NEWBIE && channel != CHANNEL_AVTALK && channel != CHANNEL_MUSIC
         && channel != CHANNEL_SHIP && channel != CHANNEL_SYSTEM && channel != CHANNEL_SPACE )
	{
		if ( !has_comlink(ch) )
		{
			send_to_char( "You need a comlink to do that!\n\r", ch);
			return;
		}
	}/* Channels Needing Comlink */

    if ( IS_NPC( ch ) && channel == CHANNEL_CLAN )
    {
		send_to_char( "Mobs can't be in clans.\n\r", ch );
		return;
    } /* Mob & Clan */
	
	if ( ( channel == CHANNEL_SHOUT || channel == CHANNEL_YELL ) &&
		( !ch->in_room->area || (planet = ch->in_room->area->planet) == NULL ) )
	{
		send_to_char( "Planet Net only works on planets...\n\r", ch );
		return;
	} /* Planet Only */

	if ( channel == CHANNEL_CLAN )
        clan = ch->pcdata->clan;

    if ( IS_NPC( ch ) && channel == CHANNEL_ORDER )
    {
		send_to_char( "Mobs can't be in orders.\n\r", ch );
		return;
    } /* Order && Mob */

    if ( IS_NPC( ch ) && channel == CHANNEL_GUILD )
    {
		send_to_char( "Mobs can't be in guilds.\n\r", ch );
		return;
    } /* MOB & Guild */
                                              
    if ( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
		send_to_char( "You can't do that here.\n\r", ch );
		return;
    } /* Silence */
    
    if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
		if ( ch->master )
			send_to_char( "I don't think so...\n\r", ch->master );
		return;
    } /* Charm */

	if ( argument[0] == '\0' )
	{
		sprintf( buf, "%s what?\n\r", verb );
		buf[0] = UPPER(buf[0]);
		send_to_char( buf, ch );	/* where'd this line go? */
		return;
    } /* No Arg */

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
    {
		ch_printf( ch, "You can't %s.\n\r", verb );
		return;
    } /* Player Silenced */

    REMOVE_BIT(ch->deaf, channel);

	switch ( channel )
	{
	default:/* IC CHANNEL */
		set_char_color( AT_GOSSIP, ch );
		ch_printf( ch, "You %s over the public network, '%s'\n\r", verb, argument );
		sprintf( buf, "$g %ss over the public network, '$t'",     verb );
		break;
	case CHANNEL_MUSIC:
		set_char_color( AT_OOC, ch );
        ch_printf( ch, "&R(Music Channel)&B:&G %s &Ysings '%s' ",ch->name,argument );
		sprintf( buf, "&R(Music Channel)&B:&G $G &Ysings '$t' " );
		break;
	case CHANNEL_CHAT:/* IC CHANNEL */
		set_char_color( AT_GOSSIP,ch);
		ch_printf(ch, "&Y[&RG&Calatic &RN&Cetwork&Y]&C %s: '%s'",ch->name,argument );
		sprintf( buf, "&Y[&RG&Calatic &RN&Cetwork&Y]&C $n: '$t'" );
		break;
	case CHANNEL_ANNOUNCE:
		set_char_color( AT_WHITE, ch );
		ch_printf( ch, "&G&W(&R^zAnnouncement&G^x&G&W)&G&W %s\n\r", argument );
		sprintf( buf, "&G&W(&R^zAnnouncement&G^x&G&W)&G&W $t" );
		break;
    case CHANNEL_CLANTALK:/* IC CHANNEL */
		set_char_color( AT_CLAN, ch );
		ch_printf( ch, "[Clan Network], %s: '%s'\n\r", ch->name, argument );
		sprintf( buf, "[Clan Network], $V: '$t'" );
		break;
    case CHANNEL_SHIP:
        set_char_color( AT_SHIP, ch );
		ch_printf( ch, "[&RS&Bhip &RT&Balk&P] '%s'\n\r", argument );
		sprintf( buf, "[&RS&Bhip &RT&Balk&P] &Y($n) &P'$t'"  );
		break;
    case CHANNEL_SYSTEM:
		set_char_color( AT_GOSSIP, ch );
		ch_printf( ch, "[&RS&Gystem &RT&Galk&C] '%s'\n\r", argument );
		sprintf( buf, "[&RS&Gystem &RT&Galk&C] &Y($n) &C'$t'"  );
		break;
    case CHANNEL_SPACE:
		set_char_color( AT_GOSSIP, ch );
		ch_printf( ch, "[&RS&Gpace &RT&Galk&C] '%s'\n\r", argument );
		sprintf( buf, "[&RS&Gpace &RT&Galk&C] &Y($n) &C'$t'"  );
		break;
    case CHANNEL_YELL:/* IC CHANNEL */
		set_char_color( AT_GOSSIP, ch );
		ch_printf( ch, "[&RP&Clanetary &RN&Cetwork&C] &Y'%s'\n\r", argument );
		sprintf( buf, "[&RP&Clanetary &RN&Cetwork&C] &Y$n, '$t'");
		break;
    case CHANNEL_SHOUT:/* IC CHANNEL */
		set_char_color( AT_GOSSIP, ch );
		ch_printf( ch, "[&RN&Geighborhood &RN&G&Wetwork&C] &Y'%s'\n\r", argument );
		sprintf( buf, "[&RN&Geighborhood &RN&G&Wetwork&C] &Y$n, '$t'");
		break;
    case CHANNEL_NEWBIE:
        set_char_color( AT_OOC, ch );
        sprintf( buf, "&R(&ON&YE&GW&BB&PI&CE&G&W) &R$G: &Y$t" );
		ch_printf( ch, "&R(&ON&YE&GW&BB&PI&CE&G&W) &R%s: &Y%s"  , ch->name,argument );
		break;
    case CHANNEL_OOC:
        sprintf( buf, "&R(&OO&YO&GC&B) &P$G: &C$t" );
		ch_printf( ch, "&R(&OO&YO&GC&B) &P%s: &C%s" , ch->name,argument );
		break;
	case CHANNEL_EMOTE:
		sprintf( buf, "&B[&WHolographic Comlink Picture&B]&W &R*&W $n $t");
		ch_printf( ch, "&B[&WHolographic Comlink Picture&B]&W &R*&W %s %s",ch->name,argument);
		break;
	case CHANNEL_QUEST:
		set_char_color( AT_GREEN, ch );
		ch_printf( ch, "&G(&RQ&Yuest &RT&Yalk&G) %s\n\r", argument );
		sprintf( buf, "&G(&RQ&Yuest &RT&Yalk&G) [$G] $t" );
		break;
    case CHANNEL_WARTALK:/* IC CHANNEL */
        set_char_color( AT_WARTALK, ch );
        ch_printf( ch, "You %s '%s'\n\r", verb, argument );
        sprintf( buf, "$n %ss '$t'", verb );
        break;
    case CHANNEL_AVTALK:
		sprintf( buf, "&Y(&RA&Yvatar &RT&Yalk&Y) &G&W$G: '$t'" );
		ch_printf( ch, "&Y(&RA&Yvatar &RT&Yalk&Y) &G&W%s: '%s'",ch->name,argument );
		break;
    case CHANNEL_IMMTALK:
		position	= ch->position;
		ch->position	= POS_STANDING;
		sprintf( buf, "&Y(&RI&G&WMM &RT&G&Walk&Y) &G&W$G: '$t'" );
		ch_printf( ch, "&Y(&RI&G&WMM &RT&G&Walk&Y) &G&W%s: '%s'",ch->name,argument );
		ch->position	= position;
		break;
	} /* Channel Format */

    if ( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
		sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
		append_to_file( LOG_FILE, buf2 );
    } /* LOGSPEECH */

    for ( d = first_descriptor; d; d = d->next )
    {
		CHAR_DATA *och;
		CHAR_DATA *vch;

		och = d->original ? d->original : d->character;
		vch = d->character;

		if ( d->connected == CON_PLAYING && vch != ch && !IS_SET(och->deaf, channel) )
		{
			char *sbuf = argument;
    		
    		position		= vch->position;
			if ( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL && channel != CHANNEL_IMMTALK 
				&& channel != CHANNEL_OOC && channel != CHANNEL_NEWBIE && channel != CHANNEL_AVTALK && channel != CHANNEL_MUSIC
				&& channel != CHANNEL_SHIP && channel != CHANNEL_SYSTEM && channel != CHANNEL_ANNOUNCE
				&& channel != CHANNEL_SPACE )
			{ if ( !has_comlink(och) ) continue; }
		
			if ( channel == CHANNEL_IMMTALK && !IS_IMMORTAL(och) )
				continue;
			if ( channel == CHANNEL_IMMTALK && !IS_IMMORTAL(vch) )
				continue;
			if ( channel == CHANNEL_WARTALK && NEW_AUTH( och ) ) /* new auth */
				continue;
			if ( channel == CHANNEL_AVTALK && !IS_HERO(och) )
				continue;
			if ( xIS_SET( vch->in_room->room_flags, ROOM_SILENCE ) )
				continue;
			if ( channel == CHANNEL_YELL )
			{
				if ( !vch->in_room || !vch->in_room->area || !vch->in_room->area->planet || ( vch->in_room->area->planet != planet ) )
					continue;
			}
			if ( channel == CHANNEL_SHOUT && vch->in_room->area != ch->in_room->area )
				continue;
			if ( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL )
			{
				vch->position	= POS_STANDING;
				vch->position	= position;
			}

			if ( channel == CHANNEL_CLAN || channel == CHANNEL_ORDER ||   channel == CHANNEL_GUILD )
			{
				if ( IS_NPC( vch ) )
					continue;
		
				if ( !vch->pcdata->clan )
					continue;
		
				if ( vch->pcdata->clan != clan )
					continue;
			}/* CLan Channels */

			if ( channel == CHANNEL_SHIP || channel == CHANNEL_SPACE || channel == CHANNEL_SYSTEM )
			{
				SHIP_DATA *ship = ship_from_cockpit( ch->in_room->vnum );
				SHIP_DATA *target;
                
				if ( !ship )
					continue;
                
				if ( !vch->in_room )
					continue;
                
				if ( channel == CHANNEL_SHIP )
				{
					if ( vch->in_room->vnum > ship->lastroom || vch->in_room->vnum < ship->firstroom )
						continue;
				} /* CHANNEL_SHIP */
                
				target = ship_from_cockpit( vch->in_room->vnum );
                
				if (!target) continue;
                
				if ( channel == CHANNEL_SYSTEM )
				{ 
					if (target->starsystem != ship->starsystem )
						continue;                            
					if ( ( abs(target->vx - ship->vx) > 100*((ship->comm)+(target->comm)+20) )
						|| ( abs(target->vy - ship->vy) > 100*((ship->comm)+(target->comm)+20) )
						|| ( abs(target->vz - ship->vz) > 100*((ship->comm)+(target->comm)+20) ) )
						continue;
				}/* CHANNEL_SYSTEM */
			}/* Ship channels*/
            
    		position		= vch->position;
			if ( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL )
			{
				vch->position	= POS_STANDING;
				vch->position	= position;
			}
	    
			if ( !knows_language( vch, ch->speaking, ch ) 
				&& (!IS_NPC(ch) || ch->speaking != 0) 
				&& ( channel != CHANNEL_NEWBIE && channel != CHANNEL_OOC 
				&& channel != CHANNEL_AUCTION && channel != CHANNEL_AVTALK 
				&& channel != CHANNEL_ANNOUNCE && channel != CHANNEL_QUEST
				&& channel != CHANNEL_MUSIC && channel != CHANNEL_EMOTE )  )
			{
				sbuf = scramble(argument, ch->speaking);

			}/* knows_language */

			/*  Scramble speech if vch or ch has nuisance flag */

			if ( !IS_NPC(ch) && ch->pcdata->nuisance
				&&   ch->pcdata->nuisance->flags > 7 
				&&	(number_percent()<((ch->pcdata->nuisance->flags-7)*10*
				ch->pcdata->nuisance->power)))
				sbuf = scramble(argument,number_range(1,10));

			if ( !IS_NPC(vch) && vch->pcdata->nuisance && 
				vch->pcdata->nuisance->flags > 7 
				&&(number_percent()<((vch->pcdata->nuisance->flags-7)*10*
				vch->pcdata->nuisance->power)))
				sbuf = scramble(argument, number_range(1,10));
		
			MOBtrigger = FALSE;
			if ( channel == CHANNEL_IMMTALK || channel == CHANNEL_AVTALK )
			{ 
				position2		= vch->position;
				vch->position	= POS_STANDING;
				act( channel == CHANNEL_AVTALK ? AT_AVATAR : AT_IMMORT , buf, ch, sbuf, vch, TO_VICT ); 
				vch->position	= position2;
			}
			else if (channel == CHANNEL_WARTALK)
			{ act( AT_WARTALK, buf, ch, sbuf, vch, TO_VICT ); }
			else if (channel == CHANNEL_OOC || channel == CHANNEL_NEWBIE )
			{
				position2		= vch->position;
				vch->position	= POS_STANDING;
				act( AT_OOC, buf, ch, sbuf, vch, TO_VICT ); 
				vch->position	= position2;
			}
			else if ( channel == CHANNEL_SHIP )
			{ act( AT_SHIP, buf, ch, sbuf, vch, TO_VICT ); }
			else if ( channel == CHANNEL_CLAN )
			{
				position2		= vch->position;
				vch->position	= POS_STANDING;
				act( AT_CLAN, buf, ch, sbuf, vch, TO_VICT );
				vch->position	= position2;
			}
			else if (channel == CHANNEL_MUSIC )
			{ act (AT_OOC, buf, ch, sbuf, vch, TO_VICT ); }
			else
			{ act( AT_GOSSIP, buf, ch, sbuf, vch, TO_VICT ); }
			vch->position	= position;
		}/* check for not deaf */
    } /* Descriptor check */
}

void to_channel( const char *argument, int channel, const char *verb, sh_int level )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if ( !first_descriptor || argument[0] == '\0' )
      return;

	sprintf(buf, "%s: %s\r\n",verb,argument);

    for ( d = first_descriptor; d; d = d->next )
    {
	CHAR_DATA *och;
	CHAR_DATA *vch;

	och = d->original ? d->original : d->character;
	vch = d->character;

	if ( !och || !vch )
	  continue;
	if ( !IS_IMMORTAL(vch)
	|| ( get_trust(vch) < sysdata.build_level && channel == CHANNEL_BUILD )
	|| ( get_trust(vch) < sysdata.log_level
	&& ( channel == CHANNEL_LOG || channel == CHANNEL_COMM) ) )
	  continue;

	if ( d->connected == CON_PLAYING
	&&  !IS_SET(och->deaf, channel)
	&&   get_trust( vch ) >= level )
	{
	  set_char_color( AT_LOG, vch );
	  send_to_char( buf, vch );
	}
    }

    return;
}

void do_shiptalk( CHAR_DATA *ch, char *argument )
{
     SHIP_DATA *ship;

     if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )  
     {
    	  send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	  return;
     }
     talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_SHIP, "shiptalk" );
     return;
}    	        

void do_systemtalk( CHAR_DATA *ch, char *argument )
{
     SHIP_DATA *ship;
     
     if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )  
     {
    	  send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	  return;
     }
     talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_SYSTEM, "systemtalk" );
     return;
}    	        

void do_spacetalk( CHAR_DATA *ch, char *argument )
{
     SHIP_DATA *ship;

     if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )  
     {
    	  send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	  return;
     }
     talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_SPACE, "spacetalk" );
     return;
}    	        

void do_ooc( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_OOC, "ooc" );
    return;
}

void do_clantalk( CHAR_DATA *ch, char *argument )
{
    if (NEW_AUTH(ch)) /* new auth */
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    if ( IS_NPC( ch ) || !ch->pcdata->clan )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_CLAN, "clantalk" );
    return;
}

void do_newbiechat( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) || ( ch->top_level > 5 && !IS_IMMORTAL(ch) 
       && !( ch->pcdata->council && !str_cmp( ch->pcdata->council->name, "Newbie Council" ) ) ) )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }
    talk_channel( ch, argument, CHANNEL_NEWBIE, "newbiechat" );
    return;
}


void do_music( CHAR_DATA *ch, char *argument )
{
  if (NEW_AUTH(ch)) /* new auth */
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_MUSIC, "sing" );
    return; 
}


void do_quest( CHAR_DATA *ch, char *argument )
{
    if (NEW_AUTH(ch)) /* new auth */
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_QUEST, "quest" );
    return;
}


void do_shout( CHAR_DATA *ch, char *argument )
{
    if (NEW_AUTH(ch)) /* new auth */
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_SHOUT, "shout" );
  return;
}



void do_yell( CHAR_DATA *ch, char *argument )
{
    if (NEW_AUTH(ch)) /* new auth */
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_YELL, "yell" );
  return;
}

void do_immtalk( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_IMMTALK, "immtalk" );
    return;
}

void do_avtalk( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_AVTALK, "avtalk" );
    return;
}

void do_say( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *vch;
    int actflags;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Say what?\n\r", ch );
	return;
    }

    if ( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }

    actflags = ch->act;
    if ( IS_NPC(ch) )
		REMOVE_BIT( ch->act, ACT_SECRETIVE );
    for ( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
    {
		char *sbuf = argument;
		
		if ( vch == ch )
			continue;
			
		/* Check to see if character is ignoring speaker */
		if (is_ignoring(vch, ch))
		{
			/* continue unless speaker is an immortal */
			if(!IS_IMMORTAL(ch) || get_trust(vch) > get_trust(ch))
				continue;
			else
			{
				set_char_color(AT_IGNORE, vch);
				ch_printf(vch,"You attempt to ignore %s, but are unable to do so.\n\r", ch->name);
			}
		}

	if ( !knows_language(vch, ch->speaking, ch)
	&&  (!IS_NPC(ch) || ch->speaking != 0) )
		sbuf = scramble(argument, ch->speaking);
	sbuf = drunk_speech( sbuf, ch );

	MOBtrigger = FALSE;
	act( AT_SAY, "$n&C says '$t'", ch, sbuf, vch, TO_VICT );
    }
    ch->act = actflags;
    MOBtrigger = FALSE;
    act( AT_SAY, "You say '$T'", ch, NULL, drunk_speech( argument, ch ), TO_CHAR ); 
    if ( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s: %s", IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument );
	append_to_file( LOG_FILE, buf );
    }
    mprog_speech_trigger( argument, ch );
    if ( char_died(ch) )
	return;
    oprog_speech_trigger( argument, ch ); 
    if ( char_died(ch) )
	return;
    rprog_speech_trigger( argument, ch ); 
    return;
}

void do_whisper( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int position;

    REMOVE_BIT( ch->deaf, CHANNEL_WHISPER );

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Whisper to whom what?\n\r", ch );
	return;
    }


    if ( (victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You have a nice little chat with yourself.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) && ( victim->switched ) 
	&& !IS_AFFECTED(victim->switched, AFF_POSSESS) )
    {
      send_to_char( "That player is switched.\n\r", ch );
      return;
    }
   else if ( !IS_NPC( victim ) && ( !victim->desc ) )
    {
      send_to_char( "That player is link-dead.\n\r", ch );
      return;
    }
    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_AFK) )
      {
      send_to_char( "That player is afk.\n\r", ch );
      return;
      }
    if ( IS_SET( victim->deaf, CHANNEL_WHISPER ) 
    && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
    {
      act( AT_PLAIN, "$E has $S whispers turned off.", ch, NULL, victim,
		TO_CHAR );
      return;
    }
    if ( !IS_NPC(victim) &&  IS_SET(victim->act, PLR_SILENCE) )
	send_to_char( "That player is silenced.  They will receive your message but can not respond.\n\r", ch );

    if ( victim->desc		/* make sure desc exists first  -Thoric */
    &&   victim->desc->connected == CON_EDITING 
    &&   get_trust(ch) < LEVEL_GOD )
    {
	act( AT_PLAIN, "$E&G&W is currently in a writing buffer.  Please try again in a few minutes.", ch, 0, victim, TO_CHAR );
        return;
    }

    /* Check to see if target of tell is ignoring the sender */
    if(is_ignoring(victim, ch))	
    {
	/* If the sender is an imm then they cannot be ignored */
	if(!IS_IMMORTAL(ch) || get_trust(victim) > get_trust(ch))
	{
		set_char_color(AT_IGNORE, ch);
		ch_printf(ch,"%s is ignoring you.\n\r",
			victim->name);
		return;
	}
	else
	{
		set_char_color(AT_IGNORE, victim);
		ch_printf(victim, "You attempt to ignore %s, but "
			"are unable to do so.\n\r", ch->name);
	}
    }

    act( AT_WHISPER, "You whisper to $N '$t'", ch, argument, victim, TO_CHAR );
    position		= victim->position;
    victim->position	= POS_STANDING;
	
	{
		char *sbuf = argument;
		
		if ( !knows_language(victim, ch->speaking, ch)
			&&  (!IS_NPC(ch) || ch->speaking != 0) )
		{
			sbuf = scramble(argument, ch->speaking);
		}
		sbuf = drunk_speech( sbuf, ch );

		act( AT_WHISPER, "$n whispers to you '$t'", ch, sbuf, victim, TO_VICT );

		if ( !xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
			act( AT_WHISPER, "$n whispers something to $N.", ch, argument, victim, TO_NOTVICT );

		victim->position	= position;
		if ( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
		{
			sprintf( buf, "%s: %s (whisper to) %s.",
				IS_NPC( ch ) ? ch->short_descr : ch->name,
				argument,
				IS_NPC( victim ) ? victim->short_descr : victim->name );
			append_to_file( LOG_FILE, buf );
		}
	}
	mprog_speech_trigger( argument, ch );
	return;
}


void do_tell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int position;
    CHAR_DATA *switched_victim = NULL;

    REMOVE_BIT( ch->deaf, CHANNEL_TELLS );
    if ( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
		send_to_char( "You can't do that here.\n\r", ch );
		return;
    }

    if (!IS_NPC(ch)
		&& ( IS_SET(ch->act, PLR_SILENCE)
		||   IS_SET(ch->act, PLR_NO_TELL) ) )
    {
		send_to_char( "You can't do that.\n\r", ch );
		return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
		send_to_char( "Tell whom what?\n\r", ch );
		return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL 
		|| ( IS_NPC(victim) && victim->in_room != ch->in_room ) 
		|| (!NEW_AUTH(ch) && NEW_AUTH(victim) && !IS_IMMORTAL(ch) ) )
    {
		send_to_char( "They aren't here.\n\r", ch );
		return;
    }

    if ( ch == victim )
    {
		send_to_char( "You have a nice little chat with yourself.\n\r", ch );
		return;
    }

	if ( !IS_NPC(ch) && victim->in_room != ch->in_room )
	{
		if ( !has_comlink(victim) && !IS_IMMORTAL(ch) )
		{
			send_to_char( "They don't seem to have a comlink!\n\r", ch);
			return;
		}
	}
	if ( !IS_NPC(ch) && victim->in_room != ch->in_room )
	{
		if ( !has_comlink(ch) && !IS_IMMORTAL(victim) )
		{
			send_to_char( "How do you plan todo this without a comlink!\n\r", ch);
			return;
		}
	}


    if (NOT_AUTHED(ch) && !NOT_AUTHED(victim) && !IS_IMMORTAL(victim) )
    {
		send_to_char( "They can't hear you because you are not authorized.\n\r", ch);
		return;
    }

    if ( !IS_NPC( victim ) && ( victim->switched ) 
		&& ( get_trust( ch ) > LEVEL_AVATAR ) 
		&& !IS_AFFECTED(victim->switched, AFF_POSSESS) )
    {
		send_to_char( "That player is switched.\n\r", ch );
		return;
    }

	else if ( !IS_NPC( victim ) && ( victim->switched ) 
		&&  IS_AFFECTED(victim->switched, AFF_POSSESS) ) 
		switched_victim = victim->switched;

	else if ( !IS_NPC( victim ) && ( !victim->desc ) )
    {
		send_to_char( "That player is link-dead.\n\r", ch );
		return;
    }

    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_AFK) )
    {
		send_to_char( "That player is afk.\n\r", ch );
		return;
    }

    if ( IS_SET( victim->deaf, CHANNEL_TELLS ) 
		&& ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
    {
		act( AT_PLAIN, "$E has $S tells turned off.", ch, NULL, victim,
			TO_CHAR );
		return;
    }

    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_SILENCE) )
		send_to_char( "That player is silenced.  They will receive your message but can not respond.\n\r", ch );

    if ( (!IS_IMMORTAL(ch) && !IS_AWAKE(victim) ) )
    {
		act( AT_PLAIN, "$E is too tired to discuss such matters with you now.",
			ch, 0, victim, TO_CHAR );
		return;
    }

    if (!IS_NPC(victim) && xIS_SET(victim->in_room->room_flags, ROOM_SILENCE ) )
    {
		act(AT_PLAIN, "A magic force prevents your message from being heard.",
			ch, 0, victim, TO_CHAR );
		return;
    }

    if ( victim->desc		/* make sure desc exists first  -Thoric */
		&&   victim->desc->connected == CON_EDITING 
		&&   get_trust(ch) < LEVEL_GOD )
    {
		act( AT_PLAIN, "$E is currently in a writing buffer.  Please try again in a few minutes.", ch, 0, victim, TO_CHAR );
        return;
    }

    /* Check to see if target of tell is ignoring the sender */
    if(is_ignoring(victim, ch))	
    {
		/* If the sender is an imm then they cannot be ignored */
		if(!IS_IMMORTAL(ch) || get_trust(victim) > get_trust(ch))
		{
			set_char_color(AT_IGNORE, ch);
			ch_printf(ch,"%s is ignoring you.\n\r",
				victim->name);
			return;
		}
		else
		{
			set_char_color(AT_IGNORE, victim);
			ch_printf(victim, "You attempt to ignore %s, but "
				"are unable to do so.\n\r", ch->name);
		}
    }

    ch->retell = victim;
    
    if(switched_victim)
		victim = switched_victim;
	
	/* Bug fix by guppy@wavecomputers.net */
	MOBtrigger = FALSE;
	{
		char *sbuf = argument;
		
		if ( !knows_language(victim, ch->speaking, ch)
			&&  (!IS_NPC(ch) || ch->speaking != 0) )
		{
			sbuf = scramble(argument, ch->speaking);
		}
		sbuf = drunk_speech( sbuf, ch );

		act( AT_TELL, "You tell $N '$t'", ch, argument, victim, TO_CHAR );
		position		= victim->position;
		victim->position	= POS_STANDING;

		act( AT_TELL, "$n tells you '$t'", ch, sbuf, victim, TO_VICT );
		MOBtrigger = TRUE;

		victim->position	= position;
		victim->reply	= ch;
		if ( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
		{
			sprintf( buf, "%s: %s (tell to) %s.",
				IS_NPC( ch ) ? ch->short_descr : ch->full_name,
				argument,
				IS_NPC( victim ) ? victim->short_descr : victim->name );
			append_to_file( LOG_FILE, buf );
		}
	}
    mprog_speech_trigger( argument, ch );
    return;
}

void do_reply( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int position;

    REMOVE_BIT( ch->deaf, CHANNEL_TELLS );
    if ( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
		send_to_char( "You can't do that here.\n\r", ch );
		return;
    }

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
    {
		send_to_char( "Your message didn't get through.\n\r", ch );
		return;
    }

    if ( ( victim = ch->reply ) == NULL )
    {
		send_to_char( "They aren't here.\n\r", ch );
		return;
    }

    if ( !IS_NPC( victim ) && ( victim->switched )
		&& can_see( ch, victim ) && ( get_trust( ch ) > LEVEL_AVATAR ) )
    {
		send_to_char( "That player is switched.\n\r", ch );
		return;
    }
	else if ( !IS_NPC( victim ) && ( !victim->desc ) )
    {
		send_to_char( "That player is link-dead.\n\r", ch );
		return;
    }

    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_AFK) )
    {
		send_to_char( "That player is afk.\n\r", ch );
		return;
    }

    if ( IS_SET( victim->deaf, CHANNEL_TELLS ) 
		&& ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
    {
		act( AT_PLAIN, "$E has $S tells turned off.", ch, NULL, victim,
			TO_CHAR );
		return;
    }

    if ( ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
		|| ( !IS_NPC(victim) && xIS_SET( victim->in_room->room_flags, ROOM_SILENCE ) ) )
    {
		act( AT_PLAIN, "$E can't hear you.", ch, 0, victim, TO_CHAR );
		return;
    }

    if ( victim->desc		/* make sure desc exists first  -Thoric */
		&&   victim->desc->connected == CON_EDITING 
		&&   get_trust(ch) < LEVEL_GOD )
    {
		act( AT_PLAIN, "$E is currently in a writing buffer.  Please try again in a few minutes.", ch, 0, victim, TO_CHAR );
        return;
    }

    /* Check to see if the receiver is ignoring the sender */
    if(is_ignoring(victim, ch))
    {
		/* If the sender is an imm they cannot be ignored */
		if(!IS_IMMORTAL(ch) || get_trust(victim) > get_trust(ch))
		{
			set_char_color(AT_IGNORE, ch);
			ch_printf(ch,"%s is ignoring you.\n\r",
				victim->name);
			return;
		}
		else
		{
			set_char_color(AT_IGNORE, victim);
			ch_printf(victim, "You attempt to ignore %s, but "
				"are unable to do so.\n\r", ch->name);
		}
    }
	MOBtrigger = FALSE;
    act( AT_TELL, "You tell $N '$t'", ch, argument, victim, TO_CHAR );
    position		= victim->position;
    victim->position	= POS_STANDING;
	{
		char *sbuf = argument;
		
		if ( !knows_language(victim, ch->speaking, ch)
			&&  (!IS_NPC(ch) || ch->speaking != 0) )
		{
			sbuf = scramble(argument, ch->speaking);
		}
		sbuf = drunk_speech( sbuf, ch );
		act( AT_TELL, "$n tells you '$t'", ch, sbuf, victim, TO_VICT );
		MOBtrigger = TRUE;
		victim->position	= position;
		victim->reply	= ch;
		ch->retell		= victim;
	}
    if ( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
		sprintf( buf, "%s: %s (reply to) %s.",
			IS_NPC( ch ) ? ch->short_descr : ch->name,
			argument,
			IS_NPC( victim ) ? victim->short_descr : victim->name );
		append_to_file( LOG_FILE, buf );
    }

    return;
}

void do_retell(CHAR_DATA *ch, char *argument)
{
	char buf[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	int position;
	CHAR_DATA *switched_victim = NULL;

	REMOVE_BIT(ch->deaf, CHANNEL_TELLS);
	if(xIS_SET(ch->in_room->room_flags, ROOM_SILENCE))
	{
		send_to_char("You can't do that here.\n\r", ch);
		return;
	}
	
	if ( !IS_NPC(ch) && (IS_SET(ch->act, PLR_SILENCE)
	||   IS_SET(ch->act, PLR_NO_TELL)) )
	{
		send_to_char("You can't do that.\n\r", ch);
		return;
	}
	
	if(argument[0] == '\0')
	{
		ch_printf(ch, "What message do you wish to send?\n\r");
		return;
	}
	
	victim = ch->retell;
	
	if(!victim)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}
	
	if(!IS_NPC(victim) && (victim->switched) &&
		(get_trust(ch) > LEVEL_AVATAR) &&
		!IS_AFFECTED(victim->switched, AFF_POSSESS))
	{
		send_to_char("That player is switched.\n\r", ch);
		return;
	}
	else if(!IS_NPC(victim) && (victim->switched) &&
		IS_AFFECTED(victim->switched, AFF_POSSESS))
	{
		switched_victim = victim->switched;
	}
	else if(!IS_NPC(victim) &&(!victim->desc))
	{
		send_to_char("That player is link-dead.\n\r", ch);
		return;
	}
	
	if(!IS_NPC(victim) && IS_SET(victim->act, PLR_AFK) )
	{
		send_to_char("That player is afk.\n\r", ch);
		return;
	}
	
	if(IS_SET(victim->deaf, CHANNEL_TELLS) &&
		(!IS_IMMORTAL(ch) || (get_trust(ch) < get_trust(victim))))
	{
		act(AT_PLAIN, "$E has $S tells turned off.", ch, NULL,
			victim, TO_CHAR);
		return;
	}
	
	if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_SILENCE) )
		send_to_char("That player is silenced. They will receive your message, but can not respond.\n\r", ch);
	
	if((!IS_IMMORTAL(ch) && !IS_AWAKE(victim)) ||
		(!IS_NPC(victim) &&
		xIS_SET(victim->in_room->room_flags, ROOM_SILENCE)))
	{
		act(AT_PLAIN, "$E can't hear you.", ch, 0, victim, TO_CHAR);
		return;
	}
	
	if(victim->desc && victim->desc->connected == CON_EDITING &&
		get_trust(ch) < LEVEL_GOD)
	{
		act(AT_PLAIN, "$E is currently in a writing buffer. Please "
			"try again in a few minutes.", ch, 0, victim, TO_CHAR);
		return;
	}
	
	/* check to see if the target is ignoring the sender */
	if(is_ignoring(victim, ch))
	{
		/* if the sender is an imm then they cannot be ignored */
		if(!IS_IMMORTAL(ch) || get_trust(victim) > get_trust(ch))
		{
			set_char_color(AT_IGNORE, ch);
			ch_printf(ch, "%s is ignoring you.\n\r",
				victim->name);
			return;
		}
		else
		{
			set_char_color(AT_IGNORE,victim);
			ch_printf(victim, "You attempy to ignore %s, but "
				"are unable to do so.\n\r", ch->name);
		}
	}

	if(switched_victim)
		victim = switched_victim;

	MOBtrigger = FALSE;
	act(AT_TELL, "You tell $N '$t'", ch, argument, victim, TO_CHAR);
	position = victim->position;
	victim->position = POS_STANDING;
	{
		char *sbuf = argument;
		
		if ( !knows_language(victim, ch->speaking, ch)
			&&  (!IS_NPC(ch) || ch->speaking != 0) )
		{
			sbuf = scramble(argument, ch->speaking);
		}
		sbuf = drunk_speech( sbuf, ch );
		act(AT_TELL, "$g tells you '$t'", ch, sbuf, victim,
			TO_VICT);
	}
	
	MOBtrigger = TRUE;
	victim->position = position;
	victim->reply = ch;
	if(xIS_SET(ch->in_room->room_flags, ROOM_LOGSPEECH))
	{
		sprintf(buf, "%s: %s (retell to) %s.",
			IS_NPC(ch) ? ch->short_descr : ch->name,
			argument,
			IS_NPC(victim) ? victim->short_descr : victim->name);
		append_to_file(LOG_FILE, buf);
	}
	
	mprog_speech_trigger(argument, ch);
	return;
}

void do_emote( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char *plast;
    int actflags;

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_NO_EMOTE) )
    {
	send_to_char( "You can't show your emotions.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Emote what?\n\r", ch );
	return;
    }

    actflags = ch->act;
    if ( IS_NPC( ch ) ) REMOVE_BIT( ch->act, ACT_SECRETIVE );
    for ( plast = argument; *plast != '\0'; plast++ )
	;

    strcpy( buf, argument );
    if ( isalpha(plast[-1]) )
	strcat( buf, "." );

		MOBtrigger = FALSE;
    act( AT_ACTION, "$G $T", ch, NULL, buf, TO_ROOM );
		MOBtrigger = FALSE;
    act( AT_ACTION, "$G $T", ch, NULL, buf, TO_CHAR );
    ch->act = actflags;
    if ( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s %s (emote)", IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument );
	append_to_file( LOG_FILE, buf );
    }
    return;
}


void do_bug( CHAR_DATA *ch, char *argument )
{
    char    buf[MAX_STRING_LENGTH];
    struct  tm *t = localtime(&current_time);

    set_char_color( AT_PLAIN, ch );
    if ( argument[0] == '\0' ) {
        send_to_char( "\n\rUsage:  'bug <message>'  (your location is automatically recorded)\n\r", ch );
        return;
    }
    sprintf( buf, "(%-2.2d/%-2.2d):  %s",
	t->tm_mon+1, t->tm_mday, argument );
    append_file( ch, BUG_FILE2, buf );
    send_to_char( "Thanks, your bug notice has been recorded.\n\r", ch );
    return;
}

void do_ide( CHAR_DATA *ch, char *argument )
{
    send_to_char("If you want to send an idea, type 'idea <message>'.\n\r", ch);
    return;
}

void do_idea( CHAR_DATA *ch, char *argument )
{
    append_file( ch, IDEA_FILE, argument );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}

void do_typo( CHAR_DATA *ch, char *argument )
{
    append_file( ch, TYPO_FILE, argument );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}

void do_qui( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_RED, ch );
    send_to_char( "If you want to QUIT, you have to spell it out.\n\r", ch );
    return;
}

void do_quit( CHAR_DATA *ch, char *argument )
{
    int x, y;
    int level;

    if ( IS_NPC(ch) )
		return;

    if ( ch->position == POS_FIGHTING )
    {
		set_char_color( AT_RED, ch );
		send_to_char( "No way! You are fighting.\n\r", ch );
		return;
    }

    if ( ch->position  < POS_STUNNED  )
    {
		set_char_color( AT_BLOOD, ch );
		send_to_char( "You're not DEAD yet.\n\r", ch );
		return;
    }

	if ( get_timer(ch, TIMER_RECENTFIGHT) > 0
		&&  !IS_IMMORTAL(ch) )
    {
		set_char_color( AT_RED, ch );
		send_to_char( "Your adrenaline is pumping too hard to quit now!\n\r", ch );
		return;
    }

	if ( !IS_NPC(ch) && ch->pcdata->birth_wait > 0 )
	{
		set_char_color( AT_RED,ch);
		send_to_char( "Your still giving birth!\n\r", ch);
		return;
	}

    if ( auction->item != NULL && ((ch == auction->buyer) || (ch == auction->seller) ) )
    {
		send_to_char("Wait until you have bought/sold the item on auction.\n\r", ch);
		return;
    }
    
    if ( !IS_IMMORTAL(ch) && ch->in_room 
		&& !xIS_SET( ch->in_room->room_flags , ROOM_HOTEL ) 
		&& !xIS_SET( ch->in_room->room_flags, ROOM_HOUSE ) 
		&& !xIS_SET( ch->in_room->room_flags, ROOM_PLR_HOME ) 
		&& !xIS_SET( ch->in_room->room_flags, ROOM_EMPTY_HOME ) 
		&& !IS_WAITING_FOR_AUTH(ch) )
    {
		send_to_char("You may not quit here.\n\r", ch);
		send_to_char("You will have to find a safer resting place such as a hotel...\n\r", ch);
		send_to_char("Maybe you could HAIL a speeder.\n\r", ch);
		return;
    }

	/* Get 'em dismounted until we finish mount saving -- Blodkai, 4/97 */
    if ( ch->position == POS_MOUNTED )
		do_dismount( ch, "" );
    set_char_color( AT_WHITE, ch );
    send_to_char( "Your surroundings begin to fade as a mystical swirling vortex of colors\n\renvelops your body... When you come to, things are not as they were.\n\r\n\r", ch );
    act( AT_SAY, "A strange voice says, 'We await your return, $g...'", ch, NULL, NULL, TO_CHAR );
    act( AT_BYE, "$g has left the game.", ch, NULL, NULL, TO_ROOM );
    set_char_color( AT_GREY, ch);

	if ( IS_SET(ch->pcdata->flags, PCFLAG_QUESTING ) )
		REMOVE_BIT( ch->pcdata->flags, PCFLAG_QUESTING);

    sprintf( log_buf, "%s has quit.", ch->name );
    quitting_char = ch;
	if ( !NEW_AUTH(ch) )
		save_char_obj( ch );
	if ( sysdata.save_pets && ch->pcdata->pet )
    {
		act( AT_BYE, "$N follows $S master into the Void.", ch, NULL, 
			ch->pcdata->pet, TO_ROOM );
		extract_char( ch->pcdata->pet, TRUE );
    }
    save_home(ch);
    saving_char = NULL;

    level = get_trust(ch);
    /*
	* After extract_char the ch is no longer valid!
	*/
    extract_char( ch, TRUE );
    for ( x = 0; x < MAX_WEAR; x++ )
	{
		for ( y = 0; y < MAX_LAYERS; y++ )
			save_equipment[x][y] = NULL;
	}
	
	/* don't show who's logging off to leaving player */
	log_string_plus( log_buf, LOG_COMM, level );
	return;
}

void do_ansi( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "ANSI ON or OFF?\n\r", ch );
	return;
    }
    if ( (strcmp(arg,"on")==0) || (strcmp(arg,"ON") == 0) ) {
	SET_BIT(ch->act,PLR_ANSI);
	set_char_color( AT_WHITE + AT_BLINK, ch);
	send_to_char( "ANSI ON!!!\n\r", ch);
	return;
    }

    if ( (strcmp(arg,"off")==0) || (strcmp(arg,"OFF") == 0) ) {
	REMOVE_BIT(ch->act,PLR_ANSI);
	send_to_char( "Okay... ANSI support is now off\n\r", ch );
	return;
    }
}

void do_sound( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "SOUND ON or OFF?\n\r", ch );
	return;
    }
    if ( (strcmp(arg,"on")==0) || (strcmp(arg,"ON") == 0) ) {
	SET_BIT(ch->act,PLR_SOUND);
	set_char_color( AT_WHITE + AT_BLINK, ch);
	send_to_char( "SOUND ON!!!\n\r", ch);
	send_to_char( "!!SOUND(hopeknow U=http://mercury.spaceports.com/~gavin1/)", ch);
	return;
    }

    if ( (strcmp(arg,"off")==0) || (strcmp(arg,"OFF") == 0) ) {
	REMOVE_BIT(ch->act,PLR_SOUND);
	send_to_char( "Okay... SOUND support is now off\n\r", ch );
	return;
    }
}

void do_save( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( !IS_SET( ch->affected_by, race_table[ch->race]->affected ) )
		SET_BIT( ch->affected_by, race_table[ch->race]->affected );
    if ( !IS_SET( ch->resistant, race_table[ch->race]->resist ) )
		SET_BIT( ch->resistant, race_table[ch->race]->resist );
    if ( !IS_SET( ch->susceptible, race_table[ch->race]->suscept ) )
		SET_BIT( ch->susceptible, race_table[ch->race]->suscept );	   

	if ( NOT_AUTHED(ch) )
	{
		send_to_char("You must either be authorized to save.\n\r", ch);
		return;
	}

	/*if ( ch->top_level < 2 )
	{ 
		send_to_char("You can't save until after you've graduated from the academy.\n\r", ch);
		return;
	}*/

    save_char_obj( ch );
    save_home (ch );
    saving_char = NULL;
    send_to_char( "Ok.\n\r", ch );
    return;
}


/*
 * Something from original DikuMUD that Merc yanked out.
 * Used to prevent following loops, which can cause problems if people
 * follow in a loop through an exit leading back into the same room
 * (Which exists in many maze areas)			-Thoric
 */
bool circle_follow( CHAR_DATA *ch, CHAR_DATA *victim )
{
    CHAR_DATA *tmp;

    for ( tmp = victim; tmp; tmp = tmp->master )
	if ( tmp == ch )
	  return TRUE;
    return FALSE;
}


void do_dismiss( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );     

    if ( arg[0] == '\0' )
    {
	send_to_char( "Dismiss whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( ( IS_AFFECTED( victim, AFF_CHARM ) )
    && ( IS_NPC( victim ) )
    && ( victim->master == ch ) )
    {
	stop_follower( victim );
        stop_hating( victim );
        stop_hunting( victim );
        stop_fearing( victim );
        act( AT_ACTION, "$g dismisses $N.", ch, NULL, victim, TO_NOTVICT );
 	act( AT_ACTION, "You dismiss $N.", ch, NULL, victim, TO_CHAR );
    }
    else
    {
	send_to_char( "You cannot dismiss them.\n\r", ch );
    }

return;
}

void do_follow( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Follow whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master )
    {
	act( AT_PLAIN, "But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR );
	return;
    }

    if ( victim == ch )
    {
	if ( !ch->master )
	{
	    send_to_char( "You already follow yourself.\n\r", ch );
	    return;
	}
	stop_follower( ch );
	return;
    }

    /*if ( ( ch->top_level - victim->top_level < -10 || ch->top_level - victim->top_level >  10 )
    &&   !IS_HERO(ch) && !(ch->top_level < 15 && !IS_NPC(victim) 
    && victim->pcdata->council 
    && !str_cmp(victim->pcdata->council->name,"Newbie Council")))
    {
	send_to_char( "You are not of the right caliber to follow.\n\r", ch );
	return;
    }*/

    if ( circle_follow( ch, victim ) )
    {
	send_to_char( "Following in loops is not allowed... sorry.\n\r", ch );
	return;
    }

    if ( ch->master )
	stop_follower( ch );

    add_follower( ch, victim );
    return;
}



void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
    if ( ch->master )
    {
	bug( "Add_follower: non-null master.", 0 );
	return;
    }

    ch->master        = master;
    ch->leader        = NULL;

    /* Support for saving pets --Shaddai */
    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_PET) && !IS_NPC(master) )
	master->pcdata->pet = ch;

    if ( can_see( master, ch ) )
    act( AT_ACTION, "$g now follows you.", ch, NULL, master, TO_VICT );

    act( AT_ACTION, "You now follow $N.",  ch, NULL, master, TO_CHAR );

    return;
}



void stop_follower( CHAR_DATA *ch )
{
    if ( !ch->master )
    {
	bug( "Stop_follower: null master.", 0 );
	return;
    }

    if ( IS_NPC(ch) && !IS_NPC(ch->master) && ch->master->pcdata->pet == ch )
	ch->master->pcdata->pet = NULL;

    if ( IS_AFFECTED(ch, AFF_CHARM) )
    {
	REMOVE_BIT( ch->affected_by, AFF_CHARM );
	affect_strip( ch, gsn_charm_person );
	if ( !IS_NPC(ch->master) )
		ch->master->pcdata->charmies--;
    }

    if ( can_see( ch->master, ch ) )
	if (!(!IS_NPC(ch->master) && IS_IMMORTAL(ch) && !IS_IMMORTAL(ch->master)))
	    act( AT_ACTION, "$g stops following you.",     ch, NULL, ch->master, TO_VICT  );
    act( AT_ACTION, "You stop following $N.",      ch, NULL, ch->master, TO_CHAR    );

    ch->master = NULL;
    ch->leader = NULL;
    return;
}

void die_follower( CHAR_DATA *ch )
{
    CHAR_DATA *fch;

    if ( ch->master )
	stop_follower( ch );

    ch->leader = NULL;

    for ( fch = first_char; fch; fch = fch->next )
    {
	if ( fch->master == ch )
	    stop_follower( fch );
	if ( fch->leader == ch )
	    fch->leader = fch;
    }
    return;
}

void do_order( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
    char argbuf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    bool found;
    bool fAll;

    strcpy( argbuf, argument );
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Order whom to do what?\n\r", ch );
	return;
    }

	if ( !str_prefix("mp",argument) )
    {
		 sprintf( buf , "&RWARNING: &G&W %s is attempting to cheat.\n\r",ch->name );
		echo_to_all( AT_WHITE, buf, ECHOTAR_ALL );
        send_to_char( "But that's cheating!\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You feel like taking, not giving, orders.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	fAll   = TRUE;
	victim = NULL;
    }
    else
    {
	fAll   = FALSE;
	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
	    return;
	}

	if ( !IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch )
	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}
    }

    found = FALSE;
    for ( och = ch->in_room->first_person; och; och = och_next )
    {
	och_next = och->next_in_room;

	if ( IS_AFFECTED(och, AFF_CHARM)
	&&   och->master == ch
	&& ( fAll || och == victim ) )
	{
	    found = TRUE;
	act( AT_ACTION, "$G orders you to '$t'.", ch, argument, och, TO_VICT );
	    interpret( och, argument );
	}
    }

    if ( found )
    {
        sprintf( log_buf, "%s: order %s.", ch->name, argbuf );
        log_string_plus( log_buf, LOG_NORMAL, ch->top_level );
	 	send_to_char( "Ok.\n\r", ch );
        WAIT_STATE( ch, 12 );
    }
    else
	send_to_char( "You have no followers here.\n\r", ch );
    return;
}

void do_group( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	CHAR_DATA *gch;
	CHAR_DATA *leader;

	leader = ch->leader ? ch->leader : ch;
	set_char_color( AT_GREEN, ch );
	ch_printf( ch, "%s's group:\n\r", PERS(leader, ch) );

/* Changed so that no info revealed on possess */
	for ( gch = first_char; gch; gch = gch->next )
	{
	    if ( is_same_group( gch, ch ) )
	    {
		set_char_color( AT_DGREEN, ch );
		if (IS_AFFECTED(gch, AFF_POSSESS))
		  ch_printf( ch,
		    "[%2d %s] %-16s %4s/%4s hp %4s/%4s mv %5s xp\n\r",
		    gch->top_level,
		    IS_NPC(gch) ? "Mob" : race_table[gch->race]->race_name,
		    capitalize( PERS(gch, ch) ),
		    "????",   
		    "????",
		    "????",
		    "????",
		    "?????"    );

		else
		  ch_printf( ch,
		    "[%2d %s] %-16s %4d/%4d hp %4d/%4d mv\n\r",
		    gch->top_level,
		    IS_NPC(gch) ? "Mob" : race_table[gch->race]->race_name,
		    capitalize( PERS(gch, ch) ),
		    gch->hit,   
		    gch->max_hit,
		    gch->move,  
		    gch->max_move   );
	    }
	}
	return;
    }

    if ( !strcmp( arg, "disband" ))
    {
	CHAR_DATA *gch;
	int count = 0;

	if ( ch->leader || ch->master )
	{
	    send_to_char( "You cannot disband a group if you're following someone.\n\r", ch );
	    return;
	}
	
	for ( gch = first_char; gch; gch = gch->next )
	{
	    if ( is_same_group( ch, gch )
	    && ( ch != gch ) )
	    {
		gch->leader = NULL;
		gch->master = NULL;
		count++;
		send_to_char( "Your group is disbanded.\n\r", gch );
	    }
	}

	if ( count == 0 )
	   send_to_char( "You have no group members to disband.\n\r", ch );
	else
	   send_to_char( "You disband your group.\n\r", ch );
	
    return;
    }

    if ( !strcmp( arg, "all" ) )
    {
	CHAR_DATA *rch;
	int count = 0;

        for ( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
	{
           if ( ch != rch
           &&   !IS_NPC( rch )
	   &&   rch->master == ch
	   &&   !ch->master
	   &&   !ch->leader
    	   &&   !is_same_group( rch, ch )
	      )
	   {
		rch->leader = ch;
		count++;
	   }
	}
	
	if ( count == 0 )
	  send_to_char( "You have no eligible group members.\n\r", ch );
	else
	{
     	   act( AT_ACTION, "$g groups $s followers.", ch, NULL, victim, TO_ROOM );
	   send_to_char( "You group your followers.\n\r", ch );
	}
    return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch->master || ( ch->leader && ch->leader != ch ) )
    {
	send_to_char( "But you are following someone else!\n\r", ch );
	return;
    }

    if ( victim->master != ch && ch != victim )
    {
    act( AT_PLAIN, "$N isn't following you.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( is_same_group( victim, ch ) && ch != victim )
    {
	victim->leader = NULL;
    act( AT_ACTION, "$g removes $N from $s group.",   ch, NULL, victim, TO_NOTVICT );
    act( AT_ACTION, "$g removes you from $s group.",  ch, NULL, victim, TO_VICT    );
    act( AT_ACTION, "You remove $N from your group.", ch, NULL, victim, TO_CHAR    );
	return;
    }

    victim->leader = ch;
    act( AT_ACTION, "$N joins $G's group.", ch, NULL, victim, TO_NOTVICT );
    act( AT_ACTION, "You join $G's group.", ch, NULL, victim, TO_VICT    );
    act( AT_ACTION, "$N joins your group.", ch, NULL, victim, TO_CHAR    );
    return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    int members;
    int amount;
    int share;
    int extra;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Split how much?\n\r", ch );
	return;
    }

    amount = atoi( arg );

    if ( amount < 0 )
    {
	send_to_char( "Your group wouldn't like that.\n\r", ch );
	return;
    }

    if ( amount == 0 )
    {
	send_to_char( "You hand out zero credits, but no one notices.\n\r", ch );
	return;
    }

    if ( ch->gold < amount )
    {
	send_to_char( "You don't have that many credits.\n\r", ch );
	return;
    }

    members = 0;
    for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    
    if (( IS_SET(ch->act, PLR_AUTOGOLD)) && (members < 2))
    return;

    if ( members < 2 )
    {
	send_to_char( "Just keep it all.\n\r", ch );
	return;
    }

    share = amount / members;
    extra = amount % members;

    if ( share == 0 )
    {
	send_to_char( "Don't even bother, cheapskate.\n\r", ch );
	return;
    }

    ch->gold -= amount;
    ch->gold += share + extra;

    set_char_color( AT_GOLD, ch );
    ch_printf( ch,
	"You split %d credits.  Your share is %d credits.\n\r",
	amount, share + extra );

    sprintf( buf, "$G splits %d credits.  Your share is %d credits.",
	amount, share );

    for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
    {
	if ( gch != ch && is_same_group( gch, ch ) )
	{
	    act( AT_GOLD, buf, ch, NULL, gch, TO_VICT );
	    gch->gold += share;
	}
    }
    return;
}



void do_gtell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Tell your group what?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->act, PLR_NO_TELL ) )
    {
	send_to_char( "Your message didn't get through!\n\r", ch );
	return;
    }

    /*
     * Note use of send_to_char, so gtell works on sleepers.
     */
/*    sprintf( buf, "%s tells the group '%s'.\n\r", ch->name, argument );*/
    for ( gch = first_char; gch; gch = gch->next )
    {
	if ( is_same_group( gch, ch ) )
	{
	    set_char_color( AT_GTELL, gch );
	    /* Groups unscrambled regardless of clan language.  Other languages
		   still garble though. -- Altrag */
	    if ( knows_language( gch, ch->speaking, gch )
	    ||  (IS_NPC(ch) && !ch->speaking) )
		ch_printf( gch, "%s tells the group '%s'.\n\r", ch->name, argument );
	    else
		ch_printf( gch, "%s tells the group '%s'.\n\r", ch->name, scramble(argument, ch->speaking) );
	}
    }

    return;
}


/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
    if ( ach->leader ) ach = ach->leader;
    if ( bch->leader ) bch = bch->leader;
    return ach == bch;
}

/*
 * this function sends raw argument over the AUCTION: channel
 * I am not too sure if this method is right..
 */

void talk_auction (char *argument)
{
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *original;

    sprintf (buf,"Auction: %s", argument); /* last %s to reset color */

    for (d = first_descriptor; d; d = d->next)
    {
        original = d->original ? d->original : d->character; /* if switched */
        if ((d->connected == CON_PLAYING) && !IS_SET(original->deaf,CHANNEL_AUCTION) 
        && !xIS_SET(original->in_room->room_flags, ROOM_SILENCE) && !NOT_AUTHED(original))
            act( AT_GOSSIP, buf, original, NULL, NULL, TO_CHAR );
    }
}



/*
 * Language support functions. -- Altrag
 * 07/01/96
 *
 * Modified to return how well the language is known 04/04/98 - Thoric
 * Currently returns 100% for known languages... but should really return
 * a number based on player's wisdom (maybe 50+((25-wisdom)*2) ?)
 */
int knows_language( CHAR_DATA *ch, int language, CHAR_DATA *cch )
{
	sh_int sn;

	if ( !IS_NPC(ch) && IS_IMMORTAL(ch) )
		return 100;
	if ( IS_NPC(ch) && !ch->speaks ) /* No langs = knows all for npcs */
		return 100;
	if ( IS_NPC(ch) && IS_SET(ch->speaks, (language & ~LANG_CLAN)) )
		return 100;
	if ( language & LANG_CLAN )
	{
		/* Clan = common for mobs.. snicker.. -- Altrag */
		if ( IS_NPC(ch) || IS_NPC(cch) )
			return 100;
		if ( ch->pcdata->clan == cch->pcdata->clan &&
			 ch->pcdata->clan != NULL )
			return 100;
	}
	if ( !IS_NPC( ch ) )
	{
	    int lang;
	    
		/* Racial languages for PCs */
	    if ( IS_SET(lang_array[race_table[ch->race]->language], language) )
	    	return 100;

	    for ( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
	      if ( IS_SET(language, lang_array[lang]) &&
	      	   IS_SET(ch->speaks, lang_array[lang]) )
	      {
		  if ( (sn = skill_lookup(lang_names[lang])) != -1 )
		    return ch->pcdata->learned[sn];
	      }
	}
	return 0;
}


bool can_learn_lang( CHAR_DATA *ch, int language )
{
	char  buf[MAX_STRING_LENGTH];

	if ( language & LANG_CLAN )
		return FALSE;
	if ( IS_NPC(ch) || IS_IMMORTAL(ch) )
		return FALSE;
	if ( lang_array[race_table[ch->race]->language] & language )
		return FALSE;
	if ( ch->speaks & language )
	{
		int lang;
		
		for ( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
			if ( language & lang_array[lang] )
			{
				int sn;
				
				if ( !(VALID_LANGS & lang_array[lang]) )
					return FALSE;
				if ( ( sn = skill_lookup( lang_names[lang] ) ) < 0 )
				{
					sprintf( buf, "Can_learn_lang: valid language without sn: %d", lang );
					bug( buf, 0 );
					continue;
				}
				if ( ch->pcdata->learned[sn] >= 99 )
					return FALSE;
			}
	}
	if ( VALID_LANGS & language )
		return TRUE;
	return FALSE;
}
int const lang_array[] = {	LANG_COMMON, LANG_WOOKIEE, LANG_TWI_LEK, LANG_RODIAN,
							LANG_HUTT, LANG_MON_CALAMARI, LANG_NOGHRI, LANG_GAMORREAN,
							LANG_JAWA, LANG_ADARIAN, LANG_EWOK, LANG_VERPINE,
							LANG_DEFEL, LANG_TRANDOSHAN, LANG_CHADRA_FAN, LANG_QUARREN, 
							LANG_DUINUOGWUIN, LANG_GOTAL, LANG_DEVARONIAN, LANG_ITHORIAN,
							LANG_BOTHAN, LANG_ASKANI, LANG_R22, LANG_R23, 
							LANG_R24, LANG_R25, LANG_DROID, LANG_SPIRITUAL, 
							LANG_MAGICAL, LANG_GOD, LANG_ANCIENT, LANG_CLAN, LANG_UNKNOWN };

char * const lang_names[] = {	"common", "wookiee", "twilek", "rodian", 
								"hutt", "mon calamari", "noghri", "gamorrean",
								"jawa", "adarian", "ewok", "verpine", 
								"defel", "trandoshan", "chadra-fan", "quarren",
								"duinuogwuin", "gotal", "devaronian", "ithorian",
								"bothan", "askani", "r22", "r23",
								"r24", "r25", "droid", "spiritual",
								"magical", "god", "ancient", "clan", "Unknown"};

/* Note: does not count racial language.  This is intentional (for now). */
int countlangs( int languages )
{
	int numlangs = 0;
	int looper;

	for ( looper = 0; lang_array[looper] != LANG_UNKNOWN; looper++ )
	{
		if ( lang_array[looper] == LANG_CLAN )
			continue;
		if ( languages & lang_array[looper] )
			numlangs++;
	}
	return numlangs;
}
void do_speak( CHAR_DATA *ch, char *argument )
{
	int langs;
	char arg[MAX_INPUT_LENGTH];
	
	argument = one_argument(argument, arg );
	
	if ( !str_cmp( arg, "all" ) && IS_IMMORTAL( ch ) )
	{
		set_char_color( AT_SAY, ch );
		ch->speaking = ~LANG_CLAN;
		send_to_char( "Now speaking all languages.\n\r", ch );
		return;
	}
	for ( langs = 0; lang_array[langs] != LANG_UNKNOWN; langs++ )
		if ( !str_prefix( arg, lang_names[langs] ) && 
			IS_SET(race_table[ch->race]->nospeak, 1 << langs) )
		{
			set_char_color( AT_SAY, ch );
			ch_printf( ch, "%ss cannot speak %s even though some can understand it.\n\r", 
				capitalize(race_table[ch->race]->race_name), arg );
			return;
		}
/*	if ( !str_prefix( arg, "common" ) && ch->race == RACE_WOOKIEE )*/
/*	if ( !str_prefix( arg, "twilek" ) && ch->race != RACE_TWI_LEK )
	{
		set_char_color( AT_SAY, ch );
		send_to_char( "To speak the Twi'lek language requires body parts that you don't have.\n\r", ch );
		return;
	}*/
	for ( langs = 0; lang_array[langs] != LANG_UNKNOWN; langs++ )
	{
		if ( !str_prefix( arg, lang_names[langs] ) )
		{
			if ( knows_language( ch, lang_array[langs], ch ) )
			{
				if ( lang_array[langs] == LANG_CLAN &&
					(IS_NPC(ch) || !ch->pcdata->clan) )
					continue;
				ch->speaking = lang_array[langs];
				set_char_color( AT_SAY, ch );
				ch_printf( ch, "You now speak %s.\n\r", lang_names[langs] );
				return;
			}
		}
	}
	set_char_color( AT_SAY, ch );
	send_to_char( "You do not know that language.\n\r", ch );
}

void do_languages( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	int lang,lang2;
    int sn,sn2;
	int numlang=0;
        	
	argument = one_argument( argument, arg );

	if(IS_NPC(ch)) {
		send_to_char("Mobs Can't Do 'lang' ",ch);
		return;
	}
	if ( arg[0] != '\0' && !str_prefix( arg, "learn" ) &&
		!IS_IMMORTAL(ch) && !IS_NPC(ch) )
	{
		CHAR_DATA *sch;
		char arg2[MAX_INPUT_LENGTH];
		int prct;
		
		argument = one_argument( argument, arg2 );
		if ( arg2[0] == '\0' )
		{
			send_to_char( "Learn which language?\n\r", ch );
			return;
		}
		for ( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
		{
			if ( lang_array[lang] == LANG_CLAN )
				continue;
			if ( !str_prefix( arg2, lang_names[lang] ) )
				break;
		}
		if ( lang_array[lang] == LANG_UNKNOWN )
		{
			send_to_char( "That is not a language.\n\r", ch );
			return;
		}
		if ( !(VALID_LANGS & lang_array[lang]) )
		{
			send_to_char( "You may not learn that language.\n\r", ch );
			return;
		}

		for ( lang2 = 0; lang_array[lang2] != LANG_UNKNOWN; lang2++ )
		{
			if ( lang_array[lang] == LANG_CLAN )
				continue;

			if ( ( sn2 = skill_lookup( lang_names[lang2] ) ) != -1 && str_prefix( arg2, lang_names[lang2] ) )
			{
				if ( ch->pcdata->learned[sn2] >= 1 )
				numlang++;
			}

		}

		if ( numlang == ( get_curr_int(ch) / 3 ) )
		{
			send_to_char( "You are not smart enough to learn any more languages.\n\r", ch);
			return;
		}

		if ( ( sn = skill_lookup( lang_names[lang] ) ) < 0 )
		{
			send_to_char( "That is not a language.\n\r", ch );
			return;
		}
		if ( lang_array[race_table[ch->race]->language] & lang_array[lang] ||
			 ch->pcdata->learned[sn] >= 99 )
		{
			act( AT_PLAIN, "You are already fluent in $t.", ch,
				 lang_names[lang], NULL, TO_CHAR );
			return;
		}
		for ( sch = ch->in_room->first_person; sch; sch = sch->next_in_room )
			if ( IS_NPC(sch) && IS_SET(sch->act, ACT_SCHOLAR) &&
					knows_language( sch, ch->speaking, ch ) &&
					knows_language( sch, lang_array[lang], sch ) &&
					(!sch->speaking || knows_language( ch, sch->speaking, sch )) )
				break;
		if ( !sch )
		{
			send_to_char( "There is no one who can teach that language here.\n\r", ch );
			return;
		}
                if ( ch->gold < 25 )
		{
			send_to_char( "language lessons cost 25 credits... you don't have enough.\n\r", ch );
			return;
		}
                ch->gold -= 25;
		/* Max 12% (5 + 4 + 3) at 24+ int and 21+ wis. -- Altrag */
		prct = 5 + (get_curr_int(ch) / 6) + (get_curr_wis(ch) / 7);
		ch->pcdata->learned[sn] += prct;
		ch->pcdata->learned[sn] = UMIN(ch->pcdata->learned[sn], 99);
		SET_BIT( ch->speaks, lang_array[lang] );
		if ( ch->pcdata->learned[sn] == prct )
			act( AT_PLAIN, "You begin lessons in $t.", ch, lang_names[lang],
				 NULL, TO_CHAR );
		else if ( ch->pcdata->learned[sn] < 60 )
			act( AT_PLAIN, "You continue lessons in $t.", ch, lang_names[lang],
				 NULL, TO_CHAR );
		else if ( ch->pcdata->learned[sn] < 60 + prct )
			act( AT_PLAIN, "You feel you can start communicating in $t.", ch,
				 lang_names[lang], NULL, TO_CHAR );
		else if ( ch->pcdata->learned[sn] < 99 )
			act( AT_PLAIN, "You become more fluent in $t.", ch,
				 lang_names[lang], NULL, TO_CHAR );
		else
			act( AT_PLAIN, "You now speak perfect $t.", ch, lang_names[lang],
				 NULL, TO_CHAR );
		return;
	}
	for ( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
		{
		       if ( !(VALID_LANGS & lang_array[lang]) )
                                continue;		
			if ( ch->speaking & lang_array[lang] ||
				(IS_NPC(ch) && !ch->speaking) )
				set_char_color( AT_RED, ch );
			else
				set_char_color( AT_SAY, ch );
	                if ( ( sn = skill_lookup( lang_names[lang] ) ) < 0 )
			   send_to_char( "(  0) ", ch );
			else
			   ch_printf( ch , "(%3d) ", ch->pcdata->learned[sn] );
		        send_to_char( lang_names[lang], ch );
			send_to_char( "\n\r", ch );			
		}
	send_to_char( "\n\r", ch );
	return;
}

void do_wartalk( CHAR_DATA *ch, char *argument )
{
    if (NEW_AUTH(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_WARTALK, "war" );
    return;
}

void do_announce( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_ANNOUNCE, "annouce" );
    return;
}



void do_rent( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_WHITE, ch );
    send_to_char( "There is no rent here.  Just save and quit.\n\r", ch );
    return;
}

void do_chat( CHAR_DATA *ch, char *argument )
{
    if (!IS_NPC(ch))
	{
		if ( ch->gold < 1 )
		{
	    	send_to_char("&RYou don't have enough credits!\n\r",ch);
    		return;
		}
		ch->gold -= 1;
	}
    
    talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_CHAT, "chat" );
    return;
}

/* tunes a characters comlink to a certain channel */
void do_tune( CHAR_DATA *ch, char *argument )
{
	
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *vch;
	int station;
	if ( IS_NPC(ch) )
	return;

	if( !has_comlink(ch) )
	{
		send_to_char("&RYou don't have a comlink to set!\n\r", ch);
		return;
	}

	if( argument[0] == '\0' )
	{
		ch_printf(ch, "&BYour comlink display reads&G&W %d &B.\n\r"
					,ch->pcdata->comchan);
		return;
	}

	argument = one_argument( argument, arg );
	station = atoi(arg);

	if( station > 101 || station < 0 )
	{
		send_to_char("&RAvailable stations are 0 through 100.\n\r", ch);
		return;
	}

	if( station == 101 && !IS_IMMORTAL(ch) )
	{
		send_to_char("&RAvailable stations are 0 through 100.\n\r", ch);
		return;
	}

	ch_printf(ch, "&BYour comlink clicks as you set it to &W%d&B.\n\r"
				,station);

	for( vch = ch->in_room->first_person ; vch ; vch = vch->next_in_room )
		if( vch != ch )
			ch_printf(vch, "&B%s fiddles with a comlink for a second.\n\r"
						, ch->full_name );

	ch->pcdata->comchan = station;
	return;
}

/* sends a message to the station your tuned too */
void do_talk( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
	int station;
	char buf[MAX_STRING_LENGTH];
	if ( IS_NPC(ch) )
	   return;

	if( !has_comlink(ch) )
	{
		send_to_char("&RYou don't seem to have a comlink!\n\r", ch);
		return;
	}

	if( argument[0] == '\0' )
	{
		send_to_char("Talk what?\n\r", ch);
		return;
	}

	if( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
    {
		send_to_char("Your silenced.\n\r", ch);
		return;
    }

	if( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
		send_to_char( "You can't do that here.\n\r", ch );
		return;
    }

	station = ch->pcdata->comchan;

	if( station <= 0 )
	{
		send_to_char("&RYour comlink is not tuned to a station!\n\r", ch);
		return;
	}
	ch_printf(ch, "&B[&Woutgoing message&B]&W:&B [&W%d&B] &Wreads&B, &W'%s'\n\r"
					,station
					,drunk_speech( argument, ch ));

    for( d = first_descriptor; d; d = d->next )
    {
		CHAR_DATA *och;
		CHAR_DATA *vch;

		och = d->original ? d->original : d->character;
		vch = d->character;

		if( d->connected == CON_PLAYING && vch != ch && !IS_NPC(vch) )
		{
        	char *sbuf = argument;
 
			if ( !has_comlink(vch) )
				continue;

    	    if( xIS_SET( vch->in_room->room_flags, ROOM_SILENCE ) )
		   		continue;

			if(	vch->pcdata->comchan != station && vch->pcdata->comchan != 101 )
				continue;

			if ( !knows_language( vch, ch->speaking, ch ) &&
			(!IS_NPC(ch) || ch->speaking != 0) )
				sbuf = scramble(argument, ch->speaking);

/*			ch_printf(vch, "&B[&Wincoming message&B]&W:&B [&W%d&B/&W%s&B] &Wreads&B, &W'%s'\n\r"
						,station
						,ch->name
						,sbuf);*/
			sprintf( buf, "&B[&Wincoming message&B]&W:&B [&W%d&B/&W$n&B] &Wreads&B, &W'$t'"
				,station);
			act( AT_GOSSIP, buf, ch, drunk_speech(sbuf,ch), vch, TO_VICT );
		}
	}

	return;
}

/* 
 * The Routine
 * Quote Code is by elfren@aros.net
 */

void do_gemote( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_EMOTE, "gemote" );
    return;
}
