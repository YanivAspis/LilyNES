#pragma once
#include <wx/wx.h>
#include <wx/grid.h>
#include "wxNESStateEvent.h"
#include "../nes/CPU2A03.h"

wxDECLARE_EVENT(EVT_CPU_STATE_GUI_UPDATE, wxNESStateEvent<CPUState>);


class wxCPUStatePanel : public wxPanel {
public:
	wxCPUStatePanel(wxWindow* parent);
	void OnResize(wxSizeEvent& evt);
	void OnGUIUpdate(wxNESStateEvent<CPUState>& evt);

private:
	// Reg Display
	wxStaticText* m_regAName;
	wxStaticText* m_regAVal;
	wxStaticText* m_regXName;
	wxStaticText* m_regXVal;
	wxStaticText* m_regYName;
	wxStaticText* m_regYVal;
	wxStaticText* m_regSName;
	wxStaticText* m_regSVal;
	wxStaticText* m_regPCName;
	wxStaticText* m_regPCVal;
	wxStaticText* m_regPName;
	wxStaticText* m_regPVal;

	// Flags display
	wxStaticText* m_flagNName;
	wxStaticText* m_flagVName;
	wxStaticText* m_flagUName;
	wxStaticText* m_flagBName;
	wxStaticText* m_flagDName;
	wxStaticText* m_flagIName;
	wxStaticText* m_flagZName;
	wxStaticText* m_flagCName;

	wxStaticText* m_flagNVal;
	wxStaticText* m_flagVVal;
	wxStaticText* m_flagUVal;
	wxStaticText* m_flagBVal;
	wxStaticText* m_flagDVal;
	wxStaticText* m_flagIVal;
	wxStaticText* m_flagZVal;
	wxStaticText* m_flagCVal;

	// Other info
	wxStaticText* m_cyclesName;
	wxStaticText* m_cyclesVal;
	wxStaticText* m_irqPendingName;
	wxStaticText* m_irqPendingVal;
	wxStaticText* m_nmiRaisedName;
	wxStaticText* m_nmiRaisedVal;


	wxDECLARE_EVENT_TABLE();


};
