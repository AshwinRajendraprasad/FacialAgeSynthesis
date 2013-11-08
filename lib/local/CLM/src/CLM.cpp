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

#include <CLM.h>

#include <stdio.h>
#include <iostream>

#include <highgui.h>

#include <filesystem.hpp>
#include <filesystem/fstream.hpp>

#define PI 3.14159265
using namespace CLMTracker;

// Using Kabsch's algorithm for aligning shapes
//This assumes that align_from and align_to are already mean normalised
Matx22d AlignShapesKabsch2D(const Mat_<double>& align_from, const Mat_<double>& align_to )
{

	cv::SVD svd(align_from.t() * align_to);
    
	// make sure no reflection is there
	// corr ensures that we do only rotaitons and not reflections
	double d = cv::determinant(svd.vt.t() * svd.u.t());

	cv::Matx22d corr = cv::Matx22d::eye();
	if(d > 0)
	{
		corr(1,1) = 1;
	}
	else
	{
		corr(1,1) = -1;
	}

    Matx22d R;
	Mat(svd.vt.t()*cv::Mat(corr)*svd.u.t()).copyTo(R);

	return R;
}

//=============================================================================
Matx22d AlignShapesWithScale(cv::Mat_<double>& src, cv::Mat_<double> dst)
{
	int n = src.rows;

	// First we mean normalise both src and dst
	double mean_src_x = cv::mean(src.col(0))[0];
	double mean_src_y = cv::mean(src.col(1))[0];

	double mean_dst_x = cv::mean(dst.col(0))[0];
	double mean_dst_y = cv::mean(dst.col(1))[0];

	Mat_<double> src_mean_normed = src.clone();
	src_mean_normed.col(0) = src_mean_normed.col(0) - mean_src_x;
	src_mean_normed.col(1) = src_mean_normed.col(1) - mean_src_y;

	Mat_<double> dst_mean_normed = dst.clone();
	dst_mean_normed.col(0) = dst_mean_normed.col(0) - mean_dst_x;
	dst_mean_normed.col(1) = dst_mean_normed.col(1) - mean_dst_y;

	// Find the scaling factor of each
	Mat src_sq;
	cv::pow(src_mean_normed, 2, src_sq);

	Mat dst_sq;
	cv::pow(dst_mean_normed, 2, dst_sq);

	double s_src = sqrt(cv::sum(src_sq)[0]/n);
	double s_dst = sqrt(cv::sum(dst_sq)[0]/n);

	src_mean_normed = src_mean_normed / s_src;
	dst_mean_normed = dst_mean_normed / s_dst;

	double s = s_dst / s_src;

	// Get the rotation
	Matx22d R = AlignShapesKabsch2D(src_mean_normed, dst_mean_normed);
		
	Matx22d	A;
	Mat(s * R).copyTo(A);

	Mat_<double> aligned = (Mat(Mat(A) * src.t())).t();
    Mat_<double> offset = dst - aligned;

	double t_x =  cv::mean(offset.col(0))[0];
	double t_y =  cv::mean(offset.col(1))[0];
    
	return A;

}

//=============================================================================
//=============================================================================

