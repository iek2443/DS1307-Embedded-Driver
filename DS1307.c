/**
  ******************************************************************************
  * @file    DS1307.c
  * @author  iek2443
  * @brief   Source file for the platform-independent DS1307 RTC driver.
  *          Includes date/time read-write operations, BCD conversions,
  *          and I2C communication helper functions via user-defined interfaces.
  * @date    Created on: Jun 13, 2025
  ******************************************************************************
  * @attention
  *
  * This file is part of a hardware abstraction layer for the DS1307 RTC.
  * It is designed to be portable across different microcontroller platforms
  * by using user-provided I2C function pointers.
  *
  ******************************************************************************
  */
#include "DS1307.h"

/**
  * @brief  Sends data to the DS1307 device over I2C.
  * @param  usr: Pointer to the DS1307 context structure that contains function pointers.
  * @param  address: I2C address of the DS1307 device.
  * @param  reg_adr: Target register address within the DS1307 device.
  * @param  ds1307_data: Pointer to the data buffer to be sent.
  * @param  size: Number of bytes to send from the data buffer.
  * @retval None
  */
static void ds1307_i2c_send(ds1307_context_t *usr, ds1307_adr_t address,
		ds1307_reg_adr_t reg_adr, uint8_t *ds1307_data, uint16_t size) {

	usr->functions.ds1307_i2c_send_ptr(address, reg_adr, ds1307_data, size);

}

/**
  * @brief  Reads data from the DS1307 device over I2C.
  * @param  usr: Pointer to the DS1307 context structure that contains function pointers.
  * @param  address: I2C address of the DS1307 device.
  * @param  reg_adr: Target register address within the DS1307 device.
  * @param  ds1307_data: Pointer to the buffer where the received data will be stored.
  * @param  size: Number of bytes to read from the device.
  * @retval None
  */
static void ds1307_i2c_read(ds1307_context_t *usr, ds1307_adr_t address,
		ds1307_reg_adr_t reg_adr, uint8_t *ds1307_data, uint16_t size) {

	usr->functions.ds1307_i2c_read_ptr(address, reg_adr, ds1307_data, size);

}

/**
  * @brief  Sets the minute value on the DS1307 device.
  * @param  usr: Pointer to the DS1307 context structure.
  * @param  minute: Minute value to set (0 to 59), in decimal format.
  * @retval None
  */
void ds1307_set_minute(ds1307_context_t *usr, uint8_t minute) {
	minute = DecToBcd(minute);
	ds1307_i2c_send(usr, DS1307_WRITE_ADR, DS1307_MIN_REG_ADR, &minute, 1);
}

/**
  * @brief  Reads the minute value from the DS1307 device and updates the context structure.
  * @param  usr: Pointer to the DS1307 context structure where the result will be stored.
  * @retval None
  */
void ds1307_get_minute(ds1307_context_t *usr) {
	uint8_t minute = 0;
	ds1307_i2c_read(usr, DS1307_READ_ADR, DS1307_MIN_REG_ADR, &minute, 1);

	usr->minute = BcdToDec(minute);
}

/**
  * @brief  Sets the second value on the DS1307 device.
  * @param  usr: Pointer to the DS1307 context structure.
  * @param  second: Second value to set (0 to 59), in decimal format.
  * @retval None
  */
void ds1307_set_second(ds1307_context_t *usr, uint8_t second) {
	second = DecToBcd(second);
	ds1307_i2c_send(usr, DS1307_WRITE_ADR, DS1307_SEC_REG_ADR, &second, 1);
}

/**
  * @brief  Reads the second value from the DS1307 device and updates the context structure.
  * @param  usr: Pointer to the DS1307 context structure where the result will be stored.
  * @retval None
  * @note   The CH (Clock Halt) bit [bit 7] is masked out during read.
  */
void ds1307_get_second(ds1307_context_t *usr) {
	uint8_t second = 0;
	ds1307_i2c_read(usr, DS1307_READ_ADR, DS1307_SEC_REG_ADR, &second, 1);
	second &= ~(1U << 7);
	usr->second = BcdToDec(second);
}


