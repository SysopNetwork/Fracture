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

BW = 74                       # banner / rule width (before the 2-space indent)
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
_menu += IND + tab('M', '       Market - trade resources for Fame') + '\r\n'
_menu += IND + tab('K', '       Research - permanent skill upgrades') + '\r\n'
_menu += IND + tab('A', '       Attack - raid a rival camp') + '\r\n'
_menu += IND + tab('H', '       News & the world chronicle') + '\r\n'
_menu += IND + tab('T', '       Explorer status') + '\r\n'
_menu += IND + tab('F', '       Fame leaderboard') + '\r\n'
_menu += IND + tab('?', '       How to play - a quick guide') + '\r\n'
_menu += IND + tab('X', '       Exit to the BBS') + '\r\n'
_menu += '\r\n' + IND + WHT + 'Choose: ' + RST
MAINMEN = T('MAINMEN', _menu, 'Main menu')

_gap = '    '                 # wider gaps spread the tab bar across the screen
_short  = '\r\n' + IND + tab('N S E W','Travel') + _gap + tab('R','Search') + _gap + tab('L','Look') + _gap + tab('C','Camp') + '\r\n'
_short += IND + tab('P','Crew') + _gap + tab('M','Market') + _gap + tab('K','Research') + _gap + tab('A','Raid') + '\r\n'
_short += IND + tab('H','News') + _gap + tab('T','Status') + _gap + tab('X','Exit') + _gap + tab('?','Help') + '\r\n'
_short += IND + tab('F','Board') + _gap + tab('U','Upgrade') + '\r\n'
_short += IND + WHT + 'Choose: ' + RST
SHORTM = T('SHORTM', _short, 'Short menu')

# HELP - how to play. Costs no AP; reached with "?". Emitted LAST in the MSG
# (see the out.append at the foot of this file) so existing message numbers
# do not shift.
def hkey(keys, body):
    return IND + YEL + keys.ljust(10) + RST + GRY + body + RST + '\r\n'
_help  = banner('HOW TO PLAY', 'a living world, rebuilt each day')
_help += '\r\n'
_help += IND + GRY + 'Explore a ' + WHT + '%d x %d' + GRY + ' world a little each day. Every action costs ' + WHT + 'Action' + RST + '\r\n'
_help += IND + WHT + 'Points' + GRY + ' (AP); when they run out, come back tomorrow. Overnight your AP' + RST + '\r\n'
_help += IND + GRY + 'refill and the unexplored wilds regrow, so the world is never used up.' + RST + '\r\n'
_help += '\r\n'
_help += sect('WHAT YOU DO') + '\r\n'
_help += hkey('N S E W R', 'Explore: travel and search to chart new tiles and win Fame.')
_help += hkey('W R 3', 'Go far: travel a direction several tiles, R to search each.')
_help += hkey('C', 'Camp: spend resources to raise its tier and storage.')
_help += hkey('P', 'Crew: house workers who gather resources for you each day.')
_help += hkey('M', 'Market: sell finds for Fame, or buy what you are short on.')
_help += hkey('K', 'Research: permanent upgrades - better yields, cheaper travel.')
_help += hkey('A', 'Raid: attack a rival camp to carry off some of their goods.')
_help += '\r\n'
_help += sect('GETTING BY') + '\r\n'
_help += IND + GRY + 'Fame is your score - chart ground, trade well, and out-build rivals to' + RST + '\r\n'
_help += IND + GRY + 'top the ' + WHT + 'F' + GRY + ' board. ' + WHT + 'L' + GRY + 'ook, ' + WHT + 'T' + GRY + ' status, ' + WHT + 'F' + GRY + ' board and ' + WHT + '?' + GRY + ' help all cost no AP.' + RST + '\r\n'
_help += '\r\n'
_help += IND + DIM + 'Press ' + WHT + 'ENTER' + DIM + ' to return.' + RST
HELP = T('HELP', _help, 'How to play')

