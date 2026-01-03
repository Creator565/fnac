#include <gint/keyboard.h>
#include <gint/display.h>
#include <gint/display-fx.h>
#include <gint/keycodes.h>
#include <gint/clock.h>
#include <gint/timer.h>
#include <gint/rtc.h>
#include <gint/gint.h>
#include <array>
#include <rand.h>
#include <cstdint>

/// @brief Mostly unbiased random number generation withing range
/// @param range maximum value + 1
/// @return Random int between 0 and range-1
unsigned bounded_rand(unsigned range)
{
    for (unsigned x, r;;)
        if (x = rand(), r = x % range, x - r <= -range)
            return r;
}

enum Cam : uint8_t {
    OFFICE,
	LEFT_DOOR,
	RIGHT_DOOR,
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

// expects the 0th element of OFFICE to be the location they go back to if the office door is closed
constexpr std::array<std::array<Cam, 2>, CAM_COUNT> graphBonny = {{
    /* OFFICE */ {CAM_1B, CAM_COUNT},
    /* LEFT_DOOR */ {OFFICE, OFFICE},
    /* RIGHT_DOOR */ {OFFICE, OFFICE},
    /* CAM_1A */ {CAM_1B, CAM_5},
    /* CAM_1B */ {CAM_5, CAM_2A},
    /* CAM_1C */ {CAM_COUNT, CAM_COUNT},
    /* CAM_2A */ {CAM_3, CAM_2B},
    /* CAM_2B */ {LEFT_DOOR, CAM_3},
    /* CAM_3 */  {CAM_2A, LEFT_DOOR},
    /* CAM_4A */ {CAM_COUNT, CAM_COUNT},
    /* CAM_4B */ {CAM_COUNT, CAM_COUNT},
    /* CAM_5 */  {CAM_1B, CAM_2A},
    /* CAM_6 */  {CAM_COUNT, CAM_COUNT},
    /* CAM_7 */  {CAM_COUNT, CAM_COUNT}
}};
Cam posBonny = CAM_1A;
uint lvlBonny = 20; // 20 for testing purposes
uint ticksBonny = (uint)(4.97/0.005);

constexpr std::array<std::array<Cam, 2>, CAM_COUNT> graphChica = {{
    /* OFFICE */ {CAM_4A, CAM_COUNT},
    /* LEFT_DOOR */ {OFFICE, OFFICE},
    /* RIGHT_DOOR */ {OFFICE, OFFICE},
    /* CAM_1A */ {CAM_1B, CAM_1B}, // copy as it is the only option
    /* CAM_1B */ {CAM_7, CAM_6},
    /* CAM_1C */ {CAM_COUNT, CAM_COUNT},
    /* CAM_2A */ {CAM_COUNT, CAM_COUNT},
    /* CAM_2B */ {CAM_COUNT, CAM_COUNT},
    /* CAM_3 */  {CAM_COUNT, CAM_COUNT},
    /* CAM_4A */ {CAM_4B, CAM_4B}, // copy as it is the only option
    /* CAM_4B */ {CAM_4A, RIGHT_DOOR},
    /* CAM_5 */  {CAM_COUNT, CAM_COUNT},
    /* CAM_6 */  {CAM_7, CAM_4A},
    /* CAM_7 */  {CAM_6, CAM_4A}
}};
Cam posChica = CAM_1A;
uint lvlChica = 20; // 20 for testing purposes
uint ticksChica = (uint)(4.98/0.005);

constexpr std::array<std::array<Cam, 2>, CAM_COUNT> graphFreddy = {{
    /* OFFICE */ {CAM_4A, CAM_COUNT},
    /* LEFT_DOOR */ {OFFICE, OFFICE},
    /* RIGHT_DOOR */ {OFFICE, OFFICE},
    /* CAM_1A */ {CAM_1B, CAM_1B},
    /* CAM_1B */ {CAM_7, CAM_7},
    /* CAM_1C */ {CAM_COUNT, CAM_COUNT},
    /* CAM_2A */ {CAM_COUNT, CAM_COUNT},
    /* CAM_2B */ {CAM_COUNT, CAM_COUNT},
    /* CAM_3 */  {CAM_COUNT, CAM_COUNT},
    /* CAM_4A */ {CAM_4B, CAM_4B},
    /* CAM_4B */ {OFFICE, OFFICE},
    /* CAM_5 */  {CAM_COUNT, CAM_COUNT},
    /* CAM_6 */  {CAM_4A, CAM_4A},
    /* CAM_7 */  {CAM_6, CAM_6}
}};
Cam posFreddy = CAM_1A;
uint lvlFreddy = 20; // 20 for testing purposes
uint ticksFreddy = (uint)(3.02/0.005);


bool leftDoorClosed = false;
bool rightDoorClosed = false;
bool dead = false;
Cam currentCam = OFFICE;
uint64_t gameTicks = 0;
uint64_t freddyWait = 0;


void move(const std::array<std::array<Cam, 2>, CAM_COUNT> graph, Cam* currentPos, bool* door) {
	int chosen = bounded_rand(2);
	Cam posNext = graph[*currentPos][chosen];
	if(posNext == OFFICE) {
		if(*door) {
			*currentPos = graph[OFFICE][0];
		}
		else {
			dead = true;
		}
	}
	else {
		*currentPos = posNext;
	}
	return;
}

int tickAll() {
    gameTicks += 1;

    if(gameTicks % ticksBonny == 0) {
        if(bounded_rand(20) + 1 <= lvlBonny) {move(graphBonny, &posBonny, &leftDoorClosed);}
    }
    if(gameTicks % ticksChica == 0) {
        if(bounded_rand(20) + 1 <= lvlChica) {move(graphChica, &posChica, &rightDoorClosed);}
    }
    if(gameTicks % ticksFreddy == 0) {
        if(bounded_rand(20) + 1 <= lvlFreddy && currentCam != posFreddy && freddyWait == 0) {
            uint64_t wait = (uint64_t)((1000 - 100 * std::min((uint)10, lvlFreddy)) / 60 / 0.005); // number of frames converted to gameticks
            freddyWait = gameTicks + wait;
        }
    }
    if(gameTicks == freddyWait) {
        move(graphFreddy, &posFreddy, &rightDoorClosed);
        freddyWait = 0;
    }

    return TIMER_CONTINUE;
}

int main(void)
{
	uint seed = rtc_ticks();
	srand(seed);
	dclear(C_WHITE);
	dprint(1, 1, C_BLACK, "Bonny: %d", posBonny);
	dprint(1, 9, C_BLACK, "Chica: %d", posChica);
	dprint(1, 17, C_BLACK, "Freddy: %d", posFreddy);
	dupdate();

    int animatronic_timer = timer_configure(TIMER_ANY, (uint64_t)(0.005*1000*1000), GINT_CALL(tickAll)); // 0.005s interval to mantain 0.01s resolution
    timer_start(animatronic_timer);

	while(1) {
		clearevents();
		if (keydown(KEY_EXIT))
		{
			break;
		}
		
		dclear(C_WHITE);
		dprint(1, 1, C_BLACK, "Bonny: %d", posBonny);
		dprint(1, 9, C_BLACK, "Chica: %d", posChica);
		dprint(1, 17, C_BLACK, "Freddy: %d", posFreddy);
        dprint(1, 17+8, C_BLACK, "gt: %llu", (unsigned long long)gameTicks);
        dprint(1, 17+8*2, C_BLACK, "fw: %llu", (unsigned long long)freddyWait);

		if(dead) {
			dclear(C_WHITE);
			dprint(1, 1, C_BLACK, "DEAD");
            dupdate();
			timer_stop(animatronic_timer);
            break;
		}

		dupdate();
	}

    if(dead) {
        getkey();
    }

	return 0;
}