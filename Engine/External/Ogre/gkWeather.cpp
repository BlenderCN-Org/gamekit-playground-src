#include <assert.h>
#include <SkyX.h>
#include "gkWeather.h"


gkWheather::gkWheather(void)
    : mWheatherListener(NULL),
    mBasicController(NULL),
    mSkyX(NULL),
    mSceneMgr(NULL),
    mWindow(NULL),
    mCamera(NULL)
{
}

gkWheather::~gkWheather(void)
{

}

bool gkWheather::initialize(Ogre::SceneManager* _sceneMgr, Ogre::RenderWindow* _window, Ogre::Camera* _camera)
{
    assert(_sceneMgr);
    assert(_window);
    assert(_camera);

    mSceneMgr = _sceneMgr;
    mWindow = _window;
    mCamera = _camera;

    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.8f, 0.8f, 0.8f));

    Ogre::Vector3 lightdir(0.55f, -0.3f, 0.75f);
    lightdir.normalise();

    mSunLight = mSceneMgr->createLight("mSunLight");
    mSunLight->setType(Ogre::Light::LT_DIRECTIONAL);
    mSunLight->setDirection(lightdir);
    mSunLight->setDiffuseColour(Ogre::ColourValue::White);
    mSunLight->setSpecularColour(Ogre::ColourValue(0.6f, 0.4f, 0.4f));

    mMoonLight = mSceneMgr->createLight("mMoonLight");
    mMoonLight->setType(Ogre::Light::LT_DIRECTIONAL);
    mMoonLight->setDirection(lightdir);
    mMoonLight->setDiffuseColour(Ogre::ColourValue(0.01f, 0.01f, 0.01f));
    mMoonLight->setSpecularColour(Ogre::ColourValue(0.02f, 0.02f, 0.02f));

    if (!initSky()) return false;

    mWheatherListener = new gkWheatherListener();
    Ogre::Root::getSingleton().addFrameListener(mWheatherListener);

    return true;
}

bool gkWheather::initSky()
{
    // Create SkyX
    mBasicController = new SkyX::BasicController();
    mSkyX = new SkyX::SkyX(mSceneMgr, mBasicController);
    mSkyX->create();

    mSkyX->getVCloudsManager()->getVClouds()->setDistanceFallingParams(Ogre::Vector2(2,-1));

    // Register SkyX listeners
    Ogre::Root::getSingleton().addFrameListener(mSkyX);
    mWindow->addListener(mSkyX);

    //mPresets[mCurrentPreset].vcWheater.x = 0.10253;
    //mPresets[mCurrentPreset].vcWheater.y = 0.10210;
    setPreset(mPresets[mCurrentPreset]);
    //mSkyX->getCloudsManager()->add(SkyX::CloudLayer::Options(/* Default options */));

    return true;
}

void gkWheather::setPreset( const SkyXSettings& preset )
{
    mSkyX->setTimeMultiplier(preset.timeMultiplier);
    mBasicController->setTime(preset.time);
    mBasicController->setMoonPhase(preset.moonPhase);
    mSkyX->getAtmosphereManager()->setOptions(preset.atmosphereOpt);

    // Layered clouds
    if (preset.layeredClouds)
    {
        // Create layer cloud
        if (mSkyX->getCloudsManager()->getCloudLayers().empty())
        {
            mSkyX->getCloudsManager()->add(SkyX::CloudLayer::Options(/* Default options */));
        }
    }
    else
    {
        // Remove layer cloud
        if (!mSkyX->getCloudsManager()->getCloudLayers().empty())
        {
            mSkyX->getCloudsManager()->removeAll();
        }
    }

    mSkyX->getVCloudsManager()->setWindSpeed(preset.vcWindSpeed);
    mSkyX->getVCloudsManager()->setAutoupdate(preset.vcAutoupdate);

    SkyX::VClouds::VClouds* vclouds = mSkyX->getVCloudsManager()->getVClouds();

    vclouds->setWindDirection(preset.vcWindDir);
    vclouds->setAmbientColor(preset.vcAmbientColor);
    vclouds->setLightResponse(preset.vcLightResponse);
    vclouds->setAmbientFactors(preset.vcAmbientFactors);
    vclouds->setWheater(preset.vcWheater.x, preset.vcWheater.y, false);

    if (preset.volumetricClouds)
    {
        // Create VClouds
        if (!mSkyX->getVCloudsManager()->isCreated())
        {
            // SkyX::MeshManager::getSkydomeRadius(...) works for both finite and infinite(=0) camera far clip distances
            mSkyX->getVCloudsManager()->create(mSkyX->getMeshManager()->getSkydomeRadius(mCamera));
        }
    }
    else
    {
        // Remove VClouds
        if (mSkyX->getVCloudsManager()->isCreated())
        {
            mSkyX->getVCloudsManager()->remove();
        }
    }

    vclouds->getLightningManager()->setEnabled(preset.vcLightnings);
    vclouds->getLightningManager()->setAverageLightningApparitionTime(preset.vcLightningsAT);
    vclouds->getLightningManager()->setLightningColor(preset.vcLightningsColor);
    vclouds->getLightningManager()->setLightningTimeMultiplier(preset.vcLightningsTM);

    mSkyX->update(0);
}

