// DemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PgmListGenerator.h"
#include "DemoDlg.h"
#include "ShortMsg.h"
#include "LongMsg.h"
#include "midi.h"
#include "MIDIDevsDlg.h"

// XML Templates:
#include "Devices/xml_Proteus.h"
#include "Devices/xml_DX7.h"
#include "Devices/xml_Wavestation.h"
#include "Devices/xml_MWXT.h"
#include "Devices/xml_Spx990.h"
#include "Devices/xml_DpPro.h"
#include "Devices/xml_StudioQuad.h"

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
// CDemoDlg dialog


CDemoDlg::CDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDemoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


CDemoDlg::~CDemoDlg()
{
  if(m_InDevice.IsOpen())
  {
      if(m_InDevice.IsRecording())
      {
          m_InDevice.StopRecording();
      }   

      m_InDevice.Close();
  }

  if(m_OutDevice.IsOpen())
  {
      m_OutDevice.Close();
  }
}

// Received MIDI long message
void CDemoDlg::ReceiveMsg(LPSTR Msg, DWORD BytesRecorded,DWORD TimeStamp)
{
	midi::CLongMsg	LongMsg(Msg,BytesRecorded);

	switch (Msg[1])
	{
		case 0x00:
			switch (Msg[2])
			{
				case 0x00: // North American Group
					switch(Msg[3])
					{
						case 0x10: // D.O.D. Electronics Group
							DigitechQuadSysex( Msg, BytesRecorded, TimeStamp);
							break;
						default:
							break;
					}
				default:
					break;
			}
			break;
		case 0x0F:	// Ensoniq Corp.
			EnsoniqDPProSysex( Msg, BytesRecorded, TimeStamp);
			break;
		case 0x12:  // GMR ID
			YamahaDX7ESysex( Msg, BytesRecorded, TimeStamp);
			break;
		case 0x18:	// EMU
			ProteusSysex( Msg, BytesRecorded, TimeStamp);
			break;
		case 0x3e:	// Waldorf Midi
			MWXTSysex( Msg, BytesRecorded, TimeStamp);
			break;
		case 0x42:	// Korg ID
			WavestationSysex( Msg, BytesRecorded, TimeStamp);
			break;
		case 0x43:	// Yamaha Corporation
			switch(Msg[3])
			{
			case 0x7E:	// SPX 990 Format No.	
				YamahaSPXSysex( Msg, BytesRecorded, TimeStamp);
				break;
			case 0x09:  // DX7 Bulk Data for 32 Voices
				YamahaDX7Sysex( Msg, BytesRecorded, TimeStamp);
				break;
			default:
				break;
			}
			break;
		default:
			break;
	}
}


// Receives an erroneous short message
void CDemoDlg::OnError(DWORD Msg, DWORD TimeStamp)
{
    MessageBox("An illegal MIDI short message was received.", "Error",
        MB_ICONSTOP | MB_OK);
}


void CDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDemoDlg)
		DDX_Control(pDX, IDC_QUAD_USER_PGMS, m_QUserPgms);
		DDX_Control(pDX, IDC_Q_MIDI, m_QMidi);
		DDX_Control(pDX, IDC_SPX_GET_PGM, m_SpxPgms);
		DDX_Control(pDX, IDC_SPX_MIDI, m_SpxMidi);
		DDX_Control(pDX, IDC_DPPRO_GET_PGM, m_DPProPgms);
		DDX_Control(pDX, IDC_DPPRO_MIDI, m_DPProMidi);
		DDX_Control(pDX, IDC_MWXT_GET_PGM, m_MWXTPgms);
		DDX_Control(pDX, IDC_MWXT_MIDI, m_MWXTMidi);
		DDX_Control(pDX, IDC_DX7_MIDI, m_DX7Midi);
		DDX_Control(pDX, IDC_DX7_GET_PGM, m_DX7Pgms);
		DDX_Control(pDX, IDC_WAVESTATION_MIDI, m_WavestationMidi);
		DDX_Control(pDX, IDC_WAVESTATION_GET_PGM, m_WavestationPgms);
		DDX_Control(pDX, IDC_PROTEUS_MIDI, m_ProteusMidi);
		DDX_Control(pDX, IDC_PROTEUS_GET_PGM, m_ProteusPgms);
						
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDemoDlg, CDialog)
	//{{AFX_MSG_MAP(CDemoDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(IDC_Q_MIDI, OnPrefQuadMididevices)
	ON_COMMAND(IDC_QUAD_USER_PGMS, OnDoQuadUserPgmDump)
	ON_COMMAND(IDC_SPX_MIDI, OnPrefSpxMididevices)
	ON_COMMAND(IDC_SPX_GET_PGM, OnDoSpxPgmDump)
	ON_COMMAND(IDC_DPPRO_MIDI, OnPrefDPProMidiDevices)
	ON_COMMAND(IDC_DPPRO_GET_PGM, OnDoDPProPgmDump)
	ON_COMMAND(IDC_MWXT_MIDI, OnPrefMWXTMidiDevices)
	ON_COMMAND(IDC_MWXT_GET_PGM, OnDoMWXTPgmDump)
	ON_COMMAND(IDC_PROTEUS_GET_PGM, OnDoProteusPgmDump)
	ON_COMMAND(IDC_PROTEUS_MIDI, OnPrefProteusMidiDevices)
	ON_COMMAND(IDC_DX7_GET_PGM, OnDoDX7PgmDump)
	ON_COMMAND(IDC_DX7_MIDI, OnPrefDX7MidiDevices)
	ON_COMMAND(IDC_WAVESTATION_GET_PGM, OnDoWavestationPgmDump)
	ON_COMMAND(IDC_WAVESTATION_MIDI, OnPrefWavestationMidiDevices)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDemoDlg message handlers

BOOL CDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
//	THREADPARAMS *Params = new THREADPARAMS;
	ghWriteEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	GetCurrentDirectory(MAX_PATH, MyPath);

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
	
	m_QUserPgms.UpdateWindow();
	m_QMidi.UpdateWindow();
	m_SpxPgms.UpdateWindow();
	m_SpxMidi.UpdateWindow();
	m_DPProPgms.UpdateWindow();
	m_DPProMidi.UpdateWindow();
	m_MWXTPgms.UpdateWindow();
	m_MWXTMidi.UpdateWindow();
	m_ProteusPgms.UpdateWindow();
	m_ProteusMidi.UpdateWindow();
	m_DX7Pgms.UpdateWindow();
	m_DX7Midi.UpdateWindow();
	m_WavestationPgms.UpdateWindow();
	m_WavestationMidi.UpdateWindow();

	// For reading the xml templates:
	m_hInstance = theApp.m_hInstance;

	// Fill the xml templates
	memcpy(m_proteus_template,		proteus_template,				sizeof(proteus_template));
	memcpy(m_dx7_template,				dx7_template,						sizeof(dx7_template));
	memcpy(m_wavestation_template, wavestation_template,	sizeof(wavestation_template));
	memcpy(m_microwave_template,	microwave_template,			sizeof(microwave_template));
	memcpy(m_spx_template,				spx_template,						sizeof(spx_template));
	memcpy(m_dppro_template,			dppro_template,					sizeof(dppro_template));
	memcpy(m_studioquad_template,	studioquad_template,		sizeof(studioquad_template));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDemoDlg::OnPaint() 
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
HCURSOR CDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CDemoDlg::OnPrefMididevices(int DevId) 
{
	MIDIOUTCAPS	moutCaps;
	MIDIINCAPS	minCaps;
	char portname[15];

		try
    {
	    CMIDIDevsDlg Dlg;

        //
        // Initialize MIDI device dialog box
        //
				sprintf_s(portname,"OutPort_Dev_%d", DevId);
        if(m_OutDevice.IsOpen())
        {
            Dlg.SetOutDevId(m_OutDevice.GetDevID());
        }

        if(m_InDevice.IsOpen())
        {
            Dlg.SetInDevId(m_InDevice.GetDevID());
        }

        // Run dialog box
        if(Dlg.DoModal() == IDOK)
        {
            // If the client clicked OK and they chose to change the 
            // MIDI output device
            if(Dlg.IsOutDevChanged())
            {
                m_OutDevice.Close();
								midi::CMIDIOutDevice::GetDevCaps(Dlg.GetOutDevId() , moutCaps);
								theApp.WriteProfileStringA("Settings", (LPCTSTR)portname, (LPCTSTR)moutCaps.szPname);
								m_OutDevice.Open(m_OutDevice.GetIDFromName(theApp.GetProfileStringA("Settings",(LPCTSTR)portname,"not connected")));
            }

            // If the client clicked OK and they chose to change the 
            // MIDI input device
            if(Dlg.IsInDevChanged())
            {
							sprintf_s(portname,"InPort_Dev_%d", DevId);

                if(m_InDevice.IsOpen())
                {
                    if(m_InDevice.IsRecording())
                    {
	                    // Stop receiving MIDI events 	
                        m_InDevice.StopRecording();   	
                    }
                
                    m_InDevice.Close(); 	
                }

								midi::CMIDIInDevice::GetDevCaps(Dlg.GetInDevId() , minCaps);
								theApp.WriteProfileStringA("Settings", (LPCTSTR)portname, (LPCTSTR)minCaps.szPname);
								m_InDevice.Open(m_InDevice.GetIDFromName(theApp.GetProfileStringA("Settings",(LPCTSTR)portname,"not connected")));
								m_InDevice.AddSysExBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
                // Start receiving MIDI events 	
                m_InDevice.StartRecording(); 
            }
        }	
    }
    catch(const std::exception &ex)
    {
        MessageBox(ex.what(), "Error", MB_ICONSTOP | MB_OK);
    }
}


