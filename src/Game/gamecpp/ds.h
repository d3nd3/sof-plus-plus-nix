#ifndef __DS
#define __DS

#include "pcode.h"
#include "list.h"
#include "fields.h"

class CScript;

enum ScriptConditionT
{ 
	COND_READY,
	COND_COMPLETED, 
	COND_SUSPENDED, 
	COND_WAIT_ALL, 
	COND_WAIT_ANY,
	COND_WAIT_TIME,
};

#define MAX_INDEX		100

#define INSTRUCTION_MAX 1000

//==========================================================================

typedef struct
{
	int		index;
	char	name[VAR_LENGTH];
} saveglobal_t;

class Variable
{
protected:
	char		Name[VAR_LENGTH];
	VariableT	Type;
	int			idx;
public:
						Variable(char *NewName = "", VariableT NewType = TypeUNKNOWN);
						Variable(CScript *Script) {}
	virtual				~Variable() {}
			void		SetIndex(CScript *Script);
			void		GetIndex(CScript *Script);
			char		*GetName(void) { return Name; }
			VariableT	GetType(void) { return Type; }
	virtual int			GetIntValue(void) { return 0; }
	virtual float		GetFloatValue(void) { return 0.0; }
	virtual void		GetVectorValue(vec3_t &VecValue) { VecValue[0] = VecValue[1] = VecValue[2] = 0.0; }
	virtual edict_t		*GetEdictValue(void) { return NULL; }
	virtual char		*GetStringValue(void) { return ""; }
	virtual void		ReadValue(CScript *Script) {}
	virtual void		Debug(CScript *Script);
	virtual void		Signal(edict_t *Which) { }
	virtual void		ClearSignal(void) { }

	virtual	void		Evaluate(CScript *Script, Variable *orig);
	virtual void		Read(CScript *Script) { assert(0); }
	virtual void		Write(CScript *Script) { assert(0); }

	virtual Variable *operator	+(Variable *VI) { return NULL; }
	virtual Variable *operator	-(Variable *VI) { return NULL; }
	virtual Variable *operator	*(Variable *VI) { return NULL; }
	virtual Variable *operator	/(Variable *VI) { return NULL; }
	virtual void	operator	=(Variable *VI) { }
	virtual bool	operator	==(Variable *VI) { return false; }
	virtual bool	operator	!=(Variable *VI) { return false; }
	virtual bool	operator	<(Variable *VI) { return false; }
	virtual bool	operator	<=(Variable *VI) { return false; }
	virtual bool	operator	>(Variable *VI) { return false; }
	virtual bool	operator	>=(Variable *VI) { return false; }
};

//==========================================================================

class IntVar : public Variable
{
protected:
	int	Value;

public:
						IntVar(char *Name = "", int InitValue = 0);
	virtual int			GetIntValue(void) { return Value; }
	virtual float		GetFloatValue(void) { return (float)Value; }
	virtual void		ReadValue(CScript *Script);
	virtual void		Debug(CScript *Script);
	virtual void		Signal(edict_t *Which);
	virtual void		ClearSignal(void);

	virtual	void		Evaluate(CScript *Script, IntVar *orig);
						IntVar(CScript *Script);
	virtual void		Write(CScript *Script);

	virtual Variable *operator	+(Variable *VI);
	virtual Variable *operator	-(Variable *VI);
	virtual Variable *operator	*(Variable *VI);
	virtual Variable *operator	/(Variable *VI);
	virtual void	operator	=(Variable *VI);

	virtual bool	operator	==(Variable *VI) { return Value == VI->GetIntValue(); }
	virtual bool	operator	!=(Variable *VI) { return Value != VI->GetIntValue(); }
	virtual bool	operator	<(Variable *VI) { return Value < VI->GetIntValue(); }
	virtual bool	operator	<=(Variable *VI) { return Value <= VI->GetIntValue(); }
	virtual bool	operator	>(Variable *VI) { return Value > VI->GetIntValue(); }
	virtual bool	operator	>=(Variable *VI) { return Value >= VI->GetIntValue(); }
};

