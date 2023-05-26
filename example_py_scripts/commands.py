import math
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
def cmd_draw_typeamatic(who,msg):
	player.draw_typeamatic(who,msg)
def cmd_draw_centered(who,msg):
	player.draw_centered(who,msg)
def cmd_draw_lower(who,msg):
	player.draw_lower(who,msg)
def cmd_con_print(who,msg):
	player.con_print(who,msg)

def cmd_players(who):
	player.con_print(who,"\x02Command disabled")

# Speed show
speed_toggle = [False] * 32
@event.frame_early
def speed_frame():
	for i in range(0,32):

		if speed_toggle[i]:
			who = ent.from_slot(i)
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
	ppnix.print(msg)
	args = msg.split(" ")
	cmd_dict = {
		".players" : cmd_players,
		".clear_text" : cmd_clear_text,
		".draw_text_at" : cmd_draw_text_at,
		".draw_typeamatic" : cmd_draw_typeamatic,
		".draw_centered" : cmd_draw_centered,
		".draw_lower" : cmd_draw_lower,
		".con_print" : cmd_con_print,
		".speed" : cmd_show_speed
	}
	try:
		cb = cmd_dict[args[0]]
		try:
			if len(args) == 1:
				cb(who)
			else:
				cb(who,*args[1:])
		except:
			player.con_print(who,"\x02Incorrect call.")
	except KeyError:
		pass
	
