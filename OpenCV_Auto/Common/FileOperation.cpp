/**
 * �t�@�C�����f�B���N�g������N���X
 *
 * API���g�����t�@�C������̒P���ȃ��b�v�N���X�ł��B
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

// �v���t�B�b�N�X
LPCTSTR CFileOperation::PREFIX_OF_TEMP_IMAGE_FILE	=_T("tmp");				// �e���|�����摜�t�@�C�����̃v���t�B�b�N�X
LPCTSTR CFileOperation::DATAFILE_EXTENTION	=_T(".rap");				// �e���|�����摜�t�@�C�����̃v���t�B�b�N�X

/**
 * �R���X�g���N�^
 *
 * private�錾���Ă���̂ŁA�Ă΂�邱�Ƃ͂Ȃ��B
 */
CFileOperation::CFileOperation()
{
	// nothing to do.
}

/**
 * �f�X�g���N�^
 */
CFileOperation::~CFileOperation()
{
	// nothing to do.
}

/**
 * ���݃`�F�b�N
 *
 * �t�@�C������уt�H���_�����݂��Ă��邩���ׂ܂��B
 *
 * @return	���݂���ꍇ��TRUE���A���݂��Ȃ��ꍇ��FALSE��Ԃ��܂��B
 */
BOOL CFileOperation::IsExist(const CString& strPath /**< �Ώۃp�X */)
{
	ASSERT(strPath.GetLength() <= MAX_PATH);

	return PathFileExists(strPath);
}

/**
 * �f�B���N�g�����H
 *
 * �f�B���N�g�����ǂ������ׂ܂��B
 *
 * @return	�f�B���N�g���̏ꍇ��TRUE���A����ȊO�̏ꍇ��FALSE��Ԃ��܂��B
 */
BOOL CFileOperation::IsDirectory(const CString& strPath /**< �Ώۃp�X */)
{
	ASSERT(strPath.GetLength() <= MAX_PATH);

	return PathIsDirectory(strPath);
}

/**
 * �g���q�擾
 *
 * �g���q�i�h�b�g�ȉ��j���擾���܂��B
 *
 * @return	�g���q�������ꍇ��NULL���A����ȊO�̏ꍇ�̓h�b�g�ȉ��̊g���q��Ԃ��܂��B
 */
LPTSTR CFileOperation::FindExtension(const CString& strPath /**< �Ώۃp�X */)
{
	ASSERT(strPath.GetLength() <= MAX_PATH);

	return PathFindExtension(strPath);
}

/**
 * �t�@�C���R�s�[
 *
 * �����̃t�@�C����V�����t�@�C���փR�s�[���܂��B
 *
 * @param	strSrcFilePath	�����̃t�@�C���̖��O
 * @param	strDstFilePath	�V�����t�@�C���̖��O<br>
 *							��̏ꍇ�́A�J�����g�f�B���N�g���ɃR�s�[����
 * @param	bFailIfExists	�t�@�C�������݂���ꍇ�̓���<br>
 *							TRUE���w�肵�A�������O�̃t�@�C�������݂����ꍇ�A
 *							�֐��͎��s���A�R�s�[���s���܂���B
 *							FALSE���w�肵�A�������O�̃t�@�C���͑��݂����ꍇ�A
 *							�����̃t�@�C�����㏑�����A�֐��͐������܂��i�m�F�_�C�A���O�͏o�Ȃ��j�B
 *
 * @return	����I���̏ꍇ��TRUE���A�ُ�I���̏ꍇ��FALSE��Ԃ��܂��B
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
 * �e���|�����t�@�C���ɃR�s�[
 *
 * ���j�[�N�Ȗ��O�����e���|�����t�@�C�����w�肳�ꂽ�t�H���_���ɍ쐬�����B
 * �e���|�����t�@�C�����ɂ�Prefix���w�肷�邱�ƁB�쐬���ꂽ�e���|�����t�@�C
 * ���Ƀt�@�C�����R�s�[�����B<br>
 * ���ꂽ�e���|�����t�@�C����Windows �̃V���b�g�_�E�����Ɏ����I�ɍ폜����
 * �邱�Ƃ͂���܂���B<br>
 * ���t�@�C���͂��̂܂܎c��܂��B
 *							
 * @return	����I���̏ꍇ��TRUE���A�ُ�I���̏ꍇ��FALSE��Ԃ��܂��B<br>
 *			�ُ�I���̏ꍇ�A�����e���|�����t�@�C�����쐬���Ă���΂������
 *			�����܂��B
 */
