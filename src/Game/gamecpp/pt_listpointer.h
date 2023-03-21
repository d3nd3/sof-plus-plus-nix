
class action_c;
class decision_c;
class sense_c;
class body_c;
class ai_public_c;
class ggOinstC;
class ggBinstC;
class ggObjC;

class	scripted_decision;

void DynListWrite();
void DynListRead();

typedef struct classcode_s
{
	int		uuid;
	int		code;
} classcode_t;

//couldn't make this a static in a template class.
extern int ptListTCNextUUID;

template <class T>
class ptListTC
{
private:
	int NewUUID() {return ptListTCNextUUID++;}
	void LoadUUID(int uuid) {if (ptListTCNextUUID<=uuid) ptListTCNextUUID=uuid+1;} // prevent new thing from conflicting with loaded things.

	map<T *,int> Index;
	map<int,T *> Pointer;
public:
	int		GetIndexFromPointer(T *thing)
			{
				/*map<T *,int>::iterator	f;

				if (!thing)
				{
					return(0);
				}
				f = Index.find(thing);
				if (f == Index.end())
				{
					return(0);
				}
				return((*f).second);
				*/
				return 1;
			}
	T		*GetPointerFromIndex(int index)
			{
				
				/*map<int,T *>::iterator	f;

				if (!index)
				{
					return(NULL);
				}
				f = Pointer.find(index);
				if (f == Pointer.end())
				{
					return(NULL);
				}
				return((*f).second);
				*/
				return NULL;
			}
	void	AddElement(T *thing)
			{
				/*
				map<T *,int>::iterator	f;
				int						idx;

				if (!thing)
				{
					return;
				}
				f = Index.find(thing);
				if (f == Index.end())
				{
					idx = NewUUID();
					Index[thing] = idx;
					Pointer[idx] = thing;
				}
				*/
			}
	void	RemoveElement(T *thing)
			{
				/*
				map<T *,int>::iterator	f;
				map<int,T *>::iterator	ff;

				if (!thing)
				{
					return;
				}
				f = Index.find(thing);
				if (f == Index.end())
				{
					return;
				}
				ff = Pointer.find((*f).second);
				assert(ff != Pointer.end()); // maps are corrupted, they should always correspond.
				Pointer.erase(ff);
				Index.erase(f);
				delete thing;
				*/
			}

	void	Write1();
	void	Write2();
	void	Read1();
	void	Read2();
};

extern ptListTC<action_c> actionL;
extern ptListTC<decision_c> decisionL;
extern ptListTC<sense_c> senseL;
extern ptListTC<body_c> bodyL;
extern ptListTC<ai_public_c> aiL;

//ghoul stuff:
extern ptListTC<ggOinstC> instL;
extern ptListTC<ggBinstC> binstL;
extern ptListTC<ggObjC> objL;

//use these wherever you need a pointer and you need to keep track of what's pointed to.  they should be used to maintain a list for savegame stuff.

//	requirements for a class being able to used by ptrTC as T:
//		needs to have member functions int GetClassCode(void) and T* NewClassForCode(int);
//		--they allow saving and loading child classes. if no child classes exist, it doesn't matter
//		what getclasscode returns, newclassforcode should always return the base class.
//		if there are children, the parent and all the children should return a unique class
//		code, and newclassforcode should allocate a new parent, or child, class based on that.
//	
//		also need Save and Load funcs
//
//		then, need to make a content list of the class, and define your ptrTC
//		and add the list to the list saving and loading in listpointer.cpp

template <class T, ptListTC<T> *ptlist>
class ptrTC
{
protected:
	T *this_pointed;
public:
	//constructors are for initialization, and for conversion, so you can use action_c_ptr as an argument, and it'll work just like a normal pointer--can pass in NULL, or &<some action_c>
	ptrTC(void){this_pointed = NULL;}
	//update list on this constructor
	ptrTC(const T *a_pointer){this_pointed = (T *)a_pointer; ptlist->AddElement((T *)this_pointed);}

	//assignments--again, works just like a pointer--update list here
	const	ptrTC &operator= (const T* new_action) {this_pointed = (T *)new_action; ptlist->AddElement((T *)this_pointed); return *this;}

	//so you can access this like, uh, a pointer...
			T *operator-> () {return this_pointed;}

	//this is a better way to make me work in logic statements--just treat me exactly like a pointer!
			operator T*() {return this_pointed;}

	//this used in place of delete for getting rid of what the pointer is pointing to. update list
			void Destroy(void){if (this_pointed) ptlist->RemoveElement(this_pointed); this_pointed = NULL;}

			void MakeIndex(T *orig) { *(int *)&this_pointed = ptlist->GetIndexFromPointer(orig); }
			void MakeIndex() { *(int *)&this_pointed = ptlist->GetIndexFromPointer(this_pointed); }
			int GetIndex() { return(ptlist->GetIndexFromPointer(this_pointed)); }
//			void MakePtr() { this_pointed = ptlist->GetPointerFromIndex((int)this_pointed); }
			void MakePtr(int idx) { this_pointed = ptlist->GetPointerFromIndex(idx); }
			T *GetPtr(int i) { ptlist->GetPointerFromIndex(i); }
};

#define sense_c_ptr ptrTC<sense_c,&senseL>
#define decision_c_ptr ptrTC<decision_c,&decisionL>
#define action_c_ptr ptrTC<action_c,&actionL>
#define ai_c_ptr ptrTC<ai_public_c,&aiL>

#define gg_inst_c_ptr ptrTC<ggOinstC,&instL>
#define gg_binst_c_ptr ptrTC<ggBinstC,&binstL>
#define gg_obj_c_ptr ptrTC<ggObjC,&objL>

#define body_c_ptr ptrTC<body_c,&bodyL>
