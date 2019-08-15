#pragma once

#include"DataManager.h"

//扫描
//设计成单例模式，整个程序只有一个扫描模块
class ScanManager
{
public:

	//比对 文件系统和数据库
	void Scan(const string& path);
	void StartScan()
	{
		while (1)
		{
			Scan("D:\\c及c++\\c++项目\\快速搜索神器\\Project1");
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
	ScanManager(const ScanManager&);//封死
	//DataManager _datamgr;
};