void CLM::Read(string clmLocation, string override_pdm_loc)
{
	// n - number of views

	// PDM location 
	
	// Location of modules
	ifstream locations(clmLocation);

	if(!locations.is_open())
	{
		cout << "Couldn't open the CLM model file aborting" << endl;
		cout.flush();
		return;
	}

	string line;
	
	vector<string> colourPatchesLocations;
	vector<string> depthPatchesLocations;
	vector<string> ccnfPatchesLocations;

	// The other module locations should be defined as relative paths from the main model
	boost::filesystem::path root = boost::filesystem::path(clmLocation).parent_path();		

	// The main file contains the references to other files
	while (!locations.eof())
	{ 
		
		getline(locations, line);

		stringstream lineStream(line);

		string module;
		string location;

		// figure out which module is to be read from which file
		lineStream >> module;
		
		getline(lineStream, location);

		if(location.size() > 0)
			location.erase(location.begin()); // remove the first space
				
		// remove carriage return at the end for compatibility with unix systems
		if(location.size() > 0 && location.at(location.size()-1) == '\r')
		{
			location = location.substr(0, location.size()-1);
		}

		// append the lovstion to root location (boost syntax)
		location = (root / location).string();
				
		if (module.compare("PDM") == 0) 
		{            
			cout << "Reading the PDM module from: " << location << "....";
			if(!override_pdm_loc.empty())
			{
				_pdm.Read(override_pdm_loc);
			}
			else
			{
				_pdm.Read(location);
			}
			cout << "Done" << endl;
		}
		else if (module.compare("Triangulations") == 0) 
		{       
			cout << "Reading the Triangulations module from: " << location << "....";
			ifstream triangulationFile(location.c_str());

			IO::SkipComments(triangulationFile);

			int numViews;
			triangulationFile >> numViews;

			// read in the triangulations
			_triangulations.resize(numViews);

			for(int i = 0; i < numViews; ++i)
			{
				IO::SkipComments(triangulationFile);
				IO::ReadMat(triangulationFile, _triangulations[i]);
			}
			cout << "Done" << endl;
		}
		else if(module.compare("PatchesIntensity") == 0)
		{
			colourPatchesLocations.push_back(location);
		}
		else if(module.compare("PatchesDepth") == 0)
		{
			depthPatchesLocations.push_back(location);
		}
		else if(module.compare("PatchesCCNF") == 0)
		{
			ccnfPatchesLocations.push_back(location);
		}
	}
  
	_cent.resize(colourPatchesLocations.size());
	_visi.resize(colourPatchesLocations.size());
	_patch.resize(colourPatchesLocations.size());
	_patchScaling.resize(colourPatchesLocations.size());

	for(size_t i = 0; i < colourPatchesLocations.size(); ++i)
	{
		string location = colourPatchesLocations[i];
		cout << "Reading the intensity SVR patch experts from: " << location << "....";
		ReadPatches(location, _cent[i], _visi[i], _patch[i], _patchScaling[i]);
		cout << "Done" << endl;
	}

	_centDepth.resize(depthPatchesLocations.size());
	_visiDepth.resize(depthPatchesLocations.size());
	_patchDepth.resize(depthPatchesLocations.size());
	_patchScalingDepth.resize(depthPatchesLocations.size());

	for(size_t i = 0; i < depthPatchesLocations.size(); ++i)
	{
		string location = depthPatchesLocations[i];
		cout << "Reading the depth SVR patch experts from: " << location << "....";
		ReadPatches(location, _centDepth[i], _visiDepth[i], _patchDepth[i], _patchScalingDepth[i]);
		cout << "Done" << endl;
	}

	if(!ccnfPatchesLocations.empty())
	{
		_cent.resize(ccnfPatchesLocations.size());
		_visi.resize(ccnfPatchesLocations.size());
		_ccnf_patch.resize(ccnfPatchesLocations.size());
		_patchScaling.resize(ccnfPatchesLocations.size());

		for(size_t i = 0; i < ccnfPatchesLocations.size(); ++i)
		{
			string location = ccnfPatchesLocations[i];
			cout << "Reading the CLNF patch experts from: " << location << "....";
			Read_CCNF_Patches(location, _cent[i], _visi[i], _ccnf_patch[i], _patchScaling[i]);			
			cout << "Done" << endl;
		}
	}
	// Initialising some values

	// local parameters (shape)
	_plocal.create(_pdm.NumberOfModes(), 1);

	// global parameters (pose)
	_pglobl.create(6,1,CV_64F);

	cshape_.create(2*_pdm.NumberOfPoints() ,1, CV_64F);

	// Patch response, jacobians and hessians
	ms_.create(2*_pdm.NumberOfPoints(),1,CV_64F);
	u_.create(6+_pdm.NumberOfModes(),1,CV_64F);
	g_.create(6+_pdm.NumberOfModes(),1,CV_64F);
	
	J_.create(2*_pdm.NumberOfPoints(),6+_pdm.NumberOfModes(), CV_64F);
	J_w_t_.create(6+_pdm.NumberOfModes(), 2*_pdm.NumberOfPoints(), CV_64F);
	H_.create(6+_pdm.NumberOfModes(),6+_pdm.NumberOfModes(), CV_64F);

	prob_.resize(_pdm.NumberOfPoints());
	wmem_.resize(_pdm.NumberOfPoints());

}

void CLM::ReadPatches(string patchesFileLocation, std::vector<cv::Mat>& centers, std::vector<cv::Mat>& visibility, std::vector<std::vector<MultiPatchExpert> >& patches, double& patchScaling)
{

	ifstream patchesFile(patchesFileLocation.c_str());

	if(patchesFile.is_open())
	{
		IO::SkipComments(patchesFile);

		patchesFile >> patchScaling;

		IO::SkipComments(patchesFile);

		int numberViews;		

		patchesFile >> numberViews; 

		// read the visibility
		centers.resize(numberViews);
		visibility.resize(numberViews);
  
		patches.resize(numberViews);

		IO::SkipComments(patchesFile);

		// centers of each view (which view corresponds to which orientation)
		for(size_t i = 0; i < centers.size(); i++)
		{
			IO::ReadMat(patchesFile, centers[i]);		
			centers[i] = centers[i] * PI / 180.0;
			//cout << centers[i];
		}

		IO::SkipComments(patchesFile);

		// the visibility of points for each of the views (which verts are visible at a specific view
		for(size_t i = 0; i < visibility.size(); i++)
		{
			IO::ReadMat(patchesFile, visibility[i]);				
		}

		int numberOfPoints = visibility[0].rows;

		IO::SkipComments(patchesFile);

		// read the patches themselves
		for(size_t i = 0; i < patches.size(); i++)
		{
			// number of patches for each view
			patches[i].resize(numberOfPoints);
			// read in each patch
			for(int j = 0; j < numberOfPoints; j++)
			{
				patches[i][j].Read(patchesFile);
			}
		}
	
	}
	else
	{
		cout << "Can't find/open the patches file" << endl;
	}
}

