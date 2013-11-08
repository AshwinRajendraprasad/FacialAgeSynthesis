#include <CCNF_patch.h>
#include <stdio.h>
#include <iostream>
#include <highgui.h>
#include <Eigen/Dense>
#include <opencv2/core/eigen.hpp>

using Eigen::MatrixXd;

using namespace CLMTracker;

//===========================================================================
void sum2one_ccnf(cv::Mat &M)
{
	double sum = 0;
	int cols = M.cols;
	int rows = M.rows;

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

// This is a modified version of openCV code that allows for precomputed dfts of templates and for precomputed dfts of an image
void crossCorr_m( const cv::Mat& img, cv::Mat& img_dft, const cv::Mat& _templ, map<int, cv::Mat>& _templ_dfts, cv::Mat& corr,
                cv::Size corrsize, int ctype,
                cv::Point anchor, double delta, int borderType )
{
    const double blockScale = 4.5;
    const int minBlockSize = 256;
    std::vector<uchar> buf;

    cv::Mat templ = _templ;
    int depth = img.depth(), cn = img.channels();
    int tdepth = templ.depth(), tcn = templ.channels();
    int cdepth = CV_MAT_DEPTH(ctype), ccn = CV_MAT_CN(ctype);

    CV_Assert( img.dims <= 2 && templ.dims <= 2 && corr.dims <= 2 );

    if( depth != tdepth && tdepth != std::max(CV_32F, depth) )
    {
        _templ.convertTo(templ, std::max(CV_32F, depth));
        tdepth = templ.depth();
    }

	// make sure both of their depths are the same?
    CV_Assert( depth == tdepth || tdepth == CV_32F);
    CV_Assert( corrsize.height <= img.rows + templ.rows - 1 &&
               corrsize.width <= img.cols + templ.cols - 1 );

    CV_Assert( ccn == 1 || delta == 0 );

    corr.create(corrsize, ctype);

    int maxDepth = depth > CV_8S ? CV_64F : std::max(std::max(CV_32F, tdepth), cdepth);
    cv::Size blocksize, dftsize;

    blocksize.width = cvRound(templ.cols*blockScale);
    blocksize.width = std::max( blocksize.width, minBlockSize - templ.cols + 1 );
    blocksize.width = std::min( blocksize.width, corr.cols );
    blocksize.height = cvRound(templ.rows*blockScale);
    blocksize.height = std::max( blocksize.height, minBlockSize - templ.rows + 1 );
    blocksize.height = std::min( blocksize.height, corr.rows );

    dftsize.width = std::max(cv::getOptimalDFTSize(blocksize.width + templ.cols - 1), 2);
    dftsize.height = cv::getOptimalDFTSize(blocksize.height + templ.rows - 1);
    if( dftsize.width <= 0 || dftsize.height <= 0 )
        CV_Error( CV_StsOutOfRange, "the input arrays are too big" );

    // recompute block size
    blocksize.width = dftsize.width - templ.cols + 1;
    blocksize.width = MIN( blocksize.width, corr.cols );
    blocksize.height = dftsize.height - templ.rows + 1;
    blocksize.height = MIN( blocksize.height, corr.rows );

    cv::Mat dftImg( dftsize, maxDepth );

    int i, k, bufSize = 0;
    if( tcn > 1 && tdepth != maxDepth )
        bufSize = templ.cols*templ.rows*CV_ELEM_SIZE(tdepth);

    if( cn > 1 && depth != maxDepth )
        bufSize = std::max( bufSize, (blocksize.width + templ.cols - 1)*
            (blocksize.height + templ.rows - 1)*CV_ELEM_SIZE(depth));

    if( (ccn > 1 || cn > 1) && cdepth != maxDepth )
        bufSize = std::max( bufSize, blocksize.width*blocksize.height*CV_ELEM_SIZE(cdepth));

    buf.resize(bufSize);

	cv::Mat dftTempl( dftsize.height*tcn, dftsize.width, maxDepth );

	// if this has not been precomputer, precompute it, otherwise use it
	if(_templ_dfts.find(dftsize.width) == _templ_dfts.end())
	{

		// compute DFT of each template plane
		for( k = 0; k < tcn; k++ )
		{
			int yofs = k*dftsize.height;
			cv::Mat src = templ;
			cv::Mat dst(dftTempl, cv::Rect(0, yofs, dftsize.width, dftsize.height));
			cv::Mat dst1(dftTempl, cv::Rect(0, yofs, templ.cols, templ.rows));

			if( tcn > 1 )
			{
				src = tdepth == maxDepth ? dst1 : cv::Mat(templ.size(), tdepth, &buf[0]);
				int pairs[] = {k, 0};
				mixChannels(&templ, 1, &src, 1, pairs, 1);
			}

			if( dst1.data != src.data )
				src.convertTo(dst1, dst1.depth());

			if( dst.cols > templ.cols )
			{
				cv::Mat part(dst, cv::Range(0, templ.rows), cv::Range(templ.cols, dst.cols));
				part = cv::Scalar::all(0);
			}
			dft(dst, dst, 0, templ.rows);
		}
		_templ_dfts[dftsize.width] = dftTempl;
	}
	else
	{
		// use the precomputed version
		dftTempl = _templ_dfts.find(dftsize.width)->second;
	}

    int tileCountX = (corr.cols + blocksize.width - 1)/blocksize.width;
    int tileCountY = (corr.rows + blocksize.height - 1)/blocksize.height;
    int tileCount = tileCountX * tileCountY;

    cv::Size wholeSize = img.size();
    cv::Point roiofs(0,0);
    cv::Mat img0 = img;

    if( !(borderType & cv::BORDER_ISOLATED) )
    {
        img.locateROI(wholeSize, roiofs);
        img0.adjustROI(roiofs.y, wholeSize.height-img.rows-roiofs.y,
                       roiofs.x, wholeSize.width-img.cols-roiofs.x);
    }
    borderType |= cv::BORDER_ISOLATED;

    // calculate correlation by blocks
    for( i = 0; i < tileCount; i++ )
    {

        int x = (i%tileCountX)*blocksize.width;
        int y = (i/tileCountX)*blocksize.height;

        cv::Size bsz(std::min(blocksize.width, corr.cols - x),
                 std::min(blocksize.height, corr.rows - y));
        cv::Size dsz(bsz.width + templ.cols - 1, bsz.height + templ.rows - 1);
        int x0 = x - anchor.x + roiofs.x, y0 = y - anchor.y + roiofs.y;
        int x1 = std::max(0, x0), y1 = std::max(0, y0);
        int x2 = std::min(img0.cols, x0 + dsz.width);
        int y2 = std::min(img0.rows, y0 + dsz.height);
        cv::Mat src0(img0, cv::Range(y1, y2), cv::Range(x1, x2));
        cv::Mat dst(dftImg, cv::Rect(0, 0, dsz.width, dsz.height));
        cv::Mat dst1(dftImg, cv::Rect(x1-x0, y1-y0, x2-x1, y2-y1));
        cv::Mat cdst(corr, cv::Rect(x, y, bsz.width, bsz.height));

        for( k = 0; k < cn; k++ )
        {
            cv::Mat src = src0;
            dftImg = cv::Scalar::all(0);

            if( cn > 1 )
            {
                src = depth == maxDepth ? dst1 : cv::Mat(y2-y1, x2-x1, depth, &buf[0]);
                int pairs[] = {k, 0};
                mixChannels(&src0, 1, &src, 1, pairs, 1);
            }

            if( dst1.data != src.data )
                src.convertTo(dst1, dst1.depth());

            if( x2 - x1 < dsz.width || y2 - y1 < dsz.height )
                copyMakeBorder(dst1, dst, y1-y0, dst.rows-dst1.rows-(y1-y0),
                               x1-x0, dst.cols-dst1.cols-(x1-x0), borderType);
			if(img_dft.empty())
			{
				dft( dftImg, dftImg, 0, dsz.height );
				img_dft = dftImg.clone();
			}
			else
			{
				dftImg = img_dft.clone();
			}
			cv::Mat dftTempl1(dftTempl, cv::Rect(0, tcn > 1 ? k*dftsize.height : 0,
                                         dftsize.width, dftsize.height));
            mulSpectrums(dftImg, dftTempl1, dftImg, 0, true);
            dft( dftImg, dftImg, cv::DFT_INVERSE + cv::DFT_SCALE, bsz.height );

            src = dftImg(cv::Rect(0, 0, bsz.width, bsz.height));

            if( ccn > 1 )
            {
                if( cdepth != maxDepth )
                {
                    cv::Mat plane(bsz, cdepth, &buf[0]);
                    src.convertTo(plane, cdepth, 1, delta);
                    src = plane;
                }
                int pairs[] = {0, k};
                mixChannels(&src, 1, &cdst, 1, pairs, 1);
            }
            else
            {
                if( k == 0 )
                    src.convertTo(cdst, cdepth, 1, delta);
                else
                {
                    if( maxDepth != cdepth )
                    {
                        cv::Mat plane(bsz, cdepth, &buf[0]);
                        src.convertTo(plane, cdepth);
                        src = plane;
                    }
                    add(src, cdst, cdst);
                }
            }
        }
    }
}

/*****************************************************************************************/

void matchTemplate_m( cv::InputArray _img, cv::Mat& _img_dft, cv::InputArray _templ, map<int, cv::Mat>& _templ_dfts, cv::OutputArray _result, int method )
{
    CV_Assert( CV_TM_SQDIFF <= method && method <= CV_TM_CCOEFF_NORMED );

    int numType = method == CV_TM_CCORR || method == CV_TM_CCORR_NORMED ? 0 :
                  method == CV_TM_CCOEFF || method == CV_TM_CCOEFF_NORMED ? 1 : 2;
    bool isNormed = method == CV_TM_CCORR_NORMED ||
                    method == CV_TM_SQDIFF_NORMED ||
                    method == CV_TM_CCOEFF_NORMED;

    cv::Mat img = _img.getMat(), templ = _templ.getMat();
    if( img.rows < templ.rows || img.cols < templ.cols )
        std::swap(img, templ);

    CV_Assert( (img.depth() == CV_8U || img.depth() == CV_32F) &&
               img.type() == templ.type() );

    cv::Size corrSize(img.cols - templ.cols + 1, img.rows - templ.rows + 1);
    _result.create(corrSize, CV_32F);
    cv::Mat result = _result.getMat();

    int cn = img.channels();
    crossCorr_m( img, _img_dft, templ, _templ_dfts, result, result.size(), result.type(), cv::Point(0,0), 0, 0);

    if( method == CV_TM_CCORR )
        return;

    double invArea = 1./((double)templ.rows * templ.cols);

    cv::Mat sum, sqsum;
    cv::Scalar templMean, templSdv;
    double *q0 = 0, *q1 = 0, *q2 = 0, *q3 = 0;
    double templNorm = 0, templSum2 = 0;

    if( method == CV_TM_CCOEFF )
    {
        integral(img, sum, CV_64F);
        templMean = mean(templ);
    }
    else
    {
        integral(img, sum, sqsum, CV_64F);
        meanStdDev( templ, templMean, templSdv );

        templNorm = CV_SQR(templSdv[0]) + CV_SQR(templSdv[1]) +
                    CV_SQR(templSdv[2]) + CV_SQR(templSdv[3]);

        if( templNorm < DBL_EPSILON && method == CV_TM_CCOEFF_NORMED )
        {
            result = cv::Scalar::all(1);
            return;
        }

        templSum2 = templNorm +
                     CV_SQR(templMean[0]) + CV_SQR(templMean[1]) +
                     CV_SQR(templMean[2]) + CV_SQR(templMean[3]);

        if( numType != 1 )
        {
            templMean = cv::Scalar::all(0);
            templNorm = templSum2;
        }

        templSum2 /= invArea;
        templNorm = sqrt(templNorm);
        templNorm /= sqrt(invArea); // care of accuracy here

        q0 = (double*)sqsum.data;
        q1 = q0 + templ.cols*cn;
        q2 = (double*)(sqsum.data + templ.rows*sqsum.step);
        q3 = q2 + templ.cols*cn;
    }

    double* p0 = (double*)sum.data;
    double* p1 = p0 + templ.cols*cn;
    double* p2 = (double*)(sum.data + templ.rows*sum.step);
    double* p3 = p2 + templ.cols*cn;

    int sumstep = sum.data ? (int)(sum.step / sizeof(double)) : 0;
    int sqstep = sqsum.data ? (int)(sqsum.step / sizeof(double)) : 0;

    int i, j, k;

    for( i = 0; i < result.rows; i++ )
    {
        float* rrow = (float*)(result.data + i*result.step);
        int idx = i * sumstep;
        int idx2 = i * sqstep;

        for( j = 0; j < result.cols; j++, idx += cn, idx2 += cn )
        {
            double num = rrow[j], t;
            double wndMean2 = 0, wndSum2 = 0;

            if( numType == 1 )
            {
                for( k = 0; k < cn; k++ )
                {
                    t = p0[idx+k] - p1[idx+k] - p2[idx+k] + p3[idx+k];
                    wndMean2 += CV_SQR(t);
                    num -= t*templMean[k];
                }

                wndMean2 *= invArea;
            }

            if( isNormed || numType == 2 )
            {
                for( k = 0; k < cn; k++ )
                {
                    t = q0[idx2+k] - q1[idx2+k] - q2[idx2+k] + q3[idx2+k];
                    wndSum2 += t;
                }

                if( numType == 2 )
                {
                    num = wndSum2 - 2*num + templSum2;
                    num = MAX(num, 0.);
                }
            }

            if( isNormed )
            {
                t = sqrt(MAX(wndSum2 - wndMean2,0))*templNorm;
                if( fabs(num) < t )
                    num /= t;
                else if( fabs(num) < t*1.125 )
                    num = num > 0 ? 1 : -1;
                else
                    num = method != CV_TM_SQDIFF_NORMED ? 0 : 1;
            }

            rrow[j] = (float)num;
        }
    }
}


//===========================================================================
void CCNF_neuron::Read(ifstream &s,bool readType)
{
  if(readType)
  {
	  int type; s >> type;
	  assert(type == IO::PATCH);
  }
  s >> neuron_type >> norm_w >> b >> alpha;
  IO::ReadMat(s, W); 
  W = W.t();

  // Can calculate the dft's of the neuron here

  //std::cout << _W << endl;
  return;
}

//===========================================================================
void CCNF_neuron::Response(cv::Mat &im, cv::Mat &im_dft, cv::Mat &resp)
{
	assert((im.type() == CV_32F) && (resp.type() == CV_64F));
	assert((im.rows>= W.rows) && (im.cols>= W.cols));

	int h = im.rows - W.rows + 1;
	int w = im.cols - W.cols + 1;
	
	// the patch area on which we will calculate reponses
	cv::Mat I;  
  
	if(neuron_type == 3)
	{
		// Perform normalisation across whole patch (ignoring the invalid values indicated by <= 0

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
		if(neuron_type == 0)
		{
			I = im;
		}
		else
		{
			printf("ERROR(%s,%d): Unsupported patch type %d!\n", __FILE__,__LINE__,neuron_type);
			abort();
		}
	}
  
	if(neuron_type == 3)
	{
		// In case of depth we use per area, rather than per patch normalisation
		matchTemplate_m(I, im_dft, W, W_dfts, res_, CV_TM_CCOEFF); // the linear multiplication, efficient calc of response

		// This is the old version that used the openCV code directly, my modification precalculates dft
		//cv::Mat res2;
		//cv::matchTemplate(I, W, res2, CV_TM_CCOEFF); // the linear multiplication, efficient calc of response
		//cout << cv::norm(res_ - res2) << endl;
	}
	else
	{
		matchTemplate_m(I, im_dft, W, W_dfts, res_, CV_TM_CCOEFF_NORMED); // the linear multiplication, efficient calc of response

		// This is the old version that used the openCV code directly, my modification precalculates dft
		//cv::Mat res2;
		//cv::matchTemplate(I, W, res2, CV_TM_CCOEFF_NORMED); // the linear multiplication, efficient calc of response
		//cout << cv::norm(res_ - res2) << endl;
	}

	cv::MatIterator_<double> p = resp.begin<double>(); // output
	cv::MatIterator_<float> q1 = res_.begin<float>(); // respone for each pixel
	cv::MatIterator_<float> q2 = res_.end<float>();

	while(q1 != q2)*p++ = (2 * alpha) * 1.0 /(1.0 + exp( -(*q1++ * norm_w + b ))); // the sigmoid applied to response

	return;
}

//===========================================================================
void CCNF_patch::Read(ifstream &s,bool readType, vector<int> window_sizes, vector<vector<cv::Mat> > sigma_components)
{
	if(readType)
	{
		int type;
		s >> type;
		
		// empty patch due to visibilities
		if(type == 0)
			return;

		assert(type == 5);
	}

	int n;
	s >> w >> h >> n;

	if(n == 0)
	{
		s >> n;
		// empty patch due to visibilities
		return;
	}
	// n - the number of neurons for this patch
	neurons.resize(n);
	for(int i = 0; i < n; i++)
		neurons[i].Read(s);

	int n_sigmas = window_sizes.size();
	this->window_sizes = window_sizes;

	Sigmas.resize(n_sigmas);

	int n_betas = 0;
	vector<double> betas;

	if(n_sigmas > 0)
	{
		n_betas = sigma_components[0].size();

		betas.resize(n_betas);

		for (int i=0; i < n_betas;  ++i)
		{
			s >> betas[i];
		}
	}

	#ifdef _OPENMP
	#pragma omp parallel for
	#endif
	for(int i = 0; i < n_sigmas; i++)
	{

		// calculate the sigmas based on alphas and betas
		float sum_alphas = 0;
		// sum the alphas first
		for(int a = 0; a < n; ++a)
		{
			sum_alphas = sum_alphas + neurons[i].alpha;
		}

		cv::Mat q1 = sum_alphas * cv::Mat_<float>::eye(window_sizes[i]*window_sizes[i], window_sizes[i]*window_sizes[i]);

		cv::Mat q2 = cv::Mat_<float>::zeros(window_sizes[i]*window_sizes[i], window_sizes[i]*window_sizes[i]);
		for (int b=0; b < n_betas; ++b)
		{
			
			q2 = q2 + ((float)betas[b]) *sigma_components[i][b];
		}

		cv::Mat  SigmaInv = 2 * (q1 + q2);
		
		// Eigen is faster in release mode, but OpenCV in debug
		#ifdef _DEBUG
			cv::Mat Sigma_d;
			cv::invert(SigmaInv, Sigma_d, cv::DECOMP_CHOLESKY);
		#else
			//sigma_decomp.
			MatrixXd SigmaInvEig;
			cv::cv2eigen(SigmaInv,SigmaInvEig);
			MatrixXd eye = MatrixXd::Identity(SigmaInv.rows, SigmaInv.cols);
			SigmaInvEig.llt().solveInPlace(eye);		
			cv::Mat Sigma_d;
			cv::eigen2cv(eye, Sigma_d);
		#endif

		//cv::Mat Sigma_d;
		//cv::invert(SigmaInv, Sigma_d, cv::DECOMP_CHOLESKY);

		Sigma_d.convertTo(Sigmas[i], CV_32F);

	}
	// Read the patch confidence
	s >> patch_confidence;
}
//===========================================================================
void CCNF_patch::Response(cv::Mat &im, cv::Mat &d_im, cv::Mat &resp)
{
	
	assert((im.type() == CV_32F) && (resp.type() == CV_64F));
	assert((im.rows >= h) && (im.cols >= w));

	int h_resp = im.rows - h + 1;
	int w_resp = im.cols - w + 1;
	if(resp.rows != h_resp || resp.cols != w_resp)
	{
		resp.create(h_resp, w_resp, CV_64F);
	}

	if(res_.rows != h_resp || res_.cols != w_resp)
	{
		res_.create(h_resp,w_resp, CV_64F);
	}
		
	resp.setTo(0);
	
	// the placeholder for the DFT of the image so it doesn't get recalculated for every response
	cv::Mat im_dft;

	// responses from the neural layers
	for(size_t i = 0; i < neurons.size(); i++)
	{		
		// Do not bother with neuron response if the alpha is tiny and will not contribute much to overall result
		if(neurons[i].alpha > 1e-4)
		{
			neurons[i].Response(im, im_dft, res_);
			resp = resp + res_;						
		}
	}

	int s_to_use = -1;

	// Find the matching sigma
	for(size_t i=0; i < window_sizes.size(); ++i)
	{
		if(window_sizes[i] == h_resp)
		{
			s_to_use = i;
			break;
		}
	}

	cv::Mat resp_vec = resp.reshape(1, h_resp*w_resp);
	cv::Mat_<float> resp_vec_f;
	resp_vec.convertTo(resp_vec_f, CV_32F);

	cv::Mat out = Sigmas[s_to_use] * resp_vec_f;

	resp = out.reshape(1, h_resp);
	resp.convertTo(resp, CV_64F);
	double min;

	cv::minMaxIdx(resp, &min, 0);
	if(min < 0)
	{
		resp = resp - min;
	}

	sum2one_ccnf(resp); 
}
