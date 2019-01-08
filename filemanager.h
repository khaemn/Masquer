#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <memory>

#include <QObject>
#include <QFile>

#include "selectionmodel.h"


class FileManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString imagePath READ imagePath WRITE setImagePath NOTIFY imagePathChanged)
    Q_PROPERTY(int pixelGridSize READ pixelGridSize WRITE setPixelGridSize NOTIFY pixelGridSizeChanged)
    Q_PROPERTY(int totalImages READ totalImages WRITE setTotalImages NOTIFY totalImagesChanged)
    Q_PROPERTY(int currentImageNumber READ currentImageNumber WRITE setCurrentImageNumber NOTIFY currentImageIndexChanged)

    void setTotalImages(int totalImages);
    void setCurrentImageNumber(int currentImageNumber);
    void setImagePath(QString imagePath);
    void setPixelGridSize(int pixelGridSize);

public:
    explicit FileManager(QObject *parent = nullptr);

    void setModel(std::shared_ptr<SelectionModel>model);

    Q_INVOKABLE void openDir(const QString& dir);
    Q_INVOKABLE void openFile(const QString& file);

    Q_INVOKABLE void loadNextImage();
    Q_INVOKABLE void loadPrevImage();
    Q_INVOKABLE void loadImageByIndex(int index);

    Q_INVOKABLE void saveSelectionFile();


    QString imagePath() const;
    int pixelGridSize() const;
    int totalImages() const;
    int currentImageNumber() const;

signals:

    void imagePathChanged(QString imagePath);
    void pixelGridSizeChanged(int pixelGridSize);
    void totalImagesChanged(int totalImages);
    void currentImageIndexChanged(int currentImageNumber);

private:
    bool writeModelToFile(const QString& filename);
    bool loadFileToModel(const QString& filename);

    std::shared_ptr<SelectionModel> _model;
    QString m_imagePath;
    int m_pixelGridSize = 100;

    QStringList m_availableImages;
    int m_currentImageFileNumber = -1;
    int m_totalImages = 0;
};

#endif // FILEMANAGER_H
