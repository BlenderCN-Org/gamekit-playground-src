/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C & Nestor Silveira

    Contributor(s): none yet
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
#ifndef _gkLogicSocket_h_
#define _gkLogicSocket_h_

#include "gkLogicCommon.h"

#include "gkLogger.h"
#include "gkLogicNode.h"

#include "gkValue.h"

#include "gkVariable.h"


#if ( (defined WIN32 && !__MINGW32__) || CPP11)
# include <type_traits>
#else
# include <tr1/type_traits>
#endif

#include "Logic/gkNodeManager.h"
//template <class T>
//struct group_number : _integral_const<int, 0> {};
//
//struct group_num : integral_constant<int,1> {};
//
//#define GROUP_TYPE(GRP_ID,TYPE)\
//template <>\
//struct group_number<TYPE> : _integral_const<int, GRP_ID> {};
//
//GROUP_TYPE(1,int);
//GROUP_TYPE(1,float);
//GROUP_TYPE(1,bool);
//GROUP_TYPE(1,gkString);


class gkILogicSocket
{
public:

	gkILogicSocket(bool useVar=false)
		: 	m_isInput(true), m_from(0), m_connected(false), m_parent(0), m_var(0)
			, m_useVar(useVar),m_refs(0),m_isDirty(false),m_isValid(true),m_needConnection(false)
			,m_maxOutConnections(0),m_socketObject(0),m_usingSocketObject(false) // zero = unlimited
	{
		if (useVar)
			m_var = new gkVariable();

	}

	gkILogicSocket(gkLogicNode* par, bool isInput,bool useVar=false)
		: 	m_isInput(isInput), m_from(0), m_connected(false), m_parent(par), m_var(0), m_useVar(useVar),m_isDirty(false),m_refs(0)
			,m_isValid(true),m_needConnection(false),m_maxOutConnections(0),m_socketObject(0),m_usingSocketObject(false)
	{
		if (useVar)
			m_var = new gkVariable();
	}

	virtual ~gkILogicSocket() { if (m_var) delete m_var;};

	void link(gkILogicSocket* fsock);
	void removeConnection(gkILogicSocket* fsock);
	GK_INLINE void _setIsInput(bool b) { m_isInput = b;}
	GK_INLINE bool isInputSocket() {return m_isInput;}
	virtual gkString getPropType() { return "";}

	GK_INLINE void _setNeedsConnection(bool needsConnections) { m_needConnection = needsConnections; }
	GK_INLINE void _addValidConnectionNode(const gkString& nodeName) { m_validTypeNames.insert(nodeName.c_str());}
	GK_INLINE void _setMaxOutConnections(int maxCon) { m_maxOutConnections = maxCon; }

	virtual bool _checkValidity();
	GK_INLINE bool isValid() { return m_isValid; }

	gkGameObject* getGameObject()const;
	GK_INLINE void setSocketObject(gkGameObject* gobj) { m_socketObject = gobj; m_usingSocketObject=true; }

	// owner node
	GK_INLINE gkLogicNode* getParent() const
	{
		return m_parent;
	}

	// what is the difference between connected and linked?
	GK_INLINE bool isLinked() const
	{
		return m_from != 0;
	}

	GK_INLINE bool isConnected() const
	{
		return m_connected;
	}

	GK_INLINE gkILogicSocket* getFrom() const
	{
		return m_from;
	}



	GK_INLINE void setName(const gkString& name)
	{
		m_name = name;
	}

	GK_INLINE gkString getName()
	{
		return m_name;
	}

	GK_INLINE int getOutSocketAmount()
	{
		return m_to.size();
	}

	GK_INLINE bool isUsingVar() { return m_useVar;}

	GK_INLINE gkVariable* getVar(bool resolveLink=false)
	{
		if (!m_useVar)
			return 0;

		if (resolveLink && m_from)
		{
			gkVariable* var = m_from->getVar();
			return var;
		}

		return m_var;
	}

	GK_INLINE void setDirty() {
		m_isDirty = true;
		// since we are dirty again, do not clear the dirty-flag from an already set clear-dirty-request
		cancelClearDirty();
	}

	void requestClearDirty() {
		gkNodeManager::getSingleton().addSocketToClearList(this);
	}

	void cancelClearDirty() {
		gkNodeManager::getSingleton().removeSocketFromClearList(this);
	}

	void forceClear() {
		m_isDirty = false;
	}

	bool isDirty() {
		return m_isDirty;
	}

	void setVar(gkVariable* newVar)
	{
		if (!m_isInput)
		{
			SocketIterator sockit(m_to);

			while (sockit.hasMoreElements())
			{
				gkILogicSocket* isock = sockit.getNext();
				if (isock->isUsingVar())
				{
//					gkString propType = isock->getPropType();
//					if (propType=="bool")
//					{
						gkVariable* curval = isock->getVar();

						if (curval != newVar)
						{
							curval->assign(*newVar);
							isock->setDirty();
						}
						//					}
				}
				else
				{
					gkLogger::write("You cannot use setVar on socket:"+isock->getName());
				}

			}
		}

		// if the data is set intentionally, the dirty flag will be set no matter if the value changed
		if (m_useVar)
		{
			if (m_var != newVar)
			{
				m_var->assign(*newVar);
				setDirty();
			}
		}
		else
		{
			gkLogger::write("You cannot use setVar on socket:"+getName());
		}

	}


//	void updateVar()
//	{
//		if (!m_isInput)
//		{
//			SocketIterator sockit(m_to);
//
//			while (sockit.hasMoreElements())
//			{
//				gkILogicSocket* isock = sockit.getNext();
//				if (isock->isUsingVar())
//				{
//					isock->getVar()->assign(*m_var);
//				}
//				GK_ASSERT(sock && "Types have to match");
//			}
//		}
//	}

