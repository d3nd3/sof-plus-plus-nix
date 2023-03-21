#include "q_shared.h"
#include "..\qcommon\palette.h"
#include "q_sh_fx.h"

// wheee! look at me! I'm _sharing_ !!!!!
cl_ghoul_data_t cl_fxg [NUM_CLGHL] = 
{
	"Objects/Generic/chunks_brick",	"brick",		"Brick",		3,// CLGHL_CHUNKBRICK
	"Objects/Generic/chunks_glass",	"glass_sm",		"glass",		3,// CLGHL_CHUNKGLASSSM
	"Objects/Generic/chunks_glass",	"glass_med",	"glass",		3,// CLGHL_CHUNKGLASSMED
	"Objects/Generic/chunks_glass",	"glass_lrg",	"glass",		3,// CLGHL_CHUNKGLASSLRG
	"Objects/Generic/chunks_metal",	"metal_sm",		"metal",		4,// CLGHL_CHUNKMETALSM
	"Objects/Generic/chunks_metal",	"metal_med",	"metal",		4,// CLGHL_CHUNKMETALMED
	"Objects/Generic/chunks_metal",	"metal_lg",		"metal",		4,// CLGHL_CHUNKMETALLRG
	"Objects/Generic/chunks_rock",	"rock_sm",		"rock",			4,// CLGHL_CHUNKROCKSM
	"Objects/Generic/chunks_rock",	"rock_med",		"rock",			4,// CLGHL_CHUNKROCKMED
	"Objects/Generic/chunks_rock",	"rock_lg",		"rock",			4,// CLGHL_CHUNKROCKLRG
	"Objects/Generic/chunks_rock",	"rock_tiny",	"rock",			4,// CLGHL_CHUNKROCKTINY
	"Objects/Generic/chunks_wall",	"wall_sm",		"wall",			4,// CLGHL_CHUNKWALLSM
	"Objects/Generic/chunks_wall",	"wall_med",		"wall",			4,// CLGHL_CHUNKWALLMED
	"Objects/Generic/chunks_wall",	"wall_lrg",		"wall",			4,// CLGHL_CHUNKWALLLRG
	"Objects/Generic/chunks_wall",	"wall_tiny",	"wall",			4,// CLGHL_CHUNKWALLTINY
	"Objects/Generic/chunks_wood",	"wood_sm",		"wood",			3,// CLGHL_CHUNKWOODSM
	"Objects/Generic/chunks_wood",	"wood_med",		"wood",			3,// CLGHL_CHUNKWOODMED
	"Objects/Generic/chunks_wood",	"wood_large",	"wood",			3,// CLGHL_CHUNKWOODLRG
	"Items/Projectiles",			"shell",		NULL,			0,// CLGHL_SHELL
	"Effects/explosion",			"explode80",	"explode",		0,// CLGHL_EXPLODEBIG
	"Effects/explosion",			"explode20",	"explode",		0,// CLGHL_EXPLODESML
	"Objects/Generic/soda_can",		"can",			NULL,			0,// CLGHL_CAN
	"Objects/Generic/paper_blowing","paper",		NULL,			1,// CLGHL_PAPER
	"Effects/explosion", 			"mushroom",	    "mushroom",		0,// CLGHL_MUSHROOM

	"this is the end of the list, so dont move it", "yeah", NULL,	0
};

