#ifndef DOWNLOADBROWSER_H
#define DOWNLOADBROWSER_H

#include "ui/browser.h"
#include "download/downloadfileinfo.h"

namespace Ui {
class DownloadBrowser;
}

class DownloadManager;
class QAria2Downloader;
class QAria2;

class DownloadBrowser : public Browser
{
    Q_OBJECT

public:
    explicit DownloadBrowser(QWidget *parent = nullptr);
    ~DownloadBrowser();

    QIcon icon() const override;
    QString name() const override;
public slots:
    void addNewDownloaderItem(const DownloadFileInfo &info, QAria2Downloader *downloader);
    void downloadSpeed(qint64 download, qint64 upload = 0);

private slots:

private:
    Ui::DownloadBrowser *ui;
    DownloadManager *manager_;
};

#endif // DOWNLOADBROWSER_H
