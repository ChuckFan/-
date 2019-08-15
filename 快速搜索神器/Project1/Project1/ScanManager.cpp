
#include "ScanManager.h"

void ScanManager::Scan(const string& path)
{
	//�����ļ�
	vector<string> localdirs;
	vector<string> localfiles;
	DirectoryList(path, localdirs, localfiles);

	std::set<string> localset;
	localset.insert(localfiles.begin(), localfiles.end());
	localset.insert(localdirs.begin(), localdirs.end());

	//���ݿ�

	std::set<string> dbset;
	DataManager::GetInstance()->GetDoc(path, dbset);

	//DataManager
	auto localit = localset.begin();
	auto dbit = dbset.begin();
	while (localit != localset.end() && dbit != dbset.end())
	{
		//˭С˭������һ��û�е�
		if (*localit < *dbit)//�����У����ݿ�û�У�����
		{
			DataManager::GetInstance()->InsertDoc(path, *localit);
			++localit;
		}
		else if (*localit > *dbit)//����û�У����ݿ��У�ɾ��
		{
			DataManager::GetInstance()->DeleteDoc(path, *dbit);
			++dbit;
		}
		else//����
		{
			++dbit;
			++localit;
		}
	}
	while (localit != localset.end())
	{
		//��������
		DataManager::GetInstance()->InsertDoc(path, *localit);
		++localit;
	}
	while (dbit != dbset.end())
	{
		//ɾ������
		DataManager::GetInstance()->DeleteDoc(path, *dbit);
		++dbit;
	}
	//�ݹ�ɨ��ȶ�����
	for (const auto& subdirs : localdirs)
	{
		string subpath = path;
		subpath += '\\';
		subpath += subdirs;
		Scan(subdirs);
	}
}