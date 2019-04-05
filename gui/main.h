#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>

#include <memory>

using std::unique_ptr, std::make_unique;

class HSGui : public wxApp {
public:
    bool OnInit() override;
};

class HSFrame : public wxFrame {
public:
    HSFrame();

protected:
    wxButton *split_button;
    wxButton *join_button;
    wxListCtrl *icon_list;
    wxGauge *progress_bar;

    enum IDs {
        split_id = wxID_HIGHEST + 1,
        join_id,
    };

    void on_split(wxCommandEvent &event);

    DECLARE_EVENT_TABLE();
};

DECLARE_APP(HSGui);
