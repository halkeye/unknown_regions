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
*                           Engineering Skills                             *
****************************************************************************/


#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

/* Jarvis says '1 Superconductor, 2 durasteel, 3 circuits, 2 batteries, 1 lens'*/

void do_makedroid( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance, temp;
    bool checktool, checkdura, checkbatt, checkoven, checkcond, checkcirc, checklens;
    OBJ_DATA *obj;
    int vnum, power, scope, ammo;
	MOB_INDEX_DATA  * pMobIndex;
	CHAR_DATA		 * mob;
	int ability;
    
	argument = one_argument( argument, arg );
    strcpy ( arg2, argument);
    
	switch( ch->substate )
	{ 
		default:
			if ( arg2[0] == '\0' )
			{
				send_to_char( "&RUsage: Makedroid <type> <name>\n\r&w", ch);
				send_to_char( "&RTypes Are:\n\r&B &W<&RProtocol&W> <&RMoverDroid&W> <&RUnknown&W> <&RUnknown&W> <&RUnknown&W> <&RUnknown&W> <&RUnknown&W> \n\r&w",ch);
				return;   
			}
			
			if ( arg[0] == '\0' && !str_cmp(arg, "protocol") && !str_cmp(arg, "MoverDroid") ) 
			{
				send_to_char( "&RTypes Are:\n\r&B &W<&RProtocol&W> <&RMoverDroid&W> <&RUnknown&W> <&RUnknown&W> <&RUnknown&W> <&RUnknown&W> <&RUnknown&W> \n\r&w",ch);
				return;
			}
			
			checktool = FALSE;
			checkdura = FALSE;
			checkbatt = FALSE;
			checkoven = FALSE;
			checkcond = FALSE;
			checkcirc = FALSE;
			checklens = FALSE;
			
			for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
			{
				if (obj->item_type == ITEM_TOOLKIT)
					checktool = TRUE;
				if ( (obj->item_type == ITEM_DURAPLAST || obj->item_type == ITEM_DURASTEEL ) && obj->count >= 2 )
					checkdura = TRUE;
				if (obj->item_type == ITEM_BATTERY && obj->count >= 2)
					checkbatt = TRUE;
				if (obj->item_type == ITEM_LENS)
					checklens = TRUE;
				if (obj->item_type == ITEM_OVEN)
					checkoven = TRUE;
				if (obj->item_type == ITEM_CIRCUIT && obj->count >= 3)
					checkcirc = TRUE;
				if (obj->item_type == ITEM_SUPERCONDUCTOR)
					checkcond = TRUE;                  
			}
			
			if ( !checktool )
			{
				send_to_char( "&RYou need toolkit to make a droid.\n\r", ch);
				return;
			}
			
			if ( !checkdura )
			{
				send_to_char( "&RYou need at least 2 pieces of metal it out of.\n\r", ch);
				return;
			}
			
			if ( !checkbatt )
			{
				send_to_char( "&RYou need at least 2 power sources for your droid.\n\r", ch);
				return;
			}
			
			if ( !checkoven )
			{
				send_to_char( "&RYou need a small furnace to heat the plastics.\n\r", ch);
				return;
			}
			
			if ( !checkcirc )
			{
				send_to_char( "&RYou need at least 3 circuit boards to create the droid.\n\r", ch);
				return;
			}
			
			if ( !checkcond )
			{
				send_to_char( "&RYou still need a small superconductor.\n\r", ch);
				return;
			}
			
			chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makedroid]);
			if ( number_percent( ) < chance )
			{
				send_to_char( "&GYou begin the long process of making a droid.\n\r", ch);
				act( AT_PLAIN, "$n takes $s tools and a large blast furnace and begins to work on something.", ch, NULL, argument , TO_ROOM );
				
				add_timer ( ch , TIMER_DO_FUN , 10 , do_makedroid , 1 );
				ch->dest_buf = str_dup(arg);
				ch->dest_buf_2 = str_dup(arg2);
				return;
			}
			send_to_char("&RYou can't figure out how to fit the parts together.\n\r",ch);
			learn_from_failure( ch, gsn_makedroid );
			return;
		
		case 1: 
			if ( !ch->dest_buf )
				return;
			if ( !ch->dest_buf_2 )
				return;
			strcpy(arg, ch->dest_buf);
			DISPOSE( ch->dest_buf);
			strcpy(arg2, ch->dest_buf_2);
			DISPOSE( ch->dest_buf_2);
			break;
			
		case SUB_TIMER_DO_ABORT:
			DISPOSE( ch->dest_buf );
			DISPOSE( ch->dest_buf_2 );
			ch->substate = SUB_NONE;    		                                   
			send_to_char("&RYou are interupted and fail to finish your work.\n\r", ch);
			return;
	}
	
	ch->substate = SUB_NONE;

    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makedroid]);
	vnum = MOB_VNUM_DROID;

	if (arg)
		bug("arg = %s",arg);
	/*if (!str_cmp(arg, "Protocol") )
		vnum = 6;
	if (!str_cmp(arg, "Unknown") )
		vnum = 250;
	else */
		vnum = 6;

    if ( (pMobIndex = get_mob_index(vnum)) == NULL ) 
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration of this error.\n\r", ch );
         return;
    }

	checktool = FALSE;
	checkdura = FALSE;
	checkbatt = FALSE;
	checkoven = FALSE;
	checkcond = FALSE;
	checkcirc = FALSE;
	checklens = FALSE;
    power     = 0;
    scope     = 0;
    ammo = 0;
    
	for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
	{
		if (obj->item_type == ITEM_TOOLKIT)
		{
			checktool = TRUE;
		}
		if (obj->item_type == ITEM_OVEN)
		{
			checkoven = TRUE;
		}
		if ( ( obj->item_type == ITEM_DURAPLAST || obj->item_type == ITEM_DURASTEEL ) && checkdura == FALSE)
		{
			checkdura = TRUE;
			separate_obj( obj );
			obj_from_char( obj );
			extract_obj( obj );
		}
		if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE)
		{
			checkbatt = TRUE;
			separate_obj( obj );
			obj_from_char( obj );
			extract_obj( obj );
		}
		if (obj->item_type == ITEM_LENS && checklens == FALSE )
		{
			checklens = TRUE;
			separate_obj( obj );
			obj_from_char( obj );
			extract_obj( obj );
			scope++;
		}
		if (obj->item_type == ITEM_SUPERCONDUCTOR && power<2)
		{
			checkcond = TRUE;
			power++;
			separate_obj( obj );
			obj_from_char( obj );
			extract_obj( obj );
		}
		if (obj->item_type == ITEM_CIRCUIT && checkcirc == FALSE)
		{
			checkcirc = TRUE;
			separate_obj( obj );
			obj_from_char( obj );
			extract_obj( obj );
		}
	}           
    chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makedroid]) ;
	temp = number_percent( );           
	if ( temp > chance*2 || ( !checktool ) || ( !checkdura ) || ( !checkbatt ) || ( !checkoven )
		|| ( !checkcond ) || ( !checkcirc ))		
	{
		send_to_char( "&RThis is here temporary until gavin figures out exactly what to put here\n\r", ch);
		send_to_char( "&RYour droid starts to power up.\n\r", ch);
		send_to_char( "&RIt says&C 'Greetings What Is My Purpose'&R.\n\r", ch);
		send_to_char( "&RSuddenly your droid starts to smoke.\n\rThe next thing you know, Your droid is in peices on the floor.\n\r", ch);
		learn_from_failure( ch, gsn_makedroid );
		return;
	}
	
	mob = create_mobile( pMobIndex );
	char_to_room( mob, ch->in_room );
	strcpy( buf, arg2 );
	STRFREE( mob->name );
	/*if (!str_cmp(arg, "Protocol") )
		mob->name = STRALLOC( "A Protocol Droid" );
	else
		mob->name = STRALLOC( "A droid");*/
	mob->name = STRALLOC( buf );
	STRFREE( mob->long_descr );
	mob->long_descr = STRALLOC( buf ); 
	STRFREE( mob->short_descr );
	mob->short_descr = STRALLOC( buf ); 
	mob->leader = ch;
	
	if (!str_cmp(arg, "Protocol") )
		mob->spec_fun = spec_lookup("spec_droid_lang"); 
	if (ch->pcdata && ch->pcdata->clan )
		mob->mob_clan = ch->pcdata->clan;
	ch->pcdata->pet = mob;
	act( AT_IMMORT, "$N has been created.", ch, NULL, mob, TO_ROOM );
	send_to_char( "You have finished making your droid.\n\r", ch );
	mob->top_level = ch->skill_level[ENGINEERING_ABILITY];
	for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )
		mob->skill_level[ability] = mob->top_level;
	mob->hit = mob->top_level*10;
	mob->max_hit = mob->hit;
	mob->armor = 100- mob->top_level*1.5;
	mob->damroll = mob->top_level/3;
	mob->hitroll = mob->top_level/3;
	SET_BIT( mob->act , ACT_DROID );
	if ( mob->master )
		stop_follower( mob );
	add_follower( mob, ch );
	SET_BIT( mob->affected_by, AFF_CHARM );
	SET_BIT( mob->act, ACT_PET);
	
	send_to_char( "&GYou finish your work and stand back to look at your newly created droid.&w\n\r", ch);
	act( AT_PLAIN, "$n finishes making $s new droid.", ch, NULL, argument , TO_ROOM );
	learn_from_success( ch, gsn_makedroid );
}

