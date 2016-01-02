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
*	    Misc module for general commands: not skills or spells	   *
****************************************************************************
* Note: Most of the stuff in here would go in act_obj.c, but act_obj was   *
* getting big.								   *
****************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "mud.h"

extern int	top_exit;

void do_buyhome( CHAR_DATA *ch, char *argument )
{
     ROOM_INDEX_DATA *room;
     AREA_DATA *pArea;
     
     if ( !ch->in_room )
         return;
         
     if ( IS_NPC(ch) || !ch->pcdata )
         return;
         
     if ( ch->pcdata->plr_home != NULL )
     {
         send_to_char( "&RYou already have a home!\n\r&w", ch);
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
     
     if ( !xIS_SET( room->room_flags , ROOM_EMPTY_HOME ) )
     {
         send_to_char( "&RThis room isn't for sale!\n\r&w", ch);
         return;   
     }
     
     if ( ch->gold < 100000 )
     {
         send_to_char( "&RThis room costs 100000 credits you don't have enough!\n\r&w", ch);
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

     ch->gold -= 100000;
     
     xREMOVE_BIT( room->room_flags , ROOM_EMPTY_HOME );
     xSET_BIT( room->room_flags , ROOM_PLR_HOME );
     
     fold_area( room->area, room->area->filename, FALSE );

     ch->pcdata->plr_home = room;
     do_save( ch , "" );

}

void do_arm( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
	int timer;

    if ( IS_NPC(ch) || !ch->pcdata )
    {
      ch_printf( ch, "You have no idea how to do that.\n\r" );
      return;
    }
    
    if ( ch->pcdata->learned[gsn_grenades] <= 0 )
    {
      ch_printf( ch, "You have no idea how to do that.\n\r" );
      return;
    }
    
    obj = get_eq_char( ch, WEAR_HOLD );
    
    if ( !obj || obj->item_type != ITEM_GRENADE )
    {
       ch_printf( ch, "You don't seem to be holding a grenade!\n\r" );
       return;
    }
	if ( argument[0] == '\0' )
		timer = 1;
	else if ( is_number(argument) )
		timer = abs(atoi(argument));
	else
		timer = 1;

    obj->timer = timer;
    STRFREE( obj->armed_by ); 
    obj->armed_by = STRALLOC( ch->name );

    ch_printf( ch, "You arm %s.\n\r", obj->short_descr );
    act( AT_PLAIN, "$n arms $p.", ch, obj, NULL, TO_ROOM );
    
    learn_from_success( ch , gsn_grenades );  
}

void do_ammo( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *wield;
    OBJ_DATA *obj;
    bool checkammo = FALSE;
    int charge =0;
    
    obj = NULL;
    wield = get_eq_char( ch, WEAR_WIELD );
    if (wield)
    {
       obj = get_eq_char( ch, WEAR_DUAL_WIELD );
       if (!obj) 
          obj = get_eq_char( ch, WEAR_HOLD );
    } 
    else 
    { 
      wield = get_eq_char( ch, WEAR_HOLD );
      obj = NULL;
    } 
    
    if (!wield || wield->item_type != ITEM_WEAPON )
    {
        send_to_char( "&RYou don't seem to be holding a weapon.\n\r&w", ch);
        return; 
    }
    
    if ( wield->value[3] == WEAPON_BLASTER )
    {
    
      if ( obj && obj->item_type != ITEM_AMMO )
      {
        send_to_char( "&RYour hands are too full to reload your blaster.\n\r&w", ch);
        return;
      }
    
      if (obj)
      {
        if ( obj->value[0] > wield->value[5] )
        {
            send_to_char( "That cartridge is too big for your blaster.", ch);
            return;
        }
        unequip_char( ch, obj );
        checkammo = TRUE;
        charge = obj->value[0];
        separate_obj( obj );
        extract_obj( obj );
      }
      else
      {
        for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
        {
           if ( obj->item_type == ITEM_AMMO)
           {
                 if ( obj->value[0] > wield->value[5] )
                 {
                    send_to_char( "That cartridge is too big for your blaster.", ch);
                    continue;
                 }
                 checkammo = TRUE;
                 charge = obj->value[0];
                 separate_obj( obj );
                 extract_obj( obj );
                 break;                         
           }  
        }
      }
    
      if (!checkammo)
      {
        send_to_char( "&RYou don't seem to have any ammo to reload your blaster with.\n\r&w", ch);
        return;
      }
      
      ch_printf( ch, "You replace your ammunition cartridge.\n\rYour blaster is charged with %d shots at high power to %d shots on low.\n\r", charge/5, charge );
      act( AT_PLAIN, "$n replaces the ammunition cell in $p.", ch, wield, NULL, TO_ROOM );
	
    }
    else  if ( wield->value[3] == WEAPON_BOWCASTER )
    {
    
      if ( obj && obj->item_type != ITEM_BOLT )
      {
        send_to_char( "&RYour hands are too full to reload your bowcaster.\n\r&w", ch);
        return;
      }
    
      if (obj)
      {
        if ( obj->value[0] > wield->value[5] )
        {
            send_to_char( "That cartridge is too big for your bowcaster.", ch);
            return;
        }
        unequip_char( ch, obj );
        checkammo = TRUE;
        charge = obj->value[0];
        separate_obj( obj );
        extract_obj( obj );
      }
      else
      {
        for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
        {
           if ( obj->item_type == ITEM_BOLT)
           {
                 if ( obj->value[0] > wield->value[5] )
                 {
                    send_to_char( "That cartridge is too big for your bowcaster.", ch);
                    continue;
                 }
                 checkammo = TRUE;
                 charge = obj->value[0];
                 separate_obj( obj );
                 extract_obj( obj );
                 break;                         
           }  
        }
      }
    
      if (!checkammo)
      {
        send_to_char( "&RYou don't seem to have any quarrels to reload your bowcaster with.\n\r&w", ch);
        return;
      }
      
      ch_printf( ch, "You replace your quarrel pack.\n\rYour bowcaster is charged with %d energy bolts.\n\r", charge );
      act( AT_PLAIN, "$n replaces the quarrels in $p.", ch, wield, NULL, TO_ROOM );
	
    }
    else
    {
    
      if ( obj && obj->item_type != ITEM_BATTERY )
      {
        send_to_char( "&RYour hands are too full to replace the power cell.\n\r&w", ch);
        return;
      }
    
      if (obj)
      {
        unequip_char( ch, obj );
        checkammo = TRUE;
        charge = obj->value[0];
        separate_obj( obj );
        extract_obj( obj );
      }
      else
      {
        for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
        {
           if ( obj->item_type == ITEM_BATTERY)
           {
                 checkammo = TRUE;
                 charge = obj->value[0];
                 separate_obj( obj );
                 extract_obj( obj );
                 break;                         
           }  
        }
      }
    
      if (!checkammo)
      {
        send_to_char( "&RYou don't seem to have a power cell.\n\r&w", ch);
        return;
      }
      
      if (wield->value[3] == WEAPON_LIGHTSABER )
      {
         ch_printf( ch, "You replace your power cell.\n\rYour lightsaber is charged to %d/%d units.\n\r", charge, charge );
         act( AT_PLAIN, "$n replaces the power cell in $p.", ch, wield, NULL, TO_ROOM );
	 act( AT_PLAIN, "$p ignites with a bright glow.", ch, wield, NULL, TO_ROOM );
      }
      else if (wield->value[3] == WEAPON_VIBRO_BLADE )
      {
         ch_printf( ch, "You replace your power cell.\n\rYour vibro-blade is charged to %d/%d units.\n\r", charge, charge );
         act( AT_PLAIN, "$n replaces the power cell in $p.", ch, wield, NULL, TO_ROOM );
      }
      else if (wield->value[3] == WEAPON_FORCE_PIKE )
      {
         ch_printf( ch, "You replace your power cell.\n\rYour force-pike is charged to %d/%d units.\n\r", charge, charge );
         act( AT_PLAIN, "$n replaces the power cell in $p.", ch, wield, NULL, TO_ROOM );
      }
      else
      {
         ch_printf( ch, "You feel very foolish.\n\r" );
         act( AT_PLAIN, "$n tries to jam a power cell into $p.", ch, wield, NULL, TO_ROOM );
      }
    }
    
    wield->value[4] = charge;
        
}

void do_setblaster( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *wield;
   OBJ_DATA *wield2;
 
   wield = get_eq_char( ch, WEAR_WIELD );
   if( wield && !( wield->item_type == ITEM_WEAPON && wield->value[3] == WEAPON_BLASTER ) )
      wield = NULL;
   wield2 = get_eq_char( ch, WEAR_DUAL_WIELD );
   if( wield2 && !( wield2->item_type == ITEM_WEAPON && wield2->value[3] == WEAPON_BLASTER ) )
      wield2 = NULL;
   
   if ( !wield && !wield2 )
   {
      send_to_char( "&RYou don't seem to be wielding a blaster.\n\r&w", ch);
      return;
   }
   
   if ( argument[0] == '\0' )
   {
      send_to_char( "&RUsage: setblaster <full|high|normal|half|low|stun>\n\r&w", ch);
      return;
   }
   
   if ( wield )
     act( AT_PLAIN, "$n adjusts the settings on $p.", ch, wield, NULL, TO_ROOM );
  
   if ( wield2 )
    act( AT_PLAIN, "$n adjusts the settings on $p.", ch, wield2, NULL, TO_ROOM );
          
   if ( !str_cmp( argument, "full" ) )
   {
      if (wield)
      {
        wield->blaster_setting = BLASTER_FULL;
        send_to_char( "&YWielded blaster set to FULL Power\n\r&w", ch);
      }
      if (wield2)
      {
        wield2->blaster_setting = BLASTER_FULL;
        send_to_char( "&YDual wielded blaster set to FULL Power\n\r&w", ch);
      }
      return;
   } 
   if ( !str_cmp( argument, "high" ) )
   {
      if (wield)
      {
        wield->blaster_setting = BLASTER_HIGH;
        send_to_char( "&YWielded blaster set to HIGH Power\n\r&w", ch);
      }
      if (wield2)
      {
        wield2->blaster_setting = BLASTER_HIGH;
        send_to_char( "&YDual wielded blaster set to HIGH Power\n\r&w", ch);
      }
      return;
   } 
   if ( !str_cmp( argument, "normal" ) )
   {
      if (wield)
      {
        wield->blaster_setting = BLASTER_NORMAL;
        send_to_char( "&YWielded blaster set to NORMAL Power\n\r&w", ch);
      }
      if (wield2)
      {
        wield2->blaster_setting = BLASTER_NORMAL;
        send_to_char( "&YDual wielded blaster set to NORMAL Power\n\r&w", ch);
      }
      return;
   } 
   if ( !str_cmp( argument, "half" ) )
   {
      if (wield)
      {
        wield->blaster_setting = BLASTER_HALF;
        send_to_char( "&YWielded blaster set to HALF Power\n\r&w", ch);
      }
      if (wield2)
      {
        wield2->blaster_setting = BLASTER_HALF;
        send_to_char( "&YDual wielded blaster set to HALF Power\n\r&w", ch);
      }
      return;
   } 
   if ( !str_cmp( argument, "low" ) )
   {
      if (wield)
      {
        wield->blaster_setting = BLASTER_LOW;
        send_to_char( "&YWielded blaster set to LOW Power\n\r&w", ch);
      }
      if (wield2)
      {
        wield2->blaster_setting = BLASTER_LOW;
        send_to_char( "&YDual wielded blaster set to LOW Power\n\r&w", ch);
      }
      return;
   } 
   if ( !str_cmp( argument, "stun" ) )
   {
      if (wield)
      {
        wield->blaster_setting = BLASTER_STUN;
        send_to_char( "&YWielded blaster set to STUN\n\r&w", ch);
      }
      if (wield2)
      {
        wield2->blaster_setting = BLASTER_STUN;
        send_to_char( "&YDual wielded blaster set to STUN\n\r&w", ch);
      }
      return;
   } 
   else
     do_setblaster( ch , "" );

}

void do_use( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char argd[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *device;
    OBJ_DATA *obj;
    ch_ret retcode;

    argument = one_argument( argument, argd );
    argument = one_argument( argument, arg );
    
    if ( !str_cmp( arg , "on" ) )
       argument = one_argument( argument, arg );
    
    if ( argd[0] == '\0' )
    {
	send_to_char( "Use what?\n\r", ch );
	return;
    }

    if ( ( device = get_eq_char( ch, WEAR_HOLD ) ) == NULL ||
       !nifty_is_name(argd, device->name) )
    {
        do_takedrug( ch , argd );    
	return;
    }
    
    if ( device->item_type == ITEM_SPICE )
    {
        do_takedrug( ch , argd );
        return;
    }
    
    if ( device->item_type != ITEM_DEVICE )
    {
	send_to_char( "You can't figure out what it is your supposed to do with it.\n\r", ch );
	return;
    }
    
    if ( device->value[2] <= 0 )
    {
        send_to_char( "It has no more charge left.", ch);
        return;
    }
    
    obj = NULL;
    if ( arg[0] == '\0' )
    {
	if ( ch->fighting )
	{
	    victim = who_fighting( ch );
	}
	else
	{
	    send_to_char( "Use on whom or what?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( victim = get_char_room ( ch, arg ) ) == NULL
	&&   ( obj    = get_obj_here  ( ch, arg ) ) == NULL )
	{
	    send_to_char( "You can't find your target.\n\r", ch );
	    return;
	}
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );

    if ( device->value[2] > 0 )
    {
        device->value[2]--;
	if ( victim )
	{
          if ( !oprog_use_trigger( ch, device, victim, NULL, NULL ) )
          {
	    act( AT_MAGIC, "$n uses $p on $N.", ch, device, victim, TO_ROOM );
	    act( AT_MAGIC, "You use $p on $N.", ch, device, victim, TO_CHAR );
          }
	}
	else
	{
          if ( !oprog_use_trigger( ch, device, NULL, obj, NULL ) )
          {
	    act( AT_MAGIC, "$n uses $p on $P.", ch, device, obj, TO_ROOM );
	    act( AT_MAGIC, "You use $p on $P.", ch, device, obj, TO_CHAR );
          }
	}

	retcode = obj_cast_spell( device->value[3], device->value[0], ch, victim, obj );
	if ( retcode == rCHAR_DIED || retcode == rBOTH_DIED )
	{
	   bug( "do_use: char died", 0 );
	   return;
	}
    }


    return;
}

void do_takedrug( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    AFFECT_DATA af;
    int drug;
    int sn;
    
    if ( argument[0] == '\0' || !str_cmp(argument, "") )
    {
	send_to_char( "Use what?\n\r", ch );
	return;
    }

    if ( (obj = find_obj(ch, argument, TRUE)) == NULL )
	return;

    if ( obj->item_type == ITEM_DEVICE )
    {
        send_to_char( "Try holding it first.\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_SPICE )
    {
	act( AT_ACTION, "$n looks at $p and scratches $s head.", ch, obj, NULL, TO_ROOM );
	act( AT_ACTION, "You can't quite figure out what to do with $p.", ch, obj, NULL, TO_CHAR );
	return;
    }

    separate_obj( obj );
    if ( obj->in_obj )
    {
	act( AT_PLAIN, "You take $p from $P.", ch, obj, obj->in_obj, TO_CHAR );
	act( AT_PLAIN, "$n takes $p from $P.", ch, obj, obj->in_obj, TO_ROOM );
    }

    if ( ch->fighting && number_percent( ) > (get_curr_dex(ch) * 2 + 48) )
    {
	act( AT_MAGIC, "$n accidentally drops $p rendering it useless.", ch, obj, NULL, TO_ROOM );
	act( AT_MAGIC, "Oops... $p gets knocked from your hands rendering it completely useless!", ch, obj, NULL ,TO_CHAR );
    }
    else
    {
	if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
	{
	    act( AT_ACTION, "$n takes $p.",  ch, obj, NULL, TO_ROOM );
	    act( AT_ACTION, "You take $p.", ch, obj, NULL, TO_CHAR );
	}
        
        if ( IS_NPC(ch) )
        {
          extract_obj( obj );
          return;
        }
        
        drug = obj->value[0];
        
        WAIT_STATE( ch, PULSE_PER_SECOND/4 );
        
	gain_condition( ch, COND_THIRST, 1 );
	
	ch->pcdata->drug_level[drug] = UMIN(ch->pcdata->drug_level[drug]+obj->value[1] , 255);
	if ( ch->pcdata->drug_level[drug] >=255 
	     || ch->pcdata->drug_level[drug] > ( ch->pcdata->addiction[drug]+100 ) ) 
	{
	   act( AT_POISON, "$n sputters and gags.", ch, NULL, NULL, TO_ROOM );
	   act( AT_POISON, "You feel sick. You may have taken too much.", ch, NULL, NULL, TO_CHAR );
	     ch->mental_state = URANGE( 20, ch->mental_state + 5, 100 );
	     af.type      = gsn_poison;
	     af.location  = APPLY_INT;
	     af.modifier  = -5;
	     af.duration  = ch->pcdata->drug_level[drug];
	     af.bitvector = AFF_POISON;
	     affect_to_char( ch, &af );
	     ch->hit = 1;
	}
		  
	switch (drug)
	{ 
	    default:
	    case SPICE_GLITTERSTIM:
	
	       sn=skill_lookup("true sight");
	       if ( sn < MAX_SKILL && !IS_AFFECTED( ch, AFF_TRUESIGHT ) )
	       { 
	  	   af.type      = sn;
	  	   af.location  = APPLY_AC;
	  	   af.modifier  = -10;
	  	   af.duration  = URANGE( 1, ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug] ,obj->value[1] );
	  	   af.bitvector = AFF_TRUESIGHT;
	  	   affect_to_char( ch, &af );
	       }
	       break;
        
            case SPICE_CARSANUM:
            
               sn=skill_lookup("sanctuary");
	       if ( sn < MAX_SKILL && !IS_AFFECTED( ch, AFF_SANCTUARY ) )
	       { 
	  	   af.type      = sn;
	  	   af.location  = APPLY_NONE;
	  	   af.modifier  = 0;
	  	   af.duration  = URANGE( 1, ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug] ,obj->value[1] );
	  	   af.bitvector = AFF_SANCTUARY;
	  	   affect_to_char( ch, &af );
	       }
	       break;
         
            case SPICE_RYLL:
            
	  	   af.type      = -1;
	  	   af.location  = APPLY_DEX;
	  	   af.modifier  = 1;
	  	   af.duration  = URANGE( 1, 2*(ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug]) ,2*obj->value[1] );
	  	   af.bitvector = AFF_NONE;
	  	   affect_to_char( ch, &af );
	  	   
	  	   af.type      = -1;
	  	   af.location  = APPLY_HITROLL;
	  	   af.modifier  = 1;
	  	   af.duration  = URANGE( 1, 2*(ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug]) ,2*obj->value[1] );
	  	   af.bitvector = AFF_NONE;
	  	   affect_to_char( ch, &af );
	  	   
	        break;
        
            case SPICE_ANDRIS:
     
	  	   af.type      = -1;
	  	   af.location  = APPLY_HIT;
	  	   af.modifier  = 10;
	  	   af.duration  = URANGE( 1, 2*(ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug]) ,2*obj->value[1] );
	  	   af.bitvector = AFF_NONE;
	  	   affect_to_char( ch, &af );
	           
	           af.type      = sn;
	  	   af.location  = APPLY_CON;
	  	   af.modifier  = 1;
	  	   af.duration  = URANGE( 1, 2*(ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug]) ,2*obj->value[1] );
	  	   af.bitvector = AFF_NONE;
	  	   affect_to_char( ch, &af );
	           
	       break;
           
        }
        	
    }
    if ( cur_obj == obj->serial )
      global_objcode = rOBJ_EATEN;
    extract_obj( obj );
    return;
}

