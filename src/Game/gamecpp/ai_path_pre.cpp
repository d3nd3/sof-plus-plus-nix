#include "g_local.h"
#include "ai_pathfinding.h"

#include <windows.h>

void SetGenericEffectInfo(edict_t *ent);

////////////////////////////////////////////////////////////////////////////////////////////////
//
//								Globals
//
////////////////////////////////////////////////////////////////////////////////////////////////

//omg - this is the ugliest use of a global I've ever seen today!!!
//I'm not kidding, either.  This is BAD
CAIListData			*aiList = 0;
CAIPaths			aiPoints;

int					failedConnects;
int					degenNodes;

int					loadVersion;
int					showboxes = 1;

////////////////////////////////////////////////////////////////////////////////////////////////
//
//								Misc functions
//
////////////////////////////////////////////////////////////////////////////////////////////////

float readFloat(byte **in)
{
	float rVal = *(float *)*in;
	*in+=4;

	return rVal;
}

void readVect(byte **in, vec3_t v)
{
	v[0] = readFloat(in);
	v[1] = readFloat(in);
	v[2] = readFloat(in);
}

int readInt(byte **in)
{
	int rVal = *(int *)*in;
	*in+=4;
	return rVal;
}

short readShort(byte **in)
{
	short rVal = *(short *)*in;
	*in+=2;
	return rVal;
}

byte readByte(byte **in)
{
	byte rVal = *(byte *)*in;
	*in+=1;
	return rVal;
}

signed char readChar(byte **in)
{
	signed char rVal = *(signed char *)*in;
	*in+=1;
	return rVal;
}


#define FLOOR_DIST_CHECK 256

void getFloorPoint(vec3_t spot, vec3_t floorPoint)
{
	vec3_t testmin = {-13, -13, 0};//zcheck?
	vec3_t testmax = { 13,  13,  24};

	trace_t tr;

	VectorCopy(spot, floorPoint);
	floorPoint[2] -= FLOOR_DIST_CHECK;
	gi.trace(spot, testmin, testmax, floorPoint, NULL, MASK_SOLID, &tr);
	VectorCopy(tr.endpos, floorPoint);
}

//is this okay?  I dunno...

#define DIVISION_DIST 4 // how far down to check
#define DIST_ABOVE_TO_CHECK 32
#define MAX_PATH_HEIGHT_DIFF 16 // ? will this cause step height concerns?

int isPathLevel(vec3_t start, vec3_t end, float maxHeightMod, vec3_t stop)
{
	// basically, what we're doing here is making certain that there are no seriously huge gaps in the path
	// if there are,this is no good

	vec3_t floorStart, floorEnd;
	vec3_t curCheck, curAdd, curCheckDown;
	vec3_t prevCheckDown;
	
	getFloorPoint(start, floorStart);
	getFloorPoint(end, floorEnd);

	int numToAdd;

	VectorSubtract(floorEnd, floorStart, curAdd);
	numToAdd = VectorLength(curAdd)/DIVISION_DIST;
	VectorNormalize(curAdd);
	VectorScale(curAdd, DIVISION_DIST, curAdd);
	VectorCopy(floorStart, prevCheckDown);
	VectorAdd(floorStart, curAdd, curCheck);
	curCheck[2] += DIST_ABOVE_TO_CHECK;//don't want this guy to start in the floor - that's a big issue right now ;/

	//Com_Printf("Checking for level path\n");

	for(int i = 1; i < numToAdd; i++)
	{	// check for huge jumps
		getFloorPoint(curCheck, curCheckDown);

		if(fabs(curCheckDown[2] - prevCheckDown[2]) > MAX_PATH_HEIGHT_DIFF * maxHeightMod)
		{
			if(stop)
			{
				VectorCopy(curCheck, stop);
			}
			return 0;
		}

		VectorAdd(curCheck, curAdd, curCheck);
		VectorCopy(curCheckDown, prevCheckDown);//crap - I forgot this ;(
	}

	if(stop)
	{
		VectorCopy(curCheck, end);
	}

	return 1;
}

void killportals (void)
{
	edict_t	*t = NULL;

	while ((t = G_Find (t, FOFS(classname), "func_areaportal")))
	{
		gi.SetAreaPortalState (t->style, 1);
	}
}

void elimEnts(void)
{
	edict_t *ed;

	killportals();

	for(ed = &g_edicts[0]; ed < &g_edicts[MAX_EDICTS]; ed++)
	{
		if(!ed->inuse)
		{
			continue;
		}

		if(ed->ai)
		{	// this is positively unsafe, but I really don't care
			G_FreeEdict(ed);
			continue;
		}
		else if(!strncmp(ed->classname, "func", 4))
		{
			// this won't work on all of them... ?
			//if((strcmp(ed->classname, "func_wall"))&&(strcmp(ed->classname, "func_breakable_brush")))
			{
				G_FreeEdict(ed);
				continue;
			}
		}
		else if(!strncmp(ed->classname, "trigger", 6))
		{
			G_FreeEdict(ed);
			continue;
		}
		else if(!strncmp(ed->classname, "spawner", 7))
		{
			G_FreeEdict(ed);
			continue;
		}
	}
}

edict_t *makeNodeEnt(vec3_t spot, int id, int style, int ignore = 0)
{
	edict_t *ed;

	ed = G_Spawn();
	VectorCopy(spot, ed->s.origin);

	ed->movetype = MOVETYPE_NOCLIP;
	ed->solid = SOLID_NOT;
	ed->classname = "nodedrawer";

	SetGenericEffectInfo(ed);

	ed->nextthink = 0;

	// I feel like I'm overlooking something here :/
	int counter = style;
	int count2 = 1;

	while(counter > 1)
	{
		count2++;
		counter>>=1;//??
	}

	switch(count2)
	{
		case 1:
			//normal point
			fxRunner.execContinualEffect("edit/editpoint", ed);
			break;
		case 3:
			fxRunner.execContinualEffect("edit/editflee", ed);
			break;
		case 14:
			fxRunner.execContinualEffect("edit/editpoint", ed);
			break;
		case 15:
			fxRunner.execContinualEffect("edit/editpoint", ed);
			break;
		case 16:
			fxRunner.execContinualEffect("edit/editfleegood", ed);
			break;
		case 17:
			fxRunner.execContinualEffect("edit/editfleebad", ed);
			break;
		default:
			fxRunner.execContinualEffect("edit/editpoint", ed);
			break;
	}

	if(ignore)
	{
		fxRunner.execContinualEffect("edit/ignore", ed);
	}

	return ed;

}

void drawMonkeyThink(edict_t *ed)
{
	ed->nextthink = level.time + .1;
	aiList->graphicData(level.time);
}

void StartNodeDrawMonkey(void)
{
	edict_t *ed = G_Spawn();

	ed->think = drawMonkeyThink;
	ed->nextthink = level.time + .1;
}

void regionTraceThink(edict_t *ed)
{
	if(level.sight_client)
	{
		aiList->ensureRegion(level.sight_client);
	}
	ed->nextthink = level.time + .1;
}

void  initRegionTrace(void)
{
	edict_t *ed = G_Spawn();

	ed->think = regionTraceThink;
	ed->nextthink = level.time + .1;
}

int walkingTrace(vec3_t start, vec3_t min, vec3_t max, vec3_t end, edict_t *ignore, int mask)
{
	float curFloorHeight;
	trace_t tr;
	vec3_t fwd, down;

	VectorCopy(start, down);
	down[2] -= 1024;

	gi.trace(start, min, max, down, ignore, mask, &tr);
	if(tr.startsolid || tr.allsolid)
	{
		return 0;
	}

	curFloorHeight = tr.endpos[2];

	VectorSubtract(end, start, fwd);
	fwd[2] = 0;
	float len = VectorNormalize(fwd);
	vec3_t curPos, curCheck;

	VectorCopy(start, curPos);
	curPos[2] = curFloorHeight - min[2];

	float distMoved = 0;

	while(distMoved < len)
	{
		VectorMA(curPos, 4.0, fwd, curCheck);

		gi.trace(curPos, min, max, curCheck, ignore, mask, &tr);

		if(tr.fraction < .99 || tr.startsolid || tr.allsolid)
		{	//uh oh - hit something.  See if the path is clear a bit above this - otherwise bad path
			float totalAdded = 0;

			while(totalAdded < 20)
			{
				curPos[2] += 4;
				curCheck[2] += 4;

				gi.trace(curPos, min, max, curCheck, ignore, mask, &tr);

				if(tr.fraction > .99 && !tr.startsolid && !tr.allsolid)
				{
					break;
				}

				totalAdded += 4;
			}
			if(totalAdded >= 20)
			{	//welp, couldn't make that connection
				return 0;
			}
		}
		else
		{
			vec3_t downVect;

			VectorCopy(curCheck, downVect);
			downVect[2] -= 40;

			gi.trace(curCheck, min, max, downVect, ignore, mask, &tr);

			if(tr.fraction > .5)
			{
				return 0;//too big of a drop off
			}
		}

		VectorCopy(curCheck, curPos);
		distMoved += 4;
	}

	return 1;
}











////////////////////////////////////////////////////////////////////////////////////////////////
//
//								CAIRegion
//
////////////////////////////////////////////////////////////////////////////////////////////////

void CAIRegion::save(FILE *fp)
{
	fwrite(&id, sizeof(id), 1, fp);
	fwrite(min, sizeof(min), 1, fp);
	fwrite(max, sizeof(max), 1, fp);
	fwrite(&targNode, sizeof(targNode), 1, fp);
	assert(targNode < 400);
}

void CAIRegion::saveExport(FILE *fp)
{
	fwrite(&id, sizeof(id), 1, fp);
	fwrite(min, sizeof(min), 1, fp);
	fwrite(max, sizeof(max), 1, fp);
	fwrite(&targNode, sizeof(targNode), 1, fp);
	fwrite(&blockSide, sizeof(blockSide), 1, fp);
	assert(targNode < 400);
}


void CAIRegion::load(FILE **fp)
{
	fread(&id, sizeof(id), 1, *fp);
	fread(min, sizeof(min), 1, *fp);
	fread(max, sizeof(max), 1, *fp);
	fread(&targNode, sizeof(targNode), 1, *fp);
	if(targNode > 400)
	{
		targNode = -1;
	}
}

#include "windows.h"

void CAIRegion::load(unsigned char **data)
{
	id = readInt(data);
	for(int i = 0; i < 3; i++)min[i] = readFloat(data);
	for(i = 0; i < 3; i++)max[i] = readFloat(data);
	targNode = readInt(data);
	blockSide = readInt(data);

	assert(targNode < 400);
	if(targNode < 1)
	{
		OutputDebugString("No node for this region!!!\n");
	}
}








