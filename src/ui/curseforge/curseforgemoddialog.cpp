#include "curseforgemoddialog.h"
#include "ui_curseforgemoddialog.h"

#include <QDir>
#include <QDesktopServices>
#include <QAction>
#include <QClipboard>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkReply>

#include "local/localmodpath.h"
#include "curseforge/curseforgemod.h"
#include "curseforgefileitemwidget.h"
#include "curseforgeimagepopup.h"
#include "util/datetimesortitem.h"
#include "util/smoothscrollbar.h"
#include "util/flowlayout.h"

CurseforgeModDialog::CurseforgeModDialog(QWidget *parent, CurseforgeMod *mod, LocalMod *localMod) :
    QDialog(parent),
    ui(new Ui::CurseforgeModDialog),
    mod_(mod),
    localMod_(localMod)
{
    ui->setupUi(this);
    ui->modDescription->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->galleryListWidget->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->fileListWidget->setVerticalScrollBar(new SmoothScrollBar(this));
    auto action = new QAction(tr("Copy website link"), this);
    connect(action, &QAction::triggered, this, [=]{
        QApplication::clipboard()->setText(mod_->modInfo().websiteUrl().toString());
    });
    ui->websiteButton->addAction(action);

    connect(mod_, &CurseforgeMod::destroyed, this, &QDialog::close);

    //update basic info
    auto updateBasicInfo = [=]{
        setWindowTitle(mod->modInfo().name() + tr(" - Curseforge"));
        ui->modName->setText(mod->modInfo().name());
        ui->modSummary->setText(mod->modInfo().summary());
        ui->modAuthors->setText(mod->modInfo().authors().join(", "));

        //update thumbnail
        //included by basic info
        auto updateThumbnail = [=]{
            QPixmap pixelmap;
            pixelmap.loadFromData(mod_->modInfo().iconBytes());
            ui->modIcon->setPixmap(pixelmap.scaled(80, 80, Qt::KeepAspectRatio));
            ui->modIcon->setCursor(Qt::ArrowCursor);
        };

        if(!mod_->modInfo().iconBytes().isEmpty())
            updateThumbnail();
        else {
            mod->acquireIcon();
            ui->modIcon->setCursor(Qt::BusyCursor);
            connect(mod_, &CurseforgeMod::iconReady, this, updateThumbnail);
        }
    };

    if(mod_->modInfo().hasBasicInfo())
        updateBasicInfo();
    else {
        mod->acquireBasicInfo();
        connect(mod_, &CurseforgeMod::basicInfoReady, this, updateBasicInfo);
    }

    //update description
    auto updateDescription = [=]{
        ui->modDescription->setFont(qApp->font());
        auto desc = mod_->modInfo().description();
//        QRegExp re(R"(<img.*src=\"(.+)\")");
//        re.setMinimal(true);
//        int pos = 0;
//        QStringList replceList;
//        while ((pos = re.indexIn(desc, pos)) != -1) {
//            replceList << re.cap(1);
//            pos +=  re.matchedLength();
//        }
//        QDir().mkdir("image_cache");
//        for(const auto &str : qAsConst(replceList)){
//            qDebug() << str;
//            QUrl url(str);
//            auto fileName = url.fileName();
//            QFileInfo fileInfo("./image_cache/" + fileName);
//            if(fileInfo.exists()) continue;
//            auto downloader = new Downloader(this);
//            connect(downloader, &Downloader::finished, ui->modDescription, &QTextBrowser::reload);
//            downloader->download(url, "./image_cache/", fileName);
//            desc.replace(str, "./image_cache/" + fileName);
//        }
        ui->modDescription->setHtml(desc);
        ui->modDescription->setCursor(Qt::ArrowCursor);
    };

    if(!mod_->modInfo().description().isEmpty())
        updateDescription();
    else{
        ui->modDescription->setCursor(Qt::BusyCursor);
        mod->acquireDescription();
        connect(mod, &CurseforgeMod::descriptionReady, this, updateDescription);
    }

    //update gallery
    if(mod->modInfo().images().isEmpty())
        ui->tabWidget->removeTab(1);
    for(const auto &image : mod->modInfo().images()){
        auto item = new QListWidgetItem();
        item->setText(image.title);
        item->setToolTip(image.description);
        item->setData(Qt::UserRole, image.url);
        item->setData(Qt::UserRole + 1, image.title);
        item->setData(Qt::UserRole + 2, image.description);
        item->setSizeHint(QSize(260, 260));
        ui->galleryListWidget->addItem(item);
        QNetworkRequest request(image.thumbnailUrl);
        static QNetworkAccessManager accessManager;
        static QNetworkDiskCache diskCache;
        diskCache.setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
        accessManager.setCache(&diskCache);
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        auto reply = accessManager.get(request);
        connect(reply, &QNetworkReply::finished, this, [=]{
            if(reply->error() != QNetworkReply::NoError) return;
            auto bytes = reply->readAll();
            QPixmap pixmap;
            pixmap.loadFromData(bytes);
            item->setIcon(QIcon(pixmap));
            item->setData(Qt::UserRole + 3, bytes);
            reply->deleteLater();
        });
    }

    //update file list
    auto updateFileList = [=]{
        ui->fileListWidget->clear();
        auto files = mod_->modInfo().allFileList();
        for(const auto &fileInfo : files){
            auto *listItem = new DateTimeSortItem();
            listItem->setData(DateTimeSortItem::Role, fileInfo.fileDate());
            listItem->setSizeHint(QSize(500, 90));
            auto itemWidget = new CurseforgeFileItemWidget(this, mod_, fileInfo, localMod_);
            itemWidget->setDownloadPath(downloadPath_);
            connect(this, &CurseforgeModDialog::downloadPathChanged, itemWidget, &CurseforgeFileItemWidget::setDownloadPath);
            ui->fileListWidget->addItem(listItem);
            ui->fileListWidget->setItemWidget(listItem, itemWidget);
        }
        ui->fileListWidget->sortItems(Qt::DescendingOrder);
        ui->fileListWidget->setCursor(Qt::ArrowCursor);
    };

    if(!mod_->modInfo().allFileList().isEmpty())
        updateFileList();
    else {
        ui->fileListWidget->setCursor(Qt::BusyCursor);
        mod->acquireAllFileList();
        connect(mod, &CurseforgeMod::allFileListReady, this, updateFileList);
    }
    if(localMod_) setDownloadPath(localMod_->path());
}

CurseforgeModDialog::~CurseforgeModDialog()
{
    delete ui;
}

void CurseforgeModDialog::setDownloadPath(LocalModPath *newDownloadPath)
{
    downloadPath_ = newDownloadPath;
    emit downloadPathChanged(newDownloadPath);
}

void CurseforgeModDialog::on_websiteButton_clicked()
{
    QDesktopServices::openUrl(mod_->modInfo().websiteUrl());
}

void CurseforgeModDialog::on_galleryListWidget_itemClicked(QListWidgetItem *item)
{
    auto popup = new CurseforgeImagePopup(
                this,
                item->data(Qt::UserRole).toUrl(),
                item->data(Qt::UserRole + 1).toString(),
                item->data(Qt::UserRole + 2).toString(),
                item->data(Qt::UserRole + 3).toByteArray());
    popup->exec();
}