/*
 * Fill a container
 * Many enhancements added by Thoric (ie: filling non-drink containers)
 */
void do_fill( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *source;
    sh_int    dest_item, src_item1, src_item2, src_item3, src_item4;
    int       diff;
    bool      all = FALSE;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    /* munch optional words */
    if ( (!str_cmp( arg2, "from" ) || !str_cmp( arg2, "with" ))
    &&    argument[0] != '\0' )
	argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Fill what?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }
    else
	dest_item = obj->item_type;

    src_item1 = src_item2 = src_item3 = src_item4 = -1;
    switch( dest_item )
    {
	default:
	  act( AT_ACTION, "$n tries to fill $p... (Don't ask me how)", ch, obj, NULL, TO_ROOM );
	  send_to_char( "You cannot fill that.\n\r", ch );
	  return;
	/* place all fillable item types here */
	case ITEM_DRINK_CON:
	  src_item1 = ITEM_FOUNTAIN; break;
	case ITEM_HERB_CON:
	  src_item1 = ITEM_HERB;	src_item2 = ITEM_HERB_CON;	break;
	case ITEM_PIPE:
	  src_item1 = ITEM_HERB;	src_item2 = ITEM_HERB_CON;	break;
	case ITEM_CONTAINER:
	  src_item1 = ITEM_CONTAINER;	src_item2 = ITEM_CORPSE_NPC;
	  src_item3 = ITEM_CORPSE_PC;	src_item4 = ITEM_CORPSE_NPC;    break;
    }

    if ( dest_item == ITEM_CONTAINER )
    {
	if ( IS_SET(obj->value[1], CONT_CLOSED) )
	{
	    act( AT_PLAIN, "The $d is closed.", ch, NULL, obj->name, TO_CHAR );
	    return;
	}
	if ( get_obj_weight( obj ) / obj->count
	>=   obj->value[0] )
	{
	   send_to_char( "It's already full as it can be.\n\r", ch );
	   return;
	}
    }
    else
    {
	diff = obj->value[0] - obj->value[1];
	if ( diff < 1 || obj->value[1] >= obj->value[0] )
	{
	   send_to_char( "It's already full as it can be.\n\r", ch );
	   return;
	}
    }

    if ( dest_item == ITEM_PIPE
    &&   IS_SET( obj->value[3], PIPE_FULLOFASH ) )
    {
	send_to_char( "It's full of ashes, and needs to be emptied first.\n\r", ch );
	return;
    }

    if ( arg2[0] != '\0' )
    {
      if ( dest_item == ITEM_CONTAINER
      && (!str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 )) )
      {
	all = TRUE;
	source = NULL;
      }
      else
      /* This used to let you fill a pipe from an object on the ground.  Seems
         to me you should be holding whatever you want to fill a pipe with.
         It's nitpicking, but I needed to change it to get a mobprog to work
         right.  Check out Lord Fitzgibbon if you're curious.  -Narn */
      if ( dest_item == ITEM_PIPE )
      {
        if ( ( source = get_obj_carry( ch, arg2 ) ) == NULL )
	{
	   send_to_char( "You don't have that item.\n\r", ch );
	   return;
	}
	if ( source->item_type != src_item1 && source->item_type != src_item2
	&&   source->item_type != src_item3 &&   source->item_type != src_item4  )
	{
	   act( AT_PLAIN, "You cannot fill $p with $P!", ch, obj, source, TO_CHAR );
	   return;
	}
      }
      else
      {
	if ( ( source =  get_obj_here( ch, arg2 ) ) == NULL )
	{
	   send_to_char( "You cannot find that item.\n\r", ch );
	   return;
	}
      }
    }
    else
	source = NULL;

    if ( !source && dest_item == ITEM_PIPE )
    {
	send_to_char( "Fill it with what?\n\r", ch );
	return;
    }

    if ( !source )
    {
	bool      found = FALSE;
	OBJ_DATA *src_next;

	found = FALSE;
	separate_obj( obj );
	for ( source = ch->in_room->first_content;
	      source;
	      source = src_next )
	{
	    src_next = source->next_content;
	    if (dest_item == ITEM_CONTAINER)
	    {
		if ( !CAN_WEAR(source, ITEM_TAKE)
		||   (IS_OBJ_STAT( source, ITEM_PROTOTYPE) && !can_take_proto(ch))
		||    ch->carry_weight + get_obj_weight(source) > can_carry_w(ch)
		||   (get_obj_weight(source) + get_obj_weight(obj)/obj->count)
		    > obj->value[0] )
		  continue;
		if ( all && arg2[3] == '.'
		&&  !nifty_is_name( &arg2[4], source->name ) )
		   continue;
		obj_from_room(source);
		if ( source->item_type == ITEM_MONEY )
		{
		   ch->gold += source->value[0];
		   extract_obj( source );
		}
		else
		   obj_to_obj(source, obj);
		found = TRUE;
	    }
	    else
	    if (source->item_type == src_item1
	    ||  source->item_type == src_item2
	    ||  source->item_type == src_item3
	    ||  source->item_type == src_item4 )
	    {
		found = TRUE;
		break;
	    }
	}
	if ( !found )
	{
	    switch( src_item1 )
	    {
		default:
		  send_to_char( "There is nothing appropriate here!\n\r", ch );
		  return;
		case ITEM_FOUNTAIN:
		  send_to_char( "There is no fountain or pool here!\n\r", ch );
		  return;
		case ITEM_HERB_CON:
		  send_to_char( "There are no herbs here!\n\r", ch );
		  return;
		case ITEM_HERB:
		  send_to_char( "You cannot find any smoking herbs.\n\r", ch );
		  return;
	    }
	}
	if (dest_item == ITEM_CONTAINER)
	{
	  act( AT_ACTION, "You fill $p.", ch, obj, NULL, TO_CHAR );
	  act( AT_ACTION, "$n fills $p.", ch, obj, NULL, TO_ROOM );
	  return;
	}
    }

    if (dest_item == ITEM_CONTAINER)
    {
	OBJ_DATA *otmp, *otmp_next;
	char name[MAX_INPUT_LENGTH];
	CHAR_DATA *gch;
	char *pd;
	bool found = FALSE;

	if ( source == obj )
	{
	    send_to_char( "You can't fill something with itself!\n\r", ch );
	    return;
	}

	switch( source->item_type )
	{
	    default:	/* put something in container */
		if ( !source->in_room	/* disallow inventory items */
		||   !CAN_WEAR(source, ITEM_TAKE)
		||   (IS_OBJ_STAT( source, ITEM_PROTOTYPE) && !can_take_proto(ch))
		||    ch->carry_weight + get_obj_weight(source) > can_carry_w(ch)
		||   (get_obj_weight(source) + get_obj_weight(obj)/obj->count)
		    > obj->value[0] )
		{
		    send_to_char( "You can't do that.\n\r", ch );
		    return;
		}
		separate_obj( obj );
		act( AT_ACTION, "You take $P and put it inside $p.", ch, obj, source, TO_CHAR );
		act( AT_ACTION, "$n takes $P and puts it inside $p.", ch, obj, source, TO_ROOM );
		obj_from_room(source);
		obj_to_obj(source, obj);
		break;
	    case ITEM_MONEY:
		send_to_char( "You can't do that... yet.\n\r", ch );
		break;
	    case ITEM_CORPSE_PC:
		if ( IS_NPC(ch) )
		{
		    send_to_char( "You can't do that.\n\r", ch );
		    return;
		}
		
		    pd = source->short_descr;
		    pd = one_argument( pd, name );
		    pd = one_argument( pd, name );
		    pd = one_argument( pd, name );
		    pd = one_argument( pd, name );

		    if ( str_cmp( name, ch->name ) && !IS_IMMORTAL(ch) )
		    {
			bool fGroup;

			fGroup = FALSE;
			for ( gch = first_char; gch; gch = gch->next )
			{
			    if ( !IS_NPC(gch)
			    &&   is_same_group( ch, gch )
			    &&   !str_cmp( name, gch->name ) )
			    {
				fGroup = TRUE;
				break;
			    }
			}
			if ( !fGroup )
			{
			    send_to_char( "That's someone else's corpse.\n\r", ch );
			    return;
			}
		     }
		      
	    case ITEM_CONTAINER:
		if ( source->item_type == ITEM_CONTAINER  /* don't remove */
		&&   IS_SET(source->value[1], CONT_CLOSED) )
		{
		    act( AT_PLAIN, "The $d is closed.", ch, NULL, source->name, TO_CHAR );
		    return;
		}
	    case ITEM_DROID_CORPSE:
	    case ITEM_CORPSE_NPC:
		if ( (otmp=source->first_content) == NULL )
		{
		    send_to_char( "It's empty.\n\r", ch );
		    return;
		}
		separate_obj( obj );
		for ( ; otmp; otmp = otmp_next )
		{
		    otmp_next = otmp->next_content;

		    if ( !CAN_WEAR(otmp, ITEM_TAKE)
		    ||   (IS_OBJ_STAT( otmp, ITEM_PROTOTYPE) && !can_take_proto(ch))
		    ||    ch->carry_number + otmp->count > can_carry_n(ch)
		    ||    ch->carry_weight + get_obj_weight(otmp) > can_carry_w(ch)
		    ||   (get_obj_weight(source) + get_obj_weight(obj)/obj->count)
			> obj->value[0] )
			continue;
		    obj_from_obj(otmp);
		    obj_to_obj(otmp, obj);
		    found = TRUE;
		}
		if ( found )
		{
		   act( AT_ACTION, "You fill $p from $P.", ch, obj, source, TO_CHAR );
		   act( AT_ACTION, "$n fills $p from $P.", ch, obj, source, TO_ROOM );
		}
		else
		   send_to_char( "There is nothing appropriate in there.\n\r", ch );
		break;
	}
	return;
    }

    if ( source->value[1] < 1 )
    {
	send_to_char( "There's none left!\n\r", ch );
	return;
    }
    if ( source->count > 1 && source->item_type != ITEM_FOUNTAIN )
      separate_obj( source );
    separate_obj( obj );

    switch( source->item_type )
    {
	default:
	  bug( "do_fill: got bad item type: %d", source->item_type );
	  send_to_char( "Something went wrong...\n\r", ch );
	  return;
	case ITEM_FOUNTAIN:
	  if ( obj->value[1] != 0 && obj->value[2] != 0 )
	  {
	     send_to_char( "There is already another liquid in it.\n\r", ch );
	     return;
	  }
	  obj->value[2] = 0;
	  obj->value[1] = obj->value[0];
	  act( AT_ACTION, "You fill $p from $P.", ch, obj, source, TO_CHAR );
	  act( AT_ACTION, "$n fills $p from $P.", ch, obj, source, TO_ROOM );
	  return;
	case ITEM_HERB:
	  if ( obj->value[1] != 0 && obj->value[2] != source->value[2] )
	  {
	     send_to_char( "There is already another type of herb in it.\n\r", ch );
	     return;
	  }
	  obj->value[2] = source->value[2];
	  if ( source->value[1] < diff )
	    diff = source->value[1];
	  obj->value[1] += diff;
	  act( AT_ACTION, "You fill $p with $P.", ch, obj, source, TO_CHAR );
	  act( AT_ACTION, "$n fills $p with $P.", ch, obj, source, TO_ROOM );
	  if ( (source->value[1] -= diff) < 1 )
 	     extract_obj( source );
	  return;
	case ITEM_HERB_CON:
	  if ( obj->value[1] != 0 && obj->value[2] != source->value[2] )
	  {
	     send_to_char( "There is already another type of herb in it.\n\r", ch );
	     return;
	  }
	  obj->value[2] = source->value[2];
	  if ( source->value[1] < diff )
	    diff = source->value[1];
	  obj->value[1] += diff;
	  source->value[1] -= diff;
	  act( AT_ACTION, "You fill $p from $P.", ch, obj, source, TO_CHAR );
	  act( AT_ACTION, "$n fills $p from $P.", ch, obj, source, TO_ROOM );
	  return;
	case ITEM_DRINK_CON:
	  if ( obj->value[1] != 0 && obj->value[2] != source->value[2] )
	  {
	     send_to_char( "There is already another liquid in it.\n\r", ch );
	     return;
	  }
	  obj->value[2] = source->value[2];
	  if ( source->value[1] < diff )
	    diff = source->value[1];
	  obj->value[1] += diff;
	  source->value[1] -= diff;
	  act( AT_ACTION, "You fill $p from $P.", ch, obj, source, TO_CHAR );
	  act( AT_ACTION, "$n fills $p from $P.", ch, obj, source, TO_ROOM );
	  return;
    }
}

