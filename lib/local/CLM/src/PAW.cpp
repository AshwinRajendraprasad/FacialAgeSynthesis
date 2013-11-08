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

#include <PAW.h>


#include <cv.h>
#include <highgui.h>

bool OPENCVWarp = false;			//this is the boolean that does the warping inside OPENCV (in this program). 
//Otherwise, the warping to neutral is done within this program, but the warping onto the map is done by the GPU with OpenGL

#define it at<int>
#define db at<double>

using namespace CLMTracker;

using namespace cv;

bool quit = false;

double oldtime = 0;

string avatarfile;
bool readfile = false;

Mat savatar;
Mat _tridx2(600, 600, CV_32S, Scalar(0));  /**<		...in the output (animated)image			*/
Mat colourimage;
Mat avatarlocal;
Mat warpedbackface;

Mat avatarWarpedHead, avatarWarpleft, avatarWarpright;
Mat avatarS, avatarSleft, avatarSright;
Mat avatarNextDst;

void sendAvatar(Mat avatarHead, Mat avatarShape){

	cvtColor(avatarHead, avatarWarpedHead, CV_BGR2RGB); 
	avatarS = avatarShape;


}


//===========================================================================
void PAW::Read(ifstream &s)
{
	IO::SkipComments(s);
	s >> _nPix >> _xmin >> _ymin;

	IO::SkipComments(s);
	IO::ReadMat(s,_src);

	IO::SkipComments(s);
	IO::ReadMat(s,_tri);

	IO::SkipComments(s);
	IO::ReadMat(s,_tridx);
	
	cv::Mat tmpMask;
	IO::SkipComments(s);		
	IO::ReadMat(s, tmpMask);	
	tmpMask.convertTo(_mask, CV_8U);	

	//cout << _mask << endl;
	IO::SkipComments(s);
	IO::ReadMat(s,_alpha);

	IO::SkipComments(s);
	IO::ReadMat(s,_beta);

	_mapx.create(_mask.rows,_mask.cols,CV_32F);
	_mapy.create(_mask.rows,_mask.cols,CV_32F);
	_coeff.create(this->nTri(),6,CV_64F); _dst = _src;
}

void sendAvatarFile(string filename){
	avatarfile = filename;
	readfile = true;
};


//=============================================================================
// cropping from the source image to the destination image using the shape in s, used to determine if shape fitting converged successfully
void PAW::Crop(const cv::Mat &src, cv::Mat &dst, cv::Mat &s)
{
  
	// set the current shape
	_dst = s;

	// prepare the mapping coefficients using the current shape
	this->CalcCoeff();

	// Do the actual mapping computation
	this->WarpRegion(_mapx,_mapy);
  
	// Do the actual warp (with bi-linear interpolation)
	cv::remap(src,dst,_mapx,_mapy,CV_INTER_LINEAR);
  
}


