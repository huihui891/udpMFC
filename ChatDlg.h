// ChatDlg.h : header file
//

#if !defined(AFX_CHATDLG_H__CA158545_90FE_4D51_8EB9_8AD271AFBB86__INCLUDED_)
#define AFX_CHATDLG_H__CA158545_90FE_4D51_8EB9_8AD271AFBB86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CChatDlg dialog
#define	WM_RECVDATA		WM_USER+1  //自定义的消息
struct RECVPARAM                   //自定义的结构体 用来传输socket和窗口句柄
{
	SOCKET sock;
	HWND hwnd;
	int lineByte;
	int height;
	BYTE *data;
};
class CChatDlg : public CDialog
{
// Construction
public:
	static DWORD WINAPI RecvProc(LPVOID lpParameter);//静态的 线程函数
	BOOL InitSocket();
	CChatDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CChatDlg)
	enum { IDD = IDD_CHAT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CChatDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnSend();
	//}}AFX_MSG
	afx_msg LRESULT OnRecvData(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	SOCKET m_socket;

	//位图数据
	BYTE* pBmpBuf;//图像数据区
	int bmpWidth;
	int bmpHeight;
	int lineByte;
	RGBQUAD* pColorTable;//颜色表
	int biBitCount;//图像类型 像素位数
	BITMAPINFO *m_pBmpInfo;		//BITMAPINFO 结构指针，显示图像时使用
	char m_chBmpBuf[1024];	///< BIMTAPINFO 存储缓冲区，m_pBmpInfo即指向此缓冲区
	BITMAPINFOHEADER* m_pBitmapInfoHeader;

	//接受的位图数据 跟上文公用文件头 只是改用接收到的数据缓冲区
	BYTE* pBmpBufRecv;
public:
	bool readBmp(char* bmpName);
	void showImg();
	afx_msg void OnBnClickedBtnShow();
	afx_msg void OnDestroy();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHATDLG_H__CA158545_90FE_4D51_8EB9_8AD271AFBB86__INCLUDED_)
