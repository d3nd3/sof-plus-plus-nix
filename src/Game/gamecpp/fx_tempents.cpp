#include "g_local.h"
#include "..\qcommon\ef_flags.h"

bool	lock_blood;

void WriteDirExp(vec3_t val)
{
	VectorScale(val, 1000, val);
	gi.WritePosition(val);
	VectorScale(val, .001, val);
}
 
int wallEffectFlag[SURF_NUM] = 
{

0,	// SURF_DEFAULT = 0,
0,	// SURF_METAL_ALSO,
0,	// SURF_SAND_YELLOW,
0,	// SURF_SAND_WHITE,
0,	// SURF_SAND_LBROWN,

0,	// SURF_SAND_DBROWN,		
0,	// SURF_GRAVEL_GREY,
0,	// SURF_GRAVEL_DBROWN,
0,	// SURF_GRAVEL_LBROWN,
0,	// SURF_SNOW,

0,	// SURF_LIQUID_BLUE,		
0,	// SURF_LIQUID_GREEN,
0,	// SURF_LIQUID_ORANGE,
0,	// SURF_LIQUID_BROWN,
0,	// SURF_WOOD_LBROWN,

0,	// SURF_WOOD_DBROWN,		
0,	// SURF_WOOD_LGREY,
0,	// SURF_STONE_LGREY,		
0,	// SURF_STONE_DGREY,	
0,	// SURF_STONE_LBROWN,		

0,	// SURF_STONE_DBROWN,		
0,	// SURF_STONE_WHITE,		
0,	// SURF_STONE_GREEN,		
0,	// SURF_STONE_RED,			
0,	// SURF_STONE_BLACK,		

0,	// SURF_GRASS_GREEN,		
0,	// SURF_GRASS_BROWN,
0,	// SURF_LIQUID_RED,
0,	// SURF_METAL,
0,	// SURF_METAL_WATERJET,

0,	// SURF_METAL_OILSPURT,	
0,	// SURF_METAL_CHEMSPURT,
1,	// SURF_METAL_COMPUTERS,
0,	// SURF_SNOW_LBROWN,
0,	// SURF_SNOW_GREY,

0,	// SURF_BLOOD,				
0,	// SURF_LIQUID_BLACK,
1,	// SURF_GLASS,
1,	// SURF_GLASS_COMPUTER,
1,	// SURF_SODAMACHINE,

1,	// SURF_PAPERWALL,			
1,	// SURF_NEWSPAPER_DAMAGE,
0,	// SURF_METAL_STEAM
};

//-------------------------------------------------------------------------
//
//						TempEnt Stuff
//
//-------------------------------------------------------------------------

void FX_MakeBloodSpray(vec3_t pos, vec3_t splatDir, int vel, int amount, edict_t *source)
{
	trace_t tr;
	vec3_t	end;

	if(lock_blood)
	{
		return;
	}
	
	if(!amount)return;

	VectorMA(pos, 128, splatDir, end);

	gi.trace (pos, NULL, NULL, end, source, MASK_SOLID, &tr);
	if((tr.fraction > .99)||(tr.ent)&&(tr.ent->solid != SOLID_BSP))
	{
		return;//fixme: tell client to do the spray & sound, but no splat
	}

	gi.WriteByte (svc_temp_entity);
	gi.WriteByte (TE_BLOODSPRAY);
	FXMSG_WriteRelativePosTR(tr);
	gi.multicast (pos, MULTICAST_PHS);
}

void FX_MakeDecalBelow(vec3_t pos, int type, unsigned int lLifeTime)
{
	if (type == FXDECAL_BLOODPOOL)
	{
		if(lock_blood)
		{
			return;
		}
		else
		{
			fxRunner.exec("gore/bloodpool", pos);
			return;
		}
	}
	

	if (0 == lLifeTime)
	{
		lLifeTime = 10000;
	}

	gi.WriteByte (svc_temp_entity);
	switch(type)
	{
	case FXDECAL_BLOODPOOL:
		gi.WriteByte (TE_BLOODPOOL);
		break;
	case FXDECAL_SCORCHMARK:
		gi.WriteByte (TE_SCORCHMARK);
		break;
	case FXDECAL_OOZE:
		gi.WriteByte (TE_OOZE);
		break;
	case FXDECAL_PIPELEFT:
		gi.WriteByte (TE_PIPELEFT);
		break;
	case FXDECAL_PUDDLE:
		gi.WriteByte (TE_PUDDLE);
		break;
	}
	gi.WritePosition (pos);
	gi.WriteLong (lLifeTime);
	gi.multicast (pos, MULTICAST_PHS);
}

void FX_MakeSparks(vec3_t pos, vec3_t dir, int type)
{
	gi.WriteByte (svc_temp_entity);
	switch(type)
	{
	case 0:
		gi.WriteByte (TE_ORANGE_SPARKS);
		break;
	case 1:
		gi.WriteByte (TE_BLUE_SPARKS);
		break;
	case 2:
		gi.WriteByte (TE_BARREL_SPARKS);
		break;
	}
	gi.WritePosition (pos);
	gi.WriteDir(dir);
	gi.multicast (pos, MULTICAST_PHS);
}

