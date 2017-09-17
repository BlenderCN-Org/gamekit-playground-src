#ifndef _GK_WATER_MANAGER_H__
#define _GK_WATER_MANAGER_H__

#include "utSingleton.h"

class gkWaterListener
    : public Ogre::FrameListener
{
public:
    gkWaterListener(Ogre::RenderWindow* win, Ogre::Camera* cam, Ogre::SceneManager *sm);
    virtual bool frameStarted( const Ogre::FrameEvent& evt );

private:
    Ogre::RenderWindow* mWindow;
    Ogre::Camera* mCamera;
    Ogre::SceneManager* mSceneMgr;
};

class gkWaterManager
    : public utSingleton<gkWaterManager>
{
public:
    gkWaterManager();
    virtual ~gkWaterManager();

    virtual bool initialize(Ogre::SceneManager* _sceneMgr, Ogre::RenderWindow* _window, Ogre::Camera* _camera);
    void destroy();
    Hydrax::Hydrax * getHydrax() const { return mHydrax; }

    UT_DECLARE_SINGLETON(gkWaterManager);

private:
    Hydrax::Hydrax *mHydrax;
    gkWaterListener* mListener;

    Ogre::SceneManager* mSceneMgr;
    Ogre::RenderWindow* mWindow;
    Ogre::Camera* mCamera;
};

#endif //_GK_WATER_MANAGER_H__
