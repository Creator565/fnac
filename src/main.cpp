#include <gint/keyboard.h>
#include <gint/display.h>
#include <gint/keycodes.h>
#include <gint/timer.h>
#include <gint/clock.h>
#include <gint/rtc.h>
#include <array>
#include <rand.h>
#include <cstdint>
#include "img.h"


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

char* camToStr(Cam cam) {
    switch (cam) 
    {
        case OFFICE: return "OFFICE";
	    case LEFT_DOOR: return "Left Door";
	    case RIGHT_DOOR: return "Right Door";
        case CAM_1A: return "1A";
        case CAM_1B: return "1B";
        case CAM_1C: return "1C";
        case CAM_2A: return "2A";
        case CAM_2B: return "2B";
        case CAM_3: return "3";
	    case CAM_4A: return "4A";
        case CAM_4B: return "4B";
        case CAM_5: return "5";
        case CAM_6: return "6";
        case CAM_7: return "7";
	    case CAM_COUNT: return "YOUR NOT SUPPOSED TO BE HERE";
    }
}

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
bool invuln;
bool infBatt;
bool infoVision;


/// @brief invoke the main menu
/// @return returns 1-6 for starting a night, 0 for starting a custom night and -1 for quitting the game
int mainMenu() {
    int selected = 0;
    while(true) {
        dimage(0, 0, &imgMenuBase);
        dtext(8, 1, C_WHITE, "Five");
        dtext(8, 1+8*1, C_WHITE, "Nights");
        dtext(8, 1+8*2, C_WHITE, "At");
        dtext(8, 1+8*3, C_WHITE, "Freddy's");

        dtext(8, 1+8*4+6, C_WHITE, "Start");
        dtext(8, 1+8*5+6, C_WHITE, "Extras");
        dtext(8, 1+8*6+6, C_WHITE, "Quit");

        drect(8, 1+8*(4+selected)+6, 8+36, 1+8*(5+selected)+6, C_INVERT);
        dupdate();
        key_event_t key = getkey();
        if(key.key == KEY_UP && selected > 0) {
            selected -= 1;
        }
        else if(key.key == KEY_DOWN && selected < 2) {
            selected += 1;
        }
        else if(key.key == KEY_EXE) {
            switch (selected)
            {
                case 0: { // night selection screen
                    int nightSelected = 0;
                    while(true) {
                        dimage(0, 0, &imgMenuBase);
                        dtext(8, 2+8*0, C_WHITE, "Night 1");
                        dtext(8, 2+8*1, C_WHITE, "Night 2");
                        dtext(8, 2+8*2, C_WHITE, "Night 3");
                        dtext(8, 2+8*3, C_WHITE, "Night 4");
                        dtext(8, 2+8*4, C_WHITE, "Night 5");
                        dtext(8, 2+8*5, C_WHITE, "Night 6");
                        dtext(8, 2+8*6, C_WHITE, "Night 7");

                        drect(8, 2+8*(nightSelected), 8+42, 2+8*(nightSelected+1), C_INVERT);
                        dupdate();
                        key_event_t key = getkey();
                        if(key.key == KEY_UP && nightSelected > 0) {
                            nightSelected -= 1;
                        }
                        else if(key.key == KEY_DOWN && nightSelected < 6) {
                            nightSelected += 1;
                        }
                        else if(key.key == KEY_EXIT || key.key == KEY_MENU) {
                            break;
                        }
                        else if(key.key == KEY_EXE) {
                            return (nightSelected+1) % 7; // return 1-6 for normail night and 0 for custom night
                        }
                    }
                    break;
                }

                case 1: { // extras menu
                    int optionSelected = 0;
                    while(true) {
                        dimage(0, 0, &imgMenuBase);
                        dprint(6, 4+9*0, C_WHITE, "Invuln %s", invuln ? "#" : "");
                        dprint(6, 4+9*1, C_WHITE, "InfBatt %s", infBatt ? "#" : "");
                        dprint(6, 4+9*2, C_WHITE, "InfoVis %s", infoVision ? "#" : "");

                        drect(6, 4+9*(optionSelected), 6+59, 4+9*(optionSelected+1), C_INVERT);
                        dupdate();
                        key_event_t key = getkey();
                        if(key.key == KEY_UP && optionSelected > 0) {
                            optionSelected -= 1;
                        }
                        else if(key.key == KEY_DOWN && optionSelected < 2) {
                            optionSelected += 1;
                        }
                        else if(key.key == KEY_EXIT || key.key == KEY_MENU) {
                            break;
                        }
                        else if(key.key == KEY_EXE) {
                            switch (optionSelected)
                            {
                                case 0: invuln = !invuln; break;
                                case 1: infBatt = !infBatt; break;
                                case 2: infoVision = !infoVision; break;
                            }
                        }
                    }
                    break;
                }
            
                case 2: { // quit game
                    return -1;
                }
            }
        }
        else if(key.key == KEY_EXIT  || key.key == KEY_MENU) {
            return -1;
        }
    }


    return 0;
}

