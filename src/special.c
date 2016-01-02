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
*			   "Special procedure" module			   *
****************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "mud.h"

/* jails for wanted flags */

bool  remove_obj      args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );

/*
 * The following special functions are available for mobiles.
 */
DECLARE_SPEC_FUN(	spec_fido		);
DECLARE_SPEC_FUN(	spec_guardian		);
DECLARE_SPEC_FUN(	spec_janitor		);
DECLARE_SPEC_FUN(	spec_poison		);
DECLARE_SPEC_FUN(	spec_thief		);
DECLARE_SPEC_FUN(	spec_customs_smut	);
DECLARE_SPEC_FUN(	spec_customs_alcohol	);
DECLARE_SPEC_FUN(	spec_customs_weapons	);
DECLARE_SPEC_FUN(	spec_customs_spice	);
DECLARE_SPEC_FUN(	spec_police_attack	);
DECLARE_SPEC_FUN(	spec_police_jail	);
DECLARE_SPEC_FUN(	spec_police_fine	);
DECLARE_SPEC_FUN(	spec_police     	);
DECLARE_SPEC_FUN(       spec_clan_guard         );
DECLARE_SPEC_FUN(       spec_questmaster        );
DECLARE_SPEC_FUN(	spec_clan_patrol	);
DECLARE_SPEC_FUN(	spec_player_patrol	);
DECLARE_SPEC_FUN(       spec_finer        );
DECLARE_SPEC_FUN(	spec_droid_lang            );
DECLARE_SPEC_FUN(	spec_tax			);
DECLARE_SPEC_FUN(   spec_clan_order );



/*
 * Given a name, return the appropriate spec fun.
 */
SPEC_FUN *spec_lookup( const char *name )
{
    if ( !str_cmp( name, "spec_fido"		  ) ) return spec_fido;
    if ( !str_cmp( name, "spec_guardian"	  ) ) return spec_guardian;
    if ( !str_cmp( name, "spec_janitor"		  ) ) return spec_janitor;
    if ( !str_cmp( name, "spec_poison"		  ) ) return spec_poison;
    if ( !str_cmp( name, "spec_thief"		  ) ) return spec_thief;
    if ( !str_cmp( name, "spec_customs_smut"  ) ) return spec_customs_smut;
    if ( !str_cmp( name, "spec_customs_alcohol"  ) ) return spec_customs_alcohol;
    if ( !str_cmp( name, "spec_customs_weapons"  ) ) return spec_customs_weapons;
    if ( !str_cmp( name, "spec_customs_spice"  ) ) return spec_customs_spice;
    if ( !str_cmp( name, "spec_police_attack"  ) ) return spec_police_attack;
    if ( !str_cmp( name, "spec_police_jail"  ) ) return spec_police_jail;
    if ( !str_cmp( name, "spec_police_fine"	))		return spec_police_fine;
    if ( !str_cmp( name, "spec_police"		))		return spec_police;
    if ( !str_cmp( name, "spec_clan_guard"	))		return spec_clan_guard;
	if ( !str_cmp( name, "spec_questmaster" ))		return spec_questmaster;
	if ( !str_cmp( name, "spec_clan_patrol" ))		return spec_clan_patrol;
	if ( !str_cmp( name, "spec_player_patrol"))		return spec_player_patrol;
	if ( !str_cmp( name, "spec_finer"		))		return spec_finer;
	if ( !str_cmp( name, "spec_droid_lang"  ))		return spec_droid_lang;
	if ( !str_cmp( name, "spec_tax"			))		return spec_tax;
	if ( !str_cmp( name, "spec_clan_order"	))		return spec_clan_order;

    return 0;
}

/*
 * Given a pointer, return the appropriate spec fun text.
 */
char *lookup_spec( SPEC_FUN *special )
{
    if ( special == spec_fido		)	return "spec_fido";
    if ( special == spec_guardian	)	return "spec_guardian";
    if ( special == spec_janitor	)	return "spec_janitor";
    if ( special == spec_poison		)	return "spec_poison";
    if ( special == spec_thief		)	return "spec_thief";
    if ( special == spec_customs_smut     )	return "spec_customs_smut";
    if ( special == spec_customs_weapons     )	return "spec_customs_weapons";
    if ( special == spec_customs_alcohol     )	return "spec_customs_alcohol";
    if ( special == spec_customs_spice     )	return "spec_customs_spice";
    if ( special == spec_police_attack     )	return "spec_police_attack";
    if ( special == spec_police_jail     )	return "spec_police_jail";
    if ( special == spec_police_fine     )	return "spec_police_fine";
    if ( special == spec_police          )	return "spec_police";
    if ( special == spec_clan_guard      )      return "spec_clan_guard";
	if ( special == spec_questmaster    )       return "spec_questmaster";
	if ( special == spec_clan_patrol      )      return "spec_clan_patrol";
	if ( special == spec_player_patrol      )      return "spec_player_patrol";
	if ( special == spec_finer      )      return "spec_finer";
	if ( special == spec_droid_lang        )    return "spec_droid_lang";
	if ( special == spec_tax		)		return "spec_tax";
	if ( special == spec_clan_order)		return "spec_clan_order";
	
    return "";
}

