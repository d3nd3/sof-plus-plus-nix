#include "common.h"
/*
	Max feed items = 7

	New feed item is appended to list.

	Grows downwards.

	When one expires, every item below is moved upwards.


	Will use std::list because its a linked list. 
	Circular buffer also works.

	Linked list:
		Frame:
			Check timer of every item in list.
				If expire:
					remove item from linked list.
					trigger refresh

		Refresh:
			Draw linked list.


	global variable meansOfDeath is used to store mod.
	g_main.cpp
		int meansOfDeath;

	g_combat.cpp
		T_Damage
			void Killed (edict_t *targ, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
				void player_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
					void gamerules_c::clientObituary(edict_t *self, edict_t *inflictor, edict_t *attacker)


*/

typedef struct killfeed_s
{
	float time_of_death;
	int means_of_death;
	edict_t * killer;
	edict_t * victim;
} killfeed_t;

std::list<killfeed_t> killFeedList;

PyObject * py_killfeed_func = NULL;

/*
	Resizes list based on expiration.
	Should be called every frame.
	If list is changed, refresh has to be triggered.
*/
void killFeedExpiration(void)
{
	void * level = stget(base_addr + 0x002ACB1C,0);
	float * time_now = level+4;

	// Iterate over the list and remove items if condition is met
	auto it = killFeedList.begin();
	while (it != killFeedList.end()) {
		// Timer expired.
		if ( *time_now - it->time_of_death > 10.0f ) {
			SOFPPNIX_DEBUG("A death card expired.");
			it = killFeedList.erase(it);
			refreshScreen(NULL);
		} else {
			++it;
		}
	}
}

/*
	Called by clientObituary hook.
	refresh is triggered.
*/
void submitDeath(int mod,edict_t * killer,edict_t * victim)
{
	SOFPPNIX_DEBUG("SubmitDeath");
	void * level = stget(base_addr + 0x002ACB1C,0);
	float * time_now = level+4;

	killfeed_t newKill;
	newKill.time_of_death = *time_now;
	newKill.means_of_death = mod;
	newKill.killer = killer;
	newKill.victim = victim;

	// Save the time the death occured.
	killFeedList.push_back(newKill);
	refreshScreen(NULL);
}


void drawKillFeed(void)
{
	// A list of dicts sent to the python callback.
	if ( py_killfeed_func != NULL ) {
		PyObject* deathList = PyList_New(0);
		
		for (const auto& death_card : killFeedList) {
			PyObject* deathDict = PyDict_New();
			PyObject * killer = createEntDict(death_card.killer);
			if ( killer == NULL ) {
				error_exit("killer pointer invalid");
			}
			PyObject * victim = createEntDict(death_card.victim);
			if ( victim == NULL ) {
				error_exit("victim pointer invalid");
			}
			PyObject* t = PyFloat_FromDouble( death_card.time_of_death);
			PyObject * m = PyLong_FromLong( death_card.means_of_death);
			PyDict_SetItemString(deathDict, "death_time",t);
			PyDict_SetItemString(deathDict, "mod",m);
			PyDict_SetItemString(deathDict, "killer",killer);
			PyDict_SetItemString(deathDict, "victim",victim);

			// Add dict to the list.
			PyList_Append(deathList, deathDict);

			Py_DECREF(t);
			Py_DECREF(m);
			Py_DECREF(killer);
			Py_DECREF(victim);

			Py_DECREF(deathDict);
		}
		

		PyObject* result = PyObject_CallFunction(py_killfeed_func,"O",deathList);

		// returns None
		Py_XDECREF(result);
		Py_DECREF(deathList);
	}

}