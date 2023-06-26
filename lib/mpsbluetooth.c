#include "mpsbluetooth.h"
#include "stdio.h"

#define MAX_BT_ADAPTER_COUNT 10
#define BLUEZ_INTERFACE "org.bluez"
#define OBJECT_PATH_ROOT "/org/bluez"

static GVariant*
proxy_call_sync(const char* name,
                const char* object_path,
                const char* interface_name,
                const char* method_name,
                GError** error)
{
    GDBusProxy *proxy;
    GDBusConnection *connection;
    GVariant *result;

    *error = NULL;
    connection = NULL;
    proxy = NULL;

    connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM,
                                NULL,
                                error);

    if (*error != NULL) {
        goto exit;
    }
    *error = NULL;

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
    g_object_unref(proxy);
    g_object_unref(connection);
    return result;
}

int
start_discovery(const char* device)
{
    printf("Start discovery\n");
    return 0;
}

int
stop_discovery(const char* device)
{
    printf("Stop discovery\n");
    return 0;
}

int list_adapters(struct bt_adapter_device** adapters)
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
    GVariant *value_variant;
    GVariantType *variant_type;

    size_t count = 0;

    *adapters = calloc(MAX_BT_ADAPTER_COUNT, sizeof(struct bt_adapter_device));

    result = proxy_call_sync(BLUEZ_INTERFACE, "/",
                             "org.freedesktop.DBus.ObjectManager",
                             "GetManagedObjects", &error);

    g_variant_get(result, "(a{oa{sa{sv}}})", &object_iter);

    while (g_variant_iter_next(object_iter, "{oa{sa{sv}}}", &object_path, &property_iter)) {
        const char* hcidev = strstr(object_path, "hci");
        if (hcidev != NULL) {
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
                        g_variant_unref(value_variant);
                        g_free(value_name);
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
