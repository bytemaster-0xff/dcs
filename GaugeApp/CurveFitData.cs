using MathNet.Numerics;
using OxyPlot;
using OxyPlot.Series;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace GaugeApp
{
    public enum CurveFitOrder
    {
        Linear,
        Second,
        Third,
        Fourth
    }
    public class CurveFitData : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        public string RawData
        {
            get; set;
        }

        public double Center
        {
            get; set;
        }

        public bool UseTwoEquations
        {
            get; set;
        }

        public List<double> Coefficients { get; set; }
        public List<double> Coefficients2 { get; set; }

        public double[] GetIndependentAxis()
        {
            var points = new List<double>();
            var lines = RawData.Split('\n');
            foreach (var line in lines)
            {
                var trimmedLine = line.Trim();
                if (!string.IsNullOrEmpty(trimmedLine))
                {
                    var parts = trimmedLine.Trim().Split(',');
                    points.Add(System.Convert.ToDouble(parts[1]));
                }
            }

            return points.ToArray();
        }
        
        public double[] GetDependentAxis()
        {

            var points = new List<double>();
            var lines = RawData.Split('\n');
            foreach (var line in lines)
            {
                var trimmedLine = line.Trim();
                if (!string.IsNullOrEmpty(trimmedLine))
                {
                    var parts = trimmedLine.Trim().Split(',');
                    points.Add(System.Convert.ToDouble(parts[0]));
                }
            }

            return points.ToArray();
        }

        public PlotModel GetChartModel()
        {
            var tmp = new PlotModel { Title = "Gauge Curve Fit", Subtitle = "Software Logistics" };

            var rawData = new LineSeries { Title = "Raw Data", MarkerType = MarkerType.Circle };
            var xAxis = GetIndependentAxis();
            var yAxis = GetDependentAxis();

            for (var idx = 0; idx < xAxis.Length; ++idx)
            {
                rawData.Points.Add(new DataPoint(xAxis[idx], yAxis[idx]));
            }

            var polynomial = Polynomial.Fit(GetIndependentAxis(), GetDependentAxis(), 4);

            var fitPoints = new LineSeries { Title = "5th order poly fit", MarkerType = MarkerType.Triangle };
            for (var idx = 0; idx < xAxis.Length; ++idx)
            {
                var x = xAxis[idx];

                var y3 = (polynomial.Coefficients[4] * System.Math.Pow(x, 4)) +
                         (polynomial.Coefficients[3] * System.Math.Pow(x, 3)) +
                         (polynomial.Coefficients[2] * System.Math.Pow(x, 2)) +
                         (polynomial.Coefficients[1] * x) +
                          polynomial.Coefficients[0];

                fitPoints.Points.Add(new DataPoint(x, y3));
            }

            Coefficients = new List<double>(polynomial.Coefficients);
            RaisePropertyChanged(nameof(Coefficients));

            tmp.Series.Add(rawData);
            tmp.Series.Add(fitPoints);

            return tmp;
        }

        private void RaisePropertyChanged([CallerMemberName] string memberName = "")
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(memberName));
        }
    }
}
