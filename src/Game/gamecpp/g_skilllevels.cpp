#include "g_local.h"

CPlayerSkill skillStats [NUM_SKILL_LEVELS] = 
{				// dodge			aimMaxDist		WanderAmount	PlayerMaxEncumbrance	ArmorModifier	SpawnLull		SpawnForceTime	canCheat			Saves	TurnSpeed
				//			accuracy		Hesitation		PlayerDamage		AmmoModifier		SpawnFrequency	SpawnNonLull	SpawnMaxNearbyGuys	SpawnMinTime			fullHealth						
	CPlayerSkill(	.3f,	.25f,	0.4f,	3.0,	.5f,	.15f,	0,			2.0,		2.0,	0.0,	0,		0,		0,		0,		1,			0.0,	-1,		.5,		1),	//EXTREMELY EASY
	CPlayerSkill(	.5f,	.4f,	0.5f,	1.5f,	1.0,	.3f,	10,			1.0,		1.0,	.3,		10,		4,		2,		2,		1,			15.0,	8,		.5,		1),	//EASY
	CPlayerSkill(	1.0,	0.6f,	0.8f,	1.0f,	1.0,	.4f,	7,			1.0,		1.0,	.85,	10,		4,		1,		3,		1,			8.0,	5,		1.0,	0),	//STANDARD
	CPlayerSkill(	3,		0.8f,	1.0f,	0.7f,	1.0,	.6f,	5,			1.0,		1.0,	1.0,	12,		6,		1,		3,		0,			6.0,	2,		1.5,	0),	//DIFFICULT
	CPlayerSkill(	99.0,	1.5f,	2.5f,	0.2f,	99.0,	1.0,	5,			5.0,		.5f,	3.0,	4,		12,		.5,		4,		0,			2.0,	0,		3,		0),	//EXTREMELY DIICULT
};

// lower hesitations make the game more exciting

CPlayerSkill::CPlayerSkill(float newaiDodgeFrequency, float newaiAccuracy, float newaiAimMaxDist, float newaiHesitation,
		float newaiWanderAmount, float newplayerDamage, int newplayerMaxEncumbrance, float newammoModifier, 
		float newarmorModifier,	float newspawnFrequency, float newspawnLullTime, float newspawnNonLullTime,
		float newspawnForceTime, float newspawnMaxGuys, int newcanCheat, float newspawnMinTime, int newNumberOfSaves, 
		float newaiTurnSpeed, int newfullHealth)
{
	// fixme - aiWanderAmount is currently not implemented

	aiDodgeFrequency = newaiDodgeFrequency; 
	aiAccuracy = newaiAccuracy; 
	aiAimMaxDist = newaiAimMaxDist; 
	aiHesitation = newaiHesitation; 
	aiWanderAmount = newaiWanderAmount; 
	aiTurnSpeed = newaiTurnSpeed;

	playerDamage = newplayerDamage;
	playerMaxEncumbrance = newplayerMaxEncumbrance;
	ammoModifier = newammoModifier; 
	armorModifier = newarmorModifier; 

	spawnFrequency = newspawnFrequency; 
	spawnLullTime = newspawnLullTime; 
	spawnNonLullTime = newspawnNonLullTime; 
	spawnForceTime = newspawnForceTime; 
	spawnMaxGuys = newspawnMaxGuys;
	cheatsAvailable = newcanCheat;
	spawnMinTime = newspawnMinTime;
	numberOfSaves = newNumberOfSaves;

	fullHealth = newfullHealth;
}

/*

  Saves
Set with sk_saving
Unlimited
…
2
1
0

Spawning
Set with sk_spawning
None (0)
Light (1)
Standard (2)
Heavy (3)
Ridiculous (4)

Enemy Toughness
Set with sk_toughness
Non-Threatening (0)
Uncertain (1)
Standard (2)
Aggressive (3)
Bloodthirsty (4)

  */

void CPlayerSkill::buildCustomSkillLevels(void)
{
	// build 'em all now

	numberOfSaves = sk_saving->value;
	cheatsAvailable = sv_cheats->value;

	spawnValue = sk_spawning->value;
	if(spawnValue < 0)spawnValue = 0;
	if(spawnValue > 4)spawnValue = 4;
	spawnFrequency = skillStats[spawnValue].spawnFrequency;
	spawnLullTime = skillStats[spawnValue].spawnLullTime; 
	spawnNonLullTime = skillStats[spawnValue].spawnNonLullTime; 
	spawnForceTime = skillStats[spawnValue].spawnForceTime; 
	spawnMaxGuys = skillStats[spawnValue].spawnMaxGuys;
	spawnMinTime = skillStats[spawnValue].spawnMinTime;
	ammoModifier = skillStats[spawnValue].ammoModifier; //this is pretty spawn related
	
	enemyValue = sk_toughness->value;
	if(enemyValue < 0)enemyValue = 0;
	if(enemyValue > 4)enemyValue = 4;
	aiDodgeFrequency = skillStats[enemyValue].aiDodgeFrequency; 
	aiAccuracy = skillStats[enemyValue].aiAccuracy; 
	aiAimMaxDist = skillStats[enemyValue].aiAimMaxDist; 
	aiHesitation = skillStats[enemyValue].aiHesitation; 
	aiWanderAmount = skillStats[enemyValue].aiWanderAmount; 
	aiTurnSpeed = skillStats[enemyValue].aiTurnSpeed;
	playerDamage = skillStats[enemyValue].playerDamage; 
	armorModifier = skillStats[enemyValue].armorModifier; 
	fullHealth = skillStats[enemyValue].fullHealth;

	int	encumidx=sk_maxencum->value;
	if(encumidx<0)encumidx=0;
	if(encumidx>4)encumidx=4;
	playerMaxEncumbrance = skillStats[encumidx].playerMaxEncumbrance;
}

float CPlayerSkill::getMoneyMultiplier(void)
{
	float val = 1;

	if(numberOfSaves == -1)
	{
		val *= .4;
	}
	else if(!numberOfSaves)
	{
		val *= 2;
	}
	else
	{
		val -= numberOfSaves * .05;
	}

	if(cheatsAvailable)
	{
		val *= .5;
	}

	if(!spawnValue)
	{
		val *= .3;
	}
	else if(spawnValue == 4)
	{
		val *= 4;//this is silly
	}
	else
	{
		val += spawnValue * .3;
	}

	if(!enemyValue)
	{
		val *= .3;
	}
	else if(enemyValue == 4)
	{	//this is very painful
		val *= 3;
	}
	else
	{
		val += enemyValue * .4;
	}

	if(!playerMaxEncumbrance)
	{
		val *= .5;
	}
	else
	{
		val *= 10.0 / playerMaxEncumbrance;
	}

	if(val < .2)
	{
		val = .2;
	}

	return val;
}
