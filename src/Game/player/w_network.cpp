#include "q_shared.h"
#include "..\ghoul\ighoul.h"
#include "w_public.h"
#include "w_types.h"
#include "w_weapons.h"
#include "w_utils.h"
#include "w_network.h"
#include "player.h"
#include "../gamecpp/game.h"
#include "w_equipment.h"
#include "qcommon.h"

#define ARG_SIZE_8		0	// Argument fits in 8 bits.
#define ARG_SIZE_16		1	// Argument fits in 16 bits.
#define ARG_SIZE_24		2	// Argument fits in 24 bits.
#define ARG_SIZE_32		3	// Argument fits in 32 bits.
#define ARG_SIZE_MASK	0x3	// Argument's ARG_SIZE size is encoded in 2 adjacent bits.
#define ARG_SIZE_SHIFT	4

extern player_com_import_t	picom;
extern player_cl_import_t	picl;
extern player_sv_import_t	pisv;

clientRICBuf::clientRICBuf(void)
{
	numRICs=0;
	owner=NULL;
	picl.SZ_Init(&sizebuf,(unsigned char *)sizebufData,sizeof(sizebufData));
}

void clientRICBuf::ClearRICs(void)
{
	numRICs=0;

	picl.SZ_Clear(&sizebuf);

	sizebuf.readcount=0;
}

void HandleArg(int argNum,byte argsFlags,sizebuf_t *sizebuf)
{
	assert((argNum>=1)&&(argNum<=2));

	byte argSize=(argsFlags>>((argNum-1)*2))&ARG_SIZE_MASK;

	switch(argSize)
	{
		case ARG_SIZE_8:
			picl.WriteByteSizebuf(sizebuf,picl.ReadByte());
			break;
		case ARG_SIZE_16:
			picl.WriteShortSizebuf(sizebuf,picl.ReadShort());
			break;
		case ARG_SIZE_24:
			picl.WriteShortSizebuf(sizebuf,picl.ReadShort());
			picl.WriteByteSizebuf(sizebuf,picl.ReadByte());
			break;
		case ARG_SIZE_32:
			picl.WriteLongSizebuf(sizebuf,picl.ReadLong());
			break;

		default:
			break;
	}
}

void clientRICBuf::ReadRICs(void)
{
	int		r,rmax;
	byte	RIC;
	byte	argsFlags=0;

	// Get number of RICs to read from the network stream.
	
	rmax=picl.ReadByte();
	picl.WriteByteSizebuf(&sizebuf,rmax);

	for(r=0;r<rmax;r++)
	{
		// Get the RIC itself and save it.

		RIC=picl.ReadByte();
		argsFlags=(RIC&0xf0)>>ARG_SIZE_SHIFT;
		picl.WriteByteSizebuf(&sizebuf,RIC);
		RIC&=0x0f;

		// Get parameters and save them.

		switch(RIC)
		{
			case RIC_RULESSETWEAPONSUSEAMMO:

				HandleArg(1,argsFlags,&sizebuf);
				break;

			case RIC_RULESSETWEAPONRELOADAUTOMATIC:

				HandleArg(1,argsFlags,&sizebuf);
				break;

			case RIC_RULESSETWEAPONSRELOAD:

				HandleArg(1,argsFlags,&sizebuf);
				break;

			case RIC_RULESSETRELOADEMPTIESWEAPON:
				
				HandleArg(1,argsFlags,&sizebuf);
				break;

			case RIC_SETBLOODYWEAPON:

				HandleArg(1,argsFlags,&sizebuf);
				break;

			case RIC_HIDEWEAPONFORDISGUISE:

				break;
		}

		numRICs++;
	}
}

int RetrieveArg(int argNum,byte argsFlags,sizebuf_t *sizebuf)
{
	assert((argNum>=1)&&(argNum<=2));

	byte	argSize;
	int		arg;

	argSize=(argsFlags>>((argNum-1)*2))&ARG_SIZE_MASK;

	switch(argSize)
	{
		case ARG_SIZE_8:
			arg=picl.ReadByteSizebuf(sizebuf);
			break;
		case ARG_SIZE_16:
			arg=picl.ReadShortSizebuf(sizebuf);
			break;
		case ARG_SIZE_24:
			arg=picl.ReadShortSizebuf(sizebuf);
			arg|=((int)(picl.ReadByteSizebuf(sizebuf)))<<16;
			break;
		case ARG_SIZE_32:
			arg=picl.ReadLongSizebuf(sizebuf);
			break;

		default:
			break;
	}

	return(arg);
}

