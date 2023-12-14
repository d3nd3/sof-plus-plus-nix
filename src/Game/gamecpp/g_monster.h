
#ifndef __G_MONSTER_H__
#define __G_MONSTER_H__

class CRadiusContent;
class ai_c;

#define MAX_SEARCHABLE_ENEMIES 64
#define MAX_SOUND_TRIGGERS 16

typedef enum
{
	VOICECODE_UNKNOWN,
	VOICECODE_BACKUP,
	VOICECODE_THROW,
	VOICECODE_DUCK,
	VOICECODE_RETREAT,
	VOICECODE_HELP,
	VOICECODE_REACT
} voiceCode_e;

typedef enum
{
	SPOOK_NOT,
	SPOOK_DEAD_FRIEND,
	SPOOK_EXPLOSIVE,	//use this for initial impact
	SPOOK_EXPLOSIVE2	//use this for the time period after the initial impact (so guys don't keep rolling)
};

class monster_global
{
private:
	float		lastPriorityUpdate;
	int			lastWakeSound;
	int			lastVoiceSound;
	voiceCode_e	lastVoiceCode;
	int			clientNode;
	int			clientRegion;

	int			nearbyEnemies[MAX_SEARCHABLE_ENEMIES];
	int			numNearbyEnemies;
	int			numSeenEnemies;

	int			numSoundTriggers;
	int			soundTriggers[MAX_SOUND_TRIGGERS];//these will be searched frequently, so make them quick
	int			soundTriggersValid;

	int			closestEnemy;
public:
			monster_global(void){ Init(); }
void		Init(void) { lastPriorityUpdate = 0; lastWakeSound = 0; lastVoiceSound = 0; lastVoiceCode = VOICECODE_UNKNOWN; soundTriggersValid = 0; closestEnemy = 0; clientNode = 0; clientRegion = 0; }
qboolean	CheckBottom (ai_c &which_ai, edict_t *ent, vec3_t pos, int motion_allowed);
qboolean	CheckBottom (edict_t *ent);
void		CheckGround (edict_t *ent);
void		CatagorizePosition (edict_t *ent);
void		WorldEffects (edict_t *ent);
void		DropToFloor (edict_t *ent);
void		SetSightClient (void);
int			Range (edict_t *self, edict_t *other, float dist_sq=0);
qboolean	Visible (edict_t *self, edict_t *other);
qboolean	Infront (edict_t *self, edict_t *other);
void		MakeSound(edict_t *listener, vec3_t origin, float time, edict_t *noisemaker, ai_sensetype_e type);
void		RadiusDeafen (edict_t *inflictor, float degree, float radius);
void		UpdateEnemyPriority(void);
int			GetClientNode(void){return clientNode;}
void		FindGuysInRadius(vec3_t center, float radius, CRadiusContent *content, int squashZ);
void		AddSeenEnemy(void){numSeenEnemies++;}
int			GetSeenEnemyCount(void){return numSeenEnemies;}

void		SetVoiceSoundTime(float newVoiceTime){lastVoiceSound=newVoiceTime;}
voiceCode_e	GetVoiceCode(void){return lastVoiceCode;}
void		SetVoiceCode(voiceCode_e newVoiceCode){lastVoiceCode=newVoiceCode;}
qboolean	IsReadyForVoiceSound(void);

void		SetWakeSoundTime(float newWakeTime){lastWakeSound=newWakeTime;}
qboolean	GetWakeSoundTime(void){return lastWakeSound;}
qboolean	IsReadyForWakeSound(void);
int			GetNumNearbyEnemies(void){return numNearbyEnemies;}

void		SpookEnemies(vec3_t center, float radius, int style);

void		InitSoundTriggers(void);
void		ClearSoundTriggers(void){numSoundTriggers = 0;}
void		AddSoundTrigger(int val);
void		RemoveSoundTrigger(int val);
int			GetActiveSoundTriggers(void){if(!soundTriggersValid)InitSoundTriggers(); return numSoundTriggers;}
int			GetSoundTrigger(int num){if(!soundTriggersValid)InitSoundTriggers(); assert(num > -1);assert(num < numSoundTriggers);return soundTriggers[num];}

edict_t		*GetClosestEnemy(void);

};

extern monster_global gmonster;

//  monster
void SP_m_x_romulan (edict_t *self);
void SP_m_x_chopper_black (edict_t *self);
void SP_m_x_chopper_green (edict_t *self);
void SP_m_x_chopper_white (edict_t *self);
void SP_m_x_hind (edict_t *self);
void SP_m_x_tank (edict_t *self);
void SP_m_x_snowcat (edict_t *self);
void SP_m_x_bull(edict_t *self);

//dogs
void SP_m_x_husky(edict_t *self);
void SP_m_x_rottweiler(edict_t *self);
void SP_m_x_germanshep(edict_t *self);
void SP_m_x_raiderdog(edict_t *self);

