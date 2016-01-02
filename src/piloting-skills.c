/***************************************************************************
*                           Unknown Region 1.0                             *
*--------------------------------------------------------------------------*
* Unknown Regions Code Additions and changes from the StarWars Reality Code*
* copyright (c) 2000 by Gavin Mogan                                        *
* -------------------------------------------------------------------------*
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
*							Pirate Skills							       *
****************************************************************************/


#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

void	sound_to_ship			args	( ( SHIP_DATA *ship , char *argument ) );

void do_undock( CHAR_DATA *ch, char *argument )
{
	int chance;
    SHIP_DATA *ship;
	SHIP_DATA *target;

    char buf[MAX_STRING_LENGTH];
    
	if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
	{
		send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
		return;
	}

	if ( ship->class > SHIP_PLATFORM )
	{
		send_to_char("&RThis isn't a spacecraft!\n\r",ch);
		return;
	}	        
	
	if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
	{
		send_to_char("&RThe controls must be at the pilots chair...\n\r",ch);
		return;
	}

	target = ship_from_entrance( ship->docked2 );
	if (  target == NULL || target == ship)
	{
		send_to_char("&RThat ship isn't here!\n\r",ch);
		return;
	}
 
	if ( autofly(ship) )
	{
		send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
		return;
	}
    	        
	if  ( ship->class == SHIP_PLATFORM )
	{
		send_to_char( "&RPlatforms can't move!\n\r" , ch );
		return;
	}   
        
	if (ship->shipstate == SHIP_HYPERSPACE)
	{
		send_to_char("&RYou can only do that in realspace!\n\r",ch);
		return;   
	}
	
	if (ship->shipstate == SHIP_DISABLED)
	{
		send_to_char("&RThe ships drive is disabled. Unable to accelerate.\n\r",ch);
		return;
	}
	
	if (ship->shipstate == SHIP_DOCKED)
	{
		send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
		return;
	}
	
	if (ship->hatchopen)
	{
		ship->hatchopen = FALSE;
		sprintf( buf , "The hatch on %s closes." , ship->name);  
		echo_to_room( AT_YELLOW , get_room_index(ship->docked2) , buf );
		echo_to_room( AT_YELLOW , get_room_index(ship->entrance) , "The hatch slides shut." );
		sound_to_room( get_room_index(ship->entrance) , "!!SOUND(door  U=http://mercury.spaceports.com/~gavin1/)" );
		sound_to_room( get_room_index(ship->docked2) , "!!SOUND(door U=http://mercury.spaceports.com/~gavin1/)" );
	}    	        

	if ( ship->class == FIGHTER_SHIP )
		chance = IS_NPC(ch) ? ch->top_level
		: (int)  (ch->pcdata->learned[gsn_starfighters]) ;
	if ( ship->class == MIDSIZE_SHIP )
		chance = IS_NPC(ch) ? ch->top_level
		: (int)  (ch->pcdata->learned[gsn_midships]) ;            
	if ( ship->class == CAPITAL_SHIP )
		chance = IS_NPC(ch) ? ch->top_level
		: (int) (ch->pcdata->learned[gsn_capitalships]);
	if ( number_percent( ) >= chance )
	{
		send_to_char("&RYou fail to work the controls properly.\n\r",ch);
		if ( ship->class == FIGHTER_SHIP )
			learn_from_failure( ch, gsn_starfighters );
		if ( ship->class == MIDSIZE_SHIP )
			learn_from_failure( ch, gsn_midships );
		if ( ship->class == CAPITAL_SHIP )
			learn_from_failure( ch, gsn_capitalships );
		return;	
	}
                
                      
    act( AT_PLAIN, "$n manipulates the ships controls.", ch,  NULL, argument , TO_ROOM );
    
	sprintf( buf, "&GDisengaging Docking Clamps with %s\n\r", target->name );
	send_to_char( buf, ch);
	sprintf( buf, "The ship docking clamps begin to disengage from %s's hull.", target->name );
    echo_to_cockpit( AT_YELLOW , ship , buf);
    sprintf( buf, "%s's docking clamps begin to disengage from %s's hull.",ship->name,target->name );
    echo_to_system( AT_ORANGE , ship , buf , target );
    
    ship->currspeed = 0;     
    target->currspeed = 0;

	ship->docked2 = target->entrance;
	target->docked2 = ship->entrance;

	target->shipstate = SHIP_SHIP2SHIP_2;
	ship->shipstate = SHIP_SHIP2SHIP_2;

    if ( ship->class == FIGHTER_SHIP )
        learn_from_success( ch, gsn_starfighters );
    if ( ship->class == MIDSIZE_SHIP )
        learn_from_success( ch, gsn_midships );
    if ( ship->class == CAPITAL_SHIP )
        learn_from_success( ch, gsn_capitalships );
  
}

