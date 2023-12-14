#include "g_local.h"
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>
#include "ds.h"
#include "w_weapons.h"

#define SCRIPT_SAVE_VERSION		3
#define ROFF_VERSION			1			// ver # for the (R)otation (O)bject (F)ile (F)ormat for script command "moverotate"
#define ROFF_EOS_MARKER			10000
vec3_t	v3ROFF_EOS_Marker = {ROFF_EOS_MARKER, ROFF_EOS_MARKER, ROFF_EOS_MARKER};

list<Variable *>	GlobalVariables;
list<CScript *>		Scripts;
map<string, void *>	ROFFs;

// used to indicate the first call to CScript::Think() _after_ the player decided
//to skip the cinematic
bool	bClearCinematicSounds = true;

// marks the start time of a cinematic (more or less)
float	s_cinematicStartTime = 0;

// hmmm, clean up duplicates...
#define UNKNOWN				0
#define RUN					1
#define JUMP				2
#define DEATH				5
#define	DEATHTHROWN			6
#define	DUCK_DN				7
#define DUCK_MID			8
#define DUCK_UP				9
#define DUCK_SHOOT			10
#define IDLE				11
#define IDLESTRETCH			12
#define IDLELOOK			13
#define SHOOT				14
#define WALK				15
#define	GUN_IDLE			17
#define	AFRAIDRUN			19

#define STD_ICLEANGUN_N_A_N		20
#define STD_ILOOKING_N_A_A		21
#define STD_IMONITORUP_N_A_N	22
#define STD_ITALK_N_A_N			23
#define STD_ITALKGEST1_N_A_N	24
#define STD_ITALKGEST2_N_A_N	25
#define STD_ITALKGEST3_N_A_N	26
#define STD_IWIPEBROW_N_A_N		27
#define STD_ILOOKING_N_MRS_2	29

#define STD_XALARMPUSH_N_A_N	30
#define STD_ISTAND_N_MRS_2		31
#define STD_ITALK_N_MRS_2		32
#define STD_ITURNVALVE_FRM_N_N	33
#define STD_ITURNVALVE_N_N_N	34
#define STD_ITURNVALVE_TO_N_N	35
#define STD_JDROP_N_A_A			36
#define STD_MKICKLOW_N_A_A		37
#define STD_MPISTOLWHIP_N_P_N	38
#define STD_SINBACK_2FRNT_N_N	39

#define STD_XYELLPOINT_FWD_A_N	40
#define STD_XYELLWAVE_BK_A_N	41
#define STD_FATTITUDE_N_P_A		42
#define LS_XGETKICKED_N_N_N		43
#define SI_XBYWALL_N_N_N		44
#define STD_SSTUMBLE_2BK_P_N	45
#define STD_WPUSH_N_N_N			46
#define STD_XAFRAIDSHAKE_N_N_N	47
#define STD_XKICKDOOR_N_A_A		48
#define	STD_XYELLATYOU_N_A_A   	49

#define STD_WCARRY1_N_N_N		50
#define STD_WNORMAL_N_PK_A		51
#define STD_WPATROL_N_MRS_2		52
#define STD_WBCKSIGNAL_N_N_N	53
#define STD_ILEANLOOK_FRM_PK_N	54
#define STD_ILEANLOOK_MID_PK_N	55
#define STD_ILEANLOOK_TO_PK_N	56
#define STD_JDOWN_N_A_A			57
#define STD_F_FWD_P_2			58
#define STD_F_FWD_S_2			59

#define STD_R_N_MRS_2			60
#define STD_R_N_PK_N			61
#define STD_RGUNUP_N_P_2		62
#define STD_XDONTSHOOT_N_A_N	63
#define STD_XFEARWAVE_N_N_N		64
#define STD_FCORNERPEAK_LT_P_A	65
#define STD_FCORNERPEAK_RT_P_A  66
#define STD_IALERT_N_P_N		67
#define STD_ICLEANGUN2_N_MRS_2	68
#define	STD_IMONITORUP2_N_A_N	69

#define STD_JDIVE2PRONE_N_A_N	70
#define STD_RAFRAID_N_N_N		71 // duplicate of AFRAIDRUN
#define STD_SBLOWN_2BK_N_N		72
#define STD_XBOOMCROUCH_N_A_N	73
#define STD_XTUMBLE_2FRNT_N_N	74
#define STD_JRUN_FWD_A_A		75
#define STD_ISMOKE_FRM_A_N		76
#define STD_ISMOKE_N_A_N		77
#define STD_ISMOKE_TO_A_N		78
#define STD_ITOUCHNUTS_N_A_N	79

#define CCH_XCROUCH_UP_MRS_2	80
#define CCH_XCROUCH_UP_PK_A		81
#define CCH_A_FWD_P_2			82
#define CCH_A_FWD_R_2			83
#define CCH_SONKNEES_2FRNT_N_N	84
#define CCH_XONKNEES_N_N_N		85
#define CCH_XCCHCOWER_MID_A_A	86
#define CCH_XCCHCOWER_MID2_A_A	87
#define PRN_XCOVERHEAD_N_A_A	88
#define STD_SVIOLENT_N_N_N		89

#define LB_PGENERICDIE_N_N_N	90
#define STD_WATTITUDE_N_PK_A	91
#define STD_WJOG_N_MRS_2		92
#define STD_WJOGGUNDWN_N_P_2	93
#define STD_WJOGGUNUP_N_P_A		94
#define STD_WPUSHCART_N_N_N		95
#define STD_FAUTOSWEEP_N_PM_A	96
#define STD_FGANGBANGER_N_M_2	97
#define STD_FSWATSTYLE_FWD_M_2	98

#define SCRIPT_RELEASE		99

#define STD_FVIOLENT_DWN_M_2	100
#define STD_FVIOLENT_DWN_P_N	101
#define STD_JUPTO_N_A_A			102
#define STD_MNINJAKICK_N_A_B	103
#define STD_MRIFLEBUTT_N_MRS_2	104
#define STD_R_DWNST_MRS_2		105
#define STD_RAIMSTRAF_LT_MRS_2	106
#define STD_RAIMSTRAF_LT_P_CBN	107
#define STD_RAIMSTRAF_RT_MRS_2	108
#define STD_RAIMSTRAF_RT_P_CBN	109

#define STD_RBACKAIM_N_MRS_2	110
#define STD_RBACKAIM_N_P_2		111
#define STD_SFALLFWD_2FRNT_N_N	112
#define STD_SSTUMBLE_2FRNT_N_N	113
#define STD_XCCHCOWER_DWN_A_A	114
#define STD_XCHOKE_N_N_N		115
#define STD_XDANCE_N_N_N		116
#define STD_XGOPRONE_N_A_A		117
#define STD_XLADDER_DWN_A_A		118
#define STD_XLADDER_UP_A_A		119

#define STD_XROLL_LT_A_A		120
#define STD_XROLL_RT_A_A		121
#define STD_XROLLTOCCH_FWD_A_A	122
#define STD_XSTARTLED_N_A_A		123
#define STD_XSTUMBLE_N_A_A		124
#define SI_ISIT_N_A_A			125
#define SI_XALERTSTAND_UP_A_A	126
#define PRN_C_N_A_2				127
#define PRN_XCOVERHEAD_TO_A_A	128
#define PRN_XFALLING_N_N_N		129

#define PRN_XFALLLAND_N_N_N		130
#define PRN_XTOSTAND_N_A_A		131
#define CCH_RAIM_FWD_MRS_2		132
#define CCH_RAIM_FWD_P_2		133
#define CCH_XCCHCOWER2_MID_A_A	134
#define LB_PCRAWL_BCK_A_N		135 // missspellled. this is actually #define LB_PCRAWL_BK_A_N
#define LB_XLEANUPSHOOT_N_P_N	136
#define CCH_A_FWD_MS_2			137
#define CCH_F_FWD_L_2			138
#define CCH_RAIM_N_L_2			139

#define CCH_TLOBGRENADE_N_A_N	140
#define STD_ECHOKEKNEES_TO_N_N	141
#define CCH_ECHOKEKNEES_N_N_N	142 
#define CCH_ECHOKEDEATH_N_N_N	143 
#define STD_SELECTROCUTE_N_N_N	144
#define STD_EOFFLADDER_UP_A_N	145
#define STD_EPOUNDDOOR_N_A_N	146
#define STD_F_FWD_R_2			147

#define STD_EBUSTGLASS_N_MRS_2	148
#define STD_ECOMEHERE_N_A_N		149

#define STD_EFALLDEATH_N_N_N	150
#define STD_EFALLIMPACT_N_N_N	151
#define STD_EFLIPTABLE_N_A_N	152
#define STD_EPUSHBUTTONS_N_N_N	153
#define STD_EPUSHCRATE_N_N_N	154
#define STD_ESIGNALALARM_N_A_N	155
#define	STD_ETYPING_N_N_N		156
#define STD_EWALL2WALK_N_MRS_2	157
#define STD_EWALL2WALK_N_PK_N	158
#define STD_EWALLIDLE_N_MRS_2	159

#define STD_EWALLIDLE_N_PK_N	160
#define STD_EWALLLOOK_LT_MRS_2	161
#define STD_EWALLLOOK_LT_PK_N	162
#define STD_EWALLLOOK_RT_MRS_2	163
#define STD_EWALLLOOK_RT_PK_N	164
#define STD_EWRENCHH_FRM_N_N	165
#define STD_EWRENCHH_N_N_N		166
#define STD_EWRENCHH_TO_N_N		167
#define STD_EWRENCHV_FRM_N_N	168
#define STD_EWRENCHV_N_N_N		169

#define STD_EWRENCHV_TO_N_N		170
#define STD_ASHUFFLE_N_P_2		171
#define STD_ASHUFFLE_N_P_A		172
#define STD_ASHUFFLE_N_MS_2		173
#define STD_ASHUFFLE_N_R_2		174
#define STD_ASHUFFLE_N_L_2		175
#define STD_IPISS_N_A_A			176

#define STD_ATHREAT_DWN_P_N		177
#define STD_ATHREAT_FWD_P_N		178
#define STD_ATHREATYEL_DWN_P_N	179

#define STD_ATHREATYEL_FWD_P_N	180
#define STD_EEXAMINEWALL_N_A_N	181

#define STD_ERAGESHOOT_N_P_N	182
#define STD_ESABREMOCK_N_A_N	183
#define STD_RTOSTOP_N_PK_N		184
#define STD_ETALKBOTH_N_A_N		185
#define STD_ETALK_N_A_N			186
#define STD_EHEADNOD_N_A_N		187
#define STD_EHEADSHAKE_N_A_N	188
#define STD_EJUMPRAIL_N_A_N		189

#define STD_WTOSTOP_N_PK_A		190
#define STD_ESPIN180_LT_A_N		191
#define STD_ESPIN180_RT_A_N		192
#define STD_ETORUN_N_P_N		193
#define STD_ILEAN_N_P_N			194
#define STD_WBACKAIM_N_MS_2		195
#define STD_ETALKRIGHT_N_A_N	196

#define STD_ECLIMBLEDG_DWN_A_N	197
#define STD_ECLIMBLEDG_MID_A_N	198
#define STD_ECLIMBLEDGE_UP_A_N	199

#define STD_EDROPGRENADE_N_A_N		200
#define STD_EFRMSHADOW_N_MRS_2		201
#define STD_EFRMSHADOW_N_P_N		202
#define STD_EHANDTALK_FRM_P_N		203
#define STD_EHANDTALK_N_P_N			204

#define STD_EHANDTALK_TO_MRS_N		205
#define STD_EHANDTALK_TO_P_N		206
#define STD_EHANDTALK_FRM_MRS_N		207
#define STD_EHEADSET_FRM_P_N		208
#define STD_EHEADSET_MID_P_N		209

#define STD_EHEADSET_TO_P_N			210
#define STD_ESALUTE_FRM_MRS_N		211
#define STD_ESALUTE_N_MRS_N			212
#define STD_ESALUTE_TO_MRS_N		213
#define STD_ETALKLEFT_N_A_N			214

#define STD_ETALKLKLT_FRM_A_N		215
#define STD_ETALKLKLT_MID_A_N		216
#define STD_ETALKLKLT_TO_A_N		217
#define STD_ETALKLKRT_FRM_A_N		218
#define STD_ETALKLKRT_MID_A_N		219

#define STD_ETALKLKRT_TO_A_N		220
#define STD_ETORUN_N_MRS_2			221
#define STD_ETURN90_LT_A_N			222
#define STD_ETURN90_RT_A_N			223
#define STD_IGUNUP_N_A_N			224

#define STD_RONFIRE_N_N_N			225
#define STD_RONFIRETODTH_N_N_N		226
#define STD_RTOSTOP_N_MRS_2			227
#define STD_SSHOULDER_LT_N_N		228
#define STD_SUNHURT_BK_PK_N			229

#define STD_EDROPCROUCH_N_A_A		230
#define STD_TRUN_N_A_N				231
#define STD_W_N_PN_N				232
#define STD_WCARRY2_N_N_N			233
#define STD_WTALK_LT_MRS_2			234

#define A4_ASCRAMBLE_N_A_N			235
#define CCH_ENODLEFT_N_A_A			236
#define CCH_ENODRIGHT_N_A_A			237
#define CCH_ESCANTALK_LT_A_A		238
#define CCH_ESCANTALK_RT_A_A		239

#define CCH_IREADY_N_A_A			240
#define STD_FAUTOSWEEP_N_M_2		241
#define STD_L_N_MS_2				242
#define STD_ISTAND_N_A_N			243
#define STD_ECHOKESHOOT_N_A_N		244

#define STD_SONFIRE_2BK_N_N			245
#define STD_ETALK_FRM_A_N			246
#define STD_ETALK_TO_A_N			247
#define STD_JDROP_TO_A_N			248
#define STD_ATHREATSHT_DWN_P_N		249

#define LB_PHOLDLEG_LT_N_N			250
#define STD_XCOCK_N_S_2				251
#define STD_STOKNEES_2BK_N_N		252
#define STD_SGUT_2SIDE_N_N			253
#define STD_XLADDERFRM_UP_A_A		254

#define STD_SGUNFROMHAND_N_N_N		255
#define CCH_ETOSTAND_N_A_N			256
#define STD_ININJA_N_P_N			257
#define STD_XHANDSPRING_FWD_A_N		258
#define STD_EGANGSALUTE_N_A_N		259

#define STD_WBACKWARDS_N_A_N		260
#define STD_SSABREDEATH_BK_N_N		261
#define STD_WSTEALTH_N_PK_N			262
#define STD_JTHROWSTAR_FWD_A_N		263
#define PRN_A_N_A_2					264

#define STD_EYELL_N_A_N				265
#define STD_IALERTLK_RT_MRS_2		266
#define STD_EPUSHBUTTONH_N_A_N		267
#define STD_IALERT_N_MRS_2			268
#define STD_RDYNAMIC_N_A_N			269

#define STD_WAIM_FWD_P_2			270
#define STD_A_FWD_P_2				271
#define STD_IETALKPOSE_N_A_N		272
#define STD_FRAPIDSWEEP_LT_P_2		273
#define STD_IALERTTRN_LT_P_N		274

#define STD_ESUPRISE90_LT_A_N		275
#define STD_FAUTOSWEEP_UP_M_2		276
#define STD_XATKPAUSE1_N_MRS_2		277
#define LB_PCRAWL_BK_P_N			278 //mmoves[235]
#define SI_EWALLIDLE_N_A_N			279

#define SI_EWALLTALK_N_A_N			280
#define STD_ETOAIM_N_P_2			281
#define STD_A_UP_P_2				282	//mmoves[243]
#define SI_EWALLFEAR_N_A_N			283
#define STD_ESALUTE_TO_A_N			284

#define STD_ESALUTE_N_A_N			285
#define STD_ESALUTE_FRM_A_N			286
#define STD_ESALUTATIONS_N_A_N		287
#define STD_ECLIMBSHAFT_N_A_N		288
#define STD_XATKPAUSE2_N_PK_N		289 //mmoves[85]

#define CCH_W_N_A_N					290
#define CCH_EWALK2CCH_N_A_N			291
#define CCH_ETALK_DWN_P_N			292
#define STD_ITALKPOSE_UP_A_N		293
#define CCH_ITALKPOSE_DWN_A_N		294

#define STD_ETALK_UP_A_N			295
#define CCH_EHEADNOD_N_A_N			296
#define CCH_ECCH2WALK_N_A_N			297
#define STD_XATKPAUSE1_N_PK_N		298 //mmoves[84]
#define STD_WFAST_N_A_N				299

#define STD_ERAGEIDLE_N_P_N			300
#define CCH_XCCHCOWER_UP_A_A		301
#define STD_IETALKPOSE_RT_A_N		302
#define STD_EBOOKLOOK_N_A_N			303
#define STD_EBOOKSCAN_N_A_N			304

#define STD_XOPENDOOR_N_A_N			305
#define STD_XCROUCH_DWN_PK_A		306
#define CCH_EHEADSET_FRM_A_N		307
#define CCH_EHEADSET_MID_A_N		308
#define STD_SHEAD_2FRNT_N_N			309

#define STD_W_N_MRS_2				310
#define STD_EDKRAIM_N_A_N			311
#define CCH_EHAWKKNEES_N_N_N		312
#define STD_EDKRAIMTALK_N_A_N		313
#define STD_EDKRFIRELK_RT_A_N		314

#define CCH_EHAWKDEATH_N_N_N		315
#define CCH_EHAWKKNEETLK_N_N_N		316
#define CCH_EHAWKCROUCH_N_A_N		317
#define CCH_EHEADSET_TO_A_N			318
#define LB_ESLIDE_N_PK_N			319

#define STD_A_FWD_P_CB				320
#define STD_IGUNUPLOOK_N_A_N		321
#define STD_FRAPID_N_P_2			322
#define STD_EDKRAIMLK_FWD_A_N		323
#define STD_EWHIRL_RT_A_N			324

#define STD_FSWEEPUP_FRM_A_A		325
#define STD_FSWEEPUP_TO_A_A			326
#define STD_XTAKECOVER_N_A_N		327
#define STD_ESABREBLUFF_N_A_A		328
#define STD_EAUTOSWEEP_N_PM_A		329

#define CCH_F_FWD_S_2				330
#define STD_F_FWD_P_CB				331
#define STD_SOUTWINDOW_N_A_N		332
#define STD_XCROUCH_DWN_MRS_2		333
#define STD_XCROUCH_DWN_P_2			334

#define STD_TGRENADE_N_A_N			335
#define STD_TUNDER_N_A_N			336
#define CCH_XCROUCH_UP_L_2			337
#define STD_XCROUCH_DWN_L_2			338
#define STD_XCROUCH_DWN_P_A			339

#define STD_RAIM_N_MRS_2			340
#define STD_WFIRE_FWD_MS_2			341
#define CCH_AFIRE_FWD_MS_2			342
#define STD_F_FWD_M_2				343
#define CCH_F_FWD_M_2				344

#define STD_EPUSHOVER_N_PK_A		345
#define STD_EMERCHALL_N_A_A			346
#define STD_EMERCHONE_N_A_A			347
#define STD_IEMERCHANT_N_A_A		348
#define STD_EMERCHWAVE_RT_A_A		349

#define STD_EMERCHTALK_N_A_A		350
#define STD_EMERCHWAVE_LT_A_A		351
#define STD_EBEG_N_A_A				352
#define STD_ICLEANGUN2_N_PK_N		353
#define CCH_XCCHCOWER_N_A_N			354


#define HELI_TAKEOFF		1
#define HELI_LAND			2
#define HELI_REPAIR			3
#define HELI_REARM			4
#define HELI_GOTO_COORDS	5
#define HELI_GOTOREL_ENTITY	6
#define HELI_GOTOREL_ENT_X	7
#define HELI_GOTOREL_ENT_Y	8
#define HELI_GOTOREL_ENT_Z	9
#define HELI_MOVEREL		10
#define HELI_PAUSE			11
#define HELI_FACE_RELENT	12
#define HELI_FACE_ABSCOORDS	13
#define HELI_FACE_ABSDIR	14
#define HELI_FACE_RELCOORDS	15
#define HELI_PILOT_FACERELENT		16
#define HELI_PILOT_FACERELCOORDS	17
#define HELI_PILOT_FACEABSCOORDS	18
#define HELI_GUNNER_FACERELENT		19
#define HELI_GUNNER_FACERELCOORDS	20
#define HELI_GUNNER_FACEABSCOORDS	21
#define HELI_STRAFE_RT				22
#define HELI_STRAFE_LT				23
#define HELI_ROCKETS_ENABLE			24	
#define HELI_ROCKETS_DISABLE		25
#define HELI_CHAINGUN_ENABLE		26
#define HELI_CHAINGUN_DISABLE		27
#define HELI_FIREAT_RELENT			28
#define HELI_FIREAT_ABSCOORDS		29
#define HELI_FIREAT_RELCOORDS		30
#define HELI_AUTOFIRE_ON			31
#define HELI_AUTOFIRE_OFF			32
#define HELI_HOVER_PASSIVE			33
#define HELI_HOVER_AGGRESSIVE		34
#define HELI_SET_WORLDMINS			35
#define HELI_SET_WORLDMAXS			36
#define HELI_SET_MAXHEALTH			37
#define HELI_SET_HEALTH				38
#define HELI_SET_DEATHDEST			39
#define HELI_SET_TRACEDIMS			40
#define HELI_AI						41
#define HELI_WAYPOINT				42
#define HELI_VOLUME					43
#define HELI_VOLUMEMINS				44
#define HELI_VOLUMEMAXS				45
#define HELI_DEBUG					46
#define HELI_CHANGE_SKIN			47
#define HELI_CHANGE_BODY			48
#define HELI_OUT_OF_CONTROL			49


#define TANK_GOTOCOORDS					1
#define TANK_FIRECANNONATCOORDS			2
#define TANK_MACHGUNAUTO				3
#define TANK_DIE						4
#define TANK_AIMTURRET					5

#include "ai_private.h"
#include "m_heliai.h"
#include "m_tankai.h"

extern void Use_Multi(edict_t *self, edict_t *other, edict_t *activator);
extern void c_swapplayer(edict_t *Self,edict_t *Cinematic);
extern void remove_non_cinematic_entites(edict_t *owner);
extern void reinstate_non_cinematic_entites(edict_t *owner);
extern cvar_t 	*Cvar_Set (char *var_name, char *value);


// kef -- begin/end cinematic mode
void CinematicFreeze(bool bEnable)
{
	if (bEnable)
	{
		// kef -- mid-air thrown knives can telefrag cinematic actors, so remove the knives
		edict_t *curEnt = NULL;
		for (int i = 1; i < globals.num_edicts; i++)
		{
			curEnt = &g_edicts[i];
			if (curEnt && (curEnt->owner == &g_edicts[1]))
			{	// if this is a knife thrown by the player, remove it
				if (curEnt->health == 31337)
				{	// it's a thrown knife all right
					G_FreeEdict(curEnt);
				}
			}
		}
		// make sure this cvar-setting stuff happens _after_ setting the client's remotecameratype
		if (g_edicts[1].client)
		{
			// we just started a cinematic. adjust the proper cvar.
			if (!game.cinematicfreeze) 
			{
				game.cinematicfreeze = 1;

				// need to make sure these commands don't stay...uh...commanded during the script
				gi.AddCommandString ("-attack\n");
				gi.AddCommandString ("-altattack\n");
				gi.AddCommandString ("-weaponExtra1\n");
				gi.AddCommandString ("-weaponExtra2\n");
				gi.AddCommandString ("-use\n");

				if(g_edicts[1].client->inv)
				{
					sharedEdict_t sh;

					sh.inv = (inven_c *)g_edicts[1].client->inv;
					sh.edict = &g_edicts[1];
					sh.inv->setOwner(&sh);

					g_edicts[1].client->inv->deactivateCurrentWeapon();
				}
			}
		}
	}
	else
	{
		if (sv_jumpcinematic->value == 2)	// Jump sent from client
		{
			gi.cvar_set("sv_jumpcinematic","0");
			// if we cleared the sound buffer on the client because we skipped a cinematic,
			//reset the proper cvar
			gi.cvar_set("s_disable", "0");
		}
		game.cinematicfreeze = 0;

		// note the fact that we'll need to clear sounds the next time we 
		//skip a cinematic
		bClearCinematicSounds = true;


/*  kef -- I'm not sure about this 'cinematic entities' stuff

	reinstate_non_cinematic_entites(NULL);
*/
		// undo the stuff from ai_c::think()
		int i = 0;

		for (i = 0; i < globals.num_edicts; i++)
		{
			if (g_edicts[i].ai)
			{
				if (255 == g_edicts[i].count)
				{	// this guy was used by the cinematic that just ended. de-flag him.
					g_edicts[i].count = 0;
				}
				else if (g_edicts[i].flags & FL_CINEMATIC_CULL)
				{	// if he's got ai and we didn't use him in the cinematic, reset the stuff
					//we mucked with in ai_c::think()
					g_edicts[i].nextthink = level.time - g_edicts[i].nextthink;
					g_edicts[i].solid = SOLID_BBOX;
					g_edicts[i].flags &= ~FL_CINEMATIC_CULL;
					if (g_edicts[i].ghoulInst)
					{
						g_edicts[i].ghoulInst->SetOnOff(true, level.time);
					}
				}
			}
		}
	}
}

//==========================================================================

typedef struct RestoreList_s
{
	int ID;
	void *(*alloc_func)(void *);
} RestoreList_t;

