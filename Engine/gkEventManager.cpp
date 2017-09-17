/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Xavier T.

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

#include <gkEventManager.h>

static IEventManager * globalEventMgr = NULL;

IEventManager * IEventManager::Get()
{
	return globalEventMgr;
}

gkEventManager::gkEventManager(const gkString& pName,bool setAsGlobal)
	: IEventManager(pName,setAsGlobal), m_activeQueue(0)
{
	tickClock = gkEngine::getSingleton().getTickClock();

	if (setAsGlobal)
		globalEventMgr = this;

	// create the listenertable for "*" (listening on all events)
	m_registry.insert(0,EventListenerTable());
}

gkEventManager::~gkEventManager()
{
	m_activeQueue = 0;
}

bool gkEventManager::addListener(IEventListener* inhandler,const gkEventType& inType) {
	if (inType.str().empty())
		return false;


	if ((inType.hash()==0) && inType.str()!="*"){
		return false;
	}


	bool foundType = m_typeList.find( inType ) != UT_NPOS;

	if (!foundType) // new type? add it to the list
	{
		m_typeList.insert(inType);
		m_registry.insert( inType.hash(), EventListenerTable() );
	}

	EventListenerTable* evtTbl = m_registry.get( inType.hash() );

	if (!evtTbl->find(inhandler))
	{
		evtTbl->push_back(inhandler);
		return true;
	}
	return false;
}

bool gkEventManager::removeListener(IEventListener* inListener, const gkEventType& inType)
{
	if (!validateType(inType))
		return false;

	m_registry.get(inType.hash())->erase(inListener);
	return true;
}

bool gkEventManager::trigger(const IEventData& inEvent)
{
	if (!validateType(inEvent.getEventType()))
		return false;


	bool processed = false;

	EventListenerTable* t;
	// first process the "*"-listeners ( that fire on every event)
	EventListenerTable* allListeners = m_registry.get(0);
	for (int i = 0;i < allListeners->size() ;i++)
	{
		allListeners->at(i)->handleEvent(inEvent);
	}

	// 2nd process the listeners coresponding to the type
	EventListenerTable* typeListeners = m_registry.get(inEvent.getEventType().hash());
	for (int i = 0;i < typeListeners->size() ;i++)
	{
		processed = processed | typeListeners->at(i)->handleEvent(inEvent);
	}

	return processed;
}

bool gkEventManager::queueEvent(const IEventDataPtr& inEvent)
{
	GK_ASSERT(m_activeQueue >= 0);
	GK_ASSERT(m_activeQueue < kNumQueues);

	if (!validateType(inEvent->getEventType()))
		return false;

	m_queues[m_activeQueue].push_back(inEvent);

	return true;
}


bool gkEventManager::abortEvent(const gkEventType& inType, bool allOfType)
{
	GK_ASSERT(m_activeQueue >= 0);
	GK_ASSERT(m_activeQueue < kNumQueues);

	if (!validateType(inType))
		return false;

	EventQueue& evtQueue = m_queues[m_activeQueue];

	bool result = false;

	for (int i=evtQueue.size()-1;i>=0;i--)
	{
		IEventDataPtr data = evtQueue.at(i);
		if (data->getEventType() == inType)
		{
			result = true;
			evtQueue.erase(evtQueue.link_at(i));
			if (!allOfType)
				break;

		}
	}
	return result;
}

bool gkEventManager::tick( unsigned long maxMillis)
{
	unsigned long int curMs = tickClock->getTimeMilliseconds();
	unsigned long int maxMs = (maxMillis == IEventManager::kINFINITE) ?
									IEventManager::kINFINITE : (curMs + maxMillis);

	EventListenerTable* asteriskListeners = m_registry.get(0);

	EventQueue& currentQueue = m_queues[m_activeQueue];

	// swap the queues so new events that e.g. a created inside other events won't get executed in this pass
	m_activeQueue = ( m_activeQueue + 1) % kNumQueues;
	m_queues[m_activeQueue].clear();

	while (currentQueue.size()>0)
	{
		IEventDataPtr event = currentQueue.front();
		currentQueue.pop_front();

		const gkEventType& eventType = event->getEventType();


		// first
		bool processed = trigger(*event.get());

		curMs = tickClock->getTimeMilliseconds();

		if (maxMillis != IEventManager::kINFINITE)
		{
			if (curMs >= maxMs)
			{
				break;
			}
		}
	}

	bool queueFlushed=true;

	while (currentQueue.size() >0)
	{
		queueFlushed = false;
		IEventDataPtr event = currentQueue.back();
		currentQueue.pop_back();
		m_queues[m_activeQueue].push_front(event);
	}

	return queueFlushed;
}

