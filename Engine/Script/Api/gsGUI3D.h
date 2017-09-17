/*
 * ttGUIScript.h
 *
 *  Created on: Sep 11, 2012
 *      Author: ttrocha
 */

#ifndef TTGUISCRIPT_H_
#define TTGUISCRIPT_H_

#include <Gui3DScreenPanel.h>
#include "gsGUI3D.h"

#include "GUI/GUI3D/gkGUI3D.h"
#include "Script/Lua/gkLuaUtils.h"

#include <vector>
#include "gsCommon.h"
#include "gkGameObject.h"
#include "gsMath.h"
#include "Gui3DPanelColors.h"
#include "OgreViewport.h"
#include "gkScene.h"
#include "gkCamera.h"
#include "OgreCamera.h"
#include "gsCore.h"
#include "../../gkValue.h"
#include <cmath>


using std::vector;

class gsGUI3DCaption;
class gsGUI3DButton;
class gsGUI3DCheckbox;
class gsGUI3DCombobox;
class gsGUI3DListbox;
class gsGUI3DElement;
class gsGUI3DProgressbar;
class gsGUI3DScrollbar;
class gsGUI3DTextField;
class gsGUI3DRectangle;

#define GS_DEBUG(x) 	gkLogger::write(x,true);


	enum GuiType {
		ENONE=0,EButton, ECaption, ECheckbox, ECombobox, EListbox, EScrollbar, ETextzone, EProgressBar, ERectangle
	};



	#define CastPanelElement(type)\
	Gui3D::type* elem = static_cast<Gui3D::type*>(element);

	class gsGUI3DScriptCallback
	{
	public:
		gsGUI3DScriptCallback(gsFunction func,const gkString& data) : m_event(func),m_data(data){}
		gsGUI3DScriptCallback(gsSelf self,gsFunction func,const gkString& data) : m_event(self,func),m_data(data) {};
		~gsGUI3DScriptCallback() {
			gkLogger::write("Destructor gsGUI3DScript callback!");
		}

		bool execute(Gui3D::PanelElement* element);
	//	{
	//		m_event.beginCall();
	//
	//		gsGUI3DElement* mappedElem = ttGUI::getSingleton().getScriptMapping(element);
	//		if (mappedElem){
	//			m_event.addArgument(mappedElem);
	//		} else {
	//			gkDebugScreen::printTo("Unknown mapped element!");
	//		}
	//		m_event.addArgument(m_name);
	//		if (element->getType() == Gui3D::ECheckbox) {
	//			CastPanelElement(Checkbox)
	//			m_event.addArgument(elem->getChecked());
	//		}
	//		else if (element->getType() == Gui3D::ECombobox)
	//		{
	//			CastPanelElement(Combobox)
	//			m_event.addArgument(elem->getValue());
	//		}
	//		else if (element->getType() == Gui3D::EListbox)
	//		{
	//			CastPanelElement(Listbox)
	//			m_event.addArgument((int)elem->getValues().size());
	//			for (int i=0;i<elem->getValues().size();i++)
	//				m_event.addArgument(elem->getValues()[i]);
	//		}
	//		else if (element->getType() == Gui3D::EProgressBar)
	//		{
	//			CastPanelElement(ProgressBar)
	//			m_event.addArgument(static_cast<float>(elem->getValue()));
	//		}
	//		else if (element->getType() == Gui3D::EScrollbar)
	//		{
	//			CastPanelElement(ScrollBar)
	//			m_event.addArgument(static_cast<float>(elem->getValue()));
	//		}
	//		else if (element->getType() == Gui3D::ETextzone)
	//		{
	//			CastPanelElement(TextZone)
	//			m_event.addArgument(elem->getValue());
	//		}
	//
	//		m_event.call();
	//		return true;
	//	}

	private:
		gkLuaEvent m_event;
		gkString m_data;
	};

	#ifndef SWIG
	class tsPanelNotifier;
	#endif

	class tsPanel
	{
	public:
		typedef utList<gsGUI3DScriptCallback*> tsScriptCallbackList;

		tsPanel(Gui3D::PanelContainer* panel,bool is2D) : m_panelContainer(panel), m_notifier(0),m_is2D(is2D){};

		~tsPanel() {
			// delete all callbacks created by this panel
			clearCallbacks();

			if (m_panelContainer)
			{
				if (m_is2D)
				{
					gkGUI3D::getSingleton().removePanel2d(static_cast<Gui3D::ScreenPanel*>(m_panelContainer));
				}
				else
				{
					gkGUI3D::getSingleton().removePanel3d(static_cast<Gui3D::Panel*>(m_panelContainer));
				}
				if (m_panelContainer)
					delete m_panelContainer;
				m_panelContainer=0;
			}

			if (m_notifier)
				delete m_notifier;
			m_notifier=0;
		}
		void setVisible(bool show) {
			m_panelContainer->getGUILayer()->setVisible(show);
		}
		bool isVisible() {
			m_panelContainer->getGUILayer()->isVisible();
		}
		bool mouseIntersection() { return m_panelContainer->intersectionUnderCursor();}
		void attachToObject(gsGameObject* obj);
		void setPosition(float x,float y,float z);
		void setScale(float x,float y,float z);
		void setRotation(float x,float y, float z);
		gsVector2 getSize() { return gsVector2(m_panelContainer->getSize());}
		Gui3D::PanelColors* getSettings() {
			return m_panelContainer->getPanelColors();
		}
	#ifndef SWIG
		void setTransformState(const gkTransformState& st);
	#endif
		gsGUI3DCaption* createCaption(float x,float y,int width, int height,const gkString& text, int fontSize=-1,int align=0, int valign=0);

		gsGUI3DButton* createButton(float x,float y,int width, int height,const gkString& text, int fontSize=-1);

		gsGUI3DCheckbox* createCheckbox(float x,float y,int width, int height);
	//	void createCheckbox(float x,float y,int width, int height, const gkString& text, gsFunction onClick);
	//	void createCheckbox(float x,float y,int width, int height, const gkString& text, gsSelf self,gsFunction onClick);

		gsGUI3DCombobox* createCombobox(float x,float y,int width, int height, int showElements=3);
	/*	void createCheckbox(float x,float y,float width, float height, const gkString& text, gsFunction onClick);
		void createCheckbox(float x,float y,float width, float height, const gkString& text, gsSelf self,gsFunction onClick);
	*/

		gsGUI3DListbox* createListbox(float x,float y,int width, int height, int showElements=3);
	/*	void createCheckbox(float x,float y,float width, float height, const gkString& text, gsFunction onClick);
		void createCheckbox(float x,float y,float width, float height, const gkString& text, gsSelf self,gsFunction onClick);
	*/

		gsGUI3DProgressbar* createProgressbar(float x,float y,int width, int height);
	////	void createProgressBar(float x,float y,int width, int height, const gkString& text, gsFunction onClick);
	////	void createProgressBar(float x,float y,int width, int height, const gkString& text,gsSelf self,gsFunction onClick);
	//
		gsGUI3DScrollbar* createScrollbar(float x,float y,int width, int height, float minValue=0, float maxValue=100);
	//	void createScrollBar(float x,float y,int width, int height, float minValue, float maxValue, const gkString& text,gsFunction onClick);
	//	void createScrollBar(float x,float y,int width, int height,float minValue, float maxValue, const gkString& text,gsSelf self,gsFunction onClick);
	//
		gsGUI3DTextField* createTextfield(float x,float y,int width, int height,const gkString&  text="");
	//	void createTextZone(float x,float y,int width, int height,const gkString&  name, const gkString& text, gsFunction onClick);
	//	void createTextZone(float x,float y,int width, int height,const gkString&  name, const gkString& text, gsSelf self,gsFunction onClick);

		gsGUI3DRectangle* createRectangle(float x,float y,int width,int height,const gsVector4& col=gsVector4(1.f,1.f,1.f,.75f));
		gsGUI3DRectangle* createRectangle(float x,float y,int width,int height,const gkString& spriteName);

		void setBackgroundColor(const gsVector4&);
		void setBackgroundColor(float r,float g,float b,float a);
		void setBackgroundBorder(float width,const gsVector4& col);


		void destroy(gsGUI3DRectangle* rect);
	#ifndef SWIG
		void addCallback(gsGUI3DScriptCallback* callback);
		void removeCallback(gsGUI3DScriptCallback* callback);
		void clearCallbacks();
		Gui3D::PanelContainer* m_panelContainer;
		bool m_is2D;

	#endif
	private:
		tsScriptCallbackList m_callbackList;
		tsPanelNotifier* m_notifier;
	};

	#ifndef SWIG
	class tsPanelNotifier : public gkGameObject::Notifier
	{
	public:
		tsPanelNotifier(tsPanel* panel) : m_panel(panel) {}
		virtual ~tsPanelNotifier() {}
		void notifyGameObjectEvent(gkGameObject* gobj, const Event& id)
		{
			if (id==gkGameObject::Notifier::UPDATED)
			{
				m_panel->setTransformState(gobj->getWorldTransformState());
			}
			else if (id==gkGameObject::Notifier::INSTANCE_DESTROYED)
			{
				if (m_panel->m_is2D)
				{
					static_cast<Gui3D::ScreenPanel*>(m_panel->m_panelContainer)->getGUILayer()->setVisible(false);
				}
				else
				{
					static_cast<Gui3D::Panel*>(m_panel->m_panelContainer)->mNode->setVisible(false,true);
				}
			}
			else if  (id==gkGameObject::Notifier::INSTANCE_CREATED)
			{
				if (m_panel->m_is2D)
				{
					static_cast<Gui3D::ScreenPanel*>(m_panel->m_panelContainer)->getGUILayer()->setVisible(true);
				}
				else
				{
					static_cast<Gui3D::Panel*>(m_panel->m_panelContainer)->mNode->setVisible(true,true);
				}
			}
		}
	private:
		tsPanel* m_panel;
	};
	#endif

	class gsGUI3DElement
	{
	public:
		gsGUI3DElement(tsPanel* panel,Gui3D::PanelElement* elem)
			:m_elem(elem),m_container(panel),m_name("undefined"){}
		~gsGUI3DElement() {
			if (m_elem)
			{
				//TODO: Wo wird das freigegeben?
	//			delete m_elem;
				m_elem=0;
			}
		}

		void setSize(int width, int height) {
			m_elem->setSize(width,height);
		}

		void setPosition(int width,int height) {
			m_elem->setPosition(width,height);
		};
		gsVector3 getPosition() {
			gkVector2 pos = m_elem->getPosition();
			return gsVector3(pos.x,pos.y,0);
		};

		void setName(const gkString& name) {
			m_name = name;
		}

		const gkString& getName() {
			return m_name;
		}

		bool isHovered() { return m_elem->getOvered();}
		bool isFocused() { return m_elem->getFocus();};
		// TODO: check if this focuses or only sets the internal flag
		void setFocus(bool b) { m_elem->setFocus(b);}
		template<class T> T cast() { return static_cast<T>(m_elem);}
		GuiType getType() { return (GuiType)m_elem->getType();  }
	protected:
		Gui3D::PanelElement* m_elem;
		tsPanel* m_container;
		gkString m_name;
	};

	#define SCRIPT_CALLBACKS(TYPE,CALLBACKNAME,SCRIPT_CALLBACKNAME)\
	void SCRIPT_CALLBACKNAME(gsFunction func,const gkString& data="") {\
		gsGUI3DScriptCallback* callback = new gsGUI3DScriptCallback(func,data);\
		cast<Gui3D::TYPE*>()->CALLBACKNAME<gsGUI3DScriptCallback>(callback,&gsGUI3DScriptCallback::execute);\
		m_container->addCallback(callback);\
	}\
	void SCRIPT_CALLBACKNAME(gsSelf self,gsFunction func,const gkString& data="") {\
		gsGUI3DScriptCallback* callback = new gsGUI3DScriptCallback(self,func,data);\
		cast<Gui3D::TYPE*>()->CALLBACKNAME<gsGUI3DScriptCallback>(callback,&gsGUI3DScriptCallback::execute);\
		m_container->addCallback(callback);\
	}

	//	void setPressedCallback(gsFunction func) {
	//		gsGUI3DScriptCallback* callback = new gsGUI3DScriptCallback(func);
	//		cast<Gui3D::Button*>()->setPressedCallback<gsGUI3DScriptCallback>(callback,&gsGUI3DScriptCallback::execute);
	//		m_container->addCallback(callback);
	//	}
	//
	//	void setPressedCallback(gsSelf self,gsFunction func) {
	//		gsGUI3DScriptCallback* callback = new gsGUI3DScriptCallback(self,func);
	//		cast<Gui3D::Button*>()->setPressedCallback<gsGUI3DScriptCallback>(callback,&gsGUI3DScriptCallback::execute);
	//		m_container->addCallback(callback);
	//	}

	class gsRectangleDesigned
	{
	public:
		gsRectangleDesigned(Gorilla::Rectangle* rect) : m_rect(rect){}

		void setBackground(const gsVector4& col){
			m_rect->background_colour(Ogre::ColourValue(col.x,col.y,col.z,col.w));
		}
		void setBackground(float r,float g, float b, float a){
			m_rect->background_colour(Ogre::ColourValue(r,g,b,a));
		}

		void setBackground(const gkString& spriteName){
			m_rect->background_image(spriteName);
		}
		void setBorder(float width,const gsVector4& col){
			m_rect->border(width,Ogre::ColourValue(col.x,col.y,col.z,col.w));
		}
		void setBorder(float width,float r,float g, float b, float a){
			m_rect->border(width,Ogre::ColourValue(r,g,b,a));
		}
	private:
		Gorilla::Rectangle* m_rect;
	};

	class gsGUI3DButton : public gsGUI3DElement,public gsRectangleDesigned
	{
	public:
		gsGUI3DButton(tsPanel* panel,Gui3D::Button* btn) : gsGUI3DElement(panel,btn), gsRectangleDesigned(btn->getRectangleDesign()) {}
		~gsGUI3DButton() {}

		void setText(const gkString& txt) {
			cast<Gui3D::Button*>()->setText(txt);
		}

		void setBackgroundImage(const gkString& defaultSpriteName,
				const gkString& overSpriteName,
				const gkString& clickedSpriteName,
				const gkString& inactiveSpriteName
			) {
			cast<Gui3D::Button*>()->setBackgroundImage(overSpriteName,defaultSpriteName,inactiveSpriteName,clickedSpriteName);
		}

		void setTextColor(float r,float g, float b, float a)
		{
			cast<Gui3D::Button*>()->getCaption()->colour(Ogre::ColourValue(r,g,b,a));
		}

		void setFont(int fontIdx)
		{
			cast<Gui3D::Button*>()->getCaption()->font(fontIdx);
		}

		SCRIPT_CALLBACKS(Button,setPressedCallback,onClicked);

	};

	class gsGUI3DCaption : public gsGUI3DElement
	{
	public:
		gsGUI3DCaption(tsPanel* panel,Gui3D::Caption* cap) : gsGUI3DElement(panel,cap) {}
		~gsGUI3DCaption() {}

		void setText(const gkString& txt) {
			cast<Gui3D::Caption*>()->text(txt);
		}

		void setBackgroundImage(const gkString& backgroundSpritename) {
			cast<Gui3D::Caption*>()->setBackgroundImage(backgroundSpritename);
		}

	//	mCaption->align(textAlign);
	//	mCaption->vertical_align(verticalAlign);
	//	mCaption->background(getColors()->transparent);
	//	mCaption->colour(getColors()->captionText);

		void setTextColor(float r,float g, float b, float a) {
			cast<Gui3D::Caption*>()->textColor(Ogre::ColourValue(r,g,b,a));
		}

		void setTextColor(const gsVector4& col)
		{
			cast<Gui3D::Caption*>()->textColor(Ogre::ColourValue(col.x,col.y,col.z,col.w));
		}

		void setFont(int fontIdx)
		{
			cast<Gui3D::Button*>()->getCaption()->font(fontIdx);
		}

	};

	class gsGUI3DCheckbox : public gsGUI3DElement
	{
	public:
		gsGUI3DCheckbox(tsPanel* panel,Gui3D::Checkbox* btn) : gsGUI3DElement(panel,btn) {}
		~gsGUI3DCheckbox() {}

		void setChecked(bool check) {
			cast<Gui3D::Checkbox*>()->setChecked(check);
		}

		bool isChecked(void) {
			return cast<Gui3D::Checkbox*>()->getChecked();
		}



		SCRIPT_CALLBACKS(Checkbox,setSelecteStateChangedCallback,onChanged)
	};

	class gsGUI3DTextField : public gsGUI3DElement
	{
	public:
		gsGUI3DTextField(tsPanel* panel,Gui3D::TextZone* btn) : gsGUI3DElement(panel,btn) {}
		~gsGUI3DTextField() {}

		void setValue(const gkString& value) {
			cast<Gui3D::TextZone*>()->setValue(value);
		}

		gkString getValue(void) {
			return cast<Gui3D::TextZone*>()->getValue();
		}

		void setMaxLength(int length) {
			cast<Gui3D::TextZone*>()->setMaxStringLength(length);
		}

		void setFont(int font) { cast<Gui3D::TextZone*>()->getCaption()->font(font);}

		SCRIPT_CALLBACKS(TextZone,setValueChangedCallback,onChanged)
	};

	class gsGUI3DRectangle : public gsGUI3DElement
	{
	public:
		gsGUI3DRectangle(tsPanel* panel, Gui3D::Rectangle* rect) : gsGUI3DElement(panel,rect) {}

		void setBackground(const gsVector4& col){
			cast<Gui3D::Rectangle*>()->setBackground(Ogre::ColourValue(col.x,col.y,col.z,col.w));
		}
		void setBackground(float r,float g, float b, float a){
			cast<Gui3D::Rectangle*>()->setBackground(Ogre::ColourValue(r,g,b,a));
		}

		void setBackground(const gkString& spriteName){
			cast<Gui3D::Rectangle*>()->setBackground(spriteName);
		}
		void setBorder(float width,const gsVector4& col){
			cast<Gui3D::Rectangle*>()->setBorder(width,Ogre::ColourValue(col.x,col.y,col.z,col.w));
		}
		void setBorder(float width,float r,float g, float b, float a){
			cast<Gui3D::Rectangle*>()->setBorder(width,Ogre::ColourValue(r,g,b,a));
		}

	};

	class gsGUI3DProgressbar : public gsGUI3DElement
	{
	public:
		gsGUI3DProgressbar(tsPanel* panel,Gui3D::ProgressBar* btn) : gsGUI3DElement(panel,btn) {}
		~gsGUI3DProgressbar() {}

		void setBackground(const gsVector4& col){
			cast<Gui3D::ProgressBar*>()->background(Ogre::ColourValue(col.x,col.y,col.z,col.w));
		}
		void setBackground(float r,float g, float b, float a){
			cast<Gui3D::ProgressBar*>()->background(Ogre::ColourValue(r,g,b,a));
		}


		void setValue(float value) {
			cast<Gui3D::ProgressBar*>()->setValue(value);
		}

		float getValue(void) {
			return cast<Gui3D::ProgressBar*>()->getValue();
		}


		SCRIPT_CALLBACKS(ProgressBar,setValueChangedCallback,onChanged)
	};

	class gsGUI3DScrollbar : public gsGUI3DElement
	{
	public:
		gsGUI3DScrollbar(tsPanel* panel,Gui3D::ScrollBar* btn) : gsGUI3DElement(panel,btn) {}
		~gsGUI3DScrollbar() {}

		void setValue(float value) {
			cast<Gui3D::ScrollBar*>()->setValue(value);
		}

		float getValue(void) {
			return cast<Gui3D::ScrollBar*>()->getValue();
		}

		void setStep(float step) {
			cast<Gui3D::ScrollBar*>()->setStep(step);
		}


		SCRIPT_CALLBACKS(ScrollBar,setValueChangedCallback,onChanged)
	};


	class gsGUI3DCombobox : public gsGUI3DElement
	{
	public:
		gsGUI3DCombobox(tsPanel* panel,Gui3D::Combobox* btn) : gsGUI3DElement(panel,btn) {}
		~gsGUI3DCombobox() {}

		void setValue(gkString str) {
			cast<Gui3D::Combobox*>()->setCurrentValue(str);
		}

		gkString getValue(void) {
			return cast<Gui3D::Combobox*>()->getValue();
		}

		void addValue(const gkString& value) {
			cast<Gui3D::Combobox*>()->addItem(value);
		}

		SCRIPT_CALLBACKS(Combobox,setValueChangedCallback,onValueChanged)

	};

	class gsGUI3DListbox : public gsGUI3DElement
	{
	public:
		gsGUI3DListbox(tsPanel* panel,Gui3D::Listbox* btn) : gsGUI3DElement(panel,btn) {}
		~gsGUI3DListbox() {}

		void setValue(gkString str) {
			cast<Gui3D::Listbox*>()->setCurrentValue(str);
		}

		void addValue(const gkString& value) {
			cast<Gui3D::Listbox*>()->addItem(value);
			cast<Gui3D::Listbox*>()->_actualize();
		}

		int amountSelected() {
			return 	cast<Gui3D::Listbox*>()->getValues().size();
		}

		gkString getSelected(int nr) {
			Gui3D::Listbox* lb = cast<Gui3D::Listbox*>();
			if (nr<lb->getValues().size())
			{
				return lb->getValues()[nr];
			}
			return 0;
		}

		int getSelectedIdx(int nr) {
			Gui3D::Listbox* lb = cast<Gui3D::Listbox*>();
			std::vector<int> vec = lb->getValuesIDX();
			if (nr<lb->getValuesIDX().size())
			{
				return vec[nr];
			}
			return -1;
		}

		void clear() {
			Gui3D::Listbox* lb = cast<Gui3D::Listbox*>();
			lb->clear();
		}

		SCRIPT_CALLBACKS(Listbox,setValueChangedCallback,onValueChanged)
	};




	class gsGUI3D
	{
	public:

		gsGUI3D(const gkString& atlas="dejavu",gsScene* scene=0 );
		~gsGUI3D() {};

		void showGUI();
		void hideGUI();
		void showConsole(bool showit);

		Gui3D::PanelColors* getPanelColors() {
			return gkGUI3D::getSingleton().getPanelColors();
		}
//		void showConsole();
//		void hideConsole();

		tsPanel* createPanel(float xpos,float ypos,float width,float height, const gkString& name,const gkString& atlas="dejavu", bool useBackground=true);
		tsPanel* createPanel3D(float width,float height, const gkString& name,const gkString& atlas="dejavu",float distance=-1, bool background=true,const gkString& sceneName="");

		void setMousePointer(const gkString& mouseSprite) {
			gkGUI3D::getSingleton().setMouseCursor(mouseSprite);
		}

		void showCursor(bool show) {
			gkGUI3D::getSingleton().showCursor(show);
		}

		void addConsoleCommand(const gkString& commandName,gsSelf self,gsFunction func);
		void addConsoleCommand(const gkString& commandName,gsFunction func);
	private:
		gkGUI3D* m_gui;
	};

