#pragma once
#include <Arduino.h>
#if defined(ESP32)
    #if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
        #define ESP32_NO_GPIO_MUX
        #define ESP32_SPI_IN_INDEX SPI3_Q_IN_IDX
    #else
        #define ESP32_SPI_IN_INDEX VSPIQ_IN_IDX
    #endif
    #define OPTIMIZE_ESP32
    #define OPTIMIZE_DMA
    //#endif
    #define ASSIGNABLE_SPI_PINS
    #define ASSIGNABLE_I2C_PINS
#endif
#if defined(ARDUINO_ARCH_STM32)
    //#define ASSIGNABLE_SPI_PINS
    #define ASSIGNABLE_I2C_PINS
#endif
#if defined(__AVR__)
    #define OPTIMIZE_AVR
#endif
#define FORCE_INLINE __attribute((always_inline))
namespace arduino {
    enum struct tft_io_type {
        spi = 0,
        i2c = 1,
        parallel8 = 2
    };
}
