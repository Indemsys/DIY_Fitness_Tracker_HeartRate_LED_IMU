#ifndef S7V30_ISM330_H
  #define S7V30_ISM330_H

  #define ISM330_CHIP_ID                              0x6A


  #define ISM330_FUNC_CFG_ACCESS                      0x01
  #define ISM330_SENSOR_SYNC_TIME_FRAME               0x04
  #define ISM330_SENSOR_SYNC_RES_RATIO                0x05
  #define ISM330_FIFO_CTRL1                           0x06
  #define ISM330_FIFO_CTRL2                           0x07
  #define ISM330_FIFO_CTRL3                           0x08
  #define ISM330_FIFO_CTRL4                           0x09
  #define ISM330_FIFO_CTRL5                           0x0A
  #define ISM330_DRDY_PULSE_CFG                       0x0B
  #define ISM330_INT1_CTRL                            0x0D
  #define ISM330_INT2_CTRL                            0x0E
  #define ISM330_WHO_AM_I                             0x0F
  #define ISM330_CTRL1_XL                             0x10
// CTRL1_XL (0x10), значение после сброса: 0x00 (биты: 0000_0000)
// CTRL1_XL = 0
//  + LSHIFT(0, 7) // ODR_XL3        | Output Data Rate (ODR) [3].
//                 // 0000: Power-down
//                 // 0001: 1.6 Hz (low power) / 12.5 Hz (high performance)
//                 // 0010: 12.5 Hz
//                 // 0011: 26 Hz
//                 // 0100: 52 Hz
//                 // 0101: 104 Hz
//                 // 0110: 208 Hz
//                 // 0111: 416 Hz
//                 // 1000: 833 Hz
//                 // 1001: 1.66 kHz
//                 // 1010: 3.33 kHz
//                 // 1011: 6.66 kHz
//                 // 11xx: Not allowed
//  + LSHIFT(0, 6) // ODR_XL2        | Output Data Rate (ODR) [2]. См. описание ODR_XL3
//  + LSHIFT(0, 5) // ODR_XL1        | Output Data Rate (ODR) [1]. См. описание ODR_XL3
//  + LSHIFT(0, 4) // ODR_XL0        | Output Data Rate (ODR) [0]. См. описание ODR_XL3
//  + LSHIFT(0, 3) // FS_XL1         | Full-Scale Selection (FS_XL) [1].
//                 // 00: ±2 g
//                 // 01: ±16 g
//                 // 10: ±4 g
//                 // 11: ±8 g
//  + LSHIFT(0, 2) // FS_XL0         | Full-Scale Selection (FS_XL) [0]. См. описание FS_XL1
//  + LSHIFT(0, 1) // LPF1_BW_SEL    | Digital LPF1 bandwidth selection.
//                 // 0: LPF1 bandwidth selection via CTRL8_XL
//                 // 1: LPF1 bandwidth selected internally
//  + LSHIFT(0, 0) // BW0_XL         | Analog chain bandwidth selection.
//                 // 0: Bandwidth = 1.5 kHz
//                 // 1: Bandwidth = 400 Hz
//  ;

  #define ISM330_CTRL2_G                              0x11
// CTRL2_G (0x11), значение после сброса: 0x00 (биты: 0000_0000)
// CTRL2_G = 0
//   + LSHIFT(0, 7) // ODR_G3         | Output Data Rate (ODR) for gyroscope [3].
//                  // 0000: Power down
//                  // 0001: 12.5 Hz (low power) / 12.5 Hz (high performance)
//                  // 0010: 26 Hz
//                  // 0011: 52 Hz
//                  // 0100: 104 Hz
//                  // 0101: 208 Hz
//                  // 0110: 416 Hz
//                  // 0111: 833 Hz
//                  // 1000: 1.66 kHz
//                  // 1001: 3.33 kHz
//                  // 1010: 6.66 kHz
//                  // 11xx: Not available
//   + LSHIFT(0, 6) // ODR_G2         | Output Data Rate (ODR) for gyroscope [2]. См. описание ODR_G3
//   + LSHIFT(0, 5) // ODR_G1         | Output Data Rate (ODR) for gyroscope [1]. См. описание ODR_G3
//   + LSHIFT(0, 4) // ODR_G0         | Output Data Rate (ODR) for gyroscope [0]. См. описание ODR_G3
//   + LSHIFT(0, 3) // FS_G1          | Full-scale selection for gyroscope [1].
//                  // 00: ±250 dps
//                  // 01: ±500 dps
//                  // 10: ±1000 dps
//                  // 11: ±2000 dps
//   + LSHIFT(0, 2) // FS_G0          | Full-scale selection for gyroscope [0]. См. описание FS_G1
//   + LSHIFT(0, 1) // FS_125         | Gyroscope full-scale at ±125 dps.
//                  // 0: Disabled
//                  // 1: Enabled
//   + LSHIFT(0, 0) // 0              | Зарезервировано. Должен быть установлен в 0
//  ;

  #define ISM330_CTRL3_C                              0x12
  #define ISM330_CTRL4_C                              0x13
  #define ISM330_CTRL5_C                              0x14
  #define ISM330_CTRL6_C                              0x15