/**
  * @brief  Sets the hour value on the DS1307 device according to the selected hour format (12H/24H).
  * @param  usr: Pointer to the DS1307 context structure.
  * @param  hour: Hour value to set. The value should always be provided in 24-hour format (0–23).
  * @retval None
  * @note   The hour value must be given in 24-hour format regardless of the current format setting.
  *         If the context is set to 12-hour format, the driver automatically converts the value to the correct 12H format with AM/PM.
  *         For example, passing 13 results in 1 PM being set when 12H mode is active.
  *         The selected hour format (12/24) must be set in usr->time_format.
  *         To change the hour format (12H or 24H), use the @ref ds1307_set_time_format function.
  * @see    ds1307_set_time_format
  */
void ds1307_set_hour(ds1307_context_t *usr, uint8_t hour) {

	if (usr->time_format == DS1307_HOUR_FORMAT_12) {

		if (hour > 11) {
			hour -= 12;
			if (hour == 0) {
				hour = 12;
				hour = DecToBcd(hour);
				hour |= (1U << 5);
			} else if (hour == 12) {
				hour = DecToBcd(hour);
				hour &= ~(1U << 5);
			} else {
				hour = DecToBcd(hour);
				hour |= (1U << 5);
			}
		} else if (hour == 0) {
			hour += 12;
		} else {
			hour = DecToBcd(hour);
			hour &= ~(1U << 5);
		}

		hour |= (1U << 6);
		ds1307_i2c_send(usr, DS1307_WRITE_ADR, DS1307_HOUR_REG_ADR, &hour, 1);
	} else {
		hour = DecToBcd(hour);
		ds1307_i2c_send(usr, DS1307_WRITE_ADR, DS1307_HOUR_REG_ADR, &hour, 1);
	}
}

/**
  * @brief  Reads the hour value from the DS1307 device and updates the context structure.
  * @param  usr: Pointer to the DS1307 context structure where the hour, time format, and AM/PM status will be stored.
  * @retval None
  * @note   Updates usr->hour, usr->time_format, and usr->time_period based on the DS1307 hour register content.
  */
void ds1307_get_hour(ds1307_context_t *usr) {
	uint8_t hour = 0;
	ds1307_i2c_read(usr, DS1307_READ_ADR, DS1307_HOUR_REG_ADR, &hour, 1);
	usr->time_format = (ds_1307_hour_format_t) ((hour & 0x40) >> 6);
	hour &= ~(1U << 6);

	if (usr->time_format == DS1307_HOUR_FORMAT_12) {

		usr->time_period = ((hour & 0x20) >> 5);
		hour &= ~(1U << 5);

	} else {
		usr->time_period = DS1307_NONE;
	}
	usr->hour = BcdToDec(hour);
}


/**
  * @brief  Sets the hour format (12-hour or 24-hour) for the DS1307 device.
  * @param  usr: Pointer to the DS1307 context structure.
  * @param  format: Desired hour format. This parameter can be one of the following values:
  *         @arg DS1307_HOUR_FORMAT_12: 12-hour format
  *         @arg DS1307_HOUR_FORMAT_24: 24-hour format
  * @retval None
  * @note   This function reads the current time from the device and converts the existing values
  *         to match the selected hour format. A small time drift (1–2 seconds) may occur due to
  *         read-modify-write operations.
  *         If precise timing is critical, it is recommended to reconfigure the date and time
  *         after using this function.
  * @see    ds1307_set_hour
  */