void *RF_IntVar(void *Data)
{
	return new IntVar((CScript *)Data);
}

void *RF_FloatVar(void *Data)
{
	return new FloatVar((CScript *)Data);
}

void *RF_VectorVar(void *Data)
{
	return new VectorVar((CScript *)Data);
}

void *RF_EntityVar(void *Data)
{
	return new EntityVar((CScript *)Data);
}

void *RF_StringVar(void *Data)
{
	return new StringVar((CScript *)Data);
}

void *RF_VariableVar(void *Data)
{
	return new VariableVar((CScript *)Data);
}

void *RF_FieldVariableVar(void *Data)
{
	return new FieldVariableVar((CScript *)Data);
}

void *RF_Signaler(void *Data)
{
	return new Signaler((CScript *)Data);
}

void *RF_MoveDoneEvent(void *Data)
{
	return new MoveDoneEvent((CScript *)Data);
}

void *RF_RotateDoneEvent(void *Data)
{
	return new RotateDoneEvent((CScript *)Data);
}

void *RF_MoveRotateEvent(void *Data)
{
	return new MoveRotateEvent((CScript *)Data);
}

void *RF_AnimateDoneEvent(void *Data)
{
	return new AnimateDoneEvent((CScript *)Data);
}

void *RF_HelicopterDoneEvent(void *Data)
{
	return new HelicopterDoneEvent((CScript *)Data);
}

void *RF_TankDoneEvent(void *Data)
{
	return new TankDoneEvent((CScript *)Data);
}

void *RF_ExecuteEvent(void *Data)
{
	return new ExecuteEvent((CScript *)Data);
}

void *RF_WaitEvent(void *Data)
{
	return new WaitEvent((CScript *)Data);
}

void *RF_Script(void *Data)
{
	return new CScript();
}

void *RF_FieldDef(void *Data)
{
	return new FieldDef((CScript *)Data, true);
}

#define RLID_INTVAR					1
#define RLID_FLOATVAR				2
#define RLID_VECTORVAR				3
#define RLID_ENTITYVAR				4
#define RLID_STRINGVAR				5
#define RLID_VARIABLEVAR			6
#define RLID_FIELDVARIABLEVAR		7
#define RLID_SIGNALER				8
#define RLID_MOVEDONEEVENT			9
#define RLID_ROTATEDONEEVENT		10
#define RLID_EXECUTEEVENT			11
#define RLID_WAITEVENT				12
#define RLID_SCRIPT					13
#define RLID_FIELDDEF				14
#define RLID_MOVEROTATEEVENT		15
#define RLID_ANIMATEDONEEVENT		16
#define RLID_HELICOPTERDONEEVENT	17
#define RLID_TANKDONEEVENT			18

RestoreList_t ScriptRL[] = 
{
	{ RLID_INTVAR,				RF_IntVar },
	{ RLID_FLOATVAR,			RF_FloatVar },
	{ RLID_VECTORVAR,			RF_VectorVar },
	{ RLID_ENTITYVAR,			RF_EntityVar },
	{ RLID_STRINGVAR,			RF_StringVar },
	{ RLID_VARIABLEVAR,			RF_VariableVar },
	{ RLID_FIELDVARIABLEVAR,	RF_FieldVariableVar },
	{ RLID_SIGNALER,			RF_Signaler },
	{ RLID_MOVEDONEEVENT,		RF_MoveDoneEvent },
	{ RLID_ROTATEDONEEVENT,		RF_RotateDoneEvent },
	{ RLID_EXECUTEEVENT,		RF_ExecuteEvent },
	{ RLID_WAITEVENT,			RF_WaitEvent },
	{ RLID_SCRIPT,				RF_Script },
	{ RLID_FIELDDEF,			RF_FieldDef },
	{ RLID_MOVEROTATEEVENT,		RF_MoveRotateEvent },
	{ RLID_ANIMATEDONEEVENT,	RF_AnimateDoneEvent },
	{ RLID_HELICOPTERDONEEVENT,	RF_HelicopterDoneEvent },
	{ RLID_TANKDONEEVENT,		RF_TankDoneEvent },

	{ 0,						NULL },
};

void *RestoreObject(RestoreList_t *RestoreList, void *Data)
{
	int				ID;
	RestoreList_t	*pos;

	gi.ReadFromSavegame('SCID', &ID, sizeof(ID));

	for(pos = RestoreList; pos->alloc_func; pos++)
	{
		if (pos->ID == ID)
		{
			return pos->alloc_func(Data);
		}
	}
	gi.dprintf("Warning: Unable to RestoreObject\n");
	return NULL;
}

//==========================================================================

void ProcessScripts(void)
{
	list<CScript *>::iterator	is;

	if (Scripts.size())
	{
		for (is=Scripts.begin();is != Scripts.end();is++)
		{
			(*is)->Think();
		}
	}
}

void ShutdownScripts()
{
	list<CScript *>::iterator		is;
	list<Variable *>::iterator		iv;
	map<string, void *>::iterator	ir;
	int								i;
	edict_t							*ent;

	while(Scripts.size())
	{
		is=Scripts.begin();
		delete (*is);

		Scripts.erase(is);
	}

	for(i = 0, ent = g_edicts; i < globals.num_edicts; i++, ent++)
	{
		ent->Script = NULL;
	}

	while(GlobalVariables.size())
	{
		iv = GlobalVariables.begin();
		delete (*iv);

		GlobalVariables.erase(iv);
	}

	while(ROFFs.size())
	{
		ir = ROFFs.begin();
		if ((*ir).second)
			gi.FS_FreeFile((*ir).second);
		ROFFs.erase(ir);
	}
}

// Save out global variables which are used thruout the level
// but not across levels

void SaveGlobals(void)
{
	list<Variable *>::iterator		it;
	int								count;

	count = GlobalVariables.size();
	gi.AppendToSavegame('SGNM', &count, sizeof(count));

	for(it = GlobalVariables.begin(); it != GlobalVariables.end(); it++)
	{
		(*it)->Write(NULL);
	}
}

void LoadGlobals(void)
{
	int			count, i;
	Variable	*temp;

	gi.ReadFromSavegame('SGNM', &count, sizeof(count));
	
	for(i = 0; i < count; i++)
	{
		temp = (Variable *)RestoreObject(ScriptRL, NULL);
		GlobalVariables.push_back(temp);
	}
}

// Script loading and saving

void SaveLocals(void)
{
	int							ver, size;
	list<CScript *>::iterator	is;

	ver = SCRIPT_SAVE_VERSION;
	gi.AppendToSavegame('SCVR', &ver, sizeof(ver));

	size = Scripts.size();
	gi.AppendToSavegame('SCSZ', &size, sizeof(size));

	for(is = Scripts.begin(); is != Scripts.end(); is++)
	{
		(*is)->Write();
	}
}

void LoadLocals(void)
{
	int		ver, size, i;
	edict_t	*ent;

	gi.ReadFromSavegame('SCVR', (void *)&ver, sizeof(ver));
	if (ver != SCRIPT_SAVE_VERSION)
	{
		gi.error("LoadScripts(): Expecting version %d, found version %d", SCRIPT_SAVE_VERSION, ver);
	}

	for(i = 0, ent = g_edicts; i < globals.num_edicts; i++, ent++)
	{
		ent->Script = NULL;
	}

	gi.ReadFromSavegame('SCSZ', (void *)&size, sizeof(size));
	for(i = 0; i < size; i ++)
	{	
		Scripts.push_back((CScript *)RestoreObject(ScriptRL, NULL));
	}
}

void script_use(edict_t *ent, edict_t *other, edict_t *activator)
{
	// kef -- used to pass in the trigger that fired the script_runner. I have determined that:
	//a) the trigger does not appear to get used and, even if it does, it gets freed soon after anyway
	//b) I need to be able to tell, say, a door where its script_runner is so it opens properly
	//
	//In light of these discoveries I'm replacing 'other' with the script_runner. lemme know if this is dumb.
//	ent->Script->AddEvent(new ExecuteEvent(level.time, other, activator) );
	ent->Script->AddEvent(new ExecuteEvent(level.time, ent, activator) );
}


/*QUAKED script_runner (.5 .5 .5) (-8 -8 -8) (8 8 8) WILL_KILL_USER
set Script to the name of the script to run when triggered
use parm1 through parm16 to send parameters to the script

--------SPAWNFLAGS----------
WILL_KILL_USER: if used from a monster's killtarget, the monster won't kill itself and must be killed from the script
*/
void SP_script_runner (edict_t *ent)
{
	char	temp[MAX_PATH];
	int		i;

	sprintf(temp,"ds/%s.os",st.script);
	ent->Script = new CScript(temp, ent);
	Scripts.push_back(ent->Script);

	for(i=0;i<NUM_PARMS;i++)
	{
		if (st.parms[i])
		{
			ent->Script->SetParameter(st.parms[i]);
		}
		else
		{
			break;
		}
	}

	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_NOT;
	ent->svflags |= SVF_NOCLIENT;
	ent->use = script_use;

//	gi.setmodel (ent, ent->model);
//	gi.linkentity (ent);
}

/*QUAKE script_parms (.5 .5 .5) ?
target the script_runner object
use parm1 through parm16 to send parameters to the script
*/
void SP_parms (edict_t *ent)
{
}

//==========================================================================

Variable *FindGlobal(char *Name)
{
	list<Variable *>::iterator	iv;

	if (GlobalVariables.size())
	{
		for (iv=GlobalVariables.begin();iv != GlobalVariables.end();iv++)
		{
			if (strcmp(Name, (*iv)->GetName()) == 0)
			{
				return *iv;
			}
		}
	}

	return NULL;
}

bool NewGlobal(Variable *Which)
{
	Variable *Check;

	Check = FindGlobal(Which->GetName());
	if (Check)
	{	// already exists
		return false;
	}

	GlobalVariables.push_back(Which);

	return true;
}

//==========================================================================

void *FindROFF(char *FileName)
{
	char							temp[MAX_PATH];
	char							sTemp[MAX_QPATH];		
	map<string, void *>::iterator	ir;
	void							*data;
	int								iFileSize;

	strcpy(temp, FileName);
	ir = ROFFs.find(string(temp));

	if (ir == ROFFs.end())
	{
		sprintf(sTemp,"ds/roff/%s",FileName);
		iFileSize = gi.FS_LoadFile(sTemp, &data);

		if (iFileSize <= 0)
		{
			Com_Printf("***********************************************\n");
			Com_Printf("Could not open .ROF file '%s'\n",FileName);
			Com_Printf("***********************************************\n");

			return NULL;
		}

		ROFFs[string(temp)] = data;
	}
	else
	{
		data = (*ir).second;
	}

	return data;
}

void RemoveROFF(char *FileName)
{
	char							temp[MAX_PATH];
	map<string, void *>::iterator	ir;

	strcpy(temp, FileName);
	ir = ROFFs.find(string(temp));

	if (ir != ROFFs.end())
	{
		if ((*ir).second)
			gi.FS_FreeFile((*ir).second);
		ROFFs.erase(ir);
	}
}

//==========================================================================

Variable::Variable(char *NewName, VariableT NewType)
{
	strncpy(Name,NewName, VAR_LENGTH);
	Type = NewType;
	idx = -1;
}

void Variable::GetIndex(CScript *Script)
{
	idx = -1;
	if (Script)
	{
		idx = Script->LookupVarIndex(this);
	}
}

void Variable::SetIndex(CScript *Script)
{
	if (Script && idx != -1)
	{
		Script->SetVarIndex(idx, this);
	}
}

void Variable::Evaluate(CScript *Script, Variable *orig)
{
	strncpy(Name, orig->Name, VAR_LENGTH);
	Type = orig->Type;
	idx = orig->idx;

	SetIndex(Script);
}

void Variable::Debug(CScript *Script)
{
	Script->DebugLine("   Name: %s\n",Name);
}

//==========================================================================

IntVar::IntVar(char *Name, int InitValue)
:Variable(Name, TypeINT)
{
	Value = InitValue;
}

void IntVar::Evaluate(CScript *Script, IntVar *orig)
{
	Value = orig->Value;

	Variable::Evaluate(Script, orig);
}

IntVar::IntVar(CScript *Script)
{
	char	loaded[sizeof(IntVar)];
				   
	gi.ReadFromSavegame('SINT', loaded, sizeof(IntVar));
	Evaluate(Script, (IntVar *)loaded);
}

void IntVar::Write(CScript *Script)
{
	int		index = RLID_INTVAR;

	gi.AppendToSavegame('SCID', &index, sizeof(index));

	GetIndex(Script);
	gi.AppendToSavegame('SINT', this, sizeof(*this));
}

void IntVar::ReadValue(CScript *Script)
{
	Value = Script->ReadInt();
}

void IntVar::Debug(CScript *Script)
{
	Variable::Debug(Script);

	Script->DebugLine("      Integer Value: %d\n",Value);
}

void IntVar::Signal(edict_t *Which)
{
	Value++;
}

void IntVar::ClearSignal(void)
{
	Value = 0;
}

Variable *IntVar::operator +(Variable *VI)
{
	return new IntVar("",Value + VI->GetIntValue());
}

Variable *IntVar::operator -(Variable *VI)
{
	return new IntVar("",Value - VI->GetIntValue());
}

Variable *IntVar::operator *(Variable *VI)
{
	return new IntVar("",Value * VI->GetIntValue());
}

Variable *IntVar::operator /(Variable *VI)
{
	return new IntVar("",Value / VI->GetIntValue());
}

void IntVar::operator =(Variable *VI)
{
	Value = VI->GetIntValue();
}

//==========================================================================

FloatVar::FloatVar(char *Name, float InitValue)
:Variable(Name, TypeFLOAT)
{
	Value = InitValue;
}

void FloatVar::Evaluate(CScript *Script, FloatVar *orig)
{
	Value = orig->Value;

	Variable::Evaluate(Script, orig);
}

FloatVar::FloatVar(CScript *Script)
{
	char	loaded[sizeof(FloatVar)];

	gi.ReadFromSavegame('SFLT', loaded, sizeof(FloatVar));
	Evaluate(Script, (FloatVar *)loaded);
}

void FloatVar::Write(CScript *Script)
{
	int			index = RLID_FLOATVAR;

	gi.AppendToSavegame('SCID', &index, sizeof(index));

	GetIndex(Script);
	gi.AppendToSavegame('SFLT', this, sizeof(*this));
}

void FloatVar::ReadValue(CScript *Script)
{
	Value = Script->ReadFloat();
}

void FloatVar::Debug(CScript *Script)
{
	Variable::Debug(Script);

	Script->DebugLine("      Float Value: %0.f\n",Value);
}

Variable *FloatVar::operator +(Variable *VI)
{
	return new FloatVar("",Value + VI->GetFloatValue());
}

Variable *FloatVar::operator -(Variable *VI)
{
	return new FloatVar("",Value - VI->GetFloatValue());
}

Variable *FloatVar::operator *(Variable *VI)
{
	return new FloatVar("",Value * VI->GetFloatValue());
}

Variable *FloatVar::operator /(Variable *VI)
{
	return new FloatVar("",Value / VI->GetFloatValue());
}

void FloatVar::operator =(Variable *VI)
{
	Value = VI->GetFloatValue();
}

//==========================================================================

VectorVar::VectorVar(char *Name, float InitValueX, float InitValueY, float InitValueZ)
:Variable(Name, TypeVECTOR)
{
	Value[0] = InitValueX;
	Value[1] = InitValueY;
	Value[2] = InitValueZ;
}

VectorVar::VectorVar(vec3_t NewValue)
:Variable("", TypeVECTOR)
{
	VectorCopy(NewValue, Value);
}

void VectorVar::Evaluate(CScript *Script, VectorVar *orig)
{
	VectorCopy(orig->Value, Value);

	Variable::Evaluate(Script, orig);
}

VectorVar::VectorVar(CScript *Script)
{
	char	loaded[sizeof(VectorVar)];

	gi.ReadFromSavegame('SVEC', loaded, sizeof(VectorVar));
	Evaluate(Script, (VectorVar *)loaded);
}

void VectorVar::Write(CScript *Script)
{
	int			index = RLID_VECTORVAR;

	gi.AppendToSavegame('SCID', &index, sizeof(index));

	GetIndex(Script);
	gi.AppendToSavegame('SVEC', this, sizeof(*this));
}

void VectorVar::GetVectorValue(vec3_t &VecValue) 
{ 
	VecValue[0] = Value[0];
	VecValue[1] = Value[1];
	VecValue[2] = Value[2]; 
}

void VectorVar::ReadValue(CScript *Script)
{
	Value[0] = Script->ReadFloat();
	Value[1] = Script->ReadFloat();
	Value[2] = Script->ReadFloat();
}

void VectorVar::Debug(CScript *Script)
{
	Variable::Debug(Script);

	Script->DebugLine("      Vector Value: [%0.f, %0.f, %0.f]\n",Value[0],Value[1],Value[2]);
}

Variable *VectorVar::operator +(Variable *VI)
{
	vec3_t V2, NewV;

	if (VI->GetType() == TypeINT || VI->GetType() == TypeFLOAT)
	{
		V2[0] = V2[1] = V2[2] = VI->GetFloatValue();
	}
	else
	{
		VI->GetVectorValue(V2);
	}

	NewV[0] = Value[0] + V2[0];
	NewV[1] = Value[1] + V2[1];
	NewV[2] = Value[2] + V2[2];

	return new VectorVar("", NewV[0], NewV[1], NewV[2]);
}

Variable *VectorVar::operator -(Variable *VI)
{
	vec3_t	V2, NewV;

	if (VI->GetType() == TypeINT || VI->GetType() == TypeFLOAT)
	{
		V2[0] = V2[1] = V2[2] = VI->GetFloatValue();
	}
	else
	{
		VI->GetVectorValue(V2);
	}

	NewV[0] = Value[0] - V2[0];
	NewV[1] = Value[1] - V2[1];
	NewV[2] = Value[2] - V2[2];

	return new VectorVar("", NewV[0], NewV[1], NewV[2]);
}

Variable *VectorVar::operator *(Variable *VI)
{
	vec3_t V2, NewV;

	if (VI->GetType() == TypeINT || VI->GetType() == TypeFLOAT)
	{
		V2[0] = V2[1] = V2[2] = VI->GetFloatValue();
	}
	else
	{
	    VI->GetVectorValue(V2);
	}

	NewV[0] = Value[0] * V2[0];
	NewV[1] = Value[1] * V2[1];
	NewV[2] = Value[2] * V2[2];

	return new VectorVar("", NewV[0], NewV[1], NewV[2]);
}

Variable *VectorVar::operator /(Variable *VI)
{
	vec3_t V2, NewV;

	if (VI->GetType() == TypeINT || VI->GetType() == TypeFLOAT)
	{
		V2[0] = V2[1] = V2[2] = VI->GetFloatValue();
	}
	else
	{
	    VI->GetVectorValue(V2);
	}

	NewV[0] = Value[0] / V2[0];
	NewV[1] = Value[1] / V2[1];
	NewV[2] = Value[2] / V2[2];

	return new VectorVar("", NewV[0], NewV[1], NewV[2]);
}

void VectorVar::operator =(Variable *VI)
{
	VI->GetVectorValue(Value);
}

bool VectorVar::operator ==(Variable *VI)
{ 
	vec3_t vec;

	if (VI->GetType() == TypeINT || VI->GetType() == TypeFLOAT)
	{
		return VectorLength(Value) == VI->GetFloatValue();
	}
	else if (VI->GetType() == TypeVECTOR)
	{
		VI->GetVectorValue(vec);

		return (VectorCompare(Value, vec) == 1);	// VC6 gives a warning about converting int to bool
	}
	
	return false;
}

bool VectorVar::operator !=(Variable *VI)
{ 
	vec3_t vec;

	if (VI->GetType() == TypeINT || VI->GetType() == TypeFLOAT)
	{
		return VectorLength(Value) != VI->GetFloatValue();
	}
	else if (VI->GetType() == TypeVECTOR)
	{
		VI->GetVectorValue(vec);

		return !VectorCompare(Value, vec);
	}
	
	return false;
}

bool VectorVar::operator <(Variable *VI)
{ 
	vec3_t	vec;
	float	compare;

	if (VI->GetType() == TypeINT || VI->GetType() == TypeFLOAT)
	{
		compare = VI->GetFloatValue();
	}
	else if (VI->GetType() == TypeVECTOR)
	{
		VI->GetVectorValue(vec);
		compare = VectorLength(vec);
	}
	else
	{
		return false;
	}

	return VectorLength(Value) < compare;
}

bool VectorVar::operator <=(Variable *VI)
{ 
	vec3_t	vec;
	float	compare;

	if (VI->GetType() == TypeINT || VI->GetType() == TypeFLOAT)
	{
		compare = VI->GetFloatValue();
	}
	else if (VI->GetType() == TypeVECTOR)
	{
		VI->GetVectorValue(vec);
		compare = VectorLength(vec);
	}
	else
	{
		return false;
	}

	return VectorLength(Value) <= compare;
}

bool VectorVar::operator >(Variable *VI)
{ 
	vec3_t	vec;
	float	compare;

	if (VI->GetType() == TypeINT || VI->GetType() == TypeFLOAT)
	{
		compare = VI->GetFloatValue();
	}
	else if (VI->GetType() == TypeVECTOR)
	{
		VI->GetVectorValue(vec);
		compare = VectorLength(vec);
	}
	else
	{
		return false;
	}

	return VectorLength(Value) > compare;
}

bool VectorVar::operator >=(Variable *VI)
{ 
	vec3_t	vec;
	float	compare;

	if (VI->GetType() == TypeINT || VI->GetType() == TypeFLOAT)
	{
		compare = VI->GetFloatValue();
	}
	else if (VI->GetType() == TypeVECTOR)
	{
		VI->GetVectorValue(vec);
		compare = VectorLength(vec);
	}
	else
	{
		return false;
	}

	return VectorLength(Value) >= compare;
}

//==========================================================================

EntityVar::EntityVar(char *Name, int InitValue)
:Variable(Name, TypeENTITY)
{
	if (InitValue == -1)
	{
		Value = NULL;
	}
	else
	{
		Value = &g_edicts[InitValue];
	}
}

EntityVar::EntityVar(edict_t *Which)
:Variable("", TypeENTITY)
{
	Value = Which;
}

void EntityVar::Evaluate(CScript *Script, EntityVar *orig)
{
	Value = GetEdictPtr((int)orig->Value);

	Variable::Evaluate(Script, orig);
}

EntityVar::EntityVar(CScript *Script)
{
	char	loaded[sizeof(EntityVar)];

	gi.ReadFromSavegame('SENT', loaded, sizeof(EntityVar));
	Evaluate(Script, (EntityVar *)loaded);
}

void EntityVar::Write(CScript *Script)
{
	int		index = RLID_ENTITYVAR;

	gi.AppendToSavegame('SCID', &index, sizeof(index));

	GetIndex(Script);

	*(int *)&Value = GetEdictNum(Value);
	gi.AppendToSavegame('SENT', this, sizeof(*this));
	Value = GetEdictPtr((int)Value);
}

void EntityVar::ReadValue(CScript *Script)
{
	int Index;

	Index = Script->ReadInt();
	if (Index == -1)
	{
		Value = NULL;
	}
	else
	{
		Value = &g_edicts[Index];
	}
}

void EntityVar::Debug(CScript *Script)
{
	Variable::Debug(Script);

	Script->DebugLine("      Entity Value: %d\n",GetIntValue());
}

int EntityVar::GetIntValue(void)
{
	if (Value)
	{
		return Value - g_edicts;
	}

	return -1;
}

void EntityVar::operator =(Variable *VI)
{
	Value = VI->GetEdictValue();
}

bool EntityVar::operator ==(Variable *VI)
{ 
	if (VI->GetType() == TypeINT)
	{
		return GetIntValue() == VI->GetIntValue();
	}
	else if (VI->GetType() == TypeENTITY)
	{
		return GetEdictValue() == VI->GetEdictValue();
	}
	
	return false;
}

bool EntityVar::operator !=(Variable *VI)
{ 
	if (VI->GetType() == TypeINT)
	{
		return GetIntValue() != VI->GetIntValue();
	}
	else if (VI->GetType() == TypeENTITY)
	{
		return GetEdictValue() != VI->GetEdictValue();
	}
	
	return false;
}

//==========================================================================

StringVar::StringVar(char *Name, char *InitValue)
:Variable(Name, TypeSTRING)
{
	strncpy(Value, InitValue, VAR_LENGTH);
}

void StringVar::Evaluate(CScript *Script, StringVar *orig)
{
	strncpy(Value, orig->Value, VAR_LENGTH);

	Variable::Evaluate(Script, orig);
}

StringVar::StringVar(CScript *Script)
{
	char	loaded[sizeof(StringVar)];

	gi.ReadFromSavegame('SSTR', loaded, sizeof(StringVar));
	Evaluate(Script, (StringVar *)loaded);
}

void StringVar::Write(CScript *Script)
{
	int			index = RLID_STRINGVAR;

	gi.AppendToSavegame('SCID', &index, sizeof(index));

	GetIndex(Script);
	gi.AppendToSavegame('SSTR', this, sizeof(*this));
}

void StringVar::ReadValue(CScript *Script)
{
	strncpy(Value, Script->ReadString(), VAR_LENGTH);
}

//==========================================================================

VariableVar::VariableVar(char *Name)
:Variable(Name, TypeUNKNOWN)
{
	Value = NULL;
}

