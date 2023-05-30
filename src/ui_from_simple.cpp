#include "ui_from_simple.h"

wxBEGIN_EVENT_TABLE(ufs::UiFromSimple::UiFromSimple, wxFrame)

wxEND_EVENT_TABLE()

std::list<int> getAvailablePorts()
{
	wchar_t lpTargetPath[5000];
	std::list<int> portList;

	for (int i = 0; i < 255; i++)
	{
		std::wstring str = L"COM" + std::to_wstring(i);
		DWORD res = QueryDosDevice(str.c_str(), lpTargetPath, 5000);
		if (res != 0)
		{
			portList.push_back(i);
		}
		if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
		}
	}
	return portList;
}

wxString FormatHexString(const wxString& aHexString)
{
	wxString formattedString;
	for (size_t i = 0; i < aHexString.Len(); i += 2)
	{
		formattedString += aHexString.Mid(i, 2) + " ";
	}
	return formattedString.Trim();
}

ufs::UiFromSimple::UiFromSimple(const wxString& aTitle)
	                : wxFrame(NULL, wxID_ANY, aTitle, wxDefaultPosition, wxSize(640, 480),
						wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
	
	Centre();
	SetIcon(wxICON(wxICON_APPLICATION));

	isConnectToDevice = false;
	m_cprs = nullptr;

	//-----------------------
	mainPanel = new wxPanel(this, wxID_ANY);
	panelTop = new wxPanel(mainPanel, -1, wxPoint(-1, -1), wxSize(-1, -1), wxBORDER_SUNKEN);
	panelBot = new wxPanel(mainPanel, -1, wxPoint(-1, -1), wxSize(-1, -1), wxBORDER_SUNKEN);
	wxBoxSizer* vbox  = new wxBoxSizer(wxVERTICAL);
	vbox->Add(panelTop, 1, wxEXPAND | wxALL, 5);
	vbox->Add(panelBot, 1, wxEXPAND | wxALL, 5);
	mainPanel->SetSizer(vbox);
	//-----------------------


	//-----------------------
	box1 = new wxStaticBox(panelTop, wxID_ANY, wxT("Controller ->"), wxPoint(5, 5), wxSize(600, 200), wxALIGN_CENTRE_HORIZONTAL);

	text0 = new wxStaticText(box1, wxID_ANY, "Port:", wxPoint(10, 20));
	combox0 = new wxComboBox(box1, wxID_ANY, "", wxPoint(70, 15), wxSize(70, 25), 0, {}, wxCB_READONLY);
	#ifdef WIN32 
	std::list<int>::iterator i;
	std::list<int> l = getAvailablePorts();
	for (i = l.begin(); i != l.end(); i++)
		combox0->Append("COM" + wxString(std::to_string(*i)));
	#endif
	
	text1 = new wxStaticText(box1, wxID_ANY, "Bd rate:", wxPoint(10, 40));
	combox1 = new wxComboBox(box1, wxID_ANY, "", wxPoint(70, 40), wxSize(70, 25), 0, {}, wxCB_READONLY);
	combox1->Append({ "9600", "14400", "19200", "28800", "38400", "56000", "57600", "115200" });

	text2 = new wxStaticText(box1, wxID_ANY, "Data bit:", wxPoint(10, 64));
	combox2 = new wxComboBox(box1, wxID_ANY, "", wxPoint(70, 65), wxSize(70, 25), 0, {}, wxCB_READONLY);
	combox2->Append("5");
	combox2->Append("8");

	text3 = new wxStaticText(box1, wxID_ANY, "Flow ctr:", wxPoint(10, 90));
	combox3 = new wxComboBox(box1, wxID_ANY, "", wxPoint(70, 90), wxSize(70, 25), 0, {}, wxCB_READONLY);
	combox3->Append({ "None", "Software", "Hardware" });

	text4 = new wxStaticText(box1, wxID_ANY, "Parity:", wxPoint(10, 115));
	combox4 = new wxComboBox(box1, wxID_ANY, "", wxPoint(70, 115), wxSize(70, 25), 0, {}, wxCB_READONLY);
	combox4->Append({ "None", "Odd" , "Even" });

	text5 = new wxStaticText(box1, wxID_ANY, "Stop bit:", wxPoint(10, 140));
	combox5 = new wxComboBox(box1, wxID_ANY, "", wxPoint(70, 140), wxSize(70, 25), 0, {}, wxCB_READONLY);
	combox5->Append({ "One", "Onepointfive" , "Two" });

	text6 = new wxStaticText(box1, wxID_ANY, "Send byte:", wxPoint(200, 20));
	textctrl0 = new wxTextCtrl(box1, wxID_ANY, "", wxPoint(280, 16), wxSize(200, 22));
	wxTextValidator hexValidator(wxFILTER_INCLUDE_CHAR_LIST);
	hexValidator.SetCharIncludes("0123456789abcdefABCDEF ");
	textctrl0->SetValidator(hexValidator);
	textctrl0->Bind(wxEVT_TEXT, &ufs::UiFromSimple::OnTextChanged, this);
	button0 = new wxButton(box1, wxID_ANY, "Send", wxPoint(500, 14), wxSize(70, 23), 0);
	button0->Bind(wxEVT_BUTTON, &ufs::UiFromSimple::OnSendByte, this);

	text7 = new wxStaticText(box1, wxID_ANY, "Send char:", wxPoint(200, 48));
	textctrl1 = new wxTextCtrl(box1, wxID_ANY, "", wxPoint(280, 45), wxSize(200, 22));
	button1 = new wxButton(box1, wxID_ANY, "Send", wxPoint(500, 44), wxSize(70, 23), 0);
	button1->Bind(wxEVT_BUTTON, &ufs::UiFromSimple::OnSendChar, this);

	button2 = new wxButton(box1, wxID_ANY, "SETUP", wxPoint(40, 170), wxSize(70, 23), 0);
	button2->Bind(wxEVT_BUTTON, &ufs::UiFromSimple::OnSetupSettings, this);
	//-----------------------


	//-----------------------
	box2 = new wxStaticBox(panelBot, wxID_ANY, wxT("Console ->"), wxPoint(5, 5), wxSize(600, 200), wxALIGN_CENTRE_HORIZONTAL);
	textctrl2 = new wxTextCtrl(box2, wxID_ANY, "", wxPoint(10, 20), wxSize(500, 170), wxTE_READONLY | wxTE_RICH | wxTE_MULTILINE);
	textctrl2->SetBackgroundColour(wxColor("Black"));
	
	button3 = new wxButton(box2, wxID_ANY, "Clear", wxPoint(520, 29), wxSize(70, 23), 0);
	button3->Bind(wxEVT_BUTTON, &ufs::UiFromSimple::OnClearConsole, this);

	button4 = new wxButton(box2, wxID_ANY, "?", wxPoint(540, 100), wxSize(23, 23), 0);
	button4->Bind(wxEVT_BUTTON, &ufs::UiFromSimple::OnOpenDialog, this);
	//-----------------------
		
	

}

