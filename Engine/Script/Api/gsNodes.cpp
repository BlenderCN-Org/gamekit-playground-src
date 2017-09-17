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

#include "gsNodes.h"

#include "Script/Lua/gkLuaUtils.h"

#include "gsCore.h"
gkLogicTree* getLogicTree(const gkString& logicTreeName)
{
	gkLogicTree* result = static_cast<gkLogicTree*>(gkNodeManager::getSingleton().getByName(logicTreeName));
	return result;
}

gkLogicTree* getCurrentLogicTree()
{
	return gkNodeManager::getSingleton().__getCurrentExecutionTree();
}
gkLogicNode* getCurrentLogicNode()
{
	return gkNodeManager::getSingleton().__getCurrentExecutionNode();
}


int	getLogicTreeAmount()
{
	return gkNodeManager::getSingleton().getResources().size();
}

gkLogicTree* getLogicTreeAt(int at)
{
	if (at < gkNodeManager::getSingleton().getResources().size())
	{
		return static_cast<gkLogicTree*>(gkNodeManager::getSingleton().getResources().at(at));
	}
	return 0;
}


void enableLogicTreeDebug(bool enable)
{
	gkNodeManager::getSingleton().enableDebugging(enable);
}
gkString getLogicTreeDebugOutput()
{
	return gkNodeManager::getSingleton().getCurrentDebugString();
}

gkString getLogicTreeDebugOutputCompressed()
{
	return gkNodeManager::getSingleton().getCurrentDebugStringCompressed();
}

bool isLogicTreeDebugging()
{
	return gkNodeManager::getSingleton().isDebugging();
}

gkLogicNode* gsLogicTree::getNodeAt(int i)
{
	if (i<m_tree->getNodeAmount())
		return m_tree->getNode(i);
	else
		return 0;
}

gkILogicSocket* gsLogicNode::getInSocketAt(int idx)
{
	if (idx<m_node->getInputs().size())
		return m_node->getInputs().at(idx);
	return 0;
}
gkILogicSocket* gsLogicNode::getOutSocketAt(int idx)
{
	if (idx<m_node->getOutputs().size())
		return m_node->getOutputs().at(idx);
	return 0;
}

void gsLogicNode::setLuaInit(gsSelf self,gsFunction func)
{
	m_node->setLuaInit(new gkLuaEvent(self,func));
}

void gsLogicNode::setLuaInit(gsFunction func)
{
	m_node->setLuaInit(new gkLuaEvent(func));
}

void gsLogicNode::setLuaEval(gsSelf self,gsFunction func)
{
	m_node->setLuaEval(new gkLuaEvent(self,func));
}

void gsLogicNode::setLuaEval(gsFunction func)
{
	m_node->setLuaEval(new gkLuaEvent(func));
}
void gsLogicNode::setLuaUpdate(gsSelf self,gsFunction func)
{
	m_node->setLuaUpdate(new gkLuaEvent(self,func));
}

void gsLogicNode::setLuaUpdate(gsFunction func)
{
	m_node->setLuaUpdate(new gkLuaEvent(func));
}

void gsLogicNode::attachObject(gsGameObject* gobj)
{
	 m_node->attachObject(gobj->get());
}
