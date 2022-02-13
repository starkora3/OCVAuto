
#include "stdafx.h"
#include "App.h"
#include "MainDlg.h"
#include "FileOperation.h"
#include "ComFunc.h"

#include <opencv2/core.hpp>    // coreモジュールのヘッダーをインクルード
#include <opencv2/highgui.hpp> // highguiモジュールのヘッダーをインクルード
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect.hpp>
//#include <opencv/highgui.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace NHCOM;


int CMainDlg::DoSearch(IMAGE_FIND& ret,
									LPCTSTR src, 
									LPCTSTR target,
									double dThreshold, double dOverlap)
{
	if (CFileOperation::IsExistFile(src) == FALSE ||
		CFileOperation::IsExistFile(target) == FALSE)
	{
		return -1;
	}

	ret.vecFind.clear();
	cv::String file = CStringA(src);
	ret.mat = cv::imread(file);
	cv::String file2 = CStringA(target);
	cv::Mat tpl = cv::imread(file2);

//	cv::Mat gref, gtpl;
//	cv::cvtColor(ret.mat, gref, CV_BGR2GRAY);
//	cv::cvtColor(tpl, gtpl, CV_BGR2GRAY);

	// マッチんぐの結果格納
	cv::Mat res(ret.mat.rows - tpl.rows + 1, ret.mat.cols - tpl.cols + 1, CV_32FC1);
	// GrayScale 比較
	//cv::matchTemplate(gref, gtpl, res, CV_TM_CCOEFF_NORMED);
	// RGB 比較
//	cv::matchTemplate(ret.mat, tpl, res, cv::TM_CCORR_NORMED);
	cv::matchTemplate(ret.mat, tpl, res, cv::TM_CCOEFF_NORMED);
	cv::threshold(res, res, dThreshold, 1., cv::THRESH_TOZERO);

	// 重なり50%以上が既に存在するなら飛ばす
	//double dOverlap = 50.;

	CString temp;
	temp.Format(_T("DoSearch dThreshold=%f,dOverlap=%f"), dThreshold, dOverlap);
	AddLog(temp);
	//VEC_FIND vecFind;
	while (true)
	{
		double minval, maxval;
		cv::Point minloc, maxloc;
		cv::minMaxLoc(res, &minval, &maxval, &minloc, &maxloc);

		if (maxval >= dThreshold)
		{// 境界以上を処理
			double dTemp = maxval*255.;

			FINDDATA dat;
			dat.rc.SetRect(maxloc.x, maxloc.y, maxloc.x + tpl.cols, maxloc.y + tpl.rows);
			dat.val = maxval;

			BOOL bFind = FALSE;
			for (VEC_FIND::const_iterator iter = ret.vecFind.begin(); iter != ret.vecFind.end(); ++iter)
			{
				if (iter->calcOverlap(dat) >= dOverlap)
				{
					bFind = TRUE;
					break;
				}
			}
			// 0 をセット？ これをすることでループが進む
			cv::floodFill(res, maxloc, cv::Scalar(0), 0, cv::Scalar(.1), cv::Scalar(1.));

			if (bFind == FALSE) 
			{
				ret.vecFind.push_back(dat);

				cv::rectangle(
					ret.mat,
					maxloc,
					cv::Point(maxloc.x + tpl.cols, maxloc.y + tpl.rows),
					CV_RGB(0, (int)dTemp, 0), 2
					);

				{
					CStringA val;
					val.Format("%.3f", maxval);
					// テキスト書き込み
					int face[] = { cv::FONT_HERSHEY_SIMPLEX,
						cv::FONT_HERSHEY_PLAIN,
						cv::FONT_HERSHEY_DUPLEX,
						cv::FONT_HERSHEY_COMPLEX,
						cv::FONT_HERSHEY_TRIPLEX,
						cv::FONT_HERSHEY_COMPLEX_SMALL,
						cv::FONT_HERSHEY_SCRIPT_SIMPLEX,
						cv::FONT_HERSHEY_SCRIPT_COMPLEX,
						cv::FONT_ITALIC };

					// 画像，テキスト，位置（左下），フォント，スケール，色，線太さ，種類
					cv::putText(ret.mat,
						(LPCSTR)val,
						cv::Point(maxloc.x, maxloc.y - 3),
						face[0], 0.8,
						cv::Scalar(0, 0, 255),
						2,
						cv::LINE_AA);
				}
			}
		}
		else
		{
			break;
		}
	}
	return (int)ret.vecFind.size();
}