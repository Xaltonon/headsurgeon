#pragma once

#include <QThread>
#include <QListView>

#include <libheadsurgeon/filesystem.hpp>
#include <libheadsurgeon/dmi.hpp>

#include "worker.hpp"

class Worker : public QObject {
    Q_OBJECT

public:
    std::string failure_reason;

public slots:
    void slice_dmi(DMI *dmi, fs::path f);
    void save_states(DMI *dmi, QModelIndexList indices, fs::path fname);

signals:
    void on_slice_done(bool success);
    void on_save_progress(int c, std::string *state);
    void on_save_done(bool success);
};
