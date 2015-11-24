// ChatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Chat.h"
#include "ChatDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChatDlg dialog

CChatDlg::CChatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChatDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChatDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChatDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChatDlg, CDialog)
	//{{AFX_MSG_MAP(CChatDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SEND, OnBtnSend)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_RECVDATA,OnRecvData)
	ON_BN_CLICKED(IDC_BTN_SHOW, &CChatDlg::OnBnClickedBtnShow)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChatDlg message handlers

BOOL CChatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	InitSocket();//socket初始化

	m_pBmpInfo = (BITMAPINFO *)m_chBmpBuf;
	m_pBitmapInfoHeader = (BITMAPINFOHEADER*)m_chBmpBuf;
	bool readflag = readBmp("2222.bmp");//图像数据初始化
	ASSERT(readflag);
	pBmpBufRecv = new BYTE[bmpHeight*lineByte];
	ASSERT(pBmpBufRecv);

	RECVPARAM *pRecvParam=new RECVPARAM;//结构体初始化
	ASSERT(pRecvParam);
	pRecvParam->sock=m_socket;
	pRecvParam->hwnd=m_hWnd;
	pRecvParam->lineByte = lineByte;
	pRecvParam->height = bmpHeight;
	pRecvParam->data = pBmpBufRecv;

	HANDLE hThread=CreateThread(NULL,0,RecvProc,(LPVOID)pRecvParam,0,NULL);//开辟线程
	CloseHandle(hThread);//关闭线程句柄 但是并没有结束线程 句柄是一种系统资源 用了要还

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChatDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChatDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CChatDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CChatDlg::InitSocket()
{
	m_socket=socket(AF_INET,SOCK_DGRAM,0);//采用udp报表套接字
	if(INVALID_SOCKET==m_socket)
	{
		MessageBox("套接字创建失败！");
		return FALSE;
	}
	SOCKADDR_IN addrSock;
	addrSock.sin_family=AF_INET;
	addrSock.sin_port=htons(6000);
	addrSock.sin_addr.S_un.S_addr=htonl(INADDR_ANY);

	int retval;
	retval=bind(m_socket,(SOCKADDR*)&addrSock,sizeof(SOCKADDR)); //绑定操作
	if(SOCKET_ERROR==retval)
	{
		closesocket(m_socket);
		MessageBox("绑定失败!");
		return FALSE;
	}
	return TRUE;

}

DWORD WINAPI CChatDlg::RecvProc(LPVOID lpParameter)
{
	SOCKET sock=((RECVPARAM*)lpParameter)->sock;
	HWND hwnd=((RECVPARAM*)lpParameter)->hwnd;
	int lineByte = ((RECVPARAM*)lpParameter)->lineByte;
	int height = ((RECVPARAM*)lpParameter)->height;
	BYTE* data = ((RECVPARAM*)lpParameter)->data;
	char *data2 = (char*)data;
	delete lpParameter;	//视频讲述时，遗忘了释放内存的操作。sunxin

	SOCKADDR_IN addrFrom;
	int len=sizeof(SOCKADDR);

	int retval;
	int total = lineByte*height;
	int dwLen = 0;
	while(TRUE)
	{
		dwLen = 0;
		while(dwLen<total)
		{
			if (total-dwLen > 1024)
			{
				retval=recvfrom(sock,data2,1024,0,(SOCKADDR*)&addrFrom,&len);
				if (SOCKET_ERROR == retval)
				{
					AfxMessageBox("recv data failed!");
					return -1;
				}
				data2 += 1024;
				dwLen += 1024;
			}
			else
			{
				retval=recvfrom(sock,data2,total-dwLen,0,(SOCKADDR*)&addrFrom,&len);
				if (SOCKET_ERROR == retval)
				{
					AfxMessageBox("recv data failed!");
					return -1;
				}
				dwLen = total;
			}
		}
		//sprintf(tempBuf,"%s说: %s",inet_ntoa(addrFrom.sin_addr),recvBuf);
		::PostMessage(hwnd,WM_RECVDATA,0,(LPARAM)data);
	}
	return 0;
}

LRESULT CChatDlg::OnRecvData(WPARAM wParam,LPARAM lParam)
{
	BYTE* data=(BYTE*)lParam;//可以直接转
	CWnd* CtrWnd = (CWnd*)GetDlgItem(IDC_STC_PIC);
	CDC *pDC = CtrWnd->GetDC();
	CRect Rect;
	CtrWnd->GetWindowRect(&Rect);

	SetStretchBltMode(pDC->GetSafeHdc(),COLORONCOLOR);
	StretchDIBits(pDC->GetSafeHdc(),
		0,
		0,
		Rect.Width(),
		Rect.Height(),
		0,
		0,
		bmpWidth,
		bmpHeight,
		data,
		m_pBmpInfo,
		DIB_RGB_COLORS,
		SRCCOPY);

	CtrWnd->ReleaseDC(pDC);

	return LRESULT(1);
}

