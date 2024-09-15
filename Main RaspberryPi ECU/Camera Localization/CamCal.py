import numpy as np
import cv2 as cv
import glob

criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, 30, 0.001)

rows = 5
columns = 8
world_scaling = 3.65    # Cm

objp = np.zeros((rows*columns, 3), np.float32)
objp[:, :2] = np.mgrid[0:rows, 0:columns].T.reshape(-1, 2)
objp = world_scaling*objp

objpoints = []
imgpoints = []

images = glob.glob('*.jpg')
height, width = (0, 0)

for fname in images:
    img = cv.imread(fname)
    gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
    height, width = gray.shape

    ret, corners = cv.findChessboardCorners(gray, (rows, columns), None)

    if ret:
        conv_size = (11, 11)
        corners = cv.cornerSubPix(gray, corners, conv_size, (-1, -1), criteria)
        cv.drawChessboardCorners(img, (rows, columns), corners, ret)
        cv.imshow('img', img)
        cv.waitKey(5000)

        objpoints.append(objp)
        imgpoints.append(corners)
cv.destroyAllWindows()

ret, mtx, dist, rvecs, tvecs = cv.calibrateCamera(objpoints, imgpoints, (width, height), None, None)
print(ret)
print(mtx)
print(dist)
print(rvecs)
print(tvecs)