void VariableVar::Evaluate(CScript *Script, VariableVar *orig)
{
	Value = Script->LookupVar((int)orig->Value);

	Variable::Evaluate(Script, orig);
}

VariableVar::VariableVar(CScript *Script)
{
	char	loaded[sizeof(VariableVar)];

	gi.ReadFromSavegame('SVVR', loaded, sizeof(VariableVar));
	Evaluate(Script, (VariableVar *)loaded);
}

void VariableVar::Write(CScript *Script)
{
	int		index = RLID_VARIABLEVAR;

	gi.AppendToSavegame('SCID', &index, sizeof(index));

	GetIndex(Script);
	*(int *)&Value = Script->LookupVarIndex(Value);
	gi.AppendToSavegame('SVVR', this, sizeof(*this));
	Value = Script->LookupVar((int)Value);
}

void VariableVar::ReadValue(CScript *Script)
{
	int Index;

	Index = Script->ReadInt();

	Value = Script->LookupVar(Index);

	if (Value)
	{
		Type = Value->GetType();
	}
}

void VariableVar::Debug(CScript *Script)
{
	Value->Debug(Script);
}

//==========================================================================

FieldVariableVar::FieldVariableVar(char *Name)
:Variable(Name, TypeUNKNOWN)
{
	Value = NULL;
	Field = NULL;
}

void FieldVariableVar::Evaluate(CScript *Script, FieldVariableVar *orig)
{
	Value = Script->LookupVar((int)orig->Value);
	Field = Script->LookupField((int)orig->Field);

	Variable::Evaluate(Script, orig);
}

FieldVariableVar::FieldVariableVar(CScript *Script)
{
	char	loaded[sizeof(FieldVariableVar)];

	gi.ReadFromSavegame('SFVV', loaded, sizeof(FieldVariableVar));
	Evaluate(Script, (FieldVariableVar *)loaded);
}

void FieldVariableVar::Write(CScript *Script)
{
	int		index = RLID_FIELDVARIABLEVAR;

	gi.AppendToSavegame('SCID', &index, sizeof(index));

	GetIndex(Script);
	*(int *)&Value = Script->LookupVarIndex(Value);
	*(int *)&Field = Script->LookupFieldIndex(Field);
	gi.AppendToSavegame('SFVV', this, sizeof(*this));
	Value = Script->LookupVar((int)Value);
	Field = Script->LookupField((int)Field);
}

void FieldVariableVar::ReadValue(CScript *Script)
{
	int Index;

	Index = Script->ReadInt();
	Value = Script->LookupVar(Index);

	Index = Script->ReadInt();
	Field = Script->LookupField(Index);
}

void FieldVariableVar::Debug(CScript *Script)
{
	Value->Debug(Script);
}

int FieldVariableVar::GetIntValue(void) 
{ 
	return Field->GetIntValue(Value); 
}

float FieldVariableVar::GetFloatValue(void) 
{ 
	return Field->GetFloatValue(Value); 
}

void FieldVariableVar::GetVectorValue(vec3_t &VecValue) 
{ 
	Field->GetVectorValue(Value, VecValue); 
}

edict_t *FieldVariableVar::GetEdictValue(void) 
{ 
	return Field->GetEdictValue(Value); 
}

char *FieldVariableVar::GetStringValue(void) 
{ 
	return Field->GetStringValue(Value); 
}

Variable *FieldVariableVar::operator +(Variable *VI)
{ 
	Variable *Result, *Val;

	Val = Field->GetValue(Value);

	Result = (*Val) + VI;

	delete Val;

	return Result;
}

Variable *FieldVariableVar::operator -(Variable *VI) 
{ 
	Variable *Result, *Val;

	Val = Field->GetValue(Value);

	Result =  (*Val) - VI;

	delete Val;

	return Result;
}

Variable *FieldVariableVar::operator *(Variable *VI) 
{ 
	Variable *Result, *Val;

	Val = Field->GetValue(Value);

	Result = (*Val) * VI;

	delete Val;

	return Result;
}

Variable *FieldVariableVar::operator /(Variable *VI) 
{ 
	Variable *Result, *Val;

	Val = Field->GetValue(Value);

	Result = (*Val) / VI;

	delete Val;

	return Result;
}

void FieldVariableVar::operator =(Variable *VI) 
{ 
	Field->SetValue(Value, VI);
}

bool FieldVariableVar::operator ==(Variable *VI) 
{ 
	Variable	*Val;
	bool		Result;

	Val = Field->GetValue(Value);

	Result = (*Val) == VI;

	delete Val;

	return Result;
}

bool FieldVariableVar::operator !=(Variable *VI) 
{ 
	Variable	*Val;
	bool		Result;

	Val = Field->GetValue(Value);

	Result = (*Val) != VI;

	delete Val;

	return Result;
}

bool FieldVariableVar::operator <(Variable *VI) 
{ 
	Variable	*Val;
	bool		Result;

	Val = Field->GetValue(Value);

	Result = (*Val) < VI;

	delete Val;

	return Result;
}

bool FieldVariableVar::operator <=(Variable *VI) 
{ 
	Variable	*Val;
	bool		Result;

	Val = Field->GetValue(Value);

	Result = (*Val) <= VI;

	delete Val;

	return Result;
}

bool FieldVariableVar::operator >(Variable *VI) 
{ 
	Variable	*Val;
	bool		Result;

	Val = Field->GetValue(Value);

	Result = (*Val) > VI;

	delete Val;

	return Result;
}

bool FieldVariableVar::operator >=(Variable *VI) 
{ 
	Variable	*Val;
	bool		Result;

	Val = Field->GetValue(Value);

	Result = (*Val) >= VI;

	delete Val;

	return Result;
}

//==========================================================================

Signaler::Signaler(edict_t *NewEdict, Variable *NewVar, SignalT NewSignalType)
{
	Edict = NewEdict;
	Var = NewVar;
	SignalType = NewSignalType;
}

void Signaler::Evaluate(CScript *Script, Signaler *orig)
{
	Edict = GetEdictPtr((int)orig->Edict);
	Var = orig->Var;
	SignalType = orig->SignalType;

	if(Var)
	{
		Var = (Variable *)RestoreObject(ScriptRL, Script);
	}
}

Signaler::Signaler(CScript *Script)
{
	char	loaded[sizeof(Signaler)];

	gi.ReadFromSavegame('SSIG', loaded, sizeof(Signaler));
	Evaluate(Script, (Signaler *)loaded);
}

Signaler::~Signaler(void)
{
	if (Var)
	{
		delete Var;
	}
}

void Signaler::Write(CScript *Script)
{
	int			index = RLID_SIGNALER;
	Variable	*tempvar;
			   
	// Write out type Id
	gi.AppendToSavegame('SCID', &index, sizeof(index));

	*(int *)&Edict = GetEdictNum(Edict);
	tempvar = Var;
	Var = (Variable *)!!Var;
	gi.AppendToSavegame('SSIG', this, sizeof(*this));
	Edict = GetEdictPtr((int)Edict);
	Var = tempvar;

	if(Var)
	{
		Var->Write(Script);
	}
}

bool Signaler::Test(edict_t *Which, SignalT WhichType)
{
	if (WhichType != SignalType)
	{
		return false;
	}

	if (Edict != Which)
	{
		return false;
	}
	
	Var->Signal(Which);

	return true;
}

bool Signaler::operator ==(Signaler *SI)
{ 
	if (Var == SI->GetVar())
	{
		return true;
	}

	return false;
}

void script_signaler(edict_t *which, SignalT SignalType)
{
	list<CScript *>::iterator	is;

	if (Scripts.size())
	{
		for (is = Scripts.begin(); is != Scripts.end(); is++)
		{
			(*is)->CheckSignalers(which, SignalType);
		}
	}
}

void move_signaler(edict_t *which)
{
	script_signaler(which, SIGNAL_MOVE);
}

void rotate_signaler(edict_t *which)
{
	script_signaler(which, SIGNAL_ROTATE);
}

void animate_signaler(edict_t *which)
{
	script_signaler(which, SIGNAL_ANIMATE);
}

void moverotate_signaler(edict_t *which)
{
	script_signaler(which, SIGNAL_MOVEROTATE);
}

void helicopter_signaler(edict_t *which)
{
	script_signaler(which, SIGNAL_HELI);
}

void tank_signaler(edict_t *which)
{
	script_signaler(which, SIGNAL_TANK);
}

//==========================================================================

// Fields are just yucky now - once H2 finals, I'm going to change them completely

#define SPEC_X				-1
#define SPEC_Y				-2
#define SPEC_Z				-3
#define SPEC_DELTA_ANGLES	-4
#define SPEC_P_ORIGIN		-5

static field_t script_fields[] =
{
	{ "x",				SPEC_X,							F_FLOAT },
	{ "y",				SPEC_Y,							F_FLOAT },
	{ "z",				SPEC_Z,							F_FLOAT },
	{ "origin",			FOFS(s.origin),					F_VECTOR },
	{ "origin_x",		FOFS(s.origin[0]),				F_FLOAT },
	{ "origin_y",		FOFS(s.origin[1]),				F_FLOAT },
	{ "origin_z",		FOFS(s.origin[2]),				F_FLOAT },
	{ "movetype",		FOFS(movetype),					F_INT },
	{ "start_origin",	FOFS(moveinfo.start_origin),	F_VECTOR },
	{ "distance",		FOFS(moveinfo.distance),		F_FLOAT },
	{ "owner",			FOFS(owner),					F_EDICT },
	{ "wait",			FOFS(wait),						F_FLOAT },
	{ "velocity",		FOFS(velocity),					F_VECTOR },
	{ "angle_velocity",	FOFS(avelocity),				F_VECTOR },
	{ "team_chain",		FOFS(teamchain),				F_EDICT },
	{ "yaw_speed",		FOFS(yaw_speed),				F_FLOAT },
	{ "modelindex",		FOFS(s.modelindex),				F_INT },
	{ "count",			FOFS(count),					F_INT },
	{ "solid",			FOFS(solid),					F_INT },
	{ "angles",			FOFS(s.angles),					F_VECTOR },
	{ "start_angles",	FOFS(moveinfo.start_angles),	F_VECTOR },
	{ "state",			FOFS(moveinfo.state),			F_INT },
	{ "health",			FOFS(health),					F_INT },
	{ "loopsound",		FOFS(s.sound),					F_INT },
	{ NULL,				0,								F_INT }
};

FieldDef::FieldDef(CScript *Script)
{
	field_t *Field;
	bool	Found;

	strncpy(Name, Script->ReadString(), VAR_LENGTH);
	Type = (VariableT)Script->ReadByte();

	FieldType = F_IGNORE;
	Offset = -1;

	Found = false;
	for (Field = script_fields; Field->name; Field++)
	{
		if (strcmp(Name, Field->name) == 0)
		{
			Offset = Field->ofs;
			FieldType = Field->type;
			Found = true;

			break;
		}
	}
	
	if (!Found)
	{
#ifdef _DEVEL
		Com_Printf("Unknown field '%s'\n",Name);
#endif //_DEVEL
	}
}

void FieldDef::Evaluate(CScript *Script, FieldDef *orig)
{
	bool		Found;
	field_t		*Field;

	strncpy(Name, orig->Name, VAR_LENGTH);
	Type = orig->Type;
	Offset = orig->Offset;
	FieldType = orig->FieldType;
	idx = orig->idx;

	if (Script && (idx != -1))
	{
		Script->SetFieldIndex(idx, this);
	}

	FieldType = F_IGNORE;
	Offset = -1;

	Found = false;
	for (Field = script_fields; Field->name; Field++)
	{
		if (strcmp(Name, Field->name) == 0)
		{
			Offset = Field->ofs;
			FieldType = Field->type;
			Found = true;
			break;
		}
	}
	
	if (!Found)
	{
#ifdef _DEVEL
		Com_Printf("Unknown field '%s'\n",Name);
#endif //_DEVEL
	}
}

FieldDef::FieldDef(CScript *Script, bool loading)
{
	char	loaded[sizeof(FieldDef)];

	gi.ReadFromSavegame('SFDF', loaded, sizeof(FieldDef));
	Evaluate(Script, (FieldDef *)loaded);
}

void FieldDef::Write(CScript *Script)
{
	int index = RLID_FIELDDEF;

	// Write out type Id
	gi.AppendToSavegame('SCID', &index, sizeof(index));

	idx = -1;
	if (Script)
	{
		idx = Script->LookupFieldIndex(this);
	}
	gi.AppendToSavegame('SFDF', this, sizeof(*this));
}

byte *FieldDef::GetOffset(Variable *Var)
{
	edict_t	*ent;
	byte	*b, *Dest;

	Dest = NULL;

	switch(Offset)
	{
		case SPEC_X:
			break;
		case SPEC_Y:
			break;
		case SPEC_Z:
			break;
		case SPEC_DELTA_ANGLES:
			ent = Var->GetEdictValue();
			if (ent && ent->client)
			{
				Dest = (byte *)&ent->client->ps.pmove.delta_angles;
			}
			break;
		default:
			ent = Var->GetEdictValue();
			if (ent)
			{
				b = (byte *)ent;
				Dest = b+Offset;
			}
			break;
	}

	return Dest;
}

Variable *FieldDef::GetValue(Variable *Var)
{
	vec3_t vec;

	switch(FieldType)
	{
		case F_INT:
			return new IntVar("", GetIntValue(Var) );
			break;

		case F_FLOAT:
			return new FloatVar("", GetFloatValue(Var) );
			break;

		case F_EDICT:
			return new EntityVar(GetEdictValue(Var));
			break;

		case F_VECTOR:
			GetVectorValue(Var, vec);
			return new VectorVar(vec);
			break;
	}

	return NULL;
}

int FieldDef::GetIntValue(Variable *Var)
{
	byte	*Dest;
	vec3_t	data;

	Dest = GetOffset(Var);

 	if (FieldType != F_INT || !Dest) 
	{
		switch(Offset)
		{
			case SPEC_X:
				Var->GetVectorValue(data);
				return (int)data[0];
				break;
			case SPEC_Y:
				Var->GetVectorValue(data);
				return (int)data[1];
				break;
			case SPEC_Z:
				Var->GetVectorValue(data);
				return (int)data[2];
				break;
		}

		return 0.0;
	}

	return *(int *)(Dest);
}

float FieldDef::GetFloatValue(Variable *Var)
{
	byte	*Dest;
	vec3_t	data;

	Dest = GetOffset(Var);

	if (FieldType != F_FLOAT || !Dest)
	{
		switch(Offset)
		{
			case SPEC_X:
				Var->GetVectorValue(data);
				return data[0];
				break;
			case SPEC_Y:
				Var->GetVectorValue(data);
				return data[1];
				break;
			case SPEC_Z:
				Var->GetVectorValue(data);
				return data[2];
				break;
		}

		return 0.0;
	}

	return *(float *)(Dest);
}

void FieldDef::GetVectorValue(Variable *Var, vec3_t &VecValue)
{
	byte	*Dest;

	Dest = GetOffset(Var);

	if (FieldType != F_VECTOR || !Dest)
	{
		VectorClear(VecValue);
		return;
	}

	VectorCopy(*(vec3_t *)(Dest), VecValue);
}

edict_t *FieldDef::GetEdictValue(Variable *Var)
{
	byte	*Dest;

	Dest = GetOffset(Var);

	if (FieldType != F_EDICT || !Dest)
	{
		return NULL;
	}

	return *(edict_t **)(Dest);
}

char *FieldDef::GetStringValue(Variable *Var)
{
	return "";
}

void FieldDef::SetValue(Variable *Var, Variable *Value)
{
	byte		*Dest;
	vec3_t		data;
	VectorVar	*new_var;

	Dest = GetOffset(Var);
	if (Dest == NULL)
	{
		switch(Offset)
		{
			case SPEC_X:
				Var->GetVectorValue(data);
				data[0] = Value->GetFloatValue();
				new_var = new VectorVar(data);
				*Var = new_var;
				delete new_var;
				break;
			case SPEC_Y:
				Var->GetVectorValue(data);
				data[1] = Value->GetFloatValue();
				new_var = new VectorVar(data);
				*Var = new_var;
				delete new_var;
				break;
			case SPEC_Z:
				Var->GetVectorValue(data);
				data[2] = Value->GetFloatValue();
				new_var = new VectorVar(data);
				*Var = new_var;
				delete new_var;
				break;
		}

		return;
	}
	
	switch(FieldType)
	{
		case F_INT:
			*(int *)(Dest) = Value->GetIntValue();
			break;
		case F_FLOAT:
			*(float *)(Dest) = Value->GetFloatValue();
			break;
		case F_EDICT:
			*(edict_t **)(Dest) = Value->GetEdictValue();
			break;
		case F_VECTOR:
			Value->GetVectorValue(*(vec3_t *)(Dest));
			break;
	}
}

//==========================================================================

Event::Event(float NewTime, EventT NewType)
{
	Time = floor((NewTime + 0.05) * 10) / 10;		// avoids stupid math rounding errors
	Type = NewType;
}

void Event::Evaluate(CScript *Script, Event *orig)
{
	Time = orig->Time;   
	Type = orig->Type;   
	Priority = orig->Priority;
}

bool Event::Process(CScript *Script)
{
	return FALSE;
}

//==========================================================================

MoveRotateEvent::MoveRotateEvent(float NewTime, edict_t *NewEnt, vec3_t v3RelOrigin, vec3_t v3RelAngles)
:Event(NewTime, EVENT_MOVEROTATE)
{
	Ent = NewEnt;

	Priority = 10;

	VectorCopy(v3RelOrigin, v3RelativeOrigin);
	VectorCopy(v3RelAngles, v3RelativeAngles);
}

void MoveRotateEvent::Evaluate(CScript *Script, MoveRotateEvent *orig)
{
	Ent = GetEdictPtr((int)orig->Ent);
	VectorCopy(orig->v3RelativeOrigin, v3RelativeOrigin);
	VectorCopy(orig->v3RelativeAngles, v3RelativeAngles);

	Event::Evaluate(Script, orig);
}

MoveRotateEvent::MoveRotateEvent(CScript *Script)
{
	char	loaded[sizeof(MoveRotateEvent)];

	gi.ReadFromSavegame('SMRE', loaded, sizeof(MoveRotateEvent));
	Evaluate(Script, (MoveRotateEvent *)loaded);
}

void MoveRotateEvent::Write(CScript *Script)
{
	int	index = RLID_MOVEROTATEEVENT;

	// Write out type Id
	gi.AppendToSavegame('SCID', &index, sizeof(index));
	// Write out data
	*(int *)&Ent = GetEdictNum(Ent);
	gi.AppendToSavegame('SMRE', this, sizeof(*this));
	Ent = GetEdictPtr((int)Ent);
}

bool MoveRotateEvent::Process(CScript *Script)
{
	if (level.time < Time)
	{
		return FALSE;
	}			   

	// special code, because "moverotate" command pushes a whole bunch of commands on the stack, I'll do the signalling
	//	by checking for specially-pushed values. Checking if the relative rotations are 10000 (ROFF_EOS_MARKER) should be ok.
	//
	if (VectorCompare(v3RelativeOrigin, v3ROFF_EOS_Marker))
	{
		// end-of-sequence...
		//
		moverotate_signaler(Ent);		
	}
	else
	{
		// normal data...
		//
		Vec3AddAssign(v3RelativeOrigin, Ent->s.origin);
		Vec3AddAssign(v3RelativeAngles, Ent->s.angles);
	}
	gi.linkentity(Ent);

	return TRUE;	
}
//==========================================================================

AnimateDoneEvent::AnimateDoneEvent(edict_t *myEnt)
:Event(0, EVENT_ANIMATE_DONE)
{
	Done = false;
	Ent = myEnt;

	Priority = 10;
}

void AnimateDoneEvent::Evaluate(CScript *Script, AnimateDoneEvent *orig)
{
	Done = orig->Done;
	Ent = GetEdictPtr((int)orig->Ent);

	Event::Evaluate(Script, orig);
}

AnimateDoneEvent::AnimateDoneEvent(CScript *Script)
{
	char	loaded[sizeof(AnimateDoneEvent)];

	gi.ReadFromSavegame('SADE', loaded, sizeof(AnimateDoneEvent));
	Evaluate(Script, (AnimateDoneEvent *)loaded);
}

void AnimateDoneEvent::Write(CScript *Script)
{
	int	index = RLID_ANIMATEDONEEVENT;

	// Write out type Id
	gi.AppendToSavegame('SCID', &index, sizeof(index));
	// Write out data
	*(int *)&Ent = GetEdictNum(Ent);
	gi.AppendToSavegame('SADE', this, sizeof(*this));
	Ent = GetEdictPtr((int)Ent);
}

bool AnimateDoneEvent::Process(CScript *Script)
{
	if (Done)
	{
		animate_signaler(Ent);
		return TRUE;
	}

	return FALSE;
}

//==========================================================================

MoveDoneEvent::MoveDoneEvent(float NewTime, edict_t *NewEnt)
:Event(NewTime, EVENT_MOVE_DONE)
{
	Ent = NewEnt;

	Priority = 10;
}

void MoveDoneEvent::Evaluate(CScript *Script, MoveDoneEvent *orig)
{
	Ent = GetEdictPtr((int)orig->Ent);

	Event::Evaluate(Script, orig);
}

MoveDoneEvent::MoveDoneEvent(CScript *Script)
{
	char	loaded[sizeof(MoveDoneEvent)];

	gi.ReadFromSavegame('SMDE', loaded, sizeof(MoveDoneEvent));
	Evaluate(Script, (MoveDoneEvent *)loaded);
}

void MoveDoneEvent::Write(CScript *Script)
{
	int	index = RLID_MOVEDONEEVENT;

	// Write out type Id
	gi.AppendToSavegame('SCID', &index, sizeof(index));
	// Write out data
	*(int *)&Ent = GetEdictNum(Ent);
	gi.AppendToSavegame('SMDE', this, sizeof(*this));
	Ent = GetEdictPtr((int)Ent);
}

bool MoveDoneEvent::Process(CScript *Script)
{
	if (level.time < Time)
	{
		return FALSE;
	}

	Script->Move_Done(Ent);
	move_signaler(Ent);

	return TRUE;
}

//==========================================================================

RotateDoneEvent::RotateDoneEvent(float NewTime, edict_t *NewEnt)
:Event(NewTime, EVENT_ROTATE_DONE)
{
	Ent = NewEnt;

	Priority = 10;
}

void RotateDoneEvent::Evaluate(CScript *Script, RotateDoneEvent *orig)
{
	Ent = GetEdictPtr((int)orig->Ent);

	Event::Evaluate(Script, orig);
}

RotateDoneEvent::RotateDoneEvent(CScript *Script)
{
	char	loaded[sizeof(RotateDoneEvent)];

	gi.ReadFromSavegame('SRDE', loaded, sizeof(RotateDoneEvent));
	Evaluate(Script, (RotateDoneEvent *)loaded);
}

void RotateDoneEvent::Write(CScript *Script)
{
	int	index = RLID_ROTATEDONEEVENT;

	// Write out type Id
	gi.AppendToSavegame('SCID', &index, sizeof(index));
	// Write out data
	*(int *)&Ent = GetEdictNum(Ent);
	gi.AppendToSavegame('SRDE', this, sizeof(*this));
	Ent = GetEdictPtr((int)Ent);
}

bool RotateDoneEvent::Process(CScript *Script)
{
	if (level.time < Time)
	{
		return FALSE;
	}

	Script->Rotate_Done(Ent);
	rotate_signaler(Ent);

	return TRUE;
}

//==========================================================================

HelicopterDoneEvent::HelicopterDoneEvent(float NewTime, edict_t *NewEnt, int actionID)
:Event(NewTime, EVENT_HELICOPTER_DONE)
{
	Ent = NewEnt;
	m_ID = actionID;

	Priority = 10;
}

void HelicopterDoneEvent::Evaluate(CScript *Script, HelicopterDoneEvent *orig)
{
	Ent = GetEdictPtr((int)orig->Ent);
	m_ID = orig->m_ID;
	Priority = orig->Priority;

	Event::Evaluate(Script, orig);
}

HelicopterDoneEvent::HelicopterDoneEvent(CScript *Script)
{
	char	loaded[sizeof(HelicopterDoneEvent)];

	gi.ReadFromSavegame('SHDE', &loaded, sizeof(HelicopterDoneEvent));
	Evaluate(Script, (HelicopterDoneEvent *)loaded);
}

void HelicopterDoneEvent::Write(CScript *Script)
{
	int	index = RLID_HELICOPTERDONEEVENT;

	// Write out type Id
	gi.AppendToSavegame('SCID', &index, sizeof(index));
	// Write out data
	*(int *)&Ent = GetEdictNum(Ent);
	gi.AppendToSavegame('SHDE', this, sizeof(*this));
	Ent = GetEdictPtr((int)Ent);
}

bool HelicopterDoneEvent::Process(CScript *Script)
{
	ai_public_c*				ai = Ent->ai;
	int							nID = -1, nLastID = -1;
	
	if (ai)
	{
		// if nID comes back 0, there is no current action
		nID = ((generic_ghoul_heli_ai*)ai)->GetCurrentActionID();
		// if nLastID comes back 0, the ai has not yet called an action's think fn
		nLastID = ((generic_ghoul_heli_ai*)ai)->GetLastThinkingActionID();

		if (0 == nID)
		{ // no current action. that might mean we just finished one, though.
			if (0 == nLastID)
			{ // haven't thought about one yet.
				return FALSE;
			}
			else
			{ // we just finished an action. if it wasn't this action, return FALSE
				if (nLastID != m_ID)
				{
					return FALSE;
				}
				else
				{ // fall through cuz we just finished this action
				}
			}
		}
		else 
		{ // currently thinking about an action and it hasn't finished yet
			return FALSE;
		}
	}

	Script->Helicopter_Done(Ent);
	helicopter_signaler(Ent);

	return TRUE;
}

