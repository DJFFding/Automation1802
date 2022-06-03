
// ModifyPythonFile.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "ModifyPythonFile.h"
#include "ModifyPythonFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CModifyPythonFileApp

BEGIN_MESSAGE_MAP(CModifyPythonFileApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CModifyPythonFileApp 构造

CModifyPythonFileApp::CModifyPythonFileApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CModifyPythonFileApp 对象

CModifyPythonFileApp theApp;


// CModifyPythonFileApp 初始化

BOOL CModifyPythonFileApp::InitInstance()
{
	CWinApp::InitInstance();
	HANDLE hBdDes = CreateBoundaryDescriptor(_T("dxy"), 0);
	SID dxy_sid;
	DWORD dxy_sid_size = sizeof(dxy_sid);
	CreateWellKnownSid(WinWorldSid, NULL, &dxy_sid, &dxy_sid_size);
	AddSIDToBoundaryDescriptor(&hBdDes, &dxy_sid);
	HANDLE hPrivateName = CreatePrivateNamespace(NULL, hBdDes, _T("dxy"));
	if (hPrivateName == NULL)
	{
		hPrivateName = OpenPrivateNamespace(hBdDes, _T("dxy"));
	}
	HANDLE hMutex = CreateMutex(NULL, TRUE, _T("dxy\\dxy"));

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return FALSE;
	}

	

	CModifyPythonFileDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	return FALSE;
}



BOOL CModifyPythonFileApp::modifyPythonFile(CString fileName)
{
	UINT rIndex = fileName.ReverseFind(_T('.'));
	CString suffix = fileName.Right(fileName.GetLength() - rIndex - 1);
	if (suffix!="py")
	{
		return FALSE;
	}
	HANDLE hFile = CreateFile(fileName, GENERIC_READ | GENERIC_WRITE,0,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
	if (GetLastError()== ERROR_FILE_NOT_FOUND)
	{
		return FALSE;
	}
	HANDLE hMapFile = CreateFileMapping(
	hFile,NULL, PAGE_READWRITE,0,0,NULL
	);
	LPVOID memAddr = MapViewOfFile(hMapFile,
		FILE_MAP_WRITE,
		0, 0, 0);

	FileMode mode = getFileMode(memAddr);
	PWCHAR content = NULL;
	if (mode==ANSI)
	{
		int fileSize = MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED,
			(LPCCH)memAddr, -1, NULL, 0);
		content = new WCHAR[fileSize];
		MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED,
			(LPCCH)memAddr, -1, content, fileSize);
	}
	else if (mode == UTF16BE)
	{
		char* _cProofread = (char*)memAddr + 2;
		if (isLittleEndian())
		{
			swapByte(_cProofread);	
		}
		content = (WCHAR*)_cProofread;
		CString handledContent = handlePythonFile(content);
		memset(content, 0, GetFileSize(hFile, NULL) - 2);
		memcpy_s(content, GetFileSize(hFile, NULL) - 2, handledContent.GetBuffer(), handledContent.GetLength());
	}
	else if (mode==UTF16LE)
	{
		char* _cProofread = (char*)memAddr + 2;
		if (!isLittleEndian())
		{
			swapByte(_cProofread);
		}
		content = (WCHAR*)_cProofread;
		CString handledContent = handlePythonFile(content);
		memset(content, 0, GetFileSize(hFile, NULL) - 2);
		memcpy_s(content, GetFileSize(hFile, NULL) - 2, handledContent.GetBuffer(), handledContent.GetLength());
	}
	else {
		int fileSize = MultiByteToWideChar(CP_UTF8,
				MB_PRECOMPOSED,
				(LPCCH)memAddr, -1, NULL, 0);
		content = new WCHAR[fileSize];
		MultiByteToWideChar(CP_UTF8,
			MB_PRECOMPOSED,
			(LPCCH)memAddr, -1, content, fileSize);
		CString handledContent = handlePythonFile(content);
		char* pFileFormat = static_cast<char*>(memAddr);
		if ((pFileFormat[0] == 0xef) &&
			(pFileFormat[1] == 0xbb) &&
			(pFileFormat[2] = 0xbf))
		{
			memset(pFileFormat+3, 0,handledContent.GetLength()*2);
			fileSize = WideCharToMultiByte(CP_UTF8,
				WC_NO_BEST_FIT_CHARS | WC_COMPOSITECHECK | WC_DEFAULTCHAR,
				handledContent.GetBuffer(), -1,
				NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_UTF8,
				WC_NO_BEST_FIT_CHARS | WC_COMPOSITECHECK | WC_DEFAULTCHAR,
				handledContent.GetBuffer(), -1,
				(LPSTR)memAddr+3, fileSize, NULL, NULL);
		}
		else {
			memset(memAddr, 0, handledContent.GetLength() * 2);
			fileSize = WideCharToMultiByte(CP_UTF8,
				WC_NO_BEST_FIT_CHARS | WC_COMPOSITECHECK | WC_DEFAULTCHAR,
				handledContent.GetBuffer(), -1,
				NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_UTF8,
				WC_NO_BEST_FIT_CHARS | WC_COMPOSITECHECK | WC_DEFAULTCHAR,
				handledContent.GetBuffer(), -1,
				(LPSTR)memAddr, fileSize, NULL, NULL);
		}
		
		delete[] content;
	}
	
	
	
	UnmapViewOfFile(memAddr);
	CloseHandle(hMapFile);
	CloseHandle(hFile);
	
	return TRUE;
}
void CModifyPythonFileApp::swapByte(char * con)
{
	int i = 0;
	while (
		!(con[i]==0&&con[i+1]==0)
		)
	{
		char tmp = con[i];
		con[i] = con[i + 1];
		con[i + 1] = tmp;
		i += 2;
	}


}
BOOL CModifyPythonFileApp::isLittleEndian()
{
	int num = 1;
	int*p = &num;
	char* p2 = (char*)p;
	return *p2==1;
}
CModifyPythonFileApp::FileMode CModifyPythonFileApp::getFileMode(LPVOID memAddr)
{
	FileMode mode = b_default8?UTF8:ANSI;
	char* pFileFormat = static_cast<char*>(memAddr);
	if (pFileFormat[0]==0xff&&pFileFormat[1]==0xfe)
	{
		mode = UTF16LE;
	}
	else if (pFileFormat[0]==0xfe&&pFileFormat[1]==0xff)
	{
		mode = UTF16BE;
	}
	else if ((pFileFormat[0]== 0xef)&&
		(pFileFormat[1]==0xbb)&&
		(pFileFormat[2]=0xbf))
	{
		mode = UTF8;
	}

	return mode;
}