void do_drink( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int amount;
    int liquid;

	if ( xIS_SET(ch->bodyparts,BODY_L_HAND) || xIS_SET(ch->bodyparts,BODY_R_HAND) )
	{
		send_to_char( "How are you going to eat with your hands in that condition?\n\r", ch);
		return;
	}

	if ( xIS_SET(ch->bodyparts,BODY_JAW) )
	{
		send_to_char( "Your mouth hurts too much to eat.\n\r", ch);
		return;
	}

    argument = one_argument( argument, arg );
    /* munch optional words */
    if ( !str_cmp( arg, "from" ) && argument[0] != '\0' )
		argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
		for ( obj = ch->in_room->first_content; obj; obj = obj->next_content )
			if ( obj->item_type == ITEM_FOUNTAIN )
				break;
		
		if ( !obj )
		{
			send_to_char( "Drink what?\n\r", ch );
			return;
		}
	}
	else
	{
		if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
		{
			send_to_char( "You can't find it.\n\r", ch );
			return;
		}
	}
	
	if ( obj->count > 1 && obj->item_type != ITEM_FOUNTAIN )
		separate_obj(obj);

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 40 )
	{
		send_to_char( "You fail to reach your mouth.  *Hic*\n\r", ch );
		return;
	}

    switch ( obj->item_type )
    {
	default:
		if ( obj->carried_by == ch )
		{
			act( AT_ACTION, "$n lifts $p up to $s mouth and tries to drink from it...", ch, obj, NULL, TO_ROOM );
			act( AT_ACTION, "You bring $p up to your mouth and try to drink from it...", ch, obj, NULL, TO_CHAR );
		}
		else
		{
			act( AT_ACTION, "$n gets down and tries to drink from $p... (Is $e feeling ok?)", ch, obj, NULL, TO_ROOM );
			act( AT_ACTION, "You get down on the ground and try to drink from $p...", ch, obj, NULL, TO_CHAR );
		}
		break;
	case ITEM_POTION:
		if ( obj->carried_by == ch )
			do_quaff( ch, obj->name );	
		else
			send_to_char( "You're not carrying that.\n\r", ch );
		break;
    case ITEM_FOUNTAIN:
		if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
		{
			act( AT_ACTION, "$n drinks from the fountain.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "You take a long thirst quenching drink.\n\r", ch );
		}
		if ( !IS_NPC(ch) )
			ch->pcdata->condition[COND_THIRST] = 40;
		break;
    case ITEM_DRINK_CON:
		if ( obj->value[1] <= 0 )
		{
			send_to_char( "It is already empty.\n\r", ch );
			return;
		}
		
		if ( ( liquid = obj->value[2] ) >= LIQ_MAX )
		{
			bug( "Do_drink: bad liquid number %d.", liquid );
			liquid = obj->value[2] = 0;
		}
		if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
		{
			act( AT_ACTION, "$n drinks $T from $p.",
				ch, obj, liq_table[liquid].liq_name, TO_ROOM );
			act( AT_ACTION, "You drink $T from $p.",
				ch, obj, liq_table[liquid].liq_name, TO_CHAR );
		}
		amount = 1; /* UMIN(amount, obj->value[1]); */
		/* what was this? concentrated drinks?  concentrated water
		too I suppose... sheesh! */
		gain_condition( ch, COND_DRUNK, amount * liq_table[liquid].liq_affect[COND_DRUNK  ] );
		gain_condition( ch, COND_FULL, amount * liq_table[liquid].liq_affect[COND_FULL   ] );
		gain_condition( ch, COND_THIRST, amount * liq_table[liquid].liq_affect[COND_THIRST ] );
		
		if ( !IS_NPC(ch) )
		{
			if ( ch->pcdata->condition[COND_DRUNK]  > 24 )
				send_to_char( "You feel quite sloshed.\n\r", ch );
			else if ( ch->pcdata->condition[COND_DRUNK]  > 18 )
				send_to_char( "You feel very drunk.\n\r", ch );
			else if ( ch->pcdata->condition[COND_DRUNK]  > 12 )
				send_to_char( "You feel drunk.\n\r", ch );
			else if ( ch->pcdata->condition[COND_DRUNK]  > 8 )
				send_to_char( "You feel a little drunk.\n\r", ch );
			else if ( ch->pcdata->condition[COND_DRUNK]  > 5 )
				send_to_char( "You feel light headed.\n\r", ch );
			if ( ch->pcdata->condition[COND_FULL]   > 40 )
				send_to_char( "You are full.\n\r", ch );
			if ( ch->pcdata->condition[COND_THIRST] > 40 )
				send_to_char( "You feel bloated.\n\r", ch );
			else if ( ch->pcdata->condition[COND_THIRST] > 36 )
				send_to_char( "Your stomach is sloshing around.\n\r", ch );
			else if ( ch->pcdata->condition[COND_THIRST] > 30 )
				send_to_char( "You do not feel thirsty.\n\r", ch );
		}

		if ( obj->value[3] )
		{
			/* The drink was poisoned! */
			AFFECT_DATA af;

			act( AT_POISON, "$n sputters and gags.", ch, NULL, NULL, TO_ROOM );
			act( AT_POISON, "You sputter and gag.", ch, NULL, NULL, TO_CHAR );
			ch->mental_state = URANGE( 20, ch->mental_state + 5, 100 );
			af.type      = gsn_poison;
			af.duration  = 3 * obj->value[3];
			af.location  = APPLY_NONE;
			af.modifier  = 0;
			af.bitvector = AFF_POISON;
			affect_join( ch, &af );
		}
		
		obj->value[1] -= amount;
		break;
	}
	WAIT_STATE(ch, PULSE_PER_SECOND );
    return;
}

void do_eat( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    ch_ret retcode;
    int foodcond;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Eat what?\n\r", ch );
	return;
    }

    if ( IS_NPC(ch) || ch->pcdata->condition[COND_FULL] > 5 )
	if ( ms_find_obj(ch) )
	    return;

    if ( (obj = find_obj(ch, argument, TRUE)) == NULL )
	return;

    if ( !IS_IMMORTAL(ch) )
    {
		if ( obj->item_type != ITEM_FOOD 
			&& obj->item_type != ITEM_PILL 
			&& obj->item_type != ITEM_CONTRACEPTIVE )
	{
	    act( AT_ACTION, "$n starts to nibble on $p... ($e must really be hungry)",  ch, obj, NULL, TO_ROOM );
	    act( AT_ACTION, "You try to nibble on $p...", ch, obj, NULL, TO_CHAR );
	    return;
	}

	if ( xIS_SET(ch->bodyparts,BODY_L_HAND) || xIS_SET(ch->bodyparts,BODY_R_HAND) )
	{
		send_to_char( "How are you going to eat with your hands in that condition?\n\r", ch);
		return;
	}

	if ( xIS_SET(ch->bodyparts,BODY_JAW) )
	{
		send_to_char( "Your mouth hurts too much to eat.\n\r", ch);
		return;
	}

	if ( !IS_NPC(ch) && ch->pcdata->condition[COND_FULL] > 40 )
	{
	    send_to_char( "You are too full to eat more.\n\r", ch );
	    return;
	}
    }

    /* required due to object grouping */
    separate_obj( obj );
    
    WAIT_STATE( ch, PULSE_PER_SECOND/2 );
    
    if ( obj->in_obj )
    {
	act( AT_PLAIN, "You take $p from $P.", ch, obj, obj->in_obj, TO_CHAR );
	act( AT_PLAIN, "$n takes $p from $P.", ch, obj, obj->in_obj, TO_ROOM );
    }
    if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
    {
      if ( !obj->action_desc || obj->action_desc[0]=='\0' )
      {
        act( AT_ACTION, "$n eats $p.",  ch, obj, NULL, TO_ROOM );
        act( AT_ACTION, "You eat $p.", ch, obj, NULL, TO_CHAR );
      }
      else
        actiondesc( ch, obj, NULL ); 
    }

    switch ( obj->item_type )
    {

    case ITEM_FOOD:
	if ( obj->timer > 0 && obj->value[1] > 0 )
	   foodcond = (obj->timer * 10) / obj->value[1];
	else
	   foodcond = 10;

	if ( !IS_NPC(ch) )
	{
	    int condition;

	    condition = ch->pcdata->condition[COND_FULL];
	    gain_condition( ch, COND_FULL, (obj->value[0] * foodcond) / 10 );
	    if ( condition <= 1 && ch->pcdata->condition[COND_FULL] > 1 )
		send_to_char( "You are no longer hungry.\n\r", ch );
	    else if ( ch->pcdata->condition[COND_FULL] > 40 )
		send_to_char( "You are full.\n\r", ch );
	}

	if (  obj->value[3] != 0
		||   (foodcond < 4 && number_range( 0, foodcond + 1 ) == 0)  )
	{
	    /* The food was poisoned! */
	    AFFECT_DATA af;

	    if ( obj->value[3] != 0 )
	    {
		act( AT_POISON, "$n chokes and gags.", ch, NULL, NULL, TO_ROOM );
		act( AT_POISON, "You choke and gag.", ch, NULL, NULL, TO_CHAR );
		ch->mental_state = URANGE( 20, ch->mental_state + 5, 100 );
	    }
	    else
	    {
		act( AT_POISON, "$n gags on $p.", ch, obj, NULL, TO_ROOM );
		act( AT_POISON, "You gag on $p.", ch, obj, NULL, TO_CHAR );
		ch->mental_state = URANGE( 15, ch->mental_state + 5, 100 );
	    }

	    af.type      = gsn_poison;
	    af.duration  = 2 * obj->value[0]
	    		 * (obj->value[3] > 0 ? obj->value[3] : 1);
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_POISON;
	    affect_join( ch, &af );
	}
	break;

	case ITEM_CONTRACEPTIVE:
	{
	    AFFECT_DATA af;

	    if ( IS_AFFECTED(ch, AFF_CONTRACEPTION) )
		break;
	    if (ch->sex != SEX_FEMALE) break;
	    af.type      = -1;
	    af.duration  = 24;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_CONTRACEPTION;
	    affect_join( ch, &af );
		bug("Contraceptive",0);
	break;
	}

    case ITEM_PILL:
	/* allow pills to fill you, if so desired */
	if ( !IS_NPC(ch) && obj->value[4] )
	{
	    int condition;

	    condition = ch->pcdata->condition[COND_FULL];
	    gain_condition( ch, COND_FULL, obj->value[4] );
	    if ( condition <= 1 && ch->pcdata->condition[COND_FULL] > 1 )
		send_to_char( "You are no longer hungry.\n\r", ch );
	    else if ( ch->pcdata->condition[COND_FULL] > 40 && !IS_SET( ch->pcdata->cyber , CYBER_REACTOR) )
		send_to_char( "You are full.\n\r", ch );
	}
	retcode = obj_cast_spell( obj->value[1], obj->value[0], ch, ch, NULL );
	if ( retcode == rNONE )
	  retcode = obj_cast_spell( obj->value[2], obj->value[0], ch, ch, NULL );
	if ( retcode == rNONE )
	  retcode = obj_cast_spell( obj->value[3], obj->value[0], ch, ch, NULL );
	break;
    }

    if ( obj->serial == cur_obj )
      global_objcode = rOBJ_EATEN;
    extract_obj( obj );
    return;
}

