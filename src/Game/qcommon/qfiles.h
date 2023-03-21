#include "palette.h"
//
// qfiles.h: quake file formats
// This file must be identical in the quake and utils directories
//

/*
==============================================================================

  .M8 texture file format

==============================================================================
*/

#define MIP_VERSION		2
#define PAL_SIZE		256
#define	MIPLEVELS		16

typedef struct miptex_s
{
	int				version;
	char			name[32];
	unsigned		width[MIPLEVELS], height[MIPLEVELS];
	unsigned		offsets[MIPLEVELS];		// four mip maps stored
	char			animname[32];			// next frame in animation chain
	paletteRGB_t	palette[PAL_SIZE];
	int				flags;
	int				contents;
	int				value;
} miptex_t;

#define MIP32_VERSION	4

// flags2 definitions
#define MIP32_NOMIP_FLAG2			0x00000001
#define MIP32_DETAILER_FLAG2		0x00000002
#define MIP32_PARENTAL_FLAG2		0x00000004
#define MIP32_SPHERICAL_FLAG2		0x00000008

typedef struct miptex32_s
{
	int			version;
	char		name[MAX_OSPATH];
	char		altname[MAX_OSPATH];			// texture substitution
	char		animname[MAX_OSPATH];			// next frame in animation chain
	char		damagename[MAX_OSPATH];		// image that should be shown when damaged
	unsigned	width[MIPLEVELS], height[MIPLEVELS];
	unsigned	offsets[MIPLEVELS];		
	int			flags;
	int			contents;
	int			value;
	float		scale_x, scale_y;
	int			mip_scale;

	// detail texturing info
	char		dt_name[MAX_OSPATH];		// detailed texture name
	float		dt_scale_x, dt_scale_y;
	float		dt_u, dt_v;
	float		dt_alpha;
	int			dt_src_blend_mode, dt_dst_blend_mode;

	int			flags2;
	float		damage_health;

	int			unused[18];				// future expansion to maintain compatibility with h2
} miptex32_t;


/*
==============================================================================

  .BSP file format

==============================================================================
*/

#define IDBSPHEADER	(('P'<<24)+('S'<<16)+('B'<<8)+'I')
		// little-endian "IBSP"

// differs from 44 in the extra lightmip fields in struct dface_t -slc
#define BSPVERSION	46


// upper design bounds
// leaffaces, leafbrushes, planes, and verts are still bounded by
// 16 bit short limits

// If any lump has more than 4 million entries, then we have a problem :)
#define MAX_MAP_ANYTHING	(4 * 1024 * 1024)

#define	MAX_MAP_MODELS		1024
#define	MAX_MAP_BRUSHES		8192
#define	MAX_MAP_ENTITIES	2048
#define	MAX_MAP_ENTSTRING	0x40000
#define	MAX_MAP_TEXINFO		8192

#define	MAX_MAP_AREAS		256
#define	MAX_MAP_AREAPORTALS	1024
#define	MAX_MAP_PLANES		65536
#define	MAX_MAP_NODES		65536
#define	MAX_MAP_BRUSHSIDES	65536
#define	MAX_MAP_LEAFS		65536
#define	MAX_MAP_VERTS		65536
#define	MAX_MAP_FACES		65536
#define	MAX_MAP_LEAFFACES	65536
#define	MAX_MAP_LEAFBRUSHES 65536
#define	MAX_MAP_PORTALS		65536
#define	MAX_MAP_EDGES		128000
#define	MAX_MAP_SURFEDGES	256000
#define	MAX_MAP_VISIBILITY	0x180000

// key / value pair sizes

#define	MAX_KEY		32
#define	MAX_VALUE	1024

//=============================================================================

typedef struct
{
	int		fileofs, filelen;
} lump_t;

#define	LUMP_ENTITIES		0
#define	LUMP_PLANES			1
#define	LUMP_VERTEXES		2
#define	LUMP_VISIBILITY		3
#define	LUMP_NODES			4
#define	LUMP_TEXINFO		5
#define	LUMP_FACES			6
#define	LUMP_LIGHTING		7
#define	LUMP_LEAFS			8
#define	LUMP_LEAFFACES		9
#define	LUMP_LEAFBRUSHES	10
#define	LUMP_EDGES			11
#define	LUMP_SURFEDGES		12
#define	LUMP_MODELS			13
#define	LUMP_BRUSHES		14
#define	LUMP_BRUSHSIDES		15
#define	LUMP_POP			16
#define	LUMP_AREAS			17
#define	LUMP_AREAPORTALS	18
#define LUMP_REGIONFACES	19
#define LUMP_LIGHTS			20
#define LUMP_REGIONS		21
#define	HEADER_LUMPS		22

typedef struct
{
	int			ident;
	int			version;	
	lump_t		lumps[HEADER_LUMPS];
} dheader_t;

