#pragma once
#include <opencv2/core.hpp>    // core���W���[���̃w�b�_�[���C���N���[�h
#include <map>
#include "FileOperation.h"
using namespace NHCOM;

namespace SETTING{
	static LPCTSTR STR_SECTION = _T("Setting");
	static LPCTSTR STR_SECTION_TASK = _T("Task");
	static LPCTSTR STR_SECTION_STEP = _T("Step%d");
	static LPCTSTR STR_SECTION_SERVICE_STEP = _T("Service%d");

	static LPCTSTR STR_KEY_TASK_FILE = _T("TaskFile");
	static LPCTSTR STR_KEY_TASK_FILE_INDEX = _T("TaskFileIdx");

	static LPCTSTR STR_KEY_TIME = _T("Time");
	static LPCTSTR STR_KEY_SLEEPR = _T("SleepRatio");
	static LPCTSTR STR_KEY_STEP_INTERVAL = _T("StepInterval");
	static LPCTSTR STR_KEY_SERVICE_INTERVAL = _T("ServiceInterval");

	static LPCTSTR STR_KEY_STEPMAX = _T("StepMax");

	static LPCTSTR STR_KEY_ERRMAX = _T("ErrorMax");

	static LPCTSTR STR_KEY_WNDHOLD = _T("WindowHold");


	static LPCTSTR STR_KEY_REFRESH = _T("Refresh");
	static LPCTSTR STR_KEY_WNDSEARCH = _T("WndSearch");
	static LPCTSTR STR_KEY_CHILDSEARCH = _T("WndChildSearch");

	static LPCTSTR STR_KEY_REBOOT = _T("Reboot");


	static LPCTSTR STR_KEY_NAME = _T("Name");
	static LPCTSTR STR_KEY_TYPE = _T("Type");

	static LPCTSTR STR_KEY_SIMG = _T("SearchImage");
	static LPCTSTR STR_KEY_TH = _T("Threshold");
	static LPCTSTR STR_KEY_OPTION = _T("Option");


}

/**
 * �����f�[�^
 */
struct FINDDATA {
	CRect rc;
	double val;

	//! �d�Ȃ�̌v�Z ���T�C�Y�͓����Ƃ݂Ȃ��Ă̌v�Z
	double calcOverlap(const FINDDATA& target)const
	{
		if (target.rc.bottom < rc.top ||
			target.rc.top > rc.bottom ||
			target.rc.left > rc.right ||
			target.rc.right < rc.left)
		{
			return 0.;
		}

		double ret = 100.;

		if (target.rc.bottom > rc.bottom)
		{
			ret *= (double)abs(rc.top - target.rc.bottom) / (double)rc.Height();
		}
		else
		{
			ret *= (double)abs(rc.bottom - target.rc.top) / (double)rc.Height();
		}

		if (target.rc.right > rc.right)
		{
			ret *= (double)abs(rc.left - target.rc.right) / (double)rc.Width();
		}
		else
		{
			ret *= (double)abs(rc.right - target.rc.left) / (double)rc.Width();
		}
		return ret;
	}
};
typedef std::vector<FINDDATA> VEC_FIND;

/**
 * �摜��������
 */
struct IMAGE_FIND {
	cv::Mat mat;
	VEC_FIND vecFind;
};

/**
 * �^�X�N���
 */
struct TASK_STATE {
	int nIdx;
	int nOffset;
	int nCounter;
	int nPreTime;

	int nStepState;

	int nStepInterval;
	int nServiceInterval;
	/**
	 * �X�e�b�v���Ƃ̎��s�����J�E���g 
	 * �����I�� ClearFail ���Ă΂Ȃ��ƃ��Z�b�g����Ȃ�
	 * ���菈���̎��s�����������ꍇ�Ƀ��t���b�V�������Ɉڍs���邽�߂Ɏg�p����B
	 * �^�C���A�E�g�l�� index=0 �ŊǗ�����
	 */
	std::map<int, int> mapFail;
	int GetFail(int idx) {
		std::map<int, int>::const_iterator iter = mapFail.find(idx);
		if (iter != mapFail.end())return iter->second;
		return 0;
	}
	void AddFail(int idx) {
		mapFail[idx] = GetFail(idx) + 1;
	}
	void ClearFail() {
		mapFail.clear();
	}

	enum {
		STAT_INIT=0,
	};

	TASK_STATE() {
		Clear();
	}

