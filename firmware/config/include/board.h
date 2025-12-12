/*
 * board.h
 */


#ifndef ARCH_BOARD_BOARD_H
#define ARCH_BOARD_BOARD_H

/************************************************************************************
 * Included Files
 ************************************************************************************/

#include <nuttx/config.h>
#ifndef __ASSEMBLY__
# include <stdint.h>
#endif

//#ifdef __KERNEL__
//# include <stm32.h>
//#endif

#ifdef CONFIG_FS_PROCFS
#  ifdef CONFIG_NSH_PROC_MOUNTPOINT
#    define STM32_PROCFS_MOUNTPOINT CONFIG_NSH_PROC_MOUNTPOINT
#  else
#    define STM32_PROCFS_MOUNTPOINT "/proc"
#  endif
#endif

/************************************************************************************
 * Pre-processor Definitions
 ************************************************************************************/

/* Clocking *************************************************************************/


#if 1
#  define HSI_CLOCK_CONFIG          /* HSI-16 clock configuration */
#elif 0
/* Make sure you installed one! */

#  define HSE_CLOCK_CONFIG          /* HSE with 8 MHz xtal */
#else
#  define MSI_CLOCK_CONFIG          /* MSI @ 4 MHz autotrimmed via LSE */
#endif

/* Clocking *****************************************************************/

#if defined(HSI_CLOCK_CONFIG)
/* The NUCLEOL432KC supports both HSE and LSE crystals (X2 and X3).
 * However, asshipped, the X3 crystal is not populated.  Therefore the
 * Nucleo-L432KC will need to run off the 16MHz HSI clock, or the 32khz-
 * synced MSI.
 *
 *   System Clock source           : PLL (HSI)
 *   SYSCLK(Hz)                    : 80000000      Determined by PLL
 *                                                 configuration
 *   HCLK(Hz)                      : 80000000     (STM32L4_RCC_CFGR_HPRE)
 *                                                (Max 80 MHz)
 *   AHB Prescaler                 : 1            (STM32L4_RCC_CFGR_HPRE)
 *                                                (Max 80 MHz)
 *   APB1 Prescaler                : 1            (STM32L4_RCC_CFGR_PPRE1)
 *                                                (Max 80 MHz)
 *   APB2 Prescaler                : 1            (STM32L4_RCC_CFGR_PPRE2)
 *                                                (Max 80 MHz)
 *   HSI Frequency(Hz)             : 16000000     (nominal)
 *   PLLM                          : 1            (STM32L4_PLLCFG_PLLM)
 *   PLLN                          : 10           (STM32L4_PLLCFG_PLLN)
 *   PLLP                          : 0            (STM32L4_PLLCFG_PLLP)
 *   PLLQ                          : 0            (STM32L4_PLLCFG_PLLQ)
 *   PLLR                          : 2            (STM32L4_PLLCFG_PLLR)
 *   PLLSAI1N                      : 12
 *   PLLSAI1Q                      : 4
 *   Flash Latency(WS)             : 4
 *   Prefetch Buffer               : OFF
 *   48MHz for USB OTG FS,         : Doable if required using PLLSAI1 or MSI
 *   RNG clock
 */

/* HSI - 16 MHz RC factory-trimmed
 * LSI - 32 KHz RC
 * MSI - variable up to 48 MHz, synchronized to LSE
 * HSE - not installed
 * LSE - not installed
 */

#define STM32L4_HSI_FREQUENCY     16000000ul
#define STM32L4_LSI_FREQUENCY     32000
#define STM32L4_LSE_FREQUENCY     32768

#define STM32L4_BOARD_USEHSI      1

/* XXX sysclk mux = pllclk */

/* XXX pll source mux = hsi */

/* REVISIT: Trimming of the HSI and MSI is not yet supported. */

