#ifndef _HDDEFINE_H_#define _HDDEFINE_H_#define ERROR_INPUT_ERROR					1		// 输入参数错误#define PG_ERROR_OK							0		// 成功#define PG_ERROR_INIT						-1		// 没有调用pgInitialize()或者已经调用pgCleanup()清理模块。#define PG_ERROR_CLOSE						-2		// 会话已经关闭（会话已经不可恢复）。#define PG_ERROR_BADPARAM					-3		// 传递的参数错误。#define PG_ERROR_NOBUF						-4		// 会话发送缓冲区已满。#define PG_ERROR_NODATA						-5		// 会话没有数据到达。#define PG_ERROR_NOSPACE					-6		// 传递的接收缓冲区太小。#define PG_ERROR_TIMEOUT					-7		// 操作超时。#define PG_ERROR_BUSY						-8		// 系统正忙。#define PG_ERROR_NOLOGIN					-9		// 还没有登录到P2P服务器。#define PG_ERROR_MAXSESS					-10		// 会话数限制#define PG_ERROR_NOCONNECT					-11		// 会话还没有连接完成#define PG_ERROR_MAXINST					-12		// 实例数限制#define PG_ERROR_SYSTEM						-127	// 系统错误。#define TAG "xushibo"#include <android/log.h>#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)#define printf LOGI#define	AMBAR_VERSION_LEN					24typedef struct __localTime_config_t{	time_t 		utcSec;	int			zone; 	//秒	int 		dstTm; 	//小时	int			reserve;}localTime_config_t;typedef struct _alarm_piont_t{	float x;	float y;}alarm_piont_t;typedef struct _alarm_param_t{	int enable;					//0-关，1-开	alarm_piont_t right_down;	alarm_piont_t left_down;	alarm_piont_t left_up;	alarm_piont_t right_up;	int drt;					//0-双向,1-向右,2-向左(越界报警有效，其他默认为0)}alarm_param_t;typedef struct  __grain_version_info_t{    char	version_number[AMBAR_VERSION_LEN];		/* 版本号 */    char	device_type[AMBAR_VERSION_LEN];			/* 产品型号*/    char	version_date[AMBAR_VERSION_LEN];		/* 版本日期 */    unsigned int server_type;    unsigned char	is_support_d1:1;				/* D1 0: 不支持, 1:支持 */    unsigned char	is_support_wifi:1;				/* 无线 0: 不支持, 1:支持*/    unsigned char   ptz_type:3;						/* 1: santachi_in, ......*/    unsigned char   reserve_1:3;    char sub_type;    char unusd[2];    char device_serial_no[32];						/* 产品序列号 */    char leave_factory_date[AMBAR_VERSION_LEN];		/* 出厂日期 */    char kernel_version[AMBAR_VERSION_LEN];    char onvif_version[AMBAR_VERSION_LEN];}version_info_t;typedef enum _DEV_CMD_E{	PTC_CMD_START_PREVIEW			= 0x00212000,	//预览	PTC_CMD_STOP_PREVIEW			= 0x00212003,	//停止预览	PTC_CMD_HEARTBEATS				= 0x00212002,	//心跳	PTC_CMD_CLIENT_NOT_CONNECT		= 0x00000001,	//客户端未连接	PTC_CMD_CLIENT_TIMEOUT			= 0x00000002,	//客户端心跳超时	PTC_CMD_CLIENT_CLOSED			= 0x00000003,	//对端关闭或Session断开	PTC_CMD_SVR_LOGOUT				= 0x00000004,	//从P2P服务器注销或掉线}DEV_CMD_E;typedef void (*alarm_callback)(int type, int subtype, int opt, void* data, int len);typedef void (*stream_callback)(unsigned long handler, void* data, int len);typedef int (*p2pCallback)(unsigned long handler,int command, char* param);typedef int (*upgrade_callback)(int send, int total);typedef int (*offline_callback)(int offline);  //1 掉线#endif//_HDDEFINE_H_