void drawCam(Cam cam) {
    if(infoVision) {
        return;
    }
    switch (cam)
    {
        case OFFICE: {
            dimage(0, 0, &imgOfficeBase);
            if(leftLightOn) { dimage(0, 0, &imgOfficeLeftLight); }
            if(rightLightOn) { dimage(0, 0, &imgOfficeRightLight); }
            if(posBonny == LEFT_DOOR && leftLightOn) { dimage(0, 0, &imgOfficeBonny); }
            if(posChica == RIGHT_DOOR && rightLightOn) { dimage(0, 0, &imgOfficeChica); }
            if(leftDoorClosed) { dimage(0, 0, &imgOfficeLeftDoor); }
            if(rightDoorClosed) { dimage(0, 0, &imgOfficeRightDoor); }
            dprint_opt(105, 1, C_WHITE, C_NONE, DTEXT_RIGHT, DTEXT_TOP, "%dAM", hour);
            dprint_opt(19, 1, C_WHITE, C_NONE, DTEXT_LEFT, DTEXT_TOP, "%d%", powerLeft);
            char hashes[powerUsage + 1]; // +1 for null terminator
            for (uint i = 0; i < powerUsage; i++) {
                hashes[i] = '#';
            }
            hashes[powerUsage] = '\0'; // null-terminate
            dprint_opt(37, 1, C_WHITE, C_NONE, DTEXT_LEFT, DTEXT_TOP, "%s", hashes);
            break;
        }
        
        case CAM_1A: {
            dimage(0, 0, &img1ABase);
            if(posChica == CAM_1A) { dimage(0, 0, &img1AChica); }
            if(posBonny == CAM_1A) { dimage(0, 0, &img1ABonny); }
            if(posFreddy == CAM_1A) { dimage(0, 0, &img1AFreddy); }
            break;
        }

        case CAM_1B: {
            dimage(0, 0, &img1BBase);
            if(posBonny == CAM_1B) { dimage(0, 0, &img1BBonny); }
            if(posChica == CAM_1B) { dimage(0, 0, &img1BChica); }
            if(posFreddy == CAM_1B) { dimage(0, 0, &img1BFreddy); }
            break;
        }

        case CAM_1C: {
            if(foxyStage == 0) { dimage(0, 0, &img1C0); }
            if(foxyStage == 1) { dimage(0, 0, &img1C1); }
            if(foxyStage == 2) { dimage(0, 0, &img1C2); }
            if(foxyStage == 3) { dimage(0, 0, &img1C3); }
            break;
        }

        case CAM_2A: {
            dimage(0, 0, &img2ABase);
            if(posBonny == CAM_2A) { dimage(0, 0, &img2ABonny); }
            if(foxyStage == 4) { dimage(0, 0, &img2AFoxy4); }
            if(foxyStage == 5) { dimage(0, 0, &img2AFoxy5); }
            break;
        }

        case CAM_2B: {
            dimage(0, 0, &img2BBase);
            if(posBonny == CAM_2B) { dimage(0, 0, &img2BBonny); }
            break;
        }

        case CAM_3: {
            dimage(0, 0, &img3Base);
            if(posBonny == CAM_3) { dimage(0, 0, &img3Bonny); }
            dimage(0, 0, &img3Light);
            break;
        }

        case CAM_4A: {
            dimage(0, 0, &img4ABase);
            if(posChica == CAM_4A) { dimage(0, 0, &img4AChica); }
            if(posFreddy == CAM_4A) { dimage(0, 0, &img4AFreddy); }
            dimage(0, 0, &img4ACables);
            break;
        }

        case CAM_4B: {
            dimage(0, 0, &img4BBase);
            if(posChica == CAM_4B) { dimage(0, 0, &img4BChica); }
            if(posFreddy == CAM_4B) { dimage(0, 0, &img4BFreddy); }
            break;
        }

        case CAM_5: {
            dimage(0, 0, &img5Base);
            if(posBonny == CAM_5) { dimage(0, 0, &img5Bonny); }
            break;
        }

        case CAM_6: {
            dclear(C_BLACK);
            dprint_opt(64, 14, C_INVERT, C_NONE, DTEXT_CENTER, DTEXT_TOP, "-CAMERA DISABLED-");
            break;
        }

        case CAM_7: {
            dimage(0, 0, &img7Base);
            if(posChica == CAM_7) { dimage(0, 0, &img7Chica); }
            if(posFreddy == CAM_7) { dimage(0, 0, &img7Freddy); }
            break;
        }
    }

    if(cam != OFFICE) {
        dprint(5, 5, C_INVERT, camToStr(currentCam));
    }

    dupdate();
    return;
}

