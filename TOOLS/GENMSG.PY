# -*- coding: utf-8 -*-
# Generates FRACTURE.MSG in CP437 with the shaded block-art ANSI style.
# Entry names, types, order, and printf specifiers are preserved so the
# compiled message numbering matches FRACTURE.H exactly.

ESC = '\x1b'
def c(code): return ESC + '[' + code + 'm'
CYN=c('1;36'); WHT=c('1;37'); YEL=c('1;33'); GRY=c('0;37')
RED=c('1;31'); MAG=c('1;35'); GRN=c('1;32'); DIM=c('1;30'); RST=c('0')
CLR = ESC + '[2J'

# CP437 block glyphs
LS='░'; MS='▒'; DS='▓'; FB='█'
LHB='▌'; RHB='▐'

BW = 52                       # banner / rule width (before the 2-space indent)
IND = '  '

def bar():
    return IND + CYN + DS*BW + RST

def brow(text, tcol):
    inner = BW - 10
    t = text[:inner].ljust(inner)
    return IND + CYN+DS+MS+LS + RST + '  ' + tcol + t + RST + '  ' + CYN+LS+MS+DS + RST

def banner(title, sub):
    return CLR + '\r\n' + '\r\n'.join([bar(), brow(title, WHT), brow(sub, GRY), bar()]) + '\r\n'

def banner_noclr(title, sub):
    return '\r\n'.join([bar(), brow(title, WHT), brow(sub, GRY), bar()]) + '\r\n'

def sect(label):
    return IND + CYN+DS+MS+LS+FB + RST + ' ' + WHT + label + RST + ' ' + CYN+FB+LS+MS+DS + RST

def rule():
    return IND + CYN + LS+MS + DS*(BW-6) + MS+LS + RST

def tab(keys, label):
    return YEL+RHB + RST + ' ' + WHT+keys+RST + ' ' + YEL+LHB + RST + GRY+label+RST

def T(name, body, label):
    return name + ' {' + body + '} T ' + label + '\r\n'

# ---------------------------------------------------------------- messages

LOGON = T('LOGON',
    CYN+DS+MS+LS + RST + ' ' + WHT+'FRACTURE' + RST + ' ' + GRY+'- a living world, rebuilt every day.' + RST + '\r\n' + RST,
    'Logon greeting')

_menu = banner_noclr('F R A C T U R E', 'a living world, rebuilt each day') + '\r\n'
_menu += IND + tab('N S E W', ' Travel the world  (1 AP)') + '\r\n'
_menu += IND + tab('R', '       Search the ground  (1 AP)') + '\r\n'
_menu += IND + tab('L', '       Look around (local map)') + '\r\n'
_menu += IND + tab('C', '       Camp - view and build it up') + '\r\n'
_menu += IND + tab('P', '       Crew - workers & production') + '\r\n'
_menu += IND + tab('H', '       News & the world chronicle') + '\r\n'
_menu += IND + tab('T', '       Explorer status') + '\r\n'
_menu += IND + tab('X', '       Exit to the BBS') + '\r\n'
_menu += '\r\n' + IND + WHT + 'Choose: ' + RST
MAINMEN = T('MAINMEN', _menu, 'Main menu')

_short  = IND + tab('N S E W','Travel') + '  ' + tab('R','Search') + '  ' + tab('L','Look') + '  ' + tab('C','Camp') + '\r\n'
_short += IND + tab('P','Crew') + '  ' + tab('H','News') + '  ' + tab('T','Status') + '  ' + tab('X','Exit') + '  ' + tab('?','Menu') + '\r\n'
_short += IND + WHT + 'Choose: ' + RST
SHORTM = T('SHORTM', _short, 'Short menu')

