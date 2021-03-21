using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.IO.Ports;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;
using System.Runtime.CompilerServices;
using OxyPlot;

namespace GaugeApp
{
    public class MainViewModel : INotifyPropertyChanged
    {
        #region Fields
        SerialPort _serialPort;
        private readonly Dispatcher _dispatcher;
        int _commandBufferIndex = 0;
        List<string> _commandBuffer = new List<string>();
        public event PropertyChangedEventHandler PropertyChanged;
        #endregion

        public MainViewModel(Dispatcher dispatcher)
        {
            _dispatcher = dispatcher;
            OpenPortCommand = new RelayCommand(OpenPort, () => _selectdPort != "-select port-" && _serialPort == null);

            ClosePortCommand = new RelayCommand(ClosePort, () => _serialPort != null);
            SendTextCommand = new RelayCommand(SendText, () => _serialPort != null && !String.IsNullOrEmpty(CommandText));
            NextTextCommand = new RelayCommand(NextText, () => _serialPort != null && _commandBufferIndex > 0);
            SendResetCommand = new RelayCommand(SendReset, () => _serialPort != null);
            PreviousTextCommand = new RelayCommand(PreviousText, () => _serialPort != null);
            SendZeroCommand = new RelayCommand(Zero, () => _serialPort != null);
            IncrementStepsCommand = new RelayCommand(() => StepperRaw += StepSize, () => _serialPort != null);
            DecrementStepsCommand = new RelayCommand(() => StepperRaw -= StepSize, () => _serialPort != null);
            FitCurveCommand = new RelayCommand(FitCurve);
            SendCoefficientsCommand = new RelayCommand(SendCoefficients, () => CurveFitModel.Coefficients != null && _serialPort != null);
        }

