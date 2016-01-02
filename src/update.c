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
*			      Regular update module			   *
****************************************************************************/

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "mud.h"

int		check_preg		args( ( CHAR_DATA *ch ) );
void	sickness		args( (CHAR_DATA *ch) );
void	birth_write		args( ( CHAR_DATA *ch, char *argument ) );
void	webwho			args( ( void ) ) ;

/* from swskills.c */
void    add_reinforcements  args( ( CHAR_DATA *ch ) );

/*
 * Local functions.
 */
void    quest_update    args( ( void ) ); /* Vassago - quest.c */
int	hit_gain	args( ( CHAR_DATA *ch ) );
int	move_gain	args( ( CHAR_DATA *ch ) );
void    gain_addiction  args( ( CHAR_DATA *ch ) );
void	mobile_update	args( ( void ) );
void	weather_update	args( ( void ) );
void	time_update	args( ( void ) );	/* FB */
void	update_taxes	args( ( void ) );
void	update_wages	args( ( void ) );
void	char_update	args( ( void ) );
void	obj_update	args( ( void ) );
void	aggr_update	args( ( void ) );
void	room_act_update	args( ( void ) );
void	obj_act_update	args( ( void ) );
void	char_check	args( ( void ) );
void    drunk_randoms	args( ( CHAR_DATA *ch ) );
void    halucinations	args( ( CHAR_DATA *ch ) );
void	subtract_times	args( ( struct timeval *etime,
				struct timeval *stime ) );
void	echo_hint		args( ( void ) );
void    auto_ship_update args( ( void ) );

void    emotion_randoms	args( ( CHAR_DATA *ch ) );

/* dream functions */
void	dream			args( ( CHAR_DATA *ch ) );

/* weather functions - FB */
void	adjust_vectors		args( ( WEATHER_DATA *weather) );
void	get_weather_echo	args( ( WEATHER_DATA *weather) );
void	get_time_echo		args( ( WEATHER_DATA *weather) );

/*
 * Global Variables
 */

CHAR_DATA *	gch_prev;
OBJ_DATA *	gobj_prev;

CHAR_DATA *	timechar;

char * corpse_descs[] =
   { 
     "The corpse of %s will soon be gone.", 
     "The corpse of %s lies here.",
     "The corpse of %s lies here.",
     "The corpse of %s lies here.",
     "The corpse of %s lies here."
   };

char * d_corpse_descs[] =
   { 
     "The shattered remains %s will soon be gone.", 
     "The shattered remains %s are here.",
     "The shattered remains %s are here.",
     "The shattered remains %s are here.",
     "The shattered remains %s are here."
   };

extern int      top_exit;

/*
 * Advancement stuff.
 */
int max_level( CHAR_DATA *ch, int ability)
{
    int level = 0;
    
    if ( IS_NPC(ch) ) 
      return 100;
      
    if ( IS_IMMORTAL(ch) )
      return 200;
    
	if ( ability == COMBAT_ABILITY )
	{
		if ( ch->main_ability == COMBAT_ABILITY ) level = 100;
		if ( ch->main_ability == HUNTING_ABILITY ) level = 50;
		level += race_table[ch->race]->ability[COMBAT_ABILITY];
		level += ch->perm_con + ch->perm_dex + ch->perm_str;
	}
	if ( ability == PILOTING_ABILITY )
	{
		level = 25;
		if ( ch->main_ability == ability ) level = 100;
		if ( ch->main_ability == ENGINEERING_ABILITY ) level = 25;
		if ( ch->main_ability == HUNTING_ABILITY ) level = 25;
		if ( ch->main_ability == SMUGGLING_ABILITY ) level = 50;
		if ( ch->main_ability == PIRACY_ABILITY ) level = 50;
		level += race_table[ch->race]->ability[PILOTING_ABILITY];
		level += ch->perm_dex*2;
    }          
    
	if ( ability == ENGINEERING_ABILITY )
	{
		if ( ch->main_ability == ability ) level = 100;
		if ( ch->main_ability == PILOTING_ABILITY ) level = 20;
		level += race_table[ch->race]->ability[ENGINEERING_ABILITY];
		level += ch->perm_int * 2;
    }

	if ( ability == HUNTING_ABILITY )
    {
		level = 0;
		if ( ch->main_ability == ability ) 
		{
			level = 100;
			level += race_table[ch->race]->ability[HUNTING_ABILITY];
		}
	}

    if ( ability == SMUGGLING_ABILITY )
    {
		if ( ch->main_ability == ability ) level = 100;
		if ( ch->main_ability == PILOTING_ABILITY ) level = 20;
		if ( ch->main_ability == ENGINEERING_ABILITY ) level = 25;
		level += race_table[ch->race]->ability[SMUGGLING_ABILITY];
		level += ch->perm_lck*2;
    }
    
    if ( ability == PIRACY_ABILITY )
    {
		level = 25;
		if ( ch->main_ability == ability )    level = 100;
		level += race_table[ch->race]->ability[PIRACY_ABILITY];
		level += ch->perm_cha + ch->perm_dex;
    }
    
    if ( ability == DIPLOMACY_ABILITY )
    {
		if ( ch->main_ability == ability )    level = 100;
		if ( ch->main_ability == PILOTING_ABILITY ) level = 50;
		level += race_table[ch->race]->ability[DIPLOMACY_ABILITY];
		level += ch->perm_cha*3;
    }
	
	if ( ability == MEDIC_ABILITY )
	{
		level = 50;
		if ( ch->main_ability == ability ) level = 100;
		if ( ch->main_ability == COMBAT_ABILITY ) level = 32;
		if ( ch->main_ability == PILOTING_ABILITY ) level = 34;
		if ( ch->main_ability == ENGINEERING_ABILITY ) level = 25;
		if ( ch->main_ability == SMUGGLING_ABILITY ) level = 25;
		if ( ch->main_ability == DIPLOMACY_ABILITY ) level = 50;
		if ( ch->main_ability == MEDIC_ABILITY ) level = 100;
		if ( ch->main_ability == PIRACY_ABILITY ) level = 1;
		if ( ch->main_ability == HUNTING_ABILITY ) level = 1;
		level += (ch->perm_cha /*+ ch->perm_con + ch->perm_dex*/);
		level += race_table[ch->race]->ability[MEDIC_ABILITY];
	}

	level = URANGE( 1, level, 150 ); 
	
    return level;
}

/*
 * Advancement stuff.
 */
void advance_level( CHAR_DATA *ch , int ability)
{
	char buf[MAX_STRING_LENGTH];
	int add_hp;
    int add_move;
    int add_gold;

	add_hp	= ((con_app[get_curr_con(ch)].hitp) < 0 ? (con_app[get_curr_con(ch)].hitp) : (5)) + number_range( 11,15 );
    add_move	= number_range( 5, (get_curr_con(ch)+get_curr_dex(ch))/4 );		
    add_gold	= number_range(ch->skill_level[ability] * 3,ch->skill_level[ability] * 4);

    add_hp	= UMAX(  1, add_hp   );
    add_move	= UMAX( 10, add_move );

	if ( (ch->max_move + add_move) >= 4000 )
		add_move = 0;
	else if (ability == SMUGGLING_ABILITY)
		ch->max_move	+= add_move;
	else if (ability == PILOTING_ABILITY);
	else if (ability == COMBAT_ABILITY)
	{
		ch->max_hit 	+= add_hp;
		ch->max_move	+= add_move;
	}
	else { ch->max_move	+= add_move; }
	ch->gold	+= add_gold;

	if ( !IS_NPC(ch) )
		REMOVE_BIT( ch->act, PLR_BOUGHT_PET );
	
	if (ability == SMUGGLING_ABILITY)
		sprintf( buf,"Your gain is: &R%d&B/&R%d&W movement points and &R%s&W credits.\n\r",
			add_move,	ch->max_move, num_punct(add_gold) );
	else if (ability == PILOTING_ABILITY)
		sprintf( buf,"Your gain is: &R%s&W credits.\n\r", num_punct(add_gold) );
	else if (ability == COMBAT_ABILITY)
		sprintf( buf,"Your gain is: &R%d&B/&R%d&W Hit Points, &R%d&B/&R%d&W movement points and &R%s&W credits.\n\r",
			add_hp,	ch->max_hit, add_move,	ch->max_move, num_punct(add_gold) );
	else
	{
		sprintf( buf,"Your gain is: &R%d&B/&R%d&W movement points and &R%s&W credits.\n\r",
			add_move, ch->max_move, num_punct(add_gold) );
	}
	set_char_color( AT_WHITE, ch );
	send_to_char( buf, ch );
    
	if ( ch->top_level < ch->skill_level[ability] && ch->top_level < 100 )
	{
		ch->top_level = URANGE( 1 , ch->skill_level[ability] , 100 );
	}

    if ( !IS_NPC(ch) )
		REMOVE_BIT( ch->act, PLR_BOUGHT_PET );

    return;

}   

void gain_exp( CHAR_DATA *ch, int gain , int ability )
{    
	if ( IS_NPC(ch) ) return;

    ch->experience[ability] = UMAX( 0, ch->experience[ability] + gain );
    
	if (NOT_AUTHED(ch) && ch->experience[ability] >= exp_level(7) )
    {
		send_to_char("You can not ascend to a higher level until you are authorized.\n\r", ch);
		ch->experience[ability] = (exp_level( ch->skill_level[ability]+1 ) - 1);
		return;
    }
    while ( ch->experience[ability] >= exp_level( ch->skill_level[ability]+1))
	{
		if ( ch->skill_level[ability] >= max_level(ch , ability) )
		{
			ch->experience[ability] = (exp_level( ch->skill_level[ability]+1 ) - 1);
			return;
		}
		set_char_color( AT_WHITE + AT_BLINK, ch );
		ch_printf( ch, "You have now obtained %s level %d!\n\r", ability_name[ability], ++ch->skill_level[ability] );
		advance_level( ch , ability);
	}

    return;
}



/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
    int gain;

    if ( IS_NPC(ch) )
    {
	gain = ch->top_level;
    }
    else
    {
	gain = UMIN( 5, ch->top_level );

	switch ( ch->position )
	{
	case POS_DEAD:	   return 0;
	case POS_MORTAL:   return -25;
	case POS_INCAP:    return -20;
	case POS_STUNNED:  return get_curr_con(ch) * 2 ;
	case POS_SLEEPING: gain += get_curr_con(ch) * 1.5;	break;
	case POS_RESTING:  gain += get_curr_con(ch); 		break;
	}
                
	if ( ch->pcdata->condition[COND_FULL]   == 0 && !IS_SET(ch->pcdata->cyber, CYBER_REACTOR) )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0 && !IS_SET(ch->pcdata->cyber, CYBER_REACTOR) )
	    gain /= 2;

    }

    if ( IS_AFFECTED(ch, AFF_POISON) )
	gain /= 4;
    
    
/*    if ( get_age( ch ) > 800 )*/
	if ( get_age(ch) > race_table[ch->race]->death_age )
    {
       send_to_char( "You are very old.\n\rYou are becoming weaker with every moment.\n\rSoon you will die.\n\r",ch);
       return -10;
    }
/*    else if ( get_age( ch ) > 500 )*/
	else if ( get_age(ch) > ( race_table[ch->race]->death_age / 1.6) )
       gain /= 10;
/*    else if ( get_age( ch ) > 400 )*/
	else if ( get_age(ch) > ( race_table[ch->race]->death_age / 2 ) )
       gain /= 5;
/*    else if ( get_age( ch ) > 300 )*/
	else if ( get_age(ch) > ( race_table[ch->race]->death_age / 2.6 ) )
       gain /= 2;
    
    if ( ch->race == RACE_TRANDOSHAN )
       gain *= 2 ;

    return UMIN(gain, ch->max_hit - ch->hit);
}

int move_gain( CHAR_DATA *ch )
{
    int gain;

    if ( IS_NPC(ch) )
    {
	gain = ch->top_level;
    }
    else
    {
	gain = UMAX( 15, 2 * ch->top_level );

	switch ( ch->position )
	{
	case POS_DEAD:	   return 0;
	case POS_MORTAL:   return -1;
	case POS_INCAP:    return -1;
	case POS_STUNNED:  return 1;
	case POS_SLEEPING: gain += get_curr_dex(ch) * 2;	break;
	case POS_RESTING:  gain += get_curr_dex(ch);		break;
	}

        
	if ( ch->pcdata->condition[COND_FULL]   == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;
    }

    if ( IS_AFFECTED(ch, AFF_POISON) )
	gain /= 4;
   
	if ( get_age(ch) > race_table[ch->race]->death_age )
	{
		send_to_char( "You are very old.\n\rYou are becoming weaker with every moment.\n\rSoon you will die.\n\r",ch);
		return -10;
	}
	else if ( get_age(ch) > ( race_table[ch->race]->death_age / 1.6) )
		gain /= 10;
	else if ( get_age(ch) > ( race_table[ch->race]->death_age / 2 ) )
		gain /= 5;
	else if ( get_age(ch) > ( race_table[ch->race]->death_age / 2.6 ) )
		gain /= 2;
/*	if ( get_age( ch ) > 500 )
       gain /= 10;
    else if ( get_age( ch ) > 300 )
       gain /= 5;
    else if ( get_age( ch ) > 200 )
       gain /= 2;*/
	  
	if ( xIS_SET(ch->bodyparts,BODY_R_LEG) )
		gain -= 10;
	if ( xIS_SET(ch->bodyparts,BODY_L_LEG) )
		gain -= 10;
	if ( xIS_SET(ch->bodyparts,BODY_L_LEG) && xIS_SET(ch->bodyparts,BODY_R_LEG) )
		gain -= 10;
		/*gain = 0 - ch->move;*/
    
    return UMIN(gain, ch->max_move - ch->move);
}

void gain_addiction( CHAR_DATA *ch )
{
    short drug;
    ch_ret retcode;
    AFFECT_DATA af;
    
    for ( drug=0 ; drug <= 9 ; drug ++ )
    {
       
       if ( ch->pcdata->addiction[drug] < ch->pcdata->drug_level[drug] )
          ch->pcdata->addiction[drug]++;
  
       if ( ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug]+150 )
       {
          switch (ch->pcdata->addiction[drug])
          {
              default:
              case SPICE_GLITTERSTIM:
                  if ( !IS_AFFECTED( ch, AFF_BLIND ) )
	          {
	             	af.type      = gsn_blindness;
	      		af.location  = APPLY_AC;
	      		af.modifier  = 10;
	      		af.duration  = ch->pcdata->addiction[drug];
	      		af.bitvector = AFF_BLIND;
	      		affect_to_char( ch, &af );
	   	  }    
              case SPICE_CARSANUM:
                  if ( !IS_AFFECTED( ch, AFF_WEAKEN ) )
	          {
	             	af.type      = -1;
	      		af.location  = APPLY_DAMROLL;
	      		af.modifier  = -10;
	      		af.duration  = ch->pcdata->addiction[drug];
	      		af.bitvector = AFF_WEAKEN;
	      		affect_to_char( ch, &af );
	   	  }
              case SPICE_RYLL:
                  if ( !IS_AFFECTED( ch, AFF_WEAKEN ) )
	          {
	             	af.type      = -1;
	      		af.location  = APPLY_DEX;
	      		af.modifier  = -5;
	      		af.duration  = ch->pcdata->addiction[drug];
	      		af.bitvector = AFF_WEAKEN;
	      		affect_to_char( ch, &af );
	   	  }
              case SPICE_ANDRIS:
                  if ( !IS_AFFECTED( ch, AFF_WEAKEN ) )
	          {
	             	af.type      = -1;
	      		af.location  = APPLY_CON;
	      		af.modifier  = -5;
	      		af.duration  = ch->pcdata->addiction[drug];
	      		af.bitvector = AFF_WEAKEN;
	      		affect_to_char( ch, &af );
	   	  }
          }
       }
  
       if ( ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug]+200 )
       {      
           ch_printf ( ch, "You feel like you are going to die. You NEED %s\n\r.",   spice_table[drug] );
           worsen_mental_state( ch, 2 );
           retcode = damage(ch, ch, 5, TYPE_UNDEFINED);
       }
       else if ( ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug]+100 )
       {      
           ch_printf ( ch, "You need some %s.\n\r",   spice_table[drug] );
           worsen_mental_state( ch, 2 );
       }
       else if ( ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug]+50 )
       {      
           ch_printf ( ch, "You really crave some %s.\n\r",   spice_table[drug] );
           worsen_mental_state( ch, 1 );
       }
       else if ( ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug]+25 )
       {      
           ch_printf ( ch, "Some more %s would feel quite nice.\n\r",   spice_table[drug] );
       }
       else if ( ch->pcdata->addiction[drug] < ch->pcdata->drug_level[drug]-50 )
       {       
           act( AT_POISON, "$n bends over and vomits.\n\r", ch, NULL, NULL, TO_ROOM );
	   act( AT_POISON, "You vomit.\n\r", ch, NULL, NULL, TO_CHAR );
           ch->pcdata->drug_level[drug] -=10;
       }
       
       if ( ch->pcdata->drug_level[drug] > 1 )
          ch->pcdata->drug_level[drug] -=2;
       else if ( ch->pcdata->drug_level[drug] > 0 )
          ch->pcdata->drug_level[drug] -=1;
       else if ( ch->pcdata->addiction[drug] > 0 && ch->pcdata->drug_level[drug] <= 0 )
          ch->pcdata->addiction[drug]--;
    }
    
}