void FX_MakeGumballs(vec3_t pos, vec3_t dir, int type)
{
	gi.WriteByte (svc_temp_entity);
	gi.WriteByte(TE_GUMBALLS);
	gi.WritePosition (pos);
	gi.WriteDir(dir);
	gi.multicast (pos, MULTICAST_PHS);
}

void FX_MakeCoins(vec3_t pos, vec3_t dir, int type)
{
	gi.WriteByte (svc_temp_entity);
	gi.WriteByte(TE_COINS);
	gi.WritePosition (pos);
	gi.WriteDir(dir);
	gi.multicast (pos, MULTICAST_PHS);
}

void FX_MakeDustPuff(vec3_t pos)
{
	gi.WriteByte (svc_temp_entity);
	gi.WriteByte (TE_DUST_PUFF);
	gi.WritePosition (pos);
	gi.multicast (pos, MULTICAST_PHS);
}

void FX_MakeElectricArc(vec3_t pos, int len, vec3_t dir)
{
	gi.WriteByte (svc_temp_entity);
	gi.WriteByte (TE_ELECTRIC_ARC);
	gi.WritePosition (pos);
	gi.WriteByte (len);// could compress a bit (maybe x2 or x4)
	gi.WriteDir (dir);
	gi.multicast (pos, MULTICAST_PHS);
}

void FX_MakeBulletWhiz(edict_t *ent, vec3_t offset, vec3_t dir, int power, int len)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_WHIZBULLET);
	gi.WriteByte(VectorNormalize(offset));// send length
	VectorScale(offset, 1000, offset);
	VectorScale(dir, 1000, dir);
	gi.WritePosition(offset);	// send direction
	gi.WritePosition(dir);		// direction of travel for the shot...
	VectorScale(offset, 1.0/1000.0, offset);
	VectorScale(dir, 1.0/1000.0, dir);
	gi.WriteByte(100);
	gi.WriteByte(220);
	gi.unicast(ent, false);

}

void FX_MakeRicochet(vec3_t vPos)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_RICOCHET);
	gi.WritePosition(vPos);	// send position
	gi.multicast(vPos, MULTICAST_PHS);
}

// So certain wall effects aren't generated in huge amount at the same time. Like when
// the shot gun is used. 
float walldamagetime = -999999.0;


