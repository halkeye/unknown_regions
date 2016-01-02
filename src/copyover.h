/*******************************************************************
********************************************************************
**  	         	Crimson Blade Codebase                    **
** 		 Coders: Noplex, Shinju, Rithmic                  **
**		CbC is based on the Smaug 1.4a Codebase           **
********************************************************************
*******************************************************************/

/****************************************************
 * Original Copyover patch by Erwin Andreasen.      *
 * This provides some changes, and easy installation*
 * instructions, and works for 1.4, and 1.4a.       *
 * --Callidyrr                                      *
 ****************************************************/

/*
 * Copyover v2.0(SMAUG) Header File
 */

/* 
 * Make sure you add CON_COPYOVER_RECOVER to the list of CON_STATES
 */

bool		fSaveCopyOver;

#define CH(d)                  ((d)->original ? (d)->original : (d)->character)
#define COPYOVER_FILE  SYSTEM_DIR "copyover.dat" /* for warm reboots    */
#define EXE_FILE       "../src/unknown"            /* executable path     */
#define MOBSAVE_FILE	"mobsave.dat"   /* save/load world data */
#define OBJSAVE_FILE	"objsave.dat"   /* save/load world data */

#define OS_CARRY	0
#define OS_CORPSE	1
#define OS_GROUND	2
#define OS_LOCKER	3

bool   write_to_descriptor        args( ( int desc, char *txt, int length ) ); /* wtb */
void   do_copyover		  args( ( CHAR_DATA *ch, char *argument ) ); /* copyover game call */
void   copyover 		  args( ( void ) ); /* copyover function itself */
void   init_descriptor            args( ( DESCRIPTOR_DATA *dnew, int desc) ); /* not used anymore except comm.c */
void   copyover_recover           args( (void) ); /* copyover recover... called db.c */
void   I3_shutdown	  	  args( ( int delay ) ); /* i3 shutdown */
void   imc_shutdown		  args( (void) ); /* imc shutdown */
void   boot_db         		  args( ( bool fCopyOver ) ); /* db.c */
void   log_printf      		  args( (char *fmt, ...) );
void   copyover_check		  args( ( void ) ); /* Timed Copyovers */
void   save_world		  args( ( void ) ); /* Saving the World */
void   load_world		  args( ( void ) ); /* Loading the World */
void   save_gamedata		  args( ( void ) ); /* Gamedata Save */

/* Lemme' Save you the trouble.. */
DECLARE_DO_FUN( do_copyover     );

