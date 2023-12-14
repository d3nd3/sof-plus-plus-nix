#include "q_shared.h"
#include "..\qcommon\mathlib.h"

/*
#if id386
#include "..\client\amd3d.h"
#endif
*/

cvar_t *use_amd3d;	// user-adjustable
cvar_t *use_mmx;	//
cvar_t *cpu_mmx;	// *not* user-adjustable
cvar_t *cpu_amd3d;	//

vec3_t vec3_origin = {0,0,0};
vec3_t vec3_up = {0,0,1};
vec3_t vec3_down = {0,0,-1};

//============================================================================

void RotatePointAroundVector( vec3_t dst, const vec3_t dir, const vec3_t point, float degrees )
{
	float	m[3][3];
	float	im[3][3];
	float	zrot[3][3];
	float	tmpmat[3][3];
	float	rot[3][3];
	int	i;
	vec3_t vr, vup, vf;

	vf[0] = dir[0];
	vf[1] = dir[1];
	vf[2] = dir[2];

	PerpendicularVector( vr, dir );
	CrossProduct( vr, vf, vup );

	m[0][0] = vr[0];
	m[1][0] = vr[1];
	m[2][0] = vr[2];

	m[0][1] = vup[0];
	m[1][1] = vup[1];
	m[2][1] = vup[2];

	m[0][2] = vf[0];
	m[1][2] = vf[1];
	m[2][2] = vf[2];

	memcpy( im, m, sizeof( im ) );

	im[0][1] = m[1][0];
	im[0][2] = m[2][0];
	im[1][0] = m[0][1];
	im[1][2] = m[2][1];
	im[2][0] = m[0][2];
	im[2][1] = m[1][2];

	memset( zrot, 0, sizeof( zrot ) );
	zrot[0][0] = zrot[1][1] = zrot[2][2] = 1.0F;

	zrot[0][0] = cos( degrees * DEGTORAD );
	zrot[0][1] = sin( degrees * DEGTORAD );
	zrot[1][0] = -sin( degrees * DEGTORAD );
	zrot[1][1] = cos( degrees * DEGTORAD );

	R_ConcatRotations( m, zrot, tmpmat );
	R_ConcatRotations( tmpmat, im, rot );

	for ( i = 0; i < 3; i++ )
	{
		dst[i] = rot[i][0] * point[0] + rot[i][1] * point[1] + rot[i][2] * point[2];
	}
}

void AngleVectors (vec3_t angles, vec3_t forward, vec3_t right, vec3_t up)
{
	float		angle;
	static float		sr, sp, sy, cr, cp, cy;
	// static to help MS compiler fp bugs

	angle = angles[YAW] * (M_PI*2 / 360);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[PITCH] * (M_PI*2 / 360);
	sp = sin(angle);
	cp = cos(angle);
	if(right||up)
	{	// as forward doesn't need these and calcing forward only is a fairly common case
		angle = angles[ROLL] * (M_PI*2 / 360);
		sr = sin(angle);
		cr = cos(angle);
	}

	if (forward)
	{
		forward[0] = cp*cy;
		forward[1] = cp*sy;
		forward[2] = -sp;
	}
	if (right)
	{
		right[0] = (-1*sr*sp*cy+-1*cr*-sy);
		right[1] = (-1*sr*sp*sy+-1*cr*cy);
		right[2] = -1*sr*cp;
	}
	if (up)
	{
		up[0] = (cr*sp*cy+-sr*-sy);
		up[1] = (cr*sp*sy+-sr*cy);
		up[2] = cr*cp;
	}
}


static void ProjectPointOnPlane( vec3_t dst, const vec3_t p, const vec3_t normal )
{
	float d;
	vec3_t n;
	float inv_denom;

	inv_denom = 1.0F / DotProduct( normal, normal );

	d = DotProduct( normal, p ) * inv_denom;

	n[0] = normal[0] * inv_denom;
	n[1] = normal[1] * inv_denom;
	n[2] = normal[2] * inv_denom;

	dst[0] = p[0] - d * n[0];
	dst[1] = p[1] - d * n[1];
	dst[2] = p[2] - d * n[2];
}

/*
** assumes "src" is normalized
*/
void PerpendicularVector( vec3_t dst, const vec3_t src )
{
	int	pos;
	int i;
	float minelem = 1.0F;
	vec3_t tempvec;

	/*
	** find the smallest magnitude axially aligned vector
	*/
	for ( pos = 0, i = 0; i < 3; i++ )
	{
		if ( fabs( src[i] ) < minelem )
		{
			pos = i;
			minelem = fabs( src[i] );
		}
	}
	tempvec[0] = tempvec[1] = tempvec[2] = 0.0F;
	tempvec[pos] = 1.0F;

	/*
	** project the point onto the plane defined by src
	*/
	ProjectPointOnPlane( dst, tempvec, src );

	/*
	** normalize the result
	*/
	VectorNormalize( dst );
}



