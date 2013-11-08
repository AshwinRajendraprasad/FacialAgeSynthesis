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

#include <PDM.h>
#include <iostream>

using namespace CLMTracker;
//===========================================================================

// Using the XYZ convention R = Rx * Ry * Rz, left-handed positive sign
Matx33d Euler2RotationMatrix(const Vec3d& eulerAngles)
{
    Matx33d rotationMatrix;

	double s1 = sin(eulerAngles[0]);
	double s2 = sin(eulerAngles[1]);
	double s3 = sin(eulerAngles[2]);

	double c1 = cos(eulerAngles[0]);
	double c2 = cos(eulerAngles[1]);
	double c3 = cos(eulerAngles[2]);
  
	rotationMatrix(0,0) = c2 * c3;
	rotationMatrix(0,1) = -c2 *s3;
	rotationMatrix(0,2) = s2;
	rotationMatrix(1,0) = c1 * s3 + c3 * s1 * s2;
	rotationMatrix(1,1) = c1 * c3 - s1 * s2 * s3;
	rotationMatrix(1,2) = -c2 * s1;
	rotationMatrix(2,0) = s1 * s3 - c1 * c3 * s2;
	rotationMatrix(2,1) = c3 * s1 + c1 * s2 * s3;
	rotationMatrix(2,2) = c1 * c2;

	return rotationMatrix;
}

// Using the XYZ convention R = Rx * Ry * Rz, left-handed positive sign
Vec3d RotationMatrix2Euler(const Matx33d& rotationMatrix)
{
	double q0 = sqrt( 1 + rotationMatrix(0,0) + rotationMatrix(1,1) + rotationMatrix(2,2) ) / 2.0;
    double q1 = (rotationMatrix(2,1) - rotationMatrix(1,2)) / (4.0*q0) ;
    double q2 = (rotationMatrix(0,2) - rotationMatrix(2,0)) / (4.0*q0) ;
    double q3 = (rotationMatrix(1,0) - rotationMatrix(0,1)) / (4.0*q0) ;

	double t1 = 2.0 * (q0*q2 + q1*q3);

	double yaw  = asin(2.0 * (q0*q2 + q1*q3));
    double pitch= atan2(2.0 * (q0*q1-q2*q3), q0*q0-q1*q1-q2*q2+q3*q3); 
    double roll = atan2(2.0 * (q0*q3-q1*q2), q0*q0+q1*q1-q2*q2-q3*q3);
    
    return Vec3d(pitch, yaw, roll);
}

Vec3d Euler2AxisAngle(const Vec3d& euler)
{
	Matx33d rotMatrix = Euler2RotationMatrix(euler);
	Vec3d axisAngle;
	cv::Rodrigues(rotMatrix, axisAngle);
	return axisAngle;
}

Vec3d AxisAngle2Euler(const Vec3d& axisAngle)
{
	Matx33d rotationMatrix;
	cv::Rodrigues(axisAngle, rotationMatrix);
	return RotationMatrix2Euler(rotationMatrix);
}

Matx33d AxisAngle2RotationMatrix(const Vec3d& axisAngle)
{
	Matx33d rotationMatrix;
	cv::Rodrigues(axisAngle, rotationMatrix);
    return rotationMatrix;
}

Vec3d RotationMatrix2AxisAngle(const Matx33d& rotationMatrix)
{
	Vec3d axisAngle;
	cv::Rodrigues(rotationMatrix, axisAngle);
    return axisAngle;
}

//=============================================================================
// Orthonormalising the 3x3 rotation matrix
void Orthonormalise(cv::Matx33d &R)
{

  cv::SVD svd(R,cv::SVD::MODIFY_A);
  
  // get the orthogonal matrix from the initial rotation matrix
  cv::Mat_<double> X = svd.u*svd.vt;
  
  // This makes sure that the handedness is preserved and no reflection happened
  // by making sure the determinant is 1 and not -1
  cv::Mat_<double> W = Mat_<double>::eye(3,3); 
  double d = determinant(X);
  W(2,2) = determinant(X);
  Mat Rt = svd.u*W*svd.vt;

  Rt.copyTo(R);

}