/* Main PLL Configuration.
 *
 * Formulae:
 *
 *   VCO input frequency        = PLL input clock frequency / PLLM,
 *                                1 <= PLLM <= 8
 *   VCO output frequency       = VCO input frequency × PLLN,
 *                                8 <= PLLN <= 86, frequency range 64 to
 *                                 344 MHz
 *   PLL output P (SAI3) clock frequency = VCO frequency / PLLP,
 *                                 PLLP = 7, or 17, or 0 to disable
 *   PLL output Q (48M1) clock frequency = VCO frequency / PLLQ,
 *                                 PLLQ = 2, 4, 6, or 8, or 0 to disable
 *   PLL output R (CLK)  clock frequency = VCO frequency / PLLR,
 *                                 PLLR = 2, 4, 6, or 8, or 0 to disable
 *
 * PLL output P is used for SAI
 * PLL output Q is used for OTG FS, SDMMC, RNG
 * PLL output R is used for SYSCLK
 * PLLP = 0 (not used)
 * PLLQ = 0 (not used)
 * PLLR = 2
 * PLLN = 10
 * PLLM = 1
 *
 * We will configure like this
 *
 *   PLL source is HSI
 *
 *   PLL_REF  = STM32L4_HSI_FREQUENCY / PLLM
 *            = 16,000,000 / 1
 *            = 16,000,000
 *
 *   PLL_VCO  = PLL_REF * PLLN
 *            = 16,000,000 * 10
 *            = 160,000,000
 *
 *   PLL_CLK  = PLL_VCO / PLLR
 *            = 160,000,000 / 2 = 80,000,000
 *   PLL_48M1 = disabled
 *   PLL_SAI3 = disabled
 *
 * ----------------------------------------
 *
 * PLLSAI1 Configuration
 *
 * The clock input and M divider are identical to the main PLL.
 * However the multiplier and postscalers are independent.
 * The PLLSAI1 is configured only if CONFIG_STM32L4_SAI1PLL is defined
 *
 *   SAI1VCO input frequency        = PLL input clock frequency
 *   SAI1VCO output frequency       = SAI1VCO input frequency × PLLSAI1N,
 *                                    8 <= PLLSAI1N <= 86, frequency range
 *                                    64 to 344 MHz
 *   SAI1PLL output P (SAI1) clock frequency = SAI1VCO frequency / PLLSAI1P,
 *                                    PLLP = 7, or 17, or 0 to disable
 *   SAI1PLL output Q (48M2) clock frequency = SAI1VCO frequency / PLLSAI1Q,
 *                                    PLLQ = 2, 4, 6, or 8, or 0 to disable
 *   SAI1PLL output R (ADC1) clock frequency = SAI1VCO frequency / PLLSAI1R,
 *                                    PLLR = 2, 4, 6, or 8, or 0 to disable
 *
 * We will configure like this
 *
 * PLLSAI1 disabled
 *
 * ----------------------------------------
 *
 * PLLSAI2 Configuration
 *
 * The clock input and M divider are identical to the main PLL.
 * However the multiplier and postscalers are independent.
 * The PLLSAI2 is configured only if CONFIG_STM32L4_SAI2PLL is defined
 *
 *   SAI2VCO input frequency        = PLL input clock frequency
 *   SAI2VCO output frequency       = SAI2VCO input frequency × PLLSAI2N,
 *                                    8 <= PLLSAI1N <= 86, frequency range
 *                                    64 to 344 MHz
 *   SAI2PLL output P (SAI2) clock frequency = SAI2VCO frequency / PLLSAI2P,
 *                                    PLLP = 7, or 17, or 0 to disable
 *   SAI2PLL output R (ADC2) clock frequency = SAI2VCO frequency / PLLSAI2R,
 *                                    PLLR = 2, 4, 6, or 8, or 0 to disable
 *
 * We will configure like this
 *
 * PLLSAI2 disabled
 *
 * ----------------------------------------
 *
 * TODO: The STM32L is a low power peripheral and all these clocks should be
 *       configurable at runtime.
 *
 * ----------------------------------------
 *
 * TODO These clock sources can be configured in Kconfig (this is not a
 *      board feature)
 *
 * USART1
 * USART2
 * USART3
 * UART4
 * UART5
 * LPUART1
 * I2C1
 * I2C3
 * LPTIM1
 * LPTIM2
 * SAI1
 * SAI2
 * CLK48
 * ADC
 * SWPMI
 * DFSDM
 */