//==========================================================================

TankDoneEvent::TankDoneEvent(float NewTime, edict_t *NewEnt, int actionID)
:Event(NewTime, EVENT_TANK_DONE)
{
	Ent = NewEnt;
	m_ID = actionID;

	Priority = 10;
}

void TankDoneEvent::Evaluate(CScript *Script, TankDoneEvent *orig)
{
	Ent = GetEdictPtr((int)orig->Ent);
	m_ID = orig->m_ID;
	Priority = orig->Priority;

	Event::Evaluate(Script, orig);
}

TankDoneEvent::TankDoneEvent(CScript *Script)
{
	char	loaded[sizeof(TankDoneEvent)];

	gi.ReadFromSavegame('STDE', loaded, sizeof(TankDoneEvent));
	Evaluate(Script, (TankDoneEvent *)loaded);
}

void TankDoneEvent::Write(CScript *Script)
{
	int	index = RLID_TANKDONEEVENT;

	// Write out type Id
	gi.AppendToSavegame('SCID', &index, sizeof(index));
	// Write out data
	*(int *)&Ent = GetEdictNum(Ent);
	gi.AppendToSavegame('STDE', this, sizeof(*this));
	Ent = GetEdictPtr((int)Ent);
}

bool TankDoneEvent::Process(CScript *Script)
{
	ai_public_c*				ai = Ent->ai;
	int							nID = -1, nLastID = -1;
	
	if (ai)
	{
		// if nID comes back 0, there is no current action
		nID = ((generic_ghoul_tank_ai*)ai)->GetCurrentActionID();
		// if nLastID comes back 0, the ai has not yet called an action's think fn
		nLastID = ((generic_ghoul_tank_ai*)ai)->GetLastThinkingActionID();

		if (0 == nID)
		{ // no current action. that might mean we just finished one, though.
			if (0 == nLastID)
			{ // haven't thought about one yet.
				return FALSE;
			}
			else
			{ // we just finished an action. if it wasn't this action, return FALSE
				if (nLastID != m_ID)
				{
					return FALSE;
				}
				else
				{ // fall through cuz we just finished this action
				}
			}
		}
		else 
		{ // currently thinking about an action and it hasn't finished yet
			return FALSE;
		}
	}

	Script->Tank_Done(Ent);
	tank_signaler(Ent);

	return TRUE;
}

//==========================================================================

ExecuteEvent::ExecuteEvent(float NewTime, edict_t *NewOther, edict_t *NewActivator)
:Event(NewTime, EVENT_SCRIPT_EXECUTE)
{
	Other = NewOther;
	Activator = NewActivator;

	Priority = 0;
}

void ExecuteEvent::Evaluate(CScript *Script, ExecuteEvent *orig)
{
	Other = GetEdictPtr((int)orig->Other);
	Activator = GetEdictPtr((int)orig->Activator);
	Priority = orig->Priority;

	Event::Evaluate(Script, orig);
}

ExecuteEvent::ExecuteEvent(CScript *Script)
{
	char	loaded[sizeof(ExecuteEvent)];

	gi.ReadFromSavegame('SEEX', loaded, sizeof(ExecuteEvent));
	Evaluate(Script, (ExecuteEvent *)loaded);
}

void ExecuteEvent::Write(CScript *Script)
{
	int	index = RLID_EXECUTEEVENT;

	// Write out type Id
	gi.AppendToSavegame('SCID', &index, sizeof(index));
	// Write out data
	*(int *)&Other = GetEdictNum(Other);
	*(int *)&Activator = GetEdictNum(Activator);
	gi.AppendToSavegame('SEEX', this, sizeof(*this));
	Other = GetEdictPtr((int)Other);
	Activator = GetEdictPtr((int)Activator);
}

bool ExecuteEvent::Process(CScript *Script)
{
	if (level.time < Time)
	{
		return FALSE;
	}

	if (Script->CheckWait())
	{
		Script->Execute(Other,Activator);
	}

	return TRUE;
}

//==========================================================================

WaitEvent::WaitEvent(float NewTime)
:Event(NewTime, EVENT_SCRIPT_WAIT)
{
	Priority = 0;
}

void WaitEvent::Evaluate(CScript *Script, WaitEvent *orig)
{
	Priority = orig->Priority;

	Event::Evaluate(Script, orig);
}

WaitEvent::WaitEvent(CScript *Script)
{
	char	loaded[sizeof(WaitEvent)];

	gi.ReadFromSavegame('SWEV', loaded, sizeof(WaitEvent));
	Evaluate(Script, (WaitEvent *)loaded);
}

void WaitEvent::Write(CScript *Script)
{
	int	index = RLID_WAITEVENT;

	// Write out type Id
	gi.AppendToSavegame('SCID', &index, sizeof(index));
	// Write out data
	gi.AppendToSavegame('SWEV', this, sizeof(*this));
}

bool WaitEvent::Process(CScript *Script)
{
	if (level.time < Time)
	{
		return FALSE;
	}

	Script->ClearTimeWait();

	if (Script->CheckWait())
	{
		Script->Execute(NULL,NULL);
	}

	return TRUE;
}

//==========================================================================

CScript::CScript(char *ScriptName, edict_t *new_owner)
{
	Clear(true);

	owner = new_owner;
	strncpy(Name, ScriptName, MAX_PATH);

	LoadFile();
}

void CScript::Write(void)
{							
	int							index = RLID_SCRIPT;
	int							size;
	saveglobal_t				sg;
	list<Variable *>::iterator	iv;
	list<Signaler *>::iterator	is;
	list<StringVar *>::iterator	isv;
	list<Event *>::iterator		iev;
	list<int>::iterator			ic;
	int							*temp;
	int							i;
	savescript_t				ss;

	// Save out script ID
	gi.AppendToSavegame('SCID', &index, sizeof(index));

	// Save out name
	gi.AppendToSavegame('SNAM', Name, sizeof(Name));

	// Save out regular script data
	ss.ScriptCondition = ScriptCondition;
	ss.ConditionInfo = ConditionInfo;
	ss.Length = Length;
	ss.Position = Position;
	ss.DebugFlags = DebugFlags;
	ss.owner = GetEdictNum(owner);
	ss.other = GetEdictNum(other);
	ss.activator = GetEdictNum(activator);

	gi.AppendToSavegame('SCRP', &ss, sizeof(ss));

	// Save out field array
	size = 0;
	for(i = 0, size = 0; i < MAX_INDEX; i++)
	{
		if (Fields[i])
		{
			size++;
		}
	}
	gi.AppendToSavegame('SFNM', &size, sizeof(size));

	for(i = 0; i < MAX_INDEX; i++)
	{
		if (Fields[i])
		{
			Fields[i]->Write(this);
		}
	}

	// Saving out global variables referenced in this script
	size = 0;
	for (iv = GlobalVariables.begin(); iv != GlobalVariables.end(); iv++)
	{
		if (LookupVarIndex(*iv) != -1)
		{
			size++;
		}
	}
	gi.AppendToSavegame('SGNM', &size, sizeof(size));

	for (iv = GlobalVariables.begin(); iv != GlobalVariables.end(); iv++)
	{
		index = LookupVarIndex(*iv);
		if (index != -1)
		{
			sg.index = index;
			strcpy(sg.name, (*iv)->GetName());

			gi.AppendToSavegame('SGLB', &sg, sizeof(sg));
		}
	}

	// Save out local variables
	size = LocalVariables.size();
	gi.AppendToSavegame('SLNM', &size, sizeof(size));

	for (iv = LocalVariables.begin(); iv != LocalVariables.end(); iv++)
	{
		(*iv)->Write(this);
	}

	// Save out parameters
	size = ParameterVariables.size();
	gi.AppendToSavegame('SPNM', &size, sizeof(size));

	for (iv = ParameterVariables.begin(); iv != ParameterVariables.end(); iv++)
	{
		(*iv)->Write(this);
	}

	// Save out stack
	size = Stack.size();
	gi.AppendToSavegame('SSNM', &size, sizeof(size));

	for (iv = Stack.begin(); iv != Stack.end(); iv++)
	{
		(*iv)->Write(this);
	}

	// Save out waiting 
	size = Waiting.size();
	gi.AppendToSavegame('SWNM', &size, sizeof(size));

	for (iv = Waiting.begin(); iv != Waiting.end(); iv++)
	{
		(*iv)->Write(this);
	}

	// Save out signalers
	size = Signalers.size();
	gi.AppendToSavegame('SINM', &size, sizeof(size));

	for (is = Signalers.begin(); is != Signalers.end(); is++)
	{
		(*is)->Write(this);
	}

	// Save out parameters
	size = ParameterValues.size();
	gi.AppendToSavegame('SVNM', &size, sizeof(size));

	for (isv = ParameterValues.begin(); isv != ParameterValues.end(); isv++)
	{
		(*isv)->Write(this);
	}

	// Save out events
	size = Events.size();
	gi.AppendToSavegame('SENM', &size, sizeof(size));

	for (iev = Events.begin(); iev != Events.end(); iev++)
	{
		(*iev)->Write(this);
	}

	// Save out conditions
	size = OnConditions.size();
	gi.AppendToSavegame('SCNM', &size, sizeof(size));
	
	temp = new int [size];
	for(ic = OnConditions.begin(), i = 0; ic != OnConditions.end(); ic++, i++)
	{
		temp[i] = *ic;
	}
	gi.AppendToSavegame('SCND', temp, sizeof(int) * size);
	delete [] temp;
}

CScript::CScript(void)
{
	int						size;
	int						i;
	int						*temp;
	savescript_t			ss;
	saveglobal_t			sg;

	Clear(true);

	// Read in name and load in script
	gi.ReadFromSavegame('SNAM', Name, sizeof(Name));
	LoadFile();

	// Read in regular scritp data
	gi.ReadFromSavegame('SCRP', &ss, sizeof(ss));

	ScriptCondition = ss.ScriptCondition;
	ConditionInfo = ss.ConditionInfo;
	Length = ss.Length;
	Position = ss.Position;
	DebugFlags = ss.DebugFlags;
	owner = GetEdictPtr(ss.owner);
	other = GetEdictPtr(ss.other);
	activator = GetEdictPtr(ss.activator);

	// fields - they'll put themselves in
	gi.ReadFromSavegame('SFNM', &size, sizeof(size));
	for(i = 0; i < size; i++)
	{
		RestoreObject(ScriptRL, this);
	}

	// Read in any used global vars
	gi.ReadFromSavegame('SGNM', &size, sizeof(size));
	for (i = 0; i < size; i++)
	{
		gi.ReadFromSavegame('SGLB', &sg, sizeof(sg));
		VarIndex[sg.index] = FindGlobal(sg.name);
	}

	// Read in local variables
	gi.ReadFromSavegame('SLNM', &size, sizeof(size));
	for(i = 0; i < size; i++)
	{
		LocalVariables.push_back((Variable *)RestoreObject(ScriptRL, this));
	}

	// Read in parameters
	gi.ReadFromSavegame('SPNM', &size, sizeof(size));
	for(i = 0; i < size; i++)
	{
		ParameterVariables.push_back((Variable *)RestoreObject(ScriptRL, this));
	}

	// Read in stack
	gi.ReadFromSavegame('SSNM', &size, sizeof(size));
	for(i = 0; i < size; i++)
	{
		Stack.push_back((Variable *)RestoreObject(ScriptRL, this));
	}

	// Read in waiting
	gi.ReadFromSavegame('SWNM', &size, sizeof(size));
	for(i = 0; i < size; i++)
	{
		Waiting.push_back((Variable *)RestoreObject(ScriptRL, this));
	}

	// Read in signalers
	gi.ReadFromSavegame('SINM', &size, sizeof(size));
	for(i = 0; i < size; i++)
	{
		Signalers.push_back((Signaler *)RestoreObject(ScriptRL, this));
	}

	// Read in parameters
	gi.ReadFromSavegame('SVNM', &size, sizeof(size));
	for(i = 0; i < size; i++)
	{
		ParameterValues.push_back((StringVar *)RestoreObject(ScriptRL, this));
	}

	// Read in events
	gi.ReadFromSavegame('SENM', &size, sizeof(size));
	for(i = 0; i < size; i++)
	{
		Events.push_back((Event *)RestoreObject(ScriptRL, this));
	}

	// Read in conditions
	gi.ReadFromSavegame('SCNM', &size, sizeof(size));
	temp = new int [size];
	gi.ReadFromSavegame('SCND', temp, sizeof(int) * size);

	for(i = 0; i < size; i++)
	{
		OnConditions.push_back(temp[i]);
	}
	delete [] temp;

	// Patch in script to owner
	owner->Script = this;
}

CScript::~CScript(void)
{
	Free(true);
}

void CScript::LoadFile(void)
{
	int Version;

	Length = gi.FS_LoadFile(Name, (void **)&Data);
	if (Length == -1)
	{
		Com_Printf(P_RED "***********************************************\n");
		Com_Printf(P_RED "Could not open script %s\n", Name);
		Com_Printf(P_RED "***********************************************\n");
	}
	else
	{
		Version = ReadInt();

		if (Version != SCRIPT_VERSION)
		{
			Com_Printf(P_RED "***********************************************\n");
			Com_Printf(P_RED "Bad script version for %s: found %d, expecting %d\n", Name, Version, SCRIPT_VERSION);
			Com_Printf(P_RED "***********************************************\n");
		}
		else
		{
			ScriptCondition = COND_READY;
		}
	}
}

void CScript::Free(bool DoData)
{
	int						i;
	list<Variable *>::iterator	iv;
	list<Signaler *>::iterator	is;
	list<StringVar *>::iterator	isv;
	list<Event *>::iterator		iev;

	if (Data && DoData)
	{
		gi.FS_FreeFile(Data);
		Data = NULL;
	}

	while(LocalVariables.size())
	{
		iv=LocalVariables.begin();
		delete (*iv);

		LocalVariables.erase(iv);
	}

	while(ParameterVariables.size())
	{
		iv=ParameterVariables.begin();
		delete (*iv);

		ParameterVariables.erase(iv);
	}

	while(Stack.size())
	{
		iv=Stack.begin();
		delete (*iv);

		Stack.erase(iv);
	}

	while(Waiting.size())
	{
		iv=Waiting.begin();
		delete (*iv);

		Waiting.erase(iv);
	}

	while(Signalers.size())
	{
		is=Signalers.begin();
		delete (*is);

		Signalers.erase(is);
	}

	while(ParameterValues.size())
	{
		isv=ParameterValues.begin();
		delete (*isv);

		ParameterValues.erase(isv);
	}

	while(Events.size())
	{
		iev=Events.begin();
		delete (*iev);

		Events.erase(iev);
	}

	OnConditions.clear();

	for(i=0;i<MAX_INDEX;i++)
	{
		if (Fields[i])
		{
			delete Fields[i];
		}
	}

	Clear(DoData);
}

void CScript::Clear(bool DoData)
{
	if (DoData)
	{
		Data = NULL;
	}

	owner = other = activator = NULL;

	memset(Fields, 0, sizeof(Fields));
	memset(VarIndex, 0, sizeof(VarIndex));

	DebugFlags = 0;
	memset(Name, 0, sizeof(Name));

	ScriptCondition = COND_COMPLETED;
	ConditionInfo = 0;
	Data = NULL;
	Position = 0;
	Length = 0;

	ClearEvents = false;
}

int CScript::GetEventNum(Event *ev)
{
	int							idx;
	list<Event *>::iterator		it;

	for(it = Events.begin(), idx = 0; it != Events.end(); it++, idx++)
	{
		if((*it) == ev)
		{
			return(idx);
		}
	}
	return(-1);
}

Event *CScript::GetEventPtr(int ev)
{
	list<Event *>::iterator		it;
	int							i;

	if(ev >= Events.size())
	{
		return(NULL);
	}
	it = Events.begin();
	for(i = 0; i < ev; i++)
	{
		it++;
	}
	return((*it));
}

int CScript::LookupVarIndex(Variable *Var) 
{
	int i;

	for(i=0;i<MAX_INDEX;i++)
	{
		if (VarIndex[i] == Var)
		{
			return i;
		}
	}

	return -1;
}

int	CScript::LookupFieldIndex(FieldDef *Field)
{
	int i;

	for(i=0;i<MAX_INDEX;i++)
	{
		if (Fields[i] == Field)
		{
			return i;
		}
	}

	return -1;
}

void CScript::SetParameter(char *Value)
{
	ParameterValues.push_back(new StringVar("parm",Value));
}

unsigned char CScript::ReadByte(void)
{
	return Data[Position++];
}

int CScript::ReadInt(void)
{
	union
	{
		int				oldvalue;
		unsigned char	newvalue[4];
	};

	newvalue[0] = ReadByte();
	newvalue[1] = ReadByte();
	newvalue[2] = ReadByte();
	newvalue[3] = ReadByte();

	return oldvalue;
}

float CScript::ReadFloat(void)
{
	union
	{
		float			oldvalue;
		unsigned char	newvalue[4];
	};

	newvalue[0] = ReadByte();
	newvalue[1] = ReadByte();
	newvalue[2] = ReadByte();
	newvalue[3] = ReadByte();

	return oldvalue;
}

char *CScript::ReadString(void)
{
	char *Pos;

	Pos = (char *)&Data[Position];

	while(ReadByte())
	{
	}

	return Pos;
}

Variable *CScript::ReadDeclaration(int &Index)
{
	int			Type;
	char		*Name;
	Variable	*RetVal;

	Name = ReadString();
	Type = ReadByte();
	Index = ReadInt();

	RetVal = NULL;
	switch(Type)
	{
		case TypeINT:
			RetVal =  new IntVar(Name);
			break;
		case TypeFLOAT:
			RetVal =  new FloatVar(Name);
			break;
		case TypeVECTOR:
			RetVal =  new VectorVar(Name);
			break;
		case TypeENTITY:
			RetVal =  new EntityVar(Name);
			break;
		case TypeSTRING:
			RetVal =  new StringVar(Name);
			break;
		case TypeUNKNOWN:
			break;
	}

	if (Index >= MAX_INDEX)
	{
		Error("Index out of range: %d > %d",Index,MAX_INDEX);
	}

	VarIndex[Index] = RetVal;

	return RetVal;
}

void CScript::PushStack(Variable *VI)
{
	if (!VI)
	{
		Error("Illegal push");
	}

	Stack.push_back(VI);
}

Variable *CScript::PopStack(void)
{
	Variable *Value;
	list<Variable *>::iterator	iv;

	if (Stack.size())
	{
		iv = --Stack.end();
		Value = *iv;
		Stack.pop_back();

		return Value;
	}

	return NULL;
}

void CScript::HandleGlobal(bool Assignment)
{
	Variable	*Var;
	int			Index;

	Var = ReadDeclaration(Index);

	if (Assignment)
	{
		Var->ReadValue(this);
	}

	if (!NewGlobal(Var))
	{
		VarIndex[Index] = FindGlobal(Var->GetName());

		delete Var;
	}
}

void CScript::HandleLocal(bool Assignment)
{
	Variable	*Var;
	int			Index;

	Var = ReadDeclaration(Index);

	if (Assignment)
	{
		Var->ReadValue(this);
	}

	NewLocal(Var);
}

void CScript::HandleParameter(bool Assignment)
{
	Variable	*Var;
	int			Index;

	Var = ReadDeclaration(Index);

	if (Assignment)
	{
		Var->ReadValue(this);
	}

	NewParameter(Var);
}

void CScript::HandleField(void)
{
	int			Index;
	FieldDef	*NewField;

	NewField = new FieldDef(this);

	Index = ReadInt();
	if (Index < 0 || Index >= MAX_INDEX)
	{
		Error("Index for field out of range: %d > %d\n",Index,MAX_INDEX);
	}

	Fields[Index] = NewField;
}

void CScript::HandleGoto(void)
{
	Position = ReadInt();
}

Variable *CScript::HandleSpawn(void)
{
	int			Count;
	edict_t		*ent;
	Variable	*Name;
	Variable	*Value;
	field_t		*f;
	const char	*NameValue;
	byte		*b;

	ent = G_Spawn();

	for(Count = ReadByte(); Count; Count--)
	{
		Name = PopStack();
		Value = PopStack();
		if (!Name || !Value)
		{
			Error("Invalid stack for HandleSpawn()");
		}

		NameValue = Name->GetStringValue();

		for (f=fields ; f->name ; f++)
		{
			if (!stricmp(f->name, (char *)NameValue) )
			{
				if (f->flags & FFL_SPAWNTEMP)
				{
					b = (byte *)&st;
				}
				else
				{
					b = (byte *)ent;
				}

				switch (f->type)
				{
					case F_STRING:
						*(char **)(b+f->ofs) = ED_NewString (Value->GetStringValue());
						break;
					case F_VECTOR:
						Value->GetVectorValue(*(vec3_t *)(b+f->ofs));
						break;
					case F_INT:
						*(int *)(b+f->ofs) = Value->GetIntValue();
						break;
					case F_FLOAT:
						*(float *)(b+f->ofs) = Value->GetFloatValue();
						break;
					case F_ANGLEHACK:
						((float *)(b+f->ofs))[0] = 0;
						((float *)(b+f->ofs))[1] = Value->GetFloatValue();
						((float *)(b+f->ofs))[2] = 0;
						break;
					case F_IGNORE:
						break;

				}
				break;
			}
		}
		delete Name;
		delete Value;
	}

	ED_CallSpawn(ent);

	return new EntityVar(ent);
}

Variable *CScript::HandleBuiltinFunction(void)
{
	int			Index;
	edict_t		*Search;
	Variable	*V1, *V2;
	Variable	*Var;

	Index = ReadByte();
	switch(Index)
	{
		case FUNC_FIND_ENTITY_WITH_TARGET:
			V1 = PopStack();
			Search = G_Find(NULL, FOFS(targetname), V1->GetStringValue());
			Var = new EntityVar(Search);

			// kef
			if (game.cinematicfreeze && Search && Search->ai)
			{	// flag these jokers as being used by a cinematic
				Search->count = 255;
			}
			delete V1;
			break;

		case FUNC_SIN:
			V1 = PopStack();
			Var = new FloatVar( "", sin( V1->GetFloatValue() * DEGTORAD ));

			delete V1;
			break;

		case FUNC_COS:
			V1 = PopStack();
			Var = new FloatVar("", cos( V1->GetFloatValue() * DEGTORAD ));

			delete V1;
			break;

		case FUNC_RANDOM:
			V1 = PopStack();
			V2 = PopStack();
			Var = new FloatVar("",  (float)gi.irand(V1->GetIntValue(), V2->GetIntValue()) );

			delete V1;
			delete V2;
			break;

		case FUNC_FIND_ENTITY_WITH_SCRIPT:
			V1 = PopStack();
			Search = G_Find(NULL, FOFS(scripttarget), V1->GetStringValue());
			Var = new EntityVar(Search);

			// kef
			if (game.cinematicfreeze && Search && Search->ai)
			{	// flag these jokers as being used by a cinematic
				Search->count = 255;
			}
			delete V1;
			break;

		case FUNC_FIND_PLAYER:
			Search = G_Find (NULL, FOFS(classname), "player");
			Var = new EntityVar(Search);
			break;

		case FUNC_SPAWN:
			Var = HandleSpawn();
			break;

		case FUNC_GET_OTHER:
			Var = new EntityVar(other);
			break;

		case FUNC_GET_ACTIVATOR:
			Var = new EntityVar(activator);
			break;

		case FUNC_GET_PLAYER:
			V1 = PopStack();
			Var = new EntityVar(g_edicts+V1->GetIntValue() + 1);

			delete V1;

			break;
	}

	return Var;
}

void CScript::HandlePush(void)
{
	int			Type;
	Variable	*Var;

	Type = ReadByte();
	switch(Type)
	{
		case PUSH_CONST_INT:
			Var = new IntVar();
			Var->ReadValue(this);
			break;
		case PUSH_CONST_FLOAT:
			Var = new FloatVar();
			Var->ReadValue(this);
			break;
		case PUSH_CONST_VECTOR:
			Var = new VectorVar();
			Var->ReadValue(this);
			break;
		case PUSH_CONST_ENTITY:
			Var = new EntityVar();
			Var->ReadValue(this);
			break;
		case PUSH_CONST_STRING:
			Var = new StringVar();
			Var->ReadValue(this);
			break;
		case PUSH_VAR:
			Var = new VariableVar();
			((VariableVar *)Var)->ReadValue(this);
			break;
		case PUSH_VAR_WITH_FIELD:
			Var = new FieldVariableVar();
			((VariableVar *)Var)->ReadValue(this);
			break;
		case PUSH_FUNCTION:
			Var = HandleBuiltinFunction();
			break;
	}

	PushStack(Var);
}

void CScript::HandlePop(void)
{
	Variable *V;

	V = PopStack();
	if (V)
	{
		delete V;
	}
}

void CScript::HandleAssignment(void)
{
	Variable *Value, *Assignee;

	Assignee = PopStack();
	Value = PopStack();
	if (Value == NULL || Assignee == NULL)
	{
		Error("Invalid stack for Add");
	}

	(*Assignee) = Value;

	delete Assignee;
	delete Value;
}