bool spec_questmaster( CHAR_DATA *ch )
{
 if ( !IS_NPC(ch) )
  return FALSE;
 else
  return TRUE;
}

bool spec_finer( CHAR_DATA *ch )
{
 if ( !IS_NPC(ch) )
  return FALSE;
 else
  return TRUE;
}

bool spec_customs_smut( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    OBJ_DATA  *obj;
    char       buf[MAX_STRING_LENGTH];
    long       ch_exp;
    
    if ( !IS_AWAKE(ch) || ch->position == POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->first_person; victim; victim = v_next )
    {
	v_next = victim->next_in_room;
	
	if ( IS_NPC(victim) || victim->position == POS_FIGHTING )
	   continue;
	
	for ( obj = victim->last_carrying; obj; obj = obj->prev_content )
	{
	    if (obj->pIndexData->item_type == ITEM_SMUT)
	    {
	       if ( victim != ch && can_see( ch, victim ) && can_see_obj( ch,obj ) )
	       {
	          sprintf( buf , "%s is illegal contraband. I'm going to have to confiscate that.", obj->short_descr );
                  do_say( ch , buf );
                  if ( obj->wear_loc != WEAR_NONE )
                     remove_obj( victim, obj->wear_loc, TRUE );
                  separate_obj( obj );
    		  obj_from_char( obj );
    		  act( AT_ACTION, "$n confiscates $p from $N.", ch, obj, victim, TO_NOTVICT );
    		  act( AT_ACTION, "$n takes $p from you.",   ch, obj, victim, TO_VICT    );
    		  obj = obj_to_char( obj, ch );
                  SET_BIT( obj->extra_flags , ITEM_CONTRABAND);
                  ch_exp = UMIN( obj->cost*10 , ( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY])  )   );
                  ch_printf( victim, "You lose %ld experience.\n\r " , ch_exp );
                  gain_exp( victim, 0-ch_exp , SMUGGLING_ABILITY);
                  return TRUE;
	       }
	       else if ( can_see( ch, victim ) && !IS_SET( obj->extra_flags , ITEM_CONTRABAND)  ) 
	       { 
                  ch_exp = UMIN( obj->cost*10 , ( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY])  )   );
                  ch_printf( victim, "You receive %ld experience for smuggling %s.\n\r " , ch_exp , obj->short_descr );
                  gain_exp( victim, ch_exp , SMUGGLING_ABILITY );
       
	          act( AT_ACTION, "$n looks at $N suspiciously.", ch, NULL, victim, TO_NOTVICT );
    		  act( AT_ACTION, "$n look at you suspiciously.",   ch, NULL, victim, TO_VICT  );
    		  SET_BIT( obj->extra_flags , ITEM_CONTRABAND);
	          
	          return TRUE;
	       }
	       else if ( !IS_SET( obj->extra_flags , ITEM_CONTRABAND)  )
	       {
                  ch_exp = UMIN( obj->cost*10 , ( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY])  )   );
                  ch_printf( victim, "You receive %ld experience for smuggling %s.\n\r " , ch_exp, obj->short_descr );
                  gain_exp( victim, ch_exp , SMUGGLING_ABILITY );
       
	          SET_BIT( obj->extra_flags , ITEM_CONTRABAND);
	          return TRUE;
	       }
	    }
	    else if ( obj->item_type == ITEM_CONTAINER )
	    {
	        OBJ_DATA *content;
	        for ( content = obj->first_content; content; content = content->next_content )
	        {
	            if (content->pIndexData->item_type == ITEM_SMUT
	            && !IS_SET( content->extra_flags , ITEM_CONTRABAND ) )
	            {
	               ch_exp = UMIN( content->cost*10 , ( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY])  )   );
                     ch_printf( victim, "You receive %ld experience for smuggling %s.\n\r " , ch_exp , content->short_descr );
                       gain_exp( victim, ch_exp, SMUGGLING_ABILITY );
	               SET_BIT( content->extra_flags , ITEM_CONTRABAND);
	               return TRUE;
	            }
	        }
	    }
	}
	
    }

    return FALSE;
}

