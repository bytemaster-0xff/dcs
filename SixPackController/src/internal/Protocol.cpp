#include "Protocol.h"
#include "PollingInput.h"
#include "ExportStreamListener.h"
#include "RingBuffer.h"

namespace DcsBios {
	ExportStreamListener* ExportStreamListener::firstExportStreamListener = NULL;
	PollingInput* PollingInput::firstPollingInput = NULL;	

	ProtocolParser::ProtocolParser() {
		processingData = false;
		state = DCSBIOS_STATE_WAIT_FOR_SYNC;
		sync_byte_count = 0;
		sys_byte_count = 0;
	}

	/*
		to be called from ISR
		stores the character in a buffer, re-enables interrupts and processes the complete
		buffer before returning to the main program.
	*/
	void ProtocolParser::processCharISR(unsigned char c) {
		incomingDataBuffer.put(c);
		if (processingData) return;
		
		processingData = true;
		while (1) {
			if (incomingDataBuffer.isEmpty()) {
				processingData = false;
				return;
			}
			unsigned char nextByte = incomingDataBuffer.get();
			interrupts();
			processChar(nextByte);
			noInterrupts();
		}
	}
	
	void ProtocolParser::processChar(unsigned char c) {
	  //Serial.println(String(state) + " " + String(c) + " " + String(sys_byte_count) + " " + String(sync_byte_count) + " " + String(count));
	

	  switch(state) {
		case DCSBIOS_STATE_WAIT_FOR_SYNC:
			/* do nothing */

			break;
			
		case DCSBIOS_STATE_ADDRESS_LOW:
			address = (unsigned int)c;
			state = DCSBIOS_STATE_ADDRESS_HIGH;

			break;
			
		case DCSBIOS_STATE_ADDRESS_HIGH:
			address = (c << 8) | address;
			if (address != 0x5555) {
				state = DCSBIOS_STATE_COUNT_LOW;				
			} else {
				state = DCSBIOS_STATE_WAIT_FOR_SYNC;
			}
			break;
			
		case DCSBIOS_STATE_COUNT_LOW:
			count = (unsigned int)c;
			state = DCSBIOS_STATE_COUNT_HIGH;
			break;
			
		case DCSBIOS_STATE_COUNT_HIGH:
			count = (c << 8) | count;
			if(count > 0 && count < 500){
				state = DCSBIOS_STATE_DATA_LOW;
			}
			else {
				Serial.println("OOPS  " + String(count));
				state = DCSBIOS_STATE_WAIT_FOR_SYNC;
			    sys_byte_count = 0;
				sync_byte_count = 0;
				count = 0;
			}
			break;
			
		case DCSBIOS_STATE_DATA_LOW:
			if(count < 0 || count > 200){
				Serial.println("OOPS  " + String(count));
				state = DCSBIOS_STATE_WAIT_FOR_SYNC;
			    sys_byte_count = 0;
				sync_byte_count = 0;
				count = 0;
			}
			else 
			{
				data = (unsigned int)c;
				count--;
				state = DCSBIOS_STATE_DATA_HIGH;
			}
			break;
			
		case DCSBIOS_STATE_DATA_HIGH:
			data = (c << 8) | data;
			count--;
			

			//ExportStreamListener::handleDcsBiosWrite(address, data);
			// skip all ESLs that cannot possibly be interested in the current address
			while(startESL && startESL->getLastAddressOfInterest() < address) {
				startESL->onConsistentData(); // inform the ExportStreamListener that no more data is following in this update
				startESL = startESL->nextExportStreamListener;
			}
			if (startESL) {
				ExportStreamListener* el = startESL;
				// assumption: no overlapping ESL ranges
				while(el) {
					if (el->getFirstAddressOfInterest() > address) break;
					if (el->getFirstAddressOfInterest() <= address && el->getLastAddressOfInterest() >= address) {
					
						el->onDcsBiosWrite(address, data);
					}
					el = el->nextExportStreamListener;
				}
			}

			
			address += 2;
			if (count == 0)
				state = DCSBIOS_STATE_ADDRESS_LOW;
			else
				state = DCSBIOS_STATE_DATA_LOW;
			break;

		case DCSBIOS_STATE_EXPECT_CMD:
			cmd = c;
			break;
		case DCSBIOS_STATE_EXPECT_CMD_ADDR_LOW:
			cmdAddress = (unsigned int)c;
			break;
		case DCSBIOS_STATE_EXPECT_CMD_ADDR_HIGH:
			cmdAddress = ((int)c << 8) | cmdAddress;
			break;			
		case DCSBIOS_STATE_EXPECT_CMD_VALUE_LOW:			
			cmdValue = (int)cmdValue;
			break;
		case DCSBIOS_STATE_EXPECT_CMD_VALUE_HIGH:
				cmdValue = ((int)c << 8) | cmdValue;
				state = DCSBIOS_STATE_WAIT_FOR_SYNC;
				callback(cmd, cmdAddress, cmdValue);
				break;	
		}
		
		if(state == DCSBIOS_STATE_WAIT_FOR_SYNC)
		{			
			if(c == 0x33) {
				sys_byte_count++;
				sync_byte_count = 0;
			}
			else
			{
				sys_byte_count = 0;
			}

			if (c == 0x55) {
				sync_byte_count++;
				sys_byte_count = 0;
			}
			else
				sync_byte_count = 0;

			if(sys_byte_count == 4) {
				state = DCSBIOS_STATE_EXPECT_CMD;
				sys_byte_count = 0;
			}

			if (sync_byte_count == 4) {
				state = DCSBIOS_STATE_ADDRESS_LOW;
				sync_byte_count = 0;
				startESL = ExportStreamListener::firstExportStreamListener;
			}		
		}
	}
}