void do_quaff( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    ch_ret retcode;

	if ( xIS_SET(ch->bodyparts,BODY_L_HAND) || xIS_SET(ch->bodyparts,BODY_R_HAND) )
	{
		send_to_char( "How are you going to eat with your hands in that condition?\n\r", ch);
		return;
	}

	if ( xIS_SET(ch->bodyparts,BODY_JAW) )
	{
		send_to_char( "Your mouth hurts too much to eat.\n\r", ch);
		return;
	}

    if ( argument[0] == '\0' || !str_cmp(argument, "") )
    {
	send_to_char( "Quaff what?\n\r", ch );
	return;
    }

    if ( (obj = find_obj(ch, argument, TRUE)) == NULL )
	return;

    if ( obj->item_type != ITEM_POTION )
    {
	if ( obj->item_type == ITEM_DRINK_CON )
	   do_drink( ch, obj->name );
	else
	{
	   act( AT_ACTION, "$n lifts $p up to $s mouth and tries to drink from it...", ch, obj, NULL, TO_ROOM );
	   act( AT_ACTION, "You bring $p up to your mouth and try to drink from it...", ch, obj, NULL, TO_CHAR );
	}
	return;
    }

    /*
     * Fullness checking					-Thoric
     */
    if ( !IS_NPC(ch) && !IS_SET( ch->pcdata->cyber, CYBER_REACTOR)
    && ( ch->pcdata->condition[COND_FULL] >= 48
    ||   ch->pcdata->condition[COND_THIRST] >= 48  ) )
    {
	send_to_char( "Your stomach cannot contain any more.\n\r", ch );
	return;
    }

	/* People with nuisance flag feels up quicker. -- Shaddai */
    /* Yeah so I can't spell I'm a coder :P --Shaddai */
    /* You are now adept at feeling up quickly! -- Blod */
    if ( !IS_NPC(ch) && ch->pcdata->nuisance &&
		ch->pcdata->nuisance->flags > 3
     &&(ch->pcdata->condition[COND_FULL]>=(48-(3*ch->pcdata->nuisance->flags)+
	ch->pcdata->nuisance->power)
     ||ch->pcdata->condition[COND_THIRST]>=(48-(ch->pcdata->nuisance->flags)+
	ch->pcdata->nuisance->power)))
    {
        send_to_char( "Your stomach cannot contain any more.\n\r", ch );
        return;
    }

    separate_obj( obj );
    if ( obj->in_obj )
    {
	act( AT_PLAIN, "You take $p from $P.", ch, obj, obj->in_obj, TO_CHAR );
	act( AT_PLAIN, "$n takes $p from $P.", ch, obj, obj->in_obj, TO_ROOM );
    }

    /*
     * If fighting, chance of dropping potion			-Thoric
     */
    if ( ch->fighting && number_percent( ) > (get_curr_dex(ch) * 2 + 48) )
    {
	act( AT_MAGIC, "$n accidentally drops $p and it smashes into a thousand fragments.", ch, obj, NULL, TO_ROOM );
	act( AT_MAGIC, "Oops... $p gets knocked from your hands and smashes into pieces!", ch, obj, NULL ,TO_CHAR );
    }
    else
    {
	if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
	{
	    act( AT_ACTION, "$n quaffs $p.",  ch, obj, NULL, TO_ROOM );
	    act( AT_ACTION, "You quaff $p.", ch, obj, NULL, TO_CHAR );
	}

        WAIT_STATE( ch, PULSE_PER_SECOND/4 );
        
	gain_condition( ch, COND_THIRST, 1 );
	retcode = obj_cast_spell( obj->value[1], obj->value[0], ch, ch, NULL );
	if ( retcode == rNONE )
	  retcode = obj_cast_spell( obj->value[2], obj->value[0], ch, ch, NULL );
	if ( retcode == rNONE )
	  retcode = obj_cast_spell( obj->value[3], obj->value[0], ch, ch, NULL );
    }
    if ( cur_obj == obj->serial )
      global_objcode = rOBJ_QUAFFED;
    extract_obj( obj );
    return;
}


/*
 * Function to handle the state changing of a triggerobject (lever)  -Thoric
 */
