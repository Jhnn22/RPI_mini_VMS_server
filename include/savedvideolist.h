#ifndef SAVEDVIDEOLIST_H
#define SAVEDVIDEOLIST_H

#include <QDialog>

namespace Ui {
class SavedVideoList;
}

class SavedVideoList : public QDialog
{
    Q_OBJECT

public:
    explicit SavedVideoList(QWidget *parent = nullptr);
    ~SavedVideoList();

    QStringList getSavedDirs();
    QStringList getSavedFiles(const QString &dirName);

signals:
    void play(QString fullPath);

private:
    Ui::SavedVideoList *ui;

    QString dirName, fileName;
};

#endif // SAVEDVIDEOLIST_H