void do_dock(CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
    int chance;
    SHIP_DATA *ship;
    SHIP_DATA *target;
    char buf[MAX_STRING_LENGTH];
    
    strcpy( arg, argument );    
    
    switch( ch->substate )
    { 
   	default:
		one_argument( argument, arg1 );
		
		if (!str_cmp(arg1,"allow"))
		{
			SHIP_DATA *wship;
			argument = one_argument( argument, arg1 );
		
			if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )  
			{
				send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
				return;
			}
			
			if ( ( wship = get_ship( argument ) ) == NULL )
			{
				send_to_char( "No such ship.\n\r", ch);
				return;
			}
			
			if ( nifty_is_name( wship->name , ship->allowdock ) )
			{
				SHIP_DATA *target2;
				strcpy( buf, "" );
				
				for ( target2 = first_ship; target2; target2 = target2->next )
					if ( nifty_is_name( target2->name ,  ship->allowdock ) && target2 != wship )
					{
						strcat ( buf, "\n\r " );
						strcat ( buf, target2->name );
						strcat ( buf, " " );
					}
				
				STRFREE ( ship->allowdock );
				ship->allowdock = STRALLOC( buf );
				ch_printf(ch, "%s is now disallowed to dock with you!", wship->name);
				save_ship(ship);
				return;
			}
			
			strcpy ( buf, ship->allowdock );
			strcat ( buf, "\n\r " );
			strcat ( buf, wship->name );
			strcat ( buf, " " );
			
			STRFREE ( ship->allowdock );
			ship->allowdock = STRALLOC( buf );
			ch_printf(ch, "%s is now allowed to dock with you!", wship->name);
			save_ship(ship);
			return;
		} /* End Allow */

		if (argument[0] == '\0')
		{
			send_to_char("&RYou need to specify a ship to dock with!\n\r",ch);
			return;
		}
			
		if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )  
		{
			send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
			return;
		}
		
		if ( ship->class > SHIP_PLATFORM )
		{
			send_to_char("&RThis isn't a spacecraft!\n\r",ch);
			return;
		}
		
		if ( (ship = ship_from_pilotseat(ch->in_room->vnum)) == NULL )  
		{
			send_to_char("&RYou don't seem to be in the pilot seat!\n\r",ch);
			return;
		}
		
		if (ship->starsystem)
			target = get_ship_here( argument, ship->starsystem );
		
		if ( !target || target == NULL )
		{
			send_to_char("&RThat ship isn't here!\n\r",ch);
			return;
		}
		
		if ( autofly(ship) )
		{
			send_to_char("&RThe ship is set on autopilot, you'll have to turn it off first.\n\r",ch);
			return;
		}
		
		if ( target->shipstate == SHIP_SHIP2SHIP )
		{
			echo_to_cockpit( AT_YELLOW , ship , "&G&W[&R^zERROR&W^x] Target is currently docked!");
			return;
		}
		
		if  ( ship->class == SHIP_PLATFORM )
		{
			send_to_char( "You can't do that here.\n\r" , ch );
			return;
		}
		
		if (  (target->vx > ship->vx + 200) || (target->vx < ship->vx - 200) ||
			(target->vy > ship->vy + 200) || (target->vy < ship->vy - 200) ||
			(target->vz > ship->vz + 200) || (target->vz < ship->vz - 200) )
		{
			echo_to_cockpit( AT_YELLOW , ship , "&G&W[&R^zERROR&W^x] Target is too far away!");
			return;
		}
		if (!nifty_is_name(ship->name, target->allowdock ) )
		{
			set_char_color(AT_RED, ch);
			ch_printf(ch, "%s has not allowed you to dock with them.\n\r If you know the ship code. Use sendcode <shipcode> <ship>.\n\r",target->name);
			return;
		}
		
		if ( !check_pilot( ch , ship ) )
		{
			send_to_char("&RHey, thats not your ship! Try renting a public one.\n\r",ch);
			return;
		}
		
		if ( ship->shipstate == SHIP_DOCKED && ship->shipstate != SHIP_DISABLED  )
		{
			send_to_char("Your Not able to dock at the present time.\n\r",ch);
			return;
		}
		
		if ( target == ship )
		{
			send_to_char("&RYou can't dock your own ship!\n\r",ch);
			return;
		}
		
		chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_shipdocking]);
		
		if ( number_percent( ) < chance )
		{
			send_to_char( "&GDocking Procedure Initated.\n\r", ch);
			act( AT_PLAIN, "$n makes some at the controls, and starts the docking procedure.", ch, NULL, argument , TO_ROOM );
			add_timer ( ch , TIMER_DO_FUN , 1 , do_dock , 1 );
			ch->dest_buf = str_dup(argument);
			return;
		}
		send_to_char("&RYou fail to work the controls properly.\n\r",ch);
		learn_from_failure( ch, gsn_shipdocking);
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
		if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )
			return;    		                                   
		send_to_char("&RYour concentration is broken. Your damm lucky you didn't hit the other ship.\n\r", ch);
		return;
	}
	
	ch->substate = SUB_NONE;
	if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )  
		return;
		
	target = get_ship_here( arg, ship->starsystem );

    if ( !ship || !target || target == NULL || target == ship)
	{
		send_to_char("&RThe ship has left the starsytem. Targeting aborted.\n\r",ch);
		return;
	}
	
	if ( !check_pilot( ch , ship ) )
	{
		send_to_char("&RHey, thats not your ship! Try renting a public one.\n\r",ch);
		return;
	}
    
	if ( ship->shipstate == SHIP_DOCKED && ship->shipstate != SHIP_DISABLED  )
	{
		send_to_char("Your Not able to dock at the present time.\n\r",ch);
		return;
	}

	chance = IS_NPC(ch) ? ch->top_level	: (int) (ch->pcdata->learned[gsn_shipdocking]);
	if ( number_percent( ) < chance )
	{               
		ship->shield = 0;
		ship->autorecharge = FALSE;
		ship->autotrack = FALSE;
		ship->autospeed = FALSE;

		if ( !target || !ship )
			return;

		if (ship->energy == 0)
		{
			send_to_char("&RThis ship has no fuel, try installing a fuel module.\n\r",ch);
			return;
		}	

		ship->currspeed = 0;     
		target->currspeed = 0;

		ship->docked2 = target->entrance;
		target->docked2 = ship->entrance;

		target->shipstate = SHIP_SHIP2SHIP;
		ship->shipstate = SHIP_SHIP2SHIP;
                
		set_char_color( AT_GREEN, ch );
		send_to_char( "Docking sequence initiated.\n\r", ch);
		act( AT_PLAIN, "$n starts to slow the ship and begins the docking sequence.", ch, NULL, argument , TO_ROOM );
		echo_to_ship( AT_YELLOW , ship , "The ship hums as starts to dock.");

		set_char_color( AT_GREEN, ch );
		sprintf( buf , "Docking sequence initiated by %s.\n\r" , ship->name);  
		echo_to_cockpit( AT_GREEN , target , buf );
		echo_to_ship( AT_YELLOW , target, "The ship hums as starts to dock.");
       	      
		sound_to_room( ch->in_room , "!!SOUND(targetlock U=http://mercury.spaceports.com/~gavin1/)" );   	         
		learn_from_success( ch, gsn_shipdocking );

	
		if ( ship->class == FIGHTER_SHIP )
			learn_from_success( ch, gsn_starfighters );
		if ( ship->class == MIDSIZE_SHIP )
			learn_from_success( ch, gsn_midships );
		if ( ship->class == CAPITAL_SHIP )
			learn_from_success( ch, gsn_capitalships );
		sound_to_ship(ship , "!!SOUND(xwing U=http://mercury.spaceports.com/~gavin1/)" );  
		return;   	   	
	}
	set_char_color( AT_RED, ch );
	send_to_char("You fail to work the controls properly!\n\r",ch);
	if ( ship->class == FIGHTER_SHIP )
		learn_from_failure( ch, gsn_starfighters );
	if ( ship->class == MIDSIZE_SHIP )
		learn_from_failure( ch, gsn_midships );
	if ( ship->class == CAPITAL_SHIP )
		learn_from_failure( ch, gsn_capitalships );
	return;
}

