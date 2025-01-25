#include "mtsettings.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDomDocument>
#include <QDebug>

/**
 * @brief Constructs an MTSettings object using the provided organization and application names.
 * @param organization The organization name.
 * @param application The application name.
 */
MTSettings::MTSettings(const QString& organization, const QString& application)
    : QSettings(organization, application) {}

/**
 * @brief Exports settings to a file in the specified format.
 * @param format The export format (e.g., JSON, INI, XML, CSV, or Plain Text).
 * @param fileName The name of the file to export to.
 * @param keysToExport The list of keys to export. If empty, exports all keys.
 * @return True if the export was successful, false otherwise.
 */
bool MTSettings::exportSettings(ExportFormat format, const QString& fileName, const QStringList& keysToExport) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << tr("Failed to open file for writing:") << fileName;
        return false;
    }

    QStringList keys = keysToExport.isEmpty() ? allKeys() : keysToExport;

    if (format == JsonFormat) {
        QJsonObject json;
        for (const QString& key : keys) {
            json[key] = value(key).toString();
        }
        QJsonDocument doc(json);
        file.write(doc.toJson(QJsonDocument::Indented));
    } else if (format == IniFormat) {
        QSettings iniSettings(fileName, QSettings::IniFormat);
        for (const QString& key : keys) {
            iniSettings.setValue(key, value(key));
        }
    } else if (format == XmlFormat) {
        QDomDocument doc;
        QDomElement root = doc.createElement("Settings");
        doc.appendChild(root);

        for (const QString& key : keys) {
            QDomElement setting = doc.createElement("Setting");
            setting.setAttribute("key", key);
            setting.setAttribute("value", value(key).toString());
            root.appendChild(setting);
        }
        QTextStream out(&file);
        doc.save(out, 4);
    } else if (format == CsvFormat) {
        QTextStream out(&file);
        for (const QString& key : keys) {
            out << key << "," << value(key).toString() << "\n";
        }
    } else if (format == PlainTextFormat) {
        QTextStream out(&file);
        for (const QString& key : keys) {
            out << key << "=" << value(key).toString() << "\n";
        }
    }

    file.close();
    return true;
}

/**
 * @brief Imports settings from a file in the specified format.
 * @param format The import format (e.g., JSON, INI, XML, CSV, or Plain Text).
 * @param fileName The name of the file to import from.
 * @return True if the import was successful, false otherwise.
 */
bool MTSettings::importSettings(ExportFormat format, const QString& fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << tr("Failed to open file for reading:") << fileName;
        return false;
    }

    if (format == JsonFormat) {
        QByteArray jsonData = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (!doc.isObject()) {
            qWarning() << tr("Invalid JSON format in file:") << fileName;
            return false;
        }
        QJsonObject json = doc.object();
        for (auto it = json.begin(); it != json.end(); ++it) {
            setValue(it.key(), it.value().toString());
        }
    } else if (format == IniFormat) {
        QSettings iniSettings(fileName, QSettings::IniFormat);
        for (const QString& key : iniSettings.allKeys()) {
            setValue(key, iniSettings.value(key));
        }
    } else if (format == XmlFormat) {
        QDomDocument doc;
        if (!doc.setContent(&file)) {
            qWarning() << tr("Invalid XML format in file:") << fileName;
            return false;
        }
        QDomElement root = doc.documentElement();
        QDomNode node = root.firstChild();
        while (!node.isNull()) {
            QDomElement element = node.toElement();
            if (!element.isNull() && element.tagName() == "Setting") {
                QString key = element.attribute("key");
                QString value = element.attribute("value");
                setValue(key, value);
            }
            node = node.nextSibling();
        }
    } else if (format == CsvFormat || format == PlainTextFormat) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = format == CsvFormat ? line.split(",") : line.split("=");
            if (parts.size() == 2) {
                setValue(parts[0], parts[1]);
            }
        }
    }

    file.close();
    return true;
}
