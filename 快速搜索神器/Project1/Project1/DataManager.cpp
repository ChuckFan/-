#include"DataManager.h"
void SqliteManager::Open(const string& path)
{
	int ret=sqlite3_open(path.c_str(), &_db);
	if (ret != SQLITE_OK)//打开成功
	{
		ERROE_LOG("sqlite3_open\n");
	}
	else
	{
		TRACE_LOG("sqlite3_open success\n");
	}
}
void SqliteManager::Close()
{
	int ret = sqlite3_close(_db);
	if (ret != SQLITE_OK)//打开成功
	{
		ERROE_LOG("sqlite3_close");
	}
	else
	{
		TRACE_LOG("sqlite3_close success\n");
	}
}
void SqliteManager::ExecuteSql(const string& sql)
{
	char* errmsg;
	int ret = sqlite3_exec(_db,sql.c_str(),nullptr,nullptr,&errmsg);
	if (ret != SQLITE_OK)//打开成功
	{
		ERROE_LOG("sqlite3_exec(%s) errmsg:%s\n",sql.c_str(),errmsg);
		sqlite3_free(errmsg);
	}
	else
	{
		TRACE_LOG("sqlite3_exec(%s) success\n",sql.c_str());
	}
}
void SqliteManager::GetTable(const string& sql, int& row, int& col, char**& ppRet)
{
	char* errmsg;
	int ret=sqlite3_get_table(_db, sql.c_str(), &ppRet, &row, &col, &errmsg);
	if (ret != SQLITE_OK)
	{
		ERROE_LOG("sqlite3_get_table(%s) errmsg:%s\n", sql.c_str());
		sqlite3_free(errmsg);
	}
	else
	{
		TRACE_LOG("sqlite3_exec(%s) success\n", sql.c_str());
	}
}
////////////////////////////////////////
void DataManager::Init()
{
	std::unique_lock<std::mutex> lock(_mtx);

	_dbmgr.Open(DB_NAME);
	char sql[256];
	sprintf(sql, "create table if not exists %s(id INTEGER PRIMARY KEY,path text,name text,name_pinyin text,name_initials text)", TB_NAME);
	_dbmgr.ExecuteSql(sql);
}
void DataManager::GetDoc(const string& path,std::set<string>& dbset)//查找path下的所有子文档
{
	char sql[256];
	sprintf(sql, "select name from %s where path='%s'", TB_NAME, path.c_str());
	int row, col;
	char** ppRet;
		std::unique_lock<std::mutex> lock(_mtx);
		AutoGetTable agt(_dbmgr, sql, row, col, ppRet);
		lock.unlock();
	for (int i = 1; i <= row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			dbset.insert(ppRet[i*col + j]);
		}
	}
}

void DataManager::InsertDoc(const string& path, const string& name)
{
	char sql[256];
	string pinyin = ChineseConvertPinYinAllSpell(name);
	string initials = ChineseConvertPinYinAllSpell(name);

	sprintf(sql, "insert into %s(path,name,name_pinyin,name_initials) values('%s','%s','%s','%s')", TB_NAME, path.c_str(), name.c_str(),pinyin.c_str(),initials.c_str());
	std::unique_lock<std::mutex> lock(_mtx);

	_dbmgr.ExecuteSql(sql);
}
void DataManager::DeleteDoc(const string& path,const string& name)
{
	char sql[256];
	sprintf(sql, "delete from %s where path='s' and name='%s'", TB_NAME, path.c_str(), name.c_str());
	_dbmgr.ExecuteSql(sql);

	string path_ = path;
	path_ += '\\';
	path_ += name;
	sprintf(sql, "delete from %s where path like '%s%'", TB_NAME, path_.c_str());
	std::unique_lock<std::mutex> lock(_mtx);

	_dbmgr.ExecuteSql(sql);
}
void DataManager::Search(const string& key, vector<std::pair<string, string>>& docinfos)
{
	//	char sql[256];
	//	string like_key;
	//	like_key += key;
	//	like_key += "%s";
	//	sprintf(sql, "select name,path from %s where name like '%s%'", TB_NAME, like_key.c_str());
	//	
	//	int row, col;
	//	char** ppRet;
	//	AutoGetTable agt(_dbmgr, sql, row, col, ppRet);
	//
	//	for (int i = 1; i <= row; ++i)
	//	{
	//			docinfos.push_back(std::make_pair(ppRet[i*col + 0], ppRet[i*col + 1]));
	//	}

	//首字母拼音实现
	char sql[256] = { '\0' };
	{
		string pinyin = ChineseConvertPinYinAllSpell(key);
		string initials = ChineseConvertPinYinInitials(key);

		sprintf(sql, "select name,path from %s where name_pinyin like'%%%s%%' or name_initials like '%%%s%%'", TB_NAME, pinyin.c_str(), initials.c_str());

		int row, col;
		char** ppRet;

		std::unique_lock<std::mutex> lock(_mtx);

		AutoGetTable agt(_dbmgr, sql, row, col, ppRet);
		lock.unlock();
		for (int i = 1; i <= row; ++i)
		{
			docinfos.push_back(std::make_pair(ppRet[i*col + 0], ppRet[i*col + 1]));
		}
	}

}

