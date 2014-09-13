from math import *
from numpy import *
import operator
import itertools

#http://www.freemaptools.com/radius-around-point.htm
earthR = 6371

class Point:
  def __init__(self, x, y, z = 0):
    self.x = x
    self.y = y
    self.z = z
  def __str__(self):
    return 'P({self.x}, {self.y}, {self.z})'.format(self=self)
  def arr(self):
    return array([self.x, self.y, self.z])

class Coordinate:
  def __init__(self, lat, lon):
    self.lat = lat
    self.lon = lon
  def __str__(self):
    return 'L({self.lat}, {self.lon})'.format(self=self)
  def arr(self):
    return array([self.lat, self.lon])

class Circle:
  def __init__(self, cd, rad):
    self.coordinate = cd
    self.radious = rad
  def __str__(self):
    return 'O({self.coordinate}: {self.radious})'.format(self=self)
  def contains(self, cd):
    lat = self.coordinate.lat
    lon = self.coordinate.lon
    if ((lat - cd.lat)*(lat - cd.lat) + (lon - cd.lon)*(lon - cd.lon) <= self.radious * self.radious):
      return True
    return False

def coordinateToPoint(cd):
  x = earthR *(math.cos(math.radians(cd.lat)) * math.cos(math.radians(cd.lon)))
  y = earthR *(math.cos(math.radians(cd.lat)) * math.sin(math.radians(cd.lon)))
  z = earthR *(math.sin(math.radians(cd.lat)))
  return Point(x,y,z)

#assuming elevation = 0
def trilateration(circleA, circleB, circleC):
  #using authalic sphere
  #if using an ellipsoid this step is slightly different
  #Convert geodetic Lat/Long to ECEF xyz
  #   1. Convert Lat/Long to radians
  #   2. Convert Lat/Long(radians) to ECEF
  PA = coordinateToPoint(circleA.coordinate)
  PB = coordinateToPoint(circleB.coordinate)
  PC = coordinateToPoint(circleC.coordinate)

  #from wikipedia
  #transform to get circle 1 at origin
  #transform to get circle 2 on x axis
  P1 = PA.arr()
  P2 = PB.arr()
  P3 = PC.arr()

  ex = (P2 - P1)/(linalg.norm(P2 - P1))
  i = dot(ex, P3 - P1)
  ey = (P3 - P1 - i*ex)/(linalg.norm(P3 - P1 - i*ex))
  ez = cross(ex,ey)
  d = linalg.norm(P2 - P1)
  j = dot(ey, P3 - P1)

  DistA = circleA.radious
  DistB = circleB.radious
  DistC = circleC.radious
  x = (pow(DistA,2) - pow(DistB,2) + pow(d,2))/(2*d)
  y = ((pow(DistA,2) - pow(DistC,2) + pow(i,2) + pow(j,2))/(2*j)) - ((i*x)/j)
  
  z = sqrt(pow(DistA,2) - pow(x,2) - pow(y,2))
  
  #triPt is an array with ECEF x,y,z of trilateration point
  triPt = P1 + x*ex + y*ey + z*ez
  
  #convert back to lat/long from ECEF
  #convert to degrees
  lat = math.degrees(math.asin(triPt[2] / earthR))
  lon = math.degrees(math.atan2(triPt[1],triPt[0]))
  return Coordinate(lat, lon) 

def multilateration(circleList, claimedLocation):
  threshold = 3*3
  continentRad = 20
  def distance(circle):
    return linalg.norm(circle.coordinate.arr() - claimedLocation.arr())
  
  # Sort the circleList based on the distance from the claimed location
  print "List of Circles:"
  for i in circleList:
    print i,
  print "\nLocation: ", claimedLocation
  circleList.sort(key=distance)

  print "Sorted List of Circles:"
  for i in circleList:
    print i,
  print "\n"
  # delete after a threshold
  circleList = circleList[0:threshold]
  #get all the 3 combinations of the remaining list
  combin3 = itertools.combinations(circleList, 3) #TODO
  # iterate through all 3 circles and calculate the center of them.
  resCD = []
  print "3 Combinations of Circles:"
  for circ3 in combin3:
    print "combination:", circ3[0], circ3[1], circ3[2]
    cd = trilateration(circ3[0], circ3[1], circ3[2]) 
    print "Trilateration output center point:", cd
    resCD.append(cd)
  
  # calculate the result
  cdCount = {}
  print "\nRegions and the number of points inside (radious = 20 in latitude/longitude):"
  for cd in resCD:
    cdCount[cd] = 0
    # make a circle around cd 
    circleCD = Circle(cd, continentRad)
    # calculate the number of points in cd
    for cdnt in resCD:
      if circleCD.contains(cdnt):
        cdCount[cd] += 1
    print circleCD, cdCount[cd]
  
  # sort to find the cd with maximum number of points
  cdCountSortedDesc = sorted(cdCount.iteritems(), key=operator.itemgetter(1))
  cdCountSortedDesc.reverse()
  # return the cd
  return cdCountSortedDesc[0][0]

def main():
  f = open('input', 'r')
  ln = 0
  landmarks = []
  for line in f:
    if line[0] == "#":
      continue
    numbers = line.split(',')
    cd = Coordinate(float(numbers[0]), float(numbers[1]))
    if (ln == 0):
      claimed = cd
    else:
      landmarks.append(Circle(cd, float(numbers[2])))
    ln = ln + 1

  cntCD = multilateration(landmarks, claimed)

  print "\nSOLUTION:"
  print "lat: ", cntCD.lat, "\t lon: ", cntCD.lon

main()