typedef Gorilla::Screen gkGorillaScreen;
typedef Gorilla::ScreenRenderable gkGorillaScreen3D;
typedef Gorilla::TextureAtlas gkTextureAtlas;
typedef Gorilla::Sprite gkSprite;

class gsSprite {
public:
	gsSprite(gkSprite* sprite) : m_sprite(sprite){
	}

private:
	gkSprite* m_sprite;
};



// FIXME: make this accessable via manager
typedef Gorilla::MarkupText gkGorillaText;

class gsGorillaText
{
public:
	gsGorillaText(Gorilla::MarkupText* markup) : m_markup(markup) {}
	gkString getText() 					{ return m_markup->text();};
	void setText(const gkString& txt) 	{ m_markup->text(txt);}
	void setSize(float x,float y)  		{ m_markup->size(x,y);}
	float getWidth()						{ return m_markup->width();}
	float getHeight()						{ return m_markup->height();}
	void setBackgroundColor(gsVector4 col) { m_markup->background(Ogre::ColourValue(col.x,col.y,col.z,col.w));}
	gsVector4 getBackgroundColor()		{ Ogre::ColourValue col=m_markup->background(); return gsVector4(col.r,col.g,col.b,col.a);}
	void setPosition(float x,float y) 	{ m_markup->left(x); m_markup->top(y);}
	float getX()							{ return m_markup->left(); }
	float getY()							{ return m_markup->top(); }
#ifndef SWIG
	gkGorillaText* _get() { return m_markup;}
#endif
private:
	Gorilla::MarkupText* m_markup;
};


