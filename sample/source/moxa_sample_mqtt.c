// Copyright (C) 2019 Moxa Inc. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

// Include Standard Library
#include <unistd.h>
// Include ioThinx I/O Library
#include <iothinx/iothinxio.h>
// Include Azure Library
#include "serializer.h"
#include "azure_c_shared_utility/platform.h"
#include "iothub_client.h"
#include "iothubtransportmqtt.h"

/*String containing Hostname, Device Id & Device Key in the format:               */
/*    "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"    */
static const char* connectionString = "[device connection string]";

/*
 * BEGIN_NAMESPACE:
 *   - This macro marks the start of a section that declares the model elements (like complex types, etc.). Declarations are typically placed in header files, so that they can be shared between translation units.
 * Parameter:
 *   - schemaNamespace  : The schema namespace.
 */
BEGIN_NAMESPACE(MOXA_SAMPLE);

/*
 * DECLARE_MODEL:
 *   - This macro allows declaring a model that can be later used to instantiate a device.
 * Parameter:
 *   - modelName                : Specifies the model name.
 *   - element1, element2, ...  : A model element (can be a property and action). A property is described in a model by using the WITH_DATA. An action is described in a model by using the WITH_ACTION macro.
 */
DECLARE_MODEL(IOTHINX_IO,

              /*
               * WITH_DATA:
               *   - The WITH_DATA macro allows declaring a model property in a model. A property can be serialized by using the SERIALIZE macro.
               * Parameter:
               *   - propertyType   : Specifies the property type. Can be any of the following types:
               *                      int, double, float, long, int8_t, uint8_t, int16_t, int32_t, int64_t, bool, ascii_char_ptr, EDM_DATE_TIME_OFFSET, EDM_GUID, EDM_BINARY, Any struct type previously introduced by another DECLARE_STRUCT.
               *   - propertyName   : Specifies the property name.
               */
              WITH_DATA(ascii_char_ptr, DeviceId),
              WITH_DATA(int, DI_Values),
              WITH_DATA(int, DO_Values),

              /*
               * WITH_ACTION:
               *   - The WITH_ACTION macro allows declaring a model action. Once the action is declared, it will have to be complemented by a C function that defines the action. The C function prototype is the following:
               *     EXECUTE_COMMAND_RESULT actionName(modelName* model, arg1Type arg1Name, arg2Type arg2Name) {} // more arguments can follow if more are declared in WITH_ACTION
               * Parameter:
               *   - actionName         : Specifies the action name.
               *   - argXtype, argXName : Defines the type and name for the Xth argument of the action. The type can be any of the primitive types or a struct type.
               */
              WITH_ACTION(DO_SetValues, int, DO_Values)
             );

/*
 * BEGIN_NAMESPACE:
 *   - This macro marks the end of a section that declares the model elements.
 * Parameter:
 *   - schemaNamespace  : The schema namespace as specified in BEGIN_NAMESPACE macro.
 */
END_NAMESPACE(MOXA_SAMPLE);

/*
 * DO_SetValues:
 *   - The callback trigger by model action when receiving DO value from IoT Hub and set it to I/O module.
 * Return:
 *   - EXECUTE_COMMAND_SUCCESS, when the final recipient of the command indicates a successful execution.
 *   - EXECUTE_COMMAND_ERROR, when a transient error either in the final recipient or until the final recipient.
 * Parameter:
 *   - iothinxio    : A pointer to a structure of type modelName.
 *   - DO_Values    : DO values of the contiguous channels; each bit holds one channel's value. The bit 0 represents digital output status of the channel 0 and so on.
 */
EXECUTE_COMMAND_RESULT DO_SetValues(IOTHINX_IO *iothinxio, int DO_Values)
{
    int rc = 0;
    uint32_t slot = 1;

    // ioThinx: Set DO value.
    rc = ioThinx_DO_SetValues(slot, DO_Values);
    if (rc != IOTHINX_ERR_OK)
    {
        printf("ioThinx_DO_SetValues() = %d\r\n", rc);
        return EXECUTE_COMMAND_ERROR;
    }

    return EXECUTE_COMMAND_SUCCESS;
}

