#include "g_local.h"
#include "matrix4.h"
#include "ai_private.h"
#include "callback.h"
#include "ai_pathfinding.h"

extern int bulletTraceHitCheck(trace_t *tr, vec3_t start, vec3_t end, int clipMask);
extern void DropKnife(edict_t &monster, edict_t *knife);
extern float Skill_DisablityModifier(edict_t *monster);

bodydekker_c::bodydekker_c()
: bodymeso_c()
{
	lastDekkerPain=0.0;
	totalDamage=0;
}

bodydekker_c::bodydekker_c(bodydekker_c *orig)
: bodymeso_c(orig)
{
	lastDekkerPain = orig->lastDekkerPain;
	totalDamage = orig->totalDamage;
}

void bodydekker_c::Evaluate(bodydekker_c *orig)
{
	lastDekkerPain = orig->lastDekkerPain;
	totalDamage = orig->totalDamage;

	bodymeso_c::Evaluate(orig);
}

void bodydekker_c::Read()
{
	char	loaded[sizeof(bodydekker_c)];

	gi.ReadFromSavegame('AIBK', loaded, sizeof(bodydekker_c));
	Evaluate((bodydekker_c *)loaded);
}

void bodydekker_c::Write()
{
	bodydekker_c	*savable;

	savable = new bodydekker_c(this);
	gi.AppendToSavegame('AIBK', savable, sizeof(*this));
	delete savable;

	WriteGZ();
}

mmove_t *bodydekker_c::GetSequenceForPain(edict_t &monster, vec3_t point, float kick, int damage, vec3_t dest, vec3_t face, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	if (level.time-lastDekkerPain<5.0)
	{
		return NULL;
	}
	lastDekkerPain=level.time;
	return bodyhuman_c::GetSequenceForPain(monster, point, kick, damage, dest, face, seqtype, goal_bbox, ideal_move, reject_actionflags);
}

#define MAX_FRONTCODES	5

