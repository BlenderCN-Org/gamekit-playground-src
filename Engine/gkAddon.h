/*
 * gkAddon.h
 *
 *  Created on: Mar 16, 2013
 *      Author: ttrocha
 */

#ifndef GKADDON_H_
#define GKADDON_H_
#include "gkEngine.h"
#include "gkMathUtils.h"

class gkAddon : public gkEngine::Listener {
public:
	gkAddon();
	virtual ~gkAddon();

	virtual void setup() = 0;

	virtual void tick(gkScalar rate) = 0;

};

#endif /* GKADDON_H_ */
