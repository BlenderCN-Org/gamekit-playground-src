/*
 * gkMessageNetForwarder.cpp
 *
 *  Created on: 11.04.2012
 *      Author: ttrocha
 */

#include "Network/gkMessageNetForwarder.h"
#include "OgreKit.h"
#include "enet/extra/serialize.h"
#include "gkMathUtils.h"
#include "gkNetSerialize.h"

gkMessageNetForwarder::gkMessageNetForwarder(){
	// TODO Auto-generated constructor stub

}

gkMessageNetForwarder::~gkMessageNetForwarder() {
	// TODO Auto-generated destructor stub
}

void gkMessageNetForwarder::handleMessage(gkMessageManager::Message* message){
#if OGREKIT_NETWORK_ENET_SUPPORT
	if (message->m_from!="network" && message->m_to=="network")
	{
		if (gkNetworkClient::getSingletonPtr()){
			UTuint8 buf[1024];
			UTuint16 packetsize;
			gkVector3* vec = new gkVector3(1,2,3);

			packetsize = 0;
//			gkNetSerialize::serialize(vec,(UTuint8*)&buf,packetsize);

			packetsize+=pack(&buf[packetsize],"Cssss",(UTuint8)11,message->m_from.c_str()
										,""
										,message->m_subject.c_str()
										,message->m_body.c_str());



			gkNetworkClient::getSingleton().sendToServer(0,true,&buf,packetsize);
		}
	}
#endif
 }

void gkMessageNetForwarder::onData(void* msg, int dataSize){
//	NetMessage* netMsg = ((NetMessage*)msg);
	UTuint8* data = (UTuint8*)msg;
	UTuint8 type;
//	unpack(data,"C", &type);
//	if (type==11)
	{
		char from[40];
		char to[40];
		char subject[40];
		char body[40];
		UTuint16 offset;
		offset = 0;
//		gkVector3* vec = gkNetSerialize::unserialize(data,offset,0);
		unpack(&data[offset],"Cssss",&type,&from,&to,&subject,&body);
//		gkLogger::write("Incoming Message! ",true);
		gkMessageManager::getSingleton().sendMessage("",
				to,subject,body);
	}
//	else {
//		gkLogger::write("Unknown incoming Message! Ignore!",true);
//	}
}
