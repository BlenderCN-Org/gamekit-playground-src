/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2014 Thomas Trocha using GameCodingComplete 3rd Edition

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

#ifndef _gkEventManager_h_
#define _gkEventManager_h_

#include "gkCommon.h"
#include "utSingleton.h"

#include "Thread/gkPtrRef.h"

#include "utTypes.h"

#include "gkString.h"
class btClock;

typedef gkHashedString gkEventType;

class IEventData : public gkReferences
{
public:
	IEventData(){};
	virtual ~IEventData(){};
	virtual const gkEventType& getEventType(void) const = 0;
	virtual float getTimestamp() const = 0;
	virtual void serialize(std::ostringstream &out) const = 0;
};

typedef gkPtrRef<IEventData> IEventDataPtr;

class gkBaseEventData : public IEventData
{
public:
	explicit gkBaseEventData( const float timestamp = 0.0f)
		:	m_timestamp (timestamp)
	{}

	virtual ~gkBaseEventData() {}
	virtual const gkEventType& getEventType(void) const = 0;
	float getTimestamp(void) const { return m_timestamp;}
	virtual void serialize(std::ostringstream &out) const = 0;
protected:
	const float m_timestamp;
};




// ----------------------------- TEST EVENT -----


typedef unsigned int ActorId;

class TestEvent_DestroyActor : public gkBaseEventData
{
	static const gkEventType sk_EventType;

	virtual const gkEventType& getEventType(void) const {
		return sk_EventType;
	}



	explicit TestEvent_DestroyActor(ActorId aid)
		: m_id(aid)
	{}

	explicit TestEvent_DestroyActor(std::istringstream& in)
	{
		in >> m_id;
	}
	virtual ~TestEvent_DestroyActor() {}

	virtual void serialize(std::ostringstream& out)
	{
		out << m_id;
	}

	ActorId m_id;
};


struct Event_Tick : public gkBaseEventData
{
	static const gkEventType sk_EventType;

	virtual const gkEventType& getEventType(void) const {
		return sk_EventType;
	}

	explicit Event_Tick(float dt)
		: m_deltaTime(dt)
	{}

	explicit Event_Tick(std::istringstream& in)
	{
		in >> m_deltaTime;
	}

	virtual void serialize(std::ostringstream& out) const
	{
		out << m_deltaTime;
	}

	float m_deltaTime;
};
// -----------------------------------------------------------------------


class IEventListener : public gkReferences
{
public:
	explicit IEventListener() {}
	virtual ~IEventListener() {}
	virtual gkString const & getListenerName(void) = 0;

	virtual bool handleEvent( IEventData const & event)
	{
		return true;
	}
};

typedef gkPtrRef<IEventListener> IEventListenerPtr;


//class TestListener : public IEventListener
//{
//public:
//	TestListener() : m_name("TEST_STRING"){}
//	virtual ~TestListener() {}
//
//	const gkString& getName() { return m_name;}
//
//	bool handleEvent(const IEventData& data)
//	{
//		if (data.getEventType()=="tick"){
//			 const Event_Tick& tickData = static_cast<const Event_Tick&>(data);
////			 gkLogger::write("Dt:"+gkToString(tickData.m_deltaTime));
//		}
//		else gkLogger::write("Handle DATA!",true);
//		return false;
//	}
//private:
//	const gkString m_name;
//};

class IEventManager
{
public:
	enum eConstants {	kINFINITE = 0xffffffff };

	explicit IEventManager( const gkString& pName, bool setAsGlobal){};
	virtual ~IEventManager(){};

	virtual bool addListener(IEventListener* inhandler, const gkEventType& inType) = 0;
	virtual bool removeListener(IEventListener* inhandler,const gkEventType& inType) = 0;
	virtual bool trigger(const IEventData& inEvent) = 0;
	virtual bool queueEvent(const IEventDataPtr& inEvent) = 0;
	virtual bool abortEvent(const gkEventType& inEvent, bool allOfType = false) = 0;
	virtual bool tick(unsigned long maxMillis = kINFINITE) = 0;
	virtual bool validateType(const gkEventType& inType) const = 0;

	static IEventManager* Get();
private:

	friend bool safeAddListener(IEventListener* listener, const gkEventType& inType);
	friend bool safeRemoveListener(IEventListener* listener, const gkEventType& inType);
	friend bool safeTriggerEvent(const IEventData& inEvent);
	friend bool safeQueueEvent(const IEventDataPtr& inEvent);
	friend bool safeAbortEvent(const gkEventType& inType, bool allOfType /*= false*/ );
	friend bool safeTickEventManager( unsigned long maxMillis/* = IEventManager::kINFINITE*/);
	friend bool safeValidationEventType( const gkEventType& inType);
};

bool safeAddListener(IEventListener* inL, const gkEventType& inType);

bool safeRemoveListener(IEventListener* inL, const gkEventType& inType);

bool safeTriggerEvent(const IEventData& inEvent);

bool safeQueueEvent(const IEventDataPtr& inEvent);
bool safeAbortEvent(const gkEventType& inType, bool allOfType);
bool safeTickEventManager( unsigned long maxMillis);
bool safeValidationEventType( const gkEventType& inType);

typedef utList<IEventListener*> EventListenerList;
typedef utList<gkEventType> EventTypeList;

class gkEventManager : public IEventManager
{
public:
	explicit gkEventManager( const gkString& pName, bool setAsGlobal);
	~gkEventManager();
	bool addListener(IEventListener* inhandler, const gkEventType& inType);
	bool removeListener(IEventListener* inhandler,const gkEventType& inType);
	bool trigger(const IEventData& inEvent);
	bool queueEvent(const IEventDataPtr& inEvent);
	bool abortEvent(const gkEventType& inEvent, bool allOfType = true);
	bool tick(unsigned long maxMillis = kINFINITE);
	bool validateType(const gkEventType& inType) const ;

//	const EventListenerList& getListenerList(const gkEventType& eventType) const;
//	EventTypeList getTypeList(void) const;

private:

	typedef utHashSet<gkEventType> EventTypeSet;
//	typedef std::set<gkEventType> EventTypeSet;
//	typedef std::pair<EventTypeSet::iterator,bool> EvenTypeSetIRes;

	typedef utList<IEventListener*> EventListenerTable;
//	typedef std::list<IEventListenerPtr> EventListenerTable;


	typedef utHashTable<utIntHashKey,EventListenerTable> EventListenerMap;
//	typedef std::map<unsigned int, EventListenerTable> EventListenerMap;
//	typedef std::pair<unsigned int,EventListenerTable> EventListenerMapEnt;
//	typedef std::pair<EventListenerMap::iterator,bool> EventListenerMapIRes;

	typedef utList<IEventDataPtr> EventQueue;
//	typedef std::list<IEventDataPtr> EventQueue;


	enum eConstants { kNumQueues = 2 };

	EventTypeSet m_typeList;
	EventListenerMap m_registry;
	EventQueue m_queues[kNumQueues];
	int m_activeQueue;
	btClock* tickClock;
};


#endif // _gkEventManager_h_
