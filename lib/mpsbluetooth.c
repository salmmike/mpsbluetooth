#include "mpsbluetooth.h"
#include "stdio.h"

#define MAX_BT_ADAPTER_COUNT 10
#define BLUEZ_INTERFACE "org.bluez"
#define OBJECT_PATH_ROOT "/org/bluez"

GDBusConnection*
get_connection()
{
    GDBusConnection *connection = NULL;
    GError* error = NULL;

    connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM,
                                NULL,
                                &error);
    if (error != NULL) {
        g_print("Error creating a connection: %s\n", error->message);
        g_free(error);
        return NULL;
    }
    return connection;
}

static GVariant*
proxy_call_sync(GDBusConnection *connection,
                const char* name,
                const char* object_path,
                const char* interface_name,
                const char* method_name,
                GError** error)
{
    GDBusProxy *proxy;
    GVariant *result;

    *error = NULL;
    proxy = NULL;

    proxy = g_dbus_proxy_new_sync(connection,
                                  G_DBUS_PROXY_FLAGS_NONE,
                                  NULL,
                                  name,
                                  object_path,
                                  interface_name,
                                  NULL,
                                  error);
    if (*error != NULL) {
        goto exit;
    }

    result = g_dbus_proxy_call_sync(proxy,
                                    method_name,
                                    NULL,
                                    G_DBUS_CALL_FLAGS_NONE,
                                    -1,
                                    NULL,
                                    error);

exit:
    if (*error != NULL) {
        g_print("Error in proxy call: %s\n", (*error)->message);
        g_free(*error);
    }
    g_object_unref(proxy);
    return result;
}

static GVariant*
get_property(GDBusConnection *connection,
             const char* name,
             const char* object_path,
             const char* interface_name,
             GError** error)
{
    GVariant* result;
    result = proxy_call_sync(connection, name, object_path,
                             "org.freedesktop.DBus.Properties",
                             "Get", error);
    return result;
}

int
is_discovering(const char* device, GDBusConnection *connection)
{
    int result = -1;
    struct bt_adapter_device* adapters;
    int count = list_adapters(&adapters, connection);

    for (int i = 0; i < count; ++i) {
        if (strcmp(adapters[i].adapter, device) == 0) {
            result = adapters[i].discovering;
        }
    }
    free(adapters);
    return result;
}

int
start_discovery(const char* device, GDBusConnection* connection)
{
    GError *error = NULL;
    int result = 0;
    int discovering = is_discovering(device, connection);
    printf("Start is discovering %d\n", discovering);
    if (discovering == 1) {
        return 0;
    }

    char object_path[255] = {0};
    sprintf(object_path, "/org/bluez/%s", device);
    g_print("Try to start %s\n", object_path);

    proxy_call_sync(connection, BLUEZ_INTERFACE, object_path,
                    "org.bluez.Adapter1", "StartDiscovery",
                    &error);

    if (error != NULL) {
        g_printerr("Start discovery error: %s\n", error->message);
        result = error->code;
        g_free(error);
    }
    return result;
}

int
stop_discovery(const char* device, GDBusConnection* connection)
{
    GError *error;
    error = NULL;
    int discovering = is_discovering(device, connection);
    printf("Stop is discovering %d\n", discovering);

    if (discovering == 0) {
        return 0;
    }

    if (discovering != 1) {
        return 1;
    }

    char object_path[255];
    sprintf(object_path, "/org/bluez/%s", device);
    g_print("Try to stop %s\n", object_path);
    proxy_call_sync(connection, BLUEZ_INTERFACE, object_path,
                    "org.bluez.Adapter1", "StopDiscovery",
                    &error);

    if (error != NULL) {
        g_printerr("Stop discovery error: %s\n", error->message);
        return 1;
    }
    return 0;
}

int list_adapters(struct bt_adapter_device** adapters, GDBusConnection *connection)
{
    GVariant *result;
    GError* error;

    GVariantIter *object_iter;
    GVariantIter *property_iter;
    GVariantIter *value_iter;
    const gchar *object_path;
    const gchar *property_name;
    const gchar *value_name;
    const gchar *value;
    int int_value;
    const GVariantType *variant_type;
    GVariant *value_variant;

    size_t count = 0;

    *adapters = calloc(MAX_BT_ADAPTER_COUNT, sizeof(struct bt_adapter_device));

    result = proxy_call_sync(connection, BLUEZ_INTERFACE, "/",
                             "org.freedesktop.DBus.ObjectManager",
                             "GetManagedObjects", &error);

    g_variant_get(result, "(a{oa{sa{sv}}})", &object_iter);

    while (g_variant_iter_next(object_iter, "{oa{sa{sv}}}", &object_path, &property_iter)) {
        char* hcidev = strstr(object_path, "hci");

        if (hcidev != NULL && strstr(hcidev, "/") == NULL) {
            strncpy((*adapters)[count].adapter, hcidev, MPS_INTERFACE_MAX_LEN - 1);

            while (g_variant_iter_next(property_iter, "{sa{sv}}", &property_name, &value_iter)) {
                if (strstr(property_name, "Adapter1") != NULL){

                    while (g_variant_iter_next(value_iter, "{sv}", &value_name, &value_variant)) {
                        variant_type = g_variant_get_type(value_variant);
                        if (strcmp(value_name, "Address") == 0) {
                            if (g_variant_type_equal(variant_type, G_VARIANT_TYPE_STRING)) {
                                g_variant_get(value_variant, "s", &value);
                                strncpy((*adapters)[count].address, value, MPS_BT_ADDRESS_MAX_LEN - 1);
                            }
                        }
                        if (strcmp(value_name, "Name") == 0) {
                            if (g_variant_type_equal(variant_type, G_VARIANT_TYPE_STRING)) {
                                g_variant_get(value_variant, "s", &value);
                                strncpy((*adapters)[count].name, value, MPS_BT_NAME_MAX_LEN - 1);
                            }
                        }
                        if (strcmp(value_name, "Discovering") == 0) {
                            if (g_variant_type_equal(variant_type, G_VARIANT_TYPE_BOOLEAN)) {
                                g_variant_get(value_variant, "b", &int_value);
                                (*adapters)[count].discovering = int_value;
                            }
                        }
                        g_variant_unref(value_variant);
                    }
                }
                g_variant_iter_free(value_iter);
            }

            g_variant_iter_free(property_iter);
            ++count;
        }
    }

    *adapters = realloc(*adapters, count * sizeof(struct bt_adapter_device));

    g_variant_iter_free(object_iter);
    g_variant_unref(result);
    return count;
}