void ds1307_set_time_format(ds1307_context_t *usr, ds_1307_hour_format_t format) {
	ds1307_get_hour(usr);
	if (usr->time_format != format) {
		ds1307_set_ch(usr, DS1307_CLOCK_DISABLE);
		switch (format) {
		case DS1307_HOUR_FORMAT_12:
			usr->time_format = DS1307_HOUR_FORMAT_12;
			break;
		case DS1307_HOUR_FORMAT_24:
			usr->time_format = DS1307_HOUR_FORMAT_24;
			if (usr->time_period == DS1307_PM) {
				if (usr->hour != 12) {
					usr->hour += 12;
				}
			} else {
				if (usr->hour == 12) {
					usr->hour = 0;
				}
			}

			break;
		default:
			break;
		}
		ds1307_set_year(usr, usr->year);
		ds1307_set_month(usr, usr->month);
		ds1307_set_date(usr, usr->date);
		ds1307_set_day(usr, usr->day);
		ds1307_set_hour(usr, usr->hour);
		ds1307_set_minute(usr, usr->minute);
		ds1307_set_second(usr, usr->second);
	}

}

/**
  * @brief  Sets the day of the month (date) value on the DS1307 device.
  * @param  usr: Pointer to the DS1307 context structure.
  * @param  date: Day of the month to set (1 to 31), in decimal format.
  * @retval None
  */
void ds1307_set_date(ds1307_context_t *usr, uint8_t date) {
	date = DecToBcd(date);
	ds1307_i2c_send(usr, DS1307_WRITE_ADR, DS1307_DATE_REG_ADR, &date, 1);
}

/**
  * @brief  Reads the day of the month (date) value from the DS1307 device and updates the context structure.
  * @param  usr: Pointer to the DS1307 context structure where the result will be stored.
  * @retval None
  */
void ds1307_get_date(ds1307_context_t *usr) {
	uint8_t date = 0;
	ds1307_i2c_read(usr, DS1307_READ_ADR, DS1307_DATE_REG_ADR, &date, 1);
	usr->date = (ds1307_day_t) BcdToDec(date);
}

/**
  * @brief  Sets the day of the week on the DS1307 device.
  * @param  usr: Pointer to the DS1307 context structure.
  * @param  day: Day of the week to set. This parameter can be one of the values defined in @ref ds1307_day_t
  *              (1 = Monday, ..., 7 = Sunday).
  * @retval None
  */
void ds1307_set_day(ds1307_context_t *usr, ds1307_day_t day) {
	uint8_t day_8bit = (uint8_t) DecToBcd(day);
	ds1307_i2c_send(usr, DS1307_WRITE_ADR, DS1307_DAY_REG_ADR, &day_8bit, 1);
}

/**
  * @brief  Reads the day of the week from the DS1307 device and updates the context structure.
  * @param  usr: Pointer to the DS1307 context structure where the result will be stored.
  * @retval None
  * @note   The day value follows the format: 1 = Monday, ..., 7 = Sunday.
  */
void ds1307_get_day(ds1307_context_t *usr) {
	uint8_t day = 0;
	ds1307_i2c_read(usr, DS1307_READ_ADR, DS1307_DAY_REG_ADR, &day, 1);
	usr->day = (ds1307_day_t) BcdToDec(day);
}

/**
  * @brief  Sets the month value on the DS1307 device.
  * @param  usr: Pointer to the DS1307 context structure.
  * @param  month: Month value to set. This parameter can be one of the values defined in @ref ds1307_month_t
  *                (1 = January, ..., 12 = December).
  * @retval None
  */

void ds1307_set_month(ds1307_context_t *usr, ds1307_month_t month) {
	uint8_t month_8bit = (uint8_t) DecToBcd(month);
	ds1307_i2c_send(usr, DS1307_WRITE_ADR, DS1307_MONTH_REG_ADR, &month_8bit,
			1);
}

/**
  * @brief  Reads the month value from the DS1307 device and updates the context structure.
  * @param  usr: Pointer to the DS1307 context structure where the result will be stored.
  * @retval None
  * @note   The month value is interpreted as: 1 = January, ..., 12 = December.
  */
void ds1307_get_month(ds1307_context_t *usr) {
	uint8_t month = 0;
	ds1307_i2c_read(usr, DS1307_READ_ADR, DS1307_MONTH_REG_ADR, &month, 1);
	usr->month = (ds1307_month_t) BcdToDec(month);
}