// CTRL6_C (0x15), значение после сброса: 0x00 (биты: 0000_0000)
// CTRL6_C = 0
//        + LSHIFT(0, 7) // TRIG_EN      | DEN edge-sensitive trigger enable.
//                       // 0: Edge-sensitive trigger mode отключён
//                       // 1: Edge-sensitive trigger mode включён
//        + LSHIFT(0, 6) // LVL1_EN      | DEN data level-sensitive trigger enable.
//                       // 0: Level-sensitive trigger mode отключён
//                       // 1: Level-sensitive trigger mode включён
//        + LSHIFT(0, 5) // LVL2_EN      | DEN level-sensitive latched enable.
//                       // 0: Level-sensitive latched mode отключён
//                       // 1: Level-sensitive latched mode включён
//        + LSHIFT(1, 4) // XL_HM_MODE   | High-performance operating mode for accelerometer.
//                       // 0: High-performance mode включён
//                       // 1: High-performance mode отключён
//        + LSHIFT(0, 3) // USR_OFF_W    | User offset weight selection.
//                       // 0: Вес 2^10 g/LSB
//                       // 1: Вес 2^6 g/LSB
//        + LSHIFT(0, 2) // 0            | Зарезервирован. Должен быть установлен в 0
//        + LSHIFT(0, 1) // FTYPE1       | Gyroscope LPF1 bandwidth selection [1].
//                       // 00: 245 Hz @ 800 Hz ODR
//                       // 01: 195 Hz @ 800 Hz ODR
//                       // 10: 155 Hz @ 800 Hz ODR
//                       // 11: 293 Hz @ 800 Hz ODR
//        + LSHIFT(0, 0) // FTYPE0       | Gyroscope LPF1 bandwidth selection [0]. См. описание FTYPE1
// ;

  #define ISM330_CTRL7_G                              0x16
// CTRL7_G (0x16), значение после сброса: 0x00 (биты: 0000_0000)
// CTRL7_G = 0
//    + LSHIFT(0, 7) // G_HM_MODE       | High-performance mode for gyroscope.
//                   // 0: High-performance mode enabled
//                   // 1: High-performance mode disabled
//    + LSHIFT(0, 6) // HP_EN_G         | Gyroscope digital high-pass filter enable.
//                   // 0: HPF disabled
//                   // 1: HPF enabled
//    + LSHIFT(0, 5) // HPM1_G          | Gyroscope high-pass filter cutoff selection [1].
//                   // 00: 16 mHz
//                   // 01: 65 mHz
//                   // 10: 260 mHz
//                   // 11: 1.04 Hz
//    + LSHIFT(0, 4) // HPM0_G          | Gyroscope high-pass filter cutoff selection [0]. См. описание HPM1_G
//    + LSHIFT(0, 3) // 0               | Зарезервировано. Должен быть установлен в 0
//    + LSHIFT(0, 2) // ROUNDING_STATUS | Source register rounding function on WAKE_UP_SRC (1Bh), TAP_SRC (1Ch), D6D_SRC (1Dh), STATUS_REG (1Eh), and FUNC_SRC1 (53h)
//                   // 0: Rounding disabled
//                   // 1: Rounding enabled
//    + LSHIFT(0, 1) // 0               | Зарезервировано. Должен быть установлен в 0
//    + LSHIFT(0, 0) // 0               | Зарезервировано. Должен быть установлен в 0
//;

  #define ISM330_CTRL8_XL                             0x17
  #define ISM330_CTRL9_XL                             0x18
  #define ISM330_CTRL10_C                             0x19
