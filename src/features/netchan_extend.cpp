typedef struct networkCommand_s {
	int msgsize;
} networkCommand_t;
int num_net_cmds[16];
networkCommand_t net_commands[16][MAX_NET_COMMANDS];
char unreliableData[16][TOTAL_PSIZE];//1390 * 1024
unsigned int unreliablePointer[16];

/*
This is also called by SV_Map before SV_SpawnServer
So you can't use multicast or sv variables here cos they are all 0. cleared for mapchange.
*/
void my_SV_SendClientMessages(void)
{

	// sv.state == ss_game
	if ( stget(0x203A1F20,0) == 2 ){
		// 83 max boxes 83 * 12 = 996
		if( _sf_sv_debug_triggers->value ) {
			if ( _sf_sv_debug_triggers->value == 3.0f ) {
				randomBoxCode();
			} else  {
				if ( *level_framenum % 5 == 0 ) {
					drawBoxesOnEnts(1,_sf_sv_debug_triggers->value);
				} else
				if ( *level_framenum % 5 == 1 ) {
					drawBoxesOnEnts(2,_sf_sv_debug_triggers->value);
				} else
				if ( *level_framenum % 5 == 2 ) {
					drawBoxesOnEnts(3,_sf_sv_debug_triggers->value);
				} else
				if ( *level_framenum % 5 == 3 ) {
					drawBoxesOnEnts(4,_sf_sv_debug_triggers->value);
				} else
				if ( *level_framenum % 5 == 4 ) {
					drawBoxesOnEnts(5,_sf_sv_debug_triggers->value);
				}
			}
			
		}
		
	}
	orig_SV_SendClientMessages();

}


//called for every client
/*
Seperated reliable unreliable from SV_SendClientDatagram

qboolean SV_SendClientDatagram (client_t *client):
	SV_BuildClientFrame (client);
	SZ_Init (&msg, msg_buf, sizeof(msg_buf));
	msg.allowoverflow = true;
	SV_WriteFrameToClient (client, &msg);
	if (client->datagram.overflowed)
		Com_Printf ("WARNING: datagram overflowed for %s\n", client->name); DELETED
	else
		SZ_Write (&msg, client->datagram.data, client->datagram.cursize); DELETED
	SZ_Clear (&client->datagram);
	if (msg.overflowed)
	{	// must have room left for the packet header
		Com_Printf ("WARNING: msg overflowed for %s\n", client->name); RENAMED FRAME DOES NOT FIT
		SZ_Clear (&msg);
	}
	// msg will only contain frame data from sv_writeframetoclient
	// this is hooked
	Netchan_Transmit (&client->netchan, msg.cursize, msg.data);
	client->message_size[sv.framenum % RATE_MESSAGES] = msg.cursize;
	return true;


	Iterates the accumulated unreliable cmds and puts them into packets, then sends them out.
*/
qboolean my_SV_SendClientDatagram(client_t *client)
{
	edict_t * ent = stget(client,CLIENT_ENT);
	if ( !ent || !ent->inuse ) return orig_SV_SendClientDatagram(client);
	int slot = get_player_slot_from_ent(ent);
	

	// the original netchan_transmit will still send frame and then the reliable on 2 seperate packets
	// This is just the frame.
	qboolean ret = orig_SV_SendClientDatagram(client);

	void * client_t = game_clients + slot * 0xd2ac;

	// headersize = 10
	int packets_sent = 0;
	int indexToPacketStart = 0;
	int numcmds = num_net_cmds[slot];
	int perPacketByteCount = 0;
	// orig_Com_Printf("This many commands : %i\n",numcmds);
	for ( int i = 0; i < numcmds; i++ ) {
		perPacketByteCount += net_commands[slot][i].msgsize;
		if ( perPacketByteCount > 1384 ) {
			// orig_Com_Printf("Full Packet wtf\n");
			// i'th command does not fit
			// sendpacket (i-1)th commands

			int full_packet_len = perPacketByteCount - net_commands[slot][i].msgsize;
			Netchan_Transmit_Unreliable(client_t + 0x526C, full_packet_len , &unreliableData[slot][indexToPacketStart]);

			packets_sent++;
			indexToPacketStart += full_packet_len;
			// reset this counter
			perPacketByteCount = net_commands[slot][i].msgsize;
		}
	}
	// remaining
	if ( perPacketByteCount ) {
		// orig_Com_Printf("Sending Packet Length = %i, offsetFrom = %i\n",tmp_parse,parsed);

		Netchan_Transmit_Unreliable(client_t + 0x526C, perPacketByteCount , &unreliableData[slot][indexToPacketStart]);
		packets_sent++;
	}

	 // orig_Com_Printf("You Sent %i Packets HeHe\n",packets_sent);

	// reset for next frame
	num_net_cmds[slot] = 0;

	#ifdef NETDEBUG
	orig_Com_Printf("unreli %i bytes : %i packets\n",unreliablePointer[slot],packets_sent);
	#endif
	unreliablePointer[slot] = 0;

	return ret;
}


