#ifndef PECTUSVIEWER_H
#define PECTUSVIEWER_H

#include <QObject>
#include <QDebug>

class PectusViewer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString visibleFileName READ getVisibleFileName NOTIFY visibleFileNameChanged)
    Q_PROPERTY(QString scanFileName READ getScanFileName NOTIFY scanFileNameChanged)
    Q_PROPERTY(bool renderStatus READ getRenderStatus NOTIFY renderStatusChanged)
    Q_PROPERTY(bool notesActive READ getNotesActive NOTIFY notesActiveChanged)
    Q_PROPERTY(QString notes READ getNotes NOTIFY notesChanged)

public:
    explicit PectusViewer(QObject *parent = nullptr);

    QString getVisibleFileName();
    QString getScanFileName();
    bool getRenderStatus();
    bool getNotesActive();
    QString getNotes();
    Q_INVOKABLE void setVisibleFileName(const QString & arg);
    Q_INVOKABLE void setScanFileName(const QString & arg);
    Q_INVOKABLE void renderScan();
    Q_INVOKABLE void toggleNotesActive();
    Q_INVOKABLE void updateNotes(const QString & arg);

private:
    QString m_visibleFileName; // Filename shown in "filename" box
    QString m_scanFileName; // True filename processed. Not set until "Render 3D Scan" is selected
    bool m_renderStatus;
    bool m_notesActive;
    QString m_notes;


signals:
    void visibleFileNameChanged(const QString & arg);
    void scanFileNameChanged(const QString & arg);
    void renderStatusChanged(bool arg);
    void notesActiveChanged(bool arg);
    void notesChanged(const QString & arg);


public slots:
};

#endif // PECTUSVIEWER_H