//==========================================================================

class FloatVar : public Variable
{
protected:
	float	Value;

public:
						FloatVar(char *Name = "", float InitValue = 0.0);
	virtual int			GetIntValue(void) { return (int)Value; }
	virtual float		GetFloatValue(void) { return Value; }
	virtual void		ReadValue(CScript *Script);
	virtual void		Debug(CScript *Script);

	virtual	void		Evaluate(CScript *Script, FloatVar *orig);
						FloatVar(CScript *Script);
	virtual void		Write(CScript *Script);

	virtual Variable *operator	+(Variable *VI);
	virtual Variable *operator	-(Variable *VI);
	virtual Variable *operator	*(Variable *VI);
	virtual Variable *operator	/(Variable *VI);
	virtual void	operator	=(Variable *VI);

	virtual bool	operator	==(Variable *VI) { return Value == VI->GetFloatValue(); }
	virtual bool	operator	!=(Variable *VI) { return Value != VI->GetFloatValue(); }
	virtual bool	operator	<(Variable *VI) { return Value < VI->GetFloatValue(); }
	virtual bool	operator	<=(Variable *VI) { return Value <= VI->GetFloatValue(); }
	virtual bool	operator	>(Variable *VI) { return Value > VI->GetFloatValue(); }
	virtual bool	operator	>=(Variable *VI) { return Value >= VI->GetFloatValue(); }
};

//==========================================================================

class VectorVar : public Variable
{
protected:
	vec3_t	Value;

public:
						VectorVar(char *Name = "", float InitValueX = 0.0, float InitValueY = 0.0, float InitValueZ = 0.0);
						VectorVar(vec3_t NewValue);
	virtual void		GetVectorValue(vec3_t &VecValue);
	virtual void		ReadValue(CScript *Script);
	virtual void		Debug(CScript *Script);

	virtual	void		Evaluate(CScript *Script, VectorVar *orig);
						VectorVar(CScript *Script);
	virtual void		Write(CScript *Script);

	virtual Variable *operator	+(Variable *VI);
	virtual Variable *operator	-(Variable *VI);
	virtual Variable *operator	*(Variable *VI);
	virtual Variable *operator	/(Variable *VI);
	virtual void	operator	=(Variable *VI);
	virtual bool	operator	==(Variable *VI);
	virtual bool	operator	!=(Variable *VI);
	virtual bool	operator	<(Variable *VI);
	virtual bool	operator	<=(Variable *VI);
	virtual bool	operator	>(Variable *VI);
	virtual bool	operator	>=(Variable *VI);
};

//==========================================================================

class EntityVar : public Variable
{
protected:
	edict_t *Value;

public:
						EntityVar(char *Name = "", int InitValue = 0);
						EntityVar(edict_t *Which);
	virtual int			GetIntValue(void);
	virtual edict_t		*GetEdictValue(void) { return Value; }
	virtual void		ReadValue(CScript *Script);
	virtual void		Debug(CScript *Script);

	virtual	void		Evaluate(CScript *Script, EntityVar *orig);
						EntityVar(CScript *Script);
	virtual void		Write(CScript *Script);

	virtual void	operator	=(Variable *VI);
	virtual bool	operator	==(Variable *VI);
	virtual bool	operator	!=(Variable *VI);
};

//==========================================================================

class StringVar : public Variable
{
protected:
	char Value[VAR_LENGTH];

public:
						StringVar(char *Name = "", char *InitValue = "");
	virtual char		*GetStringValue(void) { return Value; }
	virtual void		ReadValue(CScript *Script);

	virtual	void		Evaluate(CScript *Script, StringVar *orig);
						StringVar(CScript *Script);
	virtual void		Write(CScript *Script);
};

//==========================================================================

