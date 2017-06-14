#include "leveldb/db.h"
#include <QtCore>
#include <iostream>

static double diffclock(clock_t clock1, clock_t clock2) {
    double diffticks = clock1 - clock2;
    double diffms = (diffticks) / (CLOCKS_PER_SEC/1000);
    return diffms;
}

int main()
{
    QStringList data;
    QFile file(":/res/PointStorage/osc_clk.txt");
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
        return 0;

    QTextStream stream(&file);
    while (!stream.atEnd())
        data.append(stream.readLine());

    QVector<QPair<double, int>> points;

    leveldb::DB *db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(
                options, QDir::currentPath().toStdString() + "/data", &db);

    Q_ASSERT(status.ok());
    int count = 0;
    clock_t end = clock();
    for (QString i : data)
    {
        db->Put(leveldb::WriteOptions(), std::to_string(count), i.toStdString());
        count++;
    }
    qDebug() << diffclock(clock(), end);
    leveldb::Iterator *it = db->NewIterator(leveldb::ReadOptions());
    count = 0;
    end = clock();
    for (it->SeekToFirst(); it->Valid(); it->Next())
    {
        QStringList point =
                QString::fromStdString(it->value().ToString()).split(';');
        points.push_back(qMakePair(point.at(0).toDouble(), point.at(1).toInt()));
    }
    qDebug() << diffclock(clock(), end);
    Q_ASSERT(it->status().ok());
    delete it;
    delete db;
}
