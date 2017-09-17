/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef NODETREENODE_H_
#define NODETREENODE_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>
#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkNodeTreeNodeImpl.h>

class gkLogicTree;

// gkNodeTreeNodeImpl

class gkNodeTreeNodeImpl
		: public gkNodeTreeNodeAbstract
{
public:
	gkNodeTreeNodeImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkNodeTreeNodeImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
	virtual void _afterInit(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff

	void _setLogicTree(gkLogicTree* tree) { m_tree = tree; }
protected:
	gkLogicTree* m_tree;
	gkGameObject* m_targetObject;
	bool m_initialized;
};


class gkNodeTreeNodeCustom : public gkNodeTreeNodeImpl
{
public:
	gkNodeTreeNodeCustom(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkNodeTreeNodeCustom(){};

	GK_INLINE gkBlenderSceneConverter::CPValue& getInnertreeInputs() { return m_innertreeInputs;}
	GK_INLINE gkBlenderSceneConverter::CPValue& getInnertreeOutput() { return m_innertreeOutputs;}

	virtual void _initialize(); // called on node-creation

private:
	gkBlenderSceneConverter::CPValue m_innertreeInputs;
	gkBlenderSceneConverter::CPValue m_innertreeOutputs;


};

class gkNodeTreeNodeCustomInstancer : public gkNodeTreeNodeInstancer
{
public:
	gkNodeTreeNodeCustomInstancer() {}

	gkLogicNode* createNode(const gkString& name);
	gkLogicNode* create(gkLogicTree* tree,Blender::bNode* bnode,gkString objectName);
};

#endif
