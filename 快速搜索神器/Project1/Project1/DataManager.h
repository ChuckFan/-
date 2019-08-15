#pragma once
#include"Common.h"
//建数据表
class SqliteManager {
public:   
			SqliteManager() :_db(nullptr)    {}
		   ~SqliteManager()    {
			   Close();
		   }
		   void Open(const string& path); 
		   void Close(); 
		   void ExecuteSql(const string& sql); 
		   void GetTable(const string& sql, int& row, int& col, char**& ppRet);
		   SqliteManager(const SqliteManager&) = delete; 
		   SqliteManager& operator=(const SqliteManager&) = delete;
private:   
	sqlite3* _db;       // 数据库对象
};
//RAII自动释放数据库回调
class AutoGetTable
{
public:
	//构造函数获取资源，析构函数释放资源
	AutoGetTable(SqliteManager& sm,const string& sql, int& row, int& col, char**& ppRet)
	{
		sm.GetTable(sql, row, col, ppRet);
		_ppRet = ppRet;
	}

	~AutoGetTable()
	{
		sqlite3_free_table(_ppRet);
	}
	//防拷贝,模仿stl的unique实现
	AutoGetTable(const AutoGetTable&) = delete;
	AutoGetTable operator=(const AutoGetTable&) = delete;
private:
	char** _ppRet;
};

//数据管理模块
#define TB_NAME "tb_doc"
#define DB_NAME "doc.db"
//为了方便加锁设计成单例模式
class DataManager
{
public:
	static DataManager* GetInstance()
	{
		static DataManager datamgr;
		datamgr.Init();
		return &datamgr;
	}
	void Init();//建表打开数据库
	void GetDoc(const string& path,std::set<string>& dbset);//查找path下的所有子文档
	void InsertDoc(const string& path, const string& name);
	void DeleteDoc(const string& path, const string& name);
	void Search(const string& key, vector<std::pair<string, string>>& docinfos);
	void SplitHighlight(const string& str, const string& key, string& prefix, string& highlight, string& suffix);
private:
	DataManager(){}
		SqliteManager _dbmgr;
		std::mutex _mtx;
};