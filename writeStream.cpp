#include <adios2.h>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>


int main(int argc, char *argv[])
{

    int port=10000;

    if(argc>1)
    {
        port = std::stoi(argv[1]);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> bool_distrib(0, 1000);
    std::uniform_int_distribution<> variable_size_distrib(10, 10000);
    std::uniform_int_distribution<> combining_steps_distrib(1, 100);
    std::uniform_int_distribution<> accuracy_distrib(0, 5);

    for(int i=0; i<1; ++i)
    {
        size_t variable_size = variable_size_distrib(gen);

        int compression_dist = bool_distrib(gen);
        bool compression;
        if(compression_dist < 800)
        {
            compression = true;
        }
        else
        {
            compression = false;
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


        int compression_accuracy = accuracy_distrib(gen);
        int required_accuracy = accuracy_distrib(gen);
        while (required_accuracy > compression_accuracy)
        {
            compression_accuracy = accuracy_distrib(gen);
        }
        std::string required_accuracy_string = "0.";
        for(int i=0; i<required_accuracy; ++i)
        {
            required_accuracy_string += "0";
        }
        required_accuracy_string += "1";

        std::string compression_accuracy_string = "0.";
        for(int i=0; i<compression_accuracy; ++i)
        {
            compression_accuracy_string += "0";
        }
        compression_accuracy_string += "1";

        int combining_steps = combining_steps_distrib(gen);

        std::vector<double> myFloats(2*variable_size);
        for(size_t i=0; i<myFloats.size(); i++)
        {
            myFloats[i]=i;
        }

        adios2::Dims shape({2, variable_size});
        adios2::Dims start({0, 0});
        adios2::Dims count({2, variable_size});

        adios2::Params engineParams;
        engineParams["IPAddress"] = "203.230.120.125";
//        engineParams["IPAddress"] = "127.0.0.1";
        engineParams["Port"] = std::to_string(port+i);
        engineParams["Monitor"] = "true";
        engineParams["TransportMode"] = "fast";
        engineParams["Threading"] = threading;
        engineParams["Verbose"] = "10";
        engineParams["FloatAccuracy"] = required_accuracy_string;
        engineParams["CombiningSteps"] = std::to_string(combining_steps);

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

        adios2::Operator szOp = adios.DefineOperator("szCompressor", adios2::ops::LossySZ);
        if(compression)
        {
            varFloats.AddOperation(szOp, {{adios2::ops::sz::key::accuracy, compression_accuracy_string}});
            std::cout << "compression: sz " << compression_accuracy_string << std::endl;
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



