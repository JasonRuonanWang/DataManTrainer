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
    std::uniform_int_distribution<> variable_size_distrib(100, 2500000);
    std::uniform_int_distribution<> combining_steps_distrib(1, 100);
    std::uniform_int_distribution<> accuracy_distrib(0, 5);

    for(int i=0; i<9999; ++i)
    {
        size_t variable_size = variable_size_distrib(gen);

        int transport_dist = bool_distrib(gen);
        std::string transport_method;
        if(transport_dist < 500)
        {
            transport_method = "reliable";
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

        std::vector<float> myFloats(variable_size);
        for(size_t i=0; i<myFloats.size(); i++)
        {
            myFloats[i]=i;
        }

        adios2::Dims shape({variable_size});
        adios2::Dims start({0});
        adios2::Dims count({variable_size});

        adios2::Params engineParams;
        engineParams["IPAddress"] = "127.0.0.1";
        engineParams["Port"] = std::to_string(10000+i);
        engineParams["Monitor"] = "true";
        engineParams["TransportMode"] = transport_method;
        engineParams["Threading"] = threading;
        engineParams["FloatAccuracy"] = accuracy_string;
        if(combining_steps > 50)
        {
            engineParams["CombiningSteps"] = std::to_string(combining_steps-50);
        }

        for(const auto &i : engineParams)
        {
            std::cout << i.first << ": " << i.second << std::endl;
        }

        adios2::ADIOS adios;
        adios2::IO io = adios.DeclareIO("TestIO");
        io.SetEngine("DataMan");
        io.SetParameters(engineParams);

        auto varFloats = io.DefineVariable<float>("myfloats", shape, start, count);
        if(compression_dist < 500)
        {
            adios2::Operator zfpOp = adios.DefineOperator("zfpCompressor", adios2::ops::LossyZFP);
            std::string accuracy_string = "0.";
            for(int i=0; i<compression_accuracy; ++i)
            {
                accuracy_string += "0";
            }
            accuracy_string += "1";
            varFloats.AddOperation(zfpOp, {{adios2::ops::zfp::key::accuracy, accuracy_string}});
        }

        adios2::Engine engine = io.Open("TrainingData", adios2::Mode::Write);

        for(size_t i=0; i<1000; i++)
        {
            engine.BeginStep();
            engine.Put<float>(varFloats, myFloats.data());
            engine.EndStep();
        }

        engine.Close();
    }

    return 0;
}



