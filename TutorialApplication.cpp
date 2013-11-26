/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
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
#include "TutorialApplication.h"

#include <OgreManualObject.h>
#include <OgreRay.h>
#include <OgreSceneQuery.h>

using namespace Ogre;

//-------------------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
    : m_activeLevel(Vector3::UNIT_Y, 0)
{
}
//-------------------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void)
{
}

void TutorialApplication::chooseSceneManager()
{
    m_SceneMgr = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE);
}

void TutorialApplication::createFrameListener()
{
    BaseApplication::createFrameListener();
    m_rayScnQuery = m_SceneMgr->createRayQuery(Ogre::Ray());
}

//-------------------------------------------------------------------------------------
void TutorialApplication::createScene(void)
{
    m_SceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    // Render the bottom grid using lines
    Ogre::ManualObject *man = m_SceneMgr->createManualObject("grid");
    man->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
    for (Real i = -GRID_SIZE + GRID_SPACING; i < GRID_SIZE; i += GRID_SPACING) {
        man->position(i, 0, -GRID_SIZE);
        man->position(i, 0, GRID_SIZE);

        man->position(-GRID_SIZE, 0, i);
        man->position(GRID_SIZE, 0, i);
    }
    man->end();
    m_SceneMgr->getRootSceneNode()->attachObject(man);

    // Create the cursor plane
    ManualObject *plane = m_SceneMgr->createManualObject("basePlane");
    plane->begin("BaseWhiteNoLighting", RenderOperation::OT_TRIANGLE_STRIP);
    plane->position(0, 0, 0);
    plane->position(0, 0, CURSOR_SIZE);
    plane->position(CURSOR_SIZE, 0, 0);
    plane->position(CURSOR_SIZE, 0, CURSOR_SIZE);
    plane->end();
    m_cursorNode = m_SceneMgr->getRootSceneNode()->createChildSceneNode("cursorNode");
    m_cursorNode->attachObject(plane);
}

void TutorialApplication::createCamera()
{
    BaseApplication::createCamera();
    mCameraMan->setStyle(OgreBites::CS_ORBIT);
}

bool TutorialApplication::mouseMoved(const OIS::MouseEvent &arg)
{
    BaseApplication::mouseMoved(arg);
    std::cout << "moust at ("
              << arg.state.X.abs << ","
              << arg.state.Y.abs << ","
              << arg.state.Z.abs << ")" << std::endl;
    std::cout << "plane at Y = " << m_activeLevel.d << std::endl;
    Viewport *vp = m_SceneMgr->getCurrentViewport();
    Ray mouseRay = mCamera->getCameraToViewportRay(
                (Real) arg.state.X.abs / vp->getActualWidth(),
                (Real) arg.state.Y.abs / vp->getActualHeight());

    if (mKeyboard->isModifierDown(OIS::Keyboard::Shift)) {
        m_Ydelta += arg.state.Y.rel;
        Real Yblocks = floor((m_Ystart + m_Ydelta) / GRID_SPACING) * GRID_SPACING;
        m_activeLevel.d = Yblocks;
        const Vector3 p = m_cursorNode->getPosition();
        m_cursorNode->setPosition(Vector3(p.x, p.y, p.z));
    } else {
        m_Ydelta = 0;
        m_Ystart = arg.state.Y.abs;
//        m_PrevPlane = m_activeLevel;
    auto r = mouseRay.intersects(m_activeLevel);
    if (r.first) {
        auto pos = mouseRay.getPoint(r.second);
        Vector3 gridPos = Vector3(floor(pos.x / GRID_SPACING) * GRID_SPACING,
                                  pos.y,
                                  floor(pos.z / GRID_SPACING) * GRID_SPACING);
        m_cursorNode->setPosition(gridPos);

    }
    }

//    m_rayScnQuery->setRay(mouseRay);
//    RaySceneQueryResult result = m_rayScnQuery->execute();

//    for (RaySceneQueryResultEntry &r : result) {
////        std::cout << "hit object '" << r.movable->getName() << "' at point !" << std::endl;
//        Vector3 pos = mouseRay.getPoint(r.distance);
//        if (r.movable->getName() == "grid") {
//            m_cursorNode->setPosition(
//                        Vector3(floor(pos.x / GRID_SPACING) * GRID_SPACING,
//                                pos.y,
//                                floor(pos.z / GRID_SPACING) * GRID_SPACING));
//        }
//        r.movable->setVisible(!r.movable->getVisible());
//    }
}



#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        TutorialApplication app;

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