// CTRL10_C (0x19), значение после сброса: 0x00 (биты: 0000_0000)
// CTRL10_C = 0
//     + LSHIFT(0, 7)    // RESERVED           | Reserved. Must be set to 0.
//     + LSHIFT(0, 6)    // RESERVED           | Reserved. Must be set to 0.
//     + LSHIFT(0, 5)    // TIMER_EN           | Enable timestamp count.
//                       // 0: Timestamp count disabled.
//                       // 1: Timestamp count enabled.
//     + LSHIFT(0, 4)    // RESERVED           | Reserved. Must be set to 0.
//     + LSHIFT(0, 3)    // TILT_EN            | Enable tilt calculation.
//                       // 0: Tilt calculation disabled.
//                       // 1: Tilt calculation enabled.
//     + LSHIFT(0, 2)    // FUNC_EN            | Enable embedded functionalities.
//                       // 0: Disable embedded functions and accelerometer filters.
//                       // 1: Enable embedded functions and accelerometer filters.
//     + LSHIFT(0, 1)    // RESERVED           | Reserved. Must be set to 0.
//     + LSHIFT(0, 0)    // RESERVED           | Reserved. Must be set to 0.
// ;

  #define ISM330_MASTER_CONFIG                        0x1A
  #define ISM330_WAKE_UP_SRC                          0x1B
  #define ISM330_TAP_SRC                              0x1C
  #define ISM330_D6D_SRC                              0x1D
  #define ISM330_STATUS_REG                           0x1E
  #define ISM330_OUT_TEMP_L                           0x20
  #define ISM330_OUT_TEMP_H                           0x21
  #define ISM330_OUTX_L_G                             0x22
  #define ISM330_OUTX_H_G                             0x23
  #define ISM330_OUTY_L_G                             0x24
  #define ISM330_OUTY_H_G                             0x25
  #define ISM330_OUTZ_L_G                             0x26
  #define ISM330_OUTZ_H_G                             0x27
  #define ISM330_OUTX_L_XL                            0x28
  #define ISM330_OUTX_H_XL                            0x29
  #define ISM330_OUTY_L_XL                            0x2A
  #define ISM330_OUTY_H_XL                            0x2B
  #define ISM330_OUTZ_L_XL                            0x2C
  #define ISM330_OUTZ_H_XL                            0x2D
  #define ISM330_SENSORHUB1_REG                       0x2E
  #define ISM330_SENSORHUB2_REG                       0x2F
  #define ISM330_SENSORHUB3_REG                       0x30
  #define ISM330_SENSORHUB4_REG                       0x31
  #define ISM330_SENSORHUB5_REG                       0x32
  #define ISM330_SENSORHUB6_REG                       0x33
  #define ISM330_SENSORHUB7_REG                       0x34
  #define ISM330_SENSORHUB8_REG                       0x35
  #define ISM330_SENSORHUB9_REG                       0x36
  #define ISM330_SENSORHUB10_REG                      0x37
  #define ISM330_SENSORHUB11_REG                      0x38
  #define ISM330_SENSORHUB12_REG                      0x39
  #define ISM330_FIFO_STATUS1                         0x3A
  #define ISM330_FIFO_STATUS2                         0x3B
  #define ISM330_FIFO_STATUS3                         0x3C
  #define ISM330_FIFO_STATUS4                         0x3D
  #define ISM330_FIFO_DATA_OUT_L                      0x3E
  #define ISM330_FIFO_DATA_OUT_H                      0x3F
  #define ISM330_TIMESTAMP0_REG                       0x40
  #define ISM330_TIMESTAMP1_REG                       0x41
  #define ISM330_TIMESTAMP2_REG                       0x42
  #define ISM330_SENSORHUB13_REG                      0x4D
  #define ISM330_SENSORHUB14_REG                      0x4E
  #define ISM330_SENSORHUB15_REG                      0x4F
  #define ISM330_SENSORHUB16_REG                      0x50
  #define ISM330_SENSORHUB17_REG                      0x51
  #define ISM330_SENSORHUB18_REG                      0x52
  #define ISM330_FUNC_SRC1                            0x53
  #define ISM330_FUNC_SRC2                            0x54

  #define ISM330_TAP_CFG                              0x58
// TAP_CFG (0x58), значение после сброса: 0x00 (биты: 0000_0000)
// TAP_CFG = 0
//        + LSHIFT(1, 7) // INTERRUPTS_ENABLE | Enable basic interrupts (6D/4D, free-fall, wake-up, tap, inactivity).
//                       // 0: Interrupt disabled
//                       // 1: Interrupt enabled
//        + LSHIFT(0, 6) // INACT_EN1         | Inactivity function enable [1].
//                       // 00: Disabled
//                       // 01: ODR = 12.5 Hz (low-power), gyro unchanged
//                       // 10: ODR = 12.5 Hz (low-power), gyro in sleep mode
//                       // 11: ODR = 12.5 Hz (low-power), gyro in power-down mode
//        + LSHIFT(0, 5) // INACT_EN0         | Inactivity function enable [0]. См. описание INACT_EN1
//        + LSHIFT(0, 4) // SLOPE_FDS         | HPF or SLOPE filter selection on wake-up and activity/inactivity functions.
//                       // 0: SLOPE filter applied
//                       // 1: HPF applied
//        + LSHIFT(1, 3) // TAP_X_EN          | Enable X direction in tap recognition.
//                       // 0: X direction disabled
//                       // 1: X direction enabled
//        + LSHIFT(1, 2) // TAP_Y_EN          | Enable Y direction in tap recognition.
//                       // 0: Y direction disabled
//                       // 1: Y direction enabled
//        + LSHIFT(1, 1) // TAP_Z_EN          | Enable Z direction in tap recognition.
//                       // 0: Z direction disabled
//                       // 1: Z direction enabled
//        + LSHIFT(0, 0) // LIR               | Latched Interrupt Request.
//                       // 0: Interrupt request not latched
//                       // 1: Interrupt request latched
// ;

  #define ISM330_TAP_THS_6D                           0x59
