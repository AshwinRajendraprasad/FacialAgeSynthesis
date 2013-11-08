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

//  Parameters of the CLM-Z and CLM trackers
//
//  Tadas Baltrusaitis
//  01/05/2012

#ifndef __CLM_PARAM_WRAP_H
#define __CLM_PARAM_WRAP_H

#include "cv.h"
#include <highgui.h>

#include <vector>
#include <iostream>

#include <filesystem.hpp>
#include <filesystem/fstream.hpp>

using namespace cv;
using namespace std;
using namespace boost::filesystem;

namespace CLMWrapper
{
	//Helper functions for parsing the inputs
	void get_video_input_output_params(vector<string> &input_video_file, vector<string> &depth_dir,
		vector<string> &output_pose_file, vector<string> &output_video_file, vector<string> &output_features_file, vector<string> &arguments);

	void get_camera_params(float &fx, float &fy, float &cx, float &cy, int &dimx, int &dimy, vector<string> &arguments);

	void get_image_input_output_params(vector<string> &input_image_files, vector<string> &input_depth_files, vector<string> &output_feature_files, vector<string> &output_image_files,
		vector<Vec6d> &input_pose_params, vector<string> &arguments);

struct CLMParameters
{

	double HighUncertaintyVal, LowUncertaintyVal;
	Matx66d LowCovariance; // very certain
	Matx66d HighCovariance; // very uncertain

	int nIter;
	double clamp, fTol; 
	bool fcheck;

	vector<int> wSizeSmall;
	vector<int> wSizeInit;
	
	vector<int> wSizeCurrent;

	string defaultModelLoc;

	string override_pdm_loc;

	// offsets from the center of face to region of interest in milimeters (used by the tracker to convert the prior to region of interest estimate
	double XLeftOffset, XRightOffset, YTopOffset, YBottomOffset;

	double scaleToDepthFactor;
	double objectSizeFactor;

	double objectWidth;

	double Xoffset;
	double Yoffset;
	double Zoffset;

	// If there is a separate expression and identity model, should we only fit identiy or is it set (this only makes sense when there is a separate model)
	bool calculate_separate_params;

	// this is used for the smooting of response maps (KDE sigma)
	double sigma;

	double reg_factor;
	double weight_factor; // factor for weighted least squares

	// SVM boundary for face checking
	double decisionBoundary;

	bool limit_pose;

	// should multiple views be considered during reinit
	bool multi_view;

	CLMParameters()
	{
		// initialise the default values
		init();
	}

