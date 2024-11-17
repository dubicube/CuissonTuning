# RX: E7   RX: BF   RX: 2F
# RX: 7D   RX: 7D   RX: 5B
# RX: FF   RX: FF   RX: A9
# RX: 07   RX: 07   RX: BF
# RX: F1   RX: F1   RX: E7
# RX: 3F   RX: 3F   RX: 7D
# RX: 0D   RX: 0D   RX: FF
# RX: 1F   RX: 1F   RX: 07
# RX: 2F   RX: 2F   RX: F1
# RX: 5B   RX: 5B   RX: 3F
# RX: A9   RX: A9   RX: 0D
# RX: BF   RX: BF   RX: 1F
# RX: E7   RX: E7   RX: 2F
# RX: 7D   RX: 7D   RX: 5B
# RX: FF   RX: FF   RX: A9
# RX: F1   RX: F1   RX: BF
# RX: 3F            RX: E7
# RX: 0D            RX: 7D
# RX: 1F


# RX: 01 D4
# RX: 01 37
# RX: 0A 3C FF 00 00 00 00 00 00 00 00
# RX: 01 BE
# RX: 04 01 80 00 7F
# RX: 04 83 50 00 AF
# RX: 08 F8 00 00 00 00 00 00 FF
# RX: 08 04 00 00 00 00 00 00 FF
# RX: 08 86 00 00 00 00 00 00 FF
# RX: 08 47 00 00 00 00 00 00 FF
# RX: 0A 97 00 02 00 00 00 14 18 89 B7
# RX: 0A 56 00 02 00 00 00 38 70 8E 24
# RX: 01 D4
# RX: 01 37
# RX: 0A 3C FF 00 00 00 00 00 00 00 00
# RX: 01 BE
# RX: 04 01 80 00 7F
# RX: 08 F8 00 00 00 00 00 00 FF




step = (64.0/16000000)*1e+9


fr = open('dump10', 'r')
# data = [[int(i, 16)>>15, int(i, 16)&0x7FFF] for i in fr.read().split('\n')]
data = fr.read().split('\n')
fr.close()

data = [int(i, 16) for i in data]
# data = [[int(i, 16)&0x7FFF, int(i, 16)>>15] for i in data]


data2 = data[256:]
data = data[:256]
# values = [i[1] for i in data]
# print(''.join([str(i) for i in values]))
# data = [i[0] for i in data]

# prev = data[0][1]
# incr = 0
# for i in range(1, len(data)):
#     if data[i][1] < prev:
#         incr+=0x8000
#     prev = data[i][1]
#     data[i][1] = data[i][1] + incr

prev = data[0]
incr = 0
for i in range(1, len(data)):
    if data[i] < prev:
        incr+=0x10000
    prev = data[i]
    data[i] = data[i] + incr
data = [i*step for i in data]


prev = data2[0]
incr = 0
for i in range(1, len(data2)):
    if data2[i] < prev:
        incr+=0x10000
    prev = data2[i]
    data2[i] = data2[i] + incr
data2 = [i*step for i in data2]




t0 = data[0]
data = [i-t0 for i in data]

data2 = [i-t0 for i in data2]


# sample = [i[1]*step for i in data if i[0] == 1]
# data = [i[1]*step for i in data if i[0] == 0]



# trames = [[i for i in data], [i for i in data2]]
trames = [[i for i in data], [data[0]]]

for i in range(1, len(data)):

    if data[i]-data[i-1] > 12000000:
        trames += [[]]

    trames[-1] += [data[i]]


for i in range(len(trames)):
    t0 = trames[i][0]
    trames[i] = [i-t0 for i in trames[i]]


# 10ms
for i in range(len(trames)):
    if len(trames[i]) > 2:
        tt = trames[i][2]
        trames[i] = [j-tt+10000000 for j in trames[i]]



f = open('test2.vcd', 'w')


f.write("""$date
	Wed Nov 13 00:00:00 2024
$end
$version
	Icarus Verilog
$end
$timescale
	1ns
$end
""")

f.write("""$scope module truc $end\n""")
for i in range(len(trames)):
    f.write('$var wire 1 ' + chr(33+i) + ' trame' + str(i) + ' $end\n')
f.write("""$upscope $end
$enddefinitions $end
""")


f.write('#0\n$dumpvars\n')
for i in range(len(trames)):
    f.write('b0 ' + chr(33+i) + '\n')
f.write('$end\n')


indexs = [0]*len(trames)
while sum(indexs) < sum([len(i) for i in trames]):
    itrame = 0
    while indexs[itrame] >= len(trames[itrame]):
        itrame+=1
    for i in range(itrame, len(trames)):
        if indexs[i] < len(trames[i]) and trames[i][indexs[i]] < trames[itrame][indexs[itrame]]:
            itrame = i

    f.write('#' + str(trames[itrame][indexs[itrame]]) + '\nb' + str((indexs[itrame]&1)) + ' ' + chr(33+itrame) + '\n')
    indexs[itrame]+=1



f.close()