// TAP_THS_6D (0x59), значение после сброса: 0x00 (биты: 0000_0000)
// TAP_THS_6D = 0
//        + LSHIFT(0, 7) // D4D_EN        | 4D orientation detection enable.
//                       // 0: 4D detection enabled
//                       // 1: 4D detection disabled (Z-axis position detection enabled)
//        + LSHIFT(0, 6) // SIXD_THS1     | Threshold for 4D/6D detection function [1].
//                       // 00: 80 degrees
//                       // 01: 70 degrees
//                       // 10: 60 degrees
//                       // 11: 50 degrees
//        + LSHIFT(0, 5) // SIXD_THS0     | Threshold for 4D/6D detection function [0]. См. описание SIXD_THS1
//        + LSHIFT(0, 4) // TAP_THS4      | Threshold for tap recognition [4].
//                       // Значение TAP_THS[4:0]: 1 LSB = FS_XL / 2^5
//        + LSHIFT(0, 3) // TAP_THS3      | Threshold for tap recognition [3]. См. описание TAP_THS4
//        + LSHIFT(0, 2) // TAP_THS2      | Threshold for tap recognition [2]. См. описание TAP_THS4
//        + LSHIFT(0, 1) // TAP_THS1      | Threshold for tap recognition [1]. См. описание TAP_THS4
//        + LSHIFT(1, 0) // TAP_THS0      | Threshold for tap recognition [0]. См. описание TAP_THS4
// ;

  #define ISM330_INT_DUR2                             0x5A
// INT_DUR2 (0x5A), значение после сброса: 0x00 (биты: 0000_0000)
// INT_DUR2 = 0
//        + LSHIFT(0, 7) // DUR3           | Maximum time gap for double tap recognition [3].
//                       // Значение DUR[3:0]: 1 LSB = 32 * ODR_XL time
//        + LSHIFT(0, 6) // DUR2           | Maximum time gap for double tap recognition [2]. См. описание DUR3
//        + LSHIFT(0, 5) // DUR1           | Maximum time gap for double tap recognition [1]. См. описание DUR3
//        + LSHIFT(0, 4) // DUR0           | Maximum time gap for double tap recognition [0]. См. описание DUR3
//        + LSHIFT(1, 3) // QUIET1         | Expected quiet time after a tap detection [1].
//                       // Значение QUIET[1:0]: 1 LSB = 4 * ODR_XL time
//                       // 00: Quiet time = 2 * ODR_XL time
//                       // 01: Quiet time = 4 * ODR_XL time
//                       // 10: Quiet time = 8 * ODR_XL time
//                       // 11: Quiet time = 12 * ODR_XL time
//        + LSHIFT(1, 2) // QUIET0         | Expected quiet time after a tap detection [0]. См. описание QUIET1
//        + LSHIFT(1, 1) // SHOCK1         | Maximum duration of overthreshold event [1].
//                       // Значение SHOCK[1:0]: 1 LSB = 8 * ODR_XL time
//                       // 00: Shock duration = 4 * ODR_XL time
//                       // 01: Shock duration = 8 * ODR_XL time
//                       // 10: Shock duration = 16 * ODR_XL time
//                       // 11: Shock duration = 24 * ODR_XL time
//        + LSHIFT(1, 0) // SHOCK0         | Maximum duration of overthreshold event [0]. См. описание SHOCK1
// ;

  #define ISM330_WAKE_UP_THS                          0x5B
// WAKE_UP_THS (0x5B), значение после сброса: 0x00 (биты: 0000_0000)
// WAKE_UP_THS = 0
//     + LSHIFT(0, 7)    // SINGLE_DOUBLE_TAP  | Single/double-tap event enable.
//                       // 0: Only single-tap events enabled.
//                       // 1: Both single-tap and double-tap events enabled.
//     + LSHIFT(0, 6)    // RESERVED           | Reserved. Must be set to 0.
//     + LSHIFT(0x00, 0) // WK_THS[5:0]        | Threshold for wakeup.
//                       // Value range: 0x00–0x3F.
//                       // 1 LSB corresponds to FS_XL / 2^6.
// ;


  #define ISM330_WAKE_UP_DUR                          0x5C
