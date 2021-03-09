import json

inputlist = ["Bandwidth", "Latency", "DropRate", "AllowedError", "StepDataSize"]
outputlist = ["CombiningSteps", "Transport", "WriterThreading", "ReaderThreading", "CompressionMethod", "CompressionAccuracy"]

dictionary = {}
dictionary[""] = 0
dictionary[None] = 0
dictionary["zfp"] = 1
dictionary["sz"] = 2
dictionary["mgard"] = 3
dictionary["fast"] = 1
dictionary["reliable"] = 2
dictionary[False] = 0
dictionary[True] = 1

fin = open('TrainingData.json', 'r')
lines = fin.readlines()

fout1 = open('TrainingDataInput.csv', 'w')
fout2 = open('TrainingDataOutput.csv', 'w')

for i in inputlist:
    fout1.write(i)
    fout1.write(", ")
fout1.write("\n")

for i in outputlist:
    fout2.write(i)
    fout2.write(", ")
fout2.write("\n")

for line in lines:

    oneDataItem = json.loads(line)
    if oneDataItem["Performance"]["Bandwidth"] == 0.0:
        continue

    for i in outputlist:
        v = None
        try:
            v = oneDataItem["Decisions"][i]
        except:
            v = None

        try:
            v = dictionary[v]
        except:
            pass

        fout2.write(str(v))
        fout2.write(", ")
    fout2.write("\n")

    for i in inputlist:
        fout1.write(str(oneDataItem["Performance"][i]))
        fout1.write(", ")
    fout1.write("\n")


