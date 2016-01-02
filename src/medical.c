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
*                      Medical Player Skills                               *
****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mud.h"

#if defined(__FreeBSD__)
	#include <dirent.h>
#else
	#include <sys/dir.h>
#endif

int		check_preg		args( ( CHAR_DATA *ch ) );
void	birth_write		args( ( CHAR_DATA *ch, char *argument ) );
bool	birth_ok		args( ( CHAR_DATA *ch, char *argument ) );

char *  const	body_parts    [MAX_BODY_PARTS] =
{
	"left leg", "right leg", "left foot", "right foot", "left arm",
	"right arm", "left wrist", "right wrist", "left knee", "right knee",
	"left ankle", "right ankle", "left sholder", "right sholder", "left hand",
	"right hand", "nose", "ribs", "jaw", "stomach", "chest"
};

int get_bodypart( char *flag )
{
    int x;
    
	for ( x = 0; x < (sizeof(body_parts) / sizeof(body_parts[0])); x++ )
		if ( !str_cmp(flag, body_parts[x]) )
			return x;
	return -1;
}


void do_diagnose ( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	CHAR_DATA * victim;
	int chance = 0;

	if (IS_NPC(ch))
		return;

	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg2 );

	if ( arg[0] == '\0' )
	{
		ch_printf(ch, "&WDiagnose who?\n\r");
		return;
	}

	if ( ( victim = get_char_room(ch, arg) ) == NULL )
	{
		send_to_char("They are not here.\n\r",ch);
		return;
	}
	
	if (IS_NPC(victim))
	{
		send_to_char("Not on NPC's.\n\r",ch);
		return;
	}
	
/*	if (ch == victim)
	{
		send_to_char("Not on yourself!\n\r",ch);
		return;
	}*/
	if ( ch != victim )
		if ( !nifty_is_name( ch->name , victim->pcdata->partner) )
		{
			send_to_char("They have not given consent!\n\r",ch);
			return;
		}

	if ( arg2[0] == '\0')
	{
		send_to_char("Syntax:\n\r\tDiagnose [players name] [type of test]\n\r",ch);
		send_to_char("Types of tests are:\n\r",ch);
		send_to_char("\tPregnacy       Drugs        Bones\n\r",ch);
		return;
	}
	chance = IS_NPC(ch) ? ch->top_level : (int)  (ch->pcdata->learned[gsn_diagnose]) ;
	if ( !str_cmp("pregnacy",arg2) || !str_prefix("preg",arg2) )
	{
		int days = 0;

		act( AT_ACTION, "You go over to $N, take a quick blood sample, and quickly take it back to your medical bag.", ch, NULL, victim, TO_CHAR);
		act( AT_ACTION, "From your medical bag, you take out a small device and quickly insert $N's blood sample.",ch, NULL, victim, TO_CHAR);
		act( AT_ACTION, "$n go over to you, takes a quick blood sample, and quickly takes it back to $s medical bag.", ch, NULL, victim, TO_VICT);
		act( AT_ACTION, "From $n's medical bag, $e takes out a small device and quickly inserts your blood sample.",ch, NULL, victim, TO_VICT);
		act( AT_ACTION, "$n go over to $N, takes a quick blood sample, and quickly takes it back to $s medical bag.", ch, NULL, victim, TO_NOTVICT);
		act( AT_ACTION, "From $n's medical bag, $e takes out a small device and quickly inserts $N's blood sample.",ch, NULL, victim, TO_NOTVICT);

		if ( number_percent() < chance )
		{
			if (!IS_NPC(victim) && IS_SET(victim->act2, EXTRA_PREGNANT) ) days = check_preg(victim);
			if (days <= 0)
			{
				act( AT_INFO, "The machine dings, and a screen displaying that $N is not pregnant.",ch,NULL, victim, TO_CHAR);
				act( AT_INFO, "You quickly inform $N that $E is not pregnant.",ch,NULL, victim, TO_CHAR);
				act( AT_INFO, "The machine dings, and a screen appears showing information that you do not understand.",ch,NULL, victim, TO_ROOM);
				act( AT_INFO, "$n then comes back and informs you that you are not pregnant.",ch,NULL, victim, TO_VICT);
				act( AT_INFO, "$n quickly inform $N something.",ch,NULL, victim, TO_NOTVICT);
			}
			else
			{
				sprintf( buf, "%d", days);
				act( AT_INFO, "The machine dings, and a screen displaying that $N is $t days pregnant.",ch, buf, victim, TO_CHAR);
				act( AT_INFO, "You quickly inform $N that $E is $t days pregnant.",ch,buf, victim, TO_CHAR);
				act( AT_INFO, "The machine dings, and a screen appears showing information that you do not understand.",ch,NULL, victim, TO_ROOM);
				act( AT_INFO, "$n then comes back and informs you that you are $t days pregnant.",ch,buf, victim, TO_VICT);
				act( AT_INFO, "$n quickly inform $N something.",ch,NULL, victim, TO_NOTVICT);
			}
			if ( ch != victim )
				learn_from_success( ch, gsn_diagnose );
			return;
		}
		else
		{
			act( AT_INFO, "The machine dings, and a screen displaying some information.",ch,NULL, victim, TO_CHAR);
			act( AT_INFO, "But you are unable to read it.",ch,NULL, victim, TO_CHAR);
			act( AT_INFO, "The machine dings, and a screen appears showing information that you do not understand.",ch,NULL, victim, TO_ROOM);
			learn_from_failure( ch, gsn_diagnose );
			return;
		}
	}
	if ( !str_cmp("Drug",arg2) || !str_prefix("Drug",arg2) )
	{
	    int drug;
		bool addicted = FALSE;

		act( AT_ACTION, "You go over to $N, take a quick blood sample, and quickly take it back to your medical bag.", ch, NULL, victim, TO_CHAR);
		act( AT_ACTION, "From your medical bag, you take out a small device and quickly insert $N's blood sample.",ch, NULL, victim, TO_CHAR);
		act( AT_ACTION, "$n go over to you, takes a quick blood sample, and quickly takes it back to $s medical bag.", ch, NULL, victim, TO_VICT);
		act( AT_ACTION, "From $n's medical bag, $e takes out a small device and quickly inserts your blood sample.",ch, NULL, victim, TO_VICT);
		act( AT_ACTION, "$n go over to $N, takes a quick blood sample, and quickly takes it back to $s medical bag.", ch, NULL, victim, TO_NOTVICT);
		act( AT_ACTION, "From $n's medical bag, $e takes out a small device and quickly inserts $N's blood sample.",ch, NULL, victim, TO_NOTVICT);
		
		if ( number_percent() < chance )
		{
			for ( drug = 0; drug <= 9; drug++ )
			{
				if ( ch->pcdata->drug_level[drug] > 0 || ch->pcdata->drug_level[drug] > 0 )
				{
					if ( buf[0] != '\0')
						sprintf(buf,"%s and %s",spice_table[drug],buf);
					else
						sprintf(buf,"%s",spice_table[drug]);
					addicted = TRUE;
				}
			}
			if ( addicted == TRUE )
			{
				act( AT_INFO, "The machine dings, and a screen displaying that $N is addicted to $t.",ch, buf, victim, TO_CHAR);
				act( AT_INFO, "You quickly inform $N that $E is addicted to $t.",ch, buf, victim, TO_CHAR);
				act( AT_INFO, "The machine dings, and a screen appears showing information that you do not understand.",ch,NULL, victim, TO_ROOM);
				act( AT_INFO, "$n then comes back and informs you that you are addicted to $t.",ch,buf, victim, TO_VICT);		
				act( AT_INFO, "You quickly inform $N something.",ch,NULL, victim, TO_NOTVICT);
			}
			else
			{
				act( AT_INFO, "The machine dings, and a screen displaying that $N is not addicted to any drugs.",ch,NULL, victim, TO_CHAR);
				act( AT_INFO, "You quickly inform $N that $E is not addicted to any drugs.",ch,NULL, victim, TO_CHAR);
				act( AT_INFO, "The machine dings, and a screen appears showing information that you do not understand.",ch,NULL, victim, TO_ROOM);
				act( AT_INFO, "$n then comes back and informs you that you are not addicted to any drugs.",ch,NULL, victim, TO_VICT);
				act( AT_INFO, "You quickly inform $N something.",ch,NULL, victim, TO_NOTVICT);
			}
			if ( ch != victim )
				learn_from_success( ch, gsn_diagnose );
		}
		else
		{
			act( AT_INFO, "The machine dings, and a screen displaying some information.",ch,NULL, victim, TO_CHAR);
			act( AT_INFO, "But you are unable to read it.",ch,NULL, victim, TO_CHAR);
			act( AT_INFO, "The machine dings, and a screen appears showing information that you do not understand.",ch,NULL, victim, TO_ROOM);
			learn_from_failure( ch, gsn_diagnose );
			return;	
		}
		return;
	}
	if ( !str_cmp("bones",arg2) || !str_prefix("Bones",arg2) )
	{
		act( AT_ACTION, "You take a handheld x-ray scanner out of your bag and quickly run over to $N.", ch, NULL, victim, TO_CHAR);
		act( AT_ACTION, "$n takes a handheld x-ray scanner out of $s bag and quickly runs over to you.", ch, NULL, victim, TO_VICT);
		act( AT_ACTION, "$n takes a handheld x-ray scanner out of $s bag and quickly runs over to $N.", ch, NULL, victim, TO_NOTVICT);
		set_char_color( AT_WHITE, ch );

		if ( number_percent() < chance )
		{
			if ( !xIS_EMPTY(victim->bodyparts) )
			{
				act( AT_ACTION, "$N has the following broken bones in $S body:",ch,NULL, victim, TO_CHAR);
				set_char_color(AT_RED,ch);
				send_to_char(ext_flag_string(&victim->bodyparts, body_parts), ch);
				send_to_char("\n\r",ch);
			}
			else
			{
				set_char_color(AT_RED,ch);
				sprintf(buf,"%s has no broken bones\n\r",victim->name);
				send_to_char(buf,ch);
			}
			if ( ch != victim )
				learn_from_success( ch, gsn_diagnose );
			return;
		}
		else
		{
			set_char_color(AT_RED,ch);
			ch_printf(ch,"You are unable to comprehend the results of %s's x-ray.\n\r", victim->name);
			learn_from_failure( ch, gsn_diagnose );
			return;
		}
		return;
	}
	return;
}