/*
================
R_ConcatRotations
================
*/
static void R_ConcatRotations (float in1[3][3], float in2[3][3], float out[3][3])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
				in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
				in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
				in1[0][2] * in2[2][2];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
				in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
				in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
				in1[1][2] * in2[2][2];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
				in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
				in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
				in1[2][2] * in2[2][2];
}


/*
================
R_ConcatTransforms
================
*/
static void R_ConcatTransforms (float in1[3][4], float in2[3][4], float out[3][4])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
				in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
				in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
				in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
				in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
				in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
				in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
				in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
				in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
				in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
				in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
				in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
				in1[2][2] * in2[2][3] + in1[2][3];
}


//============================================================================




// this is the slow, general version
static int BoxOnPlaneSide2 (vec3_t emins, vec3_t emaxs, struct cplane_s *p)
{
	int		i;
	float	dist1, dist2;
	int		sides;
	vec3_t	corners[2];

	for (i=0 ; i<3 ; i++)
	{
		if (p->normal[i] < 0)
		{
			corners[0][i] = emins[i];
			corners[1][i] = emaxs[i];
		}
		else
		{
			corners[1][i] = emins[i];
			corners[0][i] = emaxs[i];
		}
	}
	dist1 = DotProduct (p->normal, corners[0]) - p->dist;
	dist2 = DotProduct (p->normal, corners[1]) - p->dist;
	sides = 0;
	if (dist1 >= 0)
		sides = 1;
	if (dist2 < 0)
		sides |= 2;

	return sides;
}

/*
==================
BoxOnPlaneSide

Returns 1, 2, or 1 + 2
==================
*/
#if !id386
int BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, struct cplane_s *p)
{
	float	dist1, dist2;
	int		sides;

// I know this isn't called for the PC, but as a note this case is covered by the calling macro anyway
#if 0
// fast axial cases
	if (p->type < 3)
	{
		if (p->dist <= emins[p->type])
			return 1;
		if (p->dist >= emaxs[p->type])
			return 2;
		return 3;
	}
#endif
	
// general case
	switch (p->signbits)
	{
	case 0:
dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		break;
	case 1:
dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		break;
	case 2:
dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		break;
	case 3:
dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		break;
	case 4:
dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		break;
	case 5:
dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		break;
	case 6:
dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		break;
	case 7:
dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		break;
	default:
		dist1 = dist2 = 0;		// shut up compiler
		assert( 0 );
		break;
	}

	sides = 0;
	if (dist1 >= p->dist)
		sides = 1;
	if (dist2 < p->dist)
		sides |= 2;

	assert( sides != 0 );

	return sides;
}
#else	// #if !id386

#pragma warning( disable: 4035 )
#pragma warning( disable: 4414 )	// "short jump to function converted to near"

