
// winClientMFCDlg.h: 头文件
//

#pragma once
#include "afxwin.h"


// CwinClientMFCDlg 对话框
class CwinClientMFCDlg : public CDialog
{
// 构造
public:
	CwinClientMFCDlg(CWnd* pParent = NULL);	// 标准构造函数

public:
	//初始化连接socket
	void InitConnect() {
		CwinClientMFCApp* App = (CwinClientMFCApp*)AfxGetApp();
		CString err;
		App->iResult = App->ConnectServer();
		switch (App->iResult)
		{
		case 1:
			err.Format(_T("%d"), WSAGetLastError());
			news.AddString(L"Error at socket():" + err);
			return;
		case 2:
			news.AddString(L"Unable to connect to server!");
			return;
		default:
			break;
		}
		//接收欢迎
		App->iResult = recv(App->ConnectSocket, App->recvbuf, App->recvbuflen, 0);
		if (App->iResult > 0) {
			CString newStr(App->recvbuf);
			news.AddString(newStr);
			err.Format(_T("%d"), App->iResult);
			news.AddString(L"Bytes received:" + err);
			news.AddString(L"输入要下载的文件名");
			//printf("server:%s\n", App->recvbuf);
			//printf("Bytes received: %d\n", App->iResult);
		}
	}
	//接收文件
	void RecvFIle(FILE*& fp) {
		CwinClientMFCApp* App = (CwinClientMFCApp*)AfxGetApp();
		CString err;
		if (fp == NULL) {
			news.AddString(L"file" + CString(App->oldFile.name) + "can not open");
			//printf("file %s can not open", App->oldFile.name);
			return;
		}

		//printf("server:file:%s;size:%fKB\n", oldFile.name, oldFile.size);
		news.AddString(L"server:file:" + CString(App->oldFile.name));
		CString fileSize;
		fileSize.Format(_T("%f"), (App->oldFile.size));
		news.AddString(L"server:size:" + fileSize);
		//ZeroMemory(recvbuf, sizeof(recvbuf));
		ZeroMemory((char*)&(App->fileS), sizeof(App->fileS));
		//接收文件
		while (App->iResult = recv(App->ConnectSocket, (char*)&(App->fileS), sizeof(App->fileS), 0)) {
			if (App->iResult > 0) {
				//printf("server send file bytes:%d;ID:%ld\n", iResult, oldFile.id);
				err.Format(_T("%d"), App->iResult);
				news.AddString(L"server send file bytes:" + err);
				err.Format(_T("%d"), App->oldFile.id);
				news.AddString(L"ID:" + err);
				//计算文件包ID
				if (App->oldFile.id != App->fileS.id) {
					App->oldFile.id--;
					//break;
				}
				else
					App->oldFile.id++;
				int writeLen = fwrite(App->fileS.content, sizeof(char), BUF_SIZE, fp);

				App->oldFile.end = App->fileS.end;
				ZeroMemory((char*)&(App->fileS), sizeof(App->fileS));
			}
			else {
				news.AddString(L"接收文件失败");
				//printf("recv file %s failed: %d\n", App->oldFile.name, WSAGetLastError());
				closesocket(App->ConnectSocket);
				break;
			}
		}
		if (App->oldFile.end) {
			news.AddString(L"文件下载完成");
			//printf("file %s download finished\n", App->oldFile.name);
			//计算用时
			App->nowT = time(NULL);
			err.Format(_T("%d"), (App->nowT - App->oldT));
			news.AddString(L"耗时:" + err);
			//printf("Time:%lds\n", App->nowT - App->oldT);
		}
		else
			news.AddString(L"文件下载失败，请尝试重传");
		//printf("file %s download interrupted,try again\n", App->oldFile.name);
		fclose(fp);
	}

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WINCLIENTMFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CString serverIp;
	CString serverPort;
	CListBox news;
	CString fileName;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};
