#include "curseforgeapi.h"

#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QDebug>

#include "util/tutil.hpp"
#include "util/mmlogger.h"
#include "config.hpp"

const QString CurseforgeAPI::PREFIX = "https://api.curseforge.com";
const QByteArray CurseforgeAPI::API_KEY = "$2a$10$7WWV1WELcwX8Xp7d68MHs.5grTyZVa6SynyeSyMh1rsE89nIa09zO";

CurseforgeAPI::CurseforgeAPI(QObject *parent) :
    QObject(parent) {
    accessManager_.setTransferTimeout(Config().getNetworkRequestTimeout());
    diskCache_.setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    accessManager_.setCache(&diskCache_);
}

CurseforgeAPI *CurseforgeAPI::api() {
    static CurseforgeAPI api;
    return &api;
}

Reply<QList<CurseforgeModInfo> > CurseforgeAPI::searchMods(int sectionId, const GameVersion &version, int index, const QString &searchFilter, int category, int sort) {
    QUrl url = PREFIX + "/v1/mods/search";

    //url query
    QUrlQuery urlQuery;

    //?
    urlQuery.addQueryItem("categoryId", QString::number(category));
    //minecraft
    urlQuery.addQueryItem("gameId", "432");
    //game version
    if(version != GameVersion::Any)
        urlQuery.addQueryItem("gameVersion", version);
    //index
    urlQuery.addQueryItem("index", QString::number(index));
    //search page size
    urlQuery.addQueryItem("pageSize", QString::number(Config().getSearchResultCount()));
    //search by name
    urlQuery.addQueryItem("searchFilter", searchFilter);
    //mod
    urlQuery.addQueryItem("sectionId", QString::number(sectionId));
    //sort, 0 for no sort spec
    urlQuery.addQueryItem("sort", QString::number(sort + 1));

    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-API-Key", API_KEY);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return { reply, [=]{
            //parse json
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug("%s", error.errorString().toUtf8().constData());
                return QList<CurseforgeModInfo> {};
            }
            auto resultList = value(jsonDocument.toVariant(), "data").toList();
            QList<CurseforgeModInfo> modInfoList;

            for(const auto &result : qAsConst(resultList))
                modInfoList << CurseforgeModInfo::fromVariant(result);

            return modInfoList;
        } };
}

Reply<int, CurseforgeFileInfo, QList<CurseforgeFileInfo> > CurseforgeAPI::getIdByFingerprint(const QString &fingerprint) {
    QUrl url = PREFIX + "/v1/fingerprints";

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-API-Key", API_KEY);
    MMLogger::network(this) << url << fingerprint;
    auto reply = accessManager_.post(request, QString("[ %1 ]").arg(fingerprint).toUtf8());
    return { reply, [=]{
            //parse json
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug("%s", error.errorString().toUtf8().constData());
                return std::make_tuple(0, CurseforgeFileInfo(), QList<CurseforgeFileInfo> {});
            }
            auto exactMatchList = value(value(jsonDocument.toVariant(), "data"), "exactMatches").toList();
            if(exactMatchList.isEmpty())
                return std::make_tuple(0, CurseforgeFileInfo(), QList<CurseforgeFileInfo>{});
            else {
                int id = value(exactMatchList.at(0), "id").toInt();
                auto file = CurseforgeFileInfo::fromVariant(value(exactMatchList.at(0), "file"));
                auto latestFileList = value(exactMatchList.at(0), "latestFiles").toList();

                QList<CurseforgeFileInfo> fileList;
                for (const auto &variant : latestFileList)
                    fileList << CurseforgeFileInfo::fromVariant(variant);

                return std::make_tuple(id, file, fileList);
            }
        } };
}

Reply<QString> CurseforgeAPI::getDescription(int id) {
    QUrl url = PREFIX + "/v1/mods/" + QString::number(id) + "/description";

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-API-Key", API_KEY);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return { reply, [=]{
            //parse json
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug("%s", error.errorString().toUtf8().constData());
                return QString(value(jsonDocument.toVariant(), "data").toString());
            }
            return QString(value(jsonDocument.toVariant(), "data").toString());
        } };
}

