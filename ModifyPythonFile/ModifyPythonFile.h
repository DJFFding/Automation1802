
// ModifyPythonFile.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// 主符号


// CModifyPythonFileApp:
// 有关此类的实现，请参阅 ModifyPythonFile.cpp
//

class CModifyPythonFileApp : public CWinApp
{
public:
	CModifyPythonFileApp();
	enum FileMode {
		UTF8,
		UTF16BE,
		UTF16LE,
		ANSI
	};
	bool b_default8=true;
// 重写
public:
	virtual BOOL InitInstance();
	BOOL modifyPythonFile(CString fileName);
	BOOL isLittleEndian();
	void swapByte(char * con);
// 实现
	CString handlePythonFile(CString content);
	DECLARE_MESSAGE_MAP()
private:
	FileMode getFileMode(LPVOID memAddr);

	struct FileModify {
		bool nextAdd4space;
		int spaceNum; //当前行的空格数
	};
};

extern CModifyPythonFileApp theApp;
