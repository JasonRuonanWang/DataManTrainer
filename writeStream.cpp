#include <adios2.h>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>


int main(int argc, char *argv[])
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> bool_distrib(0, 1000);
    std::uniform_int_distribution<> variable_size_distrib(10, 1500);
    std::uniform_int_distribution<> combining_steps_distrib(1, 100);
    std::uniform_int_distribution<> accuracy_distrib(0, 5);

    for(int i=0; i<9999; ++i)
    {
        size_t variable_size = variable_size_distrib(gen);

        int transport_dist = bool_distrib(gen);
        std::string transport_method;
        if(transport_dist < 500)
        {
            transport_method = "fast";
        }
        else
        {
            transport_method = "fast";
        }

        int threading_dist = bool_distrib(gen);
        std::string threading;
        if(threading_dist < 500)
        {
            threading = "true";
        }
        else
        {
            threading = "false";
        }

        int compression_dist = bool_distrib(gen);

        int compression_accuracy = accuracy_distrib(gen);
        int float_accuracy = accuracy_distrib(gen);
        while (float_accuracy > compression_accuracy)
        {
            float_accuracy = accuracy_distrib(gen);
        }
        std::string accuracy_string = "0.";
        for(int i=0; i<float_accuracy; ++i)
        {
            accuracy_string += "0";
        }
        accuracy_string += "1";

        int combining_steps = combining_steps_distrib(gen);

        std::vector<double> myFloats(variable_size*variable_size);
        for(size_t i=0; i<myFloats.size(); i++)
        {
            myFloats[i]=i;
        }

        adios2::Dims shape({variable_size, variable_size});
        adios2::Dims start({0, 0});
        adios2::Dims count({variable_size, variable_size});

        adios2::Params engineParams;
        engineParams["IPAddress"] = "203.230.120.125";
        engineParams["IPAddress"] = "127.0.0.1";
        engineParams["Port"] = std::to_string(10000+i);
        engineParams["Monitor"] = "true";
        engineParams["TransportMode"] = transport_method;
        engineParams["Threading"] = threading;
        engineParams["Verbose"] = "0";
        engineParams["FloatAccuracy"] = accuracy_string;
        if(combining_steps > 50)
        {
            engineParams["CombiningSteps"] = std::to_string(combining_steps-50);
        }

        for(const auto &i : engineParams)
        {
            std::cout << i.first << ": " << i.second << std::endl;
        }
        std::cout << "variable size: " << variable_size << std::endl;

        adios2::ADIOS adios;
        adios2::IO io = adios.DeclareIO("TestIO");
        io.SetEngine("DataMan");
        io.SetParameters(engineParams);

        auto varFloats = io.DefineVariable<double>("myfloats", shape, start, count);
        if(compression_dist >= 250 && compression_dist < 500)
        {
            adios2::Operator zfpOp = adios.DefineOperator("zfpCompressor", adios2::ops::LossyZFP);
            std::string accuracy_string = "0.";
            for(int i=0; i<compression_accuracy; ++i)
            {
                accuracy_string += "0";
            }
            accuracy_string += "1";
            varFloats.AddOperation(zfpOp, {{adios2::ops::zfp::key::accuracy, accuracy_string}});
            std::cout << "compression: zfp " << accuracy_string << std::endl;
        }
        else if(compression_dist >= 500 && compression_dist < 750)
        {
            adios2::Operator mgardOp = adios.DefineOperator("mgardCompressor", adios2::ops::LossyMGARD);
            std::string accuracy_string = "0.";
            for(int i=0; i<compression_accuracy; ++i)
            {
                accuracy_string += "0";
            }
            accuracy_string += "1";
            varFloats.AddOperation(mgardOp, {{adios2::ops::mgard::key::accuracy, accuracy_string}});
            std::cout << "compression: mgard " << accuracy_string << std::endl;
        }
        else if(compression_dist >= 750)
        {
            adios2::Operator szOp = adios.DefineOperator("szCompressor", adios2::ops::LossySZ);
            std::string accuracy_string = "0.";
            for(int i=0; i<compression_accuracy; ++i)
            {
                accuracy_string += "0";
            }
            accuracy_string += "1";
            varFloats.AddOperation(szOp, {{adios2::ops::sz::key::accuracy, accuracy_string}});
            std::cout << "compression: sz " << accuracy_string << std::endl;
        }

        adios2::Engine engine = io.Open("TrainingData", adios2::Mode::Write);

        for(size_t i=0; i<1000; i++)
        {
            engine.BeginStep();
            engine.Put<double>(varFloats, myFloats.data());
            engine.EndStep();
        }

        engine.Close();
    }

    return 0;
}