typedef Gorilla::LineList gkGorillaLineList;

class gsGorillaLineList {
public:
	gsGorillaLineList(Gorilla::LineList* list) : m_list(list) {}

	void begin(float thickness,const gsVector4& col=gsVector4(1.0f,1.0f,1.0f,0.75f))
	{
		m_list->begin(thickness,Ogre::ColourValue(col.x,col.y,col.z,col.w));
	}

	void position(float x,float y)
	{
		m_list->position(x,y);
	}

	void endLine(bool closed=false)
	{
		m_list->end(closed);
	}

#ifndef SWIG
	gkGorillaLineList* _get() { return m_list;}
#endif
private:
	Gorilla::LineList* m_list;
};

class gkGorillaRectangle {
public:
	gkGorillaRectangle(Gorilla::Rectangle* rect,
						const gkString& currentSpriteName="");
	gkGorillaRectangle(Gorilla::Rectangle* rect,
						const gkString& animationPrefix,
						int from, int to, float timer);

	void update(gkScalar delta);

	inline Gorilla::Rectangle* getRect() { return m_rect; }
private:
	Gorilla::Rectangle* m_rect;

	bool m_isAnimation;
	gkString m_currentSpriteName;
	gkString m_animationPrefix;
	int m_from;
	int m_currentFrame;
	int m_to;
	float m_timer;
	float m_initalTimer;

};

