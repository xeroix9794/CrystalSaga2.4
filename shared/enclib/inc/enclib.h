#if !defined __ENC_LIB__
#define __ENC_LIB__

typedef unsigned char __byte;

extern int SetEncKey(const __byte* key);

extern int Encrypt(__byte* buf, int len, const __byte* pwd, int plen);
extern int Decrypt(__byte* buf, int len, const __byte* enc, int elen);

#if NDEBUG
#pragma comment(lib, "EncLib.lib")
#else
#pragma comment(lib, "EncLib_d.lib")
#endif

#endif
