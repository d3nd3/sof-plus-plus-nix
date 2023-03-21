#if !defined(IGHOUL_INC)
#define IGHOUL_INC

#include "matrix4.h"
#include "packet.h"
typedef unsigned short int GhoulID;

const GhoulID NULL_GhoulID(0);
const GhoulID MAX_GhoulID(0xffff);

struct RenderInfo
{
	float Time;
	Vect3 Ambient;
	Vect3 AmbientPos;
	Vect3 LightPos;
	bool UseCamera;
	bool UseAmbientPos;
	bool UseMapLights;
	bool UseSpecular;
	bool UseFastLights;
	bool UseGlLighting;
	bool UseDirectional;
	bool UseAttenuation;
	bool ForceShadow;
	bool ShadowOnly;
	bool NoTextureStack;
	float ShadowDark;
	float ShadowTilt;
	float ShadowDist;
	float (*ShadowTrace)(float *start,float *end);
	int MaxShadows;
	int MaxMapLights;
	Matrix4 ToWorld;
	Vect3 WorldViewPoint;
	Vect3 WorldViewDir;
	bool UseNoLights;
	virtual bool CullBoundBox(const float *box) const {return false;}
	RenderInfo() {UseNoLights=false;UseGlLighting=false;ShadowTrace=0;MaxShadows=0;}
};

struct QuakeLight
{
	Vect3 Origin;
	float Radius;
	Vect3 Color;
};

typedef short int GhoulUUID;

class IGhoulCallBack;
class IGhoulInst;


struct HitRecord
{
	IGhoulInst *Inst;
	GhoulID Mesh;
	float Distance;
	Vect3 Normal;
	Vect3 ContactPoint;
};

enum TexChannel
{
	Diffuse=0,
	Specular
};

enum PartType
{
	ptDummy,
	ptMesh,
	ptCamera,
	ptLight
};

enum GhoulSpeed
{
	gs4Over8=0,
	gs5Over8,
	gs6Over8,
	gs7Over8,
	gsOne,
	gs9Over8,
	gs10Over8,
	gs11Over8,
	gs12Over8,
	gs13Over8,
	gs14Over8,
	gs15Over8,
	gs16Over8,
	gsEND
};

class IGhoul;

class IGhoulObj
{
public:
	virtual GhoulID RegisterSequence(const char *Filename,bool Subsequences=false)=0;
	virtual void RegisterEverything(bool Skins=true)=0;
	virtual void PreCache(bool Specular)=0;
	virtual GhoulID FindSequence(const char *Filename)=0;
	virtual GhoulID NumSequences() const=0;
	virtual void GetSequenceName(GhoulID who,char *dest) const=0;
	virtual GhoulID RegisterPart(const char *PartName)=0;
	virtual GhoulID FindPart(const char *PartName)=0;
	virtual GhoulID NumParts() const=0;
	virtual PartType GetPartType(GhoulID who) const=0;
	virtual void GetPartName(GhoulID who,char *dest) const=0;
	virtual GhoulID RegisterNoteToken(const char *Token)=0;
	virtual GhoulID FindNoteToken(const char *Token)=0;
	virtual GhoulID NumNoteTokens() const=0;
	virtual void FireAllNoteCallBacks(IGhoulCallBack *c,GhoulID Token=0)=0;
	virtual GhoulID RegisterMaterial(const char *Mat)=0;
	virtual GhoulID FindMaterial(const char *Mat)=0;
	virtual GhoulID NumMaterials() const=0;
	virtual void GetMaterialName(GhoulID who,char *dest) const=0;
	virtual GhoulID RegisterSkin(GhoulID Mat,const char *Skin,TexChannel Channel=Diffuse)=0;
	virtual GhoulID FindSkin(GhoulID Mat,const char *Skin,TexChannel Channel=Diffuse)=0;
	virtual GhoulID NumSkins() const=0;
	virtual void GetSkinName(GhoulID who,char *dest) const=0;
	virtual GhoulID GetSkinMaterial(GhoulID who) const=0;
	virtual IGhoulInst *NewInst()=0;
	virtual GhoulUUID MyUUID() const=0;
	virtual IGhoul *GetMyGhoul() const=0;
	virtual void Destroy()=0;
};

class IGhoulCallBack
{
public:
	virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data)=0; 
};