# STATUS - specifiers must stay: %s %ld %d %ld %ld %d %d %s %ld %ld %ld %ld %ld %ld
_st  = CLR + '\r\n' + sect('EXPLORER STATUS') + '\r\n' + rule() + '\r\n'
_st += IND + GRY + ' Explorer   : ' + WHT + '%s' + RST + '\r\n'
_st += IND + GRY + ' Fame       : ' + YEL + '%ld' + RST + '\r\n'
_st += IND + GRY + ' Action Pts : ' + GRN + '%d' + GRY + ' remaining today' + RST + '\r\n'
_st += IND + GRY + ' Explored   : ' + WHT + '%ld' + GRY + '     Discovered : ' + WHT + '%ld' + RST + '\r\n'
_st += IND + GRY + ' Location   : ' + WHT + '(%d, %d)' + RST + '\r\n'
_st += IND + GRY + ' Camp       : ' + WHT + '%s' + GRY + '  (storage %ld each)' + RST + '\r\n'
_st += rule() + '\r\n'
_st += IND + WHT + ' RESOURCES  ' + GRY + 'Wood ' + WHT + '%ld' + GRY + '  Stone ' + WHT + '%ld' + GRY + '  Metal ' + WHT + '%ld' + GRY + '  Food ' + WHT + '%ld' + GRY + '  Water ' + WHT + '%ld' + RST + '\r\n'
_st += rule() + '\r\n' + RST
STATUS = T('STATUS', _st, 'Explorer status')

EXITMSG = T('EXITMSG',
    '\r\n' + IND + CYN+DS+MS+LS + RST + ' ' + GRY + 'Safe travels, ' + YEL + '%s' + GRY + '. The world will have changed when you return.' + RST + ' ' + CYN+LS+MS+DS + RST + '\r\n' + RST,
    'Exit message')

INVALID = T('INVALID',
    IND + RED+RHB + RST + ' ' + RED + 'That is not a valid option.' + RST + '\r\n',
    'Invalid selection')

_fm  = CLR + '\r\n' + banner_noclr('WELCOME, EXPLORER', 'a fractured world awaits your mark') + '\r\n'
_fm += IND + GRY + 'A fractured world stretches out before you, rebuilt a little' + RST + '\r\n'
_fm += IND + GRY + 'each day. Travel it, chart new ground, and earn your Fame.' + RST + '\r\n'
_fm += IND + GRY + 'Your camp stands at the center of the map - build it up.' + RST + '\r\n' + RST
FIRSTM = T('FIRSTM', _fm, 'First-time welcome')

NOAP = T('NOAP',
    IND + RED+RHB + RST + ' ' + RED + 'You are out of Action Points for today.' + RST + '\r\n' + IND + GRY + '  Rest now; more will arrive with the new day.' + RST + '\r\n' + RST,
    'Out of Action Points')

EXPNADA = T('EXPNADA',
    IND + GRY + RHB + ' You search the area but turn up nothing to gather.' + RST + '\r\n',
    'Search - nothing')

NEWTILE = T('NEWTILE',
    IND + MAG+DS+MS+LS + ' NEW GROUND! ' + RST + GRY + 'You chart unexplored ' + WHT + '%s' + GRY + '.  ' + YEL + '(+%d Fame)' + RST + '\r\n',
    'Search - charted new tile')

EDGE = T('EDGE',
    IND + RED+RHB + RST + ' ' + RED + 'You cannot travel beyond the edge of the known world.' + RST + '\r\n',
    'Edge of world')

MOVED = T('MOVED', IND + GRY + 'You travel %s.' + RST + '\r\n', 'Travel confirmation')

LOOKHD = T('LOOKHD',
    CLR + '\r\n' + IND + CYN+DS+MS+LS+FB + RST + ' ' + WHT + 'LOCATION (%d, %d)' + GRY + ' - ' + WHT + '%s' + RST + ' ' + CYN+FB+LS+MS+DS + RST,
    'Look header')

_leg  = RST + '\r\n' + rule() + '\r\n'
_leg += IND + WHT + '@' + GRY + ' you   ' + YEL + '?' + GRY + ' unexplored   ' + '. plains  f forest  ^ mountains' + RST + '\r\n'
_leg += IND + GRY + ': desert  ~ swamp  w water  R ruins' + RST + '\r\n' + RST
LEGEND = T('LEGEND', _leg, 'Map legend')

