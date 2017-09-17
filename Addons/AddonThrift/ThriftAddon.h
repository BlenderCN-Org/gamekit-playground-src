/*
 * GameAddon.h
 *
 *  Created on: Mar 16, 2013
 *      Author: ttrocha
 */

#ifndef THRIFTGAMEADDON_H_
#define THRIFTGAMEADDON_H_

#include "gkAddon.h"

#include "generated/gen-cpp/TGameServerService.h"
#include "gkThriftManager.h"
//#include "Network/gkNetworkManager.h"




class ThriftAddon : public gkAddon {
public:

	ThriftAddon();
	virtual ~ThriftAddon();

	void setup();
	void tick(gkScalar rate);

	static HTTPClient<gs::TGameServerServiceClient>* thriftclient;

};

#endif /* GAMEADDON_H_ */