void do_birth( CHAR_DATA *ch, char *argument )
{
	int chance;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_STRING_LENGTH];
	char newname[MAX_STRING_LENGTH];
	CHAR_DATA *victim;

    if (IS_NPC(ch)) return;
	argument = one_argument( argument, arg );
	
	if ( arg[0] == '\0' )
	{
		send_to_char("&WWho will be giving birth?\n\r",ch);
		return;
	}

	if ( ( victim = get_char_room(ch, arg) ) == NULL )
	{
		send_to_char("They are not here.\n\r",ch);
		return;
	}

	if (IS_NPC(victim)) return;
    if (!IS_SET(victim->act2, EXTRA_PREGNANT)) { return; }
    if (!IS_SET(victim->act2, EXTRA_LABOUR)) { return; }
	if ( victim->pcdata->birth_wait > 0 )
	{
		send_to_char("She is already giving birth.\n\r",ch);
		return;
	}
	
	if (victim->sex != SEX_FEMALE)
	{
		send_to_char("Chaaa! Riiight!\n\r",ch);
		return;
	}

	if ( !nifty_is_name( ch->name , victim->pcdata->partner) )
	{
		send_to_char("They have not given consent!\n\r",ch);
		return;
	}

    if ( argument[0] == '\0' )
    {
		if (victim->pcdata->genes[4] == SEX_MALE)
		    ch_printf( ch, "What does %s wish to name her little boy?\n\r", victim->name );
		else if (victim->pcdata->genes[4] == SEX_FEMALE)
		    ch_printf( ch, "What does %s wish to her little girl?\n\r", victim->name );
		else
		    ch_printf( ch, "What does %s wish to name her child?\n\r", victim->name );
		return;
    }

	chance = IS_NPC(ch) ? ch->top_level : (int)  (ch->pcdata->learned[gsn_birth]) ;
	if ( number_percent() < chance )
	{
		act( AT_ACTION,	"You prepare $N for delivery.",ch, NULL, victim, TO_CHAR);
		act( AT_ACTION, "$n prepares you for delivery.", ch, NULL, victim, TO_VICT);
		act( AT_ACTION, "$n prepares $N for delivery.", ch, NULL, victim, TO_NOTVICT);
	}
	else
	{
		send_to_char("&RYou don't know what todo.\n\r",ch);
		learn_from_failure( ch, gsn_birth );
		return;
	}

    if (!check_parse_name(argument, TRUE))
    {
		send_to_char( "Thats an illegal name.\n\r", ch );
		return;
    }

	sprintf( newname, "%s%c/%s", PLAYER_DIR, tolower(argument[0]), capitalize( argument ) );

	if ( access( newname, F_OK ) == 0 )
    {
		send_to_char( "That player already exists.\n\r", ch );
		return;
    }

	sprintf( buf, "%s %s", victim->pcdata->cparents, argument);
    if (!birth_ok(victim, buf))
    {
		send_to_char( "Bug - please inform Gavin.\n\r", ch );
		return;
    }
    argument[0] = UPPER(argument[0]);
	victim->pcdata->birth_name = STRALLOC( argument );
	victim->pcdata->birth_wait = 15;
	send_to_char( "OOC NOTE: this will take a few min, so for role playing purposes, you might want to stick around.\n\r",ch);
	send_to_char( "OOC NOTE: this will take a few min, so for role playing purposes, you might want to act aproprately.\n\r",victim);
    save_char_obj( victim );
	learn_from_success( ch, gsn_birth );
    return;
}

void do_abort(CHAR_DATA *ch, char *argument)
{
	char arg[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	int chance;

    if (IS_NPC(ch)) {
    	 /* Have a sence of humor :-) */
    	 send_to_char("Mobs can't perform such operations.\n\r", ch);
    	 return;
    }
	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char("&WWho's baby are your murdering today?\n\rSyntax: Abort <person>\n\r",ch);
		return;
	}
	if ( ( victim = get_char_room(ch, arg) ) == NULL )
	{
		send_to_char("Who is asking for this operation?.\n\r",ch);
		return;
	}
	if ( IS_NPC(victim) )
	{
		send_to_char("Mobs are computer generated characters.  Computers aren't alive. They don't have babies.\n\r",ch);
		return;
	}

	if (victim->sex != SEX_FEMALE)
	{
		send_to_char("I wouldn't sugest that if I were you...\n\r", ch);
		return;
	}

    if (!IS_SET(victim->act2, EXTRA_PREGNANT)) {
    	/* Well to begin if you have nothing no sence in using the {} Just gonna cause
    	   some extra work for the computer. But off that you should give the user
    	   an idea of why it isn't working.
    	*/
    	 ch_printf(ch, "Hey! %s isn't pregnant. What are you trying to do?\n\r", victim->name);
    	 return; 
    }

    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_abort]) ;
                
    if ( number_percent( ) > chance*2   )
    {
      send_to_char("&RYou fail to preform the operation.\n\r", ch);
      ch_printf(victim, "&R%s fails to preform the operation.\n\r", ch->name);
      victim->hit -= 300;
      return;
    }
    REMOVE_BIT(victim->act2, EXTRA_PREGNANT);
    REMOVE_BIT(victim->act2, EXTRA_LABOUR);
    
    act(AT_RED, "You have completed the Abortion Operation on $N, and it has been a success!", ch, NULL, victim, TO_CHAR);
    act(AT_RED, "$n has completed the Abortion Operation on $N, and it has been a success!", ch, NULL, victim, TO_ROOM);
    act(AT_RED, "$n has completed the Abortion Operation on you, and it has been a success!", ch, NULL, victim, TO_VICT);
    act(AT_RED, "You are no longer pregnant.", ch, NULL, victim, TO_VICT);
    /* Check if the room sterile flag is set. If not then they get to have an
       infection flag added. In update.c have it subtracts hp or something. They
       can have it fixed with another skill.
    */
    
	/*if (!xIS_SET(ch->in_room->room_flags, ROOM_SANITARY))*/
	{
		sh_int chance = 50;
		if (xIS_SET(ch->in_room->room_flags, ROOM_DIRTY) )
			chance -= 50;
		if (xIS_SET(ch->in_room->room_flags, ROOM_SANITARY) )
			chance += 20;
		else
			chance -= 20;
		
		if (ch->pcdata->clean_amount <= 0 )
			chance -= 30;
		else
			chance += 30;

     	/* A 25% chance roughly of getting an infection - skyrunner*/
		
		/* Not anymore, now it depends on the char and the room 
		 * Hopefully it'll work - Gavin 
		 */
		if (number_range(0,100) > chance )
		{
			victim->pcdata->infect_amount[BODY_STOMACH] += 15; 
			/* You could do a warning here but I wouldn't. Gives it away to early. */
			SET_BIT(victim->act2, EXTRA_INFECTION); /* Kept the extra going just for ease */
			/* Poor woman. Now she has an infection. :-( */
			/* Later we could add something where guys can get an infection also */
		}
	}
    
    save_char_obj( victim );
    learn_from_success( ch, gsn_abort );
    return;
}

