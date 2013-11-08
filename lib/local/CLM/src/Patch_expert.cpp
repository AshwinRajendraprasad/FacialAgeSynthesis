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

#include <Patch_expert.h>
#include <stdio.h>
#include <iostream>
#include <highgui.h>

#define SGN(x) (x<0) ? 0:1
using namespace CLMTracker;

//===========================================================================
void sum2one(cv::Mat &M)
{
	assert(M.type() == CV_64F);
	double sum = 0; int cols = M.cols, rows = M.rows;

	if(M.isContinuous()){cols *= rows;rows = 1;}

	for(int i = 0; i < rows; i++)
	{
		const double* Mi = M.ptr<double>(i);
		for(int j = 0; j < cols; j++)
		{
			sum += *Mi++;
		}
	}
	if( sum!= 0)
	{
		M /= sum;
	}
}
//===========================================================================
void Grad(cv::Mat im,cv::Mat grad)
{

	assert((im.rows == grad.rows) && (im.cols == grad.cols));
	assert((im.type() == CV_32F) && (grad.type() == CV_32F));
	int x,y,h = im.rows,w = im.cols; float vx,vy; grad = cv::Scalar(0);
	cv::MatIterator_<float> gp  = grad.begin<float>() + w+1;
	cv::MatIterator_<float> px1 = im.begin<float>()   + w+2;
	cv::MatIterator_<float> px2 = im.begin<float>()   + w;
	cv::MatIterator_<float> py1 = im.begin<float>()   + 2*w+1;
	cv::MatIterator_<float> py2 = im.begin<float>()   + 1;
	for(y = 1; y < h-1; y++){ 
	for(x = 1; x < w-1; x++){
		vx = *px1++ - *px2++;
		vy = *py1++ - *py2++;
		*gp++ = vx*vx + vy*vy;
	}
	px1 += 2; px2 += 2; py1 += 2; py2 += 2; gp += 2;
	}
	return;

	/*float filter[3] = {1, 0, -1};
	float dfilter[1] = {1};	
	cv::Mat filterX = cv::Mat(1,3,CV_32F, filter).clone();
	cv::Mat filterY = cv::Mat(1,1,CV_32F, dfilter).clone();
			
	cv::Mat gradX;
	cv::Mat gradY;
	cv::sepFilter2D(im, gradX, CV_32F, filterY, filterX, cv::Point(-1,-1), 0);
	cv::sepFilter2D(im, gradY, CV_32F, filterX.t(), filterY, cv::Point(-1,-1), 0);
	cv::pow(gradX,2, gradX);
	cv::pow(gradY,2, gradY);
	grad = gradX + gradY;
			
	grad.row(0).setTo(0);
	grad.col(0).setTo(0);
	grad.col(grad.cols-1).setTo(0);
	grad.row(grad.rows-1).setTo(0);		*/

}

//===========================================================================
void PatchExpert::Read(ifstream &s,bool readType)
{
  if(readType)
  {
	  int type; s >> type;
	  assert(type == IO::PATCH);
  }
  s >> _t >> _confidence >> _a >> _b;
  IO::ReadMat(s,_W); 
  _W = _W.t();
  //std::cout << _W << endl;
  return;
}

//===========================================================================
void PatchExpert::Response(cv::Mat &im,cv::Mat &resp)
{
	assert((im.type() == CV_32F) && (resp.type() == CV_64F));
	assert((im.rows>=_W.rows) && (im.cols>=_W.cols));

	int h = im.rows - _W.rows + 1;
	int w = im.cols - _W.cols + 1;
	
	// the patch area on which we will calculate reponses
	cv::Mat I;
  
	if(resp.rows != h || resp.cols != w)
	{
		resp.create(h,w,CV_64F);
	}

	if(res_.rows != h || res_.cols != w)
	{
		res_.create(h,w,CV_32F);
	}
  
	if(_t == 0)
	{
		// Perform normalisation across whole patch
		cv::Scalar mean;
		cv::Scalar std;
		//double norm = cv::norm(im_, cv::NORM_L2);
		cv::meanStdDev(im, mean, std);
		I = (im - mean[0]) / std[0];
	}
	else
	{
		if(im_.rows == im.rows && im_.cols == im.cols)
		{
			I = im_;
		}
		else if(im_.rows >= im.rows && im_.cols >= im.cols)
		{
			I = im_(cv::Rect(0,0,im.cols,im.rows));
		}
		else
		{
			im_.create(im.rows,im.cols,CV_32F);
			I = im_;
		}

		if(_t == 1)
		{
			Grad(im,I);
		}
		else
		{
			printf("ERROR(%s,%d): Unsupported patch type %d!\n", __FILE__,__LINE__,_t);
			abort();
		}
	}
  
	cv::matchTemplate(I,_W,res_,CV_TM_CCOEFF_NORMED); // the linear multiplication, efficient calc of response

	cv::MatIterator_<double> p = resp.begin<double>(); // output
	cv::MatIterator_<float> q1 = res_.begin<float>(); // respone for each pixel
	cv::MatIterator_<float> q2 = res_.end<float>();
	while(q1 != q2)*p++ = 1.0/(1.0 + exp( -(*q1++ * _a + _b ))); // the respnse times weight and + bias

	return;
}