typedef struct bsplumps
{
	void		*in_addr;
	void		*out_addr;
	int			count;
	int			min;
	int			max;
	int			additional;
	char		*lumpname;
	int			insize;
	int			outsize;
	void		(*Process)(void *in, void *oout, int count, void (*callback)(void *cm_out, void *cm_in) = NULL);
	bool		checksummed;
	byte		checksum;
} bsplumps_t;

typedef struct
{
	float		mins[3], maxs[3];
	float		origin[3];		// for sounds or lights
	int			headnode;
	int			firstface, numfaces;	// submodels just draw faces
										// without walking the bsp tree
} dmodel_t;


typedef struct
{
	float	point[3];
} dvertex_t;


// 0-2 are axial planes
#define	PLANE_X			0
#define	PLANE_Y			1
#define	PLANE_Z			2

// 3-5 are non-axial planes snapped to the nearest
#define	PLANE_ANYX		3
#define	PLANE_ANYY		4
#define	PLANE_ANYZ		5

// planes (x&~1) and (x&~1)+1 are allways opposites

typedef struct
{
	float	normal[3];
	float	dist;
	int		type;		// PLANE_X - PLANE_ANYZ ?remove? trivial to regenerate
} dplane_t;

typedef struct
{
	int			planenum;
	int			children[2];	// negative numbers are -(leafs+1), not nodes
	short		mins[3];		// for frustom culling
	short		maxs[3];
	unsigned short	firstface;
	unsigned short	numfaces;	// counting both sides
} dnode_t;


typedef struct texinfo_s
{
	float		vecs[2][4];		// [s/t][xyz offset]
	int			flags;			// miptex flags + overrides
	int			value;			// light emission, etc
	char		texture[32];	// texture name (textures/*.wal)
	int			nexttexinfo;	// for animations, -1 = end of chain
} texinfo_t;


// note that edge 0 is never used, because negative edge nums are used for
// counterclockwise use of the edge in a face
typedef struct
{
	unsigned short	v[2];		// vertex numbers
} dedge_t;

typedef struct
{
	unsigned short	planenum;
	short		side;

	int			firstedge;		// we must support > 64k edges
	short		numedges;	
	short		texinfo;

	short		region;
	int			first_regionface;
	short		num_regionfaces;

	byte		styles[MAXLIGHTMAPS];

	int			lightofs;		// start of [numstyles*surfsize] samples
	byte		lightmip_smax,lightmip_tmax,lightmip_X,lightmip_Y;	// int lightmip;

	short		texturemins[2];
	short		extents[2];
} dface_t;

typedef struct
{
	int					firstedge;
	short				numedges;	
} dregionface_t;

typedef struct
{
	int				contents;			// OR of all brushes (not needed?)

	short			cluster;
	short			area;
	short			region;

	short			mins[3];			// for frustum culling
	short			maxs[3];

	unsigned short	firstleafface;
	unsigned short	numleaffaces;

	unsigned short	firstleafbrush;
	unsigned short	numleafbrushes;
} dleaf_t;

typedef struct
{
	unsigned short	planenum;		// facing out of the leaf
	short	texinfo;
} dbrushside_t;

typedef struct
{
	int			firstside;
	int			numsides;
	int			contents;
} dbrush_t;

#define	ANGLE_UP	-1
#define	ANGLE_DOWN	-2


// the visibility lump consists of a header with a count, then
// byte offsets for the PVS and PHS of each cluster, then the raw
// compressed bit vectors
#define	DVIS_PVS	0
#define	DVIS_PHS	1
typedef struct
{
	int			numclusters;
	int			bitofs[8][2];	// bitofs[numclusters][2]
} dvis_t;

// each area has a list of portals that lead into other areas
// when portals are closed, other areas may not be visible or
// hearable even if the vis info says that it should be
typedef struct
{
	int		portalnum;
	int		otherarea;
} dareaportal_t;

typedef struct
{
	int		numareaportals;
	int		firstareaportal;
} darea_t;

typedef struct
{
	int			connecting_face;
	int			height_change;
	vec3_t		points[2];
} dpathpoint_t;

typedef struct
{
	int			start_point;
	int			num_points;
	vec3_t		center;
	float		radius;
} dpathinfo_t;

typedef enum
{
	emit_surface,
	emit_point,
	emit_spotlight,
	emit_sunlight	// Argh! - sunlight
} emittype_t;

typedef struct
{
	vec3_t		origin;
	vec3_t		color;
	float		intensity;
	emittype_t	type;
	int			falloff;	// falloff type: 0=linear, 1=inverse, 2=invsquare
} light_t;

// Fog Flags
#define FOG_PULSATE		0x00000001
#define FOG_HEIGHT		0x00000002

typedef struct
{
	union
	{
		struct
		{
			int				mode;
			float			density;
			float			density_range;
			float			start;
			float			end;
			float			height;
			byte			color[4];
			unsigned long	flags;
			float			time;
			float			distance_cull;
			float			chop_size;
			float			expansion[8];
		} fog;
	};
} dregion_t;
