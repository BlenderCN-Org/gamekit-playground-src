/*
 * gkGUI3DScript.cpp
 *
 *  Created on: Sep 11, 2012
 *      Author: ttrocha
 */

#include "gsGUI3D.h"
#include <vector>
#ifndef SWIG_UnknownError
# include "Script/Api/Generated/swigluarun.h"
#endif
#include "GUI/GUI3D/gkGUI3D.h"
#include "../../gkGameObject.h"


gsGUI3D::gsGUI3D(const gkString& atlas,gsScene* scene)  {
	if (!gkGUI3D::getSingletonPtr()){
		m_gui = new gkGUI3D();

		if (scene)
		{
			gkScene* gorillaScene = scene->cast<gkScene>();
			if (!gorillaScene->isInstanced())
			{
				GS_DEBUG("Gorilla: Tried to init gorilla, but the scene '"+gorillaScene->getName()+"' isn't instanced yet!! Using the default-scene instead!");
				m_gui->initGorilla(atlas);
				return;
			}
			m_gui->initGorilla(atlas,gorillaScene);

		}
		else
			m_gui->initGorilla(atlas);

	}
}

void gsGUI3D::showGUI() {
	gkGUI3D::getSingleton().mScreen->show();
}

void gsGUI3D::hideGUI() {
	gkGUI3D::getSingleton().mScreen->hide();
}

void gsGUI3D::addConsoleCommand(const gkString& commandName,gsSelf self,gsFunction func)
{
	m_gui->getConsole(true)->addCommand(commandName,self,func);
}
void gsGUI3D::addConsoleCommand(const gkString& commandName,gsFunction func)
{
	m_gui->getConsole(true)->addCommand(commandName,func);
}

void gsGUI3D::showConsole(bool showIt)
{
	m_gui->showConsole(showIt);
}
//void gsGUI3D::showConsole() {
//	OgreConsole::getSingleton().setVisible(true);
//}
//
//void gsGUI3D::hideConsole() {
//	OgreConsole::getSingleton().setVisible(false);
//}

bool gsGUI3DScriptCallback::execute(Gui3D::PanelElement* element)
{
	m_event.beginCall();
	gkDebugScreen::printTo("callback!");
	gsGUI3DElement* mappedElem = gkGUI3D::getSingleton().getScriptMapping(element);
	if (mappedElem){
		if (element->getType() == Gui3D::EButton) {
			m_event.addArgument("gsGUI3DButton *",mappedElem);
		}
		else if (element->getType() == Gui3D::ECheckbox) {
			m_event.addArgument("gsGUI3DCheckbox *",mappedElem);
		}
		else if (element->getType() == Gui3D::ECombobox)
		{
			m_event.addArgument("gsGUI3DCombobox *",mappedElem);
		}
		else if (element->getType() == Gui3D::EListbox)
		{
			m_event.addArgument("gsGUI3DListbox *",mappedElem);
		}
		else if (element->getType() == Gui3D::EProgressBar)
		{
			m_event.addArgument("gsGUI3DProgessBar *",mappedElem);
		}
		else if (element->getType() == Gui3D::EScrollbar)
		{
			m_event.addArgument("gsGUI3DScrollbar *",mappedElem);
		}
		else if (element->getType() == Gui3D::ETextzone)
		{
			m_event.addArgument("gsGUI3DTextZone *",mappedElem);
		}
		m_event.addArgument(m_data);
	} else {
		gkDebugScreen::printTo("Unknown mapped element!");
	}

	m_event.call();
	return true;
}




tsPanel* gsGUI3D::createPanel(float xpos,float ypos,float width,float height, const gkString& name,const gkString& atlas,bool useBackground) {

	Gorilla::Screen* screen = gkGUI3D::getSingleton().createScreen(name,atlas);
	Gui3D::ScreenPanel* panel = new Gui3D::ScreenPanel(gkGUI3D::getSingleton().mGui3D,
											screen,
											Ogre::Vector2(xpos, ypos),
											Ogre::Vector2(width, height),
											atlas,
		 									name,useBackground);

	// TODO: add screen panel to gui!?
	gkGUI3D::getSingleton().addPanel2d(panel);

	return new tsPanel(panel,true);
}



