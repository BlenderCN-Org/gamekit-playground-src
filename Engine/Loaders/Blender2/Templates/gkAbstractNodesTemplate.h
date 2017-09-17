/*
 * gkNodeInstancerTemplate.h
 *
 *  Created on: Nov 13, 2015
 *      Author: ttrocha
 */

#ifndef /*name|u,post#_h_:treename*/treename_h_/*endname*/
#define /*name|u,post#_h_:treename*/treename_h_/*endname*/

#include <Blender.h>
#include "Loaders/Blender2/gkNodeTreeConverter.h"
#include "Logic/gkLogicNode.h"
#include "Loaders/Blender2/gkBlenderSceneConverter.h"
#include "Logic/gkMotionNode.h"
#include "Logic/gkLogicTree.h"
#include "gkGameObject.h"


	typedef utArray<int> IntArray;
	typedef utArray<int> BoolArray;
	typedef utArray<float> FloatArray;


class gkGeneratedLogicNode : public gkLogicNode
{
public:
		gkGeneratedLogicNode(gkLogicTree* parent, size_t id, const gkString& name="") : gkLogicNode(parent, id,name) {}
		virtual ~gkGeneratedLogicNode() {}
};

/*block:abstractNode*/
// /*name|fu,pre#gk,post#Abstract,setvar#abstractName:nodeName*/gkTemplateNodeAbstract/*endname*/
// implemented by /*name|fu,pre#gk,post#Impl,setvar#gkNodeNameImpl:nodeName*/test/*endname*/ = /*name:gkNodeNameImpl*//*endname*/
class /*name:abstractName*/gkTemplateNodeAbstract/*endname*/ : public gkGeneratedLogicNode
{
public:

	enum
	{
/*block:socketsEnum*/
		/*name:socketName*/iInt/*endname*/,
/*endblock:socketsEnum*/
/*block:rip*/
		iFloat,
		iBool,
		iVec,
		iCol,
		iStr,
		iObj,
		oInt,
		oFloat,
		oBool,
		oVec,
		oCol,
		oStr,
		oObj
/*endblock:rip*/
	};

