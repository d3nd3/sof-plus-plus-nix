"""
print_showcase.py

Demonstrates every text/HUD "print" mechanism exposed by the python API and
where each one lands on the client's screen. Drop this file next to the other
example_py_scripts and it will register a set of chat commands:

    .showcase        - fire every mechanism at once, each labelled with its
                        own name, so you can compare positioning side by side.
    .showcase_off     - stop the persistent HUD overlay (draw_text_at / draw_direct
                        need to be redrawn every frame - see note below).
    .show_text_at     - player.draw_text_at()   - explicit x,y pixel coordinates
    .show_direct      - player.draw_direct()    - raw layout string, no helper math
    .show_centered    - player.draw_centered()  - StringPackage CENTER_CUSTOM
    .show_lower       - player.draw_lower()     - StringPackage CENTER_LOWER_CUSTOM
    .show_credit      - player.draw_credit()    - StringPackage CREDIT_CUSTOM
    .show_typeamatic  - player.draw_typeamatic()- StringPackage CINEMATIC_CUSTOM
    .show_con         - player.con_print()      - classic console/chat print

WHY THE POSITIONING DIFFERS
============================
There are really two families of mechanism here:

1) Layout-buffer draws: draw_text_at / draw_img_at / draw_direct
   These append raw quake "layout string" tokens (e.g. `xv 160 yv 112 string "..."`)
   into a per-client buffer (see src/features/pages.cpp:append_layout_string).
   - You give explicit pixel offsets from screen center: x in [-320,320],
     y in [-240,240], (0,0) is the middle of a 640x480 reference screen.
   - The buffer is *wiped every server frame* (layout_clear), so anything drawn
     this way is only visible for a single frame unless you re-issue the draw
     call every frame (see event.frame_early in the persistent overlay below).
   - draw_direct skips the x/y helper math entirely and lets you write the raw
     `xv/yv/xl/xr/yt/yb ... string/picn ...` tokens yourself.
   - IMPORTANT: this buffer is only actually transmitted to the client while
     the STAT_LAYOUTS bit is set, which is the same flag that drives the
     scoreboard screen (see src/game_modes/dm_always.cpp:refreshScreen /
     runRefreshLayout). In practice that means .show_text_at / .show_direct /
     .showcase only become visible while you're holding the scoreboard key
     (default TAB) - they render as part of that screen, not as a free-floating
     always-on HUD.

2) StringPackage prints: draw_centered / draw_lower / draw_credit / draw_typeamatic
   These go through spackage_print_ref() (src/strip.cpp) using the reference
   strings registered in main.cpp's SOFREESP block (CENTER_CUSTOM,
   CENTER_LOWER_CUSTOM, CREDIT_CUSTOM, CINEMATIC_CUSTOM). Each reference has a
   fixed, hardcoded screen position/behaviour baked into the engine itself
   (per the Jedi Outcast SDK's qcommon/strip.cpp, which this engine's
   StringPackage system is derived from):
   - draw_centered   -> SP_FLAG_CENTERED (0x1)  : horizontally centered, but
                                                   vertically anchored toward
                                                   the BOTTOM of the screen
                                                   (svc_centerprint - despite
                                                   the name, not screen-middle).
   - draw_lower      -> SP_FLAG_CAPTIONED (0x4)
                         | SP_FLAG_ALWAYS_PRINT
                           (0x40)                : yellow subtitle/caption text,
                                                   bottom of screen. CAPTIONED
                                                   alone only shows when the
                                                   client has cl_subtitles 1;
                                                   ALWAYS_PRINT forces it to
                                                   show regardless - that's why
                                                   CENTER_LOWER_CUSTOM sets both.
   - draw_credit     -> SP_FLAG_CREDIT (0x8)     : rendered via SCR_FadePic,
                                                   the same fade-in/hold/fade-out
                                                   mechanism used for end-game
                                                   credits (a full credit line
                                                   is actually
                                                   "xpos;ypos;duration;fadein;
                                                   fadeout;r,g,b;filename" -
                                                   we only drive the text case).
   - draw_typeamatic -> SP_FLAG_TYPEAMATIC (0x2) : SCR_CinematicString(0,104,...) -
                                                   very near the bottom of the
                                                   screen (not center), typed
                                                   out character-by-character,
                                                   cinematic-subtitle style.
   You cannot move these - there's no x/y argument because the engine owns the
   placement, and in practice all four of these cluster toward the bottom of
   the screen rather than the middle. Fire-and-forget: they persist for their
   own built-in duration, you don't need to redraw them every frame.

3) con_print is the plain old console/chat-log print (orig_cprintf, PRINT_HIGH)
   - it doesn't touch the HUD/layout system at all, it just appends a line to
   the client's console/chat history in the top-left overlay.
"""
import ppnix
import player
import ent
import event

LAYOUT_HUD = 0
LAYOUT_PAGE = 1

# who is drawing the persistent overlay right now (per slot, bool)
showcase_active = [False] * 32


