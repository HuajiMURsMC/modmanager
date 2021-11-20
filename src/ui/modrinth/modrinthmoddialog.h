#ifndef MODRINTHMODDIALOG_H
#define MODRINTHMODDIALOG_H

#include <QDialog>

class ModrinthMod;
class LocalMod;
class LocalModPath;
class QStandardItemModel;
namespace Ui {
class ModrinthModDialog;
}

class ModrinthModDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModrinthModDialog(QWidget *parent, ModrinthMod *mod, LocalMod *localMod = nullptr);
    ~ModrinthModDialog();

signals:
    void downloadPathChanged(LocalModPath *path);

public slots:
    void setDownloadPath(LocalModPath *newDownloadPath);

private slots:
    void updateBasicInfo();
    void updateFullInfo();
    void updateFileList();
    void updateIcon();
    void on_websiteButton_clicked();

    void on_modSummary_customContextMenuRequested(const QPoint &pos);

private:
    Ui::ModrinthModDialog *ui;
    QStandardItemModel *model_;
    ModrinthMod *mod_;
    LocalMod *localMod_ = nullptr;
    LocalModPath *downloadPath_ = nullptr;
    bool transltedSummary_ = false;
};

#endif // MODRINTHMODDIALOG_H
