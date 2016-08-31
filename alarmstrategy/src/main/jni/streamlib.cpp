#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "streamlib.h"

#define SANTACHI_I_FRAME_TYPE		(1)

#define DATA_SPACE					(10*256*1024)	// 默认空间大小
#define INDEX_FRAME_NUM  			DATA_SPACE/80   //(6*512)	/* 默认 index num */
#define ST_STREAM_MAX_FRAME_SIZE	(800*1024)		// 默认最大帧大小

#define  INITVALUE					-1

#define  	STREAM_SCUSSESS   0
#define   	STREAM_FAIL        -1

#define  	STREAM_PRINTF  printf

static stream_param_t g_stream_param;			//初始化数据
static stream_ch_t *g_pstream_ch;				//通道管理

int qh_stream_init_avstream(stream_param_t *p_stream_param)
{
    int i = 0;
    int j = 0;
    int k = 0;

    g_stream_param = *p_stream_param;

    if(g_stream_param.ch_num <= 0 || g_stream_param.ch_num > STREAM_MAX_CH_NUM)  
    {
        STREAM_PRINTF(" chu num %d err\n",g_stream_param.ch_num);
        return STREAM_FAIL;
    }


    /*后期加入参数判断*/
    /*................................*/


    printf("g_stream_param.ch_num = %d\n",g_stream_param.ch_num);// 3

    /*通道管理区*/
    g_pstream_ch  = (stream_ch_t *)malloc(sizeof(stream_ch_t) * g_stream_param.ch_num);
    if(g_pstream_ch  == NULL)
    {
        STREAM_PRINTF("g_pstream_ch malloc err!\n");
        return STREAM_FAIL;
    }

    memset(g_pstream_ch, 0, sizeof(stream_ch_t) * g_stream_param.ch_num);

    /*各通道参数*/
    for(i = 0; i < g_stream_param.ch_num; i++)
    {
        STREAM_PRINTF("------------------->init ch_no = %d\n",i);

        g_pstream_ch[i].total_space    = (g_stream_param.stream_ch_attr[i].data_space <= 0 ? DATA_SPACE : g_stream_param.stream_ch_attr[i].data_space); // 5MB 3MB 1MB
        // 索引个数根据具体应用需求设置，确保索引区循环覆盖幅度小于数据区，以免丢失数据
        g_pstream_ch[i].index_num      = (g_stream_param.stream_ch_attr[i].index_num <= 0 ? INDEX_FRAME_NUM : g_stream_param.stream_ch_attr[i].index_num); // 5K 3K 1K
        g_pstream_ch[i].max_frame_size = (g_stream_param.stream_ch_attr[i].max_frame_size <= 20*1024 ? ST_STREAM_MAX_FRAME_SIZE : g_stream_param.stream_ch_attr[i].max_frame_size);// 800KB 500KB 500KB

        g_pstream_ch[i].index_vail_end = INITVALUE;

        /*数据区*/
        g_pstream_ch[i].p_buf_data  = (char *)malloc(g_pstream_ch[i].total_space);
        if(g_pstream_ch[i].p_buf_data == NULL)
        {
            STREAM_PRINTF("p_buf_data malloc err!\n");
            goto 	ERR;
        }

        /* 最小安全块距离*/
        g_pstream_ch[i].block_safe_diff = g_pstream_ch[i].max_frame_size;

        /* 最小安全帧距离*/
        g_pstream_ch[i].frame_safe_num = 12;

        /* 取帧最大个数是帧数的 1/3 */
        g_pstream_ch[i].block_max_frame_num = g_pstream_ch[i].index_num / 3;

        STREAM_PRINTF("ch %d stream space %d index num %d  max frame size %d\n", i
                      , g_pstream_ch[i].total_space
                      , g_pstream_ch[i].index_num
                      , g_pstream_ch[i].max_frame_size);

        //printf("p_buf_data 0x%x\n",g_pstream_ch[i].p_buf_data);


        /*索引区*/
        g_pstream_ch[i].p_stream_index = (stream_index_t *)malloc(sizeof(*(g_pstream_ch[i].p_stream_index))* (g_pstream_ch[i].index_num));
        if(g_pstream_ch[i].p_stream_index == NULL)
        {
            STREAM_PRINTF("p_stream_index malloc err!\n");
            return STREAM_FAIL;
        }

        memset(g_pstream_ch[i].p_stream_index, 0
               , sizeof(*(g_pstream_ch[i].p_stream_index)) * (g_pstream_ch[i].index_num ));

        for(k = 0; k <  g_pstream_ch[i].index_num; k++)
        {
            g_pstream_ch[i].p_stream_index[k].data_circle_no = -3;
            g_pstream_ch[i].p_stream_index[k].frame_type = 0;
        }


    }

    return STREAM_SCUSSESS;

ERR:

    for(i = 0; i < g_stream_param.ch_num; i++)
    {
        if(g_pstream_ch[i].p_buf_data != NULL)
        {
            free(g_pstream_ch[i].p_buf_data);
        }

        if(g_pstream_ch[i].p_stream_index != NULL)
        {
            free(g_pstream_ch[i].p_stream_index);
        }
    }

    return STREAM_FAIL;

}

