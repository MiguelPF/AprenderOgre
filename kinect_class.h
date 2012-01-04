
/****************************************************************************
*   Class for OpenNi handle by Miguel Pousa Freire                          *
*    Win32 Version                                                          *
****************************************************************************/
#ifndef __KINECT_CLASS_H__
#define __KINECT_CLASS_H__
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <XnPropNames.h>
#include <iostream>
											

//#define SAMPLE_XML_PATH "C:/Documents and Settings/Miguel/Escritorio/SamplesConfig.xml"
#define SAMPLE_XML_PATH "SamplesConfig.xml"

class kinect_class   {
	    public:
					
			xn::Context g_Context;
			xn::DepthGenerator g_DepthGenerator;
			xn::UserGenerator g_UserGenerator;
			xn::ScriptNode g_scriptNode;

			XnBool g_bNeedPose;
			XnChar g_strPose[20];
			XnBool g_bDrawBackground;
			XnBool g_bDrawPixels;
			XnBool g_bDrawSkeleton;
			XnBool g_bPrintID;
			XnBool g_bPrintState;
			XnBool g_bPause;
			XnBool g_bIstracking;
			XnUserID UserId;
			char numUsers[25]; //To print the number of users
			char strLabel[50]; //To print in screen the user number, the one who is calibrated, etc...
			XnBool g_bSuccess;
			XnStatus nRetVal;
			xn::EnumerationErrors errors;
			XnCallbackHandle hUserCallbacks, hCalibrationCallbacks, hPoseCallbacks;
			XnSkeletonJointTransformation joint1_head, joint2_neck, joint3_leftshoulder, joint4_rightshoulder, joint5_leftelbow, joint6_rightelbow;
			XnSkeletonJointTransformation joint7_lefthand, joint8_righthand, joint9_torso, joint10_lefthip, joint11_righthip, joint12_leftknee;
			XnSkeletonJointTransformation joint13_rightknee, joint14_leftfoot, joint15_rightfoot;

		static void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
			{
				//generator.GetPoseDetectionCap().StartPoseDetection("Psi", nId);
				generator.GetSkeletonCap().RequestCalibration(nId, TRUE);

			}

		static void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
			{
				  kinect_class* This = (kinect_class*)pCookie;
				  This->g_bIstracking = false;
			}

		static void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
			{
			}

		static void XN_CALLBACK_TYPE UserCalibration_CalibrationEnd(xn::SkeletonCapability& capability, XnUserID nId, XnBool bSuccess, void* pCookie)
			{
				kinect_class* This = (kinect_class*)pCookie;
			 	if (bSuccess)
				{
					// Calibration succeeded
					This->g_UserGenerator.GetSkeletonCap().StartTracking(nId);
				}
				else
				{
					// Calibration failed
					if (This->g_bNeedPose)
					{
						This->g_UserGenerator.GetPoseDetectionCap().StartPoseDetection("Psi", nId);
					}
					else
					{
						This->g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
					}
				}
			}

