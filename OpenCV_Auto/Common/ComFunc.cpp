
#include "stdafx.h"
#include "ComFunc.h"
#include <shlobj.h>
#include <Shlwapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>

LPCTSTR STR_WQUOTE = _T("\"");

LPCTSTR lpszDefaultTemp = _T("Tmp");
LPCTSTR lpszDefaultSavePath = _T("Temporary");

using namespace NHCOM;
/**
 * 文字コード変換 UTF8 -> SJIS
 *
 * @return 成功or失敗
 */
BOOL ComFunc::UTF8toSJIS(const std::string& src,		/**<[in] 入力文字列*/
				std::string& dest			/**<[out] 出力文字列*/)
{
	BOOL bRet = FALSE;
	LPWSTR lpWideString=NULL;
	LPSTR lpString =NULL;
	try
	{
		do
		{// UTF8からUnicodeに変換した場合の長さを求める。
			int iWideLength = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), (int)strlen(src.c_str()), NULL, 0);
			if (iWideLength == 0)
			{
				break;
			}
			lpWideString = new WCHAR[iWideLength+1];
			::ZeroMemory(lpWideString,sizeof(WCHAR)*(iWideLength+1));
			// UTF8からUnicodeに変換する。
			int nRet = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), (int)strlen(src.c_str()), lpWideString, iWideLength);
			if(nRet ==0)
			{
				break;
			}

			// UnicodeからShift_JISに変換した場合の長さを求める。
			int iLength = ::WideCharToMultiByte(CP_ACP, 0, lpWideString, iWideLength, NULL, 0, NULL, NULL);
			if(iLength ==0)
			{
				break;
			}

			lpString = new char[iLength+1];
			::ZeroMemory(lpString,iLength+1);
			// UnicodeからShift_JISに変換する
			nRet = ::WideCharToMultiByte(CP_ACP, 0, lpWideString, iWideLength, lpString, iLength, NULL, NULL);
			if(nRet ==0)
			{
				break;
			}

			dest = lpString;
			bRet = TRUE;
		}while(0);
	}
	catch(std::exception& /*e*/)
	{
	}

	if(lpWideString)
	{
		delete [] lpWideString;
	}
	if(lpString)
	{
		delete [] lpString;
	}
	return bRet;
}


/**
 * 文字コード変換 UTF8 -> UNICODE
 *
 * @return 成功or失敗
 */
BOOL ComFunc::UTF8toUNICODE(const std::string& src,	/**<[in] 入力文字列*/
				   std::wstring& dest		/**<[out] 出力文字列*/	)
{
	BOOL bRet = FALSE;
	LPWSTR lpWideString=NULL;
	try
	{
		do
		{
			// UTF8からUnicodeに変換した場合の長さを求める。
			int iWideLength = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), (int)src.length(), NULL, 0);
			if (iWideLength == 0)
			{
				break;
			}
			lpWideString = new WCHAR[iWideLength+1];
			::ZeroMemory(lpWideString,sizeof(WCHAR)*(iWideLength+1));

			// UTF8からUnicodeに変換する。
			int nRet = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), (int)src.length(), lpWideString, iWideLength);
			if(nRet ==0)
			{
				break;
			}

			dest = lpWideString;
			bRet = TRUE;
		}while(0);
		// 変換に成功
	}
	catch(std::exception& /*e*/)
	{
	}
	if(lpWideString)
	{
		delete [] lpWideString;
	}

	return bRet;
}

/**
 * 文字コード変換 UNICODE -> SJIS
 *
 * @return 成功or失敗
 */
