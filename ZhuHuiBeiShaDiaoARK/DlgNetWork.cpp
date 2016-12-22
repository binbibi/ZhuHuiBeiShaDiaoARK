// DlgNetWork.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgNetWork.h"
#include "afxdialogex.h"
#include <Windows.h>
#include <IPHlpApi.h>


#pragma comment(lib, "Iphlpapi.lib") 
#pragma comment(lib, "WSOCK32.LIB")


// CDlgNetWork 对话框

IMPLEMENT_DYNAMIC(CDlgNetWork, CPropertyPage)

CDlgNetWork::CDlgNetWork()
	: CPropertyPage(CDlgNetWork::IDD)
	, m_tcp_count(0)
	, m_udp_count(0)
{

}

CDlgNetWork::~CDlgNetWork()
{
}

void CDlgNetWork::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NETWORK_LOOK, m_List_NetWorkLook);
}


BEGIN_MESSAGE_MAP(CDlgNetWork, CPropertyPage)
END_MESSAGE_MAP()


// CDlgNetWork 消息处理程序


BOOL CDlgNetWork::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	CString str;
	//设置有滚动条 如果宽度没有达到要求 就会崩溃;
	m_List_NetWorkLook.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES);
	m_List_NetWorkLook.InsertColumn(0,L"协议",0,50);
	m_List_NetWorkLook.InsertColumn(1,L"本地地址",0,140);
	m_List_NetWorkLook.InsertColumn(2,L"远程地址",0,140);
	m_List_NetWorkLook.InsertColumn(3,L"连接状态",0,100);
	m_List_NetWorkLook.InsertColumn(4,L"进程ID",0,50);
	m_List_NetWorkLook.InsertColumn(5,L"进程路径",0,350);

	EnumTcp();
	EnumUdp();

	str.Format(L"当前系统共有 %d 个Tcp %d 个Udp连接",m_tcp_count,m_udp_count);
	AfxGetMainWnd()->SetWindowTextW(str);

	return TRUE;
}


void CDlgNetWork::EnumTcp(void)
{
	MIB_TCPTABLE_OWNER_PID table;
    DWORD dwSize = sizeof(table),fret; 
    char *Tmp;
    in_addr tmp1;
	CStringA str;
	CString wstr;
    char tmp2[MAX_PATH];
	int item = 0;
	WCHAR* szPath = (WCHAR*)malloc(260*2);
	WCHAR* szDosPath = (WCHAR*)malloc(260*2);
	memset(szPath,'\0',260*2);
	memset(szDosPath,'\0',260*2);

    fret=GetExtendedTcpTable(&table,&dwSize,TRUE,AF_INET,TCP_TABLE_OWNER_PID_ALL,0);
    if(fret==ERROR_INSUFFICIENT_BUFFER)
    {
          LPBYTE lpBuf = new BYTE[dwSize];   
          PMIB_TCPTABLE_OWNER_PID pTable = (PMIB_TCPTABLE_OWNER_PID)lpBuf;
          fret=GetExtendedTcpTable(pTable,&dwSize,TRUE,AF_INET,TCP_TABLE_OWNER_PID_ALL,0);
          if(fret==NO_ERROR)
          {
              for(ULONG i=0;i<pTable->dwNumEntries;i++)   
              {   
				  item = m_List_NetWorkLook.GetItemCount();
				  
				  m_List_NetWorkLook.InsertItem(item,L"Tcp");

                  memcpy(&tmp1,&pTable->table[i].dwLocalAddr,4);
                  Tmp=inet_ntoa(tmp1);
                  
				  str.Format("%s:%d",Tmp,ntohs((u_short)pTable->table[i].dwLocalPort));
				  wstr = str;
				  m_List_NetWorkLook.SetItemText(item,1,wstr);

				  memcpy(&tmp1,&pTable->table[i].dwRemoteAddr,4);
                  Tmp=inet_ntoa(tmp1);
                  
				  str.Format("%s:%d",Tmp,ntohs((u_short)pTable->table[i].dwRemotePort));
				  wstr = str;
				  m_List_NetWorkLook.SetItemText(item,2,wstr);

				  GetTcpState(pTable->table[i].dwState,tmp2);
                  
				  str.Format("%s",tmp2);
				  wstr = str;
				  m_List_NetWorkLook.SetItemText(item,3,wstr);
			
				  wstr.Format(L"%ld",pTable->table[i].dwOwningPid);
				  m_List_NetWorkLook.SetItemText(item,4,wstr);

				  if(pTable->table[i].dwOwningPid == 4)
				  {
					  m_List_NetWorkLook.SetItemText(item,5,L"System");
				  }else if(GetProcessPath(pTable->table[i].dwOwningPid,szPath) != NULL)
				  {
					  if(NtPathToDosPathW(szPath,szDosPath))
						m_List_NetWorkLook.SetItemText(item,5,szDosPath);
					  else
						m_List_NetWorkLook.SetItemText(item,5,szPath);
				  }
                
               }   
			  m_tcp_count = pTable->dwNumEntries;
           }
     }

	free(szPath);
	free(szDosPath);
}