bool spec_customs_weapons( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    OBJ_DATA  *obj;
    char       buf[MAX_STRING_LENGTH];
    long       ch_exp;
	CLAN_DATA	*clan;
	CLAN_DATA	*vclan;

    if ( !IS_AWAKE(ch) || ch->position == POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->first_person; victim; victim = v_next )
    {
		v_next = victim->next_in_room;
	
		if ( IS_NPC(victim) || victim->position == POS_FIGHTING )
			continue;

		if ( victim->pcdata && victim->pcdata->clan )
				vclan = victim->pcdata->clan;
		if ( IS_NPC(ch) && ch->mob_clan )
				clan = ch->mob_clan;
		if ( victim->pcdata && vclan && ch->mob_clan && clan && clan == vclan)
			continue;
        	
		for ( obj = victim->last_carrying; obj; obj = obj->prev_content )
		{
			if (obj->pIndexData->item_type == ITEM_WEAPON)
			{
				if ( victim != ch && can_see( ch, victim ) && can_see_obj( ch,obj ) )
				{
	    
					sprintf( buf , "Weapons are banned from non-military usage. I'm going to have to confiscate %s.", obj->short_descr );
					do_say( ch , buf );
					if ( obj->wear_loc != WEAR_NONE )
						remove_obj( victim, obj->wear_loc, TRUE );
					separate_obj( obj );
					obj_from_char( obj );
					if ( obj->value[3] != WEAPON_LIGHTSABER) {
						act( AT_ACTION, "$n confiscates $p from $N.", ch, obj, victim, TO_NOTVICT );
						act( AT_ACTION, "$n takes $p from you.",   ch, obj, victim, TO_VICT    );
					}
					else {
						if ( !IS_NPC(ch) && IS_SET( ch->act , PLR_SOUND ) )
						{
							act( AT_ACTION, "!!SOUND(saberoff.wav U=http://mercury.spaceports.com/~gavin1/)$n confiscates $p from $N.", ch, obj, victim, TO_NOTVICT );
							act( AT_ACTION, "!!SOUND(saberoff.wav U=http://mercury.spaceports.com/~gavin1/)$n takes $p from you.",   ch, obj, victim, TO_VICT    );
						}
						else
						{
							act( AT_ACTION, "$n confiscates $p from $N.", ch, obj, victim, TO_NOTVICT );
							act( AT_ACTION, "$n takes $p from you.",   ch, obj, victim, TO_VICT    );
						}
					}
    		
					obj = obj_to_char( obj, ch );
					SET_BIT( obj->extra_flags , ITEM_CONTRABAND);
					ch_exp = UMIN( obj->cost*10 , ( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY])  )  );
					ch_printf( victim, "You lose %ld experience.\n\r " , ch_exp );
					gain_exp( victim, 0-ch_exp , SMUGGLING_ABILITY);
					return TRUE;
				}
				else if ( can_see( ch, victim ) && !IS_SET( obj->extra_flags , ITEM_CONTRABAND)  ) 
				{ 
					ch_exp = UMIN( obj->cost*10 , ( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY])  )   );
					ch_printf( victim, "You receive %ld experience for smuggling %d.\n\r " , ch_exp, obj->short_descr );
					gain_exp( victim, ch_exp, SMUGGLING_ABILITY );

					act( AT_ACTION, "$n looks at $N suspiciously.", ch, NULL, victim, TO_NOTVICT );
					act( AT_ACTION, "$n look at you suspiciously.",   ch, NULL, victim, TO_VICT  );
					SET_BIT( obj->extra_flags , ITEM_CONTRABAND);
					return TRUE;
				}
				else if ( !IS_SET( obj->extra_flags , ITEM_CONTRABAND)  )
				{
					ch_exp = UMIN( obj->cost*10 , ( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY])  )   );
					ch_printf( victim, "You receive %ld experience for smuggling %s.\n\r " , ch_exp , obj->short_descr);
					gain_exp( victim, ch_exp , SMUGGLING_ABILITY);

					SET_BIT( obj->extra_flags , ITEM_CONTRABAND);
					return TRUE;
				}
			}
			else if ( obj->item_type == ITEM_CONTAINER )
			{
				OBJ_DATA *content;
				for ( content = obj->first_content; content; content = content->next_content )
				{
					if (content->pIndexData->item_type == ITEM_WEAPON
						&& !IS_SET( content->extra_flags , ITEM_CONTRABAND ) )
					{
						ch_exp = UMIN( content->cost*10 , ( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY])  )   );
						ch_printf( victim, "You receive %ld experience for smuggling %s.\n\r " , ch_exp , content->short_descr);
						gain_exp( victim, ch_exp, SMUGGLING_ABILITY );
						SET_BIT( content->extra_flags , ITEM_CONTRABAND);
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}

