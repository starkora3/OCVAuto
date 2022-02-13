/**
 * ファイル＆ディレクトリ操作クラス
 *
 * APIを使ったファイル操作の単純なラップクラスです。
 *
 * @file	FileOperation.h
 */
#pragma once

namespace NHCOM
{

/**
 * @brief ファイル＆ディレクトリ操作クラス
 *
 * 全てのメソッドがstaticとなっています。
 *
 * @class	CFileOperation
 */
class CFileOperation
{
public:

	//! パスの中の欲しい部分
	enum RETURNPART
	{
		RET_DRIVE,		//!< ドライブ
		RET_DIR,		//!< ディレクトリ
		RET_FNAME,		//!< ファイル名（拡張子なし）
		RET_EXT,		//!< 拡張子
	};

public:

	//! テンポラリ画像ファイル名のプレフィックス
	static LPCTSTR PREFIX_OF_TEMP_IMAGE_FILE;
	//! データファイルの拡張子
	static LPCTSTR DATAFILE_EXTENTION;

	//! デストラクタ
	virtual ~CFileOperation();							

	//! 存在チェック
	static BOOL IsExist(const CString& strPath);					
	//! ディレクトリか？
	static BOOL IsDirectory(const CString& strPath);				

	//! 拡張子取得
	static LPTSTR FindExtension(const CString& strPath);

	//! ファイルコピー
	static BOOL CopyFile(const CString& strSrcFilePath,
						 const CString& strDstFilePath,
						 BOOL bFailIfExists);				

	//! テンポラリファイルにコピー
	static BOOL CopyToTmpFile(const CString& strSrcFilePath,
							  const CString& strDstDirPath, 
							  const CString& strPrefix, 
							  CString& strTempName);				

	//! 指定した拡張子のテンポラリファイルにコピー
	static BOOL CopyToTmpFileWithExtension(const CString& strSrcFilePath,
							  const CString& strDstDirPath, 
							  const CString& strPrefix, 
							  const CString& strExtension,
							  CString& strTempName);	

	//! ファイル移動
	static BOOL MoveFile(const CString& strSrcFilePath,
						 const CString& strDstFilePath);

	//! ファイルリネーム
	static BOOL RenameFile(const CString& strSrcPath,
						   const CString& strDetPath);

	//! ファイル削除
	static BOOL DeleteFile(const CString& strFilePath);

	//! ディレクトリ作成
	static BOOL CreateDirectory(const CString& strDirPath);

	//!  空ディレクトリ削除
	static BOOL RemoveDirectory(const CString& strDirPath);

	//! ディレクトリ削除
	static BOOL DeleteDirectory(const CString& strDirPath);

	//! テンポラリファイル作成
	static CString CreateTmpFileInDir(const CString& strDirPath,
									  const CString& strPrefix);

	//! パスからファイル名を取得
	static CString GetFileNameAtPath(const CString& strPath, 
									 BOOL bExt = TRUE);

	//! パスからディレクトリ名を取得
	static CString GetDirNameAtPath(const CString& strPath);

	//! パスから任意の部分を取得
	static CString GetPartOfPathAtPath(const CString& strPath,
									   const RETURNPART ePart);

	//! 書き込み可能化
	static BOOL Writable(const CString& strFilePath);

	//! ファイルから文字列の読み込み
	static CString ReadString(CFile& cFile);

	//! ファイル分割
	static BOOL SplitFile(LPCTSTR lpszFile,
						  CFile* pFile,
						  TCHAR* pErr=NULL);

	//! ファイルの存在確認
	static BOOL IsExistFile(LPCTSTR lpszFiler);	

	//! ディレクトリの存在確認
	static BOOL IsExistDirectory(LPCTSTR lpszFiler);

	//! 
	static BOOL TrashFile(const CString& strFilePath);

	//! 読み込み・書き込み関連
	enum ERR_CODE{
		ERR_MAXLEN = 1024,	//!< エラー文字列最大長
		ERR_NONE = 0,		//!< エラー無し
		ERR_SYSTEM,			//!< システムエラー
		ERR_FILEOPEN,		//!< オープンエラー
		ERR_READ_LEN,		//!< 長さ違反
		ERR_FILEID,			//!< ファイルIDが異なる
		ERR_GETTEMPFILENAME,//!< テンポラリファイル作成失敗
		ERR_THUMBCREATE,	//!< サムネイル作成失敗
		ERR_INVALIDVAL,		//!< 値が不正
		ERR_FILEVERNEW,		//!< バージョンが更新されている
	};

	//! エラーメッセージ設定
	static void SetErrString(ERR_CODE nErr,
							 LPTSTR pErr,
							 LPCTSTR pSub1 = NULL,
							 LPCTSTR pSub2 = NULL);

	//! エラーメッセージ設定
	static void SetSysErrString(LPTSTR pErr, 
								CException* p);

	//! チェックサムの計算
	static int CalcCheckSum(LPCTSTR lpszFile, DWORD& dwSum);

	//! ファイル追加書き込み
	static BOOL AddWriteSJISFile(LPCTSTR lpszFile,const CString& strBuff);

	//! SJIS でファイル書き込み
	static BOOL WriteSJISFile(LPCTSTR lpszFile,const CString& strBuff);
	//! SJIS ファイル読込み
	static BOOL ReadSJISFile(LPCTSTR lpszFile,CString& strBuff);


private:
	//! コンストラクタ
	CFileOperation();
	//! エラーメッセージ表示
	static void ShowSystemErrorMessage();
};

};