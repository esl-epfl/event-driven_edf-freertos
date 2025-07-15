#include "devices_params.h"

asStates asState = asStateInit;

appSM_heatup_states 	appSM_heatup_state 	= appSM_heatup_state_init;
appSM_ready_states 		appSM_ready_state 	= appSM_ready_state_init;
appSM_brew_states 		appSM_brew_state 	= appSM_brew_state_init;
appSM_APO_states		appSM_APO_state 	= appSM_APO_state_init;

struct globalVars global_vars = {
		.temperature = 0, .flow=0, .volume_passed=0,
		.temperature_target=TEMP_HEATUP, .volume_target = CUP_SIZE_ESPR
};

struct PARAMS params = {
	.app.temp_espr = TEMP_ESPR,
	.app.temp_lungo = TEMP_LUNGO,
	.app.temp_heatup = TEMP_HEATUP,
	.app.cup_size_espr = CUP_SIZE_ESPR,
	.app.cup_size_lungo = CUP_SIZE_LUNGO,
	.app.heatUpRamp = HEATUP_RAMP,
	.app.temp_max_slope_adjustment = TEMP_MAX_SLOPE_ADUJSMENT,
	.app.ready_wait_time = READY_WAIT_TIME, //todo 20*1000
	.app.brew_regulation_period = BREW_REGULATION_PERIOD, //todo 6*1000
	.app.brew_updating_period = BREW_UPDATING_PERIOD, //todo 300
	.app.brew_noreg_step1_time = BREW_NOREG_STEP1_TIME, //todo 1 * 1000
	.app.brew_noreg_step2_time = BREW_NOREG_STEP2_TIME, //todo 4 * 1000
	.app.brew_noreg_step2_flow_limit = BREW_NOREG_STEP2_FLOW_LIMIT,
	.app.brew_reg_init_time = BREW_REG_INIT_TIME,

	.sys_srvc.timer.cmd.sleep = SS_TIMER_CMD_SLEEP,
	.sys_srvc.timer.cmd.start_intr_based = SS_TIMER_CMD_START_INTRBASED,
	.sys_srvc.timer.cmd.start_freerunning = SS_TIMER_CMD_START_FREERUNNING,
	.sys_srvc.timer.cmd.read_freerunning = SS_TIMER_CMD_READ_FREERUNNING,
	.sys_srvc.timer.cmd.finish_freerunning = SS_TIMER_CMD_FINISH_FREERUNNING,

	.sys_srvc.hmi.cmd.sleep = SS_HMI_CMD_SLEEP,
	.sys_srvc.hmi.cmd.activate = SS_HMI_CMD_ACTIVATE,
	.sys_srvc.hmi.cmd.read = SS_HMI_CMD_READ,
	.sys_srvc.hmi.cmd.reset = SS_HMI_CMD_RESET,

	.sys_srvc.temp.cmd.sleep = 0,
	.sys_srvc.temp.cmd.activate = 1,

	.sys_srvc.flow.cmd.sleep = SS_FLOW_CMD_SLEEP,
	.sys_srvc.flow.cmd.rst_activate = SS_FLOW_CMD_RST_ACTIVATE,
	.sys_srvc.flow.cmd.read = SS_FLOW_CMD_READ,

	.sys_srvc.heatpump.cmd.sleep = SS_HEATPUMP_CMD_SLEEP,
	.sys_srvc.heatpump.cmd.en_heater = SS_HEATPUMP_CMD_EN_HEATER,
	.sys_srvc.heatpump.cmd.en_pump = SS_HEATPUMP_CMD_EN_PMP,
	.sys_srvc.heatpump.cmd.en_both = SS_HEATPUMP_CMD_EN_BOTH,

	.sys_srvc.trgpump.cmd.sleep = 0,
	.sys_srvc.trgpump.cmd.activate = 1,

	.sys_srvc.trgheater.cmd.sleep = 0,
	.sys_srvc.trgheater.cmd.activate = 1,

	.sys_srvc.heatalg.cmd.sleep = 0,
	.sys_srvc.heatalg.cmd.activate = 1

};








