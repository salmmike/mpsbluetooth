#include <gtest/gtest.h>
#include <cassert>

extern "C" {
    #include "mpsbluetooth.h"
}

#define NAME bluetooth_unittest

TEST(NAME, test_start_stop_discovery)
{
    ASSERT_EQ(start_discovery("hci0"), 0);
    ASSERT_EQ(stop_discovery("hci0"), 0);
}

TEST(NAME, test_list_adapters)
{
    struct bt_adapter_device* adapters;
    int count = list_adapters(&adapters);
    ASSERT_GT(count, 0) << "At least one bluetooth device needed for test to pass";
    for (int i = 0; i < count; ++i) {
        ASSERT_TRUE(strlen(adapters[i].adapter) > 1)
                    << "Adapter name fault:" << adapters[i].adapter;
        ASSERT_EQ(strlen(adapters[i].address), 17)
                  << "Address" << adapters[i].address << "wrong length";
        ASSERT_TRUE(strlen(adapters[i].address) > 0)
                    << "Name" << adapters[i].name << "not found";
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
