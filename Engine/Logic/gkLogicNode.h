/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): Nestor Silveira.
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
#ifndef _gkLogicNode_h_
#define _gkLogicNode_h_

#include <rapidjson/document.h>
#include "gkLogicCommon.h"
#include "gkLogicSocket.h"
#include "gkString.h"
#include "gkMathUtils.h"

#include "gkVariable.h"

#include "utTypes.h"
class gkGameObject;
class gkLogicTree;

#define NODESOCK_PREFIX_IN "IN_"
#define NODESOCK_PREFIX_OUT "OUT_"

class gkLuaEvent;

class gkLogicNode
{
	friend class gkLogicTree;
public:
	typedef utList<gkILogicSocket*>  Sockets;
	typedef utListIterator<Sockets> SocketIterator;
	typedef utHashTable<utCharHashKey,gkILogicSocket*> SocketByNameMap;
	typedef utHashTable<utCharHashKey,gkVariable*> PropertyByNameMap;
	static const int N_MAX_SOCKETS = 25;

	// the higher the execution-bucket the later the update-calls
	enum ExecutionBucket {
		BUCKET_SETTER=0,
		BUCKET_GETTER=10,
		BUCKET_EXECUTOR=20
	};

public:
	gkLogicNode(gkLogicTree* parent, UTsize handle, const gkString& name="");

	virtual ~gkLogicNode();

	// methods that get called by the nodetree directly
	// do first run initialization
	virtual void initialize();
	// do the update logic
	virtual void update(gkScalar tick);

	// see if an update is wanted
	virtual bool evaluate(gkScalar tick);

	// if you want lua to coexist override upcoming-methods:
	virtual void _initialize(){};
	virtual void _afterInit(){};
	virtual void _afterInit2(){};
	virtual void _update(gkScalar tick){};
	virtual void _preUpdate(){};
	virtual void _postUpdate(){};
	virtual bool _evaluate(gkScalar tick){ return true; };

	virtual bool debug(rapidjson::Value& val);

	virtual gkString getName() { return m_name; };

	// override this to check if all values are ok (the right inputs and such)
	virtual bool isValid() { return m_isValid; }
	virtual bool checkValidity();

	gkILogicSocket*          getInputSocket(UTsize index);
	gkILogicSocket*          getOutputSocket(UTsize index);

	GK_INLINE void          attachObject(gkGameObject* ob)  {m_object = ob;}
	GK_INLINE gkGameObject* getAttachedObject(void)         {return m_other != 0 ? m_other : m_object; }

	GK_INLINE const UTsize  getHandle(void)         {return m_handle;}
public:
	GK_INLINE bool          hasLinks(void)          {return m_hasLinks;}
	GK_INLINE void          setLinked(void)         {m_hasLinks = true;}
	GK_INLINE void          setPriority(int v)      {m_priority = v;}
	GK_INLINE int           getPriority(void)       {return m_priority;}
	GK_INLINE int			getBucketNr(void)		{return m_bucketNr;}
	GK_INLINE void			setBucketNr(int bucketNr) { m_bucketNr=bucketNr;}

	GK_INLINE Sockets& getInputs(void)  {return m_inputs;}
	GK_INLINE Sockets& getOutputs(void) {return m_outputs;}
	// TODO: STATIC OUTPUT
	virtual gkString getType() { return m_type;};
	virtual gkString getUUIDPropname() { return "";}
private:
	GK_INLINE void          setHandle(UTsize handle) { m_handle = handle; }

public:
	void setLuaInit(gkLuaEvent* init) { m_luaInit = init; }
	void setLuaEval(gkLuaEvent* eval) { m_luaEval = eval; }
	void setLuaUpdate(gkLuaEvent* update) { m_luaUpdate = update; }
	void setIgnoreLua(bool ignoreLua) { m_ignoreLUA = ignoreLua; }
protected:
	void setDebugStatus(rapidjson::Value& jsonNode,const gkString& statusmsg);
	void addDebugSocket(rapidjson::Value& val,const gkString& socketName, const gkString& valueType, const gkString& value,bool isInput);
	enum GetObjectMode {
		only_global, // only by name
		only_local, // if the attached object is a group-instance it will search in the group, otherwise global
		local_then_global
	};
	gkGameObject* getObject(const gkString& objName,GetObjectMode mode=local_then_global);
	gkLuaEvent* m_luaInit;
	gkLuaEvent* m_luaEval;
	gkLuaEvent* m_luaUpdate;
	bool m_ignoreLUA;

public:
	void addSocketData(gkILogicSocket* dest,const gkString name,bool isInput)
	{
		dest->_setIsInput(isInput);
		dest->setName(name);
		int idx = m_inputs.size() + m_outputs.size();
		m_sockets[idx]=dest;
		if (isInput)
		{
			m_inputs.push_back(dest);
			gkString iSockName = Ogre::StringUtil::startsWith(name,"IN_")
								?	name
								: 	"IN_"+name;
			m_socketNameMap.insert(iSockName.c_str(),dest);
		}
		else
		{
			m_outputs.push_back(dest);
			gkString iSockName = Ogre::StringUtil::startsWith(name,"OUT_")
								?	name
								: 	"OUT_"+name;
			m_socketNameMap.insert(iSockName.c_str(),dest);
		}
		dest->addRef();
	}
	gkILogicSocket* addDynSock(const gkString& name,bool isInput)
	{
		gkILogicSocket* dest = new gkILogicSocket(this,isInput,true);
		addSocketData(dest,name,isInput);
		return dest;
	}


