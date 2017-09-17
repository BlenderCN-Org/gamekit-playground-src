/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef MOTIONNODE_H_
#define MOTIONNODE_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>
#include "Logic/gkMotionNode.h"
// gkMotionNodeImpl

class gkMotionNodeImpl
		: public gkMotionNodeAbstract
{
public:
	gkMotionNodeImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	~gkMotionNodeImpl();

	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff

private:
	int     getLRS();
	void    applyConstraints(int lrs);
	void    applyObject(gkVector3& vec);
	void    updateOthername();


	short           m_flag;

	gkGameObject*   m_current;
	gkString        m_otherName;

};

//class MotionNodeInstancer : public gkMotionNodeInstancer
//{
//protected:
//	gkLogicNode* addNode(gkLogicNode* lNode, gkLogicTree* tree)
//	{
//		gkMotionNodeImpl* _tempNode = static_cast<gkMotionNodeImpl*>(lNode);
//
//		gkMotionNode* node = tree->createNode<gkMotionNode>(lNode->getName());
//		node->setPropMotionType((gkMotionTypes)_tempNode->getPropMotion());
//		node->getX()->setValue(_tempNode->getIN_X()->getValue());
//		node->getY()->setValue(_tempNode->getIN_Y()->getValue());
//		node->getZ()->setValue(_tempNode->getIN_Z()->getValue());
//		gkString val = _tempNode->getIN_OBJECT()->getValue();
//		node->getOBJECT()->setValue(val);
//		node->getUPDATE()->setValue(_tempNode->getIN_UPDATE()->getValue());
//		node->setSpace((int)_tempNode->getPropSpace());
//
//		node->setPropMax(_tempNode->getPropMaxVec());
//		node->setPropMin(_tempNode->getPropMinVec());
//		// get rid of lNode since we just used it for reading the data from blender
//		// TODO: shared_ptr
//		return node;
//	}
//};

#endif
