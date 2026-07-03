# -*- coding: utf-8 -*-
# ---------------------------------------------------------------------------
# BBSFUZZ.PY - adversarial / multi-account testing for Fracture.
#
# Two jobs:
#   1. fuzz   - hammer the command parser with malformed, boundary, and
#               oversized input and confirm the session (and the BBS) survive
#               each one. A command that never returns the "Choose:" prompt is
#               flagged as a hang or crash.
#   2. raid    - a real multi-account play-through: build two camps up to a
#               Cabin by exploring for Fame and buying materials, then raid one
#               with the other and confirm loot moved and the victim is told.
#
# Reuses the telnet/login machinery from BBSTEST.PY. Reads accounts from
# BBSTEST.LOCAL.INI (gitignored). Run:  python BBSFUZZ.PY [fuzz|raid|all]
# ---------------------------------------------------------------------------

import configparser
import contextlib
import os
import re
import sys
import time

import BBSTEST as bt

HERE = os.path.dirname(os.path.abspath(__file__))


def load_cfg():
    cfg = configparser.ConfigParser()
    cfg.read(os.path.join(HERE, "BBSTEST.LOCAL.INI"))
    return cfg


def acct(cfg, section):
    return cfg.get(section, "userid"), cfg.get(section, "password")


def open_game(host, port, uid, pw):
    ses = bt.Session(host, port)
    bt.do_login(ses, uid, pw)
    entry = bt.enter_fracture(ses)
    return ses, entry


def game_cmd(ses, text, timeout=15):
    """Send a command; return (screen_text, alive) where alive is False if the
    'Choose:' prompt never came back (a hang or a crash)."""
    mark = len(ses.text)
    try:
        ses.send(text)
    except Exception:
        return "", False
    hit = bt._await_choose(ses, text[:16], timeout=timeout, recover=True)
    ses.drain(0.4)
    return ses.text[mark:], (hit == "Choose:")


def logoff(ses):
    try:
        ses.send("X"); ses.expect(["Make your selection", "to exit):"], timeout=8)
        ses.send("X")
        if ses.expect(["(Y/N)", "sure", "y/n", "gain"], timeout=5, answer_pauses=False):
            ses.send("Y")
        ses.drain(1.0)
    except Exception:
        pass
    ses.close()


@contextlib.contextmanager
def session(host, port, uid, pw):
    """Open a game session and ALWAYS log off, even if the body raises -- so a
    crash never leaves a player stuck online (which then blocks re-login)."""
    ses, entry = open_game(host, port, uid, pw)
    try:
        yield ses, entry
    finally:
        logoff(ses)


# ------------------------------------------------------------------ fuzz ----

FUZZ = [
    # empty / whitespace / unknown
    "", " ", "   ", "Z", "zzz", "!!!", "12345", "?", "??",
    # market: missing args, bad sides, bad resource selectors, bad quantities
    "M", "M ", "M S", "M B", "M S 1", "M X 1 1", "M S 0 1", "M S 6 1",
    "M S 9 1", "M S 1 0", "M S 1 -1", "M S 1 -999", "M S 1 abc", "M S x y",
    "M S 1 999999999", "M S 1 " + "9" * 40, "M B 3 999999999",
    "M B 0 1", "M B 6 1", "M B 3 -5",
    # research: bad/extra args
    "K", "K R", "K X", "K 999", "K R R R",
    # raid: self, nonexistent, oversized name, punctuation, injection-ish
    "A", "A Fracture", "A NobodyHere", "A " + "Z" * 200, "A ;drop table",
    "A @#$%^&*()", "A \" OR 1=1", "A a b c d e",
    # workers: bad selectors
    "P", "P 0", "P 6", "P 9", "P abc", "P 1 2 3 4 5",
    # camp upgrade spam, look/status/news repeats
    "U", "U U U", "C", "L", "H", "T",
    # oversized / many-token line
    "M S 1 1 " + "x " * 40, "N S E W R L C P M K A H T " * 2,
]


