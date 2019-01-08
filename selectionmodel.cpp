#include "selectionmodel.h"
#include <QDebug>

SelectionModel::SelectionModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant SelectionModel::headerData(int, Qt::Orientation, int) const
{
    return QVariant();
}

QModelIndex SelectionModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return createIndex(row, column);;
}

QModelIndex SelectionModel::parent(const QModelIndex &) const
{
    return createIndex(0,0);
}

int SelectionModel::rowCount(const QModelIndex &) const
{
    return _data.size();
}

int SelectionModel::columnCount(const QModelIndex &) const
{
    return _data.at(0).size();
}

QVariant SelectionModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);
    const auto row = index.row();
    const auto col = index.column();
    if (!index.isValid()
            || row >= rowCount()
            || col >= _data.at(row).size())
    {
        qDebug() << "Invalid model index requested:" << col << row;
        return QVariant();
    }
    int value = _data.at(row).at(col);
    if (value < -1 || value > 1)
    {
        qDebug() << "Wrong value retrieved:" << value
                 << "row" << row
                 << "col" << col
                 << "total rows" << _data.size()
                 << "total cols in row" << _data[row].size()
                 << "value" << _data[row][col];
    }
    return QVariant::fromValue<int>(value);
}

void SelectionModel::init(int width, int height) // READ FILE HERE
{
    beginResetModel();
    _data.clear();
    for (auto y(0); y < height; ++y) {
        _data.push_back(std::vector<int>(width));
    }
    setWidth(columnCount());
    setHeight(rowCount());
    endResetModel();
    emit dataChanged(index(0,0), index(rowCount(), columnCount()));
}

void SelectionModel::init(PointMatrix &&data)
{
    beginResetModel();
    _data = data;
    setWidth(columnCount());
    setHeight(rowCount());
    endResetModel();
    emit dataChanged(index(0,0), index(rowCount(), columnCount()));
}

void SelectionModel::setChunk(int x, int y, ChunkState state)
{
    if ((x < 0 || x >= columnCount())
        || (y < 0 || y >= rowCount())
        || (_data.at(y).at(x) == state)) {
        return;
    }

    _data[y][x] = state;
    emit dataChanged(this->index(x,y), this->index(x,y));
    // Strange temporary solution.
    beginResetModel();
    endResetModel();
}

int SelectionModel::width() const
{
    return m_width;
}

int SelectionModel::height() const
{
    return m_height;
}





void SelectionModel::setWidth(int width)
{
    m_width = width;
    emit widthChanged(m_width);
}

void SelectionModel::setHeight(int height)
{
    m_height = height;
    emit heightChanged(m_height);
}
