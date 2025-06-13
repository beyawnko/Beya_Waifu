#ifndef FFPARSE_HELPERS_H
#define FFPARSE_HELPERS_H

#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

// Utility function to run ffprobe and return parsed JSON output.
// ffprobePath should point to the ffprobe executable.
// mediaPath is the file to probe.
inline QJsonDocument parseFfprobeJson(const QString &ffprobePath, const QString &mediaPath)
{
    QProcess proc;
    QString cmd = QString("\"%1\" -v quiet -print_format json -show_streams -show_format -i \"%2\"")
                     .arg(ffprobePath, mediaPath);
    proc.start(cmd);
    proc.waitForFinished(-1);
    QByteArray output = proc.readAllStandardOutput();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(output, &err);
    if (err.error != QJsonParseError::NoError)
        return QJsonDocument();
    return doc;
}

#endif // FFPARSE_HELPERS_H
