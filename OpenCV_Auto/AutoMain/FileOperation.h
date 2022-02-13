/**
 * �t�@�C�����f�B���N�g������N���X
 *
 * API���g�����t�@�C������̒P���ȃ��b�v�N���X�ł��B
 *
 * @file	FileOperation.h
 */
#pragma once

namespace NHCOM
{

/**
 * @brief �t�@�C�����f�B���N�g������N���X
 *
 * �S�Ẵ��\�b�h��static�ƂȂ��Ă��܂��B
 *
 * @class	CFileOperation
 */
class CFileOperation
{
public:

	//! �p�X�̒��̗~��������
	enum RETURNPART
	{
		RET_DRIVE,		//!< �h���C�u
		RET_DIR,		//!< �f�B���N�g��
		RET_FNAME,		//!< �t�@�C�����i�g���q�Ȃ��j
		RET_EXT,		//!< �g���q
	};

public:

	//! �e���|�����摜�t�@�C�����̃v���t�B�b�N�X
	static LPCTSTR PREFIX_OF_TEMP_IMAGE_FILE;
	//! �f�[�^�t�@�C���̊g���q
	static LPCTSTR DATAFILE_EXTENTION;

	//! �f�X�g���N�^
	virtual ~CFileOperation();							

	//! ���݃`�F�b�N
	static BOOL IsExist(const CString& strPath);					
	//! �f�B���N�g�����H
	static BOOL IsDirectory(const CString& strPath);				

	//! �g���q�擾
	static LPTSTR FindExtension(const CString& strPath);

	//! �t�@�C���R�s�[
	static BOOL CopyFile(const CString& strSrcFilePath,
						 const CString& strDstFilePath,
						 BOOL bFailIfExists);				

	//! �e���|�����t�@�C���ɃR�s�[
	static BOOL CopyToTmpFile(const CString& strSrcFilePath,
							  const CString& strDstDirPath, 
							  const CString& strPrefix, 
							  CString& strTempName);				

	//! �w�肵���g���q�̃e���|�����t�@�C���ɃR�s�[
	static BOOL CopyToTmpFileWithExtension(const CString& strSrcFilePath,
							  const CString& strDstDirPath, 
							  const CString& strPrefix, 
							  const CString& strExtension,
							  CString& strTempName);	

	//! �t�@�C���ړ�
	static BOOL MoveFile(const CString& strSrcFilePath,
						 const CString& strDstFilePath);

	//! �t�@�C�����l�[��
	static BOOL RenameFile(const CString& strSrcPath,
						   const CString& strDetPath);

	//! �t�@�C���폜
	static BOOL DeleteFile(const CString& strFilePath);

	//! �f�B���N�g���쐬
	static BOOL CreateDirectory(const CString& strDirPath);

	//!  ��f�B���N�g���폜
	static BOOL RemoveDirectory(const CString& strDirPath);

	//! �f�B���N�g���폜
	static BOOL DeleteDirectory(const CString& strDirPath);

	//! �e���|�����t�@�C���쐬
	static CString CreateTmpFileInDir(const CString& strDirPath,
									  const CString& strPrefix);

	//! �p�X����t�@�C�������擾
	static CString GetFileNameAtPath(const CString& strPath, 
									 BOOL bExt = TRUE);

	//! �p�X����f�B���N�g�������擾
	static CString GetDirNameAtPath(const CString& strPath);

	//! �p�X����C�ӂ̕������擾
	static CString GetPartOfPathAtPath(const CString& strPath,
									   const RETURNPART ePart);

	//! �������݉\��
	static BOOL Writable(const CString& strFilePath);

	//! �t�@�C�����當����̓ǂݍ���
	static CString ReadString(CFile& cFile);

	//! �t�@�C������
	static BOOL SplitFile(LPCTSTR lpszFile,
						  CFile* pFile,
						  TCHAR* pErr=NULL);

	//! �t�@�C���̑��݊m�F
	static BOOL IsExistFile(LPCTSTR lpszFiler);	

	//! �f�B���N�g���̑��݊m�F
	static BOOL IsExistDirectory(LPCTSTR lpszFiler);

	//! 
	static BOOL TrashFile(const CString& strFilePath);

	//! �ǂݍ��݁E�������݊֘A
	enum ERR_CODE{
		ERR_MAXLEN = 1024,	//!< �G���[������ő咷
		ERR_NONE = 0,		//!< �G���[����
		ERR_SYSTEM,			//!< �V�X�e���G���[
		ERR_FILEOPEN,		//!< �I�[�v���G���[
		ERR_READ_LEN,		//!< �����ᔽ
		ERR_FILEID,			//!< �t�@�C��ID���قȂ�
		ERR_GETTEMPFILENAME,//!< �e���|�����t�@�C���쐬���s
		ERR_THUMBCREATE,	//!< �T���l�C���쐬���s
		ERR_INVALIDVAL,		//!< �l���s��
		ERR_FILEVERNEW,		//!< �o�[�W�������X�V����Ă���
	};

	//! �G���[���b�Z�[�W�ݒ�
	static void SetErrString(ERR_CODE nErr,
							 LPTSTR pErr,
							 LPCTSTR pSub1 = NULL,
							 LPCTSTR pSub2 = NULL);

	//! �G���[���b�Z�[�W�ݒ�
	static void SetSysErrString(LPTSTR pErr, 
								CException* p);

	//! �`�F�b�N�T���̌v�Z
	static int CalcCheckSum(LPCTSTR lpszFile, DWORD& dwSum);

	//! �t�@�C���ǉ���������
	static BOOL AddWriteSJISFile(LPCTSTR lpszFile,const CString& strBuff);

	//! SJIS �Ńt�@�C����������
	static BOOL WriteSJISFile(LPCTSTR lpszFile,const CString& strBuff);
	//! SJIS �t�@�C���Ǎ���
	static BOOL ReadSJISFile(LPCTSTR lpszFile,CString& strBuff);


private:
	//! �R���X�g���N�^
	CFileOperation();
	//! �G���[���b�Z�[�W�\��
	static void ShowSystemErrorMessage();
};

};