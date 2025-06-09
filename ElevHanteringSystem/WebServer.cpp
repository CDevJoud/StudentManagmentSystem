#include "WebServer.hpp"
#include <SFML/System/Sleep.hpp>

#include <iostream>
#include <functional>
#include "SharedData.hpp"
#include <SFML/System/FileInputStream.hpp>
#include "inja.hpp"

/*nlohmann::json GetUserByUsername(sqlite3* db, const std::string& name) {
    sqlite3_stmt* stmt;
    nlohmann::json userJson;

    const char* query = "SELECT * FROM Users WHERE Username = ?;";
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            userJson["UserID"] = sqlite3_column_int(stmt, 0);
            userJson["Username"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            userJson["Email"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            userJson["Password"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            userJson["LoggedIn"] = sqlite3_column_int(stmt, 4);
            userJson["Session"] = sqlite3_column_int(stmt, 5);
        }
    }

    sqlite3_finalize(stmt);
    return userJson;
}*/

static nlohmann::json* GetUsernameFromDB(const std::string& name) {
	if (g.db["Users"].is_array()) {
		auto& users = g.db["Users"];
		for (auto& user : users) {
			if (user["Username"] == name) {
				return &user;
			}
		}
	}
	return nullptr;
}

static nlohmann::json* GetUserSessionFromDB(const int& sessionID) {
	if (g.db["Users"].is_array()) {
		auto& users = g.db["Users"];
		for (auto& user : users) {
			if (user["Session"] == sessionID) {
				return &user;
			}
		}
	}
	return nullptr;
}

WebServer::WebServer(const sf::IpAddress& IpAddress, const sf::Int16 Port) : 
	m_bIsRunning(false) {
	auto status = WebServer::GetListener().listen(Port, IpAddress);
	if (status == sf::Socket::Done) {
		std::cout << "WebServer Running on " + IpAddress.toString() + ":" + std::to_string(Port) << std::endl;

		if (g.config["WebServer"].is_object()) {
			auto& cfgWS = g.config["WebServer"];
			if (cfgWS["Routing"].is_object()) {
				for (auto& i : cfgWS["Routing"].items()) {
					this->routing[i.key()] = i.value();
				}
			}
		}
		this->m_bIsRunning = true;
	} else {
		std::cout << "Could Not listen to the following port: " + IpAddress.toString() + ":" + std::to_string(Port) << std::endl;
	}
}

WebServer::~WebServer() {
	WebServer::GetListener().close();
}

bool WebServer::RunService() {
	while (WebServer::IsRunning()) {
		auto client = new Client;
		if (WebServer::GetListener().accept(client->socket) != sf::Socket::Done) {
			delete client;
			continue;
		}
		else {
			std::cout << "New Client Connected " << client->socket.getRemoteAddress().toString() << ":" << client->socket.getRemotePort() << std::endl;
			client->thread = new sf::Thread(std::bind(&WebServer::HandleClient, this, client));
			client->thread->launch();
		}
	}
	return true;
}

bool WebServer::IsRunning() const {
	return this->m_bIsRunning;
}

sf::IpAddress WebServer::GetIpAddress() const {
	return this->m_IpAddress;
}

sf::TcpListener& WebServer::GetListener() {
	return this->m_listener;
}

void WebServer::Terminate() {
	this->m_bIsRunning = false;
}

int WebServer::HandleClient(Client* client) {
	Http req;
	req.RecvRequest(client->socket);
	if (req.GetMethod() == "GET") {
		WebServer::HandleGetMethod(req, client);
	} else if (req.GetMethod() == "POST") {
		WebServer::HandlePostMethod(req, client);
	}
	std::cout << "Client " << client->socket.getRemoteAddress() << ":" << client->socket.getRemotePort() << " served!" << std::endl;
	client->socket.disconnect();
	delete client;
	return 0;
}

