#include "global.h"

#ifdef _RELEASE
QWSInputMethod *g_im = NULL;
#endif

const int g_comBufSize = 1024;
QQueue<unsigned char> g_comBuf;
QSemaphore g_comBufFree(g_comBufSize), g_comBufUsed(0);

const int g_tcpBufSize = 64;
QQueue<unsigned char> g_tcpBuf;
QSemaphore g_tcpBufFree(g_tcpBufSize), g_tcpBufUsed(0);
