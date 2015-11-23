#coding:utf-8
import leancloud, random
from leancloud import User, Query

leancloud.init('2acd2q062gt9fc0cppsdqyozhxriq7ymtaev99tj9806lps2', 
   master_key='xxna70h7xpeys4b6bcrfoqu6odph34rckwo67ecxtyscj5ms')

def genPhoneNum():
  s1 = random.choice(('13', '15', '18'))
  n2 = random.randint(0, 9999)
  n3 = random.randint(0, 99999)

  s2 = "%04d" % n2
  s3 = "%05d" % n3
  number = s1 + s2 + s3
  return number

def genID():
  n = [0, 0, 0]
  s = []
  ID = ""
  for i in range (0, 3):
    n[i] = random.randint(0, 99999)
    s.append("%05d" % n[i])
    ID += s[i]
  return ID

def uploadUser():
  username = genPhoneNum()
  deviceId = genID()
  providerName = random.choice(("中国移动", "中国联通", "中国电信"))
  model = random.choice(('MX4 Pro', 'MI 4 LTE', 'SM-G9009W'))
  sdkVersion = random.choice(('19', '17', '21'))
  releaseVersion = random.choice(('4.3', '4.4.4', '4.1.2'))

  user = User()
  query = Query(User) 
  query.equal_to('username', username)

  if(query.find()):
    username = genPhoneNum()

  user.set("username", username)
  user.set("password", "123456")
  user.set("deviceId", deviceId)
  user.set("providerName", providerName)
  user.set("model", model)
  user.set("sdkVersion", sdkVersion)
  user.set("releaseVersion", releaseVersion)
  user.set("value", 0)
  user.sign_up()

  user.set("mobilePhoneNumber", username)
  user.save()

def uploadUsers(n):
  for i in range (0, n):
    uploadUser()
  print "%d user has been created." % n

uploadUser()


