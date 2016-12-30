// DlgFilterDriver.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgFilterDriver.h"
#include "afxdialogex.h"

/*
#define FILE_DEVICE_BEEP                0x00000001
#define FILE_DEVICE_CD_ROM              0x00000002
#define FILE_DEVICE_CD_ROM_FILE_SYSTEM  0x00000003
#define FILE_DEVICE_CONTROLLER          0x00000004
#define FILE_DEVICE_DATALINK            0x00000005
#define FILE_DEVICE_DFS                 0x00000006
#define FILE_DEVICE_DISK                0x00000007
#define FILE_DEVICE_DISK_FILE_SYSTEM    0x00000008
#define FILE_DEVICE_FILE_SYSTEM         0x00000009
#define FILE_DEVICE_INPORT_PORT         0x0000000a
#define FILE_DEVICE_KEYBOARD            0x0000000b
#define FILE_DEVICE_MAILSLOT            0x0000000c
#define FILE_DEVICE_MIDI_IN             0x0000000d
#define FILE_DEVICE_MIDI_OUT            0x0000000e
#define FILE_DEVICE_MOUSE               0x0000000f
#define FILE_DEVICE_MULTI_UNC_PROVIDER  0x00000010
#define FILE_DEVICE_NAMED_PIPE          0x00000011
#define FILE_DEVICE_NETWORK             0x00000012
#define FILE_DEVICE_NETWORK_BROWSER     0x00000013
#define FILE_DEVICE_NETWORK_FILE_SYSTEM 0x00000014
#define FILE_DEVICE_NULL                0x00000015
#define FILE_DEVICE_PARALLEL_PORT       0x00000016
#define FILE_DEVICE_PHYSICAL_NETCARD    0x00000017
#define FILE_DEVICE_PRINTER             0x00000018
#define FILE_DEVICE_SCANNER             0x00000019
#define FILE_DEVICE_SERIAL_MOUSE_PORT   0x0000001a
#define FILE_DEVICE_SERIAL_PORT         0x0000001b
#define FILE_DEVICE_SCREEN              0x0000001c
#define FILE_DEVICE_SOUND               0x0000001d
#define FILE_DEVICE_STREAMS             0x0000001e
#define FILE_DEVICE_TAPE                0x0000001f
#define FILE_DEVICE_TAPE_FILE_SYSTEM    0x00000020
#define FILE_DEVICE_TRANSPORT           0x00000021
#define FILE_DEVICE_UNKNOWN             0x00000022
#define FILE_DEVICE_VIDEO               0x00000023
#define FILE_DEVICE_VIRTUAL_DISK        0x00000024
#define FILE_DEVICE_WAVE_IN             0x00000025
#define FILE_DEVICE_WAVE_OUT            0x00000026
#define FILE_DEVICE_8042_PORT           0x00000027
#define FILE_DEVICE_NETWORK_REDIRECTOR  0x00000028
#define FILE_DEVICE_BATTERY             0x00000029
#define FILE_DEVICE_BUS_EXTENDER        0x0000002a
#define FILE_DEVICE_MODEM               0x0000002b
#define FILE_DEVICE_VDM                 0x0000002c
#define FILE_DEVICE_MASS_STORAGE        0x0000002d
#define FILE_DEVICE_SMB                 0x0000002e
#define FILE_DEVICE_KS                  0x0000002f
#define FILE_DEVICE_CHANGER             0x00000030
#define FILE_DEVICE_SMARTCARD           0x00000031
#define FILE_DEVICE_ACPI                0x00000032
#define FILE_DEVICE_DVD                 0x00000033
#define FILE_DEVICE_FULLSCREEN_VIDEO    0x00000034
#define FILE_DEVICE_DFS_FILE_SYSTEM     0x00000035
#define FILE_DEVICE_DFS_VOLUME          0x00000036
#define FILE_DEVICE_SERENUM             0x00000037
#define FILE_DEVICE_TERMSRV             0x00000038
#define FILE_DEVICE_KSEC                0x00000039
#define FILE_DEVICE_FIPS                0x0000003A
#define FILE_DEVICE_INFINIBAND          0x0000003B
#define FILE_DEVICE_VMBUS               0x0000003E
#define FILE_DEVICE_CRYPT_PROVIDER      0x0000003F
#define FILE_DEVICE_WPD                 0x00000040
#define FILE_DEVICE_BLUETOOTH           0x00000041
#define FILE_DEVICE_MT_COMPOSITE        0x00000042
#define FILE_DEVICE_MT_TRANSPORT        0x00000043
#define FILE_DEVICE_BIOMETRIC           0x00000044
#define FILE_DEVICE_PMI                 0x00000045
#define FILE_DEVICE_EHSTOR              0x00000046
#define FILE_DEVICE_DEVAPI              0x00000047
#define FILE_DEVICE_GPIO                0x00000048
#define FILE_DEVICE_USBEX               0x00000049
#define FILE_DEVICE_CONSOLE             0x00000050
#define FILE_DEVICE_NFP                 0x00000051
#define FILE_DEVICE_SYSENV              0x00000052
#define FILE_DEVICE_VIRTUAL_BLOCK       0x00000053
#define FILE_DEVICE_POINT_OF_SERVICE    0x00000054
#define FILE_DEVICE_STORAGE_REPLICATION 0x00000055
#define FILE_DEVICE_TRUST_ENV           0x00000056
*/

