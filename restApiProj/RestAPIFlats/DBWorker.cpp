#include "DBWorker.h"

DBWorker::DBWorker(const std::wstring &path)
    : db(nullptr)
{
    if (sqlite3_open16(path.c_str(), &db))
    {
        errorStack.push((const wchar_t*)sqlite3_errmsg16(db));
    }
    else
    {
        tic = TablesInfoCache(*this);
    }
}

DBWorker::~DBWorker()
{
    sqlite3_close(db);
}

int DBWorker::ExecuteNonQuery(const std::wstring &query)
{
    sqlite3_stmt *stmt;
    sqlite3_prepare16(db, query.c_str(), query.length() * sizeof(wchar_t), &stmt, 0);
    int res = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (res != SQLITE_OK && res != SQLITE_ROW)
    {
        errorStack.push((const wchar_t*)sqlite3_errmsg16(db));
    }
        

    return sqlite3_changes(db);
}

Table *DBWorker::ExecuteTableReader(const std::wstring &query)
{
    Table *tbl = new Table();

    sqlite3_stmt *stmt;
    sqlite3_prepare16(db, query.c_str(), query.length() * sizeof(wchar_t), &stmt, 0);

    unsigned int columnCount = sqlite3_column_count(stmt);

    tbl->Columns.reserve(columnCount);

    for (int i = 0; i < columnCount; i++)
        tbl->Columns.push_back((wchar_t *)sqlite3_column_name16(stmt, i));

    int res;
    while ((res = sqlite3_step(stmt)) != SQLITE_DONE)
    {
        if (res != SQLITE_OK && res != SQLITE_ROW)
        {
            errorStack.push((const wchar_t*)sqlite3_errmsg16(db));
            break;
        }
        std::vector<std::wstring> tempVec;
        tempVec.reserve(columnCount);
        for (int i = 0; i < columnCount; i++)
        {
            auto tempWchar = (const wchar_t *)sqlite3_column_text16(stmt, i);
            tempVec.push_back(tempWchar ? tempWchar : L"NULL");
        }
        tbl->Rows.push_back(tempVec);
    }

    sqlite3_finalize(stmt);

    return tbl;
}

web::json::value DBWorker::ExecuteNonQueryJSON(const std::wstring &query)
{
    sqlite3_stmt *stmt;
    sqlite3_prepare16(db, query.c_str(), query.length() * sizeof(wchar_t), &stmt, 0);
    int res = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (res != SQLITE_OK && res != SQLITE_ROW && res != SQLITE_DONE)
    {
        errorStack.push((const wchar_t*)sqlite3_errmsg16(db));
    }
        
    web::json::value ret;
    ret[U("RowsModified")] = web::json::value::string(std::to_wstring(sqlite3_changes(db)));

    return ret;
}

web::json::value DBWorker::ExecuteTableReaderJSON(const std::wstring &query)
{
    sqlite3_stmt *stmt;
    sqlite3_prepare16(db, query.c_str(), query.length() * sizeof(wchar_t), &stmt, 0);

    unsigned int columnCount = sqlite3_column_count(stmt);

    web::json::value columns;

    for (int i = 0; i < columnCount; i++)
        columns[i] = web::json::value::string((wchar_t *)sqlite3_column_name16(stmt, i));

    web::json::value rows;

    int res;
    for (int cnt = 0; (res = sqlite3_step(stmt)) != SQLITE_DONE; cnt++)
    {
        if (res != SQLITE_OK && res != SQLITE_ROW)
        {
            errorStack.push((const wchar_t*)sqlite3_errmsg16(db));
            break;
        }
        web::json::value row;
        for (int i = 0; i < columnCount; i++)
        {
            auto tempWchar = (const wchar_t *)sqlite3_column_text16(stmt, i);

            row[i] = web::json::value::string(tempWchar ? tempWchar : L"NULL");
        }
        rows[cnt] = row;
    }

    sqlite3_finalize(stmt);

    web::json::value ans;
    ans[U("Columns")] = columns;
    ans[U("Rows")] = rows;

    return ans;
}

