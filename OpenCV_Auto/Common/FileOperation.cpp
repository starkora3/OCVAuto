/**
 * ファイル＆ディレクトリ操作クラス
 *
 * APIを使ったファイル操作の単純なラップクラスです。
 *
 * @file	FileOperation.cpp
 */
#include "stdafx.h"
#include "FileOperation.h"

#include <Shlwapi.h>			// for Shell Path Handling Functions
#include "ComFunc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace NHCOM;

#define LENGTH_BUFFER 1024

// プレフィックス
LPCTSTR CFileOperation::PREFIX_OF_TEMP_IMAGE_FILE	=_T("tmp");				// テンポラリ画像ファイル名のプレフィックス
LPCTSTR CFileOperation::DATAFILE_EXTENTION	=_T(".rap");				// テンポラリ画像ファイル名のプレフィックス

/**
 * コンストラクタ
 *
 * private宣言しているので、呼ばれることはない。
 */
CFileOperation::CFileOperation()
{
	// nothing to do.
}

/**
 * デストラクタ
 */
CFileOperation::~CFileOperation()
{
	// nothing to do.
}

/**
 * 存在チェック
 *
 * ファイルおよびフォルダが存在しているか調べます。
 *
 * @return	存在する場合はTRUEを、存在しない場合はFALSEを返します。
 */
BOOL CFileOperation::IsExist(const CString& strPath /**< 対象パス */)
{
	ASSERT(strPath.GetLength() <= MAX_PATH);

	return PathFileExists(strPath);
}

/**
 * ディレクトリか？
 *
 * ディレクトリかどうか調べます。
 *
 * @return	ディレクトリの場合はTRUEを、それ以外の場合はFALSEを返します。
 */
BOOL CFileOperation::IsDirectory(const CString& strPath /**< 対象パス */)
{
	ASSERT(strPath.GetLength() <= MAX_PATH);

	return PathIsDirectory(strPath);
}

/**
 * 拡張子取得
 *
 * 拡張子（ドット以下）を取得します。
 *
 * @return	拡張子が無い場合はNULLを、それ以外の場合はドット以下の拡張子を返します。
 */
LPTSTR CFileOperation::FindExtension(const CString& strPath /**< 対象パス */)
{
	ASSERT(strPath.GetLength() <= MAX_PATH);

	return PathFindExtension(strPath);
}

/**
 * ファイルコピー
 *
 * 既存のファイルを新しいファイルへコピーします。
 *
 * @param	strSrcFilePath	既存のファイルの名前
 * @param	strDstFilePath	新しいファイルの名前<br>
 *							空の場合は、カレントディレクトリにコピーする
 * @param	bFailIfExists	ファイルが存在する場合の動作<br>
 *							TRUEを指定し、同じ名前のファイルが存在した場合、
 *							関数は失敗し、コピーを行いません。
 *							FALSEを指定し、同じ名前のファイルは存在した場合、
 *							既存のファイルを上書きし、関数は成功します（確認ダイアログは出ない）。
 *
 * @return	正常終了の場合はTRUEを、異常終了の場合はFALSEを返します。
 */
BOOL CFileOperation::CopyFile(const CString& strSrcFilePath,
							  const CString& strDstFilePath,
							  BOOL bFailIfExists)
{
	if(::CopyFile(strSrcFilePath, strDstFilePath, bFailIfExists))
	{
		return TRUE;
	}

	return FALSE;
}

/**
 * テンポラリファイルにコピー
 *
 * ユニークな名前を持つテンポラリファイルが指定されたフォルダ内に作成される。
 * テンポラリファイル名にはPrefixを指定すること。作成されたテンポラリファイ
 * ルにファイルがコピーされる。<br>
 * 作られたテンポラリファイルはWindows のシャットダウン時に自動的に削除され
 * ることはありません。<br>
 * 元ファイルはそのまま残ります。
 *							
 * @return	正常終了の場合はTRUEを、異常終了の場合はFALSEを返します。<br>
 *			異常終了の場合、もしテンポラリファイルを作成していればそれを削
 *			除します。
 */
