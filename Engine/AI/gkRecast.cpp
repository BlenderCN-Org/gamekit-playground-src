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
#include "btBulletDynamicsCommon.h"
#include "gkRecast.h"
#include "gkMeshData.h"
#include "gkEngine.h"
#include "gkScene.h"
#include "gkLogger.h"
#include "gkActiveObject.h"
#include "gkGameObject.h"

#include "Recast.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "DebugDraw.h"
#include "RecastDebugDraw.h"
#include <memory>

#include "gkValue.h"
BuildContext::BuildContext() :
	m_messageCount(0),
	m_textPoolSize(0)
{
	resetTimers();
}

BuildContext::~BuildContext()
{
}

// Virtual functions for custom implementations.
void BuildContext::doResetLog()
{
	m_messageCount = 0;
	m_textPoolSize = 0;
}

void BuildContext::doLog(const rcLogCategory category, const char* msg, const int len)
{
	if (!len) return;
	if (m_messageCount >= MAX_MESSAGES)
		return;
	char* dst = &m_textPool[m_textPoolSize];
	int n = TEXT_POOL_SIZE - m_textPoolSize;
	if (n < 2)
		return;
	char* cat = dst;
	char* text = dst+1;
	const int maxtext = n-1;
	// Store category
	*cat = (char)category;
	// Store message
	const int count = rcMin(len+1, maxtext);
	memcpy(text, msg, count);
	text[count-1] = '\0';
	m_textPoolSize += 1 + count;
	m_messages[m_messageCount++] = dst;
}

void BuildContext::doResetTimers()
{
	for (int i = 0; i < RC_MAX_TIMERS; ++i)
		m_accTime[i] = -1;
}

void BuildContext::doStartTimer(const rcTimerLabel label)
{
	m_startTime[label] = getPerfTime();
}

void BuildContext::doStopTimer(const rcTimerLabel label)
{
	const TimeVal endTime = getPerfTime();
	const int deltaTime = (int)(endTime - m_startTime[label]);
	if (m_accTime[label] == -1)
		m_accTime[label] = deltaTime;
	else
		m_accTime[label] += deltaTime;
}

int BuildContext::doGetAccumulatedTime(const rcTimerLabel label) const
{
	return m_accTime[label];
}

void BuildContext::dumpLog(const char* format, ...)
{
	// Print header.
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
	printf("\n");

	// Print messages
	const int TAB_STOPS[4] = { 28, 36, 44, 52 };
	for (int i = 0; i < m_messageCount; ++i)
	{
		const char* msg = m_messages[i]+1;
		int n = 0;
		while (*msg)
		{
			if (*msg == '\t')
			{
				int count = 1;
				for (int j = 0; j < 4; ++j)
				{
					if (n < TAB_STOPS[j])
					{
						count = TAB_STOPS[j] - n;
						break;
					}
				}
				while (--count)
				{
					putchar(' ');
					n++;
				}
			}
			else
			{
				putchar(*msg);
				n++;
			}
			msg++;
		}
		putchar('\n');
	}
}

int BuildContext::getLogCount() const
{
	return m_messageCount;
}

const char* BuildContext::getLogText(const int i) const
{
	return m_messages[i]+1;
}


gkDetourNavMesh::~gkDetourNavMesh()
{
	if (m_p) {
		delete m_p;
		m_p=0;
	}

	if (m_query)
	{
		delete m_query;
		m_query=0;
	}
}