BOOL CFileOperation::CopyToTmpFile(const CString& strSrcFilePath,	/**< �����t�@�C���� */
								   const CString& strDstDirPath,	/**< �R�s�[��f�B���N�g���� */ 
								   const CString& strPrefix,		/**< �t������ړ��� */
								   CString& strTempName				/**< �e���|�����t�@�C�����i�t���p�X�j */
								   )
{
	// ���j�[�N�ȃt�@�C���������e���|�����t�@�C�����쐬����
	if(!::GetTempFileName(strDstDirPath, strPrefix, 0, strTempName.GetBuffer(MAX_PATH)))
	{
		strTempName.ReleaseBuffer();					// �ꉞ
		return FALSE;
	}
	strTempName.ReleaseBuffer();

	// �t�@�C���R�s�[
	if(!CopyFile(strSrcFilePath, strTempName, FALSE))
	{
		// �쐬�����e���|�����t�@�C�����폜����
		::_tremove(strTempName);
		return FALSE;
	}

	return TRUE;
}

/**
 * �w�肵���g���q�̃e���|�����t�@�C���ɃR�s�[
 *
 * ���j�[�N�Ȗ��O�Ɗg���q�����e���|�����t�@�C�����w�肳�ꂽ�t�H���_���ɍ쐬�����B
 * �e���|�����t�@�C�����ɂ�Prefix��Extension���w�肷�邱�ƁB
 * �쐬���ꂽ�e���|�����t�@�C���Ƀt�@�C�����R�s�[�����B<br>
 * ���ꂽ���j�[�N�Ȋg���q�̃e���|�����t�@�C����
 * Windows �̃V���b�g�_�E�����Ɏ����I�ɍ폜����邱�Ƃ͂���܂���B<br>
 * ���t�@�C���͂��̂܂܎c��܂��B
 *
 * @return	����I���̏ꍇ��TRUE���A�ُ�I���̏ꍇ��FALSE��Ԃ��܂��B<br>
 *			�ُ�I���̏ꍇ�A�����e���|�����t�@�C�����쐬���Ă���΂������
 *			�����܂��B
 */
