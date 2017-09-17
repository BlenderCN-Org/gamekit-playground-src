/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): none yet.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/

#ifndef _gsNetwork_h_
#define _gsNetwork_h_



#include "utCommon.h"
#include "Network/gkNetworkManager.h"

#include "gsCommon.h"
class gsNetwork
{
	gsNetwork(){};
	~gsNetwork(){};

};

extern void initCallbacks();

static gkLuaEvent* socketCallback = 0;

extern void onSocketOpen(int fd, void* userData);
extern void onSocketMessage(int fd, void* userData);
extern void onSocketError(int fd, int err, const char* msg, void* userData);

extern void setSocketCallback(gsFunction func);

extern void startNetServer(unsigned short port);
extern void stopNetServer();
extern bool isServerRunning();
extern void startNetClient(gkString serverAddress,unsigned short port);
extern void stopNetClient();
extern bool isClientRunning();


#endif