PDT_NAV_MESH gkRecast::createNavMesh(PMESHDATA meshData, const Config& config, gkScene* scene)
{
	BuildContext m_ctx;
	bool m_keepInterResults = false;

	if (!meshData.get())
		return PDT_NAV_MESH(0);

	if (!scene)
		scene = gkEngine::getSingleton().getActiveScene();

	rcConfig cfg;

	cfg.cs = config.CELL_SIZE;
	cfg.ch = config.CELL_HEIGHT;

	GK_ASSERT(cfg.ch && "cfg.ch cannot be zero");
	GK_ASSERT(cfg.ch && "cfg.ch cannot be zero");

	cfg.walkableSlopeAngle = config.AGENT_MAX_SLOPE;
	cfg.walkableHeight = (int)ceilf(config.AGENT_HEIGHT / cfg.ch);
	cfg.walkableClimb = (int)ceilf(config.AGENT_MAX_CLIMB / cfg.ch);
	cfg.walkableRadius = (int)ceilf(config.AGENT_RADIUS / cfg.cs);
	cfg.maxEdgeLen = (int)(config.EDGE_MAX_LEN / cfg.cs);
	cfg.maxSimplificationError = config.EDGE_MAX_ERROR;
	cfg.minRegionArea = (int)rcSqr(config.REGION_MIN_SIZE);
	cfg.mergeRegionArea = (int)rcSqr(config.REGION_MERGE_SIZE);
	cfg.maxVertsPerPoly = gkMin(config.VERTS_PER_POLY, DT_VERTS_PER_POLYGON);
	cfg.tileSize = config.TILE_SIZE;
	cfg.borderSize = cfg.walkableRadius + 4; // Reserve enough padding.
	cfg.detailSampleDist = config.DETAIL_SAMPLE_DIST < 0.9f ? 0 : cfg.cs * config.DETAIL_SAMPLE_DIST;
	cfg.detailSampleMaxError = cfg.ch * config.DETAIL_SAMPLE_ERROR;

	if (!meshData->getVertCount())
		return PDT_NAV_MESH(0);

	gkScalar bmin[3], bmax[3];

	const gkScalar* verts = meshData->getVerts();
	int nverts = meshData->getVertCount();
	const int* tris = meshData->getTris();
	const gkScalar* trinorms = meshData->getNormals();
	int ntris = meshData->getTriCount();

	gkLogger::write("Create navmesh: nverts:"+gkToString(nverts)+" ntris:"+gkToString(ntris));

	rcCalcBounds(verts, nverts, bmin, bmax);

	//
	// Step 1. Initialize build config.
	//

	// Set the area where the navigation will be build.
	// Here the bounds of the input mesh are used, but the
	// area could be specified by an user defined box, etc.
	rcVcopy(cfg.bmin, bmin);
	rcVcopy(cfg.bmax, bmax);
	rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);

	// Reset build times gathering.
	m_ctx.resetTimers();

	// Start the build process.
	m_ctx.startTimer(RC_TIMER_TOTAL);

	m_ctx.log(RC_LOG_PROGRESS, "Building navigation:");
	m_ctx.log(RC_LOG_PROGRESS, " - %d x %d cells", cfg.width, cfg.height);
	m_ctx.log(RC_LOG_PROGRESS, " - %.1fK verts, %.1fK tris", nverts/1000.0f, ntris/1000.0f);

	//
	// Step 2. Rasterize input polygon soup.
	//

	// Allocate voxel heightfield where we rasterize our input data to.
	rcHeightfield* m_solid = rcAllocHeightfield();
	if (!m_solid)
	{
		m_ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
		return PDT_NAV_MESH(0);;
	}
	if (!rcCreateHeightfield(&m_ctx, *m_solid, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs, cfg.ch))
	{
		m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
		return PDT_NAV_MESH(0);
	}


