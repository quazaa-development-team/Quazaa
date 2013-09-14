#include "dialogneighbourinfo.h"
#include "ui_dialogneighbourinfo.h"

#include "geoiplist.h"
#include "neighbours.h"
#include "networktype.h"

DialogNeighbourInfo::DialogNeighbourInfo(CNeighboursTableModel::Neighbour *pNeighbour, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNeighbourInfo)
{
    ui->setupUi(this);

    ui->textEditConnectionInfo->setHtml(QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"> p, li { white-space: pre-wrap; }</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\"><p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt; font-weight:600;\">%1<br /></span><img src=\"%2\" width=\"20\" height=\"20\" /><span style=\" font-size:8pt;\"> %3<br />%4<br />%5</span></p></body></html>")
                                        .arg(pNeighbour->oAddress.toStringWithPort())
                                        .arg(":/Resource/Flags/" + pNeighbour->sCountry.toLower() + ".png")
                                        .arg(GeoIP.countryNameFromCode(pNeighbour->sCountry))
                                        .arg(pNeighbour->sUserAgent)
                                        .arg(neighbourConnectionDescription(pNeighbour)));
    ui->labelNetworkImage->setPixmap(pNeighbour->iNetwork.pixmap(QSize(41,41)));
}

DialogNeighbourInfo::~DialogNeighbourInfo()
{
    delete ui;
}

QString DialogNeighbourInfo::neighbourConnectionDescription(CNeighboursTableModel::Neighbour *pNeighbour)
{
    if(pNeighbour->nDiscoveryProtocol == dpG2) {
        QString selfMode;
        QString otherMode;

        if(Neighbours.IsG2Hub())
        {
            selfMode = "Hub";
        } else {
            selfMode = "Leaf";
        }

        switch (pNeighbour->nType) {
            case G2_HUB:
            otherMode = "Hub";
            break;
            case G2_LEAF:
            otherMode = "Leaf";
            break;
            default:
            otherMode = tr("Unknown");
            break;
        }

        return tr("%1: %2-to-%3 Connection").arg("Gnutella 2").arg(selfMode).arg(otherMode);
    } else {
        return tr("Unknown Connection Type");
    }
}