int BoxOnPlaneSide_Intel (vec3_t emins, vec3_t emaxs, struct cplane_s *p);	// proto the other version for asm jmp
int BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, struct cplane_s *p)
{
	/*
	static int bops_initialized;
	static int Ljmptab[8];
	
	__asm {
		mov		eax,[cpu_amd3d]
		cmp		[eax]cvar_s.value,0	// sneaky, float 0 is same binary as int 0 on Intel/K6 <g>
		je		BoxOnPlaneSide_Intel		
		mov		eax,[use_amd3d]
		cmp		[eax]cvar_s.value,0	// sneaky, float 0 is same binary as int 0 on Intel/K6 <g>
		je		BoxOnPlaneSide_Intel

		// AMD/K6 version...
		
		push ebx
			
		cmp bops_initialized, 1
		je  initialized
		mov bops_initialized, 1
		
		mov Ljmptab[0*4], offset Lcase0
		mov Ljmptab[1*4], offset Lcase1
		mov Ljmptab[2*4], offset Lcase2
		mov Ljmptab[3*4], offset Lcase3
		mov Ljmptab[4*4], offset Lcase4
		mov Ljmptab[5*4], offset Lcase5
		mov Ljmptab[6*4], offset Lcase6
		mov Ljmptab[7*4], offset Lcase7
			
initialized:		

		// can't do this when function is __declspec( naked )!
//		mov			ebx,[emins]
//		mov			ecx,[emaxs]
//		mov			edx,[p]

		mov			ebx,ds:dword ptr[4+4+esp]	// emins
		mov			ecx,ds:dword ptr[4+8+esp]	// emaxs
		mov			edx,ds:dword ptr[4+12+esp]	// p


// unfortunately since the K6 support is via macros (and C has crap macro handling) then this code is a little longer
//	in places than I'd like because I have to put in extra opcodes...
//
		femms
		movq		mm0,[edx]p.normal		// mm0	p->normal[1]			| p->normal[0]		
		movq		mm5,[ecx]				// mm5  emaxs[1]				| emaxs[0]
		movq		mm6,[ebx]				// mm6  emins[1]				| emins[0]
		pfmul		(_mm5,_mm0)				// mm5  p->normal[1]*emaxs[1]	| p->normal[0]*emaxs[0]		// keep mm5
		pfmul		(_mm6,_mm0)				// mm6  p->normal[1]*emins[1]	| p->normal[0]*emins[0]		// keep mm6
		movd		mm7,[edx+8]p.normal		// mm7			...				| p->normal[2]
		movd		mm0,[ecx+8]				// mm0			...				| emaxs[2]
		punpckldq	mm7,mm7					// mm7	p->normal[2]			| p->normal[2]
		punpckldq	mm0,[ebx+8]				// mm0	emins[2]				| emaxs[2]
		movzx		eax,[edx]p.signbits
		pfmul		(_mm7,_mm0)				// mm7	p->normal[2]*emins[2]	| p->normal[2]*emaxs[2]		// keep mm7
		cmp			eax,8
		ja			Lerror

		jmp			dword ptr[Ljmptab+eax*4]

// mm5  p->normal[1]*emaxs[1]	| p->normal[0]*emaxs[0]		
// mm6  p->normal[1]*emins[1]	| p->normal[0]*emins[0]		
// mm7	p->normal[2]*emins[2]	| p->normal[2]*emaxs[2]		

//dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
//dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];

Lcase0:	movq		mm0,mm5										// p->normal[1]*emaxs[1]							| p->normal[0]*emaxs[0]
									movq		mm1,mm6			// p->normal[1]*emins[1]							| p->normal[0]*emins[0]			
		pfacc		(_mm0,_mm0)									// p->normal[1]*emaxs[1] + p->normal[0]*emaxs[0]	| p->normal[1]*emaxs[1] + p->normal[0]*emaxs[0]
									pfacc		(_mm1,_mm1)		// p->normal[1]*emins[1] + p->normal[0]*emins[0]	| p->normal[1]*emins[1] + p->normal[0]*emins[0]
		pfadd		(_mm0,_mm7)									// mm0.l = dist1		
									pfadd		(_mm1,_mm7)		// mm1.h = dist2
									psrlq		mm1,32			// mm1.l = dist2
		
		jmp			LSetSides

//dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
//dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];

Lcase1:	movq		mm0,mm6										//						...							| p->normal[0]*emins[0]				
									movq		mm1,mm6			// p->normal[1]*emins[1]							| .....
		punpckldq	mm0,mm7										// p->normal[2]*emaxs[2]							| p->normal[0]*emins[0]
									punpckhdq	mm1,mm7			// p->normal[2]*emins[2]							| p->normal[1]*emins[1]
		pfacc		(_mm0,_mm0)									// p->normal[2]*emaxs[2] + p->normal[0]*emins[0]	| p->normal[2]*emaxs[2] + p->normal[0]*emins[0]
									pfacc		(_mm1,_mm1)		// p->normal[2]*emins[2] + p->normal[1]*emins[1]	| p->normal[2]*emins[2] + p->normal[1]*emins[1]
		pfadd		(_mm0,_mm5)									// mm0.h = dist1
									pfadd		(_mm1,_mm5)		// mm1.l = dist2
		psrlq		mm0,32										// mm0.l = dist1
				
		jmp			LSetSides

//dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
//dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];

Lcase2:	movq		mm0,mm5										//													| p->normal[0]*emaxs[0]
									movq		mm1,mm5			// p->normal[1]*emaxs[1]							| ...
		punpckldq	mm0,mm7										// p->normal[2]*emaxs[2]							| p->normal[0]*emaxs[0]
									punpckhdq	mm1,mm7			// p->normal[2]*emins[2]							| p->normal[1]*emaxs[1]
		pfacc		(_mm0,_mm0)									// p->normal[2]*emaxs[2] + p->normal[0]*emaxs[0]	| p->normal[2]*emaxs[2] + p->normal[0]*emaxs[0]
									pfacc		(_mm1,_mm1)		// p->normal[2]*emins[2] + p->normal[1]*emaxs[1]	| p->normal[2]*emins[2] + p->normal[1]*emaxs[1]
		pfadd		(_mm0,_mm6)									// mm0.h = dist1
									pfadd		(_mm1,_mm6)		// mm1.l = dist2
		psrlq		mm0,32										// mm0.l = dist1
		jmp			LSetSides

//dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
//dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];

Lcase3:	movq		mm0,mm6										// p->normal[1]*emins[1]							| p->normal[0]*emins[0]
									movq		mm1,mm5			// p->normal[1]*emaxs[1]							| p->normal[0]*emaxs[0]		
		pfacc		(_mm0,_mm0)									// p->normal[1]*emins[1] + p->normal[0]*emins[0]	| p->normal[1]*emins[1] + p->normal[0]*emins[0]
									pfacc		(_mm1,_mm1)		// p->normal[1]*emaxs[1] + p->normal[0]*emaxs[0]	| p->normal[1]*emaxs[1] + p->normal[0]*emaxs[0]		
		pfadd		(_mm0,_mm7)									// mm0.l = dist1
									pfadd		(_mm1,_mm7)		// mm1.h = dist2
									psrlq		mm1,32			// mm1.l = dist2
		jmp			LSetSides
		
//dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
//dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];

Lcase4:	movq		mm0,mm5										// p->normal[1]*emaxs[1]							| p->normal[0]*emaxs[0]		
									movq		mm1,mm6			// p->normal[1]*emins[1]							| p->normal[0]*emins[0]
		pfacc		(_mm0,_mm0)									// p->normal[1]*emaxs[1] + p->normal[0]*emaxs[0]	| p->normal[1]*emaxs[1] + p->normal[0]*emaxs[0]		
									pfacc		(_mm1,_mm1)		// p->normal[1]*emins[1] + p->normal[0]*emins[0]	| p->normal[1]*emins[1] + p->normal[0]*emins[0]
		pfadd		(_mm0,_mm7)									// mm0.h = dist2
									pfadd		(_mm1,_mm7)		// mm1.l = dist1
		psrlq		mm0,32			// mm0.l = dist2
		
		jmp			LSetSides

//dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
//dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];

Lcase5:	movq		mm0,mm5										// p->normal[1]*emaxs[1]							| ...
									movq		mm1,mm5			//													| p->normal[0]*emaxs[0]
		punpckhdq	mm0,mm7										// p->normal[2]*emins[2]							| p->normal[1]*emaxs[1]
									punpckldq	mm1,mm7			// p->normal[2]*emaxs[2]							| p->normal[0]*emaxs[0]
		pfacc		(_mm0,_mm0)									// p->normal[2]*emins[2] + p->normal[1]*emaxs[1]	| p->normal[2]*emins[2] + p->normal[1]*emaxs[1]
									pfacc		(_mm1,_mm1)		// p->normal[2]*emaxs[2] + p->normal[0]*emaxs[0]	| p->normal[2]*emaxs[2] + p->normal[0]*emaxs[0]
		pfadd		(_mm0,_mm6)									// mm0.l = dist2
									pfadd		(_mm1,_mm6)		// mm1.h = dist1
									psrlq		mm1,32			// mm1.l = dist1

		jmp			LSetSides

//dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
//dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];

Lcase6:	movq		mm0,mm6										// p->normal[1]*emins[1]							| ......
									movq		mm1,mm6			//						...							| p->normal[0]*emins[0]				
		punpckhdq	mm0,mm7										// p->normal[2]*emins[2]							| p->normal[1]*emins[1]
									punpckldq	mm1,mm7			// p->normal[2]*emaxs[2]							| p->normal[0]*emins[0]
		pfacc		(_mm0,_mm0)									// p->normal[2]*emins[2] + p->normal[1]*emins[1]	| p->normal[2]*emins[2] + p->normal[1]*emins[1]
									pfacc		(_mm1,_mm1)		// p->normal[2]*emaxs[2] + p->normal[0]*emins[0]	| p->normal[2]*emaxs[2] + p->normal[0]*emins[0]
		pfadd		(_mm0,_mm5)									// mm0.l = dist2
									pfadd		(_mm1,_mm5)		// mm1.h = dist1
									psrlq		mm1,32			// mm1.l = dist1

		jmp			LSetSides

//dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
//dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];

Lcase7:	movq		mm0,mm6										// p->normal[1]*emins[1]							| p->normal[0]*emins[0]			
									movq		mm1,mm5			// p->normal[1]*emaxs[1]							| p->normal[0]*emaxs[0]
		pfacc		(_mm0,_mm0)									// p->normal[1]*emins[1] + p->normal[0]*emins[0]	| p->normal[1]*emins[1] + p->normal[0]*emins[0]
									pfacc		(_mm1,_mm1)		// p->normal[1]*emaxs[1] + p->normal[0]*emaxs[0]	| p->normal[1]*emaxs[1] + p->normal[0]*emaxs[0]
		pfadd		(_mm0,_mm7)									// mm0.h = dist2
									pfadd		(_mm1,_mm7)		// mm1.l = dist1		
		psrlq		mm0,32										// mm0.l = dist2
	
//		jmp			LSetSides

LSetSides:
		// mm0.l = dist1
		// mm1.l = dist2
		
		movd		mm2,[edx]p.dist	// (because of only partial K6 inline support)
		pfcmpge		(_mm0,_mm2)
									pfcmpgt		(_mm2,_mm1)
		movd		ebx,mm0
									movd		ecx,mm2
		and			ebx,1
									and			ecx,2
		mov			eax,ebx
		or			eax,ecx			// eax = return val

		femms

		pop			ebx		
		ret
Lerror:	emms		// rather than 'femms', since this handily stops the compiler complaining about no 'emms' at MMX end (and no speed issue here)
		int 3
	}
	*/
	return 0;
}
int BoxOnPlaneSide_Intel (vec3_t emins, vec3_t emaxs, struct cplane_s *p)
{
	/*
	static int bops_initialized;
	static int Ljmptab[8];

	__asm {

		push ebx
			
		cmp bops_initialized, 1
		je  initialized
		mov bops_initialized, 1
		
		mov Ljmptab[0*4], offset Lcase0
		mov Ljmptab[1*4], offset Lcase1
		mov Ljmptab[2*4], offset Lcase2
		mov Ljmptab[3*4], offset Lcase3
		mov Ljmptab[4*4], offset Lcase4
		mov Ljmptab[5*4], offset Lcase5
		mov Ljmptab[6*4], offset Lcase6
		mov Ljmptab[7*4], offset Lcase7
			
initialized:

// fast axial cases
//	if (p->type < 3)
//	{
//		if (p->dist <= emins[p->type])
//			return 1;
//		if (p->dist >= emaxs[p->type])
//			return 2;
//		return 3;
//	}		
		mov		ecx,ds:dword ptr[4+4+esp]	// emins
		mov		ebx,ds:dword ptr[4+8+esp]	// emaxs
		mov		edx,ds:dword ptr[4+12+esp]	// p
//  This is the code for the fast axial case, I noticed it was missing from the ASM version, unfortunately I then
//	spotted that the macro which calls this function does the check itself so this isn't needed - duh!  However, you
//	should remember to remove the check from the C version above or you're doing duplicate work.
#if 0
		movzx	eax,[edx]p.type		
		cmp		eax,3
		jae		NoAxialShortcut

		fld		[edx]p.dist			
		shl		eax, 2		
		add		eax, ecx	// emins
		fcomp	DWORD PTR [eax]
		fnstsw	ax
		test	ah, 65     ; 00000041H
		je		SHORT $L2691
		
		mov		eax,1
		jmp		SHORT BoxExit

$L2691:	// if (p->dist >= emaxs[p->type])

		movzx	eax,[edx]p.type		
		shl		eax, 2		
		add		eax,ebx		// emaxs
		fld		[edx]p.dist			
		fcomp	DWORD PTR [eax]
		fnstsw	ax
		test	ah, 1
		mov		eax,3
		jne		SHORT BoxExit
		dec		eax		
		jmp		SHORT BoxExit 		

NoAxialShortcut:
#endif

//		mov edx,ds:dword ptr[4+12+esp]
//		mov ecx,ds:dword ptr[4+4+esp]		
//		mov ebx,ds:dword ptr[4+8+esp]
		movzx eax,ds:byte ptr[17+edx]
		cmp eax,8
		ja Lerror
		fld ds:dword ptr[0+edx]
		fld st(0)
		jmp dword ptr[Ljmptab+eax*4]
Lcase0:
		fmul ds:dword ptr[ebx]
		fld ds:dword ptr[0+4+edx]
		fxch st(2)
		fmul ds:dword ptr[ecx]
		fxch st(2)
		fld st(0)
		fmul ds:dword ptr[4+ebx]
		fld ds:dword ptr[0+8+edx]
		fxch st(2)
		fmul ds:dword ptr[4+ecx]
		fxch st(2)
		fld st(0)
		fmul ds:dword ptr[8+ebx]
		fxch st(5)
		faddp st(3),st(0)
		fmul ds:dword ptr[8+ecx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase1:
		fmul ds:dword ptr[ecx]
		fld ds:dword ptr[0+4+edx]
		fxch st(2)
		fmul ds:dword ptr[ebx]
		fxch st(2)
		fld st(0)
		fmul ds:dword ptr[4+ebx]
		fld ds:dword ptr[0+8+edx]
		fxch st(2)
		fmul ds:dword ptr[4+ecx]
		fxch st(2)
		fld st(0)
		fmul ds:dword ptr[8+ebx]
		fxch st(5)
		faddp st(3),st(0)
		fmul ds:dword ptr[8+ecx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase2:
		fmul ds:dword ptr[ebx]
		fld ds:dword ptr[0+4+edx]
		fxch st(2)
		fmul ds:dword ptr[ecx]
		fxch st(2)
		fld st(0)
		fmul ds:dword ptr[4+ecx]
		fld ds:dword ptr[0+8+edx]
		fxch st(2)
		fmul ds:dword ptr[4+ebx]
		fxch st(2)
		fld st(0)
		fmul ds:dword ptr[8+ebx]
		fxch st(5)
		faddp st(3),st(0)
		fmul ds:dword ptr[8+ecx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase3:
		fmul ds:dword ptr[ecx]
		fld ds:dword ptr[0+4+edx]
		fxch st(2)
		fmul ds:dword ptr[ebx]
		fxch st(2)
		fld st(0)
		fmul ds:dword ptr[4+ecx]
		fld ds:dword ptr[0+8+edx]
		fxch st(2)
		fmul ds:dword ptr[4+ebx]
		fxch st(2)
		fld st(0)
		fmul ds:dword ptr[8+ebx]
		fxch st(5)
		faddp st(3),st(0)
		fmul ds:dword ptr[8+ecx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase4:
		fmul ds:dword ptr[ebx]
		fld ds:dword ptr[0+4+edx]
		fxch st(2)
		fmul ds:dword ptr[ecx]
		fxch st(2)
		fld st(0)
		fmul ds:dword ptr[4+ebx]
		fld ds:dword ptr[0+8+edx]
		fxch st(2)
		fmul ds:dword ptr[4+ecx]
		fxch st(2)
		fld st(0)
		fmul ds:dword ptr[8+ecx]
		fxch st(5)
		faddp st(3),st(0)
		fmul ds:dword ptr[8+ebx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase5:
		fmul ds:dword ptr[ecx]
		fld ds:dword ptr[0+4+edx]
		fxch st(2)
		fmul ds:dword ptr[ebx]
		fxch st(2)
		fld st(0)
		fmul ds:dword ptr[4+ebx]
		fld ds:dword ptr[0+8+edx]
		fxch st(2)
		fmul ds:dword ptr[4+ecx]
		fxch st(2)
		fld st(0)
		fmul ds:dword ptr[8+ecx]
		fxch st(5)
		faddp st(3),st(0)
		fmul ds:dword ptr[8+ebx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase6:
		fmul ds:dword ptr[ebx]
		fld ds:dword ptr[0+4+edx]
		fxch st(2)
		fmul ds:dword ptr[ecx]
		fxch st(2)
		fld st(0)
		fmul ds:dword ptr[4+ecx]
		fld ds:dword ptr[0+8+edx]
		fxch st(2)
		fmul ds:dword ptr[4+ebx]
		fxch st(2)
		fld st(0)
		fmul ds:dword ptr[8+ecx]
		fxch st(5)
		faddp st(3),st(0)
		fmul ds:dword ptr[8+ebx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase7:
		fmul ds:dword ptr[ecx]
		fld ds:dword ptr[0+4+edx]
		fxch st(2)
		fmul ds:dword ptr[ebx]
		fxch st(2)
		fld st(0)
		fmul ds:dword ptr[4+ecx]
		fld ds:dword ptr[0+8+edx]
		fxch st(2)
		fmul ds:dword ptr[4+ebx]
		fxch st(2)
		fld st(0)
		fmul ds:dword ptr[8+ecx]
		fxch st(5)
		faddp st(3),st(0)
		fmul ds:dword ptr[8+ebx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
LSetSides:
		faddp st(2),st(0)
		fcomp ds:dword ptr[12+edx]
		xor ecx,ecx
		fnstsw ax
		fcomp ds:dword ptr[12+edx]
		and ah,1
		xor ah,1
		add cl,ah
		fnstsw ax
		and ah,1
		add ah,ah
		add cl,ah		
		mov eax,ecx
//BoxExit:
		pop ebx
		ret
Lerror:
		int 3
	}
	*/
	return 0;
}
#pragma warning( default: 4414 )	// short jump to function converted to near
#pragma warning( default: 4035 )
#endif	// #if !id386

