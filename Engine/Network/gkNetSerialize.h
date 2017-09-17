/*
 * gkNetSerialize.h
 *
 *  Created on: 12.04.2012
 *      Author: ttrocha
 */

#ifndef GKNETSERIALIZE_H_
#define GKNETSERIALIZE_H_

#define NETTYPE_VEC3 1
#include "gkMathUtils.h"
#include "utCommon.h"

class gkNetSerialize {
public:
	gkNetSerialize();
	virtual ~gkNetSerialize();

	static void serialize(gkVector3* in, UTuint8 *buf, UTuint16 &offset);
	static gkVector3* unserialize(UTuint8 *buf,UTuint16 &offset,gkVector3* vec);
};

#endif /* GKNETSERIALIZE_H_ */
