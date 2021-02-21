#include <iostream>
#include <numeric>
#include <vector>
#include "adios2.h"

int main(int argc, char **argv)
{

    for(int i=0; i<9999; ++i)
    {
        std::vector<double> myFloats;
        bool initialStep = true;

        adios2::Params engineParams;
        engineParams["IPAddress"] = "127.0.0.1";
        engineParams["Port"] = std::to_string(10000+i);
        engineParams["Monitor"] = "true";
        engineParams["Timeout"] = "10";
        engineParams["MaxStepBufferSize"] = "1100000000";

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



