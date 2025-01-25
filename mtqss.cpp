#include "mtqss.h"

/**
 * @brief Constructs an MTQss object with a parent widget.
 * @param parent The parent widget on which the QSS operations will be performed.
 */
MTQss::MTQss(QWidget* parent)
    : QObject(parent), m_parentWidget(parent) {}

/**
 * @brief Exports QSS styles for the given widget and its children.
 * @param fileName The name of the QSS file to export.
 * @return True if the export was successful, false otherwise.
 */
bool MTQss::exportQss(const QString& fileName) {
    if (!m_parentWidget) {
        qWarning() << tr("Parent widget is null.");
        return false;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << tr("Failed to open file for writing:") << fileName;
        return false;
    }

    QTextStream out(&file);

    // Recursive scan for child widgets
    QList<QWidget*> widgets = m_parentWidget->findChildren<QWidget*>();
    widgets.prepend(m_parentWidget); // Include the parent widget

    for (QWidget* widget : widgets) {
        QString className = widget->metaObject()->className();
        QString objectName = widget->objectName();

        if (!objectName.isEmpty()) {
            out << "#" << objectName << " {\n";
            out << "    /* Add your styles here */\n";
            out << "}\n\n";
        }

        if (!className.isEmpty()) {
            out << className << " {\n";
            out << "    /* Add your styles here */\n";
            out << "}\n\n";
        }
    }

    file.close();
    return true;
}

/**
 * @brief Generates a default QSS file template for all widgets in the parent hierarchy.
 * @param fileName The name of the QSS file to generate.
 * @return True if the generation was successful, false otherwise.
 */
bool MTQss::generateQssTemplate(const QString& fileName) {
    return exportQss(fileName); // Reuse the export logic
}

/**
 * @brief Imports QSS styles from a file and applies them to the application.
 * @param fileName The name of the QSS file to import.
 * @return True if the import was successful, false otherwise.
 */
bool MTQss::importQss(const QString& fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << tr("Failed to open file for reading:") << fileName;
        return false;
    }

    QTextStream in(&file);
    QString styleSheet = in.readAll();
    file.close();

    if (m_parentWidget) {
        m_parentWidget->setStyleSheet(styleSheet);
        return true;
    }

    qWarning() << tr("Parent widget is null. Cannot apply styles.");
    return false;
}
