#ifndef ENGINEEXCEPTION_H
#define ENGINEEXCEPTION_H

#include <QException>
#include <QMetaType>

/**
 * An exception raised by the engine.
 */
class EngineException : public QException {
    private:
        /**
         * The error message
         */
        QString errorMsg;

    public:
        /**
         * Constructor.
         */
        EngineException();

        /**
         * Constructor.
         *
         * @param errorMsg the error message
         */
        EngineException(QString errorMsg);

        /**
         * Copy constructor.
         */
        EngineException(const EngineException &other);

        /**
         * Destructor.
         */
        ~EngineException();

        /**
         * Returns the error message.
         *
         * @return the error message
         */
        QString getMessage();
};

// See http://doc.qt.io/qt-4.8/custom-types.html
Q_DECLARE_METATYPE(EngineException)

#endif // ENGINEEXCEPTION_H