void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
	int condition;
	ch_ret retcode;

	if ( value == 0 || IS_NPC(ch) || get_trust(ch) >= LEVEL_IMMORTAL || NEW_AUTH(ch)) /* new auth */
		return;

	if ( ( ( iCond == COND_FULL ) || ( iCond == COND_THIRST ) ) 
		&& (ch->pcdata->cyber & CYBER_REACTOR ) )
		return;

	condition = ch->pcdata->condition[iCond];
    ch->pcdata->condition[iCond] = URANGE( 0, condition + value, 48 );

    if ( ch->pcdata->condition[iCond] == 0 )
    {
		switch ( iCond )
		{
		case COND_FULL:
			if ( ch->top_level <= LEVEL_AVATAR && !(ch->pcdata->cyber & CYBER_REACTOR) )
			{
				set_char_color( AT_HUNGRY, ch );
				send_to_char( "You are STARVING!\n\r",  ch );
				act( AT_HUNGRY, "$n is starved half to death!", ch, NULL, NULL, TO_ROOM);
				worsen_mental_state( ch, 1 );
				retcode = damage(ch, ch, 5, TYPE_UNDEFINED);
			}
			break;

		case COND_THIRST:
			if ( ch->top_level <= LEVEL_AVATAR  && !( ch->pcdata->cyber & CYBER_REACTOR) )
			{
				set_char_color( AT_THIRSTY, ch );
				send_to_char( "You are DYING of THIRST!\n\r", ch );
				act( AT_THIRSTY, "$n is dying of thirst!", ch, NULL, NULL, TO_ROOM);
				worsen_mental_state( ch, 2 );
				retcode = damage(ch, ch, 5, TYPE_UNDEFINED);
			}
			break;

		case COND_DRUNK:
			if ( condition != 0 ) {
				set_char_color( AT_SOBER, ch );
				send_to_char( "You are sober.\n\r", ch );
			}
			retcode = rNONE;
			break;
		default:
			bug( "Gain_condition: invalid condition type %d", iCond );
			retcode = rNONE;
			break;
		}
    }

	if ( retcode != rNONE )
		return;

	if ( ch->pcdata->condition[iCond] == 1 )
    {
		switch ( iCond )
		{
		case COND_FULL:
			if ( ch->top_level <= LEVEL_AVATAR  && !(ch->pcdata->cyber & CYBER_REACTOR) )
			{
				set_char_color( AT_HUNGRY, ch );
				send_to_char( "You are really hungry.\n\r",  ch );
				act( AT_HUNGRY, "You can hear $n's stomach growling.", ch, NULL, NULL, TO_ROOM);
				if ( number_bits(1) == 0 )
					worsen_mental_state( ch, 1 );
			} 
			break;

		case COND_THIRST:
			if ( ch->top_level <= LEVEL_AVATAR  && !(ch->pcdata->cyber & CYBER_REACTOR) )
			{
				set_char_color( AT_THIRSTY, ch );
				send_to_char( "You are really thirsty.\n\r", ch );
				worsen_mental_state( ch, 1 );
				act( AT_THIRSTY, "$n looks a little parched.", ch, NULL, NULL, TO_ROOM);
			} 
			break;

		case COND_DRUNK:
			if ( condition != 0 ) {
				set_char_color( AT_SOBER, ch );
				send_to_char( "You are feeling a little less light headed.\n\r", ch );
			}
			break;
		}
    }


	if ( ch->pcdata->condition[iCond] == 2 )
	{
		switch ( iCond )
		{
		case COND_FULL:
			if ( ch->top_level <= LEVEL_AVATAR  && !(ch->pcdata->cyber & CYBER_REACTOR) )
			{
				set_char_color( AT_HUNGRY, ch );
				send_to_char( "You are hungry.\n\r",  ch );
			} 
			break;

		case COND_THIRST:
			if ( ch->top_level <= LEVEL_AVATAR && !(ch->pcdata->cyber & CYBER_REACTOR) )
			{
				set_char_color( AT_THIRSTY, ch );
				send_to_char( "You are thirsty.\n\r", ch );
			}
			break;
		}
	}

    if ( ch->pcdata->condition[iCond] == 3 )
    {
		switch ( iCond )
		{
		case COND_FULL:
			if ( ch->top_level <= LEVEL_AVATAR  && !(ch->pcdata->cyber & CYBER_REACTOR) )
			{
				set_char_color( AT_HUNGRY, ch );
				send_to_char( "You are a mite peckish.\n\r",  ch );
			} 
			break;

		case COND_THIRST:
			if ( ch->top_level <= LEVEL_AVATAR  && !(ch->pcdata->cyber & CYBER_REACTOR) )
			{
				set_char_color( AT_THIRSTY, ch );
				send_to_char( "You could use a sip of something refreshing.\n\r", ch );
			} 
			break;
		}
    }
    return;
}



/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Mud cpu time.
 */
void mobile_update( void )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *ch;
    EXIT_DATA *pexit;
    int door;
    ch_ret     retcode;

    retcode = rNONE;

    /* Examine all mobs. */
    for ( ch = last_char; ch; ch = gch_prev )
    {
	set_cur_char( ch );
	if ( ch == first_char && ch->prev )
	{
	    bug( "mobile_update: first_char->prev != NULL... fixed", 0 );
	    ch->prev = NULL;
	}
	  
	gch_prev = ch->prev;
	
	if ( gch_prev && gch_prev->next != ch )
	{
	    sprintf( buf, "FATAL: Mobile_update: %s->prev->next doesn't point to ch.",
		ch->name );
	    bug( buf, 0 );	    
	    bug( "Short-cutting here", 0 );
	    gch_prev = NULL;
	    ch->prev = NULL;
	    do_shout( ch, "Thoric says, 'Prepare for the worst!'" );
	}
	
	if ( !IS_NPC(ch) )
	{
		emotion_randoms(ch);
	    drunk_randoms(ch);
	    halucinations(ch);
		if (ch->pcdata->stage[0] > 0 || ch->pcdata->stage[2] > 0)
	    {
			CHAR_DATA *vch;
			if (ch->pcdata->stage[1] > 0 && ch->pcdata->stage[2] >= 225)
			{
				char arg[MAX_STRING_LENGTH];
				char memberslist[MAX_STRING_LENGTH];
				char * members;
				
				ch->pcdata->stage[2]++;
				members = str_dup(ch->pcdata->x_partner);
				strcpy(memberslist,"");

				while ( members[0] != '\0' )
				{
					members = one_argument(members,arg);
					if ( (vch = get_char_room( ch, arg)) != NULL )
					{
						if ( vch != NULL &&
							!IS_NPC(vch) &&
							((vch->pcdata->stage[2] >= 200 && vch->sex == SEX_FEMALE) ||
							(ch->pcdata->stage[2] >= 200 && ch->sex == SEX_FEMALE)))
						{
							if (ch->in_room != vch->in_room) continue;
							if (vch->pcdata->stage[2] >= 225 &&
								ch->pcdata->stage[2] >= 225 &&
								vch->pcdata->stage[2] < 240 &&
								ch->pcdata->stage[2] < 240)
							{
								ch->pcdata->stage[2] = 240;
								vch->pcdata->stage[2] = 240;
							}
							if (ch->sex == SEX_MALE && vch->pcdata->stage[2] >= 240)
							{
								act(AT_SOCIAL,"You thrust deeply between $N's warm, damp thighs.",ch,NULL,vch,TO_CHAR);
								act(AT_SOCIAL,"$n thrusts deeply between your warm, damp thighs.",ch,NULL,vch,TO_VICT);
								act(AT_SOCIAL,"$n thrusts deeply between $N's warm, damp thighs.",ch,NULL,vch,TO_NOTVICT);
								if (vch->pcdata->stage[2] > ch->pcdata->stage[2])
									ch->pcdata->stage[2] = vch->pcdata->stage[2];
							}
							else if (ch->sex == SEX_FEMALE && vch->pcdata->stage[2] >= 240)
							{
								act(AT_SOCIAL,"You squeeze your legs tightly around $N, moaning loudly.",ch,NULL,vch,TO_CHAR);
								act(AT_SOCIAL,"$n squeezes $s legs tightly around you, moaning loudly.",ch,NULL,vch,TO_VICT);
								act(AT_SOCIAL,"$n squeezes $s legs tightly around $N, moaning loudly.",ch,NULL,vch,TO_NOTVICT);
								if (vch->pcdata->stage[2] > ch->pcdata->stage[2])
									ch->pcdata->stage[2] = vch->pcdata->stage[2];
							}
						}
						if (ch->pcdata->stage[2] >= 250)
						{
							if ( vch != NULL &&
								!IS_NPC(vch) && ch->in_room == vch->in_room)
							{
								vch->pcdata->stage[2] = 250;
								if (ch->sex == SEX_MALE)
								{
									stage_update(ch,vch,2);
									stage_update(vch,ch,2);
								}
								else
								{
									stage_update(vch,ch,2);
									stage_update(ch,vch,2);
								}
								ch->pcdata->stage[0] = 0;
								vch->pcdata->stage[0] = 0;
								if (!IS_SET(ch->act2, EXTRA_EXP))
								{
									send_to_char("Congratulations on achieving a simultanious orgasm! Recieve 100000 exp!\n\r",ch);
									SET_BIT(ch->act2, EXTRA_EXP);
									ch->exp += 100000;
								}
								if (!IS_SET(vch->act2, EXTRA_EXP))
								{
									send_to_char("Congratulations on achieving a simultanious orgasm! Recieve 100000 exp!\n\r",vch);
									SET_BIT(vch->act2, EXTRA_EXP);
									vch->exp += 100000;
								}
							}
						}
					}
				}
			}
			else
			{
				if (ch->pcdata->stage[0] > 0 && ch->pcdata->stage[2] < 1 &&
					ch->position != POS_RESTING) 
				{
					if (ch->pcdata->stage[0] > 1)
						ch->pcdata->stage[0] -= 1;
					else
						ch->pcdata->stage[0] = 0;
				}
				else if (ch->pcdata->stage[2]>0 && ch->pcdata->stage[0] < 1)
				{
					if (ch->pcdata->stage[2] > 10)
						ch->pcdata->stage[2] -= 10;
					else
						ch->pcdata->stage[2] = 0;
					if (ch->sex == SEX_MALE && ch->pcdata->stage[2] == 0)
						send_to_char("You feel fully recovered.\n\r",ch);
				}
			}
		}
		continue;
	}

	if ( !ch->in_room
	||   IS_AFFECTED(ch, AFF_CHARM)
	||   IS_AFFECTED(ch, AFF_PARALYSIS) )
	    continue;

/* Clean up 'animated corpses' that are not charmed' - Scryn */

        if ( ch->pIndexData->vnum == 5 && !IS_AFFECTED(ch, AFF_CHARM) )
	{
	  if(ch->in_room->first_person)
	    act(AT_MAGIC, "$n returns to the dust from whence $e came.", ch, NULL, NULL, TO_ROOM);
          
   	  if(IS_NPC(ch)) /* Guard against purging switched? */
	    extract_char(ch, TRUE);
	  continue;
	}

	if ( !IS_SET( ch->act, ACT_RUNNING )
	&&   !IS_SET( ch->act, ACT_SENTINEL )
	&&   !ch->fighting && ch->hunting )
	{
	  if (  ch->top_level < 20 )
	   WAIT_STATE( ch, 6 * PULSE_PER_SECOND );
	  else	if (  ch->top_level < 40 )
	   WAIT_STATE( ch, 5 * PULSE_PER_SECOND );
	  else if (  ch->top_level < 60 )
	   WAIT_STATE( ch, 4 * PULSE_PER_SECOND );
	  else	if (  ch->top_level < 80 )
	   WAIT_STATE( ch, 3 * PULSE_PER_SECOND );
	  else	if (  ch->top_level < 100 )
	   WAIT_STATE( ch, 2 * PULSE_PER_SECOND );
	  else
	   WAIT_STATE( ch, 1 * PULSE_PER_SECOND );
	  hunt_victim( ch );
	  continue;
	}  
        else if ( !ch->fighting && !ch->hunting 
        && !IS_SET( ch->act, ACT_RUNNING)
        && ch->was_sentinel && ch->position >= POS_STANDING )
	{
	   act( AT_ACTION, "$n leaves.", ch, NULL, NULL, TO_ROOM );
	   char_from_room( ch );
	   char_to_room( ch , ch->was_sentinel );
	   act( AT_ACTION, "$n arrives.", ch, NULL, NULL, TO_ROOM );
	   SET_BIT( ch->act , ACT_SENTINEL );            
	   ch->was_sentinel = NULL;
	}
	
	/* Examine call for special procedure */
	if ( !IS_SET( ch->act, ACT_RUNNING )
	&&    ch->spec_fun )
	{
	    if ( (*ch->spec_fun) ( ch ) )
		continue;
	    if ( char_died(ch) )
		continue;
	}
        
        if ( !IS_SET( ch->act, ACT_RUNNING )
	&&    ch->spec_2 )
	{
	    if ( (*ch->spec_2) ( ch ) )
		continue;
	    if ( char_died(ch) )
		continue;
	}

	/* Check for mudprogram script on mob */
	if ( IS_SET( ch->pIndexData->progtypes, SCRIPT_PROG ) )
	{
	    mprog_script_trigger( ch );
	    continue;
	}

	if ( ch != cur_char )
	{
	    bug( "Mobile_update: ch != cur_char after spec_fun", 0 );
	    continue;
	}

	/* That's all for sleeping / busy monster */
	if ( ch->position != POS_STANDING )
	    continue;
        
        
	if ( IS_SET(ch->act, ACT_MOUNTED ) )
	{
	    if ( IS_SET(ch->act, ACT_AGGRESSIVE) )
		do_emote( ch, "snarls and growls." );
	    continue;
	}

	if ( xIS_SET(ch->in_room->room_flags, ROOM_SAFE )
	&&   IS_SET(ch->act, ACT_AGGRESSIVE) )
	    do_emote( ch, "glares around and snarls." );


	/* MOBprogram random trigger */
	if ( ch->in_room->area->nplayer > 0 )
	{
	    mprog_random_trigger( ch );
	    if ( char_died(ch) )
		continue;
	    if ( ch->position < POS_STANDING )
	        continue;
	}

        /* MOBprogram hour trigger: do something for an hour */
        mprog_hour_trigger(ch);

	if ( char_died(ch) )
	  continue;

	rprog_hour_trigger(ch);
	if ( char_died(ch) )
	  continue;

	if ( ch->position < POS_STANDING )
	  continue;

	/* Scavenge */
	if ( IS_SET(ch->act, ACT_SCAVENGER)
	&&   ch->in_room->first_content
	&&   number_bits( 2 ) == 0 )
	{
	    OBJ_DATA *obj;
	    OBJ_DATA *obj_best;
	    int max;

	    max         = 1;
	    obj_best    = NULL;
	    for ( obj = ch->in_room->first_content; obj; obj = obj->next_content )
	    {
		if ( CAN_WEAR(obj, ITEM_TAKE) && obj->cost > max 
		&& !IS_OBJ_STAT( obj, ITEM_BURRIED ) )
		{
		    obj_best    = obj;
		    max         = obj->cost;
		}
	    }

	    if ( obj_best )
	    {
		obj_from_room( obj_best );
		obj_to_char( obj_best, ch );
		act( AT_ACTION, "$n gets $p.", ch, obj_best, NULL, TO_ROOM );
	    }
	}

	/* Wander */
	if ( !IS_SET(ch->act, ACT_RUNNING)
	&&   !IS_SET(ch->act, ACT_SENTINEL)
	&&   !IS_SET(ch->act, ACT_PROTOTYPE)
	&& ( door = number_bits( 5 ) ) <= 9
	&& ( pexit = get_exit(ch->in_room, door) ) != NULL
	&&   pexit->to_room
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !xIS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
	&& ( !IS_SET(ch->act, ACT_STAY_AREA)
	||   pexit->to_room->area == ch->in_room->area ) )
	{
	    retcode = move_char( ch, pexit, 0 );
						/* If ch changes position due
						to it's or someother mob's
						movement via MOBProgs,
						continue - Kahn */
	    if ( char_died(ch) )
	      continue;
	    if ( retcode != rNONE || IS_SET(ch->act, ACT_SENTINEL)
	    ||    ch->position < POS_STANDING )
	        continue;
	}

	/* Flee */
	if ( ch->hit < ch->max_hit / 2
	&& ( door = number_bits( 4 ) ) <= 9
	&& ( pexit = get_exit(ch->in_room,door) ) != NULL
	&&   pexit->to_room
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !xIS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) )
	{
	    CHAR_DATA *rch;
	    bool found;

	    found = FALSE;
	    for ( rch  = ch->in_room->first_person;
		  rch;
		  rch  = rch->next_in_room )
	    {
		if ( is_fearing(ch, rch) )
		{
		    switch( number_bits(2) )
		    {
			case 0:
			  sprintf( buf, "Get away from me, %s!", rch->name );
			  break;
			case 1:
			  sprintf( buf, "Leave me be, %s!", rch->name );
			  break;
			case 2:
			  sprintf( buf, "%s is trying to kill me!  Help!", rch->name );
			  break;
			case 3:
			  sprintf( buf, "Someone save me from %s!", rch->name );
			  break;
		    }
		    do_yell( ch, buf );
		    found = TRUE;
		    break;
		}
	    }
	    if ( found )
		retcode = move_char( ch, pexit, 0 );
	}
    }

    return;
}

