/****************************************************************************
 * [S]imulated [M]edieval [A]dventure multi[U]ser [G]ame      |   \\._.//   *
 * -----------------------------------------------------------|   (0...0)   *
 * SMAUG 1.4 (C) 1994, 1995, 1996, 1998  by Derek Snider      |    ).:.(    *
 * -----------------------------------------------------------|    {o o}    *
 * SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,      |   / ' ' \   *
 * Scryn, Rennard, Swordbearer, Gorog, Grishnakh, Nivek,      |~'~.VxvxV.~'~*
 * Tricops and Fireblade                                      |             *
 * ------------------------------------------------------------------------ *
 * 			Gorog's Revenge on Unruly Bastards		    *
 ****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

	
/*
 * Find the position of a target substring in a source string.
 * Returns pointer to the first occurrence of the string pointed to 
 * bstr in the string pointed to by astr. It returns a null pointer
 * if no match is found.  --  Gorog (with help from Thoric)
 *
 * Note I made a change when modifying str_infix. If the target string is
 * null, I return NULL (meaning no match was found). str_infix returns
 * FALSE (meaning a match was found).  *grumble*
 */
char *str_str( char *astr, char *bstr )
{
    int sstr1, sstr2, ichar;
    char c0;

    if ( ( c0 = LOWER(bstr[0]) ) == '\0' )
        return NULL;

    sstr1 = strlen(astr);
    sstr2 = strlen(bstr);

    for ( ichar = 0; ichar <= sstr1 - sstr2; ichar++ )
        if ( c0 == LOWER(astr[ichar]) && !str_prefix(bstr, astr+ichar) )
            return (astr+ichar);
    return NULL;
}

/*
 * Counts the number of times a target string occurs in a source string.
 * case insensitive -- Gorog
 */
int str_count(char *psource, char *ptarget)
{
   char *ptemp=psource;
   int count=0;
   
   while ( (ptemp=str_str(ptemp, ptarget)) )
   {
      ptemp++;
      count++;
   }
   return count;
}

/*
 * Displays the help screen for the "opfind" command
 */
void opfind_help (CHAR_DATA *ch)
{
   send_to_char( "Syntax:\n\r", ch);
   send_to_char( "opfind n lo_vnum hi_vnum text \n\r"
      "   Search obj vnums between lo_vnum and hi_vnum \n\r"
      "   for obj progs that contain an occurrence of text. \n\r"
      "   Display a maxiumu of n lines.\n\r\n\r", ch );
   send_to_char( "opfind n mud text \n\r"
      "   Search all the objs in the mud for\n\r"
      "   obj progs that contain an occurrence of text. \n\r"
      "   Display a maxiumu of n lines.\n\r\n\r", ch );
   
   send_to_char( "Example:\n\r", ch);
   send_to_char( "opfind 20 901 969 if isnpc \n\r"
      "   Search all obj progs in Olympus (vnums 901 thru 969)\n\r"
      "   and display all objects that contain the text \"if isnpc\".\n\r"
      "   Display a maximum of 20 lines.\n\r\n\r", ch );
   send_to_char( "Example:\n\r", ch);
   send_to_char( "opfind 100 mud mpslay \n\r"
      "   Search all obj progs in the entire mud\n\r"
      "   and display all objects that contain the text \"mpslay\".\n\r"
      "   Display a maximum of 100 lines.\n\r\n\r", ch );
}

/*
 * Search objects for obj progs containing a specified text string.
 */
void do_opfind( CHAR_DATA *ch, char *argument )   /* Gorog */
{
   OBJ_INDEX_DATA  *   pObj;
   MPROG_DATA      *   pProg;
   char                arg1 [MAX_INPUT_LENGTH];
   char                arg2 [MAX_INPUT_LENGTH];
   char                arg3 [MAX_INPUT_LENGTH];
   int                 lo_vnum=1, hi_vnum=MAX_VNUMS;
   int                 tot_vnum, tot_hits=0;
   int                 i, disp_cou=0, disp_limit;
   
   argument = one_argument( argument, arg1 );   /* display_limit */
   argument = one_argument( argument, arg2 );

   if ( arg1[0]=='\0' || arg2[0]=='\0' || !is_number(arg1) )
   {
      opfind_help(ch);
      return;
   }

   disp_limit = atoi (arg1);
   disp_limit = UMAX(0, disp_limit);

   if ( str_cmp(arg2, "mud") )
   {
      argument = one_argument( argument, arg3 );
      if ( arg3[0]=='\0' || argument[0]=='\0' 
      ||   !is_number(arg2) || !is_number(arg3) )
      {
         opfind_help(ch);
         return;
      }
      else
      {
		  lo_vnum = URANGE(1, atoi(arg2), MAX_VNUMS);
         hi_vnum = URANGE(1, atoi(arg3), MAX_VNUMS);
         if ( lo_vnum > hi_vnum )
         {
            opfind_help(ch);
            return;
         }
      }
   }   
   if ( argument[0] == '\0' )
   {
      opfind_help(ch);
      return;
   }

   for (i = lo_vnum; i <= hi_vnum; i++)
   {
      if ( (pObj=get_obj_index(i)) && (pProg=pObj->mudprogs) )
      {
         tot_vnum = 0;
         for ( ; pProg; pProg=pProg->next)
             tot_vnum += str_count(pProg->comlist, argument);
         tot_hits += tot_vnum;
         if ( tot_vnum && ++disp_cou <= disp_limit)
            pager_printf( ch, "%5d %5d %5d\n\r", disp_cou, i, tot_vnum);
      }
   }
   pager_printf( ch, "Total: %10d\n\r", tot_hits);
}
		
