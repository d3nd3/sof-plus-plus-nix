#include "ai_pathfinding.h"

////////////////////////////////////////////////////////////////////////////////////////////////
//
//								Misc functions
//
////////////////////////////////////////////////////////////////////////////////////////////////

float readFloat(byte **in);
void readVect(byte **in, vec3_t v);
int readInt(byte **in);
short readShort(byte **in);
byte readByte(byte **in);
signed char readChar(byte **in);

qboolean IsUseableBrush(edict_t *what);
qboolean IsBrushUseableNow(edict_t *what, float when, edict_t *by_whom);
qboolean NotBadDoor(edict_t *what, float when, edict_t *by_whom);


// this is a bit of a misnomer - it's actually a section of a cone...

// NOTE - fixme - optimize this.  it's probably safe to store off both the dir and the cone side dir....
// that will reduce this operation from 4 vectornormalizes down to 1...  also the conestart

int pointBetweenSpheres(vec3_t start, float startrad, vec3_t end, float endrad, vec3_t testpoint)
{
	vec3_t	dir;
	float	len;
	vec3_t	intest;

	VectorSubtract(end, start, dir);

	//test against the first plane
	VectorSubtract(testpoint, start, intest);
	if(DotProduct(intest, dir) < 0)
	{
		return 0;
	}

	//test against the second plane
	VectorSubtract(end, testpoint, intest);
	if(DotProduct(intest, dir) < 0)
	{
		return 0;
	}

	
	//now test against the edges of the cone itself

	len = VectorNormalize(dir);

	vec3_t coneStart;

	VectorMA(start, -(startrad * len)/(endrad - startrad), dir, coneStart);

	vec3_t	sideDir;
	vec3_t	testDir;
	vec3_t	coneSideDir;
	vec3_t	coneSide;

	PerpendicularVector(sideDir, dir);

	//find the direction from the conestart to the testpoint
	VectorSubtract(testpoint, coneStart, testDir);
	VectorNormalize(testDir);

	//find the direction from the conestart to an arbitrary coneedge
	VectorMA(end, endrad, sideDir, coneSide);
	VectorSubtract(coneSide, coneStart, coneSideDir);
	VectorNormalize(coneSideDir);

	if(endrad < startrad)
	{
		VectorScale(dir, -1, dir);
	}

	if(DotProduct(testDir, dir) < DotProduct(coneSideDir, dir))
	{
		return 0;
	}

	return 1;
}





////////////////////////////////////////////////////////////////////////////////////////////////
//
//								CAIFloor related stuff
//
////////////////////////////////////////////////////////////////////////////////////////////////

vec3_t posTestVal;

void CAIFloor::load(byte **inData)
{
	readVect(inData, min);
/*	min[0] = (readChar(inData)*48);
	min[1] = (readChar(inData)*48);
	min[2] = (readChar(inData)*48);*/
	partial = readByte(inData);
	id = readShort(inData);
	for(int i = 0; i < NUM_NEAR_NODES; i++)
	{
		nearNodes[i] = readShort(inData);
	}
	for(i = 0; i < 4; i++)
	{
		adjFloors[i] = readShort(inData);
	}
	topZ = readFloat(inData);
}

int CAIFloor::getBestClearFloor(int recurse, CAIFloorSystem &system, int sideFrom)
{
	if(!system.floorIsOccupied(id, -1, -1, -1, -1))
	{
		if(!gi.inPVS(min, level.sight_client->s.origin))
		{
			return id;
		}
	}

	if(!recurse)
	{
		return -1;
	}

	//note - this is overkill, but if we really needed to flood properly, the below approach isn't correct:
	// it doesn't handle cases where missing floors leave gaps on i - 1
	for(int i = 0; i < 4; i++)
	{
		if(sideFrom != -1 && sideFrom != i && sideFrom != ((i+1)&3))
		{
			continue;
		}
		if(adjFloors[i] == -1)
		{
			continue;
		}

		CAIFloor *testFloor = system.getFloorForNumber(adjFloors[i]);
		if(!testFloor)
		{
			continue;
		}

		int bestId = testFloor->getBestClearFloor(recurse - 1, system, i);
		
		if(bestId)
		{
			return bestId;
		}
	}

	return -1;
}





void CAIFloorYRow::load(byte **inData)
{
	yValue = readFloat(inData);
	numFloors = readInt(inData);
	assert(numFloors > 0 && numFloors < 30);

	//floors = new CAIFloor[numFloors];
	floors = aiPoints.getFloors()->acquireFloorSpace(numFloors);//Cool!  This is so much better!

	for(int i = 0; i < numFloors; i++)
	{
		floors[i].load(inData);
	}
}

CAIFloor *CAIFloorYRow::getFloorForPosition(vec3_t position)
{
	int curCheck = numFloors-1;

	while(curCheck > -1)
	{
		if(position[2] > floors[curCheck].getZValue())
		{
			posTestVal[2] = floors[curCheck].getZValue();
			return &floors[curCheck];
		}
		curCheck--;
	}

	return 0;
}


void CAIFloorXRow::load(byte **inData)
{
	xValue = readFloat(inData);
	numYRows = readInt(inData);

	assert(numYRows > -1 && numYRows < 300);

	yRows = new CAIFloorYRow[numYRows];

	for(int i = 0; i < numYRows; i++)
	{
		yRows[i].load(inData);
	}
}

CAIFloor *CAIFloorXRow::getFloorForPosition(vec3_t position)
{
	int curCheck = numYRows>>1;
	int curCheckAdd = 1;
	int tempFloors = numYRows;

	while(tempFloors)
	{
		tempFloors>>=1;
		curCheckAdd<<=1;
	}
	curCheckAdd>>=2;//?

	for(;;)
	{
		if(position[1] < yRows[curCheck].getYValue())
		{
			curCheck -= curCheckAdd;
			if(curCheck < 0)curCheck = 0;
		}
		else if (position[1] > yRows[curCheck].getYValue() + 48)
		{
			curCheck += curCheckAdd;
			if(curCheck >= numYRows)curCheck = numYRows - 1;
		}
		else
		{
			posTestVal[1] = yRows[curCheck].getYValue();
			return yRows[curCheck].getFloorForPosition(position);
		}
		if(curCheckAdd == 0)
		{
			return 0;
		}
		curCheckAdd>>=1;
	}
	return 0;
}

void CAIFloorXRow::clear(void)
{
	delete[] yRows;
	yRows = 0;
}