tsPanel* gsGUI3D::createPanel3D(float width,float height,const gkString& name,const gkString& atlas,float distance, const bool background,const gkString& sceneName) {
	gkScene* scene = sceneName.empty() ?  gkEngine::getSingleton().getActiveScene()
										: static_cast<gkScene*>(gkSceneManager::getSingleton().getByName(sceneName));

	if (!scene) {
		gsDebugPrint(gkString("CreatePanel("+name+"): Unknown scene-name:"+sceneName).c_str());
		return 0;
	}

	Gui3D::Panel* panel = new Gui3D::Panel(gkGUI3D::getSingleton().mGui3D,
											scene->getManager(),
											Ogre::Vector2(width, height),
											distance==-1?100:distance,
											atlas,
											name,background);

	gkGUI3D::getSingleton().addPanel3d(panel);

	return new tsPanel(panel,false);
}

void gsTextureAtlas::setFontOffset(int fontsize,float offsetX,float offsetY){
	Gorilla::GlyphData* font = m_atlas->getGlyphData(fontsize);
	if (font) {
		for (int i=0;i<font->mGlyphs.size();i++){
			Gorilla::Glyph* glyph = font->mGlyphs.at(i);
			glyph->uvLeft = glyph->uvLeft + offsetX*m_atlas->getInvTextureCoordsX();
			glyph->uvTop  = glyph->uvTop  + offsetY*m_atlas->getInvTextureCoordsY();
		}
	}
}


void tsPanel::addCallback(gsGUI3DScriptCallback* callback)
{
	if (!m_callbackList.find(callback))
	{
		m_callbackList.push_back(callback);
	}
}

void tsPanel::removeCallback(gsGUI3DScriptCallback* callback)
{
	if (callback)
	{
		delete callback;
		m_callbackList.erase(callback);
	}
}

void tsPanel::clearCallbacks()
{
	tsScriptCallbackList::Iterator iter(m_callbackList);
	while (iter.hasMoreElements())
	{
		delete iter.getNext();
	}
	m_callbackList.clear();
}

void tsPanel::setPosition(float x,float y,float z)
{
	if (m_is2D)
	{
		static_cast<Gui3D::ScreenPanel*>(m_panelContainer)->setPosition(gkVector2(x,y));
	}
	else
	{
		static_cast<Gui3D::Panel*>(m_panelContainer)->mNode->setPosition(x,y,z);
	}
}

void tsPanel::setRotation(float x,float y,float z)
{
	if (m_is2D)
	{
		gkLogger::write("setRotation not supported for Screen2D!",true);
//		static_cast<Gui3D::ScreenPanel*>(m_panelContainer)->setPosition(gkVector2(x,y));
	}
	else
	{
		static_cast<Gui3D::Panel*>(m_panelContainer)->mNode->setOrientation(gkEuler(x,y,z).toQuaternion());
	}
}

void tsPanel::setScale(float x,float y, float z)
{
	if (m_is2D)
	{
		gkLogger::write("setScale not supported for Screen2D!",true);
//		static_cast<Gui3D::ScreenPanel*>(m_panelContainer)->setPosition(gkVector2(x,y));
	}
	else
	{
		static_cast<Gui3D::Panel*>(m_panelContainer)->mNode->setScale(x,y,z);
	}
}


void tsPanel::setBackgroundColor(const gsVector4& col)
{
	m_panelContainer->setBackgroundColor(Ogre::ColourValue(col.x,col.y,col.z,col.w));
}
void tsPanel::setBackgroundColor(float r,float g,float b,float a)
{
	m_panelContainer->setBackgroundColor(Ogre::ColourValue(r,g,b,a));
}

void tsPanel::setBackgroundBorder(float width,const gsVector4& col)
{
	m_panelContainer->setBorder(width,Ogre::ColourValue(col.x,col.y,col.z,col.w));
}


gsGUI3DRectangle* tsPanel::createRectangle(float x,float y,int width,int height,const gsVector4& col)
{
	Gui3D::Rectangle* rect = m_panelContainer->makeRectangle(x,y,width,height,Ogre::ColourValue(col.x,col.y,col.z,col.w));
	gsGUI3DRectangle* tsRect = new gsGUI3DRectangle(this,rect);
	return tsRect;
}

gsGUI3DRectangle* tsPanel::createRectangle(float x,float y,int width,int height,const gkString& spriteName)
{
	Gui3D::Rectangle* rect = m_panelContainer->makeRectangle(x,y,width,height,spriteName);
	gsGUI3DRectangle* tsRect = new gsGUI3DRectangle(this,rect);
	return tsRect;
}


