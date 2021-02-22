import json

f = open('TrainingData.json', 'r')
lines = f.readlines()

dataItemList = []

for line in lines:
    try:
        oneDataItem = json.loads(line)
        dataItemList.append(oneDataItem)
    except:
        pass


for i in dataItemList:
    print(i)



