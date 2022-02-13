#include "stdafx.h"
#include "App.h"
#include "MainDlg.h"
#include "FileOperation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace SETTING;

void CMainDlg::InitLoadSetting()
{
	CString temp;
	
	GetSetting(STR_KEY_TASK_FILE, temp, _T(""));

	m_cmbSelectTask.ResetContent();
	m_vecDataFile.clear();
	int nIdx = 0;
	m_cmbSelectTask.AddString(_T("�f�t�H���g"));

	m_vecDataFile.push_back(DATAFILE());

	m_cmbName.AddString(_T("umamusu1"));
	m_cmbName.AddString(_T("umamusu2"));
	m_cmbName.AddString(_T("Nox2"));
	m_cmbName.AddString(_T("Nox3"));
	m_cmbName.AddString(_T("Nox�}���`"));
	m_cmbName.AddString(_T("bluest"));

	
	GetSetting(STR_WND_NAME_DEFAULT, temp, _T("Nox2"));
	m_cmbName.SetWindowText(temp);

	// �^�X�N�t�@�C��
	CString strTaskFile;
	GetSetting(STR_KEY_TASK_FILE, strTaskFile);
	while (!strTaskFile.IsEmpty())
	{
		int nFind = strTaskFile.Find(_T("|"));
		DATAFILE cData;
		CString path;
		if (nFind >= 0)
		{
			path = strTaskFile.Left(nFind);
			path.Trim();

			strTaskFile = strTaskFile.Mid(nFind +1);
		}
		else
		{
			path = strTaskFile;
			path.Trim();

			strTaskFile.Empty();
		}
		// ���O�擾
		GetTask(STR_KEY_NAME, temp, _T("(None)"), path);

		cData.disp = temp;
		cData.path = path;
		m_vecDataFile.push_back(cData);

		m_cmbSelectTask.AddString(cData.disp);
	}
	GetSetting(STR_KEY_TASK_FILE_INDEX,temp);
	nIdx = _ttoi(temp);
	if (nIdx >=0 && nIdx < m_cmbSelectTask.GetCount())
	{
		m_cmbSelectTask.SetCurSel(nIdx);
		m_cCurDFile = m_vecDataFile[nIdx];
	}
	else
	{
		m_cmbSelectTask.SetCurSel(0);
	}
}


CString CMainDlg::GetSettingFile(LPCTSTR strFile) {
	if (_tcslen(strFile) == 0)
	{
		if (m_strIniFile.IsEmpty())
		{
			TCHAR tcPath[MAX_PATH];
			::ZeroMemory(tcPath, MAX_PATH*sizeof(TCHAR));
			DWORD dwResult = ::GetModuleFileName(NULL, tcPath, MAX_PATH);
			if (dwResult)
			{
				// �t�@�C��������������
				TCHAR tcDrive[_MAX_DRIVE], tcDir[_MAX_DIR], tcFName[_MAX_FNAME];;
				::ZeroMemory(tcDrive, _MAX_DRIVE*sizeof(TCHAR));
				::ZeroMemory(tcDir, _MAX_DIR*sizeof(TCHAR));
				::ZeroMemory(tcFName, _MAX_FNAME*sizeof(TCHAR));
				_tsplitpath_s(tcPath,
					tcDrive, _MAX_DRIVE,
					tcDir, _MAX_DIR,
					tcFName, _MAX_FNAME,
					NULL, 0);

				_stprintf_s(tcPath, MAX_PATH, _T("%s%s%s.ini"), tcDrive, tcDir, tcFName);
				m_strIniFile = tcPath;
			}
		}
		return m_strIniFile;
	}
	else
	{
		if (m_strIniDir.IsEmpty())
		{
			TCHAR tcPath[MAX_PATH];
			::ZeroMemory(tcPath, MAX_PATH*sizeof(TCHAR));
			DWORD dwResult = ::GetModuleFileName(NULL, tcPath, MAX_PATH);
			if (dwResult)
			{
				// �t�@�C��������������
				TCHAR tcDrive[_MAX_DRIVE], tcDir[_MAX_DIR], tcFName[_MAX_FNAME];;
				::ZeroMemory(tcDrive, _MAX_DRIVE*sizeof(TCHAR));
				::ZeroMemory(tcDir, _MAX_DIR*sizeof(TCHAR));
				::ZeroMemory(tcFName, _MAX_FNAME*sizeof(TCHAR));
				_tsplitpath_s(tcPath,
					tcDrive, _MAX_DRIVE,
					tcDir, _MAX_DIR,
					tcFName, _MAX_FNAME,
					NULL, 0);

				_stprintf_s(tcPath, MAX_PATH, _T("%s%s"), tcDrive, tcDir);
				m_strIniDir = tcPath;
			}
		}
		return CString(m_strIniDir + strFile);
	}
}


