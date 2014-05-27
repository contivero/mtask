#define MOUSEINT		12
#define MOUSE			0x60
#include "kernel.h"
typedef unsigned char byte;
typedef unsigned int dword;

static void 
mouseint(unsigned irq);
static void mouse_wait(byte a_type);
unsigned char mouse_write(byte a_write);
byte mouse_read();

static int veces=1;

static void mouse_wait(byte a_type) //unsigned char
{
  dword _time_out=100000; //unsigned int
  if(a_type==0)
  {
    while(_time_out--) //Data
    {
      if((inb(0x64) & 1)==1)
      {
        return;
      }
    }
    return;
  }
  else
  {
    while(_time_out--) //Signal
    {
      if((inb(0x64) & 2)==0)
      {
        return;
      }
    }
    return;
  }
}

unsigned char mouse_write(byte a_write) //unsigned char
{
  //Wait to be able to send a command
  mouse_wait(1);
  //Tell the mouse we are sending a command
  outb(0x64, 0xD4);
  //Wait for the final part
  mouse_wait(1);
  //Finally write
  outb(0x60, a_write);
}

byte mouse_read()
{
  //Get's response from mouse
  mouse_wait(0);
  return inb(0x60);
}





static void 
mouseint(unsigned irq)
{
	// Interrupción de mouse.
	//unsigned c = inb(MOUSE);
	//PutMsgQueueCond(scan_mq, &c);
	printk("LLEGO interrupcion MOUSE %d \n",veces);
	veces++;
}

//se inicializa el mouse
void
mt_mouse_init(void)
	{	
	void DisableInts(void);

	 byte _status;  //unsigned char

	  //Enable the auxiliary mouse device
	  mouse_wait(1);
	  outb(0x64, 0xA8);
	 
	  //Enable the interrupts
	  mouse_wait(1);
	  outb(0x64, 0x20);
	  mouse_wait(0);
	  _status=(inb(0x60) | 2);
	  mouse_wait(1);
	  outb(0x64, 0x60);
	  mouse_wait(1);
	  outb(0x60, _status);
	 
	  //Tell the mouse to use default settings
	  mouse_write(0xF6);
	  mouse_read();  //Acknowledge
	 
	  //Enable the mouse
	  mouse_write(0xF4);
	  mouse_read();  //Acknowledge

	void RestoreInts(void);
	
	printk("se habilito int mouse\n");
	Delay(3000);
	
	mt_set_int_handler(MOUSEINT, mouseint);
	mt_enable_irq(MOUSEINT);
}

