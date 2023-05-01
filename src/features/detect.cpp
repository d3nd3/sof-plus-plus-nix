#include "common.h"
/*
	typedef struct usercmd_s
	{
		byte	msec;
		byte	buttons;
		byte	lightlevel;		// light level the player is standing on
		char	lean;			// -1 or 1
		short	angles[3];
		short	forwardmove;
		short	sidemove;
		short	upmove;
		float	fireEvent;
		float	altfireEvent;
	} usercmd_t;
*/
/*
newcmd oldcmd oldestcmd

newcmd is the combination of the previous.
because readDeltaUsercmd calls memcpy.

How to handle usercmd angle values wrapping?

if cmd.pitch < -2048:
	cmd.pitch += 4096
elif cmd.pitch > 2047:
	cmd.pitch -= 4096

halfwaypoint in between : -1 and 0

4096 / 2 = 2048
*/

bool newMaxAngle(short angle1, short angle2, short &newmax)
{
	short diff = angle1 - angle2;
	// always take the shortest path , so the max angle is 2048, representing 180 degrees.
	if ( diff != 0 ) {
		if ( diff > 2048 ) diff = 4096 - diff;
		if ( diff > newmax ) {
			newmax = diff;
			return true;
		}
	}
	return false;
}
std::array<short, 32> detect_max_yaw;
std::array<short, 32> detect_max_pitch;


void validateInputs(int playerSlot, usercmd_t * now, usercmd_t * old)
{
	
	/*
		can create 2 comparisons. now <=> old :: call diff_new and old <=> oldest :: call diff_old.

		4096 is the largest move which represents 360 degrees.

		Since we take the shortest distance between 2 angles.
		2048 is the largest value, representing 180 degrees.
	*/
	// pitch
	if ( newMaxAngle(now->angles[0],old->angles[0], detect_max_pitch[playerSlot]) ) {
		SOFPPNIX_DEBUG("max_pitch: %d", detect_max_pitch[playerSlot]);
	}

	// yaw
	if ( newMaxAngle(now->angles[1],old->angles[1], detect_max_yaw[playerSlot]) ) {
		SOFPPNIX_DEBUG("max_yaw: %d", detect_max_yaw[playerSlot]);
	}
	
}

std::array<edict_t*, 32> distractor;
void spawnDistraction(edict_t * ent,int slot)
{
	distractor[slot] = orig_G_Spawn();
	distractor[slot]->solid = SOLID_TRIGGER;
	gclient_t * gcl = ent->client;
	short * origin = gcl->ps.pmove.origin;
	// gclient_t * gcl = *(unsigned int*)((void*)ent + EDICT_GCLIENT);
	// short * origin = (short*)((void*)gcl + GCLIENT_PS_PM_ORIGIN);
	// SOFPPNIX_DEBUG("Origin: %hd %hd %hd",origin[0],origin[1],origin[2]);

	// SOFPPNIX_DEBUG("VelocityFloat: %f %f %f",ent->velocity[0],ent->velocity[1],ent->velocity[2]);
	
	// vec_t vel = VectorLength(ent->velocity);
	// SOFPPNIX_DEBUG("Velocity: %f",vel);
}



/*
gclient_s - this structure is cleared on each PutClientInServer(),

contains player_state_t	ps, which contains pmove_state_t which contains delta_angles

delta angles are set in PutClientInServer. Ensure call this after.

The best thing to exploit is the GetNearest function.

*/
void simulateAim(short  * yaw, short * pitch, short * delta_angles)
{
	float float_delta_pitch = SHORT2ANGLE(delta_angles[0]);
	float float_delta_yaw = SHORT2ANGLE(delta_angles[1]);



	// GCLIENT_PS_PM_DELTA
/*
	float	fViewAngles[2]; // store viewangles in here
	
	float Y_BASE_LENGTH; // horizontal hypotenuse
	float deltax = SHORT2ANGLE( *delta[1] );
	float deltay = SHORT2ANGLE( *delta[0] );
	vec3_t v3Distance;
	v3Distance[0] = v3TargOrigin[0] - *cl_refdef_vieworg[0];
	v3Distance[1] = v3TargOrigin[1] - *cl_refdef_vieworg[1];
	v3Distance[2] = v3TargOrigin[2] - ((*cl_refdef_vieworg[2]) -13.0f);

	Y_BASE_LENGTH = sqrt(  (v3Distance[0] * v3Distance[0]) +  (v3Distance[1] * v3Distance[1])  );
	float fHypoDistance =  sqrt((v3Distance[2] * v3Distance[2]) + (Y_BASE_LENGTH * Y_BASE_LENGTH ) ); 
	//my_Com_Printf(P_BLUE"v3Distance[0] : %f\nv3Distance[1] : %f\nv3Distance[2] : %f\n",v3Distance[0],v3Distance[1],v3Distance[2]);
	

	
	fViewAngles[0] =  -1 * asin( 
		v3Distance[2] / 
		fHypoDistance
		) * (ONEEIGHTYOVERPI)  - deltay; // y

	
	if ( v3Distance[0] >= 0 ) // + x  // top quadrants
		fViewAngles[1] = asin( v3Distance[1] / Y_BASE_LENGTH ) * (ONEEIGHTYOVERPI) - deltax;
	else // - x
	{
		if ( v3Distance[1] >= 0 ) 
			fViewAngles[1] = 180 - asin( v3Distance[1] / Y_BASE_LENGTH ) * (ONEEIGHTYOVERPI) - deltax;
		else 
			fViewAngles[1] = -180 - asin( v3Distance[1] / Y_BASE_LENGTH ) * (ONEEIGHTYOVERPI) - deltax;
	
	}

	if (fViewAngles[1] > 180)
		fViewAngles[1] -= 360;
	else
	if (fViewAngles[1] < -180)
	fViewAngles[1] += 360; 
*/
}