class gsGorillaRectangle {
public:
	gsGorillaRectangle(gkGorillaRectangle* rect) : m_rect(rect) {}
	virtual ~gsGorillaRectangle() {}

	void setBackgroundColor(float r,float g, float b, float a)
	{
		Ogre::ColourValue col(r,g,b,a);
		m_rect->getRect()->background_colour(col);
	}

	void setBackgroundColor(const gsVector4& c) {
		m_rect->getRect()->background_colour(Ogre::ColourValue(c.x,c.y,c.z,c.w));
	}

	float getPosX() {
		return m_rect->getRect()->position().x;
	}

	float getPosY() {
		return m_rect->getRect()->position().y;
	}

	float getWidth() {
		return m_rect->getRect()->width();
	}

	float getHeight() {
		return m_rect->getRect()->height();
	}

	void setPosition(float x, float y) {
		m_rect->getRect()->position(x,y);
	}

	void setSprite(const gkString& name) {
		m_rect->getRect()->background_image(name);
	}

	void update(float dt=0.0166666f) {
		m_rect->update(dt);
	}

	void setBorder(float width,gsVector4* col) {
		m_rect->getRect()->border(width,Ogre::ColourValue(col->x,col->y,col->z,col->w));
	}

	gkGorillaRectangle* _get() {
		return m_rect;
	}
private:
	gkGorillaRectangle* m_rect;
};

