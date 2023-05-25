def cmd_draw_text_at_gray(who,msg):
	player.draw_text_at(who,50,50,msg,True)
def cmd_draw_text_at_white(who,msg):
	player.draw_text_at(who,50,50,msg,False)
def cmd_draw_typeamatic(who,msg):
	player.draw_typeamatic(who,msg)
def cmd_draw_centered(who,msg):
	player.draw_centered(who,msg)
def cmd_draw_lower(who,msg):
	player.draw_lower(who,msg)
def cmd_con_print(who,msg):
	player.con_print(who,msg)

def cmd_players(who,msg):
	player.con_print(who,"\x02Command disabled\n")
@event.say
def msg(who,msg):
	ppnix.print(msg)
	args = msg.split(" ")
	cmd_dict = {
		".players" : cmd_players,
		".draw_text_at_gray" : cmd_draw_text_at_gray,
		".draw_text_at_white" : cmd_draw_text_at_white,
		".draw_typeamatic" : cmd_draw_typeamatic,
		".draw_centered" : cmd_draw_centered,
		".draw_lower" : cmd_draw_lower,
		".con_print" : cmd_con_print
	}
	try:
		cmd_dict[args[0]](who,''.join(args[1:]))
	except KeyError:
		pass
