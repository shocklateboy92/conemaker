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
    : m_activeLevel(Vector3::UNIT_Y, 0),
      m_verticalMode(false),
      m_pointNode(nullptr)
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
//    m_rayScnQuery = m_SceneMgr->createRayQuery(Ogre::Ray());
}

//-------------------------------------------------------------------------------------

void scaleMesh(const Ogre::Entity *_ent, const Ogre::Vector3 &_scale)
{
    bool added_shared = false;
    Ogre::Mesh* mesh = _ent->getMesh().getPointer();
    Ogre::Vector3 Minimum=mesh->getBounds().getMaximum();
    Ogre::Vector3 Maximum=mesh->getBounds().getMinimum();
    // Run through the submeshes, modifying the data
    for(int i = 0;i < mesh->getNumSubMeshes();i++)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);
        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
        if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
        {
            if(submesh->useSharedVertices)
            {
                added_shared = true;
            }
            const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
            Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
            // lock buffer for read and write access
            unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_NORMAL));
            Ogre::Real* pReal;
            for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);
                // modify x coord
                (*pReal) *= _scale.x;
                ++pReal;
                // modify y coord
                (*pReal) *= _scale.y;
                ++pReal;
                // modify z coord
                (*pReal) *= _scale.z;
                pReal-=2;
                Minimum.x=Minimum.x<(*pReal)?Minimum.x:(*pReal);
                Maximum.x=Maximum.x>(*pReal)?Maximum.x:(*pReal);
                ++pReal;
                Minimum.y=Minimum.y<(*pReal)?Minimum.y:(*pReal);
                Maximum.y=Maximum.y>(*pReal)?Maximum.y:(*pReal);
                ++pReal;
                Minimum.z=Minimum.z<(*pReal)?Minimum.z:(*pReal);
                Maximum.z=Maximum.z>(*pReal)?Maximum.z:(*pReal);
            }
            vbuf->unlock();
        }
    }
    mesh->_setBounds(Ogre::AxisAlignedBox(Minimum,Maximum),false);
}

const std::vector<Vector3> make_cases() {
    std::vector<Vector3> v;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            for (int z = -1; z <= 1; z++) {
                v.push_back(Vector3(x, y, z) *
                            TutorialApplication::GRID_SPACING);
            }
        }
    }

    // remove the {0, 0, 0} case
    v.erase(std::find(v.begin(), v.end(), Vector3(0, 0, 0)));
    return v;
}

const std::vector<Vector3> TutorialApplication::CONE_CASES = make_cases();

inline Real distance(Real x, Real y) {
    return abs(x - y) + floor(std::min(x, y) * 1.5 / TutorialApplication::GRID_SPACING) * TutorialApplication::GRID_SPACING;
}

inline Real distance3(Real _x, Real _y, Real _z) {
    Real x(abs(_x)), y(abs(_y)), z(abs(_z));
    Real c = std::min({x,y,z});
    Real d2;
    if (x == c) {
        d2 = distance(y-c, z-c);
    } else if (y == c) {
        d2 = distance(x-c, z-c);
    } else if (z == c) {
        d2 = distance(x-c, y-c);
    } else {
        assert (false && "WTF MATH?!");
    }

    return d2 + floor(c * 1.75 / TutorialApplication::GRID_SPACING) * TutorialApplication::GRID_SPACING;
}

void TutorialApplication::createCones(ManualObject *obj, std::vector<Vector3> &seen, SceneNode *parentNode, Vector3 pos, Vector3 dir)
{
    if (std::find(seen.begin(), seen.end(), pos) != seen.end()) {
        // we've already drawn this point
        return;
    }
    seen.push_back(pos);

    for (Vector3 c : CONE_CASES) {
        if (c.angleBetween(dir) <= Degree(45)) {
            Vector3 pNext = pos + c;
            Real distance = distance3(pNext.x, pNext.y, pNext.z);
            if (distance <= CONE_SIZE) {
                createCones(obj, seen, parentNode, pNext, dir);
            }
        }
    }

    Entity *cube = m_SceneMgr->createEntity(SceneManager::PT_CUBE);
    cube->setMaterialName("Template/Red50");
    SceneNode *node = parentNode->createChildSceneNode();
    node->attachObject(cube);
    Vector3 offset(pos.x / -abs(pos.x) * GRID_SPACING / 2,
                   pos.y / -abs(pos.y) * GRID_SPACING / 2,
                   pos.z / -abs(pos.z) * GRID_SPACING / 2);
    node->setPosition(pos - offset);
    node->scale(0.1, 0.1, 0.1);
}

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

    // Create the cursor point
    m_pointNode = m_SceneMgr->getRootSceneNode()->createChildSceneNode("coneBase");
    Entity *circle = m_SceneMgr->createEntity(SceneManager::PT_SPHERE);
    scaleMesh(circle, {0.05f, 0.05f, 0.05f});
    circle->setMaterialName(BASE_MATERIAL);
    m_pointNode->attachObject(circle);
//    m_pointNode->scale(0.05f, 0.05f, 0.05f);

    // Create all possible cones, so they can be shown later
    for (Vector3 coneDir : CONE_CASES) {
        std::vector<Vector3> seenList;
        SceneNode *childNode = m_pointNode->createChildSceneNode();
        m_coneNodes.push_back(childNode);
        createCones(nullptr, seenList, childNode,
                    m_pointNode->getPosition(), coneDir);
    }
    assert (m_coneNodes.size() == CONE_CASES.size());
    m_pointNode->setVisible(false, true);
}

