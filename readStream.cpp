#include <iostream>
#include <numeric>
#include <vector>
#include "adios2.h"

int main(int argc, char **argv)
{

    std::vector<float> myFloats;
    bool initialStep = true;

    adios2::Params engineParams;
    engineParams["IPAddress"] = "127.0.0.1";
    engineParams["Port"] = "50000";
    engineParams["Monitor"] = "true";
    engineParams["MaxStepBufferSize"] = "1100000000";

    adios2::ADIOS adios;
    adios2::IO io = adios.DeclareIO("TestIO");
    io.SetEngine("DataMan");
    io.SetParameters(engineParams);

    adios2::Engine engine = io.Open("Test", adios2::Mode::Read);

    while(true)
    {
        auto status = engine.BeginStep();
        if(status == adios2::StepStatus::EndOfStream)
        {
            break;
        }
        auto varFloats = io.InquireVariable<float>("myfloats");
        if(initialStep)
        {
            auto shape = varFloats.Shape();
            myFloats.resize(std::accumulate(shape.begin(), shape.end(), sizeof(float), std::multiplies<size_t>()));
            initialStep = false;
        }

        engine.Get<float>(varFloats, myFloats.data());
        engine.EndStep();

    }

    engine.Close();
    return 0;
}