void CDlgNetWork::EnumUdp(void)
{
	    MIB_UDPTABLE_OWNER_PID  table;   
        DWORD dwSize = sizeof(table);   
        char *Tmp;
        in_addr tmp1;
		CStringA str;
		CString wstr;
		WCHAR* szPath = (WCHAR*)malloc(260*2);
		WCHAR* szDosPath = (WCHAR*)malloc(260*2);
		memset(szPath,'\0',260*2);
		memset(szDosPath,'\0',260*2);

		int item = 0;
        if (GetExtendedUdpTable(&table,&dwSize,TRUE,AF_INET,UDP_TABLE_OWNER_PID,0) ==ERROR_INSUFFICIENT_BUFFER)   
        {   
                LPBYTE lpBuf = new BYTE[dwSize];
                PMIB_UDPTABLE_OWNER_PID pTable = (PMIB_UDPTABLE_OWNER_PID)lpBuf;   
                if (GetExtendedUdpTable(pTable,&dwSize,TRUE,AF_INET,UDP_TABLE_OWNER_PID,0)==NO_ERROR)   
                {   
                        for(ULONG i=0;i<pTable->dwNumEntries;i++)   
                        {
							item = m_List_NetWorkLook.GetItemCount();
							m_List_NetWorkLook.InsertItem(item,L"Udp");
                            memcpy(&tmp1,&pTable->table[i].dwLocalAddr,4);
                            Tmp=inet_ntoa(tmp1);
                            
							str.Format("%s:%d",Tmp,ntohs((u_short)pTable->table[i].dwLocalPort));
							wstr = str;
							m_List_NetWorkLook.SetItemText(item,1,wstr);

							m_List_NetWorkLook.SetItemText(item,2,L"***");

							m_List_NetWorkLook.SetItemText(item,3,L"");


							wstr.Format(L"%ld",pTable->table[i].dwOwningPid);
							m_List_NetWorkLook.SetItemText(item,4,wstr);

							 if(pTable->table[i].dwOwningPid == 4)
							 {
								  m_List_NetWorkLook.SetItemText(item,5,L"System");
							  }else if(GetProcessPath(pTable->table[i].dwOwningPid,szPath) != NULL)
							  {
								  if(NtPathToDosPathW(szPath,szDosPath))
									m_List_NetWorkLook.SetItemText(item,5,szDosPath);
								  else
									m_List_NetWorkLook.SetItemText(item,5,szPath);
							  }

                        }   

						m_udp_count = pTable->dwNumEntries;
                }   
        }   

		free(szPath);
		free(szDosPath);
}


void CDlgNetWork::GetTcpState(ULONG dwState, char* str)
{
	 switch (dwState)
     {
                case 1:
                {
                        lstrcpyA(str,"CLOSED");
                        return;
                }
                case 2:
                {
                        lstrcpyA(str,"LISTENING");
                        return;
                }
                case 5:
                {
                        lstrcpyA(str,"ESTABLISHED");
                        return;
                }
                case 8:
                {
                        lstrcpyA(str,"CLOSE_WAIT");
                        return;
                }
                case 11:
                {
                        lstrcpyA(str,"TIME_WAIT");
                        return;
                }
                default:
                {
                        lstrcpyA(str,"UNKNOW");
                        return;
                }
       }
}


WCHAR* CDlgNetWork::GetProcessPath(DWORD pid, WCHAR* szPath)
{
	CString str;
	BOOL Io_Success = FALSE;
	ULONGLONG Buff = 0;
	DWORD dwRet = 0;

	memset(szPath,'\0',260*2);
	Io_Success = DeviceIoControl(g_device,IOCTL_GetProcessPath, &pid, sizeof(DWORD), szPath, 260*2,&dwRet,NULL);

	if(Io_Success)
		return szPath;
	else
		return NULL;
}