void do_splint(CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	int	value, chance;
	CHAR_DATA *victim;
    
	argument = one_argument( argument, arg );

	if (IS_NPC(ch)) return;
    
	switch( ch->substate )
	{ 
		default:
			if ( arg[0] == '\0' )
			{
				ch_printf(ch,"Splint who's what?!?\n\r");
				return;
			}
			
			if ( ( victim = get_char_room(ch, arg) ) == NULL )
			{
				send_to_char("They are not here.\n\r",ch);
				return;
			}
			if ( IS_NPC(victim) )
			{
				ch_printf(ch,"UHHH RIIIIGHT!\n\r");
				return;
			}
			if ( victim == ch )
			{
				send_to_char("How do you expect to splint yourself?\n\r",ch);
				return;
			}
			if ( !nifty_is_name( ch->name , victim->pcdata->partner) )
			{
				send_to_char("They have not given consent!\n\r",ch);
				return;
			}
			if ( argument[0] == '\0')
			{
				ch_printf(ch,"Splint who's what?!?\n\r");
				return;
			}
			chance = IS_NPC(ch) ? ch->top_level : (int)  (ch->pcdata->learned[gsn_splint]) ;
			if ( number_percent() < chance )
			{
				act( AT_ACTION, "You take out some plaster and gauss from your bag and begin to assemble a cast on $N's $t.", ch, argument, victim, TO_CHAR);
				act( AT_ACTION, "$n takes out some plaster and gauss from $s's bag and begins to assemble a cast on your $t.", ch, argument, victim, TO_VICT);
				act( AT_ACTION, "$n takes out some plaster and gauss from $s's bag and begins to assemble a cast on $N's $t.", ch, argument, victim, TO_NOTVICT);
				WAIT_STATE( victim, 10 * PULSE_VIOLENCE );
				add_timer ( ch , TIMER_DO_FUN , 10 , do_splint , 1 );
				ch->dest_buf = str_dup(arg);
				ch->dest_buf_2 = str_dup(argument);
				return;
			}
			send_to_char("&RYou don't know what todo.\n\r",ch);
			learn_from_failure( ch, gsn_splint );
			return;
		
		case 1: 
			if ( !ch->dest_buf )
				return;
			if ( !ch->dest_buf_2 )
				return;
			strcpy( arg, ch->dest_buf);
			DISPOSE( ch->dest_buf);
			strcpy(arg3, ch->dest_buf_2);
			DISPOSE( ch->dest_buf_2);
			break;
			
		case SUB_TIMER_DO_ABORT:
			DISPOSE( ch->dest_buf );
			DISPOSE( ch->dest_buf_2 );
			ch->substate = SUB_NONE;    		                                   
			send_to_char("&RYou are interupted and fail to finish your work.\n\r", ch);
			return;
	}
	if ( ( victim = get_char_room(ch, arg) ) == NULL )
	{
		send_to_char("They are not here.\n\r",ch);
		return;
	}
	if ( victim == ch )
	{
		send_to_char("How do you expect to splint yourself?\n\r",ch);
		return;
	}
    chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_splint]) ;
	if ( number_percent() > chance*2)
	{
		act( AT_ACTION, "You fumble and mess up.", ch, NULL, NULL, TO_CHAR);
		act( AT_ACTION, "$n fumbles and creats a big mess.", ch, NULL,NULL, TO_ROOM);
		learn_from_failure( ch, gsn_splint );
		return;
	}

	value = get_bodypart( arg3 );
	if ( value < 0 || value > MAX_BITS )
	{
		ch_printf( ch, "Unknown Body part: %s\n\r", arg3 );
		return;
	}
	else if ( !xIS_SET( victim->bodyparts, value ) )
	{
		send_to_char("Now why would you want to splint that? Its not even broken.\n\r",ch);
		return;
	}
	else
	{
		act( AT_ACTION, "You finish and create a splended cast on $N's $t.", ch, arg3, victim, TO_CHAR);
		act( AT_ACTION, "$n finishes and create a splended cast on your $t.", ch, arg3, victim, TO_VICT);
		act( AT_ACTION, "$n finishes and create a splended cast on  $N's $t.", ch, arg3, victim, TO_NOTVICT);
		xREMOVE_BIT( victim->bodyparts, value );
	}
	learn_from_success( ch, gsn_splint );
}

