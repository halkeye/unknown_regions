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
*                           Player Vendor module                           *
****************************************************************************/


#include <sys/types.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "mud.h"

CHAR_DATA *	find_keeper	args( ( CHAR_DATA *ch ) );

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !strcmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

/*
This is the command used to buy a contract from a vendor to place a player
owned vendor
*/
void do_buyvendor (CHAR_DATA *ch, char *argument)
{
	OBJ_DATA *deed;
	char buf1[MAX_STRING_LENGTH];
	
	if (IS_NPC(ch))
		return;

	if ( ch->pcdata->vendor != NULL )
	{
		ch_printf( ch, "You already own a vendor!\n\r");
		return;
	}
	if ( (ch->gold < COST_BUY_VENDOR ) )
	{
		sprintf(buf1, "You are to poor!\n\r");
		send_to_char (buf1, ch);
		return;
	}

	if ( (get_age(ch)) < AGE_BUY_VENDOR )
	{
		sprintf (buf1, "you must be at least %d years old to buy a vendor.\n\r", AGE_BUY_VENDOR);
		send_to_char (buf1, ch);
		return;
	}

	if ( (get_obj_index (OBJ_VNUM_DEED) ) == NULL )
	{
		bug ("BUYVENDOR: Deed is missing!");
		return;
	}
	deed = create_object ( get_obj_index(OBJ_VNUM_DEED), 0);
	obj_to_char (deed, ch);
	send_to_char_color ("&bVery well, you may have a contract for a vendor.\n\r", ch);
	ch->gold = ch->gold - COST_BUY_VENDOR;
}


/*this is the command that places the vendor at the specified location*/
void do_placevendor (CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *vendor;
	MOB_INDEX_DATA *temp;
	OBJ_DATA *obj;
	OBJ_DATA *obj2;
	char buf [MAX_INPUT_LENGTH];
	bool checkdeed = FALSE;
	ROOM_INDEX_DATA *room;
    AREA_DATA *pArea;
     
	if ( !ch->in_room )
		return;

	if ( IS_NPC(ch) )
		return;

	if ( (get_age(ch)) < AGE_BUY_VENDOR )
	{
		ch_printf(ch, "You are not old enough to start a shop, you must be at least %d years old.\n\r", AGE_BUY_VENDOR);
		return;
	}
	
	if ( ch->pcdata->vendor != NULL )
	{
		ch_printf( ch, "You already own a vendor!\n\r");
		return;
	}

	for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
	{
		if (obj->pIndexData->vnum == OBJ_VNUM_DEED)
		{
			checkdeed = TRUE;	
		}
	}

	if (!checkdeed)
	{
		send_to_char( "You do not have a deed!.\n\r", ch );
	    return;
	}

    if ( argument[0] == '\0' )
    {
		send_to_char( "Set the room name.  A very brief single line room description.\n\r", ch );
		send_to_char( "Usage: Placevendor <Room Name>\n\r", ch );
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


	if ( (temp = get_mob_index (MOB_VNUM_VENDOR) ) == NULL )
	{
		log_string ("do_placevendor: no vendor vnum");
		return;
	}

	if ( !xIS_SET( ch->in_room->room_flags, ROOM_EMPTYSHOP ) )
	{
		send_to_char( "Not here\n\r", ch );
		return;
	}

    STRFREE( room->name );
    room->name = STRALLOC( argument );

	xREMOVE_BIT(ch->in_room->room_flags, ROOM_EMPTYSHOP );

	char_to_room( create_mobile( temp ), ch->in_room );
	vendor = get_char_room(ch, temp->player_name);

	sprintf (buf, vendor->long_descr, ch->name);
	vendor->long_descr =  STRALLOC( buf );

	sprintf (buf, "%s", ch->name);

	vendor->owner = STRALLOC(buf);
	vendor->home = ch->in_room;

    fold_area( room->area, room->area->filename, FALSE );

	save_vendor (vendor);
	
	for ( obj2 = ch->last_carrying; obj2; obj2 = obj2->prev_content )
	{
		if (obj2->pIndexData->vnum == OBJ_VNUM_DEED)
		{
			separate_obj( obj2 );
			obj_from_char( obj2 );
			extract_obj( obj2 );
			break;	
		}
	}

	act( AT_ACTION, "$n appears in a swirl of smoke.\n", vendor, NULL, NULL, TO_ROOM );
	ch->pcdata->vendor = vendor->pIndexData;
}

void do_pricevendor (CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *vendor;
	CHAR_DATA *ch1;
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	char buf [MAX_INPUT_LENGTH];

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);

	if ( arg1[0] == '\0' )
	{
	    send_to_char("&BS&Wyntx&B: &Wpricevendor &B<&Witem&B> &B<&Wcost&B>\n\r",ch);
	    return;
	}

	if (arg2[0] == '\0')
	{
		send_to_char("&BS&Wyntx&B: &Wpricevendor &B<&Witem&B> &B<&Wcost&B>\n\r",ch);
		return;
	}


	if ( ( vendor = find_keeper (ch) ) == NULL )
	{
		send_to_char ("What Vendor?\n\r",ch);
		return;
	}

	if ( vendor->owner == NULL )
	{
		sprintf(buf, "Sorry, %s, but I'm not one of those guys", ch->name);
		do_say(vendor,buf);
		interpret(vendor,"shudder");
		return;
	}

	if ( vendor->owner == NULL )
	{
		sprintf(buf, "Sorry, %s, but I'm not one of those guys", ch->name);
		do_say(vendor,buf);
		interpret(vendor,"shudder");
		return;
	}
	ch1 = get_char_world(ch, vendor->owner );
	if ( ch1 != ch )
	{
		sprintf(buf, "%s is a theif!!!, %s is trying to scam me!", 
			ch->name,he_she[ch->sex]);
		do_say(vendor,"STOP THIEF!");
		return;
	}

	if ( !ch->pcdata->vendor || ch->pcdata->vendor != NULL )
		ch->pcdata->vendor = vendor->pIndexData;
	if ( ch->fighting)
	{
		send_to_char ("Not while you fighting!\n\r",ch);
		return;
	}
	
	if ( (obj  = get_obj_carry( vendor, arg1 )) != NULL)
	{
		obj->cost = atoi (arg2);
		do_say(vendor,"I changed the price on that item for you");
		save_vendor(vendor);
		return;
	}

	do_say(vendor,"I do have that item!");
	save_vendor(vendor);
	return;

}