/* prescaler common to all PLL inputs; will be 1 (XXX source is implicitly
 * as per comment above HSI)
 */

#define STM32L4_PLLCFG_PLLM             RCC_PLLCFG_PLLM(1)

/* 'main' PLL config; we use this to generate our system clock via the R
 *  output.  We set it up as 16 MHz / 1 * 10 / 2 = 80 MHz
 *
 * XXX NOTE:  currently the main PLL is implicitly turned on and is
 * implicitly the system clock; this should be configurable since not all
 * applications may want things done this way.
 */

#define STM32L4_PLLCFG_PLLN             RCC_PLLCFG_PLLN(10)
#define STM32L4_PLLCFG_PLLP             0
#undef  STM32L4_PLLCFG_PLLP_ENABLED
#define STM32L4_PLLCFG_PLLQ             RCC_PLLCFG_PLLQ_2
#define STM32L4_PLLCFG_PLLQ_ENABLED
#define STM32L4_PLLCFG_PLLR             RCC_PLLCFG_PLLR(2)
#define STM32L4_PLLCFG_PLLR_ENABLED

/* 'SAIPLL1' is used to generate the 48 MHz clock, since we can't
 * do that with the main PLL's N value.  We set N = 12, and enable
 * the Q output (ultimately for CLK48) with /4.  So,
 * 16 MHz / 1 * 12 / 4 = 48 MHz
 *
 * XXX NOTE:  currently the SAIPLL /must/ be explicitly selected in the
 * menuconfig, or else all this is a moot point, and the various 48 MHz
 * peripherals will not work (RNG at present).  I would suggest removing
 * that option from Kconfig altogether, and simply making it an option
 * that is selected via a #define here, like all these other params.
 */

#define STM32L4_PLLSAI1CFG_PLLN         RCC_PLLSAI1CFG_PLLN(12)
#define STM32L4_PLLSAI1CFG_PLLP         0
#undef  STM32L4_PLLSAI1CFG_PLLP_ENABLED
#define STM32L4_PLLSAI1CFG_PLLQ         RCC_PLLSAI1CFG_PLLQ_4
#define STM32L4_PLLSAI1CFG_PLLQ_ENABLED
#define STM32L4_PLLSAI1CFG_PLLR         0
#undef  STM32L4_PLLSAI1CFG_PLLR_ENABLED

/* 'SAIPLL2' is not used in this application */

#define STM32L4_PLLSAI2CFG_PLLN         RCC_PLLSAI2CFG_PLLN(8)
#define STM32L4_PLLSAI2CFG_PLLP         0
#undef  STM32L4_PLLSAI2CFG_PLLP_ENABLED
#define STM32L4_PLLSAI2CFG_PLLR         0
#undef  STM32L4_PLLSAI2CFG_PLLR_ENABLED

#define STM32L4_SYSCLK_FREQUENCY  80000000ul

/* CLK48 will come from PLLSAI1 (implicitly Q) */

#if defined(CONFIG_STM32L4_USBFS) || defined(CONFIG_STM32L4_RNG)
#  define STM32L4_USE_CLK48       1
#  define STM32L4_CLK48_SEL       RCC_CCIPR_CLK48SEL_PLLSAI1
#  define STM32L4_HSI48_SYNCSRC   SYNCSRC_NONE
#endif

/* NOT enable the LSE oscillator, used automatically trim the MSI, and for RTC */

//#define STM32L4_USE_LSE           1

/* AHB clock (HCLK) is SYSCLK (80MHz) */

