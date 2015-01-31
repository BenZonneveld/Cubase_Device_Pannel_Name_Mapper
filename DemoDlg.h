// DemoDlg.h : header file
//

#if !defined(AFX_DEMODLG_H__51B03386_1002_11D7_865D_0030BD08B6D9__INCLUDED_)
#define AFX_DEMODLG_H__51B03386_1002_11D7_865D_0030BD08B6D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MIDIOutDevice.h"
#include "MIDIInDevice.h"
#include "Resource.h"
#include "LongMsg.h"
#include "ShortMsg.h"
#include "midi.h"

#define MAXBAR					200
#define DIGITECH_ID			0
#define SPX_ID					1
#define DPPRO_ID				2
#define MWXT_ID					3
#define DX7_ID					4
#define WAVESTATION_ID	5
#define PROTEUS_ID			6

/////////////////////////////////////////////////////////////////////////////
// CDemoDlg dialog


class CDemoDlg : public CDialog,
                 public midi::CMIDIReceiver
{
// Construction
public:
	CDemoDlg(CWnd* pParent = NULL);	// standard constructor
    ~CDemoDlg();

    // Receives short messages
		void ReceiveMsg(DWORD Msg, DWORD TimeStamp) {};

    // Receives long messages
    void ReceiveMsg(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp);

    // Called when an invalid short message is received
    void OnError(DWORD Msg, DWORD TimeStamp);

    // Called when an invalid long message is received
    void OnError(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp) {}

// Dialog Data
	//{{AFX_DATA(CDemoDlg)
	enum { IDD = IDD_MIDIDEVDEMOV2_DIALOG };
    CButton				m_QUserPgms;
		CButton				m_QMidi;
		CButton				m_SpxPgms;
		CButton				m_SpxMidi;
		CButton				m_DPProPgms;
		CButton				m_DPProMidi;
		CButton				m_MWXTPgms;
		CButton				m_MWXTMidi;
		CButton				m_ProteusPgms;
		CButton				m_ProteusMidi;
		CButton				m_DX7Pgms;
		CButton				m_DX7Midi;
		CButton				m_WavestationPgms;
		CButton				m_WavestationMidi;
		CProgressCtrl	m_ProgressBar;

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDemoDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
    midi::CMIDIOutDevice m_OutDevice;
    midi::CMIDIInDevice m_InDevice;

	// Generated message map functions
	//{{AFX_MSG(CDemoDlg)
	virtual BOOL OnInitDialog();
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	// Midi Preferences for the devices
	afx_msg void OnPrefQuadMididevices();
	afx_msg void OnPrefSpxMididevices();
	afx_msg void OnPrefDPProMidiDevices();
	afx_msg void OnPrefMWXTMidiDevices();
	afx_msg void OnPrefDX7MidiDevices();
	afx_msg void OnPrefWavestationMidiDevices();
	afx_msg void OnPrefProteusMidiDevices();
	// The actual transmit Stuff
	afx_msg void OnDoQuadUserPgmDump();
	afx_msg void OnDoSpxPgmDump();
	afx_msg void OnDoDPProPgmDump();
	afx_msg void OnDoMWXTPgmDump();
	afx_msg void OnDoDX7PgmDump();
	afx_msg void OnDoWavestationPgmDump();
	afx_msg void OnDoProteusPgmDump();

	afx_msg void OnFileExit();
	afx_msg void OnHelpAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	// Thread definitions
	static DWORD WINAPI DoQuadUserPgmDump(LPVOID Parameter);
	static DWORD WINAPI DoSpxUserPgmDump(LPVOID Parameter);
	static DWORD WINAPI DoDPProPgmDump(LPVOID Parameter);
	static DWORD WINAPI DoMWXTPgmDump(LPVOID Parameter);
	static DWORD WINAPI DoDX7PgmDump(LPVOID Parameter);
	static DWORD WINAPI	DoWavestationPgmDump(LPVOID Parameter);
	static DWORD WINAPI DoProteusPgmDump(LPVOID Parameter);

	// Sysex Receivers
	void	DigitechQuadSysex(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp);
	void	YamahaDX7Sysex(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp);
	void	YamahaSPXSysex(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp);
	void	EnsoniqDPProSysex(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp);
	void	MWXTSysex(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp);
	void	DX7Sysex(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp);
	void	WavestationSysex(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp);
	void	ProteusSysex(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp);

//	void	CreateDeviceXML(LPSTR filename, char *xml_template, char *presetnames);
	// Global Stuff
	void EnablePorts(int DevId);
	void DisablePorts();
	void DisableButtons(void);
	bool CheckDevicePorts(int DevId);
	void MidiCheck(void);

	void OnPrefMididevices(int DevId);

	unsigned char m_dx_bank;
	HANDLE ghWriteEvent;
	bool m_Abort;
	HACCEL m_hAccelTable;

	HINSTANCE m_hInstance;

	unsigned char	SysXBuffer[256*1024];
	// For Proteus
	char m_proteus_presets[128][18];
	char m_dx7_presets[10][32][15]; // 10 bank, 32 presets with a name of 11
	char m_wavestation_presets[4][50][25]; // 4 bank, 50 presets with a name of 16
	char m_microwave_presets[2][128][25]; // 2 bank, 128 presets with a name of 16
	char m_spx_presets[100][25];
	char m_dppro_presets[3][128][35]; // 3 banks, 128 presets
	char m_studioquad_presets[2][281][45]; // 2 banks, 100 user , 180 factory
	char m_studioquad_presetnmbr;
	
	TCHAR MyPath[MAX_PATH];
	CWinThread    *q_Thread;
};


		//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEMODLG_H__51B03386_1002_11D7_865D_0030BD08B6D9__INCLUDED_)
