#pragma once

#include <string>
#include <vector>

#ifdef _UNICODE
#define TString std::wstring
#else
#define TString std::string
#endif

#define _TEMP_CURRENT		// テンポラリフォルダをアプリケーションパスに作成

namespace NHCOM
{

//! 
class ComFunc
{
public:
	typedef std::vector<CString> VEC_STRING;

	//! パラメータへの分割 (トークン指定)
	static void SplitParam(CString strID, VEC_STRING& vecKey,CString strToken);
	//! パラメータの連結(トークン指定)
	static CString CombineParam(const VEC_STRING& vecParam,LPCTSTR lpszSplitter);

	//! 新しいテンポラリファイル名取得
	static void GetNewTempFileName(CString& strFile,LPCTSTR lpsz=NULL);
	//! アプリケーションデータパス取得
	static BOOL GetAppDataPath(CString& strPath) ;
	//! パス結合
	static CString GetPathCombine(const CString& strLHS,
							   const CString& strRHS);
	//! テンポラリパス取得
	static BOOL GetTemporaryPath(CString& strPath);

	//! アプリケーションパス取得
	static BOOL GetApplicationPath(CString& strPath);

	//! アプリケーションパス取得
	static CString GetApplicationPath();

	//! パスからディレクトリ名を取得
	static CString GetDirNameAtPath(const CString& strPath);

	//! パス存在判定
	static BOOL IsExistPath(const CString& strPath);
	//! ディレクトリ存在判定
	static BOOL IsExistDirectory(LPCTSTR lpszFiler);
	//! ファイル存在判定
	static BOOL IsExistFile(LPCTSTR lpszFiler);

	//! HOSTNAME -> IPADDRESS
	//static BOOL GetIPAddress(LPCTSTR lpszHostName,std::vector<ULONG>& vecIP);
	//! HTML 取得
	static BOOL GetURLandCreateFile(LPCTSTR lpszURL,LPCTSTR lpszSaveFile);
	//! HTML 取得
	static CString GetURLandCreateTempFile(LPCTSTR lpszURL);

	//! テンポラリファイル削除
	static void CleanupFiles();
	static void CleanupFilesSub(CString strPath);

	//! ＨＴＭＬタグ除去
	static CString RemoveHtmlTag(CString strHtml);

	//! HTMLをテキストに変換
	static CString HTMLConvert(CString strHtml);
	//! ファイル名禁止文字を変換
	static CString FNameConvert(CString strHtml);

	//! SJIS でファイル書き込み
	static BOOL WriteSJISFile(LPCTSTR lpszFile,const CString& strBuff);
	//! SJIS ファイル読込み
	static BOOL ReadSJISFile(LPCTSTR lpszFile,CString& strBuff);

	//! 添付 でファイルに書き込み
	static CString Write2TempFile(const CString& strBuff);

	// エラー文字列取得
	static CString GetLastErrorString(DWORD dwErrorNo, BOOL* pSeikou = NULL);

	//! 文字コード変換 UTF8 -> SJIS
	static BOOL UTF8toSJIS(const std::string& src, std::string& dest);
	//! 文字コード変換 UTF8 -> UNICODE
	static BOOL UTF8toUNICODE(const std::string& src, std::wstring& dest);
	//! 文字コード変換 SJIS -> UTF8
	static BOOL SJIStoUTF8(const std::string& src, std::string& dest);
	//! 文字コード変換 SJIS -> UNICODE
	static BOOL SJIStoUNICODE(const std::string& src, std::wstring& dest);

	//! 文字コード変換 UNICODE -> SJIS
	static BOOL UNICODEtoSJIS(const std::wstring& src, std::string& dest);
	//! 文字コード変換 UNICODE -> UTF8
	static BOOL UNICODEtoUTF8(const std::wstring& src, std::string& dest);

	//! 文字コード変換 EUC -> UNICODE
	static BOOL EUCtoUNICODE(const std::string& src, std::wstring& dest);

};

};