def _label(msg):
	# \x07 is the "yellow" color escape used elsewhere in this codebase (see pages.py)
	return "\x07" + msg


# ---------------------------------------------------------------------------
# 1) Layout-buffer draws - need x/y, need to be redrawn every frame to persist.
# ---------------------------------------------------------------------------

@event.frame_early
def showcase_frame():
	"""
	draw_text_at / draw_direct are wiped every server frame, so to keep them
	on screen continuously we have to re-append them every frame. This is the
	same pattern pages.py uses for its scoreboard/chat pages.
	"""
	for slot in range(0, 32):
		if not showcase_active[slot]:
			continue
		who = ent.from_slot(slot)
		if who is None:
			showcase_active[slot] = False
			continue

		# draw_text_at(mode, who, x, y, msg) - explicit pixel offset from center.
		# top-left-ish quadrant
		player.draw_text_at(LAYOUT_HUD, who, -300, -200, _label("draw_text_at(-300,-200)"))
		player.draw_text_at(LAYOUT_HUD, who, -40, 0, _label("draw_text_at(-40,0) <- screen center"))
		player.draw_text_at(LAYOUT_HUD, who, -300, 180, _label("draw_text_at(-300,180) bottom-left"))
		player.draw_text_at(LAYOUT_HUD, who, 100, -200, _label("draw_text_at(100,-200) top-right"))

		# draw_direct(mode, who, msg) - raw layout token, same coordinate system
		# manually. xl/xr = from left/right edge, yt/yb = from top/bottom edge.
		player.draw_direct(LAYOUT_HUD, who, 'xl 8 yb 8 string "draw_direct: xl 8 yb 8 (bottom-left corner)"')
		player.draw_direct(LAYOUT_HUD, who, 'xr 8 yt 8 string "draw_direct: xr 8 yt 8 (top-right corner)"')


def cmd_showcase_on(who):
	slot = ent.get_slot(who)
	showcase_active[slot] = True
	player.con_print(who, _label("Showcase HUD overlay armed. Hold your scoreboard key (TAB) to see it - draw_text_at/draw_direct only render on that screen. Type .showcase_off to stop."))


def cmd_showcase_off(who):
	slot = ent.get_slot(who)
	showcase_active[slot] = False
	player.clear_layout(LAYOUT_HUD, who)
	player.con_print(who, _label("Showcase HUD overlay OFF."))


# ---------------------------------------------------------------------------
# 2) StringPackage prints - fixed engine-owned position, fire-and-forget.
# ---------------------------------------------------------------------------

def cmd_show_text_at(who):
	player.draw_text_at(LAYOUT_HUD, who, -100, -50, _label("draw_text_at: one-shot, explicit x=-100 y=-50"))


def cmd_show_direct(who):
	player.draw_direct(LAYOUT_HUD, who, 'xv 160 yv 200 string "draw_direct: raw token xv 160 yv 200"')


def cmd_show_centered(who):
	player.draw_centered(who, "draw_centered: horizontally centered, near BOTTOM of screen (SP_FLAG_CENTERED)")


def cmd_show_lower(who):
	player.draw_lower(who, "draw_lower: yellow caption text, bottom of screen, always shown (SP_FLAG_CAPTIONED|ALWAYS_PRINT)")


def cmd_show_credit(who):
	player.draw_credit(who, "draw_credit: fades in/out via SCR_FadePic, end-credit style (SP_FLAG_CREDIT)")


def cmd_show_typeamatic(who):
	player.draw_typeamatic(who, "draw_typeamatic: very bottom of screen, typed out letter by letter (SP_FLAG_TYPEAMATIC)")


def cmd_show_con(who):
	player.con_print(who, "con_print: plain console/chat line, top-left, scrolls with chat history")


# ---------------------------------------------------------------------------
# Fire everything at once for a side-by-side comparison.
# ---------------------------------------------------------------------------

def cmd_showcase_all(who):
	player.con_print(who, _label("Firing every print mechanism at once - watch where each lands."))
	cmd_showcase_on(who)
	cmd_show_centered(who)
	cmd_show_lower(who)
	cmd_show_credit(who)
	cmd_show_typeamatic(who)
	cmd_show_con(who)


@event.disconnect
def showcase_disconnect(who):
	slot = ent.get_slot(who)
	showcase_active[slot] = False


@event.say
def msg(who, msg):
	if not msg[0].startswith("."):
		return
	try:
		cb = showcase_cmd_dict[msg[0]]
	except KeyError:
		return
	cb(who)


showcase_cmd_dict = {
	".showcase": cmd_showcase_all,
	".showcase_off": cmd_showcase_off,
	".show_text_at": cmd_show_text_at,
	".show_direct": cmd_show_direct,
	".show_centered": cmd_show_centered,
	".show_lower": cmd_show_lower,
	".show_credit": cmd_show_credit,
	".show_typeamatic": cmd_show_typeamatic,
	".show_con": cmd_show_con,
}
