#include "batmon.h"

void batmon(void) {

    /* read battery monitor sensor -- hey where's the battery? */
    /* actually this is a simulation, isn't it? =) */

    switch (status) {
        /* charge */
        case NOT_ASSOCIATED:
        case ASSOCIATED:
            battery_charge += 5;
            if (battery_charge > 100) battery_charge = 100;
            led_heartbeat(NULL);
        break;

        /* discharge */
        default:
            battery_charge -= 1;
            if (battery_charge < 0) battery_charge = 0;
        break;
    }

    /* turn on red led if battery charge is low */
    if (battery_charge < 48) {
        leds_on(LEDS_RED);
    } else {
        leds_off(LEDS_RED);
    }

    etimer_reset(&battery_timer);

    printf("[I] updated battery status: %d %%\n", battery_charge);
}
