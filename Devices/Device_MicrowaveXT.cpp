#include "stdafx.h"
#include "..\DemoDlg.h"

void CDemoDlg::OnPrefMWXTMidiDevices()
{
	DisablePorts();
	EnablePorts(MWXT_ID);
	OnPrefMididevices(MWXT_ID);
}

void CDemoDlg::OnDoMWXTPgmDump()
{
	EnablePorts(MWXT_ID);
	DisableButtons();
	m_Abort = false;
	ResetEvent(ghWriteEvent);
	m_ProgressBar.SetStep(1);
	q_Thread = ::AfxBeginThread((AFX_THREADPROC)DoMWXTPgmDump, this);
}

DWORD CDemoDlg::DoMWXTPgmDump(LPVOID Parameter)
{
	CDemoDlg *	pThis = reinterpret_cast<CDemoDlg *>(Parameter);
	FILE *Device_Xml_File;
	TCHAR NPath[MAX_PATH];
	char comparestring[512];
	unsigned char presetnmbr=0;
	
	sprintf_s(NPath,"%s\\Device Panels", pThis->MyPath);
	::CreateDirectory(NPath,NULL);
	SetCurrentDirectory(NPath);

	int MaxPreset=100;

	int device_id=pThis->m_Device_Id_Channel;
	char tMsg[9];

	tMsg[0]=0xF0;
	tMsg[1]=0x3e; // Waldorf ID
	tMsg[2]=0x0E; // Equipment ID for Microwave 2
	tMsg[3]=device_id; // Device ID, here broadcast
	tMsg[4]=0x00;	// Message ID: SNDR
	tMsg[5]=0x10;
	tMsg[6]=0x00;
	tMsg[7]=(tMsg[5]+tMsg[6])&0x7F;
	tMsg[8]=0xF7;
	
	// transmit sysex
	midi::CLongMsg LongMsg(tMsg,sizeof(tMsg));
	LongMsg.SendMsg(pThis->m_OutDevice);
	WaitForSingleObject(pThis->ghWriteEvent, INFINITE);
	if (pThis->m_Abort == true )
			goto abort;

	ResetEvent(pThis->ghWriteEvent);
	pThis->m_ProgressBar.StepIt();

	// Now create the device xml:
	OPEN_TEMPLATE(pThis->m_hInstance, IDR_XML_MWXT)
	fopen_s(&Device_Xml_File, "Microwave XT.xml", "wb");

	sprintf_s(comparestring,"<string name=\"Name\" value=\"A Preset 0");

	char unsigned bank = 0;
	char banklabel[2]="A";
	string line;							
	while(getline(Panel_Template, line))
	{
		if ( strstr(line.c_str(),
								comparestring) == NULL )
		{
			fprintf_s(Device_Xml_File,"%s",line.c_str());
			if ( strstr(line.c_str(),
									"</MidiDevices>") != NULL )
			{
									break;
			}
		} else {
			fprintf_s(Device_Xml_File,
								"                     <string name=\"Name\" value=\"%s\" wide=\"true\"/>\r\n",
								pThis->m_microwave_presets[bank][presetnmbr]);
			
			presetnmbr++;
			if ( presetnmbr > 127 )
			{
				bank = 1;
				presetnmbr = 0;
				*banklabel='B';
			}
			sprintf_s(comparestring,
					"                     <string name=\"Name\" value=\"%s Preset %d",
					banklabel,
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

void CDemoDlg::MWXTSysex(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	midi::CLongMsg	LongMsg(Msg,BytesRecorded);
	char presetname[17];
	int OFFSET=247;
	int presetnmbr=1;
	char unsigned bank=0;
	char banklabel[2]="A";
	DWORD pos=OFFSET;
	int i;

	while ( pos < BytesRecorded )
	{
		for( i=0; i < 16 ; i++)
		{
			presetname[i]=Msg[pos+i];
		}
		presetname[i]=0x0;
		sprintf_s(this->m_microwave_presets[bank][presetnmbr-1],
							"%s%03d  %s",
							banklabel,
							presetnmbr,
							presetname);
		presetnmbr++;
		if ( presetnmbr > 128 )
		{
			bank=1;
			presetnmbr=1;
			*banklabel='B';
		}
		pos=pos+256;
	}

	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	SetEvent(ghWriteEvent);
}

