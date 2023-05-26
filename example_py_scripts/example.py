# player.draw_text_at(100,100,"Edit!")
# player.draw_typeamatic(text)
# player.draw_centered(text)
# player.draw_centered_lower(text)
# player.con_print(ent,text)

# player.equip_armor(ent,100)

# ppnix.print("test")

# Use correct Parameters
@event.map_spawn
def mapspawn():
	ppnix.print("Just an Example!!")


@event.connect
def connect(ent):
	# player.draw_text_at(100,100,"Edit!")
	ppnix.print("On Connect!")
