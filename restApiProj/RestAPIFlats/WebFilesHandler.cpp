#include "WebFilesHandler.h"

static const std::map<std::wstring, std::wstring> extensionToMIME = {
	{L"html", L"text/html"},
	{L"htm", L"text/html"},
	{L"css", L"text/css"},
	{L"js", L"text/javascript"},
	{L"apng", L"image/apng"},
	{L"png", L"image/png"},
	{L"jpeg", L"image/jpeg"},
	{L"jpg", L"image/jpeg"},
};

void WebFilesHandler::BuildResponse()
{
	ParsePath();
	if (response.status_code() != 200) return;
	ReadFile();
	wprintf(L"%ws\n",content_type.c_str());
}

void WebFilesHandler::ParsePath()
{
	int dotPos = filePath.find_last_of(L'.');
	if (dotPos == -1) { response.set_status_code(404); return; }

	std::wstring extension = filePath.substr(dotPos+1);

	auto temp = extensionToMIME.find(extension);

	if(temp!=extensionToMIME.end())
	content_type = temp->second;
	else
	content_type = L"text/plain";
	
}

void WebFilesHandler::ReadFile()
{
	std::wifstream f;
	f.open(filePath);

	std::wstring str;
	if (f.good()) {
		std::wstringstream ss;
		ss << f.rdbuf();
		str = ss.str();

		response.set_body(str, content_type.c_str());
	}
	else
	{
		response.set_status_code(404);
	}

}

WebFilesHandler::WebFilesHandler(const std::wstring& filePath)
	:response(200)
{
	this->filePath = filePath;
	BuildResponse();
}

http_response WebFilesHandler::getResponse()
{
	return response;
}