void do_collectgold (CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *vendor;
	CHAR_DATA *ch1;
	long gold;
	char buf [MAX_INPUT_LENGTH];
	char name[MAX_INPUT_LENGTH];

	if ( ( vendor = find_keeper (ch) ) == NULL )
	{
		send_to_char ("What Vendor?\n\r",ch);
		return;
	}

	if (vendor->owner == NULL)
	{
		send_to_char("Thats not a vendor!\n\r",ch);
		return;
    }
	sprintf(name, "%s", vendor->owner);

	if ( vendor->owner == NULL )
	{
		sprintf(buf, "Sorry, %s, but I'm not one of those guys", ch->name);
		do_say(vendor,buf);
		interpret(vendor,"shudder");
		return;
	}
	ch1 = get_char_world(ch, vendor->owner );
	if ( ch1 != ch )
	{
		sprintf(buf, "%s is a theif!!!, %s is trying to make me give %s money!", 
			ch->name,he_she[ch->sex], him_her[ch->sex]);
		do_yell(vendor, buf);
		do_say(vendor,"STOP THIEF!");
		return;
	}


	if ( ch->fighting)
	{
		send_to_char ("Not while you fightZ!\n\r",ch);
		return;
	}
	if ( !ch->pcdata->vendor || ch->pcdata->vendor != NULL )
		ch->pcdata->vendor = vendor->pIndexData;

	gold = vendor->gold;
	gold -= (gold * VENDOR_FEE);
	vendor->gold = 0;
	ch->gold += gold;

	send_to_char_color ("&GYour vendor gladly hands over his earnings minus a small fee of course..\n\r",ch);
	act( AT_ACTION, "$n hands over some money.\n\r", vendor, NULL, NULL, TO_ROOM );
}


/* Write vendor to file */
void fwrite_vendor( FILE *fp, CHAR_DATA *mob )
{
	if ( !IS_NPC( mob ) || !fp )
		return;
	fprintf( fp, "Vnum     %d\n", mob->pIndexData->vnum );
	if (mob->gold > 0)
		fprintf (fp, "Gold     %d\n",mob->gold);
	if ( mob->home )
		fprintf( fp, "Home     %d\n", mob->home->vnum );
	if (mob->owner != NULL )
		fprintf (fp, "Owner     %s~\n", mob->owner );
	if ( QUICKMATCH( mob->short_descr, mob->pIndexData->short_descr) == 0 )
		fprintf( fp, "Short	    %s~\n", mob->short_descr );
	fprintf( fp, "Position   %d\n", mob->position );
	fprintf( fp, "Flags   %d\n",   mob->act );
	fprintf( fp, "END\n" );
	return;
}


