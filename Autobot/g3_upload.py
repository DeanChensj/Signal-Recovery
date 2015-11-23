#coding:utf-8
import leancloud, random, time
import urllib2, urllib, json
from leancloud import Object, Query, User

ISOTIMEFORMAT='%Y%m%d%H%M%S'

leancloud.init('2acd2q062gt9fc0cppsdqyozhxriq7ymtaev99tj9806lps2', 
	'00uobniq5vwm5riykfbz0e5vzxfn1ekozu3pjc5pi378ye5h')

ak = 'I0Br2yF1vx8pvZwRKzOO34wu'

class G3Data(Object):

    def is_cheated(self):
        return self.get('cheatMode')

def gecodeAddress(ak, lati, longi):
    pre = "http://api.map.baidu.com/geocoder/v2/?ak="
    suffix = "&output=json"
    url = "%s%s&location=%f,%f%s" % (pre, ak, lati, longi, suffix)
    req = urllib2.urlopen(url)
 
    web = req.read()
    content = json.loads(web)

    if (content['status']):
        address = 'good game'
    else:
        result = content['result']
        address = result['formatted_address'] + result['sematic_description']
    return address

def getIndex(lati, longi):
    LONGITUDE = 121.42187;
    LATITUDE = 31.015759;

    #经度方向距离原点差几个格子
    x = (longi - LONGITUDE) / 0.00278
    #纬度方向距离原点差几个格子
    y = (lati - LATITUDE) / 0.00278

    nindex = x + 15 * y
    return nindex

def uploadG3(results):
    g3Data = G3Data()

    count = len(results)
    phoneNumber = results[random.randint(0, count - 1)].get('username')

    g3Data.set('phonenumber', phoneNumber)
    g3Data.set('isGsm', False)          #是否GSM信号 2G or 3G 
    g3Data.set("CdmaDbm", random.randint(-90, -45))            #联通3G 信号强度
    g3Data.set("CdmaEcio", random.randint(-160, -1))            #联通3G 载干比
    g3Data.set("EvdoDbm", -120)             #电信3G 信号强度
    g3Data.set("EvdoEcio", random.randint(-90, -1))            #电信3G 载干比
    g3Data.set("GsmSignalStrength", -1)   #2g
    g3Data.set("GsmBitErrorRate", 99)     #2G 误码率
    g3Data.set("timeStamp", time.strftime(ISOTIMEFORMAT, time.localtime( time.time())))

    lati = round(random.uniform(31.04, 31.33), 6) 
    longi = round(random.uniform(121.21, 121.53), 6)
    addr = gecodeAddress(ak, lati, longi)

    if(addr.strip()==''):
        print '地址不明'
        return

    g3Data.set("index", round(getIndex(lati, longi)))
    g3Data.set("longitude", longi)
    g3Data.set("latitude", lati)
    g3Data.set("address", addr)

    g3Data.save()

def uploadG3s():
    query = Query(User) 
    query.exists("username")
    results = query.find()

    for i in range (0, 10):
        uploadG3(results)  

uploadG3s()