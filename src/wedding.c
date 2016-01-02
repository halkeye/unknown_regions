/***************************************************************************
*	MARRY.C written by Ryouga for Vilaross Mud (baby.indstate.edu 4000)	   *
*	Please leave this and all other credit include in this package.        *
*	Email questions/comments to ryouga@jessi.indstate.edu		           *
****************************************************************************
*	Modified By Gavin(ur_gavin@hotmail.com) for Unkown Regions             *
***************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"


void do_propose( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

	if ( ch->pcdata->marriage && ch->pcdata->marriage[0] != '\0' )
	{
		if (IS_SET(ch->act2, EXTRA_MARRIED))
			send_to_char("But you are already married!\n\r",ch);
		else
			send_to_char("But you are already engaged!\n\r",ch);
		return;
    }

    if ( arg[0] == '\0' )
    {
		send_to_char("Who do you wish to propose marriage to?\n\r",ch);
		return;
    }

	if ( ( victim = get_char_room(ch, arg) ) == NULL )
	{
		send_to_char("They are not here.\n\r",ch);
		return;
    }

	if ( IS_NPC(victim) )
	{
		send_to_char("Not on NPC's.\n\r",ch);
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

	if ( victim->pcdata->marriage && victim->pcdata->marriage[0] != '\0' )
    {
		if (IS_SET(victim->act2, EXTRA_MARRIED))
			send_to_char("But they are already married!\n\r",ch);
		else
			send_to_char("But they are already engaged!\n\r",ch);
		return;
	}

/*	if ( (ch->sex == SEX_MALE && victim->sex == SEX_FEMALE) ||
		(ch->sex == SEX_FEMALE && victim->sex == SEX_MALE) )*/
	if ( ch->sex == victim->sex )
	{
		send_to_char("Not on this mud bub!\n\r",ch);
		return;
	}
	else
	{
		ch->pcdata->propose = victim;
		act(AT_WHITE, "You propose marriage to $M.",ch,NULL,victim,TO_CHAR);
		act(AT_WHITE, "$n gets down on one knee and proposes to $N.",ch,NULL,victim,TO_NOTVICT);
		act(AT_WHITE, "$n asks you quietly 'Will you marry me?'",ch,NULL,victim,TO_VICT);
		return;
	}
	send_to_char("I don't think that would be a very good idea...\n\r",ch);
	return;
}

void do_accept( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

	if ( ch->pcdata->marriage && ch->pcdata->marriage[0] != '\0' )
	{
		if (IS_SET(ch->act2, EXTRA_MARRIED))
			send_to_char("But you are already married!\n\r",ch);
		else
			send_to_char("But you are already engaged!\n\r",ch);
		return;
	}

    if ( arg[0] == '\0' )
    {
		send_to_char("Who's proposal of marriage do you wish to accept?\n\r",ch);
		return;
    }

    if ( ( victim = get_char_room(ch, arg) ) == NULL )
    {
		send_to_char("They are not here.\n\r",ch);
		return;
    }

    if ( IS_NPC(victim) )
    {
		send_to_char("Not on NPC's.\n\r",ch);
		return;
    }

	if ( victim->pcdata->marriage && victim->pcdata->marriage[0] != '\0' )
    {
		if (IS_SET(victim->act2, EXTRA_MARRIED))
			send_to_char("But they are already married!\n\r",ch);
		else
			send_to_char("But they are already engaged!\n\r",ch);
		return;
    }

    if ( victim->pcdata->propose != ch )
    {
		send_to_char("But they haven't proposed to you!\n\r",ch);
		return;
    }

	if ( ch->sex == victim->sex )
	{
		send_to_char("Not on this mud bub!\n\r",ch);
		return;
	}
	else
    {
		victim->pcdata->propose = NULL;
		ch->pcdata->propose = NULL;
		victim->pcdata->marriage = str_alloc( ch->name );
		ch->pcdata->marriage = str_alloc( victim->name );
		act(AT_WHITE,"You accept $S offer of marriage.",ch,NULL,victim,TO_CHAR);
		act(AT_WHITE,"$n accepts $N's offer of marriage.",ch,NULL,victim,TO_NOTVICT);
		act(AT_WHITE, "$n accepts your offer of marriage.",ch,NULL,victim,TO_VICT);
		save_char_obj(victim);
		save_char_obj(ch);
		sprintf(buf,"%s and %s are now engaged!\n\r",ch->name,victim->name);
		send_to_char(buf, ch);
		send_to_char(buf, victim);
/*		echo_to_all( AT_WHITE, buf, ECHOTAR_ALL );*/
		return;
	}
	/*send_to_char("I don't think that would be a very good idea...\n\r",ch);*/
    return;
}