void CScript::HandleAdd(void)
{
	Variable *V1, *V2;

	V1 = PopStack();
	V2 = PopStack();
	if (V1 == NULL || V2 == NULL)
	{
		Error("Invalid stack for Add");
	}

	PushStack((*V1) + V2);

	delete V1;
	delete V2;
}

void CScript::HandleSubtract(void)
{
	Variable *V1, *V2;

	V1 = PopStack();
	V2 = PopStack();
	if (V1 == NULL || V2 == NULL)
	{
		Error("Invalid stack for Subtract");
	}

	PushStack((*V1) - V2);

	delete V1;
	delete V2;
}

void CScript::HandleMultiply(void)
{
	Variable *V1, *V2;

	V1 = PopStack();
	V2 = PopStack();
	if (V1 == NULL || V2 == NULL)
	{
		Error("Invalid stack for Multiply");
	}

	PushStack((*V1) * V2);

	delete V1;
	delete V2;
}

void CScript::HandleDivide(void)
{
	Variable *V1, *V2;

	V1 = PopStack();
	V2 = PopStack();
	if (V1 == NULL || V2 == NULL)
	{
		Error("Invalid stack for Divide");
	}

	PushStack((*V1) / V2);

	delete V1;
	delete V2;
}

void CScript::HandleDebug(void)
{
	list<Variable *>::iterator	iv;
	int						Flags;

	Flags = ReadByte();

	if (Flags)
	{
		if (Flags & DEBUG_ENABLE)
		{
			Flags &= ~DEBUG_ENABLE;
			DebugFlags |= Flags;
		}
		else
		{
			DebugFlags &= ~Flags;
		}
	}
	else
	{
		StartDebug();

		if (ParameterVariables.size())
		{
			DebugLine("   Parameters:\n");
			for (iv=ParameterVariables.begin();iv != ParameterVariables.end();iv++)
			{
				(*iv)->Debug(this);
			}
		}

		if (GlobalVariables.size())
		{
			DebugLine("   Global Variables:\n");
			for (iv=GlobalVariables.begin();iv != GlobalVariables.end();iv++)
			{
				(*iv)->Debug(this);
			}
		}

		if (LocalVariables.size())
		{
			DebugLine("   Local Variables:\n");
			for (iv=LocalVariables.begin();iv != LocalVariables.end();iv++)
			{
				(*iv)->Debug(this);
			}
		}
		EndDebug();
	}
}

void CScript::HandleDebugStatement(void)
{
	DebugLine("%s\n",ReadString());
}

void CScript::HandleAddAssignment(void)
{
	Variable *Value, *Assignee, *temp;

	Assignee = PopStack();
	Value = PopStack();
	if (Value == NULL || Assignee == NULL)
	{
		Error("Invalid stack for AddAssignment");
	}

	temp = (*Assignee) + Value;
	(*Assignee) = temp;
	delete temp;

	delete Assignee;
	delete Value;
}

void CScript::HandleSubtractAssignment(void)
{
	Variable *Value, *Assignee, *temp;

	Assignee = PopStack();
	Value = PopStack();
	if (Value == NULL || Assignee == NULL)
	{
		Error("Invalid stack for SubtractAssignment");
	}

	temp = (*Assignee) - Value;
	(*Assignee) = temp;
	delete temp;

	delete Assignee;
	delete Value;
}

void CScript::HandleMultiplyAssignment(void)
{
	Variable *Value, *Assignee, *temp;

	Assignee = PopStack();
	Value = PopStack();
	if (Value == NULL || Assignee == NULL)
	{
		Error("Invalid stack for MultiplyAssignment");
	}

	temp = (*Assignee) * Value;
	(*Assignee) = temp;
	delete temp;

	delete Assignee;
	delete Value;
}

void CScript::HandleDivideAssignment(void)
{
	Variable *Value, *Assignee, *temp;

	Assignee = PopStack();
	Value = PopStack();
	if (Value == NULL || Assignee == NULL)
	{
		Error("Invalid stack for DivideAssignment");
	}

	temp = (*Assignee) / Value;
	(*Assignee) = temp;
	delete temp;

	delete Assignee;
	delete Value;
}

bool CScript::HandleWait(bool ForAll)
{
	int			count;
	Variable	*VI;

	count = ReadByte();
	if (count & WAIT_CLEAR)
	{
		ConditionInfo = WAIT_CLEAR;
	}
	else
	{
		ConditionInfo = 0;
	}

	count &= ~WAIT_CLEAR;

	for(;count;count--)
	{
		VI = PopStack();
		if (!VI)
		{
			Error("Invalid stack for HandleWait");
		}

		Waiting.push_back(VI);
	}

	if (ForAll)
	{
		ScriptCondition = COND_WAIT_ALL;
	}
	else
	{
		ScriptCondition = COND_WAIT_ANY;
	}

	if (CheckWait())
	{
		FinishWait(NULL,false);

		return false;
	}
	
	return true;
}

bool CScript::HandleTimeWait(void)
{
	Variable	*V;
	float		NextTime;

	V = PopStack();
	if (!V)
	{
		Error("Invalid stack for Time Wait");
	}

	NextTime = level.time + V->GetFloatValue();
	delete V;

	if (NextTime <= level.time)
	{
		return false;
	}

	AddEvent(new WaitEvent(NextTime) );

	ScriptCondition = COND_WAIT_TIME;

	return true;
}

bool CScript::HandleIf(bool Reverse)
{
	int			Condition;
	int			Location;
	Variable	*V1, *V2;
	bool		Result;

	Condition = ReadByte();
	Location = ReadInt();

	V2 = PopStack();
	V1 = PopStack();

	if (V1 == NULL || V2 == NULL)
	{
		Error("Invalid stack for If");
	}

	Result = false;

	switch(Condition)
	{
		case COND_EQUAL:
			if ((*V1) == V2)
			{
				Result = true;
			}
			break;
		case COND_LESS_THAN:
			if ((*V1) < V2)
			{
				Result = true;
			}
			break;
		case COND_LESS_THAN_EQUAL:
			if ((*V1) <= V2)
			{
				Result = true;
			}
			break;
		case COND_GREATER_THAN:
			if ((*V1) > V2)
			{
				Result = true;
			}
			break;
		case COND_GREATER_THAN_EQUAL:
			if ((*V1) >= V2)
			{
				Result = true;
			}
			break;
		case COND_NOT_EQUAL:
			if ((*V1) != V2)
			{
				Result = true;
			}
			break;
	}

	if (Result == Reverse)
	{
		Position = Location;
	}

	delete V1;
	delete V2;

	return (Result == Reverse);
}

void CScript::HandleOn(void)
{
	OnConditions.push_back(Position-1);

	// First Push
	if (ReadByte() != CODE_PUSH)
	{
		Error("Expecting PUSH for on statement");
	}
	HandlePush();
	delete PopStack();

	// Second Push
	if (ReadByte() != CODE_PUSH)
	{
		Error("Expecting PUSH for on statement");
	}
	HandlePush();
	delete PopStack();

	ReadByte();	// Condition
	ReadInt();	// Jump Location
}

bool CScript::ProcessOn(int NewPosition)
{
	SavePosition = Position;

	Position = NewPosition;

	if (ReadByte() != CODE_ON)
	{
		Error("Invalid opcode when handling on statement");
	}

	if (ReadByte() != CODE_PUSH)
	{
		Error("Expecting PUSH for on statement");
	}
	HandlePush();

	// Second Push
	if (ReadByte() != CODE_PUSH)
	{
		Error("Expecting PUSH for on statement");
	}
	HandlePush();

	if (!HandleIf(true))
	{
		Position = SavePosition;
		return false;
	}

	ClearEvents = true;
	SaveScriptCondition = ScriptCondition;
	ScriptCondition = COND_READY;

	Execute(NULL, NULL);

	return true;
}

void CScript::HandleResume(void)
{
	if (!ClearEvents)
	{
		Error("Resume command executed outside of ON branch");
	}

	ClearEvents = false;

	Position = SavePosition;
	ScriptCondition = SaveScriptCondition;
}

void CScript::HandlePrint(void)
{
	int			Flags;
	Variable	*Text, *Entity, *Level;
	char		*TextValue;
	int			TextIndex;
	int			LevelValue;
	edict_t		*ent;

	Entity = Level = NULL;
	LevelValue = PRINT_HIGH;
	ent = NULL;

	Flags = ReadByte();
	
	Text = PopStack();
	if (!Text)
	{
		Error("Invalid stack for Print");
	}
	if (Text->GetType() == TypeSTRING)
	{
		TextValue = Text->GetStringValue();
	}
	else
	{
		TextValue = "";
		TextIndex = Text->GetIntValue();
	}

	if (Flags & PRINT_LEVEL)
	{
		Level = PopStack();
		if (!Level)
		{
			Error("Invalid stack for Print");
		}
		LevelValue = Level->GetIntValue();
	}

	if (Flags & PRINT_ENTITY)
	{
		Entity = PopStack();
		if (!Entity)
		{
			Error("Invalid stack for Print");
		}
		ent = Entity->GetEdictValue();
	}

	if (!sv_jumpcinematic->value || !game.cinematicfreeze)
	{

		if (Text->GetType() == TypeSTRING)
		{
			if (ent)
			{
				gi.cprintf(ent, LevelValue, TextValue);
			}
			else
			{
				gi.bprintf(LevelValue, TextValue);
			}
		}
		else
		{
			gi.SP_Print(ent, TextIndex);
		}
	}


	delete Text;
	if (Entity)
	{
		delete Entity;
	}
	if (Level)
	{
		delete Level;
	}
}

void CScript::HandlePlaySound(void)
{
	int			Flags;
	Variable	*SoundName, *Entity, *Volume, *Attenuation, *Channel, *TimeDelay;
	char		*SoundValue;
	float		VolumeValue, AttenuationValue, TimeDelayValue;
	int			ChannelValue;
	edict_t		*ent;


	Entity = Volume = Attenuation = Channel = TimeDelay = NULL;
	ent = NULL;
	VolumeValue = 1.0;
	AttenuationValue = ATTN_NORM;

	ChannelValue = CHAN_VOICE;

	TimeDelayValue = 0.0;

	Flags = ReadByte();
	
	SoundName = PopStack();
	if (!SoundName)
	{
		Error("Invalid stack for PlaySound");
	}
	SoundValue = SoundName->GetStringValue();

	if (Flags & PLAY_SOUND_TIMEDELAY)
	{
		TimeDelay = PopStack();
		if (!TimeDelay)
		{
			Error("Invalid stack for PlaySound");
		}
		TimeDelayValue = TimeDelay->GetFloatValue();
	}

	if (Flags & PLAY_SOUND_CHANNEL)
	{
		Channel = PopStack();
		if (!Channel)
		{
			Error("Invalid stack for PlaySound");
		}
		ChannelValue = Channel->GetIntValue();
	}

	if (Flags & PLAY_SOUND_ATTENUATION)
	{
		Attenuation = PopStack();
		if (!Attenuation)
		{
			Error("Invalid stack for PlaySound");
		}
		AttenuationValue = Attenuation->GetFloatValue();
	}

	if (Flags & PLAY_SOUND_VOLUME)
	{
		Volume = PopStack();
		if (!Volume)
		{
			Error("Invalid stack for PlaySound");
		}
		VolumeValue = Volume->GetFloatValue();
	}

	if (Flags & PLAY_SOUND_ENTITY)
	{
		Entity = PopStack();
		if (!Entity)
		{
			Error("Invalid stack for PlaySound");
		}
		ent = Entity->GetEdictValue();
	}

	if (!sv_jumpcinematic->value || !game.cinematicfreeze)
	{
		gi.sound(ent, ChannelValue, gi.soundindex(SoundValue), VolumeValue, AttenuationValue, TimeDelayValue);
	}

	delete SoundName;
	if (Entity)
	{
		delete Entity;
	}
	if (Volume)
	{
		delete Volume;
	}
	if (Attenuation)
	{
		delete Attenuation;
	}
	if (Channel)
	{
		delete Channel;
	}
	if (TimeDelay)
	{
		delete TimeDelay;
	}
}

void CScript::HandleUnloadSound(void)
{
	Variable	*SoundName;
	char		*SoundValue;
	
	SoundName = PopStack();
	if (!SoundName)
	{
		Error("Invalid stack for HandleUnloadSound");
	}
	SoundValue = SoundName->GetStringValue();

	gi.unload_sound(SoundValue);

	delete SoundName;
}

void CScript::HandleUnloadROFF(void)
{
	Variable	*ROFFName;
	char		*ROFFValue;
	
	ROFFName = PopStack();
	if (!ROFFName)
	{
		Error("Invalid stack for HandleUnloadROFF");
	}
	ROFFValue = ROFFName->GetStringValue();

	RemoveROFF(ROFFValue);

	delete ROFFName;
}

void CScript::HandleFeature(bool Enable)
{
	int FeatureType;

	FeatureType = ReadByte();

	switch(FeatureType)
	{
		case FEATURE_TRIGGER:
			HandleTrigger(Enable);
			break;

		case FEATURE_AMBIENT_SOUNDS:
			break;

		case FEATURE_CINEMATICS:
			CinematicFreeze(Enable);
			break;

		case FEATURE_PLAGUE_SKINS:
			break;
	}
}

void CScript::HandleCacheSound(void)
{
	Variable	*SoundName;
	char		*SoundValue;

	SoundName = PopStack();
	if (!SoundName)
	{
		Error("Invalid stack for HandleChacheSound");
	}
	SoundValue = SoundName->GetStringValue();


	if (!sv_jumpcinematic->value || !game.cinematicfreeze)
	{
		gi.soundindex(SoundValue);
	}

	delete SoundName;
}

void CScript::HandleCacheStringPackage(void)
{
	Variable	*SPName;
	char		*SPValue;

	SPName = PopStack();
	if (!SPName)
	{
		Error("Invalid stack for HandleCacheStringPackage");
	}
	SPValue = SPName->GetStringValue();

	gi.SP_Register(SPValue);

	delete SPName;
}

void CScript::HandleCacheROFF(void)
{
	Variable	*SPName;
	char		*SPValue;

	SPName = PopStack();
	if (!SPName)
	{
		Error("Invalid stack for HandleCacheROFF");
	}
	SPValue = SPName->GetStringValue();

	FindROFF(SPValue);

	delete SPName;
}


void CScript::HandleMove(void)
{
	int			Flags;
	Variable	*Signaler, *Rate, *Duration, *Amount, *Entity;
	edict_t		*ent;
	vec3_t		Vec,Dest,Diff;
	vec_t		Length;

	Signaler = Rate = Duration = NULL;

	Flags = ReadByte();

	if (Flags & MOVE_SIGNALER)
	{
		Signaler = PopStack();
	}

	if (Flags & MOVE_RATE)
	{
		Rate = PopStack();
	}

	if (Flags & MOVE_DURATION)
	{
		Duration = PopStack();
	}

	Amount = PopStack();
	Entity = PopStack();

	Amount->GetVectorValue(Vec);

	ent = Entity->GetEdictValue();
	if (ent)
	{
		if (!Rate && !Duration)
		{
			if (!(Flags & MOVE_ABSOLUTE))
			{
				VectorAdd(ent->s.origin, Vec, ent->s.origin);
			}
			else
			{
				VectorCopy(Vec, ent->s.origin);
			}
			if (ent->chain)
			{
				VectorAdd(ent->chain->s.origin, Vec, ent->chain->s.origin);
			}
		}
		else
		{
			if (!(Flags & MOVE_ABSOLUTE))
			{
				VectorAdd(ent->s.origin, Vec, Dest);
			}
			else
			{
				VectorCopy(Vec, Dest);
			}

			VectorSubtract(ent->s.origin, Dest, Diff);
			Length = VectorLength(Diff);

			if (Rate && Duration)
			{
				ent->moveinfo.decel = ent->moveinfo.accel = ent->moveinfo.speed = Rate->GetFloatValue();
				Length = Rate->GetFloatValue() * Duration->GetFloatValue();
				VectorNormalize(Diff);
				VectorMA(ent->s.origin, Length, Diff, Dest);
			}
			else if (Rate)
			{
				ent->moveinfo.decel = ent->moveinfo.accel = ent->moveinfo.speed = Rate->GetFloatValue();
			}
			else
			{
				ent->moveinfo.decel = ent->moveinfo.accel = ent->moveinfo.speed = Length / Duration->GetFloatValue();
			}

			if (DebugFlags & DEBUG_MOVE)
			{
				StartDebug();
				DebugLine("   Moving Entity %d\n",Entity->GetIntValue());
				DebugLine("      From (%7.3f, %7.3f, %7.3f)\n",ent->s.origin[0],ent->s.origin[1],ent->s.origin[2]);
				DebugLine("      To   (%7.3f, %7.3f, %7.3f)\n",Dest[0], Dest[1], Dest[2]);
				EndDebug();
			}

			if (Signaler)
			{
				AddSignaler(ent, Signaler, SIGNAL_MOVE);
				Signaler=0;
			}

			Move(ent, Dest);
		}
	}

	delete Amount;
	delete Entity;
//	Signaling routine will handle this....
//  not if AddSignaler isn't called :)
	if (Signaler)
	{
		delete Signaler;
	}
	if (Rate)
	{
		delete Rate;
	}
	if (Duration)
	{
		delete Duration;
	}
}

void CScript::HandleRotate(void)
{
	int			Flags;
	Variable	*Signaler, *Rate, *Duration, *Amount, *Entity;
	edict_t		*ent;
	vec3_t		Vec,Dest,Diff;
	vec_t		Length;

	Signaler = Rate = Duration = NULL;

	Flags = ReadByte();

	if (Flags & ROTATE_SIGNALER)
	{
		Signaler = PopStack();
	}

	if (Flags & ROTATE_RATE)
	{
		Rate = PopStack();
	}

	if (Flags & ROTATE_DURATION)
	{
		Duration = PopStack();
	}

	Amount = PopStack();
	Entity = PopStack();

	Amount->GetVectorValue(Vec);

	ent = Entity->GetEdictValue();
	if (ent)
	{
		if (!Rate && !Duration)
		{
			VectorAdd(ent->s.angles, Vec, ent->s.angles);
			if (ent->chain)
			{
				VectorAdd(ent->chain->s.angles, Vec, ent->chain->s.angles);
			}
		}
		else
		{
			if (!(Flags & MOVE_ABSOLUTE))
			{
				VectorAdd(ent->s.angles, Vec, Dest);
			}
			else
			{
				VectorCopy(Vec, Dest);
			}

			VectorSubtract(ent->s.angles, Dest, Diff);
			Length = VectorLength(Diff);

			if (Rate && Duration)
			{
				ent->moveinfo.speed = Rate->GetFloatValue();
				Length = Rate->GetFloatValue() * Duration->GetFloatValue();
				VectorNormalize(Diff);
				VectorMA(ent->s.angles, Length, Diff, Dest);
			}
			else if (Rate)
			{
				ent->moveinfo.speed = Rate->GetFloatValue();
			}
			else
			{
				ent->moveinfo.speed = Length / Duration->GetFloatValue();
			}

			VectorCopy(Dest, ent->moveinfo.start_angles);
			VectorCopy(Dest, ent->moveinfo.end_angles);

			if (DebugFlags & DEBUG_ROTATE)
			{
				StartDebug();
				DebugLine("   Rotating Entity %d\n",Entity->GetIntValue());
				DebugLine("      From (%7.3f, %7.3f, %7.3f)\n",ent->s.angles[0],ent->s.angles[1],ent->s.angles[2]);
				DebugLine("      To   (%7.3f, %7.3f, %7.3f)\n",ent->moveinfo.end_angles[0], ent->moveinfo.end_angles[1], ent->moveinfo.end_angles[2]);
				EndDebug();
			}

			if (Signaler)
			{
				AddSignaler(ent, Signaler, SIGNAL_ROTATE);
				Signaler=0;
			}
			Rotate(ent);
		}
	}

	delete Amount;
	delete Entity;
//	Signaling routine will handle this
	if (Signaler)
	{
		delete Signaler;
	}
	if (Rate)
	{
		delete Rate;
	}
	if (Duration)
	{
		delete Duration;
	}
}

void CScript::HandleMoveRotate(void)
{
	int			Flags;
	Variable	*Signaler, *FileName, *Entity;
	edict_t		*ent;

	Signaler = NULL;

	Flags = ReadByte();

	if (Flags & MOVEROTATE_SIGNALER)	
	{
		Signaler = PopStack();
	}

	FileName = PopStack();
	Entity = PopStack();

	ent = Entity->GetEdictValue();
	if (ent)
	{
		byte *pbROFData;

		pbROFData = (byte *)FindROFF(FileName->GetStringValue());
		if (pbROFData)
		{
			// check file header... (struct declared here because nowhere else in program ever sees the format)
			//
			typedef struct
			{
				char	sHeader[4];		// "ROFF" (Rotation, Origin File Format)
				long	lVersion;	
				float	fCount;			// personally I'd have thought this should be an int, but I vos only follovink orderz
										// cuz originally the plugin was only capable of writing out floats
				//
				// Followed by main body data...
				//
				//		for each fCount:-
				//
				//		Float, float, float: relative origin change
				//		Float, float, float: relative rotation change
				// 
			} ROFHDR, *LPROFHDR;

			LPROFHDR lpROFHdr = (LPROFHDR) pbROFData;

			// (If code size is ever an issue I could always combine these checks into one and just say "bad file %s"...)
			//
			if (strncmp(lpROFHdr->sHeader,"ROFF",4)!=0)
			{
				Com_Printf("***********************************************\n");
				Com_Printf("Bad header in .ROF file '%s'\n",FileName->GetStringValue());
				Com_Printf("***********************************************\n");
			}
			else
			{
				if (lpROFHdr->lVersion != ROFF_VERSION)
				{
					Com_Printf("***********************************************\n");
					Com_Printf("Bad version (%d) in .ROF file '%s'\n",lpROFHdr->lVersion,FileName->GetStringValue());
					Com_Printf("***********************************************\n");
				}
				else
				{
					if (lpROFHdr->fCount <= 0)
					{
						Com_Printf("***********************************************\n");
						Com_Printf("Bad count %f in .ROF file '%s'\n",lpROFHdr->fCount,FileName->GetStringValue());
						Com_Printf("***********************************************\n");
					}
					else
					{// file valid...

						// note that since I read the whole file in at once and stack up a big queue of these, then I 
						//	need to keep a running copy of where the ent's origin and angles would be, and add the relative
						//	changes to those, rather than to its current ones...
						//
						vec3_t	v3OriginAtThisPoint,
								v3RotationAtThisPoint;

						VectorCopy(ent->s.origin,v3OriginAtThisPoint);
						VectorCopy(ent->s.angles,v3RotationAtThisPoint);

						if (Signaler)
						{
							AddSignaler(ent, Signaler, SIGNAL_MOVEROTATE);
						}

						vec3_t *v3ROFData = (vec3_t *) &lpROFHdr[1];
						for (int i=0; i<(int)(lpROFHdr->fCount); i++, v3ROFData+=2)
						{
							//		Float, float, float: relative origin change
							//		Float, float, float: relative rotation change
							//
							AddEvent(new MoveRotateEvent(level.time + (i * FRAMETIME), ent, v3ROFData[0], v3ROFData[1]));

							// debug lines broken into 2 places just to make the variable printing more convenient...
							//							
							if (DebugFlags & DEBUG_MOVEROTATE)
							{
								StartDebug();
								DebugLine("   MoveRotating Entity %d\n",Entity->GetIntValue());
								DebugLine("      Origin         (%7.3f, %7.3f, %7.3f)\n",v3ROFData[0][0],v3ROFData[0][1],v3ROFData[0][2]);
								DebugLine("      Rotation       (%7.3f, %7.3f, %7.3f)\n",v3ROFData[1][0],v3ROFData[1][1],v3ROFData[1][2]);
								DebugLine("      -- -- --\n");
								DebugLine("      Origin   Start (%7.3f, %7.3f, %7.3f)\n",v3OriginAtThisPoint[0],v3OriginAtThisPoint[1],v3OriginAtThisPoint[2]);
								DebugLine("      Rotation Start (%7.3f, %7.3f, %7.3f)\n",v3RotationAtThisPoint[0],v3RotationAtThisPoint[1],v3RotationAtThisPoint[2]);
								DebugLine("      -- -- --\n");

								Vec3AddAssign(v3ROFData[0],v3OriginAtThisPoint);
								Vec3AddAssign(v3ROFData[1],v3RotationAtThisPoint);

								DebugLine("      Origin   Stop (%7.3f, %7.3f, %7.3f)\n",v3OriginAtThisPoint[0],v3OriginAtThisPoint[1],v3OriginAtThisPoint[2]);
								DebugLine("      Rotation Stop (%7.3f, %7.3f, %7.3f)\n",v3RotationAtThisPoint[0],v3RotationAtThisPoint[1],v3RotationAtThisPoint[2]);				
								EndDebug();
							}
						}// for (int i=0; i<(int)(lpROFHdr->fCount); i++)

						if (Signaler)
						{
							// add a fake MoveRotate event with special values if we're doing signalling to indicate EOS...
							//							
							AddEvent(new MoveRotateEvent(level.time + (i * FRAMETIME), ent, v3ROFF_EOS_Marker, v3ROFF_EOS_Marker));							
							Signaler=0;
						}
					}// file valid
				}// version ok
			}// header ok
		}// file opened ok
	}// if (ent)

	delete FileName;
	delete Entity;
	delete Signaler; // 100% ok if Signaler=NULL

}// void CScript::HandleMoveRotate(void)