void move(const std::array<std::array<Cam, 2>, CAM_COUNT> graph, Cam* currentPos, bool* door) {
	int chosen = bounded_rand(2);
	Cam posNext = graph[*currentPos][chosen];
	if(posNext == OFFICE) {
		if(*door || invuln) {
			*currentPos = graph[OFFICE][0];
            if(currentCam == OFFICE) {
                drawCam(OFFICE);
            }
            if(currentCam == *currentPos) {
                drawCam(*currentPos);
            }
		}
		else {
			dead = true;
		}
	}
	else {
        if(currentCam == *currentPos) {
            Cam temp = *currentPos;
            *currentPos = posNext;
            drawCam(temp);
        }
        else if(currentCam == posNext) {
            *currentPos = posNext;
            drawCam(*currentPos);
        }
        else if(( posNext == LEFT_DOOR || posNext == RIGHT_DOOR ) && currentCam == OFFICE) {
            *currentPos = posNext;
            drawCam(OFFICE);
        }
        else {
            *currentPos = posNext;
        }
	}
	return;
}

int tickAll() {
    gameTicks += 1;

    if(gameTicks % (uint)(ticksBasePower/powerUsage) == 0) {
        if(!infBatt) {
            powerLeft -= 1;
        }
        if(powerLeft == 0) {
            leftDoorClosed = false;
            rightDoorClosed = false;
            leftLightOn = false;
            rightLightOn = false;
            drawCam(OFFICE);
            dimage(0, 0, &imgBlackout);
            dupdate();
            sleep_ms(4000);
            dimage(0, 0, &imgFreddyBlackoutJumpscare);
            dupdate();
            dead = true;
        }
        if(currentCam == OFFICE) {
            drawCam(OFFICE);
        }
    }

    if(gameTicks % ticksBonny == 0) {
        if(bounded_rand(20) + 1 <= lvlBonny) {move(graphBonny, &posBonny, &leftDoorClosed);}
        if(dead) {
            leftDoorClosed = false;
            rightDoorClosed = false;
            leftLightOn = false;
            rightLightOn = false;
            drawCam(OFFICE);
            dimage(0, 0, &imgBonnyJumpscare);
            dupdate();
        }
    }
    if(gameTicks % ticksChica == 0) {
        if(bounded_rand(20) + 1 <= lvlChica) {move(graphChica, &posChica, &rightDoorClosed);}
        if(dead) {
            leftDoorClosed = false;
            rightDoorClosed = false;
            leftLightOn = false;
            rightLightOn = false;
            drawCam(OFFICE);
            dimage(0, 0, &imgChicaJumpscare);
            dupdate();
        }
    }
    if(gameTicks % ticksFreddy == 0) {
        if(bounded_rand(20) + 1 <= lvlFreddy && currentCam != posFreddy && freddyWait == 0) {
            uint64_t wait = (uint64_t)((1000 - 100 * std::min((uint)10, lvlFreddy)) / 60 / 0.01); // number of frames converted to gameticks
            freddyWait = gameTicks + wait;
        }
    }
     if(gameTicks == foxyWait || (foxyStage > 2 && foxyWait == 0)) {
        // executes when foxy is dashing
        foxyWait = 0;
        foxyStage += 1;
        if(foxyStage == 4 || foxyStage == 5) {
            // play foxy dashing animation
            if(currentCam == CAM_2A) {
                drawCam(CAM_2A);
            }
            foxyWait = gameTicks + (uint64_t)(1.6/0.01);
        }
        if(foxyStage == 6) {
            if(currentCam == CAM_2A) {
                drawCam(CAM_2A);
            }
            if(leftDoorClosed || invuln) { 
                foxyStage = 0;
                if(!infBatt) {
                    powerLeft -= foxyPowerDrain;
                }
                foxyPowerDrain += 5;
             }
            else { 
                dead = true;
                leftDoorClosed = false;
                rightDoorClosed = false;
                leftLightOn = false;
                rightLightOn = false;
                drawCam(OFFICE);
                dimage(0, 0, &imgFoxyJumpscare);
                dupdate();
            }
        }
    }
    if(gameTicks % ticksFoxy == 0) {
        if(bounded_rand(20) + 1 <= lvlFoxy && !foxyLocked && foxyStage <= 2) {
            foxyStage += 1;
        }
        if(foxyStage == 3 && foxyWait == 0) {
            foxyWait = gameTicks + (uint64_t)(25/0.01);
        }
    }

    if(gameTicks == freddyWait) {
        move(graphFreddy, &posFreddy, &rightDoorClosed);
        freddyWait = 0;
        if(dead) {
            leftDoorClosed = false;
            rightDoorClosed = false;
            leftLightOn = false;
            rightLightOn = false;
            drawCam(OFFICE);
            dimage(0, 0, &imgFreddyJumpscare);
            dupdate();
        }
    }
    if(gameTicks == foxyLockWait) {
        foxyLockWait = 0;
        foxyLocked = false;
    }

    if(gameTicks == (1*60+30)/0.01) { // 1AM
        hour = 1;
        if(currentCam == OFFICE) {
            drawCam(OFFICE);
        }
    }
    if(gameTicks == (1*60+30+(1*60+29)*1)/0.01) { // 2AM
        hour = 2;
        lvlBonny += 1;
        if(currentCam == OFFICE) {
            drawCam(OFFICE);
        }
    }
    if(gameTicks == (1*60+30+(1*60+29)*2)/0.01) { // 3AM
        hour = 3;
        lvlBonny += 1;
        lvlChica += 1;
        lvlFoxy += 1;
        if(currentCam == OFFICE) {
            drawCam(OFFICE);
        }
    }
    if(gameTicks == (1*60+30+(1*60+29)*3)/0.01) { // 4AM
        hour = 4;
        lvlBonny += 1;
        lvlChica += 1;
        lvlFoxy += 1;
        if(currentCam == OFFICE) {
            drawCam(OFFICE);
        }
    }
    if(gameTicks == (1*60+30+(1*60+29)*4)/0.01) { // 5AM
        hour = 5;
        if(currentCam == OFFICE) {
            drawCam(OFFICE);
        }
    }
    if(gameTicks == (1*60+30+(1*60+29)*5)/0.01) { // 6AM
        hour = 6;
        if(currentCam == OFFICE) {
            drawCam(OFFICE);
        }
    }

    if(dead || hour == 6) {
        return TIMER_STOP;
    }
    return TIMER_CONTINUE;
}