void CAIFloorSystem::load(byte **inData)
{
	clear();

	numFloors = readInt(inData);
	numXRows = readInt(inData);
	assert(numXRows > -1 && numXRows < 500);

	floors = new CAIFloor[numFloors];
	xRows = new CAIFloorXRow[numXRows];

	for(int i = 0; i < numXRows; i++)
	{
		xRows[i].load(inData);
	}
	assert(floorsUsed == numFloors);

	occupArray = (numFloors)/16 + 1;//each entry is two bits, and there are 4 bytes per int
	floorOccupation = new int[occupArray];
	memset(floorOccupation, 0, sizeof(int) * occupArray);
}

void CAIFloorSystem::clear()
{
	for(int i = 0; i < numXRows; i++)
	{
		xRows[i].clear();
	}
	// I should probably learn properly about destructors =)
	delete[] xRows;
	numXRows = 0;
	delete[] floors;
	delete[] floorOccupation;
	floorOccupation = 0;
	occupArray = 0;
	floors = 0;
	xRows = 0;
	numFloors = 0;
	floorsUsed = 0;
}

CAIFloor *CAIFloorSystem::getFloorForPosition(vec3_t position)
{
	int curCheck = numXRows>>1;
	int curCheckAdd = 1;
	int tempFloors = numXRows;

	while(tempFloors)
	{
		tempFloors>>=1;
		curCheckAdd<<=1;
	}
	curCheckAdd>>=2;//?

	if(!xRows)
	{
		return 0;
	}

	if(!position)
	{
		return 0;
	}

	for(;;)
	{
		if(curCheck < 0)
		{
			curCheck = 0;
		}
		if(curCheck >= numXRows)
		{
			curCheck = numXRows - 1;
		}
		if(position[0] < xRows[curCheck].getXValue())
		{
			curCheck -= curCheckAdd;
			if(curCheck < 0)curCheck = 0;
		}
		else if (position[0] > xRows[curCheck].getXValue() + 48)
		{
			curCheck += curCheckAdd;
			if(curCheck >= numXRows)curCheck = numXRows - 1;
		}
		else
		{
			posTestVal[0] = xRows[curCheck].getXValue();
			return xRows[curCheck].getFloorForPosition(position);
		}
		if(curCheckAdd == 0)
		{
			return 0;
		}
		curCheckAdd>>=1;
	}

	return 0;
}