class IGhoulInst
{
public:
	enum EndCondition
	{
		Loop,
		Hold, // hold at end of seq
		BackAndForth,
		HoldFrame, // hold at PlayPos
		EndMarker //do not use
	};
	enum MatrixType
	{
		JacobianLocal,
		JacobianEntity,
		JacobianLocalInv,
		JacobianEntityInv,
		Local,
		Entity,
		LocalInv,
		EntityInv
	};
	virtual void SetXForm(const float *m)=0;
	virtual void GetXForm(float *m)=0;
	virtual void SetXForm(const Matrix4 &m)=0;
	virtual void GetXForm(Matrix4 &m)=0;
	virtual void SetOnOff(bool OnOff,float Now)=0;
	virtual bool GetOnOff()=0;
	virtual void SetSpeed(GhoulSpeed speed)=0;
	virtual GhoulSpeed GetSpeed()=0;
	virtual void SetPartOnOff(GhoulID Part,bool OnOff)=0;
	virtual bool GetPartOnOff(GhoulID Part)=0;
	virtual void SetAllPartsOnOff(bool OnOff)=0;

	virtual void SetFrameOverride(GhoulID Mat,GhoulID Skin,GhoulID Part=0)=0;
	virtual GhoulID GetFrameOverride(GhoulID Mat,GhoulID Part=0,TexChannel Channel=Diffuse)=0;
	virtual void ClearFrameOverride(GhoulID Mat,GhoulID Part=0,TexChannel Channel=Diffuse)=0;
	virtual void SetTint(float r,float g,float b,float a)=0;
	virtual void GetTint(float *r,float *g,float *b,float *a)=0;
	virtual void SetTintOnAll(float r,float g,float b,float a)=0;

	virtual void Play(GhoulID Seq,float Now,float PlayPos=0.0f,bool Restart=true,EndCondition ec=Loop, bool MatchCurrentPos=false, bool reverseAnim=false)=0;
	virtual GhoulID GetPlayingSequence()=0;
	virtual void Pause(float Now)=0;
	virtual void Resume(float Now)=0;

	virtual void SetUserData(void *user)=0;
	virtual void *GetUserData()=0;
	virtual void AddNoteCallBack(IGhoulCallBack *c,GhoulID Token=0)=0;
	virtual void RemoveNoteCallBack(IGhoulCallBack *c,GhoulID Token=0)=0;

	virtual void AddMatrixCallBack(IGhoulCallBack *c,GhoulID Part=0,MatrixType kind=JacobianLocal)=0;
	virtual void RemoveMatrixCallBack(IGhoulCallBack *c,GhoulID Part=0,MatrixType kind=JacobianLocal)=0;

	virtual float GetSequenceLength(GhoulID Seq,EndCondition ec=Loop,float *SecsPerFrame=0)=0; 

	virtual void Bolt(GhoulID Bolt,IGhoulInst *Bolted,GhoulID BoltedBolt=0)=0;
	virtual void SetMyBolt(GhoulID Bolt=0)=0;
	virtual GhoulID GetMyBolt() const=0;
	virtual void UnBolt(IGhoulInst *Bolted)=0;

	virtual int GetNumChildren() const=0;
	virtual IGhoulInst *GetChild(int Child,GhoulID &bolt) const=0;
	virtual IGhoulObj *GetGhoulObject() const=0;
	virtual IGhoulInst *GetParent() const=0;
	virtual IGhoulInst *Clone(bool CloneChildren=true) const=0;

	virtual int SaveState(unsigned char *buffer,int size,IGhoulCallBack **CallBacks,int NumCallBacks, const char *name) const=0;
	virtual GhoulUUID RestoreState(unsigned char *buffer,int size,IGhoulCallBack **CallBacks,int NumCallBacks, const char *name)=0;

	virtual GhoulUUID MyUUID() const =0;
	virtual GhoulUUID MyObjUUID() const =0;

	virtual void Render(RenderInfo &r)=0;
	virtual void PreCache(bool Specular,bool Textures,bool Verts)=0;
	virtual int RayTrace(float Time,const Vect3 &start,const Vect3 &dir,HitRecord *Hits,int MaxHits)=0;
	virtual int BoxTrace(float Time,const Matrix4 &ToWorld,const Vect3 &start,const Vect3 &end,const Vect3 &mins,const Vect3 &maxs,HitRecord *Hits,int MaxHits)=0;

	virtual void ServerUpdate(float Time)=0;
	virtual void TurnCallBacksOff()=0;
	virtual void TurnMatrixCallBacksOff()=0;
	virtual void TurnCallBacksOn(float Now)=0;
	virtual void TurnMatrixCallBacksOn(float Now)=0;
	virtual void GetBoltMatrix(float Time,Matrix4 &m,GhoulID Part=0,MatrixType kind=JacobianLocal,bool ToRoot=true)=0;
	virtual void GetBoundBox(float Time,const Matrix4 &ToWorld,Vect3 &mins,Vect3 &maxs,GhoulID Part=0,bool Accurate=false)=0;
	virtual int QueueForClient(float dist)=0;
	virtual void Destroy()=0;