class gsTextureAtlas {
public:
	gsTextureAtlas(gkTextureAtlas* atlas) : m_atlas(atlas) {}
	~gsTextureAtlas() {}


//	void setFontOffset(int fontsize,float offsetX,float offsetY);


	gkSprite* createSprite(const gkString& sprite_name,const float x,const float y,const float w,const float h) {
		return m_atlas->createSprite(sprite_name,x,y,w,h);
	}

	void createSpriteTiles(const gkString& prefix,int cols,int rows) {
		gkVector2 texDim(m_atlas->getTextureSize());

		float pxCol = texDim.x / cols;
		float pxRow = texDim.y / rows;


		for (int c=0;c<cols;c++){
			for (int r=0;r<rows;r++) {
				gkString spriteName(prefix+gkToString(r)+"_"+gkToString(c));
				createSprite(spriteName,pxCol*c,pxRow*r,pxCol,pxRow);
				gkDebugScreen::printTo("Created sprite:"+spriteName);
			}
		}
	}

	gkSprite* getSprite(const gkString& name) {
		return m_atlas->getSprite(name);
	}

	float getTexSizeX() {
		return m_atlas->getTextureSize().x;
	}

	float getTexSizeY() {
		return m_atlas->getTextureSize().y;
	}

	void setFontOffset(int fontsize,float offsetX,float offsetY);

