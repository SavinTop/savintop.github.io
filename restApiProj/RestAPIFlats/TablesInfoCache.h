#pragma once

//это все нужно было для быстрого доступа к информации о таблицах, тут все загружается и пакуется в удобном формате 
//ну или не очень быстрого, в этом проекте по сути нет никаких оптимизаций 

#include <string>
#include <map>
#include "TypeChecker.h"

//ну это просто было необходимо)
class DBWorker;

//информация о столбце
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

//информация о таблице)
static struct TableInfo
{
	std::wstring name;
	std::wstring pkName;

	std::map<std::wstring, Column> columns;

	//полностью обновляет информацию о таблице
	void Update(const std::wstring& name, DBWorker& db);
	//полностью обновляет информацию о таблице
	TableInfo(const std::wstring& name, DBWorker& db);
};

class TablesInfoCache
{
	std::map<std::wstring, TableInfo> tables;
public:
	TablesInfoCache() {};
	//полностью обновляет информацию о таблицах
	TablesInfoCache(DBWorker& db);
	//полностью обновляет информацию о таблицах
	void Update(DBWorker& db);

	//гетер для таблиц
	std::map<std::wstring, TableInfo>& getTables() { return tables; };
};