BOOL ComFunc::UNICODEtoSJIS(const std::wstring& src, /**<[in] 入力文字列*/
				   std::string& dest		/**<[out] 出力文字列*/)
{
	BOOL bRet = FALSE;
	LPSTR lpString =NULL;
	try
	{
		do
		{
			// UnicodeからShift_JISに変換した場合の長さを求める。
			int iLength = (int)::WideCharToMultiByte(CP_ACP, 0,  src.c_str(), (int)src.length(), NULL, 0, NULL, NULL);
			if(iLength ==0)
			{
				break;
			}

			lpString = new char[iLength+1];
			::ZeroMemory(lpString,iLength+1);
			// UnicodeからShift_JISに変換する
			int nRet = (int)::WideCharToMultiByte(CP_ACP, 0,  src.c_str(), (int)src.length(), lpString, iLength, NULL, NULL);
			if(nRet ==0)
			{
				break;
			}

			dest = lpString;
			bRet = TRUE;
		}while(0);
	}
	catch(std::exception& /*e*/)
	{
	}

	if(lpString)
	{
		delete [] lpString;
	}
	return bRet;
}

/**
 * 文字コード変換 UNICODE -> UTF8
 *
 * @return 成功or失敗
 */
BOOL ComFunc::UNICODEtoUTF8(const std::wstring& src, /**<[in] 入力文字列*/
				   std::string& dest		/**<[out] 出力文字列*/)
{
	BOOL bRet = FALSE;
	LPSTR lpString =NULL;
	try
	{
		do
		{
			// UnicodeからUTF8に変換した場合の長さを求める。
			int iLength = (int)::WideCharToMultiByte(CP_UTF8, 0,  src.c_str(), (int)src.length(), NULL, 0, NULL, NULL);
			if(iLength ==0)
			{
				break;
			}

			lpString = new char[iLength+1];
			::ZeroMemory(lpString,iLength+1);
			// UnicodeからUTF8に変換する
			int nRet = (int)::WideCharToMultiByte(CP_UTF8, 0,  src.c_str(), (int)src.length(), lpString, iLength, NULL, NULL);
			if(nRet ==0)
			{
				break;
			}

			dest = lpString;
			bRet = TRUE;
		}while(0);
	}
	catch(std::exception& /*e*/)
	{
	}

	if(lpString)
	{
		delete [] lpString;
	}
	return bRet;
}


/**
 * 文字コード変換 SJIS -> UTF8
 *
 * @return 成功or失敗
 */
BOOL ComFunc::SJIStoUTF8(const std::string& src, /**<[in] 入力文字列*/
				std::string& dest		/**<[out] 出力文字列*/)
{
	BOOL bRet = FALSE;
	LPWSTR lpWideString=NULL;
	LPSTR lpString =NULL;
	try
	{
		do
		{
			// SJISからUnicodeに変換した場合の長さを求める。
			int iWideLength = ::MultiByteToWideChar(CP_ACP, 0, src.c_str(), (int)src.length(), NULL, 0);
			if (iWideLength == 0)
			{
				break;
			}
			lpWideString = new WCHAR[iWideLength+1];
			::ZeroMemory(lpWideString,sizeof(WCHAR)*(iWideLength+1));

			// SJISからUnicodeに変換する。
			int nRet = ::MultiByteToWideChar(CP_ACP, 0, src.c_str(), (int)src.length(), lpWideString, iWideLength);
			if(nRet ==0)
			{
				break;
			}

			// UnicodeからUTF8に変換した場合の長さを求める。
			int iLength = ::WideCharToMultiByte(CP_UTF8, 0, lpWideString, iWideLength, NULL, 0, NULL, NULL);
			if(iLength ==0)
			{
				break;
			}

			lpString = new char[iLength+1];
			::ZeroMemory(lpString,iLength+1);
			// UnicodeからShift_JISに変換する
			nRet = ::WideCharToMultiByte(CP_UTF8, 0, lpWideString, iWideLength, lpString, iLength, NULL, NULL);
			if(nRet ==0)
			{
				break;
				
			}
			dest = lpString;
			bRet = TRUE;
		}while(0);
	}
	catch(std::exception& /*e*/)
	{
	}

	if(lpWideString)
	{
		delete [] lpWideString;
	}
	if(lpString)
	{
		delete [] lpString;
	}
	return bRet;
}

/**
 * 文字コード変換 SJIS -> UNICODE
 *
 * @return 成功or失敗
 */