// malloc
// realloc
// free
char * overflowed_data[16];
int data_len[16];
void my_SZ_Write(sizebuf_t * buf, void * data, int length)
{
	register int tt asm("ebp");
	unsigned int ret_address = *(unsigned int*)(tt + 4);
	if ( ret_address == 0x20062262 ) {
		// specific hook @ sv_startsound for client
		// unsigned char temp[256];
		// sprintf(temp,"say unreliable data written of length : %i\n",length);
		// orig_Cbuf_AddText(temp);
		for ( int i =0; i < maxclients->value; i++ ) {
			void * client_t = game_clients + i * 0xd2ac;
			sizebuf_t * dg = client_t + 0x2c4;
			int state = *(int*)(client_t);
			if ( state == cs_free || state == cs_zombie)
				continue;

			// unreliable
			if ( buf == dg ) {
				edict_t * ent = get_ent_from_player_slot(i);
				sv_multicast->cursize = length;
				my_PF_Unicast(ent,0);
				return;
			}
		}

		
	} else {
		orig_SZ_Write(buf,data,length);
	}
	
}



/*
MULTICAST_ALL,
MULTICAST_PHS,
MULTICAST_PVS,
MULTICAST_ALL_R,
MULTICAST_PHS_R,
MULTICAST_PVS_R
*/
sizebuf_t * sv_multicast = 0x203F6C34;
// sof Multicast has a third argument for some reason
// I THINK THIRD ARGUMENT IS SLOT NUMBER + 1 , EXCEPT HIM!
void my_SV_Multicast (vec3_t origin, multicast_t to, int unknown)
{
	// return orig_SV_Multicast(origin,to,unknown);
	// sv_multicast->overflowed = false;
	// sv_multicast->cursize = 0;
	// return;
	
	sizebuf_t * dg;
	int size_before = sv_multicast->cursize;
	int dg_before[16];
	for ( int i = 0 ; i < maxclients->value;i++ ) {
		void * client_t = game_clients + i * 0xd2ac;

		int state = *(int*)(client_t);
		if ( state == cs_free || state == cs_zombie)
			continue;
		// dont send unreliable to ppl who have not spawned
		if (state != cs_spawned )
			continue;
		dg = client_t + 0x2C4;
		dg_before[i] = dg->cursize;
	}

	// let it be called before
	// if this overflows writing to datagram, datagram will be cleared, cursize math corrupts
	// lets make sure the unreliable buffer does not overflow by clearing it often?
	orig_SV_Multicast(origin,to,unknown);

	// unreliable
	if ( to < 3 ) {
		int maxloop = maxclients->value;
		for ( int i=0; i < maxloop;i++) {
			void * client_t = game_clients + i * 0xd2ac;

			int state = *(int*)(client_t);
			if ( state == cs_free || state == cs_zombie)
				continue;
			// dont send unreliable to ppl who have not spawned
			if (state != cs_spawned )
				continue;

			// unreliable datagram
			dg = client_t + 0x2C4;
			int len = dg->cursize - dg_before[i];
			if ( dg->cursize >  dg_before[i]) {
				// orig_Com_Printf("unreli pointer = %i\n",unreliablePointer[i]);
				memcpy(&(unreliableData[i][unreliablePointer[i]]),dg->data + dg_before[i],len);

				unreliablePointer[i]+=len;
				// save cursize
				net_commands[i][num_net_cmds[i]].msgsize = len;
				num_net_cmds[i]++;
			}
			// clear datagram unreliable buffer
			dg->cursize = 0;
			dg->overflowed = false;
		}
	}
	
	/*
	Multicast will copy sv.multicast into ALL reliable or unreliable client buffers
	*/
	
}
// I THINK THIRD ARGUMENT IS SLOT NUMBER , EXCEPT HIM!
void my_SV_Multicast2 (vec3_t origin, multicast_t to,int unknown)
{
	// sv_multicast->overflowed = false;
	// sv_multicast->cursize = 0;
	// return;
	
	sizebuf_t * dg;
	int size_before = sv_multicast->cursize;
	int dg_before[16];
	void * client_t = NULL;
	int i = 0;
	for ( i = 0 ; i < maxclients->value;i++ ) {
		client_t = game_clients + i * 0xd2ac;

		int state = *(int*)(client_t);
		if ( state == cs_free || state == cs_zombie)
			continue;
		// dont send unreliable to ppl who have not spawned
		if (state != cs_spawned )
			continue;
		dg = client_t + 0x2C4;
		dg_before[i] = dg->cursize;
	}

	// let it be called before
	orig_SV_Multicast2(origin,to,unknown);

	// unreliable
	if ( to < 3) {
		int maxloop = maxclients->value;
		for ( i=0; i < maxloop;i++) {
			client_t = game_clients + i * 0xd2ac;

			int state = *(int*)(client_t);
			if ( state == cs_free || state == cs_zombie)
				continue;
			// dont send unreliable to ppl who have not spawned
			if (state != cs_spawned )
				continue;

			dg = client_t + 0x2C4;
			// sample how many bytes were written to the buffer
			int len = dg->cursize - dg_before[i];

			if ( dg->cursize >  dg_before[i]) {
				// orig_Com_Printf("Client %i ::  %i %i\n",i,len,unknown);

				// orig_Com_Printf("unreli pointer = %i\n",unreliablePointer[i]);
				memcpy(&(unreliableData[i][unreliablePointer[i]]),dg->data + dg_before[i],len);
				unreliablePointer[i]+=len;
				// save cursize
				net_commands[i][num_net_cmds[i]].msgsize = len;
				num_net_cmds[i]++;
			}
			// this is quite important to make sure the unreliable buffer never overflows
			dg->cursize = 0;
			dg->overflowed = false;
		}
	}
	
	/*
	Multicast will copy sv.multicast into ALL reliable or unreliable client buffers
	*/
}

