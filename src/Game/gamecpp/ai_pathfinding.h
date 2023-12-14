#ifndef __AI_PATH_H
#define __AI_PATH_H

#include "ai_path_pre.h"

enum
{
	FLOOR_YMINUS,
	FLOOR_XPLUS,
	FLOOR_YPLUS,
	FLOOR_XMINUS,
};

enum
{
	FLOORSLOT_EMPTY = 0,
	FLOORSLOT_SINGLE,
	FLOORSLOT_DOUBLE
};

class CAIFloorSystem;

class CAIFloor
{
private:
	vec3_t			min;
	int				partial;
	int				id;
	short			nearNodes[NUM_NEAR_NODES];
	float			topZ;
	short			adjFloors[4];
public:
	int				getID(void){return id;}
	int				getNode(void){return nearNodes[0];}
	int				getPartial(void){return partial;}
	void			load(byte **inData);
	float			getZValue(void){return min[2];}
	float			*getMin(void){return min;}
	short			getAdj(int side){return adjFloors[side];}
	short			getNearNode(int val){return nearNodes[val];}

	void			setTopZ(float newVal){topZ = newVal;}
	float			getTopZ(void){return topZ;}

	int				getBestClearFloor(int recurse, CAIFloorSystem &system, int sideFrom);
};

class CAIFloorYRow
{
private:
	int				numFloors;
	CAIFloor		*floors;
	float			yValue;
public:
	CAIFloorYRow(void){numFloors = 0; floors = 0; yValue = 0;}
	void			clear(void){delete[] floors; floors = 0; numFloors = 0;}
	CAIFloor		*getFloorForPosition(vec3_t position);
	float			getYValue(void){return yValue;}
	void			load(byte **inData);
};

class CAIFloorXRow
{
private:
	int				numYRows;
	CAIFloorYRow	*yRows;
	float			xValue;
public:
	CAIFloorXRow(void){numYRows = 0; yRows = 0; xValue = 0;}
	CAIFloor		*getFloorForPosition(vec3_t position);
	float			getXValue(void){return xValue;}
	void			load(byte **inData);
	void			clear(void);
};

//quicker implementation?
#define MAX_USED_FLOORS 64

class CAIFloorSystem
{
private:
	int				numFloors;
	CAIFloor		*floors;
	int				numXRows;
	CAIFloorXRow	*xRows;

	int				floorsUsed;

	int				occupArray;
	int				*floorOccupation;

public:
	CAIFloorSystem(void){numXRows = 0; xRows = 0; numFloors = 0; floors = 0; occupArray = 0; floorOccupation = 0;}
	void			clear(void);
	CAIFloor		*getFloorForPosition(vec3_t position);
	void			load(byte **inData);
	CAIFloor		*acquireFloorSpace(int numSlots)
		{floorsUsed += numSlots; assert(floorsUsed <= numFloors); return &floors[floorsUsed - numSlots];}
	CAIFloor		*getFloorForNumber(int num){assert(num < numFloors);return &floors[num];}
	void			clearOccupiedFloors(void);
	void			addOccupiedFloor(int val);
	void			addOccupiedFloor(CAIFloor *val);
	int				floorIsOccupied(int val, int c1, int c2, int c3, int c4);
	int				getNearestFreePosition(vec3_t start, vec3_t spot);
};

class CAIPathLink
{
protected:
	int				linkId;
	float			dist;
	connectType		type;

public:
	void			load(byte **inData);
	int				getLinkId(void){return linkId;}
	connectType		getType(void){return type;}
	float			getDist(void){return dist;}
};

class CAIPathNode
{
protected:
	int				id;
	vec3_t			pos;
	int				numLinks;
	CAIPathLink		*link;
	int				numCombatLinks;
	CAIPathLink		*combatLinks;
	byte			*path;
	short			*pathRanks;
	float			radius;
	int				pointType;
	int				flee1;	// this is a friendly flee point... enemy will use only if flee2 is not set
	int				flee2;	// enemy flee point

