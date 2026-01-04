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
uint foxyPowerDrain;


uint ticksBasePower;
bool leftDoorClosed;
bool rightDoorClosed;
bool leftLightOn;
bool rightLightOn;
bool dead;
Cam currentCam;
uint64_t gameTicks;
int animatronic_timer;
uint hour;
uint powerUsage;
uint powerLeft;
uint actionWait;


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

    if(gameTicks % (uint)(ticksBasePower/powerUsage) == 0) {
        powerLeft -= 1;
        if(powerLeft == 0) {
            dead = true;
        }
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
            if(leftDoorClosed) { 
                foxyStage = 0;
                powerLeft -= foxyPowerDrain;
                foxyPowerDrain += 5;
             }
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
    dprint(1, 1+8*5, C_BLACK, "Power: %d", powerLeft);
    dprint(1, 1+8*6, C_BLACK, "Power Drain: %d", powerUsage);
    dprint(1, 1+8*7, C_BLACK, "Camera %d", currentCam);
    dupdate();
}

void startCustomNight(uint bonnyLVL, uint chicaLVL, uint freddyLVL, uint foxyLVL, float powerDrainTime) {
    // reset global values
    leftDoorClosed = false;
    rightDoorClosed = false;
    dead = false;
    currentCam = OFFICE;
    gameTicks = 0;
    animatronic_timer = -1;
    hour = 12;
    powerUsage = 1;
    powerLeft = 100;
    leftLightOn = false;
    rightLightOn = false;
    actionWait = 0;

    // reset animatronic values
    posBonny = CAM_1A;
    posChica = CAM_1A;
    posFreddy = CAM_1A;
    freddyWait = 0;
    foxyStage = 0;
    foxyLocked = false;
    foxyLockWait = 0;
    foxyWait = 0;
    foxyPowerDrain = 1;

    // set correct night specific values
    lvlBonny = bonnyLVL;
    lvlChica = chicaLVL;
    lvlFreddy = freddyLVL;
    lvlFoxy = foxyLVL;
    ticksBasePower = (uint)(powerDrainTime/0.01);

    uint seed = rtc_ticks();
	srand(seed);

    int animatronic_timer = timer_configure(TIMER_ANY, (uint64_t)(0.01*1000*1000), GINT_CALL(tickAll));
    timer_start(animatronic_timer);
}

void switchCamera(Cam newCam) {
    if(newCam != OFFICE && currentCam == OFFICE) {powerUsage += 1;}
    if(newCam == OFFICE && currentCam != OFFICE) {powerUsage -= 1;}

    if(newCam != OFFICE) {
        foxyLocked = true;
        foxyLockWait = 0;
        if(foxyStage == 4 && ( newCam == CAM_1C || newCam == CAM_2A )) {
            foxyWait = 0;
        }
    }
    else {
        if(currentCam != OFFICE) {
            foxyLockWait = gameTicks + (uint64_t)((bounded_rand(1667+1-83)+83)/100/0.01);
        }
    }

    // drawing code for the cam goes here

    currentCam = newCam;
    actionWait = 10;
    return;
}

/// @param door 0 is for the left door and 1 is for the right one
void switchDoor(int door) {
    if(door == 0) {
        leftDoorClosed = !leftDoorClosed;
        if(leftDoorClosed) {powerUsage += 1;}
        else {powerUsage -= 1;}
    }
    if(door == 1) {
        rightDoorClosed = !rightDoorClosed;
        if(rightDoorClosed) {powerUsage += 1;}
        else {powerUsage -= 1;}
    }
    actionWait = 90;
}

/// @param light 0 is for the left light and 1 is for the right one
void switchLight(int light) {
    if(light == 0) {
        leftLightOn = !leftLightOn;
        if(leftLightOn) {powerUsage += 1;}
        else {powerUsage -= 1;}
    }
    if(light == 1) {
        rightLightOn = !rightLightOn;
        if(rightLightOn) {powerUsage += 1;}
        else {powerUsage -= 1;}
    }
    actionWait = 90;
}


int main(void)
{
    startCustomNight(0, 0, 0, 0, 9.6);

	while(1) {
		clearevents();
		if(keydown(KEY_EXIT)) { break; }
		
        if(actionWait == 0) {
            if((currentCam == OFFICE)) {
                if(keydown(KEY_PLUS)) { switchDoor(0); }
                if(keydown(KEY_MINUS)) { switchDoor(1); }
                if(keydown(KEY_TIMES)) { switchLight(0); }
                if(keydown(KEY_DIV)) { switchLight(1); }
            }

            // camera switches
            if(keydown(KEY_0)) {switchCamera(OFFICE);}
            if(keydown(KEY_1)) {switchCamera(CAM_1A);}
            if(keydown(KEY_2)) {switchCamera(CAM_2A);}
            if(keydown(KEY_3)) {switchCamera(CAM_3);}
            if(keydown(KEY_4)) {switchCamera(CAM_4A);}
            if(keydown(KEY_5)) {switchCamera(CAM_5);}
            if(keydown(KEY_6)) {switchCamera(CAM_6);}
            if(keydown(KEY_7)) {switchCamera(CAM_7);}

            if(keydown(KEY_XOT)) {
                if(currentCam == CAM_1B || currentCam == CAM_1C) {switchCamera(CAM_1A);}
                if(currentCam == CAM_2B) {switchCamera(CAM_2A);}
                if(currentCam == CAM_4B) {switchCamera(CAM_4A);}
            }
            if(keydown(KEY_LOG)) {
                if(currentCam == CAM_1A || currentCam == CAM_1C) {switchCamera(CAM_1B);}
                if(currentCam == CAM_2A) {switchCamera(CAM_2B);}
                if(currentCam == CAM_4A) {switchCamera(CAM_4B);}
            }
            if(keydown(KEY_LN)) {
                if(currentCam == CAM_1A || currentCam == CAM_1B) {switchCamera(CAM_1C);}
            }
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

        if(actionWait > 0) {
            actionWait -= 1;
        }

		dupdate();
	}

    if(dead || hour == 6) {
        getkey();
    }

	return 1;
}