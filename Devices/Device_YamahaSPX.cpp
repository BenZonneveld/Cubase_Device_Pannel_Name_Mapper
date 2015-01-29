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

	q_Thread = ::AfxBeginThread((AFX_THREADPROC)DoSpxUserPgmDump, this);
}

DWORD CDemoDlg::DoSpxUserPgmDump(LPVOID Parameter)
{
	CDemoDlg *pThis = reinterpret_cast<CDemoDlg *>(Parameter);

	TCHAR NPath[MAX_PATH];	
	sprintf_s(NPath,"%s\\SPX990", pThis->MyPath);
	::CreateDirectory(NPath,NULL);
	SetCurrentDirectory(NPath);

	int MaxPreset=100;

	int channel=0;
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
	
	fopen_s(&pThis->Pgm_File, "Spx User Pgms.txt", "w");

	// transmit sysex
	for ( int pgm = 1 ; pgm <= MaxPreset ; pgm++)
	{
		// construct sysex
		tMsg[13]=pgm;
	
		fprintf_s(pThis->Pgm_File,"%03d  ", pgm);
		// transmit sysex
		midi::CLongMsg LongMsg(tMsg,sizeof(tMsg));
		LongMsg.SendMsg(pThis->m_OutDevice);
		WaitForSingleObject(pThis->ghWriteEvent, INFINITE);
		ResetEvent(pThis->ghWriteEvent);
	}

	fclose(pThis->Pgm_File);
	EnableButtons();

	SetCurrentDirectory(pThis->MyPath);
	return 0;
}

void CDemoDlg::YamahaSPXSysex(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	midi::CLongMsg	LongMsg(Msg,BytesRecorded);
	int NameOffset=16;
	char PgmName[17];
	int count=0;

	for(int byte=0; byte < (2*16) ; byte+=2)
	{
		PgmName[count]=(Msg[byte+NameOffset]<<4)+Msg[byte+NameOffset+1];
		count++;
	}	
	PgmName[count]=0x00;
	fprintf_s(Pgm_File, "%s\n", PgmName);
	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	SetEvent(ghWriteEvent);
}