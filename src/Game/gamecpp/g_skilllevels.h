#ifndef __G_SKILLLEVELS_H
#define __G_SKILLLEVELS_H

#define NUM_SKILL_LEVELS 5

class CPlayerSkill
{
private:
	float	aiDodgeFrequency;
	float	aiAccuracy;
	float	aiAimMaxDist;
	float	aiHesitation;
	float	aiWanderAmount;
	float	aiTurnSpeed;

	float	playerDamage;
	int		playerMaxEncumbrance;
	float	ammoModifier;
	float	armorModifier;
	int		cheatsAvailable;
	int		fullHealth;

	float	spawnFrequency;
	float	spawnLullTime;
	float	spawnNonLullTime;
	float	spawnForceTime;
	float	spawnMaxGuys;
	float	spawnMinTime;

	int		numberOfSaves;

	int		spawnValue;
	int		enemyValue;

	float	moneyMult;

public:
	//yuck
	CPlayerSkill(void){spawnValue = -1; enemyValue = -1; moneyMult = -1;};//ew!
	CPlayerSkill(float newaiDodgeFrequency, float newaiAccuracy, float newaiAimMaxDist, float newaiHesitation,
		float newaiWanderAmount, float newplayerDamage,  int newplayerMaxEncumbrance, float newammoModifier,
		float newarmorModifier,	float newspawnFrequency, float newspawnLullTime, float newspawnNonLullTime,
		float newspawnForceTime, float newspawnMaxGuys, int newcanCheat, float newspawnMinTime, int newNumberOfSaves, 
		float newaiTurnSpeed, int newfullHealth);

	float	getDodge(void){return aiDodgeFrequency;}
	float	getAccuracy(void){return aiAccuracy;}
	float	getAimMaxDist(void){return aiAimMaxDist;}
	float	getHesitation(void){return aiHesitation;}
	float	getWanderAmount(void){return aiWanderAmount;}
	float	getTurnSpeed(void){return aiTurnSpeed;}

	float	getPlayerDamageMod(void){return playerDamage;}
	int		getPlayerMaxEncumbrance(void){return playerMaxEncumbrance;}
	float	getAmmoMod(void){return ammoModifier;}
	float	getArmorMod(void){return armorModifier;}
	int		canCheat(void){return cheatsAvailable;}
	int		fullHealthEachLevel(void){return fullHealth;}
	
	float	getSpawnFreq(void){return spawnFrequency;}
	float	getSpawnLullTime(void){return spawnLullTime;}
	float	getSpawnNonLullTime(void){return spawnNonLullTime;}
	float	getSpawnForceTime(void){return spawnForceTime;}
	float	getSpawnMaxGuys(void){return spawnMaxGuys;}
	float	getSpawnMinTime(void){return spawnMinTime;}

	int		getNumberOfSaves(void){return numberOfSaves;}

	void	buildCustomSkillLevels(void);

	float	getMoneyMultiplier(void);

	float	getMoney(void){return moneyMult;}
	void	setMoneyMult(void){moneyMult = getMoneyMultiplier(); }

	float	getInited(void){return (enemyValue != -1);}

	int		getSpawnValue(void){return spawnValue;}
	int		getEnemyValue(void){return enemyValue;}
};

extern CPlayerSkill skillStats[NUM_SKILL_LEVELS];

#endif