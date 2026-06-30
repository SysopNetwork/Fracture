/*  Message-number definitions derived from "FRACTURE.MSG"               */
/*  Numbers are the sequential position of each entry in the .MSG file,   */
/*  counting the LEVELn markers. Keep this header in sync with the .MSG.  */

#define WELCOPT     1   /* B  - sysop option: greet users at logon?        */
#define DAYAP       2   /* N  - sysop option: Action Points granted daily  */
#define LOGON       4   /* T  - welcome shown at BBS logon                 */
#define MAINMEN     5   /* T  - the module's main menu                     */
#define SHORTM      6   /* T  - short re-prompt                            */
#define STATUS      7   /* T  - explorer status (name, fame, AP, stats)    */
#define EXITMSG     8   /* T  - farewell on leaving the module (%s userid) */
#define INVALID     9   /* T  - invalid menu selection                     */
#define FIRSTM     10   /* T  - greeting for a brand-new explorer          */
#define NOAP       11   /* T  - out of Action Points for today             */
#define EXPNADA    12   /* T  - explore outcome: nothing found             */
#define EXPRES     13   /* T  - explore outcome: minor find (+%d fame)     */
#define EXPDISC    14   /* T  - explore outcome: discovery (+%d fame)      */