	float			checkKey;//used for the searches

	float			lastOccupyTime;
	int				occupiedBy;

	int				ignoreForSpawning;
	float			minHeight;
	float			maxHeight;

public:
	CAIPathNode(void){link = 0; path = 0; combatLinks = 0; numLinks = 0; numCombatLinks = 0; checkKey = 0; 
					lastOccupyTime = 0; occupiedBy = 0; pathRanks = 0;}

	void			load(byte **inData, int numNodes);
	void			clean(void);
	int				getDest(int finalNode);
	int				getRank(int node){if(node == -1)return 15999; return pathRanks[node];}
	void			getPos(vec3_t output){VectorCopy(pos, output);}
	float			*getPos(){return pos;}
	float			getRadius(void){return radius;}
	int				getNumLinks(void){return numLinks;}
	int				getLink(int val){assert(val > -1); assert(val < numLinks); return link[val].getLinkId();}

	int				getNumCombatPoints(void){return numCombatLinks;}
	int				getCombatPoints(int val){assert(val > -1); assert(val < numCombatLinks); return combatLinks[val].getLinkId();}

	CAIPathLink		*getLinkWithId(int val);
	int				getType(void){return pointType;}

	void			setCheckKey(float val){checkKey = val;}
	float			getCheckKey(void){return checkKey;}

	int				getFleePoint(int id){if(id == 1)return flee1; if(id == 2)return flee2; return 0;}

	int				isValidCombatPoint(vec3_t blockDir);

	int				pathInRange(void);
	int				pathLinksInRange(int numNodes);
	int				getSpawnNode(float key, vec3_t spot, int allowBehind, vec3_t fwd);
	void			markClearedNodes(float key);
};

class CAIPaths
{
protected:
	int				numNodes;
	CAIPathNode		*node;
	int				numCombatNodes;
	CAIPathNode		*combatNode;
	int				clientNode;
	int				clientCorners[4];
	int				searchKey;

	CAIFloorSystem	floors;

	int				getClearPath(int startRegion, int endNode, int approach, nodeData_t *data, vec3_t start, edict_t *ent);
	int				clearLineForFloor(int startID, vec3_t end, nodeData_t *data, int approach);
	int				getFloorPath(int startRegion, int nextNode, nodeData_t *data, int recurse = 3, int ignoreRegion = -1, float *distFound = 0, int sourceStart = -1);
	int				clearForDirection(int startRegion, int endNode, nodeData_t *data, vec3_t endSpot);

public:
	CAIPaths(void){node = 0; numNodes = 0; combatNode = 0; numCombatNodes = 0;}
	void			load(void);
	void			clean(void);
	void			getPath(vec3_t start, vec3_t end, nodeData_t *data, ai_public_c *who = 0, edict_t *ent = 0, int approach = 1, edict_t *targ = 0);
	void			getReversePath(vec3_t start, vec3_t end, nodeData_t *data, ai_public_c *who = 0, edict_t *ent = 0, edict_t *targ = 0);
	int				isActive(void){return (numNodes > 0);}

	// returns the distance or 0 if invalid
	int				getNearestFleePoint(nodeData_t *data, vec3_t outSpot, int team);
	void			getCombatPoint(nodeData_t *data, vec3_t start, vec3_t end, vec3_t dir);

	int				getNode(vec3_t pos, int prev, int prevDest = 0);
	void			linkEnemyIntoPath(vec3_t pos, nodeData_t *data);

	CAIPathNode		*getNode(int val){return &node[val];}
	void			setupClientPathInfo(int *node, int *region);

	int				getSpawnPosition(vec3_t outSpot, int allowBehind);
	CAIFloorSystem	*getFloors(void){return &floors;}

	void			idealWanderPoint(nodeData_t *data, ai_public_c *who = 0);
	int				search(nodeData_t *data, ai_public_c *who = 0);

	int				getLookSpot(vec3_t source, vec3_t outSpot, int key);
};

extern CAIPaths aiPoints;

#endif