// Exit the program
void CDemoDlg::OnFileExit() 
{
	SendMessage(WM_CLOSE);
}

void CDemoDlg::OnHelpAbout() 
{
	CAboutDlg Dlg;

    Dlg.DoModal();	
}

void CDemoDlg::EnablePorts(int DevId)
{
	char portname[15];

	sprintf_s(portname,"InPort_Dev_%d", DevId);
  m_InDevice.SetReceiver(*this);
  m_InDevice.Open(
		m_InDevice.GetIDFromName(
			theApp.GetProfileStringA("Settings",
																(LPCTSTR)portname,
																"not connected"
															)
			)
	);
	
	m_InDevice.AddSysExBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));

	// Start receiving MIDI events
  m_InDevice.StartRecording();

	sprintf_s(portname,"OutPort_Dev_%d", DevId);
  m_OutDevice.Open(m_OutDevice.GetIDFromName(
		theApp.GetProfileStringA(
				"Settings",(LPCTSTR)portname,"not connected")));
}

void CDemoDlg::DisablePorts()
{
	m_OutDevice.Close();
	if(m_InDevice.IsRecording())
  {
		// Stop receiving MIDI events 	
    m_InDevice.StopRecording();   	
  }

	m_InDevice.Close();
}

void CDemoDlg::DisableButtons()
{
	DISABLE_BUTTON(m_QUserPgms)
	DISABLE_BUTTON(m_QMidi)
	DISABLE_BUTTON(m_SpxPgms)
	DISABLE_BUTTON(m_SpxMidi)
	DISABLE_BUTTON(m_DPProPgms)
	DISABLE_BUTTON(m_DPProMidi)
	DISABLE_BUTTON(m_MWXTPgms)
	DISABLE_BUTTON(m_MWXTMidi)
	DISABLE_BUTTON(m_ProteusPgms)
	DISABLE_BUTTON(m_ProteusMidi)
	DISABLE_BUTTON(m_DX7Pgms)
	DISABLE_BUTTON(m_DX7Midi)
	DISABLE_BUTTON(m_WavestationPgms)
	DISABLE_BUTTON(m_WavestationMidi)
}

