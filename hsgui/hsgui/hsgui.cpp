#include "hsgui/hsgui.hpp"

#include <QApplication>
#include <QFileDialog>
#include <QVBoxLayout>

#include <sstream>

Hsgui::Hsgui()
    : dmi_model(dmi)
{
    setWindowTitle("Headsurgeon");

    slice_button = new QPushButton{"Slice"};
    slice_button->connect(slice_button, &QPushButton::clicked, this,
                          &Hsgui::on_slice);
    save_button = new QPushButton{"Save All"};
    save_button->connect(save_button, &QPushButton::clicked, this,
                         &Hsgui::on_save);
    save_button->setEnabled(false);

    QHBoxLayout *control_box = new QHBoxLayout;
    control_box->addWidget(slice_button);
    control_box->addWidget(save_button);

    icon_view = new QListView;
    icon_view->setModel(&dmi_model);
    icon_view->setFlow(QListView::LeftToRight);
    icon_view->setResizeMode(QListView::Adjust);
    icon_view->setViewMode(QListView::IconMode);
    icon_view->setSelectionMode(QListView::MultiSelection);

    auto selection_model = icon_view->selectionModel();
    connect(selection_model, &QItemSelectionModel::selectionChanged,
            this, &Hsgui::on_selection_change);

    progress_bar = new QProgressBar;

    status_label = new QLabel;
    status_label->setSizePolicy({QSizePolicy::Expanding, QSizePolicy::Fixed});

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addLayout(control_box);
    vbox->addWidget(icon_view);
    vbox->addWidget(progress_bar);
    vbox->addWidget(status_label);

    setLayout(vbox);

    qRegisterMetaType<fs::path>("fs::path");
    qRegisterMetaType<QModelIndexList>("QModelIndexList");
    connect(this, &Hsgui::slice_dmi, &worker, &Worker::slice_dmi);
    connect(&worker, &Worker::on_slice_done, this, &Hsgui::on_slice_done);
    connect(this, &Hsgui::save_states, &worker, &Worker::save_states);
    connect(&worker, &Worker::on_save_progress, this, &Hsgui::on_save_progress);
    connect(&worker, &Worker::on_save_done, this, &Hsgui::on_save_done);
    worker.moveToThread(&worker_thread);
    worker_thread.start();
}

Hsgui::~Hsgui() {
    worker_thread.quit();
    worker_thread.wait();
}

void Hsgui::on_slice() {
    auto file = QFileDialog::getOpenFileName(this, "Select DMI to slice.", "",
                                             "DMI Files (*.dmi)");
    if (file == "")
        return;

    slice_button->setEnabled(false);
    save_button->setEnabled(false);
    progress_bar->setRange(0, 0);

    emit slice_dmi(&dmi, file.toStdString());
}

void Hsgui::on_save() {
    auto dir = QFileDialog::getExistingDirectory(this, "Select output folder.", "");

    if (dir == "")
        return;

    slice_button->setEnabled(false);
    save_button->setEnabled(false);
    progress_bar->setValue(0);

    auto indices = icon_view->selectionModel()->selectedIndexes();
    if (!indices.size())
        for (int i = 0; i < dmi.states.size(); i++)
            indices.push_back(dmi_model.index(i));

    progress_bar->setRange(0, indices.size());
    emit save_states(&dmi, indices, dir.toStdString());
}

void Hsgui::on_slice_done(bool success) {
    slice_button->setEnabled(true);
    progress_bar->setRange(0, 100);
    progress_bar->reset();

    update_display();

    if (!success) {
        status_label->setText(QString::fromStdString(worker.failure_reason));
        return;
    }

    std::stringstream message;
    message << "Loaded " << dmi.states.size() << " icon states.";
    status_label->setText(QString::fromStdString(message.str()));

    save_button->setEnabled(true);
}

void Hsgui::on_save_done(bool success) {
    slice_button->setEnabled(true);
    save_button->setEnabled(true);
    progress_bar->setRange(0, 100);
    progress_bar->reset();

    if (success)
        status_label->setText("Successfully saved icon states.");
    else
        status_label->setText(QString::fromStdString(worker.failure_reason));
}

void Hsgui::update_display() {
    icon_view->setGridSize(QSize(dmi.width + 32, dmi.height + 32));
    dmi_model.modify();
}

void Hsgui::on_save_progress(int c, std::string *state) {
    progress_bar->setValue(c);
    status_label->setText(QString::fromStdString(*state));
}

void Hsgui::on_selection_change(const QItemSelection &selected,
                                const QItemSelection &deselected) {
    if (icon_view->selectionModel()->selectedIndexes().size())
        save_button->setText("Save Selected");
    else
        save_button->setText("Save All");
}


int main(int argc, char **argv) {
    QApplication app{argc, argv};
    Hsgui hsgui;
    hsgui.show();

    return app.exec();
}
