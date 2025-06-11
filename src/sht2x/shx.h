
#define POLYNOMIAL  0x131 //P(x)=x^8+x^5+x^4+1 = 100110001

#define   TRIG_T_MEASUREMENT_HM    0xE3 // command trig. temp meas. hold master
#define   TRIG_RH_MEASUREMENT_HM   0xE5 // command trig. humidity meas. hold master
#define   TRIG_T_MEASUREMENT_POLL  0xF3 // command trig. temp meas. no hold master
#define   TRIG_RH_MEASUREMENT_POLL 0xF5 // command trig. humidity meas. no hold master
#define   USER_REG_W               0xE6 // command writing user register
#define   USER_REG_R               0xE7 // command reading user register
#define   SOFT_RESET               0xFE  // command soft reset



#define   SHT2x_RES_12_14BIT       0x00 // RH=12bit, T=14bit
#define   SHT2x_RES_8_12BIT        0x01 // RH= 8bit, T=12bit
#define   SHT2x_RES_10_13BIT       0x80 // RH=10bit, T=13bit
#define   SHT2x_RES_11_11BIT       0x81 // RH=11bit, T=11bit
#define   SHT2x_RES_MASK           0x85  // Mask for res. bits (7,0) in user reg.



#define   SHT2x_EOB_ON             0x40 // end of battery
#define   SHT2x_EOB_MASK           0x40 // Mask for EOB bit(6) in user reg.


#define   SHT2x_HEATER_ON          0x04 // heater on
#define   SHT2x_HEATER_OFF         0x00 // heater off
#define   SHT2x_HEATER_MASK        0x04 // Mask for Heater bit(2) in user reg.


// measurement signal selection
#define HUMIDITYsta    0
#define  TEMPsta       1



#define  I2C_ADR_W                128   // sensor I2C address + write bit
#define  I2C_ADR_R                129    // sensor I2C address + read bit

//end of add by liuzaihua 2011-7-12 21:26

//---------- Enumerations ------------------------------------------------------
#define   LOW               0
#define   HIGH              1
#define    ACK              0
#define    NO_ACK           1


#define    ACK_ERROR              0x01
#define     TIME_OUT_ERROR        0x02
#define     CHECKSUM_ERROR        0x04
#define     UNIT_ERROR            0x08  
  


#define SCL	              PA13
#define SDA	              PA12