/****************************
*paint Command	            *
*Added By Gavin				*
*Feb 17 2000				*
*****************************/

void do_paint(CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
    SHIP_DATA *ship;

	if ( IS_NPC(ch) )
		return;

    argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' )
	{
		send_to_char( "&RUsage: Paint <ship name> <color> <desc>\n\r&w", ch);
		return;   
	}

	if ( arg2[0] == '\0'
		&& ( str_cmp(arg2, "Blood") && str_cmp(arg2, "Green") 
		&& str_cmp(arg2, "DBlue") && str_cmp(arg2, "Cyan") 
		&& str_cmp(arg2, "LRed") && str_cmp(arg2, "LGreen")
		&& str_cmp(arg2, "LBlue") && str_cmp(arg2, "Blue") 
		&& str_cmp(arg2, "Brown") && str_cmp(arg2, "Purple") 
		&& str_cmp(arg2, "Yellow") && str_cmp(arg2, "Pink") 
		&& str_cmp(arg2, "White") )
		) {
		send_to_char( "&RColors Are:\n\r&B",ch);
		send_to_char( "&B<&RBlood&B> <&RGreen&B> <&RDBlue&B> <&RCyan&B> <&RLRed&B> \n\r&w",ch);
		send_to_char( "&B<&RLGreen&B> <&RLBlue&B> <&RBlue&B> <&RBrown&B> <&RPurple&B> \n\r&w",ch);
		send_to_char( "&B<&RYellow&B> <&RPink&B> <&RWhite&B> \n\r&w",ch);
		return;
	}
	if ( argument[0] == '\0' )
	{
		send_to_char( "&RUsage: Paint <ship name> <color> <desc>\n\r&w", ch);
		send_to_char( "&RYou Must include a description\n\r&w",ch);
		return;   
	}

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



	if ( !ship->owner || !ship->pilot || !ship->copilot || check_pilot(ch,ship )) {
			STRFREE( ship->description );
			ship->description = STRALLOC( argument );
			STRFREE( ship->color );
			ship->color = STRALLOC( arg2 );
			save_ship( ship );
			sprintf(buf, "%s has be repainted.", ship->name);
			send_to_char(buf,ch);
			return;
	} 
	else
	{
		sprintf(buf, "Sorry, %s, You are not able to paint that ship, try it when your a pilot.",ch->name);
		send_to_char(buf,ch);
		return;
	}
    do_setship( ch, "" );
    return;
}
/*
 * Get color from text
 */
