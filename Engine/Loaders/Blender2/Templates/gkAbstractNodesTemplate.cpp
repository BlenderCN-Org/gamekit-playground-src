/*
 * gkAbstractNodesTemplate.cpp
 *
 *  Created on: Nov 18, 2015
 *      Author: ttrocha
 */

#include /*name|dq,fu,pre#gk,post#.h:treename*/"gkAbstractNodesTemplate.h"/*endname*/

/*block:implInclude*/#include /*name|dq,fu,pre#gk,post#Impl.h:nodeName*/"gkTemplateNode.h"/*endname*/

#include "gkLogger.h"

#include "utTypes.h"
/*endblock:implInclude*/




/*block:instancerCreate*/
gkLogicNode* /*name:instancerName*/TemplateNodeInstancer/*endname*/::createNode(const gkString& name)
{
	/*name:gkNodeNameImpl*/gkTemplateNodeImpl/*endname*/* newNode = new /*name:gkNodeNameImpl*/gkTemplateNodeImpl/*endname*/(0,0,name);
	return newNode;
}

gkLogicNode* /*name:instancerName*/TemplateNodeInstancer/*endname*/::create(gkLogicTree* tree,Blender::bNode* bnode,gkString objName)
{
	gkString nodeName(bnode->name);

	/*name:gkNodeNameImpl*/gkTemplateNodeImpl/*endname*/* newNode = static_cast</*name:gkNodeNameImpl*/gkTemplateNodeImpl/*endname*/*>(createNode(nodeName));

	int PRIORITY = gkBlenderSceneConverter::retrieveCustomPropertyInt("PRIORITY",bnode->prop,0);
	if (PRIORITY!=0)
	{
		newNode->setBucketNr(PRIORITY);
	}

	// get properties
/*block:propEnum*/
	int /*name|fl:propName*/enumIntID/*endname*/ = gkBlenderSceneConverter::retrieveCustomPropertyInt(/*name|dq:propName*/"enumInt"/*endname*/,bnode->prop,/*name:defaultValue*/0/*endname*/);
	newNode->/*name|pre#setProp:propName*/setPropenumIntEnum/*endname*/((/*name:abstractName*/gkTemplateNodeImpl/*endname*/::/*name|fu:propName*/enumIntEnum/*endname*/)/*name|fl:propName*/enumIntID/*endname*/);
/*endblock:propEnum*/
/*block:propFloat*/
	float /*name|fl:propName*/numFloat/*endname*/ = gkBlenderSceneConverter::retrieveCustomPropertyFloat(/*name|dq:propName*/"numFloat"/*endname*/,bnode->prop,/*name:defaultValue*/0.f/*endname*/);
	newNode->/*name|pre#setProp:propName*/setPropnumFloat/*endname*/(/*name|fl:propName*/numFloat/*endname*/);
/*endblock:propFloat*/
/*block:propInt*/
	int /*name|fl:propName*/numInt/*endname*/ = gkBlenderSceneConverter::retrieveCustomPropertyInt(/*name|dq:propName*/"numInt"/*endname*/,bnode->prop,/*name:defaultValue*/0/*endname*/);
	newNode->/*name|pre#setProp:propName*/setPropnumInt/*endname*/((int)/*name|fl:propName*/numInt/*endname*/);
/*endblock:propInt*/
/*block:propBool*/
	int /*name|fl:propName*/numBool/*endname*/ = gkBlenderSceneConverter::retrieveCustomPropertyInt(/*name|dq:propName*/"numBool"/*endname*/,bnode->prop,/*name:defaultValue*/0/*endname*/);
	newNode->/*name|pre#setProp:propName*/setPropnumBool/*endname*/((bool)/*name|fl:propName*/numBool/*endname*/);
/*endblock:propBool*/
/*block:propFloatVec*/
	/*name:arrayType*/FloatArray/*endname*/ /*name|fl:propName*/numFloatVec/*endname*/;
	/*block:useDefaultArray*/
	FloatArray /*name|pre#default:propName*/defaultFloatXYZ/*endname*/(/*name:size*/3/*endname*/,0.f);
	/*endblock:useDefaultArray*/
	gkBlenderSceneConverter::retrieveCustomPropertyFloatVec(/*name|dq:propName*/"numFloatVec"/*endname*/,bnode->prop,/*name|fl:propName*/numFloatVec/*endname*//*block:useDefaultArray2*/,/*name|pre#default:propName*/defaultFloatXYZ/*endname*//*endblock:useDefaultArray2*/);
	newNode->/*name|pre#setProp:propName*/setNumFloatVec/*endname*/(/*name|fl:propName*/numFloatVec/*endname*/);
/*endblock:propFloatVec*/
/*block:propIntVec*/
	IntArray /*name|fl:propName*/numIntVec/*endname*/;
	IntArray /*name|pre#default:propName*/defaultIntXYZ/*endname*/(/*name:size*/3/*endname*/,0);
	gkBlenderSceneConverter::retrieveCustomPropertyIntVec(/*name|dq:propName*/"numIntVec"/*endname*/,bnode->prop,/*name|fl:propName*/numIntVec/*endname*/,/*name|pre#default:propName*/defaultIntXYZ/*endname*/);
	newNode->/*name|pre#setProp:propName*/setNumIntVec/*endname*/(/*name|fl:propName*/numIntVec/*endname*/);
/*endblock:propIntVec*/
/*block:propBoolVec*/
	IntArray /*name|fl:propName*/numBoolVec/*endname*/;
	IntArray /*name|pre#default:propName*/defaultBoolXYZ/*endname*/(/*name:size*/3/*endname*/,0);
	gkBlenderSceneConverter::retrieveCustomPropertyIntVec(/*name|dq:propName*/"numBoolVec"/*endname*/,bnode->prop,/*name|fl:propName*/numBoolVec/*endname*/,/*name|pre#default:propName*/defaultBoolXYZ/*endname*/);
	newNode->/*name|pre#setProp:propName*/setNumBoolVec/*endname*/(/*name|fl:propName*/numBoolVec/*endname*/);
/*endblock:propBoolVec*/
/*block:propColor*/
	gkVector3 /*name|fl:propName*/tColor/*endname*/;
	gkBlenderSceneConverter::retrieveCustomPropertyFloatVec(/*name|dq:propName*/"tColor"/*endname*/,bnode->prop,/*name|fl:propName*/tColor/*endname*/);
	newNode->/*name|pre#setProp:propName*/setProptColor/*endname*/(gkColor(tColor.x,tColor.y,tColor.z));
/*endblock:propColor*/
/*block:propString*/
	gkString /*name|fl:propName*/stringValue/*endname*/ = gkBlenderSceneConverter::retrieveCustomPropertyString(/*name|dq:propName*/"stringValue"/*endname*/,bnode->prop,/*name|dq:defaultValue*/""/*endname*/);
	newNode->/*name|pre#setProp:propName*/setPropstringValue/*endname*/(/*name|fl:propName*/stringValue/*endname*/);
/*endblock:propString*/

	// set values of ingoing-sockets that were not connected to another node
	Blender::bNodeSocket* socket = (Blender::bNodeSocket*)bnode->inputs.first;
	while (socket) {

		if (!socket->link) // no link? set value
		{
			int a=0;

			if (0) {}
/*block:intSocket*/
			else if (gkString(socket->identifier)==/*name|dq:origSocketName*/"iInt"/*endname*/) {
				int /*name:socketName*/iInt/*endname*/ = gkNodeTreeConverter::getBNodeSocketInt(socket);
				newNode->/*name|pre#get:socketName*/getiInt/*endname*/()->setValue(/*name:socketName*/iInt/*endname*/);
			}
/*endblock:intSocket*/
/*block:floatSocket*/
			else if (gkString(socket->identifier)==/*name|dq:origSocketName*/"iFloat"/*endname*/) {
				float /*name:socketName*/iFloat/*endname*/ = gkNodeTreeConverter::getBNodeSocketFloat(socket);
				newNode->/*name|pre#get:socketName*/getiFloat/*endname*/()->setValue(/*name:socketName*/iFloat/*endname*/);
			}
/*endblock:floatSocket*/
/*block:boolSocket*/
			else if (gkString(socket->identifier)==/*name|dq:origSocketName*/"iBool"/*endname*/) {
				bool /*name:socketName*/boolValue/*endname*/ = gkNodeTreeConverter::getBNodeSocketBoolean(socket);
				newNode->/*name|pre#get:socketName*/getiBool/*endname*/()->setValue(/*name:socketName*/boolValue/*endname*/);
			}
/*endblock:boolSocket*/
/*block:vecSocket*/
			else if (gkString(socket->identifier)==/*name|dq:origSocketName*/"iVec"/*endname*/) {
				gkVector3 /*name:socketName*/vector/*endname*/ = gkNodeTreeConverter::getBNodeSocketVector(socket);
				newNode->/*name|pre#get:socketName*/getiVec/*endname*/()->setValue(/*name:socketName*/vector/*endname*/);
			}
/*endblock:vecSocket*/
/*block:colSocket*/
			else if (gkString(socket->identifier)==/*name|dq:origSocketName*/"iCol"/*endname*/) {
				gkColor /*name:socketName*/color/*endname*/ = gkNodeTreeConverter::getBNodeSocketColor(socket);
				newNode->/*name|pre#get:socketName*/getiCol/*endname*/()->setValue(/*name:socketName*/color/*endname*/);
			}
/*endblock:colSocket*/
/*block:stringSocket*/
			else if (gkString(socket->identifier)==/*name|dq:origSocketName*/"iStr"/*endname*/) {
				gkString /*name:socketName*/stringValue/*endname*/ = gkNodeTreeConverter::getBNodeSocketString(socket);
				newNode->/*name|pre#get:socketName*/getiStr/*endname*/()->setValue(/*name:socketName*/stringValue/*endname*/);
			}
/*endblock:stringSocket*/
/*block:objectSocket*/
			else if (gkString(socket->identifier)==/*name|dq:origSocketName*/"iObj"/*endname*/) {
				bool /*name:socketName*/objValue/*endname*/ = gkNodeTreeConverter::getBNodeSocketObject(socket);
				newNode->/*name|pre#get:socketName*/getiObj/*endname*/()->setValue(/*name:socketName*/objValue/*endname*/);
			}
/*endblock:objectSocket*/
//			else
//			{
//				processDynamicNodeInput(socket,newNode);
//			}
		}
		socket = socket->next;
	}

	// you have the chance to override the creation of the generated implementation in the
	// addNode-method. if you return another node you created inside the addNode-Method, the
	// the newNode gets deleted. (TODO: do this somehow else! error-prune!)
	gkLogicNode* finalNode = addNode(newNode,tree);
	if (finalNode != newNode) {
		delete newNode;
	}
	return finalNode;
}
/*endblock:instancerCreate*/

void /*name|fu,post#_addNodeInstancer:treename*/NodeTree_addNodeInstancer/*endname*/()
{
/*block:addInstancer*/
	gkNodeTreeConverter::addNodeInstancer(new /*name:instancerName*/TemplateNodeInstancer/*endname*/());
/*endblock:endInstancer*/
}

/*block:nodeStatic*/
gkString /*name|fu,pre#gk,post#Abstract:nodeName*/gkTemplateNodeAbstract/*endname*/::NODE_NAME = /*name|dq:nodeName*/"gkTemplateNodeAbstract"/*endname*/;
/*endblock:nodeStatic*/
/*block:socketStaticImpl*/
gkString /*name|fu,pre#gk,post#Abstract:nodeName*/gkTemplateNodeAbstract/*endname*/::/*name|pre#SOCKET_:socketName*/SOCKET_OUT_Int/*endname*/ = /*name|dq:socketName*/"OUT_INT"/*endname*/;
/*endblock:socketStaticImpl*/