int qh_stream_destroy_avstream(void)
{
    int i = 0;

    for(i = 0; i < g_stream_param.ch_num; i++)
    {

        if(g_pstream_ch[i].p_buf_data != NULL)
        {
            free(g_pstream_ch[i].p_buf_data);
            g_pstream_ch[i].p_buf_data = NULL;
        }

        if(g_pstream_ch[i].p_stream_index != NULL)
        {
            free(g_pstream_ch[i].p_stream_index);
            g_pstream_ch[i].p_stream_index = NULL;
        }

    }

    if (g_pstream_ch != NULL)
    {
        free(g_pstream_ch);
        g_pstream_ch = NULL;
    }

    return STREAM_SCUSSESS;
}

//接收数据流存入指定的通道缓存区(实际写入内存g_pstream_ch中)。调用前需要将数据封装成统一的格式，数据格式为：帧头 + 帧数据。
int qh_stream_sendFrameToPool(stream_frame_t *frame)
{
    stream_ch_t *p_ch_stream = NULL;
    stream_index_t *p_cur_index = NULL;
    int cur_frame_len = 0;

    int ch = 0;
    int ch_type = 0;

    int data_start_pos = 0;
    int t_index = 0;

    if (frame == NULL)
    {
        return STREAM_FAIL;
    }

    ch = frame->ch;

    ch_type = frame->ch_type;

    p_ch_stream = (stream_ch_t *)&g_pstream_ch[ch];

	// 帧长: 帧头 + 帧数据
    cur_frame_len = sizeof(ptc_frame_head_t) + frame->frame_head.len;

    if(cur_frame_len >= p_ch_stream->max_frame_size)////帧数据太大则丢弃
    {
        STREAM_PRINTF("\033[0;36m ch %d video frame is %d too len  MAX frame size %d, and lost it \033[0m\n", ch, cur_frame_len,p_ch_stream->max_frame_size);
        return STREAM_FAIL;
    }

    p_cur_index = &p_ch_stream->p_stream_index[p_ch_stream->index_write_pos];

    if((p_ch_stream->total_space - p_ch_stream->data_write_pos) < cur_frame_len)//剩余可用存储空间小于当前帧，环加一，数据写位置置0，
    {
        p_ch_stream->buf_max_pos = p_ch_stream->data_write_pos;
        p_ch_stream->index_max_pos = p_ch_stream->index_vail_end;
        p_ch_stream->data_circle_no++; //当前帧位于数据区的环号
        p_ch_stream->data_write_pos = 0;
    }

    data_start_pos = p_ch_stream->data_write_pos;

    p_ch_stream->data_write_pos += cur_frame_len;

    p_cur_index->frame_pos = data_start_pos;

    ///把帧数据(帧头+帧数据)存储到偏移位置为data_start_pos的地方
#if 1
    memcpy(&p_ch_stream->p_buf_data[data_start_pos], (void*)&frame->frame_head, sizeof(ptc_frame_head_t));
    data_start_pos += sizeof(ptc_frame_head_t);
#endif
    memcpy(&p_ch_stream->p_buf_data[data_start_pos], frame->frame_data, frame->frame_head.len);

    p_cur_index->time = frame->frame_head.sec;
    p_cur_index->utime = frame->frame_head.usec;
    p_cur_index->frame_type = frame->frame_head.frame_type;

    p_cur_index->data_circle_no = p_ch_stream->data_circle_no;	// 当前帧位于数据区的环号
    p_cur_index->index_circle_no = p_ch_stream->index_circle_no; // 当前帧位于索引区的环号

    p_cur_index->frame_end = p_cur_index->frame_pos + cur_frame_len; // 当前帧在数据区的结束为止

    p_ch_stream->index_vail_end = p_ch_stream->index_write_pos;

    if((t_index = p_ch_stream->index_write_pos + 1) >= p_ch_stream->index_num)// 索引数超过了最大值
    {
        p_ch_stream->index_circle_no++; // 环加一 
    }

    p_ch_stream->index_write_pos = t_index % p_ch_stream->index_num;

    return STREAM_SCUSSESS;

}

