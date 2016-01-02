#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mud.h"

int check_preg ( CHAR_DATA *ch );

void do_consent( CHAR_DATA *ch, char *argument )
{
	char arg [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg2 );

	if (IS_NPC(ch)) return;

	if ( arg[0] == '\0' )
	{
		ch_printf(ch, "&G&WSyntax:\n\r"
			"(to list        ) consent list\n\r"
			"(for non-xsocial) consent player-name\n\r"
			"(for xsocials   ) consent xsocial player-name\n\r");
		return;
	}
	if ( !str_cmp("list", arg ) )
	{
		ch_printf(ch, "&WYou have the following people currently consented: \n\r&B%s\n\r",
			ch->pcdata->partner);
		ch_printf(ch, "&WYou have the following people currently consented for xsocials: \n\r"
			"&B%s\n\r", ch->pcdata->x_partner );
		return;
	}

	if ( !str_cmp("none",arg) ) 
	{
		str_free( ch->pcdata->partner );
		ch->pcdata->partner = str_alloc( "" );
		ch_printf( ch,"&WCleared\n\r");
		return;
	}

	if ( !str_cmp("xsocial",arg) ) 
	{
		if ( !str_cmp("none",arg2) )
		{
			STRFREE( ch->pcdata->x_partner );
			ch->pcdata->x_partner = STRALLOC( "" );
			ch_printf( ch,"&WCleared\n\r");
			return;
		}
		if ( ( victim = get_char_room(ch, arg2) ) == NULL )
		{
			send_to_char("They are not here.\n\r",ch);
			return;
		}

		if (IS_NPC(victim))
		{
			send_to_char("Not on NPC's.\n\r",ch);
			return;
		}

		if (ch == victim)
		{
			send_to_char("Not on yourself!\n\r",ch);
			return;
		}
		if ( get_age(ch) < race_table[ch->race]->consent )
		{
			send_to_char("You must mature a little more before you start thinking about that.\n\r",ch);
			return;
		}
		if ( get_age(victim) < race_table[victim->race]->consent )
		{
			send_to_char("They must mature a little more before they should start thinking about that.\n\r",ch);
			return;
		}

		if ( !ch->pcdata->x_partner || ch->pcdata->x_partner[0] == '\0' )
			sprintf( buf, "%s", victim->name );
		else
		{
			sprintf( buf, "%s %s", ch->pcdata->x_partner, victim->name );
			STRFREE( ch->pcdata->x_partner );
		}
		ch->pcdata->x_partner = str_alloc( buf );

		ch_printf(ch, "You have given %s permission to use xsocials.\n\r",victim->name);
		ch_printf(victim, "%s has given you permission to use xsocials.\n\r",ch->name);
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

    if (ch == victim)
    {
	send_to_char("Not on yourself!\n\r",ch);
	return;
    }

	if ( !ch->pcdata->partner || ch->pcdata->partner[0] == '\0' )
		sprintf( buf, "%s", victim->name );
	else
	{
		sprintf( buf, "%s %s", ch->pcdata->partner, victim->name );
		str_free( ch->pcdata->partner );
	}
	ch->pcdata->partner = str_alloc( buf );

	ch_printf(ch, "You have consented %s now\n\r",victim->name);
	ch_printf(victim, "%s has consented you now\n\r",ch->name);
    return;
}
/*
 * Checks how old a char is
 * Done by Gavin
 * With help from Darrik Vequir of SW:RiP
 */
void birth_date( CHAR_DATA *ch )
{
	int years;
	int months;
	int days;

    if (IS_NPC(ch)) return;
	if (!IS_NPC(ch) && IS_SET(ch->act2, EXTRA_PREGNANT) ) 
	{
		int preg;
		preg = check_preg(ch);
	}

	if ( !ch->pcdata->birthday.year && !ch->pcdata->birthday.day && !ch->pcdata->birthday.month )
	{
		bug("%s doesn't have a Birthday",ch->name);
		return;
	}
	years = ( (time_info.year - ch->pcdata->birthday.year) + ch->pcdata->age );
	months = 12 + ( time_info.month - ch->pcdata->birthday.month );
	months %= 12;
	days = 30 + ( time_info.day - ch->pcdata->birthday.day);
	days %= 30;

	if ( time_info.day < ch->pcdata->birthday.day )
		months--;
	if ( months < 0 )
		months += 12;
	if ( time_info.month < ch->pcdata->birthday.month )
		years--;

	ch_printf(ch,"You are %d years, %d %s and %d %s old.\n\r",
		years,months,(months == 1) ? "month" : "months",days,
		(days == 1) ? "days" : "days");

	return; /*final return*/
}

int check_preg( CHAR_DATA *ch ) /* returns days */
{
	int years = 0;
	int months = 0;
	int days = 0;
	int end = 0;

    if (IS_NPC(ch)) return -1;
	if (!IS_NPC(ch) && !IS_SET(ch->act2, EXTRA_PREGNANT) )
	{
		bug("check_preg: %s is not pregnant", ch->name, 0);
		return -1;
	}

	if ( !ch->pcdata->pregnacy.day &&
		!ch->pcdata->pregnacy.month && 
		!ch->pcdata->pregnacy.year )
	{
		bug("%s doesn't have pregnacy info",ch->name);
		return -1;
	}

	years = ( time_info.year - ch->pcdata->pregnacy.year);
	months = 12 + ( time_info.month - ch->pcdata->pregnacy.month );
	months %= 12;
	days = 30 + ( time_info.day - ch->pcdata->pregnacy.day);
	days %= 30;

	if ( time_info.day < ch->pcdata->pregnacy.day )
		months--;
	if ( months < 0 )
		months += 12;
	if ( time_info.month < ch->pcdata->pregnacy.month )
		years--;

	end += (365 * years);
	end += (30 * months);
	end += days;
	if ( end >= number_range(240,290) /*270*/ && !IS_SET(ch->act2, EXTRA_LABOUR) )
	{
		send_to_char("You just feel your water break.\n\rIts Time!\n\r",ch);
		SET_BIT(ch->act2, EXTRA_LABOUR);
	}
	return end; /*final return*/
}

void do_xsocials( CHAR_DATA *ch, char *argument )
{
    int iHash;
    int col = 0;
    XSOCIALTYPE *xsocial;
	char buf [MAX_INPUT_LENGTH];

    set_pager_color( AT_PLAIN, ch );
    for ( iHash = 0; iHash < 27; iHash++ )
		for ( xsocial = xsocial_index[iHash]; xsocial; xsocial = xsocial->next )
		{
			sprintf( buf, "%-12s", xsocial->name );
			send_to_char( buf, ch );
			if ( ++col % 6 == 0 )
				send_to_char( "\n\r", ch );;
		}

	if ( col % 6 != 0 )
		send_to_pager( "\n\r", ch );
    return;
}
