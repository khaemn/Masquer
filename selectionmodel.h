#ifndef SELECTIONMODEL_H
#define SELECTIONMODEL_H

#include <QAbstractItemModel>

#include <vector>

using PointMatrix = std::vector<std::vector<int>>;

class SelectionModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)

    void setWidth(int width);
    void setHeight(int height);

public:
    enum ChunkState {
        FORBIDDEN = -1,
        DEFAULT = 0,
        SELECTED = 1
    };
    Q_ENUMS(ChunkState)

    explicit SelectionModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /// Inits the model with proper size.
    void init(int width, int height);
    void init(PointMatrix&& data);

    Q_INVOKABLE void setChunk(int x, int y, ChunkState state);

    int width() const;
    int height() const;

signals:
    void widthChanged(int width);
    void heightChanged(int height);

private:
    PointMatrix _data;
    int m_width = 0;
    int m_height = 0;
};

#endif // SELECTIONMODEL_H