/**
  * @brief  Sets the year value on the DS1307 device (supports full 4-digit year).
  * @param  usr: Pointer to the DS1307 context structure.
  * @param  year: Full year value to set (e.g., 2024). Only the last two digits are written to the device,
  *               and the century part is stored internally in usr->century.
  * @retval None
  * @note   The DS1307 device stores only the last two digits of the year.
  *         The higher part (century) is calculated and stored separately in usr->century for full year tracking.
  */
void ds1307_set_year(ds1307_context_t *usr, uint16_t year) {
	uint8_t year_8bit = (uint8_t) (year % 100);
	usr->century = year - ((uint16_t) year_8bit);
	year_8bit = DecToBcd(year_8bit);
	ds1307_i2c_send(usr, DS1307_WRITE_ADR, DS1307_YEAR_REG_ADR, &year_8bit, 1);
}

/**
  * @brief  Reads the year value from the DS1307 device and updates the context structure.
  * @param  usr: Pointer to the DS1307 context structure where the result will be stored.
  * @retval None
  * @note   The final full year is reconstructed by combining usr->century and the 2-digit year read from the device.
  */
void ds1307_get_year(ds1307_context_t *usr) {
	uint8_t year = 0;
	ds1307_i2c_read(usr, DS1307_READ_ADR, DS1307_YEAR_REG_ADR, &year, 1);
	usr->year = usr->century + ((uint16_t) BcdToDec(year));

}

/**
  * @brief  Enables or disables the DS1307 oscillator by setting the CH (Clock Halt) bit.
  * @param  usr: Pointer to the DS1307 context structure.
  * @param  clock: Clock state to set. This parameter can be one of the following values:
  *         @arg DS1307_CLOCK_ENABLE:  Enables the oscillator (CH = 0)
  *         @arg DS1307_CLOCK_DISABLE: Disables the oscillator (CH = 1)
  * @retval None
  * @note   Disabling the clock stops the timekeeping functions. This may be used to pause time updates during configuration.
  *         The CH bit is located in bit 7 of the seconds register.
  */
void ds1307_set_ch(ds1307_context_t *usr, ds1307_clock_t clock) {
	uint8_t ch = 0;
	if (clock == DS1307_CLOCK_DISABLE) {
		ch |= (1U << 7);
		ds1307_i2c_send(usr, DS1307_WRITE_ADR, DS1307_SEC_REG_ADR, &ch, 1);
	} else {
		ch &= ~(1U << 7);
		ds1307_i2c_send(usr, DS1307_WRITE_ADR, DS1307_SEC_REG_ADR, &ch, 1);
	}

}

/**
  * @brief  Reads the full date and time information from the DS1307 device and updates the context structure.
  * @param  usr: Pointer to the DS1307 context structure where the date and time values will be stored.
  * @retval None
  * @note   This function sequentially reads year, month, date, day, hour, minute, and second values
  *         from the DS1307 and populates the corresponding fields in the context structure.
  */
void DS1307_read_date_time(ds1307_context_t *usr) {

	ds1307_get_year(usr);
	ds1307_get_month(usr);
	ds1307_get_date(usr);
	ds1307_get_day(usr);
	ds1307_get_hour(usr);
	ds1307_get_minute(usr);
	ds1307_get_second(usr);

}

/**
  * @brief  Converts a BCD (Binary-Coded Decimal) value to decimal.
  * @param  value: 8-bit BCD value to convert.
  * @retval Decimal representation of the input BCD value.
  */
static uint8_t BcdToDec(uint8_t value) {

	return ((((value & 0xf0) >> 4) * 10) + (value & 0x0f));

}

/**
  * @brief  Converts a decimal value to BCD (Binary-Coded Decimal) format.
  * @param  value: 8-bit decimal value to convert (0 to 99).
  * @retval BCD representation of the input decimal value.
  */
static uint8_t DecToBcd(uint8_t value) {

	return (((value / 10) << 4) + (value % 10));

}
