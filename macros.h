#define LINE_LENGTH(x) sizeof(x[0])
#define ARRAY_SIZE(x)  ( sizeof( x ) / LINE_LENGTH(x) )

#define ENABLE_BUTTON(x) (x.EnableWindow(TRUE); x.UpdateWindow(); )

#define EnableButtons() \
	pThis->m_QUserPgms.EnableWindow(TRUE); \
	pThis->m_QMidi.EnableWindow(TRUE); \
	pThis->m_SpxPgms.EnableWindow(TRUE); \
	pThis->m_SpxMidi.EnableWindow(TRUE); \
	pThis->m_DPProPgms.EnableWindow(TRUE); \
	pThis->m_DPProMidi.EnableWindow(TRUE); \
	pThis->m_MWXTPgms.EnableWindow(TRUE); \
	pThis->m_MWXTMidi.EnableWindow(TRUE); \
	pThis->m_ProteusPgms.EnableWindow(TRUE); \
	pThis->m_ProteusMidi.EnableWindow(TRUE); \
	pThis->m_DX7Pgms.EnableWindow(TRUE); \
	pThis->m_DX7Midi.EnableWindow(TRUE); \
	pThis->m_WavestationPgms.EnableWindow(TRUE); \
	pThis->m_WavestationMidi.EnableWindow(TRUE); \
	pThis->m_QUserPgms.UpdateWindow(); \
	pThis->m_QMidi.UpdateWindow(); \
	pThis->m_SpxPgms.UpdateWindow(); \
	pThis->m_SpxMidi.UpdateWindow(); \
	pThis->m_DPProPgms.UpdateWindow(); \
	pThis->m_DPProMidi.UpdateWindow(); \
	pThis->m_MWXTPgms.UpdateWindow(); \
	pThis->m_MWXTMidi.UpdateWindow(); \
	pThis->m_ProteusPgms.UpdateWindow(); \
	pThis->m_ProteusMidi.UpdateWindow(); \
	pThis->m_DX7Pgms.UpdateWindow(); \
	pThis->m_DX7Midi.UpdateWindow(); \
	pThis->m_WavestationPgms.UpdateWindow(); \
	pThis->m_WavestationMidi.UpdateWindow();