//===========================================================================
// Clamping the parameter values to be within a c standard deviations
void PDM::Clamp(cv::Mat &p, double n_sigmas, const Mat& E)
{
	cv::MatConstIterator_<double> e  = E.begin<double>();
	cv::MatIterator_<double> p1 =  p.begin<double>();
	cv::MatIterator_<double> p2 =  p.end<double>();
	double v;

	// go over all parameters
	for(; p1 != p2; ++p1,++e)
	{
		v = n_sigmas*sqrt(*e);
		// if the values is too extreme clamp it
		if(fabs(*p1) > v)
		{
			if(*p1 > 0.0)
			{
				*p1=v;
			}
			else
			{
				*p1=-v;
			}
		}
	}

}
//===========================================================================
// Compute the 3D representation of shape (in object space) using the local parameters
void PDM::CalcShape3D(cv::Mat &s, const Mat& p_local)
{
	s = _M + _V*p_local;
}
//===========================================================================
// Get the 2D shape (in image space) from global and local parameters
void PDM::CalcShape2D(cv::Mat &s, const Mat& p_local, const Mat &pglobl)
{

	int n = _M.rows/3;
	double a=pglobl.at<double>(0,0); // scaling factor
	double x=pglobl.at<double>(4,0); // x offset
	double y=pglobl.at<double>(5,0); // y offset

	// get the rotation matrix from the euler angles
	Vec3d euler(pglobl.at<double>(1,0), pglobl.at<double>(2,0), pglobl.at<double>(3,0));
	Matx33d currRot = Euler2RotationMatrix(euler);
	
	// get the 3D shape of the object
	Mat_<double> Shape_3D = _M + _V*p_local;

	// create the 2D shape matrix
	if((s.rows != _M.rows) || (s.cols = 1))
		s.create(2*n,1,CV_64F);

	// for every vertex
	for(int i = 0; i < n; i++)
	{
		// Transform this using the weak-perspective mapping to 2D from 3D
		s.at<double>(i  ,0) = a * ( currRot(0,0) * Shape_3D.at<double>(i, 0) + currRot(0,1) * Shape_3D.at<double>(i+n  ,0) + currRot(0,2) * Shape_3D.at<double>(i+n*2,0) ) + x;
		s.at<double>(i+n,0) = a * ( currRot(1,0) * Shape_3D.at<double>(i, 0) + currRot(1,1) * Shape_3D.at<double>(i+n  ,0) + currRot(1,2) * Shape_3D.at<double>(i+n*2,0) ) + y;
	}
}

//===========================================================================
// Given the region of interest and a set of local parameters compute the representative CLM model parameters
void PDM::CalcParams(Mat& pGlobal, const Rect& roi, const Mat& p_local, const Vec3d rotation)
{

	// get the shape instance based on local params
	Mat_<double> currentShape(_M.size());

	CalcShape3D(currentShape, p_local);

	// rotate the shape
	Matx33d rotationMatrix = Euler2RotationMatrix(rotation);

	Mat reshaped = currentShape.reshape(1, 3);

	Mat rotatedShape = (Mat(rotationMatrix) * reshaped);

	// Get the width of expected shape
	double minX;
	double maxX;
	cv::minMaxLoc(rotatedShape.row(0), &minX, &maxX);	

	double minY;
	double maxY;
	cv::minMaxLoc(rotatedShape.row(1), &minY, &maxY);

	double width = abs(minX - maxX);
	double height = abs(minY - maxY);

	// As openCV produces a slightly too generous bounding box tighten it
	double scaling = ((roi.width / width) + (roi.height / height)) / 2;
	scaling = scaling * 0.88; // this is determined from experiments on the face detectors

	// The estimate of face center also needs some correction
	double tx = roi.x + roi.width / 2;
	double ty = roi.y + 0.61 * roi.height;

	pGlobal.create(6, 1, CV_64F);

	pGlobal.at<double>(0) = scaling;
	pGlobal.at<double>(1) = rotation[0];
	pGlobal.at<double>(2) = rotation[1];
	pGlobal.at<double>(3) = rotation[2];
	pGlobal.at<double>(4) = tx;
	pGlobal.at<double>(5) = ty;
}

