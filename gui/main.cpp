#include "main.h"

#include <memory>

#include <wx/listctrl.h>

IMPLEMENT_APP(HSGui);

bool HSGui::OnInit() {
    auto frame = new HSFrame;
    SetTopWindow(frame);
    frame->Show(true);

    return true;
}

HSFrame::HSFrame() : wxFrame(nullptr, wxID_ANY, "Headsurgeon") {
    auto vbox = new wxBoxSizer(wxVERTICAL);
    auto hbox = new wxBoxSizer(wxHORIZONTAL);

    split_button = new wxButton(this, split_id, "Split");
    join_button = new wxButton(this, wxID_ANY, "Join");
    icon_list = new wxListCtrl(this, wxID_ANY);
    progress_bar = new wxGauge(this, wxID_ANY, 0);

    hbox->Add(split_button, 1, wxALL, 5);
    hbox->Add(join_button, 1, wxALL, 5);

    vbox->Add(hbox, 0, wxEXPAND | wxALL, 5);
    vbox->Add(icon_list, 1, wxEXPAND | wxALL, 5);
    vbox->Add(progress_bar, 0, wxEXPAND | wxALL, 5);

    SetSizer(vbox);

    wxListItem item;
    item.SetText("Hello");
    item.SetId(0);
    icon_list->InsertItem(item);
}

BEGIN_EVENT_TABLE(HSFrame, wxFrame)
    EVT_BUTTON(split_id, HSFrame::on_split)
END_EVENT_TABLE()

void HSFrame::on_split(wxCommandEvent &event) {
    wxFileDialog file_dia{this, "Ass", "", "", "DMI Files|*.dmi", wxFD_OPEN | wxFD_FILE_MUST_EXIST};
    file_dia.ShowModal();
}