int bodydekker_c::ShowDamage (edict_t &monster, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb)
{
	vec3_t		bodyloc;//, footorigin;
	gz_code		listOfFrontCodes[MAX_FRONTCODES];
	int			numOfFrontCodes=0;
	gz_code		frontCode=numGoreZones;
	gz_code		backCode=numGoreZones;
	vec3_t		forward, right, up;
	vec3_t		to_impact;
	ggOinstC	*myInstance=game_ghoul.FindOInst(monster.ghoulInst);
	int			i,j,backHoleHitNum;
	int			take;
	float		curGoreDist,bestGoreDist,tdist;
	Vect3		EntryPos;
	bool		EntryPosIsValid=false;
	vec3_t		ExitPos,tvec;
	Vect3		zonePos;
	Vect3		Direction;
	Matrix4		ZoneMatrix;
	ggBinstC	*oldLeftWeap, *oldRightWeap;
	ggBinstC	*boltPiece=NULL;//contender for being shot off
	qboolean	oneLimbSevered=false;
//	vec3_t		debrisNorm;
	bool		bKnife = false;
	bool		bArmor = false; // need to know this to handle the knife properly
	int			nNearMissZone = -1;

	bDropBoltons = false; // Dekker can't have stuff blown off of him

	if (attacker==&monster)
	{
		return 0;
	}

	dflags&=~(DT_SEVER|DT_MANGLE);

	//first, store off what i got in my hands...
	oldLeftWeap=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l"));
	oldRightWeap=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r"));

	if (monster.spawnflags & SPAWNFLAG_NO_WOUND)
	{	// rather than just exit without wounding, let's fire our killtarget here
		monster.spawnflags &= ~SPAWNFLAG_NO_WOUND;
		edict_t *t = NULL;
		if (t = G_Find (t, FOFS(targetname), monster.killtarget))
		{
			t->use(t, &monster, &monster);
			monster.killtarget = NULL;
		}
	}

	// if we're the target of a scripted kill...well...we're gonna die right the heck now
	switch(nNextShotKillsMe)
	{
	case NEXTSHOT_EXPLODE_MY_NUG:
		{
			dflags |= DT_SEVER;
			// no break...intentional.
		}
	case NEXTSHOT_KILLS:
		{
			damage = 9999;

			// I suppose that bad things might happen if this flag is on and we get shot again
			nNextShotKillsMe = NEXTSHOT_NULL;
			break;
		}
	default:
		//i'm dekker, be gentle...
		if (damage > 10)
		{
			damage = 10;
		}
		break;
	}
	
	for (i=0;i<3;i++)
	{
		Direction[i]=dir[i];
	}

	if (inflictor)
	{
		bKnife = ((254 == inflictor->elasticity) && attacker->client);
	}

	//if unknown type of damage, don't even bother, just accept the value
	if( !(dflags & (DT_PROJECTILE|DT_MELEE|DT_DIRECTIONALGORE|DT_MANGLE)) &&
		!bKnife)
	{
//		return damage;//other tests?  I dunno - knife should be here maybe too
		if (damage<1)
		{
			return damage;
		}
		if (damage<5 && gi.irand(0,1))//just magical armor is all
		{
			return 0;
		}

		return 1;//other tests?  I dunno - knife should be here maybe too
	}
	else if (bKnife)
	{
		// need to perform special trace here, treating thrown knife like a bullet
		trace_t	tr;
		vec3_t	endPos, firePoint, fwd;

		// knife velocity is stored in pos1 (cuz knife->velocity is zero after impact)
		VectorCopy(inflictor->pos1, fwd);
		VectorAdd(attacker->s.origin, attacker->client->ps.viewoffset, firePoint);
//		VectorCopy(attacker->s.origin, firePoint);
		VectorMA(firePoint, 2, fwd, endPos); // gives a length of 1600

		gi.polyTrace(firePoint, NULL, NULL, endPos, inflictor, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER|CONTENTS_SHOT,
			&tr, bulletTraceHitCheck);
		if (0 == bullet_numHits)
		{	// entered this guy's bbox but missed him. in a perfect world we'd be able to allow 
			//the knife to fly along its merry way til it actually hits something, but for now i'll
			//just drop it here
			if ( (nNearMissZone = HandleNearMiss(monster, inflictor)) == -1)
			{	// couldn't find a zone anywhere near the knife
				return 0;
			}
		}
		VectorCopy(fwd, dir);
		VectorCopy(endPos, point);
		VectorCopy(firePoint, origin);
		dflags = DT_PROJECTILE;
	}

	take = damage;

	//shotgun shot--plop on generic damage for now
	if (!((dflags&(DT_PROJECTILE|DT_DIRECTIONALGORE|DT_MANGLE))))
	{
		//new, very random zone-chooser.
		vec3_t myFacing, dmgFacing, mySide, myUp;

		AngleVectors(monster.s.angles, myFacing, mySide, myUp);

		VectorSubtract(monster.s.origin, origin, dmgFacing);
		VectorNormalize(dmgFacing);

		if(DotProduct(myFacing, dmgFacing) > .707)
		{//nearly colinear - being hit from behind
			backCode=GZ_CHEST_FRONT;
			switch(gi.irand(0,3))
			{
			case 0:
				listOfFrontCodes[numOfFrontCodes++]=GZ_CHEST_BACK;
				break;
			case 1:
				listOfFrontCodes[numOfFrontCodes++]=GZ_SHLDR_LEFT_BACK;
				break;
			case 2:
				listOfFrontCodes[numOfFrontCodes++]=GZ_SHLDR_RIGHT_BACK;
				break;
			default:
			case 3:
				listOfFrontCodes[numOfFrontCodes++]=GZ_GUT_BACK;
				break;
			}
		}
		else if(DotProduct(myFacing, dmgFacing) < -.707)
		{//in the front - ow!
			backCode=GZ_CHEST_BACK;
			switch(gi.irand(0,3))
			{
			case 0:
				listOfFrontCodes[numOfFrontCodes++]=GZ_CHEST_FRONT;
				break;
			case 1:
				listOfFrontCodes[numOfFrontCodes++]=GZ_SHLDR_LEFT_FRONT;
				break;
			case 2:
				listOfFrontCodes[numOfFrontCodes++]=GZ_SHLDR_RIGHT_FRONT;
				break;
			default:
			case 3:
				listOfFrontCodes[numOfFrontCodes++]=GZ_GUT_FRONT;
				break;
			}
		}
		else if(DotProduct(mySide, dmgFacing) > 0)
		{//um...
			backCode=GZ_CHEST_BACK;
			switch(gi.irand(0,4))
			{
			case 0:
				listOfFrontCodes[numOfFrontCodes++]=GZ_CHEST_FRONT;
				break;
			case 1:
				listOfFrontCodes[numOfFrontCodes++]=GZ_ARM_UPPER_LEFT;
				break;
			case 2:
				listOfFrontCodes[numOfFrontCodes++]=GZ_GUT_BACK;
				break;
			case 3:
				listOfFrontCodes[numOfFrontCodes++]=GZ_SHLDR_LEFT_BACK;
				break;
			default:
			case 4:
				listOfFrontCodes[numOfFrontCodes++]=GZ_GUT_FRONT;
				break;
			}

		}
		else
		{//er...
			backCode=GZ_CHEST_BACK;
			switch(gi.irand(0,4))
			{
			case 0:
				listOfFrontCodes[numOfFrontCodes++]=GZ_CHEST_FRONT;
				break;
			case 1:
				listOfFrontCodes[numOfFrontCodes++]=GZ_ARM_UPPER_RIGHT;
				break;
			case 2:
				listOfFrontCodes[numOfFrontCodes++]=GZ_GUT_BACK;
				break;
			case 3:
				listOfFrontCodes[numOfFrontCodes++]=GZ_SHLDR_RIGHT_BACK;
				break;
			default:
			case 4:
				listOfFrontCodes[numOfFrontCodes++]=GZ_GUT_FRONT;
				break;
			}
		}
	}
	else if(dflags & DT_DIRECTIONALGORE)
	{
		// sort out what sort of damage should be shown based on which side is being pelted
		// note that all of this is kind of a 2d representation of everything - good enough for now, I s'pose =/

		vec3_t myFacing, dmgFacing, mySide;

		AngleVectors(monster.s.angles, myFacing, mySide, 0);

		VectorSubtract(monster.s.origin, origin, dmgFacing);
		VectorNormalize(dmgFacing);

		if(DotProduct(myFacing, dmgFacing) > .707)
		{//nearly colinear - being hit from behind
			backCode=GZ_CHEST_FRONT;
			listOfFrontCodes[numOfFrontCodes++]=GZ_CHEST_BACK;
			listOfFrontCodes[numOfFrontCodes++]=GZ_SHLDR_LEFT_BACK;
			listOfFrontCodes[numOfFrontCodes++]=GZ_SHLDR_RIGHT_BACK;
			listOfFrontCodes[numOfFrontCodes++]=GZ_GUT_BACK;
		}
		else if(DotProduct(myFacing, dmgFacing) < -.707)
		{//in the front - ow!
			backCode=GZ_CHEST_BACK;
			listOfFrontCodes[numOfFrontCodes++]=GZ_CHEST_FRONT;
			listOfFrontCodes[numOfFrontCodes++]=GZ_SHLDR_LEFT_FRONT;
			listOfFrontCodes[numOfFrontCodes++]=GZ_SHLDR_RIGHT_FRONT;
			listOfFrontCodes[numOfFrontCodes++]=GZ_GUT_FRONT;
		}
		else if(DotProduct(mySide, dmgFacing) > 0)
		{//um...
			backCode=GZ_CHEST_BACK;
			listOfFrontCodes[numOfFrontCodes++]=GZ_CHEST_FRONT;
			listOfFrontCodes[numOfFrontCodes++]=GZ_ARM_UPPER_LEFT;
			listOfFrontCodes[numOfFrontCodes++]=GZ_GUT_BACK;
			listOfFrontCodes[numOfFrontCodes++]=GZ_SHLDR_LEFT_BACK;
			listOfFrontCodes[numOfFrontCodes++]=GZ_GUT_FRONT;

		}
		else
		{//er...
			backCode=GZ_CHEST_BACK;
			listOfFrontCodes[numOfFrontCodes++]=GZ_CHEST_FRONT;
			listOfFrontCodes[numOfFrontCodes++]=GZ_ARM_UPPER_RIGHT;
			listOfFrontCodes[numOfFrontCodes++]=GZ_GUT_BACK;
			listOfFrontCodes[numOfFrontCodes++]=GZ_SHLDR_RIGHT_BACK;
			listOfFrontCodes[numOfFrontCodes++]=GZ_GUT_FRONT;
		}
	}
	//projectile--go through the trace and get the best frontHole
	else
	{

	

		//NO HITS!!!
		if (bullet_numHits == 0 && (nNearMissZone == -1) )
		{
			//if i'm a client, try again :(
			if (monster.client && monster.ghoulInst)
			{
			}
			if (bullet_numHits == 0)
			{
//				gi.dprintf("Missed!\n");
				return 0;
			}
		}

		if (!bKnife)
		{
			//get exit wound
			//doing exit wound first, so i can get multiple entry wounds and stop consideration at the exit wound--the other way around wouldn't work: only want one exit wound.
			for (i=bullet_numHits-1;i>=0;i--)
			{
				//hit my instance
				if (bullet_Hits[i].Inst==myInstance->GetInstPtr())
				{
					//hit something
					if (bullet_Hits[i].Mesh)
					{
						boltPiece=NULL;//don't blow stuff off that got hit after this.

					
						//don't already have a backhole
						if (backCode==numGoreZones)
						{
							VectorScale(*(vec3_t*)(&bullet_EntDir),bullet_Hits[i].Distance,ExitPos);
							Vec3AddAssign(*(vec3_t*)(&bullet_EntStart), ExitPos);
							tdist=bestGoreDist=9999999;
							for (j=0;j<numGoreZones;j++)
							{
								//hit the part that this zone is attached to--should still check to make sure shot is entry, and find the best zone on this part.
								if (bullet_Hits[i].Mesh==gzones[j].parent_id)
								{
									if (!gzones[j].bolt)
									{
										if (backCode==numGoreZones || tdist>bullet_Hits[i].Distance)
										{
											tdist=bullet_Hits[i].Distance;
											backCode = (gz_code)j;
										}
									}
									else
									{
										//is this a better match than what's already in fronthole?
										bullet_Hits[i].Inst->GetBoltMatrix(level.time,ZoneMatrix,gzones[j].bolt,IGhoulInst::MatrixType::Entity);
										ZoneMatrix.GetRow(3,zonePos);
										VectorSubtract(ExitPos,*(vec3_t*)(&zonePos),tvec);
										curGoreDist = VectorLengthSquared(tvec);
										if (backCode==numGoreZones || curGoreDist<bestGoreDist)
										{
											tdist=bullet_Hits[i].Distance;
											backCode = (gz_code)j;
											bestGoreDist = curGoreDist;
										}
									}
								}
							}
							if (backCode!=numGoreZones)
							{
								// tdist is distance from gun point to enemy 
								VectorScale(dir,tdist,ExitPos);
								VectorAdd(origin,ExitPos,ExitPos);
								backHoleHitNum=i;
							}
						}
					}
				}
				//hit a bolt-on
				else// if (backCode==numGoreZones)
				{
					boltPiece = myInstance->FindBoltInstance(bullet_Hits[i].Inst);
				}
			}
		} // if (!bKnife)
		else
		{
			backHoleHitNum = bullet_numHits - 1;
		}

		//if the first thing this shot hit was a bolt-on, try to blow it off.
		if (boltPiece && boltPiece->GetBolterBolt())
		{
			int k;
			VectorScale(dir,tdist,*(vec3_t*)(&EntryPos));
			for (k=0;k<3;k++)
			{
				EntryPos[k]+=point[k];
			}
			bool	bTooToughToDropWeapon = false, bHitAWeapon = false;
			float	fArmorPercentLeft = GetArmorPercentRemaining();
			float	fHealthPercentLeft = (float)monster.health / (float)monster.max_health;

			ai_c *my_ai = (ai_c *)((ai_public_c *)monster.ai);

			// if this is an armored guy...
			//only let him drop his gun if he's down to < 50% armor or < 50% health.
			if(my_ai && !(my_ai->GetMySkills()->getDropWeap()))
			{	// rocket guys dropping their guns is weak
				bTooToughToDropWeapon = true;
			}
			if ( (fArmorPercentLeft > 0.50) && (fHealthPercentLeft > 0.50) )
			{
				bTooToughToDropWeapon = true;
			}
			bHitAWeapon = false;//(GetWeaponType(boltPiece) != SFW_EMPTYSLOT);
			if(monster.flags & FL_IAMTHEBOSS)
			{	// the boss hates it when you shoot his weapon.  So do so.
			}
			else if ( (bHitAWeapon && !bTooToughToDropWeapon) ||
				 (!bHitAWeapon) ||
				 dm->isDM())
			{
				take = DropBoltOn(monster, boltPiece, EntryPos, Direction, take);
			}
			else if (bHitAWeapon && bTooToughToDropWeapon)
			{	// if we hit a weapon but this guy is too tough to drop it yet, return
				//some minimal amount of damage
				return 0;//take*.25;
			}

			//did i get a weapon shot out of my hands???
			if(oldLeftWeap&&oldLeftWeap!=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l")))
			{
				lWeaponShot=true;
			}
			if (oldRightWeap&&oldRightWeap!=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r")))
			{
				rWeaponShot=true;
			}

			if (take == 0) // this is purty hacky, right hyaw...
			{
				return 0;//1;
			}
		}

		//get entrance wound--stop at one!
		for (i=0;i<bullet_numHits&&i<backHoleHitNum&&numOfFrontCodes<1;i++)
		{
			//hit my instance
			if (bullet_Hits[i].Inst==myInstance->GetInstPtr())
			{
				//hit something
				if (bullet_Hits[i].Mesh)
				{
					VectorScale(*(vec3_t*)(&bullet_EntDir),bullet_Hits[i].Distance,*(vec3_t*)(&EntryPos));
					EntryPos += bullet_EntStart;
					tdist=bestGoreDist=9999999;
					for (j=0;j<numGoreZones;j++)
					{
						//hit the part that this zone is attached to--should still check to make sure shot is entry, and find the best zone on this part.
						if (bullet_Hits[i].Mesh==gzones[j].parent_id)
						{
							if (!gzones[j].bolt)
							{
								if (frontCode==numGoreZones || tdist>bullet_Hits[i].Distance)
								{
									tdist=bullet_Hits[i].Distance;
									frontCode = (gz_code)j;
								}
							}
							else
							{
								//is this a better match than what's already in fronthole?
								bullet_Hits[i].Inst->GetBoltMatrix(level.time,ZoneMatrix,gzones[j].bolt,IGhoulInst::MatrixType::Entity);
								ZoneMatrix.GetRow(3,zonePos);
								VectorSubtract(*(vec3_t*)(&EntryPos),*(vec3_t*)(&zonePos),tvec);
								curGoreDist = VectorLengthSquared(tvec);
								if (frontCode==numGoreZones || curGoreDist<bestGoreDist)
								{
									tdist=bullet_Hits[i].Distance;
									frontCode = (gz_code)j;
									bestGoreDist = curGoreDist;
								}
							}
						}
					}
					if (frontCode!=numGoreZones)
					{
						int k;

						VectorScale(dir,tdist*(7/8),*(vec3_t*)(&EntryPos));
						for (k=0;k<3;k++)
						{
							EntryPos[k]+=point[k];
						}
						for (k=0;k<3;k++)
						{
							if (EntryPos[k]<monster.s.origin[k]+monster.mins[k])
							{
								EntryPos[k]=monster.s.origin[k]+monster.mins[k];
							}
							if (EntryPos[k]>monster.s.origin[k]+monster.maxs[k])
							{
								EntryPos[k]=monster.s.origin[k]+monster.maxs[k];
							}
						}
						EntryPosIsValid=true;

						listOfFrontCodes[numOfFrontCodes++]=frontCode;
					}
				}
			}
		}
	}

	// if we're here because of a near miss from a thrown knife, fake a couple of things
	if (nNearMissZone != -1)
	{
		numOfFrontCodes = 1;
		listOfFrontCodes[0] = nNearMissZone;
	}

	//update for monster armor here
	if (monster.ai)
	{
		//go thru all front holes, distributing damage amongst the armor
		for (i=0;i<numOfFrontCodes;i++)
		{
			frontCode=listOfFrontCodes[i];
			//make sure hole is valid
			if (frontCode >= 0 && frontCode < numGoreZones)
			{
				//if fronthole has armor, evaluate
				if (gzones[frontCode].damage<0)
				{
					// kef -- I _think_ getting in here means we hit armor
					bArmor = true;

					//first, show the armor damage (this may immediately switch over to bloodiness, but that should be ok)
					if (gzones[frontCode].id&&myInstance->GetInstPtr()->SetFrameOverride("gz", "gz_armor", gzones[frontCode].id))
					{
						myInstance->GetInstPtr()->SetPartOnOff(gzones[frontCode].id, true);
					}

					//update buddy zone, if any
					if (gzones[frontCode].buddyzone<numGoreZones && gzones[gzones[frontCode].buddyzone].id
						&& myInstance->GetInstPtr()->SetFrameOverride("gz", "gz_armor", gzones[gzones[frontCode].buddyzone].id))
					{
						myInstance->GetInstPtr()->SetPartOnOff(gzones[gzones[frontCode].buddyzone].id, true);
					}

					// this handles the special properties some of the weapons have regarding armor piercing / absorbtion
					float penetrateDmg = 0;
					if(gzones[frontCode].damage < 0)
					{
						take -= (take * absorb);
						//penetrateDmg = take * penetrate;
						//take -= penetrateDmg;
					}

					float tempDmg;//to handle armor piercers
					tempDmg = take;
					if(penetrate > 0)
					{
						tempDmg *= 2;
					}

					//all damage covered
					if (tempDmg <= -gzones[frontCode].damage)
					{
						bodyDamage&=~BODYDAMAGE_RECENT;
						gzones[frontCode].damage+=tempDmg;

						if (EntryPosIsValid)
						{
							vec3_t sparkdir;
							VectorSubtract(origin,point,sparkdir);
							VectorNormalize(sparkdir);
							VectorScale(dir,-1.0f,sparkdir);
							fxRunner.setDir(sparkdir);
							/*if(absorb)
							{
								fxRunner.exec("weakspark", *((vec3_t*)&EntryPos));//eh, using same spark as walls now--hope that's ok
							}
							else if(penetrate)
							{
								fxRunner.exec("armorhit", *((vec3_t*)&EntryPos));//eh, using same spark as walls now--hope that's ok
							}
							else*/
							{
								fxRunner.exec("spark2", *((vec3_t*)&EntryPos));//eh, using same spark as walls now--hope that's ok
							}
							gi.sound (&monster, CHAN_AUTO, gi.soundindex ("impact/armor/hit.wav"), .6, ATTN_NORM, 0);
//							FX_MakeSparks(*((vec3_t*)&EntryPos), sparkdir, 1);
						}
						// kef -- handle thrown knives properly
						if (bKnife)
						{	// just drop the knife where it hit the guy
							DropKnife(monster, inflictor);
						}
						return 0;
					}
					//not all damage covered
					else
					{
						tempDmg+=gzones[frontCode].damage;
						gzones[frontCode].damage=0;
					}
					if(penetrate > 0)
					{	// if we were destroying armor a second ago, return to our appropriate damage amount
						tempDmg *= .5;
					}
					take = tempDmg;
				}
			}
		}
	}

	//for each of the front holes, scale the damage (this should cover massive headshot damage, and lower armshot damage)
	for (i=0;i<numOfFrontCodes;i++)
	{
		frontCode=listOfFrontCodes[i];
		//make sure hole is valid
		if (frontCode >= 0 && frontCode < numGoreZones)
		{
			take=(int)((float)take*gzones[frontCode].damScale);
		}
	}

	AngleVectors(monster.s.angles, forward, right, up);
	VectorSubtract(point, monster.s.origin, to_impact);
	VectorNormalize(to_impact);

	Emote(monster, EMOTION_PAIN, 10.0);
				
	VectorSubtract(point, monster.s.origin, bodyloc);


	//DON'T ADJUST TAKE AFTER THIS POINT!!!!!!! I CHECK TO SEE IF I'M DEAD HERE!!!!!

	//dying--drop weapons
	if (monster.health <= take)
	{
		ggBinstC	*bolted;
		vec3_t		handPos;
		int			safeIters=0;
		Emote(monster, EMOTION_PAIN, 99999.0);
		VectorSet(handPos,0,0,0);

		// this is a little sad, but we can't drop boltons if we were hit by a train
		if (bDropBoltons)
		{
			// drop what's in left hand
			monster.ghoulInst->GetBoltMatrix(level.time,ZoneMatrix,monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l"),IGhoulInst::MatrixType::Entity);
	//		EntToWorldMatrix(monster.s.origin, monster.s.angles, ZoneMatrix);
			ZoneMatrix.GetRow(3,zonePos);
			handPos[0]=zonePos[0];
			handPos[1]=zonePos[1];
			handPos[2]=zonePos[2];
			while (myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l"))&&safeIters++<10)
			{
				bolted=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l"));
				DropBoltOn(monster, bolted, zonePos, Direction, take);
			}

			//drop what's in right hand
			monster.ghoulInst->GetBoltMatrix(level.time,ZoneMatrix,monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r"),IGhoulInst::MatrixType::Entity);
	//		EntToWorldMatrix(monster.s.origin, monster.s.angles, ZoneMatrix);
			ZoneMatrix.GetRow(3,zonePos);
			handPos[0]=zonePos[0];
			handPos[1]=zonePos[1];
			handPos[2]=zonePos[2];
			safeIters=0;
			while (myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r"))&&safeIters++<10)
			{
				bolted=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r"));
				DropBoltOn(monster, bolted, zonePos, Direction, take);
			}

			//lose any headgear
			monster.ghoulInst->GetBoltMatrix(level.time,ZoneMatrix,monster.ghoulInst->GetGhoulObject()->FindPart("abolt_head_t"),IGhoulInst::MatrixType::Entity);
	//		EntToWorldMatrix(monster.s.origin, monster.s.angles, ZoneMatrix);
			ZoneMatrix.GetRow(3,zonePos);
			handPos[0]=zonePos[0];
			handPos[1]=zonePos[1];
			handPos[2]=zonePos[2];
			safeIters=0;
			while (myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("abolt_head_t"))&&safeIters++<10)
			{
				bolted=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("abolt_head_t"));
				DropBoltOn(monster, bolted, zonePos, Direction, take);
			}
		} // if (bDropBoltons)

	}

	for (i=0;i<numOfFrontCodes;i++)
	{
		frontCode=listOfFrontCodes[i];

		if (!oneLimbSevered)
		{
			// kef -- handle armor properly in case of throwing knife
			if (bArmor && bKnife)
			{
				DropKnife(monster, inflictor);
				// setting inflictor to NULL will prevent ShowFrontHoleDamage from sticking the knife in our boy. it
				//shouldn't affect anything else in that fn, though.
				inflictor = NULL;
			}
			oneLimbSevered=ShowFrontHoleDamage (monster, frontCode, backCode, inflictor, attacker, dir, point, origin, take, knockback, dflags, mod);
//			oneLimbSevered=ShowFrontHoleDamage (monster, GZ_ARM_LOWER_RIGHT, backCode, inflictor, attacker, dir, point, origin, take, knockback, dflags, mod);
		}
	}

	if (!lock_gorezones)
	{
		ShowBackHoleDamage (monster, frontCode, backCode, inflictor, attacker, dir, ExitPos, origin, take, knockback, dflags, mod);
	}
	return take;
}
