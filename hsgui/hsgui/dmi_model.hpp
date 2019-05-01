#pragma once

#include <QAbstractListModel>
#include <libheadsurgeon/dmi.hpp>

class DMIModel : public QAbstractListModel {
public:
    DMIModel(DMI &dmi);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void modify();

private:
    DMI &dmi;
};