void FX_MakeWallDamage(trace_t &tr, vec3_t dir, int size, int markType,int debris)
{
	bool tSwap;
	byte walldamagesurface;

	if(tr.surface != NULL)
	{
		if(tr.surface->flags & SURF_SKY)
		{
			return;
		}
	}

	// kef -- enabling the following #if will output to c:\user\sof.log every texture
	//you shoot along with its associated surfaceType
#if 0
//	m->wallType = tr.surface->textureinfo->surfaceType;
	char buf[512];
	byte surfaceType = tr.surface->flags >> 24;
		switch(surfaceType)
		{
		case SURF_DEFAULT:
			sprintf(buf, "SURF_DEFAULT");
			break;
		case SURF_METAL:
			sprintf(buf, "SURF_METAL");
			break;
		case SURF_SAND_YELLOW:
			sprintf(buf, "SURF_SAND_YELLOW");
			break;
		case SURF_SAND_WHITE:
			sprintf(buf, "SURF_SAND_WHITE");
			break;
		case SURF_SAND_LBROWN:
			sprintf(buf, "SURF_SAND_LBROWN");
			break;
		case SURF_SAND_DBROWN:
			sprintf(buf, "SURF_SAND_DBROWN");
			break;

		case SURF_GRAVEL_GREY:
			sprintf(buf, "SURF_GRAVEL_GREY");
			break;
		case SURF_GRAVEL_DBROWN:
			sprintf(buf, "SURF_GRAVEL_DBROWN");
			break;
		case SURF_GRAVEL_LBROWN:
			sprintf(buf, "SURF_GRAVEL_LBROWN");
			break;

		case SURF_SNOW:
			sprintf(buf, "SURF_SNOW");
			break;

		case SURF_LIQUID_BLUE:
			sprintf(buf, "SURF_LIQUID_BLUE");
			break;
		case SURF_LIQUID_GREEN:
			sprintf(buf, "SURF_LIQUID_GREEN");
			break;
		case SURF_LIQUID_ORANGE:
			sprintf(buf, "SURF_LIQUID_ORANGE");
			break;
		case SURF_LIQUID_BROWN:
			sprintf(buf, "SURF_LIQUID_BROWN");
			break;

		case SURF_WOOD_LBROWN:
			sprintf(buf, "SURF_WOOD_LBROWN");
			break;
		case SURF_WOOD_DBROWN:
			sprintf(buf, "SURF_WOOD_DBROWN");
			break;
		case SURF_WOOD_LGREY:
			sprintf(buf, "SURF_WOOD_LGREY");
			break;

		case SURF_STONE_LGREY:			// chunks.tga
			sprintf(buf, "SURF_STONE_LGREY");
			break;
		case SURF_STONE_DGREY:	// chunksdrkclgry.tga
			sprintf(buf, "SURF_STONE_DGREY");
			break;
		case SURF_STONE_LBROWN:		// chunksbrwn.tga
			sprintf(buf, "SURF_STONE_LBROWN");
			break;
		case SURF_STONE_DBROWN:		// chunksbrwn.tga
			sprintf(buf, "SURF_STONE_DBROWN");
			break;
		case SURF_STONE_WHITE:		// chunkswht.tga
			sprintf(buf, "SURF_STONE_WHITE");
			break;
		case SURF_STONE_GREEN:		// chunkgreen.tga
			sprintf(buf, "SURF_STONE_GREEN");
			break;
		case SURF_STONE_RED:			// chunkred.tga
			sprintf(buf, "SURF_STONE_RED");
			break;
		case SURF_STONE_BLACK:		// chunkblk.tga
			sprintf(buf, "SURF_STONE_BLACK");
			break;

		case SURF_GRASS_GREEN:
			sprintf(buf, "SURF_GRASS_GREEN");
			break;
		case SURF_GRASS_BROWN:
			sprintf(buf, "SURF_GRASS_BROWN");
			break;

		case SURF_LIQUID_RED:
			sprintf(buf, "SURF_LIQUID_RED");
			break;

		case SURF_METAL_STEAM:
			sprintf(buf, "SURF_METAL_STEAM");
			break;
		case SURF_METAL_WATERJET:
			sprintf(buf, "SURF_METAL_WATERJET");
			break;
		case SURF_METAL_OILSPURT:
			sprintf(buf, "SURF_METAL_OILSPURT");
			break;
		case SURF_METAL_CHEMSPURT:
			sprintf(buf, "SURF_METAL_CHEMSPURT");
			break;
		case SURF_METAL_COMPUTERS:
			sprintf(buf, "SURF_METAL_COMPUTERS");
			break;

		case SURF_SNOW_LBROWN:
			sprintf(buf, "SURF_SNOW_LBROWN");
			break;
		case SURF_SNOW_GREY:
			sprintf(buf, "SURF_SNOW_GREY");
			break;
		case SURF_BLOOD:
			sprintf(buf, "SURF_BLOOD");
			break;
		case SURF_LIQUID_BLACK:
			sprintf(buf, "SURF_LIQUID_BLACK");
			break;
		case SURF_GLASS:
			sprintf(buf, "SURF_GLASS");
			break;
		case SURF_GLASS_COMPUTER:
			sprintf(buf, "SURF_GLASS_COMPUTER");
			break;
		case SURF_SODAMACHINE:
			sprintf(buf, "SURF_SODAMACHINE");
			break;
		case SURF_PAPERWALL:
			sprintf(buf, "SURF_PAPERWALL");
			break;
		case SURF_NEWSPAPER_DAMAGE:
			sprintf(buf, "SURF_NEWSPAPER_DAMAGE");
			break;
		}

	Com_Printf("___ %s %s\n", tr.surface->textureinfo->name, 
		buf);
#endif

	//WOOP WOOP!  this test is based upon the test inside of FXMSG_WriteRelativePosTR to determine
	// whether blood should be thrown.  I think it's a bad test, but I'm paralleling (good verb) the
	// test for now.
	if ((tr.ent) && ((tr.ent->ai)||(tr.ent->client)) && (lock_blood))
	{
		return;
	}

	// So certain wall effects aren't generated in huge amount at the same time. Like when
	// the shot gun is used. 
	walldamagesurface = tr.surface->flags >> 24;

	if (walldamagesurface >= 0)
	{
		if (wallEffectFlag[walldamagesurface])
		{
			if (walldamagetime>level.time)
				return;

			if (walldamagetime<level.time)
				walldamagetime=level.time + .2f;
		}
	}

	tSwap = gi.DamageTexture(tr.surface, 25);

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte((tSwap) ? TE_WALLSEVEREDAMAGE:TE_WALLDAMAGE);
	FXMSG_WriteRelativePosTR(tr);
	WriteDirExp(dir);
	gi.WriteByte(size);
	gi.WriteByte(markType);
	gi.WriteByte(debris);
	gi.multicast(tr.endpos, MULTICAST_PHS);
}

void FX_MakeBarrelExplode(vec3_t pos, edict_t *source)
{
	gi.WriteByte (svc_temp_entity);
	gi.WriteByte (TE_BARREL_EXPLODE);
	FXMSG_WriteRelativePos_TestGround(pos, source);
//  FIXME  need an explosion sound.  and it's really quiet?!?
	gi.multicast (pos, MULTICAST_PHS);
}

void FX_MakeSkyLightning(void)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_SKY_LIGHTNING);
	gi.multicast(vec3_origin, MULTICAST_ALL_R);
}


float	debrisTime;
int		debrissurfType;

void FX_ThrowDebris(vec3_t	origin, vec3_t norm,byte numchunks, byte scale, byte skin,byte x_max,byte y_max,byte z_max,byte surfType)
{
	if ((skin == MAT_ROCK_FLESH) && (lock_blood))
	{
		return;
	}	

	// If the same type of debris is being thrown within a tenth of a second, don't throw it
	if (debrissurfType == surfType)	
	{
		if ((debrisTime + .1) >= level.time)
		{
			return;
		}
	}	

	debrissurfType = surfType;
	debrisTime = level.time;

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_DEBRIS);
	gi.WritePosition(origin);
	gi.WriteDir(norm);	
	gi.WriteByte(numchunks);
	gi.WriteByte(scale);
	gi.WriteByte(skin);
	gi.WriteByte(x_max);
	gi.WriteByte(y_max);
	gi.WriteByte(z_max);
	gi.WriteByte(surfType);
	gi.multicast(origin, MULTICAST_PHS);
}

