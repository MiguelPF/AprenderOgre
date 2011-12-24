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
#define NUM_ANIMS 13

//-------------------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
	{
	}
//-------------------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void)
	{
	}

//-------------------------------------------------------------------------------------
void TutorialApplication::createScene(void)
	{
	// Set ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.25, 0.25, 0.25));

	setupBody();
	// Cargamos las animaciones del personaje
	setupAnimations();

	// Create a light
	Ogre::Light* pointLight = mSceneMgr->createLight("pointLight");
	pointLight->setType(Ogre::Light::LT_POINT);
	pointLight->setPosition(Ogre::Vector3(250, 150, 250));
	pointLight->setDiffuseColour(Ogre::ColourValue::White);
	pointLight->setSpecularColour(Ogre::ColourValue::White);

	//create our character controller
	kinectState=false;
	}
// ------------------------------------------------------------------------------------

void TutorialApplication::setupBody()
	{
	// Creamos al personaje
	entidadPersonaje = mSceneMgr->createEntity("entidadPersonaje", "Sinbad.mesh");
	entidadPersonaje->setCastShadows(true);
	nodoPersonaje = mSceneMgr->getRootSceneNode()->createChildSceneNode("nodoPersonaje");
	nodoPersonaje->attachObject(entidadPersonaje);
	nodoPersonaje->setPosition(0, 5.75, 0);

	// create swords and attach to sheath
	mSword1 = mSceneMgr->createEntity("SinbadSword1", "Sword.mesh");
	mSword2 = mSceneMgr->createEntity("SinbadSword2", "Sword.mesh");
	entidadPersonaje->attachObjectToBone("Sheath.L", mSword1);
	entidadPersonaje->attachObjectToBone("Sheath.R", mSword2);

	// create a couple of ribbon trails for the swords, just for fun
	Ogre::NameValuePairList params;
	params["numberOfChains"] = "2";
	params["maxElements"] = "80";
	mSwordTrail = (Ogre::RibbonTrail*)mSceneMgr->createMovableObject("RibbonTrail", &params);
	mSwordTrail->setMaterialName("Examples/LightRibbonTrail");
	mSwordTrail->setTrailLength(20);
	mSwordTrail->setVisible(false);
	mSceneMgr->getRootSceneNode()->attachObject(mSwordTrail);

	for (int i = 0; i < 2; i++)
		{
		mSwordTrail->setInitialColour(i, 1, 0.8, 0);
		mSwordTrail->setColourChange(i, 0.75, 1.25, 1.25, 1.25);
		mSwordTrail->setWidthChange(i, 1);
		mSwordTrail->setInitialWidth(i, 0.5);
		}

	}