static void ClearBounds (vec3_t mins, vec3_t maxs)
{
	mins[0] = mins[1] = mins[2] = 99999;
	maxs[0] = maxs[1] = maxs[2] = -99999;
}

void AddPointToBounds (vec3_t v, vec3_t mins, vec3_t maxs)
{
	int		i;
	vec_t	val;

	for (i=0 ; i<3 ; i++)
	{
		val = v[i];
		if (val < mins[i])
			mins[i] = val;
		if (val > maxs[i])
			maxs[i] = val;
	}
}

vec_t VectorNormalize (vec3_t v)
{
	float	length, ilength;

	length = DotProduct(v, v);
	length = sqrt (length);		// FIXME

	if (length)
	{
		ilength = 1/length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}
		
	return length;

}

float NormalizeAngle (float angle)
{
// FIXME:  make this use fmod
	while (angle >= 360)
	{
		angle -= 360;
	}
	while (angle < 0)
	{
		angle += 360;
	}
	return angle;
}

void CrossProduct (vec3_t v1, vec3_t v2, vec3_t cross)
{
	cross[0] = v1[1]*v2[2] - v1[2]*v2[1];
	cross[1] = v1[2]*v2[0] - v1[0]*v2[2];
	cross[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

double sqrt(double x);

vec_t VectorLength(vec3_t v)
{
	float	length;
	
	length = DotProduct(v, v);
	length = sqrt (length);		// FIXME

	return length;
}

static vec_t VectorSeparation(vec3_t v0, vec3_t v1)
{
	vec3_t	v;
	float	length;

	VectorSubtract(v0, v1, v);
	length = DotProduct(v, v);
	length = sqrt (length);		// FIXME

	return length;
}

//====================================================================================

/*
============
COM_SkipPath
============
*/
char *COM_SkipPath (char *pathname)
{
	char	*last;
	
	last = pathname;
	while (*pathname)
	{
		if (*pathname=='/')
			last = pathname+1;
		pathname++;
	}
	return last;
}

/*
============
COM_StripExtension
============
*/
void COM_StripExtension (char *in, char *out)
{
	while (*in && *in != '.')
		*out++ = *in++;
	*out = 0;
}

/*
============
COM_FileExtension
============
*/
char *COM_FileExtension (char *in)
{
	static char exten[8];
	int		i;

	while (*in && *in != '.')
		in++;
	if (!*in)
		return "";
	in++;
	for (i=0 ; i<7 && *in ; i++,in++)
		exten[i] = *in;
	exten[i] = 0;
	return exten;
}

/*
============
COM_FileBase
============
*/
static void COM_FileBase (char *in, char *out)
{
	char *s, *s2;
	
	s = in + strlen(in) - 1;
	
	while (s != in && *s != '.')
		s--;
	
	for (s2 = s ; s2 != in && *s2 != '/' ; s2--)
	;
	
	if (s-s2 < 2)
		out[0] = 0;
	else
	{
		s--;
		strncpy (out,s2+1, s-s2);
		out[s-s2] = 0;
	}
}

/*
============
COM_FilePath

Returns the path up to, but not including the last /
============
*/
void COM_FilePath (char *in, char *out)
{
	char *s;
	
	s = in + strlen(in) - 1;
	
	while (s != in && *s != '/')
		s--;

	strncpy (out,in, s-in);
	out[s-in] = 0;
}


/*
==================
COM_DefaultExtension
==================
*/
static void COM_DefaultExtension (char *path, char *extension)
{
	char    *src;
//
// if path doesn't have a .EXT, append extension
// (extension should include the .)
//
	src = path + strlen(path) - 1;

	while (*src != '/' && src != path)
	{
		if (*src == '.')
			return;                 // it has an extension
		src--;
	}

	strcat (path, extension);
}

/*
============================================================================

					BYTE ORDER FUNCTIONS

============================================================================
*/

// can't just use function pointers, or dll linkage can
// mess up when qcommon is included in multiple places
short	(*_BigShort) (short l);
int		(*_BigLong) (int l);
float	(*_BigFloat) (float l);

short	BigShort(short l){return _BigShort(l);}
int		BigLong (int l) {return _BigLong(l);}
float	BigFloat (float l) {return _BigFloat(l);}

static short   ShortSwap (short l)
{
	byte    b1,b2;

	b1 = l&255;
	b2 = (l>>8)&255;

	return (b1<<8) + b2;
}

static int    LongSwap (int l)
{
	byte    b1,b2,b3,b4;

	b1 = l&255;
	b2 = (l>>8)&255;
	b3 = (l>>16)&255;
	b4 = (l>>24)&255;

	return ((int)b1<<24) + ((int)b2<<16) + ((int)b3<<8) + b4;
}

static float FloatSwap (float f)
{
	union
	{
		float	f;
		byte	b[4];
	} dat1, dat2;
	
	
	dat1.f = f;
	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];
	return dat2.f;
}

/*
================
Swap_Init
================
*/
void Swap_Init (void)
{
	_BigShort = ShortSwap;
	_BigLong = LongSwap;
	_BigFloat = FloatSwap;
}

/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday
============
*/
char *va(char *format, ...)
{
	va_list		argptr;
	static char		string[1024];
	
	va_start (argptr, format);
	vsprintf (string, format,argptr);
	va_end (argptr);

	return string;	
}

char	com_token[MAX_TOKEN_CHARS];

/*
==============
COM_Parse

Parse a token out of a string
==============
*/
char *COM_Parse (char **data_p)
{
	int		c;
	int		len;
	char	*data;

	data = *data_p;
	len = 0;
	com_token[0] = 0;
	
	if (!data)
	{
		*data_p = NULL;
		return "";
	}
		
// skip whitespace
skipwhite:
	while ( (byte)(c = *data) <= ' ')
	{
		if (c == 0)
		{
			*data_p = NULL;
			return "";
		}
		data++;
	}
	
// skip // comments
	if (c=='/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;
		goto skipwhite;
	}
	

// handle quoted strings specially
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c=='\"' || !c)
			{
				com_token[len] = 0;
				*data_p = data;
				return com_token;
			}
			if (len < MAX_TOKEN_CHARS)
			{
				com_token[len] = c;
				len++;
			}
		}
	}

