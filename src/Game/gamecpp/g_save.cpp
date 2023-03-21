#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

#include "g_local.h"
#include "ds.h"
#include "fields.h"
#include "callback.h"
#include "ef_flags.h"

// all pointer fields should be listed here, or savegames
// won't work properly (they will crash and burn).
// this wasn't just tacked on to the fields array, because
// these don't need names, we wouldn't want map fields using
// some of these, and if one were accidentally present twice
// it would double swizzle (fuck) the pointer.

void AngleMove_Begin(edict_t *self);
void AngleMove_Done(edict_t *self);
void AngleMove_Final(edict_t *self);
void auto_gun_arc(edict_t *self);
void auto_gun_target(edict_t *self);
void banner_animate(edict_t *self);
void barrel_chemical_fall(edict_t *self);
void bed_deccel(edict_t *self);
void bed_stop(edict_t *self);
void blinds_stoppain(edict_t *self);
void blownpartremove_think(edict_t *self);
void boltDropNotSolid(edict_t *self);
void bosnia_truck_remove(edict_t *self);
void button_done(edict_t *self);
void button_return(edict_t *self);
void button_wait(edict_t *self);
void buttonInit(edict_t *self);
void c4Explode(edict_t *self);
void cash_register_paindone(edict_t *self);
void chair_squeak(edict_t *self);
void chandelier_spin(edict_t *self);
void CharThink(edict_t *self);
void chunk_spewer_think(edict_t *self);
void ClearEffects (edict_t *ent);
void Cobra2_OutOfControl(edict_t *self);
void Cobra2AIHelper(edict_t *self);
void Cobra2AutoFireHelper(edict_t *self);
void Cobra2ChainGunEnableHelper(edict_t *self);
void Cobra2DeathDestHelper(edict_t *self);
void Cobra2FaceAbsDirHelper(edict_t *self);
void Cobra2FaceAbsHelper(edict_t *self);
void Cobra2FaceRelCoordsHelper(edict_t *self);
void Cobra2FaceRelEntHelper(edict_t *self);
void Cobra2FireAtAbsHelper(edict_t *self);
void Cobra2FireAtRelEntHelper(edict_t *self);
void Cobra2FireAtRelHelper(edict_t *self);
void Cobra2GotoCoordsHelper(edict_t *self);
void Cobra2GotoRelEntHelper(edict_t *self);
void Cobra2HeadFaceAbsCoordsHelper(edict_t *self);
void Cobra2HeadFaceRelCoordsHelper(edict_t *self);
void Cobra2HeadFaceRelEntHelper(edict_t *self);
void Cobra2HealthHelper(edict_t *self);
void Cobra2HoverHelper(edict_t *self);
void Cobra2LandHelper(edict_t *self);
void Cobra2RearmHelper(edict_t *self);
void Cobra2RocketsEnableHelper(edict_t *self);
void Cobra2StrafeHelper(edict_t *self);
void Cobra2TraceDimsHelper(edict_t *self);
void Cobra2WorldHelper(edict_t *self);
void Cobra2Think_AI(edict_t *self);
void Cobra2Think_DipFireAt(edict_t *self);
void Cobra2Think_FireAt(edict_t *self);
void Cobra2Think_GotoFireAt(edict_t *self);
void Cobra2Think_HeadToDest(edict_t *self);
void Cobra2Think_Hover(edict_t *self);
void Cobra2Think_Land(edict_t *self);
void Cobra2Think_MovementFinish(edict_t *self);
void Cobra2Think_Repair(edict_t *self);
void Cobra2Think_SidestrafeToDest(edict_t *self);
void Cobra2Think_SmartHeadToDest(edict_t *self);
void Cobra2Think_TakeOff(edict_t *self);
void Cobra2Think_TurnToFace(edict_t *self);
void Cobra2TraceDimsHelper(edict_t *self);
void Cobra2WorldHelper(edict_t *self);
void cone_fall(edict_t *self);
void dekker_init (edict_t *self);
void desk_phone_effects(edict_t *self);
void door_go_down(edict_t *self);
void door_hit_bottom(edict_t *self);
void door_hit_top(edict_t *self);
void door_secret_done(edict_t *self);
void door_secret_move1(edict_t *self);
void door_secret_move2(edict_t *self);
void door_secret_move3(edict_t *self);
void door_secret_move4(edict_t *self);
void door_secret_move5(edict_t *self);
void door_secret_move6(edict_t *self);
void door_spawn_areaportal_think(edict_t *self);
void DoRespawn(edict_t *self);
void drawMonkeyThink(edict_t *self);
void dustsource_think(edict_t *self);
void EmergencyLightFade(edict_t *self);
void EmergencyLightRedGlow(edict_t *self);
void EmergencyLightReturn(edict_t *self);
void emergency_light_blue_sequence(edict_t *self);
void emergency_light_sequence(edict_t *self);
void EmergencyLightStutter(edict_t *self);
void exploding_heli_final(edict_t *self);
void exploding_heli_remove(edict_t *self);
void exploding_heli_think(edict_t *self);
void exploding_heli_think2(edict_t *self);
void fan_big_accelerate(edict_t *self);
void FinishRespawn(edict_t *self);
void fire_extinguisher_dying(edict_t *self);
void fireballBurn(edict_t *self);
void fireballRemove(edict_t *self);
void FireThink(edict_t *self);
void fish_sink(edict_t *self);
void fish_think(edict_t *self);
void flag_pole_sequence(edict_t *self);
void flameGlobThink(edict_t *self);
void flashpackExplode(edict_t *self);
void flatbed_accel(edict_t *self);
void flatbed_deccel(edict_t *self);
void flatbed_linecheck(edict_t *self);
void flatbed_remove(edict_t *self);
void flatbed_stop(edict_t *self);
void func_clock_think(edict_t *self);
void funcfade_think (edict_t *self);
void func_object_release(edict_t *self);
void func_remote_camera_think(edict_t *self);
void func_timer_think(edict_t *self);
void func_train_find(edict_t *self);
void funcshake_think(edict_t *self);
void G_FreeEdict(edict_t *self);
void generic_animal_init(edict_t *self);
void generic_armor_init(edict_t *self);
void generic_enemy_npc_init(edict_t *self);
void generic_grunt_init(edict_t *self);
void generic_heavyweapons_init(edict_t *self);
void generic_heli_init(edict_t *self);
void generic_leader_init(edict_t *self);
void generic_npc_init(edict_t *self);
void generic_snowcat_init (edict_t *self);
void generic_tank_init(edict_t *self);
void GetReadyForBlownPartAIToTakeOver(edict_t *ent);
void ghoul_model_think(edict_t *self);
void gib_think(edict_t *self);
void globe_spin(edict_t *self);
void grenadeThink(edict_t *self);
void gun_big_target (edict_t *self);
void guttedcow_swing(edict_t *self);
void HeliMissileRemove2(edict_t *self);
void hydrant_pulse(edict_t *self);
void infernoThink(edict_t *self);
void invis_attack_think(edict_t *self);
void iraq_fruit_stand_throw(edict_t *self);
void itemDropToFloor(edict_t *self);
void keep_using_think(edict_t *self);
void kill_lamp(edict_t *self);
void lamp_hang_swing(edict_t *self);
void LightningGenThink(edict_t *self);
void linetrapThink(edict_t *self);
void makebloodpool(edict_t *self);
void MissileCountdown(edict_t *self);
void MissileRemove(edict_t *self);
void MissileSplit(edict_t *ent);
void MolitovThink(edict_t *self);
void Move_Begin(edict_t *self);
void Move_Done(edict_t *self);
void Move_Final(edict_t *self);
void multi_wait(edict_t *self);
void Obj_tipoverthink(edict_t *self);
void ObjectStopMove(edict_t *self);
void pathtest_think(edict_t *self);
void plat_go_down(edict_t *self);
void plat_hit_bottom(edict_t *self);	
void plat_hit_top(edict_t *self);
void platInit(edict_t *self);
void portcullis_think(edict_t* self);
void rotateInit(edict_t *self);
void searchbeam_think(edict_t* ent);
void searchlight_think(edict_t *self);
void security_cam_rotate(edict_t *self);
void security_cam_scan(edict_t *self);
void sideobeef_swing(edict_t *self);
void sink_spurt(edict_t *self);
void skyfadeThink(edict_t *self);
void SnowcatW_GotoCoords(edict_t *ent);
void SnowcatW_FireCannonAtCoords(edict_t *ent);
void SnowcatW_MachGunAuto(edict_t *ent);
void soundGenInit(edict_t *self);
void spark_gen_think(edict_t *self);
void spawner_booster_defaultthink(edict_t *ent);
void spawnerInit(edict_t *ent);
void spawnerThink(edict_t *ent);
void speaker_spark(edict_t *self);
void SteamThink(edict_t *self);
void stoplight_change(edict_t *self);
void TankW_GotoCoords(edict_t *ent);
void TankW_FireCannonAtCoords(edict_t *ent);
void TankW_CannonAttack(edict_t *ent);
void TankW_MachGunAuto(edict_t *ent);
void TankW_Die(edict_t *ent);
void TankW_AimTurret(edict_t *ent);
void target_crosslevel_target_think(edict_t *self);
void target_earthquake_think(edict_t *self);
void target_explosion_explode(edict_t *self);
void target_laser_start(edict_t *self);
void target_laser_think(edict_t *self);
void target_lightramp_think(edict_t *self);
void Testboltzap(edict_t *self);
void Think_AccelMove(edict_t *self);
void Think_CalcMoveSpeed(edict_t *self);
void Think_Delay(edict_t *self);
void Think_SpawnDoorTrigger(edict_t *self);
void toilet_dribble(edict_t *self);
void toilet_pulse(edict_t *self);
void train_accel(edict_t *self);
void train_deccel(edict_t *self);
void train_getlength(edict_t *self);
void train_linecheck(edict_t *self);
void train_next(edict_t *self);
void train_stop(edict_t *self);
void train_wait(edict_t *self);
void trainarm_think(edict_t *self);
void trash_piece_think(edict_t *self);
void trashcan_barrel_fall(edict_t *self);
void trigger_countdown_think(edict_t *self);
void trigger_elevator_init(edict_t *self);
void tv_ceiling_sequence(edict_t *self);
void tv_damaged_think(edict_t *self);
void tv_wall_sequence(edict_t *self);
void urinal_cookie_think(edict_t *self);
void urinal_die2(edict_t *self);
void wet_floor_fall(edict_t *self);
void WhiteThink(edict_t *self);
void spawnerThink(edict_t *ent);
void fireEnemyMWave(edict_t *self);

