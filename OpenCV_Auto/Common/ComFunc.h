#pragma once

#include <string>
#include <vector>

#ifdef _UNICODE
#define TString std::wstring
#else
#define TString std::string
#endif

#define _TEMP_CURRENT		// �e���|�����t�H���_���A�v���P�[�V�����p�X�ɍ쐬

namespace NHCOM
{

//! 
class ComFunc
{
public:
	typedef std::vector<CString> VEC_STRING;

	//! �p�����[�^�ւ̕��� (�g�[�N���w��)
	static void SplitParam(CString strID, VEC_STRING& vecKey,CString strToken);
	//! �p�����[�^�̘A��(�g�[�N���w��)
	static CString CombineParam(const VEC_STRING& vecParam,LPCTSTR lpszSplitter);

	//! �V�����e���|�����t�@�C�����擾
	static void GetNewTempFileName(CString& strFile,LPCTSTR lpsz=NULL);
	//! �A�v���P�[�V�����f�[�^�p�X�擾
	static BOOL GetAppDataPath(CString& strPath) ;
	//! �p�X����
	static CString GetPathCombine(const CString& strLHS,
							   const CString& strRHS);
	//! �e���|�����p�X�擾
	static BOOL GetTemporaryPath(CString& strPath);

	//! �A�v���P�[�V�����p�X�擾
	static BOOL GetApplicationPath(CString& strPath);

	//! �A�v���P�[�V�����p�X�擾
	static CString GetApplicationPath();

	//! �p�X����f�B���N�g�������擾
	static CString GetDirNameAtPath(const CString& strPath);

	//! �p�X���ݔ���
	static BOOL IsExistPath(const CString& strPath);
	//! �f�B���N�g�����ݔ���
	static BOOL IsExistDirectory(LPCTSTR lpszFiler);
	//! �t�@�C�����ݔ���
	static BOOL IsExistFile(LPCTSTR lpszFiler);

	//! HOSTNAME -> IPADDRESS
	//static BOOL GetIPAddress(LPCTSTR lpszHostName,std::vector<ULONG>& vecIP);
	//! HTML �擾
	static BOOL GetURLandCreateFile(LPCTSTR lpszURL,LPCTSTR lpszSaveFile);
	//! HTML �擾
	static CString GetURLandCreateTempFile(LPCTSTR lpszURL);

	//! �e���|�����t�@�C���폜
	static void CleanupFiles();
	static void CleanupFilesSub(CString strPath);

	//! �g�s�l�k�^�O����
	static CString RemoveHtmlTag(CString strHtml);

	//! HTML���e�L�X�g�ɕϊ�
	static CString HTMLConvert(CString strHtml);
	//! �t�@�C�����֎~������ϊ�
	static CString FNameConvert(CString strHtml);

	//! SJIS �Ńt�@�C����������
	static BOOL WriteSJISFile(LPCTSTR lpszFile,const CString& strBuff);
	//! SJIS �t�@�C���Ǎ���
	static BOOL ReadSJISFile(LPCTSTR lpszFile,CString& strBuff);

	//! �Y�t �Ńt�@�C���ɏ�������
	static CString Write2TempFile(const CString& strBuff);

	// �G���[������擾
	static CString GetLastErrorString(DWORD dwErrorNo, BOOL* pSeikou = NULL);

	//! �����R�[�h�ϊ� UTF8 -> SJIS
	static BOOL UTF8toSJIS(const std::string& src, std::string& dest);
	//! �����R�[�h�ϊ� UTF8 -> UNICODE
	static BOOL UTF8toUNICODE(const std::string& src, std::wstring& dest);
	//! �����R�[�h�ϊ� SJIS -> UTF8
	static BOOL SJIStoUTF8(const std::string& src, std::string& dest);
	//! �����R�[�h�ϊ� SJIS -> UNICODE
	static BOOL SJIStoUNICODE(const std::string& src, std::wstring& dest);

	//! �����R�[�h�ϊ� UNICODE -> SJIS
	static BOOL UNICODEtoSJIS(const std::wstring& src, std::string& dest);
	//! �����R�[�h�ϊ� UNICODE -> UTF8
	static BOOL UNICODEtoUTF8(const std::wstring& src, std::string& dest);

	//! �����R�[�h�ϊ� EUC -> UNICODE
	static BOOL EUCtoUNICODE(const std::string& src, std::wstring& dest);

};

};