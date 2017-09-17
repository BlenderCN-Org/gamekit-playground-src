/*
 * AndroidGame.h
 *
 *  Created on: Feb 28, 2013
 *      Author: ttrocha
 */

#ifndef BBGAME_H_
#define BBGAME_H_

#include "gkString.h"
#include "gkUserDefs.h"
#include "gkCoreApplication.h"
#include "gkMessageManager.h"
#include "gkMathUtils.h"
#include "android/AndroidInputManager.h"

class gkBlackberryGame : public gkCoreApplication
{
public:
	gkBlackberryGame(const gkString& initialBlend);
	virtual ~gkBlackberryGame();
	// gkCoreApplication
	bool setup(void);
	// gkMessageManager-listener
	void handleMessage(gkMessageManager::Message* message);

private:
	const gkString 	m_initialBlendName;
	gkBlendFile* 	m_initialBlend;
};

#endif /* ANDROIDGAME_H_ */