		static void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
			{
			 kinect_class* This = (kinect_class*)pCookie;
			 This->g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
			 This->g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
			}
        /**
                Constructor

                Inicializa OpenNI y se prepara
                para capturar eventos.
        */
        kinect_class()
			{
				g_bNeedPose = FALSE;
				//g_strPose[20] = static_cast<XnChar>("");
				g_bDrawBackground = TRUE;
				g_bDrawPixels = TRUE;
				g_bDrawSkeleton = TRUE;
				g_bPrintID = TRUE;
				g_bPrintState = TRUE;
				g_bPause = false;
				g_bIstracking = false;
				nRetVal = XN_STATUS_OK;
				nRetVal = g_Context.InitFromXmlFile(SAMPLE_XML_PATH, &errors);
				hUserCallbacks = NULL;
				hPoseCallbacks = NULL;
				hCalibrationCallbacks = NULL;
					if (nRetVal == XN_STATUS_NO_NODE_PRESENT)
					{
						XnChar strError[1024];
						errors.ToString(strError, 1024);
					}
					// Make sure we have all OpenNI nodes we will be needing for this sample
					xn::NodeInfoList nodes;
					//VALIDATE_GENERATOR(type, desc, generator)
					nRetVal = g_Context.EnumerateExistingNodes(nodes, XN_NODE_TYPE_DEPTH);			
					if (nodes.IsEmpty())										
						{															
										
						}															
					(*(nodes.Begin())).GetInstance(g_DepthGenerator);		

					//VALIDATE_GENERATOR(XN_NODE_TYPE_USER, "User", g_UserGenerator);
					nRetVal = g_Context.EnumerateExistingNodes(nodes, XN_NODE_TYPE_USER);			
					if (nodes.IsEmpty())										
						{															
										
						}															
					(*(nodes.Begin())).GetInstance(g_UserGenerator);	

				if (nRetVal != XN_STATUS_OK)
				{
					nRetVal = g_UserGenerator.Create(g_Context);									
				}
				g_UserGenerator.RegisterUserCallbacks(kinect_class::User_NewUser, kinect_class::User_LostUser, this, hUserCallbacks);
				g_UserGenerator.GetSkeletonCap().RegisterCalibrationCallbacks(kinect_class::UserCalibration_CalibrationStart, kinect_class::UserCalibration_CalibrationEnd, this, hCalibrationCallbacks);
				if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration())
				{
					g_bNeedPose = TRUE;
					g_UserGenerator.GetPoseDetectionCap().RegisterToPoseCallbacks(kinect_class::UserPose_PoseDetected, NULL, this, hPoseCallbacks);
					g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
				}
				g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
				nRetVal = g_Context.StartGeneratingAll();
			}

		/**
                Destructor

                Destruye el objeto de tipo kinect_class.
        */
         ~kinect_class()
			{
			g_Context.StopGeneratingAll();
			if (NULL != hPoseCallbacks)
			{
				g_UserGenerator.GetPoseDetectionCap().UnregisterFromPoseCallbacks(hPoseCallbacks);
				hPoseCallbacks = NULL;
			}
			if (NULL != hUserCallbacks)
			{
				g_UserGenerator.UnregisterUserCallbacks(hUserCallbacks);
				hUserCallbacks = NULL;
			}
			if (NULL != hCalibrationCallbacks)
			{
				g_UserGenerator.GetSkeletonCap().UnregisterCalibrationCallbacks(hCalibrationCallbacks);
				hCalibrationCallbacks = NULL;
			}
			g_Context.Shutdown();
			}

		 /**
         *  Bucle principal
         * 
         *  Inicia el bucle principal
         */
		void kinect_class::buclePrincipal()
			{
				xn::SceneMetaData sceneMD;
				xn::DepthMetaData depthMD;
				g_Context.WaitNoneUpdateAll();
				UserId=0;
				// Process the data
				g_DepthGenerator.GetMetaData(depthMD);
				g_UserGenerator.GetUserPixels(0, sceneMD);
				XnSkeletonJoint  eJoint;
				//XnSkeletonJointTransformation &Joint=0;
				XnUserID aUsers[15];
				XnUInt16 nUsers = 15;
				g_UserGenerator.GetUsers(aUsers, nUsers);
				
				sprintf(numUsers, "%d", nUsers);
				if (nUsers==0) sprintf(strLabel, "%d", 0); 
				
				for (int i = 0; i < nUsers; ++i)
				{
					XnPoint3D com;
					g_UserGenerator.GetCoM(aUsers[i], com);  //centro de masas del user i
					//g_DepthGenerator.ConvertRealWorldToProjective(1, &com, &com);
					sprintf(strLabel, "%d", aUsers[i]);  //User i
					if (g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i]))
						{
						sprintf(strLabel, "%d - Tracking", aUsers[i]);
						//Process nId user joints
						// g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i],XN_SKEL_HEAD,XnSkeletonJointTransformation &Joint); 
						
						g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_HEAD, joint1_head);
						g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_NECK, joint2_neck);
						g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_LEFT_SHOULDER, joint3_leftshoulder);
						g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_RIGHT_SHOULDER, joint4_rightshoulder);
						g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_LEFT_ELBOW, joint5_leftelbow);
						g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_RIGHT_ELBOW, joint6_rightelbow);
						g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_LEFT_HAND, joint7_lefthand);
						g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_RIGHT_HAND, joint8_righthand);
						g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_TORSO, joint9_torso);
						g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_LEFT_HIP, joint10_lefthip);
						g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_RIGHT_HIP, joint11_righthip);
						g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_LEFT_KNEE, joint12_leftknee);
						g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_RIGHT_KNEE, joint13_rightknee);
						g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_LEFT_FOOT, joint14_leftfoot);
						g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_RIGHT_FOOT, joint15_rightfoot);

						g_bIstracking = true; //Avisa de que hay un tracking activo
						UserId=aUsers[i];
						}
					else if (g_UserGenerator.GetSkeletonCap().IsCalibrating(aUsers[i]))
						{
						sprintf(strLabel, "%d - Calibrating...", aUsers[i]);
						}
					else
						{
						sprintf(strLabel, "%d - Looking for pose", aUsers[i]);
						}
				} 
			}



			
		
     friend class TutorialApplication;

	};
#endif