void FX_C4Explosion(edict_t *ent)
{
	vec3_t dir;
	vec3_t	pos;

	AngleVectors(ent->s.angles, dir, NULL, NULL);

	VectorMA(ent->s.origin, 2.0, dir, pos);

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_C4EXPLODE);
	FXMSG_WriteRelativePos_Dir(pos, dir, ent);
//	FXMSG_WriteRelativePos_Dir(ent->s.origin, dir, ent);
	gi.multicast(ent->s.origin, MULTICAST_PHS);
}

void FX_Flashpack(vec3_t pos)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_FLASHPACK);
	gi.WritePosition(pos);
	gi.multicast(pos, MULTICAST_PHS);
}

void FX_NeuralPulse(vec3_t pos)
{
	vec3_t spot;

	VectorCopy(pos, spot);
	spot[2] += 16;

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_NEURALPULSE);
	gi.WritePosition(spot);
	gi.multicast(spot, MULTICAST_PHS);
}

void FX_MakeAutoGunMuzzleFlash(edict_t *self)
{
#if 0
	vec3_t	pos;
	VectorCopy(self->s.origin, pos);

	// write uuid of left muzzle and right muzzle
	gi.WriteShort(myInst->MyUUID());
	gi.WriteShort(myInst->MyUUID());
	int		angle;
	//do adjustments here

	angle = NormalizeAngle(self->s.angles[1]+self->s.angle_diff);
	pos[0] += 30 * cos(angle*DEGTORAD);
	pos[1] += 30 * sin(angle*DEGTORAD);
	pos[2] += 3;

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_AUTOGUNMUZZLEFLASH);
	gi.WritePosition(pos);
	gi.WriteByte((byte)(angle/2));

	gi.multicast(pos, MULTICAST_PHS);
#endif
}

void FX_StrikeWall(vec3_t spot, byte wallType)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_WALLSTRIKE);
	gi.WritePosition(spot);
	gi.WriteByte(wallType);
	gi.multicast(spot, MULTICAST_PHS);
}

void FX_BubbleTrail(vec3_t start, vec3_t end)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_UNDERWATER_BULLET);
	gi.WritePosition(start);
	gi.WritePosition(end);
	gi.multicast(start, MULTICAST_PHS);
}

void FX_SmokeBurst(vec3_t pos, float size)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_SMOKE_BURST);
	gi.WritePosition(pos);
	gi.WriteByte(size);
	gi.multicast(pos, MULTICAST_PHS);
}

void FX_MakeLine(vec3_t start, vec3_t end, paletteRGBA_t color, int lifetime, int mcastType)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_TEST_LINE);
	gi.WritePosition(start);
	gi.WritePosition(end);
	gi.WriteLong(color.c);
	gi.WriteLong(lifetime);
	gi.multicast(start, (multicast_t)mcastType);
}

void FX_WallSparks(vec3_t pos, vec3_t dir, float size)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_WALLSPARKS);
	gi.WritePosition(pos);
	gi.WriteDir(dir);
	gi.WriteByte(size);
	gi.multicast(pos, MULTICAST_PHS);
}

void FX_Bleed(edict_t *ent, GhoulID boltID, int amount)
{
	if (boltID==0)
		return;		// We don't want blood on a random bolt, that looks extraordinarily stupid.

	if(lock_blood)
	{
		fxRunner.exec("gore/noblood", ent, boltID);
	}
	else
	{
		fxRunner.exec("gore/stumpblood", ent, boltID);
	}
}

void FX_BloodCloud(edict_t *ent, GhoulID boltID, int amount)
{
	if (boltID==0)
		return;		// We don't want blood on a random bolt, that looks extraordinarily stupid.

	if (amount > 50)
	{
		if(lock_blood)
		{
			fxRunner.exec("gore/noblood", ent, boltID);
		}
		else
		{
			fxRunner.exec("gore/bloodcloud", ent, boltID);
		}
	}
	else
	{
		if(lock_blood)
		{
			fxRunner.exec("gore/noblood_s", ent, boltID);
		}
		else
		{
			fxRunner.exec("gore/bloodcloud_s", ent, boltID);
		}
	}
}

void FX_HeadExplosion(edict_t *ent, GhoulID boltID)
{
	if(lock_blood)
	{
		return;
	}
	
	fxRunner.exec("gore/headcloud", ent, boltID);
}

void FX_BloodJet(edict_t *ent, vec3_t spot, vec3_t jetDir)
{
	if(lock_blood)
	{
		return;
	}
	
	fxRunner.setDir(jetDir);
	fxRunner.exec("environ/gb_exitw", spot);
}

void FX_HeliGun(edict_t *ent, IGhoulInst *heli, IGhoulInst *muzzle, GhoulID heliToMuzzleBolt, GhoulID muzEnd)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_HELIGUN);
	gi.WriteShort(heli->MyUUID());
	gi.WriteShort(muzzle->MyUUID());
	gi.WriteShort(heliToMuzzleBolt);
	gi.WriteShort(muzEnd);
	gi.WriteShort(ent - g_edicts);
	gi.multicast(ent->s.origin, MULTICAST_PHS);
}

