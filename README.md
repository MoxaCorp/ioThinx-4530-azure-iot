<!-- Copyright (C) 2019 Moxa Inc. All rights reserved. -->
<!-- SPDX-License-Identifier: Apache-2.0               -->

# AZURE-IOT-SDK-C

> **Table of Contents**
>
> [Cloud: Create device instance for connecting by SDK program](#cloud-azure)
>
> [Host: Cross-compiling the SDK](#host-x86_64-linux)
>
> [Target: Executing the SDK program](#target-arm-linux)

## Cloud (Azure)

> Create device instance for connecting by SDK program

### Sign in to Cloud

* Sign in to [Azure IoT Cloud][cloud]. If you do not have an account, please register a new one.

### Create IoT Hub

1. In the left navigation pane, choose **Create a resource** > **Internet of Things** > **IoT Hub**.

    ![create_iot_hub_01][create_iot_hub_01]

2. On the **Basics** tab, fill in the necessary information then choose **Next: Size and scale »**.

    ![create_iot_hub_02][create_iot_hub_02]

3. On the **Size and scale** tab, fill in the necessary information then choose **Review + create**.

    ![create_iot_hub_03][create_iot_hub_03]

4. On the **Review + create** tab, review the information you fill before then choose **Create**.

    ![create_iot_hub_04][create_iot_hub_04]

5. Creating the IoT Hub will takes a few minutes.

    ![create_iot_hub_05][create_iot_hub_05]

6. Finish creating the IoT Hub.

    ![create_iot_hub_06][create_iot_hub_06]

### Create Device

1. In the left navigation pane, choose **All resources**, then search and choose the **[IoT Hub you create before]**.

    ![create_device_01][create_device_01]

2. On the **Explorers** item, choose **IoT devices** > **Add**.

    ![create_device_02][create_device_02]

3. On the **Create a device** page, fill in the necessary information then choose **Save**.

    ![create_device_03][create_device_03]

4. Finish creating the Device.

    ![create_device_04][create_device_04]

### Copy Device Connection String

> **Device Connection String** is the key to build connection between physical and virtual device on cloud
>
> You will need this infomation in the section [Build the SDK](#build-the-sdk)

1. In the left navigation pane, choose **All resources**, then search and choose the **[IoT Hub you create before]**.

    ![copy_device_connection_string_01][copy_device_connection_string_01]

2. On the **Explorers** item, choose **IoT devices** > **[Device you create before]**.

    ![copy_device_connection_string_02][copy_device_connection_string_02]

3. Finish copying the **Connection String (primary key)** of Device.

    ![copy_device_connection_string_03][copy_device_connection_string_03]

### View Device Messages

> You can view the following device messages after section [Execute the SDK](#execute-the-sdk)

1. On the right side of the search bar above, choose >_ to open **Cloud Shell**.

    ![view_device_messages_01][view_device_messages_01]

2. Add extension **azure-cli-iot-ext** to Azure CLI.

    ```
    user@Azure:~$ az extension add --name azure-cli-iot-ext
    ```

3. Monitor device telemetry and messages that sent to IoT Hub.

    ```
    user@Azure:~$ az iot hub monitor-events -n Example-IoT-Hub
    ```
    ![view_device_messages_02][view_device_messages_02]

## Host (x86_64-linux)

> Cross-compiling the SDK

### Setup the Environment

1. Setup a network connection to allow host able to access the network.

2. Install GNU cross-toolchain provide by MOXA.

3. Install following package from package manager.

    ```
    cmake git rsync tree vim
    ```

### Build the SDK

1. Clone repository of MOXA cloud connectivity tool from github.

    ```
    user@Linux:~$ git clone https://github.com/MoxaCorp/ioThinx-4530-azure-iot.git
    ```

2. Setup dependencies and SDK to output directory.

    ```
    user@Linux:~$ cd azure
    ```
    ```
    user@Linux:~/azure$ ./setup.sh
    ```
    * For more setup.sh options.

    ```
    user@Linux:~/azure$ ./setup.sh --help

    Usage: ./setup.sh [options]

    Options:
        -git                Git repository of SDK.
                            Default: https://github.com/Azure/azure-iot-sdk-c.git

        -ver                Version of SDK.
                            Default: 2018-10-03

        --toolchain         GNU cross-toolchain directory.
                            Default: /usr/local/arm-linux-gnueabihf

        --help              Display this help and exit.

    Examples:
        Default             ./setup.sh
        Specify             ./setup.sh -git https://github.com/Azure/azure-iot-sdk-c.git -ver 2018-10-03
                            ./setup.sh --toolchain /usr/local/arm-linux-gnueabihf
    ```

3. Add the **Connection String (primary key)** of Device to SDK sample code such as example **simplesample_amqp.c**. [[Copy Connection String](#copy-device-connection-string)]

    ```
    user@Linux:~/azure$ vim output/sdk_azure/serializer/samples/simplesample_amqp/simplesample_amqp.c
    ```
    ```
    /*String containing Hostname, Device Id & Device Key in the format:             */
    /*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"    */
    static const char* connectionString = "HostName=Example-IoT-Hub.azure-devices.net;DeviceId=Example-Device;SharedAccessKey=hQUkHWv6Gq+1r2yQU5vLzQ86KUjyIquow+XuuDG1DqM=";
    ```

4. Build the whole SDK.

    ```
    user@Linux:~/azure$ ./build.sh
    ```
    * All compiled SDK program can be found in the following directory, including example **simplesample_amqp**.

    ```
    user@Linux:~/azure$ tree output/sdk_azure/cmake/iotsdk_linux/serializer/samples
    output/sdk_azure/cmake/iotsdk_linux/serializer/samples
    ├── devicemethod_simplesample
    │   └── devicemethod_simplesample
    ├── devicetwin_simplesample
    │   └── devicetwin_simplesample
    ├── remote_monitoring
    │   └── remote_monitoring
    ├── simplesample_amqp
    │   └── simplesample_amqp
    ├── simplesample_http
    │   └── simplesample_http
    ├── simplesample_mqtt
    │   └── simplesample_mqtt
    └── temp_sensor_anomaly
        └── temp_sensor_anomaly
    ```

* You can also reference to the MOXA sample code with ioThinx I/O library **moxa_sample_mqtt.c** in the following directory.

    ```
    user@Linux:~/azure$ tree sample
    sample
    ├── binary
    │   └── moxa_sample_mqtt
    └── source
        └── moxa_sample_mqtt.c
    ```
    * The compiled MOXA program **moxa_sample_mqtt** will be generated after the whole SDK is built.

* Note

    ```
    In general, the setup.sh only needs to be executed once.
    The build.sh needs to be executed after any code change of the SDK.
    ```

## Target (arm-linux)

> Executing the SDK program

### Setup the Environment

1. Setup a network connection to allow target able to access the network.

2. Install following package from package manager.

    ```
    tree
    ```

3. Copy compiled SDK program from host to target.

    ```
    moxa@Moxa:~$ tree
    .
    └── simplesample_amqp
    ```

### Execute the SDK

1. Execute SDK program that cross-compiled by host.

    ```
    moxa@Moxa:~$ sudo ./simplesample_amqp
    ```

2. [View device messages on cloud](#view-device-messages).

## Reference

[1] [https://github.com/Azure/azure-iot-sdk-c][Reference_01]

[2] [https://docs.microsoft.com/en-us/azure/iot-hub][Reference_02]

[comment]: # (Images)
[create_iot_hub_01]: readme/create_iot_hub_01.png
[create_iot_hub_02]: readme/create_iot_hub_02.png
[create_iot_hub_03]: readme/create_iot_hub_03.png
[create_iot_hub_04]: readme/create_iot_hub_04.png
[create_iot_hub_05]: readme/create_iot_hub_05.png
[create_iot_hub_06]: readme/create_iot_hub_06.png

[create_device_01]: readme/create_device_01.png
[create_device_02]: readme/create_device_02.png
[create_device_03]: readme/create_device_03.png
[create_device_04]: readme/create_device_04.png

[copy_device_connection_string_01]: readme/copy_device_connection_string_01.png
[copy_device_connection_string_02]: readme/copy_device_connection_string_02.png
[copy_device_connection_string_03]: readme/copy_device_connection_string_03.png

[view_device_messages_01]: readme/view_device_messages_01.png
[view_device_messages_02]: readme/view_device_messages_02.png

[comment]: # (Links)
[cloud]: https://portal.azure.com
[Reference_01]: https://github.com/Azure/azure-iot-sdk-c
[Reference_02]: https://docs.microsoft.com/en-us/azure/iot-hub