void CScript::HandleUse(void)
{
	Variable	*Entity;
	edict_t		*use_ent;

	Entity = PopStack();

	use_ent = Entity->GetEdictValue();
	if (use_ent && use_ent->use && use_ent->inuse)
	{
		use_ent->use(use_ent,other,activator);
		if (use_ent->ai)
		{
			use_ent->ai->SetStartleability(false);
		}
	}

	delete Entity;
}

void CScript::HandleTrigger(bool Enable)
{
	Variable	*Entity;
	edict_t		*trigger_ent;

	Entity = PopStack();

	trigger_ent = Entity->GetEdictValue();
	if (trigger_ent)
	{
		if (Enable)
		{
			trigger_ent->solid = SOLID_TRIGGER;
			trigger_ent->use = Use_Multi;
			gi.linkentity (trigger_ent);
		}
		else
		{
			trigger_ent->solid = SOLID_NOT;
			trigger_ent->use = NULL;
		}
	}

	delete Entity;
}

void CScript::HandleRemove (void)
{
	Variable	*Remove;
	edict_t		*RemoveEnt;

	Remove = NULL;

	Remove = PopStack();
	RemoveEnt = Remove->GetEdictValue ();
	if (RemoveEnt)
	{
		G_FreeEdict(RemoveEnt);
	}
	if (Remove)
	{
		delete Remove;
	}
}

void CScript::HandleHelicopter(void)
{
	int						Flags;
	Variable				*FloatArg, *Vector, *Target, *Command, *Heli, *Signaler;
	float					theFloat;
	vec3_t					vPos;
	edict_t					*theTarget = NULL;
	int						actionID = -1;


	FloatArg = Vector = Target = Command = Heli = Signaler = NULL;
	Flags = ReadByte();

	if (Flags & HELICOPTER_SIGNALER)
	{
		Signaler = PopStack();
	}

	if (Flags & HELICOPTER_FLOAT)
	{
		FloatArg = PopStack();
		theFloat = FloatArg->GetFloatValue();
	}

	if (Flags & HELICOPTER_VECT)
	{
		Vector = PopStack();
		Vector->GetVectorValue(vPos);
	}
		
	if (Flags & HELICOPTER_TARG)
	{
		Target = PopStack();
		theTarget = Target->GetEdictValue();
	}

	Command = PopStack();

	Heli = PopStack();
	
	ai_public_c*				ai = NULL;
	scripted_decision*					DecisionPointer;
	edict_t*					ent = NULL;

	if (Heli && (ent = Heli->GetEdictValue()))
	{
		if (ai = ent->ai)
		{
			if (!(DecisionPointer = ((ai_c*)(ai))->FindScriptedDecision(ent)))
			{
				ai->NewDecision(DecisionPointer = new scripted_decision(NULL,SCRIPTED_PRIORITY_ROOT,0,ent),ent);
			}

			if (Signaler)
			{
				AddSignaler(ent, Signaler, SIGNAL_HELI);
				Signaler=0;
			}

			switch(Command->GetIntValue())
			{// many of these will be combined into less-clunky stuff a bit later
			 // Also, much of this will eventually have to be routed through BODY, as sequences will
			// vary from model to model
				case HELI_CHANGE_BODY:
				case HELI_CHANGE_SKIN:
					{
						// need to pass the edict_t* of the heli into MoveAction(), so we pass in ent instead of theTarget
						((ai_c*)ai)->NewAction( ((generic_ghoul_heli_ai*)ai)->MoveAction(DecisionPointer,
								NULL, (ai_c*)ai, &MMoves[15], Command->GetIntValue(), 
								vPos, ent, theFloat),ent, true); 
						actionID = ((generic_ghoul_heli_ai*)ai)->GetMostRecentlyAddedActionID();
						break;
					}
				case HELI_TAKEOFF:
				case HELI_LAND:
				case HELI_GOTO_COORDS://			v3HSE_Coords
				case HELI_GOTOREL_ENTITY://		v3HSE_Coords	HSE_Entity
				case HELI_GOTOREL_ENT_X://						HSE_Entity	fHSE_Arg
				case HELI_GOTOREL_ENT_Y://						HSE_Entity	fHSE_Arg
				case HELI_GOTOREL_ENT_Z://						HSE_Entity	fHSE_Arg
				case HELI_MOVEREL://				v3HSE_Coords
				case HELI_STRAFE_RT://										fHSE_Arg
				case HELI_STRAFE_LT://										fHSE_Arg
				case HELI_HOVER_PASSIVE:	//									fHSE_Arg
				case HELI_HOVER_AGGRESSIVE://								fHSE_Arg
				case HELI_SET_DEATHDEST://		v3HSE_Coords
				case HELI_SET_WORLDMINS:
				case HELI_SET_WORLDMAXS:
				case HELI_REPAIR:
				case HELI_REARM:
				case HELI_SET_TRACEDIMS:
				case HELI_SET_HEALTH:
				case HELI_SET_MAXHEALTH:
				case HELI_AI:				//								fHSE_Arg
				case HELI_WAYPOINT:			// vector
				case HELI_VOLUME:			// vector
				case HELI_VOLUMEMINS:			// vector
				case HELI_VOLUMEMAXS:			// vector
				case HELI_OUT_OF_CONTROL:
					{
						((ai_c*)ai)->NewAction( ((generic_ghoul_heli_ai*)ai)->MoveAction(DecisionPointer,
								NULL, (ai_c*)ai, &MMoves[15], Command->GetIntValue(), 
								vPos, theTarget, theFloat),ent, true); 
						actionID = ((generic_ghoul_heli_ai*)ai)->GetMostRecentlyAddedActionID();
					}
					break;
				case HELI_DEBUG:
					{
						ent->spawnflags |= DEBUG_WAYPOINTS;
						break;
					}
				case HELI_FACE_RELENT://			v3HSE_Coords	HSE_Entity
				case HELI_FACE_ABSCOORDS://		v3HSE_Coords
				case HELI_FACE_ABSDIR://		v3HSE_Coords
				case HELI_FACE_RELCOORDS://		v3HSE_Coords
				case HELI_PILOT_FACERELENT://	v3HSE_Coords	HSE_Entity
				case HELI_PILOT_FACERELCOORDS://	v3HSE_Coords
				case HELI_PILOT_FACEABSCOORDS://	v3HSE_Coords
				case HELI_GUNNER_FACERELENT://	v3HSE_Coords	HSE_Entity
				case HELI_GUNNER_FACERELCOORDS://v3HSE_Coords
				case HELI_GUNNER_FACEABSCOORDS://v3HSE_Coords
					{
						((ai_c*)ai)->NewAction( ((generic_ghoul_heli_ai*)ai)->FaceAction(DecisionPointer,
								NULL, (ai_c*)ai, &MMoves[15], Command->GetIntValue(), 
								vPos, theTarget, theFloat),ent, true); 
						actionID = ((generic_ghoul_heli_ai*)ai)->GetMostRecentlyAddedActionID();
					}
					break;
				case HELI_ROCKETS_ENABLE://
				case HELI_ROCKETS_DISABLE://
				case HELI_CHAINGUN_ENABLE://
				case HELI_CHAINGUN_DISABLE://
				case HELI_FIREAT_RELENT://		v3HSE_Coords	HSE_Entity	fHSE_Arg
				case HELI_FIREAT_ABSCOORDS://	v3HSE_Coords				fHSE_Arg
				case HELI_FIREAT_RELCOORDS://	v3HSE_Coords				fHSE_Arg
				case HELI_AUTOFIRE_ON://
				case HELI_AUTOFIRE_OFF://
					{
						((ai_c*)ai)->NewAction( ((generic_ghoul_heli_ai*)ai)->AttackAction(DecisionPointer,
								NULL, (ai_c*)ai, &MMoves[15], Command->GetIntValue(), 
								vPos, theTarget, theFloat),ent,true); 
						actionID = ((generic_ghoul_heli_ai*)ai)->GetMostRecentlyAddedActionID();
					}
					break;
				default:
					break;
			}
			Helicopter(ent, actionID);
		}
	}
	delete (Heli);
	delete (Command);
	if (Target)
	{
		delete Target;
	}
	if (Vector)
	{
		delete Vector;
	}
	if (FloatArg)
	{
		delete FloatArg;
	}
	delete Signaler; // 100% ok if Signaler=NULL
}

void CScript::HandleTank(void)
{
	int						Flags;
	Variable				*FloatArg, *Vector, *Target, *Command, *TanK, *Signaler;
	float					theFloat;
	vec3_t					vPos;
	edict_t					*theTarget = NULL;
	int						actionID = -1;


	FloatArg = Vector = Target = Command = TanK = Signaler = NULL;
	Flags = ReadByte();

	if (Flags & TANK_SIGNALER)
	{
		Signaler = PopStack();
	}

	if (Flags & TANK_FLOAT)
	{
		FloatArg = PopStack();
		theFloat = FloatArg->GetFloatValue();
	}

	if (Flags & TANK_VECT)
	{
		Vector = PopStack();
		Vector->GetVectorValue(vPos);
	}
		
	if (Flags & TANK_TARG)
	{
		Target = PopStack();
		theTarget = Target->GetEdictValue();
	}

	Command = PopStack();

	TanK = PopStack();
	
	ai_public_c*				ai = NULL;
	scripted_decision*					DecisionPointer;
	edict_t*					ent = NULL;

	if (TanK && (ent = TanK->GetEdictValue()))
	{
		if (ai = ent->ai)
		{
			if (!(DecisionPointer = ((ai_c*)(ai))->FindScriptedDecision(ent)))
			{
				ai->NewDecision(DecisionPointer = new scripted_decision(NULL,SCRIPTED_PRIORITY_ROOT,0,ent),ent);
			}

			if (Signaler)
			{
				AddSignaler(ent, Signaler, SIGNAL_TANK);
				Signaler=0;
			}

			switch(Command->GetIntValue())
			{
				// have all commands generate the same type of action (for simplicity's sake)
				case TANK_GOTOCOORDS:
				case TANK_FIRECANNONATCOORDS:
				case TANK_MACHGUNAUTO:
				case TANK_DIE:
				case TANK_AIMTURRET:
					{
						((ai_c*)ai)->NewAction( ((generic_ghoul_tank_ai*)ai)->TankAction(DecisionPointer,
								NULL, (ai_c*)ai, &MMoves[15], Command->GetIntValue(), 
								vPos, theTarget, theFloat),ent, true); 
						actionID = ((generic_ghoul_tank_ai*)ai)->GetMostRecentlyAddedActionID();
					}
					break;
				default:
					break;
			}
			Tank(ent, actionID);
		}
	}
	delete (TanK);
	delete (Command);
	if (Target)
	{
		delete Target;
	}
	if (Vector)
	{
		delete Vector;
	}
	if (FloatArg)
	{
		delete FloatArg;
	}
	delete Signaler; // 100% ok if Signaler=NULL
}

