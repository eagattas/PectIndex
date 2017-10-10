#include "pectusprocessor.h"
#include <QFile>


PectusProcessor::PectusProcessor(QObject *parent) : QObject(parent), m_fileName(""), vertices(), faces(){

}


void PectusProcessor::setFileName(const QString & filename){
    QString newname = filename.right(filename.size() - 8);//removes file:///
    m_fileName = newname;
    processFile();
}

void PectusProcessor::processFile(){
    QFile inputFile(m_fileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          QStringList parts = line.split(" ");
          if(parts[0] == "v"){
              Vertex v(parts[1].toDouble(), parts[2].toDouble(), parts[3].toDouble());
              vertices.push_back(v);
          }
          else if(parts[0] == "vt"){
              Texture v(parts[1].toDouble(), parts[2].toDouble());
              textures.push_back(v);
          }
          else if(parts[0] == "vn"){
              Vertex v(parts[1].toDouble(), parts[2].toDouble(), parts[3].toDouble());
              normals.push_back(v);
          }
          else if(parts[0] == "f"){
              Face v(parts[1], parts[2], parts[3]);
              faces.push_back(v);
          }
       }
       inputFile.close();
    }
    else{
        qDebug() << QString("Not open");
    }
}

QString PectusProcessor::getFileName(){
    return m_fileName;
}
