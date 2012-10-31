// lib for RFM12B from https://github.com/jcw/jeelib
#include <JeeLib.h>
// avr sleep instructions
#include <avr/sleep.h>

/*************************************************************/

// structure of data
typedef struct {
	long ping;
	int nodeid;
	byte lobat		:1;
	int battvol;
} Payload;
Payload measure;

static byte NODEID = 2;       // ID this node
static byte NODEGROUP = 212;

// Input/Output definition
// Analog
#define LDRPin            0
#define BatteryVolPin     1
#define CustomA3          3

// Digital
#define CustomD3          3
#define CustomD4          4
#define MOSFET            5
#define RTC_INT           6
#define CustomD7          7
#define ONEWIRE_DATA      8
#define ACT_LED           9 // or custom use

// Settings
#define MEASURE_PERIOD    300 // how often to measure, in tenths of seconds
#define RETRY_PERIOD      10 // how soon to retry if ACK didn't come in
#define RETRY_ACK         5 // maximum number of times to retry
#define ACK_TIME          10 // number of milliseconds to wait for an ack
#define REPORT_EVERY      6 // report every N measurement cycles
#define SMOOTH            3 // smoothing factor used for running averages

#define VCC_OK            330  // 3.3V  
#define VCC_LOW           310
#define VCC_CRIT          300

// Used devices and buses
#define DEBUG             0 // debug mode - serial output
#define LED_ON            1 // use act led for transmission


#define RADIO_SYNC_MODE   2

#define rf12_sleep(x)

/************************************************************/

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

enum { MEASURE, REPORT, TASKS };

static word schedbuf[TASKS];
Scheduler scheduler (schedbuf, TASKS);

Port batvol (3); // Analog 2

byte count = 0;
static byte reportCount;

void setup()
{
    rf12_initialize(NODEID, RF12_433MHZ, NODEGROUP);

#if DEBUG
    Serial.begin(115200);
#endif

  rf12_sleep(RF12_SLEEP);

  reportCount = REPORT_EVERY;
  scheduler.timer(MEASURE, 0);
  
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  batvol.digiWrite2(0);
  ldr.digiWrite2(0);
  
}
// Usage: smoothedAverage(payload.humi, humi, firstTime);
static int smoothedAverage(int prev, int next, byte firstTime =0) {
    if (firstTime)
        return next;
    return ((SMOOTH - 1) * prev + next + SMOOTH / 2) / SMOOTH;
}

void loop()
{
  #if DEBUG
     Serial.print('.');
     delay(2);	
  #endif
  
  switch (scheduler.pollWaiting()) {
    
    case MEASURE:
        scheduler.timer(MEASURE, MEASURE_PERIOD);
        doMeasure();

        if (++reportCount >= REPORT_EVERY) {
          reportCount = 0;
          scheduler.timer(REPORT, 0);
        }
        break;
    case REPORT:
        if (DEBUG) {
          transmissionRS();
          doReport();
        }
        else
        {
		if (LED_ON) {
			activityLed(1);
			}
          doReport();
		if (LED_ON) {
			activityLed(0);
			}
        }
        break;
  }
}


static void doReportACK()
{
  for (byte i = 0; i < RETRY_ACK; ++i) {
    rf12_sleep(RF12_WAKEUP);
    while (!rf12_canSend())
      rf12_recvDone();
    rf12_sendStart(RF12_HDR_ACK, &measure, sizeof measure);
    rf12_sendWait(RADIO_SYNC_MODE);
    byte acked = waitForACK();
    rf12_sleep(RF12_SLEEP);
    
    if (acked) {
      #if DEBUG
        Serial.print("ACK! ");
        Serial.println((int) i);
        Serial.println("-------");
        delay(2);
      #endif
      scheduler.timer(MEASURE, MEASURE_PERIOD);
      return;
    }
    Sleepy::loseSomeTime(RETRY_PERIOD * 100);
  }
  scheduler.timer(MEASURE, MEASURE_PERIOD);
  #if DEBUG
    Serial.print("no ACK! ");
    Serial.println("-------");
    delay(2);
  #endif
}

static void doReport()
{
  rf12_sleep(RF12_WAKEUP);
  while (!rf12_canSend())
    rf12_recvDone();
  rf12_sendStart(0, &measure, sizeof measure, RADIO_SYNC_MODE);
  rf12_sleep(RF12_SLEEP);
}

static byte waitForACK() {
  MilliTimer t;
  while (!t.poll(ACK_TIME)) {
    if (rf12_recvDone() && rf12_crc == 0 && rf12_hdr == (RF12_HDR_DST | RF12_HDR_CTL | NODEID))
      return 1;
     set_sleep_mode(SLEEP_MODE_IDLE);
     sleep_mode();
  }
  return 0;
}

static void transmissionRS()
{
  activityLed(1);
  Serial.print("LOBAT " );
  Serial.println(measure.lobat, DEC);
  Serial.print("BATVOL ");
  Serial.println(measure.battvol);
  activityLed(0);
}

static void activityLed (byte on) {
  pinMode(ACT_LED, OUTPUT);
  digitalWrite(ACT_LED, on);
  delay(150);
}

static void doMeasure() {
  byte firstTime = measure.humi == 0;
  
  measure.nodeid = NODEID;
  measure.lobat = rf12_lowbat();
  measure.battvol = map(batvol.anaRead(), 0, 1023, 0, 660);
}

