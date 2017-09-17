/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef MATHNODE_H_
#define MATHNODE_H_

#include "BNodes/out-src/src-gen/Generated/gkGamekit.h"
#include "Logic/gkMathNode.h"

#include "gkValue.h"

#include "gkLogger.h"
// gkMathNodeImpl

class gkMathNodeImpl
		: public gkMathNodeAbstract
{
public:
	gkMathNodeImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	~gkMathNodeImpl();

	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff
};

class MathNodeInstancer : public gkMathNodeInstancer
{
protected:
	gkLogicNode* addNode(gkLogicNode* node,gkLogicTree* tree)
	{
		gkMathNodeImpl* _tempNode = static_cast<gkMathNodeImpl*>(node);

		gkLogicNode* mathNode = 0;

		switch (_tempNode->getPropOperation())
		{
			case gkMathNodeImpl::Operation_add:
												mathNode = tree->createNode<gkMathNode<gkScalar,MTH_ADD> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_mul: mathNode =  tree->createNode<gkMathNode<float,MTH_MULTIPLY> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_inv: mathNode =  tree->createNode<gkMathNode<float,MTH_INVERSE> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_sin: mathNode =  tree->createNode<gkMathNode<float,MTH_SINE> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_cos: mathNode =  tree->createNode<gkMathNode<float,MTH_COSINE> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_tan: mathNode =  tree->createNode<gkMathNode<float,MTH_TANGENT> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_sub: mathNode =  tree->createNode<gkMathNode<float,MTH_SUBTRACT> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_div: mathNode =  tree->createNode<gkMathNode<float,MTH_DIVIDE> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_gt: mathNode =  tree->createNode<gkMathNode<float,MTH_GREATER_THAN> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_lt: mathNode =  tree->createNode<gkMathNode<float,MTH_LESS_THAN> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_log: mathNode =  tree->createNode<gkMathNode<float,MTH_LOGARITHM> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_asin: mathNode =  tree->createNode<gkMathNode<float,MTH_ARCSINE> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_acos: mathNode =  tree->createNode<gkMathNode<float,MTH_ARCCOSINE> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_atan: mathNode =  tree->createNode<gkMathNode<float,MTH_ARCTANGENT> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_max: mathNode =  tree->createNode<gkMathNode<float,MTH_MAXIMUM> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_min: mathNode =  tree->createNode<gkMathNode<float,MTH_MINIMUM> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_power: mathNode =  tree->createNode<gkMathNode<float,MTH_POWER> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_round: mathNode =  tree->createNode<gkMathNode<float,MTH_ROUND> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_sqr: mathNode =  tree->createNode<gkMathNode<float,MTH_SQR> >(node->getName());
												break;
			case gkMathNodeImpl::Operation_no_func: mathNode =  tree->createNode<gkMathNode<float,MTH_NO_FUNC> >(node->getName());
												break;

			default:
				gkLogger::write("Warning! Unknown enum:"+gkToString((int)_tempNode->getPropOperation()),true);
				break;
		}

		// just for the purpose of setting the default-values make a dummy-cast
		gkMathNode<float,MTH_ADD>* m = static_cast<gkMathNode<float,MTH_ADD>*>(mathNode);
		float a = _tempNode->getIN_A()->getValue();
		m->getA()->setValue(a);
		float b = _tempNode->getIN_B()->getValue();
		m->getB()->setValue(b);

		return mathNode;
	}
};

template<class T> void set(T to,gkMathNodeImpl* node)
{

}

#endif
