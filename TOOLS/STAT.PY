# Read-only status dump for the test accounts (costs no Action Points).
import sys, time, configparser
import BBSTEST as bt
import BBSFUZZ as bf

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
cfg = configparser.ConfigParser(); cfg.read("BBSTEST.LOCAL.INI")
host, port = cfg.get("bbs", "host"), cfg.getint("bbs", "port")

for section in ("himmey", "kledus", "mark"):
    uid, pw = cfg.get(section, "userid"), cfg.get(section, "password")
    for attempt in range(2):
        try:
            with bf.session(host, port, uid, pw) as (ses, _):
                st = bf.status(ses)
                print("%-8s AP=%s Fame=%s camp=%s res=%s" %
                      (uid, st.get("ap"), st.get("fame"), st.get("camp"), st.get("res")))
            break
        except Exception as e:
            if attempt == 0:
                time.sleep(20)          # let a ghost session clear, then retry
            else:
                print("%-8s <login failed: %s>" % (uid, e))
