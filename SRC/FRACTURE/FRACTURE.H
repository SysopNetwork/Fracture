/*  Message-number definitions derived from "FRACTURE.MSG"               */
/*  Numbers are the sequential position of each entry in the .MSG file,   */
/*  counting the LEVELn markers. Keep this header in sync with the .MSG.  */

#define WELCOPT     1   /* B  - sysop option: greet users at logon?        */
#define DAYAP       2   /* N  - sysop option: Action Points granted daily  */
#define WLDSIZ      3   /* N  - sysop option: world dimension (tiles)      */
#define LOGON       5   /* T  - welcome shown at BBS logon                 */
#define MAINMEN     6   /* T  - the module's main menu                     */
#define SHORTM      7   /* T  - short re-prompt                            */
#define STATUS      8   /* T  - explorer status (name/fame/AP/stats/pos)   */
#define EXITMSG     9   /* T  - farewell on leaving the module (%s userid) */
#define INVALID    10   /* T  - invalid menu selection                     */
#define FIRSTM     11   /* T  - greeting for a brand-new explorer          */
#define NOAP       12   /* T  - out of Action Points for today             */
#define EXPNADA    13   /* T  - search outcome: nothing gathered           */
#define NEWTILE    14   /* T  - search outcome: charted new tile (%s, +%d) */
#define EDGE       15   /* T  - blocked at the edge of the world           */
#define MOVED      16   /* T  - travel confirmation (%s direction)         */
#define LOOKHD     17   /* T  - local view header (%d x, %d y, %s terrain) */
#define LEGEND     18   /* T  - local map legend                           */
#define GATHER     19   /* T  - resources gathered (%d amount, %s name)    */