//=============================================================================
// Calculate the warping coefficients
void PAW::CalcCoeff()
{
	int i,j,k,l,p=this->nPoints();
	double c1,c2,c3,c4,c5,c6,*coeff,*alpha,*beta;

	for(l = 0; l < this->nTri(); l++)
	{
	  
		i = _tri.it(l,0);
		j = _tri.it(l,1);
		k = _tri.it(l,2);

		c1 = _dst.db(i  ,0);
		c2 = _dst.db(j  ,0) - c1;
		c3 = _dst.db(k  ,0) - c1;
		c4 = _dst.db(i+p,0);
		c5 = _dst.db(j+p,0) - c4;
		c6 = _dst.db(k+p,0) - c4;

		coeff = _coeff.ptr<double>(l);
		alpha = _alpha.ptr<double>(l);
		beta  = _beta.ptr<double>(l);

		coeff[0] = c1 + c2*alpha[0] + c3*beta[0];
		coeff[1] =      c2*alpha[1] + c3*beta[1];
		coeff[2] =      c2*alpha[2] + c3*beta[2];
		coeff[3] = c4 + c5*alpha[0] + c6*beta[0];
		coeff[4] =      c5*alpha[1] + c6*beta[1];
		coeff[5] =      c5*alpha[2] + c6*beta[2];
	}
}
//=============================================================================
void PAW::unCalcCoeff(cv::Mat &s, cv::Mat &snew)
{

	
		cvtColor(colourimage, colourimage, CV_BGR2RGB); 


	int i,j,k,l,p = (s.rows/2);
	double c1,c2,c3,c4,c5,c6,*coeff,*alpha,*beta,d1,d2,d3,d4,d5,d6;
	
			Mat afftrans, invafftrans, trianglematrix;

			
	int numl = this->nTri();
	
						//cout << _dst << endl;
	Mat backgroundimage = Mat::zeros(colourimage.size(),CV_8UC3);

	for(l = 0; l < numl; l++)
	{
	  
		i = _tri.it(l,0);								//find the indices of the triangle in question
		j = _tri.it(l,1);
		k = _tri.it(l,2);

		c1 = s.db(i  ,0);							//i.x		Look up the current shape values in s
		c2 = s.db(j  ,0) - c1;						//j.x-i.x		these bits for X
		c3 = s.db(k  ,0) - c1;						//k.x-i.x		(relative shape)
		c4 = s.db(i+p,0);							//i.y		these bits for Y
		c5 = s.db(j+p,0) - c4;						//j.y-i.y
		c6 = s.db(k+p,0) - c4;						//k.y-i.y

		coeff = _coeff.ptr<double>(l);					//look up the alpha, beta values 
		alpha = _alpha.ptr<double>(l);
		beta  = _beta.ptr<double>(l);

		coeff[0] = c1 + c2*alpha[0] + c3*beta[0];
		coeff[1] =      c2*alpha[1] + c3*beta[1];
		coeff[2] =      c2*alpha[2] + c3*beta[2];
		coeff[3] = c4 + c5*alpha[0] + c6*beta[0];
		coeff[4] =      c5*alpha[1] + c6*beta[1];
		coeff[5] =      c5*alpha[2] + c6*beta[2];

		d1 = coeff[0];
		d2 = coeff[1];
		d3 = coeff[2];
		d4 = coeff[3];
		d5 = coeff[4];
		d6 = coeff[5];
	



		//these coefficients coeff[1]...coeff[6] are used to generate an INVERSE MAP, ie, in mapping the large image to the small image these coefficients create a map of the size of the smaller image...
		//... within which the indices to be read from the larger image are looked up. 

		if(i <= p){



			bool AVATARWITHOPENCV = true;

			if(AVATARWITHOPENCV){
	afftrans = (Mat_<double>(2, 3) << 
	d2,d3,d1,
	d5,d6,d4);
	 

	 	invertAffineTransform(afftrans, invafftrans);
		d2 = invafftrans.db(0,0);
		d3 = invafftrans.db(0,1);
		d1 = invafftrans.db(0,2);
		d5 = invafftrans.db(1,0);
		d6 = invafftrans.db(1,1);
		d4 = invafftrans.db(1,2);
		
	 	snew.db(i,0) = d1 + (d2*s.db(i,0)) + (d3*s.db(i+p,0));
		snew.db(i+p,0) = d4 + (d5*s.db(i,0)) + (d6*s.db(i+p,0));

	 	snew.db(j,0) = d1 + (d2*s.db(j,0)) + (d3*s.db(j+p,0));
		snew.db(j+p,0) = d4 + (d5*s.db(j,0)) + (d6*s.db(j+p,0));

	 	snew.db(k,0) = d1 + (d2*s.db(k,0)) + (d3*s.db(k+p,0));
		snew.db(k+p,0) = d4 + (d5*s.db(k,0)) + (d6*s.db(k+p,0));


		
		if(!avatarWarpedHead.empty() && OPENCVWarp){
		
		Point2f srcpoints[3], dstpoints[3];
		srcpoints[0] = Point2f(s.db(i,0),s.db(i+p,0));
		srcpoints[1] = Point2f(s.db(j,0),s.db(j+p,0));
		srcpoints[2] = Point2f(s.db(k,0),s.db(k+p,0));
		
		cv::Point pts[3] = {Point(avatarS.db(i,0),avatarS.db(i+p,0)), Point(avatarS.db(j,0),avatarS.db(j+p,0)), Point(avatarS.db(k,0),avatarS.db(k+p,0)) };
	
		float ox = 0.0; 
		float oy = 0.0;

		dstpoints[0] = Point2f(avatarS.db(i,0)+ox,avatarS.db(i+p,0)+oy);
		dstpoints[1] = Point2f(avatarS.db(j,0)+ox,avatarS.db(j+p,0)+oy);
		dstpoints[2] = Point2f(avatarS.db(k,0)+ox,avatarS.db(k+p,0)+oy);


		Mat warp_mat = 	getAffineTransform(dstpoints,srcpoints);
		
		Mat newimage; 
		Mat mask = Mat::zeros(avatarWarpedHead.size(),CV_8UC3);
		Mat maskedimage;


		fillConvexPoly(mask, pts, 3, Scalar(255,255,255), 8, 0);
	//	imshow("mask with poly" , mask);
	//	imshow("avatar warped head", avatarWarpedHead);
		avatarWarpedHead.copyTo(maskedimage, mask);
	//	imshow("masked image : ", maskedimage);
		warpAffine(maskedimage, newimage, warp_mat, backgroundimage.size(), 1, 0, 0);
		backgroundimage = backgroundimage | newimage;
	//	imshow("the background image", backgroundimage);
	//	waitKey(500);
		}


		







		}

		}

	}



	if(!avatarWarpedHead.empty() && OPENCVWarp){	
		Mat reversemask, greybackground;


		cvtColor(backgroundimage, greybackground, CV_RGB2GRAY);
		threshold(greybackground, reversemask, 1, 255, CV_THRESH_BINARY_INV );


		Mat newcolourimage;
		colourimage.copyTo(newcolourimage,reversemask);
		add(newcolourimage, backgroundimage, newcolourimage);
		imshow("colour image", newcolourimage);
	}
	
}
//======================================================================
void PAW::WarpRegion(cv::Mat &mapx,cv::Mat &mapy)
{
	assert((mapx.type() == CV_32F) && (mapy.type() == CV_32F));

	if((mapx.rows != _mask.rows) || (mapx.cols != _mask.cols))
		_mapx.create(_mask.rows,_mask.cols,CV_32F);

	if((mapy.rows != _mask.rows) || (mapy.cols != _mask.cols))
		_mapy.create(_mask.rows,_mask.cols,CV_32F);

	int x,y,j,k=-1;
	double yi,xi,xo,yo,*a=NULL,*ap;
	

	//cv::imshow("XmapBefore", mapx);
	//cv::imshow("YmapBefore", mapy);

	cv::MatIterator_<float> xp = mapx.begin<float>();
	cv::MatIterator_<float> yp = mapy.begin<float>();
	cv::MatIterator_<uchar> mp = _mask.begin<uchar>();
	cv::MatIterator_<int>   tp = _tridx.begin<int>();
	//cout << _mask << endl << _mask.type();
	//CV_8U

	

	//	cout << "y min = " << _ymin << ", x min = " << _xmin << endl;



	for(y = 0; y < _mask.rows; y++)
	{
		yi = double(y) + _ymin;

	


		for(x = 0; x < _mask.cols; x++)
		{
			xi = double(x) + _xmin;

			if(*mp == 0)
			{
				*xp = -1;
				*yp = -1;
			}
			else
			{
		
				j = *tp;
				if(j != k)
				{
					a = _coeff.ptr<double>(j);			//a is the pointer to the coefficients
					k = j;
				}  	
				ap = a;									//ap is now the pointer to the coefficients
				xo = *ap++;								//look at the first coefficient (and increment). first coefficient is an x offset
				xo += *ap++ * xi;						//second coefficient is an x scale as a function of x
				*xp = float(xo + *ap++ * yi);			//third coefficient ap(2) is an x scale as a function of y
				yo = *ap++;								//then fourth coefficient ap(3) is a y offset
				yo += *ap++ * xi;						//fifth coeff adds coeff[4]*x to y
				*yp = float(yo + *ap++ * yi);			//final coeff adds coeff[5]*y to y




			}
			mp++; tp++; xp++; yp++;						//cycle through. All this is used as a REVERSE map, so that the (30,30) pixel in the new (small) image looks at the (30,30) pixel in mapx and mapy, ...
			//... and reads from the original image at points (mapx(30,30),mapy(30,30)). 
		}
	}
	

	 //  cv::imshow("mapdif", mapx - mapy);
}


