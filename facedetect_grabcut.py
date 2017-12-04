#!/usr/bin/env python3

'''
face detection using haar cascades

USAGE:
    facedetect.py [--cascade <cascade_fn>] [--nested-cascade <cascade_fn>] [<video_source>]
'''

# Python 2/3 compatibility
from __future__ import print_function, division

import numpy as np
import cv2

# local modules
from video import create_capture
from common import clock, draw_str


def detect(img, cascade):
    rects = cascade.detectMultiScale(img, scaleFactor=1.3, minNeighbors=4,
                            minSize=(30, 30), flags=cv2.CASCADE_SCALE_IMAGE)
    if len(rects) == 0:
        return rects

    rects = rects[np.argsort(rects[:, 2])]
    rects[:,2:] += rects[:,:2]
    xc = (rects[:, 0] + rects[:, 2]) / 2
    yc = (rects[:, 1] + rects[:, 3]) / 2
    rects[:, 0] = xc - 1.3 * (xc - rects[:, 0])
    rects[:, 1] = yc - 1.6 * (yc - rects[:, 1])
    rects[:, 2] = xc + 1.3 * (rects[:, 2] - xc)
    rects[:, 3] = yc + 1.8 * (rects[:, 3] - yc)
    
    return rects

def draw_rects(img, rects, color):
    for x1, y1, x2, y2 in rects:
        cv2.rectangle(img, (x1, y1), (x2, y2), color, 2)

if __name__ == '__main__':
    import sys, getopt
    print(__doc__)

    args, video_src = getopt.getopt(sys.argv[1:], '', ['cascade=', 'nested-cascade='])
    try:
        video_src = video_src[0]
    except:
        video_src = 0
    args = dict(args)
    cascade_fn = args.get('--cascade', "C:/opencv/opencv/data/haarcascades/haarcascade_frontalface_alt.xml")
    # nested_fn  = args.get('--nested-cascade', "C:/opencv/opencv/data/haarcascades/haarcascade_eye.xml")

    cascade = cv2.CascadeClassifier(cascade_fn)
    # nested = cv2.CascadeClassifier(nested_fn)

    cam = create_capture(video_src, fallback='lena')

    while True:
        ret, img = cam.read()
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        gray = cv2.equalizeHist(gray)

        t = clock()
        rects = detect(gray, cascade)

        if len(rects):
            rect = tuple(rects[0]//2)
            img2 = cv2.pyrDown(img)
            mask = np.zeros(img2.shape[:2],np.uint8)
            bgdModel = np.zeros((1,65),np.float64)
            fgdModel = np.zeros((1,65),np.float64)
            cv2.grabCut(img2, mask, rect, bgdModel, fgdModel, 1,
                        cv2.GC_INIT_WITH_RECT)
            mask2 = (mask==1)|(mask==3)
            img *= mask2.repeat(2, 0).repeat(2, 1)[:,:,np.newaxis]

        draw_rects(img, rects, (0, 255, 0))
        # if not nested.empty():
        #     for x1, y1, x2, y2 in rects:
        #         roi = gray[y1:y2, x1:x2]
        #         vis_roi = vis[y1:y2, x1:x2]
        #         subrects = detect(roi.copy(), nested)
        #         draw_rects(vis_roi, subrects, (255, 0, 0))
        dt = clock() - t

        draw_str(img, (20, 20), 'time: %.1f ms' % (dt*1000))
        cv2.imshow('facedetect', img)

        k = cv2.waitKey(5)
        if k == 27:
            break
    cv2.destroyAllWindows()
