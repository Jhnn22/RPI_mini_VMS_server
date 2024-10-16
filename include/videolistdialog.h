#ifndef VIDEOLISTDIALOG_H
#define VIDEOLISTDIALOG_H

#include <QDialog>

namespace Ui {
class VideoListDialog;
}

class VideoListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VideoListDialog(QWidget *parent = nullptr);
    ~VideoListDialog();

    QStringList getSavedDirs();
    QStringList getSavedFiles(const QString &dirName);
    void listManage(const QStringList& names, bool flag);

signals:
    void play(QString fullPath);

private:
    Ui::VideoListDialog *ui;

    QString dirName, fileName;
};

#endif // VIDEOLISTDIALOG_H