//	{
//		// Allocate array that can hold triangle flags.
//		// If you have multiple meshes you need to process, allocate
//		// and array which can hold the max number of triangles you need to process.
//
//		utArray<unsigned char> triflags;
//		triflags.resize(ntris);
//
//		// Find triangles which are walkable based on their slope and rasterize them.
//		// If your input data is multiple meshes, you can transform them here, calculate
//		// the flags for each of the meshes and rasterize them.
//		memset(triflags.ptr(), 0, ntris * sizeof(unsigned char));
//		rcMarkWalkableTriangles(cfg.walkableSlopeAngle, verts, nverts, tris, ntris, triflags.ptr());
//		rcRasterizeTriangles(verts, nverts, tris, triflags.ptr(), ntris, heightField);
//	}

	// Allocate array that can hold triangle area types.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	unsigned char* m_triareas = new unsigned char[ntris];
	if (!m_triareas)
	{
		gkPrintf("buildNavigation: Out of memory 'm_triareas' (%d).", ntris);
		return PDT_NAV_MESH(0);
	}

	// Find triangles which are walkable based on their slope and rasterize them.
	// If your input data is multiple meshes, you can transform them here, calculate
	// the are type for each of the meshes and rasterize them.
	memset(m_triareas, 0, ntris*sizeof(unsigned char));
	rcMarkWalkableTriangles(&m_ctx, cfg.walkableSlopeAngle, verts, nverts, tris, ntris, m_triareas);
	rcRasterizeTriangles(&m_ctx, verts, nverts, tris, m_triareas, ntris, *m_solid, cfg.walkableClimb);

	if (!m_keepInterResults)
	{
		delete [] m_triareas;
		m_triareas = 0;
	}

	//
	// Step 3. Filter walkables surfaces.
	//

	// Once all geoemtry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	rcFilterLowHangingWalkableObstacles(&m_ctx, cfg.walkableClimb, *m_solid);
	rcFilterLedgeSpans(&m_ctx, cfg.walkableHeight, cfg.walkableClimb, *m_solid);
	rcFilterWalkableLowHeightSpans(&m_ctx, cfg.walkableHeight, *m_solid);


	//
	// Step 4. Partition walkable surface to simple regions.
	//

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	rcCompactHeightfield* m_chf = rcAllocCompactHeightfield();
	if (!m_chf)
	{
		gkPrintf("buildNavigation: Out of memory 'chf'.");
		return PDT_NAV_MESH(0);
	}
	if (!rcBuildCompactHeightfield(&m_ctx, cfg.walkableHeight, cfg.walkableClimb, *m_solid, *m_chf))
	{
		gkPrintf("buildNavigation: Could not build compact data.");
		return PDT_NAV_MESH(0);
	}

	if (!m_keepInterResults)
	{
		rcFreeHeightField(m_solid);
		m_solid = 0;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(&m_ctx, cfg.walkableRadius, *m_chf))
	{
		m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
		return PDT_NAV_MESH(0);
	}


	//
	// Mark areas from objects
	//


