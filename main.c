//  WAREABLE MATRIX min
//    License: CC BY http://fukuno.jig.jp/

#define TYPE 0 // 通常
//#define TYPE 1 // マトリクスLEDの位置、間違えた時


#include "LPC1100.h"
#include "uart.h"
#include "xprintf.h"
#include <string.h>

#define boolean unsigned char
#define true 1
#define false 0

boolean startsWith(char* s, char* key) {
	for (int i = 0;; i++) {
		char c1 = s[i];
		char c2 = key[i];
		if (c2 == '\0')
			return true;
		if (c1 == '\0')
			return false;
		if (c1 != c2)
			return false;
	}
}

#if TYPE==0
#define ROTATE 0
const char xpos[] = { 4, 2, 109, 3, 100, 108, 5, 103 };
const char ypos[] = { 6, 8, 9, 102, 7, 101, 11, 10 };
#endif
#if TYPE==1
#define ROTATE 2
/*
	x	y
0	0_6	0_4
1	1_2	1_1
2	0_11	1_0
3	1_3	0_2
4	0_9	0_5
5	0_10	0_8
6	0_7	1_9
7	0_3	1_8
*/
const char xpos[] = { 6, 102, 5, 103, 9, 10, 4, 3 };
const char ypos[] = { 7, 101, 100, 2, 11, 8, 109, 108 };
#endif

short d0def = 0;
short d1def = 0;

void init() {
	IOCON_PIO0_7 = 0x000000d0;
	GPIO0DIR |= 1 << 7;
	IOCON_PIO0_4 = 0x000000d0;
	GPIO0DIR |= 1 << 4;
	IOCON_PIO0_3 = 0x000000d0;
	GPIO0DIR |= 1 << 3;
	IOCON_PIO0_2 = 0x000000d0;
	GPIO0DIR |= 1 << 2;
	IOCON_PIO1_9 = 0x000000d0;
	GPIO1DIR |= 1 << 9;
	IOCON_PIO1_8 = 0x000000d0;
	GPIO1DIR |= 1 << 8;
	
	IOCON_PIO0_8 = 0x000000d0;
	GPIO0DIR |= 1 << 8;
	IOCON_PIO0_9 = 0x000000d0;
	GPIO0DIR |= 1 << 9;
	IOCON_SWCLK_PIO0_10 = 0x000000d1;
	GPIO0DIR |= 1 << 10;
	IOCON_R_PIO0_11 = 0x000000d1;
	GPIO0DIR |= 1 << 11;
	IOCON_PIO0_5 = 0x000000d0;
	GPIO0DIR |= 1 << 5;
	IOCON_PIO0_6 = 0x000000d0;
	GPIO0DIR |= 1 << 6;
	IOCON_R_PIO1_0 = 0x000000d1;
	GPIO1DIR |= 1 << 0;
	IOCON_R_PIO1_1 = 0x000000d1;
	GPIO1DIR |= 1 << 1;
	IOCON_R_PIO1_2 = 0x000000d1;
	GPIO1DIR |= 1 << 2;
	IOCON_SWDIO_PIO1_3 = 0x000000d1;
	GPIO1DIR |= 1 << 3;
	
	/*
	x		y
0	PIO0_4	PIO0_6
1	PIO0_2	PIO0_8
2	PIO1_9	PIO0_9
3	PIO0_3	PIO1_2
4	PIO1_0	PIO0_7
5	PIO1_8	PIO1_1
6	PIO0_5	PIO0_11
7	PIO1_3	PIO0_10
	*/
	// 	y->x // x = HIGH
	//            BA9876543210
	for (int i = 0; i < 8; i++) {
		int nx = xpos[i];
		if (nx < 100) {
			d0def |= (1 << nx);
		} else {
			d1def |= (1 << (nx - 100));
		}
	}
	GPIO0DATA = d0def;
	GPIO1DATA = d1def;
//	GPIO0MASKED[0b000000111100] = 0x7ff;
//	GPIO1MASKED[0b001100001001] = 0x7ff;
	
	// 0_5 つかない
	//            BA9876543210
	/*
	GPIO0DATA = 0b010000111100;
	GPIO1DATA = 0b001100000001;
	for (;;);
	*/
}
void matrixled_on(int x, int y) {
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (x > 7)
		x = 7;
	if (y > 7)
		y = 7;
	
	// rotate
#if ROTATE==0
	x = 7 - x;
#elif ROTATE==2
	y = 7 - y;
#endif
	// view
	
	int d0 = d0def; // 0b000000111100;
	int d1 = d1def; // 0b001100001001;
	int ny = ypos[y];
	if (ny < 100) {
		d0 |= 1 << ny;
	} else {
		d1 |= 1 << (ny - 100);
	}
	int nx = xpos[x];
	if (nx < 100) {
		d0 &= ~(1 << nx);
	} else {
		d1 &= ~(1 << (nx - 100));
	}
	GPIO0DATA = d0;
	GPIO1DATA = d1;
}
void matrixled_off() {
	GPIO0DATA = d0def;
	GPIO1DATA = d1def;
//	GPIO0DATA = 0b000000111100;
//	GPIO1DATA = 0b001100001001;
}

