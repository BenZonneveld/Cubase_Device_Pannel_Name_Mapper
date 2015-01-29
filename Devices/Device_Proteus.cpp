#include "stdafx.h"
#include "..\DemoDlg.h"

void CDemoDlg::OnPrefProteusMidiDevices()
{
	DisablePorts();
	EnablePorts(PROTEUS_ID);
	OnPrefMididevices(PROTEUS_ID);
}

void CDemoDlg::OnDoProteusPgmDump()
{
	EnablePorts(PROTEUS_ID);
	DisableButtons();
	q_Thread = ::AfxBeginThread((AFX_THREADPROC)DoProteusPgmDump, this);
}

DWORD CDemoDlg::DoProteusPgmDump(LPVOID Parameter)
{
	CDemoDlg *pThis = reinterpret_cast<CDemoDlg *>(Parameter);
	TCHAR NPath[MAX_PATH];
	char tMsg[8];
	int channel=0;

	char comparestring[512];
	unsigned char presetnmbr=0;
	sprintf_s(NPath,"%s\\Proteus_1", pThis->MyPath);
	::CreateDirectory(NPath,NULL);
	SetCurrentDirectory(NPath);

	/*
	F0 18 04 dd 00 ll mm F7 	ll = preset # lsb, mm = msb
		7F7F = all user presets
		7E7F = all factory presets
		*/
	tMsg[0]=0xF0;
	tMsg[1]=0x18; // Emu ID
	tMsg[2]=0x04; // Product ID for Proteus
	tMsg[3]=0x00; // Device ID
	tMsg[4]=0x00;	// Command
	tMsg[5]=0x10; // Preset Number lsb 
	tMsg[6]=0x00;	// Preset Number msb
	tMsg[7]=0xF7;
	
	fopen_s(&pThis->Pgm_File, "Proteus Pgms.txt", "w");

	for (int count =0 ; count < 191 ; count++)
	{
		tMsg[5]=count&0x7F;
		tMsg[6]=(count&0x80)>>7;

		// transmit sysex
		midi::CLongMsg LongMsg(tMsg,sizeof(tMsg));
		LongMsg.SendMsg(pThis->m_OutDevice);
		WaitForSingleObject(pThis->ghWriteEvent, INFINITE);
		ResetEvent(pThis->ghWriteEvent);
	}

	fclose(pThis->Pgm_File);

	// Now create the device xml:
	fopen_s(&pThis->Pgm_File, "Proteus.xml", "w");

	sprintf_s(comparestring,"<string name=\"Name\" value=\"Preset 0");

	for ( int count = 0 ; count < ARRAY_SIZE(pThis->m_proteus_template) ; count++)
	{
//<string name=\"Name\" value=\"Preset
		if ( strstr(pThis->m_proteus_template[count],
								comparestring) == NULL )
		{
			fprintf_s(pThis->Pgm_File,"%s\r\n",pThis->m_proteus_template[count]);
		} else {
			fprintf_s(pThis->Pgm_File, "                     <string name=\"Name\" value=\"%s\" wide=\"true\"/>\r\n",pThis->m_proteus_presets[presetnmbr]);
			
			presetnmbr++;
			if ( presetnmbr < 128 )
			{
				sprintf_s(comparestring,"                     <string name=\"Name\" value=\"Preset %d",presetnmbr);
			}
		}
	}

	fclose(pThis->Pgm_File);
	EnableButtons();

	SetCurrentDirectory(pThis->MyPath);
	return 0;
}

void CDemoDlg::ProteusSysex(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	midi::CLongMsg	LongMsg(Msg,BytesRecorded);
	char presetname[13];
	int presetnmbr=Msg[5]+(Msg[6]<<7)+1;
	int i=0;

	for (int pos=7; pos < 7+(2*12) ; pos=pos+2)
	{
		presetname[i]=Msg[pos];
		i++;
	}
		
	presetname[i]=0x0;
	if ( presetnmbr > 128 )
	{
		fprintf_s(Pgm_File, "x%03d  %s\n",presetnmbr, presetname);
	} else {
		sprintf_s(this->m_proteus_presets[presetnmbr-1],"%03d  %s", presetnmbr, presetname);
		fprintf_s(Pgm_File, "%03d  %s\n",presetnmbr, presetname);
	}
	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	SetEvent(ghWriteEvent);
}