	gkILogicSocket* getOutSocketAt(int at);

	int addRef() { return ++m_refs;}
	int removeRef() { return --m_refs;};

protected:
	gkString m_name;

	bool m_isInput;
	bool m_isDirty;

	// validity flags
	typedef utHashSet<const char*> ValidTypes;
	bool m_isValid;
	bool m_needConnection;
	int m_maxOutConnections;
	ValidTypes	m_validTypeNames; // a set of node-name which are accept by this socket

	typedef utList<gkILogicSocket*> Sockets;
	typedef utListIterator<Sockets> SocketIterator;

	// from socket to 'this' (used to link an input socket with an output socket)
	// Only one makes sense
	gkILogicSocket* m_from;

	// from 'this' to sockets (used to link an output socket with one or more than one input socket)
	Sockets m_to;
	gkVariable* m_var; // alternative
	bool m_useVar;

	gkGameObject* m_socketObject;
	bool 		  m_usingSocketObject;

private:
	int m_refs;
	bool m_connected;
	// owner node
	gkLogicNode* m_parent;
};

template<typename T>
class gkLogicSocket : public gkILogicSocket
{
public:
	gkLogicSocket(bool useVar=false)
		: gkILogicSocket(useVar)
	{
	}

	gkLogicSocket(gkLogicNode* par, bool isInput, T defaultValue, bool useVar=false)
		: gkILogicSocket(par, isInput,useVar), m_data(defaultValue)
	{
	}



	void setValue(const T& value)
	{

		if (!m_isInput)
		{
			SocketIterator sockit(m_to);

			while (sockit.hasMoreElements())
			{
				gkILogicSocket* isock = sockit.getNext();
				if (isock->isUsingVar())
				{
//					gkString propType = isock->getPropType();
//					if (propType=="bool")
//					{
						T curval = isock->getVar()->get<T>();

						if (curval != value)
						{
							isock->getVar()->setValue(value);
							isock->setDirty();
						}
						//					}
				}
				else
				{
					if (value != m_data)
					{
						gkLogicSocket<T>* sock = dynamic_cast<gkLogicSocket<T>*>(isock);
						sock->setValue(value);
						// dirty will be set in the sockets setValue-call
					}
				}

			}
		}

		// if the data is set intentionally, the dirty flag will be set no matter if the value changed
		if (m_useVar)
		{
			T curval = m_var->get<T>();

			if (curval != value)
			{
				m_var->setValue(value);
				setDirty();
			}
		}
		else
		{
			if (m_data != value)
			{
				m_data = value;
				setDirty();
			}
		}
	}


	GK_INLINE T getValue() const
	{
		if (m_from)
		{
			if (m_from->isUsingVar())
			{
				return m_from->getVar()->get<T>();
				//				return m_from->getVar()->get<T>();
			}
			else
			{
				gkLogicSocket<T>* sock = dynamic_cast<gkLogicSocket<T>*>(m_from);

				GK_ASSERT(sock && "Types have to match");

				return sock->getValue();
			}
		}
		if (m_useVar)
		{
			return m_var->get<T>();
		}
		else
		{
			return m_data;
		}
	}

	T& getRefValue()
	{
//		if (m_from)
//		{
//			gkLogicSocket<T>* sock = dynamic_cast<gkLogicSocket<T>*>(m_from);
//
//			GK_ASSERT(sock && "Types have to match");
//
//			return sock->getRefValue();
//		}

		if (m_useVar)
		{
			return m_var->get<T>();
		}
		else
		{
			return m_data;
		}
	}


private:
	T m_data;

};

template<typename T>
gkLogicSocket<T>* getSocket(gkILogicSocket* pSock)
{
	return static_cast<gkLogicSocket<T>*>(pSock);
}

#define ADD_ISOCK( name, value ){      \
    addISock < _SOCKET_TYPE_##name > ( m_sockets[ (name) ], value, #name );    \
}

#define ADD_ISOCK_VAR( name, value ){      \
    addISock < _SOCKET_TYPE_##name > ( m_sockets[ (name) ], value, #name, true );    \
}


#define ADD_OSOCK(name, value){      \
    addOSock< _SOCKET_TYPE_##name > ( m_sockets[ (name) ], value, #name );    \
}

#define ADD_OSOCK_VAR(name, value){      \
    addOSock< _SOCKET_TYPE_##name > ( m_sockets[ (name) ], value, #name, true );    \
}


#define SET_SOCKET_VALUE( name, value )    \
    setSocketValue< _SOCKET_TYPE_##name > ( (name), (value) )    \

#define GET_SOCKET_VALUE( name )      \
    getSocketValue< _SOCKET_TYPE_##name > ( (name) )    \

#define GET_SOCKET_REF_VALUE( name )      \
    getSocketRefValue< _SOCKET_TYPE_##name > ( (name) )    \

#define GET_SOCKET( name )      \
    getSocket< _SOCKET_TYPE_##name > ( (name) )    \

#define DECLARE_SOCKET_TYPE( name, type)    \
    typedef type _SOCKET_TYPE_##name;    \
    GK_INLINE gkLogicSocket< type >* get##name(bool requestClear=false) \
	{  \
		gkLogicSocket< type >* result=getSocket< type > ( (name) ); \
		if (requestClear && result->isDirty()) result->requestClearDirty(); \
		return result;\
    } \



#define CHECK_RETV(cond) {                  \
    if ((cond)) return;                     \
}

#endif//_gkLogicSocket_h_
