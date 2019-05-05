/***********************************************************************
  Arduino sketch for programming of the original JP1 remotes
 ***********************************************************************

                                             14 Mar 2012 by Tim6502 
  
  This is for the original JP1 remote control interface ONLY.
  Not intended for JP1.1, 1.2, 1.3, etc. remotes!
  
  See http://www.hifi-remote.com/jp1/help/ for background on
  remote controls that are programmable via JP1. 
  
  The early JP1 remotes were programmable through the parallel port on PCs.
  These days, parallel ports -as well as the original JP1 remotes- are 
  becoming increasingly rare (both being replaced by the USB kind).
  Since at the moment I have a few more JP1 remotes (a handful) than
  parallel ports in operation (zero), I had to find a way to program
  updates for my audio-visual equipment lineup.
  
  The Arduino can be accessed via USB and can also easily talk to the
  EEPROM that contains the program for the JP1 remote.  The software 
  for JP1/JP1.X programming does communicate via USB to a JP1 EEPROM
  programmer firmware in order to program the original JP1 remotes.
  It appears that the Arduino can easily be enlisted to cover the combo
  of USB serial driver and JP1 EEPROM programmer. Since Kevin Timmerman's
  JP1 EEPROM Programmer software was open-sourced (Thanks, Kevin!), 
  here we are!
  
  
  Below are instructions and the Arduino sketch on how you can share the fun.
  
  
  (1) Make yourself a JP1 cable - for example by cutting up an IDE cable.
      View from the front of the female plug on the cable:
          +----------+    JP1 Wire pinout on remote: 
         /  1  3  5  |     1 - Vdd U1      2 - Vdd U2                             
         |  2  4  6  |     3 - Ground      4 - Serial Data (SDA)             
         +-----------+     5 - Reset       6 - Serial Clock (SCL)
  
   (2) Connect the wires from this cable as follows:
         - connect 1 and 2 together
         - connect 3 to Arduino Ground
         - connect 4 (SDA) to Arduino Analog Pin 4
         - connect 5 (Reset) to Arduino Ground
           NOTE: you will need to unplug the remote from the cable to use it.
         - connect 6 (SCL) to Arduino Analog Pin 5
         
       If the battery voltage in your remote is _higher_ than the voltage
       in the Arduino, you also need to pull up the following JP1 lines
       via a 10k Ohm resistor each, connected to the JP1 Vdd line 1 (or 2):
         4 (SDA) and 6 (SCL).
       
   (3) Attach the Arduino to your PC and upload this sketch.
       When you run the RemoteMasterIR or IR programs, select 
              JP1.X Serial... interface.
       Yes, even though this is for a non JP1.X remote!
       Now you should be able to download and/or upload the EEPROM
       content from or to your remote.
        
       Note that after an upload, the Arduino will be restarted by the
       RM / IR program. If you want to avoid this, you would need to
       pull RESET high with an adequate resistor value, or -if using
       an external USB-Serial TTL converter device, make connections
       except for the line that will pull on RESET.
   
 
   This sketch is directly based on assembler code from:      
   
   JP1 EEPROM Programming Adapter Firmware
   Copyright (C) 2010 Kevin Timmerman
   jp1epa [@t] compendiumarcana [d0t] com
   http://www.compendiumarcana.com/jp1epa
   
   For EEPROM communication this sketch uses example code by
   davekw7x, March, 2011, which in turn was
   derived from sketch for 24C64 devices posted on
      http://www.arduino.cc/playground/Code/I2CEEPROM
   from  hkhijhe   Date: 01/10/2010.
   
   
   While this worked for me with a *duino Duemillanova clone and with a
   URC 6131 remote using 2k 24C32 EEPROM chips (the default size this 
   sketch selects for EEPROMs), it has not been tested with other EEPROM 
   chips or other sizes.
   More importantly, you should be aware that you are using these 
   instructions and code at your own risk.  There are no guarantees
   that this will work, or that it will not damage or blow up your 
   equipment (or yourself). You have been warned!
   -  Good luck!
   
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

*/

