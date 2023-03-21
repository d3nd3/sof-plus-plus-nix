#include "g_local.h"
#include "ai_private.h"

extern mmove_t MMoves[];


ptListTC<action_c> actionL;
ptListTC<decision_c> decisionL;
ptListTC<sense_c> senseL;
ptListTC<body_c> bodyL;
ptListTC<ai_public_c> aiL;

//ghoul stuff:
ptListTC<ggOinstC> instL;
ptListTC<ggBinstC> binstL;
ptListTC<ggObjC> objL;

int ptListTCNextUUID = 1;

template <class T>
void ptListTC<T>::Write1()
{
	int						count;
	classcode_t				ccode;
	map<int, T *>::iterator	it;

	count = Pointer.size();
	gi.AppendToSavegame('AICT', &count, sizeof(count));

	for (it = Pointer.begin(); it != Pointer.end(); it++)
	{
		ccode.uuid = (*it).first;
		ccode.code = (*it).second->GetClassCode();
		gi.AppendToSavegame('AICC', &ccode, sizeof(ccode));
	}
}

template <class T>
void ptListTC<T>::Write2()
{
	map<int,T *>::iterator it;

	for (it = Pointer.begin(); it != Pointer.end(); it++)
	{
		(*it).second->Write();
	}
}

//just get the info needed to build my list
template <class T>
void ptListTC<T>::Read1()
{
	int						i, count;
	classcode_t				ccode;
	map<int, T *>::iterator	it;

	//make sure the list is empty before loading
	for (it = Pointer.begin(); it != Pointer.end(); it++)
	{
		delete (*it).second;
	}
	Pointer.clear();
	Index.clear();

	gi.ReadFromSavegame('AICT', &count, sizeof(count));
	// read the block
	for (i = 0; i < count; i++)
	{
		gi.ReadFromSavegame('AICC', &ccode, sizeof(ccode));

		T *newT = (T *)T::NewClassForCode(ccode.code);
		LoadUUID(ccode.uuid);

		Index[newT] = ccode.uuid;
		Pointer[ccode.uuid] = newT;
	}
}

//now fill the list
template <class T>
void ptListTC<T>::Read2()
{
	map<int,T *>::iterator	it;

	for (it = Pointer.begin(); it != Pointer.end(); it++)
	{
		(*it).second->Read();
	}
}

// Save out all the chunks

void DynListWrite()
{
	objL.Write1();
	// Dependent on obj
	binstL.Write1();
	// Dependent on binst
	instL.Write1();

	actionL.Write1();
	decisionL.Write1();
	senseL.Write1();
	bodyL.Write1();
	aiL.Write1();


	objL.Write2();
	instL.Write2();
	binstL.Write2();

	actionL.Write2();
	decisionL.Write2();
	senseL.Write2();
	bodyL.Write2();
	aiL.Write2();
}

void DynListRead()
{
	objL.Read1();
	binstL.Read1();
	instL.Read1();

	actionL.Read1();
	decisionL.Read1();
	senseL.Read1();
	bodyL.Read1();
	aiL.Read1();


	objL.Read2();
	instL.Read2();
	binstL.Read2();

	actionL.Read2();
	decisionL.Read2();
	senseL.Read2();
	bodyL.Read2();
	aiL.Read2();
}

// end