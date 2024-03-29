#define DEBUGGING

//MS:


#include "WebSocketClient.h"

#include "sha1.h"
#include "Base64.h"


bool WebSocketClient::handshake(Client &client) {

    socket_client = &client;

    // If there is a connected client->
    if (socket_client->connected()) {
        // Check request and look for websocket handshake
#ifdef DEBUGGING
            Serial.println(F("Client connected"));
#endif
        if (analyzeRequest()) {
#ifdef DEBUGGING
                Serial.println(F("Websocket established"));
#endif

                return true;

        } else {
            // Might just need to break until out of socket_client loop.
#ifdef DEBUGGING
            Serial.println(F("Invalid handshake"));
#endif
            disconnectStream();

            return false;
        }
    } else {
        return false;
    }
}

bool WebSocketClient::analyzeRequest() {
    String temp;

    int bite;
    bool foundupgrade = false;
    unsigned long intkey[2];
    String serverKey;
    char keyStart[17];
    char b64Key[25];
    String key = "------------------------";

    randomSeed(analogRead(0));

    for (int i=0; i<16; ++i) {
        keyStart[i] = (char)random(1, 256);
    }

    base64_encode(b64Key, keyStart, 16);

    for (int i=0; i<24; ++i) {
        key[i] = b64Key[i];
    }

#ifdef DEBUGGING
    Serial.println(F("Sending websocket upgrade headers"));
    Serial.println(F("------"));
    Serial.print(F("GET "));
    Serial.print(path);
    Serial.print(F(" HTTP/1.1\r\n"));
    Serial.print(F("Upgrade: websocket\r\n"));
    Serial.print(F("Connection: Upgrade\r\n"));
    Serial.print(F("Host: "));    
    Serial.print(host);
    Serial.print(F(":5010"));
    Serial.print(CRLF); 
    Serial.print(F("Origin: http://localhost:4200"));
    Serial.print(CRLF); 
    Serial.print(F("Sec-WebSocket-Key: "));
    Serial.print(key);
    Serial.print(CRLF);
    Serial.print(F("Sec-WebSocket-Protocol: "));
    Serial.print(protocol);
    Serial.print(CRLF);
    Serial.print(F("Sec-WebSocket-Version: 13\r\n"));
    Serial.print(CRLF);
#endif    

    socket_client->print(F("GET "));
    socket_client->print(path);
    socket_client->print(F(" HTTP/1.1\r\n"));
    socket_client->print(F("Upgrade: websocket\r\n"));
    socket_client->print(F("Connection: Upgrade\r\n"));
    socket_client->print(F("Host: "));
    socket_client->print(host);
    socket_client->print(":5010");
    socket_client->print(CRLF); 
    socket_client->print(F("Origin: http://localhost:4200"));
    socket_client->print(CRLF); 
    socket_client->print(F("Sec-WebSocket-Key: "));
    socket_client->print(key);
    socket_client->print(CRLF);
    socket_client->print(F("Sec-WebSocket-Protocol: "));
    socket_client->print(protocol);
    socket_client->print(CRLF);
    socket_client->print(F("Sec-WebSocket-Version: 13\r\n"));
    socket_client->print(CRLF);

#ifdef DEBUGGING
    Serial.println(F("Analyzing response headers"));
#endif

    while (socket_client->connected() && !socket_client->available()) {
        delay(100);
#ifdef DEBUGGING
        Serial.println("Waiting...");
#endif
    }

    // TODO: More robust string extraction
    while ((bite = socket_client->read()) != -1) {

        temp += (char)bite;

        if ((char)bite == '\n') {
#ifdef DEBUGGING
            Serial.print("Got Header: " + temp);
#endif
            if (!foundupgrade && temp.startsWith("Upgrade: websocket")) {
                foundupgrade = true;
            } else if (temp.startsWith("Sec-Websocket-Accept: ") || temp.startsWith("Sec-WebSocket-Accept: ")) {
                serverKey = temp.substring(22,temp.length() - 2); // Don't save last CR+LF
            }
            temp = "";		
        }

        if (!socket_client->available()) {
          delay(20);
        }
    }

    key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    uint8_t *hash;
    char result[21];
    char b64Result[30];

    SHA1Context sha;
    int err;
    uint8_t Message_Digest[20];
    
    err = SHA1Reset(&sha);
    err = SHA1Input(&sha, reinterpret_cast<const uint8_t *>(key.c_str()), key.length());
    err = SHA1Result(&sha, Message_Digest);
    hash = Message_Digest;

    for (int i=0; i<20; ++i) {
        result[i] = (char)hash[i];
    }
    result[20] = '\0';

    base64_encode(b64Result, result, 20);

    // if the keys match, good to go
    return serverKey.equals(String(b64Result));
}


