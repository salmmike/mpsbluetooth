#include <stdio.h>
#include <mpsbluetooth.h>

int main()
{
    int keep = 1;
    GDBusConnection* connection = get_connection();
    while(keep) {
        printf("Check == 1, Start == 2, stop == 3\n");
        scanf("%d", &keep);

        if (keep == 1) {
            printf("hci0: %d\n", is_discovering("hci0", connection));
            printf("hci1: %d\n", is_discovering("hci1", connection));
        }
        if (keep == 2) {
            start_discovery("hci1", connection);
        }

        if (keep == 3) {
            stop_discovery("hci1", connection);
        }

    }
}