static CHAR *DeviceType[100] =
{
	"BEEP"
	,"CD_ROM"
	,"CD_ROM_FILE_SYSTEM"
	,"CONTROLLER"
	,"DATALINK"
	,"DFS"
	,"DISK"
	,"DISK_FILE_SYSTEM"
	,"FILE_SYSTEM"
	,"INPORT_PORT"
	,"KEYBOARD"
	,"MAILSLOT"
	,"MIDI_IN"
	,"MIDI_OUT"
	,"MOUSE"
	,"MULTI_UNC_PROVIDER"
	,"NAMED_PIPE"
	,"NETWORK"
	,"NETWORK_BROWSER"
	,"NETWORK_FILE_SYSTEM"
	,"NULL"
	,"PARALLEL_PORT"
	,"PHYSICAL_NETCARD"
	,"PRINTER"
	,"SCANNER"
	,"SERIAL_MOUSE_PORT"
	,"SERIAL_PORT"
	,"SCREEN"
	,"SOUND"
	,"STREAMS"
	,"TAPE"
	,"TAPE_FILE_SYSTEM"
	,"TRANSPORT"
	,"UNKNOWN"
	,"VIDEO"
	,"VIRTUAL_DISK"
	,"WAVE_IN"
	,"WAVE_OUT"
	,"_PORT"
	,"NETWORK_REDIRECTOR"
	,"BATTERY"
	,"BUS_EXTENDER"
	,"MODEM"
	,"VDM"
	,"MASS_STORAGE"
	,"SMB"
	,"KS"
	,"CHANGER"
	,"SMARTCARD"
	,"ACPI"
	,"DVD"
	,"FULLSCREEN_VIDEO"
	,"DFS_FILE_SYSTEM"
	,"DFS_VOLUME"
	,"SERENUM"
	,"TERMSRV"
	,"KSEC"
	,"FIPS"
	,"INFINIBAND"
	,"VMBUS"
	,"CRYPT_PROVIDER"
	,"WPD"
	,"BLUETOOTH"
	,"MT_COMPOSITE"
	,"MT_TRANSPORT"
	,"BIOMETRIC"
	,"PMI"
	,"EHSTOR"
	,"DEVAPI"
	,"GPIO"
	,"USBEX"
	,"CONSOLE"
	,"NFP"
	,"SYSENV"
	,"VIRTUAL_BLOCK"
	,"POINT_OF_SERVICE"
	,"STORAGE_REPLICATION"
	,"TRUST_ENV"
};

