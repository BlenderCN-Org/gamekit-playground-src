/*
 * gkNodeInstancerTemplate.h
 *
 *  Created on: Nov 13, 2015
 *      Author: ttrocha
 */

#ifndef /*name|u,post#_script_h_:treename*/treename_script_h_/*endname*/

#define /*name|u,post#_script_h_:treename*/treename_script_h_/*endname*/

#include /*name|fu,dq,pre#gk,post#.h:treename*/"gkAbstractNodesTemplate.h"/*endname*/

#include "gsNodes.h"

/*block:scriptNode*/
// /*name|fu,pre#gk,post#Abstract,setvar#abstractName:nodeName*/gkTemplateNodeAbstract/*endname*/
// /*name|fu,pre#gs,post#Abstract,setvar#scriptName:nodeName*/gsTemplateNodeAbstract/*endname*/

class /*name:scriptName*/gsTemplateNode/*endname*/ : public gsLogicNode
{
public:
	/*name:scriptName*/gsTemplateNode/*endname*/(/*name:abstractName*/gkTemplateNodeAbstract/*endname*/* parent) : gsLogicNode(parent),m_parent(parent) {};
	~/*name:scriptName*/gsTemplateNode/*endname*/() {};

// properties:
	/*block:propString*/
	public:
		// Description:/*name:description*/The object name/*endname*/
		GK_INLINE gkString /*name|pre#getProp:propName*/getStringValue/*endname*/() { return m_parent->/*name|pre#getProp:propName*/getPropstringValue/*endname*/(); }
		GK_INLINE void /*name|pre#setProp:propName*/setStringValue/*endname*/(const gkString& stValue) { m_parent->/*name|pre#setProp:propName*/setPropstringValue/*endname*/(stValue); }
	/*endblock:propString*/
	/*block:propEnum*/
	public:
		// Description:/*name:description*/The object name/*endname*/
		GK_INLINE int /*name|pre#getProp:propName*/getEnumInt/*endname*/() { return m_parent->/*name|pre#getProp:propName*/getPropenumIntEnum/*endname*/(); }
		GK_INLINE void /*name|pre#setProp:propName*/setEnumInt/*endname*/(int value) {
			m_parent->/*name|pre#setProp:propName*/setPropenumIntEnum/*endname*/(
				(/*name|fu,pre#gk,post#Abstract,setvar#abstractName:nodeName*/gkTemplateNodeAbstract/*endname*/::/*name|fu:propName*/enumIntEnum/*endname*/)value
			);
		}
	/*endblock:propEnum*/

	/*block:propFloat*/
		// Description:/*name:description*/The object name/*endname*/
		GK_INLINE float /*name|pre#getProp:propName*/getNumFloat/*endname*/() { return m_parent->/*name|pre#getProp:propName*/getPropnumFloat/*endname*/(); }
		GK_INLINE void /*name|pre#setProp:propName*/setNumFloat/*endname*/(float value) { m_parent->/*name|pre#setProp:propName*/setPropnumFloat/*endname*/(value); }
	/*endblock:propFloat*/
	/*block:propInt*/
	public:
		// Description:/*name:description*/The object name/*endname*/
		GK_INLINE int /*name|pre#getProp:propName*/getNumInt/*endname*/() { return m_parent->/*name|pre#getProp:propName*/getPropnumInt/*endname*/(); }
		GK_INLINE void /*name|pre#setProp:propName*/setNumInt/*endname*/(int value) { m_parent->/*name|pre#setProp:propName*/setPropnumInt/*endname*/(value); }
	/*endblock:propInt*/
	/*block:propBool*/
	public:
		// Description:/*name:description*/The object name/*endname*/
		GK_INLINE bool /*name|pre#getProp:propName*/getNumBool/*endname*/() { return m_parent->/*name|pre#getProp:propName*/getPropnumBool/*endname*/(); }
		GK_INLINE void /*name|pre#setProp:propName*/setNumBool/*endname*/(int value) { m_parent->/*name|pre#setProp:propName*/setPropnumBool/*endname*/(value); }
	/*endblock:propBool*/
	/*block:propFloatVec*/
	public:

		GK_INLINE /*name:arrayType*/FloatArray/*endname*/ /*name|pre#getProp:propName*/getNumFloatVec/*endname*/() { return m_parent->/*name|pre#getProp:propName*/getNumFloatVec/*endname*/(); }
		GK_INLINE void /*name|pre#setProp:propName*/setNumFloatVec/*endname*/(const /*name:arrayType*/FloatArray/*endname*/& vec) { m_parent->/*name|pre#setProp:propName*/setNumFloatVec/*endname*/(vec); }
	/*endblock:propFloatVec*/
	/*block:propIntVec*/
	public:
		GK_INLINE IntArray /*name|pre#getProp:propName*/getNumIntVec/*endname*/() { return m_parent->/*name|pre#getProp:propName*/getNumIntVec/*endname*/(); }
		GK_INLINE void /*name|pre#setProp:propName*/setNumIntVec/*endname*/(const IntArray& vec) { m_parent->/*name|pre#setProp:propName*/setNumIntVec/*endname*/(vec); }
	/*endblock:propIntVec*/
	/*block:propBoolVec*/
	public:
//		GK_INLINE BoolArray /*name|pre#getProp:propName*/getNumBoolVec/*endname*/() { return m_parent->/*name|pre#getProp:propName*/getNumBoolVec/*endname*/(); }
//		GK_INLINE void /*name|pre#setProp:propName*/setNumBoolVec/*endname*/(const BoolArray& vec) { m_parent->/*name|pre#setProp:propName*/setNumBoolVec/*endname*/(vec); }
	/*endblock:propBoolVec*/
	/*block:propColor*/
	public:
		GK_INLINE gkColor /*name|pre#getProp:propName*/getTColor/*endname*/() { return m_parent->/*name|pre#getProp:propName*/getProptColor/*endname*/(); }
		GK_INLINE void /*name|pre#setProp:propName*/setTColor/*endname*/(const gkColor& color) { m_parent->/*name|pre#setProp:propName*/setProptColor/*endname*/(color); }
	/*endblock:propColor*/

private:
	/*name:abstractName*/gkTemplateNodeAbstract/*endname*/* m_parent;

};
/*endblock:scriptNode*/
#endif
