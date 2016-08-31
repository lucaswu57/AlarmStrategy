#ifndef __PLAYLIB_H__
#define __PLAYLIB_H__

#include "include/libavcodec/avcodec.h"


#ifdef __cplusplus
 extern "C"{
#endif

typedef enum eVideoFormat{
  VIDEO_FORMAT_UNKOWN = 0,
  VIDEO_FORMAT_H264 = 1,
  VIDEO_FORMAT_MJPEG = 2,
}VideoFormat;


typedef struct structVideoDecode {
    AVCodec*        mAVCodec;
    AVCodecContext* mAVCodecContext;
    AVFrame*        mAVFrame;
    AVPicture       mAVPicture;
   struct SwsContext*     mSwsContext;
} videodecode_t;



void ffmpeg_init();
void ffmpeg_done();

int video_began(int video_format, int image_width, int image_height, long *vd_handle);
void video_ended(long* vd_handle);
int video_decode(long vd_handle, uint8_t* in_264Buffer, int in_264BufferSize, uint8_t* out_RGB24Buffer, int* out_image_width, int* out_image_height,int* outsize);


void save_bmp(unsigned char * data,int data_size,int w,int h,FILE * out);
typedef  struct  tagBITMAPFILEHEADER
{
unsigned short int  bfType;       //位图文件的类型，必须为BM
unsigned long       bfSize;       //文件大小，以字节为单位
unsigned short int  bfReserved1; //位图文件保留字，必须为0
unsigned short int  bfReserved2; //位图文件保留字，必须为0
unsigned long       bfOffBits;  //位图文件头到数据的偏移量，以字节为单位
}BITMAPFILEHEADER;
typedef  struct  tagBITMAPINFOHEADER
{
long biSize;                        //该结构大小，字节为单位
long  biWidth;                     //图形宽度以象素为单位
long  biHeight;                     //图形高度以象素为单位
short int  biPlanes;               //目标设备的级别，必须为1
short int  biBitCount;             //颜色深度，每个象素所需要的位数
short int  biCompression;        //位图的压缩类型
long  biSizeImage;              //位图的大小，以字节为单位
long  biXPelsPerMeter;       //位图水平分辨率，每米像素数
long  biYPelsPerMeter;       //位图垂直分辨率，每米像素数
long  biClrUsed;            //位图实际使用的颜色表中的颜色数
long  biClrImportant;       //位图显示过程中重要的颜色数
}BITMAPINFOHEADER;
typedef  struct
{
BITMAPFILEHEADER  file; //文件信息区
BITMAPINFOHEADER  info; //图象信息区
}bmp;
#ifdef __cplusplus
 }
#endif
#endif