void printData() {
    dclear(C_WHITE);
	dprint(1, 1+8*0, C_BLACK, "Bonny: %s", camToStr(posBonny));
	dprint(1, 1+8*1, C_BLACK, "Chica: %s", camToStr(posChica));
	dprint(1, 1+8*2, C_BLACK, "Freddy: %s", camToStr(posFreddy));
    dprint(1, 1+8*3, C_BLACK, "Foxy: %d", foxyStage);
    dprint(1, 1+8*4, C_BLACK, "Hour: %dAM", hour);
    dprint(1, 1+8*5, C_BLACK, "Power: %d", powerLeft);
    dprint(1, 1+8*6, C_BLACK, "Drain: %d", powerUsage);
    dprint(1, 1+8*7, C_BLACK, "Cam: %s", camToStr(currentCam));
    dprint(80, 1+8*0, C_BLACK, "GT: %d", gameTicks);
    dprint(80, 1+8*1, C_BLACK, "AW: %d", actionWait);
    dprint(80, 1+8*2, C_BLACK, "LDr: %s", leftDoorClosed ? "Yes" : "No");
    dprint(80, 1+8*3, C_BLACK, "RDr: %s", rightDoorClosed ? "Yes" : "No");
    dprint(80, 1+8*4, C_BLACK, "LLt: %s", leftLightOn ? "On" : "Off");
    dprint(80, 1+8*5, C_BLACK, "RLt: %s", rightLightOn ? "On" : "Off");
    dprint(80, 1+8*6, C_BLACK, "FLk: %s", foxyLocked ? "Yes" : "No");
    dprint(80, 1+8*7, C_BLACK, "FLkWt: %d", foxyLockWait);
    dupdate();
}

