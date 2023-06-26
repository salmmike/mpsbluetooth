#ifndef _MPSBLUETOOTH_H
#define _MPSBLUETOOTH_H

#include <glib.h>
#include <gio/gio.h>

#define MPS_BT_ADDRESS_MAX_LEN 25
#define MPS_BT_NAME_MAX_LEN 25
#define MPS_BT_UUID_LEN 25
#define MPS_INTERFACE_MAX_LEN 25

/**
 * @brief Struct reprecenting a discovered device.
 */
struct discovery_data
{
    char address[MPS_BT_ADDRESS_MAX_LEN]; /* Bluetooth addres of discovered device. */
    char name[MPS_BT_ADDRESS_MAX_LEN]; /* Name of discovered device. */
};

/**
 * @brief Struct available bluetooth adapter devices.
 */
struct bt_adapter_device
{
    char adapter[MPS_INTERFACE_MAX_LEN]; /* Name of adapter device, e.g. hci0. */
    char address[MPS_BT_ADDRESS_MAX_LEN]; /* Address of adapter device, e.g. 12:34:56:78:9A:BC. */
    char name[MPS_BT_NAME_MAX_LEN]; /* Name of adapter, e.g. abc. */
};

/**
 * @brief Start the device discovery.
 * @return Non-zero value on error.
 * @param device the name of the used bluetooth device, e.g. hci0
 */
int
start_discovery(const char* device);

/**
 * @brief Start the device discovery.
 * @return Non-zero value on error.
 * @param device the name of the used bluetooth device, e.g. hci0
 */
int
stop_discovery(const char* device);

/**
 * @brief List available bluetooth adapter devices. Caller must free the struct if any
 * devices are found.
 * @return Count of found adapters. Negative value on failure.
 * @param adapters list of structs holding the adapter names.
 */
int
list_adapters(struct bt_adapter_device** adapters);

#endif