void CScript::HandleAnimate(void)
{
	int					Flags;
	Variable			*Signaler, *Moving, *Turning, *Repeat, *Action, *Entity, *Source, *Holding, *Target, *NullTarget, *Emotion;
	edict_t				*ent, *SourceEnt, *TargetEnt;
	float				HoldingVal;
	vec3_t				MovingVal;
	vec3_t				TurningVal;
	int					RepeatVal = 1, NullTargetVal = 0;
	int					ActionVal;
	int					EmotionVal=0;
	bool				Absolute = false;
	bool				Kill = false;
	void				(*SignalerRoutine)(edict_t*);

	SignalerRoutine = NULL;
	Signaler = Moving = Turning = Repeat = Action = Entity = Source = Holding = Target = NullTarget = Emotion = NULL;
	SourceEnt = TargetEnt = NULL;
	VectorClear(MovingVal);
	VectorClear(TurningVal);
	RepeatVal = 1;

	Flags = ReadInt();

	if (Flags & ANIMATE_EMOTION)
	{
		Emotion = PopStack();
		EmotionVal = Emotion->GetIntValue();
	}

	if (Flags & ANIMATE_NULLTARGET)
	{
		NullTarget = PopStack();
		NullTargetVal = NullTarget->GetIntValue();
	}
	
	if (Flags & ANIMATE_TARGET)
	{
		Target = PopStack();
		TargetEnt = Target->GetEdictValue();
	}

	if (Flags & ANIMATE_SOURCE)
	{
		Source = PopStack();
		SourceEnt = Source->GetEdictValue();
	}

	if (Flags & ANIMATE_SIGNALER)
	{
		Signaler = PopStack();
	}

	if (Flags & ANIMATE_MOVING)
	{
		Moving = PopStack();
		Moving->GetVectorValue(MovingVal);
	}

	if (Flags & ANIMATE_TURNING)
	{
		Turning = PopStack();
		Turning->GetVectorValue(TurningVal);
	}

	if (Flags & ANIMATE_REPEAT)
	{
		Repeat = PopStack();
		RepeatVal = Repeat->GetIntValue();
	}

	if (Flags & ANIMATE_MOVING_ABS)
	{
		Absolute = true;
	}

	if (Flags & ANIMATE_KILL)
	{
		Kill = true;
	}

	if (Flags & ANIMATE_HOLD)
	{
		Holding = PopStack();
		HoldingVal = Holding->GetFloatValue();
	}

	Action = PopStack();
	ActionVal = Action->GetIntValue();

	Entity = PopStack();
	ent = Entity->GetEdictValue();

	if (ent)
	{
		ai_c*						ai;
		body_c*						body;
		scripted_decision*			DecisionPointer;
		action_code					ActCode=ACTCODE_STAND;
		float						SpeedVal=0.0;
		mmove_t						*SequencePtr=NULL;
//		vec3_t						OldPosition, Destination;

		//set a default holding value
		if (!(Flags & ANIMATE_HOLD))
		{
			if (EmotionVal)
			{
				HoldingVal=9999.9;
			}
			else
			{
				HoldingVal=50.0;
			}
		}

//		VectorClear(OldPosition);
		ai = (ai_c*)((ai_public_c*) ent->ai);

		//eek--no ai, abort
		if (!ai)
		{
			Com_Printf("entity %s had no ai--aborting HandleAnimate!\n",ent->targetname);
			return;
		}

		body = ai->GetBody();
		if (!(DecisionPointer = ai->FindScriptedDecision(ent)))
		{
			ai->NewDecision(DecisionPointer = new scripted_decision(NULL,SCRIPTED_PRIORITY_ROOT,0,ent),ent);
		}

		if (!EmotionVal)
		{
			switch(ActionVal)
			{// many of these will be combined into less-clunky stuff a bit later
			 // Also, much of this will eventually have to be routed through BODY, as sequences will
			// vary from model to model
				case UNKNOWN:
					break;
				case RUN:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					break;
				case JUMP:
					ActCode=ACTCODE_JUMP;
					SpeedVal=10.0;
					break;

				case DEATH:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[5];
					break;
				case DEATHTHROWN:
					ActCode=ACTCODE_DEATH;
					SpeedVal=150.0;
					SequencePtr=&MMoves[18];
					break;
				case DUCK_DN:
					SequencePtr=&MMoves[12];
					break;
				case DUCK_MID:
					SequencePtr=&MMoves[13];
					break;
				case DUCK_UP:
					SequencePtr=&MMoves[15];
					break;
				case DUCK_SHOOT:
					break;
				case IDLE:
					break;
				case IDLESTRETCH:
					break;
				case IDLELOOK:
					break;
				case SHOOT:
					// SHOOT should support an entity target or an (x,y,z) target
		//			ai->NewAction(ai->AttackAction(DecisionPointer, NULL, &MMoves[], 
					break;
				case WALK:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					break;
				case AFRAIDRUN:
				case GUN_IDLE:
					break;

				case A4_ASCRAMBLE_N_A_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[352];
					break;
				case CCH_A_FWD_MS_2:
					SequencePtr=&MMoves[61];
					break;
				case CCH_A_FWD_P_2:
					SequencePtr=&MMoves[20];
					break;
				case CCH_A_FWD_R_2:
					SequencePtr=&MMoves[62];
					break;
				case CCH_AFIRE_FWD_MS_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[483];
					break;
				case CCH_ECCH2WALK_N_A_N:
					SequencePtr=&MMoves[436];
					break;
				case CCH_ECHOKEKNEES_N_N_N:
					SequencePtr=&MMoves[249];
					break;			
				case CCH_ECHOKEDEATH_N_N_N:
					SequencePtr=&MMoves[264];
					break;
				case CCH_EHAWKCROUCH_N_A_N:
					SequencePtr=&MMoves[264];
					break;
				case CCH_EHAWKDEATH_N_N_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[450];
					break;		
				case CCH_EHAWKKNEES_N_N_N:
					SequencePtr=&MMoves[447];
					break;		
				case CCH_EHAWKKNEETLK_N_N_N:
					SequencePtr=&MMoves[451];
					break;
				case CCH_EHEADNOD_N_A_N:
					SequencePtr=&MMoves[435];
					break;
				case CCH_EHEADSET_FRM_A_N:
					SequencePtr=&MMoves[443];
					break;
				case CCH_EHEADSET_MID_A_N:
					SequencePtr=&MMoves[444];
					break;
				case CCH_EHEADSET_TO_A_N:
					SequencePtr=&MMoves[453];
					break;
				case CCH_ENODLEFT_N_A_A:
					SequencePtr=&MMoves[353];
					break;
				case CCH_ENODRIGHT_N_A_A:
					SequencePtr=&MMoves[354];
					break;
				case CCH_ESCANTALK_LT_A_A:
					SequencePtr=&MMoves[355];
					break;
				case CCH_ESCANTALK_RT_A_A:
					SequencePtr=&MMoves[356];
					break;
				case CCH_ETALK_DWN_P_N:
					SequencePtr=&MMoves[431];
					break;
				case CCH_ETOSTAND_N_A_N:
					SequencePtr=&MMoves[394];
					break;
				case CCH_EWALK2CCH_N_A_N:
					SequencePtr=&MMoves[430];
					break;
				case CCH_F_FWD_L_2:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[237];
					break;
				case CCH_F_FWD_M_2:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[88];
					break;
				case CCH_F_FWD_S_2:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[59];
					break;
				case CCH_IREADY_N_A_A:
					SequencePtr=&MMoves[357];
					break;
				case CCH_ITALKPOSE_DWN_A_N:
					SequencePtr=&MMoves[433];
					break;
				case CCH_RAIM_FWD_MRS_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[233];
					break;
				case CCH_RAIM_FWD_P_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[234];
					break;
				case CCH_RAIM_N_L_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[238];
					break;
				case CCH_TLOBGRENADE_N_A_N:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[239];
					break;
				case CCH_W_N_A_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=50.0;
					SequencePtr=&MMoves[429];
					break;
				case CCH_XCCHCOWER_MID_A_A:
					SequencePtr=&MMoves[149];
					break;
				case CCH_XCCHCOWER2_MID_A_A:
				case CCH_XCCHCOWER_MID2_A_A:
					SequencePtr=&MMoves[150];
					break;
				case CCH_XCCHCOWER_N_A_N:
					SequencePtr=&MMoves[536];
					break;
				case CCH_XCCHCOWER_UP_A_A:
					SequencePtr=&MMoves[194];
					break;
				case CCH_XCROUCH_UP_L_2:
					SequencePtr=&MMoves[250];
					break;
				case CCH_XCROUCH_UP_MRS_2:
					SequencePtr=&MMoves[95];
					break;
				case CCH_XCROUCH_UP_PK_A:
					SequencePtr=&MMoves[15];
					break;
				case LB_ESLIDE_N_PK_N:
					SequencePtr=&MMoves[454];
					break;
				case LB_PCRAWL_BK_P_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal= 5.0;
					SequencePtr=&MMoves[235];
					break;
				case LB_PHOLDLEG_LT_N_N:
					ActCode = ACTCODE_DEATH;
					SequencePtr=&MMoves[33];
					break;
				case LB_XLEANUPSHOOT_N_P_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[236];
					break;
				case LS_XGETKICKED_N_N_N:
					ActCode=ACTCODE_PAIN;
					SequencePtr=&MMoves[139];
					break;

				case PRN_A_N_A_2:
					SequencePtr=&MMoves[410];
					break;
				case PRN_C_N_A_2:
					SequencePtr=&MMoves[241];
					break;
				case PRN_XCOVERHEAD_TO_A_A:
					SequencePtr=&MMoves[197];
					break;
				case PRN_XFALLING_N_N_N:
					ActCode=ACTCODE_FALL;
					SequencePtr=&MMoves[230];
					break;
				case PRN_XFALLLAND_N_N_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[231];
					break;
				case PRN_XTOSTAND_N_A_A:
					SequencePtr=&MMoves[232];
					break;
				case SI_EWALLFEAR_N_A_N:
					SequencePtr=&MMoves[423];
					break;
				case SI_EWALLIDLE_N_A_N:
					SequencePtr=&MMoves[420];
					break;
				case SI_EWALLTALK_N_A_N:
					SequencePtr=&MMoves[421];
					break;
				case SI_ISIT_N_A_A:
					SequencePtr=&MMoves[228];
					break;
				case SI_XALERTSTAND_UP_A_A:
					SequencePtr=&MMoves[229];
					break;
				case SI_XBYWALL_N_N_N:
					SequencePtr=&MMoves[140];
					break;
				case STD_A_FWD_P_2:
					SequencePtr=&MMoves[21];
					break;
				case STD_A_FWD_P_CB:
					SequencePtr=&MMoves[22];
					break;
				case STD_A_UP_P_2:
					SequencePtr=&MMoves[243];
					break;
				case STD_ATHREAT_DWN_P_N:
					SequencePtr=&MMoves[294];
					break;
				case STD_ATHREAT_FWD_P_N:
					SequencePtr=&MMoves[295];
					break;
				case STD_ATHREATSHT_DWN_P_N:
					ActCode=ACTCODE_ATTACK;
					SequencePtr = &MMoves[389];
					break;
				case STD_ATHREATYEL_DWN_P_N:
					SequencePtr=&MMoves[296];
					break;
				case STD_ATHREATYEL_FWD_P_N:
					SequencePtr=&MMoves[297];
					break;
				case STD_EAUTOSWEEP_N_PM_A:
					ActCode=ACTCODE_ATTACK;
					SequencePtr = &MMoves[472];
					break;
				case STD_EBEG_N_A_A:
					SequencePtr=&MMoves[535];
					break;
				case STD_EBOOKLOOK_N_A_N:
					SequencePtr=&MMoves[441];
					break;
				case STD_EBOOKSCAN_N_A_N:
					SequencePtr=&MMoves[442];
					break;
				case STD_EBUSTGLASS_N_MRS_2:
					SequencePtr=&MMoves[272];
					break;
				case STD_ECHOKEKNEES_TO_N_N:
					SequencePtr=&MMoves[248];
					break;
				case STD_ECHOKESHOOT_N_A_N:
					ActCode = ACTCODE_ATTACK;
					SequencePtr=&MMoves[358];
					break;
				case STD_ECLIMBLEDG_DWN_A_N:
					SequencePtr=&MMoves[314];
					break;
				case STD_ECLIMBLEDG_MID_A_N:
					SequencePtr=&MMoves[315];
					break;
				case STD_ECLIMBLEDGE_UP_A_N:
					SequencePtr=&MMoves[316];
					break;
				case STD_ECLIMBSHAFT_N_A_N:
					SequencePtr=&MMoves[428];
					break;
				case STD_ECOMEHERE_N_A_N:
					SequencePtr=&MMoves[292];
					break;
				case STD_EDKRAIM_N_A_N:
					SequencePtr=&MMoves[446];
					break;
				case STD_EDKRAIMLK_FWD_A_N:
					SequencePtr=&MMoves[457];
					break;
				case STD_EDKRAIMTALK_N_A_N:
					SequencePtr=&MMoves[448];
					break;
				case STD_EDKRFIRELK_RT_A_N:
					SequencePtr=&MMoves[449];
					break;
				case STD_EDROPCROUCH_N_A_A:
					SequencePtr=&MMoves[347];
					break;
				case STD_EDROPGRENADE_N_A_N:
					SequencePtr=&MMoves[317];
					break;
				case STD_EEXAMINEWALL_N_A_N:
					SequencePtr=&MMoves[298];
					break;
				case STD_EFALLDEATH_N_N_N:
					SequencePtr=&MMoves[318];
					break;
				case STD_EFALLIMPACT_N_N_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[319];
					break;
				case STD_EFLIPTABLE_N_A_N:
					SequencePtr=&MMoves[273];
					break;
				case STD_EFRMSHADOW_N_MRS_2:
					SequencePtr=&MMoves[320];
					break;
				case STD_EFRMSHADOW_N_P_N:
					SequencePtr=&MMoves[321];
					break;
				case STD_EGANGSALUTE_N_A_N:
					SequencePtr=&MMoves[405];
					break;
				case STD_EHANDTALK_FRM_P_N:
					SequencePtr=&MMoves[322];
					break;
				case STD_EHANDTALK_N_P_N:
					SequencePtr=&MMoves[323];
					break;
				case STD_EHANDTALK_TO_MRS_N:
					SequencePtr=&MMoves[324];
					break;
				case STD_EHANDTALK_TO_P_N:
					SequencePtr=&MMoves[325];
					break;
				case STD_EHANDTALK_FRM_MRS_N:
					SequencePtr=&MMoves[326];
					break;
				case STD_EHEADNOD_N_A_N:
					SequencePtr=&MMoves[305];
					break;
				case STD_EHEADSET_FRM_P_N:
					SequencePtr=&MMoves[327];
					break;
				case STD_EHEADSET_MID_P_N:
					SequencePtr=&MMoves[328];
					break;
				case STD_EHEADSET_TO_P_N:
					SequencePtr=&MMoves[329];
					break;
				case STD_EHEADSHAKE_N_A_N:
					SequencePtr=&MMoves[306];
					break;
				case STD_EJUMPRAIL_N_A_N:
					ActCode=ACTCODE_JUMP;
					SequencePtr=&MMoves[307];
					break;
				case STD_EMERCHALL_N_A_A:
					SequencePtr=&MMoves[529];
					break;
				case STD_EMERCHONE_N_A_A:
					SequencePtr=&MMoves[530];
					break;
				case STD_EMERCHTALK_N_A_A:
					SequencePtr=&MMoves[533];
					break;
				case STD_EMERCHWAVE_LT_A_A:
					SequencePtr=&MMoves[534];
					break;
				case STD_EMERCHWAVE_RT_A_A:
					SequencePtr=&MMoves[532];
					break;
				case STD_EOFFLADDER_UP_A_N:
					SequencePtr=&MMoves[266];
					break;
				case STD_EPOUNDDOOR_N_A_N:
					SequencePtr=&MMoves[267];
					break;
				case STD_EPUSHBUTTONH_N_A_N:
					SequencePtr=&MMoves[413];
					break;
				case STD_EPUSHBUTTONS_N_N_N:
					SequencePtr=&MMoves[274];
					break;
				case STD_EPUSHCRATE_N_N_N:
					ActCode=ACTCODE_MOVE;
					SequencePtr=&MMoves[275];
					break;
				case STD_EPUSHOVER_N_PK_A:
					SequencePtr=&MMoves[484];
					break;
				case STD_ERAGESHOOT_N_P_N:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[300];
					break;
				case STD_ERAGEIDLE_N_P_N:
					SequencePtr=&MMoves[438];
					break;
				case STD_ESABREBLUFF_N_A_A:
					SequencePtr=&MMoves[471];
					break;
				case STD_ESABREMOCK_N_A_N:
					SequencePtr=&MMoves[301];
					break;
				case STD_ESALUTATIONS_N_A_N:
					SequencePtr=&MMoves[427];
					break;
				case STD_ESALUTE_FRM_A_N:
					SequencePtr=&MMoves[426];
					break;
				case STD_ESALUTE_FRM_MRS_N:
					SequencePtr=&MMoves[330];
					break;
				case STD_ESALUTE_N_A_N:
					SequencePtr=&MMoves[425];
					break;
				case STD_ESALUTE_N_MRS_N:
					SequencePtr=&MMoves[331];
					break;
				case STD_ESALUTE_TO_A_N:
					SequencePtr=&MMoves[424];
					break;
				case STD_ESALUTE_TO_MRS_N:
					SequencePtr=&MMoves[332];
					break;
				case STD_ESIGNALALARM_N_A_N:
					SequencePtr=&MMoves[276];
					break;
				case STD_ESPIN180_LT_A_N:
					SequencePtr=&MMoves[309];
					break;
				case STD_ESPIN180_RT_A_N:
					SequencePtr=&MMoves[310];
					break;
				case STD_ESUPRISE90_LT_A_N:
					SequencePtr=&MMoves[418];
					break;
				case STD_ETALK_FRM_A_N:
					SequencePtr=&MMoves[383];
					break;
				case STD_ETALK_N_A_N:
					SequencePtr=&MMoves[304];
					break;
				case STD_ETALK_TO_A_N:
					SequencePtr=&MMoves[384];
					break;
				case STD_ETALK_UP_A_N:
					SequencePtr=&MMoves[434];
					break;
				case STD_ETALKBOTH_N_A_N:
					SequencePtr=&MMoves[303];
					break;
				case STD_ETALKLEFT_N_A_N:
					SequencePtr=&MMoves[333];
					break;
				case STD_ETALKLKLT_FRM_A_N:
					SequencePtr=&MMoves[334];
					break;
				case STD_ETALKLKLT_MID_A_N:
					SequencePtr=&MMoves[335];
					break;
				case STD_ETALKLKLT_TO_A_N:
					SequencePtr=&MMoves[336];
					break;
				case STD_ETALKLKRT_FRM_A_N:
					SequencePtr=&MMoves[337];
					break;
				case STD_ETALKLKRT_MID_A_N:
					SequencePtr=&MMoves[338];
					break;
				case STD_ETALKLKRT_TO_A_N:
					SequencePtr=&MMoves[339];
					break;
				case STD_ETALKRIGHT_N_A_N:
					SequencePtr=&MMoves[313];
					break;
				case STD_ETOAIM_N_P_2:
					SequencePtr=&MMoves[422];
					break;
				case STD_ETORUN_N_MRS_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[340];
					break;
				case STD_ETORUN_N_P_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[311];
					break;
				case STD_ETURN90_LT_A_N:
					SequencePtr=&MMoves[341];
					break;
				case STD_ETURN90_RT_A_N:
					SequencePtr=&MMoves[342];
					break;
				case STD_ETYPING_N_N_N:
					SequencePtr=&MMoves[277];
					break;
				case STD_EWALL2WALK_N_MRS_2:
					ActCode=ACTCODE_MOVE;
					SequencePtr=&MMoves[278];
					break;
				case STD_EWALL2WALK_N_PK_N:
					ActCode=ACTCODE_MOVE;
					SequencePtr=&MMoves[279];
					break;
				case STD_EWALLIDLE_N_MRS_2:
					SequencePtr=&MMoves[280];
					break;
				case STD_EWALLIDLE_N_PK_N:
					SequencePtr=&MMoves[281];
					break;
				case STD_EWALLLOOK_LT_MRS_2:
					SequencePtr=&MMoves[282];
					break;
				case STD_EWALLLOOK_LT_PK_N:
					SequencePtr=&MMoves[283];
					break;
				case STD_EWALLLOOK_RT_MRS_2:
					SequencePtr=&MMoves[284];
					break;
				case STD_EWALLLOOK_RT_PK_N:
					SequencePtr=&MMoves[285];
					break;
				case STD_EWHIRL_RT_A_N:
					SequencePtr=&MMoves[461];
					break;
				case STD_EWRENCHH_FRM_N_N:
					SequencePtr=&MMoves[286];
					break;
				case STD_EWRENCHH_N_N_N:
					SequencePtr=&MMoves[287];
					break;
				case STD_EWRENCHH_TO_N_N:
					SequencePtr=&MMoves[288];
					break;
				case STD_EWRENCHV_FRM_N_N:
					SequencePtr=&MMoves[289];
					break;
				case STD_EWRENCHV_N_N_N:
					SequencePtr=&MMoves[290];
					break;
				case STD_EWRENCHV_TO_N_N:
					SequencePtr=&MMoves[291];
					break;
				case STD_EYELL_N_A_N:
					SequencePtr=&MMoves[411];
					break;
				case STD_F_FWD_M_2:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[37];
					break;
				case STD_FATTITUDE_N_P_A:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[7];
					break;
				case STD_FAUTOSWEEP_N_M_2:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[115];
					break;
				case STD_FAUTOSWEEP_UP_M_2:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[419];
					break;
				case STD_F_FWD_P_2:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[26];
					break;
				case STD_F_FWD_P_CB:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[23];
					break;
				case STD_F_FWD_R_2:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[92];
					break;
				case STD_F_FWD_S_2:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[90];
					break;
				case STD_FAUTOSWEEP_N_PM_A:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[212];
					break;
				case STD_FGANGBANGER_N_M_2:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[213];
					break;
				case STD_FRAPID_N_P_2:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[456];
					break;
				case STD_FRAPIDSWEEP_LT_P_2:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[416];
					break;
				case STD_FSWATSTYLE_FWD_M_2:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[214];
					break;
				case STD_FSWEEPUP_FRM_A_A:
					ActCode=ACTCODE_STAND;
					SequencePtr=&MMoves[462];
					break;
				case STD_FSWEEPUP_TO_A_A:
					ActCode=ACTCODE_STAND;
					SequencePtr=&MMoves[463];
					break;
				case STD_FVIOLENT_DWN_M_2:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[215];
					break;
				case STD_FVIOLENT_DWN_P_N:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[216];
					break;
				case STD_IALERT_N_MRS_2:
					SequencePtr=&MMoves[108];
					break;
				case STD_IALERT_N_P_N:
					SequencePtr=&MMoves[103];
					break;
				case STD_IALERTLK_RT_MRS_2:
					SequencePtr=&MMoves[412];
					break;
				case STD_IALERTTRN_LT_P_N:
					SequencePtr=&MMoves[417];
					break;
				case STD_ICLEANGUN_N_A_N:
					SequencePtr=&MMoves[43];
					break;
				case STD_ICLEANGUN2_N_MRS_2:
					SequencePtr=&MMoves[202];
					break;
				case STD_ICLEANGUN2_N_PK_N:
					SequencePtr=&MMoves[43];
					break;
				case STD_IEMERCHANT_N_A_A:
					SequencePtr=&MMoves[531];
					break;
				case STD_IETALKPOSE_N_A_N:
					SequencePtr=&MMoves[415];
					break;
				case STD_IETALKPOSE_RT_A_N:
					SequencePtr=&MMoves[439];
					break;
				case STD_IGUNUP_N_A_N:
					SequencePtr=&MMoves[343];
					break;
				case STD_IGUNUPLOOK_N_A_N:
					SequencePtr=&MMoves[455];
					break;
				case STD_ILEAN_N_P_N:
					SequencePtr=&MMoves[312];
					break;
				case STD_ILOOKING_N_A_A:
					SequencePtr=&MMoves[16];
					break;
				case STD_IMONITORUP_N_A_N:
					SequencePtr=&MMoves[44];
					break;
				case STD_IMONITORUP2_N_A_N:
					SequencePtr=&MMoves[203];
					break;
				case STD_ININJA_N_P_N:
					SequencePtr=&MMoves[395];
					break;
				case STD_IPISS_N_A_A:
					SequencePtr=&MMoves[293];
					break;
				case STD_ISMOKE_FRM_A_N:
					SequencePtr=&MMoves[205];
					break;
				case STD_ISMOKE_N_A_N:
					SequencePtr=&MMoves[206];
					break;
				case STD_ISMOKE_TO_A_N:
					SequencePtr=&MMoves[207];
					break;
				case STD_ITOUCHNUTS_N_A_N:
					SequencePtr=&MMoves[204];
					break;
				case STD_ITALK_N_A_N:
					SequencePtr=&MMoves[45];
					break;
				case STD_ITALK_N_MRS_2:
					SequencePtr=&MMoves[116];
					break;
				case STD_ITALKGEST1_N_A_N:
					SequencePtr=&MMoves[46];
					break;
				case STD_ITALKGEST2_N_A_N:
					SequencePtr=&MMoves[47];
					break;
				case STD_ITALKGEST3_N_A_N:	
					SequencePtr=&MMoves[48];
					break;
				case STD_ITALKPOSE_UP_A_N:
					SequencePtr=&MMoves[432];
					break;
				case STD_IWIPEBROW_N_A_N:
					SequencePtr=&MMoves[49];
					break;
				case STD_ISTAND_N_A_N:
					SequencePtr=&MMoves[0];
					break;
				case STD_ISTAND_N_MRS_2:
					SequencePtr=&MMoves[39];
					break;
				case STD_ILOOKING_N_MRS_2:
					SequencePtr=&MMoves[40];
					break;
				case STD_ILEANLOOK_FRM_PK_N:
					SequencePtr=&MMoves[145];
					break;
				case STD_ILEANLOOK_MID_PK_N:
					SequencePtr=&MMoves[146];
					break;
				case STD_ILEANLOOK_TO_PK_N:
					SequencePtr=&MMoves[147];
					break;
				case STD_ITURNVALVE_FRM_N_N:
					SequencePtr=&MMoves[133];
					break;
				case STD_ITURNVALVE_N_N_N:
					SequencePtr=&MMoves[134];
					break;
				case STD_ITURNVALVE_TO_N_N:
					SequencePtr=&MMoves[135];
					break;
				case STD_JDIVE2PRONE_N_A_N:
					ActCode=ACTCODE_JUMP;
					SequencePtr=&MMoves[53];
					break;
				case STD_JDOWN_N_A_A:
					ActCode=ACTCODE_JUMP;
					SequencePtr=&MMoves[148];
					break;
				case STD_JDROP_N_A_A:
					ActCode=ACTCODE_FALL;
					SequencePtr=&MMoves[9];
					break;
				case STD_JDROP_TO_A_N:
					ActCode=ACTCODE_JUMP;
					SpeedVal=150.0;
					SequencePtr=&MMoves[388];
					break;
				case STD_JRUN_FWD_A_A:
					ActCode=ACTCODE_JUMP;
					SpeedVal=150.0;
					SequencePtr=&MMoves[126];
					break;
				case STD_JTHROWSTAR_FWD_A_N:
					ActCode=ACTCODE_JUMP;
					SpeedVal=150.0;
					SequencePtr=&MMoves[409];
					break;
				case STD_JUPTO_N_A_A:
					ActCode=ACTCODE_JUMP;
					SpeedVal=150.0;
					SequencePtr=&MMoves[6];
					break;
				case STD_L_N_MS_2:
					SequencePtr=&MMoves[173];
					break;
				case STD_MKICKLOW_N_A_A:
					SequencePtr=&MMoves[136];
					break;
				case STD_MNINJAKICK_N_A_B:
					SequencePtr=&MMoves[217];
					break;
				case STD_MPISTOLWHIP_N_P_N:
					SequencePtr=&MMoves[137];
					break;
				case STD_MRIFLEBUTT_N_MRS_2:
					SequencePtr=&MMoves[218];
					break;
				case STD_R_DWNST_MRS_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[190];
					break;
				case STD_R_N_MRS_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[58];
					break;
				case STD_R_N_PK_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[2];
					break;
				case STD_RAFRAID_N_N_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[19];
					break;
				case STD_RAIM_N_MRS_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[100];
					break;
				case STD_RAIMSTRAF_LT_MRS_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[98];
					break;
				case STD_RAIMSTRAF_LT_P_CBN:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[96];
					break;
				case STD_RAIMSTRAF_RT_MRS_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[99];
					break;
				case STD_RAIMSTRAF_RT_P_CBN:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[97];
					break;
				case STD_RBACKAIM_N_MRS_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[182];
					break;
				case STD_RBACKAIM_N_P_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[132];
					break;
				case STD_RDYNAMIC_N_A_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[414];
					break;
				case STD_RGUNUP_N_P_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[4];
					break;
				case STD_RONFIRE_N_N_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[344];
					break;
				case STD_RONFIRETODTH_N_N_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[345];
					break;
				case STD_RTOSTOP_N_MRS_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[346];
					break;
				case STD_RTOSTOP_N_PK_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[302];
					break;
				case STD_SBLOWN_2BK_N_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[18];
					break;
				case STD_SELECTROCUTE_N_N_N:
					SequencePtr=&MMoves[265];
					break;
				case STD_SFALLFWD_2FRNT_N_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[219];
					break;
				case STD_SGUNFROMHAND_N_N_N:
					ActCode = ACTCODE_PAIN;
					SequencePtr = &MMoves[125];
					break;
				case STD_SGUT_2SIDE_N_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[200];
					break;
				case STD_SHEAD_2FRNT_N_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[25];
					break;
				case STD_SONFIRE_2BK_N_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[245];
					break;
				case STD_SOUTWINDOW_N_A_N:
					ActCode=ACTCODE_FALL;
					SequencePtr=&MMoves[478];
					break;
				case STD_SSABREDEATH_BK_N_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[407];
					break;
				case STD_SSHOULDER_LT_N_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[30];
					break;
				case STD_SSTUMBLE_2BK_P_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[141];
					break;
				case STD_SSTUMBLE_2FRNT_N_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[220];
					break;
				case STD_STOKNEES_2BK_N_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[5];
					break;
				case STD_SUNHURT_BK_PK_N:
					SequencePtr=&MMoves[165];
					break;
				case STD_SVIOLENT_N_N_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[196];
					break;
				case STD_TGRENADE_N_A_N:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[474];
					break;
				case STD_TRUN_N_A_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=150.0;
					SequencePtr=&MMoves[348];
					break;
				case STD_TUNDER_N_A_N:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[480];
					break;
				case STD_W_N_MRS_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[445];
					break;
				case STD_W_N_PN_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[349];
					break;
				case STD_WAIM_FWD_P_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[157];
					break;
				case STD_WATTITUDE_N_PK_A:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[8];
					break;
				case STD_WBACKAIM_N_MS_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[162];
					break;
				case STD_WBACKWARDS_N_A_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[406];
					break;
				case STD_WBCKSIGNAL_N_N_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[67];
					break;
				case STD_WCARRY1_N_N_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[56];
					break;
				case STD_WCARRY2_N_N_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[350];
					break;
				case STD_WFAST_N_A_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[437];
					break;
				case STD_WFIRE_FWD_MS_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[482];
					break;
				case STD_WJOG_N_MRS_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[208];
					break;
				case STD_WJOGGUNDWN_N_P_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[209];
					break;
				case STD_WJOGGUNUP_N_P_A:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[210];
					break;
				case STD_WNORMAL_N_PK_A:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[1];
					break;
				case STD_WPATROL_N_MRS_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[57];
					break;
				case STD_WPUSHCART_N_N_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[211];
					break;
				case STD_WSTEALTH_N_PK_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[408];
					break;
				case STD_WTALK_LT_MRS_2:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[351];
					break;
				case STD_WTOSTOP_N_PK_A:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[308];
					break;
				case STD_XAFRAIDSHAKE_N_N_N:
					SequencePtr=&MMoves[124];
					break;
				case STD_XALARMPUSH_N_A_N:
					SequencePtr=&MMoves[50];
					break;
				case STD_XATKPAUSE1_N_MRS_2:
					SequencePtr=&MMoves[82];
					break;
				case STD_XATKPAUSE1_N_PK_N:
					SequencePtr=&MMoves[84];
					break;
				case STD_XATKPAUSE2_N_PK_N:
					SequencePtr=&MMoves[85];
					break;
				case STD_XBOOMCROUCH_N_A_N:
					SequencePtr=&MMoves[54];
					break;
				case STD_XCCHCOWER_DWN_A_A:
					SequencePtr=&MMoves[181];
					break;
				case STD_XCHOKE_N_N_N:
					SequencePtr=&MMoves[221];
					break;
				case STD_XCOCK_N_S_2:
					SequencePtr=&MMoves[397];
					break;
				case STD_XCROUCH_DWN_L_2:
					SequencePtr=&MMoves[252];
					break;
				case STD_XCROUCH_DWN_MRS_2:
					SequencePtr=&MMoves[94];
					break;
				case STD_XCROUCH_DWN_P_2:
					SequencePtr=&MMoves[479];
					break;
				case STD_XCROUCH_DWN_P_A:
					SequencePtr=&MMoves[481];
					break;
				case STD_XCROUCH_DWN_PK_A:
					SequencePtr=&MMoves[12];
					break;
				case STD_XDANCE_N_N_N:
					SequencePtr=&MMoves[222];
					break;
				case STD_XDONTSHOOT_N_A_N:
					SequencePtr=&MMoves[188];
					break;
				case STD_XFEARWAVE_N_N_N:
					SequencePtr=&MMoves[189];
					break;
				case STD_XGOPRONE_N_A_A:
					SequencePtr=&MMoves[223];
					break;
				case STD_XHANDSPRING_FWD_A_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=100.0;
					SequencePtr=&MMoves[396];
					break;
				case STD_XKICKDOOR_N_A_A:
					SequencePtr=&MMoves[143];
					break;
				case STD_XLADDER_DWN_A_A:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[224];
					break;
				case STD_XLADDER_UP_A_A:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[225];
					break;
				case STD_XLADDERFRM_UP_A_A:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[393];
					break;
				case STD_XOPENDOOR_N_A_N:
					SequencePtr=&MMoves[91];
					break;
				case STD_XROLL_LT_A_A:
					SequencePtr=&MMoves[106];
					break;
				case STD_XROLL_RT_A_A:
					SequencePtr=&MMoves[105];
					break;
				case STD_XROLLTOCCH_FWD_A_A:
					SequencePtr=&MMoves[226];				
					break;
				case STD_XSTARTLED_N_A_A:
					SequencePtr=&MMoves[171];
					break;
				case STD_XSTUMBLE_N_A_A:
					SequencePtr=&MMoves[227];
					break;
				case STD_XTAKECOVER_N_A_N:
					ActCode=ACTCODE_MOVE;
					SpeedVal=10.0;
					SequencePtr=&MMoves[464];
					break;
				case STD_XYELLATYOU_N_A_A:
					SequencePtr=&MMoves[144];
					break;
				case STD_XYELLPOINT_FWD_A_N:
					SequencePtr=&MMoves[51];
					break;
				case STD_XYELLWAVE_BK_A_N:
					SequencePtr=&MMoves[52];
					break;
				case STD_XTUMBLE_2FRNT_N_N:
					SequencePtr=&MMoves[55];
					break;
				case PRN_XCOVERHEAD_N_A_A:
					SequencePtr=&MMoves[198];
					break;			
				case LB_PGENERICDIE_N_N_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[77];
					break;
				case CCH_SONKNEES_2FRNT_N_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[131];
					break;
				case CCH_XONKNEES_N_N_N:
					SequencePtr=&MMoves[132];
					break;
				case STD_SINBACK_2FRNT_N_N:
					ActCode=ACTCODE_DEATH;
					SequencePtr=&MMoves[138];
					break;
				case STD_WPUSH_N_N_N:
					ActCode=ACTCODE_MOVE;
					SequencePtr=&MMoves[142];
					break;
				case STD_FCORNERPEAK_LT_P_A:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[65];
					break;
				case STD_FCORNERPEAK_RT_P_A:
					ActCode=ACTCODE_ATTACK;
					SequencePtr=&MMoves[66];
					break;
				case STD_ASHUFFLE_N_P_2:
					SequencePtr=&MMoves[35];
					break;
				case STD_ASHUFFLE_N_P_A:
					SequencePtr=&MMoves[36];
					break;
				case STD_ASHUFFLE_N_MS_2:
					SequencePtr=&MMoves[101];
					break;
				case STD_ASHUFFLE_N_R_2:
					SequencePtr=&MMoves[102];
					break;
				case STD_ASHUFFLE_N_L_2:
					SequencePtr=&MMoves[260];
					break;


				case SCRIPT_RELEASE:
					ActCode=ACTCODE_SCRIPTRELEASE;//AHA! this isn't really an action, just an alert to the script decision that it can finish.
	/*
					if (ent->deadflag != DEAD_DEAD) // hmm, don't like this, but I can't check for a NULL
						// decision pointer, and I'm not sure how best to check for a VALID one...  err....
					{
						ai->NewAction(ai->EndScriptAction(DecisionPointer), ent, true);
					}
	*/
					break;
				default:
					break;
			}
		}//if !emotion

		Event *NewEvent=NULL;


			if (Signaler)
			{
				AddSignaler(ent, Signaler, SIGNAL_ANIMATE);
	/*

				action_c	*LastAction = ((ai_c*)(ai))->GetLastAction();
				
				LastAction->AttachSignalEvent(AddEvent(new AnimateDoneEvent(ent)));
	*/
				NewEvent = AddEvent(new AnimateDoneEvent(ent));
				Signaler=0;
			}

		for (int count = 0; count<RepeatVal; count++)
		{
	//		if (ActCode==ACTCODE_STAND)
	//		{
	//			CallAnimation(ActCode,	Flags,	ai, DecisionPointer, vec3_origin, vec3_origin, false, ent, SpeedVal, HoldingVal, NewEvent, SequencePtr);
	//		}
	//		else
	//		{
			if (count == (RepeatVal - 1)) // only signal the last time through
			{
				CallAnimation(ActCode,	Flags,	ai, DecisionPointer, MovingVal, TurningVal, Absolute, ent,
					SpeedVal, HoldingVal, NewEvent, TargetEnt, Kill, NullTargetVal, EmotionVal, SequencePtr);
			}
			else
			{
				CallAnimation(ActCode,	Flags,	ai, DecisionPointer, MovingVal, TurningVal, Absolute, ent,
					SpeedVal, HoldingVal, NULL, TargetEnt, Kill, NullTargetVal, EmotionVal, SequencePtr);
			}
//			}
		}
	}
	delete Target;
	delete NullTarget;
	delete Emotion;

	delete Action;
	delete Entity;
	if (Holding)
	{
		delete Holding;
	}
	if (Source)
	{
		delete Source;
	}
//	Signaling routine will handle this
	if (Signaler)
	{
		delete Signaler;
	}
	if (Repeat)
	{
		delete Repeat;
	}
	if (Turning)
	{
		delete Turning;
	}
	if (Moving)
	{
		delete Moving;
	}
}

void CScript::HandleResetAI(void)
{
	Variable			*Entity;
	edict_t				*ent;
	ai_c				*ai;

	Entity = PopStack();
	if (!Entity)
	{
		Error("Invalid stack for HandleResetAI()");
	}

	ent = Entity->GetEdictValue();
	delete Entity;

	if (ent)
	{
		ai = (ai_c*)((ai_public_c*) ent->ai);

		if (!ai)
		{
			Com_Printf("entity %s had no ai--aborting HandleResetAI!\n",ent->targetname);
			return;
		}

		ai->CancelScripting(ent);
	}
}


