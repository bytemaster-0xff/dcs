import { Observable, ReplaySubject } from 'rxjs';
import DcsUpdate from './DcsUpdate';

export default class ExportDataParser {
    private state = 'WAIT_FOR_SYNC';
    private syncByteCount = 0;
    private addressBuffer: ArrayBuffer;
    private addressUInt8: Uint8Array;
    private addressUInt16: Uint16Array;
    private countBuffer: ArrayBuffer;
    private countUInt8: Uint8Array;
    private countUInt16: Uint16Array;
    private dataBuffer: ArrayBuffer;
    private dataUInt8: Uint8Array;
    private dataUInt16: Uint16Array;

    protected dcsUpdate$ = new ReplaySubject<DcsUpdate>();

    private exportDataListeners: Array<{ callback: (address: number, data: ArrayBuffer) => void }>;
    private endOfUpdateListeners: Array<() => void>;

    constructor() {
        this.addressBuffer = new ArrayBuffer(2);
        this.addressUInt8 = new Uint8Array(this.addressBuffer);
        this.addressUInt16 = new Uint16Array(this.addressBuffer);
        this.countBuffer = new ArrayBuffer(2);
        this.countUInt8 = new Uint8Array(this.countBuffer);
        this.countUInt16 = new Uint16Array(this.countBuffer);
        this.dataBuffer = new ArrayBuffer(2);
        this.dataUInt8 = new Uint8Array(this.dataBuffer);
        this.dataUInt16 = new Uint16Array(this.dataBuffer);

        this.exportDataListeners = new Array<{ callback: (address: number, data: ArrayBuffer) => void }>();
        this.endOfUpdateListeners = new Array<() => void>();
    }

    public registerEndOfUpdateCallback(callback: () => void) {
        this.endOfUpdateListeners.push(callback);
    }
    public unregisterEndOfUpdateListener(callback: any) {
        this.endOfUpdateListeners = this.endOfUpdateListeners.filter(cb => cb !== callback);
    }

    public registerExportDataListener(callback: (address: number, data: ArrayBuffer) => void) {
        this.exportDataListeners.push({ callback });
    }

    public unregisterExportDataListener(callback: any) {
        this.exportDataListeners = this.exportDataListeners.filter(l => l.callback !== callback);
    }

    private notifyData(address: number, data: ArrayBuffer) {
      //console.log(address, data);
        this.dcsUpdate$.next({ address, data });
    }

    private notifyEndOfUpdate() {
        for (const cb of this.endOfUpdateListeners) {
            cb();
        }
    }

    public processByte(c: number) {
        switch (this.state) {
            case 'WAIT_FOR_SYNC':
                break;

            case 'ADDRESS_LOW':
                this.addressUInt8[0] = c;
                this.state = 'ADDRESS_HIGH';
                break;

            case 'ADDRESS_HIGH':
                this.addressUInt8[1] = c;
                if (this.addressUInt16[0] !== 0x5555) {
                    this.state = 'COUNT_LOW';
                } else {
                    this.state = 'WAIT_FOR_SYNC';
                }
                break;

            case 'COUNT_LOW':
                this.countUInt8[0] = c;
                this.state = 'COUNT_HIGH';
                break;

            case 'COUNT_HIGH':
                this.countUInt8[1] = c;
                this.state = 'DATA_LOW';
                break;

            case 'DATA_LOW':
                this.dataUInt8[0] = c;
                this.countUInt16[0]--;
                this.state = 'DATA_HIGH';
                break;

            case 'DATA_HIGH':
                this.dataUInt8[1] = c;
                this.countUInt16[0]--;
                this.notifyData(this.addressUInt16[0], this.dataBuffer);
                this.addressUInt16[0] += 2;
                if (this.countUInt16[0] === 0) {
                    this.state = 'ADDRESS_LOW';
                } else {
                    this.state = 'DATA_LOW';
                }
                break;
        }

        if (c === 0x55) {
            this.syncByteCount++;
        } else {
            this.syncByteCount = 0;
        }
        if (this.syncByteCount === 4) {
            this.state = 'ADDRESS_LOW';
            this.syncByteCount = 0;
            this.notifyEndOfUpdate();
        }
    }

    onDcsUpdate(): Observable<DcsUpdate> {
        return this.dcsUpdate$.asObservable();
    }
}
