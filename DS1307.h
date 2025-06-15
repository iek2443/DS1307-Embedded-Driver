/**
  ******************************************************************************
  * @file    DS1307.h
  * @author  iek2443
  * @brief   Header file for the platform-independent DS1307 RTC driver.
  *          Contains register definitions, enums, context structures,
  *          and function prototypes for date/time operations via I2C.
  * @date    Created on: Jun 13, 2025
  ******************************************************************************
  * @attention
  *
  * This file provides a portable interface to interact with the DS1307
  * Real-Time Clock (RTC) chip. The driver uses user-defined I2C function
  * pointers for platform independence.
  *
  ******************************************************************************
  */

#ifndef INC_DS1307_H_
#define INC_DS1307_H_

#include <stdint.h>

/**
 * @brief  DS1307 I2C address definitions.
 */
typedef enum {
	DS1307_7_BIT_ADR = 0x68, /*!< 7-bit I2C address of DS1307 */
	DS1307_WRITE_ADR = 0xD0, /*!< 8-bit write address (including R/W bit) */
	DS1307_READ_ADR = 0xD1 /*!< 8-bit read address (including R/W bit) */
} ds1307_adr_t;

/**
 * @brief  DS1307 register address map.
 */
typedef enum {
	DS1307_SEC_REG_ADR = 0x00, /*!< Seconds register */
	DS1307_MIN_REG_ADR, /*!< Minutes register */
	DS1307_HOUR_REG_ADR, /*!< Hours register */
	DS1307_DAY_REG_ADR, /*!< Day of the week register */
	DS1307_DATE_REG_ADR, /*!< Day of the month register */
	DS1307_MONTH_REG_ADR, /*!< Month register */
	DS1307_YEAR_REG_ADR, /*!< Year register */
	DS1307_CONT_REG_ADR /*!< Control register */
} ds1307_reg_adr_t;

/**
 * @brief  DS1307 clock control enumeration for CH (Clock Halt) bit.
 */
typedef enum {
	DS1307_CLOCK_ENABLE, /*!< Enable clock (CH = 0) */
	DS1307_CLOCK_DISABLE /*!< Disable clock (CH = 1) */
} ds1307_clock_t;

/**
 * @brief  Enumeration for days of the week (1 = Monday, 7 = Sunday).
 */
typedef enum {
	DS1307_MONDAY = 1,
	DS1307_TUESDAY,
	DS1307_WEDNESDAY,
	DS1307_THURSDAY,
	DS1307_FRIDAY,
	DS1307_SATURDAY,
	DS1307_SUNDAY
} ds1307_day_t;

/**
 * @brief  Enumeration for months of the year (1 = January, 12 = December).
 */
typedef enum {
	DS1307_JANUARY = 1,
	DS1307_FEBRUARY,
	DS1307_MARCH,
	DS1307_APRIL,
	DS1307_MAY,
	DS1307_JUNE,
	DS1307_JULY,
	DS1307_AUGUST,
	DS1307_SEPTEMBER,
	DS1307_OCTOBER,
	DS1307_NOVEMBER,
	DS1307_DECEMBER
} ds1307_month_t;

/**
 * @brief  Enumeration for time periods used in 12-hour format.
 */
typedef enum {
	DS1307_AM, /*!< Ante meridiem (AM) */
	DS1307_PM, /*!< Post meridiem (PM) */
	DS1307_NONE /*!< Not applicable (used in 24-hour format) */
} ds1307_timeperiod_t;

/**
 * @brief  Enumeration for hour format modes.
 */
typedef enum {
	DS1307_HOUR_FORMAT_24, /*!< 24-hour format */
	DS1307_HOUR_FORMAT_12 /*!< 12-hour format */
} ds_1307_hour_format_t;

/**
 * @brief  Function pointer type for I2C memory write operation.
 * @param  address: I2C address of the DS1307 device.
 * @param  reg_adr: Register address within the DS1307 device.
 * @param  ds1307_data: Pointer to the data buffer to write.
 * @param  size: Number of bytes to write.
 * @retval None
 */
typedef void (*ds1307_i2c_mem_write_func_t)(ds1307_adr_t address,
		ds1307_reg_adr_t reg_adr, uint8_t *ds1307_data, uint16_t size);

/**
 * @brief  Function pointer type for I2C memory read operation.
 * @param  address: I2C address of the DS1307 device.
 * @param  reg_adr: Register address within the DS1307 device.
 * @param  ds1307_data: Pointer to the buffer to store received data.
 * @param  size: Number of bytes to read.
 * @retval None
 */
typedef void (*ds1307_i2c_mem_read_func_t)(ds1307_adr_t address,
		ds1307_reg_adr_t reg_adr, uint8_t *ds1307_data, uint16_t size);

/**
 * @brief  Structure holding user-provided function pointers for I2C communication.
 * @note   These function pointers must be assigned to valid platform-specific
 *         I2C read and write implementations before using the driver.
 */