def run_fuzz(cfg):
    host, port = cfg.get("bbs", "host"), cfg.getint("bbs", "port")
    uid, pw = acct(cfg, "bbs")
    print("[fuzz] %s as %s -- %d adversarial inputs" % (host, uid, len(FUZZ)))
    ses, _ = open_game(host, port, uid, pw)

    broke = []
    for i, cmd in enumerate(FUZZ):
        screen, alive = game_cmd(ses, cmd, timeout=12)
        shown = (cmd[:34] + "...") if len(cmd) > 37 else cmd
        if not alive:
            print("  [%2d] %-40s -> NO PROMPT (hang/crash?)" % (i, repr(shown)))
            broke.append(cmd)
            # try to resynchronize; if the BBS died this will fail and we stop
            try:
                ses.send("");
                if bt._await_choose(ses, "resync", timeout=8) != "Choose:":
                    print("       session unrecoverable - stopping fuzz")
                    break
            except Exception:
                print("       connection lost - stopping fuzz")
                break
        else:
            print("  [%2d] %-40s -> ok" % (i, repr(shown)))
    logoff(ses)
    print("[fuzz] done. %d input(s) failed to return a prompt." % len(broke))
    return broke


# ------------------------------------------------------------------ raid ----

def parse_status(txt):
    def num(pat):
        m = re.search(pat, txt)
        return int(m.group(1)) if m else None
    st = {"fame": num(r"Fame\s*:\s*(\d+)"),
          "ap": num(r"Action Pts\s*:\s*(\d+)")}
    m = re.search(r"Camp\s*:\s*([A-Za-z]+)", txt)
    st["camp"] = m.group(1) if m else None
    m = re.search(r"RESOURCES\s+Wood\s+(\d+)\s+Stone\s+(\d+)\s+Metal\s+(\d+)"
                  r"\s+Food\s+(\d+)\s+Water\s+(\d+)", txt)
    st["res"] = ({"Wood": int(m.group(1)), "Stone": int(m.group(2)),
                  "Metal": int(m.group(3)), "Food": int(m.group(4)),
                  "Water": int(m.group(5))} if m else None)
    return st


def status(ses):
    txt, _ = game_cmd(ses, "T")
    return parse_status(txt)


def ensure_fame(ses, target, max_iters=60):
    st = status(ses)
    for i in range(max_iters):
        if (st.get("fame") or 0) >= target:
            return st
        game_cmd(ses, "R")            # search current tile -> discovery Fame + loot
        game_cmd(ses, "E")            # move into fresh ground
        if i % 4 == 3:
            st = status(ses)
    return status(ses)


def cash_up(ses):
    """Sell surplus non-build resources (Metal/Food/Water) to raise Fame."""
    res = status(ses).get("res") or {}
    for rnum, rname in ((3, "Metal"), (4, "Food"), (5, "Water")):
        if res.get(rname, 0) > 0:
            game_cmd(ses, "M S %d %d" % (rnum, res[rname]))


def build_cabin(ses):
    """Reach a Cabin: sell surplus for Fame, buy the 30 Wood / 10 Stone
    shortfall, upgrade. Retries so a Fame-capped buy does not strand us."""
    for _ in range(3):
        st = status(ses)
        if st.get("camp") and st["camp"] != "Tent":
            return st                 # already built up
        res = st.get("res") or {}
        if (st.get("fame") or 0) < 130:
            cash_up(ses)
        if res.get("Wood", 0) < 30:
            game_cmd(ses, "M B 1 %d" % (30 - res.get("Wood", 0)))
        if res.get("Stone", 0) < 10:
            game_cmd(ses, "M B 2 %d" % (10 - res.get("Stone", 0)))
        game_cmd(ses, "U")            # upgrade Tent -> Cabin
    return status(ses)