void CAIFloorSquare::save(FILE *fp)
{
	for(int i = 0; i < 3; i++)
	{
		fwrite(&min[i], sizeof(float), 1, fp);
	}
	fwrite(&partial, sizeof(partial), 1, fp);
}

void CAIFloorSquare::load(FILE **fp)
{
	for(int i = 0; i < 3; i++)
	{
		fread(&min[i], sizeof(float), 1, *fp);
	}
	fread(&partial, sizeof(partial), 1, *fp);
}

void CAIFloorSquare::clearNear(void)
{
	for(int i = 0; i < NUM_NEAR_NODES; i++)
	{
		nearNodes[i] = -1;
		nearNodeDist[i] = 512*512;
	}
}

int CAIFloorSquare::setFirstFreeNode(int val)
{
	int i = 0;
	while(nearNodes[i] != -1 && nearNodes[i] != val)
	{
		i++;
	}

	if(i < NUM_NEAR_NODES)
	{
		nearNodes[i] = val;
		return i;
	}
	else
	{
		return -1;
	}
}

int CAIFloorSquare::hasNodes(void)
{
	if(nearNodes[0] == -1)
	{
		return 0;
	}
	return 1;
}

void CAIFloorSquare::saveExport(FILE **fp)
{
	for(int i = 0; i < 3; i++)
	{
		fwrite(&min[i], sizeof(float), 1, *fp);
		//signed char val = min[i] / 48;
		//fwrite(&val, 1, 1, *fp);
	}
	fwrite(&partial, sizeof(partial), 1, *fp);
	fwrite(&id, sizeof(id), 1, *fp);
	for(i = 0; i < NUM_NEAR_NODES; i++)
	{
		fwrite(&nearNodes[i], sizeof(nearNodes[0]), 1, *fp);
	}
	fwrite(&yMinus, sizeof(yMinus), 1, *fp);
	fwrite(&xPlus, sizeof(xPlus), 1, *fp);
	fwrite(&yPlus, sizeof(yPlus), 1, *fp);
	fwrite(&xMinus, sizeof(xMinus), 1, *fp);
	fwrite(&topZ, sizeof(topZ), 1, *fp);
}


void CAIFloorDataEdit::save(FILE *fp)
{
	fwrite(&regionsInUse, sizeof(regionsInUse), 1, fp);
	for(int i = 0; i < regionsInUse; i++)
	{
		floorSpaces[i].save(fp);
	}
}

void CAIFloorDataEdit::load(FILE **fp)
{
	fread(&regionsInUse, sizeof(regionsInUse), 1, *fp);
	for(int i = 0; i < regionsInUse; i++)
	{
		floorSpaces[i].load(fp);
	}
}

void CAIFloorDataEdit::ensureRegion(vec3_t spot)
{
	vec3_t spot2;
	int partial;
	for(int j = -3; j < 4; j++)
	{
		for(int k = -3; k < 4; k++)
		{
			int drawColor = 0;

			if(j == 0 && k == 0)
			{
				int asdf = 9;
			}

			VectorCopy(spot, spot2);
			spot2[0] += 48 * j;
			spot2[1] += 48 * k;

			trace_t tr;

			gi.trace(spot, vec3_origin, vec3_origin, spot2, 0, MASK_SOLID, &tr);

			if(fabs(spot2[0] - tr.endpos[0]) > 48)
			{
				continue;
			}
			if(fabs(spot2[1] - tr.endpos[1]) > 48)
			{
				continue;
			}

			VectorCopy(tr.endpos, spot2);

			vec3_t pmin, pmax;

			for(int i = 0; i < 2; i++)
			{
				pmin[i] = ((((int)spot2[i])+4096) / 48)*48 - 4096;
				pmax[i] = pmin[i] + 48;
			}
			vec3_t testend;
			VectorCopy(spot2, testend);
			testend[2] -= 256;
			
			gi.trace(spot2, vec3_origin,vec3_origin, testend, 0, MASK_SOLID, &tr);
			pmin[2] = tr.endpos[2];
			pmax[2] = pmin[2] + 16;

			int found = 0;
			for(i = 0; i < regionsInUse; i++)
			{
				vec3_t dif;
				VectorSubtract(floorSpaces[i].getMin(), pmin, dif);
				if(fabs(dif[2]) > 32)
				{
					continue;//rules are different for this
				}
				dif[2] = 0;
				if(VectorLength(dif) < 3)
				{
					found = 1;
					partial = floorSpaces[i].getPartial();
					//Com_Printf("Found %d squares\n", regionsInUse);
					break;
				}
			}
			if(!found)
			{
				pmax[2] = pmin[2] + 64;//? enemy height?

				vec3_t testmin, testmax;
				VectorCopy(pmin, testmin);
				testmin[0] += 8;
				testmin[1] += 8;
				testmin[2] += 16;
				VectorCopy(pmax, testmax);
				testmax[0] -= 8;
				testmax[1] -= 8;

				gi.trace(vec3_origin, testmin, testmax, vec3_origin, 0, MASK_SOLID, &tr);

				partial = 0;
				if(tr.fraction < 1.0 || tr.startsolid || tr.allsolid)
				{
					partial = 1;
				}

				//check for floor gaps
				vec3_t center;
				VectorCopy(testmin, center);
				center[0] += 16;
				center[1] += 16;
				for(int j = 0; j < 4; j++)
				{
					vec3_t testTop;
					trace_t tr;

					VectorCopy(center, testTop);

					testTop[0] += 16 * ((j>1)?-1:1);
					testTop[1] += 16 * ((j&1)?-1:1);

					vec3_t testBottom;

					VectorCopy(testTop, testBottom);
					testBottom[2] -= (25+16);//we start out 16 above floor, 24 is max step height - one more is no good

					gi.trace(testTop, vec3_origin, vec3_origin, testBottom, 0, MASK_SOLID, &tr);
					if(tr.fraction > .99 && !tr.startsolid && !tr.allsolid)
					{	// uh oh
						partial |= PARTIAL_FLOORGAP;
						break;
					}

				}


				if(regionsInUse < 15999)//ick
				{
					CAIRegion newSpace;
					VectorCopy(pmin, floorSpaces[regionsInUse].getMin());
					floorSpaces[regionsInUse].setPartial(partial);
					regionsInUse++;
					Com_Printf("%d regions in use now\n", regionsInUse);
				}
				else
				{
					Com_Printf("Out of regions!!!  Huge problem!\n");
				}
			}

			if(showboxes)
			{
				if((((int)(level.time*10)+i)%5)==0)
				{	//only draw these twice a second
					gi.WriteByte(svc_temp_entity);
					gi.WriteByte(TE_TESTBOX);
					gi.WritePosition(pmin);
					gi.WritePosition(pmax);
					if(partial & PARTIAL_FLOORGAP)
					{
						gi.WriteByte(0);
						gi.WriteByte(255);
						gi.WriteByte(0);
					}
					else if(partial & PARTIAL_FLOOR)
					{
						gi.WriteByte(255);
						gi.WriteByte(0);
						gi.WriteByte(0);
					}
					else
					{
						gi.WriteByte(0);
						gi.WriteByte(0);
						gi.WriteByte(255);
					}
					gi.multicast(level.sight_client->s.origin, MULTICAST_PHS);
				}
			}
		}
	}
}

int floorSpaceSort (void const *a, void const *b)
{
	CAIFloorSquare anum, bnum;

	anum = *(CAIFloorSquare *)a;
	bnum = *(CAIFloorSquare *)b;

	if(anum.getMin()[0] < bnum.getMin()[0])
	{
		return -1;
	}
	else if(anum.getMin()[0] > bnum.getMin()[0])
	{
		return 1;
	}
	else
	{
		if(anum.getMin()[1] < bnum.getMin()[1])
		{
			return -1;
		}
		else if(anum.getMin()[1] > bnum.getMin()[1])
		{
			return 1;
		}
		else
		{
			if(anum.getMin()[2] < bnum.getMin()[2])
			{
				return -1;
			}
			else if(anum.getMin()[2] > bnum.getMin()[2])
			{
				return 1;
			}
		}
	}

	return 0;
}



void CAIFloorDataEdit::saveExport(FILE **fp)
{
	qsort(floorSpaces, regionsInUse, sizeof(floorSpaces[0]), floorSpaceSort);

	int numX;
	int *numY;
	int **numZ;

	numX = 1;

	for(int i = 1; i < regionsInUse; i++)
	{
		if(floorSpaces[i].getMin()[0] != floorSpaces[i - 1].getMin()[0])
		{
			numX++;
		}
	}
	numY = new int[numX];
	numZ = new int*[numX];

	int countY = 1;
	int curX = 0;
	for(i = 1; i < regionsInUse; i++)
	{
		if(floorSpaces[i].getMin()[0] != floorSpaces[i - 1].getMin()[0])
		{
			numY[curX] = countY;
			numZ[curX] = new int[countY];
			countY = 1;
			curX++;
		}
		else if(floorSpaces[i].getMin()[1] != floorSpaces[i - 1].getMin()[1])
		{
			countY++;
		}
	}
	if(curX == numX - 1)
	{	//catch the  last bit
		numY[curX] = countY;
		numZ[curX] = new int[countY];
		countY = 1;
		curX++;
	}
	assert(curX == numX);

	curX = 0;
	int curY = 0;
	int countZ = 1;
	int pending;
	for(i = 1; i < regionsInUse; i++)
	{
		pending = 0;
		if(floorSpaces[i].getMin()[0] != floorSpaces[i - 1].getMin()[0])
		{
			numZ[curX][curY] = countZ;
			countZ = 1;

			curX++;
			curY = 0;
			countZ = 1;
		}
		else if(floorSpaces[i].getMin()[1] != floorSpaces[i - 1].getMin()[1])
		{
			numZ[curX][curY] = countZ;
			countZ = 1;
			curY++;
		}
		else
		{
			countZ++;//no choice but to be so
			pending = 1;
		}
	}
	//fill in the last straggler
	numZ[numX-1][numY[numX-1]-1] = countZ;
	//assert(curX == numX);

	int curFloor = 0;

	fwrite(&regionsInUse, sizeof(regionsInUse), 1, *fp);
	fwrite(&numX, sizeof(numX), 1, *fp);
	for(i = 0; i < numX; i++)
	{
		fwrite(&(floorSpaces[curFloor].getMin()[0]), sizeof(float), 1, *fp);
		fwrite(&numY[i], sizeof(float), 1, *fp);
		for(int j = 0; j < numY[i]; j++)
		{
			fwrite(&(floorSpaces[curFloor].getMin()[1]), sizeof(float), 1, *fp);
			fwrite(&numZ[i][j], sizeof(float), 1, *fp);
			for(int k = 0; k < numZ[i][j]; k++)
			{
				floorSpaces[curFloor].saveExport(fp);
				curFloor++;
			}
		}
	}
	assert(curFloor == regionsInUse);

	delete[] numY;
	delete[] numZ;
}

