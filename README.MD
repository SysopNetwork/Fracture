<div align="center">

```
       ███████╗██████╗  █████╗  ██████╗████████╗██╗   ██╗██████╗ ███████╗
       ██╔════╝██╔══██╗██╔══██╗██╔════╝╚══██╔══╝██║   ██║██╔══██╗██╔════╝
       █████╗  ██████╔╝███████║██║        ██║   ██║   ██║██████╔╝█████╗
       ██╔══╝  ██╔══██╗██╔══██║██║        ██║   ██║   ██║██╔══██╗██╔══╝
       ██║     ██║  ██║██║  ██║╚██████╗   ██║   ╚██████╔╝██║  ██║███████╗
       ╚═╝     ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝╚══════╝
```

*A living world rebuilt every day. Compete to leave your mark before it vanishes.*

**A persistent, multiplayer exploration and survival-strategy door game for The Major BBS v10.**

By **[Sysop Network](https://github.com/SysopNetwork)** — https://github.com/SysopNetwork/Fracture

[![The Major BBS](https://img.shields.io/badge/The%20Major%20BBS-v10-blue)](https://github.com/SysopNetwork/Fracture)
[![Language](https://img.shields.io/badge/language-C-555555?logo=c&logoColor=white)]()
[![Platform](https://img.shields.io/badge/platform-Windows%20x86%20(Win32)-lightgrey?logo=windows&logoColor=white)]()
[![License](https://img.shields.io/badge/license-MIT-blue?logo=opensourceinitiative&logoColor=white)](LICENSE)
[![Release](https://img.shields.io/github/v/release/SysopNetwork/Fracture)](https://github.com/SysopNetwork/Fracture/releases)

📋 **[See what's new in v1.0.0 →](CHANGELOG.MD)** · 🛠️ **[Technical reference →](TECHNICAL.MD)**

</div>

---

Players log in daily, spend a limited pool of Action Points to explore a tile map,
build and upgrade a camp, gather resources, research technology, and compete to
become the most famous explorer on the server — in a world that changes a little
every midnight. Sessions are short (5–10 minutes), fun solo, and better with a crowd.

## ✨ Features

| | Feature | Description |
|---|---|---|
| ⏳ | **Daily Action Points** | Every meaningful action (travel, search, build, research, raid) spends Action Points, so sessions stay quick |
| 🌍 | **A World That Evolves** | At midnight a configurable share of unexplored tiles is regenerated; charted ground and player camps are always preserved |
| 🧭 | **First Discoveries** | Charting a tile first earns Fame and a permanent entry in the server chronicle |
| 🏕️ | **Camp Building** | Grow your home base from a Tent to a Citadel; each tier raises storage and houses a larger crew |
| 👷 | **Workers & Production** | Recruit NPC workers who gather resources on their own each day, fed on Food and scaled by the weather |
| 🔬 | **Research Track** | A five-tier knowledge ladder that permanently raises gather yields and cheapens travel |
| 💱 | **Trading Post** | A server-wide exchange: sell resources for Fame and buy them back, with scarcity-weighted prices |
| ⚔️ | **Player Raiding** | Challenge a rival's camp and carry off a share of their stores; newcomers still in a Tent are protected |
| 🏆 | **Fame Leaderboard** | Ranking is driven by what you discover and build, not by grinding combat |
| 📰 | **Daily News** | The chronicle of discoveries and builds greets every player on entry |
| 🌦️ | **Weather & Seasons** | Daily weather and a rotating season cycle that affect production |
| 🖥️ | **ANSI Interface** | 80×24, keyboard-driven, shaded block-art screens that run well over Telnet |
| 📝 | **Editable Text** | Every dynamic value is a named text variable, so sysops can reword any screen freely |
| 🛡️ | **Sysop Tools** | Hidden in-game player editor, Action Point control, and full game reset (SYSOP key only) |
| ⚙️ | **Highly Configurable** | 27 options cover world size, Action Points, regeneration, weather, workers, prices and raiding — every major system has an on/off switch |

---

## 🚀 Installation

### Requirements

- The Major BBS v10
- A free slot in the Menu Tree to add the module

### Install a release

1. Download the latest **[release](https://github.com/SysopNetwork/Fracture/releases)**
   zip and extract it.
2. With the BBS **shut down** (the host locks module DLLs while running), copy
   `FRACTURE.DLL`, `FRACTURE.MDF` and `FRACTURE.MSG` into your Major BBS v10
   directory.
3. Run the **Menu Tree Editor** and add **Fracture** to your menu.
4. Start the system and enter the module.

No data files ship with the release — Fracture creates its Btrieve files on first
run, and the host compiles `FRACTURE.MSG` to `FRACTURE.MCV` at startup.

### Upgrading

Shut the BBS down and copy the new `FRACTURE.DLL`, `FRACTURE.MDF` and
`FRACTURE.MSG` over the old ones. Your players, world and chronicle live in the
`FRAC*.DAT` files and are kept. If you have customized option values or message
text, re-apply them after replacing `FRACTURE.MSG`.

---

## ⚙️ Configuration

Every option lives in the standard MBBS configuration editor under
**Fracture: configurable options**. Full details are in [TECHNICAL.MD](TECHNICAL.MD).

### 🌍 World & Action Points

<table>
<tr>
<th width="150">Option</th>
<th>Description</th>
<th width="90">Default</th>
<th width="90">Range</th>
</tr>
<tr><td><code>WELCOPT</code></td><td>Greet players at logon</td><td><code>YES</code></td><td>Y/N</td></tr>
<tr><td><code>DAYAP</code></td><td>Action Points granted per player per day</td><td><code>20</code></td><td>1–200</td></tr>
<tr><td><code>APFREE</code></td><td>Refill Action Points on every entry (casual/test mode)</td><td><code>NO</code></td><td>Y/N</td></tr>
<tr><td><code>WLDSIZ</code></td><td>World size, tiles per edge</td><td><code>40</code></td><td>8–64</td></tr>
<tr><td><code>REGENPCT</code></td><td>Unexplored tiles regenerated each midnight (%) — <code>0</code> freezes the world</td><td><code>5</code></td><td>0–100</td></tr>
<tr><td><code>TRAVAP</code></td><td>Base Action Points to travel one tile</td><td><code>1</code></td><td>1–20</td></tr>
<tr><td><code>SRCHAP</code></td><td>Action Points to search a tile</td><td><code>1</code></td><td>1–20</td></tr>
<tr><td><code>FAMEDISC</code></td><td>Greatest Fame for a first discovery (the roll's floor is 40% of it)</td><td><code>20</code></td><td>1–500</td></tr>
<tr><td><code>FAMEBLD</code></td><td>Fame per camp tier reached when building</td><td><code>10</code></td><td>0–500</td></tr>
</table>

### 🌦️ Weather & Seasons

<table>
<tr>
<th width="150">Option</th>
<th>Description</th>
<th width="90">Default</th>
<th width="90">Range</th>
</tr>
<tr><td><code>WXOPT</code></td><td>Enable weather and seasons</td><td><code>YES</code></td><td>Y/N</td></tr>
<tr><td><code>SEASLEN</code></td><td>Days per season</td><td><code>7</code></td><td>1–90</td></tr>
</table>

### 👷 Workers

<table>
<tr>
<th width="150">Option</th>
<th>Description</th>
<th width="90">Default</th>
<th width="90">Range</th>
</tr>
<tr><td><code>WRKOPT</code></td><td>Enable workers and production</td><td><code>YES</code></td><td>Y/N</td></tr>
<tr><td><code>WRKPROD</code></td><td>Resource units produced per worker per day</td><td><code>2</code></td><td>0–50</td></tr>
<tr><td><code>WRKFOOD</code></td><td>Food eaten per worker per day</td><td><code>1</code></td><td>0–50</td></tr>
<tr><td><code>WRKCWOOD</code></td><td>Wood cost to recruit a worker</td><td><code>8</code></td><td>0–200</td></tr>
<tr><td><code>WRKCFOOD</code></td><td>Food cost to recruit a worker</td><td><code>4</code></td><td>0–200</td></tr>
</table>

### 💱 Trading Post & Research

<table>
<tr>
<th width="150">Option</th>
<th>Description</th>
<th width="90">Default</th>
<th width="90">Range</th>
</tr>
<tr><td><code>MKTOPT</code></td><td>Enable the trading post</td><td><code>YES</code></td><td>Y/N</td></tr>
<tr><td><code>MKTSELL</code></td><td>Fame earned per resource unit sold</td><td><code>2</code></td><td>0–100</td></tr>
<tr><td><code>MKTBUY</code></td><td>Fame charged per resource unit bought</td><td><code>3</code></td><td>0–100</td></tr>
<tr><td><code>RSCHOPT</code></td><td>Enable the research track</td><td><code>YES</code></td><td>Y/N</td></tr>
</table>

### ⚔️ Raiding

<table>
<tr>
<th width="150">Option</th>
<th>Description</th>
<th width="90">Default</th>
<th width="90">Range</th>
</tr>
<tr><td><code>RADOPT</code></td><td>Enable player raiding</td><td><code>YES</code></td><td>Y/N</td></tr>
<tr><td><code>RADCOST</code></td><td>Action Points per raid</td><td><code>5</code></td><td>0–200</td></tr>
<tr><td><code>RADLOOT</code></td><td>Percent of resources looted on a winning raid</td><td><code>25</code></td><td>0–100</td></tr>
<tr><td><code>RADFAME</code></td><td>Fame for winning a raid</td><td><code>5</code></td><td>0–100</td></tr>
<tr><td><code>RADMIN</code></td><td>Camp tier required to raid or be raided (0 Tent … 5 Citadel)</td><td><code>1</code></td><td>0–5</td></tr>
</table>

### 📰 News & Leaderboard

<table>
<tr>
<th width="150">Option</th>
<th>Description</th>
<th width="90">Default</th>
<th width="90">Range</th>
</tr>
<tr><td><code>NEWSMAX</code></td><td>Chronicle entries the news view shows</td><td><code>12</code></td><td>1–20</td></tr>
<tr><td><code>BRDMAX</code></td><td>Rows on the Fame leaderboard</td><td><code>15</code></td><td>3–20</td></tr>
</table>

### 📝 Message text

The screens live in `FRACTURE.MSG` under **Fracture: module text** and can be
reworded in the configuration editor. Dynamic values appear as named text
variables (`FRA_FAME`, `FRA_TARGET`, `FRA_WX`, …) that may be moved, repeated or
dropped freely — the module never depends on their order.

---

## 🛡️ Sysop Tools

A hidden `!` command at the main prompt is available only to users holding the BBS
**SYSOP** key, and is invisible to everyone else:

| Command | Action |
|---|---|
| `! find <name>` | Inspect a player record |
| `! set <name> <field> <value>` | Edit a player's fields |
| `! ap <name\|ALL> <n>` | Set Action Points for one player or everyone |
| `! wipe <name> YES` | Delete a player |
| `! reset CONFIRM` | Wipe all players, regenerate the world and clear the news |

Destructive commands require their literal confirmation token.

---

## 🔨 Building from Source

Fracture is a Win32 (x86) C DLL built with Visual Studio 2022 (toolset v143)
against The Major BBS v10 Module SDK. Copy `MBBSSDK.props.example` to
`MBBSSDK.props`, point it at your SDK, then:

```
MSBuild.exe Fracture.sln -p:Configuration=Release -p:Platform=Win32
```

> **x86 only — never x64.** The Major BBS v10 host is 32-bit and will not load a
> 64-bit module.

See **[BUILDING.MD](BUILDING.MD)** for prerequisites, packaging and deployment.

---

## 📁 Architecture

```
Fracture/
├── SRC/FRACTURE/
│   ├── FRACTURE.C/.H      # Module lifecycle, menu dispatch, command parsing, options
│   ├── PLAYER.C/.H        # Player records, daily Action Point reset, leaderboard
│   ├── WORLD.C/.H         # Tile world: generation, travel, charting, minimap, regeneration
│   ├── RESOURCE.C/.H      # The five materials and per-terrain gather yields
│   ├── CAMP.C/.H          # Camp tiers, storage caps, upgrades
│   ├── HISTORY.C/.H       # Server chronicle and the daily news view
│   ├── WEATHER.C/.H       # Weather conditions and the season cycle
│   ├── WORKER.C/.H        # NPC worker crews and passive production
│   ├── MARKET.C/.H        # Trading post (resources <-> Fame)
│   ├── RESEARCH.C/.H      # Five-tier research track
│   ├── RAID.C/.H          # Player raiding
│   ├── SYSOP.C/.H         # Hidden in-game sysop tools
│   ├── TVARS.C/.H         # Named text variables (FRA_*)
│   ├── FRACTURE.MSG       # Message/option source (generated by TOOLS/GENMSG.PY)
│   ├── FRACTURE.MDF       # Module definition file
│   └── FRACTURE.rc        # Windows version resource
├── TOOLS/
│   ├── GENMSG.PY          # Generates FRACTURE.MSG (block-art screens, CP437)
│   ├── BBSTEST.PY         # Telnet smoke test against a running BBS
│   ├── BBSFUZZ.PY         # Parser fuzzing and multi-account raid test
│   ├── STAT.PY            # Read-only status dump for the test accounts
│   └── BBSTEST.INI.EXAMPLE
├── DIST/1.0.0/            # Deployable release files
├── DOCS/ADR/              # Architecture decision records
├── TECHNICAL.MD           # Technical reference
├── BUILDING.MD            # Build, package and deploy guide
├── CHANGELOG.MD           # Release notes
├── MBBSSDK.props.example  # SDK path template
└── Fracture.sln
```

Game state is stored in Btrieve files through the DFA API, one per system:
`FRACTURE.DAT` (players), `FRACWLD.DAT` (world tiles), `FRACHIST.DAT`
(chronicle), `FRACWX.DAT` (weather) and `FRACWRK.DAT` (workers).

---

## 🏆 Credits

Fracture is developed and maintained by [Mark Laudenbach](https://github.com/laudenbachm) at [Sysop Network](https://github.com/SysopNetwork).

---

## 💖 Tips / Donations

- **Cash App:** [$laudenbachm](https://cash.app/$laudenbachm)
- **Ko-fi:** [ko-fi.com/laudenbachm](https://ko-fi.com/laudenbachm)

---

## 📄 License

MIT — Copyright (c) 2026 Sysop Network
