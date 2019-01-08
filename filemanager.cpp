#include "filemanager.h"

#include <QImageReader>
#include <QDebug>
#include <QUrl>
#include <QDir>

static const QString MODEL_EXTENSION = ".txt";
static const QStringList EXTENSION_FILTERS = { "*.png", "*.jpg", "*.jpeg" };


FileManager::FileManager(QObject *parent) : QObject(parent)
{
}

void FileManager::setModel(std::shared_ptr<SelectionModel> model)
{
    _model = model;
}

void FileManager::openFile(const QString &file)
{
    if (file.isEmpty())
    {
        return;
    }

    auto path = QUrl(file).toLocalFile();

    QImageReader reader(path);
    const QSize sizeOfImage = reader.size();
    const int height = sizeOfImage.height();
    const int width = sizeOfImage.width();


    qDebug() << "Loaded an image " << path << " with size " << width << "x" << height;

    if (!loadFileToModel(path)) {
        const auto gridWidth = roundf(static_cast<float>(width) / m_pixelGridSize);
        const auto gridHeight = roundf(static_cast<float>(height) / m_pixelGridSize);
        qDebug() << "Initing model grid with size " << m_pixelGridSize << ":" << gridWidth << "x" << gridHeight;
        _model->init(gridWidth, gridHeight);
    }

    setTotalImages(m_availableImages.size());
    setCurrentImageNumber(m_currentImageFileNumber);
    setImagePath(file);
}

void FileManager::openDir(const QString &dir)
{
    const QUrl url(dir);
    QString dirPath = url.toLocalFile();
    QDir imageFolder(dirPath);

    m_availableImages.clear();
    setTotalImages(0);

    auto availableFiles = imageFolder.entryList(EXTENSION_FILTERS);

    if (availableFiles.empty())
    {
        qDebug() << "No" << EXTENSION_FILTERS << "found in given folder.";
        return;
    }

    for (const auto file : availableFiles)
    {
        m_availableImages.append(QUrl::fromLocalFile(imageFolder.path() + "/" + file).toString());
    }

    m_currentImageFileNumber = 0;
    openFile(m_availableImages.at(m_currentImageFileNumber));
}

void FileManager::loadNextImage()
{
    saveSelectionFile();
    if (m_availableImages.isEmpty())
    {
        return;
    }
    setCurrentImageNumber(std::min((m_availableImages.size() - 1), (m_currentImageFileNumber + 1)));
    openFile(m_availableImages.at(m_currentImageFileNumber));
}

void FileManager::loadPrevImage()
{
    saveSelectionFile();
    if (m_availableImages.isEmpty())
    {
        return;
    }
    setCurrentImageNumber(std::max(0, (m_currentImageFileNumber - 1)));
    openFile(m_availableImages.at(m_currentImageFileNumber));
}

void FileManager::loadImageByIndex(int index)
{
    saveSelectionFile();
    if (m_availableImages.isEmpty())
    {
        return;
    }
    setCurrentImageNumber(std::min((m_availableImages.size() - 1), std::max(0, (index - 1))));
    openFile(m_availableImages.at(m_currentImageFileNumber));
}

void FileManager::saveSelectionFile()
{
    if (m_imagePath.isEmpty())
    {
        // Nothing to dave if the filename is empty.
        return;
    }
    QUrl url(m_imagePath);
    QFileInfo pathToImage(url.toLocalFile());

    // Extracting the file name only
    const auto imageFilename = pathToImage.fileName();
    auto withoutExtension = imageFilename.mid(0, imageFilename.indexOf('.'));
    const auto selectionFilename = withoutExtension.append(MODEL_EXTENSION);
    auto fullpath = pathToImage.absoluteDir().path().append("/" + selectionFilename);

    writeModelToFile(fullpath);
}

QString FileManager::imagePath() const
{
    return m_imagePath;
}

int FileManager::pixelGridSize() const
{
    return m_pixelGridSize;
}