	virtual GhoulID GetStateSequence()=0;
// string based interface

	bool IsLive() {return (GetPlayingSequence()!=0)&&GetOnOff();}

	bool SetPartOnOff(const char *Part,bool OnOff)
	{
		GhoulID t=GetGhoulObject()->FindPart(Part);
		if (!t)
			return false;
		SetPartOnOff(t,OnOff);
		return true;
	}
	bool GetPartOnOff(const char *Part)
	{
		GhoulID t=GetGhoulObject()->FindPart(Part);
		if (!t)
			return false;
		return GetPartOnOff(t);
	}
	bool SetFrameOverride(const char *Mat,const char *Skin,const char *Part)
	{
		GhoulID t=GetGhoulObject()->FindPart(Part);
		GhoulID m=GetGhoulObject()->FindMaterial(Mat);
		if (!m)
			return false;
		GhoulID s=GetGhoulObject()->FindSkin(m,Skin);
		if (!s)
			return false;
		SetFrameOverride(m,s,t);
		return true;
	}
	bool SetFrameOverride(const char *Mat,const char *Skin,GhoulID Part=0)
	{
		GhoulID m=GetGhoulObject()->FindMaterial(Mat);
		if (!m)
			return false;
		GhoulID s=GetGhoulObject()->FindSkin(m,Skin);
		if (!s)
			return false;
		SetFrameOverride(m,s,Part);
		return true;
	}
	bool SetFrameOverride(GhoulID Mat,const char *Skin,GhoulID Part=0)
	{
		if (!Mat)
			return false;
		GhoulID s=GetGhoulObject()->FindSkin(Mat,Skin);
		if (!s)
			return false;
		SetFrameOverride(Mat,s,Part);
		return true;
	}
	bool SetFrameOverride(const char *Mat,GhoulID Skin,GhoulID Part=0)
	{
		GhoulID m=GetGhoulObject()->FindMaterial(Mat);
		if (!m)
			return false;
		if (!Skin)
			return false;
		SetFrameOverride(m,Skin,Part);
		return true;
	}
	GhoulID GetFrameOverride(const char *Mat,const char *Part,TexChannel Channel=Diffuse)
	{
		GhoulID m=GetGhoulObject()->FindMaterial(Mat);
		GhoulID p=GetGhoulObject()->FindPart(Part);
		if (!m)
			return 0;
		return GetFrameOverride(m,p,Channel);
	}
	GhoulID GetFrameOverride(const char *Mat,GhoulID Part=0,TexChannel Channel=Diffuse)
	{
		GhoulID m=GetGhoulObject()->FindMaterial(Mat);
		if (!m)
			return 0;
		return GetFrameOverride(m,Part,Channel);
	}
	bool ClearFrameOverride(const char *Mat,const char *Part,TexChannel Channel)
	{
		GhoulID m=GetGhoulObject()->FindMaterial(Mat);
		GhoulID p=GetGhoulObject()->FindPart(Part);
		if (!m)
			return false;
		ClearFrameOverride(m,p,Channel);
		return true;
	}
	bool Play(const char *Seq,float Now,float PlayPos,bool Restart,EndCondition ec, bool MatchCurrentPos, bool reverseAnim)
	{
		GhoulID s=GetGhoulObject()->FindSequence(Seq);
		if (!s)
			return false;
		Play(s,Now,PlayPos,Restart,ec,MatchCurrentPos,reverseAnim);
		return true;
	}
	bool AddNoteCallBack(IGhoulCallBack *c,const char *Token)
	{
		GhoulID t=GetGhoulObject()->FindNoteToken(Token);
		AddNoteCallBack(c,t);
		return true;
	}
	bool RemoveNoteCallBack(IGhoulCallBack *c,const char *Token)
	{
		GhoulID t=GetGhoulObject()->FindNoteToken(Token);
		RemoveNoteCallBack(c,t);
		return true;
	}
	bool AddMatrixCallBack(IGhoulCallBack *c,const char *Part,MatrixType kind)
	{
		GhoulID t=GetGhoulObject()->FindPart(Part);
		AddMatrixCallBack(c,t,kind);
		return true;
	}
	bool RemoveMatrixCallBack(IGhoulCallBack *c,const char *Part,MatrixType kind)
	{
		GhoulID t=GetGhoulObject()->FindPart(Part);
		RemoveMatrixCallBack(c,t,kind);
		return true;
	}
	float GetSequenceLength(const char *Seq,EndCondition ec)
	{
		GhoulID s=GetGhoulObject()->FindSequence(Seq);
		if (!s)
			return 0.0f;
		return GetSequenceLength(s,ec);
	}
	bool Bolt(const char *bolt, IGhoulInst *Bolted, const char *BoltedBolt)
	{
		GhoulID b = GetGhoulObject()->FindPart(bolt);
		GhoulID bb = Bolted->GetGhoulObject()->FindPart(BoltedBolt);
		Bolt(b, Bolted, bb);
		return true;
	}
	bool SetMyBolt(const char *Bolt)
	{
		GhoulID b=GetGhoulObject()->FindPart(Bolt);
		SetMyBolt(b);
		return true;
	}
	bool GetBoltMatrix(float Time,Matrix4 &m,const char *Part,MatrixType kind,bool ToRoot)
	{
		GhoulID t=GetGhoulObject()->FindPart(Part);
		GetBoltMatrix(Time,m,t,kind,ToRoot);
		return true;
	}
	bool GetBoundBox(float Time,const Matrix4 &ToWorld,Vect3 &mins,Vect3 &maxs,const char *Part)
	{
		GhoulID t=GetGhoulObject()->FindPart(Part);
		GetBoundBox(Time,ToWorld,mins,maxs,t);
		return true;
	}
};