/*
 * send_Callback:
 *   - The callback specified by the device for receiving confirmation of the delivery of the IoT Hub message.
 * Parameter:
 *   - result               : Enumeration passed in by the IoT Hub when the event confirmation callback is invoked to indicate status of the event processing in the hub.
 *   - usercontextcallback  : User specified context that will be provided to the callback. This can be NULL.
 */
static void send_Callback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *usercontextcallback)
{
    printf("%s\r\n", ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
    return;
}

/*
 * recv_Callback:
 *   - The callback specified by the device for receiving messages from IoT Hub.
 * Return:
 *   - IOTHUBMESSAGE_ACCEPTED, the client will accept the message, meaning that it will not be received again by the client.
 *   - IOTHUBMESSAGE_REJECTED, the message will be rejected. The message will not be resent to the device.
 *   - IOTHUBMESSAGE_ABANDONED, the message will be abandoned. The implies that the user could not process the message but it expected that the message will be resent to the device from the service. Message is only valid in the scope of the callback.
 * Parameter:
 *   - iothubmessagehandle  : Handle value that is used when invoking other functions for IoT Hub message.
 *   - usercontextcallback  : User specified context that will be provided to the callback. This can be NULL.
 */
static IOTHUBMESSAGE_DISPOSITION_RESULT recv_Callback(IOTHUB_MESSAGE_HANDLE iothubmessagehandle, void *usercontextcallback)
{
    int rc = 0;
    const unsigned char *buf = NULL;
    unsigned char *tmp = NULL;
    size_t buf_size = 0;

    // Azure: Fetches a pointer and size for the data associated with the IoT hub message handle.
    rc = IoTHubMessage_GetByteArray(iothubmessagehandle, &buf, &buf_size);
    if (rc != IOTHUB_MESSAGE_OK)
    {
        printf("IoTHubMessage_GetByteArray() = %d\r\n", rc);
        printf("%s\r\n", ENUM_TO_STRING(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ABANDONED));
        return IOTHUBMESSAGE_ABANDONED;
    }

    // buf is not zero terminated.
    tmp = calloc(buf_size + 1, sizeof(char));
    if (tmp == NULL)
    {
        printf("calloc() = %s\r\n", "NULL");
        printf("%s\r\n", ENUM_TO_STRING(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ABANDONED));
        return IOTHUBMESSAGE_ABANDONED;
    }

    memcpy(tmp, buf, buf_size);

    // Azure: Execute command.
    rc = EXECUTE_COMMAND(usercontextcallback, tmp);
    switch (rc)
    {
    case EXECUTE_COMMAND_FAILED:
        free(tmp);
        printf("%s\r\n", ENUM_TO_STRING(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_REJECTED));
        return IOTHUBMESSAGE_REJECTED;
    case EXECUTE_COMMAND_ERROR:
        free(tmp);
        printf("%s\r\n", ENUM_TO_STRING(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ABANDONED));
        return IOTHUBMESSAGE_ABANDONED;
    default:
        break;
    }

    free(tmp);
    printf("%s\r\n", ENUM_TO_STRING(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ACCEPTED));
    return IOTHUBMESSAGE_ACCEPTED;
}

/*
 * send_Message:
 *   - Send device message data to IoT Hub.
 * Return:
 *   - 0 on success or any other error on failure.
 * Parameter:
 *   - iothubclienthandle   : Handle value that is used when invoking other functions for IoT Hub client.
 *   - iothinxio            : A pointer to a structure of type modelName.
 */
static int send_Message(IOTHUB_CLIENT_HANDLE iothubclienthandle, IOTHINX_IO *iothinxio)
{
    int rc = 0;
    unsigned char *buf = NULL;
    size_t buf_size = 0;

    IOTHUB_MESSAGE_HANDLE iothubmessagehandle = NULL;

    // Azure: Serialize data.
    rc = SERIALIZE(&buf, &buf_size, iothinxio->DeviceId, iothinxio->DI_Values, iothinxio->DO_Values);
    if (rc != CODEFIRST_OK)
    {
        printf("SERIALIZE() = %d\r\n", rc);
        return -1;
    }

    // Azure: Creates a new IoT hub message from a byte array.
    iothubmessagehandle = IoTHubMessage_CreateFromByteArray(buf, buf_size);
    if (iothubmessagehandle == NULL)
    {
        free(buf);
        printf("IoTHubMessage_CreateFromByteArray() = %s\r\n", "NULL");
        return -1;
    }

    // Azure: Asynchronous call to send the message specified by eventMessageHandle.
    rc = IoTHubClient_SendEventAsync(iothubclienthandle, iothubmessagehandle, send_Callback, NULL);
    if (rc != IOTHUB_CLIENT_OK)
    {
        // Azure: Frees all resources associated with the given message handle.
        IoTHubMessage_Destroy(iothubmessagehandle);
        free(buf);
        printf("IoTHubClient_SendEventAsync() = %d\r\n", rc);
        return -1;
    }

    // Azure: Frees all resources associated with the given message handle.
    IoTHubMessage_Destroy(iothubmessagehandle);
    free(buf);
    return 0;
}

/*
 * Azure_IoT_Init:
 *   - Initialize the Azure IoT.
 * Return:
 *   - 0 on success or any other error on failure.
 * Parameter:
 *   - iothubclienthandle   : Handle value that is used when invoking other functions for IoT Hub client.
 *   - iothinxio            : A pointer to a structure of type modelName.
 */
static int Azure_IoT_Init(IOTHUB_CLIENT_HANDLE *iothubclienthandle, IOTHINX_IO **iothinxio)
{
    int rc = 0;

    // Azure: Initialize platform.
    rc = platform_init();
    if (rc != 0)
    {
        printf("platform_init() = %d\r\n", rc);
        return -1;
    }

    // Azure: Initialize serializer.
    rc = serializer_init(NULL);
    if (rc != SERIALIZER_OK)
    {
        printf("serializer_init() = %d\r\n", rc);
        // Azure: Deinitialize platform.
        platform_deinit();
        return -1;
    }

    // Azure: Create iothubclient handle.
    *iothubclienthandle = IoTHubClient_CreateFromConnectionString(connectionString, MQTT_Protocol);
    if (*iothubclienthandle == NULL)
    {
        printf("IoTHubClient_CreateFromConnectionString() = %s\r\n", "NULL");
        // Azure: Deinitialize serializer.
        serializer_deinit();
        // Azure: Deinitialize platform.
        platform_deinit();
        return -1;
    }

    // Azure: Create data model.
    *iothinxio = CREATE_MODEL_INSTANCE(MOXA_SAMPLE, IOTHINX_IO);
    if (*iothinxio == NULL)
    {
        printf("CREATE_MODEL_INSTANCE() = %s\r\n", "NULL");
        // Azure: Disposes of resources allocated by the IoT Hub client.
        IoTHubClient_Destroy(*iothubclienthandle);
        // Azure: Deinitialize serializer.
        serializer_deinit();
        // Azure: Deinitialize platform.
        platform_deinit();
        return -1;
    }

    // Azure: Set receive callback.
    rc = IoTHubClient_SetMessageCallback(*iothubclienthandle, recv_Callback, *iothinxio);
    if (rc != IOTHUB_CLIENT_OK)
    {
        printf("IoTHubClient_SetMessageCallback() = %d\r\n", rc);
        // Azure: Destroy data model.
        DESTROY_MODEL_INSTANCE(*iothinxio);
        // Azure: Disposes of resources allocated by the IoT Hub client.
        IoTHubClient_Destroy(*iothubclienthandle);
        // Azure: Deinitialize serializer.
        serializer_deinit();
        // Azure: Deinitialize platform.
        platform_deinit();
        return -1;
    }

    return 0;
}

int main(int argc, char const *argv[])
{
    int rc = 0;
    uint32_t slot = 1;
    uint32_t di_values = 0;
    uint32_t do_values = 0;

    IOTHUB_CLIENT_HANDLE iothubclienthandle = NULL;
    IOTHINX_IO *iothinxio = NULL;

    // ioThinx: Initialize I/O.
    rc = ioThinx_IO_Client_Init();
    if (rc != IOTHINX_ERR_OK)
    {
        printf("ioThinx_IO_Client_Init() = %d\r\n", rc);
        return -1;
    }

    // Azure: Initialize IoT.
    rc = Azure_IoT_Init(&iothubclienthandle, &iothinxio);
    if (rc != 0)
    {
        return -1;
    }

    // ioThinx: Get DI value.
    rc = ioThinx_DI_GetValues(slot, &di_values);
    if (rc != IOTHINX_ERR_OK)
    {
        printf("ioThinx_DI_GetValues() = %d\r\n", rc);
        return -1;
    }

    // ioThinx: Get DO value.
    rc = ioThinx_DO_GetValues(slot, &do_values);
    if (rc != IOTHINX_ERR_OK)
    {
        printf("ioThinx_DO_GetValues() = %d\r\n", rc);
        return -1;
    }

    // Azure: Set data.
    iothinxio->DeviceId = "ioThinx";
    iothinxio->DI_Values = di_values;
    iothinxio->DO_Values = do_values;

    // Azure: Send data.
    rc = send_Message(iothubclienthandle, iothinxio);
    if (rc != 0)
    {
        printf("send_Message() = %d\r\n", rc);
        // Azure: Destroy data model.
        DESTROY_MODEL_INSTANCE(iothinxio);
        // Azure: Disposes of resources allocated by the IoT Hub client.
        IoTHubClient_Destroy(iothubclienthandle);
        // Azure: Deinitialize serializer.
        serializer_deinit();
        // Azure: Deinitialize platform.
        platform_deinit();
        return -1;
    }

    while (true)
    {
        // ioThinx: Get DI value.
        rc = ioThinx_DI_GetValues(slot, &di_values);
        if (rc != IOTHINX_ERR_OK)
        {
            printf("ioThinx_DI_GetValues() = %d\r\n", rc);
            return -1;
        }

        // ioThinx: Get DO value.
        rc = ioThinx_DO_GetValues(slot, &do_values);
        if (rc != IOTHINX_ERR_OK)
        {
            printf("ioThinx_DO_GetValues() = %d\r\n", rc);
            return -1;
        }

        // If any value change of DI or DO.
        if (iothinxio->DI_Values != di_values || iothinxio->DO_Values != do_values)
        {
            // Azure: Update data.
            iothinxio->DI_Values = di_values;
            iothinxio->DO_Values = do_values;

            // Azure: Send data.
            rc = send_Message(iothubclienthandle, iothinxio);
            if (rc != 0)
            {
                printf("send_Message() = %d\r\n", rc);
                // Azure: Destroy data model.
                DESTROY_MODEL_INSTANCE(iothinxio);
                // Azure: Disposes of resources allocated by the IoT Hub client.
                IoTHubClient_Destroy(iothubclienthandle);
                // Azure: Deinitialize serializer.
                serializer_deinit();
                // Azure: Deinitialize platform.
                platform_deinit();
                return -1;
            }
        }

        // Sleep 1 second.
        sleep(1);
    }

    // Azure: Destroy data model.
    DESTROY_MODEL_INSTANCE(iothinxio);
    // Azure: Disposes of resources allocated by the IoT Hub client.
    IoTHubClient_Destroy(iothubclienthandle);
    // Azure: Deinitialize serializer.
    serializer_deinit();
    // Azure: Deinitialize platform.
    platform_deinit();
    return 0;
}
