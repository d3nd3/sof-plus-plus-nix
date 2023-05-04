#include "common.h"


class FloatMemoryItem : public MemoryItem {
public:
	FloatMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"float",offset,info) {}

	PyObject* get_py(void* baseAddress) {
		return PyFloat_FromDouble(*(float*)(baseAddress + offset));
	}
	float get(void* baseAddress) {
		return *(float*)(baseAddress + offset);
	}
	void set(void* baseAddress,float value) {
		*(float*)(baseAddress + offset) = value;
	}
};
class IntMemoryItem : public MemoryItem {
public:
	IntMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"int",offset,info) {}

	PyObject* get_py(void* baseAddress) {
		return PyLong_FromLong(*(int*)(baseAddress + offset));
	}
	int get(void* baseAddress) {
		return *(int*)(baseAddress + offset);
	}
	void set(void* baseAddress,int value) {
		*(int*)(baseAddress + offset) = value;
	}
};
class VectorMemoryItem : public MemoryItem {
public:
	VectorMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"vector",offset,info) {}

	PyObject* get_py(void* baseAddress) {
		vec3_t vec;
		VectorCopy(*(vec3_t*)(baseAddress + offset),vec);
		return Py_BuildValue("(fff)",vec[0],vec[1],vec[2]);
	}
	vec3_t* get(void* baseAddress) {
		return (vec3_t*)(baseAddress + offset);
	}
	void set(void* baseAddress,vec3_t value) {
		VectorCopy(value,*(vec3_t*)(baseAddress + offset));
	}
};
typedef vec_t vec2_t[2];
class Vector2DMemoryItem : public MemoryItem {
public:
	Vector2DMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"vector2d",offset,info) {}

	PyObject* get_py(void* baseAddress) {
		
		return Py_BuildValue("(ff)",*(vec_t*)(baseAddress + offset),*(vec_t*)(baseAddress + offset + sizeof(vec_t)));
	}
	vec2_t* get(void* baseAddress) {
		return (vec2_t*)(baseAddress + offset);
	}
	void set(void* baseAddress, unsigned int offset, vec2_t value) {
		*(vec_t*)((char*)baseAddress + offset) = value[0];
		*(vec_t*)((char*)baseAddress + offset + sizeof(vec_t)) = value[1];
	}
};
class ShortVector2DMemoryItem : public MemoryItem {
public:
	ShortVector2DMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"shortvector2d",offset,info) {}

	PyObject* get_py(void* baseAddress) {
		return Py_BuildValue("(hh)",*(short*)(baseAddress + offset), *(short*)(baseAddress + offset + sizeof(short)));
	}
	short* get(void* baseAddress) {
		return (short*)(baseAddress + offset);
	}
	void set(void* baseAddress,short* value) {
		*(short*)(baseAddress + offset) = value;
	}
};

class StringMemoryItem : public MemoryItem {
public:
	StringMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"string",offset,info) {}
	PyObject* get_py(void * baseAddress) {
		return PyUnicode_FromString((char*)(baseAddress + offset));
	}
	std::string get(void* baseAddress) {
		return (char*)(baseAddress + offset);
	}
	void set(void* baseAddress,std::string value) {
		strlcpy((char*)(baseAddress + offset),value.c_str(),value.length());
	}
};
class PointerMemoryItem : public MemoryItem {
public:
	PointerMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"pointer",offset,info) {}

	PyObject* get_py(void* baseAddress) {
		return PyLong_FromVoidPtr(*(void**)(baseAddress + offset));
	}
	void* get(void* baseAddress) {
		return *(void**)(baseAddress + offset);
	}
	void set(void* baseAddress,void* value) {
		*(void**)(baseAddress + offset) = value;
	}
};
class ShortMemoryItem : public MemoryItem {
public:
	ShortMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"short",offset,info) {}
	PyObject* get_py(void* baseAddress) {
		return PyLong_FromLong(*(short*)(baseAddress + offset));
	}
	short get(void* baseAddress) {
		return *(short*)(baseAddress + offset);
	}
	void set(void* baseAddress,short value) {
		*(short*)(baseAddress + offset) = value;
	}
};
class ByteMemoryItem : public MemoryItem {
public:
	ByteMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"byte",offset,info) {}
	PyObject* get_py(void* baseAddress) {
		return PyLong_FromLong(*(char*)(baseAddress + offset));
	}
	char get(void* baseAddress) {
		return *(char*)(baseAddress + offset);
	}
	void set(void* baseAddress,char value) {
		*(char*)(baseAddress + offset) = value;
	}
};

