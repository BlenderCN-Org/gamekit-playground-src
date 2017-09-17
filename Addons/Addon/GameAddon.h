/*
 * GameAddon.h
 *
 *  Created on: Mar 16, 2013
 *      Author: ttrocha
 */

#ifndef GAMEADDON_H_
#define GAMEADDON_H_

#include "gkAddon.h"

class GameAddon : public gkAddon {
public:
	GameAddon();
	virtual ~GameAddon();

	void setup();
	void tick(gkScalar rate);

};

#endif /* GAMEADDON_H_ */
