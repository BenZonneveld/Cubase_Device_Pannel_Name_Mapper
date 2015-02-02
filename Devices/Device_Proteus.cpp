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
	m_Abort = false;
	ResetEvent(ghWriteEvent);
	m_ProgressBar.SetStep(MAXBAR/191);
	q_Thread = ::AfxBeginThread((AFX_THREADPROC)DoProteusPgmDump, this);
}

DWORD CDemoDlg::DoProteusPgmDump(LPVOID Parameter)
{
	CDemoDlg *pThis = reinterpret_cast<CDemoDlg *>(Parameter);
	FILE *Device_Xml_File;
	TCHAR NPath[MAX_PATH];
	char tMsg[8];
	int device_id=pThis->m_Device_Id_Channel;

	char comparestring[512];
	unsigned char presetnmbr=0;
	sprintf_s(NPath,"%s\\Device Panels", pThis->MyPath);
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
	tMsg[3]=device_id; // Device ID
	tMsg[4]=0x00;	// Command
	tMsg[5]=0x10; // Preset Number lsb 
	tMsg[6]=0x00;	// Preset Number msb
	tMsg[7]=0xF7;
	
	for (int count =0 ; count < 191 ; count++)
	{
		tMsg[5]=count&0x7F;
		tMsg[6]=(count&0x80)>>7;

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
	OPEN_TEMPLATE(pThis->m_hInstance, IDR_XML_PROTEUS)

	fopen_s(&Device_Xml_File, "Proteus.xml", "wb");

	sprintf_s(comparestring,"<string name=\"Name\" value=\"Preset 0");

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
			pThis->m_ProgressBar.StepIt();
			fprintf_s(Device_Xml_File,
				"                     <string name=\"Name\" value=\"%s\" wide=\"true\"/>\r\n",
				pThis->m_proteus_presets[presetnmbr]);
			
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
	presetnmbr--;
	if ( presetnmbr > 127 )
	{
		;
	} else {
		sprintf_s(this->m_proteus_presets[presetnmbr],
			"%3d  %s", 
			presetnmbr,
			presetname);
	}
	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	SetEvent(ghWriteEvent);
}