// parse a regular word
	do
	{
		if (len < MAX_TOKEN_CHARS)
		{
			com_token[len] = c;
			len++;
		}
		data++;
		c = *data;
	} while ((byte)c > 32);

	if (len == MAX_TOKEN_CHARS)
	{
//		Com_Printf ("Token exceeded %i chars, discarded.\n", MAX_TOKEN_CHARS);
		len = 0;
	}
	com_token[len] = 0;

	*data_p = data;
	return com_token;
}

/*
============================================================================

					LIBRARY REPLACEMENT FUNCTIONS

============================================================================
*/



void Com_sprintf (char *dest, int size, char *fmt, ...)
{
	int		len;
	va_list	argptr;
	char	bigbuffer[0x10000];

	va_start (argptr,fmt);
	len = vsprintf (bigbuffer,fmt,argptr);
	va_end (argptr);
	if (len >= size)
		Com_Printf ("Com_sprintf: overflow of %i in %i\n", len, size);
	strncpy (dest, bigbuffer, size-1);
}

/*
=====================================================================

  INFO STRINGS

=====================================================================
*/

const char *Info_NextKey(const char *s, char *key, char *value)
{
	char	c;

	assert(key);
	assert(value);

	*key = 0;
	*value = 0;

	c = *s++;
	if((c != '\\') || (c == '\n') || !c)
	{
		return(NULL);
	}
	// Get key
	while(true)
	{
		c = *s++;
		if(c == '\\')
		{
			break;
		}
		if((c == '\n') || !c)
		{
			return(NULL);
		}
		*key++ = c;
	}
	*key++ = 0;

	// Get value
	while(true)
	{
		c = *s;
		if(c == '\\')
		{
			break;
		}
		if((c == '\n') || !c)
		{
			break;
		}
		s++;
		*value++ = c;
	}
	*value++ = 0;
	return(s);
}