int CAIFloorDataEdit::getFloorForSpot(vec3_t spot)
{
	float bestHeight = -9999;
	int bestFloor = -1;

	for(int i = 0; i < regionsInUse; i++)
	{
		if(floorSpaces[i].getMin()[0] > spot[0])
		{
			continue;
		}
		if(floorSpaces[i].getMin()[1] > spot[1])
		{
			continue;
		}
		if(floorSpaces[i].getMin()[0] + 48 < spot[0])
		{
			continue;
		}
		if(floorSpaces[i].getMin()[1] + 48 < spot[1])
		{
			continue;
		}
		if(floorSpaces[i].getMin()[2] > spot[2])
		{
			continue;
		}
		if(floorSpaces[i].getMin()[2] < bestHeight)
		{
			continue;
		}

		bestFloor = i;
		bestHeight = floorSpaces[i].getMin()[2];
	}
	return bestFloor;
}

void CAIFloorDataEdit::AddNodesViaLine(vec3_t start, vec3_t end, int id1, int id2)
{
	int curFloor = getFloorForSpot(start);
	int endFloor = getFloorForSpot(end);
	int	differsOnlyByZ = 0;

	if(curFloor == -1)
	{
		return;
	}

	vec3_t startPos, startSide;

	VectorCopy(start, startPos);

	vec3_t difOuter;
	VectorSubtract(end, startPos, difOuter);

	startSide[1] = difOuter[0];
	startSide[0] = -difOuter[1];
	startSide[2] = 0;

	if(endFloor == -1)
	{
		return;
	}

	do
	{
		int nextLeft, nextRight;

		vec3_t center;
		VectorCopy(floorSpaces[curFloor].getMin(), center);
		center[0] += 24;
		center[1] += 24;

		vec3_t dist;
		int addedNode = floorSpaces[curFloor].setFirstFreeNode(id1);
		if(addedNode != -1)
		{
			VectorSubtract(start, center, dist);
			floorSpaces[curFloor].setNearDist(addedNode, VectorLength(dist));
		}

		addedNode = floorSpaces[curFloor].setFirstFreeNode(id2);
		if(addedNode != -1)
		{
			VectorSubtract(end, center, dist);
			floorSpaces[curFloor].setNearDist(addedNode, VectorLength(dist));
		}

		if(fabs(floorSpaces[curFloor].getMin()[0] - floorSpaces[endFloor].getMin()[0]) < 5)
		{//we're already in the proper x plane - just move in y
			if(floorSpaces[curFloor].getMin()[1] > floorSpaces[endFloor].getMin()[1])
			{
				curFloor = floorSpaces[curFloor].getYMinus();
			}
			else
			{
				curFloor = floorSpaces[curFloor].getYPlus();
			}
		}
		else if(fabs(floorSpaces[curFloor].getMin()[1] - floorSpaces[endFloor].getMin()[1]) < 5)
		{//we're already in the proper x plane - just move in y
			if(floorSpaces[curFloor].getMin()[0] > floorSpaces[endFloor].getMin()[0])
			{
				curFloor = floorSpaces[curFloor].getXMinus();
			}
			else
			{
				curFloor = floorSpaces[curFloor].getXPlus();
			}
		}
		else 
		{
			if(difOuter[0] > 0 && difOuter[1] > 0)
			{
				nextLeft = floorSpaces[curFloor].getYPlus();
				nextRight = floorSpaces[curFloor].getXPlus();
			}
			else if(difOuter[0] < 0 && difOuter[1] > 0)
			{
				nextLeft = floorSpaces[curFloor].getXMinus();
				nextRight = floorSpaces[curFloor].getYPlus();
			}
			else if(difOuter[0] > 0 && difOuter[1] < 0)
			{
				nextLeft = floorSpaces[curFloor].getXPlus();
				nextRight = floorSpaces[curFloor].getYMinus();
			}
			else if(difOuter[0] < 0 && difOuter[1] < 0)
			{
				nextLeft = floorSpaces[curFloor].getYMinus();
				nextRight = floorSpaces[curFloor].getXMinus();
			}

			vec3_t dif;
			VectorSubtract(center, startPos, dif);
			if(DotProduct(startSide, dif) > 0)
			{
				curFloor = nextRight;
			}
			else
			{
				curFloor = nextLeft;
			}
		}

		if(curFloor > -1)
		{
			if(fabs(floorSpaces[curFloor].getMin()[0] - floorSpaces[endFloor].getMin()[0]) < 5 &&
				fabs(floorSpaces[curFloor].getMin()[1] - floorSpaces[endFloor].getMin()[1]) < 5)
			{
				differsOnlyByZ = 1;
			}
		}

	}while(curFloor > -1 && curFloor != endFloor && !differsOnlyByZ);
}