void CLM::Read_CCNF_Patches(string patchesFileLocation, std::vector<cv::Mat>& centers, std::vector<cv::Mat>& visibility, std::vector<std::vector<CCNF_patch> >& patches, double& patchScaling)
{

	ifstream patchesFile(patchesFileLocation.c_str());

	if(patchesFile.is_open())
	{
		IO::SkipComments(patchesFile);

		patchesFile >> patchScaling;

		IO::SkipComments(patchesFile);

		int numberViews;		

		patchesFile >> numberViews; 

		// read the visibility
		centers.resize(numberViews);
		visibility.resize(numberViews);
  
		patches.resize(numberViews);

		IO::SkipComments(patchesFile);

		// centers of each view (which view corresponds to which orientation)
		for(size_t i = 0; i < centers.size(); i++)
		{
			IO::ReadMat(patchesFile, centers[i]);		
			centers[i] = centers[i] * PI / 180.0;
			//cout << centers[i];
		}

		IO::SkipComments(patchesFile);

		// the visibility of points for each of the views (which verts are visible at a specific view
		for(size_t i = 0; i < visibility.size(); i++)
		{
			IO::ReadMat(patchesFile, visibility[i]);				
		}
		//cout << visibility[0] << endl;
		int numberOfPoints = visibility[0].rows;

		IO::SkipComments(patchesFile);

		// Read the possible SigmaInvs (without beta), this will be followed by patch reading (this assumes all of them have the same type, and number of betas)
		int num_win_sizes;
		int num_sigma_comp;
		patchesFile >> num_win_sizes;
		IO::SkipComments(patchesFile);

		vector<int> windows;
		windows.resize(num_win_sizes);

		vector<vector<cv::Mat>> sigma_components;
		sigma_components.resize(num_win_sizes);

		for (int w=0; w < num_win_sizes; ++w)
		{
			IO::SkipComments(patchesFile);
			patchesFile >> windows[w];			
			IO::SkipComments(patchesFile);
			patchesFile >> num_sigma_comp;

			sigma_components[w].resize(num_sigma_comp);

			for(int s=0; s < num_sigma_comp; ++s)
			{
				IO::ReadMat(patchesFile, sigma_components[w][s]);
			}
		}


		// read the patches themselves
		for(size_t i = 0; i < patches.size(); i++)
		{
			IO::SkipComments(patchesFile);
			// number of patches for each view
			patches[i].resize(numberOfPoints);
			// read in each patch
			for(int j = 0; j < numberOfPoints; j++)
			{
				patches[i][j].Read(patchesFile, true, windows, sigma_components);
			}
		}
	
	}
	else
	{
		cout << "Can't find/open the patches file" << endl;
	}
}

//=============================================================================
// Getting the closest view center based on orientation
int CLM::GetViewIdx(int scale)
{	
	int idx = 0;
	if(this->nViews() == 1)
	{
		return 0;
	}
	else
	{
		int i;
		double v1,v2,v3,d,dbest = -1.0;

		for(i = 0; i < this->nViews(scale); i++)
		{
			v1 = _pglobl.at<double>(1,0) - _cent[scale][i].at<double>(0,0); 
			v2 = _pglobl.at<double>(2,0) - _cent[scale][i].at<double>(1,0);
			v3 = _pglobl.at<double>(3,0) - _cent[scale][i].at<double>(2,0);
			
			d = v1*v1 + v2*v2 + v3*v3;
			if(dbest < 0 || d < dbest)
			{
				dbest = d;
				idx = i;
			}
		}
		return idx;
	}
}

int CLM::GetDepthViewIdx(int scale)
{
	int idx = 0;
	if(this->nViews() == 1)
	{
		return 0;
	}
	else
	{
		int i;
		double v1,v2,v3,d,dbest = -1.0;

		for(i = 0; i < this->nViews(); i++)
		{
			v1 = _pglobl.at<double>(1,0) - _centDepth[scale][i].at<double>(0,0); 
			v2 = _pglobl.at<double>(2,0) - _centDepth[scale][i].at<double>(1,0);
			v3 = _pglobl.at<double>(3,0) - _centDepth[scale][i].at<double>(2,0);
			d = v1*v1 + v2*v2 + v3*v3;
			if(dbest < 0 || d < dbest)
			{
				dbest = d;
				idx = i;
			}
		}
		return idx;
	}
}