//=============================================================================


void PAW::WarpToNeutral(Mat &image, Mat &shape, Mat &neutralshape){

	Mat newimage;
	this->unCalcCoeff(shape, neutralshape);
	Mat bigmapx,bigmapy;
	this->unWarpRegion(_mapx,_mapy);
	resize(_mapx, bigmapx, Size(128,128), 0, 0, INTER_LINEAR);
	resize(_mapy, bigmapy, Size(128,128), 0, 0, INTER_LINEAR);
	remap(image,image,bigmapx,bigmapy,CV_INTER_LINEAR);

}



//=============================================================================

void ParseToPAW(cv::Mat shape,cv::Mat localshape, cv::Mat global){
	avatarlocal = localshape;
		savatar = shape;

}

//=============================================================================

void parsecolour(Mat image){
	image.copyTo(colourimage);
}

	


//===========================================================================


void PAW::unWarpRegion(cv::Mat &mapx,cv::Mat &mapy)
{
	assert((mapx.type() == CV_32F) && (mapy.type() == CV_32F));

	if((mapx.rows != _mask.rows) || (mapx.cols != _mask.cols))
		_mapx.create(_mask.rows,_mask.cols,CV_32F);

	if((mapy.rows != _mask.rows) || (mapy.cols != _mask.cols))
		_mapy.create(_mask.rows,_mask.cols,CV_32F);

	int x,y,j,k=-1;
	double yi,xi,xo,yo,*a=NULL,*ap;
	

	//cv::imshow("XmapBefore", mapx);
	//cv::imshow("YmapBefore", mapy);

	cv::MatIterator_<float> xp = mapx.begin<float>();
	cv::MatIterator_<float> yp = mapy.begin<float>();
	cv::MatIterator_<uchar> mp = _mask.begin<uchar>();
	cv::MatIterator_<int>   tp = _tridx.begin<int>();
	//cout << _mask << endl << _mask.type();
	//CV_8U

	

	//	cout << "y min = " << _ymin << ", x min = " << _xmin << endl;



	for(y = 0; y < _mask.rows; y++)
	{
		yi = double(y) + _ymin;

	


		for(x = 0; x < _mask.cols; x++)
		{
			xi = double(x) + _xmin;
			j = *tp;


			if(j == -1)
			{
				*xp = -10000;
				*yp = -10000;
			}
			else{
		
				
				if(j != k)
				{
					a = _coeff.ptr<double>(j);			//a is the pointer to the coefficients
					k = j;
				}  	
				ap = a;									//ap is now the pointer to the coefficients
				xo = *ap++;								//look at the first coefficient (and increment). first coefficient is an x offset
				xo += *ap++ * xi;						//second coefficient is an x scale as a function of x
				*xp = float(xo + *ap++ * yi);			//third coefficient ap(2) is an x scale as a function of y
				yo = *ap++;								//then fourth coefficient ap(3) is a y offset
				yo += *ap++ * xi;						//fifth coeff adds coeff[4]*x to y
				*yp = float(yo + *ap++ * yi);			//final coeff adds coeff[5]*y to y

			

			}
			
			mp++; tp++; xp++; yp++;						//cycle through. All this is used as a REVERSE map, so that the (30,30) pixel in the new (small) image looks at the (30,30) pixel in mapx and mapy, ...
			//... and reads from the original image at points (mapx(30,30),mapy(30,30)). 
		}
	}
	

	 //  cv::imshow("mapdif", mapx - mapy);
}