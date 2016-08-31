package com.sunbo.main.aidl;  

interface IMyServiceCallback {
	void ShowData(in byte[] data);
	void doCmd(int app,int cmd,int sub_cmd,String content);
}