// WAKE_UP_DUR (0x5C), значение после сброса: 0x00 (биты: 0000_0000)
// WAKE_UP_DUR = 0
//     + LSHIFT(0, 7)    // FF_DUR5           | Free-fall duration extension.
//                       // 0: Duration not extended.
//                       // 1: Duration extended.
//     + LSHIFT(0x00, 5) // WAKE_DUR[1:0]     | Wake-up duration event.
//                       // 1 LSB = 1 ODR_time.
//                       // Default: 00.
//     + LSHIFT(0, 4)    // TIMER_HR          | Timestamp register resolution setting.
//                       // 0: 1 LSB = 6.4 ms.
//                       // 1: 1 LSB = 25 μs.
//     + LSHIFT(0x00, 0) // SLEEP_DUR[3:0]    | Sleep mode duration.
//                       // 1 LSB = 512 ODR_time.
//                       // Default: 0000 (16 ODR).
// ;
  #define ISM330_FREE_FALL                            0x5D
// FREE_FALL (0x5D), значение после сброса: 0x00 (биты: 0000_0000)
// FREE_FALL = 0
//     + LSHIFT(0x00, 3) // FF_DUR[4:0]       | Free-fall duration event.
//                       // Value range: 0x00–0x1F.
//                       // For configuration, refer to FF_DUR5 in WAKE_UP_DUR (0x5C).
//     + LSHIFT(0x00, 0) // FF_THS[2:0]       | Free-fall threshold setting.
//                       // 000: 156 mg.
//                       // 001: 219 mg.
//                       // 010: 250 mg.
//                       // 011: 312 mg.
//                       // 100: 344 mg.
//                       // 101: 406 mg.
//                       // 110: 469 mg.
//                       // 111: 500 mg.
// ;

  #define ISM330_MD1_CFG                              0x5E
  #define ISM330_MD2_CFG                              0x5F
// MD2_CFG (0x5F), значение после сброса: 0x00 (биты: 0000_0000)
// MD2_CFG = 0
//        + LSHIFT(0, 7) // INT2_INACT_STATE | Routing of inactivity event on INT2.
//                       // 0: Inactivity event routing disabled
//                       // 1: Inactivity event routing enabled
//        + LSHIFT(1, 6) // INT2_SINGLE_TAP  | Routing of single-tap event on INT2.
//                       // 0: Single-tap event routing disabled
//                       // 1: Single-tap event routing enabled
//        + LSHIFT(0, 5) // INT2_WU          | Routing of wake-up event on INT2.
//                       // 0: Wake-up event routing disabled
//                       // 1: Wake-up event routing enabled
//        + LSHIFT(0, 4) // INT2_FF          | Routing of free-fall event on INT2.
//                       // 0: Free-fall event routing disabled
//                       // 1: Free-fall event routing enabled
//        + LSHIFT(0, 3) // INT2_DOUBLE_TAP  | Routing of double-tap event on INT2.
//                       // 0: Double-tap event routing disabled
//                       // 1: Double-tap event routing enabled
//        + LSHIFT(0, 2) // INT2_6D          | Routing of 6D orientation event on INT2.
//                       // 0: 6D orientation event routing disabled
//                       // 1: 6D orientation event routing enabled
//        + LSHIFT(0, 1) // INT2_TILT        | Routing of tilt event on INT2.
//                       // 0: Tilt event routing disabled
//                       // 1: Tilt event routing enabled
//        + LSHIFT(0, 0) // INT2_IRON        | Routing of soft-iron/hard-iron algorithm end event on INT2.
//                       // 0: Soft-iron/hard-iron algorithm event routing disabled
//                       // 1: Soft-iron/hard-iron algorithm event routing enabled
// ;

  #define ISM330_MASTER_CMD_CODE                      0x60
  #define ISM330_SENS_SYNC_SPI_ERROR_CODE             0x61
  #define ISM330_OUT_MAG_RAW_X_L                      0x66
  #define ISM330_OUT_MAG_RAW_X_H                      0x67
  #define ISM330_OUT_MAG_RAW_Y_L                      0x68
  #define ISM330_OUT_MAG_RAW_Y_H                      0x69
  #define ISM330_OUT_MAG_RAW_Z_L                      0x6A
  #define ISM330_OUT_MAG_RAW_Z_H                      0x6B
  #define ISM330_INT_OIS                              0x6F
  #define ISM330_CTRL1_OIS                            0x70
  #define ISM330_CTRL2_OIS                            0x71
  #define ISM330_CTRL3_OIS                            0x72
  #define ISM330_X_OFS_USR                            0x73
  #define ISM330_Y_OFS_USR                            0x74
  #define ISM330_Z_OFS_USR                            0x75


// Маски для регистра CTRL1_XL (0x10)

