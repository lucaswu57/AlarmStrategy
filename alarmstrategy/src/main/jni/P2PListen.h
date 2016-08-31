#ifndef _P2PLISTEN_H_
#define _P2PLISTEN_H_

#include <stdint.h>
#include "HDDefine.h"

#ifdef __cplusplus
extern "C"
{
#endif

int P2P_Init(p2pCallback callback);
void P2P_CleanUp();
unsigned long P2P_CreateSession(char* pSessionID, char* p2p_ip, int p2p_port);
int P2P_CloseSession(unsigned long handler);
int P2P_SendStream(unsigned long handler, void* data, int len);

#ifdef __cplusplus
}
#endif

#endif//_P2PLISTEN_H_