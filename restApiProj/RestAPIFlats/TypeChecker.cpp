#include "TypeChecker.h"

bool TypeChecker::hasType(const std::wstring& str, ColumnType type)
{
	switch (type)
	{
	case ColumnType::integer: return isInteger(str);
	case ColumnType::real: return isReal(str);
	case ColumnType::bit: return isBit(str);
	default: return true;
	}
}

bool TypeChecker::isInteger(const std::wstring& st)
{
	if (st.empty()) return true;
	int i = (st[0] == L'-' || st[0] == L'+') ? 1 : 0;
	if (i && st.size() == 1) return false;
	for (; i < st.size(); i++)
	{
		wchar_t el = st[i];
		if (el > L'9' || el < L'0') return false;
	}
	
	return true;
}

bool TypeChecker::isReal(const std::wstring& st)
{
	if (st.empty()) return true;
	int i = (st[0] == L'-' || st[0] == L'+') ? 1 : 0;
	if (i && st.size() == 1) return false;
	if (i && st[1] == L'.') return false;
	if (!i && st[0] == L'.') return false;
	bool dotFounded = false;
	for (; i < st.size(); i++)
	{
		wchar_t el = st[i];

		if (el == L'.')
		{
			if (dotFounded) return false; else dotFounded = true;
		}else
		if (el > L'9' || el < L'0') return false;
	}

	return true;
}

bool TypeChecker::isBit(const std::wstring& st)
{
	return st == L"1" || st==L"0";
}
