#include <adios2.h>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

int main(int argc, char **argv)
{

    for(int i=0; i<9999; ++i)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> bool_distrib(0, 1000);
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

        std::vector<double> myFloats;
        bool initialStep = true;

        adios2::Params engineParams;
        engineParams["IPAddress"] = "203.230.120.125";
        engineParams["IPAddress"] = "127.0.0.1";
        engineParams["Port"] = std::to_string(10000+i);
        engineParams["Monitor"] = "true";
        engineParams["Timeout"] = "30";
        engineParams["Verbose"] = "10";
        engineParams["Threading"] = threading;
        engineParams["MaxStepBufferSize"] = "1100000000";

        for(const auto &i : engineParams)
        {
            std::cout << i.first << ": " << i.second << std::endl;
        }

        adios2::ADIOS adios;
        adios2::IO io = adios.DeclareIO("TestIO");
        io.SetEngine("DataMan");
        io.SetParameters(engineParams);

        adios2::Engine engine = io.Open("TrainingData", adios2::Mode::Read);

        while(true)
        {
            auto status = engine.BeginStep();
            if(status == adios2::StepStatus::EndOfStream)
            {
                break;
            }
            auto varFloats = io.InquireVariable<double>("myfloats");
            if(initialStep)
            {
                auto shape = varFloats.Shape();
                myFloats.resize(std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<size_t>()));
                initialStep = false;
            }
            engine.Get<double>(varFloats, myFloats.data());
            engine.EndStep();
        }
        engine.Close();
    }
    return 0;
}