bool gkEventManager::validateType(const gkEventType& inType) const
{
	if (inType.str().empty())
		return false;


	if ((inType.hash()==0) && inType.str()!="*"){
		return false;
	}

	if (inType.str()!="*" && m_typeList.find(inType)==UT_NPOS)
	{
		return false;
	}

//	EventListenerMap::const_iterator itWC = m_registry.find(0);
//
//	EventTypeSet::const_iterator evIt = m_typeList.find(inType);
//
////	if (inType.hash()!=0 && itWC==m_registry.end() && evIt == m_typeList.end())
//	if (inType.hash()==0 && itWC==m_registry.end() && evIt == m_typeList.end())
//	{
//		GK_ASSERT(0 && "Failed validation of an eventype that is not registered");
//		return false;
//	}

	return true;
}

//EventListenerList gkEventManager::getListenerList(const gkEventType& eventType) const
//{
//	if (!validateType(eventType))
//		return EventListenerList();
//
//	EventListenerMap::const_iterator itListeners = m_registry.find(eventType.hash());
//
//	if (itListeners == m_registry.end())
//		return EventListenerList();
//
//	const EventListenerTable& table = itListeners->second;
//
//	if (table.size()==0)
//		return EventListenerList();
//
//	EventListenerList result;
//	result.reserve(table.size());
//
//	for (EventListenerTable::const_iterator it=table.begin(),end=table.end();
//			it!=end; it++)
//	{
//		result.push_back(*it);
//	}
//
//	return result;
//}
//
//EventTypeList gkEventManager::getTypeList(void) const
//{
//	if (m_typeList.size() == 0)
//		return EventTypeList();
//
//	EventTypeList result;
//	result.reserve(m_typeList.size() );
//
//	for(EventTypeSet::const_iterator it = m_typeList.begin(),itEnd=m_typeList.end();
//			it!=itEnd;it++)
//	{
//		result.push_back(*it);
//	}
//
//	return result;
//}

bool safeAddListener(IEventListener* inL, const gkEventType& inType)
{
	GK_ASSERT(IEventManager::Get() && ("No Eventmanager"));
	return IEventManager::Get()->addListener(inL,inType);
}

bool safeRemoveListener(IEventListener* inL, const gkEventType& inType)
{
	GK_ASSERT(IEventManager::Get() && ("No Eventmanager"));
	return IEventManager::Get()->removeListener(inL,inType);
}

bool safeTriggerEvent(const IEventData& inEvent)
{
	GK_ASSERT(IEventManager::Get() && ("No Eventmanager"));
	return IEventManager::Get()->trigger(inEvent);
}

bool safeQueueEvent(const IEventDataPtr& inEvent)
{
	GK_ASSERT(IEventManager::Get() && ("No Eventmanager"));
	return IEventManager::Get()->queueEvent(inEvent);
}
bool safeAbortEvent(const gkEventType& inType, bool allOfType)
{
	GK_ASSERT(IEventManager::Get() && ("No Eventmanager"));
	return IEventManager::Get()->abortEvent(inType,allOfType);
}
bool safeTickEventManager( unsigned long maxMillis)
{
	GK_ASSERT(IEventManager::Get() && ("No Eventmanager"));
	return IEventManager::Get()->tick(maxMillis);
}
bool safeValidationEventType( const gkEventType& inType)
{
	GK_ASSERT(IEventManager::Get() && ("No Eventmanager"));
	return IEventManager::Get()->validateType(inType);
}

// -----------------------------------
const gkEventType TestEvent_DestroyActor::sk_EventType("destory_actor");
const gkEventType Event_Tick::sk_EventType("tick");

