# extract_letters.py
# Extracts the individual letters into a template format.

import cv2

TEMPLATE_SIDE_LENGTH = 64

img = cv2.imread("../data/letters.png")
if img is None:
  exit(0)
canny = cv2.Canny(img, 100, 100)
contours, h = cv2.findContours(canny, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
letters = []
i: int = 1
for contour in contours:
  x, y, w, h = cv2.boundingRect(contour)
  crop = img[y:y+h, x:x+w]
  rsz_crop = cv2.resize(crop, (TEMPLATE_SIDE_LENGTH, TEMPLATE_SIDE_LENGTH))
  cv2.imwrite(f"../data/{i}.png", rsz_crop)
  i += 1

