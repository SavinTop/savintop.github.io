#include "TablesInfoCache.h"
#include <memory>
#include "DBWorker.h"

void TableInfo::Update(const std::wstring& name, DBWorker& db)
{
	static std::map<std::wstring, ColumnType> name_type = {
	{L"integer", ColumnType::integer},
	{L"int", ColumnType::integer},
	{L"float", ColumnType::real},
	{L"double", ColumnType::real},
	{L"real", ColumnType::real},
	{L"string", ColumnType::string},
	{L"text", ColumnType::string},
	{L"bit", ColumnType::bit},
	{L"boolean", ColumnType::bit},
	{L"bool", ColumnType::bit},
	};

	this->name = name;
	columns.clear();
	
	std::unique_ptr<Table> tbl(db.ExecuteTableReader(L"select [name], [type], [notnull], [pk] from pragma_table_info(\"" + name+ L"\")"));
	std::unique_ptr<Table> tblForeign(db.ExecuteTableReader(L"select [table], [from], [to] from pragma_foreign_key_list(\""+name+L"\")"));

	std::map<std::wstring, std::pair<std::wstring, std::wstring>> foreignKeys;

	for (int i = 0; i < tblForeign->Rows.size(); i++)
	{
		std::wstring& tempName = tblForeign->Rows[i][1];
		foreignKeys.insert({ tempName, std::make_pair(tblForeign->Rows[i][0], tblForeign->Rows[i][2]) });
	}

	sqlite3* dbh = db.getHandle();

	for (const auto& row : tbl->Rows)
	{
		Column Temp;
		Temp.name = row[0];
		Temp.sqliteType = row[1];
		auto res = name_type.find(Temp.sqliteType);
		Temp.type = res != name_type.end() ? res->second : ColumnType::string;
		Temp.notNull = (row[2] == L"1");
		Temp.pk = (row[3] == L"1");

		if (Temp.pk) pkName = Temp.name;

		auto fres = foreignKeys.find(Temp.name);
		if (fres != foreignKeys.end())
		{
			Temp.fk = true;
			Temp.tableTo = fres->second.first;
			Temp.columnTo = fres->second.second;
		}
		else Temp.fk = false;

		columns.insert({ Temp.name, Temp });
	}
	
}

TableInfo::TableInfo(const std::wstring& name, DBWorker& db)
{
	Update(name, db);
}

TablesInfoCache::TablesInfoCache(DBWorker& db)
{
	Update(db);
}

void TablesInfoCache::Update(DBWorker& db)
{
	tables.clear();
	std::wstring query = L"SELECT name FROM sqlite_master WHERE type = 'table' AND name NOT LIKE 'sqlite_%';";
	std::unique_ptr<Table> tbl(db.ExecuteTableReader(query));

	for (const auto& row : tbl->Rows)
	{
		tables.insert({ row[0], TableInfo(row[0], db) });
	}
}