        #region Methods
        public void OpenPort()
        {
            try
            {
                _serialPort = new SerialPort();
                _serialPort.PortName = SelectedPort;
                _serialPort.BaudRate = 115200;
                _serialPort.Handshake = System.IO.Ports.Handshake.None;
                _serialPort.Parity = Parity.None;
                _serialPort.DataBits = 8;
                _serialPort.StopBits = StopBits.One;
                _serialPort.ReadTimeout = 200;
                _serialPort.WriteTimeout = 50;
                _serialPort.DataReceived += _serialPort_DataReceived;
                _serialPort.Open();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Could not open serial port: {ex.Message}");
                _serialPort.Close();
            }

            OpenPortCommand.RaiseCanExecuteChanged();
            ClosePortCommand.RaiseCanExecuteChanged();
            SendTextCommand.RaiseCanExecuteChanged();
            NextTextCommand.RaiseCanExecuteChanged();
            SendResetCommand.RaiseCanExecuteChanged();
            PreviousTextCommand.RaiseCanExecuteChanged();
            SendZeroCommand.RaiseCanExecuteChanged();
            IncrementStepsCommand.RaiseCanExecuteChanged();
            DecrementStepsCommand.RaiseCanExecuteChanged();
            SendCoefficientsCommand.RaiseCanExecuteChanged();
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(IsPortClosed)));
        }

        public void ClosePort()
        {
            if (_serialPort != null)
            {
                _serialPort.Close();
                _serialPort.Dispose();
                _serialPort = null;
                OpenPortCommand.RaiseCanExecuteChanged();
                ClosePortCommand.RaiseCanExecuteChanged();
                SendTextCommand.RaiseCanExecuteChanged();
                NextTextCommand.RaiseCanExecuteChanged();
                SendResetCommand.RaiseCanExecuteChanged();
                PreviousTextCommand.RaiseCanExecuteChanged();
                SendZeroCommand.RaiseCanExecuteChanged();
                IncrementStepsCommand.RaiseCanExecuteChanged();
                DecrementStepsCommand.RaiseCanExecuteChanged();
                SendCoefficientsCommand.RaiseCanExecuteChanged();
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(IsPortClosed)));
            }
        }

        public void SendText(string text)
        {
            if (_serialPort != null)
            {
                _serialPort.WriteLine(text);
                Debug.WriteLine(text);
            }
        }

        public void SendReset()
        {
            SendText("reset");
        }

        public void Zero()
        {
            SendText("zero");
        }

        public void SendText()
        {
            if (_serialPort != null && !String.IsNullOrEmpty(CommandText))
            {
                _serialPort.WriteLine(CommandText);
                var topCommandItem = _commandBuffer.FirstOrDefault();

                if (topCommandItem != CommandText)
                    _commandBuffer.Insert(0, CommandText);

                _commandBufferIndex = 0;
                CommandText = String.Empty;
            }
        }

        public async void SendCoefficients()
        {
            for (var idx = 0; idx < CurveFitModel.Coefficients.Count; ++idx)
            {
                SendText($"set-p{idx + 1}-stepper {CurveFitModel.Coefficients[idx]}");
                await Task.Delay(200);
            }
        }

        public void FitCurve()
        {
            ChartModel = CurveFitModel.GetChartModel();
            SendCoefficientsCommand.RaiseCanExecuteChanged();
        }

        public void NextText()
        {
            if (_commandBufferIndex > 0)
            {
                _commandBufferIndex--;
                CommandText = _commandBuffer[_commandBufferIndex];
            }
        }

        public void PreviousText()
        {
            if (_commandBufferIndex < _commandBuffer.Count)
            {
                CommandText = _commandBuffer[_commandBufferIndex];
                _commandBufferIndex++;
            }
        }
        #endregion

        private void _serialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {

            _dispatcher.Invoke(DispatcherPriority.Normal, new Action(() =>
            {
                while (_serialPort.BytesToRead > 0)
                {
                    var data = _serialPort.ReadLine();
                    Console.Add($"{DateTime.Now} - {data.Trim()}");

                    var setting = Settings.OrderByDescending(set => set.Label.Length).Where(st => data.StartsWith(st.Label)).FirstOrDefault();

                    if (setting != null)
                    {

                        setting.Value = data.Split(':')[1].Trim();
                        switch (setting.Key)
                        {
                            case "set-min-left": 
                                _leftServoMin = Convert.ToInt32(setting.Value); 
                                _leftServoRaw = LeftServoMin;
                                RaisePropertyChanged(nameof(LeftServoMin));
                                RaisePropertyChanged(nameof(LeftServoRaw));
                                break;
                            case "set-max-left": 
                                _leftServoMax = Convert.ToInt32(setting.Value);
                                RaisePropertyChanged(nameof(LeftServoMax));
                                break;
                            case "set-minvalue-left": 
                                _leftServoMinValue = Convert.ToDouble(setting.Value); 
                                _leftServoValue = LeftServoMinValue;
                                RaisePropertyChanged(nameof(LeftServoMinValue));
                                RaisePropertyChanged(nameof(LeftServoValue));
                                break;
                            case "set-maxvalue-left": 
                                _leftServoMaxValue = Convert.ToDouble(setting.Value);
                                RaisePropertyChanged(nameof(LeftServoMaxValue));
                                break;

                            case "set-min-right": 
                                _rightServoMin = Convert.ToInt32(setting.Value); 
                                _rightServoRaw = RightServoMin;
                                RaisePropertyChanged(nameof(RightServoMin));
                                RaisePropertyChanged(nameof(RightServoRaw));
                                break;
                            case "set-max-right": 
                                _rightServoMax = Convert.ToInt32(setting.Value);
                                RaisePropertyChanged(nameof(RightServoMax));
                                break;
                            case "set-minvalue-right": 
                                _rightServoMinValue = Convert.ToDouble(setting.Value); 
                                RightServoValue = RightServoMinValue;
                                RaisePropertyChanged(nameof(RightServoMinValue));
                                RaisePropertyChanged(nameof(RightServoValue));
                                break;
                            case "set-maxvalue-right": 
                                _rightServoMaxValue = Convert.ToDouble(setting.Value);
                                RaisePropertyChanged(nameof(RightServoMaxValue));
                                break;

                            case "set-minvalue-stepper": 
                                _stepperValueMin = Convert.ToDouble(setting.Value); 
                                RaisePropertyChanged(nameof(StepperValueMin)); 
                                break;
                            case "set-maxvalue-stepper": 
                                _stepperValueMax = Convert.ToDouble(setting.Value); 
                                RaisePropertyChanged(nameof(StepperValueMax)); 
                                break;
                        }
                    }
                }
            }));

        }
        public void Init()
        {
            RefreshPorts();
            Settings.Add(new SettingsItem(SendText) { Label = "Left Servo Exists", Key = "set-exists-left", DataType = DataTypes.Boolean });
            Settings.Add(new SettingsItem(SendText) { Label = "Left Servo Address", Key = "set-addr-left", DataType = DataTypes.Address });
            Settings.Add(new SettingsItem(SendText) { Label = "Left Servo Min", Key = "set-min-left", DataType = DataTypes.Int });
            Settings.Add(new SettingsItem(SendText) { Label = "Left Servo Max", Key = "set-max-left", DataType = DataTypes.Int });
            Settings.Add(new SettingsItem(SendText) { Label = "Left Servo Min Val", Key = "set-minvalue-left", DataType = DataTypes.Double });
            Settings.Add(new SettingsItem(SendText) { Label = "Left Servo Max Val", Key = "set-maxvalue-left", DataType = DataTypes.Double });

            Settings.Add(new SettingsItem(SendText) { Label = "Right Servo Exists", Key = "set-exists-right", DataType = DataTypes.Boolean });
            Settings.Add(new SettingsItem(SendText) { Label = "Right Servo Address", Key = "set-addr-right", DataType = DataTypes.Address });
            Settings.Add(new SettingsItem(SendText) { Label = "Right Servo Min", Key = "set-min-right", DataType = DataTypes.Int });
            Settings.Add(new SettingsItem(SendText) { Label = "Right Servo Max", Key = "set-max-right", DataType = DataTypes.Int });
            Settings.Add(new SettingsItem(SendText) { Label = "Right Servo Min Val", Key = "set-minvalue-right", DataType = DataTypes.Double });
            Settings.Add(new SettingsItem(SendText) { Label = "Right Servo Max Val", Key = "set-maxvalue-right", DataType = DataTypes.Double });

            Settings.Add(new SettingsItem(SendText) { Label = "Stepper Exists", Key = "set-exists-stepper", DataType = DataTypes.Boolean });
            Settings.Add(new SettingsItem(SendText) { Label = "Stepper Address", Key = "set-addr-stepper", DataType = DataTypes.Address });

            Settings.Add(new SettingsItem(SendText) { Label = "Stepper Speed", Key = "set-speed-stepper", DataType = DataTypes.Int });
            Settings.Add(new SettingsItem(SendText) { Label = "Stepper Zero Offset", Key = "set-offset-stepper", DataType = DataTypes.Int });
            Settings.Add(new SettingsItem(SendText) { Label = "Stepper Mapping Fun", Key = "set-mapfunc-stepper", DataType = DataTypes.MapFunc });
            Settings.Add(new SettingsItem(SendText) { Label = "Stepper Center", Key = "set-cen-stepper", DataType = DataTypes.Double });
            Settings.Add(new SettingsItem(SendText) { Label = "Stepper Min Val", Key = "set-minvalue-stepper", DataType = DataTypes.Double });
            Settings.Add(new SettingsItem(SendText) { Label = "Stepper Max Val", Key = "set-maxvalue-stepper", DataType = DataTypes.Double });

            for (int idx = 1; idx <= 5; ++idx)
            {
                Settings.Add(new SettingsItem(SendText) { Label = $"Stepper P{idx}", Key = $"set-p{idx}-stepper", DataType = DataTypes.Double });
                Settings.Add(new SettingsItem(SendText) { Label = $"Stepper 2x P{idx}", Key = $"set-2xp{idx}--stepper", DataType = DataTypes.Double });
            }
        }

        private void RefreshPorts()
        {
            var ports = new List<string>();
            ports.Add("-select port-");
            foreach (var port in SerialPort.GetPortNames().ToList())
            {
                ports.Add(port);
            }

            Ports = ports;
        }

        #region Properties
        private List<string> _ports;
        public List<string> Ports
        {
            get { return _ports; }
            private set
            {
                _ports = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Ports)));
            }
        }

        private string _selectdPort = "-select port-";
        public string SelectedPort
        {
            get { return _selectdPort; }
            set
            {
                _selectdPort = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(SelectedPort)));
                OpenPortCommand.RaiseCanExecuteChanged();
                ClosePortCommand.RaiseCanExecuteChanged();
            }
        }

        public bool IsPortClosed
        {
            get => _selectdPort != null;
        }

        string _commandText;
        public string CommandText
        {
            get { return _commandText; }
            set
            {
                _commandText = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(CommandText)));
                SendTextCommand.RaiseCanExecuteChanged();
            }
        }
        #endregion

        DateTime _debounce = DateTime.MinValue;

        #region Left Servo
        int _leftServoRaw = 600;
        public int LeftServoRaw
        {
            get => _leftServoRaw;
            set
            {
                if (_leftServoRaw != value)
                {
                    _leftServoRaw = value;
                    RaisePropertyChanged();
                    _debounce = DateTime.Now;
                    Task.Run(async () =>
                    {
                        await Task.Delay(250);
                        if ((DateTime.Now - _debounce).TotalMilliseconds > 250)
                            SendText($"left-raw {value}");
                    });
                }
            }
        }

        double _leftServoValue;
        public double LeftServoValue
        {
            get => _leftServoValue;
            set
            {
                if (_leftServoValue != value)
                {
                    _leftServoValue = value;
                    _debounce = DateTime.Now;
                    RaisePropertyChanged();
                    Task.Run(async () =>
                    {
                        await Task.Delay(250);
                        if ((DateTime.Now - _debounce).TotalMilliseconds > 250)
                            SendText($"left {value}");
                    });
                }
            }
        }

        private double _leftServoMinValue;
        public double LeftServoMinValue
        {
            get => _leftServoMinValue;
            set
            {
                if (_leftServoMinValue != value)
                {
                    _leftServoMinValue = value;
                    RaisePropertyChanged();
                    SendText($"set-minvalue-left {value}");
                }
            }
        }

        private double _leftServoMaxValue;
        public double LeftServoMaxValue
        {
            get => _leftServoMaxValue;
            set
            {
                _leftServoMaxValue = value;
                RaisePropertyChanged();
                SendText($"set-maxvalue-left {value}");
            }
        }

        private int _leftServoMin;
        public int LeftServoMin
        {
            get => _leftServoMin;
            set
            {
                if (_leftServoMin != value)
                {
                    _leftServoMin = value;
                    RaisePropertyChanged();
                    SendText($"set-min-left {value}");
                }

            }
        }

        private int _leftServoMax;
        public int LeftServoMax
        {
            get => _leftServoMax;
            set
            {
                if (_leftServoMax != value)
                {
                    _leftServoMax = value;
                    RaisePropertyChanged();
                    SendText($"set-max-left {value}");
                }
            }
        }
        #endregion        

        #region Right Servo
        int _rightServoRaw = 600;
        public int RightServoRaw
        {
            get => _rightServoRaw;
            set
            {
                if (_rightServoRaw != value)
                {
                    _rightServoRaw = value;
                    RaisePropertyChanged();
                    _debounce = DateTime.Now;
                    Task.Run(async () =>
                    {
                        await Task.Delay(250);
                        if ((DateTime.Now - _debounce).TotalMilliseconds > 250)
                            SendText($"right-raw {value}");
                    });
                }
            }
        }

        double _rightServoValue;
        public double RightServoValue
        {
            get => _rightServoValue;
            set
            {
                if (_rightServoValue != value)
                {
                    _rightServoValue = value;
                    RaisePropertyChanged();
                    _debounce = DateTime.Now;
                    Task.Run(async () =>
                    {
                        await Task.Delay(250);
                        if ((DateTime.Now - _debounce).TotalMilliseconds > 250)
                            SendText($"right {value}");
                    });
                }
            }
        }

        private double _rightServoMinValue;
        public double RightServoMinValue
        {
            get => _rightServoMinValue;
            set
            {
                _rightServoMinValue = value;
                RaisePropertyChanged();
                SendText($"set-minvalue-right {value}");
            }
        }

        private double _rightServoMaxValue;
        public double RightServoMaxValue
        {
            get => _rightServoMaxValue;
            set
            {
                if (_rightServoMaxValue != value)
                {
                    _rightServoMaxValue = value;
                    RaisePropertyChanged();
                    SendText($"set-maxvalue-right {value}");
                }
            }
        }

        private int _rightServoMin;
        public int RightServoMin
        {
            get => _rightServoMin;
            set
            {
                if (_rightServoMin != value)
                {
                    _rightServoMin = value;
                    RaisePropertyChanged();
                    SendText($"set-min-right {value}");
                }
            }
        }

        private int _rightServoMax;
        public int RightServoMax
        {
            get => _rightServoMax;
            set
            {
                if (_rightServoMax != value)
                {
                    _rightServoMax = value;
                    RaisePropertyChanged();
                    SendText($"set-max-right {value}");
                }
            }
        }
        #endregion

        #region Stepper
        private int _stepperRaw = 0;
        public int StepperRaw
        {
            get => _stepperRaw;
            set
            {
                _stepperRaw = value;
                RaisePropertyChanged();
                _debounce = DateTime.Now;
                Task.Run(async () =>
                {
                    await Task.Delay(250);
                    if ((DateTime.Now - _debounce).TotalMilliseconds > 250)
                        SendText($"stepper-raw {value}");
                });

            }
        }

        private int _stepSize = 10;
        public int StepSize
        {
            get => _stepSize;
            set
            {
                _stepSize = value;
                RaisePropertyChanged();
            }
        }

        private double _stepperValue = 0;
        public double StepperValue
        {
            get => _stepperValue;
            set
            {
                _stepperValue = value;
                RaisePropertyChanged();
                _debounce = DateTime.Now;
                Task.Run(async () =>
                {
                    await Task.Delay(250);
                    if ((DateTime.Now - _debounce).TotalMilliseconds > 250)
                        SendText($"stepper {value}");
                });

            }
        }

        private double _stepperValueMin = 0;
        public double StepperValueMin
        {
            get => _stepperValueMin;
            set
            {
                _stepperValueMin = value;
                RaisePropertyChanged();
                SendText($"set-minvalue-stepper {value}");
            }
        }

        private double _stepperValueMax = 0;
        public double StepperValueMax
        {
            get => _stepperValueMax;
            set
            {
                _stepperValueMax = value;
                RaisePropertyChanged();
                SendText($"set-maxvalue-stepper {value}");
            }
        }
        #endregion

        #region Collection Settings
        public ObservableCollection<SettingsItem> Settings
        {
            get;
        } = new ObservableCollection<SettingsItem>();

        public ObservableCollection<String> Console
        {
            get;
        } = new ObservableCollection<String>();
        #endregion

        #region Commands
        public RelayCommand OpenPortCommand { get; }
        public RelayCommand ClosePortCommand { get; }

        public RelayCommand SendTextCommand { get; }
        public RelayCommand PreviousTextCommand { get; }
        public RelayCommand NextTextCommand { get; }
        public RelayCommand SendResetCommand { get; }
        public RelayCommand SendZeroCommand { get; }
        public RelayCommand FitCurveCommand { get; }
        public RelayCommand SendCoefficientsCommand { get; }
        public RelayCommand IncrementStepsCommand { get; }
        public RelayCommand DecrementStepsCommand { get; }
        #endregion


        PlotModel _chartModel;
        public PlotModel ChartModel
        {
            get => _chartModel;
            set
            {
                _chartModel = value;
                RaisePropertyChanged();
            }
        }


        CurveFitData _curveFitModel = new CurveFitData();
        public CurveFitData CurveFitModel
        {
            get => _curveFitModel;
            set
            {
                _curveFitModel = value;
                RaisePropertyChanged();
            }
        }


        private void RaisePropertyChanged([CallerMemberName] string memberName = "")
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(memberName));
        }
    }
}
