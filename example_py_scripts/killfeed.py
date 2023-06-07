import ppnix
import player
import ent
import event

LAYOUT_HUD = 0
LAYOUT_PAGE = 1

def lenIgnoreColor(name):
	count = 0
	for x in range(len(name)):
		if ord(name[x]) >= 32:
			count += 1

	return count

def stripColor(name):
	nonColorName = ""
	for char in name:
		if ord(char) >= 32:
			nonColorName += char
	return nonColorName

@event.draw_killfeed
def custom_killfeed(forClient,killStack):
	# print("Trying to draw kill-feed")
	# print(len(killStack))
	# print(killStack)

	# top right icon scaled inwards by ratio of screen width.
	# so catered for 1080p resolution.
	# -320 -> 320
	# -240 -> 240
	
	startY = 120 - 32*len(killStack)//2;
	# deathcard = 128x24 ... 8px centered text ... 24x24 mini-logos
	for death in killStack:
		victimSlot = ent.get_slot(death["victim"])
		killerSlot = ent.get_slot(death["killer"])

		killerName = "\x07" + stripColor(player.get_name(death["killer"]))
		victimName = "\x1F" + stripColor(player.get_name(death["victim"]))

		mod = death["mod"]
		# print(f" MOD = {mod} // victim = {victimSlot} // killer = {victimSlot}" )

		# img
		# player.draw_direct(LAYOUT_HUD,None,f"xr -256 yv {startY} picn c/k ")

		# RIGHT TO LEFT.
		# headshot icon
		spaceBetweenNames = 11
		if death["headshot"]:
			headshotSpace = 24
			player.draw_direct(LAYOUT_HUD,forClient,f"xr {0 - lenIgnoreColor(victimName)*8 - headshotSpace+4} yv {startY+4} picn c/hs ")
			spaceBetweenNames += headshotSpace//8

		# gun icon
		player.draw_direct(LAYOUT_HUD,forClient,f"xr {0 - lenIgnoreColor(victimName)*8 - spaceBetweenNames*8 - 8} yv {startY+4} picn c/sg ")

		# name max width = 16 * 8 = 128
		# Draw names with spaces between them ( for images )
		killLine = killerName + " "*spaceBetweenNames + victimName
		player.draw_direct(LAYOUT_HUD,forClient,f"xr {0 - lenIgnoreColor(victimName)*8 - lenIgnoreColor(killerName)*8 - spaceBetweenNames*8} yv {startY+8} string \"\x01{killLine}\"")
		
		startY += 24 + 8
	
	# player.draw_img_at(LAYOUT_PAGE,who,startX+256,startY,"c/c")
	# player.draw_text_at(LAYOUT_PAGE,who,startX,startY,line)