int CAIFloorSystem::floorIsOccupied(int val, int c1, int c2, int c3, int c4)
{
	int ix;
	int pos;

	ix = val>>4;
	pos = (val - (ix<<4))<<1;

	int curState = (floorOccupation[ix]>>pos)&3;

	if(curState == FLOORSLOT_EMPTY)
	{
		return 0;
	}
	else if(curState == FLOORSLOT_DOUBLE)
	{
		return 1;
	}
	else if(val == c1 || val == c2 || val == c3 || val == c4)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void CAIFloorSystem::addOccupiedFloor(int val)
{
	int ix;
	int pos;

	if(val < 0)
	{
		return;
	}

	assert(val < occupArray*16);

	ix = val>>4;
	pos = (val - (ix<<4))<<1;

	int curState = (floorOccupation[ix]>>pos)&3;

	if(curState == FLOORSLOT_EMPTY)
	{
		floorOccupation[ix] &= ~(curState<<pos);
		floorOccupation[ix] |= (FLOORSLOT_SINGLE<<pos);
	}
	else
	{
		floorOccupation[ix] &= ~(curState<<pos);
		floorOccupation[ix] |= (FLOORSLOT_DOUBLE<<pos);
	}
}

void CAIFloorSystem::addOccupiedFloor(CAIFloor *val)
{
	if(!val)return;
	addOccupiedFloor(val->getID());
}

void CAIFloorSystem::clearOccupiedFloors(void)
{
	memset(floorOccupation, 0, sizeof(int) * occupArray);
}

int CAIFloorSystem::getNearestFreePosition(vec3_t start, vec3_t spot)
{
	CAIFloor *floor = getFloorForPosition(start);
	if(!floor)
	{
		return 0;
	}

	int floorID = floor->getBestClearFloor(2, *this, -1);

	if(floorID == -1)
	{
		return 0;
	}

	VectorCopy(floors[floorID].getMin(), spot);
	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//								CAIPathLink
//
////////////////////////////////////////////////////////////////////////////////////////////////

void CAIPathLink::load(byte **inData)
{
	linkId = readInt(inData);
	dist = readFloat(inData);
	type = (connectType)readInt(inData);

	assert(type >= LINK_NORMAL);
	assert(type <= LINK_JUMP);
	assert(dist >= 0.0f);
	assert(dist < 9000);//this should catch any possible distance in our game, anyway
	assert(linkId > -1);
	assert(linkId < 4096);//way high range - should catch screwed up stuff
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//								CAIPathNode
//
////////////////////////////////////////////////////////////////////////////////////////////////

int CAIPathNode::pathLinksInRange(int numNodes)
{
	for(int i = 0; i < numNodes; i++)
	{
		if(path[i] == 0)
		{
			continue;//these are okay
		}
		if(link[path[i]-1].getLinkId() > numNodes)
		{
			return 0;
		}
	}
	return 1;
}

int CAIPathNode::pathInRange(void)
{
	for(int i = 0; i < sizeof(path); i++)
	{
		if(path[i] > numLinks)
		{
			return 0;
		}
	}
	return 1;
}

#include "windows.h"

void CAIPathNode::load(byte **inData, int numNodes)
{
	pointType = readInt(inData);

	assert(pointType <= EPS_FLEEENEMY);

	id = readInt(inData);

	assert(id > -1);
	assert(id < 512);

	readVect(inData, pos);
	pos[2] += 14;//strange, huh.  Monster origin is a fair bit higher than node origin...

	if(!(pointType & EPS_LINKMAIN))
	{	// these point types don't need any extra info
		return;
	}



	numLinks = readInt(inData);

	link = new CAIPathLink[numLinks];
	path = new byte[numNodes];

	for(int i = 0; i < numLinks; i++)
	{
		link[i].load(inData);
	}

	memcpy(path, *inData, numNodes);
	*inData += numNodes;

	pathRanks = new short[numNodes];
	memcpy(pathRanks, *inData, numNodes * sizeof(short));
	*inData += numNodes * sizeof(short);

//	assert(pathInRange());
//	assert(pathLinksInRange(numNodes));

	numCombatLinks = readInt(inData);

	combatLinks = new CAIPathLink[numCombatLinks];

	for(i = 0; i < numCombatLinks; i++)
	{
		combatLinks[i].load(inData);
	}

	radius = readFloat(inData);
	flee1 = readInt(inData);
	flee2 = readInt(inData);

	ignoreForSpawning = readInt(inData);

	minHeight = readFloat(inData);
	maxHeight = readFloat(inData);
}

void CAIPathNode::clean(void)
{
	delete[] link;
	delete[] path;
	delete[] pathRanks;
	delete[] combatLinks;
}

CAIPathLink	*CAIPathNode::getLinkWithId(int val)
{
	for(int i = 0; i< numLinks; i++)
	{
		if(link[i].getLinkId() == val)
		{
			return &link[i];
		}
	}
	return 0;
}

int	CAIPathNode::getDest(int finalNode)
{
	if(!path || (path < 0))
	{
		Com_Printf("Error!  Problem with path!\n");
		return finalNode;
	}
	//return path[finalNode];
	//compensate for an off by one error from above

	if(id == finalNode)
	{
		return finalNode;
	}
	if(path[finalNode] == 0)
	{
		// there's no way to get here - why the hell wasn't I checking against this
		// this is uberpathetic
		//return finalNode;//just force myself towards target 'cause I'm dumb
		return 0;
	}

	int val = link[path[finalNode]-1].getLinkId();
	assert(val >= 0);
	return val;
}

int CAIPathNode::isValidCombatPoint(vec3_t blockDir)
{
	// better check the guard direction too
	return 1;
}

#define MIN_SEARCH_RAD 300

int CAIPathNode::getSpawnNode(float key, vec3_t spot, int allowBehind, vec3_t fwd)
{
	checkKey = key;

	//if(( (((!(level.clearedNodes[id]))&&(!allowBehind)) || (level.clearedNodes[id]&&allowBehind))
	if(( (((!(level.clearedNodes[id]))&&(!allowBehind)) || (allowBehind))
		||(game.playerSkills.getSpawnValue() > 3))&&(!ignoreForSpawning))
	{
		vec3_t dif;
		VectorSubtract(pos, level.sight_client->s.origin, dif);

		float dist = VectorNormalize(dif);

		if(!gi.inPVS(pos, level.sight_client->s.origin)||
			((dist > 400)&&(DotProduct(fwd, dif) < .5)))//leaves a 120 degree wedge in the front - should be safe
		{//just in case
			if(dist < 600)
			{
				CRadiusContent rad(pos, 100, 1, 1);
				if(!rad.getNumFound())
				//if(aiPoints.getFloors()->getNearestFreePosition(pos, spot))
				{
					VectorCopy(pos, spot);
					return 1;
				}
			}
		}
	}

	// this random value will help us end up with different points sometimes
	int offset = gi.irand(0, 1293);

	for(int i = 0; i < numLinks; i++)
	{
		if(aiPoints.getNode(link[(i+offset)%numLinks].getLinkId())->checkKey == key)continue;
		if(aiPoints.getNode(link[(i+offset)%numLinks].getLinkId())->getSpawnNode(key, spot, allowBehind, fwd))
		{
			return 1;
		}
	}
	return 0;//if we got here, this is a dead end of sorts
}

void CAIPathNode::markClearedNodes(float key)
{
	if(!level.clearedNodes[id])
	{
		if(gi.inPVS(pos, level.sight_client->s.origin))
		{
			level.clearedNodes[id] = 1;
		}
	}

	checkKey = key;

	if(!level.clearedNodes[id])
	{
		return;//once we hit a node that is not clear, we are quite done
	}

	for(int i = 0; i < numLinks; i++)
	{
		if(aiPoints.getNode(link[i].getLinkId())->checkKey == key)continue;
		aiPoints.getNode(link[i].getLinkId())->markClearedNodes(key);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//								CAIPaths
//
////////////////////////////////////////////////////////////////////////////////////////////////

void CAIPaths::load(void)
{
	byte	*setInfo;
	byte	*curPos;
	int		fileLen;
	char	version;
	int		i;

	char	name[MAX_OSPATH];

	clean();

	Com_sprintf (name, sizeof(name), "maps/%s.nvb", level.mapname);


	fileLen = gi.FS_LoadFile (name, (void **)&setInfo);
	if (fileLen == -1)
	{
		numNodes = 0;
		numCombatNodes = 0;
		//Com_Printf("WARNING: This map doesn't appear to have navigation data - it is likely that the enemies will behave poorly\n");
		return;
	}

	curPos = setInfo;

	version = readInt(&curPos);
	if(version != AI_EXPORT_VERSION)
	{
		numNodes = 0;
		numCombatNodes = 0;
		Com_Printf ("Invalid version (%d) for Info Point File (Should be %d) - please re-export the path data\n", version, AI_EXPORT_VERSION);
		gi.FS_FreeFile(setInfo);
		return;
	}


	numNodes = readInt(&curPos);
	node = new CAIPathNode[numNodes];
	for(i = 0; i < numNodes; i++)
	{
		node[i].load(&curPos, numNodes);
	}

	numCombatNodes = readInt(&curPos);
	assert(numCombatNodes < 1000);//uhoh
	combatNode = new CAIPathNode[numCombatNodes];
	for(i = 0; i < numCombatNodes; i++)
	{
		combatNode[i].load(&curPos, numCombatNodes);
	}

	floors.load(&curPos);

	assert((curPos - setInfo) == fileLen);//if not, we're loading too much - BAD BAD BAD


	gi.FS_FreeFile(setInfo);
}

void CAIPaths::clean(void)
{
	for(int i = 0; i < numNodes; i++)
	{
		node[i].clean();
	}
	for(i = 0; i < numCombatNodes; i++)
	{
		combatNode[i].clean();
	}
	delete[] node;
	delete[] combatNode;

	node = 0;
	combatNode = 0;
	numNodes = 0;

	floors.clear();
}

void CAIPaths::setupClientPathInfo(int *nodeVal, int *region)
{	// this guy comes up a lot, you know?
	if((ai_nonodes->value)||(!numNodes))
	{
		return;
	}
	
	floors.clearOccupiedFloors();

	CAIFloor *floor = floors.getFloorForPosition(level.sight_client->s.origin);

//	FX_MakeRing(level.sight_client->s.origin, gi.irand(3,5));

	if(floor)
	{
		*region = floor->getID();
		*nodeVal = floor->getNode();
		if(*nodeVal < 1)
		{
			*nodeVal = 0;
		}
		if(*nodeVal >= numNodes)
		{
			*nodeVal = 0;
		}
	}

	clientNode = *nodeVal;

	//fixme - use the adj stuff for this to be quite a bit faster
	vec3_t spot;
	VectorCopy(level.sight_client->s.origin, spot);
	spot[0] -= 16;
	spot[1] -= 16;
	CAIFloor *floor2 = floors.getFloorForPosition(spot);
	if(floor2)
	{
		clientCorners[0] = floor2->getID();
		floors.addOccupiedFloor(floor2);
	}

	spot[0] += 32;
	floor2 = floors.getFloorForPosition(spot);
	if(floor2)
	{
		clientCorners[1] = floor2->getID();
		floors.addOccupiedFloor(floor2);
	}

	spot[1] += 32;
	floor2 = floors.getFloorForPosition(spot);
	if(floor2)
	{
		clientCorners[2] = floor2->getID();
		floors.addOccupiedFloor(floor2);
	}

	spot[0] -= 32;
	floor2 = floors.getFloorForPosition(spot);
	if(floor2)
	{
		clientCorners[3] = floor2->getID();
		floors.addOccupiedFloor(floor2);
	}

	if(nodeVal > 0)
	{
		node[*nodeVal].markClearedNodes(searchKey++);
	}
}

ai_public_c *lazy;

int CAIPaths::getNode(vec3_t pos, int prev, int prevDest)
{
	CAIFloor *floor = floors.getFloorForPosition(pos);
	int retVal;
	if(floor)
	{
		retVal = floor->getNode();
		if(retVal < 1)
		{
			retVal = 0;
		}
	}
	else
	{
		retVal = 0;
	}
	return retVal;
}

void CAIPaths::linkEnemyIntoPath(vec3_t pos, nodeData_t *data)
{
	if((ai_nonodes->value)||(!numNodes))
	{
		return;
	}

	vec3_t testpos;

	VectorCopy(pos, testpos);
	testpos[2] += 16;

	CAIFloor *floor = floors.getFloorForPosition(testpos);

	if(floor)
	{
		data->curRegion = floor->getID();
	}
	else
	{//uhoh
		data->curRegion = -1;
	}
	VectorCopy(testpos, data->curSpot);
	floors.addOccupiedFloor(data->curRegion);

	vec3_t spot;
	VectorCopy(testpos, spot);

	if(data->curRegion == -1)
	{
		vec3_t spot;
		VectorCopy(testpos, spot);
		spot[0] -= 16;
		spot[1] -= 16;
		floor = floors.getFloorForPosition(spot);
		if(floor)
		{
			data->curRegion = floor->getID();
		}
	}
	if(data->curRegion == -1)
	{
		spot[0] += 32;
		floor = floors.getFloorForPosition(spot);
		if(floor)
		{
			data->curRegion = floor->getID();
		}
	}
	if(data->curRegion == -1)
	{
		spot[1] += 32;
		floor = floors.getFloorForPosition(spot);
		if(floor)
		{
			data->curRegion = floor->getID();
		}
	}
	if(data->curRegion == -1)
	{
		spot[0] -= 32;
		floor = floors.getFloorForPosition(spot);
		if(floor)
		{
			data->curRegion = floor->getID();
		}
	}
	if(data->curRegion == -1)
	{
		data->curNode = 0;
	}
	else
	{
		floor = floors.getFloorForNumber(data->curRegion);
		if(floor)
		{
			int oldNode = data->curNode;
			data->curNode = floor->getNode();
			if(oldNode != data->curNode)
			{
				data->lastNode3 = data->lastNode2;
				data->lastNode2 = data->lastNode;
				data->lastNode = oldNode;
			}
		}
		else
		{
			data->curRegion = -1;
			data->curNode = 0;
		}
	}
	data->corner1 = data->corner2 = data->corner3 = data->corner4 = data->curRegion;
}

qboolean IsUseableBrush(edict_t *what);
qboolean IsBrushUseableNow(edict_t *what, float when, edict_t *by_whom);

int CAIPaths::clearLineForFloor(int startID, vec3_t end, nodeData_t *data, int approach)
{
	int curFloor = startID;
	int endFloorNum;
	int	differsOnlyByZ = 0;
	int clearedPartial = 1;//this lets us know if we're on clear squares yet
	int lastFailed = 0;

	CAIFloor *endFloor = floors.getFloorForPosition(end);

	if(endFloor)
	{
		endFloorNum = endFloor->getID();
	}
	else
	{
		endFloorNum = -1;
	}

	if(startID == endFloorNum)
	{
		return !(floors.floorIsOccupied(curFloor, data->corner1, data->corner2, data->corner3, data->corner4));
	}

	vec3_t startPos, startSide;

	VectorCopy(floors.getFloorForNumber(startID)->getMin(), startPos);
	startPos[2] = 0;

	vec3_t difOuter;
	VectorSubtract(end, startPos, difOuter);

	startSide[1] = difOuter[0];
	startSide[0] = -difOuter[1];
	startSide[2] = 0;

	CAIFloor *floor = floors.getFloorForNumber(curFloor);
	if(floor)
	{
		clearedPartial = !(floor->getPartial());
	}

	if(endFloorNum == -1)
	{
		return 0;
	}

	if(aipoints_show->value)
	{
		paletteRGBA_t	col = {250, 250, 0, 250};
		FX_MakeLine(floors.getFloorForNumber(startID)->getMin(), floors.getFloorForNumber(endFloorNum)->getMin(), col, 1);
		FX_MakeRing(floors.getFloorForNumber(startID)->getMin(), 3);
		FX_MakeRing(floors.getFloorForNumber(endFloorNum)->getMin(), 3);
	}

	do
	{
		int nextLeft, nextRight;

		CAIFloor *floor = floors.getFloorForNumber(curFloor);

/*		vec3_t show;
		VectorCopy(floor->getMin(), show);
		show[0] += 24;
		show[1] += 24;
		show[2] += 8;
		FX_MakeRing(show, 6);*/

		if(fabs(floor->getMin()[0] - endFloor->getMin()[0]) < 5)
		{//we're already in the proper x plane - just move in y
			if(floor->getMin()[1] > endFloor->getMin()[1])
			{
				curFloor = floor->getAdj(FLOOR_YMINUS);
			}
			else
			{
				curFloor = floor->getAdj(FLOOR_YPLUS);
			}
		}
		else if(fabs(floor->getMin()[1] - endFloor->getMin()[1]) < 5)
		{//we're already in the proper x plane - just move in y
			if(floor->getMin()[0] > endFloor->getMin()[0])
			{
				curFloor = floor->getAdj(FLOOR_XMINUS);
			}
			else
			{
				curFloor = floor->getAdj(FLOOR_XPLUS);
			}
		}
		else 
		{
			if(difOuter[0] >= 0 && difOuter[1] >= 0)
			{
				nextLeft = floor->getAdj(FLOOR_YPLUS);
				nextRight = floor->getAdj(FLOOR_XPLUS);
			}
			else if(difOuter[0] <= 0 && difOuter[1] >= 0)
			{
				nextLeft = floor->getAdj(FLOOR_XMINUS);
				nextRight = floor->getAdj(FLOOR_YPLUS);
			}
			else if(difOuter[0] >= 0 && difOuter[1] <= 0)
			{
				nextLeft = floor->getAdj(FLOOR_XPLUS);
				nextRight = floor->getAdj(FLOOR_YMINUS);
			}
			else if(difOuter[0] <= 0 && difOuter[1] <= 0)
			{
				nextLeft = floor->getAdj(FLOOR_YMINUS);
				nextRight = floor->getAdj(FLOOR_XMINUS);
			}
			else
			{
				return 0;//it really shouldn't ever get here, though
			}

			vec3_t dif;
			VectorSubtract(floor->getMin(), startPos, dif);
			if(DotProduct(startSide, dif) > 0 && nextRight != -1)
			{
				if(nextRight != -1 && !lastFailed)
				{
					curFloor = nextRight;
					lastFailed = 0;
				}
				else
				{
					lastFailed = 1;
				}
			}
			else
			{
				if(nextLeft != -1 && !lastFailed)
				{
					lastFailed = 0;
					curFloor = nextLeft;
				}
				else
				{
					lastFailed = 1;
					curFloor = nextRight;
				}
			}
		}

		if(fabs(floor->getMin()[0] - endFloor->getMin()[0]) < 5 &&
			fabs(floor->getMin()[1] - endFloor->getMin()[1]) < 5)
		{
			differsOnlyByZ = 1;
		}

		//er
		if(curFloor != -1 && curFloor != startID)
		{
			CAIFloor *floor2 = floors.getFloorForNumber(curFloor);
			if(floor2 && (floor2->getPartial()&PARTIAL_FLOORGAP))
			{
				if(clearedPartial)
				{	//we reject these once we're on clear ground
					differsOnlyByZ = 1;
				}
			}
			else
			{
				clearedPartial = 1;
			}
		}

/*		if(floors.floorIsOccupied(curFloor, data->corner1, data->corner2, data->corner3, data->corner4))
		{
			if(!approach && (curFloor == clientCorners[0] || curFloor == clientCorners[1] || 
				curFloor == clientCorners[2] || curFloor == clientCorners[3]))
			{	//when retreating, we do not want to go through the player - that would be bad
				differsOnlyByZ = 1;
			}
			//differsOnlyByZ = 1;
		}*/

	}while(curFloor > -1 && curFloor != endFloorNum && !differsOnlyByZ);

	if(curFloor == endFloorNum)
	{
		return 1;
	}
	return 0;
}


int CAIPaths::getClearPath(int startRegion, int endNode, int approach, nodeData_t *data, vec3_t start, edict_t *ent)
{
	CAIFloor *floor = floors.getFloorForNumber(startRegion);

	int limit;
	int travelNode;
	int foundNode = -1;

	travelNode = -1;

	if(endNode == -1)
	{
		return -1;
	}

	if(approach)
	{
		limit = -1;
	}
	else
	{
		limit = 15000;
	}

	do
	{
		for(int i = 0; i < NUM_NEAR_NODES; i++)
		{
			int testNode = floor->getNearNode(i);
			if(testNode == -1)
			{
				break;//okay, all done
			}
			if(travelNode == -1)
			{
				travelNode = floor->getNearNode(i);
				continue;
			}
			if(		((node[endNode].getRank(testNode) < node[endNode].getRank(travelNode) ||
					node[endNode].getRank(travelNode) <= limit)
				&& approach
				&& node[endNode].getRank(testNode) > limit)
				
					||

					((node[endNode].getRank(testNode) > node[endNode].getRank(travelNode) ||
					node[endNode].getRank(travelNode) >= limit)
				&& !approach
				&& node[endNode].getRank(testNode) < limit))
			{
				travelNode = testNode;
			}
		}

		//always check out the last next as well, just in case
		if(data->nextNode > 0 &&
			(travelNode < 1 ||
			(node[endNode].getRank(data->nextNode) > node[endNode].getRank(travelNode) && !approach) ||
			(node[endNode].getRank(data->nextNode) < node[endNode].getRank(travelNode) && approach)))
		{
			travelNode = data->nextNode;
		}

		if (travelNode!=-1)
		{
			assert(travelNode>=0);
			assert(travelNode<numNodes);


			//okay, so now we have the node we'd like to take a peek at
			if((!approach && node[endNode].getRank(travelNode) >= limit) ||
				(approach && node[endNode].getRank(travelNode) <= limit))
			{
				travelNode = -1;
			}
			else if(clearLineForFloor(startRegion, node[travelNode].getPos(), data, approach))
			{
				trace_t tr;

				vec3_t tempMin, tempMax;
				VectorCopy(ent->mins, tempMin);
				VectorCopy(ent->maxs, tempMax);
				tempMin[2] += 20;
				tempMax[2] -= 10;

				vec3_t t;
				VectorSubtract(node[travelNode].getPos(), start, t);
				VectorNormalize(t);
				VectorMA(start, 64, t, t);

				gi.trace(start, tempMin, tempMax, t, ent, MASK_MONSTERSOLID, &tr);

				//bad doors ruin it for everyone.  No good.
				if(NotBadDoor(tr.ent, level.time, ent))
				{
					foundNode = travelNode;
				}
				else
				{
					limit = node[endNode].getRank(travelNode);//eh?
					travelNode = -1;
				}
			}
			else
			{
				limit = node[endNode].getRank(travelNode);
				travelNode = 0;
				foundNode = 0;
				//stop, ya bastard.  If we could remember this were a bad door, tihs would be something
				// else entirely
			}
		}

	}while(travelNode != -1 && foundNode == -1);

	return travelNode;
}

int CAIPaths::getFloorPath(int startRegion, int nextNode, nodeData_t *data, int recurse, int ignoreRegion, float *distFound, int sourceStart)
{
	CAIFloor *floor = floors.getFloorForNumber(startRegion);

	if(startRegion == -1)
	{
		return startRegion;
	}

	if(!floor)
	{
		return startRegion;
	}

	vec3_t dif;
	int regToGoTo = startRegion;

	int altRegions[4];
	int numAltRegions = 0;

	VectorSubtract(node[nextNode].getPos(), floor->getMin(), dif);
	float bestDist = VectorLengthSquared(dif) * (floor->getPartial() ? 1:.5);

	for(int i = 0; i < 4; i++)
	{
		if(floor->getAdj(i) == -1 || floor->getAdj(i) == ignoreRegion || floor->getAdj(i) == sourceStart)
		{
			continue;
		}
		
		CAIFloor *other = floors.getFloorForNumber(floor->getAdj(i));

		if(!other)
		{
			continue;
		}

		vec3_t test;
		VectorCopy(other->getMin(), test);
		test[0] += 24;
		test[1] += 24;

		VectorSubtract(node[nextNode].getPos(), test, dif);
		float vls = VectorLengthSquared(dif) * (other->getPartial() ? 1:.5);

		if(vls >= bestDist)
		{
			altRegions[numAltRegions] = other->getID();
			numAltRegions++;
			continue;
		}

		for(int j = 0; j < NUM_NEAR_NODES; j++)
		{
			if(other->getNearNode(j) == nextNode || other->getNearNode(j) == -1)
			{
				break;//all this crap should prevent some oscillation
			}
		}
		if(!(other->getPartial()&PARTIAL_FLOORGAP) && j < NUM_NEAR_NODES && other->getNearNode(j) != -1 
			&& !floors.floorIsOccupied(floor->getAdj(i), data->corner1, data->corner2, data->corner3, data->corner4))
		{
			bestDist = vls;
			regToGoTo = other->getID();
		}
	}

	//warning - recursion could go haywire here
	//if(recurse && selectedFloor != -1)
	if(regToGoTo == startRegion && numAltRegions && recurse)
	{//if only one floor was closer than my current one, and it is blocked, we need to try the ones to my side

		for(int i = 0; i < numAltRegions; i++)
		{
			float testDist;
			int side = getFloorPath(altRegions[i], nextNode, data, recurse - 1, startRegion, &testDist, sourceStart);

			if(side != altRegions[i] && side != startRegion && (testDist < bestDist))
			{
				regToGoTo = side;
				//break;
			}
		}
	}

	if(distFound)
	{
		*distFound = bestDist;
	}
	return regToGoTo;
}

#define MIN_CIRCLE_TIME 3

int CAIPaths::clearForDirection(int startRegion, int endNode, nodeData_t *data, vec3_t endSpot)
{
	CAIFloor *floor;

	if(startRegion < 1)
	{
		return 0;
	}

	floor = floors.getFloorForNumber(startRegion);


	vec3_t dif;
	VectorSubtract(endSpot, floor->getMin(), dif);
	float bestDist = VectorLengthSquared(dif);

	int pathValid = 0;

	for(int i = 0; i < 4; i++)
	{
		if(floor->getAdj(i) == -1)
		{	//fixme - not right
			continue;
		}
		
		CAIFloor *other = floors.getFloorForNumber(floor->getAdj(i));

		if(!other)
		{
			continue;
		}

		VectorSubtract(endSpot, other->getMin(), dif);
		float vls = VectorLengthSquared(dif);
	
		if(vls >= bestDist)
		{
			continue;
		}

		int j;
		if(endNode != -1)
		{
			for(j = 0; j < NUM_NEAR_NODES; j++)
			{
				if(other->getNearNode(j) == endNode || other->getNearNode(j) == -1)
				{
					break;//all this crap should prevent some oscillation
				}
			}
		}

		if(((endNode == -1|| j < NUM_NEAR_NODES && other->getNearNode(j) != -1))  &&
			!(other->getPartial()&PARTIAL_FLOORGAP) &&
			//!(other->getPartial()&PARTIAL_FLOOR) &&
			!floors.floorIsOccupied(floor->getAdj(i), data->corner1, data->corner2, data->corner3, data->corner4))
		{
			pathValid = 1;
		}
	}
	return pathValid;
}

void CAIPaths::getPath(vec3_t start, vec3_t end, nodeData_t *data, ai_public_c *who, edict_t *ent, int approach, edict_t *targ)
{
	if((ai_nonodes->value)||(!numNodes)||(!data->curNode))
	{
		data->blocked = 1;
		VectorCopy(start, data->goPoint);//igh
		return;
	}

	if(who->BeingWatched())
	{
		int asdf = 9;
	}

	CAIFloor *endFloor = floors.getFloorForPosition(end);
	if(!endFloor)
	{
		return;
	}

	data->finalNode = endFloor->getNode();
	data->blocked = 0;

	if(data->finalNode == -1)
	{
		data->nextNode = 0;
		VectorCopy(start, data->goPoint);
		data->blocked = 1;
		return;
	}


	int travelNode = getClearPath(data->curRegion, data->finalNode, approach, data, start, ent);

	CAIFloor *floor = floors.getFloorForNumber(data->curRegion);

	if(travelNode < 1)
	{
		travelNode = 0;//just to be safe
	}

	data->nextNode = travelNode;

	if(node[travelNode].getDest(data->finalNode) < 1)
	{	//oh dear
		data->nextNode = 0;
	}

	if(aipoints_show->value && data->nextNode)
	{
		paletteRGBA_t	col = {250, 0, 0, 250};
		FX_MakeLine(start, node[data->nextNode].getPos(), col, 1);
		FX_MakeRing(node[data->nextNode].getPos(), 6);
	}


	if(!data->nextNode || data->nextNode == -1)
	{
		data->nextNode = 0;
		VectorCopy(start, data->goPoint);
		data->blocked = 1;
	}
	else if((endFloor && endFloor->getID() == data->curRegion)&&(approach))
	{	// we're already within 48 of our ultimate goal - yikes... only if approaching, of course !
		VectorCopy(end, data->goPoint);
	}
	else
	{
		vec3_t testMin;
		vec3_t testMax;

		VectorCopy(ent->mins, testMin);
		//testMin[0] += 4;
		//testMin[1] += 4;
		testMin[2] += 20;//stepheightish
		VectorCopy(ent->maxs, testMax);
		//testMax[2] -= 10;//er?
		//testMax[0] -= 4;
		//testMax[1] -= 4;

		if(!floor)
		{
			VectorCopy(start, data->goPoint);
			data->blocked = 1;
		}
		else
		{
			int regToGoTo = data->curRegion;
			int pathCheck = 1;

			if(clearForDirection(data->curRegion, -1, data, end))
			{
				VectorCopy(end, data->goPoint);
				trace_t tr;

				vec3_t dif2;
				VectorSubtract(end, start, dif2);
/*				if(VectorNormalize(dif2) > 32)
				{
					VectorMA(start, 32.0, dif2, data->goPoint);
				}
				else
				{*/
					VectorCopy(end, data->goPoint);
//				}

				gi.trace(start, testMin, testMax, data->goPoint, ent, MASK_MONSTERSOLID, &tr);
				if((tr.fraction > .99 && !tr.startsolid && !tr.allsolid) || (approach && tr.ent == level.sight_client) ||
					(tr.ent && IsUseableBrush(tr.ent) && IsBrushUseableNow(tr.ent, level.time, ent)))
				{
					//VectorMA(start, 32, dif2, data->goPoint);
					VectorCopy(end, data->goPoint);
					Enemy_Printf(who, "Go to dest - 'tis clear\n");
					pathCheck = 0;
					int asdf = 9;
				}
			}

			if(pathCheck && clearForDirection(data->curRegion, data->nextNode, data, node[data->nextNode].getPos()))
			{
				VectorCopy(node[data->nextNode].getPos(), data->goPoint);
				trace_t tr;

				vec3_t dif2;
				VectorSubtract(node[data->nextNode].getPos(), start, dif2);
				VectorNormalize(dif2);
				VectorScale(dif2, 64.0, dif2);
				VectorAdd(dif2, start, dif2);
				VectorCopy(dif2, data->goPoint);

/*				if(VectorNormalize(dif2) > 32)
				{
					VectorScale(dif2, 32.0, dif2);
					VectorAdd(dif2, start, dif2);
					VectorCopy(dif2, data->goPoint);
				}
				else
				{
					VectorCopy(node[data->nextNode].getPos(), data->goPoint);
				}*/

				// we are testing 64 instead of 32 now, but it only has to be halfway successful.  Mainly to avoid evil doors
				gi.trace(start, testMin, testMax, data->goPoint, ent, MASK_MONSTERSOLID, &tr);
				if(((tr.fraction > .5 && !tr.startsolid && !tr.allsolid) || (approach && tr.ent == level.sight_client) ||
					(tr.ent && IsUseableBrush(tr.ent) && IsBrushUseableNow(tr.ent, level.time, ent))))
				{
					//Com_Printf("Clear in direction, straight line\n");
					Enemy_Printf(who, "Clear in direction, so use that\n");
					pathCheck = 0;
					int asdf = 9;
					VectorSubtract(node[data->nextNode].getPos(), start, dif2);
					VectorNormalize(dif2);
					VectorScale(dif2, 32.0, dif2);
					VectorAdd(dif2, start, dif2);
					VectorCopy(dif2, data->goPoint);

				}
			}

			if(pathCheck)
			{
				regToGoTo = getFloorPath(data->curRegion, data->nextNode, data, 3, -1, 0, data->curRegion);

				CAIFloor *goodFloor = floors.getFloorForNumber(regToGoTo);

				if(regToGoTo != data->curRegion)
				{
					CAIFloor *goodFloor = floors.getFloorForNumber(regToGoTo);
					if(goodFloor)
					{
						VectorCopy(goodFloor->getMin(), data->goPoint);
						data->goPoint[0] += 24;
						data->goPoint[1] += 24;
						data->goPoint[2] += 24;
						Enemy_Printf(who, "Procede to node %d from region %d to region %d\n", data->nextNode, (int)floor->getID(), (int)goodFloor->getID());
					}
				}
				else
				{	// probably have excessively tight quarters then =/
					VectorCopy(node[data->nextNode].getPos(), data->goPoint);
					//VectorCopy(start, data->goPoint);
				}
			}

			trace_t tr;

			gi.trace(start, testMin, testMax, data->goPoint, ent, MASK_MONSTERSOLID, &tr);

			if(((tr.fraction > .99 || tr.ent == targ) && !tr.startsolid && !tr.allsolid) ||
				(approach && tr.ent == level.sight_client) ||
				(tr.ent && IsUseableBrush(tr.ent) && IsBrushUseableNow(tr.ent, level.time, ent)))
			{
				if(pathCheck && regToGoTo != data->curRegion)
				{
					floors.addOccupiedFloor(regToGoTo);
				}

				int asdf = 9;
			}
			else
			{
				Enemy_Printf(who, "Trying for side\n");
//				VectorCopy(start, data->goPoint);

				vec3_t sideDir;
				sideDir[1] = tr.plane.normal[0];
				sideDir[0] = -tr.plane.normal[1];
				sideDir[2] = 0;

				vec3_t goalDif, toAvoid;
				VectorSubtract(data->goPoint, start, goalDif);

				int skip = 0;
				if(DotProduct(sideDir, goalDif) > .717)
				{
					VectorCopy(sideDir, toAvoid);
				}
				else if(DotProduct(sideDir, goalDif) < -.717)
				{
					VectorScale(sideDir, -1.0, toAvoid);
				}
				else
				{
					skip = 1;
				}

				if(!skip)
				{
					VectorNormalize(toAvoid);//irk
					VectorMA(toAvoid, .2, tr.plane.normal, toAvoid);
					VectorNormalize(toAvoid);

					vec3_t testSpot;
					VectorMA(start, 20, toAvoid, testSpot);//?
					gi.trace(start, testMin, testMax, testSpot, ent, MASK_MONSTERSOLID, &tr);

					if((tr.fraction > .99 && !tr.startsolid && !tr.allsolid) || (approach && tr.ent == level.sight_client) ||
						(tr.ent && IsUseableBrush(tr.ent) && IsBrushUseableNow(tr.ent, level.time, ent)))
					{
						CAIFloor * myFloor = floors.getFloorForPosition(testSpot);
						if(myFloor)
						{
							floors.addOccupiedFloor(myFloor->getID());
						}
						VectorCopy(testSpot, data->goPoint);
					}
					else
					{//stuck
						VectorCopy(start, data->goPoint);
					}
				}
				else
				{
					data->nextNode = 0;
					VectorCopy(start, data->goPoint);
					data->blocked = 1;
				}
			}

		}
	}

	if(aipoints_show->value)
	{
		paletteRGBA_t	col = {0, 250, 0, 250};
		FX_MakeLine(start, data->goPoint, col, 1);
	}

	VectorCopy(start, data->curSpot);

	vec3_t dif;
	VectorSubtract(data->goPoint, data->curSpot, dif);
	if(VectorLengthSquared(dif) < .1)
	{
		data->lastDirSetTime = level.time;
		VectorCopy(dif, data->lastDir);
	}
	else
	{
		VectorNormalize(dif);
		if((DotProduct(dif, data->lastDir) < -.717)&&(level.time - data->lastDirSetTime < MIN_CIRCLE_TIME))
		{	//are we being encouranged to backtrack??? - this is a 90 degree wedge behind the guy
			VectorCopy(data->curSpot, data->goPoint);
			data->blocked = 1;
		}
		else
		{
			data->lastDirSetTime = level.time;
			VectorCopy(dif, data->lastDir);
		}
	}
}

void CAIPaths::getReversePath(vec3_t start, vec3_t end, nodeData_t *data, ai_public_c *who, edict_t *ent, edict_t *targ)
{
	getPath(start, end, data, who, ent, 0, targ);
}


#define MAX_COMBAT_POINT_RANGE 768
#define MIN_COMBAT_POINT_RANGE 128

void CAIPaths::getCombatPoint(nodeData_t *data, vec3_t start, vec3_t end, vec3_t dir)
{
}

int CAIPaths::getNearestFleePoint(nodeData_t *data, vec3_t outSpot, int team)
{
	int fleeNode;

	if((ai_nonodes->value)||(data->curNode==-1)||(data->curNode == 0))
	{
		return 0;
	}

	if(!team)
	{	// team 0 is the good guys, as it were
		fleeNode = node[data->curNode].getFleePoint(1);
	}
	else
	{
		fleeNode = node[data->curNode].getFleePoint(2);
		if(!fleeNode)
		{	// default back =/
			fleeNode = node[data->curNode].getFleePoint(1);
		}
	}

	if(!fleeNode)
	{
		VectorCopy(data->curSpot, outSpot);
		return 0;
	}

	VectorCopy(node[fleeNode].getPos(), outSpot);
	return 1;
}

#define MIN_BEHIND_DIST 400

void CAIPaths::idealWanderPoint(nodeData_t *data, ai_public_c *who)
{
	CAIFloor *floor;

	floor = floors.getFloorForPosition(data->curSpot);

	if(!floor)
	{
		data->blocked = 1;
		return;
	}

	for(int i = 0; i < 4; i++)
	{
		if(floor->getAdj(i) == -1)
		{
			continue;
		}
		
		CAIFloor *other = floors.getFloorForNumber(floor->getAdj(i));

		if(!other)
		{
			continue;
		}

		if(floors.floorIsOccupied(floor->getAdj(i), data->corner1, data->corner2, data->corner3, data->corner4))
		{//someone near me - see if I can make space
			int oppositeFloor = floor->getAdj((i+2)&3);
			if(oppositeFloor != -1)
			{
				CAIFloor *opFloor = floors.getFloorForNumber(oppositeFloor);

				if(opFloor)
				{
					if(!opFloor->getPartial())
					{
						if(!floors.floorIsOccupied(oppositeFloor, data->corner1, data->corner2, data->corner3, data->corner4))
						{
							data->blocked = 0;
							VectorCopy(opFloor->getMin(), data->goPoint);
							floors.addOccupiedFloor(oppositeFloor);
						}
					}
				}
			}
		}
	}
}

int CAIPaths::getSpawnPosition(vec3_t outSpot, int allowBehind)
{
	if((ai_nonodes->value)||(!numNodes)||(clientNode < 1)||(!level.sight_client))
	{
		return 0;
	}

	vec3_t fwd;
	AngleVectors(level.sight_client->s.angles, fwd, 0, 0);

	if(node[clientNode].getSpawnNode(searchKey++, outSpot, allowBehind, fwd))
	{
		return 1;
	}
	return 0;
}

int CAIPaths::search(nodeData_t *data, ai_public_c *who)
{
	if((ai_nonodes->value)||(!numNodes)||(clientNode < 1)||(!level.sight_client))
	{
		return 0;
	}

	int nodeToSee = 0;

	for(int i = 0; i < node[data->curNode].getNumLinks(); i++)
	{
		int checkNode = node[data->curNode].getLink(i);

		if(checkNode == data->lastNode)
		{
			continue;
		}

		if(checkNode == data->lastNode2)
		{
			continue;
		}
		if(checkNode == data->lastNode3)
		{
			continue;
		}

		if(node[checkNode].getNumLinks() == 1)
		{	//dead end
			continue;
		}

/*		if(node[checkNode].getNumLinks() == 2)
		{
			if((node[checkNode].getLink(0) == data->lastNode && node[checkNode].getLink(1) == data->curNode)||
				(node[checkNode].getLink(1) == data->lastNode && node[checkNode].getLink(0) == data->curNode))
			{
				continue;
			}
		}*/

		if(nodeToSee)
		{	// two possible paths - just hang around 'cause we sadly don't know where to go right now
			return 0;
		}

		nodeToSee = checkNode;
	}

	if(nodeToSee)
	{
		VectorCopy(node[nodeToSee].getPos(), data->goPoint);
		return 1;
	}
	return 0;
}

int CAIPaths::getLookSpot(vec3_t source, vec3_t outSpot, int key)
{
	int myNode = getNode(source, 0);

	if(!myNode)
	{
		return 0;
	}

	CAIPathNode *checkNode = &node[myNode];

	if(!checkNode)
	{
		return 0;
	}

	if(checkNode->getNumLinks() <= 0)
	{
		return 0;
	}

	int lookNode = checkNode->getLink(key % checkNode->getNumLinks());

	if(!lookNode)
	{
		return 0;
	}

	checkNode = &node[lookNode];

	if(!checkNode)
	{
		return 0;
	}

	VectorCopy(checkNode->getPos(), outSpot);

	return 1;
}