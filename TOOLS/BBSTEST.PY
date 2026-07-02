# -*- coding: utf-8 -*-
# ---------------------------------------------------------------------------
# BBSTEST.PY - automated smoke test for the Fracture door module.
#
# Connects to a running Major BBS v10 over Telnet, logs in with a test
# account, enters the Fracture menu, drives a sequence of commands, and
# checks the captured output for the things a build should get right:
#   - every new screen renders (status, map, camp, crew, news)
#   - the weather / season line shows up
#   - the crew screen works
#   - the host's output more-prompt "(N)onstop,(Q)uit,(C)ontinue?" never
#     appears while inside the game (the nonstop fix)
#
# It reads its target and credentials from BBSTEST.LOCAL.INI (kept out of the
# repo); copy BBSTEST.INI.EXAMPLE to BBSTEST.LOCAL.INI and fill it in. The
# host address and account never live in this committed file.
#
# Usage:   python BBSTEST.PY [--config PATH] [--verbose] [--keep]
# Exit code 0 = all checks passed, 1 = one or more failed, 2 = setup error.
# ---------------------------------------------------------------------------

import argparse
import configparser
import os
import re
import socket
import sys
import time

# --- Telnet protocol bytes --------------------------------------------------
IAC, DONT, DO, WONT, WILL, SB, SE = 255, 254, 253, 252, 251, 250, 240
OPT_ECHO, OPT_SGA = 1, 3

ANSI_CSI = re.compile(r"\x1b\[[0-9;?]*[ -/]*[@-~]")
ANSI_2CH = re.compile(r"\x1b[()][A-Za-z0-9]")

# Prompts the BBS may throw up that we clear by pressing a key.
PAUSE_PROMPTS = ("(N)onstop", "Nonstop", "more...", "MORE", "press", "Press",
                 "continue", "Continue", "[C]", "hit a key", "any key")


def strip_ansi(text):
    text = ANSI_CSI.sub("", text)
    text = ANSI_2CH.sub("", text)
    # keep printable ASCII, CP437 high range, and newlines; blank the rest
    return "".join(c if (c == "\n" or c == "\t" or 32 <= ord(c) < 127 or ord(c) >= 128)
                   else " " for c in text)


class Session(object):
    """A telnet connection with an expect/send loop and a full transcript."""

    def __init__(self, host, port, verbose=False):
        self.sock = socket.socket()
        self.sock.settimeout(2.0)
        self.sock.connect((host, port))
        self.verbose = verbose
        self.raw = bytearray()       # every byte received (post-IAC)
        self.text = ""               # ANSI-stripped running transcript
        self._pending = bytearray()

    # -- low level ----------------------------------------------------------
    def _handle_iac(self, data):
        """Strip and answer telnet IAC negotiation; return the clean bytes.
        We agree to SGA (this BBS requires it) and let the server echo."""
        reply = bytearray()
        clean = bytearray()
        i = 0
        while i < len(data):
            b = data[i]
            if b == IAC and i + 1 < len(data):
                cmd = data[i + 1]
                if cmd in (DO, DONT) and i + 2 < len(data):
                    opt = data[i + 2]
                    reply += bytes([IAC, WILL, opt]) if opt == OPT_SGA \
                        else bytes([IAC, WONT, opt])
                    i += 3
                    continue
                if cmd in (WILL, WONT) and i + 2 < len(data):
                    opt = data[i + 2]
                    reply += bytes([IAC, DO, opt]) if opt in (OPT_SGA, OPT_ECHO) \
                        else bytes([IAC, DONT, opt])
                    i += 3
                    continue
                if cmd == SB:
                    j = i + 2
                    while j < len(data) and not (data[j] == IAC and j + 1 < len(data)
                                                 and data[j + 1] == SE):
                        j += 1
                    i = j + 2
                    continue
                i += 2
                continue
            clean.append(b)
            i += 1
        if reply:
            self.sock.sendall(bytes(reply))
        return bytes(clean)

    def _pump(self):
        """Read one chunk, feed the transcript. Return True if bytes arrived."""
        try:
            chunk = self.sock.recv(4096)
        except socket.timeout:
            return False
        if not chunk:
            raise EOFError("connection closed by BBS")
        clean = self._handle_iac(chunk)
        self.raw += clean
        piece = strip_ansi(clean.decode("cp437", "replace"))
        self.text += piece
        if self.verbose and piece.strip():
            sys.stderr.write(piece)
            sys.stderr.flush()
        return True

    # -- high level ---------------------------------------------------------
    def send(self, s, cr=True):
        payload = (s + ("\r" if cr else "")).encode("cp437", "replace")
        self.sock.sendall(payload)

    def expect(self, patterns, timeout=15, answer_pauses=True):
        """Read until one of `patterns` appears in newly-arrived text or we
        time out. Optionally auto-answer pause/more prompts with a CR while
        waiting. Returns the matched pattern or None on timeout."""
        if isinstance(patterns, str):
            patterns = [patterns]
        deadline = time.time() + timeout
        start = len(self.text)
        last_pause = ""
        while time.time() < deadline:
            got = self._pump()
            window = self.text[max(0, start - 40):]
            for p in patterns:
                if p in self.text[start:] or p in window:
                    return p
            if answer_pauses:
                tail = self.text[-60:]
                for pp in PAUSE_PROMPTS:
                    if pp in tail and tail != last_pause:
                        self.send("", cr=True)
                        last_pause = tail
                        break
            if not got:
                # idle; keep looping until the deadline
                pass
        return None

    def drain(self, seconds=2.0):
        """Absorb whatever is still coming for a short while."""
        deadline = time.time() + seconds
        while time.time() < deadline:
            self._pump()

    def close(self):
        try:
            self.sock.close()
        except Exception:
            pass