void TutorialApplication::createCamera()
{
    BaseApplication::createCamera();
    mCameraMan->setStyle(OgreBites::CS_ORBIT);
}

Ray TutorialApplication::getMouseRay() {
    const OIS::MouseState s = mMouse->getMouseState();
    Viewport *vp = m_SceneMgr->getCurrentViewport();

    return mCamera->getCameraToViewportRay(
                (Real) s.X.abs / vp->getActualWidth(),
                (Real) s.Y.abs / vp->getActualHeight());
}

static std::size_t prevCone = 0;
bool TutorialApplication::keyPressed(const OIS::KeyEvent &arg)
{
    switch (arg.key) {
    case OIS::KC_LSHIFT:
    case OIS::KC_RSHIFT:
        std::cout << "Entering vertical mode" << std::endl;
        m_verticalMode = true;
        break;
    case OIS::KC_1:
        m_mode = NoneMode;
        m_cursorNode->setVisible(true);
        m_pointNode->setVisible(false);
        break;
    case OIS::KC_2:
        m_mode = TrollMode;
        m_cursorNode->setVisible(true);
        m_pointNode->setVisible(false);
        break;
    case OIS::KC_3:
        m_mode = WitchMode;
        m_cursorNode->setVisible(false);
        m_pointNode->setVisible(true, false);
        break;
    case OIS::KC_I:
        m_coneNodes[prevCone]->setVisible(false, true);
        prevCone++;
        if (prevCone == CONE_CASES.size()) {
            prevCone = 0;
        }
        std::cout << "setting " << m_coneNodes[prevCone] << " visible" << std::endl;
        m_coneNodes[prevCone]->setVisible(true, true);
        break;
    }

    return BaseApplication::keyPressed(arg);
}

bool TutorialApplication::keyReleased(const OIS::KeyEvent &arg)
{
    if (arg.key == OIS::KC_LSHIFT || arg.key == OIS::KC_RSHIFT) {
        std::cout << "Exiting vertical mode" << std::endl;
        m_verticalMode = false;
    }
    return BaseApplication::keyReleased(arg);
}

bool TutorialApplication::mouseMoved(const OIS::MouseEvent &arg)
{
    auto ret = BaseApplication::mouseMoved(arg);
//    std::cout << "moust at ("
//              << arg.state.X.abs << ","
//              << arg.state.Y.abs << ","
//              << arg.state.Z.abs << ")" << std::endl;
//    std::cout << "plane at Y = " << m_activeLevel.d << std::endl;

    Ray mouseRay = getMouseRay();

    if (m_verticalMode) {
        // ignored for now, untill I figure out how to make it intuitive
    } else {
        auto r = mouseRay.intersects(m_activeLevel);
        if (r.first) {
            auto pos = mouseRay.getPoint(r.second);
            auto gridPos =
                    Vector3(floor(pos.x / GRID_SPACING) * GRID_SPACING,
                            pos.y,
                            floor(pos.z / GRID_SPACING) * GRID_SPACING);
            m_cursorNode->setPosition(gridPos);
            auto pointPos =
                    Vector3(round(pos.x / GRID_SPACING) * GRID_SPACING,
                            pos.y,
                            round(pos.z / GRID_SPACING) * GRID_SPACING);
            m_pointNode->setPosition(pointPos);

            if (m_mode == WitchMode) {
                for (std::size_t i = 0; i < CONE_CASES.size(); i++) {
                    auto c = CONE_CASES[i];
                    bool containsCreatures = true;
                    for (Vector3 creature : m_ogres) {
                        Vector3 dir = creature - pointPos;

                        // cone is facing wrong way for creature
                        if (dir.angleBetween(c) > Degree(45)) {
                            containsCreatures = false;
                            break;
                        }

                        // creature too far away for cone
                        if (distance3(dir.x, dir.y, dir.z) > CONE_SIZE) {
                            containsCreatures = false;
                        }
                    }

                    if (containsCreatures) {
                        m_coneNodes[i]->setVisible(true);
                    } else {
                        m_coneNodes[i]->setVisible(false);
                    }
                }
            }
        }
    }
    return ret;
}

bool TutorialApplication::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    if (m_mode == TrollMode) {
        const Vector3 p = m_cursorNode->getPosition();
        m_ogres.push_back(p);

        Ogre::Entity *troll = m_SceneMgr->createEntity("ogrehead.mesh");
        Vector3 bounds = troll->getBoundingBox().getSize();
        Real dim = std::max({bounds.x, bounds.y, bounds.z});
        Real scale = GRID_SPACING / dim;
        bounds = bounds * scale * 0.5f;

        Ogre::SceneNode *node = m_SceneMgr->getRootSceneNode()->createChildSceneNode();
        node->setPosition(p.x + bounds.x, p.y + bounds.y, p.z + bounds.z);
//        node->showBoundingBox(true);
        node->scale(scale, scale, scale);
        node->attachObject(troll);
    }

    return BaseApplication::mouseReleased(arg, id);
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