#include <Wire.h>

// The seven-bit device address for EEPROMs
const byte DEVADDR = 0x50;

const char hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

byte ee_size;
byte ee_fill;
byte jp1_cmd;
byte jp1_len;
byte jp1_chk;
byte addrl;
byte addrh;

byte countl;

byte data_buf[40];

boolean fTwoByteAddress;
boolean fFillTestPattern;
boolean fShowAckWait;

byte SerRx();
void SerTx(byte b);
byte SerRxChk();
void fill();
void fill_abort();
void cmd_lookup();
void send_msg(char *s);
void ACK();
void SerCRLF();
void R();
void S(); 
void C();
void E();
void I();
void V();
void d();
void f();
void z();
void t();
void _1();
void _2();
void C04();
void C08();
void C16();
void C32();
void x();
void info();
void tx_hex_byte(byte b);
void eeprom_write_byte(byte deviceaddress, int eeaddress, byte data);
void eeprom_write_pages(byte deviceaddress, unsigned eeaddr, unsigned length, const byte * data);
int eeprom_read_byte(byte deviceaddress, unsigned eeaddr);
byte get_addr_len();

void setup() {
    Serial.begin(38400);
    Wire.begin();

    fTwoByteAddress = false;
    fFillTestPattern = false;
    fShowAckWait = false;

    C16();
}
    


void loop() {
   jp1_cmd = (byte) SerRx();
   jp1_chk = jp1_cmd;
   fShowAckWait = false;

   cmd_lookup();
}


void cmd_lookup() {
   switch ((char) jp1_cmd) {
      case 'R': R(); break;
      case 'S': S(); break;
      case 'C': C(); break;
      case 'E': E(); break;
      case 'I': I(); break;
      case 'V': V(); break;
      case 'd': d(); break;
      case 'f': f(); break;
      case 'z': z(); break;
      case 't': t(); break;
      case '1': _1(); break;
      case '2': _2(); break;
      case '4': C04(); break;
      case '8': C08(); break;
      case '6': C16(); break;
      case '3': C32(); break;
      case 'x': x(); break;
      case '?': info(); break;
      default: break;
    };
};


void send_msg(char *s) {
    int i = 0;
    while (s[i]) {
        jp1_chk ^= s[i];
        SerTx(s[i]);
        i++;
    }
}


void ACK() {
   SerTx(6);
}


//  --- Ping
//  -> E
//  <- <ACK>
void E() {
   ACK();
}

// --- Identity
// -> I
// <- [data] <chk>
void I() {
    jp1_chk = 0;
    send_msg("eeprom");
    SerTx(jp1_chk);
}

// --- Version
// -> V
// <- [data] <chk>
//
// - Flash size is in units of 128 bytes. Get max MSB of EE address * 2
// - EEPROM base address MSB
// - EEPROM base address LSB
void V() {
    jp1_chk = 2 * ee_size;
    SerTx(jp1_chk);
    SerTx(0);
    SerTx(0);
    SerTx(jp1_chk);
}


// --- Read
// -> R <addrh> <addrl> <len> <chk>
// <- r [data] <chk>

void R() {
   jp1_chk=(byte)'R';
   get_addr_len();
   byte chk = SerRx();
   if ( chk ^ jp1_chk ) {
       // bad_checksum
       return;
   }

   jp1_chk = (byte)'r';
   SerTx(jp1_chk);

   for (int i=0; i<jp1_len; i++) {
        byte b = eeprom_read_byte(DEVADDR, i + addrl + 256*addrh);
        jp1_chk ^= b;
        SerTx(b);
   }
   SerTx(jp1_chk);
}


// --- Write
// -> S <addrh> <addrl> <len> [data] <chk>
// <- <ACK>