void update_taxes( void )
{
     PLANET_DATA *planet;
     CLAN_DATA *clan;
     
    for ( planet = first_planet; planet; planet = planet->next )
    {
        clan = planet->governed_by;
        if ( clan )
        {
/*            int sCount = 0;*/
			clan->funds += get_taxes(planet)/720;
			save_clan( clan );
            save_planet( planet );
        }
    }
    
}

/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update( void )
{   
    CHAR_DATA *ch;
    CHAR_DATA *ch_save;
    sh_int save_count = 0;

    ch_save	= NULL;
    for ( ch = last_char; ch; ch = gch_prev )
    {
		if ( ch == first_char && ch->prev )
		{
			bug( "char_update: first_char->prev != NULL... fixed", 0 );
			ch->prev = NULL;
		}
		gch_prev = ch->prev;
		set_cur_char( ch );
		if ( gch_prev && gch_prev->next != ch )
		{
			bug( "char_update: ch->prev->next != ch", 0 );
			return;
		}

        /*
		 *  Do a room_prog rand check right off the bat
		 *   if ch disappears (rprog might wax npc's), continue
		 */
		if(!IS_NPC(ch))
			rprog_random_trigger( ch );

		if( char_died(ch) )
			continue;

		if(IS_NPC(ch))
			mprog_time_trigger(ch);   

		if( char_died(ch) )
			continue;

		rprog_time_trigger(ch);

		if( char_died(ch) )
			continue;

			/*
			 * See if player should be auto-saved.
			 */
		if ( !IS_NPC(ch) && !NEW_AUTH(ch)
			&& current_time - ch->save_time > (sysdata.save_frequency*60) )
			ch_save	= ch;
		else
			ch_save	= NULL;

		if ( ch->position >= POS_STUNNED )
		{
			if ( ch->hit  < ch->max_hit )
				ch->hit  += hit_gain(ch);

			if ( ch->move < ch->max_move )
				ch->move += move_gain(ch);
		}
		if ( !IS_NPC(ch) && IS_SET(ch->/*pcdata->*/act2, EXTRA_INFECTION) &&
			!xIS_SET(ch->in_room->room_flags, ROOM_SANITARY) ) 
		{
			int x;
			for ( x = 0; x < MAX_BODY_PARTS; x++ )
			{
				if ( ch->pcdata->infect_amount[x] > 750 )
					ch->pcdata->infect_amount[x] += 5;
				if ( ch->pcdata->infect_amount[x] > 500 )
					ch->pcdata->infect_amount[x] += 5; 
				if ( ch->pcdata->infect_amount[x] > 250 )
					ch->pcdata->infect_amount[x] += 5;
				if ( ch->pcdata->infect_amount[x] > 100	 )
					ch->pcdata->infect_amount[x] += 5;	
				ch->pcdata->infect_amount[x] = URANGE( 0 , ch->pcdata->infect_amount[x], 1000);
			}
		}

		if ( ch->position == POS_STUNNED )
			update_pos( ch );

		/* To make people with a nuisance's flags life difficult 
		 * --Shaddai
		 */
		
        if ( !IS_NPC(ch) && ch->pcdata->nuisance )
        {
			long int temp;
			
			if ( ch->pcdata->nuisance->flags < MAX_NUISANCE_STAGE )
			{
				temp = ch->pcdata->nuisance->max_time-ch->pcdata->nuisance->time;
				temp *= ch->pcdata->nuisance->flags;
				temp /= MAX_NUISANCE_STAGE;
				temp += ch->pcdata->nuisance->time;
				if ( temp < current_time )
					ch->pcdata->nuisance->flags++;
			}
        }
        
        if ( ch->pcdata )
			gain_addiction( ch );
		
		if ( !IS_NPC(ch) && ch->top_level < LEVEL_IMMORTAL )
		{
			OBJ_DATA *obj;

			if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
				&&   obj->item_type == ITEM_LIGHT
				&&   obj->value[2] > 0 )
			{
				if ( --obj->value[2] == 0 && ch->in_room )
				{
					ch->in_room->light -= obj->count;
					act( AT_ACTION, "$p goes out.", ch, obj, NULL, TO_ROOM );
					act( AT_ACTION, "$p goes out.", ch, obj, NULL, TO_CHAR );
					if ( obj->serial == cur_obj )
						global_objcode = rOBJ_EXPIRED;
					extract_obj( obj );
				}
			}

			if ( ch->pcdata->condition[COND_DRUNK] > 8 )
				worsen_mental_state( ch, ch->pcdata->condition[COND_DRUNK]/8 );
			if ( ch->pcdata->condition[COND_FULL] > 1 )
			{
				switch( ch->position )
				{
				case POS_SLEEPING:  better_mental_state( ch, 4 ); dream( ch );	break;
				case POS_RESTING:   better_mental_state( ch, 3 );	break;
				case POS_SITTING:
				case POS_MOUNTED:   better_mental_state( ch, 2 );	break;
				case POS_STANDING:  better_mental_state( ch, 1 );	break;
				case POS_FIGHTING:
					if ( number_bits(2) == 0 )
						better_mental_state( ch, 1 );
					break;
				}
			}
			if ( ch->pcdata->condition[COND_THIRST] > 1 )
			{
				switch( ch->position )
				{
				case POS_SLEEPING:  better_mental_state( ch, 5 );	break;
				case POS_RESTING:   better_mental_state( ch, 3 );	break;
				case POS_SITTING:
				case POS_MOUNTED:   better_mental_state( ch, 2 );	break;
				case POS_STANDING:  better_mental_state( ch, 1 );	break;
				case POS_FIGHTING:
					if ( number_bits(2) == 0 )
						better_mental_state( ch, 1 );
					break;
				}
			}
			gain_condition( ch, COND_DRUNK,  -1 );
			gain_condition( ch, COND_FULL,  -1 + race_table[ch->race]->hunger_mod );

			if ( !IS_NPC( ch ) && ch->pcdata->nuisance )
			{
				int value;
				
				value = ((0 - ch->pcdata->nuisance->flags)*ch->pcdata->nuisance->power);
				gain_condition ( ch, COND_THIRST, value );
				gain_condition ( ch, COND_FULL, --value );
			}
			
			if ( ch->in_room )
			{
				switch( ch->in_room->sector_type )
				{ 
				default:
					gain_condition( ch, COND_THIRST, -1 + race_table[ch->race]->thirst_mod);  break;
				case SECT_DESERT:
					gain_condition( ch, COND_THIRST, -3 + race_table[ch->race]->thirst_mod);  break;
				case SECT_UNDERWATER:
				case SECT_OCEANFLOOR:
					if ( number_bits(1) == 0 )
						gain_condition( ch, COND_THIRST, -1 + race_table[ch->race]->thirst_mod);  break;
				}
			}
		}

		if ( !char_died(ch) )
		{
		/*
		 * Careful with the damages here,
		 *   MUST NOT refer to ch after damage taken,
		 *   as it may be lethal damage (on NPC).
		 */
			if ( IS_AFFECTED(ch, AFF_POISON) )
			{
				act( AT_POISON, "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
				act( AT_POISON, "You shiver and suffer.", ch, NULL, NULL, TO_CHAR );
				ch->mental_state = URANGE( 20, ch->mental_state	+ 4 , 100 );
				damage( ch, ch, 6, gsn_poison );
			}
			else if ( ch->position == POS_INCAP )
				damage( ch, ch, 1, TYPE_UNDEFINED );
			else if ( ch->position == POS_MORTAL )
				damage( ch, ch, 4, TYPE_UNDEFINED );
			if ( char_died(ch) )
				continue;
			if ( ch->mental_state >= 30 )
			{
				switch( (ch->mental_state+5) / 10 )
				{
				case  3:
					send_to_char( "You feel feverish.\n\r", ch );
					act( AT_ACTION, "$n looks kind of out of it.", ch, NULL, NULL, TO_ROOM );
					break;
				case  4:
					send_to_char( "You do not feel well at all.\n\r", ch );
					act( AT_ACTION, "$n doesn't look too good.", ch, NULL, NULL, TO_ROOM );
					break;
				case  5:
					send_to_char( "You need help!\n\r", ch );
					act( AT_ACTION, "$n looks like $e could use your help.", ch, NULL, NULL, TO_ROOM );
					break;
				case  6:
					send_to_char( "Seekest thou a cleric.\n\r", ch );
					act( AT_ACTION, "Someone should fetch a healer for $n.", ch, NULL, NULL, TO_ROOM );
					break;
				case  7:
					send_to_char( "You feel reality slipping away...\n\r", ch );
					act( AT_ACTION, "$n doesn't appear to be aware of what's going on.", ch, NULL, NULL, TO_ROOM );
					break;
				case  8:
					send_to_char( "You begin to understand... everything.\n\r", ch );
					act( AT_ACTION, "$n starts ranting like a madman!", ch, NULL, NULL, TO_ROOM );
					break;
				case  9:
					send_to_char( "You are ONE with the universe.\n\r", ch );
					act( AT_ACTION, "$n is ranting on about 'the answer', 'ONE' and other mumbo-jumbo...", ch, NULL, NULL, TO_ROOM );
					break;
				case 10:
					send_to_char( "You feel the end is near.\n\r", ch );
					act( AT_ACTION, "$n is muttering and ranting in tongues...", ch, NULL, NULL, TO_ROOM );
					break;
				}
			}
			if ( ch->mental_state <= -30 )
			{
				switch( (abs(ch->mental_state)+5) / 10 )
				{
				case  10:
					if ( ch->position > POS_SLEEPING )
					{
						if ( (ch->position == POS_STANDING
							|| ch->position < POS_FIGHTING)
							&& number_percent()+10 < abs(ch->mental_state) )
							do_sleep( ch, "" );
						else
							send_to_char( "You're barely conscious.\n\r", ch );
					}
					break;
				case   9:
					if ( ch->position > POS_SLEEPING )
					{
						if ( (ch->position == POS_STANDING
							|| ch->position < POS_FIGHTING)
							&& (number_percent()+20) < abs(ch->mental_state) )
							do_sleep( ch, "" );
						else
							send_to_char( "You can barely keep your eyes open.\n\r", ch );
					}
					break;
				case   8:
					if ( ch->position > POS_SLEEPING )
					{
						if ( ch->position < POS_SITTING
							&&  (number_percent()+30) < abs(ch->mental_state) )
							do_sleep( ch, "" );
						else
							send_to_char( "You're extremely drowsy.\n\r", ch );
					}
					break;
				case   7:
					if ( ch->position > POS_RESTING )
						send_to_char( "You feel very unmotivated.\n\r", ch );
					break;
				case   6:
					if ( ch->position > POS_RESTING )
						send_to_char( "You feel sedated.\n\r", ch );
					break;
				case   5:
					if ( ch->position > POS_RESTING )
						send_to_char( "You feel sleepy.\n\r", ch );
					break;
				case   4:
					if ( ch->position > POS_RESTING )
						send_to_char( "You feel tired.\n\r", ch );
					break;
				case   3:
					if ( ch->position > POS_RESTING )
						send_to_char( "You could use a rest.\n\r", ch );
					break;
				}
			}

			if ( ch->backup_wait > 0 )
			{
				--ch->backup_wait;
				if ( ch->backup_wait == 0 )
					add_reinforcements( ch );
			}
			if ( !IS_NPC(ch) && ch->pcdata->birth_wait > 0 )
			{
				--ch->pcdata->birth_wait;
				if ( ch->pcdata->birth_wait == 0 && ch->pcdata->birth_name[0] != '\0' )
				{
					bug("birth_wait: == 0 (child: %s)", ch->pcdata->birth_name,0);
					birth_write( ch, ch->pcdata->birth_name );
					ch->pcdata->genes[9] += 1;
					ch->hit /= 2;
					ch->move /= 2;
					REMOVE_BIT(ch->act2, EXTRA_PREGNANT);
					REMOVE_BIT(ch->act2, EXTRA_LABOUR);
					act(AT_ACTION,"You hold your new baby in your arms.", ch, NULL, NULL, TO_CHAR );
					act(AT_ACTION,"$n holds her new baby in her arms.", ch, NULL, NULL, TO_ROOM );
					act(AT_ACTION,"You lay back in exhaustion.", ch, NULL, NULL, TO_CHAR );
					act(AT_ACTION,"$n lays back in exhaustion.", ch, NULL, NULL, TO_ROOM );
					ch->position = POS_RESTING;
					save_char_obj( ch );
				}
				else if ( ch->pcdata->birth_wait == 0 && ch->pcdata->birth_name[0] == '\0' )
				{
					bug("%s has no ch->pcdata->birth_name", ch->name, 0 );
				}
				else
				{
					act(AT_ACTION,"$n clenches her teeth in extreme pain.", ch, NULL, NULL, TO_ROOM );
					act(AT_ACTION,"You clench your teeth in extreme pain.", ch, NULL, NULL, TO_CHAR );
					ch->hit = (ch->hit * 0.75);
					ch->move = (ch->move * 0.75);
				}
			}
			if ( !IS_NPC(ch) && ch->pcdata->clean_amount > 0 )
			{
				--ch->pcdata->clean_amount;
				if ( ch->pcdata->clean_amount <= 0 )
				{
					act(AT_ACTION,"Your hands start to feel grimy again.", ch, NULL, NULL, TO_CHAR );
					save_char_obj( ch );
				}
			}
				
			if ( !IS_NPC (ch) )
			{
				if ( ++ch->timer > 15 && !ch->desc )
				{
					if ( ch->in_room && !xIS_SET( ch->in_room->room_flags , ROOM_HOTEL ) 
						&& !xIS_SET( ch->in_room->room_flags, ROOM_HOUSE ) 
						&& !xIS_SET( ch->in_room->room_flags, ROOM_PLR_HOME ) 
						&& !xIS_SET( ch->in_room->room_flags, ROOM_EMPTY_HOME ) )
					{
						ROOM_INDEX_DATA * room;
						room = ch->in_room;
						xSET_BIT( room->room_flags, ROOM_HOTEL );
						ch->position = POS_RESTING;
						ch->hit = UMAX ( 1 , ch->hit ); 
						save_char_obj( ch );
						do_quit( ch, "" );
						xREMOVE_BIT( room->room_flags, ROOM_HOTEL );
					}
					else
					{
						ch->position = POS_RESTING;
						ch->hit = UMAX ( 1, ch->hit );
						save_char_obj( ch );
						do_quit( ch, "" );
					}
				}
			}
			else if ( ch == ch_save && IS_SET( sysdata.save_flags, SV_AUTO ) 
				&& ++save_count < 10 )
				save_char_obj( ch ); /* save max of 10 per tick */
		}
    }
	return;
}