	void loadPackfile(const gkString& packfile) {

		gkPath packFile(packfile);
		if (packFile.exists()) {
			gkString txt(packFile.getAsString());

			utStringArray lines;
			utStringUtils::split(lines,txt,"\n");
			utStringArray::Iterator iter(lines);
			int idx = 0;
			int size = lines.size();
			while (4+idx*7 < size) {
				gkString spriteName(lines.at(4+idx*7));

				utStringArray nameA;


				gkString xy(lines.at(4+idx*7+2));
				gkString size(lines.at(4+idx*7+3));

				utStringArray xyA;
				utStringUtils::split(xyA,xy,":, ");



				utStringArray dimA;
				utStringUtils::split(dimA,size,":, ");


				float x,y,w,h;
				gkFromString(xyA.at(1),x);
				gkFromString(xyA.at(2),y);
				gkFromString(dimA.at(1),w);
				gkFromString(dimA.at(2),h);


				utStringUtils::split(nameA,spriteName,"-");
				if (nameA.at(0)=="font") {
					int fontsize;
					gkFromString(nameA.at(1),fontsize);
					setFontOffset(fontsize,x,y);
					gkDebugScreen::printTo("font:"+nameA.at(1)+"->x:"+xyA.at(1)+"->y:"+xyA.at(2));
				} else {
					gkDebugScreen::printTo("sprite:"+spriteName+"->x:"+xyA.at(1)+"->y:"+xyA.at(2)+" dim:"+dimA.at(1)+","+dimA.at(2));
					createSprite(spriteName,x,y,w,h);
				}


				idx++;

			}
		}
	}
private:
	gkTextureAtlas* m_atlas;
};