// ODR_XL (Output Data Rate) [3:0]
  #define CTRL1_XL_ODR_XL_POWER_DOWN  (0x00)  // Power-down
  #define CTRL1_XL_ODR_XL_1_6HZ       (0xD0)  // 1.6 Hz (low power) / 12.5 Hz (high performance)
  #define CTRL1_XL_ODR_XL_12_5HZ      (0x10)  // 12.5 Hz
  #define CTRL1_XL_ODR_XL_26HZ        (0x20)  // 26 Hz
  #define CTRL1_XL_ODR_XL_52HZ        (0x30)  // 52 Hz
  #define CTRL1_XL_ODR_XL_104HZ       (0x40)  // 104 Hz
  #define CTRL1_XL_ODR_XL_208HZ       (0x50)  // 208 Hz
  #define CTRL1_XL_ODR_XL_416HZ       (0x60)  // 416 Hz
  #define CTRL1_XL_ODR_XL_833HZ       (0x70)  // 833 Hz
  #define CTRL1_XL_ODR_XL_1_66KHZ     (0x80)  // 1.66 kHz
  #define CTRL1_XL_ODR_XL_3_33KHZ     (0x90)  // 3.33 kHz
  #define CTRL1_XL_ODR_XL_6_66KHZ     (0xA0)  // 6.66 kHz

// FS_XL (Full-Scale Selection) [1:0]
  #define CTRL1_XL_FS_XL_2G           (0x00)  // ±2 g
  #define CTRL1_XL_FS_XL_16G          (0x04)  // ±16 g
  #define CTRL1_XL_FS_XL_4G           (0x08)  // ±4 g
  #define CTRL1_XL_FS_XL_8G           (0x0С)  // ±8 g

// LPF1_BW_SEL (LPF1 Bandwidth Selection) [1 bit]
  #define CTRL1_XL_LPF1_BW_CTRL8      (0x00)  // Полоса пропускания задаётся CTRL8_XL
  #define CTRL1_XL_LPF1_BW_INTERNAL   (0x02)  // Полоса пропускания фиксирована

// BW0_XL (Analog Bandwidth Selection) [1 bit]
  #define CTRL1_XL_BW0_XL_1_5KHZ      (0x00)  // Bandwidth = 1.5 kHz
  #define CTRL1_XL_BW0_XL_400HZ       (0x01)  // Bandwidth = 400 Hz


// Маски для регистра CTRL2_G (0x11)

// ODR_G (Output Data Rate) [3:0]
  #define CTRL2_G_ODR_G_POWER_DOWN   (0x00)  // Power-down
  #define CTRL2_G_ODR_G_12_5HZ       (0x10)  // 12.5 Hz (low power / high performance)
  #define CTRL2_G_ODR_G_26HZ         (0x20)  // 26 Hz
  #define CTRL2_G_ODR_G_52HZ         (0x30)  // 52 Hz
  #define CTRL2_G_ODR_G_104HZ        (0x40)  // 104 Hz
  #define CTRL2_G_ODR_G_208HZ        (0x50)  // 208 Hz
  #define CTRL2_G_ODR_G_416HZ        (0x60)  // 416 Hz
  #define CTRL2_G_ODR_G_833HZ        (0x70)  // 833 Hz
  #define CTRL2_G_ODR_G_1_66KHZ      (0x80)  // 1.66 kHz
  #define CTRL2_G_ODR_G_3_33KHZ      (0x90)  // 3.33 kHz
  #define CTRL2_G_ODR_G_6_66KHZ      (0xA0)  // 6.66 kHz

// FS_G (Full-Scale Selection) [1:0]
  #define CTRL2_G_FS_G_250DPS        (0x00)  // ±250 dps
  #define CTRL2_G_FS_G_500DPS        (0x04)  // ±500 dps
  #define CTRL2_G_FS_G_1000DPS       (0x08)  // ±1000 dps
  #define CTRL2_G_FS_G_2000DPS       (0x0C)  // ±2000 dps

// FS_125 (Gyroscope Full-Scale at ±125 dps) [1 bit]
  #define CTRL2_G_FS_125_DISABLED    (0x00)  // ±125 dps disabled
  #define CTRL2_G_FS_125_ENABLED     (0x02)  // ±125 dps enabled

// Зарезервированный бит [0]
  #define CTRL2_G_RESERVED_BIT       (0x00)  // Всегда 0


// Макросы для активных значений битов регистра MD2_CFG (0x5F)

// INT2_INACT_STATE: Включение маршрутизации события неактивности
  #define MD2_CFG_INT2_INACT_STATE_ENABLED    (0x80)  // 1: Routing of inactivity event enabled

