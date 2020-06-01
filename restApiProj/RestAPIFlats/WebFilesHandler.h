#pragma once

#include <cpprest/json.h>
#include <cpprest/http_listener.h>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace web::http;

//нужно же как-то передавать странички в браузер, этот класс обрабатывает запрос на получение ресурса
//html, js, css и по большей части все, так как картинки починить не получилось, такой ресурс, к сожалению, не получить

class WebFilesHandler
{
	http_response response;
	std::wstring filePath;
	std::wstring content_type;

	//парсит путь к ресурсу
	void ParsePath();
	//читает файл
	void ReadFile();
	//строит ответ, который будет отправлен на клиент
	void BuildResponse();

public:
	WebFilesHandler(const std::wstring& filePath);

	http_response getResponse();
};