# --- the actual test steps --------------------------------------------------

def do_login(ses, userid, password):
    if not ses.expect("User-ID", timeout=20):
        raise RuntimeError("never saw the User-ID prompt")
    ses.send(userid)
    if not ses.expect(["assword", "PASSWORD"], timeout=15):
        raise RuntimeError("never saw the password prompt (bad user-id?)")
    ses.send(password)
    # Settle through the logon notices, who's-online list, broadcasts and
    # "(Press ENTER)" pauses until the real TOP-menu selection prompt appears.
    # Don't match bare "Fracture" - it shows up in the greeting and the who
    # list long before the menu is actually waiting for input.
    hit = ses.expect(["Make your selection", "to exit):"], timeout=45,
                     answer_pauses=True)
    ses.drain(1.5)
    if not hit:
        raise RuntimeError("did not reach the TOP menu prompt after login "
                           "(wrong password, or unexpected prompt)")
    return hit


PAUSE_SEEN = []          # labels of screens where the (N)onstop prompt fired


def _await_choose(ses, label, timeout=15):
    """Wait for Fracture's 'Choose:' prompt. If the host's more-prompt fires
    first, record it against `label` and answer 'C' to continue - so we both
    detect the regression and keep the session moving."""
    while True:
        hit = ses.expect(["Choose:", "(N)onstop", "Nonstop"], timeout=timeout,
                         answer_pauses=False)
        if hit in ("(N)onstop", "Nonstop"):
            if label not in PAUSE_SEEN:
                PAUSE_SEEN.append(label)
            ses.send("C")            # continue past the pause
            continue
        return hit                   # "Choose:" or None (timeout)


def enter_fracture(ses):
    mark = len(ses.text)
    ses.send("F")
    if not _await_choose(ses, "entry", timeout=20):
        raise RuntimeError("Fracture did not come up (no 'Choose:' prompt)")
    ses.drain(1.0)
    return ses.text[mark:]


def run_cmd(ses, cmd, label):
    """Send one Fracture command, capture the screen it draws."""
    mark = len(ses.text)
    ses.send(cmd)
    _await_choose(ses, label, timeout=15)
    ses.drain(0.8)
    return ses.text[mark:]


CHECKS = []


def check(name, ok, detail=""):
    CHECKS.append((name, bool(ok), detail))


