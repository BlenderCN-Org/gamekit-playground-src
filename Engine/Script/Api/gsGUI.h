/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2012 Thomas Trocha(dertom)

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
#ifndef _gsGUI_h_
#define _gsGUI_h_
#include <Rocket/Core/Element.h>
#include "gsCore.h"
#include "rocket/gkGUIDocumentRocket.h"
#include "gkGUI.h"
#include <Rocket/Controls/ElementFormControl.h>
#include <Rocket/Core/Box.h>
#include "gkString.h"
#include <Rocket/Core/String.h>
#include <Rocket/Core/Vector2.h>
#include "gkGUIManager.h"
#include "GUI/gkGUI.h"
#include "Rocket/Core/String.h"
#include "../Lua/gkLuaUtils.h"
#include <string.h>
#include "../../../Dependencies/Source/libRocket/Include/Rocket/Core/Property.h"

#include "Script/Lua/gkLuaManager.h"
#ifdef OGREKIT_COMPILE_LIBROCKET

typedef Rocket::Core::Element gkGuiElement;

class gsGuiElementQuery;

class gsGuiDocumentRocket
{
public:
	OGRE_KIT_WRAP_CLASS_COPY_CTOR(gsGuiDocumentRocket, gkInstancedObject, m_object);
	OGRE_KIT_INTERNAL_CAST(m_object);

	void createInstance() { m_object->createInstance();}
	void destroyInstance() {m_object->destroyInstance();}
	bool isInstanced() { return m_object->isInstanced();}
	gkString getName() { return m_object->getName();}

	gkGuiElement* getElementById(const gkString& id) {
		return static_cast<gkGuiDocumentRocket*>(m_object)->_getDocument()->GetElementById(id.c_str());
	}

	int getChildAmount(bool includingNonDom=false)
	{
		return static_cast<gkGuiDocumentRocket*>(m_object)->_getDocument()->GetNumChildren(includingNonDom);
	}

	gkGuiElement* getChild(int nr=0)
	{
		return static_cast<gkGuiDocumentRocket*>(m_object)->_getDocument()->GetChild(nr);
	}

	gsGuiElementQuery* getElementsByClass(const gkString& clazzName);
	gsGuiElementQuery* getElementsByTagname(const gkString& tagName);

	gkString getRML()
	{
		Rocket::Core::String content;
		static_cast<gkGuiDocumentRocket*>(m_object)->_getDocument()->GetRML(content);
		return gkString(content.CString());
	}

	gkString getInnerRML()
	{
		return static_cast<gkGuiDocumentRocket*>(m_object)->_getDocument()->GetInnerRML().CString();
	}

private:

};

class gsGuiElementQuery
{
	friend class gsGuiDocumentRocket;
	friend class gsGuiElement;

public:
	int size();
	gkGuiElement* at(int nr);
private:
	utList<gkGuiElement*> mQueryElements;
};

class gsGuiElementEventListener
#ifndef SWIG
		: public Rocket::Core::EventListener
#endif
{
public:
	gsGuiElementEventListener(const gkString& eventName,gsSelf self,gsFunction func)
		: m_evt(self,func){ m_evt.setDeleteAfterExecution(false);}
	gsGuiElementEventListener(const gkString& eventName,gsFunction func)
		: m_evt(func){m_evt.setDeleteAfterExecution(false);}

	virtual void ProcessEvent(Rocket::Core::Event& event) {
//		m_evt.beginCall();
		gkGUI::CURRENT_GUIEVENT_ELEMENT = event.GetCurrentElement();
		gkGUI::CURRENT_GUIEVENT_TARGET = event.GetTargetElement();

		gkGUI::CURRENT_DRAG_ELEMENT = static_cast< Rocket::Core::Element* >(event.GetParameter< void* >("drag_element", NULL));
		gkGUI::CURRENT_GUIEVENT = &event;
		m_evt.addArgument((char*)event.GetType().CString());

		gkLuaManager::getSingleton().addPreparedLuaEvent(&m_evt);
//		m_evt.call();
		gkGUI::CURRENT_GUIEVENT_TARGET = 0;
		gkGUI::CURRENT_GUIEVENT_ELEMENT = 0;
		gkGUI::CURRENT_GUIEVENT = 0;
	}

private:
	gkLuaCurState* self;
	gkLuaCurState* func;
	gkPreparedLuaEvent m_evt;
};

