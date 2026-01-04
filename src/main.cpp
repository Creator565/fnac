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
const uint ticksBonny = (uint)(4.97/0.01);
Cam posBonny;
uint lvlBonny;

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
const uint ticksChica = (uint)(4.98/0.01);
Cam posChica;
uint lvlChica;

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
const uint ticksFreddy = (uint)(3.02/0.01);
Cam posFreddy;
uint lvlFreddy;
uint64_t freddyWait;

const uint ticksFoxy = (uint)(5.01/0.01);
uint foxyStage;
bool foxyLocked;
uint64_t foxyLockWait;
uint lvlFoxy;
uint64_t foxyWait;


bool leftDoorClosed;
bool rightDoorClosed;
bool dead;
Cam currentCam;
uint64_t gameTicks;
int animatronic_timer;
uint hour;


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

    if(currentCam != OFFICE) {
        foxyLocked = true;
        foxyLockWait = 0;
    }
    if(currentCam == OFFICE && foxyLockWait == 0 && foxyLocked) {
        foxyLockWait = gameTicks + (uint64_t)((bounded_rand(1667+1-83)+83)/100/0.01);
    }

    if(gameTicks % ticksBonny == 0) {
        if(bounded_rand(20) + 1 <= lvlBonny) {move(graphBonny, &posBonny, &leftDoorClosed);}
    }
    if(gameTicks % ticksChica == 0) {
        if(bounded_rand(20) + 1 <= lvlChica) {move(graphChica, &posChica, &rightDoorClosed);}
    }
    if(gameTicks % ticksFreddy == 0) {
        if(bounded_rand(20) + 1 <= lvlFreddy && currentCam != posFreddy && freddyWait == 0) {
            uint64_t wait = (uint64_t)((1000 - 100 * std::min((uint)10, lvlFreddy)) / 60 / 0.01); // number of frames converted to gameticks
            freddyWait = gameTicks + wait;
        }
    }
    if(gameTicks % ticksFoxy == 0) {
        if(bounded_rand(20) + 1 <= lvlFoxy && !foxyLocked && foxyStage <= 3) {
            foxyStage += 1;
        }
        if(foxyStage == 4 && foxyWait == 0) {
            foxyWait = gameTicks + (uint64_t)(25/0.01);
        }
    }

    if(gameTicks == freddyWait) {
        move(graphFreddy, &posFreddy, &rightDoorClosed);
        freddyWait = 0;
    }
    if(gameTicks == foxyWait || (foxyStage > 3 && foxyWait == 0)) {
        // executes when foxy is dashing
        foxyWait = 0;
        foxyStage += 1;
        if(foxyStage == 5 || foxyStage == 6) {
            // play foxy rdashing animation
            foxyWait = gameTicks + (uint64_t)(1.5/0.01);
        }
        if(foxyStage == 7) {
            if(leftDoorClosed) { foxyStage = 0; }
            else { dead = true; }
        }
    }
    if(gameTicks == foxyLockWait) {
        foxyLockWait = 0;
        foxyLocked = false;
    }

    if(gameTicks == (1*60+30)/0.01) { // 1AM
        hour = 1;
    }
    if(gameTicks == (1*60+30+(1*60+29)*1)/0.01) { // 2AM
        hour = 2;
    }
    if(gameTicks == (1*60+30+(1*60+29)*2)/0.01) { // 3AM
        hour = 3;
    }
    if(gameTicks == (1*60+30+(1*60+29)*3)/0.01) { // 4AM
        hour = 4;
    }
    if(gameTicks == (1*60+30+(1*60+29)*4)/0.01) { // 5AM
        hour = 5;
    }
    if(gameTicks == (1*60+30+(1*60+29)*5)/0.01) { // 6AM
        hour = 6;
    }

    if(dead || hour == 6) {
        return TIMER_STOP;
    }
    return TIMER_CONTINUE;
}

void printData() {
    dclear(C_WHITE);
	dprint(1, 1, C_BLACK, "Bonny: %d", posBonny);
	dprint(1, 1+8, C_BLACK, "Chica: %d", posChica);
	dprint(1, 1+8*2, C_BLACK, "Freddy: %d", posFreddy);
    dprint(1, 1+8*3, C_BLACK, "Foxy stage: %d", foxyStage);
    dprint(1, 1+8*4, C_BLACK, "Hour: %dAM", hour);
    dupdate();
}

void startCustomNight(uint bonnyLVL, uint chicaLVL, uint freddyLVL, uint foxyLVL) {
    // reset global values
    leftDoorClosed = false;
    rightDoorClosed = false;
    dead = false;
    currentCam = OFFICE;
    gameTicks = 0;
    animatronic_timer = -1;
    hour = 12;

    // reset animatronic values
    posBonny = CAM_1A;
    posChica = CAM_1A;
    posFreddy = CAM_1A;
    freddyWait = 0;
    foxyStage = 0;
    foxyLocked = false;
    foxyLockWait = 0;
    foxyWait = 0;

    // set correct ai levels
    lvlBonny = bonnyLVL;
    lvlChica = chicaLVL;
    lvlFreddy = freddyLVL;
    lvlFoxy = foxyLVL;

    uint seed = rtc_ticks();
	srand(seed);

    int animatronic_timer = timer_configure(TIMER_ANY, (uint64_t)(0.01*1000*1000), GINT_CALL(tickAll));
    timer_start(animatronic_timer);
}

int main(void)
{
    startCustomNight(0, 0, 0, 0);

	while(1) {
		clearevents();
		if (keydown(KEY_EXIT))
		{
			break;
		}
		
        printData();

        // death logic instead of a function call
		if(dead) {
			dclear(C_WHITE);
			dprint(1, 1, C_BLACK, "DEAD");
            dupdate();
            timer_stop(animatronic_timer);
            break;
		}

        if(hour == 6) {
			dclear(C_WHITE);
			dprint(1, 1, C_BLACK, "WIN");
            dupdate();
            timer_stop(animatronic_timer);
            break;
		}

		dupdate();
	}

    if(dead || hour == 6) {
        getkey();
    }

	return 1;
}