#pragma once

#include <string>

static enum class ColumnType
{
	integer, string, real, bit
};

namespace TypeChecker
{
	bool hasType(const std::wstring& str, ColumnType type);

	bool isInteger(const std::wstring& st);
	bool isReal(const std::wstring& st);
	bool isBit(const std::wstring& st);
}