class gsGuiElement
{
public:
	gsGuiElement(gkGuiElement* element) : m_element(element), m_evtListeners(0){}
	~gsGuiElement() {
		EventListenerTable::Iterator iter(m_evtListeners);
//		while (iter.hasMoreElements()) {
//			gsGuiElementEventListener* listener = *iter.getNext().second;
//			delete listener;
//		}
	}


	void addEventListener(const gkString& evtName,gsSelf self, gsFunction func) {
		if (m_evtListeners.find(utCharHashKey(evtName.c_str()))==UT_NPOS) {
			gsGuiElementEventListener* newEventListener = new gsGuiElementEventListener(evtName,self,func);
			m_element->AddEventListener(evtName.c_str(),newEventListener);
			m_evtListeners.insert(utCharHashKey(evtName.c_str()),newEventListener);
		} else {
			gkDebugScreen::printTo("WARNING! Element has already an eventlistener on "+evtName+"!! IGNORING NEW ONE!!");
		}
	}

	void addEventListener(const gkString& evtName,gsFunction func) {
		if (m_evtListeners.find(utCharHashKey(evtName.c_str()))==UT_NPOS) {
			gsGuiElementEventListener* newEventListener = new gsGuiElementEventListener(evtName,func);
			m_element->AddEventListener(evtName.c_str(),newEventListener);
			m_evtListeners.insert(utCharHashKey(evtName.c_str()),newEventListener);
		} else {
			gkDebugScreen::printTo("WARNING! Element has already an eventlistener on "+evtName+"!! IGNORING NEW ONE!!");
		}
	}

	void removeEventListener(const gkString& evtName) {
		gsGuiElementEventListener* listener = *m_evtListeners.get(evtName.c_str());

		if (listener) {
			m_element->RemoveEventListener(evtName.c_str(),listener);
			delete listener;
			m_evtListeners.erase(utCharHashKey(evtName.c_str()));
		} else {
			gkDebugScreen::printTo("WARNING! Tried to remove eventListener "+evtName+"!! but there was not such eventlistener on this element!!");
		}
	}

	gkString getValue() {
		gkString tagName(m_element->GetTagName().CString());
		if (tagName == "input")
		{
			Rocket::Controls::ElementFormControl* control = static_cast<Rocket::Controls::ElementFormControl*>(m_element);
			return control->GetValue().CString();
		}
		else if (tagName == "select") {
			int idx = getSelection();
			return getOption(idx);
		}

		return "Not supported as output:"+tagName;
	}

	void setValue(const gkString& value) {
		gkString tagName(m_element->GetTagName().CString());
		if (tagName == "input")
		{
			Rocket::Controls::ElementFormControl* control = static_cast<Rocket::Controls::ElementFormControl*>(m_element);
			control->SetValue(value.c_str());
		}
		else {
			gkLogger::write("gsgui:setValue not supported for tag"+value);
		}
	}

	void setValueIdx(int value) {
		gkString tagName(m_element->GetTagName().CString());
		if (tagName == "select")
		{
			setSelection(value);
		}
		else {
			gkLogger::write("gsgui:setValue not supported for tag"+value);
		}
	}


	void setClass(const gkString& clazzName, bool value)
	{
		m_element->SetClass(clazzName.c_str(),value);
	}

	bool isClassSet(const gkString& clazzName) {
		return m_element->IsClassSet(clazzName.c_str());
	}


	void setPseudoClass(const gkString& clazzName, bool value)
	{
		m_element->SetPseudoClass(clazzName.c_str(),value);
	}

	bool isPseudoClassSet(const gkString& clazzName) {
		return m_element->IsPseudoClassSet(clazzName.c_str());
	}

