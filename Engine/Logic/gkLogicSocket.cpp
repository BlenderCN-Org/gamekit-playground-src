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
#include "gkLogicSocket.h"
#include "gkLogicNode.h"
#include "gkVariable.h"
#include "gkLogger.h"
#include <typeinfo>

#include "gkValue.h"

void gkILogicSocket::link(gkILogicSocket* fsock)
{
	GK_ASSERT(fsock);
//	gkLogger::write(gkString(typeid(*this).name())+" == "+gkString(typeid(*fsock).name()));
	GK_ASSERT(typeid(*this) == typeid(*fsock) && "Types have to match");

	if (m_isInput)
	{
		GK_ASSERT(!m_from && "Only one link for input socket");

		GK_ASSERT(!fsock->m_isInput && "Cannot link input to input");

		m_from = fsock;
	}
	else
	{
		GK_ASSERT(fsock->m_isInput && "Cannot link output to output");

		if (!m_to.find(fsock))
		{
			m_to.push_back(fsock);
		}
	}

	fsock->m_connected = m_connected = true;

	if (m_parent)
	{
		m_parent->setLinked();

		gkLogicNode* nd = fsock->getParent();

		if (nd) nd->setLinked();
	}
}

void gkILogicSocket::removeConnection(gkILogicSocket* fsock)
{
	if (m_isInput)
	{
		if (m_from == fsock)
		{
			m_from = 0;
			fsock->m_connected = false;
		}
		else
		{
			gkLogger::write("tried to remove illegal from-connection! from!=fsock",true);
		}
	}
	else
	{
		m_to.erase(fsock);
		if (m_to.size() == 0)
		{
			fsock->m_connected = false;
		}
	}
}

gkILogicSocket* gkILogicSocket::getOutSocketAt(int at)
{
	if (at < m_to.size())
	{
		return m_to.at(at);
	}
	gkLogger::write(
			"WARNING["+m_parent->getName()+
					"/"+getName()+"]: Index of of bound. Tried to access idx:"+gkToString(at),true);
	return 0;
}


gkGameObject* gkILogicSocket::getGameObject()const
{
	if (!m_isInput && m_usingSocketObject)
		return m_socketObject;

	if (m_from && m_from->m_parent)
		return m_from->getGameObject();

	return m_parent ? m_parent->getAttachedObject() : 0;
}

bool gkILogicSocket::_checkValidity()
{
	if (!isConnected())
	{
		if (m_needConnection)
		{
			gkLogger::write("Socket: "+getParent()->getName()+"."+getName()+": needs to be connected!");
			return m_isValid = false;
		}
		return m_isValid = true;
	}

	if (!m_isInput && m_maxOutConnections>0 && m_to.size()>m_maxOutConnections)
	{
		gkLogger::write("Socket: "+getParent()->getName()+"."+getName()+": Too many Outgoing Connections:"+gkToString((int)m_to.size()));
	}

	// do I want to allow "needed Connection" without specifiying valid nodes?
	// let's give it a try
	if (m_validTypeNames.size()==0)
		return m_isValid = true;

	// check if the connected
	if (m_isInput)
	{
		const gkString& connectionNodeType = getFrom()->getParent()->getType();
		bool found = m_validTypeNames.find(connectionNodeType.c_str()) != UT_NPOS;
		if (!found)
		{
			gkLogger::write("Socket "+getParent()->getName()+"."+getName()+": is connected to invalid node of type:"+connectionNodeType);
			return m_isValid = false;
		}
		return m_isValid = true;
	}
	else
	{
		// we have to check all outgoing nodes
		Sockets::Iterator iter(m_to);
		bool result=true;
		while (iter.hasMoreElements())
		{
			gkILogicSocket* sock = iter.getNext();
			gkLogicNode* parent = sock->getParent();
			const gkString& connectionNodeType = parent->getType();
			bool check = m_validTypeNames.find(connectionNodeType.c_str()) != UT_NPOS;
			if (!check)
			{
				gkLogger::write("Socket "+getParent()->getName()+"."+getName()+": is connected to invalid node of type:"+connectionNodeType);
				result = false;
				// we could break here, but let's see if there are more invalid connections
			}
		}
		return m_isValid = result;
	}
}
