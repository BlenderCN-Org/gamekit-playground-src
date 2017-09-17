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

#ifndef __gsnodes__
#define __gsnodes__
#include "gsMath.h"

extern gkLogicTree* getLogicTree(const gkString& logicTreeName);
extern int	getLogicTreeAmount();
extern gkLogicTree* getLogicTreeAt(int at);
extern gkLogicTree* getCurrentLogicTree();
extern gkLogicNode* getCurrentLogicNode();
extern void enableLogicTreeDebug(bool enable);
extern gkString getLogicTreeDebugOutput();
extern gkString getLogicTreeDebugOutputCompressed();
extern bool isLogicTreeDebugging();

class gkGameObject;
class gsGameObject;


class gsLogicTree
{
public:
	gsLogicTree(gkLogicTree* tree) : m_tree(tree) {};
	~gsLogicTree() {};

	gkLogicNode* getNodeByName(const gkString& name) { return m_tree->getNodeByName(name); }
	gkLogicNode* getNodeAt(int i);
	int getNodeAmount()                              { return m_tree->getNodeAmount(); }
	gkString getName() 								 { return m_tree->getName();}
	gkString getType()								 { return m_tree->getGroupName();}
private:
	gkLogicTree* m_tree;
};

class gsILogicSocket
{
public:
	gsILogicSocket(gkILogicSocket* socket) : m_socket(socket) {}
	~gsILogicSocket(){};

	bool isConnected() { return m_socket->isConnected();}
	bool isLinked() { return m_socket->isLinked();}

	gkILogicSocket* getFrom() { return m_socket->getFrom(); }
	void removeConnection(gsILogicSocket* sock) { return m_socket->removeConnection(sock->m_socket);}

	int getOutSocketAmount() { return m_socket->getOutSocketAmount();}
	gkILogicSocket* getOutSocketAt(int at) { return m_socket->getOutSocketAt(at); }
	gkString getName() { return m_socket->getName();}
	gkLogicNode* getNode() { return m_socket->getParent();}
	gkGameObject* getGameObject() { return m_socket->getGameObject();}
	gkVariable* getVar() { return m_socket->getVar();}
	void setVar(bool b) { gkVariable var(b);m_socket->setVar(&var);}
	void setVar(float f) { gkVariable var(f); m_socket->setVar(&var);}
	void setVar(const gkString st) { gkVariable var(st); m_socket->setVar(&var);}
	void link(gsILogicSocket* otherSocket) { if (otherSocket) m_socket->link(otherSocket->m_socket); }
	bool isDirty() { return m_socket->isDirty();}
	void requestClearDirty() { m_socket->requestClearDirty();}
//	void updateVar() { m_socket->updateVar(); }
private:
	gkILogicSocket* m_socket;
};


class gsVariable
{
public:
	gsVariable(gkVariable* var) : m_var(var) {};

	float getFloat() { return m_var->getValueReal();}
	int getInt() { return m_var->getValueInt();}
	gkString getString() { return m_var->getValueString();}
	bool getBool() { return m_var->getValueBool();}
	gsVector2 getVector2() { return gsVector2(m_var->getValueVector2());}
	gsVector3 getVector3() { return gsVector3(m_var->getValueVector3());}
	gsVector4 getVector4() { return gsVector4(m_var->getValueVector4());}
	void setFloat(float f) { m_var->setValue(f);}
	void setInt(int i) { m_var->setValue(i);}
	void setBool(bool b) { m_var->setValue(b);}
	void set(const gkVector3& vec) { m_var->setValue(vec);}
	void setString(const gkString& st) { m_var->setValue(st);}
private:
	gkVariable* m_var;
};

class gsLogicNode
{
public:
	gsLogicNode(gkLogicNode* node) : m_node(node) {}
	~gsLogicNode(){}

	int getInSocketAmount() { return m_node->getInputs().size();}
	int getOutSocketAmount() { return m_node->getOutputs().size();}
	gkILogicSocket* getInSocketAt(int idx);
	gkILogicSocket* getOutSocketAt(int idx);
	gkString getName() { return m_node->getName();}
	gkString getType() { return m_node->getType();}
	gkILogicSocket* getInSocket(const gkString& name){ return m_node->getInSocketByName(name);}
	gkILogicSocket* getOutSocket(const gkString& name){ return m_node->getOutSocketByName(name);}
	gkVariable* getProperty(const gkString& name) { return m_node->getProperty(name);}
	gkGameObject* getGameObject() { return m_node->getAttachedObject();}
	bool isValid() { return m_node->isValid();}


	void attachObject(gsGameObject* gobj);

	void setLuaInit(gsSelf self,gsFunction func);
	void setLuaInit(gsFunction func);
	void setLuaUpdate(gsSelf self,gsFunction func);
	void setLuaUpdate(gsFunction func);
	void setLuaEval(gsSelf self,gsFunction func);
	void setLuaEval(gsFunction func);

	void callCPPInit() { m_node->_initialize(); }
	void callCPPUpdate(float tick=0) { m_node->_update(tick);}
	bool callCPPEval(float tick=0) { return m_node->_evaluate(tick);}

private:
	gkLogicNode* m_node;
};

//class gsTemplateNode
//{
//public:
//	gsTestNodeAbstract(gkTestNodeAbstract* )
//};

#endif
