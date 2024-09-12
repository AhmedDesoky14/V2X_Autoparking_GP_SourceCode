#include "filereading.h"

#include <QFile>
#include <QTextStream>

fileReading::fileReading(QObject *parent)
    : QObject{parent}
{}

QString fileReading::readFileFunc(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "Error: Could not open file";
    }
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    return content;


}
