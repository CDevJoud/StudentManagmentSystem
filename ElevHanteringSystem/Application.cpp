#define NOMINMAX
#include <Windows.h>
#include "Application.hpp"
#include "SFML/System.hpp"
#include "nlohmann/json.hpp"
#include <iostream>
#include "SharedData.hpp"

static void SaveToFile(const std::string& fName, const std::string& data) {
	HANDLE hFile = CreateFileA(fName.c_str(), GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	DWORD w = 0;
	WriteFile(hFile, data.c_str(), data.size(), &w, nullptr);
	CloseHandle(hFile);
}

static std::string LoadFile(const std::string& fName) {
	HANDLE hFile = CreateFileA(fName.c_str(), GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	DWORD w = 0;
	LARGE_INTEGER size;
	GetFileSizeEx(hFile, &size);
	std::string data(size.QuadPart, 0);

	ReadFile(hFile, data.data(), size.QuadPart, &w, nullptr);
	CloseHandle(hFile);
	return data;
}

Application::Application() : 
	m_ws(nullptr) {
	auto fIn = sf::FileInputStream();
	auto ip = std::string("127.0.0.1");
	auto port = WEBSERVER_PORT;
	if (fIn.open("config.json")) {
		auto fileData = std::string();
		fileData.resize(fIn.getSize(), 0);
		fIn.read(fileData.data(), fIn.getSize());

		g.config = nlohmann::json::parse(fileData);
		if (g.config["WebServer"].is_object()) {
			auto& JWS = g.config["WebServer"]; // JSON WebServer

			if (JWS["Ip"].is_string()) {
				ip = JWS["Ip"];
			}
			if (JWS["Port"].is_number()) {
				port = JWS["Port"];
			}
			if (JWS["Proxy"].is_object()) {
				if (!JWS["Proxy"]["Enabled"]) {
					JWS["Proxy"]["Ip"] = ip;
					JWS["Proxy"]["Port"] = port;
				}
			}
		}
	}
	auto fIndb = sf::FileInputStream();
	if (fIndb.open("db.json")) {
		auto fileData = std::string();
		fileData.resize(fIndb.getSize(), 0);
		fIndb.read(fileData.data(), fileData.size());

		g.db = nlohmann::json::parse(fileData);
	}

	this->m_ws = new WebServer(ip, port);
	this->m_thread = new sf::Thread(std::bind(&WebServer::RunService, this->m_ws));
	this->m_thread->launch();
}

Application::~Application() {
	delete Application::GetWebServer();
	delete this->m_thread;
}

void Application::run() {
	while (this->m_ws->IsRunning()) {
		std::string cmd;
		std::getline(std::cin, cmd, '\n');
		if (cmd == "stop") {
			SaveToFile("config.json", g.config.dump(2, ' '));
			SaveToFile("db.json", g.db.dump(2, ' '));
			
			this->m_ws->Terminate();
			
			this->m_thread->terminate();
			
			
		} else if (cmd == "update --config") {
			g.config = nlohmann::json::parse(LoadFile("config.json"));
			std::cout << "Config File Updated!" << std::endl;
		} else if (cmd == "update --db") {
			g.db = nlohmann::json::parse(LoadFile("db.json"));
			std::cout << "DataBase File Updated!" << std::endl;
		} else if (cmd=="save --db") {
			SaveToFile("db.json", g.db.dump());
			std::cout << "DataBase File Saved!" << std::endl;
		}
	}
}

WebServer* Application::GetWebServer() {
	return this->m_ws;
}