char *get_colortext( char * color )
{
	if ( color == NULL )
		return "";
	if ( color[0] == '\0' )
		return "";
    if ( !str_cmp(color, "Blood") )
		return "&r";
	if ( !str_cmp(color, "Green") )
		return "&g";
	if ( !str_cmp(color, "DBlue") )
		return "&b";
	if ( !str_cmp(color, "Cyan") )
		return "&c";
	if ( !str_cmp(color, "LRed") )
		return "&R";
	if ( !str_cmp(color, "LGreen") )
		return "&G";
	if ( !str_cmp(color, "Blue") )
		return "&B";
	if ( !str_cmp(color, "LBlue") )
		return "&C";
	if ( !str_cmp(color, "Brown") )
		return "&O";
	if ( !str_cmp(color, "Purple") )
		return "&p";
	if ( !str_cmp(color, "Yellow") )
		return "&Y";
	if ( !str_cmp(color, "Pink") )
		return "&P";
	if ( !str_cmp(color, "White") )
		return "&W";
/*    return "&w";*/
	return "";
}


/****************************
*paint Command	            *
*Added By Gavin				*
*Feb 17 2000				*
*****************************/

void do_vandalize(CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
    SHIP_DATA *ship;
	int chance;

	if ( IS_NPC(ch) )
		return;

    argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' )
	{
		send_to_char( "&RUsage: Paint <ship name> <color> <desc>\n\r&w", ch);
/*		send_to_char( "&RTypes Are:\n\r&B &W<&RProtocol&W> <&RMoverDroid&W> <&RUnknown&W> <&RUnknown&W> <&RUnknown&W> <&RUnknown&W> <&RUnknown&W> \n\r&w",ch);*/
		return;   
	}

	if ( arg2[0] == '\0'
		&& ( str_cmp(arg2, "Blood") && str_cmp(arg2, "Green") 
		&& str_cmp(arg2, "DBlue") && str_cmp(arg2, "Cyan") 
		&& str_cmp(arg2, "LRed") && str_cmp(arg2, "LGreen")
		&& str_cmp(arg2, "LBlue") && str_cmp(arg2, "Blue") 
		&& str_cmp(arg2, "Brown") && str_cmp(arg2, "Purple") 
		&& str_cmp(arg2, "Yellow") && str_cmp(arg2, "Pink") 
		&& str_cmp(arg2, "White") )
		) {
		send_to_char( "&RColors Are:\n\r&B",ch);
		send_to_char( "&B<&RBlood&B> <&RGreen&B> <&RDBlue&B> <&RCyan&B> <&RLRed&B> \n\r&w",ch);
		send_to_char( "&B<&RLGreen&B> <&RLBlue&B> <&RBlue&B> <&RBrown&B> <&RPurple&B> \n\r&w",ch);
		send_to_char( "&B<&RYellow&B> <&RPink&B> <&RWhite&B> \n\r&w",ch);
		return;
	}
	if ( argument[0] == '\0' )
	{
		send_to_char( "&RUsage: Paint <ship name> <color> <desc>\n\r&w", ch);
		send_to_char( "&RYou Must include a description\n\r&w",ch);
		return;   
	}

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
    chance = IS_NPC(ch) ? ch->top_level
		: (int)  (ch->pcdata->learned[gsn_vandalize]) ;
	if ( number_percent( ) < chance )
	{
		STRFREE( ship->description );
		ship->description = STRALLOC( argument );
		STRFREE( ship->color );
		ship->color = STRALLOC( arg2 );
		save_ship( ship );
		sprintf(buf, "%s has be repainted.\n\r", ship->name);
		send_to_char(buf,ch);
		{
			long xpgain;
         
			xpgain = UMIN( 500 ,( exp_level(ch->skill_level[PIRACY_ABILITY]+1) -
				exp_level(ch->skill_level[PIRACY_ABILITY]) ) );
			gain_exp(ch, xpgain, PIRACY_ABILITY);
			ch_printf( ch , "You gain %d Piracy experience.", xpgain );
		}
		learn_from_success( ch, gsn_vandalize );
		return;
	}
	else
	{
		sprintf(buf, "For some reason you miss %s.", ship->name);
		send_to_char(buf,ch);
		learn_from_failure( ch, gsn_vandalize );
		return;
	}
    do_setship( ch, "" );
    return;
}