//===========================================================================
// Given an image and model parameters, draw the detected landmarks on that image
void PDM::Draw(cv::Mat img, const Mat& plocal, const Mat& pglobl, Mat_<int>& triangulation, Mat& visibilities)
{

	Mat shape2D;

	CalcShape2D(shape2D, plocal, pglobl);

	// Draw the calculated shape on top of an image
	Draw(img, shape2D, triangulation, visibilities);	

}

//===========================================================================
// Given an image and model parameters, draw the detected landmarks on that image
void PDM::Draw(cv::Mat img, const Mat& shape2D, Mat_<int>& triangulation, Mat& visibilities)
{
	int n = shape2D.rows/2;

	for( int i = 0; i < n; ++i)
	{		
		if(visibilities.at<int>(i))
		{
			Point featurePoint((int)shape2D.at<double>(i), (int)shape2D.at<double>(i +n));

			cv::circle(img, featurePoint, 1, Scalar(0,0,255), 2);
		}
	}
	
}

void PDM::Identity(Mat& p_local, Mat& p_global)
{
	p_local = Mat::zeros(_V.cols,1,CV_64F);

	p_global = (Mat_<double>(6,1) << 1, 0, 0, 0, 0, 0);
  
}


//===========================================================================
// Calculate the PDM's Jacobian over rigid parameters (rotation, translation and scaling), the additional input W represents trust for each of the landmarks and is part of Non-Uniform RLMS 
void PDM::CalcRigidJacob(const Mat& p_local, const Mat& pglobl,cv::Mat &Jacob, const Mat_<double> W, cv::Mat &Jacob_t_w)
{
  
	// number of verts
	int n = _M.rows/3;
  
	double X,Y,Z;

	double s = pglobl.at<double>(0,0);
  	
	// Get the current 3D shape (not affected by global transform)
	this->CalcShape3D(_Shape_3D, p_local);

	 // Get the rotation matrix
	Vec3d euler(pglobl.at<double>(1,0), pglobl.at<double>(2,0), pglobl.at<double>(3,0));
	Matx33d currRot = Euler2RotationMatrix(euler);
	
	double r11 = currRot(0,0);
	double r12 = currRot(0,1);
	double r13 = currRot(0,2);
	double r21 = currRot(1,0);
	double r22 = currRot(1,1);
	double r23 = currRot(1,2);
	double r31 = currRot(2,0);
	double r32 = currRot(2,1);
	double r33 = currRot(2,2);

	cv::MatIterator_<double> Jx = Jacob.begin<double>();
	cv::MatIterator_<double> Jy = Jx + n*6;

	for(int i = 0; i < n; i++)
	{
    
		X = _Shape_3D.at<double>(i,0);
		Y = _Shape_3D.at<double>(i+n,0);
		Z = _Shape_3D.at<double>(i+n*2,0);    
		
		// The rigid jacobian from the axis angle rotation matrix approximation using small angle assumption (R * R')
		// where R' = [1, -wz, wy
		//             wz, 1, -wx
		//             -wy, wx, 1]
		// And this is derived using the small angle assumption on the axis angle rotation matrix parametrisation

		// scaling term
		*Jx++ =  X  * r11 + Y * r12 + Z * r13;
		*Jy++ =  X  * r21 + Y * r22 + Z * r23;
		
		// rotation terms
		*Jx++ = s * (Y * r13 - Z * r12);
		*Jy++ = s * (Y * r23 - Z * r22);
		*Jx++ = -s * (X * r13 - Z * r11);
		*Jy++ = -s * (X * r23 - Z * r21);
		*Jx++ = s * (X * r12 - Y * r11);
		*Jy++ = s * (X * r22 - Y * r21);
		
		// translation terms
		*Jx++ = 1.0;
		*Jy++ = 0.0;
		*Jx++ = 0.0;
		*Jy++ = 1.0;

	}

	Mat Jacob_w = Mat::zeros(Jacob.rows, Jacob.cols, Jacob.type());
	
	Jx =  Jacob.begin<double>();
	Jy =  Jx + n*6;

	cv::MatIterator_<double> Jx_w =  Jacob_w.begin<double>();
	cv::MatIterator_<double> Jy_w =  Jx_w + n*6;

	// Iterate over all Jacobian values and multiply them by the weight in diagonal of W
	for(int i = 0; i < n; i++)
	{
		double w_x = W.at<double>(i, i);
		double w_y = W.at<double>(i+n, i+n);

		for(int j = 0; j < Jacob.cols; ++j)
		{
			*Jx_w++ = *Jx++ * w_x;
			*Jy_w++ = *Jy++ * w_y;
		}		
	}

	Jacob_t_w = Jacob_w.t();
}