// -------------------------------------------------------------------------------
void TutorialApplication::setupAnimations()
	{
	// this is very important due to the nature of the exported animations
	//entidadPersonaje->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);	
	Ogre::String animNames[] =
		{"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
		"SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};

	//set all to manualy controlled
	Ogre::Quaternion q = Ogre::Quaternion::IDENTITY;
	Ogre::Quaternion q2,q3;
	Ogre::Vector3 xAxis,yAxis,zAxis;
	q.FromAngleAxis(Ogre::Degree(90),Ogre::Vector3(0,0,-1));
	q.ToAxes(xAxis,yAxis,zAxis);
	q2.FromAngleAxis(Ogre::Degree(90),xAxis);
	setupBone("Humerus.L",q*q2);
	q.FromAngleAxis(Ogre::Degree(90),Ogre::Vector3(0,0,1));
	q.ToAxes(xAxis,yAxis,zAxis);
	q2.FromAngleAxis(Ogre::Degree(90),xAxis);
	setupBone("Humerus.R",q*q2);

	q.FromAngleAxis(Ogre::Degree(90),Ogre::Vector3(0,0,-1));	 
	q2.FromAngleAxis(Ogre::Degree(45),Ogre::Vector3(0,-1,0));

	setupBone("Ulna.L",q*q2);

	q.FromAngleAxis(Ogre::Degree(90),Ogre::Vector3(0,0,1));	 	
	setupBone("Ulna.R",q*q2.Inverse());

	q.FromAngleAxis(Ogre::Degree(180),Ogre::Vector3(0,1,0));
	setupBone("Chest",q);
	setupBone("Stomach",q);
	q.FromAngleAxis(Ogre::Degree(180),Ogre::Vector3(1,0,0));	 	
	q2.FromAngleAxis(Ogre::Degree(180),Ogre::Vector3(0,1,0));
	setupBone("Thigh.L",q*q2);
	setupBone("Thigh.R",q*q2);
	setupBone("Calf.L",q*q2);
	setupBone("Calf.R",q*q2);
	setupBone("Root",Ogre::Degree(0),Ogre::Degree(0),Ogre::Degree(0));

	//rotate the body
	//mBodyNode->yaw(Degree(0),Node::TransformSpace::TS_WORLD);

	Ogre::Skeleton* skel = entidadPersonaje->getSkeleton();

	Ogre::Skeleton::BoneIterator bi = skel->getBoneIterator();

	// populate our animation list
	for (int i = 0; i < NUM_ANIMS; i++)
		{
		mAnims[i] = entidadPersonaje->getAnimationState(animNames[i]);
		mAnims[i]->setLoop(true);
		//mFadingIn[i] = false;
		//mFadingOut[i] = false;

		// disable animation updates
		Ogre::Animation* anim = entidadPersonaje->getSkeleton()->getAnimation(animNames[i]);

		anim->destroyNodeTrack(entidadPersonaje->getSkeleton()->getBone("Ulna.L")->getHandle());
		anim->destroyNodeTrack(entidadPersonaje->getSkeleton()->getBone("Ulna.R")->getHandle());
		anim->destroyNodeTrack(entidadPersonaje->getSkeleton()->getBone("Humerus.L")->getHandle());
		anim->destroyNodeTrack(entidadPersonaje->getSkeleton()->getBone("Humerus.R")->getHandle());
		anim->destroyNodeTrack(entidadPersonaje->getSkeleton()->getBone("Stomach")->getHandle());
		anim->destroyNodeTrack(entidadPersonaje->getSkeleton()->getBone("Chest")->getHandle());
		anim->destroyNodeTrack(entidadPersonaje->getSkeleton()->getBone("Thigh.L")->getHandle());
		anim->destroyNodeTrack(entidadPersonaje->getSkeleton()->getBone("Thigh.R")->getHandle());
		anim->destroyNodeTrack(entidadPersonaje->getSkeleton()->getBone("Calf.L")->getHandle());
		anim->destroyNodeTrack(entidadPersonaje->getSkeleton()->getBone("Calf.R")->getHandle());
		anim->destroyNodeTrack(entidadPersonaje->getSkeleton()->getBone("Root")->getHandle());
		}

	// start off in the idle state (top and bottom together)
	//setBaseAnimation(ANIM_IDLE_BASE);
	//setTopAnimation(ANIM_IDLE_TOP);

	// relax the hands since we're not holding anything
	//mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
	//mSwordsDrawn = false;
	}
// -------------------------------------------------------------------------------

void TutorialApplication::setupBone(const Ogre::String& name,const Ogre::Quaternion& q)
	{
	Ogre::Bone* bone = entidadPersonaje->getSkeleton()->getBone(name);
	bone->setManuallyControlled(true);
	bone->setInheritOrientation(false);

	bone->resetOrientation();
	bone->setOrientation(q);

	bone->setInitialState();
	}
// -------------------------------------------------------------------------------

void TutorialApplication::setupBone(const Ogre::String& name,const Ogre::Degree& yaw,const Ogre::Degree& pitch,const Ogre::Degree& roll)
	{
	Ogre::Bone* bone = entidadPersonaje->getSkeleton()->getBone(name);
	bone->setManuallyControlled(true);
	bone->setInheritOrientation(false);

	bone->resetOrientation();

	bone->yaw(yaw);
	bone->pitch(pitch);
	bone->roll(roll);

	bone->setInitialState();

	}
// -----------------------------------------------------------

void TutorialApplication::transformBone(const Ogre::String& modelBoneName, XnSkeletonJoint skelJoint, bool flip)
	{
	// Get the model skeleton bone info
	Ogre::Skeleton* skel = entidadPersonaje->getSkeleton();
	Ogre::Bone* bone = skel->getBone(modelBoneName);
	Ogre::Quaternion qI = bone->getInitialOrientation();
	Ogre::Quaternion newQ = Ogre::Quaternion::IDENTITY;

	// Get the openNI bone info
	xn::SkeletonCapability pUserSkel = objeto_kinect->g_UserGenerator.GetSkeletonCap();		
	XnSkeletonJointOrientation jointOri;
	pUserSkel.GetSkeletonJointOrientation(objeto_kinect->UserId, skelJoint, jointOri);

	static float deg = 0;
	if(jointOri.fConfidence == 1 )
		{
		Ogre::Matrix3 matOri(jointOri.orientation.elements[0],-jointOri.orientation.elements[1],jointOri.orientation.elements[2],
			-jointOri.orientation.elements[3],jointOri.orientation.elements[4],-jointOri.orientation.elements[5],
			jointOri.orientation.elements[6],-jointOri.orientation.elements[7],jointOri.orientation.elements[8]);
		Ogre::Quaternion q;
		newQ.FromRotationMatrix(matOri);
		bone->resetOrientation(); //in order for the conversion from world to local to work.
		newQ = bone->convertWorldToLocalOrientation(newQ);
		bone->setOrientation(newQ*qI);			
		} 
	}


// ------------------------------------------------
void TutorialApplication::PSupdateBody()
	{
	static bool bNewUser = true;
	static bool bRightAfterSwardsPositionChanged = false;
	static Ogre::Vector3 origTorsoPos;

	xn::SkeletonCapability pUserSkel = objeto_kinect->g_UserGenerator.GetSkeletonCap();

	//set smoothing according to the players request.
	/*
	if(m_SmoothingDelta!=0)
	{
	m_SmoothingFactor += 0.01 * m_SmoothingDelta;
	if(m_SmoothingFactor >= 1)
	m_SmoothingFactor = 0.99;
	if(m_SmoothingFactor <= 0)
	m_SmoothingFactor = 0.00;
	pUserSkel.SetSmoothing(m_SmoothingFactor);
	Ogre::DisplayString blah = "H/N ";
	blah.append(Ogre::StringConverter::toString((Real)m_SmoothingFactor));
	//m_help->setParamValue("Smoothing", blah);
	} */

	// check for start/end pose
	/*
	if (IN_POSE_FOR_LONG_TIME == m_pEndPoseDetector->checkPoseDuration())
	{
	m_UserGenerator.GetSkeletonCap().StopTracking(m_candidateID);
	m_UserGenerator.GetPoseDetectionCap().StartPoseDetection("Psi", m_candidateID);
	m_candidateID = 0;
	resetBonesToInitialState();
	m_pEndPoseDetector->Reset();
	m_pStartPoseDetector->Reset();

	// end pose OK - re-apply the quit slider
	// but make sure we're not in a session already -- nuke the session generator
	m_pSessionManager->EndSession();
	m_pQuitFlow->SetActive(m_pQuitSSlider);

	suppress = false;
	bNewUser = true;

	return;
	} */

	// We dont care about the result of this, it is a simple progress-keeping mechanism
	//m_pStartPoseDetector->checkPoseDuration();

	Ogre::Skeleton* skel = entidadPersonaje->getSkeleton();
	Ogre::Bone* rootBone = skel->getBone("Root");
	XnSkeletonJointPosition torsoPos=objeto_kinect->joint9_torso.position;

	if (objeto_kinect->g_bIstracking)
		{
		if(bNewUser)
			{			
			//pUserSkel.GetSkeletonJointPosition(objeto_kinect->UserId, XN_SKEL_TORSO, torsoPos);
			if(torsoPos.fConfidence > 0.5)
				{
				origTorsoPos.x = -torsoPos.position.X;
				origTorsoPos.y = torsoPos.position.Y;
				origTorsoPos.z = -torsoPos.position.Z;
				bNewUser = false;
				} 
			}
		transformBone("Stomach",XN_SKEL_TORSO, true); //por ahora el flip no funciona
		transformBone("Waist", XN_SKEL_WAIST);
		transformBone("Root", XN_SKEL_WAIST);
		transformBone("Chest",XN_SKEL_TORSO, true);
		transformBone("Humerus.L",XN_SKEL_LEFT_SHOULDER);
		transformBone("Humerus.R",XN_SKEL_RIGHT_SHOULDER);
		transformBone("Ulna.L",XN_SKEL_LEFT_ELBOW);
		transformBone("Ulna.R",XN_SKEL_RIGHT_ELBOW);
		transformBone("Thigh.L",XN_SKEL_LEFT_HIP);
		transformBone("Thigh.R",XN_SKEL_RIGHT_HIP);
		transformBone("Calf.L",XN_SKEL_LEFT_KNEE);
		transformBone("Calf.R",XN_SKEL_RIGHT_KNEE);

		if(!bNewUser)
			{			 
			pUserSkel.GetSkeletonJointPosition(objeto_kinect->UserId, XN_SKEL_TORSO, torsoPos);
			Ogre::Vector3 newPos;
			newPos.x = -torsoPos.position.X;
			newPos.y = torsoPos.position.Y;
			newPos.z = -torsoPos.position.Z;
			Ogre::Vector3 newPos2 = (newPos - origTorsoPos)/100;
			newPos2.y -= 0.3;
			if (newPos2.y < 0)
				{
				newPos2.y /= 2.5;
				if (newPos2.y < -1.5)
					{
					newPos2.y = -1.5;
					}
				}

			if(torsoPos.fConfidence > 0.5)
				{
				rootBone->setPosition(newPos2);
				}
			}

		//do gestures for swords
		/*
		if ((mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
		{
		XnSkeletonJointPosition headPos;
		XnSkeletonJointPosition leftHand;
		XnSkeletonJointPosition rightHand;
		pUserSkel.GetSkeletonJointPosition(m_candidateID, XN_SKEL_HEAD, headPos);
		pUserSkel.GetSkeletonJointPosition(m_candidateID, XN_SKEL_LEFT_HAND, leftHand);
		pUserSkel.GetSkeletonJointPosition(m_candidateID, XN_SKEL_RIGHT_HAND, rightHand);
		if(leftHand.fConfidence > 0 && headPos.fConfidence > 0)
		{
		XnV3DVector leftvec = XnV3DVector(leftHand.position);
		XnV3DVector rightvec = XnV3DVector(rightHand.position);
		XnV3DVector headvec = XnV3DVector(headPos.position);
		XnV3DVector tempvec;
		tempvec = leftvec - rightvec;
		if(tempvec.MagnitudeSquared() < 50000)
		{
		tempvec = leftvec - headvec;
		if(!bRightAfterSwardsPositionChanged &&
		tempvec.MagnitudeSquared() < 100000)
		{

		if(leftHand.position.Z+150 > headPos.position.Z)
		{
		// take swords out (or put them back, since it's the same animation but reversed)
		setTopAnimation(ANIM_DRAW_SWORDS, true);
		mTimer = 0;
		bRightAfterSwardsPositionChanged = true;
		}
		}
		} else
		{
		bRightAfterSwardsPositionChanged = false;
		}
		}	
		} */
		} // end if player calibrated

	else
		{
		//return to initialState
		if(!bNewUser)
			{
			rootBone->resetToInitialState();
			}
		bNewUser = true;
		} 

	}

// ---------------------------------------------------------
void TutorialApplication::createFrameListener(void){
	BaseApplication::createFrameListener();
	Ogre::StringVector items_kinect;
	items_kinect.push_back("Num Users");
	items_kinect.push_back("User");
	items_kinect.push_back("Estado");
	items_kinect.push_back("");
	mKinectPanel = mTrayMgr->createParamsPanel(OgreBites::TL_NONE, "KinectPanel", 350, items_kinect);
	mKinectPanel->hide();
	}

//-------------------------------------------------------------------------------------
bool TutorialApplication::processUnbufferedInput(const Ogre::FrameEvent& evt)
	{
	if (kinectState)
		{
		objeto_kinect->buclePrincipal();
		PSupdateBody();    //Actualiza el skeleton de Ogre con el Skeleton de Kinect!!
		if (mKinectPanel->isVisible())
			{
			// update contents
			mKinectPanel->setParamValue(0,Ogre::String(objeto_kinect->numUsers));
			mKinectPanel->setParamValue(1,Ogre::String(objeto_kinect->strLabel));

			}
		}
	return true;
	}
//-------------------------------------------------------------------------------------
bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
	{
	bool ret = BaseApplication::frameRenderingQueued(evt);
	if(!processUnbufferedInput(evt)) return false;  // Process unbufered input
	return ret;
	}
//-------------------------------------------------------------------------------------
// OIS::KeyListener
bool TutorialApplication::keyPressed( const OIS::KeyEvent& evt ){
	bool ret = BaseApplication::keyPressed(evt);
	if (evt.key == OIS::KC_K)   // toggle visibility of the kinect details
		{
		if (mKinectPanel->getTrayLocation() == OgreBites::TL_NONE)
			{
			mTrayMgr->moveWidgetToTray(mKinectPanel, OgreBites::TL_TOPLEFT, 0);
			mKinectPanel->show();
			}
		else
			{
			mTrayMgr->removeWidgetFromTray(mKinectPanel);
			mKinectPanel->hide();
			}
		} else if (evt.key == OIS::KC_V) 
		{
		if (!kinectState) {
			kinectState=true;
			objeto_kinect = new kinect_class();
			} else {
				kinectState=false;
				delete objeto_kinect;
			}
			}
		return ret;
	}
bool TutorialApplication::keyReleased( const OIS::KeyEvent& evt ){
	bool ret = BaseApplication::keyReleased(evt);
	return ret;
	}
// OIS::MouseListener
bool TutorialApplication::mouseMoved( const OIS::MouseEvent& evt ){
	bool ret = BaseApplication::mouseMoved(evt);
	return ret;
	}
bool TutorialApplication::mousePressed( const OIS::MouseEvent& evt, OIS::MouseButtonID id ){
	bool ret = BaseApplication::mousePressed(evt,id);
	return ret;
	}
bool TutorialApplication::mouseReleased( const OIS::MouseEvent& evt, OIS::MouseButtonID id ){
	bool ret = BaseApplication::mouseReleased(evt,id);
	return ret;
	}

// -----------------------------------------------------------------------------------------------------

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
