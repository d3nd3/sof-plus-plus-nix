#pragma once

//
// config strings are a general means of communication from
// the server to all connected clients.
// Each config string can be at most MAX_QPATH characters.
//
#define	CS_NAME				0
#define	CS_CDTRACK			1
#define	CS_SKY				2
#define	CS_SKYAXIS			3		// %f %f %f format
#define	CS_SKYROTATE		4
#define CS_SKYCOLOR			5
#define	CS_MAPCHECKSUM		6		// checksum string

#define CS_CTF_BLUE_STAT	8
#define CS_CTF_RED_STAT		9
#define CS_CTF_BLUE_TEAM   	10
#define CS_CTF_RED_TEAM	   	11
#define CS_AMBSET			12
#define CS_MUSICSET			13
#define CS_TERRAINNAME		14
#define CS_SCREENEFFECT		15
#define CS_DEBRISPRECACHE	16

#define	CS_MAXCLIENTS		31

#define	CS_SHOWNAMES		32
#define	CS_SHOWTEAMS		33
#define	CS_SHOWINFOINDARK	34

#define	CS_MODELS			35

#define	CS_SOUNDS			(CS_MODELS+MAX_MODELS)
#define CS_EFFECTS			(CS_SOUNDS+MAX_SOUNDS)
#define	CS_IMAGES			(CS_EFFECTS+MAX_EFPACKS)
#define	CS_LIGHTS			(CS_IMAGES+MAX_IMAGES)
#define	CS_PLAYERSKINS		(CS_LIGHTS+MAX_LIGHTSTYLES)
#define	CS_PLAYERICONS		(CS_PLAYERSKINS+MAX_CLIENTS)
#define CS_STRING_PACKAGES	(CS_PLAYERICONS+MAX_PLAYERICONS)
#define CS_WELCOME			(CS_STRING_PACKAGES+MAX_STRING_PACKAGES)
#define CS_GHOULFILES		(CS_WELCOME + 4)
#define CS_CONTROL_FLAGS	(CS_GHOULFILES+MAX_GHOULFILES)
#define	MAX_CONFIGSTRINGS	(CS_CONTROL_FLAGS+10)

// end