#ifndef PRODUCT_H
#define PRODUCT_H

typedef struct product_t {
    uint32_t product_id;
	char expiration_date[8]; // gg/mm/yy
	float price;
} product_t;
#endif
