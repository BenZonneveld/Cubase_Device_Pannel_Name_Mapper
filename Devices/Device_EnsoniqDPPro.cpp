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

	q_Thread = ::AfxBeginThread((AFX_THREADPROC)DoDPProPgmDump, this);

	MessageBox("On the DP Pro goto 'System', page 15, select type 'All RAM Presets' and press Enter on the DP Pro.", "Warning", 
                MB_ICONWARNING | MB_OK);
}

DWORD CDemoDlg::DoDPProPgmDump(LPVOID Parameter)
{
	CDemoDlg *pThis = reinterpret_cast<CDemoDlg *>(Parameter);

	TCHAR NPath[MAX_PATH];	
	sprintf_s(NPath,"%s\\DP Pro", pThis->MyPath);
	::CreateDirectory(NPath,NULL);
	SetCurrentDirectory(NPath);

	fopen_s(&pThis->Pgm_File, "DPPro User Pgms.txt", "w");

	// transmit sysex
	WaitForSingleObject(pThis->ghWriteEvent, INFINITE);
	ResetEvent(pThis->ghWriteEvent);

	fclose(pThis->Pgm_File);
	EnableButtons();

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
	int pos=0;
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
		fprintf_s(Pgm_File, "%d%03d  %s\n", bank,presetnmbr, presetname);

		++presetnmbr;
		if ( presetnmbr > 127 )
		{
			++bank;
			presetnmbr=0;
		}

		pos=pos+PGMOFFSET;
	}	
	// Print the factory Presetnames
	for ( count = 0 ; count < 127 ; count++ )
	{
		fprintf_s(Pgm_File,"%s\n", dp_presetnames[count]);
	}

	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	SetEvent(ghWriteEvent);
}