BOOL ComFunc::SJIStoUNICODE(const std::string& src, /**<[in] 入力文字列*/
				   std::wstring& dest	/**<[out] 出力文字列*/	)
{
	BOOL bRet = FALSE;
	LPWSTR lpWideString=NULL;
	try
	{
		do
		{
			// UTF8からUnicodeに変換した場合の長さを求める。
			int iWideLength = ::MultiByteToWideChar(CP_ACP, 0, src.c_str(), (int)src.length(), NULL, 0);
			if (iWideLength == 0)
			{
				break;
			}
			lpWideString = new WCHAR[iWideLength+1];
			::ZeroMemory(lpWideString,sizeof(WCHAR)*(iWideLength+1));

			// UTF8からUnicodeに変換する。
			int nRet = ::MultiByteToWideChar(CP_ACP, 0, src.c_str(), (int)src.length(), lpWideString, iWideLength);
			if(nRet ==0)
			{
				break;
			}

			dest = lpWideString;
			bRet = TRUE;
		}while(0);
		// 変換に成功
	}
	catch(std::exception& /*e*/)
	{
	}
	if(lpWideString)
	{
		delete [] lpWideString;
	}

	return bRet;
}


/***********************************************************
* パラメータへの分割 (トークン指定)
***********************************************************/
void ComFunc::SplitParam(CString strBuff, VEC_STRING& vecParam,CString strToken)
{
	vecParam.clear();

	for(int nIdx = strBuff.Find(strToken);
			nIdx>=0;nIdx= strBuff.Find(strToken))
	{
		vecParam.push_back(strBuff.Left(nIdx));
		strBuff = strBuff.Mid(nIdx + strToken.GetLength());
	}
	vecParam.push_back(strBuff);
/*	LPTSTR tcDummy=NULL;
	LPTSTR tcParam = _tcstok_s(strID.GetBuffer(strID.GetLength()),strToken,&tcDummy);
	while(tcParam != NULL)
	{
		vecKey.push_back(tcParam);
		tcParam = _tcstok_s(NULL,strToken,&tcDummy);
	}

	strID.ReleaseBuffer();*/
}

//! キーを連結したIDの取得
CString ComFunc::CombineParam(const VEC_STRING& vecParam,LPCTSTR lpszSplitter)
{
	if( vecParam.size())
	{
		CString strParam=vecParam[0];
		for(UINT i=1;i<vecParam.size();i++)
		{
			CString strTemp;
			strTemp.Format(_T("%s%s%s"),strParam,lpszSplitter,vecParam[i]);
			strParam = strTemp;
		}
		return strParam;
	}
	return _T("");
}

// ===============================================================================
// HOSTNAME -> IPADDRESS
// ===============================================================================
/*
BOOL ComFunc::GetIPAddress(LPCTSTR lpszHostName,std::vector<ULONG>& vecIP)
{
	if(_tcslen(lpszHostName)==0)
	{
		return FALSE;
	}
	vecIP.clear();

	WSAData data;
	WSAStartup(MAKEWORD(2,0), &data);

#ifdef _UNICODE
	std::wstring strUnicodeTemp = (LPWSTR)lpszHostName;
	std::string strSJisTemp;

	if( UNICODEtoSJIS(strUnicodeTemp,strSJisTemp) ==FALSE )
	{
		return NULL;
	}

	struct hostent* pHostent = gethostbyname(strSJisTemp.c_str());

	if (pHostent ==NULL) 
	{
		 return FALSE;
	}

#else
	struct hostent* pHostent = gethostbyname(lpszHostName);
	if (pHostent ==NULL) 
	{
		 return FALSE;
	}
#endif

	for(int i = 0; pHostent->h_addr_list[i]; i++)
	{
		TRACE(_T("☆ %s = %s \n"),lpszHostName,inet_ntoa(*(reinterpret_cast<struct in_addr *>(pHostent->h_addr_list[i]))));
		vecIP.push_back(reinterpret_cast<struct in_addr *>(pHostent->h_addr_list[i])->S_un.S_addr);
	}
	TRACE(_T("\n"));


	WSACleanup();

	return TRUE;
}
*/
// ===============================================================================
// URLをテンポラリファイルに取得
// ===============================================================================
CString ComFunc::GetURLandCreateTempFile(LPCTSTR lpszURL)
{
	CString strFile;
	GetNewTempFileName(strFile);
	if( strFile.IsEmpty())
	{
		return NULL;
	}

	if( GetURLandCreateFile(lpszURL,strFile) )
	{
		return strFile;
	}
	return NULL;
}


