#include "mbed.h"
//#include "Grove_LCD_RGB_Backlight.h"
#include "main.h"

#define PERIOD				0.00125f
//#define SENSIBILITE 0.03
//#define VITESSE 		0.25f
#define REF 				0.325f
#define KP				2.5f
#define TI				0.0000875f
#define TD				1.0f
#define MAX				0.325f
#define MIN				-0.325f

/*#define REF 				0.3f
#define KP				2.0f
#define TI				0.00005f
#define TD				1.0f
#define MAX				0.3
#define MIN				-0.3f*/

AnalogIn ldr1(P0_23);
AnalogIn ldr2(P0_24);
AnalogIn potmet(A2);
PwmOut mot1(P2_2);
PwmOut mot2(P2_3);
//Grove_LCD_RGB_Backlight lcd(I2C_SDA,I2C_SCL);
DigitalIn ir(P4_29);
u8 stop = 0, obs = 1;
Timer t;

void init_mot()
{
	mot1.period(PERIOD);
	mot2.period(PERIOD);
}
/*
//retourner 0 si le sens est bon, 1 s'il faut tourner a gauche, -1 s'il faut tourner a droite
float direction()
{
	float ldrL, ldrR;
	static float integral, i_prec = 0.0f;
	
	ldrL = ldr1.read();
	ldrR = ldr2.read();
	
	if(ldrL-ldrR < 0.05 && ldrR-ldrL < SENSIBILITE) {//les 2 capteurs ont la meme valeur
		return 0;
	}
	else if(ldrL - ldrR >= SENSIBILITE) {//il faut tourner a droite
		return -1;
	}
	else {
		return 1;
	}
}

void d_gauche(){
	mot1 = 0.0f;
	mot2 = VITESSE;
}

void d_droite(){
	mot1 = VITESSE;
	mot2 = 0.0f;
}

void d_toutDroit(){
	mot1 = VITESSE;
	mot2 = VITESSE;
}

void deplacement(){
	if (direction()==0)
		d_toutDroit();
	else if (direction()==1)
		d_gauche();
	else
		d_droite();
}*/

void deplacement_pid()
{
	static float I = 0, err_prec = 0;
	float P = 0, err, vitesse, D;
	
	err = ldr1.read() - ldr2.read();
	
	if(err > 0.175)
		P = 0.325f;
	else if(err < -0.175)
		P = -0.325f;
	else
		P = err * KP;
	
	/*if(err > 0.15)
		P = 0.3f;
	else if(err < -0.15)
		P = -0.3f;
	else
		P = err * KP;*/
	
	if(err < 0.03 && err > -0.03)
		I = 0;
	else
		I += err*TI;
	
	D = (err-err_prec) * TD;
	vitesse = P + I + D;
	
	if(vitesse > MAX)
		vitesse = MAX;
	if(vitesse < MIN)
		vitesse = MIN;
	
	mot1 = REF + vitesse;
	mot2 = REF - vitesse;
	
	err_prec = err;
}

int main() {
	float pot,ldr2In;
	uint32_t cpt = 0;
	//char s[20];
	//lcd.init();
	init_mot();
	
  while(1) {  
		/*pot = ldr1.read();
		ldr2In = ldr2.read();
		sprintf(s,"L:%.3f R:%.3f",pot,ldr2In);
		lcd.clear();
		lcd.print(s);*/
		stop = !(ir.read());
			
		//deplacement();		
		if(obs && stop){	
			mot1 = 0.0f;
			mot2 = 0.0f;
			if(cpt < 3/PERIOD) {
				cpt++;
			}
			else {
				cpt = 0;
				obs = 0;
			}
		}		
		else {
			deplacement_pid();
			cpt = 0;
		}
		
		wait(PERIOD);
  }
}