void FX_WhiteBurn(edict_t *ent)
{
	IGhoulInst *myInst = ent->ghoulInst;

	if(!myInst)return;//hmm...

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_WHITEBURN);
	gi.WriteShort(myInst->MyUUID());
	gi.WriteShort(ent - g_edicts);
	gi.multicast(ent->s.origin, MULTICAST_PHS);
}

void FX_WhiteBlast(vec3_t org)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_WHITEBLAST);
	gi.WritePosition(org);
	gi.multicast(org, MULTICAST_PHS);
}

void FX_MinimiTrail(edict_t *ent)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_MINIMISMOKE);
	gi.WriteShort(ent - g_edicts);
	gi.multicast(ent->s.origin, MULTICAST_PHS);
}

void FX_Explosion(vec3_t pos, byte size)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_EXPLOSION);
	gi.WritePosition(pos);
	gi.WriteByte(size);
	gi.multicast(pos, MULTICAST_PHS);
}

void FX_VehicleExplosion(vec3_t pos, byte size)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_VEHICLEEXPLOSION);
	gi.WritePosition(pos);
	gi.WriteByte(size);
	gi.multicast(pos, MULTICAST_PHS);
}

void FX_SmokePuff(vec3_t pos,byte red,byte green,byte blue,byte alpha)
{
	gi.WriteByte (svc_temp_entity);
	gi.WriteByte (TE_SMOKE_PUFF);
	gi.WritePosition (pos);
	gi.WriteByte (red);
	gi.WriteByte (green);
	gi.WriteByte (blue);
	gi.WriteByte (alpha);

	gi.multicast (pos, MULTICAST_PHS);
}

void FX_LittleExplosion(vec3_t pos, byte size, byte bSound)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_LITTLEEXPLOSION);
	gi.WritePosition(pos);
	gi.WriteByte(size);
	gi.WriteByte(bSound);
	gi.multicast(pos, MULTICAST_PHS);
}

void FX_MediumExplosion(vec3_t pos, byte size, byte bSound)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_MEDIUMEXPLOSION);
	gi.WritePosition(pos);
	gi.WriteByte(size);
	gi.WriteByte(bSound);
	gi.multicast(pos, MULTICAST_PHS);
}

void FX_LargeExplosion(vec3_t pos, byte size, byte bSound)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_LARGEEXPLOSION);
	gi.WritePosition(pos);
	gi.WriteByte(size);
	gi.WriteByte(bSound);
	gi.multicast(pos, MULTICAST_PHS);
}

void FX_MakeRing(vec3_t pos, short size)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_MAKERING);
	gi.WritePosition(pos);
	gi.WriteShort(size);
	gi.multicast(pos, MULTICAST_PVS);
}

void FX_PaperCloud(vec3_t vPos, byte nCount/*0 generates random num of papers*/)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_PAPERCLOUD);
	gi.WritePosition(vPos);
	gi.WriteShort(nCount);
	gi.multicast(vPos, MULTICAST_PHS);
}

void FX_SodaCans(vec3_t vPos, vec3_t vDir, byte nSpeed, byte nCount/*0 generates random num of cans*/)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_SODACANS);
	gi.WritePosition(vPos);
	gi.WritePosition(vDir);
	gi.WriteShort(nSpeed);
	gi.WriteShort(nCount);
	gi.multicast(vPos, MULTICAST_PHS);
}

void FX_FloorSpace(edict_t *ent)
{
	vec3_t spot;
	
	VectorCopy(ent->s.origin, spot);
	spot[2] -= 256;

	trace_t tr;

	gi.trace(ent->s.origin, vec3_origin, vec3_origin, spot, ent, MASK_SOLID, &tr);

	VectorCopy(tr.endpos, spot);
	spot[2] += 1.0;

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_FLOORSPACE);
	gi.WritePosition(spot);
	gi.multicast(spot, MULTICAST_PVS);
}

void FX_DropDustBits(vec3_t spot, int amount)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_DUSTBITS);
	gi.WritePosition(spot);
	gi.WriteShort(amount);
	gi.multicast(spot, MULTICAST_PVS);
}

void FX_PhoneHit(vec3_t vPos)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_PHONEHIT);
	gi.WritePosition(vPos);	// send position
	gi.multicast(vPos, MULTICAST_PHS);
}

void FX_TrashcanHit(vec3_t vPos)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_TRASHCANHIT);
	gi.WritePosition(vPos);	// send position
	gi.multicast(vPos, MULTICAST_PHS);
}

// Do not hit a surface
void FX_HandleDMShotEffects(vec3_t start, vec3_t dir, vec3_t end, edict_t *shooter, int tracerfx, IGhoulInst *gun, int muzzlefx)
{
	if(!shooter->client)
	{
		return;
	}

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_DMSHOT);

	gi.WriteByte(muzzlefx);
	gi.WriteByte(tracerfx);

	gi.WriteByte(shooter - g_edicts);
	gi.WritePosition(start);
	gi.WritePosition(end);

	gi.multicast(start, MULTICAST_PHS);
}