	gkString getAttribute(const gkString& attrib)
	{
		return m_element->GetAttribute< Rocket::Core::String >(attrib.c_str(), "").CString();
	}

	void setAttribute(const gkString& attribName, const gkString& value)
	{
		m_element->SetAttribute<Rocket::Core::String>(Rocket::Core::String(attribName.c_str()),
														Rocket::Core::String(value.c_str()));
	}

	bool hasAttribute(const gkString& attribName)
	{
		return m_element->HasAttribute(Rocket::Core::String(attribName.c_str()));
	}

	void removeAttribute(const gkString& attribName)
	{
		m_element->RemoveAttribute(Rocket::Core::String(attribName.c_str()));
	}

	gkString getProperty(const gkString& propName)
	{
		const Rocket::Core::Property* prop = m_element->GetProperty(propName.c_str());
		if (!prop) {
			gkDebugScreen::printTo("rk-err: Unknown Property ["+propName+"] in "+getTagName());
			return "";
		}
		return gkString(prop->ToString().CString());
	}

	/* caution this is only the number value! there is NO conversion so you have to use px*/
	int getPropertyInt(const gkString& propName)
	{
		const Rocket::Core::Property* prop = m_element->GetProperty(propName.c_str());
		if (!prop) {
			gkDebugScreen::printTo("rk-err: Unknown Property ["+propName+"] in "+getTagName());
			return 0;
		}

		int result = prop->Get<int>();
		return result;
	}

	void setProperty(const gkString& propName, const gkString& value)
	{
		m_element->SetProperty(propName.c_str(),value.c_str());
	}


	float resolveProperty(const gkString& propName, const float baseValue=0.0f)
	{
		return m_element->ResolveProperty(propName.c_str(),baseValue);
	}

	void test() {

		float ctop = m_element->GetClientTop();
		float cleft = m_element->GetClientLeft();
		float cwidth = m_element->GetClientWidth();
		float cheight = m_element->GetClientHeight();
		Rocket::Core::Box::Area ct = m_element->GetClientArea();
		float oHeight = m_element->GetOffsetHeight();
		float oLeft = m_element->GetOffsetLeft();
		float otop = m_element->GetOffsetTop();
		float owidth = m_element->GetOffsetWidth();
		Rocket::Core::Box b = m_element->GetBox(Rocket::Core::Box::CONTENT);
		Rocket::Core::Vector2f bsize(b.GetSize(Rocket::Core::Box::CONTENT));
		int a = 0;
	}


	gkString getId()
	{
		return gkString(m_element->GetId().CString());
	}

	void setId(const gkString& id)
	{
		m_element->SetId(id.c_str());
	}

	gkString getRML()
	{
		Rocket::Core::String content;
		m_element->GetRML(content);
		return gkString(content.CString());
	}

	gkString getInnerRML()
	{
		return m_element->GetInnerRML().CString();
	}



	int getChildAmount(bool includingNonDom=false)
	{
		return m_element->GetNumChildren(includingNonDom);
	}

	gkGuiElement* getChild(int nr=0)
	{
		return m_element->GetChild(nr);
	}

	void appendChild(gsGuiElement* elem) {
		if (elem)
			m_element->AppendChild(elem->get());
	}

	void removeChild(gsGuiElement* elem) {
		if (elem)
			m_element->RemoveChild(elem->get());
	}

	void remove() {
		gkGuiElement* parent = m_element->GetParentNode();
		if (parent) {
			parent->RemoveChild(m_element);
		}
	}

	void insertBefore(gsGuiElement* before,gsGuiElement* elem) {
		if (before && elem) {
			m_element->InsertBefore(elem->get(),before->get());
		}
	}

	void replaceChild(gsGuiElement* before,gsGuiElement* elem) {
		if (before && elem) {
			m_element->ReplaceChild(before->get(),elem->get());
		}
	}


	gkGuiElement* getPreviousSibling() {
		return m_element->GetPreviousSibling();
	}

