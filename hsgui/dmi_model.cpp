#include "dmi_model.hpp"

#include <QImage>

DMIModel::DMIModel(DMI &dmi) : dmi(dmi) {}

int DMIModel::rowCount(const QModelIndex &) const { return dmi.states.size(); }

QVariant DMIModel::data(const QModelIndex &index, int role) const {
    if (index.row() >= rowCount())
        return {};

    if (!dmi.states.size())
        return {};

    switch (role) {
    case Qt::DisplayRole:
        return QString::fromStdString(dmi.states[index.row()].name);

    case Qt::DecorationRole: {
        if (!dmi.states[index.row()].images.size())
            return {};

        auto &preview = dmi.states[index.row()].images[0][0];
        return QImage(preview.pixels.data(), preview.size.x, preview.size.y,
                      QImage::Format_RGBA8888);
    }

    case Qt::ToolTipRole:
        return QString::fromStdString(dmi.states[index.row()].name);

    default:
        return {};
    }
}

void DMIModel::modify() {
    endResetModel();
    insertRows(0, rowCount());
}
