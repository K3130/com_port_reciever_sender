#include "ui_from_simple.h"



class App : public wxApp
{
public:
	virtual bool OnInit()
	{
		ufs::UiFromSimple* ui = new ufs::UiFromSimple(wxT("Com port reciever-sender."));
		ui->Show(true);

		return true;
	}
};

IMPLEMENT_APP(App)