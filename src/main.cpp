#include <gint/keyboard.h>
#include <gint/display.h>
#include <gint/display-fx.h>
#include <gint/keycodes.h>
#include <gint/clock.h>
#include <gint/timer.h>
#include <gint/rtc.h>
#include <array>
#include <rand.h>
#include <ctime>

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


bool leftDoorClosed = false;
bool rightDoorClosed = false;
bool dead = false;
Cam currentCam = OFFICE;

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

int tickAnimatronics() {
	if(bounded_rand(20) + 1 <= lvlBonny) {move(graphBonny, &posBonny, &leftDoorClosed);}
	if(bounded_rand(20) + 1 <= lvlChica) {move(graphChica, &posChica, &rightDoorClosed);}
	
	return TIMER_CONTINUE;
}

int tickFreddy() {
	if(bounded_rand(20) + 1 <= lvlFreddy && currentCam != posFreddy) {move(graphFreddy, &posFreddy, &rightDoorClosed);}

	return TIMER_CONTINUE;
}

int animatTimer = timer_configure(TIMER_ANY, 5*1000*1000, GINT_CALL(tickAnimatronics));
int freddyTimer = timer_configure(TIMER_ANY, (uint64_t)(3.02*1000*1000), GINT_CALL(tickFreddy));

int main(void)
{
	int seed = rtc_ticks();
	srand(seed);
	dclear(C_WHITE);
	dprint(1, 1, C_BLACK, "Bonny: %d", posBonny);
	dprint(1, 9, C_BLACK, "Chica: %d", posChica);
	dprint(1, 17, C_BLACK, "Freddy: %d", posFreddy);
	dupdate();
	timer_start(animatTimer);
	timer_start(freddyTimer);
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

		if(dead) {
			dclear(C_WHITE);
			dprint(1, 1, C_BLACK, "DEAD");
			timer_stop(animatTimer);
			timer_stop(freddyTimer);
		}
		
		dupdate();
	}
	return 0;
}