GATHER = T('GATHER',
    IND + GRY + RHB + ' You gather ' + GRN + '%d' + GRY + ' %s.' + RST + '\r\n',
    'Resources gathered')

# CAMPVW - specifiers: %s %d %d %ld
_cv  = CLR + '\r\n' + sect('YOUR CAMP') + '\r\n' + rule() + '\r\n'
_cv += IND + GRY + ' Type     : ' + WHT + '%s' + RST + '\r\n'
_cv += IND + GRY + ' Location : ' + WHT + '(%d, %d)' + RST + '\r\n'
_cv += IND + GRY + ' Storage  : ' + WHT + '%ld' + GRY + ' of each resource' + RST + '\r\n'
_cv += rule() + '\r\n' + RST
CAMPVW = T('CAMPVW', _cv, 'Camp view')

CAMPUPC = T('CAMPUPC',
    IND + GRY + 'Upgrade to ' + WHT + '%s' + GRY + ' costs ' + YEL + '%ld' + GRY + ' Wood  ' + YEL + '%ld' + GRY + ' Stone  ' + YEL + '%ld' + GRY + ' Metal.' + RST + '\r\n' + IND + GRY + 'Press ' + YEL+RHB+' U '+LHB + GRY + ' to build it.' + RST + '\r\n' + RST,
    'Camp upgrade cost')

CAMPMAX = T('CAMPMAX',
    IND + MAG+DS+MS+LS + RST + ' ' + MAG + 'Your camp is fully built - a mighty Citadel.' + RST + '\r\n' + RST,
    'Camp at max')

CAMPUP = T('CAMPUP',
    IND + MAG+DS+MS+LS + ' CAMP UPGRADED! ' + RST + GRY + 'Your camp is now a ' + WHT + '%s' + GRY + '.  ' + YEL + '(+%d Fame)' + RST + '\r\n',
    'Camp upgraded')

CANTAFF = T('CANTAFF',
    IND + RED+RHB + RST + ' ' + RED + 'You do not have the resources for that upgrade yet.' + RST + '\r\n',
    'Cannot afford')

STORFUL = T('STORFUL',
    IND + RED+RHB + RST + ' ' + RED + 'Your camp storage is full - make room by upgrading it.' + RST + '\r\n',
    'Storage full')

NEWSHDR = T('NEWSHDR',
    banner('THE FRACTURE CHRONICLE', 'the shared story of this world') + '\r\n' + RST,
    'News header')

# NEWSDSC - specifiers: %s %s %s %d %d
NEWSDSC = T('NEWSDSC',
    IND + CYN+RHB + RST + ' ' + DIM + '%s' + RST + '  ' + WHT + '%s' + GRY + ' charted ' + MAG + '%s' + GRY + ' at (%d, %d)' + RST + '\r\n',
    'News - discovery')

# NEWSBLD - specifiers: %s %s %s
NEWSBLD = T('NEWSBLD',
    IND + CYN+RHB + RST + ' ' + DIM + '%s' + RST + '  ' + WHT + '%s' + GRY + ' built a ' + YEL + '%s' + RST + '\r\n',
    'News - building')

NEWSNON = T('NEWSNON',
    IND + YEL + 'The chronicle is empty. Be the first to make history.' + RST + '\r\n',
    'News - empty')

NEWSFTR = T('NEWSFTR', rule() + '\r\n' + RST, 'News footer')

# WXLINE - specifiers: %s %s %s  (weather name, flavor, season)
WXLINE = T('WXLINE',
    IND + CYN+LS+MS+DS+FB + RST + ' ' + WHT + 'Weather ' + YEL + '%s' + GRY + ' - %s' + WHT + '   Season ' + YEL + '%s' + RST + '\r\n' + RST,
    'Weather & season line')

# --- workers / production ---
CREWHD = T('CREWHD',
    CLR + '\r\n' + sect('YOUR CREW') + '\r\n' + rule() + '\r\n' + RST,
    'Crew screen header')