void RestartMusic(edict_t *ent);
void StartMusic(edict_t *ent, byte songID);

void CScript::HandlePlaySong()
{
	Variable	*SongID = PopStack();

	int song = SongID->GetIntValue();

	if(song < 0)
	{
		RestartMusic(level.sight_client);
	}
	else
	{
		StartMusic(level.sight_client, song);
	}

	delete SongID;
}

void CScript::HandleSetCvar()
{
	Variable	*val;
	Variable	*cvarName;

	val = PopStack();
	if (!val)
	{
		Error("Invalid stack for HandleSetCvar()");
	}

	cvarName = PopStack();
	if (!cvarName)
	{
		Error("Invalid stack for HandleSetCvar()");
	}

	if (val->GetType() == TypeSTRING)
	{
		gi.cvar_set(cvarName->GetStringValue(), val->GetStringValue());
	}
	else
	{
		gi.cvar_setvalue(cvarName->GetStringValue(), val->GetFloatValue());
	}

	delete val;
	delete cvarName;
}

void CScript::HandleConsoleCommand()
{
	Variable	*Command;
		
		
	Command = PopStack();
	if (!Command)
	{
		Error("Invalid stack for HandleConsoleCommand()");
	}

	gi.AddCommandString(Command->GetStringValue());
	gi.AddCommandString("\n");

	delete Command;
}

void CScript::CallAnimation(action_code actCode, int Flags,	ai_public_c* ai,
									scripted_decision *DecisionPointer, vec3_t MovingVal,
									vec3_t TurningVal, bool Absolute, edict_t *ent,
									float speed, float HoldingVal, Event *EventPtr,
									edict_t* TargetEnt, bool ShouldKill, int NullTarget, 
									int Emotion, mmove_t *preferredAnim)
{
	if (!DecisionPointer || !ai || !DecisionPointer)
	{
		return;
	}
	scriptOrder_c newOrder;
	newOrder.absoluteDest=Absolute;
	newOrder.actionType=actCode;
	VectorCopy(MovingVal,newOrder.destVec);
	VectorCopy(TurningVal,newOrder.turnVec);
	newOrder.flags=Flags;
	newOrder.holdTime=HoldingVal;
	newOrder.preferredMove=preferredAnim;
	newOrder.SignalEvent=EventPtr;
	newOrder.speed=speed;
	newOrder.target = TargetEnt;
	newOrder.kill = ShouldKill;
	newOrder.NullTarget = NullTarget;
	newOrder.Emotion = Emotion;

	DecisionPointer->AddOrder(*(ai_c*)ai, *TargetEnt, newOrder);

	//eek! make sure i'm activated!

	ai->Activate(*ent);
	ai->SetStartleability(false);
}

/*
void CScript::CallMovementAnimation(int Flags,	ai_public_c* ai,
									scripted_decision *DecisionPointer, vec3_t MovingVal,
									vec3_t TurningVal, bool Absolute, edict_t *ent,
									float speed, mmove_t *preferredAnim)
{
	if (!DecisionPointer)
	{
		return;
	}
	scriptOrder_t newOrder;
	newOrder.absoluteDest=Absolute;
	newOrder.actionType=ACTCODE_MOVE;
	VectorCopy(MovingVal,newOrder.destVec);
	VectorCopy(TurningVal,newOrder.turnVec);
	newOrder.flags=Flags;
	newOrder.holdTime=10.0;//10 sec timeout ok? this is arbitrary...
	newOrder.preferredMove=preferredAnim;

	DecisionPointer->AddOrder(newOrder);
}
*/
//yick. fixme; this is better, more flexible than what was here, but the angle stuff is bollocks!
//i'll rework it soon to queue up goals in the script decision.
void CScript::CallMovementAnimation(int Flags,	ai_public_c* ai,
									scripted_decision *DecisionPointer, vec3_t MovingVal,
									vec3_t TurningVal, bool Absolute, edict_t *ent,
									float speed, int AnimationNumber)
{
	vec3_t	Destination, FaceDestination, OldPosition, AnimDestination, TempDestination, AnimAngles;
	mmove_t	*newanim;
	body_c	*body=((ai_c*)(ai))->GetBody();

	VectorClear(OldPosition);

	if (!((ai_c*)(ai))->GetLastActionDestination(OldPosition))
	{
		VectorCopy(ent->s.origin, OldPosition);
	}

	if (Flags & ANIMATE_TURNING)
	{
		VectorCopy(OldPosition, Destination);
		VectorCopy(TurningVal, FaceDestination);
	}
	else if (Flags & ANIMATE_MOVING)
	{
		if (Absolute)
		{
			VectorCopy(MovingVal, Destination);
		}
		else
		{
			VectorAdd(OldPosition, MovingVal, Destination);
		}
		VectorCopy(Destination, FaceDestination);
	}


	if (VectorCompare(Destination,vec3_origin))
	{
		VectorCopy(OldPosition, AnimDestination);
	}
	else
	{
		VectorSubtract(Destination, OldPosition, AnimDestination);
		if (VectorCompare(AnimDestination, vec3_origin))
		{
			VectorCopy(OldPosition, AnimDestination);
		}
		else
		{
			VectorNormalize(AnimDestination);
			VectorScale(AnimDestination, speed, AnimDestination);
			VectorAdd(AnimDestination, OldPosition, AnimDestination);
		}
	}

	if (VectorCompare(FaceDestination,vec3_origin))
	{
		VectorCopy(ent->s.angles, AnimAngles);
	}
	else
	{
		VectorSubtract(FaceDestination, OldPosition, TempDestination);
		if (VectorCompare(TempDestination, vec3_origin))
		{
			VectorCopy(ent->s.angles, AnimAngles);
		}
		else
		{
			VectorNormalize(TempDestination);
			vectoangles(TempDestination, AnimAngles);
		}
	}

	if (AnimationNumber<0)
	{
		newanim=body->GetSequenceForMovement(*ent, AnimDestination, FaceDestination, OldPosition, AnimAngles,  ACTSUB_NORMAL, BBOX_PRESET_STAND);
	}
	else
	{
		newanim=body->GetSequenceForMovement(*ent, AnimDestination, FaceDestination, OldPosition, AnimAngles, ACTSUB_NORMAL, BBOX_PRESET_STAND, &MMoves[AnimationNumber]);
	}

	ai->NewAction(ai->WalkAction(DecisionPointer, NULL, newanim, Destination, FaceDestination), ent, true);


//	if (Flags & ANIMATE_TURNING)
//	{
//		ai->NewAction(ai->WalkAction(DecisionPointer, NULL, &MMoves[AnimationNumber], OldPosition, TurningVal), ent, true);
//	}
//	else if (Flags & ANIMATE_MOVING)
//	{
//		if (Absolute)
//		{
//			VectorCopy(MovingVal, Destination);
//		}
//		else
//		{
//			VectorAdd(OldPosition, MovingVal, Destination);
//		}
//		ai->NewAction(ai->WalkAction(DecisionPointer, NULL, &MMoves[AnimationNumber], Destination, Destination), ent, true);
//	}


	return;
}

void CScript::HandleCopyPlayerAttributes(void)
{
	Variable	*Player, *Destination;
	edict_t		*PlayerEnt, *DestinationEnt;

	Destination = PopStack();
	if (!Destination)
	{
		Error("Invalid stack for HandleCopyPlayerAttributes()");
	}
	DestinationEnt = Destination->GetEdictValue();

	Player = PopStack();
	if (!Player)
	{
		Error("Invalid stack for HandleCopyPlayerAttributes()");
	}
	PlayerEnt = Player->GetEdictValue();

	delete Player;
	delete Destination;
}

void CScript::HandleSetViewAngles(void)
{
	Variable	*Player, *Angles;
	edict_t		*PlayerEnt;
	vec3_t		vec;

	Angles = PopStack();
	if (!Angles)
	{
		Error("Invalid stack for HandleSetViewAngles()");
	}
	Angles->GetVectorValue(vec);

	Player = PopStack();
	if (!Player)
	{
		Error("Invalid stack for HandleSetViewAngles()");
	}
	PlayerEnt = Player->GetEdictValue();

	PlayerEnt->client->ps.pmove.delta_angles[0]=ANGLE2SHORT(-PlayerEnt->client->resp.cmd_angles[0]);
	PlayerEnt->client->ps.pmove.delta_angles[1]=ANGLE2SHORT(vec[1]-PlayerEnt->client->resp.cmd_angles[1]);
	PlayerEnt->client->ps.pmove.delta_angles[2]=ANGLE2SHORT(-PlayerEnt->client->resp.cmd_angles[2]);

	gi.linkentity(PlayerEnt);

	delete Player;
	delete Angles;
}

void CScript::HandleSetCacheSize(void)
{
	Variable	*CacheSize;

	CacheSize = PopStack();
	if (!CacheSize)
	{
		Error("Invalid stack for HandleSetCacheSize()");
	}

	delete CacheSize;
}

void CScript::Move_Done(edict_t *ent)
{
	VectorClear (ent->velocity);

	VectorCopy(ent->moveinfo.end_origin, ent->s.origin);
//	this would be a proper fix, but too late in the project to change as it might break things - rjr
//	gi.linkentity(ent);
}

void CScript::Move(edict_t *ent, vec3_t Dest)
{
	float	frames;

	VectorCopy(Dest, ent->moveinfo.end_origin);

	VectorSubtract (ent->moveinfo.end_origin, ent->s.origin, ent->moveinfo.dir);
	ent->moveinfo.remaining_distance = VectorNormalize (ent->moveinfo.dir);
	if (ent->moveinfo.remaining_distance <= 0)
	{
		frames = 0;
	}
	else
	{
		frames = floor((ent->moveinfo.remaining_distance / ent->moveinfo.speed) / FRAMETIME) + 1;
	}

	VectorScale (ent->moveinfo.dir, ent->moveinfo.remaining_distance/frames/FRAMETIME, ent->velocity);

	AddEvent(new MoveDoneEvent(level.time + (frames * FRAMETIME), ent));
}

void CScript::Rotate_Done (edict_t *ent)
{
	VectorClear (ent->avelocity);
}

void CScript::Rotate(edict_t *ent)
{
	float	distance;
	vec3_t	destdelta;
	float	frames;

	VectorSubtract (ent->moveinfo.start_angles, ent->s.angles, destdelta);

	distance = VectorNormalize (destdelta);
	if (ent->moveinfo.speed <= 0)
	{
		frames = 0;
		VectorClear (ent->avelocity);
	}
	else
	{
		frames = floor((distance / ent->moveinfo.speed) / FRAMETIME) + 1;
		VectorScale (destdelta, distance/frames/FRAMETIME, ent->avelocity);
	}

	AddEvent(new RotateDoneEvent(level.time + (frames * FRAMETIME), ent));
}

void CScript::Helicopter_Done (edict_t *ent)
{
	// kef -- not sure if we need anything in here
}

void CScript::Helicopter(edict_t *ent, int actionID)
{
	// kef -- anything else in here?
	float frames = 10000; // I hope I don't have to come up with a real number here

	AddEvent(new HelicopterDoneEvent(level.time + (frames * FRAMETIME), ent, actionID));
}

void CScript::Tank_Done (edict_t *ent)
{
	// kef -- not sure if we need anything in here
}

void CScript::Tank(edict_t *ent, int actionID)
{
	// kef -- anything else in here?
	float frames = 10000; // I hope I don't have to come up with a real number here

	AddEvent(new TankDoneEvent(level.time + (frames * FRAMETIME), ent, actionID));
}

Event* CScript::AddEvent(Event *Which)
{
	list<Event *>::iterator	ie;
	float					time;

	if (ClearEvents)
	{	// we must be in an ON condition, not going to be resumed, to clear out all the old events
		while(Events.size())
		{
			ie=Events.begin();
			delete (*ie);

			Events.erase(ie);
		}

		ClearEvents = false;
	}

	if (Events.size())
	{
		time = Which->GetTime();
		for (ie=Events.begin();ie != Events.end();ie++)
		{
			if ( (*ie)->GetTime() > time)
			{
				break;
			}
		}
		Events.insert(ie, Which);
	}
	else
	{
		Events.push_back(Which);
	}

#ifdef _DEBUG
	float				testtime;

	time = 0;
	for (ie=Events.begin();ie != Events.end();ie++)
	{
		testtime = (*ie)->GetTime();
		if (testtime < time)
		{
			DebugBreak();
		}
	}
#endif
	return Which;
}

void CScript::ProcessEvents(void)
{
	list<Event *>::iterator	ie, next;

	ie = Events.begin();				//	dk

	while(Events.size())
	{
//		ie = Events.Begin();			//	dk

		if ((*ie)->Process(this))
		{
			delete (*ie);
			Events.erase(ie);
			ie = Events.begin();		//	dk
		}
		else 
		{
			ie++;						//	dk
			if (ie == Events.end())		//	dk
			{							//	dk
				break;
			}							//	dk
		}
	}
}

void CScript::ClearTimeWait(void)
{
	if (ScriptCondition == COND_WAIT_TIME)
	{
		ScriptCondition = COND_READY;
	}
}

void CScript::AddSignaler(edict_t *Edict, Variable *Var, SignalT SignalType)
{
	list<Signaler *>::iterator	is;
	Signaler *NewSig;

	NewSig = new Signaler(Edict, Var, SignalType);

	// Note that this check does not need to be in there - signalers are very flexible, but if used
	// incorrectly, they can result in weird behavior - this check prevents more than one command using
	// the same signal varaible prior to a wait command
	for (is=Signalers.begin();is != Signalers.end();is++)
	{
		if (*(*is) == NewSig)
		{
			Error("Renner Error #1: Variable '%s' is being used for multiple signals", Var->GetName() );
		}
	}

	Signalers.push_back(NewSig);
}

void CScript::CheckSignalers(edict_t *Which, SignalT SignalType)
{
	list<Signaler *>::iterator	is, next;
	bool					DoCheckWait = false;

	if (Signalers.size())
	{
		for (is=Signalers.begin();is != Signalers.end();is = next)
		{
			next = is;
			next++;
			if ((*is)->Test(Which, SignalType))
			{
				delete (*is);
				Signalers.erase(is);

				DoCheckWait = true;
			}
		}
	}

	if (DoCheckWait && (ScriptCondition == COND_WAIT_ANY || ScriptCondition == COND_WAIT_ALL))
	{
		if (CheckWait())
		{
			FinishWait(Which, true);
		}
	}
}

bool CScript::CheckWait(void)
{
	list<Variable *>::iterator	iv;
	int						count, needed;

	if (ScriptCondition == COND_WAIT_ALL)
	{
		needed = Waiting.size();
	}
	else if (ScriptCondition == COND_WAIT_ANY)
	{
		needed = 1;
	}
	else if (ScriptCondition == COND_WAIT_TIME)
	{
		return false;
	}
	else if (ScriptCondition == COND_READY)
	{
		return true;
	}
	else
	{
		return false;
	}

	count = 0;
	if (Waiting.size())
	{
		for (iv=Waiting.begin();iv != Waiting.end();iv++)
		{
			if ( (*iv)->GetIntValue())
			{
				count++;
			}
		}
	}

	if (count == needed)
	{
		ScriptCondition = COND_READY;

		return true;
	}

	return false;
}

void CScript::FinishWait(edict_t *Which, bool NoExecute)
{
	list<Variable *>::iterator	iv;

	if (Waiting.size())
	{
		for (iv=Waiting.begin();iv != Waiting.end();iv++)
		{
			if (ConditionInfo == WAIT_CLEAR)
			{
				(*iv)->ClearSignal();
			}

			delete *iv;
		}
	}
	Waiting.erase(Waiting.begin(), Waiting.end() );

	if (NoExecute)
	{
		Execute(Which, NULL);
	}
}

void CScript::Error (char *error, ...)
{
	va_list argptr;
	char	text[1024];
	
	va_start (argptr, error);
	vsprintf (text, error, argptr);
	va_end (argptr);

	gi.error(text);
}

void CScript::StartDebug(void)
{
	DebugLine("-------------------------------\n");
	DebugLine("Script: %s\n",Name);
	DebugLine("   DEBUG at %d\n",Position);
}

void CScript::EndDebug(void)
{
	DebugLine("-------------------------------\n");
}

void CScript::DebugLine (char *debugtext, ...)
{
	va_list argptr;
	char	text[1024];
	
	va_start (argptr, debugtext);
	vsprintf (text, debugtext, argptr);
	va_end (argptr);

	Com_Printf(P_PURPLE "%s",text);

#ifdef _DEBUG
	OutputDebugString(text);
#endif
}

void CScript::Think(void)
{
	int					i = 0, null_snd = 0;
	list<int>::iterator	it;

	if (sv_jumpcinematic->value == 2)
	{	// skipping this cinematic...kill any sounds played during the cinematic
		if (bClearCinematicSounds)
		{
			bClearCinematicSounds = false;
			gi.AddCommandString ("stopsound\n");
		}
	}

	for(it=OnConditions.begin(); it != OnConditions.end(); it++)
	{
		if (ProcessOn((*it)))
		{
			OnConditions.erase(it);
			break;
		}
	}

	ProcessEvents();
}

ScriptConditionT CScript::Execute(edict_t *new_other, edict_t *new_activator)
{
	bool				Done;
	int					InstructionCount;

	if (ScriptCondition != COND_READY)
	{
		return ScriptCondition;
	}

	if (DebugFlags & DEBUG_TIME)
	{
		StartDebug();
		DebugLine("   Current Time: %10.1f\n",level.time);
		EndDebug();
	}

	if (new_other)
	{
		other = new_other;
	}
	if (new_activator)
	{
		activator = new_activator;
	}

	InstructionCount = 0;
	Done = false;
	while (!Done)
	{
		InstructionCount++;
		if (InstructionCount > INSTRUCTION_MAX)
		{
			Error("Runaway loop for script OR more than 1000 instructions");
		}

		switch(ReadByte())
		{
			case CODE_NEW_GLOBAL:
				HandleGlobal(false);
				break;
			case CODE_NEW_GLOBAL_PLUS_ASSIGNMENT:
				HandleGlobal(true);
				break;
			case CODE_NEW_LOCAL:
				HandleLocal(false);
				break;
			case CODE_NEW_LOCAL_PLUS_ASSIGNMENT:
				HandleLocal(true);
				break;
			case CODE_NEW_PARAMETER:
				HandleParameter(false);
				break;
			case CODE_NEW_PARAMETER_PLUS_DEFAULT:
				HandleParameter(true);
				break;
			case CODE_FIELD:
				HandleField();
				break;
			case CODE_ASSIGNMENT:
				HandleAssignment();
				break;
			case CODE_ADD:
				HandleAdd();
				break;
			case CODE_SUBTRACT:
				HandleSubtract();
				break;
			case CODE_MULTIPLY:
				HandleMultiply();
				break;
			case CODE_DIVIDE:
				HandleDivide();
				break;
			case CODE_ADD_ASSIGNMENT:
				HandleAddAssignment();
				break;
			case CODE_SUBTRACT_ASSIGNMENT:
				HandleSubtractAssignment();
				break;
			case CODE_MULTIPLY_ASSIGNMENT:
				HandleMultiplyAssignment();
				break;
			case CODE_DIVIDE_ASSIGNMENT:
				HandleDivideAssignment();
				break;
			case CODE_GOTO:
				HandleGoto();
				break;
			case CODE_PUSH:
				HandlePush();
				break;
			case CODE_POP:
				HandlePop();
				break;
			case CODE_IF:
				HandleIf();
				break;
			case CODE_ON:
				HandleOn();
				break;
			case CODE_RESUME:
				HandleResume();
				Done = true;
				break;
			case CODE_EXIT:
				ScriptCondition = COND_COMPLETED;
				Done = true;
				break;
			case CODE_SUSPEND:
				//ScriptCondition = COND_SUSPENDED;
				Done = true;
				break;
			case CODE_DEBUG:
				HandleDebug();
				break;
			case CODE_WAIT_SECONDS:
				Done = HandleTimeWait();
				break;
			case CODE_WAIT_ALL:
				Done = HandleWait(true);
				break;
			case CODE_WAIT_ANY:
				Done = HandleWait(false);
				break;
			case CODE_MOVE:
				HandleMove();
				break;
			case CODE_ROTATE:
				HandleRotate();
				break;
			case CODE_USE:
				{
					byte	oldFreeze = game.cinematicfreeze;
					bool	bPlayerIsAlive = (g_edicts[1].health > 0);
					if (oldFreeze && !bPlayerIsAlive)
					{	// I don't know how we got a script started with the player dead, but end it now.
						//ScriptCondition = COND_COMPLETED;
						CinematicFreeze(false);
						Done = true;
						break;
					}
					HandleUse();
					if (oldFreeze != game.cinematicfreeze)
					{
						// mark the start time of this cinematic
						s_cinematicStartTime = level.time;
						if (!bPlayerIsAlive)
						{
							// we just started a cinematic and the player's pushing up daisies. end the script.
							//ScriptCondition = COND_COMPLETED;
							CinematicFreeze(false);
							Done = true;
						}
					}
					break;
				}
			case CODE_COPY_PLAYER_ATTRIBUTES:
				HandleCopyPlayerAttributes();
				break;
			case CODE_SET_VIEW_ANGLES:
				HandleSetViewAngles();
				break;
			case CODE_SET_CACHE_SIZE:
				HandleSetCacheSize();
				break;
			case CODE_ANIMATE:
				HandleAnimate();
				break;
			case CODE_RESET_AI:
				HandleResetAI();
				break;
			case CODE_PRINT:
				HandlePrint();
				break;
			case CODE_PLAY_SOUND:
				HandlePlaySound();
				break;
			case CODE_UNLOAD_SOUND:
				HandleUnloadSound();
				break;
			case CODE_UNLOAD_ROFF:
				HandleUnloadROFF();
				break;
			case CODE_ENABLE:
				HandleFeature(true);
				break;
			case CODE_DISABLE:
				HandleFeature(false);
				break;
			case CODE_DEBUG_STATEMENT:
				HandleDebugStatement();
				break;
			case CODE_CACHE_SOUND:
				HandleCacheSound();
				break;
			case CODE_CACHE_STRING_PACKAGE:
				HandleCacheStringPackage();
				break;
			case CODE_CACHE_ROFF:
				HandleCacheROFF();
				break;
			case CODE_REMOVE:
				HandleRemove();
				break;
			case CODE_HELICOPTER:
				HandleHelicopter();
				break;
			case CODE_PLAYSONG:
				HandlePlaySong();
				break;
			case CODE_SETCVAR:
				HandleSetCvar();
				break;
			case CODE_CONSOLE_COMMAND:
				HandleConsoleCommand();
				break;
			case CODE_MOVEROTATE:
				HandleMoveRotate();
				break;
			case CODE_TANK:
				HandleTank();
				break;
			default:
				Done = true;
				break;
		}

		if (Position >= Length)
		{
			Done = true;
			ScriptCondition = COND_COMPLETED;
		}
	}
	return ScriptCondition;
}

Variable *CScript::FindLocal(char *Name)
{
	list<Variable *>::iterator	iv;

	if (LocalVariables.size())
	{
		for (iv=LocalVariables.begin();iv != LocalVariables.end();iv++)
		{
			if (strcmp(Name, (*iv)->GetName()) == 0)
			{
				return *iv;
			}
		}
	}

	return NULL;
}

bool CScript::NewLocal(Variable *Which)
{
	Variable *Check;

	Check = FindLocal(Which->GetName());
	if (Check)
	{	// already exists
		return false;
	}

	LocalVariables.push_back(Which);

	return true;
}

Variable *CScript::FindParameter(char *Name)
{
	list<Variable *>::iterator	iv;

	if (ParameterVariables.size())
	{
		for (iv=ParameterVariables.begin();iv != ParameterVariables.end();iv++)
		{
			if (strcmp(Name, (*iv)->GetName()) == 0)
			{
				return *iv;
			}
		}
	}

	return NULL;
}

bool CScript::NewParameter(Variable *Which)
{
	Variable	*Check;
	StringVar	*ParmValue;
	edict_t		*Search;
	Variable	*temp;
	vec3_t		vec;

	Check = FindParameter(Which->GetName());
	if (Check)
	{	// already exists
		return false;
	}

	ParameterVariables.push_back(Which);

	if (!ParameterValues.size())
	{
		Error("Missing Parameter");
	}

	ParmValue = *ParameterValues.begin();
	ParameterValues.erase(ParameterValues.begin());

	switch(Which->GetType())
	{
		case TypeENTITY:
			Search = G_Find(NULL, FOFS(targetname), ParmValue->GetStringValue());
			temp = new EntityVar(Search);
			break;

		case TypeINT:
			temp = new IntVar("parm",atol(ParmValue->GetStringValue()));
			break;

		case TypeFLOAT:
			temp = new FloatVar("parm",atof(ParmValue->GetStringValue()));
			break;

		case TypeVECTOR:
			sscanf (ParmValue->GetStringValue(), "%f %f %f", &vec[0], &vec[1], &vec[2]);
			temp = new VectorVar("parm",vec[0],vec[1],vec[2]);
			break;

		default:
			delete ParmValue;
			return false;
			break;
	}

	(*Which) = temp;

	delete temp;
	delete ParmValue;

	return true;
}

//==========================================================================

