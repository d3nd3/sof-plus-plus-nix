#pragma once

#define EYEBALL_HEIGHT		35 //26	//22 <-- old values
#define DUCK_EYEHEIGHT		0


/*
dk -- 2/4/00

To change the view height:

The viewheight is sent from the client to the server via MSG_WriteChar and and MSG_ReadChar, which write/read
signed bytes.  Thus, we have a range of -128 - 127.  The view height in actual world units is defined above.  Then there is
some clamping done in SV_CalcViewOffset1stPerson in p_view.cpp.  The relevant variable is v.  I've got it set up now to clamp 
between -11 and 40.  Now to transmit this from server to client, I subtract 14.6 on the serer side, and then multiply by 5.
This gives us the desired range of -128 to 127.  Of course, on the client, I then multiply by .2 and add 14.6. 

*/
// end

