#ifndef NSPRS_PARAMS_H_
#define NSPRS_PARAMS_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//default app values
#define TEMP_ESPR					80//95
#define TEMP_LUNGO					80//95
#define TEMP_HEATUP					75//90
#define CUP_SIZE_ESPR				138
#define CUP_SIZE_LUNGO				328
#define HEATUP_RAMP					0.5
#define TEMP_MAX_SLOPE_ADUJSMENT	0

#define HEATALG_SLOW_TIMEON			250
#define HEATALG_SLOW_TIMEOFF		250


#define READY_WAIT_TIME				5000
#define BREW_REGULATION_PERIOD		6000
#define BREW_UPDATING_PERIOD		300
#define BREW_NOREG_STEP1_TIME		1000
#define BREW_NOREG_STEP2_TIME		4000
#define BREW_NOREG_STEP2_FLOW_LIMIT	30
#define BREW_REG_INIT_TIME			1000

//cmds which send to timer sys srvc
#define SS_TIMER_CMD_SLEEP					10
#define SS_TIMER_CMD_START_INTRBASED		1
#define SS_TIMER_CMD_START_FREERUNNING		2
#define SS_TIMER_CMD_READ_FREERUNNING		3
#define SS_TIMER_CMD_FINISH_FREERUNNING		4
//cmds which sends to temp sys srvc
#define SS_TEMP_CMD_SLEEP			0
#define SS_TEMP_CMD_READ			1
//cmds which sends to flow sys srvc
#define SS_FLOW_CMD_SLEEP			0
#define SS_FLOW_CMD_RST_ACTIVATE	1
#define SS_FLOW_CMD_READ			2
//cmds which sends to HMI sys srvc
#define SS_HMI_CMD_SLEEP			0
#define SS_HMI_CMD_ACTIVATE			1
#define SS_HMI_CMD_READ				2
#define SS_HMI_CMD_RESET			3
//cmds which sends heatpump sys srvc
#define SS_HEATPUMP_CMD_SLEEP		1<<2
#define SS_HEATPUMP_CMD_EN_HEATER	1<<0
#define SS_HEATPUMP_CMD_EN_PMP		1<<1
#define SS_HEATPUMP_CMD_EN_BOTH		1<<0 | 1<<1
//cmds which sends pump sys srvc
#define SS_PUMP_CMD_SLEEP		0
#define SS_PUMP_CMD_EN			1

#define SS_TRGHEATER_CMD_SLEEP		0
#define SS_TRGHEATER_CMD_EN		1


//temporary! //todo: remove after implementing the related functions
#define HEATUP_INITIAL_TIME			2500 //ms
#define HEATUP_IDLE_TIME			1000
#define BREW_REG_ON_TIME			3000




//================= PRIORITIES ==============
#define IDLE_PRIORITY		tskIDLE_PRIORITY
#define LOW_PRIORITY		IDLE_PRIORITY + 1
#define NORMAL_PRIORITY		LOW_PRIORITY + 1
#define HIGH_PRIORITY		NORMAL_PRIORITY + 1

//====== platform ========
#define MB_PYNQZ2						0
#define ARMCM0_NUCLEO_F070RB			1
#define NUCLEO_G070RB					2
#define PLATFORM						NUCLEO_G070RB

//=== Buttons =====
//bits pressing and releasing
#define ESPRS_PRESSED				0
#define ESPRS_RELEASED				1
#define LUNGO_PRESSED				2
#define LUNGO_RELEASED				3
#define HW_PRESSED					4
#define HW_RELEASED					5
#define NSPRS_PRESSED				6
#define NSPRS_RELEASED				7

//=== global variables ===
struct globalVars {
	int temperature;
	int temperature_target;
	unsigned int flow;
	unsigned int volume_passed;
	unsigned int volume_target;
};
extern struct globalVars global_vars;


//===== app states
typedef enum {
	asStateInit,
	asStateHeatup,
	asStateReady,
	asStateBrewing,
	asStatePoweroff,
	asStateTest
} asStates;
extern asStates asState;

typedef enum {
	appSM_heatup_state_init,
	appSM_heatup_state_waitTempReched,
	appSM_heatup_state_finished
} appSM_heatup_states;
extern appSM_heatup_states appSM_heatup_state;

typedef enum {
	appSM_ready_state_init,
	appSM_ready_state_waitAndCheck
} appSM_ready_states;
extern appSM_ready_states appSM_ready_state;

typedef enum {
	appSM_brew_state_init,
	appSM_brew_state_checkExit,
	appSM_brew_state_exit
} appSM_brew_states;
extern appSM_brew_states appSM_brew_state;

typedef enum {
	appSM_APO_state_init,
	appSM_APO_state_checkForWakeup
} appSM_APO_states;
extern appSM_APO_states appSM_APO_state;


//=== commands
struct HEATPUMP_CMD{
	int sleep;
	int en_heater;
	int en_pump;
	int en_both;
};
struct TRGPUMP_CMD{
	int sleep;
	int activate;
};
struct TRGHEATER_CMD{
	int sleep;
	int activate;
};
struct HEATPUMP{
	const struct HEATPUMP_CMD cmd;
};
struct TRGPUMP{
	const struct TRGPUMP_CMD cmd;
};
struct TRGHEATER{
	const struct TRGHEATER_CMD cmd;
};

struct TEMP_CMD{
	int sleep;
	int activate;
};
struct TEMP{
	const struct TEMP_CMD cmd;
};

struct FLOW_CMD{
	int sleep;
	int rst_activate;
	int read;
};
struct FLOW{
	const struct FLOW_CMD cmd;
};

struct HMI_CMD{
	int sleep;
	int activate;
	int read;
	int reset;
};
struct HMI{
	const struct HMI_CMD cmd;
};

struct TIMER_CMDS{
	int sleep;
	int start_intr_based;
	int start_freerunning;
	int read_freerunning;
	int finish_freerunning;
};
struct TIMER{
	const struct TIMER_CMDS cmd;
};

struct HEATALG_CMDS{
	int sleep;
	int activate;
};
struct HEATALG{
	const struct HEATALG_CMDS cmd;
};

struct SYS_SRVCS{
	struct TIMER timer;
	struct TEMP temp;
	struct FLOW flow;
	struct HMI hmi;
	struct HEATPUMP heatpump;
	struct TRGPUMP trgpump;
	struct TRGHEATER trgheater;
	struct HEATALG heatalg;
};


//struct
struct PARAMS_APP{
	int temp_espr;
	int temp_lungo;
	int temp_heatup;
	int cup_size_espr; //Flowmeter pulses of an espresso cup  + losses(40ml + 5ml)
	int cup_size_lungo; //Flowmeter pulses of a lungo cup + losses (110ml + 14ml)
	float heatUpRamp;
	int temp_max_slope_adjustment;
	int ready_wait_time;
	int brew_regulation_period;
	int brew_updating_period;
	int brew_noreg_step1_time;
	int brew_noreg_step2_time;
	int brew_noreg_step2_flow_limit;
	int brew_reg_init_time;
};

struct PARAMS{
	struct PARAMS_APP app;
	struct SYS_SRVCS sys_srvc;
};

extern struct PARAMS params;


#endif //NSPRS_PARAMS_H_
