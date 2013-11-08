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

#ifndef __CLM_h_
#define __CLM_h_
#include "PDM.h"
#include "Patch_expert.h"
#include "CCNF_patch.h"
#include <vector>

using namespace std;
namespace CLMTracker
{
  //===========================================================================
  /** 
      A Constrained Local Model
  */
  class CLM{
  public:
    PDM                               _pdm;   /**< 3D Shape model           */
    cv::Mat_<double>                  _plocal;/**< local parameters         */
    cv::Mat                           _pglobl;/**< global parameters        */

	double						      _likelihood;

	// Patches
	vector<vector<Mat> >               _cent;  /**< Centers/view/scale (Euler)     */
    vector<vector<Mat> >               _visi;  /**< Visibility for each scale and view */

	vector<vector<vector<MultiPatchExpert> > >			_patch; /**< Patches/point/view       */
	vector<vector<vector<CCNF_patch> > >				_ccnf_patch; /**< Patches/point/view       */
	vector<double>										_patchScaling; // what the scaling for the patch is in training

	vector<vector<Mat> >								_centDepth;  /**< Centers/view (Euler)     */
    vector<vector<Mat> >								_visiDepth;  /**< Visibility for each view */    
    vector<vector<vector<MultiPatchExpert> > >			_patchDepth; /**< Patches/point/view       */
    vector<double>										_patchScalingDepth; // what the scaling for the patch is in training

	vector<Mat_<int> >                _triangulations; // the triangulation per each view (for drawing purposes only)

    CLM(){;}

	void Read(string clmLocation, string override_pdm_loc = string());
	void ReadPatches(string patchesFileLocation, vector<Mat>& centers, vector<Mat>& visibility, vector<vector<MultiPatchExpert> >& patches, double& patchScaling);
	void Read_CCNF_Patches(string patchesFileLocation, std::vector<cv::Mat>& centers, std::vector<cv::Mat>& visibility, std::vector<std::vector<CCNF_patch> >& patches, double& patchScaling);

	// The number of views at a particular scale
    inline int nViews(int scale=0){return _cent[scale].size();}

    int GetViewIdx(int scale = 0);
    int GetDepthViewIdx(int scale = 0);

    bool Fit(const cv::Mat_<uchar>& im, const cv::Mat_<float>& depthImg, const std::vector<int>& wSize, int nIter = 10,double clamp = 3.0,double fTol = 0.0, bool morphology = false, double sigma = 1, double reg_factor = 50, double weight_factor = 0, bool limit_pose = true);

  private:
    cv::Mat cshape_; // the 2D shape holder, so that new memory does not have to be allocated every time
    cv::Mat ms_,u_,g_; // The mean shift, and other linear system holders
	cv::Mat J_,H_;  // Jacobian, and Hessian placeholders
	cv::Mat J_w_t_; // Transposed weighted Jacobian placeholder (J.t() * W), assuming that W is diagonal

    std::vector<cv::Mat> prob_,wmem_; // placeholders for the response maps and areas of interest
	
	void VectorisedMeanShift(Mat_<double>& meanShifts, const Mat_<double> &iis, const Mat_<double> &jjs, const Mat_<double> &dxs, const Mat_<double> &dys, const Size patchSize, double sigma, int scale, int view_id);
	void NonVectorisedMeanShift(Mat& ms_, const Mat_<double> &dxs, const Mat_<double> &dys, int resp_size, double a, int scale, int view_id);

	// Returns the model likelihood
    double Optimize(Mat& final_global, Mat_<double>& final_local,  const Mat_<double>& initial_global, const Mat_<double>& initial_local_ident,
		          const Mat_<double>& baseShape, const Matx22d& sim_img_to_ref, const Matx22d& sim_ref_to_img, int resp_size, int idx, int nIter, double fTol,
				  double clamp, bool rigid, int scale, double sigma, double reg_factor, double weight_factor);

  };
  //===========================================================================
}
#endif
