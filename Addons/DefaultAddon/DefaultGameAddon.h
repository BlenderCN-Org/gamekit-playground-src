/*
 * GameAddon.h
 *
 *  Created on: Mar 16, 2013
 *      Author: ttrocha
 */

#ifndef GAMEADDON_H_
#define GAMEADDON_H_

#include "gkAddon.h"

extern "C" {
	#include <lstate.h>
	void install_Templates_DefaultAddon(lua_State* L);
}

class DefaultGameAddon : public gkAddon {
public:
	DefaultGameAddon();
	virtual ~DefaultGameAddon();

	void setup();
	void tick(gkScalar rate);

};

#endif /* GAMEADDON_H_ */
