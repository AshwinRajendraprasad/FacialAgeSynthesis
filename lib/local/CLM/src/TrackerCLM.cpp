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

#include <TrackerCLM.h>

#include <iostream>
#include <string>

#include "highgui.h"

#define db at<double>

#define PI 3.14159265
using namespace CLMTracker;

void TrackerCLM::Read(string name, string override_pdm_loc)
{
  	cout << "Reading the CLM model from: " << name << endl;
	
	ifstream locations(name);
	if(!locations.is_open())
	{
		cout << "Couldn't open the model file, aborting" << endl;
		return;
	}
	string line;
	
	// The other module locations should be defined as relative paths from the main model
	boost::filesystem::path root = boost::filesystem::path(name).parent_path();	

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

		// append to root
		location = (root / location).string();
		if (module.compare("CLM") == 0) 
		{ 
			cout << "Reading the CLM module from: " << location << endl;
			// if location has surrounding " remove them			
			_clm.Read(location, override_pdm_loc);
		}
		else if (module.compare("FaceDetConversion") == 0) 
		{          
			cout << "Reading the Face detection transform module....";
			ifstream conversion(location);

			IO::ReadMat(conversion, transformROIHaar);
			cout << "Done" << endl;

		}
		else if (module.compare("FaceChecker") == 0) // Don't do face checking atm, as a new one needs to be trained
		{            
			cout << "Reading the Face validation transform module....";
			_det_valid.Read(location);
			cout << "Done" << endl;
		}
	}
 
	_shape.create(2*_clm._pdm.NumberOfPoints(), 1, CV_64F);
  
	_success = false;
  	_clm._pdm.Identity(_clm._plocal, _clm._pglobl);
}