	gkGuiElement* getNextSibling() {
		return m_element->GetNextSibling();
	}


	float getWidth() {
		return m_element->GetClientWidth();
	}

	void setWidth(float w) {
		m_element->SetProperty("width", Rocket::Core::Property(w, Rocket::Core::Property::PX));
	}

	float getHeight() {
		return m_element->GetClientHeight();
	}

	void setHeight(float h) {
		m_element->SetProperty("height", Rocket::Core::Property(h, Rocket::Core::Property::PX));
	}

	float getAbsX() {
		return m_element->GetAbsoluteLeft();
	}
	float getAbsY() {
		return m_element->GetAbsoluteTop();
	}

	void setInnerRML(const char* innerRML)
	{
		if (innerRML)
			m_element-> SetInnerRML(Rocket::Core::String(innerRML));
		else
			gkLogger::write("innerRML: Tried to set null!");
	}

	gkString getTagName() {
		return m_element->GetTagName().CString();
	}

	int getSelection() {
		gkString tag(getTagName());
		if (tag == "select") {
			Rocket::Controls::ElementFormControlSelect* select = static_cast<Rocket::Controls::ElementFormControlSelect*>(m_element);
			return select->GetSelection();
		}
		gkDebugScreen::printTo("Rk-ERR: getSelection() unsupported for tag "+tag);
		return -1;
	}

	void setSelection(int selectionIdx) {
		gkString tag(getTagName());
		if (tag == "select") {
			Rocket::Controls::ElementFormControlSelect* select = static_cast<Rocket::Controls::ElementFormControlSelect*>(m_element);
			select->SetSelection(selectionIdx);
			return;
		}
		gkDebugScreen::printTo("Rk-ERR: setSelection() unsupported for tag "+tag);
	}

	int getNumOptions() {
		gkString tag(getTagName());
		if (tag == "select") {
			Rocket::Controls::ElementFormControlSelect* select = static_cast<Rocket::Controls::ElementFormControlSelect*>(m_element);
			return select->GetNumOptions();
		}
		gkDebugScreen::printTo("Rk-ERR: getNumOptions() unsupported for tag "+tag);
		return -1;
	}

	gkString getOption(int idx) {
		gkString tag(getTagName());
		if (tag == "select") {
			Rocket::Controls::ElementFormControlSelect* select = static_cast<Rocket::Controls::ElementFormControlSelect*>(m_element);
			Rocket::Controls::SelectOption* option = select->GetOption(idx);

			if (!option) {
				gkDebugScreen::printTo("Rk-ERR: no option with idx:"+gkToString(idx)+" for tag "+tag);
				return "";
			}

			return option->GetValue().CString();
		}
		gkDebugScreen::printTo("Rk-ERR: getOption() unsupported for tag "+tag);
		return "";
	}

	void setDatasource(const gkString& src) {
		gkString tag(getTagName());

		if (tag == "datagrid") {
			Rocket::Controls::ElementDataGrid* grid = static_cast<Rocket::Controls::ElementDataGrid*>(m_element);
			grid->SetDataSource(src.c_str());
		} else {
			gkDebugScreen::printTo("Rk-ERR: setDatasource(..) unsupported for tag <"+tag+">");
		}
	}
	gkGuiElement* get() { return m_element; }

	gkGuiElement* getParent() { return m_element->GetParentNode();}

	gkGuiElement* getElementById(const gkString& id) {
		return m_element->GetElementById(id.c_str(),false);
	}

	gkGuiElement* clone()
	{
		return m_element->Clone();
	}

	gsGuiElementQuery* getElementsByClass(const gkString& clazzName);
	gsGuiElementQuery* getElementsByTagname(const gkString& tagName);
private:
    gkGuiElement* m_element;

    typedef utHashTable<utCharHashKey,gsGuiElementEventListener*> EventListenerTable;
    EventListenerTable m_evtListeners;
};

class gsGUI
{
public:
	gsGUI();
	~gsGUI();

