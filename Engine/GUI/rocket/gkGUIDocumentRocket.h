/*
 * gkGuiElementRocket.h
 *
 *  Created on: Jan 12, 2013
 *      Author: ttrocha
 */

#ifndef GKGUIELEMENTROCKET_H_
#define GKGUIELEMENTROCKET_H_
#include "gkLogger.h"
#include "gkInstancedObject.h"
#include "Rocket/Core/ElementDocument.h"

class gkGuiDocumentRocket : public gkInstancedObject {
public:
	gkGuiDocumentRocket(gkInstancedManager* creator, const gkResourceName& name, const gkResourceHandle& handle);
	gkGuiDocumentRocket(const gkString& documentText,gkInstancedManager* creator, const gkResourceName& name, const gkResourceHandle& handle);

	virtual ~gkGuiDocumentRocket();

	virtual void createInstanceImpl(void);
	virtual void destroyInstanceImpl(void);
	virtual void notifyResourceDestroying(void);

	Rocket::Core::ElementDocument* _getDocument() { return m_document;}

private:
	Rocket::Core::ElementDocument* m_document;

};

#endif /* GKGUIELEMENTROCKET_H_ */
