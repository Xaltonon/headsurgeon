#pragma once

#include <libheadsurgeon/filesystem.hpp>
#include <ui.h>

#include <optional>
#include <thread>

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
    int on_quit();

    void slice(fs::path f);

private:
    Mode current_mode;
    std::optional<std::thread> current_op;

    uiInitOptions ui_opts;

    uiWindow *window;
    uiButton *slice_button;
    uiButton *join_button;
    uiButton *save_button;

    uiTable *icon_table;
    uiTableModelHandler t_handler;
    uiTableModel *t_model;
    uiProgressBar *progress_bar;
    uiLabel *status_bar;

    uiBox *vbox;
    uiBox *button_box;
    uiBox *bottom_box;
};