gsGUI3DCaption* tsPanel::createCaption(float x,float y,int width, int height,const gkString& text,int fontSize, int align, int valign) {
	size_t oldSize = getSettings()->buttonTextSize;

	if (fontSize!=-1)
		getSettings()->buttonTextSize = fontSize;

	Gui3D::Caption* caption = m_panelContainer->makeCaption(x,y,width,height,text,(Gorilla::TextAlignment)align,(Gorilla::VerticalAlignment)valign);
	gsGUI3DCaption* tsCaption = new gsGUI3DCaption(this,caption);
	gkGUI3D::getSingleton().addScriptMapping(caption,tsCaption);
	getSettings()->buttonTextSize = oldSize;

	return tsCaption;
}

gsGUI3DButton* tsPanel::createButton(float x,float y,int width, int height, const gkString& text, int fontSize)
{
	size_t oldSize = getSettings()->buttonTextSize;

	if (fontSize!=-1)
		getSettings()->buttonTextSize = fontSize;

	Gui3D::Button* btn = m_panelContainer->makeButton(x,y,width,height,text);
	gsGUI3DButton* tsBtn = new gsGUI3DButton(this,btn);
	gkGUI3D::getSingleton().addScriptMapping(btn,tsBtn);

	getSettings()->buttonTextSize = oldSize;

	return tsBtn;
}



gsGUI3DCheckbox* tsPanel::createCheckbox(float x,float y,int width, int height)
{
	Gui3D::Checkbox* checkbox = m_panelContainer->makeCheckbox(x,y,width,height);
	gsGUI3DCheckbox* tsCheckbox = new gsGUI3DCheckbox(this,checkbox);
	gkGUI3D::getSingleton().addScriptMapping(checkbox,tsCheckbox);
	return tsCheckbox;
}


gsGUI3DProgressbar* tsPanel::createProgressbar(float x,float y,int width, int height)
{
	Gui3D::ProgressBar* progressbar = m_panelContainer->makeProgressBar(x,y,width,height);
	gsGUI3DProgressbar* tsProgressbar = new gsGUI3DProgressbar(this,progressbar);
	gkGUI3D::getSingleton().addScriptMapping(progressbar,tsProgressbar);
	return tsProgressbar;
}

gsGUI3DScrollbar* tsPanel::createScrollbar(float x,float y,int width, int height, float minValue, float maxValue)
{
	Gui3D::ScrollBar* scrollbar = m_panelContainer->makeScrollBar(x,y,width,height,minValue,maxValue);
	scrollbar->setDisplayValue(false);
	gsGUI3DScrollbar* tsScrollbar = new gsGUI3DScrollbar(this,scrollbar);
	gkGUI3D::getSingleton().addScriptMapping(scrollbar,tsScrollbar);
	return tsScrollbar;
}

gsGUI3DListbox* tsPanel::createListbox(float x,float y,int width, int height, int showElements)
{
	std::vector<gkString> items;
	Gui3D::Listbox* list = m_panelContainer->makeListbox(x,y,width,height, items, showElements );
	gsGUI3DListbox* tsList = new gsGUI3DListbox(this,list);
	gkGUI3D::getSingleton().addScriptMapping(list,tsList);
	return tsList;
}

gsGUI3DTextField* tsPanel::createTextfield(float x,float y,int width, int height,const gkString&  text)
{
	Gui3D::TextZone* textZone = m_panelContainer->makeTextZone(x,y,width,height,text);
	gsGUI3DTextField* tsTextzone = new gsGUI3DTextField(this,textZone);
	gkGUI3D::getSingleton().addScriptMapping(textZone,tsTextzone);
	return tsTextzone;
}

gsGUI3DCombobox* tsPanel::createCombobox(float x,float y,int width, int height,int showElements)
{
	std::vector<gkString> items;
	Gui3D::Combobox* combo = m_panelContainer->makeCombobox(x,y,width,height, items, showElements );
	gsGUI3DCombobox* tsCombo = new gsGUI3DCombobox(this,combo);
	gkGUI3D::getSingleton().addScriptMapping(combo,tsCombo);
	return tsCombo;
}

void tsPanel::destroy(gsGUI3DRectangle* rect)
{
	m_panelContainer->destroyRectangle(rect->cast<Gui3D::Rectangle*>());
}


void tsPanel::attachToObject(gsGameObject* parentObj)
{

	if(parentObj && parentObj->m_object)
	{
		if (m_notifier)
		{
			gkLogger::write("Detach modifier...",true);
			parentObj->cast<gkGameObject>()->removeEventListener(m_notifier);
			delete m_notifier;
		}

		m_notifier = new tsPanelNotifier(this);
		parentObj->cast<gkGameObject>()->addEventListener(m_notifier);
		m_notifier->notifyGameObjectEvent(parentObj->cast<gkGameObject>(),gkGameObject::Notifier::UPDATED);
	}

}

