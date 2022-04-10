#pragma once
#include "wx/wx.h"
#include "wxMainFrame.h"

class LilyNESApp : public wxApp
{
public:
    LilyNESApp();
    bool OnInit();

private:
    wxMainFrame* m_mainFrame;
};