/*返回当前最新的数据位置*/
int qh_stream_getLastPos(int ch, int ch_type, stream_rpos_t *rpos)
{
    stream_ch_t *p_ch_stream = NULL;
    stream_index_t *p_vail_index = NULL;
    int t_index = 0;

    p_ch_stream = &g_pstream_ch[ch];

    if(p_ch_stream->index_vail_end >= 0)
    {
        t_index = p_ch_stream->index_vail_end; // 有效帧位置
    }
    else
    {
        t_index = 0;
    }

    rpos->p_buf_data = p_ch_stream->p_buf_data;
    rpos->index_num = p_ch_stream->index_num;
    rpos->p_stream_index = p_ch_stream->p_stream_index;

    p_vail_index = &p_ch_stream->p_stream_index[t_index];

    rpos->data_start_pos = rpos->data_end_pos = p_vail_index->frame_pos;
    rpos->index_start_pos = rpos->index_end_pos = t_index;

    rpos->block_index_pos = t_index;
    rpos->block_index_count = 0;

    rpos->index_circle_no = p_vail_index->index_circle_no;
    rpos->data_circle_no = p_vail_index->data_circle_no;

    STREAM_PRINTF("data start %d index start %d data_circle %d index circle %d\n"
                  , rpos->data_start_pos
                  , rpos->index_start_pos
                  , rpos->data_circle_no
                  , rpos->index_circle_no);

    return STREAM_SCUSSESS;
}

