using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO.Ports;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;

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
            NextTextCommand = new RelayCommand(NextText, () => _serialPort != null &&  _commandBufferIndex > 0);
            SendResetCommand = new RelayCommand(SendReset, () => _serialPort != null);
            PreviousTextCommand = new RelayCommand(PreviousText, () => _serialPort != null);
            SendZeroCommand = new RelayCommand(Zero, () => _serialPort != null);
            IncrementStepsCommand = new RelayCommand(() => StepperRaw += StepSize, () => _serialPort != null);
            DecrementStepsCommand = new RelayCommand(() => StepperRaw -= StepSize, () => _serialPort != null);
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
            catch(Exception ex)
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
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(IsPortClosed)));
            }
        }

        public void SendText(string text)
        {
            if (_serialPort != null)
            {
                _serialPort.WriteLine(text);
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
                
                if(topCommandItem != CommandText)
                    _commandBuffer.Insert(0, CommandText);

                _commandBufferIndex = 0;
                CommandText = String.Empty;
            }
        }

        public void NextText()
        {
            if(_commandBufferIndex > 0)
            {
                _commandBufferIndex--;
                CommandText = _commandBuffer[_commandBufferIndex];
            }
        }

        public void PreviousText()
        {
            if(_commandBufferIndex < _commandBuffer.Count)
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
                    Console.Add(data.Trim());

                    var setting = Settings.OrderByDescending(set=>set.Label.Length).Where(st => data.StartsWith(st.Label)).FirstOrDefault();
                    
                    if (setting != null)
                    {
                     
                        setting.Value = data.Split(':')[1].Trim();
                        switch(setting.Key)
                        {
                            case "set-min-left": LeftServoMin = Convert.ToInt32(setting.Value); LeftServoRaw = LeftServoMin; break;
                            case "set-max-left": LeftServoMax = Convert.ToInt32(setting.Value); break;
                            case "set-minvalue-left": LeftServoMinValue = Convert.ToDouble(setting.Value); LeftServoValue = LeftServoMinValue; break;
                            case "set-maxvalue-left": LeftServoMaxValue = Convert.ToDouble(setting.Value); break;

                            case "set-min-right": RightServoMin = Convert.ToInt32(setting.Value); break;
                            case "set-max-right": RightServoMax = Convert.ToInt32(setting.Value); RightServoRaw = RightServoMax; break;
                            case "set-minvalue-right": RightServoMinValue = Convert.ToDouble(setting.Value); RightServoValue = RightServoMinValue; break;
                            case "set-maxvalue-right": RightServoMaxValue = Convert.ToDouble(setting.Value); break;

                            case "set-minvalue-stepper": StepperValueMin = Convert.ToDouble(setting.Value); break;
                            case "set-maxvalue-stepper": StepperValueMax= Convert.ToDouble(setting.Value); break;
                        }
                    }
                }
            }));

        }
        public void Init()
        {
            RefreshPorts();
            Settings.Add(new SettingsItem(SendText) { Label = "Left Servo Exists", Key = "set-exists-left", DataType = DataTypes.Boolean});
            Settings.Add(new SettingsItem(SendText) { Label = "Left Servo Address", Key = "set-addr-left", DataType = DataTypes.Address });
            Settings.Add(new SettingsItem(SendText) { Label = "Left Servo Min", Key = "set-min-left", DataType = DataTypes.Int });
            Settings.Add(new SettingsItem(SendText) { Label = "Left Servo Max", Key = "set-max-left", DataType = DataTypes.Int });
            Settings.Add(new SettingsItem(SendText) { Label = "Left Servo Min Val", Key = "set-minvalue-left", DataType = DataTypes.Double });
            Settings.Add(new SettingsItem(SendText) { Label = "Left Servo Max Val", Key = "set-maxvalue-left", DataType = DataTypes.Double });

            Settings.Add(new SettingsItem(SendText) { Label = "Right Servo Exists", Key = "set-exists-right", DataType = DataTypes.Boolean});
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
                Settings.Add(new SettingsItem(SendText) { Label = $"Stepper 2x P{idx}", Key = $"set-2x-p{idx}--stepper", DataType = DataTypes.Double });
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
        int _leftServo = 600;
        public int LeftServoRaw
        {
            get => _leftServo;
            set
            {
                if(_leftServo != value)
                {
                    _leftServo = value;
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(LeftServoRaw)));
                    _debounce = DateTime.Now;
                    Task.Run(async () =>
                    {
                        await Task.Delay(250);
                        if((DateTime.Now - _debounce).TotalMilliseconds > 250)
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
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(_leftServoValue)));
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
                _leftServoMinValue = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(LeftServoMinValue)));
                SendText($"set-minvalue-left {value}");
            }
        }

        private double _leftServoMaxValue;
        public double LeftServoMaxValue
        {
            get => _leftServoMaxValue;
            set
            {
                _leftServoMaxValue = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(LeftServoMaxValue)));
                SendText($"set-maxvalue-left {value}");
            }
        }

        private int _leftServoMin;
        public int LeftServoMin
        {
            get => _leftServoMin;
            set
            {
                _leftServoMin = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(LeftServoMin)));
                SendText($"set-min-left {value}");
            }
        }

        private int _leftServoMax;
        public int LeftServoMax
        {
            get => _leftServoMax;
            set
            {
                _leftServoMax = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(LeftServoMax)));
                SendText($"set-max-left {value}");
            }
        }
        #endregion        

        #region Right Servo
        int _rightServo = 600;
        public int RightServoRaw
        {
            get => _rightServo;
            set
            {
                if (_rightServo != value)
                {
                    _rightServo = value;
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(RightServoRaw)));
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
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(_rightServoValue)));
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
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(RightServoMinValue)));
                SendText($"set-minvalue-right {value}");
            }
        }

        private double _rightServoMaxValue;
        public double RightServoMaxValue
        {
            get => _rightServoMaxValue;
            set
            {
                _rightServoMaxValue = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(RightServoMaxValue)));
                SendText($"set-maxvalue-right {value}");
            }
        }

        private int _rightServoMin;
        public int RightServoMin
        {
            get => _rightServoMin;
            set
            {
                _rightServoMin = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(RightServoMin)));
                SendText($"set-min-right {value}");
            }
        }

        private int _rightServoMax;
        public int RightServoMax
        {
            get => _rightServoMax;
            set
            {
                _rightServoMax = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(RightServoMax)));
                SendText($"set-max-right {value}");
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
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(StepperRaw)));
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
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(StepSize)));
            }
        }

        private double _stepperValue = 0;
        public double StepperValue
        {
            get => _stepperValue;
            set
            {
                _stepperValue = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(StepperValue)));
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
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(StepperValueMin)));
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
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(StepperValueMax)));
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
        public RelayCommand IncrementStepsCommand { get; }
        public RelayCommand DecrementStepsCommand { get; }
        #endregion
    }
}