# CREWFT - specifiers: %d used, %d cap, %d wood cost, %d food cost
CREWFT = T('CREWFT',
    rule() + '\r\n' + IND + YEL + '%d' + GRY + ' of ' + YEL + '%d' + GRY + ' crew slots filled.' + RST + '\r\n'
    + IND + GRY + 'Recruit with ' + WHT+RHB+' P n '+LHB + GRY + ' (n = 1-5).  Each costs ' + YEL + '%d' + GRY + ' Wood, ' + YEL + '%d' + GRY + ' Food.' + RST + '\r\n' + RST,
    'Crew footer')

HIRED = T('HIRED',
    IND + MAG+DS+MS+LS + RST + ' ' + GRY + 'You take on a ' + WHT + '%s' + GRY + ' - welcome to the crew.' + RST + '\r\n',
    'Recruited a worker')

HIREFUL = T('HIREFUL',
    IND + RED+RHB + RST + ' ' + RED + 'Your camp cannot house any more workers - build it up first.' + RST + '\r\n',
    'Crew at capacity')

# HIRECST - specifiers: %d wood cost, %d food cost
HIRECST = T('HIRECST',
    IND + RED+RHB + RST + ' ' + RED + 'Not enough resources - a worker costs ' + WHT + '%d' + RED + ' Wood and ' + WHT + '%d' + RED + ' Food.' + RST + '\r\n',
    'Cannot afford worker')

HIREBAD = T('HIREBAD',
    IND + RED+RHB + RST + ' ' + RED + 'That is not a worker to hire. Use ' + WHT + 'P 1' + RED + ' through ' + WHT + 'P 5' + RED + '.' + RST + '\r\n',
    'Invalid worker selection')

# PRODSUM - specifier: %d total gained
PRODSUM = T('PRODSUM',
    IND + GRN+DS+MS+LS + RST + ' ' + GRY + 'While you were away your crew stocked ' + GRN + '%d' + GRY + ' resource(s).' + RST + '\r\n',
    'Production summary')

PRODHUN = T('PRODHUN',
    IND + RED+RHB + RST + ' ' + RED + 'Some workers went hungry - keep Food in storage.' + RST + '\r\n',
    'Workers hungry')

# --- arrival quips (restyled, keep [2J and one per entry) ---
QUIP_TEXT = [
    '"Finally, I\'m here."',
    '"Better late than..."',
    '"Bow down, peasants."',
    '"Chaos has arrived."',
    '"Professional overthinker here."',
    '"An absolute disaster, reporting in."',
    '"Hide your snacks."',
    '"I brought snacks."',
    '"Fear me, mortals."',
    '"Death warmed over. Let\'s do this."',
    '"Nobody panic. ...Okay, maybe panic."',
    '"The disappointment is mutual."',
]
def quip(n, text):
    body = CLR + '\r\n' + IND + CYN+DS+MS+LS + RST + ' ' + YEL + text + RST + '\r\n' + RST
    return T('QUIP%d' % n, body, 'Arrival quip %d' % n)

# ---------------------------------------------------------------- assemble

CRLF = '\r\n'
out = []
out.append('')  # leading blank line (file begins with CRLF)

# --- config: general ---
out.append('LEVEL4 {Fracture: configurable options}')
out.append('')
out.append(' If you want Fracture to greet players with a short message as')
out.append(' they log on to the BBS, set this option to YES.')
out.append('')
out.append('WELCOPT {Greet players at logon? YES} B')
out.append('')
out.append(' How many Action Points each player receives per day. Action')
out.append(' Points are spent on actions such as traveling and searching.')
out.append('')
out.append('DAYAP {Action Points granted to each player per day? 20} N 1 200')
out.append('')
out.append(' The size of the world, in tiles, along each edge. The world is')
out.append(' first generated at this size. Changing it later resizes the')
out.append(' playable area on the next launch; any player left outside the new')
out.append(' bounds is brought home to their camp.')
out.append('')
out.append('WLDSIZ {World size (tiles per edge)? 40} N 8 64')
out.append('')
out.append('LEVEL6 {Fracture: module text}')
out.append('')

