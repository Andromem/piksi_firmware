/*
 * Copyright (C) 2012 Fergus Noble <fergusnoble@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SWIFTNAV_SBP_MESSAGES_H
#define SWIFTNAV_SBP_MESSAGES_H

/** \addtogroup sbp
 * \{ */

/** \defgroup msgs Messages
 * Swift Binary Protocol Message ID's.
 * \{ */

#define MSG_PRINT 0x01 /**< Python */

#define MSG_ALMANAC   0x69 /**< C */
#define MSG_SET_TIME  0x68 /**< C */

#define MSG_BOOTLOADER_HANDSHAKE   0xB0 /**< C and Python */
#define MSG_BOOTLOADER_JUMP_TO_APP 0xB1 /**< C */

#define MSG_CW_START   0xC1 /**< C */
#define MSG_CW_RESULTS 0xC0 /**< Python */

#define MSG_NAP_DEVICE_DNA 0xDD /**< C and Python */

#define MSG_STM_FLASH_WRITE 0xE0 /**< C */
#define MSG_STM_FLASH_READ  0xE1 /**< C and Python */
#define MSG_STM_FLASH_ERASE 0xE2 /**< C */
#define MSG_STM_FLASH_DONE  0xE0 /**< Python */

#define MSG_STM_UNIQUE_ID 0xE5 /**< C and Python */

#define MSG_M25_FLASH_WRITE 0xF0 /**< C */
#define MSG_M25_FLASH_READ  0xF1 /**< C and Python */
#define MSG_M25_FLASH_ERASE 0xF2 /**< C */
#define MSG_M25_FLASH_DONE  0xF0 /**< Python */

#define MSG_SOLUTION 0x50 /**< Python */
#define MSG_DOPS     0x51 /**< Python */
#define MSG_PR_ERRS  0x52 /**< Python */

#define MSG_TRACKING_STATE 0x22 /**< Python */

/** \} */

/** \} */

#endif /* SWIFTNAV_SBP_MESSAGES_H */