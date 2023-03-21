#pragma once

/*
==============================================================

MATHLIB

==============================================================
*/

extern vec3_t vec3_origin;	
extern vec3_t vec3_up;
extern vec3_t vec3_down;

#define Macro_VectorCopy(a,b)	(b[0]=a[0],b[1]=a[1],b[2]=a[2])

_inline void VectorSubtract(vec3_t a, vec3_t b, vec3_t c)
{
	c[0] = a[0] - b[0];
	c[1] = a[1] - b[1];
	c[2] = a[2] - b[2];
}

_inline void VectorAdd(vec3_t a, vec3_t b, vec3_t c)
{
	c[0] = a[0] + b[0];
	c[1] = a[1] + b[1];
	c[2] = a[2] + b[2];
}

_inline void VectorMakePos(vec3_t in, vec3_t out)
{
	out[0] = fabs(in[0]);
	out[1] = fabs(in[1]);
	out[2] = fabs(in[2]);
}

_inline void VectorCopy(vec3_t in, vec3_t out)
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}

__inline vec_t DotProduct (const vec3_t v1, const vec3_t v2)
{
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

_inline void CrossProduct (vec3_t v1, vec3_t v2, vec3_t cross)
{
	cross[0] = v1[1]*v2[2] - v1[2]*v2[1];
	cross[1] = v1[2]*v2[0] - v1[0]*v2[2];
	cross[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

_inline vec_t VectorLengthSquared(vec3_t v)
{
	float	length;
	
	length = DotProduct(v, v);

	return length;
}

_inline void VectorClear (vec3_t in)
{
	in[0] = 0;
	in[1] = 0;
	in[2] = 0;
}

_inline void VectorSet (vec3_t in, vec_t x, vec_t y, vec_t z)
{
	in[0] = x;
	in[1] = y;
	in[2] = z;
}

_inline void VectorInverse (vec3_t v)
{
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}

_inline void VectorNegate (vec3_t in, vec3_t out)
{
	out[0] = -in[0];
	out[1] = -in[1];
	out[2] = -in[2];
}

_inline void VectorScale (vec3_t in, vec_t scale, vec3_t out)
{
	out[0] = in[0] * scale;
	out[1] = in[1] * scale;
	out[2] = in[2] * scale;
}

_inline void VectorRadiansToDegrees (vec3_t in, vec3_t out)
{
	out[0] = in[0] * RADTODEG;
	out[1] = in[1] * RADTODEG;
	out[2] = in[2] * RADTODEG;
}

_inline void VectorDegreesToRadians (vec3_t in, vec3_t out)
{
	out[0] = in[0] * DEGTORAD;
	out[1] = in[1] * DEGTORAD;
	out[2] = in[2] * DEGTORAD;
}

_inline void VectorScaleByVector (vec3_t in, vec3_t scale, vec3_t out)
{
	out[0] = in[0] * scale[0];
	out[1] = in[1] * scale[1];
	out[2] = in[2] * scale[2];
}

_inline void Vec3SubtractAssign(vec3_t value, vec3_t subFrom)
{
	subFrom[0] -= value[0];
	subFrom[1] -= value[1];
	subFrom[2] -= value[2];
}

_inline void Vec3AddAssign(vec3_t value, vec3_t addTo)
{
	addTo[0] += value[0];
	addTo[1] += value[1];
	addTo[2] += value[2];
}

_inline void Vec3MultAssign(vec3_t value, vec3_t multBy)
{
	multBy[0] *= value[0];
	multBy[1] *= value[1];
	multBy[2] *= value[2];
}

_inline void Vec3ScaleAssign(vec_t value, vec3_t scaleBy)
{
	scaleBy[0] *= value;
	scaleBy[1] *= value;
	scaleBy[2] *= value;
}

_inline VectorAverage(vec3_t mins, vec3_t maxs, vec3_t average)
{
	average[0] = (mins[0] + maxs[0]) * 0.5f;
	average[1] = (mins[1] + maxs[1]) * 0.5f;
	average[2] = (mins[2] + maxs[2]) * 0.5f;
}

_inline bool FloatIsZeroEpsilon(float f)
{
	return (fabs(f) < FLOAT_ZERO_EPSILON);
}

_inline bool FloatIsZero(float f, float epsilon)
{
	return (fabs(f) < epsilon);
}

_inline bool Vec3EqualsEpsilon(vec3_t v1, vec3_t v2)
{
	if(!FloatIsZeroEpsilon(v1[0] - v2[0]) || !FloatIsZeroEpsilon(v1[1] - v2[1]) || !FloatIsZeroEpsilon(v1[2] - v2[2]))
	{
		return false;
	}
	return true;
}

_inline bool Vec3IsZero(vec3_t vec)
{
	return !( vec[0] != 0.0 || vec[1] != 0.0 || vec[2] != 0.0 );
}

_inline bool Vec3NotZero(vec3_t vec)
{
	return ( vec[0] != 0.0 || vec[1] != 0.0 || vec[2] != 0.0 );
}

_inline bool VectorCompare (vec3_t v1, vec3_t v2)
{
	if (v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2])
	{
		return(false);
	}
	return(true);
}

_inline void Vector_GetHalfWayPoint(vec3_t start, vec3_t end, vec3_t halfway)
{
	halfway[0] = ((end[0] - start[0])/2) + start[0];
	halfway[1] = ((end[1] - start[1])/2) + start[1];
	halfway[2] = ((end[2] - start[2])/2) + start[2];
}

_inline void VectorMA (vec3_t veca, float scale, vec3_t vecb, vec3_t vecc)
{
	vecc[0] = veca[0] + scale * vecb[0];
	vecc[1] = veca[1] + scale * vecb[1];
	vecc[2] = veca[2] + scale * vecb[2];
}

float flrand(float min, float max);
int irand(int min, int max);

void ClearBounds (vec3_t mins, vec3_t maxs);
void AddPointToBounds (vec3_t v, vec3_t mins, vec3_t maxs);
vec_t VectorLength (vec3_t v);
// void CrossProduct (vec3_t v1, vec3_t v2, vec3_t cross);
vec_t VectorNormalize (vec3_t v);		// returns vector length	
vec_t VectorSeparation(vec3_t v0, vec3_t v1);

void R_ConcatRotations (float in1[3][3], float in2[3][3], float out[3][3]);
void R_ConcatTransforms (float in1[3][4], float in2[3][4], float out[3][4]);

void AngleVectors (vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
int BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, struct cplane_s *plane);
float NormalizeAngle(float angle);

_inline float LerpAngle (float a2, float a1, float frac)
{
	if (a1 - a2 > 180)
	{
		a1 -= 360;
	}
	else if (a1 - a2 < -180)
	{
		a1 += 360;
	}

	return a2 + frac * (a1 - a2);
}

_inline float	anglemod(float a)
{
	return (360.0/65536) * ((int)(a*(65536/360.0)) & 65535);
}

_inline int minimum(int x, int y)
{
	if(x < y)
	{
		return(x);
	}
	else
	{
		return(y);
	}
}

_inline int maximum(int x, int y)
{
	if(x > y)
	{
		return(x);
	}
	else
	{
		return(y);
	}
}

#define BOX_ON_PLANE_SIDE(emins, emaxs, p)	\
	(((p)->type < 3)?						\
	(										\
		((p)->dist <= (emins)[(p)->type])?	\
			1								\
		:									\
		(									\
			((p)->dist >= (emaxs)[(p)->type])?\
				2							\
			:								\
				3							\
		)									\
	)										\
	:										\
		BoxOnPlaneSide( (emins), (emaxs), (p)))


void ProjectPointOnPlane( vec3_t dst, const vec3_t p, const vec3_t normal );
void PerpendicularVector( vec3_t dst, const vec3_t src );
void RotatePointAroundVector( vec3_t dst, const vec3_t dir, const vec3_t point, float degrees );

// end