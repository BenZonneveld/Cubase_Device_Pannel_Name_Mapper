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

	sprintf_s(NPath,"%s\\DX7", pThis->MyPath);
	::CreateDirectory(NPath,NULL);
	SetCurrentDirectory(NPath);

#define DX_MODE
	// For Yamaha Mode
	CHAR tMsg[5];
	tMsg[0]=0xF0;
	tMsg[1]=0x43; // Yamaha ID
	tMsg[2]=0x20+channel; // Midi Channel
	tMsg[3]=0x09; // Request Bulk Dump
	tMsg[4]=0xF7;

//	// For E!
//#define E_MODE
//	char tMsg[8];
//	tMsg[0]=0xF0;
//	tMsg[1]=0x12; // GMR ID
//	tMsg[2]=channel; // Midi Channel
//	tMsg[3]=0x00; // E! for 6 OP FM
//	tMsg[4]=0x00;	// Module #
//	tMsg[5]=0x00; // Bank 
//	tMsg[6]=0x11;	// Request 32 Voices/Functions 
//	tMsg[7]=0xF7;

	fopen_s(&pThis->Pgm_File, "DX7 Pgms.txt", "w");

#ifdef DX_MODE
#define CHANNEL	0
	for( int bank = 0 ; bank < 10 ; bank++)
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
#else
	for (int bank = 0 ; bank < 9 ; bank++)
	{
		tMsg[5]=bank;
		pThis->m_dx_bank=bank;

		// transmit sysex
		midi::CLongMsg LongMsg(tMsg,sizeof(tMsg));
		LongMsg.SendMsg(pThis->m_OutDevice);
		WaitForSingleObject(pThis->ghWriteEvent, INFINITE);
		ResetEvent(pThis->ghWriteEvent);
	}
#endif
	fclose(pThis->Pgm_File);
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
		fprintf_s(Pgm_File, "%d.%02d  %s\n",m_dx_bank, presetnmbr, presetname);
	}
		
	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	SetEvent(ghWriteEvent);
}

void CDemoDlg::YamahaDX7ESysex(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	midi::CLongMsg	LongMsg(Msg,BytesRecorded);
	char presetname[11];
	unsigned char bank=Msg[5];
	// Byte 6 is 0x01 for bulk voice dump
	int OFFSET=7;
#define NAMEOFFSET	118
	int presetnmbr=0;
	int i;
	char patchdata[257];

	for (DWORD pos=OFFSET; pos < BytesRecorded ; pos=pos+256)
	{
		memcpy(patchdata,Msg+pos,256);
		for (i=0; i < 10; i++)
		{
			presetname[i]=patchdata[0x7e+(2*i)]&&patchdata[0x7f+(2*i)];
		}
		presetname[i]=0x0;
		presetnmbr++;
		fprintf_s(Pgm_File, "%d.%3d  %s\n", bank, presetnmbr, presetname);
	}
		
	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	SetEvent(ghWriteEvent);
}