bool spec_customs_alcohol( CHAR_DATA *ch )
{   
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    OBJ_DATA  *obj;
    char       buf[MAX_STRING_LENGTH];
    int        liquid;
    long       ch_exp;
    
    if ( !IS_AWAKE(ch) || ch->position == POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->first_person; victim; victim = v_next )
    {
	v_next = victim->next_in_room;
	
	if ( IS_NPC(victim) || victim->position == POS_FIGHTING )
	   continue;
	
	for ( obj = victim->last_carrying; obj; obj = obj->prev_content )
	{
	    if (obj->pIndexData->item_type == ITEM_DRINK_CON)
	    { 
	     if ( ( liquid = obj->value[2] ) >= LIQ_MAX )
	        liquid = obj->value[2] = 0;
	     
	     if ( liq_table[ liquid ].liq_affect[COND_DRUNK] > 0 )
	     {
	       if ( victim != ch && can_see( ch, victim ) && can_see_obj( ch,obj ) )
	       {
	          sprintf( buf , "%s is illegal contraband. I'm going to have to confiscate that.", obj->short_descr );
                  do_say( ch , buf );
                  if ( obj->wear_loc != WEAR_NONE )
                    remove_obj( victim, obj->wear_loc, TRUE );
                  separate_obj( obj );
    		  obj_from_char( obj );
    		  act( AT_ACTION, "$n confiscates $p from $N.", ch, obj, victim, TO_NOTVICT );
    		  act( AT_ACTION, "$n takes $p from you.",   ch, obj, victim, TO_VICT    );
    		  obj = obj_to_char( obj, ch );
                  SET_BIT( obj->extra_flags , ITEM_CONTRABAND);
                  ch_exp = UMIN( obj->cost*10 , ( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY])  )   );
                  ch_printf( victim, "You lose %ld experience. \n\r" , ch_exp );
                  gain_exp( victim, 0-ch_exp , SMUGGLING_ABILITY);
                  return TRUE;
	       }
	       else if ( can_see( ch, victim ) && !IS_SET( obj->extra_flags , ITEM_CONTRABAND)  ) 
	       { 
                  ch_exp = UMIN( obj->cost*10 , ( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY])  )   );
                  ch_printf( victim, "You receive %ld experience for smuggling %d. \n\r" , ch_exp , obj->short_descr);
                  gain_exp( victim, ch_exp , SMUGGLING_ABILITY);
       
	          act( AT_ACTION, "$n looks at $N suspiciously.", ch, NULL, victim, TO_NOTVICT );
    		  act( AT_ACTION, "$n look at you suspiciously.",   ch, NULL, victim, TO_VICT  );
    		  SET_BIT( obj->extra_flags , ITEM_CONTRABAND);
	          return TRUE;
	       }
	       else if ( !IS_SET( obj->extra_flags , ITEM_CONTRABAND)  )
	       {
                  ch_exp = UMIN( obj->cost*10 , ( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY])  )   );
                  ch_printf( victim, "You receive %ld experience for smuggling %d. \n\r" , ch_exp , obj->short_descr);
                  gain_exp( victim, ch_exp , SMUGGLING_ABILITY);
       
	          SET_BIT( obj->extra_flags , ITEM_CONTRABAND);
	          return TRUE;
	       }
	     }
	    }
	    else if ( obj->item_type == ITEM_CONTAINER )
	    {
	        OBJ_DATA *content;
	        for ( content = obj->first_content; content; content = content->next_content )
	        {
	            if (content->pIndexData->item_type == ITEM_DRINK_CON
	            && !IS_SET( content->extra_flags , ITEM_CONTRABAND ) )
	            {
	               if ( ( liquid = obj->value[2] ) >= LIQ_MAX )
	                    liquid = obj->value[2] = 0;
	               if ( liq_table[ liquid ].liq_affect[COND_DRUNK] <= 0 )
	                    continue;
	               ch_exp = UMIN( content->cost*10 , ( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY])  )   );
                     ch_printf( victim, "You receive %ld experience for smuggling %d.\n\r " , ch_exp , content->short_descr);
                       gain_exp( victim, ch_exp , SMUGGLING_ABILITY);
	               SET_BIT( content->extra_flags , ITEM_CONTRABAND);
	               return TRUE;
	            }
	        }
	    }
	}
	
    }

    return FALSE;
}

