#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "streamlib.h"
#include "P2PListen.h"
#include "UvcCamera.h"
#include "cJSON.h"
#include "HDCamera.h"

typedef struct
{
	pthread_t pid;
	int stream_type;
	int video_state;
	stream_callback callback;
}video_info_t;

static int g_init = 0;
static char g_session_id[64];
static unsigned long g_session_handler = 0;
static int stream = 0;
static int preview_flag = 0;
static p2pCallback g_p2p_callback;

static void* preview_thread(void* param)
{
	pthread_detach(pthread_self());
	
	int first = 1;
	ptc_frame_head_t frame_head;
	ptc_frame_video_t pVideo;
	char *frame_pos;
	stream_rpos_t rpos;
	int send_IFrameflag = 1;
	
	while(g_init)
	{
		if(preview_flag == 0)
		{
			first = 1;
			usleep(1000*5);
			continue;
		}
		
		if(first)
		{
			qh_stream_getLastPos(stream, 0, &rpos);
			first = 0;
		}

		if((frame_pos = qh_getOneFrame(stream, &rpos)) == NULL)
		{
			//printf("%s --> %s --> L%d : qh_getOneFrame return NULL \n", __FILE__, __FUNCTION__, __LINE__);
			usleep(1000*5);
			continue;
		}
		
		memcpy(&frame_head, frame_pos, sizeof(ptc_frame_head_t));// ???
		
		memset(&pVideo, 0, sizeof(ptc_frame_video_t));
		memcpy(&pVideo.frame_head, &frame_head, sizeof(ptc_frame_head_t));
		memcpy(pVideo.frame_body, frame_pos+sizeof(ptc_frame_head_t), frame_head.len);
		
		int left_size = frame_head.len + sizeof(ptc_frame_head_t);
		if(left_size > STREAM_MAX_LEN)
		{
			//printf("%s --> %s --> L%d : sget tcp net frame too len = %ld \n", __FILE__, __FUNCTION__, __LINE__, frame_head.len);
			left_size = 300*1024;
			continue;
		}

		if(g_session_handler)
		{
			int nRet = P2P_SendStream(g_session_handler, (void*)&pVideo, left_size);
			if (nRet < PG_ERROR_OK)
			{
				int times = 1;
				while(PG_ERROR_NOBUF == nRet || PG_ERROR_NOCONNECT == nRet)
				{
					if(times > 5)
					{
						break;
					}
					usleep(1000*5);
					nRet = P2P_SendStream(g_session_handler, (void*)&pVideo, left_size);
					times ++;
				}

				if(PG_ERROR_NOBUF == nRet || PG_ERROR_NOCONNECT == nRet)
				{
					//printf("%s --> %s --> L%d : pgWrite error : %d \n", __FILE__, __FUNCTION__, __LINE__, nRet);
				}
				else if(nRet < PG_ERROR_OK)
				{
					//printf("%s --> %s --> L%d : pgWrite error : %d \n", __FILE__, __FUNCTION__, __LINE__, nRet);
					break;
				}
				else
				{
					//printf("**************************************************\n");
				}
			}
		}
	}
	
	printf("close preview_thread\n");
	return NULL;
}

static int p2p_callback(unsigned long handler,int command, char* param)
{
	if(handler != g_session_handler)
	{
		return 0;
	}
	
	switch(command)
	{
		case PTC_CMD_START_PREVIEW:
		{
			cJSON *jsonroot = cJSON_Parse((const char*)param);
			if(jsonroot)
			{
				cJSON *json_stream = cJSON_GetObjectItem( jsonroot, "ch_type");
				if(json_stream)
				{
					stream = json_stream->valueint;
					preview_flag = 1;
				}
			}
			break;
		}
		case PTC_CMD_STOP_PREVIEW:
		case PTC_CMD_CLIENT_NOT_CONNECT:
		case PTC_CMD_CLIENT_TIMEOUT:
		case PTC_CMD_CLIENT_CLOSED:
		case PTC_CMD_SVR_LOGOUT:
		{
			preview_flag = 0;
			if(g_p2p_callback)
			{
				g_p2p_callback(0, command, NULL);
			}
			break;
		}
		default:
			break;
	}
	return 0;
}

