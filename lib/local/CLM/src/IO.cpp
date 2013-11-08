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

#include <IO.h>
#include <stdio.h>

using namespace CLMTracker;

//===========================================================================
void IO::ReadMat(ifstream& s,cv::Mat &M)
{
	// Read in the number of rows, columns and the data type
	int row,col,type;
	
	s >> row >> col >> type;

	M = cv::Mat(row, col, type);
	
	switch(M.type())
	{
		case CV_64FC1: 
		{
			cv::MatIterator_<double> begin_it = M.begin<double>();
			cv::MatIterator_<double> end_it = M.end<double>();
			
			while(begin_it != end_it)
			{
				s >> *begin_it++;
			}
		}
		break;
		case CV_32FC1:
		{
			cv::MatIterator_<float> begin_it = M.begin<float>();
			cv::MatIterator_<float> end_it = M.end<float>();

			while(begin_it != end_it)
			{
				s >> *begin_it++;
			}
		}
		break;
		case CV_32SC1:
		{
			cv::MatIterator_<int> begin_it = M.begin<int>();
			cv::MatIterator_<int> end_it = M.end<int>();
			while(begin_it != end_it)
			{
				s >> *begin_it++;
			}
		}
		break;
		case CV_8UC1:
		{
			cv::MatIterator_<uchar> begin_it = M.begin<uchar>();
			cv::MatIterator_<uchar> end_it = M.end<uchar>();
			while(begin_it != end_it)
			{
				s >> *begin_it++;
			}
		}
		break;
		default:
			printf("ERROR(%s,%d) : Unsupported Matrix type %d!\n", __FILE__,__LINE__,M.type()); abort();


	}
}

void IO::SkipComments(ifstream& stream)
{	
	while(stream.peek() == '#' || stream.peek() == '\n'|| stream.peek() == ' ' || stream.peek() == '\r')
	{
		std::string skipped;
		std::getline(stream, skipped);
	}
}