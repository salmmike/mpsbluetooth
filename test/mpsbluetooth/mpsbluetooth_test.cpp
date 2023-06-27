#include <gtest/gtest.h>
#include <cassert>
#include <cstdlib>

extern "C" {
    #include "mpsbluetooth.h"
}

#define NAME bluetooth_unittest

TEST(NAME, test_start_stop_discovery)
{
    GDBusConnection* connection = get_connection();
    ASSERT_EQ(start_discovery("hci0", connection), 0);
    ASSERT_EQ(is_discovering("hci0", connection), 1) << "Discovey didn't turn on.";

    ASSERT_EQ(stop_discovery("hci0", connection), 0);
    ASSERT_EQ(is_discovering("hci0", connection), 0) << "Discovery didn't turn off.";
    g_object_unref(connection);
}

TEST(NAME, test_is_discovering)
{
    GDBusConnection* connection = get_connection();
    int discovering = is_discovering("hci0", connection);
    printf("Is discovering: %d\n", discovering);
    ASSERT_TRUE(discovering == 0 || discovering == 1) << "Value: " << discovering;
    g_object_unref(connection);
}

TEST(NAME, test_list_adapters)
{
    GDBusConnection* connection = get_connection();
    struct bt_adapter_device* adapters;
    int count = list_adapters(&adapters, connection);
    ASSERT_GT(count, 0) << "At least one bluetooth device needed for test to pass";
    for (int i = 0; i < count; ++i) {
        printf("Adapter: %s, Name: %s, Address: %s, Discovering %d\n",
        adapters[i].adapter, adapters[i].name, adapters[i].address, adapters[i].discovering);

        ASSERT_TRUE(strlen(adapters[i].adapter) > 1)
                    << "Adapter name fault:" << adapters[i].adapter;

        ASSERT_EQ(strlen(adapters[i].address), 17)
                  << "Address " << adapters[i].address << " wrong length";

        ASSERT_TRUE(strlen(adapters[i].address) > 0)
                    << "Name" << adapters[i].name << "not found";
    }
    g_object_unref(connection);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
