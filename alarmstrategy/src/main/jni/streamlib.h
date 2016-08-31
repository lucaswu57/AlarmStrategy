#ifndef __STREAMLIB_H__
#define __STREAMLIB_H__

#include <pthread.h>

#define STREAM_MAX_CH_NUM 2			//通道数量2 0:主码流，1-子码流

#define STREAM_MAX_LEN (800 * 1024)
#define PTC_VIDEO_FRAME_SIZE				(255*1024)

typedef struct
{
	unsigned int len;
	unsigned int no;
	unsigned int sec;
	unsigned int usec;
	unsigned short width;
	unsigned short height;
	unsigned char frame_type;			// 1=I, 2=P, 3=A, 255=录像结束帧 PTC_FRAME_E
	unsigned char frame_sub_type;		//视频帧音频帧分别对应子类型: PTC_VIDEO_ENCODE_E PTC_AUDIO_ENCODE_E
	unsigned char frame_rate;
	unsigned char security;
	unsigned char padding_bytes;
	unsigned char channel_type;			//通道类型 PTC_CH_TYPE_E
	unsigned char channel_no;			//从零开始
	unsigned char reserve[1];
	unsigned int flag;					//PTC_FRAME_FLAG
}ptc_frame_head_t;

typedef struct
{
	ptc_frame_head_t frame_head;
	unsigned char frame_body[PTC_VIDEO_FRAME_SIZE];
}ptc_frame_video_t;

typedef struct __stream_index_t	//流索引结构体
{
	char frame_type;		/* i帧 p帧 音频 GPS */
	int  time;
	int  utime;
	int frame_pos; 			/* 帧在buf中的位置 */
	int frame_end;			/* frame len  = frame head + size */ /* cp前面 */
	int data_circle_no;  	/* 当前帧是buf的第几环上的数据 */
	int index_circle_no;
}stream_index_t;

typedef struct __stream_rpos_t  // 流buf读操作控制
{
	int index_start_pos; /* 取帧索引开始位置 */
	int index_end_pos;  /* 取帧索引 结束帧位置*/
	int index_circle_no; /* 环数开始帧的环数 不跨环 */
	int index_num;
	int block_index_pos;
	int block_index_count;
	stream_index_t *p_stream_index; /* 索引地址 */
	int data_start_pos;
	int data_end_pos;
	int data_circle_no;
	char *p_buf_data; /* 数据区地址 */
}stream_rpos_t;

typedef struct
{
	int ch;
	int ch_type;
	ptc_frame_head_t frame_head;
	unsigned char *frame_data;
}stream_frame_t;

typedef struct __stream_ch_attr_t
{
	int max_frame_size;       /* 最大帧数据长度 */
	int data_space;	        /*数据存储区长度*/
	int index_num;             /*索引数量*/
}stream_ch_attr_t;

typedef struct __stream_param_t
{
	int ch_num;					/* 设备通道数 包括GPS 单独算通道优化 */
	stream_ch_attr_t stream_ch_attr[STREAM_MAX_CH_NUM];
}stream_param_t;

typedef struct __stream_ch_t
{
	int index_vail_end;    /* 有效帧位置 */

	int index_write_pos;   /* 控制 流buf 写操作 */
	int index_circle_no;

	int data_write_pos;
	int data_circle_no;

	int max_frame_size;	/* 每个通道根据不同的需求设置不同的大小 */

	int buf_max_pos;	/* data buf 调头位置 */
	int index_max_pos;	/* data buf调头时的帧位置 */

	int block_safe_diff;	/* 取块操作预留预防覆盖发生 */

	int total_space;	/* 当前buf空间大小 */
	char *p_buf_data;

	int frame_safe_num;
	int block_max_frame_num; /* index_num/3 */

	int index_num;		/* index 个数*/
	stream_index_t *p_stream_index;	/* 按照计算得到的index个数分配空间 */

	pthread_mutex_t buf_mutex;
}stream_ch_t;

#ifdef __cplusplus
extern "C"
{
#endif

int qh_stream_init_avstream(stream_param_t *p_stream_param);
int qh_stream_destroy_avstream(void);
int qh_stream_sendFrameToPool(stream_frame_t *frame);
int qh_stream_getLastPos(int ch, int ch_type, stream_rpos_t *rpos);
int qh_stream_getOneFrame(int ch, int ch_type, int real_frame_diff, stream_rpos_t *rpos);
char* qh_getOneFrame(int ch, stream_rpos_t *p_netstream_rpos);

#ifdef __cplusplus
}
#endif

#endif //__STREAMLIB_H__