/****************************
*GetCode Command	        *
*Added By Gavin				*
*Mar 21 2000				*
*****************************/

void do_getcode(CHAR_DATA *ch, char *argument )
{
	SHIP_DATA *ship;
	PLANET_DATA *planet;
	int chance;
	char code[25];

	if (IS_NPC(ch) )
		return;

	if ( !ch->in_room || !ch->in_room->vnum )
	{
		bug("%s is not in a room ",ch->name);
		return;
	}
	if ( ( planet = ch->in_room->area->planet ) != NULL )
	{
		CLAN_DATA *clan;
		chance = IS_NPC(ch) ? ch->top_level	: (int)  (ch->pcdata->learned[gsn_stealcode]) ;

		if ( ch->pcdata->clan )
			clan = ch->pcdata->clan;

		if ( planet->governed_by != clan || clan == NULL )
		{
			if ( number_percent( ) < chance )
			{
				if ( planet->code && planet->code > 0 )
				{
					sprintf(code,"%d",planet->code);
					act( AT_ACTION, "You sit down at the computer console.",
						ch, NULL, NULL, TO_CHAR);
					act( AT_ACTION, "A few moments later, you determin the planet code is $t.", 
						ch, code, NULL, TO_CHAR);
					act( AT_ACTION, "$n sits down at the computer console.",
						ch, NULL, NULL, TO_ROOM);
					learn_from_success( ch, gsn_stealcode );
					return;
				}
				else
				{
					act( AT_ACTION, "You sit down at the computer console.",
						ch, NULL, NULL, TO_CHAR);
					act( AT_ACTION, "A few moments later, you determin that the planet does not have a code set.", 
						ch, NULL, NULL, TO_CHAR);
					act( AT_ACTION, "$n sits down at the computer console.",
						ch, NULL, NULL, TO_ROOM);
					learn_from_success( ch, gsn_stealcode );
					return;
				}
			}
			else
			{
				if ( chance <= 1 )
				{
					send_to_char("This is not your planet\n\r",ch);
					return;
				}
				else
				{
					act( AT_ACTION, "You sit down at the computer console.",ch, NULL, NULL, TO_CHAR);
					act( AT_ACTION, "$n sits down at the computer console.",ch, NULL, NULL, TO_ROOM);
					act( AT_ACTION, "A few moments later, the console comes up flashing \"Access Denied\"",	ch, NULL, NULL, TO_CHAR);
					learn_from_failure( ch, gsn_stealcode );
					return;
				} 
			} 
		} 

		if ( planet->code && planet->code > 0 )
		{
			sprintf(code,"%d",planet->code);
			act( AT_ACTION, "You sit down at the computer console.",
				ch, NULL, NULL, TO_CHAR);
			act( AT_ACTION, "A few moments later, you determin the planet code is $t.", 
				ch, code, NULL, TO_CHAR);
			act( AT_ACTION, "$n sits down at the computer console.",
				ch, NULL, NULL, TO_ROOM);
			return;
		}
	}
	else
	{

		ship = ship_from_cockpit(ch->in_room->vnum);
		if ( ship == NULL )
		{
			set_char_color(AT_RED,ch);
			send_to_char("You can only get the code from the cockpit of the ship!\n\r",ch);
			return;
		}

		chance = IS_NPC(ch) ? ch->top_level	: (int)  (ch->pcdata->learned[gsn_stealcode]) ;

		if ( !check_pilot(ch,ship) && str_cmp(ship->owner,"Public") )
		{
			if ( number_percent( ) < chance )
			{
				if ( ship->code && ship->code > 0 )
				{
					sprintf(code,"%d",ship->code);
					act( AT_ACTION, "You sit down at the computer console.",
						ch, NULL, NULL, TO_CHAR);
					act( AT_ACTION, "A few moments later, you determin the ship code is $t.", 
						ch, code, NULL, TO_CHAR);
					act( AT_ACTION, "$n sits down at the computer console.",
						ch, NULL, NULL, TO_ROOM);
					learn_from_success( ch, gsn_stealcode );
					return;
				}
				else
				{
					act( AT_ACTION, "You sit down at the computer console.",
						ch, NULL, NULL, TO_CHAR);
					act( AT_ACTION, "A few moments later, you determin that the ship does not have a code set.", 
						ch, NULL, NULL, TO_CHAR);
					act( AT_ACTION, "$n sits down at the computer console.",
						ch, NULL, NULL, TO_ROOM);
					learn_from_success( ch, gsn_stealcode );
					return;
				}
			}
			else
			{
				if ( chance <= 1 )
				{
					send_to_char("This is not your ship\n\r",ch);
					return;
				}
				else
				{
					act( AT_ACTION, "You sit down at the computer console.",ch, NULL, NULL, TO_CHAR);
					act( AT_ACTION, "$n sits down at the computer console.",ch, NULL, NULL, TO_ROOM);
					act( AT_ACTION, "A few moments later, the console comes up flashing \"Access Denied\"",	ch, NULL, NULL, TO_CHAR);
					learn_from_failure( ch, gsn_stealcode );
					if ( ship->alarm > 0 )
					{
						CHAR_DATA *owner;
						CHAR_DATA *pilot1;
						CHAR_DATA *pilot2;

						if ( str_cmp("Unowned", ship->owner) && str_cmp("Public", ship->owner) )
						{
							if ( get_clan(ship->owner) == NULL ) 
							{
								if ((owner = get_char_world(ch,ship->owner)) != NULL) 
									ch_printf( owner, "&RSomeone is trying to extract your code from %s!\n\r", ship->name );
								if ( ship->pilot)
									pilot1 = get_char_world(ch,ship->pilot);
								if (pilot1 != NULL) 
									ch_printf( pilot1, "&RSomeone is trying to extract your code from %s!\n\r", ship->name );
								if ( ship->copilot)
									pilot2 = get_char_world(ch,ship->copilot);
								if (pilot2 != NULL) 
									ch_printf( pilot2, "&RSomeone is trying to extract your code from %s!\n\r", ship->name );
							}
						} /* if ship is not onowned or public */
					}/* if ship has alarms */
					return;
				} /* else - if ( chance <= 1 ) */
			} /* else - !check_pilot(ch,ship) && str_cmp(ship->owner,"Public") */
		} /* if ship is not owned by them or public */

		if ( ship->code && ship->code > 0 )
		{
			set_char_color(AT_RED,ch);
			ch_printf(ch,"The Code is %d!\n\r",ship->code);
			return;
		}
	}
}