void (*think_f[])(edict_t *self) =
{
	AngleMove_Begin,
	AngleMove_Done,
	AngleMove_Final,
	auto_gun_arc,
	auto_gun_target,
	banner_animate,
	barrel_chemical_fall,
	bed_deccel,
	bed_stop,
	blinds_stoppain,
	blownpartremove_think,
	boltDropNotSolid,
	bosnia_truck_remove,
	button_done,
	button_return,
	button_wait,
	buttonInit,
	c4Explode,
	cash_register_paindone,
	chair_squeak,
	chandelier_spin,
	CharThink,
	chunk_spewer_think,
	ClearEffects,
	Cobra2_OutOfControl,
	Cobra2AIHelper,
	Cobra2AutoFireHelper,
	Cobra2ChainGunEnableHelper,
	Cobra2DeathDestHelper,
	Cobra2FaceAbsDirHelper,
	Cobra2FaceAbsHelper,
	Cobra2FaceRelCoordsHelper,
	Cobra2FaceRelEntHelper,
	Cobra2FireAtAbsHelper,
	Cobra2FireAtRelEntHelper,
	Cobra2FireAtRelHelper,
	Cobra2GotoCoordsHelper,
	Cobra2GotoRelEntHelper,
	Cobra2HeadFaceAbsCoordsHelper,
	Cobra2HeadFaceRelCoordsHelper,
	Cobra2HeadFaceRelEntHelper,
	Cobra2HealthHelper,
	Cobra2HoverHelper,
	Cobra2LandHelper,
	Cobra2RearmHelper,
	Cobra2RocketsEnableHelper,
	Cobra2StrafeHelper,
	Cobra2TraceDimsHelper,
	Cobra2WorldHelper,
	Cobra2Think_AI,
	Cobra2Think_DipFireAt,
	Cobra2Think_FireAt,
	Cobra2Think_GotoFireAt,
	Cobra2Think_HeadToDest,
	Cobra2Think_Hover,
	Cobra2Think_Land,
	Cobra2Think_MovementFinish,
	Cobra2Think_Repair,
	Cobra2Think_SidestrafeToDest,
	Cobra2Think_SmartHeadToDest,
	Cobra2Think_TakeOff,
	Cobra2Think_TurnToFace,
	Cobra2TraceDimsHelper,
	Cobra2WorldHelper,
	cone_fall,
	dekker_init,
	desk_phone_effects,
	door_go_down,
	door_hit_bottom,
	door_hit_top,
	door_secret_done,
	door_secret_move1,
	door_secret_move2,
	door_secret_move3,
	door_secret_move4,
	door_secret_move5,
	door_secret_move6,
	door_spawn_areaportal_think,
	DoRespawn,
	drawMonkeyThink,
	dustsource_think,
	EmergencyLightFade,
	EmergencyLightRedGlow,
	EmergencyLightReturn,
	emergency_light_blue_sequence,
	emergency_light_sequence,
	EmergencyLightStutter,
	exploding_heli_final,
	exploding_heli_remove,
	exploding_heli_think,
	exploding_heli_think2,
	fan_big_accelerate,
	FinishRespawn,
	fireballBurn,
	fireballRemove,
	fire_extinguisher_dying,
	FireThink,
	fish_sink,
	fish_think,
	flag_pole_sequence,
	flameGlobThink,
	flashpackExplode,
	flatbed_accel,
	flatbed_deccel,
	flatbed_linecheck,
	flatbed_remove,
	flatbed_stop,
	func_clock_think,
	funcfade_think,
	func_object_release,
	func_remote_camera_think,
	func_timer_think,
	func_train_find,
	funcshake_think,
	G_FreeEdict,
	generic_animal_init,
	generic_armor_init,
	generic_enemy_npc_init,
	generic_grunt_init,
	generic_heavyweapons_init,
	generic_heli_init,
	generic_leader_init,
	generic_npc_init,
	generic_snowcat_init,
	generic_tank_init,
	GetReadyForBlownPartAIToTakeOver,
	ghoul_model_think,
	gib_think,
	globe_spin,
	grenadeThink,
	gun_big_target,
	guttedcow_swing,
	HeliMissileRemove2,
	hydrant_pulse,
	infernoThink,
	invis_attack_think,
	iraq_fruit_stand_throw,
	itemDropToFloor,
	keep_using_think,
	kill_lamp,
	lamp_hang_swing,
	linetrapThink,
	LightningGenThink,
	makebloodpool,
	MissileCountdown,
	MissileRemove,
	MissileSplit,
	MolitovThink,
	Move_Begin,
	Move_Done,
	Move_Final,
	multi_wait,
	Obj_tipoverthink,
	ObjectStopMove,
	pathtest_think,
	plat_go_down,
	plat_hit_bottom,
	plat_hit_top,
	platInit,
	portcullis_think,
	rotateInit,
	searchbeam_think,
	searchlight_think,
	security_cam_rotate,
	security_cam_scan,
	sideobeef_swing,
	sink_spurt,
	skyfadeThink,
	SnowcatW_GotoCoords,
	SnowcatW_FireCannonAtCoords,
	SnowcatW_MachGunAuto,
	soundGenInit,
	spark_gen_think,
	spawner_booster_defaultthink,
	spawnerInit,
	spawnerThink,
	speaker_spark,
	SteamThink,
	stoplight_change,
	TankW_GotoCoords,
	TankW_FireCannonAtCoords,
	TankW_CannonAttack,
	TankW_MachGunAuto,
	TankW_Die,
	TankW_AimTurret,
	target_crosslevel_target_think,
	target_earthquake_think,
	target_explosion_explode,
	target_laser_start,
	target_laser_think,
	target_lightramp_think,
	Testboltzap,
	Think_AccelMove,
	Think_CalcMoveSpeed,
	Think_Delay,
	Think_SpawnDoorTrigger,
	toilet_dribble,
	toilet_pulse,
	train_accel,
	train_deccel,
	train_getlength,
	train_linecheck,
	train_next,
	train_stop,
	train_wait,
	trainarm_think,
	trash_piece_think,
	trashcan_barrel_fall,
	trigger_countdown_think,
	trigger_elevator_init,
	tv_ceiling_sequence,
	tv_damaged_think,
	tv_wall_sequence,
	urinal_cookie_think,
	urinal_die2,
	wet_floor_fall,
	WhiteThink,
	spawnerThink,
	fireEnemyMWave
};

void door_blocked(edict_t *self, edict_t *other);
void door_secret_blocked(edict_t *self, edict_t *other);
void plat_blocked(edict_t *self, edict_t *other);
void rotating_blocked(edict_t *self, edict_t *other);
void train_blocked(edict_t *self, edict_t *other);

void (*blocked_f[])(edict_t *self, edict_t *other) =
{
	door_blocked,
	door_secret_blocked,
	plat_blocked,
	rotating_blocked,
	train_blocked
};