void do_makefurniture( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance;
    bool checksew, checkfab; 
    OBJ_DATA *obj;
    OBJ_DATA *material;
            
    argument = one_argument( argument, arg );
    strcpy( arg2 , argument );
	
    switch( ch->substate )
    { 
	default:
    	        
		if ( arg2[0] == '\0' )
		{
			send_to_char( "&RUsage: Makefurniture <type> <name>\n\r&w", ch);
			send_to_char( "&RTypes Are:\n\r&B <bed> <chair> <desk> <chest> <drawer> <sink>\n\r<cabinets> <fridgerator> <sofa> <recliner> <holo-vid>\n\r&w",ch);
			return;   
		}

		if ( ( arg[0] == '\0' )
			&& str_cmp(arg, "bed") && str_cmp(arg, "chair") && str_cmp(arg, "desk")
			&& str_cmp(arg, "chest") && str_cmp(arg, "drawer") && str_cmp(arg, "sink")
			&& str_cmp(arg, "cabinets") && str_cmp(arg, "fridgerator") && str_cmp(arg, "sofa")
			&& str_cmp(arg, "recliner")	&& str_cmp(arg, "holo-vid")
			) {
			send_to_char( "&RTypes Are:\n\r&B <bed> <chair> <desk> <chest> <drawer> <sink>\n\r<cabinets> <fridgerator> <sofa> <recliner> <holo-vid>\n\r&w",ch);
			return;
		}
 
    	        checksew = FALSE;
                checkfab = FALSE;
        
                if ( !xIS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RYou need to be in a factory or workshop to do that.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TIMBER)
                    checkfab = TRUE;
                  if (obj->item_type == ITEM_TOOLKIT)
          	    checksew = TRUE;
                }
                
                if ( !checkfab )
                {
                   send_to_char( "&RYou need some sort of building material.\n\r", ch);
                   return;
                }
 
                if ( !checksew )
                {
                   send_to_char( "&RYou need the propertools.\n\r", ch);
                   return;
                }

    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makefurniture]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of creating a piece of furniture.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s tools and some building material and begins to work.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 10 , do_makefurniture , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   ch->dest_buf_2 = str_dup(arg2);
    		   return;
	        }
	        send_to_char("&RYou can't figure out what to do.\n\r",ch);
	        learn_from_failure( ch, gsn_makefurniture );
    	   	return;	
    	
    	case 1: 
    		if ( !ch->dest_buf )
    		     return;
    		if ( !ch->dest_buf_2 )
    		     return;
    		strcpy(arg, ch->dest_buf);
    		DISPOSE( ch->dest_buf);
    		strcpy(arg2, ch->dest_buf_2);
    		DISPOSE( ch->dest_buf_2);
    		break;
    		
    	case SUB_TIMER_DO_ABORT:
    		DISPOSE( ch->dest_buf );
    		DISPOSE( ch->dest_buf_2 );
    		ch->substate = SUB_NONE;    		                                   
    	        send_to_char("&RYou are interupted and fail to finish your work.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makefurniture]);
    
    checksew = FALSE;
    checkfab = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checksew = TRUE;
       if (obj->item_type == ITEM_TIMBER && checkfab == FALSE)
       {
          checkfab = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          material = obj;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makefurniture]) ;
                
    if ( number_percent( ) > chance*2  || ( !checkfab ) || ( !checksew ) )
    {
       send_to_char( "&RYou stand back and look at your newly created furniture.\n\r", ch);
       send_to_char( "&RIt suddenly dawns upon you that you have created the most useless\n\r", ch);
       send_to_char( "&Rpiece of furniture you've ever seen. You quickly hide your mistake...\n\r", ch);
       learn_from_failure( ch, gsn_makefurniture );
       return;
    }

    obj = material; 

	if ( !str_cmp(arg, "bed") )
	{
		obj->item_type = ITEM_FURNITURE;
		CLEAR_BITS(obj->wear_flags);
		SET_BIT( obj->wear_flags, ITEM_TAKE );
		obj->level = level;
		STRFREE( obj->name );
		strcpy( buf, arg2 );
		obj->name = STRALLOC( buf );
		strcpy( buf, arg2 );
		STRFREE( obj->short_descr );
		obj->short_descr = STRALLOC( buf );        
		STRFREE( obj->description );
		strcat( buf, " was dropped here." );
		obj->description = STRALLOC( buf );
		obj->value[0] = level;
		obj->value[1] = 0;
		obj->value[2] = 585;      
		obj->value[3] = 10;      
		obj->cost *= 2;
		obj->weight = 250;
	}
	if ( !str_cmp(arg, "chair") )
	{
		obj->item_type = ITEM_FURNITURE;
		CLEAR_BITS(obj->wear_flags);
		SET_BIT( obj->wear_flags, ITEM_TAKE );
		obj->level = level;
		STRFREE( obj->name );
		strcpy( buf, arg2 );
		obj->name = STRALLOC( buf );
		strcpy( buf, arg2 );
		STRFREE( obj->short_descr );
		obj->short_descr = STRALLOC( buf );        
		STRFREE( obj->description );
		strcat( buf, " was dropped here." );
		obj->description = STRALLOC( buf );
		obj->value[0] = level;
		obj->value[1] = 0;
		obj->value[2] = 9;      
		obj->value[3] = 10;      
		obj->cost *= 2;
		obj->weight = 250;
	}
	if ( !str_cmp(arg, "desk") )
	{
		obj->item_type = ITEM_FURNITURE;
		CLEAR_BITS(obj->wear_flags);
		SET_BIT( obj->wear_flags, ITEM_TAKE );
		obj->level = level;
		STRFREE( obj->name );
		strcpy( buf, arg2 );
		obj->name = STRALLOC( buf );
		strcpy( buf, arg2 );
		STRFREE( obj->short_descr );
		obj->short_descr = STRALLOC( buf );        
		STRFREE( obj->description );
		strcat( buf, " was dropped here." );
		obj->description = STRALLOC( buf );
		obj->value[0] = level;
		obj->value[1] = 0;
		obj->value[2] = 521;      
		obj->value[3] = 10;      
		obj->cost *= 2;
		obj->weight = 250;
	}
	if ( !str_cmp(arg, "chest") )
	{
		obj->item_type = ITEM_FURNITURE;
		CLEAR_BITS(obj->wear_flags);
		SET_BIT( obj->wear_flags, ITEM_TAKE );
		obj->level = level;
		STRFREE( obj->name );
		strcpy( buf, arg2 );
		obj->name = STRALLOC( buf );
		strcpy( buf, arg2 );
		STRFREE( obj->short_descr );
		obj->short_descr = STRALLOC( buf );        
		STRFREE( obj->description );
		strcat( buf, " was dropped here." );
		obj->description = STRALLOC( buf );
		obj->value[0] = level;
		obj->value[1] = 0;
		obj->value[2] = 1170;      
		obj->value[3] = 10;      
		obj->cost *= 2;
		obj->weight = 250;
	}
	if ( !str_cmp(arg, "drawer") )
	{
		obj->item_type = ITEM_FURNITURE;
		CLEAR_BITS(obj->wear_flags);
		SET_BIT( obj->wear_flags, ITEM_TAKE );
		obj->level = level;
		STRFREE( obj->name );
		strcpy( buf, arg2 );
		obj->name = STRALLOC( buf );
		strcpy( buf, arg2 );
		STRFREE( obj->short_descr );
		obj->short_descr = STRALLOC( buf );        
		STRFREE( obj->description );
		strcat( buf, " was dropped here." );
		obj->description = STRALLOC( buf );
		obj->value[0] = level;
		obj->value[1] = 0;
		obj->value[2] = 1170;   
		obj->value[3] = 10;      
		obj->cost *= 2;
		obj->weight = 250;
	}
	if ( !str_cmp(arg, "sink") )
	{
		obj->item_type = ITEM_FURNITURE;
		CLEAR_BITS(obj->wear_flags);
		SET_BIT( obj->wear_flags, ITEM_TAKE );
		obj->level = level;
		STRFREE( obj->name );
		strcpy( buf, arg2 );
		obj->name = STRALLOC( buf );
		strcpy( buf, arg2 );
		STRFREE( obj->short_descr );
		obj->short_descr = STRALLOC( buf );        
		STRFREE( obj->description );
		strcat( buf, " was dropped here." );
		obj->description = STRALLOC( buf );
		obj->value[0] = level;
		obj->value[1] = 0;
		obj->value[2] = 0;      
		obj->value[3] = 10;      
		obj->cost *= 2;
		obj->weight = 250;
	}
	if ( !str_cmp(arg, "cabinets") )
	{
		obj->item_type = ITEM_FURNITURE;
		CLEAR_BITS(obj->wear_flags);
		SET_BIT( obj->wear_flags, ITEM_TAKE );
		obj->level = level;
		STRFREE( obj->name );
		strcpy( buf, arg2 );
		obj->name = STRALLOC( buf );
		strcpy( buf, arg2 );
		STRFREE( obj->short_descr );
		obj->short_descr = STRALLOC( buf );        
		STRFREE( obj->description );
		strcat( buf, " was dropped here." );
		obj->description = STRALLOC( buf );
		obj->value[0] = level;
		obj->value[1] = 0;
		obj->value[2] = 1170;  
		obj->value[3] = 10;      
		obj->cost *= 2;
		obj->weight = 250;
	}
	if ( !str_cmp(arg, "fridgerator") )
	{
		obj->item_type = ITEM_FURNITURE;
		CLEAR_BITS(obj->wear_flags);
		SET_BIT( obj->wear_flags, ITEM_TAKE );
		obj->level = level;
		STRFREE( obj->name );
		strcpy( buf, arg2 );
		obj->name = STRALLOC( buf );
		strcpy( buf, arg2 );
		STRFREE( obj->short_descr );
		obj->short_descr = STRALLOC( buf );        
		STRFREE( obj->description );
		strcat( buf, " was dropped here." );
		obj->description = STRALLOC( buf );
		obj->value[0] = level;
		obj->value[1] = 0;
		obj->value[2] = 1170; 
		obj->value[3] = 10;      
		obj->cost *= 2;
		obj->weight = 400;
	}
	if ( !str_cmp(arg, "sofa") )
	{
		obj->item_type = ITEM_FURNITURE;
		CLEAR_BITS(obj->wear_flags);
		SET_BIT( obj->wear_flags, ITEM_TAKE );
		obj->level = level;
		STRFREE( obj->name );
		strcpy( buf, arg2 );
		obj->name = STRALLOC( buf );
		strcpy( buf, arg2 );
		STRFREE( obj->short_descr );
		obj->short_descr = STRALLOC( buf );        
		STRFREE( obj->description );
		strcat( buf, " was dropped here." );
		obj->description = STRALLOC( buf );
		obj->value[0] = level;
		obj->value[1] = 0;
		obj->value[2] = 585; 
		obj->value[3] = 10;      
		obj->cost *= 2;
		obj->weight = 250;
	}
	if ( !str_cmp(arg, "recliner") )
	{
		obj->item_type = ITEM_FURNITURE;
		CLEAR_BITS(obj->wear_flags);
		SET_BIT( obj->wear_flags, ITEM_TAKE );
		obj->level = level;
		STRFREE( obj->name );
		strcpy( buf, arg2 );
		obj->name = STRALLOC( buf );
		strcpy( buf, arg2 );
		STRFREE( obj->short_descr );
		obj->short_descr = STRALLOC( buf );        
		STRFREE( obj->description );
		strcat( buf, " was dropped here." );
		obj->description = STRALLOC( buf );
		obj->value[0] = level;
		obj->value[1] = 0;
		obj->value[2] = 585;      
		obj->value[3] = 10;      
		obj->cost *= 2;
		obj->weight = 250;
	}
	if ( !str_cmp(arg, "holo-vid") )
	{
		obj->item_type = ITEM_FURNITURE;
		CLEAR_BITS(obj->wear_flags);
		SET_BIT( obj->wear_flags, ITEM_TAKE );
		obj->level = level;
		STRFREE( obj->name );
		strcpy( buf, arg2 );
		obj->name = STRALLOC( buf );
		strcpy( buf, arg2 );
		STRFREE( obj->short_descr );
		obj->short_descr = STRALLOC( buf );        
		STRFREE( obj->description );
		strcat( buf, " was dropped here." );
		obj->description = STRALLOC( buf );
		obj->value[0] = level;
		obj->value[1] = 0;
		obj->value[2] = 1170;      
		obj->value[3] = 10;      
		obj->cost *= 2;
		obj->weight = 250;
	}
	if ( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
		REMOVE_BIT( obj->extra_flags, ITEM_PROTOTYPE );
	obj = obj_to_char( obj, ch );
	send_to_char( "&GYou finish your work and hold up your newly created piece of furniture.&w\n\r", ch);
	act( AT_PLAIN, "$n finishes creating a piece of furniture.", ch, NULL, argument , TO_ROOM );
	{
		long xpgain;
		
		xpgain = UMIN( obj->cost*100 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
		gain_exp(ch, xpgain, ENGINEERING_ABILITY);
		ch_printf( ch , "You gain %d engineering experience.", xpgain );
	}
	learn_from_success( ch, gsn_makefurniture );
}

void do_makekeyring( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance;
    bool checktool, checkoven, checkplastic; 
    OBJ_DATA *obj;
    OBJ_DATA *plastic;
    int value, cost;
	
    strcpy( arg , argument );
    
    switch( ch->substate )
    { 
	default:
		
		if ( arg[0] == '\0' )
		{
			send_to_char( "&RUsage: Makekeyring <name>\n\r&w", ch);
			return;   
		}
		
		checktool = FALSE;
		checkoven = FALSE;
		checkplastic = FALSE;
        
		if ( !xIS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
		{
			send_to_char( "&RYou need to be in a factory or workshop to do that.\n\r", ch);
			return;
		}
		
		for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
		{
			if (obj->item_type == ITEM_TOOLKIT)
				checktool = TRUE;
			if (obj->item_type == ITEM_OVEN)
				checkoven = TRUE;
			if (obj->item_type == ITEM_DURAPLAST)
				checkplastic = TRUE;
		}
		
		if ( !checktool )
		{
			send_to_char( "&RYou need a toolkit.\n\r", ch);
			return;
		}
		
		if ( !checkoven )
		{
			send_to_char( "&RYou need an oven.\n\r", ch);
			return;
		}
		
		if ( !checkplastic )
		{
			send_to_char( "&RYou need some duraplast.\n\r", ch);
			return;
		}

		chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makekeyring]);
		if ( number_percent( ) < chance )
		{
			send_to_char( "&GYou begin the long process of creating a keyring.\n\r", ch);
			act( AT_PLAIN, "$n takes $s toolkit and some duraplast and begins to work.", ch, NULL, argument , TO_ROOM );
			add_timer ( ch , TIMER_DO_FUN , 15 , do_makekeyring , 1 );
			ch->dest_buf   = str_dup(arg);
			return;
		}
		send_to_char("&RYou can't figure out what to do.\n\r",ch);
		learn_from_failure( ch, gsn_makekeyring );
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
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makekeyring]);
    
    checkplastic = FALSE;
    checkoven = FALSE;
    checktool = FALSE;
    value=0;
    cost=0;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
		if (obj->item_type == ITEM_TOOLKIT)
			checktool = TRUE;
		if (obj->item_type == ITEM_OVEN)
			checkoven = TRUE;
		if (obj->item_type == ITEM_DURAPLAST && checkplastic == FALSE)
		{
			checkplastic = TRUE;
			separate_obj( obj );
			obj_from_char( obj );
			plastic = obj;
		}
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makekeyring]) ;
	
    if ( number_percent( ) > chance*2  || ( !checkoven ) || ( !checktool ) || ( !checkplastic ) )
    {
		send_to_char( "&RYou hold up your newly created keyring.\n\r", ch);
		send_to_char( "&RIt suddenly dawns upon you that you have created the most useless\n\r", ch);
		send_to_char( "&Rpiece of junk you've ever seen. You quickly hide your mistake...\n\r", ch);
		learn_from_failure( ch, gsn_makekeyring );
		return;
    }

    obj = plastic; 

    obj->item_type = ITEM_KEYRING;
	CLEAR_BITS(obj->wear_flags);
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    STRFREE( obj->name );
    strcpy( buf, arg );
    obj->name = STRALLOC( buf );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was dropped here." );
    obj->description = STRALLOC( buf );
    obj->value[0] = obj->value[1];      
    obj->cost *= 10;
    obj->cost += cost;
	if ( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
		REMOVE_BIT( obj->extra_flags, ITEM_PROTOTYPE );
	
    obj = obj_to_char( obj, ch );
	
    send_to_char( "&GYou finish your work and hold up your newly created keyring.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes making a new keyring.", ch, NULL, argument , TO_ROOM );
    
    {
		long xpgain;
		xpgain = UMIN( obj->cost*100 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
		gain_exp(ch, xpgain, ENGINEERING_ABILITY);
		ch_printf( ch , "You gain %d engineering experience.", xpgain );
	}
	learn_from_success( ch, gsn_makekeyring );
}

void do_upgradeship( CHAR_DATA *ch, char *argument )
{
	return;
}
#if 0
	char buf[MAX_STRING_LENGTH];
/*	char arg[MAX_STRING_LENGTH];
    int chance;*/
	bool upgrade;
    OBJ_DATA *obj;

	SHIP_DATA * ship;

	if ( ( ship = ship_from_engine( ch->in_room->vnum ) ) == NULL )
	{
		send_to_char("This can only be done in the engine room\n\r",ch);
		return;
	}

	if ( !check_pilot(ch,ship) )
	{
		send_to_char("This is not your ship.\n\r",ch);
		return;
	}
	if ( !ship->prototype )
	{
		send_to_char("This is a custom ship. Unable to upgrade.\n\r",ch);
		return;
	}

	if ( argument[0] == '\0' )
	{
		sprintf(buf, 
			"Current Status:\n\r"
			"  Energy:      %d   Shields:     %d\n\r"
			"  Hull:        %d   Comm:        %d\n\r"
			"  Sensor:      %d   Astro Array: %d\n\r"
			"  Hyperspeed:  %d   Real Speed:  %d\n\r"
			"  Missiles:    %d   Chaff:       %d\n\r"
			"  Lasers:      %d   Tractorbeam: %d\n\r"
			"  Manuever:    %d   Ion Cannons: %d\n\r"
			"  Bombs:       %d\n\r",
			ship->energy, ship->maxshield,
			ship->maxhull, ship->comm,
			ship->sensor, ship->astro_array,
			ship->hyperspeed, ship->realspeed,
			ship->maxmissiles, ship->maxchaff,
			ship->lasers, ship->tractorbeam,
			ship->manuever, ship->ion,
			ship->maxbombs);
		send_to_char(buf, ch);
		return;
	}
	upgrade = FALSE;
	
	for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
	{
		if ( ship->class == FIGHTER_SHIP && obj->item_type == ITEM_FIGHTERCOMP)
			upgrade = TRUE;
		if ( ship->class == MIDSIZE_SHIP && obj->item_type == ITEM_MIDCOMP)
			upgrade = TRUE;
		if ( ship->class == CAPITAL_SHIP && obj->item_type == ITEM_CAPITALCOMP)
			upgrade = TRUE;
	}

	if ( !upgrade )
	{
		send_to_char( "You do not have a component\n\r", ch);
		return;
	}

	if ( !str_cmp("Engines", argument ) )
	{
		if ( ship->class == FIGHTER_SHIP )
			ship->realspeed += number_range(1,50);
		if ( ship->class == MIDSIZE_SHIP )
			ship->realspeed += number_range(25,100);
		if ( ship->class == CAPITAL_SHIP )
			ship->realspeed += number_range(75,150);
		send_to_char("Engine upgrade installed.\n\r", ch);
		act( AT_ACTION, "$n quickly installs a component.", ch, NULL, NULL, TO_ROOM );
	}
	else if ( !str_cmp("Shields", argument ) )
	{
		if ( ship->class == FIGHTER_SHIP )
			ship->maxshield += number_range(1,50);
		if ( ship->class == MIDSIZE_SHIP )
			ship->maxshield += number_range(25,100);
		if ( ship->class == CAPITAL_SHIP )
			ship->maxshield += number_range(75,150);
		send_to_char("Shield updrade installed.\n\r", ch);
		act( AT_ACTION, "$n quickly installs a component.", ch, NULL, NULL, TO_ROOM );
	}
	else if ( !str_cmp("Hyperdrive", argument ) )
	{
		if ( ship->class == FIGHTER_SHIP )
			ship->hyperspeed += number_range(1,50);
		if ( ship->class == MIDSIZE_SHIP )
			ship->hyperspeed += number_range(25,100);
		if ( ship->class == CAPITAL_SHIP )
			ship->hyperspeed += number_range(75,150);
		send_to_char("Hyperdrive upgrade installed.\n\r", ch);
		act( AT_ACTION, "$n quickly installs a component.", ch, NULL, NULL, TO_ROOM );
	}
	else if ( !str_cmp("Alarm", argument ) )
	{
		if ( ship->class == FIGHTER_SHIP )
			ship->alarm += 1;
		if ( ship->class == MIDSIZE_SHIP )
			ship->alarm += 1;
		if ( ship->class == CAPITAL_SHIP )
			ship->alarm += 1;
		send_to_char("Alarm installed.\n\r", ch);
		act( AT_ACTION, "$n quickly installs a component.", ch, NULL, NULL, TO_ROOM );
	}
	else
	{
		send_to_char("Your choices are \"Engines\", \"Shields\", \"Hyperdrive\", or \"Alarm\"\n\r",ch);
		return;
	}

    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
		if ( ship->class == FIGHTER_SHIP && obj->item_type == ITEM_FIGHTERCOMP && upgrade == FALSE)
		{
			separate_obj( obj );
			obj_from_char( obj );
			upgrade = TRUE;
		}
		if ( ship->class == MIDSIZE_SHIP && obj->item_type == ITEM_MIDCOMP && upgrade == FALSE)
		{
			separate_obj( obj );
			obj_from_char( obj );
			upgrade = TRUE;
		}
		if ( ship->class == CAPITAL_SHIP && obj->item_type == ITEM_CAPITALCOMP && upgrade == FALSE)
		{
			separate_obj( obj );
			obj_from_char( obj );
			upgrade = TRUE;
		}
    }                            

	set_char_color( AT_WHITE, ch );
	send_to_char( "Incomplete Command. Notify Gavin\n\r", ch );
	return;
}
#endif

