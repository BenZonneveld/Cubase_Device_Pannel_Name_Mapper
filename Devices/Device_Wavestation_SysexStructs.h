typedef struct
{
	char	Bank_Num;
	char	Patch_Num;
	unsigned char	Level;
	char	output;
	unsigned char Part_Mode;
	unsigned char Lo_Key;
	unsigned char Hi_Key;
	unsigned char Lo_Vel;
	unsigned char Hi_Vel;
	char	Trans;
	char	Detune;
	unsigned char	TuneTab;
	unsigned char	Micro_Tune_Key;
	unsigned char Midi_Out_Channel;
	char	Midi_Prog_Num;
	byte	Sus_Enable;
	unsigned short	Delay;
} part;

typedef struct
{
	char	Perf_Name[16];
	char	Fx_Perf_Block[21];
	part	Parts[8];
} performance;

char WavestationBankNames[4][5]=
{	"RAM1","RAM2","ROM","CARD" };