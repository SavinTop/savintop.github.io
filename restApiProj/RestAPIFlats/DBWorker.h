#pragma once

#include "sqlite/sqlite3.h"
#include <cpprest/json.h>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include "TablesInfoCache.h"


struct Table
{
	std::vector<std::wstring> Columns;
	std::vector<std::vector<std::wstring>> Rows;
};

class DBWorker
{
	sqlite3* db;
	TablesInfoCache tic;
	std::stack<std::wstring> errorStack;

public:
	DBWorker(const std::wstring &path);
	~DBWorker();

	int ExecuteNonQuery(const std::wstring& query);
	Table* ExecuteTableReader(const std::wstring& query);
	
	web::json::value ExecuteNonQueryJSON(const std::wstring& query);
	web::json::value ExecuteTableReaderJSON(const std::wstring& query);
	web::json::value GetColumnsInformationJSON(const std::wstring& tableName);
	
	web::json::value GetTableJSON(const std::wstring& columns, const std::wstring& tableName, std::wstring where = L"");

	web::json::value DeleteRow(const std::wstring& tableName, const std::wstring& pkValue);
	web::json::value AddRow(const std::wstring& tableName, web::json::array& columns, web::json::array& row);
	web::json::value EditRow(const std::wstring& tableName, web::json::array& columns, web::json::array& row, const std::wstring& keyValue);

	bool isErrorOccured();
	std::stack<std::wstring>& getErrorStack();

	sqlite3* getHandle() { return db; }
	TablesInfoCache& getTablesInfoCache() { return tic; }
};