/*
===============
Info_ValueForKey

Searches the string for the given
key and returns the associated value, or an empty string.
===============
*/
char *Info_ValueForKey (const char *s, const char *key)
{
	char	pkey[512];
	static	char value[2][512];	// use two buffers so compares
								// work without stomping on each other
	static	int	valueindex;
	char	*o;
	
	valueindex ^= 1;
	if (*s == '\\')
		s++;
	while (1)
	{
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return "";
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value[valueindex];

		while (*s != '\\' && *s)
		{
			if (!*s)
				return "";
			*o++ = *s++;
		}
		*o = 0;

		if (!strcmp (key, pkey) )
			return value[valueindex];

		if (!*s)
			return "";
		s++;
	}
}

void Info_RemoveKey (char *s, char *key)
{
	char	*start;
	char	pkey[512];
	char	value[512];
	char	*o;

	if (strstr (key, "\\"))
	{
//		Com_Printf ("Can't use a key with a \\\n");
		return;
	}

	while (1)
	{
		start = s;
		if (*s == '\\')
			s++;
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while (*s != '\\' && *s)
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		if (!strcmp (key, pkey) )
		{
			strcpy (start, s);	// remove this part
			return;
		}

		if (!*s)
			return;
	}

}


/*
==================
Info_Validate

Some characters are illegal in info strings because they
can mess up the server's parsing
==================
*/
bool Info_Validate (char *s)
{
	if (strstr (s, "\""))
		return false;
	if (strstr (s, ";"))
		return false;
	return true;
}