/**
 * @brief  Structure holding user-provided function pointers for I2C communication.
 * @note   These function pointers must be assigned to valid platform-specific
 *         I2C read and write implementations before using the driver.
 */
typedef struct {
	ds1307_i2c_mem_write_func_t ds1307_i2c_send_ptr; /*!< Pointer to I2C write function */
	ds1307_i2c_mem_read_func_t ds1307_i2c_read_ptr; /*!< Pointer to I2C read function */
} ds1307_user_func_t;

/**
 * @brief  DS1307 context structure containing time data and I2C function pointers.
 * @note   This structure serves as the main interface between the user application
 *         and the DS1307 driver. It holds both the current date/time values and the
 *         user-provided I2C function pointers required for communication.
 * @see    ds1307_user_func_t
 */
typedef struct {
	ds1307_user_func_t functions; /*!< User-defined I2C read/write function pointers */
	ds1307_day_t day; /*!< Day of the week (1 = Monday, 7 = Sunday) */
	ds1307_month_t month; /*!< Month of the year (1 = January, 12 = December) */
	uint16_t year; /*!< Full 4-digit year (e.g., 2025) */
	uint8_t hour; /*!< Current hour value (0–23 for 24H format, 1–12 for 12H format) */
	uint8_t minute; /*!< Current minute value (0–59) */
	uint8_t second; /*!< Current second value (0–59) */
	uint8_t date; /*!< Day of the month (1–31) */
	ds1307_timeperiod_t time_period; /*!< Time period indicator (AM, PM, or NONE for 24H mode) */
	ds_1307_hour_format_t time_format; /*!< Hour format: 12-hour or 24-hour */
	uint16_t century; /*!< Century offset (e.g., 2000 or 2100) for full year reconstruction */
} ds1307_context_t;

/**
 * @brief Sends data to the DS1307 over I2C using user-defined function. (internal use only)
 */
static void ds1307_i2c_send(ds1307_context_t *usr, ds1307_adr_t address,
		ds1307_reg_adr_t reg_adr, uint8_t *ds1307_data, uint16_t size);

/**
 * @brief Sets the minute value on the DS1307.
 */
static void ds1307_i2c_read(ds1307_context_t *usr, ds1307_adr_t address,
		ds1307_reg_adr_t reg_adr, uint8_t *ds1307_data, uint16_t size);

/**
 * @brief  Sets the minute value.
 */
void ds1307_set_minute(ds1307_context_t *usr, uint8_t minute);

/**
 * @brief  Gets the minute value and updates the context.
 */
void ds1307_get_minute(ds1307_context_t *usr);

/**
 * @brief  Sets the second value.
 */
void ds1307_set_second(ds1307_context_t *usr, uint8_t second);

/**
 * @brief  Gets the second value and updates the context.
 */
void ds1307_get_second(ds1307_context_t *usr);

/**
 * @brief  Sets the hour value. Format must be set in usr->time_format.
 */
void ds1307_set_hour(ds1307_context_t *usr, uint8_t hour);

/**
 * @brief  Gets the hour value and updates the context.
 */
void ds1307_get_hour(ds1307_context_t *usr);

/**
 * @brief  Sets the hour format (12H/24H) and updates the time accordingly.
 */
void ds1307_set_time_format(ds1307_context_t *usr, ds_1307_hour_format_t format);

/**
 * @brief  Sets the day of the week.
 */
void ds1307_set_day(ds1307_context_t *usr, ds1307_day_t day);

/**
 * @brief  Gets the day of the week and updates the context.
 */
void ds1307_get_day(ds1307_context_t *usr);

/**
 * @brief  Sets the date (day of the month).
 */
void ds1307_set_date(ds1307_context_t *usr, uint8_t date);

/**
 * @brief  Gets the date and updates the context.
 */
void ds1307_get_date(ds1307_context_t *usr);

/**
 * @brief  Sets the month value.
 */
void ds1307_set_month(ds1307_context_t *usr, ds1307_month_t month);

/**
 * @brief  Gets the month and updates the context.
 */
void ds1307_get_month(ds1307_context_t *usr);

/**
 * @brief  Sets the full year value (century stored separately).
 */
void ds1307_set_year(ds1307_context_t *usr, uint16_t year);

/**
 * @brief  Gets the full year and updates the context.
 */
void ds1307_get_year(ds1307_context_t *usr);

/**
 * @brief  Reads all date and time values from the DS1307 and updates the context.
 */
void DS1307_read_date_time(ds1307_context_t *usr);

/**
 * @brief  Converts a decimal value to BCD format (internal use only).
 */
static uint8_t DecToBcd(uint8_t value);

/**
 * @brief  Converts a BCD value to decimal format (internal use only).
 */
static uint8_t BcdToDec(uint8_t value);

/**
 * @brief  Enables or disables the DS1307 oscillator via CH bit.
 */
void ds1307_set_ch(ds1307_context_t *usr, ds1307_clock_t value);

#endif /* INC_DS1307_H_ */