/**
* �ݒ�
*/
BOOL CMainDlg::SetSetting(
	LPCTSTR key,
	LPCTSTR strValue)
{
	CString strFile = GetSettingFile();

	WritePrivateProfileString(STR_SECTION,
		key,
		strValue,
		strFile);
	return TRUE;
}

/**
* �ݒ�擾
*/
BOOL CMainDlg::GetSetting(
	LPCTSTR key,
	CString& strValue,
	LPCTSTR lpszDefault)
{
	CString strFile = GetSettingFile();

	static const UINT LEN_BUFFER = 8192;
	strValue.Empty();
	GetPrivateProfileString(STR_SECTION,
		key,
		lpszDefault,
		strValue.GetBuffer(LEN_BUFFER),
		LEN_BUFFER,
		strFile);
	strValue.ReleaseBuffer();

	if (strValue.IsEmpty())
	{
		return FALSE;
	}
	return TRUE;
}

/**
* �ݒ�擾
*/
BOOL CMainDlg::GetTask(
	LPCTSTR key,
	CString& strValue,
	LPCTSTR lpszDefault,
	LPCTSTR lpszPath)
{
	CString strFile = (_tcslen(lpszPath)==0)?GetSettingFile(m_cCurDFile.path): GetSettingFile(lpszPath);;

	static const UINT LEN_BUFFER = 8192;
	strValue.Empty();
	GetPrivateProfileString(STR_SECTION_TASK,
		key,
		lpszDefault,
		strValue.GetBuffer(LEN_BUFFER),
		LEN_BUFFER,
		strFile);
	strValue.ReleaseBuffer();

	if (strValue.IsEmpty())
	{
		return FALSE;
	}
	return TRUE;
}

/**
* �ݒ�擾
*/
BOOL CMainDlg::GetStep(
	int nIdx,
	LPCTSTR key,
	CString& strValue,
	LPCTSTR lpszDefault)
{
	return GetStepSub(STR_SECTION_STEP,
									nIdx,key,strValue,lpszDefault);
}
BOOL CMainDlg::GetStepSub(
		LPCTSTR name,
		int nIdx,
		LPCTSTR key,
		CString& strValue,
		LPCTSTR lpszDefault)
{
	CString strSection;
	strSection.Format(name, nIdx);

	CString strFile = GetSettingFile(m_cCurDFile.path);

	static const UINT LEN_BUFFER = 8192;
	strValue.Empty();
	GetPrivateProfileString(strSection,
		key,
		lpszDefault,
		strValue.GetBuffer(LEN_BUFFER),
		LEN_BUFFER,
		strFile);
	strValue.ReleaseBuffer();

	if (strValue.IsEmpty())
	{
		return FALSE;
	}
	return TRUE;
}
/**
* �T�[�r�X�X�e�b�v�擾
*/
BOOL CMainDlg::GetServiceStep(
	int nIdx,
	LPCTSTR key,
	CString& strValue,
	LPCTSTR lpszDefault)
{
	return GetStepSub(STR_SECTION_SERVICE_STEP,
		nIdx, key, strValue, lpszDefault);
}
/**
* �����X�e�b�v�擾
*/
BOOL CMainDlg::GetRescueStep(
	int nIdx,
	LPCTSTR key,
	CString& strValue,
	LPCTSTR lpszDefault)
{
	return GetStepSub(STR_SECTION_RESCUE_STEP,
		nIdx, key, strValue, lpszDefault);
}

