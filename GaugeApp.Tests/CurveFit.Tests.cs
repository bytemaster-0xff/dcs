using MathNet.Numerics;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace GaugeApp.Tests
{
    [TestClass]
    public class CurveFit
    {
        [TestMethod]
        public void FitCurveLinear()
        {
            var data = new CurveFitData()
            {
                RawData =
@"50,180
75,240,
100,160,
125,180,
150,220,
175,300"
            };

            Tuple<double, double> p = Fit.Line(data.GetIndependentAxis(), data.GetDependentAxis());
            Console.WriteLine(p.Item1);
            Console.WriteLine(p.Item2);

            for (int idx = 3; idx <= 5; ++idx)
            {
                double[] coeficients = Fit.Polynomial(data.GetIndependentAxis(), data.GetDependentAxis(), idx);
                Console.WriteLine($"Fit curve {idx} coefficietns");
                for(int coIdx = 0; coIdx < idx; ++coIdx)
                {
                    Console.WriteLine($"\t{coeficients[coIdx]}");
                }
            }
            

        }

        [TestMethod]
        public void GetAxis()
        {
            var data = new CurveFitData()
            {
                RawData =
@"50,180
75,240,
100,160,
125,180,
150,220,
175,300"
            };

            var xAxis = data.GetIndependentAxis();
            var yAxis = data.GetDependentAxis();

            for (int idx = 0; idx < xAxis.Length; ++idx)
            {
                Console.WriteLine($"{idx}. {xAxis[idx]}, {yAxis[idx]}");
            }


        }
    }
}
