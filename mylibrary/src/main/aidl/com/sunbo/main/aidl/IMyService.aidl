package com.sunbo.main.aidl;  

import com.sunbo.main.aidl.IMyServiceCallback;


interface IMyService {  

	void SetCurStatus(int app,int status,int sub_status);//设置当前所在的app执行的状态和子状态
	int SendCmd(int app,int cmd,int sub_cmd,String content);//app向主进程发送命令 content 根据具体的cmd判断

    void register(IMyServiceCallback cb);//注册回调
    void unregister();//取消回调
}