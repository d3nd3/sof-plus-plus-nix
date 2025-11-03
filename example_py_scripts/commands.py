import ppnix
import player
import ent
import event

import math
from pages import cmd_page_chat,cmd_page_scoreboard

def VectorLengthFlat(v):
	length = v[0]**2 + v[1]**2
	length = math.sqrt(length)
	return length
def VectorLength(v):
	length = sum(component**2 for component in v)
	length = math.sqrt(length)
	return length



def cmd_clear_text(who):
	player.clear_text(who)
def cmd_draw_text_at(who,x,y,msg):
	player.draw_text_at(who,int(x),int(y),msg)
def cmd_draw_img_at(who,x,y,img_path):
	player.draw_img_at(who,int(x),int(y),img_path)
def cmd_draw_direct(who,msg):
	player.draw_direct(who,msg)

def cmd_draw_credit(who,msg):
	player.draw_credit(who,msg)
def cmd_draw_typeamatic(who,msg):
	player.draw_typeamatic(who,msg)
def cmd_draw_centered(who,msg):
	player.draw_centered(who,msg)
def cmd_draw_lower(who,msg):
	player.draw_lower(who,msg)
def cmd_con_print(who,msg):
	player.con_print(who,msg)

"""
 # CC FPS Ping Score PPM FRG DIE SK FLG REC Name
-- -- --- ---- ----- --- --- --- -- --- --- ---------------
 0 gb 142   19     0   0   0   0  0   0   0 Dende

Seems .players was heavily inspired from:
	CTFStats() @g_ctf.c
	It sets frags and deaths in player_die().
	Disconnect - reset - FRAGS/DEATHS
	PutClientInServer - reset - FRAGS/DEATHS
	G_RunFrame - FPS + PPM
	CTFtouch_HOOKS - FLG REC
	PutClientInServer DIE_CALLBACK - ent->die = player_die (PLAYERDIE_HOOK) - FRAGS/DEATHS

	5df2aC matches 1004F2A8 (flag capture)

There is GCLIENT->NETNAME and CLIENT->NAME. (CLIENT->NAME is larger 32 vs 16, perhaps color stripped?).

"""
def cmd_players(who):
	# player.con_print(who,"\x02Command disabled")
	player.con_print(who,f"\x04Players (fps max = 0; SoF-plus-plus-nix {ppnix.read_cvar('_++nix_version')})\n\x01 # CC FPS Ping Score PPM FRG DIE SK FLG REC Name\n-- -- --- ---- ----- --- --- --- -- --- --- ---------------")
	for slot in range(0,16):
		e = ent.from_slot(slot)

		if e is not None:
			stats = player.get_stats(e)
			# :02 :0 means padding for some-reason, without the 0 it doens't pad with 0's but spaces?
			player.con_print(
				who,
				(f'{slot:2} 00 '
				 f'{stats["fps"]:3} 0000 00000 000 '
				 f'{stats["frags"]:3} '
				 f'{stats["deaths"]:3} '
				 f'{stats["suicides"]:2} '
				 f'000 000 '
				 f'{e["gclient"]["name"]}')
			)

		# slot
		# CC
		# fps
		# e["ping"]
		# e["score"]
		# ppm
		# stats_FRAGS[slot]
		# stats_DEATHS[slot]
		# stats_SUICIDES[slot]
		# flag captures
		# flag recoveries
		# e["name"]
		# if e is not None:
		"""
		
		# ppnix.print(stats)
		print(stats)	
		player.con_print(
		    who,
		    (f'player {x}\n'
		     f'headshots : {stats["headshots"]}, '
		     f'throatshots : {stats["throatshots"]}, '
		     f'nutshots : {stats["nutshots"]}, '
		     f'armorpickups : {stats["armorpickups"]}\n')
		)
		"""
		



def cmd_get_time(who):
	player.con_print( who , f"Time: {ppnix.get_time()}" )

def cmd_refresh(who):
	player.refresh_layout(who)

# Speed show
speed_toggle = [False] * 32
@event.frame_early
def speed_frame():
	for i in range(0,32):
		who = ent.from_slot(i)
		if who is None:
			return
		if speed_toggle[i]:
			v = VectorLengthFlat(who["velocity"])
			player.draw_centered(who,f"Speed : {round(v)}")

def cmd_show_speed(who):
	slot = who["skinnum"]
	speed_toggle[slot] = not speed_toggle[slot]

@event.disconnect
def speed_disc(who):
	slot = who["skinnum"]
	speed_toggle[slot] = False

@event.say
def msg(who,msg):
	# ppnix.print(msg[0])

	if msg[0][0] == ".":
		
		try:
			cb = cmd_dict[msg[0]]
			# try:
			if len(msg) == 1:
				cb(who)
			else:
				cb(who,*msg[1:])
			# except:
			# 	player.con_print(who,"\x02Incorrect call.")
		except KeyError:
			pass
	
	# player.draw_text_at(None,50,50,"test")


cmd_dict = {
	".players" : cmd_players,
	".clear_text" : cmd_clear_text,
	".draw_text_at" : cmd_draw_text_at,
	".draw_img_at" : cmd_draw_img_at,
	".draw_direct" : cmd_draw_direct,
	".draw_credit" : cmd_draw_credit,
	".draw_typeamatic" : cmd_draw_typeamatic,
	".draw_centered" : cmd_draw_centered,
	".draw_lower" : cmd_draw_lower,
	".con_print" : cmd_con_print,
	".speed" : cmd_show_speed,
	".get_time" :  cmd_get_time,
	".refresh" : cmd_refresh,
	".c" : cmd_page_chat,
	".s" : cmd_page_scoreboard
}