// ===============================================================================
// HTML 取得
// ===============================================================================
BOOL ComFunc::GetURLandCreateFile(LPCTSTR lpszURL,LPCTSTR lpszSaveFile)
{
	HRESULT hResult = URLDownloadToFile(NULL,
											lpszURL,
											lpszSaveFile,
											0,
											NULL);
	if( S_OK == hResult )
	{
		return TRUE;
	}
	return FALSE;
}

/**
 * テンポラリファイル削除
 */
void ComFunc::CleanupFiles()
{
	CString strDelPath;
	if( ComFunc::GetTemporaryPath(strDelPath))
	{
		ASSERT( !strDelPath.IsEmpty());
		if( !strDelPath.IsEmpty())
		{
			CleanupFilesSub(strDelPath);
		}
	}
}

/**
 *
 */
void ComFunc::CleanupFilesSub(CString strDelPath)
{
	ASSERT(!strDelPath.IsEmpty());
	if(strDelPath.IsEmpty())
	{
		return;
	}
	CString strDelFind = ComFunc::GetPathCombine(strDelPath,_T("*"));
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strDelFind);
	while(bWorking)
	{
		bWorking = finder.FindNextFile();
		CString strFName=finder.GetFileName();
		if( strFName != _T(".") &&
			strFName != _T("..") &&
			!strFName.IsEmpty())
		{
			if( finder.IsDirectory())
			{// 再帰的に
				CString strDelFolder = ComFunc::GetPathCombine(strDelPath,strFName);
				CleanupFilesSub(strDelFolder);
				if( ::RemoveDirectory(strDelFolder) )
				{
				}
			}
			else
			{
				CString strDelFile = ComFunc::GetPathCombine(strDelPath,strFName);
				if( ::DeleteFile(strDelFile) )
				{
					//TRACE(_T(" ☆ Delete File : %s\n"),strDelFile);
				}
				else
				{
					//TRACE(_T(" ★ Delete Error : %s\n"),strDelFile);
				}
			}
		}
	}
}

/**
 * タグの削除
 */
CString ComFunc::RemoveHtmlTag(CString strHtml)
{
	static LPCTSTR strLT = _T("<");
	static LPCTSTR strGT = _T(">");

	int nIndex = strHtml.Find(strLT);
	while(nIndex >=0)
	{
		int nEnd = strHtml.Find(strGT,nIndex+1);
		if(nEnd < 0)
		{
			break;
		}
		strHtml= strHtml.Left(nIndex) + strHtml.Mid(nEnd+1);
		nIndex = strHtml.Find(strLT);
	}

	return strHtml;
}

/**
 * 新しいテンポラリファイル名取得
 */
void ComFunc::GetNewTempFileName(CString& strFile,LPCTSTR lpsz)
{
	CString strTemp(lpsz);
	if( strTemp.IsEmpty())
	{
		strTemp=lpszDefaultTemp;
	}

	//--------------------------------------------------
	// 一時ファイル格納フォルダ取得
	//--------------------------------------------------
	CString strTempDir;
	GetTemporaryPath(strTempDir);

	// ユニークなファイル名を持つテンポラリファイルを作成する
	if(!::GetTempFileName(strTempDir, 
							strTemp, 
							0, 
							strFile.GetBuffer(MAX_PATH)))
	{
	}
	strFile.ReleaseBuffer();
}

//! アプリケーションパス取得
CString ComFunc::GetApplicationPath()
{
	CString str;
	GetApplicationPath(str);
	return str;
}

/**
 * テンポラリファイルパス取得
 */
