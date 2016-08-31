/****************************************************************
  copyright   : Copyright (C) 2013, chenbichao,
              : All rights reserved.
              : www.pptun.com, www.peergine.com
              : 
  filename    : pgLibConnect.h
  discription : 
  modify      : create, chenbichao, 2013/11/10
              :
			  : modify, chenbichao, 2013/12/18
			  : 1. pgEvent()������uTimeout��������Ϊ��0ʱ���ȴ����������ء�
			  :
			  : modify, chenbichao, 2013/12/30
			  : 1. pgEvent()�ӿں��������ϱ����ߺ����������¼�
			  :
			  : modify, chenbichao, 2014/03/17
			  : 1. PG_INIT_CFG_S�ṹ����uTryP2PTimeout���ԣ�����ָ��P2P��͸�ĳ�ʱʱ�䡣
			  :
			  : modify, chenbichao, 2014/03/26
			  : 1. ����PG_EVENT_OFFLINE�¼�������ָ������ʱ�Զ˲����ߡ�
			  :
			  : modify, chenbichao, 2014/04/01
			  : 1. ����PG_EVENT_INFO�¼���ָʾ���˻�Զ˵�NAT���͡���ַ��Ϣ���״̬��
			  : 2. ���ӡ�PG_CNNT_E��ö�٣���ʾ���ӵ����ͣ�NAT���ͣ�
			  : 3. �ṹ��PG_INFO_S����uIsForward���Ը�ΪuCnntType����ʾ�������͡�
			  :
			  : modify, chenbichao, 2014/04/02
			  : 1. ����pgLevel()�ӿں��������������͹رղ�ͬ������־�����
			  :
			  : modify, chenbichao, 2014/05/22
			  : 1. ����PG_CNNT_IPV4_PeerFwd�������ͣ�����P2P�ڵ���ת��������
			  : 2. PG_INIT_CFG_S�ṹ����uAllowForward���ԣ���������P2P�ڵ�������ڵ�ת��������
			  :
			  : modify, chenbichao, 2014/05/30
			  : 1. PG_INIT_CFG_S�ṹ��uAllowForward���Ը�ΪuForwardSpeed����������ת���Ĵ������ơ�
			  :
			  : modify, chenbichao, 2014/10/29
			  : 1. ����PG_EVENT_LAN_SCAN�¼�����ʾ������P2P�ڵ����������ˡ�
			  : 2. ����PG_LAN_SCAN_S���ݽṹ��������ȡ������P2P�ڵ������Ľ����
			  : 3. ����pgLanScanStart()����������������ڵ�P2P�ڵ�����
			  : 4. ����pgLanScanResult()��������ȡ�������ڵ�P2P�ڵ������Ľ��
			  :
			  : modify, chenbichao, 2015/01/18
			  : 1. PG_INIT_CFG_S�ṹ��uSessTimeout���Ե�ȱʡֵ��Ϊ30��
			  :
			  : modify, chenbichao, 2015/03/30
			  : 1. ����PG_NET_MODE_Eѡ�����緽ʽö�١�
			  : 2. ����pgServerNetMode()�������л�����P2P�����������緽ʽ��
			  : 
			  : modify, chenbichao, 2015/04/21
			  : 1. ����PG_INFO_S�ṹ��szListenID���ԣ�������ȡ�����˵�ID��
			  : 2. �޸�PG_LAN_SCAN_S�ṹ��szPeerIDΪszListenID���������塣
			  : 3. ����pgConnected()�������жϻỰ������û����ɡ�
			  : 
*****************************************************************/
#ifndef _PG_LIB_CONNECT_H
#define _PG_LIB_CONNECT_H


