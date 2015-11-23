
def process(input, output):
    LONGTITUDE = 121.42187 + 0.006259
    LATITUDE = 31.015759 + 0.006328
    GAP = 0.001
    L = 25
    W = 16

    cnt = [0 for x in range(801)]
    s = [0 for x in range(801)]
    t = [2 for x in range(801)]

    fin = open(input, 'r')
    fout = open(output, 'w')

    longi = 0.0
    lati = 0.0
    time = 0
    ss = 0

    for line in fin:
        longi, lati, time, ss, tp = line.split()
        longi = float(longi)
        lati = float(lati)
        time = int(time)
        ss = float(ss)
        tp = int(tp)
        index = int((lati - LATITUDE) / GAP) * L + int((longi - LONGTITUDE) / GAP)
        #print "%f %f %d %f" % (longi, lati, time, ss)

        if (index > 400 or index < 0) :
            continue
        s[index + time * L * W] = s[index + time * L * W] + ss
        cnt[index + time * L * W] = cnt[index + time * L * W] + 1
        if(tp < t[index]):
            t[index] = tp

    for index in range(0, 801):
        #print "cnt[%d] = %d" % (index, cnt[index])
        if (cnt[index] != 0):
            if index >= L*W:
                print >> fout, "%d %d %f %d" % (index - L*W, 1, s[index] / cnt[index], t[index - L*W])
            else:
                print >> fout, "%d %d %f %d" % (index, 0, s[index] / cnt[index], t[index])
process("in.txt", "data.txt")

