
// ModifyPythonFileDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ModifyPythonFile.h"
#include "ModifyPythonFileDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CModifyPythonFileDlg 对话框



CModifyPythonFileDlg::CModifyPythonFileDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MODIFYPYTHONFILE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CModifyPythonFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SHOWFILEMNAME, m_staticFileName);
}

BEGIN_MESSAGE_MAP(CModifyPythonFileDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_DROPFILES()
	ON_WM_NCRBUTTONDOWN()
	ON_COMMAND(IDM_QUIT,OnCancel)
	ON_COMMAND(IDM_CHARACTER, OnSelected)
END_MESSAGE_MAP()


// CModifyPythonFileDlg 消息处理程序

BOOL CModifyPythonFileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	SetWindowPos(NULL, 0, 0, 440, 140, 
		 SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOMOVE);
	CRect windowRect;
	GetWindowRect(&windowRect);
	m_size = windowRect.Size();
	m_staticFont.CreatePointFont(120, _T("宋体"));
	m_staticFileName.SetFont(&m_staticFont, FALSE);
	CString content = _T("请托放python文件到对话框");
	CSize fontSize;
	GetTextExtentExPoint(m_staticFileName.GetDC()->m_hDC,
		content, content.GetLength(), 0, NULL, NULL, &fontSize);
	m_staticFileName.MoveWindow(0, (m_size.cy-fontSize.cy)/2, m_size.cx,fontSize.cy, TRUE);
	m_staticFileName.SetWindowText(content);
	return TRUE; 
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CModifyPythonFileDlg::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文
}





LRESULT CModifyPythonFileDlg::OnNcHitTest(CPoint point)
{
	LRESULT result = CDialogEx::OnNcHitTest(point);

	return result==HTCLIENT?HTCAPTION:result;
}


void CModifyPythonFileDlg::OnDropFiles(HDROP hDropInfo)
{
	CDialogEx::OnDropFiles(hDropInfo);
	UINT droppedFileCount = DragQueryFile(hDropInfo, -1, NULL, 0);
	CString fileName;
	for (int i = 0; i < droppedFileCount; i++)
	{
		fileName.Empty();
		TCHAR szFileName[MAX_PATH] = { 0 };
		DragQueryFile(hDropInfo, i, szFileName, MAX_PATH);
		fileName.Append(szFileName, MAX_PATH);
		int rIndex = fileName.ReverseFind(_T('\\'));
		fileName = fileName.Right(fileName.GetLength() - rIndex-1);
		rIndex = fileName.ReverseFind(_T('.'));
		CString suffix = fileName.Right(fileName.GetLength() - rIndex - 1);
		
		if (suffix==_T("py"))
		{
			CSize fontSize;
			GetTextExtentExPoint(m_staticFileName.GetDC()->m_hDC,
				fileName, fileName.GetLength(), 0, NULL, NULL, &fontSize);
			UINT height = ((fileName.GetLength()*fontSize.cx / m_size.cx) + 1)*fontSize.cy;
			m_staticFileName.SetWindowPos(NULL, 0, (m_size.cy - height) / 2, m_size.cx,
				height,
				SWP_NOMOVE | SWP_NOZORDER);
			m_staticFileName.SetWindowText(fileName);
			CString pathName = szFileName;
			((CModifyPythonFileApp*)AfxGetApp())->modifyPythonFile(pathName);
		}
	}



}

void CModifyPythonFileDlg::OnSelected()
{
	theApp.b_default8 = !theApp.b_default8;
}







void CModifyPythonFileDlg::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
}


void CModifyPythonFileDlg::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	CMenu contextMenu;
	contextMenu.CreatePopupMenu();
	if (theApp.b_default8)
	{
		contextMenu.AppendMenu(MF_POPUP | MF_STRING | MF_CHECKED,
			IDM_CHARACTER, _T("UTF8"));
	}
	else {
		contextMenu.AppendMenu(MF_POPUP | MF_STRING | MF_UNCHECKED,
			IDM_CHARACTER, _T("UTF8"));
	}
	contextMenu.AppendMenu(MF_POPUP | MF_STRING,
		IDM_QUIT, _T("退出"));
	contextMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_NONOTIFY
		, point.x, point.y, this, 0);
	contextMenu.DestroyMenu();
	CDialogEx::OnNcRButtonDown(nHitTest, point);
}


void CModifyPythonFileDlg::OnOK()
{
}
