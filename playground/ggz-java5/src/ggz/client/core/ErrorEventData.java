package ggz.client.core;

import ggz.common.ClientReqError;

/**
 * @brief The data describing an error.
 * 
 * When an error occurrs, a pointer to a struct of this type will be passed as
 * the event data.
 */
public class ErrorEventData {

    /**
     * @brief A default error message.
     * @note This also provides backward-compatability.
     */
    public String message;

    /**
     * @brief The type of error that occurred.
     * @note Not all errors are possible with all events.
     */
    public ClientReqError status;
    
    ErrorEventData(ClientReqError code) {
        this.status = code;
    }
}
