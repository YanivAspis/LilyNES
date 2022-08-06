#pragma once
#include "wx/wx.h"
#include "wxMainFrame.h"

class LilyNESApp : public wxApp
{
public:
    LilyNESApp();
    bool OnInit();
    void OnClose(wxCloseEvent& evt);
    
    void OnTimer(wxTimerEvent& evt);
    void OnKeyDown(wxKeyEvent& evt);

private:
    wxMainFrame* m_mainFrame;
    std::map < int, std::function<void(wxMainFrame&)>> m_keyPressFuncs;
    std::map < int, std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>> m_nesKeyTranslator;
    wxTimer* m_keyTimer;
};

