# Fracture

*A living world rebuilt every day. Compete to leave your mark before it vanishes.*

Fracture is a persistent, multiplayer door game for **The Major BBS v10**. It is a
world-exploration and survival-strategy game where the world itself is the primary
gameplay mechanic. Players log in daily, spend a limited pool of Action Points to
explore the map, build and upgrade a camp, gather resources, research technology,
and compete to become the most famous explorer on the server — in a world that
changes a little every midnight.

Fracture is designed for short, satisfying daily sessions: easy to learn, fun for a
single player, and better with a crowd.

## Features

- **Daily Action Point loop** — every meaningful action (travel, search, build,
  research, raid) spends Action Points, encouraging quick 5–10 minute sessions.
- **A world that evolves** — at midnight a configurable share of unexplored tiles is
  regenerated. Charted ground and player camps are always preserved.
- **Exploration & first discoveries** — charting a tile first earns Fame and a
  permanent entry in the server chronicle, surfaced to everyone as daily news.
- **Camp building** — grow your home base from a Tent to a Citadel; each tier raises
  resource storage and houses a larger worker crew.
- **Workers & passive production** — recruit a crew of NPC workers who gather
  resources on their own each day, fed on Food and scaled by the weather.
- **Research track** — a five-tier knowledge ladder bought with resources that
  permanently raises gather yields and cheapens travel.
- **Trading post** — a server-wide exchange for selling resources for Fame and
  buying them back, with scarcity-weighted prices.
- **Player raiding** — challenge a rival's camp and carry off a share of their
  stores; newcomers still in a Tent can neither raid nor be raided.
- **Fame instead of levels** — ranking is driven by what you discover and build, not
  by grinding combat, with a live leaderboard of the server's top explorers.
- **Daily news, weather, and seasons** that make the server feel alive and give the
  world a history of its own.
- **ANSI interface** — 80x24, keyboard-driven block-art screens that run well over
  Telnet.
- **Highly configurable** — sysops can tune world size, daily Action Points, world
  regeneration, weather and season length, worker production, trading prices, and
  raiding rules; every major system has an on/off switch.

## Installation

1. Copy the module DLL and the files from the release `DIST/<version>/` directory into
   your Major BBS v10 installation.
2. Run the Menu Tree Editor and add **Fracture** to your menu.
3. Restart the system and enter the module.

See [BUILDING.MD](BUILDING.MD) for instructions on building from source.

## Configuration

Fracture exposes its gameplay tunables as standard MBBS configuration options (world
size, daily Action Points, midnight regeneration, weather and season length, worker
production and upkeep, trading prices, raiding cost and loot, and on/off switches for
each major system). The full option list is documented in
[TECHNICAL.MD](TECHNICAL.MD).

## Usage

Enter the module from your BBS menu, then use the on-screen keyboard menus to explore,
build, research, and trade. Each login opens with the day's news so you can see what
has changed in the world since you were last online.

## Version

Fracture follows [Semantic Versioning](https://semver.org/). See
[CHANGELOG.MD](CHANGELOG.MD) for release history.

## License

Released under the [MIT License](LICENSE). Copyright © Sysop Network.

## Support

Fracture is developed and maintained by Mark Laudenbach at Sysop Network.
