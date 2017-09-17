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
#include "gsGUI.h"
#if defined OGREKIT_COMPILE_LIBROCKET

gsGUI::gsGUI() : m_guiManager(gkGUIManager::getSingletonPtr())
{
}

gsGUI::~gsGUI()
{
}

void gsGUI::loadFont(const gkString& fontname)
{
	m_guiManager->loadFont(fontname);
}

void gsGUI::showDebugger(bool showit)
{
	m_guiManager->showDebugger(showit);
}

gsGuiDocumentRocket::gsGuiDocumentRocket(gkInstancedObject* obj) : m_object(obj)
{}



gsGuiElementQuery* gsGuiDocumentRocket::getElementsByClass(const gkString& clazzName)
{
	Rocket::Core::ElementList elemList;
	static_cast<gkGuiDocumentRocket*>(m_object)->_getDocument()->GetElementsByClassName(elemList,Rocket::Core::String(clazzName.c_str()));
	gsGuiElementQuery* result = new gsGuiElementQuery;
	for (int i=0;i<elemList.size();i++)
	{
		result->mQueryElements.push_back(elemList.at(i));
	}
	return result;
}

gsGuiElementQuery* gsGuiDocumentRocket::getElementsByTagname(const gkString& tagName)
{
	Rocket::Core::ElementList elemList;
	static_cast<gkGuiDocumentRocket*>(m_object)->_getDocument()->GetElementsByTagName(elemList,Rocket::Core::String(tagName.c_str()));
	gsGuiElementQuery* result = new gsGuiElementQuery;
	for (int i=0;i<elemList.size();i++)
	{
		result->mQueryElements.push_back(elemList.at(i));
	}
	return result;
}

gkGuiDocumentRocket* gsGUI::getDocument(const gkString& docName)
{
	gkGuiDocumentRocket* doc = static_cast<gkGuiDocumentRocket*>(m_guiManager->getDocument(docName));
	return static_cast<gkGuiDocumentRocket*>(doc);
}


gkGuiElement* gsGuiElementQuery::at(int nr)
{
	if (nr<mQueryElements.size())
		return mQueryElements.at(nr);
	else
	{
		gkDebugScreen::printTo("Index out of bounds in GUI-Query:"+gkToString(nr));
		return 0;
	}
}


int gsGuiElementQuery::size(void)
{
	return mQueryElements.size();
}


void gsGUI::loadCursor(const gkString& cursorName) {
	m_guiManager->loadCursor(cursorName);
}


gkGuiElement* getDraggedElement() {
	return  gkGUI::CURRENT_DRAG_ELEMENT;
}

gkGuiElement* getCurrentGUIElement() {
	return gkGUI::CURRENT_GUIEVENT_ELEMENT;
}

gkGuiElement* getCurrentTargetElement() {
	return gkGUI::CURRENT_GUIEVENT_TARGET;
}



gkString getCurrentRCSSEvent() {
	return gkGUI::CURRENT_RCSSEVENT_NAME;
}
gkGuiElement* getCurrentRCSSElement() {
	return gkGUI::CURRENT_RCSSEVENT_ELEMENT;
}
gkString getCurrentRCSSProperty() {
	return gkGUI::CURRENT_RCSSEVENT_PROPERTY;
}

void gsGuiDataSource::GetRow(Rocket::Core::StringList& row, const Rocket::Core::String& table, int row_index, const Rocket::Core::StringList& columns)
{
	for (size_t i = 0; i < columns.size(); i++)
	{

		mLuaGetRowColumn.beginCall();
		mLuaGetRowColumn.addArgument((char*)table.CString());
		mLuaGetRowColumn.addArgument(row_index);
		mLuaGetRowColumn.addArgument((char*)columns[i].CString());
		const char* result = mLuaGetRowColumn.callString();

		if (result)
			row.push_back(result);
//		else
//			row.push_back("No value");
	}
}

int gsGuiDataSource::GetNumRows(const Rocket::Core::String& table)
{
	mLuaGetNumRows.beginCall();
	mLuaGetNumRows.addArgument((char*)table.CString());

	int result = mLuaGetNumRows.callInt();

 	return result;
}

void gsGuiDataSource::update(const gkString& table)
{
	gkGuiDataSource::NotifyRowChange(table.c_str());
//	NotifyRowChange(table.c_str());
}

void gsGuiDataFormatter::FormatData(Rocket::Core::String& formatted_data, const Rocket::Core::StringList& raw_data)
{
	mLuaFormatter.beginCall();
	for (int i=0;i<raw_data.size();i++)
	{
		mLuaFormatter.addArgument((char*)raw_data[i].CString());
	}
	const char* result = mLuaFormatter.callString();

	formatted_data = result;
}

gkString getStringEventParameter(const gkString& name)
{
	return gkGUI::getStringEventParameter(name);
}
int getIntEventParameter(const gkString& name)
{
	return gkGUI::getIntEventParameter(name);
}


gsGuiElementQuery* gsGuiElement::getElementsByClass(const gkString& clazzName)
{
	Rocket::Core::ElementList elemList;
	m_element->GetElementsByClassName(elemList,Rocket::Core::String(clazzName.c_str()));
	gsGuiElementQuery* result = new gsGuiElementQuery;
	for (int i=0;i<elemList.size();i++)
	{
		result->mQueryElements.push_back(elemList.at(i));
	}
	return result;
}

gsGuiElementQuery* gsGuiElement::getElementsByTagname(const gkString& tagName)
{
	Rocket::Core::ElementList elemList;
	m_element->GetElementsByTagName(elemList,Rocket::Core::String(tagName.c_str()));

	gsGuiElementQuery* result = new gsGuiElementQuery;
	for (int i=0;i<elemList.size();i++)
	{
		result->mQueryElements.push_back(elemList.at(i));
	}
	return result;
}
#endif
