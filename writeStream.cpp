#include <iostream>
#include <vector>
#include <random>
#include <adios2.h>


int main(int argc, char *argv[])
{

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> variable_size_distrib(100, 10000000);
    std::uniform_int_distribution<> bool_distrib(0, 1000);
    std::uniform_int_distribution<> combining_steps_distrib(2, 100);

    size_t variable_size = 10000000;
//    size_t variable_size = variable_size_distrib(gen);

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
    engineParams["Port"] = "50000";
    engineParams["Monitor"] = "true";
    engineParams["TransportMode"] = transport_method;
    engineParams["Threading"] = threading;
    engineParams["CombiningSteps"] = std::to_string(combining_steps);

    for(const auto &i : engineParams)
    {
        std::cout << i.first << ": " << i.second << std::endl;
    }

    adios2::ADIOS adios;
    adios2::IO io = adios.DeclareIO("TestIO");
    io.SetEngine("DataMan");
    io.SetParameters(engineParams);

    auto varFloats = io.DefineVariable<float>("myfloats", shape, start, count);

    adios2::Engine engine = io.Open("Test", adios2::Mode::Write);

    for(size_t i=0; i<1000; i++)
    {
        engine.BeginStep();
        engine.Put<float>(varFloats, myFloats.data());
        engine.EndStep();
    }

    engine.Close();

    return 0;
}



