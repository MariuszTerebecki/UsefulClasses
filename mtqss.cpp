#include "mtqss.h"
#include <QRegularExpression>

/**
 * @brief Constructs an MTQss object with a parent widget.
 * @param parent The parent widget on which the QSS operations will be performed.
 */
MTQss::MTQss(QWidget* parent)
    : QObject(parent) {}

/**
 * @brief Exports QSS styles for the given widget and its children.
 * @param fileName The name of the QSS file to export.
 * @return True if the export was successful, false otherwise.
 */
bool MTQss::exportQss(const QString& fileName) {
    if (!parent()) {
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
    QList<QWidget*> widgets = parent()->findChildren<QWidget*>();
    widgets.prepend(qobject_cast<QWidget *>(parent())); // Include the parent widget

    for (QWidget* widget : widgets) {
        QString className = widget->metaObject()->className();
        QString objectName = widget->objectName();
        QString styleSheet = widget->styleSheet();

        // Write object-specific styles
        if (!objectName.isEmpty()) {
            out << "#" << objectName << " {\n";
            if (!styleSheet.isEmpty()) {
                out << "    " << styleSheet << "\n";
            } else {
                out << "    \n";
            }
            out << "}\n\n";
        }

        // Write class-specific styles
        if (!className.isEmpty()) {
            out << className << " {\n";
            if (!styleSheet.isEmpty()) {
                out << "    " << styleSheet << "\n";
            } else {
                out << "    \n";
            }
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

    QWidget *parent = qobject_cast<QWidget *>(this->parent());

    if (!parent) {
        qWarning() << tr("Parent widget is null. Cannot apply styles.");
        return false;
    }

    // Parse the QSS file and handle styles
    QRegularExpression regex(R"(#(\w+)\s*{([\s\S]*?)}|(\w+)\s*{([\s\S]*?)})");
    QRegularExpressionMatchIterator it = regex.globalMatch(styleSheet);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();

        QString objectName = match.captured(1); // Object-specific selector
        QString objectStyles = match.captured(2);
        QString className = match.captured(3); // Class-specific selector
        QString classStyles = match.captured(4);

        if (!objectName.isEmpty()) {
            // Special handling for parent widget (if it's identified as #TestWindow)
            if (objectName == parent->objectName()) {
                parent->setStyleSheet(objectStyles.trimmed());
            } else {
                QWidget* widget = parent->findChild<QWidget*>(objectName);
                if (widget) {
                    widget->setStyleSheet(objectStyles.trimmed());
                }
            }
        } else if (!className.isEmpty()) {
            // Apply styles to all widgets of the given class
            QList<QWidget*> widgets = parent->findChildren<QWidget*>(className);
            for (QWidget* widget : widgets) {
                widget->setStyleSheet(classStyles.trimmed());
            }
        }
    }

    return true;
}
