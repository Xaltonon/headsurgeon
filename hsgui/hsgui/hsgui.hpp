#pragma once

#include <libheadsurgeon/filesystem.hpp>
#include <libheadsurgeon/dmi.hpp>
#include <ui.h>

#include <optional>
#include <thread>

struct DMITableHandler {
    DMITableHandler(DMI &dmi);

    uiTableModelHandler handler;
    DMI &dmi;

    int NumColumns();
    uiTableValueType ColumnType(int);
    int NumRows();
    uiTableValue *CellValue(int, int);
    void SetCellValue(int, int, const uiTableValue *);
};

class Hsgui {
public:
    Hsgui();

    void run();

protected:
    enum class Mode {
        IDLE,
        SLICING,
        JOINING,
    };

    template <typename T, void (Hsgui::*F)()> auto cb() {
        return [](T, void *d) {
            Hsgui *g = static_cast<Hsgui *>(d);
            (g->*F)();
        };
    }

    template <typename T, int (Hsgui::*F)()> auto cb() {
        return [](T, void *d) {
            Hsgui *g = static_cast<Hsgui *>(d);
            return (g->*F)();
        };
    }

    void on_slice();
    void on_join();
    void on_save();
    int on_quit();

    void refresh_table();
    void slice(fs::path f);
    void slice_save();

private:
    Mode current_mode;
    std::optional<std::thread> current_op;
    DMI dmi;

    uiInitOptions ui_opts;

    uiWindow *window;
    uiButton *slice_button;
    uiButton *join_button;
    uiButton *save_button;

    uiTable *icon_table;
    DMITableHandler t_handler;
    uiTableModel *t_model;
    uiProgressBar *progress_bar;
    uiLabel *status_bar;

    uiBox *vbox;
    uiBox *button_box;
    uiBox *bottom_box;
};
