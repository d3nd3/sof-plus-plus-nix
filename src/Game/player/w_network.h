#ifndef __W_NETWORK_H_
#define __W_NETWORK_H_

#include "qcommon.h"

// ****************************************************************************
// RICs (Remote Inventory Commands) 
// --------------------------------
// Issued by server (i.e. game.dll) and read by clients. Allows the server to
// remotely instruct a client's view-weapon prediction to add ammo, weapons,
// items etc. to it's (predicted) inventory.
// ****************************************************************************

typedef enum
{
	RIC_RULESSETWEAPONSUSEAMMO,
	RIC_RULESSETWEAPONSRELOAD,
	RIC_RULESSETWEAPONRELOADAUTOMATIC,
	RIC_RULESSETRELOADEMPTIESWEAPON,
	RIC_SETBLOODYWEAPON,
	RIC_HIDEWEAPONFORDISGUISE,
} RIC_t;

// ****************************************************************************
// RICBuf
// ------
// ****************************************************************************

#define RIC_BUFFERSIZE 256

class RICBuf
{
protected:
	
	int			numRICs;
	sizebuf_t	sizebuf;
	char		sizebufData[RIC_BUFFERSIZE];
	inven_c		*owner;

public:

	inven_c	*getOwner(){return owner;}
	void setOwner(inven_c *inv){owner=inv;}
};

// ****************************************************************************
// clientRICBuf
// ------------
// ****************************************************************************

class clientRICBuf:public RICBuf
{
public:
	
	clientRICBuf(void);
	
	void ClearRICs(void);
	void ReadRICs(void);
	void ProcessRICs(void);
};

// ****************************************************************************
// serverRICBuf
// ------------
// ****************************************************************************

class serverRICBuf:public RICBuf
{
public:

	serverRICBuf(void);

	void ClearRICs(void);
	void AddRIC(RIC_t RIC,char *argsFormat,...);
	void WriteRICs(void);
};

#endif __W_NETWORK_H_