void tsPanel::setTransformState(const gkTransformState& state)
{
	if (m_is2D)
	{
		gkLogger::write("SetTransformstate not supported for Screen2D!",true);
//		static_cast<Gui3D::ScreenPanel*>(m_panelContainer)->setPosition(gkVector2(x,y));
	}
	else
	{
		Gui3D::Panel* panel = static_cast<Gui3D::Panel*>(m_panelContainer);
		panel->mNode->setPosition(state.loc);
		panel->mNode->setOrientation(state.rot);
		panel->mNode->setScale(state.scl);

	}


}
//void tsPanel::createProgressBar(float x,float y,int width, int height, const gkString& text)
//{
//	Gui3D::ProgressBar* btn = m_panel->makeProgressBar(x,y,width,height);
//	btn->setValue(0.2);
//}
//
//void tsPanel::createProgressBar(float x,float y,int width, int height, const gkString& text, gsSelf self,gsFunction onClick)
//{
//	Gui3D::ProgressBar* btn = m_panel->makeProgressBar(x,y,width,height);
//	gsGUI3DScriptCallback* callback = new gsGUI3DScriptCallback(text,self,onClick);
//	btn->setValueChangedCallback<gsGUI3DScriptCallback>(callback,&gsGUI3DScriptCallback::execute);
//	addCallback(callback);
//}
//
//void tsPanel::createProgressBar(float x,float y,int width, int height, const gkString& text,  gsFunction onClick)
//{
//	Gui3D::ProgressBar* btn = m_panel->makeProgressBar(x,y,width,height);
//	gsGUI3DScriptCallback* callback = new gsGUI3DScriptCallback(text,onClick);
//	btn->setValueChangedCallback<gsGUI3DScriptCallback>(callback,&gsGUI3DScriptCallback::execute);
//	addCallback(callback);
//}
//
//
//void tsPanel::createScrollBar(float x,float y,int width, int height, float min, float max,const gkString& text)
//{
//	Gui3D::ScrollBar* btn = m_panel->makeScrollBar(x,y,width,height,min,max);
//}
//
//void tsPanel::createScrollBar(float x,float y,int width, int height, float min, float max,const gkString& text, gsSelf self,gsFunction onClick)
//{
//	Gui3D::ScrollBar* btn = m_panel->makeScrollBar(x,y,width,height,min,max);
//	gsGUI3DScriptCallback* callback = new gsGUI3DScriptCallback(text,self,onClick);
//	btn->setValueChangedCallback<gsGUI3DScriptCallback>(callback,&gsGUI3DScriptCallback::execute);
//	addCallback(callback);
//}
//
//void tsPanel::createScrollBar(float x,float y,int width, int height,float min, float max, const gkString& text,  gsFunction onClick)
//{
//	Gui3D::ScrollBar* btn = m_panel->makeScrollBar(x,y,width,height,min,max);
//	gsGUI3DScriptCallback* callback = new gsGUI3DScriptCallback(text,onClick);
//	btn->setValueChangedCallback<gsGUI3DScriptCallback>(callback,&gsGUI3DScriptCallback::execute);
//	addCallback(callback);
//}
//
//void tsPanel::createTextZone(float x,float y,int width, int height, const gkString& name,const gkString& text)
//{
//	Gui3D::TextZone* btn = m_panel->makeTextZone(x,y,width,height,text);
//}
//
//void tsPanel::createTextZone(float x,float y,int width, int height,const gkString& name, const gkString& text, gsSelf self,gsFunction onClick)
//{
//	Gui3D::TextZone* btn = m_panel->makeTextZone(x,y,width,height,text);
//	gsGUI3DScriptCallback* callback = new gsGUI3DScriptCallback(name,self,onClick);
//	btn->setValueChangedCallback<gsGUI3DScriptCallback>(callback,&gsGUI3DScriptCallback::execute);
//	addCallback(callback);
//}
//
//void tsPanel::createTextZone(float x,float y,int width, int height, const gkString& name,const gkString& text, gsFunction onClick)
//{
//	Gui3D::TextZone* btn = m_panel->makeTextZone(x,y,width,height,text);
//	btn->setMaxStringLength(20);
//
//	gsGUI3DScriptCallback* callback = new gsGUI3DScriptCallback(name,onClick);
//	btn->setValueChangedCallback<gsGUI3DScriptCallback>(callback,&gsGUI3DScriptCallback::execute);
//	addCallback(callback);
//}



