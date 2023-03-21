#ifndef __AI_PATH_PRE_H
#define __AI_PATH_PRE_H

#include "q_shared.h"
#include "g_local.h"
#include "fields.h"
#include "ai.h"

#define AI_NODE_VERSION			8
#define AI_NODE_VERSION_CUR		11
#define AI_EXPORT_VERSION		40

#define MAX_LINK_DIST			512
#define MAX_COMBATPOINT_DIST	384
#define MIN_NODE_DIST			96
#define NODE_COLINEAR			.97			// this should be 14 degrees or so...

#define MAX_PATH_SLOPE			.5			// sin of 30 degrees

#define COS_25					.9063
#define SIN_25					.4226

#define COS_45					.70711

#define NUM_NEAR_NODES			12

#define NODEDISTSCALE			25.0

#define EPS_NORMAL				0x00000001
#define EPS_FLEE				0x00000004
#define EPS_FLEEFRIENDLY		0x00008000
#define EPS_FLEEENEMY			0x00010000
#define EPS_ANYPOINT			0xFFFFFFFF

#define EPS_LINKMAIN (EPS_NORMAL|EPS_FLEE|EPS_FLEEFRIENDLY|EPS_FLEEENEMY)

#define EPS_FLEEGROUP (EPS_FLEE|EPS_FLEEENEMY|EPS_FLEEFRIENDLY)

// if a floor is only partial, record if any of it's edges are clear if it's adjacent to another partial
#define PARTIAL_FLOOR		0x1
#define PARTIAL_YMINUS		0x2
#define PARTIAL_XPLUS		0x4
#define PARTIAL_YPLUS		0x8
#define PARTIAL_XMINUS		0x10
#define PARTIAL_FLOORGAP	0x20	//these are very very very very bad


class CAIRegion
{
private:
	int				id;
	vec3_t			min;
	vec3_t			max;
	int				targNode;
	int				blockSide;

public:
	CAIRegion(void){targNode = -1; blockSide = 0;}
	void	save(FILE *fp);
	void	load(FILE **fp);
	void	load(unsigned char **data);
	void	saveExport(FILE *fp);
};

class CAIFloorSquare
{
private:
	vec3_t			min;

	float			topZ;//this being the only possible changing thing
	short			id;
	short			yMinus;
	short			xPlus;
	short			yPlus;
	short			xMinus;
	byte			partial;
	short			nearNodes[NUM_NEAR_NODES];
	float			nearNodeDist[NUM_NEAR_NODES];
public:
	CAIFloorSquare(void)
		{id = -1; xPlus = -1; xMinus = -1; yPlus = -1; yMinus = -1; topZ = 0;}
	float	*getMin(void){return min;}
	void	setPartial(int val){partial = val;}
	int		getPartial(void){return partial;}
	void	save(FILE *fp);
	void	load(FILE **fp);
	void	saveExport(FILE **fp);

	void	setId(short newVal){id = newVal;}
	short	getId(void){return id;}
	void	setYMinus(short newVal){yMinus = newVal;}
	short	getYMinus(void){return yMinus;}
	void	setXPlus(short newVal){xPlus = newVal;}
	short	getXPlus(void){return xPlus;}
	void	setYPlus(short newVal){yPlus = newVal;}
	short	getYPlus(void){return yPlus;}
	void	setXMinus(short newVal){xMinus = newVal;}
	short	getXMinus(void){return xMinus;}

	void	setTopZ(float newVal){topZ = newVal;}
	float	getTopZ(void){return topZ;}

	void	clearNear(void);
	void	setNearNode(int i, int val){nearNodes[i] = val;}
	int		getNearNode(int i){return nearNodes[i];}
	void	setNearDist(int i, float dist){nearNodeDist[i] = dist;}
	float	getNearDist(int i){return nearNodeDist[i];}
	int		setFirstFreeNode(int val);
	int		hasNodes(void);

	short	getNode(void){return nearNodes[0];}
	void	setNode(short val){nearNodes[0] = val;}

	void	setPartialFlag(int val){partial |= val;}
	int		getPartialFlag(int val){return partial & val;}

};

class CAIFloorDataEdit
{
private:
	int				regionsInUse;
	CAIFloorSquare	floorSpaces[16000];//erm
public:
	CAIFloorDataEdit(void){regionsInUse = 0;}
	void			ensureRegion(vec3_t spot);
	void			save(FILE *fp);
	void			load(FILE **fp);
	void			clear(void){regionsInUse = 0;}
	void			saveExport(FILE **fp);
	void			prepareForExport(void);
	void			AddNodesViaLine(vec3_t start, vec3_t end, int id1, int id2);
	int				getFloorForSpot(vec3_t spot);
	int				clearLineForFloor(int startID, int endID, int traceStyle);
};



