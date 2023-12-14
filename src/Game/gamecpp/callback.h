// ------------------------------------------------------------------------
//
// All callback definitions that need to be enumerated for the savegame
// must be in here
//
// ------------------------------------------------------------------------

#pragma once

// Helicopter callbacks

class MainRotor2Callback :public IGhoulCallBack
{
public:
	bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};

class MainHeliCallback2 :public IGhoulCallBack
{
public:
	bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};

// Tank callbacks
class TankTreadCallback :public IGhoulCallBack
{
public:
	bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};

// Snowcat callbacks
class SnowcatTreadCallback :public IGhoulCallBack
{
public:
	bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};


// pickup callbacks
class PickupRespawnCallback :public IGhoulCallBack
{
public:
	bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};



// ???

class TestSeqEndCallback :public IGhoulCallBack
{
public:
	bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};

// In gamecpp object callbacks

class SideOBeefCallback :public IGhoulCallBack
{
public:
	bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};

class valveCallback :public IGhoulCallBack
{
public:
	bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};

class GuttedCowCallback :public IGhoulCallBack
{
public:
	bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};

class BannerCallback :public IGhoulCallBack
{
public:
	bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};

class HanginLightCallback :public IGhoulCallBack
{
public:
	bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};

// ummm...  ok, so this probably is a crappy place to put this.  My apologies, Steve...
class gutCallback :public IGhoulCallBack
{
public:
	bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};

// Body callbacks

class FollowGenericPivot :public IGhoulCallBack
{
public:
	virtual bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};

class FootRightCallback :public IGhoulCallBack
{
private:
	float last_vert_speed;
public:
	virtual bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};

class FootLeftCallback :public IGhoulCallBack
{
private:
	float last_vert_speed;
public:
	virtual bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};

class GroundCallback :public IGhoulCallBack
{
private:
	float last_vert_speed;
public:
	virtual bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};

class MouthCallback :public IGhoulCallBack
{
private:
	float last_vert_speed;
public:
	virtual bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};


//sets pos1 to the position of the bolt, rotated correctly to the world, but still assuming ent origin to be 0,0,0 (will deal with that later, when pos1 is used)
class SetVectorCallback :public IGhoulCallBack
{
private:
	float last_vert_speed;
public:
	virtual bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};

class MonsterSeqEndCallback :public IGhoulCallBack
{
public:
	virtual bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};

class MonsterSeqBeginCallback :public IGhoulCallBack
{
public:
	virtual bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};

class InAirCallBack:public IGhoulCallBack
{
public:
	virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data);
};

class JumpCallBack:public IGhoulCallBack
{
public:
	virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data);
};

class ThudCallBack:public IGhoulCallBack
{
public:
	virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data);
};

class KneeCallBack:public IGhoulCallBack
{
public:
	virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data);
};

class FireCallBack:public IGhoulCallBack
{
public:
	virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data);
};

class MonsterSoundCallBack:public IGhoulCallBack
{
public:
	virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data);
};

class ThrowCallBack:public IGhoulCallBack
{
public:
	virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data);
};

class DropWeaponCallBack:public IGhoulCallBack
{
public:
	virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data);
};

class LimbEOSCallback:public IGhoulCallBack
{
public:
	virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data);
};
// end