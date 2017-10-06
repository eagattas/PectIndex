#include "pectusviewer.h"

PectusViewer::PectusViewer(QObject *parent) : QObject(parent), m_scanFileName(""), m_renderStatus(false), m_notesActive(false), m_notes("")
{

}

QString PectusViewer::getScanFileName(){
    return m_scanFileName;
}

bool PectusViewer::getRenderStatus(){
    return m_renderStatus;
}

bool PectusViewer::getNotesActive(){
    return m_notesActive;
}

QString PectusViewer::getNotes(){
    return m_notes;
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

void PectusViewer::toggleNotesActive(){
    m_notesActive = !m_notesActive;
    emit notesActiveChanged(m_notesActive);
}

void PectusViewer::updateNotes(const QString &arg){
    m_notes = arg;
    emit notesChanged(arg);
}
