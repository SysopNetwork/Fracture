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
#define MOVED      16   /* T  - travel confirmation (unused: erased by the */
                        /*      map redraw; kept to preserve numbering)     */
#define LOOKHD     17   /* T  - local view header (%d x, %d y, %s terrain) */
#define LEGEND     18   /* T  - local map legend                           */
#define GATHER     19   /* T  - resources gathered (%d amount, %s name)    */
#define CAMPVW     20   /* T  - camp view (%s tier, %d x, %d y, %ld store) */
#define CAMPUPC    21   /* T  - next upgrade cost (%s, %ld, %ld, %ld)      */
#define CAMPMAX    22   /* T  - camp fully upgraded                        */
#define CAMPUP     23   /* T  - upgrade complete (%s tier, +%d fame)       */
#define CANTAFF    24   /* T  - cannot afford the upgrade                  */
#define STORFUL    25   /* T  - storage full, gathered nothing            */
#define NEWSHDR    26   /* T  - news chronicle header                     */
#define NEWSDSC    27   /* T  - news line: discovery (%s date, %s who,    */
                        /*      %s terrain, %d x, %d y)                    */
#define NEWSBLD    28   /* T  - news line: build (%s date, %s who, %s tier)*/
#define NEWSNON    29   /* T  - news: the chronicle is empty              */
#define NEWSFTR    30   /* T  - news chronicle footer                     */
/* entry 31 is a second LEVEL4 marker (more configuration options)         */
#define REGENPCT   32   /* N  - sysop option: % unexplored tiles regen'd   */
                        /*      each midnight (0..100)                     */
/* entry 33 is a LEVEL6 marker (arrival quips)                             */
#define QUIP1      34   /* T  - first arrival quip; QUIP1..QUIPN are a     */
#define QUIPN      45   /* T  - contiguous pool, one shown at random       */
/* entry 46 is a LEVEL4 marker (weather & seasons config)                  */
#define WXOPT      47   /* B  - sysop option: enable weather & seasons     */
#define SEASLEN    48   /* N  - sysop option: days per season (1..90)      */
/* entry 49 is a LEVEL6 marker (weather text)                              */
#define WXLINE     50   /* T  - daily weather/season line (%s name, %s     */
                        /*      flavor, %s season)                         */
/* entry 51 is a LEVEL4 marker (workers & production config)               */
#define WRKOPT     52   /* B  - sysop option: enable workers & production  */
#define WRKPROD    53   /* N  - sysop option: units per worker per day     */
#define WRKFOOD    54   /* N  - sysop option: Food per worker per day       */
/* entry 55 is a LEVEL6 marker (worker text)                               */
#define CREWHD     56   /* T  - crew screen frame/header                   */
#define CREWFT     57   /* T  - crew footer (%d used, %d cap, %d wood cost, */
                        /*      %d food cost)                              */
#define HIRED      58   /* T  - recruited a worker (%s worker name)        */
#define HIREFUL    59   /* T  - crew at camp capacity                      */
#define HIRECST    60   /* T  - cannot afford (%d wood cost, %d food cost) */
#define HIREBAD    61   /* T  - invalid worker selection                   */
#define PRODSUM    62   /* T  - daily production summary (%d total gained)  */
#define PRODHUN    63   /* T  - some workers went hungry (no Food)          */
