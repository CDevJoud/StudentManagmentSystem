#pragma once
#include <SFML/Network/TcpSocket.hpp>
#include <string>
#include <map>

class Http {
public:
	Http();
	void RecvRequest(sf::TcpSocket& socket);
	void ManageRequest();

	std::string GetMethod() const { return this->method; }
	std::string GetURL() const { return this->url; }
	std::string GetVersion() const { return this->version; }
	std::string GetBody() const { return this->m_body; }

	std::string GetField(const std::string& name) {
		if (this->m_fields.count(name)) { return this->m_fields[name]; }
		else { return std::string(); }
	}

	void Clear() {
		method.clear();
		url.clear();
		version.clear();
		m_bin.clear();
		m_body.clear();
		m_fields.clear();
	}
	
	void SetMethod(const std::string& method) { this->method = method; }
	void SetResponseCode(const sf::Uint16 code, const std::string& msg) { respCode = code; respMsg = msg; }
	void SetURL(const std::string& url) { this->url = url; }
	void SetVersion(const std::string& version) { this->version = version; }
	void SetBody(const std::string& body) { this->m_body = body; }
	void SetField(const std::string& name, const std::string& value) { this->m_fields[name] = value; }

	void SendResponse(sf::TcpSocket& socket);
private:
	std::string method, url, version;
	std::string m_bin;
	std::string m_body;
	std::map<std::string, std::string> m_fields;

	sf::Uint16 respCode;
	std::string respMsg;
};