//===========================================================================
// Calculate the PDM's Jacobian over all parameters (rigid and non-rigid), the additional input W represents trust for each of the landmarks and is part of Non-Uniform RLMS
void PDM::CalcJacob(const Mat& plocal, const Mat& pglobl, const Mat_<double>& V, cv::Mat &Jacob, const Mat_<double> W, cv::Mat &Jacob_t_w)
{ 
	
	// number of vertices
	int n = V.rows/3;
	// number of non-rigid parameters
	int m = V.cols;
	
	double X,Y,Z;
	Jacob = Mat_<double>(2*n, 6+m);
	
	double s = pglobl.at<double>(0,0);
  	
	this->CalcShape3D(_Shape_3D, plocal);
	
	Vec3d euler(pglobl.at<double>(1,0), pglobl.at<double>(2,0), pglobl.at<double>(3,0));
	Matx33d currRot = Euler2RotationMatrix(euler);
	
	double r11 = currRot(0,0);
	double r12 = currRot(0,1);
	double r13 = currRot(0,2);
	double r21 = currRot(1,0);
	double r22 = currRot(1,1);
	double r23 = currRot(1,2);
	double r31 = currRot(2,0);
	double r32 = currRot(2,1);
	double r33 = currRot(2,2);

	cv::MatIterator_<double> Jx =  Jacob.begin<double>();
	cv::MatIterator_<double> Jy =  Jx + n*(6+m);
	cv::MatConstIterator_<double> Vx =  V.begin();
	cv::MatConstIterator_<double> Vy =  Vx + n*m;
	cv::MatConstIterator_<double> Vz =  Vy + n*m;

	for(int i = 0; i < n; i++)
	{
    
		X = _Shape_3D.at<double>(i,0);
		Y = _Shape_3D.at<double>(i+n,0);
		Z = _Shape_3D.at<double>(i+n*2,0);    
    
		// The rigid jacobian from the axis angle rotation matrix approximation using small angle assumption (R * R')
		// where R' = [1, -wz, wy
		//             wz, 1, -wx
		//             -wy, wx, 1]
		// And this is derived using the small angle assumption on the axis angle rotation matrix parametrisation

		// scaling term
		*Jx++ =  X  * r11 + Y * r12 + Z * r13;
		*Jy++ =  X  * r21 + Y * r22 + Z * r23;
		
		// rotation terms
		*Jx++ = s * (Y * r13 - Z * r12);
		*Jy++ = s * (Y * r23 - Z * r22);
		*Jx++ = -s * (X * r13 - Z * r11);
		*Jy++ = -s * (X * r23 - Z * r21);
		*Jx++ = s * (X * r12 - Y * r11);
		*Jy++ = s * (X * r22 - Y * r21);
		
		// translation terms
		*Jx++ = 1.0;
		*Jy++ = 0.0;
		*Jx++ = 0.0;
		*Jy++ = 1.0;

		for(int j = 0; j < m; j++,++Vx,++Vy,++Vz)
		{
			// How much the change of the non-rigid parameters (when object is rotated) affect 2D motion
			*Jx++ = s*(r11*(*Vx) + r12*(*Vy) + r13*(*Vz));
			*Jy++ = s*(r21*(*Vx) + r22*(*Vy) + r23*(*Vz));
		}
	}	

	// Adding the weights here
	Mat Jacob_w = Jacob.clone();
	
	if(cv::trace(W)[0] != W.rows) 
	{
		Jx =  Jacob.begin<double>();
		Jy =  Jx + n*(6+m);

		cv::MatIterator_<double> Jx_w =  Jacob_w.begin<double>();
		cv::MatIterator_<double> Jy_w =  Jx_w + n*(6+m);

		// Iterate over all Jacobian values and multiply them by the weight in diagonal of W
		for(int i = 0; i < n; i++)
		{
			double w_x = W.at<double>(i, i);
			double w_y = W.at<double>(i+n, i+n);

			for(int j = 0; j < Jacob.cols; ++j)
			{
				*Jx_w++ = *Jx++ * w_x;
				*Jy_w++ = *Jy++ * w_y;
			}
		}
	}
	Jacob_t_w = Jacob_w.t();

}

