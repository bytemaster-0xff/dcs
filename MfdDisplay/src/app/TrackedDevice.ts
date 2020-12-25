export class TrackedDevice {
    address: number;
    device: AircraftDevice;
    callback: (value: number) => void;
}

export class AircraftDevice {
    category: String;
    control_type: String;
    description: String;
    identifier: String;
    inputs: DeviceInput[];
    outputs: DeviceOutput[];
    momentary_positions: string;
    currentValue?: number;
}

export class DeviceInput {
    argument: string;
    description: string;
    interface: string;
    max_value?: number;
}

export class DeviceOutput {
    address: number;
    description: string;
    mask: number;
    max_value: string;
    shift_by: number;
    suffix: string;
    type: string;
}


export class Line {
    x1: number;
    x2: number;
    y1: number;
    y2: number;
    labelX?: number;
    labelY?: number;
    label?: string;
    rotate?: number;
}

export class SegmentsSpec {
    min: number;
    max: number;
    centerX: number;
    centerY: number;
    radius: number;
    degrees: number = 360;
    startAngle: number = 0;
    label: string;
    segmentCount: number;
    minorSegmentCount: number;
    segmentLength: number = 5;
    numberDecimal: number = 1;
}