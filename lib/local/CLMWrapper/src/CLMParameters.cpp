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

#include <CLMParameters.h>

using namespace CLMWrapper;

// Extracting the following command line arguments -f, -fd, -op, -of, -ov (and possible ordered repetitions)
void CLMWrapper::get_video_input_output_params(vector<string> &input_video_files, vector<string> &depth_dirs,
	vector<string> &output_pose_files, vector<string> &output_video_files, vector<string> &output_features_files, vector<string> &arguments)
{
	bool* valid = new bool[arguments.size()];

	for(size_t i = 0; i < arguments.size(); ++i)
	{
		valid[i] = true;
	}

	string root = "";
	// First check if there is a root argument (so that videos and outputs could be defined more easilly)
	for(size_t i = 0; i < arguments.size(); ++i)
	{
		if (arguments[i].compare("-root") == 0) 
		{                    
			root = arguments[i + 1];
			valid[i] = false;
			valid[i+1] = false;			
			i++;
		}		
	}

	for(size_t i = 0; i < arguments.size(); ++i)
	{
		if (arguments[i].compare("-f") == 0) 
		{                    
			input_video_files.push_back(root + arguments[i + 1]);
			valid[i] = false;
			valid[i+1] = false;			
			i++;
		}		
		else if (arguments[i].compare("-fd") == 0) 
		{                    
			depth_dirs.push_back(root + arguments[i + 1]);
			valid[i] = false;
			valid[i+1] = false;		
			i++;
		} 
		else if (arguments[i].compare("-op") == 0)
		{
			output_pose_files.push_back(root + arguments[i + 1]);
			valid[i] = false;
			valid[i+1] = false;
			i++;
		} 
		else if (arguments[i].compare("-of") == 0)
		{
			output_features_files.push_back(root + arguments[i + 1]);
			valid[i] = false;
			valid[i+1] = false;
			i++;
		} 
		else if (arguments[i].compare("-ov") == 0)
		{
			output_video_files.push_back(root + arguments[i + 1]);
			valid[i] = false;
			valid[i+1] = false;
			i++;
		} 
		else if (arguments[i].compare("-help") == 0)
		{
			cout << "Input output files are defined as: -f <infile> -fd <indepthdir> -op <outpose> -of <outfeatures> -ov <outvideo>\n"; // Inform the user of how to use the program				
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

void CLMWrapper::get_camera_params(float &fx, float &fy, float &cx, float &cy, int &dimx, int &dimy, vector<string> &arguments)
{
	bool* valid = new bool[arguments.size()];

	for(size_t i=0; i < arguments.size(); ++i)
	{
		valid[i] = true;
		if (arguments[i].compare("-fx") == 0) 
		{                    
			fx = stof(arguments[i + 1]);
			valid[i] = false;
			valid[i+1] = false;			
			i++;
		}		
		else if (arguments[i].compare("-fy") == 0) 
		{                    
			fy = stof(arguments[i + 1]);
			valid[i] = false;
			valid[i+1] = false;		
			i++;
		} 
		else if (arguments[i].compare("-cx") == 0)
		{
			cx = stof(arguments[i + 1]);
			valid[i] = false;
			valid[i+1] = false;
			i++;
		} 
		else if (arguments[i].compare("-cy") == 0)
		{
			cy = stof(arguments[i + 1]);
			valid[i] = false;
			valid[i+1] = false;
			i++;
		} 
		else if (arguments[i].compare("-dimx") == 0)
		{
			dimx = stoi(arguments[i + 1]);
			valid[i] = false;
			valid[i+1] = false;
			i++;
		} 
		else if (arguments[i].compare("-dimy") == 0)
		{
			dimy = stoi(arguments[i + 1]);
			valid[i] = false;
			valid[i+1] = false;
			i++;
		} 
		else if (arguments[i].compare("-help") == 0)
		{
			cout << "Camera parameters are defined as: -fx <float focal length x> -fy <float focal length y> -cx <float optical center x> -cy <float optical center y> -dimx <width of image (would be resized to this)> -dimy -dimx <height of image (would be resized to this)>"  << endl; // Inform the user of how to use the program				
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

void CLMWrapper::get_image_input_output_params(vector<string> &input_image_files, vector<string> &input_depth_files, vector<string> &output_feature_files, vector<string> &output_image_files,
		vector<Vec6d> &input_pose_params, vector<string> &arguments)
{
	bool* valid = new bool[arguments.size()];

	vector<Vec2d> ts;
	vector<Vec3d> rs;
	vector<double> as;

	string out_pts_dir, out_img_dir;

	for(size_t i = 0; i < arguments.size(); ++i)
	{
		valid[i] = true;
		if (arguments[i].compare("-f") == 0) 
		{                    
			input_image_files.push_back(arguments[i + 1]);
			valid[i] = false;
			valid[i+1] = false;			
			i++;
		}		
		else if (arguments[i].compare("-fd") == 0) 
		{                    
			input_depth_files.push_back(arguments[i + 1]);
			valid[i] = false;
			valid[i+1] = false;		
			i++;
		}
		else if (arguments[i].compare("-fdir") == 0) 
		{                    

			// this should load the bounding boxes if available
			
			path p (arguments[i+1]);   // p reads clearer than argv[1] in the following code

			try
			{
				if (exists(p))    // does p actually exist?
				{
					if (is_directory(p))      // is p a directory?
					{

						vector<path> v;                                // so we can sort them later

						copy(directory_iterator(p), directory_iterator(), back_inserter(v));

						for (vector<path>::const_iterator it (v.begin()); it != v.end(); ++it)
						{
							if(it->extension().string().compare(".jpg") == 0 || it->extension().string().compare(".png") == 0)
							{
								
								
								input_image_files.push_back(it->string());
								path current = *it;
								path bbox = current.replace_extension("txt");

								// If there is a bounding box file create initialisation there
								if(exists(bbox))
								{

									std::ifstream in_bbox(bbox.string());

									double minX, minY, maxX, maxY;

									in_bbox >> minX >> minY >> maxX >> maxY;

									double width = 144.8524;
									double height =  149.1956;

									double tx = (minX + maxX)/2.0;
									double ty = (minY + maxY)/2.0;

									double a = ((maxX - minX) / width + (maxY - minY)/height)/2.0;

									in_bbox.close();

									input_pose_params.push_back(Vec6d(a, 0,  0, 0, tx, ty));

								}
							}


						}

					}
				}
			}
			catch (const filesystem_error& ex)
			{
				cout << ex.what() << '\n';
			}

			valid[i] = false;
			valid[i+1] = false;		
			i++;
		}
		else if (arguments[i].compare("-ofdir") == 0) 
		{
			out_pts_dir = arguments[i + 1];
			valid[i] = false;
			valid[i+1] = false;
			i++;
		}
		else if (arguments[i].compare("-oidir") == 0) 
		{
			out_img_dir = arguments[i + 1];
			valid[i] = false;
			valid[i+1] = false;
			i++;
		}
		else if (arguments[i].compare("-of") == 0)
		{
			output_feature_files.push_back(arguments[i + 1]);
			valid[i] = false;
			valid[i+1] = false;
			i++;
		} 
		else if (arguments[i].compare("-oi") == 0)
		{
			output_image_files.push_back(arguments[i + 1]);
			valid[i] = false;
			valid[i+1] = false;
			i++;
		} 
		else if (arguments[i].compare("-t") == 0)
		{
			Vec2d t;
			t[0] = stof(arguments[i + 1]);
			t[1] = stof(arguments[i + 2]);
			valid[i] = false;
			valid[i+1] = false;
			valid[i+2] = false;
			ts.push_back(t);
			i = i+2;
		}
		else if (arguments[i].compare("-r") == 0)
		{
			Vec3d r;
			r[0] = stof(arguments[i + 1]);
			r[1] = stof(arguments[i + 2]);
			r[2] = stof(arguments[i + 3]);
			valid[i] = false;
			valid[i+1] = false;
			valid[i+2] = false;
			valid[i+3] = false;
			rs.push_back(r);
			i = i+3;
		}
		else if (arguments[i].compare("-a") == 0)
		{
			double a = stof(arguments[i + 1]);
			as.push_back(a);
			valid[i] = false;
			valid[i+1] = false;
			i++;
		}
		else if (arguments[i].compare("-help") == 0)
		{
			cout << "Input output files are defined as: -f <infile (can have multiple ones)> -fd <indepthdir(can have multiple ones)> -of <outfeatures(can have multiple ones)> -oi <outimages (can have multiple ones)> " << endl; // Inform the user of how to use the program				
		}
	}

	// If any output directories are defined populate them based on image names

	if(!out_img_dir.empty())
	{
		for(size_t i=0; i < input_image_files.size(); ++i)
		{
			path image_loc(input_image_files[i]);

			path fname = image_loc.filename();
			fname = fname.replace_extension("jpg");
			output_image_files.push_back(out_img_dir + "/" + fname.string());
		}
	}

	if(!out_pts_dir.empty())
	{
		for(size_t i=0; i < input_image_files.size(); ++i)
		{
			path image_loc(input_image_files[i]);

			path fname = image_loc.filename();
			fname = fname.replace_extension("pts");
			output_feature_files.push_back(out_pts_dir + "/" + fname.string());
		}
	}

	// If we have any pose specifications convert them to Vec6d
	assert(rs.size() == ts.size() && rs.size() == as.size());
	for(size_t i=0; i < rs.size(); ++i)
	{
		input_pose_params.push_back(Vec6d(as[i], rs[i][0],  rs[i][1], rs[i][2], ts[i][0], ts[i][1]));
	}

	for(int i=arguments.size()-1; i >= 0; --i)
	{
		if(!valid[i])
		{
			arguments.erase(arguments.begin()+i);
		}
	}

}