//高亮切割分段
void DataManager::SplitHighlight(const string& str, const string& key, string& prefix, string& highlight, string& suffix)
{
	//1.key原串的直接匹配
	size_t ht_start = str.find(key);
	if (ht_start != string::npos)
	{
		prefix = str.substr(0, ht_start);
		highlight = key;
		suffix = str.substr(ht_start + key.size(), string::npos);
		return;
	}
	//prefix = str;//匹配不上需要一些解决方案，否则会出现不对齐
	//TRACE_LOG("highlight no match\n");
	//return;

	//2.key是拼音全拼
	{
		string str_ap = ChineseConvertPinYinAllSpell(str);//str_ap为str的全拼
		string key_ap = ChineseConvertPinYinAllSpell(key);

		size_t ht_index = 0;
		size_t ap_index = 0;
		size_t ht_start, ht_len = 0;

		size_t ap_start = str_ap.find(key_ap);

		if (ap_start != string::npos)
		{

			size_t ap_end = ap_start + key_ap.size();
			while (ap_index < ap_end)
			{
				if (ap_index == ap_start)
				{
					ht_start = ht_index;
				}
				if (str[ht_index] >= 0 && str[ht_index] <= 127)
				{
					++ht_index;
					++ap_index;
				}
				else
				{
					char chinese[3] = { '\0' };
					chinese[0] = str[ht_index];
					chinese[1] = str[ht_index + 1];
					string ap_str = ChineseConvertPinYinAllSpell(chinese);
					ht_index += 2;//跳一个汉字
					ap_index += ap_str.size();//跳一个汉字拼音的长度

				}

			}
			ht_len = ht_index - ht_start;
			prefix = str.substr(0, ht_start);
			highlight = str.substr(ht_start, ht_len);
			suffix = str.substr(ht_start + ht_len, string::npos);
		}
		return;
	}

	//3.key是拼音首字母
	{
		string init_str = ChineseConvertPinYinAllSpell(str);
		string init_key = ChineseConvertPinYinAllSpell(key);
		size_t init_start = init_str.find(init_key);
		if (init_start != string::npos)
		{
			size_t init_end = init_start + init_key.size();
			size_t init_index = 0, ht_index = 0;
			size_t ht_start = 0, ht_len = 0;
			while (init_index < init_end)
			{
				if (init_index == init_start)
				{
					ht_start = ht_index;
				}
				if (str[ht_index] >= 0 && str[ht_index] <= 127)
				{
					++ht_index;
					++init_index;
				}
				else//汉字
				{
					ht_index += 2;
					++init_index;
				}
			}
			ht_len = ht_index - ht_start;
			prefix = str.substr(0, ht_start);
			highlight = str.substr(ht_start, ht_len);
			suffix = str.substr(ht_start + ht_len, string::npos);

			return;
		}
	}
	TRACE_LOG("split highlight no match.str:%s.,key:%s\n", str.c_str(), key.c_str());
	prefix = str;
}