//=============================================================================
bool CLM::Fit(const cv::Mat_<uchar>& im, const cv::Mat_<float>& depthImg, const std::vector<int>& wSize, int nIter, double clamp, double fTol, bool morphology, double sigma, double reg_factor, double weight_factor, bool limit_pose)
{
	assert(im.type() == CV_8U);
	

	int n = _pdm.NumberOfPoints(); 
	
	cv::Mat_<uchar> mask;
	cv::Mat_<float> clampedDepth;	

	// Background elimination from the depth image
	if(!depthImg.empty())
	{
		// use the current estimate of the face region to determine what is foreground and background
		double tx = _pglobl.at<double>(4);
		double ty = _pglobl.at<double>(5);

		// if we are too close to the edge fail
		if(tx - 9 <= 0 || ty - 9 <= 0 || tx + 9 >= im.cols || ty + 9 >= im.rows)
		{
			cout << "Face estimate is too close to the edge, tracking failed" << endl;
			return false;
		}

		_pdm.CalcShape2D(cshape_, _plocal, _pglobl);

		double minX, maxX, minY, maxY;

		cv::minMaxLoc(cshape_(Range(0, n),Range(0,1)), &minX, &maxX);
		cv::minMaxLoc(cshape_(Range(n, n*2),Range(0,1)), &minY, &maxY);

		double width = 3 * (maxX - minX); // the area of interest: cshape minX and maxX with some scaling
		double height = 2.5 * (maxY - minY); // These scalings are fairly ad-hoc

		// getting the region of interest from the depth image,
		// so we don't get other objects lying at same depth as head in the image but away from it
		cv::Rect_<int> roi((int)(tx-width/2), (int)(ty - height/2), (int)width, (int)height);

		// clamp it if it does not lie fully in the image
		if(roi.x < 0) roi.x = 0;
		if(roi.y < 0) roi.y = 0;
		if(roi.width + roi.x >= depthImg.cols) roi.x = depthImg.cols - roi.width;
		if(roi.height + roi.y >= depthImg.rows) roi.y = depthImg.rows - roi.height;
		
		if(width > depthImg.cols)
		{
			roi.x = 0; roi.width = depthImg.cols;
		}
		if(height > depthImg.rows)
		{
			roi.y = 0; roi.height = depthImg.rows;
		}

		if(roi.width == 0) roi.width = depthImg.cols;
		if(roi.height == 0) roi.height = depthImg.rows;

		if(roi.x >= depthImg.cols) roi.x = 0;
		if(roi.y >= depthImg.rows) roi.y = 0;

		// can clamp the depth values based on shape
		mask = cv::Mat_<uchar>(depthImg.rows, depthImg.cols, (uchar)0);

		cv::Mat_<uchar> currentFrameMask = depthImg > 0;

		// check if there is any depth near the estimate
		if(sum(currentFrameMask(cv::Rect((int)tx - 8, (int)ty - 8, 16, 16))/255)[0] > 0)
		{
			double Z = cv::mean(depthImg(cv::Rect((int)tx - 8, (int)ty - 8, 16, 16)), currentFrameMask(cv::Rect((int)tx - 8, (int)ty - 8, 16, 16)))[0]; // Z offset from the surface of the face
				
			cv::Mat dRoi = depthImg(roi);

			cv::Mat mRoi = mask(roi);

			cv::inRange(dRoi, Z - 200, Z + 200, mRoi);
			
			mask = mask / 255;
		
			Mat_<float> maskF;
			mask.convertTo(maskF, CV_32F);

			clampedDepth = depthImg.mul(maskF);
		}
		else
		{
			cout << "No depth signal found in foreground, tracking failed" << endl;
			return false;
		}
	}

	double currScale = _pglobl.at<double>(0);

	// Find the closest depth and colour patch scales, and start wIter below, this will make sure that the last iteration is done at the best scale available
	int scale;
	int depthScale;

	double minDist;
	for( size_t i = 0; i < _patchScaling.size(); ++i)
	{
		if(i==0 || std::abs(_patchScaling[i] - currScale) < minDist)
		{
			minDist = std::abs(_patchScaling[i] - currScale);
			scale = i + 1;
		}

	}
	
	for( size_t i = 0; i < _patchScalingDepth.size(); ++i)
	{
		if(i == 0 || std::abs(_patchScalingDepth[i] - currScale) < minDist)
		{
			minDist = std::abs(_patchScalingDepth[i] - currScale);
			depthScale = i + 1;
		}

	}

	scale = scale - wSize.size();
	depthScale = depthScale - wSize.size();

	if(scale < 0)
		scale = 0;

	if(depthScale < 0)
		depthScale = 0;

	int numColScales = _patchScaling.size();
	int numDepthScales = _patchScalingDepth.size();

	// Go over the max number of iterations for optimisation
	for(size_t witer = 0; witer < wSize.size(); witer++)
	{

		_pdm.CalcShape2D(cshape_, _plocal, _pglobl);		
		
		int view_id = this->GetViewIdx(scale);		
		
		cv::Mat referenceShape;
		
		// Initialise the reference shape on which we'll be warping
		cv::Mat_<double> globalRef(6,1, 0.0);
		globalRef.at<double>(0) = _patchScaling[scale];

		_pdm.CalcShape2D(referenceShape, _plocal, globalRef);
		
		// similarity and inverse similarity	
		Mat_<double> reference_shape_2D = (referenceShape.reshape(1, 2).t());
		Mat_<double> image_shape_2D = cshape_.reshape(1, 2).t();

		Matx22d sim_img_to_ref = AlignShapesWithScale(image_shape_2D, reference_shape_2D);
		Matx22d sim_ref_to_img = sim_img_to_ref.inv(DECOMP_LU);

		double a1 = sim_ref_to_img(0,0);
		double b1 = -sim_ref_to_img(0,1);
		
		// for visualisation
		bool visi = false;

#ifdef _OPENMP
#pragma omp parallel for
#endif
		// calculate the patch responses for every vertex
		for(int i = 0; i < n; i++)
		{
			
			if(_visi[scale][view_id].rows == n)
			{
				if(_visi[scale][view_id].at<int>(i,0) == 0)
				{
					continue;
				}
			}

			int w;
			int h;
			if(!_ccnf_patch.empty())
			{
				w = wSize[witer]+_ccnf_patch[scale][view_id][i].w - 1; 
				h = wSize[witer]+_ccnf_patch[scale][view_id][i].h - 1;				
			}
			else
			{
				w = wSize[witer]+_patch[scale][view_id][i]._w - 1; 
				h = wSize[witer]+_patch[scale][view_id][i]._h - 1;
			}

			if((w>wmem_[i].cols) || (h>wmem_[i].rows))
			{
				wmem_[i].create(h,w,CV_32F);
			}

			// map matrix for quadrangle extraction - scale and rotate to mean shape
			cv::Mat sim = (cv::Mat_<float>(2,3) << a1,-b1, cshape_.at<double>(i,0), b1,a1,cshape_.at<double>(i+n,0));

			// TODO this needs to move to the proper c++ implementation
			cv::Mat_<float> wimg = wmem_[i](cv::Rect(0,0,w,h));
			CvMat wimg_o = wimg;

			CvMat sim_o = sim;

			IplImage im_o = im;
			
			cvGetQuadrangleSubPix(&im_o,&wimg_o,&sim_o);
			
			// get the correct size response window
			prob_[i] = Mat_<double>(wSize[witer], wSize[witer]);

			if(!_ccnf_patch.empty())
			{
				_ccnf_patch[scale][view_id][i].Response(wimg, wimg, prob_[i]);
			}
			else
			{
				_patch[scale][view_id][i].Response(wimg, prob_[i]);
			}
			bool visiResp = false;

			if(visiResp)
			{
				cv::Mat window;
				cv::pyrUp(wimg, window);
				cv::imshow("quadrangle", window / 255.0);

				double minD;
				double maxD;

				cv::minMaxLoc(prob_[i], &minD, &maxD);
				Mat visiProb;
				cv::pyrUp( prob_[i]/maxD, visiProb);
				cv::imshow("col resp", visiProb);		
				cv::waitKey(0);
			}

			// if we have a corresponding depth patch, and we are confident in it, also don't use depth for final iteration as it's not reliable enough it seems
			if(!depthImg.empty() && _visiDepth[depthScale][view_id].at<int>(i,0) && witer != wSize.size() - 1)
			{

				cv::Mat dProb = prob_[i].clone();
				cv::Mat depthWindow(h,w, clampedDepth.type());

				CvMat dimg_o = depthWindow;
				cv::Mat maskWindow(h,w, CV_32F);
				CvMat mimg_o = maskWindow;
				IplImage d_o = clampedDepth;
				IplImage m_o = mask;

				cvGetQuadrangleSubPix(&d_o,&dimg_o,&sim_o);

				
				cvGetQuadrangleSubPix(&m_o,&mimg_o,&sim_o);

				depthWindow.setTo(0, maskWindow < 1);

				_patchDepth[depthScale][view_id][i].ResponseDepth(depthWindow, dProb);
							
				prob_[i] = prob_[i] + dProb;

				// Sum to one
				double sum = 0; int cols = prob_[i].cols, rows = prob_[i].rows;
				if(prob_[i].isContinuous()){cols *= rows;rows = 1;}
				for(int x = 0; x < rows; x++){
					const double* Mi = prob_[i].ptr<double>(x);
					for(int y = 0; y < cols; y++)
					{
						sum += *Mi++;
					}
				}
				// To avoid division by 0 issues
				if(sum == 0)
				{
					sum = 1;
				}


				prob_[i] /= sum;

				if(visiResp)
				{
					double minD;
					double maxD;

					cv::minMaxLoc(dProb, &minD, &maxD);
					Mat visiProb;
					cv::pyrUp( dProb/maxD, visiProb);
					cv::imshow("Depth resp", visiProb);	

					cv::minMaxLoc(prob_[i], &minD, &maxD);
					cv::pyrUp( prob_[i]/maxD, visiProb);
					cv::imshow("Comb resp", visiProb);		

					//cv::waitKey(100);	
				}

			}
		}

		// the actual optimisation step

		// rigid pose optimisation
		this->Optimize(_pglobl, _plocal, _pglobl.clone(), _plocal.clone(), cshape_, sim_img_to_ref, sim_ref_to_img, wSize[witer], view_id, nIter, fTol, clamp, true, scale, sigma, reg_factor, weight_factor);

		// non-rigid optimisation
		this->_likelihood = this->Optimize(_pglobl, _plocal, _pglobl.clone(), _plocal.clone(), cshape_, sim_img_to_ref, sim_ref_to_img, wSize[witer], view_id, nIter, fTol, clamp, false, scale, sigma, reg_factor, weight_factor);

		double currA = _pglobl.at<double>(0);
		
		// do not let the pose get out of hand

		if(limit_pose)
		{
			if(_pglobl.at<double>(1) > PI / 2)
				_pglobl.at<double>(1) = PI/2;
			if(_pglobl.at<double>(1) < -PI / 2)
				_pglobl.at<double>(1) = -PI/2;
			if(_pglobl.at<double>(2) > PI / 2)
				_pglobl.at<double>(2) = PI/2;
			if(_pglobl.at<double>(2) < -PI / 2)
				_pglobl.at<double>(2) = -PI/2;
			if(_pglobl.at<double>(3) > PI / 2)
				_pglobl.at<double>(3) = PI/2;
			if(_pglobl.at<double>(3) < -PI / 2)
				_pglobl.at<double>(3) = -PI/2;
		}

		// If there are more scales to go, and we don't need to upscale too much move to next scale level
		if(scale < numColScales - 1 && 0.9 * _patchScaling[scale] < currA)
		{
			scale++;			
		}

		if(depthScale < numDepthScales - 1 && 0.9 * _patchScalingDepth[depthScale] < currA)
		{
			depthScale++;
		}

		// Can't track very small images reliably (less than ~30px across)
		if(_pglobl.at<double>(0) < 0.2)
		{
			cout << "Detection too small for CLM" << endl;
			return false;
		}		
	}

	return true;
}