	int Total()const {
		return nOffset + nCounter;
	}
	void Clear() {
		nIdx = 0;
		nOffset = 0;
		nCounter = 0;
		nPreTime = 0;
		nStepState = STAT_INIT;

		nStepInterval = 0;
		nServiceInterval = 0;
	}
	void NextStep(int n=1) {
		nIdx+=n;
		nOffset += nCounter;
		nCounter = 0;
		nStepState = STAT_INIT;
	}
	void BackStep(int n=1) {
		nIdx-=n;
		if (nIdx < 0)
		{
			nIdx = 0;
		}
		// �߂�Ƃ��́ACounter �� Offset �l�ɂ���B
		nOffset += nCounter;
		nCounter = nOffset;
		nOffset = 0;
		nStepState = STAT_INIT;
	}
	void SetStep(int idx) {
		nIdx = idx;
		if (nIdx < 0)
		{
			nIdx = 0;
		}
		nOffset += nCounter;
		nCounter = 0;
		nStepState = STAT_INIT;
	}
	void Add() {
		nCounter++;
		nStepInterval--;
		nServiceInterval--;
		if (nPreTime > 0)nPreTime--;
	}
};

/**
 */
struct TASK_STEP {
	TASK_STEP() :nNumber(0), nOK(0),nNG(0), nErr(0), nTimeOut(0){}
	int nNumber;
	int type;
	CString strName;
	CString strSearchImage;
//	int nResizeBase;
	double dThreshold;
	
	//! �����l�I�v�V�����l
	std::map<CStringA, int> mapOption;
	int GetValue(CStringA key,int nDefault=0)const{
		key.MakeLower();
		std::map<CStringA, int>::const_iterator iter = mapOption.find(key);
		if(iter != mapOption.end())return iter->second;
		return nDefault;
	}
	//! �I�v�V�����l�Ǎ���
	void ReadOption(CString str) {
		mapOption.clear();
		int nIdx = str.Find(_T(","));
		while (nIdx >= 0)
		{
			CString temp = str.Left(nIdx);
			str = str.Mid(nIdx + 1);
			nIdx = str.Find(_T(","));

			ReadOptionSub(temp);
		}
		ReadOptionSub(str);
	}
	void ReadOptionSub(CString value)
	{
		value.Trim();
		int i = value.Find(_T(":"));
		if (i > 0)
		{
			CStringA name(value.Left(i));
			value = value.Mid(i + 1);
			name.MakeLower();
			name.Trim();
			value.Trim();
			mapOption[name] = _ttoi(value);
		}
	}


//	int nParam;
//	int nParam2;
//	int nParam3;
	CString strParam;
	CString strParam2;
	CString strParam3;

	int nOK;
	int nNG;
	int nErr;

	int nStartTime;
	int nTimeOut;	// �^�C���A�E�g�𒴂�����G���[����
};
typedef std::vector<TASK_STEP> VEC_STEP;

// -------------------------------------------------------
/**
 * �^�X�N�f�[�^
 */ 
struct TASK_DATA {
	/**
	 * ���̃^�X�N�̑S�̎��� 
     *  ������������烊�Z�b�g�����
	 */
	int nWholeTime;
	//! �X�e�b�v�C���^�[�o��
	int nInterval;
	//! �A���^�C���A�E�g�G���[�ő��
	int nErrorMax;
	//! ���t���b�V�������C���f�b�N�X
	int nRefresh;
	//! �ċN���܂ł̎���
	int nReboot;
	
	CString strWndSearch;
	CString strChildWndSearch;

	VEC_STEP vecStep;
	void Clear() {
		vecStep.clear();
	}
	BOOL IsExist()const {
		return vecStep.size() ? TRUE : FALSE;
	}
};

struct DELETE_CACHE {
	std::vector<CString> m_vecFile;
	int m_nMax;

	DELETE_CACHE() {
		m_nMax = 100;
	}
	virtual ~DELETE_CACHE() {
		while(m_vecFile.size()>0)
		{
			CFileOperation::DeleteFile(m_vecFile[0]);
			m_vecFile.erase(m_vecFile.begin());
		}
	}
	void Add(LPCTSTR str) {
		if (_tcslen(str) == 0)
		{
			return;
		}
		m_vecFile.push_back(str);

		if((int)m_vecFile.size()>m_nMax)
		{
			CFileOperation::DeleteFile(m_vecFile[0]);
			m_vecFile.erase(m_vecFile.begin());
		}
	}
};