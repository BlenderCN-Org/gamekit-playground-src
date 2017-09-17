 #ifndef __GK_WHEATHER_H__
#define __GK_WHEATHER_H__

#include "utSingleton.h"

class gkWheatherListener
    : public Ogre::FrameListener
{
public:
    virtual bool frameStarted( const Ogre::FrameEvent& evt );
};

class gkWheather :
    public utSingleton<gkWheather>
{
public:
    /** SkyX settings struct
    @remarks These just are the most important SkyX parameters, not all SkyX parameters.
    */
    typedef struct SkyXSettings
    {
        /** Constructor
        @remarks Skydome + vol. clouds + lightning settings
        */
        SkyXSettings(const Ogre::Vector3 t, const Ogre::Real& tm, const Ogre::Real& mp, const SkyX::AtmosphereManager::Options& atmOpt,
            const bool& lc, const bool& vc, const Ogre::Real& vcws, const bool& vcauto, const Ogre::Radian& vcwd,
            const Ogre::Vector3& vcac, const Ogre::Vector4& vclr,  const Ogre::Vector4& vcaf, const Ogre::Vector2& vcw,
            const bool& vcl, const Ogre::Real& vclat, const Ogre::Vector3& vclc, const Ogre::Real& vcltm)
            : time(t), timeMultiplier(tm), moonPhase(mp), atmosphereOpt(atmOpt), layeredClouds(lc), volumetricClouds(vc), vcWindSpeed(vcws)
            , vcAutoupdate(vcauto), vcWindDir(vcwd), vcAmbientColor(vcac), vcLightResponse(vclr), vcAmbientFactors(vcaf), vcWheater(vcw)
            , vcLightnings(vcl), vcLightningsAT(vclat), vcLightningsColor(vclc), vcLightningsTM(vcltm)
        {}

        /** Constructor
        @remarks Skydome + vol. clouds
        */
        SkyXSettings(const Ogre::Vector3 t, const Ogre::Real& tm, const Ogre::Real& mp, const SkyX::AtmosphereManager::Options& atmOpt,
            const bool& lc, const bool& vc, const Ogre::Real& vcws, const bool& vcauto, const Ogre::Radian& vcwd,
            const Ogre::Vector3& vcac, const Ogre::Vector4& vclr,  const Ogre::Vector4& vcaf, const Ogre::Vector2& vcw)
            : time(t), timeMultiplier(tm), moonPhase(mp), atmosphereOpt(atmOpt), layeredClouds(lc), volumetricClouds(vc), vcWindSpeed(vcws)
            , vcAutoupdate(vcauto), vcWindDir(vcwd), vcAmbientColor(vcac), vcLightResponse(vclr), vcAmbientFactors(vcaf), vcWheater(vcw), vcLightnings(false)
        {}

        /** Constructor
        @remarks Skydome settings
        */
        SkyXSettings(const Ogre::Vector3 t, const Ogre::Real& tm, const Ogre::Real& mp, const SkyX::AtmosphereManager::Options& atmOpt, const bool& lc)
            : time(t), timeMultiplier(tm), moonPhase(mp), atmosphereOpt(atmOpt), layeredClouds(lc), volumetricClouds(false), vcLightnings(false)
        {}

        /// Time
        Ogre::Vector3 time;
        /// Time multiplier
        Ogre::Real timeMultiplier;
        /// Moon phase
        Ogre::Real moonPhase;
        /// Atmosphere options
        SkyX::AtmosphereManager::Options atmosphereOpt;
        /// Layered clouds?
        bool layeredClouds;
        /// Volumetric clouds?
        bool volumetricClouds;
        /// VClouds wind speed
        Ogre::Real vcWindSpeed;
        /// VClouds autoupdate
        bool vcAutoupdate;
        /// VClouds wind direction
        Ogre::Radian vcWindDir;
        /// VClouds ambient color
        Ogre::Vector3 vcAmbientColor;
        /// VClouds light response
        Ogre::Vector4 vcLightResponse;
        /// VClouds ambient factors
        Ogre::Vector4 vcAmbientFactors;
        /// VClouds wheater
        Ogre::Vector2 vcWheater;
        /// VClouds lightnings?
        bool vcLightnings;
        /// VClouds lightnings average aparition time
        Ogre::Real vcLightningsAT;
        /// VClouds lightnings color
        Ogre::Vector3 vcLightningsColor;
        /// VClouds lightnings time multiplier
        Ogre::Real vcLightningsTM;
    }SkyXSettings;

public:
    /** Demo presets
    @remarks The best way of determinate each parameter value is by using a real-time editor.
    These presets have been quickly designed using the Paradise Editor, which is a commercial solution.
    At the time I'm writting these lines, SkyX 0.1 is supported by Ogitor. Hope that the Ogitor team will
    support soon SkyX 0.2, this way you all are going to be able to quickly create cool SkyX configurations.
    */
    static const SkyXSettings mPresets[];

    static const Ogre::uint32 mCurrentPreset = 1;

private:
    gkWheatherListener* mWheatherListener;
    SkyX::SkyX* mSkyX;
    SkyX::BasicController* mBasicController;
    Ogre::Light* mSunLight;
    Ogre::Light* mMoonLight;

    Ogre::SceneManager* mSceneMgr;
    Ogre::RenderWindow* mWindow;
    Ogre::Camera* mCamera;

public:
    Ogre::Light* getSunLight() const { return mSunLight; }
    Ogre::Light* getMoonLight() const { return mMoonLight; }
    SkyX::SkyX* getSkyX() const
    {
        return mSkyX;
    }

    UT_DECLARE_SINGLETON(gkWheather);

public:
    gkWheather(void);
    virtual ~gkWheather(void);
    virtual bool initialize(Ogre::SceneManager* _sceneMgr, Ogre::RenderWindow* _window, Ogre::Camera* _camera);
    virtual void destroy();
    virtual bool initSky();
    virtual void setPreset(const SkyXSettings& preset);
};

#endif // __GK_WHEATHER_H__