/*
	Redirect unreliable into our buffer.
*/
void my_PF_Unicast (edict_t *ent, qboolean reliable)
{
	// sv_multicast->overflowed = false;
	// sv_multicast->cursize = 0;
	// return;

	if ( !reliable ) {
		// write directly into our buffer instead of using datagram unreliable buffer
		int slot =  get_player_slot_from_ent(ent);
		// if ( !_sf_sv_sofree_debug->value ) {
			num_net_cmds[slot]++;
			// save cursize
			net_commands[slot][num_net_cmds[slot]-1].msgsize = sv_multicast->cursize;
			// orig_Com_Printf("unreli pointer = %i\n",unreliablePointer[slot]);
			memcpy(&(unreliableData[slot][unreliablePointer[slot]]),sv_multicast->data,sv_multicast->cursize);
			unreliablePointer[slot]+=sv_multicast->cursize;
		// } else {
		// 	orig_PF_Unicast(ent,reliable);
		// }


		sv_multicast->overflowed = false;
		sv_multicast->cursize = 0;
		
	} else {
		orig_PF_Unicast(ent,reliable);
	}
	
	/*
	Unicast will copy sv.multicast into reliable or unreliable client buffer
	*/
	// orig_PF_Unicast(ent,reliable);
}


void my_SV_WriteFrameToClient (client_t *client, sizebuf_t *msg)
{
	// client->lastframe
	// this disables delta compression
	// *(int*)(client + 0x204) = -1;
	orig_SV_WriteFrameToClient(client,msg);
}


void my_MSG_WriteShort(sizebuf_t *sb, int c)
{
	orig_MSG_WriteShort(sb,c);
}
sizebuf_t * g_msg;
int sizeAtEmit;
void * savedTo;
int entcount;
int removeCmdCount;
bool inwritedelta;
/*
Limit entities
*/
void my_MSG_WriteDeltaEntity (struct entity_state_s *from, struct entity_state_s *to, sizebuf_t *msg, qboolean force, qboolean newentity)
{
	inwritedelta = true;
	orig_MSG_WriteDeltaEntity(from,to,msg,force,newentity);
	inwritedelta = false;
	// inside SV_EmitPacketEntities
	if ( g_msg ) {
		// g_msg->maxsize == 1400
		int cursize = g_msg->cursize - sizeAtEmit;
		
		// break if we run out of room
		// size of an entity and size of header
		if (  g_msg->cursize >= ( g_msg->maxsize - 40 - 16) ) {
			// break from SV_EmitPacketEntities loop please by faking to->num_entities
			*(int*)(savedTo + 0xEC) = 0;
			// orig_Com_Error(ERR_FATAL,"This map has too many entities, please ask map developer to fix it\n");
			// orig_Com_Printf("cursize = %i g_msg.cursize = %i g_msg.maxsize = %i\n",cursize,g_msg->cursize,g_msg->maxsize);
			orig_Cbuf_AddText("say overflowing entitiesDelta\n");
		}
		entcount++;
	}
}


