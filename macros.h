#define LINE_LENGTH(x) sizeof(x[0])
#define ARRAY_SIZE(x)  ( sizeof( x ) / LINE_LENGTH(x) )

#define FIND_XML_RESOURCE(x,y) FindResource(x, MAKEINTRESOURCE(y), "Xml_Template") 

#define OPEN_TEMPLATE(x,y) 	HRSRC hResource = FIND_XML_RESOURCE(x,y); \
	if (hResource) \
	{ \
		HGLOBAL hLoadedResource = LoadResource(x, hResource); \
		if (hLoadedResource) \
		{ \
				LPVOID pLockedResource = LockResource(hLoadedResource); \
				if (pLockedResource) \
				{ \
						DWORD dwResourceSize = SizeofResource(x, hResource); \
						if (0 != dwResourceSize) \
						{ \
							istrstream Panel_Template(static_cast<char *>(pLockedResource)); \
							Panel_Template.rdbuf()->pubsetbuf(static_cast<char *>(pLockedResource), dwResourceSize);

#define CLOSE_TEMPLATE\
							memset(static_cast<char *>(pLockedResource), 0, dwResourceSize);\
							UnlockResource(pLockedResource);\
						}\
        }\
    }\
	}


#define DISABLE_BUTTON(x) x.EnableWindow(FALSE); \
	x.UpdateWindow();
#define ENABLE_BUTTON(x) pThis->x.EnableWindow(TRUE); \
	pThis->x.UpdateWindow();
#define EnableButtons() \
	ENABLE_BUTTON(m_QUserPgms) \
	ENABLE_BUTTON(m_QMidi) \
	ENABLE_BUTTON(m_SpxPgms) \
	ENABLE_BUTTON(m_SpxMidi) \
	ENABLE_BUTTON(m_DPProPgms) \
	ENABLE_BUTTON(m_DPProMidi) \
	ENABLE_BUTTON(m_MWXTPgms) \
	ENABLE_BUTTON(m_MWXTMidi) \
	ENABLE_BUTTON(m_ProteusPgms) \
	ENABLE_BUTTON(m_ProteusMidi) \
	ENABLE_BUTTON(m_DX7Pgms) \
	ENABLE_BUTTON(m_DX7Midi) \
	ENABLE_BUTTON(m_WavestationPgms) \
	ENABLE_BUTTON(m_WavestationMidi)
