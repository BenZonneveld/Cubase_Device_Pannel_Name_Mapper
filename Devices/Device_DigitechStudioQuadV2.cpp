#include "stdafx.h"
#include "..\DemoDlg.h"
#include "Device_Quad_Factory.h"

void CDemoDlg::OnPrefQuadMididevices()
{
	DisablePorts();
	EnablePorts(DIGITECH_ID);
	OnPrefMididevices(DIGITECH_ID);
}

// Do the pgm dump for the quad
void CDemoDlg::OnDoQuadUserPgmDump()
{
	EnablePorts(DIGITECH_ID);
	DisableButtons();

	q_Thread = ::AfxBeginThread((AFX_THREADPROC)DoQuadUserPgmDump, this);
}



DWORD CDemoDlg::DoQuadUserPgmDump(LPVOID Parameter)
{
	CDemoDlg *pThis = (CDemoDlg *)Parameter;

	TCHAR NPath[MAX_PATH];	
	sprintf_s(NPath,"%s\\Studio Quad V2", pThis->MyPath);
	::CreateDirectory(NPath,NULL);
	SetCurrentDirectory(NPath);
   
	unsigned char pgm;
	unsigned char channel=0;
	char tMsg[12];

#define MAX_USER	0x63
#define MAX_PRESET 0xB3

	tMsg[0]=0xF0;
	tMsg[1]=0x00;
	tMsg[2]=0x00;
	tMsg[3]=0x10;
	tMsg[4]=channel;
	tMsg[5]=0x46;
	tMsg[6]=0x01;
	tMsg[7]=0x00;
	tMsg[8]=0;
	tMsg[9]=0x00;
	tMsg[11]=0xF7;
	
	fopen_s(&pThis->Pgm_File, "QuadV2 Pgms.txt", "w");

	for ( pgm = 0 ; pgm <= MAX_USER ; pgm++)
	{
		// construct sysex
		tMsg[10]=pgm;
	
		fprintf_s(pThis->Pgm_File,"User %03d  ", pgm+1);
		// transmit sysex
		midi::CLongMsg LongMsg(tMsg,sizeof(tMsg));
		LongMsg.SendMsg(pThis->m_OutDevice);
		WaitForSingleObject(pThis->ghWriteEvent, INFINITE);
		ResetEvent(pThis->ghWriteEvent);
	}

	// Print the factory Presetnames
	for ( pgm = 0 ; pgm < 180 ; pgm++ )
	{
		fprintf_s(pThis->Pgm_File,"%s\n", quad_presetnames[pgm]);
	}
	fclose(pThis->Pgm_File);
	EnableButtons();

	SetCurrentDirectory(pThis->MyPath);
	return 0;
}


void CDemoDlg::DigitechQuadSysex(LPSTR Msg, DWORD BytesRecorded,DWORD TimeStamp)
{
	midi::CLongMsg	LongMsg(Msg,BytesRecorded);

	char presetname[30];
	int count=0;
	int modulecountIDs=Msg[20]+1;
	int modulecountTypes=Msg[20+(2*(modulecountIDs+1))]+1;
	int NameOffset=20+(2*(modulecountIDs+1))+(2*modulecountTypes);

	for(int pos=0; pos <= 48; pos++)
	{
		if ( Msg[pos+NameOffset]==0 && Msg[pos+NameOffset+1]==0)
			break;
		if ( Msg[pos+NameOffset]!= 0 )
		{
			presetname[count]=Msg[pos+NameOffset];
			count++;
		}
	}
	presetname[count]=0x00;

	fprintf_s(Pgm_File, "%s\n", presetname);
	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	SetEvent(ghWriteEvent);
}