# STATUS - specifiers must stay: %s %ld %d %ld %ld %d %d %d %d %s %ld %ld %ld %ld %ld %ld
_st  = CLR + '\r\n' + sect('EXPLORER STATUS') + '\r\n' + rule() + '\r\n' + '\r\n'
_st += IND + GRY + ' Explorer   : ' + WHT + '%s' + RST + '\r\n'
_st += IND + GRY + ' Fame       : ' + YEL + '%ld' + RST + '\r\n'
_st += IND + GRY + ' Action Pts : ' + GRN + '%d' + GRY + ' remaining today' + RST + '\r\n'
_st += IND + GRY + ' Explored   : ' + WHT + '%ld' + GRY + '     Discovered : ' + WHT + '%ld' + RST + '\r\n'
_st += IND + GRY + ' Location   : ' + WHT + '(%d, %d)' + GRY + '   World ' + WHT + '%d x %d' + RST + '\r\n'
_st += IND + GRY + ' Camp       : ' + WHT + '%s' + GRY + '  (storage %ld each)' + RST + '\r\n'
_st += '\r\n' + rule() + '\r\n'
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
_cv  = CLR + '\r\n' + sect('YOUR CAMP') + '\r\n' + rule() + '\r\n' + '\r\n'
_cv += IND + GRY + ' Type     : ' + WHT + '%s' + RST + '\r\n'
_cv += IND + GRY + ' Location : ' + WHT + '(%d, %d)' + RST + '\r\n'
_cv += IND + GRY + ' Storage  : ' + WHT + '%ld' + GRY + ' of each resource' + RST + '\r\n'
_cv += '\r\n' + rule() + '\r\n' + RST
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

# --- market / trading post ---
MARKHD = T('MARKHD',
    CLR + '\r\n' + sect('TRADING POST') + '\r\n' + rule() + '\r\n' + RST,
    'Market screen header')

# MARKFT - specifier: %ld (player's Fame)
MARKFT = T('MARKFT',
    rule() + '\r\n' + IND + GRY + 'Your Fame: ' + YEL + '%ld' + RST + '\r\n'
    + IND + GRY + 'Sell with ' + WHT+RHB+' M S n q '+LHB + GRY + '   Buy with ' + WHT+RHB+' M B n q '+LHB + GRY + '   (n = 1-5, q = amount)' + RST + '\r\n' + RST,
    'Market footer')

# SOLD - specifiers: %ld qty, %s resource, %ld fame
SOLD = T('SOLD',
    IND + GRN+DS+MS+LS + RST + ' ' + GRY + 'Sold ' + WHT + '%ld' + GRY + ' %s for ' + YEL + '+%ld Fame' + GRY + '.' + RST + '\r\n',
    'Sold at market')

# BOUGHT - specifiers: %ld qty, %s resource, %ld fame
BOUGHT = T('BOUGHT',
    IND + MAG+DS+MS+LS + RST + ' ' + GRY + 'Bought ' + WHT + '%ld' + GRY + ' %s for ' + YEL + '-%ld Fame' + GRY + '.' + RST + '\r\n',
    'Bought at market')

MKTFULL = T('MKTFULL',
    IND + RED+RHB + RST + ' ' + RED + 'Your camp storage for that resource is already full.' + RST + '\r\n',
    'Market - storage full')

MKTNORE = T('MKTNORE',
    IND + RED+RHB + RST + ' ' + RED + 'You do not have that much to sell.' + RST + '\r\n',
    'Market - not enough resource')

MKTNOFA = T('MKTNOFA',
    IND + RED+RHB + RST + ' ' + RED + 'You do not have enough Fame to buy any.' + RST + '\r\n',
    'Market - not enough Fame')

MKTBADR = T('MKTBADR',
    IND + RED+RHB + RST + ' ' + RED + 'That is not a valid trade. Use ' + WHT + 'M S 3 10' + RED + ' to sell, ' + WHT + 'M B 3 10' + RED + ' to buy.' + RST + '\r\n',
    'Market - bad trade')

# --- research track ---
RSCHHD = T('RSCHHD',
    CLR + '\r\n' + sect('RESEARCH BENCH') + '\r\n' + rule() + '\r\n' + RST,
    'Research screen header')