// Hit a surface with a normal effect.
void FX_HandleDMShotHitEffects(vec3_t start, vec3_t dir, vec3_t end, edict_t *shooter, int tracerfx, IGhoulInst *gun, int muzzlefx)
{
	if(!shooter->client)
	{
		return;
	}
	vec3_t bigDir;

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_DMSHOTHIT);

	gi.WriteByte(muzzlefx);
	gi.WriteByte(tracerfx);

	gi.WriteByte(shooter - g_edicts);
	gi.WritePosition(start);

	VectorScale(dir, 2048, bigDir);
	gi.WritePosition(bigDir);//maybe send as 2byte?

	gi.multicast(start, MULTICAST_PHS);
}

void FX_HandleDMShotBigHitEffects(vec3_t start, vec3_t dir, vec3_t end, edict_t *shooter, int tracerfx, IGhoulInst *gun, int muzzlefx)
{
	if(!shooter->client)
	{
		return;
	}
	vec3_t bigDir;

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_DMSHOTBIGHIT);

	gi.WriteByte(muzzlefx);
	gi.WriteByte(tracerfx);

	gi.WriteByte(shooter - g_edicts);
	gi.WritePosition(start);

	VectorScale(dir, 2048, bigDir);
	gi.WritePosition(bigDir);//maybe send as 2byte?

	gi.multicast(start, MULTICAST_PHS);
}

void FX_HandleDMMuzzleFlash(vec3_t start, vec3_t dir, edict_t *shooter, IGhoulInst *gun, int muzzlefx)
{
	if(!shooter->client)
	{
		return;
	}
	vec3_t bigDir;

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_DMMUZZLEFLASH);

	gi.WriteByte(muzzlefx);

	gi.WriteByte(shooter - g_edicts);
	gi.WritePosition(start);

	VectorScale(dir, 2048, bigDir);
	gi.WritePosition(bigDir);//maybe send as 2byte?

	gi.multicast(start, MULTICAST_PHS);
}



//-----------------------------------------------------------------------------------------

// angvel is stored 1 bit for sign, 6 bits for whole part, and 9 bits for decimal

int AngvelInRotateRange(vec3_t angVel)
{
	for(int i = 0; i < 3; i++)
	{
		if(fabs(angVel[i]) > 31)
		{
			return 0;
		}
	}

	return 1;
}

void FXA_SetRotate(edict_t *targ, vec3_t angVel, IGhoulInst *bolton, IGhoulInst *bolt2, IGhoulInst *bolt3, IGhoulInst *bolt4)
{
	// if this assertion fails, you have given an angular velocity that is too big - 31 is the max value positive or negative...
	// if this comes up (and I hope it doesn't) we can change it - but this keeps it cheaper
	assert(AngvelInRotateRange(angVel));

	// kef -- terribly important that the effectData is all cleared out aside from the bolt data we wish to save
	memset(targ->s.effectData, 0, sizeof(aEffectInfo_t) * NUM_EFFECTS);

	targ->s.effects |= EF_ROTATE;
	eft_rotate_t	*rot = (eft_rotate_t *)targ->s.effectData;

	rot->buffer = 999;//?
	rot->xRot = angVel[0] * 512;
	rot->yRot = angVel[1] * 512;
	rot->zRot = angVel[2] * 512;
	if (bolton)
	{
		rot->boltonID = bolton->MyUUID();
	}
	else
	{
		rot->boltonID = 0;
	}
	if (bolt2)
	{
		rot->boltonID2 = bolt2->MyUUID();
	}
	else
	{
		rot->boltonID2 = 0;
	}
	if (bolt3)
	{
		rot->boltonID3 = bolt3->MyUUID();
	}
	else
	{
		rot->boltonID3 = 0;
	}
	if (bolt4)
	{
		rot->boltonID4 = bolt4->MyUUID();
	}
	else
	{
		rot->boltonID4 = 0;
	}
}


eft_rotate_t *FXA_GetRotate(edict_t *targ)
{
	if(!(targ->s.effects & EF_ROTATE))
	{
		return 0;
	}

	return (eft_rotate_t *)targ->s.effectData;
}

void FXA_RemoveRotate(edict_t *targ)
{
	targ->s.effects &= ~EF_ROTATE;

	memset(targ->s.effectData, 0, sizeof(aEffectInfo_t) * NUM_EFFECTS);
}


// Sets the entity flags so that the item fades out over a certain amount of time.
void FXA_SetFadeOut(edict_t *targ, float duration)
{
	eft_fade_t *fade = (eft_fade_t *)(targ->s.effectData);

	// First, check if we are already fading in or out.

	// Nah, let it look bad if there already is a fade.  It is more solid that way.
#if 0
	if (targ->s.effects & EF_FADE)
	{
		// Is it fading out already? (dur>0)
		if (fade->duration > 0)
		{	// Is last fade long since past?
			if (((float)(fade->startTime + fade->duration)/1000.0) > level.time)
			{	// Still fading last fade, don't do anything
				return;
			}
		}

		// Overwrite effect if fading in. (dur < 0)
	}
#endif

	// Get rid of all continual effects.  Why?  Well, we will be invalidating the data.
	fxRunner.clearContinualEffects(targ);
	targ->s.effects &= ~EF_INVIS_PULSE;
	targ->s.effects |= EF_FADE;

	fade->startTime = (int)(level.time * 1000.0);		// Convert current time to ms.
	fade->duration = (int)(duration * 1000.0);			// Convert duration to ms.
}