void initUART() {
	uart0_init();
	xdev_out(uart0_putc);
	xdev_in(uart0_getc);
}

/*
systick
*/
volatile int systick;
void InitSysTick(int hz) {
	SYST_RVR = SYSCLK / hz - 1;
	SYST_CSR = 0x07;
}
void SysTick_Handler(void) {
	systick++;
}
void wait(int n) {
	int endt = systick + n;
	for (;;) {
		if (systick > endt)
			break;
	}
}

// util
void setMatrix(char* data) {
	for (int j = 0; j < 8; j++) {
		char d = data[j];
		for (int i = 0; i < 8; i++) {
			if (d & (1 << i)) {
				matrixled_on(i, j);
			} else {
				matrixled_off();
			}
			wait(1);
		}
	}
	matrixled_off();
}

// urt

/*
0123456789abcdef
ff181818181818ff
*/



// bitman

/* bitman
0098e41f1fe49800
0884e43e3ee48408
*/
void decode(unsigned char* src, unsigned char* dst) {
	for (int i = 0; i < 16; i++) {
		int c = *(src + i);
		if (c >= '0' && c <= '9')
			c -= '0';
		else if (c >= 'a' && c <= 'f')
			c -= 'a' - 10;
		else if (c >= 'A' && c <= 'F')
			c -= 'A' - 10;
		else
			break;
		if (i % 2 == 1) {
			dst[i / 2 % 8] = (dst[i / 2 % 8] & 0b11110000) | c;
		} else {
			dst[i / 2 % 8] = (dst[i / 2 % 8] & 0b1111) | (c << 4);
		}
	}
}
boolean bitman() {
	unsigned char data[16];
	decode((unsigned char*)"0098e41f1fe49800", data);
	decode((unsigned char*)"0884e43e3ee48408", data + 8);
	
	int ptn = 0;
	for (int i = 0; i < 100 * 7; i++) {
		setMatrix(data + ptn * 8);
		if (i % 100 == 99)
			ptn = 1 - ptn;
		
		if (uart0_test())
			return 0;
	}
	return 1;
}

// jigjp

#if 0
#define LEN_DATA_ANIM 34
const unsigned char DATA_ANIM[] = {
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b10000000,
  0b11111010,
  0b00000000,
  0b00111010,
  0b00000000,
  0b10111000,
  0b10101000,
  0b11111000,
  0b00001000,
  0b00100000,
  0b10000000,
  0b11111010,
  0b00000000,
  0b11111000,
  0b00101000,
  0b00111000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000
};
#else
#define LEN_DATA_ANIM 58
const unsigned char DATA_ANIM[] = {
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b11111110,
  0b00010010,
  0b00010010,
  0b00001100,
  0b00000000,
  0b01111100,
  0b10000010,
  0b10000010,
  0b01000100,
  0b00000000,
  0b11111110,
  0b00011000,
  0b00110000,
  0b11111110,
  0b00000000,
  0b01111110,
  0b10000000,
  0b01110000,
  0b10000000,
  0b01111110,
  0b00000000,
  0b11111110,
  0b00000000,
  0b11111110,
  0b00011000,
  0b00110000,
  0b11111110,
  0b00000000,
  0b11111110,
  0b00011000,
  0b00110000,
  0b11111110,
  0b00000000,
  0b11111110,
  0b10010010,
  0b10010010,
  0b00000000,
  0b11111110,
  0b00010010,
  0b00110010,
  0b11001100,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000
};
#endif

boolean animate(char* data, int len) {
	for (;;) {
		for (int i = 0; i < len - 8; i++) {
			for (int j = 0; j < 30; j++) {
				setMatrix(data + i);
				if (uart0_test())
					return 0;
			}
		}
		break;
	}
	return 1;
}

void test() {
	// test
	for (;;) {
		for (int i = 0; i < 8 * 8; i++) {
			matrixled_on(i % 8, i / 8);
			wait(1);
		}
	}
}

//#define LEN_DATA 2048
#define LEN_DATA 1024
char data[LEN_DATA];

#define N_FRAME ((LEN_DATA - 8) / 10)
struct Frame {
	char frame[N_FRAME][8];
	short waitms[N_FRAME];
};
struct Frame* fr;