BOOL CFileOperation::CopyToTmpFileWithExtension(
							const CString& strSrcFilePath,	/**< �����t�@�C���� */
							const CString& strDstDirPath,	/**< �R�s�[��f�B���N�g���� */
							const CString& strPrefix,		/**< �t������ړ��� */
							const CString& strExtension,	/**< �쐬����g���q */
							CString& strTempName			/**< �e���|�����t�@�C�����i�t���p�X�j */
							)
{
	// ���j�[�N�ȃt�@�C���������e���|�����t�@�C�����쐬����
	if(!::GetTempFileName(strDstDirPath, strPrefix, 0, strTempName.GetBuffer(MAX_PATH)))
	{
		strTempName.ReleaseBuffer();					// �ꉞ
		return FALSE;
	}
	strTempName.ReleaseBuffer();

	if(IsExist(strTempName))
	{// ��t�@�C�����쐬�����̂��폜
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

	// �t�@�C���R�s�[
	if(!CopyFile(strSrcFilePath, strTempName, FALSE))
	{
		// �쐬�����e���|�����t�@�C�����폜����
		::_tremove(strTempName);
		return FALSE;
	}

	return TRUE;
}

/**
 * �t�@�C���ړ�
 *
 * �t�@�C���܂��̓f�B���N�g�����i�f�B���N�g���̏ꍇ�́A���̎q���܂߁j�ړ����A
 * �w�肳�ꂽ�ꍇ�͖��O�̕ύX���s���܂��B�ړ���́A�����f�B���N�g���ł�����
 * ���܂��񂵁i���̏ꍇ�͖��O�̕ύX�̂݁j�A���̃f�B���N�g���ł����܂��܂���B<br>
 * <em>�������A�f�B���N�g���𑼂̃{�����[���ֈړ����悤�Ƃ���ƁAMoveFile
 * �֐������s���邱�Ƃɒ��ӂ��Ă��������B</em>
 *
 * @return	����I���̏ꍇ��TRUE���A�ُ�I���̏ꍇ��FALSE��Ԃ��܂��B
 */
BOOL CFileOperation::MoveFile(const CString& strSrcFilePath, /**< �����t�@�C���� */
							  const CString& strDstFilePath	 /**<  �V�����t�@�C����*/)
{
	if(::MoveFile(strSrcFilePath, strDstFilePath))
	{
		return TRUE;
	}

	return FALSE;
}

/**
 * �t�@�C�����l�[��
 *
 * strSrcPath�Ŏw�肵���t�@�C���܂��̓f�B���N�g���̖��O�A
 * strDetPath�Ŏw�肵�����O�ɕύX���܂��B
 *
 * @return	����I���̏ꍇ��TRUE���A�ُ�I���̏ꍇ��FALSE��Ԃ��܂��B
 */
BOOL CFileOperation::RenameFile(const CString& strSrcPath, /**< �����t�@�C���܂��̓f�B���N�g���̃p�X */
								const CString& strDetPath  /**< �ύX�t�@�C���܂��̓f�B���N�g���̃p�X */)
{
	if(!::_trename(strSrcPath, strDetPath))
	{
		return FALSE;
	}
	return TRUE;
}

/**
 * �t�@�C���폜
 *
 * �t�@�C�����폜���܂��B<br>
 * ReadOnly�ȃt�@�C�����������������Ă���폜���܂��B
 *
 * @return	����I���̏ꍇ��TRUE���A�ُ�I���̏ꍇ��FALSE��Ԃ��܂��B
 */
BOOL CFileOperation::DeleteFile(const CString& strFilePath /**< �Ώۃp�X */)
{
	// ReadOnly��������
	DWORD dwAttributes = ::GetFileAttributes(strFilePath);
	if(dwAttributes & FILE_ATTRIBUTE_READONLY)
	{
		if(!::SetFileAttributes(strFilePath, dwAttributes & ~FILE_ATTRIBUTE_READONLY))
		{
			return FALSE;
		}
	}

	// �t�@�C���폜
	if(!::DeleteFile(strFilePath))
	{
		return FALSE;
	}
	return TRUE;
}

/**
 * �f�B���N�g���쐬
 *
 * �f�B���N�g�����쐬���܂��B
 *
 * @return	����I���̏ꍇ��TRUE���A�ُ�I���̏ꍇ��FALSE��Ԃ��܂��B
 */
BOOL CFileOperation::CreateDirectory(const CString& strDirPath /**< �쐬����f�B���N�g���p�X */)
{
	if(!::CreateDirectory(strDirPath, NULL))
	{
		return FALSE;
	}
	return TRUE;
}

//------------------------------------------------------
/**
 * ��f�B���N�g���폜
 *
 * ��̃f�B���N�g�����폜���܂��B<br>
 * ReadOnly�ȃf�B���N�g�����������������Ă���폜���܂��B<br>
 * ��łȂ��ꍇ�͎��s���܂��B
 *
 * @return	����I���̏ꍇ��TRUE���A�ُ�I���̏ꍇ��FALSE��Ԃ��܂��B
 * @see		CFileOperation::DeleteDirectory
 */
//------------------------------------------------------
BOOL CFileOperation::RemoveDirectory(const CString& strDirPath /**< �Ώۃp�X */)
{
	// ReadOnly��������
	DWORD dwAttributes = GetFileAttributes(strDirPath);
	if(dwAttributes & FILE_ATTRIBUTE_READONLY)
	{
		if(!::SetFileAttributes(strDirPath,
								dwAttributes & ~FILE_ATTRIBUTE_READONLY))
		{
			return FALSE;
		}
	}

	// �f�B���N�g���폜
	if(!::RemoveDirectory(strDirPath))
	{
		return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------
/**
 * �f�B���N�g���폜
 *
 * �f�B���N�g���𒆐g���ƍ폜���܂��B<br>
 * ReadOnly�ȃf�B���N�g����t�@�C�����������������Ă���폜���܂��B<br>
 *
 * @return	����I���̏ꍇ��TRUE���A�ُ�I���̏ꍇ��FALSE��Ԃ��܂��B
 * @see		CFileOperation::RemoveDirectory
 */
//------------------------------------------------------
BOOL CFileOperation::DeleteDirectory(const CString& strDirPath /**< �Ώۃp�X */)
{
	CFileFind Finder;
	BOOL bMoreFile = Finder.FindFile(ComFunc::GetPathCombine(strDirPath, _T("*.*")));
	while(bMoreFile)
	{
		bMoreFile = Finder.FindNextFile();

		// �f�B���N�g���̏ꍇ
		if(Finder.IsDirectory())
		{
			// "." ����� ".." �͖�������
			if(Finder.IsDots())
			{
				continue;
			}

			// �f�B���N�g���폜
			if(!DeleteDirectory(Finder.GetFilePath()))
			{
#ifdef _DEBUG
				CString strMsg;
				strMsg.Format(_T("\t%s�̍폜�Ɏ��s���܂����B\n"), Finder.GetFilePath());
				TRACE(strMsg);
#endif // _DEBUG
				// ���s���Ă����s����B
				//Finder.Close();
				//return FALSE;
			}
		}
		// �t�@�C���̏ꍇ
		else
		{
			// �t�@�C���폜
			if(!DeleteFile(Finder.GetFilePath()))
			{
#ifdef _DEBUG
				CString strMsg;
				strMsg.Format(_T("\t%s�̍폜�Ɏ��s���܂����B\n"), Finder.GetFilePath());
				TRACE(strMsg);
#endif // _DEBUG
				// ���s���Ă����s����B
//				Finder.Close();
//				return FALSE;
			}
		}
	}
	Finder.Close();

	// ���Y�f�B���N�g���폜
	if(!CFileOperation::RemoveDirectory(strDirPath))
	{
#ifdef _DEBUG
		CString strMsg;
		strMsg.Format(_T("\t%s�̍폜�Ɏ��s���܂����B\n"), strDirPath);
		TRACE(strMsg);
#endif // _DEBUG
		return FALSE;
	}

	return TRUE;
}

/**
 * �e���|�����t�@�C���쐬
 *
 * ���j�[�N�ȃt�@�C���������e���|�����t�@�C�����쐬���܂��B
 *
 * @return	����I���̏ꍇ�͍쐬�����e���|�����t�@�C���̃p�X���A�ُ�I
 *			���̏ꍇ�͋󕶎����Ԃ��܂��B
 */
CString CFileOperation::CreateTmpFileInDir(const CString& strDirPath,	/**< �쐬��f�B���N�g���̃p�X */
										   const CString& strPrefix		/**< �t������ړ��� */)
{
	CString strTmpFilePath;

	UINT uiResult = ::GetTempFileName(strDirPath, strPrefix, 0, strTmpFilePath.GetBuffer(MAX_PATH));

	strTmpFilePath.ReleaseBuffer();

	if(uiResult == 0) // �G���[
	{
		strTmpFilePath.Empty();
	}

	return strTmpFilePath;
}

/**
 * �p�X����t�@�C�������擾
 *
 * �p�X����t�@�C�������擾���܂��B
 *
 * @return	�t�@�C����
 */
CString CFileOperation::GetFileNameAtPath(const CString& strPath,/**< �Ώۃp�X */ 
										  BOOL bExt /*= TRUE*/ /**< �g���q�̗L�� */)
{
	ASSERT(!strPath.IsEmpty());

	CString strFileName;								// �t�@�C�����i�g���q�Ȃ��j
	CString strFileExt;									// �g���q

	// ����
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
 * �p�X����f�B���N�g�������擾
 *
 * �p�X����f�B���N�g�������擾���܂��B
 *
 * @return	�f�B���N�g�����i�p�X�j
 */
CString CFileOperation::GetDirNameAtPath(const CString& strPath /**< �Ώۃp�X */)
{
	ASSERT(!strPath.IsEmpty());

	CString strDrive;								// �h���C�u
	CString strDir;									// �f�B���N�g��

	// ����
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
 * �p�X����C�ӂ̕������擾
 *
 * �p�X����C�ӂ̕������擾���܂��B
 *
 * @return	�C�ӂ̕���
 */
CString CFileOperation::GetPartOfPathAtPath(const CString& strPath,	/**< �Ώۃp�X */
											const RETURNPART ePart	/**< �p�X���̎擾���� */)
{
	ASSERT(!strPath.IsEmpty());

	CString strDrive;								// �h���C�u
	CString strDir;									// �f�B���N�g��
	CString strFileName;								// �t�@�C�����i�g���q�Ȃ��j
	CString strFileExt;									// �g���q

	// ����
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
 * �G���[���b�Z�[�W�\��
 *
 * �G���[���b�Z�[�W��\�����܂��B<br>
 * GetLastError()�̓��e��\�����܂��B<br>
 * ���Debug�p�H
 */
void CFileOperation::ShowSystemErrorMessage()
{
	LPVOID pMessageBuffer;

	// �G���[������擾
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // ����̌���
		reinterpret_cast<LPTSTR>(&pMessageBuffer),
		0,
		NULL
	);

	// �������\������B
	::AfxMessageBox(reinterpret_cast<LPTSTR>(pMessageBuffer),MB_OK);

	// �o�b�t�@���������B
	LocalFree(pMessageBuffer);
}

/**
 *	�������݉\��
 *
 *	ReadOnly�������������鎖�ŏ������݉\�ɂ��܂��B
 *
 *	@return	����
 */
BOOL CFileOperation::Writable(const CString& strFilePath /**< �Ώۃp�X */)
{
	ASSERT(!strFilePath.IsEmpty());

	// �t�@�C�����݃`�F�b�N
	if(!IsExist(strFilePath))
	{
		return TRUE;
	}
	
	// ReadOnly��������
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
 * �t�@�C�����當����̓ǂݍ���
 *
 * @return �ǂݍ��񂾕�����
 */
CString CFileOperation::ReadString(CFile& cFile /**< �ǂݍ��ރt�@�C�� */)
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
	{// ������̏I�[�������炸
		nFind = 0;
		return _T("");
	}
	else
	{
		nFind += (int)((TCHAR*)lpFind - byTemp);
	}
	// �V�[�N�ʒu����
	cFile.Seek(ulPosTemp,CFile::begin);

	CString str;
	cFile.Read(str.GetBuffer(nFind+1),(nFind+1) * sizeof(TCHAR));
	str.ReleaseBuffer();

	return str;
}

/**
 * �G���[���b�Z�[�W�ݒ�
 */
void CFileOperation::SetErrString(ERR_CODE nErr, /**< �G���[�R�[�h */
								  TCHAR* pErr,	/**< �G���[���b�Z�[�W */
								  LPCTSTR /*pSub1*/,/**< �����g�p */
								  LPCTSTR /*pSub2*/ /**< �����g�p */)
{
	if(pErr == FALSE)
	{
		return;
	}
}

/**
 * �G���[���b�Z�[�W�ݒ�
 */
void CFileOperation::SetSysErrString(TCHAR* pErr,	/**< �G���[���b�Z�[�W */
									 CException* p	/**< ��O */)
{
	#define BUFF_LEN 256
	TCHAR tcSub1[BUFF_LEN];

	p->GetErrorMessage(tcSub1,BUFF_LEN);

	SetErrString(ERR_SYSTEM,pErr,tcSub1);
}

/**
 * �t�@�C������
 *
 * @return ����
 */
BOOL CFileOperation::SplitFile(LPCTSTR lpszFile, /**< �Ώۃt�@�C���� */
							   CFile* pFile,	 /**< �ǂݍ��ރt�@�C�� */
							   LPTSTR pErr /**< �G���[���b�Z�[�W */)
{
	CFile cTempFile;
    BYTE pReadBuf[LENGTH_BUFFER];           // ���̓o�b�t�@

	try
	{
		if( !cTempFile.Open(lpszFile,CFile::modeWrite|CFile::modeCreate))
		{// ���s
			// �G���[�Z�b�g
			CFileOperation::SetErrString( CFileOperation::ERR_FILEOPEN,pErr);
			return FALSE;
		}

		BOOL bRet = TRUE;
		do
		{
			// �f�[�^�t�@�C���T�C�Y�ǂݍ���
			ULONGLONG ullFileSize;
			if( pFile->Read(&ullFileSize, sizeof(ULONGLONG)) !=  sizeof(ULONGLONG))
			{// ���s
				bRet = FALSE;
				// �G���[�Z�b�g
				CFileOperation::SetErrString( CFileOperation::ERR_READ_LEN,pErr);
				break;
			}
			TRACE("SplitFileSize= %ld \n",ullFileSize);

			// �f�[�^�t�@�C���̏o��
			while(ullFileSize >= LENGTH_BUFFER)
			{
				int nRCnt = pFile->Read(pReadBuf,LENGTH_BUFFER);
				if(LENGTH_BUFFER != nRCnt)
				{// 
					cTempFile.Write(pReadBuf,nRCnt);
					// �G���[�Z�b�g
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
				{// ���s
					// �G���[�Z�b�g
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
		// �G���[�Z�b�g
		CFileOperation::SetSysErrString(pErr,p);
		cTempFile.Close();
		p->Delete();
		return FALSE;
	}
}

/**
 * �t�@�C���̑��݊m�F
 */
BOOL CFileOperation::IsExistFile(LPCTSTR lpszFiler /**< ���ׂ�t�@�C���� */)
{ 
    CFileFind FileFind;
    
    // ���ׂẴt�@�C��������
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
 * �f�B���N�g���̑��݊m�F
 *
 * @return ����
 */
BOOL CFileOperation::IsExistDirectory(LPCTSTR lpszFiler /**< ���ׂ�f�B���N�g���� */)
{ 
    CFileFind FileFind;
    
    // ���ׂẴt�@�C��������
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
 * �`�F�b�N�T���̌v�Z
 *
 * @return �G���[�R�[�h
 */
int CFileOperation::CalcCheckSum(LPCTSTR lpszFile, /**< �t�@�C���� */
								 DWORD& dwSum /**< �`�F�b�N�T�� */)
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
		{// �I�[�v���G���[
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
 * �t�@�C�����S�~���ֈړ�
 *
 * @return ����
 */
BOOL CFileOperation::TrashFile(const CString& strFilePath /**< �Ώۃp�X */)
{
	// double null-terminated �ɂ���
	CString strTemp(strFilePath);
	TCHAR* pStart = strTemp.GetBuffer(strTemp.GetLength() + 1 + 1);
	TCHAR* p =  pStart + strTemp.GetLength();
	*( p + 1 ) = 0;

	// SHFILEOPSTRUCT �\���̃����o�̒l��ݒ�
	SHFILEOPSTRUCT FileOp;
	FileOp.hwnd = NULL;
	FileOp.wFunc = FO_DELETE;
	FileOp.pFrom = pStart;
	FileOp.pTo = NULL; // pTo is ignored.
	FileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
	FileOp.fAnyOperationsAborted = FALSE;
	FileOp.hNameMappings = NULL;
	FileOp.lpszProgressTitle = NULL;

	// �t�@�C�����폜
	int iRet = SHFileOperation( &FileOp );

	strTemp.ReleaseBuffer();

	return iRet == 0;
}


//! �t�@�C����������
BOOL  CFileOperation::AddWriteSJISFile(LPCTSTR lpszFile,const CString& strBuff)
{
#ifdef _UNICODE
	std::string strChar = CW2A(strBuff);
#else
	std::string strChar = strBuff;
#endif
	CFile cFile;
	if(IsExistFile(lpszFile))
	{// ���ɑ���
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

//! ��������
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

//! �Ǎ���
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