class IGhoul
{
public:
	virtual IGhoulObj *NewObj()=0;
	virtual void GlPrep(void *gl=0)=0; //gl is assumed to be an GhoulOpenGL *
	virtual void PreCache(bool Specular)=0;
	virtual void GlUnprep()=0;
	virtual void BeginServerFrame()=0;
	virtual void EndServerFrame()=0;
	virtual void DestroyAllObjects()=0;
	virtual void FlushUnusedFiles()=0;
	virtual void Destroy()=0;

	virtual IGhoulInst *FindClientInst(GhoulUUID key)=0;
	virtual IGhoulObj *FindClientObj(GhoulUUID key)=0;

	virtual void SetFilenameMapper(void (*Map)(char *dest,const char *src))=0;

	//server
	virtual void AddClient(int client)=0;
	virtual void RemoveClient(int client)=0;
	virtual void FlushClients()=0;
	virtual void RemoveAllClients()=0;
	virtual bool PackReliable(int client,int packetID,OutPacket &op)=0;
	virtual bool NeedReliable(int client)=0;
	virtual bool Pack(int client,int packetID,OutPacket &op,float baseTime)=0;
	virtual void AckPack(int client,int packetID)=0;
	virtual void ReliableHitWire(int client,int packetID)=0;
	virtual void GhoulClearClientQueue(int client,int packetID)=0;
	virtual void AddFileForDownLoad(const char *name)=0;

	//client
	virtual void Precache()=0;
	virtual void UnPackReliable(int packetID,InPacket &ip,bool specular)=0;
	virtual void UnPack(int packetID,InPacket &ip,float now)=0;
	virtual int GetSavedReliableSize() const=0;
	virtual const unsigned char *GetSavedReliable()=0;

	virtual int GetNumExportLights()=0;
	virtual void GetExportLight(int i,QuakeLight &ql)=0;
	virtual void BeginRenderFrame()=0;
	virtual void AddImportLight(const QuakeLight &ql)=0;
	virtual void EndRenderFrame()=0;
	virtual void FlushMeshCache()=0;

	virtual void AddMapLight(const QuakeLight &ql)=0;
	virtual void FinishMapLights()=0;
	virtual void ClearMapLights()=0;

	virtual void SetGammaTable(unsigned char * table)=0;
	virtual void SetMip(int mip)=0;
	virtual void SetPolyOffset(float factor,float unit)=0;

	virtual int GetNumTexturesBound()=0;
	virtual int GetNumTexturesBoundBytes()=0;
	virtual int GetNumMeshesRendered()=0;
	virtual int GetNumTrianglesRendered()=0;
	virtual int GetNumTrianglesRenderedSpec()=0;
	virtual int GetNumVertsRendered()=0;
	virtual int GetNumCornersRendered()=0;
	virtual int GetNumLightRaysRendered()=0;
	virtual void ClearStats()=0;
};


IGhoul *GetGhoul(bool Client=false,bool Menu=false);


#endif