void startCustomNight(uint bonnyLVL, uint chicaLVL, uint freddyLVL, uint foxyLVL, float powerDrainTime = 3) {
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
    drawCam(OFFICE);
}

bool customNightMenu() {
    int selected = 0;
    int bonnyLvl = 0;
    int chicaLvl = 0;
    int freddyLvl = 0;
    int foxyLvl = 0;
    while(true) {
        dimage(0, 0, &imgCNMenuBase);
        dprint_opt(19, 40, C_WHITE, C_NONE, DTEXT_RIGHT, DTEXT_TOP, "%d", freddyLvl);
        dprint_opt(53, 40, C_WHITE, C_NONE, DTEXT_RIGHT, DTEXT_TOP, "%d", bonnyLvl);
        dprint_opt(84, 40, C_WHITE, C_NONE, DTEXT_RIGHT, DTEXT_TOP, "%d", chicaLvl);
        dprint_opt(116, 40, C_WHITE, C_NONE, DTEXT_RIGHT, DTEXT_TOP, "%d", foxyLvl);
        switch (selected)
        {
            case 0: drect(3, 40, 25, 47, C_INVERT); break;
            case 1: drect(37, 40, 59, 47, C_INVERT); break;
            case 2: drect(68, 40, 90, 47, C_INVERT); break;
            case 3: drect(101, 40, 122, 47, C_INVERT); break;
            case 4: drect(88, 53, 116, 61, C_INVERT); break;
        }
        dupdate();
        key_event_t key = getkey();
        if(key.key == KEY_LEFT && selected > 0) {
            selected -= 1;
        }
        else if(key.key == KEY_RIGHT && selected < 4) {
            selected += 1;
        }
        else if(key.key == KEY_UP && selected != 4) {
            switch (selected)
            {
                case 0: {
                    freddyLvl += 1;
                    if(freddyLvl > 20) {
                        freddyLvl = 0;
                    }
                    break;
                }
                case 1: {
                    bonnyLvl += 1;
                    if(bonnyLvl > 20) {
                        bonnyLvl = 0;
                    }
                    break;
                }
                case 2: {
                    chicaLvl += 1;
                    if(chicaLvl > 20) {
                        chicaLvl = 0;
                    }
                    break;
                }
                case 3: {
                    foxyLvl += 1;
                    if(foxyLvl > 20) {
                        foxyLvl = 0;
                    }
                    break;
                }
            }
        }
        else if(key.key == KEY_DOWN && selected != 4) {
            switch (selected)
            {
                case 0: {
                    freddyLvl -= 1;
                    if(freddyLvl < 0) {
                        freddyLvl = 20;
                    }
                    break;
                }
                case 1: {
                    bonnyLvl -= 1;
                    if(bonnyLvl < 0) {
                        bonnyLvl = 20;
                    }
                    break;
                }
                case 2: {
                    chicaLvl -= 1;
                    if(chicaLvl < 0) {
                        chicaLvl = 20;
                    }
                    break;
                }
                case 3: {
                    foxyLvl -= 1;
                    if(foxyLvl < 0) {
                        foxyLvl = 20;
                    }
                    break;
                }
            }
        }
        else if(key.key == KEY_EXE) {
            switch (selected)
            {
                case 0: freddyLvl += 1; break;
                case 1: bonnyLvl += 1; break;
                case 2: chicaLvl += 1; break;
                case 3: foxyLvl += 1; break;
                case 4: startCustomNight(bonnyLvl, chicaLvl, freddyLvl, foxyLvl); return false;
            }
        }
        else if(key.key == KEY_EXIT  || key.key == KEY_MENU) {
            return true;
        }
    }
    return false;
}

