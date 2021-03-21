using System.Diagnostics;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace GaugeApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        MainViewModel _viewModel;

        public MainWindow()
        {
            InitializeComponent();
            _viewModel = new MainViewModel(Dispatcher);
            _viewModel.Init();
            _viewModel.Console.CollectionChanged += Console_CollectionChanged;
            DataContext = _viewModel;
        }

        private void Console_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            lock (this)
            {
                var lastItemIndex = ConsoleOutput.Items.Count - 1;
                var listItem = ConsoleOutput.Items[lastItemIndex];
                ConsoleOutput.Items.Refresh();
                ConsoleOutput.ScrollIntoView(listItem);
            }
        }

        private void TextBlock_KeyUp(object sender, KeyEventArgs e)
        {
            if(e.Key == Key.Up)
            {
                _viewModel.PreviousTextCommand.Execute(null);
            }
            else if(e.Key == Key.Down)
            {
                _viewModel.NextTextCommand.Execute(null);
            }
            else if (e.Key == Key.Enter)
            {
                _viewModel.SendTextCommand.Execute(null);
            }
        }

        private void TextBox_TextChanged(object sender, System.Windows.Controls.TextChangedEventArgs e)
        {
            _viewModel.CommandText = (sender as TextBox).Text;
        }
    }
}