class VariableVar : public Variable
{
protected:
	Variable		*Value;

public:
						VariableVar(char *Name = "");
	virtual int			GetIntValue(void) { return Value->GetIntValue(); }
	virtual float		GetFloatValue(void) { return Value->GetFloatValue(); }
	virtual void		GetVectorValue(vec3_t &VecValue) { Value->GetVectorValue(VecValue); }
	virtual edict_t		*GetEdictValue(void) { return Value->GetEdictValue(); }
	virtual char		*GetStringValue(void) { return Value->GetStringValue(); }
	virtual void		ReadValue(CScript *Script);
	virtual void		Debug(CScript *Script);
	virtual void		Signal(edict_t *Which) { Value->Signal(Which); }
	virtual void		ClearSignal(void) { Value->ClearSignal(); }

	virtual	void		Evaluate(CScript *Script, VariableVar *orig);
						VariableVar(CScript *Script);
	virtual void		Write(CScript *Script);

	virtual Variable *operator	+(Variable *VI) { return (*Value) + VI; }
	virtual Variable *operator	-(Variable *VI) { return (*Value) - VI; }
	virtual Variable *operator	*(Variable *VI) { return (*Value) * VI; }
	virtual Variable *operator	/(Variable *VI) { return (*Value) / VI; }
	virtual void	operator	=(Variable *VI) { (*Value) = VI; }

	virtual bool	operator	==(Variable *VI) { return (*Value) == VI; }
	virtual bool	operator	!=(Variable *VI) { return (*Value) != VI; }
	virtual bool	operator	<(Variable *VI) { return (*Value) < VI; }
	virtual bool	operator	<=(Variable *VI) { return (*Value) <= VI; }
	virtual bool	operator	>(Variable *VI) { return (*Value) > VI; }
	virtual bool	operator	>=(Variable *VI) { return (*Value) >= VI; }
};

//==========================================================================

class FieldDef;

class FieldVariableVar : public Variable
{
protected:
	Variable		*Value;
	FieldDef		*Field;
public:
						FieldVariableVar(char *Name = "");
	virtual int			GetIntValue(void);
	virtual float		GetFloatValue(void);
	virtual void		GetVectorValue(vec3_t &VecValue);
	virtual edict_t		*GetEdictValue(void);
	virtual char		*GetStringValue(void);
	virtual void		ReadValue(CScript *Script);
	virtual void		Debug(CScript *Script);
	virtual void		Signal(edict_t *Which) { Value->Signal(Which); }
	virtual void		ClearSignal(void) { Value->ClearSignal(); }

	virtual	void		Evaluate(CScript *Script, FieldVariableVar *orig);
						FieldVariableVar(CScript *Script);
	virtual void		Write(CScript *Script);

	virtual Variable *operator	+(Variable *VI);
	virtual Variable *operator	-(Variable *VI);
	virtual Variable *operator	*(Variable *VI);
	virtual Variable *operator	/(Variable *VI);
	virtual void	operator	=(Variable *VI);

	virtual bool	operator	==(Variable *VI);
	virtual bool	operator	!=(Variable *VI);
	virtual bool	operator	<(Variable *VI);
	virtual bool	operator	<=(Variable *VI);
	virtual bool	operator	>(Variable *VI);
	virtual bool	operator	>=(Variable *VI);
};

//==========================================================================

enum SignalT
{
	SIGNAL_MOVE,
	SIGNAL_ROTATE,
	SIGNAL_ANIMATE,
	SIGNAL_MOVEROTATE,
	SIGNAL_HELI,
	SIGNAL_TANK
};

class Signaler
{
private:
	edict_t		*Edict;
	Variable	*Var;
	SignalT		SignalType;

public:
								Signaler() {}
								Signaler(edict_t *NewEdict, Variable *NewVar, SignalT NewSignalType);
								~Signaler(void);
			bool				Test(edict_t *Which, SignalT WhichType);
			edict_t				*GetEdict(void) { return Edict; }
			Variable			*GetVar(void) { return Var; }
			SignalT				GetType(void) { return SignalType; }
			bool	operator	==(Signaler *SI);

	virtual	void		Evaluate(CScript *Script, Signaler *orig);
						Signaler(CScript *Script);
	virtual void		Write(CScript *Script);
};

