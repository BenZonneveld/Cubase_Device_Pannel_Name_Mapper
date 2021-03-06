#if !defined(AFX_MIDIDEVSDLG_H__1BDCC560_1300_11D7_865D_0030BD08B6D9__INCLUDED_)
#define AFX_MIDIDEVSDLG_H__1BDCC560_1300_11D7_865D_0030BD08B6D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MIDIDevsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMIDIDevsDlg dialog

class CMIDIDevsDlg : public CDialog
{
// Construction
public:
	CMIDIDevsDlg(CWnd* pParent = NULL);   // standard constructor

    //
    // Accessors
    //
		
    UINT GetOutDevId() const { return m_OutDevId; }
    UINT GetInDevId() const { return m_InDevId; }
		UINT GetDeviceChannelId() const { return m_DeviceChannelId; }

    //
    // Mutators
    //

    void SetOutDevId(UINT OutDevId) { m_OutDevId = OutDevId; }
    void SetInDevId(UINT InDevId) { m_InDevId = InDevId; }
		void SetDeviceId(UINT DeviceId) { m_dev_id = DeviceId; }
		void SetMaxDeviceChannel(UINT MaxDevChannel) { m_MaxDeviceChannel = MaxDevChannel; }
		void SetOffSet(bool Offset) { m_OffSet = Offset; }

    // Has the client selected a new output device?
    bool IsOutDevChanged() const { return m_OutChanged; }

    // Has the client selected a new input device?
    bool IsInDevChanged() const { return m_InChanged; }

		// Has the client selected a new device id / channel?
		bool IsDeviceChannelIdChanged() const { return m_DeviceChannelIdChanged; }

// Dialog Data
	//{{AFX_DATA(CMIDIDevsDlg)
	enum { IDD = IDD_MIDI_DEVS_DLG };
	CComboBox	m_OutDevsCombo;
	CComboBox	m_InDevsCombo;
	CComboBox	m_DeviceChannelIdCombo;
	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMIDIDevsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    UINT m_OutDevId;
    UINT m_InDevId;
		UINT m_DeviceChannelId;
		UINT m_MaxDeviceChannel;
		int m_dev_id;

    bool m_OutChanged;
    bool m_InChanged;
		bool m_DeviceChannelIdChanged;
		bool m_OffSet;

	// Generated message map functions
	//{{AFX_MSG(CMIDIDevsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIDIDEVSDLG_H__1BDCC560_1300_11D7_865D_0030BD08B6D9__INCLUDED_)