def main():
    ap = argparse.ArgumentParser(description="Fracture BBS smoke test")
    ap.add_argument("--config", default=None, help="path to BBSTEST.LOCAL.INI")
    ap.add_argument("--verbose", action="store_true", help="stream the session to stderr")
    ap.add_argument("--keep", action="store_true", help="write transcripts to disk")
    args = ap.parse_args()

    here = os.path.dirname(os.path.abspath(__file__))
    cfg_path = args.config or os.path.join(here, "BBSTEST.LOCAL.INI")
    if not os.path.exists(cfg_path):
        sys.stderr.write("No config at %s\n"
                         "Copy BBSTEST.INI.EXAMPLE to BBSTEST.LOCAL.INI and fill it in.\n"
                         % cfg_path)
        return 2
    cfg = configparser.ConfigParser()
    cfg.read(cfg_path)
    host = cfg.get("bbs", "host", fallback=os.environ.get("BBS_HOST", ""))
    port = cfg.getint("bbs", "port", fallback=int(os.environ.get("BBS_PORT", "23")))
    userid = cfg.get("bbs", "userid", fallback=os.environ.get("BBS_USER", ""))
    password = cfg.get("bbs", "password", fallback=os.environ.get("BBS_PASS", ""))
    if not host or not userid:
        sys.stderr.write("config is missing host or userid\n")
        return 2

    print("Fracture smoke test -> %s:%d as %s" % (host, port, userid))
    ses = None
    fracture_log = ""
    try:
        ses = Session(host, port, verbose=args.verbose)
        do_login(ses, userid, password)
        print("  logged in, main menu reached")
        entry = enter_fracture(ses)
        fracture_log += entry
        print("  entered Fracture")

        # A bare Enter at the menu must redraw the prompt exactly once. The
        # short menu ends in "Choose:", so a healthy redraw shows exactly one;
        # two means the double-menu regression is back. Drain the full reply
        # rather than stopping at the first prompt so we'd catch a second.
        mark = len(ses.text)
        ses.send("")
        ses.drain(2.5)
        bare = ses.text[mark:]
        fracture_log += bare
        n_prompts = bare.count("Choose:")
        check("bare Enter redraws menu once", n_prompts == 1,
              "saw %d 'Choose:' prompts (double-menu regression)" % n_prompts)
        print("  bare Enter -> %d menu prompt(s)" % n_prompts)

        # Drive the new + core systems. Each returns the screen it drew.
        screens = {}
        for cmd, label in [("T", "status"), ("L", "look/map"), ("N", "move north"),
                           ("R", "search"), ("C", "camp"), ("P", "crew"),
                           ("M", "market"), ("M S 5 1", "sell"), ("H", "news"),
                           ("T", "status-2")]:
            screens[label] = run_cmd(ses, cmd, label)
            fracture_log += screens[label]
            print("  ran %-10s (%d chars)" % (cmd, len(screens[label])))

        # ---- assertions (only on output produced INSIDE Fracture) ----------
        # Run these BEFORE logging off: the logoff closes the connection, and
        # we don't want a normal disconnect to skip the real checks.
        blob = fracture_log
        check("entered game (menu shown)", "Choose:" in entry)
        check("weather/season line", ("Weather" in blob) and
              any(s in blob for s in ("Spring", "Summer", "Autumn", "Winter")))
        check("status screen", "EXPLORER STATUS" in blob or "Explorer" in screens.get("status", ""))
        check("map / look screen", ("LOCATION" in blob) and ("unexplored" in blob))
        check("camp screen", "CAMP" in screens.get("camp", "").upper())
        check("crew screen", "CREW" in screens.get("crew", "").upper())
        check("news chronicle", "CHRONICLE" in blob.upper() or "chronicle" in blob)
        mkt = screens.get("market", "")
        check("market screen", "TRADING POST" in mkt and "sell" in mkt and "buy" in mkt)
        check("market trade responds",
              any(t in screens.get("sell", "")
                  for t in ("Sold", "do not have", "valid trade", "full", "Fame")))
        check("NO (N)onstop pause inside game", not PAUSE_SEEN,
              "more-prompt fired on: %s (nonstop fix not deployed?)"
              % ", ".join(PAUSE_SEEN) if PAUSE_SEEN else "")

        if args.keep:
            sp = os.path.join(here, "BBSTEST.OUT")
            with open(sp + ".txt", "w", encoding="utf-8", errors="replace") as f:
                f.write(fracture_log)
            with open(sp + ".raw", "wb") as f:
                f.write(bytes(ses.raw))
            print("  transcripts written to %s.txt / .raw" % sp)

    except Exception as e:
        print("  ERROR: %s" % e)
        check("session completed", False, str(e))
    finally:
        # Best-effort clean logoff so we don't strand the account's channel on
        # a small-license test system. The BBS closes the socket on logoff, so
        # an EOF here is success, not a failure - swallow it.
        if ses:
            try:
                ses.send("X")          # exit Fracture -> TOP menu
                ses.expect(["Make your selection", "to exit):"], timeout=8)
                ses.send("X")          # exit system (logoff)
                if ses.expect(["(Y/N)", "sure", "Y/n", "y/n", "gain"], timeout=5,
                              answer_pauses=False):
                    ses.send("Y")
                ses.drain(1.5)
            except Exception:
                pass                   # disconnect on logoff is expected
            ses.close()

    # ---- report -----------------------------------------------------------
    print("\n%-40s  %s" % ("CHECK", "RESULT"))
    print("-" * 58)
    passed = 0
    for name, ok, detail in CHECKS:
        tag = "PASS" if ok else "FAIL"
        line = "%-40s  %s" % (name, tag)
        if detail and not ok:
            line += "  <- " + detail
        print(line)
        passed += ok
    print("-" * 58)
    print("%d/%d checks passed" % (passed, len(CHECKS)))
    return 0 if passed == len(CHECKS) and CHECKS else 1


if __name__ == "__main__":
    sys.exit(main())
