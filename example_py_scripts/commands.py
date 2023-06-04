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
def cmd_draw_text_at(who,msg,x,y,gray):
	player.draw_text_at(who,int(x),int(y),msg,True if int(gray) else False)
def cmd_draw_img_at(who,img_path,x,y):
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

def cmd_players(who):
	# player.con_print(who,"\x02Command disabled")

	for x in range(0,16):
		e = ent.from_slot(x)
		if e is not None:
			stats = player.get_stats(e)
			# ppnix.print(stats)
			print(stats)	
			player.con_print(who,f'player {x}\nheadshots : {stats["headshots"]}, throatshots : {stats["throatshots"]}, nutshots : {stats["nutshots"]}, armorpickups : {stats["armorpickups"]}\n')


def cmd_get_time(who):
	player.con_print( who , f"Time: {ppnix.get_time()}" )

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
			try:
				if len(msg) == 1:
					cb(who)
				else:
					cb(who,*msg[1:])
			except:
				player.con_print(who,"\x02Incorrect call.")
		except KeyError:
			pass
	
	# player.draw_text_at(None,50,50,"test",False)


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
	".c" : cmd_page_chat,
	".s" : cmd_page_scoreboard
}