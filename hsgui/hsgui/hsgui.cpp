#include "hsgui/hsgui.hpp"

#include <cstdlib>
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

void Hsgui::refresh_table() {}

void Hsgui::slice(fs::path f) {
    std::stringstream message{};
    int prev_size = dmi.states.size();

    uiProgressBarSetValue(progress_bar, -1);

    try {
        dmi = DMI();
        dmi.load(f);
    } catch (const DMIError &e) {
        uiLabelSetText(status_bar, e.describe().c_str());
        goto end;
    }

    refresh_table();

    message << "loaded " << dmi.states.size() << " icon states";
    uiLabelSetText(status_bar, message.str().c_str());
    current_mode = Mode::SLICING;
    uiControlEnable(uiControl(save_button));

end:
    int new_size = dmi.states.size();
    for (int i = 0; i < std::min(prev_size, new_size); i++)
        uiTableModelRowChanged(t_model, i);
    if (new_size < prev_size)
        for (int i = new_size; i < prev_size; i++)
            uiTableModelRowDeleted(t_model, i);
    else
        for (int i = prev_size; i < new_size; i++)
            uiTableModelRowInserted(t_model, i);

    uiControlEnable(uiControl(slice_button));
    uiControlEnable(uiControl(join_button));
    uiProgressBarSetValue(progress_bar, 0);
}

void Hsgui::on_join() {}

void Hsgui::slice_save() {

end:
    uiControlEnable(uiControl(slice_button));
    uiControlEnable(uiControl(join_button));
    uiProgressBarSetValue(progress_bar, 0);
}

void Hsgui::on_save() {
    if (current_op) {
        current_op.value().join();
        current_op.reset();
    }

    if (current_mode == Mode::SLICING) {
        uiControlDisable(uiControl(slice_button));
        uiControlDisable(uiControl(join_button));
        uiControlDisable(uiControl(save_button));
        current_op.emplace(&Hsgui::slice_save, this);
    }
}

int Hsgui::on_quit() {
    if (current_op)
        current_op.value().join();

    uiQuit();
    return 1;
}

Hsgui::Hsgui() : current_mode(Mode::IDLE), current_op(), dmi(), t_handler(dmi) {
    uiInit(&ui_opts);

    window = uiNewWindow("Headsurgeon", 320, 240, false);
    uiWindowSetMargined(window, true);

    slice_button = uiNewButton("Slice");
    uiButtonOnClicked(slice_button, cb<uiButton *, &Hsgui::on_slice>(), this);
    join_button = uiNewButton("Join");
    uiButtonOnClicked(join_button, cb<uiButton *, &Hsgui::on_join>(), this);
    save_button = uiNewButton("Save");
    uiControlDisable(uiControl(save_button));
    uiButtonOnClicked(save_button, cb<uiButton *, &Hsgui::on_join>(), this);

    button_box = uiNewHorizontalBox();
    uiBoxSetPadded(button_box, true);
    uiBoxAppend(button_box, uiControl(slice_button), true);
    uiBoxAppend(button_box, uiControl(join_button), true);

    auto mh = reinterpret_cast<uiTableModelHandler*>(&t_handler);
    t_model = uiNewTableModel(mh);

    uiTableParams t_params{t_model, -1};
    icon_table = uiNewTable(&t_params);
    uiTableAppendTextColumn(icon_table, "Icon State", 0, -1, nullptr);

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

// wow this is ugly, thanks C
#define HANDLER(f, a, ...)                                                     \
    [](uiTableModelHandler *h, uiTableModel *, ##__VA_ARGS__) {                \
        auto t = reinterpret_cast<DMITableHandler *>(h);                       \
        return t->f a;                                                         \
    }

DMITableHandler::DMITableHandler(DMI &dmi) : dmi(dmi) {
    handler = {
        HANDLER(NumColumns, ()), HANDLER(ColumnType, (x), int x),
        HANDLER(NumRows, ()), HANDLER(CellValue, (x, y), int x, int y),
        HANDLER(SetCellValue, (x, y, z), int x, int y, const uiTableValue *z)};
}

#undef HANDLER

int DMITableHandler::NumColumns() { return 1; }

uiTableValueType DMITableHandler::ColumnType(int) {
    return uiTableValueTypeString;
}

int DMITableHandler::NumRows() {
    return dmi.states.size();
}

uiTableValue *DMITableHandler::CellValue(int row, int column) {
    std::cout << dmi.states.size() << " " << row << ":" << column << "\n";
    return uiNewTableValueString(dmi.states[row].name.c_str());
}

void DMITableHandler::SetCellValue(int row, int column,
                                   const uiTableValue *val) {
}