BOOL ComFunc::GetTemporaryPath(CString& strPath)
{
	strPath.Empty();

	BOOL bResult = FALSE;
#ifdef _TEMP_CURRENT
	bResult = GetApplicationPath(strPath);
	if( bResult)
	{
		strPath = GetPathCombine(strPath , _T("temp"));
		if(!IsExistPath(strPath))
		{
			if(!CreateDirectory(strPath,NULL))
			{
				TRACE("%s(%d): %s\n", __FILE__, __LINE__, "GetAppDataPath - ディレクトリ作成失敗");
				return FALSE;
			}
		}
	}
#else
	bResult = ::SHGetSpecialFolderPath(NULL, strPath.GetBuffer(MAX_PATH), CSIDL_APPDATA, FALSE);
	strPath.ReleaseBuffer();
	if(!bResult)
	{
		TRACE("%s(%d): %s\n", __FILE__, __LINE__, "GetAppDataPath - error");
		return FALSE;
	}

	// モジュール名取得
	CString strAppName;
	{
		CString strAppPath;
		DWORD dwResult = ::GetModuleFileName(NULL, strAppPath.GetBuffer(MAX_PATH), MAX_PATH);
		strAppPath.ReleaseBuffer();

		_tsplitpath(strAppPath,NULL,NULL,strAppName.GetBuffer(_MAX_FNAME),NULL);
		strAppName.ReleaseBuffer();
		if( strAppName.IsEmpty())
		{
			strAppName = lpszDefaultSavePath;
		}
	}

	strPath = GetPathCombine(strPath, strAppName);

	// アプリケーションデータフォルダが存在しない場合は作成する
	if(!IsExistPath(strPath))
	{
		if(!CreateDirectory(strPath,NULL))
		{
			TRACE("%s(%d): %s\n", __FILE__, __LINE__, "GetAppDataPath - ディレクトリ作成失敗");
			return FALSE;
		}
	}
#endif

	return TRUE;
}

/**
 * パス連結
 */
CString ComFunc::GetPathCombine(const CString& strLHS,
							   const CString& strRHS)
{
	CString strResult;

#ifdef _DEBUG
	// strLHSの末尾が '.' や '..' の場合は危険なので
	// 一応チェックしておく。
	if(!strLHS.IsEmpty())
	{
		ASSERT(strLHS[strLHS.GetLength()-1] != '.');
	}
#endif // _DEBUG

#ifdef _UNICODE
	PathCombine((LPWSTR)strResult.GetBuffer(MAX_PATH),(LPCWSTR)(LPCTSTR)strLHS, (LPCWSTR)(LPCTSTR)strRHS);
#else
	PathCombine(strResult.GetBuffer(MAX_PATH),strLHS, strRHS);
#endif
	strResult.ReleaseBuffer();

	return strResult;
}


// =================================================================================
// ファイルの存在確認
// =================================================================================
BOOL ComFunc::IsExistFile(LPCTSTR lpszFiler)
{ 
    CFileFind FileFind;
    
    // すべてのファイルを検索
    if(!FileFind.FindFile(lpszFiler)) 
	{
        return FALSE; 
	}

	FileFind.FindNextFile();
	if(FileFind.IsDirectory()) 
	{
		return FALSE;
	}
	return TRUE;
}


// =================================================================================
// ファイルの存在確認
// =================================================================================
BOOL ComFunc::IsExistDirectory(LPCTSTR lpszFiler)
{ 
    CFileFind FileFind;
    
    // すべてのファイルを検索
    if(!FileFind.FindFile(lpszFiler)) 
	{
        return FALSE; 
	}

	FileFind.FindNextFile();
	if(FileFind.IsDirectory()) 
	{
		return TRUE;
	}
	return FALSE;
}

// =================================================================================
// 
// =================================================================================
BOOL ComFunc::IsExistPath(const CString& strPath)
{
    CFileFind FileFind;
    
    // すべてのファイルを検索
    if(!FileFind.FindFile(strPath)) 
	{
        return FALSE; 
	}


	return TRUE;
}

// =================================================================================
// 
// =================================================================================
CString ComFunc::HTMLConvert(CString strHtml)
{
	strHtml.Replace(_T("&lt;"),_T("＜"));
	strHtml.Replace(_T("&gt;"),_T("＞"));

	strHtml.Replace(_T("&amp;"),_T("＆"));
	strHtml.Replace(_T("&quot;"),_T("\""));
	strHtml.Replace(_T("&nbsp;"),_T(" "));


	return strHtml;
}

