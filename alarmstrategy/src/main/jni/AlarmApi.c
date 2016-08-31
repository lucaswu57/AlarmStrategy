//
// Created by admin on 2016/7/4.
//
#include <string.h>
#include <stdio.h>
#include "com_qihancloud_alarmstrategy_AlarmApi.h"
#include "cJSON.h"
#include "time.h"
#include "HDDefine.h"
#include "HDCamera.h"
#include "streamlib.h"
#include "UvcCamera.h"
#include "P2PListen.h"
#include "avilib.h"
#include "playlib.h"

#define TAG "57"
#include <android/log.h>
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__) // 定义LOGF类型

#ifdef __cplusplus
extern "C" {
#endif

#define VIDEO_BUF_LEN 2*1024*1024//视频帧缓冲区大小


static JavaVM *g_jvm = 0;
static jobject g_obj = 0;
static  jclass  native_proxy_cls;
static int recordflag = 0;
static int recordstart = 0;//是否正在写文件
static int firstFrame = 0;  //录制时是否为第一帧
avi_t *out_fd;      //avilib库中定义的文件数据结构
static const char* dir_name = "/storage/emulated/0/Movies/record/";
char filename[128];
static int captureflag = 0;

static int show_firstFrame = 0;
static int videoFrameNum = 0;


/*
static int frame_no = -1;
static int interrupt = 0;
*/


/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    Init
 * Signature: (Ljava/lang/String;)I
 */

void p2p_callBack(unsigned long handler,int command, char* param);
int offline_callBack(int offline);
void ala_callback(int type, int subtype, int opt, void* data, int len);
void streamCallback(unsigned long handler, void* data, int len);
//void record_streamCallback(unsigned long handler, void* data, int len);
void stopRecord();
int upgrade_callBack(int send, int total);

 //int HD_Init(char* device_id, p2pCallback callback);
JNIEXPORT jint JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_Init(JNIEnv *env, jobject obj, jstring device_id){
   if(g_jvm == 0)
            {
               (*env)->GetJavaVM(env,&g_jvm);
            }
    if(g_obj == 0)
             {
               g_obj = (*env)->NewGlobalRef(env, obj);
             }
    jclass temp_native_proxy_cls=(*env)->GetObjectClass(env, obj);
    native_proxy_cls=(*env)->NewGlobalRef(env, temp_native_proxy_cls);

   char *_device_id=(*env)->GetStringUTFChars(env, device_id, NULL);
    if(_device_id == NULL)
        {
            (*env)->ReleaseStringUTFChars(env,device_id,_device_id);
        }
  int result = (int)HD_Init(_device_id,p2p_callBack,offline_callBack);
  ffmpeg_init();
  return result;

  }

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    CleanUp
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_CleanUp
  (JNIEnv *env, jobject obj){
        HD_CleanUp();
  }

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    GetIdarlingState
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_GetIdarlingState
  (JNIEnv *env, jobject obj){
  int result = (int)HD_GetIdarlingState();//1开启，0-没开启
  return result;
  }

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    SetAlarm
 * Signature: (ILcom/qihancloud/alarmstrategy/alarm/bean/PointInfoList;)I
 */
JNIEXPORT jint JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_SetAlarm
  (JNIEnv *env, jobject obj, jint type,jint status,jobject alarm_data,jint drt){
        int _type = (int) type;
       /*char *_alarm_data = (*env)->GetStringUTFChars(env, alarm_data, 0);
       cJSON *alarm_data_json = cJSON_Parse(_alarm_data);
       cJSON *enable = cJSON_GetObjectItem(alarm_data_json, "enable");
       cJSON *rdx = cJSON_GetObjectItem(alarm_data_json, "rdx");
       cJSON *rdy = cJSON_GetObjectItem(alarm_data_json, "rdy");
       cJSON *ldx = cJSON_GetObjectItem(alarm_data_json, "ldx");
       cJSON *ldy = cJSON_GetObjectItem(alarm_data_json,"ldy");
       cJSON *lux = cJSON_GetObjectItem(alarm_data_json, "lux");
       cJSON *luy = cJSON_GetObjectItem(alarm_data_json, "luy");
       cJSON *rux = cJSON_GetObjectItem(alarm_data_json, "rux");
       cJSON *ruy = cJSON_GetObjectItem(alarm_data_json, "ruy");
       cJSON *drt = cJSON_GetObjectItem(alarm_data_json, "drt");
       int _enable = enable->valueint;
       float _rdx=rdx->valuedouble;
       float _rdy=rdy->valuedouble;
       float _ldx=ldx->valuedouble;
       float _ldy=ldy->valuedouble;
       float _lux=lux->valuedouble;
       float _luy=luy->valuedouble;
       float _rux=rux->valuedouble;
       float _ruy=ruy->valuedouble;
       int _drt=drt->valueint;
       alarm_param_t time;
       time.enable=_enable;
       alarm_piont_t rd;
       rd.x=_rdx;
       rd.y=_rdy;
       time.right_down=rd;
       alarm_piont_t ld;
       ld.x=_ldx;
       ld.y=_ldy;
       time.left_down=ld;
       alarm_piont_t lu;
       lu.x=_lux;
       lu.y=_luy;
       time.left_up=lu;
       alarm_piont_t ru;
       ru.x=_rux;
       ru.y=_ruy;
       time.right_up=ru;
       time.drt=_drt;  */


           alarm_param_t alarmCfg;
           memset(&alarmCfg,0,sizeof(alarm_param_t));

           if((jint)type == 1)
           {
               alarmCfg.enable = (jint)status;
           }
           else
           {
                if((jint)type == 3){
                    alarmCfg.drt = (jint)drt;
                }


               jclass list_cls = (*env)->GetObjectClass(env,alarm_data);
               if(list_cls == NULL)
               {
                   LOGI("GetObjectClass");
                   return -99;
               }

               jmethodID list_get = (*env)->GetMethodID(env,list_cls,"get","(I)Ljava/lang/Object;");
               jmethodID list_size = (*env)->GetMethodID(env,list_cls,"size","()I");

               int len =  (*env)->CallIntMethod(env,alarm_data,list_size);
               LOGE("len = %d",len);

               if(len>0)
                 alarmCfg.enable = 1;

               int i=0;
               for(i=0;i<len;i++)
               {
                   jobject cfg_obj = (*env)->CallObjectMethod(env,alarm_data,list_get,i);
                   jclass cfg_cls = (*env)->GetObjectClass(env,cfg_obj);

                   jmethodID cfg_getx = (*env)->GetMethodID(env,cfg_cls,"getPercentX","()F");
                   jmethodID cfg_gety = (*env)->GetMethodID(env,cfg_cls,"getPercentY","()F");

                   jfloat x = (*env)->CallFloatMethod(env,cfg_obj,cfg_getx);
                   jfloat y = (*env)->CallFloatMethod(env,cfg_obj,cfg_gety);

                   if(i == 0)
                   {
                       alarmCfg.left_up.x = x;
                       alarmCfg.left_up.y = y;

                   }
                   else if(i == 1)
                   {

                       alarmCfg.right_up.x = x;
                       alarmCfg.right_up.y = y;

                   }
                   else if(i == 2)
                   {
                      alarmCfg.right_down.x = x;
                      alarmCfg.right_down.y = y;
                   }
                   else if(i == 3)
                   {
                       alarmCfg.left_down.x = x;
                       alarmCfg.left_down.y = y;
                   }

               }

               if(alarmCfg.enable == 1)
               {
                   if(alarmCfg.right_down.x < 0 || alarmCfg.right_down.x > 100 || alarmCfg.right_down.y < 0 || alarmCfg.right_down.y > 100
                       || alarmCfg.left_down.x < 0 || alarmCfg.left_down.x > 100 || alarmCfg.left_down.y < 0 || alarmCfg.left_down.y > 100
                       || alarmCfg.left_up.x < 0 || alarmCfg.left_up.x > 100 || alarmCfg.left_up.y < 0 || alarmCfg.left_up.y > 100
                       || alarmCfg.right_up.x < 0 || alarmCfg.right_up.x > 100 || alarmCfg.right_up.y < 0 || alarmCfg.right_up.y > 100)
                     return 0;
               }
           }
           int result= HD_SetAlarm(_type,alarmCfg);

       return result;
  }

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    GetAlram
 * Signature: (ILcom/qihancloud/alarmstrategy/alarm/bean/PointInfoList;)I
 */
/*JNIEXPORT jstring JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_GetAlram
  (JNIEnv *env, jobject obj, jint type){
    int _type=(int)type;
      alarm_param_t data;
      HD_GetAlram(_type,&data);
      cJSON *param=cJSON_CreateObject();
      cJSON_AddNumberToObject(param,"enable",data.enable);
      cJSON_AddNumberToObject(param,"rdx",data.right_down.x);
      cJSON_AddNumberToObject(param,"rdy",data.right_down.y);
      cJSON_AddNumberToObject(param,"ldx",data.left_down.x);
      cJSON_AddNumberToObject(param,"ldy",data.left_down.y);
      cJSON_AddNumberToObject(param,"lux",data.left_up.x);
      cJSON_AddNumberToObject(param,"luy",data.left_up.y);
      cJSON_AddNumberToObject(param,"rux",data.right_up.x);
      cJSON_AddNumberToObject(param,"ruy",data.right_up.y);
      cJSON_AddNumberToObject(param,"drt",data.drt);
      char *_param=cJSON_Print(&param);
     jstring result=(*env)->NewStringUTF(env,_param);
      return result;

  }*/

  JNIEXPORT jint JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_GetAlram
    (JNIEnv *env, jobject obj, jint type,jobject alarm_data,jobject point){
        jclass list_cls = (*env)->GetObjectClass(env,alarm_data);
            if(list_cls == NULL)
            {
                LOGI("GetObjectClass");
                return -99;
            }

            jmethodID list_add = (*env)->GetMethodID(env,list_cls,"add","(Ljava/lang/Object;)Z");

            jclass cfg_cls = (*env)->GetObjectClass(env,point);//FindClass�޷��ҵ���ԭ��δ֪��ֱ�Ӵ����࣬��ʱ���������
            if(cfg_cls == NULL)
            {
                LOGI("GetObjectClass++");
                return -99;
            }
            jmethodID cfg_costruct = (*env)->GetMethodID(env,cfg_cls,"<init>","(FFFFI)V");

            alarm_param_t alarmCfg;
            memset(&alarmCfg,0,sizeof(alarm_param_t));


            int ret = HD_GetAlram((int)type, &alarmCfg);
            if(ret < 0)
            {
                return ret;
            }

            if(alarmCfg.enable == 1)
            {
                if(alarmCfg.right_down.x < 0 || alarmCfg.right_down.x > 100 || alarmCfg.right_down.y < 0 || alarmCfg.right_down.y > 100
                    || alarmCfg.left_down.x < 0 || alarmCfg.left_down.x > 100 || alarmCfg.left_down.y < 0 || alarmCfg.left_down.y > 100
                    || alarmCfg.left_up.x < 0 || alarmCfg.left_up.x > 100 || alarmCfg.left_up.y < 0 || alarmCfg.left_up.y > 100
                    || alarmCfg.right_up.x < 0 || alarmCfg.right_up.x > 100 || alarmCfg.right_up.y < 0 || alarmCfg.right_up.y > 100)
                  return -1;
            }
            if(alarmCfg.enable == 1)
            {
                if((jint)type == 1)
                    return 1;

                if((jint)type == 3)
                    ret = alarmCfg.drt;
                   LOGE("alarmCfg.drt = %d",ret);

                 jobject cfgobj = (*env)->NewObject(env,cfg_cls,cfg_costruct,(jfloat)0,(jfloat)0,(jfloat)alarmCfg.left_up.x,(jfloat)alarmCfg.left_up.y,1);

                 (*env)->CallBooleanMethod(env,alarm_data,list_add,cfgobj);

                  cfgobj = (*env)->NewObject(env,cfg_cls,cfg_costruct,(jfloat)0,(jfloat)0,(jfloat)alarmCfg.right_up.x,(jfloat)alarmCfg.right_up.y,2);

                  (*env)->CallBooleanMethod(env,alarm_data,list_add,cfgobj);

                if((jint)type == 2)
                {
                    cfgobj = (*env)->NewObject(env,cfg_cls,cfg_costruct,(jfloat)0,(jfloat)0,(jfloat)alarmCfg.right_down.x,(jfloat)alarmCfg.right_down.y,3);

                    (*env)->CallBooleanMethod(env,alarm_data,list_add,cfgobj);

                    cfgobj = (*env)->NewObject(env,cfg_cls,cfg_costruct,(jfloat)0,(jfloat)0,(jfloat)alarmCfg.left_down.x,(jfloat)alarmCfg.left_down.y,4);

                    (*env)->CallBooleanMethod(env,alarm_data,list_add,cfgobj);

                }

            }
            else
            {
                if((jint)type == 1)
                       return 0;
            }
        return ret;
        }


/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    SetTime
 * Signature: (JII)I
 */
JNIEXPORT jint JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_SetTime
  (JNIEnv *env, jobject obj, jlong time, jint zone, jint dstTm){
    int _zone=(int)zone;
    int _dstTm=(int)dstTm;
            time_t _time=(time_t)time;
        int result= HD_SetTime(_time,_zone,_dstTm);
        return result;
  }

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    GetTime
 * Signature: (Lcom/qihancloud/alarmstrategy/alarm/bean/CameraTime;)I
 */
JNIEXPORT jstring JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_GetTime
  (JNIEnv *env, jobject obj){
  localTime_config_t time_local = {0};
       HD_GetTime(&time_local);
       cJSON *ctime=cJSON_CreateObject();
       cJSON_AddNumberToObject(ctime,"zone",time_local.zone);
      cJSON_AddNumberToObject(ctime,"dst_time",time_local.dstTm);
      cJSON_AddNumberToObject(ctime,"reserve",time_local.reserve);
      cJSON_AddNumberToObject(ctime,"time",(long)time_local.utcSec);
      char *ch_time=cJSON_Print(ctime);
      jstring result=(*env)->NewStringUTF(env,ch_time);
      return result;
      }

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    GetVersion
 * Signature: (Lcom/qihancloud/alarmstrategy/alarm/bean/VersionInfo;)I
 */
JNIEXPORT jstring JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_GetVersion
  (JNIEnv *env, jobject obj){
        version_info_t version_info = {0};
        HD_GetVersion(&version_info);
         cJSON *cversion=cJSON_CreateObject();
         cJSON_AddStringToObject(cversion,"version_number",version_info.version_number);
        cJSON_AddStringToObject(cversion,"device_type",version_info.device_type);
        cJSON_AddStringToObject(cversion,"version_date",version_info.version_date);
        cJSON_AddNumberToObject(cversion,"server_type",version_info.server_type);
        char *ch_version=cJSON_Print(cversion);
        jstring result=(*env)->NewStringUTF(env,ch_version);
        return result;

  }

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    SetAlarmCallback
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_SetAlarmCallback
  (JNIEnv *env, jobject obj){
    HD_SetAlarmCallback(ala_callback);
  }

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    CreateSession
 * Signature: (Ljava/lang/String;I)Ljava/lang/String;
 * char* HD_CreateSession(char* p2p_ip, int p2p_port);
 */
JNIEXPORT jstring JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_CreateSession
  (JNIEnv *env, jobject obj, jstring ip, jint port){
        char *_ip=(*env)->GetStringUTFChars(env,ip,NULL);
        int _port = (int)port;
         char* _session=HD_CreateSession(_ip,_port);
         jstring result=(*env)->NewStringUTF(env,_session);

        return result;
  }

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    CloseSession
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_CloseSession
  (JNIEnv *env, jobject obj){
     HD_CloseSession();
  }

JNIEXPORT jlong JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_StartVideo
      (JNIEnv *env, jobject obj,jint stream_type){
            int _stream_type = (int)stream_type;
            long result = HD_StartVideo(_stream_type,streamCallback);
            return result;
      }



    JNIEXPORT void JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_StopVideo
      (JNIEnv *env, jobject obj,jlong handler){
            long _handler = (long)handler;
            HD_StopVideo(_handler);
      }


       //  public native void UpdateIdarling(long size,String data);

     //static const char* const PathName = "/sdcard/test";
     JNIEXPORT jint JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_UpdateIdarling
       (JNIEnv *env, jobject object, jlong size,jbyteArray data){
       long _size = (long)size;
      // int len = (long)size;
       //char *_data = (*env)->GetStringUTFChars(env,data,NULL);

        /* 将数据保存成文件
       int fd;
       fd = open(PathName,02,0666);
       if(fd == -1){
       LOGE("open faile");
       }
       int res = write(fd,_data,len);
       if(res!=len){
         LOGE("write faile res=%d,len = %d",res,len);
         }
         LOGE("write success res=%d,len = %d",res,len);
       close(fd);
       */

       //jsize   strLen = env->GetArrayLength(data);
       char* _data= (char*)((*env)->GetByteArrayElements(env,data, NULL));
        LOGE("update _size = %lld",size);
         int res = HD_UpdateIdarling(_size,_data,upgrade_callBack);
         (*env)->ReleaseByteArrayElements(env, data, _data, 0);

         return res;
       }




  void p2p_callBack(unsigned long handler,int command, char* param){
        JNIEnv *env = 0;
        if ((*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL) != JNI_OK) {
            LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
            return;
        }
            jlong _handler = (jlong)handler;
            jint  _command = (jint)command;
           // char *_param=(*env)->GetStringUTFChars(env,param,NULL);
           jstring _param=(*env)->NewStringUTF(env,param);
            jmethodID java_p2p_callback=(*env)->GetMethodID(env,native_proxy_cls,"P2pCallBack","(JILjava/lang/String;)V");
            (*env)->CallVoidMethod(env, g_obj, java_p2p_callback,_handler,_command,_param);

      if ((*g_jvm)->DetachCurrentThread(g_jvm)!=JNI_OK) {
          LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
      }

    LOGI("handler:%lu, command:%d, param:%s \n", handler, command, param);
    HD_CloseSession();

  }
  void ala_callback(int type, int subtype, int opt, void* data, int len){

  JNIEnv *env = 0;
      if ((*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL) != JNI_OK) {
          LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
          return;
      }
        jint _type=(jint)type;
        jint _subtype=(jint)subtype;
        jint _opt=(jint)opt;
        jint _len=(jint)len;

        /*jbyteArray byteArray= (*env)->NewByteArray(env,len);
        (*env)->SetByteArrayRegion(env,byteArray, 0, len, (jbyte*)data);
        (*env)->DeleteLocalRef(env,byteArray);
        */
        jmethodID java_alarm_callback=(*env)->GetMethodID(env,native_proxy_cls,"AlarmCallBack","(III)V");
        (*env)->CallVoidMethod(env, g_obj, java_alarm_callback,_type,_subtype,_opt);


        if ((*g_jvm)->DetachCurrentThread(g_jvm)!=JNI_OK) {
                    LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
                }
  }

void streamCallback(unsigned long handler, void* data, int len ){
    JNIEnv *env = 0;
    if ((*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL) != JNI_OK) {
        LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
        return;
    }

    ptc_frame_head_t* frameInfo = (ptc_frame_head_t*)data;
    char* data_temp = (char*)data;
    int head_len = sizeof(ptc_frame_head_t);


        jlong _handler = (jlong)handler;
        jint _len = (jint)len;



        if(frameInfo->frame_type == 1 || frameInfo->frame_type == 2)
            {
                if(show_firstFrame ==1){
                    if(frameInfo->frame_type != 1){
                        goto step_end;
                    }else{
                        show_firstFrame=0;
                    }
                }else{
                    if(frameInfo->no!=(videoFrameNum+1)){
                        show_firstFrame = 1;
                        goto step_end;
                    }
                }

                jbyteArray byteArray= (*env)->NewByteArray(env,len-head_len);
                (*env)->SetByteArrayRegion(env,byteArray, 0, len-head_len, (jbyte*)data+head_len);

                jmethodID java_stream_callback=(*env)->GetMethodID(env,native_proxy_cls,"StreamCallBack","(J[BIII)V");
                // LOGE("frameInfo->width%d，frameInfo->height%d,frameInfo->len%d",frameInfo->width,frameInfo->height,len-head_len);
                (*env)->CallVoidMethod(env, g_obj, java_stream_callback,_handler,byteArray,len-head_len,frameInfo->width,frameInfo->height);
                (*env)->DeleteLocalRef(env,byteArray);

                videoFrameNum = frameInfo->no;
            }

        long frame_len = (long)(len-head_len);
       // char* frameBuffer= (char*)((*env)->GetByteArrayElements(env,byteArray, NULL));

        //是否录制视频
        if(recordflag==1)
        {
            recordstart = 1;
            if(firstFrame ==1)
            {
                   if(frameInfo->frame_type != 1 )
                   {
                       recordstart = 0;
                       goto step_end;
                   }else
                   {
                        time_t starttime;
                        struct tm * tm;
                        time ( &starttime );
                        tm = localtime ( &starttime );
                        //sprintf(filename,"%s%s",dir_name,asctime(timeinfo));
                        sprintf(filename,"%s%d_%d_%d_%d_%d_%d",dir_name,tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
                        out_fd = AVI_open_output_file(filename); //把文件描述符绑定到此文件上
                        if(out_fd == NULL)
                        {
                            printf("open file erro");
                        }
                        AVI_set_video(out_fd,frameInfo->width,frameInfo->height,frameInfo->frame_rate,"H264");//设置视频文件的格式
                        //void AVI_set_audio(avi_t *AVI, int channels, long rate, int bits, int format, long mp3rate);
                        //AVI_set_audio(out_fd,0,8000,16,1,0x55);
                        AVI_set_audio(out_fd,1,8000,8,6,64);
                        firstFrame = 0;
                   }
            }else
            {
                 //视频帧
                if(frameInfo->frame_type == 1 || frameInfo->frame_type == 2)
                    {
                       if(AVI_write_frame(out_fd,data+head_len,frame_len,1)<0)//向视频文件中写入一帧图像
                          {
                            printf("write video frame erro!");
                          }
                    }else{  //音频帧
                        if(AVI_write_audio(out_fd,data+head_len,frame_len)<0)
                            {
                               printf("write audio frame erro!");
                            }
                    }
                    frame_len = 0;
            }
            recordstart = 0;
        }



        //是否视频截图
        if(captureflag==1)
        {
               if(frameInfo->frame_type == 1)

               {
                    captureflag = 0;

                    int Outsize = 0;
                    long vd_handle = 0;//解码结构体地址

                    int ret = video_began(1,frameInfo->width, frameInfo->height, &vd_handle);
                    if(ret <0){
                        video_ended(&vd_handle);
                        goto step_end;
                       }

                    char* tempbuf = (char*)malloc(VIDEO_BUF_LEN);
                    memset(tempbuf, 0, VIDEO_BUF_LEN);
                    LOGE("video_decode vd_handle =%ld,frameInfo->len = %d,frame_len = %d",vd_handle,frameInfo->len,frame_len);
                    //int video_decode(long vd_handle, uint8_t* in_264Buffer, int in_264BufferSize, uint8_t* out_RGB24Buffer, int* out_image_width, int* out_image_height,int* outsize)
                    int result = video_decode(vd_handle, (uint8_t*)data_temp+head_len,frameInfo->len,(uint8_t*)tempbuf,(int*)&frameInfo->width,(int*)&frameInfo->height,&Outsize);
                    //LOGE("video_decode result =%d,tempbuf size = %s",result,tempbuf);

                    if(result>=0)
                    {
                       jbyteArray bufferArray= (*env)->NewByteArray(env,Outsize);
                       (*env)->SetByteArrayRegion(env,bufferArray, 0,Outsize, (jbyte*)tempbuf);

                       jmethodID java_stream_callback=(*env)->GetMethodID(env,native_proxy_cls,"ImageCallBack","(J[BIII)V");
                       (*env)->CallVoidMethod(env, g_obj, java_stream_callback,_handler,bufferArray,Outsize,frameInfo->width,frameInfo->height);
                        (*env)->DeleteLocalRef(env,bufferArray);
                    }else
                    {
                        LOGE("video_decode faile");
                    }
                    free(tempbuf);
           }

        }

    step_end:
    if ((*g_jvm)->DetachCurrentThread(g_jvm)!=JNI_OK) {
            LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
        }
}

JNIEXPORT void JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_startCapture
      (JNIEnv *env, jobject obj){
            //firstFrame_capture =1;
            captureflag =1;
      }

JNIEXPORT jint JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_startRecord
      (JNIEnv *env, jobject obj){
            //record_handle = HD_StartVideo(0,record_streamCallback);
            int res = 0;
            if(recordflag ==0){
                firstFrame =1;
                recordflag =1;
            }else{
                res = -1;  //已经开启录制，开启录制失败
            }
            return res;
      }

JNIEXPORT jint JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_stopRecord
      (JNIEnv *env, jobject obj){
      int res = 0;
          if(recordflag ==1){
               recordflag = 0;
               //stopRecord();
               while(recordstart == 1)usleep(100*1000);

              int i = AVI_close(out_fd);  //关闭文件描述符，并保存文件

              out_fd = NULL;
              if(i==0){
                  time_t endTime;
                  struct tm * timeinfo;
                  time (&endTime);
                  timeinfo = localtime (&endTime );

                  char newname[128];
                  sprintf(newname,"%s-%d_%d_%d_%d_%d_%d.avi",filename,timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);

                  int ret = -1;
                  ret = rename(filename,newname);

                  jstring path=(*env)->NewStringUTF(env,newname);
                  jmethodID java_scan_media=(*env)->GetMethodID(env,native_proxy_cls,"scanMedia","(Ljava/lang/String;)V");
                  (*env)->CallVoidMethod(env, g_obj, java_scan_media,path);


                  if(ret < 0)
                  LOGI("rename error");
              }else{
                  LOGE("close error");
               }
          }else{
               res = -1;  //已经关闭录制，关闭录制失败
          }
          return res;
      }

void stopRecord(){
        while(recordstart == 1)usleep(100*1000);

       int i = AVI_close(out_fd);  //关闭文件描述符，并保存文件

       out_fd = NULL;
       if(i==0){
           time_t endTime;
           struct tm * timeinfo;
           time (&endTime);
           timeinfo = localtime (&endTime );

           char newname[128];
           //sprintf(newname,"%s-%d.avi",filename,endTime.tv_sec);
           sprintf(newname,"%s-%d_%d_%d_%d_%d_%d.avi",dir_name,timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);


           int ret = -1;
           ret = rename(filename,newname);

           /*jstring path=(*env)->NewStringUTF(env,newname);
           jmethodID java_scan_media=(*env)->GetMethodID(env,native_proxy_cls,"scanMedia","(Ljava/lang/String;)V");
           (*env)->CallVoidMethod(env, g_obj, java_scan_media,path);*/


           if(ret < 0)
           LOGI("rename error");
       }else{
           LOGE("close error");
        }

}

int offline_callBack(int offline){
      JNIEnv *env = 0;
      if ((*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL) != JNI_OK)
      {
          LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
          return;
      }
        jint _offline=(jint)offline;
        jmethodID java_offline_callback=(*env)->GetMethodID(env,native_proxy_cls,"OfflineCallBack","(I)V");
        (*env)->CallVoidMethod(env, g_obj, java_offline_callback,_offline);


    if ((*g_jvm)->DetachCurrentThread(g_jvm)!=JNI_OK)
    {
       LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
    }
}
int upgrade_callBack(int send, int total){
      JNIEnv *env = 0;
      if ((*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL) != JNI_OK)
      {
          LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
          return;
      }
        jint _send=(jint)send;
        jint _total=(jint)total;
        jmethodID java_upgrade_callback=(*env)->GetMethodID(env,native_proxy_cls,"UpgradeCallBack","(II)V");
        (*env)->CallVoidMethod(env, g_obj, java_upgrade_callback,_send,_total);


    if ((*g_jvm)->DetachCurrentThread(g_jvm)!=JNI_OK)
    {
       LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
    }
}





#ifdef __cplusplus
}
#endif