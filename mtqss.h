#ifndef MTQSS_H
#define MTQSS_H

#include <QObject>
#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QDebug>

/**
 * @class MTQss
 * @brief Handles generation, export, and import of QSS (Qt Style Sheets) files.
 */
class MTQss : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs an MTQss object with a parent widget.
     * @param parent The parent widget on which the QSS operations will be performed.
     */
    explicit MTQss(QWidget* parent = nullptr);

    /**
     * @brief Exports QSS styles for the given widget and its children.
     * @param fileName The name of the QSS file to export.
     * @return True if the export was successful, false otherwise.
     */
    bool exportQss(const QString& fileName);

    /**
     * @brief Generates a default QSS file template for all widgets in the parent hierarchy.
     * @param fileName The name of the QSS file to generate.
     * @return True if the generation was successful, false otherwise.
     */
    bool generateQssTemplate(const QString& fileName);

    /**
     * @brief Imports QSS styles from a file and applies them to the application.
     * @param fileName The name of the QSS file to import.
     * @return True if the import was successful, false otherwise.
     */
    bool importQss(const QString& fileName);

};

#endif // MTQSS_H