//==========================================================================

class FieldDef
{
private:
	char			Name[VAR_LENGTH];
	VariableT		Type;
	int				Offset;
	fieldtype_t		FieldType;
	int				idx;

public:
						FieldDef() {}
						FieldDef(CScript *Script);
			byte		*GetOffset(Variable *Var);
			Variable	*GetValue(Variable *Var);
			int			GetIntValue(Variable *Var);
			float		GetFloatValue(Variable *Var);
			void		GetVectorValue(Variable *Var, vec3_t &VecValue);
			edict_t		*GetEdictValue(Variable *Var);
			char		*GetStringValue(Variable *Var);
			void		SetValue(Variable *Var, Variable *Value);

	virtual	void		Evaluate(CScript *Script, FieldDef *orig);
						FieldDef(CScript *Script, bool loading);
	virtual void		Write(CScript *Script);
};

//==========================================================================

enum EventT
{
	EVENT_MOVE_DONE,
	EVENT_ROTATE_DONE,
	EVENT_SCRIPT_WAIT,
	EVENT_SCRIPT_EXECUTE,
	EVENT_MOVEROTATE,
	EVENT_ANIMATE_DONE,
	EVENT_HELICOPTER_DONE,
	EVENT_TANK_DONE
};

class Event
{
protected:
	float		Time;
	EventT		Type;
	int			Priority;

public:
						Event() {}
						Event(float NewTime, EventT NewType);
	virtual				~Event() {}
			float		GetTime(void) { return Time; }
			EventT		GetType(void) { return Type; }
			int			GetPriority(void) { return Priority; }
	virtual bool		Process(CScript *Script);
	virtual void		SetEventState(qboolean done){};

	virtual	void		Evaluate(CScript *Script, Event *orig);
						Event(CScript *Script) { assert(0); }
	virtual void		Write(CScript *Script) { assert(0); }
};
//==========================================================================

class AnimateDoneEvent : public Event
{
private:
	
	qboolean	Done;
	edict_t		*Ent;

public:
						AnimateDoneEvent() {}
						AnimateDoneEvent(edict_t *ent);
	virtual bool		Process(CScript *Script);
	virtual void		SetEventState(qboolean done){Done = done;};

	virtual	void		Evaluate(CScript *Script, AnimateDoneEvent *orig);
						AnimateDoneEvent(CScript *Script);
	virtual void		Write(CScript *Script);
};

//==========================================================================

class MoveDoneEvent : public Event
{
private:
	edict_t	*Ent;

public:
						MoveDoneEvent() {}
						MoveDoneEvent(float NewTime, edict_t *NewEnt);
	virtual bool		Process(CScript *Script);

	virtual	void		Evaluate(CScript *Script, MoveDoneEvent *orig);
						MoveDoneEvent(CScript *Script);
	virtual void		Write(CScript *Script);
};

//==========================================================================

class RotateDoneEvent : public Event
{
private:
	edict_t	*Ent;

public:
						RotateDoneEvent() {}
						RotateDoneEvent(float NewTime, edict_t *NewEnt);
	virtual bool		Process(CScript *Script);

	virtual	void		Evaluate(CScript *Script, RotateDoneEvent *orig);
						RotateDoneEvent(CScript *Script);
	virtual void		Write(CScript *Script);
};

//==========================================================================

class HelicopterDoneEvent : public Event
{
private:
	edict_t	*Ent;
	int		m_ID;

public:
						HelicopterDoneEvent() {}
						HelicopterDoneEvent(float NewTime, edict_t *NewEnt, int actionID);
	virtual bool		Process(CScript *Script);

	virtual	void		Evaluate(CScript *Script, HelicopterDoneEvent *orig);
						HelicopterDoneEvent(CScript *Script);
	virtual void		Write(CScript *Script);
};

//==========================================================================

class TankDoneEvent : public Event
{
private:
	edict_t	*Ent;
	int		m_ID;

public:
						TankDoneEvent() {}
						TankDoneEvent(float NewTime, edict_t *NewEnt, int actionID);
	virtual bool		Process(CScript *Script);

