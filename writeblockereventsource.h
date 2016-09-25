#ifndef WRITEBLOCKEREVENTSOURCE_H
#define WRITEBLOCKEREVENTSOURCE_H

/**
 * Enumeration for the source of a event.
 */
enum WriteBlockerEventSource {
    /**
     * The application (engine or user)
     */
    APPLICATION,

    /**
     * Third party software (eg. regedit)
     */
    EXTERNAL
};

#endif // WRITEBLOCKEREVENTSOURCE_H