void CLM::VectorisedMeanShift(Mat_<double>& meanShifts, const Mat_<double> &iis, const Mat_<double> &jjs, const Mat_<double> &dxs, const Mat_<double> &dys, const Size patchSize, double sigma, int scale, int view_id)
{
	meanShifts = Mat_<double>(dxs.rows, 2, (double)0);

	double a = -0.5 / (sigma * sigma);

	#ifdef _OPENMP
		#pragma omp parallel for
	#endif
	for(int i=0; i < dxs.rows; ++i)
	{
		if(_visi[scale][view_id].at<int>(i,0) == 0  ||
			_patch[scale][view_id][i]._p[0]._confidence < 0.1 ||
			cv::sum(prob_[i])[0] == 0 ||
			dxs.at<double>(i) < 0 ||
			dxs.at<double>(i) > patchSize.width ||
			dys.at<double>(i) < 0 ||
			dys.at<double>(i) > patchSize.height)
			continue;

		// prob_[i] = patch responses
		Mat_<double> patch_resp = prob_[i].reshape(1, 1);

		Mat_<double> vxs = iis - dxs.at<double>(i);
		Mat_<double> vys = jjs - dys.at<double>(i);
		
		Mat vxs2;
		Mat vys2;

		cv::pow(vxs, 2, vxs2);
		cv::pow(vys, 2, vys2);

		Mat gauss_resp;
		cv::exp(a * (vxs2 + vys2), gauss_resp);

		Mat_<double> vs = patch_resp.mul(gauss_resp);
	
		Mat_<double> mxss = vs.mul(iis);
		Mat_<double> myss = vs.mul(jjs);
    
		// this part is caluclating sum(K(x_i - dx)*x_i)
		double mxs = cv::sum(mxss)[0];
		double mys = cv::sum(myss)[0];
    
		double sumVs = cv::sum(vs)[0];
		if(sumVs == 0)
			sumVs = 1;

		double msx = mxs / sumVs - dxs.at<double>(i);
		double msy = mys / sumVs - dys.at<double>(i);
    
		// if we are sampling outside the range ignore
	
		meanShifts.at<double>(i, 0) = msx;
		meanShifts.at<double>(i, 1) = msy;

	}

}

