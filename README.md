<!--
#
# Copyright © 2018 Moxa Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
-->

# AZURE-IOT-SDK-C

> **Description:**
>
> Cloud: Create device instance for connecting by SDK program
>
> Host: Cross-compiling the SDK
>
> Target: Executing the SDK program

## Cloud (Azure)

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

1. In the left navigation pane, choose **All resources**, then search and choose the **[IoT Hub you create before]**.

    ![copy_device_connection_string_01][copy_device_connection_string_01]

2. On the **Explorers** item, choose **IoT devices** > **[Device you create before]**.

    ![copy_device_connection_string_02][copy_device_connection_string_02]

3. Finish coping the **Connection String (primary key)** of Device.

    ![copy_device_connection_string_03][copy_device_connection_string_03]

### View Device Messages

1. On the right side of the search bar above, choose **>_** to open **Cloud Shell**.

    ![view_device_messages_01][view_device_messages_01]

2. Add extension **azure-cli-iot-ext** to Azure CLI.

    ```
    $ az extension add --name azure-cli-iot-ext
    ```

2. Monitor device telemetry and messages that sent to IoT Hub.

    ```
    $ az iot hub monitor-events -n Example-IoT-Hub
    ```
    ![view_device_messages_02][view_device_messages_02]

## Host (x86_64-linux)

### Setup the Environment

1. Setup a network connection to allow host able to access the network.

2. Install GNU cross-toolchain provide by MOXA.

3. Install following package from package manager.

    ```
    cmake git rsync
    ```

### Build the SDK

1. Setup dependencies and SDK to output directory.

    ```
    $ ./setup.sh
    ```
    * For more setup.sh options.

    ```
    $ ./setup.sh --help

    Usage: ./setup.sh [options]

    Options:
        -git                Git repository of SDK.
                            Default: https://github.com/Azure/azure-iot-sdk-c.git

        -ver                Version of SDK.
                            Default: 2018-10-03

        --toolchain         GNU cross-toolchain directory.
                            Default: /usr/local/bin/gcc-linaro-5.1-2015.08-x86_64_arm-linux-gnueabihf

        --help              Display this help and exit.

    Examples:
        Default             ./setup.sh
        Specify             ./setup.sh -git https://github.com/Azure/azure-iot-sdk-c.git -ver 2018-10-03
                            ./setup.sh --toolchain /usr/local/bin/gcc-linaro-5.1-2015.08-x86_64_arm-linux-gnueabihf
    ```

2. Add the **Connection String (primary key)** of Device to SDK sample code such as example **simplesample_amqp.c**. [[Copy Connection String](#copy-device-connection-string)]

    ```
    $ vim output/sdk_azure/serializer/samples/simplesample_amqp/simplesample_amqp.c
    ```
    ```
    /*String containing Hostname, Device Id & Device Key in the format:             */
    /*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"    */
    static const char* connectionString = "HostName=Example-IoT-Hub.azure-devices.net;DeviceId=Example-Device;SharedAccessKey=hQUkHWv6Gq+1r2yQU5vLzQ86KUjyIquow+XuuDG1DqM=";
    ```

3. Build the whole SDK.

    ```
    $ ./build.sh
    ```
    * All compiled SDK program can be found in the following directory, including example **simplesample_amqp**.

    ```
    $ tree output/sdk_azure/cmake/iotsdk_linux/serializer/samples
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

* Note

    ```
    In general, the setup.sh only needs to be executed once.
    The build.sh needs to be executed after any code change of the SDK.
    ```

## Target (arm-linux)

### Setup the Environment

1. Setup a network connection to allow target able to access the network.

2. Copy compiled SDK program from host to target

    ```
    $ tree
    .
    └── simplesample_amqp
    ```

### Execute the SDK

1. Execute SDK program that cross-compiled by host.

    ```
    $ ./simplesample_amqp
    ```
    * You need to install the dependency library for the SDK program if any not found.

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