   static gkString NODE_NAME;

/*block:socketDeclare*/
    static gkString /*name|pre#SOCKET_:socketName*/SOCKET_OUT_Int/*endname*/;
	DECLARE_SOCKET_TYPE(/*name:socketName*/iInt/*endname*/,/*name:socketType:socketType*/int/*endname*/);
/*endblock:socketDeclare*/
/*block:rip*/
	DECLARE_SOCKET_TYPE(iFloat,/*map:socketType:float*/float/*endmap*/);
	DECLARE_SOCKET_TYPE(iObj,/*map:socketType:object*/bool/*endmap*/);
	DECLARE_SOCKET_TYPE(iBool,/*map:socketType:bool*/bool/*endmap*/);
	DECLARE_SOCKET_TYPE(iVec,/*map:socketType:vector*/gkVector3/*endmap*/);
	DECLARE_SOCKET_TYPE(iCol,/*map:socketType:color*/gkColor/*endmap*/);
	DECLARE_SOCKET_TYPE(iStr,/*map:socketType:string*/gkString/*endmap*/);
	DECLARE_SOCKET_TYPE(oInt,/*map:socketType:int*/int/*endmap*/);
	DECLARE_SOCKET_TYPE(oFloat,float);
	DECLARE_SOCKET_TYPE(oBool,bool);
	DECLARE_SOCKET_TYPE(oVec,gkVector3);
	DECLARE_SOCKET_TYPE(oCol,gkColor);
	DECLARE_SOCKET_TYPE(oStr,gkString);
/*endblock:rip*/
	/*name:abstractName*/gkTemplateNodeAbstract/*endname*/(gkLogicTree* parent, size_t id, const gkString& name="") : gkGeneratedLogicNode(parent, id,name)
	{
		m_type = /*name|dq:nodeName*/"gkTemplateNodeAbstract"/*endname*/;
		/*block:propInitString*/
			INIT_PROPERTY(/*name:propName*/stringValue/*endname*/,gkString,/*name|dq:defaultValue*/""/*endname*/)
		/*endblock:propInitString*/
		/*block:propInitInt*/
			INIT_PROPERTY(/*name:propName*/numInt/*endname*/,int,/*name:defaultValue*/1/*endname*/)
		/*endblock:propInitInt*/
		/*block:propInitFloat*/
			INIT_PROPERTY(/*name:propName*/numFloat/*endname*/,float,/*name:defaultValue*/0.f/*endname*/);
		/*endblock:propInitFloat*/
		/*block:propInitEnum*/
			INIT_PROPERTY(/*name:propName*/enumIntEnum/*endname*/,int,(int)/*name:defaultValue*/0/*endname*/);
		/*endblock:propInitEnum*/
		/*block:propInitBool*/
			INIT_PROPERTY(/*name:propName*/numBool/*endname*/,bool,/*name:defaultValue*/false/*endname*/);
		/*endblock:propInitBool*/
		/*block:propInitVec2*/
			INIT_PROPERTY(/*name:propName*/vec2/*endname*/,gkVector2,gkVector2/*name:defaultValue*/()/*endname*/);
		/*endblock:propInitVec2*/
		/*block:propInitVec3*/
			INIT_PROPERTY(/*name:propName*/vec3/*endname*/,gkVector3,gkVector3/*name:defaultValue*/()/*endname*/);
		/*endblock:propInitVec3*/
		/*block:propInitVec4*/
			INIT_PROPERTY(/*name:propName*/vec4/*endname*/,gkVector4,gkVector4/*name:defaultValue*/()/*endname*/);
		/*endblock:propInitVec4*/
		/*block:propInitColor*/
			INIT_PROPERTY(/*name:propName*/tColor/*endname*/,gkColor,gkColor/*name:defaultValue*/()/*endname*/);
		/*endblock:propInitColor*/

/*block:addInputSocket*/
		ADD_ISOCK_VAR(/*name:socketName*/iInt/*endname*/,/*name:socketValue*/0/*endname*/);
		// 0 outconnection == unlimited
		// no need for input-socket:
		// /*name|pre#get:socketName*/getiInt/*endname*/()->_setMaxOutConnections(/*name:outConnections*/0/*endname*/);
		/*name|pre#get:socketName*/getiInt/*endname*/()->_setNeedsConnection(/*name:needsConnection*/false/*endname*/);
/*block:addValidType*/
		/*name|pre#get:socketName*/getiInt/*endname*/()->_addValidConnectionNode(/*name|dq:validTypeName*/"NodeTreeNode"/*endname*/);
/*endblock:addValidType*/
/*endblock:addInputSocket*/
/*block:addOutputSocket*/
		ADD_OSOCK_VAR(/*name:socketName*/oFloat/*endname*/,/*name:socketValue*/0/*endname*/);
		/*name|pre#get:socketName*/getoFloat/*endname*/()->_setNeedsConnection(/*name:needsConnection*/false/*endname*/);
		/*name|pre#get:socketName*/getoFloat/*endname*/()->_setMaxOutConnections(/*name:outConnections*/0/*endname*/);
/*block:addValidType*/
		/*name|pre#get:socketName*/getoFloat/*endname*/()->_addValidConnectionNode(/*name|dq:validTypeName*/"NodeTreeNode"/*endname*/);
/*endblock:addValidType*/
/*endblock:addOutputSocket*/
/*block:rip*/
		ADD_ISOCK(iFloat,/*map:socketDefault:float*/0.f/*endmap*/);
		ADD_ISOCK(iBool,/*map:socketDefault:bool*/false/*endmap*/);
		ADD_ISOCK(iVec,/*map:socketDefault:vector*/gkVector3()/*endmap*/);
		ADD_ISOCK(iCol,/*map:socketDefault:color*/gkColor()/*endmap*/);
		ADD_ISOCK(iStr,/*map:socketDefault:string*/""/*endmap*/);
		ADD_OSOCK(oInt,/*map:socketDefault:int*/0/*endmap*/);
		ADD_OSOCK(oBool,false);
		ADD_OSOCK(oVec,gkVector3());
		ADD_OSOCK(oCol,gkColor());
		ADD_OSOCK(oStr,"");
/*endblock:rip*/

	};

	virtual ~/*name:abstractName*/gkTemplateNodeAbstract/*endname*/() {}