void pullorpush( CHAR_DATA *ch, OBJ_DATA *obj, bool pull )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA		*rch;
    bool		 isup;
    ROOM_INDEX_DATA	*room,  *to_room;
    EXIT_DATA		*pexit, *pexit_rev;
    int			 edir;
    char		*txt;

    if ( IS_SET( obj->value[0], TRIG_UP ) )
      isup = TRUE;
    else
      isup = FALSE;
    switch( obj->item_type )
    {
	default:
	  sprintf( buf, "You can't %s that!\n\r", pull ? "pull" : "push" );
	  send_to_char( buf, ch );
	  return;
	  break;
	case ITEM_BUTTON:
	  if ( (!pull && isup) || (pull & !isup) )
	  {
		sprintf( buf, "It is already %s.\n\r", isup ? "in" : "out" );
		send_to_char( buf, ch );
		return;
	  }
	  break;
    }
    if( (pull) && IS_SET(obj->pIndexData->progtypes,PULL_PROG) )
    {
	if ( !IS_SET(obj->value[0], TRIG_AUTORETURN ) )
 	  REMOVE_BIT( obj->value[0], TRIG_UP );
 	oprog_pull_trigger( ch, obj );
        return;
    }
    if( (!pull) && IS_SET(obj->pIndexData->progtypes,PUSH_PROG) )
    {
	if ( !IS_SET(obj->value[0], TRIG_AUTORETURN ) )
	  SET_BIT( obj->value[0], TRIG_UP );
	oprog_push_trigger( ch, obj );
        return;
    }

    if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
    {
      sprintf( buf, "$n %s $p.", pull ? "pulls" : "pushes" );
      act( AT_ACTION, buf,  ch, obj, NULL, TO_ROOM );
      sprintf( buf, "You %s $p.", pull ? "pull" : "push" );
      act( AT_ACTION, buf, ch, obj, NULL, TO_CHAR );
    }

    if ( !IS_SET(obj->value[0], TRIG_AUTORETURN ) )
    {
	if ( pull )
	  REMOVE_BIT( obj->value[0], TRIG_UP );
	else
	  SET_BIT( obj->value[0], TRIG_UP );
    }

    if ( IS_SET( obj->value[0], TRIG_RAND4 )
    ||	 IS_SET( obj->value[0], TRIG_RAND6 ) )
    {
	int maxd;

	if ( ( room = get_room_index( obj->value[1] ) ) == NULL )
	{
	    bug( "PullOrPush: obj points to invalid room %d", obj->value[1] );
	    return;
	}

	if ( IS_SET( obj->value[0], TRIG_RAND4 ) )
	  maxd = 3;
	else
	  maxd = 5;

	randomize_exits( room, maxd );
	for ( rch = room->first_person; rch; rch = rch->next_in_room )
	{
	   send_to_char( "You hear a loud rumbling sound.\n\r", rch );
	   send_to_char( "Something seems different...\n\r", rch );
	}
    }
    if ( IS_SET( obj->value[0], TRIG_DOOR ) )
    {
	room = get_room_index( obj->value[1] );
	if ( !room )
	  room = obj->in_room;
	if ( !room )
	{
	  bug( "PullOrPush: obj points to invalid room %d", obj->value[1] );
	  return;
	}	
	if ( IS_SET( obj->value[0], TRIG_D_NORTH ) )
	{
	  edir = DIR_NORTH;
	  txt = "to the north";
	}
	else
	if ( IS_SET( obj->value[0], TRIG_D_SOUTH ) )
	{
	  edir = DIR_SOUTH;
	  txt = "to the south";
	}
	else
	if ( IS_SET( obj->value[0], TRIG_D_EAST ) )
	{
	  edir = DIR_EAST;
	  txt = "to the east";
	}
	else
	if ( IS_SET( obj->value[0], TRIG_D_WEST ) )
	{
	  edir = DIR_WEST;
	  txt = "to the west";
	}
	else
	if ( IS_SET( obj->value[0], TRIG_D_UP ) )
	{
	  edir = DIR_UP;
	  txt = "from above";
	}
	else
	if ( IS_SET( obj->value[0], TRIG_D_DOWN ) )
	{
	  edir = DIR_DOWN;
	  txt = "from below";
	}
	else
	{
	  bug( "PullOrPush: door: no direction flag set.", 0 );
	  return;
	}
	pexit = get_exit( room, edir );
	if ( !pexit )
	{
	    if ( !IS_SET( obj->value[0], TRIG_PASSAGE ) )
	    {
		bug( "PullOrPush: obj points to non-exit %d", obj->value[1] );
		return;
	    }
	    to_room = get_room_index( obj->value[2] );
	    if ( !to_room )
	    {
		bug( "PullOrPush: dest points to invalid room %d", obj->value[2] );
		return;
	    }
	    pexit = make_exit( room, to_room, edir );
	    pexit->keyword	= STRALLOC( "" );
	    pexit->description	= STRALLOC( "" );
	    pexit->key		= -1;
	    pexit->exit_info	= 0;
	    top_exit++;
	    act( AT_PLAIN, "A passage opens!", ch, NULL, NULL, TO_CHAR );
	    act( AT_PLAIN, "A passage opens!", ch, NULL, NULL, TO_ROOM );
	    return;
	}
	if ( IS_SET( obj->value[0], TRIG_UNLOCK )
	&&   IS_SET( pexit->exit_info, EX_LOCKED) )
	{
	    REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	    act( AT_PLAIN, "You hear a faint click $T.", ch, NULL, txt, TO_CHAR );
	    act( AT_PLAIN, "You hear a faint click $T.", ch, NULL, txt, TO_ROOM );
	    if ( ( pexit_rev = pexit->rexit ) != NULL
	    &&   pexit_rev->to_room == ch->in_room )
		REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	    return;
	}
	if ( IS_SET( obj->value[0], TRIG_LOCK   )
	&&  !IS_SET( pexit->exit_info, EX_LOCKED) )
	{
	    SET_BIT(pexit->exit_info, EX_LOCKED);
	    act( AT_PLAIN, "You hear a faint click $T.", ch, NULL, txt, TO_CHAR );
	    act( AT_PLAIN, "You hear a faint click $T.", ch, NULL, txt, TO_ROOM );
	    if ( ( pexit_rev = pexit->rexit ) != NULL
	    &&   pexit_rev->to_room == ch->in_room )
		SET_BIT( pexit_rev->exit_info, EX_LOCKED );
	    return;
	}
	if ( IS_SET( obj->value[0], TRIG_OPEN   )
	&&   IS_SET( pexit->exit_info, EX_CLOSED) )
	{
	    REMOVE_BIT(pexit->exit_info, EX_CLOSED);
	    for ( rch = room->first_person; rch; rch = rch->next_in_room )
		act( AT_ACTION, "The $d opens.", rch, NULL, pexit->keyword, TO_CHAR );
	    if ( ( pexit_rev = pexit->rexit ) != NULL
	    &&   pexit_rev->to_room == ch->in_room )
	    {
		REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
		for ( rch = to_room->first_person; rch; rch = rch->next_in_room )
		    act( AT_ACTION, "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	    }
	    return;
	}
	if ( IS_SET( obj->value[0], TRIG_CLOSE   )
	&&  !IS_SET( pexit->exit_info, EX_CLOSED) )
	{
	    SET_BIT(pexit->exit_info, EX_CLOSED);
	    for ( rch = room->first_person; rch; rch = rch->next_in_room )
		act( AT_ACTION, "The $d closes.", rch, NULL, pexit->keyword, TO_CHAR );
	    if ( ( pexit_rev = pexit->rexit ) != NULL
	    &&   pexit_rev->to_room == ch->in_room )
	    {
		SET_BIT( pexit_rev->exit_info, EX_CLOSED );
		for ( rch = to_room->first_person; rch; rch = rch->next_in_room )
		    act( AT_ACTION, "The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	    }
	    return;
	}
    }
}


void do_pull( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Pull what?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    {
	act( AT_PLAIN, "I see no $T here.", ch, NULL, arg, TO_CHAR );
	return;
    }

    pullorpush( ch, obj, TRUE );
}

void do_push( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Push what?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    {
	act( AT_PLAIN, "I see no $T here.", ch, NULL, arg, TO_CHAR );
	return;
    }

    pullorpush( ch, obj, FALSE );
}

/* pipe commands (light, tamp, smoke) by Thoric */
void do_tamp( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *pipe;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Tamp what?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( (pipe = get_obj_carry( ch, arg )) == NULL )
    {
	send_to_char( "You aren't carrying that.\n\r", ch );
	return;
    }
    if ( pipe->item_type != ITEM_PIPE )
    {
	send_to_char( "You can't tamp that.\n\r", ch );
	return;
    }
    if ( !IS_SET( pipe->value[3], PIPE_TAMPED ) )
    {
	act( AT_ACTION, "You gently tamp $p.", ch, pipe, NULL, TO_CHAR );
	act( AT_ACTION, "$n gently tamps $p.", ch, pipe, NULL, TO_ROOM );
	SET_BIT( pipe->value[3], PIPE_TAMPED );
	return;
    }
    send_to_char( "It doesn't need tamping.\n\r", ch );
}

void do_smoke( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *pipe;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Smoke what?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( (pipe = get_obj_carry( ch, arg )) == NULL )
    {
	send_to_char( "You aren't carrying that.\n\r", ch );
	return;
    }
    if ( pipe->item_type != ITEM_PIPE )
    {
	act( AT_ACTION, "You try to smoke $p... but it doesn't seem to work.", ch, pipe, NULL, TO_CHAR );
	act( AT_ACTION, "$n tries to smoke $p... (I wonder what $e's been putting his $s pipe?)", ch, pipe, NULL, TO_ROOM );
	return;
    }
    if ( !IS_SET( pipe->value[3], PIPE_LIT ) )
    {
	act( AT_ACTION, "You try to smoke $p, but it's not lit.", ch, pipe, NULL, TO_CHAR );
	act( AT_ACTION, "$n tries to smoke $p, but it's not lit.", ch, pipe, NULL, TO_ROOM );
	return;
    }
    if ( pipe->value[1] > 0 )
    {
	if ( !oprog_use_trigger( ch, pipe, NULL, NULL, NULL ) )
	{
	   act( AT_ACTION, "You draw thoughtfully from $p.", ch, pipe, NULL, TO_CHAR );
	   act( AT_ACTION, "$n draws thoughtfully from $p.", ch, pipe, NULL, TO_ROOM );
	}

	if ( IS_VALID_HERB( pipe->value[2] ) && pipe->value[2] < top_herb )
	{
	    int sn		= pipe->value[2] + TYPE_HERB;
	    SKILLTYPE *skill	= get_skilltype( sn );

	    WAIT_STATE( ch, skill->beats );
	    if ( skill->spell_fun )
		obj_cast_spell( sn, UMIN(skill->min_level, ch->top_level),
			ch, ch, NULL );
	    if ( obj_extracted( pipe ) )
		return;
	}
	else
	    bug( "do_smoke: bad herb type %d", pipe->value[2] );

	SET_BIT( pipe->value[3], PIPE_HOT );
	if ( --pipe->value[1] < 1 )
	{
	   REMOVE_BIT( pipe->value[3], PIPE_LIT );
	   SET_BIT( pipe->value[3], PIPE_DIRTY );
	   SET_BIT( pipe->value[3], PIPE_FULLOFASH );
	}
    }
}

void do_light( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *pipe;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Light what?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( (pipe = get_obj_carry( ch, arg )) == NULL )
    {
	send_to_char( "You aren't carrying that.\n\r", ch );
	return;
    }
    if ( pipe->item_type != ITEM_PIPE )
    {
	send_to_char( "You can't light that.\n\r", ch );
	return;
    }
    if ( !IS_SET( pipe->value[3], PIPE_LIT ) )
    {
	if ( pipe->value[1] < 1 )
	{
	  act( AT_ACTION, "You try to light $p, but it's empty.", ch, pipe, NULL, TO_CHAR );
	  act( AT_ACTION, "$n tries to light $p, but it's empty.", ch, pipe, NULL, TO_ROOM );
	  return;
	}
	act( AT_ACTION, "You carefully light $p.", ch, pipe, NULL, TO_CHAR );
	act( AT_ACTION, "$n carefully lights $p.", ch, pipe, NULL, TO_ROOM );
	SET_BIT( pipe->value[3], PIPE_LIT );
	return;
    }
    send_to_char( "It's already lit.\n\r", ch );
}

void do_empty( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( !str_cmp( arg2, "into" ) && argument[0] != '\0' )
	argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Empty what?\n\r", ch );
	return;
    }
    if ( ms_find_obj(ch) )
	return;

    if ( (obj = get_obj_carry( ch, arg1 )) == NULL )
    {
	send_to_char( "You aren't carrying that.\n\r", ch );
	return;
    }
    if ( obj->count > 1 )
      separate_obj(obj);

    switch( obj->item_type )
    {
	default:
	  act( AT_ACTION, "You shake $p in an attempt to empty it...", ch, obj, NULL, TO_CHAR );
	  act( AT_ACTION, "$n begins to shake $p in an attempt to empty it...", ch, obj, NULL, TO_ROOM );
	  return;
	case ITEM_PIPE:
	  act( AT_ACTION, "You gently tap $p and empty it out.", ch, obj, NULL, TO_CHAR );
	  act( AT_ACTION, "$n gently taps $p and empties it out.", ch, obj, NULL, TO_ROOM );
	  REMOVE_BIT( obj->value[3], PIPE_FULLOFASH );
	  REMOVE_BIT( obj->value[3], PIPE_LIT );
	  obj->value[1] = 0;
	  return;
	case ITEM_DRINK_CON:
	  if ( obj->value[1] < 1 )
	  {
		send_to_char( "It's already empty.\n\r", ch );
		return;
	  }
	  act( AT_ACTION, "You empty $p.", ch, obj, NULL, TO_CHAR );
	  act( AT_ACTION, "$n empties $p.", ch, obj, NULL, TO_ROOM );
	  obj->value[1] = 0;
	  return;
	case ITEM_CONTAINER:
	  if ( IS_SET(obj->value[1], CONT_CLOSED) )
	  {
		act( AT_PLAIN, "The $d is closed.", ch, NULL, obj->name, TO_CHAR );
		return;
	  }
	  if ( !obj->first_content )
	  {
		send_to_char( "It's already empty.\n\r", ch );
		return;
	  }
	  if ( arg2[0] == '\0' )
	  {
		if ( xIS_SET( ch->in_room->room_flags, ROOM_NODROP )
		|| ( !IS_NPC(ch) &&  IS_SET( ch->act, PLR_LITTERBUG ) ) )
		{
		       set_char_color( AT_MAGIC, ch );
		       send_to_char( "A magical force stops you!\n\r", ch );
		       set_char_color( AT_TELL, ch );
		       send_to_char( "Someone tells you, 'No littering here!'\n\r", ch );
		       return;
		}
		if ( xIS_SET( ch->in_room->room_flags, ROOM_NODROPALL ) )
		{
		   send_to_char( "You can't seem to do that here...\n\r", ch );
		   return;
		}
		if ( empty_obj( obj, NULL, ch->in_room ) )
		{
		    act( AT_ACTION, "You empty $p.", ch, obj, NULL, TO_CHAR );
		    act( AT_ACTION, "$n empties $p.", ch, obj, NULL, TO_ROOM );
		    if ( IS_SET( sysdata.save_flags, SV_DROP ) )
			save_char_obj( ch );
		}
		else
		    send_to_char( "Hmmm... didn't work.\n\r", ch );
	  }
	  else
	  {
		OBJ_DATA *dest = get_obj_here( ch, arg2 );

		if ( !dest )
		{
		    send_to_char( "You can't find it.\n\r", ch );
		    return;
		}
		if ( dest == obj )
		{
		    send_to_char( "You can't empty something into itself!\n\r", ch );
		    return;
		}
		if ( dest->item_type != ITEM_CONTAINER )
		{
		    send_to_char( "That's not a container!\n\r", ch );
		    return;
		}
		if ( IS_SET(dest->value[1], CONT_CLOSED) )
		{
		    act( AT_PLAIN, "The $d is closed.", ch, NULL, dest->name, TO_CHAR );
		    return;
		}
		separate_obj( dest );
		if ( empty_obj( obj, dest, NULL ) )
		{
		    act( AT_ACTION, "You empty $p into $P.", ch, obj, dest, TO_CHAR );
		    act( AT_ACTION, "$n empties $p into $P.", ch, obj, dest, TO_ROOM );
		    if ( !dest->carried_by
		    &&    IS_SET( sysdata.save_flags, SV_PUT ) )
			save_char_obj( ch );
		}
		else
		    act( AT_ACTION, "$P is too full.", ch, obj, dest, TO_CHAR );
	  }
	  return;
    }
}
 
/*
 * Apply a salve/ointment					-Thoric
 */
void do_apply( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    ch_ret retcode;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Apply what?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( ( obj = get_obj_carry( ch, argument ) ) == NULL )
    {
	send_to_char( "You do not have that.\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_SALVE )
    {
	act( AT_ACTION, "$n starts to rub $p on $mself...",  ch, obj, NULL, TO_ROOM );
	act( AT_ACTION, "You try to rub $p on yourself...", ch, obj, NULL, TO_CHAR );
	return;
    }

    separate_obj( obj );

    --obj->value[1];
    if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
    {
	if ( !obj->action_desc || obj->action_desc[0]=='\0' )
	{
	    act( AT_ACTION, "$n rubs $p onto $s body.",  ch, obj, NULL, TO_ROOM );
	    if ( obj->value[1] <= 0 )
		act( AT_ACTION, "You apply the last of $p onto your body.", ch, obj, NULL, TO_CHAR );
	    else
		act( AT_ACTION, "You apply $p onto your body.", ch, obj, NULL, TO_CHAR );
	}
	else
	    actiondesc( ch, obj, NULL ); 
    }

    WAIT_STATE( ch, obj->value[2] );
    retcode = obj_cast_spell( obj->value[4], obj->value[0], ch, ch, NULL );
    if ( retcode == rNONE )
	retcode = obj_cast_spell( obj->value[5], obj->value[0], ch, ch, NULL );

    if ( !obj_extracted(obj) && obj->value[1] <= 0 )
	extract_obj( obj );

    return;
}

void actiondesc( CHAR_DATA *ch, OBJ_DATA *obj, void *vo )
{
    char charbuf[MAX_STRING_LENGTH];
    char roombuf[MAX_STRING_LENGTH];
    char *srcptr = obj->action_desc;
    char *charptr = charbuf;
    char *roomptr = roombuf;
/*    const char *ichar;
    const char *iroom;*/

	const char *ichar = "You";
    const char *iroom = "Someone";

while ( *srcptr != '\0' )
{
  if ( *srcptr == '$' ) 
  {
    srcptr++;
    switch ( *srcptr )
    {
      case 'e':
        ichar = "you";
        iroom = "$e";
        break;

      case 'm':
        ichar = "you";
        iroom = "$m";
        break;

      case 'n':
        ichar = "you";
        iroom = "$n";
        break;

      case 's':
        ichar = "your";
        iroom = "$s";
        break;

      /*case 'q':
        iroom = "s";
        break;*/

      default: 
        srcptr--;
        *charptr++ = *srcptr;
        *roomptr++ = *srcptr;
        break;
    }
  }
  else if ( *srcptr == '%' && *++srcptr == 's' ) 
  {
    ichar = "You";
    iroom = IS_NPC( ch ) ? ch->short_descr : ch->name;
  }
  else
  {
    *charptr++ = *srcptr;
    *roomptr++ = *srcptr;
    srcptr++;
    continue;
  }

  while ( ( *charptr = *ichar ) != '\0' )
  {
    charptr++;
    ichar++;
  }

  while ( ( *roomptr = *iroom ) != '\0' )
  {
    roomptr++;
    iroom++;
  }
  srcptr++;
}

*charptr = '\0';
*roomptr = '\0';

/*
sprintf( buf, "Charbuf: %s", charbuf );
log_string_plus( buf, LOG_HIGH, LEVEL_LESSER ); 
sprintf( buf, "Roombuf: %s", roombuf );
log_string_plus( buf, LOG_HIGH, LEVEL_LESSER ); 
*/

switch( obj->item_type )
{
  case ITEM_FOUNTAIN:
    act( AT_ACTION, charbuf, ch, obj, ch, TO_CHAR );
    act( AT_ACTION, roombuf, ch, obj, ch, TO_ROOM );
    return;

  case ITEM_DRINK_CON:
    act( AT_ACTION, charbuf, ch, obj, liq_table[obj->value[2]].liq_name, TO_CHAR );
    act( AT_ACTION, roombuf, ch, obj, liq_table[obj->value[2]].liq_name, TO_ROOM );
    return;

  case ITEM_PIPE:
    return;

  case ITEM_ARMOR:
  case ITEM_WEAPON:
  case ITEM_LIGHT:
    act( AT_ACTION, charbuf, ch, obj, ch, TO_CHAR );
    act( AT_ACTION, roombuf, ch, obj, ch, TO_ROOM );
    return;
  
  case ITEM_FOOD:
  case ITEM_CONTRACEPTIVE:
  case ITEM_PILL:
    act( AT_ACTION, charbuf, ch, obj, ch, TO_CHAR );
    act( AT_ACTION, roombuf, ch, obj, ch, TO_ROOM );
    return;

  default:
    return;
}
return;
}

void do_hail( CHAR_DATA *ch , char *argument )
{
    char buf[MAX_STRING_LENGTH];
	char arg[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
    int vnum;
    ROOM_INDEX_DATA *room;
	SHIP_DATA *target;
	SHIP_DATA *ship;

	
	argument = one_argument( argument, arg );
	strcpy( arg2, argument );
    
    if ( !ch->in_room )
       return;

    if ( ch->position < POS_FIGHTING )
    {
       send_to_char( "You might want to stop fighting first!\n\r", ch );
       return;
    }

	if ( arg[0] != '\0' )
	{
		if ( (ship = ship_from_cockpit(ch->in_room->vnum) ) == NULL )
		{
			send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
			return;
		}

		if ( arg2[0] == '\0' )
		{
			send_to_char( "&RUsage: hail <ship> <message>\n\r&w", ch);
			return;
		}

		if ( !ship->starsystem )
		{
			send_to_char( "&RYou need to launch first!\n\r&w", ch);
			return;
		}

		target = get_ship_here(arg,ship->starsystem);

		if (  target == NULL )
		{

			send_to_char("&RThat ship isn't here!\n\r",ch);
			return;
		}
		
		if (  target == ship )
		{
			send_to_char("&RWhy don't you just say it?\n\r",ch);
			return;
		}


		if ( ( abs(target->vx - ship->vx) > 100*((ship->comm)+(target->comm)+20) )
			|| ( abs(target->vy - ship->vy) > 100*((ship->comm)+(target->comm)+20) )
			|| ( abs(target->vz - ship->vz) > 100*((ship->comm)+(target->comm)+20) ) )
		{
			send_to_char("&RThat ship is out of the range of your comm system.\n\r&w", ch);
			return;
		}

		strcpy( buf , "You hail the " );
		strcat( buf , target->name );
		strcat( buf , ": &C" );
		strcat( buf , arg2 );
		strcat( buf , "&w\n\r" );

		echo_to_ship( AT_WHITE , ship , buf);

		strcpy( buf , ship->name );
		strcat( buf , " hails you: &C" );
		strcat( buf , arg2 );
		strcat( buf , "&w\n\r" );
    
		echo_to_ship( AT_WHITE , target , buf);

		return;
	}
	
	if ( ch->position < POS_STANDING )
    {
       send_to_char( "You might want to stand up first!\n\r", ch );
       return;
    }

    if ( xIS_SET( ch->in_room->room_flags , ROOM_INDOORS ) )
    {
       send_to_char( "You'll have to go outside to do that!\n\r", ch );
       return;
    }
    
    if ( xIS_SET( ch->in_room->room_flags , ROOM_SPACECRAFT ) )
    {
       send_to_char( "You can't do that on spacecraft!\n\r", ch );
       return;
    }

	vnum = ch->in_room->vnum;
    
    for ( vnum = ch->in_room->area->low_r_vnum  ;  vnum <= ch->in_room->area->hi_r_vnum  ;  vnum++ )
	{
		room = get_room_index ( vnum );
            
		if ( room != NULL )
		{
			if ( xIS_SET(room->room_flags , ROOM_HOTEL ) && !xIS_SET(room->room_flags, ROOM_HOUSE ) )
				break;   
			else 
				room = NULL;   
		}
	}
    
    if ( room == NULL )
    {
       send_to_char( "The Hover Taxi Arrives.\n\r", ch );
	   send_to_char( "But he informs &RYou&w there are not any hotels around.\n\r", ch );
       return;
    }
  
	sprintf(buf, "%s hails a Hover Taxi and drives off to %s.\n\r", ch->name, room->name );
    act( AT_ACTION, buf, ch, NULL, NULL,  TO_ROOM );

    char_from_room( ch );
    char_to_room( ch, room );

	ch_printf( ch, "A Hover Taxi picks you up and drives you to %s.\n\r",
	    room->name );
   
    act( AT_ACTION, "$n $T", ch, NULL, "arrives on a Hover Taxi, gets out and pays the driver before it leaves.",  TO_ROOM );
                               
    do_look( ch, "auto" );
   
}

void do_train( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *mob;
	bool tfound = FALSE;
	bool successful = FALSE;
	
	if ( IS_NPC(ch) )
		return;
	
	strcpy( arg, argument );
	switch( ch->substate )
	{ 
	default:
		if ( arg[0] == '\0' )
		{
			send_to_char( "Train what?\n\r", ch );
			send_to_char( "\n\rChoices: strength, intelligence, wisdom, dexterity, constitution or charisma\n\r", ch );
			return;	
		}
		if ( !IS_AWAKE(ch) )
		{
			send_to_char( "In your dreams, or what?\n\r", ch );
			return;
		}
		for ( mob = ch->in_room->first_person; mob; mob = mob->next_in_room )
		{
			if ( IS_NPC(mob) && IS_SET(mob->act, ACT_TRAIN) )
			{
				tfound = TRUE;
				break;
			}
		}
		if ( (!mob) || (!tfound) )
		{
			send_to_char( "You can't do that here.\n\r", ch );
			return;
		}
		if ( str_cmp( arg, "str" ) && str_cmp( arg, "strength" )
			&& str_cmp( arg, "dex" ) && str_cmp( arg, "dexterity" )
			&& str_cmp( arg, "con" ) && str_cmp( arg, "constitution" )
			&& str_cmp( arg, "cha" ) && str_cmp( arg, "charisma" )
			&& str_cmp( arg, "wis" ) && str_cmp( arg, "wisdom" )
			&& str_cmp( arg, "int" ) && str_cmp( arg, "intelligence" ) )
		{
			do_train ( ch , "" );
			return;
		}
		if ( !str_cmp( arg, "str" ) || !str_cmp( arg, "strength" ) )
		{
			if( mob->perm_str <= ch->perm_str 
			/*	|| ch->perm_str >= 20 + race_table[ch->race]->str_plus */
				|| ch->perm_str >= GET_MAX_STR(ch)
				|| ch->perm_str >= 25 )
			{
				act( AT_TELL, "$n tells you 'I cannot help you... you are already stronger than I.'",
					mob, NULL, ch, TO_VICT );
				return;
			}
			send_to_char("&GYou begin your weight training.\n\r", ch);
		}
		if ( !str_cmp( arg, "dex" ) || !str_cmp( arg, "dexterity" ) )
		{
			if( mob->perm_dex <= ch->perm_dex 
				/* || ch->perm_dex >= 20 + race_table[ch->race]->dex_plus */
				|| ch->perm_dex >= GET_MAX_DEX(ch) 
				|| ch->perm_dex >= 25 )
			{
				act( AT_TELL, "$n tells you 'I cannot help you... you are already more dextrous than I.'",
					mob, NULL, ch, TO_VICT );
				return;
			}
			send_to_char("&GYou begin to work at some challenging tests of coordination.\n\r", ch);
		}
		if ( !str_cmp( arg, "int" ) || !str_cmp( arg, "intelligence" ) )
		{
			if( mob->perm_int <= ch->perm_int 
				/*|| ch->perm_int >= 20 + race_table[ch->race]->int_plus */
				|| ch->perm_int >= GET_MAX_INT(ch)
				|| ch->perm_int >= 25 )
			{
				act( AT_TELL, "$n tells you 'I cannot help you... you are already more educated than I.'",
					mob, NULL, ch, TO_VICT );
				return;
			}
			send_to_char("&GYou begin your studies.\n\r", ch);
		}
		if ( !str_cmp( arg, "wis" ) || !str_cmp( arg, "wisdom" ) )
		{
			if( mob->perm_wis <= ch->perm_wis 
				/*|| ch->perm_wis >= 20 + race_table[ch->race]->wis_plus */
				|| ch->perm_wis >= GET_MAX_WIS(ch)
				|| ch->perm_wis >= 25 )
			{
				act( AT_TELL, "$n tells you 'I cannot help you... you are already far wiser than I.'",
					mob, NULL, ch, TO_VICT );
				return;
			}
			send_to_char("&GYou begin contemplating several ancient texts in an effort to gain wisdom.\n\r", ch);
		}
		if ( !str_cmp( arg, "con" ) || !str_cmp( arg, "constitution" ) )
		{
			if( mob->perm_con <= ch->perm_con 
				/*|| ch->perm_con >= 20 + race_table[ch->race]->con_plus */
				|| ch->perm_con >= GET_MAX_CON(ch)
				|| ch->perm_con >= 25 )
			{
				act( AT_TELL, "$n tells you 'I cannot help you... you are already healthier than I.'",
					mob, NULL, ch, TO_VICT );
				return;
			}
			send_to_char("&GYou begin your endurance training.\n\r", ch);
		}
		if ( !str_cmp( arg, "cha" ) || !str_cmp( arg, "charisma" ) )
		{
			if( mob->perm_cha <= ch->perm_cha 
				/*|| ch->perm_cha >= 20 + race_table[ch->race]->cha_plus */
				|| ch->perm_cha >= GET_MAX_CHA(ch)
				|| ch->perm_cha >= 25 )
			{
				act( AT_TELL, "$n tells you 'I cannot help you... you already are more charming than I.'",
					mob, NULL, ch, TO_VICT );
				return;
			}
			send_to_char("&GYou begin lessons in manners and refinement.\n\r", ch);
		}
		add_timer ( ch , TIMER_DO_FUN , 10 , do_train , 1 );
		ch->dest_buf = str_dup(arg);
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
		send_to_char("&RYou fail to complete your training.\n\r", ch);
		return;
	}
	ch->substate = SUB_NONE;
/*	if ( number_bits ( 2 ) == 0 )
    {
        successful = TRUE;
    }*/
	successful = TRUE;
	if ( !str_cmp( arg, "str" ) || !str_cmp( arg, "strength" ) )
	{
		if ( !successful )
		{
			send_to_char("&RYou feel that you have wasted a lot of energy for nothing...\n\r", ch);
			return;	
		} 
		send_to_char("&GAfter much exercise you feel a little stronger.\n\r", ch);
		ch->perm_str++;
		return;
	}
	if ( !str_cmp( arg, "dex" ) || !str_cmp( arg, "dexterity" ) )
	{
		if ( !successful )
		{
			send_to_char("&RAfter all that training, you still feel like a clutz...\n\r", ch);
			return;	
		} 
		send_to_char("&GAfter working hard at many challenging tasks you feel a bit more coordinated.\n\r", ch);
		ch->perm_dex++;
		return;
	}
	if ( !str_cmp( arg, "int" ) || !str_cmp( arg, "intelligence" ) )
	{
		if ( !successful )
		{
			send_to_char("&RHitting the books leaves you only with sore eyes...\n\r", ch);
			return;	
		} 
		send_to_char("&GAfter much study you feel a little more intellegent.\n\r", ch);
		ch->perm_int++;
		return;
	}
	if ( !str_cmp( arg, "wis" ) || !str_cmp( arg, "wisdom" ) )
	{
		if ( !successful )
		{
			send_to_char("&RStudying the ancient texts has left you more confused than wise...\n\r", ch);
			return;	
		} 
		send_to_char("&GAfter contemplating several seemingly meaningless events you suddenly \n\rreceive a flash of insight into the workings of the universe.\n\r", ch);
		ch->perm_wis++;
		return;
	}
	if ( !str_cmp( arg, "con" ) || !str_cmp( arg, "constitution" ) )
	{
		if ( !successful )
		{
			send_to_char("&RYou spend long a long aerobics session exersising very hard but finish\n\rfeeling only tired and out of breath....\n\r", ch);
			return;	
		} 
		send_to_char("&GAfter a long tiring exersise session you feel much healthier than before.\n\r", ch);
		ch->perm_con++;
		return;
	}
	if ( !str_cmp( arg, "cha" ) || !str_cmp( arg, "charisma" ) )
	{
		if ( !successful )
		{
			send_to_char("&RYou finish your self improvement session feeling a little depressed.\n\r", ch);
			return;	
		} 
		send_to_char("&GYou spend some time focusing on how to improve your personality and feel \n\rmuch better about yourself and the ways others see you.\n\r", ch);
		ch->perm_cha++;
		return;
	}   	
}

void do_suicide( CHAR_DATA *ch, char *argument )
{
	char logbuf[MAX_STRING_LENGTH];
	char arg[MAX_STRING_LENGTH];
	
	if ( IS_NPC(ch) || !ch->pcdata )
	{
		send_to_char( "Yeah right!\n\r", ch );
		return;
	}
	if ( argument[0] == '\0' )
	{
		send_to_char( "&RIf you really want to delete this character type suicide and your password.\n\r", ch );
		send_to_char( "&RWith an optional phrase after the password for your own custom suicide message.\n\r",ch);
		return;
	}
	argument = one_argument(argument, arg);
	if ( strcmp( crypt( arg, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
		send_to_char( "Sorry wrong password.\n\r", ch );
		sprintf( logbuf , "%s attempting to commit suicide... WRONG PASSWORD!" , ch->name );
		log_string( logbuf );
		return;
	}
	if ( argument[0] == '\0' )
	{
		act( AT_BLOOD, "With a sad determination and trembling hands you slit your own throat!",  ch, NULL, NULL, TO_CHAR    );
		act( AT_BLOOD, "Cold shivers run down your spine as you watch $n slit $s own throat!",  ch, NULL, NULL, TO_ROOM );
	}
	else
	{
		act( AT_BLOOD, "$G $T", ch, NULL, argument, TO_ROOM );
		act( AT_BLOOD, "$G $T", ch, NULL, argument, TO_CHAR );
	}
	sprintf( logbuf , "%s has commited suicide." , ch->name );
	log_string( logbuf );
	set_cur_char(ch);
	raw_kill( ch, ch );
}

void do_bank( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
    long amount = 0;
    
    argument = one_argument( argument , arg1 );
    argument = one_argument( argument , arg2 );
	argument = one_argument( argument , arg3 );
    
    if ( IS_NPC(ch) || !ch->pcdata )
       return;
    
    if (!ch->in_room || !xIS_SET(ch->in_room->room_flags, ROOM_BANK) )
    {
       send_to_char( "You must be in a bank to do that!\n\r", ch );
       return;
    }

    if ( arg1[0] == '\0' )
    {
		if ( ch->pcdata->clan )
		{ ch_printf(ch,"To Deposit/Withdraw from clan funds:\n\rUsage: BANK clan <deposit|withdraw|ballance> [amount]\n\r"); }
       send_to_char( "Usage: BANK <deposit|withdraw|ballance> [amount]\n\r", ch );
       return;
    }

	if ( !str_cmp( arg1 , "clan" ) )
	{
		CLAN_DATA * clan;
		if ( !ch->pcdata->clan )
		{
			send_to_char( "Your not in a clan.\n\r",ch);
			return;
		}
		if (arg3[0] != '\0' )
			amount = atoi(arg3);

	    clan = ch->pcdata->clan;

		if ( !str_prefix( arg2 , "deposit" ) )
		{
			if ( amount  <= 0 )
			{
				send_to_char( "You may only deposit amounts greater than zero.\n\r", ch );
				do_bank( ch , "" );
				return;
			}
       
			if ( ch->gold < amount )
			{
				send_to_char( "You don't have that many credits on you.\n\r", ch );
				return;
			}
       
			ch->gold -= amount;
			clan->funds += amount;
       
			ch_printf( ch , "You deposit %ld credits into your clans account.\n\r" ,amount );
			return;
		}
		else if ( !str_prefix( arg2 , "withdraw" ) )
		{
			if ( !ch->pcdata->clan )
			{
				send_to_char( "Your not in a clan.\n\r",ch);
				return;
			}
			if ( (ch->pcdata && ch->pcdata->bestowments
				&&    is_name("withdraw", ch->pcdata->bestowments))
				||   !str_cmp( ch->name, ch->pcdata->clan->leader  ))
				;
			else
			{
				send_to_char( "&RYour organization hasn't seen fit to bestow you with that ability." ,ch );
				return;
			}
			if ( amount  <= 0 )
			{
				send_to_char( "You may only withdraw amounts greater than zero.\n\r", ch );
				do_bank( ch , "" );
				return;
			}
			if ( clan->funds < amount )
			{
				send_to_char( "Your clan doesn't have that many credits in its account.\n\r", ch );
				return;
			}
			ch->gold += amount;
			clan->funds -= amount;
       
			ch_printf( ch , "You withdraw %ld credits from your clans account.\n\r" ,amount );
			return;
		}
		else if ( !str_prefix( arg2 , "ballance" ) )
		{
			if ( !ch->pcdata->clan )
			{
				send_to_char( "Your not in a clan.\n\r",ch);
				return;
			}
			ch_printf( ch , "Your clan has %ld credits in its account.\n\r" , clan->funds );
			return; 
		}
		else
		{
			do_bank( ch , "" );
			return;
		}

	}
	else {
		if (arg2[0] != '\0' )
			amount = atoi(arg2);

		if ( !str_prefix( arg1 , "deposit" ) )
		{
			if ( amount  <= 0 )
			{
				send_to_char( "You may only deposit amounts greater than zero.\n\r", ch );
				do_bank( ch , "" );
				return;
			}
       
			if ( ch->gold < amount )
			{
				send_to_char( "You don't have that many credits on you.\n\r", ch );
				return;
			}
       
			ch->gold -= amount;
			ch->pcdata->bank += amount;
       
			ch_printf( ch , "You deposit %ld credits into your account.\n\r" ,amount );
			return;
		}
		else if ( !str_prefix( arg1 , "withdraw" ) )
		{
			if ( amount  <= 0 )
			{
				send_to_char( "You may only withdraw amounts greater than zero.\n\r", ch );
				do_bank( ch , "" );
				return;
			}
			if ( ch->pcdata->bank < amount )
			{
				send_to_char( "You don't have that many credits in your account.\n\r", ch );
				return;
			}
			ch->gold += amount;
			ch->pcdata->bank -= amount;
       
			ch_printf( ch , "You withdraw %ld credits from your account.\n\r" ,amount );
			return;
		}
		else if ( !str_prefix( arg1 , "ballance" ) )
		{
			ch_printf( ch , "You have %ld credits in your account.\n\r" , ch->pcdata->bank );
			return; 
		}
		else
		{
			do_bank( ch , "" );
			return;
		}
	}   
}

void do_rap( CHAR_DATA *ch, char *argument )
{
	EXIT_DATA *pexit;
	char       arg [ MAX_INPUT_LENGTH ];
	char       buf [ MAX_INPUT_LENGTH ];
	
	one_argument( argument, arg );
	
	if ( arg[0] == '\0' )
	{
		send_to_char( "Rap on what?\n\r", ch );
		return;
	}
	if ( ch->fighting )
	{
		send_to_char( "You have better things to do with your hands right now.\n\r", ch );
		return;
	}
	if ( ( pexit = get_exit(ch->in_room,get_dir(arg) )) != NULL)
	{
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA       *pexit_rev;
		char            *keyword;
		
		if ( (to_room = pexit->to_room) != NULL && xIS_SET( to_room->room_flags , ROOM_EMPTY_HOME ) )
		{
			send_to_char( "No Need to use the intercom, nobody lives there!\n\r", ch);
			return;   
		}
		else if ( (to_room = pexit->to_room) != NULL 
			&& !xIS_SET(to_room->room_flags,ROOM_PLR_HOME) 
			&& !xIS_SET(to_room->room_flags,ROOM_HOUSE) )
		{
			send_to_char( "Nobody Owns That Home!\n\r", ch);
			return;   
		}
		keyword = capitalize( dir_name[pexit->vdir] );
		act( AT_ACTION, "You use the intercom to the $d.", ch, NULL, keyword, TO_CHAR );
		act( AT_ACTION, "$n uses the intercom to the $d.", ch, NULL, keyword, TO_ROOM );
		if ( (to_room = pexit->to_room) != NULL && (pexit_rev = pexit->rexit) != NULL 
			&& pexit_rev->to_room        == ch->in_room )
		{
			sprintf( buf, "%s uses the intercom from outside!\n\r", ch->name );
			echo_to_room( AT_ACTION , to_room , buf );
			ch->buzzedfrom = ch->in_room->vnum;
			ch->buzzed = pexit->to_room->vnum;
		}
	}
	else
	{
		send_to_char("There is no home there!\n\r",ch);
	}
	return;
}

void do_invite( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	ROOM_INDEX_DATA *room;
	CHAR_DATA *rch;
	
	one_argument( argument, arg );
	
	if (IS_NPC(ch))
		return;
	
	if ( arg[0] == '\0' )
	{
		send_to_char( "invite whom?\n\r", ch );
		return;
	}
	if ( ch->fighting )
	{
		send_to_char( "You have better things to do with your hands right now.\n\r", ch );
		return;
	}
	if (!xIS_SET(ch->in_room->room_flags,ROOM_HOUSE) )
	{
		if (ch->pcdata->plr_home && ch->pcdata->plr_home->vnum != ch->in_room->vnum)
		{
			send_to_char("You do not own this home",ch);
			return;
		}
	}
	else
	{
		HOME_DATA *home;
		if ( ( home = home_from_entrance(ch->in_room->vnum) ) == NULL )
		{
			bug("No Home here(%d)",ch->in_room->vnum,0);
			return;
		}
		if ( home && !check_member(ch,home))
		{
			send_to_char("You do not own this home",ch);
			return;
		}
	}
	
	rch = get_char_world(ch, arg);
	if (!rch)
	{
		send_to_char("There is no-one there!\n\r",ch);
		return;
	}
	if ( !rch->buzzed || !rch->buzzedfrom )
		return;
	if ( rch->buzzed != ch->in_room->vnum || rch->buzzedfrom != rch->in_room->vnum )
	{
		ch_printf(ch,"%s doesn't want in!\n\r", he_she[rch->sex] );
		return;
	}
	
	room = ch->in_room;
	ch_printf( rch, "%s invites you in!\n\r", ch->name );
	ch_printf( ch, "You invite %s in!\n\r", rch->name );
	
	char_from_room( rch );
	char_to_room( rch , ch->in_room );
	
	act( AT_ACTION, "$N gets invited into $n's room!", ch, NULL, rch, TO_NOTVICT);
	
	return;
}

void do_sellhome( CHAR_DATA *ch, char *argument )
{
     ROOM_INDEX_DATA *room;
     
     if ( !ch->in_room )
         return;
         
     if ( IS_NPC(ch) || !ch->pcdata )
         return;
         
     if ( ch->pcdata->plr_home == NULL )
     {
         send_to_char( "&RYou do not have a home to sell!\n\r&w", ch);
         return;   
     }
     
	 if ( ch->pcdata->plr_home )
    {
		 ch->gold += 75000;
      room = ch->pcdata->plr_home; 
	  
      STRFREE( room->name );
      room->name = STRALLOC( "An Empty Apartment" );

      xREMOVE_BIT( room->room_flags , ROOM_PLR_HOME );
      xSET_BIT( room->room_flags , ROOM_EMPTY_HOME );
     
      fold_area( room->area, room->area->filename, FALSE );      
	  ch->pcdata->plr_home = NULL;
    }

	 do_save( ch , "" );

}

void do_cyber(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *mob;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int cost;

    if ( IS_NPC(ch) )
	return;

    /* check for surgeon */
    for ( mob = ch->in_room->first_person; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC(mob) && IS_SET(mob->act, ACT_IS_SURGEON) )
            break;
    }

    /* if there are no surgeon, display the characters enhancements*/ 
    if ( mob == NULL )
    {
		sprintf(buf, "Cyber Parts:\n\r %s\n\r", 
			cyber_bit_name(ch->pcdata->cyber));
		send_to_char(buf,ch);
		return;
    }

    one_argument(argument,arg);

	/* if there are a surgeon, give a list*/
    if (arg[0] == '\0')
    {
        /* display price list */
	do_say( mob, "I have these parts in stock:" );
	do_say( mob,"  comm    : Internal Comm Unit        750000 creds");
	do_say( mob,"  eyes    : Infrared Eyes            1000000 creds");
	do_say( mob,"  legs    : Cyber Replacements       1250000 creds");
	do_say( mob,"  reflex  : Augmented Reflexes       1500000 creds");
	do_say( mob,"  mind    : Internal Computer        1600000 creds");
	do_say( mob,"  muscle  : Augmented Muscles        1600000 creds");
	do_say( mob,"  chest   : External Chest Plating   1750000 creds");
	do_say( mob,"  reactor : Internal Reactor         2000000 creds"); 
	do_say( mob,"  sterile : cybernetic sterliation   1150000 creds"); 
	do_say( mob," Type cyber <type> to buy one, or help cyber to get more info.");
	return;
    }

    /* Lets see what the character wants to have */
    if (!str_prefix(arg,"comm"))
    {
		cost  = 750000;
		if (cost > (ch->gold))
		{
			do_say( mob, "You do not have enough creds for my services." );
			return;
		}
		if (IS_SET(ch->pcdata->cyber,CYBER_COMM))
		{
			do_say(mob,"You already got that part." );
			return;
		}
		SET_BIT (ch->pcdata->cyber, CYBER_COMM );
	}

    else if (!str_prefix(arg,"eyes"))
    {
		cost  = 1000000;
		if (cost > (ch->gold))
		{
			do_say( mob, "You do not have enough creds for my services." );
			return;
		}
		if (IS_SET(ch->pcdata->cyber,CYBER_EYES))
		{
			do_say(mob,"You already got that part." );
			return;
		}
		SET_BIT (ch->pcdata->cyber, CYBER_EYES );	
		ch->affected_by	  = AFF_INFRARED;
	}
    else if (!str_prefix(arg,"legs"))
    {
		cost = 1250000;
		if (cost > (ch->gold))
		{
			do_say( mob, "You do not have enough creds for my services." );
			return;
		}
		if (IS_SET(ch->pcdata->cyber,CYBER_LEGS))
		{
			do_say(mob,"You already got that part." );
			return;
		}
		SET_BIT (ch->pcdata->cyber, CYBER_LEGS );
		ch->max_move += number_range ( 200 , 500 );
	}
    else if (!str_prefix(arg,"chest"))
	{
		cost  = 1750000;
		if (cost > (ch->gold))
		{
			do_say( mob, "You do not have enough creds for my services." );
			return;
		}
		if (IS_SET(ch->pcdata->cyber,CYBER_CHEST))
		{
			do_say(mob,"You already got that part." );
			return;
		}
		SET_BIT (ch->pcdata->cyber, CYBER_CHEST );
	}
	else if (!str_prefix(arg,"reflex"))
	{
		cost = 1500000;
		if (cost > (ch->gold))
		{
			do_say( mob, "You do not have enough creds for my services." );
			return;
		}
		if (IS_SET(ch->pcdata->cyber,CYBER_REFLEXES))
		{
			do_say(mob,"You already got that part." );
			return;
		}
		SET_BIT (ch->pcdata->cyber, CYBER_REFLEXES );
	}
	else if (!str_prefix(arg,"reactor"))
	{
		cost = 2000000;
		if (cost > (ch->gold))
		{
			do_say( mob, "You do not have enough creds for my services." );
			return;
		}
		if (IS_SET(ch->pcdata->cyber,CYBER_REACTOR))
		{
			do_say(mob,"You already got that part." );
			return;
		}
		SET_BIT (ch->pcdata->cyber, CYBER_REACTOR );
	}
	else if (!str_prefix(arg,"mind"))
    {
		cost = 450000;
		if (cost > (ch->gold))
		{
			do_say( mob, "You do not have enough creds for my services." );
			return;
		}
		if (IS_SET(ch->pcdata->cyber,CYBER_MIND))
		{
			do_say(mob,"You already got that part." );
			return;
		}
		SET_BIT (ch->pcdata->cyber, CYBER_MIND );
	}
	
	else if (!str_prefix(arg,"muscle"))
    {
		cost = 1600000;
		if (cost > (ch->gold))
		{
			do_say( mob, "You do not have enough creds for my services." );
			return;
		}
		if (IS_SET(ch->pcdata->cyber,CYBER_STRENGTH))
		{
			do_say(mob,"You already got that part." );
			return;
		}
		SET_BIT (ch->pcdata->cyber, CYBER_STRENGTH );
	}
	else if (!str_prefix(arg,"sterile"))
	{
		cost = 1150000;
		if (cost > (ch->gold))
		{
			do_say( mob, "You do not have enough creds for my services.");
			return;
		}
		if (IS_SET(ch->pcdata->cyber,CYBER_STERILE))
		{
			do_say(mob,"You already have had that operation");
			return;
		}
		SET_BIT(ch->pcdata->cyber,CYBER_STERILE);
	}
	else 
    {
		do_say( mob, "Type 'cyber' for a list of cybernetics." );
		return;
	}

    WAIT_STATE(ch,PULSE_VIOLENCE);

    ch->gold -= cost;
    do_say( mob,"There we go, better then then the original." ); 
}

void do_homeid( CHAR_DATA *ch, char *argument )
{
	HOME_DATA * home;
	
	if ( !ch->in_room )
		return;
	
	if ( IS_NPC(ch) || !ch->pcdata )
		return;
		
	if ( ch->pcdata->plr_home != NULL )
		ch_printf( ch, "Your home ID number is %d.\n\r" , ch->pcdata->plr_home->vnum );
	
	if ( ( home = home_from_vnum( ch->in_room->vnum ) ) != NULL )
		ch_printf( ch, "You are in a home now (%s).\n\r" , home->name );
	else
		ch_printf( ch, "You are not in a home right now.\n\r" );
}

/*
 * Extended Bitvector Routines					-Thoric
 */

/* check to see if the extended bitvector is completely empty */
bool ext_is_empty( EXT_BV *bits )
{
    int x;

    for ( x = 0; x < XBI; x++ )
	if ( bits->bits[x] != 0 )
	    return FALSE;

    return TRUE;
}

void ext_clear_bits( EXT_BV *bits )
{
    int x;

    for ( x = 0; x < XBI; x++ )
	bits->bits[x] = 0;
}

/* for use by xHAS_BITS() -- works like IS_SET() */
int ext_has_bits( EXT_BV *var, EXT_BV *bits )
{
    int x, bit;

    for ( x = 0; x < XBI; x++ )
	if ( (bit=(var->bits[x] & bits->bits[x])) != 0 )
	    return bit;

    return 0;
}

/* for use by xSAME_BITS() -- works like == */
bool ext_same_bits( EXT_BV *var, EXT_BV *bits )
{
    int x;

    for ( x = 0; x < XBI; x++ )
	if ( var->bits[x] != bits->bits[x] )
	    return FALSE;

    return TRUE;
}

/* for use by xSET_BITS() -- works like SET_BIT() */
void ext_set_bits( EXT_BV *var, EXT_BV *bits )
{
    int x;

    for ( x = 0; x < XBI; x++ )
	var->bits[x] |= bits->bits[x];
}

/* for use by xREMOVE_BITS() -- works like REMOVE_BIT() */
void ext_remove_bits( EXT_BV *var, EXT_BV *bits )
{
    int x;

    for ( x = 0; x < XBI; x++ )
	var->bits[x] &= ~(bits->bits[x]);
}

/* for use by xTOGGLE_BITS() -- works like TOGGLE_BIT() */
void ext_toggle_bits( EXT_BV *var, EXT_BV *bits )
{
    int x;

    for ( x = 0; x < XBI; x++ )
	var->bits[x] ^= bits->bits[x];
}

/*
 * Read an extended bitvector from a file.			-Thoric
 */
EXT_BV fread_bitvector( FILE *fp )
{
    EXT_BV ret;
    int c, x = 0;
    int num = 0;
    
    memset( &ret, '\0', sizeof(ret) );
    for ( ;; )
    {
	num = fread_number(fp);
	if ( x < XBI )
	    ret.bits[x] = num;
	++x;
	if ( (c=getc(fp)) != '&' )
	{
	    ungetc(c, fp);
	    break;
	}
    }

    return ret;
}

/* return a string for writing a bitvector to a file */
char *print_bitvector( EXT_BV *bits )
{
    static char buf[XBI * 12];
    char *p = buf;
    int x, cnt = 0;

    for ( cnt = XBI-1; cnt > 0; cnt-- )
	if ( bits->bits[cnt] )
	    break;
    for ( x = 0; x <= cnt; x++ )
    {
	sprintf(p, "%d", bits->bits[x]);
	p += strlen(p);
	if ( x < cnt )
	    *p++ = '&';
    }
    *p = '\0';

    return buf;
}

/*
 * Write an extended bitvector to a file			-Thoric
 */
void fwrite_bitvector( EXT_BV *bits, FILE *fp )
{
    fputs( print_bitvector(bits), fp );
}


EXT_BV meb( int bit )
{
    EXT_BV bits;

    xCLEAR_BITS(bits);
    if ( bit >= 0 )
	xSET_BIT(bits, bit);

    return bits;
}


EXT_BV multimeb( int bit, ... )
{
    EXT_BV bits;
    va_list param;
    int b;
    
    xCLEAR_BITS(bits);
    if ( bit < 0 )
	return bits;

    xSET_BIT(bits, bit);

    va_start(param, bit);

    while ((b = va_arg(param, int)) != -1)
	xSET_BIT(bits, b);

    va_end(param);

    return bits;
}

void do_clone( CHAR_DATA *ch, char *argument )
{
	long credits;
	long quest;
	char clanname[MAX_STRING_LENGTH];
	char bestowments[MAX_STRING_LENGTH];
	int flags,bank;
	ROOM_INDEX_DATA *home;
	MOB_INDEX_DATA *vendor;
	int package,value;
	bool success = TRUE;
	
	if ( IS_NPC(ch) )
	{
		ch_printf( ch, "Yeah right!\n\r" );
		return;
	}
	if ( ch->in_room->vnum != 10001 )
	{
		ch_printf( ch, "You can't do that here!\n\r" );
		return;
	}
	if ( argument[0] == '\0' ) 
	{
		ch_printf( ch, "Syntax: clone [Package #]\n\r");
		ch_printf( ch, "Please Choose a cloning Package.\n\r" );
		ch_printf( ch, "At this time we have the following packages:\n\r" );
		ch_printf( ch, "[  1000] [Package 1] Back Room Job.\n\r");
		ch_printf( ch, "\tHigh chance of failed Cloning\n\r");
		ch_printf( ch, "\tLeaves Nothing For The New Clone\n\r");
		ch_printf( ch, "[ 30000] [Package 2] Cloning By Interns.\n\r");
		ch_printf( ch, "\tAverage chance of failed Cloning\n\r");
		ch_printf( ch, "\tLeaves the new clone 1k credits and a comlink\n\r");
		ch_printf(ch, "&B[&W 75000&B]&W &B[&WPackage 3&B]&W Cloning By New Doctors\n\r");
		ch_printf(ch, "\tBelow Average chance of failed Cloning(Thats good)\n\r");
		ch_printf( ch, "[100000] [Package 4] Cloning By Respectable Scientits.\n\r");
		ch_printf( ch, "\tNext to no chance of failed Cloning\n\r");
		ch_printf( ch, "\tLeaves the new clone 10k credits, a comlink, and a blaster \n\r");
		return;
	}
	if ( argument[0] != '\0' ) 
	{
		value = atoi(argument);
		if ( value == 1 ) 
		{
			if ( ch->gold < 1000 ) 
			{
				ch_printf( ch, "You don't have enough credits... You need %d.\n\r" , 1000 );
				return;
			}
			else
			{
				ch->gold -= 1000;
				ch_printf( ch, "You pay %d credits for cloning.\n\r", 1000 );
				ch_printf( ch, "You are escorted into a small room.\n\r\n\r" );
				char_from_room( ch );
				char_to_room( ch, get_room_index( 10011 ) );
				package = 1;
			}
		}
		else if ( value == 2 ) 
		{
			if ( ch->gold < 30000 ) 
			{
				ch_printf( ch, "You don't have enough credits... You need %d.\n\r", 30000 );
				return;
			}
			else
			{
				ch->gold -= 30000;
				ch_printf( ch, "You pay %d credits for cloning.\n\r", 30000 );
				ch_printf( ch, "You are escorted into a small room.\n\r\n\r");
				char_from_room( ch );
				char_to_room( ch, get_room_index( 10012 ) );
				package = 2;
			}
		}
		else if ( value == 3 ) 
		{
			if ( ch->gold < 75000 )
			{
				ch_printf( ch, "You do not have enough credits for this operation\n\r");
				return;
			}
			else
			{
				ch->gold -= 75000;
				ch_printf( ch, "You pay %d credits for cloning!\n\r", 75000);
				ch_printf( ch, "You are escored into a small room\n\r");
				char_from_room( ch );
				char_to_room( ch, get_room_index( 10013 ) );
				package = 3;
			}
		}
		else if ( value == 4 )
		{
			if ( ch->gold < 100000 )
			{
				ch_printf( ch, "You do not have enough credits... You need %d.\n\r", 100000 );
				return;
			}
			else
			{
				ch->gold -= 100000;
				ch_printf( ch, "You pay %d credits for cloning.\n\r", 100000 );
				ch_printf( ch, "You are escorted into a small room.\n\r\n\r");
				char_from_room( ch );
				char_to_room( ch, get_room_index( 10014 ) );
				package = 4;
			}
		}
		else
		{
			ch_printf( ch, "Syntax: clone [Package #]\n\r");
			return;
		}
	}
	 
	value = number_percent();
	if ( package == 1 && value <= 75 ) { success = FALSE; }
	else if ( package == 2 && value <= 40 ) { success = FALSE; }
	else if ( package == 3 && value <= 25 ) { success = FALSE; }
	else if ( package == 4 && value <= 5 ) { success = FALSE; }
	else if ( package > 4 ) { bug( "Out of package range clonning package", 0); return; }
	else { success = TRUE; }

	if ( success)
	{
		flags   = ch->act;
		
		bank = ch->pcdata->bank;
		ch->pcdata->bank = 0;
		credits = ch->gold;
		ch->gold = 0;
		if ( package == 4 )
			ch->gold = 10000;
		quest =  ch->pcdata->quest_curr;
		ch->pcdata->quest_curr = 0;
		home = ch->pcdata->plr_home;
		ch->pcdata->plr_home = NULL;
		vendor = ch->pcdata->vendor;
		ch->pcdata->vendor = NULL;
		
		if ( ch->pcdata->clan_name && ch->pcdata->clan_name[0] != '\0' )
		{
			strcpy( clanname, ch->pcdata->clan_name);
			STRFREE( ch->pcdata->clan_name );
			ch->pcdata->clan_name = STRALLOC( "" );
			strcpy( bestowments, ch->pcdata->bestowments);
			DISPOSE( ch->pcdata->bestowments );
			ch->pcdata->bestowments = str_dup( "" );
			ch->pcdata->pdeaths++;
			save_clone( ch, package );
			STRFREE( ch->pcdata->clan_name );
			ch->pcdata->clan_name = STRALLOC( clanname );
			DISPOSE( ch->pcdata->bestowments );
			ch->pcdata->bestowments = str_dup( bestowments );
		} 
		else
		{
			ch->pcdata->pdeaths++;
			save_clone( ch, package );
		}
		ch->pcdata->plr_home = home;
		ch->gold = credits;    
		ch->act = flags;
		ch->pcdata->quest_curr = quest;
		ch->pcdata->bank = bank;
		ch->pcdata->vendor = vendor;
	}
	
	if ( package == 1 )
	{
		char_from_room( ch );
		char_to_room( ch, get_room_index( 10015 ) );     
	}
	else if ( package == 2 )
	{
		char_from_room( ch );
		char_to_room( ch, get_room_index( 10016 ) );     
	}
	else if ( package == 3 )
	{
		char_from_room( ch );
		char_to_room( ch, get_room_index( 10017 ) );
	}
	else if ( package == 4 )
	{
		char_from_room( ch );
		char_to_room( ch, get_room_index( 10018 ) );     
	}
	else
	{
		char_from_room( ch );
		char_to_room( ch, get_room_index( 10002 ) );     
	}
	do_look( ch , "" );
	send_to_char( "\n\r&WA small tissue sample is taken from your arm.\n\r"
		"&ROuch!\n\r\n\r", ch );
	
	if ( success)
	{	
		ch->pcdata->pdeaths--;
		send_to_char( "&WYou have been succesfully cloned.\n\r", ch );
	}
	else
	{
		send_to_char( "&WThe cloning process failed.\n\r", ch);
	}
	ch->hit--;
}


void do_knock( CHAR_DATA *ch, char *argument)
{
	EXIT_DATA *pexit;
	char arg [ MAX_INPUT_LENGTH ];
	
	one_argument( argument, arg );
	
	if ( arg[0] == '\0' )
	{
		send_to_char( "Knock on what?\n\r", ch );
		return;
	}
	if ( ch->fighting )
	{
		send_to_char( "You have better things to do with your hands right now.\n\r", ch );
		return;
	}
	if ( ( pexit = find_door( ch, arg, FALSE ) ) != NULL )
	{
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA       *pexit_rev;
		char            *keyword;
		if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
		{
			send_to_char( "Why knock?  It's open.\n\r", ch );
			return;
		}
		if ( IS_SET( pexit->exit_info, EX_SECRET ) )
			keyword = "wall";
		else
			keyword = pexit->keyword;
		act( AT_ACTION, "You knock loudly on the $d.", ch, NULL, keyword, TO_CHAR );
		act( AT_ACTION, "$n knock loudly on the $d.", ch, NULL, keyword, TO_ROOM );
		if ( (to_room = pexit->to_room) != NULL
			&& ( pexit_rev = pexit->rexit) != NULL 
			&& pexit_rev->to_room == ch->in_room )
		{
			CHAR_DATA *rch;
			for ( rch = to_room->first_person; rch; rch = rch->next_in_room )
			{
				act( AT_ACTION, "Someone knock loudly from the other side of the $d.",
					rch, NULL, pexit_rev->keyword, TO_CHAR );
			}
		}
	}
	else
	{
		act( AT_ACTION, "You make knocking motions through the air.",
			ch, NULL, NULL, TO_CHAR );
		act( AT_ACTION, "$n makes knocking motions through the air.",
			ch, NULL, NULL, TO_ROOM );
	}
	return;
}
