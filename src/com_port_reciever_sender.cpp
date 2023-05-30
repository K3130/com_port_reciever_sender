#include "com_port_reciever_sender.h"


bool cprs::ComPortRecieverSender::connect_to_port(CCHAR aPort)
{
	m_port.open(aPort, m_err);

	m_port.set_option(asio::serial_port_base::baud_rate(m_baudRate));
	m_port.set_option(asio::serial_port_base::character_size(m_dataBit));
	m_port.set_option(asio::serial_port_base::flow_control(m_flowControl));
	m_port.set_option(asio::serial_port_base::parity(m_parity));
	m_port.set_option(asio::serial_port_base::stop_bits(m_stopBits));
	
	//##### Start run() in thread #####
	read_data();
	//asio::thread thread(boost::bind(&asio::io_service::run, &m_io));
	m_thread = std::thread([this]() { m_io.run(); });

	if (!is_open())
		return false;

	return true;
}

void cprs::ComPortRecieverSender::close()
{
	m_io.stop();
	if (m_thread.joinable())
		m_thread.join();
	
	m_port.cancel();
	/*if (m_port.is_open())
		m_port.close();*/	
}

bool cprs::ComPortRecieverSender::is_open() const
{
	return m_port.is_open();
}

bool cprs::ComPortRecieverSender::send_data_bytes(const std::vector<UCHAR>& aData)
{
	return write_data_bytes(aData.data(), aData.size());	
}

bool cprs::ComPortRecieverSender::send_data_string(const std::string& aData)
{
	return write_data_string(aData.c_str(), aData.length());
}

std::array<CHAR, cprs::SERIAL_PORT_READ_BUF_SIZE> cprs::ComPortRecieverSender::get_response()
{
	return m_buffer;
}

std::string cprs::ComPortRecieverSender::get_error_string() const
{
	return m_err.message();
}

bool cprs::ComPortRecieverSender::write_data_bytes(const UCHAR *aDataWrite, const UINT aDataLen)
{
	if (!is_open()) return false;
	
	asio::async_write(m_port, 
		              asio::buffer(aDataWrite, aDataLen), 
		              std::bind(&ComPortRecieverSender::handle_write_data, this, std::placeholders::_1, std::placeholders::_2));

	if (!m_err) return true;
	return false;
}

bool cprs::ComPortRecieverSender::write_data_string(CCHAR aDataWrite, const UINT aDataLen)
{
	if (!is_open()) return false;
	
	asio::async_write(m_port,
		asio::buffer(aDataWrite, aDataLen),
		std::bind(&ComPortRecieverSender::handle_write_data, this, std::placeholders::_1, std::placeholders::_2));

	if (!m_err) return true;
	return false;
}

void cprs::ComPortRecieverSender::handle_write_data(const boost::system::error_code& aErr, UINT aByatesTransfered)
{
	if (!aErr) {
	} else {
		m_err = aErr;
	}
}

bool cprs::ComPortRecieverSender::read_data()
{
	if (!is_open()) return false;

	//##### Clear buffer #####
	m_buffer.fill({});

	m_port.async_read_some(asio::buffer(m_buffer),
						   std::bind(&cprs::ComPortRecieverSender::handle_read_data, 
							   this, std::placeholders::_1, std::placeholders::_2));

	
	return false;
}

void cprs::ComPortRecieverSender::handle_read_data(const boost::system::error_code& aErr, UINT aByatesTransfered)
{
	if (!aErr && aByatesTransfered)
	{
		if (m_observer)
		{
			m_observer->on_data_received(std::vector<CHAR>(m_buffer.begin(), m_buffer.end()));
		}
		read_data();
	}
	else {
		m_err = aErr;
	}
	
}