ufs::UiFromSimple::~UiFromSimple()
{
	if (isConnectToDevice)
	{
		m_cprs->close();
		delete m_cprs;
	}
}

void ufs::UiFromSimple::on_data_received(const std::vector<CHAR>& aData)
{
	textctrl2->SetDefaultStyle(wxTextAttr(*wxGREEN));
	textctrl2->AppendText("-> ");
	for (auto i : aData)
	{
		textctrl2->AppendText(i);
	}
	textctrl2->AppendText("\n");	
}

void ufs::UiFromSimple::OnClearConsole(wxCommandEvent& aEvent)
{
	textctrl2->Clear();
}

void ufs::UiFromSimple::OnSetupSettings(wxCommandEvent& aEvent)
{
	if (isConnectToDevice)
	{
		m_cprs->close();
		delete m_cprs;
		m_cprs = nullptr;
		isConnectToDevice = false;
		textctrl2->SetDefaultStyle(wxTextAttr(*wxGREEN));
		textctrl2->AppendText("Previous connection is closed.\n");
	}

	cprs::UINT baudrate = wxAtoi(combox1->GetValue());
	cprs::UINT databit = wxAtoi(combox2->GetValue());

	asio::serial_port_base::flow_control::type flowcontrol;
	if (combox3->GetValue() == "None")
		flowcontrol = asio::serial_port_base::flow_control::type::none;
	else if (combox3->GetValue() == "Software")
		flowcontrol = asio::serial_port_base::flow_control::type::software;
	else if (combox3->GetValue() == "Hardware")
		flowcontrol = asio::serial_port_base::flow_control::type::hardware;
	else
		flowcontrol = asio::serial_port_base::flow_control::type::none;

	asio::serial_port_base::stop_bits::type stopbit;
	if (combox5->GetValue() == "One")
		stopbit = asio::serial_port_base::stop_bits::type::one;
	else if (combox5->GetValue() == "Onepointfive")
		stopbit = asio::serial_port_base::stop_bits::type::onepointfive;
	else if (combox5->GetValue() == "Two")
		stopbit = asio::serial_port_base::stop_bits::type::two;
	else
		stopbit = asio::serial_port_base::stop_bits::type::one;

	asio::serial_port_base::parity::type parity;
	if (combox4->GetValue() == "None")
		parity = asio::serial_port_base::parity::type::none;
	else if (combox4->GetValue() == "Odd")
		parity = asio::serial_port_base::parity::type::odd;
	else if (combox4->GetValue() == "Even")
		parity = asio::serial_port_base::parity::type::even;
	else
		parity = asio::serial_port_base::parity::type::none;

	if (combox0->GetValue() == "")
	{
		textctrl2->SetDefaultStyle(wxTextAttr(*wxRED));
		textctrl2->AppendText("Port no set.\n");
	}
	else if (baudrate == 0)
	{
		textctrl2->SetDefaultStyle(wxTextAttr(*wxRED));
		textctrl2->AppendText("Baudrate no set.\n");
	}
	else if (databit == 0)
	{
		textctrl2->SetDefaultStyle(wxTextAttr(*wxRED));
		textctrl2->AppendText("Data bit not set.\n");
	}
	else
	{
		m_cprs = new cprs::ComPortRecieverSender(baudrate, databit, flowcontrol, stopbit, parity);
		isConnectToDevice = true;
		if (m_cprs->connect_to_port(combox0->GetValue().c_str()))
		{
			isConnectToDevice = true;
			textctrl2->SetDefaultStyle(wxTextAttr(*wxGREEN));
			textctrl2->AppendText("New connection.\n");

			//##### set observer #####
			m_cprs->set_observer(this);
		}
		else
		{
			textctrl2->SetDefaultStyle(wxTextAttr(*wxRED));
			textctrl2->AppendText("Failed to connect.\n");
			textctrl2->AppendText(m_cprs->get_error_string() + "\n");
			delete m_cprs;
		}
	}


	
	
}