# RSCHFT - specifiers: %ld Wood, %ld Stone, %ld Metal (next tier cost)
RSCHFT = T('RSCHFT',
    rule() + '\r\n' + IND + GRY + 'Next tier costs ' + YEL + '%ld' + GRY + ' Wood  ' + YEL + '%ld' + GRY + ' Stone  ' + YEL + '%ld' + GRY + ' Metal.' + RST + '\r\n'
    + IND + GRY + 'Research it with ' + WHT+RHB+' K R '+LHB + RST + '\r\n' + RST,
    'Research footer')

# RSCHUP - specifiers: %s tier name, %d gather pct
RSCHUP = T('RSCHUP',
    IND + MAG+DS+MS+LS + ' RESEARCHED! ' + RST + GRY + 'You reach ' + WHT + '%s' + GRY + '.  Gather bonus now ' + GRN + '+%d%%' + GRY + '.' + RST + '\r\n',
    'Research advanced')

RSCHMAX = T('RSCHMAX',
    IND + MAG+DS+MS+LS + RST + ' ' + MAG + 'Research complete - you have mastered every field.' + RST + '\r\n' + RST,
    'Research maxed')

RSCHNOR = T('RSCHNOR',
    IND + RED+RHB + RST + ' ' + RED + 'You do not have the resources for the next tier yet.' + RST + '\r\n',
    'Research - cannot afford')

# --- raiding ---
RAIDHD = T('RAIDHD',
    CLR + '\r\n' + sect('RAID A RIVAL') + '\r\n' + rule() + '\r\n' + RST,
    'Raid screen header')

# RAIDFT - specifier: %d (Action Point cost)
RAIDFT = T('RAIDFT',
    rule() + '\r\n' + IND + GRY + 'Raid a rival with ' + WHT+RHB+' A name '+LHB + GRY + ' - it costs ' + YEL + '%d' + GRY + ' Action Points.' + RST + '\r\n'
    + IND + GRY + 'Only camps built past a Tent can raid or be raided.' + RST + '\r\n' + RST,
    'Raid footer')

RAIDEMP = T('RAIDEMP',
    IND + YEL + 'No rivals have built a camp worth raiding yet.' + RST + '\r\n',
    'Raid - no targets')

# RAIDWIN - specifiers: %s target, %ld loot
RAIDWIN = T('RAIDWIN',
    IND + MAG+DS+MS+LS + ' RAID! ' + RST + GRY + 'You overrun ' + WHT + '%s' + GRY + ' and haul off ' + GRN + '%ld' + GRY + ' resources.  ' + YEL + '(+5 Fame)' + RST + '\r\n',
    'Raid succeeded')

# RAIDREP - specifier: %s target
RAIDREP = T('RAIDREP',
    IND + RED+DS+MS+LS + RST + ' ' + GRY + 'The camp of ' + WHT + '%s' + GRY + ' holds - your raid is driven off.' + RST + '\r\n',
    'Raid repelled')

# RAIDNOAP - specifier: %d Action Points needed
RAIDNOAP = T('RAIDNOAP',
    IND + RED+RHB + RST + ' ' + RED + 'A raid needs ' + WHT + '%d' + RED + ' Action Points, and you are short.' + RST + '\r\n',
    'Raid - not enough AP')

# RAIDPROT - specifier: %s target
RAIDPROT = T('RAIDPROT',
    IND + RED+RHB + RST + ' ' + RED + 'That camp is only a Tent - too small to raid. Pick someone built up.' + RST + '\r\n',
    'Raid - target protected')

RAIDWEAK = T('RAIDWEAK',
    IND + RED+RHB + RST + ' ' + RED + 'Build your camp to a Cabin before you can raid.' + RST + '\r\n',
    'Raid - raider too weak')

RAIDSELF = T('RAIDSELF',
    IND + RED+RHB + RST + ' ' + RED + 'You cannot raid your own camp.' + RST + '\r\n',
    'Raid - self')

RAIDBAD = T('RAIDBAD',
    IND + RED+RHB + RST + ' ' + RED + 'No such rival. Check the name and try ' + WHT + 'A name' + RED + ' again.' + RST + '\r\n',
    'Raid - no such target')