#define STM32L4_RCC_CFGR_HPRE     RCC_CFGR_HPRE_SYSCLK      /* HCLK  = SYSCLK / 1 */
#define STM32L4_HCLK_FREQUENCY    STM32L4_SYSCLK_FREQUENCY

/* APB1 clock (PCLK1) is HCLK / 1 (80MHz) */

#define STM32L4_RCC_CFGR_PPRE1    RCC_CFGR_PPRE1_HCLK       /* PCLK1 = HCLK / 1 */
#define STM32L4_PCLK1_FREQUENCY   (STM32L4_HCLK_FREQUENCY / 1)

/* The timer clock frequencies are automatically defined by hardware.
 * If the APB prescaler equals 1, the timer clock frequencies are set to the
 * same frequency as that of the APB domain. Otherwise they are set to twice.
 *
 * REVISIT : this can be configured
 */

#define STM32L4_APB1_TIM2_CLKIN   (STM32L4_PCLK1_FREQUENCY)
#define STM32L4_APB1_TIM6_CLKIN   (STM32L4_PCLK1_FREQUENCY)
#define STM32L4_APB1_TIM7_CLKIN   (STM32L4_PCLK1_FREQUENCY)
#define STM32L4_APB1_LPTIM1_CLKIN (STM32L4_PCLK1_FREQUENCY)
#define STM32L4_APB1_LPTIM2_CLKIN (STM32L4_PCLK1_FREQUENCY)

/* APB2 clock (PCLK2) is HCLK (80MHz) */

#define STM32L4_RCC_CFGR_PPRE2    RCC_CFGR_PPRE2_HCLK       /* PCLK2 = HCLK / 1 */
#define STM32L4_PCLK2_FREQUENCY   (STM32L4_HCLK_FREQUENCY / 1)

/* The timer clock frequencies are automatically defined by hardware.
 * If the APB prescaler equals 1, the timer clock frequencies are set to the
 * same frequency as that of the APB domain. Otherwise they are set to twice.
 *
 * REVISIT : this can be configured
 */

#define STM32L4_APB2_TIM1_CLKIN   (STM32L4_PCLK2_FREQUENCY)
#define STM32L4_APB2_TIM15_CLKIN  (STM32L4_PCLK2_FREQUENCY)
#define STM32L4_APB2_TIM16_CLKIN  (STM32L4_PCLK2_FREQUENCY)

#endif

#define BOARD_TIM1_FREQUENCY    STM32L4_HCLK_FREQUENCY
#define BOARD_TIM2_FREQUENCY    STM32L4_HCLK_FREQUENCY
#define BOARD_TIM6_FREQUENCY    STM32L4_HCLK_FREQUENCY
#define BOARD_TIM7_FREQUENCY    STM32L4_HCLK_FREQUENCY
#define BOARD_TIM15_FREQUENCY   STM32L4_HCLK_FREQUENCY
#define BOARD_TIM16_FREQUENCY   STM32L4_HCLK_FREQUENCY
#define BOARD_LPTIM1_FREQUENCY  STM32L4_HCLK_FREQUENCY
#define BOARD_LPTIM2_FREQUENCY  STM32L4_HCLK_FREQUENCY

/* LED index values for use with board_userled() */

#define BOARD_LED1        0
#define BOARD_NLEDS       1

/* LED bits for use with board_userled_all() */

#define BOARD_LD1         BOARD_LED1
#define BOARD_LED1_BIT    (1 << BOARD_LED1)
#define BOARD_STATUS_LED  BOARD_LED1
#define BOARD_STATUS_LED_BIT  (1 << BOARD_LED1)

#define RED_LED          0
#define GREEN_LED        1


#define GPIO_LD1 /* PA1 */ (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_CLEAR | \
                            GPIO_PORTA | GPIO_PIN1)
#define GPIO_LD2 /* PA6 */ (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_CLEAR | \
                            GPIO_PORTA | GPIO_PIN6)
#define I2C_RST  /* PA4 */ (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_CLEAR | \
                            GPIO_PORTA | GPIO_PIN4)