	// possible parameters are -clm 'file' which specifies the default location of main clm root
	CLMParameters(vector<string> &arguments)
	{
		// initialise the default values
	    init(); 

		bool* valid = new bool[arguments.size()];
		for(size_t i = 0; i < arguments.size(); ++i)
		{
			valid[i] = true;

			if (arguments[i].compare("-mloc") == 0) 
			{                    
				string model_loc = arguments[i + 1];
				defaultModelLoc = model_loc;
				valid[i] = false;
				valid[i+1] = false;
				i++;

			}
			if (arguments[i].compare("-clm_sigma") == 0) 
			{                    
				sigma = stof(arguments[i + 1]);
				valid[i] = false;
				valid[i+1] = false;
				i++;
			}
			else if (arguments[i].compare("-pdm_loc") == 0) 
			{                    
				override_pdm_loc = arguments[i + 1];
				valid[i] = false;
				valid[i+1] = false;
				i++;
			}
			else if(arguments[i].compare("-w_reg") == 0)
			{
				weight_factor = stof(arguments[i + 1]);
				valid[i] = false;
				valid[i+1] = false;
				i++;
			}
			else if(arguments[i].compare("-reg") == 0)
			{
				reg_factor = stof(arguments[i + 1]);
				valid[i] = false;
				valid[i+1] = false;
				i++;
			}
			else if (arguments[i].compare("-multi_view") == 0) 
			{                    
				multi_view = (bool)(stoi(arguments[i + 1]));
				valid[i] = false;
				valid[i+1] = false;
				i++;
			}
			else if(arguments[i].compare("-fcheck") == 0)
			{
				fcheck = (bool)stoi(arguments[i + 1]);
				valid[i] = false;
				valid[i+1] = false;
				i++;
			}
			else if(arguments[i].compare("-n_iter") == 0)
			{
				nIter = stoi(arguments[i + 1]);
				valid[i] = false;
				valid[i+1] = false;
				i++;
			}
			else if(arguments[i].compare("-clmacc") == 0)
			{
				wSizeSmall = vector<int>(3);
				wSizeInit = vector<int>(3);

				// For fast tracking
				wSizeSmall[0] = 11; wSizeSmall[1] = 9; wSizeSmall[2] = 7;
		
				// Just for initialisation
				wSizeInit.at(0) = 15; wSizeInit.at(1) = 11; wSizeInit.at(2) = 11;

				nIter = 10;

				valid[i] = false;
			}
			else if(arguments[i].compare("-clmfast") == 0)
			{
				wSizeSmall = vector<int>(2);
				wSizeInit = vector<int>(3);

				// For fast tracking
				wSizeSmall[0] = 9; wSizeSmall[1] = 7;
		
				// Just for initialisation
				wSizeInit.at(0) = 11; wSizeInit.at(1) = 9; wSizeInit.at(2) = 7;

				nIter = 5;

				valid[i] = false;
			}
			else if (arguments[i].compare("-clmwild") == 0) 
			{                    
				// For in the wild fitting these parameters are suitable
				wSizeInit = vector<int>(3);
				wSizeInit[0] = 15; wSizeInit[1] = 21; wSizeInit[2] = 21;		

				sigma = 2;
				reg_factor = 25;
				weight_factor = 5;
				nIter = 10;

				valid[i] = false;
			}
			else if (arguments[i].compare("-help") == 0)
			{
				cout << "CLM parameters are defined as follows: -mloc <location of model file> -pdm_loc <override pdm location> -w_reg <weight term for patch rel.> -reg <prior regularisation> -clm_sigma <float sigma term> -fcheck <should face checking be done 0/1> -n_iter <num EM iterations> -clmacc/-clwild (type of images expected)" << endl; // Inform the user of how to use the program				
			}
		}

		for(int i=arguments.size()-1; i >= 0; --i)
		{
			if(!valid[i])
			{
				arguments.erase(arguments.begin()+i);
			}
		}

	}

	private:
		void init()
		{
			HighUncertaintyVal = 1000000;
			LowUncertaintyVal = 0.000001;
			LowCovariance = Matx66d::eye() * LowUncertaintyVal;
			HighCovariance = Matx66d::eye() * HighUncertaintyVal;

			// number of iterations that will be performed at each clm scale
			nIter = 5;

			// how many standard deviations from should be clamped for morphology and expression
			clamp = 3;

			// the tolerance for convergence
			fTol = 0.01;

			// using an external face checker based on SVM
			fcheck = true;

			wSizeSmall = vector<int>(2);
			wSizeInit = vector<int>(3);

			// For fast tracking
			wSizeSmall[0] = 9;
			wSizeSmall[1] = 7;

			// Just for initialisation
			wSizeInit.at(0) = 11;
			wSizeInit.at(1) = 9;
			wSizeInit.at(2) = 7;

			wSizeCurrent = wSizeInit;

			defaultModelLoc = "model/main.txt";

			XLeftOffset = 100;
			XRightOffset = 100;
			YTopOffset = 100;
			YBottomOffset = 100;

			objectWidth = 200;

			calculate_separate_params = false;

			sigma = 1.5;
			reg_factor = 25;
			weight_factor = 0; // by default we're not Non-Uniform RLMS (as the weight factor will depend on the dataset and modalities used)

			decisionBoundary = -0.5;

			limit_pose = true;
			multi_view = false;

		}
};

}

#endif // __CLM_PARAM_WRAP_H