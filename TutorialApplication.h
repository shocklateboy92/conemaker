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

class TutorialApplication : public BaseApplication
{
public:
    static const constexpr Ogre::Real GRID_SIZE = 100.0f;
    static const constexpr Ogre::Real GRID_SPACING = 10.0f;
    static const constexpr Ogre::Real CURSOR_SIZE = GRID_SPACING;

    TutorialApplication(void);
    virtual ~TutorialApplication(void);

protected:
    virtual void chooseSceneManager() override;
    virtual void createFrameListener() override;

    virtual void createCamera(void) override;
    virtual void createScene(void);

    virtual bool keyPressed(const OIS::KeyEvent &arg) override;
    virtual bool keyReleased(const OIS::KeyEvent &arg) override;
    virtual bool mouseMoved(const OIS::MouseEvent &arg) override;

private:
    Ogre::Ray getMouseRay(void);

    Ogre::Real m_Ydelta;
    Ogre::Real m_Ystart;
    Ogre::SceneNode *m_cursorNode;
    Ogre::RaySceneQuery *m_rayScnQuery;
    Ogre::Plane m_activeLevel;

    Ogre::Plane m_verticalLevel;
    bool m_verticalMode;
};

#endif // #ifndef __TutorialApplication_h_
