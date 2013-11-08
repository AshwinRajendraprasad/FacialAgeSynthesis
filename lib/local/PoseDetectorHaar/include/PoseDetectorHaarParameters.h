///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012, Tadas Baltrusaitis, all rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//     * The software is provided under the terms of this licence stricly for
//       academic, non-commercial, not-for-profit purposes.
//     * Redistributions of source code must retain the above copyright notice, 
//       this list of conditions (licence) and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright 
//       notice, this list of conditions (licence) and the following disclaimer 
//       in the documentation and/or other materials provided with the 
//       distribution.
//     * The name of the author may not be used to endorse or promote products 
//       derived from this software without specific prior written permission.
//     * As this software depends on other libraries, the user must adhere to 
//       and keep in place any licencing terms of those libraries.
//     * Any publications arising from the use of this software, including but
//       not limited to academic journal and conference publications, technical
//       reports and manuals, must cite the following work:
//
//       Tadas Baltrusaitis, Peter Robinson, and Louis-Philippe Morency. 3D
//       Constrained Local Model for Rigid and Non-Rigid Facial Tracking.
//       IEEE Conference on Computer Vision and Pattern Recognition (CVPR), 2012.    
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED 
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO 
// EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////

//  Header for All Gavam methods utilities for the tracker and the tracking methods themselves
//
//  Description: Main Gavam header
//
//  Louis-Philippe Morency
//	18/03/2002
//
//  Tadas Baltrusaitis
//  12/07/2011

#ifndef __POSE_DETECTOR_PARAM_H
#define __POSE_DETECTOR_PARAM_H

#include <cv.h>

using namespace cv;

namespace PoseDetectorHaar
{

struct PoseDetectorHaarParameters
{
	double HighUncertaintyVal, LowUncertaintyVal;
	Matx66d LowCovariance; // very certain
	Matx66d HighCovariance; // very uncertain
	
	// Face parameters (used to estimate the depth of the object)
	double ObjectWidth;	

	// search window around the current pose
	double SearchWindowWidth, SearchWindowHeight;

	// offsets from the center of face to region of interest in milimeters (basically the bounding box of the object in 3D)
	double XLeftOffset, XRightOffset, YTopOffset, YBottomOffset, ZNearOffset, ZFarOffset;	

	// the offsets from the estimates based on width
	double XEstimateOffset, YEstimateOffset, ZEstimateOffset;

	// there might be cases where the offsets are different if depth is used to refine the tracking
	double XDepthEstimateOffset, YDepthEstimateOffset, ZDepthEstimateOffset;

	string ClassifierLocation;

	PoseDetectorHaarParameters()
	{
		HighUncertaintyVal = 1000;
		LowUncertaintyVal = 0.000001;
		LowCovariance = Matx66d::eye() * LowUncertaintyVal;
		HighCovariance = Matx66d::eye() * HighUncertaintyVal;

		ObjectWidth = 200;
	
		SearchWindowWidth = ObjectWidth * 2;
		SearchWindowHeight = ObjectWidth * 2;

		XLeftOffset = 300;
		XRightOffset = 300;
		YTopOffset = 200;
		YBottomOffset = 200;
		ZNearOffset = 300;
		ZFarOffset = 250;		

		XEstimateOffset = 0;
		YEstimateOffset = 0;
		ZEstimateOffset = 0;

		XDepthEstimateOffset = 0;
		YDepthEstimateOffset = 0;
		ZDepthEstimateOffset = 100;

		ClassifierLocation = "classifiers/haarcascade_frontalface_default.xml";
	}

};

}
#endif __POSE_DETECTOR_PARAM_H