class CAIConnect
{
protected:
	int				linkId;
	float			dist;
	int				first;
	int				valid;
	connectType		type;

public:
	CAIConnect(int newId, float curDist, int initNode, connectType ctype = LINK_NORMAL){linkId = newId; dist = curDist; first = initNode; valid = 1; type = ctype;}
	CAIConnect(void){linkId = 0; dist = 0; first = 0;valid = 1; type = LINK_NORMAL;}
	void			setId(int newVal){linkId = newVal;}
	int				getLinkId(void){return linkId;}
	void			setDist(float val){dist = val;}
	float			getDist(void){return dist;}
	int				getFirst(void){return first;} 
	void			setFirst(int val){first = val;}
	int				getValid(void){return valid;}
	void			setValid(int newVal){valid = newVal;} 
	connectType		getType(void){return type;}
	void			save(FILE **fp);
};

class CAINode
{
protected:
	list<CAIConnect>	links;//ids of everything connected to this...
	list<CAIConnect>	combatPoints;

	vec3_t				pos;
	int					id;
	byte				*paths;
	short				*pathRanks;//lower is better
	int					checked;
	float				radius;
	int					pointType;
	vec3_t				dest;
	edict_t				*myMarker;
	int					flee1;
	int					flee2;
	int					ignoreForSpawning;
	float				minHeight;
	float				maxHeight;

	list<int>			connectedNodes;
	vec3_t				min;
	vec3_t				max;

	int					myFloor;
public:
	CAINode(vec3_t spot, int newId)	{set(spot, newId); paths = 0; radius = 0; pointType = EPS_NORMAL;
					VectorClear(dest);ignoreForSpawning = 0; minHeight = 0; maxHeight = 0; pathRanks = 0;}
	CAINode(void){id = 0; VectorClear(pos); paths = 0; VectorClear(dest); pointType = EPS_NORMAL;
					ignoreForSpawning = 0; minHeight = 0; maxHeight = 0; pathRanks = 0;}

	void			addLink(int newLink, int isCombatPoint = 0, connectType cType = LINK_NORMAL);
	void			save(FILE *fp);
	void			load(FILE **fp);
	void			savePaths(FILE **fp);
	void			setOrg(vec3_t newOrg){VectorCopy(newOrg, pos);}
	void			drawConnections(void);
	float			*getPos(void){return pos;}
	void			setChecked(int val){checked = val;}
	int				getChecked(void){return checked;}
	void			initPathData(int num);
	void			calcPath(void);
	int				getId(void){return id;}
	void			set(vec3_t spot, int newId){VectorCopy(spot, pos); id = newId;}
	int				isConnectedTo(int linkId);
	void			destroyLink(int deadId);
	void			setId(int newId){id = newId;}
	void			calcRadius(void);
	void			setPointType(int newVal){pointType = newVal;}
	int				getPointType(void){return pointType;}
	int				getPathToPoint(int p);
	void			clearLinks(void);
	float			*getDest(void){if(pointType == EPS_NORMAL)return 0; return dest;}
	void			setPos(vec3_t newPos){VectorCopy(newPos, pos);}
	void			setMarker(edict_t *ed){myMarker = ed;}
	edict_t			*getMarker(void){return myMarker;}
	float			getPathDist(int destNode);
	void			removeBadLink(int deadId);
	void			removeBadCombatPoint(int deadId);
	void			destroyLinks(void);
	CAIConnect		*getLinkNum(int num);
	int				toggleIgnore(void){return (ignoreForSpawning = !ignoreForSpawning);}

	int				getNumLinks(void){return links.size();}
	int				getNumCombatPoints(void){return combatPoints.size();}

	void			display(void);
	void			setMyFloor(int val){myFloor = val;}
	int				getMyFloor(void){return myFloor;}

	int				getIgnore(void){return ignoreForSpawning;}
};

class CAIListData
{
protected:
	vector<CAINode>		nodes;
	CAIFloorDataEdit	floorData;

	vector<CAINode>		combatPoints;//obsolete
	vector<CAIRegion>	regions;//obsolete
public:
	CAIListData(void){}
	CAINode			*getNode(int num);
	CAINode			*getCombatNode(int num);
	void			addPoint(edict_t *ent, char *cmd, int type, float *altCenter = 0);
	void			save(void);
	void			connect(CAINode &one, CAINode &two, connectType newType);
	void			load(void);
	void			exportPaths(void);
	void			clearChecks(void);
	int				getNumElems(void){return nodes.size();}
	void			eliminateRedundantConnections(void);
	void			buildPathConnections(void);
	void			saveExport(void);
	void			add(edict_t *ent);
	int				usesCmd(edict_t *ent, char	*cmd);
	void			connectNode(CAINode &start, int combat = 0);
	void			validateNodeLinks(void);
	int				getNearestNode(vec3_t spot, float *distVal = 0);
	int				getNearestCombatPoint(vec3_t spot, float *distVal = 0);
	void			deleteNode(vec3_t spot);
	void			toggleIgnoreNode(vec3_t spot);
	void			movePoint(vec3_t spot);
	void			graphicData(float val);
	void			numberLists(void);
	void			checkEnvironments(void);
	void			ensureRegion(edict_t *ent);
	void			clearNodesFromWall(void);
};

extern CAIListData *aiList;






#endif //__ENEMY_PRE_H