void CLM::NonVectorisedMeanShift(Mat& ms_, const Mat_<double> &dxs, const Mat_<double> &dys, int resp_size, double a, int scale, int view_id)
{
	
	int n = dxs.rows;
	
	// for every point (patch) basically calculating v
	#ifdef _OPENMP
	#pragma omp parallel for
	#endif
	for(int i = 0; i < n; i++)
	{

		if(_visi[scale][view_id].at<int>(i,0) == 0  || cv::sum(prob_[i])[0] == 0)
		{
			ms_.at<double>(i,0) = 0;
			ms_.at<double>(i+n,0) = 0;
			continue;
		}
		double dx = dxs.at<double>(i);
		double dy = dys.at<double>(i);

		int ii,jj;
		double v,vx,vy,mx=0.0,my=0.0,sum=0.0;

		// Iterate over the patch responses here
		cv::MatIterator_<double> p = prob_[i].begin<double>();
			
		for(ii = 0; ii < resp_size; ii++)
		{
			vx = (dy-ii)*(dy-ii);
			for(jj = 0; jj < resp_size; jj++)
			{
				vy = (dx-jj)*(dx-jj);

				// the probability at the current, xi, yi
				v = *p++;

				// the KDE evaluation of that point
				v *= exp(a*(vx+vy));

				sum += v;

				// mean shift in x and y
				mx += v*jj;
				my += v*ii; 

				// for visualisation
				//meanShiftPatch.at<float>(ii,jj) = v;
			}
		}
		// setting the actual mean shift update, sigma is hardcoded it seems (actually could experiment with it slightly, as scale has changed)

		double msx = (mx/sum - dx);
		double msy = (my/sum - dy);

		ms_.at<double>(i,0) = msx;
		ms_.at<double>(i+n,0) = msy;
			
	}
}

