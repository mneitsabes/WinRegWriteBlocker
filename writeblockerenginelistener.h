#ifndef WRITEBLOCKERENGINELISTENER_H
#define WRITEBLOCKERENGINELISTENER_H

#include "writeblockereventsource.h"

/**
 * Interface for the listeners of the engine.
 */
class WriteBlockerEngineListener {
    public:
        /**
         * Used by the engine to notify the view that the current state has changed.
         *
         * The new state, the source and the duration is provided. The duration is in secondes (if the write protection is off for 1 minute for example).
         *
         * @param isWriteBlocking the current state
         * @param source the source (see WriteBlockEventSource)
         * @param duration the duration (0 if no duration)
         */
        virtual void writeBlockerStateChanged(bool isWriteBlocking, WriteBlockerEventSource source, int duration) = 0;

        /**
         * Used by the engine to notify the view if a error occurs.
         *
         * @param msg the error message
         */
        virtual void writeBlockerError(QString msg) = 0;
};

#endif // WRITEBLOCKERENGINELISTENER_H