class gsGorillaScreenFunctions
{
public:
	gsGorillaScreenFunctions(Gorilla::Layer* layer) : m_layer(layer) {}

	virtual ~gsGorillaScreenFunctions() {
		GorillaRects::Iterator iter(m_rects);
		while (iter.hasMoreElements()){
			gkGorillaRectangle* rect = iter.getNext();
			delete rect;
		}

	}

	gkGorillaRectangle* createRectangle(float x,float y,float width,float height,const gkString& spritename="")
	{
		if (!spritename.empty() && (width < 0.f || height < 0.f))
		{
			Gorilla::Sprite* sprite = m_layer->_getSprite(spritename);
			if (sprite)
			{
				width = sprite->spriteWidth * abs(width);
				height = sprite->spriteHeight * abs(height);
			}
		}

		gkGorillaRectangle* rect =  new gkGorillaRectangle(m_layer->createRectangle(x,y,width,height),spritename);

		m_rects.push_back(rect);
		return rect;
	}

	gkGorillaText* createMarkupText(int defaultGlyphIndex, float x, float y, const gkString& text)
	{
		gkGorillaText* mtext = m_layer->createMarkupText(defaultGlyphIndex,x,y,text);

		return mtext;
	}

	gkGorillaLineList* createLineList(float thickness,const gsVector4& col=gsVector4(1.0f,1.0f,1.0f,0.5f))
	{
		gkGorillaLineList* lineList = m_layer->createLineList();
		lineList->begin(thickness,Ogre::ColourValue(col.x,col.y,col.z,col.w));
		return lineList;
	}

