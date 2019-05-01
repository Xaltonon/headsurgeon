#pragma once

#include <QLabel>
#include <QListView>
#include <QProgressBar>
#include <QPushButton>
#include <QWidget>
#include <QMutex>
#include <libheadsurgeon/dmi.hpp>

#include "worker.hpp"
#include "dmi_model.hpp"

class Hsgui : public QWidget {
    Q_OBJECT

public:
    Hsgui();
    ~Hsgui();

protected:
    void update_display();

public slots:
    void on_slice();
    void on_slice_done(bool success);

    void on_save();
    void on_save_progress(int c, std::string *state);
    void on_save_done(bool success);

    void on_selection_change(const QItemSelection &selected,
                             const QItemSelection &deselected);

signals:
    void slice_dmi(DMI *dmi, fs::path fname);
    void save_states(DMI *dmi, QModelIndexList indices, fs::path fname);

private:
    bool busy = false;
    QThread worker_thread;
    Worker worker;
    DMI dmi;
    DMIModel dmi_model;

    QPushButton *slice_button;
    QPushButton *save_button;

    QListView *icon_view;

    QLabel *status_label;
    QProgressBar *progress_bar;
};
