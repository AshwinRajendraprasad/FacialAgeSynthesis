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

#ifndef __Patch_expert_h_
#define __Patch_expert_h_
#include "IO.h"
namespace CLMTracker
{
  //===========================================================================
  /** 
      A Patch Expert
  */
class PatchExpert{
	public:
		int     _t; /**< Type of patch (0=raw,1=grad,2=lbp) */
		double  _a; /**< scaling                            */
		double  _b; /**< bias                               */
		cv::Mat _W; /**< Gain                               */
		double  _confidence; /**< Confidence of the current patch expert   */

		PatchExpert(){;}

		inline int w(){return _W.cols;}
		inline int h(){return _W.rows;}

		void Read(std::ifstream &s,bool readType = true);
		void Response(cv::Mat &im,cv::Mat &resp);    
		void ResponseDepth(cv::Mat &im,cv::Mat &resp);

	private:
		cv::Mat im_,res_;
};
  //===========================================================================
  /**
     A Multi-patch Expert that can include different patch types. Raw pixel values or image gradients
  */
class MultiPatchExpert{
	public:
		int _w,_h;  /**< Width and height of patch */
		std::vector<PatchExpert> _p; /**< List of patches           */

		MultiPatchExpert(){;}
	
		void Read(std::ifstream &s,bool readType = true);

		// actual work
		void Response(cv::Mat &im,cv::Mat &resp);    
		void ResponseDepth(cv::Mat &im,cv::Mat &resp);    

	private:
		cv::Mat res_;
};
  //===========================================================================
}
#endif