bool WebSocketClient::handleStream(String& data, uint8_t *opcode) {
    uint8_t msgtype;
    uint8_t bite;
    unsigned int length;
    uint8_t mask[4];
    uint8_t index;
    unsigned int i;
    bool hasMask = false;

    if (!socket_client->connected() || !socket_client->available())
    {
        return false;
    }      

    msgtype = timedRead();
    if (!socket_client->connected()) {
        return false;
    }

    length = timedRead();

    if (length & WS_MASK) {
        hasMask = true;
        length = length & ~WS_MASK;
    }


    if (!socket_client->connected()) {
        return false;
    }

    index = 6;

    if (length == WS_SIZE16) {
        length = timedRead() << 8;
        if (!socket_client->connected()) {
            return false;
        }
            
        length |= timedRead();
        if (!socket_client->connected()) {
            return false;
        }   

    } else if (length == WS_SIZE64) {
#ifdef DEBUGGING
        Serial.println(F("No support for over 16 bit sized messages"));
#endif
        return false;
    }

    if (hasMask) {
        // get the mask
        mask[0] = timedRead();
        if (!socket_client->connected()) {
            return false;
        }

        mask[1] = timedRead();
        if (!socket_client->connected()) {

            return false;
        }

        mask[2] = timedRead();
        if (!socket_client->connected()) {
            return false;
        }

        mask[3] = timedRead();
        if (!socket_client->connected()) {
            return false;
        }
    }
        
    data = "";
        
    if (opcode != NULL)
    {
      *opcode = msgtype & ~WS_FIN;
    }
                
    if (hasMask) {
        for (i=0; i<length; ++i) {
            data += (char) (timedRead() ^ mask[i % 4]);
            if (!socket_client->connected()) {
                return false;
            }
        }
    } else {
        for (i=0; i<length; ++i) {
            data += (char) timedRead();
            if (!socket_client->connected()) {
                return false;
            }
        }            
    }
    
    return true;
}

uint16_t WebSocketClient::handleStream(uint8_t *data, uint16_t buffer_size, uint8_t *opcode) {
    uint8_t msgtype;
    uint8_t bite;
    unsigned int length;
    uint8_t mask[4];
    uint8_t index;
    unsigned int i;
    bool hasMask = false;
    int idx = 0;

    if (!socket_client->connected())
    {
        Serial.println("no socket connect 0");
    }

    if(!socket_client->available())
    {
        Serial.println("Not Data Available");
        return 0;
    }      

    msgtype = timedRead();
    if (!socket_client->connected()) {
        Serial.println("no socket connect 1");
        return 0;
    }

    length = timedRead();

    if (length & WS_MASK) {
        hasMask = true;
        length = length & ~WS_MASK;
    }


    if (!socket_client->connected()) {
        Serial.println("no socket connect 2");
        return 0;
    }

    index = 6;

    if (length == WS_SIZE16) {
        length = timedRead() << 8;
        if (!socket_client->connected()) {
            Serial.println("no socket connect 3");
            return 0;
        }
            
        length |= timedRead();
        if (!socket_client->connected()) {
            Serial.println("no socket connect 4");
            return 0;
        }   

    } else if (length == WS_SIZE64) {
#ifdef DEBUGGING
        Serial.println(F("No support for over 16 bit sized messages"));
#endif
        return 0;
    }

    if(length == 0) {
        Serial.println(F("Length 0 received."));
        return 0;
    }

    if (hasMask) {
        // get the mask
        mask[0] = timedRead();
        if (!socket_client->connected()) {
            return 0;
        }

        mask[1] = timedRead();
        if (!socket_client->connected()) {

            return 0;
        }

        mask[2] = timedRead();
        if (!socket_client->connected()) {
            return 0;
        }

        mask[3] = timedRead();
        if (!socket_client->connected()) {
            return 0;
        }
    }

    Serial.println("Got these bytes" + String(length));;
            
        
    if (opcode != NULL)
    {
      *opcode = msgtype & ~WS_FIN;
    }
                
    if (hasMask) {
        for (i=0; i<length; ++i) {
            data[idx++] = (uint8_t) (timedRead() ^ mask[i % 4]);
            if (!socket_client->connected()) {
                return 0;
            }

            if(idx == buffer_size) 
                return idx;
        }
    } else {
        for (i=0; i<length; ++i) {
            data[idx++] = (uint8_t) timedRead();

            if (!socket_client->connected()) {
                return 0;
            }

            if(idx == buffer_size) 
                return idx;
        }            
    }
    
    return idx;
}