// TODO: Take it in again:

	gkGameObjectSet& objects = scene->getInstancedObjects();
	gkGameObjectSet::Iterator it = objects.iterator();
	while (it.hasMoreElements())
	{
		gkGameObject* obj = it.getNext();

		if (!obj->getNavData().isEmpty())
		{
			size_t tBaseIndex = obj->getNavData().triangleBaseIndex;
			size_t vBaseIndex = tBaseIndex / 2;

			const float* v = verts + vBaseIndex;
			const int nVerts = obj->getNavData().nIndex / 3;

			const gkGameObjectProperties& prop = obj->getProperties();

			rcMarkConvexPolyArea(&m_ctx,v, nVerts, obj->getNavData().hmin, obj->getNavData().hmax, prop.m_findPathFlag, *m_chf);
		}
	}

	// Partition the heightfield so that we can use simple algorithm later to triangulate the walkable areas.
	// There are 3 martitioning methods, each with some pros and cons:
	// 1) Watershed partitioning
	//   - the classic Recast partitioning
	//   - creates the nicest tessellation
	//   - usually slowest
	//   - partitions the heightfield into nice regions without holes or overlaps
	//   - the are some corner cases where this method creates produces holes and overlaps
	//      - holes may appear when a small obstacles is close to large open area (triangulation can handle this)
	//      - overlaps may occur if you have narrow spiral corridors (i.e stairs), this make triangulation to fail
	//   * generally the best choice if you precompute the nacmesh, use this if you have large open areas
	// 2) Monotone partioning
	//   - fastest
	//   - partitions the heightfield into regions without holes and overlaps (guaranteed)
	//   - creates long thin polygons, which sometimes causes paths with detours
	//   * use this if you want fast navmesh generation
	// 3) Layer partitoining
	//   - quite fast
	//   - partitions the heighfield into non-overlapping regions
	//   - relies on the triangulation code to cope with holes (thus slower than monotone partitioning)
	//   - produces better triangles than monotone partitioning
	//   - does not have the corner cases of watershed partitioning
	//   - can be slow and create a bit ugly tessellation (still better than monotone)
	//     if you have large open areas with small obstacles (not a problem if you use tiles)
	//   * good choice to use for tiled navmesh with medium and small sized tiles

	// TODO
	int m_partitionType = SAMPLE_PARTITION_WATERSHED;

	if (m_partitionType == SAMPLE_PARTITION_WATERSHED)
	{
		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if (!rcBuildDistanceField(&m_ctx, *m_chf))
		{
			gkPrintf("buildNavigation: Could not build distance field.");
			return PDT_NAV_MESH(0);
		}

		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(&m_ctx, *m_chf, 0, cfg.minRegionArea, cfg.mergeRegionArea))
		{
			gkPrintf("buildNavigation: Could not build watershed regions.");
			return PDT_NAV_MESH(0);
		}
	}
	else if (m_partitionType == SAMPLE_PARTITION_MONOTONE)
	{
		// Partition the walkable surface into simple regions without holes.
		// Monotone partitioning does not need distancefield.
		if (!rcBuildRegionsMonotone(&m_ctx, *m_chf, 0, cfg.minRegionArea, cfg.mergeRegionArea))
		{
			gkPrintf("buildNavigation: Could not build monotone regions.");
			return PDT_NAV_MESH(0);
		}
	}
	else // SAMPLE_PARTITION_LAYERS
	{
		gkLogger::write("gkRecast: UNKNOWN PARTITIONING!");
//		// Partition the walkable surface into simple regions without holes.
//		if (!rcBuildLayerRegions(&m_ctx, *m_chf, 0, cfg.minRegionArea))
//		{
//			gkPrintf("buildNavigation: Could not build layer regions.");
//			return PDT_NAV_MESH(0);
//		}
	}



	//
	// Step 5. Trace and simplify region contours.
	//

	// Create contours.
	rcContourSet* m_cset = rcAllocContourSet();
	if (!m_cset)
	{
		gkPrintf("buildNavigation: Out of memory 'cset'.");
		return PDT_NAV_MESH(0);
	}
	if (!rcBuildContours(&m_ctx, *m_chf, cfg.maxSimplificationError, cfg.maxEdgeLen, *m_cset))
	{
		gkPrintf("buildNavigation: Could not create contours.");
		return PDT_NAV_MESH(0);
	}

	//
	// Step 6. Build polygons mesh from contours.
	//

	// Build polygon navmesh from the contours.
	rcPolyMesh* m_pmesh = rcAllocPolyMesh();
	if (!m_pmesh)
	{
		gkPrintf("buildNavigation: Out of memory 'pmesh'.");
		return PDT_NAV_MESH(0);
	}
	if (!rcBuildPolyMesh(&m_ctx, *m_cset, cfg.maxVertsPerPoly, *m_pmesh))
	{
		gkPrintf("buildNavigation: Could not triangulate contours.");
		return PDT_NAV_MESH(0);
	}

	//
	// Step 7. Create detail mesh which allows to access approximate height on each polygon.
	//

	rcPolyMeshDetail* m_dmesh = rcAllocPolyMeshDetail();
	if (!m_dmesh)
	{
		gkPrintf("buildNavigation: Out of memory 'pmdtl'.");
		return PDT_NAV_MESH(0);
	}

	if (!rcBuildPolyMeshDetail(&m_ctx, *m_pmesh, *m_chf, cfg.detailSampleDist, cfg.detailSampleMaxError, *m_dmesh))
	{
		gkPrintf("buildNavigation: Could not build detail mesh.");
		return PDT_NAV_MESH(0);
	}

	if (!m_keepInterResults)
	{
		rcFreeCompactHeightfield(m_chf);
		m_chf = 0;
		rcFreeContourSet(m_cset);
		m_cset = 0;
	}


	// At this point the navigation mesh data is ready, you can access it from pmesh.
	// See rcDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.

	//
	// Step 8. Create Detour data from Recast poly mesh.
	//


	PDT_NAV_MESH navMesh;

	// Update poly flags from areas.
	for (int i = 0; i < m_pmesh->npolys; ++i)
		m_pmesh->flags[i] = 0xFFFF & m_pmesh->areas[i];

	dtNavMeshCreateParams params;
	memset(&params, 0, sizeof(params));
	params.verts = m_pmesh->verts;
	params.vertCount = m_pmesh->nverts;
	params.polys = m_pmesh->polys;
	params.polyAreas = m_pmesh->areas;
	params.polyFlags = m_pmesh->flags;
	params.polyCount = m_pmesh->npolys;
	params.nvp = m_pmesh->nvp;
	params.detailMeshes = m_dmesh->meshes;
	params.detailVerts = m_dmesh->verts;
	params.detailVertsCount = m_dmesh->nverts;
	params.detailTris = m_dmesh->tris;
	params.detailTriCount = m_dmesh->ntris;
	/*        params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
	    params.offMeshConRad = m_geom->getOffMeshConnectionRads();
	    params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
	    params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
	    params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
	    params.offMeshConCount = m_geom->getOffMeshConnectionCount();
	    */
	params.walkableHeight = cfg.walkableHeight * cfg.ch;
	params.walkableRadius = cfg.walkableRadius * cfg.cs;;
	params.walkableClimb = cfg.walkableClimb * cfg.ch;
	rcVcopy(params.bmin, m_pmesh->bmin);
	rcVcopy(params.bmax, m_pmesh->bmax);
	params.cs = cfg.cs;
	params.ch = cfg.ch;
	params.buildBvTree = true;

	unsigned char* navData = 0;
	int navDataSize = 0;

	if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
	{
		gkPrintf("Could not build Detour navmesh.");
		return PDT_NAV_MESH(0);
	}

	navMesh = PDT_NAV_MESH(new gkDetourNavMesh(new dtNavMesh,new dtNavMeshQuery));

