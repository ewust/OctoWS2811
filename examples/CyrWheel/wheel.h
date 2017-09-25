
#define SECTION_LEN 176             // number of pixels in one section
#define STRIP_LEN (SECTION_LEN)   // longest strip length. Pretend all are equal (even though some are only 2 sections long)
#define WHEEL_LEN (SECTION_LEN*5)   // number of pixels in one side of the wheel (top or bottom)


int wheel_idx_to_led_num(int idx, bool is_top);
