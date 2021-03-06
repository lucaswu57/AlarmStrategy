/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_qihancloud_alarmstrategy_AlarmApi */

#ifndef _Included_com_qihancloud_alarmstrategy_AlarmApi
#define _Included_com_qihancloud_alarmstrategy_AlarmApi
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    Init
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_Init
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    CleanUp
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_CleanUp
  (JNIEnv *, jobject);

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    GetIdarlingState
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_GetIdarlingState
  (JNIEnv *, jobject);

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    SetAlarm
 * Signature: (ILcom/qihancloud/alarmstrategy/alarm/bean/PointInfoList;)I
 */
JNIEXPORT jint JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_SetAlarm
  (JNIEnv *, jobject, jint, jint,jobject,jint);

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    GetAlram
 * Signature: (ILcom/qihancloud/alarmstrategy/alarm/bean/PointInfoList;)I
 */
JNIEXPORT jint JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_GetAlram
  (JNIEnv *, jobject, jint,jobject,jobject);

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    SetTime
 * Signature: (JII)I
 */
JNIEXPORT jint JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_SetTime
  (JNIEnv *, jobject, jlong, jint, jint);

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    GetTime
 * Signature: (Lcom/qihancloud/alarmstrategy/alarm/bean/CameraTime;)I
 */
JNIEXPORT jstring JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_GetTime
  (JNIEnv *, jobject);

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    GetVersion
 * Signature: (Lcom/qihancloud/alarmstrategy/alarm/bean/VersionInfo;)I
 */
JNIEXPORT jstring JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_GetVersion
  (JNIEnv *, jobject);

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    SetAlarmCallback
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_SetAlarmCallback
  (JNIEnv *, jobject);

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    StartVideo
 * Signature:
 */
JNIEXPORT jlong JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_StartVideo
  (JNIEnv *, jobject,jint);

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    StopVideo
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_StopVideo
  (JNIEnv *, jobject,jlong);


/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    CreateSession
 * Signature: (Ljava/lang/String;I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_CreateSession
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     com_qihancloud_alarmstrategy_AlarmApi
 * Method:    CloseSession
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_CloseSession
  (JNIEnv *, jobject);

  /*
   * Class:     com_qihancloud_alarmstrategy_AlarmApi
   * Method:    UpdateIdarling
   * Signature: ()V
   */

   //  public native void UpdateIdarling(long size,String data);
  JNIEXPORT jint JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_UpdateIdarling
    (JNIEnv *, jobject, jlong, jbyteArray);
    //(JNIEnv *, jobject, jlong, jstring);

JNIEXPORT jint JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_startRecord
      (JNIEnv *, jobject);

JNIEXPORT jint JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_stopRecord
      (JNIEnv *, jobject );

JNIEXPORT void JNICALL Java_com_qihancloud_alarmstrategy_AlarmApi_startCapture
      (JNIEnv *, jobject);


#ifdef __cplusplus
}
#endif
#endif