typedef struct _FILTER_DRIVER_INFO
{
	ULONG	Count;
	ULONG	DeviceType;
	ULONG64	AttachedDevice;
	WCHAR	SysName[MAX_PATH];
	WCHAR	HostSysName[MAX_PATH];
	WCHAR	HostSysPath[MAX_PATH];
}FILTER_DRIVER_INFO, *PFILTER_DRIVER_INFO;

// CDlgFilterDriver 对话框

IMPLEMENT_DYNAMIC(CDlgFilterDriver, CPropertyPage)

CDlgFilterDriver::CDlgFilterDriver()
	: CPropertyPage(IDD_DLG_FILTER_DRIVER)
{

}

CDlgFilterDriver::~CDlgFilterDriver()
{
}

void CDlgFilterDriver::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILTER_DRIVER, m_list_filterDrv);
}


BEGIN_MESSAGE_MAP(CDlgFilterDriver, CPropertyPage)
END_MESSAGE_MAP()


// CDlgFilterDriver 消息处理程序


BOOL CDlgFilterDriver::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	m_list_filterDrv.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES);
	m_list_filterDrv.InsertColumn(0, L"类型", 0, 70);
	m_list_filterDrv.InsertColumn(1, L"驱动对象名", 0, 150);
	m_list_filterDrv.InsertColumn(2, L"设备对象", 0, 130);
	m_list_filterDrv.InsertColumn(3, L"宿主驱动对象名", 0, 150);
	m_list_filterDrv.InsertColumn(4, L"宿主路径", 0, 250);
	m_list_filterDrv.InsertColumn(5, L"文件厂商", LVCFMT_CENTER, 130);

	EnumFilterDriver();

	return TRUE;
}


void CDlgFilterDriver::EnumFilterDriver()
{
	CString str;
	CString szTrim = L"\\??\\";
	CString csSysroot;
	WCHAR	szSysRootBuff[MAX_PATH] = { 0 };
	WCHAR	szFileInfo[_MAX_PATH] = { 0 };
	PFILTER_DRIVER_INFO pFltInfo = NULL;

	GetWindowsDirectoryW(szSysRootBuff, MAX_PATH);
	csSysroot.Format(L"%s", szSysRootBuff);

	pFltInfo = (PFILTER_DRIVER_INFO)malloc(sizeof(FILTER_DRIVER_INFO) * 500);
	if (pFltInfo == NULL)
		return;

	if (!ARKDeviceIoControl(IOCTL_ENUMFILTERDRIVER, NULL, 0, pFltInfo, sizeof(FILTER_DRIVER_INFO) * 500))
	{
		free(pFltInfo);
		return;
	}
	
	ULONG Count =  pFltInfo[0].Count;
	ULONG item = 0;

	for (size_t i = 0; i < Count; i++)
	{
		item = m_list_filterDrv.GetItemCount();
		str.Format(L"%S", DeviceType[pFltInfo[i].DeviceType]);

		m_list_filterDrv.InsertItem(item, str);

		str.Format(L"%s", pFltInfo[i].SysName);
		m_list_filterDrv.SetItemText(item, 1, str);

		str.Format(L"0x%p", pFltInfo[i].AttachedDevice);
		m_list_filterDrv.SetItemText(item, 2, str);

		str.Format(L"%s", pFltInfo[i].HostSysName);
		m_list_filterDrv.SetItemText(item, 3, str);

		if (*pFltInfo[i].HostSysPath != 0)
		{
			str.Format(L"%s", pFltInfo[i].HostSysPath);
			str = str.TrimLeft(szTrim);
			str.Replace(L"SystemRoot", csSysroot);
			m_list_filterDrv.SetItemText(item, 4, str);

			RtlZeroMemory(szFileInfo, MAX_PATH * 2);

			GetFileInfo(str.GetBuffer(), szFileInfo);

			str.Format(L"%s", szFileInfo);

			m_list_filterDrv.SetItemText(item, 5, str);
		}
	}

	free(pFltInfo);
	pFltInfo = NULL;

	str.Format(L"[过滤驱动]当前系统共有 %d 个过滤驱动", Count);

	AfxGetMainWnd()->SetWindowTextW(str);
}
