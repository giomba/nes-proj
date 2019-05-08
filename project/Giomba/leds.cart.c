#include "leds.cart.h"

void led_blink(void* ptr) {
    printf("[I] led blink\n");
    if (! (leds_get() & LEDS_GREEN)) {
        leds_on(LEDS_GREEN);
        ctimer_set(&led_timer, CLOCK_SECOND, led_blink, NULL);
    } else {
        leds_off(LEDS_GREEN);
    }
    return;
}


void led_heartbeat(void* ptr) {
    printf("[I] led heartbeat\n");

/*
    static uint8_t times = 4;

    if (times == 4) leds_off(LEDS_GREEN);
    if (--times != 0) {
        leds_toggle(LEDS_GREEN);
        ctimer_set(&led_timer, 20, led_heartbeat, NULL);
    } else {
        times = 4;
    }
*/
}
