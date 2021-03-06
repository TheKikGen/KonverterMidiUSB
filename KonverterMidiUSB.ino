/*
 *******************************************************************************
 * Serial MIDI and USB Host bidirectional converter
 * TheKikGen Labs. 2018.  
 * Inspired from a sample code by Yuuichi Akagawa.
 *******************************************************************************
 
 TESTED AND WORKING WITH :
 - Arduino UNO genuine and clones
 - USB_Host_Shield_2.0 official library
 - Keyes SUB host shield (www.funduino.com)

 You need to deploy the midiXParser library into your Aruino library directory.
 Download is here : https://github.com/TheKikGen/midiXparser 
 */

#include <midiXparser.h>
#include <usbh_midi.h>
#include <usbhub.h>

// Midi serial parser
midiXparser midiSerial;

#define LED 13

USB Usb;
USBH_MIDI usbMidi(&Usb);


//////////////////////////////////////////////////////////////////////////////
// Scan and parse sysex flows
// ----------------------------------------------------------------------------
// We use the midiXparser 'on the fly' mode, allowing to tag bytes as "captured"
// when they belong to a midi SYSEX message, without storing them in a buffer.
// In that way, we can proceed infinite sysex size !
// SYSEX Error (not correctly terminated by 0xF7 for example) are cleaned up,
// to restore a correct parsing state.
///////////////////////////////////////////////////////////////////////////////

void scanMidiSerialSysExToUsb( midiXparser* serialMidiParser ) {

  static uint8_t MIDIPacket[4]={0,0,0,0};
  static uint8_t packetLen = 0 ;

  byte readByte = serialMidiParser->getByte();
  
  // Normal End of SysEx or : End of SysEx with error.
  // Force clean end of SYSEX as the midi usb driver
  // will not understand if we send the packet as is
  if ( serialMidiParser->wasSysExMode() ) {
      packetLen++;
      MIDIPacket[packetLen] = midiXparser::eoxStatus;

      // CIN = 5/6/7  sysex ends with one/two/three bytes,
      MIDIPacket[0] = packetLen + 4 ;
      usbMidi.SendRawData(4,MIDIPacket);

      packetLen = 0;
      memset(MIDIPacket,0,4);

      return;
  } else

  // Fill USB sysex packet
  if ( serialMidiParser->isSysExMode() ) {
    packetLen++;
    MIDIPacket[packetLen] = readByte ;  

    // Packet complete ?
    if (packetLen == 3 ) {
        MIDIPacket[0] =  4 ; // Sysex start or continue
        usbMidi.SendRawData(4,MIDIPacket);
        packetLen = 0;
        memset(MIDIPacket,0,4);
    }

  }
    
}


///////////////////////////////////////////////////////////////////////////////
// Scan Serial port for MIDI data
// ----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

void serialMidiPoll() {

  if (Serial.available() ) {
    digitalWrite(LED,HIGH);

    // Echo to Midi OUT serial. No need to parse
    byte readByte = Serial.read();
    Serial.write(readByte);
    
    // Parsing is for USB
    if (midiSerial.parse( readByte ) ) {

        // We manage sysEx "on the fly". Clean end of a sysexe msg ?
        if ( midiSerial.getMidiMsgType() == midiXparser::sysExMsgTypeMsk )
          scanMidiSerialSysExToUsb(&midiSerial) ;

        else // Not a sysex. The message is complete.
          usbMidi.SendData(midiSerial.getMidiMsg());
    }
    else
    // Acknowledge any sysex error
    if ( midiSerial.isSysExError() )
        scanMidiSerialSysExToUsb(&midiSerial) ;
    else
    // Check if a SYSEX mode active and send bytes on the fly.
      if ( midiSerial.isSysExMode() && midiSerial.isByteCaptured() ) {
        scanMidiSerialSysExToUsb(&midiSerial) ;
    }
   
  }
}

///////////////////////////////////////////////////////////////////////////////
// Read a MIDI USB event and send it to the USART.
// ----------------------------------------------------------------------------
// USB MIDI will do all the parsing stuff for us. Including sysex.
// We just need to get the length of the MIDI message embedded in the packet
// ex : Note-on message on virtual cable 1 (CN=0x1; CIN=0x9) 	19 9n kk vv => 9n kk vv
///////////////////////////////////////////////////////////////////////////////
static void sendUSBMidiToSerial(uint8_t *MIDIPacket)
{

	/* Process a MIDI command that has been received */
	uint8_t CIN = *MIDIPacket & 0x0F;
	switch (CIN) {
					// 1 byte
					case 0x05: case 0x0F:
						Serial.write(*(MIDIPacket+1));
						break;

					// 2 bytes
					case 0x02: case 0x06: case 0x0C: case 0x0D:
						Serial.write(MIDIPacket+1,2);
						break;

					// 3 bytes
					case 0x03: case 0x07: case 0x04: case 0x08:
					case 0x09: case 0x0A: case 0x0B: case 0x0E:
						Serial.write(MIDIPacket+1,3);
						break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Poll USB MIDI Controler and send to serial MIDI
// ----------------------------------------------------------------------------
// The received buffer can contain several MIDI packets
///////////////////////////////////////////////////////////////////////////////

void usbMidiPoll()
{
  uint8_t   recvBuf[MIDI_EVENT_PACKET_SIZE];
  uint8_t   p = 0;
  uint16_t  rcvd;

  
  if ( usbMidi.RecvData( &rcvd, recvBuf) != 0 ) return;

  while (p < MIDI_EVENT_PACKET_SIZE)  { 
    if ( recvBuf[p] == 0 && recvBuf[p+1] == 0 ) return ;   
    sendUSBMidiToSerial(&recvBuf[p]);        
    p += 4;
    serialMidiPoll();  // Let a chance to serial here if several packets received
  }
}
///////////////////////////////////////////////////////////////////////////////
// Generate a microsec delay
// ----------------------------------------------------------------------------
// Delay time max is 16383 us
///////////////////////////////////////////////////////////////////////////////
void uSecDelay(unsigned long t1, unsigned long t2, unsigned long delayTime)
{
  unsigned long t3;

  if ( t1 > t2 ) {
    t3 = (0xFFFFFFFF - t1 + t2);
  } else {
    t3 = t2 - t1;
  }

  if ( t3 < delayTime ) {
    delayMicroseconds(delayTime - t3);
  }
}

///////////////////////////////////////////////////////////////////////////////
// SETUP
// ----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

void setup()
{

  Serial.begin(31250);      // Midi baud rate
 // midiSerial.setMidiChannelFilter(midiXparser::allChannel);
  midiSerial.setMidiMsgFilter( midiXparser::allMsgTypeMsk );
 // midiSerial.setSysExFilter(true,0); // Sysex on the fly

   digitalWrite(LED,HIGH);
  // Initialize USB. Plug and play approach.
    while (Usb.Init() == -1 ) {
    usbMidi.Release();
    delay(5000); // Wait 5 seconds before a new try
  }
  // Insure everything is initialized : let time
  delay( 5000 );

}

///////////////////////////////////////////////////////////////////////////////
// LOOP
// ----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
void loop()
{
  
  Usb.Task();
  unsigned long t1 = micros();
  
  if ( Usb.getUsbTaskState() == USB_STATE_RUNNING )
  {
    serialMidiPoll();  
    usbMidiPoll();  // NB : Includes also a serialMidiPoll() call.
  }
  //delay(1ms)
  uSecDelay(t1, micros(), 1000);
  digitalWrite(LED,LOW);

}