def stock_loot(ses):
    """After reaching a Cabin, build a resource pile to be looted: buy what
    Fame allows, then gather a few tiles (AP-funded) so there is always loot."""
    fame = status(ses).get("fame") or 0
    qty = min(40, max(0, (fame - 10) // 3))   # Wood costs ~3 Fame; keep a little
    if qty > 0:
        game_cmd(ses, "M B 1 %d" % qty)
    for _ in range(12):
        game_cmd(ses, "R")            # gather regardless of Fame
        game_cmd(ses, "E")
    return status(ses)


def run_raid(cfg):
    host, port = cfg.get("bbs", "host"), cfg.getint("bbs", "port")
    ha, hp = acct(cfg, "himmey")
    ka, kp = acct(cfg, "kledus")
    results = []

    def note(name, ok, extra=""):
        extra = extra.encode("ascii", "replace").decode()   # console-safe
        results.append((name, bool(ok), extra))
        print("  %-46s %s %s" % (name, "PASS" if ok else "FAIL", extra))

    # 1. attacker builds to a Cabin (market-funded; explores only if broke)
    print("[raid] building attacker %s ..." % ha)
    with session(host, port, ha, hp) as (ses, _):
        st = build_cabin(ses)
        if st.get("camp") in (None, "Tent"):   # too poor to buy materials
            ensure_fame(ses, 140); st = build_cabin(ses)
        note("attacker reached a Cabin+", st.get("camp") not in (None, "Tent"),
             "(camp=%s)" % st.get("camp"))

    # 2. defender builds to a Cabin and stocks resources to be looted
    print("[raid] building defender %s ..." % ka)
    with session(host, port, ka, kp) as (ses, _):
        st = build_cabin(ses)
        if st.get("camp") in (None, "Tent"):
            ensure_fame(ses, 140); build_cabin(ses)
        stock_loot(ses)               # give the raider something worth taking
        dst = status(ses)
        def_before = dst.get("res")
        note("defender reached a Cabin+", dst.get("camp") not in (None, "Tent"),
             "(camp=%s)" % dst.get("camp"))
        note("defender holds loot", bool(def_before) and sum(def_before.values()) > 0,
             "(res=%s)" % def_before)

    # 3. attacker raids the defender until a win (the clash is a die roll)
    print("[raid] %s raids %s ..." % (ha, ka))
    loot, outcome = None, None
    atk_before = atk_after = {}
    with session(host, port, ha, hp) as (ses, _):
        atk_before = status(ses).get("res") or {}
        for attempt in range(12):
            r, alive = game_cmd(ses, "A %s" % ka)
            if not alive:
                outcome = "SESSION DIED"; break
            if "overrun" in r:
                m = re.search(r"haul off\s+(\d+)\s+resources", r)
                loot = int(m.group(1)) if m else 0
                outcome = "win"; break
            if "driven off" in r:
                outcome = "repel"; continue
            outcome = "other: " + re.sub(r"\s+", " ", r)[:70]; break
        atk_after = status(ses).get("res") or {}
    note("raid produced a win", outcome == "win", "(outcome=%s)" % outcome)
    if outcome == "win":
        gain = sum(atk_after.values()) - sum(atk_before.values())
        note("attacker gained resources", gain > 0,
             "(reported loot=%s, net gain=%d)" % (loot, gain))

    # 4. defender logs back in: RAIDED notice + resources dropped
    print("[raid] defender %s checks in ..." % ka)
    with session(host, port, ka, kp) as (ses, entry):
        def_after = status(ses).get("res")
    note("defender saw the RAIDED notice", "RAIDED" in entry)
    if def_before and def_after:
        lost = sum(def_before.values()) - sum(def_after.values())
        note("defender lost resources", lost > 0, "(lost=%d)" % lost)
    return results


# ------------------------------------------------------------------ main ----

def main():
    try:
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass
    mode = sys.argv[1] if len(sys.argv) > 1 else "all"
    cfg = load_cfg()
    broke, raid_results = [], []
    if mode in ("fuzz", "all"):
        broke = run_fuzz(cfg)
    if mode in ("raid", "all"):
        print()
        raid_results = run_raid(cfg)

    print("\n===== SUMMARY =====")
    print("fuzz: %d input(s) failed to return a prompt%s" %
          (len(broke), (" -> " + ", ".join(repr(b[:20]) for b in broke)) if broke else ""))
    if bt.ESCAPES:
        print("escapes: session fell out to the BBS menu on -> %s"
              % ", ".join(repr(e) for e in bt.ESCAPES))
    if raid_results:
        passed = sum(1 for _, ok, _ in raid_results if ok)
        print("raid: %d/%d checks passed" % (passed, len(raid_results)))
    # exit non-zero if anything looks wrong
    bad = len(broke) + sum(1 for _, ok, _ in raid_results if not ok)
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