void do_makemedkit( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance, strength, weight;
    bool checktool, checkdrink, checkchem;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    
    strcpy( arg , argument );
    
    switch( ch->substate )
	{ 
	default:
		if ( arg[0] == '\0' )
		{
			send_to_char( "&RUsage: Makemedkit <name>\n\r&w", ch);
			return;   
		}
		
		checktool	= FALSE;
		checkdrink	= FALSE;
		checkchem = FALSE;
		
		if ( !xIS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
		{
			send_to_char( "&RYou need to be in a factory or workshop to do that.\n\r", ch);
			return;
		}
		
		for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
		{
			if (obj->item_type == ITEM_TOOLKIT)
				checktool = TRUE;
			if (obj->item_type == ITEM_DRINK_CON && obj->value[1] == 0 )
				checkdrink = TRUE;
			if (obj->item_type == ITEM_CHEMICAL)
				checkchem = TRUE;                  
		}
		
		if ( !checktool )
		{
			send_to_char( "&RYou need toolkit to make a med kit.\n\r", ch);
			return;
		}
		
		if ( !checkdrink )
		{
			send_to_char( "&RYou will need an empty drink container to mix and hold the chemicals.\n\r", ch);
			return;
		}
		
		if ( !checkchem )
		{
			send_to_char( "&RSome chemicals would come in handy!\n\r", ch);
			return;
		}
		
		chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makemedkit]);
		if ( number_percent( ) < chance )
		{
			send_to_char( "&GYou begin the long process of making a medkit.\n\r", ch);
			act( AT_PLAIN, "$n takes $s tools and a drink container and begins to work on something.", ch, NULL, argument , TO_ROOM );
			add_timer ( ch , TIMER_DO_FUN , 25 , do_makemedkit , 1 );
			ch->dest_buf   = str_dup(arg);
			return;
		}
		send_to_char("&RYou your mixture fizles and dies.\n\r",ch);
		learn_from_failure( ch, gsn_makemedkit );
		return;
	case 1: 
		if ( !ch->dest_buf )
			return;
		strcpy(arg, ch->dest_buf);
		DISPOSE( ch->dest_buf);
		break;
	case SUB_TIMER_DO_ABORT:
		DISPOSE( ch->dest_buf );
		ch->substate = SUB_NONE;    		                                   
		send_to_char("&RYou are interupted and fail to finish your work.\n\r", ch);
		return;
	}
	
	ch->substate = SUB_NONE;
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makemedkit]);
    vnum = 10425;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration of this error.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkdrink = FALSE;
    checkchem = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_DRINK_CON && checkdrink == FALSE && obj->value[1] == 0 )
       {
          checkdrink = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_CHEMICAL)
       {
          strength = URANGE( 10, obj->value[0], level * 5 );
          weight = obj->weight;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkchem = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makemedkit]) ;
                
	if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdrink ) || ( !checkchem ) )
	{
		send_to_char( "&RJust as you are about to finish your work,\n\ryour newly created medkit bubles over and dies...doh!\n\r", ch);
		learn_from_failure( ch, gsn_makemedkit );
		return;
	}
	
	obj = create_object( pObjIndex, level );
	
	obj->item_type = ITEM_MEDPAC;
	CLEAR_BITS(obj->wear_flags);
    SET_BIT( obj->wear_flags, ITEM_HOLD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = weight;
    STRFREE( obj->name );
    strcpy( buf , arg );
    strcat( buf , " medkit");
    obj->name = STRALLOC( buf );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was carelessly left here." );
    obj->description = STRALLOC( buf );
    obj->value[0] = strength/2;
    obj->value[1] = strength;
    obj->cost = obj->value[1]*5;
	if ( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
		REMOVE_BIT( obj->extra_flags, ITEM_PROTOTYPE );

                                                                   
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish your work and hold up your superb new med kit.&w\n\r", ch);
    act( AT_PLAIN, "$g finishes making $s brand new medkit and holds it up for everyone to see.", ch, NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( obj->cost*50 ,( exp_level(ch->skill_level[MEDIC_ABILITY]+1) - exp_level(ch->skill_level[MEDIC_ABILITY]) ) );
         gain_exp(ch, xpgain, MEDIC_ABILITY );
         ch_printf( ch , "You gain %d medical experience.", xpgain );
    }
	learn_from_success( ch, gsn_makemedkit );
}

/*
 * Does things like height upgrades, weight changes, etc, maybe even hair coloring
 */
void do_cosmetic( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_STRING_LENGTH], arg2[MAX_STRING_LENGTH], arg3[MAX_STRING_LENGTH];
	int	chance, value;
	CHAR_DATA * victim;
	
	if (IS_NPC(ch))
		return;
	
	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg2 );
	strcpy( arg3, argument );
	
	if ( arg[0]=='\0' || arg2[0] == '\0')
	{
		send_to_char("Syntax:\n\r"
			"  Cosmetic [players name] [type of surgery] [amount(max 5)]\n\r"
			"  Types of surgery are:\n\r"
			" *  Leg [Exten]sions     * Leg [Short]enings\n\r"
			"    [Lypo]suction          [Indo]suction\n\r"
			"Note: * indicates you are able to specify the amount changed\n\r"
			"Warning, the type of surgery must either be\n\r"
			"the word in the []\'s or type the full name\n\r"
			"between quotation marks.\n\r",ch);
		return;
	}
	
	if ( arg3[0] == '\0' )
	{
		value = 5;
	}
	else
	{
		value = is_number( arg3 ) ? atoi( arg3 ) : -1;
		if ( atoi(arg3) < -1 && value == -1 )
			value = atoi(arg3);
		value = abs(value);
		value = URANGE(1,value,5);
	}
	
	if ( ( victim = get_char_room(ch, arg) ) == NULL )
	{
		send_to_char("They are not here.\n\r",ch);
		return;
	}
	
	if (IS_NPC(victim))
	{
		send_to_char("Not on NPC's.\n\r",ch);
		return;
	}
	
	if ( !nifty_is_name( ch->name , victim->pcdata->partner) )
	{
		send_to_char("They have not given consent!\n\r",ch);
		return;
	}
	
	if ( !str_cmp("Leg Extensions",arg2) || !str_prefix("Exten",arg2) )
	{
		if ( victim->move < 100 )
		{
			act( AT_ACTION, "$N's legs are too soar to operate.", ch, NULL, victim, TO_CHAR);
			return;
		}
		
		if ( victim->hit < 100 )
		{
			act( AT_ACTION, "If you continue, you could kill $N!.", ch, NULL, victim, TO_CHAR );
			return;
		}
		
		act( AT_ACTION, "You take out set of pills and feed $N one.", ch, NULL, victim, TO_CHAR);
		act( AT_ACTION, "While $N is knocked out, you quickly install $N's Leg Extensions.", ch, NULL, victim, TO_CHAR);
		act( AT_ACTION, "$n feeds you a pill. You suddenly feel very sleepy.", ch, NULL, victim, TO_VICT);
		act( AT_ACTION, "$n feeds $N a pill... $N Falls Asleep", ch, NULL, victim, TO_NOTVICT);
		
		chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_cosmetic]);
		
		act( AT_ACTION, "While $N is asleep, $n quickly does an operation on $N.", ch, NULL, victim, TO_NOTVICT);
		act( AT_ACTION, "You Quickly Finish as $N is waking up.", ch, NULL, victim, TO_CHAR);
		{
			sh_int chance = 50;
			if (xIS_SET(ch->in_room->room_flags, ROOM_DIRTY) )
				chance -= 50;
			if (xIS_SET(ch->in_room->room_flags, ROOM_SANITARY) )
				chance += 20;
			else
				chance -= 20;
			if (ch->pcdata->clean_amount <= 0 )
				chance -= 30;
			else
				chance += 30;
			
			/* A 25% chance roughly of getting an infection - skyrunner*/
			/* Not anymore, now it depends on the char and the room 
			 * Hopefully it'll work - Gavin 
			 */
			if (number_range(0,100) > chance )
			{
				victim->pcdata->infect_amount[BODY_L_LEG] += 15; 
				victim->pcdata->infect_amount[BODY_R_LEG] += 15;
				SET_BIT(victim->act2, EXTRA_INFECTION);
			}
		}
		if ( number_percent( ) < chance )
		{
			send_to_char("The operation was a complete success!\n\r",ch);
			act( AT_ACTION, "You Wake Up a few moments later and your legs feel funny.", ch, NULL, victim, TO_VICT);
			victim->height += value;
			victim->move = 0;
			victim->hit = 15;
			learn_from_success( ch, gsn_cosmetic );
			return;
		}
		else
		{
			send_to_char("Opps\n\r",ch);
			victim->height -= 5;
			victim->move = 0;
			victim->hit = 15;
			act( AT_ACTION, "You Wake Up a few moments later and your legs feel funny.", ch, NULL, victim, TO_VICT);
			learn_from_failure( ch, gsn_cosmetic );
			return;
		}
	}/* Leg Extentions */
	
	if ( !str_cmp("Leg Shortenings",arg2) || !str_prefix("Short",arg2) )
	{
		if ( victim->move < 100 )
		{
			act( AT_ACTION, "$N's legs are too soar to operate.", ch, NULL, victim, TO_CHAR);
			return;
		}
		if ( victim->hit < 100 )
		{
			act( AT_ACTION, "If you continue, you could kill $N!.", ch, NULL, victim, TO_CHAR );
			return;
		}
		
		act( AT_ACTION, "You take out set of pills and feed $N one.", ch, NULL, victim, TO_CHAR);
		act( AT_ACTION, "While $N is knocked out, you quickly install $N's Leg Extensions.", ch, NULL, victim, TO_CHAR);
		act( AT_ACTION, "$n feeds you a pill. You suddenly feel very sleepy.", ch, NULL, victim, TO_VICT);
		act( AT_ACTION, "$n feeds $N a pill... $N Falls Asleep", ch, NULL, victim, TO_NOTVICT);
		
		chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_cosmetic]);
		
		act( AT_ACTION, "While $N is asleep, $n quickly does an operation on $N.", ch, NULL, victim, TO_NOTVICT);
		act( AT_ACTION, "You Quickly Finish as $N is waking up.", ch, NULL, victim, TO_CHAR);
		{
			sh_int chance = 50;
			if (xIS_SET(ch->in_room->room_flags, ROOM_DIRTY) )
				chance -= 50;
			if (xIS_SET(ch->in_room->room_flags, ROOM_SANITARY) )
				chance += 20;
			else
				chance -= 20;
			if (ch->pcdata->clean_amount <= 0 )
				chance -= 30;
			else
				chance += 30;
			/* A 25% chance roughly of getting an infection - skyrunner*/
			
			/* Not anymore, now it depends on the char and the room 
			 * Hopefully it'll work - Gavin 
			 */
			if (number_range(0,100) > chance )
			{
				victim->pcdata->infect_amount[BODY_L_LEG] += 15; 
				victim->pcdata->infect_amount[BODY_R_LEG] += 15;
				SET_BIT(victim->act2, EXTRA_INFECTION);
			}
		}
		if ( number_percent( ) < chance )
		{
			send_to_char("The operation was a complete success!\n\r",ch);
			act( AT_ACTION, "You Wake Up a few moments later and your legs feel funny.", ch, NULL, victim, TO_VICT);
			victim->height -= value;
			victim->hit = 15;
			victim->move = 0;
			learn_from_success( ch, gsn_cosmetic );
			return;
		}
		else
		{
			send_to_char("Opps\n\r",ch);
			victim->move = 0;
			victim->hit = 15;
			act( AT_ACTION, "You Wake Up a few moments later and your legs feel funny.", ch, NULL, victim, TO_VICT);
			learn_from_failure( ch, gsn_cosmetic );
			return;
		}
	}/* Leg Shortenings */
	
	if ( !str_cmp("Lyposuction",arg2) || !str_prefix("Lypo",arg2) )
	{
		if ( victim->move < 100 )
		{
			act( AT_ACTION, "$N's is too weak to operate.", ch, NULL, victim, TO_CHAR);
			return;
		}
		if ( victim->hit < 100 )
		{
			act( AT_ACTION, "If you continue, you could kill $N!.", ch, NULL, victim, TO_CHAR );
			return;
		}
		
		act( AT_ACTION, "You take out set of pills and feed $N one.", ch, NULL, victim, TO_CHAR);
		act( AT_ACTION, "$n feeds you a pill. You suddenly feel very sleepy.", ch, NULL, victim, TO_VICT);
		act( AT_ACTION, "$n feeds $N a pill... $N Falls Asleep", ch, NULL, victim, TO_NOTVICT);
		act( AT_ACTION, "Then you quickly take out a machine out of your medical bag.", ch, NULL, victim, TO_CHAR);
		act( AT_ACTION, "Then $n quickly takes out a machine out from $s's medical bag.", ch, NULL, victim, TO_NOTVICT);
		
		chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_cosmetic]);
		act( AT_ACTION, "You attach your machine to $N's midsection.", ch, NULL, victim, TO_CHAR);
		{
			sh_int chance = 50;
			if (xIS_SET(ch->in_room->room_flags, ROOM_DIRTY) )
				chance -= 50;
			if (xIS_SET(ch->in_room->room_flags, ROOM_SANITARY) )
				chance += 20;
			else
				chance -= 20;
			if (ch->pcdata->clean_amount <= 0 )
				chance -= 30;
			else
				chance += 30;
			/* A 25% chance roughly of getting an infection - skyrunner*/
			
			/* Not anymore, now it depends on the char and the room 
			 * Hopefully it'll work - Gavin 
			 */
			
			if (number_range(0,100) > chance )
			{
				victim->pcdata->infect_amount[BODY_STOMACH] += 15; 
				SET_BIT(victim->act2, EXTRA_INFECTION);
			}
		}
		if ( number_percent( ) < chance )
		{
			act( AT_ACTION, "The machine humms.. Then Suddenly Dings.", ch, NULL, victim, TO_CHAR);
			act( AT_ACTION, "$n does an operation while $N is asleep.", ch, NULL, victim, TO_NOTVICT);
			act( AT_ACTION, "The operation is a complete success! And $N is just starting to wake up.", ch, NULL, victim, TO_CHAR);
			act( AT_ACTION, "You wake a few moments later and you feel soar all over.", ch, NULL, victim, TO_VICT);
			victim->weight -= number_range(5,50);
			victim->move = 0;
			victim->hit = 15;
			learn_from_success( ch, gsn_cosmetic );
			return;
		}
		else
		{
			act( AT_ACTION, "The machine humms, but doesn't sound right... *Ding* Operation Complete.", ch, NULL, victim, TO_CHAR);
			act( AT_ACTION, "$n does an operation while $N is asleep.", ch, NULL, victim, TO_NOTVICT);
			act( AT_ACTION, "The operation is a complete failure! And $N is just starting to wake up.", ch, NULL, victim, TO_CHAR);
			act( AT_ACTION, "You wake a few moments later and you feel soar all over.", ch, NULL, victim, TO_VICT);
			victim->move = 0;
			victim->hit = 15;
			learn_from_failure( ch, gsn_cosmetic );
			return;
		}
	}/* Lyposuction */

	if ( !str_cmp("Indosuction",arg2) || !str_prefix("Indo",arg2) )
	{
		if ( victim->move < 100 )
		{
			act( AT_ACTION, "$N's is too weak to operate.", ch, NULL, victim, TO_CHAR);
			return;
		}

		if ( victim->hit < 100 )
		{
			act( AT_ACTION, "If you continue, you could kill $N!.", ch, NULL, victim, TO_CHAR );
			return;
		}
		
		act( AT_ACTION, "You take out set of pills and feed $N one.", ch, NULL, victim, TO_CHAR);
		act( AT_ACTION, "$n feeds you a pill. You suddenly feel very sleepy.", ch, NULL, victim, TO_VICT);
		act( AT_ACTION, "$n feeds $N a pill... $N Falls Asleep", ch, NULL, victim, TO_NOTVICT);
		act( AT_ACTION, "Then you quickly take out a machine out of your medical bag.", ch, NULL, victim, TO_CHAR);
		act( AT_ACTION, "Then $n quickly takes out a machine out from $s's medical bag.", ch, NULL, victim, TO_NOTVICT);
		chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_cosmetic]);
		act( AT_ACTION, "You attach your machine to $N's midsection.", ch, NULL, victim, TO_CHAR);
		{
			sh_int chance = 50;
			if (xIS_SET(ch->in_room->room_flags, ROOM_DIRTY) )
				chance -= 50;
			if (xIS_SET(ch->in_room->room_flags, ROOM_SANITARY) )
				chance += 20;
			else
				chance -= 20;
			if (ch->pcdata->clean_amount <= 0 )
				chance -= 30;
			else
				chance += 30;

			/* A 25% chance roughly of getting an infection - skyrunner*/
			
			/* Not anymore, now it depends on the char and the room 
			* Hopefully it'll work - Gavin 
			*/
			if (number_range(0,100) > chance )
			{
				victim->pcdata->infect_amount[BODY_STOMACH] += 15; 
				/* You could do a warning here but I wouldn't. Gives it away to early. */
				SET_BIT(victim->act2, EXTRA_INFECTION); /* Kept the extra going just for ease */
				/* Poor woman. Now she has an infection. :-( */
				/* Later we could add something where guys can get an infection also */
			}
		}
		if ( number_percent( ) < chance )
		{
			act( AT_ACTION, "The machine humms.. Then Suddenly Dings.", ch, NULL, victim, TO_CHAR);
			act( AT_ACTION, "$n does an operation while $N is asleep.", ch, NULL, victim, TO_NOTVICT);
			act( AT_ACTION, "The operation is a complete success! And $N is just starting to wake up.", ch, NULL, victim, TO_CHAR);
			act( AT_ACTION, "You wake a few moments later and you feel soar all over.", ch, NULL, victim, TO_VICT);
			victim->weight += number_range(5,50);
			victim->move = 0;
			victim->hit = 15;
			learn_from_success( ch, gsn_cosmetic );
			return;
		}
		else
		{
			act( AT_ACTION, "The machine humms, but doesn't sound right... *Ding* Operation Complete.", ch, NULL, victim, TO_CHAR);
			act( AT_ACTION, "$n does an operation while $N is asleep.", ch, NULL, victim, TO_NOTVICT);
			act( AT_ACTION, "The operation is a complete failure! And $N is just starting to wake up.", ch, NULL, victim, TO_CHAR);
			act( AT_ACTION, "You wake a few moments later and you feel soar all over.", ch, NULL, victim, TO_VICT);
			victim->move = 0;
			victim->hit = 15;
			learn_from_failure( ch, gsn_cosmetic );
			return;
		}
	}/* Indosuction */
	return;
}


