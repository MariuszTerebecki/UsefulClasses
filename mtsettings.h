#ifndef MTSETTINGS_H
#define MTSETTINGS_H

#include <QSettings>
#include <QString>
#include <QStringList>
#include <QVariant>

/**
 * @class MTSettings
 * @brief An extended QSettings class with additional functionality for exporting and importing settings.
 */
class MTSettings : public QSettings {
public:
    /**
     * @brief Supported export and import formats.
     */
    enum ExportFormat {
        JsonFormat,      ///< JSON format
        IniFormat,       ///< INI format
        XmlFormat,       ///< XML format
        CsvFormat,       ///< CSV format
        PlainTextFormat  ///< Plain text format
    };

    /**
     * @brief Constructs an MTSettings object.
     * @param organization The organization name.
     * @param application The application name.
     */
    MTSettings(const QString& organization, const QString& application);

    /**
     * @brief Exports settings to a file in the specified format.
     * @param format The export format (e.g., JSON, INI, XML, CSV, or Plain Text).
     * @param fileName The name of the file to export to.
     * @param keysToExport The list of keys to export. If empty, exports all keys.
     * @return True if the export was successful, false otherwise.
     */
    bool exportSettings(ExportFormat format, const QString& fileName, const QStringList& keysToExport = QStringList());

    /**
     * @brief Imports settings from a file in the specified format.
     * @param format The import format (e.g., JSON, INI, XML, CSV, or Plain Text).
     * @param fileName The name of the file to import from.
     * @return True if the import was successful, false otherwise.
     */
    bool importSettings(ExportFormat format, const QString& fileName);

private:
         // No additional members needed since QSettings handles storage.
};

#endif // MTSETTINGS_H
