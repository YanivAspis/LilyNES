#pragma once
#include "wx/wx.h"
#include "wxMainFrame.h"

class LilyNESApp : public wxApp
{
public:
    LilyNESApp();
    bool OnInit();
    void OnKeyDown(wxKeyEvent& evt);
    void OnKeyUp(wxKeyEvent& evt);

private:
    wxMainFrame* m_mainFrame;
    std::map < int, std::function<void(wxMainFrame&)>> m_keyDownFuncs;
    std::map < int, std::function<void(wxMainFrame&)>> m_keyUpFuncs;
};