	template<typename T>
	void addISock(gkILogicSocket* dest, T defaultValue,const gkString& name="",bool useVar=false)
	{
		dest = new gkLogicSocket<T>(this, true, defaultValue,useVar);
		addSocketData(static_cast<gkILogicSocket*>(dest),name,true);
//		dest->setName(name);
//		int idx = m_inputs.size() + m_outputs.size();
//
//		GK_ASSERT(idx < N_MAX_SOCKETS);
//
//		m_sockets[idx] = dest;
//
//		m_inputs.push_back(dest);
//		if (name!="")
//		{
//			gkString iSockName = Ogre::StringUtil::startsWith(name,"IN_")
//								?	name
//								: 	"IN_"+name;
//			m_socketNameMap.insert(iSockName.c_str(),dest);
//			gkLogger::write("Added inputSocket:"+iSockName,true);
//
//		}
	}

	template<typename T>
	void addOSock(gkILogicSocket* dest, T defaultValue,const gkString& name="",bool useVar=false)
	{

		dest = new gkLogicSocket<T>(this, false, defaultValue,useVar);
		addSocketData(dest,name,false);
//		dest->setName(name);
//
//		int idx = m_inputs.size() + m_outputs.size();
//
//		GK_ASSERT(idx < N_MAX_SOCKETS);
//
//		m_sockets[idx] = dest;
//
//		m_outputs.push_back(dest);
//		if (name!="")
//		{
//			gkString oSockName = Ogre::StringUtil::startsWith(name,"OUT_")
//								?	name
//								: 	"OUT_"+name;
//			m_socketNameMap.insert(oSockName.c_str(),dest);
//			gkLogger::write("Added outputSocket:"+oSockName,true);
//		}
	}

	template<typename T>
	gkLogicSocket<T>* getSocket(int idx)
	{
		GK_ASSERT(idx < N_MAX_SOCKETS);
		return static_cast<gkLogicSocket<T>*>(m_sockets[idx]);
	}


	gkILogicSocket* _getSocketByName(const gkString& name)
	{
		gkILogicSocket** result = m_socketNameMap.get(name.c_str());
		if (result)
		{
			return *result;
		}
		return 0;
	}

	gkILogicSocket* getInSocketByName(const gkString& name)
	{
		return _getSocketByName(NODESOCK_PREFIX_IN+name);
	}

	gkILogicSocket* getOutSocketByName(const gkString& name)
	{
		return _getSocketByName(NODESOCK_PREFIX_OUT+name);
	}

	gkVariable* getProperty(const gkString& name);

	GK_INLINE PropertyByNameMap& getPropertyMap() { return m_propertyNameMap; }

	void setProperty(const gkString& name, gkVariable* var)
	{
		if (m_propertyNameMap.find(name.c_str())==UT_NPOS)
		{
			m_propertyNameMap.insert(name.c_str(),var);
		}
	}



	template<typename T>
	void setProperty(const gkString& name, T value)
	{
		gkVariable* var = 0;

		int pos;
		if ((pos=m_propertyNameMap.find(name.c_str()))!=UT_NPOS)
		{
			var = m_propertyNameMap.at(pos);
		}
		else
		{
			var = new gkVariable();
			m_propertyNameMap.insert(name.c_str(),var);
		}
		var->setValue(value);
	}

	// change the name this socket is mapped to. this is needed if
	// you want to wire a generated node with different socket names to
	// an manually created socket
	void remapSocketName(const gkString& currentName, const gkString& newName)
	{
		gkILogicSocket* socket = _getSocketByName(currentName);
		if (socket)
		{
			m_socketNameMap.insert(newName.c_str(),socket);
			m_socketNameMap.remove(currentName.c_str());
		}
	}

	template<typename T>
	T getSocketValue(int idx)
	{
		GK_ASSERT(idx < N_MAX_SOCKETS);

		return static_cast<gkLogicSocket<T>*>(m_sockets[idx])->getValue();
	}

	template<typename T>
	T& getSocketRefValue(int idx)
	{
		GK_ASSERT(idx < N_MAX_SOCKETS);

		return static_cast<gkLogicSocket<T>*>(m_sockets[idx])->getRefValue();
	}


	template<typename T>
	void setSocketValue(int idx, T data)
	{
		GK_ASSERT(idx < N_MAX_SOCKETS);

		static_cast<gkLogicSocket<T>*>(m_sockets[idx])->setValue(data);
	}


protected:
	gkString 		m_name;
	gkString 		m_type;
	UTsize    m_handle;
	gkGameObject*   m_object, *m_other;
	gkLogicTree*    m_parent;
	bool            m_hasLinks;
	Sockets         m_inputs;
	Sockets         m_outputs;
	int             m_priority;
	int				m_bucketNr;

	bool			m_isValid;


	SocketByNameMap m_socketNameMap;
	PropertyByNameMap m_propertyNameMap;

	gkILogicSocket* m_sockets[N_MAX_SOCKETS];

	bool m_printDebug;
	bool debugCheckVariable(gkVariable* var);
public:
	GK_INLINE void setPrintDebug(bool debug) { m_printDebug = debug;}
	GK_INLINE bool getPrintDebug() { return m_printDebug; }

};

#define DECLARE_PROPERTY(NAME,TYPE,TYPE_UPPER) \
protected:\
	gkVariable* m_##NAME;\
public:\
	GK_INLINE TYPE getProp##NAME(){ return m_##NAME->getValue##TYPE_UPPER();} \
	GK_INLINE void setProp##NAME(TYPE t)   {m_##NAME->setValue((TYPE)t);}


#define INIT_PROPERTY(NAME,TYPE,DEFAULT) \
	m_##NAME = new gkVariable(DEFAULT); \
	setProperty(#NAME,m_##NAME);


#endif//_gkLogicNode_h_