BOOL CFileOperation::CopyToTmpFile(const CString& strSrcFilePath,	/**< 既存ファイル名 */
								   const CString& strDstDirPath,	/**< コピー先ディレクトリ名 */ 
								   const CString& strPrefix,		/**< 付加する接頭辞 */
								   CString& strTempName				/**< テンポラリファイル名（フルパス） */
								   )
{
	// ユニークなファイル名を持つテンポラリファイルを作成する
	if(!::GetTempFileName(strDstDirPath, strPrefix, 0, strTempName.GetBuffer(MAX_PATH)))
	{
		strTempName.ReleaseBuffer();					// 一応
		return FALSE;
	}
	strTempName.ReleaseBuffer();

	// ファイルコピー
	if(!CopyFile(strSrcFilePath, strTempName, FALSE))
	{
		// 作成したテンポラリファイルを削除する
		::_tremove(strTempName);
		return FALSE;
	}

	return TRUE;
}

/**
 * 指定した拡張子のテンポラリファイルにコピー
 *
 * ユニークな名前と拡張子を持つテンポラリファイルが指定されたフォルダ内に作成される。
 * テンポラリファイル名にはPrefixとExtensionを指定すること。
 * 作成されたテンポラリファイルにファイルがコピーされる。<br>
 * 作られたユニークな拡張子のテンポラリファイルは
 * Windows のシャットダウン時に自動的に削除されることはありません。<br>
 * 元ファイルはそのまま残ります。
 *
 * @return	正常終了の場合はTRUEを、異常終了の場合はFALSEを返します。<br>
 *			異常終了の場合、もしテンポラリファイルを作成していればそれを削
 *			除します。
 */
BOOL CFileOperation::CopyToTmpFileWithExtension(
							const CString& strSrcFilePath,	/**< 既存ファイル名 */
							const CString& strDstDirPath,	/**< コピー先ディレクトリ名 */
							const CString& strPrefix,		/**< 付加する接頭辞 */
							const CString& strExtension,	/**< 作成する拡張子 */
							CString& strTempName			/**< テンポラリファイル名（フルパス） */
							)
{
	// ユニークなファイル名を持つテンポラリファイルを作成する
	if(!::GetTempFileName(strDstDirPath, strPrefix, 0, strTempName.GetBuffer(MAX_PATH)))
	{
		strTempName.ReleaseBuffer();					// 一応
		return FALSE;
	}
	strTempName.ReleaseBuffer();

	if(IsExist(strTempName))
	{// 空ファイルが作成されるのを削除
		::DeleteFile(strTempName);
	}

	::PathRemoveExtension(strTempName.GetBuffer(MAX_PATH));

	strTempName.ReleaseBuffer();

	if(!::PathAddExtension(strTempName.GetBuffer(MAX_PATH),strExtension))
	{
		strTempName.ReleaseBuffer();
		return FALSE;
	}
	strTempName.ReleaseBuffer();

	// ファイルコピー
	if(!CopyFile(strSrcFilePath, strTempName, FALSE))
	{
		// 作成したテンポラリファイルを削除する
		::_tremove(strTempName);
		return FALSE;
	}

	return TRUE;
}

/**
 * ファイル移動
 *
 * ファイルまたはディレクトリを（ディレクトリの場合は、その子も含め）移動し、
 * 指定された場合は名前の変更も行います。移動先は、同じディレクトリでもかま
 * いませんし（この場合は名前の変更のみ）、他のディレクトリでもかまいません。<br>
 * <em>ただし、ディレクトリを他のボリュームへ移動しようとすると、MoveFile
 * 関数が失敗することに注意してください。</em>
 *
 * @return	正常終了の場合はTRUEを、異常終了の場合はFALSEを返します。
 */
