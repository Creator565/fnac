#include <gint/keyboard.h>
#include <gint/display.h>
#include <gint/display-fx.h>
#include <gint/keycodes.h>
#include <gint/clock.h>
#include <gint/timer.h>
#include <array>
#include <rand.h>

unsigned bounded_rand(unsigned range)
{
    for (unsigned x, r;;)
        if (x = rand(), r = x % range, x - r <= -range)
            return r;
}

enum Cam : uint8_t {
    OFFICE,
    CAM_1A,
    CAM_1B,
    CAM_1C,
    CAM_2A,
    CAM_2B,
    CAM_3,
	CAM_4A,
    CAM_4B,
    CAM_5,
    CAM_6,
    CAM_7,
	CAM_COUNT
};

constexpr std::array<std::array<Cam, 2>, CAM_COUNT> graphBonny = {{
    /* OFFICE */ {CAM_COUNT, CAM_COUNT},
    /* CAM_1A */ {CAM_1B, CAM_5},
    /* CAM_1B */ {CAM_5, CAM_2A},
    /* CAM_1C */ {CAM_COUNT, CAM_COUNT},
    /* CAM_2A */ {CAM_3, CAM_2B},
    /* CAM_2B */ {OFFICE, CAM_3},
    /* CAM_3 */ {CAM_2A, OFFICE},
    /* CAM_4A */ {CAM_COUNT, CAM_COUNT},
    /* CAM_4B */ {CAM_COUNT, CAM_COUNT},
    /* CAM_5 */ {CAM_1B, CAM_2A},
    /* CAM_6 */ {CAM_COUNT, CAM_COUNT},
    /* CAM_7 */ {CAM_COUNT, CAM_COUNT},
}};
Cam posBonny = CAM_1A;
int lvlBonny = 20; // 20 for testing purposes

int tickAnimatronics() {
	int mov = bounded_rand(20) + 1;
	if(mov <= lvlBonny) {

	}
	dprint(1, 1, C_BLACK, "Bonny: %d", posBonny);
	return TIMER_CONTINUE;
}

int tickFreddy() {
	dprint(1, 9, C_BLACK, "TICKED FREDDY");
	dupdate();
	return TIMER_CONTINUE;
}


int main(void)
{
	dclear(C_WHITE);
	dupdate();
	int animatTimer = timer_configure(TIMER_ANY, 5*1000*1000, GINT_CALL(tickAnimatronics));
	int freddyTimer = timer_configure(TIMER_ANY, 3*1000*1000, GINT_CALL(tickFreddy));
	timer_start(animatTimer);
	timer_start(freddyTimer);
	while(1) {
		clearevents();
		if (keydown(KEY_EXIT))
		{
			break;
		}
		dupdate();
	}
	return 1;
}