#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "pgLibConnect.h"
#include "P2PListen.h"
#include "string.h"

#define PTC_MSG_SIZE			(4*1024)

typedef struct
{
	int create_flag;
	unsigned int connect;
	unsigned int uInstID;
	unsigned int uSessionID;
	char szSessionID[64];
	uint64_t recv_jiffies;
	char p2p_server[32];
	pthread_t msg_thread_id;
}session_info_t;

typedef struct
{
	unsigned int flag;		//QHKJ
	unsigned int cmd;
	unsigned int len;
	unsigned short ver;
	unsigned short ack;
	unsigned short pack_count;
	unsigned short pack_no;
	unsigned int pack_len;
	unsigned char channel_type;		//????????0=????????1=??????
	unsigned char channel_no;		//????????0???
	unsigned char reserved[6];
}ptc_head_t;

typedef struct
{
	ptc_head_t head;
	unsigned char data[PTC_MSG_SIZE];
}ptc_msg_t;

static uint64_t curr_jiffies;
static p2pCallback g_p2p_callback;
static int g_p2p_init = 0;

static void* sanbot_jiffies_thread(void* p)
{
	pthread_detach(pthread_self());
	
	while(g_p2p_init)
	{
		usleep(1*1000*1000);
		curr_jiffies++;
	}
	return NULL;
}