void FXA_SetFadeIn(edict_t *targ, float duration)
{
	eft_fade_s *fade;

	fade = (eft_fade_t *)(targ->s.effectData);

	// First, check if we are already fading in or out.

	// Nah, let it look bad if there already is a fade.  It is more solid that way.
#if 0
	if (targ->s.effects & EF_FADE)
	{
		// Is it fading in already? (dur<0)
		if (fade->duration < 0)
		{	// Is last fade long since past?
			if (((float)(fade->startTime - fade->duration)/1000.0) > level.time)
			{	// Still fading last fade, don't do anything
				return;
			}
		}

		// Overwrite effect if fading out. (dur > 0)
	}
#endif

	// Get rid of all continual effects.  Why?  Well, we will be invalidating the data.
	fxRunner.clearContinualEffects(targ);
	targ->s.effects &= ~EF_INVIS_PULSE;
	targ->s.effects |= EF_FADE;
	
	fade->startTime = (int)(level.time * 1000.0);		// Convert current time to ms.
	fade->duration = -(int)(duration * 1000.0);			// Convert duration to ms.  Negative indicates fade in.
}



// Returns true if the object should be removed.
// Removes the effects flag if completely fading in.
int FXA_CheckFade(edict_t *targ)		
{
	if (!(targ->s.effects & EF_FADE))
		return false;		// Keep object, no fade.
	
	eft_fade_s *fade = (eft_fade_t *)(targ->s.effectData);

	// Is it fading out?
	if (fade->duration > 0)
	{
		if ((float)(fade->startTime + fade->duration)/1000.0 <= level.time)
		{	// Done fading.
			// Turn off fade variables
			targ->s.effects &= ~EF_FADE;
			fade->duration = 0;
			fade->startTime = 0;

			// Tint to fully transparent until removed
			if (targ->ghoulInst)
			{
				targ->ghoulInst->SetTintOnAll(1.0,1.0,1.0,0.0);
			}

			return true;	// Kill object
		}
	}
	else
	{	// It must be fading in...
		if ((float)(fade->startTime + fade->duration)/1000.0 <= level.time)
		{	// Done fading.
			// Turn off fade variables
			targ->s.effects &= ~EF_FADE;
			fade->duration = 0;
			fade->startTime = 0;

			// Tint to fully opaque
			if (targ->ghoulInst)
			{
				targ->ghoulInst->SetTintOnAll(1.0,1.0,1.0,1.0);
			}

			// Make sure the client entity is opaque.
			FX_SetEvent(targ, EV_TINTCLEAR);
		}
	}

	return false;
}








#define SCALESHIFT 0x10

CFXSender fxRunner;

int CFXSender::findEffectID(int effectID, edict_t *ent, GhoulID bolt)
{
	int test = effectID;

	for(int i = 0; i < NUM_EFFECTS; i++)
	{
		if(ent->s.effectData[i].effectId == test)
		{
			if(bolt)
			{
				if(bolt == ent->s.effectData[i].bolt)
				{
					return i;
				}
			}
			else
			{
				return i;
			}
		}
	}
	return -1;
}

void CFXSender::clear(void)
{
	flags = 0;
}

void CFXSender::sendData(vec3_t source, multicast_t castType, edict_t *ignoreClient)
{
	if(flags)
	{
		gi.WriteByte(flags);

		if(flags & EFF_SCALE)
		{
			gi.WriteShort(scale);
		}
		if(flags & EFF_NUMELEMS)
		{
			gi.WriteByte(numElements);
		}
		if(flags & EFF_POS2)
		{
			gi.WritePosition(pos2);
		}
		if(flags & EFF_DIR)
		{	//dir is not enough
			vec3_t bigDir;
			VectorScale(dir, 2048, bigDir);
			gi.WritePosition(bigDir);
		}
		if(flags & EFF_MIN)
		{
			gi.WritePosition(min);
		}
		if(flags & EFF_MAX)
		{
			gi.WritePosition(max);
		}
		if(flags & EFF_LIFETIME)
		{
			gi.WriteShort(lifeTime);
		}
		if(flags & EFF_RADIUS)
		{
			gi.WriteByte(radius);
		}

		clear();
	}

	sendFlags = 0;

	if(ignoreClient)
	{
		assert(ignoreClient - g_edicts > 0);
		assert(ignoreClient - g_edicts < MAX_EDICTS);//um, yeah!
		gi.multicastignore(source, castType, ignoreClient-g_edicts);
	}
	else
	{
		gi.multicast(source, castType);
	}
}

void CFXSender::exec(int effectID, vec3_t pos, multicast_t castType, edict_t *ignoreClient)
{
	if(!effectID)
	{
		return;
	}

	int id = effectID;

	gi.WriteByte(svc_effect);
	gi.WriteByte(id);

	sendFlags |= EFAT_POS;
	if(flags)sendFlags |= EFAT_HASFLAGS;

	gi.WriteByte(sendFlags);
	gi.WritePosition(pos);

	sendData(pos, castType, ignoreClient);
}