CString CModifyPythonFileApp::handlePythonFile(CString content)
{
	for (int i = 0; i < content.GetLength(); i++)
	{
		if (content[i]==9)
		{
			content.SetAt(i, _T(' '));
			content.Insert(i, _T("   "));
		}
	}




	CString handled;
	int iLine = 0;
	FileModify fm;
	fm.spaceNum = 0;
	fm.nextAdd4space = false;
	while (!content.IsEmpty())
	{
		int index = content.Find(_T('\n'))==-1?content.GetLength()-1: content.Find(_T('\n'));
		CString line = content.Left(index + 1);
		if (iLine==0)
		{
			int iSharp = line.Find(_T('#')) == -1 ? line.GetLength() : line.Find(_T('#'));
			for (int i = 0; i < iSharp; i++)
			{
				if (line[i]==_T(' '))
				{
					continue;
				}
				if (i!=0&&line[i-1]==_T(' '))
				{
					line = line.Mid(i);
				}
				if (line[i]==_T(':'))
				{
					fm.nextAdd4space = true;
					fm.spaceNum = 0;
					break;
				}
			}
		}
		else {
			int iSharp = line.Find(_T('#'))==-1? line.GetLength(): line.Find(_T('#'));
			if (fm.nextAdd4space)
			{
				fm.spaceNum = fm.spaceNum + 4;
			}
			fm.nextAdd4space = false;
			int spaceNum = 0;
			for (int i = 0; i < iSharp; i++)
			{
				if (line[i]==_T(' '))
				{
					spaceNum++;
					continue;
				}
				break;
			}
			
			if ((spaceNum%4)==0)
			{
				if (spaceNum>fm.spaceNum)
				{
					line.Delete(fm.spaceNum, spaceNum - fm.spaceNum);
				}
				else {
					fm.spaceNum = spaceNum;
				}
			}
			else {
				if (fm.spaceNum<=spaceNum)
				{
					line.Delete(fm.spaceNum, spaceNum - fm.spaceNum);
				}
				else {
					for (int j = 0; j < fm.spaceNum - spaceNum; j++)
					{
						line.Insert(spaceNum,_T(' '));
					}	
				}
			}
			iSharp = line.Find(_T('#')) == -1 ? line.GetLength() : line.Find(_T('#'));
			for (int i = fm.spaceNum; i < iSharp; i++)
			{
				if (line[i] == _T(':'))
				{
					fm.nextAdd4space = true;
					fm.spaceNum = fm.spaceNum + 4;
					break;
				}
			}
		}
		handled += line;
		if (index == -1)
		{
			break;
		}
		content = content.Mid(index + 1);
		iLine++;
	}
	

	return handled;
}