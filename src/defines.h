/*
 * Unknown Regions Version -- Scryn
 */
#define UR_VERSION_MAJOR		"1"
#define UR_VERSION_MINOR		"0.11"

#define ANSI_KEY				'&'
#define ANSI_CODE_LENGTH		2

#define MAX_VNUMS				2000000000
/*#define MAX_VNUMS				2097152000*/

#define MOB_VNUM_VENDOR			51				/* vnum of vendor*/
#define AGE_BUY_VENDOR			18				/* minimum age to buy a vendor */
#define COST_BUY_VENDOR			1				/* cost of a vendor */
#define OBJ_VNUM_DEED			79				/* vnum of deed */
#define ROOM_VNUM_VENSTOR		2				/* where messed up vendors go! */
#define VENDOR_DIR				"../vendor/"	/* Directory For Vendors */
#define VENDOR_FEE				0.5				/* Money That Vendors Take */

#define MAX_HAIR				36
#define MAX_EYE					20
#define MAX_BUILD				10
#define MAX_COMPLEX				8

#define ROOM_PLUOGUS_QUIT		32148			/* Room That Idlers Quit */

#define ROOM_SHUTTLE_BUS		32140			/* Control Room For PLUOGUS */
#define ROOM_SHUTTLE_BUS_2		32410			/* Control Room For TOCCA */

#define ROOM_CORUSCANT_SHUTTLE	199				/* Control Room for Coruscant Shuttle */
#define ROOM_SHUTTLE1			7265			/* Control Room for Shuttle 1 */
#define ROOM_CORUSCANT_TURBOCAR	226				/* Control Room for Coruscant Turbocar */

/*
 * Shuttle 2 (Automated Shuttle 2)
 */
#define SHUTTLE1_STOP1_MSG	"Welcome to the Coruscant Crescent Moon."
#define SHUTTLE1_STOP1       7224
#define SHUTTLE1_STOP2       202
#define SHUTTLE1_STOP2_MSG	"Welcome to Coruscant."


/*
 * Body Parts Stuff
 */
typedef enum
{
	BODY_L_LEG, BODY_R_LEG, 
	BODY_L_FOOT, BODY_R_FOOT,
	BODY_L_ARM,	BODY_R_ARM,
	BODY_L_WRIST, BODY_R_WRIST,
	BODY_L_KNEE, BODY_R_KNEE,
	BODY_L_ANKLE, BODY_R_ANKLE,
	BODY_L_SHOLDER, BODY_R_SHOLDER,
	BODY_L_HAND, BODY_R_HAND,
	BODY_NOSE,	BODY_RIBS,
	BODY_JAW, BODY_STOMACH,
	BODY_CHEST,
	MAX_BODY_PARTS
} body_part_flags;
extern  char *  const   body_parts    [MAX_BODY_PARTS];


