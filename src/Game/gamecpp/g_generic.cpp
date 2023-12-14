/******************************************************
 * Generic Objects	                                  *
 ******************************************************/

#include "g_local.h"
#include "g_obj.h"
#include "ai_private.h" // hmmm, is this kosher?
#include "callback.h"
#include "..\qcommon\ef_flags.h"

extern void debug_drawbox(edict_t* self,vec3_t vOrigin, vec3_t vMins, vec3_t vMaxs, int nColor);

void drawthink(edict_t *ent)
{
	debug_drawbox(ent, NULL, NULL, NULL, 0xFFFFFFFF);
	ent->nextthink = level.time + .1;
}

//  For barrel
#define	BARREL_ON_FIRE			4
#define BARREL_EXPLOSIVE		4

//  For car_american
#define CAR_AMERICAN_POLICE		0
#define CAR_AMERICAN_TAXI		1
#define CAR_AMERICAN_CADILLAC	2
#define CAR_AMERICAN_MIAMI		3

#define CAR_AMERICAN_ON		4
#define CAR_AMERICAN_SIREN	8

// For security camera
#define SECCAM_ROTATE			4
#define SECCAM_TRIGGERING		8
#define SECCAM_WALL				16

// european car
#define CAR_TRIGGER_SPAWN		4

// forklift
#define FORKLIFT_TRIGGER_SPAWN	4


#define SPEAKER_WALL			4

#define AUTOGUN_ACTIVE			4
#define AUTOGUN_ARC_LIMITED		8
#define AUTOGUN_NO_TRIPOD		16
#define AUTOGUN_NO_HANGY_THINGY	32

#define AUTOGUN_TOGGLEABLE		16
#define AUTOGUN_TRIGGER_SPAWN	32

#define TANK_PROPANE_NODEBRIS	4

#define TRAIN_TRIGGERED			1
#define TRAIN_RESPAWNER			2
#define TRAIN_START_ACTIVE		4
#define TRAIN_DECCELERATE		8
#define TRAIN_DISAPPEAR			16
#define TRAIN_IDLE				32

#define TRAIN_BRAKE_FACTOR		5

#define FAN_BIG_START_OFF		4

#define SF_NOANIMATE			4

#define PLAYER_FLIPPABLE		(1<<2)

// c4 flags
#define C4_START_OFF			(1<<2)

#define SHELF_FALL	(M_PI*.05)
#define TOP_FALL	(M_PI*.06)


// Health of each object
#define HLTH_GEN_ACCESS_CARD		500
#define HLTH_GEN_BANNER				0
#define HLTH_GEN_BARBWIRE_COIL		500
#define HLTH_GEN_BARREL_BURNING		260
#define HLTH_GEN_BARREL_CHEMICAL	260
#define HLTH_GEN_BARREL_WINE		500
#define HLTH_GEN_BENCH				250	
#define HLTH_GEN_BLINDS				200	
#define HLTH_GEN_BOLTON				200	
#define	HLTH_GEN_BOOKCASE			500
#define	HLTH_GEN_BOOKSHELF			500
#define	HLTH_GEN_BROOM				60
#define HLTH_GEN_BUNK				500
#define	HLTH_GEN_CAR_AMERICAN		2000
#define	HLTH_GEN_CAR_EUROPEAN		2000
#define	HLTH_GEN_CAR_HONDA			2000
#define	HLTH_GEN_CAR_ROLLS			2000
#define	HLTH_GEN_CAR_SPORTS			2000
#define	HLTH_GEN_CART				400
#define	HLTH_GEN_CASH_REGISTER		1000
#define	HLTH_GEN_CHAIR_LEGS			400
#define	HLTH_GEN_CHAIR_LEGS2		400
#define	HLTH_GEN_CHAIR_OFFICE		600
#define	HLTH_GEN_CHUNKS_BRICK		50
#define	HLTH_GEN_CHUNKS_GLASS		25
#define	HLTH_GEN_CHUNKS_METAL		100
#define	HLTH_GEN_CHUNKS_ROCK		50
#define	HLTH_GEN_CHUNKS_WALL		10
#define	HLTH_GEN_CHUNKS_WOOD		30
#define HLTH_GEN_COMP_KEYBOARD		100
#define HLTH_GEN_COMP_MONITOR		250
#define HLTH_GEN_COMP_MONITOR2		250
#define HLTH_GEN_CRATE_FISH			500
#define HLTH_GEN_DESK_PHONE			50
#define HLTH_GEN_FAN_BIG			500
#define HLTH_GEN_FAUCET				150
#define HLTH_GEN_FIRE_EXTINGUISHER	150
#define HLTH_GEN_FIRE_HYDRANT		500
#define HLTH_GEN_FISH				150
#define HLTH_GEN_FISH_DEAD			100
#define HLTH_GEN_FLAG_POLE			500
#define HLTH_GEN_FORKLIFT			2000
#define HLTH_GEN_GARBAGE_BAG		100
#define	HLTH_GEN_GUN_AUTO			2000
#define	HLTH_GEN_GUN_BIG			1//4000
#define	HLTH_GEN_HEAP				200
#define HLTH_GEN_LIGHT_BEAM			250	
#define HLTH_GEN_LIGHT_FLARE		500
#define	HLTH_GEN_MOTORCYCLE			2000
#define	HLTH_GEN_ORANGE_CONE		500
#define HLTH_GEN_PALETTE_JACK		1000
#define HLTH_GEN_PAPER_BLOWING		25
#define HLTH_GEN_PARKING_METER		500
#define HLTH_GEN_PHONE_BOOTH		1000
#define HLTH_GEN_PILLOW				100
#define HLTH_GEN_PLANT_POTTED       200
#define HLTH_GEN_PLANT_TALL         200
#define HLTH_GEN_RADAR_BASE			5000
#define HLTH_GEN_RADAR_DISH			5000
#define HLTH_GEN_RADIO				100
#define HLTH_GEN_SECURITY_CAM		200
#define HLTH_GEN_SHOVEL				200
#define HLTH_GEN_SHRUB1				200
#define HLTH_GEN_SINK				200
#define HLTH_GEN_SOFA  				2000
#define HLTH_GEN_SOFA_CHAIR			1000
#define HLTH_GEN_SPEAKER			100
#define HLTH_GEN_STOPLIGHT			2000
#define HLTH_GEN_STREETSIGN			500
#define	HLTH_GEN_SWORD				150
#define	HLTH_GEN_SWORD_LARGE		150
#define HLTH_GEN_TABLE_BACK			50
#define HLTH_GEN_TABLE_LEG			50
#define HLTH_GEN_TABLE_ROUND		400
#define HLTH_GEN_TABLE1				400
#define HLTH_GEN_TABLE2				1000
#define HLTH_GEN_TANK1				4000
#define HLTH_GEN_TANK_GAS			8
#define HLTH_GEN_TANK_PROPANE		8
#define HLTH_GEN_TOILET				200
#define HLTH_GEN_TRAIN				9000
#define HLTH_GEN_TRASH_ASHTRAY		100
#define HLTH_GEN_TRASHCAN_BARREL	200
#define HLTH_GEN_TREE_DEAD			1000
#define HLTH_GEN_TREE_THIN			500
#define HLTH_GEN_TRUCKCAB			3000
#define HLTH_GEN_TV_CEILING			300
#define HLTH_GEN_TV_WALL			300
#define HLTH_GEN_URINAL				200
#define HLTH_GEN_URINAL_CAKE		100
#define HLTH_GEN_URN				25
#define HLTH_GEN_WASTE_BASKET		150
#define HLTH_GEN_WPN_BAT			400
#define HLTH_GEN_WPN_DESERT_EAGLE	1000
#define HLTH_GEN_WPN_GLOCK			1000
#define HLTH_GEN_WPN_INGRAM			1000
#define HLTH_GEN_WPN_KATANA			1000
#define HLTH_GEN_WPN_MINIMI			1000
#define HLTH_GEN_WPN_SIG3000		1000
#define HLTH_GEN_WPN_SPAS12			1000
#define HLTH_GEN_WEB_MOSS_ANIM		150
#define HLTH_GEN_WETFLOOR_SIGN		150

//  misc_generic...
void SP_misc_generic_access_card (edict_t *ent);
void SP_misc_generic_banner_generic (edict_t *ent);
void SP_misc_generic_barbwire_coil (edict_t *ent);
void SP_misc_generic_barrel_burning (edict_t *ent);
void SP_misc_generic_barrel_chemical (edict_t *ent);
void SP_misc_generic_barrel_wine (edict_t *ent);
void SP_misc_generic_bench (edict_t *ent);
void SP_misc_generic_blinds (edict_t *ent);
void SP_misc_generic_bookshelf (edict_t *ent);
void SP_misc_generic_broom (edict_t *ent);
void SP_misc_generic_bunk (edict_t *ent);
void SP_misc_generic_car_american (edict_t *ent);
void SP_misc_generic_car_european (edict_t *ent);
void SP_misc_generic_car_honda (edict_t *ent);
void SP_misc_generic_car_rolls (edict_t *ent);
void SP_misc_generic_car_sports (edict_t *ent);
void SP_misc_generic_cart (edict_t *ent);
void SP_misc_generic_cash_register (edict_t *ent);
void SP_misc_generic_chair_legs (edict_t *ent);
void SP_misc_generic_chair_legs2 (edict_t *ent);
void SP_misc_generic_chair_office (edict_t *ent);
void SP_misc_generic_chunks_brick (edict_t *ent);
void SP_misc_generic_chunks_glass (edict_t *ent);
void SP_misc_generic_chunks_metal (edict_t *ent);
void SP_misc_generic_chunks_rock (edict_t *ent);
void SP_misc_generic_chunks_wall (edict_t *ent);
void SP_misc_generic_chunks_wood (edict_t *ent);
void SP_misc_generic_comp_keyboard (edict_t *ent);
void SP_misc_generic_comp_monitor (edict_t *ent);
void SP_misc_generic_comp_monitor2 (edict_t *ent);
void SP_misc_generic_crate_fish (edict_t *ent);
void SP_misc_generic_desk_phone (edict_t *ent);
void SP_misc_generic_fan_big (edict_t *ent);
void SP_misc_generic_faucet (edict_t *ent);
void SP_misc_generic_fire_extinguisher (edict_t *ent);
void SP_misc_generic_fire_hydrant (edict_t *ent);
void SP_misc_generic_fish (edict_t *ent);
void SP_misc_generic_fish_dead (edict_t *ent);
void SP_misc_generic_flag_pole (edict_t *ent);
void SP_misc_generic_forklift (edict_t *ent);
void SP_misc_generic_garbage_bag (edict_t *ent);
void SP_misc_generic_gun_auto (edict_t *ent);
void SP_misc_generic_gun_auto2 (edict_t *ent);
void SP_misc_generic_gun_auto3 (edict_t *ent);
void SP_misc_generic_gun_big (edict_t *ent);
void SP_misc_generic_gun_castle (edict_t *ent);
void SP_misc_generic_heap (edict_t *ent);
void SP_misc_generic_orange_cone(edict_t *ent);
void SP_misc_generic_light_beam (edict_t *ent);
void SP_misc_generic_light_flare (edict_t *ent);
void SP_misc_generic_motorcycle (edict_t *ent);
void SP_misc_generic_palette_jack (edict_t *ent);
void SP_misc_generic_paper_blowing (edict_t *ent);
void SP_misc_generic_parking_meter (edict_t *ent);
void SP_misc_generic_phone_booth (edict_t *ent);
void SP_misc_generic_pillow (edict_t *ent);
void SP_misc_generic_plant_potted (edict_t *ent);
void SP_misc_generic_plant_tall (edict_t *ent);
void SP_misc_generic_radar_dish (edict_t *ent);
void SP_misc_generic_radio (edict_t *ent);
void SP_misc_generic_security_camera (edict_t *ent);
void SP_misc_generic_security_camera2 (edict_t *ent);
void SP_misc_generic_shovel (edict_t *ent);
void SP_misc_generic_shrub (edict_t *ent);
void SP_misc_generic_sink (edict_t *ent);
void SP_misc_generic_sofa (edict_t *ent);
void SP_misc_generic_sofa_chair (edict_t *ent);
void SP_misc_generic_speaker (edict_t *ent);
void SP_misc_generic_sprinkler (edict_t *ent);
void SP_misc_generic_stoplight (edict_t *ent);
void SP_misc_generic_street_name_sign (edict_t *ent);
void SP_misc_generic_sword (edict_t *ent);
void SP_misc_generic_sword_large (edict_t *ent);
void SP_misc_generic_table_round (edict_t *ent);
void SP_misc_generic_table1 (edict_t *ent);
void SP_misc_generic_table2 (edict_t *ent);
void SP_misc_generic_tank_gas (edict_t *ent);
void SP_misc_generic_tank_propane (edict_t *ent);
void SP_misc_generic_toilet (edict_t *ent);
void SP_misc_generic_train (edict_t *ent);
void SP_misc_generic_trash_ashtray (edict_t *ent);
void SP_misc_generic_trashcan_barrel (edict_t *ent);
void SP_misc_generic_tree_dead (edict_t *ent);
void SP_misc_generic_tree_thin (edict_t *ent);
void SP_misc_generic_truckcab (edict_t *ent);
void SP_misc_generic_tv_ceiling (edict_t *ent);
void SP_misc_generic_tv_wall (edict_t *ent);
void SP_misc_generic_urinal (edict_t *ent);
void SP_misc_generic_urinal_cake (edict_t *ent);
void SP_misc_generic_urn (edict_t *ent);
void SP_misc_generic_waste_basket_wire (edict_t *ent);
void SP_misc_generic_web_moss_animated (edict_t *ent);
void SP_misc_generic_wet_floor_sign (edict_t *ent);
void SP_misc_generic_wpn_bat(edict_t *ent);
void SP_misc_generic_wpn_desert_eagle(edict_t *ent);
void SP_misc_generic_wpn_glock(edict_t *ent);
void SP_misc_generic_wpn_ingram(edict_t *ent);
void SP_misc_generic_wpn_katana(edict_t *ent);
void SP_misc_generic_wpn_minimi(edict_t *ent);
void SP_misc_generic_wpn_sig3000(edict_t *ent);
void SP_misc_generic_wpn_spas12(edict_t *ent);
void SP_misc_generic_wpn_c4(edict_t *ent);

void SP_misc_test_gun_auto (edict_t *ent);


spawn_t genericSpawns[] =
{	
	// misc_generic...	
	{"misc_generic_access_card",		SP_misc_generic_access_card},
	{"misc_generic_banner",				SP_misc_generic_banner_generic},
	{"misc_generic_barbwire_coil",		SP_misc_generic_barbwire_coil},
	{"misc_generic_barrel_burning",		SP_misc_generic_barrel_burning},
	{"misc_generic_barrel_chemical",	SP_misc_generic_barrel_chemical},
	{"misc_generic_barrel_wine",		SP_misc_generic_barrel_wine},
	{"misc_generic_bench",				SP_misc_generic_bench},
	{"misc_generic_blinds",				SP_misc_generic_blinds},
	{"misc_generic_bookshelf",			SP_misc_generic_bookshelf},
	{"misc_generic_broom",				SP_misc_generic_broom},
	{"misc_generic_bunk",				SP_misc_generic_bunk},
	{"misc_generic_car_american",		SP_misc_generic_car_american},
	{"misc_generic_car_european",		SP_misc_generic_car_european},
	{"misc_generic_car_honda",			SP_misc_generic_car_honda},
	{"misc_generic_car_rolls",			SP_misc_generic_car_rolls},
	{"misc_generic_car_sports",			SP_misc_generic_car_sports},
	{"misc_generic_cart",				SP_misc_generic_cart},
	{"misc_generic_cash_register",		SP_misc_generic_cash_register},
	{"misc_generic_chair_legs",			SP_misc_generic_chair_legs},
	{"misc_generic_chair_legs2",		SP_misc_generic_chair_legs2},
	{"misc_generic_chair_office",		SP_misc_generic_chair_office},
	{"misc_generic_chunks_brick",		SP_misc_generic_chunks_brick},
	{"misc_generic_chunks_glass",		SP_misc_generic_chunks_glass},
	{"misc_generic_chunks_metal",		SP_misc_generic_chunks_metal},
	{"misc_generic_chunks_rock",		SP_misc_generic_chunks_rock},
	{"misc_generic_chunks_wall",		SP_misc_generic_chunks_wall},
	{"misc_generic_chunks_wood",		SP_misc_generic_chunks_wood},
	{"misc_generic_comp_keyboard",		SP_misc_generic_comp_keyboard},
	{"misc_generic_comp_monitor",		SP_misc_generic_comp_monitor},
	{"misc_generic_comp_monitor2",		SP_misc_generic_comp_monitor2},
	{"misc_generic_crate_fish",			SP_misc_generic_crate_fish},
	{"misc_generic_desk_phone",			SP_misc_generic_desk_phone},
	{"misc_generic_fan_big",			SP_misc_generic_fan_big},
	{"misc_generic_faucet",				SP_misc_generic_faucet},
	{"misc_generic_fire_extinguisher",	SP_misc_generic_fire_extinguisher},
	{"misc_generic_fire_hydrant",		SP_misc_generic_fire_hydrant},
	{"misc_generic_fish",				SP_misc_generic_fish},
	{"misc_generic_fish_dead",			SP_misc_generic_fish_dead},
	{"misc_generic_flag_pole",			SP_misc_generic_flag_pole},
	{"misc_generic_forklift",			SP_misc_generic_forklift},
	{"misc_generic_garbage_bag",		SP_misc_generic_garbage_bag},
	{"misc_generic_gun_auto",			SP_misc_generic_gun_auto},
	{"misc_generic_gun_auto2",			SP_misc_generic_gun_auto2},
	{"misc_generic_gun_auto3",			SP_misc_generic_gun_auto3},
	{"misc_generic_gun_big",			SP_misc_generic_gun_big},
	{"misc_generic_gun_castle",			SP_misc_generic_gun_castle},
	{"misc_generic_heap",				SP_misc_generic_heap},
	{"misc_generic_orange_cone",		SP_misc_generic_orange_cone},
	{"misc_generic_light_beam",			SP_misc_generic_light_beam},
	{"misc_generic_light_flare",		SP_misc_generic_light_flare},
	{"misc_generic_motorcycle",			SP_misc_generic_motorcycle},
	{"misc_generic_palette_jack",		SP_misc_generic_palette_jack},
	{"misc_generic_paper_blowing",		SP_misc_generic_paper_blowing},
	{"misc_generic_parking_meter",		SP_misc_generic_parking_meter},
	{"misc_generic_phone_booth",		SP_misc_generic_phone_booth},
	{"misc_generic_pillow",				SP_misc_generic_pillow},
	{"misc_generic_plant_potted",		SP_misc_generic_plant_potted},
	{"misc_generic_plant_tall",			SP_misc_generic_plant_tall},
	{"misc_generic_radar_dish",			SP_misc_generic_radar_dish},
	{"misc_generic_radio",				SP_misc_generic_radio},
	{"misc_generic_security_camera",	SP_misc_generic_security_camera},
	{"misc_generic_security_camera2",	SP_misc_generic_security_camera2},
	{"misc_generic_shovel",				SP_misc_generic_shovel},
	{"misc_generic_shrub",				SP_misc_generic_shrub},
	{"misc_generic_sink",				SP_misc_generic_sink},
	{"misc_generic_sofa",				SP_misc_generic_sofa},
	{"misc_generic_sofa_chair",			SP_misc_generic_sofa_chair},
	{"misc_generic_speaker",			SP_misc_generic_speaker},
	{"misc_generic_sprinkler",			SP_misc_generic_sprinkler},
	{"misc_generic_stoplight",			SP_misc_generic_stoplight},
	{"misc_generic_street_name_sign",	SP_misc_generic_street_name_sign},
	{"misc_generic_sword",				SP_misc_generic_sword},
	{"misc_generic_sword_large",		SP_misc_generic_sword_large},
	{"misc_generic_table_round",		SP_misc_generic_table_round},
	{"misc_generic_table1",				SP_misc_generic_table1},
	{"misc_generic_table2",				SP_misc_generic_table2},
	{"misc_generic_tank_gas",			SP_misc_generic_tank_gas},
	{"misc_generic_tank_propane",		SP_misc_generic_tank_propane},
	{"misc_generic_train",				SP_misc_generic_train},
	{"misc_generic_trash_ashtray",		SP_misc_generic_trash_ashtray},
	{"misc_generic_trashcan_barrel",	SP_misc_generic_trashcan_barrel},
	{"misc_generic_tree_dead",			SP_misc_generic_tree_dead},
	{"misc_generic_tree_thin",			SP_misc_generic_tree_thin},
	{"misc_generic_toilet",				SP_misc_generic_toilet},
	{"misc_generic_truckcab",			SP_misc_generic_truckcab},
	{"misc_generic_tv_ceiling",			SP_misc_generic_tv_ceiling},
	{"misc_generic_tv_wall",			SP_misc_generic_tv_wall},
	{"misc_generic_urinal",				SP_misc_generic_urinal},
	{"misc_generic_urinal_cake",		SP_misc_generic_urinal_cake},
	{"misc_generic_urn",				SP_misc_generic_urn},
	{"misc_generic_waste_basket_wire",	SP_misc_generic_waste_basket_wire},
	{"misc_generic_wpn_bat",			SP_misc_generic_wpn_bat},
	{"misc_generic_wpn_desert_eagle",	SP_misc_generic_wpn_desert_eagle},
	{"misc_generic_wpn_glock",			SP_misc_generic_wpn_glock},
	{"misc_generic_wpn_ingram",			SP_misc_generic_wpn_ingram},
	{"misc_generic_wpn_katana",			SP_misc_generic_wpn_katana},
	{"misc_generic_wpn_minimi",			SP_misc_generic_wpn_minimi},
	{"misc_generic_wpn_sig3000",		SP_misc_generic_wpn_sig3000},
	{"misc_generic_wpn_spas12",			SP_misc_generic_wpn_spas12},
	{"misc_generic_wpn_c4",				SP_misc_generic_wpn_c4},
	{"misc_generic_web_moss_animated",	SP_misc_generic_web_moss_animated},
	{"misc_generic_wet_floor_sign",		SP_misc_generic_wet_floor_sign},

	{"misc_test_gun_auto",				SP_misc_test_gun_auto},
	
	{NULL,								NULL},
};

typedef enum
{
	OBJ_ACCESS_CARD = 0,
	OBJ_BANNER,
	OBJ_BARBWIRE_COIL,
	OBJ_BARREL_BURNING,
	OBJ_BARREL_CHEMICAL,
	OBJ_BARREL_WINE,
	OBJ_BENCH_BOTTOM,
	OBJ_BENCH_TOP,
	OBJ_BLINDS,
	OBJ_BOOKCASE,
	OBJ_BOOKSHELF,
	OBJ_BOOKSHELF_BROKE,
	OBJ_BROOM,
	OBJ_BUNK,
	OBJ_BUNKBED,
	OBJ_BUNKSIDE2,
	OBJ_CAR_AMERICAN,
	OBJ_CAR_EUROPEAN,
	OBJ_CAR_HONDA,
	OBJ_CAR_ROLLS,
	OBJ_CAR_SPORTS,
	OBJ_CART,
	OBJ_CASH_REGISTER,
	OBJ_CHAIR_LEGS,
	OBJ_CHAIR_LEGS_LEGS,
	OBJ_CHAIR_LEGS2,
	OBJ_CHAIR_OFFICE_SEAT,
	OBJ_CHAIR_OFFICE_BASE,
	OBJ_CHUNKS_BRICK,
	OBJ_CHUNKS_GLASS,
	OBJ_CHUNKS_METAL,
	OBJ_CHUNKS_ROCK,
	OBJ_CHUNKS_WALL,
	OBJ_CHUNKS_WOOD,
	OBJ_COMP_KEYBOARD,
	OBJ_COMP_MONITOR,
	OBJ_COMP_MONITOR2,
	OBJ_COMP_MONITOR2_TOWER,
	OBJ_CRATE_FISH,
	OBJ_DESK_PHONE,
	OBJ_FAN_BIG,
	OBJ_FAUCET,
	OBJ_FIRE_EXTINGUISHER,
	OBJ_FIRE_HYDRANT,
	OBJ_FISH,
	OBJ_FISH_DEAD,
	OBJ_FLAG_POLE,
	OBJ_FORKLIFT,
	OBJ_GARBAGE_BAG,
	OBJ_GUN_AUTO,
	OBJ_GUN_AUTO_BASE,
	OBJ_GUN_AUTO_ARM,
	OBJ_NULL,
	OBJ_GUN_BIG_BASE,
	OBJ_GUN_BIG_GUN_LEFT,
	OBJ_GUN_BIG_GUN_RIGHT,
	OBJ_GUN_BIG_SHIELD,
	OBJ_HEAP,
	OBJ_LIGHT_BEAM,
	OBJ_LIGHT_FLARE,
	OBJ_MOTORCYCLE,
	OBJ_ORANGE_CONE,
	OBJ_PALETTE_JACK,
	OBJ_PAPER_BLOWING,
	OBJ_PARKING_METER,
	OBJ_PHONE_BOOTH,
	OBJ_PILLOW,
	OBJ_PLANT_POTTED,
	OBJ_PLANT_TALL,
	OBJ_RADAR_BASE,
	OBJ_RADAR_DISH,
	OBJ_RADIO,
	OBJ_SECURITY_CAM_CAMERA,
	OBJ_SECURITY_CAM_POLE,
	OBJ_SHOVEL,
	OBJ_SHRUB1,
	OBJ_SINK,
	OBJ_SOFA,
	OBJ_SOFA_CHAIR,
	OBJ_SPEAKER,
	OBJ_STOPLIGHT,
	OBJ_STREETSIGN,
	OBJ_SWORD,
	OBJ_SWORD_LARGE,
	OBJ_TABLE_ROUND,
	OBJ_TABLE1,
	OBJ_TABLE1_BACK,
	OBJ_TABLE1_LEG,
	OBJ_TABLE2,
	OBJ_TABLE2_SIDE1,
	OBJ_TABLE2_SIDE2,
	OBJ_TABLE2_SIDE3,
	OBJ_TABLE2_TOP2,
	OBJ_TANK1,
	OBJ_TANK_GAS,
	OBJ_TANK_PROPANE,
	OBJ_TOILET,
	OBJ_TRAIN,
	OBJ_TRASH_ASHTRAY,
	OBJ_TRASHCAN_BARREL,
	OBJ_TREE_DEAD,
	OBJ_TREE_THIN,
	OBJ_TRUCKCAB,
	OBJ_TV_CEILING,
	OBJ_TV_CEILING_SUPPORT,
	OBJ_TV_WALL,
	OBJ_URINAL,
	OBJ_URINAL_CAKE,
	OBJ_URN,
	OBJ_WASTE_BASKET,
	OBJ_WEB_MOSS_ANIM,
	OBJ_WETFLOOR_SIGN,
	OBJ_WPN_BAT,
	OBJ_WPN_DESERT_EAGLE,
	OBJ_WPN_GLOCK,
	OBJ_WPN_INGRAM,
	OBJ_WPN_KATANA,
	OBJ_WPN_MINIMI,
	OBJ_WPN_SIG3000,
	OBJ_WPN_SPAS12,
	OBJ_WPN_C4,
	MAX_OBJS
};



modelSpawnData_t genericModelData[MD_GENERIC_SIZE] =
{	
//      dir							file				surfaceType			material			health						solid			material file      cnt  scale
"objects/generic/access_card",		"card",				SURF_DEFAULT,		MAT_DEFAULT,		HLTH_GEN_ACCESS_CARD,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_ACCESS_CARD
"objects/generic/banner_generic",	"banner",			SURF_DEFAULT,		MAT_DEFAULT,		HLTH_GEN_BANNER,			SOLID_BBOX,		"banner",			0,	DEBRIS_SM,	NULL,	// OBJ_BANNER
"objects/generic/barbwire_coil",	"barbwire",			SURF_METAL,			MAT_DEFAULT,		HLTH_GEN_BARBWIRE_COIL,		SOLID_BBOX,		NULL,				15,	DEBRIS_SM,	NULL,	// OBJ_BARBWIRE_COIL
"objects/generic/barrel_burning",	"barrel",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_BARREL_BURNING,	SOLID_BBOX,		NULL,				0,	DEBRIS_MED,	NULL,	// OBJ_BARREL_BURNING
"objects/generic/barrel_chemical",	"barrel",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_BARREL_CHEMICAL,	SOLID_BBOX,		"barrel",			0,	DEBRIS_MED,	NULL,	// OBJ_BARREL_CHEMICAL
"objects/generic/barrel_wine",		"barrel",			SURF_WOOD_LBROWN,	MAT_WOOD_LBROWN,	HLTH_GEN_BARREL_WINE,		SOLID_BBOX,		NULL,				0,	DEBRIS_MED,	NULL,	// OBJ_BARREL_WINE
"objects/generic/bench",			"bottom",			SURF_WOOD_LBROWN,	MAT_WOOD_LBROWN,	HLTH_GEN_BENCH,				SOLID_BBOX,		"bench",			10,	DEBRIS_MED,	NULL,	// OBJ_BENCH_BOTTOM
"objects/generic/bench",			"top",				SURF_WOOD_LBROWN,	MAT_WOOD_LBROWN,	HLTH_GEN_BENCH,				SOLID_BBOX,		"bench",			10,	DEBRIS_MED,	NULL,	// OBJ_BENCH_TOP
"objects/generic/blinds",			"blinds",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_BLINDS,			SOLID_BBOX,		"blinds",			0,	DEBRIS_MED,	NULL,	// OBJ_BLINDS
"objects/generic/bookcase",			"case",				SURF_WOOD_LBROWN,	MAT_WOOD_LBROWN,	HLTH_GEN_BOOKCASE,			SOLID_BBOX,		"case",				0,	DEBRIS_LRG,	NULL,	// OBJ_BOOKCASE
"objects/generic/bookshelf",		"shelf",			SURF_WOOD_LBROWN,	MAT_WOOD_LBROWN,	HLTH_GEN_BOOKSHELF,			SOLID_BBOX,		"shelf",			0,	DEBRIS_SM,	NULL,	// OBJ_BOOKSHELF
"objects/generic/bookshelf",		"middle",			SURF_WOOD_LBROWN,	MAT_WOOD_LBROWN,	HLTH_GEN_BOOKSHELF,			SOLID_BBOX,		"shelf",			0,	DEBRIS_SM,	NULL,	// OBJ_BOOKSHELF_BROKE
"objects/generic/broom",			"broom",			SURF_WOOD_LBROWN,	MAT_WOOD_LBROWN,	HLTH_GEN_BROOM,				SOLID_BBOX,		"broom",			0,	DEBRIS_SM,	NULL,	// OBJ_BROOM
"objects/generic/bunk",				"side2",			SURF_METAL,			MAT_METAL_LGREY,	HLTH_GEN_BUNK,				SOLID_BBOX,		"bunk",				0,	DEBRIS_MED,	NULL,	// OBJ_BUNK
"objects/generic/bunk",				"bed",				SURF_METAL,			MAT_METAL_LGREY,	HLTH_GEN_BOLTON,			SOLID_NOT,		"bunk",				0,	DEBRIS_MED,	NULL,	// OBJ_BUNKBED
"objects/generic/bunk",				"side1",			SURF_METAL,			MAT_METAL_LGREY,	HLTH_GEN_BOLTON,			SOLID_NOT,		"bunk",				0,	DEBRIS_MED,	NULL,	// OBJ_BUNKSIDE2
"objects/generic/car_american",		"car",				SURF_METAL,			MAT_METAL_LGREY,	HLTH_GEN_CAR_AMERICAN,		SOLID_BBOX,		"car",				0,	DEBRIS_SM,	NULL,	// OBJ_CAR_AMERICAN
"objects/generic/car_european",		"car",				SURF_METAL,			MAT_METAL_LGREY,	HLTH_GEN_CAR_EUROPEAN,		SOLID_BBOX,		"car",				0,	DEBRIS_SM,	NULL,	// OBJ_CAR_EUROPEAN
"objects/generic/car_honda",		"car",				SURF_METAL,			MAT_METAL_LGREY,	HLTH_GEN_CAR_HONDA,			SOLID_BBOX,		"car",				0,	DEBRIS_SM,	NULL,	// OBJ_CAR_HONDA
"objects/generic/car_rolls",		"car",				SURF_METAL,			MAT_METAL_LGREY,	HLTH_GEN_CAR_ROLLS,			SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_CAR_ROLLS
"objects/generic/car_sports",		"car",				SURF_METAL,			MAT_METAL_LGREY,	HLTH_GEN_CAR_SPORTS,		SOLID_BBOX,		"car",				0,	DEBRIS_SM,	NULL,	// OBJ_CAR_SPORTS
"objects/generic/cart",				"cart",				SURF_METAL,			MAT_METAL_LGREY,	HLTH_GEN_CASH_REGISTER,		SOLID_BBOX,		"cart",				0,	DEBRIS_SM,	NULL,	// OBJ_CART
"objects/generic/cash_register",	"register",			SURF_METAL,			MAT_METAL_LGREY,	HLTH_GEN_BUNK,				SOLID_BBOX,		"register",			0,	DEBRIS_SM,	NULL,	// OBJ_CASH_REGISTER
"objects/generic/chair_legs",		"chair",			SURF_METAL,			MAT_METAL_LGREY,	HLTH_GEN_CHAIR_LEGS,		SOLID_BBOX,		"legs",				0,	DEBRIS_SM,	NULL,	// OBJ_CHAIR_LEGS
"objects/generic/chair_legs",		"legs",				SURF_METAL,			MAT_METAL_LGREY,	HLTH_GEN_CHAIR_LEGS,		SOLID_BBOX,		"legs",				0,	DEBRIS_SM,	NULL,	// OBJ_CHAIR_LEGS_LEGS
"objects/generic/chair_legs2",		"legs2",			SURF_METAL,			MAT_METAL_LGREY,	HLTH_GEN_CHAIR_LEGS2,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_CHAIR_LEGS2
"objects/generic/chair_office",		"top",				SURF_METAL,			MAT_METAL_SHINY,	HLTH_GEN_CHAIR_OFFICE,		SOLID_BBOX,		"office",			0,	DEBRIS_SM,	NULL,	// OBJ_CHAIR_OFFICE_SEAT
"objects/generic/chair_office",		"base",				SURF_METAL,			MAT_METAL_SHINY,	HLTH_GEN_CHAIR_OFFICE,		SOLID_BBOX,		"office",			0,	DEBRIS_SM,	NULL,	// OBJ_CHAIR_OFFICE_BASE
"objects/generic/chunks_brick",		"brick",			SURF_STONE_DBROWN,	MAT_BRICK_DBROWN,	HLTH_GEN_CHUNKS_BRICK,		SOLID_BBOX,		"brick",			0,	DEBRIS_SM,	NULL,	// OBJ_CHUNKS_BRICK
"objects/generic/chunks_glass",		"glass",			SURF_GLASS,			MAT_GLASS,			HLTH_GEN_CHUNKS_GLASS,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_CHUNKS_GLASS
"objects/generic/chunks_metal",		"metal",			SURF_METAL,			MAT_METAL_LGREY,	HLTH_GEN_CHUNKS_METAL,		SOLID_BBOX,		"metal",			0,	DEBRIS_SM,	NULL,	// OBJ_CHUNKS_METAL
"objects/generic/chunks_rock",		"rock",				SURF_STONE_DBROWN,	MAT_ROCK_DBROWN,	HLTH_GEN_CHUNKS_ROCK,		SOLID_BBOX,		"rock",				0,	DEBRIS_SM,	NULL,	// OBJ_CHUNKS_ROCK
"objects/generic/chunks_wall",		"wall",				SURF_STONE_DBROWN,	MAT_ROCK_DBROWN,	HLTH_GEN_CHUNKS_WALL,		SOLID_BBOX,		"wall",				0,	DEBRIS_SM,	NULL,	// OBJ_CHUNKS_WALL
"objects/generic/chunks_wood",		"wood",				SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_GEN_CHUNKS_WOOD,		SOLID_BBOX,		"wood",				0,	DEBRIS_SM,	NULL,	// OBJ_CHUNKS_WOOD
"objects/generic/comp_keyboard",	"keyboard",			SURF_METAL,			MAT_WALL_GREY,		HLTH_GEN_COMP_KEYBOARD,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_COMP_KEYBOARD
"objects/generic/comp_monitor",		"monitor",			SURF_GLASS_COMPUTER,MAT_WALL_GREY,		HLTH_GEN_COMP_MONITOR,		SOLID_BBOX,		"monitor",			0,	DEBRIS_SM,	NULL,	// OBJ_COMP_MONITOR
"objects/generic/comp_monitor2",	"monitor2",			SURF_GLASS_COMPUTER,MAT_WALL_GREY,		HLTH_GEN_COMP_MONITOR2,		SOLID_BBOX,		"monitor2",			0,	DEBRIS_SM,	NULL,	// OBJ_COMP_MONITOR2
"objects/generic/comp_monitor2",	"tower",			SURF_GLASS_COMPUTER,MAT_WALL_GREY,		HLTH_GEN_COMP_MONITOR2,		SOLID_BBOX,		"monitor2",			0,	DEBRIS_SM,	NULL,	// OBJ_COMP_MONITOR2_TOWER
"objects/generic/crate_fish",		"crate",			SURF_WOOD_LBROWN,	MAT_WOOD_LBROWN,	HLTH_GEN_CRATE_FISH,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_CRATE_FISH
"objects/generic/desk_phone",		"phone",			SURF_METAL,			MAT_WALL_GREY,		HLTH_GEN_DESK_PHONE,		SOLID_BBOX,		"phone",			0,	DEBRIS_SM,	NULL,	// OBJ_DESK_PHONE
"objects/generic/fan",				"fan",				SURF_METAL,			MAT_METAL_SHINY,	HLTH_GEN_FAN_BIG,			SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_FAN_BIG
"objects/generic/faucet",			"faucet",			SURF_METAL,			MAT_METAL_SHINY,	HLTH_GEN_FAUCET,			SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_FAUCET
"objects/generic/fire_extinguisher","exting",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_FIRE_EXTINGUISHER,	SOLID_BBOX,		"exting",			0,	DEBRIS_SM,	NULL,	// OBJ_FIRE_EXTINGUISHER
"objects/generic/fire_hydrant",		"hydrant",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_FIRE_HYDRANT,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_FIRE_HYDRANT
"objects/generic/fish",				"fish",				SURF_BLOOD,			MAT_ROCK_FLESH,		HLTH_GEN_FISH,				SOLID_BBOX,		"fish",				0,	DEBRIS_SM,	NULL,	// OBJ_FISH
"objects/generic/fish",				"fish",				SURF_BLOOD,			MAT_ROCK_FLESH,		HLTH_GEN_FISH,				SOLID_BBOX,		"fish",				0,	DEBRIS_SM,	NULL,	// OBJ_FISH_DEAD
"objects/generic/flag_pole",		"flag_pole",		SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_FLAG_POLE,			SOLID_BBOX,		"flag_pole",		0,	DEBRIS_SM,	NULL,	// OBJ_FLAG_POLE
"objects/generic/forklift",			"forklift",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_FORKLIFT,			SOLID_BBOX,		"forklift",			0,	DEBRIS_SM,	NULL,	// OBJ_FORKLIFT
"objects/generic/garbage_bag",		"bag",				SURF_DEFAULT,		MAT_WALL_GREY,		HLTH_GEN_GARBAGE_BAG,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_GARBAGE_BAG
"objects/generic/gun_auto",			"gun",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_GUN_AUTO,			SOLID_BBOX,		"gun",				0,	DEBRIS_SM,	NULL,	// OBJ_GUN_AUTO
"objects/generic/gun_auto",			"base",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_GUN_AUTO,			SOLID_BBOX,		"gun",				0,	DEBRIS_SM,	NULL,	// OBJ_GUN_AUTO_BASE
"objects/generic/gun_auto",			"arm",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_GUN_AUTO,			SOLID_BBOX,		"gun",				0,	DEBRIS_SM,	NULL,	// OBJ_GUN_AUTO_ARM
"objects/generic/gun_auto",			"null",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_GUN_AUTO,			SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_NULL
"objects/generic/gun_big",			"base",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_GUN_BIG,			SOLID_BBOX,		"gun",				0,	DEBRIS_MED,	NULL,	// OBJ_GUN_BIG_BASE
"objects/generic/gun_big",			"gunleft",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_GUN_BIG,			SOLID_BBOX,		"gun",				0,	DEBRIS_MED,	NULL,	// OBJ_GUN_BIG_GUN_LEFT
"objects/generic/gun_big",			"gunright",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_GUN_BIG,			SOLID_BBOX,		"gun",				0,	DEBRIS_MED,	NULL,	// OBJ_GUN_BIG_GUN_RIGHT
"objects/generic/gun_big",			"shield",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_GUN_BIG,			SOLID_BBOX,		"gun",				0,	DEBRIS_MED,	NULL,	// OBJ_GUN_BIG_SHIELD
"objects/generic/heap_generic",		"heap",				SURF_DEFAULT,		MAT_DEFAULT,		HLTH_GEN_HEAP,				SOLID_BBOX,		"heap",				0,	DEBRIS_SM,	NULL,	// OBJ_HEAP
"objects/generic/beam",				"beam",				SURF_NONE,			MAT_NONE,			HLTH_GEN_LIGHT_BEAM,		SOLID_NOT,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_LIGHT_BEAM
"objects/light/flare",				"flare",			SURF_NONE,			MAT_NONE,			HLTH_GEN_LIGHT_FLARE,		SOLID_NOT,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_LIGHT_FLARE
"objects/generic/motorcycle",		"motorcycle",		SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_MOTORCYCLE,		SOLID_BBOX,		"motorcycle",		0,	DEBRIS_SM,	NULL,	// OBJ_MOTORCYCLE
"objects/generic/orange_cone",		"orangecone",		SURF_STONE_WHITE,	MAT_WALL_WHITE,		HLTH_GEN_ORANGE_CONE,		SOLID_BBOX,		"orangecone",		5,	DEBRIS_MED,	NULL,	// OBJ_ORANGE_CONE
"objects/generic/palette_jack",		"pallette_jack",	SURF_METAL,			MAT_METAL_SHINY,	HLTH_GEN_PALETTE_JACK,		SOLID_BBOX,		NULL,				15,	DEBRIS_SM,	NULL,	// OBJ_PALETTE_JACK
"objects/generic/paper_blowing",	"paper",			SURF_DEFAULT,		MAT_DEFAULT,		HLTH_GEN_PAPER_BLOWING,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_PAPER_BLOWING
"objects/generic/parking_meter",	"parking_meter",	SURF_METAL,			MAT_METAL_SHINY,	HLTH_GEN_PARKING_METER,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_PARKING_METER
"objects/generic/phone_booth",		"phone_booth",		SURF_METAL,			MAT_METAL_SHINY,	HLTH_GEN_PHONE_BOOTH,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_PHONE_BOOTH
"objects/generic/pillow",			"pillow",			SURF_STONE_WHITE,	MAT_WALL_WHITE,		HLTH_GEN_PILLOW,			SOLID_BBOX,		"pillow",			4,	DEBRIS_SM,	NULL,	// OBJ_PILLOW
"objects/generic/plant_potted",		"plant",			SURF_DEFAULT,		MAT_METAL_SHINY,	HLTH_GEN_PLANT_POTTED,		SOLID_BBOX,		"plant",			0,	DEBRIS_SM,	NULL,	// OBJ_PLANT_POTTED
"objects/generic/plant_tall",		"plant",			SURF_DEFAULT,		MAT_DEFAULT,		HLTH_GEN_PLANT_TALL,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_PLANT_TALL
"objects/generic/radar",			"radar_base",		SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_RADAR_BASE,		SOLID_BBOX,		"radar_base",		0,	DEBRIS_SM,	NULL,	// OBJ_RADAR_BASE
"objects/generic/radar",			"radar_dish",		SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_RADAR_DISH,		SOLID_BBOX,		"radar_dish",		0,	DEBRIS_SM,	NULL,	// OBJ_RADAR_DISH
"objects/generic/radio",			"radio",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_RADIO,				SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_RADIO
"objects/generic/security_camera/test",	"camera",		SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_SECURITY_CAM,		SOLID_BBOX,		"camera",			10,	DEBRIS_SM,	NULL,	// OBJ_SECURITY_CAM_CAMERA
"objects/generic/security_camera/test",	"arm",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_SECURITY_CAM,		SOLID_BBOX,		"camera",			10,	DEBRIS_SM,	NULL,	// OBJ_SECURITY_CAM_POLE
"objects/generic/shovel",			"shovel",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_SHOVEL,			SOLID_BBOX,		"shovel",			0,	DEBRIS_SM,	NULL,	// OBJ_SHOVEL
"objects/generic/shrub1",			"shrub",			SURF_DEFAULT,		MAT_DEFAULT,		HLTH_GEN_SHRUB1,			SOLID_BBOX,		"shrub",			0,	DEBRIS_SM,	NULL,	// OBJ_SHRUB1
"objects/generic/sink",				"sink",				SURF_STONE_LGREY,	MAT_ROCK_LGREY,		HLTH_GEN_SINK,				SOLID_BBOX,		"sink",				0,	DEBRIS_SM,	NULL,	// OBJ_SINK
"objects/generic/sofa",				"sofa",				SURF_DEFAULT,		MAT_DEFAULT,		HLTH_GEN_SOFA,				SOLID_BBOX,		"sofa",				0,	DEBRIS_SM,	NULL,	// OBJ_SOFA
"objects/generic/sofa_chair",		"chair",			SURF_DEFAULT,		MAT_DEFAULT,		HLTH_GEN_SOFA_CHAIR,		SOLID_BBOX,		"chair",			0,	DEBRIS_SM,	NULL,	// OBJ_SOFA_CHAIR
"objects/generic/speaker",			"speaker",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_SPEAKER,			SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_SPEAKER
"objects/generic/stoplight",		"stoplight",		SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_STOPLIGHT,			SOLID_BBOX,		"stoplight",		0,	DEBRIS_SM,	NULL,	// OBJ_STOPLIGHT
"objects/generic/street_name_sign",	"sign",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_STREETSIGN,		SOLID_BBOX,		"sign",				0,	DEBRIS_SM,	NULL,	// OBJ_STREETSIGN
"objects/generic/sword",			"sword",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_SWORD,				SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_SWORD
"objects/generic/sword",			"sword",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_SWORD,				SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_SWORD_LARGE
"objects/generic/table_round",		"table",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TABLE_ROUND,		SOLID_BBOX,		"table",			0,	DEBRIS_SM,	NULL,	// OBJ_TABLE_ROUND
"objects/generic/table1",			"top",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TABLE1,			SOLID_BBOX,		"table1",			0,	DEBRIS_SM,	NULL,	// OBJ_TABLE1
"objects/generic/table1",			"back",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TABLE_ROUND,		SOLID_BBOX,		"table1",			0,	DEBRIS_SM,	NULL,	// OBJ_TABLE1_BACK
"objects/generic/table1",			"leg",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TABLE_ROUND,		SOLID_BBOX,		"table1",			0,	DEBRIS_SM,	NULL,	// OBJ_TABLE1_LEG
"objects/generic/table2",			"top01",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TABLE2,			SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_TABLE2
"objects/generic/table2",			"side01",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TABLE2,			SOLID_NOT,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_TABLE2_SIDE1
"objects/generic/table2",			"side02",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TABLE2,			SOLID_NOT,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_TABLE2_SIDE2
"objects/generic/table2",			"side03",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TABLE2,			SOLID_NOT,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_TABLE2_SIDE3
"objects/generic/table2",			"top02",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TABLE2,			SOLID_NOT,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_TABLE2_TOP2
"objects/generic/tank1",			"tank",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TANK1,				SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_TANK1
"objects/generic/tank_gas",			"gas",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TANK_GAS,			SOLID_BBOX,		NULL,				0,	DEBRIS_LRG,	NULL,	// OBJ_TANK_GAS
"objects/generic/tank_propane",		"propane",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TANK_PROPANE,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_TANK_PROPANE
"objects/generic/toilet",			"toilet",			SURF_STONE_WHITE,	MAT_WALL_WHITE,		HLTH_GEN_TOILET,			SOLID_BBOX,		"toilet",			0,	DEBRIS_MED,	NULL,	// OBJ_TOILET
"objects/generic/train_generic",	"train",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TRAIN,				SOLID_BBOX,		"train",			0,	DEBRIS_SM,	NULL,	// OBJ_TRAIN
"objects/generic/trash_ashtray",	"ashtray",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TRASH_ASHTRAY,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_TRASH_ASHTRAY
"objects/generic/trashcan_barrel",	"trashcan",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TRASHCAN_BARREL,	SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_TRASHCAN_BARREL
"objects/generic/tree_dead",		"tree",				SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_GEN_TREE_DEAD,			SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_TREE_DEAD
"objects/generic/tree_thin",		"tree",				SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_GEN_TREE_THIN,			SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_TREE_THIN
"objects/generic/truckcab",			"truckcab",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TRUCKCAB,			SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_TRUCKCAB
"objects/generic/tv_ceiling",		"tv",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TV_CEILING,		SOLID_BBOX,		"face",				10,	DEBRIS_SM,	NULL,	// OBJ_TV_CEILING
"objects/generic/tv_ceiling",		"supports",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TV_CEILING,		SOLID_BBOX,		NULL,				10,	DEBRIS_SM,	NULL,	// OBJ_TV_CEILING_SUPPORT
"objects/generic/tv_wall",			"tv_wall",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_TV_WALL,			SOLID_BBOX,		"tv_wall",			10,	DEBRIS_SM,	NULL,	// OBJ_TV_WALL
"objects/generic/urinal",			"urinal",			SURF_STONE_WHITE,	MAT_WALL_WHITE,		HLTH_GEN_URINAL,			SOLID_BBOX,		"urinal",			0,	DEBRIS_SM,	NULL,	// OBJ_URINAL
"objects/generic/urinal_cake",		"urinal_cake",		SURF_STONE_WHITE,	MAT_ROCK_LGREY,		HLTH_GEN_URINAL_CAKE,		SOLID_BBOX,		NULL,				0,	DEBRIS_TINY,NULL,	// OBJ_URINAL_CAKE
"objects/generic/urn",				"urn",				SURF_GLASS,			MAT_WALL_DARKBROWN,	HLTH_GEN_URN,				SOLID_BBOX,		NULL,				20,	DEBRIS_SM,	NULL,	// OBJ_URN
"objects/generic/waste_basket",		"waste_basket",		SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_WASTE_BASKET,		SOLID_BBOX,		"waste_basket",		0,	DEBRIS_SM,	NULL,	// OBJ_WASTE_BASKET
"objects/generic/web_moss_animated","web_moss",			SURF_DEFAULT,		MAT_DEFAULT,		HLTH_GEN_WEB_MOSS_ANIM,		SOLID_NOT,		"web_moss",			0,	DEBRIS_SM,	NULL,	// OBJ_WEB_MOSS_ANIM
"objects/generic/wet_floor_sign",	"sign",				SURF_DEFAULT,		MAT_DEFAULT,		HLTH_GEN_WETFLOOR_SIGN,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_WETFLOOR_SIGN
"Enemy/Bolt",						"w_bat",			SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_GEN_WPN_BAT,			SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_WPN_BAT
"Enemy/Bolt",						"w_desert_eagle",	SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_WPN_DESERT_EAGLE,	SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_WPN_DESERT_EAGLE
"Enemy/Bolt",						"w_pistol1",		SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_WPN_GLOCK,			SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_WPN_GLOCK
"Enemy/Bolt",						"w_machinepistol",	SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_WPN_INGRAM,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_WPN_INGRAM
"Enemy/Bolt",						"w_katana",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_WPN_INGRAM,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_WPN_KATANA
"Enemy/Bolt",						"w_machinegun",		SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_WPN_MINIMI,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_WPN_MINIMI
"Enemy/Bolt",						"w_sniperrifle",	SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_WPN_SIG3000,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_WPN_SIG3000
"Enemy/Bolt",						"w_shotgun",		SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_WPN_SPAS12,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_WPN_SPAS12
"Items/Projectiles",				"c4",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_GEN_WPN_SPAS12,		SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// OBJ_WPN_C4

};

// multipliers to keep bigger objects from being flipped as far as smaller objects by Obj_painflip
#define FLIPMULT_TRASH_ASHTRAY			0.03f
#define FLIPMULT_COMP_KEYBOARD			0.8f
#define FLIPMULT_DESK_PHONE				0.9f
#define FLIPMULT_WASTE_BASKET			0.2f

// multipliers to keep heavier objects from rolling as far as smaller objects via Obj_painroll
#define ROLLMULT_TRASH_ASHTRAY			0.3f
#define ROLLMULT_WASTE_BASKET			0.6f

short TestforFloor (vec3_t origin, vec3_t vel, vec3_t accel, edict_t* ignore);

void tv_damaged_think (edict_t *ent);
void tv_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void tv_die (edict_t *ent, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);


//-------------------------------------------------------------
// For objects that break apart
//-------------------------------------------------------------
void benchtop_pain (edict_t *self,int partLost, vec3_t boltPos);
void bunk_partpain (edict_t *self,int partLost, vec3_t boltPos);
void table1_legpain (edict_t *self,int partLost, vec3_t boltPos);
void table2_legpain (edict_t *self,int partLost, vec3_t boltPos);
void chair_legs_partpain (edict_t *self,int partLost, vec3_t boltPos);
void bookcase_partpain (edict_t *self,int partLost, vec3_t boltPos);

// Breaking Parts Data enum
typedef enum
{
	BPD_TABLE1 = 0,
	BPD_TABLE2,
	BPD_BENCH,
	BPD_BUNK,
	BPD_CHAIR_LEGS,
	BPD_BOOKCASE,
	BPD_MAX
};

// Breaking Parts Data for each object
objBreak_t genericObjBreak [BPD_MAX] = 
{ 
// # of parts   objParts ptr
	5,			NULL,			// BPD_TABLE1
	4,			NULL,			// BPD_TABLE2
	1,			NULL,			// BPD_BENCH
	4,			NULL,			// BPD_BUNK
	4,			NULL,			// BPD_CHAIR_LEGS
	3,			NULL,			// BPD_BOOKCASE
};


// Breaking Parts Data for each part of each object 
// WARNING: when updating this be sure to update genericObjBreak if more parts are added
objParts_t genericObjBoltPartsData[21] =
{	
	// BPD_TABLE1
	"DUMMY01", 1,	table1_legpain,	// Leg1
	"DUMMY02", 2,	table1_legpain,	// Leg2
	"DUMMY03", 3,	table1_legpain,	// Leg3
	"DUMMY04", 4,	table1_legpain,	// Leg4
	"DUMMY05", 5,	table1_legpain,	// Front

	// BPD_TABLE2
	"DUMMY08", 1,	table2_legpain,	// Short side panel on short length of table
	"DUMMY09", 2,	table2_legpain,	// Long side panel
	"DUMMY10", 3,	table2_legpain,	// Medium side panel
	"DUMMY07", 4,	table2_legpain,	// Short side panel on long length of table
//	"DUMMY12", 5,	table2_legpain,	//  This is hidden - we don't want it shot off


	// BPD_BENCH
	"DUMMY02", 1,	NULL,	// top of the bench

	// BPD_BUNK
	"DUMMY05", 1,	bunk_partpain,	// Side 
	"DUMMY01", 2,	bunk_partpain,	// Top Bunk
	"DUMMY03", 3,	bunk_partpain,	// Bottom Bunk
	"DUMMY03", 4,	bunk_partpain,	// Other side

	// BPD_CHAIR_LEGS
	"DUMMY01", 1,	chair_legs_partpain,	// 
	"DUMMY02", 2,	chair_legs_partpain,	// 
	"DUMMY03", 3,	chair_legs_partpain,	// 
	"DUMMY04", 4,	chair_legs_partpain,	// 

	// BPD_BOOKCASE
	"DUMMY01", 1,	bookcase_partpain,		// shelf
	"DUMMY02", 2,	bookcase_partpain,		// shelf
	"DUMMY03", 3,	bookcase_partpain,		// shelf
};


/*QUAKED misc_generic_access_card (1 .5 0) (-2.95 -3.71 -1.18) (5.99 3.71 1.18)  INVULNERABLE  NOPUSH x x x x FLUFF
An access card
------ SPAWNFLAGS ------
INVULNERABLE - N/A, can't be damaged.
NOPUSH - N/A, can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_access_card (edict_t *ent)
{
	VectorSet (ent->mins, -2, -3,-1);
	VectorSet (ent->maxs,  5,  3, 1);

	SimpleModelInit2(ent,&genericModelData[OBJ_ACCESS_CARD],NULL,NULL);

	ent->spawnflags |= SF_INVULNERABLE;
	ent->spawnflags |= SF_NOPUSH;
}

void banner_animate (edict_t *ent)
{
	if (ent->spawnflags & SF_NOANIMATE)		// No animating
	{
		ent->ghoulInst->Pause(level.time);
	}
	else 
	{
		ent->s.sound = gi.soundindex("Ambient/Models/Banner/BanFlap.wav");// Make a flapping sound
		ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;

		SimpleModelSetSequence(ent,genericModelData[OBJ_BANNER].file,SMSEQ_LOOP);
	}
}

BannerCallback  theBannerCallback;

bool BannerCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	edict_t* self = (edict_t*)ent;

	banner_animate(self);

	return true;
}

void banner_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	GhoulID	tempNote=0;
	ggObjC	*cLight;

	if (!ent->ghoulInst)
	{
		return;
	}

	SimpleModelSetSequence(ent,"pain",SMSEQ_HOLD);

	// register a callback for the end of the pain sequence
	cLight = game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject());
	tempNote=cLight->GetMyObject()->FindNoteToken("EOS");

	if (tempNote && ent->ghoulInst)
	{
		ent->ghoulInst->AddNoteCallBack(&theBannerCallback,tempNote);
	}
}


/*QUAKED misc_generic_banner (1 .5 0) (-4.2 -50.12 -125.3) (4.01 51 126)  INVULNERABLE  NOPUSH  NOANIMATE x x x FLUFF
2 story tall wall banner that waves in the wind
------ KEYS ------
Skin - 
0 - symbol of "Ugandan organization" 
1 - Iraq flag
2 - Saddam
3 - Siberia
------ SPAWNFLAGS ------
INVULNERABLE - N/A, can't be damaged.
NOPUSH - N/A, can't be pushed
NOANIMATE - won't flutter
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_banner_generic (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -4, -51, -70);//126
	VectorSet (ent->maxs, 4, 51, 70);//126

	// It has a pain anim so it has to be vulnerable.  But's going to take a long time to kill
	ent->health = 999999;
	if (ent->spawnflags & SF_INVULNERABLE)
	{
		ent->spawnflags &= ~SF_INVULNERABLE;
	}

//	ent->spawnflags |= SF_INVULNERABLE;
	ent->spawnflags |= SF_NOPUSH;

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "banner2";
		break;
	case 2: // Saddam -- potentially objectionable
		if (lock_textures)
		{	// replace objectionable textures
			skinname = "banner5";
		}
		else
		{
			skinname = "banner3";
		}
		break;
	case 3:
		skinname = "banner4";
		break;
	default :
		skinname = "banner1";
		break;
	}

	ent->pain = banner_pain;

	SimpleModelInit2(ent,&genericModelData[OBJ_BANNER],skinname,NULL);

	if (ent->spawnflags & SF_NOANIMATE)
		ent->ghoulInst->Pause(level.time);
	else	
	{
		ent->think = banner_animate;
		ent->nextthink = level.time + ((rand() % 5) * .1) ;
	}

	gi.soundindex("Ambient/Models/Banner/BanFlap.wav"); // precache sound
}


void barbwire_touch (edict_t *ent, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	vec3_t		backoff;

	VectorSubtract(other->s.origin, ent->s.origin, backoff);
	T_Damage(other, ent, ent, backoff, other->s.origin, other->s.origin, 1, 1, DT_MELEE, MOD_BARBWIRE);	
}

/*QUAKED misc_generic_barbwire_coil (1 .5 0) (-15 -59 -17) (15 59 17)  VULNERABLE  NOPUSH x x x x FLUFF
Roll of barbed-wire.  Belongs atop a fence.  
------ KEYS ------
message - text printed when killed
------ SPAWNFLAGS ------
VULNERABLE - can be damaged.
NOPUSH - N/A, can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
Hurts player when touched, causes player to jerk back.
*/
void SP_misc_generic_barbwire_coil (edict_t *ent)
{

	VectorSet (ent->mins, -15, -59, -17);
	VectorSet (ent->maxs,  15,  59,  17);

	// Backwards about invincibility
	if (ent->spawnflags & SF_INVULNERABLE)
	{
		ent->spawnflags &= ~SF_INVULNERABLE; 
	}
	else 
	{
		ent->spawnflags |= SF_INVULNERABLE;
	}

	ent->spawnflags |= SF_NOPUSH;	// Won't ever move

	ent->touch = barbwire_touch;

	SimpleModelInit2(ent,&genericModelData[OBJ_BARBWIRE_COIL],NULL,NULL);
}


void barrel_fire_touch (edict_t *ent, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	vec3_t		up = {0,0,1};
	vec3_t		point;

	
//	if (abs(other->absmin[2] - ent->absmax[2]) < 5)
	if ((other->s.origin[2] + other->mins[2]) > ent->s.origin[2])
	{
		VectorSet(point, other->s.origin[0], other->s.origin[1], other->absmin[2]);
		T_Damage(other, ent, ent, up, point, point, 1, 0, DT_FIRE|DAMAGE_NO_ARMOR, MOD_FIRE);	
	}
	return;
}

/*QUAKED misc_generic_barrel_burning (1 .5 0) (-10 -10 -12) (10 10 19) VULNERABLE NOPUSH ON_FIRE x x x FLUFF
Open barrel.  
------ SPAWNFLAGS ------
VULNERABLE - can be damaged.
NOPUSH - won't move
ON_FIRE: Puts fire on top.
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
Will burn the player if it is ON_FIRE and the player stands on it.
*/
void SP_misc_generic_barrel_burning (edict_t *ent)
{
	GhoulID		boltID;

	VectorSet (ent->mins, -11, -10, -15);
	VectorSet (ent->maxs, 11, 10, 15);

	// Backwards about invincibility
	if (ent->spawnflags & SF_INVULNERABLE)
	{
		ent->spawnflags &= ~SF_INVULNERABLE; 
	}
	else 
	{
		ent->spawnflags |= SF_INVULNERABLE;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_BARREL_BURNING],NULL,NULL);

	if (ent->spawnflags & BARREL_ON_FIRE)
	{
		boltID = ent->ghoulInst->GetGhoulObject()->FindPart("DUMMY01");
		fxRunner.execContinualEffect("environ/firesmall1", ent,boltID);

		ent->s.sound = gi.soundindex("Ambient/Gen/FireFX/FireSmall.wav");
		ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;

		ent->touch = barrel_fire_touch;
	}
}


/******************************************************************************/
// Barrel Chemical Code
/******************************************************************************/
void barrel_chemical_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	self->die = NULL;

	if (self->spawnflags & BARREL_EXPLOSIVE)
	{
  		FX_MakeBarrelExplode(self->s.origin, self);

		fxRunner.exec("weapons/world/airexplode", self->s.origin);
		FX_C4Explosion(self);

		T_RadiusDamage (self, self, 350, self, 128, 0);
		gmonster.RadiusDeafen(self, 150, 200);
		ShakeCameras (self->s.origin, 700, 1000, DEFAULT_JITTER_DELTA);
		IncreaseSpawnIntensity(.5);
	}

	BecomeDebris (self,inflictor,attacker,damage,point);
	
}

void barrel_chemical_fall (edict_t *self)
{
	if (self->health <= 0)	
	{
		return;
	}

	if ((self->s.angles[0] >= 90) || (self->s.angles[0] <= -90) ||
		(self->s.angles[2] >= 90) || (self->s.angles[2] <= -90))
	{
		if (self->s.angles[0] >= 90)
		{
			self->s.angles[0] = 90;
		}
		if (self->s.angles[0] <= -90)
		{
			self->s.angles[0] = -90;
		}
		if (self->s.angles[2] >= 90)
		{
			self->s.angles[2] = 90;
		}
		if (self->s.angles[2] <= -90)
		{
			self->s.angles[2] = -90;
		}
		VectorClear(self->avelocity);
		if (self->health < 25)
		{
			self->nextthink = level.time + .3;
		}
		else
		{
			// finished falling. need to change the bbox
			if (!(self->spawnflags & BARREL_EXPLOSIVE) && (abs(self->velocity[0]) < 50) &&
				(abs(self->velocity[1])<50) && (abs(self->velocity[2])<50))
			{
				FX_MakeDecalBelow(self->s.origin,FXDECAL_OOZE, 0); // ooze
			}
			self->nextthink = 0;
			self->mins[2] += 6;
			self->gravity = 1;
			self->movetype = MOVETYPE_STEP;
			self->pain = NULL;
			gi.linkentity(self);
		}
		return;
	}
	self->avelocity[0] *= 1.6;
	self->avelocity[2] *= 1.6;
	self->nextthink = level.time + .1;
}

void barrel_chemical_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t	falldir;
	vec3_t	prelimavel;
	float	angle;

	VectorSubtract(self->s.origin, other->s.origin, falldir);
	VectorNormalize(falldir);
	angle = 360 - self->s.angle_diff;
	angle = NormalizeAngle(angle);
	angle *= DEGTORAD;
	prelimavel[0] = falldir[0] * -70;
	prelimavel[2] = falldir[1] * 70;
	self->avelocity[0] = -1*(prelimavel[0] * cos(angle) + prelimavel[2] * sin(angle));
	self->avelocity[2] = -1*(prelimavel[2] * cos(angle) + prelimavel[0] * -1 * sin(angle));
	VectorScale(falldir, damage*4, self->velocity);
	self->velocity[2] = 150;
	self->think = barrel_chemical_fall;
	self->nextthink = level.time + .1;
	self->elasticity = .7;
}

void barrel_chemical_use (edict_t *self, edict_t *other, edict_t *activator)
{
	barrel_chemical_die (self, self, other, self->health, vec3_origin);
}

/*QUAKED misc_generic_barrel_chemical (1 .5 0) (-9 -9 -15) (10 8 15) INVULNERABLE NOPUSH EXPLOSIVE DEKKER x x FLUFF
Chemical barrel.  
------ KEYS ------
message - text printed when killed
skin - 
0 - normal barrel
1 - snowy barrel (Siberia)
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - won't move
EXPLOSIVE: Will explode when it dies.
DEKKER - special barrel for Dekker to play with
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- Tips over when shot and leaks. 
- Explosive barrels are given a red skin, others are brown.
*/
void SP_misc_generic_barrel_chemical (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -9, -9, -15);
	VectorSet (ent->maxs, 10, 8, 15);

	ent->pain = barrel_chemical_pain;
	ent->die = barrel_chemical_die;
	ent->use = barrel_chemical_use;

	if (ent->spawnflags & BARREL_EXPLOSIVE)
	{
		skinname = "barrel2";
		if (0 == ent->health)
		{
			ent->health = 1;
		}
	}
	else
	{
		switch (ent->s.skinnum)
		{
		case 1:
			skinname = "barrel3";
			break;
		default :
			skinname = "barrel";
			break;
		}
	}


	SimpleModelInit2(ent,&genericModelData[OBJ_BARREL_CHEMICAL],skinname,NULL);

	// cache sound CLSFX_EXPLODE
	entSoundsToCache[CLSFX_EXPLODE] = 1;
	// cache debris CLGHL_EXPLODEBIG, CLGHL_EXPLODESML
	entDebrisToCache[CLGHL_EXPLODEBIG] = DEBRIS_YES;
	entDebrisToCache[CLGHL_EXPLODESML] = DEBRIS_YES;

	gi.soundindex("Ambient/Gen/FireFX/FireSmall.wav"); // precache sound
}

/*QUAKED misc_generic_barrel_wine (1 .5 0) (-15 -10 -10) (15 10 10)  INVULNERABLE  NOPUSH x x x x FLUFF
A barrel of wine laying on it's side
------ KEYS ------
message - text printed when killed
------ SPAWNFLAGS ------
INVULNERABLE - can be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_barrel_wine (edict_t *ent)
{

	VectorSet (ent->mins, -15, -10, -10);
	VectorSet (ent->maxs,  15,  10,  10);

	SimpleModelInit2(ent,&genericModelData[OBJ_BARREL_WINE],NULL,NULL);
}


// called when the top is shot off of the bench
void benchtop_pain(edict_t *self, int nPartLost, vec3_t boltPos)
{
	//HLTH_GEN_BENCH
}

// pain fn for the actual bench entity
void benchbottom_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	//HLTH_GEN_BENCH
	trace_t			trace;
	vec3_t			fwd, right, up, vEnd, vToWhereHit;
	IGhoulInst*		topInst = NULL;
	Matrix4			matTop, matRot, matNew;

	AngleVectors(self->s.angles,fwd,right,up);
	VectorSubtract(wherehit, self->s.origin, vToWhereHit);
	
	// was it shot from the front?
	if (DotProduct(fwd, vToWhereHit) > 0)
	{
		// if the bench is still upright, try to knock it over
		if (DotProduct(up, vec3_up) > 0.9f)
		{
			VectorMA(self->s.origin, -100.0f, fwd, vEnd);
			gi.trace (self->s.origin, NULL, NULL, vEnd, self, self->clipmask, &trace);
			if (trace.fraction < 1.0f)
			{
			}
			else
			{
				// nothing stopping us from falling backward
				vec3_t vTemp;
				self->s.angles[PITCH] = -90;
				VectorScale(fwd, -20.0f, vTemp);
				VectorAdd(self->s.origin, vTemp, self->s.origin);
				vTemp[2] -= 6;
				// modify bbox so that absmin and absmax remain the same
				VectorSubtract(self->mins, vTemp, self->mins);
				VectorSubtract(self->maxs, vTemp, self->maxs);

				// knock off the top part (if it's still there)
				if (topInst = SimpleModelGetBolt(self, 1))
				{
					topInst->GetXForm(matTop);
					matTop.Rotate(1, -3.1416f*0.125f);
					topInst->SetXForm(matTop);
				}
			}
		}
	}
	else
	{
		// it was hit from behind. try to knock it forward
		if (DotProduct(up, vec3_up) > 0.9f)
		{
			VectorMA(self->s.origin, 100.0f, fwd, vEnd);
			gi.trace (self->s.origin, NULL, NULL, vEnd, self, self->clipmask, &trace);
			if (trace.fraction < 1.0f)
			{
			}
			else
			{
				// nothing stopping us from falling forward
				vec3_t vTemp;
				VectorScale(fwd, 20.0f, vTemp);
				vTemp[2] -= 7.0f;
				self->s.angles[PITCH] = 70;
				VectorAdd(self->s.origin, vTemp, self->s.origin);
				// modify bbox so that absmin and absmax remain the same
				VectorSubtract(self->mins, vTemp, self->mins);
				VectorSubtract(self->maxs, vTemp, self->maxs);

				// knock off the top part (if it's still there)
				if (topInst = SimpleModelGetBolt(self, 1))
				{
					VectorSet(vEnd, -1.5f, -2.0f, 0);
					topInst->GetXForm(matTop);
					matTop.Translate(*(Vect3*)&vEnd);
					matRot.Rotate(2, 3.141f*0.33f);
					matNew.Concat(matTop, matRot);
					topInst->SetXForm(matNew);
				}
			}
		}
	}

	Obj_partpain(self, other, kick, damage, wherehit);
}

/*QUAKED misc_generic_bench (1 .5 0)  (-11 -30 -18) (11 30 18) INVULNERABLE NOPUSH x x x x FLUFF
A highbacked wooden bench.
------ KEYS ------
message - text printed when killed
skin - 
0 - NYC bench (brown wood)
1 - Trainyard bench (beat up, faded)
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - won't move
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- falls apart when shot
*/
void SP_misc_generic_bench (edict_t *ent)
{
	char *skinname;

	// Set up object to break apart when shot
	Obj_partbreaksetup (BPD_BENCH,&genericModelData[OBJ_BENCH_BOTTOM],
		genericObjBoltPartsData,genericObjBreak);

	VectorSet (ent->mins, -10, -30,-17);
	VectorSet (ent->maxs,  10,  30, 17);


	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "bench2";
		break;
	default :
		skinname = "bench";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_BENCH_BOTTOM],skinname,NULL);

	gi.linkentity (ent);

	SimpleModelAddBolt(ent,genericModelData[OBJ_BENCH_BOTTOM],"DUMMY02",
				genericModelData[OBJ_BENCH_TOP],"DUMMY01",skinname);

	ent->pain = benchbottom_pain;
	VectorCopy(ent->s.angles,ent->moveinfo.start_angles);
}


void blinds_stoppain (edict_t *ent)
{
	ent->ghoulInst->Pause(level.time);
}

void blind_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	ent->ghoulInst->Resume(level.time);
	ent->think = blinds_stoppain;
	ent->nextthink = level.time + (5 * FRAMETIME);

	SetSkin(ent,genericModelData[OBJ_BLINDS].dir,genericModelData[OBJ_BLINDS].file, 
		 genericModelData[OBJ_BLINDS].materialfile,"blindsdmg", 0);

}

/*QUAK-ED misc_generic_blinds (1 .5 0) (-9 -55 -65) (9 55 65)  VULNERABLE  NOPUSH  ANIMATE x x x FLUFF
A big set of grey window blinds
------ SPAWNFLAGS ------
VULNERABLE - can be hurt
NOPUSH - can't be pushed
ANIMATE - blinds flap in the breeze
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- shakes when shot 
*/
void SP_misc_generic_blinds (edict_t *ent)
{

	VectorSet (ent->mins, -1, -55, -65);
	VectorSet (ent->maxs, 0, 55, 65);

	// Backwards about invincibility
	if (ent->spawnflags & SF_INVULNERABLE)
	{
		ent->spawnflags &= ~SF_INVULNERABLE; 
	}
	else 
	{
		ent->spawnflags |= SF_INVULNERABLE;
	}

	ent->spawnflags |= SF_NOPUSH;	// Won't ever move

 	ent->pain = blind_pain;

	SimpleModelInit2(ent,&genericModelData[OBJ_BLINDS],NULL,NULL);

	SetSkin(ent,genericModelData[OBJ_BLINDS].dir,
		genericModelData[OBJ_BLINDS].file, 
		genericModelData[OBJ_BLINDS].materialfile,"blinds", 0);

//	SimpleModelScale(ent,0.6f);

	if (ent->spawnflags & SF_NOANIMATE)
		ent->ghoulInst->Pause(level.time);

//	else	// Make a flapping sound
//		ent->s.sound = gi.soundindex("Ambient/Models/Banner/BanFlap.wav");
}


void bookcase_partpain (edict_t *self,int partLost, vec3_t boltPos)
{
}

// Boltee is the thing being added on
// Bolter is the thing it is being added to
/*QUAKED misc_generic_bookshelf (1 .5 0) (-40 -16 0) (40 16 64)  INVULNERABLE  NOPUSH x x x x FLUFF
A bookshelf
--------SPAWNFLAGS----------
INVULNERABLE - can't be hurt
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_bookshelf (edict_t *ent)
{
	char			*skinname;

	// Set up object to break apart when shot
	Obj_partbreaksetup (BPD_BOOKCASE,&genericModelData[OBJ_BOOKCASE],
		genericObjBoltPartsData,genericObjBreak);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "middle";
		break;
	default :
		skinname = "case";
		break;
	}


	VectorSet (ent->mins, -40, -16,-0);
	VectorSet (ent->maxs,  40,  16, 64);

	SimpleModelInit2(ent,&genericModelData[OBJ_BOOKCASE],skinname,NULL);
	gi.linkentity (ent);

	SimpleModelAddBolt(ent,genericModelData[OBJ_BOOKCASE],"DUMMY01",
						genericModelData[OBJ_BOOKSHELF],"DUMMY01",NULL);
	SimpleModelAddBolt(ent,genericModelData[OBJ_BOOKCASE],"DUMMY02",
						genericModelData[OBJ_BOOKSHELF],"DUMMY04",NULL);
	SimpleModelAddBolt(ent,genericModelData[OBJ_BOOKCASE],"DUMMY03",
						genericModelData[OBJ_BOOKSHELF],"DUMMY01",NULL);

	ent->pain = Obj_partpain;
	VectorCopy(ent->s.angles,ent->moveinfo.start_angles);
}

void broom_fall (edict_t *self)
{
	if (self->health <= 0)	
	{
		return;
	}

	if ((self->s.angles[0] >= 90) || (self->s.angles[0] <= -90) ||
		(self->s.angles[2] >= 90) || (self->s.angles[2] <= -90))
	{
		if (self->s.angles[0] >= 90)
		{
			self->s.angles[0] = 90;
		}
		if (self->s.angles[0] <= -90)
		{
			self->s.angles[0] = -90;
		}
		if (self->s.angles[2] >= 90)
		{
			self->s.angles[2] = 90;
		}
		if (self->s.angles[2] <= -90)
		{
			self->s.angles[2] = -90;
		}
		VectorClear(self->avelocity);
		if (self->health < 25)
		{
			self->nextthink = level.time + .3;
		}
		else
		{
			self->nextthink = 0;
		}
		return;
	}
	self->avelocity[0] *= 1.6;
	self->avelocity[2] *= 1.6;
	self->nextthink = level.time + .1;
}


void broom_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{

}

/*QUAKED misc_generic_broom (1 .5 0) (-3 -17 -26) (3 17 26)  INVULNERABLE  NOPUSH x x x x FLUFF
An upright push-broom
------ KEYS ------
skin - 
0 - old beat up broom
1 - new broom
------ SPAWNFLAGS ------
INVULNERABLE - can't be hurt
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_broom (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -2, -16,-25);
	VectorSet (ent->maxs,  2,  16, 25);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "broom2";
		break;
	default :
		skinname = "broom";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_BROOM],skinname,NULL);

	ent->pain = broom_pain;

}

void bunk_layflat(edict_t *self)
{
	self->s.angles[PITCH] = self->moveinfo.start_angles[PITCH];
	self->s.angles[PITCH] -= 90;
	VectorSet (self->mins, -40, -20,35);
	VectorSet (self->maxs,  40,  20, 40);
	self->solid = SOLID_BBOX;
	gi.linkentity (self);
}

void bunk_layflat2(edict_t *self)
{
	self->s.angles[PITCH] = self->moveinfo.start_angles[PITCH];
	self->s.angles[PITCH] += 90;
	VectorSet (self->mins, -40, -20,35);
	VectorSet (self->maxs,  40,  20, 40);
	self->solid = SOLID_BBOX;
	gi.linkentity (self);
}

void bunk_roll1(edict_t *self)
{
	self->s.angles[PITCH] -= 6;
	VectorSet (self->mins, -40, -20,-24);
	VectorSet (self->maxs,  40,  20, 29);
	gi.linkentity (self);
}

void bunk_roll2(edict_t *self)
{
	self->s.angles[PITCH] -= 30;
	VectorSet (self->mins, -40, -20,-10);
	VectorSet (self->maxs,  40,  20, 29);
	gi.linkentity (self);
}

void bunk_roll3(edict_t *self)
{
	self->s.angles[PITCH] += 6;
	VectorSet (self->mins, -40, -20,-24);
	VectorSet (self->maxs,  40,  20, 29);
	gi.linkentity (self);
}

void bunk_roll4(edict_t *self)
{
	self->s.angles[PITCH] += 30;
	VectorSet (self->mins, -40, -20,-10);
	VectorSet (self->maxs,  40,  20, 29);
	gi.linkentity (self);
}

#define BUNK_FALL		(M_PI*.11)
#define BUNKSIDE_FALL	(M_PI*.3)

void bunk_sidestogether(edict_t *self)
{
	Matrix4 matOld;
	IGhoulInst* ShelfInst;
	IGhoulInst	*BunkBoltInst;
	Matrix4	mat1,mat2;
	vec3_t boltPos;
	vec3_t	right,fwd;

	BunkBoltInst = SimpleModelGetBolt(self, 1);

	if (BunkBoltInst)
	{
		GetGhoulPosDir(self->s.origin, self->s.angles, BunkBoltInst,
					   NULL, "DUMMY04", boltPos, NULL, NULL, NULL);
	}

	AngleVectors(self->s.angles,fwd,right,NULL);
	VectorMA(boltPos,-5,right,boltPos);
	VectorMA(boltPos,80,fwd,boltPos);

	ShelfInst = BunkBoltInst;
	ShelfInst->GetXForm(matOld);
	mat1=mat2=matOld;
	mat2.Rotate(2,BUNKSIDE_FALL);
	matOld.Concat(mat1, mat2);
	ShelfInst->SetXForm(matOld);

	// Turn other side	
	GetGhoulPosDir(self->s.origin, self->s.angles, BunkBoltInst,
					   NULL, "DUMMY03", boltPos, NULL, NULL, NULL);
	BunkBoltInst = SimpleModelGetBolt(self, 4);

	AngleVectors(self->s.angles,fwd,right,NULL);
	VectorMA(boltPos,-5,right,boltPos);
	VectorMA(boltPos,80,fwd,boltPos);

	ShelfInst = BunkBoltInst;
	ShelfInst->GetXForm(matOld);
	mat1=mat2=matOld;
	mat2.Rotate(2,-BUNKSIDE_FALL);
	matOld.Concat(mat1, mat2);
	ShelfInst->SetXForm(matOld);

}

// called when a part is shot off of the bunkbed
void bunk_partpain (edict_t *self,int partLost, vec3_t boltPos)
{
	Matrix4			matOld;
	IGhoulInst*		ShelfInst;
	IGhoulInst		*BunkBoltInst;
	Matrix4			mat1,mat2;
	bool			B1, B2, B3, B4;
//	ggOinstC *myInstance;
//	ggObjC *MyGhoulObj;
	boltInstInfo_c *boltInfo = NULL;
	vec3_t			debrisNorm;

	if (!self->objInfo || !(boltInfo = (boltInstInfo_c*)self->objInfo->GetInfo(OIT_BOLTINST)) )
	{
		return;
	}
	B1 = boltInfo->IsOn(1);
	B2 = boltInfo->IsOn(2);
	B3 = boltInfo->IsOn(3);
	B4 = boltInfo->IsOn(4);


	// Bolt1 - Other side of bed
	// Bolt2 - Top bunk
	// Bolt3 - Bottom bunk
	// Bolt4 - Other side of bed

	switch (partLost)
	{
	case(1):	// Side of bed

		// Has other side, and both beds
		// Make bed lean to side and top bunk angle down
		if ((B2) && (B3) && (B4))
		{
			bunk_roll1(self);

			BunkBoltInst = SimpleModelGetBolt(self, 2);

			// FIXME: have we been passed this boltPos?
			GetGhoulPosDir(self->s.origin, self->s.angles, BunkBoltInst,
							   NULL, "DUMMY01", boltPos, NULL, NULL, NULL);

			ShelfInst = BunkBoltInst;
			ShelfInst->GetXForm(matOld);
			mat1=mat2=matOld;
			mat2.Rotate(2,-BUNK_FALL);
			matOld.Concat(mat1, mat2);
			ShelfInst->SetXForm(matOld);

		}
		// Has other side and top bunk only
		// Make bed lean to side
		else if ((B2) && (!B3) && (B4))
		{
			bunk_roll2(self);
		}
		// Other side gone, has top and bottom bunk
		// Make top bunk go away and drop bottom bunk to floor
		else if ((B2) && (B3) && (!B4))
		{
			Obj_partkill (self,2,true);

			self->s.angles[PITCH] = self->moveinfo.start_angles[PITCH];

			VectorSet (self->mins, -40, -20,-20);
			VectorSet (self->maxs,  40,  20, 5);
			gi.linkentity (self);

		}
		// Other side gone, bottom bunk gone, has top bunk
		// Make top bunk go away and drop bottom bunk to floor
		else if ((B2) && (!B3) && (!B4))
		{
			self->s.angles[PITCH] = self->moveinfo.start_angles[PITCH];

			VectorSet (self->mins, -40, -20,15);
			VectorSet (self->maxs,  40,  20,25);
			gi.linkentity (self);

		}
		// Both bunks are gone, one side there
		else if ((!B1) && (!B2) && (!B3) && (B4))
		{

			// Turn other side	
			BunkBoltInst = SimpleModelGetBolt(self,4);

			GetGhoulPosDir(self->s.origin, self->s.angles, BunkBoltInst,
							   NULL, "DUMMY03", boltPos, NULL, NULL, NULL);


			ShelfInst = BunkBoltInst;
			ShelfInst->GetXForm(matOld);
			mat1=mat2=matOld;
			mat2.Rotate(2,BUNKSIDE_FALL);
			matOld.Concat(mat1, mat2);
			ShelfInst->SetXForm(matOld);

			bunk_layflat(self);
		}	
		else if ((!B1) && (!B2) && (B3) && (B4))
		{
			bunk_roll1(self);
		}
		else if ((!B1) && (!B2) && (B3) && (!B4))
		{
			self->s.angles[PITCH] = self->moveinfo.start_angles[PITCH];

			VectorSet (self->mins, -40, -20,-20);
			VectorSet (self->maxs,  40,  20, 5);
			gi.linkentity (self);
		}

		break;
	case(2):	// Top bunk
		// Both bunks are gone, both sides are there
		if ((!B2) && (!B3) && (B1) && (B4))
		{			
			bunk_sidestogether(self);
		}	
		// Both bunks are gone, one side there
		else if ((!B1) && (!B2) && (!B3) && (B4))
		{
			bunk_layflat(self);
		}	
		// Both bunks are gone, other side there
		else if ((B1) && (!B2) && (!B3) && (!B4))
		{
			bunk_layflat2(self);
		}	


		break;

	case(3):	// Bottom bunk
		// Both bunks are gone, one side there
		if ((!B1) && (!B2) && (!B3) && (B4))
		{
			bunk_layflat(self);
		}	
		// Both bunks are gone, other side there
		if ((B1) && (!B2) && (!B3) && (!B4))
		{
			bunk_layflat2(self);
		}	
		// Both bunks are gone, both sides are there
		else if ((!B2) && (!B3) && (B1) && (B4))
		{			
			bunk_sidestogether(self);
		}	

		break;
	case(4):	// Other side
		// Both bunks there and other side there
		if ((B1) && (B2) && (B3) && (!B4))
		{
			Obj_partkill (self,2,true);	// Remove top bunk

			bunk_roll3(self);

		}	
		// Both bunks are gone, one side there
		else if ((B1) && (!B2) && (!B3) && (!B4))
		{
			// Turn other side	
			BunkBoltInst = SimpleModelGetBolt(self,1);

			GetGhoulPosDir(self->s.origin, self->s.angles, BunkBoltInst,
							   NULL, "DUMMY03", boltPos, NULL, NULL, NULL);

			ShelfInst = BunkBoltInst;
			ShelfInst->GetXForm(matOld);
			mat1=mat2=matOld;
			mat2.Rotate(2,-BUNKSIDE_FALL);
			matOld.Concat(mat1, mat2);
			ShelfInst->SetXForm(matOld);

			bunk_layflat2(self);
		}	
		// Bottom bunk gone, top bunk there, one side there
		else if ((B1) && (B2) && (!B3) && (!B4))
		{
			bunk_roll4(self);
		}
		// Top bunk gone, bottom bunk there, one side there
		else if ((B1) && (!B2) && (B3) && (!B4))
		{
			bunk_roll3(self);
		}
		else if ((!B1) && (!B2) && (B3) && (!B4))
		{
			self->s.angles[PITCH] = self->moveinfo.start_angles[PITCH];

			VectorSet (self->mins, -40, -20,-20);
			VectorSet (self->maxs,  40,  20, 5);
			gi.linkentity (self);
		}
		break;
	default:
		break;
	}

	VectorClear(debrisNorm);
	// Throw debris and make it go away
	FX_ThrowDebris(boltPos,debrisNorm, 5, DEBRIS_SM, self->material, 0,0,0, self->surfaceType);
	FX_SmokePuff(boltPos,120,120,120,200);
}


/*QUAKED misc_generic_bunk (1 .5 0)  (-41 -21 -30) (41 21 30)  INVULNERABLE  NOPUSH x x x x FLUFF
Bunk bed
------ KEYS ------
Skin - 
0 - hi-tech bunk
1 - iraq bunk
------ SPAWNFLAGS ------
INVULNERABLE - can't be hurt
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- falls apart when shot
*/
void SP_misc_generic_bunk (edict_t *ent)
{
	char			*skinname;
	IGhoulInst*		BunkInst;
	Matrix4			matOld,mat2,mat1;
	ggBinstC		*bInstC = NULL;
	IGhoulInst		*boltInst = NULL;
	IGhoulInst		*boltInst2 = NULL;


	Obj_partbreaksetup (BPD_BUNK,&genericModelData[OBJ_BUNK],
		genericObjBoltPartsData,genericObjBreak);

	VectorSet (ent->mins, -40, -20,-29);
	VectorSet (ent->maxs,  40,  20, 29);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "bunk2";
		break;
	default :
		skinname = "bunk";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_BUNK],skinname,NULL);

	// Bolt1 - Other side of bed
	SimpleModelAddBolt(ent,genericModelData[OBJ_BUNK],"DUMMY05",
						genericModelData[OBJ_BUNKSIDE2],"DUMMY05",NULL);
	// Bolt2 - Top bunk
	SimpleModelAddBolt(ent,genericModelData[OBJ_BUNK],"DUMMY01",
						genericModelData[OBJ_BUNKBED],"DUMMY02",NULL);
	// Bolt3 - Bottom bunk
	SimpleModelAddBolt(ent,genericModelData[OBJ_BUNK],"DUMMY03",
						genericModelData[OBJ_BUNKBED],"DUMMY02",NULL);
	// Bolt4 - Other side of bed
	SimpleModelAddBolt(ent,genericModelData[OBJ_BUNK],"DUMMY03",
						genericModelData[OBJ_BUNKSIDE2],"DUMMY05",NULL);

	SimpleModelRemoveObject(ent,"SIDE02");
	
	// Rotating side panel 
	if (BunkInst = SimpleModelGetBolt(ent, 4))
	{
		BunkInst->GetXForm(matOld);
		mat1=mat2=matOld;
		mat2.Rotate(0,-M_PI*1);
		matOld.Concat(mat1, mat2);
		BunkInst->SetXForm(matOld);
	}
	ent->pain = Obj_partpain;
	VectorCopy(ent->s.angles,ent->moveinfo.start_angles);

}

void car_big_explode (edict_t *self)
{
	vec3_t	debrisNorm;
	VectorClear(debrisNorm);

	fxRunner.exec("weapons/world/airexplode", self->s.origin);
	FX_C4Explosion(self);

	BlindingLight(self->s.origin, self->health*10, 0.9, 0.5);
	ShakeCameras (self->s.origin, 100, 300, DEFAULT_JITTER_DELTA);

	FX_ThrowDebris(self->s.origin,debrisNorm, 8,DEBRIS_SM, self->material, 0,0,0, 0);
}

void car_flip_think (edict_t *self)
{
	if ((self->velocity[2] < 5) && (self->velocity[2] > -5))
	{
		self->s.angles[2] += self->count;

		if (self->s.angles[2] > 185)
		{
			self->count = -2;
		}
		else if (self->s.angles[2] < 170)
		{
			self->count = 2;
		}
	}

	if (level.time > self->pain_debounce_time)
	{
		car_big_explode(self);
	}
	else
	{
		self->nextthink = level.time + .1;
	}
}

void car_explode_think (edict_t *self)
{
	if (self->s.angles[2] > 172)
	{
		self->avelocity[2] = 0;
		self->think = car_flip_think;
		self->nextthink = level.time + .1;
		self->count = 2;	
		self->pain_debounce_time = level.time + 2;
	}
	else
	{
		self->think = car_explode_think;
		self->nextthink = level.time + .1;
	}
}

void car_explode (edict_t *self, edict_t *other, edict_t *activator)
{
	T_RadiusDamage (self, self, 100, self, 100, 0);

	gmonster.RadiusDeafen(self, 200, 200);

	fxRunner.exec("weapons/world/airexplode", self->s.origin);
	FX_C4Explosion(self);

	self->movetype = MOVETYPE_DAN;	// Can be pushed around

	self->velocity[2] = 500;
	self->avelocity[2] = 180;

	self->think = car_explode_think;
	self->nextthink = level.time + .1;
}

#define AMERICANCAR_TIREDRIVERFRONT	1
#define AMERICANCAR_TIREDRIVERBACK	2
#define AMERICANCAR_TIREPSNGRFRONT	4
#define AMERICANCAR_TIREPSNGRBACK	8

void car_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	ent = ent;
}
//void car_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
//{
//	float fDot;
//	vec3_t up, facing,towherehit,pathdir,cross;


//	AngleVectors(ent->s.angles,NULL,right,NULL);

//	VectorSubtract(wherehit,ent->s.origin,facing);
//	VectorNormalize(facing);

//	AngleVectors(ent->s.angles, right, NULL, NULL);
//	fDot = DotProduct(right, facing);


//	VectorSet(up, 0, 0, 1);
//	VectorSubtract(other->s.origin, ent->s.origin, pathdir);
//	VectorNormalize(pathdir);
//	VectorSubtract(wherehit,ent->s.origin, towherehit);
//	CrossProduct(pathdir, towherehit, cross);
//	fDot = DotProduct(cross, up);


//	fDot = fDot;

	// Is other within range??????
//	if ((fDot < 0.88) && (fDot > 0.80))
//	{
	// front passenger tire
//		ent->s.angles[ROLL] += 5;
//		ent->s.angles[PITCH] += 3;
//		VectorSet (ent->mins, -69, -30,-22);
//	}

	// front driver tire
/*		ent->s.angles[ROLL] -= 5;
		ent->s.angles[PITCH] += 3;
		VectorSet (ent->mins, -69, -30,-22);
*/

	// back driver tire
/*		ent->s.angles[ROLL] -= 5;
		ent->s.angles[PITCH] -= 3;
		VectorSet (ent->mins, -69, -30,-22);
*/

	// back passenger tire
/*		ent->s.angles[ROLL] += 5;
		ent->s.angles[PITCH] -= 3;
		VectorSet (ent->mins, -69, -30,-22);
*/


//		gi.linkentity (ent);

	// fuck

//}



/*QUAKED misc_generic_car_american (1 .5 0) (-95 -112 -29) (95 113 53)  INVULNERABLE  NOPUSH CAR_ON SIREN x x FLUFF
Police car, Taxi or Cadillac.
------ KEYS ------
Skin - 
0 - police car
1 - taxi 
2 - black cadillac
3 - miami police
------ SPAWNFLAGS ------
INVULNERABLE - can't be hurt
NOPUSH - N/A can't be pushed
CAR_ON - car lights will be on
SIREN - police car lights will be flashing
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_car_american (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -69, -30,-25);
	VectorSet (ent->maxs,  69,  30, 25);

	ent->spawnflags |= SF_NOPUSH;	// Won't ever move

	switch (ent->s.skinnum)
	{
	case 3:
		skinname = "car4";
		break;
	case 2:
		skinname = "car3";
		break;
	case 1:
		skinname = "car2";
		break;
	default :
		skinname = "car1";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_CAR_AMERICAN],skinname,NULL);

	VectorSet (ent->mins, -69, -30,-25);
	VectorSet (ent->maxs,  69,  30, 25);

	gi.linkentity (ent);

	if ((ent->s.skinnum==CAR_AMERICAN_POLICE) || (ent->s.skinnum==CAR_AMERICAN_MIAMI))
	{
		SimpleModelRemoveObject(ent,"_TAXILIGHT01");
		SimpleModelRemoveObject(ent,"TAXI_OMNI01");
		SimpleModelRemoveObject(ent,"TAXI_OMNI02");

		SimpleModelSetSequence(ent,"off",SMSEQ_HOLD);
	}
	else  
	{
//		SimpleModelRemoveObject(ent,"LIGHT_SIRENS01");
//		SimpleModelRemoveObject(ent,"LIGHT_SIRENS02");
		SimpleModelRemoveObject(ent,"MIDDLE_SECTION");
		SimpleModelRemoveObject(ent,"REDSIREN");
		SimpleModelRemoveObject(ent,"BLUESIREN");

		SimpleModelRemoveObject(ent,"REDCONE");
		SimpleModelRemoveObject(ent,"BLUECONE");

		SimpleModelRemoveObject(ent,"WORLD_1");
		SimpleModelRemoveObject(ent,"WORLD_2");

		SimpleModelSetSequence(ent,"off",SMSEQ_HOLD);

		if (ent->s.skinnum==CAR_AMERICAN_CADILLAC)
		{
			SimpleModelRemoveObject(ent,"_TAXILIGHT01");
			SimpleModelRemoveObject(ent,"TAXI_OMNI01");
			SimpleModelRemoveObject(ent,"TAXI_OMNI02");
		}
	}

	if (ent->spawnflags & CAR_AMERICAN_ON)
	{
		SimpleModelSetSequence(ent,"on",SMSEQ_HOLD);
	}

	if (ent->spawnflags & CAR_AMERICAN_SIREN)
	{
		ent->s.sound = gi.soundindex("Ambient/Models/Alarm/Police.wav");	
		ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_NORMATTN;

		SimpleModelSetSequence(ent,"car",SMSEQ_LOOP);
	}

//	ent->pain = car_pain;
	ent->count = 0;	// Poor little count gets used to flag which tires have been shot out
}

void car_european_use(edict_t *self, edict_t *other, edict_t *activator)
{
	self->solid = SOLID_BBOX;
	// he should be visible
	if (self->ghoulInst)
	{
		self->ghoulInst->SetOnOff(true, level.time);
	}
}

/*QUAKED misc_generic_car_european (1 .5 0) (-70 -31 -26) (70 31 26)  INVULNERABLE  NOPUSH TRIGGER_SPAWN x x x FLUFF
1980's Ford escort-like European egg-mobile car.
------ KEYS ------
Skin - 
0 -	clean and tan
1 -	dirty/rusty and red
2 - shot full o' holes, missing hood
------ SPAWNFLAGS ------
INVULNERABLE - can't be hurt
NOPUSH - N/A can't be pushed
TRIGGER_SPAWN - use it and it shows up
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_car_european (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -91, -33,-28);
	VectorSet (ent->maxs,  91,  33, 30);

	ent->spawnflags |= SF_NOPUSH;	// Won't ever move

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "car2";
		break;
	case 2:
		skinname = "car3";
		break;
	default :
		skinname = "car";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_CAR_EUROPEAN],skinname,NULL);
	if (ent->spawnflags & CAR_TRIGGER_SPAWN)
	{
		ent->solid = SOLID_NOT;
		// he shouldn't even be visible
		if (ent->ghoulInst)
		{
			ent->ghoulInst->SetOnOff(false, level.time);
		}
		ent->use = car_european_use;
	}
}

/*QUAKED misc_generic_car_honda (1 .5 0) (-56 -25 -24) (56 25 31)  INVULNERABLE  NOPUSH x x x x FLUFF
Tiny Japanese police car
------ SPAWNFLAGS ------
INVULNERABLE - can't be hurt
NOPUSH - N/A can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_car_honda (edict_t *ent)
{
	VectorSet (ent->mins, -55, -25,-23);
	VectorSet (ent->maxs,  55,  25, 30);

	ent->spawnflags |= SF_NOPUSH;	// Won't ever move

	SimpleModelInit2(ent,&genericModelData[OBJ_CAR_HONDA],NULL,NULL);

}

void generic_car_rolls_pain2(edict_t *self)
{
	IGhoulObj	*obj = self->ghoulInst->GetGhoulObject();
	GhoulID		hood = NULL_GhoulID;

	if (obj)
	{
		hood = obj->FindPart("dummy01");
		fxRunner.stopContinualEffect("environ/helismoke", self, hood);
	}
}

void generic_car_rolls_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	IGhoulObj	*obj = self->ghoulInst->GetGhoulObject();
	GhoulID hood = NULL_GhoulID;

	if (NULL == obj)
	{
		return;
	}
	if (!self->count)
		SimpleModelSetSequence(self,"car",SMSEQ_HOLD);

	self->pain = NULL;

	Obj_explode(self,&self->s.origin,0,0);

	if (self->s.sound)
	{
		self->s.sound = NULL;
		gi.sound (self, CHAN_BODY, gi.soundindex ("Ambient/Models/Alarm/CarDisarm.wav"), .8, ATTN_NORM, 0);
	}

	self->touch = NULL;

	hood = obj->FindPart("dummy01");

	fxRunner.execContinualEffect("environ/helismoke", self, hood, 0);

	self->think = generic_car_rolls_pain2;
	self->nextthink = level.time + 5;

}


void car_rolls_use (edict_t *self, edict_t *other, edict_t *activator)
{
	SimpleModelSetSequence(self,"car",SMSEQ_HOLD);
	self->count = 1;
}

void car_rolls_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if (other->client)
	{
		self->s.sound =  gi.soundindex("Ambient/Models/Alarm/CarAlarm.wav");
		self->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		self->touch = NULL;
	}
}


/*QUAKED misc_generic_car_rolls (1 .5 0) (-85 -30 -25) (85 30 47)  INVULNERABLE  NOPUSH x x x x FLUFF
A luxury car.
------ SPAWNFLAGS ------
INVULNERABLE - can't be hurt
NOPUSH - N/A can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- car alarm goes off when touched
- hood pops open when shot
*/
void SP_misc_generic_car_rolls (edict_t *ent)
{
	VectorSet (ent->mins, -84, -29,-24);
	VectorSet (ent->maxs,  84,  29, 46);

	ent->spawnflags |= SF_NOPUSH;	// Won't ever move
	ent->pain = generic_car_rolls_pain;
	ent->use = car_rolls_use;	// Use for trigger_uses
	ent->plUse = car_rolls_use;	// Use for when player pushes 'use' key
	ent->touch = car_rolls_touch;

	SimpleModelInit2(ent,&genericModelData[OBJ_CAR_ROLLS],NULL,NULL);

	ent->ghoulInst->Pause(level.time);

	ent->count = 0;

	// cache sound CLSFX_EXPLODE
	entSoundsToCache[CLSFX_EXPLODE] = 1;

	gi.soundindex ("Ambient/Models/Alarm/CarDisarm.wav"); // precache sound
	gi.soundindex("Ambient/Models/Alarm/CarAlarm.wav"); // precache sound
	gi.effectindex("environ/helismoke");
}

/*QUAKED misc_generic_car_sports (1 .5 0) (-83 -36 -20) (83 36 20)  INVULNERABLE  NOPUSH x x x x FLUFF
a Lamborghini sports car.
------ KEYS ------
Skin -
0 - shiny red
1 - shiny yellow
------ SPAWNFLAGS ------
INVULNERABLE - can't be hurt
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_car_sports (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -82, -35,-19);
	VectorSet (ent->maxs,  82,  35, 19);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "car2";
		break;
	default :
		skinname = "car";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_CAR_SPORTS],skinname,NULL);
}

/*QUAKED misc_generic_cart (1 .5 0)   (-11 -21 -12) (11 21 12)  INVULNERABLE  NOPUSH x x x x FLUFF
A wheeled cart.  Can put stuff on both shelves.
------ KEYS ------
skin -
0 - Clean, metal
1 - Dirty, bloody (ugn2)
2 - wood (iraq)
------ SPAWNFLAGS ------
INVULNERABLE - can't be hurt
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/

void SP_misc_generic_cart (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -10, -20,-11);
	VectorSet (ent->maxs,  10,  20, 11);
	ent->mass = 25;

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "cart2";
		break;
	case 2:
		skinname = "cartwood";
		ent->surfaceType = SURF_WOOD_LBROWN;
		ent->material = MAT_WOOD_LBROWN;
		break;
	default:
		skinname = "cart";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_CART],skinname,NULL);

}

void cash_register_paindone(edict_t *self)
{
	fxRunner.stopContinualEffect("environ/normalsmoke", self);
}

void cash_register_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t		spray1, spray2,fwd,up;

	VectorSet(spray1, (rand()%21) - 10, (rand()%21) - 10, 0);
	VectorSet(spray2, (rand()%21) - 10, (rand()%21) - 10, 0);

	FX_MakeSparks(self->s.origin, spray1, 2);
	FX_MakeSparks(self->s.origin, spray2, 2);

	AngleVectors(self->s.angles,fwd,NULL,up);

	if (self->s.skinnum==0)
	{
		SetSkin(self,genericModelData[OBJ_CASH_REGISTER].dir,genericModelData[OBJ_CASH_REGISTER].file, 
			 genericModelData[OBJ_CASH_REGISTER].materialfile,"registerd", 0);
	}
	self->pain = NULL;

	fxRunner.execContinualEffect("environ/normalsmoke", self);

	self->think = cash_register_paindone;
	self->nextthink = level.time + 2;

}

/*QUAKED misc_generic_cash_register (1 .5 0) (-9 -9 -10) (9 9 10) INVULNERABLE  NOPUSH x x x x FLUFF
A cash register. 
------ KEYS ------
skin : 
0 - normal clean register
1 - damaged
------ SPAWNFLAGS ------
INVULNERABLE - can't be hurt
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- has damage skin when shot
*/
void SP_misc_generic_cash_register (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -8, -8,-9);
	VectorSet (ent->maxs,  8,  8, 9);

	ent->pain = cash_register_pain;

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "registerd";
		break;
	default:
		skinname = "register";
		break;
	}	

	SimpleModelInit2(ent,&genericModelData[OBJ_CASH_REGISTER],NULL,NULL);

	SetSkin(ent,genericModelData[OBJ_CASH_REGISTER].dir,
		genericModelData[OBJ_CASH_REGISTER].file, 
		genericModelData[OBJ_CASH_REGISTER].materialfile,skinname, 0);
	gi.effectindex("environ/normalsmoke");
}

void chair_legs_flat(edict_t *self)
{
	VectorCopy(self->moveinfo.start_angles,self->s.angles);
	VectorSet (self->mins, -10, -9,-0);
	VectorSet (self->maxs,  10,  9, 18);
	gi.linkentity(self);
}

void chair_legs_roll1(edict_t *self)
{
	self->s.angles[PITCH] += 40;
	VectorSet (self->mins, -10, -9,-6);
	VectorSet (self->maxs,  10,  9, 18);
	gi.linkentity(self);
}

void chair_legs_roll2(edict_t *self)
{
	self->s.angles[ROLL] -= 40;
	VectorSet (self->mins, -10, -9,-6);
	VectorSet (self->maxs,  10,  9, 18);
	gi.linkentity(self);
}

void chair_legs_roll3(edict_t *self)
{
	self->s.angles[ROLL] += 40;
	VectorSet (self->mins, -10, -9,-6);
	VectorSet (self->maxs,  10,  9, 18);
	gi.linkentity(self);
}

void chair_legs_roll4(edict_t *self)
{
	self->s.angles[PITCH] -= 40;
	VectorSet (self->mins, -10, -9,-6);
	VectorSet (self->maxs,  10,  9, 18);
	gi.linkentity(self);
}

void chair_legs_partpain (edict_t *self,int partLost, vec3_t boltPos)
{
	bool		B1, B2, B3, B4;
	boltInstInfo_c *boltInfo = NULL;
	vec3_t	debrisNorm;

	if (!self->objInfo || !(boltInfo = (boltInstInfo_c*)self->objInfo->GetInfo(OIT_BOLTINST)) )
	{
		return;
	}
	B1 = boltInfo->IsOn(1);
	B2 = boltInfo->IsOn(2);
	B3 = boltInfo->IsOn(3);
	B4 = boltInfo->IsOn(4);

	if ((!B1) && (!B2) && (!B3) && (!B4))
	{	
		chair_legs_flat(self);
		return;
	}

	// These are given as you face the chair
	// bolt1 - left front leg
	// bolt2 - right front leg
	// bolt3 - right back leg
	// bolt4 - left back leg

	switch (partLost)
	{
	case 1:	// left front leg
		if ((!B2) && (B3) && (B4))
		{
			chair_legs_roll1(self);
		}
		else if ((B2) && (B3) && (!B4))
		{
			chair_legs_roll3(self);
		}
		else if ((B2) && (!B3) && (B4))
		{
			Obj_partkill (self,2,true);	// Remove legs
			Obj_partkill (self,4,true);	// Remove legs
			chair_legs_flat(self);
		}
		// Throw debris
		VectorClear(debrisNorm);
		FX_ThrowDebris(self->s.origin,debrisNorm, 7, DEBRIS_SM, self->material,0,0,0, self->surfaceType);

		break;
	case 2:	// right front leg
		if ((!B1) && (B3) && (B4))
		{
			chair_legs_roll1(self);
		}
		else if ((B1) && (!B3) && (B4))
		{
			chair_legs_roll2(self);
		}
		else if ((B1) && (B3) && (!B4))
		{
			Obj_partkill (self,1,true);	// Remove legs
			Obj_partkill (self,3,true);	// Remove legs
			chair_legs_flat(self);
		}
		// Throw debris
		VectorClear(debrisNorm);
		FX_ThrowDebris(self->s.origin,debrisNorm, 7, DEBRIS_SM, self->material,0,0,0, self->surfaceType);
		break;
	case 3:	// right back leg
		if ((!B2) && (B1) && (B4))
		{
			chair_legs_roll2(self);
		}
		else if ((B2) && (B1) && (!B4))
		{
			chair_legs_roll4(self);
		}
		else if ((!B1) && (B2) && (B4))
		{
			Obj_partkill (self,2,true);	// Remove legs
			Obj_partkill (self,4,true);	// Remove legs
			chair_legs_flat(self);
		}
		// Throw debris
		VectorClear(debrisNorm);
		FX_ThrowDebris(self->s.origin,debrisNorm, 7, DEBRIS_SM, self->material,0,0,0, self->surfaceType);
		break;
	case 4:	// left back leg
		if ((!B1) && (B2) && (B3))
		{
			chair_legs_roll3(self);
		}
		else if ((B1) && (B2) && (!B3))
		{
			chair_legs_roll4(self);
		}
		else if ((B1) && (!B2) && (B3))
		{
			Obj_partkill (self,1,true);	// Remove legs
			Obj_partkill (self,4,true);	// Remove legs
			chair_legs_flat(self);
		}
		// Throw debris
		VectorClear(debrisNorm);
		FX_ThrowDebris(self->s.origin,debrisNorm, 7, DEBRIS_SM, self->material,0,0,0, self->surfaceType);
		break;
	default :
		break;
	}
}

/*QUAKED misc_generic_chair_legs (1 .5 0)  (-11 -10 -19) (11 10 19) INVULNERABLE  NOPUSH x x x x FLUFF
A chair with four legs 
------ KEYS------
skin - 
0 - metal
1 - wooden
2 - white plastic
------ SPAWNFLAGS ------
INVULNERABLE - can't be hurt
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- falls apart when shot
*/
void SP_misc_generic_chair_legs (edict_t *ent)
{
	char *skinname;
	ggBinstC		*bInstC = NULL;
	IGhoulInst		*boltInst = NULL;
	IGhoulInst		*boltInst2 = NULL;

	VectorSet (ent->mins, -10, -9,-12);
	VectorSet (ent->maxs,  10,  9, 18);
	ent->mass = 50;

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "legsb";
		ent->surfaceType = SURF_WOOD_LBROWN;
		ent->material = MAT_WOOD_LBROWN;
		break;
	case 2:
		skinname = "legsplastic";
		ent->surfaceType = SURF_DEFAULT;
		ent->material = MAT_WALL_WHITE;
		break;
	default:
		skinname = "legs";
		ent->surfaceType = SURF_METAL;
		ent->material = MAT_METAL_LGREY;
		break;
	}

	Obj_partbreaksetup (BPD_CHAIR_LEGS,&genericModelData[OBJ_CHAIR_LEGS],
		genericObjBoltPartsData,genericObjBreak);

	SimpleModelInit2(ent,&genericModelData[OBJ_CHAIR_LEGS],skinname,NULL);

	// These are given as you face the chair
	// bolt1 - left front leg
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY01",
						genericModelData[OBJ_CHAIR_LEGS_LEGS],"DUMMY01",skinname);
	// bolt2 - right front leg
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY02",
						genericModelData[OBJ_CHAIR_LEGS_LEGS],"DUMMY01",skinname);
	// bolt3 - right back leg
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY03",
						genericModelData[OBJ_CHAIR_LEGS_LEGS],"DUMMY01",skinname);
	// bolt4 - left back leg
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY04",
						genericModelData[OBJ_CHAIR_LEGS_LEGS],"DUMMY01",skinname);
	ent->pain = Obj_partpain;
	VectorCopy(ent->s.angles,ent->moveinfo.start_angles);

}


/*QUAKED misc_generic_chair_legs2 (1 .5 0)  (-13 -11 -21) (13 11 21) INVULNERABLE  NOPUSH x x x x FLUFF
A chair with two long solid legs on the sides.
------ SPAWNFLAGS ------
INVULNERABLE - can't be hurt
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_chair_legs2 (edict_t *ent)
{
	VectorSet (ent->mins, -12, -10,-20);
	VectorSet (ent->maxs,  12,  10, 20);

	SimpleModelInit2(ent,&genericModelData[OBJ_CHAIR_LEGS2],NULL,NULL);
}


void chair_squeak (edict_t *self)
{
	int				soundrand;
	eft_rotate_t	*rotInfo = NULL;
	
	if (rotInfo = FXA_GetRotate(self))
	{
		if ( abs(rotInfo->xRot) > 2000)
		{
			soundrand = gi.irand(0,2);
		}
		else if ( abs(rotInfo->xRot) > 1000)
		{
			soundrand = gi.irand(0,6);
		}
		else
		{
			soundrand = -1;
		}
		switch(soundrand)
		{
		case 0:
		case 1:
		case 2:
			gi.sound (self, CHAN_BODY, gi.soundindex ("Impact/Chair/Squeak1.wav"), .8, ATTN_NORM, 0);
			break;
		default:
			break;
		}
		// slap some friction on this thing
		rotInfo->xRot *= 0.75f;
		if (abs(rotInfo->xRot) < (0.3f * 512))//512 accounts for the shift induced by FXA_SetRotate()
		{
			// stop the madness!
			FXA_RemoveRotate(self);
			self->think = NULL;
		}
		else
		{
			self->nextthink = level.time + 0.5f;
		}
	}
}

void chair_spin (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	IGhoulInst		*seat = NULL;
	vec3_t			towherehit, cross, pathdir, up, angVel = {0,0,0};
	eft_rotate_t	*rotInfo = NULL;
	float fMult = 0.1f * damage;

	if (seat = SimpleModelGetBolt(self, 1))
	{
		// only spin the seat portion of the chair
		VectorSet(up, 0, 0, 1);
		VectorSubtract(other->s.origin, self->s.origin, pathdir);
		VectorNormalize(pathdir);
		VectorSubtract(wherehit,self->s.origin, towherehit);
		CrossProduct(pathdir, towherehit, cross);
		if (DotProduct(cross, up) > 0)
		{
			if (rotInfo = FXA_GetRotate(self))
			{
				// adding to an existing rotation
				rotInfo->xRot += fMult*512;//512 accounts for the shift induced by FXA_SetRotate()
			}
			else
			{
				// need to create a rotating effect
				angVel[PITCH] = (fMult<30)?fMult:30;
				FXA_SetRotate(self, angVel, seat);
			}
		}
		else if (DotProduct(cross, up) < 0)
		{
			if (rotInfo = FXA_GetRotate(self))
			{
				// adding to an existing rotation
				rotInfo->xRot += (-fMult)*512;//512 accounts for the shift induced by FXA_SetRotate()
			}
			else
			{
				// need to create a rotating effect
				angVel[PITCH] = (fMult<30)?-fMult:-30;
				FXA_SetRotate(self, angVel, seat);
			}
		}
		
		self->think = chair_squeak;
		// need to think pretty often cuz that's where we'll slow down the chair's rotation to simulate friction
		self->nextthink = level.time + 0.5f;
	}
}

/*QUAKED misc_generic_chair_office (1 .5 0) (-10 -12 -18) (12 12 23) INVULNERABLE  NOPUSH x x x x FLUFF
Office chair on castors. 
------ KEYS ------
skin - 
0 - bright metal
1 - dark Tokyo chair
------ SPAWNFLAGS ------
INVULNERABLE - can't be hurt
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- Spins and creaks when shot.
 */
void SP_misc_generic_chair_office (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -13, -13, -17.5);
	VectorSet (ent->maxs, 13, 13, 24);

	ent->pain = chair_spin;

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "office2";
		break;
	default:
		skinname = "office";
		break;
	}


	SimpleModelInit2(ent,&genericModelData[OBJ_CHAIR_OFFICE_BASE],skinname,NULL);
	SimpleModelAddBolt(ent,genericModelData[OBJ_CHAIR_OFFICE_BASE],"DUMMY02",
						genericModelData[OBJ_CHAIR_OFFICE_SEAT],"DUMMY01",skinname);

	switch (ent->s.skinnum)
	{
	case 1:
		ent->material = MAT_METAL_DGREY;
		break;
	default:
		break;
	}

	gi.soundindex ("Impact/Chair/Squeak1.wav"); // precache sound
}

/*QUAKED misc_generic_chunks_brick (1 .5 0) (-7 -4 -3) (7 4 3)   INVULNERABLE NOPUSH x x x x FLUFF
Brick chunks used for debris
------ KEYS ------
Skin - 
0 - red brick (default)
1 - dark grey
2 - dark brown
3 - light brown
4 - light grey

Style - type of chunk
0 - normal brick (default)
1 - half a brick
2 - 1/4 of brick
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_chunks_brick (edict_t *ent)
{
	char *partname,*skinname;

	VectorSet (ent->mins, -6, -3, -2);
	VectorSet (ent->maxs,  6,  3,  2);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "grey_dk";
		break;
	case 2:
		skinname = "brwn_dk";
		break;
	case 3:
		skinname = "brwn_lt";
		break;
	case 4:
		skinname = "grey_lt";
		break;
	default:
		skinname = "red";
		break;
	}

	switch (ent->style)
	{
	case 1:
		partname = "CHUNK_2";
		break;
	case 2:
		partname = "CHUNK_3";
		break;
	default:
		partname = "CHUNK_1";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_CHUNKS_BRICK],skinname,partname);

}

/*QUAKED misc_generic_chunks_glass (1 .5 0) (-8 -7 -0.5) (8 7 0.5)   INVULNERABLE  x x x x x FLUFF
Glass shards used for debris
------ KEYS ------
style - type of chunk
0 - small triangular (default)
1 - medium triangular
2 - long thin
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_chunks_glass (edict_t *ent)
{
	char *partname;

	VectorSet (ent->mins, 0, -1, -1);
	VectorSet (ent->maxs, 1, 1, 1);

	switch (ent->style)
	{
	case 1:
		partname = "GLASS02";
		break;
	case 2:
		partname = "GLASS03";
		break;
	default:
		partname = "GLASS01";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_CHUNKS_GLASS],NULL,partname);
}

/*QUAKED misc_generic_chunks_metal (1 .5 0) (-5 -2 -1) (5 2 1)   INVULNERABLE   x x x x x FLUFF
Metal chunks used for debris.
------ KEYS ------
style - type of chunk
0 - wide and long piece (default)
1 - long thin piece
2 - smaller piece
3 - another small piece
skin - 
0 - dark
1 - rusty
2 - shiny
3 - light
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_chunks_metal (edict_t *ent)
{
	char *partname,*skinname;

	VectorSet (ent->mins, -4, -2, -4);
	VectorSet (ent->maxs,  4,  2,  4);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "rusty";
		break;
	case 2:
		skinname = "shiny";
		break;
	case 3:
		skinname = "light";
		break;
	default:
		skinname = "dark";
		break;
	}

	switch (ent->style)
	{
	case 1:
		partname = "METAL2";
		break;
	case 2:
		partname = "METAL3";
		break;
	case 3:
		partname = "METAL4";
		break;
	default:
		partname = "METAL1";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_CHUNKS_METAL],skinname,partname);
}

/*QUAKED misc_generic_chunks_rock (1 .5 0) (-5 -2 -1) (5 2 1)  INVULNERABLE   x x x x x FLUFF
Rock chunks used for debris.
------ KEYS ------
style - type of chunk
0 - big rock (default)
1 - medium rock
2 - small rock
3 - another small rock
skin - 
0 - brwn_dk
1 - brwn_lt
2 - grey_dk
3 - grey_lt
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_chunks_rock (edict_t *ent)
{
	char *partname,*skinname;

	VectorSet (ent->mins, 0, -1, -1);
	VectorSet (ent->maxs, 1, 1, 1);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "brwn_lt";
		break;
	case 2:
		skinname = "grey_dk";
		break;
	case 3:
		skinname = "grey_lt";
		break;
	default:
		skinname = "brwn_dk";
		break;
	}

	switch (ent->style)
	{
	case 1:
		partname = "ROCK2";
		break;
	case 2:
		partname = "ROCK3";
		break;
	case 3:
		partname = "ROCK4";
		break;
	default:
		partname = "ROCK1";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_CHUNKS_ROCK],skinname,partname);
}


/*QUAKED misc_generic_chunks_wall (1 .5 0) (-5 -6 -10) (5 8 10)  INVULNERABLE  x x x x x FLUFF
Wall chunks used as debris
------ KEYS ------
Skin :
0 - black (default)
1 - brown
2 - dark brown
3 - light brown
4 - grey
5 - dark grey
6 - light grey
7 - green
8 - orange cone
9 - red
10 - white
Style - type of chunk
0 - small (default)
1 - thin flat
2 - round chunk
3 - other round chunk
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_chunks_wall (edict_t *ent)
{
	char *skinname,*partname;

	VectorSet (ent->mins, -4, -2, -4);
	VectorSet (ent->maxs, 4, 2, 4);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "brown";
		ent->surfaceType = SURF_STONE_BLACK;
		break;
	case 2:
		skinname = "lbrown";
		ent->surfaceType = SURF_STONE_LGREY;
		break;
	case 3:
		skinname = "dbrown";
		ent->surfaceType = SURF_STONE_LBROWN;
		break;
	case 4:
		skinname = "grey";
		ent->surfaceType = SURF_STONE_LBROWN;
		break;
	case 5:
		skinname = "dgrey";
		ent->surfaceType = SURF_STONE_LGREY;
		break;
	case 6:
		skinname = "lgrey";
		ent->surfaceType = SURF_STONE_DGREY;
		break;
	case 7:
		skinname = "green";
		ent->surfaceType = SURF_STONE_DGREY;
		break;
	case 8:
		skinname = "orange_cone";
		ent->surfaceType = SURF_STONE_GREEN;
		break;
	case 9:
		skinname = "red";
		ent->surfaceType = SURF_STONE_RED;
		break;
	case 10:
		skinname = "white";
		ent->surfaceType = SURF_SAND_LBROWN;
		break;
	default:
		skinname = "black";
		ent->surfaceType = SURF_STONE_LGREY;
		break;
	}

	switch (ent->style)
	{
	case 1:
		partname = "_SMALLCHUNK";
		break;
	case 2:
		partname = "_MEDIUMCHUNK";
		break;
	case 3:
		partname = "_LARGECHUNK";
		break;
	default:
		partname = "_LITTLECHUNK";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_CHUNKS_WALL],skinname,partname);

}

/*QUAKED misc_generic_chunks_wood (1 .5 0) (-5 -2 -1) (5 2 1)   INVULNERABLE   x x x x x FLUFF
Wood chunks used for debris
------ KEYS ------
Style - type of chunk
0 - small piece(default)
1 - medium piece
2 - another small piece
3 - large piece
Skin :
0 - dark brown (default)
1 - dark grey
2 - light brown
3 - light grey
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_chunks_wood (edict_t *ent)
{
	char *partname,*skinname;

	VectorSet (ent->mins, 0, -1, -1);
	VectorSet (ent->maxs, 1, 1, 1);


	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "grey_dk";
		break;
	case 2:
		skinname = "brwn_lt";
		break;
	case 3:
		skinname = "grey_lt";
		break;
	default:
		partname = "brwn_dk";
		break;
	}

	switch (ent->style)
	{
	case 1:
		partname = "WOOD2";
		break;
	case 2:
		partname = "WOOD3";
		break;
	case 3:
		partname = "WOOD4";
		break;
	default:
		partname = "WOOD1";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_CHUNKS_WOOD],skinname,partname);
}

void comp_keyboard_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	Obj_painflip (ent,other,damage*FLIPMULT_COMP_KEYBOARD);
}

/*QUAKED misc_generic_comp_keyboard (1 .5 0) (-4 -12 -2) (4 12 2)   INVULNERABLE  NOPUSH  x x x x FLUFF
A keyboard to a computer.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_comp_keyboard (edict_t *ent)
{
	VectorSet (ent->mins, -4, -11, -2);
	VectorSet (ent->maxs, 4, 11, 2);

	SimpleModelInit2(ent,&genericModelData[OBJ_COMP_KEYBOARD],NULL,NULL);

	ent->pain = comp_keyboard_pain;
}


void comp_monitor_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t		fwd,up;
	vec3_t		holdPos;

	SetSkin(ent,genericModelData[OBJ_COMP_MONITOR].dir,genericModelData[OBJ_COMP_MONITOR].file, 
		 genericModelData[OBJ_COMP_MONITOR].materialfile,"damaged", 1);

	AngleVectors(ent->s.angles,fwd,NULL,up);

	VectorMA(ent->s.origin,10,fwd,holdPos);

	// kef -- alot of these fx are also being performed in MakeComputerGlassDamage(), which also
	//gets called when a comp_monitor is shot
	/*
	// Throw some sparks
	VectorSet(spray1, (rand()%21) - 10, (rand()%21) - 10, 0);
	FX_MakeSparks(holdPos, spray1, 1);
	VectorSet(spray1, (rand()%21) - 10, (rand()%21) - 10, 0);
	FX_MakeSparks(holdPos, spray1, 1);
	VectorSet(spray1, (rand()%41) - 20, (rand()%41) - 20, (irand(5,10)));
	FX_MakeSparks(holdPos, spray1, 1);

	// Throw some chunks
	FX_ThrowDebris(holdPos, 3,DEBRIS_SM, MAT_METAL_DGREY,0,0,0, SURF_METAL_COMPUTERS);
	FX_ThrowDebris(holdPos, 5,DEBRIS_SM, MAT_GLASS,0,0,0, SURF_GLASS_COMPUTER);
*/
	FX_LittleExplosion(holdPos, 100, 0);	

	fxRunner.execContinualEffect("environ/normalsmoke", ent);

	gi.sound (ent, CHAN_VOICE, gi.soundindex("Impact/Glassbreak/Monitor.wav"), .6, ATTN_NORM, 0);

	ent->pain = NULL;//tv_pain;

	ent->think = tv_damaged_think;
	ent->nextthink = level.time;

	ent->pain_debounce_time = level.time + gi.flrand(6,12);	// How long to smoke
	ent->s.sound =  gi.soundindex("Ambient/Gen/Electric/buzz.wav");
	ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
}


void comp_monitor_die (edict_t *ent, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	vec3_t	debrisNorm;

	VectorClear(debrisNorm);

	// Throw some chunks
	FX_ThrowDebris(ent->s.origin,debrisNorm, 7, DEBRIS_SM, ent->material,0,0,0, ent->surfaceType);
	tv_die(ent, inflictor, attacker, damage, point);
}

// POSSIBLY: Bring up Activision/Raven Logo when used

/*QUAKED misc_generic_comp_monitor (1 .5 0) (-9 -12 -12) (9 12 12)  INVULNERABLE  NOPUSH x x x x FLUFF
A computer and monitor.  
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- sparks and smokes when shot
*/
void SP_misc_generic_comp_monitor (edict_t *ent)
{
	VectorSet (ent->mins, -9, -12, -12);
	VectorSet (ent->maxs, 9, 12, 12);

	SimpleModelInit2(ent,&genericModelData[OBJ_COMP_MONITOR],NULL,NULL);

	SetSkin(ent,genericModelData[OBJ_COMP_MONITOR].dir,
		genericModelData[OBJ_COMP_MONITOR].file, 
		genericModelData[OBJ_COMP_MONITOR].materialfile,"monitor", 1);

	ent->pain = comp_monitor_pain;
	ent->die = comp_monitor_die;

	// cache sound CLSFX_EXP_SMALL
	entSoundsToCache[CLSFX_EXP_SMALL] = 1;

	gi.soundindex("Impact/Glassbreak/Monitor.wav"); // precache sound
	gi.soundindex("Ambient/Gen/Electric/buzz.wav"); // precache sound
	gi.effectindex("environ/normalsmoke");
}

// FIXME : effect is positioned right and the damage skin on the monitor isn't appearing
void comp_monitor2_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t		fwd,right,up;
	vec3_t		holdPos;

	SetSkin(ent,genericModelData[OBJ_COMP_MONITOR2].dir,genericModelData[OBJ_COMP_MONITOR2].file, 
		 genericModelData[OBJ_COMP_MONITOR2].materialfile,"monitor2_d", 1);

	AngleVectors(ent->s.angles,fwd,right,up);

	VectorMA(ent->s.origin,14,fwd,holdPos);

	FX_LittleExplosion(holdPos, 100, 0);	

	fxRunner.execContinualEffect("environ/normalsmoke", ent);

	gi.sound (ent, CHAN_VOICE, gi.soundindex("Impact/Glassbreak/Monitor.wav"), .6, ATTN_NORM, 0);

	ent->pain = NULL;//tv_pain;

	ent->think = tv_damaged_think;
	ent->nextthink = level.time;

	ent->pain_debounce_time = level.time + gi.flrand(6,12);	// How long to smoke
	ent->s.sound =  gi.soundindex("Ambient/Gen/Electric/buzz.wav");
	ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
}


/*QUAKED misc_generic_comp_monitor2 (1 .5 0) (-34 -15 -8) (9 7 15)  INVULNERABLE  NOPUSH x x x x FLUFF
A high tech computer with a large monitor.
------ KEYS ------
skin - 
0 - clean
1 - damaged
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- sparks and smoke when shot
- switches to damage skin when shot
*/
void SP_misc_generic_comp_monitor2 (edict_t *ent)
{
	VectorSet (ent->mins, -9, -12, -12);
	VectorSet (ent->maxs, 9, 12, 12);

	SimpleModelInit2(ent,&genericModelData[OBJ_COMP_MONITOR2_TOWER],NULL,NULL);

	SimpleModelAddBolt(ent,genericModelData[OBJ_COMP_MONITOR2_TOWER],"DUMMY01",
						genericModelData[OBJ_COMP_MONITOR2],"DUMMY01",NULL);

	if (ent->s.skinnum)
	{
		SetSkin(ent,genericModelData[OBJ_COMP_MONITOR2_TOWER].dir,
			genericModelData[OBJ_COMP_MONITOR2_TOWER].file, 
			genericModelData[OBJ_COMP_MONITOR2_TOWER].materialfile,"monitor2_d", 1);
	}

	ent->pain = comp_monitor2_pain;
//	ent->die = comp_monitor_die;

	// cache sound CLSFX_EXP_SMALL
	entSoundsToCache[CLSFX_EXP_SMALL] = 1;
	gi.effectindex("environ/normalsmoke");
	gi.soundindex("Ambient/Gen/Electric/buzz.wav");
}


/*QUAKED misc_generic_crate_fish (1 .5 0)  (-10 -8 -1) (11 8 8) INVULNERABLE  NOPUSH x x x x FLUFF
A crate for holding fish. Great for the docks.
------ SPAWNFLAGS ------
INVULNERABLE - can't be hurt
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_crate_fish (edict_t *ent)
{
	VectorSet (ent->mins, -10, -7,-1);
	VectorSet (ent->maxs,  10,  7, 8);

	SimpleModelInit2(ent,&genericModelData[OBJ_CRATE_FISH],NULL,NULL);
}


void desk_phone_effects (edict_t *ent)
{
	vec3_t		up = { 0,0,1};
	qboolean		bSmoking = fxRunner.hasEffect("environ/normalsmoke", ent);

	if (ent->pain_debounce_time <= level.time && bSmoking)	// Turn off the smoke and sparks?
	{
		fxRunner.stopContinualEffect("environ/normalsmoke", ent);
		ent->think = NULL;
		ent->nextthink = 0;
		ent->s.sound = 0;

		return;
	}

/*	if ((rand() % 20) < 5)
	{
		FX_MakeSparks(ent->s.origin, up, 2);
	}
*/
	ent->think = desk_phone_effects;
	ent->nextthink = level.time + .2;
}

void desk_phone_pain2 (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	FX_PhoneHit(ent->s.origin);
	Obj_painflip (ent,other,damage*FLIPMULT_DESK_PHONE);
}

void desk_phone_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t		up;

	Obj_painflip (ent,other,damage*FLIPMULT_DESK_PHONE);

	FX_LittleExplosion(ent->s.origin, 10, 0);	
	fxRunner.execContinualEffect("environ/normalsmoke", ent);
	AngleVectors(ent->s.angles,NULL,NULL,up);

	FX_MakeSparks(ent->s.origin, up, 2);
	FX_PhoneHit(ent->s.origin);

	ent->think = desk_phone_effects;
	ent->nextthink = level.time + .1;

	ent->pain_debounce_time = level.time + gi.flrand(.5,1.5);

	ent->pain = desk_phone_pain2;
}

/*QUAKED misc_generic_desk_phone (1 .5 0) (-5 -7 -3) (5 6 3)   INVULNERABLE  NOPUSH x x x x FLUFF
A desk phone.
--------KEYS------------
Skin :
 0 - clean (default)
 1 - dirty yucky phone
--------SPAWNFLAGS----------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- light flashes
- spins and sparks when shot
*/
void SP_misc_generic_desk_phone (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -8, -10, -5);
	VectorSet (ent->maxs, 8, 10, 5);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "phone2";
		break;
	default:
		skinname = "phone";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_DESK_PHONE],skinname,NULL);

	ent->pain = desk_phone_pain;


	// cache sound CLSFX_EXP_SMALL, CLSFX_PHONE_HIT
	entSoundsToCache[CLSFX_EXP_SMALL] = 1;
	entSoundsToCache[CLSFX_PHONE_HIT] = 1;
	gi.effectindex("environ/normalsmoke");
}

void fan_big_accelerate(edict_t *ent)
{
	vec3_t		angVel = {0,0,0};

	if (ent->count == 2)
	{	// accelerate
		if ((ent->speed - ent->elasticity) < ent->volume)
		{	// close enough. stop accelerating.
			ent->count = 1;
			angVel[YAW] = ent->speed;
			ent->elasticity = ent->speed;
			ent->think = NULL;
		}
		else
		{	// keep accelerating.
			ent->elasticity += ent->volume;
			angVel[YAW] = ent->elasticity;
			ent->nextthink = level.time + FRAMETIME;
		}
		FXA_SetRotate(ent, angVel, ent->ghoulInst);
	}
	else
	{	// decelerate
		if ((ent->elasticity) < ent->volume)
		{	// close enough. stop decelerating.
			ent->count = 0;
			angVel[YAW] = 0;
			ent->think = NULL;
			ent->elasticity = 0;
		}
		else
		{	// keep decelerating.
			ent->elasticity -= ent->volume;
			angVel[YAW] = ent->elasticity;
			ent->nextthink = level.time + FRAMETIME;
		}
		FXA_SetRotate(ent, angVel, ent->ghoulInst);
	}
}

void fan_big_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	vec3_t		angVel = {0,0,0};

	angVel[YAW] = ent->speed;

	if (ent->count)	// Stop moving
	{
		//angVel[YAW] = 0;
		//ent->count = 0;
		ent->count = -2;
		ent->think = fan_big_accelerate;
		ent->nextthink = level.time + FRAMETIME;
		FXA_SetRotate(ent, angVel, ent->ghoulInst);
	}
	else			// Rotate, damn you!
	{
		//angVel[YAW] = ent->speed;
		//ent->count = 1;
		ent->count = 2;
		ent->think = fan_big_accelerate;
		ent->nextthink = level.time + FRAMETIME;
	}

}

/*QUAKED misc_generic_fan_big  (1 .5 0) (-64 -40 -63) (63 0 63)   INVULNERABLE  NOPUSH START_OFF x x x FLUFF
A really, really, big fan
------ KEYS ------
speed - how fast fan spins (default 1.5) **must be less than 31!!**
volume - how fast fan accelerates or decelerates (default 0.05)
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A, can't ever be pushed
AUTOMATE - spins
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- When targeted it will turn off or on.
*/
void SP_misc_generic_fan_big (edict_t *ent)
{
	vec3_t		angVel = {0,0,0};

	VectorSet (ent->mins, -64, -40, -63);
	VectorSet (ent->maxs,  63,   0,  63);

	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&genericModelData[OBJ_FAN_BIG],NULL,NULL);
	ent->count = 0;
	ent->elasticity = 0;
	if (!ent->volume)
	{
		ent->volume = 0.05;
	}
	ent->use = fan_big_use;

	// set the fan to rotate
	if (!(ent->spawnflags & FAN_BIG_START_OFF))
	{
//		angVel[YAW] = 1.5f;
		if (!ent->speed)
			ent->speed = 1.5f;

		angVel[YAW] = ent->speed;

		ent->count = 1;
		ent->elasticity = ent->speed;
		FXA_SetRotate(ent, angVel, ent->ghoulInst);
	}
	if (ent->speed > 31)
	{	// must be capped due to FXA_SetRotate()
		ent->speed = 31;
	}
}

/*QUAKED misc_generic_faucet  (1 .5 0) (-7 -6 -6) (12 10 6)   INVULNERABLE  NOPUSH x x x x FLUFF
A sink faucet.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A, can't ever be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_faucet (edict_t *ent)
{
	VectorSet (ent->mins, -6, -5, -5);
	VectorSet (ent->maxs, 11,  9,  5);

	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&genericModelData[OBJ_FAUCET],NULL,NULL);
}

/******************************************************************************/
// Fire Extinguisher Code
/******************************************************************************/
void fire_extinguisher_dying (edict_t *self);
void fire_extinguisher_explode (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void fire_extinguisher_boom (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);

void extinguisher_use (edict_t *self, edict_t *other, edict_t *activator)
{
	vec3_t	vel;

	self->health = 100 + (rand()%10);
	VectorSubtract(activator->s.origin, self->s.origin, vel);
	vel[2] *= .2;// downplay the vertical component
	VectorNormalize(vel);

	self->s.sound = gi.soundindex("Impact/Exting/ExtingLeak.wav");
	self->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;

	self->think = fire_extinguisher_dying;
	self->nextthink = level.time + .1;
	self->pain = fire_extinguisher_explode;
	self->die = fire_extinguisher_boom;
}

void fire_extinguisher_remove (edict_t *self)
{
	ObjBecomeDebris(self, self, self, 1000, vec3_origin);
	G_FreeEdict(self);
}

void fire_extinguisher_explode (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	T_RadiusDamage (self, other, self->health, self, 60 + (self->health * .5), MOD_EXPLOSIVE);
	ShakeCameras (self->s.origin, 60, 900, DEFAULT_JITTER_DELTA);
	self->s.sound = 0;
	// because the effect attaches to the wall in front of the entity, turn the entity 
	//backward prior to performing the effect
	self->s.angles[YAW] += 180;
	anglemod(self->s.angles[YAW]);
//	fxRunner.exec("environ/extinguisherdeath", self);
	self->think = fire_extinguisher_remove;
	self->nextthink = level.time + .1;
}

void fire_extinguisher_dying (edict_t *self)
{
	self->health--;
	self->nextthink = level.time + .1;

	if(self->health < 1)
	{
		self->think = NULL;
		self->nextthink = 0;
		self->die = BecomeDebris;
		self->takedamage = DAMAGE_YES;
		self->pain = NULL;
		self->health = 1;
		self->s.sound = 0;
	}
}

void fire_extinguisher_boom (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	fire_extinguisher_explode (self, inflictor, 0, damage, point);
}

void fire_extinguisher_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	SetSkin(self,genericModelData[OBJ_FIRE_EXTINGUISHER].dir,
		genericModelData[OBJ_FIRE_EXTINGUISHER].file, 
		genericModelData[OBJ_FIRE_EXTINGUISHER].materialfile,"damaged", 0);

	self->health = 50;

	self->s.sound = gi.soundindex("Impact/Exting/ExtingLeak.wav");
	self->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;

	fxRunner.exec("environ/extinguisher1", self);
	self->think = fire_extinguisher_dying;
	self->nextthink = level.time + .1;
	self->pain = fire_extinguisher_explode;
	self->die = fire_extinguisher_boom;
}

/*QUAKED misc_generic_fire_extinguisher (1 .5 0) (-4 -7 -13) (4 7 13)  INVULNERABLE  NOPUSH x x x x FLUFF
A wall mounted fire extinguisher.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A, can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- spews foam when shot
- explodes if shot again
- when 'used' spews foam
*/
void SP_misc_generic_fire_extinguisher (edict_t *ent)
{
	VectorSet (ent->mins, -2, -6, -12);
	VectorSet (ent->maxs, 4, 8, 12);

	ent->spawnflags |= SF_NOPUSH;

	ent->die = fire_extinguisher_die;
	ent->use = extinguisher_use;	// Use for trigger_uses
	ent->plUse = extinguisher_use;	// Use for when player pushes 'use' key

	SimpleModelInit2(ent,&genericModelData[OBJ_FIRE_EXTINGUISHER],NULL,NULL);

	SetSkin(ent,genericModelData[OBJ_FIRE_EXTINGUISHER].dir,
		genericModelData[OBJ_FIRE_EXTINGUISHER].file, 
		genericModelData[OBJ_FIRE_EXTINGUISHER].materialfile,"exting", 0);

	gi.soundindex("Impact/Exting/ExtingLeak.wav");// precache sound
	gi.effectindex("environ/extinguisher1");
//	gi.effectindex("environ/extinguisherdeath");
//	SetForRespawn(ent, SP_misc_generic_fire_extinguisher, 30.0);// Keep - might be used in death match
}


/******************************************************************************/
// Fire Hydrant Code
/******************************************************************************/

void hydrant_pulse (edict_t *self)
{
	self->nextthink = level.time + (gi.irand(1,26) * .1) + .5;

}

void hydrant_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	edict_t			*hydrant,*water;
	vec3_t			debrisNorm;
	vec3_t up;


	VectorClear(debrisNorm);

	FX_ThrowDebris(self->s.origin,debrisNorm, 5,DEBRIS_SM, 0, 0,0,0, 0);

	AngleVectors(self->s.angles, 0, 0, up);

	fxRunner.exec("environ/toiletspout", self->s.origin);

	self->think = G_FreeEdict;
	self->nextthink = level.time + 1;


	return;




	water = G_Spawn();
	VectorCopy(self->s.origin,water->s.origin);
	water->solid = SOLID_NOT;
	water->takedamage = DAMAGE_NO;
	water->health = 1;
	water->think = hydrant_pulse;
	water->nextthink = level.time + gi.flrand (.3F, 3.0F);
	gi.linkentity (water);

	water->s.sound =  gi.soundindex("Ambient/Gen/WtrFX/SprayBig.wav");
	water->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;

	FX_MakeDecalBelow(self->s.origin, FXDECAL_PIPELEFT, 0); // pipe

	// Create a hydrant to throw
	hydrant = G_Spawn();
	hydrant->movetype = MOVETYPE_DAN;
	hydrant->solid = SOLID_BBOX;
	hydrant->clipmask = MASK_MONSTERSOLID|MASK_PLAYERSOLID;
	VectorSet (hydrant->mins, -8, -8, -8);
	VectorSet (hydrant->maxs, 8, 8, 8);
	VectorSet (hydrant->velocity, gi.irand(-51,50), gi.irand(-51,50), gi.irand(450,650));

	// So it doesn't fall back on top of the water spout
	if ((hydrant->velocity[0] > -5) && (hydrant->velocity[0] < 5))
		hydrant->velocity[0] = gi.irand(10,50);

	// So it doesn't fall back on top of the water spout
	if ((hydrant->velocity[1] > -5) && (hydrant->velocity[1] < 5))
		hydrant->velocity[1] = gi.irand(10,50);

	VectorSet (hydrant->avelocity, gi.irand(-51,50), gi.irand(-51,50), gi.irand(-51,50));

	game_ghoul.SetSimpleGhoulModel (hydrant,"objects/generic/fire_hydrant","fire_hydrant");

	hydrant->takedamage = DAMAGE_YES;
	hydrant->surfaceType = SURF_METAL;
	hydrant->health = 500;
	hydrant->die = BecomeDebris;
	VectorCopy(self->s.origin, hydrant->s.origin);
	gi.linkentity (hydrant);

	G_FreeEdict (self);	// Get rid of the old hydrant

}

/*QUAKED misc_generic_fire_hydrant (1 .5 0) (-8 -8 -10) (8 8 14) INVULNERABLE   NOPUSH x x x x FLUFF
A red fire hydrant. 
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A, can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
-  flies into air and a jet of water shoots out of ground when it dies
*/
void SP_misc_generic_fire_hydrant (edict_t *ent)
{
	VectorSet (ent->mins, -8, -8, -10);
	VectorSet (ent->maxs, 8, 8, 14);

	ent->spawnflags |= SF_NOPUSH;

	if (ent->spawnflags & SF_INVULNERABLE)
	{
		ent->takedamage = DAMAGE_NO;
	}
	else
	{
		ent->takedamage = DAMAGE_YES;
		ent->die = hydrant_die;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_FIRE_HYDRANT],NULL,NULL);

	gi.soundindex("Ambient/Gen/WtrFX/SprayBig.wav"); // precache sound
}

void fish_think(edict_t *self)
{
	// fish swims between self->pos1 and self->pos2
	vec3_t vTemp = {0,0,0}, fwd;

	if (self->attenuation)
	{
		VectorSubtract(self->s.origin, self->pos2, vTemp);
	}
	else
	{
		VectorSubtract(self->s.origin, self->pos1, vTemp);
	}
	// are we near our next patrol point?
	if (100 > VectorLengthSquared(vTemp))
	{
		self->attenuation = !self->attenuation;
		VectorNegate(self->velocity, self->velocity);
		AngleVectors(self->s.angles, fwd, NULL, NULL);
		VectorNegate(fwd, fwd);
		vectoangles(fwd, self->s.angles);
	}
	gi.linkentity(self);
	self->think = fish_think;
	self->nextthink = level.time + 0.1f;
}

void fish_sink(edict_t *self)
{
	// lower his patrol route towards the bottom of the tank
	if ( (self->s.origin[2] + self->mins[2]) > (self->volume+3))
	{
		self->s.origin[2] -= 1;
	}


	// need some sort of check for tank emptiness


	gi.linkentity(self);
	self->think = fish_sink;
	self->nextthink = level.time + 0.1f;
}

void fish_die(edict_t *self, edict_t *other, edict_t *activator)
{
	// actually a "use" function...gets called when the fishy's tank empties.

	// speed up the swimming (fishy is getting anxious)
	VectorScale(self->velocity, 2, self->velocity);
	self->think = fish_sink;
	self->nextthink = level.time + 0.1f;
}

/*QUAKED misc_generic_fish (1 .5 0) (-18 -6 -8) (18 6 8)  INVULNERABLE   NOPUSH x x x x FLUFF
A large aquarium fish. 
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ KEYS ------
Skin :
0 - white with red spots (default)
1 - gold
Angles :
- holds the second point in patrol route. If not set, fish don't move.
Volume :
- height of floor of tank
------ SPECIALS ------
Patrols between its origin and a second point stored in angles.
*/
void SP_misc_generic_fish (edict_t *ent)
{
	char *skinname;
	vec3_t vel;

	VectorSet (ent->mins, -17, -6, -7);
	VectorSet (ent->maxs,  17,  6,  7);

	VectorCopy(ent->s.origin, ent->pos1);
	VectorCopy(ent->s.angles, ent->pos2);

	if (ent->pos2[0] || ent->pos2[1] || ent->pos2[2])
	{
		// use ent->attenuation to mark which patrol point we're aiming for (0 == origin)
		VectorSubtract(ent->pos2, ent->pos1, vel);
		VectorNormalize(vel);
		vectoangles(vel, ent->s.angles);
		VectorScale(vel, 20, ent->velocity);
		ent->attenuation = 1;
		ent->think = fish_think;
		ent->use = fish_die;
		ent->nextthink = level.time + 0.1f;
	}

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "yellow";
		break;
	default:
		skinname = "white";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_FISH],skinname,NULL);
	SimpleModelSetSequence2(ent->ghoulInst,genericModelData[OBJ_FISH].file,SMSEQ_LOOP);

	ent->flags |= FL_NO_KNOCKBACK;
	ent->movetype = MOVETYPE_FLY;	// Can be pushed around
}

/*QUAKED misc_generic_fish_dead (1 .5 0) (-18 -4 -8) (18 3 8)  INVULNERABLE   NOPUSH x x x x FLUFF
A big dead fish laying on it's side.
------ KEYS ------
Skin :
0 - white with red spots (default)
1 - gold
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_fish_dead (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -17, -6, -1);
	VectorSet (ent->maxs,  17,  6,  7);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "yellow";
		break;
	default:
		skinname = "white";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_FISH_DEAD],skinname,NULL);

	ent->s.angles[ROLL]+= 90;

}


void flag_pole_sequence (edict_t *ent)
{
	ggObjC *simpObj;
	GhoulID simpSeq;
	ggOinstC *myInstance;

	if (!ent->ghoulInst)
	{
		return;
	}

	simpObj = game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject());
	if (!simpObj)
	{
		return;
	}

	//changed this to false, can't cache new seqs in after instances are created --ss
	simpSeq = simpObj->FindSequence(genericModelData[OBJ_FLAG_POLE].file);

	if (!simpSeq)
	{
		return;
	}

	myInstance = simpObj->FindOInst(ent->ghoulInst);
	if (!myInstance)
	{
		return;
	}

	// Play sequence
	myInstance->PlaySequence(simpSeq, level.time);

}



/*QUAKED misc_generic_flag_pole (1 .5 0) (-23 -66 -418) (17 39 37)  INVULNERABLE   NOPUSH x x x x FLUFF
A flag on a tall pole.
------ KEYS ------
Skin :
0 - ??? (default)
1 - USA
2 - ??? 
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A can't ever be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_flag_pole (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -4, -65, -417);
	VectorSet (ent->maxs,  4,  38,   36);

	ent->spawnflags |= SF_NOPUSH;

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "flag_pole2";
		break;
	case 2:
		skinname = "flag_pole3";
		break;
	default:
		skinname = "flag_pole";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_FLAG_POLE],skinname,NULL);

	ent->ghoulInst->Pause(level.time);

	ent->think = flag_pole_sequence;
	ent->nextthink = level.time + ((rand() % 5) * .1);

}

void forklift_use(edict_t *self, edict_t *other, edict_t *activator)
{
	self->solid = SOLID_BBOX;
	// he should be visible
	if (self->ghoulInst)
	{
		self->ghoulInst->SetOnOff(true, level.time);
	}
}

/*QUAKED misc_generic_forklift (1 .5 0) (-74 -55 -54) (92 5 53)  INVULNERABLE   NOPUSH TRIGGER_SPAWN x x x FLUFF
A forklift.
------ KEYS ------
Skin - 
0 - blue/grey
1 - yellow
2 - green
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
TRIGGER_SPAWN - use it and it shows up
FLUFF - won't show if gl_pictip is set
*/

void SP_misc_generic_forklift (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -73, -54, -53);
	VectorSet (ent->maxs,  91,   4,  52);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "forklift2";
		break;
	case 2:
		skinname = "forklift3";
		break;
	default:
		skinname = "forklift";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_FORKLIFT],skinname,NULL);

	ent->ghoulInst->Pause(level.time);
	if (ent->spawnflags & FORKLIFT_TRIGGER_SPAWN)
	{
		ent->solid = SOLID_NOT;
		// he shouldn't even be visible
		if (ent->ghoulInst)
		{
			ent->ghoulInst->SetOnOff(false, level.time);
		}
		ent->use = forklift_use;
	}
}


void garbage_bag_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	// create a cloud of papers when this object is hit
	FX_PaperCloud(ent->s.origin, 3);
	Obj_painroll (ent,other,damage,0);
}

/*QUAKED misc_generic_garbage_bag (1 .5 0) (-10 -9 -9) (10 10 12)  INVULNERABLE  NOPUSH x x x x FLUFF
A black plastic bag of garbage.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- rolls around when shot
- spews paper when shot
*/
// POSSIBLY : have rats be spawned when this dies
void SP_misc_generic_garbage_bag (edict_t *ent)
{
	VectorSet (ent->mins, -9, -10, -9);
	VectorSet (ent->maxs, 9, 10, 10);

	ent->pain = garbage_bag_pain;

	ent->die = SpecialBecomeDebris;
	SimpleModelInit2(ent,&genericModelData[OBJ_GARBAGE_BAG],NULL,NULL);

	// cache debris CLGHL_PAPER, CLGHL_CAN
	entDebrisToCache[CLGHL_PAPER] = DEBRIS_YES;
	entDebrisToCache[CLGHL_CAN] = DEBRIS_YES;
}





void AnglesFromDir(vec3_t direction, vec3_t angles);
void auto_gun_target (edict_t *self);
// this one's for guns with a tripod base
void auto_gun_target2(edict_t *self);

qboolean TargetWithinArc(vec3_t targetPos, vec3_t selfPos, float origAngle, float arc) 
{
	vec3_t		toTarget, baseVector;
	float		angleBetween;

	VectorSubtract(targetPos, selfPos, toTarget);
	toTarget[2] = 0;
	VectorNormalize(toTarget);
	VectorSet(baseVector, cos(DEGTORAD * origAngle), sin(DEGTORAD * origAngle), 0);
	angleBetween = fabs(acos(DotProduct(toTarget, baseVector))) * RADTODEG;
//	Com_Printf("Angle between is %f\n", angleBetween);
	if (angleBetween > arc){ return false;}
	return true;
}

void auto_gun_target (edict_t *self)
{
	vec3_t		targetDir;
	edict_t		*target = NULL;
	trace_t		tr;

	self->nextthink = level.time + FRAMETIME;

	CRadiusContent rad(self->s.origin, self->volume);

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		target = rad.foundEdict(i);

		if(target->client && !(target->flags & FL_NOTARGET))
		//  right now, we're only targeting the player.  should make this switchable by designers
		{
			// trace to see if I can see player.
			gi.trace (self->s.origin, NULL, NULL, target->s.origin, self, MASK_SHOT| MASK_WATER, &tr);
			if ((tr.ent->client) && (tr.ent->health > 0))
			{	// can see the player

				// if I couldn't see the player before, play my "I just woke up" sound
				if (NULL == self->enemy)
				{
					gi.sound (self, CHAN_VOICE, gi.soundindex("Enemy/MGun/wake.wav"), .6, ATTN_NORM, 0);
				}
				self->enemy = target;
				self->nextthink = level.time + .1;
				self->think = auto_gun_target;
			}
			else
			{
				self->enemy = NULL;
				return;
			}
		}	
	}
	if (NULL == self->enemy)
	{
		return;
	}
	if (self->enemy->flags & FL_NOTARGET)
	{
		return;
	}
	VectorSubtract (self->enemy->s.origin, self->s.origin, targetDir);

	// is target within range?
	if (VectorLengthSquared(targetDir) > (self->volume*self->volume))
	{	// not in range
		self->avelocity[1] = 0;
		//  perhaps return to original position?
		if (self->s.sound == gi.soundindex("Enemy/MGun/trnslow.wav"))
			{
				self->s.sound =  gi.soundindex("Enemy/MGun/trnstop.wav");
				self->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
			}
		self->nextthink = level.time +  .1;
		return;
	}
	// guns now use ghoul rotation stuff
	auto_gun_target2(self);
	return;
}

void auto_gun_target2(edict_t *self)
{
	IGhoulInst*					gunInst = SimpleModelGetBolt(self, 1),
								*baseInst = NULL;
	GhoulID						gunBolt;
	boltonOrientation_c			boltonInfo;
	vec3_t						fwd, vToTarget, barrel1Pos, barrel2Pos, vPos;
	trace_t						tr;
	boltonOrientation_c::retCode_e ret = boltonOrientation_c::retCode_e::ret_TRUE;

	self->nextthink = level.time + FRAMETIME;

	if (gunInst && gunInst->GetGhoulObject())
	{
		gunBolt = gunInst->GetGhoulObject()->FindPart("DUMMY01");
	}
	if (!gunBolt || !self->enemy)
	{
		return;
	}

	// turn the turret to face the player 
	//
	// the tripod version can pitch as well as yaw. with the hanging version, the entity
	//(arm attached to ceiling) yaws while the gun pitchs
	if (!gunInst->GetParent() || !gunInst->GetParent()->GetGhoulObject())
	{
		return;
	}
	boltonInfo.root = self;
	if (self->spawnflags & AUTOGUN_NO_TRIPOD)
	{
		// no tripod, yaw the entity
		boltonInfo.boltonInst = self->ghoulInst;
		boltonInfo.parentInst = NULL;
		VectorCopy(self->enemy->s.origin, boltonInfo.vTarget);
		boltonInfo.fMinPitch = 0;
		boltonInfo.fMaxPitch = 0;
		boltonInfo.fMinYaw = -self->elasticity;
		boltonInfo.fMaxYaw = self->elasticity;
		boltonInfo.fMaxTurnSpeed = self->speed;
		boltonInfo.bUsePitch = false;
		boltonInfo.bUseYaw = true;
		boltonInfo.bToRoot = true;
		ret = boltonInfo.OrientEnt();
		// pitch the gun
		if (ret == boltonOrientation_c::retCode_e::ret_TRUE)
		{
			boltonInfo.boltonInst = gunInst;
			boltonInfo.boltonID = gunBolt;
			boltonInfo.parentInst = gunInst->GetParent();
			boltonInfo.parentID = gunInst->GetParent()->GetGhoulObject()->FindPart("DUMMY02");
			VectorCopy(self->enemy->s.origin, boltonInfo.vTarget);
			boltonInfo.fMinPitch = -self->decel;
			boltonInfo.fMaxPitch = self->accel;
			boltonInfo.fMinYaw = 0;
			boltonInfo.fMaxYaw = 0;
			boltonInfo.fMaxTurnSpeed = self->speed;
			boltonInfo.bUsePitch = true;
			boltonInfo.bUseYaw = false;
			boltonInfo.bToRoot = true;
			ret = boltonInfo.OrientBolton();
		}
	}
	else
	{
		// we've got a tripod, so yaw the ent->bolt1 (a null)
		baseInst = SimpleModelGetBolt(self, 1);
		if (!baseInst || !baseInst->GetGhoulObject())
		{
			return;
		}
		boltonInfo.boltonInst = baseInst;
		boltonInfo.boltonID = baseInst->GetGhoulObject()->FindPart("DUMMY01");
		boltonInfo.parentInst = self->ghoulInst;
		VectorCopy(self->enemy->s.origin, boltonInfo.vTarget);
		boltonInfo.fMinPitch = 0;
		boltonInfo.fMaxPitch = 0;
		boltonInfo.fMinYaw = -self->elasticity;
		boltonInfo.fMaxYaw = self->elasticity;
		boltonInfo.fMaxTurnSpeed = self->speed;
		boltonInfo.bUsePitch = false;
		boltonInfo.bUseYaw = true;
		boltonInfo.bToRoot = true;
		ret = boltonInfo.OrientBolton();
		// pitch the gun
		gunInst = SimpleModelGetBolt(self, 2);
		if (NULL == gunInst)
		{
			return;
		}
		if (ret == boltonOrientation_c::retCode_e::ret_TRUE)
		{
			if (gunInst->GetGhoulObject())
			{
				gunBolt = gunInst->GetGhoulObject()->FindPart("DUMMY01");
			}
			else
			{
				return;
			}
			boltonInfo.parentInst = boltonInfo.boltonInst;
			boltonInfo.parentID = boltonInfo.boltonID;
			boltonInfo.boltonInst = gunInst;
			boltonInfo.boltonID = gunBolt;
			VectorCopy(self->enemy->s.origin, boltonInfo.vTarget);
			boltonInfo.fMinPitch = -self->decel;
			boltonInfo.fMaxPitch = self->accel;
			boltonInfo.fMinYaw = 0;
			boltonInfo.fMaxYaw = 0;
			boltonInfo.fMaxTurnSpeed = self->speed;
			boltonInfo.bUsePitch = true;
			boltonInfo.bUseYaw = false;
			boltonInfo.bToRoot = true;
			ret = boltonInfo.OrientBolton();
		}
	}

	// if we're now pointing at our target, uh, SHOOT IT!!!
	VectorSubtract(self->enemy->s.origin, self->s.origin, vToTarget);
	GetGhoulPosDir(self->s.origin, self->s.angles, gunInst, gunBolt, "DUMMY01", NULL, NULL, NULL, fwd);
	VectorNormalize(vToTarget); // ouch
	if (DotProduct(vToTarget, fwd) > 0.9f)
	{
		bool bBurstFire = false;
		// pointing at our target
		if (self->s.sound == gi.soundindex("Enemy/MGun/trnslow.wav"))
		{
			self->s.sound = 0;
			gi.sound (self, CHAN_VOICE, gi.soundindex("Enemy/MGun/trnstop.wav"), .6, ATTN_NORM, 0);
		}
		// fire in tri-burst mode, cuz constant fire is too deadly
		baseObjInfo_c	*pObjInfo = self->objInfo;
		objFlipInfo_c		*pFlipInfo = NULL;
		// unbelievable hack, using the entity's objFlipInfo_c struct like this...
		//
		// radius -------------	number of shots in a burst			(3)	
		// startTime ----------	number of seconds between bursts	(.5)
		// ground -------------	number of bursts between reloads	(5)
		// initRadiusAng ------	reload time (in seconds)			(3)
		//
		if (pObjInfo && (pFlipInfo = (objFlipInfo_c*)pObjInfo->GetInfo(OIT_FLIP)))
		{
			if ( (level.time > pFlipInfo->initRadiusAng) && 
				 (level.time > pFlipInfo->startTime) )
			{
				// fire a burst
				if (pFlipInfo->radius < 3) // 3-shot burst
				{
					bBurstFire = true;
					pFlipInfo->radius++;
				}
				else
				{
					// ended our burst
					pFlipInfo->radius = 0;
					if (pFlipInfo->ground < 5) // 5 bursts before reload
					{
						pFlipInfo->ground++;
						pFlipInfo->startTime = level.time + .5; // .5 seconds between bursts
					}
					else
					{
						// begin reload 
						pFlipInfo->ground = 0;

						// play a "reload" sound here?

						// make sure we can fire our next burst immediately after reload
						pFlipInfo->startTime = level.time;
						pFlipInfo->initRadiusAng = level.time + 3.0; // 3-second reload time
					}
				}
			}
		}
		if (!bBurstFire)
		{
			return;
		}
		// try tracing to the player's head first (20 units above player's origin)
		VectorSet(vPos, self->enemy->s.origin[0], self->enemy->s.origin[1], self->enemy->s.origin[2] + 20);
		gi.trace (self->s.origin, NULL, NULL, vPos, self, MASK_SHOT| MASK_WATER, &tr);
		if ( !(tr.ent && tr.ent->client) )
		{
			// couldn't trace to the player's head, try his origin
			gi.trace (self->s.origin, NULL, NULL, self->enemy->s.origin, self, MASK_SHOT| MASK_WATER, &tr);
		}
		if (tr.ent && tr.ent->client)
		{	
			self->s.sound = 0;
			// gotta call this once for each barrel. don't send the gunInst cuz that would generate
			//muzzle flashes, which we're doing later on via fxRunner
			GetGhoulPosDir(self->s.origin, self->s.angles, gunInst, 0, "_LEFTFLASH", barrel1Pos, NULL, NULL, NULL);
			GetGhoulPosDir(self->s.origin, self->s.angles, gunInst, 0, "_FLASHRIGHT", barrel2Pos, NULL, NULL, NULL);
			weapons.attack(ATK_AUTOGUN, self, barrel1Pos, fwd);
			weapons.attack(ATK_AUTOGUN, self, barrel2Pos, fwd);

			assert(gunInst->GetGhoulObject());
			GhoulID leftBarrel = gunInst->GetGhoulObject()->FindPart("_LEFTFLASH");
			GhoulID rightBarrel = gunInst->GetGhoulObject()->FindPart("_FLASHRIGHT");

			if(leftBarrel)
			{
				fxRunner.execWithInst("weapons/othermz/autogun", self, gunInst, leftBarrel, MULTICAST_PHS, 1);
			}
			if(rightBarrel)
			{
				fxRunner.execWithInst("weapons/othermz/autogun", self, gunInst, rightBarrel, MULTICAST_PHS, 1);
			}

			if (true || 1 == pFlipInfo->radius)
			{	// this is the first shot of a burst, so play the firing sound
				gi.sound(self, CHAN_VOICE, gi.soundindex("Weapons/Autoshot/fire.wav"), .9, ATTN_NORM, 0);
			}
		}
		else
		{
			self->s.sound = 0;
			self->nextthink = level.time + .1;
		}
	}
	else
	{
		// since we're not shooting, if we turned during this frame play the turning sound
		if (ret == boltonOrientation_c::retCode_e::ret_FALSE)
		{
			self->s.sound =  gi.soundindex("Enemy/MGun/trnslow.wav");
			self->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		}
	}
}

void auto_gun_arc (edict_t *self)
{
	vec3_t		arcorg;
	vec3_t		dir;
	float		facingAngle;
	
	
	facingAngle = NormalizeAngle(self->s.angles[1] + self->s.angle_diff);
	
	VectorCopy(self->s.origin, arcorg);
	arcorg[2] += 6;

	arcorg[1] += gi.irand(-7,6);

	VectorSet(dir, cos(facingAngle*DEGTORAD), sin(facingAngle*DEGTORAD), 0);
	FX_MakeElectricArc(arcorg, 40, dir);
	self->nextthink = level.time + .1;
	self->health--;
	if (self->health <= 0)
	{
		self->nextthink = -1;
	}
}

void auto_gun_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	// kill any existing sound for this entity
	self->s.sound =	0;
	self->avelocity[1] = 0;
	
	fxRunner.exec("weapons/world/autogundeath", self);
	self->health = gi.irand(5,20);
	self->think = auto_gun_arc;
	self->nextthink = level.time + .1;
	self->die = NULL;
	self->takedamage = DAMAGE_NO;
	// Biessman wants debris, eh? I'LL GIVE HIM DEBRIS!!!
	Obj_explode(self,&self->s.origin,1,1);
	ObjBecomeDebris(self,inflictor,attacker,damage,point);
}

void big_gun_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	// kill any existing sound for this entity
	self->s.sound =	0;
	self->avelocity[1] = 0;
	
	fxRunner.exec("weapons/world/autogundeath", self);
	self->die = NULL;
	self->takedamage = DAMAGE_NO;
	// Biessman wants debris, eh? I'LL GIVE HIM DEBRIS!!!
	Obj_explode(self,&self->s.origin,1,1);
	ObjBecomeDebris(self,inflictor,attacker,damage,point);
}

void auto_gun_spark(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
//	vec3_t vSparksPos, vSparksNormal;

//	VectorSubtract(self, other, vSparksNormal);
//	FX_MakeSparks(*(vec3_t*)&vSparksPos, *(vec3_t*)&vNormal, gi.irand(0,2));
}

void spawn_gun_auto(edict_t* ent)
{
	char		*skinname;
	ggBinstC		*bInstC = NULL;
	IGhoulInst		*boltInst = NULL;
	boltInstInfo_c	*newBoltInfo = NULL;
	float		fTemp = 0;

	// Backwards about invincibility
	if (ent->spawnflags & SF_INVULNERABLE)
	{
		ent->spawnflags &= ~SF_INVULNERABLE; 
	}
	else 
	{
		ent->spawnflags |= SF_INVULNERABLE;
	}

	// Backwards about pushability
	if (ent->spawnflags & SF_NOPUSH)
	{
		ent->spawnflags &= ~SF_NOPUSH; 
	}
	else 
	{
		ent->spawnflags |= SF_NOPUSH;
	}

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "gun2";
		break;
	default:
		skinname = "gun";
		break;
	}

	ent->s.effects |= EF_NO_BULLET;
	ent->flags |= FL_NO_KNOCKBACK;
	ent->pain = auto_gun_spark;
	// scale the health based on difficulty level
	ent->health = 100 + (game.playerSkills.getEnemyValue());

	if (!ent->speed)
	{
		ent->speed = 45;
	}
	// convert from deg/s to radians/frame
	ent->speed *= (0.1*DEGTORAD);
	if (!ent->volume)
	{
		ent->volume = 400;
	}
	if (!ent->wait)
	{
		ent->wait = 30;
	}
	ent->die = auto_gun_die;
	if (ent->spawnflags & AUTOGUN_ACTIVE)
	{
		ent->takedamage = DAMAGE_YES;
		ent->think = auto_gun_target;
		ent->nextthink = level.time + (ent->wait * .1);
	}

	ent->elasticity = M_PI; // allow 360 degrees of yaw
	if (ent->spawnflags & AUTOGUN_ARC_LIMITED)
	{
/*		fTemp = anglemod(ent->s.angles[YAW] + st.maxyaw);
		if (fTemp > 180)
		{
			fTemp = -(360 - fTemp);
		}
		if (fTemp < -180)
		{
			fTemp = -(-360 - fTemp);
		}
		ent->elasticity = fTemp * DEGTORAD; // max yaw
		fTemp = anglemod(ent->s.angles[YAW] - st.maxyaw);
		if (fTemp > 180)
		{
			fTemp = -(360 - fTemp);
		}
		if (fTemp < -180)
		{
			fTemp = -(-360 - fTemp);
		}
		ent->bouyancy = fTemp * DEGTORAD; // min yaw
*/
		ent->accel = st.maxpitch * DEGTORAD; // above
		ent->decel = st.minpitch * DEGTORAD; // below
	}
	else
	{
		ent->accel = 0; // allow no pitch above horizontal
		ent->decel = 0; // allow no pitch below horizontal
	}

	if ( (ent->spawnflags & AUTOGUN_NO_TRIPOD) && !(ent->spawnflags & AUTOGUN_NO_HANGY_THINGY) )
	{
		// no tripod, connect the gun to the hanging arm bracket thingy.
		SimpleModelInit2(ent,&genericModelData[OBJ_GUN_AUTO_ARM],skinname,NULL);
		ent->gravity = 0;
		// we want the hanging arm thingy
		SimpleModelAddBolt(ent, genericModelData[OBJ_GUN_AUTO_ARM], "DUMMY02", 
			genericModelData[OBJ_GUN_AUTO], "DUMMY02", skinname);
	}
	else if (ent->spawnflags & AUTOGUN_NO_TRIPOD)
	{
		// there is no tripod, but we're going to bolt a
		//null to the tripod and then bolt the gun to the null and then turn off the tripod.
		//the null will yaw and the gun will pitch.
		SimpleModelInit2(ent,&genericModelData[OBJ_GUN_AUTO_BASE],skinname,NULL);
		bInstC = SimpleModelAddBolt(ent, genericModelData[OBJ_GUN_AUTO_BASE], "DUMMY01", 
			genericModelData[OBJ_NULL], "DUMMY01", skinname);

		if (bInstC && (boltInst = bInstC->GetInstPtr()))
		{
			// the NULL actually has some geometry in it (ghoul made me do it :< ) so turn it off
			SimpleModelTurnOnOff(boltInst, false);
			ComplexModelAddBolt(bInstC, genericModelData[OBJ_NULL],
					"DUMMY01", genericModelData[OBJ_GUN_AUTO], "DUMMY01", skinname);
		}
		SimpleModelTurnOnOff(ent->ghoulInst, false);
		// turn off the "no tripod" flag so it'll fire properly. 
		// hacky? just a bit.
		ent->spawnflags &= ~AUTOGUN_NO_TRIPOD;
	}
	else
	{
		// there is a tripod,  don't bolt on the arm thingy. instead, we're going to bolt a
		//null to the tripod and then bolt the gun to the null.
		//the null will yaw and the gun will pitch.
		SimpleModelInit2(ent,&genericModelData[OBJ_GUN_AUTO_BASE],skinname,NULL);
		bInstC = SimpleModelAddBolt(ent, genericModelData[OBJ_GUN_AUTO_BASE], "DUMMY01", 
			genericModelData[OBJ_NULL], "DUMMY01", skinname);

		// ugh. ughity ugh ugh.
		if (bInstC && (boltInst = bInstC->GetInstPtr()))
		{
			// the NULL actually has some geometry in it (ghoul made me do it :< ) so turn it off
			SimpleModelTurnOnOff(boltInst, false);
			ComplexModelAddBolt(bInstC, genericModelData[OBJ_NULL],
					"DUMMY01", genericModelData[OBJ_GUN_AUTO], "DUMMY01", skinname);
		}
	}

	gi.soundindex("Enemy/MGun/trnslow.wav"); // precache sound
	gi.soundindex("Enemy/MGun/trnstop.wav"); // precache sound
	gi.soundindex("Weapons/Autoshot/fire.wav"); // precache sound
	gi.soundindex("Enemy/MGun/wake.wav"); // precache sound

	// precache muzzleflash effect and death effect
	gi.effectindex("weapons/othermz/autogun");
	gi.effectindex("weapons/world/autogundeath");

	// if anyone gets caught near this thing when it explodes
	gi.effectindex("environ/onfireburst");
}



void auto_gun_test_think(edict_t *self)
{
	vec3_t						fwd, barrel1Pos;

	AngleVectors(self->s.angles, fwd, NULL, NULL);

	VectorMA(self->s.origin, 27, fwd, barrel1Pos);

	fxRunner.setDir(fwd);
	fxRunner.exec("weapons/othermz/autogun", barrel1Pos);

	self->nextthink = level.time + 2.5;
}



// This is Pat's test autogun.
void spawn_test_gun_auto(edict_t* ent)
{
	char		*skinname;
	ggBinstC		*bInstC = NULL;
	IGhoulInst		*boltInst = NULL;
	boltInstInfo_c	*newBoltInfo = NULL;
	float		fTemp = 0;

	skinname = "gun";
	ent->flags |= FL_NO_KNOCKBACK;
	ent->pain = auto_gun_spark;
	ent->health = 10000;
	// convert from deg/s to radians/frame
	ent->die = auto_gun_die;
	ent->takedamage = DAMAGE_NO;
	ent->think = auto_gun_test_think;
	ent->nextthink = level.time + 2.5;

	ent->touch = NULL;

	// there is a tripod,  don't bolt on the arm thingy. instead, we're going to bolt a
	//null to the tripod and then bolt the gun to the null.
	//the null will yaw and the gun will pitch.
	SimpleModelInit2(ent,&genericModelData[OBJ_GUN_AUTO_BASE],skinname,NULL);
	bInstC = SimpleModelAddBolt(ent, genericModelData[OBJ_GUN_AUTO_BASE], "DUMMY01", 
		genericModelData[OBJ_NULL], "DUMMY01", skinname);

	// ugh. ughity ugh ugh.
	if (bInstC && (boltInst = bInstC->GetInstPtr()))
	{
		// the NULL actually has some geometry in it (ghoul made me do it :< ) so turn it off
		SimpleModelTurnOnOff(boltInst, false);
		ComplexModelAddBolt(bInstC, genericModelData[OBJ_NULL],
				"DUMMY01", genericModelData[OBJ_GUN_AUTO], "DUMMY01", skinname);
	}

	// precache muzzleflash effect
	gi.effectindex("weapons/othermz/autogun");
}


/*QUAKED misc_generic_gun_auto (1 .5 0)  (-28 -21 -34) (21 21 19) VULNERABLE PUSH ACTIVE ARC_LIMITED x x FLUFF
Tri-pod mounted twin machine gun.   
------ SPAWNFLAGS ------
VULNERABLE - can be hurt
PUSH - can be moved
ACTIVE - allow these guns to track and shoot.  
ARC_LIMITED - won't rotate outside of given arc
FLUFF - won't show if gl_pictip is set
--------KEYS------------
Skin -
0 - green (Bosnia)
1 - silver (Siberia)
Speed - angular velocity in degrees per second the gun will track. Default 45.
Volume - the distance the guns will track.  Default 400.
Wait - time interval in tenths of seconds that a gun in sleep mode will wait Default 30
Health - designers can't edit health
maxyaw - used in conjuction with ARC_LIMITED will define the arc the gun will look for 
targets on either side of its original facing (in degrees, to the left or right)
minpitch - used in conjuction with ARC_LIMITED will define the arc the gun will look for 
targets below its original facing (in degrees, default is 0)
maxpitch - used in conjuction with ARC_LIMITED will define the arc the gun will look for 
targets above its original facing (in degrees, default is 0)
------ SPECIALS ------
- will track and shoot the player
*/
void SP_misc_generic_gun_auto (edict_t *ent)
{
	ent->spawnflags |= AUTOGUN_NO_HANGY_THINGY;

	VectorSet (ent->mins, -28, -21, -34);
	VectorSet (ent->maxs,  21,  21,  19);

	spawn_gun_auto(ent);
}

/*QUAKED misc_generic_gun_auto2 (1 .5 0)  (-28 -18 -8) (21 21 18) VULNERABLE PUSH ACTIVE ARC_LIMITED x x FLUFF
Twin machine gun with a hanging arm thingy, to be attached to the ceiling.  
------ SPAWNFLAGS ------
VULNERABLE - can be hurt
PUSH - can be moved
ACTIVE - allow these guns to track and shoot.  
ARC_LIMITED - won't rotate outside of given arc
FLUFF - won't show if gl_pictip is set
--------KEYS------------
Skin -
0 - green (Bosnia)
1 - silver (Siberia)
Speed - angular velocity in degrees per second the gun will track. Default 45.
Volume - the distance the guns will track.  Default 400.
Wait - time interval in tenths of seconds that a gun in sleep mode will wait Default 30
Health - designers can't edit health
maxyaw - used in conjuction with ARC_LIMITED will define the arc the gun will look for 
targets on either side of its original facing (in degrees, to the left or right)
minpitch - used in conjuction with ARC_LIMITED will define the arc the gun will look for 
targets below its original facing (in degrees, default is 0)
maxpitch - used in conjuction with ARC_LIMITED will define the arc the gun will look for 
targets above its original facing (in degrees, default is 0)
------ SPECIALS ------
- will track and shoot the player
*/
void SP_misc_generic_gun_auto2 (edict_t *ent)
{
	ent->spawnflags |= AUTOGUN_NO_TRIPOD;

	VectorSet (ent->mins, -27, -18, -8);
	VectorSet (ent->maxs,  21,  21,  18);
	spawn_gun_auto(ent);
}

/*QUAKED misc_generic_gun_auto3 (1 .5 0)  (-21 -18 -8) (21 21 8) VULNERABLE PUSH ACTIVE ARC_LIMITED x x FLUFF
Twin machine gun with no tripod and no hanging arm thingy. Presumably this would just sit on a brush.
------ SPAWNFLAGS ------
VULNERABLE - can be hurt
PUSH - can be moved
ACTIVE - allow these guns to track and shoot.  
ARC_LIMITED - won't rotate outside of given arc
FLUFF - won't show if gl_pictip is set
--------KEYS------------
Skin -
0 - green (Bosnia)
1 - silver (Siberia)
Speed - angular velocity in degrees per second the gun will track. Default 45.
Volume - the distance the guns will track.  Default 400.
Wait - time interval in tenths of seconds that a gun in sleep mode will wait Default 30
Health - designers can't edit health
maxyaw - used in conjuction with ARC_LIMITED will define the arc the gun will look for 
targets on either side of its original facing (in degrees, to the left or right)
minpitch - used in conjuction with ARC_LIMITED will define the arc the gun will look for 
targets below its original facing (in degrees, default is 0)
maxpitch - used in conjuction with ARC_LIMITED will define the arc the gun will look for 
targets above its original facing (in degrees, default is 0)
------ SPECIALS ------
- will track and shoot the player
*/
void SP_misc_generic_gun_auto3 (edict_t *ent)
{
	ent->spawnflags |= (AUTOGUN_NO_HANGY_THINGY | AUTOGUN_NO_TRIPOD);
	VectorSet (ent->mins, -21, -18, -8);
	VectorSet (ent->maxs,  21,  21,  8);
	spawn_gun_auto(ent);
}



void SP_misc_test_gun_auto (edict_t *ent)
{
	VectorSet (ent->mins, -28, -21, -34);
	VectorSet (ent->maxs,  21,  21,  19);

	spawn_test_gun_auto(ent);
}




bool gun_big_target2(edict_t *self)
{
	IGhoulInst					*shieldInst,*gunInst;
	GhoulID						shieldBolt;
	boltonOrientation_c			boltonInfo;
	vec3_t						fwd, vToTarget, barrelPos;
	trace_t						tr;
	boltonOrientation_c::retCode_e ret = boltonOrientation_c::retCode_e::ret_TRUE;
	GhoulID gunBarrel;
	bool bRet = false;
	objFlipInfo_c	*pFlip = NULL;

	shieldInst = SimpleModelGetBolt(self, 1);

	self->nextthink = level.time + FRAMETIME;

	if (shieldInst && shieldInst->GetGhoulObject())
	{
		shieldBolt = shieldInst->GetGhoulObject()->FindPart("DUMMY01");
	}

	if (self->objInfo)
	{
		pFlip = (objFlipInfo_c*)self->objInfo->GetInfo(OIT_FLIP);
	}
	if (!shieldBolt || !self->enemy)
	{
		if (pFlip)
		{	// hack. using flipinfo for storage.
			pFlip->ground = 0;
		}
		return false;
	}

	// turn the shield to face the player 
	//
	if (!shieldInst->GetParent() || !shieldInst->GetParent()->GetGhoulObject())
	{
		if (pFlip)
		{	// hack. using flipinfo for storage.
			pFlip->ground = 0;
		}
		return false;
	}
	boltonInfo.root = self;


	VectorCopy(self->enemy->s.origin,self->pos2);
	self->pos2[2] += 30;

	boltonInfo.root = self;
	boltonInfo.boltonInst = shieldInst;
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY01");
	boltonInfo.parentInst = self->ghoulInst;
	boltonInfo.parentID = boltonInfo.parentInst->GetGhoulObject()->FindPart("DUMMY01");

	VectorCopy(self->pos2, boltonInfo.vTarget);
	boltonInfo.fMinPitch = -M_PI*0.1;
	boltonInfo.fMaxPitch = M_PI*0.25;
	boltonInfo.fMinYaw = -4000;
	boltonInfo.fMaxYaw = 4000;
	boltonInfo.fMaxTurnSpeed = .04;
	boltonInfo.bUsePitch = false;
	boltonInfo.bUseYaw = true;
	boltonInfo.bToRoot = true;
	boltonInfo.OrientBolton();


	boltonInfo.bUsePitch = true;
	boltonInfo.bUseYaw = false;

	// Turn gun to face player
	boltonInfo.boltonInst = SimpleModelGetBolt(self, 2);
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY02");
	boltonInfo.OrientBolton();

	// Turn gun to face player
	boltonInfo.boltonInst = SimpleModelGetBolt(self, 4);
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY03");
	boltonInfo.OrientBolton();

	boltonInfo.vTarget[2] -= 20; // try to keep top and bottom guns parallel

	// Turn gun to face player
	boltonInfo.root = self;
	boltonInfo.boltonInst = SimpleModelGetBolt(self, 3);
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY02");
	boltonInfo.OrientBolton();

	// Turn gun to face player
	boltonInfo.boltonInst = SimpleModelGetBolt(self, 5);
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY03");
	boltonInfo.OrientBolton();



	// if we're now pointing at our target shoot it
	VectorSubtract(self->enemy->s.origin, self->s.origin, vToTarget);
	GetGhoulPosDir(self->s.origin, self->s.angles, shieldInst, shieldBolt, "DUMMY01", NULL, NULL, NULL, fwd);
	VectorNormalize(vToTarget); // ouch
	if (DotProduct(vToTarget, fwd) > 0.95f)
	{
		// pointing at our target
		gi.trace (self->s.origin, NULL, NULL, self->enemy->s.origin, self, MASK_SHOT| MASK_WATER, &tr);
		if (tr.ent)
		{	
			if (tr.ent->client)
			{
				self->s.sound = 0;

				if (gunInst = SimpleModelGetBolt(self,2))
				{
					//if (pFlip && (pFlip->ground < 1))
					{
						//SimpleModelSetSequence2(gunInst, "gunright", SMSEQ_LOOP);
					}
					GetGhoulPosDir(self->s.origin, self->s.angles,gunInst, 0, "_FLASHTOPRIGHT", barrelPos, NULL, NULL, NULL);
					VectorSubtract(self->pos2, barrelPos, vToTarget);
					weapons.attack(ATK_AUTOGUN, self, barrelPos, vToTarget, gunInst);

					gunBarrel = gunInst->GetGhoulObject()->FindPart("_FLASHTOPRIGHT");
					if(gunBarrel)
					{
						fxRunner.execWithInst("weapons/othermz/autogun", self, gunInst, gunBarrel, MULTICAST_PHS, 1);
					}
				}
				if (gunInst = SimpleModelGetBolt(self,3))
				{
					//if (pFlip && (pFlip->ground < 1))
					{
						//SimpleModelSetSequence2(gunInst, "gunright", SMSEQ_LOOP);
					}
					GetGhoulPosDir(self->s.origin, self->s.angles,gunInst, 0, "_FLASHTOPRIGHT", barrelPos, NULL, NULL, NULL);
					VectorSubtract(self->pos2, barrelPos, vToTarget);
					weapons.attack(ATK_AUTOGUN, self, barrelPos, vToTarget, gunInst);

					gunBarrel = gunInst->GetGhoulObject()->FindPart("_FLASHTOPRIGHT");
					if(gunBarrel)
					{
						fxRunner.execWithInst("weapons/othermz/autogun", self, gunInst, gunBarrel, MULTICAST_PHS, 1);
					}
				}
				if (gunInst = SimpleModelGetBolt(self,4))
				{
					//if (pFlip && (pFlip->ground < 1))
					{
						//SimpleModelSetSequence2(gunInst, "gunleft", SMSEQ_LOOP);
					}
					GetGhoulPosDir(self->s.origin, self->s.angles,gunInst, 0, "_FLASHTOPLEFT", barrelPos, NULL, NULL, NULL);
					VectorSubtract(self->pos2, barrelPos, vToTarget);
					weapons.attack(ATK_AUTOGUN, self, barrelPos, vToTarget, gunInst);

					gunBarrel = gunInst->GetGhoulObject()->FindPart("_FLASHTOPLEFT");
					if(gunBarrel)
					{
						fxRunner.execWithInst("weapons/othermz/autogun", self, gunInst, gunBarrel, MULTICAST_PHS, 1);
					}
				}
				if (gunInst = SimpleModelGetBolt(self,5))
				{
					//if (pFlip && (pFlip->ground < 1))
					{
						//SimpleModelSetSequence2(gunInst, "gunleft", SMSEQ_LOOP);
					}
					GetGhoulPosDir(self->s.origin, self->s.angles,gunInst, 0, "_FLASHTOPLEFT", barrelPos, NULL, NULL, NULL);
					VectorSubtract(self->pos2, barrelPos, vToTarget);
					weapons.attack(ATK_AUTOGUN, self, barrelPos, fwd, gunInst);

					gunBarrel = gunInst->GetGhoulObject()->FindPart("_FLASHTOPLEFT");
					if(gunBarrel)
					{
						fxRunner.execWithInst("weapons/othermz/autogun", self, gunInst, gunBarrel, MULTICAST_PHS, 1);
					}
				}
				bRet = true;
			}
		}
		else
		{
			self->nextthink = level.time + .1;
		}
	}
	if (false && !bRet)
	{
		// since we're not shooting, if we turned during this frame play the turning sound
		if (ret == boltonOrientation_c::retCode_e::ret_FALSE)
		{
//			self->s.sound =  gi.soundindex("Enemy/MGun/trnslow.wav");
//			self->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		}
		// freeze our guns' animations
		if (gunInst = SimpleModelGetBolt(self,2))
		{
			SimpleModelSetSequence2(gunInst, "gunright", SMSEQ_HOLD);
		}
		if (gunInst = SimpleModelGetBolt(self,3))
		{
			SimpleModelSetSequence2(gunInst, "gunright", SMSEQ_HOLD);
		}
		if (gunInst = SimpleModelGetBolt(self,4))
		{
			SimpleModelSetSequence2(gunInst, "gunleft", SMSEQ_HOLD);
		}
		if (gunInst = SimpleModelGetBolt(self,5))
		{
			SimpleModelSetSequence2(gunInst, "gunleft", SMSEQ_HOLD);
		}
	} 
/*	if (self->objInfo && (pFlip = (objFlipInfo_c*)self->objInfo->GetInfo(OIT_FLIP)))
	{	// hack. using flipinfo for storage.
		pFlip->ground = bRet?1.0:0.0;
	}
*/	return bRet;
}


void gun_big_target (edict_t *self)
{
	vec3_t		targetDir;
	edict_t		*target = NULL;
	trace_t		tr;

	self->nextthink = level.time + FRAMETIME;

	CRadiusContent rad(self->s.origin, self->volume);

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		target = rad.foundEdict(i);

		if(target->client)
		//  right now, we're only targeting the player.  should make this switchable by designers
		{
			// trace to see if I can see player.
			gi.trace (self->s.origin, NULL, NULL, target->s.origin, self, MASK_SHOT| MASK_WATER, &tr);
			if ((tr.ent->client) && (tr.ent->health > 0))
			{	
				self->enemy = target;
				self->nextthink = level.time + .1;
				self->think = gun_big_target;
			}
			else
			{
				self->enemy = NULL;
				return;
			}
		}	
	}
	if (NULL == self->enemy)
	{
		return;
	}
	VectorSubtract (self->enemy->s.origin, self->s.origin, targetDir);

	// is target within range?
	if (VectorLengthSquared(targetDir) > (self->volume*self->volume))
	{	// not in range
		self->avelocity[1] = 0;
		//  perhaps return to original position?
		if (self->s.sound == gi.soundindex("Enemy/MGun/trnslow.wav"))
		{
			self->s.sound = 0;
			self->s.sound =  gi.soundindex("Enemy/MGun/trnstop.wav");
			self->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		}
		self->nextthink = level.time +  .1;
		return;
	}

	// guns now use ghoul rotation stuff

	// gun_big_target2 will return true if it fired.
	if (!gun_big_target2(self))
	{	// didn't fire so make sure we aren't looping the firing sequence for the gun barrels
	}
	return;
}

bool gun_castle_target2(edict_t *self)
{
	IGhoulInst					*shieldInst,*gunInst;
	GhoulID						shieldBolt;
	boltonOrientation_c			boltonInfo;
	vec3_t						fwd, vToTarget, barrelPos;
	trace_t						tr;
	boltonOrientation_c::retCode_e ret = boltonOrientation_c::retCode_e::ret_TRUE;
	GhoulID gunBarrel;
	bool bRet = false;
	objFlipInfo_c	*pFlip = NULL;

	shieldInst = SimpleModelGetBolt(self, 1);

	self->nextthink = level.time + FRAMETIME;

	if (shieldInst && shieldInst->GetGhoulObject())
	{
		shieldBolt = shieldInst->GetGhoulObject()->FindPart("DUMMY01");
	}

	if (self->objInfo)
	{
		pFlip = (objFlipInfo_c*)self->objInfo->GetInfo(OIT_FLIP);
	}
	if (!shieldBolt || !self->enemy)
	{
		if (pFlip)
		{	// hack. using flipinfo for storage.
			pFlip->ground = 0;
		}
		return false;
	}

	// turn the shield to face the player 
	//
	if (!shieldInst->GetParent() || !shieldInst->GetParent()->GetGhoulObject())
	{
		if (pFlip)
		{	// hack. using flipinfo for storage.
			pFlip->ground = 0;
		}
		return false;
	}
	boltonInfo.root = self;


	VectorCopy(self->enemy->s.origin,self->pos2);
	self->pos2[2] += 30;

	boltonInfo.root = self;
	boltonInfo.boltonInst = shieldInst;
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY01");
	boltonInfo.parentInst = self->ghoulInst;
	boltonInfo.parentID = boltonInfo.parentInst->GetGhoulObject()->FindPart("DUMMY01");

	VectorCopy(self->pos2, boltonInfo.vTarget);
	boltonInfo.fMinPitch = -M_PI*0.1;
	boltonInfo.fMaxPitch = M_PI*0.25;
	boltonInfo.fMinYaw = -self->elasticity;
	boltonInfo.fMaxYaw = self->elasticity;
	boltonInfo.fMaxTurnSpeed = self->speed;
	boltonInfo.bUsePitch = false;
	boltonInfo.bUseYaw = true;
	boltonInfo.bToRoot = true;
	boltonInfo.OrientBolton();


	boltonInfo.bUsePitch = true;
	boltonInfo.bUseYaw = false;

	// Turn gun to face player
	boltonInfo.boltonInst = SimpleModelGetBolt(self, 2);
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY02");
	boltonInfo.OrientBolton();

	// Turn gun to face player
	boltonInfo.boltonInst = SimpleModelGetBolt(self, 4);
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY03");
	boltonInfo.OrientBolton();

	boltonInfo.vTarget[2] -= 20; // try to keep top and bottom guns parallel

	// Turn gun to face player
	boltonInfo.root = self;
	boltonInfo.boltonInst = SimpleModelGetBolt(self, 3);
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY02");
	boltonInfo.OrientBolton();

	// Turn gun to face player
	boltonInfo.boltonInst = SimpleModelGetBolt(self, 5);
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY03");
	boltonInfo.OrientBolton();



	// if we're now pointing at our target shoot it
	VectorSubtract(self->enemy->s.origin, self->s.origin, vToTarget);
	vToTarget[2]+=30;
	GetGhoulPosDir(self->s.origin, self->s.angles, shieldInst, shieldBolt, "DUMMY01", NULL, NULL, NULL, fwd);
	VectorNormalize(vToTarget); // ouch
	VectorNormalize(fwd); // ouch ouch
	if (DotProduct(vToTarget, fwd) > 0.95f)
	{
		// pointing at our target
		gi.trace (self->s.origin, NULL, NULL, self->enemy->s.origin, self, MASK_SHOT| MASK_WATER, &tr);
		if (tr.ent)
		{	
			if (tr.ent->client)
			{
				self->s.sound = 0;

				if (gunInst = SimpleModelGetBolt(self,2))
				{
					//if (pFlip && (pFlip->ground < 1))
					{
						//SimpleModelSetSequence2(gunInst, "gunright", SMSEQ_LOOP);
					}
					GetGhoulPosDir(self->s.origin, self->s.angles,gunInst, 0, "_FLASHTOPRIGHT", barrelPos, NULL, NULL, NULL);
					VectorSubtract(self->pos2, barrelPos, vToTarget);
					weapons.attack(ATK_AUTOGUN, self, barrelPos, vToTarget, gunInst);

					gunBarrel = gunInst->GetGhoulObject()->FindPart("_FLASHTOPRIGHT");
					if(gunBarrel)
					{
						fxRunner.execWithInst("weapons/othermz/autogun", self, gunInst, gunBarrel, MULTICAST_PHS, 1);
					}
				}
				if (gunInst = SimpleModelGetBolt(self,3))
				{
					//if (pFlip && (pFlip->ground < 1))
					{
						//SimpleModelSetSequence2(gunInst, "gunright", SMSEQ_LOOP);
					}
					GetGhoulPosDir(self->s.origin, self->s.angles,gunInst, 0, "_FLASHTOPRIGHT", barrelPos, NULL, NULL, NULL);
					VectorSubtract(self->pos2, barrelPos, vToTarget);
					weapons.attack(ATK_AUTOGUN, self, barrelPos, vToTarget, gunInst);

					gunBarrel = gunInst->GetGhoulObject()->FindPart("_FLASHTOPRIGHT");
					if(gunBarrel)
					{
						fxRunner.execWithInst("weapons/othermz/autogun", self, gunInst, gunBarrel, MULTICAST_PHS, 1);
					}
				}
				if (gunInst = SimpleModelGetBolt(self,4))
				{
					//if (pFlip && (pFlip->ground < 1))
					{
						//SimpleModelSetSequence2(gunInst, "gunleft", SMSEQ_LOOP);
					}
					GetGhoulPosDir(self->s.origin, self->s.angles,gunInst, 0, "_FLASHTOPLEFT", barrelPos, NULL, NULL, NULL);
					VectorSubtract(self->pos2, barrelPos, vToTarget);
					weapons.attack(ATK_AUTOGUN, self, barrelPos, vToTarget, gunInst);

					gunBarrel = gunInst->GetGhoulObject()->FindPart("_FLASHTOPLEFT");
					if(gunBarrel)
					{
						fxRunner.execWithInst("weapons/othermz/autogun", self, gunInst, gunBarrel, MULTICAST_PHS, 1);
					}
				}
				if (gunInst = SimpleModelGetBolt(self,5))
				{
					//if (pFlip && (pFlip->ground < 1))
					{
						//SimpleModelSetSequence2(gunInst, "gunleft", SMSEQ_LOOP);
					}
					GetGhoulPosDir(self->s.origin, self->s.angles,gunInst, 0, "_FLASHTOPLEFT", barrelPos, NULL, NULL, NULL);
					VectorSubtract(self->pos2, barrelPos, vToTarget);
					weapons.attack(ATK_AUTOGUN, self, barrelPos, fwd, gunInst);

					gunBarrel = gunInst->GetGhoulObject()->FindPart("_FLASHTOPLEFT");
					if(gunBarrel)
					{
						fxRunner.execWithInst("weapons/othermz/autogun", self, gunInst, gunBarrel, MULTICAST_PHS, 1);
					}
				}
				bRet = true;
			}
		}
		else
		{
			self->nextthink = level.time + .1;
		}

		//eh?  this stuff belongs here!!!!
		if (gunInst = SimpleModelGetBolt(self,2))
		{
			SimpleModelSetSequence2(gunInst, "gunright", SMSEQ_HOLD);
		}
		if (gunInst = SimpleModelGetBolt(self,3))
		{
			SimpleModelSetSequence2(gunInst, "gunright", SMSEQ_HOLD);
		}
		if (gunInst = SimpleModelGetBolt(self,4))
		{
			SimpleModelSetSequence2(gunInst, "gunleft", SMSEQ_HOLD);
		}
		if (gunInst = SimpleModelGetBolt(self,5))
		{
			SimpleModelSetSequence2(gunInst, "gunleft", SMSEQ_HOLD);
		}

	}
	if (false && !bRet)
	{
		// since we're not shooting, if we turned during this frame play the turning sound
		if (ret == boltonOrientation_c::retCode_e::ret_FALSE)
		{
//			self->s.sound =  gi.soundindex("Enemy/MGun/trnslow.wav");
//			self->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		}
		// freeze our guns' animations
/*		if (gunInst = SimpleModelGetBolt(self,2))
		{
			SimpleModelSetSequence2(gunInst, "gunright", SMSEQ_HOLD);
		}
		if (gunInst = SimpleModelGetBolt(self,3))
		{
			SimpleModelSetSequence2(gunInst, "gunright", SMSEQ_HOLD);
		}
		if (gunInst = SimpleModelGetBolt(self,4))
		{
			SimpleModelSetSequence2(gunInst, "gunleft", SMSEQ_HOLD);
		}
		if (gunInst = SimpleModelGetBolt(self,5))
		{
			SimpleModelSetSequence2(gunInst, "gunleft", SMSEQ_HOLD);
		}
*/
	} 
/*	if (self->objInfo && (pFlip = (objFlipInfo_c*)self->objInfo->GetInfo(OIT_FLIP)))
	{	// hack. using flipinfo for storage.
		pFlip->ground = bRet?1.0:0.0;
	}
*/	return bRet;
}


void gun_castle_target (edict_t *self)
{
	vec3_t		targetDir;
	edict_t		*target = NULL;
	trace_t		tr;

	self->nextthink = level.time + FRAMETIME;

	CRadiusContent rad(self->s.origin, self->volume);

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		target = rad.foundEdict(i);

		if(target->client)
		//  right now, we're only targeting the player.  should make this switchable by designers
		{
			// trace to see if I can see player.
			gi.trace (self->s.origin, NULL, NULL, target->s.origin, self, MASK_SHOT| MASK_WATER, &tr);
			if ((tr.ent->client) && (tr.ent->health > 0))
			{	
				self->enemy = target;
				self->nextthink = level.time + .1;
				self->think = gun_castle_target;
			}
			else
			{
				self->enemy = NULL;
				return;
			}
		}	
	}
	if (NULL == self->enemy)
	{
		return;
	}
	VectorSubtract (self->enemy->s.origin, self->s.origin, targetDir);

	// is target within range?
	if (VectorLengthSquared(targetDir) > (self->volume*self->volume))
	{	// not in range
		self->avelocity[1] = 0;
		//  perhaps return to original position?
		if (self->s.sound == gi.soundindex("Enemy/MGun/trnslow.wav"))
		{
			self->s.sound = 0;
			self->s.sound =  gi.soundindex("Enemy/MGun/trnstop.wav");
			self->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		}
		self->nextthink = level.time +  .1;
		return;
	}

	// guns now use ghoul rotation stuff

	// gun_big_target2 will return true if it fired.
	if (!gun_castle_target2(self))
	{	// didn't fire so make sure we aren't looping the firing sequence for the gun barrels
	}
	return;
}

void gun_castle_getready2(edict_t *self)
{
	IGhoulInst					*shieldInst;
	GhoulID						shieldBolt;
	boltonOrientation_c			boltonInfo;
	boltonOrientation_c::retCode_e ret = boltonOrientation_c::retCode_e::ret_TRUE;
	objFlipInfo_c	*pFlip = NULL;

	shieldInst = SimpleModelGetBolt(self, 1);

	self->nextthink = level.time + FRAMETIME;

	if (shieldInst && shieldInst->GetGhoulObject())
	{
		shieldBolt = shieldInst->GetGhoulObject()->FindPart("DUMMY01");
	}

	if (self->objInfo)
	{
		pFlip = (objFlipInfo_c*)self->objInfo->GetInfo(OIT_FLIP);
	}
	if (!shieldBolt || !shieldInst->GetParent() || !shieldInst->GetParent()->GetGhoulObject())
	{
		return;
	}
	boltonInfo.root = self;


//	VectorCopy(self->enemy->s.origin,self->pos2);
//	self->pos2[2] += 30;

//	AngleVectors(self->s.origin, fwd, NULL, NULL);
//	VectorScale(fwd, -100, fwd);
//	VectorAdd(fwd, self->s.origin, self->pos2);

	boltonInfo.root = self;
	boltonInfo.boltonInst = shieldInst;
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY01");
	boltonInfo.parentInst = self->ghoulInst;
	boltonInfo.parentID = boltonInfo.parentInst->GetGhoulObject()->FindPart("DUMMY01");

	VectorCopy(self->pos2, boltonInfo.vTarget);
	boltonInfo.fMinPitch = -M_PI*0.1;
	boltonInfo.fMaxPitch = M_PI*0.25;
	boltonInfo.fMinYaw = -self->elasticity;
//	boltonInfo.fMaxYaw = self->elasticity;
	boltonInfo.fMaxYaw = 0;
	boltonInfo.fMaxTurnSpeed = self->speed*0.1;
	boltonInfo.bUsePitch = false;
	boltonInfo.bUseYaw = true;
	boltonInfo.bToRoot = true;
	boltonInfo.OrientBolton();


	boltonInfo.bUsePitch = true;
	boltonInfo.bUseYaw = false;

	// Turn gun to face player
	boltonInfo.boltonInst = SimpleModelGetBolt(self, 2);
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY02");
	boltonInfo.OrientBolton();

	// Turn gun to face player
	boltonInfo.boltonInst = SimpleModelGetBolt(self, 4);
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY03");
	boltonInfo.OrientBolton();

	boltonInfo.vTarget[2] -= 20; // try to keep top and bottom guns parallel

	// Turn gun to face player
	boltonInfo.root = self;
	boltonInfo.boltonInst = SimpleModelGetBolt(self, 3);
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY02");
	boltonInfo.OrientBolton();

	// Turn gun to face player
	boltonInfo.boltonInst = SimpleModelGetBolt(self, 5);
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY03");
	ret = boltonInfo.OrientBolton();

	// if we're now pointing at our target shoot it
//	VectorSubtract(self->enemy->s.origin, self->s.origin, vToTarget);
//	GetGhoulPosDir(self->s.origin, self->s.angles, shieldInst, shieldBolt, "DUMMY01", NULL, NULL, NULL, fwd);
//	VectorNormalize(vToTarget); // ouch

	// since we're not shooting, if we turned during this frame play the turning sound
	if (ret == boltonOrientation_c::retCode_e::ret_FALSE)
	{
		self->think = gun_castle_target;
//		self->s.sound =  gi.soundindex("Enemy/MGun/trnslow.wav");
//		self->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
	}
}

void gun_castle_getready1(edict_t *self)
{
	IGhoulInst					*shieldInst;
	GhoulID						shieldBolt;
	boltonOrientation_c			boltonInfo;
	vec3_t						fwd;
	boltonOrientation_c::retCode_e ret = boltonOrientation_c::retCode_e::ret_TRUE;
	objFlipInfo_c	*pFlip = NULL;

	shieldInst = SimpleModelGetBolt(self, 1);

	self->nextthink = level.time + FRAMETIME;

	if (shieldInst && shieldInst->GetGhoulObject())
	{
		shieldBolt = shieldInst->GetGhoulObject()->FindPart("DUMMY01");
	}

	if (self->objInfo)
	{
		pFlip = (objFlipInfo_c*)self->objInfo->GetInfo(OIT_FLIP);
	}
	if (!shieldBolt || !shieldInst->GetParent() || !shieldInst->GetParent()->GetGhoulObject())
	{
		return;
	}
	boltonInfo.root = self;


//	VectorCopy(self->enemy->s.origin,self->pos2);
//	self->pos2[2] += 30;

//	AngleVectors(self->s.origin, fwd, NULL, NULL);
//	VectorScale(fwd, -100, fwd);
//	VectorAdd(fwd, self->s.origin, self->pos2);

	boltonInfo.root = self;
	boltonInfo.boltonInst = shieldInst;
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY01");
	boltonInfo.parentInst = self->ghoulInst;
	boltonInfo.parentID = boltonInfo.parentInst->GetGhoulObject()->FindPart("DUMMY01");

	VectorCopy(self->pos2, boltonInfo.vTarget);
	boltonInfo.fMinPitch = -M_PI*0.1;
	boltonInfo.fMaxPitch = M_PI*0.25;
	boltonInfo.fMinYaw = -self->elasticity;
//	boltonInfo.fMaxYaw = self->elasticity;
	boltonInfo.fMaxYaw = 0;
	boltonInfo.fMaxTurnSpeed = self->speed*0.1;
	boltonInfo.bUsePitch = false;
	boltonInfo.bUseYaw = true;
	boltonInfo.bToRoot = true;
	boltonInfo.OrientBolton();


	boltonInfo.bUsePitch = true;
	boltonInfo.bUseYaw = false;

	// Turn gun to face player
	boltonInfo.boltonInst = SimpleModelGetBolt(self, 2);
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY02");
	boltonInfo.OrientBolton();

	// Turn gun to face player
	boltonInfo.boltonInst = SimpleModelGetBolt(self, 4);
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY03");
	boltonInfo.OrientBolton();

	boltonInfo.vTarget[2] -= 20; // try to keep top and bottom guns parallel

	// Turn gun to face player
	boltonInfo.root = self;
	boltonInfo.boltonInst = SimpleModelGetBolt(self, 3);
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY02");
	boltonInfo.OrientBolton();

	// Turn gun to face player
	boltonInfo.boltonInst = SimpleModelGetBolt(self, 5);
	boltonInfo.boltonID = boltonInfo.boltonInst->GetGhoulObject()->FindPart("DUMMY03");
	ret = boltonInfo.OrientBolton();

	// if we're now pointing at our target shoot it
//	VectorSubtract(self->enemy->s.origin, self->s.origin, vToTarget);
//	GetGhoulPosDir(self->s.origin, self->s.angles, shieldInst, shieldBolt, "DUMMY01", NULL, NULL, NULL, fwd);
//	VectorNormalize(vToTarget); // ouch

	// since we're not shooting, if we turned during this frame play the turning sound
	if (ret == boltonOrientation_c::retCode_e::ret_FALSE)
	{
		GetGhoulPosDir(self->s.origin, self->s.angles, shieldInst, shieldBolt, "DUMMY01", NULL, NULL, NULL, fwd);
//		AngleVectors(self->s.angles, fwd, NULL, NULL);
		VectorScale(fwd, -100, fwd);
		VectorAdd(fwd, self->s.origin, self->pos2);

		self->think = gun_castle_getready2;
//		self->s.sound =  gi.soundindex("Enemy/MGun/trnslow.wav");
//		self->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
	}
}

void gun_castle_makesound (edict_t *self)
{
	self->think = gun_castle_target;
	gi.sound(self, CHAN_VOICE, gi.soundindex("Enemy/MGun/Wake2.wav"), 1.0, ATTN_NORM, 0);//fixme - bounce
	self->nextthink = level.time + 5.0;
}

void gun_castle_use(edict_t *self, edict_t *other, edict_t *activator)
{
//	vec3_t	fwd;
	if (self->think == gun_castle_target)
	{
		self->think = NULL;
		self->nextthink = 0.0;
	}
	else
	{
//		self->think = gun_castle_getready1;
//		self->think = gun_castle_target;
		self->think = gun_castle_makesound;
		if (self->wait*0.5 < 5.1)
		{
			self->nextthink = level.time;// + (self->wait /** .1*/);
		}
		else
		{
			self->nextthink = level.time + (self->wait*0.5 /** .1*/ - 5.0);
		}

/*
		IGhoulInst					*shieldInst;
		GhoulID						shieldBolt;
		shieldInst = SimpleModelGetBolt(self, 1);

		if (shieldInst && shieldInst->GetGhoulObject())
		{
			shieldBolt = shieldInst->GetGhoulObject()->FindPart("DUMMY01");
		}

		if (!shieldBolt || !shieldInst->GetParent() || !shieldInst->GetParent()->GetGhoulObject())
		{
			return;
		}

		GetGhoulPosDir(self->s.origin, self->s.angles, shieldInst, shieldBolt, "DUMMY01", NULL, NULL, NULL, fwd);
//		AngleVectors(self->s.angles, fwd, NULL, NULL);
		VectorScale(fwd, -100, fwd);
		VectorAdd(fwd, self->s.origin, self->pos2);
*/
	}
}



/*QUAKED misc_generic_gun_big (1 .5 0)  (-112 -75 -59) (48 76 39)  INVULNERABLE  NOPUSH  ACTIVE ARC_LIMITED x x FLUFF
AWWWWWWW yeah....   Big ass quad machine gun turret
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A can't ever be pushed
ACTIVE - allow these guns to track and shoot.  
ARC_LIMITED - won't rotate outside of given arc
FLUFF - won't show if gl_pictip is set
------ KEYS ------
Skin - 
0 - bosnian
1 - siberian
Speed - angular velocity in degrees per second the gun will track. Default 45.
Volume - the distance the guns will track.  Default 400.
Wait - time interval in tenths of seconds that a gun in sleep mode will wait Default 30
Health - designers can't edit health
maxyaw - used in conjuction with ARC_LIMITED will define the arc the gun will look for 
targets on either side of its original facing (in degrees, to the left or right)
minpitch - used in conjuction with ARC_LIMITED will define the arc the gun will look for 
targets below its original facing (in degrees, default is 0)
maxpitch - used in conjuction with ARC_LIMITED will define the arc the gun will look for 
targets above its original facing (in degrees, default is 0)
*/

/*QUAKED misc_generic_gun_castle (1 .5 0)  (-56 -37 -29) (24 38 19)  INVULNERABLE  NOPUSH  ACTIVE ARC_LIMITED TOGGLEABLE TRIGGER_SPAWN FLUFF
AWWWWWWW yeah....   Big ass quad machine gun turret
------ SPAWNFLAGS ------
INVULNERABLE - N/A can't ever be damaged.
NOPUSH - N/A can't ever be pushed
ACTIVE - allow these guns to track and shoot.  
ARC_LIMITED - won't rotate outside of given arc
TOGGLEABLE - using through trigger will activate/deactivate
TRIGGER_SPAWN - start inactive
FLUFF - won't show if gl_pictip is set
------ KEYS ------
Skin - 
0 - order
1 - bosnian
2 - siberian
Speed - angular velocity in degrees per second the gun will track. Default 45.
Volume - the distance the guns will track.  Default 400.
Wait - time interval in tenths of seconds that a gun in sleep mode will wait Default 15
Health - designers can't edit health
maxyaw - used in conjuction with ARC_LIMITED will define the arc the gun will look for 
targets on either side of its original facing (in degrees, to the left or right)
minpitch - used in conjuction with ARC_LIMITED will define the arc the gun will look for 
targets below its original facing (in degrees, default is 0)
maxpitch - used in conjuction with ARC_LIMITED will define the arc the gun will look for 
targets above its original facing (in degrees, default is 0)
*/



void SP_misc_generic_gun_castle (edict_t *ent)
{
	char *skinname;
	ggBinstC	*baseBinst = NULL;
	Matrix4		inMat1, inMat2, outMat;

	gi.soundindex("Enemy/MGun/Wake2.wav");

	ent->spawnflags |= SF_INVULNERABLE;
	ent->takedamage = DAMAGE_NO;

//	VectorSet (ent->mins, -111, -74, -58);
//	VectorSet (ent->maxs,   47,  75,  38);
	VectorSet (ent->mins, -56, -37, -29);
	VectorSet (ent->maxs,   24,  38,  19);
//	ent->s.origin[2]-=37;

	ent->spawnflags |= SF_NOPUSH;

	switch (ent->s.skinnum)
	{
	case 2:
		skinname = "gun2";
		break;
	case 1:
		skinname = "gun";
		break;
	default:
		skinname = "gun_dekker";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_GUN_BIG_BASE],skinname,NULL);

	ent->ghoulInst->GetXForm(inMat1);
	inMat2.Scale(0.5);
	outMat.Concat(inMat1, inMat2);
	ent->ghoulInst->SetXForm(outMat);
	
	baseBinst = SimpleModelAddBolt(ent,genericModelData[OBJ_GUN_BIG_BASE],"DUMMY01",
						genericModelData[OBJ_GUN_BIG_SHIELD],"DUMMY01",skinname);

	if (baseBinst)
	{
		ComplexModelAddBolt(baseBinst,
			genericModelData[OBJ_GUN_BIG_SHIELD], "DUMMY02", 
			genericModelData[OBJ_GUN_BIG_GUN_RIGHT], "DUMMY02", skinname);
		ComplexModelAddBolt(baseBinst,
			genericModelData[OBJ_GUN_BIG_SHIELD], "DUMMY04", 
			genericModelData[OBJ_GUN_BIG_GUN_RIGHT], "DUMMY02", skinname);
		ComplexModelAddBolt(baseBinst,
			genericModelData[OBJ_GUN_BIG_SHIELD], "DUMMY03", 
			genericModelData[OBJ_GUN_BIG_GUN_LEFT], "DUMMY03", skinname);
		ComplexModelAddBolt(baseBinst,
			genericModelData[OBJ_GUN_BIG_SHIELD], "DUMMY05", 
			genericModelData[OBJ_GUN_BIG_GUN_LEFT], "DUMMY03", skinname);
	}


	ent->s.effects |= EF_NO_BULLET;
	ent->flags |= FL_NO_KNOCKBACK;
//	ent->pain = auto_gun_spark;
	ent->health = 200 + (game.playerSkills.getEnemyValue() * 150);

	if (!ent->speed)
	{
		ent->speed = 45;
	}


	// convert from deg/s to radians/frame
	ent->speed *= (0.1*DEGTORAD);
	if (!ent->volume)
	{
		ent->volume = 400;
	}
	if (!ent->wait)
	{
		ent->wait = 15;
	}

	if (ent->spawnflags & AUTOGUN_ACTIVE)
	{
		if (!(ent->spawnflags & AUTOGUN_TRIGGER_SPAWN))
		{
			ent->think = gun_castle_makesound;
			if (ent->wait*0.5 < 5.1)
			{
				ent->nextthink = level.time;// + (self->wait /** .1*/);
			}
			else
			{
				ent->nextthink = level.time + (ent->wait*0.5 /** .1*/ - 5.0);
			}

//			ent->think = gun_castle_target;
//			ent->nextthink = level.time + (ent->wait /** .1*/);
		}
		if (ent->spawnflags & AUTOGUN_TOGGLEABLE)
		{
			ent->use = gun_castle_use;
		}
	}
	ent->die = big_gun_die;

	if (ent->spawnflags & AUTOGUN_ARC_LIMITED)
	{
		ent->elasticity = st.maxyaw * DEGTORAD; // left/right
		ent->accel = st.maxpitch * DEGTORAD; // above
		ent->decel = st.minpitch * DEGTORAD; // below
	}
	else
	{
		ent->elasticity = M_PI; // allow 360 degrees of yaw
		ent->accel = 0; // allow no pitch above horizontal
		ent->decel = 0; // allow no pitch below horizontal
	}

	gi.soundindex("Enemy/MGun/trnslow.wav"); // precache sound
	gi.soundindex("Enemy/MGun/trnstop.wav"); // precache sound
	gi.soundindex("Weapons/Autoshot/fire.wav"); // precache sound
	gi.soundindex("Enemy/MGun/wake.wav"); // precache sound

	// precache muzzleflash effect and death effect
	gi.effectindex("weapons/othermz/autogun");
	gi.effectindex("weapons/world/autogundeath");

	// if anyone gets caught near this thing when it explodes
	gi.effectindex("environ/onfireburst");
}

void SP_misc_generic_gun_big (edict_t *ent)
{
	char *skinname;
	ggBinstC	*baseBinst = NULL;

	VectorSet (ent->mins, -111, -74, -58);
	VectorSet (ent->maxs,   47,  75,  38);

	ent->spawnflags |= SF_NOPUSH;

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "gun2";
		break;
	default:
		skinname = "gun";
		break;
	}

	SimpleModelInit2(ent,&genericModelData[OBJ_GUN_BIG_BASE],skinname,NULL);
	baseBinst = SimpleModelAddBolt(ent,genericModelData[OBJ_GUN_BIG_BASE],"DUMMY01",
						genericModelData[OBJ_GUN_BIG_SHIELD],"DUMMY01",skinname);

	if (baseBinst)
	{
		ComplexModelAddBolt(baseBinst,
			genericModelData[OBJ_GUN_BIG_SHIELD], "DUMMY02", 
			genericModelData[OBJ_GUN_BIG_GUN_RIGHT], "DUMMY02", skinname);
		ComplexModelAddBolt(baseBinst,
			genericModelData[OBJ_GUN_BIG_SHIELD], "DUMMY04", 
			genericModelData[OBJ_GUN_BIG_GUN_RIGHT], "DUMMY02", skinname);
		ComplexModelAddBolt(baseBinst,
			genericModelData[OBJ_GUN_BIG_SHIELD], "DUMMY03", 
			genericModelData[OBJ_GUN_BIG_GUN_LEFT], "DUMMY03", skinname);
		ComplexModelAddBolt(baseBinst,
			genericModelData[OBJ_GUN_BIG_SHIELD], "DUMMY05", 
			genericModelData[OBJ_GUN_BIG_GUN_LEFT], "DUMMY03", skinname);
	}


	ent->s.effects |= EF_NO_BULLET;
	ent->flags |= FL_NO_KNOCKBACK;
//	ent->pain = auto_gun_spark;
	ent->health = 400 + (game.playerSkills.getEnemyValue() * 200);

	if (!ent->speed)
	{
		ent->speed = 45;
	}


	// convert from deg/s to radians/frame
	ent->speed *= (0.1*DEGTORAD);
	if (!ent->volume)
	{
		ent->volume = 400;
	}
	if (!ent->wait)
	{
		ent->wait = 30;
	}

	if (ent->spawnflags & AUTOGUN_ACTIVE)
	{
		ent->takedamage = DAMAGE_YES;
		ent->think = gun_big_target;
		ent->nextthink = level.time + (ent->wait * .1);
	}
	ent->die = big_gun_die;

	if (ent->spawnflags & AUTOGUN_ARC_LIMITED)
	{
		ent->elasticity = st.maxyaw * DEGTORAD; // left/right
		ent->accel = st.maxpitch * DEGTORAD; // above
		ent->decel = st.minpitch * DEGTORAD; // below
	}
	else
	{
		ent->elasticity = M_PI; // allow 360 degrees of yaw
		ent->accel = 0; // allow no pitch above horizontal
		ent->decel = 0; // allow no pitch below horizontal
	}

	// precache muzzleflash effect
	gi.effectindex("weapons/othermz/autogun");
}







/*QUAKED misc_generic_heap (1 .5 0) (-21 -16 -2) (17 18 3) INVULNERABLE   NOPUSH x x x x FLUFF
A heap of paper/guts/poop
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A, can't ever be pushed
FLUFF - won't show if gl_pictip is set
------ KEYS ------
Skin :
 0 - pile of paper. (default)
 1 - pile of guts.
 2 - cow patties... moo...
*/
void SP_misc_generic_heap (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -20, -15, -1);
	VectorSet (ent->maxs,  16,  17,  2);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "heap2";
		ent->surfaceType = SURF_LIQUID_RED;
		break;
	case 2:
		skinname = "heap3";
		ent->surfaceType = SURF_LIQUID_BROWN;
		break;
	default:
		skinname = "heap";
		ent->surfaceType = SURF_SAND_WHITE;
		break;
	}

	ent->spawnflags |= SF_NOPUSH;
	
	SimpleModelInit2(ent,&genericModelData[OBJ_HEAP],skinname,NULL);

}

/*QUAKED misc_generic_light_beam (1 .5 0) (-14 -313 -31) (51 142 6) INVULNERABLE   NOPUSH x x x x FLUFF
A beam of light like what comes off a car or train headlight.
------ SPAWNFLAGS ------
INVULNERABLE - N/A, can't ever be damaged.
NOPUSH - N/A, can't ever be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_light_beam (edict_t *ent)
{
	VectorSet (ent->mins, -13, -312, -30);
	VectorSet (ent->maxs,  50,  141,  5);

	ent->spawnflags |= SF_INVULNERABLE;
	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&genericModelData[OBJ_LIGHT_BEAM],NULL,NULL);

//	SimpleModelRemoveObject(ent,"WORLD_OMNI");
}

/*QUAKED misc_generic_light_flare (1 .5 0) (-5 -5 -5) (5 5 5) INVULNERABLE   NOPUSH x x x x FLUFF
A halo for lights.
------ SPAWNFLAGS ------
INVULNERABLE - N/A, can't ever be damaged.
NOPUSH - N/A, can't ever be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_light_flare (edict_t *ent)
{
	VectorSet (ent->mins, -5, -5, -5);
	VectorSet (ent->maxs,  5,  5,  5);

	ent->spawnflags |= SF_INVULNERABLE;
	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&genericModelData[OBJ_LIGHT_FLARE],NULL,NULL);

}

void motorcycle_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	SetSkin(ent,genericModelData[OBJ_MOTORCYCLE].dir,genericModelData[OBJ_MOTORCYCLE].file, 
		 genericModelData[OBJ_MOTORCYCLE].materialfile,"damaged", 0);

}

/*QUAKED misc_generic_motorcycle (1 .5 0) (-45 -18 -26) (45 21 24) INVULNERABLE   NOPUSH x x x x FLUFF
A red motorcycle.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A, can't ever be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- shows damage skin when shot
*/
void SP_misc_generic_motorcycle (edict_t *ent)
{
	VectorSet (ent->mins, -44, -17, -25);
	VectorSet (ent->maxs,  44,  20,  23);

	ent->spawnflags |= SF_NOPUSH;

	ent->pain = motorcycle_pain;

	SimpleModelInit2(ent,&genericModelData[OBJ_MOTORCYCLE],NULL,NULL);

	SetSkin(ent,genericModelData[OBJ_MOTORCYCLE].dir,
		genericModelData[OBJ_MOTORCYCLE].file, 
		genericModelData[OBJ_MOTORCYCLE].materialfile,"motorcycle", 0);

}

void cone_fall (edict_t *self)
{
	if ((self->s.angles[0] >= 100) || (self->s.angles[0] <= -100) ||
		(self->s.angles[2] >= 100) || (self->s.angles[2] <= -100))
	{
		if (self->s.angles[0] >= 100)
		{
			self->s.angles[0] = 100;
		}
		if (self->s.angles[0] <= -100)
		{
			self->s.angles[0] = -100;
		}
		if (self->s.angles[2] >= 100)
		{
			self->s.angles[2] = 100;
		}
		if (self->s.angles[2] <= -100)
		{
			self->s.angles[2] = -100;
		}
		VectorClear(self->avelocity);
		self->nextthink = 0;
		self->solid = SOLID_BBOX;
		VectorSet(self->mins, -7, -7, -7);
		VectorSet(self->maxs, 7, 7, 7);
		return;
	}
	self->avelocity[0] *= 1.5;
	self->avelocity[2] *= 1.5;
	self->health = 1;
	self->nextthink = level.time + .1;
}

void cone_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t	falldir;
	vec3_t	prelimavel;
	float	angle;

	VectorSubtract(self->s.origin, other->s.origin, falldir);
	VectorNormalize(falldir);
	angle = 360 - self->s.angle_diff;
	angle = NormalizeAngle(angle);
	angle *= DEGTORAD;
	prelimavel[0] = falldir[0] * -70;
	prelimavel[2] = falldir[1] * 70;
	self->avelocity[0] = -1 * (prelimavel[0] * cos(angle) + prelimavel[2] * sin(angle));
	self->avelocity[2] = -1 * ( prelimavel[2] * cos(angle) + prelimavel[0] * -1 * sin(angle));
	VectorClear(self->velocity);
	self->velocity[2] = 250;
	self->think = cone_fall;
	self->nextthink = level.time + .1;
	self->pain = NULL;

	SetSkin(self,genericModelData[OBJ_ORANGE_CONE].dir,genericModelData[OBJ_ORANGE_CONE].file, 
		 genericModelData[OBJ_ORANGE_CONE].materialfile,"damaged", 0);

}

/*QUAKED misc_generic_orange_cone (1 .5 0) (-7 -7 -13) (7 7 13) INVULNERABLE   NOPUSH x x x x FLUFF
An orange cone traffic cone.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- falls over when shot
*/
void SP_misc_generic_orange_cone(edict_t *ent)
{
	VectorSet (ent->mins, -6, -6, -13);
	VectorSet (ent->maxs,  6,  6,  13);

	ent->pain = cone_pain;

	SimpleModelInit2(ent,&genericModelData[OBJ_ORANGE_CONE],NULL,NULL);

	SetSkin(ent,genericModelData[OBJ_ORANGE_CONE].dir,
		genericModelData[OBJ_ORANGE_CONE].file, 
		genericModelData[OBJ_ORANGE_CONE].materialfile,"orangecone", 0);

}


/*QUAKED misc_generic_palette_jack (1 .5 0) (-30 -11 -21) (30 11 21) INVULNERABLE   NOPUSH x x x x FLUFF
A palette jack.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_palette_jack (edict_t *ent)
{
	VectorSet (ent->mins, -29, -10, -20);
	VectorSet (ent->maxs,  29,  10,  20);

	SimpleModelInit2(ent,&genericModelData[OBJ_PALETTE_JACK],NULL,NULL);
}

/*QUAKED misc_generic_paper_blowing (1 .5 0) (-3 -3 -0) (3 3 1) INVULNERABLE NOPUSH NOANIMATE x x x FLUFF
A piece of paper fluttering in the breeze.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - won't move
NOANIMATE - won't flutter
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_paper_blowing (edict_t *ent)
{
	VectorSet (ent->mins, -8, -8, -2);
	VectorSet (ent->maxs,  8,  8,  2);

	SimpleModelInit2(ent,&genericModelData[OBJ_PAPER_BLOWING],NULL,NULL);

//	if (ent->spawnflags & SF_NOANIMATE)
//		ent->ghoulInst->Pause(level.time);
}

/*QUAKED misc_generic_parking_meter (1 .5 0) (-2 -5 -22) (2 5 22) INVULNERABLE NOPUSH x x x x FLUFF
A curbside parking meter
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A can't ever be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_parking_meter (edict_t *ent)
{

	VectorSet (ent->mins, -1, -4, -21);
	VectorSet (ent->maxs,  1,  4,  21);

	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&genericModelData[OBJ_PARKING_METER],NULL,NULL);
}


void payphone_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
/*	vec3_t	forward, vPos;

	if ( (rand() % 10) < 5 )
	{
		AngleVectors(self->s.angles, forward, NULL, NULL);
		VectorMA(self->s.origin, 10, forward, vPos);
		FX_MakeCoins(vPos, forward, 0);
	}*/
	FX_PhoneHit(self->s.origin);
}

/*QUAKED misc_generic_phone_booth (1 .5 0) (-11 -28 -20) (11 28 20) INVULNERABLE NOPUSH x x x x FLUFF
A group of three phone booths
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A, won't ever move
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- rings when shot
*/
void SP_misc_generic_phone_booth (edict_t *ent)
{
	VectorSet (ent->mins, -10, -27, -19);
	VectorSet (ent->maxs,  10,  27,  19);

	ent->spawnflags |= SF_NOPUSH;
	ent->pain = payphone_pain;

	SimpleModelInit2(ent,&genericModelData[OBJ_PHONE_BOOTH],NULL,NULL);
}


/*QUAKED misc_generic_pillow (1 .5 0) (-19 -31 -3) (19 31 3) INVULNERABLE NOPUSH x x x x FLUFF
A large pillow to sit on
------ KEYS ------
Skin :
 0 - white pillow. (default)
 1 - brown pillow for sitting (sushi bar)
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - won't move
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_pillow (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -18, -30, -2);
	VectorSet (ent->maxs, 18, 30, 2);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "pillow2";
		ent->surfaceType = SURF_LIQUID_RED;
		break;
	default:
		skinname = "pillow";
		break;
	}	

	SimpleModelInit2(ent,&genericModelData[OBJ_PILLOW],skinname,NULL);

}


/*QUAKED misc_generic_plant_potted (1 .5 0) (-34 -43 -19) (24 28 44) INVULNERABLE   NOPUSH x x x x FLUFF
A large palm plant in an urn-shaped pot.
------ KEYS ------
skin -
0 - black vase w/gold band
1 - dark grey vase
2 - gold vase
3 - clay vase 
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - won't move
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_plant_potted (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -33, -42, -18);
	VectorSet (ent->maxs,  23,  27,  43);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "plant2";
		break;
	case 2:
		skinname = "plant3";
		break;
	case 3:
		skinname = "plant4";
		break;
	default:
		skinname = "plant";
		break;
	}	

	SimpleModelInit2(ent,&genericModelData[OBJ_PLANT_POTTED],skinname,NULL);
}

/*QUAKED misc_generic_plant_tall (1 .5 0) (-14 -13 -21) (14 13 21) INVULNERABLE   NOPUSH x x x x FLUFF
A little tree (waist high) in a pot. 
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - won't move
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_plant_tall (edict_t *ent)
{
	VectorSet (ent->mins, -33, -42, -18);
	VectorSet (ent->maxs,  23,  27,  43);

	SimpleModelInit2(ent,&genericModelData[OBJ_PLANT_TALL],NULL,NULL);
}


/*QUAKED misc_generic_radar_dish (1 .5 0) (-49 -36 -39) (48 37 50)  INVULNERABLE NOPUSH NOANIMATE x x x FLUFF
A radar dish that spins around and around.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - won't move
NOANIMATE - won't spin
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_radar_dish (edict_t *ent)
{
	ggBinstC	*dish;
	vec3_t		angVel = {0,0,0};

	VectorSet (ent->mins, -64, -64, -15);
	VectorSet (ent->maxs,  64,  64,  70);

	SimpleModelInit2(ent,&genericModelData[OBJ_RADAR_BASE],NULL,NULL);

	if (!ent->ghoulInst)
	{
		return;
	}
	ent->ghoulInst->Pause(level.time);

//	ent->think = radar_dish_sequence;
//	ent->nextthink = level.time + ((rand() % 5) * .2);


	dish = SimpleModelAddBolt(ent,genericModelData[OBJ_RADAR_BASE],"DISH_BOLT",
						genericModelData[OBJ_RADAR_DISH],"DISH_BOLT",NULL);
//	SetSkin2(dish->GetInstPtr(),genericModelData[OBJ_RADAR_DISH].dir,genericModelData[OBJ_RADAR_DISH].file, 
//		 genericModelData[OBJ_RADAR_DISH].materialfile,"radar_dish");

	// set the dish to rotate
	angVel[YAW] = 1.5f;
	if (dish)
	{
		FXA_SetRotate(ent, angVel, dish->GetInstPtr());
	}

//	top->nextthink = level.time + FRAMETIME*2;
//	top->think = radar_dish_spin;
}

/*QUAKED misc_generic_radio (1 .5 0) (-4 -14 -9) (4 14 9)  INVULNERABLE NOPUSH x x x x FLUFF
A boom box.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - won't move
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_radio (edict_t *ent)
{
	VectorSet (ent->mins, -4, -14, -10);
	VectorSet (ent->maxs, 4, 14, 9);

	SimpleModelInit2(ent,&genericModelData[OBJ_RADIO],NULL,NULL);
}


void security_cam_scan (edict_t *ent)
{
	trace_t			trace;
	edict_t			*player;

	// do a trace to player, if < 1, bail
	player = G_Find (NULL, FOFS(classname), "player");
	if (player)
	{
		ent->owner = player;
		gi.trace (ent->s.origin, NULL, NULL, player->s.origin, ent, ent->clipmask, &trace);
		ent->owner = NULL;
		if (trace.fraction == 1)
		{
			// determine if it is within the camera's fov, if not, bail
			if (WithinFOV(ent, player->s.origin, ent->attenuation))
			{
				// if it is, run G_UseTargets

				// Jersey wants a sound here -- replace this with a real one
				gi.sound(ent, CHAN_VOICE, gi.soundindex("Ambient/Models/Camera/Alarm.wav"), .6, ATTN_NORM, 0);
				G_UseTargets(ent, player);
			}		
		}
	}

	if (!(ent->spawnflags & SECCAM_ROTATE))	// Not rotating
	{
		ent->nextthink = level.time + 1;
	}	
}

// Looking back and forth 
void security_cam_rotate (edict_t *ent)
{			
	ent->s.angles[YAW] += ent->speed;	// Move 
	--ent->style;

	if (ent->style == 0)
	{
		ent->style = ent->count;
		ent->speed *= -1;
	}

	if (ent->spawnflags & SECCAM_TRIGGERING)
	{
		if (ent->touch_debounce_time < level.time)
		{
			security_cam_scan(ent);
			ent->touch_debounce_time = level.time + 1;
		}
	}

	ent->think = security_cam_rotate;
	ent->nextthink = level.time + FRAMETIME;

}

void security_cam_init (edict_t *ent)
{
	// Does it rotate?????
	if (ent->spawnflags & SECCAM_ROTATE)
	{
		if (!ent->speed)		// Default rotation speed
			ent->speed = 5;		

		if (!ent->count)		// Default rotation max
			ent->count = 90;	

		ent->count = (ent->count/ent->speed) * 2;	// Number of times to turn before reversing
		ent->style = ent->count /2;					// Because we're starting in the middle
		
		//gi.sound(ent, CHAN_BODY, gi.soundindex("Ambient/Models/Camera/MoveLP.wav"), .8, ATTN_NORM, 0);
		ent->s.sound =  gi.soundindex("Ambient/Models/Camera/MoveLP.wav");
		ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		ent->think = security_cam_rotate;
		ent->nextthink = level.time + FRAMETIME;
	}

	// Trigger something if it spots the player????
	if (ent->spawnflags & SECCAM_TRIGGERING)
	{
		if (!ent->attenuation)		// Default field of view
		{
			ent->attenuation = 30;
		}

		if (!(ent->spawnflags & SECCAM_ROTATE))	// Not rotating
		{
			ent->think = security_cam_scan;
			ent->nextthink = level.time + 1;
		}	
		else
		{
			ent->touch_debounce_time = level.time + 1;	// Search for player once every second
		}

	}

	ent->spawnflags |= SF_NOPUSH;
}

/*QUAKED misc_generic_security_camera (1 .5 0) (-8 -2 -4) (8 2 24) INVULNERABLE NOPUSH ROTATE TRIGGERING x x FLUFF
Ceiling mounted security camera
------ KEYS ------
count - amount to rotate to one side from current facing (count = 30 means it rotates 60 total) (default 90)
speed - degrees per second to rotate (default 5)
attenuation - the half-fov in degrees (0 to 180), defaults to 30
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A can't be pushed
ROTATE - camera will rotate on z axis
TRIGGERING - camera will trigger its target if it spots the player
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_security_camera (edict_t *ent)
{
	IGhoulInst	*camInst = NULL;

	VectorSet (ent->mins, -8, -2, -4);
	VectorSet (ent->maxs,  8,  2,  24);

	security_cam_init(ent);

	SimpleModelInit2(ent,&genericModelData[OBJ_SECURITY_CAM_POLE],NULL,"camera");
	SimpleModelScale(ent,2.0f);

	SimpleModelAddBolt(ent,genericModelData[OBJ_SECURITY_CAM_POLE],"DUMMY01",
					genericModelData[OBJ_SECURITY_CAM_CAMERA],"DUMMY01",NULL);

	if (camInst = SimpleModelGetBolt(ent, 1))
	{
		SimpleModelSetSequence2(camInst,genericModelData[OBJ_SECURITY_CAM_CAMERA].file,SMSEQ_LOOP);
	}

	//precache sounds
	gi.soundindex("Ambient/Models/Camera/Alarm.wav");
	gi.soundindex("Ambient/Models/Camera/MoveLP.wav");
}

/*QUAKED misc_generic_security_camera2 (1 .5 0) (-8 -2 -4) (8 2 4) INVULNERABLE NOPUSH ROTATE TRIGGERING x x FLUFF
Rotating security camera without mounting pole
------ KEYS ------
count - amount to rotate to one side from current facing (count = 30 means it rotates 60 total) (default 90)
speed - degrees per second to rotate (default 5)
attenuation - the half-fov in degrees (0 to 180), defaults to 30
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A can't be pushed
ROTATE - camera will rotate on z axis
TRIGGERING - camera will trigger its target if it spots the player
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_security_camera2 (edict_t *ent)
{
	VectorSet (ent->mins, -8, -2, -4);
	VectorSet (ent->maxs,  8,  2,  4);

	security_cam_init(ent);

	SimpleModelInit2(ent,&genericModelData[OBJ_SECURITY_CAM_CAMERA],NULL,"camera");
	SimpleModelScale(ent,2.0f);

	SimpleModelSetSequence(ent,genericModelData[OBJ_SECURITY_CAM_CAMERA].file,SMSEQ_LOOP);

	//precache sounds
	gi.soundindex("Ambient/Models/Camera/Alarm.wav");
	gi.soundindex("Ambient/Models/Camera/MoveLP.wav");
}

/*QUAKED misc_generic_shovel (1 .5 0) (-5 -3 -24) (5 3 24)  INVULNERABLE  NOPUSH x x x x FLUFF
Shovel, standing on its tip.  
------ KEYS ------
skin - 
0 - normal shovel
1 - showy shovel (Siberia)
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_shovel (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -5, -3, -24);
	VectorSet (ent->maxs, 5, 3, 24);

	ent->spawnflags |= SF_NOPUSH;

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "shovel2";
		break;
	default:
		skinname = "shovel";
		break;
	}	

	SimpleModelInit2(ent,&genericModelData[OBJ_SHOVEL],skinname,NULL);

}

/*QUAKED misc_generic_shrub (1 .5 0) (-20 -20 -13) (20 20 13)  INVULNERABLE  NOPUSH x x x x FLUFF
A small leafy shrub.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_shrub (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -20, -20, -13);
	VectorSet (ent->maxs,  20,  20,  13);

	ent->spawnflags |= SF_NOPUSH;

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "shrub2";
		break;
	default:
		skinname = "shrub";
		break;
	}	

	SimpleModelInit2(ent,&genericModelData[OBJ_SHRUB1],skinname,NULL);
}


void sink_spurt (edict_t *self)
{

	vec3_t fwd, backward;

	AngleVectors(self->s.angles, fwd, 0, 0);

	VectorNegate(fwd, backward);
	fxRunner.setDir(backward);
	fxRunner.exec("environ/k_waterspout3", self->s.origin);

	self->think = G_FreeEdict;
	// here, lifetime is in seconds
	//self->nextthink = level.time + lLifeTime;
	self->nextthink = level.time + 3;

}

void sink_create_water(edict_t *self)
{
	edict_t		*water;
	vec3_t		forward;


	water = G_Spawn();
	water->think = sink_spurt;
	water->nextthink = level.time + .1;
	VectorCopy(self->s.origin, water->s.origin);

	VectorCopy(self->s.angles, water->s.angles);
//	water->s.angles[1] = self->s.angle_diff; 

	AngleVectors(water->s.angles, forward, NULL, NULL);
//	VectorMA(water->s.origin, -11, forward, water->s.origin);

	water->s.sound =  gi.soundindex("Ambient/Gen/WtrFX/SprayBig.wav");
	water->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;

	water->count = 10;

	gi.linkentity (water);
}

void sink_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	vec3_t	debrisNorm;

	// ObjBecomeDebris() only uses self and attacker
	gi.sound(self, CHAN_VOICE, gi.soundindex("Impact/Break/Ceramic.wav"), .6, ATTN_NORM, 0);
	VectorClear(debrisNorm);
	FX_ThrowDebris(self->s.origin,debrisNorm, 8, DEBRIS_SM, 0, 0,0,0, 0);
//	ObjBecomeDebris(self, NULL, other, 0, vec3_origin);
	G_FreeEdict(self);	// Remove sink
}

void urinal_create_water(edict_t *self);


// drop sink from wall. sink_touch() will take care of debrisifying it
void sink_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	edict_t		*sink;
	vec3_t		vF = {0,0,0};

	AngleVectors(self->s.angles, vF, NULL, NULL);

//	if (self->health > (HLTH_GEN_SINK / 4))
//		return;

	// Create water to come gushing out of wall
	urinal_create_water(self);

	// Create sink to drop to floor
	sink = G_Spawn();

	VectorSet (sink->mins, -10, -11, -5);
	VectorSet (sink->maxs, 10, 11, 6);

	SimpleModelInit2(sink,&genericModelData[OBJ_SINK],"damaged",NULL);

	sink->health = HLTH_GEN_SINK;
	sink->s.effects |= EF_NO_BULLET;
	sink->touch = sink_touch;
	sink->material = self->material;
	sink->surfaceType = self->surfaceType;
	sink->nextthink = 0.0f;
	// appears as though the sink needs a bit of a push to fall off of the wall. restroom gravity is evidently weaker than normal gravity.
	VectorScale(vF, 10.0f, self->velocity);
//	sink->velocity[2] = -40.0f;
	VectorCopy(self->s.origin, sink->s.origin);
	VectorCopy(self->s.angles, sink->s.angles);
//	sink->s.angles[1] = self->s.angle_diff; 
	VectorMA(sink->s.origin, 5.0f, vF, sink->s.origin);
	gi.linkentity (sink);

	G_FreeEdict(self);	// Remove original sink


}

// Display damage skin on first hit
void sink_pain(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t debrisNorm;
	char *skinname = NULL;

	gi.sound(self, CHAN_VOICE, gi.soundindex("Impact/Break/Ceramic.wav"), .6, ATTN_NORM, 0);

	VectorClear(debrisNorm);
	FX_ThrowDebris(self->s.origin, debrisNorm, 5, DEBRIS_SM, 0, 0,0,0, 0);

	switch (self->s.skinnum)
	{
	case 1:
		skinname = "sinkclean2d";
		break;
	case 2:
		skinname = "damaged";
		break;
	default:
		skinname = "sinkclean2d";
		break;
	}	

	SetSkin(self, genericModelData[OBJ_SINK].dir, genericModelData[OBJ_SINK].file,
		genericModelData[OBJ_SINK].materialfile, skinname, 0);
	self->pain = NULL;
}

/*QUAKED misc_generic_sink (1 .5 0) (-11 -12 -6) (11 12 7) INVULNERABLE  NOPUSH x x x x FLUFF
A wall mounted porcelain sink.
_______KEYS________
skin - 
1 - clean skin
2 - dirty skin

------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A,can't ever be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- falls off wall and spouts water when shot
*/
void SP_misc_generic_sink (edict_t *ent)
{
	char *skinname = NULL;

	VectorSet (ent->mins, -11, -12, -6);
	VectorSet (ent->maxs, 11, 12, 7);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "sinkclean";
		break;
	case 2:
		skinname = "sink";
		break;
	default:
		skinname = "sinkclean";
		break;
	}	

	ent->spawnflags |= SF_NOPUSH;

	ent->pain = sink_pain;
	ent->die = sink_die;
	ent->s.effects |= EF_NO_BULLET;

	SimpleModelInit2(ent,&genericModelData[OBJ_SINK],NULL,NULL);
	SetSkin(ent, genericModelData[OBJ_SINK].dir, genericModelData[OBJ_SINK].file,
		genericModelData[OBJ_SINK].materialfile, skinname, 0);

	gi.soundindex("Ambient/Gen/WtrFX/SprayBig.wav"); // precache sound
	gi.soundindex("Impact/Break/Ceramic.wav"); // precache sound
	gi.effectindex("environ/k_waterspout3");
}


void sofa_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t	pathdir;
	vec3_t	up;
//	vec3_t	towherehit;
//	vec3_t	cross;

	up[0] = 0;
	up[1] = 0;
	up[2] = 1;

	self->health = 10000;
	VectorSubtract(self->s.origin, other->s.origin, pathdir);
	VectorNormalize(pathdir);
	VectorMA(self->velocity, damage*2, pathdir, self->velocity);
	// since the vel. is influenced by damage, need to remember to tweak this base
	// on final weapon damages
	self->velocity[2] = 20;
//	VectorSubtract(wherehit,self->s.origin, towherehit);
//	This next was the old,cool rotating stuff...  <sigh>
//	VectorScale(pathdir, -1, pathdir);
/*	CrossProduct(pathdir, towherehit, cross);
	if (DotProduct(cross, up) > 0)
	{	
		self->avelocity[1] = (rand()%(40*damage)); 
	}
	else if (DotProduct(cross, up) < 0)
	{
		self->avelocity[1] = (rand()%(40*damage))*-1; 
	}
*/
	wherehit[0] += ((rand()%21)*.1) - 1;
	wherehit[1] += ((rand()%21)*.1) - 1;
	wherehit[2] += ((rand()%21)*.1) - 1;

	FX_MakeDustPuff(wherehit);
}

/*QUAKED misc_generic_sofa (1 .5 0) (-17 -36 -17) (17 36 17) INVULNERABLE  NOPUSH x x x x FLUFF
A sofa.
------ KEYS ------
skin - 
0 - dirty green skin
1 - fancy white leather skin
2 - brown cloth with a design
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- emits smoke puffs when shot
*/
void SP_misc_generic_sofa (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -17, -36, -17);
	VectorSet (ent->maxs,  17,  36,  17);

	ent->pain = sofa_pain;
	ent->friction = 1.0;
	
	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "sofa2";
		break;
	case 2:
		skinname = "sofa3";
		break;
	default:
		skinname = "sofa";
		break;
	}	

	SimpleModelInit2(ent,&genericModelData[OBJ_SOFA],skinname,NULL);

}

/*QUAKED misc_generic_sofa_chair (1 .5 0) (-17 -16 -17) (17 16 17) INVULNERABLE  NOPUSH x x x x FLUFF
A padded recliner-type chair
------ KEYS ------
skin - 
0 - brown with buttons
1 - brown cloth with a design
2 - brown with no buttons
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- emits smoke puffs when shot
*/

void SP_misc_generic_sofa_chair (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -17, -16, -17);
	VectorSet (ent->maxs,  17,  16,  17);

	ent->spawnflags |= SF_NOPUSH;

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "chair2";
		break;
	case 2:
		skinname = "chair2a";
		break;
	default:
		skinname = "chair";
		break;
	}	

	SimpleModelInit2(ent,&genericModelData[OBJ_SOFA_CHAIR],skinname,NULL);

	ent->pain = sofa_pain;
	ent->friction = 1.0;
}



void speaker_spark (edict_t *self)
{
	vec3_t dir, holdorigin;
	vec3_t		forward,right;
	
	if (self->health == 0)
	{
		self->nextthink = 0;
	}
	else
	{
		dir[0] = 0;
		dir[1] = 0;
		dir[2] = 1;
		self->health--;

		AngleVectors(self->s.angles, forward, right, NULL);
		VectorMA(self->s.origin, 14, forward, holdorigin);
		VectorMA(holdorigin, 4, right, holdorigin);

		FX_MakeSparks(holdorigin, dir, 0);
		T_RadiusDamage (self, self->owner, 20.0, NULL, 10, MOD_EXPLOSIVE);
		self->nextthink = level.time + (((rand()%30) * .1) + .1);
	}
}

void speaker_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	vec3_t			pathdir;
	float			sign;
	char			*dir = "objects/generic/speaker";
	char			*file = "speaker";
	IGhoulInst*		speakerInst = NULL;
	Matrix4			mat1;
	vec3_t			fwd, right;

	self->movetype = MOVETYPE_DAN;	// Can be pushed around

	sign = (rand()&1) ? 1:-1;
	VectorSubtract(self->s.origin, inflictor->s.origin, pathdir);
	VectorNormalize(pathdir);
	VectorMA(self->velocity, damage*5, pathdir, self->velocity);
	self->velocity[2] = 0;
//	self->avelocity[0] = sign * (100 + (rand()%100) + (damage*15));
	self->avelocity[1] = sign * (100 + (rand()%100) + (damage));
//	self->avelocity[2] = sign * (100 + (rand()%100) + (damage*15));
	self->gravity = 1;

	FX_MakeSparks(self->s.origin, pathdir, 0);
	// explosion is really just to get the sound
	FX_LittleExplosion(self->s.origin, 10, 1);	

	// help it away from the wall
	AngleVectors(self->s.angles,fwd,right,NULL);
	VectorMA(self->s.origin, 10, fwd, self->s.origin);

	self->think = speaker_spark;
	self->health = (rand()%10);
	if (self->health < 4)
		self->health = 4;

	self->nextthink = level.time;
	self->takedamage = DAMAGE_NO;
	self->owner = attacker;

	SimpleModelSetSequence(self,"speaker_down",SMSEQ_LOOP);

	if (!(self->spawnflags & SPEAKER_WALL))
	{
		// shrink our bbox a bit
		self->mins[2] = -5;
	}
}

/*QUAKED misc_generic_speaker (1 .5 0) (-3 -5 -10) (7 5 13)  INVULNERABLE  NOPUSH WALLMOUNT x x x FLUFF
A speaker hung from ceiling.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A,can't ever be pushed
WALLMOUNT - removes pole 
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- sparks and falls when shot
- can remove wallmount using WALLMOUNT spawnflag
*/
void SP_misc_generic_speaker (edict_t *ent)
{
	VectorSet (ent->mins, -5, -5, -12);
	VectorSet (ent->maxs, 5, 5, 12);

	ent->spawnflags |= SF_NOPUSH;

	ent->die = speaker_die;

	SimpleModelInit2(ent,&genericModelData[OBJ_SPEAKER],NULL,NULL);

	if (ent->spawnflags & SPEAKER_WALL) //remove pole
	{
		SimpleModelRemoveObject(ent,"_ARM01");
		// shrink our bbox a bit
		ent->mins[2] = -5;
	}

	// cache sound CLSFX_EXP_SMALL
	entSoundsToCache[CLSFX_EXP_SMALL] = 1;
}

// FIXME :this needs an effect on it.
void stoplight_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	SimpleModelSetSequence(self,"stoplight",SMSEQ_HOLD);
	SetSkin(self,genericModelData[OBJ_STOPLIGHT].dir,genericModelData[OBJ_STOPLIGHT].file, 
		 genericModelData[OBJ_STOPLIGHT].materialfile,"stoplight_d", 0);

	self->think = NULL;
	self->nextthink = NULL;
}

void stoplight_change(edict_t *ent)
{
	char *seqname;

	++ent->count;

	if (ent->count == 1)
		seqname = "blue";
	else if (ent->count == 2)
		seqname = "red";
	else 
	{
		ent->count=0;
		seqname = "yellow";
	}

	SimpleModelSetSequence(ent,seqname,SMSEQ_HOLD);

	ent->think = stoplight_change;
	ent->nextthink = level.time + 5;
}

/*QUAKED misc_generic_stoplight (1 .5 0)(-6 -5 -44) (9 4 44)  INVULNERABLE  NOPUSH x x x x FLUFF
A traffic light red - yellow - green, y'know. 
------ KEYS ------
skin - 
0 - clean
1 - damaged
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A,can't ever be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- changes to damage skin when shot
*/
void SP_misc_generic_stoplight (edict_t *ent)
{
	char *seqname;
	char *skinname;


	VectorSet (ent->mins, -6, -5, -44);
	VectorSet (ent->maxs,  9,  4,  44);

	ent->spawnflags |= SF_NOPUSH;

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "stoplight_d";
		break;
	default:
		skinname = "stoplight";
		break;
	}	

	SimpleModelInit2(ent,&genericModelData[OBJ_STOPLIGHT],NULL,NULL);

	if (ent->s.skinnum)
	{
		SetSkin(ent,genericModelData[OBJ_STOPLIGHT].dir,genericModelData[OBJ_STOPLIGHT].file, 
			 genericModelData[OBJ_STOPLIGHT].materialfile,"stoplight_d", 0);
	}
	else 
	{
		ent->count=0;

		seqname = "yellow";

		SimpleModelSetSequence(ent,seqname,SMSEQ_HOLD);

		ent->pain = stoplight_pain;
		ent->think = stoplight_change;
		ent->nextthink = level.time + 5;
	}
}


/*QUAKED misc_generic_street_name_sign (1 .5 0) (-12 -12 -36) (12 12 36)  INVULNERABLE  NOPUSH x x x x FLUFF
A corner street sign.
------ KEYS ------
skin - 
0 - Russian sign
1 - American sign
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't ever be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_street_name_sign (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -12, -12, -36);
	VectorSet (ent->maxs,  12,  12,  36);

	ent->spawnflags |= SF_NOPUSH;

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "sign2";
		break;
	default:
		skinname = "sign";
		break;
	}	

	SimpleModelInit2(ent,&genericModelData[OBJ_STREETSIGN],skinname,NULL);

}

/*QUAKED misc_generic_sword (1 .5 0) (-23 -2 -2) (23 2 2)  INVULNERABLE  NOPUSH x x x x FLUFF
A katana
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't ever be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_sword (edict_t *ent)
{
	VectorSet (ent->mins, -23, -2, -2);
	VectorSet (ent->maxs,  23,  2,  2);

	SimpleModelInit2(ent,&genericModelData[OBJ_SWORD],NULL,NULL);
}

/*QUAKED misc_generic_sword_large (1 .5 0) (-29 -2 -2) (29 2 2)  INVULNERABLE  NOPUSH x x x x FLUFF
A large katana
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't ever be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_sword_large (edict_t *ent)
{
	VectorSet (ent->mins, -29, -2, -2);
	VectorSet (ent->maxs,  29,  2,  2);

	SimpleModelInit2(ent,&genericModelData[OBJ_SWORD_LARGE],NULL,NULL);
	SimpleModelScale(ent,1.30f);
}

/*QUAKED misc_generic_table_round (1 .5 0)  (-27 -23 -16) (27 23 16)  INVULNERABLE  NOPUSH x x x x FLUFF
Octagonal table with a support in the middle.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't ever be pushed
FLUFF - won't show if gl_pictip is set
------ KEYS ------
skin - 
0 - metal table (default)
1 - wooden table
*/
void SP_misc_generic_table_round (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -27, -23, -16);
	VectorSet (ent->maxs,  27,  23,  16);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "table2";
		ent->surfaceType = SURF_WOOD_LBROWN;
		ent->material = MAT_WOOD_LBROWN;
		break;
	default:
		skinname = "table";
		ent->surfaceType = SURF_METAL;
		ent->material = MAT_METAL_SHINY;
		break;
	}	

	SimpleModelInit2(ent,&genericModelData[OBJ_TABLE_ROUND],skinname,NULL);

	VectorSet (ent->mins, -27, -23, -16);
	VectorSet (ent->maxs,  27,  23,  16);

	gi.linkentity (ent);

}


void table1_flat(edict_t *self)
{
	self->s.angles[ROLL] = self->moveinfo.start_angles[ROLL];	
	self->s.angles[PITCH] = self->moveinfo.start_angles[PITCH];	
	VectorSet (self->mins, -41, -24, 10);
	VectorSet (self->maxs,  41,  24, 15);
			
	self->spawnflags &= ~SF_NOPUSH;
	gi.linkentity (self);
}

void table1_rollright(edict_t *self)
{
	self->s.angles[ROLL] += 30;

	self->s.angles[PITCH] = self->moveinfo.start_angles[PITCH];

	VectorSet (self->mins, -41, -24, -7);
	VectorSet (self->maxs,  41,  24,  16);
			
	gi.linkentity (self);
}


void table1_rollleft(edict_t *self)
{
	self->s.angles[ROLL] -= 30;

	self->s.angles[PITCH] = self->moveinfo.start_angles[PITCH];

	VectorSet (self->mins, -41, -24, -7);
	VectorSet (self->maxs,  41,  24,  16);
			
	gi.linkentity (self);
}

void table1_pitchforward(edict_t *self)
{
	if (self->s.angles[ROLL] != self->moveinfo.start_angles[ROLL])	// Already laying on the long edge
		return;

	self->s.angles[PITCH] += 20;

	VectorSet (self->mins, -41, -24, -7);
	VectorSet (self->maxs,  41,  24,  16);
			
	gi.linkentity (self);
}

void table1_pitchbackward(edict_t *self)
{
	if (self->s.angles[ROLL] != self->moveinfo.start_angles[ROLL])	// Already laying on the long edge
		return;

	self->s.angles[PITCH] -= 20;

	VectorSet (self->mins, -41, -24, -7);
	VectorSet (self->maxs,  41,  24,  16);
			
	gi.linkentity (self);
}

// called when a part is shot off of the table
void table1_legpain (edict_t *self,int partLost, vec3_t boltPos)
{
	bool		bTwoLegs = false, B1, B2, B3, B4, B5;
	boltInstInfo_c *boltInfo = NULL;
	vec3_t		debrisNorm;

	if (!self->objInfo || !(boltInfo = (boltInstInfo_c*)self->objInfo->GetInfo(OIT_BOLTINST)) )
	{
		return;
	}
	B1 = boltInfo->IsOn(1);
	B2 = boltInfo->IsOn(2);
	B3 = boltInfo->IsOn(3);
	B4 = boltInfo->IsOn(4);
	B5 = boltInfo->IsOn(5);

	// All other legs there?
	if ((B1) && (B2) && (B3))
		return;

	if ((B1) && (B2) && (B4))
		return;

	if ((B1) && (B3) && (B4))
		return;

	if ((B2) && (B3) && (B4))
		return;

	VectorClear(debrisNorm);

	// Throw debris and make it go away
	FX_ThrowDebris(boltPos,debrisNorm, 5, DEBRIS_SM, self->material, 0,0,0, self->surfaceType);
	FX_SmokePuff(boltPos,120,120,120,200);

	// All legs and front gone??
	if ((!B1) && (!B2) && (!B3) && (!B4) && (!B5))
	{
		table1_flat(self);
		return;
	}


	switch (partLost)
	{
	case(1):
		// Both back legs are gone?
		if ((!B1) && (!B2))
		{
			table1_rollright(self);
		}
		// Both back legs are gone?
		else if ((!B1) && (!B3)  && (!B5))
		{
			table1_pitchbackward(self);
		}
		break;
	case(2):
		// Both back legs are gone?
		if ((!B1) && (!B2))
		{
			table1_rollright(self);
		}
		else if ((!B2) && (!B4) && (!B5))
		{
			table1_pitchforward(self);
		}
		break;
	case(3):
		if ((!B1) && (!B3)  && (!B5))
		{
			table1_pitchbackward(self);
		}
		else if ((!B3) && (!B4)  && (!B5))
		{
			table1_rollleft(self);
		}
		break;
	case(4):
		if ((!B2) && (!B4)  && (!B5))
		{
			table1_pitchforward(self);
		}
		else if ((!B3) && (!B4)  && (!B5))
		{
			table1_rollleft(self);
		}
		break;
	case(5):
		if ((!B1) && (!B3)  && (!B5))
		{
			table1_pitchbackward(self);
		}
		else if ((!B2) && (!B4)  && (!B5))
		{
			table1_pitchforward(self);
		}
		else if ((!B3) && (!B4)  && (!B5))
		{
			table1_rollleft(self);
		}

	default:
		break;
	}
}

void tabletouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{	// the purpose of this fn is to find all of the objects resting on the table
	//and store them in a linked list of sorts
	objInfo_c	*infoPtr = self->objInfo?self->objInfo->GetInfo(OIT_BOLTINST):NULL;

	if (other->client || other->ai)
	{	// we sure as heck don't want to add the player or an enemy to our list
		return;
	}
	if (NULL == infoPtr)
	{	//	nowhere to store information
		return;
	}
	if (infoPtr->head == self)
	{	// ooh, I've got an idea. let's _not_ go into an infinite loop.
		return;
	}
	if (other == world)
	{	// ...and let's talk about how bad it would be to think that the worldspawn 
		//landed on top of this table...
		return;
	}
	while (infoPtr->head != NULL)
	{	// if this head _is_ other, we've already added other to our list so exit
		if (infoPtr->head == other)
		{
			return;
		}
		// find the next node in the list
		if (NULL == infoPtr->head->objInfo)
		{	// next node doesn't have an objInfo
			return;
		}
		infoPtr = infoPtr->head->objInfo->GetInfo(OIT_BOLTINST);
	}
	// with any luck at all, getting here means we have a valid infoPtr->head which we can
	//use like a "next" pointer in a linked list. sheesh. stupid tables.
	infoPtr->head = other;
}

/*QUAKED misc_generic_table1 (1 .5 0) (-41 -24 -18) (41 24 15)  INVULNERABLE  NOPUSH PLAYER_FLIPPABLE x x x FLUFF
A rectangular table with four legs and a front panel.
------ KEYS ------
Skin :
0 - dirty wooden table. (default)
1 - high tech metal/white table
2 - dirty wooden table with stuff on it
3 - high tech table with stuff on it
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't ever be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- falls apart when shot
*/
void SP_misc_generic_table1 (edict_t *ent)
{
	char *skinname;

	// Set up object to break apart when shot
	Obj_partbreaksetup (BPD_TABLE1,&genericModelData[OBJ_TABLE1],
		genericObjBoltPartsData,genericObjBreak);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "table1A";
		ent->surfaceType = SURF_METAL;
		ent->material = MAT_METAL_SHINY;
		break;
	case 2:
		skinname = "table1B";
		ent->surfaceType = SURF_WOOD_LBROWN;
		ent->material = MAT_WOOD_LBROWN;
		break;
	case 3:
		skinname = "table1C";
		ent->surfaceType = SURF_METAL;
		ent->material = MAT_METAL_SHINY;
		break;
	default:
		skinname = "table1";
		ent->surfaceType = SURF_WOOD_LBROWN;
		ent->material = MAT_WOOD_LBROWN;
		break;
	}	

	SimpleModelInit2(ent,&genericModelData[OBJ_TABLE1],skinname,NULL);

	VectorSet (ent->mins, -41, -24, -18);
	VectorSet (ent->maxs,  41,  24,  10);

	gi.linkentity (ent);

	// These are given as you face the front of the desk :
	// Bolt1 - back right leg
	SimpleModelAddBolt(ent,genericModelData[OBJ_TABLE1],"DUMMY01",
						genericModelData[OBJ_TABLE1_LEG],"DUMMY01",skinname);
	// Bolt2 - back left leg
	SimpleModelAddBolt(ent,genericModelData[OBJ_TABLE1],"DUMMY02",
						genericModelData[OBJ_TABLE1_LEG],"DUMMY01",skinname);
	// Bolt3 - front left leg
	SimpleModelAddBolt(ent,genericModelData[OBJ_TABLE1],"DUMMY03",
						genericModelData[OBJ_TABLE1_LEG],"DUMMY01",skinname);
	// Bolt4 - front right leg
	SimpleModelAddBolt(ent,genericModelData[OBJ_TABLE1],"DUMMY04",
						genericModelData[OBJ_TABLE1_LEG],"DUMMY01",skinname);
	// Bolt5 - front panel
	SimpleModelAddBolt(ent,genericModelData[OBJ_TABLE1],"DUMMY05",
						genericModelData[OBJ_TABLE1_BACK],"DUMMY05",skinname);
	ent->pain = Obj_partpain;
	VectorCopy(ent->s.angles,ent->moveinfo.start_angles);

	// do we want the player to be able to flip this object 90 degrees?
	if (ent->spawnflags & PLAYER_FLIPPABLE)
	{
		ent->plUse = FlipObject;
	}

	ent->touch = tabletouch;
}


void table2_flat(edict_t *self)
{
	self->s.angles[ROLL] = self->moveinfo.start_angles[ROLL];	
	self->s.angles[PITCH] = self->moveinfo.start_angles[PITCH];	

	VectorSet (self->mins, -64, -106, -1);
	VectorSet (self->maxs,  47,  32,  4);

	gi.linkentity (self);
}

void table2_shorttopbreak(edict_t *self)
{
	Matrix4			matOld,mat2,mat1;
	IGhoulInst*		TopInst;

	if (TopInst = SimpleModelGetBolt(self, 5))
	{
		TopInst->GetXForm(matOld);
		mat1=mat2=matOld;
		mat2.Rotate(1,-TOP_FALL);
		matOld.Concat(mat1, mat2);
		TopInst->SetXForm(matOld);
	}
	self->s.angles[0] += 8;
}

void table2_longtopbreak(edict_t *self)
{
	self->s.angles[ROLL] += 18;
}

void table2_roll1(edict_t *self)
{
	self->s.angles[PITCH] += 18;
}

// Long side panels are gone, tip forward
void table2_roll2(edict_t *self)
{
	self->s.angles[PITCH] -= 12;
	self->s.angles[ROLL] -= 12;

	VectorSet (self->mins, -64, -106, -16);
	VectorSet (self->maxs,  47,  32,  4);

	gi.linkentity (self);

}

// called when a part is shot off of the table
void table2_legpain (edict_t *self,int partLost, vec3_t boltPos)
{
	bool			bTwoLegs = false, B1, B2, B3, B4;
	vec3_t			debrisNorm;
	boltInstInfo_c	*boltInfo = NULL;

	if (!self->objInfo || !(boltInfo = (boltInstInfo_c*)self->objInfo->GetInfo(OIT_BOLTINST)) )
	{
		return;
	}
	B1 = boltInfo->IsOn(1);
	B2 = boltInfo->IsOn(2);
	B3 = boltInfo->IsOn(3);
	B4 = boltInfo->IsOn(4);

	// Bolt1 - Short side panel on short length of table
	// Bolt2 - Long side panel 
	// Bolt3 - Medium side panel 
	// Bolt4 - Short side panel on long length of table
	// Bolt5 - Short table top

	// Throw debris and make it go away
	VectorClear(debrisNorm);
	FX_ThrowDebris(boltPos,debrisNorm, 5, DEBRIS_SM, self->material, 0,0,0, self->surfaceType);
	FX_SmokePuff(boltPos,120,120,120,200);

	// All legs gone??
	if ((!B1) && (!B2) && (!B3) && (!B4))
	{
		table2_flat(self);
		return;
	}

	switch (partLost)
	{
	case(1):	// Bolt1 - Short side panel on short length of table
		if ((!B2) && (!B3) && (B4))
		{
			table2_roll1(self);
		}
		else if ((!B3) && (B2) && (B4))
		{
			table2_shorttopbreak(self);
		}

		break;
	case(2):	// Bolt2 - Long side panel 
		if ((B1) && (!B2)  && (!B3) && (B4))
		{
			table2_roll2(self);
		}

		break;
	case(3):	// Bolt3 - Medium side panel
		if ((!B1) && (B2) && (B4))
		{
			table2_shorttopbreak(self);
		}
		else if ((B1) && (!B2)  && (!B3) && (B4))
		{
			table2_roll2(self);
		}
		break;
	case(4):	// Bolt4 - Short side panel on long length of table
		if ((!B2) && (B1) && (B3))
		{
			table2_longtopbreak(self);
		}
		break;
	default:
		break;
	}
}



/*QUAKED misc_generic_table2 (1 .5 0) (-64 -106 -30) (47 32 4)  INVULNERABLE  NOPUSH x x x x FLUFF
An 'L' shaped table with panels on four sides.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't ever be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- falls apart when shot
*/
void SP_misc_generic_table2 (edict_t *ent)
{
	IGhoulInst*		PanelInst;
	Matrix4			matOld,mat2,mat1;

	// Set up object to break apart when shot
	Obj_partbreaksetup (BPD_TABLE2,&genericModelData[OBJ_TABLE2],
		genericObjBoltPartsData,genericObjBreak);

	if (!st.scale)
		st.scale = .80f;	// Scaled down normally
	else if (st.scale > .20f)
		st.scale -= .20f;

//	VectorSet (ent->mins, -64, -106, -26);
//	VectorSet (ent->maxs,  47,  32,  4);

	VectorSet (ent->mins, -56, -98, -26);
	VectorSet (ent->maxs,  39,  24,  4);

	SimpleModelInit2(ent,&genericModelData[OBJ_TABLE2],NULL,NULL);

	gi.linkentity (ent);

	// Bolt1 - Short side panel on short length of table
	SimpleModelAddBolt(ent,genericModelData[OBJ_TABLE2],"DUMMY08",
						genericModelData[OBJ_TABLE2_SIDE1],"DUMMY08",NULL);
	// Bolt2 - Long side panel 
	SimpleModelAddBolt(ent,genericModelData[OBJ_TABLE2],"DUMMY09",
						genericModelData[OBJ_TABLE2_SIDE2],"DUMMY09",NULL);
	// Bolt3 - Medium side panel 
	SimpleModelAddBolt(ent,genericModelData[OBJ_TABLE2],"DUMMY10",
						genericModelData[OBJ_TABLE2_SIDE3],"DUMMY10",NULL);
	// Bolt4 - Short side panel on long length of table
	SimpleModelAddBolt(ent,genericModelData[OBJ_TABLE2],"DUMMY07",
						genericModelData[OBJ_TABLE2_SIDE1],"DUMMY08",NULL);
	// Bolt5 - Short table top
	SimpleModelAddBolt(ent,genericModelData[OBJ_TABLE2],"DUMMY12",
						genericModelData[OBJ_TABLE2_TOP2],"DUMMY12",NULL);
	// Rotating side panel 
	if (PanelInst = SimpleModelGetBolt(ent, 4))
	{
		PanelInst->GetXForm(matOld);
		mat1=mat2=matOld;
		mat2.Rotate(0,-M_PI*.50);
		matOld.Concat(mat1, mat2);
		PanelInst->SetXForm(matOld);
	}
	ent->pain = Obj_partpain;
	ent->touch = tabletouch;

}


void tank_gas_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	self->die = NULL;
	self->pain = NULL;
	Obj_explode(self,&self->s.origin,1,1);
	ObjBecomeDebris(self,inflictor,attacker,damage,point);
}

void tank_gas_use (edict_t *self, edict_t *other, edict_t *activator)
{
	tank_gas_die(self,other,activator,100,self->s.origin);
}


/*QUAKED misc_generic_tank_gas (1 .5 0) (-58 -23 -27) (58 23 19)  INVULNERABLE  NOPUSH x x x x FLUFF
A big natural gas tank.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't ever be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- when 'used' it blows up, even when invulnerable.
- blows up when shot
*/
void SP_misc_generic_tank_gas (edict_t *ent)
{
	VectorSet (ent->mins, -58, -23, -27);
	VectorSet (ent->maxs,  58,  23,  19);

	SimpleModelInit2(ent,&genericModelData[OBJ_TANK_GAS],NULL,NULL);

	ent->die = tank_gas_die;
	ent->use = tank_gas_use;

	// cache sound CLSFX_EXPLODE
	entSoundsToCache[CLSFX_EXPLODE] = 1;

	// precache explosion effect
	gi.effectindex("weapons/world/airexplode");
	gi.effectindex("environ/onfireburst");
}


void tank_propane_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	self->die = NULL;
	self->pain = NULL;
	Obj_explode(self,&self->s.origin,1,1);

	if (self->spawnflags && TANK_PROPANE_NODEBRIS)
	{
		G_FreeEdict(self);
	}	
	else
	{
		BecomeDebris(self,inflictor,attacker,damage,point);
	}
}

/*QUAKED misc_generic_tank_propane (1 .5 0) (-3 -3 -25) (3 3 25)  INVULNERABLE  NOPUSH  NODEBRIS x x x FLUFF
A tall thin propane tank.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't ever be pushed
NODEBRIS - won't produce debris when killed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_tank_propane (edict_t *ent)
{
	VectorSet (ent->mins, -3, -3, -25);
	VectorSet (ent->maxs,  3,  3,  25);

	SimpleModelInit2(ent,&genericModelData[OBJ_TANK_PROPANE],NULL,NULL);

	ent->die = tank_propane_die;


	// cache sound CLSFX_EXPLODE
	entSoundsToCache[CLSFX_EXPLODE] = 1;
	gi.effectindex("environ/onfireburst");
}

void toilet_pulse (edict_t *self)
{

	if (self->health <= 0)
	{
		G_FreeEdict(self);
		return;
	}
}

void toilet_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	edict_t			*water;
	vec3_t			pos;

	water = G_Spawn();
	water->health = 50;
	water->takedamage = DAMAGE_NO;
	water->think = G_FreeEdict;
	water->nextthink = level.time + 3.0;// estimated length of environ/toiletspout.eft
	VectorCopy(self->s.origin, water->s.origin);

	VectorCopy(self->s.angles, water->s.angles);

	water->s.sound =  gi.soundindex("Ambient/Gen/WtrFX/SprayBig.wav");
	water->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;

	water->count = 10;

	VectorCopy(self->s.origin, pos);
	fxRunner.exec("environ/toiletspout", pos);
	gi.linkentity (water);

	ObjBecomeDebris(self, NULL, self, 0, vec3_origin);
}

void toilet_dribble (edict_t *self)
{
	self->nextthink = level.time + .1 * (rand()%5);
}

void toilet_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t				pathdir, offset, vel;
	vec3_t				accel = {0,0,-800};
	
	VectorSubtract(other->s.origin, self->s.origin, pathdir);
	VectorNormalize(pathdir);
	VectorScale(pathdir, gi.irand(90,111), pathdir);
	pathdir[2] += 150;
	VectorCopy(pathdir, vel);
	pathdir[0] = (pathdir[0]/4) + 128;
	pathdir[1] = (pathdir[1]/4) + 128;
	pathdir[2] = (pathdir[2]/4) + 128;


	VectorSubtract(wherehit,self->s.origin, offset);

	SetSkin(self,genericModelData[OBJ_TOILET].dir,genericModelData[OBJ_TOILET].file, 
		 genericModelData[OBJ_TOILET].materialfile,"damaged", 0);

			
	self->think = toilet_dribble;
	self->nextthink = level.time + .5;
	self->health = 1;
}

/*QUAKED misc_generic_toilet (1 .5 0) (-14 -9 -16) (14 9 16)  INVULNERABLE  NOPUSH x x x x FLUFF
A toilet
------ KEYS ------
Skin :
 0 - dirty New York. (default)
 1 - clean Tokyo
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't ever be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- produces fountain of water when shot.
*/
void SP_misc_generic_toilet (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -14, -9, -16);
	VectorSet (ent->maxs,  14,  9,  16);

	ent->pain = toilet_pain;
	ent->die = toilet_die;

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "toiletclean";
		break;
	default:
		skinname = "toilet";
		break;
	}	

	SimpleModelInit2(ent,&genericModelData[OBJ_TOILET],NULL,NULL);
	SetSkin(ent,genericModelData[OBJ_TOILET].dir,genericModelData[OBJ_TOILET].file, 
		 genericModelData[OBJ_TOILET].materialfile,skinname, 0);
	ent->movetype = MOVETYPE_NONE;

	gi.soundindex("Ambient/Gen/WtrFX/SprayBig.wav"); // precache sound
	gi.effectindex("environ/toiletspout");
}


/*QUAKED misc_generic_train (1 .5 0) (-54 -164 -60) (55 168 60)  TRIGGERED RESPAWNER START_ACTIVE DECCELERATE DISAPPEAR IDLE FLUFF
A train car.
------ KEYS ------
accel - rate of acceleration (default 100)
skin - 
0 - modern graffitti covered subway car
1 - old-fashioned subway
2 - cattle car
3 - box car
style - maximum speed (default 600)
------ SPAWNFLAGS ------
TRIGGERED - set if train is to respond to a trigger.
RESPAWNER - set if you want the train to disappear when it stops and respawn.
When set, "wait" is the time in seconds that will pass before the train respawns
after disappearing.
START_ACTIVE - set the train is to begin the level moving, as opposed to being triggered.
DECCELERATE will cause the train to slow down instead of instantly stopping
DISAPPEAR - go away when train stops.
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- train moves
*/

void train_go (edict_t *self, edict_t *other, edict_t *activator);

void train_stop (edict_t *self)
{
	GrabStuffOnTop (self);
	VectorClear(self->velocity);
	self->s.sound = 0;
	self->nextthink = -1;
	if (self->spawnflags & TRAIN_RESPAWNER)
	{
		InitiateRespawn(self);
		G_FreeEdict(self);
	}

	if ((self->spawnflags & TRAIN_DISAPPEAR) && !(self->spawnflags & TRAIN_RESPAWNER))
	{
		G_FreeEdict(self);
	}
}

void train_deccel (edict_t *self)
{
	GrabStuffOnTop (self);

	float	distanceLeft;
	float	thisDist;
	float	xdiff, ydiff;
	float	curSpeed;
	vec3_t	newVel;

	xdiff = self->s.origin[0] - self->volume;
	ydiff = self->s.origin[1] - self->attenuation;
	distanceLeft = sqrt(xdiff*xdiff + ydiff*ydiff ) - 167; // -167 due to removing the box from the trace
	
	curSpeed = sqrt(self->velocity[0]*self->velocity[0] + self->velocity[1]*self->velocity[1]);
	thisDist = curSpeed * .1;

	if (curSpeed < 20)
	{
			newVel[0] = self->velocity[0] * (distanceLeft/thisDist);	
			newVel[1] = self->velocity[1] * (distanceLeft/thisDist);	
			newVel[2] = 0;
			VectorCopy(newVel, self->velocity);
			self->think = train_stop;
			self->nextthink = level.time + .1;
			return;
	}
			
	newVel[0] = self->velocity[0] * (distanceLeft/((TRAIN_BRAKE_FACTOR+1)*thisDist));	
	newVel[1] = self->velocity[1] * (distanceLeft/((TRAIN_BRAKE_FACTOR+1)*thisDist));	
	newVel[2] = 0;
	VectorCopy(newVel, self->velocity);
	self->nextthink = level.time + .1;
}

qboolean EndofLineCheck (edict_t *self)
{
	float	distanceLeft;
	float	thisDist;
	float	xdiff, ydiff;
	float	curSpeed;

	xdiff = self->s.origin[0] - self->volume;
	ydiff = self->s.origin[1] - self->attenuation;
	distanceLeft = sqrt(xdiff*xdiff + ydiff*ydiff ) - 167; // -167 due to removing the box from the trace
	
	curSpeed = sqrt(self->velocity[0]*self->velocity[0] + self->velocity[1]*self->velocity[1]);
	thisDist = curSpeed * .1;
//	thisDist = curSpeed * .1;
	
	if (VectorCompare(vec3_origin, self->velocity))
	{
		self->think = train_stop;
		self->nextthink = level.time + .1;
		return true;
	}


	if (self->spawnflags & TRAIN_DECCELERATE)
	{
		if (distanceLeft < (TRAIN_BRAKE_FACTOR*thisDist))
		{
			vec3_t			newVel;
		
			//adjust velocity then stop
			newVel[0] = self->velocity[0] * (distanceLeft/((TRAIN_BRAKE_FACTOR+1)*thisDist));	
			newVel[1] = self->velocity[1] * (distanceLeft/((TRAIN_BRAKE_FACTOR+1)*thisDist));	
			newVel[2] = 0;
			VectorCopy(newVel, self->velocity);
			self->think = train_deccel;
			self->nextthink = level.time + .1;
			if (self->s.sound == gi.soundindex("Ambient/Locs/Subway/Train/Move.wav"))
			{
//				self->s.sound = 0;
	// 9/13/99 kef -- Chia doesn't care for this sound
//				gi.sound(self, CHAN_VOICE, gi.soundindex("Ambient/Locs/Subway/Train/Brake.wav"), .6, ATTN_NORM, 0);
			}
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		if (distanceLeft < thisDist)
		{
			vec3_t			newVel;
		
			//adjust velocity then stop
			newVel[0] = self->velocity[0] * (distanceLeft/thisDist);	
			newVel[1] = self->velocity[1] * (distanceLeft/thisDist);	
			newVel[2] = 0;
			VectorCopy(newVel, self->velocity);
			self->think = train_stop;
			self->nextthink = level.time + .1;

			if (self->s.sound == gi.soundindex("Ambient/Locs/Subway/Train/Move.wav") && !(self->spawnflags & TRAIN_RESPAWNER) && !(self->spawnflags & TRAIN_DISAPPEAR))
			{
				self->s.sound = 0;
	// 9/13/99 kef -- Chia doesn't care for this sound
//				gi.sound(self, CHAN_VOICE, gi.soundindex("Ambient/Locs/Subway/Train/Brake.wav"), .6, ATTN_NORM, 0);
			}
			return false;
		}
		else
		{
			return true;
		}
	}
}

void train_getlength (edict_t *self)
{
#if 0	// kef -- moved to train_go to avoid problems with early tracing, namely
		//if car1 spawns behind car2, car1's trace would hit car2 so car1 would
		//never move beyond the initial position of car2
	trace_t		trace;
	vec3_t		start, end, mins, maxs;


	VectorCopy(self->s.origin,start);
	VectorCopy(self->s.origin, end);
	VectorSet(mins, -1, -1, -1);
	VectorSet(maxs, 1, 1, 1);
	end[0] += (-8000 * sin(self->s.angles[1] * DEGTORAD)); // maps are 8192 end to end at most
	end[1] += (8000 * cos(self->s.angles[1] * DEGTORAD));
	start[2] += 40;
	end[2] += 40;
	gi.trace (start, NULL, NULL, end, self, self->clipmask, &trace);
//	trace = gi.trace (self->s.origin, self->mins, self->maxs, end, self, self->clipmask);
	self->volume = self->s.origin[0] + (-8000 * sin(self->s.angles[1] * DEGTORAD) * trace.fraction);  // using for storage
	self->attenuation = self->s.origin[1] + (8000 * cos(self->s.angles[1] * DEGTORAD) * trace.fraction);  //using for storage
#endif
	if (self->spawnflags & TRAIN_START_ACTIVE)
	{
		train_go(self, self, self);
	}
}


void train_linecheck (edict_t *self)
{
	vec3_t			sparkspot1, sparkdir;
	int				chance;

	
	GrabStuffOnTop (self);
	EndofLineCheck(self);

	chance = gi.irand(0,6);
	if (!chance)
	{
		int			xsign, ysign;
		
		
		xsign = -1 + (gi.irand(0,1)*2);
		ysign = -1 + (gi.irand(0,1)*2);
		
		VectorCopy(self->s.origin, sparkspot1);
		// got these offsets from Bobby in Max
		//
		// x = 52
		// y = 80
		// z = -55
		VectorSet(sparkspot1, 
			self->s.origin[0] + (xsign * 52),
			self->s.origin[1] + (ysign * 80),
			self->s.origin[2] - 55);
		FX_MakeSparks(sparkspot1, sparkdir, 0);
	}
	self->nextthink = level.time + .1;
}

void train_accel (edict_t *self)
{
	/*	potentially useful data:
		this acceleration is 1000 units/s^2, which works out to 28.6 m/s^2, or 2.9g's!
		uses 35 units/m
	  (+4 Geek, holy avenger)
		*/
	
	vec3_t			sparkspot1, sparkdir;
	int				chance;
	
	GrabStuffOnTop (self);//, velChange);
	if ((double)(self->velocity[0]*self->velocity[0] + self->velocity[1]*self->velocity[1]) < (self->style*self->style))
	{
		self->velocity[0] += (-1 * self->accel * sin(self->s.angles[1] * DEGTORAD));
		self->velocity[1] += (self->accel * cos(self->s.angles[1] * DEGTORAD));
		self->nextthink = level.time + .1;
	}
	else
	{
		self->think = train_linecheck;
		self->nextthink = level.time + .1;
	}

	chance = gi.irand(0,5);
	if (!chance)
	{
		int			xsign, ysign;
		
		
		xsign = -1 + (gi.irand(0,2)*2);
		ysign = -1 + (gi.irand(0,2)*2);
		
		VectorCopy(self->s.origin, sparkspot1);
		VectorSet(sparkspot1, self->s.origin[0] - xsign * 54*cos(self->s.angles[1] * DEGTORAD) - ysign * 105*sin(self->s.angles[1]* DEGTORAD),
			self->s.origin[1] - xsign * 54*sin(self->s.angles[1] * DEGTORAD) + ysign * 105*cos(self->s.angles[1]* DEGTORAD), self->s.origin[2]+15);
		FX_MakeSparks(sparkspot1, sparkdir, 0);
	}
	EndofLineCheck(self);
}

void train_go (edict_t *self, edict_t *other, edict_t *activator)
{
	int chance;
	trace_t		trace;
	vec3_t		start, end, mins, maxs;


	// kef -- moved trace stuff here from train_getlength()
	
	// need to trace from the bottom of the train's bbox
	VectorCopy(self->s.origin,start);
	VectorCopy(self->s.origin, end);
	VectorSet(mins, -1, -1, -1);
	VectorSet(maxs, 1, 1, 1);
	end[0] += (-8000 * sin(self->s.angles[1] * DEGTORAD)); // maps are 8192 end to end at most
	end[1] += (8000 * cos(self->s.angles[1] * DEGTORAD));
	start[2] += 40;
	end[2] += 40;
	gi.trace (start, NULL, NULL, end, self, self->clipmask, &trace);
//	trace = gi.trace (self->s.origin, self->mins, self->maxs, end, self, self->clipmask);
	self->volume = self->s.origin[0] + (-8000 * sin(self->s.angles[1] * DEGTORAD) * trace.fraction);  // using for storage
	self->attenuation = self->s.origin[1] + (8000 * cos(self->s.angles[1] * DEGTORAD) * trace.fraction);  //using for storage


	// trains with facing 0 are "pointing" north (pos y)
	GrabStuffOnTop (self);
	self->velocity[0] = -1 * self->accel * sin(self->s.angles[1] * DEGTORAD);
	self->velocity[1] = self->accel * cos(self->s.angles[1] * DEGTORAD);
	self->friction = 0;
	self->gravity = 0;
	self->nextthink = level.time + .1;
	self->think = train_accel;
	self->use = NULL;
	self->plUse = NULL;

	self->s.sound = gi.soundindex("Ambient/Locs/Subway/Train/Move.wav");	
//	self->s.sound_data = (255 & ENT_VOL_MASK) | SND_FARATTN;
	// 9/13/99 kef -- Chia wanted less attn
	self->s.sound_data = (255 & ENT_VOL_MASK) | SND_FARATTN;

	chance = gi.irand(1,3);
	if (chance==1)
		gi.sound (self, CHAN_BODY, gi.soundindex ("Ambient/Locs/Subway/Train/Horn2.wav"), 1.0, ATTN_NORM, 0);

	EndofLineCheck(self);
}

void train_smush (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	vec3_t		tempVel;
	vec3_t goalFacing, right,holdPos;
	float fDot;


	if ( (other->client == NULL) && (other->takedamage == DAMAGE_NO) && (other != &g_edicts[0]) )	// Can't be hurt
	{	// yeah? well meet the irresistable force, baby
		G_FreeEdict(other);
//		Com_Printf("Tossed out other\n");
		return;
	}

	if (VectorCompare(self->velocity, vec3_origin))
	{
//		Com_Printf("Train not moving\n");
		return;
	}

// train stoppage debugging stuff
//	vec3_t foo;
//	VectorSubtract(self->s.origin, other->s.origin, foo);
//	foo[2] = 0;
//	float fLen = VectorLength(foo);

	float fDiff = (self->s.origin[2] + self->maxs[2]) - (other->s.origin[2] + other->mins[2]);
	if (fDiff<= 0.0f) // above train
	{
//		Com_Printf("Above train return fDiff = %f, fLen = %f\n", fDiff, fLen);
		return;
	}
	// Set point at front of train
	AngleVectors(self->s.angles,NULL,right,NULL);
	VectorMA(self->s.origin,165,right,holdPos);

	VectorSubtract(other->s.origin,holdPos,goalFacing);
	VectorNormalize(goalFacing);

	AngleVectors(self->s.angles, right, NULL, NULL);
	fDot = DotProduct(right, goalFacing);

	// Is other within range??????
	if ((fDot < .25) && (fDot > -.25))
	{	// Bonk
		bodyorganic_c* victimBody = NULL;
		if (other->ai)
		{	// don't let the victim drop any boltons. they'll stop the train.
			if (victimBody = ((bodyorganic_c*)((ai_c*)((ai_public_c*)other->ai))->GetBody()) )
			{
				victimBody->CanDropBoltons(false);
			}
		}
		gi.sound (self, CHAN_VOICE, gi.soundindex ("Ambient/Locs/Subway/Train/Impact.wav"), 1.0, ATTN_NORM, 0);

		T_Damage(other, self, self, tempVel, other->s.origin, other->s.origin, 9000, 1000, DAMAGE_NO_KNOCKBACK|DT_MANGLE, MOD_CRUSH);
	}
	else
	{
		Com_Printf("Failed fDot test %f\n", fDot);
	}



/*
	VectorCopy(self->velocity, tempVel);
	VectorNormalize(tempVel);
	VectorScale(tempVel, 10, tempVel);

	// define a box of death.
	killBox = G_Spawn();
	VectorSet(move, -164*sin(self->s.angles[1] * DEGTORAD), 164*cos(self->s.angles[1] * DEGTORAD) , 60);
	VectorAdd(self->s.origin, move, killBox->s.origin);
	VectorSet(killBox->mins, -54, 0, -58);
	VectorSet(killBox->maxs, 54, 2, 58);
	// box needs to be thin enough NOT to include the player(or colliding object)
	//when it spawns, otherwise it will ignore that object during the trace...  bad
	VectorAdd(killBox->s.origin, tempVel, end);
	VectorCopy(self->s.angles,killBox->s.angles);
	killBox->solid = SOLID_NOT;
	killBox->movetype = MOVETYPE_NONE;
	killBox->owner = self;
	BboxRotate(killBox);

	gi.trace (killBox->s.origin, killBox->mins, killBox->maxs, end, killBox, self->clipmask, &trace);
	G_FreeEdict(killBox);
	if (trace.fraction >= 1)
	{
		return;
	}
	T_Damage(trace.ent, self, self, tempVel, other->s.origin, other->s.origin, 99999, 1000, DAMAGE_NO_KNOCKBACK, 0);
	*/
}

void SP_misc_generic_train (edict_t *ent)
{
	char *skinname;
	int holdspawn;

	VectorSet (ent->mins, -54, -164, -60);
	VectorSet (ent->maxs,  55,  168,  60);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "train2";
		break;
	case 2:
		skinname = "train3";
		break;
	case 3:
		skinname = "train4";
		break;
	default:
		skinname = "train";
		break;
	}	

	ent->takedamage = DAMAGE_NO;

	holdspawn = ent->spawnflags;
	ent->spawnflags = 0;

	if (!(ent->spawnflags & TRAIN_TRIGGERED) && !(ent->spawnflags & TRAIN_START_ACTIVE))
		ent->spawnflags |= SF_NOPUSH;

	if (ent->spawnflags & TRAIN_IDLE)
		ent->s.sound = gi.soundindex("Ambient/Locs/Subway/Train/Idle.wav");


	SimpleModelInit2(ent,&genericModelData[OBJ_TRAIN],skinname,NULL);
	ent->spawnflags = holdspawn;

	SetSkin(ent,genericModelData[OBJ_TRAIN].dir,genericModelData[OBJ_TRAIN].file, 
		 genericModelData[OBJ_TRAIN].materialfile,skinname, 0);


	switch (ent->s.skinnum)
	{
	case 1:
		SimpleModelSetObject(ent,"TRAIN");
		SimpleModelRemoveObject(ent,"WORLD_OMNI");
		break;
	case 2:
		SimpleModelSetObject(ent,"TRAIN");
		SimpleModelRemoveObject(ent,"WORLD_OMNI");
		break;
	case 3:
		SimpleModelSetObject(ent,"TRAIN");
		SimpleModelRemoveObject(ent,"WORLD_OMNI");
		break;
	default:
		break;
	}	
	

	if (ent->s.skinnum == 0)
	{
		SimpleModelAddBolt(ent,genericModelData[OBJ_TRAIN],"headlight1",
							genericModelData[OBJ_LIGHT_BEAM],"to_headlight",NULL);

		SimpleModelAddBolt(ent,genericModelData[OBJ_TRAIN],"headlight2",
							genericModelData[OBJ_LIGHT_BEAM],"to_headlight",NULL);
	}

	if ((ent->spawnflags & TRAIN_TRIGGERED) || (ent->spawnflags & TRAIN_START_ACTIVE))
	{
		ent->use = train_go;
		ent->movetype = MOVETYPE_DAN;
		ent->s.origin[2] += 1; // hack to avoid the sliding into floor brushes problem
		ent->think = train_getlength;
		ent->nextthink = level.time + .1;
		ent->touch = train_smush;
		ent->elasticity  = -1; // trains should plow through anything in their path
		ent->flags |= FL_NO_KNOCKBACK;
	}

	if (ent->spawnflags & TRAIN_RESPAWNER)
	{
		ent->s.origin[2] -= 1; // hack to avoid the sliding into floor brushes problem
		SetForRespawn(ent, SP_misc_generic_train, ent->wait);
		ent->s.origin[2] += 1; // hack to avoid the sliding into floor brushes problem
	}

	if (!ent->style)
	{
		ent->style = 600;
	}
	if (!ent->accel)
	{
		ent->accel = 100;
	}
	
	gi.soundindex("Ambient/Locs/Subway/Train/Move.wav"); // precache sound
	// 9/13/99 kef -- Chia doesn't care for this sound
//	gi.soundindex("Ambient/Locs/Subway/Train/Brake.wav"); // precache sound
	gi.soundindex ("Ambient/Locs/Subway/Train/Horn2.wav"); // precache sound
	gi.soundindex ("Ambient/Locs/Subway/Train/Impact.wav"); // precache sound
}

void ashtray_pain2 (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	FX_MakeRicochet(ent->s.origin);
	Obj_painroll (ent,other,damage*ROLLMULT_TRASH_ASHTRAY,1);
}

void ashtray_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	FX_MakeRicochet(ent->s.origin);
	Obj_painflip (ent,other,damage*FLIPMULT_TRASH_ASHTRAY);

	Obj_tipover(ent,other,damage);
	ent->pain = ashtray_pain2;
}


/*QUAKED misc_generic_trash_ashtray (1 .5 0) (-5 -4 -9) (5 4 9)  INVULNERABLE  NOPUSH x x x x FLUFF
A cylindrical ashtray/trashcan.  Like what is placed outside Raven's entrance.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- fall over and roll around when shot
*/
void SP_misc_generic_trash_ashtray (edict_t *ent)
{
	int i = 0;

	VectorSet (ent->mins, -5, -4, -9);
	VectorSet (ent->maxs,  5,  4,  9);
	ent->pain = ashtray_pain;

	SimpleModelInit2(ent,&genericModelData[OBJ_TRASH_ASHTRAY],NULL,NULL);

	// cache sound (CLSFX_RICOCHETS...CLSFX_RICOCHETS+NUM_RICOCHETSOUNDS-1)
	for (i = 0; i < NUM_RICOCHETSOUNDS; i++)
	{
		entSoundsToCache[CLSFX_RICOCHETS+i] = 1;
	}
}



void trashcan_barrel_fall (edict_t *self)
{
	if ((self->s.angles[0] >= 85) || (self->s.angles[0] <= -85) ||
		(self->s.angles[2] >= 85) || (self->s.angles[2] <= -85))
	{
		if (self->s.angles[0] >= 85)
		{
			self->s.angles[0] = 85;
		}
		if (self->s.angles[0] <= -85)
		{
			self->s.angles[0] = -85;
		}
		if (self->s.angles[2] >= 85)
		{
			self->s.angles[2] = 85;
		}
		if (self->s.angles[2] <= -85)
		{
			self->s.angles[2] = -85;
		}
		VectorClear(self->avelocity);
		self->nextthink = 0;
		return;
	}
	self->avelocity[0] *= 1.9;
	self->avelocity[2] *= 1.9;
	self->nextthink = level.time + .1;
}

void trashcan_barrel_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t	falldir;
	vec3_t	prelimavel;
	float	angle;

	VectorSubtract(self->s.origin, other->s.origin, falldir);
	VectorNormalize(falldir);
	angle = 360 - self->s.angle_diff;
	while (angle >= 360)
	{
		angle -= 360;
	}
	angle *= DEGTORAD;
	prelimavel[0] = falldir[0] * -70;
	prelimavel[2] = falldir[1] * 70;
	self->avelocity[0] = -1*(prelimavel[0] * cos(angle) + prelimavel[2] * sin(angle));
	self->avelocity[2] = -1*(prelimavel[2] * cos(angle) + prelimavel[0] * -1 * sin(angle));
	VectorClear(self->velocity);
	self->velocity[2] = 120;
	self->think = trashcan_barrel_fall;
	self->nextthink = level.time + .1;
	self->pain = NULL;
}

/*QUAKED misc_generic_trashcan_barrel (1 .5 0) (-15 -16 -17) (15 16 22)  INVULNERABLE  NOPUSH x x x x FLUFF
Round steel trashcan with lid. 
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- falls over when shot
- spawns paper when shot
*/
 void SP_misc_generic_trashcan_barrel (edict_t *ent)
{

	VectorSet (ent->mins, -15, -16, -17);
	VectorSet (ent->maxs,  15,  16,  22);

	ent->pain = trashcan_barrel_pain;

	SimpleModelInit2(ent,&genericModelData[OBJ_TRASHCAN_BARREL],NULL,NULL);
	ent->die = SpecialBecomeDebris;
}

/*QUAK-ED misc_generic_tree_dead (1 .5 0) (-43 -43 -41) (40 67 110)  INVULNERABLE  NOPUSH x x x x FLUFF
Large (slightly taller than player) tree trunk
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_tree_dead (edict_t *ent)
{
	VectorSet (ent->mins, -43, -43, -41);
	VectorSet (ent->maxs,  40,  67, 110);

	ent->die = G_Explode;
	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&genericModelData[OBJ_TREE_DEAD],NULL,NULL);
}

/*QUAKED misc_generic_tree_thin (1 .5 0) (-26 -15 -66) (25 16 96)  INVULNERABLE  NOPUSH x x x x FLUFF
Bare thin tree.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_tree_thin (edict_t *ent)
{

	VectorSet (ent->mins, -26, -15, -66);
	VectorSet (ent->maxs,  25,  16,  96);

	ent->die = G_Explode;
	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&genericModelData[OBJ_TREE_THIN],NULL,NULL);
}


/*QUAKED misc_generic_truckcab (1 .5 0) (-91 -40 -49) (91 40 49)  INVULNERABLE  NOPUSH x x x x FLUFF
A cab for a tractor-trailer.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- explodes when shot
*/
void SP_misc_generic_truckcab (edict_t *ent)
{
	VectorSet (ent->mins, -91, -40, -49);
	VectorSet (ent->maxs,  91,  40,  49);

	ent->spawnflags |= SF_NOPUSH;
	ent->die = G_Explode;

	SimpleModelInit2(ent,&genericModelData[OBJ_TRUCKCAB],NULL,NULL);
}

void tv_die (edict_t *ent, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	vec3_t	spray1;
	eft_rotate_t	*rotInfo = NULL;
	vec3_t		debrisNorm;

	// Throw some sparks
	VectorSet(spray1, (rand()%21) - 10, (rand()%21) - 10, 0);
	FX_MakeSparks(ent->s.origin, spray1, 1);
	VectorSet(spray1, (rand()%21) - 10, (rand()%21) - 10, 0);
	FX_MakeSparks(ent->s.origin, spray1, 1);
	VectorSet(spray1, (rand()%41) - 20, (rand()%41) - 20, (gi.irand(5,10)));
	FX_MakeSparks(ent->s.origin, spray1, 1);

	// Throw some chunks
	FX_ThrowDebris(ent->s.origin,debrisNorm, 5,DEBRIS_SM, MAT_GLASS,0,0,0, SURF_GLASS);

	FX_MediumExplosion(ent->s.origin, 50, 1);	

	BecomeDebris(ent,inflictor,attacker,damage,point);

}

void tv_touch(edict_t *ent, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	// when the tv drops off of the wall and hits something, have it spark and flip over
	vec3_t			spray1;
	Matrix4			mat;
	IGhoulInst		*inst = NULL;
	vec3_t			debrisNorm;

	// Throw some sparks
	VectorSet(spray1, (rand()%21) - 10, (rand()%21) - 10, 0);
	FX_MakeSparks(ent->s.origin, spray1, 1);

	// Throw some chunks
	VectorClear(debrisNorm);
	FX_ThrowDebris(ent->s.origin,debrisNorm, 3,DEBRIS_SM, MAT_METAL_DGREY,0,0,0, SURF_METAL_COMPUTERS);
	FX_ThrowDebris(ent->s.origin,debrisNorm, 5,DEBRIS_SM, MAT_GLASS,0,0,0, SURF_GLASS_COMPUTER);
	FX_LittleExplosion(ent->s.origin, 200, 1);
	
	if (inst = SimpleModelGetBolt(ent, 1))
	{
		// rotate to some absolute angle
		inst->GetXForm(mat);
		mat.Rotate(2, 1.58f);
		inst->SetXForm(mat);
	}
	ent->spawnflags |= SF_NOPUSH;
	ent->movetype = MOVETYPE_DAN;
				
	ent->touch = NULL;
}
void tv_pain2 (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t		spray1, spray2;

	VectorSet(spray1, (rand()%21) - 10, (rand()%21) - 10, 0);
	VectorSet(spray2, (rand()%21) - 10, (rand()%21) - 10, 0);

	FX_MakeSparks(self->s.origin, spray1, 1);
	FX_MakeSparks(self->s.origin, spray2, 1);
}

void tv_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t		spray1, spray2;
	vec3_t		debrisNorm;

	VectorSet(spray1, (rand()%21) - 10, (rand()%21) - 10, 0);
	VectorSet(spray2, (rand()%21) - 10, (rand()%21) - 10, 0);

	FX_MakeSparks(self->s.origin, spray1, 1);
	FX_MakeSparks(self->s.origin, spray2, 1);

	if (strcmp(self->classname, "misc_generic_tv_ceiling") == 0 &&
		self->bouyancy) // just using bouyancy as a counter, really
	{
		// drop existing TV off the wall, turn off existing TV's supports
		SimpleModelRemoveObject2(self->ghoulInst,"support1");
		SimpleModelRemoveObject2(self->ghoulInst,"support2");
		self->spawnflags ^= SF_NOPUSH;
		self->movetype = MOVETYPE_DAN;

		// some chunks (those supports didn't just disappear, did they?)
		VectorClear(debrisNorm);
		FX_ThrowDebris(self->s.origin,debrisNorm, 3,DEBRIS_SM, MAT_METAL_DGREY,0,0,0, SURF_METAL);

		self->bouyancy = 0;
		self->touch = tv_touch;
		self->pain = tv_pain2;
		gi.linkentity(self);

		// because the model is now rotated some (see tv_damaged_think) we should alter
		//the bottom of the bbox accordingly
		self->mins[2] += 9;
	}
}

void tv_damaged_think (edict_t *ent)
{
	vec3_t			fwd,right, tvFwd, tvUp = {0,0,1}, tvPos, vDown = {0, 0, -1};
	vec3_t			variance, angVel = {0,0,0};
	short			length;
	vec3_t			holdPos;
	Matrix4			matOld, matRot, matNew;
	IGhoulInst*		inst = NULL;
	qboolean		bSmoking = fxRunner.hasEffect("environ/normalsmoke", ent);

	if (ent->pain_debounce_time <= level.time && bSmoking)	// Turn off the smoke and sparks?
	{
		fxRunner.stopContinualEffect("environ/normalsmoke", ent);
		ent->s.sound = 0;
		bSmoking = false;
	}

	if (bSmoking)
	{
		AngleVectors(ent->s.angles,fwd,right,NULL);
	}

	if (strcmp(ent->classname, "misc_generic_tv_ceiling") == 0)
	{
		if (inst = SimpleModelGetBolt(ent, 1) )
		{
			// stop the rotation once the tv is pointed down
			GetGhoulPosDir(ent->s.origin, ent->s.angles, inst, NULL, "face", tvPos, tvFwd, NULL, tvUp);
			VectorCopy(tvFwd, fwd);
			VectorNormalize(tvFwd); // ouch
			if (DotProduct(vDown, tvFwd) < 0.90)
			{
				// it's not pointing down yet so rotate down some more
				inst->GetXForm(matOld);
				matRot.Rotate(2, 0.3f);
				matNew.Concat(matOld, matRot);
				inst->SetXForm(matNew);
			}
			else
			{
				ent->bouyancy = 1;
			}
		}
		if (bSmoking)
		{
			VectorCopy(tvUp, variance);
			VectorScale(variance, rand()%16 - 8, variance);
			length = 20;
			VectorMA(ent->s.origin,8,fwd,holdPos);
			holdPos[2] -= 5;
			VectorMA(holdPos,-7,right,holdPos);
			VectorAdd(holdPos,variance,holdPos);
			FX_MakeElectricArc(holdPos, length, right);
		}
	}
	else
	{
		if (bSmoking)
		{
			VectorSet(variance, 0, 0, rand()%12 - 6);
			length = 18;
			VectorMA(ent->s.origin,8,fwd,holdPos);
			VectorMA(holdPos,-6,right,holdPos);
			VectorAdd(holdPos,variance,holdPos);
			FX_MakeElectricArc(holdPos, length, right);
		}
	}

	ent->nextthink = level.time + .1;

}

void tv_ceiling_pain1 (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t	spray1,holdPos;
	vec3_t		fwd,up;
	vec3_t		debrisNorm;
	IGhoulInst	*inst = NULL;

	// gotta remove the FACE and WORLD_LIGHT from the tv, which is bolted to the support
	if (inst = SimpleModelGetBolt(ent, 1))
	{
		SimpleModelRemoveObject2(inst,"FACE");
		SimpleModelRemoveObject2(inst,"WORLD_LIGHT");
	}

	AngleVectors(ent->s.angles,fwd,NULL,up);

	VectorMA(ent->s.origin,10,fwd,holdPos);

	// Throw some sparks
	VectorSet(spray1, (rand()%21) - 10, (rand()%21) - 10, 0);
	FX_MakeSparks(holdPos, spray1, 1);
	VectorSet(spray1, (rand()%21) - 10, (rand()%21) - 10, 0);
	FX_MakeSparks(holdPos, spray1, 1);
	VectorSet(spray1, (rand()%41) - 20, (rand()%41) - 20, (gi.irand(5,10)));
	FX_MakeSparks(holdPos, spray1, 1);

	// Throw some chunks
	VectorClear(debrisNorm);
	FX_ThrowDebris(holdPos,debrisNorm, 3,DEBRIS_SM, MAT_METAL_DGREY,0,0,0, SURF_METAL);
	FX_ThrowDebris(holdPos,debrisNorm, 5,DEBRIS_SM, MAT_GLASS,0,0,0, SURF_GLASS);

	FX_LittleExplosion(holdPos, 100, 0);	


	gi.sound (ent, CHAN_VOICE, gi.soundindex("Impact/Glassbreak/Monitor.wav"), .6, ATTN_NORM, 0);

	ent->pain = tv_pain;

	ent->think = tv_damaged_think;
	ent->nextthink = level.time;

	ent->pain_debounce_time = level.time + gi.flrand(6,12);	// How long to smoke
	ent->s.sound =  gi.soundindex("Ambient/Gen/Electric/buzz.wav");
	ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;

}

void tv_ceiling_sequence (edict_t *ent)
{
	IGhoulInst	*inst = NULL;
	if (inst = SimpleModelGetBolt(ent, 1))
	{
		SimpleModelSetSequence2(inst,"tv",SMSEQ_LOOP);
	}
	ent->s.sound =  gi.soundindex("Ambient/Models/Monitor/Static.wav");
	ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
}

void tv_ceiling_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	vec3_t wherehit = {0,0,0};

	tv_ceiling_pain1 (ent,other, 0, 10,wherehit);
}

// POSSIBLY : when shot drop to floor and explode
// NOTE: the bounding box for the tv_ceiling reflects the scaling factor of 1.5

/*QUAKED misc_generic_tv_ceiling (1 .5 0) (-18 -19 -22) (19 19 28) INVULNERABLE  NOPUSH x x x x FLUFF
TV mounted from the ceiling. Has face of newcaster flashing on it.
Targeting with trigger makes it die
------ KEYS ------
style :
0 - newscaster fading in and out
1 - bad guy symbol fading in and out
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A, can't ever be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- falls to ground when shot
- smokes and sparks when shot
*/
void SP_misc_generic_tv_ceiling (edict_t *ent)
{
	VectorSet (ent->mins, -18, -19, -22);
	VectorSet (ent->maxs,  19,  19,  28);

	ent->spawnflags |= SF_NOPUSH;
	ent->pain = tv_ceiling_pain1;
	ent->use = tv_ceiling_use;
	ent->die = tv_die;
	// need someplace to keep track of damage time
	ent->bouyancy = 0.0f;

	SimpleModelInit2(ent,&genericModelData[OBJ_TV_CEILING_SUPPORT],NULL,NULL);

	SimpleModelAddBolt(ent,genericModelData[OBJ_TV_CEILING_SUPPORT],"DUMMY01",
						genericModelData[OBJ_TV_CEILING],"DUMMY01",NULL);

	SimpleModelScale(ent,1.5f);

	ent->think = tv_ceiling_sequence;
	ent->nextthink = level.time + .1 + ((rand() % 5) * .1);

	// cache sound CLSFX_EXP_SMALL, CLSFX_EXP_MEDIUM
	entSoundsToCache[CLSFX_EXP_SMALL] = 1;
	entSoundsToCache[CLSFX_EXP_MEDIUM] = 1;
	// cache debris CLGHL_CHUNKGLASSSM, CLGHL_CHUNKMETALSM
	entDebrisToCache[CLGHL_CHUNKGLASSSM] = DEBRIS_YES;
	entDebrisToCache[CLGHL_CHUNKMETALSM] = DEBRIS_YES;

	gi.soundindex("Impact/Glassbreak/Monitor.wav"); // precache sound
	gi.soundindex("Ambient/Gen/Electric/buzz.wav"); // precache sound
	gi.soundindex("Ambient/Models/Monitor/Static.wav"); // precache sound
	gi.effectindex("environ/normalsmoke");

	if (ent->style == 1)
	{
		SetSkin(ent,genericModelData[OBJ_TV_CEILING].dir,genericModelData[OBJ_TV_CEILING].file, 
			 genericModelData[OBJ_TV_CEILING].materialfile,"tv_br0", 1);
	}
}


void tv_wall_pain1 (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t	spray1;
	vec3_t		debrisNorm;

	ent->movetype = MOVETYPE_DAN;	// Drop like a rock when shot

	// Remove the proper parts when damaged
	if (ent->style == 0)	// Monochrome screen
	{
		SimpleModelRemoveObject(ent,"FACE");

		SimpleModelRemoveObject(ent,"WORLD_OMNI");
	}
	else			// Radar screen
	{
  		SimpleModelRemoveObject(ent,"FACE2");

		SimpleModelRemoveObject(ent,"FACE2B");

		SimpleModelRemoveObject(ent,"FACE2C");

		SimpleModelRemoveObject(ent,"FACE2D");

		ent->s.sound = 0;
	}

	// Damaged skin
	SetSkin(ent,genericModelData[OBJ_TV_WALL].dir,genericModelData[OBJ_TV_WALL].file, 
		 genericModelData[OBJ_TV_WALL].materialfile,"tv_wall_d", 0);

	// Throw some sparks
	VectorSet(spray1, (rand()%21) - 10, (rand()%21) - 10, 0);
	FX_MakeSparks(ent->s.origin, spray1, 1);
	VectorSet(spray1, (rand()%21) - 10, (rand()%21) - 10, 0);
	FX_MakeSparks(ent->s.origin, spray1, 1);
	VectorSet(spray1, (rand()%41) - 20, (rand()%41) - 20, (gi.irand(5,10)));
	FX_MakeSparks(ent->s.origin, spray1, 1);

	// Throw some chunks
	VectorClear(debrisNorm);
	FX_ThrowDebris(ent->s.origin,debrisNorm, 3,DEBRIS_SM, MAT_METAL_DGREY,0,0,0, SURF_METAL);
	FX_ThrowDebris(ent->s.origin,debrisNorm, 5,DEBRIS_SM, MAT_GLASS,0,0,0, SURF_GLASS);

	FX_LittleExplosion(ent->s.origin, 100, 0);	

	ent->pain_debounce_time = level.time + gi.flrand(6,12);	// How long to smoke

	ent->pain = tv_pain;

	// drop off the wall
	ent->spawnflags ^= SF_NOPUSH;

	ent->think = tv_damaged_think;
	ent->nextthink = level.time;

	gi.sound (ent, CHAN_VOICE, gi.soundindex("Impact/Glassbreak/Monitor.wav"), .6, ATTN_NORM, 0);

	ent->s.sound =  gi.soundindex("Ambient/Gen/Electric/buzz.wav");
	ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
}

void tv_wall_sequence (edict_t *ent)
{
	SimpleModelSetSequence(ent,"tv_wall",SMSEQ_LOOP);
}

void tv_wall_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	vec3_t wherehit = {0,0,0};

	tv_wall_pain1 (ent,other, 0, 10,wherehit);
}


// NOTE: the bounding box for the tv_wall reflects the scaling factor of 1.5
/*QUAKED misc_generic_tv_wall (1 .5 0) (-5 -9 -9) (6 9 7) INVULNERABLE  NOPUSH x x x x FLUFF
A wall-mounted TV. Has a scrolling monochrome screen
Targeting with trigger makes it die
------ KEYS ------
style :
0 - monochrome screen
1 - radar screen
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A, can't ever be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- falls to floor when shot
- smokes and sparks when shot
*/
void SP_misc_generic_tv_wall (edict_t *ent)
{
	VectorSet (ent->mins, -5, -9, -9);
	VectorSet (ent->maxs,  6,  9,  7);

	ent->spawnflags |= SF_NOPUSH;
	ent->pain = tv_wall_pain1;
	ent->use = tv_wall_use;
	ent->die = tv_die;

	SimpleModelInit2(ent,&genericModelData[OBJ_TV_WALL],NULL,NULL);
	SetSkin(ent,genericModelData[OBJ_TV_WALL].dir,genericModelData[OBJ_TV_WALL].file, 
		 genericModelData[OBJ_TV_WALL].materialfile,"tv_wall", 0);

	if (ent->style == 0)	// Monochrome screen
	{
  		SimpleModelRemoveObject(ent,"FACE2");

		SimpleModelRemoveObject(ent,"FACE2B");

		SimpleModelRemoveObject(ent,"FACE2C");

		SimpleModelRemoveObject(ent,"FACE2D");
	}
	else	// Radar screen with grid
	{
		SimpleModelRemoveObject(ent,"FACE");

		SimpleModelRemoveObject(ent,"WORLD_OMNI");

		ent->s.sound = gi.soundindex("ambient/models/Panel/Panel1.wav");
		ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
	}


	SimpleModelScale(ent,1.5f);

	ent->think = tv_wall_sequence;
	ent->nextthink = level.time + ((rand() % 5) * .1);

	// cache sound CLSFX_EXP_SMALL, CLSFX_EXP_MEDIUM
	entSoundsToCache[CLSFX_EXP_SMALL] = 1;
	entSoundsToCache[CLSFX_EXP_MEDIUM] = 1;
	// cache debris CLGHL_CHUNKGLASSSM, CLGHL_CHUNKMETALSM
	entDebrisToCache[CLGHL_CHUNKGLASSSM] = DEBRIS_YES;
	entDebrisToCache[CLGHL_CHUNKMETALSM] = DEBRIS_YES;

	gi.soundindex("Impact/Glassbreak/Monitor.wav"); // precache sound
	gi.soundindex("Ambient/Gen/Electric/buzz.wav"); // precache sound
	gi.effectindex("environ/normalsmoke");
}


void urinal_cookie_touch (edict_t *ent, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	// deal out a bit o' damage from flying urinal pucks
	float speed = sqrt(ent->velocity[0] * ent->velocity[0] + ent->velocity[1] * ent->velocity[1] +
		ent->velocity[2] * ent->velocity[2]);
	if (( speed > 20) && (other->takedamage))
	{
		T_Damage(other, ent, ent->owner, vec3_origin, ent->s.origin, ent->s.origin, int(sqrt(speed)*.1), 0, DT_PROJECTILE, 0);
	}
}

void urinal_cookie_think (edict_t *ent)
{
	ent->nextthink = 0;
	G_FreeEdict(ent);
}

void urinal_cookie_toss (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t				accel = {0,0,-800};

	if (level.time < self->pain_debounce_time)
		return;


	SetSkin(self,genericModelData[OBJ_URINAL].dir,genericModelData[OBJ_URINAL].file, 
		 genericModelData[OBJ_URINAL].materialfile,"damaged", 0);

	self->think = toilet_dribble;
	self->nextthink = level.time + .5;

	self->pain_debounce_time = level.time + gi.flrand (6,12);
}

void urinal_die2(edict_t *self)
{
	// ObjBecomeDebris() only uses self and attacker, and it only uses the name of the attacker
	//and even then it only uses it for debug messages
	ObjBecomeDebris(self, NULL, self, 0, vec3_origin);

}

void urinal_create_water(edict_t *self)
{
	edict_t		*water;
	vec3_t		forward;


	water = G_Spawn();
	water->think = sink_spurt;
	water->nextthink = level.time + .1;
	VectorCopy(self->s.origin, water->s.origin);

	VectorCopy(self->s.angles, water->s.angles);

	AngleVectors(water->s.angles, forward, NULL, NULL);

	water->s.sound =  gi.soundindex("Ambient/Gen/WtrFX/SprayBig.wav");
	water->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;

	water->count = 10;

	gi.linkentity (water);
}


void urinal_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	urinal_create_water(self);
	self->think = urinal_die2;
	self->nextthink = level.time + .1f;
}

void urinal_use (edict_t *self, edict_t *other, edict_t *activator)
{
	gi.sound (self, CHAN_BODY, gi.soundindex("Ambient/Models/Urinal/Flush.wav"), 1.0, ATTN_NORM, 0);
}

/*QUAKED misc_generic_urinal (1 .5 0) (-6 -9 -11) (9 9 14)  INVULNERABLE  NOPUSH x x x x FLUFF
A urinal.
------ KEYS ------
Skin :
 0 - dirty New York. (default)
 1 - clean Tokyo
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A, can't ever be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- throws out puck when shot
- produces water fountain when destroyed
*/
void SP_misc_generic_urinal (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -6, -9, -11);
	VectorSet (ent->maxs, 9, 9, 14);

	ent->spawnflags |= SF_NOPUSH;

	ent->pain = urinal_cookie_toss;
	ent->use = urinal_use;		// Use for trigger_uses
	ent->plUse = urinal_use;	// Use for when player pushes 'use' key
	ent->die = urinal_die;

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "urinal2"; //clean
		break;
	default:
		skinname = "urinal";  //dirty
		break;
	}	

	gi.soundindex("Ambient/Models/Urinal/Flush.wav");
	gi.soundindex("Ambient/Gen/WtrFX/SprayBig.wav");
	gi.effectindex("environ/k_waterspout3");

	SimpleModelInit2(ent,&genericModelData[OBJ_URINAL],NULL,NULL);
	SetSkin(ent,genericModelData[OBJ_URINAL].dir,genericModelData[OBJ_URINAL].file, 
		 genericModelData[OBJ_URINAL].materialfile,skinname, 0);
	ent->s.renderfx |= RF_LIGHT_FASTEST;

}


/*QUAKED misc_generic_urinal_cake (1 .5 0) (-3 -3 -1) (3 3 1)  INVULNERABLE  NOPUSH x x x x FLUFF
A nice pink urinal cake.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_urinal_cake (edict_t *ent)
{
	VectorSet (ent->mins, -3, -3, -1);
	VectorSet (ent->maxs, 3, 3, 1);

//	ent->pain = urinal_cookie_toss;

	SimpleModelInit2(ent,&genericModelData[OBJ_URINAL_CAKE],NULL,NULL);
}

/*QUAKED misc_generic_urn (1 .5 0) (-16 -16 -14) (16 16 14)  INVULNERABLE  NOPUSH x x x x FLUFF
A fine looking urn.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_urn (edict_t *ent)
{
	VectorSet (ent->mins, -16, -16, -14);
	VectorSet (ent->maxs,  16,  16,  14);

	SimpleModelInit2(ent,&genericModelData[OBJ_URN],NULL,NULL);
}

void waste_pain2 (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	Obj_painroll (ent,other,damage*ROLLMULT_WASTE_BASKET,1);
}

void waste_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	Obj_painflip (ent,other,damage*ROLLMULT_WASTE_BASKET);

	Obj_tipover(ent,other,damage);
	ent->pain = waste_pain2;
}

/*QUAKED misc_generic_waste_basket_wire (1 .5 0) (-6 -7 -7) (6 7 7)  INVULNERABLE  NOPUSH x x x x FLUFF
Small, circular wastebasket
------ KEYS ------
Skin :
 0 - wire mesh. (default)
 1 - solid metal
 2 - rusty wire
 3 - rusty solid
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- falls over and rolls around when shot
*/
void SP_misc_generic_waste_basket_wire (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -6, -7, -7);
	VectorSet (ent->maxs,  6,  7,  7);

	ent->pain = waste_pain;

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "waste_basket2";
		break;
	case 2:
		skinname = "waste_basket3";
		break;
	case 3:
		skinname = "waste_basket4";
		break;
	default:
		skinname = "waste_basket";
		break;
	}	

	SimpleModelInit2(ent,&genericModelData[OBJ_WASTE_BASKET],skinname,NULL);

}


/*QUAKED misc_generic_web_moss_animated (1 .5 0) (-4 -15 -10) (-1 15 10)  INVULNERABLE  NOPUSH  NOANIMATE x x x FLUFF
moss/cobwebs blowing in the breeze
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
NOANIMATE - won't animate
FLUFF - won't show if gl_pictip is set
------ KEYS ------
Skin :
0 - cobweb1 (default)
1 - cobweb2 
2 - moss1
3 - moss2
4 - bloodflap
*/
void SP_misc_generic_web_moss_animated (edict_t *ent)
{
	char *skinname;

	ent->spawnflags |= SF_INVULNERABLE;
	ent->spawnflags |= SF_NOPUSH;

	VectorSet (ent->mins, -4, -15, -10);
	VectorSet (ent->maxs, -1, 15, 10);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "web2";
		break;
	case 2:
		skinname = "moss1";
		break;
	case 3:
		skinname = "moss2";
		break;
	case 4:
		skinname = "bloodflap1";
		break;
	default:
		skinname = "web1";
		break;
	}	

	SimpleModelInit2(ent,&genericModelData[OBJ_WEB_MOSS_ANIM],skinname,NULL);

}


void wet_floor_fall (edict_t *self)
{
	if ((self->s.angles[0] >= 128) || (self->s.angles[0] <= -128) ||
		(self->s.angles[2] >= 90) || (self->s.angles[2] <= -90))
	{
		if (self->s.angles[0] >= 128)
		{
			self->s.angles[0] = 128;
		}
		if (self->s.angles[0] <= -128)
		{
			self->s.angles[0] = -128;
		}
		if (self->s.angles[2] >= 90)
		{
			self->s.angles[2] = 90;
		}
		if (self->s.angles[2] <= -90)
		{
			self->s.angles[2] = -90;
		}
		VectorClear(self->avelocity);
		self->nextthink = 0;
		return;
	}
	self->avelocity[0] *= 1.5;
	self->avelocity[2] *= 1.5;
	self->health = 1;
	self->nextthink = level.time + .1;
}

void wet_floor_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t	falldir;
	vec3_t	prelimavel;
	float	angle;

	VectorSubtract(self->s.origin, other->s.origin, falldir);
	VectorNormalize(falldir);
	angle = 360 - self->s.angle_diff;
	angle = NormalizeAngle(angle);
	angle *= DEGTORAD;
	prelimavel[0] = falldir[0] * -70;
	prelimavel[2] = falldir[1] * 70;
	self->avelocity[0] = -1 * (prelimavel[0] * cos(angle) + prelimavel[2] * sin(angle));
	self->avelocity[2] = -1 * ( prelimavel[2] * cos(angle) + prelimavel[0] * -1 * sin(angle));
	VectorClear(self->velocity);
	self->velocity[2] = 160;
	self->think = wet_floor_fall;
	self->nextthink = level.time + .1;
	self->pain = NULL;
}

/*QUAKED misc_generic_wet_floor_sign (1 .5 0) (-9 -8 -8) (9 8 8)  INVULNERABLE  NOPUSH  x x x x FLUFF
A sign that says 'wet floor'
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- falls over when shot
*/
void SP_misc_generic_wet_floor_sign (edict_t *ent)
{
	VectorSet (ent->mins, -9, -8, -8);
	VectorSet (ent->maxs,  9,  8,  8);

	ent->pain = wet_floor_pain;

	SimpleModelInit2(ent,&genericModelData[OBJ_WETFLOOR_SIGN],NULL,NULL);
}

/*QUAKED misc_generic_wpn_bat (1 .5 0) (-35 -33 55) (-31 3 60)  INVULNERABLE  NOPUSH  x x x x FLUFF
A bat for cracking heads  (not a pickup)
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_wpn_bat (edict_t *ent)
{
	VectorSet (ent->mins, -35, -33, 55);
	VectorSet (ent->maxs, -31,   3, 60);

	SimpleModelInit2(ent,&genericModelData[OBJ_WPN_BAT],NULL,NULL);

}

/*QUAKED misc_generic_wpn_desert_eagle (1 .5 0) (-42 -12 55) (-30 1 58)  INVULNERABLE  NOPUSH  x x x x FLUFF
A weapon Desert Eagle  (not a pickup)
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_wpn_desert_eagle (edict_t *ent)
{
	VectorSet (ent->mins, -42, -12, 55);
	VectorSet (ent->maxs, -30,   1, 58);

	SimpleModelInit2(ent,&genericModelData[OBJ_WPN_DESERT_EAGLE],NULL,NULL);

}

/*QUAKED misc_generic_wpn_glock (1 .5 0) (-40 -10 56) (-31 1 58)  INVULNERABLE  NOPUSH  x x x x FLUFF
A weapon Glock (not a pickup)
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_wpn_glock (edict_t *ent)
{
	VectorSet (ent->mins, -40, -10, 56);
	VectorSet (ent->maxs, -31,   1, 58);

	SimpleModelInit2(ent,&genericModelData[OBJ_WPN_GLOCK],NULL,NULL);
}

/*QUAKED misc_generic_wpn_ingram (1 .5 0) (-44 -13 55) (-27 2 59)  INVULNERABLE  NOPUSH  x x x x FLUFF
A weapon Ingram  (not a pickup)
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_wpn_ingram (edict_t *ent)
{
	VectorSet (ent->mins, -44, -13,  55);
	VectorSet (ent->maxs, -27,   2,  59);

	SimpleModelInit2(ent,&genericModelData[OBJ_WPN_INGRAM],NULL,NULL);
}

/*QUAKED misc_generic_wpn_katana (1 .5 0) (-37 -42 54) (-30 9 61)  INVULNERABLE  NOPUSH  x x x x FLUFF
A weapon Katana  (not a pickup)
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_wpn_katana (edict_t *ent)
{
	VectorSet (ent->mins, -37, -42,  54);
	VectorSet (ent->maxs, -30,   9,  61);

	SimpleModelInit2(ent,&genericModelData[OBJ_WPN_KATANA],NULL,NULL);
}

/*QUAKED misc_generic_wpn_minimi (1 .5 0) (-7 -2 -1) (29 7 3)  INVULNERABLE  NOPUSH  x x x x FLUFF
A weapon Minimi (not a pickup)
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_wpn_minimi (edict_t *ent)
{
	VectorSet (ent->mins,  -7, -2, -1);
	VectorSet (ent->maxs,  29,  7,  3);

	SimpleModelInit2(ent,&genericModelData[OBJ_WPN_MINIMI],NULL,NULL);
}

/*QUAKED misc_generic_wpn_sig3000 (1 .5 0) (-60 -21 55) (-26 8 58)  INVULNERABLE  NOPUSH  x x x x FLUFF
A weapon SIG3000 (not a pickup)
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_wpn_sig3000 (edict_t *ent)
{
	VectorSet (ent->mins, -60, -21, 55);
	VectorSet (ent->maxs, -26,   8, 58);

	SimpleModelInit2(ent,&genericModelData[OBJ_WPN_SIG3000],NULL,NULL);
}

/*QUAKED misc_generic_wpn_spas12 (1 .5 0) (-60 -6 55) (-30 1 58)  INVULNERABLE  NOPUSH  x x x x FLUFF
A weapon spas12 (not a pickup)
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_generic_wpn_spas12 (edict_t *ent)
{
	VectorSet (ent->mins, -60, -6, 55);
	VectorSet (ent->maxs, -30,  1, 58);

	SimpleModelInit2(ent,&genericModelData[OBJ_WPN_SPAS12],NULL,NULL);
}

void generic_wpn_c4_use(edict_t *self, edict_t *other, edict_t *activator)
{
	if (self->spawnflags & C4_START_OFF)
	{	// at spawn time, this thing should be invisible, then become visible when "used"
		SimpleModelTurnOnOff(self->ghoulInst, true);
	}
}

/*QUAKED misc_generic_wpn_c4 (1 .5 0) (-4 -2 -4) (3 2 4)  INVULNERABLE  NOPUSH  START_OFF x x x 
A c4 charge (not a pickup). No FLUFF flag cuz we'll probably only want it for a KOS1 cinematic.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
START_OFF - doesn't appear until used
*/
void SP_misc_generic_wpn_c4 (edict_t *ent)
{
	VectorSet (ent->mins, -4, -2, -4);
	VectorSet (ent->maxs, 3,  2, 4);

	SimpleModelInit2(ent,&genericModelData[OBJ_WPN_C4],NULL,NULL);

	if (ent->spawnflags & C4_START_OFF)
	{	// at spawn time, this thing should be invisible, then become visible when "used"
		SimpleModelTurnOnOff(ent->ghoulInst, false);
	}
	ent->use = generic_wpn_c4_use;
	ent->movetype = MOVETYPE_NONE;
}