/****************************
*SetCode Command	        *
*Added By Gavin				*
*Mar 21 2000				*
*****************************/

void do_setcode(CHAR_DATA *ch, char *argument )
{
	SHIP_DATA *ship;
	PLANET_DATA *planet;
	int value;
	char arg1[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];

	argument = one_argument( argument, arg1 );

	value = atoi(arg1);

	if ( IS_NPC(ch) || !ch->pcdata )
	{
		do_say( ch, "I'm not allowed to do this" );
		return;
	}

	if ( !ch->in_room || !ch->in_room->vnum || !ch->in_room->area )
	{
		bug("%s is not in a room ",ch->name);
		return;
	}

	if ( ( planet = ch->in_room->area->planet ) != NULL )
	{
		CLAN_DATA *clan;
		if ( !ch->pcdata->clan )
		{
			send_to_char("Please Join A clan before attempting to set the planetary code.",ch );
			return;
		}
		
		clan = ch->pcdata->clan;

		if ( (ch->pcdata && ch->pcdata->bestowments
			&& is_name("setcode", ch->pcdata->bestowments))
			|| !str_cmp( ch->name, clan->leader  )
			|| !str_cmp( ch->name, clan->number1 )
			|| !str_cmp( ch->name, clan->number2 ) ) 
			;
		else
		{
			send_to_char( "Huh?\n\r", ch );
			return;
		}
		if ( planet->governed_by != clan )
		{
			send_to_char("&W[&R^zERROR&W^x] You do not have access to change the code.\n\r",ch);
			return;
		}
		if ( value && value > 0 )
		{
			planet->code = value;
			set_char_color(AT_RED,ch);
			sprintf(buf, "The Code is now %d!\n\r", planet->code);
			send_to_char(buf,ch);
			save_planet( planet );
			return;
		}
		else
		{
			set_char_color(AT_RED,ch);
			send_to_char("The code must be greater then Zero\n\r",ch);
			return;
		}
		
	}
	else
	{
		ship = ship_from_cockpit(ch->in_room->vnum);
		if ( ship == NULL )
		{
			set_char_color(AT_RED,ch);
			send_to_char("You can only set the code from the cockpit of the ship!\n\r",ch);
			return;
		}

		if ( !check_pilot(ch,ship) || !str_cmp(ship->owner,"Public") )
		{
			send_to_char("This is not your ship\n\r",ch);
			return;
		}

		if ( value && value > 0 )
		{
			ship->code = value;
			set_char_color(AT_RED,ch);
			sprintf(buf, "The Code is now %d!\n\r", ship->code);
			send_to_char(buf,ch);
			return;
		}
		else
		{
			set_char_color(AT_RED,ch);
			send_to_char("The code must be greater then Zero\n\r",ch);
			return;
		}
	}
}

