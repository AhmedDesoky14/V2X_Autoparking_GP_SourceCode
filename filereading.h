#ifndef FILEREADING_H
#define FILEREADING_H

#include <QObject>
#include <QString>

class fileReading: public QObject
{
     Q_OBJECT
public:
     explicit fileReading(QObject *parent = nullptr);

    Q_INVOKABLE QString readFileFunc(const QString &filePath);

};

#endif // FILEREADING_H
