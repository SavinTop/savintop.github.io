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

//Собственно класс, который работает с базой данных
class DBWorker
{
	sqlite3* db;
	TablesInfoCache tic;
	std::stack<std::wstring> errorStack;

public:
	DBWorker(const std::wstring &path);
	~DBWorker();

	//все методы, которые возвращают не JSON были созданны для внутреннего использования, а JSON методы 
	//для передачи на фронт

	//отправляет запрос в базу данных, и возвращает число затронутых строк
	int ExecuteNonQuery(const std::wstring& query);
	//отправляет запрос в базу данных, и считывает результирующую таблицу
	Table* ExecuteTableReader(const std::wstring& query);
	
	//аналог функции ExecuteNonQuery, только возращаемый результат в формате JSON
	web::json::value ExecuteNonQueryJSON(const std::wstring& query);

	//аналог функции ExecuteTableReader, только возращаемый результат в формате JSON
	web::json::value ExecuteTableReaderJSON(const std::wstring& query);

	//берёт информацию из кешированной таблицы, и переводит в формат JSON 
	web::json::value GetColumnsInformationJSON(const std::wstring& tableName);
	
	//конечный результат, эта функция и используется для GET запроса, он объеденяет данные из функций ExecuteTableReaderJSON и GetColumnsInformationJSON
	web::json::value GetTableJSON(const std::wstring& columns, const std::wstring& tableName, std::wstring where = L"");

	//удаляет строку в таблице, с ключем pkValue
	web::json::value DeleteRow(const std::wstring& tableName, const std::wstring& pkValue);
	//добавляет строку, не знаю зачем я это комментирую)))
	web::json::value AddRow(const std::wstring& tableName, web::json::array& columns, web::json::array& row);
	//редактирует строку, по ключу keyValue
	web::json::value EditRow(const std::wstring& tableName, web::json::array& columns, web::json::array& row, const std::wstring& keyValue);

	//собственно возникла ли ошибка
	bool isErrorOccured();
	//гетер для стека ошибок
	std::stack<std::wstring>& getErrorStack();

	//гетер для дискриптора sqlite3
	sqlite3* getHandle() { return db; }
	//гетер для информации о таблицах
	TablesInfoCache& getTablesInfoCache() { return tic; }
};