bool spec_customs_spice( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    OBJ_DATA  *obj;
    char       buf[MAX_STRING_LENGTH];
    long       ch_exp;
    
    if ( !IS_AWAKE(ch) || ch->position == POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->first_person; victim; victim = v_next )
    {
	v_next = victim->next_in_room;
	
	if ( IS_NPC(victim) || victim->position == POS_FIGHTING )
	   continue;
	
	for ( obj = victim->last_carrying; obj; obj = obj->prev_content )
	{
	    if (obj->pIndexData->item_type == ITEM_SPICE || obj->pIndexData->item_type  == ITEM_RAWSPICE)
	    {
	       if ( victim != ch && can_see( ch, victim ) && can_see_obj( ch,obj ) )
	       {
	          sprintf( buf , "%s is illegal contraband. I'm going to have to confiscate that.", obj->short_descr );
                  do_say( ch , buf );
                  if ( obj->wear_loc != WEAR_NONE )
                    remove_obj( victim, obj->wear_loc, TRUE );
                  separate_obj( obj );
    		  obj_from_char( obj );
    		  act( AT_ACTION, "$n confiscates $p from $N.", ch, obj, victim, TO_NOTVICT );
    		  act( AT_ACTION, "$n takes $p from you.",   ch, obj, victim, TO_VICT    );
    		  obj = obj_to_char( obj, ch );
                  SET_BIT( obj->extra_flags , ITEM_CONTRABAND);
                  ch_exp = UMIN( obj->cost*10 , ( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY])  )   );
                  ch_printf( victim, "You lose %ld experience. \n\r" , ch_exp );
                  gain_exp( victim, 0-ch_exp , SMUGGLING_ABILITY);
	          return TRUE;
	       }
	       else if ( can_see( ch, victim ) && !IS_SET( obj->extra_flags , ITEM_CONTRABAND)  ) 
	       { 
                  ch_exp = UMIN( obj->cost*10 , ( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY])  )   );
                  ch_printf( victim, "You receive %ld experience for smuggling %s. \n\r" , ch_exp , obj->short_descr);
                  gain_exp( victim, ch_exp , SMUGGLING_ABILITY );
       
	          act( AT_ACTION, "$n looks at $N suspiciously.", ch, NULL, victim, TO_NOTVICT );
    		  act( AT_ACTION, "$n look at you suspiciously.",   ch, NULL, victim, TO_VICT  );
    		  SET_BIT( obj->extra_flags , ITEM_CONTRABAND);
	          return TRUE;
	       }
	       else if ( !IS_SET( obj->extra_flags , ITEM_CONTRABAND)  )
	       {
                  ch_exp = UMIN( obj->cost*10 , ( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY])  )   );
                  ch_printf( victim, "You receive %ld experience for smuggling %s. \n\r" , ch_exp , obj->short_descr);
                  gain_exp( victim, ch_exp , SMUGGLING_ABILITY);
       
	          SET_BIT( obj->extra_flags , ITEM_CONTRABAND);
	          return TRUE;
	       }
	    }
	    else if ( obj->item_type == ITEM_CONTAINER )
	    {
	        OBJ_DATA *content;
	        for ( content = obj->first_content; content; content = content->next_content )
	        {
	            if (content->pIndexData->item_type == ITEM_SPICE
	            && !IS_SET( content->extra_flags , ITEM_CONTRABAND ) )
	            {
	               ch_exp = UMIN( content->cost*10 , ( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY])  )   );
                     ch_printf( victim, "You receive %ld experience for smuggling %s.\n\r " , ch_exp , content->short_descr);
                       gain_exp( victim, ch_exp, SMUGGLING_ABILITY );
	               SET_BIT( content->extra_flags , ITEM_CONTRABAND);
	               return TRUE;
	            }
	        }
	    }
	}
	
    }

    return FALSE;
}

