#include "pectusviewer.h"

PectusViewer::PectusViewer(QObject *parent) : QObject(parent), m_scanFileName(""), m_renderStatus(false)
{

}

QString PectusViewer::getScanFileName(){
    return m_scanFileName;
}

bool PectusViewer::getRenderStatus(){
    return m_renderStatus;
}

void PectusViewer::setScanFileName(const QString & arg){
    if (arg.endsWith(".obj")){
        m_scanFileName = arg;
        emit scanFileNameChanged(arg);
    }
}

void PectusViewer::renderScan(){
    if (m_scanFileName.endsWith(".obj")){
        m_renderStatus = true;
        emit renderStatusChanged(true);
    }
}