void Info_SetValueForKey (char *s, char *key, char *value)
{
	char	newi[MAX_INFO_STRING], *v;
	int		maxsize = MAX_INFO_STRING;

	if (strstr (key, "\\") || strstr (value, "\\") )
	{
		Com_Printf ("Can't use keys or values with a \\\n");
		return;
	}

	if (strstr (key, ";") )
	{
		Com_Printf ("Can't use keys or values with a semicolon\n");
		return;
	}

	if (strstr (key, "\"") || strstr (value, "\"") )
	{
		Com_Printf ("Can't use keys or values with a \"\n");
		return;
	}

	if (strlen(key) > MAX_INFO_KEY-1 || strlen(value) > MAX_INFO_KEY-1)
	{
		Com_Printf ("Keys and values must be < 64 characters.\n");
		return;
	}
	Info_RemoveKey (s, key);
	if (!value || !strlen(value))
		return;

	Com_sprintf (newi, sizeof(newi), "\\%s\\%s", key, value);

	if (strlen(newi) + strlen(s) > maxsize)
	{
		Com_Printf ("Info string length exceeded\n");
		return;
	}

	// only copy ascii values (which is all 255 chars now with foreign characters)
	s += strlen(s);
	v = newi;
	while (*v)
	{
		*s++ = *v++;
	}
	*s = 0;
}

//====================================================================
