//basado en el codigo del profesor Hugo y en la pagina http://forum.osdev.org/viewtopic.php?t=10247



#include <kernel.h>
#include <mtask.h>
//falla al mover el mouse a la izquierda, se mueve 255 posiciones de golpe siempre

// Definiciones del controlador de mouse PS/2
#define MOUSE				0x60
#define MOUSE_CTL			0x64
#define MOUSE_IBF			0x01
#define MOUSE_OBF			0x02
#define MOUSE_INT			12
#define MOUSE_ENABLE		0xA8
#define MOUSE_GETSTATUS		0x20
#define MOUSE_SETSTATUS		0x60
#define MOUSE_ENAIRQ		0x02
#define MOUSE_CMD			0xD4
#define MOUSE_DEFAULT		0xF6
#define MOUSE_ENASTREAM		0xF4
#define MOUSE_ACK			0xFA
#define MOUSE_NACK			0xFF

// Parámetros de recepción por polling
#define MOUSE_NTRIES		1000
#define MOUSE_DELAY			10


typedef unsigned char byte;
typedef unsigned int dword;

//color original de donde se encontraba el mouse
static short  originalCharacter;

void WriteCharacter( unsigned char backcolour);
void detectClickOnBar();

#define MOUSE_PRIO			10000		// Alta prioridad, para que funcione como "bottom half" de la interrupción
#define MOUSE_BUFSIZE		32
static char mouse_cycle=0;     //unsigned char
unsigned char mouse_byte[3];    //signed char

static  int mouse_x=0;         //signed char
static  int mouse_y=0;         //signed char
static  int mouse_x_prev=0;         //valor previo de x
static  int mouse_y_prev=0;         //valor previo de y 
static bool initialPosition=true;

typedef enum{OFF, ON} state;

//funcion que apaga mouse de la pantalla, no modifica internamente la posicion en la que esta
void turnOffMouse(){

			volatile short * where;// posicion actual
		     where = (volatile short *)VIDMEM + (mouse_y * 80 + mouse_x) ;
		*where =originalCharacter;
}


void turnOnMouse(){
	initialPosition=true;
	WriteCharacter(LIGHTRED);

}
//Si se le pasa el parametro ON ,marca la pestaña de la consola para indicar que consola se esta empleando
//Si se le pasa el parametro OFF, desmarca la pestaña
void turnOnOFFTab(state s,int i){
	volatile short * where;//nueva posicion
	unsigned char c;
	unsigned char caracter;
	int start, end;
	unsigned char forecolour;
	unsigned char backcolour;
	
	
	
	//seccion donde se determinan los colores segun se quiere encender o apagar pestaña
	switch (s){
				case OFF:{
						forecolour=WHITE;
						backcolour=BLACK;
					break;
				}
				case ON:{
					forecolour=BLACK;
					backcolour=WHITE;		
					break;
				}
	}

	short attrib = (backcolour << 4) | (forecolour & 0x0F);	

	// seccion donde se determinan los limites a pintar o despintar
	if(i==1){

		start=0;
		end=8;
	}else if(i==2){
		start=11;
		end=19;

	}else if(i==3){
		start=22;
		end=30;
	}else if(i==4){
		start=33;
		end=41;
	}	
	int k;
	for( k=start;k<end;k++){
			     where = (volatile short *)VIDMEM + (k) ;
				c=*where;
				caracter=(unsigned char) (c&0x0ff);
				*where = caracter | (attrib << 8);
			if(k==mouse_x)
				originalCharacter=*where;
		}


}


//funcion que apaga todas las pestañas
void clearAllTabs(){
	int j=0;
	for(j=0;j<=4;j++){
		
	}


}



// funcion que detecta si se hizo click sobre la barra superior.
//Si este es el caso, se fija en que pestaña se hizo click y llama a la funcion de cambio de consola.
void detectClickOnBar(){


	if(mouse_y==0 && mouse_x>=0){

		if( mouse_x<8){
			printk("seleciono con1 \n");
			turnOnTab(1);
		}else if(mouse_x>10 && mouse_x<19){

			printk("seleciono con2\n");
			turnOnTab(2);
		}else if(mouse_x>21 && mouse_x<30){

			printk("seleciono con3\n");
			turnOnTab(3);
		}else if(mouse_x>32 && mouse_x<41){

			printk("seleciono con4\n");
			turnOnTab(4);
		}







	}
}