void do_autopsy( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
	char arg[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int chance;

	strcpy ( arg, argument);

    if ( IS_NPC(ch) || !ch->pcdata )
    {
		send_to_char( "Huh?\n\r", ch );
		return;
    }
    
    if ( ch->pcdata->learned[gsn_autopsy] <= 0 )
    {
		send_to_char( "Huh?\n\r", ch );
		return;
    }
	switch( ch->substate )
	{ 
		default:
			if ( arg[0] == '\0' )
			{
				send_to_char( "Syntax: autopsy corpsename.\n\r" ,ch );
				return;
			}
			obj = get_obj_here( ch, arg );
			if ( !obj )
			{
				send_to_char( "This only works on corpses!\n\r", ch );
				return;
			}
			if ( obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC )
			{
				send_to_char( "This only works on corpses!\n\r", ch);
				return;
			}
			chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_autopsy]);

			if ( number_percent( ) < chance )
			{
				send_to_char( "&GYou begin the long process doing an autopsy on the corpse.\n\r", ch);
				act( AT_PLAIN, "$n begins to work the corpse.", ch, NULL, argument , TO_ROOM );			
				add_timer ( ch , TIMER_DO_FUN , 10 , do_autopsy , 1 );
				ch->dest_buf = str_dup(arg);
				return;
			}
			send_to_char("&RYou can't figure out how to proceed.\n\r",ch);
			learn_from_failure( ch , gsn_autopsy );
			return;
		
		case 1: 
			if ( !ch->dest_buf )
				return;
			strcpy(arg, ch->dest_buf);
			DISPOSE( ch->dest_buf);
			break;
			
		case SUB_TIMER_DO_ABORT:
			DISPOSE( ch->dest_buf );
			ch->substate = SUB_NONE;    		                                   
			send_to_char("&RYou are interupted and fail to finish your work.\n\r", ch);
			return;
	}
	ch->substate = SUB_NONE;

	obj = get_obj_here( ch, arg );
	if ( !obj )
	{
		send_to_char( "This only works on corpses!\n\r", ch );
		return;
	}
	
	if ( obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC )
	{
		send_to_char( "This only works on corpses!\n\r", ch);
		return;
	}
	
	chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_autopsy]);
	if ( number_percent( ) < chance )
	{ /* Success */
		act( AT_IMMORT, "$n has finished $s work on the corpse.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "You have finished your autopsy.\n\r", ch );
		if ( !obj->armed_by )
		{
			sprintf( buf, "Only to find out that you are unable to determine the killer.\n\r");
			send_to_char( buf, ch);
		}
		else
		{
			sprintf( buf, "Only to find that the killer is none other than %s!\n\r", obj->armed_by );
			send_to_char( buf, ch);
		}
	}
	else
	{ /* Failed */
		act( AT_IMMORT, "$n has finished $s work on the corpse.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "You have finished your autopsy.\n\r", ch );
		sprintf( buf, "But am unable to determen the cause of death.\n\r");
		send_to_char( buf, ch);
	}

	/* required due to object grouping */
	separate_obj( obj );
	/* Then Remove from game */ 
	extract_obj( obj );

	{
		int range;
		sh_int chance = 50;
		if (xIS_SET(ch->in_room->room_flags, ROOM_DIRTY) )
			chance -= 50;
		if (xIS_SET(ch->in_room->room_flags, ROOM_SANITARY) )
			chance += 20;
		else
			chance -= 20;
		if (ch->pcdata->clean_amount <= 0 )
			chance -= 30;
		else
			chance += 30;

		/* A 25% chance roughly of getting an infection - skyrunner*/
		
		/* Not anymore, now it depends on the char and the room 
		 * Hopefully it'll work - Gavin 
		 */
		range = number_range(0,100);
		bug("Range = %d and chance = %d", range, chance );
		if ( range > chance )
		{
			ch->pcdata->infect_amount[BODY_L_HAND] += 15; 
			ch->pcdata->infect_amount[BODY_R_HAND] += 15;
			/* You could do a warning here but I wouldn't. Gives it away to early. */
			SET_BIT(ch->act2, EXTRA_INFECTION); /* Kept the extra going just for ease */
			/* Poor woman. Now she has an infection. :-( */
			/* Later we could add something where guys can get an infection also */
		}
	}
	{
		long xpgain;
		
		xpgain = UMIN( 15000 ,( exp_level(ch->skill_level[MEDIC_ABILITY]+1) - exp_level(ch->skill_level[MEDIC_ABILITY]) ) );
		gain_exp(ch, xpgain, MEDIC_ABILITY);
		ch_printf( ch , "You gain %d medical experience.", xpgain );
	}	
	learn_from_success( ch , gsn_autopsy );  
}

void do_make_infect_med ( CHAR_DATA * ch, char * argument )
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int level, chance, strength, weight;
	bool checkdrink, checkchem;
	OBJ_DATA *obj;
	OBJ_DATA *material;
	
	strcpy( arg , argument );
	switch( ch->substate )
	{ 
	default:
		if ( arg[0] == '\0' )
		{
			send_to_char( "&RUsage: Make_infect_med <name>\n\r&w", ch);
			return;
		}
		
		checkdrink	= FALSE;
		checkchem = FALSE;
		
		if ( !xIS_SET( ch->in_room->room_flags, ROOM_SANITARY ) )
		{
			send_to_char( "&RYou need to be in a clean room todo that.\n\r", ch);
			return;
		}
		
		for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
		{
			if (obj->item_type == ITEM_DRINK_CON && obj->value[1] == 0 )
				checkdrink = TRUE;
			if (obj->item_type == ITEM_CHEMICAL)
				checkchem = TRUE;
		}
		if ( !checkdrink )
		{
			send_to_char( "&RYou will need an empty drink container to mix and hold the chemicals.\n\r", ch);
			return;
		}
		if ( !checkchem )
		{
			send_to_char( "&RSome chemicals would come in handy!\n\r", ch);
			return;
		}
		
		chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_make_infect_med]);
		if ( number_percent( ) < chance )
		{
			send_to_char( "&GYou begin the long process of making some cureing medicine for infections.\n\r", ch);
			act( AT_PLAIN, "$n takes $s tools and a drink container and begins to work on something.", ch, NULL, argument , TO_ROOM );
			add_timer ( ch , TIMER_DO_FUN , 25 , do_make_infect_med , 1 );
			ch->dest_buf   = str_dup(arg);
			return;
		}
		send_to_char("&RYou your mixture fizles and dies.\n\r",ch);
		learn_from_failure( ch, gsn_make_infect_med );
		return;
	case 1: 
		if ( !ch->dest_buf )
			return;
		strcpy(arg, ch->dest_buf);
		DISPOSE( ch->dest_buf);
		break;
	case SUB_TIMER_DO_ABORT:
		DISPOSE( ch->dest_buf );
		ch->substate = SUB_NONE;    		                                   
		send_to_char("&RYou are interupted and fail to finish your work.\n\r", ch);
		return;
	}
	
	ch->substate = SUB_NONE;
    
	level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_make_infect_med]);
	
	checkdrink = FALSE;
	checkchem = FALSE;
	
	for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
	{
		if (obj->item_type == ITEM_DRINK_CON && checkdrink == FALSE && obj->value[1] == 0 )
		{
			checkdrink = TRUE;
			separate_obj( obj );
			obj_from_char( obj );
			extract_obj( obj );
		}
		if (obj->item_type == ITEM_CHEMICAL)
		{
			strength = URANGE( 10, obj->value[0], level * 5 );
			weight = obj->weight;
			separate_obj( obj );
			obj_from_char( obj );
			material = obj;
			checkchem = TRUE;
		}
	}
    
	obj = material;
	chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_make_infect_med]);
	
	if ( number_percent( ) > chance*2  || ( !checkdrink ) || ( !checkchem ) )
	{
		send_to_char( "&RJust as you are about to finish your work,\n\ryour concoction bubbles over and dies!\n\r", ch);
		learn_from_failure( ch, gsn_make_infect_med );
		return;
	}
	
	obj->item_type = ITEM_SYRINGE;
	CLEAR_BITS(obj->wear_flags);
    SET_BIT( obj->wear_flags, ITEM_HOLD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = weight;
    STRFREE( obj->name );
    strcpy( buf , arg );
    strcat( buf , " infection medicine");
    obj->name = STRALLOC( buf );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was carelessly left here." );
    obj->description = STRALLOC( buf );
    obj->value[0] = strength/2;
    obj->value[1] = strength;
	obj->value[2] = SYRINGE_INFECT_CURE;
	obj->value[3] = 0;
	obj->value[4] = 0;
	obj->value[5] = 0;
    obj->cost = obj->value[1]*5;
	if ( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
		REMOVE_BIT( obj->extra_flags, ITEM_PROTOTYPE );

                                                                   
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish working on the infection medicine.&w\n\r", ch);
    act( AT_PLAIN, "$g finishes making $s concoction.", ch, NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( obj->cost*50 ,( exp_level(ch->skill_level[MEDIC_ABILITY]+1) - exp_level(ch->skill_level[MEDIC_ABILITY]) ) );
         gain_exp(ch, xpgain, MEDIC_ABILITY );
         ch_printf( ch , "You gain %d medical experience.", xpgain );
    }
	learn_from_success( ch, gsn_make_infect_med );
}


void do_washup ( CHAR_DATA * ch, char * argument )
{
	bool checksoap;
    OBJ_DATA *obj;
	CHAR_DATA * victim;
	int chance;

	/* This command is to temporarly sterilize the char.. it'll help to prevent infections
	 * with this, and a starilized/clean room.. there is no chance of infection
	 * otherwise, with a clean room, and not washed, chance of infection is more then 
	 * without washed, but less then washed and not in a clean room.
	 *
	 * As of Mar 22, it is not completed yet
	 * well. its 4:32pm, and its looking close to done.
	 * - Gavin
	 */

	/*
	 * a) change washup syntax to washup char.. ie washup self
	 * b)  when typing washup by it self, it'll say how clean you are...
	 * - Gavin
	 */
	if ( IS_NPC(ch) )
		return;
	/*if ( argument[0] == '\0' )
	{
		ch_printf(ch, "Your hands are %s.\n\r",
			( ch->pcdata->clean_amount < 25 ? "kinda dirty" :
			 ( ( ch->pcdata->clean_amount >= 25 
				&& ch->pcdata->clean_amount < 50) ? " slightly clean" :
			 ( (ch->pcdata->clean_amount >= 50 && 
				ch->pcdata->clean_amount < 75) ? " clean" : 
			 ( (ch->pcdata->clean_amount >= 75 && 
				ch->pcdata->clean_amount < 100)	? "spotless" : "an error" ) ) ) ) );
		return;
	}*/
	if ( argument[0] == '\0' )
	{
		char buf[MAX_STRING_LENGTH];

		if ( ch->pcdata->clean_amount < 25 )
			sprintf( buf, "Your hands are kinda dirty\n\r");
		else if (ch->pcdata->clean_amount >= 25 && ch->pcdata->clean_amount < 50) 
			sprintf( buf, "Your hands are slightly clean\n\r");
		else if (ch->pcdata->clean_amount >= 50 && ch->pcdata->clean_amount < 75) 
			sprintf( buf, "Your hands are clean\n\r");
		else if (ch->pcdata->clean_amount >= 75 && ch->pcdata->clean_amount < 100)
			sprintf( buf, "Your hands are spotless.\n\r");
		else
			sprintf( buf, "error\n\r" );
		send_to_char(buf, ch);
		return;
	}
	checksoap = FALSE;
	if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
		send_to_char("They're not here...\n\r",ch);
		return;
    }
	for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
	{
		if (obj->item_type == ITEM_SOAP)
			checksoap = TRUE;
	}

	if ( !checksoap )
	{
		send_to_char( "&RYou need some soap to wash your hands.\n\r", ch);
		return;
	}

    checksoap = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_SOAP)
       {
          obj->value[0] -= 1;
		  if ( obj->value[0] == 0)
		  {
			  separate_obj( obj );
			  obj_from_char( obj );
			  extract_obj( obj );
		  }
		  checksoap = TRUE;
       }
    }                      
    
    chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_washup]) ;
                
	if ( number_percent( ) > chance*2  || !checksoap )
	{
		for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
		{
			if (obj->item_type == ITEM_SOAP)
			{
				separate_obj( obj );
				obj_from_char( obj );
				extract_obj( obj );
				break;
			}
		}
		send_to_char( "&RYou slip and waste the soap as it touches the ground", ch);
		learn_from_failure( ch, gsn_washup );
		return;
	}
	if ( ch == victim )
	{
		if ( victim->pcdata->clean_amount >= 90 )
		{
			send_to_char("Stop washing your hands, they're plenty clean enough.\n\r",ch);
			return;
		}
		else if ( ch->pcdata->clean_amount <= 0 )
		{
			act(AT_ACTION,"You thoroughly wash your hands.", ch, NULL, NULL, TO_CHAR );
			act(AT_ACTION,"$n thoroughly washes $s hands.", ch, NULL, NULL, TO_ROOM );
			ch->pcdata->clean_amount += number_range(10,35);
		}
		else if ( ch->pcdata->clean_amount >= 25 )
		{
			act(AT_ACTION,"You wash your hands carefully.", ch, NULL, NULL, TO_CHAR );
			act(AT_ACTION,"$n carefully washes $s hands.", ch, NULL, NULL, TO_ROOM );
			ch->pcdata->clean_amount += number_range(10,35);
		}
		ch->pcdata->clean_amount = URANGE( 1, ch->pcdata->clean_amount, 100 );
	}
	else
	{
		if ( victim->pcdata->clean_amount >= 90 )
		{
			send_to_char("Stop washing thier hands, they're plenty clean enough.\n\r",ch);
			return;
		}
		else if ( victim->pcdata->clean_amount <= 0 )
		{
			act(AT_ACTION,"You thoroughly wash $N's hands.", ch, NULL, victim, TO_CHAR );
			act(AT_ACTION,"$n thoroughly washes $N's hands.", ch, NULL, victim, TO_NOTVICT );
			act(AT_ACTION,"$n thoroughly washes your hands.", ch, NULL, victim, TO_VICT );
			victim->pcdata->clean_amount += number_range(10,35);
		}
		else if ( victim->pcdata->clean_amount >= 25 )
		{
			act(AT_ACTION,"You wash $N's hands carefully.", ch, NULL, victim, TO_CHAR );
			act(AT_ACTION,"$n carefully washes $N's hands.", ch, NULL, victim, TO_NOTVICT );
			act(AT_ACTION,"$n carefully washes your hand.", ch, NULL, victim, TO_VICT );
			victim->pcdata->clean_amount += number_range(10,35);
		}
		victim->pcdata->clean_amount = URANGE( 1, victim->pcdata->clean_amount, 100 );
	}
	WAIT_STATE( ch, skill_table[gsn_washup]->beats );
	{
         long xpgain;
         xpgain = UMIN( 500 ,( exp_level(ch->skill_level[MEDIC_ABILITY]+1) - exp_level(ch->skill_level[MEDIC_ABILITY]) ) );
         gain_exp(ch, xpgain, MEDIC_ABILITY );
         ch_printf( ch , "You gain %d medical experience.", xpgain );
    }
	learn_from_success( ch, gsn_washup );
	return;
}

