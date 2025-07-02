/**
******************************************************************************
*
* @file       urlfactory.cpp
* @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
* @brief      
* @see        The GNU Public License (GPL) Version 3
* @defgroup   OPMapWidget
* @{
* 
*****************************************************************************/
/* 
* This program is free software; you can redistribute it and/or modify 
* it under the terms of the GNU General Public License as published by 
* the Free Software Foundation; either version 3 of the License, or 
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful, but 
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
* for more details.
* 
* You should have received a copy of the GNU General Public License along 
* with this program; if not, write to the Free Software Foundation, Inc., 
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
#include "urlfactory.h"
#include <QRegExp>
#include <qmath.h>
#include <QRandomGenerator>
#include <QElapsedTimer>

namespace core {

    const double UrlFactory::EarthRadiusKm = 6378.137; // WGS-84

    UrlFactory::UrlFactory()
    {
        /// <summary>
        /// timeout for map connections
        /// </summary>

        /// <summary>
        /// Gets or sets the value of the User-agent HTTP header.
        /// </summary>
        UserAgent = QString("Mozilla/5.0 (Windows NT 6.1; WOW64; rv:%1.0) Gecko/%2%3%4 Firefox/%5.0.%6").arg(QString::number(Random(3,14)), QString::number(Random(QDate().currentDate().year() - 4, QDate().currentDate().year())), QString::number(Random(11,12)), QString::number(Random(10,30)), QString::number(Random(3,14)), QString::number(Random(1,10))).toLatin1();

        Timeout = 5 * 1000;
        CorrectGoogleVersions=true;
        isCorrectedGoogleVersions = false;
        UseGeocoderCache=true;
        UsePlacemarkCache=true;
    }

    UrlFactory::~UrlFactory()
    {
    }

    int UrlFactory::Random(int low, int high)
    {
        return low + QRandomGenerator::global()->generate() % (high - low);
    }

    QString UrlFactory::TileXYToQuadKey(const int &tileX,const int &tileY,const int &levelOfDetail) const
    {
        QString quadKey;
        for(int i = levelOfDetail; i > 0; i--)
        {
            char digit = '0';
            int mask = 1 << (i - 1);
            if((tileX & mask) != 0)
            {
                digit++;
            }
            if((tileY & mask) != 0)
            {
                digit++;
                digit++;
            }
            quadKey.append(digit);
        }
        return quadKey;
    }

    int UrlFactory::GetServerNum(const Point &pos,const int &max) const
    {
        return (pos.X() + 2 * pos.Y()) % max;
    }

    void UrlFactory::setIsCorrectGoogleVersions(bool value)
    {
        isCorrectedGoogleVersions=value;
    }

    bool UrlFactory::IsCorrectGoogleVersions()
    {
        return isCorrectedGoogleVersions;
    }

    void UrlFactory::TryCorrectGoogleVersions()
    {
        QMutexLocker locker(&mutex);
        if(CorrectGoogleVersions && !IsCorrectGoogleVersions())
        {
            QNetworkReply *reply;
            QNetworkRequest qheader;
            QNetworkAccessManager network;
            QEventLoop q;
            QTimer tT;
            tT.setSingleShot(true);
            connect(&network, SIGNAL(finished(QNetworkReply*)),
                    &q, SLOT(quit()));
            connect(&tT, SIGNAL(timeout()), &q, SLOT(quit()));
#ifdef DEBUG_URLFACTORY
            qDebug()<<"Correct GoogleVersion";
#endif //DEBUG_URLFACTORY
            setIsCorrectGoogleVersions(true);
            QString url = "https://maps.google.com/maps/api/js?v=3.2&sensor=false";

            qheader.setUrl(QUrl(url));
            qheader.setRawHeader("User-Agent",UserAgent);
            reply=network.get(qheader);
            tT.start(Timeout);
            q.exec();
            if(!tT.isActive())
                return;
            tT.stop();
            if( (reply->error()!=QNetworkReply::NoError))
            {
#ifdef DEBUG_URLFACTORY
                qDebug()<<"Try corrected version withou abort or error:"<<reply->errorString();
#endif //DEBUG_URLFACTORY
                return;
            }
            QString html=QString(reply->readAll());
            QRegExp reg("\"*https://mt0.google.com/vt/lyrs=m@(\\d*)",Qt::CaseInsensitive);
            if(reg.indexIn(html)!=-1)
            {
                QStringList gc=reg.capturedTexts();
                VersionGoogleMap = QString("m@%1").arg(gc[1]);
                VersionGoogleMapChina = VersionGoogleMap;
#ifdef DEBUG_URLFACTORY
                qDebug()<<"TryCorrectGoogleVersions, VersionGoogleMap: "<<VersionGoogleMap;
#endif //DEBUG_URLFACTORY
            }

            reg=QRegExp("\"*https://mt0.google.com/vt/lyrs=h@(\\d*)",Qt::CaseInsensitive);
            if(reg.indexIn(html)!=-1)
            {
                QStringList gc=reg.capturedTexts();
                VersionGoogleLabels = QString("h@%1").arg(gc[1]);
                VersionGoogleLabelsChina = VersionGoogleLabels;
#ifdef DEBUG_URLFACTORY
                qDebug()<<"TryCorrectGoogleVersions, VersionGoogleLabels: "<<VersionGoogleLabels;
#endif //DEBUG_URLFACTORY
            }
            reg=QRegExp("\"*https://khms\\D?\\d.google.com/kh\\?v=(\\d*)",Qt::CaseInsensitive);
            if(reg.indexIn(html)!=-1)
            {
                QStringList gc=reg.capturedTexts();
                VersionGoogleSatellite = gc[1];
                VersionGoogleSatelliteKorea = VersionGoogleSatellite;
                VersionGoogleSatelliteChina = "s@" + VersionGoogleSatellite;

                qDebug()<<"TryCorrectGoogleVersions, VersionGoogleSatellite: "<<VersionGoogleSatellite;

            }
            reg=QRegExp("\"*https://mt0.google.com/vt/lyrs=t@(\\d*),r@(\\d*)",Qt::CaseInsensitive);
            if(reg.indexIn(html)!=-1)
            {
                QStringList gc=reg.capturedTexts();
                VersionGoogleTerrain = QString("t@%1,r@%2").arg(gc[1],gc[2]);
                VersionGoogleTerrainChina = VersionGoogleTerrain;
#ifdef DEBUG_URLFACTORY
                qDebug()<<"TryCorrectGoogleVersions, VersionGoogleTerrain: "<<VersionGoogleTerrain;
#endif //DEBUG_URLFACTORY
            }
            reply->deleteLater();

        }

    }

    QString UrlFactory::MakeImageUrl(const MapType::Types &type,const Point &pos,const int &zoom,const QString &language)
    {
#ifdef DEBUG_URLFACTORY
        qDebug()<<"Entered MakeImageUrl";
#endif //DEBUG_URLFACTORY

        QString outPut;
        QTextStream Stream(&outPut);

        switch(type)
        {
        case MapType::GoogleMap:
            {
                QString sec1; // after &x=...
                QString sec2; // after &zoom=...
                GetSecGoogleWords(pos,  sec1,  sec2);
                TryCorrectGoogleVersions();

                Stream << "https://mt" << GetServerNum(pos, 4) << ".google.com/vt/lyrs=" << VersionGoogleMap << "&hl=" << language << "&x=" << pos.X() << sec1 << "&y=" << pos.Y() << "&z=" << zoom << "&s=" << sec2;
                return outPut;
            }
            break;
        case MapType::GoogleSatellite:
            {
                QString sec1; // after &x=...
                QString sec2; // after &zoom=...
                GetSecGoogleWords(pos,  sec1,  sec2);
                TryCorrectGoogleVersions();

                Stream << "https://khm" << GetServerNum(pos, 4) << ".google.com/kh/v=" << VersionGoogleSatellite << "&hl=" << language << "&x=" << pos.X() << sec1 << "&y=" << pos.Y() << "&z=" << zoom << "&s=" << sec2;
                return outPut;
            }
            break;
        case MapType::GoogleLabels:
            {
                QString sec1; // after &x=...
                QString sec2; // after &zoom=...
                GetSecGoogleWords(pos,  sec1,  sec2);
                TryCorrectGoogleVersions();

                Stream << "https://mt" << GetServerNum(pos, 4) << ".google.com/vt/lyrs=" << VersionGoogleLabels << "&hl=" << language << "&x=" << pos.X() << sec1 << "&y=" << pos.Y() << "&z=" << zoom << "&s=" << sec2;
                return outPut;
            }
            break;
        case MapType::GoogleTerrain:
            {
                QString sec1; // after &x=...
                QString sec2; // after &zoom=...
                GetSecGoogleWords(pos,  sec1,  sec2);
                TryCorrectGoogleVersions();

                Stream << "https://mt" << GetServerNum(pos, 4) << ".google.com/vt/v=" << VersionGoogleTerrain << "&hl=" << language << "&x=" << pos.X() << sec1 << "&y=" << pos.Y() << "&z=" << zoom << "&s=" << sec2;
                return outPut;
            }
            break;
        case MapType::GoogleMapChina:
            {
                QString sec1; // after &x=...
                QString sec2; // after &zoom=...
                GetSecGoogleWords(pos,  sec1,  sec2);
                TryCorrectGoogleVersions();
                // https://mt0.google.cn/vt/v=w2.101&hl=zh-CN&gl=cn&x=12&y=6&z=4&s=Ga

                Stream << "https://mt" << GetServerNum(pos, 4) << ".google.cn/vt/lyrs=" << VersionGoogleMapChina << "&hl=zh-CN&gl=cn&x=" << pos.X() << sec1 << "&y=" << pos.Y() << "&z=" << zoom << "&s=" << sec2;
                return outPut;
            }
            break;
        case MapType::GoogleSatelliteChina:
            {
                QString sec1; // after &x=...
                QString sec2; // after &zoom=...
                GetSecGoogleWords(pos,  sec1,  sec2);
                //  TryCorrectGoogleVersions();
                // https://khm0.google.cn/kh/v=46&x=12&y=6&z=4&s=Ga

                Stream << "https://mt" << GetServerNum(pos, 4) << ".google.cn/vt/lyrs=" << VersionGoogleSatelliteChina << "&hl=zh-CN&gl=cn&x=" << pos.X() << sec1 << "&y=" << pos.Y() << "&z=" << zoom << "&s=" << sec2;
                return outPut;
            }
            break;
        case MapType::GoogleLabelsChina:
            {
                QString sec1; // after &x=...
                QString sec2; // after &zoom=...
                GetSecGoogleWords(pos,  sec1,  sec2);
                TryCorrectGoogleVersions();
                // https://mt0.google.cn/vt/v=w2t.110&hl=zh-CN&gl=cn&x=12&y=6&z=4&s=Ga

                Stream << "https://mt" << GetServerNum(pos, 4) << ".google.cn/vt/imgtp=png32&lyrs=" << VersionGoogleLabelsChina << "&hl=zh-CN&gl=cn&x=" << pos.X() << sec1 << "&y=" << pos.Y() << "&z=" << zoom << "&s=" << sec2;
                return outPut;
            }
            break;
        case MapType::GoogleTerrainChina:
            {
                QString sec1; // after &x=...
                QString sec2; // after &zoom=...
                GetSecGoogleWords(pos,  sec1,  sec2);
                TryCorrectGoogleVersions();
                // https://mt0.google.cn/vt/v=w2p.110&hl=zh-CN&gl=cn&x=12&y=6&z=4&s=Ga

                Stream << "https://mt" << GetServerNum(pos, 4) << ".google.cn/vt/lyrs=" << VersionGoogleTerrainChina << "&hl=zh-CN&gl=cn&x=" << pos.X() << sec1 << "&y=" << pos.Y() << "&z=" << zoom << "&s=" << sec2;
                return outPut;
            }
            break;
        case MapType::GoogleMapKorea:
            {
                QString sec1; // after &x=...
                QString sec2; // after &zoom=...
                GetSecGoogleWords(pos,  sec1,  sec2);

                //https://mt3.gmaptiles.co.kr/mt/v=kr1.11&hl=lt&x=109&y=49&z=7&s=

                Stream << "https://mt" << GetServerNum(pos, 4) << ".gmaptiles.co.kr/mt/v=" << VersionGoogleMapKorea << "&hl=" << language << "&x=" << pos.X() << sec1 << "&y=" << pos.Y() << "&z=" << zoom << "&s=" << sec2;
                return outPut;
            }
            break;
        case MapType::GoogleSatelliteKorea:
            {
                QString sec1; // after &x=...
                QString sec2; // after &zoom=...
                GetSecGoogleWords(pos,  sec1,  sec2);

                //   https://khm1.google.co.kr/kh/v=54&x=109&y=49&z=7&s=

                Stream << "https://khm" << GetServerNum(pos, 4) << ".google.co.kr/kh/v=" << VersionGoogleSatelliteKorea << "&x=" << pos.X() << sec1 << "&y=" << pos.Y() << "&z=" << zoom << "&s=" << sec2;
                return outPut;
            }
            break;
        case MapType::GoogleLabelsKorea:
            {
                QString sec1; // after &x=...
                QString sec2; // after &zoom=...
                GetSecGoogleWords(pos,  sec1,  sec2);

                //  https://mt1.gmaptiles.co.kr/mt/v=kr1t.11&hl=lt&x=109&y=50&z=7&s=G

                Stream << "https://mt" << GetServerNum(pos, 4) << ".gmaptiles.co.kr/mt/v=" << VersionGoogleLabelsKorea << "&hl=" << language << "&x=" << pos.X() << sec1 << "&y=" << pos.Y() << "&z=" << zoom << "&s=" << sec2;
                return outPut;
            }
            break;
        case MapType::YahooMap:
            {
                Stream << "https://maps" << (GetServerNum(pos, 2)) + 1 << ".yimg.com/hx/tl?v=" << VersionYahooMap << "&.intl=" << language << "&x=" << pos.X() << "&y=" << ((1 << zoom) >> 1) - 1 - pos.Y() << "&z=" << zoom + 1 << "&r=1";
                return outPut;
            }

        case MapType::YahooSatellite:
            {
                Stream << "https://maps" << 3 << ".yimg.com/ae/ximg?v=" << VersionYahooSatellite << "&t=a&s=256&.intl=" << language << "&x=" << pos.X() << "&y=" << ((1 << zoom) >> 1) - 1 - pos.Y() << "&z=" << zoom + 1 << "&r=1";
                return outPut;
            }
            break;
        case MapType::YahooLabels:
            {
                Stream << "https://maps" << 1 << ".yimg.com/hx/tl?v=" << VersionYahooLabels << "&t=h&.intl=" << language << "&x=" << pos.X() << "&y=" << ((1 << zoom) >> 1) - 1 - pos.Y() << "&z=" << zoom + 1 << "&r=1";
                return outPut;
            }
            break;
        case MapType::OpenStreetMap:
            {
                char letter= "abc"[GetServerNum(pos, 3)];
                Stream << "https://" << letter << ".tile.openstreetmap.org/" << zoom << "/" << pos.X() << "/" << pos.Y() << ".png";
                return outPut;
            }
            break;
        case MapType::OpenStreetOsm:
            {
                char letter = "abc"[GetServerNum(pos, 3)];
                Stream << "https://" << letter << ".tah.openstreetmap.org/Tiles/tile/" << zoom << "/" << pos.X() << "/" << pos.Y() << ".png";
                return outPut;
            }
            break;
        case MapType::OpenStreetMapSurfer:
            {
                // https://tiles1.mapsurfer.net/tms_r.ashx?x=37378&y=20826&z=16
                Stream << "https://tiles1.mapsurfer.net/tms_r.ashx?x=" << pos.X() << "&y=" << pos.Y() << "&z=" << zoom;
                return outPut;
            }
            break;
        case MapType::OpenStreetMapSurferTerrain:
            {
                // https://tiles2.mapsurfer.net/tms_t.ashx?x=9346&y=5209&z=14
                Stream << "https://tiles2.mapsurfer.net/tms_t.ashx?x=" << pos.X() << "&y=" << pos.Y() << "&z=" << zoom;
                return outPut;
            }
            break;
        case MapType::BingMap:
            {
                QString key = TileXYToQuadKey(pos.X(), pos.Y(), zoom);
                QString token = !BingMapsClientToken.isEmpty() ? "&token=" + BingMapsClientToken : QString("");

                Stream << "https://ecn.t" << GetServerNum(pos, 4) << ".tiles.virtualearth.net/tiles/r" << key << ".png?g=" << VersionBingMaps << "&mkt=" << language << token;
                return outPut;
            }
            break;
        case MapType::BingSatellite:
            {
                QString key = TileXYToQuadKey(pos.X(), pos.Y(), zoom);
                QString token = !BingMapsClientToken.isEmpty() ? "&token=" + BingMapsClientToken : QString("");

                Stream << "https://ecn.t" << GetServerNum(pos, 4) << ".tiles.virtualearth.net/tiles/a" << key << ".jpeg?g=" << VersionBingMaps << "&mkt=" << language << token;
                return outPut;
            }
            break;
        case MapType::BingHybrid:
            {
                QString key = TileXYToQuadKey(pos.X(), pos.Y(), zoom);
                QString token = !BingMapsClientToken.isEmpty() ? "&token=" + BingMapsClientToken : QString("");

                Stream << "https://ecn.t" << GetServerNum(pos, 4) << ".tiles.virtualearth.net/tiles/h" << key << ".jpeg?g=" << VersionBingMaps << "&mkt=" << language << token;
                return outPut;
            }

        case MapType::ArcGIS_Map:
            {
                // https://server.arcgisonline.com/ArcGIS/rest/services/ESRI_StreetMap_World_2D/MapServer/tile/0/0/0.jpg

                return QString("https://server.arcgisonline.com/ArcGIS/rest/services/ESRI_StreetMap_World_2D/MapServer/tile/%1/%2/%3").arg(zoom, pos.Y(), pos.X());
            }
            break;
        case MapType::ArcGIS_Satellite:
            {
                // https://server.arcgisonline.com/ArcGIS/rest/services/ESRI_Imagery_World_2D/MapServer/tile/1/0/1.jpg

                return QString("https://server.arcgisonline.com/ArcGIS/rest/services/ESRI_Imagery_World_2D/MapServer/tile/%1/%2/%3").arg(zoom, pos.Y(), pos.X());
            }
            break;
        case MapType::ArcGIS_ShadedRelief:
            {
                // https://server.arcgisonline.com/ArcGIS/rest/services/ESRI_ShadedRelief_World_2D/MapServer/tile/1/0/1.jpg

                return QString("https://server.arcgisonline.com/ArcGIS/rest/services/ESRI_ShadedRelief_World_2D/MapServer/tile/%1/%2/%3").arg(zoom, pos.Y(), pos.X());
            }
            break;
        case MapType::ArcGIS_Terrain:
            {
                // https://server.arcgisonline.com/ArcGIS/rest/services/NGS_Topo_US_2D/MapServer/tile/4/3/15

                return QString("https://server.arcgisonline.com/ArcGIS/rest/services/NGS_Topo_US_2D/MapServer/tile/%1/%2/%3").arg(zoom, pos.Y(), pos.X());
            }
            break;
        case MapType::ArcGIS_WorldTopo:
            {
                // https://server.arcgisonline.com/ArcGIS/rest/services/World_Topo_Map/MapServer/tile/4/3/15
                Stream << "https://server.arcgisonline.com/ArcGIS/rest/services/World_Topo_Map/MapServer/tile/" << zoom << "/" << pos.Y() << "/" << pos.X();
                return outPut;
            }
            break;
        case MapType::ArcGIS_MapsLT_OrtoFoto:
            {
                // https://www.maps.lt/ortofoto/mapslt_ortofoto_vector_512/map/_alllayers/L02/R0000001b/C00000028.jpg
                // https://arcgis.maps.lt/ArcGIS/rest/services/mapslt_ortofoto/MapServer/tile/0/9/13
                // return string.Format("https://www.maps.lt/ortofoto/mapslt_ortofoto_vector_512/map/_alllayers/L{0:00}/R{1:x8}/C{2:x8}.jpg", zoom, pos.Y(), pos.X());
                // https://dc1.maps.lt/cache/mapslt_ortofoto_512/map/_alllayers/L03/R0000001c/C00000029.jpg
                // return string.Format("https://arcgis.maps.lt/ArcGIS/rest/services/mapslt_ortofoto/MapServer/tile/{0}/{1}/{2}", zoom, pos.Y(), pos.X());
                // https://dc1.maps.lt/cache/mapslt_ortofoto_512/map/_alllayers/L03/R0000001d/C0000002a.jpg
                //TODO verificar
                return QString("https://dc1.maps.lt/cache/mapslt_ortofoto/map/_alllayers/L%1/R%2/C%3.jpg").arg(zoom,2,10,(QChar)'0').arg(pos.Y(),8,16,(QChar)'0').arg(pos.X(),8,16,(QChar)'0');
            }
            break;
        case MapType::ArcGIS_MapsLT_Map:
            {
                // https://www.maps.lt/ortofoto/mapslt_ortofoto_vector_512/map/_alllayers/L02/R0000001b/C00000028.jpg
                // https://arcgis.maps.lt/ArcGIS/rest/services/mapslt_ortofoto/MapServer/tile/0/9/13
                // return string.Format("https://www.maps.lt/ortofoto/mapslt_ortofoto_vector_512/map/_alllayers/L{0:00}/R{1:x8}/C{2:x8}.jpg", zoom, pos.Y(), pos.X());
                // https://arcgis.maps.lt/ArcGIS/rest/services/mapslt/MapServer/tile/7/1162/1684.png
                // https://dc1.maps.lt/cache/mapslt_512/map/_alllayers/L03/R0000001b/C00000029.png
                //TODO verificar
                // https://dc1.maps.lt/cache/mapslt/map/_alllayers/L02/R0000001c/C00000029.png
                return QString("https://dc1.maps.lt/cache/mapslt/map/_alllayers/L%1/R%2/C%3.png").arg(zoom,2,10,(QChar)'0').arg(pos.Y(),8,16,(QChar)'0').arg(pos.X(),8,16,(QChar)'0');
            }
            break;
        case MapType::ArcGIS_MapsLT_Map_Labels:
            {
                //https://arcgis.maps.lt/ArcGIS/rest/services/mapslt_ortofoto_overlay/MapServer/tile/0/9/13
                //return string.Format("https://arcgis.maps.lt/ArcGIS/rest/services/mapslt_ortofoto_overlay/MapServer/tile/{0}/{1}/{2}", zoom, pos.Y(), pos.X());
                //https://dc1.maps.lt/cache/mapslt_ortofoto_overlay_512/map/_alllayers/L03/R0000001d/C00000029.png
                //TODO verificar
                return QString("https://dc1.maps.lt/cache/mapslt_ortofoto_overlay/map/_alllayers/L%1/R%2/C%3.png").arg(zoom,2,10,(QChar)'0').arg(pos.Y(),8,16,(QChar)'0').arg(pos.X(),8,16,(QChar)'0');
            }
            break;
        case MapType::PergoTurkeyMap:
            {
                // https://{domain}/{layerName}/{zoomLevel}/{first3LetterOfTileX}/{second3LetterOfTileX}/{third3LetterOfTileX}/{first3LetterOfTileY}/{second3LetterOfTileY}/{third3LetterOfTileXY}.png

                // https://map3.pergo.com.tr/tile/00/000/000/001/000/000/000.png
                // That means: Zoom Level: 0 TileX: 1 TileY: 0

                // https://domain/tile/14/000/019/371/000/011/825.png
                // That means: Zoom Level: 14 TileX: 19371 TileY:11825

                //               string x = pos.X().ToString("000000000").Insert(3, "/").Insert(7, "/"); // - 000/000/001
                //               string y = pos.Y().ToString("000000000").Insert(3, "/").Insert(7, "/"); // - 000/000/000
                QString x=QString("%1").arg(QString::number(pos.X()),9,(QChar)'0');
                x.insert(3,"/").insert(7,"/");
                QString y=QString("%1").arg(QString::number(pos.Y()),9,(QChar)'0');
                y.insert(3,"/").insert(7,"/");
                //"https://map03.pergo.com.tr/tile/2/000/000/003/000/000/002.png"
                return QString("https://map%1.pergo.com.tr/tile/%2/%3/%4.png").arg(GetServerNum(pos, 4)).arg(zoom,2,10,(QChar)'0').arg(x).arg(y);
            }
            break;
        case MapType::SigPacSpainMap:
            {
                return QString("https://sigpac.mapa.es/kmlserver/raster/%1@3785/%2.%3.%4.img").arg(levelsForSigPacSpainMap[zoom]).arg(zoom).arg(pos.X()).arg((2 << (zoom - 1)) - pos.Y() - 1);
            }
            break;

        case MapType::YandexMapRu:
            {
                QString server = "vec";

                //https://vec01.maps.yandex.ru/tiles?l=map&v=2.10.2&x=1494&y=650&z=11

                return QString("https://%1").arg(server)+QString("0%2.maps.yandex.ru/tiles?l=map&v=%3&x=%4&y=%5&z=%6").arg(GetServerNum(pos, 4)+1).arg(VersionYandexMap).arg(pos.X()).arg(pos.Y()).arg(zoom);
            }
            break;
        case MapType::Statkart_Topo:
            {
                return QString("https://opencache.statkart.no/gatekeeper/gk/gk.open_gmaps?layers=topo4&zoom=%1&x=%2&y=%3").arg(zoom).arg(pos.X()).arg(pos.Y());
            }
            break;
        case MapType::Statkart_Basemap:
            {
                return QString("https://opencache.statkart.no/gatekeeper/gk/gk.open_gmaps?layers=norgeskart_bakgrunn&zoom=%1&x=%2&y=%3").arg(zoom).arg(pos.X()).arg(pos.Y());
            }
            break;
        case MapType::Eniro_Topo:
            {
                return QString("https://map.eniro.com/geowebcache/service/tms1.0.0/map/%1/%2/%3.png").arg(zoom).arg(pos.X()).arg((1<<zoom)-1-pos.Y());
            }
            break;
        case MapType::JapanMap:
            {
                //char letter = "abc"[GetServerNum(pos, 3)];
                Stream << "https://cyberjapandata.gsi.go.jp/xyz/std/" << zoom << "/" << pos.X() << "/" << pos.Y();
                return outPut;
            }
            break;
        default:
            break;
        }

        return QString();                ;
    }

    void UrlFactory::GetSecGoogleWords(const Point &pos,  QString &sec1, QString &sec2)
    {
        sec1 = ""; // after &x=...
        sec2 = ""; // after &zoom=...
        int seclen = ((pos.X() * 3) + pos.Y()) % 8;
        sec2 = SecGoogleWord.left(seclen);
        if(pos.Y() >= 10000 && pos.Y() < 100000)
        {
            sec1 = "&s=";
        }
    }

    QString UrlFactory::MakeGeocoderUrl(QString keywords)
    {
        QString key = keywords.replace(' ', '+');
        return QString("https://maps.google.com/maps/geo?q=%1&output=csv&key=%2").arg(key, GoogleMapsAPIKey);
    }

    QString UrlFactory::MakeReverseGeocoderUrl(internals::PointLatLng &pt,const QString &language)
    {
        QString output;
        QTextStream Stream(&output);

        Stream << "https://maps.google.com/maps/geo?hl=" << language << "&ll=" << pt.Lat() << "," << pt.Lng() << "&output=csv&key=" << GoogleMapsAPIKey;
        return output;
    }

    internals::PointLatLng UrlFactory::GetLatLngFromGeodecoder(const QString &keywords, GeoCoderStatusCode::Types &status)
    {
        return GetLatLngFromGeocoderUrl(MakeGeocoderUrl(keywords),UseGeocoderCache,status);
    }

    internals::PointLatLng UrlFactory::GetLatLngFromGeocoderUrl(const QString &url, const bool &useCache, GeoCoderStatusCode::Types &status)
    {
#ifdef DEBUG_URLFACTORY
        qDebug()<<"Entered GetLatLngFromGeocoderUrl:";
#endif //DEBUG_URLFACTORY
        status = GeoCoderStatusCode::Unknow;
        internals::PointLatLng ret(0,0);
        QString urlEnd = url.mid(url.indexOf("geo?q=")+6);
        urlEnd.replace( QRegExp(
                "[^"
                "A-Z,a-z,0-9,"
                "\\^,\\&,\\',\\@,"
                "\\{,\\},\\[,\\],"
                "\\,,\\$,\\=,\\!,"
                "\\-,\\#,\\(,\\),"
                "\\%,\\.,\\+,\\~,\\_"
                "]"), "_" );

        QString geo = useCache ? Cache::Instance()->GetGeocoderFromCache(urlEnd) : "";

        if(geo.isEmpty())
        {
#ifdef DEBUG_URLFACTORY
            qDebug()<<"GetLatLngFromGeocoderUrl:Not in cache going internet";
#endif //DEBUG_URLFACTORY
            QNetworkReply *reply;
            QNetworkRequest qheader;
            QNetworkAccessManager network;
            qheader.setUrl(QUrl(url));
            qheader.setRawHeader("User-Agent",UserAgent);
            reply=network.get(qheader);
#ifdef DEBUG_URLFACTORY
            qDebug()<<"GetLatLngFromGeocoderUrl:URL="<<url;
#endif //DEBUG_URLFACTORY
            QElapsedTimer time;
            time.start();
            while( (!(reply->isFinished()) || (time.elapsed()>(6*Timeout))) ){QCoreApplication::processEvents(QEventLoop::AllEvents);}
#ifdef DEBUG_URLFACTORY
            qDebug()<<"Finished?"<<reply->error()<<" abort?"<<(time.elapsed()>Timeout*6);
#endif //DEBUG_URLFACTORY
            if( (reply->error()!=QNetworkReply::NoError) || (time.elapsed()>Timeout*6))
            {
#ifdef DEBUG_URLFACTORY
                qDebug()<<"GetLatLngFromGeocoderUrl::Network error";
#endif //DEBUG_URLFACTORY
                return internals::PointLatLng(0,0);
            }
            {
#ifdef DEBUG_URLFACTORY
                qDebug()<<"GetLatLngFromGeocoderUrl:Reply ok";
#endif //DEBUG_URLFACTORY
                geo=reply->readAll();


                // cache geocoding
                if(useCache && geo.startsWith("200"))
                {
                    Cache::Instance()->CacheGeocoder(urlEnd, geo);
                }
            }
            reply->deleteLater();
        }


        // parse values
        // true : 200,4,56.1451640,22.0681787
        // false: 602,0,0,0
        {
            QStringList values = geo.split(',');
            if(values.count() == 4)
            {
                status = (GeoCoderStatusCode::Types) QString(values[0]).toInt();
                if(status == GeoCoderStatusCode::G_GEO_SUCCESS)
                {
                    double lat = QString(values[2]).toDouble();
                    double lng = QString(values[3]).toDouble();

                    ret = internals::PointLatLng(lat, lng);
#ifdef DEBUG_URLFACTORY
                    qDebug()<<"Lat="<<lat<<" Lng="<<lng;
#endif //DEBUG_URLFACTORY
                }
            }
        }
        return ret;
    }

    Placemark UrlFactory::GetPlacemarkFromGeocoder(internals::PointLatLng location)
    {
        return GetPlacemarkFromReverseGeocoderUrl(MakeReverseGeocoderUrl(location, LanguageStr), UsePlacemarkCache);
    }

    Placemark UrlFactory::GetPlacemarkFromReverseGeocoderUrl(const QString &url, const bool &useCache)
    {

        Placemark ret("");
#ifdef DEBUG_URLFACTORY
        qDebug()<<"Entered GetPlacemarkFromReverseGeocoderUrl:";
#endif //DEBUG_URLFACTORY
        // status = GeoCoderStatusCode::Unknow;
        QString urlEnd = url.right(url.indexOf("geo?hl="));
        urlEnd.replace( QRegExp(
                "[^"
                "A-Z,a-z,0-9,"
                "\\^,\\&,\\',\\@,"
                "\\{,\\},\\[,\\],"
                "\\,,\\$,\\=,\\!,"
                "\\-,\\#,\\(,\\),"
                "\\%,\\.,\\+,\\~,\\_"
                "]"), "_" );

        QString reverse = useCache ? Cache::Instance()->GetPlacemarkFromCache(urlEnd) : "";

        if(reverse.isEmpty())
        {
#ifdef DEBUG_URLFACTORY
            qDebug()<<"GetLatLngFromGeocoderUrl:Not in cache going internet";
#endif //DEBUG_URLFACTORY
            QNetworkReply *reply;
            QNetworkRequest qheader;
            QNetworkAccessManager network;
            qheader.setUrl(QUrl(url));
            qheader.setRawHeader("User-Agent",UserAgent);
            reply=network.get(qheader);
#ifdef DEBUG_URLFACTORY
            qDebug()<<"GetLatLngFromGeocoderUrl:URL="<<url;
#endif //DEBUG_URLFACTORY
            QElapsedTimer time;
            time.start();
            while( (!(reply->isFinished()) || (time.elapsed()>(6*Timeout))) ){QCoreApplication::processEvents(QEventLoop::AllEvents);}
#ifdef DEBUG_URLFACTORY
            qDebug()<<"Finished?"<<reply->error()<<" abort?"<<(time.elapsed()>Timeout*6);
#endif //DEBUG_URLFACTORY
            if( (reply->error()!=QNetworkReply::NoError) || (time.elapsed()>Timeout*6))
            {
#ifdef DEBUG_URLFACTORY
                qDebug()<<"GetLatLngFromGeocoderUrl::Network error";
#endif //DEBUG_URLFACTORY
                return ret;
            }
            {
#ifdef DEBUG_URLFACTORY
                qDebug()<<"GetLatLngFromGeocoderUrl:Reply ok";
#endif //DEBUG_URLFACTORY
                QByteArray a=(reply->readAll());
                QTextCodec *codec = QTextCodec::codecForName("UTF-8");
                reverse = codec->toUnicode(a);
#ifdef DEBUG_URLFACTORY
                qDebug()<<reverse;
#endif //DEBUG_URLFACTORY
                // cache geocoding
                if(useCache && reverse.startsWith("200"))
                {
                    Cache::Instance()->CachePlacemark(urlEnd, reverse);
                }
            }
            reply->deleteLater();
        }


        // parse values
        // true : 200,4,56.1451640,22.0681787
        // false: 602,0,0,0
        if(reverse.startsWith("200"))
        {
            QString acc = reverse.left(reverse.indexOf('\"'));
            ret = Placemark(reverse.remove(reverse.indexOf('\"')));
            ret.SetAccuracy ((int)  (( (QString) acc.split(',')[1]).toInt())    );

        }
        return ret;
    }

    double UrlFactory::GetDistance(internals::PointLatLng p1, internals::PointLatLng p2)
    {
        double dLat1InRad = p1.Lat() * (M_PI / 180);
        double dLong1InRad = p1.Lng() * (M_PI / 180);
        double dLat2InRad = p2.Lat() * (M_PI / 180);
        double dLong2InRad = p2.Lng() * (M_PI / 180);
        double dLongitude = dLong2InRad - dLong1InRad;
        double dLatitude = dLat2InRad - dLat1InRad;
        double a = pow(sin(dLatitude / 2), 2) + cos(dLat1InRad) * cos(dLat2InRad) * pow(sin(dLongitude / 2), 2);
        double c = 2 * atan2(sqrt(a), sqrt(1 - a));
        double dDistance = EarthRadiusKm * c;
        return dDistance;
    }
}
