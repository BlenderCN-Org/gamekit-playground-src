/*
 * gkEvents.h
 *
 *  Created on: Feb 8, 2016
 *      Author: ttrocha
 */

#ifndef ENGINE_EVENTS_GKEVENTS_H_
#define ENGINE_EVENTS_GKEVENTS_H_

#include <gkEventManager.h>



class gkMessageEvent : public gkBaseEventData
{
public:
	virtual const gkEventType& getEventType(void) const { return m_typeName;}
	explicit gkMessageEvent(const gkString& subtype) : m_subtype(subtype),m_typeName( (sk_EventTypePrefix.str()+"_"+subtype) ) {}
	explicit gkMessageEvent(std::istringstream& in);
	virtual ~gkMessageEvent();

	virtual void serialize(std::ostringstream& out) const;

	void setData(const gkString& key,gkVariable* var);
	gkVariable* getData(const gkString& key) const;
	static const gkEventType sk_EventTypePrefix;
private:
	gkString m_subtype;

	typedef utHashTable<utCharHashKey,gkVariable*> DataMap;

	DataMap m_data;
	const gkEventType m_typeName;
};


#endif /* ENGINE_EVENTS_GKEVENTS_H_ */