void S() {
    jp1_chk=(byte)'S';
    byte b;
    get_addr_len();
    
    for (int i=0; i<jp1_len; i++) {
        b = SerRxChk();
        data_buf[i]=b;
    }
    b = SerRx();
    if (b ^ jp1_chk) {
        // bad_checksum
        return;
    }
    ACK();
    eeprom_write_pages(DEVADDR, addrl + addrh * 256, jp1_len, data_buf);
}


// --- Erase
// -> C <addrh> <addrl> <counth> <countl> <chk>
// <- <ACK>
void C() {
   get_addr_len();
   countl = SerRx();
   jp1_chk ^= countl;

   byte b = SerRx();
   if (b ^ jp1_chk) {
       // bad_checksum
   } else {
       // add code to clear EEPROM here
       ACK();
   }
}


// --- d - Display EEPROM contents
// code for content dump taken from davekw7x (not from K. Timmerman code)													;
void d() {
    SerCRLF();
    
    unsigned numPages  = (ee_size * 2 * 256) / 16;
    
    for (unsigned i = 0; i < numPages; i++) {
        byte buffer[16]; // Hold a page of EEPROM
        char outbuf[6];  //Room for three hex digits and ':' and ' ' and '\0'
        sprintf(outbuf, "%03x: ", i);
        Serial.print(outbuf);
        for (int j = 0; j < 16; j++) {
            buffer[j]=eeprom_read_byte(DEVADDR, j+i*16);
            if (j == 8) {
                Serial.print(" ");
            }
            sprintf(outbuf, "%02x ", buffer[j]);
            Serial.print(outbuf);            
        }
        Serial.print(" |");
        for (int j = 0; j < 16; j++) {
            if (isprint(buffer[j])) {
                Serial.print(buffer[j]);
            }
            else {
                Serial.print('.');
            }
        }
        Serial.println("|");
    }
    SerCRLF();
}


// 1 - One byte addressing
void _1() {
    fTwoByteAddress = false;
    ACK();
}

// 2 - Two byte addressing
void _2() {
     fTwoByteAddress=true;
     ACK();
}

// 4 - 24C04 - 512 byte EEPROM
void C04() {
   ee_size = 0x02;
   _1();
}

// 8 - 24C08 - 1K byte EEPROM
void C08() {
   ee_size = 0x04;
   _1();
}

// 6 - 24C16 - 2K byte EEPROM
void C16() {
   ee_size = 0x08;
   _1();
}

// 3 - 24C32 - 4K byte EEPROM
void C32() {
   ee_size = 0x10;
   _2();
}

// f - Fill EEPROM with 0xFF
void f() {
    ee_fill = 0xFF;
    fFillTestPattern = false;
    fill();
}

// z - Fill EEPROM with 0x00
void z() {
    ee_fill = 0;
    fFillTestPattern = false;
    fill();
}

// t - Fill EEPROM with test pattern
void t() {
     fFillTestPattern = true;
     fill();
}

// x - Release remote reset
void x() {
}


// ? - Product information
void info() {
    SerCRLF();
    send_msg("JP1 EEPROM Programming Adapter\r\n");
    send_msg("Copyright 2010 Kevin Timmerman\r\n");
    send_msg("Ported to Arduino 2012 Tim6502\r\n");
    send_msg("Rev 003 Config ");
    tx_hex_byte(ee_size);
    SerTx(' ');
    SerTx( (fTwoByteAddress) ? '2' : '1' );
    SerCRLF();
    SerCRLF();
    send_msg("d Display EEPROM contents\r\n");
    send_msg("f Fill with FF\r\n");
    send_msg("z Fill with 00\r\n");
    send_msg("t Fill with test pattern\r\n");
    send_msg("4 24C04\r\n");
    send_msg("8 24C08\r\n");
    send_msg("6 24C16\r\n");
    send_msg("3 24C32");
    SerCRLF();
}