int HD_Init(char* device_id, p2pCallback callback, offline_callback callback2)
{
	if(device_id == NULL || strlen(device_id) != 32)
	{
		return -1;
	}
	if(g_init == 1)
	{
		return 0;
	}
	
	g_init = 1;
	int ret = qh_cam_open(callback2);
	if(ret != 0)
	{
		g_init = 0;
		return -1;
	}
	
	ret = P2P_Init(p2p_callback);
	if(ret != 0)
	{
		g_init = 0;
		qh_cam_close();
		return -1;
	}
	
	pthread_t id;
	ret = pthread_create(&id,NULL, preview_thread, NULL);
	if(ret != 0)
	{
		g_init = 0;
		qh_cam_close();
		return -1;
	}
	
	memset(g_session_id, 0, 64);
	sprintf(g_session_id, "%s0", device_id);
	g_p2p_callback = callback;
	
	return 0;
}

void HD_CleanUp()
{
	g_init = 0;
	qh_cam_close();
}

int HD_GetIdarlingState()
{
	return qh_cam_status();
}

int HD_SetAlarm(int alarm_type, alarm_param_t alarm_data)
{
	return qh_cam_set_alarm(alarm_type, alarm_data);
}

int HD_GetAlram(int alarm_type, alarm_param_t* p_param_out)
{
	return qh_cam_get_alram(alarm_type, p_param_out);
}

int HD_SetTime(time_t utcSec, int zone, int dstTm)
{
	return qh_cam_set_localtime(utcSec, zone, dstTm);
}

int HD_GetTime(localTime_config_t* p_time_out)
{
	return qh_cam_get_localtime(p_time_out);
}

int HD_GetVersion(version_info_t* p_version_out)
{
	return qh_cam_get_version(p_version_out);
}

void HD_SetAlarmCallback(alarm_callback callback)
{
	set_alarm_callback(callback);
}

static void* video_thread(void* param)
{
	video_info_t* pInfo = (video_info_t*)param;
	if(pInfo)
	{
		int first = 1;
		char *frame_pos;
		stream_rpos_t rpos;
		int send_IFrameflag = 1;
		
		while(pInfo->video_state)
		{			
			if(first)
			{
				qh_stream_getLastPos(pInfo->stream_type, 0, &rpos);
				first = 0;
			}

			if((frame_pos = qh_getOneFrame(pInfo->stream_type, &rpos)) == NULL)
			{
				//printf("%s --> %s --> L%d : qh_getOneFrame return NULL \n", __FILE__, __FUNCTION__, __LINE__);
				usleep(1000*5);
				continue;
			}
			
			ptc_frame_video_t* pVideo = (ptc_frame_video_t*)frame_pos;
			
			if(send_IFrameflag == 1)	//��һ֡��Ҫ����I֡
			{
				if ((pVideo->frame_head.frame_type != 1)&&(pVideo->frame_head.frame_type != 6))
				{
					printf("%s --> %s --> L%d : send_IFrameflag == 1 \n", __FILE__, __FUNCTION__, __LINE__);
					continue;
				}
				send_IFrameflag = 0;
			}

			if(pInfo->callback)
			{
				int len = pVideo->frame_head.len + sizeof(ptc_frame_head_t);
				pInfo->callback((unsigned long)pInfo, pVideo, len);
			}
		}
	}
	
	printf("close preview_thread\n");
	return NULL;
}

unsigned long HD_StartVideo(int stream_type, stream_callback callback)
{
	video_info_t* pInfo = (video_info_t*)malloc(sizeof(video_info_t));
	if(pInfo)
	{
		memset(pInfo, 0, sizeof(video_info_t));
		pInfo->stream_type = stream_type;
		pInfo->callback = callback;
		pInfo->video_state = 1;
		if(pthread_create(&pInfo->pid, NULL, video_thread, (void*)pInfo) == 0)
		{
			return (unsigned long)pInfo;
		}
		
		free(pInfo);
		return 0;
	}
	return 0;
}

void HD_StopVideo(unsigned long handler)
{
	video_info_t* pInfo = (video_info_t*)handler;
	if(pInfo)
	{
		pInfo->video_state = 0;
		pthread_join(pInfo->pid, NULL);
		free(pInfo);
	}
}

char* HD_CreateSession(char* p2p_ip, int p2p_port)
{
	if(qh_cam_status() != 1)
	{
		return NULL;
	}
	if(g_session_handler == 0)
	{
		g_session_handler = P2P_CreateSession(g_session_id, p2p_ip, p2p_port);
		if(g_session_handler == 0)
		{
			return NULL;
		}
		return g_session_id;
	}
	return NULL;
}

void HD_CloseSession()
{
	preview_flag = 0; 
	if(g_session_handler != 0)
	{
		P2P_CloseSession(g_session_handler);
		g_session_handler = 0;
	}
	g_session_handler = 0;
}

int HD_UpdateIdarling(long size, char* buf, upgrade_callback callback)
{
	return qh_cam_set_update(size, buf, callback);
}