	void destroyRectangle(gsGorillaRectangle* rect);
	void destroyMarkup(gsGorillaText* text);
	void destroyLineList(gsGorillaLineList* lineList);


	int getRectAmount() {
		return m_rects.size();
	}

	gkGorillaRectangle* getRect(int i) {
		if (i<m_rects.size()) {
			return m_rects.at(i);
		}
		gkDebugScreen::printTo("Index out of bounds:"+gkToString(i)+" max:"+gkToString(((int)m_rects.size()-1)));
		return 0;
	}

protected:
	Gorilla::Layer* m_layer;

	typedef utList<gkGorillaRectangle*> GorillaRects;

	GorillaRects m_rects;


};

class gsGorillaScreen : public gsGorillaScreenFunctions
{
	friend class gsGorilla;
public:
	gsGorillaScreen(gkGorillaScreen* screen)
		: m_screen(screen), gsGorillaScreenFunctions(screen->createLayer(0))
	{}

	~gsGorillaScreen() {}

	void setVisible(bool visible)
	{
		m_screen->setVisible(visible);
	}


private:
	gkGorillaScreen* m_screen;
};

class gsGorillaScreen3D : public gsGorillaScreenFunctions
{
	friend class gsGorilla;
public:
	class gkGorillaGameObjectCallback : public gkGameObject::Notifier {
	public:
		gkGorillaGameObjectCallback(Gorilla::ScreenRenderable* scrn,gkGorillaRectangle* rect)
			: m_screenRenderable(scrn),m_rect(rect)
		{}
		~gkGorillaGameObjectCallback() {
		}

		void notifyGameObjectEvent(gkGameObject* gobj, const Event& id) {
			const gkVector3& pos = gobj->getWorldPosition();
			m_rect->getRect()->position(pos.y*100.0f,-pos.z*100.0f);
		}

	private:
		Gorilla::ScreenRenderable* m_screenRenderable;
		gkGorillaRectangle* m_rect;
	};

	gsGorillaScreen3D(gkGorillaScreen3D* screen) : m_screen(screen), gsGorillaScreenFunctions(screen->createLayer(0))
	{}

	~gsGorillaScreen3D() {}

	void generateFromScene(gsScene* scene);
	gkGorillaRectangle* generateFromObject(gkGameObject* gobj);
	gkGorillaRectangle* generateFromObject(gsGameObject* gobj);
private:
	gkGorillaScreen3D* m_screen;
};


class gsGorilla
{
public:
		gsGorilla() {
			if (!(m_silverback = Gorilla::Silverback::getSingletonPtr()))
				m_silverback = new Gorilla::Silverback;
		}

		gkTextureAtlas* loadAtlas(const gkString& name,const gkString group=Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME)
		{
			return m_silverback->loadAtlas(name,group);
		}

		gkGorillaScreen* createScreen(gsScene* scene, gkString atlas="")
		{
			if (atlas.empty())
			{
				loadAtlas("dejavu");
				atlas="dejavu";
			}
			else
			{
				loadAtlas(atlas);
			}

			if (scene) {
				Ogre::Viewport* vp = (scene->cast<gkScene>())->getMainCamera()->getCamera()->getViewport();
				return m_silverback->createScreen(vp,atlas);
			}
			return 0;
		}

		gkGorillaScreen3D* createScreen3D(gsGameObject* rootObj, gkString atlas="" ){
			if (atlas.empty())
			{
				loadAtlas("dejavu");
				atlas="dejavu";
			}
			else
			{
				loadAtlas(atlas);
			}

			if (rootObj) {
				gkGameObject* gobj = rootObj->get();
				gkVector3 ws(gobj->getWorldScale());

				gkGorillaScreen3D* screen3d = m_silverback->createScreenRenderable(gkVector2(ws.x,ws.y),atlas);


				//gobj->getNode()->attachObject(screen3d);
				gobj->_attachMoveable(screen3d);
				return screen3d;
			}
			return 0;
		}

		void destroyScreen(gsGorillaScreen* screen) {
			if (screen)
				m_silverback->destroyScreen(screen->m_screen);
		}
private:
		Gorilla::Silverback* m_silverback;


};



#endif /* TTGUISCRIPT_H_ */
