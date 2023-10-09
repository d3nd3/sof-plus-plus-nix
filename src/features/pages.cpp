#include "common.h"
std::unordered_map<std::string,PyObject*> py_page_draw_routines;
std::string current_page[32];

std::array<char [1024],32> page_layout;
std::array<char [1024],32> hud_layout;
std::array<int,32> strip_layout_size = {0};

void page_none(edict_t * ent, edict_t * killer);
void page_scoreboard(edict_t * ent, edict_t * killer);
void page_chat(edict_t * ent, edict_t * killer);




/*
	Different views of scoreboard.
	When scoreboard is toggled, it can show alternate views with a timeout.
	Perhaps sticky option can be used.
	eg.
		.stats * == show stats sticky
		.players * == show country code
		.chat * == show chat history



	If any the layout buffer changes, a refresh of screen is required, if want to see the changes instantly.
	Else has to wait until natural refresh occurs. ( toggle of scoreboard ).

	There is a draw function for each page. They only are called on 'refresh' frames.

	The layout data is emptied every server frame, so its built up every frame too. This makes sense because the data is dynamic. Perhaps one could separate fixed data from dynamic data to save some processing.

	Or it could be one function that is called after the draw, instead of every frame. But then the data is not fed live information, could only work for the static part.  Idea of discarding what you've just drawn because its stale.

	The bold test was solved by using a sb_client string offscreen, it enables antialiased text client-side.

	The buffers are cleared after each 'refresh'. And also drawn each 'refresh' technically, too.

	Thus draw routines should only exist inside the draw hooks which are called in sync with the refreshes.

*/

void showScoreboard(edict_t * ent, unsigned int slot, int showscores,edict_t * killer=NULL, qboolean logfile = 0)
{
	
	if ( showscores) {
		// Does the routine exist in the map, is it registered by python?
		// Draw the page that is currently set, with timeout.
		// If sticky set, make permanant.
		
		if ( py_page_draw_routines.count(current_page[slot]) ) {
			
			// Draw.
			PyObject * who = createEntDict(ent);
			if ( who == NULL ) {
				error_exit("killer pointer invalid");
			}
			PyObject * who_killer = createEntDict(killer);
			if ( who_killer == NULL ) {
				error_exit("killer pointer invalid");
			} 
			// Draw the current page.
			PyObject* result = PyObject_CallFunction(py_page_draw_routines[current_page[slot]],"OO",who,who_killer);
			// returns None
			Py_XDECREF(result);

			Py_XDECREF(who);
			Py_XDECREF(who_killer);
		} else {
			currentGameMode->clientScoreboardMessage(ent,killer,0);
		}
	}
	// SOFPPNIX_DEBUG("%s",strip_layouts[slot]);
	

}
bool page_should_refresh[32] = {false};
/*
	called by:
		my_Cmd_Say_f
		py_player_refresh_layout
		clientConnect
*/
void refreshScreen(edict_t * ent)
{
	if ( ent == NULL ) {
		for ( int i = 0 ; i < maxclients->value;i++ ) {
			void * client = getClientX(i);
			int state = *(int*)(client);
			if (state != cs_spawned )
				continue;

			page_should_refresh[i] = true;
		}
		return;
	}
	int slot = slot_from_ent(ent);
	void * client = getClientX(slot);
	int state = *(int*)(client);
	if (state != cs_spawned )
		return;
	
	page_should_refresh[slot] = true;
}
/*
	ctf_team_sb enables anti-aliasing.
*/
void empty_reset_hud_layout(int slot)
{

	hud_layout[slot][0] = 0x00;
	sprintf(hud_layout[slot],"ctf_team_sb 0 -4000 0 0 0 0 xr %i yb -16 string \"\x07%s\" ",0 - (sofreebuild_len*8+8),sofreebuildstring);
	// sprintf(hud_layout[slot],"xr %i yb -16 string \"\x07%s\" ",0 - (sofreebuild_len*8+8),sofreebuildstring);
	strip_layout_size[slot] = strlen(hud_layout[slot]) + strlen(page_layout[slot]);

	// SOFPPNIX_DEBUG("Clearing hud layout for slot %i. lengths = %i + %i = %i",slot,strlen(hud_layout[slot]),strlen(page_layout[slot]),strip_layout_size[slot]);

}
void empty_reset_page_layout(int slot)
{
	page_layout[slot][0] = 0x00;

	strip_layout_size[slot] = strlen(hud_layout[slot]) + strlen(page_layout[slot]);
}

/*
	0 arguments
	clears screen. ( Excluding scoreboard ).

	Puts a watermark on the screen.

	Intention: Clear without remove watermark.

	dm->checkEvents - NotActive.
	clientScorebordMessage - Clear if toggle off.
	dm->clientEndFrame - overrides G_SetStats. To force Layout on.

	called by:
		xxxx showScoreboard (removed) (no longer true).xxxx

		1)clientConnect
		2)SV_RunGameFrame (early).
		3)py_player_clear_layout
	LATEST:
		I only want this called after a resfresh occured.

*/
void layout_clear(LayoutMode mode,edict_t * ent)
{
	if ( ent == NULL ) {
		// SOFPPNIX_DEBUG("Clearing screen");

		for ( int i = 0 ; i < maxclients->value;i++ ) {
			void * client = getClientX(i);
			int state = *(int*)(client);
			if (state != cs_spawned )
				continue;

			switch (mode) {
				case hud:
					empty_reset_hud_layout(i);
					break;
				case page:
					empty_reset_page_layout(i);
					break;
				default:
					error_exit("Bad layout mode");
			}
		}

		// orig_Com_Printf("Layoutstring is : %s\n",layoutstring);

		return;
	}
	int i = slot_from_ent(ent);
	// SOFPPNIX_DEBUG("Reset for slot %i",i);
	void * client = getClientX(i);
	int state = *(int*)(client);
	if (state != cs_spawned )
		return;

	switch (mode) {
		case hud:
			empty_reset_hud_layout(i);
			break;
		case page:
			empty_reset_page_layout(i);
			break;
		default:
			error_exit("Bad layout mode");
	}

}

