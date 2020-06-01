#pragma once

//когда я закончил проект, это было где-то неделю назад, он был не таким большим, но вот я решил его протестировать
//и как оказалось, sqlite настолько гибкий, что не имеет статической типизации, это мне не подходило
//ну я и добавил проверку на несколько типов

#include <string>

//все типы, кроме string проверяются, любой тип sqlite который не подходит ни под один из них, просто будет string
static enum class ColumnType
{
	integer, string, real, bit
};

//ну думаю тут все понятно 
namespace TypeChecker
{
	bool hasType(const std::wstring& str, ColumnType type);

	bool isInteger(const std::wstring& st);
	bool isReal(const std::wstring& st);
	bool isBit(const std::wstring& st);
}