Reply<CurseforgeFileInfo> CurseforgeAPI::getFileInfo(int id, int FileID) {
    QUrl url = PREFIX + "/v1/mods/" + QString::number(id) + "/files/" + QString::number(FileID);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-API-Key", API_KEY);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return { reply, [=]{
            //parse json
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug("%s", error.errorString().toUtf8().constData());
                return CurseforgeFileInfo();
            }
            auto result = value(jsonDocument.toVariant(), "data");
            return CurseforgeFileInfo::fromVariant(result);
        } };
}

Reply<QList<CurseforgeFileInfo> > CurseforgeAPI::getFiles(int id) {
    QUrl url = PREFIX + "/v1/mods/" + QString::number(id) + "/files";

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-API-Key", API_KEY);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return { reply, [=]{
            //parse json
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug("%s", error.errorString().toUtf8().constData());
                return QList<CurseforgeFileInfo> {};
            }
            auto resultList = value(jsonDocument.toVariant(), "data").toList();

            QList<CurseforgeFileInfo> fileInfoList;
            for(const auto &result : qAsConst(resultList))
                fileInfoList << CurseforgeFileInfo::fromVariant(result);

            return fileInfoList;
        } };
}

Reply<CurseforgeModInfo> CurseforgeAPI::getInfo(int id) {
    QUrl url = PREFIX + "/v1/mods/" + QString::number(id);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-API-Key", API_KEY);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return { reply, [=]{
            //parse json
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug("%s", error.errorString().toUtf8().constData());
                return CurseforgeModInfo();
            }
            auto result = value(jsonDocument.toVariant(), "data");
            return CurseforgeModInfo::fromVariant(result);
        } };
}

Reply<QList<GameVersion> > CurseforgeAPI::getMinecraftVersionList() {
    QUrl url = PREFIX + "/v1/minecraft/version";

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-API-Key", API_KEY);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return Reply<QList<GameVersion> >(reply, [=] {
        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return QList<GameVersion> {};
        }
        auto list = value(jsonDocument.toVariant(), "data").toList();
        QList<GameVersion> versionList;
        for(const auto &entry : qAsConst(list))
            versionList << value(entry, "versionString").toString();

        return versionList;
    });
}

Reply<QList<CurseforgeCategoryInfo> > CurseforgeAPI::getSectionCategories(int sectionId) {
    QUrl url = PREFIX + "/v1/categories/" + QString::number(sectionId);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-API-Key", API_KEY);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return { reply, [=]{
            return QList<CurseforgeCategoryInfo> {};
            //parse json
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug("%s", error.errorString().toUtf8().constData());
                return QList<CurseforgeCategoryInfo> {};
            }
            QFile cachedFile(cachedCategoriesFilePath(sectionId));
            if(cachedFile.open(QIODevice::WriteOnly)) {
                cachedFile.write(value(jsonDocument.toVariant(), "data").toJsonDocument().toJson());
                cachedFile.close();
            }
            auto list = value(jsonDocument.toVariant(), "data").toList();
            QList<CurseforgeCategoryInfo> categoryList;
            for(const auto &entry : qAsConst(list)) {
                auto category = CurseforgeCategoryInfo::fromVariant(entry);
                if(category.url().isEmpty() || category.url() == QUrl("https://www.curseforge.com"))
                    continue;
                categoryList << category;
            }
            return categoryList;
        } };
}

QList<CurseforgeCategoryInfo> CurseforgeAPI::cachedSectionCategories(int sectionId) {
    QByteArray bytes;
    QFile cachedFile(cachedCategoriesFilePath(sectionId));
    if(!cachedFile.open(QIODevice::ReadOnly)) return {};
    bytes = cachedFile.readAll();
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(bytes, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug("%s", error.errorString().toUtf8().constData());
        return QList<CurseforgeCategoryInfo> {};
    }
    auto list = jsonDocument.toVariant().toList();
    QList<CurseforgeCategoryInfo> categoryList;
    for(const auto &entry : qAsConst(list)) {
        auto category = CurseforgeCategoryInfo::fromVariant(entry);
        if(category.url().isEmpty() || category.url() == QUrl("https://www.curseforge.com"))
            continue;
        categoryList << category;
    }
    return categoryList;
}

QString CurseforgeAPI::cachedCategoriesFilePath(int sectionId) {
    return QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation))
           .absoluteFilePath("curseforge/section-%1-categories.json").arg(sectionId);
}
