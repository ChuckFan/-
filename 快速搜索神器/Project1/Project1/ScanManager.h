#pragma once

#include"DataManager.h"

//ɨ��
//��Ƴɵ���ģʽ����������ֻ��һ��ɨ��ģ��
class ScanManager
{
public:

	//�ȶ� �ļ�ϵͳ�����ݿ�
	void Scan(const string& path);
	void StartScan()
	{
		while (1)
		{
			Scan("D:\\c��c++\\c++��Ŀ\\������������\\Project1");
			std::this_thread::sleep_for(std::chrono::seconds(3));
		}
	}
	static ScanManager* CreateInstance()
	{
		static ScanManager scanmgr;
		static std::thread thd(&StartScan, &scanmgr);
		thd.detach();
		return &scanmgr;
	}
private:
	ScanManager()
	{
		//_datamgr.Init();
	}
	ScanManager(const ScanManager&);//����
	//DataManager _datamgr;
};