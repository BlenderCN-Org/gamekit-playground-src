/*
 * ttScript.h
 *
 *  Created on: Apr 3, 2013
 *      Author: ttrocha
 */

#ifndef TTSCRIPT_H_
#define TTSCRIPT_H_

#include "utStreams.h"
#include "gkString.h"

void activateEmbeddedScripts();


class ttSuperTest {
public:
	int add(int a,int b) { return a+b;}

};

#endif /* TTSCRIPT_H_ */