void WebServer::HandleGetMethod(Http& req, Client* client) {
	std::string url = (req.GetURL() == "/" ? "/" : req.GetURL().data() + 1);
	if (this->routing.count(url)) {
		std::cout << "Client " << client->socket.getRemoteAddress() << ":" << client->socket.getRemotePort() << " requested " << this->routing[url] << std::endl;
		

		if (url == "dashboard") {
			WebServer::HandleGetDashboard(req, client);
		}
		else {
			inja::Environment env;
			Http res;
			res.SetVersion("HTTP/1.1");
			res.SetResponseCode(200, "OK!");
			sf::FileInputStream fIn;
			if (fIn.open(this->routing[url])) {
				std::string data(fIn.getSize(), 0);
				fIn.read(data.data(), fIn.getSize());
				res.SetBody(env.render(data, g.config));
				res.SendResponse(client->socket);
			}
		}
	}
	else {
		inja::Environment env;
		sf::FileInputStream fIn;
		if (fIn.open("404.html")) {
			std::string templateStr(fIn.getSize(), 0);
			fIn.read(templateStr.data(), fIn.getSize());

			Http res;
			res.SetVersion("HTTP/1.1");
			res.SetResponseCode(404, "File Not Found!");
			nlohmann::json data;
			data["file"]["name"] = req.GetURL();

			auto sessionvalue = req.GetField("Cookie");
			if(!sessionvalue.empty())
			{
				auto sessionID = std::stoi(sessionvalue.substr(sessionvalue.find_first_of('=') + 1));
				auto& user = *GetUserSessionFromDB(sessionID);

				if (user["LoggedIn"]) {
					data["destination"]["name"] = "dashboard";
					data["destination"]["file"] = "/dashboard";
				}
				else {
					data["destination"]["name"] = "login";
					data["destination"]["file"] = "/";
				}

				res.SetBody(env.render(templateStr, data));
				res.SendResponse(client->socket);
			}
		}
	}
}

void WebServer::HandleGetDashboard(Http& req, Client* client) {
	Http res;
	inja::Environment env;
	sf::FileInputStream fIn;
	
	auto sessionvalue = req.GetField("Cookie");
	auto sessionID = std::stoi(sessionvalue.substr(sessionvalue.find_first_of('=') + 1));

	auto& user = *GetUserSessionFromDB(sessionID);
	if(user["LoggedIn"]) {
		res.SetVersion("HTTP/1.1");
		res.SetResponseCode(200, "OK!");

		if (fIn.open("dashboard.html")) {
			std::string page(fIn.getSize(), 0);
			fIn.read(page.data(), fIn.getSize());
			nlohmann::json updatedTemplateData = user;
			updatedTemplateData["WebServer"]["Proxy"]["Ip"] = g.config["WebServer"]["Proxy"]["Ip"];
			updatedTemplateData["WebServer"]["Proxy"]["Port"] = g.config["WebServer"]["Proxy"]["Port"];


			auto htmlPage = env.render(page, updatedTemplateData);
			res.SetBody(htmlPage);
			res.SendResponse(client->socket);
		}
	} else {
		res.SetVersion("HTTP/1.1");
		res.SetResponseCode(401, "Unauthorized!");

		if (fIn.open("unauth.html")) {
			std::string page(fIn.getSize(), 0);
			fIn.read(page.data(), fIn.getSize());

			res.SetBody(page);
			res.SendResponse(client->socket);
		}
	}
}

void WebServer::HandlePostMethod(Http& req, Client* client) {
	auto url = req.GetURL();
	if (url == "/login") {
		WebServer::HandlePostLogin(req, client);
	}
	else if (url == "/logout") {
		WebServer::HandlePostLogout(req, client);
	}
	else if (url == "/save") {
		WebServer::HandlePostSave(req, client);
	}
}