BOOL CFileOperation::MoveFile(const CString& strSrcFilePath, /**< 既存ファイル名 */
							  const CString& strDstFilePath	 /**<  新しいファイル名*/)
{
	if(::MoveFile(strSrcFilePath, strDstFilePath))
	{
		return TRUE;
	}

	return FALSE;
}

/**
 * ファイルリネーム
 *
 * strSrcPathで指定したファイルまたはディレクトリの名前、
 * strDetPathで指定した名前に変更します。
 *
 * @return	正常終了の場合はTRUEを、異常終了の場合はFALSEを返します。
 */
BOOL CFileOperation::RenameFile(const CString& strSrcPath, /**< 既存ファイルまたはディレクトリのパス */
								const CString& strDetPath  /**< 変更ファイルまたはディレクトリのパス */)
{
	if(!::_trename(strSrcPath, strDetPath))
	{
		return FALSE;
	}
	return TRUE;
}

/**
 * ファイル削除
 *
 * ファイルを削除します。<br>
 * ReadOnlyなファイルも属性を除去してから削除します。
 *
 * @return	正常終了の場合はTRUEを、異常終了の場合はFALSEを返します。
 */
BOOL CFileOperation::DeleteFile(const CString& strFilePath /**< 対象パス */)
{
	// ReadOnly属性除去
	DWORD dwAttributes = ::GetFileAttributes(strFilePath);
	if(dwAttributes & FILE_ATTRIBUTE_READONLY)
	{
		if(!::SetFileAttributes(strFilePath, dwAttributes & ~FILE_ATTRIBUTE_READONLY))
		{
			return FALSE;
		}
	}

	// ファイル削除
	if(!::DeleteFile(strFilePath))
	{
		return FALSE;
	}
	return TRUE;
}

/**
 * ディレクトリ作成
 *
 * ディレクトリを作成します。
 *
 * @return	正常終了の場合はTRUEを、異常終了の場合はFALSEを返します。
 */
BOOL CFileOperation::CreateDirectory(const CString& strDirPath /**< 作成するディレクトリパス */)
{
	if(!::CreateDirectory(strDirPath, NULL))
	{
		return FALSE;
	}
	return TRUE;
}

//------------------------------------------------------
/**
 * 空ディレクトリ削除
 *
 * 空のディレクトリを削除します。<br>
 * ReadOnlyなディレクトリも属性を除去してから削除します。<br>
 * 空でない場合は失敗します。
 *
 * @return	正常終了の場合はTRUEを、異常終了の場合はFALSEを返します。
 * @see		CFileOperation::DeleteDirectory
 */
