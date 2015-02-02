#include "stdafx.h"
#include "..\DemoDlg.h"


// Request the data
void CDemoDlg::OnPrefSpxMididevices()
{
	DisablePorts();
	EnablePorts(SPX_ID);
	OnPrefMididevices(SPX_ID);
}

void CDemoDlg::OnDoSpxPgmDump()
{
	EnablePorts(SPX_ID);
	DisableButtons();
	m_Abort = false;
	ResetEvent(ghWriteEvent);
	m_ProgressBar.SetStep(MAXBAR/100);
	q_Thread = ::AfxBeginThread((AFX_THREADPROC)DoSpxUserPgmDump, this);
}

DWORD CDemoDlg::DoSpxUserPgmDump(LPVOID Parameter)
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

	int MaxPreset=100;

	int channel=pThis->m_Device_Id_Channel - 1;
	char tMsg[15];

	tMsg[0]=0xF0;
	tMsg[1]=0x43;
	tMsg[2]=0x20+channel;
	tMsg[3]=0x7E;
	tMsg[4]=0x4C;
	tMsg[5]=0x4D;
	tMsg[6]=0x20;
	tMsg[7]=0x20;
	tMsg[8]=0x38;
	tMsg[9]=0x41;
	tMsg[10]=0x32;
	tMsg[11]=0x31;
	tMsg[12]='M';
//	tMsg[13]=0x;
	tMsg[14]=0xF7;
	
	// transmit sysex
	for ( int pgm = 1 ; pgm <= MaxPreset ; pgm++)
	{
		// construct sysex
		tMsg[13]=pgm;
	
		// transmit sysex
		midi::CLongMsg LongMsg(tMsg,sizeof(tMsg));
		LongMsg.SendMsg(pThis->m_OutDevice);
		WaitForSingleObject(pThis->ghWriteEvent, INFINITE);
		if (pThis->m_Abort == true )
			goto abort;

		ResetEvent(pThis->ghWriteEvent);
		pThis->m_ProgressBar.StepIt();
	}

	// Now create the device xml:
	OPEN_TEMPLATE(pThis->m_hInstance, IDR_XML_SPX)

	fopen_s(&Device_Xml_File, "SPX990.xml", "wb");

	sprintf_s(comparestring,"<string name=\"Name\" value=\"Preset 0");

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
				pThis->m_spx_presets[presetnmbr]);
			
			presetnmbr++;
			if ( presetnmbr < 128 )
			{
				sprintf_s(comparestring,
					"                     <string name=\"Name\" value=\"Preset %d",
					presetnmbr);
			}
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

void CDemoDlg::YamahaSPXSysex(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	midi::CLongMsg	LongMsg(Msg,BytesRecorded);
	int NameOffset=16;
	char unsigned PgmName[17];
	int count=0;
	byte test=0;

	for(int byte=0; byte < (2*16) ; byte+=2)
	{
		PgmName[count]=(Msg[byte+NameOffset]<<4)+Msg[byte+NameOffset+1];
		if ( PgmName[count] > 0x7F )
		{
			PgmName[count]=' ';
		}
		count++;
	}	
	PgmName[count]=0x00;
	sprintf_s(this->m_spx_presets[Msg[15]-1],
			"%2d  %s", 
			Msg[15],
			PgmName);

	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	SetEvent(ghWriteEvent);
}