/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update( void )
{   
    OBJ_DATA *obj;
    sh_int AT_TEMP;
	
    for ( obj = last_object; obj; obj = gobj_prev )
    {
		CHAR_DATA *rch;
		char *message;

		if ( obj == first_object && obj->prev )
		{
			bug( "obj_update: first_object->prev != NULL... fixed", 0 );
			obj->prev = NULL;
		}
		gobj_prev = obj->prev;
		if ( gobj_prev && gobj_prev->next != obj )
		{
			bug( "obj_update: obj->prev->next != obj", 0 );
			return;
		}
		set_cur_obj( obj );
		if ( obj->carried_by )
			oprog_random_trigger( obj ); 
		else
			if( obj->in_room && obj->in_room->area->nplayer > 0 )
				oprog_random_trigger( obj ); 

		if( obj_extracted(obj) )
			continue;

		if ( obj->item_type == ITEM_WEAPON && obj->carried_by  &&
			( obj->wear_loc == WEAR_WIELD || obj->wear_loc == WEAR_DUAL_WIELD ) &&
			obj->value[3] != WEAPON_BLASTER && obj->value[4] > 0 &&
			obj->value[3] != WEAPON_BOWCASTER &&  obj->value[3] != WEAPON_FORCE_PIKE)
		{
			obj->value[4]--;
			if ( obj->value[4] <= 0 )
			{
				if ( obj->value[3] == WEAPON_LIGHTSABER )
				{
					act( AT_PLAIN, "$p fizzles and dies." , obj->carried_by, obj, NULL, TO_CHAR );
					act( AT_PLAIN, "$n's lightsaber fizzles and dies." , obj->carried_by, NULL, NULL, TO_ROOM );
				}
				else if ( obj->value[3] == WEAPON_VIBRO_BLADE )
				{
					act( AT_PLAIN, "$p stops vibrating." , obj->carried_by, obj, NULL, TO_CHAR );
				}
			}
        }                  

		if ( obj->item_type == ITEM_PIPE )
		{
			if ( IS_SET( obj->value[3], PIPE_LIT ) )
			{
				if ( --obj->value[1] <= 0 )
				{
					obj->value[1] = 0;
					REMOVE_BIT( obj->value[3], PIPE_LIT );
				}
				else if ( IS_SET( obj->value[3], PIPE_HOT ) )
					REMOVE_BIT( obj->value[3], PIPE_HOT );
				else
				{
					if ( IS_SET( obj->value[3], PIPE_GOINGOUT ) )
					{
						REMOVE_BIT( obj->value[3], PIPE_LIT );
						REMOVE_BIT( obj->value[3], PIPE_GOINGOUT );
					}
					else
						SET_BIT( obj->value[3], PIPE_GOINGOUT );
				}
				if ( !IS_SET( obj->value[3], PIPE_LIT ) )
					SET_BIT( obj->value[3], PIPE_FULLOFASH );
			}
			else
				REMOVE_BIT( obj->value[3], PIPE_HOT );
		}


		/* Corpse decay (npc corpses decay at 8 times the rate of pc corpses) - Narn */

        if ( obj->item_type == ITEM_CORPSE_PC || obj->item_type == ITEM_CORPSE_NPC 
			|| obj->item_type == ITEM_DROID_CORPSE )
        {
			sh_int timerfrac = UMAX(1, obj->timer - 1);
			if ( obj->item_type == ITEM_CORPSE_PC )
				timerfrac = (int)(obj->timer / 8 + 1);

			if ( obj->timer > 0 && obj->value[2] > timerfrac )
			{
				char buf[MAX_STRING_LENGTH];
				char name[MAX_STRING_LENGTH];
				char *bufptr;
				bufptr = one_argument( obj->short_descr, name ); 
				bufptr = one_argument( bufptr, name ); 
				bufptr = one_argument( bufptr, name ); 

				separate_obj(obj);
				obj->value[2] = timerfrac;
				if ( obj->item_type == ITEM_DROID_CORPSE )
					sprintf( buf, d_corpse_descs[ UMIN( timerfrac - 1, 4 ) ], 
					bufptr ); 
				else 
					sprintf( buf, corpse_descs[ UMIN( timerfrac - 1, 4 ) ], 
					capitalize( bufptr ) ); 

				STRFREE( obj->description );
				obj->description = STRALLOC( buf ); 
			}  
        }
		
		/* don't let inventory decay */
		if ( IS_OBJ_STAT(obj, ITEM_INVENTORY) )
			continue;

        if ( obj->timer > 0 && obj->timer < 5 && obj->item_type == ITEM_ARMOR )
        {
			if ( obj->carried_by )
			{ 
				act( AT_TEMP, "$p is almost dead." , obj->carried_by, obj, NULL, TO_CHAR );
			}
        }

		if ( ( obj->timer <= 0 || --obj->timer > 0 ) )
			continue;
		
		
		/* if we get this far, object's timer has expired. */
		
		AT_TEMP = AT_PLAIN;
		switch ( obj->item_type )
		{
		default:
			message = "$p has depleted itself.";
			AT_TEMP = AT_PLAIN;
			break;
			
		case ITEM_GRENADE:
			explode( obj );
			return;
			break;
			
		case ITEM_FOUNTAIN:
			message = "$p dries up.";
			AT_TEMP = AT_BLUE;
			break;
		case ITEM_CORPSE_NPC:
			message = "$p decays into dust and blows away.";
			AT_TEMP = AT_OBJECT;
			break;
		case ITEM_DROID_CORPSE:
			message = "$p rusts away into oblivion.";
			AT_TEMP = AT_OBJECT;
			break;
		case ITEM_CORPSE_PC:
			message = "$p decays into dust and is blown away...";
			AT_TEMP = AT_MAGIC;
			break;
		case ITEM_FOOD:
			message = "$p is devoured by a swarm of maggots.";
			AT_TEMP = AT_HUNGRY;
			break;
		case ITEM_SCRAPS:
			message = "$p crumbles and decays into nothing.";
			AT_TEMP = AT_OBJECT;
			break;
		case ITEM_FIRE:
			if (obj->in_room)
				--obj->in_room->light;
			message = "$p burns out.";
			AT_TEMP = AT_FIRE;
		}
        
		if ( obj->carried_by )
		{ 
			act( AT_TEMP, message, obj->carried_by, obj, NULL, TO_CHAR );
		}
		else if ( obj->in_room
	  &&      ( rch = obj->in_room->first_person ) != NULL
	  &&	!IS_OBJ_STAT( obj, ITEM_BURRIED ) )
	  {
	    act( AT_TEMP, message, rch, obj, NULL, TO_ROOM );
            act( AT_TEMP, message, rch, obj, NULL, TO_CHAR );
	  }
 
	if ( obj->serial == cur_obj )
	  global_objcode = rOBJ_EXPIRED;
	extract_obj( obj );
    }
    return;
}


/*
 * Function to check important stuff happening to a player
 * This function should take about 5% of mud cpu time
 */
void char_check( void )
{
    CHAR_DATA *ch, *ch_next;
    EXIT_DATA *pexit;
    static int cnt = 0;
    int door, retcode;

    cnt = (cnt+1) % 2;

    for ( ch = first_char; ch; ch = ch_next )
    {
	set_cur_char(ch);
	ch_next = ch->next;
	will_fall(ch, 0);

	if ( char_died( ch ) )
	  continue;

	if ( IS_NPC( ch ) )
	{
	    if ( cnt != 0 )
		continue;

	    /* running mobs	-Thoric */
	    if ( IS_SET(ch->act, ACT_RUNNING) )
	    {
		if ( !IS_SET( ch->act, ACT_SENTINEL )
		&&   !ch->fighting && ch->hunting )
		{
		    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
		    hunt_victim( ch );
		    continue;
		}

		if ( ch->spec_fun )
		{
		    if ( (*ch->spec_fun) ( ch ) )
			continue;
		    if ( char_died(ch) )
			continue;
		}
                if ( ch->spec_2 )
		{
		    if ( (*ch->spec_2) ( ch ) )
			continue;
		    if ( char_died(ch) )
			continue;
		}

		if ( !IS_SET(ch->act, ACT_SENTINEL)
		&&   !IS_SET(ch->act, ACT_PROTOTYPE)
		&& ( door = number_bits( 4 ) ) <= 9
		&& ( pexit = get_exit(ch->in_room, door) ) != NULL
		&&   pexit->to_room
		&&   !IS_SET(pexit->exit_info, EX_CLOSED)
		&&   !xIS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
		&& ( !IS_SET(ch->act, ACT_STAY_AREA)
		||   pexit->to_room->area == ch->in_room->area ) )
		{
		    retcode = move_char( ch, pexit, 0 );
		    if ( char_died(ch) )
			continue;
		    if ( retcode != rNONE || IS_SET(ch->act, ACT_SENTINEL)
		    ||    ch->position < POS_STANDING )
			continue;
		}
	    }
	    continue;
	}
	else
	{
	    if ( ch->mount
	    &&   ch->in_room != ch->mount->in_room )
	    {
		REMOVE_BIT( ch->mount->act, ACT_MOUNTED );
		ch->mount = NULL;
		ch->position = POS_STANDING;
		send_to_char( "No longer upon your mount, you fall to the ground...\n\rOUCH!\n\r", ch );
	    }

	    if ( ( ch->in_room && ch->in_room->sector_type == SECT_UNDERWATER )
	    || ( ch->in_room && ch->in_room->sector_type == SECT_OCEANFLOOR ) )
	    {
		if ( !IS_AFFECTED( ch, AFF_AQUA_BREATH ) )
		{
		    if ( get_trust(ch) < LEVEL_IMMORTAL )
		    {
			int dam;

        		  	
			dam = number_range( ch->max_hit / 50 , ch->max_hit / 30 );
			dam = UMAX( 1, dam );
			if(  ch->hit <= 0 )
			    dam = UMIN( 10, dam );
			if ( number_bits(3) == 0 )
			  send_to_char( "You cough and choke as you try to breathe water!\n\r", ch );
			damage( ch, ch, dam, TYPE_UNDEFINED );
		    }
		}
	    }
	
	    if ( char_died( ch ) )
		continue; 

	    if ( ch->in_room
	    && (( ch->in_room->sector_type == SECT_WATER_NOSWIM )
	    ||  ( ch->in_room->sector_type == SECT_WATER_SWIM ) ) )
	    {
		if ( !IS_AFFECTED( ch, AFF_FLYING )
		&& !IS_AFFECTED( ch, AFF_FLOATING ) 
		&& !IS_AFFECTED( ch, AFF_AQUA_BREATH )
		&& !ch->mount )
		{
			if ( get_trust(ch) < LEVEL_IMMORTAL )
			{
			    int dam;

			    if ( ch->move > 0 )
				    ch->move--;
			    else
			    {
				dam = number_range( ch->max_hit / 50, ch->max_hit / 30 );
				dam = UMAX( 1, dam );
				if(  ch->hit <= 0 )
			           dam = UMIN( 10, dam );
				if ( number_bits(3) == 0 )
				   send_to_char( "Struggling with exhaustion, you choke on a mouthful of water.\n\r", ch );
				damage( ch, ch, dam, TYPE_UNDEFINED );
			    }
          	      }
		}
	    }

	}
    }
}


/*
 * Aggress.
 *
 * for each descriptor
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function should take 5% to 10% of ALL mud cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *   who leads the party into the room.
 *
 */
