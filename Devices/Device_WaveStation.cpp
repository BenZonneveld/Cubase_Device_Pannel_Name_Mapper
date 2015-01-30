#include "stdafx.h"
#include "..\DemoDlg.h"
#include "Device_Wavestation_SysexStructs.h"

void CDemoDlg::OnPrefWavestationMidiDevices()
{
	DisablePorts();
	EnablePorts(WAVESTATION_ID);
	OnPrefMididevices(WAVESTATION_ID);
}

void CDemoDlg::OnDoWavestationPgmDump()
{
	EnablePorts(WAVESTATION_ID);
	DisableButtons();

	q_Thread = ::AfxBeginThread((AFX_THREADPROC)DoWavestationPgmDump, this);
}

DWORD CDemoDlg::DoWavestationPgmDump(LPVOID Parameter)
{
	CDemoDlg *pThis = reinterpret_cast<CDemoDlg *>(Parameter);

	TCHAR NPath[MAX_PATH];	
	unsigned char channel=0;
	
	char comparestring[512];
	unsigned char presetnmbr=0;
	unsigned char xml_presetnmbr=0;

	sprintf_s(NPath,"%s\\Device Panels", pThis->MyPath);
	::CreateDirectory(NPath,NULL);
	SetCurrentDirectory(NPath);

	CHAR tMsg[7];
	tMsg[0]=0xF0;
	tMsg[1]=0x42; // Korg ID
	tMsg[2]=0x30+channel; // Midi Channel
	tMsg[3]=0x28; // Wavestation Device ID
	tMsg[4]=0x1D; // Request Dump Request
	tMsg[5]=0x00; // Bank number (1-11)
	tMsg[6]=0xF7;

	for ( int bank=0; bank < 4 ; bank++)
	{
		tMsg[5]=bank;

		// transmit sysex
		midi::CLongMsg LongMsg(tMsg,sizeof(tMsg));
		LongMsg.SendMsg(pThis->m_OutDevice);
		WaitForSingleObject(pThis->ghWriteEvent, INFINITE);
		ResetEvent(pThis->ghWriteEvent);
	}

	// Now create the device xml:
	OPEN_TEMPLATE(pThis->m_hInstance, IDR_XML_WAVESTATION)
	fopen_s(&pThis->Pgm_File, "Wavestation.xml", "w");

	sprintf_s(comparestring,"<string name=\"Name\" value=\"RAM1 Preset 0");

	char unsigned bank = 0;
	string line;							
	while(getline(Panel_Template, line))
	{
		if ( strstr(line.c_str(),
								comparestring) == NULL )
		{
			fprintf_s(pThis->Pgm_File,"%s",line.c_str());
		} else {
			fprintf_s(pThis->Pgm_File,
								"                     <string name=\"Name\" value=\"%s\" wide=\"true\"/>\r\n",
								pThis->m_wavestation_presets[bank][presetnmbr]);
			
			presetnmbr++;
			if ( presetnmbr > 49 )
			{
				bank++;
				presetnmbr = 0;
			}
			if ( presetnmbr > 99 )
			{
				xml_presetnmbr = 0;
			} else {
				xml_presetnmbr++;
			}
			sprintf_s(comparestring,
								"                     <string name=\"Name\" value=\"%s Preset %d",
								WavestationBankNames[bank],
								xml_presetnmbr);

		}
	}
	fclose(pThis->Pgm_File);
	CLOSE_TEMPLATE

	EnableButtons();

	SetCurrentDirectory(pThis->MyPath);
	return 0;
}

void CDemoDlg::WavestationSysex(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	midi::CLongMsg	LongMsg(Msg,BytesRecorded);
	performance PerfBank;
	unsigned char *ptr=NULL;

	// Byte 6 is 0x01 for bulk voice dump
	int OFFSET=6;
	int presetnmbr=0;
	unsigned char Decoded=0;
	unsigned char bank=Msg[5];
	int i;

	int size=sizeof(performance)-1;
//	DWORD pos=OFFSET;
	for (DWORD pos=OFFSET; pos < (BytesRecorded-(2*size)) ; pos=pos+(2*size))
	{
		ptr=(unsigned char *)&PerfBank;
		for(i=0;i < size;i++)
		{
			Decoded=Msg[pos+(2*i)] + (Msg[pos+(2*i)+1]<<4);
			if ( Decoded == 0x7F )
			{
				Decoded = ' ';
			}
			memcpy(ptr,&Decoded,1);
			ptr++;
		}

		sprintf_s(this->m_wavestation_presets[bank][presetnmbr],
							"%02d  %s\n",
							presetnmbr,
							PerfBank.Perf_Name);
		presetnmbr++;
		if ( presetnmbr > 49 )
		{
			presetnmbr = 0;
			bank++;
		}
	}
		
	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	SetEvent(ghWriteEvent);
}
