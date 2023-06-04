import ppnix
import player
import ent
import event


# protects the @event from being fired when imported.
if __name__ == "builtins":
	@event.map_spawn
	def pages_begin():
		ppnix.print("pages begin...")
		for i in range(0,32):
			player.set_page(i,"scoreboard")

	@event.frame_early
	def think():
		now = ppnix.get_time()


	# -320 -> 320
	# -240 -> 240
	@event.draw_page("scoreboard")
	def draw_scoreboard(who,killer):
		# ppnix.print("Scoreboard!")

		info = "\x07---> Hotswitches type: .c .p <---"
		print(len(info))
		player.draw_text_at(who,0-len(info)*8//2,-180,info,False)
		player.orig_scoreboard(who,killer);



	@event.draw_page("chat")
	def draw_chat(who,killer):
		# ppnix.print("Chat!")

		chatLines = 7
		gapY = 32
		widthX = 60
		# 4 lines for console print(4x8), 16 pixel alignment away from crosshair
		startY = -232 + 32+16-8

		# center text
		widthPixels = widthX*8
		startX = -320 + ((640 - 512) // 2)
		# 80 characters per line.

		player.draw_img_at(who,startX,startY,"c/c")
		player.draw_img_at(who,startX+256,startY,"c/c")

		# horizontal - 512 background
		# vertical - 32 * 8 = 256

		# 24 pixel gap at bottom
		startY += 12
		# draw help here.
		info = "\x07---> Hotswitches type: .s .p <---"
		player.draw_text_at(who,startX+widthPixels//2-len(info)*8//2,startY,info,False)

		startY += 32
		# line len can't be greater than 64 or escapes backgrnd.
		# startX += (512 - line_len*8)*0.5;
		startX += 4
		# Colored names consume space in buffer
		chat_lines = ppnix.get_chat_vectors(chatLines)
		# print(chat_lines)
		for line in chat_lines:
			# offsetx+157,offsety+114
			# 23 byte overhead per line
			# 8 lines.
			# (60 + 23) * 8 = 664
			player.draw_text_at(who,startX,startY,line,False)
			startY += 32


def cmd_page_chat(who):
	player.set_page(ent.get_slot(who),"chat")
	player.refresh_layout(who)

def cmd_page_scoreboard(who):
	player.set_page(ent.get_slot(who),"scoreboard")
	player.refresh_layout(who)