//=============================================================================
double CLM::Optimize(Mat& final_global, Mat_<double>& final_local, const Mat_<double>& initial_global, const Mat_<double>& initial_local,
	const Mat_<double>& baseShape, const Matx22d& sim_img_to_ref, const Matx22d& sim_ref_to_img, int resp_size, int view_id,
	int nIter, double fTol,double clamp, bool rigid, int scale, double sigma, double reg_factor, double weight_factor)
{
	
	int n=_pdm.NumberOfPoints();  
	
	// Mean, eigenvalues, eigenvectors
	Mat_<double> M = this->_pdm._M;
	Mat_<double> E = this->_pdm._E;
	Mat_<double> V = this->_pdm._V;

	int m = V.cols;

	cv::Mat u,g;
	
	// Jacobian, and transposed weighted jacobian
	cv::Mat J,J_w_t;

	// Hessian approximation
	cv::Mat H; 
	
	Mat_<double> current_global = initial_global.clone();
	Mat_<double> current_local = initial_local.clone();

	Mat_<double> current_shape;
	Mat_<double> previous_shape;

	// Jacobian and Hessian placeholders
	if(rigid)
	{
		u = u_(cv::Rect(0,0,1,6));
		g = g_(cv::Rect(0,0,1,6)); 
		J = J_(cv::Rect(0,0,6,2*n));
		J_w_t = J_w_t_(cv::Rect(0,0,2*n,6));
		H = H_(cv::Rect(0,0,6,6));
	}
	else
	{
		u = u_;
		g = g_;
		J = J_;
		J_w_t = J_w_t_;
		H = H_;
	}

	// Pre-calculate the regularisation term
	Mat_<double> regTerm;
		 
	if(rigid)
	{
		regTerm = Mat_<double>::zeros(6,6);
	}
	else
	{
		Mat_<double> regularisations = Mat_<double>::zeros(1, 6 + m);

		// Setting the regularisation to the inverse of eigenvalues
		Mat(reg_factor / E).copyTo(regularisations(cv::Rect(6, 0, m, 1)));			
		regTerm = Mat::diag(regularisations.t());
	}	

	// Precalculate generalised Tikhonov regularisation term
	// For generalised Tikhonov
	Mat_<double> P;

	if(weight_factor > 0)
	{
		P = Mat_<double>::zeros(n*2, n*2);

		for (int p=0; p < n; p++)
		{
			if(!_ccnf_patch.empty())
			{

				// for the x dimension
				P.at<double>(p,p) = P.at<double>(p,p)  + _ccnf_patch[scale][view_id][p].patch_confidence;
				
				// for they y dimension
				P.at<double>(p+n,p+n) = P.at<double>(p,p);

			}
			else
			{
				for(size_t pc=0; pc < _patch[scale][view_id][p]._p.size(); pc++)
				{
					// for the x dimension
					P.at<double>(p,p) = P.at<double>(p,p)  + _patch[scale][view_id][p]._p.at(pc)._confidence;
				}	
				// for the y dimension
				P.at<double>(p+n,p+n) = P.at<double>(p,p);
			}
		}
		P = weight_factor * P;
	}
	else
	{
		P = Mat_<double>::eye(n*2, n*2);
	}

	Mat_<double> dxs, dys;

	// Number of iterations
	for(int iter = 0; iter < nIter; iter++)
	{
		//double currPatchScaling = currentGlobal.at<double>(0) / _patchScaling[scale];

		// get the current estimates of x
		_pdm.CalcShape2D(current_shape, current_local, current_global);
		
		if(iter > 0)
		{
			// if the shape hasn't changed terminate
			if(cv::norm(current_shape, previous_shape) < fTol)
				break;
		}

		current_shape.copyTo(previous_shape);

		// purely for visualisation
		Mat_<float> meanShiftPatch(resp_size, resp_size);		

		// calculate the appropriate Jacobians in 2D, even though the actual behaviour is in 3D, using small angle approximation and oriented shape
		if(rigid)
		{
			_pdm.CalcRigidJacob(current_local, current_global, J, P, J_w_t);
		}
		else
		{
			_pdm.CalcJacob(current_local, current_global, V, J, P, J_w_t);
		}
		
		// useful for mean shift calculation
		double a = -0.5/(sigma*sigma);

		Mat_<double> current_shape_2D = current_shape.reshape(1, 2).t();
		Mat_<double> base_shape_2D = baseShape.reshape(1, 2).t();
		Mat_<double> offsets = (current_shape_2D - base_shape_2D) * Mat(sim_img_to_ref).t();
		//cout << offsets << endl;
		dxs = offsets.col(0) + (resp_size-1)/2;
		dys = offsets.col(1) + (resp_size-1)/2;

		NonVectorisedMeanShift(ms_, dxs, dys, resp_size, a, scale, view_id);

		// Now transform the mean shifts to the reference frame of the image, as opposed to one of ref shape
		Mat_<double> mean_shifts_2D = (ms_.reshape(1, 2)).t();
		
		mean_shifts_2D = mean_shifts_2D * Mat(sim_ref_to_img).t();
		ms_ = Mat(mean_shifts_2D.t()).reshape(1, n*2);
		
		// remove non-visible observations
		for(int i = 0; i < n; ++i)
		{
			if(_visi[scale][view_id].rows == n)
			{
				// if patch unavailable for current index
				if(_visi[scale][view_id].at<int>(i,0) == 0 || cv::sum(prob_[i])[0] == 0)
				{				
					cv::Mat Jx = J.row(i);
					Jx = cvScalar(0);
					cv::Mat Jy = J.row(i+n);
					Jy = cvScalar(0);
					ms_.at<double>(i,0) = 0.0;
					ms_.at<double>(i+n,0) = 0.0;
				}
			}
		}

		// projection of the meanshifts onto the jacobians (using the weighted Jacobian, see Baltrusaitis 2013)
		g = J_w_t * ms_;
		if(!rigid)
		{
			g(cv::Rect(0,6,1, m)) = g(cv::Rect(0,6,1, m)) - regTerm(cv::Rect(6,6, m, m)) * current_local;
		}
		// calculating the Hessian approximation		
		H = J_w_t * J;

		// Add the Tikhonov regularisation
		H = H + regTerm;

		// as u is a subset of u_, this solving only applies to the update actually being performed (rigid vs. non rigid)
		// the actual solution for delta p, eq (36) in Saragih 2011  
		cv::solve(H,g,u,CV_CHOLESKY);
		
		// update the reference
		_pdm.CalcReferenceUpdate(u, current_local, current_global, V);		
		

		// clamp to the local parameters for valid expressions
		if(!rigid)
		{
			_pdm.Clamp(current_local, clamp, E);		
		}


	}

	// compute the log likelihood

	double loglhood = 0;

	for(int i = 0; i < n; i++)
	{

		if(_visi[scale][view_id].at<int>(i,0) == 0 )
		{
			continue;
		}
		double dx = dxs.at<double>(i);
		double dy = dys.at<double>(i);

		int ii,jj;
		double v,vx,vy,sum=0.0;

		// Iterate over the patch responses here
		cv::MatIterator_<double> p = prob_[i].begin<double>();
			
		for(ii = 0; ii < resp_size; ii++)
		{
			vx = (dy-ii)*(dy-ii);
			for(jj = 0; jj < resp_size; jj++)
			{
				vy = (dx-jj)*(dx-jj);

				// the probability at the current, xi, yi
				v = *p++;

				// the KDE evaluation of that point
				v *= exp(-0.5*(vx+vy)/(sigma*sigma));

				sum += v;
			}
		}

		// the offset is there for numerical stability
		loglhood += log(sum + 1e-8);

	}	
	loglhood = loglhood/sum(_visi[scale][view_id])[0];

	final_global = current_global;
	final_local = current_local;

	return loglhood;

}