void init_frame() {
	for (int i = 0; i < N_FRAME; i++) {
		fr->waitms[i] = 0;
		memset(fr->frame[i], 0, 8);
	}
	/*
	*(long*)fr->frame[0] = (long)0x55aa55aa55aa55aa;
	*(long*)fr->frame[1] = (long)0xaa55aa55aa55aa55;
//	decode("0098e41f1fe49800", frame[0]);
	fr->waitms[0] = fr->waitms[1] = 100;
	*/
}

#include "iap.h"
// len 512, 1024, 2048, 4096
//void saveFlash(char* buf, int len)

void loadFlash(char* buf, int len) {
	for (int i = 0; i < len; i++)
		data[i] = SAVED_FLASH[i];
}
boolean load() {
	fr = (struct Frame*)(data + 8);
	loadFlash(data, LEN_DATA);
	if (startsWith(data, "MATLED00")) {
		println("MATLED00");
		if (fr->waitms[0] == 0)
			return false;
		return true;
	}
	memcpy(data, "MATLED00", 8);
	init_frame();
	return false;
}
void save() {
	saveFlash(data, LEN_DATA);
}

/*
MATLED SET n data wait
	n 0-100, data:nnx8 wait(msec)
MATLED RUN

MATLED SHOW FFFFFFFFFFFFFFFF
MATLED SHOW 55aa55aa55aa55aa
MATLED SHOW 183C7EFFFF7E3C18

00011000
00111100
01111110
11111111
11111111
01111110
00111100
00011000
*/
int parseInt(char* s) {
	int res = 0;
	for (int i = 0;; i++) {
		char c = *s;
		if (c >= '0' && c <= '9') {
			res = res * 10 + (c - '0');
		} else {
			return res;
		}
		s++;
	}
}
int indexOf(char* s, char c) {
	for (int i = 0;; i++) {
		if (*s == 0)
			return -1;
		if (*s == c)
			return i;
		s++;
	}
}

void println(char* s) {
	for (;;) {
		char c = *s++;
		if (c == 0)
			break;
		uart0_putc(c);
	}
	uart0_putc('\n');
}


void uart() {
#define SIZE_BUF 128
	
	char buf[SIZE_BUF];
	int nbuf = 0;
	
//	init_frame();
//	load(); // 初回はデータクリアいるかも？
	int mode = 1;
	
	unsigned char data[8];
	for (int i = 0; i < 8; i++)
		data[i] = 0;
	
	int n = 0;
	int cnt = 0;
	int nframe = 0;
	for (int i = 0;; i++) {
		while (uart0_test()) {
			int c = uart0_getc();
			if (c == '\n') {
				buf[nbuf] = '\0';
				if (startsWith(buf, "MATLED SHOW ")) {
					decode(buf + (9 + 3), data);
					println("SHOW");
				} else if (startsWith(buf, "MATLED SET ")) {
					char* pbuf = buf + 11;
					int nf = parseInt(pbuf);
					if (nf >= 0 && nf <= N_FRAME) {
						int n = indexOf(pbuf, ' ');
						if (n >= 0) {
							pbuf += n + 1;
//							println(pbuf);
							decode(pbuf, fr->frame[nf]);
							decode(pbuf, data); // 停止時の画面にも表示
							n = indexOf(pbuf, ' ');
							int nw = 100;
							if (n >= 0) {
								pbuf += n + 1;
								nw = parseInt(pbuf);
							}
							fr->waitms[nf] = nw;
						}
					}
				} else if (startsWith(buf, "MATLED CLEAR")) {
					mode = 0;
					init_frame();
				} else if (startsWith(buf, "MATLED RUN")) {
					mode = 1;
					println("RUN");
				} else if (startsWith(buf, "MATLED STOP")) {
					mode = 0;
					println("STOP");
				} else if (startsWith(buf, "MATLED SAVE")) {
					save();
					println("SAVE");
				} else if (startsWith(buf, "MATLED LOAD")) {
					load();
					println("LOAD");
				}
				nbuf = 0;
				continue;
			} else if (c == '\r') {
			} else {
				if (nbuf < SIZE_BUF - 1)
					buf[nbuf++] = c;
			}
		}
		if (mode == 0) {
			setMatrix(data);
		} else {
			setMatrix(fr->frame[nframe]);
			
			cnt++;
			if (cnt >= fr->waitms[nframe]) {
				cnt = 0;
				int bknframe = nframe;
				for (;;) {
					nframe++;
					if (nframe == N_FRAME)
						nframe = 0;
					if (fr->waitms[nframe])
						break;
					if (bknframe == nframe) {
						mode = 0;
						break;
					}
				}
			}
		}
	}
}

int main() {
	InitSysTick(20000);
	init();
	
	initUART();
	
//	uart();
	
	//test();
	if (!load()) {
		for (;;) {
			if (!animate(DATA_ANIM, LEN_DATA_ANIM))
				break;
			if (!bitman())
				break;
		}
	}
	uart();
	
	return 0;
}