void gkWheather::destroy()
{
    if (mWheatherListener)
    {
        Ogre::Root::getSingleton().removeFrameListener(mWheatherListener);
        delete mWheatherListener;
        mWheatherListener = NULL;
    }

    if (mSkyX)
    {
        Ogre::Root::getSingleton().removeFrameListener(mSkyX);
        mWindow->removeListener(mSkyX);
        delete mSkyX;
        mSkyX = NULL;
    }
}

/** Demo presets
    @remarks The best way of determinate each parameter value is by using a real-time editor.
             These presets have been quickly designed using the Paradise Editor, which is a commercial solution.
             At the time I'm writting these lines, SkyX 0.1 is supported by Ogitor. Hope that the Ogitor team will
             support soon SkyX 0.3, this way you all are going to be able to quickly create cool SkyX configurations.
 */
const gkWheather::SkyXSettings gkWheather::mPresets[] = {
    // Sunset
    gkWheather::SkyXSettings(Ogre::Vector3(8.85f, 7.5f, 20.5f),  -0.8f, 0, SkyX::AtmosphereManager::Options(9.77501f, 10.2963f, 0.01f, 0.0022f, 0.000675f, 30, Ogre::Vector3(0.57f, 0.52f, 0.44f), -0.991f, 3, 4), false, true, 300, false, Ogre::Radian(270), Ogre::Vector3(0.63f,0.63f,0.7f), Ogre::Vector4(0.35, 0.2, 0.92, 0.1), Ogre::Vector4(0.4, 0.7, 0, 0), Ogre::Vector2(0.8,1)),
    // Clear
    gkWheather::SkyXSettings(Ogre::Vector3(17.16f, 7.5f, 20.5f), 0.8, 0, SkyX::AtmosphereManager::Options(9.77501f, 10.2963f, 0.01f, 0.0017f, 0.000675f, 30, Ogre::Vector3(0.57f, 0.54f, 0.44f), -0.991f, 2.5f, 4), false),
    // Thunderstorm 1
    gkWheather::SkyXSettings(Ogre::Vector3(12.23, 7.5f, 20.5f),  0.8, 0, SkyX::AtmosphereManager::Options(9.77501f, 10.2963f, 0.01f, 0.00545f, 0.000375f, 30, Ogre::Vector3(0.55f, 0.54f, 0.52f), -0.991f, 1, 4), false, true, 300, false, Ogre::Radian(0), Ogre::Vector3(0.63f,0.63f,0.7f), Ogre::Vector4(0.25, 0.4, 0.5, 0.1), Ogre::Vector4(0.45, 0.3, 0.6, 0.1), Ogre::Vector2(1,1), true, 0.5, Ogre::Vector3(1,0.976,0.92), 2),
    // Thunderstorm 2
    gkWheather::SkyXSettings(Ogre::Vector3(10.23, 7.5f, 20.5f),  0.8, 0, SkyX::AtmosphereManager::Options(9.77501f, 10.2963f, 0.01f, 0.00545f, 0.000375f, 30, Ogre::Vector3(0.55f, 0.54f, 0.52f), -0.991f, 0.5, 4), false, true, 300, false, Ogre::Radian(0), Ogre::Vector3(0.63f,0.63f,0.7f), Ogre::Vector4(0, 0.02, 0.34, 0.24), Ogre::Vector4(0.29, 0.3, 0.6, 1), Ogre::Vector2(1,1), true, 0.5, Ogre::Vector3(0.95,1,1), 2),
    // Desert
    gkWheather::SkyXSettings(Ogre::Vector3(7.59f, 7.5f, 20.5f), 0.8, -0.8f, SkyX::AtmosphereManager::Options(9.77501f, 10.2963f, 0.01f, 0.0072f, 0.000925f, 30, Ogre::Vector3(0.71f, 0.59f, 0.53f), -0.997f, 2.5f, 1), true),
    // Night
    gkWheather::SkyXSettings(Ogre::Vector3(21.5f, 7.5, 20.5), 0.8, -0.25, SkyX::AtmosphereManager::Options(), true)
};


bool gkWheatherListener::frameStarted( const Ogre::FrameEvent& evt )
{
    SkyX::SkyX* skyX = gkWheather::getSingleton().getSkyX();
    //skyX->update(evt.timeSinceLastFrame);
    gkWheather::getSingleton().getMoonLight()->setDirection(
        -skyX->getController()->getMoonDirection());
    gkWheather::getSingleton().getSunLight()->setDirection(
        -skyX->getController()->getSunDirection());
    return true;
}

UT_IMPLEMENT_SINGLETON(gkWheather);
