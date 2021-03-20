using System;
using System.Windows.Input;

namespace GaugeApp
{
    public class RelayCommand : ICommand
    {
        public event EventHandler CanExecuteChanged;

        readonly Action _cmd;
        readonly Action<Object> _objCmd;
        readonly Func<bool> _canExecuted;

        public RelayCommand(Action cmd, Func<bool> canExecute)
        {
            _cmd = cmd;
            _canExecuted = canExecute;
        }

        public void RaiseCanExecuteChanged()
        {
            CanExecuteChanged?.Invoke(this, null);
        }

        public RelayCommand(Action cmd)
        {
            _cmd = cmd;
        }

        public RelayCommand(Action<Object> cmd)
        {
            _objCmd = cmd;
        }

        public bool CanExecute(object parameter)
        {
            return _canExecuted == null ? true : _canExecuted();
        }

        public void Execute(object parameter)
        {

            if (_cmd == null)
                _objCmd(parameter);
            else if (_cmd != null)
                _cmd();
            else
                throw new ArgumentException("No provided pcommand");
        }
    }
}
