/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef MOUSENODE_H_
#define MOUSENODE_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>
#include "Logic/gkMouseNode.h"
// gkMouseNodeImpl

class gkMouseNodeImpl
		: public gkMouseNodeAbstract
{
public:
	gkMouseNodeImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	~gkMouseNodeImpl();

	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff

private:
	float m_curAbsX;
	float m_curAbsY;
	float m_curRelX;
	float m_curRelY;
	bool m_curIsMotion;
	bool m_curIsWheelMotion;
	float m_curWheel;
	float m_curScaleX;
	float m_curScaleY;
};

//class MouseNodeInstancer : public gkMouseNodeInstancer
//{
//protected:
//	gkLogicNode* addNode(gkLogicNode* lnode, gkLogicTree* tree)
//	{
//		gkMouseNodeImpl* _tempNode = static_cast<gkMouseNodeImpl*>(lnode);
//		gkMouseNode* mouseNode = tree->createNode<gkMouseNode>(lnode->getName());
//		mouseNode->getSCALE_X()->setValue(_tempNode->getIN_SCALE_X()->getValue());
//		mouseNode->getSCALE_Y()->setValue(_tempNode->getIN_SCALE_Y()->getValue());
//		return mouseNode;
//	}
//};

#endif
