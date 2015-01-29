#include "stdafx.h"
#include "..\DemoDlg.h"

void CDemoDlg::OnPrefMWXTMidiDevices()
{
//	int size=ARRAY_SIZE(m_microwave_template);
	DisablePorts();
	EnablePorts(MWXT_ID);
	OnPrefMididevices(MWXT_ID);
}

void CDemoDlg::OnDoMWXTPgmDump()
{
	EnablePorts(MWXT_ID);
	DisableButtons();

	q_Thread = ::AfxBeginThread((AFX_THREADPROC)DoMWXTPgmDump, this);
}

DWORD CDemoDlg::DoMWXTPgmDump(LPVOID Parameter)
{
	CDemoDlg *pThis = reinterpret_cast<CDemoDlg *>(Parameter);

	TCHAR NPath[MAX_PATH];	
	sprintf_s(NPath,"%s\\Microwave XT", pThis->MyPath);
	::CreateDirectory(NPath,NULL);
	SetCurrentDirectory(NPath);

	int MaxPreset=100;

	int channel=0;
	char tMsg[9];

	tMsg[0]=0xF0;
	tMsg[1]=0x3e; // Waldorf ID
	tMsg[2]=0x0E; // Equipment ID for Microwave 2
	tMsg[3]=0x7F; // Device ID, here broadcast
	tMsg[4]=0x00;	// Message ID: SNDR
	tMsg[5]=0x10;
	tMsg[6]=0x00;
	tMsg[7]=(tMsg[5]+tMsg[6])&0x7F;
	tMsg[8]=0xF7;
	
	fopen_s(&pThis->Pgm_File, "MWXT Pgms.txt", "w");

	// transmit sysex
	midi::CLongMsg LongMsg(tMsg,sizeof(tMsg));
	LongMsg.SendMsg(pThis->m_OutDevice);
	WaitForSingleObject(pThis->ghWriteEvent, INFINITE);
	ResetEvent(pThis->ghWriteEvent);

	fclose(pThis->Pgm_File);
	EnableButtons();

	SetCurrentDirectory(pThis->MyPath);
	return 0;
}

void CDemoDlg::MWXTSysex(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	midi::CLongMsg	LongMsg(Msg,BytesRecorded);
	char presetname[17];
	int OFFSET=247;
	int presetnmbr=1;
	char bank[2]="A";
	DWORD pos=OFFSET;
	int i;

	while ( pos < BytesRecorded )
	{
		for( i=0; i < 16 ; i++)
		{
			presetname[i]=Msg[pos+i];
		}
		presetname[i]=0x0;
		fprintf_s(Pgm_File, "%s%03d  %s\n", bank,presetnmbr, presetname);
		presetnmbr++;
		if ( presetnmbr > 128 )
		{
			bank[0]='B';
			presetnmbr=1;
		}
		pos=pos+256;
	}

	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	SetEvent(ghWriteEvent);
}

