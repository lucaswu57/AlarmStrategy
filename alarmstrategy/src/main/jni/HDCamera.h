#ifndef _HDCAMERA_H_
#define _HDCAMERA_H_

#include "HDDefine.h"

#ifdef __cplusplus
extern "C"{
#endif

//初始化、反初始化
int HD_Init(char* device_id, p2pCallback callback, offline_callback callback2);
void HD_CleanUp();
//获取idarling状态 返回值:0-未连接,1-已连接
int HD_GetIdarlingState();
//获取、设置报警参数 alarm_type:1-遮挡，2-入侵，3-越界 返回成功失败
int HD_SetAlarm(int alarm_type, alarm_param_t alarm_data);
int HD_GetAlram(int alarm_type, alarm_param_t* p_param_out);
//获取、设置时间
int HD_SetTime(time_t utcSec, int zone, int dstTm);//utcSec:utc秒数,zone:时区秒数(例:东八区28800),dstTm:夏令时
int HD_GetTime(localTime_config_t* p_time_out);
//获取版本号
int HD_GetVersion(version_info_t* p_version_out);
//设置报警回调
void HD_SetAlarmCallback(alarm_callback callback);
//升级idarling
int HD_UpdateIdarling(long size, char* buf, upgrade_callback callback); //0:升级成功 非0失败
//预览视频
unsigned long HD_StartVideo(int stream_type, stream_callback callback);
void HD_StopVideo(unsigned long handler);
//p2p
char* HD_CreateSession(char* p2p_ip, int p2p_port);
void HD_CloseSession();

#ifdef __cplusplus
}
#endif

#endif//_HDCAMERA_H_