// INT2_SINGLE_TAP: Включение маршрутизации одиночного касания
  #define MD2_CFG_INT2_SINGLE_TAP_ENABLED     (0x40)  // 1: Routing of single-tap event enabled

// INT2_WU: Включение маршрутизации события пробуждения
  #define MD2_CFG_INT2_WU_ENABLED             (0x20)  // 1: Routing of wake-up event enabled

// INT2_FF: Включение маршрутизации события свободного падения
  #define MD2_CFG_INT2_FF_ENABLED             (0x10)  // 1: Routing of free-fall event enabled

// INT2_DOUBLE_TAP: Включение маршрутизации двойного касания
  #define MD2_CFG_INT2_DOUBLE_TAP_ENABLED     (0x08)  // 1: Routing of double-tap event enabled

// INT2_6D: Включение маршрутизации события 6D ориентации
  #define MD2_CFG_INT2_6D_ENABLED             (0x04)  // 1: Routing of 6D orientation event enabled

// INT2_TILT: Включение маршрутизации события наклона
  #define MD2_CFG_INT2_TILT_ENABLED           (0x02)  // 1: Routing of tilt event enabled

// INT2_IRON: Включение маршрутизации soft-iron/hard-iron алгоритма
  #define MD2_CFG_INT2_IRON_ENABLED           (0x01)  // 1: Routing of soft-iron/hard-iron algorithm event enabled

// WAKE_UP_SRC (0x1B) — Wake-up interrupt source register
typedef struct {
    uint8_t Z_WU          : 1; // Wake-up event detection status on Z-axis.
                               // 0: No wake-up event on Z-axis.
                               // 1: Wake-up event detected on Z-axis.
    uint8_t Y_WU          : 1; // Wake-up event detection status on Y-axis.
                               // 0: No wake-up event on Y-axis.
                               // 1: Wake-up event detected on Y-axis.
    uint8_t X_WU          : 1; // Wake-up event detection status on X-axis.
                               // 0: No wake-up event on X-axis.
                               // 1: Wake-up event detected on X-axis.
    uint8_t WU_IA         : 1; // Wake-up event detection status.
                               // 0: No wake-up event detected.
                               // 1: Wake-up event detected.
    uint8_t SLEEP_STATE_IA: 1; // Sleep event detection status.
                               // 0: No sleep event detected.
                               // 1: Sleep event detected.
    uint8_t FF_IA         : 1; // Free-fall event detection status.
                               // 0: No free-fall event detected.
                               // 1: Free-fall event detected.
    uint8_t RESERVED1     : 1; // Reserved. Must be set to 0.
    uint8_t RESERVED2     : 1; // Reserved. Must be set to 0.
} WAKE_UP_SRC_t;

// TAP_SRC (0x1C) — Tap source register
typedef struct {
    uint8_t Z_TAP      : 1; // Tap event detection status on Z-axis.
                            // 0: No tap event on Z-axis.
                            // 1: Tap event detected on Z-axis.
    uint8_t Y_TAP      : 1; // Tap event detection status on Y-axis.
                            // 0: No tap event on Y-axis.
                            // 1: Tap event detected on Y-axis.
    uint8_t X_TAP      : 1; // Tap event detection status on X-axis.
                            // 0: No tap event on X-axis.
                            // 1: Tap event detected on X-axis.
    uint8_t TAP_SIGN   : 1; // Sign of acceleration detected by tap event.
                            // 0: Positive sign of acceleration.
                            // 1: Negative sign of acceleration.
    uint8_t DOUBLE_TAP : 1; // Double-tap event detection status.
                            // 0: No double-tap event detected.
                            // 1: Double-tap event detected.
    uint8_t SINGLE_TAP : 1; // Single-tap event detection status.
                            // 0: No single-tap event detected.
                            // 1: Single-tap event detected.
    uint8_t TAP_IA     : 1; // Tap event detection status.
                            // 0: No tap event detected.
                            // 1: Tap event detected.
    uint8_t RESERVED   : 1; // Reserved. Must be set to 0.
} TAP_SRC_t;

// D6D_SRC (0x1D) — 6D orientation source register
typedef struct {
    uint8_t XL        : 1; // X-axis low event (under threshold).
                           // 0: Event not detected.
                           // 1: Event detected (under threshold).
    uint8_t XH        : 1; // X-axis high event (over threshold).
                           // 0: Event not detected.
                           // 1: Event detected (over threshold).
    uint8_t YL        : 1; // Y-axis low event (under threshold).
                           // 0: Event not detected.
                           // 1: Event detected (under threshold).
    uint8_t YH        : 1; // Y-axis high event (over threshold).
                           // 0: Event not detected.
                           // 1: Event detected (over threshold).
    uint8_t ZL        : 1; // Z-axis low event (under threshold).
                           // 0: Event not detected.
                           // 1: Event detected (under threshold).
    uint8_t ZH        : 1; // Z-axis high event (over threshold).
                           // 0: Event not detected.
                           // 1: Event detected (over threshold).
    uint8_t D6D_IA    : 1; // Interrupt active for position change (portrait, landscape, face-up, face-down).
                           // 0: No change detected.
                           // 1: Change detected.
    uint8_t DEN_DRDY  : 1; // DEN data-ready signal.
                           // 0: Data not ready.
                           // 1: Data ready.
} D6D_SRC_t;