void aggr_update( void )
{
    DESCRIPTOR_DATA *d, *dnext;
    CHAR_DATA *wch;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;
    struct act_prog_data *apdtmp;

#ifdef UNDEFD
  /*
   *  GRUNT!  To do
   *
   */
        if ( IS_NPC( wch ) && wch->mpactnum > 0
	    && wch->in_room->area->nplayer > 0 )
        {
            MPROG_ACT_LIST * tmp_act, *tmp2_act;
	    for ( tmp_act = wch->mpact; tmp_act;
		 tmp_act = tmp_act->next )
	    {
                 oprog_wordlist_check( tmp_act->buf,wch, tmp_act->ch,
				      tmp_act->obj, tmp_act->vo, ACT_PROG );
                 DISPOSE( tmp_act->buf );
            }
	    for ( tmp_act = wch->mpact; tmp_act; tmp_act = tmp2_act )
	    {
                 tmp2_act = tmp_act->next;
                 DISPOSE( tmp_act );
            }
            wch->mpactnum = 0;
            wch->mpact    = NULL;
        }
#endif

    /* check mobprog act queue */
    while ( (apdtmp = mob_act_list) != NULL )
    {
	wch = mob_act_list->vo;
	if ( !char_died(wch) && wch->mpactnum > 0 )
	{
	    MPROG_ACT_LIST * tmp_act;

	    while ( (tmp_act = wch->mpact) != NULL )
	    {
		if ( tmp_act->obj && obj_extracted(tmp_act->obj) )
		  tmp_act->obj = NULL;
		if ( tmp_act->ch && !char_died(tmp_act->ch) )
		  mprog_wordlist_check( tmp_act->buf, wch, tmp_act->ch,
					tmp_act->obj, tmp_act->vo, ACT_PROG );
		wch->mpact = tmp_act->next;
		DISPOSE(tmp_act->buf);
		DISPOSE(tmp_act);
	    }
	    wch->mpactnum = 0;
	    wch->mpact    = NULL;
        }
	mob_act_list = apdtmp->next;
	DISPOSE( apdtmp );
    }


    /*
     * Just check descriptors here for victims to aggressive mobs
     * We can check for linkdead victims to mobile_update	-Thoric
     */
    for ( d = first_descriptor; d; d = dnext )
    {
	dnext = d->next;
	if ( d->connected != CON_PLAYING || (wch=d->character) == NULL )
	   continue;

	if ( char_died(wch)
	||   IS_NPC(wch)
	||   wch->top_level >= LEVEL_IMMORTAL
	||  !wch->in_room )
	    continue;

	for ( ch = wch->in_room->first_person; ch; ch = ch_next )
	{
	    int count;

	    ch_next	= ch->next_in_room;

	    if ( !IS_NPC(ch)
	    ||   ch->fighting
	    ||   IS_AFFECTED(ch, AFF_CHARM)
	    ||   !IS_AWAKE(ch)
	    ||   ( IS_SET(ch->act, ACT_WIMPY) )
	    ||   !can_see( ch, wch ) )
		continue;

	    if ( is_hating( ch, wch ) )
	    {
		found_prey( ch, wch );
		continue;
	    }

	    if ( !IS_SET(ch->act, ACT_AGGRESSIVE)
	    ||    IS_SET(ch->act, ACT_MOUNTED)
	    ||    xIS_SET(ch->in_room->room_flags, ROOM_SAFE ) )
		continue;

	    victim = wch;

	    if ( !victim )
	    {
		bug( "Aggr_update: null victim.", count );
		continue;
	    }

            if ( get_timer(victim, TIMER_RECENTFIGHT) > 0 )
                continue;

	    if ( IS_NPC(ch) && IS_SET(ch->attacks, ATCK_BACKSTAB ) )
	    {
		OBJ_DATA *obj;

		if ( !ch->mount
    		&& (obj = get_eq_char( ch, WEAR_WIELD )) != NULL
    		&& obj->value[3] == 11
		&& !victim->fighting
		&& victim->hit >= victim->max_hit )
		{
		    WAIT_STATE( ch, skill_table[gsn_backstab]->beats );
		    if ( !IS_AWAKE(victim)
		    ||   number_percent( )+5 < ch->top_level )
		    {
			global_retcode = multi_hit( ch, victim, gsn_backstab );
			continue;
		    }
		    else
		    {
			global_retcode = damage( ch, victim, 0, gsn_backstab );
			continue;
		    }
		}
	    }
	    global_retcode = multi_hit( ch, victim, TYPE_UNDEFINED );
	}
    }

    return;
}

/* From interp.c */
bool check_social  args( ( CHAR_DATA *ch, char *command, char *argument ) );

/*
 * drunk randoms	- Tricops
 * (Made part of mobile_update	-Thoric)
 */
