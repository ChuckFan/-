
#include "ScanManager.h"

void ScanManager::Scan(const string& path)
{
	//本地文件
	vector<string> localdirs;
	vector<string> localfiles;
	DirectoryList(path, localdirs, localfiles);

	std::set<string> localset;
	localset.insert(localfiles.begin(), localfiles.end());
	localset.insert(localdirs.begin(), localdirs.end());

	//数据库

	std::set<string> dbset;
	DataManager::GetInstance()->GetDoc(path, dbset);

	//DataManager
	auto localit = localset.begin();
	auto dbit = dbset.begin();
	while (localit != localset.end() && dbit != dbset.end())
	{
		//谁小谁就是另一方没有的
		if (*localit < *dbit)//本地有，数据库没有，新增
		{
			DataManager::GetInstance()->InsertDoc(path, *localit);
			++localit;
		}
		else if (*localit > *dbit)//本地没有，数据库有，删除
		{
			DataManager::GetInstance()->DeleteDoc(path, *dbit);
			++dbit;
		}
		else//不变
		{
			++dbit;
			++localit;
		}
	}
	while (localit != localset.end())
	{
		//新增数据
		DataManager::GetInstance()->InsertDoc(path, *localit);
		++localit;
	}
	while (dbit != dbset.end())
	{
		//删除数据
		DataManager::GetInstance()->DeleteDoc(path, *dbit);
		++dbit;
	}
	//递归扫描比对数据
	for (const auto& subdirs : localdirs)
	{
		string subpath = path;
		subpath += '\\';
		subpath += subdirs;
		Scan(subdirs);
	}
}