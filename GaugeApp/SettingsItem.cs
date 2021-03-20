using System;
using System.ComponentModel;

namespace GaugeApp
{
    public enum DataTypes
    {
        Boolean,
        Int,
        Address,
        Double,
        String,
        MapFunc
    }

    public class SettingsItem : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        Action<string> _sendCommand;

        public SettingsItem(Action<string> sendCommand)
        {
            UpdateValueCommand = new RelayCommand(UpdateValue, () => IsDirty);
            _sendCommand = sendCommand ?? throw new ArgumentNullException(nameof(sendCommand));
        }

        public void UpdateValue()
        {
            _sendCommand($"{Key} {Value}");
        }

        public string Label { get; set; }
        public string Key { get; set; }

        public DataTypes DataType { get; set; }

        private string _value;
        public string Value
        {
            get { return _value; }
            set
            {
                if (_value != value)
                {
                    _value = value;
                    IsDirty = true;
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Value)));
                    UpdateValueCommand.RaiseCanExecuteChanged();
                }
            }
        }

        private bool _isDirty;
        public bool IsDirty
        {
            get { return _isDirty; }
            set
            {
                _isDirty = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(IsDirty)));
            }
        }

        public RelayCommand UpdateValueCommand { get; }
    }
}