class ShortVectorMemoryItem : public MemoryItem {
public:
	ShortVectorMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"shortvector",offset,info) {}
	PyObject* get_py(void* baseAddress) {
		return Py_BuildValue("(hhh)",*(short*)(baseAddress + offset), *(short*)(baseAddress + offset + sizeof(short)), *(short*)(baseAddress + offset + sizeof(short) * 2));
	}
	short* get(void* baseAddress) {
		return (short*)(baseAddress + offset);
	}
	void set(void* baseAddress,short* value) {
		*(short*)(baseAddress + offset) = value;
	}
};


static std::unordered_map<std::string, MemoryItem*> MemoryItem::entityProperties;
void MemoryItem::initMemoryItems(void)
{
	//--------------------------------------------------------------------------------------------------------
	// e.entity_state_t.origin
	entityProperties["origin"] = new VectorMemoryItem("origin",4,"3d location of entity.");
	// e.entity_state_t.angles
	entityProperties["angles"] = new VectorMemoryItem("angles",16,"3d orientation of entity.");
	// e.mins
	entityProperties["mins"] = new VectorMemoryItem("mins",284,"mins is used for collision bounding box creation. It represents the lowest corner of the 3d box.");
	// e.maxs
	entityProperties["maxs"] = new VectorMemoryItem("maxs",296,"maxs is used for collision bounding box creation. It represents the highest corner of the 3d box.");
	// e.velocity
	entityProperties["velocity"] = new VectorMemoryItem("velocity",588,"velocity is the speed an entity is moving.");
	// e.pos1
	entityProperties["pos1"] = new VectorMemoryItem("pos1",564,"pos1 is an extra paramter used for weird things, not sure.");

	//--------------------------------------------------------------------------------------------------------
	// e.max_health
	entityProperties["health_max"] = new IntMemoryItem("health_max",752,"maximum health of entity.");
	// e.health
	entityProperties["health"] = new IntMemoryItem("health",748,"current health of entity.");
	// e.solid
	entityProperties["solid"] = new IntMemoryItem("solid",344,"solid is collision with other entities.");
	// e.inuse
	entityProperties["inuse"] = new IntMemoryItem("inuse",120,"inuse is whether an entity has been initiated and ready to use or not.");
	// e.clipmask
	entityProperties["clipmask"] = new IntMemoryItem("clipmask",348,"clipmask determines which surfaces this ent should collide with. 0 dont collide with anything.\nLook in q_shared.h file for numbers to use.");
	// e.movetype
	entityProperties["movetype"] = new IntMemoryItem("movetype",412,"movetype is the mode in which the ent should interact with world and physics.");
	//e.entity_state_t.skinnum;
	entityProperties["skinnum"] = new IntMemoryItem("skinnum",44,"skinnum is either the slot num if its a player or an alternative texture num for some entities.");
	// e.mass
	entityProperties["mass"] = new IntMemoryItem("mass",636,"mass some number used in physics calculations.");
	// e.flags
	entityProperties["flags"] = new IntMemoryItem("flags",416,"flags status information for this ent, not sure.");
	//--------------------------------------------------------------------------------------------------------

	// e.model
	entityProperties["model"] = new StringMemoryItem("model",420,"model is a hint to some entities for which ghoulmodel to load.");
	// e.classname
	entityProperties["classname"] = new StringMemoryItem("classname",436,"classname determines which entity spawn function gets run, thus which type of ent this it.");
	// e.targetname
	entityProperties["targetname"] = new StringMemoryItem("targetname",456,"targetname is used as a label for triggers, so they can become targets of other triggers.");
	// e.target
	entityProperties["target"] = new StringMemoryItem("target",452,"target is used to set which entity/trigger you will activate/use.");
	// e.soundName
	entityProperties["soundname"] = new StringMemoryItem("soundname",488,"soundname a variable used for some events, not sure.");
	//--------------------------------------------------------------------------------------------------------

	// e.Script [CScript*]
	entityProperties["script"] = new PointerMemoryItem("script"	,1108,"script is an instance to ds script file object.");

	// e.ghoulInst [IGhoulInst*]
	entityProperties["ghoulInst"] = new PointerMemoryItem("ghoulInst",	356,"ghoulInst is a ghoul object pointer.");

	// e.enemy
	entityProperties["enemy"] = new PointerMemoryItem("enemy",804,"enemy no idea what this is.");
	//--------------------------------------------------------------------------------------------------------

	// e.stopspeed
	entityProperties["stopspeed"] = new FloatMemoryItem("stopspeed",640,"stopspeed some number used in physics calculations.");
	// e.friction
	entityProperties["friction"] = new FloatMemoryItem("friction",644,"friction is some number used in physics calculations.");
	// e.airresistance
	entityProperties["airresistance"] = new FloatMemoryItem("airresistance",652,"airresistance is some number used in physics caluclations.");
	// e.bouyancy
	entityProperties["bouyancy"] = new FloatMemoryItem("bouyancy",656,"bouyancy is some number used in physics calculations.");
	// e.gravity
	entityProperties["gravity"] = new FloatMemoryItem("gravity",648,"gravity is how fast the entity falls downwards.");
	// e.nextthink
	entityProperties["nextthink"] = new FloatMemoryItem("nextthink",692,"next_think when the entitys think function should run next.");
	//--------------------------------------------------------------------------------------------------------

	/*
	END OF BASIC ENT FIELDS
	*/

	std::unordered_map<std::string, MemoryItem*> gclientProperties;
	//--------------------------------------------------------------------------------------------------------
	//.gclient->player_state_t.gun
	gclientProperties["gunghoulinst"] = new PointerMemoryItem("gunghoulinst",108,"gunghoulinst is the ghoul object pointer for the players gun.");
	//.gclient->player_state_t.viewangles
	gclientProperties["viewangles"] = new VectorMemoryItem("viewangles",28,"viewangles is the players viewing angle. Use sf_sv_vector_set to understand this format.");
	// gclient_t->player_state_t.pmove_state_t.delta_angles[3]
	gclientProperties["delta_angles"] = new ShortVector2DMemoryItem("delta_angles",20,"delta_angles is the players viewing angle. Use sf_sv_vector_set to understand this format.");
	// gclient_t->client_respawn_t.cmd_angles[3]
	gclientProperties["cmd_angles"] = new VectorMemoryItem("cmd_angles",780,"cmd_angles is the players viewing angle. Use sf_sv_vector_set to understand this format.");

	//--------------------------------------------------------------------------------------------------------
	// gclient_t->player_state_t.pmove_state_t.gravity
	// gclientProperties["gravity"] = new MemoryItem("gravity",					"float",18,"gravity is how fast the player falls to the ground.");
	// gclient_t->player_state_t.pmove_state_t.movescale [other movescale is 26]
	gclientProperties["movescale"] = new FloatMemoryItem("movescale",GCLIENT_MOVESCALE,"movescale is how slowed the player's movement is.");
	//--------------------------------------------------------------------------------------------------------
	// gclient_t->client_respawn_t.score
	gclientProperties["score"] = new IntMemoryItem("score",776,"score is the points the player has.");
}