void do_breakup( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim1;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
	send_to_char("Syntax: breakup <person>\n\r",ch);
	return;
    }
    if ( ( victim1 = get_char_room(ch, arg1) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }
    if (IS_NPC(victim1))
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }
    if (!str_cmp(ch->name, victim1->pcdata->marriage) &&
	!str_cmp(victim1->name, ch->pcdata->marriage))
    {
		if (IS_SET(victim1->act2,EXTRA_MARRIED) || IS_SET(ch->act2,EXTRA_MARRIED))
		{
		    send_to_char("But you are married!\n\r",ch);
			return;
		}
		REMOVE_BIT(victim1->act2, EXTRA_MARRIED);
		REMOVE_BIT(ch->act2, EXTRA_MARRIED);
		str_free( victim1->pcdata->marriage );
		victim1->pcdata->marriage = str_alloc( "" );
		str_free( ch->pcdata->marriage );
		ch->pcdata->marriage = str_alloc( "" );
		save_char_obj(victim1);
		save_char_obj(ch);

		sprintf(buf,"%s breaks of the engagement with %s are now!",ch->name,victim1->name);
		echo_to_all( AT_WHITE, buf, ECHOTAR_ALL );
		return;
	}
    send_to_char("But your not engaged to him!\n\r",ch);
    return;
}


void do_marry( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim1;
    CHAR_DATA *victim2;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char("Syntax: marry <person> <person>\n\r",ch);
	return;
    }
    if ( ( victim1 = get_char_room(ch, arg1) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }
    if ( ( victim2 = get_char_room(ch, arg2) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }
    if (IS_NPC(victim1) || IS_NPC(victim2))
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }
	if (!str_cmp(victim1->name, victim2->pcdata->marriage) &&
	!str_cmp(victim2->name, victim1->pcdata->marriage))
    {
	SET_BIT(victim1->act2, EXTRA_MARRIED);
	SET_BIT(victim2->act2, EXTRA_MARRIED);
	str_free( victim1->pcdata->marriage );
	str_free( victim2->pcdata->marriage );
	victim1->pcdata->marriage = str_alloc(victim2->name);
	victim2->pcdata->marriage = str_alloc(victim1->name);
	save_char_obj(victim1);
	save_char_obj(victim2);
	sprintf(buf,"%s and %s are now married!\n\r",victim1->name,victim2->name);
	send_to_char(buf,ch);
	send_to_char(buf,victim1);
	send_to_char(buf,victim2);
	return;
    }
    send_to_char("But they are not yet engaged!\n\r",ch);
    return;
}

void do_divorce( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim1;
    CHAR_DATA *victim2;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char("Syntax: divorse <person> <person>\n\r",ch);
	return;
    }
    if ( ( victim1 = get_char_room(ch, arg1) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }
    if ( ( victim2 = get_char_room(ch, arg2) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }
    if (IS_NPC(victim1) || IS_NPC(victim2))
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }
    if (!str_cmp(victim1->name, victim2->pcdata->marriage) &&
	!str_cmp(victim2->name, victim1->pcdata->marriage))
    {
	if (!IS_SET(victim1->act2,EXTRA_MARRIED) || !IS_SET(victim2->act2,EXTRA_MARRIED))
	{
	    send_to_char("But they are not married!\n\r",ch);
	    return;
	}
	REMOVE_BIT(victim1->act2, EXTRA_MARRIED);
	REMOVE_BIT(victim2->act2, EXTRA_MARRIED);
	STRFREE(victim1->pcdata->marriage);
	victim1->pcdata->marriage = STRALLOC( "" );
	STRFREE(victim2->pcdata->marriage);
	victim2->pcdata->marriage = STRALLOC( "" );
	save_char_obj(victim1);
	save_char_obj(victim2);
	sprintf(buf,"%s and %s are now divorced!\n\r",victim1->name,victim2->name);
	send_to_char(buf,ch);
	send_to_char(buf,victim1);
	send_to_char(buf,victim2);	
/*	do_info(ch,buf);*/
	return;
    }
    send_to_char("But they are not married!\n\r",ch);
    return;
}