# --- module text (restyled) ---
for m in [LOGON, MAINMEN, SHORTM, STATUS, EXITMSG, INVALID, FIRSTM, NOAP,
          EXPNADA, NEWTILE, EDGE, MOVED, LOOKHD, LEGEND, GATHER, CAMPVW,
          CAMPUPC, CAMPMAX, CAMPUP, CANTAFF, STORFUL, NEWSHDR, NEWSDSC,
          NEWSBLD, NEWSNON, NEWSFTR]:
    out.append(m.rstrip('\r\n'))   # each T() already ends with CRLF; add blank sep below
    out.append('')

# --- config: world evolution ---
out.append('LEVEL4 {Fracture: world evolution}')
out.append('')
out.append(' The percentage of unexplored tiles regenerated with fresh terrain')
out.append(' each midnight, during the nightly cleanup. Charted tiles, camps,')
out.append(' and player-built structures are never changed. Set to 0 to freeze')
out.append(' the world.')
out.append('')
out.append('REGENPCT {Unexplored tiles regenerated each midnight (%)? 5} N 0 100')
out.append('')

# --- arrival quips ---
out.append('LEVEL6 {Fracture: arrival quips}')
out.append('')
for i, txt in enumerate(QUIP_TEXT, start=1):
    out.append(quip(i, txt).rstrip('\r\n'))
    out.append('')

# --- config: weather & seasons ---
out.append('LEVEL4 {Fracture: weather & seasons}')
out.append('')
out.append(' Turn the daily weather and season cycle on or off. When on, one')
out.append(' weather condition affects the whole server each day (shown on')
out.append(' entry, in status, and with the news), gathering yields rise and')
out.append(' fall with it, and storms and snow make travel cost more Action')
out.append(' Points.')
out.append('')
out.append('WXOPT {Enable weather and seasons? YES} B')
out.append('')
out.append(' How many days each season lasts before the world turns to the')
out.append(' next. Seasons cycle Spring, Summer, Autumn, Winter and bias which')
out.append(' weather each day is likely to bring.')
out.append('')
out.append('SEASLEN {Days per season? 7} N 1 90')
out.append('')
out.append('LEVEL6 {Fracture: weather text}')
out.append('')
out.append(WXLINE.rstrip('\r\n'))
out.append('')

# --- config: workers & production ---
out.append('LEVEL4 {Fracture: workers & production}')
out.append('')
out.append(' Turn the crew system on or off. When on, players can house NPC')
out.append(' workers at their camp (up to a limit set by the camp tier) who')
out.append(' produce resources passively - one day\'s worth each new day the')
out.append(' player logs in. The crew eats Food, and the day\'s weather scales')
out.append(' what they produce.')
out.append('')
out.append('WRKOPT {Enable workers and production? YES} B')
out.append('')
out.append(' Base units of its resource each worker produces per day, before')
out.append(' the day\'s weather is applied.')
out.append('')
out.append('WRKPROD {Resource units produced per worker per day? 2} N 0 50')
out.append('')
out.append(' Food each worker eats per day. If the camp cannot feed the whole')
out.append(' crew, the hungry workers still produce but the shortfall is noted.')
out.append('')
out.append('WRKFOOD {Food eaten per worker per day? 1} N 0 50')
out.append('')
out.append('LEVEL6 {Fracture: worker text}')
out.append('')
for m in [CREWHD, CREWFT, HIRED, HIREFUL, HIRECST, HIREBAD, PRODSUM, PRODHUN]:
    out.append(m.rstrip('\r\n'))
    out.append('')

text = CRLF.join(out) + CRLF
data = text.encode('cp437')

with open('FRACTURE.MSG.new', 'wb') as f:
    f.write(data)
print('wrote FRACTURE.MSG.new  (%d bytes)' % len(data))