void do_sendcode (CHAR_DATA *ch, char *argument ) 
{
	SHIP_DATA *ship;
	int value;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	one_argument( argument, arg3 );

	value = atoi(arg3);

	if (IS_NPC(ch) )
		return;

	if ( !ch->in_room || !ch->in_room->vnum )
	{
		bug("%s is not in a room ",ch->name);
		return;
	}
	if ( arg1[0] == '\0')
	{
		set_char_color(AT_ACTION,ch);
		send_to_char("Usage:\n\r",ch);
		send_to_char("  For Docking: sendcode docking 'ship' code\n\r",ch);
		send_to_char("  For Opening The Bay: sendcode bay 'ship' code\n\r",ch);
		send_to_char("  For Planet Shield: sendcode shield 'planet' code\n\r",ch);
		return;
	}
	
	ship = ship_from_cockpit(ch->in_room->vnum);
	if ( ship == NULL )
	{
		set_char_color(AT_RED,ch);
		send_to_char("You can only send the code from the cockpit of the ship!\n\r",ch);
		return;
	}

	if ( !str_cmp(arg1, "Docking" ) )
	{
		SHIP_DATA *wship;
		if ( ( wship = get_ship( arg2 ) ) == NULL )
		{
			send_to_char( "No such ship.\n\r", ch);
			return;
		}
		
		if ( value <= 0 )
		{
			send_to_char( "&G&W[&RWarning&G&W] Must transmit a code greater than zero.\n\r",ch);
			return;
		}
		
		if ( wship->code != value ) 
		{
			send_to_char( "&G&W[&RERROR&G&W] Invalid Code! Please Try Again.\n\r",ch);
			return;
		}
		
		if ( nifty_is_name( ship->name , wship->allowdock ) )
		{
			send_to_char( "&R^zInvalid Option!\n\r&G&W&W^xThis ship is able to dock already\n\r",ch);
			return;
		}
		strcpy ( buf, wship->allowdock );
		strcat ( buf, "\n\r " );
		strcat ( buf, ship->name );
		strcat ( buf, " " );
		
		STRFREE ( wship->allowdock );
		wship->allowdock = STRALLOC( buf );
		ch_printf(ch, "&R^zValid Option!\n\r&G&W&W^xDocking Allowed with %s\n\r", wship->name);
		return;
	}
	else if ( !str_cmp(arg1, "bay" ) )
	{
		SHIP_DATA *wship;
		if ( ( wship = get_ship( arg2 ) ) == NULL )
		{
			send_to_char( "No such ship.\n\r", ch);
			return;
		}
		
		if ( value <= 0 )
		{
			send_to_char( "&G&W[&RWarning&G&W] Must transmit a code greater than zero.\n\r",ch);
			return;
		}
		
		if ( wship->code != value ) 
		{
			send_to_char( "&G&W[&RERROR&G&W] Invalid Code! Please Try Again.\n\r",ch);
			return;
		}
		
		if ( IS_SET( wship->flags, SHIP_BAYOPEN ))
		{
			TOGGLE_BIT( wship->flags, SHIP_BAYOPEN );
			wship->bayopen = FALSE;
			sprintf( buf, "!!SOUND(BayClose U=http://mercury.spaceports.com/~gavin1/)Closing %s's bay doors." , wship->name );
            echo_to_system( AT_RED , wship , buf , NULL );
			sound_to_ship(wship , "!!SOUND(BayClose U=http://mercury.spaceports.com/~gavin1/)" );
			sprintf( buf, "Closing bay doors.");
			echo_to_ship( AT_RED , wship , buf  );
			save_ship(wship);
		}
		else
		{
			wship->bayopen = TRUE;
			TOGGLE_BIT( wship->flags, SHIP_BAYOPEN );
			sprintf( buf, "!!SOUND(BayClose U=http://mercury.spaceports.com/~gavin1/)Opening %s's bay doors." , wship->name );
            echo_to_system( AT_RED , wship , buf , NULL );
			sound_to_ship(wship , "!!SOUND(BayClose U=http://mercury.spaceports.com/~gavin1/)" );
			sprintf( buf, "Opening bay doors.");
			echo_to_ship( AT_RED , wship , buf );
			save_ship(wship);
		}
		send_to_char( "&G&W[&GSUCCESS&G&W] Bay Door Control Toggled.\n\r",ch);
		return;
	}
	else if ( !str_cmp(arg1, "shield" ) )
	{
		PLANET_DATA *planet;
		if ( ( planet = get_planet( arg2 ) ) == NULL )
		{
			send_to_char( "No such planet.\n\r", ch);
			return;
		}
		if ( !planet->x || !planet->y || !planet->z || !planet->starsystem )
		{
			send_to_char("Planet is bugged, Notify a imm.\n\r",ch);
			bug("Planet: %s doesn't work for sendcode", planet->name);
			return;
		}
		
		if (  (planet->x > ship->vx + 200) || (planet->x < ship->vx - 200) ||
			(planet->y > ship->vy + 200) || (planet->y < ship->vy - 200) ||
			(planet->z > ship->vz + 200) || (planet->z < ship->vz - 200) ||
			(planet->starsystem != ship->starsystem ) )
		{
			send_to_char("&G&W[&R^zERROR&W^x] &RTarget is too far away!\n\r",ch);
			return;
		}
		
		if ( value <= 0 )
		{
			send_to_char( "&G&W[&RWarning&G&W] Must transmit a code greater than zero.\n\r",ch);
			return;
		}
		
		if ( planet->code != value ) 
		{
			send_to_char( "&G&W[&RERROR&G&W] Invalid Code! Please Try Again.\n\r",ch);
			return;
		}

		if ( planet->shields <= 0 )
		{
			send_to_char("&W[&R^zFAILURE&W^x] Shield Generators Are Unavailable or Destroyed\n\r",ch);
			return;
		}
		
		if ( IS_SET(planet->flags, PLANET_PSHIELD) )
		{
			TOGGLE_BIT(planet->flags, PLANET_PSHIELD);
			send_to_char( "&G&W[&GSUCCESS&G&W] Planetary Shield On.\n\r",ch);
			save_planet( planet );
		}
		else
		{
			TOGGLE_BIT(planet->flags, PLANET_PSHIELD);
			send_to_char( "&G&W[&GSUCCESS&G&W] Planetary Shield Off.\n\r",ch);
			save_planet( planet );
		}
		return;
	}
}