//ectos
void SP_m_kos_eofficer (edict_t *self);
void SP_m_nyc_etourist (edict_t *self);
void SP_m_irq_eofficer (edict_t *self);
void SP_m_irq_eman1 (edict_t *self);
void SP_m_nyc_estockbroker (edict_t *self);
void SP_m_nyc_eskinhead3 (edict_t *self);
void SP_m_nyc_ebum (edict_t *self);
void SP_m_nyc_epunk (edict_t *self);
void SP_m_sib_eofficer (edict_t *self);
void SP_m_sib_escientist1 (edict_t *self);
void SP_m_tok_mman1 (edict_t *self);
void SP_m_afr_eworker (edict_t *self);
void SP_m_afr_ecommander (edict_t *self);
//unfinished
void SP_m_kos_erefugee (edict_t *self);

//mesos
void SP_m_x_mskinboss (edict_t *self);
void SP_m_nyc_mskinhead1 (edict_t *self);
void SP_m_nyc_mskinhead2a (edict_t *self);
void SP_m_nyc_mpunk (edict_t *self);
void SP_m_nyc_mswat (edict_t *self);
void SP_m_nyc_mswatleader (edict_t *self);
void SP_m_nyc_mpolitician (edict_t *self);
void SP_m_kos_mgrunt1 (edict_t *self);
void SP_m_cas_mraider2a (edict_t *self);
void SP_m_irq_mpolice (edict_t *self);
void SP_m_irq_msoldier1 (edict_t *self);
void SP_m_irq_msoldier2a (edict_t *self);
void SP_m_irq_mrepguard1a (edict_t *self);
void SP_m_irq_moilworker (edict_t *self);
void SP_m_sib_mtrooper1b (edict_t *self);
void SP_m_sib_mcleansuit (edict_t *self);
void SP_m_tok_mman2 (edict_t *self);
void SP_m_afr_msniper (edict_t *self);
void SP_m_afr_msoldier1a (edict_t *self);
void SP_m_afr_msoldier2 (edict_t *self);
//unfinished
void SP_m_x_mraiderboss1 (edict_t *self);
void SP_m_x_mraiderboss2 (edict_t *self);
void SP_m_x_miraqboss(edict_t *self);
void SP_m_nyc_mskinhead2b (edict_t *self);
void SP_m_afr_msoldier1b (edict_t *self);
void SP_m_afr_msoldier3 (edict_t *self);
void SP_m_afr_mbrute (edict_t *self);
void SP_m_afr_mrocket (edict_t *self);
void SP_m_kos_mgrunt2 (edict_t *self);
void SP_m_kos_mgrunt3 (edict_t *self);
void SP_m_kos_msniper1a (edict_t *self);
void SP_m_kos_msniper1b (edict_t *self);
void SP_m_kos_mcomtroop (edict_t *self);
void SP_m_kos_mbrute1a (edict_t *self);
void SP_m_kos_mbrute1b (edict_t *self);
void SP_m_kos_mmechanic (edict_t *self);
void SP_m_kos_mrebel (edict_t *self);
void SP_m_kos_mklaleader (edict_t *self);
void SP_m_sib_mtrooper1a (edict_t *self);
void SP_m_sib_mtrooper2 (edict_t *self);
void SP_m_sib_mguard1 (edict_t *self);
void SP_m_sib_mguard3 (edict_t *self);
void SP_m_sib_mguard4 (edict_t *self);
void SP_m_sib_mmechanic (edict_t *self);
void SP_m_irq_msoldier2b (edict_t *self);
void SP_m_irq_mrepguard1b (edict_t *self);
void SP_m_irq_mcommander (edict_t *self);
void SP_m_irq_mbrute1a (edict_t *self);
void SP_m_irq_mbrute1b (edict_t *self);
void SP_m_irq_mbodyguard (edict_t *self);
void SP_m_irq_mrocket (edict_t *self);
void SP_m_irq_msaddam (edict_t *self);
void SP_m_irq_mman2 (edict_t *self);
void SP_m_tok_mhench1 (edict_t *self);
void SP_m_tok_mhench2 (edict_t *self);
void SP_m_tok_mkiller (edict_t *self);
void SP_m_tok_fassassin (edict_t *self);
void SP_m_tok_mninja (edict_t *self);
void SP_m_tok_mbrute (edict_t *self);
void SP_m_cas_mraider1 (edict_t *self);
void SP_m_cas_mraider2b (edict_t *self);
void SP_m_cas_mbrute (edict_t *self);
void SP_m_cas_mrocket (edict_t *self);

void SP_m_x_mcharacter(edict_t *self);
void SP_m_x_mcharacter_snow(edict_t *self);
void SP_m_x_mcharacter_desert(edict_t *self);
void SP_m_x_mmerc(edict_t *self);
void SP_m_x_mhurtmerc(edict_t *self);
void SP_m_x_msam(edict_t *self);

//females
void SP_m_nyc_fskinchick(edict_t *self);
void SP_m_x_ftaylor (edict_t *self);
//unfinished
void SP_m_nyc_fwoman(edict_t *self);
void SP_m_sib_fguard2(edict_t *self);
void SP_m_sib_fscientist2(edict_t *self);
void SP_m_irq_fwoman1 (edict_t *self);
void SP_m_irq_fwoman2 (edict_t *self);
void SP_m_tok_fwoman1 (edict_t *self);
void SP_m_tok_fwoman2 (edict_t *self);
void SP_m_cas_ffemale (edict_t *self);

#endif