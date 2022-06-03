
// ModifyPythonFileDlg.h: 头文件
//

#pragma once


// CModifyPythonFileDlg 对话框
class CModifyPythonFileDlg : public CDialogEx
{
// 构造
public:
	CModifyPythonFileDlg(CWnd* pParent = nullptr);	// 标准构造函数
	
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MODIFYPYTHONFILE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSelected();
	CStatic m_staticFileName;
	CSize m_size;
	CFont m_staticFont;
	virtual void PostNcDestroy();
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	virtual void OnOK();
};
