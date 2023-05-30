#pragma once

#include <asio.hpp>
#include <asio/serial_port.hpp>
#include <asio/thread.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/bind/bind.hpp>



namespace cprs
{
	typedef unsigned int UINT;
	typedef int INT;
	typedef char CHAR;
	typedef unsigned char UCHAR;
	typedef const char* CCHAR;

	//##### Setup buff size const #####
	constexpr UINT SERIAL_PORT_READ_BUF_SIZE = 3130;
	//##### make observer #####
	class ComPortReceiverSenderObserver
	{
	public:
		virtual ~ComPortReceiverSenderObserver(){}
		virtual void on_data_received(const std::vector<CHAR>& aData) = 0;
	};

	class ComPortRecieverSender final
	{
	public:
		//##### Set baud rate, data bit, flow control, stop bits #####
		ComPortRecieverSender(UINT aBaudRate, UINT aDataBit,
			asio::serial_port_base::flow_control::type aFlowControl,
			asio::serial_port_base::stop_bits::type aStopBits,
			asio::serial_port_base::parity::type aParity) : m_baudRate(aBaudRate), m_dataBit(aDataBit),
			m_flowControl(aFlowControl), m_stopBits(aStopBits), m_parity(aParity), m_port(m_io)
		{
			
		}
		
		ComPortRecieverSender(const ComPortRecieverSender&) = delete;
		ComPortRecieverSender() = delete;
		~ComPortRecieverSender() { close(); }

	public:
		//##### Setup observer #####
		void set_observer(ComPortReceiverSenderObserver* aObserver) { m_observer = aObserver; }
		bool connect_to_port(CCHAR aPort);
		void close();
		bool is_open() const;
		//##### Write some bytes #####
		bool send_data_bytes(const std::vector<UCHAR>& aData);
		//##### Write some symbols #####
		bool send_data_string(const std::string& aData);
		//##### Get response #####
		std::array<CHAR, SERIAL_PORT_READ_BUF_SIZE> get_response();
		//##### Get error string #####
		std::string get_error_string() const;

	private:
		bool write_data_bytes(const UCHAR* aDataWrite, const UINT aDataLen);
		bool write_data_string(CCHAR aDataWrite, const UINT aDataLen);
		void handle_write_data(const boost::system::error_code& aErr, UINT aByatesTransfered);
		bool read_data();
		void handle_read_data(const boost::system::error_code& aErr, UINT aByatesTransfered);	

	private:
		UINT m_baudRate;
		UINT m_dataBit;
		asio::serial_port_base::flow_control::type m_flowControl;
		asio::serial_port_base::parity::type m_parity;
		asio::serial_port_base::stop_bits::type m_stopBits;

	private:
		ComPortReceiverSenderObserver* m_observer;
		asio::io_service m_io;
		std::thread m_thread;
		asio::serial_port m_port;
		boost::system::error_code m_err;
		std::array<CHAR, SERIAL_PORT_READ_BUF_SIZE> m_buffer{};
	};	
};