/*
 * Displays the help screen for the "mpfind" command
 */
void mpfind_help (CHAR_DATA *ch)
{
   send_to_char( "Syntax:\n\r", ch);
   send_to_char( "mpfind n lo_vnum hi_vnum text \n\r"
      "   Search mob vnums between lo_vnum and hi_vnum \n\r"
      "   for mob progs that contain an occurrence of text. \n\r"
      "   Display a maxiumu of n lines.\n\r\n\r", ch );
   send_to_char( "mpfind n mud text \n\r"
      "   Search all the mobs in the mud for\n\r"
      "   mob progs that contain an occurrence of text. \n\r"
      "   Display a maxiumu of n lines.\n\r\n\r", ch );
   
   send_to_char( "Example:\n\r", ch);
   send_to_char( "mpfind 20 901 969 if isnpc \n\r"
      "   Search all mob progs in Olympus (vnums 901 thru 969)\n\r"
      "   and display all mobs that contain the text \"if isnpc\".\n\r"
      "   Display a maximum of 20 lines.\n\r\n\r", ch );
   send_to_char( "Example:\n\r", ch);
   send_to_char( "mpfind 100 mud mpslay \n\r"
      "   Search all mob progs in the entire mud\n\r"
      "   and display all mobs that contain the text \"mpslay\".\n\r"
      "   Display a maximum of 100 lines.\n\r\n\r", ch );
}

/*
 * Search mobs for mob progs containing a specified text string.
 */
void do_mpfind( CHAR_DATA *ch, char *argument )   /* Gorog */
{
   MOB_INDEX_DATA  *   pMob;
   MPROG_DATA      *   pProg;
   char                arg1 [MAX_INPUT_LENGTH];
   char                arg2 [MAX_INPUT_LENGTH];
   char                arg3 [MAX_INPUT_LENGTH];
   int                 lo_vnum=1, hi_vnum=MAX_VNUMS;
   int                 tot_vnum, tot_hits=0;
   int                 i, disp_cou=0, disp_limit;
   
   argument = one_argument( argument, arg1 );   /* display_limit */
   argument = one_argument( argument, arg2 );

   if ( arg1[0]=='\0' || arg2[0]=='\0' || !is_number(arg1) )
   {
      mpfind_help(ch);
      return;
   }

   disp_limit = atoi (arg1);
   disp_limit = UMAX(0, disp_limit);

   if ( str_cmp(arg2, "mud") )
   {
      argument = one_argument( argument, arg3 );
      if ( arg3[0]=='\0' || !is_number(arg2) || !is_number(arg3) )
      {
         mpfind_help(ch);
         return;
      }
      else
      {
		 lo_vnum = URANGE(1, atoi(arg2), MAX_VNUMS);
         hi_vnum = URANGE(1, atoi(arg3), MAX_VNUMS);
         if ( lo_vnum > hi_vnum )
         {
            mpfind_help(ch);
            return;
         }
      }
   }   
   if ( argument[0] == '\0' )
   {
      mpfind_help(ch);
      return;
   }

   for (i = lo_vnum; i <= hi_vnum; i++)
   {
      if ( (pMob=get_mob_index(i)) && (pProg=pMob->mudprogs) )
      {
         tot_vnum = 0;
         for ( ; pProg; pProg=pProg->next)
             tot_vnum += str_count(pProg->comlist, argument);
         tot_hits += tot_vnum;
         if ( tot_vnum && ++disp_cou <= disp_limit)
            pager_printf( ch, "%5d %5d %5d\n\r", disp_cou, i, tot_vnum);
      }
   }
   pager_printf( ch, "Total: %10d\n\r", tot_hits);
}
		
/*
 * Displays the help screen for the "rpfind" command
 */
void rpfind_help (CHAR_DATA *ch)
{
   send_to_char( "Syntax:\n\r", ch);
   send_to_char( "rpfind n lo_vnum hi_vnum text \n\r"
      "   Search room vnums between lo_vnum and hi_vnum \n\r"
      "   for room progs that contain an occurrence of text. \n\r"
      "   Display a maxiumu of n lines.\n\r\n\r", ch );
   send_to_char( "rpfind n mud text \n\r"
      "   Search all the rooms in the mud for\n\r"
      "   room progs that contain an occurrence of text. \n\r"
      "   Display a maxiumu of n lines.\n\r\n\r", ch );
   
   send_to_char( "Example:\n\r", ch);
   send_to_char( "rpfind 20 901 969 if isnpc \n\r"
      "   Search all room progs in Olympus (vnums 901 thru 969)\n\r"
      "   and display all vnums that contain the text \"if isnpc\".\n\r"
      "   Display a maximum of 20 lines.\n\r\n\r", ch );
   send_to_char( "Example:\n\r", ch);
   send_to_char( "rpfind 100 mud mpslay \n\r"
      "   Search all room progs in the entire mud\n\r"
      "   and display all vnums that contain the text \"mpslay\".\n\r"
      "   Display a maximum of 100 lines.\n\r\n\r", ch );
}

