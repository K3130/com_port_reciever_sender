#pragma once

#include "com_port_reciever_sender.h"
#include <wx/wx.h>
#include <wx/regex.h>
#include <wx/textctrl.h>
#include <wx/string.h>
#include <wx/aboutdlg.h>
#include <iomanip>
#include <list>
#include <string>



namespace ufs
{
	class UiFromSimple : public wxFrame, public cprs::ComPortReceiverSenderObserver
	{

	public:
		UiFromSimple(const wxString& aTitle);
		~UiFromSimple();

	private:
		wxPanel* mainPanel;
		wxPanel* panelTop;
		wxPanel* panelBot;
		wxStaticBox* box1;
		wxStaticText* text0;
		wxComboBox* combox0;
		wxStaticText* text1;
		wxComboBox* combox1;
		wxStaticText* text2;
		wxComboBox* combox2;
		wxStaticText* text3;
		wxComboBox* combox3;
		wxStaticText* text4;
		wxComboBox* combox4;
		wxStaticText* text5;
		wxComboBox* combox5;
		wxStaticText* text6;
		wxTextCtrl* textctrl0;
		wxButton* button0;
		wxStaticText* text7;
		wxTextCtrl* textctrl1;
		wxButton* button1;
		wxStaticBox* box2;
		wxTextCtrl* textctrl2;
		wxButton* button2;
		wxButton* button3;
		wxButton* button4;
		

		bool isConnectToDevice;
		cprs::ComPortRecieverSender* m_cprs;
		std::vector<CHAR> m_readBuf;
		

	private:
		void on_data_received(const std::vector<CHAR>& aData) override;
		void OnClearConsole(wxCommandEvent& aEvent);
		void OnSetupSettings(wxCommandEvent& aEvent);
		void OnSendByte(wxCommandEvent& aEvent);
		void OnSendChar(wxCommandEvent& aEvent);
		void OnTextChanged(wxCommandEvent& aEvent);
		void OnOpenDialog(wxCommandEvent& aEvent);

		wxDECLARE_EVENT_TABLE();
	};
};