//funcion que cambia el fondo de una posicion de memoria
void WriteCharacter(  unsigned char backcolour)
{	unsigned char c;
	unsigned char cprev;
	 unsigned char forecolour;
	
	if(mouse_x!=mouse_x_prev || mouse_y!=mouse_y_prev ||initialPosition)
			{
		/*
			//devolver color anterior
			volatile short * prev=(volatile short *)VIDMEM + (mouse_y_prev * 80 + mouse_x_prev) ;
			
			short prev_value=*prev;//valor previo incluyendo colores y atributos de color
			cprev=prev_value;//caracter anterior			
			//unsigned char forecolourprev=(prev_value>>8)&0x0f;
			

			printk("forecolour previo %d \n",originalForeColour);
			short attribprev=( originalBackColour<< 4) | (originalForeColour & 0x0F);

			 *prev = cprev | (attribprev << 8);
						

	*/
	volatile short * prev=(volatile short *)VIDMEM + (mouse_y_prev * 80 + mouse_x_prev) ;
	volatile short * where;//nueva posicion
		     where = (volatile short *)VIDMEM + (mouse_y * 80 + mouse_x) ;
	if(!initialPosition){
			*prev=originalCharacter;
			//initialPosition=false;
			//printk("entro en no initial pos\n");
		}		
		initialPosition=false;
			
		//SE PINTA LA NUEVA POSICION
			
		     
		     
		     originalCharacter=*where;
		     short aux=*where;//se guarda una copia del caracter y sus colores originales
		     c=(unsigned char) (aux&0x0ff);
		    
		    forecolour=(unsigned char)((c>>8) &0xf);
		     //originalForeColour=forecolour;
		    //originalBackColour=(unsigned char)((c>>8) &0xf0);//se guarda el color de fondo  de la nueva posicion actual
		     //printk("color de fondo original %d\n",originalBackColour );
		     //se graba el nuevo color de fondo
		     short attrib = (backcolour << 4) | (forecolour & 0x0F);
		     *where = c | (attrib << 8);

		 }    
		  
}




// Interrupción de mouse.
static void 
mouse_int(unsigned irq)
{
	// Interrupción de mouse.
	//unsigned c = inb(MOUSE);
	//PutMsgQueueCond(scan_mq, &c);
	//printk("LLEGO interrupcion MOUSE \n");
	//veces++;

	 switch(mouse_cycle)
  {
    case 0:
      mouse_byte[0]=inb(0x60);
      mouse_cycle++;
      break;
    case 1:
      mouse_byte[1]=inb(0x60);
      mouse_cycle++;
      break;
    case 2:{

		      mouse_byte[2]=inb(0x60);
		      //mouse_x=mouse_byte[1];
		      //mouse_y=mouse_byte[2];
		      mouse_cycle=0;
		    
		      //precionado el boton izquierdo
		      if (mouse_byte[0] & 0x1)
      				//printk("Left button is pressed!\n");
		      		detectClickOnBar();
			     
			int delta_y = mouse_byte[2];
			if ( mouse_byte[0] & 0x20 )
				delta_y -= 256; 

			int delta_x = mouse_byte[1];
			if ( mouse_byte[0] & 0x10 )
				delta_x -= 256; 

//printk("dx %d dy %d\n", delta_x, delta_y);



			      //delta y negativo
			mouse_y_prev=mouse_y;		
			    	
			  if((mouse_byte[0] & 0x20)==0x20){
			    if(mouse_y<ABSROWS){
				mouse_y-=delta_y;


					}
			    	//mouse_y=(mouse_y<0)?0:mouse_y;
			    	mouse_y=(mouse_y<ABSROWS)?mouse_y:(ABSROWS-1);
			   // printk("movY negativo\n");
			  }else{//deltaY positivo
			  		if(mouse_y>0)
			     		 mouse_y-=delta_y;
			     	mouse_y=(mouse_y<0)?0:mouse_y;
			     // printk("movY positivo\n");
			  }



			//unsigned char aux=mouse_byte[1];
			mouse_x_prev=mouse_x;		
			    	
			  //deltaX negativo
			  if((mouse_byte[0] & 0x10)==0x10){

			   
			   if(mouse_x>0){


			    	mouse_x=mouse_x+delta_x;
			    	//printk("se decrementa x en %u\n",aux);
			   		}
			    mouse_x=(mouse_x<0)?0:mouse_x;

			    
			    //printk("mov x negativo\n");
			  }else{//deltaX positivo

			    //printk("mov x positivo\n");
			    	
			      if(mouse_x<NUMCOLS)//solo incrementa mientras no se pase de los limites de la pantalla
			      	mouse_x=mouse_x+delta_x;
			      mouse_x=(mouse_x>=NUMCOLS)?(NUMCOLS-1):mouse_x;
			      //printk("desplazamiento positivo en %u unidades \n",aux);
			     
			  }
			  //si se movio el mouse, se marca la nueva posicion
			  
			WriteCharacter( LIGHTRED);//DESCOMENTAR ESTO!!!!
	  //printk("X: %d Y: %d. \n", mouse_x, mouse_y);



			  // printk("x: %u \n",mouse_x);

      break;
  	}
  }



  
}

