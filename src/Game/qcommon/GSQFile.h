#ifndef __GSQFILE_H_
#define __GSQFILE_H_

// Glurk - do something about this mess - it's vile -MW.
class IGhoulObj;
typedef unsigned short int GhoulID;
class IGhoulInst;

/*
==============================================================

GHOUL FILESYSTEM STUFF

==============================================================
*/

int FindGSQFile(char *gsqdir, char *gsqfile, void **buffer);
bool ReadGsqEntry(int &filesize, char **tdata, char *seqname);
void	PrecacheGSQFile(char *dirname, char *gsq_file, IGhoulObj *object);
int  RegisterGSQSequences(char *gsqdir, char *subclass, IGhoulObj *object);
void TurnOffPartsFromGSQFile(char *dirname, char *poff_file, IGhoulObj *this_object, IGhoulInst *this_inst);



#define MAX_TEAMNAME_LENGTH			50
#define	MAX_MATERIALNAME_LENGTH		50
#define	MAX_SKINNAME_LENGTH			100
#define	MAX_PARTNAME_LENGTH			50
#define	MAX_MODELINFONAME_LENGTH	100
#define	MAX_GHBNAME_LENGTH			100
#define	MAX_GSQNAME_LENGTH			100
#define	MAX_FACESETNAME_LENGTH		50
#define	MAX_DIRECTORYNAME_LENGTH	100
#define MAX_MENUSTRING_LENGTH		200

//public player-model textfile stuff (sorry, i didn't want to make a GPMFile_public.h):
class TeamInfoC
{
public:
	char name[MAX_TEAMNAME_LENGTH];
};

//i'm not sure how this is going to be handled--a directory should be enough, but in case there's more i'm separating it out now.
class VoiceInfoC
{
public:
	char voiceDir[MAX_DIRECTORYNAME_LENGTH];//injury & death directory
};

class SkinInfoC
{
public:
	char matName[MAX_MATERIALNAME_LENGTH];
	char skinName[MAX_SKINNAME_LENGTH];
	char partName[MAX_PARTNAME_LENGTH];//this may have to be taken out because it could blow texture memory through the ceiling, but it's just too cool for me to leave it off to begin with. i'll demonstrate shortly.
};

//some of the player types require twiddling with a part or two beyond what the typical guy of the same general type would have on/off (e.g. mohawks)
class ExtraPartToggleInfoC
{
public:
	char partName[MAX_PARTNAME_LENGTH];
	bool partState;//true=on;false=off
};

class BoltOnInfoC
{
public:
	float scale;
	char parentBolt[MAX_PARTNAME_LENGTH];//bolts are parts
	char childBolt[MAX_PARTNAME_LENGTH];//bolts are parts
	char childModelInfo[MAX_MODELINFONAME_LENGTH];//this should be the name of a modelinfo file. maybe .gbm instead of .gpm?
};

//Don't ever new one of these!  This is just the interface class to let you call the functions that are available in the class that's really returned by NewPlayerModelInfo.
class IPlayerModelInfoC
{
public:
	//will load the player description textfile, and set all info according to that.  called by constructor, but should be able to recall for a new model.
	virtual	void	Init(char *newFileName){}

	// Returns and sets the object associated with this model
	virtual IGhoulObj	*GetObject(void) const { return(NULL); }
	virtual void		SetObject(IGhoulObj *obj) { }

	//the number of skins that need to be set for this guy
	virtual	int		GetNumSkins(void){return 0;}
	//the number of bolt-ons available for this guy
	virtual	int		GetNumBoltOns(void){return 0;}
	//the number of extra twiddles to part on/off state that need to be done for this guy
	virtual int		GetNumPartToggles(void){return 0;}

	//the ghb's and the gsq's should be in this directory--this is mostly to be game_ghoul-friendly
	virtual	char	*GetGHBDir(void){return "";}
	//this is the ghb the menu can use (if we can get the menu using the game ghb, this is meaningless)
	virtual	char	*GetMenuGHB(void){return "";}
	//this is the ghb the game needs to load (the menu might be able to use it too, but we'll not hold our breath on that)
	virtual	char	*GetGameGHB(void){return "";}

	//this is the gsq file listing the sequences to register--use this to get the _poff .gsq too(game ghb only)
	virtual	char	*GetSeqGSQ(void){return "";}
	//this is the face base, to be used by the body for changing expressions
	virtual	char	*GetFaceSet(void){return "";}

	//these guys return true if they filled the class passed in, false if they failed for some reason
	virtual	bool	GetTheMenuString(char *putstringhere){return false;}
	virtual	bool	GetTeamInfo(TeamInfoC &putteamhere){return false;}
	virtual	bool	GetVoiceInfo(VoiceInfoC &putvoiceinfohere){return false;}
	virtual	bool	GetSkin(int skinnum, SkinInfoC &putskinhere){return false;}
	virtual	bool	GetBoltOn(int boltonnum, BoltOnInfoC &putboltonhere){return false;}
	virtual	bool	GetPartToggle(int togglenum, ExtraPartToggleInfoC &puttogglehere){return false;}

	virtual	bool	IsLoaded(void){return false;}

	virtual	~IPlayerModelInfoC(){}
};


	//if you pass in a NULL pointer to this, it won't set anything up (no file-loading, nothing)
	//so if you want to create one of these guys but don't know the file yet, pass in NULL, don't pass in trash

//urh! you need to delete this when you're done with it!
IPlayerModelInfoC *NewPlayerModelInfo(char *modelname);

#endif // __GSQFILE_H_