import cv2
import numpy as np
import math

height = 1080

with open('dat/road.dat', 'r') as f:
	data = f.readlines()
points = []
xmin, ymin = 1e+10, 1e+10
xmax, ymax = -1e+10, -1e+10
for item in data:

	item = item.split(' ')
	# item[3] = item[3][:-2]

	item[0] = (float)(item[0])
	item[1] = (float)(item[1])
	# item[2] = (float)(item[2])
	# item[3] = (float)(item[3])

	if item[0] < xmin:
		xmin = item[0]
	# if item[2] < xmin:
		# xmin = item[2]
	if item[1] < ymin:
		ymin = item[1]
	# if item[3] < ymin:
		# ymin = item[3]

	if item[0] > xmax:
		xmax = item[0]
	# if item[2] > xmax:
		# xmax = item[2]
	if item[1] > ymax:
		ymax = item[1]
	# if item[3] > ymax:
		# ymax = item[3]
	points.append((item[0], item[1]))
	# points.append((item[2], item[3]))

print xmin,xmax
print ymin,ymax

# xmax = -73.7
# xmin = -74.1
# ymax = 41.0
# ymin = 40.5

width = (int)(height * (ymax - ymin) / (xmax - xmin))
img = np.zeros((height, width, 3), np.uint8)
for item in points:
	x = height - (int)((item[0] - xmin) / (xmax - xmin) * height)
	y = (int)((item[1] - ymin) / (ymax - ymin) * width)
	cv2.circle(img,(y,x),1,(0,0,255),-1)

# cv2.namedWindow('map')
# cv2.imshow('map', img)
# while True:
# 	key = cv2.waitKey(1)
# 	if key == ord('q'):
# 		break
# cv2.destroyAllWindows()
cv2.imwrite('./images/road.png', img)