bool spec_police( CHAR_DATA *ch )
{
	CHAR_DATA *victim;
	CHAR_DATA *v_next;
	int vip;
	char buf[MAX_STRING_LENGTH];
	
	if ( !IS_AWAKE(ch) || ch->fighting )
		return FALSE;
	
	for ( victim = ch->in_room->first_person; victim; victim = v_next )
	{
		v_next = victim->next_in_room;
		if ( IS_NPC(victim) )
			continue;
		if ( !can_see( ch, victim ) )
			continue;
		if ( number_bits ( 1 ) == 0 ) 
			continue;
		for ( vip = 0 ; vip < 32 ; vip++ )
		{
			if ( IS_SET ( ch->vip_flags , 1 << vip ) &&  IS_SET( victim->pcdata->wanted_flags , 1 << vip) )
			{
				sprintf( buf , "Hey you're wanted on %s!", planet_flags[vip] );
				do_say( ch , buf );
				REMOVE_BIT( victim->pcdata->wanted_flags , 1 << vip );
				if ( ch->top_level >= victim->top_level )
					multi_hit( ch, victim, TYPE_UNDEFINED );
				else
				{
					act( AT_ACTION, "$n fines $N an enormous amount of money.", ch, NULL, victim, TO_NOTVICT );
					act( AT_ACTION, "$n fines you an enourmous amount of money.",   ch, NULL, victim, TO_VICT    );
					victim->gold /= 2;
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}

bool spec_police_attack( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    int vip;
    char buf[MAX_STRING_LENGTH];
    
    if ( !IS_AWAKE(ch) || ch->fighting )
	return FALSE;

    for ( victim = ch->in_room->first_person; victim; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( IS_NPC(victim) )
	   continue;
        if ( !can_see( ch, victim ) )
	   continue;
        if ( number_bits ( 1 ) == 0 ) 
	   continue;
	for ( vip = 0 ; vip < 32 ; vip++ )
          if ( IS_SET ( ch->vip_flags , 1 << vip ) &&  IS_SET( victim->pcdata->wanted_flags , 1 << vip) ) 
          {
              sprintf( buf , "Hey you're wanted on %s!", planet_flags[vip] );
                  do_say( ch , buf );                 
              REMOVE_BIT( victim->pcdata->wanted_flags , 1 << vip ); 	                   
              multi_hit( ch, victim, TYPE_UNDEFINED );
              return TRUE;         
          }
    
    }

    return FALSE;

}

bool spec_police_fine( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    int vip;
    char buf[MAX_STRING_LENGTH];

    if ( !IS_AWAKE(ch) || ch->fighting )
	return FALSE;

    for ( victim = ch->in_room->first_person; victim; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( IS_NPC(victim) )
	   continue;
        if ( !can_see( ch, victim ) )
	   continue;
        if ( number_bits ( 1 ) == 0 ) 
	   continue;
	for ( vip = 0 ; vip <= 31 ; vip++ )
          if ( IS_SET ( ch->vip_flags , 1 << vip ) &&  IS_SET( victim->pcdata->wanted_flags , 1 << vip) ) 
          {
              sprintf( buf , "Hey you're wanted on %s!", planet_flags[vip] );
                  do_say( ch , buf );
              act( AT_ACTION, "$n fines $N an enormous amount of money.", ch, NULL, victim, TO_NOTVICT );
    	      act( AT_ACTION, "$n fines you an enourmous amount of money.",   ch, NULL, victim, TO_VICT    );
    	      victim->gold /= 2;
    	      REMOVE_BIT( victim->pcdata->wanted_flags , 1 << vip ); 	                   
              return TRUE;         
          }
    
    }

    return FALSE;

}

bool spec_police_jail( CHAR_DATA *ch )
{

    ROOM_INDEX_DATA *jail = NULL;
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
	PLANET_DATA *planet = NULL;
    CLAN_DATA   *clan = NULL;
    int vip;
    char buf[MAX_STRING_LENGTH];

    if ( !IS_AWAKE(ch) || ch->fighting )
	return FALSE;

    for ( victim = ch->in_room->first_person; victim; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( IS_NPC(victim) )
	   continue;
        if ( !can_see( ch, victim ) )
	   continue;
        if ( number_bits ( 1 ) == 0 ) 
	   continue;
	for ( vip = 0 ; vip <= 31 ; vip++ )
          if ( IS_SET ( ch->vip_flags , 1 << vip ) &&  IS_SET( victim->pcdata->wanted_flags , 1 << vip) ) 
          {
			  sprintf( buf , "Hey you're wanted on %s!", planet_flags[vip] );
			  do_say( ch , buf );                 
			  sprintf(buf, "You Are Under Arrest!");
			  do_say( ch , buf );                 
			  sprintf(buf, "You have the right to remain silent, anything you , do , or think, can, and will be used against you in a court of law.");
			  do_say( ch , buf );                 
              
			if (ch->in_room->area->planet)
		  		planet = ch->in_room->area->planet;
			if ( planet )
				clan = planet->governed_by;
			if ( clan )
				jail = get_room_index(clan->jail);

              if ( jail )
              {
                REMOVE_BIT( victim->pcdata->wanted_flags , 1 << vip ); 	                     
                act( AT_ACTION, "$n ushers $N off to jail.", ch, NULL, victim, TO_NOTVICT );
    	        act( AT_ACTION, "$n escorts you to jail.",   ch, NULL, victim, TO_VICT    );
    	        char_from_room( victim );
    	        char_to_room( victim , jail );
    	      }
    	      return TRUE;         
          }
    
    }

    return FALSE;
    
}


bool spec_fido( CHAR_DATA *ch )
{
    OBJ_DATA *corpse;
    OBJ_DATA *c_next;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if ( !IS_AWAKE(ch) )
	return FALSE;

    for ( corpse = ch->in_room->first_content; corpse; corpse = c_next )
    {
	c_next = corpse->next_content;
	if ( corpse->item_type != ITEM_CORPSE_NPC )
	    continue;

    act( AT_ACTION, "$n savagely devours a corpse.", ch, NULL, NULL, TO_ROOM );
	for ( obj = corpse->first_content; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    obj_from_obj( obj );
	    obj_to_room( obj, ch->in_room );
	}
	extract_obj( corpse );
	return TRUE;
    }

    return FALSE;
}

bool spec_guardian( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    CHAR_DATA *ech;
    char *crime;
    int max_evil;

    if ( !IS_AWAKE(ch) || ch->fighting )
	return FALSE;

    max_evil = 300;
    ech      = NULL;
    crime    = "";

    for ( victim = ch->in_room->first_person; victim; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting
	&&   who_fighting( victim ) != ch
	&&   victim->alignment < max_evil )
	{
	    max_evil = victim->alignment;
	    ech      = victim;
	}
    }

    if ( victim && xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
    {
	sprintf( buf, "%s is a %s!  As well as a COWARD!",
		victim->name, crime );
	do_yell( ch, buf );
	return TRUE;
    }

    if ( victim )
    {
	sprintf( buf, "%s is a %s!  PROTECT THE INNOCENT!!",
		victim->name, crime );
	do_shout( ch, buf );
	multi_hit( ch, victim, TYPE_UNDEFINED );
	return TRUE;
    }

    if ( ech )
    {
    act( AT_YELL, "$n screams 'PROTECT THE INNOCENT!!",
	    ch, NULL, NULL, TO_ROOM );
	multi_hit( ch, ech, TYPE_UNDEFINED );
	return TRUE;
    }

    return FALSE;
}

bool spec_janitor( CHAR_DATA *ch )
{
    OBJ_DATA *trash;
    OBJ_DATA *trash_next;

    if ( !IS_AWAKE(ch) )
	return FALSE;

    for ( trash = ch->in_room->first_content; trash; trash = trash_next )
    {
	trash_next = trash->next_content;
	if ( !IS_SET( trash->wear_flags, ITEM_TAKE )
	||    IS_OBJ_STAT( trash, ITEM_BURRIED ) )
	    continue;
	if ( trash->item_type == ITEM_DRINK_CON
	||   trash->item_type == ITEM_TRASH
	||   trash->cost < 10
	||  (trash->pIndexData->vnum == OBJ_VNUM_SHOPPING_BAG
	&&  !trash->first_content) )
	{
	    act( AT_ACTION, "$n picks up some trash.", ch, NULL, NULL, TO_ROOM );
	    obj_from_room( trash );
	    obj_to_char( trash, ch );
	    return TRUE;
	}
    }

    return FALSE;
}



bool spec_poison( CHAR_DATA *ch )
{
    CHAR_DATA *victim;

    if ( ch->position != POS_FIGHTING
    || ( victim = who_fighting( ch ) ) == NULL
    ||   number_percent( ) > 2 * ch->top_level )
	return FALSE;

    act( AT_HIT, "You bite $N!",  ch, NULL, victim, TO_CHAR    );
    act( AT_ACTION, "$n bites $N!",  ch, NULL, victim, TO_NOTVICT );
    act( AT_POISON, "$n bites you!", ch, NULL, victim, TO_VICT    );
    spell_poison( gsn_poison, ch->top_level, ch, victim );
    return TRUE;
}



bool spec_thief( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    int gold, maxgold;

    if ( ch->position != POS_STANDING )
	return FALSE;

    for ( victim = ch->in_room->first_person; victim; victim = v_next )
    {
	v_next = victim->next_in_room;

	if ( IS_NPC(victim)
	||   get_trust(victim) >= LEVEL_IMMORTAL
	||   number_bits( 2 ) != 0
	||   !can_see( ch, victim ) )	/* Thx Glop */
	    continue;

	if ( IS_AWAKE(victim) && number_range( 0, ch->top_level ) == 0 )
	{
	    act( AT_ACTION, "You discover $n's hands in your wallet!",
		ch, NULL, victim, TO_VICT );
	    act( AT_ACTION, "$N discovers $n's hands in $S wallet!",
		ch, NULL, victim, TO_NOTVICT );
	    return TRUE;
	}
	else
	{
	    maxgold = ch->top_level * ch->top_level * 1000;
	    gold = victim->gold
	    	 * number_range( 1, URANGE(2, ch->top_level/4, 10) ) / 100;
	    ch->gold     += 9 * gold / 10;
	    victim->gold -= gold;
	    if ( ch->gold > maxgold )
	    {
		boost_economy( ch->in_room->area, ch->gold - maxgold/2 );
		ch->gold = maxgold/2;
	    }
	    return TRUE;
	}
    }

    return FALSE;
}


bool spec_clan_guard( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
	CLAN_DATA *vclan;
	CLAN_DATA *clan;

    if ( !IS_AWAKE(ch) || ch->fighting || !ch->mob_clan )
	return FALSE;

    for ( victim = ch->in_room->first_person; victim; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( !can_see( ch, victim ) )
	   continue;
        if ( get_timer(victim, TIMER_RECENTFIGHT) > 0 )
	   continue;

		if ( !IS_NPC(victim) && IS_AWAKE(victim) && victim->pcdata && victim->pcdata->clan )
			vclan = victim->pcdata->clan;
		if ( IS_NPC(ch) && ch->mob_clan )
			clan = ch->mob_clan;

		if ( !IS_NPC( victim ) && vclan && IS_AWAKE(victim) 
			&& IS_NPC(ch) && clan != vclan 
			&& nifty_is_name(vclan->name , clan->atwar ) )
		{
			do_yell( ch, "Hey your not allowed in here!" );
			multi_hit( ch, victim, TYPE_UNDEFINED );
			return TRUE;         
		}
	}
    return FALSE;
}

bool spec_player_guard( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;

    if ( !IS_AWAKE(ch) || ch->fighting )
	return FALSE;

    for ( victim = ch->in_room->first_person; victim; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( !can_see( ch, victim ) )
	   continue;
        if ( get_timer(victim, TIMER_RECENTFIGHT) > 0 )
	   continue;
        if ( !IS_NPC( victim ) && victim->pcdata && IS_AWAKE(victim)
               && str_cmp( ch->leader->name , victim->name ) )
          {
	      do_yell( ch, "Hey you're not allowed in here!" );
              multi_hit( ch, victim, TYPE_UNDEFINED );
              return TRUE;         
          }
    }

    return FALSE;
}

bool spec_clan_patrol( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;

	if ( IS_NPC(ch) )
		return FALSE; /* If its a char BAAAAD */

    if ( !IS_AWAKE(ch) || ch->fighting )
	return FALSE;

    for ( victim = ch->in_room->first_person; victim; victim = v_next )
    {
		v_next = victim->next_in_room;
		if ( !can_see( ch, victim ) )
			continue;
        if ( get_timer(victim, TIMER_RECENTFIGHT) > 0 )
			continue;

		if ( !IS_NPC( victim ) )
		{
			if ( victim->pcdata->clan )
			{
				if ( ch->mob_clan == victim->pcdata->clan )
					return FALSE;
				if ( !IS_AWAKE(victim) 
					|| !nifty_is_name(victim->pcdata->clan->name , ch->mob_clan->atwar ) )
					return FALSE;
			}
			else
				return FALSE;
		}
		else if ( IS_NPC(victim) )
		{
			if ( victim->mob_clan )
			{
				if ( ch->mob_clan == victim->mob_clan )
					return FALSE;
				if ( !IS_AWAKE(victim) 
					|| !nifty_is_name(victim->mob_clan->name , ch->mob_clan->atwar ) )
					return FALSE;
			}
			else
				return FALSE;
		}
		
		do_yell( ch, "Hey you're not allowed around here!" );
		multi_hit( ch, victim, TYPE_UNDEFINED );
		return TRUE;         
	}
    return FALSE;
}

bool spec_player_patrol( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;

    if ( !IS_AWAKE(ch) || ch->fighting )
	return FALSE;

    for ( victim = ch->in_room->first_person; victim; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( !can_see( ch, victim ) )
	   continue;
        if ( get_timer(victim, TIMER_RECENTFIGHT) > 0 )
	   continue;
        if ( !IS_NPC( victim ) && victim->pcdata && IS_AWAKE(victim)
               && ( victim->top_level > 29 ) && str_cmp( ch->leader->name , victim->name ) )
          {
	      do_yell( ch, "Hey you're not allowed around here!" );
              multi_hit( ch, victim, TYPE_UNDEFINED );
              return TRUE;         
          }
    }

    return FALSE;
}


bool spec_droid_lang( CHAR_DATA *ch) /*You can add here special characteristics*/
{
      if (!IS_NPC(ch))
	return FALSE;
      else
	return TRUE;
}

bool spec_tax( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
	PLANET_DATA *planet;

    int tax;
    char buf[MAX_STRING_LENGTH];

    if ( !IS_AWAKE(ch) || ch->fighting )
	return FALSE;

    for ( victim = ch->in_room->first_person; victim; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( IS_NPC(victim) )
	   continue;
        if ( !can_see( ch, victim ) )
	   continue;
        if ( number_bits ( 1 ) == 0 ) 
	   continue;
		if ( victim->top_level > 100 )
	   continue;
	    if ( victim->gold < 1 )
	   continue;
	  do_say( ch , "Due to the inflation of credits we must collect 8% of your current credits." );
      tax = (victim->gold*0.08);
      victim->gold -= tax;
      ch_printf( victim, "You pay %d credits to the planet administration.\n\r", tax );
	  if (ch->in_room && ch->in_room->area && ch->in_room->area->planet )
	  {
		  planet = ch->in_room->area->planet;
		  if (planet->governed_by)
		  {
			  planet->governed_by->funds += tax;
			  sprintf( buf , "I am sure that %s is greatful for your contribution.\n\r",
				  planet->governed_by->name );
			  do_say( ch , buf );
		  }
	  }
    }

    return FALSE;

}
bool spec_clan_order( CHAR_DATA *ch) /*You can add here special characteristics*/
{
      if (!IS_NPC(ch))
	return FALSE;
      else
	return TRUE;
}