// STATUS_REG (0x1E) — Status register
typedef struct {
    uint8_t XLDA       : 1; // Accelerometer new data available.
                            // 0: No new data available at accelerometer output.
                            // 1: New data available at accelerometer output.
    uint8_t GDA        : 1; // Gyroscope new data available.
                            // 0: No new data available at gyroscope output.
                            // 1: New data available at gyroscope output.
    uint8_t TDA        : 1; // Temperature sensor new data available.
                            // 0: No new data available at temperature sensor output.
                            // 1: New data available at temperature sensor output.
    uint8_t RESERVED   : 5; // Reserved. Must be set to 0.
} STATUS_REG_t;

// FUNC_SRC1 (0x53) — Tilt, hard/soft-iron, and sensor hub interrupt source register
typedef struct {
    uint8_t SENSORHUB_END_OP : 1; // Sensor hub communication status.
                                  // 0: Sensor hub communication not concluded.
                                  // 1: Sensor hub communication concluded.
    uint8_t SI_END_OP        : 1; // Hard/soft-iron calculation status.
                                  // 0: Hard/soft-iron calculation not concluded.
                                  // 1: Hard/soft-iron calculation concluded.
    uint8_t HI_FAIL          : 1; // Fail in hard/soft-iron algorithm.
                                  // 0: No fail in hard/soft-iron algorithm.
                                  // 1: Fail occurred in hard/soft-iron algorithm.
    uint8_t TILT_IA          : 1; // Tilt event detection status.
                                  // 0: Tilt event not detected.
                                  // 1: Tilt event detected.
    uint8_t RESERVED         : 4; // Reserved. Must be set to 0.
} FUNC_SRC1_t;


typedef struct
{
    uint16_t      reg_addr;
    char  const  *reg_name;
    uint8_t      *reg_val;

} T_ISM330_reg_descr;


typedef __packed struct
{
  WAKE_UP_SRC_t wake_up_src;
  TAP_SRC_t     tap_src;
  D6D_SRC_t     d6d_src;
  STATUS_REG_t  status_reg;
  FUNC_SRC1_t   func_src1;

} T_ism330_src_regs;



extern const sf_spi_instance_t g_sf_spi_ISM330;
extern T_ism330_src_regs       g_ism330_src_regs;
extern T_ism330_src_regs       g_ism330_src_regs_prev;
extern T_ism330_src_regs       g_ism330_src_regs_hold;
extern float                   g_accel_scale_factor;
extern float                   g_gyro_scale_factor;


uint32_t   ISM330_reg_count(void);
T_ISM330_reg_descr const* ISM330_get_reg_descr(uint8_t indx);
ssp_err_t  ISM330_open(void);
ssp_err_t  ISM330_close(void);
ssp_err_t  ISM330_write_buf(uint8_t addr, uint8_t *buf, uint32_t sz);
ssp_err_t  ISM330_read_buf(uint8_t addr, uint8_t *buf, uint32_t sz);
ssp_err_t  ISM330_read_register(uint8_t addr, uint8_t *val);
ssp_err_t  ISM330_write_register(uint8_t addr, uint8_t val);
ssp_err_t  ISM330_lock(uint32_t timeout);
ssp_err_t  ISM330_unlock(void);

void       ISM330_set_sample_rate(uint8_t rate);
float      ISM330_get_sample_rate(void);
ssp_err_t  ISM330_Set_accelerometer_scale_rate(uint8_t scale, uint8_t rate);
ssp_err_t  ISM330_Set_gyro_scale_rate(uint8_t scale, uint8_t rate);
ssp_err_t  ISM330_enable_source_register_rounding(void);
ssp_err_t  ISM330_enable_low_power_accelerometer(void);

ssp_err_t  ISM330_config_xyz_tap_recognition(uint8_t en_double_tap);
ssp_err_t  ISM330_config_tilt_recognition(void);
ssp_err_t  ISM330_config_free_fall_recognition(void);
ssp_err_t  ISM330_config_wake_up_recognition(void);
ssp_err_t  ISM330_config_6d_recognition(void);

ssp_err_t  ISM330_INT2_activity_interrupt_control(uint8_t int_mask);
ssp_err_t  ISM330_activity_interrupts_control(uint8_t enable);

#endif