void clientRICBuf::ProcessRICs(void)
{
	int		r,rmax,
			param1;
	byte	RIC;
	byte	argsFlags=0;

	// Get number of RICs to read from local storage.

	rmax=picl.ReadByteSizebuf(&sizebuf);

	for(r=0;r<rmax;r++)
	{
		// Get the RIC iteself.

		RIC=picl.ReadByteSizebuf(&sizebuf);
		argsFlags=(RIC&0xf0)>>ARG_SIZE_SHIFT;
		RIC&=0x0f;

		// Get parameters and call the appropriate method for the RIC.

		switch(RIC)
		{
			case RIC_RULESSETWEAPONSUSEAMMO:

				param1=RetrieveArg(1,argsFlags,&sizebuf);
				owner->rulesSetWeaponsUseAmmo(param1);
				break;

			case RIC_RULESSETWEAPONRELOADAUTOMATIC:

				param1=RetrieveArg(1,argsFlags,&sizebuf);
				owner->rulesSetWeaponReloadAutomatic(param1);
				break;

			case RIC_RULESSETWEAPONSRELOAD:

				param1=RetrieveArg(1,argsFlags,&sizebuf);
				owner->rulesSetWeaponsReload(param1);
				break;

			case RIC_RULESSETRELOADEMPTIESWEAPON:
				
				param1=RetrieveArg(1,argsFlags,&sizebuf);
				owner->rulesSetReloadEmptiesWeapon(param1);

				break;
				
			case RIC_SETBLOODYWEAPON:

				param1=RetrieveArg(1,argsFlags,&sizebuf);
				owner->setBloodyWeapon(param1);
				break;

			case RIC_HIDEWEAPONFORDISGUISE:
				
				owner->becomeDisguised();
				break;
		}

		numRICs--;
	}
}

serverRICBuf::serverRICBuf(void)
{
	numRICs=0;
	owner=NULL;
	pisv.SZ_Init(&sizebuf,(unsigned char *)sizebufData,sizeof(sizebufData));
}

void serverRICBuf::ClearRICs(void)
{
	numRICs=0;

	pisv.SZ_Clear(&sizebuf);
}

void serverRICBuf::AddRIC(RIC_t RIC,char *argsFormat,...)
{
	// Only allowed 16 different RIC types, else error.

	assert((RIC>=0)&&(RIC<=15));
	
	// Retrieve the RIC's arguments.

	int		argc=0;
	int		args[2];
	byte	argsFlags=0;

	va_list	argPtr;

	va_start(argPtr,argsFormat);

	for(char *p=argsFormat;*p;p++)
	{	
		switch(*p)
		{
			case 'i':
					
				// Grab arg and save it.
				
				int arg;

				arg=va_arg(argPtr,int);
				args[argc]=arg;
				argc++;				

				// Determine argument size for arg - to be packed in with the RIC byte.

				if(!(arg&~0xff))
					argsFlags|=ARG_SIZE_8<<(argc-1)*2;
				else if(!(arg&~0xffff))
					argsFlags|=ARG_SIZE_16<<(argc-1)*2;
				else if(!(arg&~0xffffff))
					argsFlags|=ARG_SIZE_24<<(argc-1)*2;
				else
					argsFlags|=ARG_SIZE_32<<(argc-1)*2;

				break;

			default:

				break;
		}

		// 2 arguments maximum, else error.

		assert(argc<=2);
	}

	// Write the RIC to the network layer...

	pisv.WriteByteSizebuf(&sizebuf,RIC|(argsFlags<<ARG_SIZE_SHIFT));

	// ...followed by the arguments.

	int arg;

	for(int i=0;i<argc;i++)
	{
		arg=args[i];

		if(!(arg&~0xff))
			pisv.WriteByteSizebuf(&sizebuf,(byte)arg);
		else if(!(arg&~0xffff))
			pisv.WriteShortSizebuf(&sizebuf,(short)arg);
		else if(!(arg&~0xffffff))
		{
			pisv.WriteShortSizebuf(&sizebuf,(short)arg&0xffff);
			pisv.WriteByteSizebuf(&sizebuf,(byte)((arg&0xff0000)>>16));
		}
		else
			pisv.WriteLongSizebuf(&sizebuf,arg);
	}

	// Update number of RICs in buffer.

	numRICs++;
}

void serverRICBuf::WriteRICs(void)
{
	if(numRICs)
	{
		int	clientNum;

		clientNum=owner->getOwner()->edict->s.number-1;

		// Write the svc_ric header byte.

		pisv.ReliableWriteByteToClient(svc_ric,clientNum);


		// Now write the number of RICs.

		pisv.ReliableWriteByteToClient(numRICs,clientNum);
		
		// And finally, the RICs themselves.

		pisv.ReliableWriteDataToClient(sizebuf.data,sizebuf.cursize,clientNum);
	}
}