void CFXSender::exec(int effectID, edict_t *ent, multicast_t castType, edict_t *ignoreClient)
{
	if(!effectID)
	{
		return;
	}

	int id = effectID;

	gi.WriteByte(svc_effect);
	gi.WriteByte(id);

	sendFlags |= EFAT_ENT;
	if(flags)sendFlags |= EFAT_HASFLAGS;

	gi.WriteByte(sendFlags);
	gi.WriteShort(ent - g_edicts);

	sendData(ent->s.origin, castType, ignoreClient);
}

void CFXSender::exec(int effectID, edict_t *ent, GhoulID bolt, multicast_t castType, int usesAltAxis, edict_t *ignoreClient)
{
	if(!effectID)
	{
		return;
	}

	int id = effectID;

	gi.WriteByte(svc_effect);
	gi.WriteByte(id);

	sendFlags |= EFAT_BOLT;
	if(flags)sendFlags |= EFAT_HASFLAGS;

	if(usesAltAxis)
	{	// seems quite a few folks use this, sadly
		sendFlags |= EFAT_ALTAXIS;
	}

	gi.WriteByte(sendFlags);
	gi.WriteShort(ent - g_edicts);
	gi.WriteShort(bolt);

	// if this assert gets hit, please contact me (Nathan)
	assert(ent->ghoulInst && ent->ghoulInst->GetGhoulObject() && bolt <= ent->ghoulInst->GetGhoulObject()->NumParts());

	sendData(ent->s.origin, castType, ignoreClient);
}

void CFXSender::execWithInst(int effectID, edict_t *ent, IGhoulInst *inst, GhoulID bolt, multicast_t castType, int usesAltAxis, edict_t *ignoreClient)
{
	if(!effectID)
	{
		return;
	}

	int id = effectID;

	gi.WriteByte(svc_effect);
	gi.WriteByte(id);

	sendFlags |= EFAT_BOLT|EFAT_BOLTANDINST;
	if(flags)sendFlags |= EFAT_HASFLAGS;

	if(usesAltAxis)
	{	// seems quite a few folks use this, sadly
		sendFlags |= EFAT_ALTAXIS;
	}

	gi.WriteByte(sendFlags);
	gi.WriteShort(ent - g_edicts);
	gi.WriteShort(inst->MyUUID());
	gi.WriteShort(bolt);

	// if this assert gets hit, please contact me (Nathan)
	assert(inst && inst->GetGhoulObject() && bolt <= inst->GetGhoulObject()->NumParts());

	sendData(ent->s.origin, castType, ignoreClient);
}

void CFXSender::execFromRandomBolt(int effectID, edict_t *ent, multicast_t castType, int usesAltAxis, edict_t *ignoreClient)
{
	exec(effectID, ent, 0, castType, usesAltAxis, ignoreClient);
}

void CFXSender::execContinualEffect(int effectID, edict_t *ent, GhoulID bolt, float size)
{
	assert(!(ent->s.effects & EF_ROTATE));
	// if this assert gets hit, please contact me (Nathan)
	assert((!bolt) || (ent->ghoulInst && ent->ghoulInst->GetGhoulObject() && bolt <= ent->ghoulInst->GetGhoulObject()->NumParts()));
	for(int i = 0; i < NUM_EFFECTS; i++)
	{
		if(ent->s.effectData[i].effectId == 0)
		{
			ent->s.effectData[i].effectId = effectID;
			ent->s.effectData[i].bolt = bolt;
			ent->s.effectData[i].size = (float)size * SCALESHIFT;//hmmm...  about .05 precision
			return;
		}
	}
}

void CFXSender::execContinualEffectFromRandomBolt(int effectID, edict_t *ent, float size)
{
	execContinualEffect(effectID, ent, 0, size);
}

void CFXSender::stopContinualEffect(int effectID, edict_t *ent, GhoulID bolt)
{
	int effectNum = findEffectID(effectID, ent, bolt);

	if(effectNum != -1)
	{
		ent->s.effectData[effectNum].effectId = 0;
	}
}

void CFXSender::editContinualEffect(int effectID, edict_t *ent, GhoulID bolt, float newSize)
{
	int effectNum = findEffectID(effectID, ent, bolt);

	if(effectNum != -1)
	{
		ent->s.effectData[effectNum].size = (float)newSize * SCALESHIFT;
	}
}

void CFXSender::clearContinualEffects(edict_t *ent)
{
	memset(ent->s.effectData, 0, sizeof(ent->s.effectData));
	
	// Clear EF_FLAGs that have data associated with them as well.
	ent->s.effects &= ~(EF_ROTATE | EF_FADE);

	// Also, make sure that the entity is fully opaque.
	if (ent->ghoulInst)
		ent->ghoulInst->SetTint(1.0,1.0,1.0,1.0);

	// Make sure the client entity is opaque.
	FX_SetEvent(ent, EV_TINTCLEAR);
}

float CFXSender::getEffectSizeVal(int effectID, edict_t *ent, GhoulID bolt)
{
	int effectNum = findEffectID(effectID, ent, bolt);

	if(effectNum != -1)
	{
		return ent->s.effectData[effectNum].size / SCALESHIFT;
	}
	return 0;
}

int CFXSender::hasEffect(int effectID, edict_t *ent, GhoulID bolt)
{
	int effectNum = findEffectID(effectID, ent, bolt);

	if(effectNum != -1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}