/*
 * Search rooms for room progs containing a specified text string.
 */
void do_rpfind( CHAR_DATA *ch, char *argument )   /* Gorog */
{
   ROOM_INDEX_DATA *   pRoom;
   MPROG_DATA      *   pProg;
   char                arg1 [MAX_INPUT_LENGTH];
   char                arg2 [MAX_INPUT_LENGTH];
   char                arg3 [MAX_INPUT_LENGTH];
   int                 lo_vnum=1, hi_vnum=MAX_VNUMS;
   int                 tot_vnum, tot_hits=0;
   int                 i, disp_cou=0, disp_limit;
   
   argument = one_argument( argument, arg1 );   /* display_limit */
   argument = one_argument( argument, arg2 );

   if ( arg1[0]=='\0' || arg2[0]=='\0' || !is_number(arg1) )
   {
      rpfind_help(ch);
      return;
   }

   disp_limit = atoi (arg1);
   disp_limit = UMAX(0, disp_limit);

   if ( str_cmp(arg2, "mud") )
   {
      argument = one_argument( argument, arg3 );
      if ( arg3[0]=='\0' || argument[0]=='\0' 
      ||   !is_number(arg2) || !is_number(arg3) )
      {
         rpfind_help(ch);
         return;
      }
      else
      {
		 lo_vnum = URANGE(1, atoi(arg2), MAX_VNUMS);
         hi_vnum = URANGE(1, atoi(arg3), MAX_VNUMS);
         if ( lo_vnum > hi_vnum )
         {
            rpfind_help(ch);
            return;
         }
      }
   }   
   if ( argument[0] == '\0' )
   {
      rpfind_help(ch);
      return;
   }

   for (i = lo_vnum; i <= hi_vnum; i++)
   {
      if ( (pRoom=get_room_index(i)) && (pProg=pRoom->mudprogs) )
      {
         tot_vnum = 0;
         for ( ; pProg; pProg=pProg->next)
             tot_vnum += str_count(pProg->comlist, argument);
         tot_hits += tot_vnum;
         if ( tot_vnum && ++disp_cou <= disp_limit)
            pager_printf( ch, "%5d %5d %5d\n\r", disp_cou, i, tot_vnum);
      }
   }
   pager_printf( ch, "Total: %10d\n\r", tot_hits);
}

/*
 * The "showlayers" command is used to list all layerable eq in the
 * mud so that we can keep track of it. It lists one line for each
 * piece of unique eq. If there are 1,000 shrouds in the game, it
 * doesn't list 1,000 lines for each shroud - just one line for the shroud.
 */

void do_showlayers( CHAR_DATA *ch, char *argument )
{
	extern    OBJ_INDEX_DATA  *obj_index_hash[MAX_KEY_HASH];
	OBJ_INDEX_DATA *pObj;
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];

	int hash;                                           /* hash counter */
	int cou = 0;                                        /* display counter */
	int display_limit;                                  /* display limit */
	int layers;											/* layer level */
	bool all_layer = TRUE;								/* All Layers */

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);

	if ( !*arg1 )
	{
		send_to_char( "Syntax:\n\r", ch);
		send_to_char( "showlayers n  -  display maximum of n lines.\n\r", ch);
		return;
	}

	if ( !*arg2 )
	{
		all_layer = TRUE;
		layers = -1;
	}
	else if ( *arg2)
	{
		all_layer = FALSE;
		layers = atoi(arg2);
	}

	display_limit = atoi(arg1);
	pager_printf(ch, "      Vnum      Wear Layer   Description \n\r");
	for (hash = 0; hash < MAX_KEY_HASH; hash++) /* loop thru obj_index_hash */
	{
		if ( obj_index_hash[hash] )
		{
			for (pObj=obj_index_hash[hash]; pObj; pObj=pObj->next)
			{
				if (pObj->layers > 0 && all_layer)
				{
					if (++cou <= display_limit)
						pager_printf(ch, "%4d %5d %9d %5d   %s\n\r",
						cou, pObj->vnum, pObj->wear_flags, pObj->layers,
						pObj->short_descr);
				}
				else if (pObj->layers == layers && !all_layer)
				{
					if (++cou <= display_limit)
						pager_printf(ch,"%4d %5d %9d %5d   %s\n\r",
						cou, pObj->vnum, pObj->wear_flags, pObj->layers,
						pObj->short_descr);
				}
			}
		}
	}
}
