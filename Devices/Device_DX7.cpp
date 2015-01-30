#include "stdafx.h"
#include "..\DemoDlg.h"

void CDemoDlg::OnPrefDX7MidiDevices()
{
	DisablePorts();
	EnablePorts(DX7_ID);
	OnPrefMididevices(DX7_ID);
}

void CDemoDlg::OnDoDX7PgmDump()
{
	EnablePorts(DX7_ID);
	DisableButtons();

	q_Thread = ::AfxBeginThread((AFX_THREADPROC)DoDX7PgmDump, this);
}

DWORD CDemoDlg::DoDX7PgmDump(LPVOID Parameter)
{
	CDemoDlg *pThis = reinterpret_cast<CDemoDlg *>(Parameter);
	TCHAR NPath[MAX_PATH];	

	int channel=0;
	char comparestring[512];
	unsigned char presetnmbr=0;
	unsigned char bank;

	sprintf_s(NPath,"%s\\Device Panels", pThis->MyPath);
	::CreateDirectory(NPath,NULL);
	SetCurrentDirectory(NPath);

	// For Yamaha Mode
	CHAR tMsg[5];
	tMsg[0]=0xF0;
	tMsg[1]=0x43; // Yamaha ID
	tMsg[2]=0x20+channel; // Midi Channel
	tMsg[3]=0x09; // Request Bulk Dump
	tMsg[4]=0xF7;

#define CHANNEL	0
	for( bank = 0 ; bank < 10 ; bank++)
	{
		midi::CShortMsg BankMsg(midi::CONTROL_CHANGE, CHANNEL, 6, bank*13, 0);
		midi::CShortMsg PgmChangeMsg(midi::PROGRAM_CHANGE, CHANNEL, 0, 0,0);
		BankMsg.SendMsg(pThis->m_OutDevice);
		PgmChangeMsg.SendMsg(pThis->m_OutDevice);
		pThis->m_dx_bank=bank;
	
		// transmit sysex
		midi::CLongMsg LongMsg(tMsg,sizeof(tMsg));
		LongMsg.SendMsg(pThis->m_OutDevice);
		WaitForSingleObject(pThis->ghWriteEvent, INFINITE);
		ResetEvent(pThis->ghWriteEvent);
	}

	// Now create the device xml:
	OPEN_TEMPLATE(pThis->m_hInstance, IDR_XML_DX7)
	fopen_s(&pThis->Pgm_File, "DX7.xml", "w");

	sprintf_s(comparestring,"<string name=\"Name\" value=\"Bank0 Preset 0");

	char unsigned bank = 0;
	string line;							
	while(getline(Panel_Template, line))
	{
		if ( strstr(line.c_str(),
								comparestring) == NULL )
		{
			fprintf_s(pThis->Pgm_File,"%s",line.c_str());
		} else {
			fprintf_s(pThis->Pgm_File, "                     <string name=\"Name\" value=\"%s\" wide=\"true\"/>\r\n",pThis->m_dx7_presets[bank][presetnmbr]);
			
			presetnmbr++;
			if ( presetnmbr > 31 )
			{
				bank++;
				presetnmbr = 0;
			}
			sprintf_s(comparestring,
					"                     <string name=\"Name\" value=\"Bank%d Preset %d",
					bank,
					presetnmbr);
		}
	}
	fclose(pThis->Pgm_File);
	CLOSE_TEMPLATE

	EnableButtons();

	SetCurrentDirectory(pThis->MyPath);
	return 0;
}

void CDemoDlg::YamahaDX7Sysex(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	midi::CLongMsg	LongMsg(Msg,BytesRecorded);
	char presetname[11];
	// Byte 6 is 0x01 for bulk voice dump
	int OFFSET=6;
#define NAMEOFFSET	118
	int presetnmbr=0;
	int i;

	for (DWORD pos=OFFSET+NAMEOFFSET; pos < BytesRecorded ; pos=pos+128)
	{
		for (i=0; i < 10; i++)
		{
			presetname[i]=Msg[pos+i];
		}
		presetname[i]=0x0;
		presetnmbr++;
		sprintf_s(this->m_dx7_presets[m_dx_bank][presetnmbr-1],
			"%2d  %s", 
			presetnmbr,
			presetname);

	}
		
	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	SetEvent(ghWriteEvent);
}