/* Eng Skill to transfer fuel from one ship to another */
void do_refuelship( CHAR_DATA *ch, char *argument )
{
	SHIP_DATA *ship;
	SHIP_DATA *target;
	int value;
	char arg1[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];

	argument = one_argument( argument, arg1 );

	value = atoi(arg1);

	if ( arg1[0] == '\0' ) {
		send_to_char("refuelship <percent>",ch);
		return;
	}

	if (IS_NPC(ch) )
		return;

	if ( !ch->in_room || !ch->in_room->vnum )
	{
		bug("%s is not in a room ",ch->name,0);
		return;
	}

	ship = ship_from_cockpit(ch->in_room->vnum);
	if ( ship == NULL )
	{
		set_char_color(AT_RED,ch);
		send_to_char("You can only send the code from the cockpit of the ship!\n\r",ch);
		return;
	}

	if ( ship->shipstate != SHIP_SHIP2SHIP )
	{
		send_to_char("&R^zInvalid Option&W^x&G&W Transfering fuel can only be done while docked!\n\r",ch);
		return;
	}

	if ( value <= 0 )
		value = 50;

	target = ship_from_entrance( ship->docked2 );
	if (  target == NULL || target == ship)
	{
		send_to_char("&R^zERROR&W^x&G&W Transfering fuel can only be done while docked with another ship!\n\r",ch);
		return;
	}

	target->energy += ship->energy/value;
	ship->energy -= ship->energy/value;

	sprintf(buf,"( %-3d%% ) of your energy has been transfered to %s\n\r",value,target->name);
	echo_to_room(AT_RED,get_room_index(ship->cockpit),buf);

}