/*
	also called by WriteDeltaEntity.
	entity upperlimit - refuse to send all ents
*/
void my_MSG_WriteUnknownDelta(sizebuf_t *sb, int c)
{
	orig_MSG_WriteUnknownDelta(sb,c);
	// inside SV_EmitPacketEntities
	if ( g_msg && !inwritedelta) {
		int cursize = g_msg->cursize - sizeAtEmit;
		// break if we run out of room
		if ( g_msg->cursize >= ( g_msg->maxsize - 40 - 16) ) {
			// break from SV_EmitPacketEntities loop please by faking to->num_entities
			*(int*)(savedTo + 0xEC) = 0;
			// orig_Com_Error(ERR_FATAL,"This map has too many entities, please ask map developer to fix it\n");
			// orig_Cbuf_AddText("say overflowing entitiesUnknown\n");
		}
		removeCmdCount++;
	}
}


/*
save msg here into global
dont allow to send too many entities to clients
entity upperlimit - refuse to send all ents
*/
void my_SV_EmitPacketEntities (void *from, void *to, sizebuf_t *msg)
{
	sizeAtEmit = msg->cursize;
	g_msg = msg;
	savedTo = to;
	entcount = 0;
	removeCmdCount = 0;
	int size_before = msg->cursize;

	// cap num_entities to 64...
	// save for restoration later
	int num_ents = *(int*)(to + 0xEC);

	
	// if ( num_ents > 64 ) {
	// 	*(int*)(to + 0xEC) = 64;
	// }

	// save and restore to->num_ents
	// orig_SV_EmitPacketEntities(from,to,msg);
	// PUT FROM As NULL to force retransmit instead of delta?? :P
	orig_SV_EmitPacketEntities(from,to,msg);

	// orig_Com_Printf("originally %i limited to %i numremoves %i\n",num_ents,entcount,removeCmdCount);

	// because i use a trick to end a loop inside emitpacketetieis whcih involve this var
	// restoration
	*(int*)(to + 0xEC) = num_ents;
	g_msg = NULL;
}

/*
	This does not append any reliable data.
*/
void Netchan_Transmit_Unreliable(netchan_t *chan, int length, byte *data)
{
	byte		send_buf[1400];
	sizebuf_t	send;
	qboolean	send_reliable = false;
	unsigned	w1, w2;

	// write the packet header
	memset (&send, 0, sizeof(send));
	send.data = send_buf;
	send.maxsize = sizeof(send_buf);

	w1 = ( chan->outgoing_sequence & ~(1<<31) ) | (send_reliable<<31);
	w2 = ( chan->incoming_sequence & ~(1<<31) ) | (chan->incoming_reliable_sequence<<31);

	chan->outgoing_sequence++;
	chan->last_sent = *(int*)0x20390D38;

	orig_MSG_WriteLong (&send, w1);
	orig_MSG_WriteLong (&send, w2);

	// send the qport if we are a client
	if (chan->sock == NS_CLIENT)
		orig_MSG_WriteShort (&send, qport->value);

	
// add the unreliable part if space is available
	if (send.maxsize - send.cursize >= length)
		orig_SZ_Write (&send, data, length);
	else
		orig_Com_Printf ("Netchan_Transmit: dumped unreliable\n");

// send the datagram
	orig_NET_SendPacket (chan->sock, send.cursize, send.data, chan->remote_address);
}

// this is not a detour, its a jmp overwrite
/*
SendClientDatagram
Builds the frame and the entities based on PHS etc.. then calls Netchan_Transmit..
*/
void my_Netchan_Transmit (netchan_t *chan, int length, byte *data)
{
	// orig_Netchan_Transmit(chan,length,data);
	// return;

	
	// sizebuf_t * reli_msg  = (sizebuf_t*)((unsigned int)chan + 0x48);
	sizebuf_t * reli_msg = &chan->message;
	// if ( reli_msg != &chan->message)
		// orig_Com_Printf("chan does not work\n");
	
	
	// send the frame !!!
	// works better when The frame is sent FIRST SHAKEY BUG.. otherwise.
	Netchan_Transmit_Unreliable(chan,length,data);

	qboolean	send_reliable;

// if the remote side dropped the last reliable message, resend it
	send_reliable = false;

	int len = chan->reliable_length;
	if (chan->incoming_acknowledged > chan->last_reliable_sequence
	&& chan->incoming_reliable_acknowledged != chan->reliable_sequence)
		send_reliable = true;

// if the reliable transmit buffer is empty, copy the current message out
	if (!chan->reliable_length && chan->message.cursize)
	{
		send_reliable = true;
		len = chan->message.cursize;
	}
	char extra[64];
	extra[0] = 0x00;
	if ( send_reliable ) {
		
		char emptybuffer;
		#ifdef NETDEBUG
		if ( reli_msg->cursize ) 
			sprintf(extra,"reli size = %i",len);
		#endif
		// send reliable!!
		orig_Netchan_Transmit(chan,0,&emptybuffer);
	}
	#ifdef NETDEBUG
	orig_Com_Printf("frame/ents = %i %s\n",length,extra);
	#endif
}