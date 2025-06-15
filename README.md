# DS1307-Embedded-Driver
Platform-independent C driver for the DS1307 Real-Time Clock (RTC) module with support for time/date read-write operations over I2C.
## Features

- Read and write time values: second, minute, hour
- Support for both 12-hour and 24-hour formats
- Day of week, date, month, and year support
- Century tracking (for full 4-digit year)
- Clock start/stop control (CH bit)
- User-friendly context-based interface
- Pure C implementation, no hardware dependency
---

## Files

- `DS1307.c` – Source file containing all driver logic.
- `DS1307.h` – Header file with enums, structs, and function declarations.
---

## Requirements

- A valid implementation of I2C memory read/write functions matching the following signatures:

```c
typedef void (*ds1307_i2c_mem_write_func_t)(ds1307_adr_t address, ds1307_reg_adr_t reg_adr,  uint8_t *data, uint16_t size);

typedef void (*ds1307_i2c_mem_read_func_t)(ds1307_adr_t address, ds1307_reg_adr_t reg_adr, uint8_t *data, uint16_t size);
```
---

## Context Structure

The `ds1307_context_t` structure is the central data model used by this driver.  
It stores both user-provided I2C function pointers and all real-time clock data fields.

This structure includes:
- I2C function pointers for read/write operations (`ds1307_user_func_t`)
- Time and date values: `second`, `minute`, `hour`, `day`, `date`, `month`, `year`
- Century support for full 4-digit year (`century`)
- Hour format selection: 12-hour or 24-hour (`time_format`)
- AM/PM indicator when 12-hour format is used (`time_period`)

The driver uses this structure to store and retrieve all time-related data.  
Calling any `ds1307_get_*()` or `DS1307_read_date_time()` function updates the relevant fields inside this context.  
Similarly, `ds1307_set_*()` functions write values from this structure to the device.

This design allows full separation between the driver logic and the hardware platform.

---

## Usage Example

### 1. Implement I2C read/write functions

You must provide two platform-specific I2C functions that match the driver's expected function pointer signatures:

```c
void my_i2c_write_function(ds1307_adr_t address, ds1307_reg_adr_t reg_adr, uint8_t *ds1307_data, uint16_t size) {
    HAL_I2C_Mem_Write(&hi2c1, address, reg_adr, I2C_MEMADD_SIZE_8BIT, ds1307_data, size, 100);
}

void my_i2c_read_function(ds1307_adr_t address, ds1307_reg_adr_t reg_adr, uint8_t *ds1307_data, uint16_t size) {
    HAL_I2C_Mem_Read(&hi2c1, address, reg_adr, I2C_MEMADD_SIZE_8BIT, ds1307_data, size, 100);
}
```

⚠️ These functions use STM32 HAL API (HAL_I2C_Mem_Read / HAL_I2C_Mem_Write).
You can adapt them to any platform (e.g., bare-metal, ESP-IDF, nRF SDK) as long as they match the signature.

### 2. Create and configure context

```c
ds1307_context_t ds1307;

ds1307.functions.ds1307_i2c_send_ptr = my_i2c_write_function;
ds1307.functions.ds1307_i2c_read_ptr = my_i2c_read_function;
```

### 3. Set date and time

```c
ds1307_set_year(&ds1307, 2025);
ds1307_set_month(&ds1307, DS1307_JUNE);
ds1307_set_date(&ds1307, 13);
ds1307_set_day(&ds1307, DS1307_FRIDAY);
ds1307_set_hour(&ds1307, 14);
ds1307_set_minute(&ds1307, 30);
ds1307_set_second(&ds1307, 0);
```

### 4. Read time continuously in both 24H and 12H formats

```c
while (1)
{
    // Read each field manually in 24H format
    for (int i = 0; i < 5; i++) {
        ds1307_get_year(&ds1307);
        ds1307_get_month(&ds1307);
        ds1307_get_date(&ds1307);
        ds1307_get_day(&ds1307);
        ds1307_get_hour(&ds1307);
        ds1307_get_minute(&ds1307);
        ds1307_get_second(&ds1307);
        HAL_Delay(1000);
    }

    // Switch to 12-hour format
    ds1307_set_time_format(&ds1307, DS1307_HOUR_FORMAT_12);

    // Use bulk read in 12H format
    for (int i = 0; i < 5; i++) {
        DS1307_read_date_time(&ds1307);
        HAL_Delay(1000);
    }

    // Switch back to 24-hour format
    ds1307_set_time_format(&ds1307, DS1307_HOUR_FORMAT_24);
}

```

---
## Contributing

Feel free to contribute with issues or pull requests.

---
## License

This driver is provided as-is, without warranty of any kind.  
You are free to use, modify, and integrate it into your own projects.

---
## Author

Created by **iek2443**  
June 13, 2025

---
