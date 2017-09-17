/*
 * gkGuiElementRocket.cpp
 *
 *  Created on: Jan 12, 2013
 *      Author: ttrocha
 */

#include "rocket/gkGUIDocumentRocket.h"
#include "gkGUIManager.h"
#include "gkPath.h"
#include "gkString.h"


gkGuiDocumentRocket::gkGuiDocumentRocket(const gkString& docText,gkInstancedManager* creator, const gkResourceName& name, const gkResourceHandle& handle)
	: gkInstancedObject(creator,name,handle), m_document(0)
{
	if (!docText.empty()) {
		m_document = gkGUIManager::getSingleton().getGUI()->getContext()->LoadDocumentFromMemory(docText.c_str());
	} else {
		gkLogger::write("Empty Text for "+name.getName()+" is not allowed",true);
	}
}

gkGuiDocumentRocket::gkGuiDocumentRocket(gkInstancedManager* creator, const gkResourceName& name, const gkResourceHandle& handle)
	: gkInstancedObject(creator,name,handle), m_document(0)
{
	gkPath path(name.getName());
	if (path.exists()) {
		gkString rml = path.getAsString();
		m_document = gkGUIManager::getSingleton().getGUI()->getContext()->LoadDocumentFromMemory(rml.c_str());
	}

	if (m_document)
		m_document->Hide();
}

gkGuiDocumentRocket::~gkGuiDocumentRocket() {
	// TODO Auto-generated destructor stub
}

void gkGuiDocumentRocket::createInstanceImpl()
{
	if (m_document)
		m_document->Show();
}

void gkGuiDocumentRocket::destroyInstanceImpl()
{
	if (m_document)
	{
		m_document->Hide();
	}
}

void gkGuiDocumentRocket::notifyResourceDestroying()
{
	if (m_document && m_document->GetReferenceCount()>0)
	{
		m_document->RemoveReference();
		m_document = 0;
	}
}
