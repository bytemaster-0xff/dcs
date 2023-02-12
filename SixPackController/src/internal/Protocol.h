#ifndef __DCSBIOS_PROTOCOL_H
#define __DCSBIOS_PROTOCOL_H

#define DCSBIOS_STATE_WAIT_FOR_SYNC 0
#define DCSBIOS_STATE_ADDRESS_LOW 1
#define DCSBIOS_STATE_ADDRESS_HIGH 2
#define DCSBIOS_STATE_COUNT_LOW 3
#define DCSBIOS_STATE_COUNT_HIGH 4
#define DCSBIOS_STATE_DATA_LOW 5
#define DCSBIOS_STATE_DATA_HIGH 6


#define DCSBIOS_STATE_EXPECT_CMD 7
#define DCSBIOS_STATE_EXPECT_CMD_ADDR_LOW 8
#define DCSBIOS_STATE_EXPECT_CMD_ADDR_HIGH 9
#define DCSBIOS_STATE_EXPECT_CMD_VALUE_LOW 10
#define DCSBIOS_STATE_EXPECT_CMD_VALUE_HIGH 11

#include "ExportStreamListener.h"
#include "RingBuffer.h"

namespace DcsBios {

	class ProtocolParser {
		private:
			volatile unsigned char cmd;
			volatile unsigned int cmdAddress;
			volatile int cmdValue;

			volatile unsigned char state;
			volatile unsigned int address;
			volatile unsigned int count;
			volatile unsigned int data;
			volatile unsigned char sync_byte_count;
			volatile unsigned char sys_byte_count;

			
			void (*callback)(unsigned char cmd, unsigned int address, int value);
			ExportStreamListener* startESL;
			RingBuffer<64> incomingDataBuffer;
			volatile bool processingData;
		public:
			void processChar(unsigned char c);
			void processCharISR(unsigned char c);
			void RegisterCommandCallback(void cb(unsigned char cmd, unsigned int address, int value)){
				callback = cb;
			}

			ProtocolParser();
	};
}

#endif 