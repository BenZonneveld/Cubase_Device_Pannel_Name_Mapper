// MIDIDevsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Cubase Device Panel Mapper.h"
#include "MIDIDevsDlg.h"
#include "MIDIOutDevice.h"
#include "MIDIInDevice.h"
#include "DeviceIds.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMIDIDevsDlg dialog


CMIDIDevsDlg::CMIDIDevsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMIDIDevsDlg::IDD, pParent),
    m_OutDevId(0),
    m_InDevId(0),
    m_OutChanged(false),
    m_InChanged(false),
		m_DeviceChannelIdChanged(false),
		m_DeviceChannelId(0),
		m_MaxDeviceChannel(0)

{
	//{{AFX_DATA_INIT(CMIDIDevsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMIDIDevsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMIDIDevsDlg)
	DDX_Control(pDX, IDC_MIDI_OUT_DEVS, m_OutDevsCombo);
	DDX_Control(pDX, IDC_MIDI_IN_DEVS, m_InDevsCombo);
	DDX_Control(pDX, IDC_MIDI_CHANNEL_DEVICE_ID, m_DeviceChannelIdCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMIDIDevsDlg, CDialog)
	//{{AFX_MSG_MAP(CMIDIDevsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMIDIDevsDlg message handlers

BOOL CMIDIDevsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char message[4];
	char dev_id[20];
  UINT i;
    //
    // Initialize output device combo box with the names of all of the 
    // MIDI output devices available
    //
    if ( m_dev_id == DPPRO_ID )
		{
			m_OutDevsCombo.EnableWindow(FALSE);
			m_OutDevsCombo.UpdateWindow();
		} else {

			MIDIOUTCAPS OutCaps;
		  for(i = 0; i < midi::CMIDIOutDevice::GetNumDevs(); i++)
			{
			  midi::CMIDIOutDevice::GetDevCaps(i, OutCaps);
        m_OutDevsCombo.AddString(OutCaps.szPname);
			}

	    if(midi::CMIDIOutDevice::GetNumDevs() > 0)
		  {
        m_OutDevsCombo.SetCurSel(m_OutDevId);
			}
		}
    //
    // Initialize input device combo box with the names of all of the
    // MIDI input devices available on this system
    //

    MIDIINCAPS InCaps;
    for(i = 0; i < midi::CMIDIInDevice::GetNumDevs(); i++)
    {
        midi::CMIDIInDevice::GetDevCaps(i, InCaps);
        m_InDevsCombo.AddString(InCaps.szPname);
    }

    if(midi::CMIDIInDevice::GetNumDevs() > 0)
    {
        m_InDevsCombo.SetCurSel(m_InDevId);
    }
	
		for (i = m_OffSet ; i <= m_MaxDeviceChannel ; i++)
		{
			sprintf_s(message,	"%d",i);
			m_DeviceChannelIdCombo.AddString((LPCTSTR)message);
		}

		sprintf_s(dev_id,"Device_Channel_Id_%d",m_dev_id);
		m_DeviceChannelId = theApp.GetProfileIntA("Settings",(LPCTSTR)dev_id,-1);
		m_DeviceChannelIdCombo.SetCurSel(m_DeviceChannelId);


	return TRUE;  
}


// Client clicked OK
void CMIDIDevsDlg::OnOK() 
{
    //
    // Check to see if the client changed the selection for either the
    // MIDI input or output devices. If so, indicate that a new 
    // selection has been made
    //

		if ( m_OutDevsCombo.IsWindowEnabled() == TRUE )
		{
	    if(m_OutDevsCombo.GetCount() > 0)
		  {
			    if(m_OutDevId != m_OutDevsCombo.GetCurSel())
				  {
					  m_OutDevId = m_OutDevsCombo.GetCurSel();
            m_OutChanged = true;
					}
			}
		}

    if(m_InDevsCombo.GetCount() > 0)
    {
        if(m_InDevId != m_InDevsCombo.GetCurSel())
        {
            m_InDevId = m_InDevsCombo.GetCurSel();
            m_InChanged = true;
        }
    }

		if ( m_DeviceChannelId != m_DeviceChannelIdCombo.GetCurSel())
		{
			m_DeviceChannelId = m_DeviceChannelIdCombo.GetCurSel();
			m_DeviceChannelIdChanged = true;
		}
	
	CDialog::OnOK();
}