/* 无调整返回0              有调整 1                无有效数据返回-1                  使用有效帧  */
static inline int change_frame_safe(int ch, int ch_type, int real_frame_diff, stream_rpos_t *rpos)
{
    stream_ch_t *p_ch_stream = NULL ;
    stream_index_t *p_vail_index = NULL;
    int data_circle, frame_circle, diff_data, diff_frame, surplus_frame;
    int t_index;

    p_ch_stream = &g_pstream_ch[ch];
    p_vail_index = &p_ch_stream->p_stream_index[p_ch_stream->index_vail_end];

    /* 读结束 = 有效数据的 结束 数据读空 */
    if(((p_vail_index->data_circle_no == rpos->data_circle_no)&& (p_vail_index->frame_end == rpos->data_end_pos))
            || p_ch_stream->index_vail_end < 0)
    {
        //  printf("====================>not buff data!\n");
        return STREAM_FAIL;
    }

    data_circle = p_vail_index->data_circle_no - rpos->data_circle_no;
    frame_circle = p_vail_index->index_circle_no - rpos->index_circle_no;

    diff_data = p_ch_stream->total_space - (data_circle * p_ch_stream->total_space + p_vail_index->frame_end - rpos->data_end_pos);

	//surplus_frame还有多少帧没读
    surplus_frame = frame_circle * p_ch_stream->index_num + p_ch_stream->index_vail_end - rpos->index_end_pos;
    diff_frame = p_ch_stream->index_num - surplus_frame;


    if((frame_circle > 1) || (data_circle > 1) || (diff_data < p_ch_stream->block_safe_diff)
            || (diff_frame < p_ch_stream->frame_safe_num))
    {
        /*小于安全距离调整*/
        t_index = (p_ch_stream->index_vail_end + p_ch_stream->frame_safe_num) % p_ch_stream->index_num;
        printf("\nt_index(%d) frame_safe_num(%d) index_vail_end(%d)\n",t_index,p_ch_stream->frame_safe_num,p_ch_stream->index_vail_end);
        while(1)
        {
            data_circle = p_vail_index->data_circle_no - p_ch_stream->p_stream_index[t_index].data_circle_no;

            diff_data = p_ch_stream->total_space - (data_circle * p_ch_stream->total_space +
                                                    p_vail_index->frame_end - p_ch_stream->p_stream_index[t_index].frame_pos);

            if((data_circle > 1) || (diff_data < p_ch_stream->block_safe_diff)
                    || (p_ch_stream->p_stream_index[t_index].frame_type != SANTACHI_I_FRAME_TYPE))
            {
                if(t_index == p_ch_stream->index_vail_end)
                {
                    STREAM_PRINTF("can not change to safe diff i frame \n");
                    return -1;
                }

                t_index = (t_index+1) % p_ch_stream->index_num;
                continue;
            }

            rpos->data_end_pos = rpos->data_start_pos = p_ch_stream->p_stream_index[t_index].frame_pos;
            rpos->data_circle_no = p_ch_stream->p_stream_index[t_index].data_circle_no;
            rpos->index_start_pos = rpos->index_end_pos = t_index;
            rpos->index_circle_no = p_ch_stream->p_stream_index[t_index].index_circle_no;
            STREAM_PRINTF("\033[0;36m net  lose  data [%d]\033[0;39m\n",ch);
            return 1;
        }
    }
    else if(real_frame_diff && (surplus_frame > real_frame_diff))
    {
        /*加速读取*/
        t_index = (rpos->index_end_pos + 1) % p_ch_stream->index_num;

        while(1)
        {
            if(p_ch_stream->p_stream_index[t_index].frame_type != SANTACHI_I_FRAME_TYPE)
            {
                if(t_index == p_ch_stream->index_vail_end)
                {
                    return 0;
                }

                t_index = (t_index + 1) % p_ch_stream->index_num;
                continue;
            }

            rpos->data_end_pos = rpos->data_start_pos = p_ch_stream->p_stream_index[t_index].frame_pos;
            rpos->data_circle_no = p_ch_stream->p_stream_index[t_index].data_circle_no;
            rpos->index_start_pos = rpos->index_end_pos = t_index;
            rpos->index_circle_no = p_ch_stream->p_stream_index[t_index].index_circle_no;

            STREAM_PRINTF("\033[0;36m net adjust lose  data[%d] \033[0;39m\n",ch);
            return 1;
        }
    }
    else
    {
        return 0;
    }
}

int qh_stream_getOneFrame(int ch, int ch_type, int real_frame_diff, stream_rpos_t *rpos)
{
    stream_ch_t *p_ch_stream = &g_pstream_ch[ch];

    stream_index_t *p_cur_index = NULL;

    if(change_frame_safe(ch, ch_type, real_frame_diff, rpos) < 0)
    {
        return 0;
    }

    /*是否首次预取或调整(避免丢失当前数据)*/
    if(rpos->data_end_pos != rpos->data_start_pos)
    {
        rpos->index_start_pos = (rpos->index_start_pos + 1) % p_ch_stream->index_num;
    }

    p_cur_index = &p_ch_stream->p_stream_index[rpos->index_start_pos];

    rpos->data_start_pos = p_cur_index->frame_pos;
    rpos->data_end_pos =  p_cur_index->frame_end;
    rpos->index_circle_no = p_cur_index->index_circle_no;
    rpos->data_circle_no = p_cur_index->data_circle_no;
    rpos->index_end_pos = rpos->index_start_pos;
    rpos->p_buf_data = p_ch_stream->p_buf_data;

    return p_cur_index->frame_end - p_cur_index->frame_pos;
}

char * qh_getOneFrame(int ch, stream_rpos_t *p_netstream_rpos)
{
    int start_data;
	
    if(qh_stream_getOneFrame(ch,0,30,p_netstream_rpos) > 0)
    {
        start_data = p_netstream_rpos->data_start_pos;
        return &p_netstream_rpos->p_buf_data[start_data];
    }

    return NULL;
}
