#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <cpprest/http_listener.h>
#include <cpprest/json.h>

#include "DBWorker.h"
#include "WebFilesHandler.h"

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

static DBWorker dbw(L"base.db");

//ну тут методы понятны, это все rest
//немного о том как вообще работает система

/*
при запуске считывается информация о таблицах, первичные ключи, внешние ключи и тд
на самом деле система универсальна, и может работать с любыми sqlite базами, фронт тоже заточен под работу с любыми данными
сервер работает очень просто
сервер обрабатывает запросы
если это get запрос, то мы проверяем, является ли первым ресурсом _bd, если да, то это работа с базой данных
если же это что-то другое, то это запрос на получение ресурса, по типу странички или скрипта js
остальные запросы работают только, если первый ресурс _bd
в зависимости от запроса, формат url может отличаться
все запросы работают с классом DBWorker через соответствующие методы
проверка типов работает только для изменения и добавления записей
все взаимодействие с внешним миром через JSON
надеюсь я достаточно хорошо изложил все))
*/

void handle_get(http_request message)
{
    http_response response;

    auto temp = message.absolute_uri().path().substr(1);

    auto testLol = message.absolute_uri().split_path(temp);

    if (testLol.size() != 0)
    {
        if (testLol[0] == L"_bd")
        {
            json::value ans = dbw.GetTableJSON(L"*",testLol[1]);

            if (dbw.isErrorOccured())
            {
                std::wstring errs;
                auto& stack = dbw.getErrorStack();
                while (!stack.empty())
                {
                    errs += stack.top() + L'\n';
                    stack.pop();
                }
                ans[U("Error")] = json::value::string(errs);
            }
                

            response.set_body(ans);
            response.set_status_code(200);
        }
        else
        {
            WebFilesHandler test(L"site/"+temp);

            response = test.getResponse();
        }
    }

    message.reply(response);
}

void handle_post(http_request message)
{
    http_response response(200);

    json::value ans;

    auto temp = message.absolute_uri().path().substr(1);

    auto testLol = message.absolute_uri().split_path(temp);

    if (testLol.size() <= 2 && testLol[0] == L"_bd")
    {
        auto innerJsonVal = json::value::parse(message.extract_string().get());

        auto& columnArray = innerJsonVal[U("columns")].as_array();
        auto& rowArray = innerJsonVal[U("row")].as_array();

        ans = dbw.AddRow(testLol[1], columnArray, rowArray);

        if (dbw.isErrorOccured())
        {
            std::wstring errs;
            auto& stack = dbw.getErrorStack();
            while (!stack.empty())
            {
                errs += stack.top() + L'\n';
                stack.pop();
            }
            ans[U("Error")] = json::value::string(errs);
        }

        response.set_body(ans);
        response.set_status_code(200);
    }
    else
    {
        ans[U("Error")] = json::value::string(L"Wrong format");

        response.set_body(ans);
        response.set_status_code(404);
    }


    message.reply(response);
}

void handle_delete(http_request message)
{
    http_response response;

    auto temp = message.absolute_uri().path().substr(1);

    auto testLol = message.absolute_uri().split_path(temp);

    if (testLol.size() <= 3 && testLol[0] == L"_bd")
    {
        json::value ans = dbw.DeleteRow(testLol[1], testLol[2]);

        if (dbw.isErrorOccured())
        {
            std::wstring errs;
            auto& stack = dbw.getErrorStack();
            while (!stack.empty())
            {
                errs += stack.top() + L'\n';
                stack.pop();
            }
            ans[U("Error")] = json::value::string(errs);
        }

        response.set_body(ans);
        response.set_status_code(200);
    }
    else
    {
        json::value ans;

        ans[U("Error")] = json::value::string(L"Wrong format");

        response.set_body(ans);
        response.set_status_code(404);
    }
    

    message.reply(response);
}

void handle_patch(http_request message)
{
    http_response response(200);

    auto temp = message.absolute_uri().path().substr(1);

    auto testLol = message.absolute_uri().split_path(temp);

    if (testLol.size() <= 3 && testLol[0] == L"_bd")
    {
        auto innerJsonVal = json::value::parse(message.extract_string().get());

        auto &columnArray = innerJsonVal[U("columns")].as_array();
        auto &rowArray = innerJsonVal[U("row")].as_array();

        json::value ans = dbw.EditRow(testLol[1], columnArray, rowArray, testLol[2]);

        if (dbw.isErrorOccured())
        {
            std::wstring errs;
            auto& stack = dbw.getErrorStack();
            while (!stack.empty())
            {
                errs += stack.top() + L'\n';
                stack.pop();
            }
            ans[U("Error")] = json::value::string(errs);
        }

        response.set_body(ans);
        response.set_status_code(200);
    }
    else
    {
        json::value ans;

        ans[U("Error")] = json::value::string(L"Wrong format");

        response.set_body(ans);
        response.set_status_code(404);
    }


    message.reply(response);
}


int main(int argc, char* argv[])
{
    dbw.ExecuteNonQuery(L"PRAGMA foreign_keys = ON;");
    if (dbw.isErrorOccured())
    {
        std::wstring errs;
        auto& stack = dbw.getErrorStack();
        while (!stack.empty())
        {
            errs += stack.top() + L'\n';
            stack.pop();
        }
        wprintf(errs.c_str());
    }

    http_listener httpListener_port80(L"http://localhost");

    httpListener_port80.support(methods::GET, handle_get);
    httpListener_port80.support(methods::POST, handle_post);
    httpListener_port80.support(methods::DEL, handle_delete);
    httpListener_port80.support(methods::PATCH, handle_patch);
    
    try
    {
        httpListener_port80
            .open()
            .then([&httpListener_port80]() { printf("Server is on\n"); })
            .wait();

        std::cin.get();
    }
    catch (...)
    {
        printf("Cant start the server\n");
    }

    return 0;
}