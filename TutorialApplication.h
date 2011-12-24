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
#include "kinect_class.h"

class TutorialApplication : public BaseApplication
{
public:
    TutorialApplication(void);
    virtual ~TutorialApplication(void);

protected:
	kinect_class *objeto_kinect;
	OgreBites::ParamsPanel* mKinectPanel;     // sample details panel
	bool kinectState;
    virtual void createScene(void);
	virtual void createFrameListener(void);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	// OIS::KeyListener
    virtual bool keyPressed( const OIS::KeyEvent& evt );
    virtual bool keyReleased( const OIS::KeyEvent& evt );
    // OIS::MouseListener
    virtual bool mouseMoved( const OIS::MouseEvent& evt );
    virtual bool mousePressed( const OIS::MouseEvent& evt, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent& evt, OIS::MouseButtonID id );
private:
	// Personaje
	Ogre::SceneNode* nodoPersonaje;
	Ogre::Entity* entidadPersonaje;
	Ogre::Entity* mSword1;
	Ogre::Entity* mSword2;
	Ogre::RibbonTrail* mSwordTrail;
	//std::map<Ogre::String, Ogre::AnimationState*> animaciones;
	Ogre::AnimationState* mAnims[13];    // master animation list
	Ogre::AnimationState* animArriba;
	Ogre::AnimationState* animAbajo;
	Ogre::Vector3 direccionPersonaje;
	void setupBone(const Ogre::String& name,const Ogre::Degree& yaw,const Ogre::Degree& pitch,const Ogre::Degree& roll);
	void setupBone(const Ogre::String& name,const Ogre::Quaternion& q);
	bool processUnbufferedInput(const Ogre::FrameEvent& evt);
	void setupBody();
	void setupAnimations();
	void transformBone(const Ogre::String& modelBoneName, XnSkeletonJoint skelJoint, bool flip=false);
	void PSupdateBody();
};

#endif // #ifndef __TutorialApplication_h_