#define LPn      /* PA5 */ (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_CLEAR | \
                            GPIO_PORTA | GPIO_PIN5)

/* If CONFIG_ARCH_LEDS is defined, the usage by the board port is defined in
 * include/board.h and src/stm32_leds.c. The LEDs are used to encode OS-related
 * events as follows:
 *
 *   SYMBOL              Meaning                 LD1
 *   ------------------- ----------------------- ------
 *   LED_STARTED         NuttX has been started  OFF
 *   LED_HEAPALLOCATE    Heap has been allocated OFF
 *   LED_IRQSENABLED     Interrupts enabled      OFF
 *   LED_STACKCREATED    Idle stack created      ON
 *   LED_INIRQ           In an interrupt         N/C
 *   LED_SIGNAL          In a signal handler     N/C
 *   LED_ASSERTION       An assertion failed     N/C
 *   LED_PANIC           The system has crashed  FLASH
 *
 * Thus is LD1 is statically on, NuttX has successfully  booted and is,
 * apparently, running normally.  If LD1 is flashing at approximately
 * 2Hz, then a fatal error has been detected and the system has halted.
 */

#define LED_STARTED                  0 /* LD1=OFF */
#define LED_HEAPALLOCATE             0 /* LD1=OFF */
#define LED_IRQSENABLED              0 /* LD1=OFF */
#define LED_STACKCREATED             1 /* LD1=ON */
#define LED_INIRQ                    2 /* LD1=no change */
#define LED_SIGNAL                   2 /* LD1=no change */
#define LED_ASSERTION                2 /* LD1=no change */
#define LED_PANIC                    3 /* LD1=flashing */

/* Button definitions ***************************************************************/
/* The STM32F7 Discovery supports one button:  Pushbutton B1, labelled "User", is
 * connected to GPIO PI11.  A high value will be sensed when the button is depressed.
 */

#define BUTTON_USER        0
#define NUM_BUTTONS        1
#define BUTTON_USER_BIT    (1 << BUTTON_USER)

/* Pushbutton B1, labelled "User", is connected to GPIO PI11.  A high value will be sensed when the
 * button is depressed. Note that the EXTI interrupt is configured.
 */

#define GPIO_BTN_USER      (GPIO_INPUT | GPIO_FLOAT | GPIO_EXTI | GPIO_PORTI | GPIO_PIN11)

/* Alternate function pin selections ************************************************/

/* USART2:
 *   RXD: PA2
 *   TXD: PA3
 */

#define GPIO_USART2_RX GPIO_USART2_RX_1    /* PA2 */
#define GPIO_USART2_TX GPIO_USART2_TX_1    /* PA3  */

#define GPIO_CAN1_RX               GPIO_CAN1_RX_1 /* PA11 */
#define GPIO_CAN1_TX               GPIO_CAN1_TX_1 /* PA12 */

#define GPIO_I2C1_SCL              (GPIO_ALT | GPIO_AF4 | GPIO_OPENDRAIN | GPIO_PULLUP | GPIO_PORTA | GPIO_PIN9) /* PA9 */
#define GPIO_I2C1_SDA              (GPIO_ALT | GPIO_AF4 | GPIO_OPENDRAIN | GPIO_PULLUP | GPIO_PORTA | GPIO_PIN10) /* PA10 */

/************************************************************************************
 * Public Data
 ************************************************************************************/
#ifndef __ASSEMBLY__

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/************************************************************************************
 * Public Function Prototypes
 ************************************************************************************/

/************************************************************************************
 * Name: stm32_boardinitialize
 *
 * Description:
 *   All STM32 architectures must provide the following entry point.  This entry point
 *   is called early in the initialization -- after all memory has been configured
 *   and mapped but before any devices have been initialized.
 *
 ************************************************************************************/

void stm32_boardinitialize(void);

#undef EXTERN
#if defined(__cplusplus)
}
#endif
#endif

#endif  /* ARCH_BOARD_BOARD_H */
