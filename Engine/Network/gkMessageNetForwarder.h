/*
 * gkMessageNetForwarder.h
 *
 *  Created on: 11.04.2012
 *      Author: ttrocha
 */

#ifndef GKMESSAGENETFORWARDER_H_
#define GKMESSAGENETFORWARDER_H_

#include "OgreKit.h"
#include "Network/gkNetworkManager.h"

#define NETPLAYER_ACTION_CREATE  1
#define NETPLAYER_ACTION_MOVE 2
#define NETPLAYER_ACTION_REMOVE 3

#define NET_TYPE_MESSAGE 1
#define NET_TYPE_PLAYER 2


class gkMessageNetForwarder : public gkMessageManager::MessageListener, public gkNetworkListener
{
public:
	gkMessageNetForwarder();
	virtual ~gkMessageNetForwarder();
	// handle gkMessages from the Message-System
	virtual void handleMessage(gkMessageManager::Message* message);
	// handle incoming network
	virtual void onData(void* data,int dataSize);

	struct NetMessage {
		char type;
		char from[40];
		char to[40];
		char subject[40];
		char body[40];
		NetMessage(gkMessageManager::Message* msg){
			type = 11;
//			strcpy(from,"test");
			strcpy(from,msg->m_from.c_str());
			strcpy(to,msg->m_to.c_str());
			strcpy(subject,msg->m_subject.c_str());
			strcpy(body,msg->m_body.c_str());
		}
	};

	struct NetPlayer {
		char type;
		char action_id;
		char player_id;
		char posx,posy,posz;
	};
};

#endif /* GKMESSAGENETFORWARDER_H_ */