	virtual	void		Evaluate(CScript *Script, TankDoneEvent *orig);
						TankDoneEvent(CScript *Script);
	virtual void		Write(CScript *Script);
};

//==========================================================================

class MoveRotateEvent : public Event
{
private:
	edict_t	*Ent;
	vec3_t v3RelativeOrigin, v3RelativeAngles;

public:
						MoveRotateEvent() {}
						MoveRotateEvent(float NewTime, edict_t *NewEnt, vec3_t v3RelOrigin, vec3_t v3RelAngles);
	virtual bool		Process(CScript *Script);

	virtual	void		Evaluate(CScript *Script, MoveRotateEvent *orig);
						MoveRotateEvent(CScript *Script);
	virtual void		Write(CScript *Script);
};


//==========================================================================

class ExecuteEvent : public Event
{
private:
	edict_t	*Other;
	edict_t	*Activator;

public:
						ExecuteEvent() {}
						ExecuteEvent(float NewTime, edict_t *NewOther = NULL, edict_t *NewActivator = NULL);
	virtual bool		Process(CScript *Script);

	virtual	void		Evaluate(CScript *Script, ExecuteEvent *orig);
						ExecuteEvent(CScript *Script);
	virtual void		Write(CScript *Script);
};

//==========================================================================

class WaitEvent : public Event
{
public:
						WaitEvent() {}
						WaitEvent(float NewTime);
	virtual bool		Process(CScript *Script);

	virtual	void		Evaluate(CScript *Script, WaitEvent *orig);
						WaitEvent(CScript *Script);
	virtual void		Write(CScript *Script);
};

//==========================================================================

typedef struct 
{
	ScriptConditionT	ScriptCondition;
	int					ConditionInfo;
	int					Length;
	int					Position;
	int					DebugFlags;
	int					owner;
	int					other;
	int					activator;
} savescript_t;

class CScript 
{
private:
	// Saved out separately
	char				Name[MAX_PATH];

	unsigned char		*Data;
	ScriptConditionT	ScriptCondition;
	int					ConditionInfo;
	int					Length;
	int					Position;
	edict_t				*owner;
	edict_t				*other;
	edict_t				*activator;
	int					DebugFlags;

	// Fields below here are not saved out, but reconstituted
	list<Variable *>	LocalVariables;
	list<Variable *>	ParameterVariables;
	list<Variable *>	Stack;
	list<Signaler *>	Signalers;
	list<Variable *>	Waiting;
	list<StringVar *>   ParameterValues;
	list<Event *>		Events;
	list<int>			OnConditions;
	Variable			*VarIndex[MAX_INDEX];
	FieldDef			*Fields[MAX_INDEX];

//	Not needed for save
	bool				ClearEvents;
	int					SavePosition;
	ScriptConditionT	SaveScriptCondition;

public:
						CScript(char *ScriptName, edict_t *new_owner);
						CScript(void);
						~CScript(void);

	void				LoadFile(void);
	void				Free(bool DoData);
	void				Clear(bool DoData);
	void				Write(void);
	int					GetEventNum(Event *ev);
	Event				*GetEventPtr(int ev);

	Variable			*LookupVar(int Index) { return VarIndex[Index]; }
	int					LookupVarIndex(Variable *Var);
	void				SetVarIndex(int Index, Variable *Var) { VarIndex[Index] = Var; }
	FieldDef			*LookupField(int Index) { return Fields[Index]; }
	int					LookupFieldIndex(FieldDef *Field);
	void				SetFieldIndex(int Index, FieldDef *Field) { Fields[Index] = Field; }
	void				SetParameter(char *Value);

	unsigned char		ReadByte(void);
	int					ReadInt(void);
	float				ReadFloat(void);
	char				*ReadString(void);
	Variable			*ReadDeclaration(int &Index);

	void				PushStack(Variable *VI);
	Variable			*PopStack(void);