void show_infect_line( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
	int x;
	
	if (IS_NPC(victim) )
		return;
	/* This just indicates if the victim accually has an infection
	 * no point in looping if he doesn't.. should be unset when
	 * infections are cleared. and set when any thing gets infected
	 */
	if ( !IS_SET(victim->/*pcdata->*/act2, EXTRA_INFECTION) )
		return;
	for ( x = 0; x < MAX_BODY_PARTS; x++ )
	{
		if ( victim->pcdata->infect_amount[x] > 100 )
		{
			sprintf(buf, "%s has a%s looking infection in %s %s.\n\r", victim->full_name, 
				( victim->pcdata->infect_amount[x] < 250 ? " minor" :
			(victim->pcdata->infect_amount[x] >= 250 
				&& victim->pcdata->infect_amount[x] < 500) ? " moderate" :
			(victim->pcdata->infect_amount[x] >= 500 && 
				victim->pcdata->infect_amount[x] < 750) ? " major" : 
			(victim->pcdata->infect_amount[x] >= 750 && 
				victim->pcdata->infect_amount[x] <= 1000)	? "n extremly bad" : "n error for" ), 
				his_her[victim->sex], body_parts[x] );
			send_to_char(buf,ch);
		}
	}
	return;
}

void do_inject ( CHAR_DATA * ch, char * argument )
{
	CHAR_DATA * victim;
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char * argument2;
	int	value, chance;
	OBJ_DATA * obj;
    
	argument = one_argument( argument, arg );

	if (IS_NPC(ch)) return;
    
	switch( ch->substate )
	{ 
		default:
			one_argument( argument, arg2 );
			if ( arg[0] == '\0' )
			{
				send_to_char("Inject Whom?\n\r",ch);
				return;
			}
			if ( ( victim = get_char_room(ch, arg) ) == NULL )
			{
				send_to_char("They are not here.\n\r",ch);
				return;
			}
			if ( IS_NPC(victim) )
			{
				ch_printf(ch,"Not on NPC's!\n\r");
				return;
			}
			if ( !nifty_is_name( ch->name , victim->pcdata->partner) )
			{
				send_to_char("They have not given consent!\n\r",ch);
				return;
			}
			if ( arg2[0] == '\0')
			{
				ch_printf(ch,"Inject whom with what medicine?\n\r");
				return;
			}
			if ( ( obj = get_obj_carry( ch, arg2 ) ) == NULL ) 
			{
				send_to_char( "You can't find that medicine.\n\r",ch);
				return;
			}
			if ( obj->item_type != ITEM_SYRINGE )
			{
				send_to_char("That isn't a syringe.\n\r",ch);
				return;
			}
			if ( obj->value[0] == 0 )
			{
				send_to_char("Its Empty.\n\r", ch);
				return;
			}
			if ( argument[0] == '\0')
			{
				ch_printf(ch,"Inject whom with what medicine?\n\r");
				return;
			}

			chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_inject]) ;
			if ( number_percent() < chance )
			{
				act( AT_ACTION, "You take out some $p and prepare to inject it into $N.", ch, obj, victim, TO_CHAR);
				act( AT_ACTION, "$n takes out some $p and prepares to inject it into you.", ch, obj, victim, TO_VICT);
				act( AT_ACTION, "$n takes out some $p and prepares to inject it into $N.", ch, obj, victim, TO_NOTVICT);
				WAIT_STATE( victim, 10 * PULSE_VIOLENCE );
				add_timer ( ch , TIMER_DO_FUN , 10 , do_inject , 1 );
				ch->dest_buf = str_dup(arg);
				ch->dest_buf_2 = str_dup(argument);
				return;
			}
			send_to_char("&RYou don't know what todo.\n\r",ch);
			learn_from_failure( ch, gsn_inject );
			return;
		
		case 1: 
			if ( !ch->dest_buf )
				return;
			if ( !ch->dest_buf_2 )
				return;
			strcpy( arg, ch->dest_buf);
			DISPOSE( ch->dest_buf);
			/*strcpy(arg3, ch->dest_buf_2);*/
			argument2 = str_dup(ch->dest_buf_2);
			DISPOSE( ch->dest_buf_2);
			break;
			
		case SUB_TIMER_DO_ABORT:
			DISPOSE( ch->dest_buf );
			DISPOSE( ch->dest_buf_2 );
			ch->substate = SUB_NONE;    		                                   
			send_to_char("&RYou are interupted and fail to finish your work.\n\r", ch);
			return;
	}
	{
		char object_arg[MAX_INPUT_LENGTH];
		
		argument2 = one_argument(argument2, object_arg );
		
		if ( ( victim = get_char_room(ch, arg) ) == NULL )
		{			
			send_to_char("They are not here.\n\r",ch);
			return;
		}
		
		if ( ( obj = get_obj_carry( ch, object_arg ) ) == NULL ) 
		{
			send_to_char("You can't find that medicine.\n\r",ch);
			return;
		}
		if ( obj->item_type != ITEM_SYRINGE)
		{
			send_to_char("That isn't a syringe.\n\r",ch);
			return;
		}
		
		chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_inject]) ;
		if ( ( number_percent() > chance*2 ) || ( obj->value[0] == 0 ))
		{
			act( AT_ACTION, "You fumble and mess up.", ch, NULL, NULL, TO_CHAR);
			act( AT_ACTION, "$n fumbles and creats a big mess.", ch, NULL,NULL, TO_ROOM);
			learn_from_failure( ch, gsn_inject );
			return;
		}

		switch( obj->value[2] )
		{
		default:
			send_to_char("Invalid Type of Syringe.\n\r",ch);
			bug("do_inject: invalid value2 = %d", obj->value[2]);
			break;

		case SYRINGE_INFECT_CURE:
			{
				value = get_bodypart( argument2 );
				if ( value < 0 || value > MAX_BODY_PARTS )
				{
					ch_printf( ch, "Unknown flag: %s\n\r", argument2 );
				}
				else
				{
					sh_int x;
					act( AT_ACTION, "You inject the medicine into $N's $t.", ch, argument2, victim, TO_CHAR);
					act( AT_ACTION, "$n injects the medicine into your $t.", ch, argument2, victim, TO_VICT);
					act( AT_ACTION, "$n injects the medicine into $N's $t.", ch, argument2, victim, TO_NOTVICT);
					victim->pcdata->infect_amount[value] = 0;
					REMOVE_BIT(victim->act2, EXTRA_INFECTION);
					for ( x = 0; x < MAX_BODY_PARTS; x++ )
					{
						if ( victim->pcdata->infect_amount[x] > 0 )
						{
							SET_BIT(victim->act2, EXTRA_INFECTION);
							break;
						}
					}
				}
				break;
			}
		}
		obj->value[0]--;
		learn_from_success( ch, gsn_inject );
	}
}

#ifdef UNCODED_COMMANDS
void do_crawl ( CHAR_DATA * ch, char * argument )
{
	/* This command is for movement when you have broken your legs, since you can't walk..
	 */
	if ( argument[0] == '\0' )
	{
		send_to_char( "Message\n\r", ch);
		return;
	}
}

#endif
