/*
 * gkEvents.cpp
 *
 *  Created on: Feb 8, 2016
 *      Author: ttrocha
 */

#include <Events/gkEvents.h>

gkMessageEvent::gkMessageEvent(std::istringstream& in)
{
	in >> m_subtype;

	int size;
	in >> size;
	for (int i=0;i<size;i++)
	{
		gkString key;
		gkString value;
		int type;
		in >> type;
		in >> key;
		in >> value;

		gkVariable* var = new gkVariable(key);
		var->setValue(type,value);
		m_data.insert(key.c_str(),var);
	}
}

gkMessageEvent::~gkMessageEvent()
{
	for (int i=0;i<m_data.size();i++)
	{
		delete m_data.at(i);
	}
}

void gkMessageEvent::serialize(std::ostringstream& out) const
{
	out << m_subtype;
	out << m_data.size();
	for (int i=0;i<m_data.size();i++)
	{
		gkVariable* data = m_data.at(i);
		out << data->getType();
		out << gkString(m_data.keyAt(i).key());
		out << data->toString();
	}
}

const gkEventType gkMessageEvent::sk_EventTypePrefix("message");

void gkMessageEvent::setData(const gkString& key,gkVariable* var)
{
	gkVariable** currentVar = m_data.get(key.c_str());

	if (currentVar)
	{
		(*currentVar)->assign(*var);
	}
	else
	{
		gkVariable* newVar = new gkVariable(*var);
		m_data.insert(key.c_str(),newVar);
	}
}

gkVariable* gkMessageEvent::getData(const gkString& key) const
{
	gkVariable** result = m_data.get(key.c_str());
	if (result)
		return *result;
	return 0;
}

