#include "stdafx.h"
#include "..\DemoDlg.h"
#include "Device_DPPro_Factory.h"

void CDemoDlg::OnPrefDPProMidiDevices()
{
	DisablePorts();
	EnablePorts(DPPRO_ID);
	OnPrefMididevices(DPPRO_ID);
}

void CDemoDlg::OnDoDPProPgmDump()
{
	EnablePorts(DPPRO_ID);
	DisableButtons();
	m_Abort = false;
	ResetEvent(ghWriteEvent);
	m_ProgressBar.SetStep(1);
	q_Thread = ::AfxBeginThread((AFX_THREADPROC)DoDPProPgmDump, this);

	MessageBox("On the DP Pro goto 'System' -> page 15\nSelect type 'All RAM Presets'\nPress Enter on the DP Pro.", "The DP Pro requires manual action.", 
                 MB_OK);
}

DWORD CDemoDlg::DoDPProPgmDump(LPVOID Parameter)
{
	CDemoDlg *pThis = reinterpret_cast<CDemoDlg *>(Parameter);
	FILE *Device_Xml_File;
	MIDIOUTCAPS OutCaps;
	TCHAR NPath[MAX_PATH];
	char comparestring[512];
	unsigned char presetnmbr=0;

	sprintf_s(NPath,"%s\\Device Panels", pThis->MyPath);
	::CreateDirectory(NPath,NULL);
	SetCurrentDirectory(NPath);

	// transmit sysex
	WaitForSingleObject(pThis->ghWriteEvent, INFINITE);
	if (pThis->m_Abort == true )
			goto abort;

	ResetEvent(pThis->ghWriteEvent);
	pThis->m_ProgressBar.StepIt();

	// Try closing the messagebox
  CWnd* pWnd = FindWindow(NULL, "The DP Pro requires manual action.");
   if(pWnd != NULL)
   {
     // Send close command to the message box window
     ::PostMessage(pWnd->m_hWnd, WM_CLOSE, 0, 0);
   }

		// Now create the device xml:
	OPEN_TEMPLATE(pThis->m_hInstance, IDR_XML_DPPRO)
	fopen_s(&Device_Xml_File, "DP Pro.xml", "wb");

	sprintf_s(comparestring,"<string name=\"Name\" value=\"User1 Preset 0");

	unsigned int bank = 0;
	string line;
	while(getline(Panel_Template, line))
	{
		if ( strstr(line.c_str(),
								comparestring) == NULL )
		{
			if (strstr(line.c_str(), "Port Name" ) == NULL )
			{
				fprintf_s(Device_Xml_File,"%s",line.c_str());
			} else {
				pThis->m_OutDevice.GetDevCaps(pThis->m_OutDevice.GetDevID(),OutCaps);
				fprintf_s(Device_Xml_File,
									"               <string name=\"Port Name\" value=\"%s\"/>\r\n",
									OutCaps.szPname);
			}
			if ( strstr(line.c_str(),
									"</MidiDevices>") != NULL )
			{
									break;
			}
		} else {
			fprintf_s(Device_Xml_File,
								"                     <string name=\"Name\" value=\"%s\" wide=\"true\"/>\r\n",
								pThis->m_dppro_presets[bank][presetnmbr]);
			
			presetnmbr++;
			if ( presetnmbr > 127 )
			{
				bank++;
				presetnmbr = 0;
			}
			sprintf_s(comparestring,
					"                     <string name=\"Name\" value=\"%s Preset %d",
					dpbankname[bank],
					presetnmbr);
		}
	}
	fclose(Device_Xml_File);
	CLOSE_TEMPLATE
abort:
	EnableButtons();
	pThis->m_ProgressBar.SetPos(0);
	SetCurrentDirectory(pThis->MyPath);
	return 0;
}

void CDemoDlg::EnsoniqDPProSysex(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	midi::CLongMsg	LongMsg(Msg,BytesRecorded);
	int PGMOFFSET=680;
	char presetname[22];
	int presetnmbr=0;
	int count=0;
	unsigned int pos=0;
	int bank=0;
	if ( Msg[5] == 0x21 )
	{
		pos = 8;
		bank=Msg[6];
	} else {
		pos = 7;
		bank=0;
	}

	while ( pos < (BytesRecorded - PGMOFFSET ) )
	{
		for (count=0; count <= 17 ; count++)
		{
			presetname[count]=(Msg[(2*count)+pos]<<4) + Msg[(2*count)+pos+1];
		}
		presetname[count]=0x0;
		sprintf_s(this->m_dppro_presets[bank][presetnmbr],
			"%d%03d  %s",
			bank,
			presetnmbr,
			presetname);

		++presetnmbr;
		if ( presetnmbr > 127 )
		{
			++bank;
			presetnmbr=0;
		}

		pos=pos+PGMOFFSET;
	}	
	// Print the factory Presetnames
	for ( count = 0 ; count < 128 ; count++ )
	{
		sprintf_s(this->m_dppro_presets[2][count],
			"%s", 
			dp_presetnames[count]);

	}

	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	SetEvent(ghWriteEvent);
}

