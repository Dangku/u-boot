#include <string.h>
#include "FreeRTOS.h"
#include "ir.h"
#include "keypad.h"
#include "gpio.h"
#include "saradc.h"
#include "suspend.h"

/*KEY GPIO*/
//#define GPIO_KEY_ID_POWER	GPIOH_1
#define GPIO_KEY_ID_POWER	GPIOD_3
/*WOL GPIO*/
#define GPIO_WOL		GPIOH_6

#define ADC_KEY_ID_POWER	520

static void vGpioKeyCallBack(struct xReportEvent event)
{
	uint32_t buf[4] = {0};

	switch (event.ulCode) {
	case GPIO_KEY_ID_POWER:
		buf[0] = POWER_KEY_WAKEUP;
		STR_Wakeup_src_Queue_Send(buf);
		break;
	case GPIO_WOL:
		buf[0] = WOL_WAKEUP;
		STR_Wakeup_src_Queue_Send(buf);
		break;
	default:
		break;
	}

	printf("GPIO key event 0x%x, key code %d, responseTicks %d\n",
		event.event, event.ulCode, event.responseTime);
}

static void vAdcKeyCallBack(struct xReportEvent event)
{
	uint32_t buf[4] = {0};

	switch (event.ulCode) {
	case ADC_KEY_ID_POWER:
		buf[0] = POWER_KEY_WAKEUP;
		STR_Wakeup_src_Queue_Send(buf);
		break;
	default:
		break;
	}

	printf("ADC key event 0x%x, key code %d, responseTime %d\n",
		event.event, event.ulCode, event.responseTime);
}


struct xGpioKeyInfo gpioKeyInfo[] = {
	GPIO_KEY_INFO(GPIO_KEY_ID_POWER, HIGH, EVENT_SHORT,
			vGpioKeyCallBack, NULL),
	GPIO_KEY_INFO(GPIO_WOL, HIGH, EVENT_SHORT,
                        vGpioKeyCallBack, NULL)
};

struct xAdcKeyInfo adcKeyInfo[] = {
	ADC_KEY_INFO(ADC_KEY_ID_POWER, 0, SARADC_CH2,
		     EVENT_SHORT,
		     vAdcKeyCallBack, NULL)
};

void vKeyPadInit(void)
{
	/* gpio key */
	vCreateGpioKey(gpioKeyInfo,
			sizeof(gpioKeyInfo)/sizeof(struct xGpioKeyInfo));
	vGpioKeyEnable();

	/* adc key */
	vCreateAdcKey(adcKeyInfo,
			sizeof(adcKeyInfo)/sizeof(struct xAdcKeyInfo));
	vAdcKeyEnable();
}

void vKeyPadDeinit(void)
{
	/* gpio key */
	vGpioKeyDisable();
	vDestroyGpioKey();

	/* adc key */
	vAdcKeyDisable();
	vDestroyAdcKey();
}