// --- Fill EEPROM with ee_fill
void fill() {
    send_msg("Fill EEPROM with ");
    if (fFillTestPattern) {
        send_msg("test pattern");
    } else {
        tx_hex_byte(ee_fill);
    }
    SerTx(' ');
    SerTx('?');
    SerCRLF();

    byte b = SerRx();
    if (b != (byte)'Y') {
        fill_abort();
        return;
    }

    addrl = 0;
    addrh = 0;
    fShowAckWait = true;
    
    SerCRLF();
    tx_hex_byte(addrh);
    tx_hex_byte(addrl);

    unsigned address=addrl + 256*addrh;
    
    while (address < ee_size*256) {
        if (fFillTestPattern) {
            eeprom_write_byte(DEVADDR, address, ee_fill);  
        } else {
            eeprom_write_byte(DEVADDR, address, addrl+addrh);
            addrl++;
            if (addrl==0) addrh++;  
        }
    }
    SerCRLF();
}


// - Abort fill
void fill_abort() {
   send_msg("Fill Canceled");
   SerCRLF();
}


// --- Get address and length from host, update checksum
byte get_addr_len() {
     addrh = SerRxChk();
     addrl = SerRxChk();
     jp1_len = SerRxChk();
}

// - CR LF
void SerCRLF() {
   SerTx(13);
   SerTx(10);
}

// - Send byte as ASCII hex
void tx_hex_byte(byte b) {
   byte nib = b / 16;
   SerTx(hex[nib]);
   nib = b % 16;
   SerTx(hex[nib]); 
}

void SerTx(byte b) {
   Serial.write(b);
}

byte SerRx() {
   int b = -1;
   while (b < 0) {
     while (Serial.available() < 0) {
        // wait here
     };
     b = Serial.read();
   }
   return (byte) b;
}

byte SerRxChk() {
  byte b = SerRx();
  jp1_chk = (b ^ jp1_chk);
  return b; 
}


void eeprom_write_byte(byte deviceaddress, int eeaddress, byte data)
{
    // Three lsb of Device address byte are bits 8-10 of eeaddress
    byte devaddr = deviceaddress | ((eeaddress >> 8) & 0x07);
    byte addr    = eeaddress;
    Wire.beginTransmission(devaddr);
    Wire.write(int(addr));
    Wire.write(int(data));
    Wire.endTransmission();
    delay(10);
}

int eeprom_read_byte(byte deviceaddress, unsigned eeaddr)
{
    byte rdata = -1;

    // Three lsb of Device address byte are bits 8-10 of eeaddress
    byte devaddr = deviceaddress | ((eeaddr >> 8) & 0x07);
    byte addr    = eeaddr;

    Wire.beginTransmission(devaddr);
    Wire.write(int(addr));
    Wire.endTransmission();
    Wire.requestFrom(int(devaddr), 1);
    if (Wire.available()) {
        rdata = Wire.read();
    }
    return rdata;
}


// Pages are blocks of 16 bytes, starting at 0x000.
// That is, pages start at 0x000, 0x010, 0x020, ...
// For a device "page write", the last byte must be
// on the same page as the first byte.
void eeprom_write_pages(byte deviceaddress, unsigned eeaddr, unsigned length, const byte * data)
{
    unsigned count=0;
    while (count < length) {
       // Three lsb of Device address byte are bits 8-10 of eeaddress
       byte devaddr = deviceaddress | ((eeaddr >> 8) & 0x07);
       byte addr    = eeaddr;
       byte nibble  = addr & 0x07;
       byte i = 0;
       
       Wire.beginTransmission(devaddr);
       Wire.write(int(addr));
       do {
          Wire.write(data[count]);
          count++; i++;
       } while (count < length && (((nibble + i) & 0x07) != 0)); 
       eeaddr += i;
       Wire.endTransmission();
       delay(10);
    }

}