cl_effect_data_t cl_fxs [NUM_CLSFX] = 
{
	"Ambient/Gen/Electric/spark1.wav",
	"Ambient/Gen/Electric/spark2.wav",
	"player/foot/metal/metal1.wav",
	"player/foot/metal/metal2.wav",
	"player/foot/metal/metal3.wav",
	"player/foot/metal/metal4.wav", 
	"player/foot/snow/snow1.wav",
	"player/foot/snow/snow2.wav",
	"player/foot/snow/snow3.wav",
	"player/foot/snow/snow4.wav",
	"player/foot/water/water1.wav", 
	"player/foot/water/water2.wav",
	"player/foot/water/water3.wav",
	"player/foot/water/water4.wav",
	"player/foot/grass/grass1.wav",
	"player/foot/grass/grass2.wav", 
	"player/foot/grass/grass3.wav",
	"player/foot/grass/grass4.wav",
	"player/foot/gravel/gravel1.wav",
	"player/foot/gravel/gravel2.wav",
	"player/foot/gravel/gravel3.wav", 
	"player/foot/gravel/gravel4.wav",
	"player/foot/sand/sand1.wav",
	"player/foot/sand/sand2.wav",
	"player/foot/sand/sand3.wav",
	"player/foot/sand/sand4.wav", 
	"player/foot/wood/wood1.wav",
	"player/foot/wood/wood2.wav",
	"player/foot/wood/wood3.wav",
	"player/foot/wood/wood4.wav",
	"player/foot/stone/stone1.wav", 
	"player/foot/stone/stone2.wav",
	"player/foot/stone/stone3.wav",
	"player/foot/stone/stone4.wav",
//	"misc/Gore/splat1.wav",
//	"misc/Gore/splat2.wav", 
//	"misc/Gore/splat3.wav",
	"Weapons/FX/Rics/ric1.wav",
	"Weapons/FX/Rics/ric2.wav",
	"Weapons/FX/Rics/ric3.wav",
	"Impact/Surfs/stone.wav",
	"Impact/Surfs/water.wav",
	"Impact/Surfs/wood.wav", 
	"Impact/Surfs/grass.wav",
	"Impact/Surfs/gravel.wav",
	"Impact/Surfs/sand.wav",
	"Impact/Surfs/snow.wav",
	"shouldn't be loaded now",	// kef - CLSFX_METALIMPACT should never be loaded now, but it still needs a slot here
	"Impact/Gore/impact1.wav", 
	"Impact/Gore/impact2.wav", 
	"Impact/Gore/impact3.wav", 
	"Weapons/FX/FlyBy/FlyBy1.wav",
	"Weapons/FX/FlyBy/FlyBy2.wav",
	"Weapons/FX/FlyBy/FlyBy3.wav",
	"Weapons/FX/FlyBy/FlyBy4.wav", 
	"Weapons/Glock/fire.wav",// this should be the first shot sound
	"ambient/Nature/weather/thnder1.wav",// this should be the first shot sound
	"weapons/FlshGren/FlshExp.wav",
	"Weapons/Autoshot/fire.wav",
	"Enemy/MGun/trnslow.wav",
	"Enemy/MGun/trnstop.wav",
	"weapons/C4/C4Exp.wav",
	"Impact/Chair/Squeak1.wav",
	"Impact/Chair/Squeak2.wav",
	"Impact/Chair/Squeak3.wav",
	"Ambient/Locs/Subway/Train/Brake.wav",
	"Ambient/Locs/Subway/Train/Horn2.wav",
	"Ambient/Locs/Subway/Train/Impact.wav",
	"Ambient/Locs/Subway/Train/Move.wav",
	"Weapons/Knife/Swing1.wav",
	"Weapons/Knife/Swing2.wav",
	"Weapons/Knife/Swing3.wav",
	"Weapons/Knife/Impact.wav",
	"Weapons/Knife/Throw.wav",
	"Player/Land/Grass.wav",
	"Player/Land/Gravel.wav",
	"Player/Land/Metal.wav",
	"Player/Land/Sand.wav",
	"Player/Land/Snow.wav",
	"Player/Land/Stone.wav",
	"Player/Land/Water.wav",
	"Player/Land/Wood.wav",
//	"Weapons/DesertEagle/Fire.wav",
//	"Weapons/Ingram/Fire.wav",
//	"Weapons/Jack/Fire.wav",
//	"Weapons/flamethrower/Fireball.wav",
//	"Weapons/jack/Fire2.wav",
//	"Weapons/Sniper/Fire.wav",
//	"weapons/spas12/fire.wav",
//	"weapons/Spas12/Fire2.wav",
//	"weapons/Minimi/Fire2.wav",
	"impact/surfs/metal1.wav",
	"impact/surfs/metal2.wav",
	"impact/surfs/metal3.wav",
//	"weapons/Minimi/Fire.wav",
//	"weapons/Mgun/explode.wav",
	"Impact/GlassBreak/GBBig.wav",
	"Impact/GlassBreak/GBMed.wav",
	"Impact/GlassBreak/GBSmall.wav",
	"Impact/Break/Metal.wav",
	"Impact/Break/Stone.wav",
	"Impact/Break/Wood.wav",
	"Ambient/Gen/Electric/buzz.wav",
	"Impact/Explosion/Big.wav",
	"Impact/Explosion/Med.wav",
	"Impact/Explosion/Small.wav",
	"Ambient/Models/Phone/Hit.wav",
	"Ambient/Models/TrashCan/LidSpin.wav",
	"weapons/FX/Tink/Metal1.wav",
	"weapons/FX/Tink/Metal2.wav",
	"weapons/FX/Tink/Metal3.wav",
	"weapons/FX/Tink/Gen1.wav",
	"weapons/FX/Tink/Gen2.wav",
	"Impact/Gore/Fall1.wav",
	"Impact/Gore/Fall2.wav",
	"Impact/Gore/Fall3.wav",
	"weapons/FX/Crack/Crack1.wav",
	"weapons/FX/Crack/Crack2.wav",
	"weapons/FX/Crack/Crack3.wav",
	"enemy/Helicopter/Fire1.wav",
	"enemy/Helicopter/Fire2.wav",
	"enemy/Helicopter/Fire3.wav",
	"weapons/FX/Tink/shot1.wav",
	"weapons/FX/Tink/shot2.wav",
	"weapons/FX/Tink/shot3.wav",
	"weapons/FX/Tink/hvy1.wav",
	"weapons/FX/Tink/hvy2.wav",
	"weapons/FX/Tink/hvy3.wav",
	"this is the end of the list, so dont move it",
};