web::json::value DBWorker::GetColumnsInformationJSON(const std::wstring &tableName)
{
    web::json::value ans;

    auto info = tic.getTables().find(tableName);
    if (info != tic.getTables().end())
    {
        auto temp = info->second.columns;

        int cnt = 0;
        for (auto &[fir, sec] : temp)
        {
            web::json::value tempArr;
            tempArr[U("Type")] = web::json::value(sec.ColumnType_str());
            tempArr[U("PrimaryKey")] = web::json::value(sec.pk);
            tempArr[U("NotNull")] = web::json::value(sec.notNull);
            if (sec.fk)
            {
                tempArr[U("ForeignKey")] = web::json::value(true);
                tempArr[U("TableTo")] = web::json::value(sec.tableTo);
                tempArr[U("ColumnTo")] = web::json::value(sec.columnTo);
                tempArr[U("RefTable")] = ExecuteTableReaderJSON(L"select * from " + sec.tableTo);
            }
            else
                tempArr[U("ForeignKey")] = web::json::value(false);

            ans[fir] = tempArr;
        }
    }
    else
    {
        errorStack.push(L"Cant find table information. Table name: "+tableName);
    }
    return ans;
}

web::json::value DBWorker::GetTableJSON(const std::wstring &columns, const std::wstring &tableName, std::wstring where)
{
    std::wstring query = L"select " + columns + L" from [" + tableName + (where.empty() ? L"]" : L"] where " + where);

    web::json::value ans = ExecuteTableReaderJSON(query);
    ans[U("ColumnsInfo")] = GetColumnsInformationJSON(tableName);

    return ans;
}

web::json::value DBWorker::DeleteRow(const std::wstring &tableName, const std::wstring &pkValue)
{
    auto info = tic.getTables().find(tableName);
    if (info != tic.getTables().end())
    {
        std::wstring &pkName = info->second.pkName;
        std::wstring query = L"delete from [" + tableName + L"] where [" + pkName + L"] = \"" + pkValue + L"\"";
        return ExecuteNonQueryJSON(query);
    }
    else
    {
        errorStack.push(L"Cant find table information. Table name: " + tableName);
    }

    return  web::json::value();
}

web::json::value DBWorker::AddRow(const std::wstring &tableName, web::json::array &columns, web::json::array &row)
{
    web::json::value ans;

    auto info = tic.getTables().find(tableName);
    if (info != tic.getTables().end())
    {
        std::wstring query = L"insert into [" + tableName + L"] (";

        for (auto el : columns)
            query += el.as_string() + L",";

        query.pop_back();
        query += L") values(";

        auto &columnsInfo = info->second.columns;

        bool tErrorAcc = false;
        std::wstring col;
        std::wstring typeExp;

        for (int i = 0; i < row.size(); i++)
        {
            const auto &el = row[i];
            std::wstring columnName = columns[i].as_string();

            auto &currColumn = columnsInfo[columnName];

            std::wstring val = el.as_string();

            if (!TypeChecker::hasType(val, currColumn.type))
            {
                tErrorAcc = true;
                col = columnName;
                typeExp = currColumn.ColumnType_str();
                break;
            }

            query += L"cast(\'" + val + L"\' as " + currColumn.sqliteType + L"),";
        }

        query.pop_back();
        query += L")";

        if (tErrorAcc)
        {
            errorStack.push(L"Type Error. Type: " + typeExp + L" Column: " + col);
        }
        else
        {
            ans = ExecuteNonQueryJSON(query);
        }
    }
    else
        errorStack.push(L"Cant find table information. Table name: " + tableName);

    return ans;
}

web::json::value DBWorker::EditRow(const std::wstring &tableName, web::json::array &columns, web::json::array &row, const std::wstring &keyValue)
{
    web::json::value ans;

    auto info = tic.getTables().find(tableName);
    if (info != tic.getTables().end())
    {
        std::wstring query = L"update [" + tableName + L"] set ";

        auto &columnsInfo = info->second.columns;

        bool tErrorAcc = false;
        std::wstring col;
        std::wstring typeExp;

        for (int i = 0; i < row.size(); i++)
        {
            const auto &el = row[i];
            std::wstring columnName = columns[i].as_string();

            auto &currColumn = columnsInfo[columnName];

            std::wstring val = el.as_string();

            if (!TypeChecker::hasType(val, currColumn.type))
            {
                tErrorAcc = true;
                col = columnName;
                typeExp = currColumn.ColumnType_str();
                break;
            }

            query += columnName + L" = " + L"cast(\'" + val + L"\' as " + currColumn.sqliteType + L"),";
        }

        query.pop_back();
        query += L" where [" + info->second.pkName + L"] = \'" + keyValue + L"\'";

        if (tErrorAcc)
        {
            errorStack.push(L"Type Error. Type: " + typeExp + L" Column: " + col);
        }
        else
        {
            ans = ExecuteNonQueryJSON(query);
        }
    }
    else
        errorStack.push(L"Cant find table information. Table name: " + tableName);

    return ans;
}

bool DBWorker::isErrorOccured()
{
    return errorStack.size();
}

std::stack<std::wstring>& DBWorker::getErrorStack()
{
    return errorStack;
}
