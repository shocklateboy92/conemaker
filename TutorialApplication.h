/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.h
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#ifndef __TutorialApplication_h_
#define __TutorialApplication_h_

#include "BaseApplication.h"
#include <vector>

class TutorialApplication : public BaseApplication
{
public:
    static const constexpr Ogre::Real GRID_SIZE = 100.0f;
    static const constexpr Ogre::Real GRID_SPACING = 10.0f;
    static const constexpr Ogre::Real CURSOR_SIZE = GRID_SPACING;
    static const constexpr Ogre::Real CONE_SIZE = 60.0f;

    static const constexpr auto BASE_MATERIAL = "BaseWhiteNoLighting";
    static const std::vector<Ogre::Vector3> CONE_CASES;

    TutorialApplication(void);
    virtual ~TutorialApplication(void);

    enum Mode {
        NoneMode = 0,
        TrollMode,
        PartyMode,
        WitchMode
    };

protected:
    virtual void chooseSceneManager() override;
    virtual void createFrameListener() override;

    virtual void createCamera(void) override;
    virtual void createScene(void);

    virtual bool keyPressed(const OIS::KeyEvent &arg) override;
    virtual bool keyReleased(const OIS::KeyEvent &arg) override;
    virtual bool mouseMoved(const OIS::MouseEvent &arg) override;
    virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id) override;

private:
    Ogre::Ray getMouseRay(void);
    void createCones(Ogre::ManualObject *obj, std::vector<Ogre::Vector3> &seen, Ogre::SceneNode *parentNode, Ogre::Vector3 pos, Ogre::Vector3 dir);

    Ogre::SceneNode *m_cursorNode;
    Ogre::Plane m_activeLevel;

    bool m_verticalMode;
    Mode m_mode;

    std::vector<Ogre::Vector3> m_ogres;
    Ogre::SceneNode *m_pointNode;
    std::vector<Ogre::SceneNode*> m_coneNodes;
};

#endif // #ifndef __TutorialApplication_h_