// =================================================================================
// 
// =================================================================================
CString  ComFunc::FNameConvert(CString strFile)
{
	strFile.Replace(_T("\r"),_T(""));
	strFile.Replace(_T("\n"),_T(""));
	strFile.Replace(_T("\t"),_T(""));
	strFile.Replace(_T("\\"),_T("￥"));
	strFile.Replace(_T("/"),_T("／"));
	strFile.Replace(_T(":"),_T("："));
	strFile.Replace(_T("*"),_T("＊"));
	strFile.Replace(_T("?"),_T("？"));
	strFile.Replace(_T("\""),_T("”"));
	strFile.Replace(_T("<"),_T("＜"));
	strFile.Replace(_T(">"),_T("＞"));
	strFile.Replace(_T("|"),_T("｜"));
	return strFile;
}

//! アプリケーションパス取得
BOOL ComFunc::GetApplicationPath(CString& strPath)
{
	strPath.Empty();

	// 実行モジュールパス取得
	DWORD dwResult = ::GetModuleFileName(NULL, strPath.GetBuffer(MAX_PATH), MAX_PATH);
	strPath.ReleaseBuffer();
	if(dwResult == 0)
	{
		ASSERT(FALSE);
		strPath.Empty();
		return FALSE;
	}

	// ファイル名を除去する
	strPath = GetDirNameAtPath(strPath);

	return TRUE;
}

/**
 * パスからディレクトリ名を取得
 *
 * パスからディレクトリ名を取得します。
 *
 * @return	ディレクトリ名（パス）
 */
CString ComFunc::GetDirNameAtPath(const CString& strPath /**< 対象パス */)
{
	ASSERT(!strPath.IsEmpty());

	CString strDrive;								// ドライブ
	CString strDir;									// ディレクトリ

	// 分解
	::_tsplitpath_s(strPath,
				 strDrive.GetBuffer(_MAX_DRIVE),_MAX_DRIVE,
				 strDir.GetBuffer(_MAX_DIR),_MAX_DIR,
				 NULL,0,
				 NULL,0);
	strDrive.ReleaseBuffer();
	strDir.ReleaseBuffer();

	return strDrive + strDir;
}

//! 書き込み
BOOL ComFunc::WriteSJISFile(LPCTSTR lpszFile,const CString& strBuff)
{
#ifdef _UNICODE
	std::string strChar = (LPCSTR)CW2A(strBuff);
#else
	std::string strChar = strBuff;
#endif
	CFile cFile;
	if( cFile.Open(lpszFile,CFile::modeCreate|CFile::modeWrite) ==FALSE)
	{
		return FALSE;
	}

	cFile.Write(strChar.c_str(),(UINT)strChar.size());

	cFile.Close();
	return TRUE;
}

//! 読込み
BOOL ComFunc::ReadSJISFile(LPCTSTR lpszFile,CString& strBuff)
{
	CFile cFile;
	try
	{
		if( cFile.Open(lpszFile,CFile::modeRead|CFile::shareDenyWrite) ==FALSE )
		{
			return FALSE;	
		}
		ULONGLONG uLen = cFile.GetLength();
		char* pBuff = new char[(size_t)uLen+1];
		::ZeroMemory(pBuff,(UINT)uLen+1);
		cFile.Read(pBuff,(UINT)uLen);
		cFile.Close();

		std::string strSrc = pBuff;
		delete pBuff;
		pBuff=NULL;

		strBuff = CA2W(strSrc.c_str());
		return TRUE;
	}
	catch(...)
	{
		cFile.Close();
	}
	return FALSE;
}


//! 添付 でファイルに書き込み
CString ComFunc::Write2TempFile(const CString& strBuff)
{
	CString strPath;
	GetNewTempFileName(strPath);

	if( WriteSJISFile(strPath,strBuff) )
	{
		return strPath;
	}
	return NULL;
}


/**
 * エラー文字列取得
 */
