#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <memory>

#include <QObject>
#include <QFile>

#include "selectionmodel.h"


class FileManager : public QObject
{
    Q_OBJECT

    // Original image file path
    Q_PROPERTY(QString imagePath READ imagePath WRITE setImagePath NOTIFY imagePathChanged)
    Q_PROPERTY(QString imageFileName READ imageFileName WRITE setImageFileName NOTIFY imageFileNameChanged)
    // Image mask (grayscale image) file paths. Until the Canvas does
    // the saving itself, it needs another path format, then the loading path.
    // TODO: it is idiotic situation, needs to be fixed asap.
    Q_PROPERTY(QString maskLoadingPath READ maskLoadingPath WRITE setMaskLoadingPath NOTIFY maskLoadingPathChanged)
    Q_PROPERTY(QString maskSavingPath READ maskSavingPath WRITE setMaskSavingPath NOTIFY maskSavingPathChanged)

    Q_PROPERTY(int pixelGridSize READ pixelGridSize WRITE setPixelGridSize NOTIFY pixelGridSizeChanged)
    Q_PROPERTY(int totalImages READ totalImages WRITE setTotalImages NOTIFY totalImagesChanged)
    Q_PROPERTY(int currentImageNumber READ currentImageNumber WRITE setCurrentImageNumber NOTIFY currentImageIndexChanged)

    void setTotalImages(int totalImages);
    void setCurrentImageNumber(int currentImageNumber);
    void setImagePath(QString imagePath);
    void setImageFileName(QString imageFileName);
    void setMaskLoadingPath(QString maskLoadingPath);
    void setPixelGridSize(int pixelGridSize);

public:
    explicit FileManager(QObject *parent = nullptr);

    void setModel(std::shared_ptr<SelectionModel>model);

    Q_INVOKABLE void openDir(const QString& dir);
    Q_INVOKABLE void openFile(const QString& file);

    Q_INVOKABLE void loadNextImage();
    Q_INVOKABLE void loadPrevImage();
    Q_INVOKABLE void loadImageByIndex(int index);

    QString imagePath() const;
    QString imageFileName() const;
    QString maskLoadingPath() const;
    int pixelGridSize() const;
    int totalImages() const;
    int currentImageNumber() const;

    QString maskSavingPath() const;

public slots:
    void setMaskSavingPath(QString maskSavingPath);

signals:

    void imagePathChanged(QString imagePath);
    void pixelGridSizeChanged(int pixelGridSize);
    void totalImagesChanged(int totalImages);
    void currentImageIndexChanged(int currentImageNumber);
    void maskLoadingPathChanged(QString maskLoadingPath);
    void imageFileNameChanged(QString imageFileName);

    void maskSavingPathChanged(QString maskSavingPath);

private:
    bool writeModelToFile(const QString& filename);
    bool loadFileToModel(const QString& filename);

    std::shared_ptr<SelectionModel> _model;
    QString m_imagePath;
    int m_pixelGridSize = 100;

    QStringList m_availableImages;
    int m_currentImageFileNumber = -1;
    int m_totalImages = 0;
    QString m_maskPath;
    QString m_imageFileName;
    QString m_maskSavingPath;
};

#endif // FILEMANAGER_H