// Escribe en el registro de datos del controlador
static void 
mouse_send(unsigned data)
{
	//printk("mouse_send 0x%02.2x\n", data);
    while ( inb(MOUSE_CTL) & MOUSE_OBF )
		Yield();
    outb(MOUSE, data);
}

// Escribe en el registro de control del controlador
static void 
mouse_send_ctl(unsigned data)
{
	//printk("mouse_send_ctl 0x%02.2x\n", data);
    while ( inb(MOUSE_CTL) & MOUSE_OBF )
		Yield();
    outb(MOUSE_CTL, data);
}

// Lee del registro de datos del controlador
static void
mouse_receive(unsigned *p, bool wait)
{
	unsigned tries_left;

	for ( tries_left = MOUSE_NTRIES ; tries_left && !(inb(MOUSE_CTL) & MOUSE_IBF) ; tries_left-- )
		Yield();
	if ( wait && !tries_left )
	{
		//printk("mouse_receive delay\n");
		Delay(MOUSE_DELAY);
		if ( !(inb(MOUSE_CTL) & MOUSE_IBF) );
		//	printk("mouse_receive timeout\n");
	}
	unsigned char c = inb(MOUSE);
	if ( p )
		*p = c;
	//printk("mouse_receive 0x%02.2x\n", c);
}

// Esta inicialización se hace con interrupción de mouse inactiva.
// Las respuestas se leen por polling.
static void
init_mouse(void)
{
	// Habilitar PS2 auxiliar
	mouse_send_ctl(MOUSE_ENABLE);
	mouse_receive(NULL, false);		// ignoramos la respuesta

	// Habilitar generación de IRQ12 leyendo y modificando
	// el "compaq status byte"
	unsigned status;
	mouse_send_ctl(MOUSE_GETSTATUS);
	mouse_receive(&status, false);	// suponemos que no va a fracasar
	status |= MOUSE_ENAIRQ;			// habilitar la generación de IRQ12
	mouse_send_ctl(MOUSE_SETSTATUS);
	mouse_send(status);
	mouse_receive(NULL, false);		// ignoramos la respuesta

	// Setear parámetros default
	mouse_send_ctl(MOUSE_CMD);
	mouse_send(MOUSE_DEFAULT);
	mouse_receive(NULL, true);		// ignoramos la respuesta

	// Habilitar el mouse para que mande eventos
	mouse_send_ctl(MOUSE_CMD);
	mouse_send(MOUSE_ENASTREAM);
	mouse_receive(NULL, true);		// ignoramos la respuesta
	//printk("posicion inicial x: %u",mouse_x);
}



// Interfaz pública

void
mt_mouse_init(void)
{
	
	init_mouse();
	mt_set_int_handler(MOUSE_INT, mouse_int);
	mt_enable_irq(MOUSE_INT);
}


void printMainBar(void){


	printk("CONSOLA1 | CONSOLA2 | CONSOLA3 | CONSOLA 4 \n");
	// int i;
	// char * str=Malloc(NUMCOLS+1);
	// for( i=0;i<NUMCOLS;i++){
	// 	str[i]="_";
	// }
	// str[NUMCOLS]=0;
	// printk("%s \n",str);
	return;
}