/* read vendor from file */
CHAR_DATA *  fread_vendor( FILE *fp )
{
	CHAR_DATA *mob = NULL;

	char *word;
	bool fMatch;
	int inroom = 0;
	ROOM_INDEX_DATA *pRoomIndex = NULL;
	CHAR_DATA *victim;
	CHAR_DATA *vnext;
	char buf [MAX_INPUT_LENGTH];
	char vnum1 [MAX_INPUT_LENGTH];
	word   = feof( fp ) ? "END" : fread_word( fp );
	if ( !strcmp(word, "Vnum") )
	{
		int vnum;

		vnum = fread_number( fp );
		mob = create_mobile(  get_mob_index(vnum));
		if ( !mob )
		{
			for ( ; ; )
			{
				word   = feof( fp ) ? "END" : fread_word( fp );
				if ( !strcmp( word, "END" ) )
					break;
			}
			bug("Fread_mobile: No index data for vnum %d", vnum );
			return NULL;
		}
	}
	else
	{
		for ( ; ; )
		{
			word   = feof( fp ) ? "END" : fread_word( fp );
			if ( !strcmp( word, "END" ) )
				break;
		}
		extract_char(mob, TRUE);
		bug("Fread_vendor: Vnum not found", 0 );
		return NULL;
	}
	for ( ; ;) {
		word   = feof( fp ) ? "END" : fread_word( fp );
		fMatch = FALSE;
		switch ( UPPER(word[0]) ) {
		case '*':
			fMatch = TRUE;
			fread_to_eol( fp );
			break;
		case '#':
			if ( !strcmp( word, "#OBJECT" ) )
			{
				fread_obj ( mob, fp, OS_CARRY );
			}
			break;
		case 'D':
			KEY( "Description", mob->description, fread_string(fp));
			break;
		case 'E':

			if ( !strcmp( word, "END" ) )
			{
				if ( inroom == 0 )
					inroom = ROOM_VNUM_VENSTOR;
				mob->home = get_room_index(inroom);
				pRoomIndex = get_room_index( inroom );
				if ( !pRoomIndex )
				{
					pRoomIndex = get_room_index( ROOM_VNUM_VENSTOR );
					mob->home = get_room_index( ROOM_VNUM_VENSTOR );
				}

				mob->in_room = pRoomIndex;
				/* the following code is to make sure no more then one player owned vendor is in the room - meckteck */
				for ( victim = mob->in_room->first_person; victim; victim = vnext )
				{
					vnext = victim->next_in_room;
					if (victim->home != NULL)
					{
						extract_char( victim, TRUE);
						break;
					}

				}

				char_to_room(mob, pRoomIndex);
				sprintf(vnum1,"%d", mob->pIndexData->vnum);
				do_makeshop (mob, vnum1 );
				sprintf (buf, mob->long_descr, mob->owner);
				mob->long_descr =  STRALLOC( buf );
				mob->hit = 10000;
				mob->max_hit = 10000;
				return mob;
			}
			break;
		case 'F':
			KEY( "Flags", mob->act, fread_number(fp));
			break;
		case 'G':
			KEY("Gold", mob->gold, fread_number(fp));
			break;
		case 'H':
			KEY("Home", inroom, fread_number(fp) );
			break;
		case 'L':
			break;
		case 'N':
			break;
		case 'O':
			KEY ("Owner", mob->owner, fread_string (fp) );
			break;
		case 'P':
			KEY( "Position", mob->position, fread_number( fp ) );
			break;
		case 'S':
			KEY( "Short", mob->short_descr, fread_string(fp));
			break;
		}
		if ( !fMatch )
		{
			bug ( "Fread_mobile: no match.", 0 );
			bug ( word, 0 );
		}
	}
	return NULL;
}

void save_vendor( CHAR_DATA *ch )
{
    char strsave[MAX_INPUT_LENGTH];
    FILE *fp;

    if ( !ch )
    {
		bug( "Save_char_obj: null ch!", 0 );
		return;
    }

	de_equip_char( ch );
    sprintf( strsave, "%s%s",VENDOR_DIR, capitalize( ch->owner ) );

    if ( ( fp = fopen( TEMP_FILE, "w" ) ) == NULL )
    {
		perror( strsave );
		bug( "Save_vendor: fopen", 0 );
    }
    else
    {
		bool ferr;

		fchmod(fileno(fp), S_IRUSR|S_IWUSR | S_IRGRP|S_IWGRP | S_IROTH|S_IWOTH);
		fprintf( fp, "#VENDOR\n"		);
		fwrite_vendor( fp, ch );

		if ( ch->first_carrying )
			fwrite_obj( ch, ch->last_carrying, fp, 0, OS_CARRY );

		fprintf(fp, "#END\n" );
		ferr = ferror(fp);
		fclose( fp );
		if (ferr)
		{
			perror(strsave);
			bug("Error writing temp file for %s -- not copying", strsave);
		}
		else
			rename(TEMP_FILE, strsave);
	}

	re_equip_char( ch );
	return;
}

void remove_vendor( CHAR_DATA *ch )
{
    char strsave[MAX_INPUT_LENGTH];

    if ( !ch )
    {
		bug( "remove_vendor: null ch!", 0 );
		return;
    }
	if ( !ch->pcdata->vendor || ch->pcdata->vendor == NULL ) return;

    sprintf( strsave, "%s%s",VENDOR_DIR, capitalize( ch->owner ) );
	remove(strsave);

	return;
}
