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

#ifndef __PDM_h_
#define __PDM_h_
#include "IO.h"

using namespace cv;

namespace CLMTracker
{
  //===========================================================================
  /** 
      A 3D Point Distribution Model
  */
class PDM{
	public:    
    
		cv::Mat_<double> _M; /**< mean 3D shape vector [x1,..,xn,y1,...yn]      */
  
		cv::Mat_<double> _V;/**< basis of variation                            */
		cv::Mat_<double> _E;/**< vector of eigenvalues (row vector)            */

		PDM(){;}
		
		void Read(string location);

		// Number of vertices
		inline int NumberOfPoints(){return _M.rows/3;}
		
		// Listing the number of modes of variation
		inline int NumberOfModes(){return _V.cols;}

		void Clamp(Mat& p_local, double sigmas, const Mat& E);

		// Initialise default parameter values
		void Identity(Mat& p_local, Mat& pglobl);

		// Compute shape in object space (3D)
		void CalcShape3D(Mat& s, const Mat& p_local);

		// Compute shape in image space (2D)
		void CalcShape2D(Mat& s, const Mat& p_local, const Mat& p_globl);
    
		// provided the region of interest, and the local parameters (with optional rotation), generates the global parameters that can generate the face in there
		void CalcParams(Mat& pGlobal, const Rect& roi, const Mat& p_local, const Vec3d rotation = Vec3d(0.0));

		// Helpers for computing Jacobians, and Jacobians with the weight matrix
		void CalcRigidJacob(const Mat& p_local, const Mat& p_globl, Mat &Jacob, const Mat_<double> W, cv::Mat &Jacob_t_w);
		void CalcJacob(const Mat& p_local, const Mat& pglobl, const Mat_<double>& V,  Mat &Jacob, const Mat_<double> W, cv::Mat &Jacob_t_w);

		// Given the current parameters, and the computed delta_p compute the updated parameters
		void CalcReferenceUpdate(cv::Mat &delta_p, cv::Mat &p_local, cv::Mat &p_globl, cv::Mat& V);

		// Helper visualisation functions
		void Draw(cv::Mat img, const Mat& p_local, const Mat& pglobl, Mat_<int>& triangulation, Mat& visibilities);
		void Draw(cv::Mat img, const Mat& shape, Mat_<int>& triangulation, Mat& visibilities);

	private:
		Mat_<double> _Shape_3D; // preallocated place holder for 3D PDM model (so that memory would not be allocated every time)

  };
  //===========================================================================
}
#endif
