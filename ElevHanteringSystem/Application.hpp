#pragma once
#include "WebServer.hpp"
#include "SFML/System/Thread.hpp"

class Application {
public:
	Application();
	virtual ~Application();
	void run();

	WebServer* GetWebServer();
private:
	WebServer* m_ws;
	sf::Thread* m_thread;
};

