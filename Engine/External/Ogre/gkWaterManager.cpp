#include <assert.h>
#include <SkyX.h>
#include <Hydrax.h>
#include <Noise/Perlin/Perlin.h>
#include <Modules/ProjectedGrid/ProjectedGrid.h>
#include <OgreCompositorManager.h>
#include "gkWeather.h"
#include "gkWaterManager.h"


gkWaterManager::gkWaterManager()
    : mHydrax(NULL),
    mListener(NULL),
    mSceneMgr(NULL),
    mWindow(NULL),
    mCamera(NULL)
{

}

gkWaterManager::~gkWaterManager()
{

}

bool gkWaterManager::initialize(Ogre::SceneManager* _sceneMgr, Ogre::RenderWindow* _window, Ogre::Camera* _camera)
{
    assert(_sceneMgr);
    assert(_window);
    assert(_camera);

    mSceneMgr = _sceneMgr;
    mWindow = _window;
    mCamera = _camera;

    mHydrax = new Hydrax::Hydrax(mSceneMgr, mCamera, mWindow->getViewport(0));
    // Create our projected grid module
    Hydrax::Module::ProjectedGrid *mModule
        = new Hydrax::Module::ProjectedGrid(// Hydrax parent pointer
        mHydrax,
        // Noise module
        new Hydrax::Noise::Perlin(/*Generic one*/),
        // Base plane
        Ogre::Plane(Ogre::Vector3(0, 1, 0), Ogre::Vector3(0, 0, 0)),
        // Normal mode
        Hydrax::MaterialManager::NM_VERTEX,
        // Projected grid options
        Hydrax::Module::ProjectedGrid::Options(/*264 /*Generic one*/));

    // Set our module
    mHydrax->setModule(static_cast<Hydrax::Module::Module*>(mModule));

    // Load all parameters from config file
    // Remarks: The config file must be in Hydrax resource group.
    // All parameters can be set/updated directly by code(Like previous versions),
    // but due to the high number of customizable parameters, since 0.4 version, Hydrax allows save/load config files.
     mHydrax->loadCfg("HydraxDemo.hdx");

    // Create water
    mHydrax->create();

    mListener = new gkWaterListener(mWindow, mCamera, mSceneMgr);
    Ogre::Root::getSingletonPtr()->addFrameListener(mListener);

    return true;
}

void gkWaterManager::destroy()
{
    if(mListener)
    {
        Ogre::Root::getSingleton().removeFrameListener(mListener);
        delete mListener;
        mListener = NULL;
    }

    if(mHydrax)
    {
        /*Ogre::CompositorChain* chain =
            Ogre::CompositorManager::getSingleton().getCompositorChain(mCamera->getViewport());
        assert(chain);*/
        delete mHydrax;
        mHydrax = NULL;

    }
}

bool gkWaterListener::frameStarted( const Ogre::FrameEvent& e )
{
    // Check camera height
    Ogre::RaySceneQuery *raySceneQuery =
        mSceneMgr->
        createRayQuery(Ogre::Ray(mCamera->getPosition() + Ogre::Vector3(0,1000000,0),
        Ogre::Vector3::NEGATIVE_UNIT_Y));
    Ogre::RaySceneQueryResult& qryResult = raySceneQuery->execute();
    Ogre::RaySceneQueryResult::iterator i = qryResult.begin();
    if (i != qryResult.end() && i->worldFragment)
    {
        if (mCamera->getPosition().y < i->worldFragment->singleIntersection.y + 30)
        {
            mCamera->
                setPosition(mCamera->getPosition().x,
                i->worldFragment->singleIntersection.y + 30,
                mCamera->getPosition().z);
        }
    }

    delete raySceneQuery;

    // Update Hydrax
    Hydrax::Hydrax* hydrax = gkWaterManager::getSingletonPtr()->getHydrax();
    SkyX::SkyX* skyx = gkWheather::getSingletonPtr()->getSkyX();
    hydrax->update(e.timeSinceLastFrame);
    hydrax->setSunPosition(skyx->getController()->getSunDirection() * 10000);

    return true;
}

gkWaterListener::gkWaterListener( Ogre::RenderWindow* win, Ogre::Camera* cam, Ogre::SceneManager *sm )
    : mWindow(win),
    mCamera(cam),
    mSceneMgr(sm)
{

}

UT_IMPLEMENT_SINGLETON(gkWaterManager);
