#include "Http.hpp"
#include <sstream>
#include <iostream>

Http::Http() {
	this->m_bin.resize(4096, 0);
}

void Http::RecvRequest(sf::TcpSocket& socket) {
	sf::Uint64 r = 0;
	socket.receive(this->m_bin.data(), this->m_bin.size(), r);

	ManageRequest();
}

void Http::ManageRequest() {
	size_t headerEnd = this->m_bin.find("\r\n\r\n");
	if (headerEnd == std::string::npos) {
		std::cout << "Invalid HTTP request: No header-body separator found.\n";
		return;
	}

	std::istringstream iss(this->m_bin.substr(0, headerEnd));  // Parse headers separately
	iss >> method >> url >> version;

	std::string line;
	size_t contentLength = 0;

	while (std::getline(iss, line, '\n')) {
		if (!line.empty() && line.back() == '\r') line.pop_back();

		std::string fieldName, fieldValue;
		auto strpos = line.find(':');
		if (strpos != std::string::npos) {
			fieldName = line.substr(0, strpos);
			fieldValue = line.substr(strpos + 2);  // Skip ": "

			this->m_fields[fieldName] = fieldValue;

			if (fieldName == "Content-Length") {
				contentLength = std::stoi(fieldValue);
			}
		}
	}

	// Extract the body correctly
	if (method == "POST" && contentLength > 0) {
		this->m_body = this->m_bin.substr(headerEnd + 4, contentLength);
	}
}

void Http::SendResponse(sf::TcpSocket& socket) {
	std::ostringstream oss;

	oss << version << " " << std::to_string(respCode) << " " << respMsg << "\r\n";

	for (auto& [name, value] : this->m_fields) {
		oss << name << ": " << value << "\r\n";
	}

	oss << "\r\n";

	oss << this->m_body;

	auto resp = oss.str();
	socket.send(resp.data(), resp.size());
}