//	if (!navMesh->m_p->init(navData, navDataSize, true, 2048))
//	{
//		delete [] navData;
//		gkPrintf("Could not init Detour navmesh");
//		return PDT_NAV_MESH(0);
//	}

	dtStatus status;

	status = navMesh->m_p->init(navData, navDataSize, DT_TILE_FREE_DATA);
	if (dtStatusFailed(status))
	{
		delete[] navData;
		gkPrintf("Could not init Detour navmesh");
		return PDT_NAV_MESH(0);
	}

	status = navMesh->m_query->init(navMesh->m_p, 2048);
	if (dtStatusFailed(status))
	{
		gkPrintf("Could not init Detour navmesh query");
		return PDT_NAV_MESH(0);
	}

	m_ctx.stopTimer(RC_TIMER_TOTAL);

	gkPrintf("Navigation mesh created: %.1fms", m_ctx.getAccumulatedTime(RC_TIMER_TOTAL) / 1000.0f);

//	scene->createNavMeshEntity("navmesh",dmesh);

	return navMesh;
}


bool gkRecast::findPath(PDT_NAV_MESH navMesh, const gkVector3& from, const gkVector3& to, const gkVector3& polyPickExt, int maxPathPolys, PATH_POINTS& path, unsigned short includeFlags, unsigned short excludeFlags)
{
	GK_ASSERT(!(includeFlags & excludeFlags) && "includeFlags with excludeFlags cannot overlap");

	if (navMesh.get() && navMesh->m_p)
	{
		gkVector3 startPos(from);
		gkVector3 endPos(to);

		std::swap(startPos.y, startPos.z);
		std::swap(endPos.y, endPos.z);

		dtQueryFilter filter;
		filter.setIncludeFlags(includeFlags);
		filter.setExcludeFlags(excludeFlags);

		dtPolyRef startRef, endRef;
		float nspos[3], nepos[3];

		navMesh->m_query->findNearestPoly(startPos.ptr(), polyPickExt.ptr(), &filter, &startRef,nspos);

		navMesh->m_query->findNearestPoly(endPos.ptr(), polyPickExt.ptr(), &filter, &endRef,nepos);

		if (startRef && endRef)
		{
			utArray<dtPolyRef> polys;
			polys.resize(maxPathPolys);


			int npolys;
			navMesh->m_query->findPath(startRef, endRef, startPos.ptr(), endPos.ptr(), &filter, polys.ptr(), &npolys,maxPathPolys);

			if (npolys > 1)
			{
				path.clear();

				for (int i=0;i<npolys*3;i +=3)
				{

					gkVector3 point;
					point.x=polys[i];
					point.y=polys[i+1];
					point.z=polys[i+2];
				}

				utArray<gkScalar> straightPath;
				straightPath.resize(maxPathPolys * 3);

				int nstraightPath;
				navMesh->m_query->findStraightPath(startPos.ptr(), endPos.ptr(),polys.ptr(), npolys,
													straightPath.ptr(), 0, 0, &nstraightPath,maxPathPolys);

				std::swap(startPos.y, startPos.z);
				std::swap(endPos.y, endPos.z);

				gkVector3 point;

				for (int i = 0; i < nstraightPath * 3; i += 3)
				{
					point.x = straightPath[i];
					point.y = straightPath[i+2];
					point.z = straightPath[i+1];

					if (path.size()==0 || path.at(path.size()-1)!=point)
						path.push_back(point);
				}

				return true;
			}
		}
	}

	return false;
}