static void* P2PMsgThread(void* param)
{
	pthread_detach(pthread_self());
	
	session_info_t* pSession = (session_info_t *)param;
	if(pSession)
	{
		PG_INIT_CFG_S stInitCfg;
		memset(&stInitCfg, 0, sizeof(stInitCfg));
		int retInit = pgInitialize(&pSession->uInstID, PG_MODE_LISTEN, pSession->szSessionID, "", pSession->p2p_server, "", &stInitCfg, NULL);
		if ( retInit != PG_ERROR_OK)
		{
			printf("%s --> %s --> L%d : Init peergine module failed error[%d]\n", __FILE__, __FUNCTION__, __LINE__, retInit);
			return NULL;
		}

		unsigned int uInstID = pSession->uInstID;
		pSession->create_flag = 1;

		while(pSession->create_flag)
		{
			if(pSession->connect == 1 && curr_jiffies - pSession->recv_jiffies > 10)
			{//回调给设备端(客户端心跳超时，设备端需要停止传流、关闭视频)
				printf("%s --> %s --> L%d : client is offline \n", __FILE__, __FUNCTION__, __LINE__);
				if(g_p2p_callback)
				{
					g_p2p_callback((unsigned long)pSession, PTC_CMD_CLIENT_TIMEOUT, NULL);
				}
				//pSession->create_flag = 0;
				continue;
			}
			else if(pSession->connect == 0 && curr_jiffies - pSession->recv_jiffies > 20)
			{//回调给设备端(客户端未连接)
				printf("%s --> %s --> L%d : client is not comes \n", __FILE__, __FUNCTION__, __LINE__);
				if(g_p2p_callback)
				{
					g_p2p_callback((unsigned long)pSession, PTC_CMD_CLIENT_NOT_CONNECT, NULL);
				}
				//pSession->create_flag = 0;
				continue;
			}

			unsigned int uEventNow = PG_EVENT_NULL;
			unsigned int uSessIDNow = 0;
			unsigned int uPrio = 0;
			int iErr = pgEvent(uInstID, &uEventNow, &uSessIDNow, &uPrio, 20);
			if (iErr != PG_ERROR_OK)
			{
				if (iErr != PG_ERROR_TIMEOUT)
				{
					printf("%s --> %s --> L%d : pgEvent iErr=%d\n", __FILE__, __FUNCTION__, __LINE__, iErr);
				}
				continue;
			}
			if (uEventNow == PG_EVENT_CONNECT) 
			{
				printf("%s --> %s --> L%d : CONNECT SessID=%u\n", __FILE__, __FUNCTION__, __LINE__, uSessIDNow);
				pSession->uSessionID = uSessIDNow;
			}
			else if (uEventNow == PG_EVENT_INFO)
			{//连接方式发生变化---暂不需要处理
				/*PG_INFO_S stInfo;
				int iRet = pgInfo(uInstID, uSessIDNow, &stInfo);
				if (iRet == PG_ERROR_OK)
				{
					printf("pgInfo: PeerID=%s, AddrPub=%s, AddrPriv=%s, CnntType=%u\n",	stInfo.szPeerID, stInfo.szAddrPub, stInfo.szAddrPriv, stInfo.uCnntType);
				}*/
			}
			else if (uEventNow == PG_EVENT_CLOSE)
			{//断开或者对方关闭
				printf("%s --> %s --> L%d : CLOSE SessID=%u\n", __FILE__, __FUNCTION__, __LINE__, uSessIDNow);
				//回调给设备端(断开或者对方关闭)
				if(g_p2p_callback)
				{
					g_p2p_callback((unsigned long)pSession, PTC_CMD_CLIENT_CLOSED, NULL);
				}
				
				pgClose(uInstID, uSessIDNow);
				pSession->uSessionID = 0;
				//pSession->create_flag = 0;
				continue;
			}
			else if (uEventNow == PG_EVENT_SVR_LOGIN)
			{
				printf("%s --> %s --> L%d : SVR_LOGIN  \n", __FILE__, __FUNCTION__, __LINE__);
			}
			else if (uEventNow == PG_EVENT_SVR_LOGOUT)
			{//从P2P服务器注销或掉线（下线）
				printf("%s --> %s --> L%d : SVR_LOGOUT \n", __FILE__, __FUNCTION__, __LINE__);
				//回调给设备端(从P2P服务器注销或掉线)
				if(g_p2p_callback)
				{
					g_p2p_callback((unsigned long)pSession, PTC_CMD_SVR_LOGOUT, NULL);
				}
				//pSession->uSessionID = 0;
				//pSession->create_flag = 0;
				continue;
			}
			else if (uEventNow == PG_EVENT_SVR_REPLY) 
			{// P2P服务器应答事件，可以调用pgServerReply()接收应答。
				/*printf("SVR_REPLY:\n");
				char szBuf[1024] = {0};
				unsigned int uParam = 0;
				int iRet = pgServerReply(uInstID, szBuf, sizeof(szBuf), &uParam);
				if (iRet < PG_ERROR_OK)
				{
					printf("pgServerReply: iErr=%d\n", iRet);
					continue;
				}
				szBuf[iRet] = '\0';
				printf("Reply: %s\n", szBuf);*/
			}
			else if (uEventNow == PG_EVENT_SVR_NOTIFY) 
			{// P2P服务器推送事件，可以调用pgServerNotify()接收推送。
				/*printf("SVR_NOTIFY:\n");
				char szBuf[1024] = {0};
				unsigned int uParam = 0;
				int iRet = pgServerNotify(uInstID, szBuf, sizeof(szBuf));
				if (iRet < PG_ERROR_OK) 
				{
					printf("pgServerNotify: iErr=%d\n", iRet);
					continue;
				}
				szBuf[iRet] = '\0';
				printf("Notify: %s\n", szBuf);*/
			}
			else if (uEventNow == PG_EVENT_WRITE) 
			{// 会话的底层发送缓冲区的空闲空间增加了，可以调用pgWrite()发送新数据。
			}
			else if (uEventNow == PG_EVENT_READ)
			{// 会话的底层接收缓冲区有数据到达，可以调用pgRead()接收新数据。
				ptc_msg_t msg;
				memset(&msg, 0, sizeof(msg));
				unsigned int uPrio1 = 0;
				int iRet = pgRead(uInstID, uSessIDNow, &msg, (sizeof(msg)), &uPrio1);
				if (iRet < PG_ERROR_OK) 
				{
					printf("pgRead: iErr=%d\n", iRet);
					continue;
				}
				pSession->connect = 1;
				pSession->recv_jiffies = curr_jiffies;
	
				if(uPrio1 == 0)
				{
					//printf("READ: SessID=%u, uPrio1=%u, cmd=%#u\n", uSessIDNow, uPrio1, msg.head.cmd);
					if (msg.head.cmd == PTC_CMD_START_PREVIEW)
					{//预览
						printf("RECV[%s]\n",msg.data);
						if(g_p2p_callback)
						{
							int ret = g_p2p_callback((unsigned long)pSession, PTC_CMD_START_PREVIEW, (char*)msg.data);
							if(ret == 0)
							{
								const char* suc = "{\"result\":0}";
								ptc_msg_t retmsg;
								memcpy(&retmsg, &msg, sizeof(ptc_head_t));
								retmsg.head.len = strlen(suc);
								retmsg.head.pack_len = strlen(suc);
								memcpy(retmsg.data, suc, retmsg.head.len);
								int datalen = sizeof(ptc_head_t) + retmsg.head.len;
								int ret = pgWrite(uInstID,  uSessIDNow, &retmsg, datalen , uPrio1);
								if(ret < PG_ERROR_OK)
								{
									printf("0:pgWrite error %d\n",ret);
								}
							}
							else
							{
								const char* suc = "{\"result\":21003}";
								ptc_msg_t retmsg;
								memcpy(&retmsg, &msg, sizeof(ptc_head_t));
								retmsg.head.len = strlen(suc);
								retmsg.head.pack_len = strlen(suc);
								memcpy(retmsg.data, suc, retmsg.head.len);
								int datalen = sizeof(ptc_head_t) + retmsg.head.len;
								ret = pgWrite(uInstID,  uSessIDNow, &retmsg, datalen , uPrio1);
								if(ret < PG_ERROR_OK)
								{
									printf("21003:pgWrite error %d\n",ret);
								}
							}
						}
						else
						{
							const char* suc = "{\"result\":21003}";
							ptc_msg_t retmsg;
							memcpy(&retmsg, &msg, sizeof(ptc_head_t));
							retmsg.head.len = strlen(suc);
							retmsg.head.pack_len = strlen(suc);
							memcpy(retmsg.data, suc, retmsg.head.len);
							int datalen = sizeof(ptc_head_t) + retmsg.head.len;
							int ret = pgWrite(uInstID,  uSessIDNow, &retmsg, datalen , uPrio1);
							if(ret < PG_ERROR_OK)
							{
								printf("21003:pgWrite error %d\n",ret);
							}
						}
					}
					else if (msg.head.cmd == PTC_CMD_STOP_PREVIEW)
					{//关闭预览
						if(g_p2p_callback)
						{
							g_p2p_callback((unsigned long)pSession, PTC_CMD_STOP_PREVIEW, NULL);
							continue;
						}
					}
				}
				else if(uPrio1 == 1)
				{
					int ret = pgWrite(uInstID, uSessIDNow, &msg, sizeof(ptc_head_t), uPrio1); 
					if(ret < PG_ERROR_OK)
					{
						printf("heartbeat failed uInstID=%u\n", uInstID);
					}
				}
			}
		}
		
		if(pSession->uSessionID)
		{
			pgClose(pSession->uInstID, pSession->uSessionID);
			pSession->uSessionID = 0;
		}
		pgCleanup(pSession->uInstID);
		
		free(pSession);
		printf("Clean peergine module.delete object\n");
	}
	
 	return NULL;
}

