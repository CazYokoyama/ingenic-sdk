/*
 * Copyright (C) 2015 Ingenic Semiconductor Co.,Ltd
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __MOTOR_H__
#define __MOTOR_H__

#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <jz_proc.h>
/*
 *  HORIZONTAL is X axis and VERTICAL is Y axis;
 *  while the Zero point is left-bottom, Origin point
 *  is cross point of horizontal midpoint and vertical midpoint.
 *
*/


/*#define PLATFORM_HAS_HORIZONTAL_MOTOR 	1*/
/*#define PLATFORM_HAS_VERTICAL_MOTOR 	1*/

enum jz_motor_cnt {
	HORIZONTAL_MOTOR,
	VERTICAL_MOTOR,
	HAS_MOTOR_CNT,
};


/*************************** HORIZONTAL  MOTOR ************************************/
#define HORIZONTAL_MIN_GPIO		GPIO_PC(13)	/**< motor start point */
#define HORIZONTAL_MAX_GPIO		GPIO_PC(14)	/**< motor stop point */
#define HORIZONTAL_GPIO_LEVEL	0		/**< motor irq style */

#define HORIZONTAL_ST1_GPIO		GPIO_PB(22)	/**< Phase A */
#define HORIZONTAL_ST2_GPIO		GPIO_PB(21)	/**< Phase B */
#define HORIZONTAL_ST3_GPIO		GPIO_PB(20)	/**< Phase C */
#define HORIZONTAL_ST4_GPIO		GPIO_PB(19)	/**< Phase D */

/*************************** VERTICAL  MOTOR ************************************/
#define VERTICAL_MIN_GPIO		GPIO_PC(18)
#define VERTICAL_MAX_GPIO		GPIO_PB(28)
#define VERTICAL_GPIO_LEVEL		0

#define VERTICAL_ST1_GPIO		GPIO_PC(11)
#define VERTICAL_ST2_GPIO		GPIO_PC(12)
#define VERTICAL_ST3_GPIO		GPIO_PC(15)
#define VERTICAL_ST4_GPIO		GPIO_PC(16)

/****************************** MOTOR END ************************************/

/* ioctl cmd */
#define MOTOR_STOP		0x1
#define MOTOR_RESET		0x2
#define MOTOR_MOVE		0x3
#define MOTOR_GET_STATUS	0x4
#define MOTOR_SPEED		0x5
#define MOTOR_GOBACK	0x6
#define MOTOR_CRUISE	0x7

/* motor speed */
#define MOTOR_MAX_SPEED	900		/**< unit: beats per second */
#define MOTOR_MIN_SPEED	100

enum motor_status {
	MOTOR_IS_STOP,
	MOTOR_IS_RUNNING,
};

struct motor_message {
	int x;
	int y;
	enum motor_status status;
	int speed;
};

struct motors_steps{
	int x;
	int y;
};

struct motor_reset_data {
	unsigned int x_max_steps;
	unsigned int y_max_steps;
	unsigned int x_cur_step;
	unsigned int y_cur_step;
};

enum motor_direction {
	MOTOR_MOVE_LEFT_DOWN = -1,
	MOTOR_MOVE_STOP,
	MOTOR_MOVE_RIGHT_UP,
};

struct motor_platform_data {
	const char name[32];
	unsigned int motor_min_gpio;
	unsigned int motor_max_gpio;
	int motor_gpio_level;

	unsigned int motor_st1_gpio;
	unsigned int motor_st2_gpio;
	unsigned int motor_st3_gpio;
	unsigned int motor_st4_gpio;
};

enum motor_ops_state {
	MOTOR_OPS_NORMAL,
	MOTOR_OPS_CRUISE,
	MOTOR_OPS_RESET,
	MOTOR_OPS_STOP,
};

struct motor_driver {
	struct motor_platform_data *pdata;
	int max_pos_irq;
	int min_pos_irq;
	int max_steps;	/* It is right-top point when x is max and y is max.*/
	int cur_steps;	/* It is left-bottom point when x is 0 and y is 0.*/
	int total_steps;
	char reset_min_pos;
	char reset_max_pos;
	enum motor_direction move_dir;
	enum motor_ops_state state;
	struct completion reset_completion;

	struct timer_list min_timer;
	struct timer_list max_timer;
	/* debug parameters */
	unsigned int max_pos_irq_cnt;
	unsigned int min_pos_irq_cnt;
};

struct motor_move {
	struct motors_steps one;
	short times;
};

struct motor_device {
	struct platform_device *pdev;
	const struct mfd_cell *cell;
	struct device	 *dev;
	struct miscdevice misc_dev;
	struct motor_driver motors[HAS_MOTOR_CNT];
	struct ingenic_tcu_chn *tcu;
	int tcu_speed;

	struct mutex dev_mutex;
	spinlock_t slock;

	enum motor_ops_state dev_state;
	struct motor_message msg;
	struct motor_move dst_move;
	struct motor_move cur_move;

	int run_step_irq;
	int flag;

	/* debug parameters */
	struct proc_dir_entry *proc;
};

#endif // __MOTOR_H__