	gkGuiDocumentRocket* getDocument(const gkString& documentName);
	void loadFont(const gkString& fontName);
	void loadCursor(const gkString& cursorName);
	void _touchMouse() { m_guiManager->getGUI()->_touchMouse(); }
	void showCursor(bool show) {
		m_guiManager->showCursor(show);
	}

	bool isCursorVisible() {
		return m_guiManager->isCursorVisible();
	}

	void setLuaTranslation(gsSelf self,gsFunction func) { m_guiManager->setLuaTranslation(new gkLuaEvent(self,func)); }
	void setLuaTranslation(gsFunction func) { m_guiManager->setLuaTranslation(new gkLuaEvent(func)); }
	// not really working. name of the cursor is specified by <title>-tag in the cursor document
	// but the new cursor doesn't show up. Using css 'cursor'-property works though
	void setCursor(const gkString& name) {
		m_guiManager->setCursor(name);
	}

	void showDebugger(bool showit);

	gkString getPathPrefix() { return m_guiManager->getPathPrefix(); }
	void setPathPrefix(const gkString& pathPrefix) { m_guiManager->setPathPrefix(pathPrefix); }

	gkGuiElement* createElement(const gkString& tagName) {
		Rocket::Core::Element* newElem = NULL;

		Rocket::Core::XMLAttributes attribs;
		newElem = Rocket::Core::Factory::InstanceElement(NULL, tagName.c_str(), tagName.c_str(), attribs);
		return newElem;
	}


private:
	gkGUIManager* m_guiManager;

};

// inside of lua-event-functions
extern gkGuiElement* getCurrentGUIElement();
extern gkGuiElement* getDraggedElement();
extern gkGuiElement* getCurrentTargetElement();
// inside rcss-inline-functions:
extern gkString getCurrentRCSSEvent();
extern gkGuiElement* getCurrentRCSSElement();
extern gkString getCurrentRCSSProperty();


extern gkString getStringEventParameter(const gkString& name);
extern int getIntEventParameter(const gkString& name);

class gsGuiDataSource
#ifndef SWIG
		: gkGuiDataSource
#endif
{
public:
	gsGuiDataSource(const gkString& dsName,gsSelf self,gsFunction funcNum,gsFunction funcRow)
			: gkGuiDataSource(dsName), mLuaGetNumRows(self,funcNum), mLuaGetRowColumn(self,funcRow)
	{
	}

	gsGuiDataSource(const gkString& dsName,gsSelf selfNum,gsFunction funcNum,gsSelf selfRow,gsFunction funcRow)
			: gkGuiDataSource(dsName), mLuaGetNumRows(selfNum,funcNum), mLuaGetRowColumn(selfRow,funcRow)
	{
	}

	gsGuiDataSource(const gkString& dsName,gsFunction funcNum,gsFunction funcRow)
			: gkGuiDataSource(dsName), mLuaGetNumRows(funcNum), mLuaGetRowColumn(funcRow)
	{
	}


	~gsGuiDataSource() {}

	void update(const gkString& table);

private:
	void GetRow(Rocket::Core::StringList& row, const Rocket::Core::String& table, int row_index, const Rocket::Core::StringList& columns);
	int GetNumRows(const Rocket::Core::String& table);
private:
	gkLuaEvent mLuaGetNumRows;
	gkLuaEvent mLuaGetRowColumn;
};

class gsGuiDataFormatter
#ifndef SWIG
	: gkGuiDataFormatter
#endif
{
public:
	gsGuiDataFormatter(const gkString& dsName,gsSelf self,gsFunction formatterFunc)
			: gkGuiDataFormatter(dsName), mLuaFormatter(self,formatterFunc)
	{
		gkLogger::write("created formater!");
	}

	~gsGuiDataFormatter()
	{
		gkLogger::write("destroyed formater!");
	}


	void FormatData(Rocket::Core::String& formatted_data, const Rocket::Core::StringList& raw_data);

private:
	gkLuaEvent mLuaFormatter;
};
#endif

#endif//_gsGUI_h_