void ufs::UiFromSimple::OnSendByte(wxCommandEvent& aEvent)
{
	if (textctrl0->GetValue().ToStdString().size() % 2 != 0)
	{
		textctrl2->SetDefaultStyle(wxTextAttr(*wxRED));
		textctrl2->AppendText("Incorrect byte input.\n");
	}
	else
	{
		if (isConnectToDevice)
		{
			std::string textdata = textctrl0->GetValue().ToStdString();
			textdata.erase(remove(textdata.begin(), textdata.end(), ' '), textdata.end());
			std::transform(textdata.begin(), textdata.end(), textdata.begin(),
				[](unsigned char c) { return std::tolower(c); });

			std::vector<UCHAR> bytearr;
			
			std::cout << std::hex << std::setfill('0');
			for (std::string::size_type i = 0; i < textdata.size() / 2; ++i) {
				bytearr.push_back(std::stoi(textdata.substr(i * 2, 2), nullptr, 16));
			}


			if (m_cprs->send_data_bytes(bytearr))
			{
				textctrl2->SetDefaultStyle(wxTextAttr(*wxGREEN));
				textctrl2->AppendText("Data send.\n");
				textctrl0->Clear();
			}
			else
			{
				textctrl2->SetDefaultStyle(wxTextAttr(*wxRED));
				textctrl2->AppendText("Failed to send.\n");
				textctrl2->AppendText(m_cprs->get_error_string() + "\n");
			}
		}
		else
		{
			textctrl2->SetDefaultStyle(wxTextAttr(*wxRED));
			textctrl2->AppendText("No setup.\n");
		}
	}
		
}

void ufs::UiFromSimple::OnSendChar(wxCommandEvent& aEvent)
{
	if (isConnectToDevice)
	{
		if (m_cprs->send_data_string(textctrl1->GetValue().ToStdString()))
		{
			textctrl2->SetDefaultStyle(wxTextAttr(*wxGREEN));
			textctrl2->AppendText("Data send.\n");
			textctrl1->Clear();
		}
		else
		{
			textctrl2->SetDefaultStyle(wxTextAttr(*wxRED));
			textctrl2->AppendText("Failed to send.\n");
			textctrl2->AppendText(m_cprs->get_error_string() + "\n");
		}
	}
	else
	{
		textctrl2->SetDefaultStyle(wxTextAttr(*wxRED));
		textctrl2->AppendText("No setup.\n");
	}
}

void ufs::UiFromSimple::OnTextChanged(wxCommandEvent& aEvent)
{
	wxTextCtrl* hexInput = static_cast<wxTextCtrl*>(aEvent.GetEventObject());
	wxString hexString = hexInput->GetValue();
	hexString.Replace(" ", wxEmptyString);

	hexInput->Freeze();
	hexInput->ChangeValue(FormatHexString(hexString));
	hexInput->SetInsertionPointEnd();
	hexInput->Thaw();
}

void ufs::UiFromSimple::OnOpenDialog(wxCommandEvent& aEvent)
{
	wxAboutDialogInfo info;
	info.SetName(wxT("Com port reciever-sender"));
	info.SetVersion(wxT("1.0.0"));
	info.SetDescription(wxT("Com port reciever-sender"));
	info.SetWebSite(wxT("https://github.com/K3130"));
	info.AddDeveloper(wxT("Konstantin Smirnov"));
	wxAboutBox(info);
}