	static gkString getStaticType() { return /*name|dq:nodeName*/"gkTemplateNodeAbstract"/*endname*/;}
/*block:propString*/
	DECLARE_PROPERTY(/*name:propName*/stringValue/*endname*/,gkString,String);
/*endblock:propString*/

/*block:propEnum*/
public:
	enum /*name|fu:propName*/enumIntEnum/*endname*/{
		/*block:enumItem*//*name:itemName*/key/*endname*/, // /*name:description*/The Key Enum/*endname*//*endblock:enumItem*/
		/*block:rip*/key2,key3,/*endblock:rip*/
	};
	DECLARE_PROPERTY(/*name:propName*/enumIntEnum/*endname*/,int,Int);
/*endblock:propEnum*/
/*block:propFloat*/
	DECLARE_PROPERTY(/*name:propName*/numFloat/*endname*/,float,Real);
/*endblock:propFloat*/
/*block:propInt*/
	DECLARE_PROPERTY(/*name:propName*/numInt/*endname*/,int,Int)
/*endblock:propInt*/
/*block:propBool*/
	DECLARE_PROPERTY(/*name:propName*/numBool/*endname*/,bool,Bool)
/*endblock:propBool*/
public:
/*block:vec2*/
	DECLARE_PROPERTY(/*name:propName*/vec2/*endname*/,gkVector2,Vector2)
/*endblock:vec2*/
/*block:vec3*/
	DECLARE_PROPERTY(/*name:propName*/vec3/*endname*/,gkVector3,Vector3)
/*endblock:vec3*/
/*block:vec4*/
	DECLARE_PROPERTY(/*name:propName*/vec4/*endname*/,gkVector4,Vector4)
/*endblock:vec4*/
/*block:array*/
	GK_INLINE FloatArray /*name|fu,pre#getProp:propName*/getNumFloatVec/*endname*/() { return /*name|fl:propName*/floatArray/*endname*/; }
	GK_INLINE void /*name|fu,pre#setProp:propName*/setNumFloatVec/*endname*/(const FloatArray& vec) { /*name|fl:propName*/floatArray/*endname*/=vec; }
protected:
	FloatArray floatArray;
/*endblock:array*/
/*block:propIntVec*/
public:
	GK_INLINE IntArray /*name|fu,pre#getProp:propName*/getNumIntVec/*endname*/() { return /*name|fl:propName*/numIntVec/*endname*/; }
	GK_INLINE void /*name|fu,pre#setProp:propName*/setNumIntVec/*endname*/(const IntArray& array) { /*name|fl:propName*/numIntVec/*endname*/ = array; }
protected:
	IntArray /*name|fl:propName*/numIntVec/*endname*/;
/*endblock:propIntVec*/
/*block:propBoolVec*/
public:
	GK_INLINE BoolArray /*name|fu,pre#getProp:propName*/getNumBoolVec/*endname*/() { return /*name|fl:propName*/numBoolVec/*endname*/; }
	GK_INLINE void /*name|fu,pre#setProp:propName*/setNumBoolVec/*endname*/(const BoolArray& boolArray) { /*name|fl:propName*/numBoolVec/*endname*/ = boolArray;}
protected:
	BoolArray /*name|fl:propName*/numBoolVec/*endname*/;
/*endblock:propBoolVec*/
/*block:propColor*/
	DECLARE_PROPERTY(/*name:propName*/tColor/*endname*/,gkColor,Color)
/*endblock:propColor*/

	gkString getUUIDPropname()
	{
		return /*name|dq:uuidProp*/""/*endname*/;
	}
};
/*endblock:abstractNode*/

/*block:instancer*/
// Instancer: /*name|fu,pre#gk,post#Instancer,setvar#instancerName:nodeName*/InstancerName/*endname*/
class /*name:instancerName*/TemplateNodeInstancer/*endname*/ : public gkNodeInstancer
{
public:
	/*name:instancerName*/TemplateNodeInstancer/*endname*/(){};
	~/*name:instancerName*/TemplateNodeInstancer/*endname*/(){};

	virtual gkLogicNode* createNode(const gkString& name);

	virtual gkLogicNode* create(gkLogicTree* tree,Blender::bNode* bnode,gkString objectName);



	gkString getName()
	{
		return /*name|dq:nodeName*/"TemplateNode"/*endname*/;
	}

	gkString getTreeID() {
		return /*name|dq:treename*/"Gamekit"/*endname*/;
	}
};
/*endblock:instancer*/



void /*name|fu,post#_addNodeInstancer:treename*/NodeTree_addNodeInstancer/*endname*/();
#endif