int FileManager::totalImages() const
{
    return m_totalImages;
}

int FileManager::currentImageNumber() const
{
    if (m_availableImages.empty())
    {
        return 0;
    }
    return m_currentImageFileNumber + 1; // To display 0 as 1 on UI
}

void FileManager::setImagePath(QString imagePath)
{
    if (m_imagePath == imagePath)
        return;

    m_imagePath = imagePath;
    emit imagePathChanged(m_imagePath);
}

void FileManager::setPixelGridSize(int pixelGridSize)
{
    if (m_pixelGridSize == pixelGridSize)
        return;

    m_pixelGridSize = pixelGridSize;
    emit pixelGridSizeChanged(m_pixelGridSize);

    // Reopen the file after changing the grid size (!)
    openFile(m_imagePath);
}

bool FileManager::writeModelToFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }
    file.flush();

    static const QString HEADER_FORMAT("%1,%2,%3\n"); // Pixel grid size, width, height
    static const QString LINE_FORMAT("%1,%2,%3\n"); // x, y, isSelected
    file.seek(0);

    const auto height = _model->rowCount();
    const auto width = _model->columnCount();

    int gridSize = m_pixelGridSize;
    file.write(HEADER_FORMAT
               .arg(gridSize)
               .arg(_model->columnCount())
               .arg(_model->rowCount())
                    .toLatin1());

    for (auto y(0); y < height; ++y) {
        for (auto x(0); x < width; ++x) {
            bool ok=false;
            const auto index = _model->index(y, x);
            auto data = _model->data(index);
            int value = data.toInt(&ok);
            if (!ok || value < 0 || value > 1)
            {
                qDebug() << "Warning! something bad happened when retrieving selection value in model at row"
                         << y
                         << "column" << x
                         << data;
            }
            QString line = LINE_FORMAT
                            .arg(x)
                            .arg(y)
                            .arg(value);
            file.write(line.toLatin1());
        }
    }

    file.close();
    return true;
}

bool FileManager::loadFileToModel(const QString &filename)
{
    // TODO: impl. safe and effective way to replace extension
    QString modelFile = filename;
    while(modelFile.rbegin() != modelFile.rend()
          && (*modelFile.rbegin() != '.'))
    {
        modelFile.chop(1);
    }
    modelFile.chop(1); // removing the '.' itself
    modelFile.append(MODEL_EXTENSION);


    QFile file(modelFile);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    file.seek(0);

    QString content = QString::fromLatin1(file.readAll());
    QStringList records = content.split("\n");

    bool ok = false;
    QStringList header = records.at(0).split(',');
    records.removeAt(0);

    if (header.size() != 3)
    {
        qDebug() << "Failed to read model file header.";
        return false;
    }

    auto pixelSize = header.at(0).toUInt(&ok);
    auto width = header.at(1).toUInt(&ok);
    auto height = header.at(2).toUInt(&ok);

    if (!ok)
    {
        qDebug() << "Failed to read model file header.";
        return false;
    }

    m_pixelGridSize = pixelSize;

    // After removing the header record, all the rest is data.
    PointMatrix matrix(height);
    for (auto& row : matrix)
    {
        row = std::vector<int>(width);
    }

    for (const QString& record : records)
    {
        if(record.isEmpty())
        {
            continue;
        }
        std::vector<int> values;
        for(QString token : record.split(','))
        {
            values.push_back(token.toInt());
        }
        auto x = values[0];
        auto y = values[1];
        auto selected = values[2];
        matrix[y][x] = selected;
    }

    _model->init(std::move(matrix));

    return true;
}

void FileManager::setTotalImages(int totalImages)
{
    if (m_totalImages == totalImages)
        return;

    m_totalImages = totalImages;
    emit totalImagesChanged(m_totalImages);
}

void FileManager::setCurrentImageNumber(int currentImageIndex)
{
    m_currentImageFileNumber = currentImageIndex;
    emit currentImageIndexChanged(m_currentImageFileNumber);
}
