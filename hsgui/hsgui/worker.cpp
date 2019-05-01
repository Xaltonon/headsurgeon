#include "hsgui/worker.hpp"

#include <iostream>

void Worker::slice_dmi(DMI *dmi, fs::path f) {
    *dmi = DMI{};

    try {
        dmi->load(f);
    } catch (const DMIError &e) {
        failure_reason = e.describe();
        emit on_slice_done(false);
        return;
    }

    emit on_slice_done(true);
}

void Worker::save_states(DMI *dmi, QModelIndexList indices, fs::path fname) {
    int total = indices.size();
    for (int i = 0; i < total; i++) {
        int index = indices[i].row();
        auto &s = dmi->states[i];
        emit on_save_progress(i, &s.name);
        try {
            s.split(fname / s.name);
        } catch (const DMIError &e) {
            failure_reason = e.describe();
            emit on_save_done(false);
            return;
        }
    }

    emit on_save_done(true);
}