# RAIDED - specifier: %d times raided
RAIDED = T('RAIDED',
    IND + RED+DS+MS+LS + ' RAIDED! ' + RST + GRY + 'While you were away your camp was raided ' + WHT + '%d' + GRY + ' time(s).' + RST + '\r\n' + RST,
    'You were raided')

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

# --- config: trading post ---
out.append('LEVEL4 {Fracture: trading post}')
out.append('')
out.append(' Turn the trading post on or off. When on, players can sell')
out.append(' gathered resources for Fame and buy resources back with Fame at')
out.append(' the rates below (each scaled by how scarce the resource is).')
out.append('')
out.append('MKTOPT {Enable the trading post? YES} B')
out.append('')
out.append(' Base Fame earned for each unit of a resource sold, before the')
out.append(' per-resource weight is applied.')
out.append('')
out.append('MKTSELL {Fame earned per resource unit sold? 2} N 0 100')
out.append('')
out.append(' Base Fame charged for each unit of a resource bought. Keep this at')
out.append(' or above the sell rate so trading is not a free source of Fame.')
out.append('')
out.append('MKTBUY {Fame charged per resource unit bought? 3} N 0 100')
out.append('')
out.append('LEVEL6 {Fracture: market text}')
out.append('')
for m in [MARKHD, MARKFT, SOLD, BOUGHT, MKTFULL, MKTNORE, MKTNOFA, MKTBADR]:
    out.append(m.rstrip('\r\n'))
    out.append('')

# --- config: research ---
out.append('LEVEL4 {Fracture: research}')
out.append('')
out.append(' Turn the research track on or off. When on, players spend gathered')
out.append(' resources to advance research tiers, each of which permanently')
out.append(' raises how much searching yields and, higher up, cheapens travel.')
out.append('')
out.append('RSCHOPT {Enable the research track? YES} B')
out.append('')
out.append('LEVEL6 {Fracture: research text}')
out.append('')
for m in [RSCHHD, RSCHFT, RSCHUP, RSCHMAX, RSCHNOR]:
    out.append(m.rstrip('\r\n'))
    out.append('')

# --- config: raiding ---
out.append('LEVEL4 {Fracture: raiding}')
out.append('')
out.append(' Turn player-versus-player raiding on or off. When on, a player with')
out.append(' a camp built past a Tent can raid a rival to carry off a share of')
out.append(' their resources; newcomers (still in a Tent) cannot raid or be')
out.append(' raided. Set to NO for a purely peaceful server.')
out.append('')
out.append('RADOPT {Enable player raiding? YES} B')
out.append('')
out.append(' Action Points a raid costs the attacker, win or lose. A higher cost')
out.append(' makes raiding a bigger commitment of the daily pool.')
out.append('')
out.append('RADCOST {Action Points per raid? 5} N 0 200')
out.append('')
out.append(' Percentage of each of the defender\'s resources a successful raid')
out.append(' carries off (capped by the raider\'s own storage).')
out.append('')
out.append('RADLOOT {Percent of resources looted on a win? 25} N 0 100')
out.append('')
out.append('LEVEL6 {Fracture: raid text}')
out.append('')
for m in [RAIDHD, RAIDFT, RAIDEMP, RAIDWIN, RAIDREP, RAIDNOAP, RAIDPROT,
          RAIDWEAK, RAIDSELF, RAIDBAD, RAIDED]:
    out.append(m.rstrip('\r\n'))
    out.append('')

# --- config: testing / casual play ---
out.append('LEVEL4 {Fracture: testing}')
out.append('')
out.append(' Refill every player\'s Action Points to the full daily amount on')
out.append(' each entry, instead of only at the midnight reset. Intended for')
out.append(' testing and casual servers that do not want a daily cap. Leave this')
out.append(' NO for normal play.')
out.append('')
out.append('APFREE {Refill Action Points on every entry? NO} B')
out.append('')

# --- help text (emitted last so message numbers of existing entries hold) ---
out.append('LEVEL6 {Fracture: help text}')
out.append('')
out.append(HELP.rstrip('\r\n'))
out.append('')

text = CRLF.join(out) + CRLF
data = text.encode('cp437')

with open('FRACTURE.MSG.new', 'wb') as f:
    f.write(data)
print('wrote FRACTURE.MSG.new  (%d bytes)' % len(data))
