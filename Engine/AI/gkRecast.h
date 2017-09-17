/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Nestor Silveira.

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
#ifndef _gkRecast_h_
#define _gkRecast_h_

#include "gkCommon.h"
#include "gkMeshData.h"

#include "OgreSimpleRenderable.h"
#include "Recast.h"
#include "PerfTimer.h"
class dtNavMesh;
class dtNavMeshQuery;

/// Recast build context.
class BuildContext : public rcContext
{
	TimeVal m_startTime[RC_MAX_TIMERS];
	int m_accTime[RC_MAX_TIMERS];

	static const int MAX_MESSAGES = 1000;
	const char* m_messages[MAX_MESSAGES];
	int m_messageCount;
	static const int TEXT_POOL_SIZE = 8000;
	char m_textPool[TEXT_POOL_SIZE];
	int m_textPoolSize;

public:
	BuildContext();
	virtual ~BuildContext();

	/// Dumps the log to stdout.
	void dumpLog(const char* format, ...);
	/// Returns number of log messages.
	int getLogCount() const;
	/// Returns log message text.
	const char* getLogText(const int i) const;

protected:
	/// Virtual functions for custom implementations.
	///@{
	virtual void doResetLog();
	virtual void doLog(const rcLogCategory /*category*/, const char* /*msg*/, const int /*len*/);
	virtual void doResetTimers();
	virtual void doStartTimer(const rcTimerLabel /*label*/);
	virtual void doStopTimer(const rcTimerLabel /*label*/);
	virtual int doGetAccumulatedTime(const rcTimerLabel /*label*/) const;
	///@}
};

struct gkDetourNavMesh : public gkReferences
{
	dtNavMesh* m_p;
	dtNavMeshQuery* m_query;

	gkDetourNavMesh() : m_p(0),m_query(0) {}
	gkDetourNavMesh(dtNavMesh* p,dtNavMeshQuery* query) : m_p(p),m_query(query) {}
	~gkDetourNavMesh();
};

typedef gkPtrRef<gkDetourNavMesh> PDT_NAV_MESH;
typedef std::vector<gkVector3> PATH_POINTS;

enum SamplePartitionType
{
	SAMPLE_PARTITION_WATERSHED,
	SAMPLE_PARTITION_MONOTONE,
	SAMPLE_PARTITION_LAYERS,
};

struct gkRecast
{
	struct Config
	{
		gkScalar CELL_SIZE;
		gkScalar CELL_HEIGHT;
		gkScalar AGENT_MAX_SLOPE;
		gkScalar AGENT_HEIGHT;
		gkScalar AGENT_MAX_CLIMB;
		gkScalar AGENT_RADIUS;
		gkScalar EDGE_MAX_LEN;
		gkScalar EDGE_MAX_ERROR;
		gkScalar REGION_MIN_SIZE;
		gkScalar REGION_MERGE_SIZE;
		int VERTS_PER_POLY;
		gkScalar DETAIL_SAMPLE_DIST;
		gkScalar DETAIL_SAMPLE_ERROR;
		int TILE_SIZE;

		Config()
		{
			CELL_SIZE = 0.3f;
			CELL_HEIGHT = 0.20f;
			AGENT_MAX_SLOPE = 45.0f;
			AGENT_HEIGHT = 2.0f;
			AGENT_MAX_CLIMB = 0.1f;
			AGENT_RADIUS = 1.0f;
			EDGE_MAX_LEN = 12.0f;
			EDGE_MAX_ERROR = 1.3f;
			REGION_MIN_SIZE =8;
			REGION_MERGE_SIZE = 20;
			VERTS_PER_POLY = 6;
			DETAIL_SAMPLE_DIST = 6.0f;
			DETAIL_SAMPLE_ERROR = 1.0f;
			TILE_SIZE = 16;
//			CELL_SIZE = 0.17f;
//			CELL_HEIGHT = 0.10f;
//			AGENT_MAX_SLOPE = 45.0f;
//			AGENT_HEIGHT = 0.6f;
//			AGENT_MAX_CLIMB = 0.3f;
//			AGENT_RADIUS = 0.3f;
//			EDGE_MAX_LEN = 12.0f;
//			EDGE_MAX_ERROR = 1.3f;
//			REGION_MIN_SIZE = 50;
//			REGION_MERGE_SIZE = 20;
//			VERTS_PER_POLY = 6;
//			DETAIL_SAMPLE_DIST = 6.0f;
//			DETAIL_SAMPLE_ERROR = 1.0f;
//			TILE_SIZE = 16;
		}
	};

	static PDT_NAV_MESH createNavMesh(
	    PMESHDATA meshData,
	    const Config& config,
		gkScene* scene=0
	);

	static bool findPath(
	    PDT_NAV_MESH navMesh,
	    const gkVector3& from,
	    const gkVector3& to,
	    const gkVector3& polyPickExt,
	    int maxPathPolys,
	    PATH_POINTS& path,
	    unsigned short includeFlags = 0xffff,
	    unsigned short excludeFlags = 0
	);



};


#endif//_gkRecast_h_
