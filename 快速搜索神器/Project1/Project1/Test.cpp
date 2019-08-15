//#include"Common.h"
//#include"DataManager.h"
#include"ScanManager.h"

void TestDirectory()
{
	vector<string> dirs;
	vector<string> files;

	DirectoryList("D:\\c��c++\\c++��Ŀ\\������������\\Project1", dirs,files);
	for (const auto& e : dirs)
	{
		cout << e << endl;
	}
	for (const auto& e : files)
	{
		cout << e << endl;
	}
}

void TestSqlite()
{
	int i;
	SqliteManager sq;
	sq.Open("test.db");

	string createtb_sql = "create table tb_doc(id integer primary key autoincrement,doc_path text,doc_name text)";
	sq.ExecuteSql(createtb_sql);

	string insert_sql = "insert into tb_doc(doc_path,doc_name) values('c��c++\','Project1')";
	sq.ExecuteSql(insert_sql);

	string query_sql = "select * from tb_doc where doc_path='c��c++\'";
	int row, col;
	char** ppRet;
	sq.GetTable(query_sql, row, col, ppRet);//�����ݿ��з��ʻص���
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			cout << ppRet[i*col+j] << "";
		}
		cout << endl;
	}
	//�������ݿ���ͷ�,����ΪRAII�����������ݿ�
	sqlite3_free_table(ppRet);
	AutoGetTable agt(sq, query_sql, row, col, ppRet);
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			cout << ppRet[i*col + j] << "";
		}
		cout << endl;
	}

}
void TestScanManager()
{
	/*ScanManager scanmgr;
	scanmgr.Scan("D:\\c��c++\\c++��Ŀ\\������������\\Project1");*/
}
void TestSearch()
{
	//ScanManager scanmgr;
	//scanmgr.Scan("D:\\c��c++\\c++��Ŀ\\������������\\Project1");
	//ScanManager::CreateInstance;
	//DataManager DataManager::GetInstance();
	//DataManager::GetInstance()->Init();
	vector<std::pair<string, string>> docinfos;
	string key;
	cout << "================================================" << endl;
	cout << "�����������ؼ��֣�";
	while (std::cin >> key)
	{
		docinfos.clear();
		DataManager::GetInstance()->Search(key, docinfos);
		printf("%-50s %-50s\n", "����", "·��");
		for (const auto& e : docinfos)
		{
			//cout << e.first << " " << e.second << endl;
			//printf("%-50s %-50s\n", e.first.c_str(), e.second.c_str());
			string prefix, highlight, suffix;
			const string& name = e.first;
			const string& path = e.second;
			DataManager::GetInstance()->SplitHighlight(e.first, key, prefix, highlight, suffix);
			cout << prefix;
			ColourPrintf(highlight.c_str());
			cout << suffix;

			//����50���ж���
			for (size_t i = name.size(); i <=50; ++i)
			{
				cout << " ";
			}

			printf("%-50s\n", e.second.c_str());
		}
		cout << "======================" << endl;
		cout << "�����������ؼ��֣�";
	}
}
void TestHighlight()
{
	{
		//1.key��ʲô������key
		//ColourPrintf("��Ҫ��\n");
		string key = "���ؿƼ�";
		string str = "�������ؿƼ�666";
		size_t pos = str.find(key);
		string prefix, suffix;
		prefix = str.substr(0, pos);//ǰ׺
		suffix = str.substr(pos + key.size(), string::npos);//��׺
		cout << prefix;
		ColourPrintf(key.c_str());
		cout << suffix << endl;
	}

	{
	//2.key��ƴ����������Ӧ����
	string key = "bitekeji";
	string str = "�������ؿƼ�666";
	string prefix, suffix;
	string str_py = ChineseConvertPinYinAllSpell(str);
	string key_py = ChineseConvertPinYinAllSpell(key);

	size_t pos = str_py.find(key);
	if (pos == string::npos)
	{
		cout << "ƴ����ƥ��" << endl;
	}
	else
	{
		
		size_t key_start = pos;
		size_t key_end = pos + key_py.size();
		char chinese[3] = {'\0'};
		size_t str_i = 0, py_i = 0;
		while (py_i < key_start)
		{
			//���ascii�ַ���Ϊ��ĸ��ֱ����һλ�������Ǻ�������λ
			if (str[str_i] >= 0 && str[str_i] <= 127)
			{
				++str_i;
				++py_i;
			}
			else
			{

				//һ������gbk����Ϊ�����ֽ�,ͨ�������ֽ��ж�һ�����֣��ټ�������ƴ��
				chinese[0] = str[str_i];
				chinese[1] = str[str_i + 1];
				str_i += 2;
				string py_str = ChineseConvertPinYinAllSpell(chinese);
				py_i += py_str.size();
			}
		}


		prefix = str.substr(0, str_i);
		size_t str_j = str_i, py_j = py_i;
		while (py_j < key_end)
		{
			if (str[str_j] >= 0 && str[str_j] <= 127)
			{
				++str_j;
				++py_j;
			}
			chinese[0] = str[str_j];
			chinese[1] = str[str_j + 1];
			str_j += 2;
			string py_str = ChineseConvertPinYinAllSpell(chinese);
			py_j += py_str.size();
		}
		
		key = str.substr(str_i,str_j-str_i);
		suffix = str.substr(str_j, string::npos);
	}
	cout << prefix;
	ColourPrintf(key.c_str());
	cout << suffix << endl;
	}

	{
		//3.key��ƴ������ĸ��������Ӧ����
		string key = "btkj";
		string str = "�������ؿƼ�666";
		string prefix, suffix;

	}
}
int main()
{
	TestDirectory();
	TestSqlite();
	TestSearch();
	TestHighlight();
	system("pause");
	return 0;
}