void apjack_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void barbwire_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void barrel_fire_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void BlownPartTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void button_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void c4Touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void car_rolls_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void Cobra2Touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void door_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void fireballCollide(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void flashTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void func_object_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void generic_monster_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void gib_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void grenadeTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void HeliMissileCollide2(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void hurt_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void infernoTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void itemAmmoTouch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void itemArmorTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void itemCashTouch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void itemEquipTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void itemHealthTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void itemWeaponTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void keep_using_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void KnifeThrowCollide(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void landmine_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void MissileCollide(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void MissileCollide2(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void MissileSmallCollide(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void molitovCollide(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void objectives_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void rotating_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void path_corner_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void PhosTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void point_combat_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void safezone_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void SimpleModelTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void sink_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void SlugCollide(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void StarThrowCollide(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void tabletouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void tank_jack_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void tanktouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void Touch_DoorTrigger(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void Touch_Multi(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void Touch_Plat_Center(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void touch_pushkillbox(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void train_smush(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void trigger_countdown_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void trigger_gravity_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void trigger_monsterjump_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void trigger_movedown_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void trigger_push_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void truck_touch (edict_t *ent, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void tv_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void urinal_cookie_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
void danglyTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);

void (*touch_f[])(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf) =
{
	apjack_touch,
	barbwire_touch,
	barrel_fire_touch,
	BlownPartTouch,
	button_touch,
	car_rolls_touch,
	c4Touch,
	Cobra2Touch,
	fireballCollide,
	door_touch,
	flashTouch,
	func_object_touch,
	generic_monster_touch,
	gib_touch,
	grenadeTouch,
	HeliMissileCollide2,
	hurt_touch,
	infernoTouch,
	itemAmmoTouch ,
	itemArmorTouch,
	itemCashTouch ,
	itemEquipTouch,
	itemHealthTouch,
	itemWeaponTouch,
	keep_using_touch,
	KnifeThrowCollide,
	landmine_touch,
	MissileCollide,
	MissileCollide2,
	MissileSmallCollide,
	molitovCollide,
	objectives_touch,
	path_corner_touch,
	PhosTouch,
	safezone_touch,
	point_combat_touch,
	rotating_touch,
	SimpleModelTouch,
	sink_touch,
	SlugCollide,
	StarThrowCollide,
	tabletouch,
	tank_jack_touch,
	tanktouch,
	Touch_DoorTrigger,
	Touch_Multi,
	Touch_Plat_Center,
	touch_pushkillbox,
	train_smush,
	trigger_countdown_touch,
	trigger_gravity_touch,
	trigger_monsterjump_touch,
	trigger_movedown_touch,
	trigger_push_touch,
	truck_touch,
	tv_touch,
	urinal_cookie_touch,
	danglyTouch,
};

void alarm_use(edict_t *self, edict_t *other, edict_t *activator);
void alley_wall_use(edict_t *self, edict_t *other, edict_t *activator);
void barrel_chemical_use(edict_t *self, edict_t *other, edict_t *activator);
void bed_go(edict_t *self, edict_t *other, edict_t *activator);
void bedwheels_use(edict_t *self, edict_t *other, edict_t *activator);
void breakable_brush_use(edict_t *self, edict_t *other, edict_t *activator);
void button_use(edict_t *self, edict_t *other, edict_t *activator);
void caged_use(edict_t *self, edict_t *other, edict_t *activator);
void candelabra_use (edict_t *ent, edict_t *other, edict_t *activator);
void car_european_use(edict_t *self, edict_t *other, edict_t *activator);
void car_rolls_use(edict_t *self, edict_t *other, edict_t *activator);
void chandelier_use (edict_t *ent, edict_t *other, edict_t *activator);
void chunk_spewer_use(edict_t *self, edict_t *other, edict_t *activator);
void Cobra2_ToggleGun(edict_t *self, edict_t *other, edict_t *activator);
void Dekker_split(edict_t *self, edict_t *other, edict_t *activator);
void desk_lamp_use (edict_t *ent, edict_t *other, edict_t *activator);
void door_secret_use(edict_t *self, edict_t *other, edict_t *activator);
void door_use(edict_t *self, edict_t *other, edict_t *activator);
void dustsource_use(edict_t *self, edict_t *other, edict_t *activator);
void effect_continual_use(edict_t *self, edict_t *other, edict_t *activator);
void effect_use(edict_t *self, edict_t *other, edict_t *activator);
void emergency_blue_use(edict_t *self, edict_t *other, edict_t *activator);
void emergency_lights_use(edict_t *self, edict_t *other, edict_t *activator);
void emergency_use(edict_t *self, edict_t *other, edict_t *activator);
void explodeUse(edict_t *self, edict_t *other, edict_t *activator);
void extinguisher_use(edict_t *self, edict_t *other, edict_t *activator);
void f_HtPG_use (edict_t *self, edict_t *other, edict_t *activator);
void fan_big_use(edict_t *self, edict_t *other, edict_t *activator);
void fire_use(edict_t *self, edict_t *other, edict_t *activator);
void fish_die(edict_t *self, edict_t *other, edict_t *activator);
void flatbed_go(edict_t *self, edict_t *other, edict_t *activator);
void flatbedwheels_use (edict_t *self, edict_t *other, edict_t *activator);
void forklift_use(edict_t *self, edict_t *other, edict_t *activator);
void func_ambientSetUse(edict_t *self, edict_t *other, edict_t *activator);
void func_clock_use(edict_t *self, edict_t *other, edict_t *activator);
void func_conveyor_use(edict_t *self, edict_t *other, edict_t *activator);
void func_musicControlUse(edict_t *self, edict_t *other, edict_t *activator);
void func_object_use(edict_t *self, edict_t *other, edict_t *activator);
void func_score_use(edict_t *self, edict_t *other, edict_t *activator);
void func_timer_use(edict_t *self, edict_t *other, edict_t *activator);
void func_wall_use(edict_t *self, edict_t *other, edict_t *activator);
void funcfade_use(edict_t *self, edict_t *other, edict_t *activator);
void funcshake_use(edict_t *self, edict_t *other, edict_t *activator);
void generic_wpn_c4_use(edict_t *self, edict_t *other, edict_t *activator);
void generic_monster_activate(edict_t *self, edict_t *other, edict_t *activator);
void gun_castle_use(edict_t *self, edict_t *other, edict_t *activator);
void hanging_use(edict_t *self, edict_t *other, edict_t *activator);
void hurt_use(edict_t *self, edict_t *other, edict_t *activator);
void ignite_use(edict_t *self, edict_t *other, edict_t *activator);
void infernoUse(edict_t *self, edict_t *other, edict_t *activator);
void invis_attack_use(edict_t *ent, edict_t *other, edict_t *activator);
void iraq_valve_use(edict_t *self, edict_t *other, edict_t *activator);
void lamp_hang_use(edict_t *self, edict_t *other, edict_t *activator);
void lantern_use (edict_t *ent, edict_t *other, edict_t *activator);
void lantern2_use(edict_t *self, edict_t *other, edict_t *activator);
void lantern3_use(edict_t *ent, edict_t *other, edict_t *activator);
void light_use(edict_t *self, edict_t *other, edict_t *activator);
void LighteningGenUse(edict_t *self, edict_t *other, edict_t *activator);
void linefountainUse(edict_t *self, edict_t *other, edict_t *activator);
void lineTrapUse(edict_t *self, edict_t *other, edict_t *activator);
void military_door_use(edict_t *self, edict_t *other, edict_t *activator);
void objectives_use(edict_t *self, edict_t *other, edict_t *activator);
void portcullis_use(edict_t *self, edict_t *other, edict_t *activator);
void raingen_use(edict_t *self, edict_t *other, edict_t *activator);
void rainTrigger_use(edict_t *self, edict_t *other, edict_t *activator);
void roadflare_use(edict_t *self, edict_t *other, edict_t *activator);
void rotating_use(edict_t *self, edict_t *other, edict_t *activator);
void safezone_use (edict_t *self, edict_t *other, edict_t *activator);
void script_use(edict_t *self, edict_t *other, edict_t *activator);
void shop_use(edict_t *self, edict_t *other, edict_t *activator);
void smoke_gen_use(edict_t *self, edict_t *other, edict_t *activator);
void smokeBurstUse(edict_t *self, edict_t *other, edict_t *activator);
void sound_gen_use(edict_t *self, edict_t *other, edict_t *activator);
void spark_gen_use(edict_t *self, edict_t *other, edict_t *activator);
void spawnerUse(edict_t *self, edict_t *other, edict_t *activator);
void street_modern_use(edict_t *self, edict_t *other, edict_t *activator);
void SteamUse(edict_t *self, edict_t *other, edict_t *activator);
void tank_gas_use(edict_t *self, edict_t *other, edict_t *activator);
void TankUse(edict_t *self, edict_t *other, edict_t *activator);
void target_earthquake_use(edict_t *self, edict_t *other, edict_t *activator);
void target_laser_use(edict_t *self, edict_t *other, edict_t *activator);
void target_lightramp_use(edict_t *self, edict_t *other, edict_t *activator);
void target_string_use(edict_t *self, edict_t *other, edict_t *activator);
void train_go(edict_t *self, edict_t *other, edict_t *activator);
void train_use(edict_t *self, edict_t *other, edict_t *activator);
void trainarm_spawn(edict_t *self, edict_t *other, edict_t *activator);
void trigger_activate_use (edict_t *self, edict_t *other, edict_t *activator);
void trigger_brush_use(edict_t *self, edict_t *other, edict_t *activator);
void trigger_countdown_use(edict_t *self, edict_t *other, edict_t *activator);
void trigger_counter_use(edict_t *self, edict_t *other, edict_t *activator);
void trigger_crosslevel_trigger_use(edict_t *self, edict_t *other, edict_t *activator);
void trigger_elevator_use(edict_t *self, edict_t *other, edict_t *activator);
void trigger_enable(edict_t *self, edict_t *other, edict_t *activator);
void trigger_push_use(edict_t *self, edict_t *other, edict_t *activator);
void trigger_relay_use(edict_t *self, edict_t *other, edict_t *activator);
void tv_ceiling_use(edict_t *self, edict_t *other, edict_t *activator);
void tv_wall_use(edict_t *self, edict_t *other, edict_t *activator);
void urinal_use(edict_t *self, edict_t *other, edict_t *activator);
void Use_Areaportal(edict_t *self, edict_t *other, edict_t *activator);
void Use_DoorTrigger(edict_t *self, edict_t *other, edict_t *activator);
void use_killbox(edict_t *self, edict_t *other, edict_t *activator);
void Use_Multi(edict_t *self, edict_t *other, edict_t *activator);
void Use_Plat(edict_t *self, edict_t *other, edict_t *activator);
void use_player_armor(edict_t *self, edict_t *other, edict_t *activator);
void use_player_health(edict_t *self, edict_t *other, edict_t *activator);
void use_player_item(edict_t *self, edict_t *other, edict_t *activator);
void Use_remote_camera(edict_t *self, edict_t *other, edict_t *activator);
void use_snipercam_zoom(edict_t *self, edict_t *other, edict_t *activator);
void use_target_changelevel(edict_t *self, edict_t *other, edict_t *activator);
void use_target_explosion(edict_t *self, edict_t *other, edict_t *activator);
void use_target_goal(edict_t *self, edict_t *other, edict_t *activator);
void Use_Target_Help(edict_t *self, edict_t *other, edict_t *activator);
void use_target_secret(edict_t *self, edict_t *other, edict_t *activator);
void use_target_spawner(edict_t *self, edict_t *other, edict_t *activator);
void Use_Target_Speaker(edict_t *self, edict_t *other, edict_t *activator);
void use_target_splash(edict_t *self, edict_t *other, edict_t *activator);
void Use_Target_Tent(edict_t *self, edict_t *other, edict_t *activator);
void valve_use_portcullis(edict_t *self, edict_t *other, edict_t *activator);
void trigger_sound_use(edict_t *self, edict_t *other, edict_t *activator);

void (*use_f[])(edict_t *self, edict_t *other, edict_t *activator) =
{
	alarm_use,
	alley_wall_use,
	barrel_chemical_use,
	bed_go,
	bedwheels_use,
	breakable_brush_use,
	button_use,
	caged_use,
	candelabra_use,
	car_european_use,
	car_rolls_use,
	chandelier_use,
	chunk_spewer_use,
	Cobra2_ToggleGun,
	Dekker_split,
	desk_lamp_use,
	door_secret_use,
	door_use,
	dustsource_use,
	effect_continual_use,
	effect_use,
	emergency_blue_use,
	emergency_lights_use,
	emergency_use,
	explodeUse,
	extinguisher_use,
	f_HtPG_use,
	fan_big_use,
	fire_use,
	fish_die,
	flatbed_go,
	flatbedwheels_use,
	forklift_use,
	func_ambientSetUse,
	func_clock_use,
	func_conveyor_use,
	func_musicControlUse,
	func_object_use,
	func_score_use,
	func_timer_use,
	func_wall_use,
	funcfade_use,
	funcshake_use,
	generic_wpn_c4_use,
	generic_monster_activate,
	gun_castle_use,
	hanging_use,
	hurt_use,
	ignite_use,
	infernoUse,
	invis_attack_use,
	iraq_valve_use,
	lamp_hang_use,
	lantern_use,
	lantern2_use,
	lantern3_use,
	light_use,
	LighteningGenUse,
	linefountainUse,
	lineTrapUse,
	military_door_use,
	objectives_use,
	portcullis_use,
	raingen_use,
	rainTrigger_use,
	roadflare_use,
	rotating_use,
	safezone_use,
	script_use,
	shop_use,
	smoke_gen_use,
	smokeBurstUse,
	sound_gen_use,
	spark_gen_use,
	spawnerUse,
	SteamUse,
	street_modern_use,
	tank_gas_use,
	TankUse,
	target_earthquake_use,
	target_laser_use,
	target_lightramp_use,
	target_string_use,
	train_go,
	train_use,
	trainarm_spawn,
	trigger_activate_use,
	trigger_brush_use,
	trigger_countdown_use,
	trigger_counter_use,
	trigger_crosslevel_trigger_use,
	trigger_elevator_use,
	trigger_enable,
	trigger_push_use,
	trigger_relay_use,
	tv_ceiling_use,
	tv_wall_use,
	urinal_use,
	Use_Areaportal,
	Use_DoorTrigger,
	use_killbox,
	Use_Multi,
	Use_Plat,
	use_player_armor,
	use_player_health,
	use_player_item,
	Use_remote_camera,
	use_snipercam_zoom,
	use_target_changelevel,
	use_target_explosion,
	use_target_goal,
	Use_Target_Help,
	use_target_secret,
	use_target_spawner,
	Use_Target_Speaker,
	use_target_splash,
	Use_Target_Tent,
	valve_use_portcullis,
	trigger_sound_use
};

void button_use(edict_t *self, edict_t *other, edict_t *activator);
void car_rolls_use(edict_t *self, edict_t *other, edict_t *activator);
void door_secret_use(edict_t *self, edict_t *other, edict_t *activator);
void extinguisher_use(edict_t *self, edict_t *other, edict_t *activator);
void generic_monster_use(edict_t *self, edict_t *other, edict_t *activator);
void keep_using_use(edict_t *self, edict_t *other, edict_t *activator);
void plDoorUse(edict_t *self, edict_t *other, edict_t *activator);
void trigger_heal_spent (edict_t *self,edict_t *other,edict_t *activator);
void trigger_heal_use (edict_t *self,edict_t *other,edict_t *activator);
void urinal_use(edict_t *self, edict_t *other, edict_t *activator);
void Use_DoorTrigger(edict_t *self, edict_t *other, edict_t *activator);
void Use_Multi(edict_t *self, edict_t *other, edict_t *activator);
void useable_use(edict_t *self, edict_t *other, edict_t *activator);

void (*pluse_f[])(edict_t *self, edict_t *other, edict_t *activator) =
{
	button_use,
	car_rolls_use,
	door_secret_use,
	extinguisher_use,
	generic_monster_use,
	keep_using_use,
	plDoorUse,
	trigger_heal_spent,
	trigger_heal_use,
	urinal_use,
	Use_DoorTrigger,
	Use_Multi,
	useable_use
};

void ashtray_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void ashtray_pain2(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void auto_gun_spark(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void banner_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit);
void barrel_chemical_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void benchbottom_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void blind_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void breakable_brush_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void broom_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void car_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void cash_register_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void chair_spin(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void chandelier_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void comp_keyboard_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void comp_monitor_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void comp_monitor2_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void cone_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void desk_phone_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void desk_phone_pain2(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void fire_extinguisher_explode(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void garbage_bag_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void generic_car_rolls_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void globe_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void gong_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void gore_pile_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void gumball_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void guttedcow_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void hanging_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void lamp_hang_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void landmine_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void lantern_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void motorcycle_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void news_machine_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void news_machine_pain2(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void Obj_partpain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void payphone_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void player_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void searchlight_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void searchlight_pain2(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void shop_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void sideobeef_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void sink_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void sofa_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void stoplight_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void toilet_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void trash_pile_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void trashcan_barrel_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void trashcan_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void tv_ceiling_pain1(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void tv_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void tv_pain2(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void tv_wall_pain1(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void urinal_cookie_toss(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void waste_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void waste_pain2(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void wet_floor_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);

void (*pain_f[])(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit) =
{
	ashtray_pain,
	ashtray_pain2,
	auto_gun_spark ,
	banner_pain,
	barrel_chemical_pain,
	benchbottom_pain,
	breakable_brush_pain,
	blind_pain,
	broom_pain,
	car_pain,
	cash_register_pain,
	chair_spin,
	chandelier_pain,
	comp_keyboard_pain,
	comp_monitor_pain,
	comp_monitor2_pain,
	cone_pain,
	desk_phone_pain,
	desk_phone_pain2,
	fire_extinguisher_explode,
	garbage_bag_pain,
	generic_car_rolls_pain,
	globe_pain,
	gong_pain,
	gore_pile_pain,
	gumball_pain,
	guttedcow_pain,
	hanging_pain,
	lamp_hang_pain,
	landmine_pain,
	lantern_pain,
	motorcycle_pain,
	news_machine_pain,
	news_machine_pain2,
	Obj_partpain,
	payphone_pain,
	player_pain,
	searchlight_pain,
	searchlight_pain2,
	shop_pain,
	sideobeef_pain,
	sink_pain,
	sofa_pain,
	stoplight_pain,
	toilet_pain ,
	trash_pile_pain,
	trashcan_barrel_pain,
	trashcan_pain ,
	tv_ceiling_pain1,
	tv_pain,
	tv_pain2,
	tv_wall_pain1,
	urinal_cookie_toss,
	waste_pain,
	waste_pain2,
	wet_floor_pain
};

void auto_gun_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void barrel_chemical_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void BecomeDebris(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void big_gun_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void bosnia_truck_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void breakable_brush_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void button_killed(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void caged_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void comp_monitor_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void debris_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void door_killed(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void door_secret_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void fire_extinguisher_boom(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void fire_extinguisher_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void flatbed_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void G_Explode(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void gib_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void hydrant_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void iraq_fruit_stand_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void iraq_fruit_stand_fruit_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void landmine_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void lantern2_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void LightBecomeDebris(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void ObjBecomeDebris(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void player_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void sink_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void speaker_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void SpecialBecomeDebris(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void tank_gas_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void tank_propane_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void toilet_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void trashcan_death(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void trigger_brush_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void tv_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void urinal_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);

void (*die_f[])(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point) =
{
	auto_gun_die,
	barrel_chemical_die,
	BecomeDebris,
	big_gun_die,
	bosnia_truck_die,
	breakable_brush_die,
	button_killed,
	caged_die,
	comp_monitor_die,
	debris_die,
	door_killed,
	door_secret_die,
	fire_extinguisher_boom,
	fire_extinguisher_die,
	flatbed_die,
	G_Explode,
	gib_die,
	iraq_fruit_stand_die,
	hydrant_die,
	iraq_fruit_stand_fruit_die,
	landmine_die,
	lantern2_die,
	LightBecomeDebris,
	ObjBecomeDebris,
	player_die,
	sink_die,
	speaker_die,
	SpecialBecomeDebris,
	tank_gas_die,
	tank_propane_die,
	toilet_die,
	trashcan_death,
	trigger_brush_die,
	tv_die,
	urinal_die
};

void SP_misc_generic_fire_extinguisher(edict_t *self);
void SP_misc_generic_train(edict_t *self);

void (*respawn_f[])(edict_t *self) =
{
	SP_misc_generic_fire_extinguisher,
	SP_misc_generic_train
};

extern BannerCallback			theBannerCallback;         
extern DropWeaponCallBack		TheDropWeaponCallBack; 
         
extern FireCallBack				TheDogFireCallBack;
extern FollowGenericPivot		TheDogFollowGenericPivot;
extern FootLeftCallback			TheDogFootLeftCallback;
extern FootRightCallback		TheDogFootRightCallback;
extern GroundCallback			TheDogGroundCallback;
extern InAirCallBack			TheDogInAirCallBack;
extern JumpCallBack				TheDogJumpCallBack;
extern KneeCallBack				TheDogKneeCallBack;
extern MonsterSeqBeginCallback	TheDogMonsterSeqBeginCallback;
extern MonsterSeqEndCallback	TheDogMonsterSeqEndCallback;
extern MouthCallback			TheDogMouthCallback;
extern SetVectorCallback		TheDogSetVectorCallback;
extern ThudCallBack				TheDogThudCallBack;

extern GroundCallback			TheCowGroundCallback;
extern SetVectorCallback		TheCowSetVectorCallback;
extern FireCallBack				TheCowFireCallBack;
extern FollowGenericPivot		TheCowFollowGenericPivot;
extern MouthCallback			TheCowMouthCallback;
extern FootRightCallback		TheCowFootRightCallback;
extern FootLeftCallback			TheCowFootLeftCallback;
extern MonsterSeqEndCallback	TheCowMonsterSeqEndCallback;
extern MonsterSeqBeginCallback	TheCowMonsterSeqBeginCallback;

extern FireCallBack				TheFireCallBack;           
extern FollowGenericPivot		TheFollowGenericPivot;     
extern FootLeftCallback			TheFootLeftCallback;       
extern FootRightCallback		TheFootRightCallback;      
extern GroundCallback			TheGroundCallback;         
extern gutCallback				theGutCallback;            
extern GuttedCowCallback		theGuttedCowCallback;      
extern HanginLightCallback		theHanginLightCallback;    
extern InAirCallBack			TheInAirCallBack;          
extern JumpCallBack				TheJumpCallBack;           
extern KneeCallBack				TheKneeCallBack;           
extern MainHeliCallback2		theMainHeliCallback2;      
extern MainRotor2Callback		theMainRotor2Callback;
extern TankTreadCallback		theTankTreadCallback;
extern MonsterSeqBeginCallback	TheMonsterSeqBeginCallback;
extern MonsterSeqEndCallback	TheMonsterSeqEndCallback;  
extern MonsterSoundCallBack		TheMonsterSoundCallBack;   
extern MouthCallback			TheMouthCallback;          
extern SetVectorCallback		TheSetVectorCallback;      
extern SideOBeefCallback		theSideOBeefCallback;      
extern TestSeqEndCallback		theTestSeqEndCallback;     
extern ThrowCallBack			TheThrowCallBack;          
extern ThudCallBack				TheThudCallBack;           
extern valveCallback			theValveCallback;       
extern PickupRespawnCallback	thePickupRespawnCallback;
extern LimbEOSCallback			TheLimbEOSCallback;
extern SnowcatTreadCallback		theSnowcatTreadCallback;

IGhoulCallBack *IGhoulCallbackList[] =
{
	&theBannerCallback,
	&TheDogFireCallBack,           
	&TheDogFollowGenericPivot,     
	&TheDogFootLeftCallback,       
	&TheDogFootRightCallback,      
	&TheDogGroundCallback,
	&TheDogInAirCallBack,          
	&TheDogJumpCallBack,           
	&TheDogKneeCallBack,           
	&TheDogMonsterSeqBeginCallback,
	&TheDogMonsterSeqEndCallback,  
	&TheDogMouthCallback,          
	&TheDogSetVectorCallback,      
	&TheDogThudCallBack,           
	&TheCowGroundCallback,
	&TheCowSetVectorCallback,
	&TheCowFireCallBack,
	&TheCowFollowGenericPivot,
	&TheCowMouthCallback,
	&TheCowFootRightCallback,
	&TheCowFootLeftCallback,
	&TheCowMonsterSeqEndCallback,
	&TheCowMonsterSeqBeginCallback,
	&TheDropWeaponCallBack,
	&TheFireCallBack,
	&TheFollowGenericPivot,
	&TheFootLeftCallback,
	&TheFootRightCallback,
	&TheGroundCallback,
	&theGutCallback,
	&theGuttedCowCallback,
	&theHanginLightCallback,
	&TheInAirCallBack,
	&TheJumpCallBack,
	&TheKneeCallBack,
	&theMainHeliCallback2, 
	&theMainRotor2Callback,
	&theTankTreadCallback,
	&TheMonsterSeqBeginCallback,
	&TheMonsterSeqEndCallback,
	&TheMonsterSoundCallBack,
	&TheMouthCallback,
	&TheSetVectorCallback,
	&theSideOBeefCallback,
	&theTestSeqEndCallback,
	&TheThudCallBack,
	&TheThrowCallBack,
	&theValveCallback,
	&thePickupRespawnCallback,
	&TheLimbEOSCallback,
	&theSnowcatTreadCallback
};

int function_table_sizes[] =
{
	sizeof(think_f),
	sizeof(blocked_f),
	sizeof(touch_f),
	sizeof(use_f),
	sizeof(pluse_f),
	sizeof(pain_f),
	sizeof(die_f),
	sizeof(respawn_f),
	sizeof(IGhoulCallbackList)
};

// -------------------------------------------------

typedef struct tables_s
{
	byte	*base;
	int		count;
} tables_t;

#define MMOVE_SIZE	4

extern mmove_t ScriptReleaseMove[];
extern mmove_t MMoves[];
extern mmove_t MDogMoves[];
extern mmove_t MCowMoves[];

tables_t mmove_tables[MMOVE_SIZE] =
{
	{ (byte *)ScriptReleaseMove, MMOVE_SCRIPT_SIZE },
	{ (byte *)MMoves, MMOVE_HUMAN_SIZE },
	{ (byte *)MDogMoves, MMOVE_DOG_SIZE },
	{ (byte *)MCowMoves, MMOVE_COW_SIZE }
};

#define MSD_SIZE	13

extern modelSpawnData_t bosniaModelData[];
extern modelSpawnData_t castleModelData[];
extern modelSpawnData_t genericModelData[];
extern modelSpawnData_t iraqModelData[];
extern modelSpawnData_t pickupModelData[];
extern modelSpawnData_t lightModelData[];
extern modelSpawnData_t nyModelData[];
extern modelSpawnData_t siberiaModelData[];
extern modelSpawnData_t tokyoModelData[];
extern modelSpawnData_t ugandaModelData[];
extern modelSpawnData_t heliModelData[];
extern modelSpawnData_t tankModelData[];
extern modelSpawnData_t snowcatModelData[];

tables_t msd_tables[MSD_SIZE] =
{
	{ (byte *)bosniaModelData,  MD_BOSNIA_SIZE	},
	{ (byte *)castleModelData,  MD_CASTLE_SIZE	},
	{ (byte *)genericModelData, MD_GENERIC_SIZE	},
	{ (byte *)iraqModelData,    MD_IRAQ_SIZE	},
	{ (byte *)pickupModelData,  MD_ITEM_SIZE	},
	{ (byte *)lightModelData,   MD_LIGHT_SIZE	},
	{ (byte *)nyModelData,	    MD_NY_SIZE		},
	{ (byte *)siberiaModelData, MD_SIBERIA_SIZE	},
	{ (byte *)tokyoModelData,   MD_TOKYO_SIZE	},
	{ (byte *)ugandaModelData,  MD_UGANDA_SIZE	},
	{ (byte *)heliModelData,	MD_HELI_SIZE	},
	{ (byte *)tankModelData,	MD_TANK_SIZE	},
	{ (byte *)snowcatModelData,	MD_SNOWCAT_SIZE	}
};

// -------------------------------------------------

field_t savefields[] =
{
	{"", FOFS(client), F_CLIENT},
	{"", FOFS(owner), F_EDICT},
	{"", FOFS(model), F_STRING},
	{"", FOFS(message), F_STRING},
	{"", FOFS(classname), F_STRING},
	{"", FOFS(target), F_STRING},
	{"", FOFS(targetname), F_STRING},
	{"", FOFS(scripttarget), F_STRING},
	{"", FOFS(killtarget), F_STRING},
	{"", FOFS(killfacing), F_STRING},
	{"", FOFS(team), F_STRING},
	{"", FOFS(pathtarget), F_STRING},
	{"", FOFS(deathtarget), F_STRING},
	{"", FOFS(combattarget), F_STRING},
	{"", FOFS(soundName), F_STRING},
	{"", FOFS(spawn1), F_STRING},
	{"", FOFS(spawn2), F_STRING},
	{"", FOFS(spawn3), F_STRING},
	{"", FOFS(target_ent), F_EDICT},
	{"", FOFS(goalentity), F_EDICT},
	{"", FOFS(movetarget), F_EDICT},

	{"", FOFS(wait_action1), F_MMOVE},
	{"", FOFS(wait_action2), F_MMOVE},
	{"", FOFS(wait_action3), F_MMOVE},
	{"", FOFS(wait_action4), F_MMOVE},
	{"", FOFS(wait_action5), F_MMOVE},
	{"", FOFS(wait_action6), F_MMOVE},
	{"", FOFS(move_action), F_MMOVE},

	{"", FOFS(objSpawnData), F_OSD},

	{"", FOFS(map), F_STRING},
	{"", FOFS(chain), F_EDICT},
	{"", FOFS(enemy), F_EDICT},
	{"", FOFS(oldenemy), F_EDICT},
	{"", FOFS(activator), F_EDICT},
	{"", FOFS(groundentity), F_EDICT},
	{"", FOFS(teamchain), F_EDICT},
	{"", FOFS(teammaster), F_EDICT},
	{"", FOFS(mynoise), F_EDICT},
	{"", FOFS(mynoise2), F_EDICT},

	{"", FOFS(think), F_THINK_F},
	{"", FOFS(moveinfo.endfunc), F_THINK_F},
	{"", FOFS(blocked), F_BLOCKED_F},
	{"", FOFS(touch), F_TOUCH_F},
	{"", FOFS(use), F_USE_F},
	{"", FOFS(plUse), F_PLUSE_F},
	{"", FOFS(pain), F_PAIN_F},
	{"", FOFS(die), F_DIE_F},
	{"", FOFS(respawnFunc), F_RESPAWN_F},

	{"", FOFS(ai), F_BOOLPTR},
	{"", FOFS(Script), F_BOOLPTR},
	{"", FOFS(objInfo), F_NULL},

	{NULL, 0, F_IGNORE}
};

field_t levelfields[] =
{
	{"", LLOFS(changemap), F_STRING},

	{"", LLOFS(sight_client), F_EDICT},
	{"", LLOFS(sight_monster), F_EDICT},
	{"", LLOFS(sight_entity), F_EDICT},
	{"", LLOFS(sound_entity), F_EDICT},
	{"", LLOFS(sound2_entity), F_EDICT},
	{"", LLOFS(current_entity), F_EDICT},

	{NULL, 0, F_IGNORE}
};

field_t clientfields[] =
{
	{"", CLOFS(inv), F_BOOLPTR},
	{"", CLOFS(body), F_BOOLPTR},
	{"", CLOFS(ps.gun), F_NULL},

	{NULL, 0, F_IGNORE}
};

//=========================================================

list<string>	stringlist;

int GetStringNum(char *str)
{
	assert(str != (char *)0xcdcdcdcd);
	// Null pointers have an index of -1
	if(!str)
	{
		return(-1);
	}
	stringlist.push_back(str);
	return(strlen(str) + 1);
}

int GetEdictNum(edict_t *e)
{
	assert(e != (edict_t *)0xcdcdcdcd);

	if(e == NULL)
	{
		return(-1);
	}
	else
	{
		return(e - g_edicts);
	}
}

int GetClientNum(gclient_t *c)
{
	assert(c != (gclient_t *)0xcdcdcdcd);

	if(c == NULL)
	{
		return(-1);
	}
	else
	{
		return(c - game.clients);
	}
}

int GetFunctionIdx(void **func_table, void *func, int table_size, char *str)
{
	int		i;

	assert(func != (void *)0xcdcdcdcd);
	// Null functions are perfectly valid
	if(!func)
	{
		return(-1);
	}
	// Enumerate function
	for(i = 0; i < (table_size / sizeof(func)); i++)
	{
		if(func_table[i] == func)
		{
			return(i);
		}
	}
#if (!_FINAL_) && (_RAVEN_)
	Com_Printf(P_RED "Unable to enumerate %s function %s\n", str, gi.GetLabel(func));
#else
	Com_Printf(P_RED "Unable to enumerate %s function\n", str);
#endif
	return(-1);
}

int GetThinkNum(void *func)
{
	return(GetFunctionIdx((void **)think_f, func, sizeof(think_f), "think"));
}

int GetBiTableIndex(byte *ptr, tables_t *table, int count, int size)
{
	int		i, j;

	assert(ptr != (byte *)0xcdcdcdcd);
	// Null pointers are perfectly valid
	if(!ptr)
	{
		return(-1);
	}
	for(j = 0; j < count; j++)
	{
		i = (ptr - table[j].base) / size;
		if((i >= 0) && (i < table[j].count))
		{
			return((j << 16) | i);
		}
	}
	Com_Printf(P_RED "Unable to enumerate bi table pointer\n");
	return(-1);
}

int GetMmoveNum(mmove_t *mm)
{
	return(GetBiTableIndex((byte *)mm, mmove_tables, MMOVE_SIZE, sizeof(mmove_t)));
}

int GetMSDNum(modelSpawnData_t *msd)
{
	return(GetBiTableIndex((byte *)msd, msd_tables, MSD_SIZE, sizeof(modelSpawnData_t)));
}

int GetEventNum(Event *ev)
{
	list<CScript *>::iterator	it;
	int							i, j;

	assert(ev != (Event *)0xcdcdcdcd);
	if(!ev)
	{
		return(-1);
	}
	for(it = Scripts.begin(), i = 0; it != Scripts.end(); it++, i++)
	{
		j = (*it)->GetEventNum(ev);
		if(j >= 0)
		{
			return((i << 16) | j);
		}
	}
	Com_Printf(P_RED "Unable to enumerate event\n");
	return(-1);
}

int GetVarNum(void *var)
{
	assert(var != (void *)0xcdcdcdcd);
	// SPECIAL CASE!! Null pointers have an index of 0 
	if(!var)
	{
		return(NULL);
	}
	// Only need to know whether pointer exists or not
	return(-1);
}

void EnumerateField(field_t *field, byte *base)
{
	void		*p;

	p = (void *)(base + field->ofs);
	switch (field->type)
	{
	case F_INT:
	case F_SHORT:
	case F_FLOAT:
		break;
	case F_STRING:
		*(int *)p = GetStringNum(*(char **)p);
		break;

	case F_VECTOR:
	case F_ANGLEHACK:
		break;

	case F_EDICT:
		*(int *)p = GetEdictNum(*(edict_t **)p);
		break;
	case F_ITEM:
		// Fixme
		*(int *)p = -1;
		break;
	case F_MMOVE:
		*(int *)p = GetMmoveNum(*(mmove_t **)p);
		break;
	case F_OSD:
		*(int *)p = GetMSDNum(*(modelSpawnData_t **)p);
		break;
	case F_CLIENT:
		*(int *)p = GetClientNum(*(gclient_t **)p);
		break;

	// These are placed in when their relevant owners are loaded
	case F_BOOLPTR:
		*(qboolean *)p = !!(*(int *)p);
		break;

	// These are pointers that are always recreated
	case F_NULL:
		*(void **)p = NULL;
		break;

	// Enumerate function pointers so as to make the loadgames not completely
	// sensitive to minor code changes
	case F_THINK_F:
		*(int *)p = GetFunctionIdx((void **)think_f, *(void **)p, sizeof(think_f), "think");
		break;
	case F_BLOCKED_F:
		*(int *)p = GetFunctionIdx((void **)blocked_f, *(void **)p, sizeof(blocked_f), "blocked");
		break;
	case F_TOUCH_F:
		*(int *)p = GetFunctionIdx((void **)touch_f, *(void **)p, sizeof(touch_f), "touch");
		break;
	case F_USE_F:
		*(int *)p = GetFunctionIdx((void **)use_f, *(void **)p, sizeof(use_f), "use");
		break;
	case F_PLUSE_F:
		*(int *)p = GetFunctionIdx((void **)pluse_f, *(void **)p, sizeof(pluse_f), "pluse");
		break;
	case F_PAIN_F:
		*(int *)p = GetFunctionIdx((void **)pain_f, *(void **)p, sizeof(pain_f), "pain");
		break;
	case F_DIE_F:
		*(int *)p = GetFunctionIdx((void **)die_f, *(void **)p, sizeof(die_f), "die");
		break;
	case F_RESPAWN_F:
		*(int *)p = GetFunctionIdx((void **)respawn_f, *(void **)p, sizeof(respawn_f), "respawn");
		break;

	case F_IGNORE:
		break;

	default:
		gi.error ("EnumerateField: unknown field type");
		break;
	}
}

void EnumerateFields(field_t *fields, byte *data, unsigned long chid, int len)
{
	field_t						*field;
	int							i;
	string						str;
	list<string>::iterator		it;

	assert(stringlist.empty());

	// Enumerate all the fields
	if(fields)
	{
		for(field = fields; field->name; field++)
		{
			EnumerateField(field, data);
		}
	}
	// Save out raw data
	gi.AppendToSavegame(chid, data, len);

	// Save out any associated strings
	it = stringlist.begin();
	for(i = 0; i < stringlist.size(); i++, it++)
	{
		gi.AppendToSavegame('STRG', (void *)(*it).c_str(), (*it).length() + 1);
	}

	// Make sure everything is cleaned up nicely
	stringlist.clear();
}

//=========================================================

char *GetStringPtr(int str)
{
	char	*strdata;

	if(str == -1)
	{
		return(NULL);
	}
	gi.ReadFromSavegame('STRG', NULL, str, (void **)&strdata);

	return(strdata);
}

edict_t *GetEdictPtr(int e)
{
	if(e == -1)
	{
		return(NULL);
	}
	assert(e >= 0);
	assert(e < 1024);
	return(g_edicts + e);
}

gclient_t *GetClientPtr(int c)
{
	if(c == -1)
	{
		return(NULL);
	}
	assert(c >= 0);
	assert(c < 256);
	return(game.clients + c);
}

void *GetFunctionPtr(void **func_table, int func, int table_size, char *str)
{
	assert(func < 4096);
	assert(func != 0xcdcdcdcd);
	// Index of -1 means Null function ptr
	if(func == -1)
	{
		return(NULL);
	}
	if((func < 0) || (func >= (table_size / sizeof(func))))
	{
		Com_Printf(va(P_RED "Unable to evaluate %s function %d\n", str, func));
		return(NULL);
	}
	return(func_table[func]);
}

void *GetThinkPtr(int func)
{
	return(GetFunctionPtr((void **)think_f, func, sizeof(think_f), "think"));
}

byte *GetBiTablePtr(int index, tables_t *table, int size)
{
	int		base, idx;

	if(index == -1)
	{
		return(NULL);
	}
	base = index >> 16;
	idx = index & 0xffff;

	return(table[base].base + (idx * size));
}

mmove_t *GetMmovePtr(int idx)
{
	return((mmove_t *)GetBiTablePtr(idx, mmove_tables, sizeof(mmove_t)));
}

modelSpawnData_t *GetMSDPtr(int idx)
{
	return((modelSpawnData_t *)GetBiTablePtr(idx, msd_tables, sizeof(modelSpawnData_t)));
}

Event *GetEventPtr(int ev)
{
	list<CScript *>::iterator	it;
	int							i;
	Event						*event;

	if(ev == -1)
	{
		return(NULL);
	}
	it = Scripts.begin();
	for(i = 0; i < (ev >> 16); i++)
	{
		it++;
	}
	event = (*it)->GetEventPtr(ev & 0xffff);
	if(!event)
	{
		Com_Printf(P_RED "Unable to evaluate event\n");
	}
	return(event);
}

void EvaluateField(field_t *field, byte *base)
{
	void		*p;

	p = (void *)(base + field->ofs);
	switch (field->type)
	{
	case F_INT:
	case F_SHORT:
	case F_FLOAT:
		break;
	case F_STRING:
		*(char **)p = GetStringPtr(*(int *)p);
		break;

	case F_VECTOR:
	case F_ANGLEHACK:
		break;

	case F_EDICT:
		*(edict_t **)p = GetEdictPtr(*(int *)p);
		break;
	case F_ITEM:
		// Fixme
		*(int *)p = 0;
		break;
	case F_MMOVE:
		*(mmove_t **)p = GetMmovePtr(*(int *)p);
		break;
	case F_OSD:
		*(modelSpawnData_t **)p = GetMSDPtr(*(int *)p);
		break;
	case F_CLIENT:
		*(gclient_t **)p = GetClientPtr(*(int *)p);
		break;

	// These fields are patched in when their relevant owners are loaded
	case F_BOOLPTR:
	case F_NULL:
		break;

	// Reevaluate function pointers from indices
	case F_THINK_F:
		*(void **)p = GetFunctionPtr((void **)think_f, *(int *)p, sizeof(think_f), "think");
		break;
	case F_BLOCKED_F:
		*(void **)p = GetFunctionPtr((void **)blocked_f, *(int *)p, sizeof(blocked_f), "blocked");
		break;
	case F_TOUCH_F:
		*(void **)p = GetFunctionPtr((void **)touch_f, *(int *)p, sizeof(touch_f), "touch");
		break;
	case F_USE_F:
		*(void **)p = GetFunctionPtr((void **)use_f, *(int *)p, sizeof(use_f), "use");
		break;
	case F_PLUSE_F:
		*(void **)p = GetFunctionPtr((void **)pluse_f, *(int *)p, sizeof(pluse_f), "pluse");
		break;
	case F_PAIN_F:
		*(void **)p = GetFunctionPtr((void **)pain_f, *(int *)p, sizeof(pain_f), "pain");
		break;
	case F_DIE_F:
		*(void **)p = GetFunctionPtr((void **)die_f, *(int *)p, sizeof(die_f), "die");
		break;
	case F_RESPAWN_F:
		*(void **)p = GetFunctionPtr((void **)respawn_f, *(int *)p, sizeof(respawn_f), "respawn");
		break;

	case F_IGNORE:
		break;

	default:
		gi.error ("EvaluateField: unknown field type");
		break;
	}
}

void EvaluateFields(field_t *fields, byte *data, unsigned long chid, int size)
{
	field_t	*field;

	// Read in field from disk
	gi.ReadFromSavegame(chid, (void *)data, size);

	// Convert indices to pointers
	if(fields)
	{
		for(field = fields; field->name; field++)
		{
			EvaluateField(field, data);
		}
	}
}

//=========================================================

map<GhoulUUID, GhoulUUID>	UUIDRemap;

GhoulUUID GetRemappedUUID(GhoulUUID old)
{
	map<GhoulUUID, GhoulUUID>::iterator	it;

	it = UUIDRemap.find(old);
	if(it == UUIDRemap.end())
	{
		assert(0);
		return(0);
	}
	return((*it).second);
}

void RemapUUIDs(void)
{
	int				i;
	edict_t			*ent;
	eft_rotate_t	*rot;

	for(i = 0, ent = g_edicts; i < globals.num_edicts; i++, ent++)
	{
		if(!ent->inuse)
		{
			continue;
		}
		if(ent->s.effects & EF_ROTATE)
		{
			rot = (eft_rotate_t *)ent->s.effectData;
			if(rot->boltonID)
			{
				rot->boltonID = GetRemappedUUID(rot->boltonID);
			}
			if(rot->boltonID2)
			{
				rot->boltonID2 = GetRemappedUUID(rot->boltonID2);
			}
			if(rot->boltonID3)
			{
				rot->boltonID3 = GetRemappedUUID(rot->boltonID3);
			}
			if(rot->boltonID4)
			{
				rot->boltonID4 = GetRemappedUUID(rot->boltonID4);
			}
		}
	}
	UUIDRemap.clear();
}

void SaveGhoulInst(IGhoulInst *inst, const char *name)
{
	byte	buffer[MAX_GHOULINST_SIZE];
	int		size;

	size = inst->SaveState(buffer, MAX_GHOULINST_SIZE, IGhoulCallbackList, sizeof(IGhoulCallbackList) / 4, name);
	gi.AppendToSavegame('GHLI', buffer, size);
}

IGhoulInst *LoadGhoulInst(IGhoulObj *obj, const char *name)
{
	IGhoulInst	*inst;
	byte		buffer[MAX_GHOULINST_SIZE];
	int			size;
	GhoulUUID	id;

	size = gi.ReadFromSavegame('GHLI', buffer, 0);

	inst = obj->NewInst();
	id = inst->RestoreState(buffer, size, IGhoulCallbackList, sizeof(IGhoulCallbackList) / 4, name);

	UUIDRemap[id] = inst->MyUUID();
	return(inst);
}

//=========================================================

#if	_DEBUG
/*
=================
ValidateLoadedData

Make sure we have no silly numbers anywhere
=================
*/

void ValidateLoadedData()
{
	int			i;
	edict_t		*ent;

	ent = g_edicts;
	for(i = 0; i < globals.num_edicts; i++, ent++)
	{
		if(ent->ai == (ai_public_c *)1)
		{
			assert(0);
		}
	}
}
#endif

/*
============
WriteGame

This will be called whenever the game goes to a new level,
and when the user explicitly saves the game.

Game information include cross level data, like multi level
triggers, help computer info, and all client states.

A single player death will automatically restore from the
last save position.
============
*/
void WriteGame (bool autosave)
{
	int				i;
	gclient_t		temp;
	game_locals_t	gtemp;

	if(!autosave)
	{
		PreserveClientData ();
	}
	gtemp = game;
	gtemp.autosaved = autosave;
	gtemp.clients = NULL;
	gi.AppendToSavegame('GAME', &gtemp, sizeof(game_locals_t));

	for(i = 0; i < game.maxclients; i++)
	{
		temp = game.clients[i];
		// Enumerate pointer fields
		EnumerateFields(clientfields, (byte *)&temp, 'CLNT', sizeof(gclient_t));
		// Save out inventory
		if(temp.inv)
		{
			game.clients[i].inv->Write();
		}
	}
}

bool ReadGame (bool autosave)
{
	int			i;
	gclient_t	*clients;

	(**gi.isClient) = 0;

	clients = game.clients;

	gi.ReadFromSavegame('GAME', (void *)&game, sizeof(game_locals_t));

	game.clients = clients;
	for(i = 0; i < game.maxclients; i++)
	{
		// Reevaluate all fields
		EvaluateFields(clientfields, (byte *)&game.clients[i], 'CLNT', sizeof(gclient_t));
		// Need to read in inventory
		if(game.clients[i].inv)
		{
			game.clients[i].inv = (invPub_c *)pe->NewInv();
			game.clients[i].inv->Read();
		}
		// Need to recreate dmInfo
		game.clients[i].dmInfo = new player_dmInfo_ic();
	}
	return(game.autosaved);
}

//==========================================================

/*
==============
WriteLevelLocals

All pointer variables (except function pointers) must be handled specially.
==============
*/
void WriteLevelLocals ()
{
	level_locals_t 	temp;

	// Copy out all data into a temp space
	temp = level;
	// Enumerate any fields that require enumeration
	EnumerateFields(levelfields, (byte *)&temp, 'LVLC', sizeof(level_locals_t));
}

/*
==============
ReadLevelLocals

All pointer variables (except function pointers) must be handled specially.
==============
*/
void ReadLevelLocals ()
{
	// Evaluate any pointers
	EvaluateFields(levelfields, (byte *)&level, 'LVLC', sizeof(level_locals_t));
}

/*
==============
WriteObjInfo

==============
*/

void WriteObjInfo()
{
	int		i, count;
	edict_t	*ent;

	for(i = 0, ent = g_edicts, count = 0; i < globals.num_edicts; i++, ent++)
	{
		if(ent->objInfo)
		{
			count++;
		}
	}
	gi.AppendToSavegame('OINM', &count, sizeof(count));

	for(i = 0, ent = g_edicts, count = 0; i < globals.num_edicts; i++, ent++)
	{
		if(ent->objInfo)
		{
			ent->objInfo->Write();
		}
	}
}

/*
==============
ReadObjInfo

==============
*/

void ReadObjInfo()
{
	int				i, count;		 
	baseObjInfo_c	*obj;

	gi.ReadFromSavegame('OINM', &count, sizeof(count));

	for(i = 0; i < count; i++)
	{
		obj = new baseObjInfo_c((edict_t *)NULL);
		obj->Read();
	}
}

/*
==============
WriteEdicts

All pointer variables (except function pointers) must be handled specially.
==============
*/

void WriteEdicts()
{
	int				i, count;
	edict_t			*ent;
	edict_t			temp;

	// Find number of edicts to write
	ent = g_edicts;
	count = 0;
	for(i = 0; i < globals.num_edicts; i++, ent++)
	{
		if(ent->inuse)
		{
		   count++;
		}
	}
	// Save this out
	gi.AppendToSavegame('NMED', &count, sizeof(int));

	// Scan thru and save out active edicts
	ent = g_edicts;
	for(i = 0; i < globals.num_edicts; i++, ent++)
	{
		if (!ent->inuse)
		{
			continue;
		}
		// Write out edict number and edict data
		gi.AppendToSavegame('EDNM', (void *)&i, sizeof(int));

		// Unlink from the world
		gi.unlinkentity(ent);
		// Copy Out data
		temp = *ent;
		// Link back in
		gi.linkentity(ent);
		// Enumerate any fields that require it
		EnumerateFields(savefields, (byte *)&temp, 'EDCT', sizeof(edict_t));
	}
}

/*
==============
ReadEdicts

All pointer variables (except function pointers) must be handled specially.
==============
*/

void ReadEdicts()
{
	int		i, count, entnum;
	edict_t	*ent;

	// Get number of edicts to read
	gi.ReadFromSavegame('NMED', (void *)&count, sizeof(int));
	// Read in and setup each edict
	for(i = 0; i < count; i++)
	{
		gi.ReadFromSavegame('EDNM', (void *)&entnum, sizeof(int));
		if (entnum >= globals.num_edicts)
		{
			globals.num_edicts = entnum + 1;
		}
		ent = g_edicts + entnum;
		// Reconstitute pointers
		EvaluateFields(savefields, (byte *)ent, 'EDCT', sizeof(edict_t));

		// let the server rebuild world links for this ent
		gi.linkentity (ent);
	}
}

/*
=================
WriteLevel
=================
*/
void WriteLevel ()
{
	// Used to make sure 
	gi.AppendToSavegame('FTSZ', function_table_sizes, sizeof(function_table_sizes));

	// write out level_locals_t
	WriteLevelLocals ();

	// write out all the entities
	WriteEdicts();

	// Save out all the scripting data
	SaveGlobals();
	SaveLocals();

	// write out the lists of dynamically allocd stuff (ais, senses, etc)
	DynListWrite();

	// Write out all the multiple bolted instance data
	WriteObjInfo();

	// Write out all the pickup data
	thePickupList.Write();
}

/*
=================
ReadLevel

SpawnEntities will already have been called on the
level the same way it was when the level was saved.

That is necessary to get the baselines
set up identically.

The server will have cleared all of the world links before
calling ReadLevel.

No clients are connected yet.
=================
*/
void ReadLevel ()
{
	int				*ftp;
	int				i;
	edict_t			*ent;

	// free any dynamic memory allocated by loading the level
	// base state
	gi.FreeTags (TAG_LEVEL);
	UUIDRemap.clear();
	thePickupList.Destroy();

	// Free the edicts spawned when creating baseline
	G_FreeAllEdicts();

	globals.num_edicts = (int)maxclients->value + 1;

	// Verify there aren't any new functions
	gi.ReadFromSavegame('FTSZ', NULL, sizeof(function_table_sizes), (void **)&ftp);
	for(i = 0; i < sizeof(function_table_sizes) / 4; i++)
	{
		if(ftp[i] != function_table_sizes[i])
		{
			gi.error("Function index tables have changed size");
		}
	}
	gi.TagFree(ftp);

	// load the level locals
	ReadLevelLocals ();

	// load all the entities
	ReadEdicts();

	// Load up scripting data and patch into edict array
	// Needs to be before the ai stuff 
	ShutdownScripts();
	LoadGlobals();
	LoadLocals();

	// load the lists of dynamically allocd stuff (ais, senses, etc)
	DynListRead();

	// Read in objinfo structures
	ReadObjInfo();

	// Read in all the pickup data
	thePickupList.Read();

	// mark all clients as unconnected
	for(i = 0; i < (int)maxclients->value; i++)
	{
		ent = g_edicts + i + 1;
		ent->client = game.clients + i;
		ent->client->pers.connected = false;
	}

	// Remap the loaded UUIDs to the new created ones
	RemapUUIDs();

	// Precache all weapons
	pe->UncacheViewWeaponModels();
	pe->PrecacheViewWeaponModels(level.weaponsAvailable);

#if	_DEBUG
	ValidateLoadedData();
#endif
}

// end