gkGorillaRectangle::gkGorillaRectangle(Gorilla::Rectangle* rect,const gkString& currentSpriteName)
	: m_rect(rect),m_currentSpriteName(currentSpriteName),m_animationPrefix(""),
	  m_from(0),m_currentFrame(0),m_to(0),m_timer(0.0f),m_initalTimer(0.0f),m_isAnimation(false)
{
	if (!currentSpriteName.empty())
		rect->background_image(currentSpriteName);
}

gkGorillaRectangle::gkGorillaRectangle(Gorilla::Rectangle* rect,
					const gkString& animationPrefix,
					int from, int to, float timer)
: m_rect(rect),m_currentSpriteName(""),m_animationPrefix(animationPrefix),
  m_from(from),m_currentFrame(from),m_to(to),m_timer(timer),m_initalTimer(timer),m_isAnimation(true)
{
	rect->background_image(animationPrefix+gkToString(from));
}

void gkGorillaRectangle::update(gkScalar delta)
{
	if (!m_isAnimation)
		return;

	if (m_timer < 0 ) {
		m_currentFrame++;
		if (m_currentFrame > m_to) {
			m_currentFrame = m_from;
		}
		m_rect->background_image(m_animationPrefix+gkToString(m_currentFrame));
		m_timer = m_initalTimer;
	} else {
		m_timer -= delta;
	}
}

void gsGorillaScreenFunctions::destroyRectangle(gsGorillaRectangle* rect)
{
	if (rect && m_layer)
	{
		// destroy the rectangle in the gorilla-layer
		m_layer->destroyRectangle(rect->_get()->getRect());
		// remove it from the internal rectangle-list
		m_rects.erase(rect->_get());
	}
}

void gsGorillaScreenFunctions::destroyMarkup(gsGorillaText* text)
{
	if (text && m_layer)
	{
		// destroy the rectangle in the gorilla-layer
		m_layer->destroyMarkupText(text->_get());
	}
}

void gsGorillaScreenFunctions::destroyLineList(gsGorillaLineList* lineList)
{
	if (lineList && m_layer)
	{
		// destroy the LineList in the gorilla-layer
		m_layer->destroyLineList(lineList->_get());
	}
}



gkGorillaRectangle* gsGorillaScreen3D::generateFromObject(gsGameObject* gsobj) {
	if (gsobj) {
		gkGameObject* gobj = gsobj->get();
		return generateFromObject(gobj);
	}
	return 0;
}

gkGorillaRectangle* gsGorillaScreen3D::generateFromObject(gkGameObject* gobj) {
	if (gobj->hasVariable("gorilla")){
		const gkVector3& pos = gobj->getWorldPosition();
		const gkVector3& scl = gobj->getWorldScale();


		if (gobj->hasVariable("sprite")) {
			gkGorillaRectangle* rect = createRectangle(pos.x*100.0f,-pos.z*100.0f,scl.x*100.0f,scl.z*100.0f);
			gkGorillaGameObjectCallback* callback = new gkGorillaGameObjectCallback(m_screen,rect);
			gobj->addEventListener(callback);

			const gkString& spriteName = gobj->getVariable("sprite")->getValueString();
			if (!spriteName.empty()){
				rect->getRect()->background_image(spriteName);
			}
			return rect;
		}
		else if (gobj->hasVariable("animsprite")) {
			gkString animPrefix = gobj->getVariable("animsprite")->getValueString();

			gkVariable* from  = gobj->getVariable("from");
			gkVariable* to 	  = gobj->getVariable("to");
			gkVariable* timer = gobj->getVariable("timer");

			if (!from || !to || !timer) {
				gkDebugScreen::printTo("You need to specify from,to,timer-properties for animsprite");
				return 0;
			}

			Gorilla::Rectangle* gRect = m_layer->createRectangle(pos.y*100.0f,-pos.z*100.0f,scl.x*100.0f,scl.y*100.0f);

			gkGorillaRectangle* rect = new gkGorillaRectangle(gRect,animPrefix,from->getValueInt(),to->getValueInt(),timer->getValueReal());
			m_rects.push_back(rect);
			gkGorillaGameObjectCallback* callback = new gkGorillaGameObjectCallback(m_screen,rect);
			gobj->addEventListener(callback);
			return rect;
		}
	}
	return 0;
}

void gsGorillaScreen3D::generateFromScene(gsScene* scene) {
	gkScene* gScene = scene->cast<gkScene>();
	gkGameObjectHashMap::Iterator iter(gScene->getObjects());
	while (iter.hasMoreElements()){
		gkGameObject* gobj = iter.getNext().second;
		gkDebugScreen::printTo("Check:"+gobj->getName());
		generateFromObject(gobj);

	}
}



