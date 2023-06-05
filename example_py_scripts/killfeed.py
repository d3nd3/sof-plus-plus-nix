import ppnix
import player
import ent
import event

LAYOUT_HUD = 0
LAYOUT_PAGE = 1

@event.draw_killfeed
def custom_killfeed(killStack):
	print("Trying to draw kill-feed")
	print(len(killStack))
	# print(killStack)

	# top right icon scaled inwards by ratio of screen width.
	# so catered for 1080p resolution.
	# -320 -> 320
	# -240 -> 240
	offsetX = 0
	startX = -256 - offsetX;
	startY = 120 - 32*len(killStack)//2;
	# deathcard = 128x24 ... 8px centered text ... 24x24 mini-logos
	for death in killStack:
		victimSlot = ent.get_slot(death["victim"])
		killerSlot = ent.get_slot(death["killer"])

		mod = death["mod"]
		print(f"Player MOD = {mod} // victim = {victimSlot} // killer = {victimSlot}" )

		# img
		player.draw_direct(LAYOUT_HUD,None,f"xr {startX} yv {startY} picn c/k ")

		killLine = "Yo moma killed yo moma"
		centerOffsetX = (256-8*len(killLine))//2
		# text
		player.draw_direct(LAYOUT_HUD,None,f"xr {startX+centerOffsetX} yv {startY+8} string \"\x01{killLine}\"")
		

		startY += 24 + 8
	
	# player.draw_img_at(LAYOUT_PAGE,who,startX+256,startY,"c/c")
	# player.draw_text_at(LAYOUT_PAGE,who,startX,startY,line)