LPCTSTR CMainDlg::GetScreenShotPath() {
	if (m_strSavePath.IsEmpty())
	{
		TCHAR tcPath[MAX_PATH];
		::ZeroMemory(tcPath, MAX_PATH*sizeof(TCHAR));
		DWORD dwResult = ::GetModuleFileName(NULL, tcPath, MAX_PATH);
		if (dwResult)
		{
			// �t�@�C��������������
			TCHAR tcDrive[_MAX_DRIVE], tcDir[_MAX_DIR], tcFName[_MAX_FNAME];;
			::ZeroMemory(tcDrive, _MAX_DRIVE*sizeof(TCHAR));
			::ZeroMemory(tcDir, _MAX_DIR*sizeof(TCHAR));
			::ZeroMemory(tcFName, _MAX_FNAME*sizeof(TCHAR));
			_tsplitpath_s(tcPath,
				tcDrive, _MAX_DRIVE,
				tcDir, _MAX_DIR,
				tcFName, _MAX_FNAME,
				NULL, 0);

			_stprintf_s(tcPath, MAX_PATH, _T("%s%sprint"), tcDrive, tcDir);
			m_strSavePath = tcPath;

			CFileOperation::CreateDirectory(m_strSavePath);

			m_strSavePath += _T("\\");
		}
	}
	return m_strSavePath;
}

/**
*  Nox.ini ���� �ݒ�擾
*/
BOOL CMainDlg::GetNoxDevice(
	LPCTSTR key,
	CString& strValue)
{
	CString strFile = GetSettingFile(_T("Nox.ini"));;

	static const UINT LEN_BUFFER = 8192;
	strValue.Empty();
	GetPrivateProfileString(STR_SECTION_DEVICE,
		key,
		_T(""),
		strValue.GetBuffer(LEN_BUFFER),
		LEN_BUFFER,
		strFile);
	strValue.ReleaseBuffer();

	if (strValue.IsEmpty())
	{
		return FALSE;
	}
	return TRUE;
}

//! ADB �f�o�C�X�ݒ�
BOOL CMainDlg::GetADBDevice(
	LPCTSTR search,
	CString& strValue) {

	try {
		CString strFile = CFileOperation::CreateTmpFileInDir(m_strTempDir, _T("dev"));


		CString strCmd;
		strCmd.Format(_T("adb devices > \"%s\""), strFile);

		if (!DoADBCommand(strCmd)) return FALSE;

		CString strBuff;
		if (!CFileOperation::ReadSJISFile(strFile, strBuff)) return FALSE;

		CString searchL = search;
		searchL.MakeLower();

		ComFunc::VEC_STRING vecLine;
		ComFunc::SplitParam(strBuff, vecLine, _T("\n"));
		for (ComFunc::VEC_STRING::const_iterator iter = vecLine.begin();
			iter != vecLine.end(); ++iter) {
			ComFunc::VEC_STRING vecParam;
			ComFunc::SplitParam(*iter, vecParam, _T("\t"));

			if (vecParam.size() == 0)continue;
			CString temp = vecParam[0];
			temp.MakeLower();

			if (temp.Find(searchL) >= 0) {
				strValue = vecParam[0];
				return TRUE;
			}
		}

		// emulator ��D�悷��
		searchL = _T("emulator");
		for (ComFunc::VEC_STRING::const_iterator iter = vecLine.begin();
			iter != vecLine.end(); ++iter) {
			ComFunc::VEC_STRING vecParam;
			ComFunc::SplitParam(*iter, vecParam, _T("\t"));

			if (vecParam.size() == 0)continue;
			CString temp = vecParam[0];
			temp.MakeLower();

			if (temp.Find(searchL) >= 0) {
				strValue = vecParam[0];
				return TRUE;
			}
		}
	}
	catch (...) {

	}
	return FALSE;
}