int P2P_Init(p2pCallback callback)
{
	g_p2p_callback = callback;
	g_p2p_init = 1;
	
	pthread_t id;
	int n = pthread_create(&id, NULL, sanbot_jiffies_thread, NULL);
	if(n != 0)
	{
		g_p2p_init = 0;
		return -1;
	}
	
	return 0;
}

void P2P_CleanUp()
{
	g_p2p_init = 0;
	g_p2p_callback = NULL;
}

unsigned long P2P_CreateSession(char* pSessionID, char* p2p_ip, int p2p_port)
{
	if(pSessionID == NULL || strlen(pSessionID) >= 64)
	{
		printf("%s --> %s --> L%d : input param error \n", __FILE__, __FUNCTION__, __LINE__);
		return 0;
	}
	
	session_info_t* session = (session_info_t*)malloc(sizeof(session_info_t));
	if(session == NULL)
	{
		printf("%s --> %s --> L%d : malloc failed\n", __FILE__, __FUNCTION__, __LINE__);
		return 0;
	}
	
	memset(session, 0, sizeof(session_info_t));
	memcpy(session->szSessionID, pSessionID, strlen(pSessionID));
	sprintf(session->p2p_server, "%s:%d", p2p_ip, p2p_port);
	session->recv_jiffies = curr_jiffies;
	
	int n = pthread_create(&session->msg_thread_id, NULL, P2PMsgThread, (void*)session);
	if(n != 0)
	{
		printf("%s --> %s --> L%d : can't create P2PMsgThread errno is %d\n", __FILE__, __FUNCTION__, __LINE__, n);
		free(session);
		return 0;
	}
	
	return (unsigned long)session;
}

int P2P_CloseSession(unsigned long handler)
{
	session_info_t* session = (session_info_t*)handler;
	if(session)
	{
		session->create_flag = 0;
		return 0;
	}
	
	return 1;
}

int P2P_SendStream(unsigned long handler, void* data, int len)
{
	session_info_t* session = (session_info_t*)handler;
	if(session)
	{
		return pgWrite(session->uInstID, session->uSessionID, data, len, 2);
	}
	
	return 1;
}