void switchCamera(Cam newCam) {
    if(newCam != OFFICE && currentCam == OFFICE) {powerUsage += 1;}
    if(newCam == OFFICE && currentCam != OFFICE) {powerUsage -= 1;}

    if(newCam != OFFICE) {
        foxyLocked = true;
        foxyLockWait = 0;
        if(foxyStage == 3 && ( newCam == CAM_1C || newCam == CAM_2A )) {
            foxyWait = 0;
        }
    }
    else {
        if(currentCam != OFFICE) {
            foxyLockWait = gameTicks + (uint64_t)((bounded_rand(1667+1-83)+83)/100/0.01);
        }
    }

    currentCam = newCam;
    drawCam(currentCam);

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
    drawCam(OFFICE);
    actionWait = 64;
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
    drawCam(OFFICE);
    actionWait = 32;
}

void startNight(int night) {
    if(night == 1) {
        startCustomNight(0, 0, 0, 0, 9.6);
    }
    else if(night == 2) {
        startCustomNight(3, 1, 1, 0, 6);
    }
    else if(night == 3) {
        startCustomNight(0, 5, 2, 1, 5);
    }
    else if(night == 4) {
        startCustomNight(2, 4, 6, bounded_rand(2) + 1, 4);
    }
    else if(night == 5) {
        startCustomNight(5, 7, 5, 3, 4);
    }
    else if(night == 6) {
        startCustomNight(10, 12, 16, 4);
    }
}


int main(void)
{
    while(true) {
        bool CNAborted = false;
        int choice = mainMenu();

        switch (choice)
        {
            case -1: return 0;
            case 0: CNAborted = customNightMenu(); break;
            default: startNight(choice); break;
            
        }

        if(!CNAborted) { // main game loop
            uint ticks = rtc_ticks();
        	while(true) {
        		clearevents();
        		if(keydown(KEY_EXIT) || keydown(KEY_MENU)) { break; }
            
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

                if(infoVision) {printData();}

                // death logic here instead of in a function call
        		if(dead) {
                    timer_stop(animatronic_timer);
                    sleep_ms(2000);
                    break;
        		}

                if(hour == 6) {
        			dclear(C_WHITE);
        			dprint(1, 1, C_BLACK, "WIN");
                    dupdate();
                    timer_stop(animatronic_timer);
                    break;
        		}

                if(rtc_ticks() - ticks >= 1 && actionWait > 0) {
                    actionWait -= 1;
                    ticks = rtc_ticks();
                }

        	}

            if(hour == 6) {
                getkey();
            }
        }
    }

	return 0;
}