#ifdef __cplusplus
extern "C" {
#endif


/**
 *  ����ģʽ���ͻ��˻�������
 */
typedef enum tagPG_MODE_E {
	PG_MODE_CLIENT,   // �ͻ���
	PG_MODE_LISTEN,   // �����ˣ�ͨ�����豸�ˣ�
	PG_MODE_BUTT,
} PG_MODE_E;


/**
 *  �����붨��
 */
typedef enum tagPG_ERROR_E {
	PG_ERROR_OK = 0,             // �ɹ�
	PG_ERROR_INIT = -1,          // û�е���pgInitialize()�����Ѿ�����pgCleanup()����ģ�顣
	PG_ERROR_CLOSE = -2,         // �Ự�Ѿ��رգ��Ự�Ѿ����ɻָ�����
	PG_ERROR_BADPARAM = -3,      // ���ݵĲ�������
	PG_ERROR_NOBUF = -4,         // �Ự���ͻ�����������
	PG_ERROR_NODATA = -5,        // �Ựû�����ݵ��
	PG_ERROR_NOSPACE = -6,       // ���ݵĽ��ջ�����̫С��
	PG_ERROR_TIMEOUT = -7,       // ������ʱ��
	PG_ERROR_BUSY = -8,          // ϵͳ��æ��
	PG_ERROR_NOLOGIN = -9,       // ��û�е�¼��P2P��������
	PG_ERROR_MAXSESS = -10,      // �Ự������
	PG_ERROR_NOCONNECT = -11,    // �Ự��û���������
	PG_ERROR_MAXINST = -12,      // ʵ��������
	PG_ERROR_SYSTEM = -127,      // ϵͳ����
} PG_ERROR_E;


/**
 *  pgEvent()�����ȴ����¼�����
 */
typedef enum tagPG_EVENT_E {
	PG_EVENT_NULL,               // NULL
	PG_EVENT_CONNECT,            // �Ự���ӳɹ��ˣ����Ե���pgWrite()�������ݡ�
	PG_EVENT_CLOSE,              // �Ự���Զ˹رգ���Ҫ����pgClose()���ܳ����ͷŻỰ��Դ��
	PG_EVENT_WRITE,              // �Ự�ĵײ㷢�ͻ������Ŀ��пռ������ˣ����Ե���pgWrite()���������ݡ�
	PG_EVENT_READ,               // �Ự�ĵײ���ջ����������ݵ�����Ե���pgRead()���������ݡ�
	PG_EVENT_OFFLINE,            // �Ự�ĶԶ˲������ˣ�����pgOpen()������Զ˲����ߣ����ϱ����¼���
	PG_EVENT_INFO,               // �Ự�����ӷ�ʽ��NAT���ͼ���б仯�ˣ����Ե���pgInfo()��ȡ���µ�������Ϣ��

	PG_EVENT_SVR_LOGIN = 16,     // ��¼��P2P�������ɹ������ߣ�
	PG_EVENT_SVR_LOGOUT,         // ��P2P������ע������ߣ����ߣ�
	PG_EVENT_SVR_REPLY,          // P2P������Ӧ���¼������Ե���pgServerReply()����Ӧ��
	PG_EVENT_SVR_NOTIFY,         // P2P�����������¼������Ե���pgServerNotify()�������͡�

	PG_EVENT_LAN_SCAN = 32,      // ɨ���������P2P�ڵ㷵���¼������Ե���pgLanScanResult()ȥ���ս����

} PG_EVENT_E;


/**
 *  ���ݷ���/�������ȼ�
 */
typedef enum tagPG_PRIORITY_E {
	PG_PRIORITY_0,         // ���ȼ�0, ������ȼ�����������ȼ��ϲ��ܷ���̫�����������ݣ���Ϊ���ܻ�Ӱ��P2Pģ�鱾�������ͨ�š���
	PG_PRIORITY_1,         // ���ȼ�1
	PG_PRIORITY_2,         // ���ȼ�2
	PG_PRIORITY_3,         // ���ȼ�3, ������ȼ�
	PG_PRIORITY_BUTT,
} PG_PRIORITY_E;


/**
 *  ͨ������������
 */
typedef enum tagPG_CNNT_E {
	PG_CNNT_Unknown = 0,            // δ֪����û�м�⵽��������

	PG_CNNT_IPV4_Pub = 4,           // ����IPv4��ַ
	PG_CNNT_IPV4_NATConeFull,       // ��ȫ׶��NAT
	PG_CNNT_IPV4_NATConeHost,       // ��������׶��NAT
	PG_CNNT_IPV4_NATConePort,       // �˿�����׶��NAT
	PG_CNNT_IPV4_NATSymmet,         // �Գ�NAT

	PG_CNNT_IPV4_Private = 12,      // ˽��ֱ��
	PG_CNNT_IPV4_NATLoop,           // ˽��NAT����

	PG_CNNT_IPV4_TunnelTCP = 16,    // TCPv4ת��
	PG_CNNT_IPV4_TunnelHTTP,        // HTTPv4ת��

	PG_CNNT_IPV4_PeerFwd = 24,      // ����P2P�ڵ�ת��

	PG_CNNT_IPV6_Pub = 32,          // ����IPv6��ַ

	PG_CNNT_IPV6_TunnelTCP = 40,    // TCPv6ת��
	PG_CNNT_IPV6_TunnelHTTP,        // HTTPv6ת��

	PG_CNNT_Offline = 0xffff,       // �Զ˲�����

} PG_CNNT_E;


/**
 * ѡ������ķ�ʽ
 */
typedef enum tagPG_NET_MODE_E {
	PG_NET_MODE_Auto,               // �Զ�ѡ������
	PG_NET_MODE_P2P,                // ֻʹ��P2P��͸
	PG_NET_MODE_Relay,              // ֻʹ��Relayת��
} PG_NET_MODE_E;


/**
 *  ��ʼ��������
 */
typedef struct tagPG_INIT_CFG_S {

	// 4�����ȼ��ķ��ͻ��������ȣ���λΪ��K�ֽڡ�
	// uBufSize[0] Ϊ���ȼ�0�ķ��ͻ��������ȣ���0��ʹ��ȱʡֵ��ȱʡֵΪ128K
	// uBufSize[1] Ϊ���ȼ�1�ķ��ͻ��������ȣ���0��ʹ��ȱʡֵ��ȱʡֵΪ128K
	// uBufSize[2] Ϊ���ȼ�2�ķ��ͻ��������ȣ���0��ʹ��ȱʡֵ��ȱʡֵΪ256K
	// uBufSize[3] Ϊ���ȼ�3�ķ��ͻ��������ȣ���0��ʹ��ȱʡֵ��ȱʡֵΪ256K
	// ��ʾ�����������ڴ治�ǳ�ʼ��ʱ�ͷ���ã�Ҫ�õ�ʱ��ŷ��䡣
	//       ���磬������256K������ǰֻʹ����16K����ֻ����16K���ڴ档
	//       ����������󣬷��͵����ݲ��ڻ��������������򻺳���ʵ��ʹ�õĳ��Ȳ���������
	unsigned int uBufSize[PG_PRIORITY_BUTT];

	// �Ự�������ӵĳ�ʱʱ�䡣����0��ʹ��ȱʡֵ��ȱʡֵΪ30�롣
	// �����ʱ���ڣ�������Ӳ��ɹ������ϱ�PG_EVENT_CLOSE�¼���
	unsigned int uSessTimeout;
									
	// ����P2P��͸��ʱ�䡣���ʱ�䵽���û�д�͸�����л���ת��ͨ�š�
	// (uTryP2PTimeout == 0)��ʹ��ȱʡֵ��ȱʡֵΪ6�롣
	// (uTryP2PTimeout > 0 && uTryP2PTimeout <= 3600)����ʱֵΪ������uTryP2PTimeout
	// (uTryP2PTimeout > 3600)������P2P��͸��ֱ����ת����
	unsigned int uTryP2PTimeout;

	// ��������P2P�ڵ�������ڵ�ת����������0��ת�����ʣ��ֽ�/�룩��0��������ת����
	// ��������: 32K (�ֽ�/��) ���ϡ�
	unsigned int uForwardSpeed;

	// ���ݳ�ʼ��������Ŀǰ��Androidϵͳ����Java VM��ָ�롣��
	// ��JNIģ����ʵ��JNI_Onload�ӿڣ���ȡ��Java VM��ָ�룬����pgInitialize()�����P2Pģ�顣
	unsigned int uParam;
								
} PG_INIT_CFG_S;


/**
 *  �Ự��Ϣ��
 */
typedef struct tagPG_INFO_S {
	char szPeerID[128];             // �Ự�Զ˵�P2P ID
	char szAddrPub[64];             // �Ự�Զ˵Ĺ���IP��ַ
	char szAddrPriv[64];            // �Ự�Զ˵�˽��IP��ַ
	char szListenID[128];           // �Ự�����˵�ID���ڿͻ��˵���pgInfo()ʱ��Ч����
	unsigned int uCnntType;         // �Ựͨ�����������ͣ�NAT���ͣ�����ö�١�PG_CNNT_E��
} PG_INFO_S;


/**
 * ������ɨ����
 */
typedef struct tagPG_LAN_SCAN_S {
	char szAddr[64];                // �����˵ľ�����IP��ַ
	char szListenID[128];           // �����˵�ID
} PG_LAN_SCAN_S;


/**
 *  ��־����ص�����
 *
 *  uLevel��[IN] ��־����
 *
 *  lpszOut��[IN] ��־�������
 */
typedef void (*TfnLogOut)(unsigned int uLevel, const char* lpszOut);


/**
 *  ������P2P��͸ģ���ʼ������
 *
 *  ������ʽ�����������������ء�
 *
 *  lpuInstID��[OUT] ʵ��ID��P2Pģ��֧�ֶ�ʵ������ʼ��ʱ����ʵ��ID��
 *
 *  uMode��[IN] ����ģʽ���ͻ��˻������ˣ���ö�١�PG_MODE_E��
 *
 *  lpszUser��[IN] �ͻ���ʱΪ�ʺ��û�����������ʱͨ��Ϊ�豸ID��
 *
 *  lpszPass��[IN] �ͻ���ʱΪ�ʺ�����
 *
 *  lpszSvrAddr��[IN] P2P�������ĵ�ַ�˿ڣ����磺��127.0.0.1:3333��
 * 
 *  lpszRelayList��[IN] �м̷�������ַ�б�P2P�޷���͸�������ͨ���м̷�����ת����
 *      ��ʽʾ����"type=0&load=0&addr=127.0.0.1:443;type=1&load=100&addr=192.168.0.1:8000"
 *      ÿ���м̷�������type��load��addr��������������м̷�����֮���÷ֺš�;��������
 * 
 *  lpstInitCfg��[IN] ��ʼ�����������ṹ��PG_INIT_CFG_S���Ķ��塣
 *
 *  pfnLogOut��[IN] ��־����ص�������ָ�롣�ص�����ԭ�ͼ���TfnLogOut�����塣
 *
 *  ����ֵ����ö�١�PG_ERROR_E���Ķ���
 */
int pgInitialize(unsigned int* lpuInstID, unsigned int uMode,
	const char* lpszUser, const char* lpszPass, const char* lpszSvrAddr,
	const char* lpszRelayList, PG_INIT_CFG_S *lpstInitCfg, TfnLogOut pfnLogOut);


/**
 *  ������P2P��͸ģ�������ͷ�������Դ��
 * 
 *  ������ʽ�����������������ء�
 *
 *  uInstID��[IN] ʵ��ID������pgInitialize()ʱ�����
 */
void pgCleanup(unsigned int uInstID);


/**
 *  ������������־����ļ���
 *
 *  ������ʽ�����������������ء�
 *
 *  uLevel��[IN] ��־�������
 *          0����Ҫ����־��Ϣ��Ĭ�Ͽ�������
 *          1�������϶൫��Ҫ����־��Ϣ��Ĭ�Ϲرգ���
 *
 *  uEnable��[IN] 0���رգ���0��������
 *
 *  ����ֵ������0Ϊ�ɹ���С��0Ϊ������
 */
int pgLevel(unsigned int uLevel, unsigned int uEnable);


/**
 *  ��������ȡʵ�����˵�P2P ID��
 * 
 *  ������ʽ�����������������ء�
 *
 *  uInstID��[IN] ʵ��ID������pgInitialize()ʱ�����
 *
 *  lpszSelfID��[OUT] ���ܱ���P2P ID�Ļ�������
 *
 *  uSize��[IN] ��lpszSelfID���������ĳ��ȣ���С��128�ֽڡ�
 *
 *  ����ֵ����ö�١�PG_ERROR_E���Ķ��塣
 */
int pgSelf(unsigned int uInstID, char* lpszSelfID, unsigned int uSize);


/**
 *  �������ȴ��ײ��¼��ĺ���������ͬʱ�ȴ�����Ự�ϵĶ����¼����¼������������ء�
 *
 *  ������ʽ�����������¼��ﵽ��ȴ���ʱ�󷵻ء�
 *
 *  uInstID��[IN] ʵ��ID������pgInitialize()ʱ�����
 *
 *  lpuEventNow��[OUT] �����ǰ�������¼�����ö�١�PG_EVENT_E������
 *
 *  lpuSessIDNow��[OUT] ��ǰ�����¼��ĻỰID��
 *      �ڡ�PG_EVENT_SVR_XXX���¼�ʱ���Դ˲�����
 *
 *  lpuPrio��[OUT] �����ǰ�����¼������ȼ�����ö�١�PG_PRIORITY_E�����塣
 *      �ڡ�PG_EVENT_SVR_XXX���¼�ʱ���Դ˲�����
 *
 *  uTimeout��[IN] �ȴ���ʱʱ��(����)����0Ϊ���ȴ����������ء�
 *
 *  ����ֵ����ö�١�PG_ERROR_E���Ķ��塣
 */
int pgEvent(unsigned int uInstID, unsigned int* lpuEventNow,
	unsigned int* lpuSessIDNow, unsigned int* lpuPrio, unsigned int uTimeout);


/**
 *  ����������һ���Ự����ͬʱ�������˷�����������ֻ���ڿͻ���ģʽ���ã���
 *
 *  ������ʽ�����������������ء�
 *      ���Ӳ���������ɣ���pgEvent()�յ�PG_EVENT_CONNECT�¼������Ӳųɹ���
 *      ���pgEvent()�յ�PG_EVENT_CLOSE�¼���˵�������޷��ɹ�����Ҫ����pgClose()�رջỰ��
 *
 *  uInstID��[IN] ʵ��ID������pgInitialize()ʱ�����
 *
 *  lpszListenID��[IN] �����˵�ID��Ҳ������������pgInitialize()�д���ġ�lpszUser���û�����
 *
 *  lpuSessID��[OUT] ����ỰID
 *
 *  ����ֵ����ö�١�PG_ERROR_E���Ķ��塣
 */
int pgOpen(unsigned int uInstID, const char* lpszListenID, unsigned int* lpuSessID);


/**
 *  ��������һ���Ự�Ϸ�������
 *
 *  ������ʽ�����������������ء�
 *
 *  uInstID��[IN] ʵ��ID������pgInitialize()ʱ�����
 *
 *  uSessID��[IN] �ỰID
 *
 *  lpvData��[IN] ���ݻ�������ַ
 *
 *  uDataLen��[IN] ���ݳ��ȡ�
 *      Ϊ��������ܣ�����һ�η��ͽϳ������ݣ�P2Pģ���ڲ��Ὣ���ݽ��з�Ƭ���䣬�ڽ��ն��ٰ�������ϻ�ԭ��
 *      ��һ�η��͵����ݳ��Ȳ��ܳ���P2Pģ���ڲ��ķ��ͻ������ĳ��ȣ��ο�PG_INIT_CFG_S�ṹ��uBufSize���Ե�˵����
 *
 *  uPriority��[IN] �������ȼ�����ö�١�PG_PRIORITY_E�����塣
 *
 *  ����ֵ������0Ϊ���͵����ݳ��ȡ�С��0Ϊ�����루��ö�١�PG_ERROR_E���Ķ��壩
 */
int pgWrite(unsigned int uInstID, unsigned int uSessID,
	const void* lpvData, unsigned int uDataLen, unsigned int uPriority);


/**
 *  ��������һ���Ự�Ͻ�������
 *
 *  ������ʽ�����������������ء�
 *
 *  uInstID��[IN] ʵ��ID������pgInitialize()ʱ�����
 *
 *  uSessID��[IN] �ỰID
 *
 *  lpvBuf��[OUT] ���ջ�������ַ
 *
 *  uBufLen��[IN] ���������ȡ�
 *      pgRead()һ�ν��յ����ݵ���pgWrite()һ�η��͵����ݡ�
 *      ���pgWrite()һ�η��͵����ݺܳ�������16K�ֽڣ�����P2Pģ���Զ������ݷ�Ƭ���д��䡣
 *      �ڽ��նˣ�P2Pģ��ѷ�Ƭ������������ϻ�ԭ����pgRead()һ�ζ���pgWrite()һ�η��͵����ݡ�
 *
 *  lpuPriority��[OUT] �������ȼ�����ö�١�PG_PRIORITY_E�����塣
 *
 *  ����ֵ������0Ϊ���յ����ݳ��ȡ�С��0Ϊ�����루��ö�١�PG_ERROR_E���Ķ��壩
 */
int pgRead(unsigned int uInstID, unsigned int uSessID,
	void* lpvBuf, unsigned int uBufLen, unsigned int* lpuPriority);


/**
 *  ��������ȡ���˵�ַ��Ϣ��һ���Ự�Զ˵ĵ�ַ��Ϣ��
 *        ��Ҫ���յ�PG_EVENT_INFO�¼��Ժ󣬲��ܳɹ���ȡ��Ч��Ϣ��
 *
 *  ������ʽ�����������������ء�
 *
 *  uInstID��[IN] ʵ��ID������pgInitialize()ʱ�����
 *
 *  uSessID��[IN] �ỰID��uSessIDΪ0ʱ����ȡ���˵���Ϣ��uSessIDΪ����0ʱ����ȡ�Զ˵���Ϣ��
 *
 *  lpstInfo��[OUT] ��ȡ���ĻỰ��Ϣ������PG_INFO_S���ṹ����
 *
 *  ����ֵ��0Ϊ�ɹ���С��0Ϊ�����루��ö�١�PG_ERROR_E���Ķ��壩
 */
int pgInfo(unsigned int uInstID, unsigned int uSessID, PG_INFO_S* lpstInfo);


/**
 *  ��������ȡһ���Ự�ķ��ͻ�����ʣ���֡����
 *
 *  ������ʽ�����������������ء�
 *
 *  uInstID��[IN] ʵ��ID������pgInitialize()ʱ�����
 *
 *  uSessID��[IN] �ỰID
 *
 *  uPriority��[IN] �������ȼ�����ö�١�PG_PRIORITY_E�����塣
 *
 *  ����ֵ�����ڵ���0Ϊ���ͻ�����ʣ���֡����С��0Ϊ������
 */
int pgPend(unsigned int uInstID, unsigned int uSessID, unsigned int uPriority);


/**
 *  �������ж�һ���Ự�Ƿ��Ѿ�������ɡ�
 *
 *  ������ʽ�����������������ء�
 *
 *  uInstID��[IN] ʵ��ID������pgInitialize()ʱ�����
 *
 *  uSessID��[IN] �ỰID
 *
 *  ����ֵ��0Ϊ�Ѿ����ӡ�С��0Ϊ�����루��ö�١�PG_ERROR_E���Ķ��壩
 */
int pgConnected(unsigned int uInstID, unsigned int uSessID);


/**
 *  �������ر�һ���Ự��
 *
 *  ������ʽ�����������������ء�
 *
 *  uInstID��[IN] ʵ��ID������pgInitialize()ʱ�����
 *
 *  uSessID��[IN] �ỰID
 */
void pgClose(unsigned int uInstID, unsigned int uSessID);


/**
 *  ��������P2P����������һ������
 *
 *  ������ʽ�����������������ء�
 *
 *  uInstID��[IN] ʵ��ID������pgInitialize()ʱ�����
 *
 *  lpszData��[IN] �����͵����ݣ������ַ�����
 *
 *  uParam��[IN] �Զ����������Ŀ����ʹ�����Ӧ���ܹ���ƥ�䣩��
 *
 *  ����ֵ��0Ϊ�ɹ���С��0Ϊ�����루��ö�١�PG_ERROR_E���Ķ��壩
 */
int pgServerRequest(unsigned int uInstID, const char* lpszData, unsigned int uParam);


/**
 *  ����������P2P��������Ӧ��
 *      pgEvent()�����յ���PG_EVENT_SVR_REPLY���¼�ʱ�����ô˺�������Ӧ�����ݡ�
 *
 *  ������ʽ�����������������ء�
 *
 *  uInstID��[IN] ʵ��ID������pgInitialize()ʱ�����
 *
 *  lpszData��[OUT] ����Ӧ�����ݵĻ����������P2P���������ص��ַ�����
 *
 *  uSize��[IN] ���ջ������ĳ���
 *
 *  lpuParam��[OUT] ������pgServerRequest()����������Զ������
 *
 *  ����ֵ�����ڵ���0Ϊ���ջ������е�Ӧ�����ݳ��ȡ�С��0Ϊ�����루��ö�١�PG_ERROR_E���Ķ��壩
 */
int pgServerReply(unsigned int uInstID, char* lpszData, unsigned int uSize, unsigned int* lpuParam);


/**
 *  ����������P2P���������������͡�
 *      pgEvent()�����յ���PG_EVENT_SVR_NOTIFY���¼�ʱ�����ô˺��������������ݡ�
 *
 *  ������ʽ�����������������ء�
 *
 *  uInstID��[IN] ʵ��ID������pgInitialize()ʱ�����
 *
 *  lpszData��[OUT] �����������ݵĻ����������P2P���������͵��ַ�����
 *
 *  uSize��[IN] ���ջ������ĳ���
 *
 *  ����ֵ�����ڵ���0Ϊ���ջ������е��������ݳ��ȡ�С��0Ϊ�����루��ö�١�PG_ERROR_E���Ķ��壩
 */
int pgServerNotify(unsigned int uInstID, char* lpszData, unsigned int uSize);


/**
 *  ������ָ������P2P�����������緽ʽ��
 *      ���ֻ�ϵͳ��ʹ��P2Pʱ������ֻ����ߣ��������л���
 *      ��ֻʹ��Relayת��(PG_NET_MODE_Relay)����ʽ���ӣ�����ǿ�ֻ�APP������״̬�µ�����������
 *
 *  ������ʽ�����������������ء�
 *
 *  uInstID��[IN] ʵ��ID������pgInitialize()ʱ�����
 *
 *  uMode��[IN] �������ӷ�ʽ������PG_NET_MODE_E��ö�١�
 *
 *  ����ֵ��0Ϊ�ɹ���С��0Ϊ�����루��ö�١�PG_ERROR_E���Ķ��壩
 */
int pgServerNetMode(unsigned int uInstID, unsigned int uMode);


/**
 *  ����������������ڵ�P2P�ڵ�������
 *
 *  ������ʽ�����������������ء�
 *
 *  uInstID��[IN] ʵ��ID������pgInitialize()ʱ�����
 *
 *  ����ֵ��0Ϊ�ɹ���С��0Ϊ�����루��ö�١�PG_ERROR_E���Ķ��壩
 */
int pgLanScanStart(unsigned int uInstID);


/**
 *  ��������ȡ������P2P�ڵ������Ľ����
 *        ���ϱ�PG_EVENT_LAN_SCAN�¼�֮����á�
 *
 *  ������ʽ�����������������ء�
 *
 *  uInstID��[IN] ʵ��ID������pgInitialize()ʱ�����
 *
 *  lpstLanList��[OUT] ���վ�����P2P�ڵ���Ϣ�����顣
 *
 *  uSize��[IN] ���ջ������ĳ���
 *
 *  ����ֵ�����ڵ���0Ϊ���ص�P2P�ڵ������С��0Ϊ�����루��ö�١�PG_ERROR_E���Ķ��壩
 */
int pgLanScanResult(unsigned int uInstID, PG_LAN_SCAN_S* lpstLanList, unsigned int uSize);


/**
 *  ��������ȡ��ģ��İ汾��
 *
 *  ������ʽ�����������������ء�
 *
 *  lpszVersion��[OUT] ���ܰ汾��Ϣ�Ļ�������
 *
 *  uSize��[IN] �������ĳ��ȣ�������ڵ���16�ֽڣ�
 */
void pgVersion(char* lpszVersion, unsigned int uSize);


#ifdef __cplusplus
}
#endif


#endif // _PG_LIB_CONNECT_H
