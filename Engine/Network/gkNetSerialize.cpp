/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2012 Thomas Trocha (dertom)

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
#include "gkNetSerialize.h"
#include "enet/extra/serialize.h"

gkNetSerialize::gkNetSerialize() {
	// TODO Auto-generated constructor stub

}

gkNetSerialize::~gkNetSerialize() {
	// TODO Auto-generated destructor stub
}

void gkNetSerialize::serialize(gkVector3* in, UTuint8 *buf, UTuint16 &offset){
	int packetSize = pack(&buf[offset],"Cfff",NETTYPE_VEC3,in->x,in->y,in->z);
	offset+=packetSize;
}

gkVector3* gkNetSerialize::unserialize(UTuint8 *buf,UTuint16 &offset,gkVector3* vec = 0) {
	if (!vec){
		vec = new gkVector3;
	}
	UTuint8 type;
	unpack(&buf[offset],"Cfff",&type,&vec->x,&vec->y,&vec->z);
	offset += 7;
	return vec;
}