//------------------------------------------------------
BOOL CFileOperation::RemoveDirectory(const CString& strDirPath /**< 対象パス */)
{
	// ReadOnly属性除去
	DWORD dwAttributes = GetFileAttributes(strDirPath);
	if(dwAttributes & FILE_ATTRIBUTE_READONLY)
	{
		if(!::SetFileAttributes(strDirPath,
								dwAttributes & ~FILE_ATTRIBUTE_READONLY))
		{
			return FALSE;
		}
	}

	// ディレクトリ削除
	if(!::RemoveDirectory(strDirPath))
	{
		return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------
/**
 * ディレクトリ削除
 *
 * ディレクトリを中身ごと削除します。<br>
 * ReadOnlyなディレクトリやファイルも属性を除去してから削除します。<br>
 *
 * @return	正常終了の場合はTRUEを、異常終了の場合はFALSEを返します。
 * @see		CFileOperation::RemoveDirectory
 */
//------------------------------------------------------
BOOL CFileOperation::DeleteDirectory(const CString& strDirPath /**< 対象パス */)
{
	CFileFind Finder;
	BOOL bMoreFile = Finder.FindFile(ComFunc::GetPathCombine(strDirPath, _T("*.*")));
	while(bMoreFile)
	{
		bMoreFile = Finder.FindNextFile();

		// ディレクトリの場合
		if(Finder.IsDirectory())
		{
			// "." および ".." は無視する
			if(Finder.IsDots())
			{
				continue;
			}

			// ディレクトリ削除
			if(!DeleteDirectory(Finder.GetFilePath()))
			{
#ifdef _DEBUG
				CString strMsg;
				strMsg.Format(_T("\t%sの削除に失敗しました。\n"), Finder.GetFilePath());
				TRACE(strMsg);
#endif // _DEBUG
				// 失敗しても続行する。
				//Finder.Close();
				//return FALSE;
			}
		}
		// ファイルの場合
		else
		{
			// ファイル削除
			if(!DeleteFile(Finder.GetFilePath()))
			{
#ifdef _DEBUG
				CString strMsg;
				strMsg.Format(_T("\t%sの削除に失敗しました。\n"), Finder.GetFilePath());
				TRACE(strMsg);
#endif // _DEBUG
				// 失敗しても続行する。
//				Finder.Close();
//				return FALSE;
			}
		}
	}
	Finder.Close();

	// 当該ディレクトリ削除
	if(!CFileOperation::RemoveDirectory(strDirPath))
	{
#ifdef _DEBUG
		CString strMsg;
		strMsg.Format(_T("\t%sの削除に失敗しました。\n"), strDirPath);
		TRACE(strMsg);
#endif // _DEBUG
		return FALSE;
	}

	return TRUE;
}

/**
 * テンポラリファイル作成
 *
 * ユニークなファイル名を持つテンポラリファイルを作成します。
 *
 * @return	正常終了の場合は作成したテンポラリファイルのパスを、異常終
 *			了の場合は空文字列を返します。
 */
CString CFileOperation::CreateTmpFileInDir(const CString& strDirPath,	/**< 作成先ディレクトリのパス */
										   const CString& strPrefix		/**< 付加する接頭辞 */)
{
	CString strTmpFilePath;

	UINT uiResult = ::GetTempFileName(strDirPath, strPrefix, 0, strTmpFilePath.GetBuffer(MAX_PATH));

	strTmpFilePath.ReleaseBuffer();

	if(uiResult == 0) // エラー
	{
		strTmpFilePath.Empty();
	}

	return strTmpFilePath;
}

/**
 * パスからファイル名を取得
 *
 * パスからファイル名を取得します。
 *
 * @return	ファイル名
 */
CString CFileOperation::GetFileNameAtPath(const CString& strPath,/**< 対象パス */ 
										  BOOL bExt /*= TRUE*/ /**< 拡張子の有無 */)
{
	ASSERT(!strPath.IsEmpty());

	CString strFileName;								// ファイル名（拡張子なし）
	CString strFileExt;									// 拡張子

	// 分解
	::_tsplitpath_s(strPath,
				 NULL,0,
				 NULL,0,
				 strFileName.GetBuffer(_MAX_FNAME),_MAX_FNAME,
				 strFileExt.GetBuffer(_MAX_EXT),_MAX_EXT);
	strFileName.ReleaseBuffer();
	strFileExt.ReleaseBuffer();

	return (bExt == TRUE) ? strFileName + strFileExt : strFileName;
}

/**
 * パスからディレクトリ名を取得
 *
 * パスからディレクトリ名を取得します。
 *
 * @return	ディレクトリ名（パス）
 */
CString CFileOperation::GetDirNameAtPath(const CString& strPath /**< 対象パス */)
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

/**
 * パスから任意の部分を取得
 *
 * パスから任意の部分を取得します。
 *
 * @return	任意の部分
 */
CString CFileOperation::GetPartOfPathAtPath(const CString& strPath,	/**< 対象パス */
											const RETURNPART ePart	/**< パス中の取得部分 */)
{
	ASSERT(!strPath.IsEmpty());

	CString strDrive;								// ドライブ
	CString strDir;									// ディレクトリ
	CString strFileName;								// ファイル名（拡張子なし）
	CString strFileExt;									// 拡張子

	// 分解
	::_tsplitpath_s(strPath,
				 strDrive.GetBuffer(_MAX_DRIVE),_MAX_DRIVE,
				 strDir.GetBuffer(_MAX_DIR),_MAX_DIR,
				 strFileName.GetBuffer(_MAX_FNAME),_MAX_FNAME,
				 strFileExt.GetBuffer(_MAX_EXT),_MAX_EXT);
	strDrive.ReleaseBuffer();
	strDir.ReleaseBuffer();
	strFileName.ReleaseBuffer();
	strFileExt.ReleaseBuffer();

	CString strResult;
	switch(ePart)
	{
	case RET_DRIVE:
		strResult = strDrive;
		break;

	case RET_DIR:
		strResult = strDir;
		break;

	case RET_FNAME:
		strResult = strFileName;
		break;

	case RET_EXT:
		strResult = strFileExt;
		break;

	default:
		ASSERT(FALSE);
		break;
	}

	return strResult;
}

/**
 * エラーメッセージ表示
 *
 * エラーメッセージを表示します。<br>
 * GetLastError()の内容を表示します。<br>
 * 主にDebug用？
 */
void CFileOperation::ShowSystemErrorMessage()
{
	LPVOID pMessageBuffer;

	// エラー文字列取得
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // 既定の言語
		reinterpret_cast<LPTSTR>(&pMessageBuffer),
		0,
		NULL
	);

	// 文字列を表示する。
	::AfxMessageBox(reinterpret_cast<LPTSTR>(pMessageBuffer),MB_OK);

	// バッファを解放する。
	LocalFree(pMessageBuffer);
}

/**
 *	書き込み可能化
 *
 *	ReadOnly属性を除去する事で書き込み可能にします。
 *
 *	@return	成否
 */
BOOL CFileOperation::Writable(const CString& strFilePath /**< 対象パス */)
{
	ASSERT(!strFilePath.IsEmpty());

	// ファイル存在チェック
	if(!IsExist(strFilePath))
	{
		return TRUE;
	}
	
	// ReadOnly属性除去
	DWORD dwAttributes = ::GetFileAttributes(strFilePath);
	if(dwAttributes & FILE_ATTRIBUTE_READONLY)
	{
		if(!::SetFileAttributes(strFilePath, dwAttributes & ~FILE_ATTRIBUTE_READONLY))
		{
			return FALSE;
		}
	}

	return TRUE;
}

/**
 * ファイルから文字列の読み込み
 *
 * @return 読み込んだ文字列
 */
CString CFileOperation::ReadString(CFile& cFile /**< 読み込むファイル */)
{
	#define BUFFER_LENGTH 1024
	TCHAR byTemp[BUFFER_LENGTH];
	ULONGLONG ulPosTemp = cFile.GetPosition();

	cFile.Read(byTemp,BUFFER_LENGTH * sizeof(TCHAR));
#ifndef _UNICODE
	LPVOID lpFind = memchr(byTemp,_T('\0'),BUFFER_LENGTH * sizeof(TCHAR));
#else
	LPVOID lpFind = wmemchr(byTemp,_T('\0'),BUFFER_LENGTH * sizeof(TCHAR));
#endif
	int nFind = 0;
	while( lpFind == NULL)
	{
		int nLenTemp = cFile.Read(byTemp,BUFFER_LENGTH * sizeof(TCHAR));
		if( nLenTemp == 0 )
		{
			break;
		}
#ifndef _UNICODE
		lpFind = memchr(byTemp,_T('\0'),BUFFER_LENGTH * sizeof(TCHAR));
#else
		lpFind = wmemchr(byTemp,_T('\0'),BUFFER_LENGTH * sizeof(TCHAR));
#endif
		nFind += nLenTemp;
	}
	if( lpFind == FALSE)
	{// 文字列の終端が見つからず
		nFind = 0;
		return _T("");
	}
	else
	{
		nFind += (int)((TCHAR*)lpFind - byTemp);
	}
	// シーク位置復元
	cFile.Seek(ulPosTemp,CFile::begin);

	CString str;
	cFile.Read(str.GetBuffer(nFind+1),(nFind+1) * sizeof(TCHAR));
	str.ReleaseBuffer();

	return str;
}

/**
 * エラーメッセージ設定
 */
void CFileOperation::SetErrString(ERR_CODE nErr, /**< エラーコード */
								  TCHAR* pErr,	/**< エラーメッセージ */
								  LPCTSTR /*pSub1*/,/**< ※未使用 */
								  LPCTSTR /*pSub2*/ /**< ※未使用 */)
{
	if(pErr == FALSE)
	{
		return;
	}
}

/**
 * エラーメッセージ設定
 */
void CFileOperation::SetSysErrString(TCHAR* pErr,	/**< エラーメッセージ */
									 CException* p	/**< 例外 */)
{
	#define BUFF_LEN 256
	TCHAR tcSub1[BUFF_LEN];

	p->GetErrorMessage(tcSub1,BUFF_LEN);

	SetErrString(ERR_SYSTEM,pErr,tcSub1);
}

/**
 * ファイル分割
 *
 * @return 成否
 */
BOOL CFileOperation::SplitFile(LPCTSTR lpszFile, /**< 対象ファイル名 */
							   CFile* pFile,	 /**< 読み込むファイル */
							   LPTSTR pErr /**< エラーメッセージ */)
{
	CFile cTempFile;
    BYTE pReadBuf[LENGTH_BUFFER];           // 入力バッファ

	try
	{
		if( !cTempFile.Open(lpszFile,CFile::modeWrite|CFile::modeCreate))
		{// 失敗
			// エラーセット
			CFileOperation::SetErrString( CFileOperation::ERR_FILEOPEN,pErr);
			return FALSE;
		}

		BOOL bRet = TRUE;
		do
		{
			// データファイルサイズ読み込み
			ULONGLONG ullFileSize;
			if( pFile->Read(&ullFileSize, sizeof(ULONGLONG)) !=  sizeof(ULONGLONG))
			{// 失敗
				bRet = FALSE;
				// エラーセット
				CFileOperation::SetErrString( CFileOperation::ERR_READ_LEN,pErr);
				break;
			}
			TRACE("SplitFileSize= %ld \n",ullFileSize);

			// データファイルの出力
			while(ullFileSize >= LENGTH_BUFFER)
			{
				int nRCnt = pFile->Read(pReadBuf,LENGTH_BUFFER);
				if(LENGTH_BUFFER != nRCnt)
				{// 
					cTempFile.Write(pReadBuf,nRCnt);
					// エラーセット
					CFileOperation::SetErrString( CFileOperation::ERR_READ_LEN,pErr);

					bRet = FALSE;
					break;
				}
				cTempFile.Write(pReadBuf,LENGTH_BUFFER);
				ullFileSize -= LENGTH_BUFFER;
			}

			if(ullFileSize > 0 && ullFileSize < LENGTH_BUFFER)
			{
				if(pFile->Read(pReadBuf,(UINT)ullFileSize) != ullFileSize)
				{// 失敗
					// エラーセット
					CFileOperation::SetErrString( CFileOperation::ERR_READ_LEN,pErr);
					bRet = FALSE;
					break;
				}
				cTempFile.Write(pReadBuf,static_cast<UINT>(ullFileSize));
			}
		}
		while(0);
		cTempFile.Close();
		return bRet;
	}
	catch(CException* p)
	{
		// エラーセット
		CFileOperation::SetSysErrString(pErr,p);
		cTempFile.Close();
		p->Delete();
		return FALSE;
	}
}

/**
 * ファイルの存在確認
 */
BOOL CFileOperation::IsExistFile(LPCTSTR lpszFiler /**< 調べるファイル名 */)
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


/**
 * ディレクトリの存在確認
 *
 * @return 成否
 */
BOOL CFileOperation::IsExistDirectory(LPCTSTR lpszFiler /**< 調べるディレクトリ名 */)
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

/**
 * チェックサムの計算
 *
 * @return エラーコード
 */
int CFileOperation::CalcCheckSum(LPCTSTR lpszFile, /**< ファイル名 */
								 DWORD& dwSum /**< チェックサム */)
{
	dwSum = 0;
	if(!IsExistFile(lpszFile))
	{
		return ERR_FILEOPEN;
	}
	CFile cFile;
	try
	{
		if(!cFile.Open(lpszFile,CFile::modeRead|CFile::shareDenyWrite))
		{// オープンエラー
			return ERR_FILEOPEN;
		}

		TCHAR tcBuff[LENGTH_BUFFER*sizeof(DWORD)];
		UINT uLen = cFile.Read(tcBuff,LENGTH_BUFFER);
		while(uLen)
		{
			UINT i=0;
			for(;(i + sizeof(DWORD))<uLen;i+=sizeof(DWORD))
			{
				dwSum += *((DWORD*)(tcBuff+i));
			}
			if(i<uLen)
			{
				DWORD dwTemp =0;
				memcpy(&dwTemp,(tcBuff+i),uLen - i);
				dwSum += dwTemp;
			}
			uLen = cFile.Read(tcBuff,LENGTH_BUFFER);
		}

		cFile.Close();
	}
	catch(CException* p)
	{
		p->Delete();
		cFile.Close();
		return ERR_SYSTEM;
	}
	return ERR_NONE;
}

/**
 * ファイルをゴミ箱へ移動
 *
 * @return 成否
 */
BOOL CFileOperation::TrashFile(const CString& strFilePath /**< 対象パス */)
{
	// double null-terminated にする
	CString strTemp(strFilePath);
	TCHAR* pStart = strTemp.GetBuffer(strTemp.GetLength() + 1 + 1);
	TCHAR* p =  pStart + strTemp.GetLength();
	*( p + 1 ) = 0;

	// SHFILEOPSTRUCT 構造体メンバの値を設定
	SHFILEOPSTRUCT FileOp;
	FileOp.hwnd = NULL;
	FileOp.wFunc = FO_DELETE;
	FileOp.pFrom = pStart;
	FileOp.pTo = NULL; // pTo is ignored.
	FileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
	FileOp.fAnyOperationsAborted = FALSE;
	FileOp.hNameMappings = NULL;
	FileOp.lpszProgressTitle = NULL;

	// ファイルを削除
	int iRet = SHFileOperation( &FileOp );

	strTemp.ReleaseBuffer();

	return iRet == 0;
}


//! ファイル書き込み
BOOL  CFileOperation::AddWriteSJISFile(LPCTSTR lpszFile,const CString& strBuff)
{
#ifdef _UNICODE
	std::string strChar = CW2A(strBuff);
#else
	std::string strChar = strBuff;
#endif
	CFile cFile;
	if(IsExistFile(lpszFile))
	{// 既に存在
		if( cFile.Open(lpszFile,CFile::modeWrite) ==FALSE)
		{
			return FALSE;
		}
		cFile.SeekToEnd();
	}
	else
	{
		if( cFile.Open(lpszFile,CFile::modeCreate|CFile::modeWrite) ==FALSE)
		{
			return FALSE;
		}
	}
	try
	{
		cFile.Write(strChar.c_str(),(UINT)strChar.size());
	}
	catch(...)
	{
	}
	cFile.Close();
	return TRUE;
}

//! 書き込み
BOOL CFileOperation::WriteSJISFile(LPCTSTR lpszFile,const CString& strBuff)
{
#ifdef _UNICODE
	std::string strChar = CW2A(strBuff);
#else
	std::string strChar = strBuff;
#endif
	CFile cFile;
	if( cFile.Open(lpszFile,CFile::modeCreate|CFile::modeWrite) ==FALSE)
	{
		return FALSE;
	}

	try
	{
		cFile.Write(strChar.c_str(),(UINT)strChar.size());
	}catch(...)
	{

	}

	cFile.Close();
	return TRUE;
}

//! 読込み
BOOL CFileOperation::ReadSJISFile(LPCTSTR lpszFile,CString& strBuff)
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

