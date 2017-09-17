///*
// * gkNodeInstancerTemplate.h
// *
// *  Created on: Nov 13, 2015
// *      Author: ttrocha
// */
//
//#ifndef ENGINE_LOADERS_BLENDER2_GKNODEINSTANCERTEMPLATE_H_
//#define ENGINE_LOADERS_BLENDER2_GKNODEINSTANCERTEMPLATE_H_
//
//#include <Blender.h>
//#include <gkNodeTreeConverter.h>
//#include "gkLogicNode.h"
//#include "gkBlenderSceneConverter.h"
//#include "gkMotionNode.h"
//#include "gkLogicTree.h"
//
//
//
///*block:abstractNode*/
//class /*name|fu,post#Abstract:nodeName*/gkTemplateNodeAbstract/*endname*/ : public gkLogicNode
//{
//public:
//
//	enum
//	{
///*block:socketsEnum*/
//		iInt,
///*endblock:socketsEnum*/
///*block:rip*/
//		iFloat,
//		iBool,
//		iVec,
//		iCol,
//		iStr,
//		oInt,
//		oFloat,
//		oBool,
//		oVec,
//		oCol,
//		oStr
///*endblock:rip*/
//	};
//
///*block:socketDeclare*/
//	DECLARE_SOCKET_TYPE(/*name:socketName*/iInt/*endname*/,/*name:socketType:socketType*/int/*endname*/);
///*endblock:socketDeclare*/
///*block:rip*/
//	DECLARE_SOCKET_TYPE(iFloat,/*map:socketType:float*/float/*endmap*/);
//	DECLARE_SOCKET_TYPE(iBool,/*map:socketType:bool*/bool/*endmap*/);
//	DECLARE_SOCKET_TYPE(iVec,/*map:socketType:vector*/gkVector3/*endmap*/);
//	DECLARE_SOCKET_TYPE(iCol,/*map:socketType:color*/gkColor/*endmap*/);
//	DECLARE_SOCKET_TYPE(iStr,/*map:socketType:string*/gkString/*endmap*/);
//	DECLARE_SOCKET_TYPE(oInt,/*map:socketType:int*/int/*endmap*/);
//	DECLARE_SOCKET_TYPE(oFloat,float);
//	DECLARE_SOCKET_TYPE(oBool,bool);
//	DECLARE_SOCKET_TYPE(oVec,gkVector3);
//	DECLARE_SOCKET_TYPE(oCol,gkColor);
//	DECLARE_SOCKET_TYPE(oStr,gkString);
///*endblock:rip*/
//	/*name|fu,post#Abstract:nodeName*/gkTemplateNodeAbstract/*endname*/(gkLogicTree* parent, size_t id) : gkLogicNode(parent, id),
///*block:propInit*//*name:propInit*//*endname*//*endblock:propInit*/
///*block:rip*/enumInt((enumIntEnum)0),numFloat(0),numInt(0),numBool(false)/*endblock:rip*/
//	{
///*block:addInputSocket*/
//		ADD_ISOCK(/*name:socketName*/iInt/*endname*/,/*name:socketValue*/0/*endname*/);
///*endblock:addInputSocket*/
///*block:rip*/
//		ADD_ISOCK(iFloat,/*map:socketDefault:float*/0.f/*endmap*/);
//		ADD_ISOCK(iBool,/*map:socketDefault:bool*/false/*endmap*/);
//		ADD_ISOCK(iVec,/*map:socketDefault:vector*/gkVector3()/*endmap*/);
//		ADD_ISOCK(iCol,/*map:socketDefault:color*/gkColor()/*endmap*/);
//		ADD_ISOCK(iStr,/*map:socketDefault:string*/""/*endmap*/);
//		ADD_OSOCK(oInt,/*map:socketDefault:int*/0/*endmap*/);
//		ADD_OSOCK(oFloat,0);
//		ADD_OSOCK(oBool,false);
//		ADD_OSOCK(oVec,gkVector3());
//		ADD_OSOCK(oCol,gkColor());
//		ADD_OSOCK(oStr,"");
///*endblock:rip*/
//	};
//
//	virtual ~/*name|fu,post#Abstract:nodeName*/gkTemplateNodeAbstract/*endname*/() {}
//
//	void initialize()=0;
//	void update(gkScalar tick)=0;
//	bool evaluate(gkScalar tick)=0;
//
//	typedef utArray<int> IntArray;
//	typedef utArray<bool> BoolArray;
//
///*block:propEnum*/
//public:
//	enum /*name|fu:propName*/enumIntEnum/*endname*/{
//		/*block:enumItem*//*name:itemName*/key/*endname*/,/*endblock:enumItem*/
//		/*block:rip*/key2,key3,/*endblock:rip*/
//	};
//	GK_INLINE /*name|fu:propName*/enumIntEnum/*endname*/ /*name|fu,pre#get:propName*/getPropEnumInt/*endname*/() { return enumInt;}
//	GK_INLINE void /*name|fu,pre#set:propName*/setPropEnumInt/*endname*/(/*name|fu:propName*/enumIntEnum/*endname*/ enumId) { enumInt = enumId; }
//private:
//	enumIntEnum /*name|fl:propName*/enumInt/*endname*/;
///*endblock:propEnum*/
///*block:propFloat*/
//public:
//	GK_INLINE float /*name|fu,pre#get:propName*/getNumFloat/*endname*/() { return numFloat; }
//	GK_INLINE void /*name|fu,pre#set:propName*/setNumFloat/*endname*/(float num) { numFloat = num; }
///*endblock:propFloat*/
//	GK_INLINE int getNumInt() { return numInt; }
//	GK_INLINE void setNumInt(int num) { numInt = num; }
//	GK_INLINE bool getNumBool() { return numBool; }
//	GK_INLINE void setNumBool(bool b) { numBool = b; }
//	GK_INLINE gkVector2 getNumFloatVec() { return numFloatVec; }
//	GK_INLINE void setNumFloatVec(const gkVector2& vec) { numFloatVec=vec; }
//
//	GK_INLINE IntArray getNumIntVec() { return numIntVec; }
//	GK_INLINE void setNumIntVec(const IntArray& array) { numIntVec = array; }
//	GK_INLINE BoolArray getNumBoolVec() { return numBoolVec; }
//	GK_INLINE void setNumBoolVec(const BoolArray& boolArray) { numBoolVec = boolArray;}
//	GK_INLINE gkColor getTColor() { return tColor; }
//	GK_INLINE void setTColor(const gkColor& color) { tColor = color; }
//private:
//	float numFloat;
//	int numInt;
//	bool numBool;
//	gkVector2 numFloatVec;
//	IntArray numIntVec;
//	BoolArray numBoolVec;
//	gkColor tColor;
//};
///*endblock:abstractNode*/
//
//
//
//
//
///*block:instancer*/
//class /*name:instancerName*/TemplateNodeInstancer/*endname*/ : public gkNodeInstancer
//{
//public:
//	/*name:instancerName*/TemplateNodeInstancer/*endname*/(){};
//	~/*name:instancerName*/TemplateNodeInstancer/*endname*/(){};
//	gkLogicNode* create(gkLogicTree* tree,Blender::bNode* bnode)
//	{
//		/*name:nodeClass*/gkTemplateNodeImpl/*endname*/* newNode = tree->createNode</*name:nodeClass*/gkTemplateNodeImpl/*endname*/>();
//
//		// get properties
//		int enumIntID = gkBlenderSceneConverter::retrieveCustomPropertyInt("enumInt",bnode->prop,0);
//		newNode->setEnumInt((gkTemplateNodeImpl::enumIntEnum)enumIntID);
//
//		float numFloat = gkBlenderSceneConverter::retrieveCustomPropertyFloat("numFloat",bnode->prop,10.95f);
//		newNode->setNumFloat(numFloat);
//
//		int numInt = gkBlenderSceneConverter::retrieveCustomPropertyInt("numInt",bnode->prop,95);
//		newNode->setNumInt(numInt);
//
//		int numBool = gkBlenderSceneConverter::retrieveCustomPropertyInt("numBool",bnode->prop,0);
//		newNode->setNumBool((bool)numBool);
//
//		gkVector2 numFloatVec;
//		gkBlenderSceneConverter::retrieveCustomPropertyFloatVec("numFloatVec",bnode->prop,numFloatVec);
//
//		utArray<int> numIntVec;
//		gkBlenderSceneConverter::retrieveCustomPropertyIntVec("numIntVec",bnode->prop,numIntVec);
//
//		utArray<int> numBoolVec;
//		gkBlenderSceneConverter::retrieveCustomPropertyIntVec("numBoolVec",bnode->prop,numBoolVec);
//
//		gkVector3 tColor;
//		gkBlenderSceneConverter::retrieveCustomPropertyFloatVec("tColor",bnode->prop,tColor);
//
//		// set values of ingoing-sockets that were not connected to another node
//		Blender::bNodeSocket* socket = (Blender::bNodeSocket*)bnode->inputs.first;
//		while (socket) {
//			if (!socket->link) // no link? set value
//			{
//				int a=0;
//
//				if (0) {}
//				else if (gkString(socket->identifier)=="iInt") {
//					int iInt = gkNodeTreeConverter::getBNodeSocketInt(socket);
//					newNode->getiInt()->setValue(iInt);
//				}
//				else if (gkString(socket->identifier)=="iFloat") {
//					float iFloat = gkNodeTreeConverter::getBNodeSocketFloat(socket);
//					newNode->getiFloat()->setValue(iFloat);
//				}
//				else if (gkString(socket->identifier)=="iBool") {
//					bool boolValue = (bool)gkNodeTreeConverter::getBNodeSocketInt(socket);
//					newNode->getiBool()->setValue(boolValue);
//				}
//				else if (gkString(socket->identifier)=="iVec") {
//					gkVector3 vector = gkNodeTreeConverter::getBNodeSocketVector(socket);
//					newNode->getiVec()->setValue(vector);
//				}
//				else if (gkString(socket->identifier)=="iCol") {
//					gkColor color = gkNodeTreeConverter::getBNodeSocketColor(socket);
//					newNode->getiCol()->setValue(color);
//				}
//				else if (gkString(socket->identifier)=="iStr") {
//					gkString stringValue = gkNodeTreeConverter::getBNodeSocketString(socket);
//					newNode->getiStr()->setValue(stringValue);
//				}
//			}
//			socket = socket->next;
//		}
//
//
//		return newNode;
//	}
//
//	gkString getName()
//	{
//		return "TemplateNode";
//	}
//
//	gkString getTreeID() {
//		return "Gamekit";
//	}
//};
///*endblock:instance*/
//
//static void NodeTree_addNodeInstancer(gkNodeTreeConverter* nodeTreeConverter);
//
//void NodeTree_addNodeInstancer(gkNodeTreeConverter* nodeTreeConverter)
//{
//	nodeTreeConverter->addNodeInstancer(new TemplateNodeInstancer());
//}
//#endif /* ENGINE_LOADERS_BLENDER2_GKNODEINSTANCERTEMPLATE_H_ */