void WebSocketClient::disconnectStream() {
#ifdef DEBUGGING
    Serial.println(F("Terminating socket"));
#endif
    // Should send 0x8700 to server to tell it I'm quitting here.
    socket_client->write((uint8_t) 0x87);
    socket_client->write((uint8_t) 0x00);
    
    socket_client->flush();
    delay(10);
    socket_client->stop();
}

bool WebSocketClient::getData(String& data, uint8_t *opcode) {
    return handleStream(data, opcode);
}    

uint16_t WebSocketClient::getData(uint8_t *data, uint16_t buffer_size, uint8_t *opcode) {
    return handleStream(data, buffer_size, opcode);
}    

void WebSocketClient::sendData(const char *str, uint8_t opcode) {
#ifdef DEBUGGING
    Serial.print(F("Sending data: "));
    Serial.println(str);
#endif
    if (socket_client->connected()) {
        sendEncodedData(str, opcode);       
    }
}

void WebSocketClient::sendData(String str, uint8_t opcode) {
#ifdef DEBUGGING
    Serial.print(F("Sending data: "));
    Serial.println(str);
#endif
    if (socket_client->connected()) {
        sendEncodedData(str, opcode);
    }
}

int WebSocketClient::timedRead() {
  while (!socket_client->available()) {
    delay(20);  
  }

  return socket_client->read();
}

void WebSocketClient::sendEncodedData(char *str, uint8_t opcode) {
    uint8_t mask[4];
    uint8_t frameBuffer[128];
    size_t bufferIndex = 0;
    size_t bytesWritten = 0;
    int size = strlen(str);

    // Opcode; final fragment
    frameBuffer[bufferIndex] = (opcode | WS_FIN);
    bufferIndex++;
    
    // NOTE: no support for > 16-bit sized messages
    if (size > 125) {
        frameBuffer[bufferIndex] = (WS_SIZE16 | WS_MASK);
        frameBuffer[bufferIndex + 1] = ((uint8_t) (size >> 8));
        frameBuffer[bufferIndex + 2] = ((uint8_t) (size & 0xFF));
        bufferIndex += 3;
    } else {
        frameBuffer[bufferIndex] = ((uint8_t) size | WS_MASK);
        bufferIndex++;
    }

    mask[0] = random(0, 256);
    mask[1] = random(0, 256);
    mask[2] = random(0, 256);
    mask[3] = random(0, 256);
    
    frameBuffer[bufferIndex] = (mask[0]);
    frameBuffer[bufferIndex + 1] = (mask[1]);
    frameBuffer[bufferIndex + 2] = (mask[2]);
    frameBuffer[bufferIndex + 3] = (mask[3]);
    bufferIndex += 4;
     
    for (int i=0; i<size; ++i) {
        frameBuffer[bufferIndex] = (str[i] ^ mask[i % 4]);
        bufferIndex++;
        if (bufferIndex == 128) {
            bytesWritten += socket_client->write(frameBuffer, bufferIndex);
#ifdef DEBUGGING
            Serial.print("Buffer size: ");
            Serial.print(bufferIndex);
            Serial.print(" Bytes Sent: ");
            Serial.println(bytesWritten);
#endif
            bufferIndex = 0;
        }
    }
    if (bufferIndex > 0) {
        bytesWritten += socket_client->write(frameBuffer, bufferIndex);
    #ifdef DEBUGGING
        Serial.print("Buffer size: ");
        Serial.print(bufferIndex);
        Serial.print(" Bytes Sent: ");
        Serial.println(bytesWritten);
    #endif
    }
}

void WebSocketClient::sendEncodedData(String str, uint8_t opcode) {
    int size = str.length() + 1;
    char cstr[size];

    str.toCharArray(cstr, size);

    sendEncodedData(cstr, opcode);
}
