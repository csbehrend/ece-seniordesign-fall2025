/*
 * ============================================================================
 * Filename   : ads7828.h
 * Author     : Connor Behrend
 * Description: Header for ADS7828 i2c communication
 * ============================================================================
 */

#define ADS7828_CMD_PD_Pos (2U)
#define ADS7828_CMD_PD_Mask (0x3UL << ADS7828_CMD_PD_Pos)
#define ADS7828_CMD_PD_BTW (0x0UL << ADS7828_CMD_PD_Pos)
#define ADS7828_CMD_PD_IROFF_ADON (0x1UL << ADS7828_CMD_PD_Pos)
#define ADS7828_CMD_PD_IRON_ADOFF (0x2UL << ADS7828_CMD_PD_Pos)
#define ADS7828_CMD_PD_IRON_ADON (0x3UL << ADS7828_CMD_PD_Pos)

#define ADS7828_CMD_CH_Pos (4U)
#define ADS7828_CMD_CH_Mask (0xFUL << ADS7828_CMD_CH_Pos)
#define ADS7828_CMD_CH_0_1 (0x0UL << ADS7828_CMD_CH_Pos)
#define ADS7828_CMD_CH_2_3 (0x1UL << ADS7828_CMD_CH_Pos)
#define ADS7828_CMD_CH_4_5 (0x2UL << ADS7828_CMD_CH_Pos)
#define ADS7828_CMD_CH_6_7 (0x3UL << ADS7828_CMD_CH_Pos)
#define ADS7828_CMD_CH_1_0 (0x4UL << ADS7828_CMD_CH_Pos)
#define ADS7828_CMD_CH_3_2 (0x5UL << ADS7828_CMD_CH_Pos)
#define ADS7828_CMD_CH_5_4 (0x6UL << ADS7828_CMD_CH_Pos)
#define ADS7828_CMD_CH_7_6 (0x7UL << ADS7828_CMD_CH_Pos)
#define ADS7828_CMD_CH_0_COM (0x8UL << ADS7828_CMD_CH_Pos)
#define ADS7828_CMD_CH_2_COM (0x9UL << ADS7828_CMD_CH_Pos)
#define ADS7828_CMD_CH_4_COM (0xAUL << ADS7828_CMD_CH_Pos)
#define ADS7828_CMD_CH_6_COM (0xBUL << ADS7828_CMD_CH_Pos)
#define ADS7828_CMD_CH_1_COM (0xCUL << ADS7828_CMD_CH_Pos)
#define ADS7828_CMD_CH_3_COM (0xDUL << ADS7828_CMD_CH_Pos)
#define ADS7828_CMD_CH_5_COM (0xEUL << ADS7828_CMD_CH_Pos)
#define ADS7828_CMD_CH_7_COM (0xFUL << ADS7828_CMD_CH_Pos)

#define ADS7828_ADDR_UPPER_Pos (3U)
#define ADS7828_ADDR_UPPER_Mask (0x1FUL << ADS7828_ADDR_UPPER_Pos)
#define ADS7828_ADDR_UPPER_FIXED (0x12UL << ADS7828_ADDR_UPPER_Pos)

#define ADS7828_ADDR_ID_Pos (1U)
#define ADS7828_ADDR_ID_Mask (0x7FUL << ADS7828_ADDR_ID_Pos)
#define ADS7828_ADDR_ID_0                                                      \
  (0x0UL << ADS7828_ADDR_ID_Pos | ADS7828_ADDR_UPPER_FIXED)
#define ADS7828_ADDR_ID_1                                                      \
  (0x1UL << ADS7828_ADDR_ID_Pos | ADS7828_ADDR_UPPER_FIXED)
#define ADS7828_ADDR_ID_2                                                      \
  (0x2UL << ADS7828_ADDR_ID_Pos | ADS7828_ADDR_UPPER_FIXED)
#define ADS7828_ADDR_ID_3                                                      \
  (0x3UL << ADS7828_ADDR_ID_Pos | ADS7828_ADDR_UPPER_FIXED)

#define ADS7828_ADDR_OP_Pos (0U)
#define ADS7828_ADDR_OP_Mask (0x1UL << ADS7828_ADDR_OP_Pos)
#define ADS7828_ADDR_OP_WRITE (0x0UL << ADS7828_ADDR_OP_Pos)
#define ADS7828_ADDR_OP_READ (0x1UL << ADS7828_ADDR_OP_Pos)

#define ADS7828_ADDR_ADC0 (ADS7828_ADDR_ID_0 >> ADS7828_ADDR_ID_Pos)
#define ADS7828_ADDR_ADC1 (ADS7828_ADDR_ID_1 >> ADS7828_ADDR_ID_Pos)
#define ADS7828_ADDR_ADC2 (ADS7828_ADDR_ID_2 >> ADS7828_ADDR_ID_Pos)
#define ADS7828_ADDR_ADC3 (ADS7828_ADDR_ID_3 >> ADS7828_ADDR_ID_Pos)