CString ComFunc::GetLastErrorString(DWORD dwErrorNo, BOOL* pSeikou)
{
	LPVOID lpMsgBuf = NULL;
	CString strRet;
 
	if( (0==FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL,
		dwErrorNo,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL)) || lpMsgBuf==NULL )
	{
		if(pSeikou)
		{
			*pSeikou = FALSE;
		}
		return strRet;
	}
 
	strRet = (LPTSTR)lpMsgBuf;
	LocalFree(lpMsgBuf);
 
	if (pSeikou)
	{
		*pSeikou = TRUE;
	}
 
	return strRet;
}


/**
 * DLL 管理
 */
class CDLLLibrary
{
public :
	CDLLLibrary(LPCTSTR name)
	{
		m_hDLL = LoadLibrary(name);

	}
	~CDLLLibrary()
	{
		if(m_hDLL)
		{
			FreeLibrary(m_hDLL);
		}
	}

	HMODULE GetHandle()const
	{
		return m_hDLL;
	}
	

private:
	HMODULE m_hDLL;

};
std::auto_ptr<CDLLLibrary> g_pMLangDLL;
typedef HRESULT (CALLBACK* TConvertINetString)(LPDWORD lpdwMode, DWORD dwSrcEncoding, DWORD dwDstEncoding, LPCSTR lpSrcStr, LPINT lpnSrcSize, LPBYTE lpDstStr, LPINT lpnDstSize);
/**
 * 文字コード変換 EUC -> UNICODE
 */
BOOL ComFunc::EUCtoUNICODE(const std::string& src, std::wstring& dest)
{
	if(g_pMLangDLL.get() ==NULL)
	{
		g_pMLangDLL.reset(new CDLLLibrary(_T("mlang.dll")));
	}
	if(g_pMLangDLL.get()==NULL ||
		g_pMLangDLL->GetHandle() ==NULL)
	{
		g_pMLangDLL.reset();
		return FALSE;
	}
	TConvertINetString pConvertINetString = reinterpret_cast<TConvertINetString>(GetProcAddress(g_pMLangDLL->GetHandle(), "ConvertINetString"));
	if (!pConvertINetString)
	{
		return FALSE;
	}

/**
437      IBM437        OEM United States
932      shift_jis     ANSI/OEM Japanese; Japanese (Shift-JIS)
1200     utf-16        Unicode UTF-16, little endian byte order (BMP of ISO 10646);
                       available only to managed applications
20127    us-ascii      US-ASCII (7-bit)
20932    EUC-JP        Japanese (JIS 0208-1990 and 0121-1990)
50220    iso-2022-jp   ISO 2022 Japanese with no halfwidth Katakana;
                       Japanese (JIS)
50222    iso-2022-jp   ISO 2022 Japanese JIS X 0201-1989;
                       Japanese (JIS-Allow 1 byte Kana - SO/SI)
51932    euc-jp        EUC Japanese
65001    utf-8         Unicode (UTF-8)
*/

	const DWORD  CP_UNICODE  = 1200;
	const DWORD  CP_EUC  = 51932;
    DWORD   Mode( 0 );

	BOOL bRet = FALSE;
	BYTE* lpWideString=NULL;
	try
	{
		do
		{
			// UTF8からUnicodeに変換した場合の長さを求める。
			int iWideLength=0;
			int nSrclLen  = (int)src.size();
			HRESULT hr = (*pConvertINetString)( &Mode, CP_EUC,CP_UNICODE, src.c_str(), &nSrclLen, NULL, &iWideLength );
			if (FAILED(hr))
			{
				break;
			}
			lpWideString = new BYTE[iWideLength+2];
			::ZeroMemory(lpWideString,(iWideLength+2));

			// UTF8からUnicodeに変換する。
			hr = (*pConvertINetString)( &Mode, CP_EUC,CP_UNICODE, src.c_str(),  &nSrclLen, lpWideString, &iWideLength );
			if(FAILED(hr))
			{
				break;
			}

			dest = (wchar_t*)lpWideString;
			bRet = TRUE;
		}while(0);
		// 変換に成功
	}
	catch(std::exception& /*e*/)
	{
	}
	if(lpWideString)
	{
		delete [] lpWideString;
	}
	return bRet;
}