void drunk_randoms( CHAR_DATA *ch )
{
    CHAR_DATA *rvch = NULL;
    CHAR_DATA *vch;
    sh_int drunk;
    sh_int position;

    if ( IS_NPC( ch ) || ch->pcdata->condition[COND_DRUNK] <= 0 )
	return;

    if ( number_percent() < 30 )
	return;

    drunk = ch->pcdata->condition[COND_DRUNK];
    position = ch->position;
    ch->position = POS_STANDING;

    if ( number_percent() < (2*drunk / 20) )
	check_social( ch, "burp", "" );
    else
    if ( number_percent() < (2*drunk / 20) )
	check_social( ch, "hiccup", "" );
    else
    if ( number_percent() < (2*drunk / 20) )
	check_social( ch, "drool", "" );
    else
    if ( number_percent() < (2*drunk / 20) )
	check_social( ch, "fart", "" );
    else
    if ( drunk > (10+(get_curr_con(ch)/5))
    &&   number_percent() < ( 2 * drunk / 18 ) )
    {
	for ( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
	    if ( number_percent() < 10 )
		rvch = vch;
	check_social( ch, "puke", (rvch ? rvch->name : "") );
    }

    ch->position = position;
    return;
}

/*
 * drunk randoms	- Tricops
 * (Made part of mobile_update	-Thoric)
 */
void emotion_randoms( CHAR_DATA *ch )
{
    CHAR_DATA *rvch = NULL;
    CHAR_DATA *vch;
    sh_int emotion;
    sh_int position;


    if ( IS_NPC( ch ) || ch->emotional_state >= 10 || ch->emotional_state <= -10 )
	return;

    if ( number_percent() < 30 )
	return;

    emotion = ch->emotional_state ;
    position = ch->position;
    ch->position = POS_STANDING;

	if ( emotion > 0 )
	{
		if ( number_percent() < (2*emotion / 20) )
			check_social( ch, "sniff", "" );
		else if ( number_percent() < (2*emotion / 20) )
			check_social( ch, "cry", "" );
		else if ( number_percent() < (2*emotion / 20) )
			check_social( ch, "drool", "" );
		else if ( number_percent() < (2*emotion / 20) )
			check_social( ch, "fart", "" );
		else if ( emotion > (10+(get_curr_con(ch)/5))
			&&   number_percent() < ( 2 * emotion / 18 ) )
		{
			for ( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
				if ( number_percent() < 10 )
					rvch = vch;
				check_social( ch, "puke", (rvch ? rvch->name : "") );
		}
	}
	else
	{
		;
	}

    ch->position = position;
    return;
}

void halucinations( CHAR_DATA *ch )
{
    if ( ch->mental_state >= 30 && number_bits(5 - (ch->mental_state >= 50) - (ch->mental_state >= 75)) == 0 )
    {
	char *t;

	switch( number_range( 1, UMIN(20, (ch->mental_state+5) / 5)) )
	{
	    default:
	    case  1: t = "You feel very restless... you can't sit still.\n\r";		break;
	    case  2: t = "You're tingling all over.\n\r";				break;
	    case  3: t = "Your skin is crawling.\n\r";					break;
	    case  4: t = "You suddenly feel that something is terribly wrong.\n\r";	break;
	    case  5: t = "Those damn little fairies keep laughing at you!\n\r";		break;
	    case  6: t = "You can hear your mother crying...\n\r";			break;
	    case  7: t = "Have you been here before, or not?  You're not sure...\n\r";	break;
	    case  8: t = "Painful childhood memories flash through your mind.\n\r";	break;
	    case  9: t = "You hear someone call your name in the distance...\n\r";	break;
	    case 10: t = "Your head is pulsating... you can't think straight.\n\r";	break;
	    case 11: t = "The ground... seems to be squirming...\n\r";			break;
	    case 12: t = "You're not quite sure what is real anymore.\n\r";		break;
	    case 13: t = "It's all a dream... or is it?\n\r";				break;
	    case 14: t = "They're coming to get you... coming to take you away...\n\r";	break;
	    case 15: t = "You begin to feel all powerful!\n\r";				break;
	    case 16: t = "You're light as air... the heavens are yours for the taking.\n\r";	break;
	    case 17: t = "Your whole life flashes by... and your future...\n\r";	break;
	    case 18: t = "You are everywhere and everything... you know all and are all!\n\r";	break;
	    case 19: t = "You feel immortal!\n\r";					break;
	    case 20: t = "Ahh... the power of a Supreme Entity... what to do...\n\r";	break;
	}
	send_to_char( t, ch );
    }
    return;
}

void auth_update( void ) 
{ 
	CHAR_DATA *victim; 
	DESCRIPTOR_DATA *d; 
	char buf [MAX_INPUT_LENGTH], log_buf [MAX_INPUT_LENGTH];
	bool found_hit = FALSE;         /* was at least one found? */

	strcpy (log_buf, "Pending authorizations:\n\r" );
	for ( d = first_descriptor; d; d = d->next ) 
	{
		if ( (victim = d->character) && IS_WAITING_FOR_AUTH(victim) )
		{
			found_hit = TRUE;
			sprintf( buf,"  Name: %-10.10s  Race: %-10.10s  Sex: %s  Ability: %-10.10s\n\r",
				victim->name, race_table[victim->race]->race_name, 
				victim->sex == SEX_MALE ? "Male   " : victim->sex == SEX_FEMALE ? "Female " : "Neutral",
				ability_name[victim->main_ability] );
			strcat (log_buf, buf);
		}
	}
	if (found_hit)
	{
		/*	log_string( log_buf ); */
		to_channel( log_buf, CHANNEL_AUTH, "Auth", 1);
	}
} 

/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler( void )
{
    static  int     pulse_taxes;
    static  int     pulse_area;
    static  int     pulse_mobile;
    static  int     pulse_violence;
    static  int     pulse_point;
    static  int	    pulse_second;
    static  int     pulse_space;
    static  int     pulse_ship;
    static  int     pulse_recharge;
    struct timeval stime;
    struct timeval etime;

    if ( timechar )
    {
		set_char_color(AT_PLAIN, timechar);
		send_to_char( "Starting update timer.\n\r", timechar );
		gettimeofday(&stime, NULL);
    }
    
    if ( --pulse_area     <= 0 )
    {
		pulse_area	= number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 );
		area_update	( );
		quest_update     ( );
		auto_ship_update ( );
		update_planet( );
    }
    
    if ( --pulse_taxes     <= 0 )
    {
		pulse_taxes	= PULSE_TAXES ;
		update_taxes	( );
    }

    if ( --pulse_mobile   <= 0 )
    {
		pulse_mobile	= PULSE_MOBILE;
		mobile_update  ( );
    }
    
    if ( --pulse_space   <= 0 )
    {
		pulse_space    = PULSE_SPACE;
		update_space  ( );
		update_bus ( );
		update_traffic ( );
    }

    if ( --pulse_recharge <= 0 )
    {
		pulse_recharge = PULSE_SPACE/3;
		recharge_ships ( );
    }
    
    if ( --pulse_ship   <= 0 )
    {
		pulse_ship  = PULSE_SPACE/10;
		move_ships  ( );
    }                        
    
    if ( --pulse_violence <= 0 )
    {
		pulse_violence	= PULSE_VIOLENCE;
		violence_update	( );
    }

    if ( --pulse_point    <= 0 )
    {
		pulse_point     = number_range( PULSE_TICK * 0.75, PULSE_TICK * 1.25 );
		auth_update     ( );			/* Gorog */
		time_update	( );
		weather_update	( );
		char_update	( );
		obj_update	( );
		clear_vrooms	( );			/* remove virtual rooms */
    }

    if ( --pulse_second   <= 0 )
    {
		pulse_second	= PULSE_PER_SECOND;
		char_check( );
		/*reboot_check( "" ); Disabled to check if its lagging a lot - Scryn*/
		/* Much faster version enabled by Altrag..
		although I dunno how it could lag too much, it was just a bunch
		of comparisons.. */
		reboot_check(0);
    }

    if ( auction->item && --auction->pulse <= 0 )
    {                                                  
		auction->pulse = PULSE_AUCTION;                     
		auction_update( );
    }

    aggr_update( );
    obj_act_update ( );
    room_act_update( );
    clean_obj_queue();		/* dispose of extracted objects */
    clean_char_queue();		/* dispose of dead mobs/quitting chars */
    if ( timechar )
    {
		gettimeofday(&etime, NULL);
		set_char_color(AT_PLAIN, timechar);
		send_to_char( "Update timing complete.\n\r", timechar );
		subtract_times(&etime, &stime);
		ch_printf( timechar, "Timing took %d.%06d seconds.\n\r",
			etime.tv_sec, etime.tv_usec );
		timechar = NULL;
    }
    tail_chain( );
    return;
}

void reboot_check( time_t reset )
{
  /*
  static char *tmsg[] =
  { "SYSTEM: Reboot in 10 seconds.",
    "SYSTEM: Reboot in 30 seconds.",
    "SYSTEM: Reboot in 1 minute.",
    "SYSTEM: Reboot in 2 minutes.",
    "SYSTEM: Reboot in 3 minutes.",
    "SYSTEM: Reboot in 4 minutes.",
    "SYSTEM: Reboot in 5 minutes.",
    "SYSTEM: Reboot in 10 minutes.",
  };
  */
  static char *tmsg[] =
  { "&G&W[&R^zSystem Announcement&G^x&G&W]&G&W Reboot in 10 seconds.",
    "&G&W[&R^zSystem Announcement&G^x&G&W]&G&W Reboot in 30 seconds.",
    "&G&W[&R^zSystem Announcement&G^x&G&W]&G&W Reboot in 1 minute.",
    "&G&W[&R^zSystem Announcement&G^x&G&W]&G&W Reboot in 2 minutes.",
    "&G&W[&R^zSystem Announcement&G^x&G&W]&G&W Reboot in 3 minutes.",
    "&G&W[&R^zSystem Announcement&G^x&G&W]&G&W Reboot in 4 minutes.",
    "&G&W[&R^zSystem Announcement&G^x&G&W]&G&W Reboot in 5 minutes.",
    "&G&W[&R^zSystem Announcement&G^x&G&W]&G&W Reboot in 10 minutes.",
  };
  static const int times[] = { 10, 30, 60, 120, 180, 240, 300, 600 };
  static const int timesize =
      UMIN(sizeof(times)/sizeof(*times), sizeof(tmsg)/sizeof(*tmsg));
  char buf[MAX_STRING_LENGTH];
  static int trun;
  static bool init;
  
  if ( !init || reset >= current_time )
  {
    for ( trun = timesize-1; trun >= 0; trun-- )
      if ( reset >= current_time+times[trun] )
        break;
    init = TRUE;
    return;
  }
  
	if ( (current_time % 1800) == 0 )
	{
		sprintf(buf, "%.24s: %d players", ctime(&current_time), num_descriptors);
		append_to_file(USAGE_FILE, buf);
    }
  
  if ( new_boot_time_t - boot_time < 60*60*18 &&
      !set_boot_time->manual )
    return;
  
  if ( new_boot_time_t <= current_time )
  {
    CHAR_DATA *vch;
    extern bool mud_down;
    
    if ( auction->item )
    {
      sprintf(buf, "Sale of %s has been stopped by mud.",
          auction->item->short_descr);
      talk_auction(buf);
      obj_to_char(auction->item, auction->seller);
      auction->item = NULL;
      if ( auction->buyer && auction->buyer != auction->seller )
      {
        auction->buyer->gold += auction->bet;
        send_to_char("Your money has been returned.\n\r", auction->buyer);
      }
    }
    echo_to_all(AT_YELLOW, "You are forced from these realms by a strong "
        "presence\n\ras life here is reconstructed.", ECHOTAR_ALL);
    
    for ( vch = first_char; vch; vch = vch->next )
      if ( !IS_NPC(vch) )
        save_char_obj(vch);
    mud_down = TRUE;
    return;
  }
  
  if ( trun != -1 && new_boot_time_t - current_time <= times[trun] ) {
    echo_to_all(AT_YELLOW, tmsg[trun], ECHOTAR_ALL);
    if ( trun <= 5 )
      sysdata.DENY_NEW_PLAYERS = TRUE;
    --trun;
    return;
  }
  return;
}
  

/* the auction update*/

void auction_update (void)
{
    int tax, pay;
    char buf[MAX_STRING_LENGTH];

	if(!auction->item)
    {
    	if(AUCTION_MEM > 0 && auction->history[0] &&
    			++auction->hist_timer == 6*AUCTION_MEM)
    	{
    		int i;
    		
    		for(i = AUCTION_MEM - 1; i >= 0; i--)
    		{
    			if(auction->history[i])
    			{
    				auction->history[i] = NULL;
    				auction->hist_timer = 0;
    				break;
    			}
    		}
    	}
    	return;
    }

    switch (++auction->going) /* increase the going state */
    {
	case 1 : /* going once */
	case 2 : /* going twice */
	    if (auction->bet > auction->starting)
		sprintf (buf, "%s: going %s for %d.", auction->item->short_descr,
			((auction->going == 1) ? "once" : "twice"), auction->bet);
	    else
		sprintf (buf, "%s: going %s (bid not received yet).", auction->item->short_descr,
			((auction->going == 1) ? "once" : "twice"));

	    talk_auction (buf);
	    break;

	case 3 : /* SOLD! */
	    if (!auction->buyer && auction->bet)
	    {
		bug( "Auction code reached SOLD, with NULL buyer, but %d gold bid", auction->bet );
		auction->bet = 0;
	    }
	    if (auction->bet > 0 && auction->buyer != auction->seller)
	    {
		sprintf (buf, "%s sold to %s for %s.",
			auction->item->short_descr,
			IS_NPC(auction->buyer) ? auction->buyer->short_descr : auction->buyer->name,
			num_punct(auction->bet) );
		talk_auction(buf);

		act(AT_ACTION, "The auctioneer materializes before you, and hands you $p.",
			auction->buyer, auction->item, NULL, TO_CHAR);
		act(AT_ACTION, "The auctioneer materializes before $n, and hands $m $p.",
			auction->buyer, auction->item, NULL, TO_ROOM);

		if ( (auction->buyer->carry_weight 
		+     get_obj_weight( auction->item ))
		>     can_carry_w( auction->buyer ) )
		{
		    act( AT_PLAIN, "$p is too heavy for you to carry with your current inventory.", auction->buyer, auction->item, NULL, TO_CHAR );
    		    act( AT_PLAIN, "$n is carrying too much to also carry $p, and $e drops it.", auction->buyer, auction->item, NULL, TO_ROOM );
		    obj_to_room( auction->item, auction->buyer->in_room );
		}
		else
		    obj_to_char( auction->item, auction->buyer );
	        pay = (int)auction->bet * 0.9;
		tax = (int)auction->bet * 0.1;
		boost_economy( auction->seller->in_room->area, tax );
                auction->seller->gold += pay; /* give him the money, tax 10 % */
                ch_printf( auction->seller, "The auctioneer pays you %s gold, charging an auction fee of",
                    num_punct(pay));
                ch_printf( auction->seller, "%s.\n\r", num_punct(tax));
/*		sprintf(buf, "The auctioneer pays you %s gold, charging an auction fee of %s.\n\r",
		  num_punct(pay),
		  num_punct(tax) );
		send_to_char(buf, auction->seller);*/
                auction->item = NULL; /* reset item */
		if ( IS_SET( sysdata.save_flags, SV_AUCTION ) )
		{
		    save_char_obj( auction->buyer );
		    save_char_obj( auction->seller );
		}
            }
            else /* not sold */
            {
                sprintf (buf, "No bids received for %s - object has been removed from auction\n\r.",auction->item->short_descr);
                talk_auction(buf);
                act (AT_ACTION, "The auctioneer appears before you to return $p to you.",
                      auction->seller,auction->item,NULL,TO_CHAR);
                act (AT_ACTION, "The auctioneer appears before $n to return $p to $m.",
                      auction->seller,auction->item,NULL,TO_ROOM);
		if ( (auction->seller->carry_weight
		+     get_obj_weight( auction->item ))
		>     can_carry_w( auction->seller ) )
		{
		    act( AT_PLAIN, "You drop $p as it is just too much to carry"
			" with everything else you're carrying.", auction->seller,
			auction->item, NULL, TO_CHAR );
		    act( AT_PLAIN, "$n drops $p as it is too much extra weight"
			" for $m with everything else.", auction->seller,
			auction->item, NULL, TO_ROOM );
		    obj_to_room( auction->item, auction->seller->in_room );
		}
		else
		    obj_to_char (auction->item,auction->seller);
		tax = (int)auction->item->cost * 0.05;
		boost_economy( auction->seller->in_room->area, tax );
		sprintf(buf, "The auctioneer charges you an auction fee of %s.\n\r", num_punct(tax) );
		send_to_char(buf, auction->seller);
		if ((auction->seller->gold - tax) < 0)
		  auction->seller->gold = 0;
		else
		  auction->seller->gold -= tax;
		if ( IS_SET( sysdata.save_flags, SV_AUCTION ) )
		    save_char_obj( auction->seller );
	    } /* else */
	    auction->item = NULL; /* clear auction */
    } /* switch */
} /* func */

void subtract_times(struct timeval *etime, struct timeval *stime)
{
  etime->tv_sec -= stime->tv_sec;
  etime->tv_usec -= stime->tv_usec;
  while ( etime->tv_usec < 0 )
  {
    etime->tv_usec += 1000000;
    etime->tv_sec--;
  }
  return;
}

void update_birthday(void)
{
	/*char buf[MAX_STRING_LENGTH];*/
	DESCRIPTOR_DATA *d;
	int ch_age;

	for( d = first_descriptor; d; d = d->next )
	{
		if ( d->connected != CON_PLAYING || IS_NPC(d->character ) )
			return;
/*		if ( IS_SET(d->character->act2,EXTRA_PREGNANT) && check_preg(d->character) >= 1 )
			sickness(d->character);*/
		ch_age = get_age(d->character);
		if ( d->character->pcdata->oldage != ch_age && ch_age >= 0 )
		{
			if ( !IS_IMMORTAL(d->character) )
			{
				int weight,height;
				float age;
				/*sprintf(buf, "&Y(&RG&Calactic &RN&Cetwork&Y)&C Announcer: '%s is %d today! Happy Birthday %s!'\n\r",
					d->character->name, get_age(d->character), d->character->name );
				echo_to_all( AT_WHITE, buf, ECHOTAR_ALL );*/
				if ( ch_age <= 17 ) 
				{
					age = ch_age;
					age /= 17;
					height = number_range(race_table[d->character->race]->height *.9, race_table[d->character->race]->height *1.1);
					height *= age;
					weight = number_range(race_table[d->character->race]->weight *.9, race_table[d->character->race]->weight *1.1);
					weight *= age;
					d->character->weight = weight;
					d->character->height = height;
				}
			}
			d->character->pcdata->oldage = ch_age;
		}
	}
	return;
}

void update_wages( void )
{
    CLAN_DATA *clan;
    char                   buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	int day;

	day     = time_info.day + 1;
	if ( day % 7 !=  6 ) 
		return;

	for( d = first_descriptor; d; d = d->next )
	{
		if ( d->connected != CON_PLAYING || IS_NPC(d->character ) )
			return;
		if ( d->character->pcdata && d->character->pcdata->clan )
		{
			clan = d->character->pcdata->clan;
			clan->funds -= clan->ranks[d->character->pcdata->clan_rank].wage;
			if ( clan->funds > 0 ) 
			{
				d->character->pcdata->bank += clan->ranks[d->character->pcdata->clan_rank].wage;
				sprintf(buf, "&W[&RBank Info&W] &BYour paycheck of %d has been placed into your account.\n\r",
					clan->ranks[d->character->pcdata->clan_rank].wage );
				send_to_char(buf,d->character);
			}
			else 
			{
				clan->funds += clan->ranks[d->character->pcdata->clan_rank].wage;
				sprintf(buf, "&W[&RBank Info&W] &BSorry %s, Your clan does not have enough money to pay you.\n\r",d->character->name);
				send_to_char(buf,d->character);
			}
		}
	}
	return;    
}

void echo_hint( void )
{
    char                   buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	int num;

	num = number_range(0,9);
	switch (num)
	{
	default:
		sprintf(buf, "HINT: This Space For Rent.\n\r");
		break;
	case 9:
	case 8:
	case 7:
	case 6:
	case 5:
	case 4:
	case 3:
	case 2:
		sprintf(buf, "HINT: When roleplaying, please stick to your bio \n\rand not suddenly change how you character acts and feels.\n\rThis will prevent the other characters getting confused.\n\r");
		break;
	case 1:
		sprintf(buf, "HINT: A Bio and description will help you get authed faster.\n\r");
		break;
	case 0:
		sprintf(buf, "HINT: If you have been requeted to change your name, you can use the command \"name\" without haveing to re-create.\n\r");
		break;
	}

	for( d = first_descriptor; d; d = d->next )
	{
		if (d->connected == CON_PLAYING &&
			!IS_NPC(d->character) && IS_AWAKE(d->character) && (d->character->top_level <= 5
))
		{
			set_char_color( AT_WHITE, d->character );
			send_to_char(buf,d->character);
		}
	}
	return;    
}

SHIP_DATA * make_mob_ship( SPACE_DATA *system , int model )
{
    SHIP_DATA *ship;
    int shipreg = 0;
    char filename[10];
    char shipname[MAX_STRING_LENGTH];
    
    if ( !system || !system->governed_by)
       return NULL;
       
    /* mobships are given filenames < 0 and are not saved */   
       
    for ( ship = first_ship ; ship ; ship = ship->next )
        if ( shipreg > atoi( ship->filename ) )
             shipreg = atoi( ship->filename );

    shipreg--;
    sprintf( filename , "%d" , shipreg );
    
    CREATE( ship, SHIP_DATA, 1 );
    LINK( ship, first_ship, last_ship, next, prev );
    
    ship->filename = str_dup( filename ) ;
    
    ship->next_in_starsystem = NULL;
    ship->prev_in_starsystem =NULL;
    ship->next_in_room = NULL;
    ship->prev_in_room = NULL;
    ship->in_room = NULL;
    ship->starsystem = NULL;
    ship->home = STRALLOC( system->name );   
    ship->owner = STRALLOC( system->governed_by->name );
    ship->pilot = STRALLOC("");
    ship->copilot = STRALLOC("");
    ship->dest = NULL;
    SET_BIT(ship->flags,SHIP_MOB);
    ship->class = 0;
    ship->hyperspeed = 0;
    ship->missilestate = MISSILE_READY;
    ship->tractorbeam = 2;
    ship->hatchopen = FALSE;
    ship->autotrack = FALSE;
    ship->autospeed = FALSE;
    ship->location = 0;
    ship->lastdoc = 0;
    ship->shipyard = 0;
    ship->collision = 0;
    ship->currjump = NULL;
    ship->chaff = 0;
    ship->maxchaff = 0;
    ship->chaff_released = FALSE;
	ship->laserdamage = 1;
    ship->target0 = NULL;
    ship->target1 = NULL;
    ship->target2 = NULL;
	ship->model = model;
	ship->color	= STRALLOC( "Cyan" );

    switch( ship->model )
    {
    case MOB_BATTLESHIP: 
       ship->realspeed = 25;
       ship->maxmissiles = 50;
       ship->lasers = 10;
       ship->maxenergy = 30000;
       ship->maxshield = 1000;
       ship->maxhull = 30000;
       ship->manuever = 25;
       sprintf( shipname , "Battlecruiser m%d (%s)" , 0-shipreg , system->governed_by->name );
       break;

    case MOB_CRUISER: 
       ship->realspeed = 50;
       ship->maxmissiles = 30;
       ship->lasers = 8;
       ship->maxenergy = 15000;
       ship->maxshield = 350;
       ship->maxhull = 10000;
       ship->manuever = 50;
       sprintf( shipname , "Cruiser m%d (%s)" , 0-shipreg , system->governed_by->name );
       break;

    case MOB_DESTROYER:
       ship->realspeed = 100;
       ship->maxmissiles = 20;
       ship->lasers = 6;
       ship->maxenergy = 7500;
       ship->maxshield = 200;
       ship->maxhull = 2000;
       ship->manuever = 100;
       ship->hyperspeed = 100;
       sprintf( shipname , "Corvette m%d (%s)" , 0-shipreg , system->governed_by->name );
       break;

    default: /* fighter */
       ship->realspeed = 255;
       ship->maxmissiles = 0;
       ship->lasers = 2;
       ship->maxenergy = 2500;
       ship->maxshield = 0;
       ship->maxhull = 100;
       ship->manuever = 100;
       sprintf( shipname , "Patrol Starfighter m%d (%s)" , 0-shipreg , system->governed_by->name );
       break;
    }
    
    ship->name = STRALLOC( shipname );
    ship->hull = ship->maxhull;
    ship->missiles = ship->maxmissiles;
    ship->energy = ship->maxenergy;
    ship->shield = 0;

    ship_to_starsystem(ship, starsystem_from_name(ship->home) );  
    ship->vx = number_range( -2000 , 2000 );
    ship->vy = number_range( -2000 , 2000 );
    ship->vz = number_range( -2000 , 2000 );
    ship->shipstate = SHIP_READY;
    ship->autopilot = TRUE;
    ship->autorecharge = TRUE;
    ship->shield = ship->maxshield;
    
    return ship;
}

void auto_ship_update ( )
{
	SPACE_DATA * system;
	
	for ( system = first_starsystem; system; system = system->next )
	{	
		int numpatrols = 0;			
		int numdestroyers = 0;           
		int numbattleships = 0;           
		int numcruisers = 0;           
		int fleetsize = 0;
		SHIP_DATA * ship;

		if ( system->governed_by )				
			for ( ship = system->first_ship ; ship ; ship = ship->next_in_starsystem )             			   
			{
				if ( !str_cmp( ship->owner , system->governed_by->name ) && 
					IS_SET(ship->flags,SHIP_MOB) )			  
				{                  				
					if ( ship->model == MOB_DESTROYER )                     
						numdestroyers++;                  
					else if ( ship->model == MOB_CRUISER )                     
						numcruisers++;                  
					else if ( ship->model == MOB_BATTLESHIP )                     
						numbattleships++;                  
					else                      
						numpatrols++;               
				} 
			}
			
		fleetsize = 100*numbattleships + 25*numcruisers + 5*numdestroyers + numpatrols;                  
		
		if ( fleetsize + 100 < system->controls )              
			make_mob_ship( system , MOB_BATTLESHIP );           
		else if ( fleetsize + 25 < system->controls && numcruisers < 5 )              
			make_mob_ship( system , MOB_CRUISER );           
		else if ( fleetsize + 5 < system->controls && numdestroyers < 5 )              
			make_mob_ship( system , MOB_DESTROYER );           
		else  if ( fleetsize < system->controls && numpatrols < 5 )              
			make_mob_ship( system , MOB_PATROL );        
		
	}
}

void update_planet ( ) 
{
	CHAR_DATA * mob = NULL;
	MOB_INDEX_DATA *pMobIndex = NULL;
	OBJ_INDEX_DATA *pObjIndex = NULL;
	ROOM_INDEX_DATA *pRoomIndex;
	AREA_DATA *pArea;
	int vnum;

	for ( pArea = first_area; pArea; pArea = pArea->next )
	{   
		for ( vnum = pArea->low_r_vnum; vnum <= pArea->hi_r_vnum; vnum++ )	
		{    
			if ( (pRoomIndex = get_room_index( vnum )) == NULL )
				continue;
			if ( !pRoomIndex->area->planet )
				continue;
			if ( xIS_SET( pRoomIndex->room_flags, ROOM_BARRACKS ) 
				&& pRoomIndex->area && pRoomIndex->area->planet)
			{
				int guard_count = 0;
				OBJ_DATA * blaster;
				GUARD_DATA * guard;
				char tmpbuf[MAX_STRING_LENGTH];
				
				if ( !(pMobIndex = get_mob_index(52)) )
				{
					bug( "Reset_all: Missing default patrol (52)(%d)", vnum );
					return;
				}
				
				for ( guard = pRoomIndex->area->planet->first_guard ; guard ; guard = guard->next_on_planet )
					guard_count++;
                
				if ( pRoomIndex->area->planet->barracks*5 <= guard_count ) 
					continue;
                
				mob = create_mobile( pMobIndex );
				char_to_room( mob, pRoomIndex );
				mob->top_level = 10;
				mob->hit = 100;
				mob->max_hit = 100;
				mob->armor = 50;
				mob->damroll = 0;
				mob->hitroll = 20;
				if ( ( pObjIndex = get_obj_index( 54 ) ) != NULL )
				{
					blaster = create_object( pObjIndex, mob->top_level );
					obj_to_char( blaster, mob );
					equip_char( mob, blaster, WEAR_WIELD );                        
				} 
				do_setblaster( mob , "full" );

				CREATE( guard , GUARD_DATA , 1 );
                
				guard->planet = pRoomIndex->area->planet;
				LINK( guard , guard->planet->first_guard, guard->planet->last_guard, next_on_planet, prev_on_planet );
				LINK( guard , first_guard, last_guard, next, prev );
				mob->guard_data = guard;           
				if ( room_is_dark(pRoomIndex) )
					SET_BIT(mob->affected_by, AFF_INFRARED);
				if ( pRoomIndex->area->planet->governed_by )
				{
					sprintf( tmpbuf , "A soldier patrols the area. (%s)\n\r" , pRoomIndex->area->planet->governed_by->name );
					STRFREE( mob->long_descr );
					mob->long_descr = STRALLOC( tmpbuf );
					mob->mob_clan  = pRoomIndex->area->planet->governed_by;
				}			
				continue; 
			}
		}
	}
}


/*
 * Function to update weather vectors according to climate
 * settings, random effects, and neighboring areas.
 * Last modified: July 18, 1997
 * - Fireblade
 */
void adjust_vectors(WEATHER_DATA *weather)
{
	NEIGHBOR_DATA *neigh;
	double dT, dP, dW;

	if(!weather)
	{
		bug("adjust_vectors: NULL weather data.", 0);
		return;
	}

	dT = 0;
	dP = 0;
	dW = 0;

	/* Add in random effects */
	dT += number_range(-rand_factor, rand_factor);
	dP += number_range(-rand_factor, rand_factor);
	dW += number_range(-rand_factor, rand_factor);
	
	/* Add in climate effects*/
	dT += climate_factor *
		(((weather->climate_temp - 2)*weath_unit) -
		(weather->temp))/weath_unit;
	dP += climate_factor *
		(((weather->climate_precip - 2)*weath_unit) -
		(weather->precip))/weath_unit;
	dW += climate_factor *
		(((weather->climate_wind - 2)*weath_unit) -
		(weather->wind))/weath_unit;
	
		
	/* Add in effects from neighboring areas */
	for(neigh = weather->first_neighbor; neigh;
			neigh = neigh->next)
	{
		/* see if we have the area cache'd already */
		if(!neigh->address)
		{
			/* try and find address for area */
			neigh->address = get_area(neigh->name);
			
			/* if couldn't find area ditch the neigh */
			if(!neigh->address)
			{
				NEIGHBOR_DATA *temp;
				bug("adjust_weather: "
					"invalid area name.", 0);
				temp = neigh->prev;
				UNLINK(neigh,
				       weather->first_neighbor,
				       weather->last_neighbor,
				       next,
				       prev);
				STRFREE(neigh->name);
				DISPOSE(neigh);
				neigh = temp;
				continue;
			}
		}
		
		dT +=(neigh->address->weather->temp -
		      weather->temp) / neigh_factor;
		dP +=(neigh->address->weather->precip -
		      weather->precip) / neigh_factor;
		dW +=(neigh->address->weather->wind -
		      weather->wind) / neigh_factor;
	}
	
	/* now apply the effects to the vectors */
	weather->temp_vector += (int)dT;
	weather->precip_vector += (int)dP;
	weather->wind_vector += (int)dW;

	/* Make sure they are within the right range */
	weather->temp_vector = URANGE(-max_vector,
		weather->temp_vector, max_vector);
	weather->precip_vector = URANGE(-max_vector,
		weather->precip_vector, max_vector);
	weather->wind_vector = URANGE(-max_vector,
		weather->wind_vector, max_vector);
	
	return;
}

/*
 * function updates weather for each area
 * Last Modified: July 31, 1997
 * Fireblade
 */
void weather_update()
{
	AREA_DATA *pArea;
	DESCRIPTOR_DATA *d;
	int limit;
	
	limit = 3 * weath_unit;
	
	for(pArea = first_area; pArea;
		pArea = (pArea == last_area) ? first_build : pArea->next)
	{
		/* Apply vectors to fields */
		pArea->weather->temp +=
			pArea->weather->temp_vector;
		pArea->weather->precip +=
			pArea->weather->precip_vector;
		pArea->weather->wind +=
			pArea->weather->wind_vector;
		
		/* Make sure they are within the proper range */
		pArea->weather->temp = URANGE(-limit,
			pArea->weather->temp, limit);
		pArea->weather->precip = URANGE(-limit,
			pArea->weather->precip, limit);
		pArea->weather->wind = URANGE(-limit,
			pArea->weather->wind, limit);
		
		/* get an appropriate echo for the area */
		get_weather_echo(pArea->weather);
	}

	for(pArea = first_area; pArea;
		pArea = (pArea == last_area) ? first_build : pArea->next)
	{
		adjust_vectors(pArea->weather);
	}
	
	/* display the echo strings to the appropriate players */
	for(d = first_descriptor; d; d = d->next)
	{
		WEATHER_DATA *weath;
		
		if(d->connected == CON_PLAYING &&
			IS_OUTSIDE(d->character) &&
			!NO_WEATHER_SECT(d->character->in_room->sector_type) &&
			IS_AWAKE(d->character))
		{
			weath = d->character->in_room->area->weather;
			if(!weath->echo)
				continue;
			set_char_color(weath->echo_color, d->character);
			ch_printf(d->character, weath->echo);
		}
	}
	
	return;
}

/*
 * get weather echo messages according to area weather...
 * stores echo message in weath_data.... must be called before
 * the vectors are adjusted
 * Last Modified: August 10, 1997
 * Fireblade
 */
void get_weather_echo(WEATHER_DATA *weath)
{
	int n;
	int temp, precip, wind;
	int dT, dP, dW;
	int tindex, pindex, windex;
	
	/* set echo to be nothing */
	weath->echo = NULL;
	weath->echo_color = AT_GREY;
	
	/* get the random number */
	n = number_bits(2);
	
	/* variables for convenience */
	temp = weath->temp;
	precip = weath->precip;
	wind = weath->wind;

	dT = weath->temp_vector;
	dP = weath->precip_vector;
	dW = weath->wind_vector;
	
	tindex = (temp + 3*weath_unit - 1)/weath_unit;
	pindex = (precip + 3*weath_unit - 1)/weath_unit;
	windex = (wind + 3*weath_unit - 1)/weath_unit;
	
	/* get the echo string... mainly based on precip */
	switch(pindex)
	{
		case 0:
			if(precip - dP > -2*weath_unit)
			{
				char *echo_strings[4] =
				{
					"The clouds disappear.\n\r",
					"The clouds disappear.\n\r",
					"The sky begins to break through "
						"the clouds.\n\r",
					"The clouds are slowly "
						"evaporating.\n\r"
				};
				
				weath->echo = echo_strings[n];
				weath->echo_color = AT_WHITE;
			}
			break;

		case 1:
			if(precip - dP <= -2*weath_unit)
			{
				char *echo_strings[4] =
				{
					"The sky is getting cloudy.\n\r",
					"The sky is getting cloudy.\n\r",
					"Light clouds cast a haze over "
						"the sky.\n\r",
					"Billows of clouds spread through "
						"the sky.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_GREY;
			}
			break;
			
		case 2:
			if(precip - dP > 0)
			{
				if(tindex > 1)
				{
					char *echo_strings[4] =
					{
						"The rain stops.\n\r",
						"The rain stops.\n\r",
						"The rainstorm tapers "
							"off.\n\r",
						"The rain's intensity "
							"breaks.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_CYAN;
				}
				else
				{
					char *echo_strings[4] =
					{
						"The snow stops.\n\r",
						"The snow stops.\n\r",
						"The snow showers taper "
							"off.\n\r",
						"The snow flakes disappear "
							"from the sky.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_WHITE;
				}
			}
			break;
			
		case 3:
			if(precip - dP <= 0)
			{
				if(tindex > 1)
				{
					char *echo_strings[4] =
					{
						"It starts to rain.\n\r",
						"It starts to rain.\n\r",
						"A droplet of rain falls "
							"upon you.\n\r",
						"The rain begins to "
							"patter.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_CYAN;
				}
				else
				{
					char *echo_strings[4] =
					{
						"It starts to snow.\n\r",
						"It starts to snow.\n\r",
						"Crystal flakes begin to "
							"fall from the "
							"sky.\n\r",
						"Snow flakes drift down "
							"from the clouds.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_WHITE;
				}
			}
			else if(tindex < 2 && temp - dT > -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The temperature drops and the rain "
						"becomes a light snow.\n\r",
					"The temperature drops and the rain "
						"becomes a light snow.\n\r",
					"Flurries form as the rain freezes.\n\r",
					"Large snow flakes begin to fall "
						"with the rain.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_WHITE;
			}
			else if(tindex > 1 && temp - dT <= -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The snow flurries are gradually "
						"replaced by pockets of rain.\n\r",
					"The snow flurries are gradually "
						"replaced by pockets of rain.\n\r",
					"The falling snow turns to a cold drizzle.\n\r",
					"The snow turns to rain as the air warms.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_CYAN;
			} 
			break;
		
		case 4:
			if(precip - dP > 2*weath_unit)
			{
				if(tindex > 1)
				{
					char *echo_strings[4] =
					{
						"The lightning has stopped.\n\r",
						"The lightning has stopped.\n\r",
						"The sky settles, and the "
							"thunder surrenders.\n\r",
						"The lightning bursts fade as "
							"the storm weakens.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_GREY;
				}
			}
			else if(tindex < 2 && temp - dT > -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The cold rain turns to snow.\n\r",
					"The cold rain turns to snow.\n\r",
					"Snow flakes begin to fall "
						"amidst the rain.\n\r",
					"The driving rain begins to freeze.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_WHITE;
			}
			else if(tindex > 1 && temp - dT <= -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The snow becomes a freezing rain.\n\r",
					"The snow becomes a freezing rain.\n\r",
					"A cold rain beats down on you "
						"as the snow begins to melt.\n\r",
					"The snow is slowly replaced by a heavy "
						"rain.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_CYAN;
			}
			break;
			
		case 5:
			if(precip - dP <= 2*weath_unit)
			{
				if(tindex > 1)
				{
					char *echo_strings[4] =
					{
						"Lightning flashes in the "
							"sky.\n\r",
						"Lightning flashes in the "
							"sky.\n\r",
						"A flash of lightning splits "
							"the sky.\n\r",
						"The sky flashes, and the "
							"ground trembles with "
							"thunder.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_YELLOW;
				}
			}
			else if(tindex > 1 && temp - dT <= -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The sky rumbles with thunder as "
						"the snow changes to rain.\n\r",
					"The sky rumbles with thunder as "
						"the snow changes to rain.\n\r",
					"The falling turns to freezing rain "
						"amidst flashes of "
						"lightning.\n\r",
					"The falling snow begins to melt as "
						"thunder crashes overhead.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_WHITE;
			}
			else if(tindex < 2 && temp - dT > -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The lightning stops as the rainstorm "
						"becomes a blinding "
						"blizzard.\n\r",
					"The lightning stops as the rainstorm "
						"becomes a blinding "
						"blizzard.\n\r",
					"The thunder dies off as the "
						"pounding rain turns to "
						"heavy snow.\n\r",
					"The cold rain turns to snow and "
						"the lightning stops.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_CYAN;
			}
			break;
			
		default:
			bug("echo_weather: invalid precip index");
			weath->precip = 0;
			break;
	}

	return;
}

/*
 * get echo messages according to time changes...
 * some echoes depend upon the weather so an echo must be
 * found for each area
 * Last Modified: August 10, 1997
 * Fireblade
 */
void get_time_echo(WEATHER_DATA *weath)
{
	int n;
	int pindex;
	
	n = number_bits(2);
	pindex = (weath->precip + 3*weath_unit - 1)/weath_unit;
	weath->echo = NULL;
	weath->echo_color = AT_GREY;
	
	switch(time_info.hour)
	{
		case 5:
		{
			char *echo_strings[4] =
			{
				"The day has begun.\n\r",
				"The day has begun.\n\r",
				"The sky slowly begins to glow.\n\r",
				"The sun slowly embarks upon a new day.\n\r"
			};
			time_info.sunlight = SUN_RISE;
			weath->echo = echo_strings[n];
			weath->echo_color = AT_YELLOW;
			break;
		}
		case 6:
		{
			char *echo_strings[4] =
			{
				"The sun rises in the east.\n\r",
				"The sun rises in the east.\n\r",
				"The hazy sun rises over the horizon.\n\r",
				"Day breaks as the sun lifts into the sky.\n\r"
			};
			time_info.sunlight = SUN_LIGHT;
			weath->echo = echo_strings[n];
			weath->echo_color = AT_ORANGE;
			break;
		}
		case 12:
		{
			if(pindex > 0)
			{
				weath->echo = "It's noon.\n\r";
			}
			else
			{
				char *echo_strings[2] =
				{
					"The intensity of the sun "
						"heralds the noon hour.\n\r",
					"The sun's bright rays beat down "
						"upon your shoulders.\n\r"
				};
				weath->echo = echo_strings[n%2];
			}
			time_info.sunlight = SUN_LIGHT;
			weath->echo_color = AT_WHITE;
			break;
		}
		case 19:
		{
			char *echo_strings[4] =
			{
				"The sun slowly disappears in the west.\n\r",
				"The reddish sun sets past the horizon.\n\r",
				"The sky turns a reddish orange as the sun "
					"ends its journey.\n\r",
				"The sun's radiance dims as it sinks in the "
					"sky.\n\r"
			};
			time_info.sunlight = SUN_SET;
			weath->echo = echo_strings[n];
			weath->echo_color = AT_RED;
			break;
		}
		case 20:
		{
			if(pindex > 0)
			{
				char *echo_strings[2] =
				{
					"The night begins.\n\r",
					"Twilight descends around you.\n\r"
				};
				weath->echo = echo_strings[n%2];
			}
			else
			{
				char *echo_strings[2] =
				{
					"The moon's gentle glow diffuses "
						"through the night sky.\n\r",
					"The night sky gleams with "
						"glittering starlight.\n\r"
				};
				weath->echo = echo_strings[n%2];
			}
			time_info.sunlight = SUN_DARK;
			weath->echo_color = AT_DBLUE;
			break;
		}
	}
	
	return;
}

/*
 * update the time
 */
void time_update()
{
	AREA_DATA *pArea;
	DESCRIPTOR_DATA *d;
	WEATHER_DATA *weath;
	
	webwho();

	if ( number_range(1,100) >= 85 )
	{
		for( d = first_descriptor; d; d = d->next )
		{
			if ( d->connected != CON_PLAYING || IS_NPC(d->character ) )
				return;
			if ( IS_SET(d->character->act2,EXTRA_PREGNANT) && check_preg(d->character) >= 1 )
				sickness(d->character);
		}
	}
	switch(++time_info.hour)
	{
	case 5:
	case 6:
	case 12:
	case 19:
	case 20:
		for(pArea = first_area; pArea;
		pArea = (pArea == last_area) ? first_build : pArea->next)
		{
			get_time_echo(pArea->weather);
		}
		
		for(d = first_descriptor; d; d = d->next)
		{
			if(d->connected == CON_PLAYING && IS_OUTSIDE(d->character) && IS_AWAKE(d->character))
			{
				weath = d->character->in_room->area->weather;
				if(!weath->echo)
					continue;
				set_char_color(weath->echo_color, d->character);
				ch_printf(d->character, weath->echo);
			}
		}
		break;
	case 24:
		time_info.hour = 0;
		time_info.day++;
		{
			PLANET_DATA *planet;
			SPACE_DATA *starsystem;
			for ( starsystem = first_starsystem; starsystem; starsystem = starsystem->next )
			{
				starsystem->controls = 0; /*what git added this!!! ;-) */ /*I did, otherwise too many ships appear - Gavin */
				for ( planet = first_planet; planet; planet = planet->next )
				{
					if ( planet->starsystem && planet->starsystem == starsystem )
						starsystem->controls += planet->controls;
				}
			}
		}
		update_birthday(); /* Voltecs player Birthdays */
		update_wages( );
		echo_hint( );
		break;
	}
	
	if(time_info.day >= 30)
	{
		time_info.day = 0;
		time_info.month++;
	}
	
	if(time_info.month >= 12)
	{
		time_info.month = 0;
		time_info.year++;
		bug("time_update: time_info.year = %d",time_info.year,0);
	}
	
	return;
}

void dream( CHAR_DATA *ch )
{
	if ( IS_NPC(ch) )
		return;
	
/*	if (get_age(ch) <= 15)*/
	{
		switch(number_range(0,5))
		{
		case 0:
			ch_printf(ch,"The target grows closer through the scopes, only a few more seconds and\n\r");
			ch_printf(ch,"it will all be over... The proximity sensors beep urgently as the TIE\n\r");
			ch_printf(ch,"fighters close from behind, but you stay on target. They fire lasers but\n\r");
			ch_printf(ch,"miss, and you press the firing stud before they can attempt another\n\r");
			ch_printf(ch,"volley. The torpedoes lance away from the fighter, straight into the\n\r");
			ch_printf(ch,"exhaust port. You pull up and away as the Death Star explodes in a shower\n\r");
			ch_printf(ch,"of destruction.\n\r");
			break;
		case 1:
			ch_printf(ch,"A Rodian is speaking to you, but no sound emits from its proboscis. You\n\r");
			ch_printf(ch,"answer it and you follow him down a dirty street. Half familiar creatures\n\r");
			ch_printf(ch,"pass by, gazing solemnly at you. Suddenly you are on a ship, looking out\n\r");
			ch_printf(ch,"a viewport as a giant space battle takes place. The Rodian stands nearby,\n\r");
			ch_printf(ch,"still silent as a star cruiser explodes in a shower of sparks. The floor\n\r");
			ch_printf(ch,"rumbles and girders fall from the ceiling as you realize that this ship\n\r");
			ch_printf(ch,"will be the next victim. Tremendous green light fills your eyes as the\n\r");
			ch_printf(ch,"ship comes apart, and then you are in free space, floating over a serene\n\r");
			ch_printf(ch,"planet. The planet moves from view and one by one the stars blink out of\n\r");
			ch_printf(ch,"existence, leaving complete darkness...\n\r");
			break;
		case 2:
			ch_printf(ch,"As twilight descends, a purple bantha floats in its bubble to the silver\n\r");
			ch_printf(ch,"kingdom of progress to do battle with the pink samurai. A Gamorrean\n\r");
			ch_printf(ch,"dances across an emerald pillow of mushrooming smoke and the three Kaufs\n\r");
			ch_printf(ch,"stand in the radioactive grass of commerce. \"The north glass is more\n\r");
			ch_printf(ch,"comfortable than the mustached morrt,\" intones the Emperor of Polished\n\r");
			ch_printf(ch,"Nebulas. A houjix swims in a river of ruby cognac and the golden yubnut\n\r");
			ch_printf(ch,"sets over a field of billowing plasteel. The vortex pulls at your toes\n\r");
			ch_printf(ch,"and all goes dark again.\n\r");
			break;
		case 3:
			ch_printf(ch,"A dark man growls at you with great big teeth, chasing you from your\n\r");
			ch_printf(ch,"bedroom down the hall. You try to run into your parent's room, but the\n\r");
			ch_printf(ch,"door won't open and the dark man is about to get you. You open your mouth\n\r");
			ch_printf(ch,"to scream but you can't, and the dark man grabs you, opening his mouth to\n\r");
			ch_printf(ch,"eat you piece by piece. Suddenly the dark man is gone.\n\r");
			break;
		case 4:
			ch_printf(ch,"A krayt dragon paws at the window with his giant claws, trying to break\n\r");
			ch_printf(ch,"in. You scream in terror, jumping down to hide under the bed. The dragon\n\r");
			ch_printf(ch,"crashes through the wall and begins to search for you. You can hear its\n\r");
			ch_printf(ch,"rancid breath as it pokes its nose to the bed, trying to find out where\n\r");
			ch_printf(ch,"you are. You can see its terrible clawed feet as it moves around the room\n\r");
			ch_printf(ch,"growling.  Its tail swipes back and forth, cracking the glowpanel and\n\r");
			ch_printf(ch,"sending everything into darkness. It bellows loudly, sending an\n\r");
			ch_printf(ch,"ear-piercing shriek through the blackness. Then there is nothing.\n\r");
			break;
		case 5:
			ch_printf(ch,"A whirring metal drill slowly descends from above, mechanically vectoring\n\r");
			ch_printf(ch,"for your head. You try to move but can't, and with excruciating horror\n\r");
			ch_printf(ch,"the drill painfully enters your skull. You feel moisture running down\n\r");
			ch_printf(ch,"your forehead, and you realize it is your own blood. At the same time, a\n\r");
			ch_printf(ch,"strange feeling covers your body, as if a thousand tiny legs are brushing\n\r");
			ch_printf(ch,"against you. As the blood clouds your eyes you see a horde of tiny\n\r");
			ch_printf(ch,"arachnids skittering up your chest. You feel a hundred tiny bites all\n\r");
			ch_printf(ch,"over and then numbness...\n\r");
			break;
		}
	}
}


void sickness (CHAR_DATA *ch)
{
	AFFECT_DATA af;
	bool sick = FALSE;
	bool butterflies = FALSE;
	bool babymove = FALSE;
	int days = check_preg(ch);

	if ( days >= 1 && days <= 30 ) /* Month 1 */
	{
		butterflies = FALSE;
		sick = FALSE;
		babymove = FALSE;
	}
	else if ( days >= 31 && days <= 60  ) /* Month 2 */
	{
		if ( number_percent() <= 70 )
			sick = TRUE;
	}
	else if ( days >= 61 && days <= 90 ) /* Month 3 */
	{
		if ( number_percent() <= 30 )
			sick = TRUE;
		if ( number_percent() <= 30 )
			butterflies = TRUE;
	}
	else if ( days >= 91 && days <= 120 ) /* Month 4 */
	{
		/*if ( number_percent() <= 25)*/
			babymove = TRUE;
		/*mother->weight *= 1.1;*/
	}
	else if ( days >= 121 && days <= 150 ) /* Month 5 */ ;
	else if ( days >= 151 && days <= 180 ) /* Month 6 */ ;
	else if ( days >= 181 && days <= 210 ) /* Month 7 */ ;
	else if ( days >= 211 && days <= 240 ) /* Month 8 */ ;
	else if ( days >= 241 ) /* Month 9+ */
	{
		if ( days >= 280 && !IS_SET(ch->act2,EXTRA_LABOUR) )
		{
			SET_BIT(ch->act2, EXTRA_LABOUR);
			ch_printf(ch, "You feel your water break!\n\r");
		}
	}

	if ( sick == TRUE )
	{
		ch_printf(ch, "You feel sick to your stomach!\n\r");
		act( AT_DYING, "$n grasps her stomach and starts to throw up.", ch, NULL, NULL, TO_ROOM );
		af.type      = gsn_poison;
		af.duration  = 20;
		af.location  = APPLY_STR;
		af.modifier  = -2;
		af.bitvector = AFF_POISON;
		affect_join( ch, &af );
		ch->mental_state = URANGE( 20, ch->mental_state + 2, 100 );
		ch->emotional_state = URANGE( 20, ch->emotional_state + 2, 100 );
	}
	if ( butterflies == TRUE )
	{
		ch_printf(ch, "You feel like there are butterflies flickering around in your stomach\n\r");
		ch->mental_state = URANGE( 20, ch->mental_state + 2, 100 );
		ch->emotional_state = URANGE( 20, ch->emotional_state + 2, 100 );
	}
	if ( babymove == TRUE )
	{
		ch_printf( ch, "You feel a small kick against your stomache.\n\r");
		ch->mental_state = URANGE( 20, ch->mental_state + 2, 100 );
		ch->emotional_state = URANGE( 20, ch->emotional_state + 2, 100 );
	}
}

void webwho() 
{
	FILE *webwho = NULL;
	if ( ( webwho = fopen( WEBWHO_FILE, "w" ) ) != NULL )
	{
		DESCRIPTOR_DATA *d;
		char buf[MAX_STRING_LENGTH];
		char council_name[MAX_INPUT_LENGTH];
		char race_text[MAX_INPUT_LENGTH];
		WHO_DATA *cur_who = NULL;
		WHO_DATA *next_who = NULL;
		WHO_DATA *first_mortal = NULL;
		WHO_DATA *first_newbie = NULL;
		WHO_DATA *first_imm = NULL;
		int nMatch = 0;

		
		d = last_descriptor;
		for ( d = last_descriptor; d; d = d->prev )
		{
			CHAR_DATA *wch;
			char const *race;

			if ( (d->connected != CON_PLAYING && d->connected != CON_EDITING)
				|| ( IS_IMMORTAL( d->character) && IS_SET(d->character->act, PLR_WIZINVIS) )
				|| d->original)
				continue;
			wch   = d->original ? d->original : d->character;
			nMatch++;
			sprintf( race_text, "%s(%s) ", NOT_AUTHED(wch) ? "N" : "", race_table[wch->race]->race_name);
			race = race_text;
			if ( wch->pcdata->rank && wch->pcdata->rank[0] != '\0' )
				race = wch->pcdata->rank;
			if ( wch->pcdata->council )
			{
				strcpy( council_name, " &G&W[" );
				if (wch->pcdata->council->head && !str_cmp (wch->name, wch->pcdata->council->head))
					strcat (council_name, "Head of ");
				if (wch->pcdata->council->head2 && !str_cmp(wch->name, wch->pcdata->council->head2))
					strcat (council_name, "Co-Head of ");
				strcat( council_name, wch->pcdata->council_name );
				strcat( council_name, "&W]" );
			}	
			else
				council_name[0] = '\0';
			sprintf(buf, "&W%s %s %s\n\r",
				race,wch->pcdata->title,council_name);
			
			/* First make the structure. */
			CREATE( cur_who, WHO_DATA, 1 );
			cur_who->text = str_dup( buf );
			
			if ( IS_IMMORTAL( wch ) ) { cur_who->type = WT_IMM; }	
			else if ( get_trust( wch ) <= 5 ) { cur_who->type = WT_NEWBIE; }
			else { cur_who->type = WT_MORTAL; }
			
			/* Then put it into the appropriate list. */
			switch ( cur_who->type )
			{
			case WT_MORTAL:
				cur_who->next = first_mortal;
				first_mortal = cur_who;
				break;
			case WT_IMM:
				cur_who->next = first_imm;
				first_imm = cur_who;
				break;
			case WT_NEWBIE:
				cur_who->next = first_newbie;
				first_newbie = cur_who;
				break;
			}
		} /* END For loop for descriptors */

		if ( first_newbie )
			fprintf( webwho, "\n\r&P+&B-------------------------&W[ &PT&phe &PU&pn&P-&PI&pnformed &W]&B-------------------------&P+&W\n\r"
				"&P+&B---------------------------------------------------------------------&P+&W\n\r");
		for ( cur_who = first_newbie; cur_who; cur_who = next_who )
		{
			fprintf( webwho, cur_who->text );
			next_who = cur_who->next;
			DISPOSE( cur_who->text );
			DISPOSE( cur_who ); 
		} 
		if ( first_mortal )
			fprintf( webwho,"\n\r&P+&B-------------------------&W[ &PG&palactic &PM&pisfits &W]&B-------------------------&P+&W\n\r"
				"&P+&B----------------------------------------------------------------------&P+&W\n\r" );
		
		for ( cur_who = first_mortal; cur_who; cur_who = next_who )
		{
			fprintf( webwho, cur_who->text );
			next_who = cur_who->next;
			DISPOSE( cur_who->text );
			DISPOSE( cur_who ); 
		} 
		if ( first_imm )
			fprintf( webwho, "\n\r&P+&B-------------------------&W[ &PT&phe &PW&pell &PI&pnformed &PO&pnes &W]&B-------------------------&P+&W\n\r"
				"&P+&B----------------------------------------------------------------------------&P+&W\n\r" );
		for ( cur_who = first_imm; cur_who; cur_who = next_who )
		{
			fprintf( webwho, cur_who->text );
			next_who = cur_who->next;
			DISPOSE( cur_who->text );
			DISPOSE( cur_who ); 
		} 
		fprintf( webwho, "&Y%d player%s.\n\r", nMatch, nMatch == 1 ? "" : "s" );
		fclose( webwho);
		return;
	}
}

