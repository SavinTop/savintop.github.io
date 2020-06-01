#pragma once

#include <string>
#include <map>
#include "TypeChecker.h"

class DBWorker;

static struct Column
{
	std::wstring name;
	ColumnType type;
	std::wstring sqliteType;
	bool notNull;
	bool pk;
	bool fk;
	std::wstring tableTo;
	std::wstring columnTo;

	std::wstring ColumnType_str() {
		static std::wstring ColumnTypeStrs[] = { L"integer",L"string",L"real",L"bit"}; return ColumnTypeStrs[(int)this->type] ;
	};
};

static struct TableInfo
{
	std::wstring name;
	std::wstring pkName;

	std::map<std::wstring, Column> columns;

	void Update(const std::wstring& name, DBWorker& db);
	TableInfo(const std::wstring& name, DBWorker& db);
};

class TablesInfoCache
{
	std::map<std::wstring, TableInfo> tables;
public:
	TablesInfoCache() {};
	TablesInfoCache(DBWorker& db);
	void Update(DBWorker& db);

	std::map<std::wstring, TableInfo>& getTables() { return tables; };
};