void WebServer::HandlePostLogin(Http& req, Client* client) {
	const auto sendFailedLogin = [&]() {
		Http res;
		res.SetField("Content-Type", "application/json");
		res.SetResponseCode(401, "Unauthorized");
		res.SetVersion("HTTP/1.1");
		nlohmann::json jRes;
		jRes["success"] = false;
		res.SetBody(jRes.dump());
		res.SendResponse(client->socket);
		};
	const auto sendSucceedLogin = [&](const int& usersessionID) {
		Http res;
		res.SetField("Content-Type", "application/json");
		res.SetResponseCode(200, "OK!");
		res.SetVersion("HTTP/1.1");
		nlohmann::json jRes;
		jRes["success"] = true;
		res.SetField("Set-Cookie", "session=" + std::to_string(usersessionID) + ";");
		res.SetBody(jRes.dump());
		res.SendResponse(client->socket);
		};
	const auto handleUserLogin = [&](const std::string& username, const std::string& password) {
		auto user = GetUsernameFromDB(username);
		if (user == nullptr) {
			sendFailedLogin();
		}
		else {
			if (password == (*user)["Password"]) {
				(*user)["LoggedIn"] = true;
				sendSucceedLogin((*user)["Session"]);
			}
			else {
				sendFailedLogin();
			}
		}
		};

	auto field = req.GetField("Content-Type");
	if (req.GetField("Content-Type") == "application/json") {
		auto bd = req.GetBody();
		bd.erase(std::find(bd.begin(), bd.end(), '\0'), bd.end());
		if (!bd.empty() && bd[0] != 0) {
			try {
				nlohmann::json data = nlohmann::json::parse(bd);
				handleUserLogin(data["username"], data["password"]);
			}
			catch (const nlohmann::json::parse_error& e) {
				std::cerr << "JSON Parse Error: " << e.what() << "\n";
			}
		}
	}
}

void WebServer::HandlePostLogout(Http& req, Client* client) {
	auto field = req.GetField("Content-Type");
	if (field == "application/json") {
		auto bd = req.GetBody();
		bd.erase(std::find(bd.begin(), bd.end(), 0), bd.end());
		if (!bd.empty() && bd[0] != 0) {
			try {
				nlohmann::json data = nlohmann::json::parse(bd);
				if (data["logout"].is_boolean()) {
					bool bLogOut = data["logout"];
					if (bLogOut) {
						auto sessionvalue = req.GetField("Cookie");
						auto sessionID = std::stoi(sessionvalue.substr(sessionvalue.find_first_of('=') + 1));

						auto& user = *GetUserSessionFromDB(sessionID);

						user["LoggedIn"] = false;

						Http res;
						res.SetField("Content-Type", "application/json");
						res.SetResponseCode(200, "OK!");
						res.SetVersion("HTTP/1.1");
						nlohmann::json jRes;
						jRes["ok"] = true;
						//res.SetField("Set-Cookie", "session=0");
						res.SetBody(jRes.dump());
						res.SendResponse(client->socket);
					}
				}
			}
			catch (const nlohmann::json::parse_error& e) {
				std::cerr << "JSON Parse Error: " << e.what() << std::endl;
			}
		}
	}
}

void WebServer::HandlePostSave(Http& req, Client* client) {
	auto field = req.GetField("Content-Type");
	if (field == "application/json") {
		auto bd = req.GetBody();
		bd.erase(std::find(bd.begin(), bd.end(), 0), bd.end());
		if (!bd.empty() && bd[0] != 0) {
			try {
				nlohmann::json data = nlohmann::json::parse(bd);

				auto session = req.GetField("Cookie");
				auto sessionID = std::stoi(session.substr(session.find_first_of('=') + 1));
				auto& user = *GetUserSessionFromDB(sessionID);

				user["Table"] = data;

				Http res;
				res.SetResponseCode(200, "OK!");
				res.SetField("Content-Type", "application/json");
				res.SetVersion("HTTP/1.1");
				nlohmann::json jRes;
				jRes["success"] = true;
				res.SetBody(jRes.dump());
				res.SendResponse(client->socket);
			}
			catch (const nlohmann::json::parse_error& e) {
				std::cerr << "JSON Parse Error: " << e.what() << std::endl;
			}
		}
	}
}
