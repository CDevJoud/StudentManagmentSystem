#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/System/Thread.hpp>
#include <map>
#include "Http.hpp"
#ifdef _DEBUG
constexpr auto WEBSERVER_PORT = 8080ui16;
#else
constexpr auto WEBSERVER_PORT = 80ui16;
#endif // _DEBUG

struct Client {
	~Client() {
		socket.disconnect();
		/*thread->terminate();*/
	}
	sf::Thread* thread = nullptr;
	sf::TcpSocket socket;
};

class WebServer {
public:
	WebServer(const sf::IpAddress& IpAddress, const sf::Int16 Port = WEBSERVER_PORT);

	virtual ~WebServer();

	bool RunService();

	bool IsRunning() const;

	sf::IpAddress GetIpAddress() const;
	sf::TcpListener& GetListener();

	void Terminate();

	int HandleClient(Client* client);
	void HandleGetMethod(Http& req, Client* client);
	void HandleGetDashboard(Http& req, Client* client);

	void HandlePostMethod(Http& req, Client* client);
	void HandlePostLogin(Http& req, Client* client);
	void HandlePostLogout(Http& req, Client* client);
	void HandlePostSave(Http& req, Client* client);
private:
	std::map<std::string, std::string> routing;
	bool m_bIsRunning;
	sf::IpAddress m_IpAddress;
	sf::TcpListener m_listener;
};

