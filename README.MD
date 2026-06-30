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

- **Daily Action Point loop** — every meaningful action (travel, explore, harvest,
  build, research, trade, raid) spends Action Points, encouraging quick 5–10 minute
  sessions.
- **A world that evolves** — at midnight a configurable share of unexplored tiles is
  regenerated. Player camps and player-built structures are always preserved.
- **Exploration & first discoveries** — uncovering a location first earns permanent
  server history, Fame, and a server-wide announcement.
- **Camp building** — grow your home base from a Tent to a Citadel, unlocking storage,
  production, workers, and defense.
- **Research tree** — spend daily Research Points across multiple categories to unlock
  better gathering, new buildings, and future content.
- **Player-driven economy** — a shared marketplace for trading, auctions, gifts, mail,
  and contracts.
- **Fame instead of levels** — ranking is driven by what you discover and build, not
  by grinding combat.
- **Daily news, weather, events, missions, and leaderboards** that make the server
  feel alive and give it a history of its own.
- **ANSI interface** — 80x24, keyboard-driven, works in plain ASCII or with full color,
  and runs well over Telnet.
- **Highly configurable** — sysops can tune world size, Action Points, weather, PvP and
  raiding, marketplace tax, production rates, event frequency, and more.

## Installation

1. Copy the module DLL and the files from the release `DIST/<version>/` directory into
   your Major BBS v10 installation.
2. Run the Menu Tree Editor and add **Fracture** to your menu.
3. Restart the system and enter the module.

See [BUILDING.MD](BUILDING.MD) for instructions on building from source.

## Configuration

Fracture exposes its gameplay tunables to the sysop (world size, daily Action Points,
weather frequency, PvP/raiding toggles, marketplace tax, research speed, resource
production, event frequency, season length, and camp limits). Configuration details
are documented in [TECHNICAL.MD](TECHNICAL.MD).

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
