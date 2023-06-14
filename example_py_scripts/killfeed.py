import ppnix
import player
import ent
import event

LAYOUT_HUD = 0
LAYOUT_PAGE = 1

MOD_C4 = 17
MOD_SHOTGUN = 9

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

# TODO: color name by ctf team.
@event.draw_killfeed
def custom_killfeed(forClient,killStack):
	# print("Trying to draw kill-feed")
	# print(len(killStack))
	# print(killStack)

	# top right icon scaled inwards by ratio of screen width.
	# so catered for 1080p resolution.
	# -320 -> 320
	# -240 -> 240
	
	# startY = 120 - 32*len(killStack)//2;
	startY = 32
	# deathcard = 128x24 ... 8px centered text ... 24x24 mini-logos
	for death in killStack:
		victimSlot = ent.get_slot(death["victim"])
		killerSlot = ent.get_slot(death["killer"])

		killerName = "\x07" + stripColor(player.get_name(death["killer"]))
		victimName = "\x1F" + stripColor(player.get_name(death["victim"]))

		mod = death["mod"]
		print(f" MOD = {mod} // victim = {victimSlot} // killer = {victimSlot}" )

		# img
		# player.draw_direct(LAYOUT_HUD,None,f"xr -256 yv {startY} picn c/k ")

		# Draw from right to left.
		
		if victimSlot == killerSlot:
			# print("Self kill")
			# Self Kill.
			# [Skull] [HOW] [victim]

			rightOffset = 0 - lenIgnoreColor(victimName)*8
			# Draw name
			killLine = victimName
			player.draw_direct(LAYOUT_HUD,forClient,f"xr {rightOffset} yt {startY+8} string \"{killLine}\"")
			
			if mod == MOD_C4:
				c4Space = 24
				rightOffset -= c4Space

				player.draw_direct(LAYOUT_HUD,forClient,f"xr {rightOffset+4} yt {startY+4} picn c/c4 ")
				
			# Skull & Crossbones
			suicideSpace = 24
			rightOffset -= suicideSpace
			player.draw_direct(LAYOUT_HUD,forClient,f"xr {rightOffset + 4} yt {startY+4} picn c/sb ")


			
		else:
			spaceCharsBetweenNames = 0
			# [killer] [GUN] [HOWSPECIAL] [victim]
			# print("Enemy kill")
			# RIGHT TO LEFT.
			# headshot icon
			rightOffset = 0 - lenIgnoreColor(victimName)*8
			
			if death["headshot"]:
				# [killer] [GUN] [HEADSHOT] [victim]
				# its a 16x16 img, but gave it extra space. 8px , 4 each side?
				headshotSpace = 24
				rightOffset -= headshotSpace
				player.draw_direct(LAYOUT_HUD,forClient,f"xr {rightOffset+4} yt {startY+4} picn c/hs ")
				spaceCharsBetweenNames += headshotSpace//8
				
			if mod == MOD_SHOTGUN:
				gunSpace = 88
				spaceCharsBetweenNames += gunSpace // 8
				rightOffset -= gunSpace
				# gun icon
				player.draw_direct(LAYOUT_HUD,forClient,f"xr {rightOffset } yt {startY-8} picn c/s1 ")
			elif mod == MOD_C4:
				c4Space = 24
				spaceCharsBetweenNames += c4Space // 8
				rightOffset -= c4Space
				player.draw_direct(LAYOUT_HUD,forClient,f"xr {rightOffset+4} yt {startY+4} picn c/c4 ")

			# name max width = 16 * 8 = 128
			# Draw names with spaces between them ( for images )
			killLine = killerName + " "*spaceCharsBetweenNames + victimName
			player.draw_direct(LAYOUT_HUD,forClient,f"xr {0 - lenIgnoreColor(victimName)*8 - lenIgnoreColor(killerName)*8 - spaceCharsBetweenNames*8} yt {startY+8} string \"{killLine}\"")
			
		startY += 24 + 8
	
	# player.draw_img_at(LAYOUT_PAGE,who,startX+256,startY,"c/c")
	# player.draw_text_at(LAYOUT_PAGE,who,startX,startY,line)