int CAIFloorDataEdit::clearLineForFloor(int startID, int endFloor, int traceStyle)
{
	int curFloor = startID;
	int	differsOnlyByZ = 0;

	if(endFloor == -1)
	{
		return 0;
	}

	vec3_t startPos, startSide;

	VectorCopy(floorSpaces[startID].getMin(), startPos);

	vec3_t difOuter;
	VectorSubtract(floorSpaces[endFloor].getMin(), startPos, difOuter);

	startPos[0] += 24;
	startPos[1] += 24;
	//startPos[2] = 0;//???? WHat the HEll

	startSide[1] = difOuter[0];
	startSide[0] = -difOuter[1];
	startSide[2] = 0;

	do
	{
		int nextLeft, nextRight;

		if(fabs(floorSpaces[curFloor].getMin()[0] - floorSpaces[endFloor].getMin()[0]) < 5)
		{//we're already in the proper x plane - just move in y
			if(floorSpaces[curFloor].getMin()[1] > floorSpaces[endFloor].getMin()[1])
			{
				curFloor = floorSpaces[curFloor].getYMinus();
			}
			else
			{
				curFloor = floorSpaces[curFloor].getYPlus();
			}
		}
		else if(fabs(floorSpaces[curFloor].getMin()[1] - floorSpaces[endFloor].getMin()[1]) < 5)
		{//we're already in the proper x plane - just move in y
			if(floorSpaces[curFloor].getMin()[0] > floorSpaces[endFloor].getMin()[0])
			{
				curFloor = floorSpaces[curFloor].getXMinus();
			}
			else
			{
				curFloor = floorSpaces[curFloor].getXPlus();
			}
		}
		else 
		{
			if(difOuter[0] > 0 && difOuter[1] > 0)
			{
				nextLeft = floorSpaces[curFloor].getYPlus();
				nextRight = floorSpaces[curFloor].getXPlus();
			}
			else if(difOuter[0] < 0 && difOuter[1] > 0)
			{
				nextLeft = floorSpaces[curFloor].getXMinus();
				nextRight = floorSpaces[curFloor].getYPlus();
			}
			else if(difOuter[0] > 0 && difOuter[1] < 0)
			{
				nextLeft = floorSpaces[curFloor].getXPlus();
				nextRight = floorSpaces[curFloor].getYMinus();
			}
			else if(difOuter[0] < 0 && difOuter[1] < 0)
			{
				nextLeft = floorSpaces[curFloor].getYMinus();
				nextRight = floorSpaces[curFloor].getXMinus();
			}

			vec3_t center, dif;
			VectorCopy(floorSpaces[curFloor].getMin(), center);
			center[0] += 24;
			center[1] += 24;
			VectorSubtract(center, startPos, dif);

			dif[2] = 0;

			assert(nextRight > -2);
			assert(nextLeft > -2);

			if(DotProduct(startSide, dif) > 0)
			{
				curFloor = nextRight;
			}
			else
			{
				curFloor = nextLeft;
			}
		}

		//er
		if(curFloor != -1)
		{
			if(fabs(floorSpaces[curFloor].getMin()[0] - floorSpaces[endFloor].getMin()[0]) < 5 &&
				fabs(floorSpaces[curFloor].getMin()[1] - floorSpaces[endFloor].getMin()[1]) < 5)
			{
				differsOnlyByZ = 1;
			}

			if(floorSpaces[curFloor].getPartial())
			{
				differsOnlyByZ = 1;
			}
		}

	}while(curFloor > -1 && curFloor != endFloor && !differsOnlyByZ);

	if(curFloor == endFloor)
	{
		if(traceStyle == 0)
		{
			return 1;
		}
		vec3_t endPos;

		startPos[2] += 16;

		VectorCopy(floorSpaces[endFloor].getMin(), endPos);
		endPos[0] += 24;
		endPos[1] += 24;
		endPos[2] += 16;

		trace_t tr;

		vec3_t min = {-16, -16, -16};
		vec3_t max = {16, 16, 16};

		if(traceStyle == 2)
		{
			gi.trace(startPos, min, max, endPos, 0, MASK_MONSTERSOLID, &tr);
		}
		else
		{
			gi.trace(startPos, vec3_origin, vec3_origin, endPos, 0, MASK_MONSTERSOLID, &tr);
		}

		if(tr.fraction > .99 && !tr.startsolid && !tr.allsolid)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

void CAIFloorDataEdit::prepareForExport(void)
{
	qsort(floorSpaces, regionsInUse, sizeof(floorSpaces[0]), floorSpaceSort);

	for(int i = 0; i < regionsInUse; i++)
	{
		floorSpaces[i].setId(i);
		floorSpaces[i].setYMinus(-1);
		floorSpaces[i].setYPlus(-1);
		floorSpaces[i].setXMinus(-1);
		floorSpaces[i].setXPlus(-1);
	}

	for(i = 0; i < regionsInUse; i++)
	{
		for(int j = i + 1; j < regionsInUse; j++)
		{	//this could be much faster, as always

			float *one, *two;

			one = floorSpaces[i].getMin();
			two = floorSpaces[j].getMin();

			if(fabs(one[2] - two[2]) > 32)
			{
				continue;
			}

			if(fabs(one[0] - two[0]) < 1)
			{//share same xcoord
				if(fabs(one[1] - two[1]) < 50)
				{
					//fixme - trace right here to make certain that there really and truly is adjacency

					if(one[1] > two[1])
					{
						//assert(floorSpaces[i].getYMinus() == -1);
						//assert(floorSpaces[j].getYPlus() == -1);
						floorSpaces[i].setYMinus(floorSpaces[j].getId());
						floorSpaces[j].setYPlus(floorSpaces[i].getId());
					}
					else
					{
						//assert(floorSpaces[i].getYPlus() == -1);
						//assert(floorSpaces[j].getYMinus() == -1);
						floorSpaces[i].setYPlus(floorSpaces[j].getId());
						floorSpaces[j].setYMinus(floorSpaces[i].getId());
					}
				}
			}

			if(fabs(one[1] - two[1]) < 1)
			{//share same xcoord
				if(fabs(one[0] - two[0]) < 50)
				{
					//fixme - trace right here to make certain that there really and truly is adjacency

					if(one[0] > two[0])
					{
						//assert(floorSpaces[i].getXMinus() == -1);
						//assert(floorSpaces[j].getXPlus() == -1);
						floorSpaces[i].setXMinus(floorSpaces[j].getId());
						floorSpaces[j].setXPlus(floorSpaces[i].getId());
					}
					else
					{
						//assert(floorSpaces[i].getXPlus() == -1);
						//assert(floorSpaces[j].getXMinus() == -1);
						floorSpaces[i].setXPlus(floorSpaces[j].getId());
						floorSpaces[j].setXMinus(floorSpaces[i].getId());
					}
				}
			}


		}
	}

	OutputDebugString("Calculating node partial status and height\n");
	for(i = 0; i < regionsInUse; i++)
	{
		vec3_t max;
		VectorCopy(floorSpaces[i].getMin(), max);
		max[0] += 48;
		max[1] += 48;
		max[2] += 64;//? enemy height?

		vec3_t testmin, testmax;
		VectorCopy(floorSpaces[i].getMin(), testmin);
		testmin[0] += 8;
		testmin[1] += 8;
		testmin[2] += 16;
		VectorCopy(max, testmax);
		testmax[0] -= 8;
		testmax[1] -= 8;

		trace_t tr;
		gi.trace(vec3_origin, testmin, testmax, vec3_origin, 0, MASK_SOLID, &tr);

		if(tr.fraction < 1.0 || tr.startsolid || tr.allsolid)
		{
			floorSpaces[i].setPartial(PARTIAL_FLOOR);
		}
		else
		{
			floorSpaces[i].setPartial(0);
		}

		//check for floor gaps
		vec3_t center;
		VectorCopy(floorSpaces[i].getMin(), center);
		center[0] += 24;
		center[1] += 24;
		for(int j = 0; j < 4; j++)
		{
			vec3_t testTop;

			VectorCopy(center, testTop);

			testTop[0] += 16 * (j>1)?-1:1;
			testTop[1] += 16 * (j&1)?-1:1;

			vec3_t testBottom;

			VectorCopy(testTop, testBottom);
			testBottom[2] -= (25+16);//we start 16 above, 24 is step height

			gi.trace(testTop, vec3_origin, vec3_origin, testBottom, 0, MASK_SOLID, &tr);
			if(tr.fraction > .99 && !tr.startsolid && !tr.allsolid)
			{	// uh oh
				floorSpaces[i].setPartialFlag(PARTIAL_FLOORGAP);
				break;
			}

		}

		vec3_t minV = {-24, -24, 0};
		vec3_t maxV = {24, 24, 1};
		vec3_t testPoint;
		vec3_t testTop;
		
		VectorCopy(floorSpaces[i].getMin(), testPoint);
		testPoint[0] += 24;
		testPoint[1] += 24;
		testPoint[2] += 24;
		VectorCopy(testPoint, testTop);
		testTop[2] += 2048;

		gi.trace(testPoint, minV, maxV, testTop, 0, MASK_SOLID, &tr);

		if(tr.startsolid || tr.allsolid)
		{
			floorSpaces[i].setTopZ(floorSpaces[i].getMin()[2]);
		}
		else
		{
			floorSpaces[i].setTopZ(tr.endpos[2]);
		}
	}

	for(i = 0; i < regionsInUse; i++)
	{
		floorSpaces[i].clearNear();
	}

	//now find out who owns each node
	for(i = 0; i < regionsInUse; i++)
	{
		floorSpaces[i].setNode(-1);
	}

	for(i = 1; i < aiList->getNumElems(); i++)
	{
		aiList->getNode(i)->setMyFloor(getFloorForSpot(aiList->getNode(i)->getPos()));
	}

	OutputDebugString("Connecting floors to nodes\n");
	for(i = 0; i < regionsInUse; i++)
	{
		if(!(i%100))
		{
			OutputDebugString(va("Connected %d floors\n", i));
		}
		int traceStyle = 2;
		do
		{
			for(int j = 1; j < aiList->getNumElems(); j++)
			{
				if(clearLineForFloor(i, aiList->getNode(j)->getMyFloor(), traceStyle))
				{
					vec3_t dist;
					int addedNode = floorSpaces[i].setFirstFreeNode(j);
					if(addedNode != -1)
					{
						VectorSubtract(floorSpaces[i].getMin(), aiList->getNode(j)->getPos(), dist);
						floorSpaces[i].setNearDist(addedNode, VectorLength(dist));
					}
				}
			}
			traceStyle--;//get progressively more lax with the criteria for nearness
		}while(floorSpaces[i].getNearNode(0) == -1 && traceStyle > -1);

		for(int j = 1; j < NUM_NEAR_NODES; j++)
		{
			//make certain that the first one is the best one
			if(floorSpaces[i].getNearNode(j) != -1)
			{
				if(floorSpaces[i].getNearDist(j) < floorSpaces[i].getNearDist(0))
				{
					float tempDist = floorSpaces[i].getNearDist(j);
					short tempNode = floorSpaces[i].getNearNode(j);
					floorSpaces[i].setNearDist(j, floorSpaces[i].getNearDist(0));
					floorSpaces[i].setNearNode(j, floorSpaces[i].getNearNode(0));
					floorSpaces[i].setNearDist(0, tempDist);
					floorSpaces[i].setNearNode(0, tempNode);
				}
			}
		}
	}

	vec3_t minSize = {-16, -16, 0};
	vec3_t maxSize = {16, 16, 24};

	OutputDebugString("Adding floor-node connections from node-node connections\n");
	for(i = 0; i < aiList->getNumElems(); i++)
	{
		CAINode *node = aiList->getNode(i);

		if(node)
		{
			for(int j = 1; j <= node->getNumLinks(); j++)
			{
				CAIConnect *connect = node->getLinkNum(j);

				if(connect)
				{
					CAINode *otherNode = aiList->getNode(connect->getLinkId());

					if(otherNode)
					{	//run the length of the path here, adding both nodes to all grid squares under them
						vec3_t one, two;
						VectorCopy(node->getPos(), one);
						VectorCopy(otherNode->getPos(), two);
						AddNodesViaLine(one, two, node->getId(), otherNode->getId());
						one[0] -= 16;
						two[0] -= 16;
						one[1] -= 16;
						two[1] -= 16;
						AddNodesViaLine(one, two, node->getId(), otherNode->getId());
						one[0] += 32;
						two[0] += 32;
						AddNodesViaLine(one, two, node->getId(), otherNode->getId());
						one[1] += 32;
						two[1] += 32;
						AddNodesViaLine(one, two, node->getId(), otherNode->getId());
						one[0] -= 32;
						two[0] -= 32;
						AddNodesViaLine(one, two, node->getId(), otherNode->getId());
					}
				}
			}
		}
	}

	OutputDebugString("Ensuring neighbors have nodes\n");
	int recheck = 0;
	do
	{
		OutputDebugString("Neighbor pass\n");
		recheck = 0;

		for(i = 0; i < regionsInUse; i++)
		{
			CAIFloorSquare *curSquare;

			curSquare = &floorSpaces[i];

			if(!curSquare->hasNodes())
			{
				continue;
			}

			if(curSquare->getXPlus() > -1 && (!floorSpaces[curSquare->getXPlus()].hasNodes()))
			{
				floorSpaces[curSquare->getXPlus()].setFirstFreeNode(curSquare->getNearNode(0));
				recheck = 1;
			}
			if(curSquare->getXMinus() > -1 && (!floorSpaces[curSquare->getXMinus()].hasNodes()))
			{
				floorSpaces[curSquare->getXMinus()].setFirstFreeNode(curSquare->getNearNode(0));
				recheck = 1;
			}
			if(curSquare->getYPlus() > -1 && (!floorSpaces[curSquare->getYPlus()].hasNodes()))
			{
				floorSpaces[curSquare->getYPlus()].setFirstFreeNode(curSquare->getNearNode(0));
				recheck = 1;
			}
			if(curSquare->getYMinus() > -1 && (!floorSpaces[curSquare->getYMinus()].hasNodes()))
			{
				floorSpaces[curSquare->getYMinus()].setFirstFreeNode(curSquare->getNearNode(0));
				recheck = 1;
			}
		}
	}while(recheck);

	for(i = 0; i < regionsInUse; i++)
	{
		CAIFloorSquare *floor = &floorSpaces[i];

		if(!floor->getPartial())
		{
			continue;
		}

		if(floor->getYMinus() != -1)
		{
			CAIFloorSquare *otherfloor = &floorSpaces[floor->getYMinus()];

			if(otherfloor->getPartial())
			{//see if their boundary is okay
				vec3_t min, max;

				VectorCopy(floor->getMin(), min);
				VectorCopy(min, max);
				min[0] += 8;
				min[1] += 8;
				max[0] += 48 - 8;
				max[1] += 48 - 8;

				//now the specific bit
				min[1] -= 24;
				max[1] -= 24;

				trace_t tr;
				gi.trace(vec3_origin, min, max, vec3_origin, 0, MASK_SOLID, &tr);

				if(tr.fraction < 1.0 || tr.startsolid || tr.allsolid)
				{
				}
				else
				{
					floor->setPartialFlag(PARTIAL_YMINUS);
					otherfloor->setPartialFlag(PARTIAL_YPLUS);
				}


			}
		}

		if(floor->getYPlus() != -1)
		{
			CAIFloorSquare *otherfloor = &floorSpaces[floor->getYPlus()];

			if(otherfloor->getPartial())
			{//see if their boundary is okay
				vec3_t min, max;

				VectorCopy(floor->getMin(), min);
				VectorCopy(min, max);
				min[0] += 8;
				min[1] += 8;
				max[0] += 48 - 8;
				max[1] += 48 - 8;

				//now the specific bit
				min[1] += 24;
				max[1] += 24;

				trace_t tr;
				gi.trace(vec3_origin, min, max, vec3_origin, 0, MASK_SOLID, &tr);

				if(tr.fraction < 1.0 || tr.startsolid || tr.allsolid)
				{
				}
				else
				{
					floor->setPartialFlag(PARTIAL_YPLUS);
					otherfloor->setPartialFlag(PARTIAL_YMINUS);
				}


			}
		}

		if(floor->getXMinus() != -1)
		{
			CAIFloorSquare *otherfloor = &floorSpaces[floor->getXMinus()];

			if(otherfloor->getPartial())
			{//see if their boundary is okay
				vec3_t min, max;

				VectorCopy(floor->getMin(), min);
				VectorCopy(min, max);
				min[0] += 8;
				min[1] += 8;
				max[0] += 48 - 8;
				max[1] += 48 - 8;

				//now the specific bit
				min[0] -= 24;
				max[0] -= 24;

				trace_t tr;
				gi.trace(vec3_origin, min, max, vec3_origin, 0, MASK_SOLID, &tr);

				if(tr.fraction < 1.0 || tr.startsolid || tr.allsolid)
				{
				}
				else
				{
					floor->setPartialFlag(PARTIAL_XMINUS);
					otherfloor->setPartialFlag(PARTIAL_XPLUS);
				}


			}
		}

		if(floor->getXPlus() != -1)
		{
			CAIFloorSquare *otherfloor = &floorSpaces[floor->getXPlus()];

			if(otherfloor->getPartial())
			{//see if their boundary is okay
				vec3_t min, max;

				VectorCopy(floor->getMin(), min);
				VectorCopy(min, max);
				min[0] += 8;
				min[1] += 8;
				max[0] += 48 - 8;
				max[1] += 48 - 8;

				//now the specific bit
				min[0] += 24;
				max[0] += 24;

				trace_t tr;
				gi.trace(vec3_origin, min, max, vec3_origin, 0, MASK_SOLID, &tr);

				if(tr.fraction < 1.0 || tr.startsolid || tr.allsolid)
				{
				}
				else
				{
					floor->setPartialFlag(PARTIAL_XPLUS);
					otherfloor->setPartialFlag(PARTIAL_XMINUS);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//								CAIConnect
//
////////////////////////////////////////////////////////////////////////////////////////////////

void CAIConnect::save(FILE **fp)
{
	fwrite(&linkId, sizeof(linkId), 1, *fp);
	fwrite(&dist, sizeof(dist), 1, *fp);
	fwrite(&type, sizeof(type), 1, *fp);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//								CAINode
//
////////////////////////////////////////////////////////////////////////////////////////////////

void CAINode::clearLinks(void)
{
	list<CAIConnect>::iterator it;

	for(it = links.begin(); it != links.end(); it++)
	{
		(*it).setValid(1);
	}
}

void CAINode::addLink(int newLink, int isCombatPoint, connectType cType)
{
	list<CAIConnect>::iterator it;
	list<CAIConnect> &addList = (isCombatPoint) ? combatPoints:links;

	for(it = addList.begin(); it != addList.end(); it++)
	{
		if((*it).getLinkId() == newLink)
		{	//already in the list - no need to add
			return;
		}
	}

	CAINode *nextNode = (isCombatPoint) ? aiList->getCombatNode(newLink):aiList->getNode(newLink);
	assert(nextNode);
	vec3_t distV;
	VectorSubtract(pos, nextNode->pos, distV);
	float d = VectorLength(distV);

	CAIConnect newConnect(newLink, d, 0, cType);	

	addList.insert(it, newConnect);
}

void CAINode::destroyLink(int deadId)
{
	list<CAIConnect>::iterator it;

	for(it = links.begin(); it != links.end(); it++)
	{
		if((*it).getLinkId() == deadId)
		{
			if((*it).getType() != LINK_NORMAL)
			{	// don't destroy special link types EVER.
				return;
			}
			links.erase(it);
			return;
		}
	}
}

void CAINode::save(FILE *fp)
{	// if this gets changed, AI_NODE_VERSION needs to be adjusted
	int oldOutdatedVar = 0;

	fwrite(pos, sizeof(pos), 1, fp);
	fwrite(&radius, sizeof(radius), 1, fp);
	fwrite(&pointType, sizeof(pointType), 1, fp);
	fwrite(&oldOutdatedVar, sizeof(oldOutdatedVar), 1, fp);

	fwrite(&ignoreForSpawning, sizeof(ignoreForSpawning), 1, fp);	

}

void CAINode::load(FILE **fp)
{	// if this gets changed, AI_NODE_VERSION needs to be adjusted

	int oldOutdatedVar;

	fread(pos, sizeof(pos), 1, *fp);
	fread(&radius, sizeof(radius), 1, *fp);
	fread(&pointType, sizeof(pointType), 1, *fp);
	fread(&oldOutdatedVar, sizeof(oldOutdatedVar), 1, *fp);

	if(loadVersion < 9)
	{	//sigh - here we go again
		ignoreForSpawning = 0;
	}
	else
	{
		fread(&ignoreForSpawning, sizeof(ignoreForSpawning), 1, *fp);
	}
}

void CAINode::savePaths(FILE **fp)
{
	int numLinks;
	list<CAIConnect>::iterator it;

	fwrite(&pointType, sizeof(pointType), 1, *fp);
	fwrite(&id, sizeof(id), 1, *fp);
	fwrite(pos, sizeof(pos), 1, *fp);

	if(!(pointType & EPS_LINKMAIN))
	{	// only main link nodes have any of this...
		return;
	}

	numLinks = links.size();
	fwrite(&numLinks, sizeof(numLinks), 1, *fp);

	for(it = links.begin(); it != links.end(); it++)
	{
		(*it).save(fp);
	}
	fwrite(paths, sizeof(paths[0]) * aiList->getNumElems(), 1, *fp);
	fwrite(pathRanks, sizeof(pathRanks[0]) * aiList->getNumElems(), 1, *fp);

	numLinks = combatPoints.size();
	fwrite(&numLinks, sizeof(numLinks), 1, *fp);
	for(it = combatPoints.begin(); it != combatPoints.end(); it++)
	{
		(*it).save(fp);
	}

	fwrite(&radius, sizeof(radius), 1, *fp);
	fwrite(&flee1, sizeof(flee1), 1, *fp);
	fwrite(&flee2, sizeof(flee2), 1, *fp);

	fwrite(&ignoreForSpawning, sizeof(ignoreForSpawning), 1, *fp);

	fwrite(&minHeight, sizeof(minHeight), 1, *fp);
	fwrite(&maxHeight, sizeof(maxHeight), 1, *fp);
}

void CAINode::drawConnections(void)
{
	list<CAIConnect>::iterator curDraw;

	if(pointType & EPS_LINKMAIN)
	{
		for(curDraw = links.begin(); curDraw != links.end(); curDraw++)
		{
			CAINode			*other = aiList->getNode((*curDraw).getLinkId());
			paletteRGBA_t	col = {250, 0, 0, 250};

			if(!(*curDraw).getLinkId())
			{	// node one is crap
				continue;
			}
			if(!other)
			{	//yeah
				continue;
			}

			if(other)
			{
				FX_MakeLine(pos, other->pos, col, 6, MULTICAST_PVS);
			}
		}

		for(curDraw = combatPoints.begin(); curDraw != combatPoints.end(); curDraw++)
		{
			CAINode			*other = aiList->getCombatNode((*curDraw).getLinkId());
			paletteRGBA_t	col = {0, 250, 0, 250};

			if(!(*curDraw).getLinkId())
			{	// node one is crap
				continue;
			}
			if(!other)
			{	//yeah
				continue;
			}

			if(other)
			{
				FX_MakeLine(pos, other->pos, col, 6, MULTICAST_PVS);
			}
		}
	}
}

void CAINode::initPathData(int num)
{	// yeah.  Prolly should dealloc this somewhere, but I'm lazy
	paths = new byte[num];
	pathRanks = new short[num];
	memset(paths, 0, sizeof(byte) * num);
	memset(pathRanks, 0, sizeof(short) * num);

}

int CAINode::isConnectedTo(int linkId)
{
	int val = 1;
	list<CAIConnect>::iterator it;

	for(it = links.begin(); it != links.end(); it++)
	{
		if((*it).getLinkId() == linkId)
		{	//wow!  connected!
			return val;
		}
		val++;
	}
	return 0;
}

void CAINode::calcPath(void)
{
	// I think this implementation is a little off - the last check is probably not quite right :/

	int curRank = 0;

	// init everything
	for(int i = 0; i < aiList->getNumElems(); i++)
	{
		paths[i] = 0;
		pathRanks[i] = 16000;
	}

	if(links.size() == 0)
	{	// can't get here, because hey, I can't get ANYWHERE
		for(int i = 0; i < aiList->getNumElems(); i++)
		{
			paths[i] = 0;
			pathRanks[i] = 16000;//fixme - if we have more than 16000 nodes, this won't work
		}
		flee1 = 0;
		flee2 = 0;
		degenNodes++;
		return;
	}

	if(pointType & EPS_FLEE)
	{	//um, if I'm a flee point, I just run to myself, yeah?
		flee1 = id;
		flee2 = id;
	}
	else
	{	// as we come across flee nodes, since we're guaranteed that we get points in near to far order,
		// we should just grab the first two...
		flee1 = 0;
		flee2 = 0;
	}

	int numAdded = 1;

	checked = 1;//duh

	list<CAIConnect>	pathList;
	list<CAIConnect>::iterator it;

	list<CAIConnect>::iterator test;

	//going to me should be via route me
	paths[id] = 0;
	pathRanks[id] = curRank;

	curRank++;

	//initialize the path list from the first entries - but still check 'em
	for(it = links.begin(); it != links.end(); it++)
	{
		CAINode *nextNode = aiList->getNode((*it).getLinkId());
		assert(nextNode);
		nextNode->checked = 1;

		CAIConnect edge((*it).getLinkId(), (*it).getDist(), (*it).getLinkId());

		pathList.insert(pathList.end(), edge);

		numAdded++;
	}

	float	minDist;

	// this is a poorly controlled loop :/
	do
	{
		// init everything for the next iteration
		minDist = 999999;
		test = pathList.end();

		// search through the list to find the path that has traveled the least distance
		for(it = pathList.begin(); it != pathList.end(); it++)
		{
			if((*it).getValid() == 0)
			{
				continue;
			}
			if((*it).getDist() < minDist)
			{	// found a shorter one!  Woo hoo!  This will cover adjacent nodes, I think
				test = it;
				minDist = (*test).getDist();
			}
		}

		if(test == pathList.end())
		{	// looks like there's no more connections - this is a problem...  ugly way to handle exiting :/
			break;
		}

		(*test).setValid(0);

		if(aiList->getNode((*test).getLinkId())->getPointType() & EPS_FLEE)
		{	// well, that should do it, I believe
			if(!flee1)
			{
				flee1 = (*test).getLinkId();
			}
			else if(!flee2)
			{
				flee2 = (*test).getLinkId();
			}
		}
		else if(aiList->getNode((*test).getLinkId())->getPointType() & EPS_FLEEFRIENDLY)
		{
			if(!flee1)
			{
				flee1 = (*test).getLinkId();
			}
		}
		else if(aiList->getNode((*test).getLinkId())->getPointType() & EPS_FLEEENEMY)
		{
			if(!flee2)
			{
				flee2 = (*test).getLinkId();
			}
		}

		//check this node out and figure out what its head is...
		CAINode *testNode = aiList->getNode((*test).getLinkId());
		assert(testNode);

		paths[testNode->getId()] = isConnectedTo((*test).getFirst());
		pathRanks[testNode->getId()] = curRank;
		curRank++;

		assert(paths[testNode->getId()] <= links.size());

		for(it = testNode->links.begin(); it != testNode->links.end(); it++)
		{
			// add all the nodes connected to this node

			CAINode *otherNode = aiList->getNode((*it).getLinkId());
			assert(otherNode);

			if(otherNode->checked)
			{	//this was already snagged by a shorter path
				continue;
			}

			float newDist = (*test).getDist() + (*it).getDist();
			CAIConnect edge((*it).getLinkId(), newDist, (*test).getFirst());
			
			pathList.insert(pathList.end(), edge);

			numAdded++;

			otherNode->checked = 1;
		}

		pathList.erase(test);
	}while(1 == 1);

	pathList.clear();
}

void CAINode::calcRadius(void)
{
	float	curAng;
	vec3_t	traceEnd;
	float	minDist = 1.0;
	vec3_t	curDir;

	for(curAng = 0; curAng < M_PI * 2.0; curAng += (M_PI * .1))//20 steps
	{
		curDir[0] = cos(curAng);
		curDir[1] = sin(curAng);
		curDir[2] = 0;//this does seem to ignore elevation largely, but oh well...

		VectorScale(curDir, 1024, traceEnd);//perty far
		VectorAdd(traceEnd, pos, traceEnd);

		trace_t tr;

		float oldFrac;

		gi.trace(pos, 0, 0, traceEnd, 0, CONTENTS_SOLID, &tr);
		
		oldFrac = tr.fraction;

		for(float curCheck = oldFrac * 1024; curCheck > 0; curCheck -= 32)
		{
			vec3_t curTest;
			vec3_t down;

			VectorMA(pos, curCheck, curDir, curTest);
			VectorCopy(curTest, down);
			down[2] -= (32+24);//ick

			gi.trace(curTest, 0, 0, down, 0, CONTENTS_SOLID, &tr);

			if(tr.fraction == 1.0)
			{
				oldFrac = curCheck/1024.0;
			}
		}

		if(oldFrac < minDist)
		{
			minDist = oldFrac;
		}
	}

	radius = minDist * 1024;
}

#define VISIBLE_INCREMENT 64
#define MAX_VIS_DIST	  1024

float CAINode::getPathDist(int destNode)
{
	if(destNode == id)
	{
		return 0;//done
	}

	if(paths[destNode] == 0)
	{
		return -1;
	}

	CAINode *other = aiList->getNode(paths[destNode]);

	vec3_t myDist;

	VectorSubtract(other->pos, pos, myDist);
	
	float further = other->getPathDist(destNode);

	if(further == -1)
	{	// ran into a problem!!!
		return -1;
	}
	else
	{
		return VectorLength(myDist) + further;
	}
}
void CAINode::destroyLinks(void)
{
	links.clear();
	combatPoints.clear();
}

CAIConnect *CAINode::getLinkNum(int num)
{
	list<CAIConnect>::iterator it;

	int counter = 1;

	for(it = links.begin(); it != links.end(); it++)
	{
		if(counter == num)
		{
			return &(*it);
		}
		counter++;
	}

	return 0;
}

int CAINode::getPathToPoint(int p)
{
	assert(paths);
	int linkNum = paths[p];

	if(linkNum == 0)
	{
		return 0;
	}

	CAIConnect *myConnect = getLinkNum(linkNum);

	if(myConnect)
	{
		return myConnect->getLinkId();
	}

	return 0;
}
void CAINode::display(void)
{
	list<int>::iterator it;

	for(it = connectedNodes.begin(); it != connectedNodes.end(); it++)
	{
		fxRunner.exec("canthear", aiList->getNode(*it)->getPos());

		paletteRGBA_t	col = {0, 250, 125, 250};
	}

	list<CAIConnect>::iterator it2;

	int i = 0;
	for(it2 = links.begin(); it2 != links.end(); it2++)
	{
		paletteRGBA_t	col = {250, 0, 0, 250};
		FX_MakeLine(pos, aiList->getNode((*it2).getLinkId())->getPos(), col, 4, MULTICAST_PVS);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//								CAIListData
//
////////////////////////////////////////////////////////////////////////////////////////////////

CAINode *CAIListData::getNode(int num)
{
	return &nodes[num];
}

CAINode *CAIListData::getCombatNode(int num)
{
	return &combatPoints[num];
}

void CAIListData::validateNodeLinks(void)
{
	vector<CAINode>::iterator first;
	vector<CAINode>::iterator second;


	int selfreference = 0;
	int	onewaylinks = 0;
	int	loopingPaths = 0;
	float	numChecks = 0;
	float	pathChecks = 0;

	for(first = nodes.begin(); first != nodes.end(); first++)
	{
		if(first == nodes.begin())
		{	// this one is insignficant
			first++;
		}
		for(second = first; second != nodes.end(); second++)
		{
			if(first == second)
			{
				continue;
			}

			int id1 = (*first).getId();
			int id2 = (*second).getId();

			int path1 = (*first).getPathToPoint(id2);
			int path2 = (*second).getPathToPoint(id1);

			if(!path1 || !path2)
			{
				failedConnects++;

				if(!path1 && !path2)
				{	// well, enough of that
					continue;
				}
				else
				{
					onewaylinks++;
				}
			}

			for(int i = 0; i < nodes.size(); i++)
			{
				if((i == id1) || (i == id2))
				{	// meaningless tests...
					continue;
				}

				int test1 = (*first).getPathToPoint(i);
				int test2 = (*second).getPathToPoint(i);

				if((test1 == id2) && (test2 == id1))
				{	// circular - fukd
					selfreference++;
				}
			}

			// see if we can get from one to two...
			if(id1)
			{
				numChecks++;
				clearChecks();

				int curNode = id1;
				while((curNode != id2) && curNode)
				{
					//map<int, CAINode>::iterator check = nodes.find(curNode);
					vector<CAINode>::iterator check = &nodes[curNode];
					if(check != nodes.end())
					{	
						if(!(*check).getChecked())
						{
							pathChecks++;
							curNode = (*check).getPathToPoint(id2);
							(*check).setChecked(1);
						}
						else
						{	// we've looped.  Damn it.
							curNode = 0;
						}
					}
					else
					{	//huh?  Crap.
						curNode = 0;
					}
				}

				if(!curNode)
				{
					loopingPaths++;
				}
			}

			// also see if we can get from two to one
			if(id2)
			{
				numChecks++;
				clearChecks();

				int curNode = id2;
				while((curNode != id1) && curNode)
				{
					vector<CAINode>::iterator check = &nodes[curNode];
					if(check != nodes.end())
					{	
						if(!(*check).getChecked())
						{
							pathChecks++;
							curNode = (*check).getPathToPoint(id1);
							(*check).setChecked(1);
						}
						else
						{	// we've looped.  Damn it.
							curNode = 0;
						}
					}
					else
					{	//huh?  Crap.
						curNode = 0;
					}
				}

				if(!curNode)
				{
					loopingPaths++;
				}
			}

		}
	}
	Com_Printf("%d nodes pointing towards each other (out of %d connections)\n", selfreference, nodes.size() * nodes.size() * nodes.size());
	Com_Printf("%d nodes link up in only one direction\n", onewaylinks);
	Com_Printf("%d Nodes were unable to find each other!\n", failedConnects);
	Com_Printf("%d Nodes that are completely unconnected\n", degenNodes);
	Com_Printf("%d Invalid looping paths\n", loopingPaths);
	Com_Printf("%f Average Nodes from point to point\n", pathChecks/numChecks);
	Com_Printf("%d Regions\n", regions.size());
}

void CAIListData::graphicData(float val)
{
	int curDraw = ((int)(val*10))%30;

	while(curDraw < nodes.size())
	{
		nodes[curDraw].drawConnections();

		curDraw += 30;
	}

	curDraw = ((int)(val*10))%30;

	while(curDraw < combatPoints.size())
	{
		combatPoints[curDraw].drawConnections();

		curDraw += 30;
	}

}

void CAIListData::addPoint(edict_t *ent, char *cmd, int type, float *altCenter)
{
	CAINode newPoint;

	if(nodes.size() == 0)
	{	//the initial node must be empty for this to work
		newPoint.set(vec3_origin, 0);
		newPoint.setPointType(EPS_NORMAL);
		nodes.push_back(newPoint);
		StartNodeDrawMonkey();
	}

	vec3_t spot;

	if(altCenter)
	{
		VectorCopy(altCenter, spot);
	}
	else
	{
		VectorCopy(ent->s.origin, spot);
	}

	newPoint.set(spot, 0);
	newPoint.setPointType(type);

	newPoint.setMarker(makeNodeEnt(spot, 0, type));

	if(type & EPS_LINKMAIN)
	{
		nodes.push_back(newPoint);
	}
	else
	{
		combatPoints.push_back(newPoint);
	}
}

void CAIListData::connect(CAINode &one, CAINode &two, connectType newType)
{
	one.addLink(two.getId(), 0, newType);
	two.addLink(one.getId(), 0, newType);
}

void CAIListData::eliminateRedundantConnections(void)
{
	vector<CAINode>::iterator out1;
	vector<CAINode>::iterator out2;
	vector<CAINode>::iterator in1;

	for(out1 = nodes.begin(); out1 != nodes.end(); out1++)
	{
		for(out2 = nodes.begin(); out2 != nodes.end(); out2++)
		{
			if(out1 == out2)
			{
				continue;
			}

			if(!(*out1).isConnectedTo((*out2).getId()))
			{
				continue;
			}

			for(in1 = nodes.begin(); in1 != nodes.end(); in1++)
			{
				if(in1 == out2)
				{
					continue;
				}
				if(!(*out1).isConnectedTo((*in1).getId()))
				{
					continue;
				}

				vec3_t dir1, dir2;
				float len1, len2;

				//ignore z's for this check - fixme?

				VectorSubtract((*out1).getPos(), (*in1).getPos(), dir1);
				dir1[2] = 0;
				len1 = VectorNormalize(dir1);
				VectorSubtract((*out2).getPos(), (*in1).getPos(), dir2);
				dir2[2] = 0;
				len2 = VectorNormalize(dir2);

				if(DotProduct(dir1, dir2) < -NODE_COLINEAR)
				{	//in is between out1 and out2
					(*out1).destroyLink((*out2).getId());
					(*out2).destroyLink((*out1).getId());
					
				}
				else if(DotProduct(dir1, dir2) > NODE_COLINEAR)
				{
					if(len1 > len2)
					{	// out2 is between out1 and in1
						(*out1).destroyLink((*in1).getId());
						(*in1).destroyLink((*out1).getId());
					}
					else
					{	// out1 is between out2 and in1
						(*out2).destroyLink((*in1).getId());
						(*in1).destroyLink((*out2).getId());
					}
				}

			}
		}
	}
}


void CAIListData::connectNode(CAINode &start, int combat)
{
	vector<CAINode>::iterator end;
	vector<CAINode>::iterator testStart;
	vector<CAINode>::iterator testEnd;

	vec3_t testmin = {-16, -16,  0};//zcheck?
	vec3_t testmax = { 16,  16,  24};

	vec3_t combatmin = {-2, -2, -2};//zcheck?
	vec3_t combatmax = { 2,  2,  2};

	int ignoreFloorTest;
	float maxHeightMod;
	int stairs;
	int ladders;
	int jump;

	if(start.getPos()[0] > 9000)
	{	// no links for you!  This guy is dead and useless
		return;
	}

	if(combat)
	{
		testStart = combatPoints.begin();
		testEnd = combatPoints.end();
	}
	else
	{
		testStart = nodes.begin();
		testEnd = nodes.end();
	}

	if(!(start.getPointType() & EPS_LINKMAIN))
	{	// these are linked to - they don't possess links themselves
		return;
	}

	for(end = testStart; end != testEnd; end++)
	{
		if((!combat)&&(start.getId() == (*end).getId()))
		{	//ignore the silly case
			continue;
		}

		if((*end).getPos()[0] > 9000)
		{	// no links for you!  This guy is dead and useless
			continue;
		}


		ignoreFloorTest = 0;
		maxHeightMod = 1.0;
		stairs = 0;
		ladders = 0;
		jump = 0;

		vec3_t distV;
		VectorSubtract(start.getPos(), (*end).getPos(), distV);

		if(combat)
		{
			if(VectorLength(distV) > MAX_COMBATPOINT_DIST)
			{
				continue;
			}
		}
		else
		{
			if(VectorLength(distV) > MAX_LINK_DIST)
			{
				continue;
			}
		}

		vec3_t testV;
		VectorCopy(distV, testV);
		VectorNormalize(testV);
		//we need to test the z component of the testV to make certain that these vectors don't have too steep a slope

		vec3_t st, e;
		VectorCopy(start.getPos(), st);
		VectorCopy((*end).getPos(), e);

		if(fabs(testV[2]) > MAX_PATH_SLOPE)
		{
			continue;
		}

		trace_t tr;

		if(combat)
		{
			gi.trace(st, combatmin, combatmax, e, NULL, MASK_SOLID, &tr);
		}
		else
		{
			gi.trace(st, testmin, testmax, e, NULL, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER|CONTENTS_SHOT|CONTENTS_WINDOW, &tr);
		}

		if(tr.fraction < .99)
		{
			continue;
		}
		if(tr.startsolid)
		{
			continue;
		}

		if((!ignoreFloorTest)&&(!isPathLevel((*end).getPos(), start.getPos(), maxHeightMod, 0)))
		{
			continue;
		}

		// clear path...
		
		if(stairs)
		{
			//signs on these?
			start.addLink((*end).getId(), 0, (start.getPos() > (*end).getPos()) ? LINK_STAIR_DOWN:LINK_STAIR_UP);
			(*end).addLink(start.getId(), 0, (start.getPos() < (*end).getPos()) ? LINK_STAIR_DOWN:LINK_STAIR_UP);
		}
		else if(ladders)
		{
			start.addLink((*end).getId(), 0, (start.getPos() > (*end).getPos()) ? LINK_LADDER_DOWN:LINK_LADDER_UP);
			(*end).addLink(start.getId(), 0, (start.getPos() < (*end).getPos()) ? LINK_LADDER_DOWN:LINK_LADDER_UP);
		}
		else if(jump)
		{
			start.addLink((*end).getId(), 0, LINK_JUMP);
		}
		else
		{
			start.addLink((*end).getId(), combat, LINK_NORMAL);
			if(!combat)
			{	// combat nodes don't get links - they are sad and friendless.
				(*end).addLink(start.getId(), 0, LINK_NORMAL);
			}
		}
	}
}

void CAIListData::buildPathConnections(void)
{
	int i;

	failedConnects = 0;
	degenNodes = 0;

	for(i = 0; i < nodes.size(); i++)
	{
		nodes[i].destroyLinks();
		connectNode(nodes[i]);

		// also connect its combat points
		connectNode(nodes[i], 1);
	}

	eliminateRedundantConnections();

	for(i = 0; i < nodes.size(); i++)
	{
		nodes[i].initPathData(nodes.size());
		clearChecks();
		nodes[i].calcPath();
	}
}

int CAIListData::getNearestNode(vec3_t spot, float *distVal)
{
	int		pos = 0;
	float	minDist = 9999999;

	if(nodes.size() <= 1)
	{	//don't destroy the dummy guy
		return 0;
	}

	for(int i = 1; i < nodes.size(); i++)
	{
		vec3_t test;

		VectorSubtract(nodes[i].getPos(), spot, test);
		float dist = VectorLength(test);
		if(dist < minDist)
		{
			minDist = dist;
			pos = i;
		}
	}

	if(distVal)
	{
		*distVal = minDist;
	}
	return pos;
}

int CAIListData::getNearestCombatPoint(vec3_t spot, float *distVal)
{
	int		pos = 0;
	float	minDist = 9999999;

	if(combatPoints.size() <= 1)
	{	//don't destroy the dummy guy
		return 0;
	}

	for(int i = 0; i < combatPoints.size(); i++)
	{
		vec3_t test;

		VectorSubtract(combatPoints[i].getPos(), spot, test);
		float dist = VectorLength(test);
		if(dist < minDist)
		{
			minDist = dist;
			pos = i;
		}
	}

	if(distVal)
	{
		*distVal = minDist;
	}
	return pos;
}

void CAINode::removeBadLink(int deadId)
{
	list<CAIConnect>::iterator it;

	for(it = links.begin(); it != links.end(); it++)
	{
		if((*it).getLinkId() == deadId)
		{
			links.erase(it);
			return;
		}
	}
}

void CAINode::removeBadCombatPoint(int deadId)
{
	list<CAIConnect>::iterator it;

	for(it = combatPoints.begin(); it != combatPoints.end(); it++)
	{
		if((*it).getLinkId() == deadId)
		{
			combatPoints.erase(it);
			return;
		}
	}
}


void CAIListData::deleteNode(vec3_t spot)
{
	int delNode, delCP;
	float nearNode = 0;
	float nearCP = 0;

	delNode = getNearestNode(spot, &nearNode);
	delCP = getNearestCombatPoint(spot, &nearCP);

	if(nearCP <= 0)
	{
		nearCP = 1024;
	}
	if(nearNode <= 0)
	{
		nearNode = 1024;
	}

	if(nearCP < nearNode)
	{
		if(nearCP > 128)
		{
			return;
		}

		G_FreeEdict(combatPoints[delCP].getMarker());

		for(int i = 0; i < nodes.size(); i++)
		{
			nodes[i].removeBadCombatPoint(delCP);
		}

		vector<CAINode>::iterator it;

		it = &combatPoints[delCP];
		combatPoints.erase(it);

		Com_Printf("Combat Point deleted\n");
	}
	else
	{
		if(nearNode > 128)
		{
			return;
		}

		G_FreeEdict(nodes[delNode].getMarker());

		for(int i = 0; i < nodes.size(); i++)
		{
			nodes[i].removeBadLink(delNode);
		}


		vector<CAINode>::iterator it;

		it = &nodes[delNode];
		nodes.erase(it);

		Com_Printf("Point deleted\n");
	}
}

void CAIListData::toggleIgnoreNode(vec3_t spot)
{
	int moveNode;
	float nearNode = 0;

	moveNode = getNearestNode(spot, &nearNode);

	if(nearNode <= 0)
	{
		return;
	}

	if(nearNode > 128)
	{
		return;
	}

	int newState = nodes[moveNode].toggleIgnore();
	edict_t *ed = nodes[moveNode].getMarker();
	
	if(ed)
	{
		if(newState)
		{
			fxRunner.execContinualEffect("edit/ignore", ed);
		}
		else
		{
			fxRunner.stopContinualEffect("edit/ignore", ed);
		}
	}



	Com_Printf("Status on node toggled\n");
}

void CAIListData::numberLists(void)
{
	int i;
	int sz = nodes.size();

	for(i = 0; i < sz; i++)
	{
		nodes[i].setId(i);//hold over from before
	}

	sz = combatPoints.size();

	for(i = 0; i < sz; i++)
	{
		combatPoints[i].setId(i);
	}
}

void CAIListData::movePoint(vec3_t spot)
{
	int moveNode;
	int moveCP;
	float nearNode = 0;
	float nearCP = 0;

	moveNode = getNearestNode(spot, &nearNode);
	moveCP = getNearestCombatPoint(spot, &nearCP);

	if((nearCP <= 0)&&(nearNode <= 0))
	{
		return;
	}

	if((nearCP > 0)&&(nearCP < nearNode))
	{
		if(nearCP > 128)
		{
			return;
		}

		combatPoints[moveCP].setPos(spot);
		edict_t *ed = combatPoints[moveCP].getMarker();
		VectorCopy(spot, ed->s.origin);
	}
	else
	{
		if(nearNode > 128)
		{
			return;
		}

		nodes[moveNode].setPos(spot);
		edict_t *ed = nodes[moveNode].getMarker();
		VectorCopy(spot, ed->s.origin);
	}

	Com_Printf("Point moved\n");
}

void CAIListData::checkEnvironments(void)
{
	int i;
	int sz = nodes.size();

	// already connected with paths at this point

	// do environmental stuff for path nodes
	for(i = 0; i < sz; i++)
	{
		nodes[i].calcRadius();
	}

	sz = combatPoints.size();

	// do the same for combat points
	for(i = 0; i < sz; i++)
	{
		combatPoints[i].calcRadius();
	}
}

extern cvar_t	*ai_editdir;

void CAIListData::saveExport(void)
{
	vector<CAINode>::iterator it;

	FILE	*navFile;
	char	name[MAX_OSPATH];

	//ew!
	//Com_sprintf (name, sizeof(name), "f:/sof/base/maps/%s.nvb", level.mapname);
	Com_sprintf (name, sizeof(name), "%s/base/maps/%s.nvb", ai_editdir->string, level.mapname);

	navFile = fopen (name, "wb");
	if (!navFile)
	{
		Com_Printf ("ERROR: couldn't open %s.\n", name);
		return;
	}

	int version = AI_EXPORT_VERSION;

	fwrite(&version, sizeof(version), 1, navFile);

	int i = nodes.size();

	fwrite(&i, sizeof(i), 1, navFile);
	for(it = nodes.begin(); it != nodes.end(); it++)
	{
		(*it).savePaths(&navFile);
	}

	i = combatPoints.size();

	fwrite(&i, sizeof(i), 1, navFile);
	for(it = combatPoints.begin(); it != combatPoints.end(); it++)
	{
		(*it).savePaths(&navFile);
	}

	floorData.saveExport(&navFile);

	fclose(navFile);
}

void CAIListData::save(void)
{
	FILE	*navFile;
	char	name[MAX_OSPATH];

	//ew!
	//Com_sprintf (name, sizeof(name), "f:/sof/base/maps/%s.nav", level.mapname);
	Com_sprintf (name, sizeof(name), "%s/base/maps/%s.nav", ai_editdir->string, level.mapname);

	navFile = fopen (name, "wb");
	if (!navFile)
	{
		Com_Printf ("ERROR: couldn't open %s.\n", name);
		return;
	}

	loadVersion = AI_NODE_VERSION_CUR;

	fwrite(&loadVersion, sizeof(loadVersion), 1, navFile);

	int i = nodes.size();

	//fixme - if deleting is added in, the gaps are GOING to have to be taken care of.

	fwrite(&i, sizeof(i), 1, navFile);
	for(int j = 0; j < i; j++)
	{
		nodes[j].save(navFile);
	}
	
	Com_Printf("Adding %d nodes\n", nodes.size());

	i = combatPoints.size();

	//fixme - if deleting is added in, the gaps are GOING to have to be taken care of.

	fwrite(&i, sizeof(i), 1, navFile);
	for(j = 0; j < i; j++)
	{
		combatPoints[j].save(navFile);
	}

	floorData.save(navFile);

	fclose(navFile);
}


void CAIListData::load(void)
{
	FILE	*navFile;
	char	name[MAX_OSPATH];

	//ew!
	Com_sprintf (name, sizeof(name), "%s/base/maps/%s.nav", ai_editdir->string, level.mapname);

	navFile = fopen (name, "rb");
	if (!navFile)
	{
		Com_Printf ("ERROR: couldn't open %s.\n", name);
		return;
	}

	fread(&loadVersion, sizeof(loadVersion), 1, navFile);
	if(loadVersion < AI_NODE_VERSION)
	{
		Com_Printf ("ERROR: wrong version for NavFile - was %d, expected %d.\n", loadVersion, AI_NODE_VERSION_CUR);
		return;
	}

	int numNodes;

	fread(&numNodes, sizeof(numNodes), 1, navFile);

	nodes.clear();
	regions.clear();

	StartNodeDrawMonkey();
	initRegionTrace();

	for(int i = 0; i < numNodes; i++)
	{
		CAINode newPoint;

		newPoint.load(&navFile);

		newPoint.setMarker(makeNodeEnt(newPoint.getPos(), 0, newPoint.getPointType(), newPoint.getIgnore()));

		nodes.push_back(newPoint);
	}

	fread(&numNodes, sizeof(numNodes), 1, navFile);

	for(i = 0; i < numNodes; i++)
	{
		CAINode newPoint;

		newPoint.load(&navFile);

		newPoint.setMarker(makeNodeEnt(newPoint.getPos(), 0, newPoint.getPointType()));

		//combatPoints.push_back(newPoint);

	}

	if(loadVersion == 10)
	{	//load in the regions as well
		fread(&numNodes, sizeof(numNodes), 1, navFile);

		for(i = 0; i < numNodes; i++)
		{
			CAIRegion newRegion;

			newRegion.load(&navFile);

			regions.push_back(newRegion);
		}
	}

	if(loadVersion >= 11)
	{
		floorData.load(&navFile);
	}
	else
	{
		floorData.clear();
	}

	fclose(navFile);
}

void CAIListData::clearChecks(void)
{
	vector<CAINode>::iterator it;

	for(it = nodes.begin(); it != nodes.end(); it++)
	{
		(*it).setChecked(0);
		(*it).clearLinks();
	}
}

int canUseOperation(int *lastTime, char *operation, char *successMsg)
{
	if(*lastTime < level.time - 1)
	{
		*lastTime = level.time;
		Com_Printf("Press key again to %s\n", operation);
		return 0;
	}
	else
	{
		Com_Printf(successMsg);
		return 1;
	}
}

int	CAIListData::usesCmd(edict_t *ent, char	*cmd)
{
	static int lastSaveTime = 0;
	static int lastExportTime = 0;
	static int lastLoadTime = 0;
	static int lastDestroyTime = 0;

	if(!game.playerSkills.canCheat())
	{	// harder difficulty levels won't let you cheat
		return 0;
	}

	if (dm->isDM() && !sv_cheats->value)
	{
		return 0;
	}

	//findVolume(ent->s.origin);


	if (stricmp(cmd, "setpoint") == 0)
	{
		addPoint(ent, cmd, EPS_NORMAL);

		Com_Printf("Nav point added\n");
	}
	else if (stricmp(cmd, "savepoints") == 0)
	{
		if(canUseOperation(&lastSaveTime, "save", "Nav points saved off\n"))
		{
			save();
		}
	}
	else if (stricmp(cmd, "loadpoint") == 0)
	{
		if(canUseOperation(&lastLoadTime, "load", "Nav points loaded\n"))
		{
			load();
		}
	}
	else if (stricmp(cmd, "exportpoint") == 0)
	{
		if(canUseOperation(&lastExportTime, "export", "Nav points exported\n"))
		{
			exportPaths();
		}
	}
	else if (stricmp(cmd, "pointhelp") == 0)
	{
		Com_Printf("a = add point     m = move point     d = delete \n");
		Com_Printf("s = save          l = load           e = export     (Hit these twice to use)\n");
		Com_Printf("1 = flee point    2 = lean point     3 = duck point    4 = cover point\n");
		Com_Printf("5 = friend flee   6 = foe flee       z = destroyents   i = togglespawning\n");
	}
	else if (stricmp(cmd, "destroyents") == 0)
	{
		if(canUseOperation(&lastDestroyTime, "destroy entities", "Entities destroyed\n"))
		{
			elimEnts();
		}
	}
	else if (stricmp(cmd, "deletepoint") == 0)
	{
		deleteNode(ent->s.origin);
	}
	else if (stricmp(cmd, "movepoint") == 0)
	{
		movePoint(ent->s.origin);
	}
	else if(stricmp(cmd, "setflee") == 0)
	{
		addPoint(ent, cmd, EPS_FLEE);

		Com_Printf("Flee point added\n");
	}
	else if(stricmp(cmd, "setfleefriendly") == 0)
	{
		addPoint(ent, cmd, EPS_FLEEFRIENDLY);

		Com_Printf("Friendly-only flee point added\n");
	}
	else if(stricmp(cmd, "setfleeenemy") == 0)
	{
		addPoint(ent, cmd, EPS_FLEEENEMY);

		Com_Printf("Enemy-only flee point added\n");
	}
	else if(stricmp(cmd, "toggleignore") == 0)
	{
		toggleIgnoreNode(ent->s.origin);
	}
	else if(stricmp(cmd, "startregions") == 0)
	{
		initRegionTrace();
	}
	else if(stricmp(cmd, "toggleboxes") == 0)
	{
		showboxes = !showboxes;
	}
	else if(stricmp(cmd, "cleargrid") == 0)
	{
		floorData.clear();
	}

	else
	{
		return 0;
	}

	return 1;

}

void CAIListData::exportPaths(void)
{
	elimEnts();

	vec3_t farAway = {9999, 9999, 9999};
	nodes[0].setPos(farAway);//?


	OutputDebugString("Clearing nodes from walls\n");
	clearNodesFromWall();
	numberLists();
	OutputDebugString("Building connections\n");
	buildPathConnections();
	OutputDebugString("Validating links\n");
	validateNodeLinks();
	OutputDebugString("Checking environment\n");
	checkEnvironments();

	OutputDebugString("Preparing floor data\n");
	floorData.prepareForExport();

	saveExport();
}

void CAIListData::clearNodesFromWall(void)
{
	vec3_t minSize = {-24, -24, -24};
	vec3_t maxSize = {24, 24, 24};

	for(int i = 0; i < nodes.size(); i++)
	{
		vec3_t nodeMin, nodeMax;

		VectorCopy(nodes[i].getPos(), nodeMin);
		VectorCopy(nodes[i].getPos(), nodeMax);

		VectorAdd(nodeMin, minSize, nodeMin);
		VectorAdd(nodeMax, maxSize, nodeMax);

		trace_t tr;

		gi.trace(vec3_origin, nodeMin, nodeMax, vec3_origin, 0, MASK_SOLID, &tr);

		if(!tr.startsolid && !tr.allsolid && tr.fraction > .98)
		{
			continue;
		}

		int foundClear = 0;
		for(int j = 0; j < 8 && !foundClear; j++)
		{
			switch(j)
			{
			case 0:
				nodeMin[0] -= 8;
				nodeMax[0] -= 8;
				break;
			case 1:
				nodeMin[0] += 16;
				nodeMax[0] += 16;
				break;
			case 2:
				nodeMin[0] -= 8;
				nodeMax[0] -= 8;
				nodeMin[1] -= 8;
				nodeMax[1] -= 8;
				break;
			case 3:
				nodeMin[1] += 16;
				nodeMax[1] += 16;
				break;
			case 4:
				nodeMin[1] -= 8;
				nodeMax[1] -= 8;
				nodeMin[0] -= 16;
				nodeMax[0] -= 16;
				break;
			case 5:
				nodeMin[0] += 32;
				nodeMax[0] += 32;
				break;
			case 6:
				nodeMin[0] -= 16;
				nodeMax[0] -= 16;
				nodeMin[1] -= 16;
				nodeMax[1] -= 16;
				break;
			case 7:
				nodeMin[1] += 32;
				nodeMax[1] += 32;
				break;
			}
			gi.trace(vec3_origin, nodeMin, nodeMax, vec3_origin, 0, MASK_SOLID, &tr);
			if(!tr.startsolid && !tr.allsolid && tr.fraction > .98)
			{
				VectorSubtract(nodeMin, minSize, nodes[i].getPos());
				foundClear = 1;
				continue;
			}
		}
		//assert(foundClear);

	}
}

void CAIListData::ensureRegion(edict_t *ent)
{
	floorData.ensureRegion(level.sight_client->s.origin);
}