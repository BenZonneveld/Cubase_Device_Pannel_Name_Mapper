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
	m_Abort = false;
	m_ProgressBar.SetStep(MAXBAR/QUAD_MAX_USER);
	q_Thread = ::AfxBeginThread((AFX_THREADPROC)DoQuadUserPgmDump, this);
}

DWORD CDemoDlg::DoQuadUserPgmDump(LPVOID Parameter)
{
	CDemoDlg *pThis = (CDemoDlg *)Parameter;
	FILE *Device_Xml_File;
	TCHAR NPath[MAX_PATH];
	char comparestring[512];
	unsigned char presetnmbr=0;
	unsigned char bank=0;

	sprintf_s(NPath,"%s\\Device Panels", pThis->MyPath);
	::CreateDirectory(NPath,NULL);
	SetCurrentDirectory(NPath);
   
	unsigned char preset_high;
	unsigned char preset_lo;
	unsigned char pgm;
	unsigned char channel=pThis->m_Device_Id_Channel - 1;
	char tMsg[12];

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
	
	for ( pgm = 0 ; pgm <= QUAD_MAX_USER ; pgm++)
	{
		// construct sysex
		tMsg[10]=pgm;
		pThis->m_studioquad_presetnmbr=pgm;
	
		// transmit sysex
		midi::CLongMsg LongMsg(tMsg,sizeof(tMsg));
		LongMsg.SendMsg(pThis->m_OutDevice);
		WaitForSingleObject(pThis->ghWriteEvent, INFINITE);
		if (pThis->m_Abort == true )
			goto abort;
		ResetEvent(pThis->ghWriteEvent);
		pThis->m_ProgressBar.StepIt();
	}

	for ( pgm = 0 ; pgm <= QUAD_MAX_PRESET; pgm++)
	{
		strcpy_s(pThis->m_studioquad_presets[1][pgm],sizeof(quad_presetnames[pgm]),quad_presetnames[pgm]);
	}

	// Now create the device xml:
	OPEN_TEMPLATE(pThis->m_hInstance, IDR_XML_STUDIOQUAD)

	fopen_s(&Device_Xml_File, "Studio Quad V2.xml", "wb");

	sprintf_s(comparestring,"<string name=\"Name\" value=\"User Preset 0");

	string line;							
	while(getline(Panel_Template, line))
	{
		if ( strstr(line.c_str(),
								comparestring) == NULL )
		{
			if ( strstr(line.c_str(), "PgmChangeSysex") == NULL )
			{
				fprintf_s(Device_Xml_File,"%s",line.c_str());
			} else {
				preset_high=(presetnmbr & 0x80 ) >> 7;
				preset_lo=presetnmbr&0x7F;
				fprintf_s(Device_Xml_File,
									"														F0000010%02X461F00%02X%02X%02XF7\r\n",
									channel,
									bank,
									preset_high,
									preset_lo);
				presetnmbr++;
				if ( bank == 0 && presetnmbr > QUAD_MAX_USER) 
				{
					bank++;
					presetnmbr = 0;
				}	
				if ( bank == 1 )
				{
					sprintf_s(comparestring,
										"                     <string name=\"Name\" value=\"Factory Preset %d",
										presetnmbr);
				} else {
					sprintf_s(comparestring,
					"                     <string name=\"Name\" value=\"User Preset %d",
					presetnmbr);
				}
			}
			// Break if end of xml detected
			if ( strstr(line.c_str(),
									"</MidiDevices>") != NULL )
			{
									break;
			}
		} else {
			fprintf_s(Device_Xml_File,
				"                     <string name=\"Name\" value=\"%s\" wide=\"true\"/>\r\n",
				pThis->m_studioquad_presets[bank][presetnmbr]);
			
			if ( bank > 1 )
			{
				bank = 1;
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
	
	sprintf_s(this->m_studioquad_presets[0][this->m_studioquad_presetnmbr],
						"%03d  %s", 
						this->m_studioquad_presetnmbr+1,
						presetname);

	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	SetEvent(ghWriteEvent);
}