	void				HandleGlobal(bool Assignment);
	void				HandleLocal(bool Assignment);
	void				HandleParameter(bool Assignment);
	void				HandleField(void);
	void				HandleGoto(void);
	Variable			*HandleSpawn(void);
	Variable			*HandleBuiltinFunction(void);
	void				HandlePush(void);
	void				HandlePop(void);
	void				HandleAssignment(void);
	void				HandleAdd(void);
	void				HandleSubtract(void);
	void				HandleMultiply(void);
	void				HandleDivide(void);
	void				HandleDebug(void);
	void				HandleDebugStatement(void);
	void				HandleAddAssignment(void);
	void				HandleSubtractAssignment(void);
	void				HandleMultiplyAssignment(void);
	void				HandleDivideAssignment(void);
	bool				HandleWait(bool ForAll);
	bool				HandleTimeWait(void);
	bool				HandleIf(bool Reverse = false);
	void				HandleOn(void);
	bool				ProcessOn(int NewPosition);
	void				HandleResume(void);

	void				HandlePrint(void);
	void				HandlePlaySound(void);
	void				HandleUnloadSound(void);
	void				HandleUnloadROFF(void);
	void				HandleFeature(bool Enable);
	void				HandleCacheSound(void);
	void				HandleCacheStringPackage(void);
	void				HandleCacheROFF(void);

	void				HandleMove(void);
	void				HandleRotate(void);
	void				HandleUse(void);
	void				HandleTrigger(bool Enable);
	void				HandleAnimate(void);
	void				HandleResetAI(void);
	void				HandlePlaySong(void);
	void				HandleSetCvar(void);
	void				HandleConsoleCommand(void);
	void				HandleRemove(void);
	void				HandleHelicopter(void);
	void				HandleTank(void);
	void				HandleMoveRotate(void);
	void				HandleCopyPlayerAttributes(void);
	void				HandleSetViewAngles(void);
	void				HandleSetCacheSize(void);

	void				Move_Done(edict_t *ent);
	void				Move(edict_t *ent, vec3_t Dest);
	void				Rotate_Done(edict_t *ent);
	void				Rotate(edict_t *ent);
	void				Helicopter_Done(edict_t *ent);
	void				Helicopter(edict_t *ent, int actionID);
	void				Tank_Done(edict_t *ent);
	void				Tank(edict_t *ent, int actionID);

	Event				*AddEvent(Event *Which);
	void				ProcessEvents(void);
	void				ClearTimeWait(void);

	void				AddSignaler(edict_t *Edict, Variable *Var, SignalT SignalType);
	void				CheckSignalers(edict_t *Which, SignalT SignalType);
	bool				CheckWait(void);
	void				FinishWait(edict_t *Which, bool NoExecute);
	void				Error (char *error, ...);
	void				StartDebug(void);
	void				EndDebug(void);
	void				DebugLine (char *debugtext, ...);

	void				Think(void);
	ScriptConditionT	Execute(edict_t *new_other, edict_t *new_activator);

	Variable			*FindLocal(char *Name);
	bool				NewLocal(Variable *Which);
	Variable			*FindParameter(char *Name);
	bool				NewParameter(Variable *Which);

	void				CallMovementAnimation(int Flags,	ai_public_c* ai,
									scripted_decision *DecisionPointer, vec3_t MovingVal,
									vec3_t TurningVal, bool Absolute, edict_t *ent,
									float speed, int AnimationNumber = -1);

	void				CallAnimation(action_code actCode, int Flags,	ai_public_c* ai,
									scripted_decision *DecisionPointer, vec3_t MovingVal,
									vec3_t TurningVal, bool Absolute, edict_t *ent,
									float speed, float HoldingVal, Event *EventPtr,
									edict_t *TargetEnt, bool ShouldKill, int NullTarget,
									int Emotion, mmove_t *preferredAnim=NULL);
};

//==========================================================================

extern	list<Variable *>	GlobalVariables;
extern	list<CScript *>		Scripts;

#endif
