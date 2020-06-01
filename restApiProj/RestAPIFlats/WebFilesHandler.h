#pragma once

#include <cpprest/json.h>
#include <cpprest/http_listener.h>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace web::http;

class WebFilesHandler
{
	http_response response;
	std::wstring filePath;
	std::wstring content_type;

	void ParsePath();
	void ReadFile();
	void BuildResponse();

public:
	WebFilesHandler(const std::wstring& filePath);

	http_response getResponse();
};

