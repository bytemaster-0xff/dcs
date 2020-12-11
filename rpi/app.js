const WebSocket = require('ws');
const aircraftProfile = require("./Profiles/P51D.json");

console.log("DCS Remote Client Started");

const ws = new WebSocket('ws://10.1.1.57:5010/api/websocket');

let state = "WAIT_FOR_SYNC";
let sync_byte_count = 0;
var address_buffer = new ArrayBuffer(2);
var address_uint8 = new Uint8Array(address_buffer);
var address_uint16 = new Uint16Array(address_buffer);

var count_buffer = new ArrayBuffer(2);
var count_uint8 = new Uint8Array(count_buffer);
var count_uint16 = new Uint8Array(count_buffer);

var data_buffer = new ArrayBuffer(2);
var data_uint8 = new Uint8Array(data_buffer);
var data_uint16 = new ArrayBuffer(data_buffer);

var listeners = [];

var dataValues = [];

var batAddress = aircraftProfile["Right Switch Panel"];
var batMask = aircraftProfile["Right Switch Panel"]["BAT"];
//console.log(batAddress, batMask);
console.log(batMask.outputs[0].address.toString(16), batMask.outputs[0].mask);

function notifyData(address, buffer){
    var byteBuffer = new Uint16Array(buffer);
   
    for(let listener of listeners){
        if(listener.address == address) {
            if(listener.callback){
                const output = listener.system.outputs[0]; 
                const value = (byteBuffer[0] & output.mask) >> output.shift_by;
                listener.callback(value)
            }
        }
    }

    dataValues[address] = buffer;
}

function notifyEndOfUpdate() {

}

function registerListener(category, parameter, callback){
    var system = aircraftProfile[category][parameter];
    if(!system) {
        console.error("Could not find sytem: " + category + " " + parameter);
    }
    listeners.push({
            'address': system.outputs[0].address,
            'system' : system,
            'callback' : callback
        }
    );
}

registerListener("Right Switch Panel","BAT", function(value) {console.log("Battery On Status :" + value);});
registerListener("Right Switch Panel","GEN", function(value) {console.log("Generator Status :" + value);});
registerListener("Control System","RUDDER_TRIM", function(value) {console.log("Rudder Trim :" + value);});
registerListener("Control System","ELEVATOR_TRIM", function(value) {console.log("Elevator :" + value);});
registerListener("Control System","FLAPS_CONTROL_HANDLE", function(value) {console.log("Flaps :" + value);});
registerListener("Control System","AILERON_TRIM", function(value) {console.log("Aileron Trim :" + value);});

function processByte(c) {
    switch(state) {
            case "WAIT_FOR_SYNC":
            break;
            
            case "ADDRESS_LOW":
                address_uint8[0] = c;
                state = "ADDRESS_HIGH";
            break;
            
            case "ADDRESS_HIGH":
                address_uint8[1] = c;
                if (address_uint16[0] !== 0x5555) {
                    state = "COUNT_LOW";
                } else {
                    state = "WAIT_FOR_SYNC";
                }
            break;
            
            case "COUNT_LOW":
                count_uint8[0] = c;
                state = "COUNT_HIGH";
            break;
            
            case "COUNT_HIGH":
                count_uint8[1] = c;
                state = "DATA_LOW";
            break;
            
            case "DATA_LOW":
                data_uint8[0] = c;
                count_uint16[0]--;
                state = "DATA_HIGH";
            break;
            
            case "DATA_HIGH":
                data_uint8[1] = c;
                count_uint16[0]--;
                notifyData(address_uint16[0], data_buffer)
                address_uint16[0] += 2;
                if (count_uint16[0] === 0) {
                    state = "ADDRESS_LOW";
                } else {
                    state = "DATA_LOW";
                }
            break;
                
        }
        
        if (c === 0x55)
            sync_byte_count++;
        else
            sync_byte_count = 0;
            
        if (sync_byte_count === 4) {
            state = "ADDRESS_LOW";
            sync_byte_count = 0;
            notifyEndOfUpdate();
        }
  }

ws.on('open', function open() {
    console.log("Opened Connection.");
    const requestLiveData = JSON.stringify({
        datatype: "live_data",
        data: {}
      });
      ws.send(requestLiveData);
});

ws.on('message', function incoming(data) {
    var msBuffer = Buffer.alloc(2);
    var lsBuffer = Buffer.alloc(2);

    msBuffer[0] = data[5];
    msBuffer[1] = data[6];

    lsBuffer[0] = data[7];
    lsBuffer[1] = data[8];
    for(let idx = 0; idx < data.length; ++idx){
        processByte(data[idx]);
    }

    //console.log(msBuffer.toString('hex') + "  " + lsBuffer.toString('hex'));

    var msWord = data[4] << 8 | data[5];
    var mlsWord = data[6] << 8 | data[7];
    //console.log(data.toString('hex'));
});