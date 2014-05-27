#define MOUSEINT		12
#define MOUSE			0x60
#include "kernel.h"

static void 
mouseint(unsigned irq);

static void 
mouseint(unsigned irq)
{
	// Interrupción de teclado. Por ahora tomamos todo lo que viene como si fueran
	// makes y breaks de teclas, cuando se envíen comandos al teclado (por ejemplo
	// para prender y apagar los LEDs), habrá que impedir que entren aquí las respuestas
	// o procesarlas por separado.

	//unsigned c = inb(MOUSE);
	//PutMsgQueueCond(scan_mq, &c);
	printk("LLEGO MOUSE\n");
}


void
mt_mouse_init(void)
{	
	printk("habilito int mouse\n");
	Delay(3000);
	mt_set_int_handler(MOUSEINT, mouseint);
	mt_enable_irq(MOUSEINT);
}
