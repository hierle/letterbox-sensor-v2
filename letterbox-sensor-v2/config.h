//
// config.h
//


// period for sending, e.g. 30 minutes
//#define PERIOD 30
#define PERIOD 30

// threshold for proximity sensor values
#define THRESHOLD 150


// if true, use OTAA (not ABP)
#define USE_OTAA true
//#define USE_OTAA false

// data rate
// https://docs.rakwireless.com/RUI3/Appendix/#data-rate-by-region
// 0 = SF12, 1 = SF11, 2 = SF10, 3 = SF9, 4 = SF8, 5 = SF7, 6 = SF7@250kHz
#define DATARATE 5

// TX power:
// https://docs.rakwireless.com/RUI3/Appendix/#tx-power-by-region
// 0 = MaxEIRP ~ 16dB,  1 = MaxEIRP - 2dB,  2 = MaxEIRP - 4dB  ...  7 = MaxEIRP - 14dB ~ 2dB
// default is 0 ... adjust to your need
// lower numbers for longer range
// higher numbers for increased battery life
#define TXPOWER 7

// ADR
// if set to true, DATARATE and TXPOWER are overwritten by ADR
//   and will probably also shorten battery life due to RX windows
#define ADR false

// confirm
// if set to 1, will probably also shorten battery life due to RX windows
#define CFM 0

// duty cycle
#define DC 0

// retry
#define RETRY 0

// FPort (1...200)
#define FPORT 1

