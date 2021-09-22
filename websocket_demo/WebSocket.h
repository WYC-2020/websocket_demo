#pragma once
#include <websocketpp/config/asio_no_tls_client.hpp>
#include "websocketpp/config/asio_client.hpp"
#include <websocketpp/client.hpp>
class WebSocket
{
	typedef websocketpp::client<websocketpp::config::asio_client> client;
	typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

public:

	WebSocket();

	~WebSocket();

	bool Start(const std::string& strUri);

	bool Stop();

	bool SendTextData(const char* pText);

	bool SendBinaryData(const char* pData, const int nSize);

	std::string Wait(int nSencond);

private:

	void on_message(websocketpp::connection_hdl hdl, message_ptr msg);

	void on_open(websocketpp::connection_hdl hdl);

	void on_close( websocketpp::connection_hdl hdl);

	void on_fail( websocketpp::connection_hdl hdl);

	void Run();

private:
	client m_client;

	client::connection_ptr m_connection_ptr;

	std::thread  m_thread;

	bool   m_bExit{ false };

	std::string m_strUrl;

	std::mutex m_mutex;
	std::condition_variable m_cv;
	bool m_bReady{ false };

	std::string m_strRecvMsg;
};

class WebSocket_tls
{
	typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
	typedef websocketpp::config::asio_tls_client::message_type::ptr message_ptr;

public:

	WebSocket_tls();

	~WebSocket_tls();

	bool Start(const std::string& strUri);

	bool Stop();

	bool SendTextData(const char* pText);

	bool SendBinaryData(const char* pData, const int nSize);

	std::string Wait(int nSencond);
private:

	void on_message(websocketpp::connection_hdl hdl, message_ptr msg);

	void on_open(websocketpp::connection_hdl hdl);

	void on_close(websocketpp::connection_hdl hdl);

	void on_fail(websocketpp::connection_hdl hdl);

	void Run();

	websocketpp::lib::shared_ptr<boost::asio::ssl::context> on_tls_init(websocketpp::connection_hdl);

private:
	client m_client;

	client::connection_ptr m_connection_ptr;

	std::thread  m_thread;

	bool   m_bExit{ false };

	std::string m_strUrl;

	std::mutex m_mutex;
	std::condition_variable m_cv;
	bool m_bReady{ false };

	std::string m_strRecvMsg;
};