//===========================================================================
// Updating the parameters
void PDM::CalcReferenceUpdate(cv::Mat &delta_p,cv::Mat &plocal,cv::Mat &pglobl, cv::Mat& V)
{
	pglobl.at<double>(0,0) += delta_p.at<double>(0,0);
	pglobl.at<double>(4,0) += delta_p.at<double>(4,0);
	pglobl.at<double>(5,0) += delta_p.at<double>(5,0);

	// get the original rotation matrix	
	Vec3d eulerGlobal(pglobl.at<double>(1,0),pglobl.at<double>(2,0),pglobl.at<double>(3,0));
	Matx33d R1 = Euler2RotationMatrix(eulerGlobal);

	// construc R' = [1, -wz, wy
	//               wz, 1, -wx
	//               -wy, wx, 1]
	Matx33d R2 = Matx33d::eye();

	R2(1,2) = -1.0*(R2(2,1) = delta_p.at<double>(1,0));
	R2(2,0) = -1.0*(R2(0,2) = delta_p.at<double>(2,0));
	R2(0,1) = -1.0*(R2(1,0) = delta_p.at<double>(3,0));
	
	// Make sure it's orthonormal
	Orthonormalise(R2);
	// Combine rotations
	Matx33d R3 = R1 *R2;

	// Extract euler angle (through axis angle first to make sure it's legal)
	Vec3d axisAngle = RotationMatrix2AxisAngle(R3);	
	Vec3d euler = AxisAngle2Euler(axisAngle);

	pglobl.at<double>(1,0) = euler[0];
	pglobl.at<double>(2,0) = euler[1];
	pglobl.at<double>(3,0) = euler[2];


	// Local parameter update, just simple addition
	if(delta_p.rows > 6)
	{
		plocal += delta_p(cv::Rect(0,6,1,V.cols));
	}
}

void PDM::Read(string location)
{
  	
	ifstream pdmLoc(location);

	IO::SkipComments(pdmLoc);

	// Reading mean values
	IO::ReadMat(pdmLoc,_M);
	
	IO::SkipComments(pdmLoc);

	// Reading principal components
	IO::ReadMat(pdmLoc,_V);
	
	IO::SkipComments(pdmLoc);
	
	// Reading eigenvalues	
	IO::ReadMat(pdmLoc,_E);

}
