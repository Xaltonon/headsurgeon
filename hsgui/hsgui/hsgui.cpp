#include "hsgui/hsgui.hpp"

#include <libheadsurgeon/dmi.hpp>

#include <iostream>
#include <sstream>
#include <thread>

void Hsgui::on_slice() {
    if (current_op) {
        current_op.value().join();
        current_op.reset();
    }

    char *f = uiOpenFile(window);
    if (!f)
        return;

    uiControlDisable(uiControl(slice_button));
    uiControlDisable(uiControl(join_button));
    uiControlDisable(uiControl(save_button));

    current_op.emplace(&Hsgui::slice, this, fs::path(f));
    uiFreeText(f);
}

void Hsgui::slice(fs::path f) {
    DMI dmi;
    std::stringstream message{};

    uiProgressBarSetValue(progress_bar, -1);

    try {
        dmi.load(f);
    } catch (const DMIError &e) {
        uiLabelSetText(status_bar, e.describe().c_str());
        goto end;
    }

    message << "loaded " << dmi.states.size() << " icon states";
    uiLabelSetText(status_bar, message.str().c_str());
    current_mode = Mode::SLICING;
    uiControlEnable(uiControl(save_button));

end:
    uiControlEnable(uiControl(slice_button));
    uiControlEnable(uiControl(join_button));
    uiProgressBarSetValue(progress_bar, 0);
}

void Hsgui::on_join() {}

int Hsgui::on_quit() {
    if (current_op)
        current_op.value().join();

    uiQuit();
    return 1;
}

Hsgui::Hsgui() : current_mode(Mode::IDLE), current_op() {
    uiInit(&ui_opts);

    window = uiNewWindow("Headsurgeon", 320, 240, false);
    uiWindowSetMargined(window, true);

    slice_button = uiNewButton("Slice");
    uiButtonOnClicked(slice_button, cb<uiButton *, &Hsgui::on_slice>(), this);
    join_button = uiNewButton("Join");
    uiButtonOnClicked(join_button, cb<uiButton *, &Hsgui::on_join>(), this);
    save_button = uiNewButton("Save");
    uiControlDisable(uiControl(save_button));
    // uiButtonOnClicked(save_button, void (*f)(uiButton *, void *), void *data)

    button_box = uiNewHorizontalBox();
    uiBoxSetPadded(button_box, true);
    uiBoxAppend(button_box, uiControl(slice_button), true);
    uiBoxAppend(button_box, uiControl(join_button), true);

    t_handler = {[](uiTableModelHandler *, uiTableModel *) { return 1; },
                 [](uiTableModelHandler *, uiTableModel *, int) {
                     return (unsigned int)uiTableValueTypeString;
                 },
                 [](uiTableModelHandler *, uiTableModel *) { return 1; },
                 [](uiTableModelHandler *, uiTableModel *, int, int) {
                     return uiNewTableValueString("fuck");
                 },
                 nullptr};
    t_model = uiNewTableModel(&t_handler);

    uiTableParams t_params{t_model, -1};
    icon_table = uiNewTable(&t_params);

    progress_bar = uiNewProgressBar();
    status_bar = uiNewLabel("");

    bottom_box = uiNewHorizontalBox();
    uiBoxSetPadded(bottom_box, true);
    uiBoxAppend(bottom_box, uiControl(status_bar), true);
    uiBoxAppend(bottom_box, uiControl(save_button), false);

    vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, true);
    uiBoxAppend(vbox, uiControl(button_box), false);
    uiBoxAppend(vbox, uiControl(icon_table), true);
    uiBoxAppend(vbox, uiControl(progress_bar), false);
    uiBoxAppend(vbox, uiControl(bottom_box), false);

    uiWindowSetChild(window, uiControl(vbox));
    uiWindowOnClosing(window, cb<uiWindow *, &Hsgui::on_quit>(), this);
}

void Hsgui::run() {
    uiControlShow(uiControl(window));
    uiMain();
}

int main() {
    Hsgui gui{};
    gui.run();
}
