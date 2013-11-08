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


#ifndef __CCNF_Patch_h_
#define __CCNF_Patch_h_
#include "IO.h"
namespace CLMTracker
{
  //===========================================================================
  /** 
	  A single Neuron response
  */
class CCNF_neuron{
	public:
	int     neuron_type; /**< Type of patch (0=raw,1=grad,3=depth) */
	double  norm_w; /**< scaling (needed as the energy of neuron might not be 1 */
	double  b; /**< bias                               */
	cv::Mat W; /**< Neural weights */

	// can have neural weight dfts that are calculated on the go as needed, this allows us not to recompute
	// the dft of the template each time
	map<int, cv::Mat> W_dfts;

	double alpha; // the alpha associated with the neuron

	CCNF_neuron(){;}

	inline int w(){return W.cols;}
	inline int h(){return W.rows;}

	void Read(std::ifstream &s,bool readType = true);
	void Response(cv::Mat &im, cv::Mat &im_dft, cv::Mat &resp);

	private:
	cv::Mat im_,res_;
};

//===========================================================================
/**
A CCNF patch expert
*/
class CCNF_patch{
public:
    
	int w,h;             /**< Width and height of patch */
    
	std::vector<CCNF_neuron> neurons;/**< List of neurons */

	// Precalculated Sigmas for certain window sizes
	std::vector<int> window_sizes;
	std::vector<cv::Mat_<float> > Sigmas;

	double   patch_confidence;

	CCNF_patch(){;}
	
	void Read(std::ifstream &s,bool readType, vector<int> window_sizes, vector<vector<cv::Mat> > sigma_components);

	// actual work (can pass in an image and a potential depth image, if the CCNF is trained with depth)
	void Response(cv::Mat &im, cv::Mat &d_im, cv::Mat &resp);    
	
	private:
	cv::Mat res_;
};
  //===========================================================================
}
#endif