static void append_to_buffer(int slot,LayoutMode mode, char * newstring)
{
	// SOFPPNIX_DEBUG("pages.cpp @Append_to_buffer : slot %i",slot);
	char * append_to;
	char * count_this;
	int * incr_this;
	switch (mode) {
		case hud:
			append_to = hud_layout[slot];
			count_this = page_layout[slot];
			break;
		case page:
			append_to = page_layout[slot];
			count_this = hud_layout[slot];
			break;
		default:
			error_exit("Bad layout mode");
	}

#if 0
	char * tmp = strdup(append_to);
	int req_size = snprintf( append_to,sizeof(hud_layout),"%s%s",tmp,newstring);
	
	if ( req_size < sizeof(hud_layout) ) {
		// Fits.
		strip_layout_size[slot] += req_size;
	} else {
		SOFPPNIX_PRINT("Warning some drawing was truncated.");
		// total is now 1024.
		strip_layout_size[slot] = strlen(count_this) + 1024; 
	}
#else
	int new_item_len = strlen(newstring);
	int freespace = 1023-strlen(append_to);
	if ( freespace < 0 ) freespace = 0;
	strncat(append_to,newstring,freespace);
	// SOFPPNIX_DEBUG("buffer now contains\n%s",append_to);
	if ( new_item_len > freespace ) {
		// SOFPPNIX_PRINT("Warning some drawing was truncated.");
		// total is now 1024.
		strip_layout_size[slot] = strlen(count_this) + 1023 + 1;
	} else
	{
		// SOFPPNIX_DEBUG("Added %i bytes to %i buffer",new_item_len,slot);
		// Fits.
		strip_layout_size[slot] += new_item_len;
	}
#endif
}
void append_layout_image(LayoutMode mode,edict_t * ent,int offsetx, int offsety, char * img_path)
{

	char newstring[256];
	snprintf(newstring,256,"xv %i yv %i picn \"%s\" ",offsetx+160,offsety+120,img_path);
	
	// broadcast
	if ( ent == NULL ) {
		for ( int i = 0 ; i < maxclients->value;i++ ) {

			void * client = getClientX(i);
			int state = *(int*)(client);
			if (state != cs_spawned )
				continue;


			append_to_buffer(i,mode,newstring);
		}
		return;
	}

	// individual client
	int i = slot_from_ent(ent);
	void * client = getClientX(i);
	int state = *(int*)(client);
	if (state != cs_spawned )
		return;
	append_to_buffer(i,mode,newstring);
}

/*
Appends to strip_layouts.
Calls RefreshScreen. ( Redraws Scoreboard ).
yb yBottom 0 = bottom of screen, up=negative, down=positive
yt yTop 0 = top of screen, up=negative, down=positive
yv centered

xl xLeft
xr xRight
xv centered

3 arguments - x y msg
*/
void append_layout_string(LayoutMode mode,edict_t * ent,int offsetx, int offsety, char * message)
{

#if 0
	int len = strlen(message);
	if ( gray ) {
		for (int i = 0; i < len; i++ ) {
			*(message+i) = *(message+i) | 0x80;
		}
	}
#endif
	char newstring[256];
	snprintf(newstring,256,"xv %i yv %i string \"%s\" ",offsetx+160,offsety+112,message);
	#if 0
	if ( ! gray ) {
		snprintf(newstring,256,"xv %i yv %i string \"%s\" ",offsetx+160,offsety+112,message);
	} else {
		snprintf(newstring,256,"xv %i yv %i altstring \"%s\" ",offsetx+160,offsety+112,message);
	}
	#endif

	// broadcast
	if ( ent == NULL ) {

		for ( int i = 0 ; i < maxclients->value;i++ ) {
			void * client = getClientX(i);
			int state = *(int*)(client);
			if (state != cs_spawned )
				continue;

			append_to_buffer(i,mode,newstring);
		}
		
		return;
	}

	int i = slot_from_ent(ent);
	void * client = getClientX(i);
	int state = *(int*)(client);
	if (state != cs_spawned )
		return;
	append_to_buffer(i,mode,newstring);
}
/*
more direct version of above.
//xl = xpos from left
//xr = xpos from right
//xv = some standardized center -160
//yt = ypos from top
//yb = ypos from bottom
//yv = some standardized center -120
//picn = a picture
//client_sb
//client_ctf_sb
//spect_sb
//team_sb
//client_conq_sb
//ctf_team_sb
//control_team_sb
//string
//altstring
//tc
//ac
*/
void append_layout_direct(LayoutMode mode,edict_t * ent,char * message)
{
	int len = strlen(message);

	char newstring[256];
	snprintf(newstring,256,"%s ",message);

	if ( ent == NULL ) {
		for ( int i = 0 ; i < maxclients->value;i++ ) {
			void * client = getClientX(i);
			int state = *(int*)(client);
			if (state != cs_spawned )
				continue;
			// hexdump(newstring,newstring+strlen(newstring));
			append_to_buffer(i,mode,newstring);
		}
		
		return;
	}
	int i = slot_from_ent(ent);
	void * client = getClientX(i);
	int state = *(int*)(client);
	if (state != cs_spawned )
		return;

	// hexdump(newstring,newstring+strlen(newstring));
	append_to_buffer(i,mode,newstring);
}