void PatchExpert::ResponseDepth(cv::Mat &im,cv::Mat &resp)
{

	assert((im.type() == CV_32F) && (resp.type() == CV_64F));
	assert((im.rows>=_W.rows) && (im.cols>=_W.cols));

	int h = im.rows - _W.rows + 1;
	int w = im.cols - _W.cols + 1;
	
	// the patch area on which we will calculate reponses
	cv::Mat I;
  
	if(resp.rows != h || resp.cols != w)
	{
		resp.create(h,w,CV_64F);
	}

	if(res_.rows != h || res_.cols != w)
	{
		res_.create(h,w,CV_32F);
	}
  
	if(_t == 0)
	{
		// Perform normalisation across whole patch
		cv::Scalar mean;
		cv::Scalar std;
		
		// ignore missing values
		cv::Mat_<uchar> mask = im > 0;
		cv::meanStdDev(im, mean, std, mask);

		// if all values the same don't divide by 0
		if(std[0] != 0)
		{
			I = (im - mean[0]) / std[0];
		}
		else
		{
			I = (im - mean[0]);
		}

		I.setTo(0, mask == 0);
	}
	else
	{
		if(im_.rows == im.rows && im_.cols == im.cols)
		{
			I = im_;
		}
		else if(im_.rows >= im.rows && im_.cols >= im.cols)
		{
			I = im_(cv::Rect(0,0,im.cols,im.rows));
		}
		else
		{
			im_.create(im.rows,im.cols,CV_32F);
			I = im_;
		}

		if(_t == 1)
		{
			Grad(im,I);
		}
		else
		{
			printf("ERROR(%s,%d): Unsupported patch type %d!\n", __FILE__,__LINE__,_t);
			abort();
		}
	}
  
	cv::matchTemplate(I,_W,res_,CV_TM_CCOEFF); // the linear multiplication, efficient calc of response
	cv::MatIterator_<double> p = resp.begin<double>(); // output
	cv::MatIterator_<float> q1 = res_.begin<float>(); // respone for each pixel
	cv::MatIterator_<float> q2 = res_.end<float>();
	while(q1 != q2)
	{
		*p++ = 1.0/(1.0 + exp( -(*q1++ * _a + _b ))); // sigmoid function of the logistic regressor
	}
	
  return;
}

//===========================================================================
void MultiPatchExpert::Read(ifstream &s,bool readType)
{
  if(readType)
  {
	  int type;
	  s >> type;
	  assert(type == IO::MPATCH);
  }

  int n;
  s >> _w >> _h >> n;
  // n - the number of patches for this view
  _p.resize(n);
  for(int i = 0; i < n; i++)
	  _p[i].Read(s);

}
//===========================================================================
void MultiPatchExpert::Response(cv::Mat &im,cv::Mat &resp)
{
	
	assert((im.type() == CV_32F) && (resp.type() == CV_64F));
	assert((im.rows >= _h) && (im.cols >= _w));

	int h = im.rows - _h + 1;
	int w = im.cols - _w + 1;
	if(resp.rows != h || resp.cols != w)resp.create(h,w,CV_64F);
	if(res_.rows != h || res_.cols != w)res_.create(h,w,CV_64F);
	
	// For the purposes of the experiment only use the response of normal intensity, for fair comparison

	if(_p.size() == 1)
	{
		_p[0].Response(im,resp);

		sum2one(resp);
		
	}
	else
	{
		// responses from multiple patch experts these can be gradients, LBPs etc.
		resp = cvScalar(1.0);
		for(size_t i = 0; i < _p.size(); i++)
		{			
			_p[i].Response(im,res_);
			sum2one(res_);
			resp = resp.mul(res_);	
		}	
		sum2one(resp); 
		
	}

}

void MultiPatchExpert::ResponseDepth(cv::Mat &im,cv::Mat &resp)
{
	assert((im.type() == CV_32F) && (resp.type() == CV_64F));
	assert((im.rows >= _h) && (im.cols >= _w));

	int h = im.rows - _h + 1;
	int w = im.cols - _w + 1;
	if(resp.rows != h || resp.cols != w)resp.create(h,w,CV_64F);
	if(res_.rows != h || res_.cols != w)res_.create(h,w,CV_64F);
	if(_p.size() == 1)
	{
		_p[0].ResponseDepth(im,resp);

		sum2one(resp);
		if(resp.at<double>(0) != resp.at<double>(0) )
		{
			_p[0].ResponseDepth(im,resp);
			cout << resp << endl;
		}
	}
	else
	{
		// responses from multiple patch experts
		resp = cvScalar(1.0);
		for(size_t i = 0; i < _p.size(); i++)
		{					
			_p[i].ResponseDepth(im,res_);
			
			sum2one(res_);
			
			resp = resp * res_;			
		}	
		sum2one(resp); 
		
	}
}
//===========================================================================
