
//ai_private.h

//ai include file for ai code only

#ifndef _AI_PRIVATE_H_
#define _AI_PRIVATE_H_

#define	jump_movemask	1
#define	fall_movemask	2
#define	step_movemask	4

extern sensedEntInfo_t NULL_sensedEntInfo;

#include "actionids.h"
#include "ai.h"
#include "ai_senses.h"//needed for spawning sounds
#include "ai_actions.h"
#include "ai_body.h"
#include "ai_bodyhuman.h"
#include "ai_bodydekker.h"
#include "ai_bodydog.h"
#include "ai_bodycow.h"
#include "ai_bodynoghoul.h"
#include "m_heliactions.h"
#include "m_snowcatactions.h"
#include "m_tankactions.h"
//#include "enemy_info.h"
#include "ai_decisions.h"

#define BLOWN_PART_FADE	1.0F
#define BODY_PART_MAXLIFE 30.0f


//animation frame flags
#define FRAMEFLAG_FOOTSTEP		0x00000001
#define FRAMEFLAG_ATTACK		0x00000002
#define FRAMEFLAG_SPEAK			0x00000004
#define FRAMEFLAG_BREATHE		0x00000008//steamy breath in winter
#define FRAMEFLAG_DROPINVENTORY	0x00000010
#define FRAMEFLAG_HOLDFRAME		0x00000020
#define FRAMEFLAG_JUMP			0x00000040
#define FRAMEFLAG_OOZE			0x00000080
#define FRAMEFLAG_LAND			0x00000100
#define FRAMEFLAG_MELEE			0x00000200//for the dog's melee attack
#define FRAMEFLAG_THROW			0x00000400//more properly left hand attack but I believe that's always a throw

#define MONSTER_SHOOT_HEIGHT	20

#endif //_AI_PRIVATE_H_