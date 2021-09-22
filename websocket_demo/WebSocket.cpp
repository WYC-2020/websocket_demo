#include "WebSocket.h"

#pragma comment(lib,"libeay32.lib")
#pragma comment(lib,"ssleay32.lib")

WebSocket::WebSocket()
{
	// Set logging to be pretty verbose (everything except message payloads)
	m_client.set_access_channels(websocketpp::log::alevel::all);
	m_client.clear_access_channels(websocketpp::log::alevel::frame_payload);

	// Initialize ASIO
	m_client.init_asio();
	m_client.set_reuse_addr(true);

	// Register our message handler
	m_client.set_message_handler(websocketpp::lib::bind(&WebSocket::on_message,this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
	m_client.set_open_handler(websocketpp::lib::bind(&WebSocket::on_open, this, websocketpp::lib::placeholders::_1));
	m_client.set_close_handler(websocketpp::lib::bind(&WebSocket::on_close, this, websocketpp::lib::placeholders::_1));
	m_client.set_fail_handler(websocketpp::lib::bind(&WebSocket::on_fail, this, websocketpp::lib::placeholders::_1));
}

WebSocket::~WebSocket()
{
	Stop();
}

void WebSocket::Run()
{
	while (!m_bExit)
	{
		m_client.run_one();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

bool WebSocket::Start(const std::string& strUri)
{
	m_strUrl = strUri;
	websocketpp::lib::error_code ec;
	m_connection_ptr = m_client.get_connection(strUri, ec);
	if (!m_connection_ptr || ec)
		return false;

	m_client.connect(m_connection_ptr);

	m_bExit = false;
	m_thread = std::thread(std::bind(&WebSocket::Run, this));

	return m_thread.joinable();
}

bool WebSocket::Stop()
{
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_bReady = true;
		m_cv.notify_all();
	}

	if (m_connection_ptr&&m_connection_ptr->get_state() == websocketpp::session::state::value::open)
	{
		m_connection_ptr->close(websocketpp::close::status::going_away, "");
	}

	m_bExit = true;
	if(m_thread.joinable())
		m_thread.join();

	return true;
}

bool WebSocket::SendTextData(const char* pText)
{
	try
	{
		return m_connection_ptr->send(pText) ? false : true;
	}
	catch (websocketpp::exception const & e)
	{
		std::cout << e.what() << std::endl;
	}
	return false;
}

bool WebSocket::SendBinaryData(const char* pData, const int nSize)
{
	try
	{
		return m_connection_ptr->send(pData, nSize, websocketpp::frame::opcode::binary) ? false : true;
	}
	catch (websocketpp::exception const & e)
	{
		std::cout << e.what() << std::endl;
	}
	return false;
}

std::string WebSocket::Wait(int nSencond)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (!m_bReady)
	{
		if (nSencond)
		{
			if (m_cv.wait_for(lock, std::chrono::seconds(nSencond)) == std::cv_status::timeout)
				break;
		}
		else
		{
			m_cv.wait(lock);
		}
	}
	m_bReady = false;
	return m_strRecvMsg;
}

void WebSocket::on_message(websocketpp::connection_hdl hdl, message_ptr msg)
{
	//std::cout << "on_message called with hdl: " << hdl.lock().get()<< " and message: " << msg->get_payload()<< std::endl;
	std::unique_lock<std::mutex> lock(m_mutex);
	m_strRecvMsg = msg->get_payload();

	m_bReady = true;
	m_cv.notify_all();

}

void WebSocket::on_open( websocketpp::connection_hdl hdl)
{
}

void WebSocket::on_close( websocketpp::connection_hdl hdl)
{
	if (m_connection_ptr&&m_connection_ptr->get_state() == websocketpp::session::state::value::open)
	{
		m_connection_ptr->close(websocketpp::close::status::going_away, "");
	}

	websocketpp::lib::error_code ec;
	m_connection_ptr = m_client.get_connection(m_strUrl, ec);

	m_client.connect(m_connection_ptr);
}

void WebSocket::on_fail(websocketpp::connection_hdl hdl)
{
	if (m_connection_ptr&&m_connection_ptr->get_state() == websocketpp::session::state::value::open)
	{
		m_connection_ptr->close(websocketpp::close::status::going_away, "");
	}

	websocketpp::lib::error_code ec;
	m_connection_ptr = m_client.get_connection(m_strUrl, ec);

	m_client.connect(m_connection_ptr);
}

WebSocket_tls::WebSocket_tls()
{
	// Set logging to be pretty verbose (everything except message payloads)
	m_client.set_access_channels(websocketpp::log::alevel::all);
	m_client.clear_access_channels(websocketpp::log::alevel::frame_payload);

	// Initialize ASIO
	m_client.init_asio();
	m_client.set_reuse_addr(true);

	// Register our message handler
	m_client.set_message_handler(websocketpp::lib::bind(&WebSocket_tls::on_message, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
	m_client.set_open_handler(websocketpp::lib::bind(&WebSocket_tls::on_open, this, websocketpp::lib::placeholders::_1));
	m_client.set_close_handler(websocketpp::lib::bind(&WebSocket_tls::on_close, this, websocketpp::lib::placeholders::_1));
	m_client.set_fail_handler(websocketpp::lib::bind(&WebSocket_tls::on_fail, this, websocketpp::lib::placeholders::_1));
	m_client.set_tls_init_handler(websocketpp::lib::bind(&WebSocket_tls::on_tls_init, this, websocketpp::lib::placeholders::_1));
}

WebSocket_tls::~WebSocket_tls()
{
	Stop();
}

bool WebSocket_tls::Start(const std::string& strUri)
{
	m_strUrl = strUri;
	websocketpp::lib::error_code ec;
	m_connection_ptr = m_client.get_connection(strUri, ec);
	if (!m_connection_ptr || ec)
		return false;

	m_client.connect(m_connection_ptr);

	m_bExit = false;
	m_thread = std::thread(std::bind(&WebSocket_tls::Run, this));

	return m_thread.joinable();
}

bool WebSocket_tls::Stop()
{
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_bReady = true;
		m_cv.notify_all();
	}

	if (m_connection_ptr&&m_connection_ptr->get_state() == websocketpp::session::state::value::open)
	{
		m_connection_ptr->close(websocketpp::close::status::going_away, "");
	}

	m_bExit = true;
	m_thread.join();

	return true;
}

bool WebSocket_tls::SendTextData(const char* pText)
{
	try
	{
		return m_connection_ptr->send(pText) ? false : true;
	}
	catch (websocketpp::exception const & e)
	{
		std::cout << e.what() << std::endl;
	}

	return false;
}

bool WebSocket_tls::SendBinaryData(const char* pData, const int nSize)
{
	try
	{
		return m_connection_ptr->send(pData, nSize, websocketpp::frame::opcode::binary) ? false : true;
	}
	catch (websocketpp::exception const & e)
	{
		std::cout << e.what() << std::endl;
	}

	return false;
}

std::string WebSocket_tls::Wait(int nSencond)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (!m_bReady)
	{
		if (nSencond)
		{
			if(m_cv.wait_for(lock, std::chrono::seconds(nSencond)) == std::cv_status::timeout)
				break;
		}
		else
		{
			m_cv.wait(lock);
		}
	}

	m_bReady = false;

	return m_strRecvMsg;
}

void WebSocket_tls::on_message(websocketpp::connection_hdl hdl, message_ptr msg)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_strRecvMsg = msg->get_payload();

	m_bReady = true;
	m_cv.notify_all();
}

void WebSocket_tls::on_open(websocketpp::connection_hdl hdl)
{

}

void WebSocket_tls::on_close(websocketpp::connection_hdl hdl)
{
	if (m_connection_ptr&&m_connection_ptr->get_state() == websocketpp::session::state::value::open)
	{
		m_connection_ptr->close(websocketpp::close::status::going_away, "");
	}

	websocketpp::lib::error_code ec;
	m_connection_ptr = m_client.get_connection(m_strUrl, ec);

	m_client.connect(m_connection_ptr);
}

void WebSocket_tls::on_fail(websocketpp::connection_hdl hdl)
{
	if (m_connection_ptr&&m_connection_ptr->get_state() == websocketpp::session::state::value::open)
	{
		m_connection_ptr->close(websocketpp::close::status::going_away, "");
	}
}

void WebSocket_tls::Run()
{
	while (!m_bExit)
	{
		m_client.run_one();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

websocketpp::lib::shared_ptr<boost::asio::ssl::context> WebSocket_tls::on_tls_init(websocketpp::connection_hdl)
{
	websocketpp::lib::shared_ptr<boost::asio::ssl::context> ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv1);

	try {
		ctx->set_options(boost::asio::ssl::context::default_workarounds |
			boost::asio::ssl::context::no_sslv2 |
			boost::asio::ssl::context::no_sslv3 |
			boost::asio::ssl::context::single_dh_use);
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return ctx;
}