void CChatDlg::OnBtnSend()  
{
	// TODO: Add your control notification handler code here
	DWORD dwIP;
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1))->GetAddress(dwIP);

	SOCKADDR_IN addrTo;
	addrTo.sin_family=AF_INET;
	addrTo.sin_port=htons(6000);
	addrTo.sin_addr.S_un.S_addr=htonl(dwIP);
	char* data = (char*)pBmpBuf;

	//这里发送的时候 每次发送1024字节 发完为止
	int total = lineByte*bmpHeight;
	int dwlen = 0;
	int key = 0;
	while(dwlen < total)
	{
		if (total - dwlen > 1024)
		{
			key = sendto(m_socket,data,1024,0,
				(SOCKADDR*)&addrTo,sizeof(SOCKADDR));
			if (SOCKET_ERROR == key)
			{
				MessageBox("send data failed!");
				return;
			}
			data += 1024;
			dwlen += 1024;
		}
		else
		{
			key = sendto(m_socket,data,total-dwlen,0,
				(SOCKADDR*)&addrTo,sizeof(SOCKADDR));
			if (SOCKET_ERROR == key)
			{
				MessageBox("send data failed!");
				return;
			}
			dwlen = total;
		}

		//int index = WSAGetLastError();
	}
}

bool CChatDlg::readBmp(char* bmpName)
{
	FILE* fp = fopen(bmpName,"rb");          //以二进制读的方式打开指定的图像文件
	if(fp == 0) return 0;
	//跳过位图文件头
	fseek(fp,sizeof(BITMAPFILEHEADER),0);
	//定义位图信息头结构变量，读取位图信息头进内存，存放在变量infoHead中
	//BITMAPINFOHEADER infoHead;
	fread(m_pBitmapInfoHeader,sizeof(BITMAPINFOHEADER),1,fp);
	bmpWidth = m_pBitmapInfoHeader->biWidth;
	bmpHeight = m_pBitmapInfoHeader->biHeight;
	biBitCount = m_pBitmapInfoHeader->biBitCount;
	//定义变量，计算图像每行像素所占的字节数（必须为４的倍数）
	lineByte=(bmpWidth * biBitCount/8 + 3)/4 * 4;
	//灰度图像有颜色表，且颜色表为256
	if(biBitCount == 8)
	{
		//申请颜色表所需要的空间，读颜色表进内存
		pColorTable = new RGBQUAD[256];
		fread(pColorTable,sizeof(RGBQUAD),256,fp);
	}
	//申请位图数据所需要的空间，读位图数据进内存
	pBmpBuf = new BYTE[lineByte * bmpHeight];
	//pBmpBufRecv = new BYTE[lineByte * bmpHeight];
	if (!pBmpBuf /*|| !pBmpBufRecv*/)
		return 0;
	fread(pBmpBuf,1,lineByte * bmpHeight,fp);
	fclose(fp);

	//showImg(bmpName);

	return true;
}

void CChatDlg::showImg()
{
	CWnd* CtrWnd = (CWnd*)GetDlgItem(IDC_PIC);
	CDC *pDC = CtrWnd->GetDC();
	CRect Rect;
	CtrWnd->GetWindowRect(&Rect);

	SetStretchBltMode(pDC->GetSafeHdc(),COLORONCOLOR);
	StretchDIBits(pDC->GetSafeHdc(),
		0,
		0,
		Rect.Width(),
		Rect.Height(),
		0,
		0,
		bmpWidth,
		bmpHeight,
		pBmpBuf,
		m_pBmpInfo,
		DIB_RGB_COLORS,
		SRCCOPY);

	CtrWnd->ReleaseDC(pDC);
}

void CChatDlg::OnBnClickedBtnShow()
{
	// TODO: 在此添加控件通知处理程序代码
	//readBmp("002.bmp");
	showImg();
}

void CChatDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	if (!pBmpBuf)
	{
		delete[] pBmpBuf;
		pBmpBuf = NULL;
	}
	if (!pBmpBufRecv)
	{
		delete[] pBmpBufRecv;
		pBmpBufRecv = NULL;
	}
	if (!pColorTable)
	{
		delete[] pColorTable;
		pColorTable = NULL;
	}

}
